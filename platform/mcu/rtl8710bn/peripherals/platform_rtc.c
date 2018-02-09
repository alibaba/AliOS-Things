/**
******************************************************************************
* @file    paltform_rtc.c 
* @author  William Xu
* @version V1.0.0
* @date    05-May-2014
* @brief   This file provide RTC driver functions.
******************************************************************************
*
*  The MIT License
*  Copyright (c) 2014 MXCHIP Inc.
*
*  Permission is hereby granted, free of charge, to any person obtaining a copy 
*  of this software and associated documentation files (the "Software"), to deal
*  in the Software without restriction, including without limitation the rights 
*  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*  copies of the Software, and to permit persons to whom the Software is furnished
*  to do so, subject to the following conditions:
*
*  The above copyright notice and this permission notice shall be included in
*  all copies or substantial portions of the Software.
*
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
*  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR 
*  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************
*/ 

//#include "MICORTOS.h"
//#include "MICOPlatform.h"

#include "board.h"
#include "platform_peripheral.h"
//#include "platform_mcu_peripheral.h" /* Include MCU-specific types */

#include "debug.h"

#include "timer_api.h"  
#include "rtc_api.h"
#include "wait_api.h"


/******************************************************
*                      Macros
******************************************************/
#define SW_RTC_TIMER_ID        TIMER5

#define MICO_VERIFY_TIME(time, valid) \
if( (time->sec > 60) || ( time->min > 60 ) || (time->hr > 24) || ( time->date > 31 ) || ( time->month > 12 )) \
  { \
    valid= false; \
  } \
else \
  { \
    valid= true; \
  }

/******************************************************
*                    Constants
******************************************************/
static int sw_rtc_en=0;

const static u8 dim[14] = { 
	31, 0, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 28 };

/******************************************************
*                   Enumerations
******************************************************/

/******************************************************
*                 Type Definitions
******************************************************/

/******************************************************
*                    Structures
******************************************************/
/******************************************************
 *               Static Function Declarations
 ******************************************************/

/******************************************************
*               Variables Definitions
******************************************************/
/******************************************************
*               Function Declarations
******************************************************/
/******************************************************
*               Function Definitions
******************************************************/
OSStatus platform_rtc_init(void)
{
#ifdef MICO_ENABLE_MCU_RTC
	rtc_init();	
	return kNoErr;
#else	
  	return kUnsupportedErr;
#endif 	
}

/**
* This function will return the value of time read from the on board CPU real time clock. Time value must be given in the format of
* the structure wiced_rtc_time_t
*
* @return    WICED_SUCCESS : on success.
* @return    WICED_ERROR   : if an error occurred with any step
*/
OSStatus platform_rtc_get_time( platform_rtc_time_t* time)
{
    	time_t seconds;
    	struct tm *timeinfo;

#ifdef MICO_ENABLE_MCU_RTC  
	if( time == NULL )
	{
	  return kParamErr;
	}

	/* fill structure */
	seconds = rtc_read();
	timeinfo = localtime(&seconds);

	time->sec = timeinfo->tm_sec;
	time->min = timeinfo->tm_min;
	time->hr = timeinfo->tm_hour;
	time->weekday = timeinfo->tm_wday;
	time->date = timeinfo->tm_mday;
	time->month= timeinfo->tm_mon+1;
	time->year= timeinfo->tm_year+1900;

	return kNoErr;
#else /* #ifdef WICED_ENABLE_MCU_RTC */
  	UNUSED_PARAMETER(time);
  	return kUnsupportedErr;
#endif /* #ifdef MICO_ENABLE_MCU_RTC */
}

/**
* This function will set MCU RTC time to a new value. Time value must be given in the format of
* the structure wiced_rtc_time_t
*
* @return    WICED_SUCCESS : on success.
* @return    WICED_ERROR   : if an error occurred with any step
*/
OSStatus platform_rtc_set_time( const platform_rtc_time_t* time )
{
#ifdef MICO_ENABLE_MCU_RTC
	struct tm timeinfo;
	struct tm *timeinfo_p;

	time_t time_in_seconds;
	bool    valid = false;
	  
	MICO_VERIFY_TIME(time, valid);
	if( valid == false )
	{
	  return kParamErr;
	}
  
	// Set the RTC
	timeinfo.tm_sec = time->sec;
	timeinfo.tm_min = time->min;
	timeinfo.tm_hour = time->hr+1;
	timeinfo.tm_mday = time->date;
	timeinfo.tm_mon = time->month-1;
	timeinfo.tm_year = time->year-1900; 
	
	time_in_seconds = mktime(&timeinfo);

	timeinfo_p = localtime(&time_in_seconds);
	//printf("platform_rtc_set_time %d %d %d %d %d %d %d\r\n", timeinfo_p->tm_year, timeinfo_p->tm_mon, timeinfo_p->tm_mday,
	//	timeinfo_p->tm_hour, timeinfo_p->tm_min, timeinfo_p->tm_sec, timeinfo_p->tm_wday);
	

	//printf("platform_rtc_set_time %lu \r\n", time_in_seconds);
       //DBG_8195A("Time as a basic string = %s", ctime(&time_in_seconds));

    	rtc_write(time_in_seconds);

	return kNoErr;
#else /* #ifdef MICO_ENABLE_MCU_RTC */
  	UNUSED_PARAMETER(time);
  	return kUnsupportedErr;
#endif /* #ifdef MICO_ENABLE_MCU_RTC */
}

OSStatus platform_rtc_enter_powersave ( void )
{
    return kUnsupportedErr;
}

OSStatus platform_rtc_abort_powersave( void )
{
    return kUnsupportedErr;
}

OSStatus platform_rtc_exit_powersave( uint32_t requested_sleep_time, uint32_t *cpu_sleep_time )
{
    return kUnsupportedErr;
}
