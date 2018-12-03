/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __IROT_PAL_H__
#define __IROT_PAL_H__

#include <stdint.h>
#include "error_code.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief allocates size bytes and returns a pointer to the allocated memory.
 *
 * @param size the memory size.
 *
 * @return the pointer to the allocated memory if success else return NULL.
 */
void* irot_pal_memory_malloc(int size);


/**
 * @brief free the memory space pointed to by ptr, which must have been returned by a previous call to irot_pal_memory_malloc.
 *
 * @param ptr the memory space pointer.
 */
void irot_pal_memory_free(void* ptr);


/**
 * @brief generate random number with the given length.
 *
 * @param buf output buffer.
 * @param len the output length to be generated with random bytes.
 *
 * @return @see irot_result_t
 */
irot_result_t irot_pal_get_random(uint8_t* buf, uint32_t len);

/**
 * @brief produce the output according to a format like printf.
 *
 * @param fmt A string like printf to format the output result.
 * @param ...
 */
void irot_pal_log(const char* fmt, ...);

#ifdef __cplusplus
}
#endif

#endif
