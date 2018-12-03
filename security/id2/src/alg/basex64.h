/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _BASE64X_H_
#define _BASE64X_H_

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int basex64_encode(const uint8_t* input, uint32_t input_len, uint8_t* output, uint32_t* output_len);

int basex64_decode(const uint8_t* input, uint32_t input_len, uint8_t* output, uint32_t* output_len);

#if defined(__cplusplus)
}
#endif

#endif
