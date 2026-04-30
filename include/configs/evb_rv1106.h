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

/* UpdateHub A/B boot scheme for RV1106 eMMC.
 *
 * Partition map (matches UPDATEHUB_BLKDEVPARTS below):
 *   p1 = system_a, p2 = system_b, p3 = data
 *
 * Linux uses `root=PARTLABEL=system_a`. cmdline_partition sets each
 * partition's volname from the (name) in blkdevparts, and the kernel's
 * name_to_dev_t matches PARTLABEL= against volname. Filesystem-level
 * LABEL= would require udev/initramfs, which we don't have.
 *
 * U-Boot 2017.09's blk_get_device_part_str only parses `dev:N` (numeric);
 * the `dev#partname` syntax is not in this fork, so ext4load uses numeric
 * partition indices. */
#define UPDATEHUB_FIND_ROOT_A \
	"setenv rootdev PARTLABEL=system_a"
#define UPDATEHUB_FIND_ROOT_B \
	"setenv rootdev PARTLABEL=system_b"
#define UPDATEHUB_LOAD_OS_A \
	"ext4load mmc 0:1 ${kernel_addr_r} /boot/zImage; " \
	"ext4load mmc 0:1 ${fdt_addr_r} /boot/rv1106g-luckfox-pico-ultra.dtb"
#define UPDATEHUB_LOAD_OS_B \
	"ext4load mmc 0:2 ${kernel_addr_r} /boot/zImage; " \
	"ext4load mmc 0:2 ${fdt_addr_r} /boot/rv1106g-luckfox-pico-ultra.dtb"
/* blkdevparts on the kernel cmdline so cmdline_partition synthesises
 * mmcblk0p1..p3 (the eMMC has no MBR — update.img doesn't write sector 0).
 * ${blkdevparts} expands from UPDATEHUB_BLKDEVPARTS in the compile-time
 * default env, so this works without env.img on flash. */
#define UPDATEHUB_BOOTARGS \
	"earlycon=uart8250,mmio32,0xff4c0000 console=ttyFIQ0 " \
	"root=${rootdev} rootfstype=ext4 rootwait " \
	"blkdevparts=${blkdevparts}"
#define UPDATEHUB_BOOTCMD \
	"bootz ${kernel_addr_r} - ${fdt_addr_r}"

#include <configs/updatehub-common.h>

/* Compile-time blkdevparts: the eMMC has no MBR (update.img doesn't write
 * sector 0), so Linux's cmdline_partition driver synthesises mmcblk0p1..p3
 * from this string. U-Boot proper also reads it via ENVF defaults to
 * enumerate partitions for ext4load. Kept out of CONFIG_ENVF_LIST so
 * saveenv can't drift it from the hardware layout. */
#define UPDATEHUB_BLKDEVPARTS \
	"blkdevparts=mmcblk0:400M@10M(system_a),400M(system_b),800M(data)\0"

/* Redefine to include UPDATEHUB_BLKDEVPARTS and UPDATEHUB_ENV (both now in
 * scope after the include) */
#undef CONFIG_EXTRA_ENV_SETTINGS
#define CONFIG_EXTRA_ENV_SETTINGS	\
	ENV_MEM_LAYOUT_SETTINGS		\
	ROCKCHIP_DEVICE_SETTINGS	\
	RKIMG_DET_BOOTDEV		\
	UPDATEHUB_BLKDEVPARTS		\
	UPDATEHUB_ENV

#endif /* !CONFIG_SPL_BUILD */
#endif /* __EVB_RV1106_H */
