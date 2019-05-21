/**
 ****************************************************************************************
 *
 * @file mesh_ps.h
 *
 * @brief Mesh Module Definitions. Mesh Power Save
 *
 * Copyright (C) RivieraWaves 2016-2016
 *
 ****************************************************************************************
 */

#ifndef _MESH_PS_H_
#define _MESH_PS_H_

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

/// Length of Mesh Awake Window duration field
#define MESH_PS_AWAKE_WINDOW_DUR_LEN        (2)
/// Default Mesh Awake Window duration value (in TUs)
#define MESH_PS_DEFAULT_AWAKE_WINDOW_DUR    (10)

/// Bit masks for per-mesh link prevent sleep value
/// A service period for which we are owner is currently in progress
#define MESH_PS_LINK_SP_OWNER               (CO_BIT(0))
/// A service period for which we are recipient is currently in progress
#define MESH_PS_LINK_SP_RECIPIENT           (CO_BIT(1))
/// Peer Mesh STA has indicated buffered BCMC traffic, waiting for end of this traffic
#define MESH_PS_LINK_WAIT_BCMC              (CO_BIT(2))
/// Waiting for reception of a peer mesh STA's beacon
#define MESH_PS_LINK_WAIT_BEACON            (CO_BIT(3))

/// Bit masks for per-mesh VIF prevent sleep values
/// Our Mesh Awake Window is currently opened, we are supposed to be awoken
#define MESH_PS_VIF_LOCAL_MAW               (CO_BIT(0))
/// A Power Save level transition is currently pending
#define MESH_PS_VIF_TRANSITION              (CO_BIT(1))
/// Waiting for confirmation of beacon transmission
#define MESH_PS_VIF_BEACON_TX               (CO_BIT(2))

/**
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Mesh Power Mode definition
enum mesh_ps_mode
{
    /// Unknown Mpde
    MESH_PS_UNKNOWN = 0,
    /// Active Mode
    MESH_PS_ACTIVE,
    /// Light Sleep Mode
    MESH_PS_LIGHT,
    /// Deep Sleep Mode
    MESH_PS_DEEP,
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
 * @brief Initialize Mesh PS module
 ****************************************************************************************
 */
void mesh_ps_init(void);

/**
 ****************************************************************************************
 * @brief Return true if Mesh PS module authorizes the system to enter in doze mode, else
 * false.
 ****************************************************************************************
 */
bool mesh_ps_sleep_check(void);

/**
 ****************************************************************************************
 * @brief Set the local Mesh Power Save mode. As the Power Management bit value is set
 * using a register, the local power save level is the same for all our peer Mesh STAs.
 *
 * @param[in] ps_state      New local Mesh PS mode
 ****************************************************************************************
 */
void mesh_ps_update_state(uint8_t ps_state);

/**
 ****************************************************************************************
 * @brief Add the Mesh Awake Window Information Element in a frame (Beacon or Probe
 * Response).
 *
 * @param[in|out] p_frame_addr      Address in memory in memory at which the MAW IE can be
 *                                  inserted
 * @param[in] maw_dur               Mesh Awake Window Duration
 *
 * @return Length of IE added in the frame
 ****************************************************************************************
 */
uint32_t mesh_ps_add_ie_mesh_awake_window(uint32_t *p_frame_addr, uint16_t maw_dur);

/**
 ****************************************************************************************
 * @brief Check if frame can be transmitted to a peer Mesh STA.
 *
 * @param[in] p_vif_entry       VIF on which the frame will be sent
 * @param[in] sta_idx           Index of STA to which the frame will be sent
 * @param[in] p_txdesc          TX Descriptor containing frame information
 *
 * @return true if frame can be transmitted, else false.
 ****************************************************************************************
 */
bool mesh_ps_tx_data_handle(struct vif_info_tag *p_vif_entry, uint8_t sta_idx, struct txdesc *p_txdesc);

/**
 ****************************************************************************************
 * @brief Handle reception of a data frame transmitted by a peer Mesh STA.
 * The function checks the Power Management bit and the Mesh Power Save Level bits in the
 * MAC header in order to detect any power save level transition from peer Mesh STA.
 * If peer Mesh STA or ourself is using a non active power save mode, the function also
 * checks the End of Service Period (EOSP)/Receiver Service Period Initiated (RSPI) bits
 * combination in order to detect start and end of a Service Period.
 *
 * @param[in] vif_idx       Index of the interface on which the frame has been received
 * @param[in] sta_idx       Index of the Mesh STA from which the frame has been received
 * @param[in] a_frame       Address of the frame in our memory
 ****************************************************************************************
 */
void mesh_ps_rx_data_handle(uint8_t vif_idx, uint8_t sta_idx, uint32_t a_frame);

/**
 ****************************************************************************************
 * @brief Handle reception of a beacon sent by a peer Mesh STA.
 * If we using a non active power save mode, the function checks the content of the TIM
 * information element in order to detect if peer mesh STA has buffered data for us. In that
 * case, we initiate a service period allowing to download the frames.
 * If peer mesh STA uses a non active power save mode, the function checks the presence
 * and if found the content of the Mesh Awake Window information element in order to monitor
 * awake period of the peer Mesh STA..
 *
 * @param[in] vif_idx       Index of the interface onwhich the beacon has been received
 * @param[in] sta_idx       Index of the Mesh STA which has sent the beacon
 * @param[in] a_bcn         Address of the beacon in the memory
 * @param[in] len_bcn       Beacon length.
 ****************************************************************************************
 */
void mesh_ps_rx_beacon_handle(uint8_t vif_idx, uint8_t sta_idx, uint32_t a_bcn, uint32_t len_bcn);

/**
 ****************************************************************************************
 * @brief Prepare beacon transmission and set duration of Mesh Awake Window if needed.
 *
 * @param[in] p_vif_entry       VIF Information
 * @param[in] p_mvif_entry      Mesh VIF Information
 ****************************************************************************************
 */
void mesh_ps_beacon_tx_handle(struct vif_info_tag *p_vif_entry, struct mesh_vif_info_tag *p_mvif_entry);

/**
 ****************************************************************************************
 * @brief Function handling transmission confirmation for the given VIF.
 *
 * @param[in] p_vif_entry   VIF on which the beacon has been transmitted
 ****************************************************************************************
 */
void mesh_ps_beacon_cfm_handle(struct vif_info_tag *p_vif_entry);

#endif //(RW_MESH_EN)

/// @}

#endif // _MESH_PS_H_
