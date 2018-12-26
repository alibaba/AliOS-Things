/**
 ******************************************************************************
 * @file    ota_port.c
 * @author  MCU China FAE team
 * @version 1.0
 * @date    26/12/2018
 * @brief   aos porting layer
 ******************************************************************************
 *
 * COPYRIGHT(c) 2018 STMicroelectronics
 *
 ******************************************************************************
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <errno.h>
#include <hal/ota.h>
#include <aos/aos.h>
#include <hal/soc/soc.h>
#include <CheckSumUtils.h>

#define KV_HAL_OTA_CRC16 "hal_ota_get_crc16"


typedef struct
{
    uint32_t ota_len;
    uint32_t ota_crc;
} ota_reboot_info_t;

static ota_reboot_info_t ota_info;
static uint16_t hal_ota_get_crc16(void);
static void hal_ota_save_crc16(uint16_t crc16);
static int boot_table_update(boot_table_t *boot_table);
static int hal_ota_switch_to_new_fw(uint8_t parti, int ota_data_len, uint16_t ota_data_crc);

static CRC16_Context contex;
#define OTA_CACHE_SIZE 8192
uint32_t _off_set = 0;
uint8_t *ota_cache = NULL;
uint8_t *ota_cache_actual = NULL;
uint32_t ota_cache_len = 0;

static int ota_init(hal_ota_module_t *m, void *something)
{
    hal_logic_partition_t *partition_info;

    LOG("stm32f4xx ota init\n");
    _off_set = *(uint32_t *)something;
    ota_info.ota_len = _off_set;
    if (_off_set == 0)
    {
        partition_info = hal_flash_get_info(HAL_PARTITION_OTA_TEMP);
        hal_flash_erase(HAL_PARTITION_OTA_TEMP, 0, partition_info->partition_length);
        CRC16_Init(&contex);
    }
    else
    {
        contex.crc = hal_ota_get_crc16();
        LOG("--------get crc16 context.crc=%d!--------\n", contex.crc);
    }

    ota_cache = aos_malloc(OTA_CACHE_SIZE + 8);
    if (ota_cache != NULL)
    {
        /*Align with 8 bytes*/
        ota_cache_actual = (uint8_t *)((uint32_t)ota_cache & ~0x7);
    }

    return 0;
}

static int ota_write(hal_ota_module_t *m, volatile uint32_t *off_set, uint8_t *in_buf, uint32_t in_buf_len)
{
    hal_logic_partition_t *partition_info;
    uint32_t start_addr;
    int ret = 0;
    uint32_t tocopy = 0;

    if (ota_info.ota_len == 0)
    {
        _off_set = 0;
        CRC16_Init(&contex);
        memset(&ota_info, 0, sizeof ota_info);
    }
    CRC16_Update(&contex, in_buf, in_buf_len);

    if (in_buf_len <= OTA_CACHE_SIZE - ota_cache_len)
    {
        tocopy = in_buf_len;
    }
    else
    {
        tocopy = OTA_CACHE_SIZE - ota_cache_len;
    }

    /*Start from last byte of remaing data*/
    memcpy(ota_cache_actual + ota_cache_len, in_buf, tocopy);
    ota_cache_len += tocopy;

    if (ota_cache_len == OTA_CACHE_SIZE)
    {
        hal_logic_partition_t *partition_info;
        uint32_t start_addr;

        partition_info = hal_flash_get_info(HAL_PARTITION_OTA_TEMP);
        start_addr = partition_info->partition_start_addr + _off_set;

        ret = FLASH_write_at(start_addr, ota_cache_actual, OTA_CACHE_SIZE);
        if (ret == 0)
        {
            _off_set += OTA_CACHE_SIZE;
            LOG("Intend to wirte %d to %p OK\n", OTA_CACHE_SIZE, start_addr);
        }
        else
        {
            LOG("Intend to wirte %d to %p ERROR\n", OTA_CACHE_SIZE, start_addr);
        }
    }

    /*keep remaining data*/
    if (in_buf_len - tocopy > 0)
    {
        /*Now ota_cache only contains remaing data*/
        memcpy(ota_cache_actual, in_buf + tocopy, in_buf_len - tocopy);
        ota_cache_len = in_buf_len - tocopy;
    }

    ota_info.ota_len += in_buf_len;

    return ret;
}

static int ota_read(hal_ota_module_t *m, volatile uint32_t *off_set, uint8_t *out_buf, uint32_t out_buf_len)
{
    hal_flash_read(HAL_PARTITION_OTA_TEMP, off_set, out_buf, out_buf_len);
    return 0;
}

static int ota_set_boot(hal_ota_module_t *m, void *something)
{

    ota_finish_param_t *param = (ota_finish_param_t *)something;
    if (param == NULL)
    {
        return -1;
    }

    if (ota_cache_len != 0)
    {
        /*Flash unaligned data at last call*/
        hal_logic_partition_t *partition_info;
        uint32_t start_addr;

        partition_info = hal_flash_get_info(HAL_PARTITION_OTA_TEMP);
        start_addr = partition_info->partition_start_addr + _off_set;

        int ret = FLASH_write_at(start_addr, ota_cache_actual, ota_cache_len);
        if (ret == 0)
        {
            _off_set += ota_cache_len;
            LOG("Intend to wirte %d to %p OK\n", ota_cache_len, start_addr);
        }
        else
        {
            LOG("Intend to wirte %d to %p ERROR\n", ota_cache_len, start_addr);
        }
        aos_free(ota_cache);
    }
    if (param->result_type == OTA_FINISH)
    {
        CRC16_Final(&contex, &ota_info.ota_crc);
        LOG("moc108 set boot with length = %d\n", ota_info.ota_len);
        hal_ota_switch_to_new_fw(param->update_type, ota_info.ota_len, ota_info.ota_crc);
        memset(&ota_info, 0, sizeof ota_info);
    }
    else if (param->result_type == OTA_BREAKPOINT)
    {
        uint32_t save_env_addr;
        hal_logic_partition_t *partition_info;
        LOG("-------save breakpoint to flash- crc=%d--------------\n", contex.crc);
        partition_info = hal_flash_get_info(HAL_PARTITION_OTA_TEMP);
        hal_ota_save_crc16(contex.crc);
    }
    return 0;
}

struct hal_ota_module_s stm32f4xx_ota_module = {
    .init = ota_init,
    .ota_write = ota_write,
    .ota_read = ota_read,
    .ota_set_boot = ota_set_boot,
};

static uint16_t hal_ota_get_crc16(void)
{
    int len = 2;
    uint16_t crc16 = 0;
    aos_kv_get(KV_HAL_OTA_CRC16, &crc16, &len);
    return crc16;
}

static void hal_ota_save_crc16(uint16_t crc16)
{
    aos_kv_set(KV_HAL_OTA_CRC16, &crc16, 2, 1);
}

static int hal_ota_switch_to_new_fw(uint8_t parti, int ota_data_len, uint16_t ota_data_crc)
{
    int ret = 0;
    uint32_t para_len = hal_flash_get_info(HAL_PARTITION_PARAMETER_1)->partition_length;
    uint8_t *buf = aos_malloc(para_len + 8);
    uint8_t *actual = (uint8_t *)((uint32_t)buf & (~0x7));

    uint32_t src1, dst1, src2, dst2;
    uint32_t off = 0;

    /*Read and copy  user parameter*/
    if (FLASH_bank2_enabled())/*BFB2 == 1*/
    {
        src1 = HAL_PARTITION_PARAMETER_3;
        dst1 = HAL_PARTITION_PARAMETER_1;
        src2 = HAL_PARTITION_PARAMETER_4;
        dst2 = HAL_PARTITION_PARAMETER_2;
    }
    else
    {
        src1 = HAL_PARTITION_PARAMETER_1;
        dst1 = HAL_PARTITION_PARAMETER_3;
        src2 = HAL_PARTITION_PARAMETER_2;
        dst2 = HAL_PARTITION_PARAMETER_4;
    }
    off = 0;
    hal_flash_read(src1, &off, actual, para_len);
    off = 0;
    hal_flash_write(dst1, &off, actual, para_len);
    off = 0;
    hal_flash_read(src2, &off, actual, para_len);
    off = 0;
    hal_flash_write(dst2, &off, actual, para_len);

    if (0 == FLASH_set_boot_bank())
    {
        LOG("Default boot bank switched successfully.\n");
        ret = 0;
        goto EXIT;
    }
    else
    {
        LOG("Error: failed changing the boot configuration\n");
        ret = -1;
        goto EXIT;
    }

EXIT:
    aos_free(buf);
    return ret;
}
