/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

#ifndef _AOS_H_
#define _AOS_H_

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>

void *aos_malloc(unsigned int size);

void aos_free(void *mem);

int digest_md5(const void *data, uint32_t length, unsigned char *digest);

int digest_sha256(const void *data, uint32_t length, unsigned char *digest);

long long aos_now_ms(void);

int csp_printf(const char *fmt, ...);

unsigned char *base64_encode(const unsigned char *input, int input_len, unsigned char *output, int *output_len);

unsigned char *base64_decode(const unsigned char *input, int input_len, unsigned char *output, int *output_len);

#if defined(__cplusplus)
}
#endif

#endif
