/******************************************************/
/**
* @file hidef.h
* Machine/compiler dependent declarations.
*/
/*----------------------------------------------------
   Copyright (c) Freescale DevTech
               All rights reserved
                  Do not modify!
 *****************************************************/

#ifndef _H_HIDEF_
#define _H_HIDEF_

#ifdef __cplusplus
	extern "C" {
#endif

#if defined(__CWCC__)
	#pragma gcc_extensions on
#endif
	
#if (defined(__CWCC__)||defined(__GNUC__))	
	#include <stddef.h>

	#if (defined _MK_xxx_H_)
    #ifdef MCU_MKL25Z4
        #define __RESET_WATCHDOG()  (void)(RCM_SRS0 |= RCM_SRS0_WDOG_MASK)
    #else
		#define __RESET_WATCHDOG()  (void)(WDOG_REFRESH = 0xA602, WDOG_REFRESH = 0xB480)
		#define __UNLOCK_WATCHDOG() (void)(WDOG_UNLOCK = 0xC520, WDOG_UNLOCK = 0xD928)
    #endif
		
		/*!< Macro to enable all interrupts. */
		#define EnableInterrupts  asm ("CPSIE i");
		/*!< Macro to disable all interrupts. */
		#define DisableInterrupts asm ("CPSID i");
	#else
		/**** Version for ColFire V1 */	
		/*!< Macro to enable all interrupts. */
		#define EnableInterrupts asm { move.w SR,D0; andi.l #0xF8FF,D0; move.w D0,SR;  }
		/*!< Macro to disable all interrupts. */
		#define DisableInterrupts asm { move.w SR,D0; ori.l #0x0700,D0; move.w D0,SR;  }
		
	#endif
#elif defined(__IAR_SYSTEMS_ICC__)
	#include <intrinsics.h>
    #ifdef MCU_MKL25Z4
        #define __RESET_WATCHDOG()  (void)(RCM_SRS0 |= RCM_SRS0_WDOG_MASK)
    #else
	#define __RESET_WATCHDOG()  (void)(WDOG_REFRESH = 0xA602, WDOG_REFRESH = 0xB480)
	#define __UNLOCK_WATCHDOG() (void)(WDOG_UNLOCK = 0xC520, WDOG_UNLOCK = 0xD928)
    #endif
	/*!< Macro to enable all interrupts. */
	#define EnableInterrupts  __enable_interrupt();
	/*!< Macro to disable all interrupts. */
	#define DisableInterrupts __disable_interrupt();
#elif defined(__CC_ARM)
	#include <stddef.h>
	
	#define __RESET_WATCHDOG()  (void)(WDOG_REFRESH = 0xA602, WDOG_REFRESH = 0xB480)
	#define __UNLOCK_WATCHDOG() (void)(WDOG_UNLOCK = 0xC520, WDOG_UNLOCK = 0xD928)
	
	/*!< Macro to enable all interrupts. */
	#define EnableInterrupts  __enable_irq();
	/*!< Macro to disable all interrupts. */
	#define DisableInterrupts __disable_irq();
#endif
	
#ifdef __cplusplus
	}
#endif

#endif

/*****************************************************/
/* end hidef.h */
