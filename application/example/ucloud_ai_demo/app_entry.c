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
#include <aos/yloop.h>
#include "netmgr.h"
#include "ulog/ulog.h"
#include "linkkit/wifi_provision_api.h"
#include "linkkit/infra/infra_compat.h"
#include "linkkit/infra/infra_defs.h"
#include "linkkit/wrappers/wrappers.h"
#include "sdkconfig.h"
#include "ucloud_ai.h"
#include "graphics.h"
#include "ucloud_ai_demo.h"
#ifdef AOS_COMP_LCD
#include "hal_lcd.h"
#endif
#ifdef AOS_USE_BEEPER
#include "beeper.h"
#endif

static char linkkit_started = 0;

extern void ucloud_ai_demo_main(void *p);
extern int linkkit_main(void *paras);
extern void set_iotx_info(void);
extern void set_ai_type(ai_model_t type);

extern void do_comparing_facebody_async(void);
extern void do_recognize_expression_async(void);

static void wifi_service_event(input_event_t *event, void *priv_data)
{
    if (event->type != EV_WIFI) {
        return;
    }

    if (event->code != CODE_WIFI_ON_GOT_SNTP)
        return;

    if (!linkkit_started) {
        LOG("start to do ucloud_ai_demo\n");
        aos_task_new("ucloud_ai_demo_main",ucloud_ai_demo_main, NULL, 1024*10);
        if (get_ucloud_ai_model() == AI_MODEL_COMPARING_FACEBODY)
            aos_task_new("linkkit", (void (*)(void *))linkkit_main, NULL, 1024 * 10);
        linkkit_started = 1;
    }
}

static void wificamera_service_event(input_event_t *event, void *priv_data)
{
    int ret;
    unsigned long value = *((unsigned long*)priv_data);

    if (event->type != EV_NETWORK) {
        return;
    }

    if (event->code != CODE_ON_GOT_WIFICAMERA_IMAGE) {
        return;
    }

    /*upload to cloud to compare facebody*/
    if (get_ucloud_ai_model() == AI_MODEL_COMPARING_FACEBODY)
        do_comparing_facebody_async();
    else if (get_ucloud_ai_model() == AI_MODEL_RECOGNIZE_EXPRESSION)
        do_recognize_expression_async();
}

int application_start(int argc, char **argv)
{
    aos_set_log_level(AOS_LL_DEBUG);
    /*set ai model*/
    set_ucloud_ai_model(AI_MODEL);

#ifdef AOS_COMP_SDL2
    graphics_initialize();
#else
#ifdef AOS_COMP_LCD
    uint8_t *frame = malloc(SCREEN_W*SCREEN_H*2);
    hal_lcd_t *hal_lcd = get_hal_lcd();
    hal_lcd->lcd_init();
    hal_lcd->lcd_rotation_set(HAL_LCD_ROTATE_90);
    /*Clear lcd frame*/
    memset(frame, 0xffff, SCREEN_W*SCREEN_H*2);
    hal_lcd->lcd_frame_draw(frame);
    free(frame);
#endif
#endif
#ifdef AOS_OLED_SH1106
	sh1106_init();
#endif

    /*set iot information*/
    set_iotx_info();
    netmgr_init();

    //aos_register_event_filter(EV_KEY, linkkit_key_process, NULL);
    aos_register_event_filter(EV_WIFI, wifi_service_event, NULL);
    aos_register_event_filter(EV_NETWORK, wificamera_service_event, NULL);

    netmgr_start(false);

    aos_loop_run();

    return 0;
}
