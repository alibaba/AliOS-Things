/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <stdint.h>

#ifndef __AOS_PART_H
#define __AOS_PART_H

typedef struct {
    const char  *partition_name;       /* vendor custom partition name */
    const char  *partition_name_std;   /* standard partition name */
    uint32_t     partition_start_addr; /* partition start address*/
    uint32_t     partition_length;     /* partition length */
    uint32_t     partition_options;    /* reserved*/
} mtd_partition_t;

extern const mtd_partition_t mtd_partitions[];
extern const int mtd_partitions_amount;

#endif /*__AOS_PART_H*/