/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */
#include "stdlib.h"
#include <aos/kernel.h>
#ifdef AOS_COMP_CLI
#include "aos/cli.h"
#endif

extern int amp_main(void);

static void amp_entry(void * arg) {
    int ret;
    (void *)arg;

    ret = amp_main();
    if (ret != 0) {
        printf("start amp task error. ret = %d\r\n", ret);
    }
}

static void amp_comp_example(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    aos_task_t amp_task;

    aos_task_new_ext(&amp_task, "amp_task", amp_entry, NULL, 8192, AOS_DEFAULT_APP_PRI - 2);

    printf("amp comp test success!\r\n");
    return;
}

#ifdef AOS_COMP_CLI
/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(amp_comp_example, amp_example, amp component base example)
#endif
