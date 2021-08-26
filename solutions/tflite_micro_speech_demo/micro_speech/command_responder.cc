/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/
#include <stdio.h>
#include <string.h>
#include "micro_speech/command_responder.h"
#include "micro_speech/micro_features/micro_model_settings.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus
#include <ulog/ulog.h>
#include "led.h"
#include "player/player.h"
#include "oled/oled.h"
#include "aos/kernel.h"
#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

static aos_task_t cmd_task_handle;
static aos_sem_t cmd_sem;
static int control_type = 0;

static void command_process_main(void *p)
{
    while (1) {
        oled_show(OLED_STR_IDLE);
        aos_sem_wait(&cmd_sem, AOS_WAIT_FOREVER);
        // player_wait_complete();
        if (control_type == 1) {
            led_switch(LED1_NUM, LED_ON);
            led_switch(LED2_NUM, LED_ON);
            led_switch(LED3_NUM, LED_ON);
            oled_show(OLED_STR_LIGHT_ON);
            player_play(PLAYER_MP3_LIGHT_ON);
        } else if (control_type == 2) {
            led_switch(LED1_NUM, LED_OFF);
            led_switch(LED2_NUM, LED_OFF);
            led_switch(LED3_NUM, LED_OFF);
            oled_show(OLED_STR_LIGHT_OFF);
            player_play(PLAYER_MP3_LIGHT_OFF);
        } else {
            oled_show(OLED_STR_IDLE);
        }
    }
}

void RespondCommandThreadInit(void)
{
    aos_sem_new(&cmd_sem, 0);
    aos_task_new_ext(&cmd_task_handle,
      "cmd_callback", command_process_main,
      NULL, 1024 * 10, AOS_DEFAULT_APP_PRI);
}

// The default implementation writes out the name of the recognized command
// to the error console. Real applications will want to take some custom
// action instead, and should implement their own versions of this function.
void RespondToCommand(tflite::ErrorReporter* error_reporter,
                      int32_t current_time, const char* found_command,
                      uint8_t score, bool is_new_command) {
    if (is_new_command) {
        TF_LITE_REPORT_ERROR(error_reporter, "Heard %s (%d) @%dms", found_command,
                                score, current_time);

        if (!strcmp(found_command, kCategoryLabels[2])) { // 打开
            control_type = 1;
            LOG("Heard 2 %s (%d) @%dms", found_command, score, current_time);
        } else if (!strcmp(found_command, kCategoryLabels[3])) { // 关闭
            control_type = 2;
            LOG("Heard 3 %s (%d) @%dms", found_command, score, current_time);
        } else {
            control_type = 3;
        }
        aos_sem_signal(&cmd_sem);
    }
}

