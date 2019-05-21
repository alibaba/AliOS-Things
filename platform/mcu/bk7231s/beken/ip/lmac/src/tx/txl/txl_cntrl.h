/**
 ****************************************************************************************
 *
 * @file txl_cntrl.h
 *
 * @brief LMAC TxPath definitions.
 *
 * Copyright (C) RivieraWaves 2011-2016
 *
 ****************************************************************************************
 */

#ifndef _TXL_CNTRL_H_
#define _TXL_CNTRL_H_


/**
 *****************************************************************************************
 * @defgroup TX_CNTRL TX_CNTRL
 * @ingroup TX
 * @brief Controller of TX data path.
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "co_int.h"

// for AC_MAX
#include "mac.h"
// for co_status_t
#include "co_status.h"
// for co_list
#include "co_list.h"
#include "tx_swdesc.h"
#include "ke_event.h"
#include "hal_machw.h"

#include "sta_mgmt.h"

#if NX_AMPDU_TX
#include "tx_swdesc.h"
#endif

/*
 * DEFINES
 ****************************************************************************************
 */
/// AC0 queue timeout
#define TX_AC0_TIMEOUT      5000000  ///< Background - 5s
/// AC1 queue timeout
#define TX_AC1_TIMEOUT      2000000  ///< Best effort - 2s
/// AC2 queue timeout
#define TX_AC2_TIMEOUT      2000000   ///< Video - 400ms
/// AC3 queue timeout
#define TX_AC3_TIMEOUT      200000   ///< Voice - 200ms
/// BCN queue timeout
#define TX_BCN_TIMEOUT      150000   ///< Beacon - 150ms

/// Tx trigger descriptor status check states
enum
{
    /// State for checking MPDU THD done status
    THD_CHK_STATE,
    /// State for checking BAR THD done status
    ATHD_CHK_STATE,
};


#if NX_AMPDU_TX
/// Aggregation process status bits
enum
{
    /// Status indicating that the aggregation formatting is done
    AGG_FORMATTED = 0x01,
    /// Status indicating that enough data of the A-MPDU has been downloaded
    AGG_DOWNLOADED = 0x02,
    /// Status indicating that enough data of the A-MPDU is allocated to be chained
    AGG_ALLOC = 0x04,
    /// Status indicating that the first MPDU of the A-MPDU has been downloaded
    AGG_FIRST_DOWNLOADED = 0x08,
    /// Status indicating that the A-MPDU can be confirmed to upper MAC
    AGG_DONE = 0x10,
    /// Status indicating that a BlockAck is available to be checked
    AGG_BA = 0x20,
    /// Status indicating that this aggregate descriptor is an intermediate one, i.e. not
    /// the final aggregate descriptor of a MU-MIMO PPDU
    AGG_INT = 0x40,
    /// Status indicating that this aggregate descriptor is part of a MU-MIMO PPDU
    AGG_MU = 0x80,
};
#endif

/// Status of a packet pushed for aggregation, when MU-MIMO is enabled
enum
{
    /// Packet will be sent in a SU PPDU
    SU_PACKET,
    #if RW_MUMIMO_TX_EN
    /// Packet will be sent in a MU PPDU
    MU_PACKET,
    /// Packet cannot be handled immediately, user position is closed for now
    MU_PAUSED,
    #endif
};

/*
 * MACROS
 ****************************************************************************************
 */
/// Conversion from Access Category to corresponding TX timer
#define TX_AC2TIMER(ac)             ((ac) + HAL_AC0_TIMER)

/// Index of the beacon queue
#define AC_BCN                        AC_MAX

/// Default TX antenna mask
#define TX_NTX_2_ANTENNA_SET(ntx)   ((CO_BIT((ntx) + 1) - 1) << ANTENNA_SET_PT_OFT)

#if RW_MUMIMO_TX_EN
#define MU_USER_MASK                (CO_BIT(RW_USER_MAX) - 1)
#endif

/*
 * TYPE and STRUCT DEFINITIONS
 ****************************************************************************************
 */
#if NX_AMPDU_TX
/// Structure containing the information about the current A-MPDU being built
struct txl_agg_build_tag
{
    /// Pointer to the allocated aggregation descriptor
    struct tx_agg_desc *desc;
    /// Pointer to the last handled TX descriptor
    struct txdesc *txdesc;
    /// Maximum length of the A-MPDU
    uint32_t max_len[NX_BW_LEN_STEPS];
    /// Minimum number of bytes per A-MPDU sub frame (to fulfill the spacing requirement)
    uint16_t mmss_bytes;
    /// Number of blank delimiters set in current desc to ensure minimal spacing from previous MPDU
    uint16_t nb_delims;
    /// Maximum number of MPDUs inside the A-MPDU
    uint8_t max_cnt;
    /// Current number of MPDUs inside the A-MPDU
    uint8_t curr_cnt;
    /// BW of transmission of the A-MPDU
    uint8_t bw;
    #if NX_BW_LEN_ADAPT
    /// Current step of per-bandwidth length
    uint8_t bw_idx;
    #endif
};
#endif

#if RW_MUMIMO_TX_EN
/// Structure containing the information about the MU-MIMO PPDU being built
struct txl_mumimo_build_info_tag
{
    /// Lists of TX descriptors attached to this MU-MIMO PPDU
    struct co_list tx[RW_USER_MAX];
    /// TX descriptor saved while the corresponding user position queue is paused
    struct txdesc *txdesc[RW_USER_MAX];
    /// Rate information of the A-MPDU under construction
    uint32_t rateinfo[RW_USER_MAX];
    /// Length of the different A-MPDUs of the MU-MIMO PPDU being built
    uint32_t length[RW_USER_MAX];
    /// PHY flags (GI, BW) used for this MU-PPDU
    uint32_t phy_flags;
    /// Number of users present in the MU-MIMO PPDU
    uint8_t nb_users;
    /// Bitfield showing which user positions are already involved in the MU-MIMO PPDU
    uint8_t users;
    /// Bitfield showing which user positions are still accepting new MPDUs
    uint8_t open;
    /// GroupId of this MU-MIMO transmission
    uint8_t group_id;
    /// First user position to be handled when unblocking the queues
    uint8_t first_user_pos;
};
#endif

/// Structure of a per-AC Tx list
struct txl_list
{
    /// Pointer to the last frame exchange chained to HW
    struct tx_hd *last_frame_exch;
    /// List of descriptors being transmitted over WLAN interface
    struct co_list transmitting[RW_USER_MAX];
    /// Pointer to the first descriptor for which the buffer allocation is required
    struct txdesc *first_to_download[RW_USER_MAX];
    /// Counter of bridge DMA transfers done
    uint16_t bridgedmacnt;
    /// State for done status check of THDs at TX trigger interrupt
    uint8_t chk_state;
    #if NX_AMPDU_TX
    ///List of aggregation control structures, directly linked to AMPDU THDs in pool
    struct co_list aggregates;
    /// Pointer to the A-MPDU descriptor for which we are currently waiting for the BA
    struct tx_agg_desc *agg_desc;
    /// Pointer to the previous A-MPDU
    struct tx_agg_desc *agg_desc_prev;
    /// Current aggregates properties
    struct txl_agg_build_tag agg[RW_USER_MAX];
    #if RW_MUMIMO_TX_EN
    struct txl_mumimo_build_info_tag mumimo;
    #endif
    /// Number of PPDUs currently in the TX queue
    uint8_t ppdu_cnt;
    #endif
    #if NX_AMSDU_TX
    /// Pointer to the last buffer descriptor of the queue
    struct tx_pbd *last_pbd[RW_USER_MAX];
    ///Index of the payload to download from the current TX descriptor
    uint8_t dwnld_index[RW_USER_MAX];
    ///Index of the payload for which a TX confirmation is awaited from the current TX descriptor
    uint8_t tx_index[RW_USER_MAX];
    #endif

};

/// Context of the Tx Control block
struct txl_cntrl_env_tag
{
    /// Transmission list per AC
    struct txl_list txlist[NX_TXQ_CNT];

    #if NX_POWERSAVE
    /// Number of packets currently in the TX path
    uint32_t pck_cnt;
    #endif

    /// Non-QoS sequence number
    uint16_t seqnbr;
    #if (NX_TX_FRAME)
    /// Indicate if we are resetting the tx path
    bool reset;
    #endif //(NX_TX_FRAME)
};

// Forward declaration
struct vif_info_tag;

/*
 * GLOBAL VARIABLE DECLARATION
 ****************************************************************************************
 */
/// Table mapping the TX prepare event bit to the queue index
extern const uint32_t txl_prep_evt_bit[NX_TXQ_CNT];

/// Tx Control context variable
extern struct txl_cntrl_env_tag txl_cntrl_env;

/*
 * FUNCTION PROTOTYPES
 ****************************************************************************************
 */

/** @name External API
 @{ */


#if NX_POWERSAVE
__INLINE bool txl_sleep_check(void)
{
    return (txl_cntrl_env.pck_cnt == 0);
}
#endif

/**
 ****************************************************************************************
 * @brief Get the current sequence control field to be used for non-QoS frames.
 * The sequence control is also incremented by this function.
 *
 * @return The sequence control field to be used
 *
 ****************************************************************************************
 */
__INLINE uint16_t txl_get_seq_ctrl(void)
{
    // Increment the sequence number
    txl_cntrl_env.seqnbr++;

    // Build the sequence control
    return (txl_cntrl_env.seqnbr << MAC_SEQCTRL_NUM_OFT);
}



/**
 ****************************************************************************************
 * @brief Initialize the LMAC TX path.
 *
 * This primitive initializes all the elements used in the TX path.
 *
 ****************************************************************************************
 */
void txl_cntrl_init(void);

/**
 ****************************************************************************************
 * @brief TX path reset function.
 * This function is part of the recovery mechanism invoked upon an error detection in the
 * LMAC. It flushes all the packets currently in the TX path and exits when all of them
 * have been confirmed to the driver
 ****************************************************************************************
 */
void txl_reset(void);

#if (NX_TX_FRAME)
/**
 ****************************************************************************************
 * @brief Check if a packet can be transmitted on a given VIF
 ****************************************************************************************
 */
bool txl_cntrl_tx_check(struct vif_info_tag *p_vif_entry);
#endif //(NX_CHNL_CTXT || NX_P2P)

/**
 ****************************************************************************************
 * @brief Push a Tx descriptor in an RA/TID queue
 *
 * This primitive allocates the different descriptors required for the MPDU described by
 * txdesc, updates it with the required information and pushes it into the queue.
 *
 * @param[in] txdesc          Descriptor of the packet to be pushed into the Tx queue
 * @param[in] access_category Access category on which the packet has to be pushed
 *
 * @return true if packet is accepted, false otherwise
 *
 ****************************************************************************************
 */
bool txl_cntrl_push(struct txdesc *txdesc, uint8_t access_category);

#if NX_TX_FRAME
/**
 ****************************************************************************************
 * @brief Push an internal frame Tx descriptor for transmission
 *
 * @param[in] txdesc          Descriptor of the packet to be pushed into the Tx queue
 * @param[in] access_category Access category on which the frame is sent
 *
 ****************************************************************************************
 */
bool txl_cntrl_push_int(struct txdesc *txdesc, uint8_t access_category);
#endif


/**
 ****************************************************************************************
 * @brief Function to be called on protocol or transmission trigger IRQ
 *
 * This primitive will chain new frame exchanges to HW, confirm transmitted ones and
 * prepare subsequent transmission.
 * The function retrieves the access category which generated the interrupt from the
 * MAC HW status register.
 *
 ****************************************************************************************
 */
void txl_transmit_trigger(void);

#if (NX_BW_LEN_ADAPT)
/**
 ****************************************************************************************
 * @brief Function to be called on BW drop IRQ
 *
 * This primitive will split the A-MPDU about to be transmitted, so that its duration is
 * not greater than the allowed one.
 *
 ****************************************************************************************
 */
void txl_transmit_bw_drop(void);
#endif

/**
 ****************************************************************************************
 * @brief Perform operations on payloads that have been transfered from host memory
 *
 * This primitive is called by the interrupt controller ISR. It performs LLC translation
 * and MIC computing if required.
 *
 ****************************************************************************************
 */
void txl_payload_handle(int access_category);

/**
 ****************************************************************************************
 * @brief Halt the MAC DMA of the specified queue
 *
 * @param[in] access_category  Access category of the queue to be halted
 *
 ****************************************************************************************
 */
void txl_cntrl_halt_ac(uint8_t access_category);

/**
 ****************************************************************************************
 * @brief Flush all the descriptors of the queue passed as parameter
 *
 * @param[in] access_category  Access category of the queue to be flushed
 * @param[in] status           Status to be sent in the confirmation
 *
 ****************************************************************************************
 */
void txl_cntrl_flush_ac(uint8_t access_category, uint32_t status);

#if NX_AMPDU_TX
/**
 ****************************************************************************************
 * @brief Check if there is an A-MPDU under construction and if the TX queue is
 * empty. If both conditions are met, the A-MPDU is closed and programmed for TX.
 *
 * @param[in] access_category  Access category of the queue to be checked
 *
 ****************************************************************************************
 */
void txl_aggregate_check(uint8_t access_category);
#endif

/**
 ****************************************************************************************
 * @brief This function returns the pointer to the first TX Header descriptor
 * chained to the MAC HW queue passed as parameter.
 * It is used for error logging when an issue is detected in the LMAC.
 *
 * @param[in] access_category TX queue for which the first THD is retrieved
 * @param[out] thd Pointer to the first tX Header descriptor chained to the MAC HW
 *
 ****************************************************************************************
 */
void txl_current_desc_get(int access_category, struct tx_hd **thd);
uint32_t txl_check_hd_is_current(uint32_t ac, struct txdesc *txd);

/**
 ****************************************************************************************
 * @brief This function increments the TX path packet counter.
 ****************************************************************************************
 */
void txl_cntrl_inc_pck_cnt(void);

/// @}
/// @}

#endif // _TXL_CNTRL_H_
