/**
 *******************************************************************************
 * @file    watchdog.c
 * @author  Lance
 * @version V1.0.0
 * @date    27-May-2013
 * @maintainer lujiangang
 * @brief   watchdog timer driver header file
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


#ifndef __WATCH_DOG_H__
#define __WATCH_DOG_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus


#include "type.h"


typedef enum _WDG_STEP_SEL
{
    WDG_STEP_1S = 0,
    WDG_STEP_3S = 1,
    WDG_STEP_4S = 2
} WDG_STEP_SEL;


/**
 * @brief  Enable the watchdog timer.
* @param  Mode	 watchdog (timeout) interval-> WDG_STEP_1S/WDG_STEP_3S/WDG_STEP_4S.
 * @return None.
 */
void WdgEn(WDG_STEP_SEL Mode);


/**
 * @brief  Disable the watchdog timer.
 * @param  None.
 * @return None.
 */
void WdgDis(void);


/**
 * @brief  Feed the watchdog.
 * @param  None.
 * @return None.
 */
void WdgFeed(void);


#ifdef  __cplusplus
}
#endif//__cplusplus

#endif	//__WATCH_DOG_H__
