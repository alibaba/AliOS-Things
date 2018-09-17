/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>

#include "unity.h"
#include "espos_time.h"
#include "espos_task.h"

#define _ESPOS_TIME_DELAY_TICK     100
#define _ESPOS_TIME_DELAY_TICK_TH  120

TEST_CASE("ESPOS get tick test", "[espos]")
{
    espos_tick_t tick1, tick2;
    tick1 = espos_get_tick_count();
    espos_task_delay(_ESPOS_TIME_DELAY_TICK);
    tick2 = espos_get_tick_count();

    TEST_ASSERT(tick2 - tick1 >= _ESPOS_TIME_DELAY_TICK);
    TEST_ASSERT(tick2 - tick1 < _ESPOS_TIME_DELAY_TICK_TH);
}

TEST_CASE("ESPOS tick ms convert test", "[espos]")
{
    TEST_ASSERT(espos_ticks_to_ms(espos_ms_to_ticks(10)) == 10);

    TEST_ASSERT(espos_ms_to_ticks(0xFFFFFFFF) == 0xFFFFFFFF / 10);
}
