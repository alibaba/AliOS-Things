/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef GIDITAL_INPUT_H
#define GIDITAL_INPUT_H

#include "aos/hal/gpio.h"
#include "stdint.h"

enum en_di_port {
    DI_PORT_0 = 0,
    DI_PORT_1,
    DI_PORT_2,
    DI_PORT_3,
    DI_PORT_SIZE
};

typedef int32_t (*digital_input_value_change_notify)(uint8_t  port,
                                                     uint32_t value);

void expansion_board_di_change_notify_register(
    digital_input_value_change_notify cb);

int32_t expansion_board_di_get_value(uint8_t port, gpio_pinstate_t *value);

int32_t expansion_board_di_init();
#endif
