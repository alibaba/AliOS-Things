/*
 * Copyright (C) 2016 YunOS Project. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _WATCHPOINT_H
#define _WATCHPOINT_H

#include <stdint.h>

/* max number of watchpiont can set */
#define WP_MAX 2

typedef enum {
    WATCH = 1,
    RWATCH,
    AWATCH
} watch_type;

/* struct to record two watchpoint information */
typedef struct watchpoint_s {
    uint32_t  get_addr;
    uint8_t   get_len;
    void (*get_callfuct)(void);
    uint8_t   alive;
} watchpoint_t;

/* function prototypes */
int wp_register(uint32_t base_addr, watch_type watch_s, void (*callfuct)(void));
int wp_unregister(uint8_t number);
int wp_enable(uint8_t number);
int wp_disable(uint8_t number);
void wp_get_callback(void);

#endif /* _WATCHPOINT_H */
