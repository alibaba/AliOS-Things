/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "aos/kernel.h"
#include <stdio.h>
#include "activation.h"


void showBuffer( UINT8 *buffer, UINT32 len )
{
    int i;
    for ( i = 0; i < len; i++) {
        printf( "0x%x ", buffer[i]);
    }
    printf("\n");
}
int get_version_test_ret()
{
    UINT8 output_buffer[OUTPUT_SPACE_SIZE] = {0};

    UINT8 version_num[4] = {0x2, 0x10, 0x4, 0x22};
    UINT8 random_num[4] = {0x21, 0x22, 0x20, 0x19};
    UINT8 mac_address[8] = {0x0, 0x1, 0x0, 0x1, 0x2F, 0x3C, 0x0E, 0x2};
    UINT8 chip_code[4] = {0x0, 0x0, 0x3, 0x2};
    UINT32 ret = aos_get_version_info( version_num, random_num, mac_address, chip_code,
                                       output_buffer, OUTPUT_SPACE_SIZE);
    if ( 0 != ret ) {
        return 1;
    }
    printf("aos_get_version_info ret: %d\n", ret );
    printf("output_buffer: %s\n", output_buffer);

    //0210042221222019000100012F3C0E0200000302PLAIN
    char *except_result="0210042221222019000100012F3C0E0200000302PLAIN";
    if( strcmp(except_result,output_buffer) == 0 )
    {
        printf( "version_verify result: successed\n");
        return 0;
    }
    else
    {
        return 2;
    }
}

static void app_delayed_action(void *arg)
{
    printf("%s", arg);
    aos_post_delayed_action(3000, app_delayed_action, arg);
}

int application_start(int argc, char *argv[])
{
    int ret = get_version_test_ret();
    const char *retStr;
    if ( ret == 1 ) {
        retStr = "version_verify result: fail1\n";
    } else if ( ret == 2 ) {
        retStr = "version_verify result: fail2\n";
    } else {
        retStr = "version_verify result: successed\n";
    }

    aos_post_delayed_action(1000, app_delayed_action, retStr);
    aos_loop_run();
    return ret;
}

