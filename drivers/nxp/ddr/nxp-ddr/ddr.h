/*
 * Copyright 2016-2020 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef DDR_H
#define DDR_H

#include "ddr_io.h"
#include "dimm.h"
#include "immap.h"

#ifndef DDRC_NUM_CS
#define DDRC_NUM_CS 4
#endif

#include "opts.h"
#include "regs.h"
#include "utility.h"

#ifdef DDR_DEBUG
#define debug(...) INFO(__VA_ARGS__)
#else
#define debug(...) VERBOSE(__VA_ARGS__)
#endif

#ifndef DDRC_NUM_DIMM
#define DDRC_NUM_DIMM 1
#endif

#define CONFIG_CS_PER_SLOT \
	(DDRC_NUM_CS / DDRC_NUM_DIMM)

/* Record of register values computed */
struct ddr_cfg_regs {
	struct {
		unsigned int bnds;
		unsigned int config;
		unsigned int config_2;
	} cs[DDRC_NUM_CS];
	unsigned int dec[10];
	unsigned int timing_cfg[10];
	unsigned int sdram_cfg[3];
	unsigned int sdram_mode[16];
	unsigned int md_cntl;
	unsigned int interval;
	unsigned int data_init;
	unsigned int clk_cntl;
	unsigned int init_addr;
	unsigned int init_ext_addr;
	unsigned int zq_cntl;
	unsigned int wrlvl_cntl[3];
	unsigned int ddr_sr_cntr;
	unsigned int sdram_rcw[6];
	unsigned int dq_map[4];
	unsigned int eor;
	unsigned int cdr[2];
	unsigned int err_disable;
	unsigned int err_int_en;
	unsigned int tx_cfg[4];
	unsigned int debug[64];
};

struct ddr_conf {
	int dimm_in_use[DDRC_NUM_DIMM];
	int cs_in_use;	/* bitmask, bit 0 for cs0, bit 1 for cs1, etc. */
	int cs_on_dimm[DDRC_NUM_DIMM];	/* bitmask */
	unsigned long long cs_base_addr[DDRC_NUM_CS];
	unsigned long long cs_size[DDRC_NUM_CS];
	unsigned long long base_addr;
	unsigned long long total_mem;
};

struct ddr_info {
	unsigned long clk;
	unsigned long long mem_base;
	unsigned int num_ctlrs;
	unsigned int dimm_on_ctlr;
	struct dimm_params dimm;
	struct memctl_opt opt;
	struct ddr_conf conf;
	struct ddr_cfg_regs ddr_reg;
	struct ccsr_ddr *ddr[NUM_OF_DDRC];
	uint16_t *phy[NUM_OF_DDRC];
	int *spd_addr;
	unsigned int ip_rev;
	int warm_boot_flag;
};

struct rc_timing {
	unsigned int speed_bin;
	unsigned int clk_adj;
	unsigned int wrlvl;
};

struct board_timing {
	unsigned int rc;
	struct rc_timing const *p;
	unsigned int add1;
	unsigned int add2;
};

enum warm_boot {
	DDR_COLD_BOOT = 0,
	DDR_WARM_BOOT = 1,
	DDR_WRM_BOOT_NT_SUPPORTED = -1,
};

int disable_unused_ddrc(struct ddr_info *priv, int mask,
			uintptr_t nxp_ccn_hn_f0_addr);
int ddr_board_options(struct ddr_info *priv);
int compute_ddrc(const unsigned long clk,
		 const struct memctl_opt *popts,
		 const struct ddr_conf *conf,
		 struct ddr_cfg_regs *ddr,
		 const struct dimm_params *dimm_params,
		 const unsigned int ip_rev);
int compute_ddr_phy(struct ddr_info *priv);
int ddrc_set_regs(const unsigned long clk,
		  const struct ddr_cfg_regs *regs,
		  const struct ccsr_ddr *ddr,
		  int twopass);
int cal_board_params(struct ddr_info *priv,
		     const struct board_timing *dimm,
		     int len);
/* return bit mask of used DIMM(s) */
int ddr_get_ddr_params(struct dimm_params *pdimm, struct ddr_conf *conf);
long long dram_init(struct ddr_info *priv
#if defined(NXP_HAS_CCN504) || defined(NXP_HAS_CCN508)
		    , uintptr_t nxp_ccn_hn_f0_addr
#endif
#ifndef CONFIG_STATIC_DDR
		    , uintptr_t nxp_i2c_addr
#endif
		);
long long board_static_ddr(struct ddr_info *info);

#endif	/* DDR_H */
