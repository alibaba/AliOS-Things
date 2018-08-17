#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <errno.h>
#include <hal/ota.h>
#include <aos/log.h>
#include <hal/soc/soc.h>
#include <aos/kernel.h>
#include "atmel_start.h"

#ifndef MIN
#  define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

extern uint32_t atsamd5x_e5x_switch_flash_bank;

uint32_t _ota_flash_offset = 0;
uint8_t  _ota_buf[NVMCTRL_PAGE_SIZE];
uint32_t _ota_buf_len = 0;

static int atsamd5x_e5x_ota_init(hal_ota_module_t *m, void *something);
static int atsamd5x_e5x_ota_write(hal_ota_module_t *m, volatile uint32_t *off_set, uint8_t *in_buf , uint32_t in_buf_len);
static int atsamd5x_e5x_ota_read(hal_ota_module_t *m,  volatile uint32_t *off_set, uint8_t *out_buf, uint32_t out_buf_len);
static int atsamd5x_e5x_ota_set_boot(hal_ota_module_t *m, void *something);


static int request_flash_bank_switch(void)
{
    uint32_t off_set;
    uint8_t  *pbuf;
    hal_logic_partition_t *partition_info;

    partition_info = hal_flash_get_info(HAL_PARTITION_PARAMETER_1);
    off_set = 0;
    pbuf = (uint8_t *)partition_info->partition_start_addr;
    hal_flash_write(HAL_PARTITION_PARAMETER_3, &off_set, pbuf, partition_info->partition_length);

    partition_info = hal_flash_get_info(HAL_PARTITION_PARAMETER_4);
    off_set = 0;
    pbuf = (uint8_t *)partition_info->partition_start_addr;
    hal_flash_write(HAL_PARTITION_PARAMETER_1, &off_set, pbuf, partition_info->partition_length);

    partition_info = hal_flash_get_info(HAL_PARTITION_PARAMETER_2);
    off_set = 0;
    pbuf = (uint8_t *)partition_info->partition_start_addr;
    hal_flash_write(HAL_PARTITION_PARAMETER_4, &off_set, pbuf, partition_info->partition_length);

    partition_info = hal_flash_get_info(HAL_PARTITION_PARAMETER_1);
    off_set = 0;
    pbuf = (uint8_t *)partition_info->partition_start_addr;
    hal_flash_write(HAL_PARTITION_PARAMETER_2, &off_set, pbuf, partition_info->partition_length);

    partition_info = hal_flash_get_info(HAL_PARTITION_PARAMETER_1);
    hal_flash_erase(HAL_PARTITION_PARAMETER_1, partition_info->partition_start_addr,
                        partition_info->partition_length);

    atsamd5x_e5x_switch_flash_bank = FBSW_MAGIC;
}


static int atsamd5x_e5x_ota_init(hal_ota_module_t *m, void *something)
{
    hal_logic_partition_t *partition_info;

    LOG("ota cb: init\n");
    _ota_flash_offset = *(uint32_t*)something;
    if (_ota_flash_offset == 0) {
        partition_info = hal_flash_get_info(HAL_PARTITION_OTA_TEMP);
        hal_flash_erase(HAL_PARTITION_OTA_TEMP, 0, partition_info->partition_length);
    }

    return 0;
}


static int atsamd5x_e5x_ota_write(hal_ota_module_t *m, volatile uint32_t *off_set, uint8_t *in_buf , uint32_t in_buf_len)
{
    hal_logic_partition_t *partition_info;
    uint32_t wr_addr;
    uint32_t cp_len;
    int ret = 0;

    partition_info = hal_flash_get_info(HAL_PARTITION_OTA_TEMP);
    wr_addr = partition_info->partition_start_addr + _ota_flash_offset;

    while (in_buf_len > 0) {
        cp_len = MIN(NVMCTRL_PAGE_SIZE - _ota_buf_len, in_buf_len);
        memcpy(&_ota_buf[_ota_buf_len], in_buf, cp_len);
        _ota_buf_len += cp_len;
        in_buf += cp_len;
        in_buf_len -= cp_len;

        if (_ota_buf_len == NVMCTRL_PAGE_SIZE) {
            if (ERR_NONE != flash_append(&FLASH_0, wr_addr, _ota_buf, NVMCTRL_PAGE_SIZE)) {
                LOG("ota cb: write error\n");
                ret = -1;
                break;
            } else {
                _ota_buf_len = 0;
                wr_addr += NVMCTRL_PAGE_SIZE;
                _ota_flash_offset += NVMCTRL_PAGE_SIZE;
            }
        }
    }

    return ret;
}


static int atsamd5x_e5x_ota_read(hal_ota_module_t *m,  volatile uint32_t *off_set, uint8_t *out_buf, uint32_t out_buf_len)
{
    LOG("ota cb: read\n");
    hal_flash_read(HAL_PARTITION_OTA_TEMP, (uint32_t*)off_set, out_buf, out_buf_len);
    return 0;
}


static int atsamd5x_e5x_ota_set_boot(hal_ota_module_t *m, void *something)
{
    ota_finish_param_t *param = (ota_finish_param_t *)something;
    hal_logic_partition_t *partition_info;
    uint32_t wr_addr;

    if (_ota_buf_len != 0) {
        partition_info = hal_flash_get_info(HAL_PARTITION_OTA_TEMP);
        wr_addr = partition_info->partition_start_addr + _ota_flash_offset;
        if (ERR_NONE != flash_append(&FLASH_0, wr_addr, _ota_buf, _ota_buf_len)) {
            return -1;
        } else {
            _ota_buf_len = 0;
            _ota_flash_offset += _ota_buf_len;
        }
    }

    if (param == NULL) {
        return -1;
    }

    if (param->result_type == OTA_FINISH)
    {
        LOG("ota cb: finish!\n");
        request_flash_bank_switch();
    } else if (param->result_type == OTA_BREAKPOINT) {
        LOG("ota cb: package is incomplete!\n");
    }

    return 0;
}


struct hal_ota_module_s aos_ota_module_atsamd5x_e5x = {
    .init = atsamd5x_e5x_ota_init,
    .ota_write = atsamd5x_e5x_ota_write,
    .ota_read = atsamd5x_e5x_ota_read,
    .ota_set_boot = atsamd5x_e5x_ota_set_boot,
};
