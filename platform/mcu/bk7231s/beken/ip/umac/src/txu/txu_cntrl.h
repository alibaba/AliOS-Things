/**
 ****************************************************************************************
 *
 * @file txu_cntrl.h
 *
 * @brief UMAC TX Path definitions.
 *
 * Copyright (C) RivieraWaves 2011-2016
 *
 ****************************************************************************************
 */

#ifndef _TXU_CNTRL_H_
#define _TXU_CNTRL_H_

#include "co_int.h"
#include "co_bool.h"
#include "rwnx_config.h"
#include "tx_swdesc.h"

/** @defgroup UMACTX TXU
 * @ingroup UMAC
 * @brief UMAC TX Path.
 * @{
 */

/*
 * DEFINES
 ****************************************************************************************
 */

/// status bit verified at umac
#define TX_STATUS_RETRY_REACHED     0x0800

/// Frame transmission done
#define TX_STATUS_DONE                 CO_BIT(0)
/// Frame retry required
#define TX_STATUS_RETRY_REQUIRED       CO_BIT(1)
/// Frame sw retry required
#define TX_STATUS_SW_RETRY_REQUIRED    CO_BIT(2)

#define TXU_CNTRL_RETRY                CO_BIT(0)
#define TXU_CNTRL_UNDER_BA             CO_BIT(1)
#define TXU_CNTRL_MORE_DATA            CO_BIT(2)
/**
 * TX Frame is a management frame:
 *      - WLAN header is already provided, no need to transform an ethernet header
 *      - Frame shall be sent as a singleton
 */
#define TXU_CNTRL_MGMT                 CO_BIT(3)
/**
 * No CCK rate can be used (valid only if TXU_CNTRL_MGMT is set)
 */
#define TXU_CNTRL_MGMT_NO_CCK          CO_BIT(4)
/**
 * Internal flags indicating that the PM monitoring has been started for this frame
 */
#define TXU_CNTRL_MGMT_PM_MON          CO_BIT(5)
#define TXU_CNTRL_AMSDU                CO_BIT(6)
#define TXU_CNTRL_MGMT_ROBUST          CO_BIT(7)
#define TXU_CNTRL_USE_4ADDR            CO_BIT(8)
#define TXU_CNTRL_EOSP                 CO_BIT(9)
#define TXU_CNTRL_MESH_FWD             CO_BIT(10)
#define TXU_CNTRL_TDLS                 CO_BIT(11)

/*
 * ENUMERATION
 ****************************************************************************************
 */

// Information Field Bit Field
enum txu_cntrl_info_bit
{
    TXU_CNTRL_IV_PRESENT    = 0,
    TXU_CNTRL_EIV_PRESENT,
    TXU_CNTRL_LLCSNAP_ADD,
};

/*
 * FUNCTION PROTOTYPES
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize the TX path and its components.
 *
 ****************************************************************************************
 */
void txu_cntrl_init(void);

/**
 ****************************************************************************************
 * @brief This is the entry point in UMAC TX path for sending host and send back
 * data frames.
 *
 * @param[in]  txdesc           Pointer to the structure that has the info of the data
 *                              packet to be transmitted.
 * @param[in]  access_category  Index of the queue in which the packet is passed
 ****************************************************************************************
 */
bool txu_cntrl_push(struct txdesc *txdesc, uint8_t access_category);

/**
 ****************************************************************************************
 * @brief Function called by the LMAC once the data buffer has been allocated. It is
 * responsible for the different header formatting (MAC Header, IV/EIV, LLC/SNAP).
 *
 * @param[in]  txdesc           Pointer to the structure that has the info of the data
 *                              packet to be transmitted.
 * @param[in]  buf              Address of the header in shared RAM
 ****************************************************************************************
 */
void txu_cntrl_frame_build(struct txdesc *txdesc, uint32_t buf);

/**
 ****************************************************************************************
 * @brief Function called by the LMAC once the data buffer has been downloaded. It is
 * responsible for the TKIP MIC computing if required.
 *
 * @param[in]  txdesc           Pointer to the structure that has the info of the data
 *                              packet to be transmitted.
 * @param[in]  ac               Access category on which the packet is sent
 ****************************************************************************************
 */
void txu_cntrl_tkip_mic_append(struct txdesc *txdesc, uint8_t ac);

/**
****************************************************************************************
* @brief This function handles status of a transmission. It returns the number of credits
* allocated to the host for the next transmissions.
*
* @param[in] txdesc  Pointer to the packet descriptor
*
****************************************************************************************
*/
void txu_cntrl_cfm(struct txdesc *txdesc);

/**
*******************************************************************************
* @brief This function Add security header to a management frame. If security
* lengths (head and tail) are not set in txdesc->umac, it will first compute them
* and update txdesc. Then it add security header. No check on frame type/subtype
* is done and it assumes that it has already been checked before.
*
* @param[in/out] txdesc    Pointer to the packet descriptor
* @param[in] frame     Pointer on frame (points on MAC header)
* @param[in] hdr_len   Size of MAC header
*******************************************************************************
*/
void txu_cntrl_protect_mgmt_frame(struct txdesc *txdesc, uint32_t frame,
                                  uint16_t hdr_len);
/// @}

#endif // _TXU_CNTRL_H_
