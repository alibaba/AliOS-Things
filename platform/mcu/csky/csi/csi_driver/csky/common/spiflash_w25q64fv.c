/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/******************************************************************************
 * @file     spiflash_w25q64fv.c
 * @brief    CSI Source File for Embedded Flash Driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "drv_spiflash.h"
#include "spiflash_w25q64fv.h"
#include "soc.h"
#include "pin.h"


#define ERR_SPIFLASH(errno) (CSI_DRV_ERRNO_SPIFLASH_BASE | errno)

#define SPIFLASH_NULL_PARAM_CHK(para)           \
    do {                                        \
        if (para == NULL) {                     \
            return ERR_SPIFLASH(DRV_ERROR_PARAMETER);   \
        }                                       \
    } while (0)

typedef void *qspi_handle_t;

static qspi_command_t s_command;
static qspi_handle_t qspi;

#ifndef true
#define true  0x01
#endif

#ifndef false
#define false 0x00
#endif

static uint8_t spiflash_init_flag = false;

typedef struct {
    spiflash_info_t spiflashinfo;
    spiflash_event_cb_t cb;
    spiflash_status_t status;
} ck_spiflash_priv_t;

static ck_spiflash_priv_t s_spiflash_handle[CONFIG_SPIFLASH_NUM];

typedef struct {
    uint32_t        base;
    uint32_t        irq;
    uint32_t        write_addr;
    uint32_t        read_addr;
    uint8_t         burst_len;
} cd_qspi_priv_t;

static cd_qspi_priv_t s_qspi_instance[CONFIG_QSPI_NUM];
static cd_qspi_reg_t *addr = NULL;
extern int32_t target_qspi_init(int32_t idx, uint32_t *base, uint32_t *irq);
/* Driver Capabilities */
static const spiflash_capabilities_t driver_capabilities = {
    1, /* event_ready */
    2, /* data_width = 0:8-bit, 1:16-bit, 2:32-bit */
    1  /* erase_chip */
};

//
// Functions
//

extern int32_t target_get_spiflash(int32_t idx, spiflash_info_t *info);

static int32_t cd_qspi_set_operate_addr(uint32_t opt_addr)
{
    addr->FLASH_COMMAND_ADDR = opt_addr;
    return 0;
}

static int32_t cd_qspi_is_busy()
{
    if (addr->FLASH_COMMAND_CONTROL & QSPI_FLASH_IS_BUSY) {
        return 1;
    } else {
        if (addr->QSPI_CONFIGURATION & QSPI_IS_BUSY) {
            return 0;
        } else {
            return 1;
        }
    }
}

static void cd_qspi_execute_the_command()
{
    addr->FLASH_COMMAND_CONTROL |= QSPI_FLASH_EXECUTE_COMMAND_EN;
}

static void cd_qspi_reset_config(void)
{
    addr->FLASH_COMMAND_CONTROL &= 0xfff;
}

static void cd_qspi_set_opcode(uint8_t opcode)
{
    addr->FLASH_COMMAND_CONTROL &= ~(0xff << QSPI_FLASH_OPCODE_OFFSET);
    addr->FLASH_COMMAND_CONTROL |= (opcode << QSPI_FLASH_OPCODE_OFFSET);
}

static void cd_qspi_set_addr_byte_num(qspi_address_size_e num)
{
    addr->FLASH_COMMAND_CONTROL &= ~(3 << ADDR_BYTE_OFFSET);
    addr->FLASH_COMMAND_CONTROL |= (num << ADDR_BYTE_OFFSET);
    addr->FLASH_COMMAND_CONTROL |= COMMAND_ADDR_EN;
}

static void cd_qspi_set_read_data_num(qspi_alternate_byte_size_e num)
{
    addr->FLASH_COMMAND_CONTROL &= ~(7 << READ_DATA_NUM_OFFSET);
    addr->FLASH_COMMAND_CONTROL |= (num << READ_DATA_NUM_OFFSET);
    addr->FLASH_COMMAND_CONTROL |= QSPI_READ_DATA_EN;
}

static void cd_qspi_set_write_data_num(qspi_alternate_byte_size_e num)
{
    addr->FLASH_COMMAND_CONTROL &= ~(7 << WRITE_DATA_NUM_OFFSET);
    addr->FLASH_COMMAND_CONTROL |= (num << WRITE_DATA_NUM_OFFSET);
    addr->FLASH_COMMAND_CONTROL |= QSPI_WRITE_DATA_EN;
}

qspi_handle_t cd_qspi_initialize(int32_t idx)
{

    if (idx < 0 || idx >= CONFIG_QSPI_NUM) {
        return NULL;
    }

    uint32_t base = 0u;
    uint32_t irq;

    int32_t real_idx = target_qspi_init(idx, &base, &irq);

    if (real_idx != idx) {
        return NULL;
    }

    cd_qspi_priv_t *qspi_priv = &s_qspi_instance[idx];

    qspi_priv->base = base;
    qspi_priv->irq  = irq;

    addr = (cd_qspi_reg_t *)base;

//    drv_nvic_enable_irq(qspi_priv->irq);

    return (qspi_handle_t)qspi_priv;
}

int32_t cd_qspi_command(qspi_handle_t handle, qspi_command_t *command)
{
    cd_qspi_priv_t *qspi_priv = handle;

    cd_qspi_reset_config();
    cd_qspi_set_opcode(command->instruction);

    if (command->address_size < QSPI_ADDRESS_BYTE_NONE) {
        cd_qspi_set_addr_byte_num(command->address_size);
        cd_qspi_set_operate_addr(command->address);
    }

#if 0

    if (command->command_type >= QSPI_COMMAND_TYPE_DATA_NONE) {
        cd_qspi_execute_the_command();

        while (cd_qspi_is_busy());

        return 0;
    } else if (command->command_type == QSPI_COMMAND_TYPE_READ_DATA) {
        cd_qspi_set_read_data_num(command->alternate_byte_size);
        qspi_priv->burst_len = command->alternate_byte_size;
        qspi_priv->read_addr = command->address;
    } else if (command->command_type == QSPI_COMMAND_TYPE_WRITE_DATA) {
        cd_qspi_set_write_data_num(command->alternate_byte_size);
        qspi_priv->burst_len = command->alternate_byte_size;
        qspi_priv->write_addr = command->address;
    }

#else

    if (command->alternate_byte_size == QSPI_ALTERNATE_BYTE_NONE) {
        cd_qspi_execute_the_command();

        while (cd_qspi_is_busy());

        return 0;
    }

    qspi_priv->burst_len = command->alternate_byte_size;
    qspi_priv->read_addr = command->address;
    qspi_priv->write_addr = command->address;
#endif
    return 0;
}

int32_t cd_qspi_receive(qspi_handle_t handle, void *data, uint32_t num)
{
    cd_qspi_priv_t *qspi_priv = handle;

    cd_qspi_set_read_data_num(qspi_priv->burst_len);

    uint8_t *buf = (uint8_t *)data;
    uint32_t *src = (uint32_t *) & (addr->FLASH_COMMAND_READL);
    uint8_t burst_len = 0;
    uint32_t i;
    uint32_t src_addr = qspi_priv->read_addr;
    uint32_t temp[2];
    uint8_t *ptr = (uint8_t *)&temp;

    burst_len = qspi_priv->burst_len + 1;

    while (num > (burst_len - 1)) {
        cd_qspi_set_operate_addr(src_addr);
        cd_qspi_execute_the_command();

        while (cd_qspi_is_busy());

        for (i = 0; i < 2; i++) {
            temp[i] = src[i];
        }

        for (i = 0; i < burst_len; i++) {
            buf[i] = ptr[i];
        }

        src_addr += burst_len;
        buf += burst_len;
        num -= burst_len;
    }

    if (num) {
        cd_qspi_set_operate_addr(src_addr);
        cd_qspi_execute_the_command();

        while (cd_qspi_is_busy());

//        cd_qspi_set_opcode(command);
        for (i = 0; i < 2; i++) {
            temp[i] = src[i];
        }

        for (i = 0; i < num; i++) {
            buf[i] = ptr[i];
        }

        buf += num;
        src_addr += num;
        num = 0;
    }

    return 0;
}

int32_t cd_qspi_send(qspi_handle_t handle, const void *data, uint32_t num)
{
    cd_qspi_priv_t *qspi_priv = handle;
    cd_qspi_set_write_data_num(qspi_priv->burst_len);

    uint32_t *dst = (uint32_t *) & (addr->FLASH_COMMAND_WRITEL);
    uint32_t dst_addr = qspi_priv->write_addr;
    uint8_t burst_len = qspi_priv->burst_len + 1;
    uint8_t buf[8] = {0};

    cd_qspi_set_operate_addr(dst_addr);

    uint8_t block_size = 8;

    if (burst_len <= 4) {
        block_size = 4;
    }

    uint32_t i;
    memcpy(buf, data, burst_len);

    for (i = burst_len; i < block_size; i++) {
        buf[i] = 0xff;
    }

    for (i = 0; i < block_size; i += 4) {
        dst[i >> 2] = buf[i] | buf[i + 1] << 8 | buf[i + 2] << 16 | buf[i + 3] << 24;
    }

    cd_qspi_execute_the_command();

    while (cd_qspi_is_busy());

    return i;
}


static int spiflash_read_status_register(uint8_t *data)
{
    s_command.instruction = W25Q64FV_READ_STATUS;
    s_command.address_size = QSPI_ADDRESS_BYTE_NONE;
    s_command.address = 0;
    s_command.alternate_byte_size = QSPI_ALTERNATE_BYTE_8;
    cd_qspi_command(qspi, &s_command);
    cd_qspi_receive(qspi, data, 1);
    return 0;
}

/**
  \brief       Get driver capabilities.
  \param[in]   spiflash handle to operate.
  \return      \ref spiflash_capabilities_t
*/
spiflash_capabilities_t csi_spiflash_get_capabilities(int32_t idx)
{
    if (idx < 0 || idx >= CONFIG_SPIFLASH_NUM) {
        spiflash_capabilities_t ret;
        memset(&ret, 0, sizeof(spiflash_capabilities_t));
        return ret;
    }

    return driver_capabilities;
}

/**
  \brief       Initialize SPIFLASH Interface. 1. Initializes the resources needed for the SPIFLASH interface 2.registers event callback function
  \param[in]   handle  spiflash handle to operate.
  \param[in]   cb_event  Pointer to \ref spiflash_event_cb_t
  \return      \ref execution_status
*/
spiflash_handle_t csi_spiflash_initialize(int32_t idx, spiflash_event_cb_t cb_event)
{
    if (idx < 0 || idx >= CONFIG_SPIFLASH_NUM) {
        return NULL;
    }

    ck_spiflash_priv_t *spiflash_priv = &s_spiflash_handle[idx];

    spiflash_info_t info;
    int32_t real_idx = target_get_spiflash(idx, &info);

    if (idx != real_idx) {
        return NULL;
    }

    spiflash_priv->spiflashinfo.start = info.start;
    spiflash_priv->spiflashinfo.end = info.end;
    spiflash_priv->spiflashinfo.sector_count = info.sector_count;

    /* initialize the spiflash context */
    spiflash_priv->cb = cb_event;
    spiflash_priv->status.busy = 0;
    spiflash_priv->status.error = 0U;
    spiflash_priv->spiflashinfo.sector_size = W25Q64FV_SECTOR_SIZE;
    spiflash_priv->spiflashinfo.page_size = W25Q64FV_PAGE_SIZE;
    spiflash_priv->spiflashinfo.program_unit = 1;
    spiflash_priv->spiflashinfo.erased_value = 0xff;

    spiflash_init_flag = true;

    qspi = cd_qspi_initialize(EXAMPLE_QSPI_IDX);
    return (spiflash_handle_t)spiflash_priv;
}

/**
  \brief       De-initialize SPIFLASH Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle  spiflash handle to operate.
  \return      \ref execution_status
*/
int32_t csi_spiflash_uninitialize(spiflash_handle_t handle)
{
    SPIFLASH_NULL_PARAM_CHK(handle);

    ck_spiflash_priv_t *spiflash_priv = (ck_spiflash_priv_t *)handle;
    spiflash_init_flag = false;
    spiflash_priv->cb = NULL;

    return 0;
}

/**
  \brief       Read data from Flash.
  \param[in]   handle  spiflash handle to operate.
  \param[in]   addr  Data address.
  \param[in]   data  Pointer to a buffer storing the data read from Flash.
  \param[in]   cnt   Number of data items to read.
  \return      number of data items read or \ref execution_status
*/
int32_t csi_spiflash_read(spiflash_handle_t handle, uint32_t addr, void *data, uint32_t cnt)
{
    SPIFLASH_NULL_PARAM_CHK(handle);
    SPIFLASH_NULL_PARAM_CHK(data);
    SPIFLASH_NULL_PARAM_CHK(cnt);

    ck_spiflash_priv_t *spiflash_priv = handle;

    if (spiflash_priv->spiflashinfo.start > addr || spiflash_priv->spiflashinfo.end < addr || spiflash_priv->spiflashinfo.start > (addr + cnt - 1) || spiflash_priv->spiflashinfo.end < (addr + cnt - 1)) {
        return ERR_SPIFLASH(DRV_ERROR_PARAMETER);
    }

    uint8_t status = 1;
    uint32_t timeout_count = 0;

    if (spiflash_priv->status.busy) {
        return ERR_SPIFLASH(DRV_ERROR_BUSY);
    }

    if (spiflash_init_flag == false) {
        return ERR_SPIFLASH(DRV_ERROR);
    }

    spiflash_priv->status.error = 0U;
    uint32_t offset = addr - spiflash_priv->spiflashinfo.start;

    s_command.instruction = W25Q64FV_READ_COMMAND;
    s_command.address_size = QSPI_ADDRESS_BYTE_3;
    s_command.address = offset;
    s_command.alternate_byte_size = QSPI_ALTERNATE_BYTE_8;

    cd_qspi_command(qspi, &s_command);
    cd_qspi_receive(qspi, data, cnt);

    while (status & 0x1 && timeout_count < 0x30000) {
        spiflash_read_status_register(&status);
        timeout_count++;
    }

    status = 1;
    timeout_count = 0;
    return cnt;
}

int32_t spi_norflash_write_page(uint32_t addr, uint8_t *data, uint32_t cnt)
{
    uint8_t status = 1;
    uint32_t timeout_count = 0;

    while (cnt > (QSPI_ALTERNATE_BYTE_8 + 1)) {
        s_command.instruction = W25Q64FV_WRITE_EN;
        s_command.address_size = QSPI_ADDRESS_BYTE_NONE;
        s_command.address = 0;
        s_command.alternate_byte_size = QSPI_ALTERNATE_BYTE_NONE;
        cd_qspi_command(qspi, &s_command);


        while (status & 0x1 && timeout_count < 0x30000) {
            spiflash_read_status_register(&status);
            timeout_count++;
        }

        status = 1;
        timeout_count = 0;

        s_command.instruction = W25Q64FV_WRITE_COMMAND;
        s_command.address_size = QSPI_ADDRESS_BYTE_3;
        s_command.address = addr;
        s_command.alternate_byte_size = QSPI_ALTERNATE_BYTE_8;
        cd_qspi_command(qspi, &s_command);
        cd_qspi_send(qspi, (uint8_t *)data, cnt);

        while (status & 0x1 && timeout_count < 0x30000) {
            spiflash_read_status_register(&status);
            timeout_count++;
        }

        status = 1;
        timeout_count = 0;

        data += (QSPI_ALTERNATE_BYTE_8 + 1);
        addr += (QSPI_ALTERNATE_BYTE_8 + 1);
        cnt -= (QSPI_ALTERNATE_BYTE_8 + 1);
    }

    if (cnt) {
        s_command.instruction = W25Q64FV_WRITE_EN;
        s_command.address_size = QSPI_ADDRESS_BYTE_NONE;
        s_command.address = 0;
        s_command.alternate_byte_size = QSPI_ALTERNATE_BYTE_NONE;
        cd_qspi_command(qspi, &s_command);

        while (status & 0x1 && timeout_count < 0x30000) {
            spiflash_read_status_register(&status);
            timeout_count++;
        }

        status = 1;
        timeout_count = 0;

        s_command.instruction = W25Q64FV_WRITE_COMMAND;
        s_command.address_size = QSPI_ADDRESS_BYTE_3;
        s_command.address = addr;
        s_command.alternate_byte_size = cnt - 1;
        cd_qspi_command(qspi, &s_command);
        cd_qspi_send(qspi, (uint8_t *)data, cnt);

        while (status & 0x1 && timeout_count < 0x30000) {
            spiflash_read_status_register(&status);
            timeout_count++;
        }

        status = 1;
        timeout_count = 0;

        data += cnt;
        addr += cnt;
    }

    return 0;
}
/**
  \brief       Program data to Flash.
  \param[in]   handle  spiflash handle to operate.
  \param[in]   addr  Data address.
  \param[in]   data  Pointer to a buffer containing the data to be programmed to Flash..
  \param[in]   cnt   Number of data items to program.
  \return      number of data items programmed or \ref execution_status
*/
int32_t csi_spiflash_program(spiflash_handle_t handle, uint32_t addr, const void *data, uint32_t cnt)
{
    SPIFLASH_NULL_PARAM_CHK(handle);
    SPIFLASH_NULL_PARAM_CHK(data);
    SPIFLASH_NULL_PARAM_CHK(cnt);

    ck_spiflash_priv_t *spiflash_priv = handle;

    if (spiflash_priv->spiflashinfo.start > addr || spiflash_priv->spiflashinfo.end < addr || spiflash_priv->spiflashinfo.start > (addr + cnt - 1) || spiflash_priv->spiflashinfo.end < (addr + cnt - 1)) {
        return ERR_SPIFLASH(DRV_ERROR_PARAMETER);
    }

    if (spiflash_priv->status.busy) {
        return ERR_SPIFLASH(DRV_ERROR_BUSY);
    }

    if (spiflash_init_flag == false) {
        return ERR_SPIFLASH(DRV_ERROR);
    }

    uint32_t num = cnt;

    spiflash_priv->status.busy = 1U;
    spiflash_priv->status.error = 0U;

    uint32_t offset = addr - spiflash_priv->spiflashinfo.start;
    uint8_t *src = (uint8_t *)data;
    uint32_t first_page_space = W25Q64FV_PAGE_SIZE - (offset & (W25Q64FV_PAGE_SIZE - 1));

    if (first_page_space == W25Q64FV_PAGE_SIZE) {
        first_page_space = 0;
    }

    if (first_page_space > cnt) {
        first_page_space = cnt;
    }

    spi_norflash_write_page(offset, src, first_page_space);
    offset += first_page_space;
    src += first_page_space;
    cnt -= first_page_space;

    while (cnt >= W25Q64FV_PAGE_SIZE) {
        spi_norflash_write_page(offset, src, W25Q64FV_PAGE_SIZE);
        offset += W25Q64FV_PAGE_SIZE;
        src += W25Q64FV_PAGE_SIZE;
        cnt -= W25Q64FV_PAGE_SIZE;
    }

    if (cnt) {
        spi_norflash_write_page(offset , src, cnt);
    }

    spiflash_priv->status.busy = 0U;

    return num;
}

/**
  \brief       Erase Flash Sector.
  \param[in]   handle  spiflash handle to operate.
  \param[in]   addr  Sector address
  \return      \ref execution_status
*/
int32_t csi_spiflash_erase_sector(spiflash_handle_t handle, uint32_t addr)
{
    ck_spiflash_priv_t *spiflash_priv = handle;
    uint32_t timeout_count = 0;

    if (spiflash_priv->spiflashinfo.start > addr || spiflash_priv->spiflashinfo.end < addr) {
        return ERR_SPIFLASH(DRV_ERROR_PARAMETER);
    }

    SPIFLASH_NULL_PARAM_CHK(handle);

    uint32_t offset = addr - spiflash_priv->spiflashinfo.start;
    uint8_t status = 1;

    if (spiflash_priv->status.busy) {
        return ERR_SPIFLASH(DRV_ERROR_BUSY);
    }

    if (spiflash_init_flag == false) {
        return ERR_SPIFLASH(DRV_ERROR);
    }

    spiflash_priv->status.busy = 1U;
    spiflash_priv->status.error = 0U;

    s_command.instruction = W25Q64FV_WRITE_EN;
    s_command.address_size = QSPI_ADDRESS_BYTE_NONE;
    s_command.address = 0;
    s_command.alternate_byte_size = QSPI_ALTERNATE_BYTE_NONE;
    cd_qspi_command(qspi, &s_command);

    while (status & 0x1 && timeout_count < 0x30000) {
        spiflash_read_status_register(&status);
        timeout_count++;
    }

    status = 1;
    timeout_count = 0;

    s_command.instruction = W25Q64FV_ERASE_SECTOR;
    s_command.address_size = QSPI_ADDRESS_BYTE_3;
    s_command.address = offset;
    s_command.alternate_byte_size = QSPI_ALTERNATE_BYTE_NONE;
    cd_qspi_command(qspi, &s_command);

    while (status & 0x1 && timeout_count < 0x30000) {
        spiflash_read_status_register(&status);
        timeout_count++;
    }

    status = 1;
    timeout_count = 0;

    spiflash_priv->status.busy = 0U;

    if (spiflash_priv->cb) {
        spiflash_priv->cb(0, SPIFLASH_EVENT_READY);
    }

    return 0;
}

/**
  \brief       Erase complete Flash.
  \param[in]   handle  spiflash handle to operate.
  \return      \ref execution_status
*/
int32_t csi_spiflash_erase_chip(spiflash_handle_t handle)
{
    SPIFLASH_NULL_PARAM_CHK(handle);
    ck_spiflash_priv_t *spiflash_priv = handle;
    uint8_t status = 1;
    uint32_t timeout_count = 0;

    if (spiflash_init_flag == false) {
        return ERR_SPIFLASH(DRV_ERROR);
    }

    spiflash_priv->status.busy = 1U;
    s_command.instruction = W25Q64FV_WRITE_EN;
    s_command.address_size = QSPI_ADDRESS_BYTE_NONE;
    s_command.address = 0x0;
    s_command.alternate_byte_size = QSPI_ALTERNATE_BYTE_NONE;
    cd_qspi_command(qspi, &s_command);

    while (status & 0x1 && timeout_count < 0x30000) {
        spiflash_read_status_register(&status);
        timeout_count++;
    }

    status = 1;
    timeout_count = 0;

    s_command.instruction = W25Q64FV_ERASE_CHIP;
    s_command.address_size = QSPI_ADDRESS_BYTE_NONE;
    s_command.address = 0x0;
    s_command.alternate_byte_size = QSPI_ALTERNATE_BYTE_NONE;
    cd_qspi_command(qspi, &s_command);

    while (status & 0x1 && timeout_count < 0x30000) {
        spiflash_read_status_register(&status);
        timeout_count++;
    }

    status = 1;
    timeout_count = 0;

    spiflash_priv->status.busy = 0U;

    if (spiflash_priv->cb) {
        spiflash_priv->cb(0, SPIFLASH_EVENT_READY);
    }

    return 0;
}

/**
  \brief       Get Flash information.
  \param[in]   handle  spiflash handle to operate.
  \return      Pointer to Flash information \ref spiflash_info_t
*/
spiflash_info_t *csi_spiflash_get_info(spiflash_handle_t handle)
{
    if (handle == NULL || spiflash_init_flag == false) {
        return NULL;
    }

    ck_spiflash_priv_t *spiflash_priv = handle;
    spiflash_info_t *spiflash_info = &(spiflash_priv->spiflashinfo);
    return spiflash_info;
}

/**
  \brief       Get SPIFLASH status.
  \param[in]   handle  spiflash handle to operate.
  \return      SPIFLASH status \ref spiflash_status_t
*/
spiflash_status_t csi_spiflash_get_status(spiflash_handle_t handle)
{
    if (handle == NULL || spiflash_init_flag == false) {
        spiflash_status_t ret;
        memset(&ret, 0, sizeof(spiflash_status_t));
        return ret;
    }

    ck_spiflash_priv_t *spiflash_priv = handle;

    uint8_t status = 0;
    spiflash_read_status_register(&status);
    spiflash_priv->status.busy = status;
    return spiflash_priv->status;
}
