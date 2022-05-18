#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <aos/flashpart.h>
#include <aos/hal/flash.h>
#include "ota_log.h"
#include "ota_hal.h"
#include "ota_import.h"
#include "ota_hal_os.h"

unsigned char *ota_cache            = NULL;
unsigned int ota_cache_len          = 0;
unsigned int ota_fw_size            = 0;
unsigned int ota_receive_total_len  = 0;
static uint32_t boot_part           = HAL_PARTITION_OTA_TEMP;
static ota_crc16_ctx ctx            = { 0, };
static aos_flashpart_ref_t ota_part = AOS_DEV_REF_INIT_VAL;

OTA_WEAK int ota_get_running_index(void)
{
    return 0;
}

OTA_WEAK int ota_hal_init(ota_boot_param_t *param)
{
    int ret = 0;
    size_t erase_len;
    uint32_t part_id;
    aos_flashpart_info_t part_info;
    aos_flash_info_t flash_info;

    if (param == NULL) {
        ret = OTA_INIT_FAIL;
        goto OTA_HAL_INIT_EXIT;
    }

    if (ota_get_running_index() == 0)
        part_id = HAL_PARTITION_OTA_TEMP;
    else
        part_id = HAL_PARTITION_APPLICATION;

    OTA_LOG_I("update partition %d\r\n", (int)part_id);

    if (aos_flashpart_get(&ota_part, part_id) || aos_flashpart_get_info(&ota_part, &part_info, &flash_info)) {
        OTA_LOG_E("open ota temp partition failed");
        ret = OTA_INIT_FAIL;
        goto OTA_HAL_INIT_EXIT;
    }

    if (param->len == 0 ||
        param->len > part_info.block_count * flash_info.pages_per_block * flash_info.page_data_size) {
        OTA_LOG_E("bin file size error!");
        ret = OTA_INIT_FAIL;
        goto OTA_HAL_INIT_EXIT;
    }

    ota_receive_total_len = 0;
    ota_fw_size = param->len;
    ota_cache = ota_malloc(OTA_FLASH_WRITE_CACHE_SIZE);
    if (NULL == ota_cache) {
        ret = OTA_INIT_FAIL;
        goto OTA_HAL_INIT_EXIT;
    }

    erase_len = param->len + flash_info.pages_per_block * flash_info.page_data_size - 1;
    erase_len /= flash_info.pages_per_block * flash_info.page_data_size;
    erase_len *= flash_info.pages_per_block * flash_info.page_data_size;
    if (aos_flashpart_erase(&ota_part, 0, erase_len)) {
        ret = OTA_INIT_FAIL;
        OTA_LOG_E("erase fail!");
        goto OTA_HAL_INIT_EXIT;
    }

    ota_crc16_init(&ctx);
    OTA_LOG_I("ota init part:%d len:%d\n", (int)part_id, param->len);

OTA_HAL_INIT_EXIT:
    if (ret != 0) {
        if (NULL != ota_cache) {
            ota_free(ota_cache);
            ota_cache = NULL;
        }
        if (aos_dev_ref_is_valid(&ota_part))
            aos_flashpart_put(&ota_part);
        OTA_LOG_E("ota init fail!");
    }

    return ret;
}

OTA_WEAK int ota_hal_final(void)
{
        return 0;
}

OTA_WEAK int ota_hal_write(unsigned int *off, char *in_buf, unsigned int in_buf_len)
{
    int ret = 0;
    int tocopy = 0;

    if (off == NULL || in_buf_len > OTA_FLASH_WRITE_CACHE_SIZE) {
        return OTA_UPGRADE_WRITE_FAIL;
    }
    if (in_buf_len <= OTA_FLASH_WRITE_CACHE_SIZE - ota_cache_len) {
        tocopy = in_buf_len;
    } else {
        tocopy = OTA_FLASH_WRITE_CACHE_SIZE - ota_cache_len;
    }
    /* Start from last byte of remaing data */
    memcpy(ota_cache + ota_cache_len, in_buf, tocopy);
    ota_cache_len += tocopy;
    if (ota_cache_len == OTA_FLASH_WRITE_CACHE_SIZE) {
        if (aos_flashpart_write(&ota_part, *off, ota_cache, OTA_FLASH_WRITE_CACHE_SIZE)) {
            ret = OTA_UPGRADE_WRITE_FAIL;
            goto EXIT;
        }
        *off += OTA_FLASH_WRITE_CACHE_SIZE;
        ota_cache_len = 0;
        ota_crc16_update(&ctx, ota_cache, OTA_FLASH_WRITE_CACHE_SIZE);
    }
    /* keep remaining data */
    if (in_buf_len - tocopy > 0) {
        /* Now ota_cache only contains remaining data */
        memcpy(ota_cache, in_buf + tocopy, in_buf_len - tocopy);
        ota_cache_len = in_buf_len - tocopy;
    }
    ota_receive_total_len += in_buf_len;
    if (ota_receive_total_len == ota_fw_size) {
        if (ota_cache_len != 0) {
            if (aos_flashpart_write(&ota_part, *off, ota_cache, ota_cache_len)) {
                ret = OTA_UPGRADE_WRITE_FAIL;
                goto EXIT;
            }
            *off += ota_cache_len;
            ota_crc16_update(&ctx, ota_cache, ota_cache_len);
        }
        if (ota_cache != NULL) {
            ota_free(ota_cache);
            ota_cache = NULL;
        }
        if (aos_dev_ref_is_valid(&ota_part))
            aos_flashpart_put(&ota_part);
    }

EXIT:
    if (ret < 0) {
        if (ota_cache != NULL) {
            ota_free(ota_cache);
            ota_cache = NULL;
        }
        if (aos_dev_ref_is_valid(&ota_part))
            aos_flashpart_put(&ota_part);
        OTA_LOG_E("ota_write err:%d", ret);
    }

    return ret;
}

OTA_WEAK int ota_hal_rollback_platform_hook(void)
{
    return 0;
}

OTA_WEAK int ota_hal_rollback(void)
{
    int ret = 0;
    ota_boot_param_t param;
    aos_flashpart_ref_t temp_part = AOS_DEV_REF_INIT_VAL;
    aos_status_t r;

    memset(&param, 0, sizeof(ota_boot_param_t));

    if (aos_flashpart_get(&temp_part, HAL_PARTITION_PARAMETER_1)) {
        OTA_LOG_E("open ota temp partition failed");
        ret = -1;
        goto OTA_ROLLBACK;
    }

    r = aos_flashpart_read(&temp_part, 0, &param, sizeof(ota_boot_param_t));
    if (r < 0 || r == AOS_FLASH_ECC_ERROR) {
        OTA_LOG_E("rollback err1:%d", (int)r);
        ret = -1;
        goto OTA_ROLLBACK;
    }

    if ((param.boot_count != 0) && (param.boot_count != 0xff)) {
        param.upg_flag = 0;
        param.boot_count = 0; /* clear bootcount to avoid rollback */
        ret = ota_update_parameter(&param);
        if (ret < 0) {
            OTA_LOG_E("rollback err2:%d", ret);
            goto OTA_ROLLBACK;
        }
        ret = ota_hal_rollback_platform_hook();
        if (ret < 0) {
            OTA_LOG_E("ota_hal_rollback_platform_hook fail!");
            goto OTA_ROLLBACK;
        }
    }

OTA_ROLLBACK:
    if (aos_dev_ref_is_valid(&temp_part))
        aos_flashpart_put(&temp_part);
    if (ret != 0)
        OTA_LOG_E("rollback err3:%d", ret);

    return ret;
}

OTA_WEAK int ota_hal_reboot_bank(void)
{
    return 0;
}

OTA_WEAK void ota_hal_image_crc16(unsigned short *outResult)
{
    ota_crc16_final(&ctx, outResult);
}

OTA_WEAK int ota_hal_platform_boot_type(void)
{
    return 0;
}

OTA_WEAK int ota_hal_boot_type()
{
    int ret = -1;
    ret = ota_hal_platform_boot_type();
    if (ret < 0) {
        OTA_LOG_E("get boot type  failed!");
    }
    return ret;
}

OTA_WEAK int ota_hal_boot(ota_boot_param_t *param)
{
    int ret = OTA_UPGRADE_WRITE_FAIL;
    uint32_t src_part_id;
    uint32_t dst_part_id;
    aos_flashpart_ref_t temp_part = AOS_DEV_REF_INIT_VAL;
    aos_flashpart_info_t part_info;
    aos_flash_info_t flash_info;

    if (ota_get_running_index() == 0) {
        src_part_id = HAL_PARTITION_OTA_TEMP;
        dst_part_id = HAL_PARTITION_APPLICATION;
    } else {
        src_part_id = HAL_PARTITION_APPLICATION;
        dst_part_id = HAL_PARTITION_OTA_TEMP;
    }

    if (aos_flashpart_get(&temp_part, src_part_id) || aos_flashpart_get_info(&temp_part, &part_info, &flash_info)) {
        if (aos_dev_ref_is_valid(&temp_part))
            aos_flashpart_put(&temp_part);
        goto OTA_HAL_BOOT_OVER;
    }

    aos_flashpart_put(&temp_part);
    param->src_adr = part_info.block_start * flash_info.pages_per_block * flash_info.page_data_size;

    if (aos_flashpart_get(&temp_part, dst_part_id) || aos_flashpart_get_info(&temp_part, &part_info, &flash_info)) {
        if (aos_dev_ref_is_valid(&temp_part))
            aos_flashpart_put(&temp_part);
        goto OTA_HAL_BOOT_OVER;
    }

    aos_flashpart_put(&temp_part);
    param->dst_adr = part_info.block_start * flash_info.pages_per_block * flash_info.page_data_size;
    param->old_size = part_info.block_count * flash_info.pages_per_block * flash_info.page_data_size;

    if (param->crc == 0 || param->crc == 0xffff) {
        OTA_LOG_I("calculate image crc");
        ota_hal_image_crc16(&param->crc);
    }
    if (param->upg_flag == OTA_UPGRADE_ALL) {
        int tmp_type = ota_hal_boot_type();
        if (tmp_type < 0) {
            OTA_LOG_E("get platform boot type err!");
            goto OTA_HAL_BOOT_OVER;
        }
        param->boot_type = (unsigned char)tmp_type;
    } else {
        param->boot_type = 0;
    }
    ret = ota_update_parameter(param);
    if (ret < 0) {
        OTA_LOG_E("update param err!");
        goto OTA_HAL_BOOT_OVER;
    }
    if (param->upg_flag == OTA_UPGRADE_ALL) {
        ret = ota_hal_final();
        if (ret < 0) {
            OTA_LOG_E("clear user boot count fail!");
            goto OTA_HAL_BOOT_OVER;
        }
    }
    OTA_LOG_I("OTA after finish dst:0x%08x src:0x%08x len:0x%08x, crc:0x%04x param crc:0x%04x upg_flag:0x%04x \r\n", param->dst_adr, param->src_adr, param->len, param->crc, param->param_crc, param->upg_flag);
OTA_HAL_BOOT_OVER:
    if (ret < 0) {
        OTA_LOG_E("set boot info failed!");
    }
    return ret;
}

OTA_WEAK int ota_hal_read(unsigned int *off, char *out_buf, unsigned int out_buf_len)
{
    int ret = 0;
    uint32_t part_id;
    aos_flashpart_ref_t temp_part = AOS_DEV_REF_INIT_VAL;
    aos_status_t r;

    if (ota_get_running_index() == 0)
        part_id = HAL_PARTITION_OTA_TEMP;
    else
        part_id = HAL_PARTITION_APPLICATION;

    boot_part = part_id;

    if (aos_flashpart_get(&temp_part, part_id)) {
        OTA_LOG_E("open ota temp partition failed");
        ret = -1;
        goto OTA_HAL_READ;
    }

    r = aos_flashpart_read(&temp_part, *off, out_buf, out_buf_len);
    if (r < 0 || r == AOS_FLASH_ECC_ERROR) {
        OTA_LOG_E("read failed:%d", (int)r);
        ret = -1;
        goto OTA_HAL_READ;
    }

OTA_HAL_READ:
    if (aos_dev_ref_is_valid(&temp_part))
        aos_flashpart_put(&temp_part);

    return ret;
}
