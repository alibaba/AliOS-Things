#ifndef CO_LLCP_H_
#define CO_LLCP_H_

/**
 ****************************************************************************************
 * @addtogroup CO_BT
 * @{
 ****************************************************************************************
 */


/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "co_bt.h"
/*
 * DEFINES
 ****************************************************************************************
 */
#define LLCP_OPCODE_MASK      (0xFF)
#define LLCP_OPCODE_POS       (0)
/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */
/// Control packet op_code
enum co_llcp_op_code
{
    /// Connection update request
    LLCP_CONNECTION_UPDATE_IND_OPCODE,
    /// Channel map request
    LLCP_CHANNEL_MAP_IND_OPCODE,
    /// Termination indication
    LLCP_TERMINATE_IND_OPCODE,
    /// Encryption request
    LLCP_ENC_REQ_OPCODE,
    /// Encryption response
    LLCP_ENC_RSP_OPCODE,
    /// Start encryption request
    LLCP_START_ENC_REQ_OPCODE,
    /// Start encryption response
    LLCP_START_ENC_RSP_OPCODE,
    /// Unknown response
    LLCP_UNKNOWN_RSP_OPCODE,
    /// Feature request
    LLCP_FEATURE_REQ_OPCODE,
    /// Feature response
    LLCP_FEATURE_RSP_OPCODE,
    /// Pause encryption request
    LLCP_PAUSE_ENC_REQ_OPCODE,
    /// Pause encryption response
    LLCP_PAUSE_ENC_RSP_OPCODE,
    /// Version indication
    LLCP_VERSION_IND_OPCODE,
    /// Reject indication
    LLCP_REJECT_IND_OPCODE,
    /// Slave feature request
    LLCP_SLAVE_FEATURE_REQ_OPCODE,
    /// Connection parameters request
    LLCP_CONNECTION_PARAM_REQ_OPCODE,
    /// Connection parameters response
    LLCP_CONNECTION_PARAM_RSP_OPCODE,
    /// Reject indication extended
    LLCP_REJECT_IND_EXT_OPCODE,
    /// Ping request
    LLCP_PING_REQ_OPCODE,
    /// Ping response
    LLCP_PING_RSP_OPCODE,
    /// Ping request
    LLCP_LENGTH_REQ_OPCODE,
    /// Ping response
    LLCP_LENGTH_RSP_OPCODE,
    /// Phy request
    LLCP_PHY_REQ_OPCODE,
    /// Phy response
    LLCP_PHY_RSP_OPCODE,
    /// Phy update indication
    LLCP_PHY_UPD_IND_OPCODE,
    /// Min used channels indication
    LLCP_MIN_USED_CHANNELS_IND_OPCODE,
    /// Opcode length
    LLCP_OPCODE_MAX_OPCODE,

    #if (BLE_TESTER)
    LLCP_OPCODE_DEBUG = 0xFF,
    #endif // (BLE_TESTER)
};

/// LLCP PDU lengths (including op_code)
enum co_llcp_length
{
   LLCP_CON_UPDATE_IND_LEN    = 12,
   LLCP_CHANNEL_MAP_IND_LEN   = 8,
   LLCP_TERM_IND_LEN          = 2,
   LLCP_ENC_REQ_LEN           = 23,
   LLCP_ENC_RSP_LEN           = 13,
   LLCP_ST_ENC_REQ_LEN        = 1,
   LLCP_ST_ENC_RSP_LEN        = 1,
   LLCP_UNKN_RSP_LEN          = 2,
   LLCP_FEAT_REQ_LEN          = 9,
   LLCP_FEAT_RSP_LEN          = 9,
   LLCP_PA_ENC_REQ_LEN        = 1,
   LLCP_PA_ENC_RSP_LEN        = 1,
   LLCP_VERS_IND_LEN          = 6,
   LLCP_REJ_IND_LEN           = 2,
   LLCP_SLAVE_FEATURE_REQ_LEN = 9,
   LLCP_REJECT_IND_EXT_LEN    = 3,
   LLCP_CON_PARAM_REQ_LEN     = 24,
   LLCP_CON_PARAM_RSP_LEN     = 24,
   LLCP_PING_REQ_LEN          = 1,
   LLCP_PING_RSP_LEN          = 1,
   LLCP_LENGTH_REQ_LEN        = 9,
   LLCP_LENGTH_RSP_LEN        = 9,
   LLCP_PHY_REQ_LEN           = 3,
   LLCP_PHY_RSP_LEN           = 3,
   LLCP_PHY_UPD_IND_LEN       = 5,
   LLCP_MIN_USED_CH_IND_LEN   = 3,
   LLCP_PDU_LENGTH_MAX        = 34
};

/// PDU lengths
enum co_pdu_length
{
   PDU_SCAN_REQ_LEN          = 12,
   PDU_CON_REQ_LEN           = 34,
   PDU_CON_RSP_LEN           = 14,
};

/*
 * MESSAGES
 ****************************************************************************************
 */

/// LLCP_CONNECTION_UPDATE_IND structure.
struct  llcp_con_update_ind
{
    /// op_code
    uint8_t         op_code;
    /// window size (units of 1.25 ms)
    uint8_t         win_size;
    /// window offset (units of 1.25 ms)
    uint16_t        win_off;
    /// interval (units of 1.25 ms)
    uint16_t        interv;
    /// connection latency (unit of connection event)
    uint16_t        latency;
    /// link supervision timeout (unit of 10 ms)
    uint16_t        timeout;
    /// instant (unit of connection event)
    uint16_t        instant;
};

/// LLCP_CHANNEL_MAP_IND structure.
struct  llcp_channel_map_ind
{
    /// op_code
    uint8_t            op_code;
    /// channel mapping
    struct le_chnl_map ch_map;
    /// instant
    uint16_t           instant;
};

/// LLCP_TERMINATE_IND structure.
struct  llcp_terminate_ind
{
    /// op_code
    uint8_t         op_code;
    /// termination code
    uint8_t         err_code;
};

/// LLCP_ENC_REQ structure.
struct  llcp_enc_req
{
    /// op_code
    uint8_t               op_code;
    /// random value
    struct rand_nb        rand;
    /// ediv
    uint16_t              ediv;
    /// skdm
    struct sess_k_div_x   skdm;
    /// ivm
    struct init_vect      ivm;
};

/// LLCP_ENC_RSP structure.
struct  llcp_enc_rsp
{
    /// op_code
    uint8_t             op_code;
    /// skds
    struct sess_k_div_x   skds;
    /// ivs
    struct init_vect    ivs;
};

/// LLCP_START_ENC_REQ structure.
struct  llcp_start_enc_req
{
    /// op_code
    uint8_t             op_code;
};

/// LLCP_START_ENC_RSP structure.
struct  llcp_start_enc_rsp
{
    /// op_code
    uint8_t             op_code;
};

/// LLCP_UNKNOWN_RSP structure.
struct  llcp_unknown_rsp
{
    /// op_code
    uint8_t         op_code;
    /// unknown type
    uint8_t         unk_type;
};

/// LLCP_FEATURE_REQ structure.
struct  llcp_feats_req
{
    /// op_code
    uint8_t             op_code;
    /// le features
    struct le_features  feats;
};


/// LLCP_FEATURE_RSP structure.
struct  llcp_feats_rsp
{
    /// op_code
    uint8_t             op_code;
    /// le features
    struct le_features  feats;
};

/// LLCP_PAUSE_ENC_REQ structure.
struct  llcp_pause_enc_req
{
    /// op_code
    uint8_t             op_code;
};

/// LLCP_PAUSE_ENC_RSP structure.
struct  llcp_pause_enc_rsp
{
    /// op_code
    uint8_t             op_code;
};

/// LLCP_VERS_IND structure
struct llcp_vers_ind
{
    /// op_code
    uint8_t     op_code;
    /// version
    uint8_t     vers;
    /// company id
    uint16_t    compid;
    /// sub version
    uint16_t    subvers;
};

/// LLCP_REJECT_IND structure.
struct  llcp_reject_ind
{
    /// op_code
    uint8_t         op_code;
    /// reject reason
    uint8_t         err_code;
};

/// LLCP_SLAVE_FEATURE_REQ structure.
struct  llcp_slave_feature_req
{
    /// op_code
    uint8_t             op_code;
    /// le features
    struct le_features  feats;
};

/// LLCP_CONNECTION_PARAM_REQ structure.
struct  llcp_con_param_req
{
    /// op_code
    uint8_t         op_code;
    /// minimum value of connInterval (units of 1.25 ms)
    uint16_t        interval_min;
    /// maximum value of connInterval (units of 1.25 ms)
    uint16_t        interval_max;
    /// connSlaveLatency value (unit of connection event)
    uint16_t        latency;
    /// connSupervisionTimeout value (unit of 10 ms)
    uint16_t        timeout;
    /// preferred periodicity (units of 1.25 ms)
    uint8_t         pref_period;
    /// ReferenceConnEventCount (unit of connection event)
    uint16_t        ref_con_event_count;
    /// Offset0 (units of 1.25 ms)
    uint16_t        offset0;
    /// Offset1 (units of 1.25 ms)
    uint16_t        offset1;
    /// Offset2 (units of 1.25 ms)
    uint16_t        offset2;
    /// Offset3 (units of 1.25 ms)
    uint16_t        offset3;
    /// Offset4 (units of 1.25 ms)
    uint16_t        offset4;
    /// Offset5 (units of 1.25 ms)
    uint16_t        offset5;
};

/// LLCP_CONNECTION_PARAM_RSP structure.
struct  llcp_con_param_rsp
{
    /// op_code
    uint8_t          op_code;
    /// minimum value of connInterval (units of 1.25 ms)
    uint16_t        interval_min;
    /// maximum value of connInterval (units of 1.25 ms)
    uint16_t        interval_max;
    /// connSlaveLatency value (unit of connection event)
    uint16_t        latency;
    /// connSupervisionTimeout value (unit of 10 ms)
    uint16_t        timeout;
    /// preferred periodicity (units of 1.25 ms)
    uint8_t         pref_period;
    /// ReferenceConnEventCount (unit of connection event)
    uint16_t        ref_con_event_count;
    /// Offset0 (units of 1.25 ms)
    uint16_t        offset0;
    /// Offset1 (units of 1.25 ms)
    uint16_t        offset1;
    /// Offset2 (units of 1.25 ms)
    uint16_t        offset2;
    /// Offset3 (units of 1.25 ms)
    uint16_t        offset3;
    /// Offset4 (units of 1.25 ms)
    uint16_t        offset4;
    /// Offset5 (units of 1.25 ms)
    uint16_t        offset5;
};

/// LLCP_REJECT_IND structure.
struct  llcp_reject_ind_ext
{
    /// op_code
    uint8_t         op_code;
    /// rejected op_code
    uint8_t         rej_op_code;
    /// reject reason
    uint8_t         err_code;
};

/// LLCP_PING_REQ structure.
struct  llcp_ping_req
{
    /// op_code
    uint8_t         op_code;
};

/// LLCP_PING_RSP structure.
struct  llcp_ping_rsp
{
    /// op_code
    uint8_t         op_code;
};

/// LLCP_LENGTH_REQ structure.
struct  llcp_length_req
{
    /// op_code
    uint8_t     op_code;
    /// The max size in reception (unit of byte)
    uint16_t    max_rx_octets;
    /// The max time in reception (unit of microsecond)
    uint16_t    max_rx_time;
    /// The max size in transmission (unit of byte)
    uint16_t    max_tx_octets;
    /// The max time in transmission (unit of microsecond)
    uint16_t    max_tx_time;
};

/// LLCP_LENGTH_RSP structure.
struct  llcp_length_rsp
{
    /// op_code
    uint8_t     op_code;
    /// The max size in reception (unit of byte)
    uint16_t    max_rx_octets;
    /// The max time in reception (unit of microsecond)
    uint16_t    max_rx_time;
    /// The max size in transmission (unit of byte)
    uint16_t    max_tx_octets;
    /// The max time in transmission (unit of microsecond)
    uint16_t    max_tx_time;
};
/// LLCP_PHY_REQ structure.
struct  llcp_phy_req
{
    /// op_code
    uint8_t    op_code;
    /// Tx phy selection
    uint8_t    tx_phys;
    /// Rx phy selection
    uint8_t    rx_phys;
};

/// LLCP_PHY_RSP structure.
struct  llcp_phy_rsp
{
    /// op_code
    uint8_t    op_code;
    /// Tx phy selection
    uint8_t    tx_phys;
    /// Rx phy selection
    uint8_t    rx_phys;
};

/// LLCP_PHY_UPD_IND structure.
struct  llcp_phy_upd_ind
{
    /// op_code
    uint8_t    op_code;
    /// master to slave phy selected
    uint8_t    m_to_s_phy;
    /// slave to master phy selected
    uint8_t    s_to_m_phy;
    /// Instant
    uint16_t   instant;
};

/// LLCP_MIN_USED_CHANNELS_IND structure.
struct  llcp_min_used_ch_ind
{
    /// op_code
    uint8_t    op_code;
    /// PHY(s) for which the slave has a minimum number of used channels requirement
    uint8_t    phys;
    /// minimum number of channels to be used on the specified PHY
    uint8_t    min_used_ch;
};

/// LLCP pdu format
union llcp_pdu
{
    /// op_code
    uint8_t  op_code;

    struct llcp_con_update_ind    con_update_ind;
    struct llcp_channel_map_ind   channel_map_ind;
    struct llcp_terminate_ind     terminate_ind;
    struct llcp_enc_req           enc_req;
    struct llcp_enc_rsp           enc_rsp;
    struct llcp_start_enc_req     start_enc_req;
    struct llcp_start_enc_rsp     start_enc_rsp;
    struct llcp_unknown_rsp       unknown_rsp;
    struct llcp_feats_req         feats_req;
    struct llcp_feats_rsp         feats_rsp;
    struct llcp_pause_enc_req     pause_enc_req;
    struct llcp_pause_enc_rsp     pause_enc_rsp;
    struct llcp_vers_ind          vers_ind;
    struct llcp_reject_ind        reject_ind;
    struct llcp_slave_feature_req slave_feature_req;
    struct llcp_con_param_req     con_param_req;
    struct llcp_con_param_rsp     con_param_rsp;
    struct llcp_reject_ind_ext    reject_ind_ext;
    struct llcp_ping_req          ping_req;
    struct llcp_ping_rsp          ping_rsp;
    struct llcp_length_req        length_req;
    struct llcp_length_rsp        length_rsp;
    struct llcp_phy_req           phy_req;
    struct llcp_phy_rsp           phy_rsp;
    struct llcp_phy_upd_ind       phy_upd_ind;
};

/// @} CO_BT
#endif // CO_LLCP_H_
