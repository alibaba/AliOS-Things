/**
  *****************************************************************************
  * @file:			timer.h
  * @author			Ingrid Chen
  * @version		V1.0.1
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

/**
* @addtogroup Çý¶¯
* @{
* @defgroup Timer Timer
* @{
*/

#ifndef __TIMER_H__
#define __TIMER_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "type.h"

/**
 * @brief	Config Timer0 and start count.
 * @param		Time range is 1~174000
 * @return	None
 * @note@	unit is uS
 */
void Timer0Set(uint32_t Time);

/**
 * @brief	Config Timer1 and start count.
 * @param		Time range is 1~174000
 * @return	None
 * @note@	unit is uS
 */
void Timer1Set(uint32_t Time);

/**
 * @brief	Clear Timer0 Interrupt Flag.
 * @param		None
 * @return	None
 */
void Timer0IntClr(void);

/**
 * @brief	Clear Timer1 Interrupt Flag.
 * @param		None
 * @return	None
 */
void Timer1IntClr(void);

/**
 * @brief	Enalbe or Disable Timer0 Halt function in debug mode.
 * @param		HaltEnable
 * @return	None
 * @note@	When enable this function, Timer0 will be halted at breakpoint.
 */
void Timer0HaltModeSet(bool HaltEnable);

/**
 * @brief	Enalbe or Disable Timer1 Halt function in debug mode.
 * @param		HaltEnable
 * @return	None
 * @note@	When enable this function, Timer1 will be halted at breakpoint.
 */
void Timer1HaltModeSet(bool HaltEnable);

/**
 * @brief	Get Timer0 Interrupt Flag.
 * @param	None
 * @return	Nonzero: timer count has not decreased to 0, zero: timer count has decreased to 0 and int flag has be set.
*/
uint32_t Timer0GetTime(void);

/**
 * @brief	Get Timer1 Interrupt Flag.
 * @param		None
 * @return	Nonzero: timer count has not decreased to 0, zero: timer count has decreased to 0 and int flag has be set.
 */
uint32_t Timer1GetTime(void);

/**
 * @brief	Close Timer0
 * @param		None
 * @return	None
 */
void Timer0Close(void);

/**
 * @brief	Close Timer1
 * @param		None
 * @return	None
 */
void Timer1Close(void);

/**
 * @brief	Adjust Timer0 after System Frequency has changed
 * @param	PreFq System Frequency before changed
 * @param	CurFq System Frequency after changed
 * @return	None
 */
void Timer0Adjust(uint32_t PreFq, uint32_t CurFq);

/**
 * @brief	Adjust Timer1 after System Frequency has changed
 * @param	PreFq System Frequency before changed
 * @param	CurFq System Frequency after changed
 * @return None
 */
void Timer1Adjust(uint32_t PreFq, uint32_t CurFq);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif

/**
 * @}
 * @}
 */
