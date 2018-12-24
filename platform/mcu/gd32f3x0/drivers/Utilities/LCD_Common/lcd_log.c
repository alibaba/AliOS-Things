/*!
    \file  lcd_log.c
    \brief LCD log driver
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-28, V1.0.0, firmware for GD32F3x0
*/

#include "lcd_log.h"

/*!
    \brief      initialize the LCD log module
    \param[in]  none
    \param[out] none
    \retval     none
*/
void lcd_log_init (void)
{
    lcd_clear(BLACK);
}

/*!
    \brief      de-initialize the LCD log module
    \param[in]  none
    \param[out] none
    \retval     none
*/
void lcd_log_deinit (void)
{
}

/*!
    \brief      display the application header (title) on the LCD screen 
    \param[in]  p_title: pointer to the string to be displayed
    \param[in]  start_x: the start x position
    \param[out] none
    \retval     none
*/
void lcd_log_header_set (uint8_t *pTitle, uint16_t StartX)
{
    lcd_rect_color_draw(0, 0, 240, 40, BLUE);

    lcd_draw_font_gbk16(StartX, 10, RED, BLUE, (char *)pTitle);

}

/*!
    \brief      display the application footer (status) on the LCD screen 
    \param[in]  p_status: pointer to the string to be displayed
    \param[in]  start_x: the start x position
    \param[out] none
    \retval     none
*/
void lcd_log_footer_set (uint8_t *pStatus, uint16_t StartX)
{
    lcd_rect_color_draw(0, 280, 240, 320, BLUE);

    lcd_draw_font_gbk16(StartX, 290, RED, BLUE, (char *)pStatus);

}

/**
  * @brief  Clear the Text Zone 
  * @param  None 
  * @retval None
  */
void lcd_log_text_zone_clear(uint16_t StartX,
                             uint16_t StartY,
                             uint16_t EndX,
                             uint16_t EndY)
{
    lcd_rect_color_draw(StartX, StartY, EndX, EndY, BLACK);
}

/**
  * @brief  Redirect the printf to the lcd 
  * @param  pStr: pointer to string to be displayed
  * @param  Len: string length
  * @retval None
  */
void lcd_log_print (uint8_t *pStr, 
                    uint16_t *pLine, 
                    uint16_t CharColor, 
                    uint16_t BackColor)
{
    if(*pLine >= 280)
    {
        *pLine = 40;

        lcd_rect_color_draw(0, 40, 240, 280, BLACK);
    }

    lcd_draw_font_gbk16(2, *pLine, CharColor, BackColor, (char *)pStr);

    *pLine += 20;
}

