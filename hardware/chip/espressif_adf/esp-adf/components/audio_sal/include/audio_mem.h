/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2018 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef _AUDIO_MEM_H_
#define _AUDIO_MEM_H_

#include <esp_types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Malloc memory in ADF
 *
 * @param[in]  size   memory size
 *
 * @return
 *     - valid pointer on success
 *     - NULL when any errors
 */
void *audio_malloc(size_t size);

/**
 * @brief   Free memory in ADF
 *
 * @param[in]  ptr  memory pointer
 *
 * @return
 *     - void
 */
void audio_free(void *ptr);

/**
 * @brief  Malloc memory in ADF, if spi ram is enabled, it will malloc memory in the spi ram
 *
 * @param[in]  nmemb   number of block
 * @param[in]  size    block memory size
 *
 * @return
 *     - valid pointer on success
 *     - NULL when any errors
 */
void *audio_calloc(size_t nmemb, size_t size);

/**
 * @brief   Malloc memory in ADF, it will malloc to internal memory
 *
 * @param[in] nmemb   number of block
 * @param[in]  size   block memory size
 *
 * @return
 *     - valid pointer on success
 *     - NULL when any errors
 */
void *audio_calloc_inner(size_t nmemb, size_t size);

/**
 * @brief   Print heap memory status
 *
 * @param[in]  tag    tag of log
 * @param[in]  line   line of log
 * @param[in]  func   function name of log
 *
 * @return
 *     - void
 */
void audio_mem_print(const char *tag, int line, const char *func);

/**
 * @brief  Reallocate memory in ADF, if spi ram is enabled, it will allocate memory in the spi ram
 *
 * @param[in]  ptr   memory pointer
 * @param[in]  size  block memory size
 *
 * @return
 *     - valid pointer on success
 *     - NULL when any errors
 */
void *audio_realloc(void *ptr, size_t size);

/**
 * @brief   Duplicate given string.
 *
 *          Allocate new memory, copy contents of given string into it and return the pointer
 *
 * @param[in]  str   String to be duplicated
 *
 * @return
 *     - Pointer to new malloc'ed string
 *     - NULL otherwise
 */
char *audio_strdup(const char *str);

/**
 * @brief   SPI ram is enabled or not
 *
 * @return
 *     - true, spi ram is enabled
 *     - false, spi ram is not enabled
 */
bool audio_mem_spiram_is_enabled(void);

/**
 * @brief   Stack on external SPI ram is enabled or not
 *
 * @return
 *     - true, stack on spi ram is enabled
 *     - false, stack on spi ram is not enabled
 */
bool audio_mem_spiram_stack_is_enabled(void);

#define AUDIO_MEM_SHOW(x)  audio_mem_print(x, __LINE__, __func__)

#ifdef __cplusplus
}
#endif

#endif /*_AUDIO_MEM_H_*/
