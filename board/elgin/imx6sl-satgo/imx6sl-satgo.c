// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2013 Freescale Semiconductor, Inc.
 *
 * Author: Fabio Estevam <fabio.estevam@freescale.com>
 */

#include <init.h>
#include <net.h>
#include <asm/arch/clock.h>
#include <asm/arch/iomux.h>
#include <asm/arch/crm_regs.h>
#include <asm/arch/imx-regs.h>
#include <asm/arch/mx6-ddr.h>
#include <asm/arch/mx6-pins.h>
#include <asm/arch/sys_proto.h>
#include <asm/gpio.h>
#include <asm/mach-imx/iomux-v3.h>
#include <asm/io.h>
#include <linux/sizes.h>
#include <common.h>

DECLARE_GLOBAL_DATA_PTR;

#define UART_PAD_CTRL  (PAD_CTL_PUS_100K_UP |			\
	PAD_CTL_SPEED_MED | PAD_CTL_DSE_40ohm |			\
	PAD_CTL_SRE_FAST  | PAD_CTL_HYS)

int dram_init(void)
{
	gd->ram_size = imx_ddr_size();

	return 0;
}

static iomux_v3_cfg_t const uart1_pads[] = {
	MX6_PAD_UART1_TXD__UART1_TXD | MUX_PAD_CTRL(UART_PAD_CTRL),
	MX6_PAD_UART1_RXD__UART1_RXD | MUX_PAD_CTRL(UART_PAD_CTRL),
};

static void setup_iomux_uart(void)
{
	imx_iomux_v3_setup_multiple_pads(uart1_pads, ARRAY_SIZE(uart1_pads));
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

	return 0;
}

int checkboard(void)
{
	puts("Board: i.MX6SL SatGo\n");

	return 0;
}

#ifdef CONFIG_SPL_BUILD

#include <fsl_esdhc_imx.h>
#include <spl.h>
#include <linux/libfdt.h>
#include <asm/arch/mx6-ddr.h>

#define USDHC_PAD_CTRL (PAD_CTL_PUS_22K_UP |			\
	PAD_CTL_SPEED_LOW | PAD_CTL_DSE_80ohm |			\
	PAD_CTL_SRE_FAST  | PAD_CTL_HYS)

static struct fsl_esdhc_cfg usdhc_cfg[1] = {
	{USDHC2_BASE_ADDR, 0, 8},
};

void reset_cpu(ulong addr)
{
}

static iomux_v3_cfg_t const usdhc2_pads[] = {
	MX6_PAD_SD2_CLK__USDHC2_CLK | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD2_CMD__USDHC2_CMD | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD2_DAT0__USDHC2_DAT0 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD2_DAT1__USDHC2_DAT1 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD2_DAT2__USDHC2_DAT2 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD2_DAT3__USDHC2_DAT3 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD2_DAT4__USDHC2_DAT4 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD2_DAT5__USDHC2_DAT5 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD2_DAT6__USDHC2_DAT6 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD2_DAT7__USDHC2_DAT7 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD2_RST__USDHC2_RST   | MUX_PAD_CTRL(USDHC_PAD_CTRL),
};

int board_mmc_getcd(struct mmc *mmc)
{
	return 1;
}

int board_mmc_init(struct bd_info *bis)
{
	imx_iomux_v3_setup_multiple_pads(usdhc2_pads, ARRAY_SIZE(usdhc2_pads));
	usdhc_cfg[0].esdhc_base = USDHC2_BASE_ADDR;
	usdhc_cfg[0].max_bus_width = 8;
	usdhc_cfg[0].sdhc_clk = mxc_get_clock(MXC_ESDHC2_CLK);

	gd->arch.sdhc_clk = usdhc_cfg[0].sdhc_clk;
	return fsl_esdhc_initialize(bis, &usdhc_cfg[0]);
}

static void ccgr_init(void)
{
	struct mxc_ccm_reg *ccm = (struct mxc_ccm_reg *)CCM_BASE_ADDR;

	writel(0xFFFFFFFF, &ccm->CCGR0);
	writel(0xFFFFFFFF, &ccm->CCGR1);
	writel(0xFFFFFFFF, &ccm->CCGR2);
	writel(0xFFFFFFFF, &ccm->CCGR3);
	writel(0xFFFFFFFF, &ccm->CCGR4);
	writel(0xFFFFFFFF, &ccm->CCGR5);
	writel(0xFFFFFFFF, &ccm->CCGR6);
	writel(0xFFFFFFFF, &ccm->CCGR7);
	writel(0x00260324, &ccm->cbcmr);
}

static int issi_dcd_table[] = {
	0x020c4068, 0xffffffff,
	0x020c406c, 0xffffffff,
	0x020c4070, 0xffffffff,
	0x020c4074, 0xffffffff,
	0x020c4078, 0xffffffff,
	0x020c407c, 0xffffffff,
	0x020c4080, 0xffffffff,
	0x020c4018, 0x00260324,
	0x020e05c0, 0x00000000,
	0x020e05b4, 0x00000000,
	0x020e0338, 0x00000030,
	0x020e0300, 0x00000030,
	0x020e031c, 0x00000030,
	0x020e0320, 0x00000030,
	0x020e032c, 0x00000000,
	0x020e05ac, 0x00000030,
	0x020e05c8, 0x00000030,
	0x020e05b0, 0x00000000,
	0x020e0344, 0x00000030,
	0x020e0348, 0x00000030,
	0x020e034c, 0x00000030,
	0x020e0350, 0x00000030,
	0x020e05d0, 0x000C0000,
	0x020e05c4, 0x00000030,
	0x020e05cc, 0x00000030,
	0x020e05d4, 0x00000030,
	0x020e05d8, 0x00000030,
	0x020e030c, 0x00000030,
	0x020e0310, 0x00000030,
	0x020e0314, 0x00000030,
	0x020e0318, 0x00000030,
	0x021b0800, 0xa1390003,
	0x021b080c, 0x00160008,
	0x021b0810, 0x001F001F,
	0x021b083c, 0x407E007F,
	0x021b0840, 0x00690069,
	0x021b0848, 0x42424645,
	0x021b0850, 0x3B383630,
	0x021b081c, 0x33333333,
	0x021b0820, 0x33333333,
	0x021b0824, 0x33333333,
	0x021b08b8, 0x00000800,
	0x021b0004, 0x00020036,
	0x021b0008, 0x24444040,
	0x021b000c, 0x8A8F7925,
	0x021b0010, 0xFF320E64,
	0x021b0018, 0x00001740,
	0x021b001c, 0x00008000,
	0x021b002c, 0x000026d2,
	0x021b0030, 0x008F1023,
	0x021b0040, 0x0000004F,
	0x021b0000, 0x84180000,
	0x021b001c, 0x04088032,
	0x021b001c, 0x00008033,
	0x021b001c, 0x00048031,
	0x021b001c, 0x09408030,
	0x021b001c, 0x04008040,
	0x021b001c, 0x0408803A,
	0x021b001c, 0x0000803B,
	0x021b001c, 0x00048039,
	0x021b001c, 0x09408038,
	0x021b001c, 0x04008048,
	0x021b0020, 0x00005800,
	0x021b0818, 0x00011117,
	0x021b0004, 0x00025576,
	0x021b001c, 0x00000000,
};

static void ddr_init(int *table, int size)
{
	int i;

	for (i = 0; i < size / 2 ; i++)
		writel(table[2 * i + 1], table[2 * i]);
}

static void spl_dram_init(void)
{
	ddr_init(issi_dcd_table, ARRAY_SIZE(issi_dcd_table));
}

void board_init_f(ulong dummy)
{
	/* setup AIPS and disable watchdog */
	arch_cpu_init();

	ccgr_init();
	board_early_init_f();

	/* setup GP timer */
	timer_init();

	/* UART clocks enabled and gd valid - init serial console */
	preloader_console_init();

	/* DDR initialization */
	spl_dram_init();

	/* Clear the BSS. */
	memset(__bss_start, 0, __bss_end - __bss_start);

	/* load/boot image from boot device */
	board_init_r(NULL, 0);
}
#endif
