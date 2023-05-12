# Copyright (c) 2012-2014 Toradex, Inc.
# SPDX-License-Identifier:      GPL-2.0+

obj-y  := omni1000.o

# Common for all Toradex modules
ifeq ($(CONFIG_SPL_BUILD),y)
# Necessary to create built-in.o
obj- := __dummy__.o
else
obj-$(CONFIG_TDX_CFG_BLOCK) += ../../toradex/common/tdx-cfg-block.o
obj-y += ../../toradex/common/tdx-common.o
obj-y += ../../toradex/common/tdx-eeprom.o
endif
