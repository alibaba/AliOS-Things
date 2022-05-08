/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#include <string.h>
#include <cmsis.h>
#include <aos/flash_core.h>
#include <hal_norflash.h>
#include <pmu.h>

typedef struct {
    aos_flash_t flash;
    uint8_t page_data_buf[256];
} flash_haas1000_t;

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

static void *bes_flash_memcpy(void *dst, const void *src, size_t num)
{
    int offset1 = (4 - ((uint32)dst & 3)) & 3;
    int offset2 = (4 - ((uint32)src & 3)) & 3;

    if (offset1 != offset2)
        return memcpy(dst, src, num);

    int wordnum = num > offset1 ? (num - offset1) / 8 : 0;
    int slice = num > offset1 ? (num - offset1) % 8 : 0;
    char *pdst = (char *)dst;
    const char *psrc = (const char *)src;
    long long *pintdst;
    const long long *pintsrc;

    while (offset1--)
        *pdst++ = *psrc++;

    pintdst = (long long *)pdst;
    pintsrc = (const long long *)psrc;

    while (wordnum--)
        *pintdst++ = *pintsrc++;

    pdst = (char *)pintdst;
    psrc = (const char *)pintsrc;

    while (slice--)
        *pdst++ = *psrc++;

    return dst;
}

void watchdog_feeddog(void);

static aos_status_t flash_read_page(aos_flash_t *flash, uint64_t page, size_t data_offset, size_t data_count,
                                    size_t spare_offset, size_t spare_count)
{
    uint32_t lock;
    uint32_t offset;

    watchdog_feeddog();
    lock = int_lock();
    hal_norflash_disable_remap(HAL_NORFLASH_ID_0);
    offset = (uint32_t)(page * flash->page_data_size + data_offset);
    bes_flash_memcpy((char *)flash->page_data_buf + data_offset, (const void *)(FLASH_NC_BASE + offset), data_count);
    hal_norflash_re_enable_remap(HAL_NORFLASH_ID_0);
    int_unlock(lock);

    return 0;
}

static aos_status_t flash_write_page(aos_flash_t *flash, uint64_t page, size_t data_offset, size_t data_count,
                                     size_t spare_offset, size_t spare_count)
{
    uint32_t lock;
    uint32_t offset;
    enum HAL_NORFLASH_RET_T r;

    watchdog_feeddog();
    lock = int_lock();
    pmu_flash_write_config();
    hal_norflash_disable_remap(HAL_NORFLASH_ID_0);
    offset = (uint32_t)(page * flash->page_data_size + data_offset);
    r = hal_norflash_write(HAL_NORFLASH_ID_0, offset, (const uint8_t *)flash->page_data_buf + data_offset, data_count);
    hal_norflash_re_enable_remap(HAL_NORFLASH_ID_0);
    pmu_flash_read_config();
    int_unlock(lock);

    return (r == HAL_NORFLASH_OK) ? 0 : -EIO;
}

static aos_status_t flash_erase_block(aos_flash_t *flash, uint64_t block)
{
    uint32_t lock;
    uint32_t block_size;
    enum HAL_NORFLASH_RET_T r;

    watchdog_feeddog();
    lock = int_lock();
    pmu_flash_write_config();
    hal_norflash_disable_remap(HAL_NORFLASH_ID_0);
    block_size = flash->pages_per_block * flash->page_data_size;
    r = hal_norflash_erase(HAL_NORFLASH_ID_0, (uint32_t)(block * block_size), block_size);
    hal_norflash_re_enable_remap(HAL_NORFLASH_ID_0);
    pmu_flash_read_config();
    int_unlock(lock);

    return (r == HAL_NORFLASH_OK) ? 0 : -EIO;
}

static const aos_flash_ops_t flash_haas1000_ops = {
    .unregister     = flash_unregister,
    .startup        = flash_startup,
    .shutdown       = flash_shutdown,
    .read_page      = flash_read_page,
    .write_page     = flash_write_page,
    .erase_block    = flash_erase_block,
    .is_bad_block   = NULL,
    .mark_bad_block = NULL,
};

static aos_status_t flash_haas1000_register(flash_haas1000_t *flash_haas1000)
{
    if (!flash_haas1000)
        return -EINVAL;

    flash_haas1000->flash.ops = &flash_haas1000_ops;
    flash_haas1000->flash.flags = AOS_FLASH_F_TYPE_NOR;
    flash_haas1000->flash.block_count = 4096;
    flash_haas1000->flash.pages_per_block = 16;
    flash_haas1000->flash.page_data_size = sizeof(flash_haas1000->page_data_buf);
    flash_haas1000->flash.page_spare_size = 0;
    flash_haas1000->flash.page_data_buf = flash_haas1000->page_data_buf;
    flash_haas1000->flash.page_spare_buf = NULL;

    return aos_flash_register(&flash_haas1000->flash);
}

static int flash_haas1000_init(void)
{
    static flash_haas1000_t flash_haas1000;

    flash_haas1000.flash.dev.id = 0;

    return (int)flash_haas1000_register(&flash_haas1000);
}

LEVEL1_DRIVER_ENTRY(flash_haas1000_init)
