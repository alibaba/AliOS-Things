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
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <mt_printf.h>
#include <arch_timer.h>
#include <clk.h>
#include "aos/cli.h"

#include "mtk_clk_provider.h"
#include "mtk_clkdbg.h"
#include "mtk_pd_provider.h"
#include "mtk_pd.h"
#include <mtk_clk_pd_basic.h>

/*
 * clkdbg ops
 */

static const struct clkdbg_ops *clkdbg_ops;

void set_clkdbg_ops(const struct clkdbg_ops *ops)
{
    clkdbg_ops = ops;
}

static const struct fmeter_clk *get_all_fmeter_clks(void)
{
    if (!clkdbg_ops || !clkdbg_ops->get_all_fmeter_clks)
        return NULL;

    return clkdbg_ops->get_all_fmeter_clks();
}

static void *prepare_fmeter(void)
{
    if (!clkdbg_ops || !clkdbg_ops->prepare_fmeter)
        return NULL;

    return clkdbg_ops->prepare_fmeter();
}

static void unprepare_fmeter(void *data)
{
    if (!clkdbg_ops || !clkdbg_ops->unprepare_fmeter)
        return;

    clkdbg_ops->unprepare_fmeter(data);
}

static uint32_t fmeter_freq(const struct fmeter_clk *fclk)
{
    if (!clkdbg_ops || !clkdbg_ops->fmeter_freq)
        return 0;

    return clkdbg_ops->fmeter_freq(fclk);
}

static const struct regname *get_all_regnames(void)
{
    if (!clkdbg_ops || !clkdbg_ops->get_all_regnames)
        return NULL;

    return clkdbg_ops->get_all_regnames();
}

static const char * const *get_all_clk_names(void)
{
    if (!clkdbg_ops || !clkdbg_ops->get_all_clk_names)
        return NULL;

    return clkdbg_ops->get_all_clk_names();
}

static const char * const *get_pwr_names(void)
{
    static const char * const default_pwr_names[32] = {""};

    if (!clkdbg_ops || !clkdbg_ops->get_pwr_names)
        return default_pwr_names;

    return clkdbg_ops->get_pwr_names();
}

static void setup_provider(struct provider *pvd)
{
    if (!clkdbg_ops || !clkdbg_ops->setup_provider)
        return;

    clkdbg_ops->setup_provider(pvd);
}

/*
 * clkdbg fmeter
 */

typedef void (*fn_fclk_freq_proc)(const struct fmeter_clk *fclk,
                    uint32_t freq);

static void proc_all_fclk_freq(fn_fclk_freq_proc proc)
{
    void *fmeter_data;
    const struct fmeter_clk *fclk;

    fclk = get_all_fmeter_clks();

    if (!fclk || !proc)
        return;

    fmeter_data = prepare_fmeter();

    for (; fclk->type; fclk++) {
        uint32_t freq;

        freq = fmeter_freq(fclk);
        proc(fclk, freq);
    }

    unprepare_fmeter(fmeter_data);
}

static void print_fclk_freq(const struct fmeter_clk *fclk,
                uint32_t freq)
{
    aos_cli_printf("%2d: %-29s: %u\r\n", fclk->id, fclk->name, freq);
}

static int clkdbg_fmeter(int argc, char ** argv)
{
    proc_all_fclk_freq(print_fclk_freq);

    return 0;
}

/*
 * clkdbg dump_regs / dump_regs2
 */

static void proc_all_regname()
{
    const struct regname *rn = get_all_regnames();

    if (!rn)
        return;

    for (; rn->base; rn++)
        aos_cli_printf("%-24s: [0x%08x] = 0x%08x\r\n",
            rn->name, PHYSADDR(rn), DRV_Reg32(PHYSADDR(rn)));
}

static int clkdbg_dump_regs(int argc, char ** argv)
{
    proc_all_regname();

    return 0;
}

/*
 * clkdbg dump_clks / dump_state
 */

static struct provider *get_all_providers(void)
{
    static struct provider providers[NR_PROVIDER];
    struct mtk_clk_provider *cp;
    int n = 0;

    if (providers[0].mcks)
        return providers;

    for (cp = get_mtk_clk_pvds(); cp->data != NULL; cp++) {
        struct mtk_clk_data *clk_data = cp->data;
        struct provider *pc = &providers[n++];

        pc->provider_name = cp->name;
        pc->mcks = clk_data->mcks;
        pc->nclks = clk_data->nclks;
        setup_provider(pc);
    }

    return providers;
}

static int mck_is_on(struct mtk_clk *mck)
{
    const struct mtk_clk_ops *ops = mck->ops;

    if (ops->is_enabled)
        return ops->is_enabled(mck);

    return mck->enable_cnt > 0;
}

static bool mck_pwr_is_on(struct mtk_clk *mck,
            uint32_t spm_pwr_status, uint32_t pwr_mask)
{
    if ((spm_pwr_status & pwr_mask) != pwr_mask)
        return false;

    return mck_is_on(mck);
}

static bool pvd_pwr_is_on(struct provider *pvd, uint32_t spm_pwr_status, int i)
{
    return mck_pwr_is_on(pvd->mcks[i], spm_pwr_status, pvd->pwr_mask);
}

static bool pvd_is_on(struct provider *pvd, int i)
{
    uint32_t spm_pwr_status = 0;

    if (pvd->pwr_mask)
        spm_pwr_status = DRV_Reg32(0x10006180);

    return pvd_pwr_is_on(pvd, spm_pwr_status, i);
}

static const char *mck_state(struct mtk_clk *mck)
{
    if (mck->enable_cnt > 0)
        return "enabled";

    return "disabled";
}

static void dump_provider_clk(struct provider *pvd, int i)
{
    struct mtk_clk *mck = pvd->mcks[i];
    struct clk *p = clk_get_parent(clk_from_mck(mck));
    const char *pn = p ? mck_from_clk(p)->name : NULL;

    aos_cli_printf("[%10s: %-17s: %3s, %3d, %10lu, %17s]\r\n",
        pvd->provider_name ? pvd->provider_name : "/ ",
        mck->name,
        pvd_is_on(pvd, i) ? "ON" : "off",
        mck->enable_cnt,
        clk_get_rate(clk_from_mck(mck)),
        pn);
}

static void dump_provider_clks(struct provider *pvd)
{
    int i;

    for (i = 0; i < pvd->nclks; i++)
        dump_provider_clk(pvd, i);
}

static int clkdbg_dump_provider_clks(int argc, char ** argv)
{
    struct provider *pvd = get_all_providers();

    for (; pvd->mcks; pvd++)
        dump_provider_clks(pvd);

    return 0;
}

static void dump_clk_state(const char *clkname)
{
    struct clk *c = clk_from_name(clkname);
    struct clk *p = c ? clk_get_parent(c) : NULL;
    struct mtk_clk *mck = mck_from_clk(c);

    if (c == NULL) {
        aos_cli_printf("[%17s: NULL]\r\n", clkname);
        return;
    }

    aos_cli_printf("[%-17s: %8s, %3d, %10lu, %17s]\r\n",
        mck->name,
        mck_state(mck),
        mck->enable_cnt,
        clk_get_rate(c),
        p ? mck_from_clk(p)->name : "- ");
}

static int clkdbg_dump_state(int argc, char ** argv)
{
    const char * const *ckn = get_all_clk_names();

    if (!ckn)
        return clkdbg_dump_provider_clks(argc, argv);

    for (; *ckn; ckn++)
        dump_clk_state(*ckn);

    return 0;
}

/*
 * clkdbg dump_muxes
 */

static void dump_provider_mux(struct provider *pvd)
{
    int i, j;

    for (i = 0; i < pvd->nclks; i++) {
        struct mtk_clk *mck = pvd->mcks[i];
        unsigned int np = mck->num_parents;

        if (np <= 1)
            continue;

        dump_provider_clk(pvd, i);

        for (j = 0; j < np; j++) {
            struct clk *p = clk_from_name(mck->pn.parent_names[j]);
            struct mtk_clk *mck_p = mck_from_clk(p);

            if (p == NULL)
                continue;

            aos_cli_printf("\t\t\t(%2d: %-17s: %8s, %10ld)\r\n",
                j,
                mck_p->name,
                mck_state(mck_p),
                clk_get_rate(p));
        }
    }
}

static int clkdbg_dump_muxes(int argc, char ** argv)
{
    struct provider *pvd = get_all_providers();

    for (; pvd->mcks; pvd++)
        dump_provider_mux(pvd);

    return 0;
}

/*
 * clkdbg pwr_status
 */

static int dump_pwr_status(uint32_t spm_pwr_status)
{
    int i;
    const char * const *pwr_name = get_pwr_names();

    aos_cli_printf("SPM_PWR_STATUS: 0x%08x\n\r\n", spm_pwr_status);

    for (i = 0; i < 32; i++) {
        const char *st = (spm_pwr_status & BIT(i)) ? "ON" : "off";

        aos_cli_printf("[%2d]: %3s: %s\r\n", i, st, pwr_name[i]);
    }

    return 0;
}

static int clkdbg_pwr_status(int argc, char ** argv)
{
    return dump_pwr_status(DRV_Reg32(0x10006180));
}

/*
 * clkdbg enable / disable / set_parent / set_rate
 */

static int clkop_int_ckname(int (*clkop)(struct clk *clk),
            const char *clkop_name, const char *clk_name,
            struct clk *ck)
{
    struct clk *clk;

    if (ck != NULL) {
        clk = ck;
    } else {
        clk = clk_from_name(clk_name);
        if (clk == NULL) {
            aos_cli_printf("clk_from_name(%s): NULL\r\n", clk_name);
            return EINVAL;
        }
    }

    return clkop(clk);
}

static int clkdbg_clkop_int_ckname(int (*clkop)(struct clk *clk),
            const char *clkop_name, const char *clk_name)
{
    int r = 0;
    int i;

    if (!clk_name)
        return 0;

    if (strcmp(clk_name, "all") == 0) {
        struct provider *pvd = get_all_providers();

        for (; pvd->mcks; pvd++) {
            for (i = 0; i < pvd->nclks; i++)
                r |= clkop_int_ckname(clkop, clkop_name, NULL,
                            clk_from_mck(pvd->mcks[i]));
        }

        aos_cli_printf("%s(%s): %d\r\n", clkop_name, clk_name, r);

        return r;
    }

    r = clkop_int_ckname(clkop, clkop_name, clk_name, NULL);
    aos_cli_printf("%s(%s): %d\r\n", clkop_name, clk_name, r);

    return r;
}

static int clkdbg_enable(int argc, char ** argv)
{
    if (argc < 2) {
        aos_cli_printf("clk_enable: invalid arguments\r\n");
        return EINVAL;
    }

    return clkdbg_clkop_int_ckname(clk_enable,
                    "clk_enable", argv[1]);
}

static int clkdbg_disable(int argc, char ** argv)
{
    if (argc < 2) {
        aos_cli_printf("clk_disable: invalid arguments\r\n");
        return EINVAL;
    }

    return clkdbg_clkop_int_ckname(clk_disable,
                    "clk_disable", argv[1]);
}

static int clkdbg_set_parent(int argc, char ** argv)
{
    char *clk_name;
    char *parent_name;
    struct clk *clk;
    struct clk *parent;
    int r;

    if (argc < 3) {
        aos_cli_printf("clk_set_parent: invalid arguments\r\n");
        return EINVAL;
    }

    clk_name = argv[1];
    parent_name = argv[2];

    clk = clk_from_name(clk_name);
    if (clk == NULL) {
        aos_cli_printf("clk_from_name(): 0x%p\r\n", clk);
        return EINVAL;
    }

    parent = clk_from_name(parent_name);
    if (parent == NULL) {
        aos_cli_printf("clk_from_name(): 0x%p\r\n", parent);
        return EINVAL;
    }

    r = clk_enable(clk);
    if (r) {
        aos_cli_printf("clk_enable(): %d\r\n", r);
        return r;
    }

    r = clk_set_parent(clk, parent);
    aos_cli_printf("clk_set_parent(%s, %s): %d\r\n", clk_name, parent_name, r);

    clk_disable(clk);

    return r;
}

static int clkdbg_set_rate(int argc, char ** argv)
{
    char *clk_name;
    struct clk *clk;
    unsigned long rate;
    int r;

    if (argc < 3) {
        aos_cli_printf("clk_set_rate: invalid arguments\r\n");
        return EINVAL;
    }

    clk_name = argv[1];
    rate = strtoul(argv[2], NULL, 0);

    clk = clk_from_name(clk_name);
    if (clk == NULL) {
        aos_cli_printf("clk_from_name(): 0x%p\r\n", clk);
        return EINVAL;
    }

    r = clk_set_rate(clk, rate);
    aos_cli_printf("clk_set_rate(%s, %lu): %d\r\n", clk_name, rate, r);

    return r;
}

/*
 * clkdbg reg_read / reg_write / reg_set / reg_clr
 */

static int clkdbg_reg_read(int argc, char ** argv)
{
    unsigned long addr;

    if (argc < 2) {
        aos_cli_printf("clkdbg_reg_read: invalid arguments\r\n");
        return EINVAL;
    }

    addr = strtoul(argv[1], NULL, 0);

    aos_cli_printf("read(0x%08x): 0x%08x\r\n", (uint32_t)addr, DRV_Reg32(addr));

    return 0;
}

static int clkdbg_reg_write(int argc, char ** argv)
{
    unsigned long addr;
    unsigned long val;

    if (argc < 3) {
        aos_cli_printf("clkdbg_reg_write: invalid arguments\r\n");
        return EINVAL;
    }

    addr = strtoul(argv[1], NULL, 0);
    val = strtoul(argv[2], NULL, 0);

    aos_cli_printf("write(0x%08x, 0x%08x)\r\n", (uint32_t)addr, (uint32_t)val);

    DRV_WriteReg32(addr, val);
    aos_cli_printf("read(0x%08x): 0x%08x\r\n", (uint32_t)addr, DRV_Reg32(addr));

    return 0;
}

static int clkdbg_reg_set(int argc, char ** argv)
{
    unsigned long addr;
    unsigned long val;

    if (argc < 3) {
        aos_cli_printf("clkdbg_reg_set: invalid arguments\r\n");
        return EINVAL;
    }

    addr = strtoul(argv[1], NULL, 0);
    val = strtoul(argv[2], NULL, 0);

    aos_cli_printf("set(0x%08x, 0x%08x)\r\n", (uint32_t)addr, (uint32_t)val);

    val = DRV_Reg32(addr) | val;
    DRV_WriteReg32(addr, val);

    aos_cli_printf("read(0x%08x): 0x%08x\r\n", (uint32_t)addr, DRV_Reg32(addr));

    return 0;
}

static int clkdbg_reg_clr(int argc, char ** argv)
{
    unsigned long addr;
    unsigned long val;

    if (argc < 3) {
        aos_cli_printf("clkdbg_reg_set: invalid arguments\r\n");
        return EINVAL;
    }

    addr = strtoul(argv[1], NULL, 0);
    val = strtoul(argv[2], NULL, 0);

    aos_cli_printf("clr(0x%08x, 0x%08x)\r\n", (uint32_t)addr, (uint32_t)val);

    val = DRV_Reg32(addr) & ~val;
    DRV_WriteReg32(addr, val);

    aos_cli_printf("read(0x%08x): 0x%08x\r\n", (uint32_t)addr, DRV_Reg32(addr));

    return 0;
}

/*
 * clkdbg dump_pds / pd_enable / pd_disable / pd_on / pd_off
 */

static int clkdbg_dump_pds(int argc, char ** argv)
{
    const struct mtk_pd_ops *ops = get_mtk_pd_ops();
    struct mtk_pd *pd = get_mtk_pds();

    for (; pd->data != NULL; pd++)
        aos_cli_printf("[%-9s: %3s, %2d]\r\n",
            pd->data->name, ops->is_on(pd) ? "ON" : "off",
            pd->enable_cnt);

    return 0;
}

static int pd_op(const char *op_name, const char *pd_name)
{
    struct mtk_pd *pd;
    int r = 0;
    int (*op)(struct mtk_pd *pd);

    if (strcmp(op_name, "mtk_pd_enable") == 0)
        op = mtk_pd_enable;
    else if (strcmp(op_name, "mtk_pd_disable") == 0)
        op = mtk_pd_disable;
    else if (strcmp(op_name, "mtk_pd_on") == 0)
        op = get_mtk_pd_ops()->power_on;
    else if (strcmp(op_name, "mtk_pd_off") == 0)
        op = get_mtk_pd_ops()->power_off;
    else {
        aos_cli_printf("unknown op: %s\r\n", op_name);
        return 0;
    }

    if (!pd_name)
        return 0;

    if (strcmp(pd_name, "all") == 0) {
        pd = get_mtk_pds();

        for (; pd->data != NULL; pd++)
            r |= op(pd);

        aos_cli_printf("%s(%s): %d\r\n", op_name, pd_name, r);

        return 0;
    }

    pd = mtk_pd_from_name(pd_name);

    if (pd == NULL)
        aos_cli_printf("%s(%s): NULL\r\n", op_name, pd_name);
    else {
        r = op(pd);
        aos_cli_printf("%s(%s): %d\r\n", op_name, pd_name, r);
    }

    return 0;
}

static int clkdbg_pd_enable(int argc, char ** argv)
{
    if (argc < 2) {
        aos_cli_printf("clkdbg_pd_enable: invalid arguments\r\n");
        return EINVAL;
    }

    return pd_op("mtk_pd_enable", argv[1]);
}

static int clkdbg_pd_disable(int argc, char ** argv)
{
    if (argc < 2) {
        aos_cli_printf("clkdbg_pd_disable: invalid arguments\r\n");
        return EINVAL;
    }

    return pd_op("mtk_pd_disable", argv[1]);
}

static int clkdbg_pd_on(int argc, char ** argv)
{
    if (argc < 2) {
        aos_cli_printf("clkdbg_pd_on: invalid arguments\r\n");
        return EINVAL;
    }

    return pd_op("mtk_pd_on", argv[1]);
}

static int clkdbg_pd_off(int argc, char ** argv)
{
    if (argc < 2) {
        aos_cli_printf("clkdbg_pd_off: invalid arguments\r\n");
        return EINVAL;
    }

    return pd_op("mtk_pd_off", argv[1]);
}

/*
 * clkdbg cmds
 */

static const struct cmd_fn *custom_cmds;

void set_custom_cmds(const struct cmd_fn *cmds)
{
    custom_cmds = cmds;
}

static int clkdbg_cmds(int argc, char ** argv);

static const struct cmd_fn common_cmds[] = {
    CMDFN("dump_regs", clkdbg_dump_regs),
    CMDFN("dump_regs2", clkdbg_dump_regs),
    CMDFN("dump_state", clkdbg_dump_state),
    CMDFN("dump_clks", clkdbg_dump_provider_clks),
    CMDFN("dump_muxes", clkdbg_dump_muxes),
    CMDFN("fmeter", clkdbg_fmeter),
    CMDFN("pwr_status", clkdbg_pwr_status),
    CMDFN("enable", clkdbg_enable),
    CMDFN("disable", clkdbg_disable),
    CMDFN("set_parent", clkdbg_set_parent),
    CMDFN("set_rate", clkdbg_set_rate),
    CMDFN("reg_read", clkdbg_reg_read),
    CMDFN("reg_write", clkdbg_reg_write),
    CMDFN("reg_set", clkdbg_reg_set),
    CMDFN("reg_clr", clkdbg_reg_clr),
    CMDFN("dump_pds", clkdbg_dump_pds),
    CMDFN("pd_enable", clkdbg_pd_enable),
    CMDFN("pd_disable", clkdbg_pd_disable),
    CMDFN("pd_on", clkdbg_pd_on),
    CMDFN("pd_off", clkdbg_pd_off),
    CMDFN("pwr_on", clkdbg_pd_on),
    CMDFN("pwr_off", clkdbg_pd_off),
    CMDFN("cmds", clkdbg_cmds),
    {}
};

static int clkdbg_cmds(int argc, char ** argv)
{
    const struct cmd_fn *cf;

    for (cf = common_cmds; cf->cmd; cf++)
        aos_cli_printf("%s\r\n", cf->cmd);

    for (cf = custom_cmds; cf && cf->cmd; cf++)
        aos_cli_printf("%s\r\n", cf->cmd);

    aos_cli_printf("\r\n");

    return 0;
}

int clkdbg_run(int argc, char ** argv)
{
    const struct cmd_fn *cf;

    if (argc < 2) {
        aos_cli_printf("clkdbg: invalid arguments\r\n");
        return EINVAL;
    }

	argv++;

    for (cf = custom_cmds; cf && cf->cmd; cf++) {
        if (strcmp(cf->cmd, argv[0]) == 0)
            return cf->fn(argc, argv);
    }

    for (cf = common_cmds; cf->cmd; cf++) {
        if (strcmp(cf->cmd, argv[0]) == 0)
            return cf->fn(argc, argv);
    }

    return 0;
}

static int cli_clkdbg(char *buf, int len, int argc, char **argv)
{
    clkdbg_run(argc, (char **)argv);

    return 0;
}

static struct cli_command cli_cmd_clkdbg[] = {
	{"clkdbg", "Commands to debug clock/mtcmos", cli_clkdbg},
};

void cli_clkdbg_register(void)
{
	aos_cli_register_commands(&cli_cmd_clkdbg[0],sizeof(cli_cmd_clkdbg) / sizeof(struct cli_command));
}
