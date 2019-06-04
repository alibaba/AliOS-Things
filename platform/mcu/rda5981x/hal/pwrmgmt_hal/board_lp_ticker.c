/* mbed Microcontroller Library
 * Copyright (c) 2006-2018 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#if (PWRMGMT_CONFIG_CPU_LOWPOWER > 0)

#include <cpu_tickless.h>
#include "rda5981.h"
#include "rda_ccfg_api.h"

#define rFREE_TIMER_INITVAL         (RDA_SCU->FTMRINITVAL)
#define rFREE_TIMER_PRESCALE        (RDA_SCU->FTMRPREVAL)
#define rFREE_TIMER_TIMESTAMP       (RDA_SCU->FTMRTS)
#define rRDA_POWER_CONTROL          (RDA_SCU->PWRCTRL)
#define rTIMER1_CONTROL             (RDA_TIM1->TCTRL)
#define rTIMER1_INTCLR              (RDA_TIM1->INTCLR)
#define FREE_TIMER_INIT_VAL         (0xFFFFFFFFUL)

#define US_TIMER_HZ                 (1000000)

#define LP_TIMER_CLOCK_SOURCE       (32768)

static int    lp_ticker_inited = 0;
//static int32_t    lp_ticker_int_flag = 0;
//int32_t    lp_ticker_soft_int_flag = 0;

extern void rda_timer_irq_set(void);
extern void rda_ccfg_aontmr_cksrc(int cksrc);

static pwr_status_t lpticker_init(void);
static uint32_t     lpticker_one_shot_max_seconds(void);
static pwr_status_t lpticker_one_shot_start(uint64_t planUs);
static pwr_status_t lpticker_one_shot_stop(uint64_t *pPassedUs);
static void lp_timer_isr(void);
static uint32_t last_timetamp;

one_shot_timer_t lpticker_one_shot = {
    lpticker_init,
    lpticker_one_shot_max_seconds,
    lpticker_one_shot_start,
    lpticker_one_shot_stop,
};

pwr_status_t lpticker_init(void)
{
    uint32_t regval;
    if (lp_ticker_inited)
        return;

    rda_ccfg_aontmr_cksrc(1);

    /* Enable apb timer clock */
    RDA_SCU->CLKGATE1 |= (0x01UL << 3);

    /* Set free timer power */
    rda_ccfg_aontmr();

    /* Delay 300us at least */
    {
        unsigned int idx = (SystemCoreClock / US_TIMER_HZ / 4UL) * 300UL;
        regval = 0U;
        while (idx--) {
            regval += rFREE_TIMER_TIMESTAMP;
        }
    }

    /* Set free timer write_en */
    rRDA_POWER_CONTROL |= (uint32_t)(0x01UL << 8);

    rFREE_TIMER_INITVAL = FREE_TIMER_INIT_VAL;

    /* Enable int */
    //rTIMER1_CONTROL |= (0x01UL << 4);
    rRDA_POWER_CONTROL &= ~(0x01UL << 18);

    /* mask int */
    //rTIMER1_CONTROL |= (0x01UL << 3);
    /* Unmask int */
    rTIMER1_CONTROL &= ~(0x01UL << 3);

    /* Set free timer prescale */
    rFREE_TIMER_PRESCALE = 0U;

    /* Enable free timer */
    rRDA_POWER_CONTROL |= (uint32_t)(0x01UL << 17);

    /* Clr free timer write_en */
    rRDA_POWER_CONTROL &= (~(uint32_t)(1 << 8));

    NVIC_SetVector(TIMER_IRQn, (uint32_t)lp_timer_isr);
    NVIC_SetPriority(TIMER_IRQn, 0x1FUL);
    NVIC_EnableIRQ(TIMER_IRQn);

    /* Set lp_ticker_inited true, after all settings done */
    lp_ticker_inited = 1;

	return PWR_OK;
}
#if 0
static inline uint32_t lp_ticker_read(void)
{
    if (!lp_ticker_inited)
        lpticker_init();

    /* Get timestamp */
    return (uint32_t)(rFREE_TIMER_TIMESTAMP);
}

void lp_ticker_set_interrupt(timestamp_t timestamp)
{
    uint32_t delta = (int32_t)(timestamp - lp_ticker_read());

    /* Set free timer write_en */
    rRDA_POWER_CONTROL |= (uint32_t)(0x01UL << 8);

    rFREE_TIMER_INITVAL = delta;

    /* Enable int */
    //rTIMER1_CONTROL |= (0x01UL << 4);
    rRDA_POWER_CONTROL |= (0x01UL << 18);

    /* Unmask int */
    //rTIMER1_CONTROL &= ~(0x01UL << 3);

    /* Clr free timer write_en */
    rRDA_POWER_CONTROL &= (~(uint32_t)(1 << 8));

    lp_ticker_int_flag = 1;
}

void lp_ticker_int_callback()
{
    /* Check the flag firstly, because following hanlder can change it */
    if (lp_ticker_soft_int_flag) {
        lp_ticker_soft_int_flag = 0;
    }else if(lp_ticker_int_flag){
        //mbed_error_printf("lp\r\n");
        lp_ticker_int_flag = 0;

        /* Set free timer write_en */
        rRDA_POWER_CONTROL |= (uint32_t)(0x01UL << 8);
        /* Disable int */
        rRDA_POWER_CONTROL &= ~(0x01UL << 18);
        /* Clr free timer write_en */
        rRDA_POWER_CONTROL &= (~(uint32_t)(1 << 8));

        rRDA_POWER_CONTROL |= (0x01UL << 28);
        __DSB();
        while (rRDA_POWER_CONTROL & (0x01UL << 28));
    }
    return;
}

void lp_ticker_fire_interrupt(void)
{
    lp_ticker_soft_int_flag = 1;
    NVIC_SetPendingIRQ(TIMER_IRQn);
}
#endif

static inline void lp_ticker_disable_interrupt(void)
{
    /* Disable int */
    rRDA_POWER_CONTROL &= ~(0x01UL << 18);
}

static inline void lp_ticker_clear_interrupt(void)
{
    rRDA_POWER_CONTROL |= (0x01UL << 28); // clear int
    __DSB();
    while (rRDA_POWER_CONTROL & (0x01UL << 28));
}

static inline void lp_ticker_free(void)
{
    /* Set free timer write_en */
    rRDA_POWER_CONTROL |= (uint32_t)(0x01UL << 8);
	
    /* Disable free timer */
    rRDA_POWER_CONTROL &= ~(0x01UL << 17);

    /* Clr free timer write_en */
    rRDA_POWER_CONTROL &= (~(uint32_t)(1 << 8));
}

static pwr_status_t lpticker_one_shot_start(uint64_t planUs)
{
#if (PWRMGMT_CONFIG_LOG_ENTERSLEEP > 0)
     static sys_time_t last_log_entersleep = 0;
#endif
    uint32_t cc_counter = planUs * LP_TIMER_CLOCK_SOURCE / 1000000;
	//printf("%s:%d\r\n",__func__,cc_counter);
    if (cc_counter < 3) {
        return PWR_ERR;
    }

#if (PWRMGMT_CONFIG_LOG_ENTERSLEEP > 0)
    if (krhino_sys_tick_get() > (last_log_entersleep + RHINO_CONFIG_TICKS_PER_SECOND)) {
        last_log_entersleep = krhino_sys_tick_get();
        printf("enter sleep %d ms\r\n", (uint32_t) planUs/1000);
    }
#endif

    /* Set free timer write_en */
    rRDA_POWER_CONTROL |= (uint32_t)(0x01UL << 8);
	
    last_timetamp = rFREE_TIMER_TIMESTAMP;
	
    rFREE_TIMER_INITVAL = cc_counter;

    /* Enable int */
    //rTIMER1_CONTROL |= (0x01UL << 4);
    rRDA_POWER_CONTROL |= (0x01UL << 18);

    /* Enable free timer */
    rRDA_POWER_CONTROL |= (uint32_t)(0x01UL << 17);

    /* Unmask int */
    //rTIMER1_CONTROL &= ~(0x01UL << 3);

    /* Clr free timer write_en */
    rRDA_POWER_CONTROL &= (~(uint32_t)(1 << 8));
	
    return PWR_OK;

}

uint8_t int_flag = 0; /* use this vale for hardware bug */

static pwr_status_t lpticker_one_shot_stop(uint64_t *pPassedUs)
{
    uint32_t interval = 0, timetamp = 0, delta = 0;
    timetamp = rFREE_TIMER_TIMESTAMP;
	interval = rFREE_TIMER_INITVAL;

    lp_ticker_disable_interrupt();
    if (int_flag == 1) {
        lp_ticker_clear_interrupt();
    }
    lp_ticker_free();

    /* fix hardware bug  */
    if ((timetamp < last_timetamp) && (last_timetamp < 0x0f000000)) {
	    printf("warning : lp timer wrapped\r\n");
	    last_timetamp = 0xffffffff;
    }

    delta = (timetamp >= last_timetamp) ? (timetamp - last_timetamp) : (timetamp + 0xffffffff - last_timetamp);
    *pPassedUs = (uint64_t)delta * 1000000 / LP_TIMER_CLOCK_SOURCE; 
//    *pPassedUs = *pPassedUs + 30;
	//mbed_error_printf("last:%d now:%d delta:%d\r\n",last_timetamp,timetamp,delta);
    //mbed_error_printf("%s:%d\r\n",__func__,*pPassedUs);
    return PWR_OK;
}

static uint32_t lpticker_one_shot_max_seconds(void){
    return (FREE_TIMER_INIT_VAL/LP_TIMER_CLOCK_SOURCE);
}

static void lp_timer_isr(void){
	//mbed_error_printf("%s\r\n",__func__);
    int_flag = 1;
	krhino_intrpt_enter();
    int_flag = 0;
	krhino_intrpt_exit();
}

#endif
