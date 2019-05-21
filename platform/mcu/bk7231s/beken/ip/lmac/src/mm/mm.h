/**
 ****************************************************************************************
 * @file mm.h
 *
 * @brief MAC Management module definitions.
 *
 * Copyright (C) RivieraWaves 2011-2016
 *
 ****************************************************************************************
 */

#ifndef _MM_H_
#define _MM_H_

/**
 *****************************************************************************************
 * @defgroup LMAC LMAC
 * @ingroup MACSW
 * @brief Lower MAC SW module.
 *****************************************************************************************
 */

/**
 *****************************************************************************************
 * @defgroup MM MM
 * @ingroup LMAC
 * @brief LMAC MAC Management module.
 * @{
 *****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
// standard includes
#include "co_int.h"
#include "co_bool.h"

// for maximum station index
#include "mac_common.h"

#include "co_utils.h"
#include "ke_timer.h"
#include "mm_task.h"
#include "hal_desc.h"
#include "hal_machw.h"
#include "hal_dma.h"
#include "sta_mgmt.h"

#include "include.h"

/*
 * DEFINES
 ****************************************************************************************
 */
/// Event mask of the TBTT kernel events
#if NX_BEACONING
#define MM_TBTT_EVT_MASK (KE_EVT_PRIMARY_TBTT_BIT | KE_EVT_SECONDARY_TBTT_BIT)
#else
#define MM_TBTT_EVT_MASK KE_EVT_PRIMARY_TBTT_BIT
#endif

/// Beacon loss threshold above which we consider the connection as lost
#define MM_BEACON_LOSS_THD          (30) 

/// Periodicity of keep-alive NULL frame transmission
#define MM_KEEP_ALIVE_PERIOD        (1000000)   // 1s

/// Mask of the TBTT interrupts
#define MM_TBTT_IRQ_MASK            (NXMAC_IMP_PRI_DTIM_BIT | NXMAC_IMP_PRI_TBTT_BIT)

/// Mask used for the MAC address compatibility checking
#define MM_MAC_ADDR_MSK             ((NX_VIRT_DEV_MAX - 1) << 8)

/// Macro returning the maximum duration of A-MPDUs according to the TXOP limit
#define TXOP(limit)  (((limit)==0) || ((limit) > MM_DEFAULT_MAX_AMPDU_DURATION))?       \
                                    MM_DEFAULT_MAX_AMPDU_DURATION:(limit);

#if NX_MULTI_ROLE
/// Wake up delay before TBTT is occurring
#define TBTT_DELAY                  400  ///< 400us
#endif

/// Margin taken when checking if the computed TBTT is not in the past
#if NX_CHNL_CTXT
#define MM_TBTT_COMPUTE_MARGIN      (CHAN_SWITCH_DELAY + 300)
#else
#define MM_TBTT_COMPUTE_MARGIN      300
#endif

/// Task identifier used for transmission of indication from MM to UMAC
#define TASK_IND  TASK_SM
/// Maximum duration of an A-MPDU (in 32us units)
#define MM_DEFAULT_MAX_AMPDU_DURATION 100

/// Default peer device accuracy (in ppm)
#define MM_AP_CLK_ACCURACY            20

/// RX filter for monitoring mode (all packets allowed)
#define MM_RX_FILTER_MONITOR  (0xFFFFFFFF & ~(NXMAC_ACCEPT_ERROR_FRAMES_BIT \
                                            | NXMAC_EXC_UNENCRYPTED_BIT | \
                                              NXMAC_EN_DUPLICATE_DETECTION_BIT))

/// RX filter for active mode (i.e. not monitoring)
#ifdef CONFIG_AOS_MESH
#define MM_RX_FILTER_ACTIVE (NXMAC_ACCEPT_MULTICAST_BIT | NXMAC_ACCEPT_BROADCAST_BIT | \
                             NXMAC_ACCEPT_MY_UNICAST_BIT | \
                             NXMAC_ACCEPT_BEACON_BIT | NXMAC_ACCEPT_OTHER_MGMT_FRAMES_BIT | \
                             NXMAC_ACCEPT_BAR_BIT | NXMAC_ACCEPT_BA_BIT |                \
                             NXMAC_ACCEPT_DATA_BIT | NXMAC_ACCEPT_Q_DATA_BIT |           \
                             NXMAC_ACCEPT_QO_S_NULL_BIT | NXMAC_ACCEPT_OTHER_DATA_FRAMES_BIT| \
                             NXMAC_ACCEPT_OTHER_BSSID_BIT)
#else
#define MM_RX_FILTER_ACTIVE (NXMAC_ACCEPT_MULTICAST_BIT | NXMAC_ACCEPT_BROADCAST_BIT | \
                             NXMAC_ACCEPT_MY_UNICAST_BIT | \
                             NXMAC_ACCEPT_BEACON_BIT | NXMAC_ACCEPT_OTHER_MGMT_FRAMES_BIT | \
                             NXMAC_ACCEPT_BAR_BIT | NXMAC_ACCEPT_BA_BIT |                \
                             NXMAC_ACCEPT_DATA_BIT | NXMAC_ACCEPT_Q_DATA_BIT |           \
                             NXMAC_ACCEPT_QO_S_NULL_BIT | NXMAC_ACCEPT_OTHER_DATA_FRAMES_BIT)                             
#endif
/// Number of supported Default+Pairwise keys
#if NX_KEY_RAM_CONFIG
#define MM_SEC_MAX_KEY_NBR      (nxmac_sta_key_max_index_getf() + 1)
#else
#define MM_SEC_MAX_KEY_NBR      64
#endif

/// Number of VLAN Id
#if NX_KEY_RAM_CONFIG
#define MM_SEC_VLAN_COUNT (NX_VIRT_DEV_MAX + RW_MESH_LINK_NB)
#else
#define MM_SEC_VLAN_COUNT 6
#endif

/// Number of default keys
#define MM_SEC_DEFAULT_KEY_COUNT (MAC_DEFAULT_KEY_COUNT * MM_SEC_VLAN_COUNT)
#if (RW_MESH_EN)
/// Number of default keys for number of supported VIFs
#define MM_SEC_DEFAULT_VIF_KEY_COUNT    (MAC_DEFAULT_KEY_COUNT * NX_VIRT_DEV_MAX)
#endif //(RW_MESH_EN)

/// NULL cipher
#define MM_SEC_CTYPE_NULL        0
/// WEP (RC4) cipher
#define MM_SEC_CTYPE_WEP         1
/// TKIP cipher
#define MM_SEC_CTYPE_TKIP        2
/// CCMP cipher
#define MM_SEC_CTYPE_CCMP        3
/// WAPI (SMS4) cipher
#define MM_SEC_CTYPE_WPI_SMS4    4

/// Macro converting a default key index to its HW index in key memory
#define MM_VIF_TO_KEY(key_idx, vif_idx)     ((key_idx) + ((vif_idx) * MAC_DEFAULT_KEY_COUNT))
/// Macro converting a STA index to the its HW pairwise key index in key memory
#define MM_STA_TO_KEY(sta_idx)              ((sta_idx) + MM_SEC_DEFAULT_KEY_COUNT)
#if (RW_MESH_EN)
/// Macro converting a Mesh Link Index and a key index to its HW index in key memory
#define MM_MLINK_TO_KEY(key_idx, mlink_idx) (((NX_VIRT_DEV_MAX + mlink_idx) * MAC_DEFAULT_KEY_COUNT) + key_idx)
#endif //(RW_MESH_EN)

/// Macro converting HW pairwise key index (>= @ref MAC_DEFAULT_KEY_COUNT) to a STA index
#define MM_KEY_TO_STA(hw_key_idx)    ((hw_key_idx) - MM_SEC_DEFAULT_KEY_COUNT)
/// Macro converting a hw_key_idx (< @ref MAC_DEFAULT_KEY_COUNT) to the VIF index it corresponds
#define MM_KEY_TO_VIF(hw_key_idx)    (((hw_key_idx) & ~(MAC_DEFAULT_KEY_COUNT - 1)) / MAC_DEFAULT_KEY_COUNT)
/// Macro converting a hw_key_idx (< @ref MAC_DEFAULT_KEY_COUNT) to the default key id ir corresponds
#define MM_KEY_TO_KEYID(hw_key_idx)  ((hw_key_idx) & (MAC_DEFAULT_KEY_COUNT - 1))
#if (RW_MESH_EN)
/// Macro converting a hw_key_idx (< @ref MAC_DEFAULT_KEY_COUNT) to the Mesh Link index it corresponds
#define MM_KEY_TO_MLINK(hw_key_idx) (((hw_key_idx & ~(MAC_DEFAULT_KEY_COUNT - 1)) / MAC_DEFAULT_KEY_COUNT) - NX_VIRT_DEV_MAX)
#endif //(RW_MESH_EN)

#if NX_MFP
#define MM_SEC_MFP_KEY_COUNT (MAC_DEFAULT_MFP_KEY_COUNT - MAC_DEFAULT_KEY_COUNT)
#define MM_SEC_MAX_MFP_KEY_NBR  MM_SEC_MAX_KEY_NBR + (MM_SEC_MFP_KEY_COUNT * MM_SEC_VLAN_COUNT)

#define MM_VIF_TO_MFP_KEY(key_idx, vif_idx) ((((key_idx) - MAC_DEFAULT_KEY_COUNT) + ((vif_idx) * MM_SEC_MFP_KEY_COUNT)) + MM_SEC_MAX_KEY_NBR)
#define MM_MFP_KEY_TO_VIF(hw_key_idx) (((hw_key_idx) - MM_SEC_MAX_KEY_NBR) / MM_SEC_MFP_KEY_COUNT)
#define MM_MFP_KEY_TO_KEYID(hw_key_idx) ((((hw_key_idx) - MM_SEC_MAX_KEY_NBR) & (MM_SEC_MFP_KEY_COUNT - 1)) + MAC_DEFAULT_KEY_COUNT)

#if (RW_UMESH_EN)
#define MM_SEC_MAX_MESH_MFP_KEY_NBR  (MM_SEC_MAX_KEY_NBR + (MM_SEC_MFP_KEY_COUNT * NX_REMOTE_STA_MAX))

// Key Index must be 0 or 1
#define MM_STA_TO_MESH_MFP_KEY(key_idx, sta_idx) (MM_SEC_MAX_MFP_KEY_NBR + (MM_SEC_MFP_KEY_COUNT * sta_idx) + key_idx)
/// Macro converting a hw_key_idx (< @ref MAC_DEFAULT_KEY_COUNT) to the default key id ir corresponds
#define MM_MESH_MFP_KEY_TO_KEYID(hw_key_idx)  ((hw_key_idx - MM_SEC_MAX_MESH_MFP_KEY_NBR) & (MM_SEC_MFP_KEY_COUNT - 1))
#endif //(RW_UMESH_EN)
#endif //(NX_MFP)

#if (NX_P2P_GO || RW_UMESH_EN)
/// Delay between call to tbtt_timer callback for AP VIFs and AP_TBTT
#define MM_PRE_AP_TBTT_DELAY_US    (500)
#endif //(NX_P2P_GO || RW_UMESH_EN)

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

///BA agreement types
enum
{
    ///BlockAck agreement for TX
    BA_AGMT_TX,
    ///BlockAck agreement for RX
    BA_AGMT_RX,
};


///BA agreement related status
enum
{
    /// Correct BA agreement establishment
    BA_AGMT_ESTABLISHED,
    /// BA agreement already exists for STA+TID requested, cannot override it (should have been deleted first)
    BA_AGMT_ALREADY_EXISTS,
    /// Correct BA agreement deletion
    BA_AGMT_DELETED,
    /// BA agreement for the (STA, TID) doesn't exist so nothing to delete
    BA_AGMT_DOESNT_EXIST,
    /// No more BA agreement can be added for the specified type
    BA_AGMT_NO_MORE_BA_AGMT,
    /// BA agreement type not supported
    BA_AGMT_NOT_SUPPORTED
};

/// Features supported by LMAC - Positions
enum mm_features
{
    /// Beaconing
    MM_FEAT_BCN_BIT         = 0,
    /// Autonomous Beacon Transmission
    MM_FEAT_AUTOBCN_BIT,
    /// Scan in LMAC
    MM_FEAT_HWSCAN_BIT,
    /// Connection Monitoring
    MM_FEAT_CMON_BIT,
    /// Multi Role
    MM_FEAT_MROLE_BIT,
    /// Radar Detection
    MM_FEAT_RADAR_BIT,
    /// Power Save
    MM_FEAT_PS_BIT,
    /// UAPSD
    MM_FEAT_UAPSD_BIT,
    /// DPSM
    MM_FEAT_DPSM_BIT,
    /// A-MPDU
    MM_FEAT_AMPDU_BIT,
    /// A-MSDU
    MM_FEAT_AMSDU_BIT,
    /// Channel Context
    MM_FEAT_CHNL_CTXT_BIT,
    /// Packet reordering
    MM_FEAT_REORD_BIT,
    /// P2P
    MM_FEAT_P2P_BIT,
    /// P2P Go
    MM_FEAT_P2P_GO_BIT,
    /// UMAC Present
    MM_FEAT_UMAC_BIT,
    /// UMAC compiled with VHT support
    MM_FEAT_UMAC_VHT_BIT,
    /// Beamformee
    MM_FEAT_BFMEE_BIT,
    /// Beamformer
    MM_FEAT_BFMER_BIT,
    /// WAPI
    MM_FEAT_WAPI_BIT,
    /// MFP
    MM_FEAT_MFP_BIT,
    /// Mu-MIMO RX support
    MM_FEAT_MU_MIMO_RX_BIT,
    /// Mu-MIMO TX support
    MM_FEAT_MU_MIMO_TX_BIT,
    /// Wireless Mesh Networking
    MM_FEAT_MESH_BIT,
    /// TDLS support
    MM_FEAT_TDLS_BIT,
};


// Forward declaration
struct vif_info_tag;

/// LMAC MAC Management Context
struct mm_env_tag
{
    /// RX filtering requested by the Upper MAC
    uint32_t rx_filter_umac;
    /// RX filtering forced to be enabled by the Lower MAC
    uint32_t rx_filter_lmac_enable;
    /// Per-AC maximum A-MPDU duration value (in 32us unit)
    uint16_t ampdu_max_dur[NX_TXQ_CNT];
    #if NX_BEACONING && (!NX_MULTI_ROLE)
    /// Flag indicating if beaconing is enabled or not
    bool beaconing;
    #endif
    /// Previous state of the MM task, prior to go to IDLE
    uint8_t prev_mm_state;
    /// Previous state of the HW, prior to go to IDLE
    uint8_t prev_hw_state;
    #if NX_CHNL_CTXT
    /// Basic rates per band
    uint32_t basic_rates[PHY_BAND_MAX];
    #endif
    #if NX_UAPSD
    /// UAPSD Timer timeout value, in microseconds
    uint32_t uapsd_timeout;
    #endif
    #if NX_POWERSAVE
    /// Local LP clock accuracy (in ppm)
    uint16_t lp_clk_accuracy;
    #endif
    /// IDLE state requested by the host
    uint8_t host_idle;

    struct mm_timer_tag machw_timer;
};

/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */
extern struct mm_env_tag mm_env;

/*
 * FUNCTION PROTOTYPES
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief This function applies the RX filter requested by the UMAC and LMAC.
 *
 ****************************************************************************************
 */
__INLINE void mm_rx_filter_set(void)
{
    // Configure the filtering of the HW
    nxmac_rx_cntrl_set(mm_env.rx_filter_umac | mm_env.rx_filter_lmac_enable);
}
/**
 ****************************************************************************************
 * @brief This function sets the RX filter for the UMAC.
 *
 * @param[in] filter  Filter bitfield requested by the UMAC
 ****************************************************************************************
 */
__INLINE void mm_rx_filter_umac_set(uint32_t filter)
{
    mm_env.rx_filter_umac = filter;

    // Configure the filtering of the HW
    mm_rx_filter_set();
}

__INLINE uint32_t mm_rx_filter_umac_get(void)
{
    return mm_env.rx_filter_umac;
}

/**
 ****************************************************************************************
 * @brief This function enables some RX filter bits on the LMAC filter
 *
 * @param[in] filter  Bits to be set
 ****************************************************************************************
 */
__INLINE void mm_rx_filter_lmac_enable_set(uint32_t filter)
{
    mm_env.rx_filter_lmac_enable |= filter;

    // Configure the filtering of the HW
    mm_rx_filter_set();
}

/**
 ****************************************************************************************
 * @brief This function disables some RX filter bits on the LMAC filter
 *
 * @param[in] filter  Bits to be cleared
 ****************************************************************************************
 */
__INLINE void mm_rx_filter_lmac_enable_clear(uint32_t filter)
{
    mm_env.rx_filter_lmac_enable &= ~filter;

    // Configure the filtering of the HW
    mm_rx_filter_set();
}

/**
 ****************************************************************************************
 * @brief This function indicates a PS state change of a peer device to the upper layers.
 *
 * @param[in] sta_idx  Index of the peer device in the station table
 * @param[in] ps_state New PS state of the peer device
 ****************************************************************************************
 */
__INLINE void mm_ps_change_ind(uint8_t sta_idx, uint8_t ps_state)
{
    struct sta_info_tag *sta = &sta_info_tab[sta_idx];

    #if (NX_P2P)
    if (sta->ps_state != ps_state)
    #endif //(NX_P2P)
    {
        struct mm_ps_change_ind *ind = KE_MSG_ALLOC(MM_PS_CHANGE_IND, TASK_API,
                                                        TASK_MM, mm_ps_change_ind);

        // Change the local value of the PS state
        sta->ps_state = ps_state;

        // Fill-in the parameters
        ind->sta_idx = sta_idx;
        ind->ps_state = ps_state;

        // Send the message
        ke_msg_send(ind);
    }
}

/**
 ****************************************************************************************
 * @brief This function requests some buffered packets to be transmitted
 *
 * @param[in] sta_idx  Index of the peer device we have to transmit to
 * @param[in] pkt_cnt  Number of packets that need to be transmitted
 * @param[in] uapsd    Flag indicating if the request is for U-APSD queues or not
 ****************************************************************************************
 */
__INLINE void mm_traffic_req_ind(uint8_t sta_idx, uint8_t pkt_cnt, bool uapsd)
{
    struct mm_traffic_req_ind *ind = KE_MSG_ALLOC(MM_TRAFFIC_REQ_IND, TASK_API,
                                                    TASK_MM, mm_traffic_req_ind);

    // Fill-in the parameters
    ind->sta_idx = sta_idx;
    ind->pkt_cnt = pkt_cnt;
    ind->uapsd = uapsd;

    // Send the message
    ke_msg_send(ind);
}


/**
 ****************************************************************************************
 * @brief MM Module main initialization function.
 * This function is called after reset and initializes all MM related env and data.
 ****************************************************************************************
 */
void mm_init(void);


/**
 ****************************************************************************************
 * @brief MM Module reset function.
 * This function is part of the recovery mechanism invoked upon an error detection in the
 * LMAC. It resets the MM state machine.
 ****************************************************************************************
 */
void mm_reset(void);

/**
 ****************************************************************************************
 * @brief Put the HW state to active.
 ****************************************************************************************
 */
void mm_active(void);

/**
 ****************************************************************************************
 * @brief MM Module mm_env_init routine.
 ****************************************************************************************
 */
void mm_env_init(void);


/**
 ****************************************************************************************
 * @brief Set the maximum A-MPDU duration based on TXOP registers.
 ****************************************************************************************
 */
void mm_env_max_ampdu_duration_set(void);


/**
 ****************************************************************************************
 * @brief Function to add a MAC address in the MAC HW key storage.
 * This function is called from MM_STA_ADD_REQ handler.
 *
 * @param[in] sta_idx  Index of the station
 * @param[in] inst_nbr Index of the VIF
 *
 * @return The HW STA index for the new STA
 *
 ****************************************************************************************
 */
uint8_t mm_sec_machwaddr_wr(uint8_t sta_idx, uint8_t inst_nbr);

/**
 ****************************************************************************************
 * @brief Function to delete a MAC address from the MAC HW key storage.
 * This function is called from MM_STA_DEL_REQ handler.
 *
 * @param[in] sta_idx Index of the station
 *
 ****************************************************************************************
 */
void mm_sec_machwaddr_del(uint8_t sta_idx);

/**
 ****************************************************************************************
 * @brief Function to write key in MAC HW.
 * This function is called from MM_KEY_ADD_REQ handler to store a key into the
 * KeyStoragRAM of the MAC HW.
 *
 * @param[in] param     Pointer to the parameters of the message.
 *
 * @return The HW key index for the new key
 *
 ****************************************************************************************
 */
uint8_t mm_sec_machwkey_wr(struct mm_key_add_req const *param);

/**
 ****************************************************************************************
 * @brief Function to delete a key from the MAC HW.
 * This function is called from MM_KEY_DEL_REQ handler to remove a key from the
 * KeyStoragRAM of the MAC HW.
 *
 * @param[in] hw_key_idx  Index of the key to be removed
 *
 ****************************************************************************************
 */
void mm_sec_machwkey_del(uint8_t hw_key_idx);

/**
 ****************************************************************************************
 * @brief Kernel event handler for the TBTT event.
 *
 * This function flushes the beacon queue and signals the event to the upper
 * MAC.
 *
 * @param[in] dummy Parameter not used but required to follow the kernel event callback
 * format
 ****************************************************************************************
 */
void mm_tbtt_evt(int dummy);

#if NX_CONNECTION_MONITOR || NX_MULTI_ROLE
/**
 ****************************************************************************************
 * @brief Function responsible for all the connection monitoring stuff. It resets the
 * beacon loss counter, checks if the beacon has changed compared to the previous one
 * (in order to know if it has to be forwarded to the upper layers or not), and sends the
 * keep-alive NULL frame if it is time to do so.
 *
 * @param[in] rhd        Pointer to the beacon RX descriptor
 * @param[in] vif_entry  Pointer to the VIF entry
 * @param[out] tim       Pointer to the TIM IE found in the beacon
 *
 * @return true if the beacon has to be uploaded (i.e because it changed since the last
 * one), false otherwise.
 *
 ****************************************************************************************
 */
bool mm_check_beacon(struct rx_hd *rhd, struct vif_info_tag *vif_entry,
                     struct sta_info_tag *p_sta_entry, uint32_t *tim);
#endif

/**
 ****************************************************************************************
 * @brief Kernel event handler for the HW IDLE event.
 *
 * This function puts the MM task in IDLE state.
 *
 * @param[in] dummy Parameter not used but required to follow the kernel event callback
 * format
 ****************************************************************************************
 */
void mm_hw_idle_evt(int dummy);

#if NX_MULTI_ROLE
/**
 ****************************************************************************************
 * @brief Setting of general HW info upon the first interface creation
 *
 * @param[in] mac_addr Primary MAC address of the interface
 ****************************************************************************************
 */
void mm_hw_info_set(struct mac_addr const *mac_addr);

/**
 ****************************************************************************************
 * @brief Set the correct HW information for an AP
 ****************************************************************************************
 */
void mm_hw_ap_info_set(void);

/**
 ****************************************************************************************
 * @brief Reset the HW information for an AP
 ****************************************************************************************
 */
void mm_hw_ap_info_reset(void);

#else
/**
 ****************************************************************************************
 * @brief Setting of interface info in the HW prior to join or create a BSS
 *
 * @param[in] type     Type of the interface that is added (@ref VIF_STA, @ref VIF_AP or @ref VIF_IBSS)
 * @param[in] mac_addr MAC address of the interface
 ****************************************************************************************
 */
void mm_hw_interface_info_set(uint8_t type, struct mac_addr const *mac_addr);
#endif

/**
 ****************************************************************************************
 * @brief MM timer callback for the station TBTT handling
 *
 * @param[in] env  Pointer to the environment to be passed to the callback upon timer
 *                 expiry
 ****************************************************************************************
 */
void mm_sta_tbtt(void *env);

/**
 ****************************************************************************************
 * @brief MM timer callback for the AP pre-TBTT handling
 *
 * @param[in] env  Pointer to the environment to be passed to the callback upon timer
 *                 expiry
 ****************************************************************************************
 */
void mm_ap_pre_tbtt(void *env);

extern struct mm_env_tag mm_env;

#if (NX_AMPDU_TX)
/**
 ****************************************************************************************
 * @brief Simple check for existence of a BA agreement for a certain (STA, TID) pair (TX)
 * @param[in] sta_idx     STA index in STA info table for which BA agreement existence must be checked
 * @param[in] tid         TID corresponding to STA index in STA info table for which BA agreement existence must be checked
 * @return 1 if existent, 0 if not.
 ****************************************************************************************
 */
bool mm_ba_agmt_tx_exists(uint8_t sta_idx, uint8_t tid);
#endif //(NX_AMPDU_TX)

#if (NX_REORD)
/**
 ****************************************************************************************
 * @brief Simple check for existence of a BA agreement for a certain (STA, TID) pair (RX)
 * @param[in] sta_idx     STA index in STA info table for which BA agreement existence must be checked
 * @param[in] tid         TID corresponding to STA index in STA info table for which BA agreement existence must be checked
 * @return 1 if existent, 0 if not.
 ****************************************************************************************
 */
bool mm_ba_agmt_rx_exists(uint8_t sta_idx, uint8_t tid);
#endif //(NX_REORD)

/**
 ****************************************************************************************
 * @brief Force the HW to go to IDLE immediately
 ****************************************************************************************
 */
void mm_force_idle_req(void);


/**
 ****************************************************************************************
 * @brief Add a station to the list of known stations
 *
 * @param[in] param         MM_STA_ADD_REQ message parameters
 * @param[out] sta_idx      Pointer to which the allocated STA index should be written
 * @param[out] hw_sta_idx   Pointer to which the allocated STA HW index should be written
 *
 * @return The status of the allocation (@ref CO_OK if successful)
 ****************************************************************************************
 */
uint8_t mm_sta_add(struct mm_sta_add_req const *param, uint8_t  *sta_idx,
                   uint8_t *hw_sta_idx);

/**
 ****************************************************************************************
 * @brief Delete a station from the list of known stations
 *
 * @param[in] sta_idx Index of the station to delete
 ****************************************************************************************
 */
void mm_sta_del(uint8_t sta_idx);

/**
 ****************************************************************************************
 * @brief Ask MM to switch back to IDLE state requested by the host (quit BYPASSED mode)
 ****************************************************************************************
 */
void mm_back_to_host_idle(void);

#if (NX_CONNECTION_MONITOR)
/**
 ****************************************************************************************
 * @brief Inform the host that connection has been lost with an AP.
 *
 * @param[in] VIF Entry used for the connection.
 ****************************************************************************************
 */
void mm_send_connection_loss_ind(struct vif_info_tag *p_vif_entry);
#endif //(NX_CONNECTION_MONITOR)

/**
 ****************************************************************************************
 * @brief Check RSSI level and inform the host if RSSI is decreased below the threshold
 * or if it is increased above the threshold.
 *
 * @param[in] vif_entry  Pointer to the VIF entry
 * @param[in] rssi       RSSI received
 ****************************************************************************************
 */
void mm_check_rssi(struct vif_info_tag *vif_entry, int8_t rssi);

/**
 ****************************************************************************************
 * @brief Send host message to stop (resp restarted) tx traffic because CSA is in
 * progress (resp. finished)
 *
 * @param[in] vif_index  Vif index
 * @param[in] enable     Traffic status
 ****************************************************************************************
 */
void mm_send_csa_traffic_ind(uint8_t vif_index, bool enable);
/// @} end of group

void mm_hw_ap_disable(void);

#endif // _MM_H_
