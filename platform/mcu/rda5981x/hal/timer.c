#include "aos/hal/timer.h"
#include "rda5981.h"
#include <stdint.h>
#include <stddef.h>

#define TICKER_TIMER0             (RDA_TIM0)
#define TICKER_TIMER1             (RDA_TIM1)

#define TIMER0_CONTROL_ENABLE       (0x01)
#define TIMER0_CONTROL_MODE         (0x02)
#define TIMER0_CONTROL_INT_MSK      (0x04)
#define TIMER1_CONTROL_ENABLE       (0x20)
#define TIMER1_CONTROL_MODE         (0x40)
#define TIMER1_CONTROL_INT_MSK      (0x80)
#define US_TIMER_HZ                 (1000000)

hal_timer_cb_t timer_cb = NULL;
void *arg = NULL;
timer_config_t timer0_config;
timer_config_t timer1_config;

void ticker0_irq_handler()
{
    if(timer0_config.reload_mode == TIMER_RELOAD_AUTO){
        /* Disable timer */
        TICKER_TIMER0->TCTRL &= (~TIMER0_CONTROL_ENABLE);

        /* Enable timer */
        TICKER_TIMER0->TCTRL |= (TIMER0_CONTROL_ENABLE);
    }
    timer0_config.cb(timer0_config.arg);
}

void ticker1_irq_handler()
{
    if(timer1_config.reload_mode == TIMER_RELOAD_AUTO){
        /* Disable timer */
        TICKER_TIMER1->TCTRL &= (~TIMER1_CONTROL_ENABLE);
        /* Delay 125us at least */
        {
            unsigned int idx = (SystemCoreClock / US_TIMER_HZ / 4UL) * 125UL;
            volatile unsigned int regval = 0U;
            while (idx--)
                regval ++;
        }
        /* Enable timer */
        TICKER_TIMER1->TCTRL |= (TIMER1_CONTROL_ENABLE);
    }
    timer1_config.cb(timer1_config.arg);
}


int32_t hal_timer_init(timer_dev_t *tim)
{
    /* Enable apb timer clock */
    RDA_SCU->CLKGATE1 |= (0x01UL << 3);
    if(tim->port == 0){
        /* Set timer0 mode */
        TICKER_TIMER0->TCTRL |= (TIMER0_CONTROL_MODE);

        /* mask timer0, disable an overflow int */
        TICKER_TIMER0->TCTRL |= (TIMER0_CONTROL_INT_MSK);
    }else if(tim->port == 1){
        /* Set timer1 mode */
        TICKER_TIMER1->TCTRL |= (TIMER1_CONTROL_MODE);

        /* mask timer1, disable an overflow int */
        TICKER_TIMER1->TCTRL |= (TIMER1_CONTROL_INT_MSK);
    }
    rda_timer_irq_set();

    return 0;
}

int32_t hal_timer_start(timer_dev_t *tim)
{
    if(tim->port == 0){
        /* Set period mode */
        if(tim->config.reload_mode == TIMER_RELOAD_MANU)
            RDA_GPIO->REVID &= ~(0x01UL << 25);
        else
            RDA_GPIO->REVID |= (0x01UL << 25);

        /* Set timer0 count */
        TICKER_TIMER0->LDCNT = tim->config.period * 40 * 1000;

        /* Enable timer0 */
        TICKER_TIMER0->TCTRL |= (TIMER0_CONTROL_ENABLE);

        /* Unmask timer0, enable an overflow int */
        TICKER_TIMER0->TCTRL &= (~(TIMER0_CONTROL_INT_MSK));

        timer0_config = tim->config;
    }else if(tim->port == 1){
        /* Set period mode */
        if(tim->config.reload_mode == TIMER_RELOAD_MANU)
            RDA_GPIO->REVID &= ~(0x01UL << 26);
        else
            RDA_GPIO->REVID |= (0x01UL << 26);

        /* Set timer1 count */
        if(timer1_config.reload_mode == TIMER_RELOAD_AUTO)
            TICKER_TIMER1->LDCNT = (tim->config.period * 32768)/1000 - 4;
        else
            TICKER_TIMER1->LDCNT = (tim->config.period * 32768)/1000;
        /* Enable timer1 */
        TICKER_TIMER1->TCTRL |= (TIMER1_CONTROL_ENABLE);

        /* Unmask timer, enable an overflow int */
        TICKER_TIMER1->TCTRL &= (~(TIMER1_CONTROL_INT_MSK));

        timer1_config = tim->config;
    }
    return 0;
}

void hal_timer_stop(timer_dev_t *tim)
{
    if(tim->port == 0){
        /* Disable timer0 */
        TICKER_TIMER0->TCTRL &= (~TIMER0_CONTROL_ENABLE);

        /* mask timer0, disable an overflow int */
        TICKER_TIMER0->TCTRL |= (TIMER0_CONTROL_INT_MSK);
    }if(tim->port == 1){
        /* Disable timer1 */
        TICKER_TIMER1->TCTRL &= (~TIMER1_CONTROL_ENABLE);

        /* mask timer1, disable an overflow int */
        TICKER_TIMER1->TCTRL |= (TIMER1_CONTROL_INT_MSK);
    }
    return;
}

int32_t hal_timer_finalize(timer_dev_t *tim)
{
    if(tim->port == 0){
        /* Disable timer0 */
        TICKER_TIMER0->TCTRL &= (~TIMER0_CONTROL_ENABLE);

        /* mask timer0, disable an overflow int */
        TICKER_TIMER0->TCTRL |= (TIMER0_CONTROL_INT_MSK);
    }if(tim->port == 1){
        /* Disable timer1 */
        TICKER_TIMER1->TCTRL &= (~TIMER1_CONTROL_ENABLE);

        /* mask timer1, disable an overflow int */
        TICKER_TIMER1->TCTRL |= (TIMER1_CONTROL_INT_MSK);
    }
    return 0;
}

