/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * (C) Copyright 2016 Rockchip Electronics Co., Ltd
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#include <configs/rv1108_common.h>

#define CONFIG_SYS_MMC_ENV_DEV	0
/*
 * UpdateHub configuration
 */

/* Environment */
#define UPDATEHUB_LOAD_OS_A     "load mmc 0:1 ${kernel_addr_r} /boot/zImage; " \
                                "load mmc 0:1 ${fdt_addr_r} /boot/${fdtfile} "
#define UPDATEHUB_FIND_ROOT_A   "part uuid mmc 0:1 uuid"

#define UPDATEHUB_LOAD_OS_B     "load mmc 0:2 ${kernel_addr_r} /boot/zImage; " \
                                "load mmc 0:2 ${fdt_addr_r} /boot/${fdtfile} "
#define UPDATEHUB_FIND_ROOT_B   "part uuid mmc 0:2 uuid"

#define UPDATEHUB_BOOTARGS      "console=${console} root=PARTUUID=${uuid} " \
                                "rootfstype=ext4 rootwait rw "
#define UPDATEHUB_BOOTCMD       "bootz ${kernel_addr_r} - ${fdt_addr_r}"

#include <configs/updatehub-common.h>

#undef CONFIG_EXTRA_ENV_SETTINGS
#define CONFIG_EXTRA_ENV_SETTINGS \
	ENV_MEM_LAYOUT_SETTINGS \
	"fdtfile=" CONFIG_DEFAULT_FDT_FILE "\0" \
	"partitions=" PARTS_DEFAULT \
	UPDATEHUB_ENV

#endif
