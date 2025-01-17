/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2018 Fuzhou Rockchip Electronics Co., Ltd
 */

#ifndef _ASM_ARCH_CRU_rk1808_H
#define _ASM_ARCH_CRU_rk1808_H

#include <common.h>

#define MHz		1000000
#define KHz		1000
#define OSC_HZ		(24 * MHz)
#define APLL_HZ		(600 * MHz)
#define PCLK_PMU_HZ	(100 * MHz)

/* PX30 pll id */
enum rk1808_pll_id {
	APLL,
	DPLL,
	CPLL,
	GPLL,
	NPLL,
	PPLL,
	PLL_COUNT,
};

struct rk1808_clk_info {
	unsigned long id;
	char *name;
	bool is_cru;
};

/* Private data for the clock driver - used by rockchip_get_cru() */
struct rk1808_clk_priv {
	struct rk1808_cru *cru;
	ulong armclk_hz;
	ulong cpll_hz;
	ulong gpll_hz;
	ulong npll_hz;
};

struct rk1808_pll {
	unsigned int con0;
	unsigned int con1;
	unsigned int con2;
	unsigned int con3;
	unsigned int con4;
	unsigned int reserved0[3];
};

struct rk1808_cru {
	struct rk1808_pll pll[5];
	unsigned int mode;
	unsigned int misc;
	unsigned int misc1;
	unsigned int reserved2[1];
	unsigned int glb_cnt_th;
	unsigned int glb_rst_st;
	unsigned int glb_srst_fst;
	unsigned int glb_srst_snd;
	unsigned int glb_rst_con;
	unsigned int reserved3[7];
	unsigned int hwffc_con0;
	unsigned int reserved4;
	unsigned int hwffc_th;
	unsigned int hwffc_intst;
	unsigned int apll_con0_s;
	unsigned int apll_con1_s;
	unsigned int clksel_con0_s;
	unsigned int reserved5;
	unsigned int clksel_con[73];
	unsigned int reserved6[3];
	unsigned int clkgate_con[20];
	unsigned int ssgtbl[32];
	unsigned int softrst_con[16];
	unsigned int reserved7[(0x380 - 0x33c) / 4 - 1];
	unsigned int sdmmc_con[2];
	unsigned int sdio_con[2];
	unsigned int emmc_con[2];
	unsigned int reserved8[(0x400 - 0x394) / 4 - 1];
	unsigned int autocs_con[10];
	unsigned int reserved9[(0x4000 - 0x424) / 4 - 1];
	struct rk1808_pll pmu_pll;
	unsigned int pmu_mode;
	unsigned int reserved10[(0x4040 - 0x4020) / 4 - 1];
	unsigned int pmu_clksel_con[8];
	unsigned int reserved11[(0x4080 - 0x405c) / 4 - 1];
	unsigned int pmu_clkgate_con[2];
	unsigned int reserved12[(0x40c0 - 0x4084) / 4 - 1];
	unsigned int pmu_autocs_con[2];
};

check_member(rk1808_cru, pmu_autocs_con[0], 0x40c0);

#define RK1808_PLL_CON(x)		((x) * 0x4)
#define RK1808_MODE_CON			0xa0
#define RK1808_PMU_PLL_CON(x)		((x) * 0x4 + 0x4000)
#define RK1808_PMU_MODE_CON		0x4020

enum {
	/* CRU_CLK_SEL0_CON */
	CORE_ACLK_DIV_SHIFT		= 12,
	CORE_ACLK_DIV_MASK		= 0x07 << CORE_ACLK_DIV_SHIFT,
	CORE_DBG_DIV_SHIFT		= 8,
	CORE_DBG_DIV_MASK		= 0x03 << CORE_DBG_DIV_SHIFT,
	CORE_CLK_PLL_SEL_SHIFT		= 7,
	CORE_CLK_PLL_SEL_MASK		= 1 << CORE_CLK_PLL_SEL_SHIFT,
	CORE_CLK_PLL_SEL_APLL		= 0,
	CORE_CLK_PLL_SEL_GPLL,
	CORE_DIV_CON_SHIFT		= 0,
	CORE_DIV_CON_MASK		= 0x0f << CORE_DIV_CON_SHIFT,

	/* CRU_CLK_SEL4_CON */
	ACLK_VOP_PLL_SEL_GPLL		= 0,
	ACLK_VOP_PLL_SEL_CPLL		= 1,
	ACLK_VOP_PLL_SEL_SHIFT		= 7,
	ACLK_VOP_PLL_SEL_MASK		= 1 << ACLK_VOP_PLL_SEL_SHIFT,
	ACLK_VOP_DIV_CON_SHIFT		= 0,
	ACLK_VOP_DIV_CON_MASK		= 0x1f << ACLK_VOP_DIV_CON_SHIFT,
	HCLK_VOP_DIV_CON_SHIFT		= 8,
	HCLK_VOP_DIV_CON_MASK		= 0x1f << HCLK_VOP_DIV_CON_SHIFT,

	/* CRU_CLK_SEL5_CON */
	DCLK_VOPRAW_SEL_VOPRAW		= 0,
	DCLK_VOPRAW_SEL_VOPRAW_FRAC	= 1,
	DCLK_VOPRAW_SEL_XIN24M		= 2,
	DCLK_VOPRAW_SEL_SHIFT		= 14,
	DCLK_VOPRAW_SEL_MASK		= 3 << DCLK_VOPRAW_SEL_SHIFT,
	DCLK_VOPRAW_PLL_SEL_CPLL	= 0,
	DCLK_VOPRAW_PLL_SEL_GPLL	= 1,
	DCLK_VOPRAW_PLL_SEL_NPLL	= 2,
	DCLK_VOPRAW_PLL_SEL_SHIFT	= 10,
	DCLK_VOPRAW_PLL_SEL_MASK	= 3 << DCLK_VOPRAW_PLL_SEL_SHIFT,
	DCLK_VOPRAW_DIV_CON_SHIFT	= 0,
	DCLK_VOPRAW_DIV_CON_MASK	= 0xff << DCLK_VOPRAW_DIV_CON_SHIFT,

	/* CRU_CLK_SEL7_CON */
	DCLK_VOPLITE_SEL_VOPRAW		= 0,
	DCLK_VOPLITE_SEL_VOPRAW_FRAC	= 1,
	DCLK_VOPLITE_SEL_XIN24M		= 2,
	DCLK_VOPLITE_SEL_SHIFT		= 14,
	DCLK_VOPLITE_SEL_MASK		= 3 << DCLK_VOPLITE_SEL_SHIFT,
	DCLK_VOPLITE_PLL_SEL_CPLL	= 0,
	DCLK_VOPLITE_PLL_SEL_GPLL	= 1,
	DCLK_VOPLITE_PLL_SEL_NPLL	= 2,
	DCLK_VOPLITE_PLL_SEL_SHIFT	= 10,
	DCLK_VOPLITE_PLL_SEL_MASK	= 3 << DCLK_VOPLITE_PLL_SEL_SHIFT,
	DCLK_VOPLITE_DIV_CON_SHIFT	= 0,
	DCLK_VOPLITE_DIV_CON_MASK	= 0xff << DCLK_VOPLITE_DIV_CON_SHIFT,

	/* CRU_CLK_SEL19_CON */
	CLK_PERI_PLL_SEL_GPLL		= 0,
	CLK_PERI_PLL_SEL_CPLL		= 1,
	CLK_PERI_PLL_SEL_SHIFT		= 15,
	CLK_PERI_PLL_SEL_MASK		= 1 << CLK_PERI_PLL_SEL_SHIFT,
	LSCLK_PERI_DIV_CON_SHIFT	= 8,
	LSCLK_PERI_DIV_CON_MASK		= 0x1f << LSCLK_PERI_DIV_CON_SHIFT,
	MSCLK_PERI_DIV_CON_SHIFT	= 0,
	MSCLK_PERI_DIV_CON_MASK		= 0x1f << MSCLK_PERI_DIV_CON_SHIFT,

	/* CRU_CLKSEL24_CON */
	EMMC_PLL_SHIFT			= 14,
	EMMC_PLL_MASK			= 3 << EMMC_PLL_SHIFT,
	EMMC_SEL_GPLL			= 0,
	EMMC_SEL_CPLL,
	EMMC_SEL_NPLL,
	EMMC_SEL_24M,
	EMMC_DIV_SHIFT			= 0,
	EMMC_DIV_MASK			= 0xff << EMMC_DIV_SHIFT,

	/* CRU_CLKSEL25_CON */
	EMMC_CLK_SEL_SHIFT		= 15,
	EMMC_CLK_SEL_MASK		= 1 << EMMC_CLK_SEL_SHIFT,
	EMMC_CLK_SEL_EMMC		= 0,
	EMMC_CLK_SEL_EMMC_DIV50,
	EMMC_DIV50_SHIFT		= 0,
	EMMC_DIV50_MASK			= 0xff << EMMC_DIV_SHIFT,

	/* CRU_CLK_SEL27_CON */
	CLK_BUS_PLL_SEL_GPLL		= 0,
	CLK_BUS_PLL_SEL_CPLL		= 1,
	CLK_BUS_PLL_SEL_SHIFT		= 15,
	CLK_BUS_PLL_SEL_MASK		= 1 << CLK_BUS_PLL_SEL_SHIFT,
	HSCLK_BUS_DIV_CON_SHIFT		= 8,
	HSCLK_BUS_DIV_CON_MASK		= 0x1f << HSCLK_BUS_DIV_CON_SHIFT,

	/* CRU_CLK_SEL28_CON */
	MSCLK_BUS_DIV_CON_SHIFT		= 8,
	MSCLK_BUS_DIV_CON_MASK		= 0x1f << MSCLK_BUS_DIV_CON_SHIFT,
	LSCLK_BUS_DIV_CON_SHIFT		= 0,
	LSCLK_BUS_DIV_CON_MASK		= 0x1f << LSCLK_BUS_DIV_CON_SHIFT,

	/* CRU_CLK_SEL59_CON */
	CLK_I2C_PLL_SEL_GPLL		= 0,
	CLK_I2C_PLL_SEL_24M,
	CLK_I2C2_PLL_SEL_SHIFT		= 15,
	CLK_I2C2_DIV_CON_SHIFT		= 8,
	CLK_I2C2_DIV_CON_MASK		= 0x7f << CLK_I2C2_DIV_CON_SHIFT,
	CLK_I2C2_PLL_SEL_MASK		= 1 << CLK_I2C2_PLL_SEL_SHIFT,
	CLK_I2C1_PLL_SEL_SHIFT		= 7,
	CLK_I2C1_DIV_CON_SHIFT		= 0,
	CLK_I2C1_DIV_CON_MASK		= 0x7f,
	CLK_I2C1_PLL_SEL_MASK		= 1 << CLK_I2C1_PLL_SEL_SHIFT,

	/* CRU_CLK_SEL60_CON */
	CLK_SPI_PLL_SEL_GPLL		= 0,
	CLK_SPI_PLL_SEL_24M,
	CLK_SPI0_PLL_SEL_SHIFT		= 15,
	CLK_SPI0_DIV_CON_SHIFT		= 8,
	CLK_SPI0_DIV_CON_MASK		= 0x7f << CLK_SPI0_DIV_CON_SHIFT,
	CLK_SPI0_PLL_SEL_MASK		= 1 << CLK_SPI0_PLL_SEL_SHIFT,
	CLK_I2C3_PLL_SEL_SHIFT		= 7,
	CLK_I2C3_DIV_CON_SHIFT		= 0,
	CLK_I2C3_DIV_CON_MASK		= 0x7f,
	CLK_I2C3_PLL_SEL_MASK		= 1 << CLK_I2C3_PLL_SEL_SHIFT,

	/* CRU_CLK_SEL61_CON */
	CLK_SPI2_PLL_SEL_SHIFT		= 15,
	CLK_SPI2_DIV_CON_SHIFT		= 8,
	CLK_SPI2_DIV_CON_MASK		= 0x7f << CLK_SPI2_DIV_CON_SHIFT,
	CLK_SPI2_PLL_SEL_MASK		= 1 << CLK_SPI2_PLL_SEL_SHIFT,
	CLK_SPI1_PLL_SEL_SHIFT		= 7,
	CLK_SPI1_DIV_CON_SHIFT		= 0,
	CLK_SPI1_DIV_CON_MASK		= 0x7f,
	CLK_SPI1_PLL_SEL_MASK		= 1 << CLK_SPI1_PLL_SEL_SHIFT,

	/* CRU_CLK_SEL62_CON */
	CLK_TSADC_DIV_CON_SHIFT		= 0,
	CLK_TSADC_DIV_CON_MASK		= 0x3ff,

	/* CRU_CLK_SEL63_CON */
	CLK_SARADC_DIV_CON_SHIFT	= 0,
	CLK_SARADC_DIV_CON_MASK		= 0x3ff,

	/* CRU_CLK_SEL69_CON */
	CLK_PWM_PLL_SEL_GPLL		= 0,
	CLK_PWM_PLL_SEL_24M,
	CLK_PWM1_PLL_SEL_SHIFT		= 15,
	CLK_PWM1_DIV_CON_SHIFT		= 8,
	CLK_PWM1_DIV_CON_MASK		= 0x7f << CLK_PWM1_DIV_CON_SHIFT,
	CLK_PWM1_PLL_SEL_MASK		= 1 << CLK_PWM1_PLL_SEL_SHIFT,
	CLK_PWM0_PLL_SEL_SHIFT		= 7,
	CLK_PWM0_DIV_CON_SHIFT		= 0,
	CLK_PWM0_DIV_CON_MASK		= 0x7f,
	CLK_PWM0_PLL_SEL_MASK		= 1 << CLK_PWM0_PLL_SEL_SHIFT,

	/* CRU_CLK_SEL70_CON */
	CLK_PWM2_PLL_SEL_SHIFT		= 7,
	CLK_PWM2_DIV_CON_SHIFT		= 0,
	CLK_PWM2_DIV_CON_MASK		= 0x7f,
	CLK_PWM2_PLL_SEL_MASK		= 1 << CLK_PWM2_PLL_SEL_SHIFT,

	/* CRU_CLK_SEL71_CON */
	CLK_I2C5_PLL_SEL_SHIFT		= 15,
	CLK_I2C5_DIV_CON_SHIFT		= 8,
	CLK_I2C5_DIV_CON_MASK		= 0x7f << CLK_I2C5_DIV_CON_SHIFT,
	CLK_I2C5_PLL_SEL_MASK		= 1 << CLK_I2C5_PLL_SEL_SHIFT,
	CLK_I2C4_PLL_SEL_SHIFT		= 7,
	CLK_I2C4_DIV_CON_SHIFT		= 0,
	CLK_I2C4_DIV_CON_MASK		= 0x7f,
	CLK_I2C4_PLL_SEL_MASK		= 1 << CLK_I2C4_PLL_SEL_SHIFT,

	/* CRU_PMU_CLK_SEL7_CON */
	CLK_I2C0_PLL_SEL_PPLL		= 0,
	CLK_I2C0_PLL_SEL_SHIFT		= 15,
	CLK_I2C0_DIV_CON_SHIFT		= 8,
	CLK_I2C0_PLL_SEL_MASK		= 1 << CLK_I2C0_PLL_SEL_SHIFT,
	CLK_I2C0_DIV_CON_MASK		= 0x3f << CLK_I2C0_DIV_CON_SHIFT,

	/* PMUCRU_CLK_SEL0_CON */
	PCLK_PMU_DIV_CON_SHIFT		= 0,
	PCLK_PMU_DIV_CON_MASK		= 0x1f << PCLK_PMU_DIV_CON_SHIFT,
};
#endif
