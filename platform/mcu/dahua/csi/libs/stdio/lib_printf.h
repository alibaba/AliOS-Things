/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
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
/******************************************************************************
 * @file     lib_printf.h
 * @brief    header file for lib printf
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/

#ifndef LIB_PRINTF_H
#define LIB_ PRINTF_H

#include <stdarg.h>

//int getch(void);
//int putch(int c);

int printf_mutex_init(void);
//int printf(const char *format, ...);

#endif  /* PRINTF_H */
