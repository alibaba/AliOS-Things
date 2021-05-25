/**
*****************************************************************************************
*     Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    gap_bond_le.h
  * @brief   This file contains all the functions prototypes for the GAP bond and pairing
  *          related functions.
  * @details
  * @author  ranhui
  * @date    2016-02-18
  * @version v1.0
  * *************************************************************************************
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef GAP_BOND_LE_H
#define GAP_BOND_LE_H

#ifdef __cplusplus
extern "C"
{
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <bt_flags.h>
#include <gap_le.h>


/** @defgroup GAP_LE_Bond_Manager GAP LE Bond Manager
  * @brief GAP LE Bond Manager
  * @{
  */
/*============================================================================*
 *                         Types
 *============================================================================*/
/** @defgroup GAP_LE_BOND_MANAGER_Exported_Types GAP LE Bond Manager Exported Types
  * @{
  */

/** @brief LE bond parameter types */
typedef enum
{
#if F_BT_LE_SMP_OOB_SUPPORT
    GAP_PARAM_BOND_OOB_DATA             = 0x210,//!< OOB Data. Read/Write. size uint8_t[16]. Default is all 0's.
#endif
    GAP_PARAM_BOND_FIXED_PASSKEY        = 0x211,//!< The fix passcode for MITM protection. Read/Write. size is uint32_t. Range is 0 - 999,999. Default is 0.
    GAP_PARAM_BOND_FIXED_PASSKEY_ENABLE = 0x212,//!< The fix passcode available for pairing. Read/Write. size is uint8_t. Default is 0(disabled).
    GAP_PARAM_BOND_SEC_REQ_ENABLE       = 0x213,//!< Send smp security request when connected. Read/Write. size is uint8_t. Default is 0(disabled).
    GAP_PARAM_BOND_SEC_REQ_REQUIREMENT  = 0x214,//!< Security request requirements. Read/Write. size is uint8_t. Default is GAP_AUTHEN_BIT_BONDING_FLAG (@ref BOND_MITM_DEFINES)
    GAP_PARAM_BOND_MIN_KEY_SIZE         = 0x215,//!< Minium LTK size required. Read/Write. size is uint8_t.
#if F_BT_LE_LOCAL_IRK_SETTING_SUPPORT
    GAP_PARAM_BOND_GEN_LOCAL_IRK_AUTO   = 0x216,//!< Auto generate local IRK. Write only. size is uint8_t. Default is 0(disabled).
    GAP_PARAM_BOND_SET_LOCAL_IRK        = 0x217,//!< Set local IRK. Write only. size is uint8_t. Default is all zero.
#endif
    GAP_PARAM_BOND_KEY_MANAGER          = 0x218,//!< Key manager. Write only. size is uint8_t. Default is 0(disabled).
    GAP_PARAM_BOND_SIGN_KEY_FLAG        = 0x219,//!< Sign key flag configure. Write only. size is uint8_t. Default is 0(disabled).
} T_LE_BOND_PARAM_TYPE;

/** End of GAP_LE_BOND_MANAGER_Exported_Types
* @}
*/


/*============================================================================*
 *                         Functions
 *============================================================================*/
/** @defgroup GAP_LE_BONDMGR_Exported_Functions GAP LE Bond Manager Exported Functions
  * @brief
  * @{
  */

/**
 * @brief   Set a GAP Bond Manager Parameter.
 *
 *          NOTE: You can call this function with a bond parameter type and it will set the
 *          bond parameter. Bond parameters are defined in @ref T_LE_BOND_PARAM_TYPE.
 *          if the "len" field sets to the size of a "uint16_t" ,the
 *          "p_value" field must point to a data with type of "uint16_t".
 *
 * @param[in]   param   Bond parameter type: @ref T_LE_BOND_PARAM_TYPE
 * @param[in]   len     Length of data to write
 * @param[in]   p_value Pointer to data to write.  This is dependent on
 *                      the parameter type and will be cast to the appropriate
 *                      data type (For example: if data type param is uint16, p_value will be cast to
 *                      pointer of uint16_t).
 *
 * @return Set result
 * @retval GAP_CAUSE_SUCCESS  Operation success.
 * @retval Others  Operation failure. No connection.
 *
 * <b>Example usage</b>
 * \code{.c}
    void app_le_gap_init(void)
    {
        ...
        uint8_t  auth_use_fix_passkey = false;
        uint32_t auth_fix_passkey = 0;
        uint8_t  auth_sec_req_enable = false;
        uint16_t auth_sec_req_flags = GAP_AUTHEN_BIT_NONE;

        le_bond_set_param(GAP_PARAM_BOND_FIXED_PASSKEY, sizeof(auth_fix_passkey), &auth_fix_passkey);
        le_bond_set_param(GAP_PARAM_BOND_FIXED_PASSKEY_ENABLE, sizeof(auth_use_fix_passkey),
                         &auth_use_fix_passkey);
        le_bond_set_param(GAP_PARAM_BOND_SEC_REQ_ENABLE, sizeof(auth_sec_req_enable), &auth_sec_req_enable);
        le_bond_set_param(GAP_PARAM_BOND_SEC_REQ_REQUIREMENT, sizeof(auth_sec_req_flags),
                         &auth_sec_req_flags);
        ....
    }
 * \endcode
 */
T_GAP_CAUSE le_bond_set_param(T_LE_BOND_PARAM_TYPE param, uint8_t len, void *p_value);

/**
 * @brief   Get a GAP Bond Manager Parameter.
 *
 *          NOTE: You can call this function with a bond parameter type and it will get a
 *          bond parameter. Bond parameters are defined in @ref T_LE_BOND_PARAM_TYPE. Also, the
 *          "p_value" field must point to a "uint16".
 *
 * @param[in]   param   Profile parameter ID: @ref T_LE_BOND_PARAM_TYPE
 * @param[in,out]  p_value pointer to location to get the value. This is dependent on
 *                      the parameter type and will be cast to the appropriate
 *                      data type (For example: if data type param is uint16, p_value will be cast to
 *                      pointer of uint16_t).
 *
 * @return Get result
 * @retval GAP_CAUSE_SUCCESS Get parameter success.
 * @retval other Get parameter failed.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test(void)
    {
    uint8_t seq_req;
    le_bond_get_param(GAP_PARAM_BOND_SEC_REQ_ENABLE, &seq_req);
    }
 * \endcode
 */
T_GAP_CAUSE le_bond_get_param(T_LE_BOND_PARAM_TYPE param, void *p_value);

/**
 * @brief   Start authentication the link.
 *
 *          NOTE: If the link has not been bonded, it will trigger pairing procedure, otherwise
 *          it will trigger the link encryption procedure.
 * @param[in] conn_id Connection ID.
 * @return Operation result.
 * @retval GAP_CAUSE_SUCCESS Operation success.
 * @retval GAP_CAUSE_NON_CONN Operation failure. No connection.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test(void)
    {
       uint8_t conn_id = 0x01;
       le_bond_pair(conn_id);
    }

    void app_handle_authen_state_evt(uint8_t conn_id, uint8_t new_state, uint16_t cause)
    {
        APP_PRINT_INFO2("app_handle_authen_state_evt:conn_id %d, cause 0x%x", conn_id, cause);

        switch (new_state)
        {
        case GAP_AUTHEN_STATE_STARTED:
            {
                APP_PRINT_INFO0("app_handle_authen_state_evt: GAP_AUTHEN_STATE_STARTED");
            }
            break;

        case GAP_AUTHEN_STATE_COMPLETE:
            {
                if (cause == GAP_SUCCESS)
                {
                    APP_PRINT_INFO0("app_handle_authen_state_evt: GAP_AUTHEN_STATE_COMPLETE pair success");

                }
                else
                {
                    APP_PRINT_INFO0("app_handle_authen_state_evt: GAP_AUTHEN_STATE_COMPLETE pair failed");
                }
            }
            break;

        default:
            {
                APP_PRINT_ERROR1("app_handle_authen_state_evt: unknown newstate %d", new_state);
            }
            break;
        }
    }
 * \endcode
 */
T_GAP_CAUSE le_bond_pair(uint8_t conn_id);

/**
 * @brief   Get the display key information
 *
 * @param[in] conn_id   Connection ID.
 * @param[in,out] p_key    Passkey valueto be displayed as a 6 digit decimal number.
 * @return Operation result.
 * @retval GAP_CAUSE_SUCCESS    Operation success.
 * @retval GAP_CAUSE_NON_CONN   Operation failure. No connection.
 *
 * <b>Example usage</b>
 * \code{.c}
    void app_handle_gap_msg(T_IO_MSG *p_io_msg)
    {
      T_LE_GAP_MSG bt_msg;
      uint8_t conn_id;

      memcpy(&bt_msg, &p_io_msg->u.param, sizeof(p_io_msg->u.param));

      switch (p_io_msg->subtype)
      {
      ...
      case GAP_MSG_LE_BOND_PASSKEY_DISPLAY:
          {
            uint32_t display_value = 0;
            conn_id = gap_msg.msg_data.gap_bond_passkey_display.conn_id;
            le_bond_get_display_key(conn_id, &display_value);
            APP_PRINT_INFO1("GAP_MSG_LE_BOND_PASSKEY_DISPLAY:passkey %d", display_value);
            le_bond_passkey_display_confirm(conn_id, GAP_CFM_CAUSE_ACCEPT);
          }
          break;
      ...
      }
      ...
    }
 * \endcode
 */
T_GAP_CAUSE le_bond_get_display_key(uint8_t conn_id, uint32_t *p_key);

/**
 * @brief   Send passkey to gap bond manager when pairing with passkey entry,
 *          and local should input passkey.
 *
 * @param[in] conn_id Connection ID.
 * @param[in] passcode  Result of Keyboard input.
 * @param[in] cause     User confirm result.
 * @return Operation result.
 * @retval GAP_CAUSE_SUCCESS    Operation success.
 * @retval GAP_CAUSE_NON_CONN   Operation failure. No connection.
 *
 * <b>Example usage</b>
 * \code{.c}
    void app_handle_gap_msg(T_IO_MSG *p_io_msg)
    {
      T_LE_GAP_MSG bt_msg;
      uint8_t conn_id;

      memcpy(&bt_msg, &p_io_msg->u.param, sizeof(p_io_msg->u.param));

      switch (p_io_msg->subtype)
      {
      ...
      case GAP_MSG_LE_BOND_PASSKEY_INPUT:
          {
              conn_id = bt_msg.msg_data.gap_bond_passkey_input.conn_id;
              APP_PRINT_INFO0("GAP_MSG_LE_BOND_PASSKEY_INPUT");
              uint32_t passKey = 888888;
              le_bond_passkey_input_confirm(conn_id, passKey, GAP_CFM_CAUSE_ACCEPT);
      }
      break;
      ...
      }
      ...
    }
 * \endcode
 */
T_GAP_CAUSE le_bond_passkey_input_confirm(uint8_t conn_id, uint32_t passcode,
                                          T_GAP_CFM_CAUSE cause);

/**
 * @brief   Send oob data to gap bond manager when pairing with out of bond,
 *          and local should input oob data.
 * @param[in] conn_id connection ID.
 * @param[in] cause   User confirm result.
 * @return Operation result.
 * @retval GAP_CAUSE_SUCCESS    Operation success.
 * @retval GAP_CAUSE_NON_CONN   Operation failure. No connection.
 *
 * <b>Example usage</b>
 * \code{.c}
    void app_handle_gap_msg(T_IO_MSG *p_io_msg)
    {
      T_LE_GAP_MSG bt_msg;
      uint8_t conn_id;

      memcpy(&bt_msg, &p_io_msg->u.param, sizeof(p_io_msg->u.param));

      switch (p_io_msg->subtype)
      {
      ...
      case GAP_MSG_LE_BOND_OOB_INPUT:
          {
              conn_id = bt_msg.msg_data.gap_bond_oob_input.conn_id;
              APP_PRINT_INFO0("GAP_MSG_LE_BOND_OOB_INPUT");
              uint8_t ooBData[GAP_OOB_LEN] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
              le_bond_set_param(GAP_PARAM_BOND_OOB_DATA, GAP_OOB_LEN, ooBData);
              le_bond_oob_input_confirm(conn_id, GAP_CFM_CAUSE_ACCEPT);
          }
          break;
      ...
      }
      ...
    }
 * \endcode
 */
#if F_BT_LE_SMP_OOB_SUPPORT
T_GAP_CAUSE le_bond_oob_input_confirm(uint8_t conn_id, T_GAP_CFM_CAUSE cause);
#endif
/**
 * @brief   Send user confirmation request to confirm result to upper stack when
             pairing with jsut work.
 * @param[in] conn_id   Connection ID.
 * @param[in] cause     User confirm result.
 * @return Operation result.
 * @retval GAP_CAUSE_SUCCESS    Operation success.
 * @retval GAP_CAUSE_NON_CONN   Operation failure. No connection.
 *
 * <b>Example usage</b>
 * \code{.c}
    void app_handle_gap_msg(T_IO_MSG *p_io_msg)
    {
        T_LE_GAP_MSG bt_msg;
        uint8_t conn_id;

        memcpy(&bt_msg, &p_io_msg->u.param, sizeof(p_io_msg->u.param));

        switch (p_io_msg->subtype)
        {
        ...
        case GAP_MSG_LE_BOND_JUST_WORK:
        {
            conn_id = gap_msg.msg_data.gap_bond_just_work_conf.conn_id;
            le_bond_just_work_confirm(conn_id, GAP_CFM_CAUSE_ACCEPT);
            APP_PRINT_INFO0("GAP_MSG_LE_BOND_JUST_WORK");
        }
        break;
        ...
        }
        ...
    }
 * \endcode
 */
T_GAP_CAUSE le_bond_just_work_confirm(uint8_t conn_id, T_GAP_CFM_CAUSE cause);


/**
 * @brief   Send user confirmation request to confirm result to upper stack  when
             pairing with passkey entry, and local shall display a passkey.
 * @param[in] conn_id   Connection ID.
 * @param[in] cause     User confirm result.
 * @return Operation result.
 * @retval GAP_CAUSE_SUCCESS    Operation success.
 * @retval GAP_CAUSE_NON_CONN   Operation failure. No connection.
 *
 * <b>Example usage</b>
 * \code{.c}
    void app_handle_gap_msg(T_IO_MSG *p_io_msg)
    {
        T_LE_GAP_MSG bt_msg;
        uint8_t conn_id;

        memcpy(&bt_msg, &p_io_msg->u.param, sizeof(p_io_msg->u.param));

        switch (p_io_msg->subtype)
        {
        ...
        case GAP_MSG_LE_BOND_PASSKEY_DISPLAY:
        {
        uint32_t display_value = 0;
        conn_id = gap_msg.msg_data.gap_bond_passkey_display.conn_id;
        le_bond_get_display_key(conn_id, &display_value);
        APP_PRINT_INFO1("GAP_MSG_LE_BOND_PASSKEY_DISPLAY:passkey %d", display_value);
        le_bond_passkey_display_confirm(conn_id, GAP_CFM_CAUSE_ACCEPT);
        }
        break;
        ...
        }
        ...
    }
 * \endcode
 */
T_GAP_CAUSE le_bond_passkey_display_confirm(uint8_t conn_id, T_GAP_CFM_CAUSE cause);

/**
 * @brief   Send user confirmation request to confirm result to upper stack
 * @param[in] conn_id   Connection ID.
 * @param[in] cause     User confirm result.
 * @return Operation result.
 * @retval GAP_CAUSE_SUCCESS    Operation success.
 * @retval GAP_CAUSE_NON_CONN   Operation failure. No connection.
 *
 * <b>Example usage</b>
 * \code{.c}
    void app_handle_gap_msg(T_IO_MSG *p_io_msg)
    {
        T_LE_GAP_MSG bt_msg;
        uint8_t conn_id;

        memcpy(&bt_msg, &p_io_msg->u.param, sizeof(p_io_msg->u.param));

        switch (p_io_msg->subtype)
        {
        ...
        case GAP_MSG_LE_BOND_USER_CONFIRMATION:
          {
            uint32_t display_value = 0;
            conn_id = gap_msg.msg_data.gap_bond_user_conf.conn_id;
            le_bond_get_display_key(conn_id, &display_value);
            APP_PRINT_INFO1("GAP_MSG_LE_BOND_USER_CONFIRMATION: passkey %d", display_value);
            le_bond_user_confirm(conn_id, GAP_CFM_CAUSE_ACCEPT);
          }
          break;
        ...
        }
        ...
    }
 * \endcode
 */
#if F_BT_LE_4_2_SC_SUPPORT
T_GAP_CAUSE le_bond_user_confirm(uint8_t conn_id, T_GAP_CFM_CAUSE cause);
#endif

/**
 * @brief   Set local key distribution field
 * @param[in] init_dist   Initiator key distribution.
 * @param[in] rsp_dist    Responder key distribution.
 * @return Operation result.
 * @retval GAP_CAUSE_SUCCESS    Operation success.
 * @retval Others               Operation failure. No connection.
 *
 * <b>Example usage</b>
 * \code{.c}
    static T_USER_CMD_PARSE_RESULT cmd_setkeydis(T_USER_CMD_PARSED_VALUE *p_parse_value)
    {
        uint8_t init_dist = p_parse_value->dw_param[0];
        uint8_t rsp_dist = p_parse_value->dw_param[1];
        le_bond_cfg_local_key_distribute(init_dist, rsp_dist);
        return (RESULT_SUCESS);
    }
 * \endcode
 */
T_GAP_CAUSE le_bond_cfg_local_key_distribute(uint8_t init_dist, uint8_t rsp_dist);

/**
 * @brief   Erase all link keys of bonded devices
 * @return  void
 *
 * <b>Example usage</b>
 * \code{.c}
    T_APP_RESULT app_gap_callback(uint8_t cb_type, void *p_cb_data)
    {
        T_APP_RESULT result = APP_RESULT_SUCCESS;
        T_LE_CB_DATA cb_data;
        memcpy(&cb_data, p_cb_data, sizeof(T_LE_CB_DATA));
        APP_PRINT_TRACE1("app_gap_callback: cb_type = %d", cb_type);
        switch (cb_type)
        {
        ...
        case GAP_MSG_LE_BOND_MODIFY_INFO:
           app_handle_bond_modify_msg(cb_data.p_le_bond_modify_info->type,
                                      cb_data.p_le_bond_modify_info->p_entry);
           break;
        ...
        }
    }

    void app_handle_bond_modify_msg(T_LE_BOND_MODIFY_TYPE type, T_LE_KEY_ENTRY *p_entry)
    {
        APP_PRINT_INFO1("app_handle_bond_modify_msg  GAP_MSG_LE_BOND_MODIFY_INFO:type=0x%x",
                        type);
        if (type == LE_BOND_CLEAR)
        {
            privacy_add_pending_resolving_list(GAP_RESOLV_LIST_OP_CLEAR, GAP_IDENT_ADDR_PUBLIC,
                                               NULL);
            privacy_handle_pending_resolving_list();
        }
        else if (type == LE_BOND_ADD)
        {
            if (p_entry->flags & LE_KEY_STORE_REMOTE_IRK_BIT)
            {
                privacy_add_pending_resolving_list(GAP_RESOLV_LIST_OP_ADD,
                                                   (T_GAP_IDENT_ADDR_TYPE)p_entry->resolved_remote_bd.remote_bd_type,
                                                   p_entry->resolved_remote_bd.addr);
            }
            else if (p_entry->flags & LE_KEY_STORE_LOCAL_IRK_BIT)
            {
                privacy_add_pending_resolving_list(GAP_RESOLV_LIST_OP_ADD,
                                                   (T_GAP_IDENT_ADDR_TYPE)p_entry->remote_bd.remote_bd_type,
                                                   p_entry->remote_bd.addr);
            }
            else
            {
            }
            privacy_handle_pending_resolving_list();
        }
        else if (type == LE_BOND_DELETE)
        {
            if (p_entry->flags & LE_KEY_STORE_REMOTE_IRK_BIT)
            {
                privacy_add_pending_resolving_list(GAP_RESOLV_LIST_OP_REMOVE,
                                                   (T_GAP_IDENT_ADDR_TYPE)p_entry->resolved_remote_bd.remote_bd_type,
                                                   p_entry->resolved_remote_bd.addr);
            }
            else if (p_entry->flags & LE_KEY_STORE_LOCAL_IRK_BIT)
            {
                privacy_add_pending_resolving_list(GAP_RESOLV_LIST_OP_REMOVE,
                                                   (T_GAP_IDENT_ADDR_TYPE)p_entry->remote_bd.remote_bd_type,
                                                   p_entry->remote_bd.addr);
            }
            else
            {
            }
            privacy_handle_pending_resolving_list();
        }
        else
        {
        }
    }
 * \endcode
 */
void le_bond_clear_all_keys(void);

/**
 * @brief   Delete bond information by index
 * @param[in] idx   Index of key entry.
 * @return Operation result.
 * @retval GAP_CAUSE_SUCCESS:   Operation success.
 * @retval GAP_CAUSE_NOT_FIND:  Operation failure. Not found.
 *
 * <b>Example usage</b>
 * \code{.c}
    T_APP_RESULT app_gap_callback(uint8_t cb_type, void *p_cb_data)
    {
        T_APP_RESULT result = APP_RESULT_SUCCESS;
        T_LE_CB_DATA cb_data;
        memcpy(&cb_data, p_cb_data, sizeof(T_LE_CB_DATA));
        APP_PRINT_TRACE1("app_gap_callback: cb_type = %d", cb_type);
        switch (cb_type)
        {
        ...
        case GAP_MSG_LE_BOND_MODIFY_INFO:
           app_handle_bond_modify_msg(cb_data.p_le_bond_modify_info->type,
                                      cb_data.p_le_bond_modify_info->p_entry);
           break;
        ...
        }
    }

    void app_handle_bond_modify_msg(T_LE_BOND_MODIFY_TYPE type, T_LE_KEY_ENTRY *p_entry)
    {
        APP_PRINT_INFO1("app_handle_bond_modify_msg  GAP_MSG_LE_BOND_MODIFY_INFO:type=0x%x",
                        type);
        if (type == LE_BOND_CLEAR)
        {
            privacy_add_pending_resolving_list(GAP_RESOLV_LIST_OP_CLEAR, GAP_IDENT_ADDR_PUBLIC,
                                               NULL);
            privacy_handle_pending_resolving_list();
        }
        else if (type == LE_BOND_ADD)
        {
            if (p_entry->flags & LE_KEY_STORE_REMOTE_IRK_BIT)
            {
                privacy_add_pending_resolving_list(GAP_RESOLV_LIST_OP_ADD,
                                                   (T_GAP_IDENT_ADDR_TYPE)p_entry->resolved_remote_bd.remote_bd_type,
                                                   p_entry->resolved_remote_bd.addr);
            }
            else if (p_entry->flags & LE_KEY_STORE_LOCAL_IRK_BIT)
            {
                privacy_add_pending_resolving_list(GAP_RESOLV_LIST_OP_ADD,
                                                   (T_GAP_IDENT_ADDR_TYPE)p_entry->remote_bd.remote_bd_type,
                                                   p_entry->remote_bd.addr);
            }
            else
            {
            }
            privacy_handle_pending_resolving_list();
        }
        else if (type == LE_BOND_DELETE)
        {
            if (p_entry->flags & LE_KEY_STORE_REMOTE_IRK_BIT)
            {
                privacy_add_pending_resolving_list(GAP_RESOLV_LIST_OP_REMOVE,
                                                   (T_GAP_IDENT_ADDR_TYPE)p_entry->resolved_remote_bd.remote_bd_type,
                                                   p_entry->resolved_remote_bd.addr);
            }
            else if (p_entry->flags & LE_KEY_STORE_LOCAL_IRK_BIT)
            {
                privacy_add_pending_resolving_list(GAP_RESOLV_LIST_OP_REMOVE,
                                                   (T_GAP_IDENT_ADDR_TYPE)p_entry->remote_bd.remote_bd_type,
                                                   p_entry->remote_bd.addr);
            }
            else
            {
            }
            privacy_handle_pending_resolving_list();
        }
        else
        {
        }
    }
 * \endcode
 */
T_GAP_CAUSE le_bond_delete_by_idx(uint8_t idx);

/**
 * @brief   Delete bond information by bluetooth device address and address type
 * @param[in] bd_addr   Bonded bluetooth device address.
 * @param[in] bd_type   Bonded bluetooth device address type.
 * @return Operation result.
 * @retval GAP_CAUSE_SUCCESS:   Operation success.
 * @retval GAP_CAUSE_NOT_FIND:  Operation failure. Not found.
 *
 * <b>Example usage</b>
 * \code{.c}
    T_APP_RESULT app_gap_callback(uint8_t cb_type, void *p_cb_data)
    {
        T_APP_RESULT result = APP_RESULT_SUCCESS;
        T_LE_CB_DATA cb_data;
        memcpy(&cb_data, p_cb_data, sizeof(T_LE_CB_DATA));
        APP_PRINT_TRACE1("app_gap_callback: cb_type = %d", cb_type);
        switch (cb_type)
        {
        ...
        case GAP_MSG_LE_BOND_MODIFY_INFO:
           app_handle_bond_modify_msg(cb_data.p_le_bond_modify_info->type,
                                      cb_data.p_le_bond_modify_info->p_entry);
           break;
        ...
        }
    }

    void app_handle_bond_modify_msg(T_LE_BOND_MODIFY_TYPE type, T_LE_KEY_ENTRY *p_entry)
    {
        APP_PRINT_INFO1("app_handle_bond_modify_msg  GAP_MSG_LE_BOND_MODIFY_INFO:type=0x%x",
                        type);
        if (type == LE_BOND_CLEAR)
        {
            privacy_add_pending_resolving_list(GAP_RESOLV_LIST_OP_CLEAR, GAP_IDENT_ADDR_PUBLIC,
                                               NULL);
            privacy_handle_pending_resolving_list();
        }
        else if (type == LE_BOND_ADD)
        {
            if (p_entry->flags & LE_KEY_STORE_REMOTE_IRK_BIT)
            {
                privacy_add_pending_resolving_list(GAP_RESOLV_LIST_OP_ADD,
                                                   (T_GAP_IDENT_ADDR_TYPE)p_entry->resolved_remote_bd.remote_bd_type,
                                                   p_entry->resolved_remote_bd.addr);
            }
            else if (p_entry->flags & LE_KEY_STORE_LOCAL_IRK_BIT)
            {
                privacy_add_pending_resolving_list(GAP_RESOLV_LIST_OP_ADD,
                                                   (T_GAP_IDENT_ADDR_TYPE)p_entry->remote_bd.remote_bd_type,
                                                   p_entry->remote_bd.addr);
            }
            else
            {
            }
            privacy_handle_pending_resolving_list();
        }
        else if (type == LE_BOND_DELETE)
        {
            if (p_entry->flags & LE_KEY_STORE_REMOTE_IRK_BIT)
            {
                privacy_add_pending_resolving_list(GAP_RESOLV_LIST_OP_REMOVE,
                                                   (T_GAP_IDENT_ADDR_TYPE)p_entry->resolved_remote_bd.remote_bd_type,
                                                   p_entry->resolved_remote_bd.addr);
            }
            else if (p_entry->flags & LE_KEY_STORE_LOCAL_IRK_BIT)
            {
                privacy_add_pending_resolving_list(GAP_RESOLV_LIST_OP_REMOVE,
                                                   (T_GAP_IDENT_ADDR_TYPE)p_entry->remote_bd.remote_bd_type,
                                                   p_entry->remote_bd.addr);
            }
            else
            {
            }
            privacy_handle_pending_resolving_list();
        }
        else
        {
        }
    }
 * \endcode
 */
T_GAP_CAUSE le_bond_delete_by_bd(uint8_t *bd_addr, T_GAP_REMOTE_ADDR_TYPE bd_type);

/**
 * @brief   Get the security level.
 * @param[in] conn_id   Connection ID.
 * @param[out] p_type   Pointer to location to get the security level.
 * @return Operation result.
 * @retval GAP_CAUSE_SUCCESS    Operation success.
 * @retval GAP_CAUSE_INVALID_PARAM   Operation failure. No connection.
 *
 * <b>Example usage</b>
 * \code{.c}
    static T_USER_CMD_PARSE_RESULT cmd_seclevel(T_USER_CMD_PARSED_VALUE *p_parse_value)
    {
        T_GAP_CAUSE cause;
        T_GAP_SEC_LEVEL sec_level;
        uint8_t conn_id = p_parse_value->dw_param[0];

        cause = le_bond_get_sec_level(conn_id, &sec_level);
        if(sec_level == GAP_SEC_LEVEL_NO)
        {
            data_uart_print("GAP_SEC_LEVEL_NO\r\n");
        }
        else if(sec_level == GAP_SEC_LEVEL_UNAUTHEN)
        {
            data_uart_print("GAP_SEC_LEVEL_UNAUTHEN\r\n");
        }
        else if(sec_level == GAP_SEC_LEVEL_AUTHEN)
        {
            data_uart_print("GAP_SEC_LEVEL_AUTHEN\r\n");
        }
        else if(sec_level == GAP_SEC_LEVEL_SC_UNAUTHEN)
        {
            data_uart_print("GAP_SEC_LEVEL_SC_UNAUTHEN\r\n");
        }
        else if(sec_level == GAP_SEC_LEVEL_SC_AUTHEN)
        {
            data_uart_print("GAP_SEC_LEVEL_SC_AUTHEN\r\n");
        }
        return (T_USER_CMD_PARSE_RESULT)cause;
    }
 * \endcode
 */
T_GAP_CAUSE le_bond_get_sec_level(uint8_t conn_id, T_GAP_SEC_LEVEL *p_type);

/** End of GAP_LE_BONDMGR_Exported_Functions
  * @}
  */

/** End of GAP_LE_Bond_Manager
  * @}
  */


#ifdef __cplusplus
}
#endif

#endif /* GAP_BOND_LE_H */
