/**
 ****************************************************************************************
 * @file p2p.h
 *
 * @brief Wi-Fi Peer-to-Peer (P2P) Management
 *
 * Copyright (C) RivieraWaves 2011-2016
 *
 ****************************************************************************************
 */

#ifndef _P2P_H_
#define _P2P_H_

/**
 *****************************************************************************************
 * @defgroup P2P P2P
 * @ingroup LMAC
 * @brief P2P module.
 * @{
 *****************************************************************************************
 */

/**
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwnx_config.h"

#if (NX_P2P)

#include "mm_timer.h"
#include "hal_desc.h"
#include "compiler.h"

/**
 * DEFINES
 ****************************************************************************************
 */


/**
 * P2P IE format
 * See wifi_p2p_technical_specification_v1.1.pdf
 *     4.1.1 - P2P IE Format Table 4
 *     4.1.14 - Notice of Absence attribute Tables 26-27
 */
#define P2P_NOA_ATT_ATT_ID_OFFSET       (0)
#define P2P_NOA_ATT_LENGTH_OFFSET       (1)
#define P2P_NOA_ATT_INDEX_OFFSET        (3)
#define P2P_NOA_ATT_CTW_OPPPS_OFFSET    (4)
#define P2P_NOA_ATT_NOA_DESC_OFFSET     (5)

#define P2P_IE_NOA_ELMT_ID_OFFSET       (0)
#define P2P_IE_NOA_LENGTH_OFFSET        (1)
#define P2P_IE_NOA_OUI_OFFSET           (2)
#define P2P_IE_NOA_OUI_TYPE_OFFSET      (5)
#define P2P_IE_NOA_ATT_ID_OFFSET        (6)
#define P2P_IE_NOA_ATT_LENGTH_OFFSET    (7)
#define P2P_IE_NOA_INDEX_OFFSET         (9)
#define P2P_IE_NOA_CTW_OPPPS_OFFSET     (10)
#define P2P_IE_NOA_NOA_DESC_OFFSET      (11)

#define P2P_IE_NOA_NO_NOA_DESC_LENGTH   (11)

/**
 * Notice of Absence descriptor format
 * See wifi_p2p_technical_specification_v1.1.pdf
 *     4.1.14 - Notice of Absence attribute Tables 28
 */
#define P2P_NOA_DESC_COUNT_OFFSET    (0)
#define P2P_NOA_DESC_DUR_OFFSET      (1)
#define P2P_NOA_DESC_INTV_OFFSET     (5)
#define P2P_NOA_DESC_START_OFFSET    (9)

#define P2P_NOA_DESC_LENGTH          (13)

#define P2P_NOA_IE_BUFFER_LEN  ((P2P_IE_NOA_NO_NOA_DESC_LENGTH + (P2P_NOA_NB_MAX * P2P_NOA_DESC_LENGTH) + 1) / 2)

/// Information allowing to find P2P IE
#define P2P_OUI_LENGTH              (3)
/// OUI Value - Wi-FiAll
#define P2P_OUI_WIFI_ALL_BYTE0      (0x50)
#define P2P_OUI_WIFI_ALL_BYTE1      (0x6F)
#define P2P_OUI_WIFI_ALL_BYTE2      (0x9A)
/// Vendor Specific OUI Type for P2P
#define P2P_OUI_TYPE_P2P            (9)
/// P2P Element ID (Vendor Specific)
#define P2P_ELMT_ID                 (MAC_ELTID_OUI)
#define P2P_ATT_LEN_OFT             (1)
#define P2P_ATT_BODY_OFT            (3)

/**
 * If the counter field of NOA attribute is equal to 255, the absence cycles shall repeat
 * until cancel.
 */
#define P2P_NOA_CONTINUOUS_COUNTER  (255)
/// Invalid P2P Information Structure Index
#define P2P_INVALID_IDX             (0xFF)
/// Maximal number of concurrent NoA
#define P2P_NOA_NB_MAX              (2)

/// Margin used to avoid to program NOA timer in the past (in us)
#define P2P_NOA_TIMER_MARGIN        (5000)
/// Minimal absence duration we can consider (in us)
#define P2P_ABSENCE_DUR_MIN         (5000)
/// Beacon RX Timeout Duration (in us)
#define P2P_BCN_RX_TO_DUR           (5000)

/// Flag allowing to read OppPS subfield of CTWindow + OppPS field
#define P2P_OPPPS_MASK              (0x80)
/// Flag allowing to read CTWindow subfield of CTWindow + OppPS field
#define P2P_CTWINDOW_MASK           (0x7F)

/// SSID Wildcard for a P2P Device
#define P2P_SSID_WILDCARD           ("DIRECT-")
/// Length of P2P SSID Wildcard
#define P2P_SSID_WILDCARD_LEN       (7)

/**
 * ENUMERATIONS
 ****************************************************************************************
 */

/// NOA Timer (see struct p2p_info_tag) status
enum p2p_noa_timer_status
{
    /// Timer not started
    P2P_NOA_TIMER_NOT_STARTED = 0,
    /// Wait for next absence
    P2P_NOA_TIMER_WAIT_NEXT_ABS,
    /// Wait for end of absence
    P2P_NOA_TIMER_WAIT_END_ABS,

    P2P_NOA_TIMER_STATUS_MAX
};

/// OppPS Timer (see struct p2p_info_tag) status
enum p2p_oppps_timer_status
{
    /// Timer not started
    P2P_OPPPS_TIMER_NOT_STARTED = 0,
    /// Wait for end of CTWindow
    P2P_OPPPS_TIMER_WAIT_END_CTW,

    P2P_OPPPS_TIMER_STATUS_MAX
};

/**
 * P2P Attribute ID definitions
 * See wifi_p2p_technical_specification_v1.1.pdf
 *     4.1.1 - P2P IE Format Table 6
 */
enum p2p_attribute_id
{
    P2P_ATT_ID_STATUS         = 0,
    P2P_ATT_ID_MINOR_REASON_CODE,
    P2P_ATT_ID_P2P_CAPABILITY,
    P2P_ATT_ID_P2P_DEVICE_ID,
    P2P_ATT_ID_GROUP_OWNER_INTENT,
    P2P_ATT_ID_CONFIG_TIMEOUT,
    P2P_ATT_ID_LISTEN_CHANNEL,
    P2P_ATT_ID_P2P_GROUP_BSSID,
    P2P_ATT_ID_EXT_LISTEN_TIMING,
    P2P_ATT_ID_INTENDED_P2P_IF_ADDR,
    P2P_ATT_ID_P2P_MANAGEABILITY,
    P2P_ATT_ID_CHANNEL_LIST,
    P2P_ATT_ID_NOTICE_OF_ABSENCE,
    P2P_ATT_ID_P2P_DEVICE_INFO,
    P2P_ATT_ID_P2P_GROUP_INFO,
    P2P_ATT_ID_P2P_GROUP_ID,
    P2P_ATT_ID_P2P_INTERFACE,
    P2P_ATT_ID_OPERATING_CHANNEL,
    P2P_ATT_ID_INVITATION_FLAGS,
    /// 19 - 220 -> Reserved
    P2P_ATT_ID_VENDOR_SPEC   = 221,
    /// 222 - 255 -> Reserved
    P2P_ATT_ID_MAX           = 255
};

/// P2P Role
enum p2p_role
{
    /// Client
    P2P_ROLE_CLIENT = 0,
    /// GO
    P2P_ROLE_GO,

    P2P_ROLE_MAX,
};

/// Operation codes to be used in p2p_bcn_update_req structure
enum p2p_bcn_upd_op
{
    /// No Operation
    P2P_BCN_UPD_OP_NONE     = 0,

    /// Add P2P NOA IE
    P2P_BCN_UPD_OP_NOA_ADD,
    /// Remove P2P NOA IE
    P2P_BCN_UPD_OP_NOA_RMV,
    /// Update P2P NOA IE
    P2P_BCN_UPD_OP_NOA_UPD,

    P2P_BCN_UPD_OP_MAX
};

#if (NX_P2P_GO)
/// Type of NoA
enum p2p_noa_type
{
    /// Concurrent NoA - Triggered by connection as STA on a different channel
    P2P_NOA_TYPE_CONCURRENT,
    /// Normal NoA
    P2P_NOA_TYPE_NORMAL,
};
#endif //(NX_P2P_GO)

/**
 * TYPES DEFINITION
 ****************************************************************************************
 */

// Forward declaration
struct vif_info_tag;

/**
 * Structure containing all information linked with NoA
 */
struct p2p_noa_info_tag
{
    /// Timer used for P2P Notice of Absence management
    struct mm_timer_tag noa_timer;
    /// Status of the NOA timer
    uint8_t noa_status;

    /// Index of the linked P2P information structure
    uint8_t p2p_index;
    /// NOA Instance
    uint8_t noa_inst;
    /**
     * NOA Counter
     * Indicate the remaining number of absence (current one included) - 255 for continuous
     */
    uint8_t noa_counter;
    /// Initial NOA Counter value
    uint8_t noa_init_counter;
    /// NOA - Absence Duration (in us)
    uint32_t noa_dur_us;
    /// NOA - Absence Interval (in us)
    uint32_t noa_intv_us;

    #if (NX_P2P_GO)
    /// Type of NoA (due to Concurrent Mode or required by Host for GO) - relevant for GO only
    uint8_t noa_type;
    /// Indicate if NoA can be paused for traffic reasons
    bool dyn_noa;
    #if (NX_CHNL_CTXT)
    /// NOA - Next Absence Duration (in us)
    uint32_t noa_next_dur_us;
    #endif //(NX_CHNL_CTXT)
    #endif //(NX_P2P_GO)

    /// NOA - Start Time - GO only
    uint32_t noa_start_time;
    /// Counter used to know when we can update the start time inside the beacon - GO only
    uint32_t noa_time_upd_cnt;

    /// Next NOA time (absence or end of absence) in peer TSF (valid only for CLI connection)
    uint32_t peer_next_noa_time;
};

/**
 * Information structure containing all information about a P2P link
 */
struct p2p_info_tag
{
    /// NOA - Up to 2 concurrent NoA
    struct p2p_noa_info_tag noa[P2P_NOA_NB_MAX];
    /// Timer used for P2P Opportunistic PS management
    struct mm_timer_tag oppps_timer;

    /// Role
    uint8_t role;
    /// Associated VIF index
    uint8_t vif_index;
    /// Status of OppPS timer
    uint8_t oppps_status;
    /// CTWindow, Opportunistic Power Save is enabled if CTW is different than 0
    uint8_t oppps_ctw;
    /// Last received/sent NOA Index
    uint8_t index;
    /// Number of NOAs currently used
    uint8_t noa_nb;
    /// Number of NOA triggered by Concurrent Mode
    uint8_t noa_cm_nb;

    /// Indicate if the NOA attribute is part of the beacon
    bool is_noa_bcn;
    /// Indicate if P2P Go is present
    bool is_go_present;
    /// Indicate if we are waiting for Beacon reception
    bool is_waiting_bcn;

    #if (NX_P2P_GO)
    /*
     * Indicate if P2P PS NoA is paused due to traffic
     *  - OppPS does not need to be paused: if we have traffic on a link, peer device won't enter in doze mode
     *  - NoA due to Concurrent mode cannot be paused
     */
    bool noa_paused;
    /*
     * Indicate the number of NoA indicated in the beacon.
     * This value can be different than noa_nb if noa_paused is true
     */
    uint8_t noa_in_bcn_nb;
    #endif //(NX_P2P_GO)
};

#if (NX_P2P_GO)
/// P2P Global Environment
struct p2p_env_tag
{
    /// Number of P2P GO VIF that have been created (up to 1)
    uint8_t nb_p2p_go;
};
#endif //(NX_P2P_GO)

/**
 * GLOBAL VARIABLES
 ****************************************************************************************
 */

#if (NX_P2P_GO)
extern struct p2p_env_tag p2p_env;
#endif //(NX_P2P_GO)

extern struct p2p_info_tag p2p_info_tab[NX_P2P_VIF_MAX];

/**
 * PUBLIC FUNCTIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize the p2p module. Set the vif_index value of each p2p structures to
 * INVALID_VIF_IDX in order to inform that the structure is available.
 ****************************************************************************************
 */
void p2p_init(void);


/**
 ****************************************************************************************
 * @brief Link a VIF entry with a P2P entry.
 *
 * @param[in] vif_index     Index of the P2P VIF
 * @param[in] role          P2P Interface Role
 *
 * @return P2P entry index if a P2P structure was available, else P2P_INVALID_IDX.
 ****************************************************************************************
 */
uint8_t p2p_create(uint8_t vif_index, uint8_t role);

/**
 ****************************************************************************************
 * @brief Release a P2P structure.
 *
 * @param[in] p2p_index     P2P entry index
 * @param[in] vif_del       Indicate if the VIF will be unregistered.
 ****************************************************************************************
 */
void p2p_cancel(uint8_t p2p_index, bool vif_del);

/**
 ****************************************************************************************
 * @bried Update the P2P entry parameters when VIF state is updated
 *
 * @param[in] p_vif_entry   VIF Entry
 * @param[in] active        Boolean value indicating if VIF is active or not
 ****************************************************************************************
 */
void p2p_set_vif_state(struct vif_info_tag *p_vif_entry, bool active);

/**
 ****************************************************************************************
 * @brief Return true if the GO device is present, else return false.
 *
 * @param[in]  p2p_index  Index of the P2P entry
 ****************************************************************************************
 */
bool p2p_is_present(uint8_t p2p_index);

/**
 ****************************************************************************************
 * @brief Look for P2P NOA Attribute inside a received beacon. Start/Update/Stop the NOA
 *        procedure accordingly with received information.
 *
 * @param[in]  p_vif_entry    VIF Entry
 * @param[in]  p_pyld_desc    Beacon Payload Descriptor
 * @param[in]  p_dma_hdrdesc  Beacon Header Descriptor
 ****************************************************************************************
 */
uint32_t p2p_cli_bcn_check_noa(struct vif_info_tag *p_vif_entry,
                               struct rx_pbd *p_pyld_desc, struct rx_dmadesc *p_dma_hdrdesc);

/**
 ****************************************************************************************
 * @brief Look for P2P NOA Attribute inside a received action frame. Start/Update/Stop the NOA
 *        procedure accordingly with received information.
 ****************************************************************************************
 */
void p2p_cli_handle_action(struct vif_info_tag *p_vif_entry, uint32_t a_frame,
                           uint16_t length, uint32_t rx_tsf);

/**
 ****************************************************************************************
 * @brief Handle P2P operation to be performed on STA_TBBT or AP_TBBT event.
 *
 * @param[in]  p_vif_entry    VIF Entry
 ****************************************************************************************
 */
void p2p_tbtt_handle(struct vif_info_tag *p_vif_entry);

/**
 ****************************************************************************************
 * @brief Handle P2P operation to be performed on Beacon reception.
 *
 * @param[in]  p_vif_entry    VIF Entry
 ****************************************************************************************
 */
void p2p_bcn_evt_handle(struct vif_info_tag *p_vif_entry);

#if (NX_P2P_GO && NX_POWERSAVE)
/**
 ****************************************************************************************
 * @brief Return if P2P PS Mode is active and can be used
 *        True if only one VIF is active and this VIF is a P2P GO VIF
 ****************************************************************************************
 */
bool p2p_go_check_ps_mode(void);
#endif //(NX_P2P_GO && NX_POWERSAVE)

#if (NX_P2P_GO)
/**
 ****************************************************************************************
 * @brief Handle update of Traffic status generated by the Traffic Detection module.
 *        When VIF is used for a P2P GO, it is used to stop/start NOA in order to increase
 *        the throughput if needed.
 *
 * @param[in] vif_index     Index of the VIF entry for which the status has been updated
 ****************************************************************************************
 */
void p2p_go_td_evt(uint8_t vif_index, uint8_t new_status);

/**
 ****************************************************************************************
 * @brief Start Opportunistic Power Save mode on P2P GO side
 *
 * @param[in] p_vif_entry    VIF Entry
 * @param[in] ctw            CTWindow
 ****************************************************************************************
 */
void p2p_go_oppps_start(struct vif_info_tag *p_vif_entry, uint8_t ctw);

/**
 ****************************************************************************************
 * @brief Stop Opportunistic Power Save mode on P2P GO side
 *
 * @param[in] p_vif_entry    VIF Entry
 ****************************************************************************************
 */
void p2p_go_oppps_stop(struct vif_info_tag *p_vif_entry);

/**
 ****************************************************************************************
 * @brief Check if GO PS state has to be updated once it is known that a peer CLI device
 *        is entered in Doze mode (used if OppPS has been enabled)
 *
 * @param[in] p_vif_entry   VIF Entry
 ****************************************************************************************
 */
void p2p_go_ps_state_update(struct vif_info_tag *p_vif_entry);

/**
 ****************************************************************************************
 * @brief Start NOA procedure on P2P GO side.
 *
 * @param[in] p_vif_entry    VIF Entry
 ****************************************************************************************
 */
bool p2p_go_noa_start(struct vif_info_tag *p_vif_entry, bool concurrent, bool dyn_noa,
                      uint8_t counter, uint32_t intv_us, uint32_t dur_us, uint32_t start_time);

/**
 ****************************************************************************************
 * @brief Start NOA procedure on P2P GO side.
 *
 * @param[in] p_vif_entry    VIF Entry
 * @param[in] noa_inst       NOA instance to be stopped
 * @param[in] host_req       Indicate if stop is request by host or not. Host is not
 * allowed to stop a NoA started for concurrent mode reasons.
 ****************************************************************************************
 */
bool p2p_go_noa_stop(struct vif_info_tag *p_vif_entry, uint8_t noa_inst, bool host_req);

#if (NX_CHNL_CTXT)
/**
 ****************************************************************************************
 * @brief Update duration of an existing NOA without stopping the NOA procedure.
 *
 * @param[in] p_vif_entry       VIF Entry
 * @param[in] noa_inst          NOA Instance for which duration has to be updated
 * @param[in] noa_dur_us        New NOA duration in microseconds
 ****************************************************************************************
 */
void p2p_go_noa_update_duration(struct vif_info_tag *p_vif_entry, uint8_t noa_inst, uint32_t noa_dur_us);
#endif //(NX_CHNL_CTXT)

/**
 ****************************************************************************************
 * @brief Return the additional length to consider in the beacon for the NOA attribute.
 * If the NOA is not part of the sent beacon, the function return 0.
 *
 * @param[in]  p2p_index    P2P entry index
 ****************************************************************************************
 */
uint8_t p2p_go_bcn_get_noa_len(uint8_t p2p_index);

/**
 ****************************************************************************************
 * @brief Initialize an NOA attribute payload buffer (attribute ID, attribute length, ...)
 *
 * @param[in]  p_noa_ie_elt     Address of a NOA attribute payload buffer
 ****************************************************************************************
 */
void p2p_go_bcn_init_noa_pyld(uint32_t a_noa_ie_elt);

/**
 ****************************************************************************************
 * @brief Trigger an update of the provided NOA attribute payload buffer.
 * This function is called by the mm_bcn module once it is possible to update the data
 * contained in the beacon after mm_bcn_update_p2p_noa is called.
 *
 * @param[in]  p2p_index     P2P entry index
 * @param[in]  p_noa_ie_elt     Address of a NOA attribute payload buffer
 ****************************************************************************************
 */
void p2p_go_bcn_upd_noa_pyld(uint8_t p2p_index, uint32_t a_noa_ie_elt);

/**
 ****************************************************************************************
 * @brief Callback to be used by mm_bcn module once operation required by p2p module
 * has been done (ADD, REMOVE, UPDATE).
 *
 * @param[in]  p2p_index     P2P entry index
 * @param[in]  operation     Required operation
 ****************************************************************************************
 */
void p2p_go_bcn_op_done(uint8_t p2p_index, uint8_t operation);

#if (NX_POWERSAVE)
/**
 ****************************************************************************************
 * @brief When HW is in doze mode, it does not trigger the TBTT interrupt anymore. Hence we use a
 * timer in order to wake up slightly before AP_TBTT occurs. This function is used as a
 * callback for the TBTT timer
 *
 ****************************************************************************************
 */
void p2p_go_pre_tbtt(struct vif_info_tag *p_vif_entry);
#endif //(NX_POWERSAVE)
#endif //(NX_P2P_GO)

#endif //(NX_P2P)

/// @} end of group

#endif // _P2P_H_
