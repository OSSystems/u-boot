/* SPDX-License-Identifier: GPL-2.0+ */

#ifndef __EMA40I_CONFIG_H
#define __EMA40I_CONFIG_H

#include <configs/sunxi-common.h>

#define CFG_ENV_FLAGS_LIST_STATIC \
	"updatehub_active:dw," \
	"upgrade_available:dw," \
	"bootcount:dw,"

#endif /* __EMA40I_CONFIG_H */
