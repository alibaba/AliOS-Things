#include "rda5981.h"

#define rTIMER_INTSTATE     (RDA_TIMINTST->INTST)
#define RDA_TIMER_IRQn      (TIMER_IRQn)
#define TIMER0_CONTROL_ENABLE       (0x01)

extern void ticker0_irq_handler();
extern void ticker1_irq_handler();
extern void free_ticker_irq_handle();

static void rda_timer_isr(void)
{
    uint32_t int_status = rTIMER_INTSTATE & 0x000FUL;
    if (int_status & (0x01UL << 2)){
        volatile uint32_t us_ticker_clrInt = RDA_TIM0->INTCLR;
        ticker0_irq_handler();
    }
    if (int_status & (0x01UL << 3)){
        volatile uint32_t lp_ticker_clrInt = RDA_TIM1->INTCLR;
        ticker1_irq_handler();
    }
    if(int_status & (0x01UL << 1)){
        RDA_SCU->PWRCTRL |= (0x01UL << 28); // clear int
        __DSB();
        while (RDA_SCU->PWRCTRL & (0x01UL << 28));
        RDA_SCU->PWRCTRL |= (0x01UL << 27); // clear timestamp
        __DSB();
        while (RDA_SCU->PWRCTRL & (0x01UL << 27));
        free_ticker_irq_handle();
    }
}

void rda_timer_irq_set(void)
{
    NVIC_SetVector(RDA_TIMER_IRQn, (uint32_t)rda_timer_isr);
    NVIC_SetPriority(RDA_TIMER_IRQn, 0x1FUL);
    NVIC_EnableIRQ(RDA_TIMER_IRQn);
}
