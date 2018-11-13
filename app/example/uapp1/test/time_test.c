/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <u_api.h>

int time_test(void)
{
    sys_time_t sys_tick;
    sys_time_t sys_time;

    sys_tick = krhino_sys_tick_get();
    printf("%s, cur sys tick 0x%x\r\n", __func__, sys_tick);

    sys_time = krhino_ticks_to_ms(sys_tick);
    printf("%s, cur sys time 0x%x\r\n", __func__, sys_time);

    sys_time = krhino_sys_time_get();
    printf("%s, cur sys time 0x%x\r\n", __func__, sys_time);

    sys_tick = krhino_ms_to_ticks(sys_time);
    printf("%s, cur sys tick 0x%x\r\n", __func__, sys_tick);

    return 0;
}

