/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef __AOS_MTDNOR_H
#define __AOS_MTDNOR_H

#include <aos/mtd.h>

int aos_mtd_nor_init(aos_mtd_t *nor, int blksize, int sector_size);

#endif /*__AOS_MTDNOR_H*/
