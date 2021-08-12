/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */
#ifndef _SH1106_HDCONFIG_H
#define _SH1106_HDCONFIG_H
#if USE_SOFT_SPI == 1
#ifdef BOARD_HAASEDUK1
#define OLED_CLK_PIN    HAL_IOMUX_PIN_P3_7
#define OLED_SDA_PIN    HAL_IOMUX_PIN_P3_5
#define OLED_RES_PIN    HAL_IOMUX_PIN_P3_6
#define OLED_DC_PIN     HAL_IOMUX_PIN_P3_4
#else
#define OLED_CLK_PIN    HAL_IOMUX_PIN_P0_5
#define OLED_SDA_PIN    HAL_IOMUX_PIN_P4_0
#define OLED_RES_PIN    HAL_IOMUX_PIN_P4_1
#define OLED_DC_PIN     HAL_IOMUX_PIN_P2_7
#endif
#else
#define HARD_SPI_PORT 1
#define SPI1_DC_PIN     HAL_IOMUX_PIN_P3_4
#define SPI1_RESET_PIN  HAL_IOMUX_PIN_P3_6
#endif
#endif