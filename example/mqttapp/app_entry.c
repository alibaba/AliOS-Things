/*
 * Copyright (c) 2014-2016 Alibaba Group. All rights reserved.
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
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <aos/aos.h>
#include <aos/yloop.h>
#include "netmgr.h"

#ifdef AOS_ATCMD
#include <atparser.h>
#endif
#ifdef CSP_LINUXHOST
#include <signal.h>
#endif

typedef struct {
    int argc;
    char **argv;
}app_main_paras_t;

int linkkit_main(void *paras);

static char linkkit_started = 0;

static app_main_paras_t entry_paras;

typedef void (*task_fun)(void *);

static void wifi_service_event(input_event_t *event, void *priv_data)
{
    if (event->type != EV_WIFI) {
        return;
    }

    if (event->code != CODE_WIFI_ON_GOT_IP) {
        return;
    }

    netmgr_ap_config_t config;
    memset(&config, 0, sizeof(netmgr_ap_config_t));
    netmgr_get_ap_config(&config);
    LOG("wifi_service_event config.ssid %s", config.ssid);
    if (strcmp(config.ssid, "adha") == 0 || strcmp(config.ssid, "aha") == 0) {
        //clear_wifi_ssid();
        return;
    }

    if (!linkkit_started) {
        aos_task_new("iotx_example",(task_fun)linkkit_main,(void *)&entry_paras,1024*6);
        linkkit_started = 1;
    }
}

#ifdef TEST_LOOP
const  char *input_data[2]= {"mqttapp","loop"};
#endif
int application_start(int argc, char **argv)
{
#ifdef CSP_LINUXHOST
    signal(SIGPIPE, SIG_IGN);
#endif

#ifdef TEST_LOOP
    argc = 2;
    argv = (char **)input_data;
#endif    
    entry_paras.argc = argc;
    entry_paras.argv = argv;

#if AOS_ATCMD
    at.set_mode(ASYN);
    at.init(AT_RECV_PREFIX, AT_RECV_SUCCESS_POSTFIX,
            AT_RECV_FAIL_POSTFIX, AT_SEND_DELIMITER, 1000);
#endif


#ifdef WITH_SAL
    sal_init();
#endif

#ifdef MDAL_MAL_ICA_TEST
    HAL_MDAL_MAL_Init();
#endif

    aos_set_log_level(AOS_LL_DEBUG);

    netmgr_init();

    aos_register_event_filter(EV_WIFI, wifi_service_event, NULL);

#ifdef CONFIG_AOS_CLI

#endif
    netmgr_start(false);

    aos_loop_run();

    return 0;
}
