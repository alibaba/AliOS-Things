/**
 ****************************************************************************************
 *
 * @file ps.h
 *
 * @brief Power-Save definitions.
 *
 * Copyright (C) RivieraWaves 2011-2016
 *
 ****************************************************************************************
 */

#ifndef _PS_H_
#define _PS_H_

/**
 ****************************************************************************************
 * @defgroup PS PS
 * @ingroup LMAC
 * @brief Power-Save mode implementation.
 * @{
 ****************************************************************************************
 */

/**
 * INCLUDE FILES
 ****************************************************************************************
 */

// for software configuration
#include "rwnx_config.h"
// for linked list definitions
#include "co_list.h"
// for mac_addr and other structure definitions
#include "mac.h"
// for maximum station index
#include "mac_common.h"
// for VIF definitions
#include "vif_mgmt.h"
#include "reg_mac_pl.h"
#include "co_math.h"
#if (NX_P2P)
#include "p2p.h"
#endif //(NX_P2P)
#if (RW_UMESH_EN)
#include "mesh_ps.h"
#endif //(RW_UMESH_EN)

#if NX_POWERSAVE

/**
 * DEFINES
 ****************************************************************************************
 */

// Definition of bits preventing from going to sleep (per VIF)
/// Station is waiting for beacon reception
#define PS_VIF_WAITING_BCN         CO_BIT(0)
/// Station is waiting for broadcast/multicast traffic from AP
#define PS_VIF_WAITING_BCMC        CO_BIT(1)
/// Station is waiting for unicast traffic from AP
#define PS_VIF_WAITING_UC          CO_BIT(2)
/// Station is waiting for WMM-PS end of service period
#define PS_VIF_WAITING_EOSP        CO_BIT(3)
/// Station is waiting for the end of the association procedure
#define PS_VIF_ASSOCIATING         CO_BIT(4)
/// P2P GO is supposed to be present
#define PS_VIF_P2P_GO_PRESENT      CO_BIT(5)

// Definition of bits preventing from going to sleep (global)
/// Upload of TX confirmations is ongoing
#define PS_TX_CFM_UPLOADING        CO_BIT(0)
/// A scanning process is ongoing
#define PS_SCAN_ONGOING            CO_BIT(1)
/// A request for going to IDLE is pending
#define PS_IDLE_REQ_PENDING        CO_BIT(2)
/// PSM is paused in order to allow data traffic
#define PS_PSM_PAUSED              CO_BIT(3)
/// A CAC period is active
#define PS_CAC_STARTED             CO_BIT(4)

//  Station is waiting for data
#define PS_WAITING_ADD_KEY        CO_BIT(6)

/// Mask showing that all ACs UAPSD enabled
#define PS_ALL_UAPSD_ACS           0x0F

/**
 * MACROS
 ****************************************************************************************
 */

#if (NX_DPSM)
/// Indicate if specified bit is set to 1 in dpsm_state
#define PS_DPSM_STATE_GET(bit_pos) \
        (ps_env.dpsm_state & (1 << PS_DPSM_STATE_ ## bit_pos))
/// Set specified bit to 1 in dpsm_state
#define PS_DPSM_STATE_SET(bit_pos) \
        (ps_env.dpsm_state |= (1 << PS_DPSM_STATE_ ## bit_pos))
/// Set specified bit to 0 in dpsm_state
#define PS_DPSM_STATE_CLEAR(bit_pos) \
        (ps_env.dpsm_state &= ~(1 << PS_DPSM_STATE_ ## bit_pos))
#endif //(NX_DPSM)

#endif //(NX_POWERSAVE)

/**
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Power Save mode setting
enum
{
    /// Power-save off
    PS_MODE_OFF,
    /// Power-save on - Normal mode
    PS_MODE_ON,
    /// Power-save on - Dynamic mode
    PS_MODE_ON_DYN,
};

#if (NX_POWERSAVE)

#if NX_UAPSD
/// Possible actions for UAPSD Timer
enum
{
    /// Start UAPSD Timer
    PS_UAPSD_TMR_START,
    /// Stop UAPSD Timer
    PS_UAPSD_TMR_STOP,
};
#endif

#if (NX_DPSM)
/// Bit position description for dpsm_state variable
enum ps_dpsm_state_bit_pos
{
    /// Indicate if DPSM is ON (Required by application)
    PS_DPSM_STATE_ON                = 0,
    /// Indicate if it has been required to pause PS Mode
    PS_DPSM_STATE_PAUSING,
    /// Indicate if it has been required to resume PS Mode
    PS_DPSM_STATE_RESUMING,
    /// Indicate if the PS Mode is currently paused
    PS_DPSM_STATE_PAUSE,
    /*
     * Indicate if MM_SET_PS_MODE_REQ has been received from UMAC while pausing/resuming
     * Mode required by UMAC will be stored in ps_env.dpsm_new_mode
     */
    PS_DPSM_STATE_SET_MODE_REQ,
};
#endif //(NX_DPSM)

/**
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// PS module environment
struct ps_env_tag
{
    /// Current Power Save mode state
    bool ps_on;
    /// TaskID of the task that requested the power save change
    ke_task_id_t taskid;
    /// Bitfield indicating which ongoing procedure prevent from going to sleep
    uint32_t prevent_sleep;
    /// Expected number of confirmations for NULL frame transmission
    uint8_t cfm_cnt;

    #if NX_UAPSD
    // Timer used for Traffic Detection Interval
    struct mm_timer_tag uapsd_timer;
    /// Flag indicating if UAPSD timer is currently active
    bool uapsd_tmr_on;
    /// Flag indicating if UAPSD is currently in use
    bool uapsd_on;
    /// UAPSD Timer timeout value, in microseconds
    uint32_t uapsd_timeout;
    #endif

    #if (NX_DPSM)
    /// Bit field containing different information used for DPSM
    uint8_t dpsm_state;
    /// Next PS Mode required by the upper layers
    uint8_t next_mode;
    #endif //(NX_DPSM)
};

/**
 * GLOBAL VARIABLES
 ****************************************************************************************
 */
extern struct ps_env_tag ps_env;

/**
 * FUNCTION PROTOTYPES
 ****************************************************************************************
 */
#if NX_UAPSD
__INLINE bool ps_uapsd_enabled(void)
{
    return (ps_env.uapsd_timeout != 0);
}
#endif

void ps_run_td_timer(void);

/**
 ****************************************************************************************
 * @brief Initialize all the entries of the station table.
 ****************************************************************************************
 */
void ps_init(void);


/**
 ****************************************************************************************
 * @brief Set the Power-save mode as requested by the upper layers.
 * This function sends the NULL frame to the AP to indicate them the mode change
 *
 * @param[in] mode       @PS_MODE_OFF or @PS_MODE_ON
 * @param[in] taskid     ID of the task that requested the power save, and to
 *                       which the confirmation will be sent
 ****************************************************************************************
 */
void ps_set_mode(uint8_t mode, ke_task_id_t taskid);

#if (RW_MESH_EN)
bool ps_check_tim(uint32_t a_tim, uint16_t aid);
#endif //(!RW_MESH_EN)

/**
 ****************************************************************************************
 * @brief Checks the TIM IE in the beacon to know if the station has to wait for
 * individually or group addressed traffic following this beacon.
 * If individually addressed traffic is buffered by AP, this function transmits the
 * PS poll frame.
 *
 * @param[in]  tim        Pointer to the TIM element in the beacon
 * @param[in]  len        Length of the beacon
 * @param[in]  vif_entry  Pointer to the VIF element associated to the beacon
 ****************************************************************************************
 */
void ps_check_beacon(struct rx_hd *rhd,uint32_t tim, uint16_t len, struct vif_info_tag *vif_entry);

/**
 ****************************************************************************************
 * @brief Checks the more data bit in the data or management frame passed as parameter
 * and behave accordingly
 *
 * @param[in]  frame      Pointer to the received data or management packet
 * @param[in]  statinfo   MAC HW status of the reception
 * @param[in]  vif_entry  Pointer to the VIF element associated to the frame
 ****************************************************************************************
 */
void ps_check_frame(uint8_t *frame, uint32_t statinfo, struct vif_info_tag *vif_entry);

#if (NX_UAPSD)
/**
 ****************************************************************************************
 * @brief Enable/disable the UAPSD for the HW queue and VIF passed as parameters
 *
 * @param[in]  vif_entry  Pointer to the VIF for UAPSD is enabled/disabled
 * @param[in]  hw_queue   HW queue index
 * @param[in]  uapsd      Flag indicating if U-APSD has to enabled or disabled
 ****************************************************************************************
 */
void ps_uapsd_set(struct vif_info_tag *vif_entry, uint8_t hw_queue, bool uapsd);
#endif //(NX_UAPSD)

#if (NX_UAPSD || NX_DPSM)
/**
 ****************************************************************************************
 * @brief Check if the frame is a UAPSD trigger frame. In such case, store the transmit
 * time in order to avoid sending a QoS-NULL trigger frame a short period after.
 *
 * @param[in]  staid  ID of the destination STA (0xFF if unknown)
 * @param[in]  tid    Priority index of the frame (0xFF if not applicable)
 ****************************************************************************************
 */
void ps_check_tx_frame(uint8_t staid, uint8_t tid);
#endif //(NX_UAPSD || NX_DPSM)

#if (NX_DPSM)
/**
 ****************************************************************************************
 * @brief Handle update of Traffic status generated by the Traffic Detection module.
 *        It is used in order to pause/restart the legacy power save mode if DPSM feature
 *        has been enabled.
 *
 * @param[in] vif_index     Index of the VIF entry for which the status has been updated
 * @param[in] new_status    Updated status
 ****************************************************************************************
 */
void ps_traffic_status_update(uint8_t vif_index, uint8_t new_status);
#endif //(NX_DPSM)

#if (NX_P2P && NX_UAPSD)
/**
 ****************************************************************************************
 * @brief Update P2P GO presence status update in order to send a trigger frame when
 *        an absence period is over after an interrupted Service Period.
 *
 * @param[in] p_vif_entry   VIF Entry
 * @param[in] absent        P2P GO presence status
 ****************************************************************************************
 */
void ps_p2p_absence_update(struct vif_info_tag *p_vif_entry, bool absent);
#endif //(NX_P2P && NX_UAPSD)

#endif

/// @}

#endif // _VIF_MGMT_H_
