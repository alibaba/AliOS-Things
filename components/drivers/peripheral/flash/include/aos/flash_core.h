/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#ifndef AOS_FLASH_CORE_H
#define AOS_FLASH_CORE_H

#include <aos/device_core.h>
#include <aos/flash.h>

struct aos_flash_ops;

typedef struct {
    aos_dev_t dev;

    /* must be initialized before registration */
    const struct aos_flash_ops *ops;
    uint32_t flags;
    uint64_t block_count;
    size_t pages_per_block;
    size_t page_data_size;
    size_t page_spare_size;
    void *page_data_buf;
    void *page_spare_buf;
} aos_flash_t;

typedef struct aos_flash_ops {
    void (*unregister)(aos_flash_t *);
    aos_status_t (*startup)(aos_flash_t *);
    void (*shutdown)(aos_flash_t *);
    aos_status_t (*read_page)(aos_flash_t *, uint64_t, size_t, size_t, size_t, size_t);
    aos_status_t (*write_page)(aos_flash_t *, uint64_t, size_t, size_t, size_t, size_t);
    aos_status_t (*erase_block)(aos_flash_t *, uint64_t);
    aos_status_t (*is_bad_block)(aos_flash_t *, uint64_t);
    aos_status_t (*mark_bad_block)(aos_flash_t *, uint64_t);
} aos_flash_ops_t;

#ifdef __cplusplus
extern "C" {
#endif

aos_status_t aos_flash_register(aos_flash_t *flash);
aos_status_t aos_flash_register_argumented(aos_flash_t *flash, uint32_t id, const aos_flash_ops_t *ops,
                                           uint32_t flags, uint64_t block_count, size_t pages_per_block,
                                           size_t page_data_size, size_t page_spare_size,
                                           void *page_data_buf, void *page_spare_buf);
aos_status_t aos_flash_unregister(uint32_t id);

#ifdef __cplusplus
}
#endif

#endif /* AOS_FLASH_CORE_H */
