/**
 * File: zx2975100_trng.h
 * Brief: Implementation of Sanechips trng
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

#ifndef ZX297100_TRNG_H
#define ZX297100_TRNG_H

/*******************************************************************************
*                               Include header files                            *
*******************************************************************************/
#include "drv_trng.h"
#include "soc.h"

/*******************************************************************************
*                                Macro definitions                              *
*******************************************************************************/
#define SEC_MOD_CLKEN0  (ZX29_AP_CRPM_BASE +0x080)

/*******************************************************************************
*                               Type definitions                                *
*******************************************************************************/
struct trng_reg_t {
    uint32_t in_output[4];  /*0x00~0c*/
    uint32_t sta_intack;        /*0x10*/
    uint32_t conctrl;           /*0x14*/
    uint32_t config;            /*0x18*/
    uint32_t alarmcnt;          /*0x1c*/
    uint32_t fro_enable;        /*0x20*/
    uint32_t fro_detune;        /*0x24*/
    uint32_t alarm_mask;        /*0x28*/
    uint32_t alarm_stop;        /*0x2c*/
    uint32_t lfsr0;             /*0x30*/
    uint32_t lfsr1;             /*0x34*/
    uint32_t lfsr2;             /*0x38*/
    uint32_t count;             /*0x3c*/
    uint32_t run_cnt;           /*0x40*/
    uint32_t run_1;             /*0x44*/
    uint32_t run_2;             /*0x48*/
    uint32_t run_3;             /*0x4c*/
    uint32_t run_4;             /*0x50*/
    uint32_t run_5;             /*0x54*/
    uint32_t run_6;             /*0x58*/
    uint32_t monobit_cnt;       /*0x5c*/
    uint32_t POKER_3_0;         /*0x60*/
    uint32_t POKER_7_4;         /*0x64*/
    uint32_t POKER_B_8;         /*0x68*/
    uint32_t POKER_F_C;         /*0x6c*/
    uint32_t test;              /*0x70*/
    uint32_t block_cnt;         /*0x74*/
    uint32_t options;           /*0x78*/
    uint32_t eip_rev;           /*0x7c*/
    uint32_t endian;            /*0x80*/
    uint32_t fro_tst;           /*0x84*/
    uint32_t fsm;               /*0x88*/
    uint32_t strap;             /*0x8c*/

};

enum buf_ram_size_t {
    Buffer_NoRam = 0,
    Buffer_Resev = 1,
    Buffer_512bit = 2,
    Buffer_1Kbit = 3,
    Buffer_2Kbit = 4,
    Buffer_4Kbit = 5,
    Buffer_8Kbit = 6,
    Buffer_16Kbit = 7,
    Buffer_End
};

enum conctrl_bit_t {
    Ready_mask = 0,
    Shutdown_oflo_mask = 1,
    Stuck_out_mask = 2,
    Noise_fail_mask = 3,
    Run_fail_mask = 4,
    Long_run_fail_mask = 5,
    Poker_fail_mask = 6,
    Monobit_fail_mask = 7,
    Test_mode = 8,
    Enable_trng = 10,
    Post_proc_en = 12,
    Re_seed = 15,

    conctrlbit_end
};


#endif //#ifndef ZX297100_TRNG_H

