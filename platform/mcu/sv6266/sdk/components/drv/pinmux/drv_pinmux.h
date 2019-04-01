#ifndef __DRV_PINMUX_H__
#define __DRV_PINMUX_H__

#include "hal_pinmux.h"

/**
 * @file drv_pinmux.h
 *
 *  Configure pinmux settings.
 *
 */


/**
 * @brief  Enable pinmux settings of GPIO pin for selected function.
 *
 * @param  function [IN] Selected function[0:31] of GPIO pin.
 *
 * @return  -1              select function error!
 * @return   0              The operation completed successfully.
 */
int8_t drv_pinmux_manual_function_select_enable(pinmux_fun_t function);


/**
 * @brief  Disable pinmux settings of GPIO pin for selected function.
 *
 * @param  function [IN] Selected function[0:31] of GPIO pin.
 *
 * @return  -1              select function error!
 * @return   0              The operation completed successfully.
 */
int8_t drv_pinmux_manual_function_select_disable(pinmux_fun_t function);


#endif  // __DRV_PINMUX_H__
