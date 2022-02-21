/*
 * am335x_mfc.h
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation version 2.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __CONFIG_AM335X_MFC_H
#define __CONFIG_AM335X_MFC_H

#include <configs/ti_am335x_common.h>
#include <linux/sizes.h>


#define CONFIG_SYS_BOOTM_LEN		SZ_16M

/* Clock Defines */
#define V_OSCK				24000000  /* Clock output from T2 */
#define V_SCLK				(V_OSCK)

#ifndef CONFIG_SPL_BUILD

/*
 * UpdateHub configuration
 */

/* Environment */
#define UPDATEHUB_LOAD_OS_A     "load mmc 0:2 ${loadaddr} /boot/zImage; " \
                                "load mmc 0:2 ${fdtaddr} /boot/${fdtfile}"
#define UPDATEHUB_FIND_ROOT_A	"if run loadbootscript; then run bootscript; fi; " \
				"part uuid mmc 0:2 uuid"

#define UPDATEHUB_LOAD_OS_B     "load mmc 0:3 ${loadaddr} /boot/zImage; " \
                                "load mmc 0:3 ${fdtaddr} /boot/${fdtfile}"
#define UPDATEHUB_FIND_ROOT_B	"if run loadbootscript; then run bootscript; fi; " \
				"part uuid mmc 0:3 uuid"

#define UPDATEHUB_BOOTARGS      "console=ttyS0,115200n8 root=PARTUUID=${uuid} " \
                                "rootfstype=ext4 rootwait rw fsck.repair=yes"
#define UPDATEHUB_BOOTCMD       "bootz ${loadaddr} - ${fdtaddr}"

#include <configs/updatehub-common.h>

#undef CONFIG_EXTRA_ENV_SETTINGS
#define CONFIG_EXTRA_ENV_SETTINGS \
	"bootscript=echo Running bootscript from mmc0 ...; source ${loadaddr}\0" \
	"loadbootscript=load mmc 0:1 ${loadaddr} u-boot.scr\0" \
	"fdtfile=am335x-mfc.dtb\0" \
	"fdtaddr=0x88000000\0" \
	"loadaddr=0x82000000\0" \
	"rdaddr=0x88080000\0" \
	UPDATEHUB_ENV
#endif

/* NS16550 Configuration */
#define CONFIG_SYS_NS16550_COM1		0x44e09000
#define CONFIG_SYS_NS16550_COM2		0x48022000
#define CONFIG_SYS_NS16550_COM3		0x48024000
#define CONFIG_SYS_NS16550_COM4		0x481a6000
#define CONFIG_SYS_NS16550_COM5		0x481a8000
#define CONFIG_SYS_NS16550_COM6		0x481aa000

/* PMIC support */
#define CONFIG_POWER_TPS65217
#define CONFIG_POWER_TPS65910

/*
 * USB configuration.  We enable MUSB support, both for host and for
 * gadget.  We set USB0 as peripheral and USB1 as host, based on the
 * board schematic and physical port wired to each.  Then for host we
 * add mass storage support and for gadget we add both RNDIS ethernet
 * and DFU.
 */
#define CONFIG_AM335X_USB0
#define CONFIG_AM335X_USB0_MODE	MUSB_PERIPHERAL
#define CONFIG_AM335X_USB1
#define CONFIG_AM335X_USB1_MODE MUSB_HOST

/*
 * Disable MMC DM for SPL build and can be re-enabled after adding
 * DM support in SPL
 */
#ifdef CONFIG_SPL_BUILD
#undef CONFIG_DM_MMC
#undef CONFIG_TIMER
#endif

/* USB Device Firmware Update support */
#ifndef CONFIG_SPL_BUILD
#define DFUARGS \
	DFU_ALT_INFO_EMMC \
	DFU_ALT_INFO_MMC \
	DFU_ALT_INFO_RAM \
	DFU_ALT_INFO_NAND
#endif

#endif	/* ! __CONFIG_AM335X_MFC_H */
