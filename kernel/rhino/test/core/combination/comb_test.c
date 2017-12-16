/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <test_fw.h>
#include "comb_test.h"

static const test_case_t comb_case_runner[] = {
    sem_event_coopr_test,
    sem_buf_queue_coopr_test,
    sem_mutex_coopr_test,
    comb_all_coopr_test,
    sem_queue_coopr_test,
    mutex_queue_coopr_test,
    mutex_buf_queue_coopr_test,
    NULL
};

void comb_test(void)
{
    if (test_case_register((test_case_t *)comb_case_runner) == 0) {
        test_case_run();
        test_case_unregister();
    }
}

