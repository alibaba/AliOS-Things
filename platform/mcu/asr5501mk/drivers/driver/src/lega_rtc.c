#include <stdio.h>
#include <time.h>
#include "lega_cm4.h"
#include "lega_common.h"
#include "lega_peripheral_reg.h"
#include "lega_rtc.h"

lega_rtc_time_t *p_lega_rtc_time = (lega_rtc_time_t *)(RTC_TIME_RETENTION_RAM_ADDR);

void SLEEP_IRQHandler(void)
{
    lega_intrpt_enter();
    struct tm tm1;
    struct tm *p_tm1;
    time_t time1;
    //RTC int sts check and clear
    if(REG_RD(LEGA_IRQ_STS_REG_ADDR) & (1 << SLEEP_IRQn))
    {
        //clear irq
        RTC->CTRL &= ~RTC_INT_ENABLE;

        tm1.tm_year = p_lega_rtc_time->year;
        tm1.tm_mon = p_lega_rtc_time->month;
        tm1.tm_mday = p_lega_rtc_time->date;
        tm1.tm_hour = p_lega_rtc_time->hr;
        tm1.tm_min = p_lega_rtc_time->min;
        tm1.tm_sec = p_lega_rtc_time->sec;

        time1 = RTC_MAX_DAY * SECOND_PER_DAY - 1;

        time1 += mktime(&tm1);

        p_tm1 = gmtime(&time1);

        p_lega_rtc_time->year = p_tm1->tm_year;
        p_lega_rtc_time->month = p_tm1->tm_mon;
        p_lega_rtc_time->date = p_tm1->tm_mday;
        p_lega_rtc_time->hr = p_tm1->tm_hour;
        p_lega_rtc_time->min = p_tm1->tm_min;
        p_lega_rtc_time->sec = p_tm1->tm_sec;
        p_lega_rtc_time->weekday = p_tm1->tm_wday;

        while(REG_RD(LEGA_IRQ_STS_REG_ADDR) & (1 << SLEEP_IRQn));
        //enable int
        RTC->CTRL |= RTC_INT_ENABLE;
    }
    lega_intrpt_exit();
}

/**
 * This function will initialize the on board CPU real time clock
 *
 *
 * @param[in]  rtc  rtc device
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t lega_rtc_init(lega_rtc_dev_t *rtc)
{
    if(NULL == rtc)
    {
        return -1;
    }
    if(0 == rtc->port)
    {
        if(!(RTC->CTRL & RTC_ENABLE)) //for deep sleep wake up reboot case consider
        {
            p_lega_rtc_time->year = RTC_INIT_YEAR;
            p_lega_rtc_time->month = RTC_INIT_MONTH;
            p_lega_rtc_time->date = RTC_INIT_DATE;
            p_lega_rtc_time->hr = RTC_INIT_HOUR;
            p_lega_rtc_time->min = RTC_INIT_MINUTE;
            p_lega_rtc_time->sec = RTC_INIT_SECOND;
            p_lega_rtc_time->weekday = RTC_INIT_WEEKDAY;

            RTC->CTRL &= ~RTC_ENABLE;
            RTC->CNT_TICK = RTC_TICK_CNT;
            RTC->CNT_DATE = (RTC_REFRESH_DAY << 17 | RTC_REFRESH_HOUR << 12 | RTC_REFRESH_MINUTE << 6 | RTC_REFRESH_SECOND);
            RTC->CTRL |= RTC_CNT_CYCLE_ENABLE | RTC_INT_ENABLE | RTC_ENABLE; //sel internal RC clock, calibration needed
            //RTC->CTRL |= RTC_CNT_CYCLE_ENABLE | RTC_INT_ENABLE | RTC_ENABLE | CLK32K_SEL | SW_OVERRIDE_REG_POR; //sel ext 32.768k XTAL
            NVIC_EnableIRQ(SLEEP_IRQn);
        }

        return 0;
    }
    return -1;
}

/**
 * This function will return the value of time read from the on board CPU real time clock.
 *
 * @param[in]   rtc   rtc device
 * @param[out]  time  pointer to a time structure
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t lega_rtc_get_time(lega_rtc_dev_t *rtc, lega_rtc_time_t *time)
{
    struct tm tm1;
    struct tm *p_tm1;
    time_t time1;

    if((NULL == rtc) || (NULL == time))
    {
        return -1;
    }

    if(0 == rtc->port)
    {
        time1 = GET_RTC_CURRENT_DATE();
        time1 = (((time1 >> 17) & 0xFF) * 86400 + ((time1 >> 12) & 0x1F) * 3600 + ((time1 >> 6) & 0x3F) * 60 + (time1 & 0x3F));

        tm1.tm_year = p_lega_rtc_time->year;
        tm1.tm_mon = p_lega_rtc_time->month;
        tm1.tm_mday = p_lega_rtc_time->date;
        tm1.tm_hour = p_lega_rtc_time->hr;
        tm1.tm_min = p_lega_rtc_time->min;
        tm1.tm_sec = p_lega_rtc_time->sec;

        time1 += mktime(&tm1);

        p_tm1 = gmtime(&time1);

        time->year = p_tm1->tm_year;
        time->month = p_tm1->tm_mon;
        time->date = p_tm1->tm_mday;
        time->hr = p_tm1->tm_hour;
        time->min = p_tm1->tm_min;
        time->sec = p_tm1->tm_sec;
        time->weekday = p_tm1->tm_wday;

        return 0;
    }
    return -1;
}

/**
 * This function will set MCU RTC time to a new value.
 *
 * @param[in]   rtc   rtc device
 * @param[out]  time  pointer to a time structure
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t lega_rtc_set_time(lega_rtc_dev_t *rtc, const lega_rtc_time_t *time)
{
    if((NULL == rtc) || (NULL == time))
    {
        return -1;
    }
    if(0 == rtc->port)
    {
        RTC->CTRL &= ~RTC_ENABLE;
        delay(200);
        RTC->CTRL |= RTC_ENABLE;
        *p_lega_rtc_time = *time;

        return 0;
    }
    return -1;
}

/**
 * De-initialises an RTC interface, Turns off an RTC hardware interface
 *
 * @param[in]  RTC  the interface which should be de-initialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t lega_rtc_finalize(lega_rtc_dev_t *rtc)
{
    if(NULL == rtc)
    {
        return -1;
    }
    if(0 == rtc->port)
    {
        NVIC_DisableIRQ(SLEEP_IRQn);
        RTC->CTRL &= ~(RTC_ENABLE | RTC_INT_ENABLE | RTC_CNT_CYCLE_ENABLE);
        RTC->CNT_TICK = 0;
        RTC->CNT_DATE = 0;
        return 0;
    }
    return -1;
}
