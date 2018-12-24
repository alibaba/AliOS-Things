/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef HAL_RTC_H
#define HAL_RTC_H
#include "stdint.h"
#include "string.h"
#include "aos/errno.h"
#include "fm33a0xx_include_all.h"
#include "board.h"

// ============== Fm33A0X Gpio Define Begin =============== //
uint8_t hex_to_bcd( uint8_t value )
{
    return ( ((value / 10) << 4) + (value % 10) );
}

uint8_t bcd_to_hex( uint8_t value )
{
    return ( ((value & 0xF0) >> 4) * 10 + (value & 0x0F) );
}

// read rtc
uint8_t get_RTC(RTC_TimeDateTypeDef* time)
{
    uint8_t i, j;
    uint8_t result = 1;
    RTC_TimeDateTypeDef temp_time1, temp_time2;
    
    for (i = 0; i < 3; i++)
    {
        RTC_TimeDate_GetEx(&temp_time1);//read time
        RTC_TimeDate_GetEx(&temp_time2);//read time again
        
        for (j = 0; j < 7; j++)//if same, success
        {
            if(((uint8_t*)(&temp_time1))[j] != ((uint8_t*)(&temp_time2))[j])
            {
                break;
            }
        }
    
        if(7 == j)
        {
            result = 0;
            memcpy((uint8_t *)(time), (uint8_t *)(&temp_time1), 7);//update time    
            break;
        }
    }
  
  if (1 == result)
  {
    return EIO;
  }
  
  return 0;
}

// ============== Fm33A0X Gpio Define End =============== //


#define HAL_RTC_FORMAT_DEC  1
#define HAL_RTC_FORMAT_BCD  2

typedef struct {
    uint8_t  format;    /* time formart 0-DEC or 1-BCD */
} rtc_config_t;

typedef struct {
    uint8_t port;        /* rtc port */
    rtc_config_t config; /* rtc config */
    void   *priv;        /* priv data */
} rtc_dev_t;

/*
 * RTC time
 */
typedef struct {
    uint8_t sec;         /* DEC format:value range from 0 to 59, BCD format:value range from 0x00 to 0x59 */
    uint8_t min;         /* DEC format:value range from 0 to 59, BCD format:value range from 0x00 to 0x59 */
    uint8_t hr;          /* DEC format:value range from 0 to 23, BCD format:value range from 0x00 to 0x23 */
    uint8_t weekday;     /* DEC format:value range from 1 to  7, BCD format:value range from 0x01 to 0x07 */
    uint8_t date;        /* DEC format:value range from 1 to 31, BCD format:value range from 0x01 to 0x31 */
    uint8_t month;       /* DEC format:value range from 1 to 12, BCD format:value range from 0x01 to 0x12 */
    uint8_t year;        /* DEC format:value range from 0 to 99, BCD format:value range from 0x00 to 0x99 */
} rtc_time_t;

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
    if (NULL == rtc)
    {
        return EIO;
    }
    
    RCC_PERCLK_SetableEx(RTCCLK, ENABLE);     //RTC Clk enable
    
    RTC_FSEL_FSEL_Set(RTC_FSEL_FSEL_PLL1HZ);//Frequency output signal
    RTC_Trim_Proc(0);                       //Trim set
    RTC_PR1SEN_PR1SEN_Setable(DISABLE);     //PR1SEN disable
                                            
    RTC_STAMPEN_STAMP0EN_Setable(DISABLE);  //STAMP Disable
    RTC_STAMPEN_STAMP1EN_Setable(DISABLE);  
    
    RTC_RTCIE_SetableEx(DISABLE, 0xFFFFFFFF);//Disable RTC IRQ
  
    return 0;
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
    RTC_TimeDateTypeDef rtc_time;
  
    if ((NULL == rtc) ||
        (NULL == time))
    {
        return EIO;
    }
    
    if (EIO == get_RTC(&rtc_time))
    {
        return EIO;
    }
  
    //update time 
    if (0 == rtc->config.format)
    {//DEC
        time->sec = bcd_to_hex(rtc_time.Second);
        time->min = bcd_to_hex(rtc_time.Minute);
        time->hr = bcd_to_hex(rtc_time.Hour);
        time->weekday = bcd_to_hex(rtc_time.Week);
        time->date = bcd_to_hex(rtc_time.Date);
        time->month = bcd_to_hex(rtc_time.Month);
        time->year = bcd_to_hex(rtc_time.Year);
    }
    else
    {//BCD
        time->sec = rtc_time.Second;
        time->min = rtc_time.Minute;
        time->hr = rtc_time.Hour;
        time->weekday = rtc_time.Week;
        time->date = rtc_time.Date;
        time->month = rtc_time.Month;
        time->year = rtc_time.Year;
    }
  
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
    uint8_t i, j;
    uint8_t result = 1;
    RTC_TimeDateTypeDef temp_time, para;
  
    if ((NULL == rtc) ||
        (NULL == time))
    {
        return EIO;
    }
    
    if (0 == rtc->config.format)
    {//DEC
        para.Second = bcd_to_hex(time->sec); 
        para.Minute = bcd_to_hex(time->min);
        para.Hour = bcd_to_hex(time->hr);
        para.Week = bcd_to_hex(time->weekday);
        para.Date = bcd_to_hex(time->date);
        para.Month = bcd_to_hex(time->month);
        para.Year = bcd_to_hex(time->year);
    }
    else
    {//BCD
        para.Second = time->sec; 
        para.Minute = time->min;
        para.Hour = time->hr;
        para.Week = time->weekday;
        para.Date = time->date;
        para.Month = time->month;
        para.Year = time->year;
    }
  
    for (i = 0; i < 3; i++)
    {
        RTC_RTCWE_Write(RTC_WRITE_ENABLE);  //RTC write enable
        RTC_TimeDate_SetEx(&para);                //set RTC
        RTC_RTCWE_Write(RTC_WRITE_DISABLE); //RTC write Disable
    
        result = get_RTC(&temp_time);   //read and compare RTC
        if (0 == result)
        {
            result = 1;
            for (j = 0; j < 7; j++) //if same, success
            {
                if(((uint8_t *)(&temp_time))[i] != ((uint8_t *)(&para))[i])
                {
                    break;
                }
            }
        }
    
        if(7 == j)
        {
            result = 0;
            break;
        }
    }
  
    if (1 == result)
    {
        return EIO;
    }
  
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
  if (NULL == rtc)
  {
    return EIO;
  }
  
  RTC_Deinit();
  
  return 0;
}

#endif /* HAL_RTC_H */

