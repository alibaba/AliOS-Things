/**
 *******************************************************************************
 * @file    sleep.h
 * @author  Lujiangang
 * @version V1.0.1
 * @date    27-May-2013
 * @brief   sleep driver header file
 *******************************************************************************
 * @attention
 *
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
* @defgroup Sleep Sleep
* @{
*/

#ifndef __SLEEP_H__
#define __SLEEP_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "type.h"

/**
 * @brief	system goto sleep mode
 * @param	None
 * @return None
 */
void Sleep(void);

/**
 * @brief	system goto Deep sleep mode
 * @param	None
 * @return None
 */
void DeepSleep(void);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif	//__SLEEP_H__

/**
 * @}
 * @}
 */
