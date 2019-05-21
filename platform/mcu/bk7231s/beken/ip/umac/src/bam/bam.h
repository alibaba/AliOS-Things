/**
 ****************************************************************************************
 *
 * @file bam.h
 *
 * @brief Functions and structures used by the UMAC ME BAM (Block ACK Manager) module.
 *
 * Copyright (C) RivieraWaves 2011-2016
 *
 ****************************************************************************************
 */

#ifndef _BAM_H_
#define _BAM_H_

/** @defgroup BAM BAM
 * @ingroup UMAC
 * @brief Declaration of all structures and functions used by the BAM module.
 * @{
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "co_status.h"
#include "mac.h"
#include "mac_frame.h"
#include "ke_task.h"

/* DEFINES
 ****************************************************************************************
 */

/// Maximum index of the task instance
#if NX_AMPDU_TX || NX_REORD
#if NX_REORD > NX_MAX_BA_TX
#define BAM_IDX_MAX            NX_REORD
#else
#define BAM_IDX_MAX            NX_MAX_BA_TX
#endif
#else
#define BAM_IDX_MAX            1
#endif

/// Invalid index of the task instances - 1 instance for manager
#define BAM_INVALID_TASK_IDX   (BAM_IDX_MAX + 1)

/// Maximal time between two consequent packet before triggering a BA AGG establishment (in us)
#define BAM_BA_AGG_DETECT_DURATION          (500)
/// Maximal time before considering that BA_ADD_REQ won't receive any response
#define BAM_RESPONSE_TO_DURATION            (500)
/// Maximal time between two consequent packet before considering a BA AGG can be deleted (in number of TU)
#define BAM_INACTIVITY_TO_DURATION          (2000)
/// Minimal time between sending two consequent ADDBA Req if error detected on the first one (in us) - 1s
#define BAM_ADDBA_REQ_INTERVAL              (1000000)

/// ADDBA Request Offsets - From Action Category
#define BAM_ADDBAREQ_BA_PARAM_OFFSET       (3)
#define BAM_ADDBAREQ_BA_TIMEOUT_OFFSET     (5)
#define BAM_ADDBAREQ_BA_SSC_OFFSET         (7)
/// ADDBA Response Offsets - From Action Category
#define BAM_ADDBARSP_STATUS_OFFSET         (3)
#define BAM_ADDBARSP_BA_PARAM_OFFSET       (5)
#define BAM_ADDBARSP_BA_TIMEOUT_OFFSET     (7)
/// DELBA Offsets - From Action Category
#define BAM_DELBA_PARAM_OFFSET             (2)
#define BAM_DELBA_REASON_OFFSET            (4)

/// Action Frames Lengths
#define BAM_ADDBAREQ_LENGTH     (9)
#define BAM_ADDBARSP_LENGTH     (9)
#define BAM_DELBA_LENGTH        (6)

/// Block Ack Parameter Set Offsets and Masks
#define BAM_BA_PARAM_AMSDU_SUP_OFFSET   (0)
#define BAM_BA_PARAM_AMSDU_SUP_MASK     (0x0001)
#define BAM_BA_PARAM_BA_POL_OFFSET      (1)
#define BAM_BA_PARAM_BA_POL_MASK        (0x0002)
#define BAM_BA_PARAM_TID_OFFSET         (2)
#define BAM_BA_PARAM_TID_MASK           (0x003C)
#define BAM_BA_PARAM_BUFF_SIZE_OFFSET   (6)
#define BAM_BA_PARAM_BUFF_SIZE_MASK     (0xFFC0)

/// DELBA Parameters Offsets and Masks
#define BAM_DELBA_PARAM_INITIATOR_OFFSET     (11)
#define BAM_DELBA_PARAM_INITIATOR_MASK       (0x0800)
#define BAM_DELBA_PARAM_TID_OFFSET           (12)
#define BAM_DELBA_PARAM_TID_MASK             (0xF000)

/// BA/BAR Control Offsets and Masks
#define BAM_BA_CTRL_POLICY_OFFSET       (0)
#define BAM_BA_CTRL_POLICY_MASK         (0x0001)
#define BAM_BA_CTRL_MULTI_TID_OFFSET    (1)
#define BAM_BA_CTRL_MULTI_TID_MASK      (0x0002)
#define BAM_BA_CTRL_COMP_BITMAP_OFFSET  (2)
#define BAM_BA_CTRL_COMP_BITMAP_MASK    (0x0004)
#define BAM_BA_CTRL_COMP_BITMAP_OFFSET  (2)
#define BAM_BA_CTRL_COMP_BITMAP_MASK    (0x0004)
#define BAM_BA_CTRL_TID_INFO_OFFSET     (11)
#define BAM_BA_CTRL_TID_INFO_MASK       (0xF000)

/// Basic SSC Offsets and Masks
#define BAM_BASIC_SSC_FN_OFFSET         (0)
#define BAM_BASIC_SSC_FN_MASK           (0x000F)
#define BAM_BASIC_SSC_SSN_OFFSET        (4)
#define BAM_BASIC_SSC_SSN_MASK          (0xFFF0)

/// Maximum size of the transmit window
#define BAM_MAX_TX_WIN_SIZE             64

/**
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Block Ack Policy Values
enum bam_ba_policy
{
    /// Immediate Block Ack
    BAM_BA_POL_IMMEDIATE = 1,
    /// Delayed Block Ack
    BAM_BA_POL_DELAYED,
};

/// Packet policy values
enum bam_pkt_policy
{
    // Send packet as a singleton, BA Agg no yet established
    BAM_PKT_POL_SINGLETON = 0,
    // Send packet as part of an A-MPDU
    BAM_PKT_POL_AMPDU
};

/// Enum listing the possible states of a BAW element
enum bam_baw_state
{
    /// Element of the BAW is free
    BAW_FREE,
    /// Element of the BAW is waiting for its acknowledgment
    BAW_PENDING,
    /// Element of the BAW has received its acknowledgment
    BAW_CONFIRMED,
};

/**
 * MACROS
 ****************************************************************************************
 */

/// Get field from Block Ack Parameter field
#define BAM_BA_PARAM_GET(ba_param, field)                            \
            ((ba_param & BAM_BA_PARAM_ ## field ## _MASK)            \
                            >> BAM_BA_PARAM_ ## field ## _OFFSET)

/// Get field from DELBA Parameter field
#define BAM_DELBA_PARAM_GET(delba_param, field)                      \
            ((delba_param & BAM_DELBA_PARAM_ ## field ## _MASK)       \
                            >> BAM_DELBA_PARAM_ ## field ## _OFFSET)

/**
 * TYPE DEFINITIONS
 ****************************************************************************************
 */
// Forward declarations
struct txdesc;

/// ADDBA Request frame Action field format (see IEEE 802.11-2012, section 8.5.5.2)
struct bam_addba_req
{
    /// Category - Set to 3 (representing Block Ack)
    uint8_t category;
    /// Block Ack Action - Set to 0 (representing ADDBA request)
    uint8_t action;
    /// Dialog Token - Copied from received ADDBA Request frame
    uint8_t dialog_token;
    /**
     * Block Ack Parameter Set (from LSB to MSB)
     * ---------------------------------------------------------------
     * | A-MSDU Supported | Block Ack Policy |   TID   | Buffer Size |
     * |       1 bit      |       1 bit      |  4 bits |    10 bits  |
     * ---------------------------------------------------------------
     */
    uint16_t ba_param_set;
    /// Block Ack Timeout Value - In TUs. A value of 0 disables the timeout
    uint16_t ba_timeout;
    /**
     * Block Ack Starting Sequence Control (from LSB to MSB)
     * -------------------------------------
     * | Fragment Number | Starting Seq Nb |
     * |    4 bits (0)   |     12 bits     |
     * -------------------------------------
     */
    uint16_t ba_start_sc;
};

/// ADDBA Response frame Action field format (see IEEE 802.11-2012, section 8.5.5.3)
struct bam_addba_rsp
{
    /// Category - Set to 3 (representing Block Ack)
    uint8_t category;
    /// Block Ack Action - Set to 1 (representing ADDBA response)
    uint8_t action;
    /// Dialog Token - Copied from received ADDBA Request frame
    uint8_t dialog_token;
    /// Status Code
    uint16_t status_code;
    /**
     * Block Ack Parameter Set (from LSB to MSB)
     * ---------------------------------------------------------------
     * | A-MSDU Supported | Block Ack Policy |   TID   | Buffer Size |
     * |       1 bit      |       1 bit      |  4 bits |    10 bits  |
     * ---------------------------------------------------------------
     */
    uint16_t ba_param_set;
    /// Block Ack Timeout Value - In TUs. A value of 0 disables the timeout
    uint16_t ba_timeout;
};

/// DELBA frame Action field format (see IEEE 802.11-2012, section 8.5.5.4)
struct bam_delba
{
    /// Category - Set to 3 (representing Block Ack)
    uint8_t category;
    /// Block Ack Action - Set to 2 (representing DELBA)
    uint8_t action;
    /**
     * DELBA Parameter Set (from LSB to MSB)
     * ------------------------------------
     * |  Reserved  | Initiator |   TID   |
     * |   11 bits  |   1 bit   |  4 bits |
     * ------------------------------------
     */
    uint16_t delba_param;
    /// Reason Code
    uint16_t reason_code;
};

/**
 * GLOBAL STRUCTURES
 ****************************************************************************************
 */
/**
 * Per RA/TID Data for AMPDU TX
 */
struct bam_baw {
    /// Starting Seq Num of BlockAck window
    uint16_t fsn;
    /// State of each element of the BA window BAW_{FREE,PENDING,CONFIRMED}
    uint8_t states[BAM_MAX_TX_WIN_SIZE];
    /// Index in the state table corresponding to the fsn value
    uint8_t fsn_idx;
    /// Size of the receiver reordering window
    uint8_t buf_size;
};

/// Declaration of BAM environment.
struct bam_env_tag
{
    /// Packet Counter
    uint32_t pkt_cnt;
    /// Last activity time (TX or RX) on this BA agreement
    uint32_t last_activity_time;
    /// Start sequence Number of BA
    uint16_t ssn;
    /// BA Setup timeout value
    uint16_t ba_timeout;
    /// Index of the station
    uint8_t  sta_idx;
    /// BA Device Type Originator/Responder
    uint8_t  dev_type;
    /// Block-Ack Policy Setup Immedaite/Delayed
    uint8_t  ba_policy;
    /// Number of buffers available for this BA
    uint8_t  buffer_size;
    /// TID for block ack setting
    uint8_t  tid;
    /// Dialog token
    uint8_t  dialog_token;
    /// Transmit window control structure
    struct bam_baw baw;
    /// Is AMSDU allowed
    uint8_t amsdu;
};

/**
 * GLOBAL VARIABLES DEFINITION
 ****************************************************************************************
 */

/// BAM module environment declaration.
extern struct bam_env_tag bam_env[BAM_IDX_MAX];

/**
 * FUNCTION PROTOTYPES
 ****************************************************************************************
 */

// Used to build the BA parameters
__INLINE uint16_t bam_build_baparamset(struct bam_env_tag *bam_env)
{
    uint16_t value = 0;

    /* The Block Ack Parameter Set field
     *
     *******************************************************************
     *  B0              *  B1               * (B2 -> B5)*  (B6->B15)   *
     *  A-MSD Supported *  Block Ack Policy * TID       *  Buffer Size *
     *******************************************************************
     *
     */
    // Fill the A-MSDU Supported bit
    value |= MAC_BA_PARAMSET_AMSDU_MASK;
    // Fill the Block Ack Policy bit
    value |= (bam_env->ba_policy << MAC_BA_PARAMSET_POLICY_OFT);
    // Fill the TID bits
    value |= (bam_env->tid << MAC_BA_PARAMSET_TID_OFT);
    // Fill the Buffer Size bits
    value |= (bam_env->buffer_size << MAC_BA_PARAMSET_BUFFER_OFT);

    return (value);
}

/**
****************************************************************************************
* @brief Initialize the BAM context.
*
*
* @return   None
*
****************************************************************************************
*/
void bam_init(void);

#if NX_AMPDU_TX || NX_REORD
/**
****************************************************************************************
* @brief Reset the BA parameters in the station info table
*
* @param[in]  bam_task_idx  indicates BAM task index
* @param[in]  sta_idx       indicates station index
* @param[in]  tid           indicates Traffic ID
*
* @return     None
*
****************************************************************************************
*/

void bam_param_sta_info_tab_reset(uint16_t bam_idx);

/**
****************************************************************************************
* @brief Allocate the BAM task.
*
* @return     bam_task_idx    indicates BAM task index
*
****************************************************************************************
*/
uint16_t bam_alloc_new_task(void);

/**
****************************************************************************************
* @brief Delete a provided Bloc kAck agreement
*
****************************************************************************************
*/
void bam_delete_ba_agg(uint8_t bam_idx);

/**
****************************************************************************************
* @brief Delete all Block Ack agreements linked to the specified staid
*
****************************************************************************************
*/
void bam_delete_all_ba_agg(uint8_t sta_idx);

/**
****************************************************************************************
* @brief start the BAM_INACTIVITY_TIMEOUT_IND timer
*
* @param[in]  bam_task_idx  indicates BAM task index
*
* @return     None
*
****************************************************************************************
*/
void bam_start_inactivity_timer(uint16_t  bam_task_idx);

/**
****************************************************************************************
* @brief Send MM_BAADD_REQ to LMAC.
*
* @param[in]  sta_idx       indicates station index
* @param[in]  bam_task_idx  indicates BAM task index
*
* @return     None
*
****************************************************************************************
*/
void bam_send_mm_ba_add_req(uint16_t sta_idx, uint16_t bam_idx);
/**
****************************************************************************************
* @brief Send Send MM_BADEL_REQ to LMAC
*
* @param[in]  sta_idx       indicates station index
* @param[in]  bam_task_idx  indicates BAM task index
*
* @return     None
*
****************************************************************************************
*/
void bam_send_mm_ba_del_req(uint16_t sta_idx, uint16_t bam_idx);
#endif

/**
****************************************************************************************
* @brief Send AIR_ADDBA_RSP.
*
* @param[in]  sta_idx       indicates station index
* @param[in]  bam_env       Pointer to the BAM environment, may be NULL in case of ADDBA
*                           RSP transmission
* @param[in]  action        Action to perform
* @param[in]  param         Param to be put (used only for ADDBA RSP)
* @param[in]  status_code   Status of the response/Reason for deletion (used for ADDBA RSP
*                           /DELBA only)
*
****************************************************************************************
*/
void bam_send_air_action_frame(uint8_t sta_idx,
                               struct bam_env_tag *bam_env,
                               uint8_t action,
                               uint8_t dialog_token,
                               uint16_t param,
                               uint16_t status_code);

#if NX_AMPDU_TX
/**
****************************************************************************************
* @brief Check if a BlockAck agreement is currently active for the RA/TID for which the
* packet is sent.
* This function is called each time a new packet is pushed to the TX path. If a BlockAck
* agreement is active, then the TX descriptor is updated accordingly to allow the LMAC
* to aggregate the packet in a A-MPDU.
* If no agreement is in place, the function initiates the creation of such an agreement
* for the future transmissions.
*
* @param[in] txdesc TX descriptor describing the packet to transmit
*
****************************************************************************************
*/
void bam_check_ba_agg(struct txdesc *txdesc);

/**
****************************************************************************************
* @brief Handles status of a transmission under BA. Move the transmission window
* accordingly and returns the number of credits allocated to the host for the next
* transmissions.
*
* @param[in] txdesc  Pointer to the packet descriptor
* @param[in] success Flag indicating if the transmission was successful (true) or not
*                    (false)
*
* @return The number of flow control credits allocated (or deallocated) for the RA/TID
* pair. Can be negative in case of credit deallocation.
*
****************************************************************************************
*/
int8_t bam_tx_cfm(struct txdesc *txdesc, bool success);

/**
****************************************************************************************
* @brief Initialize the transmission window structure.
* This function is called upon the creation of a TX block ack agreement.
*
* @param[in] bam_env Pointer to the BAM structure allocated for the new agreement
*
****************************************************************************************
*/
void bam_baw_init(struct bam_env_tag *bam_env);

/**
****************************************************************************************
* @brief Flush the transmission window.
* This function then returns the number of credits that should be added/removed for the
* associated RA/TID queue.
*
* @param[in] baw Pointer to the transmission window to be flushed
*
* @return The number of credits that need to be added/removed
*
****************************************************************************************
*/
int8_t bam_flush_baw(struct bam_baw *baw);
#endif

#if NX_REORD
/**
****************************************************************************************
* @brief
*
****************************************************************************************
*/
void bam_rx_active(uint8_t sta_idx, uint8_t tid);
#endif

/// @} end of group

#endif // _BAM_H_
