/**
******************************************************************************
* @file    platform_mcu_peripheral.c 
* @author  William Xu
* @version V1.0.0
* @date    05-May-2014
* @brief   This file provide functions called by MICO to drive stm32f2xx 
*          platform: - e.g. power save, reboot, platform initialize
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


#include "platform_peripheral.h"
#include "board.h"
//#include "mico_platform.h"
#include <string.h> // For memcmp
//#include "crt0.h"
//#include "mico_rtos.h"
#include "platform_init.h"

#include "debug.h"
#include "sleep_ex_api.h"

/******************************************************
*                      Macros
******************************************************/

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
*               Function Declarations
******************************************************/

#ifndef MICO_DISABLE_MCU_POWERSAVE
static unsigned long  stop_mode_power_down_hook( unsigned long sleep_ms );
#else
static unsigned long  idle_power_down_hook( unsigned long sleep_ms );
#endif


/******************************************************
*               Variables Definitions
******************************************************/

#ifndef MICO_DISABLE_MCU_POWERSAVE
static bool          wake_up_interrupt_triggered  = false;
static volatile uint32_t     rtk_clock_needed_counter  = 0;
#endif /* #ifndef MICO_DISABLE_MCU_POWERSAVE */

/******************************************************
 *               Function Definitions
 ******************************************************/

OSStatus platform_mcu_powersave_init(void)
{
    return kNoErr;
}

OSStatus platform_mcu_powersave_disable( void )
{
#ifndef MICO_DISABLE_MCU_POWERSAVE
    /* Atomic operation starts */
    //DISABLE_INTERRUPTS;

    //pmu_acquire_wakelock(PMU_OS);

    /* Atomic operation ends */
    //ENABLE_INTERRUPTS;
	
    return kNoErr;	
#else
    return kNoErr;
#endif
    return kNoErr;
}

OSStatus platform_mcu_powersave_enable( void )
{
#ifndef MICO_DISABLE_MCU_POWERSAVE
    /* Atomic operation starts */
    //DISABLE_INTERRUPTS;

    //pmu_release_wakelock(PMU_OS);
	
    /* Atomic operation ends */
    //ENABLE_INTERRUPTS;
	
    return kNoErr;	
#else
    return kNoErr;
#endif
    return kNoErr;
}

void platform_mcu_powersave_exit_notify( void )
{
#if 0
#ifndef MICO_DISABLE_MCU_POWERSAVE
    wake_up_interrupt_triggered = true;
#endif
#endif
}

/******************************************************
 *               RTOS Powersave Hooks
 ******************************************************/

void platform_idle_hook( void )
{
    //ENABLE_INTERRUPTS;
    //__asm("wfi");
}

uint32_t platform_power_down_hook( uint32_t sleep_ms )
{
#ifdef MICO_DISABLE_MCU_POWERSAVE
    /* If MCU powersave feature is disabled, enter idle mode when powerdown hook is called by the RTOS */
    return idle_power_down_hook( sleep_ms );

#else
    /* If MCU powersave feature is enabled, enter STOP mode when powerdown hook is called by the RTOS */
    return stop_mode_power_down_hook( sleep_ms );

#endif

	return 0;
}

#ifdef MICO_DISABLE_MCU_POWERSAVE
/* MCU Powersave is disabled */
static unsigned long idle_power_down_hook( unsigned long sleep_ms  )
{

    UNUSED_PARAMETER( sleep_ms );
    //ENABLE_INTERRUPTS;
    //__asm("wfi");
    return 0;
}
#else
static unsigned long stop_mode_power_down_hook( unsigned long sleep_ms )
{
  UNUSED_PARAMETER(sleep_ms);
  
  if (rtk_clock_needed_counter == 0 )
  {

    //DISABLE_INTERRUPTS;
    
	sleep_ex(0xFFFFFFFF, sleep_ms);
    /* as soon as interrupts are enabled, we will go and execute the interrupt handler */
    /* which triggered a wake up event */
    //ENABLE_INTERRUPTS;
    wake_up_interrupt_triggered = false;
    return 0;
  }
  else
  {
    //ENABLE_INTERRUPTS;
    __asm("wfi");
    
    /* Note: We return 0 ticks passed because system tick is still going when wfi instruction gets executed */
    return 0;
  }
}
#endif /* MICO_DISABLE_MCU_POWERSAVE */
      
