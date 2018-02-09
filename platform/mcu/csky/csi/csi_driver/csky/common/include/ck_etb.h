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
 * @file     ck_etb.h
 * @brief    header file for event trigger block driver
 * @version  V1.0
 * @date     28. octorber 2017
 ******************************************************************************/
#ifndef _CK_ETB_H_
#define _CK_ETB_H_

#include <soc.h>

#define ETB_CHANNEL_FUNC_ERROR     1
#define ETB_NO_SUCH_TRIGGER        2
#define ETB_NO_SUCH_CHANNEL_AVAIL  3
#define ETB_MODE_ERROR             4

typedef enum {
    CK_ETB_CH0    = 0,
    CK_ETB_CH1    = 1,
    CK_ETB_CH2    = 2,
    CK_ETB_CH3    = 3,
    CK_ETB_CH4    = 4,
    CK_ETB_CH5    = 5,
    CK_ETB_CH6    = 6,
    CK_ETB_CH7    = 7,
    CK_ETB_CH8    = 8,
    CK_ETB_CH9    = 9,
    CK_ETB_CH10   = 10,
    CK_ETB_CH11   = 11,
    CK_ETB_CH12   = 12,
    CK_ETB_CH13   = 13,
    CK_ETB_CH14   = 14,
    CK_ETB_CH15   = 15,
    CK_ETB_CH16   = 16,
    CK_ETB_CH17   = 17,
    CK_ETB_CH18   = 18,
    CK_ETB_CH19   = 19,
    CK_ETB_CH20   = 20,
    CK_ETB_CH21   = 22,
    CK_ETB_CH23   = 23,
    CK_ETB_CH24   = 24,
    CK_ETB_CH25   = 25,
    CK_ETB_CH26   = 26,
    CK_ETB_CH27   = 27,
    CK_ETB_CH28   = 28,
    CK_ETB_CH29   = 29,
    CK_ETB_CH30   = 30,
    CK_ETB_CH31   = 31
} CKENUM_ETB_CHANNEL;

#define CK_ETB_EN          CSKY_ETB_BASE
#define CK_ETB_SOFTTRIG    (CSKY_ETB_BASE + 0x4)
#define CK_ETB_CFG0_CH0    (CSKY_ETB_BASE + 0x8)
#define CK_ETB_CFG1_CH0    (CSKY_ETB_BASE + 0xc)
#define CK_ETB_CFG0_CH1    (CSKY_ETB_BASE + 0x10)
#define CK_ETB_CFG1_CH1    (CSKY_ETB_BASE + 0x14)
#define CK_ETB_CFG0_CH2    (CSKY_ETB_BASE + 0x18)
#define CK_ETB_CFG1_CH2    (CSKY_ETB_BASE + 0x1c)
#define CK_ETB_CFG_CH3     (CSKY_ETB_BASE + 0x30)
#define CK_ETB_CFG_CH4     (CSKY_ETB_BASE + 0x34)
#define CK_ETB_CFG_CH5     (CSKY_ETB_BASE + 0x38)
#define CK_ETB_CFG_CH6     (CSKY_ETB_BASE + 0x3c)
#define CK_ETB_CFG_CH7     (CSKY_ETB_BASE + 0x40)
#define CK_ETB_CFG_CH8     (CSKY_ETB_BASE + 0x44)
#define CK_ETB_CFG_CH9     (CSKY_ETB_BASE + 0x48)
#define CK_ETB_CFG_CH10    (CSKY_ETB_BASE + 0x4c)
#define CK_ETB_CFG_CH11    (CSKY_ETB_BASE + 0x50)
#define CK_ETB_CFG_CH12    (CSKY_ETB_BASE + 0x54)
#define CK_ETB_CFG_CH13    (CSKY_ETB_BASE + 0x58)
#define CK_ETB_CFG_CH14    (CSKY_ETB_BASE + 0x5c)
#define CK_ETB_CFG_CH15    (CSKY_ETB_BASE + 0x60)
#define CK_ETB_CFG_CH16    (CSKY_ETB_BASE + 0x64)
#define CK_ETB_CFG_CH17    (CSKY_ETB_BASE + 0x68)
#define CK_ETB_CFG_CH18    (CSKY_ETB_BASE + 0x6c)
#define CK_ETB_CFG_CH19    (CSKY_ETB_BASE + 0x70)
#define CK_ETB_CFG_CH20    (CSKY_ETB_BASE + 0x74)
#define CK_ETB_CFG_CH21    (CSKY_ETB_BASE + 0x78)
#define CK_ETB_CFG_CH22    (CSKY_ETB_BASE + 0x7c)
#define CK_ETB_CFG_CH23    (CSKY_ETB_BASE + 0x80)
#define CK_ETB_CFG_CH24    (CSKY_ETB_BASE + 0x84)
#define CK_ETB_CFG_CH25    (CSKY_ETB_BASE + 0x88)
#define CK_ETB_CFG_CH26    (CSKY_ETB_BASE + 0x8c)
#define CK_ETB_CFG_CH27    (CSKY_ETB_BASE + 0x90)
#define CK_ETB_CFG_CH28    (CSKY_ETB_BASE + 0x94)
#define CK_ETB_CFG_CH29    (CSKY_ETB_BASE + 0x98)
#define CK_ETB_CFG_CH30    (CSKY_ETB_BASE + 0x9c)
#define CK_ETB_CFG_CH31    (CSKY_ETB_BASE + 0xa0)

#define CK_ETB_TRIGGER_MODE_BIT   1
#define CK_ETB_SRC0_EN_CH0_BIT    0
#define CK_ETB_SRC1_EN_CH0_BIT    10
#define CK_ETB_SRC2_EN_CH0_BIT    20

#define CK_ETB_CH0_SOURCE_TRIGGER_MAX          123
#define CK_ETB_CH0_SOURCE_TRIGGER_BITS_WIDTH   7
#define CK_ETB_CH0_DEST_TRIGGER_MAX            255
#define CK_ETB_CH0_DEST_TRIGGER_BIT_BASE       24
#define CK_ETB_CH0_DEST_TRIGGER_BITS_WIDTH     8

#define CK_ETB_DEST0_EN_CH1_BIT    0
#define CK_ETB_DEST1_EN_CH1_BIT    10
#define CK_ETB_DEST2_EN_CH1_BIT    20

#define CK_ETB_CH1_DEST_TRIGGER_MAX              255
#define CK_ETB_CH1_DEST_TRIGGER_BITS_WIDTH       8
#define CK_ETB_CH1_SOURCE_TRIGGER_MAX            123
#define CK_ETB_CH1_SOURCE_TRIGGER_BIT_BASE       25
#define CK_ETB_CH1_SOURCE_TRIGGER_BITS_WIDTH     7

#define CK_ETB_DEST0_EN_CH2_BIT    0
#define CK_ETB_DEST1_EN_CH2_BIT    10
#define CK_ETB_DEST2_EN_CH2_BIT    20

#define CK_ETB_CH2_DEST_TRIGGER_MAX              255
#define CK_ETB_CH2_DEST_TRIGGER_BITS_WIDTH       8
#define CK_ETB_CH2_SOURCE_TRIGGER_MAX            123
#define CK_ETB_CH2_SOURCE_TRIGGER_BIT_BASE       25
#define CK_ETB_CH2_SOURCE_TRIGGER_BITS_WIDTH     7

#define CK_ETB_CH3_31_SOURCE_TRIGGER_MAX            123
#define CK_ETB_CH3_31_SOURCE_TRIGGER_BITS_WIDTH     7
#define CK_ETB_CH3_31_DEST_TRIGGER_MAX              255
#define CK_ETB_CH3_31_DEST_TRIGGER_BIT_BASE         24
#define CK_ETB_CH3_31_DEST_TRIGGER_BITS_WIDTH       8

#define CK_ETB_CH3_31_SOURCE_BASE_BIT    12
#define CK_ETB_CH3_31_DEST_BASE_BIT    24

#define CHANNEL_ENABLE_COMMAND    1
#define CHANNEL_DISABLE_COMMAND   0

//source location and its' number
typedef enum {
    RTC = 0
} CKENUM_ETB_SOURCE_LOCALTION;

//dest location and its' number
typedef enum {
    DMA_CH0 = 0
} CKENUM_ETB_DEST_LOCALTION;


#endif /* __CK_ETB_H */

