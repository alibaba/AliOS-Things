/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

/**
 * This file is created to support Linux style use of mtd device.
 * Currently only part definitions of linux's mtd-api.h are supported.
 */

#ifndef __AOS_MTD_ABI_H
#define __AOS_MTD_ABI_H

#include <stdint.h>

struct erase_info_user {
    uint32_t start;
    uint32_t length;
};

struct mtd_info_user {
    uint8_t  type;
    uint32_t flags;
    uint32_t size;       /* Total size of the MTD */
    uint32_t erasesize;
    uint32_t writesize;
    uint32_t oobsize;    /* Amount of OOB data per block (e.g. 16) */
    uint64_t padding;    /* Old obsolete field; do not use */
};

#endif /* __AOS_MTD_ABI_H */
