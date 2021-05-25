/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     hids_rmc.h
  * @brief    Head file for using Human Interface Device Service Remote Controller.
  * @details  HIDS RMC data structs and external functions declaration.
  * @author   Chenjie Jin
  * @date     2018-5-7
  * @version  v1.1
  * *************************************************************************************
  */

/* Define to prevent recursive inclusion */
#ifndef _HIDS_RMC_H
#define _HIDS_RMC_H

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

/* Add Includes here */
#include "profile_server.h"

/** @defgroup HIDS_RmC HIDS Human Interface Device Service of Remote Controller
  * @brief Human Interface Device Service Remote Controller
  * @{
  */

/** @defgroup HIDS_RmC_Exported_Constants HIDS RmC Exported Constants
  * @brief macros that other .c files may use all defined here
  * @{
  */

/** @brief  HIDS Report Id Definition. */
#define HIDS_KB_REPORT_ID                      1
#if FEATURE_SUPPORT_MULTIMEDIA_KEYBOARD
#define HIDS_MM_KB_REPORT_ID                   2
#endif
#if (VOICE_ENC_TYPE == SW_MSBC_ENC)
#define HIDS_VOICE_REPORT_ID                0x5A // input and output
#elif (VOICE_ENC_TYPE == SW_SBC_ENC)
#define HIDS_VOICE_REPORT_ID                0x5B // input and output
#elif (VOICE_ENC_TYPE == SW_OPT_ADPCM_ENC)
#define HIDS_VOICE_REPORT_ID                0x5C // input and output
#else
#define HIDS_VOICE_REPORT_ID                0x5A // input and output
#endif


/** @defgroup HIDS_RmC_Application_Parameters HIDS RmC Application Parameters
  * @brief  Type of parameters set/got from application.
  * @{
  */
#define HIDS_PARAM_PROTOCOL_MODE      1
#define HIDS_PARAM_SUSPEND_MODE       2
#define HIDS_PARAM_HID_INFO           3
#define HIDS_PARAM_REPORT             4
/** @} */

///@cond
/** @brief  HIDS_RmC related UUIDs. */
#define GATT_UUID_HID                          0x1812
#define GATT_UUID_CHAR_HID_CONTROL_POINT       0x2A4C
#define GATT_UUID_CHAR_HID_INFO                0x2A4A
#define GATT_UUID_CHAR_REPORT                  0x2A4D
#define GATT_UUID_CHAR_REPORT_MAP              0x2A4B
#define GATT_UUID_CHAR_RECORD_ACCESS_POINT     0x2A52
#define GATT_UUID_CHAR_PROTOCOL_MODE           0x2A4E
#define GATT_UUID_CHAR_BOOT_KB_IN_REPORT       0x2A22
#define GATT_UUID_CHAR_BOOT_KB_OUT_REPORT      0x2A32
#define GATT_UUID_CHAR_BOOT_MOUSE_IN_REPORT    0x2A33
#define GATT_UUID_CHAR_REPORT_REFERENCE        0x2908
#define GATT_UUID_CHAR_EXTERNAL_REPORT_REFERENCE  0x2907

///@endcond

/** @defgroup HIDS_RmC_Attribute_Index HIDS RmC Attribute Index
  * @brief  Index defines for Characteristic's value in HID Device
  * @{
  */
#define GATT_SRV_HID_INDEX_MAX                    23
#define GATT_SVC_HID_INFO_INDEX                   (2) /**<  @brief  Index for HID Information chars's value      */
#define GATT_SVC_HID_CONTROL_POINT_INDEX          (4) /**<  @brief  Index for HID Control Point chars's value      */
#define GATT_SVC_HID_PROTOCOL_MODE_INDEX          (6) /**<  @brief  Index for HID Protocol Mode chars's value      */
#define GATT_SVC_HID_REPORT_MAP_INDEX             (8) /**<  @brief  Index for HID Report Map chars's value      */

#define GATT_SRV_HID_KB_INPUT_INDEX               (10)
#define GATT_SRV_HID_KB_CCCD_INDEX                (GATT_SRV_HID_KB_INPUT_INDEX + 1)

#if RCU_VOICE_EN
#define GATT_SRV_HID_VOICE_INPUT_INDEX            (14)
#define GATT_SRV_HID_VOICE_INPUT_CCCD_INDEX       (GATT_SRV_HID_VOICE_INPUT_INDEX + 1)
#define GATT_SRV_HID_VOICE_OUTPUT_INDEX           (18)
#endif

#if FEATURE_SUPPORT_MULTIMEDIA_KEYBOARD
#if RCU_VOICE_EN
#define GATT_SRV_HID_MM_KB_INPUT_INDEX         (21)
#define GATT_SRV_HID_MM_KB_INPUT_CCCD_INDEX    (GATT_SRV_HID_MM_KB_INPUT_INDEX + 1)
#else
#define GATT_SRV_HID_MM_KB_INPUT_INDEX         (14)
#define GATT_SRV_HID_MM_KB_INPUT_CCCD_INDEX    (GATT_SRV_HID_MM_KB_INPUT_INDEX + 1)
#endif
#endif
/** @} */

/** @defgroup HIDS_RmC_Upstream_Message HIDS RmC Upstream Message
  * @brief  Upstream message used to inform application.
  * @{
  */

/** @defgroup HIDS_RmC_Read_Info HIDS RmC Read Info
  * @brief  Parameter for read characteristic value.
  * @{
  */
#define HIDS_READ_PARAM_PROTOCOL_MODE                       1
#define HIDS_READ_PARAM_SUSPEND_MODE                        2
#define HIDS_READ_PARAM_HID_INFO                            3
#define HIDS_READ_PARAM_REPORT                              4
/** @} */

/** @defgroup HIDS_RmC_Write_Info HIDS RmC Write Info
  * @brief  Parameter for write characteristic value.
  * @{
  */
#define HID_WRITE_VIOCE_CMD                                 1
#define HID_WRITE_PROTOCOL_MODE                             2
#define HID_WRITE_SUSPEND_MODE                              3
#define HID_WRITE_OUTPUT                                    4
#define HID_WRITE_INPUT_REPORT                              5
/** @} */

/** @defgroup HIDS_RmC_Notify_Indicate_Info HIDS RmC Notify Indicate Info
  * @brief  Parameter for enable or disable notification or indication.
  * @{
  */
#define HID_NOTIFY_INDICATE_KB_ENABLE                       1
#define HID_NOTIFY_INDICATE_KB_DISABLE                      2
#define HID_NOTIFY_INDICATE_VOICE_ENABLE                    3
#define HID_NOTIFY_INDICATE_VOICE_DISABLE                   4
#define HID_NOTIFY_INDICATE_MM_KB_ENABLE                    5
#define HID_NOTIFY_INDICATE_MM_KB_DISABLE                   6

/** @} */

/** @} End of HIDS_RmC_Upstream_Message */

/** @} End of HIDS_RmC_Exported_Constants */

/** @defgroup HIDS_RmC_Exported_Types HIDS RmC Exported Types
  * @brief  types that other.c files may use all defined here
  * @{
  */

/* Add all public types here */

/** @struct _HID_INFO_ATTRB hids_rmc.h "hids_rmc.h"
  * @brief type define for value of HID information
  */
typedef struct _HID_INFO_ATTRB
{
    uint8_t  CountryCode;
    uint8_t  bFlags;
    uint16_t BcdHID;
} HID_INFO_ATTRB;

/** @brief  enum for TProfile_HIDReportType*/
typedef enum
{
    HID_INPUT_TYPE   = 1, /**< @enum Input report */
    HID_OUTPUT_TYPE  = 2, /**< @enum Output report */
    HID_FEATURE_TYPE = 3 /**< @enum Feature report */
} TProfile_HIDReportType;

/** @struct _TProfile_REPORT_REFE_TABLE hids_rmc.h "hids_rmc.h"
  * @brief type define for info reference to report data
  */
typedef struct _TProfile_REPORT_REFE_TABLE
{
    uint8_t report_size;  /**<  @brief  report size according to value of Report Map value, byte */
    uint8_t report_id;    /**<  @brief  report ID defined in value of Report Map Char, byte */
    uint8_t report_type;  /**<  @brief  report Type(Input,Output,Feature), ref: @ref TProfile_HIDReportType */
} TProfile_REPORT_REFE_TABLE;

/** @defgroup HIDS_RmC_Callback_Data HIDS RmC Callback Data
  * @brief HIDS RmC data struct for notification data to application.
  * @{
  */

/** @brief value for HID protocol mode characteristic */
enum TProfile_HID_PROTOCOL_MODE
{
    BOOT_PROTOCOL_MODE = 0, /**<  @enum  boot protocol mode */
    REPORT_PROCOCOL_MODE = 1 /**<  @enum report protocol mode */
};

typedef union _THID_WRITE_PARAMETER
{
    uint8_t voice_enable;
    uint8_t protocol_mode; /**< ref: @ref TProfile_HID_PROTOCOL_MODE */
    uint8_t suspend_mode;
    struct
    {
        uint8_t reportLen;
        uint8_t *report;
    } report_data;
} THID_WRITE_PARAMETER;

/** @struct _THID_WRITE_MSG hids_rmc.h "hids_rmc.h"
  * @brief write message
  */
typedef struct _THID_WRITE_MSG
{
    uint8_t write_type; /**< ref: @ref HIDS_RmC_Write_Info */
    THID_WRITE_PARAMETER write_parameter;
} THID_WRITE_MSG;

typedef union _THID_UPSTREAM_MSG_DATA
{
    uint8_t notification_indification_index; /**< ref: @ref HIDS_RmC_Notify_Indicate_Info */
    uint8_t read_value_index; /**< ref: @ref HIDS_RmC_Read_Info */
    THID_WRITE_MSG write;
} THID_UPSTREAM_MSG_DATA;

/** @struct _T_HID_CALLBACK_DATA hids_rmc.h "hids_rmc.h"
  * @brief callback data
  */
typedef struct _T_HID_CALLBACK_DATA
{
    T_SERVICE_CALLBACK_TYPE     msg_type;
    THID_UPSTREAM_MSG_DATA    msg_data;
} T_HID_CALLBACK_DATA;
/** @} End of HIDS_RmC_Callback_Data */

/** @} End of HIDS_RmC_Exported_Types */

/** @defgroup HIDS_RmC_Exported_Functions HIDS RmC Exported Functions
  * @brief functions that other .c files may use all defined here
  * @{
  */
bool hids_set_parameter(uint8_t param_type, uint8_t length, void *value_ptr);
uint8_t hids_add_service(void *p_func);
/** @} End of HIDS_RmC_Exported_Functions */

/** @} End of HIDS_RmC */


#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif /* _HIDS_RMC_H */
