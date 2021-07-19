/***************************************************************************
 *
 * Copyright 2015-2019 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#ifndef __SPEECH_MEMORY_H__
#define __SPEECH_MEMORY_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef VQE_SIMULATE
#include "med_memory.h"

#define speech_heap_init(a, b)          med_heap_init(a, b)
#define speech_heap_add_block(a, b)     med_heap_add_block(a, b)
#define speech_malloc(a)                med_malloc(a)
#define speech_realloc(a, b)            med_realloc(a, b)
#define speech_calloc(a, b)             med_calloc(a, b)
#define speech_free(a)                  med_free(a)
#define speech_memory_info(a, b, c)     med_memory_info(a, b, c)

#define speech_heap_set_cp(a)           med_heap_set_cp(a)
#else
#include <stddef.h>
#include <stdlib.h>

void speech_heap_init(void *begin_addr, size_t size);
void *speech_malloc(size_t size);
void speech_free(void *p);
void *speech_calloc(size_t nmemb, size_t size);
void *speech_realloc(void *ptr, size_t size);
void speech_memory_info(size_t *total,
                    size_t *used,
                    size_t *max_used);

#define speech_heap_set_cp(a)
#endif

#ifdef __cplusplus
}
#endif

#endif