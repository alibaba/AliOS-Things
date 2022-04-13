/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */
#ifndef _SH1106_HDCONFIG_H
#define _SH1106_HDCONFIG_H

#define USE_SOFT_SPI 1

#if  USE_SOFT_SPI == 1
#define OLED_CLK_PIN    HAL_IOMUX_PIN_P0_5
#define OLED_SDA_PIN    HAL_IOMUX_PIN_P4_0
#define OLED_RES_PIN    HAL_IOMUX_PIN_P4_1
#define OLED_DC_PIN     HAL_IOMUX_PIN_P2_7
#else
#define HARD_SPI_PORT 1
#define SPI1_DC_PIN     HAL_IOMUX_PIN_P3_4
#define SPI1_RESET_PIN  HAL_IOMUX_PIN_P3_6
#endif

#define RECORD_SCREEN   0
#define FB_FRAME_EN     0

#endif