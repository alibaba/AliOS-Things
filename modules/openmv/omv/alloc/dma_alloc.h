/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * DMA buffer allocator.
 */
#ifndef __DMA_ALLOC_H__
#define __DMA_ALLOC_H__
#include <stdint.h>
void dma_alloc_init0();
void *dma_alloc(uint32_t size, void *periph);
void dma_alloc_free(void *ptr);
#endif // __DMA_ALLOC_H__
