#include "hal/soc/timer.h"
#include "rda5981.h"
#include <stdint.h>
#include <stddef.h>

#define US_TICKER_TIMER             (RDA_TIM0)

#define TIMER0_CONTROL_ENABLE       (0x01)
#define TIMER0_CONTROL_MODE         (0x02)
#define TIMER0_CONTROL_INT_MSK      (0x04)

hal_timer_cb_t timer_cb = NULL;
void *arg = NULL;
timer_config_t timer_config;

void us_ticker_irq_handler()
{
    if(timer_config.reload_mode == TIMER_RELOAD_AUTO){
        /* Disable timer */
        US_TICKER_TIMER->TCTRL &= (~TIMER0_CONTROL_ENABLE);

        /* Enable timer */
        US_TICKER_TIMER->TCTRL |= (TIMER0_CONTROL_ENABLE);
    }
    timer_config.cb(timer_config.arg);
}

int32_t hal_timer_init(timer_dev_t *tim)
{
    /* Enable apb timer clock */
    RDA_SCU->CLKGATE1 |= (0x01UL << 3);

    /* Set timer mode */
    US_TICKER_TIMER->TCTRL |= (TIMER0_CONTROL_MODE);

    /* mask timer, disable an overflow int */
    US_TICKER_TIMER->TCTRL |= (TIMER0_CONTROL_INT_MSK);
    
    rda_timer_irq_set();
    
    return 0;
}

int32_t hal_timer_start(timer_dev_t *tim)
{
    /* Set period mode */
    if(tim->config.reload_mode == TIMER_RELOAD_MANU)
        RDA_GPIO->REVID &= ~(0x01UL << 25);
    else
        RDA_GPIO->REVID |= (0x01UL << 25);

    /* Set timer count */
    US_TICKER_TIMER->LDCNT = tim->config.period * 40;

    /* Enable timer */
    US_TICKER_TIMER->TCTRL |= (TIMER0_CONTROL_ENABLE);
    
    /* Unmask timer, enable an overflow int */
    US_TICKER_TIMER->TCTRL &= (~(TIMER0_CONTROL_INT_MSK));

    timer_config = tim->config;
    return 0;
}

void hal_timer_stop(timer_dev_t *tim)
{
    /* Disable timer */
    US_TICKER_TIMER->TCTRL &= (~TIMER0_CONTROL_ENABLE);

    /* mask timer, disable an overflow int */
    US_TICKER_TIMER->TCTRL |= (TIMER0_CONTROL_INT_MSK);

    return;
}

int32_t hal_timer_finalize(timer_dev_t *tim)
{
    /* Disable timer */
    US_TICKER_TIMER->TCTRL &= (~TIMER0_CONTROL_ENABLE);

    /* mask timer, disable an overflow int */
    US_TICKER_TIMER->TCTRL |= (TIMER0_CONTROL_INT_MSK);
}

