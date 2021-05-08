/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include "rx8130ce.h"
#include <stdint.h>
#if AOS_COMP_CLI
#include "aos/cli.h"
#endif

static void rx8130ce_comp_example(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    int ret = 0;
    uint8_t data[7] = {8, 25, 18, 7, 3, 1, 21};
    uint8_t data_rtn[7] = {0};
    uint16_t size = 7;
    uint8_t reg_rtn;

    printf("\r\n=====RTC test : set rtc time for RX8130CE=====\r\n");

    ret = rx8130ce_init();
    if (ret) {
        printf("=====RTC test : RX8130CE test: init FAIL=====\r\n");
        ret = -1;
    }

    /*ret = hal_rtc_set_time(NULL, (rtc_time_t*)data);*/
    ret = rx8130ce_set_time(data, sizeof(rtc_time_t));
    if (ret) {
        printf("=====RTC test : RX8130CE test: write FAIL=====\r\n");
        ret = -1;
    }

    osDelay(3000);

    printf("=====RTC test : RX8130CE test: get time=====\r\n");

    /*ret = hal_rtc_get_time(NULL, (rtc_time_t*)data_rtn);*/
    ret = rx8130ce_get_time(data_rtn, sizeof(rtc_time_t));
    if (ret) {
        printf("=====RTC test : RX8130CE test: write FAIL=====\r\n");
        ret = -1;
    }

    printf("get time: %us %um %uh %uw %ud %um %uy\r\n", (unsigned int)data_rtn[0],
            (unsigned int)data_rtn[1], (unsigned int)data_rtn[2],
            (unsigned int)data_rtn[3], (unsigned int)data_rtn[4],
            (unsigned int)data_rtn[5], (unsigned int)data_rtn[6]);

    for (uint8_t i = 0; i < 7; i++) {
        if (data[i] != data_rtn[i]) {
            if (((data[0] + 2) == data_rtn[0]) || ((data[0] + 3) == data_rtn[0])) {
                printf("=====RTC test : RX8130CE test: time is correct=====\r\n");
                continue;
            } else {
                printf("=====RTC test : RX8130CE test: time is wrong, set %u, get %u "
                        "=====\r\n",
                        (unsigned int)data[0], (unsigned int)data_rtn[0]);
                ret = -1;
            }

            if (i > 0) {
                printf("=====RTC test : RX8130CE test: loopback FAIL=====\r\n");
                ret = -1;
            }
        }
    }

    osDelay(20);

    if (ret) {
        printf("rx8130ce comp test failed!\r\n");
        return -1;
    } else {
        printf("rx8130ce comp test success!\r\n");
        return;
    }
}

#if AOS_COMP_CLI
/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(rx8130ce_comp_example, rx8130ce_example,
        rx8130ce component example)
#endif
