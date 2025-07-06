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
#include <asm/mach-imx/video.h>

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

#define DISP0_PWR_EN	IMX_GPIO_NR(1, 21)

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

static iomux_v3_cfg_t const bl_pads[] = {
	MX6_PAD_SD1_DAT3__GPIO1_IO21 | MUX_PAD_CTRL(NO_PAD_CTRL),
};

#if defined(CONFIG_VIDEO_IPUV3)
static void enable_backlight(void)
{
	imx_iomux_v3_setup_multiple_pads(bl_pads, ARRAY_SIZE(bl_pads));
	gpio_request(DISP0_PWR_EN, "Display Power Enable");
	gpio_direction_output(DISP0_PWR_EN, 1);
}

static void enable_lvds(struct display_info_t const *dev)
{
	enable_backlight();
}

struct display_info_t const displays[] = {{
	.bus	= -1,
	.addr	= 0,
	.pixfmt	= IPU_PIX_FMT_RGB666,
	.detect	= NULL,
	.enable	= enable_lvds,
	.mode	= {
		.name           = "Hannstar-XGA",
		.refresh        = 60,
		.xres           = 1024,
		.yres           = 768,
		.pixclock       = 15384,
		.left_margin    = 160,
		.right_margin   = 24,
		.upper_margin   = 29,
		.lower_margin   = 3,
		.hsync_len      = 136,
		.vsync_len      = 6,
		.sync           = FB_SYNC_EXT,
		.vmode          = FB_VMODE_NONINTERLACED
} } };
size_t display_count = ARRAY_SIZE(displays);

/*
 * Do not overwrite the console
 * Use always serial for U-Boot console
 */
int overwrite_console(void)
{
	return 1;
}

static void setup_display(void)
{
	struct mxc_ccm_reg *mxc_ccm = (struct mxc_ccm_reg *)CCM_BASE_ADDR;
	struct iomuxc *iomux = (struct iomuxc *)IOMUXC_BASE_ADDR;
	int reg;

	enable_ipu_clock();

	/* Turn on LDB0, LDB1, IPU,IPU DI0 clocks */
	reg = readl(&mxc_ccm->CCGR3);
	reg |=  MXC_CCM_CCGR3_LDB_DI0_MASK | MXC_CCM_CCGR3_LDB_DI1_MASK;
	writel(reg, &mxc_ccm->CCGR3);

	/* set LDB0, LDB1 clk select to 011/011 */
	reg = readl(&mxc_ccm->cs2cdr);
	reg &= ~(MXC_CCM_CS2CDR_LDB_DI0_CLK_SEL_MASK
		 | MXC_CCM_CS2CDR_LDB_DI1_CLK_SEL_MASK);
	reg |= (3 << MXC_CCM_CS2CDR_LDB_DI0_CLK_SEL_OFFSET)
	      | (3 << MXC_CCM_CS2CDR_LDB_DI1_CLK_SEL_OFFSET);
	writel(reg, &mxc_ccm->cs2cdr);

	reg = readl(&mxc_ccm->cscmr2);
	reg |= MXC_CCM_CSCMR2_LDB_DI0_IPU_DIV | MXC_CCM_CSCMR2_LDB_DI1_IPU_DIV;
	writel(reg, &mxc_ccm->cscmr2);

	reg = readl(&mxc_ccm->chsccdr);
	reg |= (CHSCCDR_CLK_SEL_LDB_DI0
		<< MXC_CCM_CHSCCDR_IPU1_DI0_CLK_SEL_OFFSET);
	reg |= (CHSCCDR_CLK_SEL_LDB_DI0
		<< MXC_CCM_CHSCCDR_IPU1_DI1_CLK_SEL_OFFSET);
	writel(reg, &mxc_ccm->chsccdr);

	reg = IOMUXC_GPR2_BGREF_RRMODE_EXTERNAL_RES
	     | IOMUXC_GPR2_DI1_VS_POLARITY_ACTIVE_LOW
	     | IOMUXC_GPR2_DI0_VS_POLARITY_ACTIVE_LOW
	     | IOMUXC_GPR2_BIT_MAPPING_CH1_SPWG
	     | IOMUXC_GPR2_DATA_WIDTH_CH1_24BIT
	     | IOMUXC_GPR2_BIT_MAPPING_CH0_SPWG
	     | IOMUXC_GPR2_DATA_WIDTH_CH0_24BIT
	     | IOMUXC_GPR2_LVDS_CH0_MODE_DISABLED
	     | IOMUXC_GPR2_LVDS_CH1_MODE_ENABLED_DI0;
	writel(reg, &iomux->gpr[2]);

	reg = readl(&iomux->gpr[3]);
	reg = (reg & ~(IOMUXC_GPR3_LVDS1_MUX_CTL_MASK
			| IOMUXC_GPR3_HDMI_MUX_CTL_MASK))
	    | (IOMUXC_GPR3_MUX_SRC_IPU1_DI0
	       << IOMUXC_GPR3_LVDS1_MUX_CTL_OFFSET);
	writel(reg, &iomux->gpr[3]);
}
#endif /* CONFIG_VIDEO_IPUV3 */

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

#if defined(CONFIG_VIDEO_IPUV3)
	setup_display();
#endif

	return 0;
}
