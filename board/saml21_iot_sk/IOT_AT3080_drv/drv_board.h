

#ifndef _DRV_BOARD_H_
#define _DRV_BOARD_H_

#include "mx_toolchain.h"
#include "mx_common.h"

#include "oled.h"
#include "color_led.h"
#include "sht2x.h"
#include "io_button.h"
#include "mx_hal.h"

#ifdef __cplusplus
extern "C"
{
#endif

void drv_board_init(void);
void drv_board_test(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

