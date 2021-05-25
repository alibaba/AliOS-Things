/**
*****************************************************************************************
*               Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
************************************************************************************************************
  * @file    platform_utils.h
  * @brief   utility helper function for user application
  * @author  lory_xu
  * @date    2017-02
  * @version v1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

#ifndef _PLATFORM_UTILS_H_
#define _PLATFORM_UTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                               Header Files
*============================================================================*/
#include <stdint.h>
#include <stdbool.h>
/** @defgroup  PLATFORM_UTILS Platform Utilities
    * @brief Utility helper functions
    * @{
    */


/*============================================================================*
 *                              Functions
 *============================================================================*/
/** @defgroup PLATFORM_UTILS_Exported_Functions Platform Utilities Exported Functions
    * @brief
    * @{
    */

/**
 * @brief Generate random number given max number allowed
 * @param max   to specify max number that allowed
 * @return random number
 */

extern uint32_t platform_random(uint32_t max);

/** @} */ /* End of group PLATFORM_UTILS_Exported_Functions */

/** @} */ /* End of group PLATFORM_UTILS */

#ifdef __cplusplus
}
#endif

#endif

