/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */
#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "aos/init.h"
#include "k_config.h"
#include "board.h"
#include "aos/hal/uart.h"
#include "stm32u5xx_hal.h"

void systick_handler(void);
void board_flash_init(void);
void board_kinit_init(kinit_t* init_args);
void board_network_init(void);
void board_wifi_init(void);
void board_gpio_init(void);
void board_dma_init(void);
void board_stduart_init(void);
void board_tick_init(void);
void board_basic_init(void);
void board_pre_init(void);
#endif