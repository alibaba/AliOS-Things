/** @defgroup ota_internal_api
 * @{
 *
 * This is an include file of OTA internal interface.
 *
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef OTA_API_H
#define OTA_API_H
#include "ota_agent.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OTA_INFO        (UAGENT_FUNC_USER_BASE)   /* ota process */
#define OTA_UPGRADE_CMD (UAGENT_FUNC_USER_BASE+1) /* upgrade command */
#define VERSION_INFORM  (UAGENT_FUNC_USER_BASE+2) /* version inform */

#define OTA_COAP_OPTION_BLOCK      23    /*OTA coap option block*/

#ifndef OTA_DOWNLOAD_RETRY_CNT
#define OTA_DOWNLOAD_RETRY_CNT     5     /*OTA download retry count default:5*/
#endif

#ifndef OTA_DOWNLOAD_TIMEOUT
#define OTA_DOWNLOAD_TIMEOUT       20000 /*OTA download timeout Unit:ms default:20s*/
#endif

#ifndef OTA_DOWNLOAD_BLOCK_SIZE
#define OTA_DOWNLOAD_BLOCK_SIZE    1024   /*OTA download block size:512*/
#endif

#ifndef OTA_FLASH_BLOCK_SIZE
#define OTA_FLASH_BLOCK_SIZE     0x10000   /*OTA erase/write block size:64K*/
#endif

#ifndef OTA_FLASH_WRITE_CACHE_SIZE
#define OTA_FLASH_WRITE_CACHE_SIZE 2048 /*OTA write flash cache size*/
#endif

#define OTA_MSG_LEN  256 /*OTA topic message max len*/

#define L_EXTRACT_U16(d) ((*((unsigned char *)(d)) << 8) | *((unsigned char *)(d) + 1))
#define L_EXTRACT_U32(d)                                              \
    ((*((unsigned char *)(d)) << 24) | (*((unsigned char *)(d) + 1) << 16) | \
    (*((unsigned char *)(d) + 2) << 8) | *((unsigned char *)(d) + 3))

#define EXTRACT_U16(d) (*((unsigned char *)(d)) | (*((unsigned char *)(d) + 1) << 8))
#define EXTRACT_U32(d)                                              \
    (*((unsigned char *)(d)) | (*((unsigned char *)(d) + 1) << 8) | \
    (*((unsigned char *)(d) + 2) << 16) | (*((unsigned char *)(d) + 3) << 24))

#define ENCODE_U16(d, val)                             \
{                                                      \
    *((unsigned char *)(d))     = (val)&0xFF;          \
    *((unsigned char *)(d) + 1) = ((val) >> 8) & 0xFF; \
}

#define ENCODE_U32(d, val)                              \
{                                                       \
    *((unsigned char *)(d))     = (val)&0xFF;           \
    *((unsigned char *)(d) + 1) = ((val) >> 8) & 0xFF;  \
    *((unsigned char *)(d) + 2) = ((val) >> 16) & 0xFF; \
    *((unsigned char *)(d) + 3) = ((val) >> 24) & 0xFF; \
}

enum {
    OTA_PROCESS_NORMAL = 0,
    OTA_PROCESS_UAGENT_OTA
};

typedef enum {
    OTA_REQ_VERSION = 0,
    OTA_UPGRADE_SOC = 1,
    OTA_UPGRADE_MCU = 2
} OTA_UPGRADE_CMD_T;

/**
*
* ota transport
*
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

/*
*
* ota verify
*/
#define OTA_HASH_NONE                (0)
#define OTA_SHA256                   (1)
#define OTA_MD5                      (2)
#define OTA_SHA256_HASH_SIZE         (32)
#define OTA_MD5_HASH_SIZE            (16)
#define OTA_SIGN_BITNUMB             (2048)

#if defined(__ICCARM__)
#define OTA_WEAK                __weak
#else
#define OTA_WEAK                __attribute__((weak))
#endif

int ota_read_parameter(ota_boot_param_t *ota_param);
int ota_update_parameter(ota_boot_param_t *ota_param);

int ota_clear();
int hal_reboot_bank(void);
int ota_clear_paramters();
int ota_is_download_mode(void);
int ota_int(ota_boot_param_t *param);
int ota_verify(ota_boot_param_t *param);
int ota_verify_fsfile(ota_boot_param_t *param, char *file_path);
int ota_write(unsigned int *off, char *in_buf, unsigned int in_buf_len);
int ota_read(unsigned int *off, char *out_buf, unsigned int out_buf_len);

int ota_fclose(int fd);
int ota_fopen(const char *filename, int mode);
int ota_fread(int fd, void *buf, unsigned int size);
int ota_fwrite(int fd, const void *buf, unsigned int size);
int ota_check_hash(unsigned char type, char *src, char *dst);  /* ota compare hash value. */
unsigned short ota_get_upgrade_flag(void);                     /* ota get upgrade flag. */
int ota_update_upg_flag(unsigned short flag);                  /* ota update upgrade flag. */
int ota_update_process(const char *err, const int step);
unsigned int ota_parse_ota_type(unsigned char *buf, unsigned int len); /* parse ota type:diff xz or normal*/
int ota_transport_inform_otaver(ota_service_t *ctx);
void ota_set_upgrade_status(char is_upgrade);
/**
 * @}
 */
#ifdef __cplusplus
}
#endif
#endif /*__OTA_API_H__*/