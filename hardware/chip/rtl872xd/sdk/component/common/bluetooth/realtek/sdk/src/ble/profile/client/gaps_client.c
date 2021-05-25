/**
*****************************************************************************************
*     Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    gaps_client.c
  * @brief   GAPS client source file.
  * @details
  * @author  jane
  * @date    2016-02-18
  * @version v0.1
  ******************************************************************************
  */

/** Add Includes here **/
#include <bt_flags.h>
#if F_BT_LE_GATT_CLIENT_SUPPORT
#include <trace_app.h>
#include <string.h>
#include <gaps_client.h>
#include <os_mem.h>

/********************************************************************************************************
* local static variables defined here, only used in this source file.
********************************************************************************************************/

/**
 * @brief  GAPS Client Link control block defination.
 */
typedef struct
{
    T_GAPS_DISC_STATE disc_state;
    uint16_t          hdl_cache[HDL_GAPS_CACHE_LEN];
} T_GAPS_LINK, *P_GAPS_LINK;

/** @brief  GAPS Client link table */
static P_GAPS_LINK gaps_table;
static uint8_t gaps_link_num;

/**<  GAPS client ID. */
static T_CLIENT_ID gaps_client = CLIENT_PROFILE_GENERAL_ID;
/**<  Callback used to send data to app from GAPS client layer. */
static P_FUN_GENERAL_APP_CB gaps_client_cb = NULL;

/**
  * @brief  Used by application, to start the discovery procedure of GAP service.
  * @param[in]  conn_id connection ID.
  * @retval true send request to upper stack success.
  * @retval false send request to upper stack failed.
  *
  * <b>Example usage</b>
  * \code{.c}
    static T_USER_CMD_PARSE_RESULT cmd_gapdis(T_USER_CMD_PARSED_VALUE *p_parse_value)
    {
        uint8_t conn_id = p_parse_value->dw_param[0];
        bool ret = gaps_start_discovery(conn_id);
        ......
    }
 * \endcode
  */
bool gaps_start_discovery(uint8_t conn_id)
{
    APP_PRINT_INFO0("gaps_start_discovery");
    /* First clear handle cache. */
    if (conn_id >= gaps_link_num)
    {
        APP_PRINT_ERROR1("gaps_start_discovery: failed invalid conn_id %d", conn_id);
        return false;
    }
    memset(&gaps_table[conn_id], 0, sizeof(T_GAPS_LINK));
    gaps_table[conn_id].disc_state = DISC_GAPS_START;
    if (client_by_uuid_srv_discovery(conn_id, gaps_client,
                                     GATT_UUID_GAP) == GAP_CAUSE_SUCCESS)
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
  *
  * <b>Example usage</b>
  * \code{.c}
    static T_USER_CMD_PARSE_RESULT cmd_gapread(T_USER_CMD_PARSED_VALUE *p_parse_value)
    {
        uint8_t conn_id = p_parse_value->dw_param[0];
        T_GAPS_READ_TYPE read_type = (T_GAPS_READ_TYPE)p_parse_value->dw_param[1];
        bool ret = gaps_read(conn_id, read_type);
        ......
    }
 * \endcode
  */
bool gaps_read(uint8_t conn_id, T_GAPS_READ_TYPE read_type)
{
    bool hdl_valid = false;
    uint16_t  handle;
    uint16_t *hdl_cache;
    if (conn_id >= gaps_link_num)
    {
        APP_PRINT_ERROR1("gaps_read: failed invalid conn_id %d", conn_id);
        return false;
    }
    hdl_cache = gaps_table[conn_id].hdl_cache;

    APP_PRINT_INFO1("gaps_read: charType = %d", read_type);

    switch (read_type)
    {
    case GAPS_READ_DEVICE_NAME:
        if (hdl_cache[HDL_GAPS_DEVICE_NAME])
        {
            handle = hdl_cache[HDL_GAPS_DEVICE_NAME];
            hdl_valid = true;
        }
        break;
    case GAPS_READ_APPEARANCE:
        if (hdl_cache[HDL_GAPS_APPEARANCE])
        {
            handle = hdl_cache[HDL_GAPS_APPEARANCE];
            hdl_valid = true;
        }
        break;
    case GAPS_READ_CENTRAL_ADDR_RESOLUTION:
        if (hdl_cache[HDL_GAPS_CENTRAL_ADDR_RESOLUTION])
        {
            handle = hdl_cache[HDL_GAPS_CENTRAL_ADDR_RESOLUTION];
            hdl_valid = true;
        }
        break;
    default:
        return false;
    }

    if (hdl_valid)
    {
        if (client_attr_read(conn_id, gaps_client, handle) == GAP_CAUSE_SUCCESS)
        {
            return true;
        }
    }

    APP_PRINT_WARN0("gaps_read: Request fail! Please check!");
    return false;
}

/**
  * @brief  Used by application, to get handle cache.
  * @param[in]  conn_id connection ID.
  * @param[in,out]  p_hdl_cache pointer of the handle cache table
  * @param[in]  len the length of handle cache table
  * @retval true success.
  * @retval false failed.
  *
  * <b>Example usage</b>
  * \code{.c}
    static T_USER_CMD_PARSE_RESULT cmd_gaphdl(T_USER_CMD_PARSED_VALUE *p_parse_value)
    {
        uint8_t conn_id = p_parse_value->dw_param[0];
        uint16_t hdl_cache[HDL_GAPS_CACHE_LEN];
        uint8_t hdl_idx;
        bool ret = gaps_get_hdl_cache(conn_id, hdl_cache, sizeof(uint16_t) * HDL_GAPS_CACHE_LEN);
        ......
    }
 * \endcode
  */
bool gaps_get_hdl_cache(uint8_t conn_id, uint16_t *p_hdl_cache, uint8_t len)
{
    if (conn_id >= gaps_link_num)
    {
        APP_PRINT_ERROR1("gaps_get_hdl_cache: failed invalid conn_id %d", conn_id);
        return false;
    }
    if (gaps_table[conn_id].disc_state != DISC_GAPS_DONE)
    {
        APP_PRINT_ERROR1("gaps_get_hdl_cache: failed invalid state %d", gaps_table[conn_id].disc_state);
        return false;
    }
    if (len != sizeof(uint16_t) * HDL_GAPS_CACHE_LEN)
    {
        APP_PRINT_ERROR1("gaps_get_hdl_cache: failed invalid len %d", len);
        return false;
    }
    memcpy(p_hdl_cache, gaps_table[conn_id].hdl_cache, len);
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
        if (app_srvs_table.srv_found_flags & APP_DISCOV_GAPS_FLAG)
        {
            gaps_set_hdl_cache(conn_id, app_srvs_table.gaps_hdl_cache, sizeof(uint16_t) * HDL_GAPS_CACHE_LEN);
        }
        ......
    }
 * \endcode
  */
bool gaps_set_hdl_cache(uint8_t conn_id, uint16_t *p_hdl_cache, uint8_t len)
{
    if (conn_id >= gaps_link_num)
    {
        APP_PRINT_ERROR1("gaps_set_hdl_cache: failed invalid conn_id %d", conn_id);
        return false;
    }
    if (gaps_table[conn_id].disc_state != DISC_GAPS_IDLE)
    {
        APP_PRINT_ERROR1("gaps_set_hdl_cache: failed invalid state %d", gaps_table[conn_id].disc_state);
        return false;
    }
    if (len != sizeof(uint16_t) * HDL_GAPS_CACHE_LEN)
    {
        APP_PRINT_ERROR1("gaps_set_hdl_cache: failed invalid len %d", len);
        return false;
    }
    memcpy(gaps_table[conn_id].hdl_cache, p_hdl_cache, len);
    gaps_table[conn_id].disc_state = DISC_GAPS_DONE;
    return true;
}

/**
  * @brief  Used by application, to check Resolvable Private Address Only Characteristics whether existing.
  * @param[in]  conn_id connection ID.
  * @param[in,out]  p_is_exist whether existing
  * @retval true success.
  * @retval false failed.
  */
bool gaps_check_resolvable_private_addr_only_char(uint8_t conn_id, bool *p_is_exist)
{
    if (conn_id >= gaps_link_num)
    {
        APP_PRINT_ERROR1("gaps_get_hdl_cache: failed invalid conn_id %d", conn_id);
        return false;
    }
    if (gaps_table[conn_id].disc_state != DISC_GAPS_DONE)
    {
        APP_PRINT_ERROR1("gaps_get_hdl_cache: failed invalid state %d", gaps_table[conn_id].disc_state);
        return false;
    }
    if (gaps_table[conn_id].hdl_cache[HDL_GAPS_RESOLVABLE_PRIVATE_ADDR_ONLY] == 0)
    {
        *p_is_exist = false;
    }
    else
    {
        *p_is_exist = true;
    }
    return true;
}

static bool gaps_client_start_gap_char_discovery(uint8_t conn_id)
{
    uint16_t start_handle;
    uint16_t end_handle;

    APP_PRINT_INFO0("gap_client_start_gap_char_discovery");
    start_handle = gaps_table[conn_id].hdl_cache[HDL_GAPS_SRV_START];
    end_handle = gaps_table[conn_id].hdl_cache[HDL_GAPS_SRV_END];
    if (client_all_char_discovery(conn_id, gaps_client, start_handle,
                                  end_handle) == GAP_CAUSE_SUCCESS)
    {
        return true;
    }
    return false;
}

static void gaps_client_discover_state_cb(uint8_t conn_id,  T_DISCOVERY_STATE discovery_state)
{
    bool cb_flag = false;
    T_GAPS_CLIENT_CB_DATA cb_data;
    cb_data.cb_type = GAPS_CLIENT_CB_TYPE_DISC_STATE;

    APP_PRINT_INFO1("gaps_client_discover_state_cb: discovery_state %d", discovery_state);

    switch (discovery_state)
    {
    case DISC_STATE_SRV_DONE:
        /* Indicate that service handle found. Start discover characteristic. */
        if ((gaps_table[conn_id].hdl_cache[HDL_GAPS_SRV_START] != 0)
            || (gaps_table[conn_id].hdl_cache[HDL_GAPS_SRV_END] != 0))
        {
            if (gaps_client_start_gap_char_discovery(conn_id) == false)
            {
                gaps_table[conn_id].disc_state = DISC_GAPS_FAILED;
                cb_flag = true;
            }
        }
        /* No GAP service handle found. Discover procedure complete. */
        else
        {
            gaps_table[conn_id].disc_state = DISC_GAPS_FAILED;
            cb_flag = true;
        }
        break;

    case DISC_STATE_CHAR_DONE:
        gaps_table[conn_id].disc_state = DISC_GAPS_DONE;
        cb_flag = true;
        break;

    case DISC_STATE_FAILED:
        APP_PRINT_ERROR0("DISC_STATE_FAILED");
        gaps_table[conn_id].disc_state = DISC_GAPS_FAILED;
        cb_flag = true;
        break;
    default:
        APP_PRINT_ERROR0("Invalid Discovery State!");
        break;
    }
    /* Send discover state to application if needed. */
    if (cb_flag && gaps_client_cb)
    {
        cb_data.cb_content.disc_state = gaps_table[conn_id].disc_state;
        (*gaps_client_cb)(gaps_client, conn_id, &cb_data);
    }
    return;
}

static void gaps_client_discover_result_cb(uint8_t conn_id,  T_DISCOVERY_RESULT_TYPE result_type,
                                           T_DISCOVERY_RESULT_DATA result_data)
{
    APP_PRINT_INFO1("gap_client_discover_result_cb: result_type = %d", result_type);
    uint16_t handle;
    switch (result_type)
    {
    case DISC_RESULT_SRV_DATA:
        gaps_table[conn_id].hdl_cache[HDL_GAPS_SRV_START] =
            result_data.p_srv_disc_data->att_handle;
        gaps_table[conn_id].hdl_cache[HDL_GAPS_SRV_END] =
            result_data.p_srv_disc_data->end_group_handle;
        break;

    case DISC_RESULT_CHAR_UUID16:
        handle = result_data.p_char_uuid16_disc_data->value_handle;
        /* we should inform intrested handles to upper application. */
        switch (result_data.p_char_uuid16_disc_data->uuid16)
        {
        case GATT_UUID_CHAR_DEVICE_NAME:
            gaps_table[conn_id].hdl_cache[HDL_GAPS_DEVICE_NAME] = handle;
            break;
        case GATT_UUID_CHAR_APPEARANCE:
            gaps_table[conn_id].hdl_cache[HDL_GAPS_APPEARANCE] = handle;
            break;
        case GATT_UUID_CHAR_CENTRAL_ADDRESS_RESOLUTION:
            gaps_table[conn_id].hdl_cache[HDL_GAPS_CENTRAL_ADDR_RESOLUTION] = handle;
            APP_PRINT_INFO0("GATT_UUID_CHAR_CENTRAL_ADDRESS_RESOLUTION found");
            break;
        case GATT_UUID_CHAR_RESOLVABLE_PRIVATE_ADDRESS_ONLY:
            gaps_table[conn_id].hdl_cache[HDL_GAPS_RESOLVABLE_PRIVATE_ADDR_ONLY] = handle;
            APP_PRINT_INFO0("GATT_UUID_CHAR_RESOLVABLE_PRIVATE_ADDRESS_ONLY found");
            break;
        default:
            /* have no intrest on this handle. */
            break;
        }
        break;

    default:
        APP_PRINT_ERROR0("Invalid Discovery Result Type!");
        break;
    }

    return;
}

static void gaps_client_read_result_cb(uint8_t conn_id,  uint16_t cause,
                                       uint16_t handle, uint16_t value_size, uint8_t *p_value)
{
    T_GAPS_CLIENT_CB_DATA cb_data;
    uint16_t *hdl_cache;
    hdl_cache = gaps_table[conn_id].hdl_cache;
    cb_data.cb_type = GAPS_CLIENT_CB_TYPE_READ_RESULT;
    cb_data.cb_content.read_result.cause = cause;

    APP_PRINT_INFO2("gap_client_read_result_cb: handle 0x%x, cause 0x%x", handle, cause);

    if (handle == hdl_cache[HDL_GAPS_APPEARANCE])
    {
        cb_data.cb_content.read_result.type = GAPS_READ_APPEARANCE;
        if (cause == GAP_SUCCESS)
        {
            uint16_t appearance;
            if (value_size != 2)
            {
                APP_PRINT_ERROR1("gaps_client_read_result_cb: invalid cccd len %d", value_size);
                return;
            }
            LE_ARRAY_TO_UINT16(appearance, p_value);
            cb_data.cb_content.read_result.data.appearance = appearance;
        }
    }
    else if (handle == hdl_cache[HDL_GAPS_DEVICE_NAME])
    {
        cb_data.cb_content.read_result.type = GAPS_READ_DEVICE_NAME;
        if (cause == GAP_SUCCESS)
        {
            cb_data.cb_content.read_result.data.device_name.value_size = value_size;
            cb_data.cb_content.read_result.data.device_name.p_value = p_value;
        }
    }
    else if (handle == hdl_cache[HDL_GAPS_CENTRAL_ADDR_RESOLUTION])
    {
        cb_data.cb_content.read_result.type = GAPS_READ_CENTRAL_ADDR_RESOLUTION;
        if (cause == GAP_SUCCESS)
        {
            cb_data.cb_content.read_result.data.central_addr_res = *p_value;
        }
    }
    else
    {
        return;
    }

    /* Inform application the read result. */
    if (gaps_client_cb)
    {
        (*gaps_client_cb)(gaps_client, conn_id, &cb_data);
    }
    return;
}


static void gaps_client_disc_cb(uint8_t conn_id)
{
    APP_PRINT_INFO0("gap_client_disc_cb.");
    if (conn_id >= gaps_link_num)
    {
        APP_PRINT_ERROR1("gaps_client_disc_cb: failed invalid conn_id %d", conn_id);
        return;
    }
    memset(&gaps_table[conn_id], 0, sizeof(T_GAPS_LINK));
    return;
}
/**
 * @brief GAPS Client Callbacks.
*/
const T_FUN_CLIENT_CBS gaps_client_cbs =
{
    gaps_client_discover_state_cb,   //!< Discovery State callback function pointer
    gaps_client_discover_result_cb,  //!< Discovery result callback function pointer
    gaps_client_read_result_cb,      //!< Read response callback function pointer
    NULL,     //!< Write result callback function pointer
    NULL,  //!< Notify Indicate callback function pointer
    gaps_client_disc_cb       //!< Link disconnection callback function pointer
};

/**
  * @brief  Add gap service client to application.
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
        gaps_client_id  = gaps_add_client(app_client_callback, APP_MAX_LINKS);
    }
 * \endcode
  */
T_CLIENT_ID gaps_add_client(P_FUN_GENERAL_APP_CB app_cb, uint8_t link_num)
{
    uint16_t size;
    if (link_num > GAPS_MAX_LINKS)
    {
        APP_PRINT_ERROR1("gaps_add_client: invalid link_num %d", link_num);
        return 0xff;
    }
    if (false == client_register_spec_client_cb(&gaps_client, &gaps_client_cbs))
    {
        gaps_client = CLIENT_PROFILE_GENERAL_ID;
        APP_PRINT_ERROR0("gaps_add_client Fail !!!");
        return gaps_client;
    }
    APP_PRINT_INFO1("gaps_add_client: client ID = %d", gaps_client);

    /* register callback for profile to inform application that some events happened. */
    gaps_client_cb = app_cb;
    gaps_link_num = link_num;
    size = gaps_link_num * sizeof(T_GAPS_LINK);
    gaps_table = os_mem_zalloc(RAM_TYPE_DATA_ON, size);

    return gaps_client;
}
#endif

