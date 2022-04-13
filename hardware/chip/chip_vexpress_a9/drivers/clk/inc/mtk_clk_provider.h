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

#ifndef _MTK_CLK_PROVIDER_H_
#define _MTK_CLK_PROVIDER_H_

#include <stdint.h>
#include <sys/types.h>

#define ARRAY_SIZE(x)       (sizeof(x) / sizeof(x[0]))
#define BIT(nr)             (1 << (nr))
#define GENMASK(h, l)       (((1U << ((h) - (l) + 1)) - 1) << (l))

#define MAX_MUX_IDX      128

#define ENOMEM    12                 /* Out of memory */
#define ENODEV    19                 /* No such device */
#define EINVAL    22                 /* Invalid argument */
#define EDEFER    517                /* Provider not ready */
#define EEXCEDE   (MAX_MUX_IDX + 1)  /* Excede max mux id */
#define MAX_ERRNO 1024               /* Max error number for clk */

#ifndef container_of
/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:    the pointer to the member.
 * @type:    the type of the container struct this is embedded in.
 * @member:    the name of the member within the struct.
 *
 */
#define container_of(ptr, type, member) ({            \
    const typeof(((type *)0)->member) * __mptr = (ptr);    \
    (type *)((char *)__mptr - offsetof(type, member)); })
#endif

#define INIT_CLK_DATA(ckdt, dmcks) \
    do { \
        int i; \
        for (i = 0; i < ARRAY_SIZE(dmcks); i++) \
            clk_data->mcks[CLK_IDX(dmcks[i].mck.id)] = &dmcks[i].mck; \
    } while (0)

#if 0
static inline int ffs(int x)
{
    int r = 1;

    if (!x)
        return 0;
    if (!(x & 0xffff)) {
        x >>= 16;
        r += 16;
    }
    if (!(x & 0xff)) {
        x >>= 8;
        r += 8;
    }
    if (!(x & 0xf)) {
        x >>= 4;
        r += 4;
    }
    if (!(x & 3)) {
        x >>= 2;
        r += 2;
    }
    if (!(x & 1)) {
        x >>= 1;
        r += 1;
    }
    return r;
}
#endif
#define min(a, b) ((a) < (b)) ? (a) : (b)
#define max(a, b) ((a) < (b)) ? (b) : (a)

#define NR_PROVIDER       10
#define PVD_ID_MASK       GENMASK(31, 16)
#define PVD_ID_SHIFT      16
#define CLK_ID_MASK       GENMASK(15, 0)
#define CLK_ID_SHIFT      0
#define CLK_ID(pvd, idx)  ((pvd) << PVD_ID_SHIFT | (idx)) /* add pvd_id */
#define CLK_IDX(id)       ((id) & CLK_ID_MASK >> CLK_ID_SHIFT) /* remove pvd_id */

#define MHZ (1000 * 1000)

struct mtk_clk;

struct mtk_clk_ops {
    int        (*enable)(struct mtk_clk *mck);
    void        (*disable)(struct mtk_clk *mck);
    int        (*is_enabled)(struct mtk_clk *mck);
    unsigned long    (*recalc_rate)(struct mtk_clk *mck, unsigned long parent_rate);
    long        (*round_rate)(struct mtk_clk *mck, unsigned long rate, unsigned long *parent_rate);
    int        (*set_parent)(struct mtk_clk *mck, uint8_t index);
    uint8_t        (*get_parent)(struct mtk_clk *mck);
    int        (*set_rate)(struct mtk_clk *mck, unsigned long rate, unsigned long parent_rate);
} __attribute__((packed));

#define CLK_SET_RATE_PARENT    BIT(0)    /* propagate rate change up one level */
#define MTK_CLK_MUX_INDEX_BIT        BIT(1)

struct clk {
    /* nothing here */
};

struct mtk_clk {
    struct clk clk;
    int         id;
    const char *name;
    int                enable_cnt;
    uint8_t                num_parents;
    uint32_t flags;

    union {
        struct mtk_clk        *parent;
        struct mtk_clk        **parents;
    } p;

    union {
        const char        *parent_name;
        const char * const    *parent_names;
    } pn;

    const struct mtk_clk_ops    *ops;
} __attribute__((packed));

#define mck_from_clk(c)    container_of(c, struct mtk_clk, clk)
#define clk_from_mck(m)    ((m) ? &(m)->clk : NULL)

struct mtk_clk_data {
    struct mtk_clk **mcks;
    unsigned int nclks;
} __attribute__((packed));

struct mtk_clk_provider {
    const char *name;
    struct mtk_clk_data *data;
} __attribute__((packed));

struct mtk_clk_data *mtk_alloc_clk_data(unsigned int clk_num);
struct clk *mtk_clk_register_clk(struct mtk_clk *mck);
int mtk_clk_add_provider(int pvd_id, const char *name, struct mtk_clk_data *clk_data);

struct mtk_clk_fixed_rate {
    struct mtk_clk        mck;
    unsigned long        rate;
} __attribute__((packed));

#define fixed_from_mtk_clk(m)    container_of(m, struct mtk_clk_fixed_rate, mck)
#define mtk_clk_from_fixed(f)    ((f) ? &(f)->mck : NULL)

extern const struct mtk_clk_ops mtk_clk_fixed_rate_ops;

#define FIXED_CLK(_id, _name, _parent, _rate) {        \
    .mck.id = _id,                    \
    .mck.name = _name,                \
    .mck.pn.parent_name = _parent,            \
    .mck.num_parents = (_parent) ? 1 : 0,        \
    .mck.ops = &mtk_clk_fixed_rate_ops,        \
    .rate = _rate,                    \
}

struct mtk_clk_fixed_factor {
    struct mtk_clk        mck;
    unsigned int        mult;
    unsigned int        div;
} __attribute__((packed));

#define factor_from_mtk_clk(m)    container_of(m, struct mtk_clk_fixed_factor, mck)
#define mtk_clk_from_factor(f)    ((f) ? &(f)->mck : NULL)

extern const struct mtk_clk_ops mtk_clk_fixed_factor_ops;

#define FACTOR(_id, _name, _parent, _mult, _div) {    \
    .mck.id = _id,                    \
    .mck.name = _name,                \
    .mck.pn.parent_name = _parent,            \
    .mck.num_parents = (_parent) ? 1 : 0,        \
    .mck.ops = &mtk_clk_fixed_factor_ops,        \
    .mult = _mult,                    \
    .div = _div,                    \
}

struct mtk_clk_gate_regs {
    uint32_t        sta_ofs;
    uint32_t        clr_ofs;
    uint32_t        set_ofs;
};

struct mtk_clk_gate {
    struct mtk_clk            mck;
    void                    *gate_base;
    const struct mtk_clk_gate_regs    *regs;
    uint8_t                shift;
} __attribute__((packed));

#define gate_from_mtk_clk(m)    container_of(m, struct mtk_clk_gate, mck)
#define mtk_clk_from_gate(g)    ((g) ? &(g)->mck : NULL)

extern const struct mtk_clk_ops mtk_clk_gate_ops;
extern const struct mtk_clk_ops mtk_clk_gate_inv_ops;
extern const struct mtk_clk_ops mtk_clk_gate_setclr_ops;
extern const struct mtk_clk_ops mtk_clk_gate_setclr_inv_ops;

#define GATE(_id, _name, _parent, _base, _shift, _regs, _ops) {    \
    .mck.id = _id,                        \
    .mck.name = _name,                    \
    .mck.pn.parent_name = _parent,                \
    .mck.num_parents = (_parent) ? 1 : 0,            \
    .mck.ops = &_ops,                    \
    .gate_base = _base,                 \
    .regs = &_regs,                        \
    .shift = _shift,                    \
}

struct mtk_clk_mux {
    struct mtk_clk        mck;
    void             *mux_base;
    uint32_t mux_ofs;
    uint32_t mux_set_ofs;
    uint32_t mux_clr_ofs;
    uint32_t upd_ofs;

    uint8_t mux_shift;
    uint8_t mux_width;
    uint8_t gate_shift;
    uint8_t upd_shift;

    uint8_t        flags;
} __attribute__((packed));

#define mux_from_mtk_clk(m)    container_of(m, struct mtk_clk_mux, mck)
#define mtk_clk_from_mux(x)    ((x) ? &(x)->mck : NULL)

extern const struct mtk_clk_ops mtk_clk_mux_ops;
extern const struct mtk_clk_ops mtk_clk_mux_clr_set_upd_ops;

#define MUX(_id, _name, _parents, _base, _ofs, _shift, _width) {    \
    .mck.id = _id,                        \
    .mck.name = _name,                    \
    .mck.pn.parent_names = _parents,            \
    .mck.num_parents = ARRAY_SIZE(_parents),        \
    .mck.ops = &mtk_clk_mux_ops,                \
    .mux_base = _base,                    \
    .mux_ofs = _ofs,                    \
    .mux_shift = _shift,                    \
    .mux_width = _width,                    \
}

#define MUX_FLAGS(_id, _name, _parents, _base, _ofs, _shift, _width, _mux_flags) {    \
    .mck.id = _id,                        \
    .mck.name = _name,                    \
    .mck.pn.parent_names = _parents,            \
    .mck.num_parents = ARRAY_SIZE(_parents),        \
    .mck.ops = &mtk_clk_mux_ops,                \
    .mux_base = _base,                    \
    .mux_ofs = _ofs,                    \
    .mux_shift = _shift,                    \
    .mux_width = _width,                    \
    .flags = _mux_flags,            \
}

#define CLR_SET_UPD_FLAGS(_id, _name, _parents, _base, _mux_ofs, _mux_set_ofs,\
            _mux_clr_ofs, _shift, _width, _gate,        \
            _upd_ofs, _upd, _mux_flags) {            \
    .mck.id = _id,                        \
    .mck.name = _name,                    \
    .mck.pn.parent_names = _parents,            \
    .mck.num_parents = ARRAY_SIZE(_parents),        \
    .mck.ops = &mtk_clk_mux_clr_set_upd_ops,                \
    .mux_base = _base,                    \
    .mux_ofs = _mux_ofs,                    \
    .mux_set_ofs = _mux_set_ofs,                \
    .mux_clr_ofs = _mux_clr_ofs,                \
    .upd_ofs = _upd_ofs,                    \
    .mux_shift = _shift,                    \
    .mux_width = _width,                    \
    .gate_shift = _gate,                    \
    .upd_shift = _upd,                    \
    .flags = _mux_flags,            \
}

struct mtk_pll_div_table {
    uint32_t div;
    unsigned long freq;
} __attribute__((packed));

struct mtk_clk_pll {
    struct mtk_clk            mck;
    void                 *pll_base;
    uint32_t            base_ofs;
    uint32_t            pwr_ofs;
    uint32_t            pcw_ofs;
    uint8_t                pcw_shift;
    uint8_t                pcwbits;
    uint8_t                pcwibits;
    uint32_t                pcw_chg_ofs;
    uint32_t            pd_ofs;
    uint8_t                pd_shift;
    uint32_t            tuner_ofs;
    uint32_t            tuner_en_ofs;
    uint8_t                tuner_en_shift;
    uint32_t            en_mask;
    uint32_t            rst_bar_mask;
    uint32_t            flags;
    unsigned long            fmax;
    unsigned long            fmin;
    const struct mtk_pll_div_table    *div_table;
} __attribute__((packed));

#define pll_from_mtk_clk(m)    container_of(m, struct mtk_clk_pll, mck)
#define mtk_clk_from_pll(p)    ((p) ? &(p)->mck : NULL)

extern const struct mtk_clk_ops mtk_clk_pll_ops;

#define PLL(_id, _name, _parent, _base, _ofs, _pwr_ofs,            \
        _pcw_ofs, _pcw_shift, _pcwbits, _pd_ofs, _pd_shift,    \
        _tuner_ofs, _tuner_en_ofs, _tuner_en_shift,        \
        _en_mask, _rst_bar_mask, _flags, _fmax, _div_table) {    \
    .mck.id = _id,                            \
    .mck.name = _name,                        \
    .mck.pn.parent_name = _parent,                    \
    .mck.num_parents = (_parent) ? 1 : 0,                \
    .mck.ops = &mtk_clk_pll_ops,                    \
    .pll_base = _base,                         \
    .base_ofs = _ofs,                        \
    .pwr_ofs = _pwr_ofs,                        \
    .pcw_ofs = _pcw_ofs,                        \
    .pcw_shift = _pcw_shift,                    \
    .pcwbits = _pcwbits,                        \
    .pd_ofs = _pd_ofs,                        \
    .pd_shift = _pd_shift,                        \
    .tuner_ofs = _tuner_ofs,                    \
    .tuner_en_ofs = _tuner_en_ofs,                    \
    .tuner_en_shift = _tuner_en_shift,                \
    .en_mask = _en_mask,                        \
    .rst_bar_mask = _rst_bar_mask,                    \
    .flags = _flags,                        \
    .fmax = _fmax,                            \
    .div_table = _div_table,                    \
}

struct mtk_clk_div {
    struct mtk_clk        mck;
    void             *div_base;
    uint32_t        div_ofs;
    uint8_t            div_shift;
    uint8_t            div_width;
} __attribute__((packed));

#define div_from_mtk_clk(m)    container_of(m, struct mtk_clk_div, mck)
#define mtk_clk_from_div(d)    ((d) ? &(d)->mck : NULL)

extern const struct mtk_clk_ops mtk_clk_div_ops;

#define DIV(_id, _name, _parent, _base, _ofs, _shift, _width) {    \
    .mck.id = _id,                        \
    .mck.name = _name,                    \
    .mck.pn.parent_name = _parent,                \
    .mck.num_parents = (_parent) ? 1 : 0,            \
    .mck.ops = &mtk_clk_div_ops,                \
    .div_base = _base,                    \
    .div_ofs = _ofs,                    \
    .div_shift = _shift,                    \
    .div_width = _width,                    \
}

struct clk *clk_from_name(const char *name);
void mtk_clk_disable_unused(void);
struct mtk_clk_provider *get_mtk_clk_pvds(void);

#endif /* _MTK_CLK_PROVIDER_H_ */
