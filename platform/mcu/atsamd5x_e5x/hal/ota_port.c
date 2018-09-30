/**
 * \file
 *
 * \brief OTA port related functionality implementation.
 *
 * Copyright (c) 2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 */
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

extern void _flash_program(void *const hw, const uint32_t dst_addr, const uint8_t *buffer, const uint16_t size);

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
    uint32_t pno[4] = {
        HAL_PARTITION_PARAMETER_1,
        HAL_PARTITION_PARAMETER_2,
        HAL_PARTITION_PARAMETER_3,
        HAL_PARTITION_PARAMETER_4
    };
    uint32_t i;
    uint32_t off_set;
    uint8_t  *pbuf;
    hal_logic_partition_t *partition_info;

    for (i = 0; i < 4; i++) {
        partition_info = hal_flash_get_info(pno[i]);
        off_set = 0;
        pbuf = (uint8_t *)partition_info->partition_start_addr;
        flash_write(&FLASH_0, partition_info->partition_start_addr - 0x80000, pbuf, partition_info->partition_length);
    }

    atsamd5x_e5x_switch_flash_bank = FBSW_MAGIC;
}


static bool ota_check_crc32(void)
{
    uint32_t crc;
    hal_logic_partition_t *partition_info;

    partition_info = hal_flash_get_info(HAL_PARTITION_OTA_TEMP);
    crc = *((uint32_t *)(partition_info->partition_start_addr + _ota_flash_offset - 4));

    PAC->WRCTRL.reg = PAC_WRCTRL_PERID(ID_DSU) | PAC_WRCTRL_KEY_CLR;

    DSU->ADDR.reg = partition_info->partition_start_addr;
    DSU->LENGTH.reg = _ota_flash_offset - 4;
    DSU->DATA.reg = 0xFFFFFFFF;
    DSU->STATUSA.reg = DSU->STATUSA.reg;
    DSU->CTRL.reg = DSU_CTRL_CRC;

    while (0 == DSU->STATUSA.bit.DONE);

    if (0 == DSU->STATUSA.bit.BERR && crc == (0xFFFFFFFF - DSU->DATA.reg)) {
        return true;
    } else {
        return false;
    }
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
    static bool image_overlow = false;

    if (image_overlow == true) {
        goto write_end;
    }

    partition_info = hal_flash_get_info(HAL_PARTITION_OTA_TEMP);
    wr_addr = partition_info->partition_start_addr + _ota_flash_offset;

    if (_ota_flash_offset + in_buf_len > partition_info->partition_length) {
        image_overlow = true;
        LOG("ota cb: Too big image\n");
        goto write_end;
    }

    while (in_buf_len > 0) {
        cp_len = MIN(NVMCTRL_PAGE_SIZE - _ota_buf_len, in_buf_len);
        memcpy(&_ota_buf[_ota_buf_len], in_buf, cp_len);
        _ota_buf_len += cp_len;
        in_buf += cp_len;
        in_buf_len -= cp_len;

        if (_ota_buf_len == NVMCTRL_PAGE_SIZE) {
            _flash_program(NVMCTRL, wr_addr, _ota_buf, NVMCTRL_PAGE_SIZE);
            _ota_buf_len = 0;
            wr_addr += NVMCTRL_PAGE_SIZE;
            _ota_flash_offset += NVMCTRL_PAGE_SIZE;
        }
    }

write_end:
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
            _ota_flash_offset += _ota_buf_len;
            _ota_buf_len = 0;
        }
    }

    if (param == NULL) {
        return -1;
    }

    if (param->result_type == OTA_FINISH)
    {
        LOG("ota cb: finish!\n");
        if (ota_check_crc32()) {
            request_flash_bank_switch();
        } else {
            LOG("ota cb: CRC error!\n");
        }
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
