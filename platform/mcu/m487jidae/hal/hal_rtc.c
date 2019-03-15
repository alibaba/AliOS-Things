/* Microcontroller Library
 * Copyright (c) 2015-2016 Nuvoton
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

#include "objects.h"
#include "nu_modutil.h"
#include "nu_bitutil.h"
#include "hal_status.h"
#include <k_api.h>

#include "aos/hal/rtc.h"
#include "board.h"

typedef enum  {
	E_RTC_GET,
	E_RTC_SET
} E_RTC_ACT;

static int  g_rtc_inited = 0;

static int rtc_pow(int a,int b) 
{ 
	int n=1;
	int x=0;
	
	for( x=0; x<b; x++ ) n=n*a; 
	return n; 
} 

static uint32_t DEC2BCD( uint32_t val )
{
  return (uint32_t) ((val / 10 * 16) + (val % 10));
}

static uint32_t BCD2DEC( uint32_t val )
{
  return (uint32_t) ((val / 16 * 10) + (val % 16));
}

static int hal_rtc_format_transform ( E_RTC_ACT eRTCAct, S_RTC_TIME_DATA_T* psRtcTData, rtc_time_t *time,  uint8_t toformat )
{
	/*
    uint8_t sec;         DEC format:value range from 0 to 59, BCD format:value range from 0x00 to 0x59
    uint8_t min;         DEC format:value range from 0 to 59, BCD format:value range from 0x00 to 0x59
    uint8_t hr;          DEC format:value range from 0 to 23, BCD format:value range from 0x00 to 0x23
    uint8_t weekday;     DEC format:value range from 1 to  7, BCD format:value range from 0x01 to 0x07
    uint8_t date;        DEC format:value range from 1 to 31, BCD format:value range from 0x01 to 0x31
    uint8_t month;       DEC format:value range from 1 to 12, BCD format:value range from 0x01 to 0x12
    uint8_t year;        DEC format:value range from 0 to 99, BCD format:value range from 0x00 to 0x99
	*/
	
	switch ( eRTCAct )
	{
		case E_RTC_GET:	// S_RTC_TIME_DATA_T to rtc_time_t
				if ( toformat == HAL_RTC_FORMAT_BCD )
				{
					time->sec 		= DEC2BCD(psRtcTData->u32Second);
					time->min 		= DEC2BCD(psRtcTData->u32Minute);
					time->hr 			= DEC2BCD(psRtcTData->u32Hour);
					time->weekday = DEC2BCD( (psRtcTData->u32DayOfWeek==0)?7:psRtcTData->u32DayOfWeek );
					time->date 		= DEC2BCD(psRtcTData->u32Day);
					time->month 	= DEC2BCD(psRtcTData->u32Month);
					time->year 		= DEC2BCD(psRtcTData->u32Year-RTC_YEAR2000);
					
				} else {
					
					time->sec 		= psRtcTData->u32Second;
					time->min 		= psRtcTData->u32Minute;
					time->hr 			= psRtcTData->u32Hour;
					time->weekday = (psRtcTData->u32DayOfWeek==0)?7:psRtcTData->u32DayOfWeek;
					time->date 		= psRtcTData->u32Day;
					time->month 	= psRtcTData->u32Month;
					time->year 		= psRtcTData->u32Year-RTC_YEAR2000;					
				}
				break;
		
		case E_RTC_SET: // rtc_time_t to S_RTC_TIME_DATA_T
				if ( toformat == HAL_RTC_FORMAT_BCD )
				{
					psRtcTData->u32Second 		= BCD2DEC(time->sec);
					psRtcTData->u32Minute 		= BCD2DEC(time->min);
					psRtcTData->u32Hour				= BCD2DEC(time->hr);
					psRtcTData->u32DayOfWeek 	= BCD2DEC(time->weekday%7);
					psRtcTData->u32Day				= BCD2DEC(time->date);
					psRtcTData->u32Month	 		= BCD2DEC(time->month);
					psRtcTData->u32Year 			= BCD2DEC(time->year)+RTC_YEAR2000;
					psRtcTData->u32TimeScale  = RTC_CLOCK_24;
				} else {
					psRtcTData->u32Second 		= time->sec;
					psRtcTData->u32Minute 		= time->min;
					psRtcTData->u32Hour				= time->hr;
					psRtcTData->u32DayOfWeek 	= time->weekday%7;
					psRtcTData->u32Day				= time->date;
					psRtcTData->u32Month	 		= time->month;
					psRtcTData->u32Year 			= time->year+RTC_YEAR2000;
					psRtcTData->u32TimeScale  = RTC_CLOCK_24;
				}				
				break;
	}
}
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
	if ( !rtc )
		goto exit_hal_rtc_init;
	
	if ( !g_rtc_inited )
	{
		CLK->APBCLK0 |= CLK_APBCLK0_RTCCKEN_Msk; // RTC Clock Enable
		RTC_Open(NULL);
		g_rtc_inited = 1;
	}
	
	return HAL_OK;

exit_hal_rtc_init:	
		
	return HAL_ERROR;
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
	S_RTC_TIME_DATA_T sInitTime;

	if ( !rtc || !time )
		return HAL_ERROR;

	if ( !g_rtc_inited )
		return HAL_ERROR;
	else {
		RTC_GetDateAndTime(&sInitTime);
		hal_rtc_format_transform ( E_RTC_GET, &sInitTime, time, rtc->config.format);
	}
	
	return HAL_OK;
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
	S_RTC_TIME_DATA_T sInitTime;

	if ( !rtc  || !time )
		return HAL_ERROR;

	if ( !g_rtc_inited )
		return HAL_ERROR;
	else
	{
		hal_rtc_format_transform ( E_RTC_SET, &sInitTime, time, rtc->config.format);
		RTC_SetDateAndTime(&sInitTime);
	}	
	return HAL_OK;
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
	if ( g_rtc_inited )
  {
		RTC_Close();
		g_rtc_inited = 0;
	}
	return HAL_OK;
}
