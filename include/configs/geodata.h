/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Configuration settings for the Jacto i.MX6ULL Geodata board.
 */
#ifndef __CONFIG_
#define __CONFIG_

#include <linux/sizes.h>
#include "mx6_common.h"

#define CFG_MXC_UART_BASE		UART1_BASE

#define CFG_MFG_ENV_SETTINGS                                    \
	"mfgtool_args=setenv bootargs console=${console},${baudrate} " \
		"rdinit=/linuxrc " \
		"g_mass_storage.stall=0 g_mass_storage.removable=1 " \
		"g_mass_storage.idVendor=0x066F g_mass_storage.idProduct=0x37FF "\
		"g_mass_storage.iSerialNumber=\"\" "\
		"clk_ignore_unused "\
		"\0" \
	"initrd_addr=0x83800000\0" \
	"initrd_high=0xffffffff\0" \
	"bootcmd_mfg=run mfgtool_args;bootz ${loadaddr} ${initrd_addr} ${fdt_addr_r};\0"

#define CFG_EXTRA_ENV_SETTINGS \
	CFG_MFG_ENV_SETTINGS \
	"script=boot.scr\0" \
	"console=ttymxc0\0" \
	"fdt_file=imx6ull-geodata.dtb\0" \
	"fdt_high=0xffffffff\0" \
	"initrd_high=0xffffffff\0" \
	"fdt_addr_r=0x83000000\0" \
	"fdt_addr=0x83000000\0" \
	"image=zImage\0" \
	"mmcdev=" __stringify(CFG_SYS_MMC_ENV_DEV) "\0" \
	"mmcpart=" __stringify(CFG_SYS_MMC_IMG_LOAD_PART) "\0" \
	"kernel_addr_r=" __stringify(CONFIG_LOADADDR) "\0" \
	"pxefile_addr_r=" __stringify(CONFIG_LOADADDR) "\0" \
	"ramdisk_addr_r=0x83800000\0" \
	"ramdiskaddr=0x83800000\0" \
	"scriptaddr=" __stringify(CONFIG_LOADADDR) "\0" \
	"loadbootscript=" \
		"load mmc ${mmcdev}:${mmcpart} ${loadaddr} ${script};\0" \
	"bootscript=echo Running bootscript from mmc ...; " \
		"source\0" \

#include <linux/stringify.h>

/* Physical Memory Map */
#define PHYS_SDRAM			MMDC0_ARB_BASE_ADDR

#define CFG_SYS_SDRAM_BASE		PHYS_SDRAM
#define CFG_SYS_INIT_RAM_ADDR	IRAM_BASE_ADDR
#define CFG_SYS_INIT_RAM_SIZE	IRAM_SIZE

/* environment organization */
#define CFG_SYS_MMC_ENV_DEV		1	/* USDHC2 */
#define CFG_SYS_MMC_IMG_LOAD_PART	1

#define CFG_MXC_USB_PORTSC		(PORT_PTS_UTMI | PORT_PTS_PTW)

#endif
