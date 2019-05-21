/**
****************************************************************************************
*
* @file rc.h
*
* Copyright (C) RivieraWaves 2015
*
* @brief Declaration of the initialization functions used in UMAC RC.
*
****************************************************************************************
*/

#ifndef _RC_ENABLE_H_
#define _RC_ENABLE_H_

/**
 *****************************************************************************************
 * @defgroup UMAC UMAC
 * @ingroup MACSW
 * @brief Upper MAC SW module.
 *****************************************************************************************
 */

/**
 *****************************************************************************************
 * @defgroup RC RC
 * @ingroup UMAC
 * @brief Declaration of all functions used for rate control algorithm.
 * @{
 *****************************************************************************************
 */


/**
* INCLUDE FILES
****************************************************************************************
*/
// standard includes
#include "co_int.h"

// for number of rate control steps
#include "hal_desc.h"
// for struct sta_pol_tbl_cntl and struct sta_info_tag
#include "sta_mgmt.h"

/**
 * DEFINES
 ****************************************************************************************
 */
// Set 1 to always use maximum allowed BW
#define RC_USE_MAX_BW         0

/// Maximum number of retries of each step of the retry chain
#define RC_MAX_NUM_RETRY         (2)
/// Rate control algorithm execution period (us)
#define RC_PERIOD_TOUT           (100000)
/// Rate control algorithm trial TX interval (number of TX)
#define RC_TRIAL_PERIOD          (10)
/// Maximum number of samples to be maintained in the statistics structure
#define RC_MAX_N_SAMPLE          (10)

#define RC_SCALE                 (16)
#define EWMA_LEVEL               (96)
#define EWMA_DIV                 (128)

#define RC_FRAC(val, div)        (((val) << RC_SCALE) / (div))
#define RC_TRUNC(val)            ((val) >> RC_SCALE)
#define DIV_ROUND_UP(n, d)       (((n) + (d) - 1) / (d))

#define RC_FIXED_RATE_NOT_SET    (0xFFFF)

#define RC_TRIAL_ENTRY_OFT       (1)
#define RC_TRIAL_ENTRY_MASK      (1 << RC_TRIAL_ENTRY_OFT)
#define RC_AGG_TX_OFT            (2)
#define RC_AGG_TX_MASK           (1 << RC_AGG_TX_OFT)
#define RC_AGG_ALLOWED_OFT       (3)
#define RC_AGG_ALLOWED_MASK      (1 << RC_AGG_ALLOWED_OFT)
#define RC_SW_RETRY_REQ_OFT      (4)
#define RC_SW_RETRY_REQ_MASK     (1 << RC_SW_RETRY_REQ_OFT)
#define RC_FIX_RATE_STATUS_OFT   (5)
#define RC_FIX_RATE_STATUS_MASK  (3 << RC_FIX_RATE_STATUS_OFT)

#define RC_TRIAL_STATUS_WAIT       (0)
#define RC_TRIAL_STATUS_CHECK      (1)
#define RC_TRIAL_STATUS_WAIT_CHECK (2)
#define RC_TRIAL_STATUS_UPD_CHAIN  (3)
#define RC_TRIAL_STATUS_RVT_CHAIN  (4)

#define RC_FIX_RATE_NOTEN_MASK   (0 << RC_FIX_RATE_STATUS_OFT)
#define RC_FIX_RATE_REQ_MASK     (1 << RC_FIX_RATE_STATUS_OFT)
#define RC_FIX_RATE_EN_MASK      (2 << RC_FIX_RATE_STATUS_OFT)

/**
 * STRUCTURES/TYPES DEFINITION
 ****************************************************************************************
 */

/// Statistics table
struct rc_rate_stats
{
    /// Number of attempts (per sampling interval)
    uint32_t attempts;
    /// Number of success (per sampling interval)
    uint32_t success;
    /// Estimated probability of success (EWMA)
    uint16_t probability;
    /// Rate configuration of the sample
    uint16_t rate_config;
    /// Number of times the sample has been skipped (per sampling interval)
    uint8_t  sample_skipped;
    /// Whether the old probability is available
    bool  old_prob_available;
    /// Number of requests of SW retry (per sampling interval)
    uint8_t n_retry;
    // Whether the rate can be used in the retry chain
    bool rate_allowed;
};

/// Step structure
struct step
{
    /// Calculated throughput
    uint32_t tp;
    /// Index of the sample in the rate_stats table
    uint16_t  idx;
};

/// Rate control structure
struct rc_sta_stats
{
    /// Last time the RC has run
    uint32_t last_rc_time;
    /// Statistics table, per sample
    struct rc_rate_stats rate_stats[RC_MAX_N_SAMPLE];
    /// Retry chain steps
    struct step retry[RATE_CONTROL_STEPS];
    /// Structure containing the copy of the information of step 1 of the retry chain,
    /// corresponding to the 2nd higher throughput
    struct step max_tp_2_trial;
    /// Number of MPDUs transmitted (per sampling interval)
    uint32_t ampdu_len;
    /// Number of AMPDUs transmitted (per sampling interval)
    uint32_t ampdu_packets;
    /// Average number of MPDUs in each AMPDU frame (EWMA)
    uint32_t avg_ampdu_len;
    /// Trial transmission period
    uint8_t sample_wait;
    /// Counter used to regulate the use of lower rates for trial transmission
    uint8_t sample_slow;
    // Status of the trial transmission
    uint8_t trial_status;
    /// Information field:
    /// bit   1 : position of the trial entry in the retry chain (0=step 0, 1=step 1)
    /// bit   2 : whether next transmission could be part of a AMPDU
    /// bit   3 : whether aggregation is allowed by the rate control
    /// bit   4 : whether the number of SW retry request has been exceeded, and it's
    ///           necessary to use the next step of the retry chain for the next transmission
    /// bit   5 : whether a fixed rate has to be set
    uint8_t info;
    // Current step 0 of the retry chain
    uint8_t sw_retry_step;
    /// Format and modulation of the station
    uint8_t format_mod;
    /// Allowed HT/VHT rates
    union
    {
        //   Bit 0-31: MCS0-MCS31 bitmask
        //             MCS0-7   : 1SS
        //             MCS8-15  : 2SS
        //             MCS16-23 : 3SS
        //             MCS24-31 : 4SS
        uint8_t     ht[4];
        #if NX_VHT
        //   Bit  0- 1: 1 SS (0=MCS 0-7, 1=MCS 0-8, 2=MCS 0-9, 3=SS not supported)
        //   Bit  2- 3: 2 SS
        //   Bit  4- 5: 3 SS
        //   Bit  6- 7: 4 SS
        //   Bit  8- 9: 5 SS
        //   Bit 10-11: 6 SS
        //   Bit 12-13: 7 SS
        //   Bit 14-15: 8 SS
        uint16_t    vht;
        #endif
    } rate_map;
    /// Allowed legacy rates
    // Bit 0-3 : 1, 2, 5.5, 11 Mbps CCK
    // Bit 4-11: 6-54 Mbps OFDM
    uint16_t rate_map_l;
    /// Maximum MCS supported (0xFF if HT rates not supported)
    uint8_t mcs_max;
    /// Minimum rate index supported
    uint8_t r_idx_min;
    /// Maximum rate index supported
    uint8_t r_idx_max;
    // Maximum BW supported - 0: 20MHz, 1: 40MHz, 2: 80MHz, 3: 160MHz
    uint8_t bw_max;
    /// Number of spatial streams supported minus 1
    uint8_t no_ss;
    /// Short GI supported
    uint8_t short_gi;
    /// Preamble type supported - 0: short and long, 1: only long
    uint8_t p_type;
    /// Number of samples for statistics
    uint16_t no_samples;
    /// Maximum allowed AMSDU size
    uint16_t max_amsdu_len;
    /// Currently allowed AMSDU size
    uint16_t curr_amsdu_len;
    /// Fixed rate configuration - 0xFFFF if disabled
    uint16_t fixed_rate_cfg;
};

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
 * @brief Gets the maximum AMSDU length
 * @param[in] sta_idx index of the station
 * @return Maximum AMSDU lenght. Value 0 means that AMSDU are not allowed.
 ****************************************************************************************
 */
static inline uint16_t rc_get_max_amsdu_len(struct sta_info_tag *sta_entry)
{
    struct sta_pol_tbl_cntl *pt = &sta_entry->pol_tbl;
    struct rc_sta_stats *rc_ss = pt->sta_stats;

    return rc_ss->curr_amsdu_len;
}

/**
 ****************************************************************************************
 * @brief Gets the aggregation enabled flag
 * @param[in] sta_idx index of the station
 * @return Whether the aggregation can be done.
 ****************************************************************************************
 */
static inline bool rc_check_aggregation(struct sta_info_tag *sta_entry)
{
    struct sta_pol_tbl_cntl *pt = &sta_entry->pol_tbl;
    struct rc_sta_stats *rc_ss = pt->sta_stats;

    return ((rc_ss->info & RC_AGG_ALLOWED_MASK) >> RC_AGG_ALLOWED_OFT);
}

void rc_init(struct sta_info_tag *sta_entry);
uint32_t rc_get_duration(uint16_t rate_config);
void rc_update_counters(uint8_t sta_idx, uint32_t attempts, uint32_t failures, bool tx_ampdu, bool retry_required);
uint8_t rc_check(uint8_t sta_idx, bool *tx_ampdu);
void rc_update_bw_nss_max(uint8_t sta_idx, uint8_t bw_max, uint8_t nss_max);
void rc_update_preamble_type(uint8_t sta_idx, uint8_t preamble_type);
void rc_init_bcmc_rate(struct sta_info_tag *sta_entry, uint8_t basic_rate_idx);
uint32_t rc_calc_tp(struct rc_sta_stats *rc_ss, uint8_t sample_idx);
bool rc_check_fixed_rate_config(struct rc_sta_stats *rc_ss, uint16_t fixed_rate_config);

/// @}

#endif /* _RC_ENABLE_H_ */
