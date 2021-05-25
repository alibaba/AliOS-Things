/**
*****************************************************************************************
*     Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    gcs_client.c
  * @brief   General Common Services client source file.
  * @details
  * @author  jane
  * @date    2018-12-13
  * @version v0.1
  ******************************************************************************
  */

/** Add Includes here **/
#include <bt_flags.h>
#if F_BT_LE_GATT_CLIENT_SUPPORT
#include <trace_app.h>
#include <string.h>
#include <gcs_client.h>
#include <os_mem.h>
#include <bt_types.h>

/********************************************************************************************************
* local static variables defined here, only used in this source file.
********************************************************************************************************/

/**
 * @brief  GCS Client Link control block defination.
 */
typedef struct
{
    T_GCS_DISCOV_TYPE discov_type;
    uint16_t result_num;
    T_GCS_DISCOV_RESULT *p_result_table;
} T_GCS_LINK;

/** @brief  GAPS Client link table */
static T_GCS_LINK *gcs_table = NULL;
static uint8_t gcs_link_num = 0;
static uint16_t gcs_max_discov_table_num = 0;
/**<  GATTS client ID. */
static T_CLIENT_ID gcs_cl_id = CLIENT_PROFILE_GENERAL_ID;
/**<  Callback used to send data to app from gcs client layer. */
static P_FUN_GENERAL_APP_CB gcs_client_cb = NULL;

T_GAP_CAUSE gcs_all_primary_srv_discovery(uint8_t conn_id)
{
    T_GAP_CAUSE cause;
    cause = client_all_primary_srv_discovery(conn_id, gcs_cl_id);
    if (cause != GAP_CAUSE_SUCCESS)
    {
        APP_PRINT_ERROR2("gcs_all_primary_srv_discovery: failed, conn_id %d, cause %d",
                         conn_id, cause);
    }
    else
    {
        gcs_table[conn_id].discov_type = GCS_ALL_PRIMARY_SRV_DISCOV;
    }
    return cause;
}

T_GAP_CAUSE gcs_by_uuid128_srv_discovery(uint8_t conn_id, uint8_t *p_uuid128)
{
    T_GAP_CAUSE cause;
    cause = client_by_uuid128_srv_discovery(conn_id, gcs_cl_id, p_uuid128);
    if (cause != GAP_CAUSE_SUCCESS)
    {
        APP_PRINT_ERROR3("gcs_by_uuid128_srv_discovery: failed, conn_id %d, cause %d, p_uuid128 %p",
                         conn_id, cause, p_uuid128);
    }
    else
    {
        gcs_table[conn_id].discov_type = GCS_BY_UUID128_SRV_DISCOV;
    }
    return cause;
}

T_GAP_CAUSE gcs_by_uuid_srv_discovery(uint8_t conn_id, uint16_t uuid16)
{
    T_GAP_CAUSE cause;
    cause = client_by_uuid_srv_discovery(conn_id, gcs_cl_id, uuid16);
    if (cause != GAP_CAUSE_SUCCESS)
    {
        APP_PRINT_ERROR3("gcs_by_uuid_srv_discovery: failed, conn_id %d, cause %d, uuid16 0x%x",
                         conn_id, cause, uuid16);
    }
    else
    {
        gcs_table[conn_id].discov_type = GCS_BY_UUID_SRV_DISCOV;
    }
    return cause;
}

T_GAP_CAUSE gcs_all_char_discovery(uint8_t conn_id, uint16_t start_handle, uint16_t end_handle)
{
    T_GAP_CAUSE cause;
    cause = client_all_char_discovery(conn_id, gcs_cl_id, start_handle, end_handle);
    if (cause != GAP_CAUSE_SUCCESS)
    {
        APP_PRINT_ERROR4("gcs_all_char_discovery: failed, conn_id %d, cause %d, start_handle 0x%x, end_handle 0x%x",
                         conn_id, cause, start_handle, end_handle);
    }
    else
    {
        gcs_table[conn_id].discov_type = GCS_ALL_CHAR_DISCOV;
    }
    return cause;
}

T_GAP_CAUSE gcs_by_uuid_char_discovery(uint8_t conn_id, uint16_t start_handle,
                                       uint16_t end_handle, uint16_t uuid16)
{
    T_GAP_CAUSE cause;
    cause = client_by_uuid_char_discovery(conn_id, gcs_cl_id, start_handle, end_handle, uuid16);
    if (cause != GAP_CAUSE_SUCCESS)
    {
        APP_PRINT_ERROR5("gcs_by_uuid_char_discovery: failed, conn_id %d, cause %d, start_handle 0x%x, end_handle 0x%x, uuid16 0x%x",
                         conn_id, cause, start_handle, end_handle, uuid16);
    }
    else
    {
        gcs_table[conn_id].discov_type = GCS_BY_UUID_CHAR_DISCOV;
    }
    return cause;
}

T_GAP_CAUSE gcs_by_uuid128_char_discovery(uint8_t conn_id, uint16_t start_handle,
                                          uint16_t end_handle, uint8_t *p_uuid128)
{
    T_GAP_CAUSE cause;
    cause = client_by_uuid128_char_discovery(conn_id, gcs_cl_id, start_handle, end_handle, p_uuid128);
    if (cause != GAP_CAUSE_SUCCESS)
    {
        APP_PRINT_ERROR5("gcs_by_uuid128_char_discovery: failed, conn_id %d, cause %d, start_handle 0x%x, end_handle 0x%x, p_uuid128 %p",
                         conn_id, cause, start_handle, end_handle, p_uuid128);
    }
    else
    {
        gcs_table[conn_id].discov_type = GCS_BY_UUID128_CHAR_DISCOV;
    }
    return cause;
}

T_GAP_CAUSE gcs_all_char_descriptor_discovery(uint8_t conn_id, uint16_t start_handle,
                                              uint16_t end_handle)
{
    T_GAP_CAUSE cause;
    cause = client_all_char_descriptor_discovery(conn_id, gcs_cl_id, start_handle, end_handle);
    if (cause != GAP_CAUSE_SUCCESS)
    {
        APP_PRINT_ERROR4("gcs_all_char_descriptor_discovery: failed, conn_id %d, cause %d, start_handle 0x%x, end_handle 0x%x",
                         conn_id, cause, start_handle, end_handle);
    }
    else
    {
        gcs_table[conn_id].discov_type = GCS_ALL_CHAR_DESC_DISCOV;
    }
    return cause;
}

T_GAP_CAUSE gcs_attr_read(uint8_t conn_id, uint16_t  handle)
{
    T_GAP_CAUSE cause;
    cause = client_attr_read(conn_id, gcs_cl_id, handle);
    if (cause != GAP_CAUSE_SUCCESS)
    {
        APP_PRINT_ERROR3("gcs_attr_read: failed, conn_id %d, cause %d, handle 0x%x",
                         conn_id, cause, handle);
    }
    return cause;
}

T_GAP_CAUSE gcs_attr_read_using_uuid16(uint8_t conn_id, uint16_t start_handle,
                                       uint16_t end_handle, uint16_t uuid16)
{
    T_GAP_CAUSE cause;
    cause = client_attr_read_using_uuid(conn_id, gcs_cl_id, start_handle, end_handle, uuid16, NULL);
    if (cause != GAP_CAUSE_SUCCESS)
    {
        APP_PRINT_ERROR5("gcs_attr_read_using_uuid16: failed, conn_id %d, cause %d, start_handle 0x%x, end_handle 0x%x, uuid16 0x%x",
                         conn_id, cause, start_handle, end_handle, uuid16);
    }
    return cause;
}

T_GAP_CAUSE gcs_attr_read_using_uuid128(uint8_t conn_id, uint16_t start_handle,
                                        uint16_t end_handle, uint8_t *p_uuid128)
{
    T_GAP_CAUSE cause;
    cause = client_attr_read_using_uuid(conn_id, gcs_cl_id, start_handle, end_handle, 0, p_uuid128);
    if (cause != GAP_CAUSE_SUCCESS)
    {
        APP_PRINT_ERROR5("gcs_attr_read_using_uuid128: failed, conn_id %d, cause %d, start_handle 0x%x, end_handle 0x%x, p_uuid128 %p",
                         conn_id, cause, start_handle, end_handle, p_uuid128);
    }
    return cause;
}

T_GAP_CAUSE gcs_attr_ind_confirm(uint8_t conn_id)
{
    T_GAP_CAUSE cause;
    cause = client_attr_ind_confirm(conn_id);
    if (cause != GAP_CAUSE_SUCCESS)
    {
        APP_PRINT_ERROR2("gcs_attr_ind_confirm: failed, conn_id %d, cause %d",
                         conn_id, cause);
    }
    return cause;
}

T_GAP_CAUSE gcs_attr_write(uint8_t conn_id, T_GATT_WRITE_TYPE write_type, uint16_t handle,
                           uint16_t length, uint8_t *p_data)
{
    T_GAP_CAUSE cause;
    cause = client_attr_write(conn_id, gcs_cl_id, write_type, handle, length, p_data);
    if (cause != GAP_CAUSE_SUCCESS)
    {
        APP_PRINT_ERROR5("gcs_attr_write: failed, conn_id %d, cause %d, write_type %d, handle 0x%x, length %d",
                         conn_id, cause, write_type, handle, length);
    }
    return cause;
}

static bool gcs_client_add_discov_result(uint8_t conn_id,  T_DISCOVERY_RESULT_TYPE result_type,
                                         T_DISCOVERY_RESULT_DATA result_data)
{
    T_GCS_LINK *p_gcs_link;
    T_GCS_DISCOV_RESULT *p_discov_result;
    if (conn_id >= gcs_link_num)
    {
        APP_PRINT_ERROR1("gcs_client_add_discov_result: invalid conn_id %d", conn_id);
        return false;
    }
    p_gcs_link = &gcs_table[conn_id];
    if (p_gcs_link->result_num >= gcs_max_discov_table_num)
    {
        APP_PRINT_ERROR1("gcs_client_add_discov_result: failed, gcs_max_discov_table_num %d",
                         gcs_max_discov_table_num);
        return false;
    }
    p_discov_result = &(p_gcs_link->p_result_table[p_gcs_link->result_num]);
    memcpy(&(p_discov_result->result_data), result_data.p_srv_disc_data,
           sizeof(T_GCS_DISCOV_RESULT_DATA));
    p_discov_result->result_type = result_type;
    p_gcs_link->result_num += 1;
    return true;
}

static bool gcs_client_clear_discov_table(uint8_t conn_id)
{
    if (conn_id < gcs_link_num)
    {
        gcs_table[conn_id].result_num = 0;
        return true;
    }
    return false;
}

static void gcs_client_discover_state_cb(uint8_t conn_id,  T_DISCOVERY_STATE discovery_state)
{
    T_GCS_CLIENT_CB_DATA cb_data;
    cb_data.cb_type = GCS_CLIENT_CB_TYPE_DISC_RESULT;
    cb_data.cb_content.discov_result.discov_type = gcs_table[conn_id].discov_type;
    APP_PRINT_INFO1("gcs discovery_state %d", discovery_state);
    if (discovery_state == DISC_STATE_FAILED || gcs_table[conn_id].result_num == 0)
    {
        cb_data.cb_content.discov_result.is_success = false;
        cb_data.cb_content.discov_result.result_num = 0;
        cb_data.cb_content.discov_result.p_result_table = NULL;
    }
    else
    {
        cb_data.cb_content.discov_result.is_success = true;
        cb_data.cb_content.discov_result.result_num = gcs_table[conn_id].result_num;
        cb_data.cb_content.discov_result.p_result_table = gcs_table[conn_id].p_result_table;
    }
    if (gcs_client_cb)
    {
        (*gcs_client_cb)(gcs_cl_id, conn_id, &cb_data);
    }
    gcs_client_clear_discov_table(conn_id);
    return;
}

static void gcs_client_discover_result_cb(uint8_t conn_id,  T_DISCOVERY_RESULT_TYPE result_type,
                                          T_DISCOVERY_RESULT_DATA result_data)
{
    APP_PRINT_INFO1("gcs_client_discover_result_cb: result_type = %d", result_type);
    gcs_client_add_discov_result(conn_id, result_type, result_data);

    return;
}

static void gcs_client_read_result_cb(uint8_t conn_id,  uint16_t cause,
                                      uint16_t handle, uint16_t value_size, uint8_t *p_value)
{
    T_GCS_CLIENT_CB_DATA cb_data;
    APP_PRINT_INFO2("gcs_client_read_result_cb: handle 0x%x, cause 0x%x", handle, cause);

    cb_data.cb_type = GCS_CLIENT_CB_TYPE_READ_RESULT;
    cb_data.cb_content.read_result.cause = cause;
    cb_data.cb_content.read_result.handle = handle;
    cb_data.cb_content.read_result.value_size = value_size;
    cb_data.cb_content.read_result.p_value = p_value;

    if (gcs_client_cb)
    {
        (*gcs_client_cb)(gcs_cl_id, conn_id, &cb_data);
    }

    return;
}

static void gcs_client_write_result_cb(uint8_t conn_id, T_GATT_WRITE_TYPE type,
                                       uint16_t handle, uint16_t cause,
                                       uint8_t credits)
{
    (void)credits;
    T_GCS_CLIENT_CB_DATA cb_data;
    APP_PRINT_INFO2("gcs_client_write_result_cb: handle 0x%x, cause 0x%x", handle, cause);

    cb_data.cb_type = GCS_CLIENT_CB_TYPE_WRITE_RESULT;
    cb_data.cb_content.write_result.cause = cause;
    cb_data.cb_content.write_result.handle = handle;
    cb_data.cb_content.write_result.type = type;

    if (gcs_client_cb)
    {
        (*gcs_client_cb)(gcs_cl_id, conn_id, &cb_data);
    }
    return;
}

static T_APP_RESULT gcs_client_notify_ind_cb(uint8_t conn_id, bool notify,
                                             uint16_t handle,
                                             uint16_t value_size, uint8_t *p_value)
{
    T_GCS_CLIENT_CB_DATA cb_data;
    T_APP_RESULT app_result = APP_RESULT_SUCCESS;

    cb_data.cb_type = GCS_CLIENT_CB_TYPE_NOTIF_IND;
    cb_data.cb_content.notif_ind.notify = notify;
    cb_data.cb_content.notif_ind.handle = handle;
    cb_data.cb_content.notif_ind.value_size = value_size;
    cb_data.cb_content.notif_ind.p_value = p_value;

    if (gcs_client_cb)
    {
        app_result = (*gcs_client_cb)(gcs_cl_id, conn_id, &cb_data);
    }
    return app_result;
}

static void gcs_client_disc_cb(uint8_t conn_id)
{
    APP_PRINT_INFO0("gcs_client_disc_cb.");
    if (conn_id < gcs_link_num)
    {
        gcs_table[conn_id].result_num = 0;
    }
}

/**
 * @brief GATTS Client Callbacks.
*/
const T_FUN_CLIENT_CBS gcs_client_cbs =
{
    gcs_client_discover_state_cb,   //!< Discovery State callback function pointer
    gcs_client_discover_result_cb,  //!< Discovery result callback function pointer
    gcs_client_read_result_cb,      //!< Read response callback function pointer
    gcs_client_write_result_cb,     //!< Write result callback function pointer
    gcs_client_notify_ind_cb,       //!< Notify Indicate callback function pointer
    gcs_client_disc_cb              //!< Link disconnection callback function pointer
};

T_CLIENT_ID gcs_add_client(P_FUN_GENERAL_APP_CB app_cb, uint8_t link_num,
                           uint16_t max_discov_table_size)
{
    uint8_t i;
    uint16_t size;
    if (link_num > GCS_MAX_LINKS)
    {
        APP_PRINT_ERROR1("gcs_add_client: invalid link_num %d", link_num);
        return 0xff;
    }

    gcs_client_cb = app_cb;
    gcs_link_num = link_num;
    gcs_max_discov_table_num = max_discov_table_size;
    size = gcs_link_num * sizeof(T_GCS_LINK);
    gcs_table = os_mem_zalloc(RAM_TYPE_DATA_ON, size);
    if (gcs_table == NULL)
    {
        APP_PRINT_ERROR0("gcs_add_client: allocate link table mem fail");
        gcs_cl_id = 0xff;
        return gcs_cl_id;
    }
    size = gcs_max_discov_table_num * sizeof(T_GCS_DISCOV_RESULT);
    for (i = 0; i < gcs_link_num; i++)
    {

        gcs_table[i].p_result_table = os_mem_zalloc(RAM_TYPE_DATA_ON, size);
        if (gcs_table[i].p_result_table == NULL)
        {
            APP_PRINT_ERROR0("gcs_add_client: allocate discov table mem fail");
            gcs_cl_id = 0xff;
            return gcs_cl_id;
        }
    }
    if (false == client_register_spec_client_cb(&gcs_cl_id, &gcs_client_cbs))
    {
        gcs_cl_id = CLIENT_PROFILE_GENERAL_ID;
        APP_PRINT_ERROR0("gcs_add_client: register fail");
        return gcs_cl_id;
    }

    APP_PRINT_INFO1("gcs_add_client: client id %d", gcs_cl_id);
    return gcs_cl_id;
}
#endif

