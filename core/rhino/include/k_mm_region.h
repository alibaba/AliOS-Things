/**
 * @file k_mm_region.h
 *
 * @copyright Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef K_MM_REGION_H
#define K_MM_REGION_H

/** @addtogroup aos_rhino mm
 *
 *  @{
 */

/**
 * Heap regino parameter, for heap init.
 */
 typedef struct {
    uint8_t *start;
    size_t   len;
} k_mm_region_t;

/** @} */

#endif /* K_MM_REGION_H */

