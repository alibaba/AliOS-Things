/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * DMA buffer allocator.
 * 
 * This is a very simple dynamic memory allocator for DMA buffers, that
 * can allocate memory in multiple domains based on the board configuration.
 */
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "py/runtime.h"
#include "py/mphal.h"
#include "dma_alloc.h"
#include "omv_boardconfig.h"
#include "common.h"

typedef union block block_t;
union block {
    struct {
        uint32_t used;  // Set to 1/true if the block is used.
        uint32_t size;  // Size of the memory block in bytes.
        void *periph;   // The peripheral that owns this block.
        block_t *next;  // Pointer to the the next block in chain.
    };
    // Make sure that blocks are always aligned to 32 bytes.
    uint8_t alignment[32];
};// Actual block memory starts right after the block header.

// Note block sizes can Not be adjusted after the first allocation.
// When a block is allocated the first time, the block size is fixed to the requested
// memory size, when the block is free'd and reused again the same block size is used.
// Use multiples of the following min size when allocating blocks to avoid fragmentation.
#define MIN_BLOCK_SIZE  (128)

// Get pointer to the block's memory.
#define BLOCK_TO_PTR(block)\
    ((void*)(block + 1))

// Get pointer to the block's header from memory pointer.
#define PTR_TO_BLOCK(ptr)\
    (((block_t *)ptr) - 1)

// Initialize the first block in a block chain.
#define INIT_BLOCK_CHAIN(D)                                     \
({                                                              \
    block_t *block = (block_t *) DMA_MEMORY_##D;                \
    block->used = false;                                        \
    block->size = OMV_DMA_MEMORY_ ##D ##_SIZE - sizeof(block_t);\
    block->next = NULL;                                         \
    block->periph = NULL;                                       \
})

#if defined(OMV_DMA_MEMORY_D1)
static uint8_t OMV_ATTR_SECTION(OMV_ATTR_ALIGNED(DMA_MEMORY_D1[OMV_DMA_MEMORY_D1_SIZE], 16), ".d1_dma_buffer");
#endif
#if defined(OMV_DMA_MEMORY_D2)
static uint8_t OMV_ATTR_SECTION(OMV_ATTR_ALIGNED(DMA_MEMORY_D2[OMV_DMA_MEMORY_D2_SIZE], 16), ".d2_dma_buffer");
#endif
#if defined(OMV_DMA_MEMORY_D3)
static uint8_t OMV_ATTR_SECTION(OMV_ATTR_ALIGNED(DMA_MEMORY_D3[OMV_DMA_MEMORY_D3_SIZE], 16), ".d3_dma_buffer");
#endif

NORETURN static void dma_alloc_fail(uint32_t size)
{
    mp_raise_msg_varg(&mp_type_MemoryError,
            MP_ERROR_TEXT("DMA buffer allocation failed while allocating %u bytes"), (uint) size);
}

void dma_alloc_init0()
{
    // Initialize the first block in each DMA buffer.
    #if defined(OMV_DMA_MEMORY_D1)
    INIT_BLOCK_CHAIN(D1);
    #endif
    #if defined(OMV_DMA_MEMORY_D2)
    INIT_BLOCK_CHAIN(D2);
    #endif
    #if defined(OMV_DMA_MEMORY_D3)
    INIT_BLOCK_CHAIN(D3);
    #endif
}

static int periph_to_domain(void *periph)
{
    // Mask the lower 16 bits to get the bus base address
    uint32_t base = ((uint32_t) periph) & 0xFFFF0000;

    switch (base) {
        #if defined(OMV_DMA_MEMORY_D1)
        case D1_APB1PERIPH_BASE:
        case D1_AHB1PERIPH_BASE:
            return 1;
        #endif
        #if defined(OMV_DMA_MEMORY_D2)
        case D2_APB1PERIPH_BASE:
        case D2_APB2PERIPH_BASE:
        case D2_AHB1PERIPH_BASE:
        case D2_AHB2PERIPH_BASE:
            return 2;
        #endif
        #if defined(OMV_DMA_MEMORY_D3)
        case D3_APB1PERIPH_BASE:
        case D3_AHB1PERIPH_BASE:
            return 3;
        #endif
        default:
            return -1;
    }
}

static void block_chain_from_domain(void *periph, void **start, void **end)
{
    // Find the domain this peripheral belongs to.
    int domain = periph_to_domain(periph);
    
    switch (domain) {
        #if defined(OMV_DMA_MEMORY_D1)
        case 1:
            *start = DMA_MEMORY_D1;
            *end   = (DMA_MEMORY_D1 + OMV_DMA_MEMORY_D1_SIZE);
            break;
        #endif
        #if defined(OMV_DMA_MEMORY_D2)
        case 2:
            *start = DMA_MEMORY_D2;
            *end   = (DMA_MEMORY_D2 + OMV_DMA_MEMORY_D2_SIZE);
            break;
        #endif
        #if defined(OMV_DMA_MEMORY_D3)
        case 3:
            *start = DMA_MEMORY_D3;
            *end   = (DMA_MEMORY_D3 + OMV_DMA_MEMORY_D3_SIZE);
            break;
        #endif
        default:
            mp_raise_msg_varg(&mp_type_MemoryError,
                MP_ERROR_TEXT("DMA buffer allocation failed. No DMA buffer reserved for peripheral 0x%x"), (uint) periph);
            break;
    }
}

void *dma_alloc(uint32_t size, void *periph)
{
    if (size == 0) {
        return NULL;
    }
    
    // Round up the alloc memory size to multiples of the minimum block size.
    size = ((size + MIN_BLOCK_SIZE - 1) / MIN_BLOCK_SIZE) * MIN_BLOCK_SIZE;

    // Find the start and end of the block chain this peripheral belongs to.
    void *block_chain_start, *block_chain_end;
    block_chain_from_domain(periph, &block_chain_start, &block_chain_end);

    block_t *block;
    for (block = block_chain_start; block != NULL; block = block->next) {
        if (!block->used && block->size >= size) {
            // Found a free block that's large enough.
            break;
        }

        if (block->next == NULL) { // last block
            // Allocate a new block if there's enough memory left.
            void *block_end = BLOCK_TO_PTR(block) + block->size;
            uint32_t freemem = block_chain_end - block_end;
            if (freemem >= (size + sizeof(block_t))) {
                block_t *new_block = (block_t *) block_end;
                new_block->next = NULL;  // This is the new last block.
                block->next = new_block; // Link new block to the old one.
                block = new_block;       // Return the new block.
                break;
            }
        }
    }

    if (block == NULL) {
        dma_alloc_fail(size);
    }

    block->used = true;     // Mark block as used.
    block->periph = periph;
    if (block->next == NULL) {
        // Adjust/set the block size only if it's newly allocated or if it's the first and only
        // block in the chain, otherwise if it's a free'd/reused block and the size is adjusted,
        // the block will shrink and it will Not be possible to increase the block size again.
        block->size = size;
    }
    #ifndef NDEBUG
    printf("alloc DMA buffer at 0x%p size: %lu next in chain: 0x%p for periph 0x%p in domain %d\n",
            block, block->size, block->next, block->periph, periph_to_domain(block->periph));
    #endif
    return BLOCK_TO_PTR(block);
}

void dma_alloc_free(void *ptr)
{
    if (ptr != NULL) {
        block_t *block = PTR_TO_BLOCK(ptr);
        #ifndef NDEBUG
        printf("free DMA buffer at 0x%p size: %lu next in chain: 0x%p for periph 0x%p in domain %d\n",
            block, block->size, block->next, block->periph, periph_to_domain(block->periph));
        #endif
        // Just mark the block as unused.
        block->used = false;
        block->periph = NULL;
    }
}
