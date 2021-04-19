/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef EXP_ADC_H
#define EXP_ADC_H

#include "aos/hal/adc.h"
#include "stdint.h"

enum en_exp_adc_port { ADC_PORT_0 = 0, ADC_PORT_1, ADC_PORT_2, ADC_PORT_SIZE };

int32_t expansion_board_adc_init(void);

int32_t expansion_board_adc_get_value(uint32_t port, uint32_t *adc_value);

int32_t board_get_voltage(uint32_t port, uint32_t *output);
#endif
