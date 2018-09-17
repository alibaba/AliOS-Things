/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>

extern int opus_test_main();

void opus_test(void *p){
    for(int i=0;i<100;i++){
        opus_test_main();
        aos_msleep(1000);
    }
}
int application_start(int argc, char *argv[])
{
    aos_task_new("pal init task", opus_test, NULL,1024*30);
    aos_loop_run();

    return 0;
}

