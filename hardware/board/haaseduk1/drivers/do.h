/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef GIDITAL_OUTPUT_H
#define GIDITAL_OUTPUT_H

#include "aos/hal/gpio.h"
#include "stdint.h"

enum en_do_port {
    DO_PORT_0 = 0,
    DO_PORT_1,
    DO_PORT_2,
    DO_PORT_3,
    DO_PORT_4,
    DO_PORT_5,
    DO_PORT_6,
    DO_PORT_7,
    DO_PORT_SIZE
};

int32_t expansion_board_do_init(void);

int32_t expansion_board_do_high(uint8_t port);

int32_t expansion_board_do_low(uint8_t port);

#endif
