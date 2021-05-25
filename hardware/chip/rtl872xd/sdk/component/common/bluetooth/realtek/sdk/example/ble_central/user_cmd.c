/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
   * @file      user_cmd.c
   * @brief     User defined test commands.
   * @details  User command interfaces.
   * @author    jane
   * @date      2017-06-06
   * @version   v1.0
   **************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
   **************************************************************************************
  */
/*============================================================================*
 *                              Header Files
 *============================================================================*/
#if CONFIG_BT_USER_COMMAND
#include <string.h>
#include <trace_app.h>
#include <gap_bond_le.h>
#include <gap_scan.h>
#include <user_cmd.h>
#include <gap.h>
#include <gap_conn_le.h>
#include <gcs_client.h>

/** @defgroup  CENTRAL_CLIENT_CMD Central Client User Command
    * @brief This file handles Central Client User Command.
    * @{
    */
/*============================================================================*
 *                              Variables
 *============================================================================*/
/** @brief User command interface data, used to parse the commands from Data UART. */
T_USER_CMD_IF    user_cmd_if;

/*============================================================================*
 *                              Functions
 *============================================================================*/
/**
 * @brief Show all devices connecting status
 *
 * <b>Command table define</b>
 * \code{.c}
    {
        "showcon",
        "showcon\n\r",
        "Show all devices connecting status\n\r",
        cmd_showcon
    },
 * \endcode
 */
static T_USER_CMD_PARSE_RESULT cmd_showcon(T_USER_CMD_PARSED_VALUE *p_parse_value)
{
    uint8_t conn_id;
    T_GAP_CONN_INFO conn_info;
    for (conn_id = 0; conn_id < BLE_CENTRAL_APP_MAX_LINKS; conn_id++)
    {
        if (le_get_conn_info(conn_id, &conn_info))
        {
            data_uart_print("ShowCon conn_id %d state 0x%x role %d\r\n", conn_id,
                            conn_info.conn_state, conn_info.role);
            data_uart_print("RemoteBd = [%02x:%02x:%02x:%02x:%02x:%02x] type = %d\r\n",
                            conn_info.remote_bd[5], conn_info.remote_bd[4],
                            conn_info.remote_bd[3], conn_info.remote_bd[2],
                            conn_info.remote_bd[1], conn_info.remote_bd[0],
                            conn_info.remote_bd_type);
        }
    }
    data_uart_print("active link num %d,  idle link num %d\r\n",
                    le_get_active_link_num(), le_get_idle_link_num());
    return (RESULT_SUCESS);
}

/**
 * @brief LE connection param update request
 *
 * <b>Command table define</b>
 * \code{.c}
    {
        "conupdreq",
        "conupdreq [conn_id] [interval_min] [interval_max] [latency] [supervision_timeout]\n\r",
        "LE connection param update request\r\n\
        sample: conupdreq 0 0x30 0x40 0 500\n\r",
        cmd_conupdreq
    },
 * \endcode
 */
static T_USER_CMD_PARSE_RESULT cmd_conupdreq(T_USER_CMD_PARSED_VALUE *p_parse_value)
{
    T_GAP_CAUSE cause;
    uint8_t  conn_id = p_parse_value->dw_param[0];
    uint16_t conn_interval_min = p_parse_value->dw_param[1];
    uint16_t conn_interval_max = p_parse_value->dw_param[2];
    uint16_t conn_latency = p_parse_value->dw_param[3];
    uint16_t supervision_timeout = p_parse_value->dw_param[4];


    cause = le_update_conn_param(conn_id,
                                 conn_interval_min,
                                 conn_interval_max,
                                 conn_latency,
                                 supervision_timeout,
                                 2 * (conn_interval_min - 1),
                                 2 * (conn_interval_max - 1)
                                );
    return (T_USER_CMD_PARSE_RESULT)cause;
}

/**
 * @brief Disconnect to remote device
 *
 * <b>Command table define</b>
 * \code{.c}
    {
        "disc",
        "disc [conn_id]\n\r",
        "Disconnect to remote device\n\r",
        cmd_disc
    },
 * \endcode
 */
static T_USER_CMD_PARSE_RESULT cmd_disc(T_USER_CMD_PARSED_VALUE *p_parse_value)
{
    uint8_t conn_id = p_parse_value->dw_param[0];
    T_GAP_CAUSE cause;
    cause = le_disconnect(conn_id);
    return (T_USER_CMD_PARSE_RESULT)cause;
}

/**
 * @brief Config authentication mode
 *
 * <b>Command table define</b>
 * \code{.c}
    {
        "authmode",
        "authmode [auth_flags] [io_cap] [sec_enable] [oob_enable]\n\r",
        "Config authentication mode\r\n\
        [auth_flags]:authentication req bit field: bit0-(bonding), bit2-(MITM), bit3-(SC)\r\n\
        [io_cap]:set io Capabilities: 0-(display only), 1-(display yes/no), 2-(keyboard noly), 3-(no IO), 4-(keyboard display)\r\n\
        [sec_enable]:Start smp pairing procedure when connected: 0-(disable), 1-(enable)\r\n\
        [oob_enable]:Enable oob flag: 0-(disable), 1-(enable)\r\n\
        sample: authmode 0x5 2 1 0\n\r",
        cmd_authmode
    },
 * \endcode
 */
static T_USER_CMD_PARSE_RESULT cmd_authmode(T_USER_CMD_PARSED_VALUE *p_parse_value)
{
    T_GAP_CAUSE cause;
    uint8_t  auth_pair_mode = GAP_PAIRING_MODE_PAIRABLE;
    uint16_t auth_flags = GAP_AUTHEN_BIT_BONDING_FLAG;
    uint8_t  auth_io_cap = GAP_IO_CAP_NO_INPUT_NO_OUTPUT;
    uint8_t  oob_enable = false;
    uint8_t  auth_sec_req_enable = false;
    uint16_t auth_sec_req_flags = GAP_AUTHEN_BIT_BONDING_FLAG;

    if (p_parse_value->param_count > 0)
    {
        auth_flags = p_parse_value->dw_param[0];
        auth_sec_req_flags = p_parse_value->dw_param[0];
    }
    if (p_parse_value->param_count > 1)
    {
        auth_io_cap = p_parse_value->dw_param[1];
    }
    if (p_parse_value->param_count > 2)
    {
        auth_sec_req_enable = p_parse_value->dw_param[2];
    }
    if (p_parse_value->param_count > 3)
    {
        oob_enable = p_parse_value->dw_param[3];
    }
    gap_set_param(GAP_PARAM_BOND_PAIRING_MODE, sizeof(auth_pair_mode), &auth_pair_mode);
    gap_set_param(GAP_PARAM_BOND_AUTHEN_REQUIREMENTS_FLAGS, sizeof(auth_flags), &auth_flags);
    gap_set_param(GAP_PARAM_BOND_IO_CAPABILITIES, sizeof(auth_io_cap), &auth_io_cap);
    gap_set_param(GAP_PARAM_BOND_OOB_ENABLED, sizeof(uint8_t), &oob_enable);
    le_bond_set_param(GAP_PARAM_BOND_SEC_REQ_ENABLE, sizeof(auth_sec_req_enable), &auth_sec_req_enable);
    le_bond_set_param(GAP_PARAM_BOND_SEC_REQ_REQUIREMENT, sizeof(auth_sec_req_flags),
                      &auth_sec_req_flags);
    cause = gap_set_pairable_mode();
    return (T_USER_CMD_PARSE_RESULT)cause;
}

/**
 * @brief Send authentication request
 *
 * <b>Command table define</b>
 * \code{.c}
    {
        "sauth",
        "sauth [conn_id]\n\r",
        "Send authentication request\n\r",
        cmd_sauth
    },
 * \endcode
 */
static T_USER_CMD_PARSE_RESULT cmd_sauth(T_USER_CMD_PARSED_VALUE *p_parse_value)
{
    uint8_t conn_id = p_parse_value->dw_param[0];
    T_GAP_CAUSE cause;
    cause = le_bond_pair(conn_id);
    return (T_USER_CMD_PARSE_RESULT)cause;
}

/**
 * @brief Send user confirmation when show GAP_MSG_LE_BOND_USER_CONFIRMATION
 *
 * <b>Command table define</b>
 * \code{.c}
    {
        "userconf",
        "userconf [conn_id] [conf]\n\r",
        "Send user confirmation when show GAP_MSG_LE_BOND_USER_CONFIRMATION\r\n\
        [conf]: 0-(Reject), 1-(Accept)\r\n\
        sample: userconf 0 1\n\r",
        cmd_userconf
    },
 * \endcode
 */
static T_USER_CMD_PARSE_RESULT cmd_userconf(T_USER_CMD_PARSED_VALUE *p_parse_value)
{
    uint8_t conn_id = p_parse_value->dw_param[0];
    T_GAP_CFM_CAUSE confirm = GAP_CFM_CAUSE_ACCEPT;
    T_GAP_CAUSE cause;
    if (p_parse_value->dw_param[1] == 0)
    {
        confirm = GAP_CFM_CAUSE_REJECT;
    }
    cause = le_bond_user_confirm(conn_id, confirm);
    return (T_USER_CMD_PARSE_RESULT)cause;
}

/**
 * @brief Input passkey when show GAP_MSG_LE_BOND_PASSKEY_INPUT
 *
 * <b>Command table define</b>
 * \code{.c}
    {
        "authkey",
        "authkey [conn_id] [passkey]\n\r",
        "Input passkey when show GAP_MSG_LE_BOND_PASSKEY_INPUT\r\n\
        [passkey]: 0 - 999999\r\n\
        sample: authkey 0 123456\n\r",
        cmd_authkey
    },
 * \endcode
 */
static T_USER_CMD_PARSE_RESULT cmd_authkey(T_USER_CMD_PARSED_VALUE *p_parse_value)
{
    uint8_t conn_id = p_parse_value->dw_param[0];
    uint32_t passcode = p_parse_value->dw_param[1];
    T_GAP_CAUSE cause;
    T_GAP_CFM_CAUSE confirm = GAP_CFM_CAUSE_ACCEPT;
    if (passcode > GAP_PASSCODE_MAX)
    {
        confirm = GAP_CFM_CAUSE_REJECT;
    }
    cause = le_bond_passkey_input_confirm(conn_id, passcode,
                                          confirm);
    return (T_USER_CMD_PARSE_RESULT)cause;
}

/**
 * @brief Clear all bonded devices information
 *
 * <b>Command table define</b>
 * \code{.c}
    {
        "bondclear",
        "bondclear\n\r",
        "Clear all bonded devices information\n\r",
        cmd_bondclear
    },
 * \endcode
 */
static T_USER_CMD_PARSE_RESULT cmd_bondclear(T_USER_CMD_PARSED_VALUE *p_parse_value)
{
    le_bond_clear_all_keys();
    return (RESULT_SUCESS);
}

/**
 * @brief Get all Bonded devices information
 *
 * <b>Command table define</b>
 * \code{.c}
    {
        "bondinfo",
        "bondinfo\n\r",
        "Get all Bonded devices information\n\r",
        cmd_bondinfo
    },
 * \endcode
 */
static T_USER_CMD_PARSE_RESULT cmd_bondinfo(T_USER_CMD_PARSED_VALUE *p_parse_value)
{
    uint8_t i;
    T_LE_KEY_ENTRY *p_entry;
    for (i = 0; i < bond_storage_num; i++)
    {
        p_entry = le_find_key_entry_by_idx(i);
        if (p_entry != NULL)
        {
            data_uart_print("bond_dev[%d]: bd 0x%02x%02x%02x%02x%02x%02x, addr_type %d, flags 0x%x\r\n",
                            p_entry->idx,
                            p_entry->remote_bd.addr[5],
                            p_entry->remote_bd.addr[4],
                            p_entry->remote_bd.addr[3],
                            p_entry->remote_bd.addr[2],
                            p_entry->remote_bd.addr[1],
                            p_entry->remote_bd.addr[0],
                            p_entry->remote_bd.remote_bd_type,
                            p_entry->flags);
        }
    }

    return (RESULT_SUCESS);
}
/************************** Central only *************************************/
/**
 * @brief Start scan
 *
 * <b>Command table define</b>
 * \code{.c}
    {
        "scan",
        "scan [filter_policy] [filter_duplicate]\n\r",
        "Start scan\r\n\
        [filter_policy]: 0-(any), 1-(whitelist), 2-(any RPA), 3-(whitelist RPA) \r\n\
        [filter_duplicate]: 0-(disable), 1-(enable) \n\r",
        cmd_scan
    },
 * \endcode
 */
static T_USER_CMD_PARSE_RESULT cmd_scan(T_USER_CMD_PARSED_VALUE *p_parse_value)
{
    T_GAP_CAUSE cause;
    uint8_t scan_filter_policy = GAP_SCAN_FILTER_ANY;
    uint8_t scan_filter_duplicate = GAP_SCAN_FILTER_DUPLICATE_ENABLE;

    if (p_parse_value->param_count > 0)
    {
        scan_filter_policy = p_parse_value->dw_param[0];
    }
    if (p_parse_value->param_count > 1)
    {
        scan_filter_duplicate = p_parse_value->dw_param[1];
    }

    le_scan_set_param(GAP_PARAM_SCAN_FILTER_POLICY, sizeof(scan_filter_policy),
                      &scan_filter_policy);
    le_scan_set_param(GAP_PARAM_SCAN_FILTER_DUPLICATES, sizeof(scan_filter_duplicate),
                      &scan_filter_duplicate);
    cause = le_scan_start();
    return (T_USER_CMD_PARSE_RESULT)cause;
}

/**
 * @brief Stop scan
 *
 * <b>Command table define</b>
 * \code{.c}
    {
        "stopscan",
        "stopscan\n\r",
        "Stop scan\n\r",
        cmd_stopscan
    },
 * \endcode
 */
static T_USER_CMD_PARSE_RESULT cmd_stopscan(T_USER_CMD_PARSED_VALUE *p_parse_value)
{
    T_GAP_CAUSE cause;
    cause = le_scan_stop();
    return (T_USER_CMD_PARSE_RESULT)cause;
}

/**
 * @brief Connect to remote device: use address
 *
 * <b>Command table define</b>
 * \code{.c}
    {
        "con",
        "con [BD0] [BD1] [BD2] [BD3] [BD4] [BD5] [addr_type]\n\r",
        "Connect to remote device: use address\r\n\
        [BD0] [BD1] [BD2] [BD3] [BD4] [BD5]: remote device address\r\n\
        [addr_type]: 0-(public), 1-(random)\r\n\
        sample: con x11 x22 x33 x44 x55 x66 0 \n\r",
        cmd_con
    },
 * \endcode
 */
static T_USER_CMD_PARSE_RESULT cmd_con(T_USER_CMD_PARSED_VALUE *p_parse_value)
{
    T_GAP_CAUSE cause;
    uint8_t addr[6] = {0};
    uint8_t addr_len;
    uint8_t addr_type = GAP_REMOTE_ADDR_LE_PUBLIC;
    T_GAP_LE_CONN_REQ_PARAM conn_req_param;

    conn_req_param.scan_interval = 0x10;
    conn_req_param.scan_window = 0x10;
    conn_req_param.conn_interval_min = 80;
    conn_req_param.conn_interval_max = 80;
    conn_req_param.conn_latency = 0;
    conn_req_param.supv_tout = 1000;
    conn_req_param.ce_len_min = 2 * (conn_req_param.conn_interval_min - 1);
    conn_req_param.ce_len_max = 2 * (conn_req_param.conn_interval_max - 1);
    le_set_conn_param(GAP_CONN_PARAM_1M, &conn_req_param);

    for (addr_len = 0; addr_len < GAP_BD_ADDR_LEN; addr_len++)
    {
        addr[addr_len] = p_parse_value->dw_param[GAP_BD_ADDR_LEN - addr_len - 1];
    }
    if (p_parse_value->param_count >= 7)
    {
        addr_type = p_parse_value->dw_param[6];
    }

    cause = le_connect(GAP_PHYS_CONN_INIT_1M_BIT, addr, (T_GAP_REMOTE_ADDR_TYPE)addr_type,
                       GAP_LOCAL_ADDR_LE_PUBLIC,
                       1000);

    return (T_USER_CMD_PARSE_RESULT)cause;
}

/************************** GATT client *************************************/
/**
 * @brief Send indication confimation
 *
 * <b>Command table define</b>
 * \code{.c}
    {
        "indconf",
        "indconf [conn_id]\n\r",
        "Send indication confimation\r\n\
        sample: indconf 0\n\r",
        cmd_indconf
    },
 * \endcode
 */
T_USER_CMD_PARSE_RESULT cmd_indconf(T_USER_CMD_PARSED_VALUE *p_parse_value)
{
    T_GAP_CAUSE ret;
    uint8_t conn_id = p_parse_value->dw_param[0];
    ret = gcs_attr_ind_confirm(conn_id);
    return (T_USER_CMD_PARSE_RESULT)ret;
}

/**
 * @brief Write data to service
 *
 * <b>Command table define</b>
 * \code{.c}
    {
        "write",
        "write [conn_id] [type] [handle] [length] [value0] [...]\n\r",
        "Write data to service\r\n\
        [type]: write type: 1-(write request), 2-(write command)\r\n\
        [handle]:attribute handle\r\n\
        [length]:value length\r\n\
        [value0]:overwrite the value0\r\n\
        sample: write 0 1 x17 2 02 00\r\n\
        sample: write 0 2 x19 10\n\r",
        cmd_write
    },
 * \endcode
 */
T_USER_CMD_PARSE_RESULT cmd_write(T_USER_CMD_PARSED_VALUE *p_parse_value)
{
    uint8_t conn_id = p_parse_value->dw_param[0];
    uint8_t write_type = p_parse_value->dw_param[1];
    uint16_t handle = p_parse_value->dw_param[2];
    uint16_t length = p_parse_value->dw_param[3];
    uint8_t data[512] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    if (p_parse_value->param_count > 4)
    {
        for (uint8_t i = 0; i < p_parse_value->param_count - 4; ++i)
        {
            data[i] = p_parse_value->dw_param[i + 4];
        }
    }

    T_GAP_CAUSE ret = gcs_attr_write(conn_id, (T_GATT_WRITE_TYPE)write_type, handle,
                                     length, data);
    return (T_USER_CMD_PARSE_RESULT)ret;
}

/**
 * @brief Discover all primary services
 *
 * <b>Command table define</b>
 * \code{.c}
    {
        "gsrvdis",
        "gsrvdis [conn_id]\n\r",
        "Discover all primary services\r\n\
        sample: gsrvdis 0\n\r",
        cmd_gsrvdis
    },
 * \endcode
 */
T_USER_CMD_PARSE_RESULT cmd_gsrvdis(T_USER_CMD_PARSED_VALUE *p_parse_value)
{
    uint8_t conn_id = p_parse_value->dw_param[0];
    T_GAP_CAUSE cause = gcs_all_primary_srv_discovery(conn_id);

    return (T_USER_CMD_PARSE_RESULT)cause;
}

/**
 * @brief Discover services by uuid
 *
 * <b>Command table define</b>
 * \code{.c}
    {
        "srvuuid",
        "srvdis [conn_id] [type] [uuid]\n\r",
        "Discover services by uuid\r\n\
        [type]:UUID type: 0-(uuid16), 1-(uuid128)\r\n\
        [uuid]:Sevice uuid\r\n\
        sample(uuid128): srvuuid 0 1 0x00006287 0x3c17d293 0x8e4814fe 0x2e4da212\r\n\
        sample(uuid16): srvuuid 0 0 0x1801\n\r",
        cmd_srvuuid
    },
 * \endcode
 */
T_USER_CMD_PARSE_RESULT cmd_srvuuid(T_USER_CMD_PARSED_VALUE *p_parse_value)
{
    uint8_t conn_id = p_parse_value->dw_param[0];
    T_GAP_CAUSE cause = GAP_CAUSE_SUCCESS;

    if (p_parse_value->dw_param[1] == 0)
    {
        uint16_t uuid16 = p_parse_value->dw_param[2];
        cause = gcs_by_uuid_srv_discovery(conn_id, uuid16);
    }
    else if (p_parse_value->dw_param[1] == 1)
    {
        uint32_t u0 = p_parse_value->dw_param[2];
        uint32_t u1 = p_parse_value->dw_param[3];
        uint32_t u2 = p_parse_value->dw_param[4];
        uint32_t u3 = p_parse_value->dw_param[5];
        uint8_t uuid128[16] =
        {
            (uint8_t)u3, (uint8_t)(u3 >> 8), (uint8_t)(u3 >> 16), (uint8_t)(u3 >> 24),
            (uint8_t)u2, (uint8_t)(u2 >> 8), (uint8_t)(u2 >> 16), (uint8_t)(u2 >> 24),
            (uint8_t)u1, (uint8_t)(u1 >> 8), (uint8_t)(u1 >> 16), (uint8_t)(u1 >> 24),
            (uint8_t)u0, (uint8_t)(u0 >> 8), (uint8_t)(u0 >> 16), (uint8_t)(u0 >> 24)
        };
        cause = gcs_by_uuid128_srv_discovery(conn_id, uuid128);
    }

    return (T_USER_CMD_PARSE_RESULT)cause;
}

/**
 * @brief Discover characteristic
 *
 * <b>Command table define</b>
 * \code{.c}
    {
        "chardis",
        "chardis [conn_id] [start handle] [end handle]\n\r",
        "Discover characteristic\r\n\
        [start handle]:Start handle\r\n\
        [end handle]:End handle\r\n\
        sample: chardis 0 xc xff\n\r",
        cmd_chardis
    },
 * \endcode
 */
T_USER_CMD_PARSE_RESULT cmd_chardis(T_USER_CMD_PARSED_VALUE *p_parse_value)
{
    uint8_t conn_id = p_parse_value->dw_param[0];
    uint16_t start_handle = p_parse_value->dw_param[1];
    uint16_t end_handle = p_parse_value->dw_param[2];

    T_GAP_CAUSE cause = gcs_all_char_discovery(conn_id, start_handle, end_handle);

    return (T_USER_CMD_PARSE_RESULT)cause;
}

/**
 * @brief Discover characteristic by uuid
 *
 * <b>Command table define</b>
 * \code{.c}
    {
        "charuuid",
        "charuuid [conn_id] [start handle] [end handle] [type] [uuid]\n\r",
        "Discover characteristic by uuid\r\n\
        [start handle]:Start handle\r\n\
        [end handle]:End handle\r\n\
        [type]:UUID type: 0-(uuid16), 1-(uuid128)\r\n\
        [uuid]:Sevice uuid\r\n\
        sample(uuid128): charuuid 0 x1 xffff 1 x00006387 x3c17d293 x8e4814fe x2e4da212\r\n\
        sample(uuid16): charuuid 0 x1 xffff 0 xb001\n\r",
        cmd_charuuid
    },
 * \endcode
 */
T_USER_CMD_PARSE_RESULT cmd_charuuid(T_USER_CMD_PARSED_VALUE *p_parse_value)
{
    uint8_t conn_id = p_parse_value->dw_param[0];
    uint16_t start_handle = p_parse_value->dw_param[1];
    uint16_t end_handle = p_parse_value->dw_param[2];
    T_GAP_CAUSE cause = GAP_CAUSE_SUCCESS;

    if (p_parse_value->dw_param[3] == 0)
    {
        uint16_t uuid16 = p_parse_value->dw_param[4];
        T_GAP_CAUSE cause = gcs_by_uuid_char_discovery(conn_id, start_handle, end_handle,
                                                       uuid16);
    }
    else if (p_parse_value->dw_param[3] == 1)
    {
        uint32_t u0 = p_parse_value->dw_param[4];
        uint32_t u1 = p_parse_value->dw_param[5];
        uint32_t u2 = p_parse_value->dw_param[6];
        uint32_t u3 = p_parse_value->dw_param[7];
        uint8_t uuid128[16] =
        {
            (uint8_t)u3, (uint8_t)(u3 >> 8), (uint8_t)(u3 >> 16), (uint8_t)(u3 >> 24),
            (uint8_t)u2, (uint8_t)(u2 >> 8), (uint8_t)(u2 >> 16), (uint8_t)(u2 >> 24),
            (uint8_t)u1, (uint8_t)(u1 >> 8), (uint8_t)(u1 >> 16), (uint8_t)(u1 >> 24),
            (uint8_t)u0, (uint8_t)(u0 >> 8), (uint8_t)(u0 >> 16), (uint8_t)(u0 >> 24)
        };
        T_GAP_CAUSE cause = gcs_by_uuid128_char_discovery(conn_id, start_handle,
                                                          end_handle,
                                                          uuid128);
    }

    return (T_USER_CMD_PARSE_RESULT)cause;
}

/**
 * @brief Discover characteristic descriptor
 *
 * <b>Command table define</b>
 * \code{.c}
    {
        "charddis",
        "charddis [conn_id] [start handle] [end handle]\n\r",
        "Discover characteristic descriptor\r\n\
        [start handle]:Start handle\r\n\
        [end handle]:End handle\r\n\
        sample: charddis 0 xc x14\n\r",
        cmd_charddis
    },
 * \endcode
 */
T_USER_CMD_PARSE_RESULT cmd_charddis(T_USER_CMD_PARSED_VALUE *p_parse_value)
{
    uint8_t conn_id = p_parse_value->dw_param[0];
    uint16_t start_handle = p_parse_value->dw_param[1];
    uint16_t end_handle = p_parse_value->dw_param[2];

    T_GAP_CAUSE cause = gcs_all_char_descriptor_discovery(conn_id, start_handle,
                                                          end_handle);

    return (T_USER_CMD_PARSE_RESULT)cause;
}

/**
 * @brief Read characteristic
 *
 * <b>Command table define</b>
 * \code{.c}
    {
        "read",
        "read [conn_id] [handle]\n\r",
        "Read characteristic\r\n\
        [handle]:attribute handle\r\n\
        sample: read 0 x1b\n\r",
        cmd_read
    },
 * \endcode
 */
T_USER_CMD_PARSE_RESULT cmd_read(T_USER_CMD_PARSED_VALUE *p_parse_value)
{
    uint8_t conn_id = p_parse_value->dw_param[0];
    uint16_t handle = p_parse_value->dw_param[1];
    T_GAP_CAUSE cause = GAP_CAUSE_SUCCESS;

    if (p_parse_value->param_count <= 2)
    {
        cause = gcs_attr_read(conn_id, handle);
    }

    return (T_USER_CMD_PARSE_RESULT)cause;
}

/**
 * @brief Read characterristic value by uuid
 *
 * <b>Command table define</b>
 * \code{.c}
    {
        "readu",
        "readu [conn_id] [start handle] [end handle] [type] [uuid]\n\r",
        "Read characterristic value by uuid\r\n\
        [start handle]:Start handle\r\n\
        [end handle]:End handle\r\n\
        [type]:UUID type: 0-(uuid16), 1-(uuid128)\r\n\
        [uuid]:Sevice uuid\r\n\
        sample(uuid128): readu 0 x1 xffff 1 x00006387 x3c17d293 x8e4814fe x2e4da212\r\n\
        sample(uuid16): readu 0 x1 xffff 0 0xb001\n\r",
        cmd_readu
    },
 * \endcode
 */
T_USER_CMD_PARSE_RESULT cmd_readu(T_USER_CMD_PARSED_VALUE *p_parse_value)
{
    uint8_t conn_id = p_parse_value->dw_param[0];
    uint16_t start_handle = p_parse_value->dw_param[1];
    uint16_t end_handle = p_parse_value->dw_param[2];
    T_GAP_CAUSE cause = GAP_CAUSE_SUCCESS;

    if (p_parse_value->dw_param[3] == 0)
    {
        uint16_t uuid16 = p_parse_value->dw_param[4];
        cause = gcs_attr_read_using_uuid16(conn_id, start_handle, end_handle, uuid16);
    }
    else if (p_parse_value->dw_param[3] == 1)
    {
        uint32_t u0 = p_parse_value->dw_param[4];
        uint32_t u1 = p_parse_value->dw_param[5];
        uint32_t u2 = p_parse_value->dw_param[6];
        uint32_t u3 = p_parse_value->dw_param[7];
        uint8_t uuid128[16] =
        {
            (uint8_t)u3, (uint8_t)(u3 >> 8), (uint8_t)(u3 >> 16), (uint8_t)(u3 >> 24),
            (uint8_t)u2, (uint8_t)(u2 >> 8), (uint8_t)(u2 >> 16), (uint8_t)(u2 >> 24),
            (uint8_t)u1, (uint8_t)(u1 >> 8), (uint8_t)(u1 >> 16), (uint8_t)(u1 >> 24),
            (uint8_t)u0, (uint8_t)(u0 >> 8), (uint8_t)(u0 >> 16), (uint8_t)(u0 >> 24)
        };
        cause = gcs_attr_read_using_uuid128(conn_id, start_handle, end_handle, uuid128);
    }

    return (T_USER_CMD_PARSE_RESULT)cause;
}

/** @brief  User command table */
const T_USER_CMD_TABLE_ENTRY user_cmd_table[] =
{
    /************************** Common cmd *************************************/
    {
        "conupdreq",
        "conupdreq [conn_id] [interval_min] [interval_max] [latency] [supervision_timeout]\n\r",
        "LE connection param update request\r\n\
        sample: conupdreq 0 0x30 0x40 0 500\n\r",
        cmd_conupdreq
    },
    {
        "showcon",
        "showcon\n\r",
        "Show all devices connecting status\n\r",
        cmd_showcon
    },
    {
        "disc",
        "disc [conn_id]\n\r",
        "Disconnect to remote device\n\r",
        cmd_disc
    },
    {
        "authmode",
        "authmode [auth_flags] [io_cap] [sec_enable] [oob_enable]\n\r",
        "Config authentication mode\r\n\
        [auth_flags]:authentication req bit field: bit0-(bonding), bit2-(MITM), bit3-(SC)\r\n\
        [io_cap]:set io Capabilities: 0-(display only), 1-(display yes/no), 2-(keyboard noly), 3-(no IO), 4-(keyboard display)\r\n\
        [sec_enable]:Start smp pairing procedure when connected: 0-(disable), 1-(enable)\r\n\
        [oob_enable]:Enable oob flag: 0-(disable), 1-(enable)\r\n\
        sample: authmode 0x5 2 1 0\n\r",
        cmd_authmode
    },
    {
        "sauth",
        "sauth [conn_id]\n\r",
        "Send authentication request\n\r",
        cmd_sauth
    },
    {
        "userconf",
        "userconf [conn_id] [conf]\n\r",
        "Send user confirmation when show GAP_MSG_LE_BOND_USER_CONFIRMATION\r\n\
        [conf]: 0-(Reject), 1-(Accept)\r\n\
        sample: userconf 0 1\n\r",
        cmd_userconf
    },
    {
        "authkey",
        "authkey [conn_id] [passkey]\n\r",
        "Input passkey when show GAP_MSG_LE_BOND_PASSKEY_INPUT\r\n\
        [passkey]: 0 - 999999\r\n\
        sample: authkey 0 123456\n\r",
        cmd_authkey
    },
    {
        "bondinfo",
        "bondinfo\n\r",
        "Get all Bonded devices information\n\r",
        cmd_bondinfo
    },
    {
        "bondclear",
        "bondclear\n\r",
        "Clear all bonded devices information\n\r",
        cmd_bondclear
    },
    /************************** Central only *************************************/
    {
        "scan",
        "scan [filter_policy] [filter_duplicate]\n\r",
        "Start scan\r\n\
        [filter_policy]: 0-(any), 1-(whitelist), 2-(any RPA), 3-(whitelist RPA) \r\n\
        [filter_duplicate]: 0-(disable), 1-(enable) \n\r",
        cmd_scan
    },
    {
        "stopscan",
        "stopscan\n\r",
        "Stop scan\n\r",
        cmd_stopscan
    },
    {
        "con",
        "con [BD0] [BD1] [BD2] [BD3] [BD4] [BD5] [addr_type]\n\r",
        "Connect to remote device: use address\r\n\
        [BD0] [BD1] [BD2] [BD3] [BD4] [BD5]: remote device address\r\n\
        [addr_type]: 0-(public), 1-(random)\r\n\
        sample: con x11 x22 x33 x44 x55 x66 0 \n\r",
        cmd_con
    },
    /************************** GATT client *************************************/
    {
        "indconf",
        "indconf [conn_id]\n\r",
        "Send indication confimation\r\n\
        sample: indconf 0\n\r",
        cmd_indconf
    },
    {
        "write",
        "write [conn_id] [type] [handle] [length] [value0] [...]\n\r",
        "Write data to service\r\n\
        [type]: write type: 1-(write request), 2-(write command)\r\n\
        [handle]:attribute handle\r\n\
        [length]:value length\r\n\
        [value0]:overwrite the value0\r\n\
        sample: write 0 1 x17 2 02 00\r\n\
        sample: write 0 2 x19 10\n\r",
        cmd_write
    },
    {
        "gsrvdis",
        "gsrvdis [conn_id]\n\r",
        "Discover all primary services\r\n\
        sample: gsrvdis 0\n\r",
        cmd_gsrvdis
    },
    {
        "srvuuid",
        "srvdis [conn_id] [type] [uuid]\n\r",
        "Discover services by uuid\r\n\
        [type]:UUID type: 0-(uuid16), 1-(uuid128)\r\n\
        [uuid]:Sevice uuid\r\n\
        sample(uuid128): srvuuid 0 1 0x00006287 0x3c17d293 0x8e4814fe 0x2e4da212\r\n\
        sample(uuid16): srvuuid 0 0 0x1801\n\r",
        cmd_srvuuid
    },
    {
        "chardis",
        "chardis [conn_id] [start handle] [end handle]\n\r",
        "Discover characteristic\r\n\
        [start handle]:Start handle\r\n\
        [end handle]:End handle\r\n\
        sample: chardis 0 xc xff\n\r",
        cmd_chardis
    },
    {
        "charuuid",
        "charuuid [conn_id] [start handle] [end handle] [type] [uuid]\n\r",
        "Discover characteristic by uuid\r\n\
        [start handle]:Start handle\r\n\
        [end handle]:End handle\r\n\
        [type]:UUID type: 0-(uuid16), 1-(uuid128)\r\n\
        [uuid]:Sevice uuid\r\n\
        sample(uuid128): charuuid 0 x1 xffff 1 x00006387 x3c17d293 x8e4814fe x2e4da212\r\n\
        sample(uuid16): charuuid 0 x1 xffff 0 xb001\n\r",
        cmd_charuuid
    },
    {
        "charddis",
        "charddis [conn_id] [start handle] [end handle]\n\r",
        "Discover characteristic descriptor\r\n\
        [start handle]:Start handle\r\n\
        [end handle]:End handle\r\n\
        sample: charddis 0 xc x14\n\r",
        cmd_charddis
    },
    {
        "read",
        "read [conn_id] [handle]\n\r",
        "Read characteristic\r\n\
        [handle]:attribute handle\r\n\
        sample: read 0 x1b\n\r",
        cmd_read
    },
    {
        "readu",
        "readu [conn_id] [start handle] [end handle] [type] [uuid]\n\r",
        "Read characterristic value by uuid\r\n\
        [start handle]:Start handle\r\n\
        [end handle]:End handle\r\n\
        [type]:UUID type: 0-(uuid16), 1-(uuid128)\r\n\
        [uuid]:Sevice uuid\r\n\
        sample(uuid128): readu 0 x1 xffff 1 x00006387 x3c17d293 x8e4814fe x2e4da212\r\n\
        sample(uuid16): readu 0 x1 xffff 0 0xb001\n\r",
        cmd_readu
    },
    /* MUST be at the end: */
    {
        0,
        0,
        0,
        0
    }
};
/** @} */ /* End of group CENTRAL_CLIENT_CMD */
#endif
