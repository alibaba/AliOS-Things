/**
 * \file            lwesp_mem.c
 * \brief           Memory manager
 */

/*
 * Copyright (c) 2022 Tilen MAJERLE
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is part of LwESP - Lightweight ESP-AT parser library.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 * Version:         v1.1.2-dev
 */
#include <limits.h>
#include "lwesp/lwesp_private.h"
#include "lwesp/lwesp_mem.h"

#if !LWESP_CFG_MEM_CUSTOM || __DOXYGEN__

#if !__DOXYGEN__
typedef struct mem_block {
    struct mem_block* next;                     /*!< Pointer to next free block */
    size_t size;                                /*!< Size of block */
} mem_block_t;
#endif /* !__DOXYGEN__ */

/**
 * \brief           Memory alignment bits and absolute number
 */
#define MEM_ALIGN_BITS              LWESP_SZ(LWESP_CFG_MEM_ALIGNMENT - 1)
#define MEM_ALIGN_NUM               LWESP_SZ(LWESP_CFG_MEM_ALIGNMENT)
#define MEM_ALIGN(x)                LWESP_MEM_ALIGN(x)

#define MEMBLOCK_METASIZE           MEM_ALIGN(sizeof(mem_block_t))

#define MEM_ALLOC_BIT               ((size_t)((size_t)1 << (sizeof(size_t) * CHAR_BIT - 1)))
#define MEM_BLOCK_FROM_PTR(ptr)     ((mem_block_t *)(((uint8_t *)(ptr)) - MEMBLOCK_METASIZE))
#define MEM_BLOCK_USER_SIZE(ptr)    ((MEM_BLOCK_FROM_PTR(ptr)->size & ~MEM_ALLOC_BIT) - MEMBLOCK_METASIZE)

static mem_block_t start_block;                 /*!< First block data for allocations */
static mem_block_t* end_block;                  /*!< Pointer to last block in linked list */
static size_t mem_available_bytes;              /*!< Number of available bytes for allocations */

/**
 * \brief           Insert a new block to linked list of free blocks
 * \param[in]       nb: Pointer to new block to insert with known size
 */
static void
mem_insertfreeblock(mem_block_t* nb) {
    mem_block_t* ptr;
    uint8_t* addr;

    /* Find block position to insert new block between */
    for (ptr = &start_block; ptr != NULL && ptr->next < nb; ptr = ptr->next) {}

    /* Check hard error on wrongly used memory */
    if (ptr == NULL) {
        return;
    }

    /*
     * If the new inserted block and block before create a one big block (contiguous)
     * then try to merge them together
     */
    addr = (void*)ptr;
    if ((uint8_t*)(addr + ptr->size) == (uint8_t*)nb) {
        ptr->size += nb->size;                  /* Expand size of block before new inserted */
        nb = ptr;                               /* Set new block pointer to block before (expanded block) */
    }

    /* Check if new block and its size is the same address as next free block newBlock points to */
    addr = (void*)nb;
    if ((uint8_t*)(addr + nb->size) == (uint8_t*)ptr->next) {
        if (ptr->next == end_block) {           /* Does it points to the end? */
            nb->next = end_block;               /* Set end block pointer */
        } else {
            nb->size += ptr->next->size;        /* Expand of current block for size of next free block which is right behind new block */
            nb->next = ptr->next->next;         /* Next free is pointed to the next one of previous next */
        }
    } else {
        nb->next = ptr->next;                   /* Our next element is now from pointer next element */
    }

    /*
     * If merge with new block and block before was not made then there
     * is a gap between free memory before and new free memory.
     *
     * We have to set block before to point to next free which is new block
     */
    if (ptr != nb) {
        ptr->next = nb;
    }
}

/**
 * \brief           Assign memory for HEAP allocations
 * \param[in]       regions: Pointer to list of regions.
 *                  Set regions in ascending order by address
 * \param[in]       len: Number of regions to assign
 */
static uint8_t
mem_assignmem(const lwesp_mem_region_t* regions, size_t len) {
    uint8_t* mem_start_addr;
    size_t mem_size;
    mem_block_t* first_block, *prev_end_block = NULL;

    if (end_block != NULL) {                    /* Regions already defined */
        return 0;
    }

    /* Check if region address are linear and rising */
    mem_start_addr = (uint8_t*)0;
    for (size_t i = 0; i < len; ++i) {
        if (mem_start_addr >= (uint8_t*)regions[i].start_addr) {/* Check if previous greater than current */
            return 0;                           /* Return as invalid and failed */
        }
        mem_start_addr = (uint8_t*)regions[i].start_addr;   /* Save as previous address */
    }

    for (; len > 0; --len, ++regions) {
        /* Check minimum region size */
        mem_size = regions->size;
        if (mem_size < (MEM_ALIGN_NUM + MEMBLOCK_METASIZE)) {
            continue;
        }
        /*
         * Get start address and check memory alignment
         * if necessary, decrease memory region size
         */
        mem_start_addr = (uint8_t*)regions->start_addr; /* Actual heap memory address */
        if (LWESP_SZ(mem_start_addr) & MEM_ALIGN_BITS) {/* Check alignment boundary */
            mem_start_addr += MEM_ALIGN_NUM - (LWESP_SZ(mem_start_addr) & MEM_ALIGN_BITS);
            mem_size -= mem_start_addr - (uint8_t*)regions->start_addr;
        }

        /* Check memory size alignment if match */
        if (mem_size & MEM_ALIGN_BITS) {
            mem_size &= ~MEM_ALIGN_BITS;        /* Clear lower bits of memory size only */
        }

        /*
         * start_block is fixed variable for start list of free blocks
         *
         * Set free blocks linked list on initialized
         *
         * Set Start block only if end block is not yet defined = first run
         */
        if (end_block == NULL) {
            start_block.next = (mem_block_t*)mem_start_addr;
            start_block.size = 0;
        }

        prev_end_block = end_block;             /* Save previous end block to set next block later */

        /*
         * Set pointer to end of free memory - block region memory
         * Calculate new end block in region
         */
        end_block = (mem_block_t*)((uint8_t*)mem_start_addr + mem_size - MEMBLOCK_METASIZE);
        end_block->next = NULL;                 /* No more free blocks after end is reached */
        end_block->size = 0;                    /* Empty block */

        /*
         * Initialize start of region memory
         * Create first block in region
         */
        first_block = (mem_block_t*)mem_start_addr;
        first_block->size = mem_size - MEMBLOCK_METASIZE;   /* Exclude end block in chain */
        first_block->next = end_block;          /* Last block is next free in chain */

        /*
         * If we have previous end block
         * End block of previous region
         *
         * Set previous end block to start of next region
         */
        if (prev_end_block != NULL) {
            prev_end_block->next = first_block;
        }

        /* Set number of free bytes available to allocate in region */
        mem_available_bytes += first_block->size;
    }

    return 1;                                   /* Regions set as expected */
}

/**
 * \brief           Allocate memory of specific size
 * \param[in]       size: Number of bytes to allocate
 * \return          Memory address on success, `NULL` otherwise
 */
static void*
mem_alloc(size_t size) {
    mem_block_t* prev, *curr, *next;
    void* retval = NULL;

    if (end_block == NULL) {                    /* If end block is not yet defined */
        return NULL;                            /* Invalid, not initialized */
    }

    if (size == 0 || size >= MEM_ALLOC_BIT) {
        return NULL;
    }

    size = MEM_ALIGN(size) + MEMBLOCK_METASIZE; /* Increase size for metadata */
    if (size > mem_available_bytes) {           /* Check if we have enough memory available */
        return 0;
    }

    /*
     * Try to find sufficient block for data
     * Go through free blocks until enough memory is found
     * or end block is reached (no next free block)
     */
    prev = &start_block;                        /* Set first first block as previous */
    curr = prev->next;                          /* Set next block as current */
    while ((curr->size < size) && (curr->next != NULL)) {
        prev = curr;
        curr = curr->next;
    }

    /*
     * Possible improvements
     * Try to find smallest available block for desired amount of memory
     *
     * Feature may be very risky later because of fragmentation
     */
    if (curr != end_block) {                    /* We found empty block of enough memory available */
        retval = (void*)((uint8_t*)prev->next + MEMBLOCK_METASIZE); /* Set return value */
        prev->next = curr->next;                /* Since block is now allocated, remove it from free chain */

        /*
         * If found free block is much bigger than required,
         * then split big block by 2 blocks (one used, second available)
         * There should be available memory for at least 2 metadata block size = 8 bytes of useful memory
         */
        if ((curr->size - size) > (2 * MEMBLOCK_METASIZE)) {/* There is more available memory then required = split memory to one free block */
            next = (mem_block_t*)(((uint8_t*)curr) + size); /* Create next memory block which is still free */
            next->size = curr->size - size;     /* Set new block size for remaining of before and used */
            curr->size = size;                  /* Set block size for used block */

            /*
             * Add virtual block to list of free blocks.
             * It is placed directly after currently allocated memory
             */
            mem_insertfreeblock(next);          /* Insert free memory block to list of free memory blocks (linked list chain) */
        }
        curr->size |= MEM_ALLOC_BIT;            /* Set allocated bit = memory is allocated */
        curr->next = NULL;                      /* Clear next free block pointer as there is no one */

        mem_available_bytes -= size;            /* Decrease available memory */
    } else {
        /* Allocation failed, no free blocks of required size */
    }
    return retval;
}

/**
 * \brief           Free memory
 * \param[in]       ptr: Pointer to memory previously returned using \ref lwesp_mem_malloc,
 *                      \ref lwesp_mem_calloc or \ref lwesp_mem_realloc functions
 */
static void
mem_free(void* ptr) {
    mem_block_t* block;

    if (ptr == NULL) {                          /* To be in compliance with C free function */
        return;
    }

    block = MEM_BLOCK_FROM_PTR(ptr);            /* Get block data pointer from input pointer */

    /*
     * Check if block is even allocated by upper bit on size
     * and next free block must be set to NULL in order to work properly
     */
    if ((block->size & MEM_ALLOC_BIT) && block->next == NULL) {
        /*
         * Clear allocated bit before entering back to free list
         * List will automatically take care for fragmentation
         */
        block->size &= ~MEM_ALLOC_BIT;          /* Clear allocated bit */
        mem_available_bytes += block->size;     /* Increase available bytes back */
        mem_insertfreeblock(block);             /* Insert block to list of free blocks */
    }
}

/**
 * \brief           Allocate memory of specific size
 * \param[in]       num: Number of elements to allocate
 * \param[in]       size: Size of element in units of bytes
 * \return          Memory address on success, `NULL` otherwise
 */
static void*
mem_calloc(size_t num, size_t size) {
    void* ptr;
    size_t tot_len = num * size;

    if ((ptr = mem_alloc(tot_len)) != NULL) {   /* Try to allocate memory */
        LWESP_MEMSET(ptr, 0x00, tot_len);       /* Reset entire memory */
    }
    return ptr;
}

/**
 * \brief           Reallocate memory to specific size
 * \note            After new memory is allocated, content of old one is copied to new memory
 * \param[in]       ptr: Pointer to current allocated memory to resize, returned using
 *                      \ref lwesp_mem_malloc, \ref lwesp_mem_calloc or \ref lwesp_mem_realloc functions
 * \param[in]       size: Number of bytes to allocate on new memory
 * \return          Memory address on success, `NULL` otherwise
 */
static void*
mem_realloc(void* ptr, size_t size) {
    void* new_ptr;
    size_t old_size;

    if (ptr == NULL) {                          /* If pointer is not valid */
        return mem_alloc(size);                 /* Only allocate memory */
    }

    old_size = MEM_BLOCK_USER_SIZE(ptr);        /* Get size of old pointer */
    new_ptr = mem_alloc(size);                  /* Try to allocate new memory block */
    if (new_ptr != NULL) {
        LWESP_MEMCPY(new_ptr, ptr, LWESP_MIN(size, old_size));  /* Copy old data to new array */
        mem_free(ptr);                          /* Free old pointer */
    }
    return new_ptr;
}

/**
 * \brief           Allocate memory of specific size
 * \param[in]       size: Number of bytes to allocate
 * \return          Memory address on success, `NULL` otherwise
 * \note            Function is not available when \ref LWESP_CFG_MEM_CUSTOM is `1` and must be implemented by user
 */
void*
lwesp_mem_malloc(size_t size) {
    void* ptr;
    lwesp_core_lock();
    ptr = mem_calloc(1, size);                  /* Allocate memory and return pointer */
    lwesp_core_unlock();
    LWESP_DEBUGW(LWESP_CFG_DBG_MEM | LWESP_DBG_TYPE_TRACE, ptr == NULL,
                 "[LWESP MEM] Allocation failed: %d bytes\r\n", (int)size);
    LWESP_DEBUGW(LWESP_CFG_DBG_MEM | LWESP_DBG_TYPE_TRACE, ptr != NULL,
                 "[LWESP MEM] Allocation OK: %d bytes, addr: %p\r\n", (int)size, ptr);
    return ptr;
}

/**
 * \brief           Reallocate memory to specific size
 * \note            After new memory is allocated, content of old one is copied to new memory
 * \param[in]       ptr: Pointer to current allocated memory to resize, returned using \ref lwesp_mem_malloc,
 *                      \ref lwesp_mem_calloc or \ref lwesp_mem_realloc functions
 * \param[in]       size: Number of bytes to allocate on new memory
 * \return          Memory address on success, `NULL` otherwise
 * \note            Function is not available when \ref LWESP_CFG_MEM_CUSTOM is `1` and must be implemented by user
 */
void*
lwesp_mem_realloc(void* ptr, size_t size) {
    lwesp_core_lock();
    ptr = mem_realloc(ptr, size);               /* Reallocate and return pointer */
    lwesp_core_unlock();
    LWESP_DEBUGW(LWESP_CFG_DBG_MEM | LWESP_DBG_TYPE_TRACE, ptr == NULL,
                 "[LWESP MEM] Reallocation failed: %d bytes\r\n", (int)size);
    LWESP_DEBUGW(LWESP_CFG_DBG_MEM | LWESP_DBG_TYPE_TRACE, ptr != NULL,
                 "[LWESP MEM] Reallocation OK: %d bytes, addr: %p\r\n", (int)size, ptr);
    return ptr;
}

/**
 * \brief           Allocate memory of specific size and set memory to zero
 * \param[in]       num: Number of elements to allocate
 * \param[in]       size: Size of each element
 * \return          Memory address on success, `NULL` otherwise
 * \note            Function is not available when \ref LWESP_CFG_MEM_CUSTOM is `1` and must be implemented by user
 */
void*
lwesp_mem_calloc(size_t num, size_t size) {
    void* ptr;
    lwesp_core_lock();
    ptr = mem_calloc(num, size);                /* Allocate memory and clear it to 0. Then return pointer */
    lwesp_core_unlock();
    LWESP_DEBUGW(LWESP_CFG_DBG_MEM | LWESP_DBG_TYPE_TRACE, ptr == NULL,
                 "[LWESP MEM] Callocation failed: %d bytes\r\n", (int)size * (int)num);
    LWESP_DEBUGW(LWESP_CFG_DBG_MEM | LWESP_DBG_TYPE_TRACE, ptr != NULL,
                 "[LWESP MEM] Callocation OK: %d bytes, addr: %p\r\n", (int)size * (int)num, ptr);
    return ptr;
}

/**
 * \brief           Free memory
 * \param[in]       ptr: Pointer to memory previously returned using \ref lwesp_mem_malloc,
 *                      \ref lwesp_mem_calloc or \ref lwesp_mem_realloc functions
 * \note            Function is not available when \ref LWESP_CFG_MEM_CUSTOM is `1` and must be implemented by user
 */
void
lwesp_mem_free(void* ptr) {
    if (ptr == NULL) {
        return;
    }
    LWESP_DEBUGF(LWESP_CFG_DBG_MEM | LWESP_DBG_TYPE_TRACE,
                 "[LWESP MEM] Free size: %d, address: %p\r\n",
                 (int)MEM_BLOCK_USER_SIZE(ptr), ptr);
    lwesp_core_lock();
    mem_free(ptr);
    lwesp_core_unlock();
}

/**
 * \brief           Assign memory region(s) for allocation functions
 * \note            You can allocate multiple regions by assigning start address and region size in units of bytes
 * \param[in]       regions: Pointer to list of regions to use for allocations
 * \param[in]       len: Number of regions to use
 * \return          `1` on success, `0` otherwise
 * \note            Function is not available when \ref LWESP_CFG_MEM_CUSTOM is `1`
 */
uint8_t
lwesp_mem_assignmemory(const lwesp_mem_region_t* regions, size_t len) {
    uint8_t ret;
    ret = mem_assignmem(regions, len);          /* Assign memory */
    return ret;
}

#endif /* !LWESP_CFG_MEM_CUSTOM || __DOXYGEN__ */

/**
 * \brief           Free memory in safe way by invalidating pointer after freeing
 * \param[in]       ptr: Pointer to pointer to allocated memory to free
 * \return          `1` on success, `0` otherwise
 */
uint8_t
lwesp_mem_free_s(void** ptr) {
    if (ptr != NULL) {
        lwesp_mem_free(*ptr);
        *ptr = NULL;
        return 1;
    }
    return 0;
}
