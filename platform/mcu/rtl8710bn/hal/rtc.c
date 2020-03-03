#include "ameba_soc.h"
#include "k_api.h"
#include "board.h"
#include "device.h"
#include "aos/hal/rtc.h"
#include <time.h>
#include "rtc_api.h"

static struct tm rtc_timeinfo;
static int rtc_en = 0;

const static u8 dim[12] = {
    31, 0, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

/**
  * @brief  This function is used to tell a year is a leap year or not.
  * @param  year: The year need to be told.
  * @retval value:
  *             - 1: This year is leap year.
  *             - 0: This year is not leap year.
  */
static inline bool is_leap_year(unsigned int year)
{
    u32 full_year = year + 1900;
    return (!(full_year % 4) && (full_year % 100)) || !(full_year % 400);
}


/**
  * @brief  This function tells how many days in a month of a year.
  * @param  year: Specified year
  * @param  month: Specified month
  * @retval value: Number of days in the month.
  */
static u8 days_in_month (u8 month, u8 year)
{
    u8 ret = dim[month];
    if (ret == 0)
        ret = is_leap_year (year) ? 29 : 28;
    return ret;
}

/**
  * @brief  This function is used to calculate month and day of month according to year and day of the year.
  * @param  year: years since 1900.
  * @param  yday: day of the year.
  * @param  mon: pointer to the variable which stores month,  the value can be 0-11
  * @param  mday: pointer to the variable which stores day of month, the value can be 1-31
  * @retval value: none
  */
static void rtc_calculate_mday(int year, int yday, int* mon, int* mday)
{
    int t_mon = -1, t_yday = yday + 1;

    while(t_yday > 0){
        t_mon ++;
        t_yday -= days_in_month(t_mon, year);
    }

    *mon = t_mon;
    *mday = t_yday + days_in_month(t_mon, year);
}

/**
  * @brief  This function is used to calculate day of week according to date.
  * @param  year: years since 1900.
  * @param  mon: which month of the year
  * @param  mday: pointer to the variable which store day of month
  * @param  wday: pointer to the variable which store day of week, the value can be 0-6, and 0 means Sunday
  * @retval value: none
  */
static void rtc_calculate_wday(int year, int mon, int mday, int* wday)
{
    int t_year = year + 1900, t_mon = mon + 1;

    if(t_mon == 1 || t_mon == 2){
        t_year --;
        t_mon += 12;
    }

    int c = t_year / 100;
    int y = t_year % 100;
    int week = (c / 4) - 2 * c + (y + y / 4) + (26 * (t_mon + 1) / 10) + mday -1;

    while(week < 0){
        week += 7;
    }
    week %= 7;

    *wday = week;
}

static u32  get_yday(rtc_time_t *time)
{
    int days = time->date;

    for(int i = 0; i < time->month-1; i++){
        days += dim[i];
    }
}

/**
  * @brief  This function is used to restore rtc_timeinfo global variable whose value is lost after system reset.
  * @param  none
  * @retval value: none
  */
static void rtc_restore_timeinfo(rtc_dev_t *rtc)
{
    u32 value, days_in_year, format;

    if(rtc->config.format == HAL_RTC_FORMAT_DEC)
        format = RTC_Format_BIN;
    else if(rtc->config.format == HAL_RTC_FORMAT_BCD)
        format = RTC_Format_BCD;

    RTC_TimeTypeDef RTC_TimeStruct;
    RTC_GetTime(format, &RTC_TimeStruct);
    rtc_timeinfo.tm_sec = RTC_TimeStruct.RTC_Seconds;
    rtc_timeinfo.tm_min = RTC_TimeStruct.RTC_Minutes;
    rtc_timeinfo.tm_hour = RTC_TimeStruct.RTC_Hours;
    rtc_timeinfo.tm_yday = RTC_TimeStruct.RTC_Days;

    value = BKUP_Read(0);

    rtc_timeinfo.tm_year = (value & BIT_RTC_BACKUP) >> 8;

    days_in_year =  (is_leap_year(rtc_timeinfo.tm_year) ? 366 : 365);
    if(rtc_timeinfo.tm_yday > days_in_year - 1){
        rtc_timeinfo.tm_year ++;
        rtc_timeinfo.tm_yday -= days_in_year;

        /* over one year, update days in RTC_TR */
        RTC_TimeStruct.RTC_Days = rtc_timeinfo.tm_yday;
        RTC_SetTime(format, &RTC_TimeStruct);
    }

    rtc_calculate_mday(rtc_timeinfo.tm_year, rtc_timeinfo.tm_yday, &rtc_timeinfo.tm_mon, &rtc_timeinfo.tm_mday);
    rtc_calculate_wday(rtc_timeinfo.tm_year, rtc_timeinfo.tm_mon, rtc_timeinfo.tm_mday, &rtc_timeinfo.tm_wday);
}


/**
  * @brief  Initializes the RTC device, include clock, RTC registers and function.
  * @param  none
  * @retval  none
  */

/**
 * This function will initialize the on board CPU real time clock
 *
 *
 * @param[in]  rtc  rtc device
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_rtc_init(rtc_dev_t *rtc)
{
    RTC_InitTypeDef RTC_InitStruct;

    RTC_ClokSource(0);

    RTC_StructInit(&RTC_InitStruct);
    RTC_InitStruct.RTC_HourFormat = RTC_HourFormat_24;

    RTC_Init(&RTC_InitStruct);

    /* 32760 need add need add 15 cycles (256Hz) every 4 min*/
    //RTC_SmoothCalibConfig(RTC_CalibSign_Positive, 15,
    // RTC_CalibPeriod_4MIN, RTC_Calib_Enable);

    rtc_en = 1;
}

/**
 * This function will return the value of time read from the on board CPU real time clock.
 *
 * @param[in]   rtc   rtc device
 * @param[out]  time  pointer to a time structure
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_rtc_get_time(rtc_dev_t *rtc, rtc_time_t *time)
{
    int format;
    struct tm tm_temp;
    RTC_TimeTypeDef RTC_TimeStruct;
    u32 delta_days = 0;

    if(BKUP_Read(0) & BIT_RTC_RESTORE){
        rtc_restore_timeinfo(rtc);
        BKUP_Clear(0, BIT_RTC_RESTORE);
    }

    _memcpy((void*)&tm_temp, (void*)&rtc_timeinfo, sizeof(struct tm));

    if(rtc->config.format == HAL_RTC_FORMAT_DEC)
        format = RTC_Format_BIN;
    else if(rtc->config.format == HAL_RTC_FORMAT_BCD)
        format = RTC_Format_BCD;
    /*hour, min, sec get from RTC*/
    RTC_GetTime(format, &RTC_TimeStruct);
    tm_temp.tm_sec = RTC_TimeStruct.RTC_Seconds;
    tm_temp.tm_min = RTC_TimeStruct.RTC_Minutes;
    tm_temp.tm_hour = RTC_TimeStruct.RTC_Hours;

    /* calculate how many days later from last time update rtc_timeinfo */
    delta_days = RTC_TimeStruct.RTC_Days - tm_temp.tm_yday;

    /* calculate  wday, mday, yday, mon, year*/
    tm_temp.tm_wday += delta_days;
    if(tm_temp.tm_wday >= 7){
        tm_temp.tm_wday = tm_temp.tm_wday % 7;
    }

    tm_temp.tm_yday += delta_days;
    tm_temp.tm_mday += delta_days;

    while(tm_temp.tm_mday > days_in_month(tm_temp.tm_mon, tm_temp.tm_year)){
        tm_temp.tm_mday -= days_in_month(tm_temp.tm_mon, tm_temp.tm_year);
        tm_temp.tm_mon++;

        if(tm_temp.tm_mon >= 12){
            tm_temp.tm_mon -= 12;
            tm_temp.tm_yday -= is_leap_year(tm_temp.tm_year) ? 366 : 365;
            tm_temp.tm_year ++;

            /* over one year, update days in RTC_TR */
            RTC_TimeStruct.RTC_Days = tm_temp.tm_yday;
            RTC_SetTime(RTC_Format_BIN, &RTC_TimeStruct);

            /* update rtc_timeinfo */
            _memcpy((void*)&rtc_timeinfo, (void*)&tm_temp, sizeof(struct tm));
        }
    }

    time->sec = tm_temp.tm_sec;
    time->min = tm_temp.tm_min;
    time->hr = tm_temp.tm_hour;
    time->weekday = tm_temp.tm_wday;
    time->date = tm_temp.tm_mday;
    time->month = tm_temp.tm_mon;
    time->year = tm_temp.tm_year;

    return 0;
}

/**
 * This function will set MCU RTC time to a new value.
 *
 * @param[in]   rtc   rtc device
 * @param[out]  time  pointer to a time structure
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_rtc_set_time(rtc_dev_t *rtc, const rtc_time_t *time)
{
    struct tm timeinfo;
    int format;

    timeinfo.tm_sec = time->sec;
    timeinfo.tm_min = time->min ;
    timeinfo.tm_hour= time->hr;
    timeinfo.tm_wday= time->weekday;
    timeinfo.tm_mday= time->date;
    timeinfo.tm_mon =time->month;
    timeinfo.tm_year= time->year;

    timeinfo.tm_yday = get_yday(time);

    RTC_TimeTypeDef RTC_TimeStruct;

    /*set time in RTC */
    RTC_TimeStruct.RTC_H12_PMAM = RTC_H12_AM;
    RTC_TimeStruct.RTC_Days = timeinfo.tm_yday;
    RTC_TimeStruct.RTC_Hours = timeinfo.tm_hour;
    RTC_TimeStruct.RTC_Minutes = timeinfo.tm_min;
    RTC_TimeStruct.RTC_Seconds = timeinfo.tm_sec;


    if(rtc->config.format == HAL_RTC_FORMAT_DEC)
        format = RTC_Format_BIN;
    else if(rtc->config.format == HAL_RTC_FORMAT_BCD)
        format = RTC_Format_BCD;

    RTC_SetTime(format, &RTC_TimeStruct);

    /* Set rtc_timeinfo*/
    _memcpy((void*)&rtc_timeinfo, (void*)&timeinfo, sizeof(struct tm));

    return 0;
}
/**
 * De-initialises an RTC interface, Turns off an RTC hardware interface
 *
 * @param[in]  RTC  the interface which should be de-initialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_rtc_finalize(rtc_dev_t *rtc)
{
    rtc_en = 0;
}

