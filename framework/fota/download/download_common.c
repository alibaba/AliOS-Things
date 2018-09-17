/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <string.h>

#include "md5.h"
#include "ota_download.h"
#include "ota_util.h"
#include "ota_log.h"
#include "ota_update_manifest.h"
#include <aos/log.h>

#define KEY_OTA_BREAKPOINT  "key_ota_breakpoint"
#define KEY_OTA_MD5         "key_ota_md5"
#define KEY_OTA_MD5_CTX     "key_ota_md5_ctx"

#define OTA_BUFFER_MAX_SIZE  1536

#define AOS_SINGLE_TAG (0xabababab)
#define AOS_KERNEL_TAG (0xcdcdcdcd)
#define AOS_APP_TAG    (0xefefefef)
#define OTA_BUF_VERIFY 512

typedef struct
{
    unsigned int   image_magic;
    unsigned int   image_size;
    unsigned char  image_md5_value[16];
    unsigned char  image_reserver[2];
    unsigned short image_crc16;
} ota_image_info;

static ota_image_info ota_image_identity;
static int ota_image_size = 0;

void save_state(uint32_t breakpoint, MD5_CTX *pMD5)
{
#ifdef CONFIG_AOS_FOTA_BREAKPOINT     
    ota_set_update_breakpoint(breakpoint);
    ota_set_cur_MD5_context(pMD5);
#endif     
}

uint32_t ota_get_update_breakpoint()
{
    uint32_t offset = 0;
#ifdef CONFIG_AOS_FOTA_BREAKPOINT    
    int len = 4;
    if (aos_kv_get(KEY_OTA_BREAKPOINT, &offset, &len)) {
        offset = 0;
    }
#endif    
    //OTA_LOG_I("ota_get_update_breakpoint=%d",offset);
    return offset;
}

int ota_set_update_breakpoint(uint32_t offset)
{
#ifdef CONFIG_AOS_FOTA_BREAKPOINT   
    return  aos_kv_set(KEY_OTA_BREAKPOINT, &offset, 4, 1);
#endif
    return 0;    
}

int ota_get_last_MD5(char *value)
{
#ifdef CONFIG_AOS_FOTA_BREAKPOINT       
    int len = 33;
    int ret = aos_kv_get(KEY_OTA_MD5, value, &len);
    return ret;
#endif
    return 0;    
}

int ota_set_cur_MD5(char *value)
{
#ifdef CONFIG_AOS_FOTA_BREAKPOINT     
    return  aos_kv_set(KEY_OTA_MD5, value, 33, 1);
#endif
    return 0;    
}

int ota_get_last_MD5_context(MD5_CTX *md5ctx)
{
#ifdef CONFIG_AOS_FOTA_BREAKPOINT      
    int len = sizeof(MD5_CTX);
    return aos_kv_get(KEY_OTA_MD5_CTX, md5ctx, &len);
#endif
    return 0; 
    
}

int ota_set_cur_MD5_context(MD5_CTX *md5ctx)
{
#ifdef CONFIG_AOS_FOTA_BREAKPOINT     
    return  aos_kv_set(KEY_OTA_MD5_CTX, md5ctx, sizeof(MD5_CTX), 1);
#endif 
    return 0;    
}

int ota_get_download_image_size()
{
    return ota_image_size;
}

void ota_set_download_image_size(int image_size)
{
    ota_image_size = image_size;
}

static int ota_check_md5(const uint8_t *cur_hash, const char *download_hash)
{
    if (cur_hash == NULL || download_hash == NULL) {
        OTA_LOG_E("update_packet MD5 check FAIL!");
        return -1;
    }
    char digest_str[33] = { 0 };
    int  i              = 0;
    for (; i < 16; i++) {
        snprintf(digest_str + i * 2, 2 + 1, "%02X", cur_hash[i]);
    }
    OTA_LOG_I("url md5=%s", download_hash);
    OTA_LOG_I("digestMD5=%s", digest_str);
    if (strncmp(digest_str, download_hash, 32)) {
        OTA_LOG_E("update_packet md5 check FAIL!");
        return -1;
    }
    return 0;
}

int ota_check_image(read_flash_cb_t read_fuc)
{
    int                   ret     = 0;
    int                   i       = 0;
    volatile uint32_t     off_set = 0;
    unsigned short        bin_parse_context_crc = 0;
    unsigned char         image_md5_value[33] = {0};
    unsigned char         download_md5_str_value[33] = {0};
    unsigned int          read_size    = 0;
    unsigned char        *rd_buf       = NULL;
    unsigned char         test_buf[33] = { 0 };
    int                   bin_size     = 0;
    MD5_CTX  g_ctx;

    bin_size                 = ota_get_download_image_size();
    OTA_LOG_I("bin_size = %d", bin_size);
    if ((NULL == read_fuc) || (bin_size <= sizeof(ota_image_info))) {
        ret = -1;
        OTA_LOG_E("check image input parameters error!");
        return ret;
    }
    off_set = bin_size - sizeof(ota_image_info);
    OTA_LOG_I("bin_size = %d", bin_size);
    OTA_LOG_I("off_set = %d", off_set);
    if (read_fuc(&off_set, (unsigned char *)&ota_image_identity,
                 sizeof(ota_image_info), 0x01) < 0) {
        ret = -1;
        OTA_LOG_E("image parse failed!");
        return ret;
    }
    memset(test_buf, 0x00, sizeof(test_buf));
    for (i = 0; i < 16; i++) {
        snprintf((char *)(test_buf + i * 2), 2 + 1, "%02X",
                     ota_image_identity.image_md5_value[i]);
    }
    OTA_LOG_I("ota_image_identity.image_magic=%x",
              ota_image_identity.image_magic);
    OTA_LOG_I("ota_image_identity.image_size=%x",
              ota_image_identity.image_size);
    OTA_LOG_I("ota_image_identity.image_md5_value=%s", test_buf);
    OTA_LOG_I("ota_image_identity.image_crc16=%x",
              ota_image_identity.image_crc16);
    if ((ota_image_identity.image_magic != AOS_SINGLE_TAG) &&
        (ota_image_identity.image_magic != AOS_KERNEL_TAG) &&
        (ota_image_identity.image_magic != AOS_APP_TAG)) {
        ret = -1;
        OTA_LOG_E("image magic error");
        return ret;
    }
    if (ota_image_identity.image_size != bin_size - sizeof(ota_image_info)) {
        ret = -1;
        OTA_LOG_E("image size error!");
        return ret;
    }
    bin_size = ota_image_identity.image_size;
    rd_buf   = aos_malloc(OTA_BUF_VERIFY);
    if (rd_buf == NULL) {
        ret = -1;
        OTA_LOG_E("ota malloc identity read buf failed!");
        return ret;
    } 
    MD5_Init(&g_ctx); 
    off_set = 0;
    while (off_set < bin_size) {
        (bin_size - off_set >= OTA_BUF_VERIFY) ? (read_size = OTA_BUF_VERIFY)
                                    : (read_size = bin_size - off_set);
        if (read_fuc(&off_set, rd_buf, read_size, 0x01) < 0) {
            ret = -1;
            OTA_LOG_E("image parse read flash failed!");
            goto OTA_IMAGE_IDENTITY_FAILED;
        }
        MD5_Update(&g_ctx, (const uint8_t *)rd_buf,read_size);   
    }

    memset(image_md5_value, 0x00, sizeof(image_md5_value));
    MD5_Final(image_md5_value, &g_ctx);
    memset(download_md5_str_value, 0x00, sizeof(download_md5_str_value));
    for (i = 0; i < 16; i++) {
        snprintf((char *)download_md5_str_value + i * 2, 2 + 1, "%02X",
                     ota_image_identity.image_md5_value[i]);
    }
    if (ota_check_md5(image_md5_value, (const char *)&download_md5_str_value) <
        0) {
        ret = -1;
        LOG("###Error md5 failed");
        OTA_LOG_E("identity image md5 failed!");
        goto OTA_IMAGE_IDENTITY_FAILED;
    }
    LOG("Great! OTA image is ok ....");
    OTA_LOG_I("Great! OTA image is ok ....");
OTA_IMAGE_IDENTITY_FAILED:
    aos_free(rd_buf);
    return ret;
}
