/**
 * Copyright (c) 2017, Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef _OS_MEM_H_
#define _OS_MEM_H_

#include <stdint.h>
#include <stddef.h>
#include <mem_types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup  OS      OSIF
 * \defgroup    Memory  Memory Management
 *
 * \brief   Allocate, free, and peek memory functions.
 * \details The Memory Management function group allows to allocate, free, and peek heap
 *          memory in the system.\n
 *
 * \ingroup     OS
 */


void *os_mem_alloc_intern(RAM_TYPE ram_type, size_t size,
                          const char *p_func, uint32_t file_line);

void *os_mem_zalloc_intern(RAM_TYPE ram_type, size_t size,
                           const char *p_func, uint32_t file_line);

void *os_mem_aligned_alloc_intern(RAM_TYPE ram_type, size_t size, uint8_t alignment,
                                  const char *p_func, uint32_t file_line);

/**
 * os_mem.h
 *
 * \brief    Allocate a memory block with required size.
 *
 * \param[in]   ram_type            RAM type for allocation.
 * \arg \c      RAM_TYPE_DATA_ON    Data ON RAM type.
 * \arg \c      RAM_TYPE_DATA_OFF   Data OFF RAM type.
 * \arg \c      RAM_TYPE_BUFFER_ON  BUFFER ON RAM type.
 * \arg \c      RAM_TYPE_BUFFER_OFF BUFFER OFF RAM type.
 *
 * \param[in]   size     Required memory size.
 *
 * \return     The address of the allocated memory block. If the address is NULL, the
 *             memory allocation failed.
 *
 * <b>Example usage</b>
 * \code{.c}
 * int test(void)
 * {
 *     size_t mem_size = 0x1000;
 *     void *p_mem = NULL;
 *
 *     p_mem = os_mem_alloc(RAM_TYPE_DATA_ON, mem_size);
 *     if (p_mem != NULL)
 *     {
 *         // Memory allocation successed, and free it.
 *         os_mem_free(p_mem);
 *     }
 *     else
 *     {
 *         // Memory allocation failed.
 *         return -1;
 *     }
 *
 *     return 0;
 * }
 * \endcode
 *
 * \ingroup  Memory
 */
#define os_mem_alloc(ram_type, size)    \
    os_mem_alloc_intern(ram_type, size, __func__, __LINE__)

/**
 * os_mem.h
 *
 * \brief    Allocate and clear a memory block with required size.
 *
 * \param[in]   ram_type            RAM type for allocation.
 * \arg \c      RAM_TYPE_DATA_ON    Data ON RAM type.
 * \arg \c      RAM_TYPE_DATA_OFF   Data OFF RAM type.
 * \arg \c      RAM_TYPE_BUFFER_ON  BUFFER ON RAM type.
 * \arg \c      RAM_TYPE_BUFFER_OFF BUFFER OFF RAM type.
 *
 * \param[in]   size     Required memory size.
 *
 * \return     The address of the allocated memory block. If the address is NULL, the
 *             memory allocation failed.
 *
 * <b>Example usage</b>
 * \code{.c}
 * int test(void)
 * {
 *     size_t mem_size = 0x1000;
 *     void *p_mem = NULL;
 *
 *     p_mem = os_mem_zalloc(RAM_TYPE_DATA_ON, mem_size);
 *     if (p_mem != NULL)
 *     {
 *         // Memory allocation successed, and free it.
 *         os_mem_free(p_mem);
 *     }
 *     else
 *     {
 *         // Memory allocation failed.
 *         return -1;
 *     }
 *
 *     return 0;
 * }
 * \endcode
 *
 * \ingroup  Memory
 */
#define os_mem_zalloc(ram_type, size)   \
    os_mem_zalloc_intern(ram_type, size, __func__, __LINE__)

/**
 * os_mem.h
 *
 * \brief    Allocate an aligned memory block with required size.
 *
 * \param[in]   ram_type            RAM type for allocation.
 * \arg \c      RAM_TYPE_DATA_ON    Data ON RAM type.
 * \arg \c      RAM_TYPE_DATA_OFF   Data OFF RAM type.
 * \arg \c      RAM_TYPE_BUFFER_ON  BUFFER ON RAM type.
 * \arg \c      RAM_TYPE_BUFFER_OFF BUFFER OFF RAM type.
 *
 * \param[in]   size        Required memory size.
 *
 * \param[in]   alignment   memory alignment in 2^N bytes. If alignment is 0, use
 *                          system default memory alignment. The aligned memory block
 *                          must use os_mem_aligned_free() API function to free.
 *
 * \return     The address of the allocated memory block. If the address is NULL, the
 *             memory allocation failed.
 *
 * <b>Example usage</b>
 * \code{.c}
 * int test(void)
 * {
 *     size_t mem_size = 0x1000;
 *     uint8_t mem_alignment = 16;
 *     void *p_mem = NULL;
 *
 *     p_mem = os_mem_aligned_alloc(RAM_TYPE_DATA_ON, mem_size, mem_alignment);
 *     if (p_mem != NULL)
 *     {
 *         // Aligned memory allocation successed, and free it.
 *         os_mem_aligned_free(p_mem);
 *     }
 *     else
 *     {
 *         // Aligned memory allocation failed.
 *         return -1;
 *     }
 *
 *     return 0;
 * }
 * \endcode
 *
 * \ingroup  Memory
 */
#define os_mem_aligned_alloc(ram_type, size, alignment) \
    os_mem_aligned_alloc_intern(ram_type, size, alignment, __func__, __LINE__)

/**
 * os_mem.h
 *
 * \brief    Free a memory block that had been allocated.
 *
 * \param[in]   p_block     The address of memory block being freed.
 *
 * \return     None.
 *
 * <b>Example usage</b>
 * \code{.c}
 * int test(void)
 * {
 *     size_t mem_size = 0x1000;
 *     void *p_mem = NULL;
 *
 *     p_mem = os_mem_alloc(RAM_TYPE_DATA_ON, mem_size);
 *     if (p_mem != NULL)
 *     {
 *         // Memory allocation successed, and free it.
 *         os_mem_free(p_mem);
 *     }
 *     else
 *     {
 *         // Memory allocation failed.
 *         return -1;
 *     }
 *
 *     return 0;
 * }
 * \endcode
 *
 * \ingroup  Memory
 */
void os_mem_free(void *p_block);

/**
 * os_mem.h
 *
 * \brief    Free an aligned memory block that had been allocated.
 *
 * \param[in]   p_block     The address of memory block being freed.
 *
 * \return     None.
 *
 * <b>Example usage</b>
 * \code{.c}
 * int test(void)
 * {
 *     size_t mem_size = 0x1000;
 *     uint8_t mem_alignment = 16;
 *     void *p_mem = NULL;
 *
 *     p_mem = os_mem_aligned_alloc(RAM_TYPE_DATA_ON, mem_size, mem_alignment);
 *     if (p_mem != NULL)
 *     {
 *         // Aligned memory allocation successed, and free it.
 *         os_mem_aligned_free(p_mem);
 *     }
 *     else
 *     {
 *         // Aligned memory allocation failed.
 *         return -1;
 *     }
 *
 *     return 0;
 * }
 * \endcode
 *
 * \ingroup  Memory
 */
void os_mem_aligned_free(void *p_block);

/**
 * os_mem.h
 *
 * \brief    Peek the unused memory size of the specified RAM type.
 *
 * \param[in]   ram_type            RAM type for allocation.
 * \arg \c      RAM_TYPE_DATA_ON    Data ON RAM type.
 * \arg \c      RAM_TYPE_DATA_OFF   Data OFF RAM type.
 * \arg \c      RAM_TYPE_BUFFER_ON  BUFFER ON RAM type.
 * \arg \c      RAM_TYPE_BUFFER_OFF BUFFER OFF RAM type.
 *
 * \return     The unused memory size in btyes.
 *
 * <b>Example usage</b>
 * \code{.c}
 * int test(void)
 * {
 *     size_t unused_data_on;
 *     size_t unused_data_off;
 *
 *     // Peek unused DATA ON memory size.
 *     unused_size = os_mem_peek(RAM_TYPE_DATA_ON);
 *
 *     // Peek unused DATA OFF memory size.
 *     unused_size = os_mem_peek(RAM_TYPE_DATA_OFF);
 *
 *     return 0;
 * }
 * \endcode
 *
 * \ingroup  Memory
 */
size_t os_mem_peek(RAM_TYPE ram_type);

#ifdef __cplusplus
}
#endif

#endif /* _OS_MEM_H_ */
