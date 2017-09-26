/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _IOTX_COMMON_HMAC_H_
#define _IOTX_COMMON_HMAC_H_

#include <string.h>

void utils_hmac_md5(const char *msg, int msg_len, char *digest, const char *key, int key_len);

void utils_hmac_sha1(const char *msg, int msg_len, char *digest, const char *key, int key_len);

#endif

