/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <aos/kernel.h>
#include "aos/init.h"
#include "board.h"
#include <k_api.h>
#include "haascv_main.h"
int application_start(int argc, char *argv[])
{
    //arg_num 为传入参数个数
    int arg_num =  1 ;
    //argvs 是传入参数列表
    char* argvs[] = {"micropython"} ;
    printf("repl_demo start\r\n");
    mpy_init();
    mpy_add_path("/data",5);
    mpy_run(arg_num,argvs);
    printf("repl_demo end\r\n");

}
