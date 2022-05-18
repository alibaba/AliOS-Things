#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <aos/flashpart.h>
#include <aos/hal/flash.h>
#include "ota_log.h"
#include "ota_import.h"
#include "ota_hal_os.h"

int ota_is_download_mode(void)
{
    int ret = 0;
    ota_boot_param_t param = { 0, };
    memset(&param, 0, sizeof(ota_boot_param_t));
    ret = ota_read_parameter(&param);
    if (ret < 0) {
        return 0;
    }
    if ((param.upg_status == OTA_TRANSPORT) && (param.upg_flag != 0xffff)) {
        OTA_LOG_I("OTA running status:0x%04x\n", param.upg_status);
        return 1;
    }
    OTA_LOG_I("ota is status:%d param crc:0x%04x\n", param.upg_status, param.param_crc);
    return ret;
}

int ota_read_parameter(ota_boot_param_t *ota_param)
{
    int ret = 0;
    unsigned short patch_crc = 0;
    aos_flashpart_ref_t part_ref = AOS_DEV_REF_INIT_VAL;
    aos_status_t r;

    if (ota_param == NULL) {
        ret = OTA_UPGRADE_PARAM_FAIL;
        goto OTA_READ_PARAM_OVER;
    }

    if (aos_flashpart_get(&part_ref, HAL_PARTITION_PARAMETER_1)) {
        OTA_LOG_E("open ota param1 partition failed");
        ret = OTA_UPGRADE_PARAM_FAIL;
        goto OTA_READ_PARAM_OVER;
    }

    memset(ota_param, 0, sizeof(ota_boot_param_t));
    r = aos_flashpart_read(&part_ref, 0, ota_param, sizeof(ota_boot_param_t));
    if (r < 0 || r == AOS_FLASH_ECC_ERROR) {
        OTA_LOG_E("read failed:%d", (int)r);
        ret = OTA_UPGRADE_PARAM_FAIL;
        goto OTA_READ_PARAM_OVER;
    }

    patch_crc = ota_get_data_crc16((const unsigned char *)ota_param, sizeof(ota_boot_param_t) - sizeof(unsigned short));
    OTA_LOG_I("ota param crc:0x%04x cal:0x%04x\n", ota_param->param_crc, patch_crc);
    if (patch_crc != ota_param->param_crc) {
        OTA_LOG_E("ota param crc err");
        ret = OTA_UPGRADE_PARAM_FAIL;
    }

OTA_READ_PARAM_OVER:
    if (aos_dev_ref_is_valid(&part_ref))
        aos_flashpart_put(&part_ref);

    return ret;
}

int ota_update_parameter(ota_boot_param_t *ota_param)
{
    int ret = 0;
    ota_boot_param_t comp_buf;
    size_t len = sizeof(ota_boot_param_t);
    size_t block_size;
    aos_flashpart_ref_t part_ref = AOS_DEV_REF_INIT_VAL;
    aos_flashpart_info_t part_info;
    aos_flash_info_t flash_info;
    aos_status_t r;

    if (ota_param == NULL) {
        ret = OTA_UPGRADE_PARAM_FAIL;
        goto OTA_UPGRADE_PARAN_OVER;
    }

    ota_param->param_crc = ota_get_data_crc16((const unsigned char *)ota_param,
                                              sizeof(ota_boot_param_t) - sizeof(unsigned short));
    OTA_LOG_I("ota update param crc:0x%04x flag:0x%04x boot_type:%d len = %d\n",
              ota_param->param_crc, ota_param->upg_flag, ota_param->boot_type, len);
    memset(&comp_buf, 0, len);

    if (aos_flashpart_get(&part_ref, HAL_PARTITION_PARAMETER_1) ||
        aos_flashpart_get_info(&part_ref, &part_info, &flash_info)) {
        OTA_LOG_E("open ota param1 partition failed");
        ret = OTA_UPGRADE_PARAM_FAIL;
        goto OTA_UPGRADE_PARAN_OVER;
    }

    block_size = flash_info.page_data_size * flash_info.pages_per_block;
    if (aos_flashpart_erase(&part_ref, 0, (len + block_size - 1) / block_size * block_size)) {
        OTA_LOG_E("erase fail!");
        ret = OTA_UPGRADE_PARAM_FAIL;
        goto OTA_UPGRADE_PARAN_OVER;
    }

    if (aos_flashpart_write(&part_ref, 0, ota_param, len)) {
        OTA_LOG_E("write fail!");
        ret = OTA_UPGRADE_PARAM_FAIL;
        goto OTA_UPGRADE_PARAN_OVER;
    }

    r = aos_flashpart_read(&part_ref, 0, &comp_buf, len);
    if (r < 0 || r == AOS_FLASH_ECC_ERROR) {
        OTA_LOG_E("read failed:%d", (int)r);
        ret = OTA_UPGRADE_PARAM_FAIL;
        goto OTA_UPGRADE_PARAN_OVER;
    }

    if (memcmp(ota_param, &comp_buf, len) != 0) {
        ret = OTA_UPGRADE_PARAM_FAIL;
        OTA_LOG_E("ota save param failed\r");
    }

OTA_UPGRADE_PARAN_OVER:
    if (aos_dev_ref_is_valid(&part_ref))
        aos_flashpart_put(&part_ref);
    OTA_LOG_I("ota update param over\n");

    return ret;
}

unsigned short ota_get_upgrade_flag(void)
{
    int ret = 0;
    unsigned short flag = 0;
    ota_boot_param_t ota_param = { 0, };
    ret = ota_read_parameter(&ota_param);
    if (ret < 0) {
        return 0xffff;
    }
    flag = ota_param.upg_flag;
    return flag;
}

int ota_update_upg_flag(unsigned short flag)
{
    int ret = 0;
    ota_boot_param_t ota_param = { 0, };
    OTA_LOG_I("upg_flag:0x%x\n", flag);
    ret = ota_read_parameter(&ota_param);
    if (ret < 0) {
        return ret;
    }
    ota_param.upg_flag = flag;
    ret = ota_update_parameter(&ota_param);
    return 0;
}

int ota_clear_paramters()
{
    int ret = 0;
    size_t block_size;
    aos_flashpart_ref_t part_ref = AOS_DEV_REF_INIT_VAL;
    aos_flashpart_info_t part_info;
    aos_flash_info_t flash_info;

    if (aos_flashpart_get(&part_ref, HAL_PARTITION_PARAMETER_1) ||
        aos_flashpart_get_info(&part_ref, &part_info, &flash_info)) {
        OTA_LOG_E("open ota param1 partition failed");
        ret = OTA_UPGRADE_PARAM_FAIL;
        goto OTA_CLEAR_PARAM_OVER;
    }

    block_size = flash_info.page_data_size * flash_info.pages_per_block;
    if (aos_flashpart_erase(&part_ref, 0, (sizeof(ota_boot_param_t) + block_size - 1) / block_size * block_size)) {
        OTA_LOG_E("erase fail! ");
        ret = OTA_UPGRADE_PARAM_FAIL;
        goto OTA_CLEAR_PARAM_OVER;
    }

OTA_CLEAR_PARAM_OVER:
    if (aos_dev_ref_is_valid(&part_ref))
        aos_flashpart_put(&part_ref);

    return ret;
}

unsigned int ota_parse_ota_type(unsigned char *buf, unsigned int len)
{
    int upg_flag = 0;
    unsigned char xz_header[6] = { 0xFD, '7', 'z', 'X', 'Z', 0x00, };
    if ((buf != NULL) && (len >= 8)) {
        if (memcmp(buf, "BSDIFF40", 8) == 0) {
            upg_flag = OTA_UPGRADE_DIFF;
        } else if (memcmp(buf, xz_header, 6) == 0) {
            upg_flag = OTA_UPGRADE_XZ;
        } else {
            upg_flag = OTA_UPGRADE_ALL;
        }
    }
    OTA_LOG_I("ota header:0x%x", upg_flag);
    return upg_flag;
}
