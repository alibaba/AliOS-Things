/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef KEY_POLL_H
#define KEY_POLL_H

#include "stdint.h"
typedef enum {
    EDK_KEY_1 = 0b0001,
    EDK_KEY_2 = 0b0010,
    EDK_KEY_3 = 0b0100,
    EDK_KEY_4 = 0b1000,
} edk_keycode_t;

typedef uint8_t key_code_t;
typedef void (*key_code_cb)(key_code_t key_code);

int key_init(key_code_cb key_func);

#endif // KEY_POLL_H
