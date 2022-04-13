
/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef SENSORS_SH1106_H
#define SENSORS_SH1106_H

#include "../font.h"
#include "../icon.h"

void    sh1106_display_example(int index);
uint8_t sh1106_init(void);
void    OLED_Refresh_GRAM(void);
void    OLED_Clear(void);
void    OLED_Full(void);
void    OLED_DrawPoint(int16_t x, int16_t y, uint8_t mode);
void    OLED_DrawLine(int16_t x0,
                      int16_t y0,
                      int16_t x1,
                      int16_t y1,
                      uint8_t mode);
void OLED_DrawVerticalLine(uint8_t x, uint8_t y, uint8_t length, uint8_t mode);
void OLED_DrawHorizontalLine(uint8_t x,
                             uint8_t y,
                             uint8_t length,
                             uint8_t mode);
void OLED_DrawRect(uint8_t x,
                   uint8_t y,
                   uint8_t width,
                   uint8_t height,
                   uint8_t mode);
void OLED_FillRect(uint8_t xMove,
                   uint8_t yMove,
                   uint8_t width,
                   uint8_t height,
                   uint8_t mode);
void OLED_DrawCircle(uint8_t x0,
                     uint8_t y0,
                     uint8_t radius,
                     uint8_t width,
                     uint8_t mode);
void OLED_FillCircle(uint8_t x0, uint8_t y0, uint8_t r, uint8_t mode);
void OLED_Show_Char(uint8_t x,
                    uint8_t y,
                    uint8_t chr,
                    uint8_t size,
                    uint8_t mode);
void OLED_Show_String(uint8_t        x,
                      uint8_t        y,
                      const uint8_t *p,
                      uint8_t        size,
                      uint8_t        mode);
void OLED_Icon_Draw(int16_t x, int16_t y, icon_t *icon, uint8_t mode);
void OLED_test(int flag);
#endif
