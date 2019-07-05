/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "coap_wrapper.h"

#ifdef HAL_CRYPTO
#include "infra_aes.h"
p_Aes128_t coap_wrapper_aes128_init(
            const uint8_t *key,
            const uint8_t *iv,
            AES_DIR_t dir)
{
    return infra_aes128_init(key, iv, dir);
}

int coap_wrapper_aes128_destroy(p_Aes128_t aes)
{
    return infra_aes128_destroy(aes);
}

int coap_wrapper_aes128_cbc_decrypt(
            p_Aes128_t aes,
            const void *src,
            size_t blockNum,
            void *dst)
{
    return infra_aes128_cbc_decrypt(aes, src, blockNum, dst);
}

int coap_wrapper_aes128_cfb_decrypt(
            p_Aes128_t aes,
            const void *src,
            size_t length,
            void *dst)
{
    return infra_aes128_cfb_decrypt(aes, src, length, dst);
}

int coap_wrapper_aes128_cfb_encrypt(
            p_Aes128_t aes,
            const void *src,
            size_t length,
            void *dst)
{
    return  infra_aes128_cfb_encrypt(aes, src, length, dst);
}

int coap_wrapper_aes128_cbc_encrypt(
            p_Aes128_t aes,
            const void *src,
            size_t blockNum,
            void *dst)
{
    return infra_aes128_cbc_encrypt(aes, src, blockNum, dst);
}
#endif /* HAL_CRYPTO */

#ifdef INFRA_CJSON
#include "infra_cjson.h"
int coap_wrapper_cjson_parse(const char *src, int src_len, lite_cjson_t *lite)
{
    return lite_cjson_parse(src, src_len, lite);
}

int coap_wrapper_cjson_object_item(lite_cjson_t *lite, const char *key, int key_len,
                           lite_cjson_t *lite_item)
{
    return lite_cjson_object_item(lite, key, key_len, lite_item);
}
#endif /* INFRA_CJSON */

#ifdef INFRA_MD5
#include "infra_md5.h"
void coap_wrapper_hmac_md5(const char *msg, int msg_len, char *digest, const char *key, int key_len)
{
    return utils_hmac_md5(msg, msg_len, digest, key, key_len);
}
#endif /* INFRA_MD5 */

#ifdef COAP_WITH_ALI_AUTH
#ifdef INFRA_JSON_PARSER
#include "infra_json_parser.h"
char *coap_wrapper_json_get_value(char *p_cJsonStr, int iStrLen, char *p_cName, int *p_iValueLen, int *p_iValueType)
{
    return json_get_value_by_name(p_cJsonStr, iStrLen, p_cName, p_iValueLen, p_iValueType);
}

char *coap_wrapper_json_value_of(char *key, char *src, int magic, char* module_name)
{
    return LITE_json_value_of(key, src, magic, module_name);
}
#endif /* INFRA_JSON_PARSER */

#if defined(INFRA_LOG) && !defined(INFRA_LOG_ALL_MUTED) 
#include "infra_log.h"
void coap_wrapper_setloglevel(int level)
{
    IOT_SetLogLevel(level);
}
#endif  /* #if defined(INFRA_LOG) && !defined(INFRA_LOG_ALL_MUTED) */

#ifdef INFRA_REPORT
#include "infra_report.h"
void coap_wrapper_set_report_func(info_report_func_pt func)
{
    iotx_set_report_func(func);
}

int coap_wrapper_report_devinfo(void *pclient)
{
    return iotx_report_devinfo(pclient);
}

/* report Firmware version */
int coap_wrapper_report_firmware_version(void *pclient)
{
    return iotx_report_firmware_version(pclient);
}

/* report ModuleID */
int coap_wrapper_report_mid(void *pclient)
{
    return iotx_report_mid(pclient);
}
#endif /* INFRA_REPORT */

#ifdef INFRA_SHA256
#include "infra_sha256.h"
void coap_wrapper_sha256(const uint8_t *input, uint32_t ilen, uint8_t output[32])
{
    return utils_sha256(input, ilen, output);
}
#endif
#endif /* COAP_WITH_ALI_AUTH */
