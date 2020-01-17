#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "ota_log.h"
#include "aos/kernel.h"
#include "aos/hal/flash.h"
#include "ota_import.h"
#include "ota_hal_os.h"
#include "ota/ota_hal.h"
#if defined OTA_CONFIG_MCU_CAN
#include "canopen/canopen.h"
#include "canopen/canopen_od.h"
#endif

unsigned char *ota_cache           = NULL;
unsigned int ota_cache_len         = 0;
unsigned int ota_fw_size           = 0;
unsigned int ota_receive_total_len = 0;
static int boot_part               = HAL_PARTITION_OTA_TEMP;
static ota_crc16_ctx  ctx          = {0};

#if defined OTA_CONFIG_BLE
static unsigned int   has_erase_page_numbs = 0;
static unsigned int   total_erase_page_numbs = 0;
extern unsigned int hal_flash_erase_sector_size(void);

unsigned int ota_ble_align_to_page(unsigned int val, unsigned int page_size)
{
    return ((val + page_size - 1) & ~(page_size - 1));
}

int ota_ble_breakpoint_process(unsigned int image_size, unsigned int *break_point)
{
    int ret = 0;
    unsigned int num_pages = 0;
    unsigned int erase_sector_size = 0;
    if(break_point == NULL) {
        return -1;
    }
    erase_sector_size = hal_flash_erase_sector_size();
    if(erase_sector_size == 0) {
        return -1;
    }
    if(image_size != *break_point) {
        *break_point = *break_point & ~(erase_sector_size - 1);
    }
    num_pages = ota_ble_align_to_page(image_size - *break_point, erase_sector_size) / erase_sector_size;
    if (num_pages > 0) {
        has_erase_page_numbs   = *break_point / erase_sector_size;
        total_erase_page_numbs = num_pages + has_erase_page_numbs;
        ret = hal_flash_erase(boot_part, *break_point, erase_sector_size);
        if (ret < 0) {
            OTA_LOG_E(" f-erase failed");
            ret = -1;
        }
        else {
            has_erase_page_numbs++;
        }
    }
    return ret;
}

OTA_WEAK int ota_ble_write(unsigned int *off, char *in_buf, int in_buf_len)
{
    int ret = -1;
    unsigned int erase_sector_size = 0;
    erase_sector_size = hal_flash_erase_sector_size();
    if(erase_sector_size != 0) {
        ret = 0;
        if((has_erase_page_numbs <= total_erase_page_numbs) &&
           (*off % erase_sector_size + in_buf_len >= erase_sector_size)) {
            ret = hal_flash_erase(boot_part, *off + in_buf_len, erase_sector_size);
            if(ret == 0) {
                has_erase_page_numbs++;
            }
        }
        if(ret == 0) {
            ret = hal_flash_write(boot_part, off, in_buf, in_buf_len);
        }
    }
    return ret;
}

OTA_WEAK int ota_ble_check_upgrade_result(void)
{
    int ret                 = -1;
    int ota_result          = -1;
    unsigned int offset     = 0;
    ota_boot_param_t param;
    memset(&param, 0, sizeof(ota_boot_param_t));
    ret = hal_flash_read(HAL_PARTITION_PARAMETER_1, &offset, &param, sizeof(ota_boot_param_t));
    if(ret >= 0) {
        if((param.boot_count != 0) && (param.boot_count != 0xff)) {
            const char *sys_ver = ota_hal_version(0, NULL);
            if(sys_ver != NULL) {
                ota_result = strncmp(param.ver, sys_ver, strlen(sys_ver));
            }
        }
    }
    return ota_result;
}
#endif

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
                    ret = hal_flash_erase(boot_part, ota_receive_total_len + off, block_size);
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
                if((param->upg_magic == OTA_BIN_MAGIC_MCU)&&(param->len > p_part_info->partition_length)) {
                    ret = ota_mcu_hal_init();
                    if(ret < 0) {
                        ret = OTA_INIT_FAIL;
                    }
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
        unsigned int offset = 0;
        hal_logic_partition_t  part_info = {0};
        hal_flash_info_get(boot_part, &part_info);
        if(*off > part_info.partition_length) {
            offset = *off - part_info.partition_length;
            ret = ota_mcu_hal_write(&offset, ota_cache, OTA_FLASH_WRITE_CACHE_SIZE);
            *off += OTA_FLASH_WRITE_CACHE_SIZE;
        } else {
            ret = hal_flash_write(boot_part, off, ota_cache, OTA_FLASH_WRITE_CACHE_SIZE);
        }
        if(*off <= part_info.partition_length)
            ota_crc16_update(&ctx, ota_cache, OTA_FLASH_WRITE_CACHE_SIZE);
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
            unsigned int offset = 0;
            hal_logic_partition_t  part_info = {0};
            hal_flash_info_get(boot_part, &part_info);
            if(*off > part_info.partition_length) {
                offset = *off - part_info.partition_length;
                ret = ota_mcu_hal_write(&offset, ota_cache, ota_cache_len);
                *off += OTA_FLASH_WRITE_CACHE_SIZE;
            } else {
                ret = hal_flash_write(boot_part, off, ota_cache, ota_cache_len);
            }
            if(*off <= part_info.partition_length)
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

OTA_WEAK int ota_hal_reboot_bank(void)
{
    return 0;
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
    hal_flash_info_get(boot_part, p_ota_info);
    hal_flash_info_get(HAL_PARTITION_APPLICATION, p_app_info);
    if(param != NULL) {
        ota_crc16_final(&ctx, &param->crc);
        param->src_adr = p_ota_info->partition_start_addr;
        param->len = (param->len < p_ota_info->partition_length)?param->len:p_ota_info->partition_length;
        param->dst_adr = p_app_info->partition_start_addr;
        param->old_size = p_app_info->partition_length;
        ret = ota_update_parameter(param);
        if(ret < 0) {
            return OTA_UPGRADE_WRITE_FAIL;
        }
        OTA_LOG_I("OTA finish dst:0x%08x src:0x%08x len:0x%08x, crc:0x%04x param crc:0x%04x upg_flag:0x%04x \r\n", param->dst_adr, param->src_adr, param->len, param->crc, param->param_crc, param->upg_flag);
    }
    return ret;
}

OTA_WEAK int ota_hal_read(unsigned int *off, char *out_buf, unsigned int out_buf_len)
{
    int ret = 0;
    unsigned int offset = 0;
    hal_logic_partition_t  part_info = {0};
    if(off == NULL) {
         return OTA_UPGRADE_WRITE_FAIL;
    }
    hal_flash_info_get(boot_part, &part_info);
    if(*off + out_buf_len > part_info.partition_length) {
         offset = *off - part_info.partition_length;
         ret = ota_mcu_hal_read(&offset, (unsigned char *)out_buf, out_buf_len);
         *off += out_buf_len;
    } else {
         ret = hal_flash_read(boot_part, off, out_buf, out_buf_len);
    }
    if(ret < 0){
        return OTA_VERIFY_IMAGE_FAIL;
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

OTA_WEAK int ota_mcu_hal_init(void)
{
    int ret = 0;
    hal_flash_dis_secure(0, 0, 0);//disable flash protect
    hal_logic_partition_t part_info = {0};
    hal_flash_info_get(HAL_PARTITION_OTA_SUB, &part_info);
    unsigned int len = part_info.partition_length;
    unsigned int off = 0;
    unsigned int block_size = 0;
    while(len > 0) {
         block_size = (len > OTA_FLASH_BLOCK_SIZE)? OTA_FLASH_BLOCK_SIZE: len;
         ret = hal_flash_erase(HAL_PARTITION_OTA_SUB, off, block_size);
         if(ret < 0) {
             ret = OTA_INIT_FAIL;
         }
         off += block_size;
         len -= block_size;
         ota_msleep(10);
    }
    OTA_LOG_I("ota mcu init.\n");
    return ret;
}

OTA_WEAK int ota_mcu_hal_read(unsigned int *off, unsigned char *buf, unsigned int len)
{
    int ret = OTA_MCU_UPGRADE_FAIL;
    if(buf == NULL || len <= 0) {
        return ret;
    }
    ret = hal_flash_read(HAL_PARTITION_OTA_SUB, off, buf, len);
    return ret;
}

OTA_WEAK int ota_mcu_hal_write(unsigned int* off, unsigned char* buf, unsigned int len)
{
    int ret = OTA_MCU_UPGRADE_FAIL;
    if(buf == NULL || len <= 0) {
        return ret;
    }
    ret = hal_flash_write(HAL_PARTITION_OTA_SUB, off, buf, len);
    return ret;
}

OTA_WEAK int ota_mcu_hal_version(void *device, char *version)
{
    int ret = OTA_MCU_UPGRADE_FAIL;
    if(version == NULL) {
        OTA_LOG_E("mcu version err.");
        return ret;
    }
#if defined OTA_CONFIG_MCU_CAN
    ret = co_sdo_read_od(device, CO_SW_VERSION_INDEX, CO_SW_VERSION_SUB_INDEX, version, OTA_VER_LEN);
    OTA_LOG_I("MCU version get:%s ret:%d ", version, ret);
#endif
    return ret;
}

OTA_WEAK int ota_mcu_hal_recv(void *device, unsigned char *buf, unsigned int len)
{
    int ret = OTA_MCU_UPGRADE_FAIL;
    if(buf == NULL || len <= 0) {
        return ret;
    }
#if defined OTA_CONFIG_MCU_CAN
    ret = co_upgrade_recv(device, len, buf, OTA_MCU_TIMEOUT);
    if (ret < 0) {
        OTA_LOG_I("recv data err:%d\n", ret);
        return OTA_MCU_UPGRADE_FAIL;
    }
    OTA_LOG_I("mcu recv:0x%02x \n", buf[0]);
#elif defined ATHOST_MQTT
    ret = atcmd_read((unsigned char*)buf, len);
#endif
    return ret;
}

OTA_WEAK int ota_mcu_hal_send(void *device, unsigned char* buf, unsigned int len)
{
    int ret = OTA_MCU_UPGRADE_FAIL;
    if(buf == NULL || len <= 0) {
        return ret;
    }
#if defined OTA_CONFIG_MCU_CAN
    ret = co_sdo_write_od(device, CO_UPGRADE_INDEX, CO_UPGRADE_SUB_INDEX, buf, len);
    if (ret < 0) {
        OTA_LOG_I("send data err:%d\n", ret);
        return OTA_MCU_UPGRADE_FAIL;
    }
    OTA_LOG_I("mcu send cmd:0x%02x \n", buf[4]);
#elif defined ATHOST_MQTT
    ret = atcmd_write(NULL, (unsigned char *)buf, 0, len, NULL);
#endif
    return ret;
}

OTA_WEAK int ota_mcu_upgrade_start(unsigned int size, char* ver, char* md5)
{
    unsigned int img_size = 0;
    int ret = OTA_MCU_UPGRADE_FAIL;
    hal_logic_partition_t part_info = {0};
    hal_flash_info_get(HAL_PARTITION_OTA_TEMP, &part_info);

    if(size <= 0 || ver == NULL || strlen(ver) >= 64) {
        OTA_LOG_I("mcu ota version.\n");
        return ret;
    }
    if(size > part_info.partition_length) {
        img_size = size - part_info.partition_length;
    } else {
        img_size = size;
    }
    OTA_LOG_I("mcu ota start: img size:%d  ver:%s \n", img_size, ver);
#if defined OTA_CONFIG_MCU_CAN
    ret = ota_mcu_can_upgrade(img_size, ver, md5);
#elif defined OTA_CONFIG_MCU_YMODEM
    ret = ota_mcu_ymodem_upgrade(img_size, ver, md5);
#endif
    return ret;
}
