/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef AMP_DEFINES_H
#define AMP_DEFINES_H

#include "amp_config.h"
#include "mbedtls/md5.h"
#include "mbedtls/sha1.h"
#include "mbedtls/aes.h"
#include "mbedtls/des.h"
#include "mbedtls/base64.h"
#include "ulog/ulog.h"
#include "aos_fs.h"

#define AMP_APP_MAIN_JS     AMP_FS_ROOT_DIR"/app.js"
#define AMP_APP_MAIN_JSB    AMP_FS_ROOT_DIR"/app.jsb"
#define AMP_APP_MAIN_JSON   AMP_FS_ROOT_DIR"/app.json"
#define AMP_APP_PACKAGE_JSON   AMP_FS_ROOT_DIR"/package.json"
#define AMP_APP_INDEX_JS     AMP_FS_ROOT_DIR"/index.js"

typedef enum {
    HAL_SEEK_SET,
    HAL_SEEK_CUR,
    HAL_SEEK_END
} hal_fs_seek_type_t;

/* sdk version */
#define AMP_VERSION_LENGTH  128
#define AMP_VERSION_NUMBER  "1.0.0"
#define AMP_GIT_COMMIT      "g9c40fab"
#define AMP_MODULE_HARDWARE aos_get_module_hardware_version()
#define AMP_MODULE_SOFTWARE aos_get_module_software_version()

/* interval device infomation */
#define AMP_DEVICE_TOKEN                "_amp_device_token"
#define AMP_DEVICE_TOKEN_VERIFY_FLAG    "_amp_device_token_verify_flag"
#define AMP_DEVICE_TOKEN_LENGTH         64

#define AMP_INTERNAL_PRODUCTKEY         "_amp_internal_productkey"
#define AMP_INTERNAL_PRODUCTSECRET      "_amp_internal_productsecret"
#define AMP_INTERNAL_DEVICENAME         "_amp_internal_devicename"
#define AMP_INTERNAL_DEVICESECRET       "_amp_internal_devicesecret"

/* customer device infomation */
#define AMP_CUSTOMER_PRODUCTKEY         "_amp_customer_productkey"
#define AMP_CUSTOMER_PRODUCTSECRET      "_amp_customer_productsecret"
#define AMP_CUSTOMER_DEVICENAME         "_amp_customer_devicename"
#define AMP_CUSTOMER_DEVICESECRET       "_amp_customer_devicesecret"

#define osWaitForever 0xFFFFFFFF /* wait forever timeout value */
#define PLATFORM_WAIT_INFINITE (~0)

#define IOTX_PRODUCT_KEY_LEN            (20)
#define IOTX_DEVICE_NAME_LEN            (32)
#define IOTX_DEVICE_SECRET_LEN          (64)
#define IOTX_PRODUCT_SECRET_LEN         (64)

/* task stack size */
#define JSENGINE_TASK_STACK_SIZE   1024 * 15

/* task priority */
#define JSE_TSK_PRIORITY        AOS_DEFAULT_APP_PRI + 5
#define CLI_TSK_PRIORITY        AOS_DEFAULT_APP_PRI + 7
#define MQTT_TSK_PRIORITY       AOS_DEFAULT_APP_PRI + 6
#define MQTTHTTP_TSK_PRIORITY   AOS_DEFAULT_APP_PRI + 4
#define SSDP_TSK_PRIORITY       AOS_DEFAULT_APP_PRI + 3
#define WEBSOCKET_TSK_PRIORITY  AOS_DEFAULT_APP_PRI + 4
#define WIFI_TSK_PRIORITY       AOS_DEFAULT_APP_PRI + 4
#define UPDATE_TSK_PRIORITY     AOS_DEFAULT_APP_PRI + 6
#define ADDON_TSK_PRIORRITY     AOS_DEFAULT_APP_PRI + 3

/* log system operation wrapper */
#define amp_debug(mod, fmt, ...)     LOGD(mod, fmt, ##__VA_ARGS__)
#define amp_info(mod, fmt, ...)      LOGI(mod, fmt, ##__VA_ARGS__)
#define amp_warn(mod, fmt, ...)      LOGW(mod, fmt, ##__VA_ARGS__)
#define amp_error(mod, fmt, ...)     LOGE(mod, fmt, ##__VA_ARGS__)
#define amp_fatal(mod, fmt, ...)     LOGF(mod, fmt, ##__VA_ARGS__)

/* JS console stdout */
#if defined(AMP_REPL_ENABLE) && defined(AMP_REPL_STDIO_EXTERNAL)
#define amp_console(fmt, ...)    do { \
        repl_printf(fmt, ##__VA_ARGS__); \
        repl_printf("\r\n"); \
    }while(0)
#else
#define amp_console(fmt, ...)    do { \
        aos_printf(fmt, ##__VA_ARGS__); \
        aos_printf("\r\n"); \
    }while(0)
#endif
/* md5 */
#define amp_md5_context     mbedtls_md5_context
#define amp_md5             mbedtls_md5
#define amp_md5_init        mbedtls_md5_init
#define amp_md5_starts      mbedtls_md5_starts
#define amp_md5_update      mbedtls_md5_update
#define amp_md5_finish      mbedtls_md5_finish
#define amp_md5_free        mbedtls_md5_free

/* sha1 */
#define amp_sha1_context    mbedtls_sha1_context
#define amp_sha1_init       mbedtls_sha1_init
#define amp_sha1_starts     mbedtls_sha1_starts
#define amp_sha1_update     mbedtls_sha1_update
#define amp_sha1_finish     mbedtls_sha1_finish
#define amp_sha1_free       mbedtls_sha1_free

/* aes */
#define AMP_AES_ENCRYPT     MBEDTLS_AES_ENCRYPT
#define AMP_AES_DECRYPT     MBEDTLS_AES_DECRYPT
#define amp_aes_context     mbedtls_aes_context
#define amp_aes_init        mbedtls_aes_init
#define amp_aes_setkey_enc  mbedtls_aes_setkey_enc
#define amp_aes_setkey_dec  mbedtls_aes_setkey_dec
#define amp_aes_crypt_cbc   mbedtls_aes_crypt_cbc
#define amp_aes_free        mbedtls_aes_free

/* des */
#define amp_des_context     mbedtls_des_context
#define amp_des_init        mbedtls_des_init
#define amp_des_setkey_enc  mbedtls_des_setkey_enc
#define amp_des_crypt_ecb   mbedtls_des_crypt_ecb
#define amp_des_free        mbedtls_des_free

/* base64 */
#define amp_base64_encode   mbedtls_base64_encode
#define amp_base64_decode   mbedtls_base64_decode

#endif /* AMP_DEFINES_H */
