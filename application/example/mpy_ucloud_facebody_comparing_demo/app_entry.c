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

static int flag = 0;

static void wifi_service_event(input_event_t *event, void *priv_data)
{
    if (event->type != EV_WIFI) {
        return;
    }

    if (event->code != CODE_WIFI_ON_GOT_SNTP)
        return;

    if (!flag) {
        int arg_num =  2 ;
        char* argvs[] = {"micropython","/data/FacebodyComparingDemo.py"} ;

        printf(" micropython:ucloud_demo start\r\n");
        mpy_init();
        mpy_run(arg_num,argvs);
        printf(" micropython:ucloud_demo end\r\n");
        flag = 1;
    }
}


int application_start(int argc, char **argv)
{
    netmgr_init();

    aos_register_event_filter(EV_WIFI, wifi_service_event, NULL);

    netmgr_start(false);

    aos_loop_run();

    return 0;
}
