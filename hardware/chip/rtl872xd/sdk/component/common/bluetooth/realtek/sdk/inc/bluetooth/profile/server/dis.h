/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     dis.h
  * @brief    Head file for using device information service.
  * @details  DIS data structs and external functions declaration.
  * @author
  * @date
  * @version  v1.0
  * *************************************************************************************
  */

/* Define to prevent recursive inclusion */
#ifndef _DEVICE_INFO_SERVICE_H_
#define _DEVICE_INFO_SERVICE_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

/* Add Includes here */
#include "profile_server.h"


/**
 * @defgroup DIS Device Information Service
 * @brief Device information service
 * @details
    The Device Information Service exposes manufacturer and/or vendor information about a device.
    This module implements the Device Information Service.
    The Device Information Service may expose one or more of the characteristics. It can be
    configured in file @ref dis_config.h.

    Application shall register device information service when initialization.

 * Sample code:
 * \code{.c}
    void app_le_profile_init(void)
    {
        dis_service_id = dis_add_service(app_profile_callback);
    }
 * \endcode

    When characteristic value was read, write or cccd updated by remote device, @ref app_profile_callback will be
    called.
    Application shall set dis parameter value by call @ref dis_set_parameter.

 * Sample code:
 * \code{.c}
    T_APP_RESULT app_profile_callback(T_SERVER_ID service_id, void *p_data)
    {
    ...
        if (dis_service_id == service_id)
        {
            T_DIS_CALLBACK_DATA *p_dis_cb_data = (T_DIS_CALLBACK_DATA *)p_data;
            switch (p_dis_cb_data->msg_type)
            {
            case SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE:
                {
                    if (p_dis_cb_data->msg_data.read_value_index == DIS_READ_MANU_NAME_INDEX)
                    {
                        const uint8_t dis_manufacture_name[] = "Realtek BT";
                        dis_set_parameter(DIS_PARAM_MANUFACTURER_NAME,
                                          sizeof(dis_manufacture_name),
                                          (void *)dis_manufacture_name);
                    }
                    else if (p_dis_cb_data->msg_data.read_value_index == DIS_READ_MODEL_NUM_INDEX)
                    {
                        const uint8_t dis_model_number[] = "Model Num 1.0";
                        dis_set_parameter(DIS_PARAM_MODEL_NUMBER,
                                          sizeof(dis_model_number),
                                          (void *)dis_model_number);
                    }
                    ...
                    ...

                }

                break;
            default:
                break;
            }
        }
    }
 * \endcode

 * @{
 */

/*============================================================================*
 *                         Macros
 *============================================================================*/
/** @defgroup DIS_Exported_Macros DIS Exported Macros
  * @brief
  * @{
  */

/** @defgroup DIS_READ_INFO_INDEX DIS Read Informatio Index
  * @brief  Read characteristic value index.
  * @{
  */
#define DIS_READ_MANU_NAME_INDEX        1
#define DIS_READ_MODEL_NUM_INDEX        2
#define DIS_READ_SERIAL_NUM_INDEX       3
#define DIS_READ_HARDWARE_REV_INDEX     4
#define DIS_READ_FIRMWARE_REV_INDEX     5
#define DIS_READ_SOFTWARE_REV_INDEX     6
#define DIS_READ_SYSTEM_ID_INDEX        7
#define DIS_READ_IEEE_CERT_STR_INDEX    8
#define DIS_READ_PNP_ID_INDEX           9
/** @} */

#define DIS_SYSTEM_ID_LENGTH                8
#define DIS_PNP_ID_LENGTH                   7

/** @} End of DIS_Exported_Macros */
/*============================================================================*
 *                         Types
 *============================================================================*/
/** @defgroup DIS_Exported_Types DIS Exported Types
  * @brief
  * @{
  */
/** @defgroup DIS_PARAM_TYPE DIS parameter type
* @brief  Type of parameters set/got from application.
* @{
*/
typedef enum
{
    DIS_PARAM_MANUFACTURER_NAME,
    DIS_PARAM_MODEL_NUMBER,
    DIS_PARAM_SERIAL_NUMBER,
    DIS_PARAM_HARDWARE_REVISION,
    DIS_PARAM_FIRMWARE_REVISION,
    DIS_PARAM_SOFTWARE_REVISION,
    DIS_PARAM_SYSTEM_ID,
    DIS_PARAM_IEEE_DATA_LIST,
    DIS_PARAM_PNP_ID
} T_DIS_PARAM_TYPE;
/** @} */

/** DIS upstream message data*/
typedef union
{
    uint8_t read_value_index;
} T_DIS_UPSTREAM_MSG_DATA;

/** DIS callback data*/
typedef struct
{
    T_SERVICE_CALLBACK_TYPE     msg_type;
    uint8_t                    conn_id;
    T_DIS_UPSTREAM_MSG_DATA     msg_data;
} T_DIS_CALLBACK_DATA;

/** @} End of DIS_Exported_Types */

/*============================================================================*
 *                         Functions
 *============================================================================*/
/** @defgroup DIS_Exported_Functions DIS Exported Functions
  * @brief
  * @{
  */

/**
 * @brief       Set a device information service parameter.
 *
 *              NOTE: You can call this function with a device information service parameter type and it will set the
 *                      device information service parameter.  Device information service parameters are defined in @ref T_DIS_PARAM_TYPE.
 *                      If the "len" field sets to the size of a "uint16_t" ,the
 *                      "p_value" field must point to a data with type of "uint16_t".
 *
 * @param[in]   param_type   Device information service parameter type: @ref T_DIS_PARAM_TYPE
 * @param[in]   length       Length of data to write
 * @param[in]   p_value Pointer to data to write.  This is dependent on
 *                      the parameter type and WILL be cast to the appropriate
 *                      data type (For example: if data type of param is uint16_t, p_value will be cast to
 *                      pointer of uint16_t).
 *
 * @return Operation result.
 * @retval true Operation success.
 * @retval false Operation failure.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test(void)
    {
        const uint8_t dis_manufacture_name[] = "Realtek BT";
        dis_set_parameter(DIS_PARAM_MANUFACTURER_NAME,
                      sizeof(dis_manufacture_name),
                      (void *)dis_manufacture_name);
    }
 * \endcode
 */
bool dis_set_parameter(T_DIS_PARAM_TYPE param_type, uint8_t length, void *p_value);

/**
  * @brief Add device information service to the BLE stack database.
  *
  * @param[in]   p_func  Callback when service attribute was read, write or cccd update.
  * @return Service id generated by the BLE stack: @ref T_SERVER_ID.
  * @retval 0xFF Operation failure.
  * @retval others Service id assigned by stack.
  *
  * <b>Example usage</b>
  * \code{.c}
    void profile_init()
    {
        server_init(1);
        dis_id = dis_add_service(app_handle_profile_message);
    }
  * \endcode
  */

T_SERVER_ID dis_add_service(void *p_func);

/** @} End of DIS_Exported_Functions */

/** @} End of DIS */


#ifdef  __cplusplus
}
#endif      /* __cplusplus */

#endif /* _DEVICE_INFO_SERVICE_H_ */
