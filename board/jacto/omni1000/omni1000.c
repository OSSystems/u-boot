// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2010-2013 Freescale Semiconductor, Inc.
 * Copyright (C) 2013, Boundary Devices <info@boundarydevices.com>
 * Copyright (C) 2014-2019, Toradex AG
 * copied from nitrogen6x
 */

#include <common.h>
#include <cpu_func.h>
#include <dm.h>
#include <image.h>
#include <init.h>
#include <net.h>
#include <linux/bitops.h>
#include <linux/delay.h>

#include <ahci.h>
#include <asm/arch/clock.h>
#include <asm/arch/crm_regs.h>
#include <asm/arch/imx-regs.h>
#include <asm/arch/mx6-ddr.h>
#include <asm/arch/mx6-pins.h>
#include <asm/arch/sys_proto.h>
#include <asm/bootm.h>
#include <asm/gpio.h>
#include <asm/mach-imx/boot_mode.h>
#include <asm/mach-imx/iomux-v3.h>
#include <dm/device-internal.h>
#include <dm/platform_data/serial_mxc.h>
#include <env.h>
#include <env_internal.h>
#include <fsl_esdhc_imx.h>
#include <imx_thermal.h>
#include <micrel.h>
#include <miiphy.h>
#include <netdev.h>
#include <fdt_support.h>
#include <linux/libfdt.h>
#include <asm/mach-imx/iomux-v3.h>
#include <asm/mach-imx/video.h>
#include <splash.h>

DECLARE_GLOBAL_DATA_PTR;

#define MODEM_ONOFF IMX_GPIO_NR(3, 4)

#define MODEM_USB_VBUS IMX_GPIO_NR(2, 18)

#define RS911X_RESET IMX_GPIO_NR(2, 4)

#define RS911X_USB_VBUS IMX_GPIO_NR(6, 10)

#define UART_PAD_CTRL  (PAD_CTL_PUS_100K_UP |			\
	PAD_CTL_SPEED_MED | PAD_CTL_DSE_40ohm |			\
	PAD_CTL_SRE_FAST  | PAD_CTL_HYS)

#define USDHC_PAD_CTRL (PAD_CTL_PUS_47K_UP |			\
	PAD_CTL_SPEED_LOW | PAD_CTL_DSE_40ohm |			\
	PAD_CTL_SRE_FAST  | PAD_CTL_HYS)

#define USDHC_EMMC_PAD_CTRL (PAD_CTL_PUS_47K_UP |		\
	PAD_CTL_SPEED_LOW | PAD_CTL_DSE_80ohm |			\
	PAD_CTL_SRE_FAST  | PAD_CTL_HYS)

#define WEAK_PULLUP	(PAD_CTL_PUS_100K_UP |			\
	PAD_CTL_SPEED_MED | PAD_CTL_DSE_40ohm | PAD_CTL_HYS |	\
	PAD_CTL_SRE_SLOW)

#define WEAK_PULLDOWN	(PAD_CTL_PUS_100K_DOWN |		\
	PAD_CTL_SPEED_MED | PAD_CTL_DSE_40ohm |			\
	PAD_CTL_HYS | PAD_CTL_SRE_SLOW)

int dram_init(void)
{
	/* use the DDR controllers configured size */
	gd->ram_size = get_ram_size((void *)CFG_SYS_SDRAM_BASE,
				    (ulong)imx_ddr_size());

	return 0;
}

static void setup_modem(void)
{
	gpio_request(MODEM_USB_VBUS, "modem_usb_vbus");
	gpio_direction_output(MODEM_USB_VBUS, 1);

	gpio_request(MODEM_ONOFF, "modem_onoff");
	gpio_direction_output(MODEM_ONOFF, 1);
}

static void setup_rs911x(void)
{
	gpio_request(RS911X_USB_VBUS, "rs911x_usb_vbus");
	gpio_direction_output(RS911X_USB_VBUS, 0);

	gpio_request(RS911X_RESET, "rs911x_reset");
	gpio_direction_output(RS911X_RESET, 0);

	mdelay(5);
	gpio_direction_output(RS911X_USB_VBUS, 1);

	mdelay(3);
	gpio_direction_output(RS911X_RESET, 1);
}

static iomux_v3_cfg_t const uart1_pads_dte[] = {
	MX6_PAD_CSI0_DAT10__UART1_RX_DATA | MUX_PAD_CTRL(UART_PAD_CTRL),
	MX6_PAD_CSI0_DAT11__UART1_TX_DATA | MUX_PAD_CTRL(UART_PAD_CTRL),
};

#if defined(CONFIG_FSL_ESDHC_IMX) && defined(CONFIG_SPL_BUILD)
/* Apalis MMC1 */
static iomux_v3_cfg_t const usdhc1_pads[] = {
	MX6_PAD_SD1_CLK__SD1_CLK   | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD1_CMD__SD1_CMD   | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD1_DAT0__SD1_DATA0 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD1_DAT1__SD1_DATA1 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD1_DAT2__SD1_DATA2 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD1_DAT3__SD1_DATA3 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_NANDF_D0__SD1_DATA4 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_NANDF_D1__SD1_DATA5 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_NANDF_D2__SD1_DATA6 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_NANDF_D3__SD1_DATA7 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_DI0_PIN4__GPIO4_IO20   | MUX_PAD_CTRL(NO_PAD_CTRL), /* CD */
#	define GPIO_MMC_CD IMX_GPIO_NR(4, 20)
};

/* eMMC */
static iomux_v3_cfg_t const usdhc3_pads[] = {
	MX6_PAD_SD3_CLK__SD3_CLK    | MUX_PAD_CTRL(USDHC_EMMC_PAD_CTRL),
	MX6_PAD_SD3_CMD__SD3_CMD    | MUX_PAD_CTRL(USDHC_EMMC_PAD_CTRL),
	MX6_PAD_SD3_DAT0__SD3_DATA0 | MUX_PAD_CTRL(USDHC_EMMC_PAD_CTRL),
	MX6_PAD_SD3_DAT1__SD3_DATA1 | MUX_PAD_CTRL(USDHC_EMMC_PAD_CTRL),
	MX6_PAD_SD3_DAT2__SD3_DATA2 | MUX_PAD_CTRL(USDHC_EMMC_PAD_CTRL),
	MX6_PAD_SD3_DAT3__SD3_DATA3 | MUX_PAD_CTRL(USDHC_EMMC_PAD_CTRL),
	MX6_PAD_SD3_DAT4__SD3_DATA4 | MUX_PAD_CTRL(USDHC_EMMC_PAD_CTRL),
	MX6_PAD_SD3_DAT5__SD3_DATA5 | MUX_PAD_CTRL(USDHC_EMMC_PAD_CTRL),
	MX6_PAD_SD3_DAT6__SD3_DATA6 | MUX_PAD_CTRL(USDHC_EMMC_PAD_CTRL),
	MX6_PAD_SD3_DAT7__SD3_DATA7 | MUX_PAD_CTRL(USDHC_EMMC_PAD_CTRL),
	MX6_PAD_SD3_RST__GPIO7_IO08 | MUX_PAD_CTRL(WEAK_PULLUP) | MUX_MODE_SION,
};
#endif /* CONFIG_FSL_ESDHC_IMX & CONFIG_SPL_BUILD */

/* mux the Apalis GPIO pins, so they can be used from the U-Boot cmdline */
static iomux_v3_cfg_t const gpio_pads[] = {
	/* Apalis GPIO1 - GPIO8 */
	MX6_PAD_NANDF_D4__GPIO2_IO04	| MUX_PAD_CTRL(WEAK_PULLUP) |
					  MUX_MODE_SION,
	MX6_PAD_NANDF_D5__GPIO2_IO05	| MUX_PAD_CTRL(WEAK_PULLUP) |
					  MUX_MODE_SION,
	MX6_PAD_NANDF_D6__GPIO2_IO06	| MUX_PAD_CTRL(WEAK_PULLUP) |
					  MUX_MODE_SION,
	MX6_PAD_NANDF_D7__GPIO2_IO07	| MUX_PAD_CTRL(WEAK_PULLUP) |
					  MUX_MODE_SION,
	MX6_PAD_NANDF_RB0__GPIO6_IO10	| MUX_PAD_CTRL(WEAK_PULLUP) |
					  MUX_MODE_SION,
	MX6_PAD_NANDF_WP_B__GPIO6_IO09	| MUX_PAD_CTRL(WEAK_PULLUP) |
					  MUX_MODE_SION,
	MX6_PAD_GPIO_2__GPIO1_IO02	| MUX_PAD_CTRL(WEAK_PULLDOWN) |
					  MUX_MODE_SION,
	MX6_PAD_GPIO_6__GPIO1_IO06	| MUX_PAD_CTRL(WEAK_PULLUP) |
					  MUX_MODE_SION,
	MX6_PAD_GPIO_4__GPIO1_IO04	| MUX_PAD_CTRL(WEAK_PULLUP) |
					  MUX_MODE_SION,
};

static void setup_iomux_gpio(void)
{
	imx_iomux_v3_setup_multiple_pads(gpio_pads, ARRAY_SIZE(gpio_pads));
}

static iomux_v3_cfg_t const usb_pads[] = {
	/* USBH_EN */
	MX6_PAD_GPIO_0__GPIO1_IO00 | MUX_PAD_CTRL(NO_PAD_CTRL) | MUX_MODE_SION,
#	define GPIO_USBH_EN IMX_GPIO_NR(1, 0)
	/* USB_VBUS_DET */
	MX6_PAD_EIM_D28__GPIO3_IO28 | MUX_PAD_CTRL(NO_PAD_CTRL),
#	define GPIO_USB_VBUS_DET IMX_GPIO_NR(3, 28)
	/* USBO1_ID */
	MX6_PAD_ENET_RX_ER__USB_OTG_ID	| MUX_PAD_CTRL(WEAK_PULLUP),
	/* USBO1_EN */
	MX6_PAD_EIM_D22__GPIO3_IO22 | MUX_PAD_CTRL(NO_PAD_CTRL) | MUX_MODE_SION,
#	define GPIO_USBO_EN IMX_GPIO_NR(3, 22)
};

/*
 * UARTs are used in DTE mode, switch the mode on all UARTs before
 * any pinmuxing connects a (DCE) output to a transceiver output.
 */
#define UCR3		0x88	/* FIFO Control Register */
#define UCR3_RI		BIT(8)	/* RIDELT DTE mode */
#define UCR3_DCD	BIT(9)	/* DCDDELT DTE mode */
#define UFCR		0x90	/* FIFO Control Register */
#define UFCR_DCEDTE	BIT(6)	/* DCE=0 */

static void setup_dtemode_uart(void)
{
	setbits_le32((u32 *)(UART1_BASE + UFCR), UFCR_DCEDTE);
	setbits_le32((u32 *)(UART2_BASE + UFCR), UFCR_DCEDTE);
	setbits_le32((u32 *)(UART4_BASE + UFCR), UFCR_DCEDTE);
	setbits_le32((u32 *)(UART5_BASE + UFCR), UFCR_DCEDTE);

	clrbits_le32((u32 *)(UART1_BASE + UCR3), UCR3_DCD | UCR3_RI);
	clrbits_le32((u32 *)(UART2_BASE + UCR3), UCR3_DCD | UCR3_RI);
	clrbits_le32((u32 *)(UART4_BASE + UCR3), UCR3_DCD | UCR3_RI);
	clrbits_le32((u32 *)(UART5_BASE + UCR3), UCR3_DCD | UCR3_RI);
}

static void setup_iomux_dte_uart(void)
{
	setup_dtemode_uart();
	imx_iomux_v3_setup_multiple_pads(uart1_pads_dte, ARRAY_SIZE(uart1_pads_dte));
}

#ifdef CONFIG_USB_EHCI_MX6
int board_ehci_hcd_init(int port)
{
	imx_iomux_v3_setup_multiple_pads(usb_pads, ARRAY_SIZE(usb_pads));
	return 0;
}
#endif

#if defined(CONFIG_FSL_ESDHC_IMX) && defined(CONFIG_SPL_BUILD)
/* use the following sequence: eMMC, MMC1, SD1 */
struct fsl_esdhc_cfg usdhc_cfg[CFG_SYS_FSL_USDHC_NUM] = {
	{USDHC3_BASE_ADDR},
	{USDHC1_BASE_ADDR},
};

int board_mmc_getcd(struct mmc *mmc)
{
	struct fsl_esdhc_cfg *cfg = (struct fsl_esdhc_cfg *)mmc->priv;
	int ret = true; /* default: assume inserted */

	switch (cfg->esdhc_base) {
	case USDHC1_BASE_ADDR:
		gpio_request(GPIO_MMC_CD, "MMC_CD");
		gpio_direction_input(GPIO_MMC_CD);
		ret = !gpio_get_value(GPIO_MMC_CD);
		break;
	}

	return ret;
}

int board_mmc_init(struct bd_info *bis)
{
	struct src *psrc = (struct src *)SRC_BASE_ADDR;
	unsigned int reg = readl(&psrc->sbmr1) >> 11;
	/*
	 * Upon reading BOOT_CFG register the following map is done:
	 * Bit 11 and 12 of BOOT_CFG register can determine the current
	 * mmc port
	 * 0x1                  SD1
	 * 0x2                  SD2
	 * 0x3                  SD4
	 */

	switch (reg & 0x3) {
	case 0x0:
		imx_iomux_v3_setup_multiple_pads(usdhc1_pads, ARRAY_SIZE(usdhc1_pads));
		usdhc_cfg[0].esdhc_base = USDHC1_BASE_ADDR;
		usdhc_cfg[0].sdhc_clk = mxc_get_clock(MXC_ESDHC_CLK);
		gd->arch.sdhc_clk = usdhc_cfg[0].sdhc_clk;
		break;
	case 0x2:
		imx_iomux_v3_setup_multiple_pads(usdhc3_pads, ARRAY_SIZE(usdhc3_pads));
		usdhc_cfg[0].esdhc_base = USDHC3_BASE_ADDR;
		usdhc_cfg[0].sdhc_clk = mxc_get_clock(MXC_ESDHC3_CLK);
		gd->arch.sdhc_clk = usdhc_cfg[0].sdhc_clk;
		break;
	default:
		puts("MMC boot device not available");
	}

	return fsl_esdhc_initialize(bis, &usdhc_cfg[0]);
}
#endif /* CONFIG_FSL_ESDHC_IMX & CONFIG_SPL_BUILD */

static iomux_v3_cfg_t const pwr_intb_pads[] = {
	/*
	 * the bootrom sets the iomux to vselect, potentially connecting
	 * two outputs. Set this back to GPIO
	 */
	MX6_PAD_GPIO_18__GPIO7_IO13 | MUX_PAD_CTRL(NO_PAD_CTRL)
};

static iomux_v3_cfg_t const lcd_pads[] = {
	MX6_PAD_SD4_DAT2__GPIO2_IO10 | MUX_PAD_CTRL(NO_PAD_CTRL),
	MX6_PAD_EIM_DA13__GPIO3_IO13 | MUX_PAD_CTRL(NO_PAD_CTRL),
};

static void setup_iomux_lcd(void)
{
	imx_iomux_v3_setup_multiple_pads(lcd_pads, ARRAY_SIZE(lcd_pads));
}

int board_early_init_f(void)
{
	imx_iomux_v3_setup_multiple_pads(pwr_intb_pads, ARRAY_SIZE(pwr_intb_pads));
	setup_iomux_dte_uart();

	setup_iomux_lcd();
	return 0;
}

#if defined(CONFIG_VIDEO_IPUV3)
#define LVDS_PWM	IMX_GPIO_NR(2, 10)
#define LVDS_BL	IMX_GPIO_NR(3, 13)
static void enable_lvds(struct display_info_t const *dev)
{
	gpio_request(LVDS_PWM, "LVDS_PWM");
	gpio_request(LVDS_BL, "LVDS_BL");
	gpio_direction_output(LVDS_PWM, 1);
	gpio_direction_output(LVDS_BL, 1);
}

struct display_info_t const displays[] = {{
	.bus	= -1,
	.addr	= 0,
	.pixfmt	= IPU_PIX_FMT_RGB24,
	.detect	= NULL,
	.enable	= enable_lvds,
	.mode	= {
		.name           = "Z101WX02J-CT736",
		.refresh        = 60,
		.xres           = 1280,
		.yres           = 800,
		.pixclock       = 14065,
		.left_margin    = 40,
		.right_margin   = 40,
		.upper_margin   = 3,
		.lower_margin   = 80,
		.hsync_len      = 10,
		.vsync_len      = 10,
		.sync           = FB_SYNC_EXT,
		.vmode          = FB_VMODE_NONINTERLACED
} }, {
	.bus	= -1,
	.addr	= 0,
	.pixfmt	= IPU_PIX_FMT_RGB24,
	.detect	= NULL,
	.enable	= enable_lvds,
	.mode	= {
		.name           = "Z101WX16J-CT736",
		.refresh        = 60,
		.xres           = 1280,
		.yres           = 800,
		.pixclock       = 13899,
		.left_margin    = 120,
		.right_margin   = 60,
		.upper_margin   = 40,
		.lower_margin   = 10,
		.hsync_len      = 15,
		.vsync_len      = 12,
		.sync           = FB_SYNC_EXT,
		.vmode          = FB_VMODE_NONINTERLACED
} }, {
	.bus	= -1,
	.addr	= 0,
	.pixfmt	= IPU_PIX_FMT_RGB24,
	.detect	= NULL,
	.enable	= enable_lvds,
	.mode	= {
		.name           = "Z080XG03JCT3",
		.refresh        = 60,
		.xres           = 1024,
		.yres           = 768,
		.pixclock       = 15384,
		.left_margin    = 24,
		.right_margin   = 160,
		.upper_margin   = 3,
		.lower_margin   = 29,
		.hsync_len      = 136,
		.vsync_len      = 6,
		.sync           = FB_SYNC_EXT,
		.vmode          = FB_VMODE_NONINTERLACED
} } };
size_t display_count = ARRAY_SIZE(displays);

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
	     | IOMUXC_GPR2_DATA_WIDTH_CH1_18BIT
	     | IOMUXC_GPR2_BIT_MAPPING_CH0_SPWG
	     | IOMUXC_GPR2_DATA_WIDTH_CH0_24BIT
	     | IOMUXC_GPR2_LVDS_CH1_MODE_DISABLED
	     | IOMUXC_GPR2_LVDS_CH0_MODE_ENABLED_DI0;
	writel(reg, &iomux->gpr[2]);

	reg = readl(&iomux->gpr[3]);
	reg = (reg & ~(IOMUXC_GPR3_LVDS0_MUX_CTL_MASK
			| IOMUXC_GPR3_HDMI_MUX_CTL_MASK))
	    | (IOMUXC_GPR3_MUX_SRC_IPU1_DI0
	       << IOMUXC_GPR3_LVDS0_MUX_CTL_OFFSET);
	writel(reg, &iomux->gpr[3]);
}
#endif /* CONFIG_VIDEO_IPUV3 */

#ifdef CONFIG_SPLASH_SCREEN
static struct splash_location default_splash_locations[] = {
	{
		.name		= "mmc_fs",
		.storage	= SPLASH_STORAGE_MMC,
		.flags		= SPLASH_STORAGE_FS,
		.devpart	= "0:1",
	},
};

int splash_screen_prepare(void)
{
	return splash_source_load(default_splash_locations,
				   ARRAY_SIZE(default_splash_locations));
}
#endif

int overwrite_console(void)
{
	return 1;
}

int board_init(void)
{
	/* address of boot parameters */
	gd->bd->bi_boot_params = PHYS_SDRAM + 0x100;

#if defined(CONFIG_VIDEO_IPUV3)
	setup_display();
#endif
	setup_iomux_gpio();
	setup_modem();
	setup_rs911x();

	return 0;
}

#ifdef CONFIG_BOARD_LATE_INIT
int board_late_init(void)
{
#if defined(CONFIG_REVISION_TAG) && defined(CONFIG_ENV_VARS_UBOOT_RUNTIME_CONFIG)
	char env_str[256];
	u32 rev;

	rev = get_board_revision();
	snprintf(env_str, ARRAY_SIZE(env_str), "%.4x", rev);
	env_set("board_rev", env_str);

#endif /* CONFIG_REVISION_TAG */

	if (is_boot_from_usb()) {
		env_set("bootcmd", "run bootcmd_mfg");
		env_set("bootdelay", "0");
	}

	return 0;
}
#endif /* CONFIG_BOARD_LATE_INIT */

int checkboard(void)
{
	char it[] = " IT";
	int minc, maxc;

	switch (get_cpu_temp_grade(&minc, &maxc)) {
	case TEMP_AUTOMOTIVE:
	case TEMP_INDUSTRIAL:
		break;
	case TEMP_EXTCOMMERCIAL:
	default:
		it[0] = 0;
	};
	printf("Model: Toradex Apalis iMX6 %s %s%s\n",
	       is_cpu_type(MXC_CPU_MX6D) ? "Dual" : "Quad",
	       (gd->ram_size == 0x80000000) ? "2GB" :
	       (gd->ram_size == 0x40000000) ? "1GB" : "512MB", it);
	return 0;
}

#ifdef CONFIG_CMD_BMODE
static const struct boot_mode board_boot_modes[] = {
	/* 4-bit bus width */
	{"mmc",	MAKE_CFGVAL(0x40, 0x20, 0x00, 0x00)},
	{"sd",	MAKE_CFGVAL(0x40, 0x28, 0x00, 0x00)},
	{NULL,	0},
};
#endif

int misc_init_r(void)
{
#ifdef CONFIG_CMD_BMODE
	add_board_boot_modes(board_boot_modes);
#endif
	return 0;
}

#ifdef CONFIG_OF_BOARD_SETUP
int ft_board_setup(void *blob, struct bd_info *bd)
{
	int offset;

	offset = fdt_node_offset_by_compatible(blob, 0, "lltc,ltc2955-poweroff");
	if ((offset > 0) && is_boot_from_usb())
		fdt_status_disabled(blob, offset);

	return 0;
}
#endif

enum env_location env_get_location(enum env_operation op, int prio)
{
	if (prio)
		return ENVL_UNKNOWN;

	if (is_boot_from_usb())
		return ENVL_NOWHERE;

	return ENVL_MMC;
}

#ifdef CONFIG_SPL_BUILD
#include <spl.h>
#include <linux/libfdt.h>
#include "asm/arch/mx6q-ddr.h"
#include "asm/arch/iomux.h"
#include "asm/arch/crm_regs.h"

static void ccgr_init(void)
{
	struct mxc_ccm_reg *ccm = (struct mxc_ccm_reg *)CCM_BASE_ADDR;

	writel(0x00C03F3F, &ccm->CCGR0);
	writel(0x0030FC03, &ccm->CCGR1);
	writel(0x0FFFFFF3, &ccm->CCGR2);
	writel(0x3FF0300F, &ccm->CCGR3);
	writel(0x00FFF300, &ccm->CCGR4);
	writel(0x0F0000F3, &ccm->CCGR5);
	writel(0x000003FF, &ccm->CCGR6);

/*
 * Setup CCM_CCOSR register as follows:
 *
 * clko2_en  = 1     --> CKO2 enabled
 * clko2_div = 000   --> divide by 1
 * clko2_sel = 01110 --> osc_clk (24MHz)
 *
 * clk_out_sel = 1   --> Output CKO2 to CKO1
 *
 * This sets both CLKO2/CLKO1 output to 24MHz,
 * CLKO1 configuration not relevant because of clk_out_sel
 * (CLKO1 set to default)
 */
	writel(0x010E0101, &ccm->ccosr);
}

#define PAD_CTL_INPUT_DDR BIT(17)

static struct mx6dq_iomux_ddr_regs mx6_ddr_ioregs = {
	/* Differential input, 40 ohm DSE */
	.dram_sdclk_0 = PAD_CTL_DSE_40ohm | PAD_CTL_INPUT_DDR,
	.dram_sdclk_1 = PAD_CTL_DSE_40ohm | PAD_CTL_INPUT_DDR,
	.dram_cas = PAD_CTL_DSE_40ohm | PAD_CTL_INPUT_DDR,
	.dram_ras = PAD_CTL_DSE_40ohm | PAD_CTL_INPUT_DDR,
	.dram_reset = PAD_CTL_DSE_40ohm | PAD_CTL_INPUT_DDR,

	/* SDKE[0:1]: BIT(12) and BIT(13) are reserved and set at reset */
	.dram_sdcke0 = 0x00003000,
	.dram_sdcke1 = 0x00003000,

	.dram_sdba2 = 0x00000000,

	/* ODT[0:1]: 40 ohm DSE, BIT(12) and BIT(13) are reserved and set at reset */
	.dram_sdodt0 = PAD_CTL_DSE_40ohm | 0x00003000,
	.dram_sdodt1 = PAD_CTL_DSE_40ohm | 0x00003000,

	/* SDQS[0:7]: 40 ohm DSE, Pull/Keeper Disabled, ODT Disabled */
	.dram_sdqs0 = PAD_CTL_DSE_40ohm,
	.dram_sdqs1 = PAD_CTL_DSE_40ohm,
	.dram_sdqs2 = PAD_CTL_DSE_40ohm,
	.dram_sdqs3 = PAD_CTL_DSE_40ohm,
	.dram_sdqs4 = PAD_CTL_DSE_40ohm,
	.dram_sdqs5 = PAD_CTL_DSE_40ohm,
	.dram_sdqs6 = PAD_CTL_DSE_40ohm,
	.dram_sdqs7 = PAD_CTL_DSE_40ohm,

	/* DQM[0:7]: Differential input, 40 ohm DSE, Pull/Keeper Disabled, ODT Disabled */
	.dram_dqm0 = PAD_CTL_DSE_40ohm | PAD_CTL_INPUT_DDR,
	.dram_dqm1 = PAD_CTL_DSE_40ohm | PAD_CTL_INPUT_DDR,
	.dram_dqm2 = PAD_CTL_DSE_40ohm | PAD_CTL_INPUT_DDR,
	.dram_dqm3 = PAD_CTL_DSE_40ohm | PAD_CTL_INPUT_DDR,
	.dram_dqm4 = PAD_CTL_DSE_40ohm | PAD_CTL_INPUT_DDR,
	.dram_dqm5 = PAD_CTL_DSE_40ohm | PAD_CTL_INPUT_DDR,
	.dram_dqm6 = PAD_CTL_DSE_40ohm | PAD_CTL_INPUT_DDR,
	.dram_dqm7 = PAD_CTL_DSE_40ohm | PAD_CTL_INPUT_DDR,
};

static struct mx6dq_iomux_grp_regs mx6_grp_ioregs = {
	/* DDR3 */
	.grp_ddr_type = 0x000C0000,

	/* SDQS[0:7]: Differential input */
	.grp_ddrmode_ctl = PAD_CTL_INPUT_DDR,

	/* DATA[0:63]: Pull/Keeper disabled */
	.grp_ddrpke = 0,

	/* ADDR[0:16], SDBA[0:1]: 40 ohm DSE */
	.grp_addds = PAD_CTL_DSE_40ohm,

	/* CS0/CS1/SDBA2/CKE0/CKE1/SDWE: 40 ohm DSE */
	.grp_ctlds = PAD_CTL_DSE_40ohm,

	/* DATA[0:63]: Differential input */
	.grp_ddrmode = PAD_CTL_INPUT_DDR,

	/* DATA[0:63]: 40 ohm DSE */
	.grp_b0ds = PAD_CTL_DSE_40ohm,
	.grp_b1ds = PAD_CTL_DSE_40ohm,
	.grp_b2ds = PAD_CTL_DSE_40ohm,
	.grp_b3ds = PAD_CTL_DSE_40ohm,
	.grp_b4ds = PAD_CTL_DSE_40ohm,
	.grp_b5ds = PAD_CTL_DSE_40ohm,
	.grp_b6ds = PAD_CTL_DSE_40ohm,
	.grp_b7ds = PAD_CTL_DSE_40ohm,
};

static struct mx6_ddr_sysinfo sysinfo = {
	.dsize = 2,         /* width of data bus: 2=64 */
	.cs_density = 32,   /* full range so that get_mem_size() works, 32Gb per CS */
	.ncs = 1,
	.cs1_mirror = 0,
	.rtt_wr = 2,        /* Dynamic ODT, RZQ/2 */
	.rtt_nom = 0,       /* Disabled */
	.walat = 0,         /* Write additional latency */
	.ralat = 5,         /* Read additional latency */
	.mif3_mode = 3,     /* Command prediction working mode */
	.bi_on = 1,         /* Bank interleaving enabled */
	.sde_to_rst = 0x10, /* 14 cycles, 200us (JEDEC default) */
	.rst_to_cke = 0x23, /* 33 cycles, 500us (JEDEC default) */
	.pd_fast_exit = 1,  /* enable precharge power-down fast exit */
	.ddr_type = DDR_TYPE_DDR3,
	.refsel = 1,        /* Refresh cycles at 32KHz */
	.refr = 3,          /* 4 refresh commands per refresh cycle */
};

static const struct mx6_mmdc_calibration mx6_mmdc_calib = {
	.p0_mpwldectrl0 = 0x0009000E,
	.p0_mpwldectrl1 = 0x0018000B,
	.p1_mpwldectrl0 = 0x00060015,
	.p1_mpwldectrl1 = 0x0006000E,
	.p0_mpdgctrl0 = 0x432A0338,
	.p0_mpdgctrl1 = 0x03260324,
	.p1_mpdgctrl0 = 0x43340344,
	.p1_mpdgctrl1 = 0x031E027C,
	.p0_mprddlctl = 0x33272D2E,
	.p1_mprddlctl = 0x2F312B37,
	.p0_mpwrdlctl = 0x3A35433C,
	.p1_mpwrdlctl = 0x4336453F,
};

static const struct mx6_ddr3_cfg ddr3_cfg = {
	.mem_speed = 1066,
	.density = 2,
	.width = 16,
	.banks = 8,
	.rowaddr = 14,
	.coladdr = 10,
	.pagesz = 2,
	.trcd = 1312,
	.trcmin = 4812,
	.trasmin = 3500,
	.SRT = 0,
};

struct mx6_ddr_sysinfo sysinfo_it = {
	.dsize = 2,         /* width of data bus: 2=64 */
	.cs_density = 32,   /* full range so that get_mem_size() works, 32Gb per CS */
	.ncs = 1,
	.cs1_mirror = 0,
	.rtt_wr = 1,        /* Dynamic ODT, RZQ/4 */
	.rtt_nom = 1,       /* RZQ/4 */
	.walat = 0,         /* Write additional latency */
	.ralat = 5,         /* Read additional latency */
	.mif3_mode = 3,     /* Command prediction working mode */
	.bi_on = 1,         /* Bank interleaving enabled */
	.sde_to_rst = 0x10, /* 14 cycles, 200us (JEDEC default) */
	.rst_to_cke = 0x23, /* 33 cycles, 500us (JEDEC default) */
	.pd_fast_exit = 1,  /* enable precharge power-down fast exit */
	.ddr_type = DDR_TYPE_DDR3,
	.refsel = 1,        /* Refresh cycles at 32KHz */
	.refr = 7,          /* 8 refresh commands per refresh cycle */
};

static const struct mx6_mmdc_calibration mx6_mmdc_calib_it = {
	.p0_mpwldectrl0 = 0x0009000E,
	.p0_mpwldectrl1 = 0x0018000B,
	.p1_mpwldectrl0 = 0x00060015,
	.p1_mpwldectrl1 = 0x0006000E,
	.p0_mpdgctrl0 = 0x03300338,
	.p0_mpdgctrl1 = 0x03240324,
	.p1_mpdgctrl0 = 0x03440350,
	.p1_mpdgctrl1 = 0x032C0308,
	.p0_mprddlctl = 0x40363C3E,
	.p1_mprddlctl = 0x3C3E3C46,
	.p0_mpwrdlctl = 0x403E463E,
	.p1_mpwrdlctl = 0x4A384C46,
};

static const struct mx6_ddr3_cfg ddr3_cfg_it = {
	.mem_speed = 1066,
	.density = 4,
	.width = 16,
	.banks = 8,
	.rowaddr = 15,
	.coladdr = 10,
	.pagesz = 2,
	.trcd = 1312,
	.trcmin = 4812,
	.trasmin = 3500,
	.SRT = 1,
};


/* Perform DDR DRAM calibration */
static void spl_dram_perform_cal(const struct mx6_ddr_sysinfo *ddr_sysinfo)
{
#ifdef CONFIG_MX6_DDRCAL
	int err;

	err = mmdc_do_write_level_calibration(ddr_sysinfo);
	if (err)
		printf("error %d from write level calibration\n", err);
	err = mmdc_do_dqs_calibration(ddr_sysinfo);
	if (err)
		printf("error %d from dqs calibration\n", err);
#endif
}

static void spl_dram_init(void)
{
	bool temp_grade_it;

	switch (get_cpu_temp_grade(NULL, NULL)) {
	case TEMP_COMMERCIAL:
	case TEMP_EXTCOMMERCIAL:
		puts("Commercial temperature grade DDR3 timings.\n");
		temp_grade_it = false;
		break;
	case TEMP_INDUSTRIAL:
	case TEMP_AUTOMOTIVE:
	default:
		puts("Industrial temperature grade DDR3 timings.\n");
		temp_grade_it = true;
		break;
	};

	mx6dq_dram_iocfg(64, &mx6_ddr_ioregs, &mx6_grp_ioregs);

	if (temp_grade_it)
		mx6_dram_cfg(&sysinfo_it, &mx6_mmdc_calib_it, &ddr3_cfg_it);
	else
		mx6_dram_cfg(&sysinfo, &mx6_mmdc_calib, &ddr3_cfg);

	udelay(100);

	if (temp_grade_it)
		spl_dram_perform_cal(&sysinfo_it);
	else
		spl_dram_perform_cal(&sysinfo);
}

#define LTC_POWER_ON		IMX_GPIO_NR(1, 10)
#define LTC_PB_INT		IMX_GPIO_NR(1, 15)
#define LTC_PB_KILL		IMX_GPIO_NR(1, 14)

static iomux_v3_cfg_t const ltc_pads[] = {
	MX6_PAD_SD2_DAT0__GPIO1_IO15 | MUX_PAD_CTRL(NO_PAD_CTRL),
	MX6_PAD_SD2_DAT1__GPIO1_IO14 | MUX_PAD_CTRL(WEAK_PULLDOWN),
};

static void setup_ltc_pads(void)
{
	imx_iomux_v3_setup_multiple_pads(ltc_pads, ARRAY_SIZE(ltc_pads));

	gpio_request(LTC_PB_KILL, "ltc_pb_kill");
	gpio_direction_output(LTC_PB_KILL, 0);

	gpio_request(LTC_POWER_ON, "ltc_power_on");
	gpio_direction_output(LTC_POWER_ON, 0);

	gpio_request(LTC_PB_INT, "ltc_pb_int");
	gpio_direction_input(LTC_PB_INT);
}

void board_init_f(ulong dummy)
{
	/* setup AIPS and disable watchdog */
	arch_cpu_init();

	ccgr_init();
	gpr_init();

	if (!is_boot_from_usb())
		setup_ltc_pads();

	/* iomux */
	board_early_init_f();

	/* setup GP timer */
	timer_init();

	/* UART clocks enabled and gd valid - init serial console */
	preloader_console_init();

	/* Make sure we use dte mode */
	setup_dtemode_uart();
	/* DDR initialization */
	spl_dram_init();

	/* Clear the BSS. */
	memset(__bss_start, 0, __bss_end - __bss_start);

	/* load/boot image from boot device */
	board_init_r(NULL, 0);
}

void reset_cpu(void)
{
}
#endif /* CONFIG_SPL_BUILD */

static struct mxc_serial_plat mxc_serial_plat = {
	.reg = (struct mxc_uart *)UART1_BASE,
	.use_dte = true,
};

U_BOOT_DRVINFO(mxc_serial) = {
	.name = "serial_mxc",
	.plat = &mxc_serial_plat,
};
