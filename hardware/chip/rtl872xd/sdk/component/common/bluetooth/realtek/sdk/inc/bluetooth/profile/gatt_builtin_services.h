/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     gatt_builtin_services.h
  * @brief    Head file for using builtin services, including GAP service and GATT service.
  * @details  GAPS data structs and external functions declaration.
  * @author   jane
  * @date     2015-5-12
  * @version  v0.1
  * *************************************************************************************
  */

/* Define to prevent recursive inclusion */
#ifndef _BUILTIN_SERVICES_H_
#define _BUILTIN_SERVICES_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

/* Add Includes here */
#include <bt_flags.h>
#include <profile_server.h>


/** @defgroup GAP_GATT_SERVICE GAP and GATT Inbox Services
  * @brief GAP and GATT inbox services
  * @{
  */

/*============================================================================*
 *                         Macros
 *============================================================================*/
/** @defgroup GAP_GATT_SERVICE_Exported_Macros GAP and GATT Service Exported Macros
  * @brief
  * @{
  */

/** @defgroup GAPS_Write_PROPERTY GAP Service Write Property
  * @brief  GAP service write property.
  * @{
  */
#define GAPS_PROPERTY_WRITE_DISABLE                   0
#define GAPS_PROPERTY_WRITE_ENABLE                    1
/** @} */


/** @defgroup GAPS_WRITE_TYPE GAP and GATT Service Write Type
  * @brief  GAP and GATT Service Write Type.
  * @{
  */
#define GAPS_WRITE_DEVICE_NAME            1
#define GAPS_WRITE_APPEARANCE             2
#define GATT_SERVICE_CHANGE_CCCD_ENABLE   3
#define GATT_SERVICE_CHANGE_CCCD_DISABLE  4
/** @} */

/** End of GAP_GATT_SERVICE_Exported_Macros
* @}
*/

/*============================================================================*
 *                              Types
 *============================================================================*/
/** @defgroup GAP_GATT_SERVICE_Exported_Types GAP and GATT Service Exported Types
  * @brief
  * @{
  */

/** @brief GAPS parameter type */
typedef enum
{
    GAPS_PARAM_DEVICE_NAME,
    GAPS_PARAM_APPEARANCE,
#if F_BT_LE_PRIVACY_SUPPORT
    GAPS_PARAM_CENTRAL_ADDRESS_RESOLUTION,
#endif
    GAPS_PARAM_DEVICE_NAME_PROPERTY,
    GAPS_PARAM_APPEARANCE_PROPERTY,
} T_GAPS_PARAM_TYPE;

/** @brief Builtin services data struct for notification data to application. */
typedef union
{
    uint8_t  opcode; //!< ref: @ref GAPS_WRITE_TYPE
    uint16_t len;
    uint8_t  *p_value;
} T_GAPS_UPSTREAM_MSG_DATA;

/** @brief Builtin services callback data to inform application */
typedef struct
{
    T_SERVICE_CALLBACK_TYPE     msg_type;
    uint8_t                     conn_id;
    T_GAPS_UPSTREAM_MSG_DATA    msg_data;
} T_GAPS_CALLBACK_DATA;

/** End of GAP_GATT_SERVICE_Exported_Types
* @}
*/
/*============================================================================*
 *                              Functions
 *============================================================================*/
/** @defgroup GAP_GATT_SERVICE_Exported_Functions GAP and GATT Service Exported Functions
  * @brief
  * @{
  */

/**
 * @brief  Register callback to builtin services.
 *
 * @param[in] p_func   Callback to notify app.
 * @return none.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test(void)
    {
        uint8_t appearance_prop = GAPS_PROPERTY_WRITE_ENABLE;
        uint8_t device_name_prop = GAPS_PROPERTY_WRITE_ENABLE;
        gaps_set_parameter(GAPS_PARAM_APPEARANCE_PROPERTY, sizeof(appearance_prop), &appearance_prop);
        gaps_set_parameter(GAPS_PARAM_DEVICE_NAME_PROPERTY, sizeof(device_name_prop), &device_name_prop);
        gatt_register_callback(gap_service_callback);
    }
    T_APP_RESULT gap_service_callback(T_SERVER_ID service_id, void *p_para)
    {
        T_APP_RESULT  result = APP_RESULT_SUCCESS;
        T_GAPS_CALLBACK_DATA *p_gap_data = (T_GAPS_CALLBACK_DATA *)p_para;
        APP_PRINT_INFO2("gap_service_callback conn_id = %d msg_type = %d\n", p_gap_data->conn_id,
                        p_gap_data->msg_type);
        if (p_gap_data->msg_type == SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE)
        {
            switch (p_gap_data->msg_data.opcode)
            {
            case GAPS_WRITE_DEVICE_NAME:
                {
                    T_LOCAL_NAME device_name;
                    memcpy(device_name.local_name, p_gap_data->msg_data.p_value, p_gap_data->msg_data.len);
                    device_name.local_name[p_gap_data->msg_data.len] = 0;
                    flash_save_local_name(&device_name);
                }
                break;

            case GAPS_WRITE_APPEARANCE:
                {
                    uint16_t appearance_val;
                    T_LOCAL_APPEARANCE appearance;

                    LE_ARRAY_TO_UINT16(appearance_val, p_gap_data->msg_data.p_value);
                    appearance.local_appearance = appearance_val;
                    flash_save_local_appearance(&appearance);
                }
                break;

            default:
                break;
            }
        }
        else if (p_gap_data->msg_type == SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION)
        {
            if (p_gap_data->msg_data.opcode == GATT_SERVICE_CHANGE_CCCD_ENABLE)
            {
                APP_PRINT_INFO0("GATT_SERVICE_CHANGE_CCCD_ENABLE");
            }
        }
        return result;
    }
 * \endcode
 */
void gatt_register_callback(void *p_func);

/**
 * @brief  Set GAP service parameter.
 *
 * @param[in] param_type   parameter type to set: @ref T_GAPS_PARAM_TYPE
 * @param[in] length       value length to be set.
 * @param[in] p_value      value to set.
 * @return parameter set result.
 * @retval 0 false
 * @retval 1 true
 *
 * <b>Example usage</b>
 * \code{.c}
    void test(void)
    {
        uint8_t appearance_prop = GAPS_PROPERTY_WRITE_ENABLE;
        uint8_t device_name_prop = GAPS_PROPERTY_WRITE_ENABLE;
        gaps_set_parameter(GAPS_PARAM_APPEARANCE_PROPERTY, sizeof(appearance_prop), &appearance_prop);
        gaps_set_parameter(GAPS_PARAM_DEVICE_NAME_PROPERTY, sizeof(device_name_prop), &device_name_prop);
        gatt_register_callback(gap_service_callback);
    }
 * \endcode
 */
bool gaps_set_parameter(T_GAPS_PARAM_TYPE param_type, uint8_t length, void *p_value);


/**
  * @brief  Set the preferred connection parameter.
  *
  * @param[in] conn_interval_min   Defines minimum value for the connection interval in the
                                    following manner:
                                    connIntervalmin = Conn_Interval_Min * 1.25 ms
                                    Conn_Interval_Min range: 0x0006 to 0x0C80
                                    Value of 0xFFFF indicates no specific minimum.
                                    Values outside the range (except 0xFFFF) are reserved for
                                    future use.
  * @param[in] conn_interval_max   Defines maximum value for the connection interval in the
                                    following manner:
                                    connIntervalmax = Conn_Interval_Max * 1.25 ms
                                    Conn_Interval_Max range: 0x0006 to 0x0C80
                                    Shall be equal to or greater than the Conn_Interval_Min.
                                    Value of 0xFFFF indicates no specific maximum.
                                    Values outside the range (except 0xFFFF) are reserved for
                                    future use.
  * @param[in] slave_latency        Defines the slave latency for the connection in number of
                                    connection events.
                                    Slave latency range: 0x0000 to 0x01F3
                                    Values outside the range are reserved for future use.
  * @param[in] supervision_timeout  Defines the connection supervisor timeout multiplier as amultiple of 10ms.
                                    Range: 0xFFFF indicates no specific value requested.
                                    Range: 0x000A to 0x0C80
                                    Time = N * 10 ms
                                    Time Range: 100 ms to 32 seconds
                                    Values outside the range (except 0xFFFF) are reserved for
                                    future use.
  * @return none
  */
void gaps_set_peripheral_preferred_conn_param(uint16_t conn_interval_min,
                                              uint16_t conn_interval_max,
                                              uint16_t slave_latency,
                                              uint16_t supervision_timeout);

/**
 * @brief  Send service changed indication.
 *
 * @param[in] conn_id      Connection id
 * @param[in] start_handle Start of Affected Attribute Handle Range
 * @param[in] end_handle   End of Affected Attribute Handle Range
 * @return parameter set result.
 * @retval 0 false
 * @retval 1 true
 *
 * <b>Example usage</b>
 * \code{.c}
    void test(void)
    {
        uint8_t  conn_id = 0;
        uint16_t start_handle = 0x05;
        uint16_t end_handle = 0xFF;
        gatts_service_changed_indicate(conn_id, start_handle, end_handle);
    }
 * \endcode
 */
bool gatts_service_changed_indicate(uint8_t conn_id, uint16_t start_handle, uint16_t end_handle);

/** End of GAP_GATT_SERVICE_Exported_Functions
* @}
*/

/** End of GAP_GATT_SERVICE
* @}
*/


#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif  /* _BUILTIN_SERVICES_H_ */
