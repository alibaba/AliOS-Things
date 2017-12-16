/**
  *****************************************************************************
  * @file:			timer.h
  * @author			Ingrid Chen
  * @version		V1.0.0
  * @data			06-June-2013
  * @Brief			Timer0 & Timer1 driver header file.
  * @maintainer        lujiangang
  ******************************************************************************
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, MVSILICON SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
  * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
  */

#ifndef __TIMER_H__
#define __TIMER_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "type.h"

/**
 * @Brief	Config Timer0 and start count.
 * @Param	Time range is 1~174000
 * @Return	None
 * @Note@	unit is uS
 */
void Timer0Set(uint32_t Time);

/**
 * @Brief	Config Timer1 and start count.
 * @Param	Time range is 1~174000
 * @Return	None
 * @Note@	unit is uS
 */
void Timer1Set(uint32_t Time);

/**
 * @Brief	Clear Timer0 Interrupt Flag.
 * @Param	None
 * @Return	None
 */
void Timer0IntClr(void);

/**
 * @Brief	Clear Timer1 Interrupt Flag.
 * @Param	None
 * @Return	None
 */
void Timer1IntClr(void);

/**
 * @Brief	Enalbe or Disable Timer0 Halt function in debug mode.
 * @Param	HaltEnable
 * @Return	None
 * @Note@	When enable this function, Timer0 will be halted at breakpoint.
 */
void Timer0HaltModeSet(bool HaltEnable);

/**
 * @Brief	Enalbe or Disable Timer1 Halt function in debug mode.
 * @Param	HaltEnable
 * @Return	None
 * @Note@	When enable this function, Timer1 will be halted at breakpoint.
 */
void Timer1HaltModeSet(bool HaltEnable);

/**
 * @Brief	Get Timer0 Interrupt Flag.
 * @Param	None
 * @Return	Nonzero: timer count has not decreased to 0, zero: timer count has decreased to 0 and int flag has be set.
*/
uint32_t Timer0GetTime(void);

/**
 * @Brief	Get Timer1 Interrupt Flag.
 * @Param	None
 * @Return	Nonzero: timer count has not decreased to 0, zero: timer count has decreased to 0 and int flag has be set.
 */
uint32_t Timer1GetTime(void);

/**
 * @Brief	Close Timer0
 * @Param	None
 * @Return	None
 */
void Timer0Close(void);

/**
 * @Brief	Close Timer1
 * @Param	None
 * @Return	None
 */
void Timer1Close(void);

/**
 * @Brief	Adjust Timer0 after System Frequency has changed
 * @Param	PreFq System Frequency before changed
 * @Param	CurFq System Frequency after changed
 * @Return	None
 */
void Timer0Adjust(uint32_t PreFq, uint32_t CurFq);

/**
 * @Brief	Adjust Timer1 after System Frequency has changed
 * @Param	PreFq System Frequency before changed
 * @Param	CurFq System Frequency after changed
 */
void Timer1Adjust(uint32_t PreFq, uint32_t CurFq);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif
