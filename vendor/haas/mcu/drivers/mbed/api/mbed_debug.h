/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef MBED_DEBUG_H
#define MBED_DEBUG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_trace.h"

#define debug(format, ...)                  TRACE(format, ##__VA_ARGS__)
#define debug_if(condition, format, ...)    do { if (condition) { TRACE(format, ##__VA_ARGS__); } } while (0)

#ifdef __cplusplus
}
#endif

#endif
