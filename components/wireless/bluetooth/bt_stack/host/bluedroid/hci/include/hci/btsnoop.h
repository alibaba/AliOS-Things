/******************************************************************************
 *
 *  Copyright 2014 Google, Inc.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

#ifndef _BTSNOOP_H_
#define _BTSNOOP_H_

#include <stdbool.h>

#include "bt_types.h"

static const char BTSNOOP_MODULE[] = "btsnoop_module";

typedef void (*btsnoop_net_callback)(const void* data, size_t length); 

typedef struct btsnoop_t {
    void (*start_up)(void);
    void (*shut_down)(void);
    // Capture |packet| and dump it to the btsnoop logs. If |is_received| is
    // true, the packet is marked as incoming. Otherwise, the packet is marked
    // as outgoing.
    void (*capture)(const BT_HDR* packet, bool is_received);
} btsnoop_t;

const btsnoop_t* btsnoop_get_interface(void);

#endif
