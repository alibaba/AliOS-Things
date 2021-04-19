/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef K_LED_H
#define K_LED_H

typedef enum { LED_OFF, LED_ON } led_e;
typedef enum { LED1_NUM = 1, LED2_NUM = 2, LED3_NUM = 3 } led_num_e;

void led_switch(led_num_e id, led_e onoff);

#endif
