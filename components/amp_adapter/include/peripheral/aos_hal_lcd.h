/**
 * @file lcd.h
 * @copyright Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef AOS_HAL_LCD_H
#define AOS_HAL_LCD_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup hal_lcd LCD
 *  lcd hal API.
 *
 *  @{
 */

#include <stdint.h>
#include <stdbool.h>

/**
 * Init the lcd module.
 *
 * @retrun 0 on success, otherwise will be failed.
 */
int32_t aos_hal_lcd_init(void);

/**
 * Uninit the lcd module.
 *
 * @retrun 0 on success, otherwise will be failed.
 */
int32_t aos_hal_lcd_uninit(void);

/**
 * lcd show rect function.
 *
 * @param[in]  x             x coordinate
 * @param[in]  y             y coordinate
 * @param[in]  w             Graphic Wide
 * @param[in]  h             Graphic High
 * @param[in]  buf           Graphic Framebuffer
 * @param[in]  rotate        Rotate (set true or false, The rotation angle is determined by the drivers)
 *
 * @return  0 on success, negative error on failure.
 */
int32_t aos_hal_lcd_show(int x, int y, int w, int h, uint8_t *buf, bool rotate);

/**
 * udisplay draw  function.
 *
 * @param[in]  x             x coordinate
 * @param[in]  y             y coordinate
 * @param[in]  w             Graphic Wide
 * @param[in]  h             Graphic High
 * @param[in]  color         Rectangle color
 *
 * @return  0 on success, negative error on failure.
 */
int32_t aos_hal_lcd_fill(int x, int y, int w, int h, uint32_t color);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* AOS_HAL_LCD_H */

