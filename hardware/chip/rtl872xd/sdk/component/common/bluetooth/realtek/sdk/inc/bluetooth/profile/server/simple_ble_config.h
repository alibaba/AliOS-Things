/**
*****************************************************************************************
*     Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    simple_ble_config.h
  * @brief   This file includes common constants or types for Simple BLE service/client.
  *          And some optional feature may be defined in this file.
  * @details
  * @author  Ethan
  * @date    2016-02-18
  * @version v0.1
  * *************************************************************************************
  */

/* Define to prevent recursive inclusion **/
#ifndef _SIMPLE_BLE_CONFIG_H_
#define _SIMPLE_BLE_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @defgroup SIMP_Service Simple Ble Service
  * @brief Simple BLE service
  * @{
  */

/** @defgroup SIMP_Service_CONFIG SIMP Service Config
  * @brief Simple BLE service configuration file
  * @{
  */
/*============================================================================*
 *                         Macros
 *============================================================================*/
/** @defgroup SIMP_Common_Exported_Macros SIMP Service Config Exported Constants
  * @brief
  * @{
  */

/** @defgroup SIMP_UUIDs SIMP UUIDs
  * @brief Simple BLE Profile UUID definitions
  * @{
  */
#define GATT_UUID_SIMPLE_PROFILE                    0xA00A
#define GATT_UUID_CHAR_SIMPLE_V1_READ               0xB001
#define GATT_UUID_CHAR_SIMPLE_V2_WRITE              0xB002
#define GATT_UUID_CHAR_SIMPLE_V3_NOTIFY             0xB003
#define GATT_UUID_CHAR_SIMPLE_V4_INDICATE           0xB004
/** @} End of SIMP_UUIDs */

/** @} End of SIMP_Common_Exported_Macros */

/** @} End of SIMP_Service_CONFIG */

/** @} End of SIMP_Service */


#ifdef __cplusplus
}
#endif

#endif
