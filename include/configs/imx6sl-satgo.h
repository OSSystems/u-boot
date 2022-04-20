/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright 2013 Freescale Semiconductor, Inc.
 *
 * Configuration settings for the Elgin i.MX6SL Satgo board.
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#include "mx6_common.h"

/* Size of malloc() pool */
#define CONFIG_SYS_MALLOC_LEN		(3 * SZ_1M)

#define CONFIG_MXC_UART_BASE		UART1_IPS_BASE_ADDR

#define CONFIG_EXTRA_ENV_SETTINGS \
        "image=zImage\0" \
        "fdt_high=0xffffffff\0" \
        "initrd_high=0xffffffff\0" \
        "fdt_file=imx6sl-satgo.dtb\0" \
        "fdt_addr=0x88000000\0" \
        "mmcdev=0\0" \
        "mmcdev=1\0" \
        UPDATEHUB_ENV

#define UPDATEHUB_LOAD_OS_A     "load mmc 0:2 ${loadaddr} /boot/${image}; " \
                                "load mmc 0:2 ${fdt_addr} /boot/${fdt_file} "
#define UPDATEHUB_FIND_ROOT_A   "part uuid mmc 0:2 uuid"

#define UPDATEHUB_LOAD_OS_B     "load mmc 0:3 ${loadaddr} /boot/${image}; " \
                                "load mmc 0:3 ${fdt_addr} /boot/${fdt_file} "
#define UPDATEHUB_FIND_ROOT_B   "part uuid mmc 0:2 uuid"

#define UPDATEHUB_BOOTARGS      "console=ttymxc0,1500000n8 root=PARTUUID=${uuid} " \
                                "rootfstype=ext4 rootwait rw "
#define UPDATEHUB_BOOTCMD       "bootz ${loadaddr} - ${fdt_addr}"

#include <configs/updatehub-common.h>

/* Miscellaneous configurable options */

/* Physical Memory Map */
#define PHYS_SDRAM			MMDC0_ARB_BASE_ADDR

#define CONFIG_SYS_SDRAM_BASE		PHYS_SDRAM
#define CONFIG_SYS_INIT_RAM_ADDR	IRAM_BASE_ADDR
#define CONFIG_SYS_INIT_RAM_SIZE	IRAM_SIZE

#define CONFIG_SYS_INIT_SP_OFFSET \
	(CONFIG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_ADDR \
	(CONFIG_SYS_INIT_RAM_ADDR + CONFIG_SYS_INIT_SP_OFFSET)

#define CONFIG_MXC_USB_PORTSC		(PORT_PTS_UTMI | PORT_PTS_PTW)
#define CONFIG_IMX_THERMAL
#define CONFIG_USBD_HS
#endif				/* __CONFIG_H */
