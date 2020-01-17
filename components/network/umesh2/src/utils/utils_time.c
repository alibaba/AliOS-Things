
/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include <stdint.h>
#include <stdio.h>
#include "osal.h"
#include "utils_time.h"
#include "define.h"

uint64_t umesh_time_spend(uint64_t start)
{
    uint64_t now, res;

    now = umesh_now_ms();
    res = now - start;
    return res;
}

uint64_t umesh_time_left(uint64_t end)
{
    uint64_t now, res;


    if (umesh_time_is_expired(end)) {
        return 0;
    }

    now = umesh_now_ms();
    res = end - now;
    return res;
}

int umesh_time_is_expired(uint64_t time)
{
    uint64_t cur_time;

    cur_time = umesh_now_ms();
    /*
     *  WARNING: Do NOT change the following code until you know exactly what it do!
     *
     *  check whether it reach destination time or not.
     */
    if ((cur_time - time) < (UINT64_MAX / 2)) {
        return 1;
    } else {
        return 0;
    }
}


int  umesh_time_countdown_ms(uint64_t *time, uint32_t millisecond)
{
    if (time == NULL) {
        return UMESH_ERR_NULL_POINTER;
    }
    *time = umesh_now_ms() + millisecond;
    return 0;
}

