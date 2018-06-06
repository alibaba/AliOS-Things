/**
 ****************************************************************************************
 *
 * @file co_pool.h
 *
 * Copyright (C) RivieraWaves 2011-2016
 *
 * @brief File Containing:
 *                         - The declaration of the structures which are involved in the
 *                           pool management operations.
 *                         - The declaration of the APIs which are used to initialize the
 *                           linked list of element within the pools as well as the APIs
 *                           which are used to allocate/release elements from/to the pools.
 *
 ****************************************************************************************
 */

#ifndef _CO_POOL_H_
#define _CO_POOL_H_


/**
 ****************************************************************************************
 * @defgroup COMMON COMMON
 * @ingroup MACSW
 * @brief  Module grouping the general purpose SW tools.
 *
 * This module contains the list management tools, math and endianess libraries, etc.
 * @{
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @defgroup CO_POOL CO_POOL
 * @ingroup COMMON
 * @brief  common module.
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "co_int.h"


/*
 * STRUCTURE DECLARATIONS
 ****************************************************************************************
 */
/** Generic descriptor header of a pool element.
 * Given that these structures are going to be located in cacheable memory area,
 * the purpose of having it is to speed up the scanning through the list of
 * buffers. Indeed since we want to avoid having to flush the cache prior to
 * perform a DMA transfer, the buffers are located in non-cacheable
 * memory area.
 */
struct co_pool_hdr
{
    /// Pointer to the next descriptor in the list.
    struct co_pool_hdr *next;
    /// Pointer to the associated element residing in non-cacheable memory area.
    void *element;
};


/// Structure containing the pool's information: free list pointer and remaining
/// number of elements available.
struct co_pool
{
    /// Pointer to the first element available in the pool.
    struct co_pool_hdr *first_ptr;
    /// Number of elements remaining available in the pool
    uint32_t freecnt;
};

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief Initialize the pool.
 *
 * During initialization this function is in charge of building the linked lists of
 * elements as well as initializing each element.
 *
 * @param[in] pool Pointer to the pool.
 * @param[in] pool_hdr Pointer to the array of pool headers.
 * @param[in] elements Pointer to the elements' array.
 * @param[in] elem_size Size of an element belonging to the pool.
 * @param[in] elem_cnt Number of element belonging to the pool.
 *
 ****************************************************************************************
 */
void co_pool_init(struct co_pool *pool,
                  struct co_pool_hdr *pool_hdr,
                  void *elements,
                  uint32_t elem_size,
                  uint32_t elem_cnt);

/**
 ****************************************************************************************
 * @brief Allocate elements from the pool.
 *
 * @param[in] pool Pointer to the pool from which elements have to be allocated.
 * @param[in] nbelem Number of elements to allocate from the pool.
 *
 * @return Pointer to the list of element(s) allocated from the pool.
 *
 ****************************************************************************************
 */
struct co_pool_hdr *co_pool_alloc(struct co_pool *pool,
                                  uint32_t nbelem);

/**
 ****************************************************************************************
 * @brief Release elements from the pool.
 *
 * @param[in] pool Pointer to the pool from which the elements are released.
 * @param[in] elements Pointer to the list of elements that have to be released from the pool.
 * @param[in] nbelem Number of element to release from the pool.
 *
 ****************************************************************************************
 */
void co_pool_free(struct co_pool *pool,
                  struct co_pool_hdr *elements,
                  uint32_t nbelem);


/// @} end of group CO_POOL
/// @} end of group COMMON


#endif // _CO_POOL_H_

