/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _BASE64_H_
#define _BASE64_H_

#if defined(__cplusplus) /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#include <stdlib.h>
#include <string.h>
/**
 * @brief base64 encode
 *
 * @param[in] input: input byte stream
 * @param[in] input_len: input stream length in byte
 * @param[out] output: base64 encoded string
 * @param[in/out] output_len: [in] for output buffer size, [out] for
 *                  base64 encoded string length
 * @Note output buffer is not NULL-terminated
 *
 * @retval  output buffer on success, otherwise NULL will return
 */
unsigned char *base64_encode(const unsigned char *input, int input_len,
                             unsigned char *output, int *output_len);

/**
 * @brief base64 decode
 *
 * @param[in] input: input byte stream
 * @param[in] input_len: input stream length in byte
 * @param[out] output: base64 decoded string
 * @param[in/out] output_len: [in] for output buffer size, [out] for
 *                  base64 decoded string length
 * @Note output buffer is not NULL-terminated
 * @retval  output buffer on success, otherwise NULL will return
 */
unsigned char *base64_decode(const unsigned char *input, int input_len,
                             unsigned char *output, int *output_len);

#if defined(__cplusplus) /* If this is a C++ compiler, use C linkage */
}
#endif

#endif

