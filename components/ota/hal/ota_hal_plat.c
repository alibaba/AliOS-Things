#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "ota_log.h"
#include "ota_import.h"
#include "ota_hal_os.h"
#include "ota_hal.h"
#include "aos_hal_flash.h"

unsigned char *ota_cache           = NULL;
unsigned int ota_cache_len         = 0;
unsigned int ota_fw_size           = 0;
unsigned int ota_receive_total_len = 0;
static int boot_part               = HAL_PARTITION_OTA_TEMP;
static ota_crc16_ctx  ctx          = {0};

OTA_WEAK int ota_hal_init(ota_boot_param_t *param)
{   
    int ret = OTA_INIT_FAIL;
    if(param != NULL) {
        ret = 0;
        hal_logic_partition_t  part_info;
        hal_logic_partition_t *p_part_info;
        p_part_info = &part_info;
        memset(p_part_info, 0, sizeof(hal_logic_partition_t));
        ret = aos_hal_flash_info_get(boot_part, p_part_info);
        if(ret != 0 || param->len == 0) {
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
                unsigned int len = p_part_info->partition_length;
                unsigned int off = 0;
                unsigned int block_size = 0;
                while(len > 0) {
                    block_size = (len > OTA_FLASH_BLOCK_SIZE)? OTA_FLASH_BLOCK_SIZE: len; 
                    ret = aos_hal_flash_erase(boot_part, ota_receive_total_len + off, block_size);
                    if(ret < 0) {
                        ret = OTA_INIT_FAIL;
                        if(NULL != ota_cache) {
                            ota_free(ota_cache);
                            ota_cache = NULL;
                        }
                        OTA_LOG_E("erase fail! ");
                        return ret;
                    }
                    off += block_size;
                    len -= block_size;
                    ota_msleep(10);
                }
            }
            ota_crc16_init(&ctx);
        }
        OTA_LOG_I("ota init part:%d len:%d \n", boot_part, param->len);
    }
    if(ret != 0) {
        if(NULL != ota_cache) {
            ota_free(ota_cache);
            ota_cache = NULL;
        }
        OTA_LOG_E("ota init fail!");
    }
    return ret;
}

OTA_WEAK int ota_hal_write(unsigned int *off, char *in_buf, unsigned int in_buf_len)
{
    int ret = 0;
    int tocopy = 0;
    if(off == NULL || in_buf_len > OTA_FLASH_WRITE_CACHE_SIZE) {
        return OTA_UPGRADE_WRITE_FAIL;
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
        ret = aos_hal_flash_write(boot_part, off, ota_cache, OTA_FLASH_WRITE_CACHE_SIZE);
        if(ret < 0) {
            goto EXIT;
        }
        ota_crc16_update(&ctx, ota_cache, OTA_FLASH_WRITE_CACHE_SIZE);
        ota_cache_len = 0;
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
            ret = aos_hal_flash_write(boot_part, off, ota_cache, ota_cache_len);
            ota_crc16_update(&ctx, ota_cache, ota_cache_len);
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

OTA_WEAK int ota_hal_rollback(void)
{
    int ret = 0;
    unsigned int offset = 0;
    ota_boot_param_t param;
    memset(&param, 0, sizeof(ota_boot_param_t));
    ret = aos_hal_flash_read(HAL_PARTITION_PARAMETER_1, &offset, &param, sizeof(ota_boot_param_t));
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

OTA_WEAK unsigned char ota_hal_boot_type()
{
    return 0;
}

OTA_WEAK void ota_hal_image_crc16(unsigned short *outResult)
{
    ota_crc16_final(&ctx, outResult);
}

OTA_WEAK int ota_hal_boot(ota_boot_param_t *param)
{
    int ret = OTA_UPGRADE_WRITE_FAIL;
    hal_logic_partition_t  ota_info;
    hal_logic_partition_t  app_info;
    hal_logic_partition_t *p_ota_info = &ota_info;
    hal_logic_partition_t *p_app_info = &app_info;
    memset(p_ota_info, 0, sizeof(hal_logic_partition_t));
    memset(p_app_info, 0, sizeof(hal_logic_partition_t));
    aos_hal_flash_info_get(boot_part, p_ota_info);
    aos_hal_flash_info_get(HAL_PARTITION_APPLICATION, p_app_info);
    if(param != NULL) {
        if(param->crc == 0 || param->crc == 0xffff) {
            OTA_LOG_I("calculate image crc");
            ota_hal_image_crc16(&param->crc);
        }
        param->src_adr = p_ota_info->partition_start_addr;
        param->dst_adr = p_app_info->partition_start_addr;
        param->old_size = p_app_info->partition_length;
        param->boot_type = ota_hal_boot_type();
        ret = ota_update_parameter(param);
        if(ret < 0) {
            return OTA_UPGRADE_WRITE_FAIL;
        }
        OTA_LOG_I("OTA after finish dst:0x%08x src:0x%08x len:0x%08x, crc:0x%04x param crc:0x%04x upg_flag:0x%04x \r\n", param->dst_adr, param->src_adr, param->len, param->crc, param->param_crc, param->upg_flag);
    }
    return ret;
}

int ota_hal_read(unsigned int *off, char *out_buf, unsigned int out_buf_len)
{
    int ret = 0;
    ret = aos_hal_flash_read(boot_part, off, out_buf, out_buf_len);
    return ret;
}

int ota_hal_reboot_bank(void)
{
    return 0;
}