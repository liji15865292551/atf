#
# Copyright 2018-2020 NXP
#
# SPDX-License-Identifier: BSD-3-Clause
#

# board-specific build parameters
BOOT_MODE	:= 	flexspi_nor
BOARD		:=	lx2160a_bbx3
POVDD_ENABLE	:=	no

ifeq (${POVDD_ENABLE},yes)
$(eval $(call add_define,CONFIG_POVDD_ENABLE))
endif

include plat/nxp/soc-lx2160/lx2160ardb/platform.def
$(eval $(call add_define,CONFIG_${FLASH_TYPE}))
$(eval $(call add_define,NXP_FLEXSPI_FLASH_SIZE))
$(eval $(call add_define_val,WARM_BOOT_FLAG_BASE_ADDR,'${BL2_BIN_XSPI_NOR_END_ADDRESS} - 2 * ${NXP_XSPI_NOR_UNIT_SIZE}'))

# get SoC common build parameters
include plat/nxp/soc-lx2160/soc.mk

BL2_SOURCES	+=	${BOARD_PATH}/ddr_init.c	\
			${BOARD_PATH}/platform.c

