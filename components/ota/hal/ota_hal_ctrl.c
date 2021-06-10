/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include "ota_log.h"
#include "ota_import.h"
#include "ota_hal_os.h"
#include "ota_hal_digest.h"
#include "ota_hal.h"
#include <aos/vfs.h>

static ota_hash_ctx_t hash_ctx  = {0};
static unsigned short upg_flag  = 0;
static unsigned char  is_header = 0;
#ifdef OTA_CONFIG_LOCAL_RSA
static unsigned int valid_image_len = 0;
static unsigned int calculated_len = 0;
#endif

int ota_int(ota_boot_param_t *param)
{
    int ret = 0;
    if (param == NULL) {
        return OTA_UPGRADE_PARAM_FAIL;
    }
    is_header = 0;
    upg_flag  = 0;
#ifdef OTA_CONFIG_LOCAL_RSA
    calculated_len = 0;
    valid_image_len = param->len - sizeof(ota_image_info_t) - sizeof(ota_sign_info_t);
#endif
    ret = ota_hal_init(param);
    if (ret == 0) {
        ret = ota_hash_init(&hash_ctx, param->hash_type);
    }
    OTA_LOG_I("ota init ret = %d\r\n", ret);
    return ret;
}

int ota_write(unsigned int *off, char *in_buf, unsigned int in_buf_len)
{
    int ret = 0;
    if ((*off == 0) && (is_header == 0)) {
        unsigned char xz_header[6] = { 0xFD, '7', 'z', 'X', 'Z', 0x00 };
        if (memcmp(in_buf, "BSDIFF40", 8) == 0) {
            upg_flag = OTA_UPGRADE_DIFF;
        } else if (memcmp(in_buf, xz_header, 6) == 0) {
            upg_flag = OTA_UPGRADE_XZ;
        }
        OTA_LOG_I("ota header:0x%x\n", upg_flag);
        is_header = 1;
    }
    ret = ota_hal_write(off, in_buf, in_buf_len);
    if (ret >= 0) {
#ifdef OTA_CONFIG_LOCAL_RSA
        if (calculated_len <= valid_image_len) {
            ota_hash_ctx_t tmp_ctx = hash_ctx;
            unsigned int tmp_len = 0;
            ((calculated_len + in_buf_len) <= valid_image_len) ? (tmp_len = in_buf_len) :
                                            (tmp_len = valid_image_len - calculated_len);
            ret = ota_hash_update(&hash_ctx, (const unsigned char *)in_buf, tmp_len);
            if (ret >= 0) {
                calculated_len += in_buf_len;
            } else {
                hash_ctx = tmp_ctx;
            }
        }
#else
        ret = ota_hash_update(&hash_ctx, (const unsigned char *)in_buf, in_buf_len);
#endif
    }
    return ret;
}

int ota_read(unsigned int *off, char *out_buf, unsigned int out_buf_len)
{
    int ret = 0;
    ret = ota_hal_read(off, out_buf, out_buf_len);
    return ret;
}

int ota_clear()
{
    is_header = 0;
    upg_flag  = 0;
    return ota_clear_paramters();
}

int ota_verify(ota_boot_param_t *param)
{
    int ret = OTA_UPGRADE_SET_BOOT_FAIL;
    unsigned char hash[32] = {0};
    char dst_hash[65] = {0};
    if (param == NULL) {
        return ret;
    }
    is_header = 0;
    if (upg_flag != 0) {
        param->upg_flag = upg_flag;
    } else if (param->upg_flag == 0) {
        param->upg_flag = OTA_UPGRADE_ALL;
    }
    memset(hash, 0x00, sizeof(hash));
    ret = ota_hash_final(&hash_ctx, hash);
    if (ret == 0) {
        ret = ota_hex2str(dst_hash, hash, sizeof(dst_hash), sizeof(hash));
        if (ret == 0) {
            ret = ota_check_hash(param->hash_type, param->hash, (char *)dst_hash);
            if (ret < 0) {
                OTA_LOG_E("ota verify hash failed!");
                return ret;
            }
            /* verify RSA signature */
#if OTA_CONFIG_LOCAL_RSA
            ret = ota_verify_rsa((unsigned char *)param->sign, (const char *)dst_hash, param->hash_type);
            if (ret < 0) {
                ret = OTA_VERIFY_RSA_FAIL;
                return ret;
            }
#else
            if (strlen(param->sign) > 0) {
                ret = ota_verify_rsa((unsigned char *)param->sign, (const char *)dst_hash, param->hash_type);
                if (ret < 0) {
                    ret = OTA_VERIFY_RSA_FAIL;
                    return ret;
                }
            }
#endif
            /* verify image */
#if !defined CONFIG_PING_PONG_OTA
            if ((param->upg_flag != OTA_UPGRADE_DIFF) && (param->upg_flag != OTA_UPGRADE_CUST)) {
                ret = ota_check_image(param->len);
                if (ret < 0) {
                    ret = OTA_VERIFY_IMAGE_FAIL;
                }
            }
#endif
        }
    }
    OTA_LOG_I("ota boot finish ret:%d crc:0x%04x", ret, param->crc);
    return ret;
}

int ota_verify_fsfile(ota_boot_param_t *param, char *file_path)
{
    int fd = -1;
    int ret = OTA_UPGRADE_SET_BOOT_FAIL;
    unsigned char hash[32] = {0};
    char dst_hash[65] = {0};
    char tmp_buf[128] = {0};
    unsigned int offset = 0;
    unsigned int read_len = 0;
    ota_hash_ctx_t tmp_hash_ctx;
    if ((param == NULL) || (file_path == NULL)) {
        return ret;
    }
    memset(hash, 0x00, sizeof(hash));
    ret = ota_hash_init(&tmp_hash_ctx, param->hash_type);
    if (ret < 0) {
        OTA_LOG_E("subdev fs file hash init failed\n");
        return ret;
    }
    fd = ota_fopen(file_path, O_RDONLY);
    if (fd < 0) {
        ret = OTA_UPGRADE_SET_BOOT_FAIL;
        OTA_LOG_E("open %s failed\n", file_path);
        return ret;
    }
    OTA_LOG_I("fs size = %d\n", param->len);
    while (offset < param->len) {
        (param->len - offset >= sizeof(tmp_buf)) ? (read_len = sizeof(tmp_buf)) : (read_len = param->len - offset);
        ret = ota_fread(fd, (void *)tmp_buf, read_len);
        if (ret < 0) {
            OTA_LOG_E("verify fs file read file failed\r\n");
            ret = OTA_VERIFY_IMAGE_FAIL;
            break;
        }
        ret = ota_hash_update(&tmp_hash_ctx, (const unsigned char *)tmp_buf, read_len);
        if (ret < 0) {
            break;
        }
        offset += read_len;
        ota_msleep(5);
    }
    if (fd >= 0) {
        (void)ota_fclose(fd);
    }
    if (ret < 0) {
        OTA_LOG_E("get fs file hash value failed\n");
        return ret;
    }
    ret = ota_hash_final(&tmp_hash_ctx, hash);
    if (ret == 0) {
        ret = ota_hex2str(dst_hash, hash, sizeof(dst_hash), sizeof(hash));
        if (ret == 0) {
            ret = ota_check_hash(param->hash_type, param->hash, (char *)dst_hash);
            if (ret < 0) {
                OTA_LOG_E("ota verify hash failed!");
                return ret;
            }
            /* verify RSA signature */
#if OTA_CONFIG_LOCAL_RSA
            ret = ota_verify_rsa((unsigned char *)param->sign, (const char *)dst_hash, param->hash_type);
            if (ret < 0) {
                ret = OTA_VERIFY_RSA_FAIL;
                return ret;
            }
#else
            if (strlen(param->sign) > 0) {
                ret = ota_verify_rsa((unsigned char *)param->sign, (const char*)dst_hash, param->hash_type);
                if (ret < 0) {
                    ret = OTA_VERIFY_RSA_FAIL;
                    return ret;
                }
            }
#endif
         }
    }
    OTA_LOG_I("verfiy fs over ret:%d\n", ret);
    return ret;
}