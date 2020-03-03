/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __COAP_WRAPPER_H__
#define __COAP_WRAPPER_H__

#if defined(__cplusplus)
extern "C" {
#endif

#ifndef bool
#define bool unsigned char
#endif

#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif

void * coap_wrapper_cjson_parse(const char *src);
void * coap_wrapper_cjson_object_item(void *root, const char *key);
char * coap_wrapper_cjson_value_string(void *node);
int coap_wrapper_cjson_value_int(void *node);
void coap_wrapper_cjson_release(void *root);

void coap_wrapper_hmac_md5(const char *msg, int msg_len, char *digest, const char *key, int key_len);

void * coap_wrapper_aes128_init(const unsigned char *key, const unsigned char *iv, bool  is_encrypt);
int coap_wrapper_aes128_destroy(void *aes);
int coap_wrapper_aes128_cbc_decrypt(void *aes, const void *src, unsigned int blockNum,void *dst);
int coap_wrapper_aes128_cbc_encrypt(void *aes, const void *src, unsigned int blockNum, void *dst);

void coap_wrapper_sha256(const unsigned char *input, unsigned int ilen, unsigned char output[32]);

#ifdef INFRA_REPORT
#include "linkkit/infra/infra_report.h"
void coap_wrapper_set_report_func(info_report_func_pt func);
int coap_wrapper_report_devinfo(void *pclient);
int coap_wrapper_report_firmware_version(void *pclient);
int coap_wrapper_report_mid(void *pclient);
#endif /* INFRA_REPORT */

#if defined(__cplusplus)
}
#endif
#endif
