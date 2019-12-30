/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

/* JSEngine System Abstract Layer */
#ifndef JSE_SYSTEM_H
#define JSE_SYSTEM_H

#if defined(__cplusplus)
extern "C" {
#endif

#include <fcntl.h>

#include "aos/kernel.h"
#include "aos/kv.h"
#include "aos/vfs.h"
#include "ulog/ulog.h"
#include "netmgr.h"
#include "network.h"
#include "mbedtls/md5.h"
#include "mbedtls/sha1.h"
#include "mbedtls/aes.h"
#include "mbedtls/des.h"
#include "mbedtls/base64.h"

/* memory operation wrapper */
#define jse_malloc aos_malloc
#define jse_calloc aos_calloc
#define jse_realloc aos_realloc
#define jse_free aos_free

/* log system operation wrapper */
enum BE_LOG_LEVEL {
    BE_LOG_LEVEL_DEBUG = (1 << 0),
    BE_LOG_LEVEL_INFO  = (1 << 1),
    BE_LOG_LEVEL_WARN  = (1 << 2),
    BE_LOG_LEVEL_ERROR = (1 << 3),
};

#define jse_debug(fmt, ...) LOGI("JSE",fmt,##__VA_ARGS__)
#define jse_info(fmt, ...) LOGI("JSE",fmt,##__VA_ARGS__)
#define jse_warn(fmt, ...) LOGW("JSE",fmt,##__VA_ARGS__)
#define jse_error(fmt, ...) LOGE("JSE",fmt,##__VA_ARGS__)

/* filesystem operation wrapper */
#define JSE_FS_ROOT_DIR "/spiffs/"
#define jse_dirent_t aos_dirent_t
#define jse_dir_t aos_dir_t
#define jse_open aos_open
#define jse_close aos_close
#define jse_lseek aos_lseek
#define jse_read aos_read
#define jse_write aos_write
#define jse_sync aos_sync
#define jse_stat aos_stat
#define jse_opendir aos_opendir
#define jse_readdir aos_readdir
#define jse_closedir aos_closedir
#define jse_unlink aos_unlink
#define jse_mkdir aos_mkdir
#define jse_rmdir aos_rmdir

#define JSE_MAX_SSID_SIZE MAX_SSID_SIZE
#define JSE_MAX_PWD_SIZE MAX_PWD_SIZE

/* md5 */
#define jse_md5_context mbedtls_md5_context
#define jse_md5 mbedtls_md5
#define jse_md5_init mbedtls_md5_init
#define jse_md5_starts mbedtls_md5_starts
#define jse_md5_update mbedtls_md5_update
#define jse_md5_finish mbedtls_md5_finish
#define jse_md5_free mbedtls_md5_free

/* sha1 */
#define jse_sha1_context mbedtls_sha1_context
#define jse_sha1_init mbedtls_sha1_init
#define jse_sha1_starts mbedtls_sha1_starts
#define jse_sha1_update mbedtls_sha1_update
#define jse_sha1_finish mbedtls_sha1_finish
#define jse_sha1_free mbedtls_sha1_free

/* aes */
#define JSE_AES_ENCRYPT MBEDTLS_AES_ENCRYPT
#define JSE_AES_DECRYPT MBEDTLS_AES_DECRYPT
#define jse_aes_context mbedtls_aes_context
#define jse_aes_init mbedtls_aes_init
#define jse_aes_setkey_enc mbedtls_aes_setkey_enc
#define jse_aes_setkey_dec mbedtls_aes_setkey_dec
#define jse_aes_crypt_cbc mbedtls_aes_crypt_cbc
#define jse_aes_free mbedtls_aes_free

/* des */
#define jse_des_context mbedtls_des_context
#define jse_des_init mbedtls_des_init
#define jse_des_setkey_enc mbedtls_des_setkey_enc
#define jse_des_crypt_ecb mbedtls_des_crypt_ecb
#define jse_des_free mbedtls_des_free

/* base64 */
#define jse_base64_encode mbedtls_base64_encode
#define jse_base64_decode mbedtls_base64_decode

/**
 * @brief       get platform type
 *
 * @return      platform type(string)
 */
const char *jse_system_get_platform_type();

/**
 * @brief       get ip address
 *
 * @param[out]  ip: ip pointer
 *
 * @return      0: success, -1: failed
 */
int32_t jse_system_get_ip(char *ip);

/**
 * @brief       kv componment(key-value) initialize
 *
 * @return      0: success, -1: failed
 */
int32_t jse_system_kv_init();

/**
 * @brief       get value based on key
 *
 * @param[in]   key: key(string)
 * @param[out]  buffer: store value to buffer
 * @param[out]  buffer_len:  value length
 *
 * @return      0: success, -1: failed
 */
int32_t jse_system_kv_get(const char *key, void *buffer, uint32_t *buffer_len);

/**
 * @brief       set key-value
 *
 * @param[in]   key: key(string)
 * @param[in]   value: value
 * @param[in]   sync: 1: sync, 2: not sync
 *
 * @return      0: success, -1: failed
 */
int32_t jse_system_kv_set(const char *key, const void *value, int len,
                          int sync);

/**
 * @brief       connect wifi with ssid & password
 *
 * @param[in]   ssid: wifi ssid
 * @param[in]   password: wifi password
 *
 * @return      0: success, -1: failed
 */
int32_t jse_system_wifi_connect(uint8_t ssid[32], uint8_t password[64]);

/**
 * @brief       get wifi ssid
 *
 * @param[out]   ssid: wifi ssid
 * @param[in]   len: wifi ssid len
 *
 * @return      0: success, -1: failed
 */
int32_t jse_system_wifi_ssid_get(char *ssid, int len);

/**
 * @brief       get wifi mac address
 *
 * @param[out]   mac: wifi mac address
 *
 * @return      0: success, -1: failed
 */
int32_t jse_system_wifi_mac_get(uint8_t mac[6]);

/**
 * @brief       wifi initialize
 *
 * @return      0: success, -1: failed
 */
int32_t jse_system_wifi_init(void);

/**
 * @brief       filesystem initialize
 *
 * @return      0: success, -1: failed
 */
int32_t jse_system_fs_init(void);

/**
 * @brief       system reboot
 *
 * @return      void
 */
void jse_system_reboot(void);




#if defined(__cplusplus)
}
#endif
#endif /* JSE_SYSTEM_H */
