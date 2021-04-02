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

#include "aos/kernel.h"
#include "netmgr.h"
#include "ulog/ulog.h"
#include "aiconfig.h"
#include <uservice/uservice.h>
#include <uservice/eventid.h>
#include "aiagent_common.h"
#include "ugraphics.h"
#ifdef AOS_USE_BEEPER
#include "beeper.h"
#endif

static char linkkit_started = 0;

extern void ucloud_ai_demo_main(void *p);
extern int linkkit_main(void *paras);
extern void set_iotx_info(void);

extern void do_comparing_facebody_async(void);
extern void do_recognize_expression_async(void);

static void wifi_service_event(uint32_t event_id, const void *param, void *context)
{
    int ai_model = AI_MODEL;

    if (event_id != EVENT_NETMGR_SNTP_SUCCESS) {
        return;
    }

    if (!linkkit_started) {
        printf("start to do ucloud_ai_demo\n");
        aos_task_new("ucloud_ai_demo_main", ucloud_ai_demo_main, NULL, 1024 * 20);
        if (ai_model == AI_MODEL_COMPARING_FACEBODY)
            aos_task_new("linkkit", (void (*)(void *))linkkit_main, NULL, 1024 * 10);
        linkkit_started = 1;
    }
}

int application_start(int argc, char **argv)
{
    aos_set_log_level(AOS_LL_DEBUG);

    /*Init ugraphics component*/
    ugraphics_init(SCREEN_W, SCREEN_H);

    /*Set screen default color*/
    ugraphics_set_color(COLOR_RED);

    /*Load default font*/
    ugraphics_load_font("/data/font/Alibaba-PuHuiTi-Heavy.ttf", 18);

    /*Set default font style*/
    ugraphics_set_font_style(UGRAPHICS_FONT_STYLE);

#ifdef AOS_OLED_SH1106
    sh1106_init();
#endif

    /*init event service*/
    event_service_init(NULL);

    /*init network service*/
    netmgr_service_init(NULL);

    /*Subscribe wifi service event*/
    event_subscribe(EVENT_NETMGR_SNTP_SUCCESS, wifi_service_event, NULL);

    while (1) {
        aos_msleep(1000);
    };

    return 0;
}
