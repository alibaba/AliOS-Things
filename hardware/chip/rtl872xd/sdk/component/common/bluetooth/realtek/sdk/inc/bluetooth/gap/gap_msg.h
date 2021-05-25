/**
*****************************************************************************************
*     Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    gap_msg.h
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
#ifndef GAP_MSG_H
#define GAP_MSG_H

#ifdef __cplusplus
extern "C"
{
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <bt_flags.h>

/** @defgroup GAP_MSG_MODULE GAP Message
  * @brief GAP message
  * @{
  */

/*============================================================================*
 *                         Macros
 *============================================================================*/
/** @defgroup Gap_Msg_Exported_Macros GAP Msg Exported Macros
  * @{
  */

/** @defgroup GAP_MSG_TYPE GAP BT Message Type Definitions
  * @brief Define the subtype of Message IO_MSG_TYPE_BT_STATUS.
  * @{
  */
#define GAP_MSG_LE_DEV_STATE_CHANGE        0x01 //!< Device state change msg type.
#define GAP_MSG_LE_CONN_STATE_CHANGE       0x02 //!< Connection state change msg type.
#define GAP_MSG_LE_CONN_PARAM_UPDATE       0x03 //!< Connection parameter update changed msg type.
#define GAP_MSG_LE_CONN_MTU_INFO           0x04 //!< Connection MTU size info msg type.

#define GAP_MSG_LE_AUTHEN_STATE_CHANGE     0x05 //!< Authentication state change msg type.
#define GAP_MSG_LE_BOND_PASSKEY_DISPLAY    0x06 //!< Bond passkey display msg type.
#define GAP_MSG_LE_BOND_PASSKEY_INPUT      0x07 //!< Bond passkey input msg type.
#define GAP_MSG_LE_BOND_OOB_INPUT          0x08 //!< Bond passkey oob input msg type.
#define GAP_MSG_LE_BOND_USER_CONFIRMATION  0x09 //!< Bond user confirmation msg type.
#define GAP_MSG_LE_BOND_JUST_WORK          0x0A //!< Bond user confirmation msg type.

/**
  * @}
  */

/** @defgroup GAP_DEVICE_STATE GAP Device State
  * @{
  */
/** @defgroup GAP_INIT_STATE GAP Initial State
  * @{
  */
#define GAP_INIT_STATE_INIT          0   //!< Waiting to be started
#define GAP_INIT_STATE_STACK_READY   1   //!< Stack is ready
/** @} End GAP_INIT_STATE */

/** @defgroup GAP_ADV_STATE GAP Advertising State
  * @{
  */
#define GAP_ADV_STATE_IDLE           0   //!< Idle, no advertising
#define GAP_ADV_STATE_START          1   //!< Start Advertising. A temporary state, haven't received the result.
#define GAP_ADV_STATE_ADVERTISING    2   //!< Advertising
#define GAP_ADV_STATE_STOP           3   //!< Stop Advertising. A temporary state, haven't received the result.
/** @} End GAP_ADV_STATE */

/** @defgroup GAP_ADV_SUB_STATE GAP Advertising Substate
  * @{
  */
#define GAP_ADV_TO_IDLE_CAUSE_STOP   0 //!<Advertising is stopped by user stop or high duty cycle directed advertising timeout
#define GAP_ADV_TO_IDLE_CAUSE_CONN   1 //!<Advertising is stopped for link establishment
/** @} End GAP_ADV_SUB_STATE */

/** @defgroup GAP_SCAN_STATE GAP Scan State
  * @{
  */
#define GAP_SCAN_STATE_IDLE          0   //!< Idle, no scanning
#define GAP_SCAN_STATE_START         1   //!< Start scanning. A temporary state, haven't received the result.
#define GAP_SCAN_STATE_SCANNING      2   //!< Scanning
#define GAP_SCAN_STATE_STOP          3   //!< Stop scanning, A temporary state, haven't received the result.
/** @} End GAP_SCAN_STATE */

/** @defgroup GAP_CONN_STATE GAP Connection State
  * @{
  */
#define GAP_CONN_DEV_STATE_IDLE          0   //!< Idle
#define GAP_CONN_DEV_STATE_INITIATING    1   //!< Initiating Connection
/** @} End GAP_CONN_STATE */

/** @} End GAP_DEVICE_STATE */

/** @defgroup BOND_PAIRING_STATE_DEFINES Authentication States
 * @{
 */
#define GAP_AUTHEN_STATE_STARTED             0x00  //!< Authentication started
#define GAP_AUTHEN_STATE_COMPLETE            0x01  //!< Authentication complete
/** @} End BOND_PAIRING_STATE_DEFINES */

/** @defgroup GAP_CONN_PARAM_UPDATE_STATUS Connection Parameter Update Status
 * @{
 */
#define GAP_CONN_PARAM_UPDATE_STATUS_SUCCESS      0 //!< Connection paramter update status success.
#define GAP_CONN_PARAM_UPDATE_STATUS_FAIL         1 //!< Connection paramter update status failed.
#define GAP_CONN_PARAM_UPDATE_STATUS_PENDING      2 //!< Connection paramter update status pending.
/** End of GAP_CONN_PARAM_UPDATE_STATUS
  * @}
  */

/** @brief GAP connection states*/
typedef enum
{
    GAP_CONN_STATE_DISCONNECTED, //!< Disconnected.
    GAP_CONN_STATE_CONNECTING,   //!< Connecting.
    GAP_CONN_STATE_CONNECTED,    //!< Connected.
    GAP_CONN_STATE_DISCONNECTING //!< Disconnecting.
} T_GAP_CONN_STATE;

/** End of Gap_Msg_Exported_Macros
* @}
*/


/*============================================================================*
 *                         Types
 *============================================================================*/
/** @defgroup Gap_Msg_Exported_Types GAP Msg Exported Types
  * @{
  */
/** @brief  Device State.*/
typedef struct
{
    uint8_t gap_init_state: 1;  //!< @ref GAP_INIT_STATE
    uint8_t gap_adv_sub_state: 1;  //!< @ref GAP_ADV_SUB_STATE
    uint8_t gap_adv_state: 2;   //!< @ref GAP_ADV_STATE
    uint8_t gap_scan_state: 2;  //!< @ref GAP_SCAN_STATE
    uint8_t gap_conn_state: 2;  //!< @ref GAP_CONN_STATE
} T_GAP_DEV_STATE;

/** @brief  The msg_data of GAP_MSG_LE_DEV_STATE_CHANGE.*/
typedef struct
{
    T_GAP_DEV_STATE new_state;
    uint16_t cause;
} T_GAP_DEV_STATE_CHANGE;


/** @brief  The msg_data of GAP_MSG_LE_CONN_STATE_CHANGE.*/
typedef struct
{
    uint8_t conn_id;
    uint8_t new_state;
    uint16_t disc_cause;
} T_GAP_CONN_STATE_CHANGE;

/** @brief  The msg_data of GAP_MSG_LE_CONN_PARAM_UPDATE.*/
typedef struct
{
    uint8_t conn_id;
    uint8_t status;
    uint16_t cause;
} T_GAP_CONN_PARAM_UPDATE;

/** @brief  The msg_data of GAP_MSG_LE_AUTHEN_STATE_CHANGE.*/
typedef struct
{
    uint8_t conn_id;
    uint8_t new_state;
    uint16_t status;
} T_GAP_AUTHEN_STATE;

/** @brief  The msg_data of GAP_MSG_LE_BOND_PASSKEY_DISPLAY.*/
typedef struct
{
    uint8_t conn_id;
} T_GAP_BOND_PASSKEY_DISPLAY;

/** @brief  The msg_data of GAP_MSG_LE_BOND_USER_CONFIRMATION.*/
typedef struct
{
    uint8_t conn_id;
} T_GAP_BOND_USER_CONF;

/** @brief  The msg_data of T_GAP_BOND_JUST_WORK_CONF.*/
typedef struct
{
    uint8_t conn_id;
} T_GAP_BOND_JUST_WORK_CONF;
/** @brief  The msg_data of GAP_MSG_LE_BOND_PASSKEY_INPUT.*/
typedef struct
{
    uint8_t conn_id;
    uint8_t key_press;
} T_GAP_BOND_PASSKEY_INPUT;

/** @brief  The msg_data of GAP_MSG_LE_BOND_OOB_INPUT.*/
typedef struct
{
    uint8_t conn_id;
} T_GAP_BOND_OOB_INPUT;

/** @brief  The msg_data of GAP_MSG_LE_CONN_MTU_INFO.*/
typedef struct
{
    uint8_t   conn_id;
    uint16_t  mtu_size;
} T_GAP_CONN_MTU_INFO;

/** @brief  The msg_data of T_LE_GAP_MSG. */
typedef union
{
    T_GAP_DEV_STATE_CHANGE     gap_dev_state_change;
    T_GAP_CONN_STATE_CHANGE    gap_conn_state_change;
    T_GAP_CONN_PARAM_UPDATE    gap_conn_param_update;
    T_GAP_CONN_MTU_INFO        gap_conn_mtu_info;

    T_GAP_AUTHEN_STATE         gap_authen_state;
    T_GAP_BOND_PASSKEY_DISPLAY gap_bond_passkey_display;
    T_GAP_BOND_PASSKEY_INPUT   gap_bond_passkey_input;
    T_GAP_BOND_OOB_INPUT       gap_bond_oob_input;
    T_GAP_BOND_USER_CONF       gap_bond_user_conf;
    T_GAP_BOND_JUST_WORK_CONF  gap_bond_just_work_conf;
} T_LE_GAP_MSG_DATA;

/** @brief  As the param of T_IO_MSG. type is IO_MSG_TYPE_BT_STATUS. */
typedef struct
{
    T_LE_GAP_MSG_DATA msg_data;
} T_LE_GAP_MSG;

/** End of Gap_Msg_Exported_Types
* @}
*/

/** End of GAP_MSG_MODULE
* @}
*/

#ifdef __cplusplus
}
#endif

#endif /* GAP_MSG_H */
