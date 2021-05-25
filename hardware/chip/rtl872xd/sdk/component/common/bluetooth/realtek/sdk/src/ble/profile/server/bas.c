/**
*********************************************************************************************************
*               Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     bas.c
* @brief    Battery service source file.
* @details  Interfaces to access Battery service.
* @author
* @date
* @version  v1.0
*********************************************************************************************************
*/
#include "stdint.h"
#include "stddef.h"
#include "string.h"
#include "trace_app.h"
#include "profile_server.h"
#include "bas.h"
#include "bas_config.h"

/********************************************************************************************************
* local static variables defined here, only used in this source file.
********************************************************************************************************/

#define GATT_UUID_BATTERY                       0x180F
#define GATT_UUID_CHAR_BAS_LEVEL                0x2A19

#define GATT_SVC_BAS_BATTERY_LEVEL_INDEX        2 /**< @brief Index for battery level chars's value */
#define GATT_SVC_BAS_CHAR_CCCD_INDEX            3 /**< @brief CCCD Index for battery level chars's value */


/**<  BatteryLevel value. */
static uint8_t battery_level = 0;
static bool bas_read_battery_level_pending = false;
/**<  Function pointer used to send event to application from pxp profile. */
/**<  Initiated in PXP_AddService. */
static P_FUN_SERVER_GENERAL_CB pfn_bas_cb = NULL;

/**< @brief  profile/service definition.  */
static const T_ATTRIB_APPL bas_attr_tbl[] =
{
    /*----------------- Battery Service -------------------*/
    /* <<Primary Service>>, .. */
    {
        (ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_LE),   /* flags     */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_PRIMARY_SERVICE),
            HI_WORD(GATT_UUID_PRIMARY_SERVICE),
            LO_WORD(GATT_UUID_BATTERY),              /* service UUID */
            HI_WORD(GATT_UUID_BATTERY)
        },
        UUID_16BIT_SIZE,                            /* bValueLen     */
        NULL,                                       /* p_value_context */
        GATT_PERM_READ                              /* permissions  */
    },

    /* <<Characteristic>>, .. */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
#if BAS_BATTERY_LEVEL_NOTIFY_SUPPORT
            (GATT_CHAR_PROP_READ |                    /* characteristic properties */
             GATT_CHAR_PROP_NOTIFY)
#else
            GATT_CHAR_PROP_READ
#endif
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    /* Battery Level value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHAR_BAS_LEVEL),
            HI_WORD(GATT_UUID_CHAR_BAS_LEVEL)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    }
#if BAS_BATTERY_LEVEL_NOTIFY_SUPPORT
    ,
    /* client characteristic configuration */
    {
        ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL,                   /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            HI_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            /* NOTE: this value has an instantiation for each client, a write to */
            /* this attribute does not modify this default value:                */
            LO_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT), /* client char. config. bit field */
            HI_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT)
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ | GATT_PERM_WRITE)          /* permissions */
    }
#endif
};
/**< @brief  Battery service size definition.  */
static const uint16_t bas_attr_tbl_size = sizeof(bas_attr_tbl);

/**
 * @brief       Set a battery service parameter.
 *
 *              NOTE: You can call this function with a battery service parameter type and it will set the
 *                      battery service parameter.  Battery service parameters are defined in @ref T_BAS_PARAM_TYPE.
 *                      If the "len" field sets to the size of a "uint16_t" ,the
 *                      "p_value" field must point to a data with type of "uint16_t".
 *
 * @param[in]   param_type   Battery service parameter type: @ref T_BAS_PARAM_TYPE
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
        uint8_t battery_level = 80;
        bas_set_parameter(BAS_PARAM_BATTERY_LEVEL, 1, &battery_level);
    }
 * \endcode
 */
bool bas_set_parameter(T_BAS_PARAM_TYPE param_type, uint8_t length, uint8_t *p_value)
{
    bool ret = true;

    switch (param_type)
    {
    default:
        {
            ret = false;
            APP_PRINT_ERROR0("bas_set_parameter failed");
        }
        break;

    case BAS_PARAM_BATTERY_LEVEL:
        {
            if (length != sizeof(uint8_t))
            {
                ret = false;
            }
            else
            {
                battery_level = p_value[0];
            }
        }
        break;
    }

    return ret;
}

/**
 * @brief       Send notify battery level notification data .
 *
 * @param[in]   conn_id  Connection id.
 * @param[in]   service_id  Service id.
 * @param[in]   battery_level  Battery level value.
 * @return Operation result.
 * @retval true Operation success.
 * @retval false Operation failure.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test(void)
    {
        uint8_t battery_level = 90;
        bas_battery_level_value_notify(conn_id, bas_id, battery_level);
    }
 * \endcode
 */
bool bas_battery_level_value_notify(uint8_t conn_id, uint8_t service_id, uint8_t battery_level)
{
    return server_send_data(conn_id, service_id, GATT_SVC_BAS_BATTERY_LEVEL_INDEX, &battery_level,
                            sizeof(battery_level), GATT_PDU_TYPE_ANY);
}

/**
 * @brief       Confirm for read battery level value request.
 *
 * @param[in]   conn_id  Callback when service attribute was read/write.
 * @param[in]   service_id  Callback when service attribute was read/write.
 * @param[in]   battery_level  Callback when service attribute was read/write.
 * @return Operation result.
 * @retval true Operation success.
 * @retval false Operation failure.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test(void)
    {
        uint8_t battery_level = 90;
        bas_battery_level_value_read_confirm(conn_id, bas_id, battery_level);
    }
 * \endcode
 */
bool bas_battery_level_value_read_confirm(uint8_t conn_id, uint8_t service_id,
                                          uint8_t battery_level)
{
    if (bas_read_battery_level_pending == true)
    {
        bas_read_battery_level_pending = false;
        return server_attr_read_confirm(conn_id, service_id, GATT_SVC_BAS_BATTERY_LEVEL_INDEX,
                                        &battery_level, sizeof(battery_level), APP_RESULT_SUCCESS);
    }
    else
    {
        return false;
    }
}

/**
 * @brief read characteristic data from service.
 *
 * @param conn_id           Connection ID.
 * @param service_id        ServiceID to be read.
 * @param attrib_index      Attribute index of getting characteristic data.
 * @param offset            Offset of characteritic to be read.
 * @param p_length          Length of getting characteristic data.
 * @param pp_value          Pointer to pointer of characteristic value to be read.
 * @return T_APP_RESULT
*/
T_APP_RESULT bas_attr_read_cb(uint8_t conn_id, T_SERVER_ID service_id, uint16_t attrib_index,
                              uint16_t offset, uint16_t *p_length, uint8_t **pp_value)
{
    T_APP_RESULT cause = APP_RESULT_SUCCESS;
    *p_length = 0;

    APP_PRINT_INFO2("bas_attr_read_cb attrib_index = %d offset %x", attrib_index, offset);

    switch (attrib_index)
    {
    default:
        {
            APP_PRINT_ERROR1("bas_attr_read_cb attrib_index = %d not found", attrib_index);
            cause  = APP_RESULT_ATTR_NOT_FOUND;
        }
        break;

    case GATT_SVC_BAS_BATTERY_LEVEL_INDEX:
        {
            T_BAS_CALLBACK_DATA callback_data;
            callback_data.msg_type = SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE;
            callback_data.conn_id = conn_id;
            callback_data.msg_data.read_value_index = BAS_READ_BATTERY_LEVEL;
            cause = pfn_bas_cb(service_id, (void *)&callback_data);
            if (cause == APP_RESULT_PENDING)
            {
                bas_read_battery_level_pending = true;
            }

            *pp_value = &battery_level;
            *p_length = sizeof(battery_level);
        }
        break;
    }
    return (cause);
}

/**
 * @brief update CCCD bits from stack.
 *
 * @param conn_id           Connection ID.
 * @param service_id        Service ID.
 * @param index             Attribute index of characteristic data.
 * @param ccc_bits          CCCD bits from stack.
 * @return None
*/
void bas_cccd_update_cb(uint8_t conn_id, T_SERVER_ID service_id, uint16_t index, uint16_t ccc_bits)
{
    T_BAS_CALLBACK_DATA callback_data;
    callback_data.msg_type = SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION;
    callback_data.conn_id = conn_id;
    bool handle = true;
    APP_PRINT_INFO2("bas_cccd_update_cb index = %d ccc_bits %x", index, ccc_bits);

    switch (index)
    {
    case GATT_SVC_BAS_CHAR_CCCD_INDEX:
        {
            if (ccc_bits & GATT_CLIENT_CHAR_CONFIG_NOTIFY)
            {
                callback_data.msg_data.notification_indification_index = BAS_NOTIFY_BATTERY_LEVEL_ENABLE;
            }
            else
            {
                callback_data.msg_data.notification_indification_index = BAS_NOTIFY_BATTERY_LEVEL_DISABLE;
            }
            break;
        }
    default:
        {
            handle = false;
            break;
        }

    }

    if (pfn_bas_cb && (handle == true))
    {
        pfn_bas_cb(service_id, (void *)&callback_data);
    }

    return;
}

/**
 * @brief BAS Service Callbacks.
*/
const T_FUN_GATT_SERVICE_CBS bas_cbs =
{
    bas_attr_read_cb,  // Read callback function pointer
    NULL, // Write callback function pointer
    bas_cccd_update_cb  // CCCD update callback function pointer
};

/**
  * @brief       Add battery service to the BLE stack database.
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
         bas_id = bas_add_service(app_handle_profile_message);
     }
  * \endcode
  */
T_SERVER_ID bas_add_service(void *p_func)
{
    T_SERVER_ID service_id;
    if (false == server_add_service(&service_id,
                                    (uint8_t *)bas_attr_tbl,
                                    bas_attr_tbl_size,
                                    bas_cbs))
    {
        APP_PRINT_ERROR1("bas_add_service: service_id %d", service_id);
        service_id = 0xff;
    }
    pfn_bas_cb = (P_FUN_SERVER_GENERAL_CB)p_func;
    return service_id;
}

