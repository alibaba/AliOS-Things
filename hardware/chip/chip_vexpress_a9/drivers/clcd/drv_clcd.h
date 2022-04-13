#ifndef DRV_CLCD_H__
#define DRV_CLCD_H__

#include <aos/kernel.h>

#ifndef BSP_LCD_WIDTH
#define BSP_LCD_WIDTH   800
#endif

#ifndef BSP_LCD_HEIGHT
#define BSP_LCD_HEIGHT  480
#endif

#define PL111_DEVICE_NAME "pl111_fb"

int drv_clcd_hw_init(void);
int drv_clcd_hw_uninit(void);
#endif
