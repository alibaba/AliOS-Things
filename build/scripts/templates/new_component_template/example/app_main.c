/*
 * Copyright (C) 2015-2020 @manufacturename@
 */

#include <stdio.h>
#include <aos/kernel.h>
#include <@componentname@.h>

int application_start(int argc, char *argv[])
{
    @componentname@_init();
    /*add your code*/

    while(1) {
        /*add your code*/
        /*printf("helloworld\r\n");*/

        aos_msleep(1000);
    };
}
