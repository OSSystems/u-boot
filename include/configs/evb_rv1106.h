/*
 * (C) Copyright 2022 Rockchip Electronics Co., Ltd
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#ifndef __EVB_RV1106_H
#define __EVB_RV1106_H

#include <configs/rv1106_common.h>

#define CONFIG_SYS_MMC_ENV_DEV 0

#define ROCKCHIP_DEVICE_SETTINGS \
			"stdout=serial,vidconsole\0" \
			"stderr=serial,vidconsole\0"
#undef CONFIG_CONSOLE_SCROLL_LINES
#define CONFIG_CONSOLE_SCROLL_LINES            10

#ifndef CONFIG_SPL_BUILD

/* Bootcount via environment for UpdateHub A/B failover */
#define CONFIG_BOOTCOUNT_LIMIT
#define CONFIG_BOOTCOUNT_ENV

/* UpdateHub A/B boot scheme for RV1106 SPI NAND */
#define UPDATEHUB_FIND_ROOT_A \
	"ubi part rootfs; ubifsmount ubi0:system_a; setenv ubi_root_vol system_a"
#define UPDATEHUB_FIND_ROOT_B \
	"ubi part rootfs; ubifsmount ubi0:system_b; setenv ubi_root_vol system_b"
#define UPDATEHUB_LOAD_OS_A \
	"ubifsload ${kernel_addr_r} /boot/zImage; " \
	"ubifsload ${fdt_addr_r} /boot/rv1106g-luckfox-pico-pro-max.dtb"
#define UPDATEHUB_LOAD_OS_B \
	"ubifsload ${kernel_addr_r} /boot/zImage; " \
	"ubifsload ${fdt_addr_r} /boot/rv1106g-luckfox-pico-pro-max.dtb"
#define UPDATEHUB_BOOTARGS \
	"earlycon=uart8250,mmio32,0xff4c0000 console=ttyFIQ0 " \
	"ubi.mtd=3 root=ubi0:${ubi_root_vol} rootfstype=ubifs rootwait"
#define UPDATEHUB_BOOTCMD \
	"bootz ${kernel_addr_r} - ${fdt_addr_r}"

#include <configs/updatehub-common.h>

#endif /* !CONFIG_SPL_BUILD */
#endif /* __EVB_RV1106_H */
