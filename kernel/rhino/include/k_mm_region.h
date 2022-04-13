/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef K_MM_REGION_H
#define K_MM_REGION_H

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

#endif /* K_MM_REGION_H */

