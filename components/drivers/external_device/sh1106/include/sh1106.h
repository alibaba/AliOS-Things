/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */
#ifndef SENSORS_SH1106_H
#define SENSORS_SH1106_H

#include "font.h"
#include "icon.h"

void sh1106_show_string(uint8_t x,
                      uint8_t y,
                      const uint8_t *p,
                      uint8_t size,
                      uint8_t mode);
uint8_t sh1106_init(void);
#endif
