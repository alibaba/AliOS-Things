/**
*****************************************************************************************
*     Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    bt_config_config.h
  * @brief   This file includes common constants or types for BT Config Server.
  *          And some optional feature may be defined in this file.
  * @details
  * @author  
  * @date    
  * @version 
  * *************************************************************************************
  */

/* Define to prevent recursive inclusion **/
#ifndef _BT_CONFIG_CONFIG_H_
#define _BT_CONFIG_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @defgroup BT_Config_Service BT Config Service
  * @brief BT Config Service
  * @{
  */

/** @defgroup BT_Config_Service_CONFIG BT Config Service Config
  * @brief BT Config Service configuration file
  * @{
  */
/*============================================================================*
 *                         Macros
 *============================================================================*/
/** @defgroup BT_Config_Common_Exported_Macros BT Config Service Config Exported Constants
  * @brief
  * @{
  */

/** @defgroup BT_Config_UUIDs BT Config UUIDs
  * @brief BT_Config Profile UUID definitions
  * @{
  */
#define GATT_UUID_BT_CONFIG_PROFILE					0xFF01
#define GATT_UUID_CHAR_BTCONFIG_READ_WRITE			0x2A0D
/** @} End of BT_Config_UUIDs */

/** @} End of BT_Config_Common_Exported_Macros */

/** @} End of BT_Config_Service_CONFIG */

/** @} End of BT_Config_Service */


#ifdef __cplusplus
}
#endif

#endif
