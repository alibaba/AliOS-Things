/*!
    \file  gd32f3x0_lcd_eval.h
    \brief LCD driver header file
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-28, V1.0.0, firmware for GD32F3x0
*/

#ifndef GD32F3X0_LCD_EVAL_H
#define GD32F3X0_LCD_EVAL_H

#ifdef __cplusplus
    extern "C" {
#endif

#include "gd32f350r_eval.h"

/* Colors */
#define RED             0xf800
#define GREEN           0x07e0
#define BLUE            0x001f
#define WHITE           0xffff
#define BLACK           0x0000
#define YELLOW          0xFFE0

/* GRAYs */
#define GRAY0           0xEF7D
#define GRAY1           0x8410
#define GRAY2           0x4208

/* PF5 tft cs */
#define LCD_CS_SET      ((uint32_t)(GPIO_BOP(GPIOF) = GPIO_PIN_5))
#define LCD_CS_CLR      ((uint32_t)(GPIO_BC(GPIOF) = GPIO_PIN_5))

/* PC3 tft rs/dc */
#define LCD_RS_SET      ((uint32_t)(GPIO_BOP(GPIOC) = GPIO_PIN_3))
#define LCD_RS_CLR      ((uint32_t)(GPIO_BC(GPIOC) = GPIO_PIN_3))

/* PC4 tft rst */
#define LCD_RST_SET     ((uint32_t)(GPIO_BOP(GPIOC) = GPIO_PIN_4))
#define LCD_RST_CLR     ((uint32_t)(GPIO_BC(GPIOC) = GPIO_PIN_4))

/* lcd init */
void lcd_init(void);
/* clear the lcd */
void lcd_clear(uint16_t color);
/* set the start display point of lcd */
void lcd_set_xy(uint16_t x,uint16_t y);
/* draw a point on the lcd */
void lcd_draw_point(uint16_t x,uint16_t y,uint16_t data);

/* bgr to rgb format conversion */
uint16_t lcd_bgr2rgb(uint16_t c);
/* draw a circle on the lcd */
void lcd_circle_draw(uint16_t x,uint16_t y,uint16_t r,uint16_t fc);
/* draw a line on the LCD */
void lcd_line_draw(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t color);
/* LCD rectangle draw */
void lcd_rect_draw(uint16_t x,uint16_t y,uint16_t w,uint16_t h,uint16_t line_color);
/* LCD box2 */
void lcd_box2(uint16_t x,uint16_t y,uint16_t w,uint16_t h,uint8_t mode);
/* draw a rectangle with color on the lcd */
void lcd_rect_color_draw(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t fc);
/* display button down */
void display_button_down(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2);
/* display button up */
void display_button_up(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2);
/* draw gbk16 font on the LCD */
void lcd_draw_font_gbk16(uint16_t x,uint16_t y,uint16_t fc,uint16_t bc,char *s);
/* draw gbk24 font on the LCD */
void lcd_draw_font_gbk24(uint16_t x,uint16_t y,uint16_t fc,uint16_t bc,char *s);
/* draw num32 font on the LCD */
void lcd_draw_font_num32(uint16_t x,uint16_t y,uint16_t fc,uint16_t bc,uint16_t num);

void lcd_region_set(uint16_t StartX, uint16_t StartY, uint16_t EndX, uint16_t EndY);

#ifdef __cplusplus
}
#endif

#endif /* GD32F3X0_LCD_EVAL_H */

