/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     hids_kb.h
  * @brief    Head file for using Human Interface Device Service.
  * @details  HIDS data structs and external functions declaration.
  * @author   Jeff_Zheng
  * @date     2017-12-01
  * @version  v1.0
  * *************************************************************************************
  */

#ifndef _HIDS_KB_H_
#define _HIDS_KB_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include "profile_server.h"

/** @defgroup HIDS_KB Human Interface Device Service of Keyboard
  * @brief  Human Interface Device Service
   * @details

    The HID Service exposes data and associated formatting for HID Devices and HID Hosts.

    Application shall register HID service when initialization through @ref hids_add_service function.

    Application can set the HID service through @ref hids_set_parameter function.

    Application can send report data of HID service to the client with a notification through @ref hids_send_report function.

  * @{
  */


/*============================================================================*
 *                         Macros
 *============================================================================*/
/** @defgroup HIDS_KB_Exported_Macros HIDS Keyboard Exported Macros
  * @brief
  * @{
  */

/** @defgroup HIDS_KB_Index HIDS Keyboard Attribute Index
  * @brief Index defines for Characteristic's value in HID Device
  * @{
  */
#define GATT_SVC_HID_PROTOCOL_MODE_INDEX            (2)
#define GATT_SVC_HID_REPORT_INPUT_INDEX             (4)
#define GATT_SVC_HID_REPORT_INPUT_CCCD_INDEX        (GATT_SVC_HID_REPORT_INPUT_INDEX+1)
#define GATT_SVC_HID_REPORT_OUTPUT_INDEX            (8)
#define GATT_SVC_HID_REPORT_FEATURE_INDEX           (11)
#define GATT_SVC_HID_REPORT_MAP_INDEX               (14)
#define GATT_SVC_HID_EXTERNAL_REPORT_REFER_INDEX    (GATT_SVC_HID_REPORT_MAP_INDEX+1)
#define GATT_SVC_HID_BOOT_KB_IN_REPORT_INDEX        (17)
#define GATT_SVC_HID_BOOT_KB_IN_REPORT_CCCD_INDEX   (GATT_SVC_HID_BOOT_KB_IN_REPORT_INDEX+1)
#define GATT_SVC_HID_BOOT_KB_OUT_REPORT_INDEX       (20)
#define GATT_SVC_HID_INFO_INDEX                     (22)
#define GATT_SVC_HID_CONTROL_POINT_INDEX            (24)
/** @} */

/** End of HIDS_KB_Exported_Macros
* @}
*/


/*============================================================================*
 *                         Types
 *============================================================================*/
/** @defgroup HIDS_KB_Exported_Types HIDS Keyboard Exported Types
  * @brief
  * @{
  */

/**
*  @brief Human Interface Device Service  parameter type
*/
typedef enum
{
    HID_PROTOCOL_MODE = 0x01,
    HID_REPORT_INPUT,
    HID_REPORT_OUTPUT,
    HID_REPORT_FEATURE,
    HID_REPORT_MAP,
    HID_EXTERNAL_REPORT_REFER,
    HID_BOOT_KB_IN_REPORT,
    HID_BOOT_KB_OUT_REPORT,
    HID_INFO,
    HID_CONTROL_POINT,
} T_HIDS_PARAM_TYPE;

/**
*  @brief Human Interface Device Service information
*/
typedef struct
{
    uint8_t  b_country_code;
    uint8_t  flags;
    uint16_t bcd_hid;
} T_HID_INFO;

/**
*  @brief Human Interface Device Service report type
*/
typedef enum
{
    HID_INPUT_TYPE   = 1,
    HID_OUTPUT_TYPE  = 2,
    HID_FEATURE_TYPE = 3
} T_PROFILE_HID_REPORT_TYPE;

/**
*  @brief Human Interface Device Service control point
*/
typedef enum
{
    HID_SUSPEND         = 0,
    HID_EXIT_SUSPEND    = 1,
} T_HID_CTL_POINT;

/**
*  @brief Human Interface Device Service protocol mode
*/
typedef enum
{
    BOOT_PROTOCOL_MODE = 0,
    REPORT_PROCOCOL_MODE = 1
} T_HID_PROTOCOL_MODE;

/** @defgroup HIDS_KB_Upstream_Message HIDS Keyboard Upstream Message
  * @brief  Upstream message used to inform application.
  * @{
  */
typedef enum
{
    NOTIFY_ENABLE,
    NOTIFY_DISABLE
} T_HID_NOTIFY;

typedef union
{
    uint8_t voice_enable;
    uint8_t protocol_mode;
    uint8_t suspend_mode;
    uint8_t output;
    struct
    {
        uint8_t reportLen;
        uint8_t *report;
    } report_data;
} T_HID_WRITE_PARAMETER;

typedef struct
{
    uint8_t write_type;
    T_HID_WRITE_PARAMETER write_parameter;
} T_HID_WRITE_MSG;

typedef struct
{
    uint8_t index;
    T_HID_NOTIFY value;
} T_HID_NOT_IND_DATA;

typedef struct
{
    uint8_t read_value_index;
    T_HID_WRITE_MSG write_msg;
    T_HID_NOT_IND_DATA not_ind_data;
} T_HID_UPSTREAM_MSG_DATA;

typedef struct
{
    uint8_t                     conn_id;
    T_SERVICE_CALLBACK_TYPE     msg_type;
    T_HID_UPSTREAM_MSG_DATA     msg_data;
} T_HID_CALLBACK_DATA;

/** @} End of HIDS_KB_Upstream_Message */


/** End of HIDS_KB_Exported_Types
* @}
*/


/*============================================================================*
 *                         Functions
 *============================================================================*/
/** @defgroup HIDS_KB_Exported_Functions HIDS Keyboard Exported Functions
  * @brief
  * @{
  */

/**
  * @brief       Add HID service to the BLE stack database.
  *
  *
  * @param[in]   p_func  Callback when service attribute was read, write or cccd update.
  * @return Service id generated by the BLE stack: @ref T_SERVER_ID.
  * @retval 0xFF Operation failure.
  * @retval Others Service id assigned by stack.
  *
  * <b>Example usage</b>
  * \code{.c}
     void profile_init()
     {
         server_init(1);
         hids_id = hids_add_service(app_handle_profile_message);
     }
  * \endcode
  */
T_SERVER_ID hids_add_service(void *p_func);

/**
 * @brief       Set a HID service parameter.
 *
 *              NOTE: You can call this function with a HID service parameter type and it will set the
 *                      HID service parameter.  HID service parameters are defined in @ref T_HIDS_PARAM_TYPE.
 *
 * @param[in]   param_type  HID service parameter type: @ref T_HIDS_PARAM_TYPE
 * @param[in]   len         Length of data to write
 * @param[in]   p_value Pointer to data to write.  This is dependent on
 *                      the parameter type and WILL be cast to the appropriate
 *                      data type
 *
 * @return Operation result.
 * @retval true Operation success.
 * @retval false Operation failure.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test(void)
    {
        uint8_t mode = 1;
        hids_set_parameter(HID_PROTOCOL_MODE, 1, &mode);
    }
 * \endcode
 */
bool hids_set_parameter(T_HIDS_PARAM_TYPE param_type, uint8_t len, void *p_value);

/**
 * @brief       Send HIDS notification data .
 *
 *
 * @param[in]   conn_id  Connection id.
 * @param[in]   service_id  Service id.
 * @param[in]   index  hids characteristic index.
 * @param[in]   p_data report value pointer.
 * @param[in]   data_len length of report data.
 * @return Operation result.
 * @retval true Operation success.
 * @retval false Operation failure.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test(void)
    {
        uint8_t conn_id = 0;
        T_SERVER_ID service_id = hids_id;
        uint8_t hid_report_input[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
        hids_send_report(conn_id, service_id, GATT_SVC_HID_REPORT_INPUT_INDEX, hid_report_input, sizeof(hid_report_input));
    }
 * \endcode
 */
bool hids_send_report(uint8_t conn_id, T_SERVER_ID service_id, uint16_t index, uint8_t *p_data,
                      uint16_t data_len);


/** @} End of HIDS_KB_Exported_Functions */

/** @} End of HIDS_KB */

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
