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
 * @file     ck_sasc_v2.h
 * @brief    head file for ck sasc
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
#ifndef _CK_SASC_H_
#define _CK_SASC_H_

#include "drv_sasc.h"
#include "soc.h"

#define REGION_USED 1
#define EFLASH_ADDR_START   0x10000000
#define EFLASH_ADDR_END     0x1003f7ff
#define EFLASH_SECTOR_SIZE  0x200
#define EFLASH_REG_BASE     0x40005000

#define EFLASH_CAR                  0x38
#define EFLASH_CR                   0x3C
#define EFLASH_RG0                  0x40
#define EFLASH_RG1                  0x44
#define EFLASH_RG2                  0x48
#define EFLASH_RG3                  0x4C
#define EFLASH_RG4                  0x50
#define EFLASH_RG5                  0x54
#define EFLASH_RG6                  0x58
#define EFLASH_RG7                  0x5C
#define EFLASH_CFG0                 0x60
#define EFLASH_CFG1                 0x64

#define IS_EFLASH_ADDR(addr) \
    ((addr >= EFLASH_ADDR_START) && (addr <= EFLASH_ADDR_END))


#define SRAM_CAR                  0x00
#define SRAM_CR                   0x04
#define SRAM_RG0                  0x08
#define SRAM_RG1                  0x0c
#define SRAM_RG2                  0x10
#define SRAM_RG3                  0x14
#define SRAM_RG4                  0x18
#define SRAM_RG5                  0x1c
#define SRAM_RG6                  0x20
#define SRAM_RG7                  0x24
#define SRAM_CFG0                 0x28
#define SRAM_CFG1                 0x2c

#define SRAM_ADDR_START      0x20000000
#define SRAM_ADDR_END        0x2000ffff

#define SRAM0_ADDR_START     0x20000000
#define SRAM0_ADDR_END       0x20003fff

#define SRAM1_ADDR_START     0x20004000
#define SRAM1_ADDR_END       0x20007fff

#define SRAM2_ADDR_START     0x20008000
#define SRAM2_ADDR_END       0x2000ffff

#define IS_SRAM_ADDR(addr) \
    ((addr >= SRAM_ADDR_START) && (addr <= SRAM_ADDR_END))

#define IS_SRAM0_ADDR(addr) \
    ((addr >= SRAM0_ADDR_START) && (addr <= SRAM0_ADDR_END))

#define IS_SRAM1_ADDR(addr) \
    ((addr >= SRAM1_ADDR_START) && (addr <= SRAM1_ADDR_END))

#define IS_SRAM2_ADDR(addr) \
    ((addr >= SRAM2_ADDR_START) && (addr <= SRAM2_ADDR_END))

#define REGION_MAX_NUM  8

#endif
