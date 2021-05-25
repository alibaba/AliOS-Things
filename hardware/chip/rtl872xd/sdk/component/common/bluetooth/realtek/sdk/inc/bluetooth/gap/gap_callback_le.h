/**
*****************************************************************************************
*     Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    gap_callback_le.h
  * @brief   This file contains function prototype for all GAP roles.
  * @details
  * @author  ranhui
  * @date    2016-02-18
  * @version v0.1
  * *************************************************************************************
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef GAP_CALLBACK_LE_H
#define GAP_CALLBACK_LE_H

#ifdef __cplusplus
extern "C"
{
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <bt_flags.h>
#include <gap_storage_le.h>
#include <gap_le_types.h>

/** @defgroup GAP_CB_MSG_MODULE GAP Callback Message
  * @brief GAP Callback Message
  * @{
  */

/*============================================================================*
 *                         Macros
 *============================================================================*/
/** @defgroup Gap_CB_Msg_Exported_Macros GAP Callback Msg Exported Macros
  * @{
  */

/** @defgroup GAP_LE_MSG_Types GAP LE Msg Types
  * @{
  */

//gap_le.h
#define GAP_MSG_LE_MODIFY_WHITE_LIST                0x01 //!<Response msg type for le_modify_white_list
#define GAP_MSG_LE_SET_RAND_ADDR                    0x02 //!<Response msg type for le_set_rand_addr
#if F_BT_LE_GAP_CENTRAL_SUPPORT
#define GAP_MSG_LE_SET_HOST_CHANN_CLASSIF           0x03 //!<Response msg type for le_set_host_chann_classif
#endif
#if F_BT_LE_4_2_DATA_LEN_EXT_SUPPORT
#define GAP_MSG_LE_WRITE_DEFAULT_DATA_LEN           0x04 //!<Response msg type for le_write_default_data_len
#endif

//gap_conn_le.h
#define GAP_MSG_LE_READ_RSSI                        0x10 //!<Response msg type for le_read_rssi
#if F_BT_LE_4_2_DATA_LEN_EXT_SUPPORT
#define GAP_MSG_LE_SET_DATA_LEN                     0x13 //!<Response msg type for le_set_data_len
#define GAP_MSG_LE_DATA_LEN_CHANGE_INFO             0x14 //!<Notification msg type for data length changed
#endif
#if F_BT_LE_GAP_CENTRAL_SUPPORT
#define GAP_MSG_LE_CONN_UPDATE_IND                  0x15 //!<Indication for le connection parameter update
#endif
#define GAP_MSG_LE_CREATE_CONN_IND                  0x16 //!<Indication for create le connection
#if F_BT_LE_5_0_SET_PHYS_SUPPORT
#define GAP_MSG_LE_PHY_UPDATE_INFO                  0x17 //!<Indication for le phyical update information
#endif
#if F_BT_LE_READ_REMOTE_FEATS
#define GAP_MSG_LE_REMOTE_FEATS_INFO                0x19 //!<Information for remote device supported features
#endif

//gap_bond_le.h
#define GAP_MSG_LE_BOND_MODIFY_INFO                 0x20 //!<Notification msg type for bond modify
#if F_BT_LE_ATT_SIGNED_WRITE_SUPPORT
#define GAP_MSG_LE_GATT_SIGNED_STATUS_INFO          0x23 //!<Notification msg type for le signed status information
#endif

//gap_scan.h
#if F_BT_LE_GAP_SCAN_SUPPORT
#define GAP_MSG_LE_SCAN_INFO                        0x30 //!<Notification msg type for le scan
#if F_BT_LE_PRIVACY_SUPPORT
#define GAP_MSG_LE_DIRECT_ADV_INFO                  0x31 //!<Notification msg type for le direct adv info
#endif
#endif

//gap_adv.h
#if F_BT_LE_GAP_PERIPHERAL_SUPPORT
#define GAP_MSG_LE_ADV_UPDATE_PARAM                 0x40 //!<Response msg type for le_adv_update_param
#endif

#define GAP_MSG_LE_GAP_STATE_MSG                    0xB0

/** End of GAP_LE_MSG_Types
  * @}
  */

/** End of Gap_CB_Msg_Exported_Macros
* @}
*/


/*============================================================================*
 *                         Types
 *============================================================================*/
/** @defgroup Gap_CB_Msg_Exported_Types GAP Callback Msg Exported Types
  * @{
  */
typedef struct
{
    uint16_t            cause;
} T_LE_CAUSE;

/** @brief  Response of le modify white list request.*/
typedef struct
{
    T_GAP_WHITE_LIST_OP operation;
    uint16_t            cause;
} T_LE_MODIFY_WHITE_LIST_RSP;

/** @brief  Response of le set random address request. */
typedef struct
{
    uint16_t        cause;
} T_LE_SET_RAND_ADDR_RSP;

#if F_BT_LE_GAP_CENTRAL_SUPPORT
/** @brief  Response of le set channel classification request. */
typedef struct
{
    uint16_t        cause;
} T_LE_SET_HOST_CHANN_CLASSIF_RSP;
#endif

/** @brief  Response for read rssi.*/
typedef struct
{
    uint8_t         conn_id;
    int8_t          rssi;
    uint16_t        cause;
} T_LE_READ_RSSI_RSP;

#if F_BT_LE_4_2_DATA_LEN_EXT_SUPPORT
/** @brief  Response for set data length, which is used for BT4.2 data length extension.*/
typedef struct
{
    uint8_t         conn_id;
    uint16_t        cause;
} T_LE_SET_DATA_LEN_RSP;

/** @brief  Notification for data length change info, which is used for BT4.2 data length extension.*/
typedef struct
{
    uint8_t         conn_id;
    uint16_t        max_tx_octets;
    uint16_t        max_tx_time;
    uint16_t        max_rx_octets;
    uint16_t        max_rx_time;
} T_LE_DATA_LEN_CHANGE_INFO;
#endif

#if F_BT_LE_GAP_CENTRAL_SUPPORT
/** @brief  Indication for connection paramete update.*/
typedef struct
{
    uint8_t         conn_id;
    uint16_t        conn_interval_min;
    uint16_t        conn_interval_max;
    uint16_t        conn_latency;
    uint16_t        supervision_timeout;
} T_LE_CONN_UPDATE_IND;
#endif

/** @brief  Indication of le connectiona. */
typedef struct
{
    uint8_t                bd_addr[6];/**< Bluetooth address of remote device */
    T_GAP_REMOTE_ADDR_TYPE remote_addr_type; /**< Address type of remote device */
}  T_LE_CREATE_CONN_IND;

#if F_BT_LE_5_0_SET_PHYS_SUPPORT
/** @brief  Notification information when phy changed.*/
typedef struct
{
    uint8_t         conn_id;
    uint16_t        cause;
    T_GAP_PHYS_TYPE tx_phy;
    T_GAP_PHYS_TYPE rx_phy;
} T_LE_PHY_UPDATE_INFO;
#endif

#if F_BT_LE_READ_REMOTE_FEATS
/** @brief  Information for remote device features.*/
typedef struct
{
    uint8_t         conn_id;
    uint16_t        cause;
    uint8_t         remote_feats[8];
} T_LE_REMOTE_FEATS_INFO;
#endif

//gap_bond_le.h
/** @brief  Bond information modify type*/
typedef enum
{
    LE_BOND_DELETE,
    LE_BOND_ADD,
    LE_BOND_CLEAR,
    LE_BOND_FULL,
    LE_BOND_KEY_MISSING,
} T_LE_BOND_MODIFY_TYPE;

/** @brief  Structure for modify bonding information.*/
typedef struct
{
    T_LE_BOND_MODIFY_TYPE type;
    T_LE_KEY_ENTRY        *p_entry;
} T_LE_BOND_MODIFY_INFO;

#if F_BT_LE_ATT_SIGNED_WRITE_SUPPORT
/** @brief  Structure for LE signed information.*/
typedef struct
{
    uint8_t         conn_id;
    uint16_t        cause;
    bool            update_local;
    uint32_t        local_sign_count;
    uint32_t        remote_sign_count;
} T_LE_GATT_SIGNED_STATUS_INFO;
#endif

//gap_scan.h
#if F_BT_LE_GAP_SCAN_SUPPORT
/** @brief  Information of le scan information.  */
typedef struct
{
    uint8_t                bd_addr[6];/**< Bluetooth address of remote device. */
    T_GAP_REMOTE_ADDR_TYPE remote_addr_type;/**< Address type of remote device. */
    T_GAP_ADV_EVT_TYPE     adv_type;/**< Advertising event type. */
    int8_t                 rssi;   /**< RSSI. */
    uint8_t                data_len;
    uint8_t                data[31];
} T_LE_SCAN_INFO;

#if F_BT_LE_PRIVACY_SUPPORT
/** @brief  Information of le direct advertising.   */
typedef struct
{
    uint8_t                bd_addr[6];
    T_GAP_REMOTE_ADDR_TYPE remote_addr_type;
    uint8_t                direct_bd_addr[6];
    T_GAP_DIRECT_ADDR_TYPE direct_addr_type;
    T_GAP_ADV_EVT_TYPE     direct_adv_type;
    int8_t                 rssi;
} T_LE_DIRECT_ADV_INFO;
#endif
#endif

//gap_adv.h
#if F_BT_LE_GAP_PERIPHERAL_SUPPORT
/** @brief  LE advertising paramter update result.*/
typedef struct
{
    uint16_t        cause;
} T_LE_ADV_UPDATE_PARAM_RSP;
#endif

/** @brief  GAP LE Callback Data*/
typedef union
{
    T_LE_CAUSE                                le_cause;

//gap_le.h
    T_LE_MODIFY_WHITE_LIST_RSP               *p_le_modify_white_list_rsp;
    T_LE_SET_RAND_ADDR_RSP                   *p_le_set_rand_addr_rsp;
#if F_BT_LE_GAP_CENTRAL_SUPPORT
    T_LE_SET_HOST_CHANN_CLASSIF_RSP          *p_le_set_host_chann_classif_rsp;
#endif
//gap_conn_le.h
    T_LE_READ_RSSI_RSP                       *p_le_read_rssi_rsp;
#if F_BT_LE_4_2_DATA_LEN_EXT_SUPPORT
    T_LE_SET_DATA_LEN_RSP                    *p_le_set_data_len_rsp;
    T_LE_DATA_LEN_CHANGE_INFO                *p_le_data_len_change_info;
#endif
#if F_BT_LE_GAP_CENTRAL_SUPPORT
    T_LE_CONN_UPDATE_IND                     *p_le_conn_update_ind;
#endif
    T_LE_CREATE_CONN_IND                     *p_le_create_conn_ind;
#if F_BT_LE_5_0_SET_PHYS_SUPPORT
    T_LE_PHY_UPDATE_INFO                     *p_le_phy_update_info;
#endif
#if F_BT_LE_READ_REMOTE_FEATS
    T_LE_REMOTE_FEATS_INFO                   *p_le_remote_feats_info;
#endif
//gap_bond_le.h
    T_LE_BOND_MODIFY_INFO                    *p_le_bond_modify_info;
#if F_BT_LE_ATT_SIGNED_WRITE_SUPPORT
    T_LE_GATT_SIGNED_STATUS_INFO             *p_le_gatt_signed_status_info;
#endif
//gap_scan.h
#if F_BT_LE_GAP_SCAN_SUPPORT
    T_LE_SCAN_INFO                           *p_le_scan_info;
#if F_BT_LE_PRIVACY_SUPPORT
    T_LE_DIRECT_ADV_INFO                     *p_le_direct_adv_info;
#endif
#endif
//gap_adv.h
#if F_BT_LE_GAP_PERIPHERAL_SUPPORT
    T_LE_ADV_UPDATE_PARAM_RSP                *p_le_adv_update_param_rsp;
#endif
    void                                     *p_gap_state_msg;
} T_LE_CB_DATA;

/** End of Gap_CB_Msg_Exported_Types
* @}
*/

/** End of GAP_CB_MSG_MODULE
* @}
*/

#ifdef __cplusplus
}
#endif

#endif /* GAP_MSG_H */
