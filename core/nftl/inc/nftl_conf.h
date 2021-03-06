/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef NFTL_CONF_H
#define NFTL_CONF_H

#include "aos/hal/flash.h"

#define NFTL_ERASED_VALUE 0xff

/* ===== logical block related definitions ===== */
/* The page size, in bytes, of the flash used for NFTL storage. */
#ifndef NFTL_CONFIG_PAGE_SIZE
#define NFTL_PAGE_SIZE (4 * 1024)
#else
#define NFTL_PAGE_SIZE NFTL_CONFIG_PAGE_SIZE
#endif

/* page number per block */
#ifndef NFTL_CONFIG_PAGE_PER_BLOCK_NUMS
#define NFTL_PAGE_PER_BLOCK_NUMS  64
#else
#define NFTL_PAGE_PER_BLOCK_NUMS NFTL_CONFIG_PAGE_PER_BLOCK_NUMS
#endif

/* nftl logical partition numbers.
 * TODO: modify below lines if partition numbers changed.
 */
#ifndef NFTL_CONFIG_LOGICAL_PARTITION_NUM
#define NFTL_LOGICAL_PARTITION_NUM 3
#else
#define NFTL_LOGICAL_PARTITION_NUM NFTL_CONFIG_LOGICAL_PARTITION_NUM
#endif

/* TODO: modify below lines if flash layput changed. */
#define NFTL_PHYSICAL_PARTITION0 HAL_PARTITION_LITTLEFS2 /* system A */
#define NFTL_PHYSICAL_PARTITION1 HAL_PARTITION_LITTLEFS3 /* system B */
#define NFTL_PHYSICAL_PARTITION2 HAL_PARTITION_LITTLEFS /* DATA */

/* TODO: Modify below lines if filesystem's raw partition size changed.
 * Below lines must be the same as nftl user's settings,
 * e.g littefs system/data size (in blocks).
 */
#define NFTL_PART0_PHY_BLOCK_NUMS 272
#define NFTL_PART1_PHY_BLOCK_NUMS NFTL_PART0_PHY_BLOCK_NUMS
#define NFTL_PART2_PHY_BLOCK_NUMS 1356
#define NFTL_PART0_RAW_BLOCK_NUMS 256
#define NFTL_PART1_RAW_BLOCK_NUMS NFTL_PART0_RAW_BLOCK_NUMS
#define NFTL_PART2_RAW_BLOCK_NUMS 1312
#define NFTL_TOTAL_RAW_BLOCK_NUMS (NFTL_PART0_RAW_BLOCK_NUMS + NFTL_PART1_RAW_BLOCK_NUMS + NFTL_PART2_RAW_BLOCK_NUMS)

/* nftl logical partition 0 blocks, i.e. after 'raw + tagpage' convertion. */
#define NFTL_PART0_BLOCK_NUMS ((NFTL_PART0_RAW_BLOCK_NUMS * NFTL_PAGE_PER_BLOCK_NUMS / (NFTL_PAGE_PER_BLOCK_NUMS - 1) + \
        NFTL_PART0_RAW_BLOCK_NUMS * NFTL_PAGE_PER_BLOCK_NUMS + \
        (NFTL_PAGE_PER_BLOCK_NUMS - 1)) / NFTL_PAGE_PER_BLOCK_NUMS)

/* buffer blocks in nftl partition, used for bad block replacement.
 * It's proposed to be 5% used for bad block.
 */
#ifndef NFTL_CONFIG_PART0_BUFFER_BLOCK_NUMS
#define NFTL_PART0_BUFFER_BLOCK_NUMS (NFTL_PART0_PHY_BLOCK_NUMS - NFTL_PART0_BLOCK_NUMS)//((NFTL_PART0_BLOCK_NUMS >> 3) >= 2 ? (NFTL_PART0_BLOCK_NUMS >> 3): 2)
#else
#define NFTL_PART0_BUFFER_BLOCK_NUMS NFTL_CONFIG_PART0_BUFFER_BLOCK_NUMS
#endif

/* nftl logical partition 1 blocks, i.e. after 'raw + tagpage' convertion. */
#define NFTL_PART1_BLOCK_NUMS ((NFTL_PART1_RAW_BLOCK_NUMS * NFTL_PAGE_PER_BLOCK_NUMS / (NFTL_PAGE_PER_BLOCK_NUMS - 1) + \
        NFTL_PART1_RAW_BLOCK_NUMS * NFTL_PAGE_PER_BLOCK_NUMS + \
        (NFTL_PAGE_PER_BLOCK_NUMS - 1)) / NFTL_PAGE_PER_BLOCK_NUMS)

#ifndef NFTL_CONFIG_PART1_BUFFER_BLOCK_NUMS
#define NFTL_PART1_BUFFER_BLOCK_NUMS (NFTL_PART1_PHY_BLOCK_NUMS - NFTL_PART1_BLOCK_NUMS)//((NFTL_PART1_BLOCK_NUMS >> 3) >= 2 ? (NFTL_PART1_BLOCK_NUMS >> 3): 2)
#else
#define NFTL_PART1_BUFFER_BLOCK_NUMS NFTL_CONFIG_PART1_BUFFER_BLOCK_NUMS
#endif

/* nftl logical partition 2 blocks, i.e. after 'raw + tagpage' convertion. */
#define NFTL_PART2_BLOCK_NUMS ((NFTL_PART2_RAW_BLOCK_NUMS * NFTL_PAGE_PER_BLOCK_NUMS / (NFTL_PAGE_PER_BLOCK_NUMS - 1) + \
        NFTL_PART2_RAW_BLOCK_NUMS * NFTL_PAGE_PER_BLOCK_NUMS + \
        (NFTL_PAGE_PER_BLOCK_NUMS - 1)) / NFTL_PAGE_PER_BLOCK_NUMS)

#ifndef NFTL_CONFIG_PART2_BUFFER_BLOCK_NUMS
#define NFTL_PART2_BUFFER_BLOCK_NUMS (NFTL_PART2_PHY_BLOCK_NUMS - NFTL_PART2_BLOCK_NUMS)//((NFTL_PART2_BLOCK_NUMS >> 3) >= 2 ? (NFTL_PART2_BLOCK_NUMS >> 3): 2)
#else
#define NFTL_PART2_BUFFER_BLOCK_NUMS NFTL_CONFIG_PART2_BUFFER_BLOCK_NUMS
#endif

/* TODO: modify below line if more logical partition added */
#if NFTL_LOGICAL_PARTITION_NUM == 1
#define NFTL_LOGICAL_TOTAL_BLOCK_NUMS (NFTL_PART0_BLOCK_NUMS)
#define NFTL_BUFFER_TOTAL_BLOCK_NUMS (NFTL_PART0_BUFFER_BLOCK_NUMS)
#elif NFTL_LOGICAL_PARTITION_NUM == 2
#define NFTL_LOGICAL_TOTAL_BLOCK_NUMS (NFTL_PART0_BLOCK_NUMS + NFTL_PART1_BLOCK_NUMS)
#define NFTL_BUFFER_TOTAL_BLOCK_NUMS (NFTL_PART0_BUFFER_BLOCK_NUMS + NFTL_PART1_BUFFER_BLOCK_NUMS)
#elif NFTL_LOGICAL_PARTITION_NUM == 3
#define NFTL_LOGICAL_TOTAL_BLOCK_NUMS (NFTL_PART0_BLOCK_NUMS + NFTL_PART1_BLOCK_NUMS + NFTL_PART2_BLOCK_NUMS)
#define NFTL_BUFFER_TOTAL_BLOCK_NUMS (NFTL_PART0_BUFFER_BLOCK_NUMS + NFTL_PART1_BUFFER_BLOCK_NUMS + NFTL_PART2_BUFFER_BLOCK_NUMS)
#elif
#error Please define more NFTL logical paritions (now 3 but need more)!
#endif


/* ===== physical related definitions ===== */

/* total physical block numbers in FTL partition. */
#define NFTL_WORK_BLOCK_NUMS (NFTL_LOGICAL_TOTAL_BLOCK_NUMS + NFTL_BUFFER_TOTAL_BLOCK_NUMS)

#define NFTL_BLOCK_NUMS NFTL_WORK_BLOCK_NUMS

/* The block size, in bytes, of the flash used for NFTL storage. */
#define NFTL_BLOCK_SIZE (NFTL_PAGE_PER_BLOCK_NUMS * NFTL_PAGE_SIZE)

/* The total nftl storage size. */
#define NFTL_TOTAL_SIZE (NFTL_BLOCK_NUMS * NFTL_BLOCK_SIZE)

#endif  /* NFTL_CONF_H */

