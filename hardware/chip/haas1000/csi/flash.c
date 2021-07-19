/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

/* CSI Nor Flash implementation */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <drv/spiflash.h>
#include <aos/mtd.h>
#include <aos/mtdpart.h>
#include "hal_trace.h"
#include "hal_norflash.h"
#include "cmsis.h"


static void * bes_flash_memcpy(void *dst, void *src, size_t num)
{
    int offset1 = (4 - ((uint32)dst & 3)) & 3;
    int offset2 = (4 - ((uint32)src & 3)) & 3;

    if(offset1 != offset2) {
        return memcpy(dst, src, num);
    }

    int wordnum = num > offset1 ? (num - offset1) /8 : 0;
    int slice = num > offset1 ? (num-offset1) % 8 : 0;
    char *pdst = (char *)dst;
    char *psrc = (char *)src;
    long long * pintsrc;
    long long * pintdst;

    while(offset1--) {
        *pdst++ = *psrc++;
    }

    pintdst = (long long*)pdst;
    pintsrc = (long long*)psrc;
    while(wordnum--) {
        *pintdst++ = *pintsrc++;
    }

    pdst = (char*)pintdst;
    psrc = (char*)pintsrc;
    while(slice--) {
        *pdst++ = *psrc++;
    }

    return dst;
}

csi_error_t csi_spiflash_spi_init(csi_spiflash_t *spiflash, uint32_t spi_idx, void *spi_cs_callback)
{
    if (spiflash == NULL) {
        return CSI_ERROR;
    }

    spiflash->spi_qspi.spi.dev.idx = spi_idx;
    spiflash->spi_qspi.spi.dev.reg_base = FLASH_NC_BASE;
    spiflash->spi_cs_callback = spi_cs_callback;
    return CSI_OK;
}

void csi_spiflash_spi_uninit(csi_spiflash_t *spiflash)
{
    if (spiflash == NULL) {
        return CSI_ERROR;
    }

    spiflash->spi_cs_callback = NULL;
    return;
}

csi_error_t csi_spiflash_get_flash_info(csi_spiflash_t *spiflash, csi_spiflash_info_t *flash_info)
{
    enum HAL_NORFLASH_RET_T ret;
    (void) spiflash;

    if (flash_info == NULL) {
        return CSI_ERROR;
    }

    ret = hal_norflash_get_size(HAL_NORFLASH_ID_0, &(flash_info->flash_size), NULL,
                        &(flash_info->page_size), &(flash_info->page_size));
    if (ret != HAL_NORFLASH_OK) {
        return CSI_ERROR;
    }

    return CSI_OK;
}

int32_t csi_spiflash_read(csi_spiflash_t *spiflash, uint32_t offset, void *data, uint32_t size)
{
    volatile char *flashPointer = NULL;
    uint32_t lock;

    (void) spiflash;

    if (data == NULL || size <= 0) {
        return CSI_ERROR;
    }

    watchdog_feeddog();
    lock = int_lock();
    hal_norflash_disable_remap(HAL_NORFLASH_ID_0);

    flashPointer = (volatile char *)(FLASH_NC_BASE + offset);
    bes_flash_memcpy(data, (void *)flashPointer, size);

    hal_norflash_re_enable_remap(HAL_NORFLASH_ID_0);
    int_unlock(lock);

    return size;
}

int32_t csi_spiflash_program(csi_spiflash_t *spiflash, uint32_t offset, const void *data, uint32_t size)
{
    int ret = 0;
    uint32_t lock;

    (void) spiflash;

    if (data == NULL || size <= 0) {
        return CSI_ERROR;
    }

    watchdog_feeddog();
    lock = int_lock();
    pmu_flash_write_config();
    hal_norflash_disable_remap(HAL_NORFLASH_ID_0);

    ret = hal_norflash_write(HAL_NORFLASH_ID_0, offset, data, size);
    hal_norflash_re_enable_remap(HAL_NORFLASH_ID_0);
    pmu_flash_read_config();
    int_unlock(lock);

    if (ret != HAL_NORFLASH_OK) {
        TRACE("error %s %d, hal_norflash_write ret:%d", __func__, __LINE__, ret);
        return CSI_ERROR;
    }

    return size;
}

csi_error_t csi_spiflash_erase(csi_spiflash_t *spiflash, uint32_t offset, uint32_t size)
{
    int ret = 0;
    uint32_t lock = 0;

    (void) spiflash;

    watchdog_feeddog();
    lock = int_lock();
    pmu_flash_write_config();
    hal_norflash_disable_remap(HAL_NORFLASH_ID_0);

    ret = hal_norflash_erase(HAL_NORFLASH_ID_0, offset, size);
    hal_norflash_re_enable_remap(HAL_NORFLASH_ID_0);
    pmu_flash_read_config();
    int_unlock(lock);

    if (ret != HAL_NORFLASH_OK) {
        TRACE("error %s %d, ret:%d", __func__, __LINE__, ret);
        return CSI_ERROR;
    }

    return CSI_OK;
}

static aos_mtd_t g_mtd_nor_dev;

struct part_info {
    uint32_t cnt;
    struct mtd_part *part;
};

static int get_mtd_part(struct part_info *info)
{
    mtd_partition_t *p;
    struct mtd_part *parts;

    parts = calloc(mtd_partitions_amount, sizeof(struct mtd_part));
    if (parts == NULL) {
        return -ENOMEM;
    }

    for (int i = 0; i < mtd_partitions_amount; i++) {
        p = &mtd_partitions[i];
        parts[i].name_std = p->partition_name_std;
        parts[i].name = p->partition_name;
        parts[i].offset = p->partition_start_addr;
        parts[i].size = p->partition_length;
    }

    info->cnt = mtd_partitions_amount;
    info->part = parts;
    return 0;
}

int csi_flash_init()
{
    struct part_info info = {0};
    uint32_t blk_size = 4096;  //sector size.
    uint32_t page_size = 256;
    int ret;

    printf("%s:%d begin..\n", __func__, __LINE__);
    if (get_mtd_part(&info)) {
        printf("%s:%d get_mtd_part failed.\n", __func__, __LINE__);
        return -1;
    }

    ret = aos_mtd_nor_init(&g_mtd_nor_dev, blk_size, page_size);
    if (ret != 0) {
        printf("%s:%d aos_mtd_nor_init failed, ret:\n", __func__, __LINE__, ret);
        return -2;
    }

    ret = aos_mtd_register(&g_mtd_nor_dev, info.part, info.cnt);
    if (ret < 0) {
        printf("%s:%d aos_mtd_register failed, ret:\n", __func__, __LINE__, ret);
    } else {
        printf("%s:%d done\n", __func__, __LINE__);
    }

    return ret;
}
