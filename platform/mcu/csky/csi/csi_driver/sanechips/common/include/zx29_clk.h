/**
 * File: clk_si.h
 * Brief: bsp APIs of Sanechips clk
 *
 * Copyright (C) 2017 Sanechips Technology Co., Ltd.
 * Author:
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef _CLK_SI_H
#define _CLK_SI_H

/*******************************************************************************
 *                             Include header files                            *
 ******************************************************************************/
#include <stdint.h>

/*******************************************************************************
 *                               Macro definitions                             *
 ******************************************************************************/

/*******************************************************************************
 *                               Type definitions                              *
 ******************************************************************************/
struct reg_info {
    uint32_t regbase;          /*clock operation register base address*/
    uint16_t offset;           /*clock operation domain offset in register*/
    uint16_t size;             /*clock operation domain size*/
};

struct clk_si_info {
    const char name[22];                  /*clock name*/
    const struct reg_info clken;          /*clock enable register information*/
    const struct reg_info auto_gate;      /*automatic gate register information*/
    const struct reg_info clk_sel;        /*clock source selection register information*/
    const struct reg_info clk_div;        /*clock source division register information*/
    const struct reg_info reset;          /*reset register information*/
};

#endif/*_CLK_SI_H*/

