
#include <stdint.h>
#include "realview.h"
#include "irq.h"
#include "board.h"

#define RT_TICK_PER_SECOND 100

#define TIMER01_HW_BASE                 0x10011000
#define TIMER23_HW_BASE                 0x10012000

#define TIMER_LOAD(hw_base)             __REG32(hw_base + 0x00)
#define TIMER_VALUE(hw_base)            __REG32(hw_base + 0x04)
#define TIMER_CTRL(hw_base)             __REG32(hw_base + 0x08)
#define TIMER_CTRL_ONESHOT              (1 << 0)
#define TIMER_CTRL_32BIT                (1 << 1)
#define TIMER_CTRL_DIV1                 (0 << 2)
#define TIMER_CTRL_DIV16                (1 << 2)
#define TIMER_CTRL_DIV256               (2 << 2)
#define TIMER_CTRL_IE                   (1 << 5)        /* Interrupt Enable (versatile only) */
#define TIMER_CTRL_PERIODIC             (1 << 6)
#define TIMER_CTRL_ENABLE               (1 << 7)

#define TIMER_INTCLR(hw_base)           __REG32(hw_base + 0x0c)
#define TIMER_RIS(hw_base)              __REG32(hw_base + 0x10)
#define TIMER_MIS(hw_base)              __REG32(hw_base + 0x14)
#define TIMER_BGLOAD(hw_base)           __REG32(hw_base + 0x18)

#define TIMER_LOAD(hw_base)             __REG32(hw_base + 0x00)
#define TIMER_VALUE(hw_base)            __REG32(hw_base + 0x04)
#define TIMER_CTRL(hw_base)             __REG32(hw_base + 0x08)
#define TIMER_CTRL_ONESHOT              (1 << 0)
#define TIMER_CTRL_32BIT                (1 << 1)
#define TIMER_CTRL_DIV1                 (0 << 2)
#define TIMER_CTRL_DIV16                (1 << 2)
#define TIMER_CTRL_DIV256               (2 << 2)
#define TIMER_CTRL_IE                   (1 << 5)        /* Interrupt Enable (versatile only) */
#define TIMER_CTRL_PERIODIC             (1 << 6)
#define TIMER_CTRL_ENABLE               (1 << 7)

#define TIMER_INTCLR(hw_base)           __REG32(hw_base + 0x0c)
#define TIMER_RIS(hw_base)              __REG32(hw_base + 0x10)
#define TIMER_MIS(hw_base)              __REG32(hw_base + 0x14)
#define TIMER_BGLOAD(hw_base)           __REG32(hw_base + 0x18)

#define SYS_CTRL                        __REG32(REALVIEW_SCTL_BASE)
#define TIMER_HW_BASE                   REALVIEW_TIMER2_3_BASE
#define TIMER_HW_OFFSET                 (0x20u)

static void rt_hw_timer_isr(int vector, void *param)
{
    /* clear interrupt */
    TIMER_INTCLR(TIMER_HW_BASE) = 0x01;
    RTOS_Tick_Handler();
}

int rt_hw_timer_init(int tick_per_second)
{
    uint32_t val;
    /*
	 * set clock frequency:
	 *	REALVIEW_REFCLK is 32KHz
	 *	REALVIEW_TIMCLK is 1MHz
	 */
    SYS_CTRL |= REALVIEW_TIMCLK;

    /* Setup Timer2 for generating irq */
    val = TIMER_CTRL(TIMER_HW_BASE);
    val &= ~TIMER_CTRL_ENABLE;
    val |= (TIMER_CTRL_32BIT | TIMER_CTRL_PERIODIC | TIMER_CTRL_IE);
    TIMER_CTRL(TIMER_HW_BASE) = val;

    TIMER_LOAD(TIMER_HW_BASE) = 1000;

    /* enable timer */
    TIMER_CTRL(TIMER_HW_BASE) |= TIMER_CTRL_ENABLE;

    /* Setup Timer3 for free-running */
    val = TIMER_CTRL(TIMER_HW_BASE + TIMER_HW_OFFSET);
    val &= ~TIMER_CTRL_ENABLE;
    val &= ~TIMER_CTRL_PERIODIC;
    val &= ~TIMER_CTRL_IE;
    val |= TIMER_CTRL_32BIT;
    TIMER_CTRL(TIMER_HW_BASE + TIMER_HW_OFFSET) = val;

    /* enable timer */
    TIMER_CTRL(TIMER_HW_BASE + TIMER_HW_OFFSET) |= TIMER_CTRL_ENABLE;

    (void)irq_register_isr(IRQ_PBA8_TIMER2_3, rt_hw_timer_isr);
    gic_enable_interrupt(IRQ_PBA8_TIMER2_3);

    return 0;
}

unsigned int hw_timer3_count(void)
{
    return(TIMER_VALUE(TIMER_HW_BASE + TIMER_HW_OFFSET));
}

int hw_timer0_init(int tick_per_second)
{
    uint32_t val;

    /* Setup Timer0 for generating irq */
    val = TIMER_CTRL(TIMER01_HW_BASE);
    val &= ~TIMER_CTRL_ENABLE;
    val &= ~TIMER_CTRL_PERIODIC;
    val &= ~TIMER_CTRL_IE;
    val |= TIMER_CTRL_32BIT;
    TIMER_CTRL(TIMER01_HW_BASE) = val;

    /* enable timer */
    TIMER_CTRL(TIMER01_HW_BASE) |= TIMER_CTRL_ENABLE;

    return 0;
}
unsigned int hw_timer0_count(void)
{
    return(TIMER_VALUE(TIMER01_HW_BASE));
}

#if 0
void timer_init(int timer, unsigned int preload)
{
    uint32_t val;

    if (timer == 0)
    {
        /* Setup Timer0 for generating irq */
        val = TIMER_CTRL(TIMER01_HW_BASE);
        val &= ~TIMER_CTRL_ENABLE;
        val |= (TIMER_CTRL_32BIT | TIMER_CTRL_PERIODIC | TIMER_CTRL_IE);
        TIMER_CTRL(TIMER01_HW_BASE) = val;

        TIMER_LOAD(TIMER01_HW_BASE) = preload;

        /* enable timer */
        TIMER_CTRL(TIMER01_HW_BASE) |= TIMER_CTRL_ENABLE;
    }
    else
    {
        /* Setup Timer1 for generating irq */
        val = TIMER_CTRL(TIMER23_HW_BASE);
        val &= ~TIMER_CTRL_ENABLE;
        val |= (TIMER_CTRL_32BIT | TIMER_CTRL_PERIODIC | TIMER_CTRL_IE);
        TIMER_CTRL(TIMER23_HW_BASE) = val;

        TIMER_LOAD(TIMER23_HW_BASE) = preload;

        /* enable timer */
        TIMER_CTRL(TIMER23_HW_BASE) |= TIMER_CTRL_ENABLE;
    }
}

void timer_clear_pending(int timer)
{
    if (timer == 0)
    {
        TIMER_INTCLR(TIMER01_HW_BASE) = 0x01;
    }
    else
    {
        TIMER_INTCLR(TIMER23_HW_BASE) = 0x01;
    }
}
#endif
