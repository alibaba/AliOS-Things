/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#ifndef AOS_FLASHPART_CORE_H
#define AOS_FLASHPART_CORE_H

#include <aos/device_core.h>
#include <aos/flashpart.h>

typedef struct {
    aos_dev_t dev;
    aos_flash_ref_t flash_ref;
    aos_flash_info_t flash_info;

    /* must be initialized before registration */
    uint32_t flash_id;
    uint64_t block_start;
    uint64_t block_count;
} aos_flashpart_t;

#ifdef __cplusplus
extern "C" {
#endif

aos_status_t aos_flashpart_register(aos_flashpart_t *flashpart);
aos_status_t aos_flashpart_register_argumented(aos_flashpart_t *flashpart, uint32_t id, uint32_t flash_id,
                                               uint32_t block_start, uint64_t block_count);
aos_status_t aos_flashpart_unregister(uint32_t id);

#ifdef __cplusplus
}
#endif

#endif /* AOS_FLASHPART_CORE_H */
