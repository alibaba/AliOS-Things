/*!
    \file  lcd_log.h
    \brief header for the lcd_log.c file
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/


#ifndef  LCD_LOG_H
#define  LCD_LOG_H

#include "gd32f307c_lcd_eval.h"
#include <string.h>

/* initializes the LCD of GD EVAL board */
void gd_eval_lcd_init(void);
/* initialize the LCD Log module */
void lcd_log_init (void);
/* de-initialize the LCD Log module */
void lcd_log_deinit (void);
/* display the application header (title) on the LCD screen  */
void lcd_log_header_set (uint8_t *ptitle, uint16_t start_x);
/* display the application footer (status) on the LCD screen */
void lcd_log_footer_set (uint8_t *pstatus, uint16_t start_x);
/* redirect the printf to the LCD */
void lcd_log_print (uint8_t *pstr, 
                    uint16_t len, 
                    uint16_t char_color, 
                    uint16_t back_color);
/* clear the text zone */
void lcd_log_textzone_clear (uint16_t start_x, 
                             uint16_t start_y, 
                             uint16_t end_x, 
                             uint16_t end_y);


#endif /* LCD_LOG_H */ 

