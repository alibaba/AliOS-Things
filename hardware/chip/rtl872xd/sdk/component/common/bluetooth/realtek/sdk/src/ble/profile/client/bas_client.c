/**
*****************************************************************************************
*     Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    bas_client.c
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
#include <bas_client.h>
#include <trace_app.h>
#include <os_mem.h>

/********************************************************************************************************
* local static variables defined here, only used in this source file.
********************************************************************************************************/

#define GATT_UUID_BATTERY                       0x180F
#define GATT_UUID_CHAR_BAS_LEVEL                0x2A19
/**
 * @brief  BAS client Link control block defination.
 */
typedef struct
{
    T_BAS_DISC_STATE disc_state;
    bool             write_notify_value;
    uint16_t         properties;
    uint16_t         hdl_cache[HDL_BAS_CACHE_LEN];
} T_BAS_LINK, *P_BAS_LINK;

static P_BAS_LINK bas_table;
static uint8_t bas_link_num;

/**<  BAS client ID. */
static T_CLIENT_ID bas_client = CLIENT_PROFILE_GENERAL_ID;
/**<  Callback used to send data to app from BAS client layer. */
static P_FUN_GENERAL_APP_CB bas_client_cb = NULL;

/**
  * @brief  Used by application, to start the discovery procedure of battery service.
  * @param[in]  conn_id connection ID.
  * @retval true send request to upper stack success.
  * @retval false send request to upper stack failed.
  *
  * <b>Example usage</b>
  * \code{.c}
    static T_USER_CMD_PARSE_RESULT cmd_basdis(T_USER_CMD_PARSED_VALUE *p_parse_value)
    {
        uint8_t conn_id = p_parse_value->dw_param[0];
        bool ret = bas_start_discovery(conn_id);
        ......
    }
  * \endcode
  */
bool bas_start_discovery(uint8_t conn_id)
{
    APP_PRINT_INFO0("bas_start_discovery");
    if (conn_id >= bas_link_num)
    {
        APP_PRINT_ERROR1("bas_start_discovery: failed invalid conn_id %d", conn_id);
        return false;
    }
    /* First clear handle cache. */
    memset(&bas_table[conn_id], 0, sizeof(T_BAS_LINK));
    bas_table[conn_id].disc_state = DISC_BAS_START;
    if (client_by_uuid_srv_discovery(conn_id, bas_client,
                                     GATT_UUID_BATTERY) == GAP_CAUSE_SUCCESS)
    {
        return true;
    }
    return false;
}

/**
  * @brief  Used by application, to read battery level.
  * @param[in]  conn_id connection ID.
  * @retval true send request to upper stack success.
  * @retval false send request to upper stack failed.
  *
  * <b>Example usage</b>
  * \code{.c}
    static T_USER_CMD_PARSE_RESULT cmd_basread(T_USER_CMD_PARSED_VALUE *p_parse_value)
    {
        uint8_t conn_id = p_parse_value->dw_param[0];
        bool ret = false;
        ret = bas_read_battery_level(conn_id);
        ......
    }
  * \endcode
  */
bool bas_read_battery_level(uint8_t conn_id)
{
    if (conn_id >= bas_link_num)
    {
        APP_PRINT_ERROR1("bas_read_battery_level: failed invalid conn_id %d", conn_id);
        return false;
    }
    if (bas_table[conn_id].hdl_cache[HDL_BAS_BATTERY_LEVEL])
    {
        uint16_t handle = bas_table[conn_id].hdl_cache[HDL_BAS_BATTERY_LEVEL];
        if (client_attr_read(conn_id, bas_client, handle) == GAP_CAUSE_SUCCESS)
        {
            return true;
        }
    }
    APP_PRINT_ERROR0("bas_read_battery_level: false handle = 0");
    return false;
}
/**
  * @brief  Used by application, to set the notification flag.
  * @param[in]  conn_id connection ID.
  * @param[in]  notify value to enable or disable notify.
  * @retval true send request to upper stack success.
  * @retval false send request to upper stack failed.
  *
  * <b>Example usage</b>
  * \code{.c}
    static T_USER_CMD_PARSE_RESULT cmd_bascccd(T_USER_CMD_PARSED_VALUE *p_parse_value)
    {
        uint8_t conn_id = p_parse_value->dw_param[0];
        bool notify = p_parse_value->dw_param[1];
        bool ret;
        ret = bas_set_notify(conn_id, notify);
        ......
    }
  * \endcode
  */
bool bas_set_notify(uint8_t conn_id, bool notify)
{
    if (conn_id >= bas_link_num)
    {
        APP_PRINT_ERROR1("bas_set_notify: failed invalid conn_id %d", conn_id);
        return false;
    }
    if (bas_table[conn_id].hdl_cache[HDL_BAS_BATTERY_LEVEL_CCCD])
    {
        uint16_t handle = bas_table[conn_id].hdl_cache[HDL_BAS_BATTERY_LEVEL_CCCD];
        uint16_t length = sizeof(uint16_t);
        uint16_t cccd_bits = notify ? 1 : 0;
        if (client_attr_write(conn_id, bas_client, GATT_WRITE_TYPE_REQ, handle,
                              length, (uint8_t *)&cccd_bits) == GAP_CAUSE_SUCCESS)
        {
            bas_table[conn_id].write_notify_value = notify;
            return true;
        }
    }
    APP_PRINT_ERROR0("bas_set_notify: false handle = 0");
    return false;
}

/**
  * @brief  Used by application, to read the notification flag.
  * @param[in]  conn_id connection ID.
  * @retval true send request to upper stack success.
  * @retval false send request to upper stack failed.
  *
  * <b>Example usage</b>
  * \code{.c}
    static T_USER_CMD_PARSE_RESULT cmd_basread(T_USER_CMD_PARSED_VALUE *p_parse_value)
    {
        uint8_t conn_id = p_parse_value->dw_param[0];
        bool ret = false;
        ret = bas_read_notify(conn_id);
        ......
    }
  * \endcode
  */
bool bas_read_notify(uint8_t conn_id)
{
    if (conn_id >= bas_link_num)
    {
        APP_PRINT_ERROR1("bas_read_notify: failed invalid conn_id %d", conn_id);
        return false;
    }
    if (bas_table[conn_id].hdl_cache[HDL_BAS_BATTERY_LEVEL_CCCD])
    {
        uint16_t handle = bas_table[conn_id].hdl_cache[HDL_BAS_BATTERY_LEVEL_CCCD];
        if (client_attr_read(conn_id, bas_client, handle) == GAP_CAUSE_SUCCESS)
        {
            return true;
        }
    }
    APP_PRINT_ERROR0("bas_read_battery_level: false handle = 0");
    return false;
}

/**
  * @brief  Used by application, to get handle cache.
  * @param[in]  conn_id connection ID.
  * @param[in]  p_hdl_cache pointer of the handle cache table
  * @param[in]  len the length of handle cache table
  * @retval true success.
  * @retval false failed.
  *
  * <b>Example usage</b>
  * \code{.c}
    static T_USER_CMD_PARSE_RESULT cmd_bashdl(T_USER_CMD_PARSED_VALUE *p_parse_value)
    {
        uint8_t conn_id = p_parse_value->dw_param[0];
        uint16_t hdl_cache[HDL_BAS_CACHE_LEN];
        bool ret = bas_get_hdl_cache(conn_id, hdl_cache,
                                     sizeof(uint16_t) * HDL_BAS_CACHE_LEN);

        ......
    }
  * \endcode
  */
bool bas_get_hdl_cache(uint8_t conn_id, uint16_t *p_hdl_cache, uint8_t len)
{
    if (conn_id >= bas_link_num)
    {
        APP_PRINT_ERROR1("bas_get_hdl_cache: failed invalid conn_id %d", conn_id);
        return false;
    }
    if (bas_table[conn_id].disc_state != DISC_BAS_DONE)
    {
        APP_PRINT_ERROR1("bas_get_hdl_cache: failed invalid state %d", bas_table[conn_id].disc_state);
        return false;
    }
    if (len != sizeof(uint16_t) * HDL_BAS_CACHE_LEN)
    {
        APP_PRINT_ERROR1("bas_get_hdl_cache: failed invalid len %d", len);
        return false;
    }
    memcpy(p_hdl_cache, bas_table[conn_id].hdl_cache, len);
    return true;
}

/**
  * @brief  Used by application, to set handle cache.
  * @param[in]  conn_id connection ID.
  * @param[in]  p_hdl_cache pointer of the handle cache table
  * @param[in]  len the length of handle cache table
  * @retval true success.
  * @retval false failed.
  *
  * <b>Example usage</b>
  * \code{.c}
    void app_discov_services(uint8_t conn_id, bool start)
    {
        ......
        if (app_srvs_table.srv_found_flags & APP_DISCOV_BAS_FLAG)
        {
            bas_set_hdl_cache(conn_id, app_srvs_table.bas_hdl_cache, sizeof(uint16_t) * HDL_BAS_CACHE_LEN);
        }
        ......
    }
  * \endcode
  */
bool bas_set_hdl_cache(uint8_t conn_id, uint16_t *p_hdl_cache, uint8_t len)
{
    if (conn_id >= bas_link_num)
    {
        APP_PRINT_ERROR1("bas_set_hdl_cache: failed invalid conn_id %d", conn_id);
        return false;
    }
    if (bas_table[conn_id].disc_state != DISC_BAS_IDLE)
    {
        APP_PRINT_ERROR1("bas_set_hdl_cache: failed invalid state %d", bas_table[conn_id].disc_state);
        return false;
    }
    if (len != sizeof(uint16_t) * HDL_BAS_CACHE_LEN)
    {
        APP_PRINT_ERROR1("bas_set_hdl_cache: failed invalid len %d", len);
        return false;
    }
    memcpy(bas_table[conn_id].hdl_cache, p_hdl_cache, len);
    bas_table[conn_id].disc_state = DISC_BAS_DONE;
    return true;
}

static bool bas_start_char_discovery(uint8_t conn_id)
{
    uint16_t start_handle;
    uint16_t end_handle;

    APP_PRINT_INFO0("bas_start_char_discovery");
    start_handle = bas_table[conn_id].hdl_cache[HDL_BAS_SRV_START];
    end_handle = bas_table[conn_id].hdl_cache[HDL_BAS_SRV_END];
    if (client_all_char_discovery(conn_id, bas_client, start_handle,
                                  end_handle) == GAP_CAUSE_SUCCESS)
    {
        return true;
    }
    return false;
}

static bool bas_start_char_descriptor_discovery(uint8_t conn_id)
{
    uint16_t start_handle;
    uint16_t end_handle;

    APP_PRINT_INFO0("bas_start_char_descriptor_discovery");
    start_handle = bas_table[conn_id].hdl_cache[HDL_BAS_BATTERY_LEVEL];
    end_handle = bas_table[conn_id].hdl_cache[HDL_BAS_SRV_END];
    if (client_all_char_descriptor_discovery(conn_id, bas_client, start_handle,
                                             end_handle) == GAP_CAUSE_SUCCESS)
    {
        return true;
    }
    return false;
}

static void bas_client_discover_state_cb(uint8_t conn_id,  T_DISCOVERY_STATE discovery_state)
{
    bool cb_flag = false;
    T_BAS_CLIENT_CB_DATA cb_data;
    cb_data.cb_type = BAS_CLIENT_CB_TYPE_DISC_STATE;

    APP_PRINT_INFO1("bas_client_discover_state_cb: discovery_state = %d", discovery_state);
    if (bas_table[conn_id].disc_state == DISC_BAS_START)
    {
        switch (discovery_state)
        {
        case DISC_STATE_SRV_DONE:
            /* Indicate that service handle found. Start discover characteristic. */
            if ((bas_table[conn_id].hdl_cache[HDL_BAS_SRV_START] != 0)
                || (bas_table[conn_id].hdl_cache[HDL_BAS_SRV_END] != 0))
            {
                if (bas_start_char_discovery(conn_id) == false)
                {
                    bas_table[conn_id].disc_state = DISC_BAS_FAILED;
                    cb_flag = true;
                }
            }
            /* No BAS handle found. Discover procedure complete. */
            else
            {
                bas_table[conn_id].disc_state = DISC_BAS_FAILED;
                cb_flag = true;
            }
            break;
        case DISC_STATE_CHAR_DONE:
            if (bas_table[conn_id].properties & GATT_CHAR_PROP_NOTIFY)
            {
                //discovery cccd
                if (bas_start_char_descriptor_discovery(conn_id) == false)
                {
                    bas_table[conn_id].disc_state = DISC_BAS_FAILED;
                    cb_flag = true;
                }
            }
            else
            {
                bas_table[conn_id].disc_state = DISC_BAS_DONE;
                cb_flag = true;
            }
            break;

        case DISC_STATE_CHAR_DESCRIPTOR_DONE:
            bas_table[conn_id].disc_state = DISC_BAS_DONE;
            cb_flag = true;
            break;

        case DISC_STATE_FAILED:
            bas_table[conn_id].disc_state = DISC_BAS_FAILED;
            cb_flag = true;
            break;

        default:
            APP_PRINT_ERROR0("Invalid Discovery State!");
            break;
        }
    }

    /* Send discover state to application if needed. */
    if (cb_flag && bas_client_cb)
    {
        cb_data.cb_content.disc_state = bas_table[conn_id].disc_state;
        (*bas_client_cb)(bas_client, conn_id, &cb_data);
    }
    return;
}


static void bas_client_discover_result_cb(uint8_t conn_id,  T_DISCOVERY_RESULT_TYPE result_type,
                                          T_DISCOVERY_RESULT_DATA result_data)
{
    APP_PRINT_INFO1("bas_client_discover_result_cb: result_type = %d", result_type);
    if (bas_table[conn_id].disc_state == DISC_BAS_START)
    {
        switch (result_type)
        {
        case DISC_RESULT_SRV_DATA:
            bas_table[conn_id].hdl_cache[HDL_BAS_SRV_START] =
                result_data.p_srv_disc_data->att_handle;
            bas_table[conn_id].hdl_cache[HDL_BAS_SRV_END] =
                result_data.p_srv_disc_data->end_group_handle;
            break;

        case DISC_RESULT_CHAR_UUID16:
            {
                uint16_t handle;
                handle = result_data.p_char_uuid16_disc_data->value_handle;
                if (result_data.p_char_uuid16_disc_data->uuid16 == GATT_UUID_CHAR_BAS_LEVEL)
                {
                    bas_table[conn_id].hdl_cache[HDL_BAS_BATTERY_LEVEL] = handle;
                    bas_table[conn_id].properties = result_data.p_char_uuid16_disc_data->properties;
                }
            }
            break;

        case DISC_RESULT_CHAR_DESC_UUID16:
            if (result_data.p_char_desc_uuid16_disc_data->uuid16 == GATT_UUID_CHAR_CLIENT_CONFIG)
            {
                bas_table[conn_id].hdl_cache[HDL_BAS_BATTERY_LEVEL_CCCD] =
                    result_data.p_char_desc_uuid16_disc_data->handle;
            }
            break;

        default:
            APP_PRINT_ERROR0("Invalid Discovery Result Type!");
            break;
        }
    }

    return;
}

static void bas_client_write_result_cb(uint8_t conn_id, T_GATT_WRITE_TYPE type,
                                       uint16_t handle,
                                       uint16_t cause,
                                       uint8_t credits)
{
    T_BAS_CLIENT_CB_DATA cb_data;
    uint16_t *hdl_cache;
    hdl_cache = bas_table[conn_id].hdl_cache;
    cb_data.cb_type = BAS_CLIENT_CB_TYPE_WRITE_RESULT;

    APP_PRINT_INFO2("bas_client_write_result_cb: handle 0x%x, cause 0x%x", handle, cause);
    cb_data.cb_content.write_result.cause = cause;

    if (handle == hdl_cache[HDL_BAS_BATTERY_LEVEL_CCCD])
    {
        if (bas_table[conn_id].write_notify_value)
        {
            cb_data.cb_content.write_result.type = BAS_WRITE_NOTIFY_ENABLE;
        }
        else
        {
            cb_data.cb_content.write_result.type = BAS_WRITE_NOTIFY_DISABLE;
        }
    }
    else
    {
        return;
    }

    if (bas_client_cb)
    {
        (*bas_client_cb)(bas_client, conn_id, &cb_data);
    }
    return;
}

static void bas_client_read_result_cb(uint8_t conn_id,  uint16_t cause,
                                      uint16_t handle, uint16_t value_size, uint8_t *p_value)
{
    T_BAS_CLIENT_CB_DATA cb_data;
    uint16_t *hdl_cache;
    hdl_cache = bas_table[conn_id].hdl_cache;
    cb_data.cb_type = BAS_CLIENT_CB_TYPE_READ_RESULT;

    APP_PRINT_INFO2("bas_client_read_result_cb: handle 0x%x, cause 0x%x", handle, cause);
    cb_data.cb_content.read_result.cause = cause;

    if (handle == hdl_cache[HDL_BAS_BATTERY_LEVEL_CCCD])
    {
        cb_data.cb_content.read_result.type = BAS_READ_NOTIFY;
        if (cause == GAP_SUCCESS)
        {
            uint16_t ccc_bit;
            if (value_size != 2)
            {
                APP_PRINT_ERROR1("bas_client_read_result_cb: invalid cccd len %d", value_size);
                return;
            }
            LE_ARRAY_TO_UINT16(ccc_bit, p_value);

            if (ccc_bit & GATT_CLIENT_CHAR_CONFIG_NOTIFY)
            {
                cb_data.cb_content.read_result.data.notify = true;
            }
            else
            {
                cb_data.cb_content.read_result.data.notify = false;
            }
        }
    }
    else if (handle == hdl_cache[HDL_BAS_BATTERY_LEVEL])
    {
        cb_data.cb_content.read_result.type = BAS_READ_BATTERY_LEVEL;
        if (cause == GAP_SUCCESS)
        {
            if (value_size != 1)
            {
                APP_PRINT_ERROR1("bas_client_read_result_cb: invalid battery value len %d", value_size);
                return;
            }
            cb_data.cb_content.read_result.data.battery_level = *p_value;
        }
    }
    else
    {
        return;
    }

    if (bas_client_cb)
    {
        (*bas_client_cb)(bas_client, conn_id, &cb_data);
    }
    return;
}

static T_APP_RESULT bas_client_notify_ind_cb(uint8_t conn_id, bool notify, uint16_t handle,
                                             uint16_t value_size, uint8_t *p_value)
{
    T_APP_RESULT app_result = APP_RESULT_SUCCESS;
    T_BAS_CLIENT_CB_DATA cb_data;
    uint16_t *hdl_cache;

    hdl_cache = bas_table[conn_id].hdl_cache;
    cb_data.cb_type = BAS_CLIENT_CB_TYPE_NOTIF_IND_RESULT;

    if (handle == hdl_cache[HDL_BAS_BATTERY_LEVEL])
    {
        cb_data.cb_content.notify_data.battery_level = *p_value;
    }
    else
    {
        return APP_RESULT_SUCCESS;
    }

    if (bas_client_cb)
    {
        app_result = (*bas_client_cb)(bas_client, conn_id, &cb_data);
    }

    return app_result;
}

static void bas_client_disc_cb(uint8_t conn_id)
{
    APP_PRINT_INFO0("bas_client_disc_cb.");
    if (conn_id >= bas_link_num)
    {
        APP_PRINT_ERROR1("bas_client_disc_cb: failed invalid conn_id %d", conn_id);
        return;
    }
    memset(&bas_table[conn_id], 0, sizeof(T_BAS_LINK));
    return;
}
/**
 * @brief BAS Client Callbacks.
*/
const T_FUN_CLIENT_CBS bas_client_cbs =
{
    bas_client_discover_state_cb,   //!< Discovery State callback function pointer
    bas_client_discover_result_cb,  //!< Discovery result callback function pointer
    bas_client_read_result_cb,      //!< Read response callback function pointer
    bas_client_write_result_cb,     //!< Write result callback function pointer
    bas_client_notify_ind_cb,       //!< Notify Indicate callback function pointer
    bas_client_disc_cb              //!< Link disconnection callback function pointer
};

/**
  * @brief      Add bas client to application.
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
        bas_client_id = bas_add_client(app_client_callback, APP_MAX_LINKS);
    }
  * \endcode
  */
T_CLIENT_ID bas_add_client(P_FUN_GENERAL_APP_CB app_cb, uint8_t link_num)
{
    uint16_t size;
    if (link_num > BAS_MAX_LINKS)
    {
        APP_PRINT_ERROR1("bas_add_client: invalid link_num %d", link_num);
        return 0xff;
    }
    if (false == client_register_spec_client_cb(&bas_client, &bas_client_cbs))
    {
        bas_client = CLIENT_PROFILE_GENERAL_ID;
        APP_PRINT_ERROR0("bas_add_client:register fail");
        return bas_client;
    }
    APP_PRINT_INFO1("bas_add_client: client id %d", bas_client);

    /* register callback for profile to inform application that some events happened. */
    bas_client_cb = app_cb;
    bas_link_num = link_num;
    size = bas_link_num * sizeof(T_BAS_LINK);
    bas_table = os_mem_zalloc(RAM_TYPE_DATA_ON, size);

    return bas_client;
}
#endif
