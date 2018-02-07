/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "hal/soc/soc.h"
#include "mico_rtos.h"
#include "driver/chip/hal_rtc.h"

#define RTC_START_YEAR (1970)
/**@brief This function will initialize the on board CPU real time clock
 *
 * @note  This function should be called by MICO system when initializing clocks, so
 *        It is not needed to be called by application
 * @param     rtc   : rtc device
 * @return    0     : on success.
 * @return    EIO   : if an error occurred with any step
 */
int32_t hal_rtc_init(rtc_dev_t *rtc)
{
    
}

/**@brief This function will return the value of time read from the on board CPU real time clock. Time value must be given in the format of
 * the structure hal_rtc_time_t
 * @param     rtc  : rtc device
 * @param     time : pointer to a time structure
 *
 * @return    0    : on success.
 * @return    EIO  : if an error occurred with any step
 */
int32_t hal_rtc_get_time(rtc_dev_t *rtc, rtc_time_t *time)
{
    if (time != NULL) {
        uint8_t isLeapYear, year, month, mday;
        RTC_WeekDay wday;
        uint8_t hour, minute, second;
    
        HAL_RTC_GetYYMMDD(&isLeapYear, &year, &month, &mday);
        HAL_RTC_GetDDHHMMSS(&wday, &hour, &minute, &second);
        
        time->year = year;
        time->month = month;
        time->date = mday;
        time->hr = hour;
        time->min = minute;
        time->sec = second;
        printf("[hal_rtc]: get time %d-%d-%d, %d:%d:%d\n", 
            year, month, mday, hour, minute, second);
        return 0;
    } else
        return -1;
}

/**@brief This function will set MCU RTC time to a new value. Time value must be given in the format of
 * the structure hal_rtc_time_t
 * @param     rtc    : rtc device
 * @param     time   : pointer to a time structure
 *
 * @return    0      : on success.
 * @return    EIO    : if an error occurred with any step
 */
int32_t hal_rtc_set_time(rtc_dev_t *rtc, const rtc_time_t *time)
{
    if (time != NULL) {
        uint8_t isLeapYear = !(time->year % 400) || (!(time->year % 4) && (time->year % 100));
        HAL_RTC_SetYYMMDD(isLeapYear, time->year, time->month, time->date);
        HAL_RTC_SetDDHHMMSS(RTC_WDAY_TUESDAY, time->hr, time->min, time->sec); // Fixup: weakday
        printf("[hal_rtc]: set time %d-%d-%d, %d:%d:%d\n", 
                    time->year + RTC_START_YEAR, time->month, time->date,
                    time->hr, time->min, time->sec);
    } else
        return -1;
}

