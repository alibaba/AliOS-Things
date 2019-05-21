/**
 ****************************************************************************************
 *
 * @file txl_hwdesc.h
 *
 * File containing the definition of the structure and API function used to initialise the pool.
 *
 * Copyright (C) RivieraWaves 2011-2016
 *
 ****************************************************************************************
 */

#ifndef _TXL_HWDESC_H_
#define _TXL_HWDESC_H_

/**
 ****************************************************************************************
 * @defgroup TX_HWDESC TX_HWDESC
 * @ingroup TX
 * @brief Tx buffer/descriptor management
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "co_int.h"
#include "co_pool.h"
#include "mac.h"
#include "hal_desc.h"
#include <limits.h> // CHAR_BIT - Defines the number of bits in a byte - typically 8, but can be 16 (DSPs)

/*
 * TYPE DECLARATIONS
 ****************************************************************************************
 */

/*
* GLOBALES VARIABLES
****************************************************************************************
*/

/*
* MACRO DEFINITIONS
*****************************************************************************************
*/
/// macro, which is a compile-time assert, used to check if a struct is aligned on a 4-bytes boundary
#define SIZEOFSTRUCT_ASSERT(x)                                                           \
{                                                                                        \
    extern void struct_is_not_4_byte_aligned();                                          \
    if ((sizeof(struct x) % (4 / (CHAR_BIT/8))) != 0)                                    \
        struct_is_not_4_byte_aligned();                                                  \
}

/*
 * GLOBAL VARIABLES DECLARATIONS
 ****************************************************************************************
 */
/// Pool of free aggregation descriptors
extern struct co_list tx_agg_desc_pool[NX_TXQ_CNT];

/*
 * FUNCTION PROTOTYPES
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief Initialize the pools of element
 *
 * Initialize the context and the chained list of each pool
 ****************************************************************************************
 */
void txl_hwdesc_init(void);

/**
 ****************************************************************************************
 * @brief Reset the pools of element
 *
 * The function is called upon a SW recovery procedure occurs
 ****************************************************************************************
 */
void txl_hwdesc_reset(void);


#if NX_AMPDU_TX
/**
 ****************************************************************************************
 * @brief Get a free aggregation descriptor structure from the free pool.
 *
 * @param[in] access_category Access category
 *
 * @return Pointer to the retrieved free structure.
 ****************************************************************************************
 */
__INLINE struct tx_agg_desc * txl_agg_desc_alloc(uint8_t access_category)
{
    return ((struct tx_agg_desc *)co_list_pop_front(&tx_agg_desc_pool[access_category]));
}

/**
 ****************************************************************************************
 * @brief Free a used aggregation descriptor structure.
 *
 * @param[in] agg_desc  Pointer to the structure to be freed.
 * @param[in] access_category Access category
 *
 ****************************************************************************************
 */
__INLINE void txl_agg_desc_free(struct tx_agg_desc *agg_desc, uint8_t access_category)
{
    //chain back into free pool
    co_list_push_back(&tx_agg_desc_pool[access_category], (struct co_list_hdr *)agg_desc);
}
#endif

/// @}

#endif // _TX_HWDESC_H_

