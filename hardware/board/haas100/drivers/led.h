/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef K_LED_H
#define K_LED_H

typedef enum {
    LED_OFF,
    LED_ON
} led_e;

void led_switch(int id, led_e onoff);

#endif
