/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "sdk-testsuites_internal.h"

CASE(HAL_OS, HAL_UptimeMs)
{
    uint64_t        before = 0;
    uint64_t        after = -1;
    int             result = 0;
    int             interval_ms = 5;

    before = HAL_UptimeMs();
    HAL_SleepMs(interval_ms);
    after = HAL_UptimeMs();
    result = after - before - interval_ms;

    ut_debug("before:   HAL_UptimeMs() = %" PRIu64, before);
    ut_debug("after:    HAL_UptimeMs() = %" PRIu64, after);
    ut_info("result:   after - before - %d = %d", interval_ms, result);

    ASSERT_IN(-1, result, 1);
}

SUITE(HAL_OS) = {
    ADD_CASE(HAL_OS, HAL_UptimeMs),
    ADD_CASE_NULL
};

