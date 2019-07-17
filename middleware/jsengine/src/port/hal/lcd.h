/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef BE_LCD_H
#define BE_LCD_H

void hal_lcd_init(void* arg);
void hal_lcd_draw_point(uint16_t xpos, uint16_t ypos, uint16_t color);
void hal_lcd_draw_line(uint16_t xpos, uint16_t ypos, uint32_t rgb,
                       uint16_t length);

#endif /* BE_LCD_H */
