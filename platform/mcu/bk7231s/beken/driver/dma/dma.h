/**
 ****************************************************************************************
 *
 * @file dma.h
 *
 * @brief DMA utility functions
 *
 * Copyright (C) RivieraWaves 2011-2016
 *
 ****************************************************************************************
 */

#ifndef _IPC_DMA_H_
#define _IPC_DMA_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "co_int.h"

// for target dependent macros
#include "rwnx_config.h"

// for CO_BIT macro
#include "co_math.h"

// for __INLINE
#include "compiler.h"

// for register address
#include "reg_dma.h"


/** @defgroup DMA DMA
 *  @ingroup PLATFORM_DRIVERS
 *  @brief DMA utility functions
 *  @{
 */

/// DMA channel indexes
enum
{
    /// Upload control channel
    IPC_DMA_CHANNEL_CTRL_RX,
    /// Upload data channel
    IPC_DMA_CHANNEL_DATA_RX,
    /// Download control channel
    IPC_DMA_CHANNEL_CTRL_TX,
    /// Download data channel
    IPC_DMA_CHANNEL_DATA_TX,
    /// Number of DMA channels
    IPC_DMA_CHANNEL_MAX,
};

/** DMA LLI assignment.  This mapping is more important than it looks because of the way
 *  the interrupts are mapped on the reference IP interrupt controller:
 *   - LLI0 to 3 are mapped to a single interrupt and therefore the handler of the TX can
 *     be called directly since the interrupt controller is indexed
 *   - LLI4 to 7 are mapped to a single interrupt and therefore the handler of the RX can
 *     be called directly since the interrupt controller is indexed
 */
enum
{
    /// LLI for BK TX queue downloads
    IPC_DMA_LLI_DATA_AC0_TX,
    /// LLI for BE TX queue downloads
    IPC_DMA_LLI_DATA_AC1_TX,
    /// LLI for VI TX queue downloads
    IPC_DMA_LLI_DATA_AC2_TX,
    /// LLI for VO TX queue downloads
    IPC_DMA_LLI_DATA_AC3_TX,
    /// LLI for BCN TX queue downloads
    IPC_DMA_LLI_DATA_BCN_TX,
    /// LLI for RX transfers
    IPC_DMA_LLI_DATA_RX0,
    /// LLI for kernel messages upload
    IPC_DMA_LLI_MSG,
    /// LLI for debug messages upload
    IPC_DMA_LLI_DBG,
    /// LLI for BK TX queue confirmation uploads
    IPC_DMA_LLI_CFM_AC0_TX,
    /// LLI for BE TX queue confirmation uploads
    IPC_DMA_LLI_CFM_AC1_TX,
    /// LLI for VI TX queue confirmation uploads
    IPC_DMA_LLI_CFM_AC2_TX,
    /// LLI for VO TX queue confirmation uploads
    IPC_DMA_LLI_CFM_AC3_TX,
    /// LLI for BCN TX queue confirmation uploads
    IPC_DMA_LLI_CFM_BCN_TX,
    /// LLI for general purpose DMA downloads
    IPC_DMA_LLI_GP_DL,
    /// LLI for debug dump uploads
    IPC_DMA_LLI_DBG_DUMP,
    /// LLI for radar pulses uploads
    IPC_DMA_LLI_RD,
    /// Number of LLI used
    IPC_DMA_LLI_MAX
};

#if NX_BEACONING
/// Mask of the DMA LLI IRQs status bits that are assigned for TX
#define IPC_DMA_LLI_TX_MASK         0x001F
/// Mask of the DMA LLI IRQs status bits that are assigned for CFM
#define IPC_DMA_LLI_CFM_MASK        0x1F00
#else
/// Mask of the DMA LLI IRQs status bits that are assigned for TX
#define IPC_DMA_LLI_TX_MASK         0x000F
/// Mask of the DMA LLI IRQs status bits that are assigned for CFM
#define IPC_DMA_LLI_CFM_MASK        0x0F00
#endif

/// Mask of the DMA LLI IRQs status bits that are assigned for RX
#define IPC_DMA_LLI_RX_MASK         0x0020

/// Indicate that an LLI counter must be incremented at the end of the LLI process
#define IPC_DMA_LLI_COUNTER_EN      0x0010
/// Mask of the LLI counter index to increment at the end of the LLI process
#define IPC_DMA_LLI_COUNTER_MASK    0x000F
/// Bit position of the LLI counter index
#define IPC_DMA_LLI_COUNTER_POS     0

/// Indicate that an IRQ must be generated at the end of the LLI process
#define IPC_DMA_LLI_IRQ_EN          0x1000
/// Mask of the LLI IRQ index to generate at the end of the LLI process
#define IPC_DMA_LLI_IRQ_MASK        0x0F00
/// Bit position of the LLI IRQ index
#define IPC_DMA_LLI_IRQ_POS         8

/// Element in the pool of TX DMA bridge descriptors.
struct dma_desc
{
    /** Application subsystem address which is used as source address for DMA payload
      * transfer*/
    uint32_t            src;
    /** Points to the start of the embedded data buffer associated with this descriptor.
     *  This address acts as the destination address for the DMA payload transfer*/
    uint32_t            dest;
    /// Complete length of the buffer in memory
    uint16_t            length;
    /// Control word for the DMA engine (e.g. for interrupt generation)
    uint16_t            ctrl;
    /// Pointer to the next element of the chained list
    uint32_t            next;
};

/// Structure describing the DMA driver environment
struct dma_env_tag
{
    /** last DMA descriptor pushed for each channel, can point to descriptor already
     * deallocated, but then will not be use`d because root register will be NULL
     */
    volatile struct dma_desc *last_dma[IPC_DMA_CHANNEL_MAX];
};

/// DMA environment structure
extern struct dma_env_tag dma_env;

/**
 ****************************************************************************************
 * @brief Initialize the bridge DMA registers
 ****************************************************************************************
 */
void dma_init(void);


/**
 ****************************************************************************************
 * @brief Chains a descriptor in the DMA
 *
 *
 * @param[in]   desc        DMA descriptor filled by the caller
 * @param[in]   channel_idx Channel index
 *
 ****************************************************************************************
 */
uint32_t dma_push(struct dma_desc *first, struct dma_desc *last, int channel_idx);
#endif // _IPC_DMA_H_

