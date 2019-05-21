/**
 ****************************************************************************************
 *
 * @file apm.h
 *
 * Copyright (C) RivieraWaves 2011-2016
 *
 * @brief Declaration of the APM module environment.
 *
 ****************************************************************************************
 */
#ifndef _APM_H_
#define _APM_H_

/** @defgroup APM APM
* @ingroup UMAC
* @brief Declaration of all structures and functions used by the APM module.
*/

#include "mac.h"
#include "co_list.h"
#include "rwnx_config.h"

#if NX_BEACONING

/// APM environment declaration
struct apm
{
    /// Pointer to the AP parameters
    struct apm_start_req const *param;
    /// List of BSS configuration messages to send
    struct co_list bss_config;
    /// Used in the Aging Algorithm
    uint8_t aging_sta_idx;
};


/// Aging Duration : The time period is 100 sec
#define AGING_DURATION       25 * TU_DURATION * TU_DURATION
/// number of stations to be checked for aging
#define MAC_AGING_STA                    10
/// the threshold for null frame
#define STAID_NOTRAFFIC_THRESHOLD_NULL   3
/// the value of the QOS capability info
#define QOS_CAPA_VALUE                  0x10

// Forward declaration
struct vif_info_tag;

/*
* FUNCTION PROTOTYPES
****************************************************************************************
*/

/**
****************************************************************************************
* @brief Initialize the APM context
****************************************************************************************
*/
void apm_init(void);

/**
****************************************************************************************
* @brief Send the AP starting confirmation to the upper layers
*
* @param[in] status Status of the AP starting procedure
****************************************************************************************
*/
void apm_start_cfm(uint8_t status);


/**
****************************************************************************************
* @brief Set the BSS parameters to the LMAC/MACHW
****************************************************************************************
*/
void apm_set_bss_param(void);

/**
 ****************************************************************************************
 * @brief Send the next BSS parameter message present in the list
 *
 ****************************************************************************************
 */
void apm_send_next_bss_param(void);

/**
****************************************************************************************
* @brief Send the AP beacon information to the Lower MAC
****************************************************************************************
*/
void apm_bcn_set(void);

/**
****************************************************************************************
* @brief Stop the AP
*
* @param[in] vif Pointer to the VIF instance
****************************************************************************************
*/
void apm_stop(struct vif_info_tag *vif);
void apm_send_action_csa_frame(struct vif_info_tag *vif_entry);

/// APM module environment declaration.
extern struct apm apm_env;

#endif

/// @}

#endif // _APM_H_
