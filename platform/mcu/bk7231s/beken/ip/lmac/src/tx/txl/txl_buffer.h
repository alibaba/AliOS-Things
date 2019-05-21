/**
 ****************************************************************************************
 *
 * @file txl_buffer.h
 *
 * @brief Management of Tx payload buffers
 *
 * Copyright (C) RivieraWaves 2011-2016
 *
 ****************************************************************************************
 */

#ifndef _TXL_BUFFER_H_
#define _TXL_BUFFER_H_

/**
 ****************************************************************************************
 * @defgroup TX_BUFFER TX_BUFFER
 * @ingroup TX
 * @brief Definition and management of Tx payload buffers
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "co_int.h"
#include "co_bool.h"
#include "rwnx_config.h"
#include "co_list.h"
#include "txl_hwdesc.h"
#include "txl_cntrl.h"
#include "tx_swdesc.h"
#include "co_utils.h"
#include "me_mic.h"


/*
 * CONSTANTS
 ****************************************************************************************
 */
/// MTU of the MAC SW
#define TX_BUFFER_MTU 1536

/// Additional buffering
#define TX_ADD_BUFFER 64

/// Max buffer size
#define TX_BUFFER_MAX (sizeof_b(struct txl_buffer_tag) + TX_BUFFER_MTU + TX_ADD_BUFFER)

/// Number of TX buffer pools
#define TX_BUFFER_POOL_MAX      (AC_MAX * RW_USER_MAX + NX_BEACONING)

/// Size of the memory area reserved for one buffer pool (in number of 32-bit words)
#if NX_AMPDU_TX
#if NX_AMSDU_TX
/// Maximum number of payloads prepared in one call to the TX prepare function
#define TX_MIN_DOWNLOAD_CNT   16
#else
/// Maximum number of payloads prepared in one call to the TX prepare function
#define TX_MIN_DOWNLOAD_CNT   16
#endif
/// Size of the buffer pool (in number of 32-bit words) per queue
#define TX_BUFFER_POOL_SIZE     ((TX_MIN_DOWNLOAD_CNT * TX_BUFFER_MAX) / 4)
#else
/// Maximum number of payloads prepared in one call to the TX prepare function
#define TX_MIN_DOWNLOAD_CNT   16
/// Size of the buffer pool (in number of 32-bit words) per queue
#define TX_BUFFER_POOL_SIZE     ((TX_MIN_DOWNLOAD_CNT * TX_BUFFER_MAX) / 4)
#endif

/// Buffer is allocated
#define TX_BUFFER_NULL          0xFFFFFFFF

/// Mask of the padding bits in the SW control information field
#define TX_BUFFER_PADDING_MASK  0x00000003

/// Offset of the padding bits in the SW control information field
#define TX_BUFFER_PADDING_OFT   0

/// Minimum amount of 32-bit words that form a super buffer
#define TX_BUFFER_MIN_SIZE      256

/// Minimum amount of data that has to be downloaded to chain an AMPDU
#define TX_BUFFER_MIN_AMPDU_DWNLD   500

/// Buffer flags
enum
{
    /// Flag indicating that the buffer is a frontier
    BUF_FRONTIER = CO_BIT(0),
    /// Flag indicating that the buffer is split
    BUF_SPLIT = CO_BIT(1),
    /// Flag indicating that enough data of the MPDU is downloaded
    BUF_ALLOC_OK = CO_BIT(2),
    /// Flag indicating that the downloaded data is not the first MSDU of an A-MSDU
    BUF_INT_MSDU = CO_BIT(3),
};

/*
 * TYPE and STRUCT DEFINITIONS
 ****************************************************************************************
 */
/// Structure containing the HW descriptors needed to handle non-contiguous buffers
struct txl_buffer_hw_desc_tag
{
    /// IPC DMA descriptor
    struct dma_desc dma_desc;
    /// Payload buffer descriptor
    struct tx_pbd   pbd;
};

/// Buffer list structure
struct txl_buffer_list_tag
{
    /// First element of the list
    struct txl_buffer_tag *first;
    /// Last element of the list
    struct txl_buffer_tag *last;
};

/// Tx rate structure
struct txl_buffer_idx_tag
{
    /// Amount of used space in the pool
    uint32_t used_area;
    /// Index of the beginning of the free part of the pool
    uint32_t free;
    /// Size of the free part of the pool
    uint32_t free_size;
    /// Index of the lastly allocated buffer
    uint32_t last;
    /// Amount of data needed for the next allocation (different from 0 only when an
    /// allocation fails)
    uint32_t next_needed;
    /// Current size allocated in the pending super buffer
    uint32_t buf_size;
    /// Pointer to the additional HW descriptor of the pool
    struct txl_buffer_hw_desc_tag *desc;
    /// Number of super buffers currently allocated
    uint8_t count;
};


/// Packet descriptor
struct txl_buffer_control
{
    /// Policy table (filled by the upper MAC)
    struct tx_policy_tbl policy_tbl;
    /// MAC Control Information field (filled by the upper MAC)
    uint32_t mac_control_info;
    /// PHY Control Information field (filled by the upper MAC)
    uint32_t phy_control_info;
    /// Status field
    uint32_t status;
};

/// Context of the Tx buffer management block
struct txl_buffer_env_tag
{
    /// Indexes of the buffer pools
    struct txl_buffer_idx_tag buf_idx[NX_TXQ_CNT][RW_USER_MAX];
    /// List elements
    struct txl_buffer_list_tag list[NX_TXQ_CNT];
};

/// Buffer header structure
struct txl_buffer_tag
{
    /// Total length of the buffer
    uint32_t length;
    /// Bit field containing some information flags about the buffer
    uint32_t flags;
    /// Linked list element allowing to chain buffers together
    struct txl_buffer_tag *next;
    /// Pointer to the TX descriptor linked to this buffer
    struct txdesc *txdesc;
    
    #if (RW_BFMER_EN)
    /// IPC DMA descriptor for the Beamforming Report
    struct dma_desc dma_desc_bfr;
    #endif //(RW_BFMER_EN)
    
    /// TX buffer descriptor
    struct tx_pbd tbd;
    
    #if RW_MUMIMO_TX_EN
    union
    {
        /// Policy table
        struct tx_policy_tbl policy_tbl;
        /// Compressed policy table
        struct tx_compressed_policy_tbl comp_pol_tbl;
    };
    #endif
    
    /// Index of the user in the access category (0 if no MU-MIMO)
    uint16_t user_idx;
	uint16_t padding;
    
    /// Payload area (force 4-byte alignment)
    uint32_t payload[];
};



/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */   
extern struct txl_buffer_hw_desc_tag txl_buffer_hw_desc[TX_BUFFER_POOL_MAX];
extern struct txl_buffer_env_tag txl_buffer_env;
extern struct txl_buffer_control txl_buffer_control_desc[NX_REMOTE_STA_MAX][2];
extern struct txl_buffer_control txl_buffer_control_desc_bcmc[NX_VIRT_DEV_MAX];

/*
 * FUNCTION PROTOTYPES
 ****************************************************************************************
 */
extern bool txl_buffer_free(struct txl_buffer_tag *, uint8_t);
extern void txl_buffer_push(uint8_t ac, struct txl_buffer_tag *buf);
#if NX_AMSDU_TX
void txl_buffer_update_tbd(struct txdesc *txdesc,
                                    uint8_t access_category,
                                    uint8_t pkt_idx);
struct txl_buffer_tag *txl_buffer_alloc(struct txdesc *txdesc, 
										uint8_t access_category,
										uint8_t user_idx, 
										uint8_t pkt_idx);
#else
struct txl_buffer_tag *txl_buffer_alloc(struct txdesc *txdesc, 
                                            uint8_t access_category,
                                            uint8_t user_idx);
#endif
extern void txl_buffer_reset(uint8_t access_category);
extern void txl_buffer_init(void);
extern uint8_t txl_buffer_count(uint8_t access_category, uint8_t user_idx);
extern struct txl_buffer_control *txl_buffer_control_get(struct txdesc *txdesc);
extern void txl_buffer_update_thd(struct txdesc *, uint8_t);
extern struct txl_buffer_tag *txl_buffer_get(struct txdesc *txdesc);
extern struct txl_buffer_tag *txl_buffer_pop(uint8_t access_category);
extern void txl_buffer_reinit(void);
extern void txl_buffer_mic_compute(struct txdesc *txdesc,
                                     uint32_t *mic_key,
                                     uint32_t start,
                                     uint32_t len,
                                     uint8_t access_category);
#endif /// TX_BUFFER_H_
// eof

