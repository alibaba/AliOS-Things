/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <setjmp.h>

#include "ulog/ulog.h"

jmp_buf buf_jump_test;

void banana(void)
{
    LOGI(TAG, "running banana()");

    /* jump and set value 1 as the return value of setjmp */
    longjmp(buf_jump_test, 1);

    LOGI(TAG, "you will never see this, because i will longjmp !");
}

void posix_jump_test_case(void)
{
    int ret = -1;

    /* set jump point */
    ret = setjmp(buf_jump_test);

    /* setjmp will return 0 at the first time, when jump back the
       return value will be decided by para of longjmp */
    if (ret == 0) {
        LOGI(TAG, "first time through");
        banana();
    } else {
    /* the second para of longjmp is 1, so ret = 1, and this branch will run */
        LOGI(TAG, "jump back");
        LOGI(TAG, "posix jump test OK !");
    }
}
