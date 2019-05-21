/**
 ****************************************************************************************
 *
 * @file vif_mgmt.h
 *
 * @brief Virtual Interface Management definitions.
 *
 * Copyright (C) RivieraWaves 2011-2016
 *
 ****************************************************************************************
 */

#ifndef _VIF_MGMT_H_
#define _VIF_MGMT_H_

/**
 ****************************************************************************************
 * @defgroup VIF_MGMT VIF_MGMT
 * @ingroup LMAC
 * @brief Virtual interfaces Management.
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
// for MM timer structure
#include "mm_timer.h"
// for MM channel structures
#include "chan.h"
// for TX frame structures
#include "txl_frame.h"
#include "me.h"

#if (NX_REORD)
// for RX definitions
#include "rxl_cntrl.h"
#endif //(NX_REORD)

/**
 * DEFINES
 ****************************************************************************************
 */

/// Duration before a Timeout is raised when waiting for a beacon
#define VIF_MGMT_BCN_TO_DUR     (10000)

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */
/// Interface types
enum
{
    /// ESS STA interface
    VIF_STA,
    /// IBSS STA interface
    VIF_IBSS,
    /// AP interface
    VIF_AP,
    /// Mesh Point interface
    VIF_MESH_POINT,
    /// Unknown type
    VIF_UNKNOWN
};

/// Macro defining an invalid VIF index
#define INVALID_VIF_IDX 0xFF

/// Macro defining an unknown tx power
#define VIF_UNDEF_POWER 0x7F

#if (TDLS_ENABLE)
extern struct sta_tdls_tag sta_tdls;
#endif

/// VIF Info Table
struct vif_info_tag
{
    /// linked list header
    struct co_list_hdr list_hdr;
    /// Bitfield indicating if this VIF currently allows sleep or not
    uint32_t prevent_sleep;
    /// EDCA parameters of the different TX queues for this VIF
    uint32_t txq_params[AC_MAX];

    #if (NX_MULTI_ROLE || NX_CHNL_CTXT || (NX_P2P_GO && NX_POWERSAVE))
    /// TBTT timer structure
    struct mm_timer_tag tbtt_timer;
    #endif //(NX_MULTI_ROLE || NX_CHNL_CTXT || (NX_P2P_GO && NX_POWERSAVE))

    #if (NX_P2P || NX_CHNL_CTXT)
    /// Timer used for Beacon Reception Timeout
    struct mm_timer_tag tmr_bcn_to;
    #endif //(NX_P2P || NX_CHNL_CTXT)

    #if (NX_MULTI_ROLE || TDLS_ENABLE)
    /// BSSID this VIF belongs to
    struct mac_addr bssid;
    #endif //(NX_MULTI_ROLE)

    #if (NX_CHNL_CTXT)
    /// Channel context on which this VIF is attached
    struct chan_ctxt_tag *chan_ctxt;
    /// TBTT Switch Information
    struct chan_tbtt_tag tbtt_switch;
    #endif //(NX_CHNL_CTXT)

    /// MAC address of the VIF
    struct mac_addr mac_addr;

    /// Type of the interface (@ref VIF_STA, @ref VIF_IBSS, @ref VIF_MESH_POINT or @ref VIF_AP)
    uint8_t type;
    /// Index of the interface
    uint8_t index;
    /// Flag indicating if the VIF is active or not
    bool active;

    /// TX power configured for the interface (dBm)
    int8_t tx_power;

    /// TX power configured for the interface (dBm) by user space
    /// (Taken into account only if lower than regulatory one)
    int8_t user_tx_power;

    union
    {
        /// STA specific parameter structure
        struct
        {
            #if NX_POWERSAVE
            /// Listen interval
            uint16_t listen_interval;
            /// Flag indicating if we are expecting BC/MC traffic or not
            bool dont_wait_bcmc;
            /// Number of error seen during transmission of last NULL frame indicating PS change
            uint8_t ps_retry;
            #endif
            /// Index of the station being the peer AP
            uint8_t ap_id;
			
            #if NX_UAPSD
            /// Time of last UAPSD transmitted/received frame
            uint32_t uapsd_last_rxtx;
            /// Bitfield indicating which queues are U-APSD enabled
            uint8_t uapsd_queues;
            #endif
			
            #if NX_CONNECTION_MONITOR
            /// Time of last keep-alive frame sent to AP
            uint32_t mon_last_tx;
            /// CRC of last received beacon
            uint32_t mon_last_crc;
            /// Number of beacon losses since last beacon reception
            uint8_t beacon_loss_cnt;
            /// CRC of fixed beacon ie
            uint32_t mon_ie_crc;
            #endif

            #if (NX_P2P)
            /// Last computed TSF Offset
            int32_t last_tsf_offset;
            /// Addition duration to be added to the CTWindow, due to the TBTT_DELAY + drift value computed in mm_tbtt_compute
            uint32_t ctw_add_dur;
            /// Status indicated if Service Period has been paused due to GO absence
            bool sp_paused;
            #endif //(NX_P2P)

            #if (NX_P2P_GO)
            // Indicate if AP Beacon has been received at least one time
            bool bcn_rcved;
            #endif //(NX_P2P_GO)

            // Current RSSI
            int8_t rssi;
            // RSSI threshold (0=threshold not set)
            int8_t rssi_thold;
            // RSSI hysteresis
            uint8_t rssi_hyst;
            // Current status of RSSI (0=RSSI is high, 1=RSSI is low)
            bool rssi_status;
            
            /// Current CSA counter
            uint8_t csa_count;
            /// Indicate if channel switch (due to CSA) just happened
            bool csa_occured;
            
            uint8_t mm_retry;
            #if (TDLS_ENABLE)
            /// TDLS station
            struct sta_tdls_tag *sta_tdls;
            #endif
        } sta;

        /// AP specific parameter structure
        struct
        {
            uint32_t dummy;
			
            #if NX_BCN_AUTONOMOUS_TX
            /// Frame descriptor that will be used for the beacon transmission
            struct txl_frame_desc_tag bcn_desc;
            /// Current length of the beacon (except TIM element)
            uint16_t bcn_len;
            /// Current length of the TIM information element
            uint16_t tim_len;
            /// Number of bit currently set in the TIM virtual bitmap
            uint16_t tim_bitmap_set;
            /// Beacon Interval used by AP or MP VIF (in TUs)
            uint16_t bcn_int;
            /// Number of TBTT interrupts between each Beacon we have to send on this VIF
            uint8_t bcn_tbtt_ratio;
            /// Number of expected TBTT interrupts before our next Beacon transmission
            uint8_t bcn_tbtt_cnt;
            /// Flag indicating if the beacon has been configured (i.e downloaded from host)
            bool bcn_configured;
            /// Current value of the DTIM counter
            uint8_t dtim_count;
            /// Byte index of the LSB set in the TIM virtual bitmap
            uint8_t tim_n1;
            /// Byte index of the MSB set in the TIM virtual bitmap
            uint8_t tim_n2;
            /// Status bit of the BC/MC traffic bufferized (0: no traffic, 1: traffic)
            uint8_t bc_mc_status;
            /// Current CSA counter
            uint8_t csa_count;            
            uint8_t csa_action_count;
            /// Current CSA offset in beacon
            uint8_t csa_oft[BCN_MAX_CSA_CPT];
            #endif
			
            /// Flag indicating how many connected stations are currently in PS
            uint8_t ps_sta_cnt;
            /// Control port ethertype
            uint16_t ctrl_port_ethertype;
        } ap;
    } u;    ///< Union of AP/STA specific parameter structures

    /// List of stations linked to this VIF
    struct co_list sta_list;

    /// Information about the BSS linked to this VIF
    struct mac_bss_info bss_info;
    /// Default key information
    #if NX_MFP
    struct key_info_tag key_info[MAC_DEFAULT_MFP_KEY_COUNT];
    #else
    struct key_info_tag key_info[MAC_DEFAULT_KEY_COUNT];
    #endif
    /// Pointer to the current default key used
    struct key_info_tag *default_key;
    #if NX_MFP
    /// Pointer to the current default mgmt key used
    struct key_info_tag *default_mgmt_key;
    #endif
    /// Connection flags
    uint32_t flags;
    /// Destination channel of CSA
    struct mm_chan_ctxt_add_req csa_channel;

    #if (NX_P2P)
    /// Indicate if this interface is configured for P2P operations
    bool p2p;
    /// Index of the linked P2P Information structure
    uint8_t p2p_index;
    /// Contain current number of registered P2P links for the VIF
    uint8_t p2p_link_nb;
    #endif //(NX_P2P)

    #if (RW_UMESH_EN)
    /// Mesh ID - Index of the used mesh_vif_info_tag structure when type is VIF_MESH_POINT
    uint8_t mvif_idx;
    #endif //(RW_UMESH_EN)

    // for private structs, such as lwip- netif
    void *priv;
};

/// VIF management module environment
struct vif_mgmt_env_tag
{
    /// list of all the free VIF entries
    struct co_list free_list;
    /// list of all the used VIF entries
    struct co_list used_list;
    #if NX_MULTI_ROLE
    /// Number of STA VIFs registered
    uint8_t vif_sta_cnt;
    /// Number of AP VIFs registered
    uint8_t vif_ap_cnt;
    #if (RW_MESH_EN)
    /// Number of MP VIFs registered
    uint8_t vif_mp_cnt;
    #endif //(RW_MESH_EN)
    #endif //(NX_MULTI_ROLE)
    #if (NX_P2P && NX_CHNL_CTXT)
    /// Number of registered P2P VIFs
    uint8_t nb_p2p_vifs;
    #endif //(NX_P2P && NX_CHNL_CTXT)
    /// Index of VIF using the lowest Beacon Interval
    uint8_t low_bcn_int_idx;
};

/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */
/// VIF management environment variable
extern struct vif_mgmt_env_tag vif_mgmt_env;

/// VIF information table
extern struct vif_info_tag vif_info_tab[NX_VIRT_DEV_MAX];


/*
 * FUNCTION PROTOTYPES
 ****************************************************************************************
 */
#if NX_MULTI_ROLE
/**
 ****************************************************************************************
 * @brief Get the current number of used VIFs
 *
 * @return The number of used VIFs
 ****************************************************************************************
 */
__INLINE int vif_mgmt_used_cnt(void)
{
    return(vif_mgmt_env.vif_ap_cnt + vif_mgmt_env.vif_sta_cnt);
}
#endif


/**
 ****************************************************************************************
 * @brief Get the pointer to the first VIF in the used list
 *
 * @return A pointer to the VIF instance
 ****************************************************************************************
 */
__INLINE struct vif_info_tag *vif_mgmt_first_used(void)
{
    return((struct vif_info_tag *)co_list_pick(&vif_mgmt_env.used_list));
}


/**
 ****************************************************************************************
 * @brief Get the pointer to the next VIF following the one passed as parameter.
 *
 * @param[in] vif_entry  Pointer to the current VIF instance
 *
 * @return A pointer to the next VIF instance
 ****************************************************************************************
 */
__INLINE struct vif_info_tag *vif_mgmt_next(struct vif_info_tag *vif_entry)
{
    return((struct vif_info_tag *)co_list_next(&vif_entry->list_hdr));
}


/**
 ****************************************************************************************
 * @brief Initialize all the entries of the station table.
 ****************************************************************************************
 */
void vif_mgmt_init(void);

#if (NX_TX_FRAME)
/**
 ****************************************************************************************
 * @brief Tries to send all the internal frames postponed during a reset procedure once
 * the TX path has been fully reset.
 ****************************************************************************************
 */
void vif_mgmt_reset(void);
#endif //(NX_TX_FRAME)

/**
 ****************************************************************************************
 * @brief Allocates new VIF entry, initializes it and fills it with information passed
 * as parameters.
 *
 * @param[in]  mac_addr  MAC address of the VIF
 * @param[in]  vif_type  VIF type
 * @param[in]  p2p       P2P Interface
 * @param[out] vif_idx   Pointer to the allocated VIF index
 *
 * @return  CO_OK if allocation was successful, CO_FAIL otherwise.
 ****************************************************************************************
 */
uint8_t vif_mgmt_register(struct mac_addr const *mac_addr,
                          uint8_t  vif_type,
                          bool p2p,
                          uint8_t *vif_idx);

/**
 ****************************************************************************************
 * @brief Deletes the VIF entry
 *
 * @param[in]  vif_idx   Index of the VIF to be deleted.
 ****************************************************************************************
 */
void vif_mgmt_unregister(uint8_t vif_idx);

/**
 ****************************************************************************************
 * @brief Add a key for a VIF
 *
 * @param[in]  param      Pointer to the key parameters.
 * @param[in]  hw_key_idx Index of the key in the HW key RAM
 ****************************************************************************************
 */
void vif_mgmt_add_key(struct mm_key_add_req const *param, uint8_t hw_key_idx);

/**
 ****************************************************************************************
 * @brief delete a key for a VIF
 *
 * @param[in]  vif      Pointer to the VIF structure
 * @param[in]  keyid    Key index
 ****************************************************************************************
 */
void vif_mgmt_del_key(struct vif_info_tag *vif, uint8_t keyid);

__INLINE uint8_t vif_mgmt_get_type(uint8_t vif_idx)
{
    return (vif_info_tab[vif_idx].type);
}

#if (NX_P2P)
__INLINE uint8_t vif_mgmt_is_p2p(uint8_t vif_idx)
{
    return (vif_info_tab[vif_idx].p2p);
}
#endif //(NX_P2P)

__INLINE uint8_t vif_mgmt_is_ap_inface(uint8_t vif_idx)
{
    return (vif_info_tab[vif_idx].type == VIF_AP);
}

__INLINE uint8_t vif_mgmt_is_sta_inface(uint8_t vif_idx)
{
    return (vif_info_tab[vif_idx].type == VIF_STA);
}

/**
 ****************************************************************************************
 * @brief
 ****************************************************************************************
 */
__INLINE struct mac_addr *vif_mgmt_get_addr(uint8_t vif_idx)
{
    return (&vif_info_tab[vif_idx].mac_addr);
}

#if (NX_TX_FRAME)
/**
 ****************************************************************************************
 * @brief Try to send TX frame whose transmission had been postponed due to bad current
 * channel and peer absence.
 ****************************************************************************************
 */
void vif_mgmt_send_postponed_frame(struct vif_info_tag *p_vif_entry);
#endif //(NX_TX_FRAME)

#if (NX_CHNL_CTXT || NX_P2P)
/**
 ****************************************************************************************
 * @brief
 ****************************************************************************************
 */
void vif_mgmt_bcn_to_prog(struct vif_info_tag *p_vif_entry);

/**
 ****************************************************************************************
 * @brief
 ****************************************************************************************
 */
void vif_mgmt_bcn_recv(struct vif_info_tag *p_vif_entry);
#endif //(NX_CHNL_CTXT || NX_P2P)

/**
 ****************************************************************************************
 * @brief Set the Beacon Interval value for an AP or a Mesh Point VIF (should have been
 * verified before any call to this function).
 * It is also considered that the driver provides only beacon intervals whose value is a
 * multiple of a previously provided beacon interval.
 *
 * @param[in]   p_vif_entry     VIF Entry for which the Beacon Interval must be set
 * @param[in]   bcn_int         Beacon Interval
 ****************************************************************************************
 */
void vif_mgmt_set_ap_bcn_int(struct vif_info_tag *p_vif_entry, uint16_t bcn_int);

/**
 ****************************************************************************************
 * @brief Execute Channel switch at the end of a CSA
 *
 * This function will take care of changing the channel for a specific vif (that is
 * already active with a context channel).
 * It will first unlink the current channel ctx, create a new channel ctxt (channel
 * configuration is read from vif_info_tag.csa_channel) and link this new context.
 * Once done:
 * For STA: restart timer/counter for beacon
 * For AP: stop beacon transmission until driver send the updated beacon
 *
 * @param[in] p_vif_entry   Pointer to the VIF structure
 *
 ****************************************************************************************
 */
void vif_mgmt_switch_channel(struct vif_info_tag *p_vif_entry);
uint32_t vif_mgmt_first_used_staid(uint32_t *id);
uint8_t vif_mgmt_get_index_by_mac(const uint8_t *mac);
struct vif_info_tag *vif_mgmt_get_entry(uint8_t id);
/// @}

#endif // _VIF_MGMT_H_
