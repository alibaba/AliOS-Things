/* mbed Microcontroller Library
 *******************************************************************************
 * Copyright (c) 2015, Realtek Semiconductor Corp.
 * All rights reserved.
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *******************************************************************************
 */
 
#ifndef ANALOGIN_EX_API_H
#define ANALOGIN_EX_API_H

#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ANALOGIN_RX_DMA_COMPLETE     = 0,
}AnalogInCallback;

void analogin_set_user_callback(analogin_t *obj, AnalogInCallback analogin_cb, void(*analogin_callback)(void *));
void analogin_clear_user_callback(analogin_t *obj, AnalogInCallback analogin_cb);
uint8_t analogin_read_u16_dma (analogin_t * obj, uint16_t *buf, uint16_t length);


#ifdef __cplusplus
}
#endif

#endif



