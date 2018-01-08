
/** @file
 *
 * Bluetooth L2CAP Application Programming Interface
 *
 * Logical Link Control and Adaptation Layer Protocol,
 * referred to as L2CAP, provides connection oriented and
 * connectionless data services to upper layer protocols with protocol
 * multiplexing capability and segmentation and reassembly operation.
 *
 *  @defgroup    l2cap   Logical Link Control and Adaptaion Protocol (L2CAP)
 *
 */
#pragma once

#include "bt_target.h"
#include "l2cdefs.h"
#include "hcidefs.h"
#include "mico_bt_types.h"
#include "mico_bt_ble.h"


/**
*  @addtogroup  l2cap_data_types        Data Types
*  @ingroup     l2cap
*
*  <b> Data Types </b> for @b Logical Link Control and Adaptation Layer Protocol (L2CAP).
*
*  @{
*/
/*****************************************************************************
 *  Constants
 ****************************************************************************/

/* Define the minimum offset that L2CAP needs in a buffer. This is made up of
 *  HCI type(1), len(2), handle(2), L2CAP len(2) and CID(2) => 9
*/

#define L2CAP_MINIMUM_OFFSET    13      /**< plus control(2), SDU length(2) */

#define L2CAP_BLE_CONN_MIN_OFFSET          9    /**< HCI type(1), len(2), handle(2), L2CAP len(2) and CID(2) */
#define L2CAP_DEFAULT_BLE_CB_POOL_ID    0xFF    /**< Use the default HCI ACL buffer pool */
#define L2CAP_BLE_COC_SDU_OFFSET           4    /**< to provide upper layer some minimal offset possibly required
                                                **  to process incoming packets */
#define L2CAP_BLE_TX_CONG_START_THRESH     3
#define L2CAP_BLE_TX_CONG_STOP_THRESH      1

/** Minimum offset for broadcast needs another two bytes for the PSM */
#define L2CAP_BROADCAST_MIN_OFFSET         11

/** Ping result codes */
#define L2CAP_PING_RESULT_OK            0       /**< Ping reply received OK     */
#define L2CAP_PING_RESULT_NO_LINK       1       /**< Link could not be setup    */
#define L2CAP_PING_RESULT_NO_RESPONSE   2       /**< Remote L2CAP did not reply */

/** Result codes for mico_bt_l2cap_data_write() */
#define L2CAP_DATAWRITE_FAILED        FALSE
#define L2CAP_DATAWRITE_SUCCESS       TRUE
#define L2CAP_DATAWRITE_CONGESTED     2

/** Values for priority parameter to mico_bt_l2cap_set_acl_priority */
#define L2CAP_PRIORITY_NORMAL       0
#define L2CAP_PRIORITY_HIGH         1

/** Values for direction parameter to mico_bt_l2cap_set_acl_priority */
#define L2CAP_DIRECTION_IGNORE              0       /**< Set ACL priority direction as ignore */
#define L2CAP_DIRECTION_DATA_SOURCE         1       /**< Set ACL priority direction as source */
#define L2CAP_DIRECTION_DATA_SINK           2       /**< Set ACL priority direction as sink */


/** Values for priority parameter to mico_bt_l2cap_set_tx_priority */
#define L2CAP_CHNL_PRIORITY_HIGH    0
#define L2CAP_CHNL_PRIORITY_MEDIUM  1
#define L2CAP_CHNL_PRIORITY_LOW     2

typedef uint8_t mico_bt_l2cap_chnl_priority_t;

/** Values for Tx/Rx data rate parameter to mico_bt_l2cap_set_chnl_data_rate */
#define L2CAP_CHNL_DATA_RATE_HIGH       3
#define L2CAP_CHNL_DATA_RATE_MEDIUM     2
#define L2CAP_CHNL_DATA_RATE_LOW        1
#define L2CAP_CHNL_DATA_RATE_NO_TRAFFIC 0

typedef uint8_t mico_bt_l2cap_chnl_data_rate_t;

/** Data Packet Flags  (bits 2-15 are reserved)
 * layer specific 14-15 bits are used for FCR SAR.
 * Used in call to mico_bt_l2cap_data_write()
 */
#define L2CAP_FLUSHABLE_MASK        0x0003
#define L2CAP_FLUSHABLE_CH_BASED    0x0000
#define L2CAP_FLUSHABLE_PACKET      0x0001
#define L2CAP_NON_FLUSHABLE_PACKET  0x0002


/** Used in mico_bt_l2cap_flush_channel num_to_flush definitions */
#define L2CAP_FLUSH_CHANNELS_ALL       0xffff
#define L2CAP_FLUSH_CHANNELS_GET       0x0000

/** Definition used for mico_bt_l2cap_set_desire_role */
#define L2CAP_ROLE_SLAVE            HCI_ROLE_SLAVE
#define L2CAP_ROLE_MASTER           HCI_ROLE_MASTER
#define L2CAP_ROLE_ALLOW_SWITCH     0x80    /**< set this bit to allow switch at create conn */
#define L2CAP_ROLE_DISALLOW_SWITCH  0x40    /**< set this bit to disallow switch at create conn */
#define L2CAP_ROLE_CHECK_SWITCH     0xC0


/** Values for 'allowed_modes' field passed in structure mico_bt_l2cap_ertm_information_t */
#define L2CAP_FCR_CHAN_OPT_BASIC    (1 << L2CAP_FCR_BASIC_MODE)
#define L2CAP_FCR_CHAN_OPT_ERTM     (1 << L2CAP_FCR_ERTM_MODE)
#define L2CAP_FCR_CHAN_OPT_STREAM   (1 << L2CAP_FCR_STREAM_MODE)

#define L2CAP_FCR_CHAN_OPT_ALL_MASK (L2CAP_FCR_CHAN_OPT_BASIC | L2CAP_FCR_CHAN_OPT_ERTM | L2CAP_FCR_CHAN_OPT_STREAM)

/** Validity check for PSM.  PSM values must be odd.  Also, all PSM values must
 *  be assigned such that the least significant bit of the most sigificant
 *  octet equals zero.
*/
#define L2C_INVALID_PSM(psm)    (((psm) & 0x0101) != 0x0001)
#define L2C_IS_VALID_PSM(psm)   (((psm) & 0x0101) == 0x0001)

/** Validity check for LE_PSM.
 *  Fixed LE_PSMs are in the range 0x0001 - 0x007F.
 *  Dynamic LE_PSM are in the range 0x0080 - 0x00FF.
 *  The values 0x0000 and 0x0100 - 0xFFFF are reserved.
*/
#define MINIMIUM_DYNAMIC_LE_PSM 0x0080
#define MAXIMUM_LE_PSM          0x00FF
#define L2C_BLE_INVALID_PSM(le_psm) (!(le_psm) || (le_psm) > MAX_LE_PSM)
#define L2C_BLE_IS_VALID_PSM(le_psm)   (((le_psm) != 0) && ((le_psm) <= MAX_LE_PSM))


/*****************************************************************************
 *  Type Definitions
 ****************************************************************************/

/** Structure for Enhanced Retransmission Mode Options
 *  Refer to Volume 3, Part A, section 5.4 of BT Core specification for details */
typedef struct {
#define L2CAP_FCR_BASIC_MODE    0x00
#define L2CAP_FCR_ERTM_MODE     0x03
#define L2CAP_FCR_STREAM_MODE   0x04

    uint8_t  mode;              /**< Requested mode of link */
    uint8_t  tx_window_size;    /**< Maximum transmit window size (1..63) */
    uint8_t  max_transmit;      /**< Maximum number of trasmission attempts */
    uint16_t rtrans_timeout;    /**< Retransmission timeout (msecs) */
    uint16_t monitor_timeout;   /**< Monitor timeout (msecs) */
    uint16_t max_pdu_size;      /**< Maximum PDU payload size */
} mico_bt_l2cap_fcr_options_t;

/** Define a structure to hold the configuration parameters. Since the
*   parameters are optional, for each parameter there is a boolean to
*   use to signify its presence or absence.
 *  Refer to Volume 3, Part A, section 5.4 of BT Core specification for details
*/
typedef struct {
    uint16_t        result;                 /**< Only used in confirm messages */
    mico_bool_t    mtu_present;            /**< TRUE if MTU option present */
    uint16_t        mtu;                    /**< Maximum transmission unit size */
    mico_bool_t    qos_present;            /**< QoS configuration present */
    mico_bt_flow_spec_t qos;               /**< QoS configuration */
    mico_bool_t    flush_timeout_present;  /**< TRUE if flush option present */
    uint16_t        flush_timeout;          /**< Flush timeout value (1 msec increments) */
    mico_bool_t    fcr_present;            /**< TRUE if Enhanced retransmission & flow control option present */
    mico_bt_l2cap_fcr_options_t fcr;       /**< Enhanced flow control and retransmission parameters */
    mico_bool_t    fcs_present;            /**< TRUE if Frame check sequence option present */
    uint8_t         fcs;                    /**< '0' if desire is to bypass FCS, otherwise '1' */
    uint16_t        flags;                  /**< bit 0: 0-no continuation, 1-continuation */
} mico_bt_l2cap_cfg_information_t;

/* L2CAP channel configured field bitmap */
#define L2CAP_CH_CFG_MASK_MTU           0x0001
#define L2CAP_CH_CFG_MASK_QOS           0x0002
#define L2CAP_CH_CFG_MASK_FLUSH_TO      0x0004
#define L2CAP_CH_CFG_MASK_FCR           0x0008
#define L2CAP_CH_CFG_MASK_FCS           0x0010
#define L2CAP_CH_CFG_MASK_EXT_FLOW_SPEC 0x0020

typedef uint16_t mico_bt_l2cap_ch_cfg_bits_t;


/** Structure that applications use to create or accept
*   connections with enhanced retransmission mode.
*/
typedef struct {
    uint8_t       preferred_mode;     /**< Preferred mode: ERTM, Streaming, or Basic */
    uint8_t       allowed_modes;      /**< Bitmask for allowed modes */
    uint8_t       user_rx_pool_id;    /**< TODO */
    uint8_t       user_tx_pool_id;    /**< TODO */
    uint8_t       fcr_rx_pool_id;     /**< TODO */
    uint8_t       fcr_tx_pool_id;     /**< TODO */

} mico_bt_l2cap_ertm_information_t;

/**@}  Data Types */

/*********************************
 *  Callback Functions Prototypes
 *********************************/


/**
 *  Connection established callback prototype.
 *
 *  @param context          : Caller context provided with mico_bt_l2cap_register()
 *  @param bd_addr          : BD Address of remote
 *  @param local_cid        : Local CID assigned to the connection
 *  @param peer_mtu         : Peer MTU
 *
 *  @return void
*/
typedef void (mico_bt_l2cap_connected_cback_t) (void *context, mico_bt_device_address_t bd_addr, uint16_t local_cid,
                                                uint16_t peer_mtu);

/**
 *  Disconnect indication callback prototype.
 *
 *  @param context          : Caller context provided with mico_bt_l2cap_register()
 *  @param local_cid        : Local CID
 *  @param ack              : Boolean whether upper layer should ack this
 *
 *  @return void
*/
typedef void (mico_bt_l2cap_disconnect_indication_cback_t) (void *context, uint16_t local_cid, mico_bool_t ack);

/**
 *  Disconnect confirm callback prototype.
 *
 *  @param context          : Caller context provided with mico_bt_l2cap_register()
 *  @param local_cid        : Local CID
 *  @param result           : Result
 *
 *  @return void
*/
typedef void (mico_bt_l2cap_disconnect_confirm_cback_t) (void *context, uint16_t local_cid, uint16_t result);

/**
 *  Data received indication callback prototype.
 *
 *  @param context          : Caller context provided with mico_bt_l2cap_register()
 *  @param local_cid        : Local CID
 *  @param p_addr_buff      : Address of buffer
 *
 *  @return void
*/
typedef void (mico_bt_l2cap_data_indication_cback_t) (void *context, uint16_t local_cid, uint8_t *p_buff,
                                                      uint16_t buf_len);

/**
 *  Congestion status callback protype. This callback is optional. If
 *  an application tries to send data when the transmit queue is full,
 *  the data will anyways be dropped.
 *
 *  @param context          : Caller context provided with mico_bt_l2cap_register()
 *  @param local_cid        : Local CID
 *  @param congested        : TRUE if congested, FALSE if uncongested
 *
 *  @return void
*/
typedef void (mico_bt_l2cap_congestion_status_cback_t) (void *context, uint16_t local_cid, mico_bool_t congested);

/**
 *  Callback prototype for number of packets completed events.
 *  This callback notifies the application when Number of Completed Packets (nocp)
 *  event has been received.
 *  This callback is originally designed for 3DG devices.
 *
 *  @param bd_addr          : Peer BD Address
 *
 *  @return void
*/
typedef void (mico_bt_l2cap_nocp_cback_t) (mico_bt_device_address_t bd_addr);

/**
 *  Transmit complete callback protype. This callback is optional. If
 *  set, L2CAP will call it when packets are sent or flushed. If the
 *  count is 0xFFFF, it means all packets are sent for that CID (eRTM
 *  mode only).
 *
 *  @param context          : Caller context provided with mico_bt_l2cap_register()
 *  @param local_cid        : Local CID
 *  @param num_sdu          : Number of SDUs sent or dropped
 *
 *  @return void
*/
typedef void (mico_bt_l2cap_tx_complete_cback_t) (void *context, uint16_t local_cid, uint16_t num_sdu);

/**
 *  Define the structure that applications use to register with
 *  L2CAP. This structure includes callback functions. All functions
 *  MUST be provided, with the exception of the "connect pending"
 *  callback and "congestion status" callback.
 *  Additionally, if registering client for dynamic PSM, connect_ind_cb() must
 *  be NULL since dynamic PSMs use this as a flag for "virtual PSM".
*/
typedef struct {
    mico_bt_l2cap_connected_cback_t             *connected_cback;              /**< BR/EDR connected event */
    mico_bt_l2cap_disconnect_indication_cback_t *disconnect_indication_cback;  /**< BR/EDR disconnect indication event */
    mico_bt_l2cap_disconnect_confirm_cback_t    *disconnect_confirm_cback;     /**< BR/EDR disconnect confirmation event */
    mico_bt_l2cap_data_indication_cback_t       *data_indication_cback;        /**< BR/EDR data received indication */
    mico_bt_l2cap_congestion_status_cback_t     *congestion_status_cback;      /**< Connection (un)congested event */
    mico_bt_l2cap_tx_complete_cback_t           *tx_complete_cback;            /**< BR/EDR transmit complete event */

    uint16_t                        mtu;
    mico_bool_t                    qos_present;
    mico_bt_flow_spec_t            qos;
    mico_bool_t                    flush_timeout_present;
    uint16_t                        flush_timeout;
    mico_bool_t                    fcr_present;
    mico_bt_l2cap_fcr_options_t    fcr;
    mico_bool_t                    fcs_present;
    uint8_t                         fcs;            /**< '0' if desire is to bypass FCS, otherwise '1' */
    mico_bool_t                    is_ob_only;     /**< Set to TRUE if registration is for outbound only to a dynamic PSM */
} mico_bt_l2cap_appl_information_t;

/*
 *
 *                      Fixed Channel callback prototypes
 *
 */

/**
 *  Fixed channel connected and disconnected.
 *
 *  @param bd_addr          : BD Address of remote
 *  @param connected        : TRUE if channel is connected, FALSE if disconnected
 *  @param reason           : Reason for connection failure
 *  @param transport        : Bluetooth Transport (BR/EDR or LE)
 *
 *  @return void
*/
typedef void (mico_bt_l2cap_fixed_chnl_cback_t) (mico_bt_device_address_t bd_addr, mico_bool_t connected,
                                                 uint16_t reason, tBT_TRANSPORT transport);

/**
 *  Signalling data received.
 *
 *  @param bd_addr          : BD Address of remote
 *  @param p_buff           : Pointer to buffer with data
 *
 *  @return void
*/
typedef void (mico_bt_l2cap_fixed_data_cback_t) (mico_bt_device_address_t bd_addr, uint8_t *p_buff, uint16_t buf_len);

/**
 *  Congestion status callback protype. This callback is optional. If
 *  an application tries to send data when the transmit queue is full,
 *  the data will anyways be dropped.
 *
 *  @param bd_addr          : remote mico_bt_device_address_t
 *  @param congested        : TRUE if congested, FALSE if uncongested
 *
 *  @return void
*/
typedef void (mico_bt_l2cap_fixed_congestion_status_cback_t) (mico_bt_device_address_t bd_addr, mico_bool_t congested);

/**
 *  Fixed channel registration info (the callback addresses and channel config)
*/
typedef struct {
    mico_bt_l2cap_fixed_chnl_cback_t               *fixed_conn_cback;  /**< TODO */
    mico_bt_l2cap_fixed_data_cback_t               *fixed_data_cback;  /**< TODO */
    mico_bt_l2cap_fixed_congestion_status_cback_t  *fixed_cong_cback;  /**< TODO */

    uint16_t                                        default_idle_timeout;  /**< TODO */
} mico_bt_l2cap_fixed_chnl_reg_t;

/**
 *  LE Connection indication callback prototype.
 *
 *  @param context          : Caller context provided with mico_bt_l2cap_le_register()
 *  @param bd_addr          : BD Address of remote
 *  @param local_cid        : Local CID assigned to the connection
 *  @param psm              : PSM that the remote wants to connect to
 *  @param id               : Identifier that the remote sent
 *  @param mtu_peer         : MTU of the peer
 *
 *  @return void
*/
typedef void (mico_bt_l2cap_le_connect_indication_cback_t) (void *context, mico_bt_device_address_t bd_addr,
                                                            uint16_t local_cid, uint16_t psm, uint8_t id, uint16_t mtu_peer);


/**
 *  LE Connection confirmation callback prototype.
 *
 *  @param context          : Caller context provided with mico_bt_l2cap_le_register()
 *  @param local_cid        : Local CID
 *  @param result           : Result - 0 = connected, non-zero means failure reason
 *  @param mtu_peer         : MTU of the peer
 *
 *  @return void
*/
typedef void (mico_bt_l2cap_le_connect_confirm_cback_t) (void *context, uint16_t local_cid,
                                                         uint16_t result, uint16_t mtu_peer);


#if (L2CAP_LE_COC_CONFORMANCE_TESTING == TRUE)
/**
 *
 *
 *  @return void
*/
typedef void (mico_bt_l2cap_le_conformance_test_cback_t) (uint16_t);
#endif


/**
 *  Define the structure that applications use to register with
 *  LE L2CAP. This structure includes callback functions. All functions
 *  MUST be provided, with the exception of the "connect pending"
 *  callback and "congestion status" callback.
 *  Additionally, if registering client for dynamic PSM, connect_ind_cb() must
 *  be NULL since dynamic PSMs use this as a flag for "virtual PSM".
*/
typedef struct {
    mico_bt_l2cap_le_connect_indication_cback_t  *le_connect_indication_cback; /**< LE connect indication event */
    mico_bt_l2cap_le_connect_confirm_cback_t     *le_connect_confirm_cback;    /**< LE connect confirm event */
    mico_bt_l2cap_disconnect_indication_cback_t  *disconnect_indication_cback; /**< LE disconnect indication event */
    mico_bt_l2cap_disconnect_confirm_cback_t     *disconnect_confirm_cback;    /**< LE disconnect confirm event */
    mico_bt_l2cap_data_indication_cback_t        *data_indication_cback;       /**< LE data received indication */
    mico_bt_l2cap_congestion_status_cback_t      *congestion_status_cback;     /**< TODO */
#if (L2CAP_LE_COC_CONFORMANCE_TESTING == TRUE)
    mico_bt_l2cap_le_conformance_test_cback_t    *conformance_test_cback;      /**< TODO */
#endif

} mico_bt_l2cap_le_appl_information_t;

/**@} Callback Functions */

/*****************************************************************************
 *  External Function Declarations
 ****************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

/**
*  @addtogroup  l2cap_api_functions       API Functions
*  @ingroup     l2cap
*
*  <b> API Functions </b> module for @b L2CAP.
*
*  @{
*/

#if (L2CAP_NUM_FIXED_CHNLS > 0)
/**
 *
 *  Function        mico_bt_l2cap_register_fixed_channel
 *
 *                  Register a fixed channel.
 *
 *  @param[in]      fixed_cid: Fixed Channel #
 *  @param[in]      p_freg: Channel Callbacks and config
 *
 *  @return:   TRUE if registered OK
 *
 */
mico_bool_t  mico_bt_l2cap_register_fixed_channel (uint16_t fixed_cid, mico_bt_l2cap_fixed_chnl_reg_t *p_freg);


/**
 *
 *  Function        mico_bt_l2cap_connect_fixed_chnl
 *
 *                  Connect an fixed signalling channel to a remote device.
 *
 *  @param[in]      fixed_cid: Fixed CID
 *  @param[in]      bd_addr: BD Address of remote
 *
 *  @return:   TRUE if connection started
 *
 */
mico_bool_t mico_bt_l2cap_connect_fixed_chnl (uint16_t fixed_cid, mico_bt_device_address_t bd_addr);


/**
 *
 *  Function        mico_bt_l2cap_send_fixed_chnl_data
 *
 *                  Write data on a fixed signalling channel.
 *
 *  @param[in]      fixed_cid: Fixed CID
 *  @param[in]      rem_bda: BD Address of remote
 *  @param[in]      p_buf: Pointer to buffer of type BT_HDR
 *
 *  @return         L2CAP_DATAWRITE_SUCCESS, if data accepted
 *                  L2CAP_DATAWRITE_FAILED,  if error
 *
 */
uint16_t mico_bt_l2cap_send_fixed_chnl_data (uint16_t fixed_cid, mico_bt_device_address_t rem_bda,
                                             uint8_t *p_buf, uint16_t buf_len);



/**
 *
 *  Function        mico_bt_l2cap_remove_fixed_chnl
 *
 *                  Remove a fixed channel to a remote device.
 *
 *  @param[in]      fixed_cid: Fixed CID
 *  @param[in]      rem_bda: BD Address of remote
 *                  Idle timeout to use (or 0xFFFF if don't care)
 *
 *  @return:   TRUE if channel removed
 *
 */
mico_bool_t mico_bt_l2cap_remove_fixed_chnl (uint16_t fixed_cid, mico_bt_device_address_t rem_bda);


/**
 *
 *  Function         mico_bt_l2cap_set_fixed_channel_timeout
 *
 *                  Higher layers call this function to set the idle timeout for
 *                  a fixed channel. The "idle timeout" is the amount of time that
 *                  a connection can remain up with no L2CAP channels on it.
 *                  A timeout of zero means that the connection will be torn
 *                  down immediately when the last channel is removed.
 *                  A timeout of 0xFFFF means no timeout. Values are in seconds.
 *                  A bd_addr is the remote BD address. If bd_addr = BT_BD_ANY,
 *                  then the idle timeouts for all active l2cap links will be
 *                  changed.
 *
 *  @param[in]      rem_bda         : Remote BD address
 *  @param[in]      fixed_cid       : Fixed CID
 *  @param[in]      idle_timeout    : Idle timeout
 *
 *  @return         TRUE if command succeeded, FALSE if failed
 *
 */
mico_bool_t mico_bt_l2cap_set_fixed_channel_timeout (mico_bt_device_address_t rem_bda, uint16_t fixed_cid,
                                                     uint16_t idle_timeout);


#endif /* (L2CAP_NUM_FIXED_CHNLS > 0) */

/**
 *
 *  Function     mico_bt_l2cap_get_current_config
 *
 *              This function returns configurations of L2CAP channel
 *
 *  @param[in]      lcid: Local CID
 *  @param[in]      pp_our_cfg: pointer of our saved configuration options
 *  @param[in]      p_our_cfg_bits: valid config in bitmap
 *  @param[in]      pp_peer_cfg: pointer of peer's saved configuration options
 *  @param[in]      p_peer_cfg_bits : valid config in bitmap
 *
 *  @return     TRUE if successful
 *
 */
mico_bool_t mico_bt_l2cap_get_current_config (uint16_t lcid,
                                              mico_bt_l2cap_cfg_information_t **pp_our_cfg,  mico_bt_l2cap_ch_cfg_bits_t *p_our_cfg_bits,
                                              mico_bt_l2cap_cfg_information_t **pp_peer_cfg, mico_bt_l2cap_ch_cfg_bits_t *p_peer_cfg_bits);


/**
 *
 *  Function         mico_bt_l2cap_register
 *
 *                  Other layers call this function to register for L2CAP
 *                  services.
 *
 *  @param[in]      psm: PSM value
 *  @param[in]      p_cb_info: L2CAP cb info
 *  @param[in]      context: Caller context to return in callbacks
 *
 *  @return         PSM to use or zero if error. Typically, the PSM returned
 *                  is the same as was passed in, but for an outgoing-only
 *                  connection to a dynamic PSM, a "virtual" PSM is returned
 *                  and should be used in the calls to mico_bt_l2cap_connect_req()
 *                  and BTM_SetSecurityLevel().
 *
 */
uint16_t mico_bt_l2cap_register (uint16_t psm, mico_bt_l2cap_appl_information_t *p_cb_information,  void *context);


/**
 *
 *  Function         mico_bt_l2cap_deregister
 *
 *                  Other layers call this function to deregister for L2CAP
 *                  services.
 *
 *  @param[in]      psm: PSM value
 *
 *  @return         void
 *
 */
void mico_bt_l2cap_deregister (uint16_t psm);


/**
 *
 *  Function         mico_bt_l2cap_allocate_psm
 *
 *                  Other layers call this function to find an unused PSM for
 *                  L2CAP services.
 *
 *  @return         PSM to use.
 *
 */
uint16_t mico_bt_l2cap_allocate_psm (void);


/**
 *
 *  Function         mico_bt_l2cap_connect_req
 *
 *                  Higher layers call this function to create an L2CAP connection.
 *                  Note that the connection is not established at this time, but
 *                  connection establishment gets started. The callback function
 *                  will be invoked when connection establishes or fails.
 *
 *  @param[in]      psm                 : PSM value
 *  @param[in]      p_bd_addr           : BD Address
 *  @param[in]      p_ertm_information  : ERTM info
 *
 *  @return         the CID of the connection, or 0 if it failed to start
 *
 */
uint16_t mico_bt_l2cap_connect_req (uint16_t psm, mico_bt_device_address_t p_bd_addr,
                                    mico_bt_l2cap_ertm_information_t *p_ertm_information);


/**
 *
 * Function         mico_bt_l2cap_ertm_enable
 *
 * Description      Enable ERTM.
 *
 *                  Calling this function will cause the linker to include
 *                  ERTM related functions.
 *
 * @param[in]       void
 *
 * @return          void
 *
 */
void mico_bt_l2cap_ertm_enable (void);


/**
 *
 *  Function         mico_bt_l2cap_ertm_connect_req
 *
 *                  Higher layers call this function to create an L2CAP connection
 *                  that needs to use Enhanced Retransmission Mode.
 *                  Note that the connection is not established at this time, but
 *                  connection establishment gets started. The callback function
 *                  will be invoked when connection establishes or fails.
 *
 *  @param[in]      psm: PSM value
 *  @param[in]      p_bd_addr: BD Address
 *  @param[in]      p_ertm_info: ERTM info
 *
 *  @return         the CID of the connection, or 0 if it failed to start
 *
 */
uint16_t mico_bt_l2cap_ertm_connect_req (uint16_t psm, mico_bt_device_address_t p_bd_addr,
                                         mico_bt_l2cap_ertm_information_t *p_ertm_information);


/**
 *
 *  Function         mico_bt_l2cap_disconnect_req
 *
 *                  Higher layers call this function to disconnect a channel.
 *
 *  @param[in]      cid: CID value
 *
 *  @return         TRUE if disconnect sent, else FALSE
 *
 */
mico_bool_t mico_bt_l2cap_disconnect_req (uint16_t cid);


/**
 *
 *  Function         mico_bt_l2cap_disconnect_rsp
 *
 *                  Higher layers call this function to acknowledge the
 *                  disconnection of a channel.
 *
 *  @param[in]      cid: CID value
 *
 *  @return         void
 *
 */
mico_bool_t mico_bt_l2cap_disconnect_rsp (uint16_t cid);


/**
 *
 *  Function        mico_bt_l2cap_data_write
 *
 *                  Higher layers call this function to write data with extended
 *
 *  @param[in]      cid: TODO
 *  @param[in]      p_data: TODO
 *  @param[in]      flags: L2CAP_FLUSHABLE_CH_BASED
 *                         L2CAP_FLUSHABLE_PACKET
 *                         L2CAP_NON_FLUSHABLE_PACKET
 *
 *  @return         L2CAP_DATAWRITE_SUCCESS, if data accepted, else FALSE
 *                  L2CAP_DATAWRITE_CONGESTED, if data accepted and the channel is congested
 *                  L2CAP_DATAWRITE_FAILED, if error
 *
 */
uint8_t mico_bt_l2cap_data_write (uint16_t cid, uint8_t *p_buf, uint16_t buf_len, uint16_t flags);

/**
 *
 *  Function         mico_bt_l2cap_set_idle_timeout
 *
 *                  Higher layers call this function to set the idle timeout for
 *                  a connection, or for all future connections. The "idle timeout"
 *                  is the amount of time that a connection can remain up with
 *                  no L2CAP channels on it. A timeout of zero means that the
 *                  connection will be torn down immediately when the last channel
 *                  is removed. A timeout of 0xFFFF means no timeout. Values are
 *                  in seconds.
 *
 *  @param[in]      cid: CID value
 *  @param[in]      timeout: Timeout value
 *  @param[in]      is_global: TRUE, if global
 *
 *  @return         TRUE if command succeeded, FALSE if failed
 *
 */
mico_bool_t mico_bt_l2cap_set_idle_timeout (uint16_t cid, uint16_t timeout,
                                            mico_bool_t is_global);


/**
 *
 *  Function         mico_bt_l2cap_set_idle_timeout_by_bd_addr
 *
 *                  Higher layers call this function to set the idle timeout for
 *                  a connection. The "idle timeout" is the amount of time that
 *                  a connection can remain up with no L2CAP channels on it.
 *                  A timeout of zero means that the connection will be torn
 *                  down immediately when the last channel is removed.
 *                  A timeout of 0xFFFF means no timeout. Values are in seconds.
 *                  A bd_addr is the remote BD address. If bd_addr = BT_BD_ANY,
 *                  then the idle timeouts for all active l2cap links will be
 *                  changed.
 *
 *  @param[in]      bd_addr: BD Address
 *  @param[in]      timeout: Timeout value
 *
 *  @return         TRUE if command succeeded, FALSE if failed
 *
 *  NOTE             This timeout applies to all logical channels active on the
 *                  ACL link.
 */
mico_bool_t mico_bt_l2cap_set_idle_timeout_by_bd_addr (mico_bt_device_address_t bd_addr, uint16_t timeout,
                                                       tBT_TRANSPORT transport);

/**
 *
 *  Function         mico_bt_l2cap_set_trace_level
 *
 *                  This function sets the trace level for L2CAP. If called with
 *                  a value of 0xFF, it simply reads the current trace level.
 *
 *  @param[in]      trace_level: Trace level
 *
 *  @return         the new (current) trace level
 *
 */
uint8_t mico_bt_l2cap_set_trace_level (uint8_t trace_level);


/**
 *
 *  Function     mico_bt_l2cap_set_desire_role
 *
 *              This function sets the desire role for L2CAP.
 *              If the new role is L2CAP_ROLE_ALLOW_SWITCH, allow switch on
 *              HciCreateConnection.
 *              If the new role is L2CAP_ROLE_DISALLOW_SWITCH, do not allow switch on
 *              HciCreateConnection.
 *
 *              If the new role is a valid role (HCI_ROLE_MASTER or HCI_ROLE_SLAVE),
 *              the desire role is set to the new value. Otherwise, it is not changed.
 *
 *  @param[in]      new_role: New role value
 *
 *  @return     the new (current) role
 *
 */
uint8_t mico_bt_l2cap_set_desire_role (uint8_t new_role);


/**
 *
 *  Function     mico_bt_l2cap_flush_channel
 *
 *              This function flushes none, some or all buffers queued up
 *              for xmission for a particular CID. If called with
 *              L2CAP_FLUSH_CHANNELS_GET (0), it simply returns the number
 *              of buffers queued for that CID L2CAP_FLUSH_CHANNELS_ALL (0xffff)
 *              flushes all buffers.  All other values specifies the maximum
 *              buffers to flush.
 *
 *  @param[in]      lcid: LCID value
 *  @param[in]      num_to_flush: Number of items for flushing
 *
 *  @return     Number of buffers left queued for that CID
 *
 */
uint16_t   mico_bt_l2cap_flush_channel (uint16_t lcid, uint16_t num_to_flush);


/**
 *
 *  Function         mico_bt_l2cap_set_acl_priority
 *
 *                  Sets the priority for an ACL channel
 *
 *  @param[in]      bd_addr: BD Address
 *  @param[in]      priority: [L2CAP_PRIORITY_NORMAL | L2CAP_PRIORITY_HIGH]
 *
 *  @return         TRUE if a valid channel, else FALSE
 *
 */
mico_bool_t mico_bt_l2cap_set_acl_priority (mico_bt_device_address_t bd_addr, uint8_t priority);

/**
 *
 *  Function         mico_bt_l2cap_set_acl_priority_ext
 *
 *                  Sets the priority for an ACL channel
 *                  with extended parameters.
 *
 *  @param[in]      bd_addr: BD Address
 *  @param[in]      priority: [L2CAP_PRIORITY_NORMAL | L2CAP_PRIORITY_HIGH]
 *  @param[in]      direction: [L2CAP_DIRECTION_DATA_SOURCE | L2CAP_DIRECTION_DATA_SINK]
 *
 *  @return         TRUE if a valid channel, else FALSE
 *
 */
mico_bool_t mico_bt_l2cap_set_acl_priority_ext (mico_bt_device_address_t bd_addr, uint8_t priority, uint8_t direction);


/**
 *
 *  Function         mico_bt_l2cap_flow_control
 *
 *                  Higher layers call this function to flow control a channel.
 *
 *                  data_enabled - TRUE data flows, FALSE data is stopped
 *
 *  @param[in]      cid: CID value
 *  @param[in]      data_enabled: data enabled
 *
 *  @return         TRUE if valid channel, else FALSE
 *
 */
mico_bool_t mico_bt_l2cap_flow_control (uint16_t cid, mico_bool_t data_enabled);


/**
 *
 *  Function         mico_bt_l2cap_set_tx_priority
 *
 *                  Sets the transmission priority for a channel. (FCR Mode)
 *
 *  @param[in]      cid: CID
 *  @param[in]      priority: [L2CAP_PRIORITY_NORMAL | L2CAP_PRIORITY_HIGH]

 *
 *  @return         TRUE if a valid channel, else FALSE
 *
 */
mico_bool_t mico_bt_l2cap_set_tx_priority (uint16_t cid, mico_bt_l2cap_chnl_priority_t priority);


/**
 *
 *  Function         mico_bt_l2cap_register_for_nocp_evt
 *
 *                   Register callback for Number of Completed Packets (NOCP) event.
 *
 *  Input Param      p_cb - callback for Number of completed packets event
 *                   p_bda - BT address of remote device
 *
 *  @return         TRUE successful
 *
 */
mico_bool_t mico_bt_l2cap_register_for_nocp_evt (mico_bt_l2cap_nocp_cback_t *p_cb, mico_bt_device_address_t p_bda);

/**
 *
 *  Function         mico_bt_l2cap_set_flush_timeout
 *
 *                  This function set the automatic flush time out in Baseband
 *                  for ACL-U packets.
 *
 *  @param[in]      bd_addr: The remote BD address of ACL link. If it is BT_DB_ANY
 *                           then the flush time out will be applied to all ACL link.
 *  @param[in]      flush_timeout: flush time out in ms
 *                           0x0000 : No automatic flush
 *                           L2CAP_NO_RETRANSMISSION : No retransmission
 *                           0x0002 - 0xFFFE : flush time out, if (flush_timeout*8)+3/5)
 *                                    <= HCI_MAX_AUTO_FLUSH_TOUT (in 625us slot).
 *                                    Otherwise, return FALSE.
 *                           L2CAP_NO_AUTOMATIC_FLUSH : No automatic flush
 *
 *  @return         TRUE if command succeeded, FALSE if failed
 *
 *  NOTE             This flush timeout applies to all logical channels active on the
 *                  ACL link.
 */
mico_bool_t mico_bt_l2cap_set_flush_timeout (mico_bt_device_address_t bd_addr, uint16_t flush_timeout);




/**
 *
 *  Function         mico_bt_l2cap_set_chnl_flushability
 *
 *                  Higher layers call this function to set a channels
 *                  flushability flags
 *
 *  @param[in]      cid: CID value
 *  @param[in]      is_flushable: TRUE, if flushable
 *
 *  @return         TRUE if CID found, else FALSE
 *
 */
mico_bool_t mico_bt_l2cap_set_chnl_flushability (uint16_t cid, mico_bool_t is_flushable);


/**
 *
 *  Function         mico_bt_l2cap_get_peer_features
 *
 *                   Get a peers features and fixed channel map
 *
 *  @param[in]      bd_addr: Peer Bd Address
 *  @param[in]      p_ext_feat: features
 *  @param[in]      p_chnl_mask: mask storage area
 *
 *  @return:    TRUE if peer is connected
 *
 */
mico_bool_t mico_bt_l2cap_get_peer_features (mico_bt_device_address_t bd_addr, uint32_t *p_ext_feat,
                                             uint8_t *p_chnl_mask);


/**
 *
 *  Function         mico_bt_l2cap_get_bdaddrby_handle
 *
 *                   Get BD address for the given HCI handle
 *
 *  @param[in]      handle: HCI handle
 *  @param[in]      bd_addr: Peer Bd Address
 *
 *  @return:    TRUE if found lcb for the given handle, FALSE otherwise
 *
 */
mico_bool_t mico_bt_l2cap_get_bdaddrby_handle (uint16_t handle, mico_bt_device_address_t bd_addr);


/**
 *
 *  Function         mico_bt_l2cap_get_chnl_fcr_mode
 *
 *                   Get the channel FCR mode
 *
 *  @param[in]      lcid: Local CID
 *
 *  @return:    Channel mode
 *
 */
uint8_t mico_bt_l2cap_get_chnl_fcr_mode (uint16_t lcid);


/**
 *
 *  Function        mico_bt_l2cap_cancel_ble_connect_req
 *
 *                  Cancel a pending connection attempt to a BLE device.
 *
 *  @param[in]      rem_bda: BD Address of remote
 *
 *  @return:   TRUE if connection was cancelled
 *
 */
mico_bool_t mico_bt_l2cap_cancel_ble_connect_req (mico_bt_device_address_t rem_bda);


/**
 *
 *  Function        mico_bt_l2cap_update_ble_conn_params
 *
 *                  Update BLE connection parameters.
 *
 *  @param[in]      rem_bdRa: Remote BD Address
 *  @param[in]      min_int: Min interval
 *  @param[in]      max_int: Max interval
 *  @param[in]      latency: Latency value
 *  @param[in]      timeout: Timeout value
 *
 *  @return:   TRUE if update started
 *
 */
mico_bool_t mico_bt_l2cap_update_ble_conn_params (mico_bt_device_address_t rem_bdRa, uint16_t min_int, uint16_t max_int,
                                                  uint16_t latency, uint16_t timeout);


/**
 *
 *  Function        mico_bt_l2cap_enable_update_ble_conn_params
 *
 *                  Update BLE connection parameters.
 *
 *  @param[in]      rem_bda: Remote Bd Address
 *  @param[in]      enable: Enable Flag
 *
 *  @return:   TRUE if update started
 *
 */
mico_bool_t mico_bt_l2cap_enable_update_ble_conn_params (mico_bt_device_address_t rem_bda, mico_bool_t enable);


/**
 *
 *  Function         mico_bt_l2cap_get_ble_conn_role
 *
 *                  This function returns the connection role.
 *
 *  @param[in]      bd_addr: BD Address
 *
 *  @return         link role.
 *
 */
uint8_t mico_bt_l2cap_get_ble_conn_role (mico_bt_device_address_t bd_addr);


/**
 *
 *  Function         mico_bt_l2cap_get_disconnect_reason
 *
 *                  This function returns the disconnect reason code.
 *
 *  @param[in]      remote_bda: Remote BD Address
 *  @param[in]      transport: Transport (BR-EDR or LE)
 *
 *  @return         disconnect reason
 *
 */
uint16_t mico_bt_l2cap_get_disconnect_reason (mico_bt_device_address_t remote_bda, tBT_TRANSPORT transport);


/**
 *
 *  Function         mico_bt_l2cap_le_register
 *
 *                  Other layers call this function to register L2CAP services
 *                  for LE_PSM.
 *
 *  @param[in]      le_psm: LE PSM value
 *  @param[in]      p_cb_info: L2CAP cb info
 *  @param[in]      context: Caller context to return in callbacks
 *
 *  @return         LE_PSM to use or zero if error. Typically the LE_PSM returned
 *                  is the same as was passed in, but for an outgoing-only
 *                  connection a "virtual" LE_PSM is returned  and should be used
 *                  in the calls to mico_bt_l2cap_le_connect_req() and mico_bt_l2cap_le_deregister().
 *
 */
uint16_t mico_bt_l2cap_le_register (uint16_t le_psm, mico_bt_l2cap_le_appl_information_t *p_cb_information,
                                    void *context);


/**
 *
 *  Function         mico_bt_l2cap_le_deregister
 *
 *                  Other layers call this function to deregister L2CAP services
 *                  for LE_PSM.
 *
 *  @param[in]      le_psm: LE PSM value
 *
 *  @return         void
 *
 */
mico_bool_t mico_bt_l2cap_le_deregister (uint16_t le_psm);


/**
 *
 *  Function         mico_bt_l2cap_le_connect_req
 *
 *                  Higher layers call this function to create an L2CAP connection
 *                  for LE_PSM.
 *                  Note that the connection is not established at this time, but
 *                  connection establishment gets started. The callback function
 *                  will be invoked when connection establishes or fails.
 *
 *  @param[in]      le_psm              : LE PSM value
 *  @param[in]      p_bd_addr           : BD Address
 *  @param[in]      bd_addr_type        : BLE_ADDR_PUBLIC or BLE_ADDR_RANDOM
 *  @param[in]      conn_mode           : BLE_CONN_MODE_HIGH_DUTY or BLE_CONN_MODE_LOW_DUTY
 *  @param[in]      rx_mtu              : Rx MTU value (must be <= ACL_POOL_SIZE)
 *  @param[in]      rx_sdu_pool_id      : Rx SDU pool ID (typically L2CAP_DEFAULT_BLE_CB_POOL_ID)
 *  @param[in]      req_security        : Security required
 *  @param[in]      req_encr_key_size   : key size
 *
 *  @return         the CID of the connection, or 0 if it failed to start
 *
 */
uint16_t mico_bt_l2cap_le_connect_req (uint16_t le_psm, mico_bt_device_address_t p_bd_addr,
                                       mico_bt_ble_address_type_t bd_addr_type,
                                       mico_bt_ble_conn_mode_t conn_mode,
                                       uint16_t rx_mtu, uint8_t rx_sdu_pool_id,
                                       uint8_t req_security, uint8_t req_encr_key_size);


/**
 *
 *  Function         mico_bt_l2cap_le_connect_rsp
 *
 *                  Higher layers call this function to accept an incoming
 *                  LE L2CAP connection, for which they had gotten an connect
 *                  indication callback.
 *
 *  @param[in]      p_bd_addr: TODO
 *  @param[in]      id: TODO
 *  @param[in]      lcid: TODO
 *  @param[in]      result: TODO
 *  @param[in]      rx_mtu: TODO
 *  @param[in]      rx_sdu_pool_id: TODO
 *
 *  @return         TRUE for success, FALSE for failure
 *
 */
mico_bool_t  mico_bt_l2cap_le_connect_rsp (mico_bt_device_address_t p_bd_addr, uint8_t id, uint16_t lcid,
                                           uint16_t result, uint16_t rx_mtu, uint8_t rx_sdu_pool_id);


/**
 *
 *  Function         mico_bt_l2cap_le_disconnect_req
 *
 *                  Higher layers call this function to disconnect a LE COC
 *                  channel.
 *
 *  @param[in]      lcid: LCID value
 *
 *  @return         TRUE if disconnect sent, else FALSE
 *
 */
mico_bool_t mico_bt_l2cap_le_disconnect_req (uint16_t lcid);


/**
 *
 *  Function         mico_bt_l2cap_le_disconnect_rsp
 *
 *                  Higher layers call this function to acknowledge the
 *                  disconnection of a LE COC channel.
 *
 *  @param[in]      lcid: TODO
 *
 *  @return         void
 *
 */
mico_bool_t mico_bt_l2cap_le_disconnect_rsp (uint16_t lcid);


/**
 *
 *  Function         mico_bt_l2cap_le_data_write
 *
 *                  Higher layers call this function to write data.
 *
 *  @param[in]      cid: Channel ID
 *  @param[in]      p_data: Input buffer
 *  @param[in]      buf_len: p_data buffer size
 *
 *  @return         L2CAP_DATAWRITE_SUCCESS, if data accepted, else FALSE
 *                  L2CAP_DATAWRITE_CONGESTED, if data accepted and the channel is congested
 *                  L2CAP_DATAWRITE_FAILED, if error
 *
 */
uint8_t mico_bt_l2cap_le_data_write (uint16_t cid, uint8_t *p_data, uint16_t buf_len, uint16_t flags);


/**
 *
 *  Function         mico_bt_l2cap_le_set_user_congestion
 *
 *                  Higher layers call this function to tell if the connection
 *                  is congested or not
 *
 *  @param[in]      lcid: LCID value
 *  @param[in]      is_congested: TRUE, if congested
 *
 *  @return         TRUE if command processed OK
 *
 */
mico_bool_t  mico_bt_l2cap_le_set_user_congestion (uint16_t lcid, mico_bool_t is_congested);


/**
 *
 *  Function         mico_bt_l2cap_le_get_peer_mtu
 *
 *                  Higher layers call this function to get peer MTU.
 *
 *  @param[in]      lcid: LCID value
 *
 *  @return         Peer MTU or 0.
 *
 */
uint16_t mico_bt_l2cap_le_get_peer_mtu (uint16_t lcid);


/**
 *
 *  Function         mico_bt_l2cap_le_determ_secur_rsp
 *
 *                  Higher layers call this function to check if the current
 *                  device security settings are sufficient to continue with
 *                  call establishment.
 *                  It is called by call acceptor on reception of LE Credit
 *                  Based Connection Request.
 *
 *  @param[in]      bd_addr: BD Address
 *  @param[in]      req_secur: Security required
 *  @param[in]      req_encr_key_size: Key size
 *
 *  @return         L2CAP_CONN_OK/L2CAP_BLE_CONN_BAD_AUTHENT/
 *                  L2CAP_BLE_CONN_BAD_KEY_SIZE/L2CAP_BLE_CONN_BAD_ENCRYPT/
 *                  L2CAP_CONN_NO_RESOURCES.
 *
 */
uint16_t mico_bt_l2cap_le_determ_secur_rsp (mico_bt_device_address_t bd_addr, uint8_t req_secur,
                                            uint8_t req_encr_key_size);


/**@} l2cap_api_functions */

#ifdef __cplusplus
}
#endif
