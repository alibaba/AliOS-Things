/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef EXP_ADC_H
#define EXP_ADC_H

#include "stdint.h"
#include "aos/hal/adc.h"

enum en_exp_adc_port {
    ADC_PORT_0 = 0,
    ADC_PORT_1,
    ADC_PORT_SIZE
};

int32_t expansion_board_adc_init(void);

int32_t expansion_board_do_high(uint8_t port);

#endif