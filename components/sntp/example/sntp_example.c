/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include "sntp/sntp.h"
#if AOS_COMP_CLI
#include "aos/cli.h"
#endif

static void sntp_comp_example(int argc, char **argv)
{
    printf("sntp test \r\n");

    sntp_request(NULL);

    sntp_time *sntp_time_get;
    sntp_time_get = sntp_obtain_time();

    printf("%d-%d-%d: week: %d, %d:%d:%d\r\n",
                 sntp_time_get->year,
                 sntp_time_get->mon,
                 sntp_time_get->day,
                 sntp_time_get->week,
                 sntp_time_get->hour,
                 sntp_time_get->min,
                 sntp_time_get->sec);
}


#if AOS_COMP_CLI
/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(sntp_comp_example, sntp_example, sntp component base example)
#endif
