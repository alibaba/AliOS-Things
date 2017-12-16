/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/******************************************************************************
 * @file     lib.c
 * @brief    source file for the lib
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
#include <config.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include "csi_core.h"     //for test
#include "soc.h"
#include "drv_usart.h"
#include "drv_timer.h"

#ifndef CONFIG_LPM_TICKLESS_SLEEP
extern uint32_t drv_coret_get_load(void);
extern uint32_t drv_coret_get_value(void);
#else
timer_handle_t system_timer;
timer_handle_t count_timer;
#endif

usart_handle_t console_handle = NULL;
extern int32_t csi_usart_putchar(usart_handle_t handle, uint8_t ch);
extern int32_t csi_usart_getchar(usart_handle_t handle, uint8_t *ch);

static void _mdelay(void)
{
#ifdef CONFIG_LPM_TICKLESS_SLEEP
    uint32_t load;
    uint32_t start;
    uint32_t cur;
    uint32_t cnt = (SYSTEM_CLOCK / 1000);

    csi_timer_get_load_value(count_timer, &load);
    csi_timer_get_current_value(count_timer, &start);

    while (1) {
        csi_timer_get_current_value(count_timer, &cur);

        if (start > cur) {
            if (start - cur >= cnt) {
                return;
            }
        } else {
            if (cur + load - start > cnt) {
                return;
            }
        }
    }
#else
    uint32_t load = drv_coret_get_load();
    uint32_t start = drv_coret_get_value();
    uint32_t cur;
    uint32_t cnt = (SYSTEM_CLOCK / 1000);

    while (1) {
        cur = drv_coret_get_value();

        if (start > cur) {
            if (start - cur >= cnt) {
                return;
            }
        } else {
            if (cur + load - start > cnt) {
                return;
            }
        }
    }
#endif
}

void mdelay(uint32_t ms)
{
    if (ms == 0) {
        return;
    }

    while (ms--) {
        _mdelay();
    }
}


#if 1    /*for printf porting*/
int fputc(int ch, FILE *stream)
{
    if (console_handle == NULL) {
        return -1;
    }

    csi_usart_putchar(console_handle, ch);
    return 0;
}

int fgetc(FILE *stream)
{
    uint8_t ch;

    if (console_handle == NULL) {
        return -1;
    }

    csi_usart_getchar(console_handle, &ch);
    return ch;
}
#endif
