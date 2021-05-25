/**
*****************************************************************************************
*     Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    simple_ble_client.c
  * @brief   Simple BLE client source file.
  * @details
  * @author  jane
  * @date    2016-02-18
  * @version v1.0
  ******************************************************************************
  */

/** Add Includes here **/
#include <bt_flags.h>
#if F_BT_LE_GATT_CLIENT_SUPPORT
#include <trace_app.h>
#include <string.h>
#include <simple_ble_client.h>
#include <os_mem.h>

/********************************************************************************************************
* local static variables defined here, only used in this source file.
********************************************************************************************************/

typedef struct
{
    T_SIMP_DISC_STATE disc_state;
    uint16_t          hdl_cache[HDL_SIMBLE_CACHE_LEN];
} T_SIMP_LINK, *P_SIMP_LINK;

static P_SIMP_LINK simp_table;
static uint8_t simp_link_num;

/**<  Simple BLE client ID. */
static T_CLIENT_ID simp_client = CLIENT_PROFILE_GENERAL_ID;
/**<  Callback used to send data to app from Simple BLE client layer. */
static P_FUN_GENERAL_APP_CB simp_client_cb = NULL;

/**
  * @brief  Used by application, to start the discovery procedure of Simple BLE server.
  * @param[in]  conn_id connection ID.
  * @retval true send request to upper stack success.
  * @retval false send request to upper stack failed.
  */
bool simp_ble_client_start_discovery(uint8_t conn_id)
{
    APP_PRINT_INFO0("simp_ble_client_start_discovery");
    if (conn_id >= simp_link_num)
    {
        APP_PRINT_ERROR1("simp_ble_client_start_discovery: failed invalid conn_id %d", conn_id);
        return false;
    }
    /* First clear handle cache. */
    memset(&simp_table[conn_id], 0, sizeof(T_SIMP_LINK));
    simp_table[conn_id].disc_state = DISC_SIMP_START;
    if (client_by_uuid_srv_discovery(conn_id, simp_client,
                                     GATT_UUID_SIMPLE_PROFILE) == GAP_CAUSE_SUCCESS)
    {
        return true;
    }
    return false;
}

/**
  * @brief  Used by application, to read data from server by using handles.
  * @param[in]  conn_id connection ID.
  * @param[in]  read_type one of characteristic that has the readable property.
  * @retval true send request to upper stack success.
  * @retval false send request to upper stack failed.
  */
bool simp_ble_client_read_by_handle(uint8_t conn_id, T_SIMP_READ_TYPE read_type)
{
    bool hdl_valid = false;
    uint16_t  handle;
    if (conn_id >= simp_link_num)
    {
        APP_PRINT_ERROR1("simp_ble_client_read_by_handle: failed invalid conn_id %d", conn_id);
        return false;
    }

    switch (read_type)
    {
    case SIMP_READ_V1_READ:
        if (simp_table[conn_id].hdl_cache[HDL_SIMBLE_V1_READ])
        {
            handle = simp_table[conn_id].hdl_cache[HDL_SIMBLE_V1_READ];
            hdl_valid = true;
        }
        break;
    case SIMP_READ_V3_NOTIFY_CCCD:
        if (simp_table[conn_id].hdl_cache[HDL_SIMBLE_V3_NOTIFY_CCCD])
        {
            handle = simp_table[conn_id].hdl_cache[HDL_SIMBLE_V3_NOTIFY_CCCD];
            hdl_valid = true;
        }
        break;
    case SIMP_READ_V4_INDICATE_CCCD:
        if (simp_table[conn_id].hdl_cache[HDL_SIMBLE_V4_INDICATE_CCCD])
        {
            handle = simp_table[conn_id].hdl_cache[HDL_SIMBLE_V4_INDICATE_CCCD];
            hdl_valid = true;
        }
        break;

    default:
        return false;
    }

    if (hdl_valid)
    {
        if (client_attr_read(conn_id, simp_client, handle) == GAP_CAUSE_SUCCESS)
        {
            return true;
        }
    }

    APP_PRINT_WARN0("simp_ble_client_read_by_handle: Request fail! Please check!");
    return false;
}

/**
  * @brief  Used by application, to read data from server by using UUIDs.
  * @param[in]  conn_id connection ID.
  * @param[in]  read_type one of characteristic that has the readable property.
  * @retval true send request to upper stack success.
  * @retval false send request to upper stack failed.
  */
bool simp_ble_client_read_by_uuid(uint8_t conn_id, T_SIMP_READ_TYPE read_type)
{
    uint16_t start_handle;
    uint16_t end_handle;
    uint16_t  uuid16;
    if (conn_id >= simp_link_num)
    {
        APP_PRINT_ERROR1("simp_ble_client_read_by_uuid: failed invalid conn_id %d", conn_id);
        return false;
    }

    switch (read_type)
    {
    case SIMP_READ_V1_READ:
        start_handle = simp_table[conn_id].hdl_cache[HDL_SIMBLE_SRV_START];
        end_handle = simp_table[conn_id].hdl_cache[HDL_SIMBLE_SRV_END];
        uuid16 = GATT_UUID_CHAR_SIMPLE_V1_READ;
        break;
    case SIMP_READ_V3_NOTIFY_CCCD:
        start_handle = simp_table[conn_id].hdl_cache[HDL_SIMBLE_V3_NOTIFY];
        end_handle = simp_table[conn_id].hdl_cache[HDL_SIMBLE_V4_INDICATE];
        uuid16 = GATT_UUID_CHAR_CLIENT_CONFIG;
        break;
    case SIMP_READ_V4_INDICATE_CCCD:
        start_handle = simp_table[conn_id].hdl_cache[HDL_SIMBLE_V4_INDICATE];
        end_handle = simp_table[conn_id].hdl_cache[HDL_SIMBLE_SRV_END];
        uuid16 = GATT_UUID_CHAR_CLIENT_CONFIG;
        break;
    default:
        return false;
    }

    if (client_attr_read_using_uuid(conn_id, simp_client, start_handle, end_handle,
                                    uuid16, NULL) == GAP_CAUSE_SUCCESS)
    {
        return true;
    }
    return false;
}

/**
  * @brief  Used by application, to enable or disable the notification of peer server's V3 Notify Characteristic.
  * @param[in]  conn_id connection ID.
  * @param[in]  notify 0--disable the notification, 1--enable the notification.
  * @retval true send request to upper stack success.
  * @retval false send request to upper stack failed.
  */
bool simp_ble_client_set_v3_notify(uint8_t conn_id, bool notify)
{
    if (conn_id >= simp_link_num)
    {
        APP_PRINT_ERROR1("simp_ble_client_set_v3_notify: failed invalid conn_id %d", conn_id);
        return false;
    }

    if (simp_table[conn_id].hdl_cache[HDL_SIMBLE_V3_NOTIFY_CCCD])
    {
        uint16_t handle = simp_table[conn_id].hdl_cache[HDL_SIMBLE_V3_NOTIFY_CCCD];
        uint16_t length = sizeof(uint16_t);
        uint16_t cccd_bits = notify ? 1 : 0;
        if (client_attr_write(conn_id, simp_client, GATT_WRITE_TYPE_REQ, handle,
                              length, (uint8_t *)&cccd_bits) == GAP_CAUSE_SUCCESS)
        {
            return true;
        }
    }

    APP_PRINT_WARN0("simp_ble_client_set_v3_notify: Request fail! Please check!");
    return false;
}

/**
  * @brief  Used by application, to enable or disable the indication of peer server's V4 Indicate Characteristic.
  * @param[in]  conn_id connection ID.
  * @param[in]  ind 0--disable the indication, 1--enable the indication.
  * @retval true send request to upper stack success.
  * @retval false send request to upper stack failed.
  */
bool simp_ble_client_set_v4_ind(uint8_t conn_id, bool ind)
{
    if (conn_id >= simp_link_num)
    {
        APP_PRINT_ERROR1("simp_ble_client_set_v4_ind: failed invalid conn_id %d", conn_id);
        return false;
    }

    if (simp_table[conn_id].hdl_cache[HDL_SIMBLE_V4_INDICATE_CCCD])
    {
        uint16_t handle = simp_table[conn_id].hdl_cache[HDL_SIMBLE_V4_INDICATE_CCCD];
        uint16_t length = sizeof(uint16_t);
        uint16_t cccd_bits = ind ? 2 : 0;
        if (client_attr_write(conn_id, simp_client, GATT_WRITE_TYPE_REQ, handle,
                              length, (uint8_t *)&cccd_bits) == GAP_CAUSE_SUCCESS)
        {
            return true;
        }
    }

    APP_PRINT_WARN0("simp_ble_client_set_v4_ind: Request fail! Please check!");
    return false;
}

/**
  * @brief  Used by application, to write data of V2 write Characteristic.
  * @param[in]  conn_id connection ID.
  * @param[in]  length  write data length
  * @param[in]  p_value point the value to write
  * @param[in]  type    write type.
  * @retval true send request to upper stack success.
  * @retval false send request to upper stack failed.
  */
bool simp_ble_client_write_v2_char(uint8_t conn_id, uint16_t length, uint8_t *p_value,
                                   T_GATT_WRITE_TYPE type)
{
    if (conn_id >= simp_link_num)
    {
        APP_PRINT_ERROR1("simp_ble_client_write_v2_char: failed invalid conn_id %d", conn_id);
        return false;
    }

    if (simp_table[conn_id].hdl_cache[HDL_SIMBLE_V2_WRITE])
    {
        uint16_t handle = simp_table[conn_id].hdl_cache[HDL_SIMBLE_V2_WRITE];
        if (client_attr_write(conn_id, simp_client, type, handle,
                              length, p_value) == GAP_CAUSE_SUCCESS)
        {
            return true;
        }
    }

    APP_PRINT_WARN0("simp_ble_client_write_v2_char: Request fail! Please check!");
    return false;
}

/**
  * @brief  Used by application, to get handle cache.
  * @param[in]  conn_id connection ID.
  * @param[in]  p_hdl_cache pointer of the handle cache table
  * @param[in]  len the length of handle cache table
  * @retval true success.
  * @retval false failed.
  */
bool simp_ble_client_get_hdl_cache(uint8_t conn_id, uint16_t *p_hdl_cache, uint8_t len)
{
    if (conn_id >= simp_link_num)
    {
        APP_PRINT_ERROR1("simp_ble_client_get_hdl_cache: failed invalid conn_id %d", conn_id);
        return false;
    }
    if (simp_table[conn_id].disc_state != DISC_SIMP_DONE)
    {
        APP_PRINT_ERROR1("simp_ble_client_get_hdl_cache: failed invalid state %d",
                         simp_table[conn_id].disc_state);
        return false;
    }
    if (len != sizeof(uint16_t) * HDL_SIMBLE_CACHE_LEN)
    {
        APP_PRINT_ERROR1("simp_ble_client_get_hdl_cache: failed invalid len %d", len);
        return false;
    }
    memcpy(p_hdl_cache, simp_table[conn_id].hdl_cache, len);
    return true;
}

/**
  * @brief  Used by application, to set handle cache.
  * @param[in]  conn_id connection ID.
  * @param[in]  p_hdl_cache pointer of the handle cache table
  * @param[in]  len the length of handle cache table
  * @retval true success.
  * @retval false failed.
  */
bool simp_ble_client_set_hdl_cache(uint8_t conn_id, uint16_t *p_hdl_cache, uint8_t len)
{
    if (conn_id >= simp_link_num)
    {
        APP_PRINT_ERROR1("simp_ble_client_set_hdl_cache: failed invalid conn_id %d", conn_id);
        return false;
    }
    if (simp_table[conn_id].disc_state != DISC_SIMP_IDLE)
    {
        APP_PRINT_ERROR1("simp_ble_client_set_hdl_cache: failed invalid state %d",
                         simp_table[conn_id].disc_state);
        return false;
    }
    if (len != sizeof(uint16_t) * HDL_SIMBLE_CACHE_LEN)
    {
        APP_PRINT_ERROR1("simp_ble_client_set_hdl_cache: failed invalid len %d", len);
        return false;
    }
    memcpy(simp_table[conn_id].hdl_cache, p_hdl_cache, len);
    simp_table[conn_id].disc_state = DISC_SIMP_DONE;
    return true;
}

static bool simp_ble_client_start_char_discovery(uint8_t conn_id)
{
    uint16_t start_handle;
    uint16_t end_handle;

    APP_PRINT_INFO0("simp_ble_client_start_simp_char_discovery");
    start_handle = simp_table[conn_id].hdl_cache[HDL_SIMBLE_SRV_START];
    end_handle = simp_table[conn_id].hdl_cache[HDL_SIMBLE_SRV_END];
    if (client_all_char_discovery(conn_id, simp_client, start_handle,
                                  end_handle) == GAP_CAUSE_SUCCESS)
    {
        return true;
    }
    return false;
}

static bool simp_ble_client_start_char_descriptor_discovery(uint8_t conn_id)
{
    uint16_t start_handle;
    uint16_t end_handle;

    APP_PRINT_INFO0("simp_ble_client_start_char_descriptor_discovery");
    start_handle = simp_table[conn_id].hdl_cache[HDL_SIMBLE_V3_NOTIFY];
    end_handle = simp_table[conn_id].hdl_cache[HDL_SIMBLE_SRV_END];
    if (client_all_char_descriptor_discovery(conn_id, simp_client, start_handle,
                                             end_handle) == GAP_CAUSE_SUCCESS)
    {
        return true;
    }
    return false;
}
static void simp_ble_client_discover_state_cb(uint8_t conn_id,  T_DISCOVERY_STATE discovery_state)
{
    bool cb_flag = false;
    T_SIMP_CLIENT_CB_DATA cb_data;
    cb_data.cb_type = SIMP_CLIENT_CB_TYPE_DISC_STATE;

    APP_PRINT_INFO1("simp_ble_client_discover_state_cb: discovery_state %d", discovery_state);
    if (simp_table[conn_id].disc_state == DISC_SIMP_START)
    {
        uint16_t *hdl_cache;
        hdl_cache = simp_table[conn_id].hdl_cache;

        switch (discovery_state)
        {
        case DISC_STATE_SRV_DONE:
            /* Indicate that service handle found. Start discover characteristic. */
            if ((hdl_cache[HDL_SIMBLE_SRV_START] != 0)
                || (hdl_cache[HDL_SIMBLE_SRV_END] != 0))
            {
                if (simp_ble_client_start_char_discovery(conn_id) == false)
                {
                    simp_table[conn_id].disc_state = DISC_SIMP_FAILED;
                    cb_flag = true;
                }
            }
            /* No Simple BLE service handle found. Discover procedure complete. */
            else
            {
                simp_table[conn_id].disc_state = DISC_SIMP_FAILED;
                cb_flag = true;
            }
            break;
        case DISC_STATE_CHAR_DONE:
            if (hdl_cache[HDL_SIMBLE_V3_NOTIFY] != 0)
            {
                if (simp_ble_client_start_char_descriptor_discovery(conn_id) == false)
                {
                    simp_table[conn_id].disc_state = DISC_SIMP_FAILED;
                    cb_flag = true;
                }
            }
            else
            {
                simp_table[conn_id].disc_state = DISC_SIMP_FAILED;
                cb_flag = true;
            }
            break;
        case DISC_STATE_CHAR_DESCRIPTOR_DONE:
            simp_table[conn_id].disc_state = DISC_SIMP_DONE;
            cb_flag = true;
            break;
        case DISC_STATE_FAILED:
            simp_table[conn_id].disc_state = DISC_SIMP_FAILED;
            cb_flag = true;
            break;
        default:
            APP_PRINT_ERROR0("simp_handle_discover_state: Invalid Discovery State!");
            break;
        }
    }

    /* Send discover state to application if needed. */
    if (cb_flag && simp_client_cb)
    {
        cb_data.cb_content.disc_state = simp_table[conn_id].disc_state;
        (*simp_client_cb)(simp_client, conn_id, &cb_data);
    }
    return;
}

/**
  * @brief  Called by profile client layer, when discover result fetched.
  * @param  conn_id: connection ID.
  * @param  result_type: indicate which type of value discovered in service discovery procedure.
  * @param  result_data: value discovered.
  * @retval None
  */
static void simp_ble_client_discover_result_cb(uint8_t conn_id,
                                               T_DISCOVERY_RESULT_TYPE result_type,
                                               T_DISCOVERY_RESULT_DATA result_data)
{
    APP_PRINT_INFO1("simp_ble_client_discover_result_cb: result_type %d", result_type);
    if (simp_table[conn_id].disc_state == DISC_SIMP_START)
    {
        uint16_t handle;
        uint16_t *hdl_cache;
        hdl_cache = simp_table[conn_id].hdl_cache;

        switch (result_type)
        {
        case DISC_RESULT_SRV_DATA:
            hdl_cache[HDL_SIMBLE_SRV_START] = result_data.p_srv_disc_data->att_handle;
            hdl_cache[HDL_SIMBLE_SRV_END] = result_data.p_srv_disc_data->end_group_handle;
            break;

        case DISC_RESULT_CHAR_UUID16:
            handle = result_data.p_char_uuid16_disc_data->value_handle;
            switch (result_data.p_char_uuid16_disc_data->uuid16)
            {
            case GATT_UUID_CHAR_SIMPLE_V1_READ:
                hdl_cache[HDL_SIMBLE_V1_READ] = handle;
                break;

            case GATT_UUID_CHAR_SIMPLE_V2_WRITE:
                hdl_cache[HDL_SIMBLE_V2_WRITE] = handle;
                break;

            case GATT_UUID_CHAR_SIMPLE_V3_NOTIFY:
                hdl_cache[HDL_SIMBLE_V3_NOTIFY] = handle;
                break;

            case GATT_UUID_CHAR_SIMPLE_V4_INDICATE:
                hdl_cache[HDL_SIMBLE_V4_INDICATE] = handle;
                break;

            default:
                /* have no intrest on this handle. */
                break;
            }

            break;

        case DISC_RESULT_CHAR_DESC_UUID16:
            /* When use client_all_char_descriptor_discovery. */
            if (result_data.p_char_desc_uuid16_disc_data->uuid16 == GATT_UUID_CHAR_CLIENT_CONFIG)
            {
                handle = result_data.p_char_desc_uuid16_disc_data->handle;
                if ((handle > hdl_cache[HDL_SIMBLE_V3_NOTIFY])
                    && (handle < hdl_cache[HDL_SIMBLE_V4_INDICATE]))
                {
                    hdl_cache[HDL_SIMBLE_V3_NOTIFY_CCCD] = handle;
                }
                else if ((handle > hdl_cache[HDL_SIMBLE_V4_INDICATE]) &&
                         (hdl_cache[HDL_SIMBLE_V4_INDICATE_CCCD] == 0))
                {
                    hdl_cache[HDL_SIMBLE_V4_INDICATE_CCCD] = handle;
                }
            }
            break;

        default:
            APP_PRINT_ERROR0("simp_handle_discover_result: Invalid Discovery Result Type!");
            break;
        }
    }

    return;
}

static void simp_ble_client_read_result_cb(uint8_t conn_id,  uint16_t cause,
                                           uint16_t handle, uint16_t value_size, uint8_t *p_value)
{
    T_SIMP_CLIENT_CB_DATA cb_data;
    uint16_t *hdl_cache;
    hdl_cache = simp_table[conn_id].hdl_cache;

    cb_data.cb_type = SIMP_CLIENT_CB_TYPE_READ_RESULT;

    APP_PRINT_INFO2("simp_ble_client_read_result_cb: handle 0x%x, cause 0x%x", handle, cause);
    cb_data.cb_content.read_result.cause = cause;

    if (handle == hdl_cache[HDL_SIMBLE_V1_READ])
    {
        cb_data.cb_content.read_result.type = SIMP_READ_V1_READ;
        if (cause == GAP_SUCCESS)
        {
            cb_data.cb_content.read_result.data.v1_read.p_value = p_value;
            cb_data.cb_content.read_result.data.v1_read.value_size = value_size;
        }
        else
        {
            cb_data.cb_content.read_result.data.v1_read.value_size = 0;
        }
    }
    else if (handle == hdl_cache[HDL_SIMBLE_V3_NOTIFY_CCCD])
    {
        cb_data.cb_content.read_result.type = SIMP_READ_V3_NOTIFY_CCCD;
        if (cause == GAP_SUCCESS)
        {
            uint16_t ccc_bit;
            if (value_size != 2)
            {
                APP_PRINT_ERROR1("simp_ble_client_read_result_cb: invalid cccd len %d", value_size);
                return;
            }
            LE_ARRAY_TO_UINT16(ccc_bit, p_value);

            if (ccc_bit & GATT_CLIENT_CHAR_CONFIG_NOTIFY)
            {
                cb_data.cb_content.read_result.data.v3_notify_cccd = true;
            }
            else
            {
                cb_data.cb_content.read_result.data.v3_notify_cccd = false;
            }
        }
    }
    else if (handle == hdl_cache[HDL_SIMBLE_V4_INDICATE_CCCD])
    {
        cb_data.cb_content.read_result.type = SIMP_READ_V4_INDICATE_CCCD;
        if (cause == GAP_SUCCESS)
        {
            uint16_t ccc_bit;
            if (value_size != 2)
            {
                APP_PRINT_ERROR1("simp_ble_client_read_result_cb: invalid cccd len %d", value_size);
                return;
            }
            LE_ARRAY_TO_UINT16(ccc_bit, p_value);

            if (ccc_bit & GATT_CLIENT_CHAR_CONFIG_INDICATE)
            {
                cb_data.cb_content.read_result.data.v4_indicate_cccd = true;
            }
            else
            {
                cb_data.cb_content.read_result.data.v4_indicate_cccd = false;
            }
        }
    }
    else
    {
        return;
    }
    /* Inform application the read result. */
    if (simp_client_cb)
    {
        (*simp_client_cb)(simp_client, conn_id, &cb_data);
    }

    return;
}

static void simp_ble_client_write_result_cb(uint8_t conn_id, T_GATT_WRITE_TYPE type,
                                            uint16_t handle, uint16_t cause,
                                            uint8_t credits)
{
    T_SIMP_CLIENT_CB_DATA cb_data;
    uint16_t *hdl_cache;
    hdl_cache = simp_table[conn_id].hdl_cache;
    cb_data.cb_type = SIMP_CLIENT_CB_TYPE_WRITE_RESULT;

    APP_PRINT_INFO2("simp_ble_client_write_result_cb: handle 0x%x, cause 0x%x", handle, cause);
    cb_data.cb_content.write_result.cause = cause;

    if (handle == hdl_cache[HDL_SIMBLE_V2_WRITE])
    {
        cb_data.cb_content.write_result.type = SIMP_WRITE_V2_WRITE;
    }
    else if (handle == hdl_cache[HDL_SIMBLE_V3_NOTIFY_CCCD])
    {
        cb_data.cb_content.write_result.type = SIMP_WRITE_V3_NOTIFY_CCCD;
    }
    else if (handle == hdl_cache[HDL_SIMBLE_V4_INDICATE_CCCD])
    {
        cb_data.cb_content.write_result.type = SIMP_WRITE_V4_INDICATE_CCCD;
    }
    else
    {
        return;
    }
    /* Inform application the write result. */
    if (simp_client_cb)
    {
        (*simp_client_cb)(simp_client, conn_id, &cb_data);
    }

    return;
}

static T_APP_RESULT simp_ble_client_notif_ind_result_cb(uint8_t conn_id, bool notify,
                                                        uint16_t handle,
                                                        uint16_t value_size, uint8_t *p_value)
{
    T_APP_RESULT app_result = APP_RESULT_SUCCESS;
    T_SIMP_CLIENT_CB_DATA cb_data;
    uint16_t *hdl_cache;
    hdl_cache = simp_table[conn_id].hdl_cache;

    cb_data.cb_type = SIMP_CLIENT_CB_TYPE_NOTIF_IND_RESULT;

    if (handle == hdl_cache[HDL_SIMBLE_V3_NOTIFY])
    {
        cb_data.cb_content.notif_ind_data.type = SIMP_V3_NOTIFY;
        cb_data.cb_content.notif_ind_data.data.value_size = value_size;
        cb_data.cb_content.notif_ind_data.data.p_value = p_value;
    }
    else if (handle == hdl_cache[HDL_SIMBLE_V4_INDICATE])
    {
        cb_data.cb_content.notif_ind_data.type = SIMP_V4_INDICATE;
        cb_data.cb_content.notif_ind_data.data.value_size = value_size;
        cb_data.cb_content.notif_ind_data.data.p_value = p_value;
    }
    else
    {
        return app_result;
    }
    /* Inform application the notif/ind result. */
    if (simp_client_cb)
    {
        app_result = (*simp_client_cb)(simp_client, conn_id, &cb_data);
    }

    return app_result;
}

static void simp_ble_client_disconnect_cb(uint8_t conn_id)
{
    APP_PRINT_INFO0("simp_ble_client_disconnect_cb.");
    if (conn_id >= simp_link_num)
    {
        APP_PRINT_ERROR1("simp_ble_client_disconnect_cb: failed invalid conn_id %d", conn_id);
        return;
    }
    memset(&simp_table[conn_id], 0, sizeof(T_SIMP_LINK));
    return;
}

/**
 * @brief Simple BLE Client Callbacks.
*/
const T_FUN_CLIENT_CBS simp_ble_client_cbs =
{
    simp_ble_client_discover_state_cb,   //!< Discovery State callback function pointer
    simp_ble_client_discover_result_cb,  //!< Discovery result callback function pointer
    simp_ble_client_read_result_cb,      //!< Read response callback function pointer
    simp_ble_client_write_result_cb,     //!< Write result callback function pointer
    simp_ble_client_notif_ind_result_cb,  //!< Notify Indicate callback function pointer
    simp_ble_client_disconnect_cb       //!< Link disconnection callback function pointer
};

/**
  * @brief      Add simple ble service client to application.
  * @param[in]  app_cb pointer of app callback function to handle specific client module data.
  * @param[in]  link_num initialize link num.
  * @return Client ID of the specific client module.
  * @retval 0xff failed.
  * @retval other success.
  *
  * <b>Example usage</b>
  * \code{.c}
    void app_le_profile_init(void)
    {
        client_init(1);
        simple_ble_client_id = simp_ble_add_client(app_client_callback, APP_MAX_LINKS);
    }
  * \endcode
  */
T_CLIENT_ID simp_ble_add_client(P_FUN_GENERAL_APP_CB app_cb, uint8_t link_num)
{
    uint16_t size;
    if (link_num > SIMP_MAX_LINKS)
    {
        APP_PRINT_ERROR1("simp_ble_add_client: invalid link_num %d", link_num);
        return 0xff;
    }
    if (false == client_register_spec_client_cb(&simp_client, &simp_ble_client_cbs))
    {
        simp_client = CLIENT_PROFILE_GENERAL_ID;
        APP_PRINT_ERROR0("simp_ble_add_client failed");
        return simp_client;
    }
    APP_PRINT_INFO1("simp_ble_add_client: simp_client %d", simp_client);

    /* register callback for profile to inform application that some events happened. */
    simp_client_cb = app_cb;
    simp_link_num = link_num;
    size = simp_link_num * sizeof(T_SIMP_LINK);
    simp_table = os_mem_zalloc(RAM_TYPE_DATA_ON, size);

    return simp_client;
}
#endif
