/**
 * \file            lwesp_mem.h
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
#ifndef LWESP_HDR_MEM_H
#define LWESP_HDR_MEM_H

#include "lwesp/lwesp.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \ingroup         LWESP
 * \defgroup        LWESP_MEM Memory manager
 * \brief           Dynamic memory manager
 * \{
 */

#if !LWESP_CFG_MEM_CUSTOM || __DOXYGEN__

/**
 * \brief           Single memory region descriptor
 */
typedef struct {
    void* start_addr;                           /*!< Start address of region */
    size_t size;                                /*!< Size in units of bytes of region */
} lwesp_mem_region_t;

uint8_t lwesp_mem_assignmemory(const lwesp_mem_region_t* regions, size_t size);

#endif /* !LWESP_CFG_MEM_CUSTOM || __DOXYGEN__ */

void*   lwesp_mem_malloc(size_t size);
void*   lwesp_mem_realloc(void* ptr, size_t size);
void*   lwesp_mem_calloc(size_t num, size_t size);
void    lwesp_mem_free(void* ptr);
uint8_t lwesp_mem_free_s(void** ptr);

/**
 * \}
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LWESP_HDR_MEM_H */
