// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright 2018 NXP
 */

#include <common.h>
#include <env.h>
#include <init.h>
#include <asm/global_data.h>

#include <asm/arch/sys_proto.h>
#include <asm/io.h>

DECLARE_GLOBAL_DATA_PTR;

int board_init(void)
{
	return 0;
}

int board_late_init(void)
{
	if (IS_ENABLED(CONFIG_ENV_VARS_UBOOT_RUNTIME_CONFIG)) {
		env_set("board_name", "LUNA_ONE");
		env_set("board_rev", "iMX8MM");
	}

	return 0;
}
