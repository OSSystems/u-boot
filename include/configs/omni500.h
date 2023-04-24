/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright 2013-2019 Toradex, Inc.
 *
 * Configuration settings for the Toradex Colibri iMX6
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#include <linux/stringify.h>

#include "mx6_common.h"

#include <asm/arch/imx-regs.h>
#include <asm/mach-imx/gpio.h>

#define CFG_MXC_UART_BASE		UART1_BASE

/* MMC Configs */
#define CFG_SYS_FSL_ESDHC_ADDR	0
#define CFG_SYS_FSL_USDHC_NUM	2

/* USB Configs */
/* Host */
#define CFG_MXC_USB_PORTSC		(PORT_PTS_UTMI | PORT_PTS_PTW)
#define CFG_MXC_USB_FLAGS		0

/* Command definition */

#define BOOT_TARGET_DEVICES(func) \
	func(MMC, mmc, 0) \
	func(USB, usb, 0)

#include <config_distro_bootcmd.h>

#define CFG_MFG_ENV_SETTINGS                                    \
	"mfgtool_args=setenv bootargs console=${console},${baudrate} " \
		"rdinit=/linuxrc " \
		"g_mass_storage.stall=0 g_mass_storage.removable=1 " \
		"g_mass_storage.idVendor=0x066F g_mass_storage.idProduct=0x37FF "\
		"g_mass_storage.iSerialNumber=\"\" "\
		"clk_ignore_unused "\
		"\0" \
	"bootcmd_mfg=run mfgtool_args; bootz ${kernel_addr_r} ${ramdisk_addr_r} ${fdt_addr_r}\0"


#define MEM_LAYOUT_ENV_SETTINGS \
	"bootm_size=0x10000000\0" \
	"fdt_addr_r=0x18000000\0" \
	"kernel_addr_r=" __stringify(CONFIG_SYS_LOAD_ADDR) "\0" \
	"pxefile_addr_r=0x18300000\0" \
	"ramdisk_addr_r=0x13000000\0" \
	"scriptaddr=0x18280000\0"

#define CFG_EXTRA_ENV_SETTINGS \
	BOOTENV \
	"image=zImage\0" \
	"fdt_file=imx6dl-omni500.dtb\0" \
	"console=ttymxc0\0" \
	CFG_MFG_ENV_SETTINGS \
	MEM_LAYOUT_ENV_SETTINGS \

/* Miscellaneous configurable options */

/* Physical Memory Map */
#define PHYS_SDRAM			MMDC0_ARB_BASE_ADDR

#define CFG_SYS_SDRAM_BASE		PHYS_SDRAM
#define CFG_SYS_INIT_RAM_ADDR	IRAM_BASE_ADDR
#define CFG_SYS_INIT_RAM_SIZE	IRAM_SIZE

#endif	/* __CONFIG_H */
