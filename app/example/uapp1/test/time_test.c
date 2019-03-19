/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <systime.h>

int time_test(void)
{
    systime_t tick;
    systime_t time;

    tick = sys_tick();
    printf("%s, cur sys tick 0x%llx\r\n", __func__, tick);

    time = tick_to_time(tick);
    printf("%s, cur sys time 0x%llx\r\n", __func__, time);

    time = sys_time();
    printf("%s, cur sys time 0x%llx\r\n", __func__, time);

    tick = time_to_tick(time);
    printf("%s, cur sys tick 0x%llx\r\n", __func__, tick);

    return 0;
}

