/**
 ****************************************************************************************
 *
 * @file hal_dma.h
 *
 * Copyright (C) RivieraWaves 2011-2016
 *
 * @brief General purpose DMA definitions
 *
 ****************************************************************************************
 */

#ifndef _HAL_DMA_H_
#define _HAL_DMA_H_

/**
 ****************************************************************************************
 * @defgroup GPDMA GPDMA
 * @ingroup HAL
 * @brief  General purpose DMA management.
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "co_int.h"
#include "co_bool.h"
// for NULL and size_t
#include <stddef.h>

// for target dependent directives
#include "rwnx_config.h"

// for __INLINE
#include "compiler.h"

// for list definitions
#include "co_list.h"

// for DMA definitions
#include "dma.h"

/*
 * DEFINITIONS
 ****************************************************************************************
 */

/// Indicate if a pool of GP DMA Descriptors have to be allocated
#define HAL_DMA_POOL                (RW_BFMER_EN)

#if (HAL_DMA_POOL)
/// Number of descriptors in pool of General Purpose DMA descriptors
#define HAL_DMA_DESC_POOL_SIZE       (10)
#endif //(HAL_DMA_POOL)

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

enum
{
    /// A general purpose DMA download
    DMA_DL,
    /// A general purpose DMA upload
    DMA_UL,
    /// Number of general purpose DMA directions
    DMA_MAX
};

/*
 * STRUCTURE DECLARATIONS
 ****************************************************************************************
 */
/// Pointer to callback function
typedef void (*cb_dma_func_ptr)(void *, int);

/// General purpose DMA descriptor definition
struct hal_dma_desc_tag
{
    /// List header for chaining
    struct co_list_hdr hdr;
    /// Pointer to the HW DMA descriptor in shared RAM
    struct dma_desc *dma_desc;
    /// Callback function pointer
    cb_dma_func_ptr cb;
    /// Pointer to be passed to the callback function
    void *env;
};



/// Environment structure definition
struct hal_dma_env_tag
{
    /// List of programmed download DMA transfers for which a callback needs to be called
    struct co_list prog[DMA_MAX];
    /// List of available GP DMA descriptors
    struct co_list free_gp_dma_descs;
};

/*
 * GLOBAL VARIABLES DECLARATIONS
 ****************************************************************************************
 */
/// HAL DMA environment variable
extern struct hal_dma_env_tag hal_dma_env;

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief Initialize the general purpose DMA module.
 ****************************************************************************************
 */
void hal_dma_init(void);

/**
 ****************************************************************************************
 * @brief Push a DMA request.
 *
 * @param[in] desc Pointer to the DMA descriptor to be used for the transfer
 * @param[in] type Type of transfer (i.e @ref DMA_DL or @ref DMA_UL)
 ****************************************************************************************
 */
void hal_dma_push(struct hal_dma_desc_tag *desc, int type);

/**
 ****************************************************************************************
 * @brief Handler for the general purpose DMA event
 *
 * @param[in] dma_queue DMA queue index (DL or UL)
 ****************************************************************************************
 */
void hal_dma_evt(int dma_queue);

/**
 ****************************************************************************************
 * @brief Interrupt handler for the general purpose DMA download interrupt
 *  ****************************************************************************************
 */
void hal_dma_dl_irq(void);


#if (HAL_DMA_POOL)
/**
 ****************************************************************************************
 * @brief Return a struct hal_dma_desc_tag object extracted from the pool of descriptors.
 * If the descriptor is pushed, it will be automatically inserted back in the pool of
 * descriptors upon confirmation of the DMA transfer.
 * If the descriptor is not pushed and must be released, use hal_dma_release_desc
 * function.
 ****************************************************************************************
 */
struct hal_dma_desc_tag *hal_dma_get_desc(void);

/**
 ****************************************************************************************
 * @brief Add back a GP DMA descriptor to the pool of descriptors.
 *
 * @param[in] p_gp_dma_desc     Descriptor to be released
 ****************************************************************************************
 */
void hal_dma_release_desc(struct hal_dma_desc_tag *p_gp_dma_desc);
#endif //(HAL_DMA_POOL)
/// @} end of group GPDMA

#endif // _HAL_DMA_H_
