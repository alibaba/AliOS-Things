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
#include "aiagent_common.h"
//#include "uvoice_player.h"
//#include "uvoice_recorder.h"
//#include "uvoice_types.h"
#include "ulog/ulog.h"
#include "led.h"
#include "oled/oled.h"
#include "hal_iomux_haas1000.h"
#include "aos/hal/gpio.h"
#include <vfsdev/gpio_dev.h>
#include "player/player.h"

#define TAG "speech"


int application_start(int argc, char **argv)
{
    uint64_t current;
    int32_t ret = 0;
    gpio_dev_t gpio_pa_en;
    struct gpio_io_config gpio_config;
    gpio_dev_t pa_en_gpio;

    /*Enable extension board PA*/
    pa_en_gpio.port = HAL_IOMUX_PIN_P2_3; // HAL_IOMUX_PIN_P0_4;
    pa_en_gpio.config = OUTPUT_OPEN_DRAIN_PULL_UP;
    hal_gpio_init(&pa_en_gpio);
    hal_gpio_output_high(&pa_en_gpio);

    aos_set_log_level(AOS_LL_DEBUG);
    led_switch(LED1_NUM, LED_OFF);
    led_switch(LED2_NUM, LED_OFF);
    led_switch(LED3_NUM, LED_OFF);

    oled_init();
    oled_show(OLED_STR_IDLE);

    /*Init sound driver*/
    audio_install_codec_driver();

    /*Init uvoice*/
    ret = uvoice_init();
    if (ret < 0) {
        LOGE(TAG, "uvoice_init failed !\n");
        return -1;
    }
    /*Init player*/
    player_init(NULL);

    player_play(PLAYER_MP3_WELCOME);
    // player_wait_complete();
    /*Init tflite-micro speech engine*/
    setup();

    while (1) {
        loop();
    };

    return 0;
}
