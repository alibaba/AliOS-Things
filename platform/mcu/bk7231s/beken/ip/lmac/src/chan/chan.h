/**
 ****************************************************************************************
 * @file chan.h
 *
 * @brief MAC Channel Management module declarations.
 *
 * Copyright (C) RivieraWaves 2011-2016
 *
 ****************************************************************************************
 */

#ifndef _CHAN_H_
#define _CHAN_H_

/**
 *****************************************************************************************
 * @defgroup CHAN CHAN
 * @ingroup LMAC
 * @brief LMAC MAC Management module.
 * @{
 *****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "co_int.h"
#include "co_bool.h"
#include "mac_common.h"
#include "co_utils.h"
#include "ke_timer.h"
#include "mm_task.h"
#include "mm_timer.h"
#include "hal_desc.h"
#include "hal_machw.h"

#if (NX_CHNL_CTXT)

/**
 * DEFINES
 ****************************************************************************************
 */

/// Number of channel contexts for traffic
#define CHAN_TRAF_CTXT_CNT     (NX_CHAN_CTXT_CNT)
/// Number of channel context (+1 for the Scan Channel Context and +1 for the Remain on Channel Context)
#define CHAN_CHAN_CTXT_CNT     (CHAN_TRAF_CTXT_CNT + 1 + 1)
/// Index of the Scan Channel Context
#define CHAN_SCAN_CTXT_IDX     (CHAN_TRAF_CTXT_CNT)
/// Index of the Remain on Channel Context
#define CHAN_ROC_CTXT_IDX      (CHAN_SCAN_CTXT_IDX + 1)
/// Index of a Channel Context that is not linked with a VIF
#define CHAN_CTXT_UNUSED       (0xFF)

/// Duration of a slot (in TUs)
#define CHAN_SLOT_DURATION_TU  (1)
/// Duration of a slot (in us)
#define CHAN_SLOT_DURATION_US  (CHAN_SLOT_DURATION_TU * 1024)
/// Number of slots initially allocated for a VIF
#define CHAN_VIF_NB_SLOTS      (50)

/// Maximal TBTT presence duration (in slots)
#define CHAN_MAX_TBTT_PRES_DUR (10)
/// Minimal presence duration on a channel (in slots)
#define CHAN_MIN_PRES_DUR      (5)
/// Minimal timer value (in us)
#define CHAN_MIN_TIMER_VALUE   (2000)
/// Maximal priority value for a TBTT switch
#define CHAN_TBTT_PRIORITY_MAX (5)
/*
 * Delay between the start of a channel switch procedure and the time the new channel
 * should be active (in us)
 */
#define CHAN_SWITCH_DELAY      (2500)
/// Switch timeout duration
#define CHAN_SWITCH_TO_DUR     (4000)
/* Delay before switching on a Scan or a ROC channel, indicate minimal 
   duration between two successive scan/roc operations*/
#define CHAN_CONN_LESS_DELAY   (90000)

/**
 * ENUMERATIONS
 ****************************************************************************************
 */
/// Channel Context State Machine
enum chan_ctxt_status
{
    /// Context has not been scheduled
    CHAN_NOT_SCHEDULED = 0,
    /// Context has been scheduled but is not been programmed
    CHAN_NOT_PROG,
    /// HW is going to IDLE state
    CHAN_GOTO_IDLE,
    /// Wait for Notification of Absence confirmation
    CHAN_WAIT_NOA_CFM,
    /// Waiting for channel duration timer end
    CHAN_WAITING_END,
    /// Present on channel but End of Channel timer not running
    CHAN_PRESENT,
    /// Context is sending a notification of absence
    CHAN_SENDING_NOA,

    CHAN_CTXT_STATUS_MAX
};

/// TBTT Switch State Machine
enum chan_tbtt_status
{
    /// TBTT Switch Timer has not yet been programmed
    CHAN_TBTT_NOT_PROG = 0,
    /// TBTT Timer is programmed
    CHAN_TBTT_PROG,
    /// TBTT Presence is in progress
    CHAN_TBTT_PRESENCE,

    CHAN_TBTT_STATUS_MAX
};

/// Channel Environment status bits (see chan_env.status)
enum chan_env_status_bit
{
    /// Remain on Channel operation is waiting
    CHAN_ENV_ROC_WAIT_BIT = 0,
    /// Scan Operation is waiting
    CHAN_ENV_SCAN_WAIT_BIT,
    /// Remain on Channel operation is in progress
    CHAN_ENV_ROC_BIT,
    /// Scan Operation is in progress
    CHAN_ENV_SCAN_BIT,
    /// Connection Less Delay Timer is programmed
    CHAN_ENV_DELAY_PROG_BIT,
    /// Switch timeout has occurred
    CHAN_ENV_TIMEOUT_BIT,
    /// Beacon Detection is in progress
    CHAN_ENV_BCN_DETECT_BIT,

    CHAN_ENV_BIT_MAX,
};

/**
 * STRUCTURES/TYPES DEFINITION
 ****************************************************************************************
 */

/// Structure containing TBTT switch information
struct chan_tbtt_tag
{
    /// List header for chaining
    struct co_list_hdr list_hdr;

    /// TBTT switch time
    uint32_t time;
    /// VIF Index
    uint8_t vif_index;
    /// Priority of this TBTT switch
    uint8_t priority;

    /// Indicate if TBTT presence is in progress (@see enum chan_tbtt_status)
    uint8_t status;
};

/// Structure describing a channel context
struct chan_ctxt_tag
{
    /// List header for chaining
    struct co_list_hdr list_hdr;

    /// Parameters of the channel
    struct mm_chan_ctxt_add_req channel;

    /// TaskID of the task to send the switch confirmation to (TASK_NONE if no confirmation)
    ke_task_id_t taskid;

    /// Number of slots allocated for the channel
    uint16_t nb_slots;
    /// Number of remaining slots
    uint16_t nb_rem_slots;
    /// Number of slots reserved for TBTT presence
    uint8_t nb_res_slots;
    /// Status (@see enum chan_ctxt_status)
    uint8_t status;

    /// Index of the channel context
    uint8_t idx;
    /// Number of VIF currently linked with the channel
    uint8_t nb_linked_vif;
    /// VIF Index - For Scan and ROC Contexts ONLY
    uint8_t vif_index;

    #if (NX_P2P)
    /// Number of P2P VIF currently linked with the channel
    uint8_t nb_linked_p2p_vif;
    /// Number of P2P VIFs linked with the channel using P2P PS and currently present
    uint8_t nb_p2p_presence;
    #endif //(NX_P2P)

    #if (NX_P2P_GO)
    /// NOA Instance allocated for Concurrent Mode between GO and STA connections
    uint8_t p2p_noa_idx;
    #endif //(NX_P2P_GO)

    #if (TDLS_ENABLE)
    bool roc_tdls;
    #endif
};

/// LMAC MAC Management Context
struct chan_env_tag
{
    /// List of free channel contexts
    struct co_list list_free_ctxt;
    /// List of scheduled channel contexts
    struct co_list list_sched_ctxt;

    /// List of TBTT Switch information
    struct co_list list_tbtt;
    /// List of delayed TBTT Switch information
    struct co_list list_tbtt_delay;

    /// Operational channel
    struct chan_ctxt_tag *current_channel;
    /// Ongoing channel switch
    struct chan_ctxt_tag *chan_switch;

    /// TBTT Switch Timer
    struct mm_timer_tag tmr_tbtt_switch;
    /// Channel Distribution Event Timer
    struct mm_timer_tag tmr_cde;
    /// Channel Operation Timer
    struct mm_timer_tag tmr_ctxt_op;
    /// Scan/RoC Delay Timer
    struct mm_timer_tag tmr_conn_less;

    /// Channel Distribution Event Duration
    uint32_t cde_dur_us;
    /// Time of last CDE event (CDE Start, Channel Switch, ...)
    uint32_t cde_time;

    // Status (@see enum chan_env_status_bit)
    uint8_t status;
    /// Number of TX confirmations awaited before channel switching
    uint8_t cfm_cnt;

    /**
     * Number of Channel Context currently scheduled (match with number of element in switch_list)
     * If value is higher than 1, then Channel Distribution Event is used
     */
    uint8_t nb_sched_ctxt;

    #if (NX_POWERSAVE)
    /// Flag saving the value of the PM bit
    uint8_t pm;
    #endif //(NX_POWERSAVE)
};

// Forward declaration
struct vif_info_tag;


/**
 * GLOBAL VARIABLES
 ****************************************************************************************
 */

extern struct chan_env_tag chan_env;

/**
 * FUNCTION PROTOTYPES
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief MM Channel Module initialization function.
 * This function is called after reset and initializes all MM Channel related env and data.
 ****************************************************************************************
 */
void chan_init(void);

#if (NX_HW_SCAN)
/**
 ****************************************************************************************
 * @brief Request to go to a specific channel for scanning.
 * This function takes care on scheduling the channel switch at an appropriate time. Upon
 * switching time, the MM channel will inform the AP(s) on which we are connected that we
 * are going to power-save so that they start buffering the traffic for us.
 *
 * @param[in] band          Band to be scanned (0: 2.4G, 1: 5G)
 * @param[in] freq          Frequency to be scanned
 * @param[in] pwr           Max power allowed (in dBm)
 * @param[in] duration      Duration of the scan on the requested channel
 * @param[in] vif_index     Index of VIF on which scan will be performed
 ****************************************************************************************
 */
void chan_scan_req(uint8_t band, uint16_t freq, int8_t pwr,
                   uint32_t duration_us, uint8_t vif_index);

#endif //(NX_HW_SCAN)

/**
 ****************************************************************************************
 * @brief Request to go to a specific channel for a given duration.
 ****************************************************************************************
 */
uint8_t chan_roc_req(struct mm_remain_on_channel_req const *req, ke_task_id_t taskid);

/**
 ****************************************************************************************
 * @brief Request to allocate a new channel context.
 *
 * @param[in]  channel  Pointer to the channel parameter structure
 * @param[out] idx      Index of the allocated channel context
 *
 * @return CO_OK if allocation was successful, CO_FAIL otherwise
 ****************************************************************************************
 */
uint8_t chan_ctxt_add(struct mm_chan_ctxt_add_req const *channel, uint8_t *idx);

/**
 ****************************************************************************************
 * @brief Free a previously allocated channel context.
 *
 * @param[in] idx      Index of the channel context to be freed
 ****************************************************************************************
 */
void chan_ctxt_del(uint8_t idx);

/**
 ****************************************************************************************
 * @brief Link a channel context to a VIF.
 *
 * @param[in] vif_idx   Index of the VIF
 * @param[in] chan_idx  Index of the channel context to be linked
 ****************************************************************************************
 */
void chan_ctxt_link(uint8_t vif_idx, uint8_t chan_idx);

/**
 ****************************************************************************************
 * @brief Unlink the channel context from a VIF.
 *
 * @param[in] vif_idx   Index of the VIF
 ****************************************************************************************
 */
void chan_ctxt_unlink(uint8_t vif_idx);

/**
 ****************************************************************************************
 * @brief Request to update the parameters of a channel context.
 *
 * @param[in]  update  Pointer to the new parameters structure
 ****************************************************************************************
 */
void chan_ctxt_update(struct mm_chan_ctxt_update_req const *update);

/**
 ****************************************************************************************
 * @brief Request to schedule the switch to a channel context.
 *
 * @param[in]  p_sched_req  Pointer to the channel scheduling structure
 * @param[in]  taskid       Kernel task ID of the task the confirmation has to be sent to
 ****************************************************************************************
 */
void chan_ctxt_sched(struct mm_chan_ctxt_sched_req const *p_sched_req, ke_task_id_t taskid);

/**
 ****************************************************************************************
 * @brief Update TBTT switch time for the provided VIF. TBTT switch will occur
 *        CHAN_SWITCH_DELAY before TBTT (shall be before AP_TBTT or STA_TBTT).
 *
 * @param[i] p_vif_entry    VIF Entry
 * @param[i] tbtt_time      Time of next TBTT
 ****************************************************************************************
 */
void chan_tbtt_switch_update(struct vif_info_tag *p_vif_entry, uint32_t tbtt_time);

/**
 ****************************************************************************************
 * @brief Handle end of TBTT Presence either after beacon and associated traffic reception
 *        or after timeout during beacon reception attempt.
 *
 * @param[in] p_vif_entry   VIF Entry
 ****************************************************************************************
 */
void chan_bcn_to_evt(struct vif_info_tag *p_vif_entry);

/**
 ****************************************************************************************
 * @brief Start a Remain on Channel procedure (duration is the beacon interval) in order
 *        to catch the beacon sent by a peer AP device.
 *
 * @param[i] p_vif_entry    VIF Entry
 ****************************************************************************************
 */
void chan_bcn_detect_start(struct vif_info_tag *p_vif_entry);

#if (NX_P2P)
/**
 ****************************************************************************************
 * @brief Function used by the P2P module to notify the chan module about an update of
 *        presence status of a GO device.
 *
 * @param[in] p_chan_entry
 * @param[in] absence        True if GO is absent, false else
 ****************************************************************************************
 */
void chan_p2p_absence_update(struct chan_ctxt_tag *p_chan_entry, bool absence);
#endif //(NX_P2P)

/**
 ****************************************************************************************
 * @brief Check if the VIF passed as parameter has currently access to the channel.
 *
 * @param[in] vif_entry   Pointer to the VIF structure
 *
 * @return true if the device is on channel, false otherwise
 ****************************************************************************************
 */
bool chan_is_on_channel(struct vif_info_tag *vif_entry);

/**
 ****************************************************************************************
 * @brief Check if transmission is allowed on a vif's channel
 *
 * Transmission is allowed as long as the vif is on the current channel and no channel
 * switch is in progress.
 * Exception: Transmitting NULL frame for notification of absence
 * (i.e. status==CHAN_SENDING_NOA) is allowed during a channel switch
 *
 * @param[in] vif_entry   Pointer to the VIF structure
 *
 * @return true if a frame can be pushed, false otherwise
 ****************************************************************************************
 */
bool chan_is_tx_allowed(struct vif_info_tag *p_vif_entry);
uint16_t chan_get_vif_frequency(struct vif_info_tag *vif);

/**
 ****************************************************************************************
 * @brief Update TX power for a channel context.
 * Set tx_power to the minimum value allowed among all vifs linked to this channel context.
 * This power value is used to configure tx power used in frame generated by HW (Ack,...).
 * Data frame will use the tx power configured at vif level.
 *
 * @param[in] p_chan_entry
 ****************************************************************************************
 */
void chan_update_tx_power(struct chan_ctxt_tag *p_chan_entry);

#endif //(NX_CHNL_CTXT)

/// @} end of group

#endif // _CHAN_H_
