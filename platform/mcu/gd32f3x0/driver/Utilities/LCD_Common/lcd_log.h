/*!
    \file  lcd_log.h
    \brief the header file of LCD log
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-28, V1.0.0, firmware for GD32F3x0
*/

#ifndef LCD_LOG_H
#define LCD_LOG_H

#include "gd32f3x0_lcd_eval.h"

/* initialize the LCD log module */
void lcd_log_init (void);
/* de-initialize the LCD log module */
void lcd_log_deinit (void);
/* display the application header (title) on the LCD screen */
void lcd_log_header_set (uint8_t *p_title, uint16_t start_x);
/* display the application footer (status) on the LCD screen */
void lcd_log_footer_set (uint8_t *p_status, uint16_t start_x);
/* redirect the printf to the lcd */
void lcd_log_print (uint8_t *pStr, uint16_t *pLine, uint16_t CharColor, uint16_t BackColor);
/* clear the text zone */
void lcd_log_text_zone_clear (uint16_t StartX, uint16_t StartY, uint16_t EndX, uint16_t EndY);

#endif /* LCD_LOG_H */

