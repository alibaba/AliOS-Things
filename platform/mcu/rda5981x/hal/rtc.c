#include "aos/hal/rtc.h"
#include <stdint.h>
#include "rda5981.h"

#define rFREE_TIMER_INITVAL         (RDA_SCU->FTMRINITVAL)
#define rFREE_TIMER_PRESCALE        (RDA_SCU->FTMRPREVAL)
#define rFREE_TIMER_TIMESTAMP       (RDA_SCU->FTMRTS)
#define rRDA_POWER_CONTROL          (RDA_SCU->PWRCTRL)
#define rTIMER1_CONTROL             (RDA_TIM1->TCTRL)
#define rTIMER1_INTCLR              (RDA_TIM1->INTCLR)
#define FREE_TIMER_INIT_VAL         (0xFFFFFFFFUL)
#define US_TIMER_HZ                 (1000000)

rtc_time_t rtc_time = {0, 0, 0, 1, 1, 1, 0};
static uint32_t sec_suppliment = 0;
void free_ticker_irq_handle()
{
    uint32_t delta,sec,min,hr,weekday,date,month,year;

    delta = 0xFFFFFFFF;
    sec = ((delta+sec_suppliment)/32768);
    sec_suppliment = ((delta+sec_suppliment)%32768);

    min = sec/60;
    min += (rtc_time.sec + sec%60)/60;
    rtc_time.sec = (rtc_time.sec + sec%60)%60;
    hr = min/60;
    hr += (rtc_time.min + min%60)/60;
    rtc_time.min = (rtc_time.min + min%60)%60;
    date = hr/24;
    date += (rtc_time.hr + hr%24)/24;
    rtc_time.hr = (rtc_time.hr + hr%24)%24;
    month = (rtc_time.date + date)/32;
    rtc_time.date = (rtc_time.date + date)%32;
    if(rtc_time.date == 0)
        rtc_time.date = 1;
    year = (rtc_time.month + month)/13;
    rtc_time.month = (rtc_time.month + month)%13;
    if(rtc_time.month == 0)
        rtc_time.month = 1;
    rtc_time.year = (rtc_time.year + year)%100;
    rtc_time.weekday = (rtc_time.weekday + date)%8;
    if(rtc_time.weekday == 0)
        rtc_time.weekday = 1;

    return;
}

int32_t hal_rtc_init(rtc_dev_t *rtc)
{
    rda_ccfg_aontmr_cksrc(1);

    /* Enable apb timer clock */
    RDA_SCU->CLKGATE1 |= (0x01UL << 3);

    /* Set free timer power */
    rda_ccfg_aontmr();

    /* Delay 300us at least */
    {
        unsigned int idx = (SystemCoreClock / US_TIMER_HZ / 4UL) * 300UL;
        unsigned int regval = 0U;
        while (idx--) {
            regval += rFREE_TIMER_TIMESTAMP;
        }
    }

    /* Set free timer write_en */
    rRDA_POWER_CONTROL |= (uint32_t)(0x01UL << 8);

    rFREE_TIMER_INITVAL = FREE_TIMER_INIT_VAL;

    /* Enable int */
    rRDA_POWER_CONTROL |= (0x01UL << 18);

    /* Unmask int */
    rTIMER1_CONTROL &= ~(0x01UL << 3);

    /* Set free timer prescale */
    rFREE_TIMER_PRESCALE = 0U;

    /* Enable free timer */
    rRDA_POWER_CONTROL |= (uint32_t)(0x01UL << 17);

    /* Clr free timer write_en */
    rRDA_POWER_CONTROL &= (~(uint32_t)(1 << 8));

    sec_suppliment = 0;
    rda_timer_irq_set();
}


int32_t hal_rtc_get_time(rtc_dev_t *rtc, rtc_time_t *time)
{
    uint32_t delta,sec,min,hr,weekday,date,month,year;

    delta = rFREE_TIMER_TIMESTAMP;

    RDA_SCU->PWRCTRL |= (0x01UL << 27); // clear timestamp
    __DSB();
    while (RDA_SCU->PWRCTRL & (0x01UL << 27));

    sec = ((delta+sec_suppliment)/32768);
    sec_suppliment = ((delta+sec_suppliment)%32768);

    min = sec/60;
    min += (rtc_time.sec + sec%60)/60;
    rtc_time.sec = (rtc_time.sec + sec%60)%60;
    hr = min/60;
    hr += (rtc_time.min + min%60)/60;
    rtc_time.min = (rtc_time.min + min%60)%60;
    date = hr/24;
    date += (rtc_time.hr + hr%24)/24;
    rtc_time.hr = (rtc_time.hr + hr%24)%24;
    month = (rtc_time.date + date)/32;
    rtc_time.date = (rtc_time.date + date)%32;
    if(rtc_time.date == 0)
        rtc_time.date = 1;
    year = (rtc_time.month + month)/13;
    rtc_time.month = (rtc_time.month + month)%13;
    if(rtc_time.month == 0)
        rtc_time.month = 1;
    rtc_time.year = (rtc_time.year + year)%100;
    rtc_time.weekday = (rtc_time.weekday + date)%8;
    if(rtc_time.weekday == 0)
        rtc_time.weekday = 1;

    memcpy(time, &rtc_time, sizeof(rtc_time_t));

}

int32_t hal_rtc_set_time(rtc_dev_t *rtc, const rtc_time_t *time)
{
    RDA_SCU->PWRCTRL |= (0x01UL << 27); // clear timestamp
    __DSB();
    while (RDA_SCU->PWRCTRL & (0x01UL << 27));
    sec_suppliment = 0;
    memcpy(&rtc_time, time, sizeof(rtc_time_t));
}

int32_t hal_rtc_finalize(rtc_dev_t *rtc)
{
    /* Set free timer write_en */
    rRDA_POWER_CONTROL |= (uint32_t)(0x01UL << 8);

    /* Disnable int */
    rRDA_POWER_CONTROL &= ~(0x01UL << 18);

    /* mask int */
    rTIMER1_CONTROL |= (0x01UL << 3);

    /* Disable free timer */
    rRDA_POWER_CONTROL &= ~(0x01UL << 17);

    /* Clr free timer write_en */
    rRDA_POWER_CONTROL &= (~(uint32_t)(1 << 8));

    sec_suppliment = 0;
    return 0;
}

