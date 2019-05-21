/**
 ****************************************************************************************
 *
 * @file txl_cfm.h
 *
 * @brief Tx confirmation module.
 *
 * Copyright (C) RivieraWaves 2011-2016
 *
 ****************************************************************************************
 */

#ifndef _TXL_CFM_H_
#define _TXL_CFM_H_

/**
 ****************************************************************************************
 * @defgroup TX_CFM TX_CFM
 * @ingroup TX
 * @brief LMAC Tx confirmation module.
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "co_int.h"
#include "co_list.h"
#include "tx_swdesc.h"

/*
 * TYPE and STRUCT DEFINITIONS
 ****************************************************************************************
 */
#if NX_AMPDU_TX
/// TID offset in BA payload byte
#define BA_PAYL_TID_BIT_OFT     12
/// TID mask in BA payload byte
#define BA_PAYL_TID_BIT_MSK     (0xF << BA_PAYL_TID_BIT_OFT)
/// SSN bit offset within SSC
#define SN_IN_SSC_BIT_OFT       4

///TID_INFO byte offset in BA payload
#define BA_PAYL_TID_BYTE_OFT   (1 + MAC_LONG_CTRLFRAME_LEN)
///Start Sequence control field byte offset in BA payload
#define BA_PAYL_SSC_BYTE_OFT   (2 + MAC_LONG_CTRLFRAME_LEN)
///Bitmap byte offset in BA payload
#define BA_PAYL_BITMAP_BYTE_OFT (4 + MAC_LONG_CTRLFRAME_LEN)
#endif

/// Maximum number of confirmation uploads that can be pending simultaneously
/// This value shall be a power of 2
#define TXL_CFM_PENDING_MAX   16
/// Mask of the confirmation indexes
#define TXL_CFM_IDX_MSK (TXL_CFM_PENDING_MAX - 1)

/// Table mapping the TX confirmation event bit to the queue index
extern const uint32_t txl_cfm_evt_bit[NX_TXQ_CNT];

/// Context of the Tx Control block
struct txl_cfm_env_tag
{
    /// Tx confirmation list
    struct co_list cfmlist[NX_TXQ_CNT];
    /// Array of pending confirmations
    volatile uint32_t user_cfm[TXL_CFM_PENDING_MAX];
    /// In index for the pending confirmation array
    uint8_t in_idx;
    /// Target LLI count to be reached to indicate the confirmation to host
    uint16_t lli_cnt;
};

/*
 * GLOBAL VARIABLE DECLARATION
 ****************************************************************************************
 */
/// Tx Control context variable
extern struct txl_cfm_env_tag txl_cfm_env;


/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initializes the CFM and BA queues.
 * These queues are useful for sending confirmation to UMAC for the txdescs in order.
 *
 * The CFM queue will handle the linked list of txdescs, each having a certain status
 * after handling the TX trigger from HW (acked, waiting for BA analysis, not acked at all)
 *
 * The BA queue holds the linked list of rxdescs of BA frames per AC, which will be used and
 * freed when the TX_CFM kernel event is handled in order to update the MPDU status
 * of those MPDUs part of AMPDUs in the confirm list.
 *
 * For singleton MPDUs the BA queue is of no use, they should be confirmed according to
 * their already set ok/not acked status.
 *
 ****************************************************************************************
 */
void txl_cfm_init(void);

/**
 ****************************************************************************************
 * @brief Push a Tx descriptor in the CFM queue
 *
 * @param[in] txdesc            Tx descriptor to be pushed in the CFM queue
 * @param[in] status            Status of the confirmation
 * @param[in] access_category   Access category on which the confirmation is pushed
 *
 ****************************************************************************************
 */
void txl_cfm_push(struct txdesc *txdesc, uint32_t status, uint8_t access_category);

/**
 ****************************************************************************************
 * @brief Background event handler of the Tx confirmation module
 *
 * TX descriptors are linked in the cfmlist, ACs mixed but in order of arrival per AC.
 * When the event is programmed, a txdesc in the cfmlist front is analyzed.
 *
 * It can be the 1st MPDU in an AMPDU whose BA reception status is known, because all the
 * MPDUs in an AMPDU are not moved from transmitting list (per AC) to cfmlist until
 * their BA reception status is known (updated by Hw in either the AMPDU THD or BAR THD.
 *
 * When it is such an MPDU, the first BA received in the ba_list queue is retrieved,
 * and the MPDUs in the AMPDU are taken one by one from the cfm list, their sattus is updated
 * and they are sent to host.
 *
 * The BA reception status is present in either AMPDU THD or BAR THD, but it doesn't matter
 * how it was received, but IF it was received, the bitmap is extracted for the AMPDU
 * MPDUs in any case.
 *
 * When the MPDU in the cfm list is a singleton MPDU, it already has its status so it is
 * released towards the host immediately without further handling.
 *
 * @param[in] dummy Parameter not used but required to follow the kernel event callback
 * format
 *
 ****************************************************************************************
 */
void txl_cfm_evt(int dummy);

#if NX_AMPDU_TX
/**
 ****************************************************************************************
 * @brief Push a Rx descriptor containing a BA frame in the appropriate aggregate
 * descriptor on the right AC.
 *
 * @param[in] rxdesc     Pointer to Rx descriptor to be pushed in the BA queue
 *
 ****************************************************************************************
 */
void txl_ba_push(struct rx_swdesc *rxdesc);
#endif

/**
 ****************************************************************************************
 * @brief Immediately confirm the descriptor passed as parameter
 *
 * @param[in] access_category  Access category corresponding to the list
 * @param[in] txdesc           Pointer to the descriptor of descriptors to be confirmed
 * @param[in] status           Status to be sent in the confirmation
 *
 ****************************************************************************************
 */
void txl_cfm_flush_desc(uint8_t access_category, struct txdesc *txdesc, uint32_t status);

/**
 ****************************************************************************************
 * @brief Immediately confirm all the descriptors of the list passed as parameter
 *
 * @param[in] access_category  Access category corresponding to the list
 * @param[in] list             Pointer to the list of descriptors to be confirmed
 * @param[in] status           Status to be sent in the confirmation
 *
 ****************************************************************************************
 */
void txl_cfm_flush(uint8_t access_category, struct co_list *list, uint32_t status);

/**
 ****************************************************************************************
 * @brief Handler for DMA transfer of confirmations
 *
 ****************************************************************************************
 */
void txl_cfm_dma_int_handler(void);
/// @}

#endif // _TX_CFM_H_
