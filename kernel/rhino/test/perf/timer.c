/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>

#define putreg32(v, x) (*(volatile uint32_t*)(x) = (v))
#define getreg32(x)    (*(volatile uint32_t *)(x))
#define HOBBIT_TIMER0_BASE (0x50000000)
#define TIMER0_LOAD_COUNT  (volatile uint32_t *)(HOBBIT_TIMER0_BASE + 0x00)
#define TIMER0_CUR_VAL     (volatile uint32_t *)(HOBBIT_TIMER0_BASE + 0x04)
#define TIMER0_CONTROL     (volatile uint32_t *)(HOBBIT_TIMER0_BASE + 0x08)
#define TIMER0_EOI         (volatile uint32_t *)(HOBBIT_TIMER0_BASE + 0x0C)
#define TIMER0_INT_STAT    (volatile uint32_t *)(HOBBIT_TIMER0_BASE + 0x10)




#define TIMER_ENABLE   (1UL << 0)
#define TIMER_MOD_FREE (0UL << 1)
#define TIMER_MOD_USER (1UL << 1)
#define TIMER_INT_MASK (1UL << 2)

static inline uint32_t readreg32(volatile uint32_t *addr)
{
    return *(volatile uint32_t *)addr;
}

static inline void writereg32(uint32_t val, volatile uint32_t *addr)
{
    *(volatile uint32_t *)addr = val;
}

void hobbit_timer0_clr(void)
{
    readreg32(TIMER0_EOI);
}

uint32_t hobbit_timer0_get_curval(void)
{
    return getreg32(TIMER0_CUR_VAL);
}

void hobbit_timer0_stop(void)
{
    uint32_t reg;

    reg  = getreg32(TIMER0_CONTROL);
    reg &= ~(TIMER_ENABLE);
    putreg32(reg, TIMER0_CONTROL);
}

void hobbit_timer0_init(uint32_t hz)
{
    uint32_t reg;

    writereg32(hz, TIMER0_LOAD_COUNT);

    reg  = readreg32(TIMER0_CONTROL);
    reg |= TIMER_MOD_USER;
    reg |= TIMER_ENABLE;
    reg &= ~(TIMER_INT_MASK);
    writereg32(reg, TIMER0_CONTROL);

    return;
}

void hobbit_timer0_start(void)
{
    uint32_t reg;

    reg  = getreg32(TIMER0_CONTROL);
    reg |= TIMER_ENABLE;
    putreg32(reg, TIMER0_CONTROL);
}



