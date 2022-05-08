/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#include <aos/flash_core.h>
#include <objects.h>
#include <PinNames.h>
#include <pinmap.h>
#include <ameba_soc.h>
#include <drv/spiflash.h>

typedef struct {
    aos_flash_t flash;
    uint32_t page_data_buf[256 / sizeof(uint32_t)];
} flash_rtl872xd_t;

static void flash_unregister(aos_flash_t *flash)
{
}

static aos_status_t flash_startup(aos_flash_t *flash)
{
    return 0;
}

static void flash_shutdown(aos_flash_t *flash)
{
}

static aos_status_t flash_read_page(aos_flash_t *flash, uint64_t page, size_t data_offset, size_t data_count,
                                    size_t spare_offset, size_t spare_count)
{
    uint32_t offset = (uint32_t)(page * flash->page_data_size + data_offset);
    uint32_t offset_aligned = offset & ~(uint32_t)0x3;
    uint32_t end = offset + (uint32_t)data_count;
    uint32_t data_offset_aligned = (uint32_t)data_offset & ~(uint32_t)0x3;

    FLASH_Write_Lock();

    for (uint32_t i = 0; offset_aligned + i < end; i += 4) {
        uint32_t *dst = (uint32_t *)&((uint8_t *)flash->page_data_buf)[data_offset_aligned + i];
        *dst = HAL_READ32(SPI_FLASH_BASE, offset_aligned + i);
    }

    FLASH_Write_Unlock();

    return 0;
}

static aos_status_t flash_write_page(aos_flash_t *flash, uint64_t page, size_t data_offset, size_t data_count,
                                     size_t spare_offset, size_t spare_count)
{
    uint32_t offset = (uint32_t)(page * flash->page_data_size + data_offset);
    uint32_t offset_aligned = offset & ~(uint32_t)0x3;
    uint32_t end = offset + (uint32_t)data_count;
    uint32_t end_aligned = end & ~(uint32_t)0x3;
    uint32_t data_offset_aligned = (uint32_t)data_offset & ~(uint32_t)0x3;
    uint32_t data_end = (uint32_t)data_offset & ~(uint32_t)0x3;
    uint32_t data_end_aligned = data_end & ~(uint32_t)0x3;

    FLASH_Write_Lock();

    if (offset_aligned != offset) {
        union {
            uint8_t u8[4];
            uint32_t u32;
        } tmp;
        tmp.u32 = HAL_READ32(SPI_FLASH_BASE, offset_aligned);
        for (uint32_t i = 0; data_offset_aligned + i < data_offset; i++)
            ((uint8_t *)flash->page_data_buf)[data_offset_aligned + i] = tmp.u8[i];
    }

    if (end_aligned != end) {
        union {
            uint8_t u8[4];
            uint32_t u32;
        } tmp;
        tmp.u32 = HAL_READ32(SPI_FLASH_BASE, end_aligned);
        for (uint32_t i = data_end & ~(uint32_t)0x3; i < 4; i++)
            ((uint8_t *)flash->page_data_buf)[data_end_aligned + i] = tmp.u8[i];
    }

    for (uint32_t i = 0; offset_aligned + i < end; i += 4)
        FLASH_TxData12B(offset_aligned + i, 4, &((uint8_t *)flash->page_data_buf)[data_offset_aligned + i]);

    FLASH_Write_Unlock();

    return 0;
}

static aos_status_t flash_erase_block(aos_flash_t *flash, uint64_t block)
{
    uint32_t block_size = (uint32_t)(flash->pages_per_block * flash->page_data_size);
    uint32_t offset = (uint32_t)(block * block_size);

    FLASH_Write_Lock();
    FLASH_Erase(EraseSector, offset);
    DCache_Invalidate(SPI_FLASH_BASE + offset, block_size);
    FLASH_Write_Unlock();

    return 0;
}

static const aos_flash_ops_t flash_rtl872xd_ops = {
    .unregister     = flash_unregister,
    .startup        = flash_startup,
    .shutdown       = flash_shutdown,
    .read_page      = flash_read_page,
    .write_page     = flash_write_page,
    .erase_block    = flash_erase_block,
    .is_bad_block   = NULL,
    .mark_bad_block = NULL,
};

static aos_status_t flash_rtl872xd_register(flash_rtl872xd_t *flash_rtl872xd)
{
    if (!flash_rtl872xd)
        return -EINVAL;

    flash_rtl872xd->flash.ops = &flash_rtl872xd_ops;
    flash_rtl872xd->flash.flags = AOS_FLASH_F_TYPE_NOR;
    flash_rtl872xd->flash.block_count = 1024;
    flash_rtl872xd->flash.pages_per_block = 16;
    flash_rtl872xd->flash.page_data_size = sizeof(flash_rtl872xd->page_data_buf);
    flash_rtl872xd->flash.page_spare_size = 0;
    flash_rtl872xd->flash.page_data_buf = flash_rtl872xd->page_data_buf;
    flash_rtl872xd->flash.page_spare_buf = NULL;

    return aos_flash_register(&flash_rtl872xd->flash);
}

static int flash_rtl872xd_init(void)
{
    static flash_rtl872xd_t flash_rtl872xd;

    flash_rtl872xd.flash.dev.id = 0;

    return (int)flash_rtl872xd_register(&flash_rtl872xd);
}

LEVEL1_DRIVER_ENTRY(flash_rtl872xd_init)
