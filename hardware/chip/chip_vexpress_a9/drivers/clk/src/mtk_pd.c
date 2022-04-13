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
#include <mt_reg_base.h>
#include <mt_printf.h>
#include <arch_timer.h>
#include <clk.h>

#include <mtk_pd.h>
#include <mtk_pd_provider.h>
#include <mtk_clk_pd_basic.h>

#define BIT(nr)             (1 << (nr))

static SemaphoreHandle_t xSemaphore_pd;
static bool mutex_initialized = false;

static void mtk_pd_lock(void)
{
    if (!mutex_initialized) {
        xSemaphore_pd = xSemaphoreCreateMutex();
        mutex_initialized = true;
    }

    xSemaphoreTake(xSemaphore_pd, pdMS_TO_TICKS(10));
}

static void mtk_pd_unlock(void)
{
    xSemaphoreGive(xSemaphore_pd);
}

#define SPM_PWR_STATUS         (SCPSYS_BASE + 0x180)
#define SPM_PWR_STATUS_2ND     (SCPSYS_BASE + 0x184)

#define PWR_RST_B_BIT          BIT(0)
#define PWR_ISO_BIT            BIT(1)
#define PWR_ON_BIT             BIT(2)
#define PWR_ON_2ND_BIT         BIT(3)
#define PWR_CLK_DIS_BIT        BIT(4)

#define mtk_pd_wait_pwr_on(pd) \
    mtk_pd_wait_for_state(pd, 0, 0, mtk_pd_is_pwr_on)
#define mtk_pd_wait_pwr_off(pd) \
    mtk_pd_wait_for_state(pd, 0, 0, mtk_pd_is_pwr_off)
#define mtk_pd_wait_bus_prot_set(reg, mask) \
    mtk_pd_wait_for_state(NULL, reg, mask, mtk_pd_is_state_set)
#define mtk_pd_wait_bus_prot_clr(reg, mask) \
    mtk_pd_wait_for_state(NULL, reg, mask, mtk_pd_is_state_clr)
#define mtk_pd_wait_sram_pwr_on(reg, mask) \
    mtk_pd_wait_for_state(NULL, reg, mask, mtk_pd_is_state_clr)
#define mtk_pd_wait_sram_pwr_down(reg, mask) \
    mtk_pd_wait_for_state(NULL, reg, mask, mtk_pd_is_state_set)

struct mtk_pd mtk_pds[NR_PD] = {{0}};

struct mtk_pd *get_mtk_pds(void)
{
    return mtk_pds;
}

int mtk_pd_is_on(struct mtk_pd *pd)
{
    const struct mtk_pd_data *pd_data = pd->data;
    uint32_t status = DRV_Reg32(SPM_PWR_STATUS) & pd_data->sta_mask;
    uint32_t status2 = DRV_Reg32(SPM_PWR_STATUS_2ND) &
        pd_data->sta_mask;

    /*
     * A domain is on when both status bits are set. If only one is set
     * return an error. This happens while powering up a domain
     */

    if (status && status2)
        return true;
    if (!status && !status2)
        return false;

    return EINVAL;
}

static inline bool mtk_pd_is_state_set(struct mtk_pd *pd, uint32_t reg, uint32_t mask)
{
    return ((DRV_Reg32(reg) & mask) == mask);
}

static inline bool mtk_pd_is_state_clr(struct mtk_pd *pd, uint32_t reg, uint32_t mask)
{
    return (!(DRV_Reg32(reg) & mask));
}

static inline bool mtk_pd_is_pwr_on(struct mtk_pd *pd, uint32_t reg, uint32_t mask)
{
    return mtk_pd_is_on(pd);
}

static inline bool mtk_pd_is_pwr_off(struct mtk_pd *pd, uint32_t reg, uint32_t mask)
{
    return (!mtk_pd_is_on(pd));
}

static int mtk_pd_wait_for_state(struct mtk_pd *pd, uint32_t reg, uint32_t mask,
        bool (*fp)(struct mtk_pd *, uint32_t, uint32_t))
{
    int time = 0;

    do {
        if (fp(pd, reg, mask))
            return 0;

        udelay(10);
        time++;
        if (time > 100)
            return ETIMEDOUT;
    } while (1);
}

static int mtk_pd_set_bus_prot(const struct bus_prot *prot_table)
{
    int i, ret;
    uint32_t reg_set, mask, reg_sta;

    for (i = 0; i < MAX_STEPS && prot_table[i].mask; i++) {
        reg_set = prot_table[i].base + prot_table[i].set_ofs;
        mask = prot_table[i].mask;
        reg_sta = prot_table[i].base + prot_table[i].sta_ofs;

        DRV_WriteReg32(reg_set, mask);

        ret = mtk_pd_wait_bus_prot_set(reg_sta, mask);
        if (ret)
            printf("%s timeout: reg_sta 0x%x = 0x%x\n", "mtk_pd_set_bus_prot",
                    reg_sta, DRV_Reg32(reg_sta));

        //return ret;
    }

    return 0;
}

static int mtk_pd_clear_bus_prot(const struct bus_prot *prot_table)
{
    int i, ret;
    uint32_t reg_clr, mask, reg_sta;

    for (i = 0; i < MAX_STEPS && prot_table[i].mask; i++) {
        reg_clr = prot_table[i].base + prot_table[i].clr_ofs;
        mask = prot_table[i].mask;
        reg_sta = prot_table[i].base + prot_table[i].sta_ofs;

        DRV_WriteReg32(reg_clr, mask);

        ret = mtk_pd_wait_bus_prot_clr(reg_sta, mask);
        if (ret)
            printf("%s timeout: reg_sta 0x%x = 0x%x\n", "mtk_pd_clear_bus_prot",
                    reg_sta, DRV_Reg32(reg_sta));

        //return ret;
    }

    return 0;
}

static void mtk_pd_bus_slave_way(const struct bus_way *way_table, bool en)
{
    int i;
    uint32_t reg_way, mask;

    for (i = 0; i < MAX_STEPS && way_table[i].mask; i++) {
        reg_way = way_table[i].base + way_table[i].en_ofs;
        mask = way_table[i].mask;

        if (en)
            DRV_WriteReg32(reg_way, ((DRV_Reg32(reg_way) & ~mask) | mask));
        else
            DRV_WriteReg32(reg_way, (DRV_Reg32(reg_way) & ~mask));
    }
}

static int mtk_pd_clk_enable(struct clk *clk[], int max_num)
{
    int i, ret = 0;

    for (i = 0; i < max_num && clk[i]; i++) {
        ret = clk_enable(clk[i]);
        if (ret) {
            for (--i; i >= 0; i--)
                clk_disable(clk[i]);

            break;
        }
    }

    return ret;
}

static void mtk_pd_clk_disable(struct clk *clk[], int max_num)
{
    int i;

    for (i = max_num - 1; i >= 0; i--) {
        if (clk[i])
            clk_disable(clk[i]);
    }
}

static int mtk_pd_on(struct mtk_pd *pd)
{
    const struct mtk_pd_data *pd_data = pd->data;
    uint32_t ctl_addr = SCPSYS_BASE + pd_data->ctl_ofs;
    uint32_t sram_pdn_addr = SCPSYS_BASE + pd_data->sram_pdn_ofs;
    uint32_t sram_pdn_ack = pd_data->sram_pdn_ack_bits;
    struct mtk_pd_sram_ops *sram_ops = &pd->sram_ops;
    uint32_t val;
    int ret = 0;
    int time = 0;
    int i;

    ret = mtk_pd_clk_enable(pd->clks, MAX_CLKS);
    if (ret)
        goto err;

    val = DRV_Reg32(ctl_addr);
    val |= PWR_ON_BIT;
    DRV_WriteReg32(ctl_addr, val);
    val |= PWR_ON_2ND_BIT;
    DRV_WriteReg32(ctl_addr, val);

    /* wait until PWR_ACK = 1 */
    ret = mtk_pd_wait_pwr_on(pd);
    if (ret) {
        printf("%s: wait %s pwr_status timeout\n", "mtk_pd_on", pd_data->name);
        goto err;
    }

    val &= ~PWR_CLK_DIS_BIT;
    DRV_WriteReg32(ctl_addr, val);

    val &= ~PWR_ISO_BIT;
    DRV_WriteReg32(ctl_addr, val);

    val |= PWR_RST_B_BIT;
    DRV_WriteReg32(ctl_addr, val);

    ret = mtk_pd_clk_enable(pd->subsys_clks, MAX_SUBSYS_CLKS);
    if (ret)
        goto err;

    if (sram_ops->sram_on) {
        ret = sram_ops->sram_on(pd);
        if (ret)
            goto err;
    } else {
        val = DRV_Reg32(sram_pdn_addr);
        val &= ~pd_data->sram_pdn_bits;
        DRV_WriteReg32(sram_pdn_addr, val);

        time = 0;
        /* wait until SRAM_PDN_ACK all 0 */
        ret = mtk_pd_wait_sram_pwr_on(sram_pdn_addr, sram_pdn_ack);
        if (ret) {
            printf("%s: wait %s sram_pdn_ack timeout, sram_pdn_addr 0x%x = 0x%x\n",
                "mtk_pd_on", pd_data->name, sram_pdn_addr, DRV_Reg32(sram_pdn_addr));
            goto err;
        }
    }

    //if (pd_data->way_table)
    mtk_pd_bus_slave_way(pd_data->way_table, true);

    //if (pd_data->prot_table) 
    {
        ret = mtk_pd_clear_bus_prot(pd_data->prot_table);
        if (ret) {
            printf("%s: clear %s bus protection error, ret = %d\n", "mtk_pd_on", pd_data->name, ret);
            goto err;
        }
    }

    return 0;

err:
    printf("%s: Failed to power on mtcmos %s\n", "mtk_pd_on", pd_data->name);
    return ret;
}

static int mtk_pd_off(struct mtk_pd *pd)
{
    const struct mtk_pd_data *pd_data = pd->data;
    uint32_t ctl_addr = SCPSYS_BASE + pd_data->ctl_ofs;
    uint32_t sram_pdn_addr = SCPSYS_BASE + pd_data->sram_pdn_ofs;
    uint32_t sram_pdn_ack = pd_data->sram_pdn_ack_bits;
    struct mtk_pd_sram_ops *sram_ops = &pd->sram_ops;
    uint32_t val=0;
    int ret = 0;
    int i;

    //if (pd_data->prot_table) 
    {
        ret = mtk_pd_set_bus_prot(pd_data->prot_table);
        if (ret) {
            printf("%s: set %s bus protection error, ret = %d\n", "mtk_pd_off", pd_data->name, ret);
            goto err;
        }
    }

    //if (pd_data->way_table)
    mtk_pd_bus_slave_way(pd_data->way_table, false);

    if (sram_ops->sram_off) {
        ret = sram_ops->sram_off(pd);
        if (ret)
            goto err;
    } else {
        val = DRV_Reg32(sram_pdn_addr);
        val |= pd_data->sram_pdn_bits;
        DRV_WriteReg32(sram_pdn_addr, val);

        /* wait until SRAM_PDN_ACK all 1 */
        ret = mtk_pd_wait_sram_pwr_down(sram_pdn_addr, sram_pdn_ack);
        if (ret) {
            printf("%s: wait %s sram_pdn_ack timeout, sram_pdn_addr 0x%x = 0x%x\n",
                "mtk_pd_off", pd_data->name, sram_pdn_addr, DRV_Reg32(sram_pdn_addr));
            goto err;
        }
    }

    mtk_pd_clk_disable(pd->subsys_clks, MAX_SUBSYS_CLKS);

    val |= PWR_ISO_BIT;
    DRV_WriteReg32(ctl_addr, val);

    val &= ~PWR_RST_B_BIT;
    DRV_WriteReg32(ctl_addr, val);

    val |= PWR_CLK_DIS_BIT;
    DRV_WriteReg32(ctl_addr, val);

    val &= ~PWR_ON_BIT;
    DRV_WriteReg32(ctl_addr, val);

    val &= ~PWR_ON_2ND_BIT;
    DRV_WriteReg32(ctl_addr, val);

    ret = mtk_pd_wait_pwr_off(pd);
    if (ret) {
        printf("%s: wait %s pwr_status timeout\n", "mtk_pd_off", pd_data->name);
        goto err;
    }

    mtk_pd_clk_disable(pd->clks, MAX_CLKS);

    return 0;

err:
    printf("%s: Failed to power off mtcmos %s\n", "mtk_pd_off", pd_data->name);
    return ret;
}

const struct mtk_pd_ops *get_mtk_pd_ops(void)
{
    static const struct mtk_pd_ops ops = {
        .is_on = mtk_pd_is_on,
        .power_on = mtk_pd_on,
        .power_off = mtk_pd_off,
    };

    return &ops;
}

static void init_pd_clks(struct mtk_pd *pd)
{
    int i;
    struct clk *clk;
    const struct mtk_pd_data *pd_data = pd->data;

    for (i = 0; i < MAX_CLKS && pd_data->clks_id[i]; i++) {
        clk = clk_get(pd_data->clks_id[i]);
        if (!is_err_or_null_clk(clk))
            pd->clks[i] = clk;
    }

    for (i = 0; i < MAX_SUBSYS_CLKS && pd_data->subsys_clks_id[i]; i++) {
        clk = clk_get(pd_data->subsys_clks_id[i]);
        if (!is_err_or_null_clk(clk))
            pd->subsys_clks[i] = clk;
    }
}

int mtk_pds_register(const struct mtk_pd_data *pds_data, int num_pds)
{
    int i;

    if (num_pds >= NR_PD) {
        printf("excede max pd number!\n");
        return EINVAL;
    }

    for (i = 0; i < num_pds; i++) {
        struct mtk_pd *pd = &mtk_pds[i];

        pd->data = &pds_data[i];

        init_pd_clks(pd);
    }

    return 0;
}

void mtk_pd_disable_unused(void)
{
    int i;
    struct mtk_pd *pd;

    for (i = NR_PD; i > 0; i--) {
        pd = &mtk_pds[i - 1];

        if (!pd->data)
            continue;

        if ((pd->enable_cnt == 0) && (mtk_pd_is_on(pd) == true)) {
            mtk_pd_on(pd);
            mtk_pd_off(pd);
        }
    }
}

struct mtk_pd *mtk_pd_get(int id)
{
    if (id >= NR_PD) {
        printf("mtk_pd_get: power domain ID invalid!");
        return (void *)EINVAL;
    }

    if (!mtk_pds[id].data) {
        printf("mtk_pd_get: mtk_pd not ready!");
        return (void *) EDEFER;
    }

    return &mtk_pds[id];
}

bool is_err_or_null_pd(struct mtk_pd *pd)
{
    return ((int)pd < MAX_ERRNO);
}

struct mtk_pd *mtk_pd_from_name(const char *name)
{
    int i;
    struct mtk_pd *pd;

    for (i = 0; i < NR_PD; i++) {
        pd = &mtk_pds[i];

        if (!pd->data)
            break;

        if (strcmp(name, pd->data->name) == 0)
            return pd;
    }

    return NULL;
}

void mtk_pd_register_sram_ops(struct mtk_pd *pd,
        int (*on)(struct mtk_pd *), int (*off)(struct mtk_pd *))
{
    struct mtk_pd_sram_ops *sram_ops = &pd->sram_ops;

    mtk_pd_lock();

    sram_ops->sram_on = on;
    sram_ops->sram_off = off;

    mtk_pd_unlock();
    return;
}

int mtk_pd_enable(struct mtk_pd *pd)
{
    int ret;

    if (!pd)
        return EINVAL;

    mtk_pd_lock();

    if (++pd->enable_cnt != 1) {
        ret = 0;
        goto OUT;
    }

    ret = mtk_pd_on(pd);

OUT:
    mtk_pd_unlock();
    return ret;
}

int mtk_pd_disable(struct mtk_pd *pd)
{
    int ret;

    if (!pd)
        return EINVAL;

    mtk_pd_lock();

    if (pd->enable_cnt <= 0) {
        printf("mtk_pd_disable: power domain %s already power down!", pd->data->name);
        ret = EINVAL;
        goto OUT;
    }

    if (--pd->enable_cnt > 0) {
        ret = 0;
        goto OUT;
    }

    ret = mtk_pd_off(pd);

OUT:
    mtk_pd_unlock();
    return ret;
}

