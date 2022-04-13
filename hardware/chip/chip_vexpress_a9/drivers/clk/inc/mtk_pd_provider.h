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

#ifndef _MTK_PD_PROVIDER_H_
#define _MTK_PD_PROVIDER_H_

#include <stdint.h>
#include <sys/types.h>

#define ENOMEM       12     /* Out of memory */
#define ENODEV       19     /* No such device */
#define EINVAL       22     /* Invalid argument */
#define ETIMEDOUT    110    /* Connection timed out */
#define EDEFER       517    /* Provider not ready */
#define MAX_ERRNO    1024   /* Max error number for mtk_pd */

#define MAX_STEPS    3
#define MAX_CLKS    3
#define MAX_SUBSYS_CLKS    5

#define BUS_PROT(_base, _en_ofs, _mask,            \
        _set_ofs,  _clr_ofs, _sta_ofs) {    \
        .base = _base,            \
        .en_ofs = _en_ofs,        \
        .mask = _mask,            \
        .set_ofs = _set_ofs,        \
        .clr_ofs = _clr_ofs,        \
        .sta_ofs = _sta_ofs,        \
    }

#define BUS_WAY(_base, _en_ofs, _mask) {    \
        .base = _base,            \
        .en_ofs = _en_ofs,        \
        .mask = _mask,            \
    }

struct bus_prot {
    uint32_t base;
    uint32_t en_ofs;
    uint32_t mask;
    uint32_t set_ofs;
    uint32_t clr_ofs;
    uint32_t sta_ofs;
};

struct bus_way {
    uint32_t base;
    uint32_t en_ofs;
    uint32_t mask;
};

struct mtk_pd_data {
    const char *name;
    uint32_t sta_mask;
    uint32_t ctl_ofs;
    uint32_t sram_pdn_ofs;
    uint32_t sram_pdn_bits;
    uint32_t sram_pdn_ack_bits;
    struct bus_prot prot_table[MAX_STEPS];    /* bus protect table */
    struct bus_way way_table[MAX_STEPS];    /* bus way_en table */
    int clks_id[MAX_CLKS];
    int subsys_clks_id[MAX_SUBSYS_CLKS];
};

struct mtk_pd;

#define NR_PD    32

struct mtk_pd_ops {
    int (*is_on)(struct mtk_pd *scpd);
    int (*power_on)(struct mtk_pd *scpd);
    int (*power_off)(struct mtk_pd *scpd);
};

struct mtk_pd_sram_ops {
    int (*sram_on)(struct mtk_pd *scpd);
    int (*sram_off)(struct mtk_pd *scpd);
};

struct mtk_pd {
    const struct mtk_pd_data *data;
    int enable_cnt;
    struct clk *clks[MAX_CLKS];
    struct clk *subsys_clks[MAX_SUBSYS_CLKS];
    struct mtk_pd_sram_ops sram_ops;
};

struct mtk_pd *get_mtk_pds(void);
int mtk_pds_register(const struct mtk_pd_data *pds_data, int num_pds);
const struct mtk_pd_ops *get_mtk_pd_ops(void);
struct mtk_pd *mtk_pd_from_name(const char *name);

#endif /* _MTK_PD_PROVIDER_H_ */
