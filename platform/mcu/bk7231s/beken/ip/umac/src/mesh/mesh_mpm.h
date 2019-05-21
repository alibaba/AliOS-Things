/**
 ****************************************************************************************
 *
 * @file mesh_mpm.h
 *
 * @brief Mesh Module Definitions. Mesh Peering Management Protocol
 *
 * Copyright (C) RivieraWaves 2016-2016
 *
 ****************************************************************************************
 */

#ifndef _MESH_MPM_H_
#define _MESH_MPM_H_

/**
 ****************************************************************************************
 * @defgroup MESH MESH
 * @ingroup UMAC
 * @brief Mesh Module Definitions.
 * @{
 ****************************************************************************************
 */

/**
 * INCLUDE FILES
 ****************************************************************************************
 */

/// RW Stack Configuration
#include "mesh.h"

#if (RW_MESH_EN)

/**
 * DEFINES
 ****************************************************************************************
 */

/// Default retry timeout duration (in TUs) (dot11MeshRetryTimeout)
#define MESH_MPM_DEFAULT_RETRY_TO_TU         (100)
/// Default confirm timeout duration (in TUs) (dot11MeshConfirmTimeout)
#define MESH_MPM_DEFAULT_CONFIRM_TO_TU       (100)
/// Default holding timeout duration (in TUs) (dot11MeshHoldingTimeout)
#define MESH_MPM_DEFAULT_HOLDING_TO_TU       (100)
/// Initial maximum number of retries for Mesh Peering Open frame (dot11MeshMaxRetries)
#define MESH_MPM_DEFAULT_MAX_RETRIES        (3)

/**
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Events for MPM state machine transitions (see IEEE Std 802.11-2012 section 13.4.3/4)
enum mesh_mpm_event
{
    /// Internally initiate a mesh peering establishment with a candidate peer mesh STA
    MESH_MPM_EVT_ACTOPN = 0,
    /// Reception of a valid Mesh Peering Open frame
    MESH_MPM_EVT_OPN_ACPT,
    /// Reception of a valid Mesh Peering Confirm frame
    MESH_MPM_EVT_CNF_ACPT,
    /// retryTimer has expired and dot11MeshMaxRetries has not been reached
    MESH_MPM_EVT_TOR1,
    /// retryTimer has expired and dot11MeshMaxRetries has been reached
    MESH_MPM_EVT_TOR2,
    /// holdingTimer has expired
    MESH_MPM_EVT_TOH,
    /// confirmTimer has expired
    MESH_MPM_EVT_TOC,
    /// Reception of a non valid Mesh Peering Open frame
    MESH_MPM_EVT_OPN_RJCT,
    /// Reception of a non valid Mesh Peering Confirm frame
    MESH_MPM_EVT_CNF_RJCT,
    /// Reception of a Mesh Peering Close frame
    MESH_MPM_EVT_CLS_ACPT,
    /**
     * A received Mesh Peering Open frame is rejected either because the maximum number of
     * peer mesh STAs has been reached or because the received frame was not valid
     */
    MESH_MPM_EVT_REQ_RJCT,
    /// Cancel of a peering is internally required
    MESH_MPM_EVT_CNCL,

    /// Number of events
    MESH_MPM_EVT_MAX,
};

/// State of MPM FSM (see IEEE Std 802.11-2012 section 13.4.2)
enum mesh_mpm_state
{
    /**
     * IDLE state
     * Ready to start a new mesh peering instance by either passively listening for an
     * incoming Mesh Peering Open frame or actively initiating a mesh peering instance.
     */

    MESH_MPM_IDLE = 0,
    /**
     * OPN_SNT state
     * Has sent a Mesh Peering Open frame and is waiting for a Mesh Peering Open frame
     * and Mesh Peering Confirm frame from the candidate peer mesh STA
     */
    MESH_MPM_OPN_SNT,
    /**
     * CNF_RCVD state
     * Has received a Mesh Peering Confirm frame, but has not received a Mesh Peering
     * Open frame.
     */
    MESH_MPM_CNF_RCVD,
    /**
     * OPEN_RCVD state
     * Has received only the Mesh Peering Open frame but not the Mesh Peering Confirm
     * frame.
     */
    MESH_MPM_OPN_RCVD,
    /**
     * ESTAB state
     * Has received both the Mesh Peering Open and the Mesh Peering Confirm frames and has
     * also sent both the Mesh Peering Open and the Mesh Peering Confirm frames.
     */
    MESH_MPM_ESTAB,
    /**
     * HOLDING state
     * Is closing the mesh peering instance with the peer mesh STA or the candidate peer
     * Mesh STA.
     */
    MESH_MPM_HOLDING,

    // Number of states
    MESH_MPM_STATE_MAX,
};

/**
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/**
 * GLOBAL VARIABLES
 ****************************************************************************************
 */

/**
 * FUNCTION PROTOTYPES
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize Mesh MPM module
 ****************************************************************************************
 */
void mesh_mpm_init(void);

/**
 ****************************************************************************************
 * @brief Start peering with a given mesh STA
 *
 * @param[in] p_sta_entry   STA Information
 ****************************************************************************************
 */
void mesh_mpm_start_peering(struct sta_info_tag *p_sta_entry);

/**
 ****************************************************************************************
 * @brief Stop peering with a given mesh STA
 *
 * @param[in] p_mlink_entry   Mesh Link Information
 ****************************************************************************************
 */
void mesh_mpm_stop_peering(struct mesh_link_info_tag *p_mlink_entry);

/**
 ****************************************************************************************
 * @brief Handle reception of a MPM frame:
 *      - Mesh Peering Open frame
 *      - Mesh Peering Confirm frame
 *      - Mesh Peering Close frame
 *
 * @param[in] sta_idx       STA Index of the link on which frame has been received
 * @param[in] type          Type of MPM frame that has been received (open, confirm, close)
 * @param[in] p_addr        Pointer to the address of the first byte after Self Protected
 *                          Action Frame code
 * @param[in] p_len         Pointer to the remaining length of data to read
 ****************************************************************************************
 */
void mesh_mpm_handle_frame(uint8_t sta_idx, uint8_t type, uint32_t *p_addr, uint16_t *p_len);

#endif //(RW_MESH_EN)

/// @}

#endif // _MESH_MPM_H_
