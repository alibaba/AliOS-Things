/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __ID2_PRIV_H__
#define __ID2_PRIV_H__

#include "ls_osa.h"
#include "id2_config.h"

#define ID2_KEY_NAME          "id2_key"
#define ID2_KEY_NAME_LEN       7

#if defined(CONFIG_ID2_DEBUG)
#define id2_log_debug(_f, ...)		            ls_osa_print("%s %d: " _f,\
                                                            __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define id2_log_debug(_f, ...)
#endif

#define id2_log_info(_f, ...)		            ls_osa_print("%s %d: " _f,\
                                                            __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define id2_log_error(_f, ...)	                    ls_osa_print("%s %d: %s: " _f,\
                                                            __FUNCTION__, __LINE__, "ERROR", ##__VA_ARGS__)

enum
{
    DES_BLOCK_SIZE	= 0x08,
    AES_BLOCK_SIZE	= 0x10,
    RSA_BLOCK_SIZE      = 0x80,
};

int hex_to_char(uint8_t hex);
int char_to_hex(char c);

int hex_to_string(const uint8_t* in, uint32_t in_len, uint8_t* out, uint32_t* out_len);
int string_to_hex(const uint8_t* in, uint32_t in_len, uint8_t* out, uint32_t* out_len);

void id2_log_hex_dump(const char* name, const uint8_t* data, uint32_t len);

#endif  /* __ID2_PRIV_H__ */

