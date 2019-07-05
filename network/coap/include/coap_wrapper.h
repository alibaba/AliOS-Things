/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __COAP_WRAPPER_H__
#define __COAP_WRAPPER_H__

#if defined(__cplusplus)
extern "C" {
#endif

#ifdef HAL_CRYPTO
#include "infra_aes.h"
p_Aes128_t coap_wrapper_aes128_init(const uint8_t *key, const uint8_t *iv, AES_DIR_t dir);
int coap_wrapper_aes128_destroy(p_Aes128_t aes);
int coap_wrapper_aes128_cbc_decrypt(p_Aes128_t aes, const void *src, size_t blockNum, void *dst);
int coap_wrapper_aes128_cfb_decrypt(p_Aes128_t aes, const void *src, size_t length, void *dst);
int coap_wrapper_aes128_cfb_encrypt(p_Aes128_t aes, const void *src, size_t length, void *dst);
int coap_wrapper_aes128_cbc_encrypt(p_Aes128_t aes, const void *src, size_t blockNum, void *dst);
#endif /* HAL_CRYPTO */

#ifdef INFRA_CJSON
#include "infra_cjson.h"
int coap_wrapper_cjson_parse(const char *src, int src_len, lite_cjson_t *lite);
int coap_wrapper_cjson_object_item(lite_cjson_t *lite, const char *key, int key_len, lite_cjson_t *lite_item);
#endif /* INFRA_CJSON */

#ifdef INFRA_MD5
void coap_wrapper_hmac_md5(const char *msg, int msg_len, char *digest, const char *key, int key_len);
#endif

#ifdef COAP_WITH_ALI_AUTH
#ifdef INFRA_JSON_PARSER
char *coap_wrapper_json_get_value(char *p_cJsonStr, int iStrLen, char *p_cName, int *p_iValueLen, int *p_iValueType);
char *coap_wrapper_json_value_of(char *key, char *src, int magic, char* module_name);
#endif /* INFRA_JSON_PARSER */

#if defined(INFRA_LOG) && !defined(INFRA_LOG_ALL_MUTED) 
void coap_wrapper_setloglevel(int level);
#endif /* defined(INFRA_LOG) && !defined(INFRA_LOG_ALL_MUTED) */

#ifdef INFRA_REPORT
#include "infra_report.h"
void coap_wrapper_set_report_func(info_report_func_pt func);
int coap_wrapper_report_devinfo(void *pclient);
int coap_wrapper_report_firmware_version(void *pclient);
int coap_wrapper_report_mid(void *pclient);
#endif /* INFRA_REPORT */

#ifdef INFRA_SHA256
void coap_wrapper_sha256(const uint8_t *input, uint32_t ilen, uint8_t output[32]);
#endif /* INFRA_SHA256 */
#endif /* COAP_WITH_ALI_AUTH */

#if defined(__cplusplus)
}
#endif
#endif
