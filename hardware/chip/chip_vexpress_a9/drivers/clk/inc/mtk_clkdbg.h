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

#ifndef _MEDIATEK_CLKDBG_H_
#define _MEDIATEK_CLKDBG_H_

#include "mtk_clk_provider.h"

struct fmeter_clk {
    uint8_t type;
    uint8_t id;
    const char *name;
};

struct regbase {
    uint32_t phys;
    const char *name;
};

struct regname {
    struct regbase *base;
    uint32_t ofs;
    const char *name;
};

#define ADDR(rn)    ((uint8_t *)(rn)->base->virt + (rn)->ofs)
#define PHYSADDR(rn)    ((rn)->base->phys + (rn)->ofs)

struct cmd_fn {
    const char    *cmd;
    int (*fn)(int, char **);
};

#define CMDFN(_cmd, _fn) {    \
    .cmd = _cmd,        \
    .fn = _fn,        \
}

struct provider {
    const char *provider_name;
    struct mtk_clk **mcks;
    unsigned int nclks;
    uint32_t pwr_mask;
};

struct clkdbg_ops {
    const struct fmeter_clk *(*get_all_fmeter_clks)(void);
    void *(*prepare_fmeter)(void);
    void (*unprepare_fmeter)(void *data);
    uint32_t (*fmeter_freq)(const struct fmeter_clk *fclk);
    const struct regname *(*get_all_regnames)(void);
    const char * const *(*get_all_clk_names)(void);
    const char * const *(*get_pwr_names)(void);
    void (*setup_provider)(struct provider *pvd);
};

void set_clkdbg_ops(const struct clkdbg_ops *ops);
void set_custom_cmds(const struct cmd_fn *cmds);
int clkdbg_run(int argc, char ** argv);
void cli_clkdbg_register(void);

#endif /* _MEDIATEK_CLKDBG_H_ */
