// SPDX-License-Identifier: GPL-2.0+
//
// Copyright (C) 2025 Fabio Estevam <festevam@gmail.com>

#include <asm/io.h>
#include <asm/arch/clock.h>
#include <asm/arch/imx-regs.h>
#include <asm/arch/crm_regs.h>
#include <asm/arch/iomux.h>
#include <asm/arch/mx6-pins.h>
#include <asm/mach-imx/iomux-v3.h>

#include <asm/mach-imx/spi.h>
#include <linux/errno.h>
#include <asm/gpio.h>
#include <asm/arch/sys_proto.h>
#include <asm/sections.h>
#include <linux/delay.h>

#include <power/pmic.h>
#include <power/pfuze100_pmic.h>

#include <image.h>
#include <init.h>
#include <serial.h>
#include <spl.h>
#include <linux/sizes.h>
#include <mmc.h>

DECLARE_GLOBAL_DATA_PTR;

#define UART_PAD_CTRL  (PAD_CTL_PUS_100K_UP |			\
	PAD_CTL_SPEED_MED | PAD_CTL_DSE_40ohm |			\
	PAD_CTL_SRE_FAST  | PAD_CTL_HYS)

#define SPI_PAD_CTRL (PAD_CTL_HYS | PAD_CTL_SPEED_MED | \
		      PAD_CTL_DSE_40ohm | PAD_CTL_SRE_FAST)

int dram_init(void)
{
	gd->ram_size = imx_ddr_size();
	return 0;
}

static const iomux_v3_cfg_t uart1_pads[] = {
	MX6_PAD_CSI0_DAT10__UART1_TX_DATA | MUX_PAD_CTRL(UART_PAD_CTRL),
	MX6_PAD_CSI0_DAT11__UART1_RX_DATA | MUX_PAD_CTRL(UART_PAD_CTRL),
};

static const iomux_v3_cfg_t ecspi1_pads[] = {
	MX6_PAD_EIM_D16__ECSPI1_SCLK | MUX_PAD_CTRL(SPI_PAD_CTRL),
	MX6_PAD_EIM_D17__ECSPI1_MISO | MUX_PAD_CTRL(SPI_PAD_CTRL),
	MX6_PAD_EIM_D18__ECSPI1_MOSI | MUX_PAD_CTRL(SPI_PAD_CTRL),
	MX6_PAD_EIM_D19__GPIO3_IO19 | MUX_PAD_CTRL(NO_PAD_CTRL),
	MX6_PAD_DISP0_DAT18__GPIO5_IO12 | MUX_PAD_CTRL(NO_PAD_CTRL),
};

static void setup_iomux_uart(void)
{
	imx_iomux_v3_setup_multiple_pads(uart1_pads, ARRAY_SIZE(uart1_pads));
}

static int pfuze_mode_init(struct udevice *dev, u32 mode)
{
	unsigned char offset, i, switch_num;
	u32 id;
	int ret;

	id = pmic_reg_read(dev, PFUZE100_DEVICEID);
	id = id & 0xf;

	if (id == 0) {
		switch_num = 6;
		offset = PFUZE100_SW1CMODE;
	} else if (id == 1) {
		switch_num = 4;
		offset = PFUZE100_SW2MODE;
	} else {
		printf("Not supported, id=%d\n", id);
		return -EINVAL;
	}

	ret = pmic_reg_write(dev, PFUZE100_SW1ABMODE, mode);
	if (ret < 0) {
		printf("Set SW1AB mode error!\n");
		return ret;
	}

	for (i = 0; i < switch_num - 1; i++) {
		ret = pmic_reg_write(dev, offset + i * SWITCH_SIZE, mode);
		if (ret < 0) {
			printf("Set switch 0x%x mode error!\n",
			       offset + i * SWITCH_SIZE);
			return ret;
		}
	}

	return ret;
}

int power_init_board(void)
{
	struct udevice *dev;
	unsigned int reg;
	int ret;

	ret = pmic_get("pmic@8", &dev);
	if (ret == -ENODEV)
		return 0;

	if (ret != 0)
		return ret;

	ret = pfuze_mode_init(dev, APS_PFM);
	if (ret < 0)
		return ret;

	/* Increase VGEN3 from 2.5 to 2.8V */
	reg = pmic_reg_read(dev, PFUZE100_VGEN3VOL);
	reg &= ~LDO_VOL_MASK;
	reg |= LDOB_2_80V;
	pmic_reg_write(dev, PFUZE100_VGEN3VOL, reg);

	/* Increase VGEN5 from 2.8 to 3V */
	reg = pmic_reg_read(dev, PFUZE100_VGEN5VOL);
	reg &= ~LDO_VOL_MASK;
	reg |= LDOB_3_00V;
	pmic_reg_write(dev, PFUZE100_VGEN5VOL, reg);
	
	return 0;
}

static void setup_spi(void)
{
        struct gpio_desc desc;
        int ret;

	imx_iomux_v3_setup_multiple_pads(ecspi1_pads, ARRAY_SIZE(ecspi1_pads));

        ret = dm_gpio_lookup_name("GPIO5_12", &desc);
        if (ret)
                return;

        ret = dm_gpio_request(&desc, "Unlock SPI NOR");
        if (ret)
                return;

        dm_gpio_set_dir_flags(&desc, GPIOD_IS_OUT | GPIOD_IS_OUT_ACTIVE);
}

int board_spi_cs_gpio(unsigned bus, unsigned cs)
{
	return (bus == 0 && cs == 0) ? (IMX_GPIO_NR(3, 19)) : -EINVAL;
}

int board_early_init_f(void)
{
	setup_iomux_uart();

	return 0;
}

int board_init(void)
{
	/* address of boot parameters */
	gd->bd->bi_boot_params = PHYS_SDRAM + 0x100;

	setup_spi();

	return 0;
}
