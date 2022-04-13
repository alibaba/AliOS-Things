/*
 * Copyright (c) 2019 MediaTek Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef __MT_GPIO_KEYS_H
#define __MT_GPIO_KEYS_H
#include "queue.h"
#define GPIO_NUM	116
#define KEY_NUM		3
#define EVENT_NUM	10
#define KEY_DEBOUNCE	50000
#define VOLUMEUP_KEY	115
#define VOLUMEDOWN_KEY	114
#define MENU_KEY		238
#define VOLUMEUP_KEY_PIN	43
#define VOLUMEDOWN_KEY_PIN	40
#define MENU_KEY_PIN		42
typedef struct {
    int keycode;
	int status;
} gpio_keys_event;
QueueHandle_t key_eventqueue;
void gpio_keys_init(void);
#endif
