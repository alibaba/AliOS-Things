/**
 * @file k_mm_debug.h
 *
 * @copyright Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef K_MM_DEBUG_H
#define K_MM_DEBUG_H

/** @addtogroup aos_rhino mm
 *  Memory debug includes buffer over flow check, memory usage statistics, etc.
 *
 *  @{
 */

#if (RHINO_CONFIG_MM_DEBUG > 0)

#define AOS_UNSIGNED_INT_MSB    (1u << (sizeof(unsigned int) * 8 - 1))

/**
 * Add owner info to the memory buffer.
 *
 * @param[in]  addr  pointer to the buffer
 * @param[in]  allocator   address of the allocator
 *
 * @return  NULL
 */
void krhino_owner_attach(void *addr, size_t allocator);

/**
 * Set owner to return address of function.
 *
 */
#define krhino_owner_return_addr(addr) \
    krhino_owner_attach(addr, (size_t)RHINO_GET_RA())

/**
 * Show heap information.
 *
 * @param[in]  len  useless anymore
 *
 * @return  RHINO_SUCCESS
 */
uint32_t dumpsys_mm_info_func(uint32_t len);

#endif /* RHINO_CONFIG_MM_DEBUG */

/** @} */

#endif /* K_MM_DEBUG_H */

