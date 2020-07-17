#
# Copyright 2020 NXP
#
# SPDX-License-Identifier: BSD-3-Clause
#

# For TRUSTED_BOARD_BOOT platforms need to include this makefile
# Following definations are to be provided by platform.mk file or
# by user - BL33_INPUT_FILE, BL32_INPUT_FILE, BL31_INPUT_FILE

PLAT_INCLUDES		+=	-Iinclude/common/tbbr

include drivers/nxp/security_monitor/snvs.mk

# Generic files for authentication framework
BL2_SOURCES		+=	drivers/auth/auth_mod.c			\
				drivers/auth/crypto_mod.c		\
				drivers/auth/img_parser_mod.c		\
				plat/common/tbbr/plat_tbbr.c

PLAT_AUTH_PATH	:=  $(PLAT_DRIVERS_PATH)/auth

# If MBEDTLS_DIR is not specified, use CSF Header option
ifeq (${MBEDTLS_DIR},)
    PLAT_INCLUDES	+= -I$(PLAT_DRIVERS_PATH)/sfp
    BL2_SOURCES		+=	$(PLAT_AUTH_PATH)/csf_hdr_parser/cot.c	\
				$(PLAT_COMMON_PATH)/csf_tbbr.c
    # IMG PARSER here is CSF header parser
    include $(PLAT_DRIVERS_PATH)/auth/csf_hdr_parser/csf_hdr.mk
    BL2_SOURCES 	+=	$(CSF_HDR_SOURCES)

    SCP_BL2_PRE_TOOL_FILTER	:= CST_SCP_BL2
    BL31_PRE_TOOL_FILTER	:= CST_BL31
    BL32_PRE_TOOL_FILTER	:= CST_BL32
    BL33_PRE_TOOL_FILTER	:= CST_BL33
else

ifeq (${DISABLE_FUSE_WRITE}, 1)
$(eval $(call add_define,DISABLE_FUSE_WRITE))
endif

# For Mbedtls currently crypto is not supported via CAAM
# enable it when that support is there
    CAAM_INTEG		:= 0
    $(eval $(call add_define,MBEDTLS_X509))
    include drivers/auth/mbedtls/mbedtls_x509.mk

    # The algorithm is RSA when using Cryptocell crypto driver
    TF_MBEDTLS_KEY_ALG_ID		:=	TF_MBEDTLS_RSA

    # Needs to be set to drive mbed TLS configuration correctly
    $(eval $(call add_define,TF_MBEDTLS_KEY_ALG_ID))

    $(eval $(call add_define,KEY_SIZE))

    include $(PLAT_DRIVERS_PATH)/sfp/sfp.mk

    BL2_SOURCES	      += $(PLAT_AUTH_PATH)/tbbr/tbbr_cot.c \
			 $(PLAT_COMMON_PATH)/nxp_rotpk.S \
			 $(PLAT_COMMON_PATH)/x509_tbbr.c

#ROTPK key is embedded in BL2 image
ifeq (${ROT_KEY},)
ROT_KEY			= $(BUILD_PLAT)/rot_key.pem
endif

ifeq (${SAVE_KEYS},1)

ifeq (${TRUSTED_WORLD_KEY},)
TRUSTED_WORLD_KEY=${BUILD_PLAT}/trusted.pem
endif

ifeq (${NON_TRUSTED_WORLD_KEY},)
NON_TRUSTED_WORLD_KEY=${BUILD_PLAT}/non-trusted.pem
endif

ifeq (${BL31_KEY},)
BL31_KEY=${BUILD_PLAT}/soc.pem
endif

ifeq (${BL32_KEY},)
BL32_KEY=${BUILD_PLAT}/trusted_os.pem
endif

ifeq (${BL33_KEY},)
BL33_KEY=${BUILD_PLAT}/non-trusted_os.pem
endif

endif

ROTPK_HASH		= $(BUILD_PLAT)/rotpk_sha256.bin

$(eval $(call add_define_val,ROTPK_HASH,'"$(ROTPK_HASH)"'))

$(BUILD_PLAT)/bl2/nxp_rotpk.o: $(ROTPK_HASH)

certificates: $(ROT_KEY)
$(ROT_KEY): | $(BUILD_PLAT)
	@echo "  OPENSSL $@"
	@if [ ! -f $(ROT_KEY) ]; then \
		openssl genrsa 2048 > $@ 2>/dev/null; \
	fi

$(ROTPK_HASH): $(ROT_KEY)
	@echo "  OPENSSL $@"
	$(Q)openssl rsa -in $< -pubout -outform DER 2>/dev/null |\
	openssl dgst -sha256 -binary > $@ 2>/dev/null

endif #MBEDTLS_DIR

# If CAAM_INTEG is not defined (would be scenario with MBED TLS)
# include mbedtls_crypto
ifeq (${CAAM_INTEG},0)
    include drivers/auth/mbedtls/mbedtls_crypto.mk
else
    include $(PLAT_DRIVERS_PATH)/crypto/caam/src/auth/auth.mk
    BL2_SOURCES		+= ${AUTH_SOURCES}
endif

