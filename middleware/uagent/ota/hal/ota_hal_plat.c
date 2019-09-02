#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "aos/kv.h"
#include "ota_log.h"
#include "aos/kernel.h"
#include "ota_plat_ctrl.h"
#include "aos/hal/flash.h"
#include "ota_hal_common.h"
#include "ota/ota_service.h"

uint8_t  *ota_cache            = NULL;
uint32_t ota_cache_len         = 0;
uint32_t ota_fw_size           = 0;
uint32_t ota_receive_total_len = 0;
static int boot_part           = HAL_PARTITION_OTA_TEMP;

OTA_WEAK int ota_hal_init(ota_boot_param_t *param)
{
    int ret = OTA_INIT_FAIL;
    if(param != NULL) {
        ret = 0;
        hal_logic_partition_t  part_info;
        hal_logic_partition_t *p_part_info;
        p_part_info = &part_info;
        memset(p_part_info, 0, sizeof(hal_logic_partition_t));
        ret = hal_flash_info_get(boot_part, p_part_info);
        if(ret != 0 || p_part_info->partition_length < param->len || param->len == 0) {
            ret = OTA_INIT_FAIL;
        }
        else {
            ota_receive_total_len = 0;
            ota_fw_size = param->len;
            ota_cache = ota_malloc(OTA_FLASH_WRITE_CACHE_SIZE);
            if (NULL == ota_cache) {
                ret = OTA_INIT_FAIL;
            }
            else {
                ret = hal_flash_erase(boot_part, ota_receive_total_len, p_part_info->partition_length);
            }
        }
        OTA_LOG_I("ota init part:%d len:%d \n", boot_part, param->len);
    }
    if(ret != 0) {
        if(NULL != ota_cache) {
            ota_free(ota_cache);
            ota_cache = NULL;
        }
        OTA_LOG_E("ota init failed!");
    }
    return ret;
}

OTA_WEAK int ota_hal_write(unsigned int *off, char *in_buf, unsigned int in_buf_len)
{
    int ret = 0;
    int tocopy = 0;
    if(in_buf_len > OTA_FLASH_WRITE_CACHE_SIZE) {
        ret = OTA_UPGRADE_WRITE_FAIL;
        goto EXIT;
    }
    if (in_buf_len <= OTA_FLASH_WRITE_CACHE_SIZE - ota_cache_len) {
        tocopy = in_buf_len;
    } else {
        tocopy = OTA_FLASH_WRITE_CACHE_SIZE - ota_cache_len;
    }
    /*Start from last byte of remaing data*/
    memcpy(ota_cache + ota_cache_len, in_buf, tocopy);
    ota_cache_len += tocopy;
    if (ota_cache_len == OTA_FLASH_WRITE_CACHE_SIZE) {
        ret = hal_flash_write(boot_part, off, ota_cache, OTA_FLASH_WRITE_CACHE_SIZE);
        if(ret < 0) {
            goto EXIT;
        }
    }
    /*keep remaining data*/
    if (in_buf_len - tocopy > 0) {
        /*Now ota_cache only contains remaing data*/
        memcpy(ota_cache, in_buf + tocopy, in_buf_len - tocopy);
        ota_cache_len = in_buf_len - tocopy;
    }
    ota_receive_total_len += in_buf_len;
    if(ota_receive_total_len == ota_fw_size) {
        if (ota_cache_len != 0) {
            ret = hal_flash_write(boot_part, (uint32_t*)off, ota_cache, ota_cache_len);
        }
        if(ota_cache != NULL) {
            ota_free(ota_cache);
            ota_cache = NULL;
        }
    }
EXIT:
    if(ret < 0) {
        if(ota_cache != NULL) {
            ota_free(ota_cache);
            ota_cache = NULL;
        }
        OTA_LOG_E("ota_write err:%d", ret);
    }
    return ret;
}

OTA_WEAK int ota_hal_read(unsigned int *off, char *out_buf, unsigned int out_buf_len)
{
    int ret = 0;
    ret = hal_flash_read(boot_part, off, out_buf, out_buf_len);
    if(ret < 0){
        return OTA_UPGRADE_WRITE_FAIL;
    }
    return ret;
}

#ifndef OTA_DUBANK
OTA_WEAK int hal_reboot_bank(void)
{
    return 0;
}
#endif

OTA_WEAK int ota_hal_boot(ota_boot_param_t *param)
{
    int ret = OTA_UPGRADE_WRITE_FAIL;
    hal_logic_partition_t  ota_info;
    hal_logic_partition_t  app_info;
    hal_logic_partition_t *p_ota_info = &ota_info;
    hal_logic_partition_t *p_app_info = &app_info;
    memset(p_ota_info, 0, sizeof(hal_logic_partition_t));
    memset(p_app_info, 0, sizeof(hal_logic_partition_t));
    hal_flash_info_get(boot_part, p_ota_info);
    hal_flash_info_get(HAL_PARTITION_APPLICATION, p_app_info);
    if(param != NULL) {
       param->src_adr = p_ota_info->partition_start_addr;
       param->dst_adr = p_app_info->partition_start_addr;
       param->old_size = p_app_info->partition_length;
       ret = ota_update_parameter(param);
       if(ret < 0) {
           return OTA_UPGRADE_WRITE_FAIL;
       }
       OTA_LOG_I("OTA finish dst:0x%08x src:0x%08x len:0x%08x, crc:0x%04x param crc:0x%04x upg_flag:0x%04x \r\n", param->dst_adr, param->src_adr, param->len, param->crc, param->param_crc, param->upg_flag);
       if(param->upg_flag != OTA_UPGRADE_DIFF && param->upg_flag != OTA_UPGRADE_XZ)
           ret = hal_reboot_bank();
    }
    return ret;
}

OTA_WEAK int ota_hal_rollback(void)
{
    int ret = 0;
    unsigned int offset = 0;
    ota_boot_param_t param;
    memset(&param, 0, sizeof(ota_boot_param_t));
    ret = hal_flash_read(HAL_PARTITION_PARAMETER_1, &offset, &param, sizeof(ota_boot_param_t));
    if(ret < 0) {
        OTA_LOG_E("rollback err:%d", ret);
        return ret;
    }
    if((param.boot_count != 0) && (param.boot_count != 0xff)) {
        param.upg_flag = 0;
        param.boot_count = 0; /*Clear bootcount to avoid rollback*/
        ret = ota_update_parameter(&param);
    }
    if(ret != 0) {
        OTA_LOG_E("rollback err:%d", ret);
    }
    return ret;
}

OTA_WEAK const char *ota_hal_version(unsigned char dev_type, char* dn)
{
    if(dev_type > 0) {
        return "v1.0.0-20180101-1000";//SYSINFO_APP_VERSION;
    }
    else {
        return SYSINFO_APP_VERSION;
    }
}
