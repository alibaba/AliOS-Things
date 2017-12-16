/*
*****************************************************************************
*					Mountain View Silicon Tech. Inc.
*	Copyright 2010, Mountain View Silicon Tech. Inc., ShangHai, China
*					All rights reserved.
*
* Filename:			coding_key.c
* Description:		coding_key scan and process c file
*
*  maintainer: Fenston
*
* Change History:
*			chf	- 01/22/2014 - V0.1
*					- created
******************************************************************************
*/
#include "type.h"
#include "app_config.h"
#include "os.h"
#include "gpio.h"
#include "irqs.h"
#include "msgq.h"
#include "app_msg.h"

#ifdef FUNC_CODING_KEY_EN

//clockwise rotation
//    ----|		   |--------|		 |-------------------
//A       |        |        |        |
//        |--------|        |--------|

//     -------|        |--------|		 |---------------
//B           |        |        |		 |
//            |--------|        |--------|


//counterclockwise rotation
//    --------|		   |--------|		 |---------------
//A           |		   |        |		 |
//            |--------|        |--------|

//     ---|        |--------|		 |-------------------
//B       |        |        |		 |
//        |--------|        |--------|


static uint8_t		ClockWiseCnt;
static uint8_t		CounterClockWiseCnt;

// Initialize coding key scan (GPIO) operation.
// Config interrupt at negative edge of signal-A
void CodingKeyInit(void)
{
	//enable pull up resister.
	GpioClrRegOneBit(CODING_KEY_A_PORT_OE, CODING_KEY_A_BIT);
	GpioClrRegOneBit(CODING_KEY_A_PORT_PU, CODING_KEY_A_BIT);
	GpioClrRegOneBit(CODING_KEY_A_PORT_PD, CODING_KEY_A_BIT);

	//enable pull up resister.
	GpioClrRegOneBit(CODING_KEY_B_PORT_OE, CODING_KEY_B_BIT);
	GpioClrRegOneBit(CODING_KEY_B_PORT_PU, CODING_KEY_B_BIT);
	GpioClrRegOneBit(CODING_KEY_B_PORT_PD, CODING_KEY_B_BIT);

	//enable int
	GpioIntEn(CODING_KEY_A_PORT_INT, CODING_KEY_A_BIT, GPIO_NEG_EDGE_TRIGGER);

	ClockWiseCnt = 0;
	CounterClockWiseCnt = 0;

	//enable gpio irqc
	NVIC_EnableIRQ(GPIO_IRQn);
}


// Coding key signal-A interrupt.
__attribute__((section(".driver.isr"))) void GpioInterrupt(void)
{
	if(GpioIntFlagGet(CODING_KEY_A_PORT_INT) == CODING_KEY_A_BIT)
	{
		GpioIntClr(CODING_KEY_A_PORT_INT, CODING_KEY_A_BIT);
		if((GpioGetReg(CODING_KEY_A_PORT_IN) & CODING_KEY_A_BIT) || (ClockWiseCnt != 0) || (CounterClockWiseCnt != 0))
		{			
			return;
		}

		if(GpioGetReg(CODING_KEY_B_PORT_IN) & CODING_KEY_B_BIT)
		{
			//clockwise rotation
			ClockWiseCnt++;
		}
		else
		{
			//counterclockwise rotation
			CounterClockWiseCnt++;
		}
	}
}

// Key process, image key value to key event.
uint16_t CodingKeyScan(void)
{
	uint16_t Msg = MSG_NONE;

	if(ClockWiseCnt)
	{
		Msg = MSG_VOL_UP;
	}
	else if(CounterClockWiseCnt)
	{
		Msg = MSG_VOL_DW;
	}
	ClockWiseCnt = 0;
	CounterClockWiseCnt = 0;

	return Msg;
}
#else // FUNC_CODING_KEY_EN
__attribute__((section(".driver.isr"))) void GpioInterrupt(void)
{

}
#endif // FUNC_CODING_KEY_EN

