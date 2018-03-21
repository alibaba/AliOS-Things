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
	u8 dword_index = 0;
	u8 shift_bits = 0; 
	u32 Temp = 0;
	
	assert_param(pin != NC);

	/* 16bit per one pad */
	/* get PADCTR dword inedex*/
	dword_index = pin >> 1;

	/* get shift_bits in PADCTR dword */
	shift_bits = (pin % 2) * 16; 

	/* get PADCTR */
	Temp = PINMUX->PADCTR[dword_index];

	/* clear Pin_Num PADCTR */
	if(shift_bits){
		Temp &= 0x0000FFFF;
	}else{
		Temp &= 0xFFFF0000;
	}
	
	/* set needs function */
	Temp |= ((function & 0xFF) | BIT(8)) << shift_bits; /* bit8 is close pad power down */

	/* set PADCTR register */
	PINMUX->PADCTR[dword_index] = Temp;
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

		case OpenDrain:
		default:
			pull_type = GPIO_PuPd_NOPULL;
		break;
	}

	PAD_PullCtrl((u32)pin, (u32)pull_type);

}
