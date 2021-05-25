/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
   * @file      link_mgr.c
   * @brief     Multilink manager functions.
   * @author    jane
   * @date      2017-06-06
   * @version   v1.0
   **************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
   **************************************************************************************
  */
/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include <ble_central_link_mgr.h>
#include <trace_app.h>
#include <string.h>

/*============================================================================*
 *                              Variables
 *============================================================================*/
/** @addtogroup  CENTRAL_CLIENT_GAP_MSG
    * @{
    */
T_APP_LINK ble_central_app_link_table[BLE_CENTRAL_APP_MAX_LINKS];

/** @} */
