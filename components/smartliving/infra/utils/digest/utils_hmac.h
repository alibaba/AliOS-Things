/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */




#ifndef _IOTX_COMMON_HMAC_H_
#define _IOTX_COMMON_HMAC_H_

#include <string.h>

#define KEY_IOPAD_SIZE        (64)
#define MD5_DIGEST_SIZE       (16)
#define SHA1_DIGEST_SIZE      (20)
#define SHA256_DIGEST_SIZE    (32)

void utils_hmac_md5(const char *msg, int msg_len, char *digest, const char *key, int key_len);

void utils_hmac_sha1(const char *msg, int msg_len, char *digest, const char *key, int key_len);

void utils_hmac_sha1_hex(const char *msg, int msg_len, char *digest, const char *key, int key_len);

void utils_hmac_sha1_raw(const char *msg, int msg_len, char *digest, const char *key, int key_len);

void utils_hmac_sha1_base64(const char *msg, int msg_len, const char *key, int key_len, char *digest, int *digest_len);


void utils_hmac_sha256(const char *msg, int msg_len, char *digest, const char *key, int key_len);

#endif

