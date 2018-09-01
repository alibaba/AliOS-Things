/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _TEST_ESPOS_RUN_IN_ISR_H_
#define _TEST_ESPOS_RUN_IN_ISR_H_

#include "espos_types.h"
#include "esp_attr.h"

#ifdef __cplusplus
    extern "C" {
#endif


typedef void (*espos_test_isr_routine)(void);

/*
 * @brief run handler in timer isr
 */
void espos_test_run_in_isr(espos_test_isr_routine routine);

#ifdef __cplusplus
    }
#endif

#endif
