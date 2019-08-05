/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "ota_log.h"
#include "ota_crypto.h"
#include "ota_plat_ctrl.h"
#include "aos/hal/flash.h"
#include "ota_hal_common.h"
#include "ota/ota_service.h"

static ota_crc16_ctx ctx               = {0};
static ota_hash_ctx_t *ota_hash_ctx    = NULL;
static int ota_is_header               = 0;
static ota_boot_param_t *ota_param     = NULL;

static int ota_header_check(char *buf, unsigned int len)
{
    unsigned char xz_header[6] = { 0xFD, '7', 'z', 'X', 'Z', 0x00 };
    if(ota_param == NULL) {
        return OTA_UPGRADE_PARAM_FAIL;
    }
    if (memcmp(buf, "BSDIFF40", 8) == 0) {
        ota_param->upg_flag = OTA_UPGRADE_DIFF;
    } else if (memcmp(buf, xz_header, 6) == 0) {
        ota_param->upg_flag = OTA_UPGRADE_XZ;
    } else {
        ota_param->upg_flag = OTA_UPGRADE_ALL;
    }
    OTA_LOG_I("ota header:0x%04x \n", ota_param->upg_flag);
    return 0;
}

int ota_int(ota_boot_param_t *param)
{
    int ret = 0;
    if(param == NULL) {
        return OTA_UPGRADE_PARAM_FAIL;
    }
    ota_param  = param;
    ota_is_header = 0;
    ret = ota_hal_init(param);
    if(ret == 0) {
        if(ota_hash_ctx != NULL) {
            ota_free(ota_hash_ctx);
            ota_hash_ctx = NULL;
        }
        ret = OTA_INIT_FAIL;
        ota_hash_ctx = ota_malloc_hash_ctx(param->hash_type);
        if (NULL != ota_hash_ctx) {
            ret = ota_hash_init(ota_hash_ctx);
            if (ret != 0) {
                ota_free(ota_hash_ctx);
                ota_hash_ctx = NULL;
            }
        }
        ota_crc16_init(&ctx);
    }
    OTA_LOG_I("ota init ret = %d\r\n", ret);
    return ret;
}

int ota_clear(ota_boot_param_t *param)
{
    int ret = OTA_UPGRADE_PARAM_FAIL;
    unsigned int offset = 0x00;
    unsigned int len = sizeof(ota_boot_param_t);
    if(param == NULL) {
        return ret;
    }
    ota_is_header = 0;
    ret = ota_hal_init(param);
    if(ret == 0) {
        if(ota_hash_ctx != NULL) {
            ota_free(ota_hash_ctx);
            ota_hash_ctx = NULL;
        }
    }
    ret = hal_flash_erase(HAL_PARTITION_PARAMETER_1, offset, len);
    OTA_LOG_I("ota clear ret = %d\r\n", ret);
    return ret;
}

int ota_write(unsigned int *off, char *in_buf, unsigned int in_buf_len)
{
    int ret = 0;
    if((*off == 0)&&(ota_is_header == 0)) {
        ota_header_check(in_buf, in_buf_len);
        ota_is_header = 1;
    }
    ret = ota_hal_write(off, in_buf, in_buf_len);
    if(ret >= 0) {
        ota_crc16_update(&ctx, in_buf, in_buf_len);
        ret = ota_hash_update((const unsigned char *)in_buf, in_buf_len, ota_hash_ctx);
        if(ret != 0) {
            ota_free(ota_hash_ctx);
            ota_hash_ctx = NULL;
        }
    }
    return ret;
}

int ota_read(unsigned int *off, char *out_buf, unsigned int out_buf_len)
{
    int ret = 0;
    ret = ota_hal_read(off, out_buf, out_buf_len);
    return ret;
}

int ota_set_boot(ota_boot_param_t *param)
{
    int ret = OTA_UPGRADE_SET_BOOT_FAIL;
    unsigned char hash_value[32] = {0};
    char calculate_hash_str_value[65] = {0};
    if (param != NULL) {
        if (param->upg_status == OTA_FINISH) {
            ota_is_header = 0;
            memset(hash_value, 0x00, sizeof(hash_value)); 
            ret = ota_hash_final(hash_value, ota_hash_ctx);
            ota_free(ota_hash_ctx);
            ota_hash_ctx = NULL;
            if(ret == 0) {
                ret = ota_hex2str(calculate_hash_str_value, hash_value, sizeof(calculate_hash_str_value), sizeof(hash_value));
                if(ret == 0) {
                    ret = ota_check_hash(param->hash_type, (char *)calculate_hash_str_value, param->hash);
                    if(ret == 0) {
                        ota_crc16_final(&ctx, &param->crc);
                        ret = ota_hal_boot(param);
                    }
                    else {
                        OTA_LOG_E("ota verify hash failed!");
                    }
                }
            }
        }
        OTA_LOG_I("ota boot finish ret:%d crc:0x%04x ", ret, param->crc);
    }
    ota_reboot();
    return ret;
}

int ota_rollback(void)
{
    return ota_hal_rollback();
}

const char *ota_version_get(unsigned char dev_type, char* dn)
{
    return ota_hal_version(dev_type, dn);
}

int ota_is_download_mode(void)
{
    int ret = 0;
    ota_boot_param_t param = {0};
    memset(&param, 0, sizeof(ota_boot_param_t));
    ret = ota_read_parameter(&param);
    if(ret < 0) {
         return 0;
    }
    if((param.upg_status == OTA_TRANSPORT) && (param.upg_flag != 0xffff)){
         OTA_LOG_I("OTA running status :0x%04x \n",param.upg_status);
         return 1;
    }
    OTA_LOG_I("ota is status:%d  param crc:0x%04x \n",param.upg_status, param.param_crc);
    return ret;
}

int ota_read_parameter(ota_boot_param_t *ota_param)
{
    int ret = OTA_UPGRADE_PARAM_FAIL;
    int param_part = HAL_PARTITION_PARAMETER_1;
    unsigned int offset = 0;
    unsigned short patch_crc = 0;
    if(ota_param == NULL) {
        return ret;
    }
    memset(ota_param, 0, sizeof(ota_boot_param_t));
    ret = hal_flash_read(param_part, &offset, ota_param, sizeof(ota_boot_param_t));
    if(ret < 0) {
        return ret;
    }
    patch_crc = ota_get_data_crc16((const unsigned char *)ota_param, sizeof(ota_boot_param_t) - sizeof(unsigned short));
    OTA_LOG_I("ota param crc:0x%04x cal:0x%04x \n", ota_param->param_crc, patch_crc);
    if(patch_crc == ota_param->param_crc) {
        return 0;
    }
    return ret;
}

int ota_update_parameter(ota_boot_param_t *ota_param)
{
    int ret = OTA_UPGRADE_PARAM_FAIL;
    if(ota_param == NULL) {
        return ret;
    }
    ota_param->param_crc = ota_get_data_crc16((const unsigned char *)ota_param, sizeof(ota_boot_param_t) - sizeof(unsigned short));
    OTA_LOG_I("ota update param crc:0x%04x flag:0x%04x \n", ota_param->param_crc, ota_param->upg_flag);
    ret = hal_flash_dis_secure(HAL_PARTITION_PARAMETER_1, 0, 0);
    if(ret >= 0) {
        unsigned int offset = 0x00;
        unsigned int len = sizeof(ota_boot_param_t);
        ota_boot_param_t comp_buf;
        memset(&comp_buf, 0, len);
	ret = hal_flash_erase(HAL_PARTITION_PARAMETER_1, offset, len);
	if(ret >= 0) {
            ret = hal_flash_write(HAL_PARTITION_PARAMETER_1, &offset, ota_param, len);
            offset = 0x00;
            if(ret >= 0) {
                 ret = hal_flash_read(HAL_PARTITION_PARAMETER_1, &offset, (unsigned char*)&comp_buf, len);
                 if(ret >= 0) {
                     if(memcmp(ota_param, (unsigned char*)&comp_buf, len) != 0) {
                         ret = OTA_UPGRADE_PARAM_FAIL;
                     }
                 }
            }
        }
    }
    return 0;
}

