#
# Copyright 2018-2020 NXP
#
# SPDX-License-Identifier: BSD-3-Clause
#


 # SoC-specific build parameters
I2C_DRIVER	:=	yes
SOC		:=	lx2160
PLAT_PATH	:=	plat/nxp
PLAT_SOC_PATH	:=	${PLAT_PATH}/soc-${SOC}
BOARD_PATH	:=	${PLAT_SOC_PATH}/${BOARD}

 # get SoC-specific defnitions
include ${PLAT_SOC_PATH}/soc.def

 # common make across all platforms
include ${PLAT_PATH}/common/common.mk

PLAT_INCLUDES	+=	-I${PLAT_SOC_PATH}/include	\
				-I${BOARD_PATH}

PLAT_BL_COMMON_SOURCES	+=	${PLAT_SOC_PATH}/aarch64/${SOC}_helpers.S	\
				${PLAT_SOC_PATH}/soc.c \
				${PLAT_COMMON_PATH}/pmu.c

BL31_SOURCES	+=	${PLAT_SOC_PATH}/$(ARCH)/${SOC}.S	\
			${PLAT_COMMON_PATH}/$(ARCH)/bl31_data.S

ifeq (${WARM_BOOT}, yes)
BL31_SOURCES	+=	${PLAT_SOC_PATH}/$(ARCH)/${SOC}_warm_rst.S
endif

ifeq (${TEST_BL31}, 1)
BL31_SOURCES	+=	${PLAT_SOC_PATH}/$(ARCH)/bootmain64.S  \
			${PLAT_SOC_PATH}/$(ARCH)/nonboot64.S
endif

include ${PLAT_SOC_PATH}/ddr_fip.mk
include ${PLAT_SOC_PATH}/erratas_soc.mk
