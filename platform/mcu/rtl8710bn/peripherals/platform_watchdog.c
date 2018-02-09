/**
******************************************************************************
* @file    paltform_watchdog.c 
* @author  William Xu
* @version V1.0.0
* @date    05-May-2014
* @brief   This file provide WDG driver functions.
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


//#include "MICOPlatform.h"
//#include "MICORTOS.h"
#include "Common.h"
#include "debug.h"

#include "board.h"
#include "platform_peripheral.h"

/******************************************************
 *                    Constants
 ******************************************************/

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
 *               Variables Definitions
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/

OSStatus platform_watchdog_init( uint32_t timeout_ms )
{
// PLATFORM_TO_DO
#ifndef MICO_DISABLE_WATCHDOG
  	OSStatus err = kNoErr;

	watchdog_init(timeout_ms);
  	watchdog_start();	
exit:
  return err;
#else
  UNUSED_PARAMETER( timeout_ms );
  return kUnsupportedErr;
#endif
}

OSStatus MicoWdgFinalize( void )
{
    	// PLATFORM_TO_DO
    	return kNoErr;
}

bool platform_watchdog_check_last_reset( void )
{
	u32 value_bk0 = BKUP_Read(BKUP_REG0);

	printf("backup reg0 0x%x\r\n", value_bk0);
	if(value_bk0 & BIT_CPU_RESET_HAPPEN){		//system reboot
		if(!(value_bk0 & BIT_SYS_RESET_HAPPEN)){
    			return true;
		}
		else
			BKUP_Clear(BKUP_REG0, BIT_SYS_RESET_HAPPEN);			
	}

    	return false;
}

OSStatus platform_watchdog_kick( void )
{
#ifndef MICO_DISABLE_WATCHDOG
	watchdog_refresh();
  	return kNoErr;
#else
  	return kUnsupportedErr;
#endif
}


