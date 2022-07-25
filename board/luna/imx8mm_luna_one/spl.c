// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright 2019, 2021 NXP
 */

#include <common.h>
#include <command.h>
#include <cpu_func.h>
#include <hang.h>
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

	/* DDR initialization */
	spl_dram_init();

	board_init_r(NULL, 0);
}
