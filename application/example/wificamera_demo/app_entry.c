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
#include "graphics.h"
#ifdef AOS_COMP_LCD
#include "hal_lcd.h"
#endif

static char wifi_started = 0;

extern void wificamera_main(void *p);

static void wifi_service_event(input_event_t *event, void *priv_data)
{
    if (event->type != EV_WIFI) {
        return;
    }

    if (!wifi_started) {
        LOG("start to do wificamera_demo\n");

        aos_task_new("wificamera_process",wificamera_main, NULL, 1024*10);
        wifi_started = 1;
    }
}

int application_start(int argc, char **argv)
{
    aos_set_log_level(AOS_LL_DEBUG);
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
    netmgr_init();

    //aos_register_event_filter(EV_KEY, linkkit_key_process, NULL);
    aos_register_event_filter(EV_WIFI, wifi_service_event, NULL);

#ifdef CONFIG_AOS_CLI
    //aos_cli_register_command(&resetcmd);
    //aos_cli_register_command(&ncmd);
#endif
    netmgr_start(false);

    aos_loop_run();

    return 0;
}
