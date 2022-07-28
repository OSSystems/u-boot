// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright 2019, 2021 NXP
 */

#include <common.h>
#include <command.h>
#include <cpu_func.h>
#include <hang.h>
#include <i2c.h>
#include <image.h>
#include <init.h>
#include <log.h>
#include <spl.h>
#include <asm/global_data.h>
#include <asm/io.h>
#include <asm/mach-imx/iomux-v3.h>
#include <asm/arch/clock.h>
#include <asm/arch/imx8mm_pins.h>
#include <asm/arch/sys_proto.h>
#include <asm/mach-imx/boot_mode.h>
#include <asm/arch/ddr.h>

#include <dm/uclass.h>
#include <dm/device.h>
#include <dm/uclass-internal.h>
#include <dm/device-internal.h>

#include <power/bd71837.h>

DECLARE_GLOBAL_DATA_PTR;

int spl_board_boot_device(enum boot_device boot_dev_spl)
{
	switch (boot_dev_spl) {
	case USB_BOOT:
		return BOOT_DEVICE_BOARD;
	case SD2_BOOT:
	case MMC2_BOOT:
		return BOOT_DEVICE_MMC1;
	case SD3_BOOT:
	case MMC3_BOOT:
		return BOOT_DEVICE_MMC2;
	default:
		return BOOT_DEVICE_NONE;
	}
}

static void spl_dram_init(void)
{
	ddr_init(&dram_timing);
}

void spl_board_init(void)
{
	if (IS_ENABLED(CONFIG_FSL_CAAM)) {
		struct udevice *dev;
		int ret;

		ret = uclass_get_device_by_driver(UCLASS_MISC, DM_DRIVER_GET(caam_jr), &dev);
		if (ret)
			printf("Failed to initialize caam_jr: %d\n", ret);
	}
	puts("Normal Boot\n");
}

#ifdef CONFIG_SPL_LOAD_FIT
int board_fit_config_name_match(const char *name)
{
	/* Just empty function now - can't decide what to choose */
	debug("%s: %s\n", __func__, name);

	return 0;
}
#endif

static int power_init_board(void)
{
	struct udevice *bus;
	struct udevice *dev;
	int ret;

	ret = uclass_get_device_by_seq(UCLASS_I2C, 0, &bus);

	ret = dm_i2c_probe(bus, 0x4b, 0, &dev);
	if (ret) {
		printf("PMIC: failed probe: %d\n", ret);
		return ret;
	}
	puts("PMIC:  BD71847\n");

	/* unlock the PMIC regs */
	dm_i2c_reg_write(dev, BD718XX_REGLOCK, 0x1);
	/* increase VDD_0P95 (VDD_GPU/VPU/DRAM) to 0.975V for 1.5GHz DDR */
	dm_i2c_reg_write(dev, BD718XX_1ST_NODVS_BUCK_VOLT, 0x83);
	/* increase VDD_SOC to 0.85V before first DRAM access */
	dm_i2c_reg_write(dev, BD718XX_BUCK1_VOLT_RUN, 0x0f);
	/* increase VDD_ARM to 0.92V for 800MHz and 1600MHz */
	dm_i2c_reg_write(dev, BD718XX_BUCK2_VOLT_RUN, 0x16);
	/* Lock the PMIC regs */
	dm_i2c_reg_write(dev, BD718XX_REGLOCK, 0x11);

	return 0;
}

void board_init_f(ulong dummy)
{
	struct udevice *dev;
	int ret;

	arch_cpu_init();

	init_uart_clk(0);

	timer_init();

	/* Clear the BSS. */
	memset(__bss_start, 0, __bss_end - __bss_start);

	ret = spl_early_init();
	if (ret) {
		debug("spl_early_init() failed: %d\n", ret);
		hang();
	}

	ret = uclass_get_device_by_name(UCLASS_CLK,
					"clock-controller@30380000",
					&dev);
	if (ret < 0) {
		printf("Failed to find clock node. Check device tree\n");
		hang();
	}

	preloader_console_init();

	enable_tzc380();

	power_init_board();

	/* DDR initialization */
	spl_dram_init();

	board_init_r(NULL, 0);
}
