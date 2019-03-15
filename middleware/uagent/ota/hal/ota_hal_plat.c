#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <errno.h>

#include "aos/kernel.h"
#include "aos/kv.h"

#include "aos/hal/flash.h"

#include "ota_hal_plat.h"
#include "ota_hal_os.h"
#include "ota_log.h"

#if defined (AOS_OTA_RECOVERY_TYPE)
#include "rec_define.h"
#endif

#if defined (BOARD_ESP8266)
#include "esp_system.h"
#include "upgrade.h"
#elif defined (STM32L496xx)
#define OTA_CACHE_SIZE       2048
uint8_t *ota_cache = NULL;
uint8_t *ota_cache_actual = NULL;
uint32_t ota_cache_len = 0;
uint32_t ota_fw_size = 0;
uint32_t ota_receive_total_len = 0;
#endif

#define OTA_CRC16  "ota_file_crc16"
static int boot_part = HAL_PARTITION_OTA_TEMP;
static unsigned int _offset = 0;
static ota_crc16_ctx ctx = {0};
const char ota_board_string[] = "board "SYSINFO_PRODUCT_MODEL;  /*don't delete it, used for ota diff recovery*/

unsigned short ota_get_crc16(void)
{
    int len = 2;
    unsigned short crc16=0;
    aos_kv_get(OTA_CRC16, &crc16, &len);
    return crc16;
}

void  ota_set_crc16(unsigned short crc16)
{
    aos_kv_set(OTA_CRC16, &crc16, 2, 1);
}

void ota_reboot_bank(void)
{
#if defined (BOARD_ESP8266)
    system_upgrade_init();
    system_upgrade_flag_set(UPGRADE_FLAG_FINISH);
    system_upgrade_reboot();
#elif defined (STM32L496xx)
    extern int flash_sw_bank(void);
    flash_sw_bank();
#endif
}

static int ota_init(void *something)
{
    int ret = 0;
    ota_boot_param_t *param = (ota_boot_param_t *)something;
    _offset = param->off_bp;
    hal_logic_partition_t *part_info = hal_flash_get_info(boot_part);
    if(part_info->partition_length < param->len || param->len == 0) {
        ret = OTA_PARAM_FAIL;
        return ret;
    }
#if defined (STM32L496xx)
    ota_fw_size = param->len;
    ota_receive_total_len = _offset;
    ota_cache = ota_malloc(OTA_CACHE_SIZE + 8);
    if (ota_cache != NULL) {
        /*Align with 8 bytes*/
        ota_cache_actual = (uint8_t *)((uint32_t)ota_cache & ~0x7);
    }
    else {
        ret = OTA_INIT_FAIL;
        return ret;
    }
#endif
    if(param->off_bp == 0) {
        int ret = 0;
        ret = hal_flash_erase(boot_part, _offset, part_info->partition_length);
        if(ret != 0) {
            ret = OTA_INIT_FAIL;
            return ret;
        }
        ota_crc16_init(&ctx);
    }
    else {
        ctx.crc=ota_get_crc16();
    }
    OTA_LOG_I("ota init off:0x%08x part:%d len:%d crc:0x%04x\n", param->off_bp, boot_part, param->len, ctx.crc);
    OTA_LOG_I("ota init %s success!\n", ota_board_string);
    return ret;
}

static int ota_write(int* off, char* in_buf, int in_buf_len)
{
    int ret = 0;
#if defined (STM32L496xx)
    uint32_t tocopy = 0;
    if(in_buf_len > OTA_CACHE_SIZE) {
        ret = OTA_UPGRADE_FAIL;
        return ret;
    }
    ota_crc16_update(&ctx, in_buf, in_buf_len);
    if (in_buf_len <= OTA_CACHE_SIZE - ota_cache_len) {
        tocopy = in_buf_len;
    }
    else {
        tocopy = OTA_CACHE_SIZE - ota_cache_len;
    }

    /*Start from last byte of remaing data*/
    memcpy(ota_cache_actual + ota_cache_len, in_buf, tocopy);
    ota_cache_len += tocopy;

    if (ota_cache_len == OTA_CACHE_SIZE) {
        ret = hal_flash_write(HAL_PARTITION_OTA_TEMP, &_offset, ota_cache_actual, OTA_CACHE_SIZE);
    }

    /*keep remaining data*/
    if (in_buf_len - tocopy > 0) {
        /*Now ota_cache only contains remaing data*/
        memcpy(ota_cache_actual, in_buf + tocopy, in_buf_len - tocopy);
        ota_cache_len = in_buf_len - tocopy;
    }

    ota_receive_total_len += in_buf_len;
    if(ota_receive_total_len == ota_fw_size) {
        if (ota_cache_len != 0) {
            int ret = hal_flash_write(HAL_PARTITION_OTA_TEMP, &_offset, ota_cache_actual, ota_cache_len);
            ota_free(ota_cache);
        }
    }
#else
    ota_crc16_update(&ctx, in_buf, in_buf_len);
    ret = hal_flash_write(boot_part, (uint32_t*)&_offset, in_buf, in_buf_len);
#endif
    return ret;
}

static int ota_read(int* off, char* out_buf, int out_buf_len)
{
    return hal_flash_read(boot_part, (uint32_t*)off, out_buf, out_buf_len);
}

static int ota_boot(void *something)
{
    int ret = 0;
    ota_boot_param_t *param = (ota_boot_param_t *)something;
    if (param == NULL) {
        ret = OTA_REBOOT_FAIL;
        return ret;
    }
    if (param->res_type == OTA_FINISH) {
        ota_crc16_final(&ctx, &param->crc);
        if (param->upg_flag == OTA_DIFF) {
#if defined (AOS_OTA_RECOVERY_TYPE)
            int offset = 0x00;
            int param_part = HAL_PARTITION_PARAMETER_1;
            PatchStatus ota_param,ota_param_r;
            #if !defined (BOARD_ESP8266)
            if(param->splict_size != SPLICT_SIZE) {
                ret = OTA_REBOOT_FAIL;
                OTA_LOG_E("diff splict size %u error, correct is %u\n", param->splict_size, SPLICT_SIZE);
                return ret;
            }
            #endif
            memset(&ota_param, 0, sizeof(PatchStatus));
            ota_param.dst_adr = HAL_PARTITION_APPLICATION;
            ota_param.src_adr = HAL_PARTITION_OTA_TEMP;
            ota_param.len = 0;
            ota_param.crc = param->crc;
            ota_param.splict_size = param->splict_size;
            ota_param.rec_size = param->rec_size;
            ota_param.diff = 1;
            ota_param.upg_flag = REC_RECOVERY_FLAG;
            ota_crc16_ctx ctx1;
            unsigned short crc;
            ota_crc16_init(&ctx1);
            ota_crc16_update(&ctx1, &ota_param, sizeof(PatchStatus) - sizeof(unsigned short));
            ota_crc16_final(&ctx1, &crc);
            ota_param.patch_crc = crc;
            offset = 0x00;
            hal_flash_erase(param_part, offset, sizeof(PatchStatus));
            offset = 0x00;
            hal_flash_write(param_part, (uint32_t*)&offset, &ota_param, sizeof(PatchStatus));
            offset = 0x00;
            memset(&ota_param_r, 0, sizeof(PatchStatus));
            hal_flash_read(param_part, (uint32_t*)&offset, &ota_param_r, sizeof(PatchStatus));
            if(memcmp(&ota_param, &ota_param_r, sizeof(PatchStatus)) != 0) {
                 ret = OTA_REBOOT_FAIL;
                 return ret;
            }
            OTA_LOG_I("diff dst:0x%08x src:0x%08x len:0x%08x, crc:0x%04x pcrc:0x%04x splict:%d.\r\n",
                        ota_param_r.dst_adr, ota_param_r.src_adr, ota_param_r.rec_size, ota_param_r.crc,
                        ota_param_r.patch_crc, ota_param_r.splict_size);
#endif
        }
        else {
#ifdef AOS_OTA_BANK_SINGLE
            int offset = 0x00;
            ota_crc16_ctx ctx1;
            unsigned short crc;
            int param_part = HAL_PARTITION_PARAMETER_1;
            extern int app_download_addr;
            extern int kernel_download_addr;
            hal_logic_partition_t *part_info = hal_flash_get_info(boot_part);
#ifndef AOS_OTA_2BOOT_UPDATE_SUPPORT
            param->src_adr = part_info->partition_start_addr;
            param->dst_adr = (param->upg_flag == OTA_APP) ? (int)&app_download_addr : (int)&kernel_download_addr;
#else
            param->src_adr  = 0;
            param->dst_adr  = 0;
            param->len      = 0;
            param->upg_flag = REC_SWAP_UPDATE_FLAG;
#endif
            ota_crc16_init(&ctx1);
            ota_crc16_update(&ctx1, param, sizeof(ota_boot_param_t) - sizeof(unsigned short));
            ota_crc16_final(&ctx1, &crc);
            param->param_crc = crc;
            ota_boot_param_t param_r;
            offset = 0x00;
            hal_flash_erase(param_part, offset, sizeof(ota_boot_param_t)); //PARTITION_BACKUP_PARAM
            offset = 0x00;
            hal_flash_write(param_part, (uint32_t*)&offset, param, sizeof(ota_boot_param_t));
            offset = 0x00;
            memset(&param_r, 0, sizeof(ota_boot_param_t));
            hal_flash_read(param_part, (uint32_t*)&offset, &param_r, sizeof(ota_boot_param_t));
            if(memcmp(param, &param_r, sizeof(ota_boot_param_t)) != 0) {
                 ret = OTA_REBOOT_FAIL;
                 return ret;
            }
            OTA_LOG_I("OTA finish dst:0x%08x src:0x%08x len:0x%08x, crc:0x%04x.\r\n", param_r.dst_adr, param_r.src_adr, param_r.len, param_r.crc);
#elif defined  AOS_OTA_BANK_DUAL
            int offset = 0x00;
            ota_crc16_ctx tmp_ctx;
            unsigned short crc;
            int param_part = HAL_PARTITION_PARAMETER_1;
            param->src_adr = 0x00;
            param->dst_adr = 0x00;
#if defined (AOS_OTA_RECOVERY_TYPE)
            param->upg_flag = REC_DUAL_UPDATE_FLAG;
#else
            param->upg_flag = 0x00;
#endif
            ota_crc16_init(&tmp_ctx);
            ota_crc16_update(&tmp_ctx, param, sizeof(ota_boot_param_t) - sizeof(unsigned short));
            ota_crc16_final(&tmp_ctx, &crc);
            param->param_crc = crc;
            ota_boot_param_t param_r;
            offset = 0x00;
            hal_flash_erase(param_part, offset, sizeof(ota_boot_param_t));
            offset = 0x00;
            hal_flash_write(param_part, (uint32_t*)&offset, param, sizeof(ota_boot_param_t));
            offset = 0x00;
            memset(&param_r, 0, sizeof(ota_boot_param_t));
            hal_flash_read(param_part, (uint32_t*)&offset, &param_r, sizeof(ota_boot_param_t));
            if(memcmp(param, &param_r, sizeof(ota_boot_param_t)) != 0) {
                 return -1;
            }
            OTA_LOG_I("OTA finish dst:0x%08x src:0x%08x len:0x%08x, crc:0x%04x.\r\n", param_r.dst_adr, param_r.src_adr, param_r.len, param_r.crc);
            ota_reboot_bank();
#endif
        }
        ota_reboot();
    }
    else if(param->res_type == OTA_BREAKPOINT) {
        OTA_LOG_I("save bp crc:0x%04x", ctx.crc);
#if defined (STM32L496xx)
        if (ota_cache_len != 0) {
            int ret = hal_flash_write(HAL_PARTITION_OTA_TEMP, &_offset, ota_cache_actual, ota_cache_len);
            ota_free(ota_cache);
        }
#endif
        ota_set_crc16(ctx.crc);
    }
    return ret;
}

static int ota_rollback(void *something)
{
    int offset = 0x00;
    int param_part = HAL_PARTITION_PARAMETER_1;
    ota_boot_param_t param_w, param_r;
    memset(&param_w, 0, sizeof(ota_boot_param_t));
    hal_flash_read(param_part, (uint32_t*)&offset, &param_w, sizeof(ota_boot_param_t));
    if((param_w.boot_count != 0) && (param_w.boot_count != 0xff)) {
        ota_crc16_ctx ctx1;
        unsigned short crc;
        param_w.upg_flag = 0;
        param_w.boot_count = 0; /*Clear bootcount to avoid rollback*/
        ota_crc16_init(&ctx1);
        ota_crc16_update(&ctx1, &param_w, sizeof(ota_boot_param_t) - sizeof(unsigned short));
        ota_crc16_final(&ctx1, &crc);
        param_w.param_crc = crc;
        offset = 0x00;
        hal_flash_erase(param_part, offset, sizeof(ota_boot_param_t));
        offset = 0x00;
        hal_flash_write(param_part, (uint32_t*)&offset, &param_w, sizeof(ota_boot_param_t));
        offset = 0x00;
        memset(&param_r, 0, sizeof(ota_boot_param_t));
        hal_flash_read(param_part, (uint32_t*)&offset, &param_r, sizeof(ota_boot_param_t));
        if(memcmp(&param_w, &param_r, sizeof(ota_boot_param_t)) != 0) {
            OTA_LOG_E("rollback failed\n");
            return -1;
        }
    }
    return 0;
}

static const char *ota_get_version(unsigned char dev_type)
{
    if(dev_type) {
        return "v1.0.0-20180101-1000";//SYSINFO_APP_VERSION;
    } else {
        return SYSINFO_APP_VERSION;
    }
}

ota_hal_module_t ota_hal_module = {
    .init     = ota_init,
    .write    = ota_write,
    .read     = ota_read,
    .boot     = ota_boot,
    .rollback = ota_rollback,
    .version  = ota_get_version,
};
