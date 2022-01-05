/*
 * Copyright (c) 2021 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
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
#ifndef __APP_HAL_H__
#define __APP_HAL_H__

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#if BES_HAL_DEBUG
#define ENTER_FUNCTION() printf("%s enter ->\n", __FUNCTION__)
#define LEAVE_FUNCTION() printf("%s <- leave\n", __FUNCTION__)
#define FOOTPRINT() printf("%s:%d\n", __FUNCTION__, __LINE__)
#define TRACEME(str, ...)  printf("%s:%d "str, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define ENTER_FUNCTION()
#define LEAVE_FUNCTION()
#define FOOTPRINT()
#define TRACEME(str, ...)
#endif
#define FAIL_FUNCTION() printf("%s:%d fail!\n", __FUNCTION__, __LINE__)

#endif // __APP_HAL_H__

