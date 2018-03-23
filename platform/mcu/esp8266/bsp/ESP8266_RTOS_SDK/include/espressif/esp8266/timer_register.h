/*
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2015 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef _TIMER_REGISTER_H_
#define _TIMER_REGISTER_H_

#define PERIPHS_TIMER_BASEDDR       0x60000600

#define FRC1_LOAD_ADDRESS           (PERIPHS_TIMER_BASEDDR + 0x0)
#define TIMER_FRC1_LOAD_VALUE           0x007FFFFF
#define TIMER_FRC1_LOAD_VALUE_S         0
#define FRC1_LOAD_DATA_MSB              22
#define FRC1_LOAD_DATA_LSB              0
#define FRC1_LOAD_DATA_MASK             0x007fffff

#define FRC1_COUNT_ADDRESS          (PERIPHS_TIMER_BASEDDR + 0x4)
#define TIMER_FRC1_COUNT                0x007FFFFF
#define TIMER_FRC1_COUNT_S              0
#define FRC1_COUNT_DATA_MSB             22
#define FRC1_COUNT_DATA_LSB             0
#define FRC1_COUNT_DATA_MASK            0x007fffff

#define FRC1_CTRL_ADDRESS           (PERIPHS_TIMER_BASEDDR + 0x8)
#define TIMER_FRC1_INT                  (BIT(8))
#define TIMER_FRC1_CTRL                 0x000000FF
#define TIMER_FRC1_CTRL_S               0
#define FRC1_CTRL_DATA_MSB              7
#define FRC1_CTRL_DATA_LSB              0
#define FRC1_CTRL_DATA_MASK             0x000000ff

#define FRC1_INT_ADDRESS            (PERIPHS_TIMER_BASEDDR + 0xC)
#define TIMER_FRC1_INT_CLR_MASK         (BIT(0))
#define FRC1_INT_CLR_MSB                0
#define FRC1_INT_CLR_LSB                0
#define FRC1_INT_CLR_MASK               0x00000001

#define FRC2_LOAD_ADDRESS           (PERIPHS_TIMER_BASEDDR + 0x20)
#define TIMER_FRC2_LOAD_VALUE           0xFFFFFFFF
#define TIMER_FRC2_LOAD_VALUE_S         0
#define FRC2_LOAD_DATA_MSB              31
#define FRC2_LOAD_DATA_LSB              0
#define FRC2_LOAD_DATA_MASK             0xffffffff

#define FRC2_COUNT_ADDRESS          (PERIPHS_TIMER_BASEDDR + 0x24)
#define TIMER_FRC2_COUNT                0xFFFFFFFF
#define TIMER_FRC2_COUNT_S              0
#define FRC2_COUNT_DATA_MSB             31
#define FRC2_COUNT_DATA_LSB             0
#define FRC2_COUNT_DATA_MASK            0xffffffff

#define FRC2_CTRL_ADDRESS           (PERIPHS_TIMER_BASEDDR + 0x28)
#define TIMER_FRC2_INT                  (BIT(8))
#define TIMER_FRC2_CTRL                 0x000000FF
#define TIMER_FRC2_CTRL_S               0
#define FRC2_CTRL_DATA_MSB              7
#define FRC2_CTRL_DATA_LSB              0
#define FRC2_CTRL_DATA_MASK             0x000000ff

#define FRC2_INT_ADDRESS            (PERIPHS_TIMER_BASEDDR + 0x2C)
#define TIMER_FRC2_INT_CLR_MASK         (BIT(0))
#define FRC2_INT_CLR_MSB                0
#define FRC2_INT_CLR_LSB                0
#define FRC2_INT_CLR_MASK               0x00000001

#define FRC2_ALARM_ADDRESS          (PERIPHS_TIMER_BASEDDR + 0x30)
#define TIMER_FRC2_ALARM                0xFFFFFFFF
#define TIMER_FRC2_ALARM_S              0
#define FRC2_ALARM_DATA_MSB             31
#define FRC2_ALARM_DATA_LSB             0
#define FRC2_ALARM_DATA_MASK            0xffffffff

#endif
