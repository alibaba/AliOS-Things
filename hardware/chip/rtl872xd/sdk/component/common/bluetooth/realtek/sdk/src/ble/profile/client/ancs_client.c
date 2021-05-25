/**
*****************************************************************************************
*     Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    ancs_client.c
  * @brief
  * @details
  * @author  jane
  * @date    2016-02-18
  * @version v1.0
  ******************************************************************************
  */
#include <bt_flags.h>
#if F_BT_LE_GATT_CLIENT_SUPPORT
/** Add Includes here **/
#include <string.h>
#include <ancs_client.h>
#include <os_mem.h>
#include <trace_app.h>

/********************************************************************************************************
* local static variables defined here, only used in this source file.
********************************************************************************************************/
/**
 * @brief  ANCS Link control block defination.
 */
typedef struct
{
    T_ANCS_DISC_STATE       disc_state;
    bool         write_notify_value;
    uint16_t     hdl_cache[HDL_ANCS_CACHE_LEN];
} T_ANCS_LINK, *P_ANCS_LINK;

/** @brief  ANCS link table */
static P_ANCS_LINK ancs_table;
static uint8_t ancs_link_num;

/**<  ANCS client ID. */
static T_CLIENT_ID ancs_client_id = CLIENT_PROFILE_GENERAL_ID;
/**<  Callback used to send data to app from ANCS client layer. */
static P_FUN_GENERAL_APP_CB ancs_client_cb = NULL;

uint8_t ANCS_UUID128_SRV[16] = {0xd0, 0x00, 0x2d, 0x12, 0x1e, 0x4b, 0x0f, 0xa4, 0x99, 0x4e, 0xce, 0xb5, 0x31, 0xf4, 0x05, 0x79};
const uint8_t ANCS_UUID128_CHAR_CONTROL_POINT[16] = {0xd9, 0xd9, 0xaa, 0xfd, 0xbd, 0x9b, 0x21, 0x98, 0xa8, 0x49, 0xe1, 0x45, 0xf3, 0xd8, 0xd1, 0x69};
const uint8_t ANCS_UUID128_CHAR_NOTIFICATION_SOURCE[16] = {0xBD, 0x1D, 0xA2, 0x99, 0xE6, 0x25, 0x58, 0x8C, 0xD9, 0x42, 0x01, 0x63, 0x0D, 0x12, 0xBF, 0x9F};
const uint8_t ANCS_UUID128_CHAR_DATA_SOURCE[16] = {0xFB, 0x7B, 0x7C, 0xCE, 0x6A, 0xB3, 0x44, 0xBE, 0xB5, 0x4B, 0xD6, 0x24, 0xE9, 0xC6, 0xEA, 0x22};


/**
  * @brief  Used by application, to start the discovery procedure of ANCS.
  * @param[in]  conn_id connection ID.
  * @retval true  send request to upper stack success.
  * @retval false  send request to upper stack failed.
  */
bool ancs_start_discovery(uint8_t conn_id)
{
    APP_PRINT_INFO0("ancs_start_discovery");
    if (conn_id >= ancs_link_num)
    {
        APP_PRINT_ERROR1("ancs_start_discovery: failed invalid conn_id %d", conn_id);
        return false;
    }
    /* First clear handle cache. */
    memset(ancs_table[conn_id].hdl_cache, 0, 2 * HDL_ANCS_CACHE_LEN);
    ancs_table[conn_id].disc_state = DISC_ANCS_START;
    if (client_by_uuid128_srv_discovery(conn_id, ancs_client_id,
                                        ANCS_UUID128_SRV) == GAP_CAUSE_SUCCESS)
    {
        return true;
    }
    return false;
}

/**
  * @brief  Used internal, start the discovery of ANCS characteristics.
  *         NOTE--user can offer this interface for application if required.
  * @param[in]  conn_id: connection ID.
  * @retval true send request to upper stack success.
  * @retval false send request to upper stack failed.
  */
static bool ancs_start_char_discovery(uint8_t conn_id)
{
    uint16_t start_handle;
    uint16_t end_handle;

    APP_PRINT_INFO0("ancs_start_char_discovery");
    start_handle = ancs_table[conn_id].hdl_cache[HDL_ANCS_SRV_START];
    end_handle = ancs_table[conn_id].hdl_cache[HDL_ANCS_SRV_END];
    if (client_all_char_discovery(conn_id, ancs_client_id, start_handle,
                                  end_handle) == GAP_CAUSE_SUCCESS)
    {
        return true;
    }
    return false;
}

/**
  * @brief  Used by application, to set the notifcation flag of notification source.
  * @param[in]  conn_id connection ID.
  * @param[in]  notify  value to enable or disable notify.
  * @retval true send request to upper stack success.
  * @retval false send request to upper stack failed.
  */
bool ancs_set_notification_source_notify(uint8_t conn_id, bool notify)
{
    if (conn_id >= ancs_link_num)
    {
        APP_PRINT_ERROR1("ancs_set_notification_source_notify: failed invalid conn_id %d", conn_id);
        return false;
    }
    if (ancs_table[conn_id].hdl_cache[HDL_ANCS_NOTIFICATION_SOURCE_CCCD])
    {
        uint16_t handle = ancs_table[conn_id].hdl_cache[HDL_ANCS_NOTIFICATION_SOURCE_CCCD];
        uint16_t length = sizeof(uint16_t);
        uint16_t cccd_bits = notify ? 1 : 0;
        if (client_attr_write(conn_id, ancs_client_id, GATT_WRITE_TYPE_REQ, handle,
                              length, (uint8_t *)&cccd_bits) == GAP_CAUSE_SUCCESS)
        {
            ancs_table[conn_id].write_notify_value = notify;
            return true;
        }
    }
    return false;
}

/**
  * @brief  Used by application, to set the notifcation flag of data source.
  * @param[in]  conn_id connection ID.
  * @param[in]  notify  value to enable or disable notify.
  * @retval true send request to upper stack success.
  * @retval false send request to upper stack failed.
  */
bool ancs_set_data_source_notify(uint8_t conn_id, bool notify)
{
    if (conn_id >= ancs_link_num)
    {
        APP_PRINT_ERROR1("ancs_set_data_source_notify: failed invalid conn_id %d", conn_id);
        return false;
    }
    if (ancs_table[conn_id].hdl_cache[HDL_ANCS_DATA_SOURCE_CCCD])
    {
        uint16_t handle = ancs_table[conn_id].hdl_cache[HDL_ANCS_DATA_SOURCE_CCCD];
        uint16_t length = sizeof(uint16_t);
        uint16_t cccd_bits = notify ? 1 : 0;
        if (client_attr_write(conn_id, ancs_client_id, GATT_WRITE_TYPE_REQ, handle,
                              length, (uint8_t *)&cccd_bits) == GAP_CAUSE_SUCCESS)
        {
            ancs_table[conn_id].write_notify_value = notify;
            return true;
        }
    }
    return false;
}

/**
  * @brief  Used by application, to get the notifcation attribute.
  * @param[in]  conn_id           connection ID.
  * @param[in]  notification_uid  value to enable or disable notify.
  * @param[in]  p_attribute_ids   Pointer to attribute ids.
  * @param[in]  attribute_ids_len Length of attribute ids.
  * @retval true send request to upper stack success.
  * @retval false send request to upper stack failed.
  */
bool ancs_get_notification_attr(uint8_t conn_id, uint32_t notification_uid,
                                uint8_t *p_attribute_ids, uint8_t attribute_ids_len)
{
    bool result = false;
    uint8_t command_id = CP_CMD_ID_GET_NOTIFICATION_ATTR;
    uint16_t length = sizeof(command_id) + sizeof(notification_uid) + attribute_ids_len;
    if (conn_id >= ancs_link_num)
    {
        APP_PRINT_ERROR1("ancs_get_notification_attr: failed invalid conn_id %d", conn_id);
        return false;
    }
    if (attribute_ids_len > 25)
    {
        return false;
    }
    if (ancs_table[conn_id].hdl_cache[HDL_ANCS_CONTROL_POINT])
    {
        uint8_t buffer[30];
        uint16_t offset = 0;
        memcpy(buffer + offset, &command_id, sizeof(command_id));
        offset += sizeof(command_id);
        memcpy(buffer + offset, &notification_uid, sizeof(notification_uid));
        offset += sizeof(notification_uid);
        memcpy(buffer + offset, p_attribute_ids, attribute_ids_len);
        if (client_attr_write(conn_id, ancs_client_id, GATT_WRITE_TYPE_REQ,
                              ancs_table[conn_id].hdl_cache[HDL_ANCS_CONTROL_POINT],
                              length, buffer) == GAP_CAUSE_SUCCESS)
        {
            result = true;
        }
    }
    return result;
}

/**
  * @brief  Used by application, to get the app attribute.
  * @param[in]  conn_id           connection ID.
  * @param[in]  p_app_identifier  value to enable or disable notify.
  * @param[in]  p_attribute_ids   Pointer to attribute ids.
  * @param[in]  attribute_ids_len Length of attribute ids.
  * @retval true send request to upper stack success.
  * @retval false send request to upper stack failed.
  */
bool ancs_get_app_attr(uint8_t conn_id, char *p_app_identifier, uint8_t *p_attribute_ids,
                       uint8_t attribute_ids_len)
{
    bool result = false;
    uint8_t command_id = CP_CMD_ID_GET_APP_ATTR;
    uint16_t length = sizeof(command_id) + strlen((const char *)p_app_identifier) + attribute_ids_len +
                      1;
    uint8_t *p_buffer = NULL;
    if (conn_id >= ancs_link_num)
    {
        APP_PRINT_ERROR1("ancs_get_app_attr: failed invalid conn_id %d", conn_id);
        return false;
    }
    if (ancs_table[conn_id].hdl_cache[HDL_ANCS_CONTROL_POINT])
    {
        uint16_t offset = 0;
        p_buffer = os_mem_zalloc(RAM_TYPE_DATA_ON, length);
        memcpy(p_buffer + offset, &command_id, sizeof(command_id));
        offset += sizeof(command_id);
        memcpy(p_buffer + offset, p_app_identifier, strlen((const char *)p_app_identifier));
        offset += strlen((const char *)p_app_identifier);
        p_buffer[offset] = 0;
        offset += 1;
        memcpy(p_buffer + offset, p_attribute_ids, attribute_ids_len);
        if (client_attr_write(conn_id, ancs_client_id, GATT_WRITE_TYPE_REQ,
                              ancs_table[conn_id].hdl_cache[HDL_ANCS_CONTROL_POINT],
                              length, p_buffer) == GAP_CAUSE_SUCCESS)
        {
            result = true;
        }
    }
    if (p_buffer != NULL)
    {
        os_mem_free(p_buffer);
    }

    return result;
}

/**
  * @brief  Used by application, to perfome the notication action.
  * @param[in]  conn_id          Connection ID.
  * @param[in]  notification_uid Notification UUID.
  * @param[in]  action_id        Action id.
  * @retval true send request to upper stack success.
  * @retval false send request to upper stack failed.
  */
bool ancs_perform_notification_action(uint8_t conn_id, uint32_t notification_uid, uint8_t action_id)
{
    bool result = false;
    uint8_t command_id = CP_CMD_ID_PERFORM_NOTIFICATION_ACTION;
    uint16_t length = sizeof(command_id) + sizeof(notification_uid) + sizeof(action_id);
    if (conn_id >= ancs_link_num)
    {
        APP_PRINT_ERROR1("ancs_perform_notification_action: failed invalid conn_id %d", conn_id);
        return false;
    }
    if (ancs_table[conn_id].hdl_cache[HDL_ANCS_CONTROL_POINT])
    {
        uint8_t buffer[12];
        uint16_t offset = 0;
        memcpy(buffer + offset, &command_id, sizeof(command_id));
        offset += sizeof(command_id);
        memcpy(buffer + offset, &notification_uid, sizeof(notification_uid));
        offset += sizeof(notification_uid);
        memcpy(buffer + offset, &action_id, sizeof(action_id));
        if (client_attr_write(conn_id, ancs_client_id, GATT_WRITE_TYPE_REQ,
                              ancs_table[conn_id].hdl_cache[HDL_ANCS_CONTROL_POINT],
                              length, buffer) == GAP_CAUSE_SUCCESS)
        {
            result = true;
        }
    }
    return result;
}

/**
  * @brief  Called by profile client layer, when discover state of discovery procedure changed.
  * @param[in]  conn_id: connection ID.
  * @param[in]  discovery_state: current service discovery state.
  * @retval None
  */
static void ancs_client_discover_state_cb(uint8_t conn_id,  T_DISCOVERY_STATE discovery_state)
{
    bool cb_flag = false;
    T_ANCS_CB_DATA cb_data;
    cb_data.cb_type = ANCS_CLIENT_CB_TYPE_DISC_STATE;

    APP_PRINT_INFO1("ancs_client_discover_state_cb: discoveryState = %d", discovery_state);
    if (ancs_table[conn_id].disc_state == DISC_ANCS_START)
    {
        switch (discovery_state)
        {
        case DISC_STATE_SRV_DONE:
            /* Indicate that service handle found. Start discover characteristic. */
            if ((ancs_table[conn_id].hdl_cache[HDL_ANCS_SRV_START] != 0)
                || (ancs_table[conn_id].hdl_cache[HDL_ANCS_SRV_END] != 0))
            {
                if (ancs_start_char_discovery(conn_id) == false)
                {
                    ancs_table[conn_id].disc_state = DISC_ANCS_FAILED;
                    cb_flag = true;
                }
            }
            /* No ANCS handle found. Discover procedure complete. */
            else
            {
                ancs_table[conn_id].disc_state = DISC_ANCS_FAILED;
                cb_flag = true;
            }
            break;
        case DISC_STATE_CHAR_DONE:
            ancs_table[conn_id].disc_state = DISC_ANCS_DONE;
            cb_flag = true;
            break;

        case DISC_STATE_FAILED:
            ancs_table[conn_id].disc_state = DISC_ANCS_FAILED;
            cb_flag = true;
            break;
        default:
            APP_PRINT_ERROR0("Invalid Discovery State!");
            break;
        }
    }

    /* Send discover state to application if needed. */
    if (cb_flag && ancs_client_cb)
    {
        cb_data.cb_content.disc_state = ancs_table[conn_id].disc_state;
        (*ancs_client_cb)(ancs_client_id, conn_id, &cb_data);
    }
    return;
}

/**
  * @brief  Called by profile client layer, when discover result fetched.
  * @param[in]  conn_id: connection ID.
  * @param[in]  result_type: indicate which type of value discovered in service discovery procedure.
  * @param[in]  result_data: value discovered.
  * @retval None
  */
static void ancs_client_discover_result_cb(uint8_t conn_id,  T_DISCOVERY_RESULT_TYPE result_type,
                                           T_DISCOVERY_RESULT_DATA result_data)
{
    APP_PRINT_INFO1("gap_client_discover_result_cb: resultType = %d", result_type);
    if (ancs_table[conn_id].disc_state == DISC_ANCS_START)
    {
        switch (result_type)
        {
        case DISC_RESULT_SRV_DATA:
            ancs_table[conn_id].hdl_cache[HDL_ANCS_SRV_START] =
                result_data.p_srv_disc_data->att_handle;
            ancs_table[conn_id].hdl_cache[HDL_ANCS_SRV_END] =
                result_data.p_srv_disc_data->end_group_handle;
            break;
        case DISC_RESULT_CHAR_UUID128:
            {
                uint16_t handle;
                handle = result_data.p_char_uuid128_disc_data->value_handle;
                if (0 == memcmp(ANCS_UUID128_CHAR_CONTROL_POINT, result_data.p_char_uuid128_disc_data->uuid128, 16))
                {
                    ancs_table[conn_id].hdl_cache[HDL_ANCS_CONTROL_POINT] = handle;
                }
                else if (0 == memcmp(ANCS_UUID128_CHAR_NOTIFICATION_SOURCE,
                                     result_data.p_char_uuid128_disc_data->uuid128, 16))
                {
                    ancs_table[conn_id].hdl_cache[HDL_ANCS_NOTIFICATION_SOURCE] = handle;
                    ancs_table[conn_id].hdl_cache[HDL_ANCS_NOTIFICATION_SOURCE_CCCD] = handle + 1;
                }
                else if (0 == memcmp(ANCS_UUID128_CHAR_DATA_SOURCE, result_data.p_char_uuid128_disc_data->uuid128,
                                     16))
                {
                    ancs_table[conn_id].hdl_cache[HDL_ANCS_DATA_SOURCE] = handle;
                    ancs_table[conn_id].hdl_cache[HDL_ANCS_DATA_SOURCE_CCCD] = handle + 1;
                }
            }
            break;

        default:
            APP_PRINT_ERROR0("Invalid Discovery Result Type!");
            break;
        }
    }

    return;
}

/**
  * @brief  Called by profile client layer, when write result arrived.
  * @param[in]  conn_id: connection ID.
  * @param[in]  type: write type
  * @param[in]  handle: handle of the value in write request.
  * @param[in]  cause: the write result.
  * @param[in]  credits: write commands credit.
  * @retval void
  */
static void ancs_client_write_result_cb(uint8_t conn_id, T_GATT_WRITE_TYPE type,
                                        uint16_t handle,
                                        uint16_t cause,
                                        uint8_t credits)
{
    T_ANCS_CB_DATA cb_data;
    uint16_t *hdl_cache;
    hdl_cache = ancs_table[conn_id].hdl_cache;
    cb_data.cb_type = ANCS_CLIENT_CB_TYPE_WRITE_RESULT;

    APP_PRINT_INFO2("ancs_client_write_result_cb: handle=0x%x cause = 0x%x", handle, cause);
    cb_data.cb_content.write_result.cause = cause;

    if (handle == hdl_cache[HDL_ANCS_NOTIFICATION_SOURCE_CCCD])
    {
        if (ancs_table[conn_id].write_notify_value == 0)
        {
            cb_data.cb_content.write_result.type = ANCS_WRITE_NOTIFICATION_SOURCE_NOTIFY_DISABLE;
        }
        else
        {
            cb_data.cb_content.write_result.type = ANCS_WRITE_NOTIFICATION_SOURCE_NOTIFY_ENABLE;
        }
    }
    else if (handle == hdl_cache[HDL_ANCS_DATA_SOURCE_CCCD])
    {
        if (ancs_table[conn_id].write_notify_value == 0)
        {
            cb_data.cb_content.write_result.type = ANCS_WRITE_DATA_SOURCE_NOTIFY_DISABLE;
        }
        else
        {
            cb_data.cb_content.write_result.type = ANCS_WRITE_DATA_SOURCE_NOTIFY_ENABLE;
        }
    }
    else if (handle == hdl_cache[HDL_ANCS_CONTROL_POINT])
    {
        cb_data.cb_content.write_result.type = ANCS_WRITE_CONTROL_POINT;
    }
    else
    {
        return;
    }

    if (ancs_client_cb)
    {
        (*ancs_client_cb)(ancs_client_id, conn_id, &cb_data);
    }
    return;
}
/**
  * @brief  Called by profile client layer, when notification or indication arrived.
  * @param[in]  conn_id: connection ID.
  * @param[in]  notify: Whether the data is notification
  * @param[in]  handle: handle of the value in received data.
  * @param[in]  value_size: size of the value in received data.
  * @param[in]  p_value: pointer to the value in received data.
  * @retval APP_RESULT_SUCCESS procedure OK.
  * @retval other procedure exception.
  */
static T_APP_RESULT ancs_client_notify_ind_cb(uint8_t conn_id, bool notify, uint16_t handle,
                                              uint16_t value_size, uint8_t *p_value)
{
    T_APP_RESULT app_result = APP_RESULT_SUCCESS;
    T_ANCS_CB_DATA cb_data;
    uint16_t *hdl_cache;

    hdl_cache = ancs_table[conn_id].hdl_cache;
    cb_data.cb_type = ANCS_CLIENT_CB_TYPE_NOTIF_IND_RESULT;

    if (handle == hdl_cache[HDL_ANCS_DATA_SOURCE])
    {
        cb_data.cb_content.notify_data.type = ANCS_FROM_DATA_SOURCE;
        cb_data.cb_content.notify_data.value_size = value_size;
        cb_data.cb_content.notify_data.p_value = p_value;
    }
    else if (handle == hdl_cache[HDL_ANCS_NOTIFICATION_SOURCE])
    {
        cb_data.cb_content.notify_data.type = ANCS_FROM_NOTIFICATION_SOURCE;
        cb_data.cb_content.notify_data.value_size = value_size;
        cb_data.cb_content.notify_data.p_value = p_value;
    }
    else
    {
        return APP_RESULT_SUCCESS;
    }

    if (ancs_client_cb)
    {
        app_result = (*ancs_client_cb)(ancs_client_id, conn_id, &cb_data);
    }

    return app_result;
}

/**
  * @brief  Called by profile client layer, when one LE link is disconnected.
  * @param[in]  conn_id: connection ID.
  * @retval void
  */
static void ancs_client_disc_cb(uint8_t conn_id)
{
    APP_PRINT_INFO0("gap_client_disc_cb.");
    memset(&ancs_table[conn_id], 0, sizeof(T_ANCS_LINK));
    if (ancs_client_cb)
    {
        T_ANCS_CB_DATA cb_data;
        cb_data.cb_type = ANCS_CLIENT_CB_TYPE_DISCONNECT_INFO;
        (*ancs_client_cb)(ancs_client_id, conn_id, &cb_data);
    }
    return;
}
/**
 * @brief ANCS Client Callbacks.
*/
const T_FUN_CLIENT_CBS ancs_cbs =
{
    ancs_client_discover_state_cb,   //!< Discovery State callback function pointer
    ancs_client_discover_result_cb,  //!< Discovery result callback function pointer
    NULL,      //!< Read response callback function pointer
    ancs_client_write_result_cb,     //!< Write result callback function pointer
    ancs_client_notify_ind_cb,  //!< Notify Indicate callback function pointer
    ancs_client_disc_cb       //!< Link disconnection callback function pointer
};

/**
 * @brief       Add ancs client.
 *
 * @param[in]   app_cb  Callbackto notify client read/write/notify/indicate events.
 * @param[in]   link_num Initialize link number
 * @return Client ID of the specific client module.
 * @retval 0xff failed.
 * @retval other success.
 *
 * <b>Example usage</b>
 * \code{.c}
    void ancs_init(uint8_t link_num)
    {
        ancs_client = ancs_add_client(ancs_client_cb, link_num);
    }
 * \endcode
 */
T_CLIENT_ID ancs_add_client(P_FUN_GENERAL_APP_CB app_cb, uint8_t link_num)
{
    uint16_t size;
    if (link_num > ANCS_MAX_LINKS)
    {
        APP_PRINT_ERROR1("ancs_add_client: invalid link_num %d", link_num);
        return 0xff;
    }
    if (false == client_register_spec_client_cb(&ancs_client_id, &ancs_cbs))
    {
        ancs_client_id = CLIENT_PROFILE_GENERAL_ID;
        APP_PRINT_ERROR0("ancs_add_client Fail !!!");
        return ancs_client_id;
    }
    APP_PRINT_INFO1("ancs_add_client: client ID = %d", ancs_client_id);

    /* register callback for profile to inform application that some events happened. */
    ancs_client_cb = app_cb;
    ancs_link_num = link_num;
    size = ancs_link_num * sizeof(T_ANCS_LINK);
    ancs_table = os_mem_zalloc(RAM_TYPE_DATA_ON, size);

    return ancs_client_id;
}
#endif

