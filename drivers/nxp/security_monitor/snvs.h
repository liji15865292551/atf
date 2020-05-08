/*
 * Copyright 2016-2020 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef SNVS_H
#define SNVS_H


/* SSM_ST field in SNVS status reg */
#define HPSTS_CHECK_SSM_ST	0x900	/* SNVS is in check state */
#define HPSTS_NON_SECURE_SSM_ST	0xb00	/* SNVS is in non secure state */
#define HPSTS_TRUST_SSM_ST	0xd00	/* SNVS is in trusted state */
#define HPSTS_SECURE_SSM_ST	0xf00	/* SNVS is in secure state */
#define HPSTS_SOFT_FAIL_SSM_ST	0x300	/* SNVS is in soft fail state */
#define HPSTS_MASK_SSM_ST	0xf00	/* SSM_ST field mask in SNVS reg */

/* SNVS register bits */
#define HPCOM_SW_SV		0x100	/* Security Violation bit */
#define HPCOM_SW_FSV		0x200	/* Fatal Security Violation bit */
#define HPCOM_SSM_ST		0x1	/* SSM_ST field in SNVS command reg */
#define HPCOM_SSM_ST_DIS	0x2	/* Disable Secure to Trusted State */
#define HPCOM_SSM_SFNS_DIS	0x4	/* Disable Soft Fail to Non-Secure */

#define NXP_LP_GPR0_OFFSET	0x90
#define NXP_LPCR_OFFSET		0x38
#define NXP_GPR_Z_DIS_BIT	24

#ifdef NXP_COINED_BB

#define NXP_LPGPR_ZEROTH_BIT		0

#ifdef NXP_WARM_BOOT
#define NXP_WARM_RST_FLAG_BIT		NXP_LPGPR_ZEROTH_BIT
#define NXP_WARM_RST_FLAG		(1 << NXP_WARM_RST_FLAG_BIT)
#endif

#endif	/* NXP_COINED_BB */

#ifndef __ASSEMBLER__

#include <stdbool.h>

#include <endian.h>
#include <lib/mmio.h>

struct snvs_regs {
	uint32_t reserved1;
	uint32_t hp_com;		/* 0x04 SNVS_HP Command Register */
	uint32_t reserved2[3];
	uint32_t hp_stat;		/* 0x14 SNVS_HP Status Register */
};

#ifdef NXP_SNVS_BE
#define snvs_read32(a)           bswap32(mmio_read_32((uintptr_t)(a)))
#define snvs_write32(a, v)       mmio_write_32((uintptr_t)(a), bswap32((v)))
#elif defined(NXP_SNVS_LE)
#define snvs_read32(a)           mmio_read_32((uintptr_t)(a))
#define snvs_write32(a, v)       mmio_write_32((uintptr_t)(a), (v))
#else
#error Please define CCSR SNVS register endianness
#endif

uint32_t get_snvs_state(uintptr_t nxp_snvs_addr);
void transition_snvs_non_secure(uintptr_t nxp_snvs_addr);
void transition_snvs_soft_fail(uintptr_t nxp_snvs_addr);
uint32_t transition_snvs_trusted(uintptr_t nxp_snvs_addr);
uint32_t transition_snvs_secure(uintptr_t nxp_snvs_addr);

uint32_t snvs_read_lp_gpr(uintptr_t nxp_snvs_addr,
			  uint32_t offset, uint32_t bit_pos);
void snvs_write_lp_gpr(uintptr_t nxp_snvs_addr, uint32_t offset,
			   uint32_t bit_pos, bool flag_val);

void snvs_disable_zeroize_lp_gpr(uintptr_t nxp_snvs_addr);
#ifdef NXP_WARM_BOOT
uint32_t snvs_warm_boot_status(uintptr_t nxp_snvs_addr);
void snvs_clr_warm_boot_flag(uintptr_t nxp_snvs_addr);
#endif

#endif	/*  __ASSEMBLER__  */

#endif	/* SNVS_H  */
