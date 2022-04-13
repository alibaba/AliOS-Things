/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <FreeRTOS.h>
#include <semphr.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <mt_printf.h>
#include <arch_timer.h>
#include <clk.h>
#include <mtk_clk_provider.h>
#include <mtk_clk_pd_basic.h>

static SemaphoreHandle_t xSemaphore_clk;
static bool mutex_initialized = false;

static void mtk_clk_lock(void)
{
    if (!mutex_initialized) {
        xSemaphore_clk = xSemaphoreCreateMutex();
        mutex_initialized = true;
    }

    xSemaphoreTake(xSemaphore_clk, pdMS_TO_TICKS(10));
}

static void mtk_clk_unlock(void)
{
    xSemaphoreGive(xSemaphore_clk);
}

struct mtk_clk_provider mtk_clk_providers[NR_PROVIDER] = {{0}};

struct mtk_clk_provider *get_mtk_clk_pvds(void)
{
    return mtk_clk_providers;
}

static struct mtk_clk *clk_from_name_nolock(const char *name)
{
    int i, j;

    if (!name)
        return NULL;

    for (i = 0; i < NR_PROVIDER; i++) {
        struct mtk_clk_data *clk_data = mtk_clk_providers[i].data;

        if (!clk_data)
            continue;

        for (j = 0; j < clk_data->nclks; j++) {
            struct mtk_clk *mck = clk_data->mcks[j];

            if (!mck || !mck->name)
                continue;

            if (!strcmp(name, mck->name))
                return mck;
        }
    }

    return NULL;
}

struct clk *clk_from_name(const char *name)
{
    struct mtk_clk *mck;

    mtk_clk_lock();
    mck = clk_from_name_nolock(name);
    mtk_clk_unlock();

    return clk_from_mck(mck);
}

struct clk *clk_get(int id)
{
    struct mtk_clk_data *clk_data;
    int pvd_id, clk_id;

    pvd_id = (id & PVD_ID_MASK) >> PVD_ID_SHIFT;
    clk_id = (id & CLK_ID_MASK) >> CLK_ID_SHIFT;

    if (pvd_id >= NR_PROVIDER) {
        printf("clk_get: invalid clock provider index %u\n", pvd_id);
        return (void *) EINVAL;
    }

    clk_data = mtk_clk_providers[pvd_id].data;
    if (!clk_data) {
        printf("clk_get: clock provider %u not ready\n", pvd_id);
        return (void *) EDEFER;
    }

    if (clk_id >= clk_data->nclks) {
        printf("clk_get: invalid clock index %u\n", clk_id);
        return (void *) EINVAL;
    }

    return clk_from_mck(clk_data->mcks[clk_id]);
}

void clk_put(struct clk *clk)
{
    // Do nothing.
}

bool is_err_or_null_clk(struct clk *clk)
{
    return ((int)clk < MAX_ERRNO);
}

static void mtk_clk_parents_init(struct mtk_clk *mck)
{
    int i;

    if (mck->num_parents < 1)
        return;

    if (mck->num_parents == 1) {
        if (mck->p.parent)
            return;

        mck->p.parent = clk_from_name_nolock(mck->pn.parent_name);

        return;
    }

    if (mck->num_parents > 1) {
        if (!mck->p.parents) {
            mck->p.parents = pvPortMalloc(sizeof(*mck->p.parents) * mck->num_parents);
            if (mck->p.parents)
                memset(mck->p.parents, 0, (sizeof(*mck->p.parents) * mck->num_parents));
            else
                return;
        }

        for (i = 0; i < mck->num_parents; i++) {
            if (!mck->pn.parent_names[i] || mck->p.parents[i])
                continue;

            mck->p.parents[i] = clk_from_name_nolock(
                        mck->pn.parent_names[i]);
        }
    }
}

static struct mtk_clk *mtk_clk_get_parent_nolock(struct mtk_clk *mck)
{
    uint8_t pi;

    if (mck->num_parents < 1)
        return NULL;

    mtk_clk_parents_init(mck);

    if (mck->num_parents == 1)
        return mck->p.parent;

    /* mck->num_parents > 1 */

    if (!mck->ops->get_parent)
        return NULL;

    pi = mck->ops->get_parent(mck);

    if (pi >= mck->num_parents || !mck->p.parents)
        return NULL;

    return mck->p.parents[pi];
}

struct clk *clk_get_parent(struct clk *clk)
{
    struct mtk_clk *mck, *mck_p;

    if (!clk)
        return NULL;

    mtk_clk_lock();

    mck = mck_from_clk(clk);
    mck_p = mtk_clk_get_parent_nolock(mck);

    mtk_clk_unlock();

    return clk_from_mck(mck_p);
}

static unsigned long mtk_clk_get_rate_nolock(struct mtk_clk *mck)
{
    struct mtk_clk *clk_p = mtk_clk_get_parent_nolock(mck);
    unsigned long parent_rate =
            clk_p ? mtk_clk_get_rate_nolock(clk_p) : 0;

    if (mck->ops->recalc_rate)
        return mck->ops->recalc_rate(mck, parent_rate);

    return parent_rate;
}

unsigned long clk_get_rate(struct clk *clk)
{
    unsigned long r;

    if (!clk)
        return 0;

    mtk_clk_lock();
    r = mtk_clk_get_rate_nolock(mck_from_clk(clk));
    mtk_clk_unlock();

    return r;
}

static int mtk_clk_set_rate_nolock(struct mtk_clk *mck, unsigned long rate)
{
    struct mtk_clk *clk_p = mtk_clk_get_parent_nolock(mck);
    unsigned long parent_rate =
            clk_p ? mtk_clk_get_rate_nolock(clk_p) : 0;
    unsigned long new_prate = parent_rate;
    long rounded = rate;
    int r = EINVAL;

    if (rate == mtk_clk_get_rate_nolock(mck))
        return 0;

    if (mck->ops->round_rate)
        rounded = mck->ops->round_rate(mck, rate, &new_prate);
    else
        new_prate = rate;

    if (clk_p && new_prate != parent_rate)
        r = mtk_clk_set_rate_nolock(clk_p, new_prate);

    if (mck->ops->set_rate && !(mck->flags & CLK_SET_RATE_PARENT))
        return mck->ops->set_rate(mck, rounded, new_prate);

    return r;
}

static int mtk_clk_set_rate(struct mtk_clk *mck, unsigned long rate)
{
    int r;

    mtk_clk_lock();
    r = mtk_clk_set_rate_nolock(mck, rate);
    mtk_clk_unlock();

    return r;
}

int clk_set_rate(struct clk *clk, unsigned long rate)
{
    int r;
    struct mtk_clk *mck;

    if (!clk)
        return 0;

    mck = mck_from_clk(clk);

    if (mck->flags & CLK_SET_RATE_PARENT)
        r = clk_set_rate(clk_get_parent(clk), rate);
    else
        r = mtk_clk_set_rate(mck, rate);

    return r;
}

static int mtk_clk_enable_nolock(struct mtk_clk *mck)
{
    struct mtk_clk *clk_p = mtk_clk_get_parent_nolock(mck);

    if (++mck->enable_cnt != 1)
        return 0;

    if (clk_p)
        mtk_clk_enable_nolock(clk_p);

    if (mck->ops->enable)
        return mck->ops->enable(mck);

    return 0;
}

int clk_enable(struct clk *clk)
{
    int r;

    if (!clk)
        return EINVAL;

    mtk_clk_lock();
    r = mtk_clk_enable_nolock(mck_from_clk(clk));
    mtk_clk_unlock();

    return r;
}

static int mtk_clk_disable_nolock(struct mtk_clk *mck)
{
    struct mtk_clk *clk_p = mtk_clk_get_parent_nolock(mck);

    if (mck->enable_cnt <= 0)
        return EINVAL;

    if (--mck->enable_cnt != 0)
        return 0;

    if (mck->ops->disable)
        mck->ops->disable(mck);

    if (clk_p)
        mtk_clk_disable_nolock(clk_p);

    return 0;
}

int clk_disable(struct clk *clk)
{
    int r;

    if (!clk)
        return EINVAL;

    mtk_clk_lock();
    r = mtk_clk_disable_nolock(mck_from_clk(clk));
    mtk_clk_unlock();

    return r;
}

static int mtk_clk_set_parent_nolock(struct mtk_clk *mck, struct mtk_clk *mck_parent)
{
    int i;

    if (!mck->ops->set_parent)
        return EINVAL;

    mtk_clk_parents_init(mck);

    if (mck->num_parents == 1) {
        if (mck->p.parent != mck_parent)
            return EINVAL;

        return 0;
    }

    for (i = 0;
        i < mck->num_parents && mck_parent != mck->p.parents[i];
        i++);

    if (i < mck->num_parents) {
        struct mtk_clk *clk_p = mtk_clk_get_parent_nolock(mck);
        int r;

        if (mck->enable_cnt > 0)
            mtk_clk_enable_nolock(mck_parent);

        r = mck->ops->set_parent(mck, i);

        if (mck->enable_cnt > 0)
            mtk_clk_disable_nolock(clk_p);

        return r;
    }

    return EINVAL;
}

int clk_set_parent(struct clk *clk, struct clk *clk_parent)
{
    int r;

    if (!clk || !clk_parent)
        return EINVAL;

    mtk_clk_lock();
    r = mtk_clk_set_parent_nolock(mck_from_clk(clk),
            mck_from_clk(clk_parent));
    mtk_clk_unlock();

    return r;
}

struct mtk_clk_data *mtk_alloc_clk_data(unsigned int clk_num)
{
    int i;
    struct mtk_clk_data *clk_data;

    clk_data = pvPortMalloc(sizeof(*clk_data));
    if (!clk_data)
        return (void *) ENOMEM;

    clk_data->mcks = pvPortMalloc(clk_num * sizeof(*clk_data->mcks));
    if (!clk_data->mcks)
        goto err_out;

    clk_data->nclks = clk_num;

    for (i = 0; i < clk_num; i++)
        clk_data->mcks[i] = NULL;

    return clk_data;

err_out:
    vPortFree(clk_data);
    return (void *) ENOMEM;
}

int mtk_clk_add_provider(int pvd_id, const char *name, struct mtk_clk_data *clk_data)
{
    struct mtk_clk_provider *mcp = &mtk_clk_providers[pvd_id];

    if (pvd_id >= NR_PROVIDER) {
        printf("excede max provider!\n");
        return EINVAL;
    }

    mcp->name = name;
    mcp->data = clk_data;

    return 0;
}

void mtk_clk_disable_unused(void)
{
    int i, j;

    for (i = NR_PROVIDER; i > 0; i--) {
        struct mtk_clk_data *clk_data = mtk_clk_providers[i - 1].data;

        if (!clk_data)
            continue;

        for (j = clk_data->nclks; j > 0; j--) {
            struct mtk_clk *mck = clk_data->mcks[j - 1];
            int is_enabled = 0;

            if (!mck)
                continue;

            if (mck->ops->is_enabled != NULL)
                is_enabled = mck->ops->is_enabled(mck);

            if (is_enabled > 0 && mck->enable_cnt == 0 &&
                    mck->ops->disable)
                mck->ops->disable(mck);
        }
    }
}

/* fixed-rate clock support */

static unsigned long
mtk_clk_fixed_rate_recalc_rate(struct mtk_clk *mck, unsigned long parent_rate)
{
    struct mtk_clk_fixed_rate *fixed = fixed_from_mtk_clk(mck);

    return fixed->rate;
}

const struct mtk_clk_ops mtk_clk_fixed_rate_ops = {
    .recalc_rate    = mtk_clk_fixed_rate_recalc_rate,
};

/* fixed-factor clock support */

static unsigned long
mtk_clk_fixed_factor_recalc_rate(struct mtk_clk *mck, unsigned long parent_rate)
{
    struct mtk_clk_fixed_factor *factor = factor_from_mtk_clk(mck);
    unsigned long long r = (unsigned long long)parent_rate * factor->mult;

    r /= factor->div;

    return (unsigned long)r;
}

static long mtk_clk_fixed_factor_round_rate(struct mtk_clk *mck, unsigned long rate,
            unsigned long *parent_rate)
{
    struct mtk_clk_fixed_factor *factor = factor_from_mtk_clk(mck);

    *parent_rate = (rate / factor->mult) * factor->div;

    return *parent_rate / factor->div * factor->mult;
}

static int mtk_clk_fixed_factor_set_rate(struct mtk_clk *mck, unsigned long rate,
            unsigned long parent_rate)
{
    return 0;
}

const struct mtk_clk_ops mtk_clk_fixed_factor_ops = {
    .recalc_rate    = mtk_clk_fixed_factor_recalc_rate,
    .round_rate    = mtk_clk_fixed_factor_round_rate,
    .set_rate    = mtk_clk_fixed_factor_set_rate,
};

/* clock gate support */

static int mtk_clk_gate_set(struct mtk_clk_gate *cg, bool has_set)
{
    if (cg == NULL || cg->regs == NULL)
        return EINVAL;

    if (has_set)
        DRV_WriteReg32((cg->gate_base + cg->regs->set_ofs), BIT(cg->shift));
    else
        DRV_WriteReg32((cg->gate_base + cg->regs->sta_ofs),
            DRV_Reg32(cg->gate_base + cg->regs->sta_ofs) | BIT(cg->shift));

    return 0;
}

static int mtk_clk_gate_clr(struct mtk_clk_gate *cg, bool has_clr)
{
    if (cg == NULL || cg->regs == NULL)
        return EINVAL;

    if (has_clr)
        DRV_WriteReg32((cg->gate_base + cg->regs->clr_ofs), BIT(cg->shift));
    else
        DRV_WriteReg32((cg->gate_base + cg->regs->sta_ofs),
            DRV_Reg32(cg->gate_base + cg->regs->sta_ofs)& ~BIT(cg->shift));

    return 0;
}

static bool mtk_clk_gate_is_set(struct mtk_clk_gate *cg)
{
    uint32_t r;

    if (cg == NULL || cg->regs == NULL)
        return false;

    r = DRV_Reg32(cg->gate_base + cg->regs->sta_ofs);

    return !!(r & BIT(cg->shift));
}

static int mtk_clk_gate_enable(struct mtk_clk *mck)
{
    return mtk_clk_gate_clr(gate_from_mtk_clk(mck), false);
}

static void mtk_clk_gate_disable(struct mtk_clk *mck)
{
    mtk_clk_gate_set(gate_from_mtk_clk(mck), false);
}

static int mtk_clk_gate_is_enabled(struct mtk_clk *mck)
{
    return !mtk_clk_gate_is_set(gate_from_mtk_clk(mck));
}

static int mtk_clk_gate_inv_enable(struct mtk_clk *mck)
{
    return mtk_clk_gate_set(gate_from_mtk_clk(mck), false);
}

static void mtk_clk_gate_inv_disable(struct mtk_clk *mck)
{
    mtk_clk_gate_clr(gate_from_mtk_clk(mck), false);
}

static int mtk_clk_gate_inv_is_enabled(struct mtk_clk *mck)
{
    return mtk_clk_gate_is_set(gate_from_mtk_clk(mck));
}

static int mtk_clk_gate_setclr_enable(struct mtk_clk *mck)
{
    return mtk_clk_gate_clr(gate_from_mtk_clk(mck), true);
}

static void mtk_clk_gate_setclr_disable(struct mtk_clk *mck)
{
    mtk_clk_gate_set(gate_from_mtk_clk(mck), true);
}

static int mtk_clk_gate_setclr_inv_enable(struct mtk_clk *mck)
{
    return mtk_clk_gate_set(gate_from_mtk_clk(mck), true);
}

static void mtk_clk_gate_setclr_inv_disable(struct mtk_clk *mck)
{
    mtk_clk_gate_clr(gate_from_mtk_clk(mck), true);
}

const struct mtk_clk_ops mtk_clk_gate_ops = {
    .enable        = mtk_clk_gate_enable,
    .disable    = mtk_clk_gate_disable,
    .is_enabled    = mtk_clk_gate_is_enabled,
};

const struct mtk_clk_ops mtk_clk_gate_inv_ops = {
    .enable        = mtk_clk_gate_inv_enable,
    .disable    = mtk_clk_gate_inv_disable,
    .is_enabled    = mtk_clk_gate_inv_is_enabled,
};

const struct mtk_clk_ops mtk_clk_gate_setclr_ops = {
    .enable        = mtk_clk_gate_setclr_enable,
    .disable    = mtk_clk_gate_setclr_disable,
    .is_enabled    = mtk_clk_gate_is_enabled,
};

const struct mtk_clk_ops mtk_clk_gate_setclr_inv_ops = {
    .enable        = mtk_clk_gate_setclr_inv_enable,
    .disable    = mtk_clk_gate_setclr_inv_disable,
    .is_enabled    = mtk_clk_gate_inv_is_enabled,
};

/* MUX support */
static int mtk_clk_mux_enable_setclr(struct mtk_clk *mck)
{
    struct mtk_clk_mux *mux = mux_from_mtk_clk(mck);
    uint32_t val;
    void *mux_clr_addr = mux->mux_base + mux->mux_clr_ofs;

    val = BIT(mux->gate_shift);
    DRV_WriteReg32(mux_clr_addr, val);

    return 0;
}

static void mtk_clk_mux_disable_setclr(struct mtk_clk *mck)
{
    struct mtk_clk_mux *mux = mux_from_mtk_clk(mck);
    uint32_t val;
    void *mux_set_addr = mux->mux_base + mux->mux_set_ofs;

    val = BIT(mux->gate_shift);
    DRV_WriteReg32(mux_set_addr, val);
}

static int mtk_clk_mux_is_enabled(struct mtk_clk *mck)
{
    struct mtk_clk_mux *mux = mux_from_mtk_clk(mck);
    uint32_t val;
    void *mux_addr = mux->mux_base + mux->mux_ofs;

    //if (mux->gate_shift < 0)
    //    return true;

    val = DRV_Reg32(mux_addr);

    return (val & BIT(mux->gate_shift)) == 0;
}

static int mtk_clk_mux_set_parent(struct mtk_clk *mck, uint8_t index)
{
    struct mtk_clk_mux *mux = mux_from_mtk_clk(mck);
    uint32_t mask = GENMASK(mux->mux_width - 1, 0);
    uint32_t val, orig;
    void *mux_addr = mux->mux_base + mux->mux_ofs;

    if (index >= mck->num_parents)
        return EINVAL;

    if (mux->flags & MTK_CLK_MUX_INDEX_BIT)
        index = 1 << index;

    val = DRV_Reg32(mux_addr);
    orig = val;

    val &= ~(mask << mux->mux_shift);
    val |= index << mux->mux_shift;

    if (val != orig)
        DRV_WriteReg32(mux_addr, val);

    return 0;
}

static int mtk_clk_mux_set_parent_setclr(struct mtk_clk *mck, uint8_t index)
{
    struct mtk_clk_mux *mux = mux_from_mtk_clk(mck);
    uint32_t mask = GENMASK(mux->mux_width - 1, 0);
    uint32_t val, orig;
    unsigned long flags = 0;

    val = DRV_Reg32(mux->mux_base + mux->mux_ofs);
    orig = val;
    val &= ~(mask << mux->mux_shift);
    val |= index << mux->mux_shift;

    if (val != orig) {
        val = (mask << mux->mux_shift);
        DRV_WriteReg32((mux->mux_base + mux->mux_clr_ofs), val);
        mb();

        val = (index << mux->mux_shift);
        DRV_WriteReg32((mux->mux_base + mux->mux_set_ofs), val);
        mb();

        //if (mux->upd_shift >= 0)
            DRV_WriteReg32((mux->mux_base + mux->upd_ofs), BIT(mux->upd_shift));
    }

    return 0;
}

static uint8_t mtk_clk_mux_get_parent(struct mtk_clk *mck)
{
    struct mtk_clk_mux *mux = mux_from_mtk_clk(mck);
    uint32_t mask = GENMASK(mux->mux_width - 1, 0);
    uint32_t val;
    void *mux_addr = mux->mux_base + mux->mux_ofs;

    val = DRV_Reg32(mux_addr);
    val >>= mux->mux_shift;
    val &= mask;

    if (val && (mux->flags & MTK_CLK_MUX_INDEX_BIT))
        val = ffs(val) - 1;

    if (val >= mck->num_parents)
        return EEXCEDE;

    return val;
}

const struct mtk_clk_ops mtk_clk_mux_ops = {
    .set_parent    = mtk_clk_mux_set_parent,
    .get_parent    = mtk_clk_mux_get_parent,
};

const struct mtk_clk_ops mtk_clk_mux_clr_set_upd_ops = {
    .enable = mtk_clk_mux_enable_setclr,
    .disable = mtk_clk_mux_disable_setclr,
    .is_enabled = mtk_clk_mux_is_enabled,
    .set_parent = mtk_clk_mux_set_parent_setclr,
    .get_parent = mtk_clk_mux_get_parent,
};

/* PLL support */

#define CON0_BASE_EN        BIT(0)
#define CON0_PWR_ON        BIT(0)
#define CON0_ISO_EN        BIT(1)
#define CON1_PCW_CHG        BIT(31)

#define POSTDIV_MASK        0x7
#define INTEGER_BITS        7

static int mtk_clk_pll_enable(struct mtk_clk *mck)
{
    struct mtk_clk_pll *pll = pll_from_mtk_clk(mck);
    uint32_t r;
    void *pwr_addr = pll->pll_base + pll->pwr_ofs;
    void *base_addr = pll->pll_base + pll->base_ofs;
    void *tuner_en_addr = pll->pll_base + pll->tuner_en_ofs;

    r = DRV_Reg32(pwr_addr) | CON0_PWR_ON;
    DRV_WriteReg32(pwr_addr, r);
    udelay(1);

    r = DRV_Reg32(pwr_addr) & ~CON0_ISO_EN;
    DRV_WriteReg32(pwr_addr, r);
    udelay(1);

    r = DRV_Reg32(base_addr);
    r |= pll->en_mask;
    DRV_WriteReg32(base_addr, r);

    if (pll->tuner_en_ofs) {
        r = DRV_Reg32(tuner_en_addr);
        r |= BIT(pll->tuner_en_shift);
        DRV_WriteReg32(tuner_en_addr, r);
    }

    udelay(20);

    if (pll->rst_bar_mask) {
        r = DRV_Reg32(base_addr);
        r |= pll->rst_bar_mask;
        DRV_WriteReg32(base_addr, r);
    }

    return 0;
}

static void mtk_clk_pll_disable(struct mtk_clk *mck)
{
    struct mtk_clk_pll *pll = pll_from_mtk_clk(mck);
    uint32_t r;
    void *base_addr = pll->pll_base + pll->base_ofs;
    void *tuner_en_addr = pll->pll_base + pll->tuner_en_ofs;
    void *pwr_addr = pll->pll_base + pll->pwr_ofs;

    if (pll->rst_bar_mask) {
        r = DRV_Reg32(base_addr);
        r &= ~pll->rst_bar_mask;
        DRV_WriteReg32(base_addr, r);
    }

    if (pll->tuner_en_ofs) {
        r = DRV_Reg32(tuner_en_addr);
        r &= ~BIT(pll->tuner_en_shift);
        DRV_WriteReg32(tuner_en_addr, r);
    }

    r = DRV_Reg32(base_addr);
    r &= ~CON0_BASE_EN;
    DRV_WriteReg32(base_addr, r);

    r = DRV_Reg32(pwr_addr)| CON0_ISO_EN;
    DRV_WriteReg32(pwr_addr, r);

    r = DRV_Reg32(pwr_addr) & ~CON0_PWR_ON;
    DRV_WriteReg32(pwr_addr, r);
}

static int mtk_clk_pll_is_enabled(struct mtk_clk *mck)
{
    struct mtk_clk_pll *pll = pll_from_mtk_clk(mck);
    void *base_addr = pll->pll_base + pll->base_ofs;

    return (DRV_Reg32(base_addr)& CON0_BASE_EN) != 0;
}

static unsigned long __mtk_pll_recalc_rate(struct mtk_clk_pll *pll, uint32_t fin,
        uint32_t pcw, int postdiv)
{
    int pcwbits = pll->pcwbits;
    int pcwfbits;
    int ibits;
    uint64_t vco;
    uint8_t c = 0;

    /* The fractional part of the PLL divider. */
    ibits = pll->pcwibits ? pll->pcwibits : INTEGER_BITS;
    pcwfbits = pcwbits > ibits ? pcwbits - ibits : 0;

    vco = (uint64_t)fin * pcw;

    if (pcwfbits && (vco & GENMASK(pcwfbits - 1, 0)))
        c = 1;

    vco >>= pcwfbits;

    if (c)
        vco++;

    return ((unsigned long)vco + postdiv - 1) / postdiv;
}

static void mtk_pll_set_rate_regs(struct mtk_clk_pll *pll, uint32_t pcw,
        int postdiv)
{
    uint32_t val, chg;
    uint32_t tuner_en = 0;
    uint32_t tuner_en_mask = BIT(pll->tuner_en_shift);
    void *tuner_en_addr = pll->pll_base + pll->tuner_en_ofs;
    void *pd_addr = pll->pll_base + pll->pd_ofs;
    void *pcw_addr = pll->pll_base + pll->pcw_ofs;
    void *pcw_chg_addr = pll->pll_base + pll->pcw_chg_ofs;

    /* disable tuner */
    if (pll->tuner_en_ofs) {
        val = DRV_Reg32(tuner_en_addr);
        tuner_en = val & tuner_en_mask;

        if (tuner_en) {
            val &= ~tuner_en_mask;
            DRV_WriteReg32(tuner_en_addr, val);
        }
    }

    /* set postdiv */
    val = DRV_Reg32(pd_addr);
    val &= ~(POSTDIV_MASK << pll->pd_shift);
    val |= (ffs(postdiv) - 1) << pll->pd_shift;

    /* postdiv and pcw need to set at the same time if on same register */
    if (pll->pd_ofs != pll->pcw_ofs) {
        DRV_WriteReg32(pd_addr, val);
        val = DRV_Reg32(pcw_addr);
    }

    /* set pcw */
    val &= ~GENMASK(pll->pcw_shift + pll->pcwbits - 1,
            pll->pcw_shift);
    val |= pcw << pll->pcw_shift;

    if (pll->tuner_ofs)
        DRV_WriteReg32(tuner_en_addr, val + 1);

    if (pll->pcw_chg_ofs) {
        chg = DRV_Reg32(pcw_chg_addr);
        chg |= CON1_PCW_CHG;
        DRV_WriteReg32(pcw_addr, val);
        DRV_WriteReg32(pcw_chg_addr, chg);
    } else {
        val |= CON1_PCW_CHG;
        DRV_WriteReg32(pcw_addr, val);
    }

    /* restore tuner_en */
    if (pll->tuner_en_ofs && tuner_en) {
        val = DRV_Reg32(tuner_en_addr);
        val |= tuner_en_mask;
        DRV_WriteReg32(tuner_en_addr, val);
    }

    udelay(20);
}

/*
 * mtk_pll_calc_values - calculate good values for a given input frequency.
 * @pll:    The pll
 * @pcw:    The pcw value (output)
 * @postdiv:    The post divider (output)
 * @freq:    The desired target frequency
 * @fin:    The input frequency
 *
 */
static void
mtk_pll_calc_values(struct mtk_clk_pll *pll, uint32_t *pcw, uint32_t *postdiv,
        uint32_t freq, uint32_t fin)
{
    unsigned long fmin = pll->fmin ? pll->fmin : 1000 * MHZ;
    const struct mtk_pll_div_table *div_table = pll->div_table;
    uint64_t _pcw;
    uint32_t val;
    int ibits;

    if (freq > pll->fmax)
        freq = pll->fmax;

    if (div_table) {
        if (freq > div_table[0].freq)
            freq = div_table[0].freq;

        for (val = 0; div_table[val + 1].freq != 0; val++) {
            if (freq > div_table[val + 1].freq)
                break;
        }
        *postdiv = 1 << val;
    } else {
        for (val = 0; val < 5; val++) {
            *postdiv = 1 << val;
            if ((uint64_t)freq * *postdiv >= fmin)
                break;
        }
    }

    /* _pcw = freq * postdiv / fin * 2^pcwfbits */
    ibits = pll->pcwibits ? pll->pcwibits : INTEGER_BITS;
    _pcw = ((uint64_t)freq << val) << (pll->pcwbits - ibits);
    _pcw /= fin;    // do_div(_pcw, fin);

    *pcw = (uint32_t)_pcw;
}

static unsigned long mtk_clk_pll_recalc_rate(struct mtk_clk *mck, unsigned long parent_rate)
{
    struct mtk_clk_pll *pll = pll_from_mtk_clk(mck);
    uint32_t postdiv;
    uint32_t pcw;
    void *pd_addr = pll->pll_base + pll->pd_ofs;
    void *pcw_addr = pll->pll_base + pll->pcw_ofs;

    postdiv = DRV_Reg32(pd_addr);
    postdiv = (postdiv >> pll->pd_shift) & POSTDIV_MASK;
    postdiv = 1 << postdiv;

    pcw = DRV_Reg32(pcw_addr);
    pcw = pcw >> pll->pcw_shift;
    pcw &= GENMASK(pll->pcwbits - 1, 0);

    return __mtk_pll_recalc_rate(pll, parent_rate, pcw, postdiv);
}

static long mtk_clk_pll_round_rate(struct mtk_clk *mck, unsigned long rate,
            unsigned long *parent_rate)
{
    struct mtk_clk_pll *pll = pll_from_mtk_clk(mck);
    uint32_t pcw = 0;
    uint32_t postdiv;

    mtk_pll_calc_values(pll, &pcw, &postdiv, rate, *parent_rate);

    return __mtk_pll_recalc_rate(pll, *parent_rate, pcw, postdiv);
}

static int mtk_clk_pll_set_rate(struct mtk_clk *mck, unsigned long rate,
            unsigned long parent_rate)
{
    struct mtk_clk_pll *pll = pll_from_mtk_clk(mck);
    uint32_t pcw = 0;
    uint32_t postdiv;

    if (rate == 0)
        return EINVAL;

    if (parent_rate == 0)
        parent_rate = 26000000;

    mtk_pll_calc_values(pll, &pcw, &postdiv, rate, parent_rate);
    mtk_pll_set_rate_regs(pll, pcw, postdiv);

    return 0;
}

const struct mtk_clk_ops mtk_clk_pll_ops = {
    .enable        = mtk_clk_pll_enable,
    .disable    = mtk_clk_pll_disable,
    .is_enabled    = mtk_clk_pll_is_enabled,
    .recalc_rate    = mtk_clk_pll_recalc_rate,
    .round_rate    = mtk_clk_pll_round_rate,
    .set_rate    = mtk_clk_pll_set_rate,
};

/* divider support */

#define DIV_ROUND_UP(n,d)    (((n) + (d) - 1) / (d))
#define div_mask(width)        ((1 << (width)) - 1)

static uint32_t calc_div(struct mtk_clk_div *div, unsigned long rate, unsigned long parent_rate)
{
    uint32_t d, maxd;

    if (rate == 0)
        rate = 1;

    maxd = div_mask(div->div_width) + 1;
    maxd = min(ULONG_MAX / rate, maxd);

    d = DIV_ROUND_UP(parent_rate, rate);

    d = max(1, d);
    d = min(maxd, d);

    return d;
}

static unsigned long mtk_clk_div_recalc_rate(struct mtk_clk *mck, unsigned long parent_rate)
{
    struct mtk_clk_div *div = div_from_mtk_clk(mck);
    uint32_t d;
    void *addr = div->div_base + div->div_ofs;

    d = DRV_Reg32(addr);
    d = (d >> div->div_shift) & div_mask(div->div_width);
    d = d + 1;

    return DIV_ROUND_UP(parent_rate, d);
}

static long mtk_clk_div_round_rate(struct mtk_clk *mck, unsigned long rate,
            unsigned long *parent_rate)
{
    struct mtk_clk_div *div = div_from_mtk_clk(mck);
    uint32_t d = calc_div(div, rate, *parent_rate);

    return DIV_ROUND_UP(*parent_rate, d);
}

static int mtk_clk_div_set_rate(struct mtk_clk *mck, unsigned long rate,
            unsigned long parent_rate)
{
    struct mtk_clk_div *div = div_from_mtk_clk(mck);
    uint32_t d = calc_div(div, rate, parent_rate) - 1;
    uint32_t mask = div_mask(div->div_width);
    uint32_t val, orig;
    void *addr = div->div_base + div->div_ofs;

    if (rate == 0 || parent_rate == 0)
        return EINVAL;

    val = DRV_Reg32(addr);
    orig = val;

    val &= ~(mask << div->div_shift);
    val |= d << div->div_shift;

    if (val != orig)
        DRV_WriteReg32(addr, val);

    return 0;
}

const struct mtk_clk_ops mtk_clk_div_ops = {
    .recalc_rate    = mtk_clk_div_recalc_rate,
    .round_rate    = mtk_clk_div_round_rate,
    .set_rate    = mtk_clk_div_set_rate,
};
