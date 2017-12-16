////////////////////////////////////////////////////////////////////////////////
//                   Mountain View Silicon Tech. Inc.
//		Copyright 2014, Mountain View Silicon Tech. Inc., ShangHai, China
//                   All rights reserved.
//
//		Filename	:power_key_driver.c
//
//		Description	:
//					power keyboard driver(soft push botton)
//
//             maintainer: lujiangang
//
//		Changelog	:
//					2014-09-25	Created Source File
///////////////////////////////////////////////////////////////////////////////
#include "type.h"
#include "adc.h"
#include "app_config.h"
#include "os.h"
#include "gpio.h"
#include "irqs.h"
#include "msgq.h"
#include "app_msg.h"
#include "wakeup.h"

#if (defined(FUNC_POWER_KEY_EN) && defined(USE_POWERKEY_SOFT_PUSH_BUTTON))

	#warning	"MV: PUSH BUTTON POWER KEY ENABLE!"

#define 	POWER_KEY_JITTER_TIME		100			//消抖时间，该时间和软开关开关机硬件时间有关
#define 	POWER_KEY_CP_TIME			100			


//extern bool PowerkeyGetOnkeyReg(void);


typedef enum _POWER_KEY_STATE
{
	POWER_KEY_STATE_IDLE,
	POWER_KEY_STATE_JITTER,
	POWER_KEY_STATE_PRESS_DOWN,
	POWER_KEY_STATE_CP,

} POWER_KEY_STATE;


TIMER			PowerKeyWaitTimer;
POWER_KEY_STATE	PowerKeyState;
										
static const uint16_t PowerKeyEvent[2] = {MSG_NONE/*按键开始*/, MSG_PWR_KEY_SP/*短按松开*/};	


// Initialize POWER_KEY scan operation.
void PowerKeyScanInit(void)
{
	DBG("PowerKeyScanInit*******\n");
	PowerKeyState = POWER_KEY_STATE_IDLE;
}


// POWER_KEY与普通的按键不同，连接按钮开关（软开关）时的主要作用还是系统开关机，当然，也允许复用短按功能。
// 短按产生时，推送短按消息。超过短按区间，此处不做任何处理，由系统硬件检测关机。
uint16_t PowerKeyScan(void)							
{
	switch(PowerKeyState)
	{
		case POWER_KEY_STATE_IDLE:
			if(!PowerkeyGetOnkeyReg())
			{
				return MSG_NONE;
			}
			else
			{	
				TimeOutSet(&PowerKeyWaitTimer, POWER_KEY_JITTER_TIME);
				PowerKeyState = POWER_KEY_STATE_JITTER;
			}
			break;
		case POWER_KEY_STATE_JITTER:
			if(!PowerkeyGetOnkeyReg())
			{
				PowerKeyState = POWER_KEY_STATE_IDLE;
			}
			else if(IsTimeOut(&PowerKeyWaitTimer))
			{
				PowerKeyState = POWER_KEY_STATE_PRESS_DOWN;
				TimeOutSet(&PowerKeyWaitTimer, POWER_KEY_CP_TIME);
				return PowerKeyEvent[0];			//return key sp value
			}
			break;
			
		case POWER_KEY_STATE_PRESS_DOWN:
			if(!PowerkeyGetOnkeyReg())
			{
				PowerKeyState = POWER_KEY_STATE_IDLE;
				return PowerKeyEvent[1];
			}
			else if(IsTimeOut(&PowerKeyWaitTimer))
			{
				PowerKeyState = POWER_KEY_STATE_CP;
				return PowerKeyEvent[0];
			}
			break;
			
		case POWER_KEY_STATE_CP:
			//此处仅保证一次按键不会响应多次短按
			if(!PowerkeyGetOnkeyReg())
			{
				PowerKeyState = POWER_KEY_STATE_IDLE;
				return PowerKeyEvent[1];//return MSG_NONE;				
			}
			else
			{
				//do no thing
			}
			break;
			
		default:
			PowerKeyState = POWER_KEY_STATE_IDLE;
			break;
	}
	return MSG_NONE;
}

#endif
