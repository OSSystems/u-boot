/*
 * Copyright (C) 2016 Holger Schurig
 *
 * Author: Holger Schurig  <schurig@dlog.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __PTXC_CONFIG_H
#define __PTXC_CONFIG_H

#include <asm/arch/imx-regs.h>
#include <asm/imx-common/gpio.h>
#include <linux/sizes.h>

#define CONFIG_MACH_TYPE                3980
#define CONFIG_MXC_UART_BASE            UART1_BASE
#define CONFIG_BAUDRATE                 115200
#define CONFIG_CONSOLE_DEV              "ttymxc0"

#define CONFIG_MX6
#include "mx6_common.h"
#include <asm/arch/imx-regs.h>
#include <asm/imx-common/gpio.h>

#define CONFIG_SYS_GENERIC_BOARD
#define CONFIG_DISPLAY_CPUINFO
#define CONFIG_DISPLAY_BOARDINFO

#define CONFIG_CMDLINE_TAG
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_REVISION_TAG

#define CONFIG_BOARD_EARLY_INIT_F
#define CONFIG_BOARD_LATE_INIT
#define CONFIG_MXC_GPIO
#define CONFIG_MXC_UART

/* MMC Configs */
#define CONFIG_FSL_ESDHC
#define CONFIG_FSL_USDHC
#define CONFIG_SYS_FSL_ESDHC_ADDR       0
#define CONFIG_SYS_FSL_USDHC_NUM        2
#define CONFIG_MMC
#define CONFIG_CMD_MMC
#define CONFIG_GENERIC_MMC
#define CONFIG_BOUNCE_BUFFER
#define CONFIG_DOS_PARTITION

/* Command definition */
#include <config_cmd_default.h>

#define CONFIG_CMD_EXT4
#define CONFIG_CMD_BOOTZ
#define CONFIG_CMD_SETEXPR
#undef CONFIG_CMD_IMLS

#define CONFIG_BOOTDELAY                1

#define CONFIG_LOADADDR                 0x12000000
#define CONFIG_SYS_TEXT_BASE            0x17800000

/* Miscellaneous configurable options */
#define CONFIG_SYS_LONGHELP
#define CONFIG_SYS_HUSH_PARSER
#define CONFIG_SYS_PROMPT_HUSH_PS2      "> "
#define CONFIG_AUTO_COMPLETE
#define CONFIG_SYS_CBSIZE               1024

/* Print Buffer Size */
#define CONFIG_SYS_PBSIZE               (CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)
#define CONFIG_SYS_MAXARGS              256
#define CONFIG_SYS_BARGSIZE             CONFIG_SYS_CBSIZE

#define CONFIG_CMD_MEMTEST
#define CONFIG_SYS_MEMTEST_START        0x10000000
#define CONFIG_SYS_MEMTEST_END          0x10010000
#define CONFIG_SYS_MEMTEST_SCRATCH      0x10800000

#define CONFIG_SYS_LOAD_ADDR            CONFIG_LOADADDR

#define CONFIG_CMDLINE_EDITING
#define CONFIG_STACKSIZE                (128 * 1024)

/* Physical Memory Map */
#define CONFIG_NR_DRAM_BANKS            1
#define PHYS_SDRAM                      MMDC0_ARB_BASE_ADDR

#define CONFIG_SYS_SDRAM_BASE           PHYS_SDRAM
#define CONFIG_SYS_INIT_RAM_ADDR        IRAM_BASE_ADDR
#define CONFIG_SYS_INIT_RAM_SIZE        IRAM_SIZE

#define CONFIG_SYS_INIT_SP_OFFSET       (CONFIG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_ADDR         (CONFIG_SYS_INIT_RAM_ADDR + CONFIG_SYS_INIT_SP_OFFSET)

/* Size of malloc() pool */
#define CONFIG_SYS_MALLOC_LEN           (10 * SZ_1M)

/* FLASH and environment organization */
#define CONFIG_ENV_IS_NOWHERE
#define CONFIG_SYS_NO_FLASH
#define CONFIG_ENV_OVERWRITE
#define CONFIG_ENV_SIZE                 (8 * 1024)

/* Device Tree */
#define CONFIG_OF_LIBFDT

/* i2c */
#define CONFIG_CMD_I2C
#define CONFIG_SYS_I2C
#define CONFIG_SYS_I2C_MXC
#define CONFIG_SYS_I2C_SPEED            100000


#define CONFIG_MFG_ENV_SETTINGS                                         \
    "mfgtool_args=setenv bootargs console=" CONFIG_CONSOLE_DEV ",115200 " \
        "bootcmd_mfg=run mfgtool_args;bootz ${loadaddr} - ${fdt_addr};\0" \

#define CONFIG_EXTRA_ENV_SETTINGS \
    CONFIG_MFG_ENV_SETTINGS \
    "script=boot.scr\0" \
    "image=zImage\0" \
    "fdt_file=" CONFIG_DEFAULT_FDT_FILE "\0" \
    "fdt_addr=0x18000000\0" \
    "fdt_high=0xffffffff\0"   \
    "console=" CONFIG_CONSOLE_DEV "\0" \
    "mmcargs=setenv bootargs console=${console},${baudrate} ${smp} " \
        "root=${mmcroot}\0" \
    "bootscript=echo Running bootscript from mmc ...; " \
        "source\0"

#define CONFIG_BOOTCOMMAND \
    "mmc dev 0;" \
    "if mmc rescan; then " \
        "ext4load mmc 0:1 ${loadaddr} /boot/vmlinuz; " \
        "ext4load mmc 0:1 ${fdt_addr} /boot/imx6q-ptxc.dtb; " \
        "setenv bootargs console=ttymxc0,115200 root=/dev/sdblk0p1 rw; " \
        "bootz ${loadaddr} - ${fdt_addr}; " \
    "fi"

#endif
