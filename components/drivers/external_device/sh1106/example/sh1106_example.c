/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#if AOS_COMP_CLI
#include "aos/cli.h"
#endif

#include "sh1106.h"

static void example_sh1106_init(int argc, char **argv)
{
    printf("sh1106 init test begin ...\r\n");
    sh1106_init();
    printf("sh1106 init test end !!!\r\n");
    return;
}

static void example_sh1106_display(int argc, char **argv)
{
    printf("sh1106 display test begin ...\r\n");
    sh1106_show_string(6,12*0,"Welcome to sh1106!",12,1);
    sh1106_show_string(24,(12+4)*1,"This is Haas!",12,1);
    sh1106_show_string(24,(12+4)*2,"2000-01-01 00:01",12,1);
    sh1106_show_string(24,(12+4)*3,"T:30^C H:70%",12,1);
    printf("sh1106 display test end !!!\r\n");
    return;
}

#if AOS_COMP_CLI
/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(example_sh1106_init, sh1106_init, sh1106 init test example)
ALIOS_CLI_CMD_REGISTER(example_sh1106_display, sh1106_display, sh1106 display test example)
#endif
