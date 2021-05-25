/**
*****************************************************************************************
*     Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    gap_conn_le.h
  * @brief   This file contains all the constants and functions prototypes for GAP protocol.
  * @details
  * @author  jane
  * @date    2016-02-18
  * @version v1.0
  * *************************************************************************************
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef GAP_CONN_LE_H
#define GAP_CONN_LE_H

#ifdef __cplusplus
extern "C"
{
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <bt_flags.h>
#include <gap.h>
#include <gap_le.h>
#include <gap_msg.h>

/** @defgroup GAP_LE_CONNECTION_MODULE GAP LE Connection Module
  * @brief GAP LE connection module
  * @{
  */

/*============================================================================*
 *                         Types
 *============================================================================*/
/** @defgroup GAP_LE_CONNECTION_Exported_Types GAP Connection Exported Types
  * @{
  */
#if F_BT_LE_GAP_CENTRAL_SUPPORT
/** @brief GAP connection phy types*/
typedef enum
{
    GAP_CONN_PARAM_1M = 0,
#if F_BT_LE_5_0_AE_ADV_SUPPORT
    GAP_CONN_PARAM_2M = 1,
    GAP_CONN_PARAM_CODED = 2,
#endif
} T_GAP_CONN_PARAM_TYPE;
#endif

/** @brief GAP link roles */
typedef enum
{
    GAP_LINK_ROLE_UNDEFINED,    //!< Unknown.
    GAP_LINK_ROLE_MASTER,       //!< Role is master.
    GAP_LINK_ROLE_SLAVE         //!< Role is slave.
} T_GAP_ROLE;

/** @brief LE connection parameter types */
typedef enum
{
    GAP_PARAM_CONN_BD_ADDR         = 0x270,//!< Address of connected device. Read only. Size is uint8[B_MAX_ADV_LEN]. Set to all zeros when not connected.
    GAP_PARAM_CONN_BD_ADDR_TYPE    = 0x271,//!< Address type of connected device. Read only. Size is uint8. Set to zero when not connected.
    GAP_PARAM_CONN_INTERVAL        = 0x272,//!< Current connection interval.  Read only. Size is uint16.  Range is 7.5ms to 4 seconds (0x0006 to 0x0C80).  Default is 0 (no connection).
    GAP_PARAM_CONN_LATENCY         = 0x273,//!< Current slave latency.  Read only.  Size is uint16.  Range is 0 to 499. Default is 0 (no slave latency or no connection).
    GAP_PARAM_CONN_TIMEOUT         = 0x274,//!< Current timeout value.  Read only.  size is uint16.  Range is 100ms to 32 seconds.  Default is 0 (no connection).
    GAP_PARAM_CONN_MTU_SIZE        = 0x275,//!< MTU size.  Read only.  size is uint16.
    GAP_PARAM_CONN_LOCAL_BD_TYPE   = 0x276,//!< Local bluetooth address type.  Read only.  size is 1 byte.
#if F_BT_LE_5_0_SET_PHYS_SUPPORT
    GAP_PARAM_CONN_RX_PHY_TYPE     = 0x277,//!< Rx phy type.  Read only.  size is 1 byte.
    GAP_PARAM_CONN_TX_PHY_TYPE     = 0x278,//!< Tx phy type.  Read only.  size is 1 byte.
#endif
#if F_BT_LE_READ_REMOTE_FEATS
    GAP_PARAM_CONN_REMOTE_FEATURES = 0x279,//!< Remote supported features.  Read only.
#endif
#if F_BT_LE_5_0_CSA2_SUPPORT
    GAP_PARAM_CONN_CHANN_ALGORITHM = 0x27a,//!< LE Channel Selection Algorithm used.  Read only.
#endif
    GAP_PARAM_CONN_HANDLE          = 0x27b,//!< LE link connection handle. Read only.
} T_LE_CONN_PARAM_TYPE;

/** @brief  Connected device information.*/
typedef struct
{
    T_GAP_CONN_STATE conn_state;             //!< Connection state
    T_GAP_ROLE       role;                   //!< Device role
    uint8_t          remote_bd[GAP_BD_ADDR_LEN];  //!< Remote address
    uint8_t          remote_bd_type;         //!< Remote address type
} T_GAP_CONN_INFO;

#if F_BT_LE_GAP_CENTRAL_SUPPORT
/** @brief  Definition of LE connection request parameter.*/
typedef struct
{
    uint16_t scan_interval;/**< Time interval from when the Controller started its last scan
                                until it begins the subsequent scan on the primary advertising channel.

                                By default @ref GAP_PARAM_USE_EXTENDED_ADV is false,
                                Range: 0x0004 to 0x4000
                                Time = N * 0.625 ms
                                Time Range: 2.5 ms to 10.24 s

                                If @ref GAP_PARAM_USE_EXTENDED_ADV is set as true,
                                Range: 0x0004 to 0xFFFF
                                Time = N * 0.625 ms
                                Time Range: 2.5 ms to 40.959375 s */
    uint16_t scan_window;/**< Duration of the scan on the primary advertising channel.
                              By default @ref GAP_PARAM_USE_EXTENDED_ADV is false,
                              Range: 0x0004 to 0x4000
                              Time = N * 0.625 ms
                              Time Range: 2.5 ms to 10.24 s

                              If @ref GAP_PARAM_USE_EXTENDED_ADV is set as true,
                              Range: 0x0004 to 0xFFFF
                              Time = N * 0.625 ms
                              Time Range: 2.5 ms to 40.959375 s */
    uint16_t conn_interval_min;/**< Minimum value for the connection interval. This shall be less
                                    than or equal to Conn_Interval_Max[i].
                                    Range: 0x0006 to 0x0C80
                                    Time = N * 1.25 ms
                                    Time Range: 7.5 ms to 4 s */
    uint16_t conn_interval_max;/**< Maximum value for the connection interval. This shall be greater
                                    than or equal to Conn_Interval_Max[i].
                                    Range: 0x0006 to 0x0C80
                                    Time = N * 1.25 ms
                                    Time Range: 7.5 ms to 4 s */
    uint16_t conn_latency;/**< Slave latency for the connection in number of connection events.
                               Range: 0x0000 to 0x01F3 */
    uint16_t supv_tout;/**< Supervision timeout for the LE Link.
                            Range: 0x000A to 0x0C80
                            Time = N * 10 ms
                            Time Range: 100 ms to 32 s */
    uint16_t ce_len_min;/**< Informative parameter recommending the minimum length of connection
                             event needed for this LE connection.
                             Range: 0x0000 - 0xFFFF
                             Time = N * 0.625 ms */
    uint16_t ce_len_max;/**< Informative parameter recommending the maximum length of connection
                             event needed for this LE connection.
                             Range: 0x0000 - 0xFFFF
                             Time = N * 0.625 ms */
} T_GAP_LE_CONN_REQ_PARAM;
#endif

/** End of GAP_LE_CONNECTION_Exported_Types
  * @}
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/
/**
 * @defgroup GAP_CONNECTION_COMMON_EXPORT_Functions GAP Connection Exported Functions
 *
 * @{
 */

/**
  * @brief  Get a GAP connection parameter.
  *
  *         NOTE: You can call this function with a GAP connection parameter type and it will get a
  *         GAP connection parameter.  GAP connection parameters are defined in @ref T_LE_CONN_PARAM_TYPE.  Also, the
  *         "p_value" field must point to a data with type "uint16_t".
  *
  * @param[in]  param Connection parameter type: @ref T_LE_CONN_PARAM_TYPE
  * @param[in,out] p_value Pointer to location to get the value.  This is dependent on
  *                  the parameter type and will be cast to the appropriate
  *                  data type (For example: if data type param is uint16_t, p_value will be cast to
  *                  pointer of uint16_t).
  * @param[in]  conn_id Connection id.
  *
  * @return Get result
  * @retval GAP_CAUSE_SUCCESS Get parameter success.
  * @retval other Get parameter failed.
  */
T_GAP_CAUSE le_get_conn_param(T_LE_CONN_PARAM_TYPE param, void *p_value, uint8_t conn_id);

/**
  * @brief  Get connection information.
  * @param[in]  conn_id Connection ID
  * @param[in,out] p_conn_info Connection infomation @ref T_GAP_CONN_INFO
  * @return Get result
  * @retval true Success
  * @retval false Get failed
  */
bool        le_get_conn_info(uint8_t conn_id, T_GAP_CONN_INFO *p_conn_info);

/**
  * @brief  Get connection address information.
  * @param[in]  conn_id Connection ID
  * @param[in,out] bd_addr Remote bluetooth device address
  * @param[in,out] bd_type Remote bluetooth device address type
  * @return Get result
  * @retval true Success
  * @retval false Get failed
  */
bool        le_get_conn_addr(uint8_t conn_id, uint8_t *bd_addr, uint8_t *bd_type);

/**
  * @brief  Get connection ID.
  * @param[in]  bd_addr Remote bluetooth device address
  * @param[in]  bd_type Remote bluetooth device address type
  * @param[in, out] p_conn_id Connection ID
  * @return Get result
  * @retval true Success
  * @retval false Get failed
  */
bool        le_get_conn_id(uint8_t *bd_addr, uint8_t bd_type, uint8_t *p_conn_id);

/**
* @brief   Get the active link number.
*
* @return  Active link number
* @retval 0 No connection created
* @retval other Connected links number
*/
uint8_t     le_get_active_link_num(void);

/**
* @brief   Get the idle link number.
*
* @return  Idle link number.
* @retval 0 Can't create new connection
* @retval other Can create new connection
*/
uint8_t     le_get_idle_link_num(void);

/**
 * @brief   Terminates the existing connection. When link is disconnected, @ref app_handle_conn_state_evt will be
 *          called with new_state as @ref GAP_CONN_STATE_DISCONNECTED.
 *
 * @param[in] conn_id connection ID to be disconnected.
 * @return  Operation result.
 * @retval GAP_CAUSE_SUCCESS Send request success
 * @retval GAP_CAUSE_NON_CONN Failed. No connection
 * @retval GAP_CAUSE_INVALID_PARAM Failed. Invalid parameter
 *
 * <b>Example usage</b>
 * \code{.c}
    void test()
    {
        uint8_t conn_id = 0x01;
        le_disconnect(conn_id);
    }

    void app_handle_conn_state_evt(uint8_t conn_id, T_GAP_CONN_STATE new_state, uint16_t disc_cause)
    {
        APP_PRINT_INFO4("app_handle_conn_state_evt: conn_id %d old_state %d new_state %d, disc_cause 0x%x",
                        conn_id, gap_conn_state, new_state, disc_cause);
        switch (new_state)
        {
        case GAP_CONN_STATE_DISCONNECTED:
            {
                if ((disc_cause != (HCI_ERR | HCI_ERR_REMOTE_USER_TERMINATE))
                    && (disc_cause != (HCI_ERR | HCI_ERR_LOCAL_HOST_TERMINATE)))
                {
                    APP_PRINT_ERROR1("app_handle_conn_state_evt: connection lost cause 0x%x", disc_cause);
                }

                le_adv_start();
            }
            break;

        case GAP_CONN_STATE_CONNECTED:
            {
                uint16_t conn_interval;
                uint16_t conn_latency;
                uint16_t conn_supervision_timeout;
                uint8_t  remote_bd[6];
                T_GAP_REMOTE_ADDR_TYPE remote_bd_type;

                le_get_conn_param(GAP_PARAM_CONN_INTERVAL, &conn_interval, conn_id);
                le_get_conn_param(GAP_PARAM_CONN_LATENCY, &conn_latency, conn_id);
                le_get_conn_param(GAP_PARAM_CONN_TIMEOUT, &conn_supervision_timeout, conn_id);
                le_get_conn_addr(conn_id, remote_bd, &remote_bd_type);
                APP_PRINT_INFO5("GAP_CONN_STATE_CONNECTED:remote_bd %s, remote_addr_type %d, conn_interval 0x%x, conn_latency 0x%x, conn_supervision_timeout 0x%x",
                                TRACE_BDADDR(remote_bd), remote_bd_type,
                                conn_interval, conn_latency, conn_supervision_timeout);
            }
            break;

        default:
            break;
        }
        gap_conn_state = new_state;
    }
 * \endcode
 */
T_GAP_CAUSE le_disconnect(uint8_t conn_id);

/**
 * @brief   Read rssi value of the connection. RSSI value will be returned by
 *          @ref app_gap_callback with cb_type @ref GAP_MSG_LE_READ_RSSI.
 *
 * @param[in] conn_id Connection ID
 * @return  Read request result.
 * @retval  GAP_CAUSE_SUCCESS: Read request sent success.<BR>
 * @retval  GAP_CAUSE_NON_CONN: Read request sent fail.<BR>
 *
 * <b>Example usage</b>
 * \code{.c}
    void test()
    {
        uint8_t conn_id = 0x01;
        le_read_rssi(conn_id);
    }

    T_APP_RESULT app_gap_callback(uint8_t cb_type, void *p_cb_data)
    {
        T_APP_RESULT result = APP_RESULT_SUCCESS;
        T_LE_CB_DATA cb_data;
        memcpy(&cb_data, p_cb_data, sizeof(T_LE_CB_DATA));
        APP_PRINT_TRACE1("app_gap_callback: cb_type = %d", cb_type);
        switch (cb_type)
        {
        ...
        case GAP_MSG_LE_READ_RSSI:
          APP_PRINT_INFO3("GAP_MSG_LE_READ_RSSI: conn_id 0x%x, cause 0x%x, rssi %d",
                          cb_data.p_le_read_rssi_rsp->conn_id,
                          cb_data.p_le_read_rssi_rsp->cause,
                          cb_data.p_le_read_rssi_rsp->rssi);
          break;
        }
        ...
    }
 * \endcode
 */
T_GAP_CAUSE le_read_rssi(uint8_t conn_id);

/**
  * @brief   Set the data length used in LL for data length extension. LE set data length response will be returned by
  *          @ref app_gap_callback with cb_type @ref GAP_MSG_LE_SET_DATA_LEN. And if data length was changed,
  *          @ref app_gap_callback with cb_type @ref GAP_MSG_LE_DATA_LEN_CHANGE_INFO will be called.
  *
  * @param[in] conn_id Connection ID
  * @param[in] tx_octets    Preferred maximum number of payload octets that the local Controller
  *                         should include in a single Link Layer packet on this connection.
                            Range 0x001B-0x00FB (all other values reserved for future use)
  * @param[in] tx_time      Preferred maximum number of microseconds that the local Controller
                            should use to transmit a single Link Layer packet on this connection.
                            Range 0x0148-0x4290 (all other values reserved for future use)
  * @return  result.
  * @retval  GAP_CAUSE_SUCCESS: Set request sent success.<BR>
  * @retval  GAP_CAUSE_NON_CONN: Set request sent fail.<BR>
  *
  * <b>Example usage</b>
  * \code{.c}
    void test()
    {
        uint8_t conn_id = 0x01;
        uint16_t tx_octets;
        uint16_t tx_time;
        le_set_data_len(conn_id, tx_octets, tx_time);
    }

    T_APP_RESULT app_gap_callback(uint8_t cb_type, void *p_cb_data)
    {
        T_APP_RESULT result = APP_RESULT_SUCCESS;
        T_LE_CB_DATA cb_data;
        memcpy(&cb_data, p_cb_data, sizeof(T_LE_CB_DATA));
        APP_PRINT_TRACE1("app_gap_callback: cb_type = %d", cb_type);
        switch (cb_type)
        {
        ...
        case GAP_MSG_LE_SET_DATA_LEN:
           APP_PRINT_INFO2("GAP_MSG_LE_SET_DATA_LEN: conn_id 0x%x, cause 0x%x",
                           cb_data.p_le_set_data_len_rsp->conn_id,
                           cb_data.p_le_set_data_len_rsp->cause);
           break;
        case GAP_MSG_LE_DATA_LEN_CHANGE_INFO:
           APP_PRINT_INFO5("GAP_MSG_LE_DATA_LEN_CHANGE_INFO: conn_id 0x%x, max_tx_octets 0x%x, max_tx_time 0x%x,
           max_rx_octets 0x%x, max_rx_time 0x%x",
                           cb_data.p_le_data_len_change_info->conn_id,
                           cb_data.p_le_data_len_change_info->max_tx_octets,
                           cb_data.p_le_data_len_change_info->max_tx_time,
                           cb_data.p_le_data_len_change_info->max_rx_octets,
                           cb_data.p_le_data_len_change_info->max_rx_time);
           break;
        }
        ...
    }
  * \endcode
  */
#if F_BT_LE_4_2_DATA_LEN_EXT_SUPPORT
T_GAP_CAUSE le_set_data_len(uint8_t conn_id, uint16_t tx_octets, uint16_t tx_time);
#endif

/**
  * @brief   Set the PHY preferences for the connection identified by the Connection_Handle.
  *          The Controller might not be able to make the change (e.g. because the peer does
  *          not support the requested PHY) or may decide that the current PHY is preferable.
  *          If the phy used was changed, @ref app_gap_callback with cb_type @ref GAP_MSG_LE_PHY_UPDATE_INFO will be called.
  *
  * @param[in] conn_id Connection ID
  * @param[in] all_phys A bit field that allows the Host to specify, for each direction,
  *                     whether it has no preference among the PHYs that the Controller
  *                     supports in a given direction or whether it has specified particular PHYs that it
  *                     prefers in the TX_PHYS or RX_PHYS parameter.
  * @param[in] tx_phys A bit field that indicates the transmitter PHYs that
  *                    the Host prefers the Controller to use. If the ALL_PHYS parameter specifies
  *                    that the Host has no preference, the TX_PHYS parameter is ignored;
  *                    otherwise at least one bit shall be set to 1.
  * @param[in] rx_phys A bit field that indicates the receiver PHYs that the
  *                    Host prefers the Controller to use. If the ALL_PHYS parameter specifies that
  *                    the Host has no preference, the RX_PHYS parameter is ignored; otherwise at
  *                    least one bit shall be set to 1.
  * @param[in] phy_options A bit field that allows the Host to specify options
  *                        for PHYs. The default value for a new connection shall be all zero bits. The
  *                        Controller may override any preferred coding for transmitting on the LE Coded
  *                        PHY.
  * @return  result.
  * @retval  GAP_CAUSE_SUCCESS: Set request sent success.<BR>
  * @retval  GAP_CAUSE_NON_CONN: Set request sent fail.<BR>
  *
  * <b>Example usage</b>
  * \code{.c}
    void test()
    {
        uint8_t conn_id = 0x01;
        uint8_t all_phys = GAP_PHYS_NO_PREFER_TX_BIT;
        uint8_t tx_phys = GAP_PHYS_PREFER_CODED_BIT;
        uint8_t rx_phys = GAP_PHYS_PREFER_CODED_BIT;
        uint16_t phy_options = GAP_PHYS_OPTIONS_CODED_PREFER_S8;
        le_set_phy(conn_id, all_phys, tx_phys, rx_phys, phy_options);
    }

    T_APP_RESULT app_gap_callback(uint8_t cb_type, void *p_cb_data)
    {
        T_APP_RESULT result = APP_RESULT_SUCCESS;
        T_LE_CB_DATA cb_data;
        memcpy(&cb_data, p_cb_data, sizeof(T_LE_CB_DATA));
        APP_PRINT_TRACE1("app_gap_callback: cb_type = %d", cb_type);
        switch (cb_type)
        {
        ...
        case GAP_MSG_LE_PHY_UPDATE_INFO:
            APP_PRINT_INFO4("GAP_MSG_LE_PHY_UPDATE_INFO:conn %d, cause 0x%x, rx_phy %d, tx_phy %d",
                       cb_data.p_le_phy_update_info->conn_id,
                       cb_data.p_le_phy_update_info->cause,
                       cb_data.p_le_phy_update_info->rx_phy,
                       cb_data.p_le_phy_update_info->tx_phy);
        break;
        }
        ...
    }
  * \endcode
  */
#if F_BT_LE_5_0_SET_PHYS_SUPPORT
T_GAP_CAUSE le_set_phy(uint8_t conn_id, uint8_t all_phys, uint8_t tx_phys, uint8_t rx_phys,
                       T_GAP_PHYS_OPTIONS phy_options);
#endif

/**
  * @brief  Set connection parameter.
  *
  *         App can call this api to modify connection interval, latency and supervision timeout value.
  *
  * @param[in] type  Connection ID for this link.
  * @param[in] p_conn_param  Connection parameters @ref T_GAP_LE_CONN_REQ_PARAM.
  * @return  result.
  * @retval GAP_CAUSE_SUCCESS Send request success
  * @retval GAP_CAUSE_NON_CONN Failed. No connection
  *
  * <b>Example usage</b>
  * \code{.c}
     void test()
     {
         T_GAP_LE_CONN_REQ_PARAM conn_req_param;
         T_GAP_REMOTE_ADDR_TYPE bd_type = GAP_REMOTE_ADDR_LE_PUBLIC;
         uint8_t bd_addr[BD_ADDR_SIZE] = {0x00, 0xe0 x04c, 0x23, 0x99, 0x87};
         conn_req_param.scan_interval = 0x10;
         conn_req_param.scan_window = 0x10;
         conn_req_param.conn_interval_min = 80;
         conn_req_param.conn_interval_max = 80;
         conn_req_param.conn_latency = 0;
         conn_req_param.supv_tout = 1000;
         conn_req_param.ce_len_min = 2 * (conn_req_param.conn_interval_min - 1);
         conn_req_param.ce_len_max = 2 * (conn_req_param.conn_interval_max - 1);
         le_set_conn_param(GAP_CONN_PARAM_1M, &conn_req_param);

         le_connect(0, bd_addr,
                   bd_type,
                   GAP_LOCAL_ADDR_LE_PUBLIC,
                   1000);
     }
  * \endcode
  */
#if F_BT_LE_GAP_CENTRAL_SUPPORT
T_GAP_CAUSE le_set_conn_param(T_GAP_CONN_PARAM_TYPE type,
                              T_GAP_LE_CONN_REQ_PARAM *p_conn_param);
#endif

/**
 * @brief   Create a connection to a connectable advertiser. @ref le_set_conn_param shall be called first to set connection parameters.
 *
 * @param[in] init_phys  The PHY(s) on which the advertising packets should be received on the primary advertising channel and the PHYs
 *                       for which connection parameters have been specified.
 * @param[in] remote_bd The Peer's Public Device Address, Random (static) Device Address, Non-Resolvable Private Address, or
                        Resolvable Private Address depending on the Peer_Address_Type parameter.
 * @param[in] remote_bd_type The type of address used in the connectable advertisement sent by the peer.
 * @param[in] local_bd_type  The type of address being used in the connection request packets.
 * @param[in] scan_timeout  Scan timeout value.
 * @return  result.
 * @retval GAP_CAUSE_SUCCESS Send request success
 * @retval GAP_CAUSE_NON_CONN Failed. No connection
 *
 * <b>Example usage</b>
 * \code{.c}
    void test()
    {
        T_GAP_LE_CONN_REQ_PARAM conn_req_param;
        T_GAP_REMOTE_ADDR_TYPE bd_type = GAP_REMOTE_ADDR_LE_PUBLIC;
        uint8_t bd_addr[BD_ADDR_SIZE] = {0x00, 0xe0 x04c, 0x23, 0x99, 0x87};
        conn_req_param.scan_interval = 0x10;
        conn_req_param.scan_window = 0x10;
        conn_req_param.conn_interval_min = 80;
        conn_req_param.conn_interval_max = 80;
        conn_req_param.conn_latency = 0;
        conn_req_param.supv_tout = 1000;
        conn_req_param.ce_len_min = 2 * (conn_req_param.conn_interval_min - 1);
        conn_req_param.ce_len_max = 2 * (conn_req_param.conn_interval_max - 1);
        le_set_conn_param(GAP_CONN_PARAM_1M, &conn_req_param);

        le_connect(0, bd_addr,
                  bd_type,
                  GAP_LOCAL_ADDR_LE_PUBLIC,
                  1000);
    }

    void app_handle_conn_state_evt(uint8_t conn_id, T_GAP_CONN_STATE new_state, uint16_t disc_cause)
    {
        APP_PRINT_INFO4("app_handle_conn_state_evt: conn_id %d old_state %d new_state %d, disc_cause 0x%x",
                        conn_id, gap_conn_state, new_state, disc_cause);
        switch (new_state)
        {
        case GAP_CONN_STATE_DISCONNECTED:
            {
                if ((disc_cause != (HCI_ERR | HCI_ERR_REMOTE_USER_TERMINATE))
                    && (disc_cause != (HCI_ERR | HCI_ERR_LOCAL_HOST_TERMINATE)))
                {
                    APP_PRINT_ERROR1("app_handle_conn_state_evt: connection lost cause 0x%x", disc_cause);
                }

                le_adv_start();
            }
            break;

        case GAP_CONN_STATE_CONNECTED:
            {
                uint16_t conn_interval;
                uint16_t conn_latency;
                uint16_t conn_supervision_timeout;
                uint8_t  remote_bd[6];
                T_GAP_REMOTE_ADDR_TYPE remote_bd_type;

                le_get_conn_param(GAP_PARAM_CONN_INTERVAL, &conn_interval, conn_id);
                le_get_conn_param(GAP_PARAM_CONN_LATENCY, &conn_latency, conn_id);
                le_get_conn_param(GAP_PARAM_CONN_TIMEOUT, &conn_supervision_timeout, conn_id);
                le_get_conn_addr(conn_id, remote_bd, &remote_bd_type);
                APP_PRINT_INFO5("GAP_CONN_STATE_CONNECTED:remote_bd %s, remote_addr_type %d, conn_interval 0x%x, conn_latency 0x%x, conn_supervision_timeout 0x%x",
                                TRACE_BDADDR(remote_bd), remote_bd_type,
                                conn_interval, conn_latency, conn_supervision_timeout);
            }
            break;

        default:
            break;
        }
        gap_conn_state = new_state;
    }

    //When link is connected, stack will read remote used feaures, and the result will be notified by msg GAP_MSG_LE_REMOTE_FEATS_INFO
    T_APP_RESULT app_gap_callback(uint8_t cb_type, void *p_cb_data)
    {
       T_APP_RESULT result = APP_RESULT_SUCCESS;
       T_LE_CB_DATA cb_data;
       memcpy(&cb_data, p_cb_data, sizeof(T_LE_CB_DATA));
       APP_PRINT_TRACE1("app_gap_callback: cb_type = %d", cb_type);
       switch (cb_type)
       {
       ...
       case GAP_MSG_LE_REMOTE_FEATS_INFO:
           APP_PRINT_INFO3("GAP_MSG_LE_REMOTE_FEATS_INFO: conn id %d, cause 0x%x, remote_feats %b",
                           cb_data.p_le_remote_feats_info->conn_id,
                           cb_data.p_le_remote_feats_info->cause,
                           TRACE_BINARY(8, cb_data.p_le_remote_feats_info->remote_feats));
           break;
       }
       ...
    }
 *
 * \endcode
 */
#if F_BT_LE_GAP_CENTRAL_SUPPORT
T_GAP_CAUSE le_connect(uint8_t init_phys, uint8_t *remote_bd,
                       T_GAP_REMOTE_ADDR_TYPE remote_bd_type,
                       T_GAP_LOCAL_ADDR_TYPE local_bd_type, uint16_t scan_timeout);
#endif
/**
  * @brief      Send connection parameter update request msg to bt stack. Connection parameter update result will be returned
  *             by @ref app_handle_conn_param_update_evt.
  *
  *         App can call this api to modify connection interval, latency and supervision timeout value.
  *
  * @param[in] conn_id  Connection ID for this link.
  * @param[in] conn_interval_min  Value range: 0x0006 - 0x0C80 (7.5ms - 4000ms, 1.25ms/step).
  * @param[in] conn_interval_max  Value range: 0x0006 - 0x0C80 (7.5ms - 4000ms, 1.25ms/step).
  * @param[in] conn_latency  Value range: 0x0000 - 0x01F3.
  * @param[in] supervision_timeout  Value range: 0x000A - 0x0C80 (100ms - 32000ms, 10ms/step).
  * @param[in] ce_length_min  Value range: 0x0006 - 0x0C80 (7.5ms - 4000ms, 1.25ms/step).
  * @param[in] ce_length_max  Value range: 0x0006 - 0x0C80 (7.5ms - 4000ms, 1.25ms/step).
  * @return  result.
  * @retval GAP_CAUSE_SUCCESS Send request success.
  * @retval GAP_CAUSE_NON_CONN Failed. No connection.
  *
  * <b>Example usage</b>
  * \code{.c}
    void test()
    {
        uint8_t conn_id = 0x01;
        T_GAP_CAUSE cause;
        uint16_t conn_interval_min = 0x10;
        uint16_t conn_interval_max = 0x10;
        uint16_t conn_latency = 50;
        uint16_t supervision_timeout = 1000;
        uint16_t ce_length_min = 2 * (conn_interval_min - 1);
        uint16_t ce_length_max = 2 * (conn_interval_max - 1);


        cause = le_update_conn_param(conn_id,
                                    conn_interval_min,
                                    conn_interval_max,
                                    conn_latency,
                                    supervision_timeout,
                                    ce_length_min,
                                    ce_length_max
                                   );

    }

    void app_handle_conn_param_update_evt(uint8_t conn_id, uint8_t status, uint16_t cause)
    {
        switch (status)
        {
        case GAP_CONN_PARAM_UPDATE_STATUS_SUCCESS:
            {
                uint16_t conn_interval;
                uint16_t conn_slave_latency;
                uint16_t conn_supervision_timeout;

                le_get_conn_param(GAP_PARAM_CONN_INTERVAL, &conn_interval, conn_id);
                le_get_conn_param(GAP_PARAM_CONN_LATENCY, &conn_slave_latency, conn_id);
                le_get_conn_param(GAP_PARAM_CONN_TIMEOUT, &conn_supervision_timeout, conn_id);
                APP_PRINT_INFO3("app_handle_conn_param_update_evt update success:conn_interval 0x%x, conn_slave_latency 0x%x, conn_supervision_timeout 0x%x",
                                conn_interval, conn_slave_latency, conn_supervision_timeout);
            }
            break;

        case GAP_CONN_PARAM_UPDATE_STATUS_FAIL:
            {
                APP_PRINT_ERROR1("app_handle_conn_param_update_evt update failed: cause 0x%x", cause);
            }
            break;

        case GAP_CONN_PARAM_UPDATE_STATUS_PENDING:
            {
                APP_PRINT_INFO0("app_handle_conn_param_update_evt update pending.");
            }
            break;

        default:
            break;
        }
    }


    //If app receive connection parameter update request from remote device, app will receive GAP_MSG_LE_CONN_UPDATE_IND
    T_APP_RESULT app_gap_callback(uint8_t cb_type, void *p_cb_data)
    {
        T_APP_RESULT result = APP_RESULT_SUCCESS;
        T_LE_CB_DATA cb_data;
        memcpy(&cb_data, p_cb_data, sizeof(T_LE_CB_DATA));
        APP_PRINT_TRACE1("app_gap_callback: cb_type = %d", cb_type);
        switch (cb_type)
        {
        ...
        case GAP_MSG_LE_CONN_UPDATE_IND:
           APP_PRINT_INFO4("GAP_MSG_LE_CONN_UPDATE_IND: conn_interval_max 0x%x, conn_interval_min 0x%x, conn_latency 0x%x
        , supervision_timeout 0x%x",
                           cb_data.p_le_conn_update_ind->conn_interval_max,
                           cb_data.p_le_conn_update_ind->conn_interval_min,
                           cb_data.p_le_conn_update_ind->conn_latency,
                           cb_data.p_le_conn_update_ind->supervision_timeout);
           if (cb_data.p_le_conn_update_ind->conn_latency > 60)
           {
               result = APP_RESULT_REJECT;
           }
           else
           {
               result = APP_RESULT_ACCEPT;
           }
           break;
           ...
        }
    }
  * \endcode
  */
T_GAP_CAUSE le_update_conn_param(uint8_t   conn_id,
                                 uint16_t  conn_interval_min,
                                 uint16_t  conn_interval_max,
                                 uint16_t  conn_latency,
                                 uint16_t  supervision_timeout,
                                 uint16_t  ce_length_min,
                                 uint16_t  ce_length_max);

/** @} */ /* End of group GAP_CONNECTION_COMMON_EXPORT_Functions */
/** @} */ /* End of group GAP_LE_CONNECTION_MODULE */


/*-------------------------------------------------------------------
-------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* GAP_CONN_LE_H */


