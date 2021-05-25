/* mbed Microcontroller Library
 *******************************************************************************
 * Copyright (c) 2014, Realtek Semiconductor Corp.
 * All rights reserved.
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *******************************************************************************
 */
#include "objects.h"
#include "pinmap.h"

/**
 * Configure pin enable and function
 */
void pin_function(PinName pin, int function) 
{
	u8 pin_index = 0;
	u32 Temp = 0;
	
	assert_param(pin != NC);

	/* 32bit per one pad */
	/* get PADCTR index*/
	pin_index = (pin) & 0x3F;

	/* get PADCTR */
	Temp = PINMUX->PADCTR[pin_index];

	/* set needs function */
	Temp &= ~PAD_BIT_MASK_FUNCTION_ID;
	Temp |= (function & PAD_BIT_MASK_FUNCTION_ID); 
	Temp &= ~PAD_BIT_SHUT_DWON; /* close pad power down */

	/* set PADCTR register */
	PINMUX->PADCTR[pin_index] = Temp;
}

/**
 * Configure pin pull-up/pull-down
 */
void pin_mode(PinName pin, PinMode mode) 
{
	u8 pull_type;
	
	switch (mode) {
		case PullNone:
			pull_type = GPIO_PuPd_NOPULL;
		break;

		case PullDown:
			pull_type = GPIO_PuPd_DOWN;
		break;

		case PullUp:
			pull_type = GPIO_PuPd_UP;
		break;

		default:
			pull_type = GPIO_PuPd_NOPULL;
		break;
	}

	PAD_PullCtrl((u32)pin, (u32)pull_type);

}
