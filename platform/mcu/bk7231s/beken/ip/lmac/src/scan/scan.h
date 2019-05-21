/**
 ****************************************************************************************
 * @file scan.h
 *
 * @brief Scanning module definitions.
 *
 * Copyright (C) RivieraWaves 2011-2016
 *
 ****************************************************************************************
 */

#ifndef _SCAN_H_
#define _SCAN_H_

/**
 *****************************************************************************************
 * @defgroup SCAN SCAN
 * @ingroup LMAC
 * @brief LMAC Scanning module.
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
#include "hal_dma.h"

#include "dma.h"

/*
 * DEFINES
 ****************************************************************************************
 */
/// Maximum number of SSIDs in a scan request
#define SCAN_SSID_MAX   2

/// Maximum number of 2.4GHz channels
#define SCAN_CHANNEL_2G4 14

/// Maximum number of 5GHz channels
#define SCAN_CHANNEL_5G  28

/// Maximum number of channels in a scan request
#define SCAN_CHANNEL_MAX (SCAN_CHANNEL_2G4 + SCAN_CHANNEL_5G)

/// Maximum length of the ProbeReq IEs
#define SCAN_MAX_IE_LEN 200

/// Bit indicating that the scanning should be passive on the specified channel
#define SCAN_PASSIVE_BIT CO_BIT(0)
/// Bit indicating the scanning is disabled on the specified channel
#define SCAN_DISABLED_BIT CO_BIT(31)

/// Duration on channel (in TUs) when actively scanning
#define SCAN_ACTIVE_DURATION          90000   // 30 ms

/// Duration on channel (in TUs) when passively scanning
#define SCAN_PASSIVE_DURATION         110000   // 120 ms

/// Definition of a channel to be scanned
struct scan_chan_tag
{
    /// Frequency of the channel
    uint16_t freq;
    /// RF band (0: 2.4GHz, 1: 5GHz)
    uint8_t band;
    /// Bit field containing additional information about the channel
    uint8_t flags;
    /// Max tx_power for this channel (dBm)
    int8_t tx_power;
};

/// Definition of a ProbeReq IE buffer
struct scan_probe_req_ie_tag
{
    /// DMA descriptor to download the IEs from host memory
    struct dma_desc dma_desc;
    /// TX payload buffer descriptor
    struct tx_pbd pbd;
    /// Buffer space for IEs
    uint32_t buf[SCAN_MAX_IE_LEN/4];
};

/// SCAN Context
struct scan_env_tag
{
    /// GP DMA descriptors
    struct hal_dma_desc_tag dma_desc;
    /// Pointer to the scanning parameters
    struct scan_start_req const *param;
    /// Address of the DS information element
    uint32_t ds_ie;
    /// Task identifier of the scanning requester
    ke_task_id_t req_id;
    /// Current channel index
    uint8_t chan_idx;
    /// Abort scan requested by host ?
    bool abort;
};

#if NX_HW_SCAN
/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */
extern struct scan_env_tag scan_env;

extern struct scan_probe_req_ie_tag scan_probe_req_ie;


/*
 * FUNCTION PROTOTYPES
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief SCAN Module main initialization function.
 * This function is called after reset and initializes all SCAN related env and data.
 ****************************************************************************************
 */
void scan_init(void);

/**
 ****************************************************************************************
 * @brief Download the information elements that will need to be added to the ProbeReq.
 *
 * @param[in] Pointer to the scan parameters
 ****************************************************************************************
 */
void scan_ie_download(struct scan_start_req const *param);


/**
 ****************************************************************************************
 * @brief Send the appropriate channel request message.
 ****************************************************************************************
 */
void scan_set_channel_request(void);


/**
 ****************************************************************************************
 * @brief Prepare and program for TX the ProbeReq required for active scanning
 ****************************************************************************************
 */
void scan_probe_req_tx(void);

/**
 ****************************************************************************************
 * @brief Send the SCAN_CANCEL_CFM message to the SCAN_CANCEL_REQ task source
 ****************************************************************************************
 */
void scan_send_cancel_cfm(uint8_t status, ke_task_id_t dest_id);
#endif

#if CFG_TX_EVM_TEST
uint32_t evm_req_tx(struct mac_addr const *mac_addr);

#endif // CFG_TX_EVM_TEST

/**
 ****************************************************************************************
 * @brief Return scan channel information currently used
 ****************************************************************************************
 */
struct scan_chan_tag const *scan_get_chan(void);
/// @} end of group

#endif // _SCAN_H_
