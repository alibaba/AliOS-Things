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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <drv_eflash.h>

#include "soc.h"
#include "aos/hal/flash.h"

#define EFLASH_SECTOR_SIZE 512

extern const hal_logic_partition_t hal_partitions[];
static eflash_handle_t kv_eflash_handle;
int g_erase_buf[EFLASH_SECTOR_SIZE / 4];

int32_t hal_flash_info_get(hal_partition_t in_partition, hal_logic_partition_t *partition)
{
    hal_logic_partition_t *logic_partition;

    logic_partition = (hal_logic_partition_t *)&hal_partitions[ in_partition ];
    memcpy(partition, logic_partition, sizeof(hal_logic_partition_t));

    return 0;
}

int32_t hal_flash_erase(hal_partition_t in_partition, uint32_t off_set, uint32_t size)
{
    int i;
    uint32_t addr;
    hal_logic_partition_t partition_info;

    if (hal_flash_info_get(in_partition, &partition_info) != 0) {
        return -1;
    }

    addr = partition_info.partition_start_addr + off_set;

    memset(g_erase_buf, 0xFF, EFLASH_SECTOR_SIZE);

    for (i = 0; i < size / EFLASH_SECTOR_SIZE; i++) {
        csi_eflash_erase_sector(kv_eflash_handle, addr + EFLASH_SECTOR_SIZE * i);
        csi_eflash_program(kv_eflash_handle, addr + EFLASH_SECTOR_SIZE * i, g_erase_buf, EFLASH_SECTOR_SIZE);
    }

    return 0;
}

int32_t hal_flash_write(hal_partition_t in_partition, uint32_t *off_set, const void *in_buf , uint32_t in_buf_len)
{
    uint32_t addr;
    hal_logic_partition_t partition_info;

    if (hal_flash_info_get(in_partition, &partition_info) != 0) {
        return -1;
    }

    addr = partition_info.partition_start_addr + *off_set;

    memset(g_erase_buf, 0xFF, EFLASH_SECTOR_SIZE);

    if ((addr + in_buf_len) <= ((addr & 0xFFFFFE00) + EFLASH_SECTOR_SIZE)) {
        csi_eflash_read(kv_eflash_handle, addr & 0xFFFFFE00, g_erase_buf, EFLASH_SECTOR_SIZE);
        memcpy((void *)((uint32_t)g_erase_buf + addr % EFLASH_SECTOR_SIZE), in_buf, in_buf_len);
        csi_eflash_erase_sector(kv_eflash_handle, addr & 0xFFFFFE00);
        csi_eflash_program(kv_eflash_handle, addr & 0xFFFFFE00, g_erase_buf, EFLASH_SECTOR_SIZE);
    } else {
        uint32_t remain_len = addr + in_buf_len - ((addr & 0xFFFFFE00) + EFLASH_SECTOR_SIZE);
        uint32_t len = in_buf_len - remain_len;

        csi_eflash_read(kv_eflash_handle, addr & 0xFFFFFE00, g_erase_buf, EFLASH_SECTOR_SIZE);
        memcpy((void *)((uint32_t)g_erase_buf + addr % EFLASH_SECTOR_SIZE), in_buf, len);
        csi_eflash_erase_sector(kv_eflash_handle, addr & 0xFFFFFE00);
        csi_eflash_program(kv_eflash_handle, addr & 0xFFFFFE00, g_erase_buf, EFLASH_SECTOR_SIZE);

        memset(g_erase_buf, 0xFF, EFLASH_SECTOR_SIZE);
        csi_eflash_read(kv_eflash_handle, (addr & 0xFFFFFE00) + EFLASH_SECTOR_SIZE, g_erase_buf, EFLASH_SECTOR_SIZE);
        memcpy((void *)g_erase_buf, (void *)((uint32_t)in_buf + len), remain_len);
        csi_eflash_erase_sector(kv_eflash_handle, (addr & 0xFFFFFE00) + EFLASH_SECTOR_SIZE);
        csi_eflash_program(kv_eflash_handle, (addr & 0xFFFFFE00) + EFLASH_SECTOR_SIZE, g_erase_buf, EFLASH_SECTOR_SIZE);
    }

    return 0;
}

int32_t hal_flash_read(hal_partition_t in_partition, uint32_t *off_set, void *out_buf, uint32_t out_buf_len)
{
    uint32_t addr;
    hal_logic_partition_t partition_info;

    if (hal_flash_info_get(in_partition, &partition_info) != 0) {
        return -1;
    }

    addr = partition_info.partition_start_addr + *off_set;


    csi_eflash_read(kv_eflash_handle, addr, out_buf, out_buf_len);

    return 0;
}

int32_t hal_flash_enable_secure(hal_partition_t partition, uint32_t off_set, uint32_t size)
{
    return 0;
}

int32_t hal_flash_dis_secure(hal_partition_t partition, uint32_t off_set, uint32_t size)
{
    return 0;
}

void hal_flash_init(void)
{
    kv_eflash_handle = csi_eflash_initialize(0, NULL);
}
