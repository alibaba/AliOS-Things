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

typedef enum {
    DIGEST_TYPE_SHA1              = 0x00,
    DIGEST_TYPE_SHA224            = 0x01,
    DIGEST_TYPE_SHA256            = 0x02,
    DIGEST_TYPE_SHA384            = 0x03,
    DIGEST_TYPE_SHA512            = 0x04,
    DIGEST_TYPE_SM3               = 0x05,
} digest_t;


/**
 * @brief allocates size bytes and returns a pointer to the allocated memory.
 *
 * @param size the memory size.
 *
 * @return the pointer to the allocated memory if success else return NULL.
 */
void *irot_pal_memory_malloc(int size);


/**
 * @brief free the memory space pointed to by ptr, which must have been returned by a previous call to irot_pal_memory_malloc.
 *
 * @param ptr the memory space pointer.
 */
void irot_pal_memory_free(void *ptr);

/**
 * @brief encode the input data with base64 algorithm.
 *
 * @param in input data.
 * @param in_len input data length.
 * @param out output data buffer.
 * @param out_len input with the output buffer size, ouput the real output data size.
 *
 * @return @see irot_result_t
 */
irot_result_t irot_pal_base64_encode(const uint8_t *in, uint32_t in_len, uint8_t *out, uint32_t *out_len);

/**
 * @brief compute the digest with the hash algorithms.
 *
 * @param in input data.
 * @param in_len input data length.
 * @param out output data buffer.
 * @param out_len input with the output buffer size, ouput the real hash data size.
 * @param type @see digest_t
 *
 * @return @see irot_result_t
 */
irot_result_t irot_pal_hash_sum(const uint8_t *in, uint32_t in_len, uint8_t *out, uint32_t *out_len, digest_t type);


/**
 * @brief generate random number with the given length.
 *
 * @param buf output buffer.
 * @param len the output length to be generated with random bytes.
 *
 * @return @see irot_result_t
 */
irot_result_t irot_pal_get_random(uint8_t *buf, uint32_t len);

/**
 * @brief produce the output according to a format like printf.
 *
 * @param fmt A string like printf to format the output result.
 * @param ...
 */
void irot_pal_log(const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif
