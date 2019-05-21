/**
 ****************************************************************************************
 *
 * @file sta_mgmt.h
 *
 * @brief UMAC Station Management definitions.
 *
 * Copyright (C) RivieraWaves 2011-2016
 *
 ****************************************************************************************
 */

#ifndef _STA_MGMT_H_
#define _STA_MGMT_H_

/**
 ****************************************************************************************
 * @defgroup STA_MGMT STA_MGMT
 * @ingroup LMAC
 * @brief Station Management.
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

// for linked list definitions
#include "co_list.h"
// for mac_addr and other structure definitions
#include "mac.h"
// for maximum station index
#include "mac_common.h"
// for struct mm_sta_add_req definition
#include "mm_task.h"

#if (TDLS_ENABLE)
// for struct tdls_chan_switch_req definition
#include "tdls_task.h"
// for mm_timer_tag
#include "mm_timer.h"
#endif

#include "rxu_cntrl.h"
#include "bam.h"

/**
 * DEFINES
 ****************************************************************************************
 */

/**
 ***********************************************************************
 * Protection Configuration Management
 ***********************************************************************
 */
/// Offset between prot_cfg value stored in sta_mgmt module and policy table
#define STA_MGMT_PROT_HW_OFT            (PROT_FRM_EX_RCX_OFT)
#define STA_MGMT_PROT_HW_MASK           (PROT_FRM_EX_RCX_MASK | MCS_INDEX_PROT_TX_RCX_MASK | BW_PROT_TX_RCX_MASK |\
                                         FORMAT_MOD_PROT_TX_RCX_MASK)
#define STA_MGMT_PROT_NAV_OFT           (PROT_FRM_EX_RCX_OFT - PROT_FRM_EX_RCX_OFT)
#define STA_MGMT_PROT_NAV_MASK          (PROT_FRM_EX_RCX_MASK >> PROT_FRM_EX_RCX_OFT)
#define STA_MGMT_PROT_MCS_OFT           (MCS_INDEX_PROT_TX_RCX_OFT - PROT_FRM_EX_RCX_OFT)
#define STA_MGMT_PROT_MCS_MASK          (MCS_INDEX_PROT_TX_RCX_MASK >> PROT_FRM_EX_RCX_OFT)
#define STA_MGMT_PROT_BW_OFT            (BW_PROT_TX_RCX_OFT - PROT_FRM_EX_RCX_OFT)
#define STA_MGMT_PROT_BW_MASK           (BW_PROT_TX_RCX_MASK >> PROT_FRM_EX_RCX_OFT)
#define STA_MGMT_PROT_FMT_MOD_OFT       (FORMAT_MOD_PROT_TX_RCX_OFT - PROT_FRM_EX_RCX_OFT)
#define STA_MGMT_PROT_FMT_MOD_MASK      (FORMAT_MOD_PROT_TX_RCX_MASK >> PROT_FRM_EX_RCX_OFT)

#define STA_MGMT_PROT_NAV_RTS_CTS_MASK  (PROT_RTS_CTS >> STA_MGMT_PROT_HW_OFT)
#define STA_MGMT_PROT_NAV_CTS_SELF_MASK (PROT_SELF_CTS >> STA_MGMT_PROT_HW_OFT)

/**
 ***********************************************************************
 * PPDU TX Configuration Management
 ***********************************************************************
 */
#define STA_MGMT_PPDU_TX_HW_MASK        (PRE_TYPE_TX_RCX_MASK)
#define STA_MGMT_PPDU_TX_PREAM_MASK     (PRE_TYPE_TX_RCX_MASK)

/**
 * ENUMERATIONS
 ****************************************************************************************
 */

enum sta_mgmt_key_type
{
    STA_MGMT_KEY_TYPE_PAIRWISE   = 0,
    STA_MGMT_KEY_TYPE_GROUP,

    STA_MGMT_KEY_TYPE_MAX
};

/// logical port state
enum sta_mgmt_port_state
{
    /// No data traffic could be exchanged with this station
    STA_MGMT_PORT_CLOSED         = 0,
    /// Encryption key is not yet available, only EAP frames could be sent
    STA_MGMT_PORT_CONTROLED,
    /// Any data types could be sent
    STA_MGMT_PORT_OPEN
};

/// Policy Table update bit field indexes
enum sta_mgmt_pol_upd
{
    /// Update Rate
    STA_MGMT_POL_UPD_RATE,
    /// Update Protection Configuration
    STA_MGMT_POL_UPD_PROT,
    /// Update PPDU TX Configuration
    STA_MGMT_POL_UPD_PPDU_TX,
    /// Update operational bandwidth
    STA_MGMT_POL_UPD_BW,
    /// Update tx power
    STA_MGMT_POL_UPD_TX_POWER,

    STA_MGMT_POL_UPD_MAX
};

/// logical port state
enum
{
    /// no data traffic could be exchanged with this station
    PORT_CLOSED = 0,
    /// encryption key is not yet available, only EAP frames could be sent
    PORT_CONTROLED,
    /// closing status for key is deleted but station is still active
    PORT_CLOSING,
    /// any data types could be sent
    PORT_OPEN
};

/**
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

// Forward declarations
struct mm_key_add_req;

/// Station Information related to Security
struct sta_mgmt_sec_info
{
    /// Information relative to the key
    struct key_info_tag key_info;
    #if (RW_MESH_EN)
    /// Mesh Group Management Keys used by peer Mesh STA
    struct key_info_tag key_mfp_mesh_info[MAC_DEFAULT_MFP_KEY_COUNT - MAC_DEFAULT_KEY_COUNT];
    #endif //(RW_MESH_EN)
    /// Pointer to the pairwise key
    struct key_info_tag *pairwise_key;
    /// Pointer to the currently used key pointer
    struct key_info_tag **cur_key;

};

/// Information related to the used policy table
struct sta_pol_tbl_cntl
{
    /// Pointer to the buffer control structures
    struct txl_buffer_control *buf_ctrl[2];
    #if RC_ENABLE
    /// Pointer to the rate control algorithm structure
    struct rc_sta_stats *sta_stats;
    #else
    /// Minimum rate index
    int r_idx_min;
    /// Maximum rate index
    int r_idx_max;
    /// Rate index currently used
    int r_idx;
    /// Number of failures at the current rate
    uint32_t fail_cnt;
    /// Number of attempts at the current rate
    uint32_t tx_cnt;
    #endif
    /**
     * Protection Configuration
     * Mapped on Rate Control entry in policy table provided to the HW (see MAC HW USer Manual)
     *      Bit 28:26 - Format and Modulation of Protection frame for Transmission -> Bit 12:14
     *      Bit 25:24 - Bandwidth of Protection Frame for Transmission             -> Bit 10:11
     *      Bit 23:17 - MCS Index of Protection Frame for Transmission             -> Bit 3:9
     *      Bit 16:14 - NAV Protection Frame Exchange                              -> Bit 0:2
     * Pushed inside Policy Table if Update Protection bit set to 1 in upd_field
     */
    uint16_t prot_cfg;
    /**
     * PPDU Transmission Configuration
     * Mapped on Rate Control entry in policy table provided to the HW (see MAC HW USer Manual)
     *      Bit 10 - Preamble Type of PPDU for Transmission -> Bit 10
     * Pushed inside Policy Table if Update PPDU TX bit set to 1 in upd_field
     */
    uint16_t ppdu_tx_cfg;
    /// Bit field indicating update to be done in the Policy Table
    uint8_t upd_field;
    /// Index of the buffer control currently used
    uint8_t buf_ctrl_idx;
};

struct sta_mgmt_ba_info
{
    /**
     * Array containing last time a packet has been transmitted
     *      - Used to detect that BA AGG is required on TX path
     */
    uint32_t last_tx_time;
    /// Array containing last time a ADD BA request has been attempted
    uint32_t last_ba_add_time;
    /// Array providing BAM task instance / TID mapping (RX Side)
    uint8_t bam_idx_rx;
    /// Array providing BAM task instance / TID mapping (TX Side)
    uint8_t bam_idx_tx;
    /// Offset to apply to the credit count of this STA/TID pair
    int8_t credit_oft;
};

#if (NX_AMPDU_TX)
///BA agreement structure - one for every possible TID, valid if params non 0
struct ba_agmt
{
    ///Starting sequence number - the one allowed as lowest in an A-MPDU - will change as MPDUs get Acked
    uint16_t ssn;
    ///Buffer size - max number of MPDUs that can be held in recipient's buffer
    uint8_t bufsz;
};
#endif //(NX_AMPDU_TX)

/// STA Info Table
struct sta_info_tag
{
    /// linked list header
    struct co_list_hdr list_hdr;
    /**
     * Beacon Interval used by peer AP or peer Mesh STA (in microseconds).
     * Used only for a VIF of type STA or MP
     */
    uint32_t bcn_int;
    /// maximum size of VHT A-MPDUs supported by the STA
    uint32_t ampdu_size_max_vht;
    /// maximum size of HT A-MPDUs supported by the STA
    uint16_t ampdu_size_max_ht;
    /// Minimal spacing to be inserted when transmitting to this STA
    uint8_t ampdu_spacing_min;
    #if (NX_POWERSAVE)
    /**
     * Maximum drift on one beacon period
     * Used only for a VIF of type STA or MP
     */
    uint16_t drift;
    #endif //(NX_POWERSAVE)
    /// AID of the station
    uint16_t aid;
    /// Interface the station belongs to
    uint8_t inst_nbr;
    /// Station index
    uint8_t staid;
    /// Power save state of the associated STA
    uint8_t ps_state;
    /// Flag indicating if the STA entry is valid or not
    bool valid;
    /// MAC address of the STA
    struct mac_addr mac_addr;

    /// Logical port state
    uint8_t ctrl_port_state;
    /// UAPSD traffic availability on host
    uint8_t uapsd_traffic_avail;
    /// UAPSD service period ongoing
    bool uapsd_sp_ongoing;
    /// control port protocol
    uint16_t ctrl_port_ethertype;
    /// STA Security Information
    struct sta_mgmt_sec_info sta_sec_info;
    /// Generic station information
    struct mac_sta_info info;
    /// Per-TID sequence numbers
    uint16_t seq_nbr[TID_MAX];
    /// TX policy table control
    struct sta_pol_tbl_cntl pol_tbl;

    #if (NX_AMPDU_TX)
    /// For TX BA agreements per TID
    struct ba_agmt ba_agmts_tx[TID_MAX];
    #endif //(NX_AMPDU_TX)

    #if (NX_REORD)
    /// For RX BA agreements per TID
    struct rxu_cntrl_reord *ba_agmts_rx[TID_MAX];
    #endif //(NX_REORD)

    struct sta_mgmt_ba_info ba_info[TID_MAX];
    /// Duplicate Detection Status (non-QoS data frames)
    uint16_t rx_nqos_last_seqcntl;
    /// Duplicate Detection Status (QoS data frames)
    uint16_t rx_qos_last_seqcntl[TID_MAX];

    #if (NX_TX_FRAME)
    // List of TX descriptor whose transmission has been postponed
    struct co_list tx_desc_post;
    #endif //(NX_TX_FRAME)

    #if (RW_MESH_EN)
    /// Mesh Link Index
    uint8_t mlink_idx;
    #endif //(RW_MESH_EN)

    #if (TDLS_ENABLE)
    /// Flag indicating if it is a TDLS station
    bool tdls_sta;
    #endif
};

/// STA management module environment
struct sta_info_env_tag
{
    /// list of all the free stations entries
    struct co_list free_sta_list;
};

#if (TDLS_ENABLE)
struct sta_tdls_tag
{
    // Flag indicating if TDLS station is active
    bool active;
    /// MAC address of the TDLS linked STA
    struct mac_addr peer_mac_addr;
    /// Station index of the TDLS linked STA
    uint8_t sta_idx;
    /// Flag indicating if the STA is the TDLS initiator
    bool initiator;
    /// Dialog token
    uint8_t dialog_token;
    /// Flag indicating if the STA has TDLS Channel Switch active
    bool chsw_active;
    /// TDLS Channel Switch band
    uint16_t chsw_band;
    /// TDLS Channel Switch type
    uint8_t chsw_type;
    /// TDLS Channel Switch frequency
    uint16_t chsw_prim20_freq;
    /// TDLS Channel Switch center frequency 1
    uint16_t chsw_center1_freq;
    /// TDLS Channel Switch center frequency 2
    uint16_t chsw_center2_freq;
    /// TDLS Channel Switch power
    int8_t chsw_tx_power;
    /// TDLS Channel Switch channel number
    uint8_t chsw_num;
    /// TDLS Channel Switch operating class
    uint8_t chsw_op_class;
    /// TDLS Channel Switch time
    uint16_t chsw_time;
    /// TDLS Channel Switch timeout
    uint16_t chsw_timeout;
    /// Flag indicating if Channel Switch Request is delayed
    bool chsw_delay;
    bool chsw_short;
    /// TDLS Channel Switch time timer
    struct mm_timer_tag chsw_time_timer;
    /// TDLS Channel Switch timeout timer
    struct mm_timer_tag chsw_timeout_timer;
    /// TDLS Channel Switch request timer
    struct mm_timer_tag chsw_req_timer;
};
#endif

/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */
/// STA management environment variable
extern struct sta_info_env_tag sta_info_env;

/// STA information table
extern struct sta_info_tag sta_info_tab[STA_MAX];


/*
 * FUNCTION PROTOTYPES
 ****************************************************************************************
 */
struct sta_info_tag *sta_mgmt_get_sta(const uint8_t *mac);

uint8_t sta_mgmt_get_staid(uint8_t vif_idx, const uint8_t *mac);

/**
 ****************************************************************************************
 * @brief Retrieve the STA index matching with the provided MAC Address
 *
 * @param[in]  sta_idx    STA Index
 ****************************************************************************************
 */
__INLINE uint8_t sta_mgmt_get_port_state(uint8_t sta_idx)
{
    return (sta_info_tab[sta_idx].ctrl_port_state);
}

/**
 ****************************************************************************************
 * @brief Retrieve the control portocol used by the STA
 *
 * @param[in]  sta_idx    STA Index
 *
 * @return The ethertype used on the control port.
 ****************************************************************************************
 */
__INLINE uint16_t sta_mgmt_get_port_ethertype(uint8_t sta_idx)
{
    return (sta_info_tab[sta_idx].ctrl_port_ethertype);
}


/**
 ****************************************************************************************
 * @brief Initialize all the entries of the station table.
 ****************************************************************************************
 */
void sta_mgmt_init(void);

/**
 ****************************************************************************************
 * @brief Allocates new station entry, initializes it and fills it with information passed
 * as parameters.
 *
 * @param[in] param      MM_STA_ADD_REQ message parameters
 * @param[in] sta_idx    Pointer to index of the STA to be registered, which will be filled
 *
 * @return  The allocated station table index.
 ****************************************************************************************
 */
uint8_t sta_mgmt_register(struct mm_sta_add_req const *param,
                          uint8_t *sta_idx);

/**
 ****************************************************************************************
 * @brief Deletes the station table and free all the resources allocated for this station.
 *
 * @param[in]  sta_idx           Index of the station to be deleted.
 ****************************************************************************************
 */
void sta_mgmt_unregister(uint8_t sta_idx);

/**
 ****************************************************************************************
 * @brief Add a key for a STA
 *
 * @param[in]  param      Pointer to the key parameters.
 * @param[in]  hw_key_idx Index of the key in the HW key RAM
 ****************************************************************************************
 */
void sta_mgmt_add_key(struct mm_key_add_req const *param, uint8_t hw_key_idx);

/**
 ****************************************************************************************
 * @brief Delete a key for a STA
 *
 * @param[in]  sta  Pointer to the STA for which the key is deleted
 ****************************************************************************************
 */
void sta_mgmt_del_key(struct sta_info_tag *sta);

/**
 ****************************************************************************************
 * @brief Return if a Block Ack agreement exists for the provided [STAID, TID] tuple
 * on RX or TX path
 *
 * @return BAM_INVALID_TASK_IDX if no agreement exists, else allocated BAM task instance
 ****************************************************************************************
 */
__INLINE int8_t sta_mgmt_get_and_clear_credit_oft(uint8_t sta_idx, uint8_t tid)
{
    int8_t credits = sta_info_tab[sta_idx].ba_info[tid].credit_oft;

    sta_info_tab[sta_idx].ba_info[tid].credit_oft = 0;
    return (credits);
}

/**
 ****************************************************************************************
 * @brief Return if a Block Ack agreement exists for the provided [STAID, TID] tuple
 * on RX or TX path
 ****************************************************************************************
 */
__INLINE void sta_mgmt_set_credit_oft(uint8_t sta_idx, uint8_t tid, int8_t credits)
{
    sta_info_tab[sta_idx].ba_info[tid].credit_oft += credits;
}

/**
 ****************************************************************************************
 * @brief Return if a Block Ack agreement exists for the provided [STAID, TID] tuple
 * on RX or TX path
 *
 * @return BAM_INVALID_TASK_IDX if no agreement exists, else allocated BAM task instance
 ****************************************************************************************
 */
__INLINE uint8_t sta_mgmt_get_tx_bam_idx(uint8_t sta_idx, uint8_t tid)
{
    return (sta_info_tab[sta_idx].ba_info[tid].bam_idx_tx);
}

__INLINE uint8_t sta_mgmt_get_rx_bam_idx(uint8_t sta_idx, uint8_t tid)
{
    return (sta_info_tab[sta_idx].ba_info[tid].bam_idx_rx);
}

/**
 ****************************************************************************************
 * @brief Return the last time at which a packet with a given TID has been transmitted on
 *        a link.
 *
 * @param[in] sta_idx   Station Index
 * @param[in] tid       TID
 ****************************************************************************************
 */
__INLINE uint32_t sta_mgmt_get_last_tx_time(uint8_t sta_idx, uint8_t tid)
{
    return (sta_info_tab[sta_idx].ba_info[tid].last_tx_time);
}

/**
 ****************************************************************************************
 * @brief Update the last time at which a packet with a given TID has been transmitted on
 *        a link.
 *
 * @param[in] sta_idx   Station Index
 * @param[in] tid       TID
 * @param[in] time      New TX Time
 ****************************************************************************************
 */
__INLINE void sta_mgmt_set_last_tx_time(uint8_t sta_idx, uint8_t tid, uint32_t time)
{
    sta_info_tab[sta_idx].ba_info[tid].last_tx_time = time;
}

/**
 ****************************************************************************************
 * @brief
 ****************************************************************************************
 */
__INLINE uint32_t sta_mgmt_get_add_ba_time(uint8_t sta_idx, uint8_t tid)
{
    return (sta_info_tab[sta_idx].ba_info[tid].last_ba_add_time);
}

/**
 ****************************************************************************************
 * @brief
 ****************************************************************************************
 */
__INLINE void sta_mgmt_set_add_ba_time(uint8_t sta_idx, uint8_t tid, uint32_t time)
{
    sta_info_tab[sta_idx].ba_info[tid].last_ba_add_time = time;
}

/**
 ****************************************************************************************
 * @brief
 *
 * @param[in]  sta_idx      STA Index
 * @param[in]  tid          TID
 ****************************************************************************************
 */
__INLINE uint16_t sta_mgmt_get_tx_ssn(uint8_t sta_idx, uint8_t tid)
{
    return sta_info_tab[sta_idx].seq_nbr[tid];
}

/**
 ****************************************************************************************
 * @brief Return the PS state of a given station
 *
 * @param[in]  sta_idx      STA Index
 *
 * @return true if the station is in PS, false otherwise
 ****************************************************************************************
 */
__INLINE bool sta_mgmt_is_in_ps(uint8_t sta_idx)
{
    return (sta_info_tab[sta_idx].ps_state != 0);
}

/**
 ****************************************************************************************
 * @brief Return the validity state of a specific station index, i.e it indicates if the
 * given station is currently registered or not
 *
 * @param[in]  sta_idx      STA Index
 *
 * @return true if the station is registered, false otherwise
 ****************************************************************************************
 */
__INLINE bool sta_mgmt_is_valid(uint8_t sta_idx)
{
	if (sta_idx >= STA_MAX)
		return false;
	
	return (sta_info_tab[sta_idx].valid);
}

/**
 ****************************************************************************************
 * @brief Retrieve the peer STA MAC Address matching with the provided STA Index
 *
 * @param[in]  sta_idx      STA Index
 *
 * @return NULL if no MAC Address not found, else the associated MAC Address
 ****************************************************************************************
 */
__INLINE struct mac_addr *sta_mgmt_get_peer_addr(uint8_t sta_idx)
{
    return (&sta_info_tab[sta_idx].mac_addr);
}

/**
 ****************************************************************************************
 * @brief Return the VIF index whose the STA depends on
 *
 * @param[in]  sta_idx      STA Index
 ****************************************************************************************
 */
__INLINE uint8_t sta_mgmt_get_vif_idx(uint8_t sta_idx)
{
    return (sta_info_tab[sta_idx].inst_nbr);
}

__INLINE uint8_t sta_mgmt_get_aid(uint8_t sta_idx)
{
    return (sta_info_tab[sta_idx].aid);
}

/**
 ****************************************************************************************
 * @brief Set the Start Sequence Number of the BA agreement for this STA/TID pair
 *
 * @param[in]  sta_idx      STA Index
 * @param[in]  tid          TID
 * @param[in]  ssn          Start sequence number of the BA agreement
 ****************************************************************************************
 */
__INLINE void sta_mgmt_set_tx_ssn(uint8_t sta_idx, uint8_t tid, uint8_t ssn)
{
    uint8_t bam_idx = sta_info_tab[sta_idx].ba_info[tid].bam_idx_tx;

    bam_env[bam_idx].ssn = ssn;
}

#if (NX_AMPDU_TX)
/**
 ****************************************************************************************
 * @brief Get the maximum number of MPDUs that can be put in an A-MPDU for this STA-TID pair
 *
 * @param[in]  sta_idx      STA Index
 * @param[in]  tid          TID
 *
 * @return The maximum number of MPDUs that can be put in an A-MPDU for this STA-TID pair
 ****************************************************************************************
 */
__INLINE uint8_t sta_mgmt_get_tx_buff_size(uint8_t sta_idx, uint8_t tid)
{
    return (sta_info_tab[sta_idx].ba_agmts_tx[tid].bufsz);
}

/**
 ****************************************************************************************
 * @brief Set the maximum number of MPDUs that can be put in an A-MPDU for this STA-TID pair
 *
 * @param[in]  sta_idx      STA Index
 * @param[in]  tid          TID
 * @param[in]  buff_size    Maximum number of MPDUs that can be put in an A-MPDU for this STA-TID pair
 ****************************************************************************************
 */
__INLINE void sta_mgmt_set_tx_buff_size(uint8_t sta_idx, uint8_t tid, uint8_t buff_size)
{
    sta_info_tab[sta_idx].ba_agmts_tx[tid].bufsz = buff_size;
}
#endif //(NX_AMPDU_TX)

/// @}

#endif // _STA_MGMT_H_
