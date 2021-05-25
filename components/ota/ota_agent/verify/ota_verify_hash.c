/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <string.h>
#include "ota_log.h"
#include "ota_import.h"
#include "ota_hal_digest.h"
#include "ota_hal_os.h"

#define OTA_BUF_VERIFY 512

int ota_hash_init(ota_hash_ctx_t *ctx, unsigned char type)
{
    int ret = 0;
    if (ctx == NULL) {
        OTA_LOG_E("Invalid hash param");
        return OTA_INVALID_PARAMETER;
    }
    ctx->hash_method = type;
    switch (type) {
        case OTA_SHA256:
            ota_sha256_init(&ctx->sha256_ctx);
            ota_sha256_starts(&ctx->sha256_ctx, 0);
            break;
        case OTA_MD5:
            ota_md5_init(&ctx->md5_ctx);
            ota_md5_starts(&ctx->md5_ctx);
            break;
        default:
            ret = OTA_INVALID_PARAMETER;
            break;
    }
    return ret;
}

int ota_hash_update(ota_hash_ctx_t *ctx, const unsigned char *buf, unsigned int len)
{
    int ret = 0;
    if (ctx == NULL) {
        OTA_LOG_E("Invalid hash param");
        return OTA_INVALID_PARAMETER;
    }
    switch (ctx->hash_method) {
        case OTA_SHA256:
            ota_sha256_update(&ctx->sha256_ctx, buf, len);
            break;
        case OTA_MD5:
            ota_md5_update(&ctx->md5_ctx, buf, len);
            break;
        default:
            ret = OTA_INVALID_PARAMETER;
            break;
    }
    return ret;
}

int ota_hash_final(ota_hash_ctx_t *ctx, unsigned char *dgst)
{
    int ret = 0;
    if (ctx == NULL) {
        OTA_LOG_E("Invalid hash param");
        return OTA_INVALID_PARAMETER;
    }
    switch (ctx->hash_method) {
        case OTA_SHA256:
            ota_sha256_finish(&ctx->sha256_ctx, dgst);
            ota_sha256_free(&ctx->sha256_ctx);
            break;
        case OTA_MD5:
            ota_md5_finish(&ctx->md5_ctx, dgst);
            ota_md5_free(&ctx->md5_ctx);
            break;
        default:
            ret = OTA_INVALID_PARAMETER;
            break;
    }
    return ret;
}

int ota_check_hash(unsigned char type, char *src, char *dst)
{
    int ret = 0;
    switch (type) {
        case OTA_SHA256:
            OTA_LOG_I("SHA256 src:%s dst:%s", src, dst);
            if (strncmp(dst, src, 64) != 0) {
                ret = OTA_VERIFY_SHA2_FAIL;
            }
            break;
        case OTA_MD5:
            OTA_LOG_I("md5 src:%s dst:%s", src, dst);
            if (strncmp(dst, src, 32) != 0) {
                ret = OTA_VERIFY_MD5_FAIL;
            }
            break;
        default:
            ret = OTA_INVALID_PARAMETER;
            break;
    }
    return ret;
}

int ota_check_image_hash(unsigned int image_len, unsigned char type, char *hash_value, int len)
{
    int ret                   = 0;
    char *rd_buf              = NULL;
    unsigned int read_size    = 0;
    unsigned int offset       = 0;
    ota_hash_ctx_t      ctx   = {0};
    ota_crc16_ctx  crc_ctx    = {0};
    unsigned short crc_result = 0;
    if (NULL == hash_value) {
        ret = OTA_VERIFY_IMAGE_FAIL;
        goto EXIT;
    }
    switch (type) {
        case OTA_SHA256:
            if (len < 32) {
                ret = OTA_VERIFY_SHA2_FAIL;
                goto EXIT;
            }
            break;
        case OTA_MD5:
            if (len < 16) {
                ret = OTA_VERIFY_MD5_FAIL;
                goto EXIT;
            }
            break;
        default:
            ret = OTA_INVALID_PARAMETER;
            goto EXIT;
    }

    rd_buf = ota_malloc(OTA_BUF_VERIFY);
    if (rd_buf == NULL) {
        ret = OTA_VERIFY_IMAGE_FAIL;
        goto EXIT;
    }
    ret = ota_hash_init(&ctx, type);
    if (ret != 0) {
        goto EXIT;
    }
    ota_crc16_init(&crc_ctx);
    while (offset < image_len) {
        unsigned int off = offset;
        (image_len - offset >= OTA_BUF_VERIFY) ? (read_size = OTA_BUF_VERIFY) : (read_size = image_len - offset);
        ret = ota_read(&off, rd_buf, read_size);
        if (ret < 0) {
            ret = OTA_VERIFY_IMAGE_FAIL;
            goto EXIT;
        }
        ret = ota_hash_update(&ctx, (const unsigned char *)rd_buf, read_size);
        if (ret != 0) {
            goto EXIT;
        }
        ota_crc16_update(&crc_ctx, (void *)rd_buf, read_size);
        offset += read_size;
        ota_msleep(5);
    }
    memset(hash_value, 0x00, len);
    ota_crc16_final(&crc_ctx, &crc_result);
    OTA_LOG_I("check image_crc16 = %x\r\n", crc_result);
    ret = ota_hash_final(&ctx, (unsigned char *)hash_value);
EXIT:
    if (NULL != rd_buf) {
        ota_free(rd_buf);
        rd_buf = NULL;
    }
    if (ret != 0) {
        OTA_LOG_E("check image hash:%d", ret);
    }
    return ret;
}

int ota_get_image_info(unsigned int image_size, ota_image_info_t *tmp_info)
{
    int ret = -1;
    unsigned int off_set = 0;
    if ((tmp_info == NULL) || (image_size <= sizeof(ota_image_info_t))) {
        OTA_LOG_E("input param err for getting image info!");
        return ret;
    }
    off_set = image_size - sizeof(ota_image_info_t);
    OTA_LOG_I("bin size:%d off:%d\r\n", image_size, off_set);
    ret = ota_read(&off_set, (char *)tmp_info, sizeof(ota_image_info_t));
    if (ret < 0) {
        OTA_LOG_E("read image info err!");
    }
    OTA_LOG_I("magic:0x%04x, size:%d, crc16:0x%02x\r\n", tmp_info->image_magic, tmp_info->image_size, tmp_info->image_crc16);
    return ret;
}

int ota_check_image(unsigned int size)
{
    int ret                 = 0;
    char image_md5[16]      = {0};
    char download_md5[33]   = {0};
    char cal_md5[33]        = {0};
    ota_image_info_t image_info = {0};

    ret = ota_get_image_info(size, &image_info);
    if (ret < 0) {
        ret = OTA_VERIFY_IMAGE_FAIL;
        goto EXIT;
    }
    ret = ota_hex2str(download_md5, (const unsigned char *)image_info.image_md5, sizeof(download_md5), sizeof(image_info.image_md5));
    if (ret != 0) {
        ret = OTA_VERIFY_IMAGE_FAIL;
        goto EXIT;
    }
    OTA_LOG_I("magic:0x%04x size:%d md5:%s crc16:0x%02x", image_info.image_magic, image_info.image_size, download_md5, image_info.image_crc16);
    if ((image_info.image_magic != OTA_BIN_MAGIC_APP) &&
        (image_info.image_magic != OTA_BIN_MAGIC_KERNEL) &&
        (image_info.image_magic != OTA_BIN_MAGIC_ALL) &&
        (image_info.image_magic != OTA_BIN_MAGIC_MCU) &&
        (image_info.image_magic != OTA_BIN_MAGIC_FS)) {
        ret = OTA_INVALID_PARAMETER;
        goto EXIT;
    }
    if (image_info.image_size == 0) {
        ret = OTA_VERIFY_IMAGE_FAIL;
        goto EXIT;
    }
    ret = ota_check_image_hash(image_info.image_size, OTA_MD5, image_md5, 16);
    if (ret != 0) {
        goto EXIT;
    }
    ret = ota_hex2str(cal_md5, (const unsigned char *)image_md5, sizeof(cal_md5), sizeof(image_md5));
    if (ret != 0) {
        ret = OTA_VERIFY_IMAGE_FAIL;
        goto EXIT;
    }
    ret = ota_check_hash(OTA_MD5, cal_md5, download_md5);
    if (ret != 0) {
        goto EXIT;
    }
EXIT:
    if (ret != 0) {
        OTA_LOG_E("ota check_image :%d", ret);
    }
    return ret;
}
