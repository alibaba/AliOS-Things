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

#ifndef _CLK_H
#define _CLK_H

#include <stdint.h>
#include <stdbool.h>

struct clk;

#define CFG_CLK_PM_SUPPORT

#ifdef CFG_CLK_PM_SUPPORT

/**
 * clk_get - lookup and obtain a reference to a clock producer.
 * @id: clock index
 *
 * Returns a struct clk corresponding to the clock producer, or
 * NULL pointer if failed.
 */
struct clk *clk_get(int id);

/**
 * clk_put    - "free" the clock source
 * @clk: clock source
 */
void clk_put(struct clk *clk);

/**
 * is_err_or_null_clk    - check the clk source is available or not
 * @clk: clock source
 *
 * Returns true if the clk is invalid or NULL pointer.
 */
bool is_err_or_null_clk(struct clk *clk);

/**
 * clk_get_rate - obtain the current clock rate (in Hz) for a clock source.
 * @clk: clock source
 * Must not be called from isr.
 */
unsigned long clk_get_rate(struct clk *clk);

/**
 * clk_set_rate - set the clock rate for a clock source
 * @clk: clock source
 * @rate: desired clock rate in Hz
 * Must not be called from isr.
 */
int clk_set_rate(struct clk *clk, unsigned long rate);

/**
 * clk_enable - turn on a clock source.
 * @clk: clock source
 * Must not be called from isr.
 */
int clk_enable(struct clk *clk);

/**
 * clk_disable - turn off a clock source.
 * @clk: clock source
 * Must not be called from isr.
 */
int clk_disable(struct clk *clk);

/**
 * clk_set_parent - set the parent clock source for this clock
 * @clk: clock source
 * @parent: parent clock source
 * Must not be called from isr.
 */
int clk_set_parent(struct clk *clk, struct clk *clk_parent);

/**
 * clk_get_parent - get the parent clock source for this clock
 * @clk: clock source
 * Must not be called from isr.
 */
struct clk *clk_get_parent(struct clk *clk);

#else    /* !CFG_CLK_PM_SUPPORT */

static inline struct clk *clk_get(int id)
{
    return NULL;
}

static inline void clk_put(struct clk *clk)
{
}

static inline bool is_err_or_null_clk(struct clk *clk)
{
    return false;
}

static inline unsigned long clk_get_rate(struct clk *clk)
{
    return 0;
}

static inline int clk_set_rate(struct clk *clk, unsigned long rate)
{
    return 0;
}

static inline int clk_enable(struct clk *clk)
{
    return 0;
}

static inline int clk_disable(struct clk *clk)
{
    return 0;
}

static inline int clk_set_parent(struct clk *clk, struct clk *clk_parent)
{
    return 0;
}

static inline struct clk *clk_get_parent(struct clk *clk)
{
    return NULL;
}

#endif

#endif /* _CLK_H */
