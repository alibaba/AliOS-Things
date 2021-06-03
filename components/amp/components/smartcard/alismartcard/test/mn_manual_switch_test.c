/*
 * Copyright (c) 2019 Alibaba Group. All rights reserved.
 * License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Description:
 * mn_manual_switch_test.c: smarcard SDK test code
 *
 */
#include <signal.h>         /* sigaction */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "aliyun_iot_smartcard_export.h"

bool is_global_exit = false;
static int select_test(int id);

static void sig_handler(int sigio)
{
    is_global_exit = true;
    exit(0);
}

static char cmd[128];
static char subcmd[128];

static int print_usage()
{
    printf("\n< ali smart card demo app >\n");
    printf("help\n");
    printf("exit\n");
    printf("mn setpath init deinit select[1 2 3] check stress\n");
    return 0;
}

static int set_path_test(void)
{
    int ret;

    ret = aliyun_iot_smartcard_set_config_file("/tmp/ali_smartcard.json");

    if (ret != 0) {
        printf("aliyun_iot_smartcard_set_config_file fail, ret = %d!\n", ret);
    } else {
        printf("aliyun_iot_smartcard_set_config_file done!\n");
    }

    return ret;
}

static int init_test(void)
{
    int ret;

    ret = aliyun_iot_smartcard_init();

    if (ret != 0) {
        printf("fail to init smart card, ret = %d!\n", ret);
    } else {
        printf("init smart card done!\n");
    }

    return ret;
}

static int deinit_test(void)
{
    int ret;

    ret = aliyun_iot_smartcard_deinit();

    if (ret != 0) {
        printf("fail to deinit smart card, ret = %d!\n", ret);
    } else {
        printf("deinit smart card done!\n");
    }

    return ret;
}

static int check_test(void)
{
    int ret;
    ret = aliyun_iot_is_smartcard();

    if (ret != 0) {
        printf("not smart card, ret = %d!\n", ret);
    } else {
        printf("is smart card!\n");
    }

    return ret;
}

static int stress_test(void)
{
    int i;
    int ret;
    int fail_cnt = 0;

    for (i = 0; i < 10; i++) {
        ret = select_test(1);

        if (ret != 0) {
            fail_cnt++;
        }

        ret = select_test(2);

        if (ret != 0) {
            fail_cnt++;
        }

        ret = select_test(3);

        if (ret != 0) {
            fail_cnt++;
        }
    }

    if (fail_cnt != 0) {
        printf("stress test fail, fail cnt %d!\n", fail_cnt);
    } else {
        printf("stress test pass!\n");
    }

    return 0;
}

static int select_test(int id)
{
    int ret;

    ret = aliyun_iot_smartcard_change_operator(id);

    if (ret != 0) {
        printf("fail to switch card%d, ret = %d!\n", id, ret);
    } else {
        printf("switch card%d done!\n", id);
    }

    return ret;
}

int main(int argc, char *argv[])
{
    int ret;
    struct sigaction sigact; /* SIGQUIT&SIGINT&SIGTERM signal handling */

    print_usage();

    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = 0;
    sigact.sa_handler = sig_handler;
    sigaction(SIGQUIT, &sigact, NULL); /* Ctrl-\ */
    sigaction(SIGINT, &sigact, NULL); /* Ctrl-C */
    sigaction(SIGTERM, &sigact, NULL); /* default "kill" command */

    //main loop
    while (!is_global_exit) {

        ret = scanf("%127s", cmd);

        if (ret == EOF) {
            perror("scanf");
            continue;
        }

        if (strncmp(cmd, "help", strlen("help")) == 0) {
            print_usage();
        } else if (strncmp(cmd, "exit", strlen("exit")) == 0) {
            exit(0);
        } else if (strncmp(cmd, "mn", strlen("mn")) == 0) {
            ret = scanf("%127s", subcmd);

            if (ret == EOF) {
                perror("scanf");
                continue;
            }

            if (strncmp(subcmd, "setpath", strlen("setpath")) == 0) {
                set_path_test();
            } else if (strncmp(subcmd, "init", strlen("init")) == 0) {
                init_test();
            } else if (strncmp(subcmd, "deinit", strlen("deinit")) == 0) {
                deinit_test();
            } else if (strstr(subcmd, "check")) {
                check_test();
            } else if (strstr(subcmd, "select1")) {
                select_test(1);
            } else if (strstr(subcmd, "select2")) {
                select_test(2);
            } else if (strstr(subcmd, "select3")) {
                select_test(3);
            } else if (strstr(subcmd, "stress")) {
                stress_test();
            } else {
                print_usage();
            }
        } else {
            print_usage();
        }
    }

    printf("manual switch test done!\n");

    return 0;
}
