/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */


#include "sdk-testsuites_internal.h"
#include "cut.h"
#include "ut_debug.h"

CASE(HAL_OS, HAL_UptimeMs) {
    uint64_t        before = 0;
    uint64_t        after = -1;
    int             result = 0;

    before = HAL_UptimeMs();
    HAL_SleepMs(5);
    after = HAL_UptimeMs();
    result = after - before - 5;

    ut_debug("before = HAL_UptimeMs() = %" PRIu64, before);
    ut_debug("after = HAL_UptimeMs() = %" PRIu64, after);
    ut_debug("result = HAL_UptimeMs() = %d", after-before);

    ASSERT_IN(-1, result, 1);
}

SUITE(HAL_OS) = {
    ADD_CASE(HAL_OS, HAL_UptimeMs),
    ADD_CASE_NULL
};
