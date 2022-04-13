/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef K_KEY_H
#define K_KEY_H

#include "stdint.h"

#define KEY_MIN_PRESS_MS   50
#define KEY_SHORT_PRESS_MS 2000
#define KEY_LONG_PRESS_MS  6000

typedef struct {
    uint32_t short_press_max_ms;   // default 2000
    uint32_t long_press_min_ms;    // default 5000
    void (*short_press_handler)(void);
    void (*long_press_handler)(void);
} key_cfg_t;

int key_init(key_cfg_t *cfg);

#endif

