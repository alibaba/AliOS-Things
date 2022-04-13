/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>

#include "amp_defines.h"

#include "amp_hal_flash.h"


extern void ql_norflash_get_addr(unsigned char *patition_name, unsigned int *start_addr, unsigned int *size);
extern unsigned int ql_norflash_do_read(unsigned char *patition_name, unsigned int addr, unsigned int buf_addr, unsigned int size);
extern unsigned int ql_norflash_do_write(unsigned char *patition_name, unsigned int addr, unsigned int buf_addr, unsigned int size);
extern int ql_norflash_do_erase(unsigned char *patition_name, unsigned int addr, unsigned int size);


int32_t amp_hal_flash_read(amp_partition_id_t id, uint32_t *offset, void *buffer, uint32_t buffer_len)
{
    unsigned int start_addr = 0;
    unsigned int part_size;
    int32_t ret = -1;

    if (id == AMP_PARTITION_ID_KV) {
        ql_norflash_get_addr("ali_flash", &start_addr, &part_size);
        ret = ql_norflash_do_read("ali_flash", start_addr + *offset, buffer, buffer_len);
    }
    return ret;
}

int32_t amp_hal_flash_write(amp_partition_id_t id, uint32_t *offset, const void *buffer, uint32_t buffer_len)
{
    unsigned int start_addr = 0;
    unsigned int part_size;
    int32_t ret = -1;

    if (id == AMP_PARTITION_ID_KV) {
        ql_norflash_get_addr("ali_flash", &start_addr, &part_size);
        ret = ql_norflash_do_write("ali_flash", start_addr + *offset, buffer, buffer_len);
    }
    return ret;
}

int32_t amp_hal_flash_erase(amp_partition_id_t id, uint32_t offset, uint32_t size)
{
    unsigned int start_addr = 0;
    unsigned int part_size;
    int32_t ret = -1;

    if (id == AMP_PARTITION_ID_KV) {
        ql_norflash_get_addr("ali_flash", &start_addr, &part_size);
        ret = ql_norflash_do_erase("ali_flash", start_addr + offset, size);
    }
    return ret;
}

