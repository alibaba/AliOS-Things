/*
 * Copyright (c) 2019 Alibaba Group. All rights reserved.
 * License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Description:
 * log_simple.c: log simple impl
 *
 */
#include <stdarg.h>
#include "log.h"
#include "aliyun_iot_smartcard_os_hal.h"

void log_file_print(int lvl, const char *str)
{
    return;
}

uint8_t log_file_init(cchar *dir, int total_size, int single_size)
{
    return 0;
}

void log_file_destroy(void)
{
    return;
}

int log_init(cchar *dir, cchar *name, LOG_STORE_TYPE type, LOG_MODE mode)
{
    return 0;
}

void log_destroy(void)
{
    return;
}

void log_print(LOG_LEVEL lvl, cchar *color, cchar *t, cchar *f, cchar *func, int l, cchar *fmt, ...)
{
    char buf[64];

    va_list argptr;
    va_start(argptr, fmt);
    vsnprintf((char *)buf, sizeof(buf), fmt, argptr);
    va_end(argptr);

    HAL_Printf("%s", buf);
}
