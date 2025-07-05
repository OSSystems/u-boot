/* SPDX-License-Identifier: GPL-2.0+ */
// Copyright (C) Fabio Estevam <festevam@gmail.com>

#ifndef __HRD1_CONFIG_H
#define __HRD1_CONFIG_H

#include <config_distro_bootcmd.h>

#include "mx6_common.h"

/* Physical Memory Map */
#define PHYS_SDRAM			MMDC0_ARB_BASE_ADDR
#define CFG_SYS_SDRAM_BASE		PHYS_SDRAM
#define CFG_SYS_INIT_RAM_ADDR		IRAM_BASE_ADDR
#define CFG_SYS_INIT_RAM_SIZE		IRAM_SIZE

#define CFG_MXC_UART_BASE		UART1_BASE

#endif
