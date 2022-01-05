#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "ota_log.h"
#include "ota_import.h"
#include "ota_hal_os.h"
#include "aos_hal_flash.h"

static int boot_part               = HAL_PARTITION_OTA_TEMP;

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
    ret = aos_hal_flash_read(param_part, &offset, ota_param, sizeof(ota_boot_param_t));
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
    unsigned int offset = 0x00;
    unsigned int len = sizeof(ota_boot_param_t);
    ota_boot_param_t comp_buf;
    if(ota_param == NULL) {
        return ret;
    }
    ota_param->param_crc = ota_get_data_crc16((const unsigned char *)ota_param, sizeof(ota_boot_param_t) - sizeof(unsigned short));
    OTA_LOG_I("ota update param crc:0x%04x flag:0x%04x\n", ota_param->param_crc, ota_param->upg_flag);
    memset(&comp_buf, 0, len);
    ret = aos_hal_flash_erase(HAL_PARTITION_PARAMETER_1, offset, len);
    if(ret >= 0) {
        ret = aos_hal_flash_write(HAL_PARTITION_PARAMETER_1, &offset, ota_param, len);
        offset = 0x00;
        if(ret >= 0) {
            ret = aos_hal_flash_read(HAL_PARTITION_PARAMETER_1, &offset, (unsigned char *)&comp_buf, len);
            if(ret >= 0) {
                 if(memcmp(ota_param, (unsigned char*)&comp_buf, len) != 0) {
                     ret = OTA_UPGRADE_PARAM_FAIL;
                     OTA_LOG_E("save param failed\r\n");
                 }
            }
        }
    }
    return 0;
}

unsigned short ota_get_upgrade_flag(void)
{
    int ret = 0;
    unsigned short flag = 0;
    ota_boot_param_t ota_param = {0};
    ret = ota_read_parameter(&ota_param);
    if(ret < 0) {
        return 0xffff;
    }
    flag = ota_param.upg_flag;
    return flag;
}

int ota_update_upg_flag(unsigned short flag)
{
    int ret = 0;
    ota_boot_param_t ota_param = {0};
    OTA_LOG_I("upg_flag:0x%x \n", flag);
    ret = ota_read_parameter(&ota_param);
    if(ret < 0) {
        return ret;
    }
    ota_param.upg_flag = flag;
    ret = ota_update_parameter(&ota_param);
    return 0;
}

int ota_clear_paramters()
{
    int ret = 0;
    unsigned int offset = 0x00;
    unsigned int len = sizeof(ota_boot_param_t);
    ret = aos_hal_flash_erase(HAL_PARTITION_PARAMETER_1, offset, len);
    OTA_LOG_I("ota clear ret = %d\r\n", ret);
    return ret;
}

unsigned int ota_parse_ota_type(unsigned char *buf, unsigned int len)
{
    int upg_flag = 0;
    unsigned char xz_header[6] = { 0xFD, '7', 'z', 'X', 'Z', 0x00 };
    if((buf != NULL) && (len >= 8)) {
        if (memcmp(buf, "BSDIFF40", 8) == 0) {
            upg_flag = OTA_UPGRADE_DIFF;
        }
        else if (memcmp(buf, xz_header, 6) == 0) {
            upg_flag = OTA_UPGRADE_XZ;
        }
        else {
            upg_flag = OTA_UPGRADE_ALL;
        }
    }
    OTA_LOG_I("ota header:0x%x", upg_flag);
    return upg_flag;
}

int ota_hal_rollback(void)
{
    int ret = 0;
    unsigned int offset = 0;
    ota_boot_param_t param;
    memset(&param, 0, sizeof(ota_boot_param_t));
    ret = aos_hal_flash_read(HAL_PARTITION_PARAMETER_1, &offset, &param, sizeof(ota_boot_param_t));
    if (ret < 0) {
        OTA_LOG_E("rollback err:%d", ret);
        return ret;
    }
    if ((param.boot_count != 0) && (param.boot_count != 0xff)) {
        param.upg_flag = 0;
        param.boot_count = 0; /*Clear bootcount to avoid rollback*/
        ret = ota_update_parameter(&param);
    }
    if (ret != 0) {
        OTA_LOG_E("rollback err:%d", ret);
    }
    return ret;
}

int ota_hal_boot(ota_boot_param_t *param)
{
    int ret = OTA_UPGRADE_WRITE_FAIL;
    hal_logic_partition_t  ota_info;
    hal_logic_partition_t  app_info;
    hal_logic_partition_t *p_ota_info = &ota_info;
    hal_logic_partition_t *p_app_info = &app_info;
    memset(p_ota_info, 0, sizeof(hal_logic_partition_t));
    memset(p_app_info, 0, sizeof(hal_logic_partition_t));
    aos_hal_flash_info_get(boot_part, &p_ota_info);
    aos_hal_flash_info_get(HAL_PARTITION_APPLICATION, &p_app_info);
    if (param != NULL) {
        if (param->crc == 0 || param->crc == 0xffff) {
            OTA_LOG_I("calculate image crc");
            ota_hal_image_crc16(&param->crc);
        }
        param->src_adr = p_ota_info->partition_start_addr;
        param->dst_adr = p_app_info->partition_start_addr;
        param->old_size = p_app_info->partition_length;
        param->boot_type = ota_hal_boot_type();
        ret = ota_update_parameter(param);
        if (ret < 0) {
            return OTA_UPGRADE_WRITE_FAIL;
        }
        OTA_LOG_I("OTA after finish dst:0x%08x src:0x%08x len:0x%08x, crc:0x%04x param crc:0x%04x upg_flag:0x%04x \r\n", param->dst_adr, param->src_adr, param->len, param->crc, param->param_crc, param->upg_flag);
    }
    return ret;
}
