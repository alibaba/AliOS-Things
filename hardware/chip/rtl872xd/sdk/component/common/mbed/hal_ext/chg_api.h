/*******************************************************************************
* Copyright (c) 2014, Realtek Semiconductor Corp.
* All rights reserved.
*
* This module is a confidential and proprietary property of RealTek and
* possession or use of this module requires written permission of RealTek.
*******************************************************************************/
#ifndef MBED_EXT_CHG_API_EXT_H
#define MBED_EXT_CHG_API_EXT_H
#if (defined(CONFIG_PLATFORM_8195BHP) && (CONFIG_PLATFORM_8195BHP == 1))  ||  (defined(CONFIG_PLATFORM_8195BLP) && (CONFIG_PLATFORM_8195BLP == 1))
///@name AmebaPro Only 
///@{
#include "device.h"

#if defined (CONFIG_PLATFORM_8195BLP)
/** @addtogroup chg CHG
 *  @ingroup    hal
 *  @brief      chg functions
 *  @{
 */

///@name Ameba Common
///@{

typedef struct chg_s chg_t;
/**
  * @brief  Initializes the CHG registers with default parameters and run CHG state machine.
  * @param  obj: CHG object define in application software.
  * @param  cc: The Constant Current value, Uint:mA.
  * @param  cv: The Constant Voltage value, Uint:mV.
  * @retval none
  */
void charger_sm(chg_t *obj, u16 cc, u16 cv);

///@}

/*\@}*/

#endif
///@}
#endif  // end of "#if defined(CONFIG_PLATFORM_8195BHP) || defined(CONFIG_PLATFORM_8195BLP)"
#endif
