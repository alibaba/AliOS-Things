/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __HAL_CHIPID_H__
#define __HAL_CHIPID_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "plat_types.h"

// BEST1000: A=0 C=1 D=2 F=3 G=4 I=5
// BEST1400: A=0 B=1 C=2(cmuRev=2&diffRom) D=3 E=4(cmuRev=3&diffRom) F=5(cmuRev=4)
// BEST1402: A=0 B=1
// BEST2000: A=0 C=1 D=2 E=3(rev=2&aonRev=4) F=4 G=5
// BEST2300: A=0 B=1(rfRev=15) C=3 D=4 E=5(rfRev=4&diffRom)

enum HAL_CHIP_METAL_ID_T {
    HAL_CHIP_METAL_ID_0,
    HAL_CHIP_METAL_ID_1,
    HAL_CHIP_METAL_ID_2,
    HAL_CHIP_METAL_ID_3,
    HAL_CHIP_METAL_ID_4,
    HAL_CHIP_METAL_ID_5,
    HAL_CHIP_METAL_ID_6,
    HAL_CHIP_METAL_ID_7,
    HAL_CHIP_METAL_ID_8,
    HAL_CHIP_METAL_ID_9,
    HAL_CHIP_METAL_ID_10,
    HAL_CHIP_METAL_ID_11,
    HAL_CHIP_METAL_ID_12,
    HAL_CHIP_METAL_ID_13,
    HAL_CHIP_METAL_ID_14,
    HAL_CHIP_METAL_ID_15,
};

enum HAL_BT_CHIP_SERIES_T {
    HAL_BT_CHIP_SERIES_2000,
    HAL_BT_CHIP_SERIES_2300,
    HAL_BT_CHIP_SERIES_2500,
};

// Invoked by hal_chipid_init() only
uint32_t read_hw_metal_id(void);

void hal_chipid_init(void);

enum HAL_CHIP_METAL_ID_T hal_get_chip_metal_id(void);
enum HAL_BT_CHIP_SERIES_T hal_get_bt_chip_series(void);

#ifdef __cplusplus
}
#endif

#endif
