/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __OTA_API_H__
#define __OTA_API_H__
#include "ota/ota_service.h"

#define OTA_COAP_OPTION_BLOCK            23        /*OTA coap option block*/

#ifndef OTA_DOWNLOAD_RETRY_CNT
#define OTA_DOWNLOAD_RETRY_CNT           5         /*OTA download retry count default:5*/
#endif

#ifndef OTA_DOWNLOAD_TIMEOUT
#define OTA_DOWNLOAD_TIMEOUT             20000     /*OTA download timeout Unit:ms default:20s*/
#endif

#ifndef OTA_DOWNLOAD_BLOCK_SIZE
#define OTA_DOWNLOAD_BLOCK_SIZE          512      /*OTA download block size:1024*/
#endif

#ifndef OTA_FLASH_WRITE_CACHE_SIZE
#define OTA_FLASH_WRITE_CACHE_SIZE       1024      /*OTA write flash cache size*/
#endif

/**
*
* ota_parse_dl_url error code
**/
typedef enum {
        OTA_PAR_SUCCESS      = 0,
        OTA_ARGS_PAR_FAIL    = -1,
        OTA_MESSAGE_PAR_FAIL = -2,
        OTA_SUCCESS_PAR_FAIL = -3,
        OTA_DATA_PAR_FAIL    = -4,
        OTA_URL_PAR_FAIL     = -5,
        OTA_VER_PAR_FAIL     = -6,
        OTA_HASH_PAR_FAIL    = -7,
        OTA_MD5_PAR_FAIL     = -8,
        OTA_SHA256_PAR_FAIL  = -9,
        OTA_SIZE_PAR_FAIL    = -10,
        OTA_SIGN_PAR_FAIL    = -11,
} OTA_TRANSPORT_ERR_E;

ota_service_t * ota_get_service_manager(void);

/**
 * ota_parse_host_url  OTA parse host url
 *
 * @param[in] char *url           pasre url to host name & uri.
 *
 * @return host_name              host name from download url.
 * @return host_uri               host uri from download url.
 */
void ota_parse_host_url(char *url, char **host_name, char **host_uri);

/**
 * ota_parse_dl_url  OTA parse download url from transport message
 *
 * @param[in] char *json          transport message from Cloud.
 *
 * @return NULL.
 */
void ota_parse_dl_url(const char *json);

#endif /*__OTA_API_H__*/

