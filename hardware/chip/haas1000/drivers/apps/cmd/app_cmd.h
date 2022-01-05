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
#ifndef __APP_CMD_H__
#define __APP_CMD_H__
#ifdef __cplusplus
extern "C"
{
#endif
#ifdef __PC_CMD_UART__

#include "hal_cmd.h"
typedef struct {
    void *param;
} APP_CMD_HANDLE;

void app_cmd_open(void);

void app_cmd_close(void);
#endif
#ifdef __cplusplus
}
#endif
#endif//__FMDEC_H__
