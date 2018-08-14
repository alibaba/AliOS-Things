/**
 * @file benchmark.h
 *
 */

#ifndef BENCHMARK_H
#define BENCHMARK_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../lv_ex_conf.h"
#if USE_LV_BENCHMARK
    
#include "lvgl/lvgl.h"
    
/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Open a graphics benchmark
 */
void benchmark_create(void);

void benchmark_start(void);

bool benchmark_is_ready(void);

uint32_t benchmark_get_refr_time(void);

/**********************
 *      MACROS
 **********************/

#endif /*USE_LV_BENCHMARK*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* BENCHMARK_H */
