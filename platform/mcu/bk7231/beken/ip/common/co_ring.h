/**
 ****************************************************************************************
 *
 * @file co_ring.h
 *
 * @brief definition of co_ring_array structure and related macros.
 *
 * Copyright (C) RivieraWaves 2011-2016
 *
 ****************************************************************************************
 */

#ifndef _CO_RING_H_
#define _CO_RING_H_

/**
 ****************************************************************************************
 * @defgroup CO_RINGARRAY CO_RINGARRAY
 * @ingroup COMMON
 * @brief  Ring buffer management.
 *
 * This module contains the ring buffer structure definition
 * and its member function proto-types.
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "co_int.h"
#include "co_bool.h"
// for NULL
#include <stddef.h>

// for __INLINE
#include "compiler.h"

// for assertions
#include "arch.h"

/*
 * STRUCTURE DECLARATIONS
 ****************************************************************************************
 */
/// Structure holds the information related to the ring.
struct co_ring
{
    /// Used index from the heap.
    uint8_t free_idx;
    /// Free index from the heap.
    uint8_t used_idx;
    /// Number of used elements.
    uint8_t used_count;
    /// Total number of elements in ring.
    uint8_t max_count;
};

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief This function initializes the co-ring array structure.
 *
 * @param[in]   ring    Pointer to ring instance holding the ring parameters.
 * @param[in]   max_count   Number of elements in the ring.
 ****************************************************************************************
 */
__INLINE void co_ring_init(struct co_ring *ring, uint8_t max_count)
{
    // sanity check
    ASSERT_ERR(ring != NULL);
    ASSERT_ERR(max_count != 0);

    // initialize the ring
    ring->free_idx = 0;
    ring->used_idx = 0;
    ring->used_count = 0;
    ring->max_count = max_count;
}

/**
 ****************************************************************************************
 * @brief Write an element onto the ring.
 * @param[in] ring  Pointer to the ring instance.
 ****************************************************************************************
 */
__INLINE void co_ring_write(struct co_ring *ring)
{
    // sanity check
    ASSERT_ERR(ring != NULL);
    ASSERT_ERR(ring->used_count < ring->max_count);

    // increment the index (roll over if necessary)
    ring->free_idx++;
    if (ring->free_idx == ring->max_count)
    {
        ring->free_idx = 0;
    }

    // increment the number of used elements
    ring->used_count++;
}

/**
 ****************************************************************************************
 * @brief Read an element from the ring.
 * @param[in] ring Pointer to the ring instance.
 ****************************************************************************************
 */
__INLINE void co_ring_read(struct co_ring *ring)
{
    // sanity check
    ASSERT_ERR(ring != NULL);
    ASSERT_ERR(ring->used_count > 0);
    ASSERT_ERR(ring->used_count <= ring->max_count);


    // increment the index (roll over if necessary)
    ring->used_idx++;
    if (ring->used_idx == ring->max_count)
    {
        ring->used_idx = 0;
    }

    // decrement the number of used elements
    ring->used_count--;
}

/**
 ****************************************************************************************
 * @brief Check if the ring is empty.
 * @param[in] ring Pointer to the ring instance.
 * @return true is the ring is empty, false otherwise.
 ****************************************************************************************
 */
__INLINE bool co_ring_empty(struct co_ring *ring)
{
    // sanity check
    ASSERT_ERR(ring != NULL);
    ASSERT_ERR(ring->used_count <= ring->max_count);

    return (ring->used_count == 0);
}

/**
 ****************************************************************************************
 * @brief Check if the ring is full.
 * @param[in] ring Pointer to the ring instance.
 * @return true is the ring is full, false otherwise.
 ****************************************************************************************
 */
__INLINE bool co_ring_full(struct co_ring *ring)
{
    // sanity check
    ASSERT_ERR(ring != NULL);
    ASSERT_ERR(ring->used_count <= ring->max_count);

    return (ring->used_count == ring->max_count);
}

/// @} end of group COLIST

#endif // _CO_RING_H_

