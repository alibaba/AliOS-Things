/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
 * All rights reserved.
 *
 * This software is supplied "AS IS" without any warranties.
 * RDA assumes no responsibility or liability for the use of the software,
 * conveys no license or title under any patent, copyright, or mask work
 * right to the product. RDA reserves the right to make changes in the
 * software without notification.  RDA also make no representation or
 * warranty that such application will be suitable for the specified use
 * without further testing or modification.
 */

#ifndef _HAL_CLK_H_
#define _HAL_CLK_H_

#include "cs_types.h"

#define FOURCC(a, b, c, d) (a | (b << 8) | (c << 16) | (d << 24))

enum
{
    FOURCC_UNKNOWN = 0,

    FOURCC_AVPS = FOURCC('A', 'V', 'P', 'S'),
    FOURCC_AVRS = FOURCC('A', 'V', 'R', 'S'),
    FOURCC_BT   = FOURCC('B', 'L', 'T', 'H'),
    FOURCC_CAMS = FOURCC('C', 'A', 'M', 'S'),
    FOURCC_CMMB = FOURCC('C', 'M', 'M', 'B'),
    FOURCC_CALIB = FOURCC('C', 'A', 'L', 'B'),
    FOURCC_CSW0 = FOURCC('C', 'S', 'W', '0'),
    FOURCC_CSW1 = FOURCC('C', 'S', 'W', '1'),
    FOURCC_CSW2 = FOURCC('C', 'S', 'W', '2'),
    FOURCC_CSW3 = FOURCC('C', 'S', 'W', '3'),
    FOURCC_CSW4 = FOURCC('C', 'S', 'W', '4'),
    FOURCC_CSW5 = FOURCC('C', 'S', 'W', '5'),
    FOURCC_CSW6 = FOURCC('C', 'S', 'W', '6'),
    FOURCC_CSW7 = FOURCC('C', 'S', 'W', '7'),
    FOURCC_PAL = FOURCC('P', 'A', 'L', ' '),
    FOURCC_VIDEO = FOURCC('V', 'I', 'D', 'O'),
    FOURCC_VOIS = FOURCC('V', 'O', 'I', 'S'),
    FOURCC_HAL_CAM = FOURCC('H', 'C', 'A', 'M'),
    FOURCC_HAL_USB = FOURCC('H', 'U', 'S', 'B'),
    FOURCC_HAL_VOC = FOURCC('H', 'V', 'O', 'C'),
    FOURCC_TEST = FOURCC('T', 'E', 'S', 'T'),
    FOURCC_TOOL = FOURCC('T', 'O', 'O', 'L'),

    FOURCC_SOFT = FOURCC('S', 'O', 'F', 'T'),

    FOURCC_ABBPLL = FOURCC('A', 'P', 'L', 'L'),
    FOURCC_AIF = FOURCC('A', 'I', 'F', ' '),
    FOURCC_AUDIO = FOURCC('A', 'D', 'I', 'O'),
    FOURCC_BCPU = FOURCC('B', 'C', 'P', 'U'),
    FOURCC_CAM = FOURCC('C', 'A', 'M', 'A'),
    FOURCC_CLKOUT = FOURCC('C', 'L', 'K', 'O'),
    FOURCC_DBG_HOST = FOURCC('D', 'B', 'G', 'H'),
    FOURCC_DMA = FOURCC('D', 'M', 'A', ' '),
    FOURCC_FLASH = FOURCC('F', 'L', 'A', 'S'),
    FOURCC_FLASH_EXT = FOURCC('F', 'L', 'S', 'E'),
    FOURCC_GOUDA = FOURCC('G', 'O', 'D', 'A'),
    FOURCC_I2C = FOURCC('I', '2', 'C', ' '),
    FOURCC_PSRAM = FOURCC('P','S','R','M'),
    FOURCC_PWM = FOURCC('P', 'W', 'M', ' '),
    FOURCC_SPI1 = FOURCC('S', 'P', 'I', '1'),
    FOURCC_SPI2 = FOURCC('S', 'P', 'I', '2'),
    FOURCC_ISPI = FOURCC('I', 'S', 'P', 'I'),
    FOURCC_RFSPI = FOURCC('R', 'S', 'P', 'I'),
    FOURCC_SDIO = FOURCC('S', 'D', 'I', 'O'),
    FOURCC_SDMMC = FOURCC('S', 'D', 'M', 'C'),
    FOURCC_SDMMC2 = FOURCC('S', 'D', 'M', '2'),
    FOURCC_SIM = FOURCC('S', 'I', 'M', ' '),
    FOURCC_SYSTEM = FOURCC('S', 'Y', 'S', 'T'),
    FOURCC_UART1 = FOURCC('U', 'R', 'T', '1'),
    FOURCC_UART2 = FOURCC('U', 'R', 'T', '2'),
    FOURCC_USB = FOURCC('U', 'S', 'B', ' '),
    FOURCC_VOC = FOURCC('V', 'O', 'C', ' '),
    FOURCC_XCV = FOURCC('X', 'C', 'V', ' '),
    FOURCC_XTAL = FOURCC('X', 'T', 'A', 'L'),
#ifdef CHIP_DIE_8909
    FOURCC_MCUPLL = FOURCC('M','C','P','L'),
    FOURCC_RFDIG = FOURCC('R', 'F', 'D', 'G'),
    FOURCC_WCN = FOURCC('W', 'C', 'N', ' '),
    FOURCC_UART3 = FOURCC('U', 'R', 'T', '3'),
    FOURCC_UART4 = FOURCC('U', 'R', 'T', '4'),
    FOURCC_GPADC = FOURCC('G', 'P', 'A', 'D'),
#endif
};

enum
{
    HAL_CLK_RATE_32K = 32768,
    HAL_CLK_RATE_13M = 13000000,
    HAL_CLK_RATE_26M = 26000000,
    HAL_CLK_RATE_39M = 39000000,
    HAL_CLK_RATE_48M = 48000000,
    HAL_CLK_RATE_52M = 52000000,
    HAL_CLK_RATE_78M = 78000000,
    HAL_CLK_RATE_89M = 89142857, //624M/7
    HAL_CLK_RATE_104M = 104000000,
    HAL_CLK_RATE_113M = 113454545, //624M/5.5
    HAL_CLK_RATE_125M = 124800000,
    HAL_CLK_RATE_139M = 138666667, //624M/4.5
    HAL_CLK_RATE_156M = 156000000,
    HAL_CLK_RATE_178M = 178285714, //624M/3.5
    HAL_CLK_RATE_208M = 208000000,
    HAL_CLK_RATE_250M = 249600000, //624M/2.5
    HAL_CLK_RATE_312M = 312000000,
    HAL_CLK_RATE_UNKNOWN = 0,
};

enum
{
    HAL_CLK_RATE_56M = 56000000,
    HAL_CLK_RATE_61M = 61090909, //336M/5.5
    HAL_CLK_RATE_67M = 67200000,
    HAL_CLK_RATE_75M = 74666667, //336M/4.5
    HAL_CLK_RATE_84M = 84000000,
    HAL_CLK_RATE_96M = 96000000, //336M/3.5
    HAL_CLK_RATE_112M = 112000000,
    HAL_CLK_RATE_134M = 134400000, //336M/2.5
    HAL_CLK_RATE_168M = 168000000,
    HAL_CLK_RATE_224M = 224000000, //336M/1.5
    HAL_CLK_RATE_336M = 336000000,
    // HAL_CLK_RATE_UNKNOWN = 0,
};


#define HAL_MODULE_CLK_INIT(name) void clkInit##name()
#define MODULE_CLK_INIT(name) clkInit##name()

struct clk;

typedef struct clk HAL_CLK_T;

bool hal_ClkInit();

HAL_CLK_T *hal_ClkGet(const uint32_t four_cc);

void hal_ClkPut(HAL_CLK_T *clk);

bool hal_ClkEnable(HAL_CLK_T *clk);

void hal_ClkDisable(HAL_CLK_T *clk);

bool hal_ClkIsEnabled(HAL_CLK_T *clk);

bool hal_ClkSetRate(HAL_CLK_T *clk, uint32_t rate);

uint32_t hal_ClkGetRate(HAL_CLK_T *clk);

bool hal_SwRequestClk(const uint32_t fourCC, uint32_t rate);

bool hal_SwReleaseClk(const uint32_t fourCC);

bool hal_SwUserIsActive(const uint32_t fourCC);

uint32_t hal_SwGetClkRate(const uint32_t fourCC);

void hal_clk_dump(bool verbose);

#endif // _HAL_CLK_H_
