/*
 * (C) Copyright 2017 Rockchip Electronics Co., Ltd
 *
 * SPDX-License-Identifier:	GPL-2.0
 */
#include <common.h>
#include <bitfield.h>
#include <clk-uclass.h>
#include <dm.h>
#include <div64.h>
#include <errno.h>
#include <syscon.h>
#include <asm/arch/clock.h>
#include <asm/arch/cru_rk3308.h>
#include <asm/arch/hardware.h>
#include <asm/io.h>
#include <dm/lists.h>
#include <dt-bindings/clock/rk3308-cru.h>

DECLARE_GLOBAL_DATA_PTR;

enum {
	VCO_MAX_HZ	= 3200U * 1000000,
	VCO_MIN_HZ	= 800 * 1000000,
	OUTPUT_MAX_HZ	= 3200U * 1000000,
	OUTPUT_MIN_HZ	= 24 * 1000000,
};

#define DIV_TO_RATE(input_rate, div)    ((input_rate) / ((div) + 1))

#define RK3308_CLK_DUMP(_id, _name)		\
{						\
	.id = _id,				\
	.name = _name,				\
}

#define RK3308_CPUCLK_RATE(_rate, _aclk_div, _pclk_div)		\
{								\
	.rate	= _rate##U,					\
	.aclk_div = _aclk_div,					\
	.pclk_div = _pclk_div,					\
}

static struct rockchip_pll_rate_table rk3308_pll_rates[] = {
	/* _mhz, _refdiv, _fbdiv, _postdiv1, _postdiv2, _dsmpd, _frac */
	RK3036_PLL_RATE(1300000000, 6, 325, 1, 1, 1, 0),
	RK3036_PLL_RATE(1200000000, 1, 50, 1, 1, 1, 0),
	RK3036_PLL_RATE(816000000, 1, 68, 2, 1, 1, 0),
	RK3036_PLL_RATE(748000000, 2, 187, 3, 1, 1, 0),
};

static struct rockchip_cpu_rate_table rk3308_cpu_rates[] = {
	RK3308_CPUCLK_RATE(1200000000, 1, 5),
	RK3308_CPUCLK_RATE(1008000000, 1, 5),
	RK3308_CPUCLK_RATE(816000000, 1, 3),
	RK3308_CPUCLK_RATE(600000000, 1, 3),
};

static const struct rk3308_clk_info clks_dump[] = {
	RK3308_CLK_DUMP(PLL_APLL, "apll"),
	RK3308_CLK_DUMP(PLL_DPLL, "dpll"),
	RK3308_CLK_DUMP(PLL_VPLL0, "vpll0"),
	RK3308_CLK_DUMP(PLL_VPLL1, "vpll1"),
	RK3308_CLK_DUMP(ACLK_BUS, "aclk_bus"),
	RK3308_CLK_DUMP(HCLK_BUS, "hclk_bus"),
	RK3308_CLK_DUMP(PCLK_BUS, "pclk_bus"),
	RK3308_CLK_DUMP(ACLK_PERI, "aclk_peri"),
	RK3308_CLK_DUMP(HCLK_PERI, "hclk_peri"),
	RK3308_CLK_DUMP(PCLK_PERI, "pclk_peri"),
	RK3308_CLK_DUMP(HCLK_AUDIO, "hclk_audio"),
	RK3308_CLK_DUMP(PCLK_AUDIO, "pclk_audio"),
};

static struct rockchip_pll_clock rk3308_pll_clks[] = {
	[APLL] = PLL(pll_rk3328, PLL_APLL, RK3308_PLL_CON(0),
		     RK3308_MODE_CON, 0, 10, 0, rk3308_pll_rates),
	[DPLL] = PLL(pll_rk3328, PLL_DPLL, RK3308_PLL_CON(8),
		     RK3308_MODE_CON, 2, 10, 0, NULL),
	[VPLL0] = PLL(pll_rk3328, PLL_VPLL0, RK3308_PLL_CON(16),
		      RK3308_MODE_CON, 4, 10, 0, NULL),
	[VPLL1] = PLL(pll_rk3328, PLL_VPLL1, RK3308_PLL_CON(24),
		      RK3308_MODE_CON, 6, 10, 0, NULL),
};

static ulong rk3308_armclk_set_clk(struct rk3308_clk_priv *priv, ulong hz)
{
	struct rk3308_cru *cru = priv->cru;
	const struct rockchip_cpu_rate_table *rate;
	ulong old_rate;

	rate = rockchip_get_cpu_settings(rk3308_cpu_rates, hz);
	if (!rate) {
		printf("%s unsupport rate\n", __func__);
		return -EINVAL;
	}

	/*
	 * select apll as cpu/core clock pll source and
	 * set up dependent divisors for PERI and ACLK clocks.
	 * core hz : apll = 1:1
	 */
	old_rate = rockchip_pll_get_rate(&rk3308_pll_clks[APLL],
					 priv->cru, APLL);
	if (old_rate > hz) {
		if (rockchip_pll_set_rate(&rk3308_pll_clks[APLL],
					  priv->cru, APLL, hz))
			return -EINVAL;
		rk_clrsetreg(&cru->clksel_con[0],
			     CORE_CLK_PLL_SEL_MASK | CORE_DIV_CON_MASK |
			     CORE_ACLK_DIV_MASK | CORE_DBG_DIV_MASK,
			     rate->aclk_div << CORE_ACLK_DIV_SHIFT |
			     rate->pclk_div << CORE_DBG_DIV_SHIFT |
			     CORE_CLK_PLL_SEL_APLL << CORE_CLK_PLL_SEL_SHIFT |
			     0 << CORE_DIV_CON_SHIFT);
	} else if (old_rate < hz) {
		rk_clrsetreg(&cru->clksel_con[0],
			     CORE_CLK_PLL_SEL_MASK | CORE_DIV_CON_MASK |
			     CORE_ACLK_DIV_MASK | CORE_DBG_DIV_MASK,
			     rate->aclk_div << CORE_ACLK_DIV_SHIFT |
			     rate->pclk_div << CORE_DBG_DIV_SHIFT |
			     CORE_CLK_PLL_SEL_APLL << CORE_CLK_PLL_SEL_SHIFT |
			     0 << CORE_DIV_CON_SHIFT);
		if (rockchip_pll_set_rate(&rk3308_pll_clks[APLL],
					  priv->cru, APLL, hz))
			return -EINVAL;
	}

	return rockchip_pll_get_rate(&rk3308_pll_clks[APLL], priv->cru, APLL);
}

static void rk3308_clk_get_pll_rate(struct rk3308_clk_priv *priv)
{
	if (!priv->dpll_hz)
		priv->dpll_hz = rockchip_pll_get_rate(&rk3308_pll_clks[DPLL],
						      priv->cru, DPLL);
	if (!priv->vpll0_hz)
		priv->vpll0_hz = rockchip_pll_get_rate(&rk3308_pll_clks[VPLL0],
						       priv->cru, VPLL0);
	if (!priv->vpll1_hz)
		priv->vpll1_hz = rockchip_pll_get_rate(&rk3308_pll_clks[VPLL1],
						       priv->cru, VPLL1);
}

static ulong rk3308_i2c_get_clk(struct clk *clk)
{
	struct rk3308_clk_priv *priv = dev_get_priv(clk->dev);
	struct rk3308_cru *cru = priv->cru;
	u32 div, con, con_id;

	switch (clk->id) {
	case SCLK_I2C0:
		con_id = 25;
		break;
	case SCLK_I2C1:
		con_id = 26;
		break;
	case SCLK_I2C2:
		con_id = 27;
		break;
	case SCLK_I2C3:
		con_id = 28;
		break;
	default:
		printf("do not support this i2c bus\n");
		return -EINVAL;
	}

	con = readl(&cru->clksel_con[con_id]);
	div = con >> CLK_I2C_DIV_CON_SHIFT & CLK_I2C_DIV_CON_MASK;

	return DIV_TO_RATE(priv->dpll_hz, div);
}

static ulong rk3308_i2c_set_clk(struct clk *clk, uint hz)
{
	struct rk3308_clk_priv *priv = dev_get_priv(clk->dev);
	struct rk3308_cru *cru = priv->cru;
	u32 src_clk_div, con_id;

	src_clk_div = DIV_ROUND_UP(priv->dpll_hz, hz);
	assert(src_clk_div - 1 <= 127);

	switch (clk->id) {
	case SCLK_I2C0:
		con_id = 25;
		break;
	case SCLK_I2C1:
		con_id = 26;
		break;
	case SCLK_I2C2:
		con_id = 27;
		break;
	case SCLK_I2C3:
		con_id = 28;
		break;
	default:
		printf("do not support this i2c bus\n");
		return -EINVAL;
	}
	rk_clrsetreg(&cru->clksel_con[con_id],
		     CLK_I2C_PLL_SEL_MASK | CLK_I2C_DIV_CON_MASK,
		     CLK_I2C_PLL_SEL_DPLL << CLK_I2C_PLL_SEL_SHIFT |
		     (src_clk_div - 1) << CLK_I2C_DIV_CON_SHIFT);

	return rk3308_i2c_get_clk(clk);
}

static ulong rk3308_mmc_get_clk(struct clk *clk)
{
	struct rk3308_clk_priv *priv = dev_get_priv(clk->dev);
	struct rk3308_cru *cru = priv->cru;
	u32 div, con, con_id;

	switch (clk->id) {
	case HCLK_SDMMC:
	case SCLK_SDMMC:
		con_id = 39;
		break;
	case HCLK_EMMC:
	case SCLK_EMMC:
	case SCLK_EMMC_SAMPLE:
		con_id = 41;
		break;
	default:
		return -EINVAL;
	}

	con = readl(&cru->clksel_con[con_id]);
	div = (con & EMMC_DIV_MASK) >> EMMC_DIV_SHIFT;

	if ((con & EMMC_PLL_MASK) >> EMMC_PLL_SHIFT
	    == EMMC_SEL_24M)
		return DIV_TO_RATE(OSC_HZ, div) / 2;
	else
		return DIV_TO_RATE(priv->vpll0_hz, div) / 2;
}

static ulong rk3308_mmc_set_clk(struct clk *clk, ulong set_rate)
{
	struct rk3308_clk_priv *priv = dev_get_priv(clk->dev);
	struct rk3308_cru *cru = priv->cru;
	int src_clk_div;
	u32 con_id;

	debug("%s %ld %ld\n", __func__, clk->id, set_rate);

	switch (clk->id) {
	case HCLK_SDMMC:
	case SCLK_SDMMC:
		con_id = 39;
		break;
	case HCLK_EMMC:
	case SCLK_EMMC:
		con_id = 41;
		break;
	default:
		return -EINVAL;
	}
	/* Select clk_sdmmc/emmc source from VPLL0 by default */
	/* mmc clock defaulg div 2 internal, need provide double in cru */
	src_clk_div = DIV_ROUND_UP(priv->vpll0_hz / 2, set_rate);

	if (src_clk_div > 127) {
		/* use 24MHz source for 400KHz clock */
		src_clk_div = DIV_ROUND_UP(OSC_HZ / 2, set_rate);
		rk_clrsetreg(&cru->clksel_con[con_id],
			     EMMC_PLL_MASK | EMMC_DIV_MASK | EMMC_CLK_SEL_MASK,
			     EMMC_CLK_SEL_EMMC << EMMC_CLK_SEL_SHIFT |
			     EMMC_SEL_24M << EMMC_PLL_SHIFT |
			     (src_clk_div - 1) << EMMC_DIV_SHIFT);
	} else {
		rk_clrsetreg(&cru->clksel_con[con_id],
			     EMMC_PLL_MASK | EMMC_DIV_MASK | EMMC_CLK_SEL_MASK,
			     EMMC_CLK_SEL_EMMC << EMMC_CLK_SEL_SHIFT |
			     EMMC_SEL_VPLL0 << EMMC_PLL_SHIFT |
			     (src_clk_div - 1) << EMMC_DIV_SHIFT);
	}

	return rk3308_mmc_get_clk(clk);
}

static ulong rk3308_saradc_get_clk(struct clk *clk)
{
	struct rk3308_clk_priv *priv = dev_get_priv(clk->dev);
	struct rk3308_cru *cru = priv->cru;
	u32 div, con;

	con = readl(&cru->clksel_con[34]);
	div = con >> CLK_SARADC_DIV_CON_SHIFT & CLK_SARADC_DIV_CON_MASK;

	return DIV_TO_RATE(OSC_HZ, div);
}

static ulong rk3308_saradc_set_clk(struct clk *clk, uint hz)
{
	struct rk3308_clk_priv *priv = dev_get_priv(clk->dev);
	struct rk3308_cru *cru = priv->cru;
	int src_clk_div;

	src_clk_div = DIV_ROUND_UP(OSC_HZ, hz);
	assert(src_clk_div - 1 <= 2047);

	rk_clrsetreg(&cru->clksel_con[34],
		     CLK_SARADC_DIV_CON_MASK,
		     (src_clk_div - 1) << CLK_SARADC_DIV_CON_SHIFT);

	return rk3308_saradc_get_clk(clk);
}

static ulong rk3308_tsadc_get_clk(struct clk *clk)
{
	struct rk3308_clk_priv *priv = dev_get_priv(clk->dev);
	struct rk3308_cru *cru = priv->cru;
	u32 div, con;

	con = readl(&cru->clksel_con[33]);
	div = con >> CLK_SARADC_DIV_CON_SHIFT & CLK_SARADC_DIV_CON_MASK;

	return DIV_TO_RATE(OSC_HZ, div);
}

static ulong rk3308_tsadc_set_clk(struct clk *clk, uint hz)
{
	struct rk3308_clk_priv *priv = dev_get_priv(clk->dev);
	struct rk3308_cru *cru = priv->cru;
	int src_clk_div;

	src_clk_div = DIV_ROUND_UP(OSC_HZ, hz);
	assert(src_clk_div - 1 <= 2047);

	rk_clrsetreg(&cru->clksel_con[33],
		     CLK_SARADC_DIV_CON_MASK,
		     (src_clk_div - 1) << CLK_SARADC_DIV_CON_SHIFT);

	return rk3308_tsadc_get_clk(clk);
}

static ulong rk3308_spi_get_clk(struct clk *clk)
{
	struct rk3308_clk_priv *priv = dev_get_priv(clk->dev);
	struct rk3308_cru *cru = priv->cru;
	u32 div, con, con_id;

	switch (clk->id) {
	case SCLK_SPI0:
		con_id = 30;
		break;
	case SCLK_SPI1:
		con_id = 31;
		break;
	case SCLK_SPI2:
		con_id = 32;
		break;
	default:
		printf("do not support this spi bus\n");
		return -EINVAL;
	}

	con = readl(&cru->clksel_con[con_id]);
	div = con >> CLK_SPI_DIV_CON_SHIFT & CLK_SPI_DIV_CON_MASK;

	return DIV_TO_RATE(priv->dpll_hz, div);
}

static ulong rk3308_spi_set_clk(struct clk *clk, uint hz)
{
	struct rk3308_clk_priv *priv = dev_get_priv(clk->dev);
	struct rk3308_cru *cru = priv->cru;
	u32 src_clk_div, con_id;

	src_clk_div = DIV_ROUND_UP(priv->dpll_hz, hz);
	assert(src_clk_div - 1 <= 127);

	switch (clk->id) {
	case SCLK_SPI0:
		con_id = 30;
		break;
	case SCLK_SPI1:
		con_id = 31;
		break;
	case SCLK_SPI2:
		con_id = 32;
		break;
	default:
		printf("do not support this spi bus\n");
		return -EINVAL;
	}

	rk_clrsetreg(&cru->clksel_con[con_id],
		     CLK_SPI_PLL_SEL_MASK | CLK_SPI_DIV_CON_MASK,
		     CLK_SPI_PLL_SEL_DPLL << CLK_SPI_PLL_SEL_SHIFT |
		     (src_clk_div - 1) << CLK_SPI_DIV_CON_SHIFT);

	return rk3308_spi_get_clk(clk);
}

static ulong rk3308_pwm_get_clk(struct clk *clk)
{
	struct rk3308_clk_priv *priv = dev_get_priv(clk->dev);
	struct rk3308_cru *cru = priv->cru;
	u32 div, con;

	con = readl(&cru->clksel_con[29]);
	div = con >> CLK_PWM_DIV_CON_SHIFT & CLK_PWM_DIV_CON_MASK;

	return DIV_TO_RATE(priv->dpll_hz, div);
}

static ulong rk3308_pwm_set_clk(struct clk *clk, uint hz)
{
	struct rk3308_clk_priv *priv = dev_get_priv(clk->dev);
	struct rk3308_cru *cru = priv->cru;
	int src_clk_div;

	src_clk_div = DIV_ROUND_UP(priv->dpll_hz, hz);
	assert(src_clk_div - 1 <= 127);

	rk_clrsetreg(&cru->clksel_con[29],
		     CLK_PWM_PLL_SEL_MASK | CLK_PWM_DIV_CON_MASK,
		     CLK_PWM_PLL_SEL_DPLL << CLK_PWM_PLL_SEL_SHIFT |
		     (src_clk_div - 1) << CLK_PWM_DIV_CON_SHIFT);

	return rk3308_pwm_get_clk(clk);
}

static ulong rk3308_vop_get_clk(struct clk *clk)
{
	struct rk3308_clk_priv *priv = dev_get_priv(clk->dev);
	struct rk3308_cru *cru = priv->cru;
	u32 div, pll_sel, vol_sel, con, parent;

	con = readl(&cru->clksel_con[8]);
	vol_sel = (con & DCLK_VOP_SEL_MASK) >> DCLK_VOP_SEL_SHIFT;
	pll_sel = (con & DCLK_VOP_PLL_SEL_MASK) >> DCLK_VOP_PLL_SEL_SHIFT;
	div = con & DCLK_VOP_DIV_MASK;

	if (vol_sel == DCLK_VOP_SEL_24M) {
		parent = OSC_HZ;
	} else if (vol_sel == DCLK_VOP_SEL_DIVOUT) {
		switch (pll_sel) {
		case DCLK_VOP_PLL_SEL_DPLL:
			parent = priv->dpll_hz;
			break;
		case DCLK_VOP_PLL_SEL_VPLL0:
			parent = priv->vpll0_hz;
			break;
		case DCLK_VOP_PLL_SEL_VPLL1:
			parent = priv->vpll0_hz;
			break;
		default:
			printf("do not support this vop pll sel\n");
			return -EINVAL;
		}
	} else {
		printf("do not support this vop sel\n");
		return -EINVAL;
	}

	return DIV_TO_RATE(parent, div);
}

static ulong rk3308_vop_set_clk(struct clk *clk, ulong hz)
{
	struct rk3308_clk_priv *priv = dev_get_priv(clk->dev);
	struct rk3308_cru *cru = priv->cru;
	ulong pll_rate, now, best_rate = 0;
	u32 i, div, best_div = 0, best_sel = 0;

	for (i = 0; i <= DCLK_VOP_PLL_SEL_VPLL1; i++) {
		switch (i) {
		case DCLK_VOP_PLL_SEL_DPLL:
			pll_rate = priv->dpll_hz;
			break;
		case DCLK_VOP_PLL_SEL_VPLL0:
			pll_rate = priv->vpll0_hz;
			break;
		case DCLK_VOP_PLL_SEL_VPLL1:
			pll_rate = priv->vpll1_hz;
			break;
		default:
			printf("do not support this vop pll sel\n");
			return -EINVAL;
		}

		div = DIV_ROUND_UP(pll_rate, hz);
		if (div > 255)
			continue;
		now = pll_rate / div;
		if (abs(hz - now) < abs(hz - best_rate)) {
			best_rate = now;
			best_div = div;
			best_sel = i;
		}
		debug("pll_rate=%lu, best_rate=%lu, best_div=%u, best_sel=%u\n",
		      pll_rate, best_rate, best_div, best_sel);
	}

	if (best_rate != hz && hz == OSC_HZ) {
		rk_clrsetreg(&cru->clksel_con[8],
			     DCLK_VOP_SEL_MASK,
			     DCLK_VOP_SEL_24M << DCLK_VOP_SEL_SHIFT);
	} else if (best_rate) {
		rk_clrsetreg(&cru->clksel_con[8],
			     DCLK_VOP_SEL_MASK | DCLK_VOP_PLL_SEL_MASK |
			     DCLK_VOP_DIV_MASK,
			     DCLK_VOP_SEL_DIVOUT << DCLK_VOP_SEL_SHIFT |
			     best_sel << DCLK_VOP_PLL_SEL_SHIFT |
			     (best_div - 1) << DCLK_VOP_DIV_SHIFT);
	} else {
		printf("do not support this vop freq\n");
		return -EINVAL;
	}

	return rk3308_vop_get_clk(clk);
}

static ulong rk3308_bus_get_clk(struct rk3308_clk_priv *priv, ulong clk_id)
{
	struct rk3308_cru *cru = priv->cru;
	u32 div, con, parent = priv->dpll_hz;

	switch (clk_id) {
	case ACLK_BUS:
		con = readl(&cru->clksel_con[5]);
		div = (con & BUS_ACLK_DIV_MASK) >> BUS_ACLK_DIV_SHIFT;
		break;
	case HCLK_BUS:
		con = readl(&cru->clksel_con[6]);
		div = (con & BUS_HCLK_DIV_MASK) >> BUS_HCLK_DIV_SHIFT;
		break;
	case PCLK_BUS:
		con = readl(&cru->clksel_con[6]);
		div = (con & BUS_PCLK_DIV_MASK) >> BUS_PCLK_DIV_SHIFT;
		break;
	default:
		return -ENOENT;
	}

	return DIV_TO_RATE(parent, div);
}

static ulong rk3308_bus_set_clk(struct rk3308_clk_priv *priv, ulong clk_id,
				ulong hz)
{
	struct rk3308_cru *cru = priv->cru;
	int src_clk_div;

	src_clk_div = DIV_ROUND_UP(priv->dpll_hz, hz);
	assert(src_clk_div - 1 <= 31);

	/*
	 * select dpll as pd_bus bus clock source and
	 * set up dependent divisors for PCLK/HCLK and ACLK clocks.
	 */
	switch (clk_id) {
	case ACLK_BUS:
		rk_clrsetreg(&cru->clksel_con[5],
			     BUS_PLL_SEL_MASK | BUS_ACLK_DIV_MASK,
			     BUS_PLL_SEL_DPLL << BUS_PLL_SEL_SHIFT |
			     (src_clk_div - 1) << BUS_ACLK_DIV_SHIFT);
		break;
	case HCLK_BUS:
		rk_clrsetreg(&cru->clksel_con[6],
			     BUS_HCLK_DIV_MASK,
			     (src_clk_div - 1) << BUS_HCLK_DIV_SHIFT);
		break;
	case PCLK_BUS:
		rk_clrsetreg(&cru->clksel_con[6],
			     BUS_PCLK_DIV_MASK,
			     (src_clk_div - 1) << BUS_PCLK_DIV_SHIFT);
		break;
	default:
		printf("do not support this bus freq\n");
		return -EINVAL;
	}

	return rk3308_bus_get_clk(priv, clk_id);
}

static ulong rk3308_peri_get_clk(struct rk3308_clk_priv *priv, ulong clk_id)
{
	struct rk3308_cru *cru = priv->cru;
	u32 div, con, parent = priv->dpll_hz;

	switch (clk_id) {
	case ACLK_PERI:
		con = readl(&cru->clksel_con[36]);
		div = (con & PERI_ACLK_DIV_MASK) >> PERI_ACLK_DIV_SHIFT;
		break;
	case HCLK_PERI:
		con = readl(&cru->clksel_con[37]);
		div = (con & PERI_HCLK_DIV_MASK) >> PERI_HCLK_DIV_SHIFT;
		break;
	case PCLK_PERI:
		con = readl(&cru->clksel_con[37]);
		div = (con & PERI_PCLK_DIV_MASK) >> PERI_PCLK_DIV_SHIFT;
		break;
	default:
		return -ENOENT;
	}

	return DIV_TO_RATE(parent, div);
}

static ulong rk3308_peri_set_clk(struct rk3308_clk_priv *priv, ulong clk_id,
				 ulong hz)
{
	struct rk3308_cru *cru = priv->cru;
	int src_clk_div;

	src_clk_div = DIV_ROUND_UP(priv->dpll_hz, hz);
	assert(src_clk_div - 1 <= 31);

	/*
	 * select dpll as pd_peri bus clock source and
	 * set up dependent divisors for PCLK/HCLK and ACLK clocks.
	 */
	switch (clk_id) {
	case ACLK_PERI:
		rk_clrsetreg(&cru->clksel_con[36],
			     PERI_PLL_SEL_MASK | PERI_ACLK_DIV_MASK,
			     PERI_PLL_DPLL << PERI_PLL_SEL_SHIFT |
			     (src_clk_div - 1) << PERI_ACLK_DIV_SHIFT);
		break;
	case HCLK_PERI:
		rk_clrsetreg(&cru->clksel_con[37],
			     PERI_HCLK_DIV_MASK,
			     (src_clk_div - 1) << PERI_HCLK_DIV_SHIFT);
		break;
	case PCLK_PERI:
		rk_clrsetreg(&cru->clksel_con[37],
			     PERI_PCLK_DIV_MASK,
			     (src_clk_div - 1) << PERI_PCLK_DIV_SHIFT);
		break;
	default:
		printf("do not support this peri freq\n");
		return -EINVAL;
	}

	return rk3308_peri_get_clk(priv, clk_id);
}

static ulong rk3308_audio_get_clk(struct rk3308_clk_priv *priv, ulong clk_id)
{
	struct rk3308_cru *cru = priv->cru;
	u32 div, con, parent = priv->vpll0_hz;

	switch (clk_id) {
	case HCLK_AUDIO:
		con = readl(&cru->clksel_con[45]);
		div = (con & AUDIO_HCLK_DIV_MASK) >> AUDIO_HCLK_DIV_SHIFT;
		break;
	case PCLK_AUDIO:
		con = readl(&cru->clksel_con[45]);
		div = (con & AUDIO_PCLK_DIV_MASK) >> AUDIO_PCLK_DIV_SHIFT;
		break;
	default:
		return -ENOENT;
	}

	return DIV_TO_RATE(parent, div);
}

static ulong rk3308_audio_set_clk(struct rk3308_clk_priv *priv, ulong clk_id,
				  ulong hz)
{
	struct rk3308_cru *cru = priv->cru;
	int src_clk_div;

	src_clk_div = DIV_ROUND_UP(priv->vpll0_hz, hz);
	assert(src_clk_div - 1 <= 31);

	/*
	 * select vpll0 as audio bus clock source and
	 * set up dependent divisors for HCLK and PCLK clocks.
	 */
	switch (clk_id) {
	case HCLK_AUDIO:
		rk_clrsetreg(&cru->clksel_con[45],
			     AUDIO_PLL_SEL_MASK | AUDIO_HCLK_DIV_MASK,
			     AUDIO_PLL_VPLL0 << AUDIO_PLL_SEL_SHIFT |
			     (src_clk_div - 1) << AUDIO_HCLK_DIV_SHIFT);
		break;
	case PCLK_AUDIO:
		rk_clrsetreg(&cru->clksel_con[45],
			     AUDIO_PLL_SEL_MASK | AUDIO_PCLK_DIV_MASK,
			     AUDIO_PLL_VPLL0 << AUDIO_PLL_SEL_SHIFT |
			     (src_clk_div - 1) << AUDIO_PCLK_DIV_SHIFT);
		break;
	default:
		printf("do not support this audio freq\n");
		return -EINVAL;
	}

	return rk3308_peri_get_clk(priv, clk_id);
}

static ulong rk3308_clk_get_rate(struct clk *clk)
{
	struct rk3308_clk_priv *priv = dev_get_priv(clk->dev);
	ulong rate = 0;

	debug("%s id:%ld\n", __func__, clk->id);

	switch (clk->id) {
	case PLL_APLL:
	case ARMCLK:
		rate = rockchip_pll_get_rate(&rk3308_pll_clks[APLL],
					     priv->cru, APLL);
		break;
	case PLL_DPLL:
		rate = rockchip_pll_get_rate(&rk3308_pll_clks[DPLL],
					     priv->cru, DPLL);
		break;
	case PLL_VPLL0:
		rate = rockchip_pll_get_rate(&rk3308_pll_clks[VPLL0],
					     priv->cru, VPLL0);
		break;
	case PLL_VPLL1:
		rate = rockchip_pll_get_rate(&rk3308_pll_clks[VPLL1],
					     priv->cru, VPLL1);
		break;
	case HCLK_SDMMC:
	case HCLK_EMMC:
	case SCLK_SDMMC:
	case SCLK_EMMC:
	case SCLK_EMMC_SAMPLE:
		rate = rk3308_mmc_get_clk(clk);
		break;
	case SCLK_I2C0:
	case SCLK_I2C1:
	case SCLK_I2C2:
	case SCLK_I2C3:
		rate = rk3308_i2c_get_clk(clk);
		break;
	case SCLK_SARADC:
		rate = rk3308_saradc_get_clk(clk);
		break;
	case SCLK_TSADC:
		rate = rk3308_tsadc_get_clk(clk);
		break;
	case SCLK_SPI0:
	case SCLK_SPI1:
		rate = rk3308_spi_get_clk(clk);
		break;
	case SCLK_PWM:
		rate = rk3308_pwm_get_clk(clk);
		break;
	case DCLK_VOP:
		rate = rk3308_vop_get_clk(clk);
		break;
	case ACLK_BUS:
	case HCLK_BUS:
	case PCLK_BUS:
		rate = rk3308_bus_get_clk(priv, clk->id);
		break;
	case ACLK_PERI:
	case HCLK_PERI:
	case PCLK_PERI:
		rate = rk3308_peri_get_clk(priv, clk->id);
		break;
	case HCLK_AUDIO:
	case PCLK_AUDIO:
		rate = rk3308_audio_get_clk(priv, clk->id);
		break;
	default:
		return -ENOENT;
	}

	return rate;
}

static ulong rk3308_clk_set_rate(struct clk *clk, ulong rate)
{
	struct rk3308_clk_priv *priv = dev_get_priv(clk->dev);
	ulong ret = 0;

	debug("%s %ld %ld\n", __func__, clk->id, rate);

	switch (clk->id) {
	case PLL_DPLL:
		ret = rockchip_pll_set_rate(&rk3308_pll_clks[DPLL], priv->cru,
					    DPLL, rate);
		priv->dpll_hz = rockchip_pll_get_rate(&rk3308_pll_clks[DPLL],
						      priv->cru, DPLL);
		break;
	case ARMCLK:
		if (priv->armclk_hz)
			rk3308_armclk_set_clk(priv, rate);
		priv->armclk_hz = rate;
		break;
	case HCLK_SDMMC:
	case HCLK_EMMC:
	case SCLK_SDMMC:
	case SCLK_EMMC:
		ret = rk3308_mmc_set_clk(clk, rate);
		break;
	case SCLK_I2C0:
	case SCLK_I2C1:
	case SCLK_I2C2:
	case SCLK_I2C3:
		ret = rk3308_i2c_set_clk(clk, rate);
		break;
	case SCLK_SARADC:
		ret = rk3308_saradc_set_clk(clk, rate);
		break;
	case SCLK_TSADC:
		ret = rk3308_tsadc_set_clk(clk, rate);
		break;
	case SCLK_SPI0:
	case SCLK_SPI1:
		ret = rk3308_spi_set_clk(clk, rate);
		break;
	case SCLK_PWM:
		ret = rk3308_pwm_set_clk(clk, rate);
		break;
	case DCLK_VOP:
		ret = rk3308_vop_set_clk(clk, rate);
		break;
	case ACLK_BUS:
	case HCLK_BUS:
	case PCLK_BUS:
		rate = rk3308_bus_set_clk(priv, clk->id, rate);
		break;
	case ACLK_PERI:
	case HCLK_PERI:
	case PCLK_PERI:
		rate = rk3308_peri_set_clk(priv, clk->id, rate);
		break;
	case HCLK_AUDIO:
	case PCLK_AUDIO:
		rate = rk3308_audio_set_clk(priv, clk->id, rate);
		break;
	default:
		return -ENOENT;
	}

	return ret;
}

#define ROCKCHIP_MMC_DELAY_SEL		BIT(11)
#define ROCKCHIP_MMC_DEGREE_OFFSET	1
#define ROCKCHIP_MMC_DEGREE_MASK	(0x3 << ROCKCHIP_MMC_DEGREE_OFFSET)
#define ROCKCHIP_MMC_DELAYNUM_OFFSET	3
#define ROCKCHIP_MMC_DELAYNUM_MASK	(0xff << ROCKCHIP_MMC_DELAYNUM_OFFSET)

#define PSECS_PER_SEC 1000000000000LL
/*
 * Each fine delay is between 44ps-77ps. Assume each fine delay is 60ps to
 * simplify calculations. So 45degs could be anywhere between 33deg and 57.8deg.
 */
#define ROCKCHIP_MMC_DELAY_ELEMENT_PSEC 60

int rockchip_mmc_get_phase(struct clk *clk)
{
	struct rk3308_clk_priv *priv = dev_get_priv(clk->dev);
	struct rk3308_cru *cru = priv->cru;
	u32 raw_value, delay_num;
	u16 degrees = 0;
	ulong rate;

	rate = rk3308_clk_get_rate(clk);

	if (rate < 0)
		return rate;

	if (clk->id == SCLK_EMMC_SAMPLE)
		raw_value = readl(&cru->emmc_con[1]);
	else
		raw_value = readl(&cru->sdmmc_con[1]);

	raw_value &= ROCKCHIP_MMC_DEGREE_MASK;
	degrees = (raw_value >>  ROCKCHIP_MMC_DEGREE_OFFSET) * 90;

	if (raw_value & ROCKCHIP_MMC_DELAY_SEL) {
		/* degrees/delaynum * 10000 */
		unsigned long factor = (ROCKCHIP_MMC_DELAY_ELEMENT_PSEC / 10) *
					36 * (rate / 1000000);

		delay_num = (raw_value & ROCKCHIP_MMC_DELAYNUM_MASK);
		delay_num >>= ROCKCHIP_MMC_DELAYNUM_OFFSET;
		degrees += DIV_ROUND_CLOSEST(delay_num * factor, 10000);
	}

	return degrees % 360;

}

int rockchip_mmc_set_phase(struct clk *clk, u32 degrees)
{
	struct rk3308_clk_priv *priv = dev_get_priv(clk->dev);
	struct rk3308_cru *cru = priv->cru;
	u8 nineties, remainder, delay_num;
	u32 raw_value, delay;
	ulong rate;

	rate = rk3308_clk_get_rate(clk);

	if (rate < 0)
		return rate;

	nineties = degrees / 90;
	remainder = (degrees % 90);

	/*
	 * Convert to delay; do a little extra work to make sure we
	 * don't overflow 32-bit / 64-bit numbers.
	 */
	delay = 10000000; /* PSECS_PER_SEC / 10000 / 10 */
	delay *= remainder;
	delay = DIV_ROUND_CLOSEST(delay, (rate / 1000) * 36 *
				(ROCKCHIP_MMC_DELAY_ELEMENT_PSEC / 10));

	delay_num = (u8)min_t(u32, delay, 255);

	raw_value = delay_num ? ROCKCHIP_MMC_DELAY_SEL : 0;
	raw_value |= delay_num << ROCKCHIP_MMC_DELAYNUM_OFFSET;
	raw_value |= nineties << ROCKCHIP_MMC_DEGREE_OFFSET;

	if (clk->id == SCLK_EMMC_SAMPLE)
		writel(raw_value | 0xffff0000, &cru->emmc_con[1]);
	else
		writel(raw_value | 0xffff0000, &cru->sdmmc_con[1]);

	debug("mmc set_phase(%d) delay_nums=%u reg=%#x actual_degrees=%d\n",
	      degrees, delay_num, raw_value, rockchip_mmc_get_phase(clk));

	return 0;

}

static int rk3308_clk_get_phase(struct clk *clk)
{
	int ret;

	switch (clk->id) {
	case SCLK_EMMC_SAMPLE:
	case SCLK_SDMMC_SAMPLE:
		ret = rockchip_mmc_get_phase(clk);
		break;
	default:
		return -ENOENT;
	}

	return ret;
}

static int rk3308_clk_set_phase(struct clk *clk, int degrees)
{
	int ret;

	switch (clk->id) {
	case SCLK_EMMC_SAMPLE:
	case SCLK_SDMMC_SAMPLE:
		ret = rockchip_mmc_set_phase(clk, degrees);
		break;
	default:
		return -ENOENT;
	}

	return ret;
}

static struct clk_ops rk3308_clk_ops = {
	.get_rate = rk3308_clk_get_rate,
	.set_rate = rk3308_clk_set_rate,
	.get_phase	= rk3308_clk_get_phase,
	.set_phase	= rk3308_clk_set_phase,
};

static void rk3308_clk_init(struct udevice *dev)
{
	struct rk3308_clk_priv *priv = dev_get_priv(dev);
	int ret;

	if (rockchip_pll_get_rate(&rk3308_pll_clks[APLL],
				  priv->cru, APLL) != APLL_HZ) {
		ret = rk3308_armclk_set_clk(priv, APLL_HZ);
		if (ret < 0)
			printf("%s failed to set armclk rate\n", __func__);
	}

	rk3308_clk_get_pll_rate(priv);

	rk3308_bus_set_clk(priv, ACLK_BUS, BUS_ACLK_HZ);
	rk3308_bus_set_clk(priv, HCLK_BUS, BUS_HCLK_HZ);
	rk3308_bus_set_clk(priv, PCLK_BUS, BUS_PCLK_HZ);

	rk3308_peri_set_clk(priv, ACLK_PERI, PERI_ACLK_HZ);
	rk3308_peri_set_clk(priv, HCLK_PERI, PERI_HCLK_HZ);
	rk3308_peri_set_clk(priv, PCLK_PERI, PERI_PCLK_HZ);

	rk3308_audio_set_clk(priv, HCLK_AUDIO, AUDIO_HCLK_HZ);
	rk3308_audio_set_clk(priv, PCLK_AUDIO, AUDIO_PCLK_HZ);
}

static int rk3308_clk_probe(struct udevice *dev)
{
	int ret;

	rk3308_clk_init(dev);

	/* Process 'assigned-{clocks/clock-parents/clock-rates}' properties */
	ret = clk_set_defaults(dev);
	if (ret)
		debug("%s clk_set_defaults failed %d\n", __func__, ret);

	return 0;
}

static int rk3308_clk_ofdata_to_platdata(struct udevice *dev)
{
	struct rk3308_clk_priv *priv = dev_get_priv(dev);

	priv->cru = dev_read_addr_ptr(dev);

	return 0;
}

static int rk3308_clk_bind(struct udevice *dev)
{
	int ret;
	struct udevice *sys_child, *sf_child;
	struct sysreset_reg *priv;
	struct softreset_reg *sf_priv;

	/* The reset driver does not have a device node, so bind it here */
	ret = device_bind_driver(dev, "rockchip_sysreset", "sysreset",
				 &sys_child);
	if (ret) {
		debug("Warning: No sysreset driver: ret=%d\n", ret);
	} else {
		priv = malloc(sizeof(struct sysreset_reg));
		priv->glb_srst_fst_value = offsetof(struct rk3308_cru,
						    glb_srst_fst);
		priv->glb_srst_snd_value = offsetof(struct rk3308_cru,
						    glb_srst_snd);
		sys_child->priv = priv;
	}

	ret = device_bind_driver_to_node(dev, "rockchip_reset", "reset",
					 dev_ofnode(dev), &sf_child);
	if (ret) {
		debug("Warning: No rockchip reset driver: ret=%d\n", ret);
	} else {
		sf_priv = malloc(sizeof(struct softreset_reg));
		sf_priv->sf_reset_offset = offsetof(struct rk3308_cru,
						    softrst_con[0]);
		sf_priv->sf_reset_num = 12;
		sf_child->priv = sf_priv;
	}

	return 0;
}

static const struct udevice_id rk3308_clk_ids[] = {
	{ .compatible = "rockchip,rk3308-cru" },
	{ }
};

U_BOOT_DRIVER(rockchip_rk3308_cru) = {
	.name		= "rockchip_rk3308_cru",
	.id		= UCLASS_CLK,
	.of_match	= rk3308_clk_ids,
	.priv_auto_alloc_size = sizeof(struct rk3308_clk_priv),
	.ofdata_to_platdata = rk3308_clk_ofdata_to_platdata,
	.ops		= &rk3308_clk_ops,
	.bind		= rk3308_clk_bind,
	.probe		= rk3308_clk_probe,
};

/**
 * soc_clk_dump() - Print clock frequencies
 * Returns zero on success
 *
 * Implementation for the clk dump command.
 */
int soc_clk_dump(void)
{
	struct udevice *cru_dev;
	const struct rk3308_clk_info *clk_dump;
	struct clk clk;
	unsigned long clk_count = ARRAY_SIZE(clks_dump);
	unsigned long rate;
	int i, ret;

	ret = uclass_get_device_by_driver(UCLASS_CLK,
					  DM_GET_DRIVER(rockchip_rk3308_cru),
					  &cru_dev);
	if (ret) {
		printf("%s failed to get cru device\n", __func__);
		return ret;
	}

	printf("CLK:\n");
	for (i = 0; i < clk_count; i++) {
		clk_dump = &clks_dump[i];
		if (clk_dump->name) {
			clk.id = clk_dump->id;
			ret = clk_request(cru_dev, &clk);
			if (ret < 0)
				return ret;

			rate = clk_get_rate(&clk);
			clk_free(&clk);
			if (i == 0) {
				if (rate < 0)
					printf("%s %s\n", clk_dump->name,
					       "unknown");
				else
					printf("%s %lu KHz\n", clk_dump->name,
					       rate / 1000);
			} else {
				if (rate < 0)
					printf("%s %s\n", clk_dump->name,
					       "unknown");
				else
					printf("%s %lu KHz\n", clk_dump->name,
					       rate / 1000);
			}
		}
	}

	return 0;
}
