////////////////////////////////////////////////////////////////////////////////
//                   Mountain View Silicon Tech. Inc.
//		Copyright 2013, Mountain View Silicon Tech. Inc., ShangHai, China
//                   All rights reserved.
//
//		Filename	:adc_key_driver.c
//
//            maintainer: Halley
//
//		Description	:
//					adc keyboard driver
//
//		Changelog	:
//					2013-05-31	Created Source File
///////////////////////////////////////////////////////////////////////////////
#include "type.h"
#include "adc.h"
#include "app_config.h"
#include "os.h"
#include "gpio.h"
#include "irqs.h"
#include "msgq.h"
#include "app_msg.h"

#ifdef FUNC_ADC_KEY_EN


#define 	ADC_KEY_SCAN_TIME			10
#define 	ADC_KEY_JITTER_TIME			30
#define 	ADC_KEY_CP_TIME				1000
#define 	ADC_KEY_CPH_TIME			200
#define		ADC_KEY_COUNT				10 			//key count per adc channel
#define		ADC_KEY_FULL_VAL			4096		//

static const uint16_t AdcKeyEvent[][5] = 
{
//	PDS(按键开始)		SPR(短按松开)		CPS(长按开始)		CPH(长按保持)		CPR(长按松开)
	{MSG_NONE,			MSG_PLAY_PAUSE, 	MSG_STOP, 			MSG_NONE, 			MSG_NONE		},	//SW1
	{MSG_NONE,			MSG_PRE, 			MSG_FB_START, 		MSG_FB_START, 		MSG_FF_FB_END	},	//SW2
	{MSG_NONE,			MSG_NEXT, 			MSG_FF_START, 		MSG_FF_START, 		MSG_FF_FB_END	},	//SW3	
	{MSG_NONE,			MSG_VOL_UP, 		MSG_VOL_UP, 		MSG_VOL_UP, 		MSG_NONE		},	//SW4
	{MSG_NONE,			MSG_VOL_DW, 		MSG_VOL_DW, 		MSG_VOL_DW, 		MSG_NONE		},	//SW5
																						
	{MSG_NONE,			MSG_EQ, 			MSG_3D,				MSG_NONE, 			MSG_NONE		},	//SW6
	{MSG_NONE,			MSG_MUTE, 			MSG_VB, 			MSG_NONE, 			MSG_NONE		},	//SW7
	{MSG_NONE,			MSG_REC, 			MSG_REC_PLAYBACK, 	MSG_NONE, 			MSG_NONE		},	//SW8
	{MSG_NONE,			MSG_MODE, 			MSG_POWER, 			MSG_NONE, 			MSG_NONE		},	//SW9
	{MSG_NONE,			MSG_NONE, 			MSG_NONE, 			MSG_NONE, 			MSG_NONE		},  //无实际按键
	
	{MSG_NONE,			MSG_MENU, 			MSG_BROWSE,			MSG_NONE, 			MSG_NONE		},	//SW10
	{MSG_NONE,			MSG_FOLDER_PRE, 	MSG_NONE, 			MSG_NONE, 			MSG_NONE		},	//SW11
	{MSG_NONE,			MSG_FOLDER_NEXT,	MSG_NONE,			MSG_NONE, 			MSG_NONE		},	//SW12
	{MSG_NONE,			MSG_REPEAT,			MSG_REPEAT_AB, 		MSG_NONE, 			MSG_NONE		},	//SW13
	{MSG_NONE,			MSG_REMIND,			MSG_LANG, 			MSG_NONE, 			MSG_NONE		},	//SW14

	{MSG_NONE,			MSG_DRC,			MSG_NONE, 			MSG_NONE,			MSG_NONE		},	//SW15
	{MSG_NONE,			MSG_NONE,			MSG_NONE, 			MSG_NONE,			MSG_NONE		},	//SW16
	{MSG_NONE,			MSG_RTC_SET_TIME, 	MSG_NONE, 			MSG_NONE, 			MSG_NONE		},	//SW17
	{MSG_NONE,			MSG_RTC_SET_ALARM,	MSG_NONE, 			MSG_NONE, 			MSG_NONE		},	//SW18
	{MSG_NONE,			MSG_NONE, 			MSG_NONE, 			MSG_NONE, 			MSG_NONE		},	//无实际按键
};	

typedef enum _ADC_KEY_STATE
{
    ADC_KEY_STATE_IDLE,
    ADC_KEY_STATE_JITTER,
    ADC_KEY_STATE_PRESS_DOWN,
    ADC_KEY_STATE_CP

} ADC_KEY_STATE;


TIMER			AdcKeyWaitTimer;
TIMER			AdcKeyScanTimer;
ADC_KEY_STATE	AdcKeyState;


// Key process, image key value to key event.
static uint8_t AdcChannelKeyGet(uint8_t Channel)
{
	uint16_t	Val;
	uint8_t	KeyIndex;

	Val = SarAdcChannelGetValue(Channel);

//	if(Val >= (ADC_KEY_FULL_VAL - (ADC_KEY_FULL_VAL / ADC_KEY_COUNT) / 2))
	if(Val >= (ADC_KEY_FULL_VAL - (ADC_KEY_FULL_VAL / ADC_KEY_COUNT)))
	{
		return 0xFF;	//no pressed key.
	}
	KeyIndex = (Val + (ADC_KEY_FULL_VAL / ADC_KEY_COUNT) / 2) * ADC_KEY_COUNT / ADC_KEY_FULL_VAL;
	return KeyIndex;
}


static uint8_t GetAdcKeyIndex(void)
{
	uint8_t KeyIndex = 0xFF;

#ifdef ADC_KEY_PORT_CH1
	KeyIndex = AdcChannelKeyGet(ADC_KEY_PORT_CH1);
#endif

#ifdef ADC_KEY_PORT_CH2
	if(KeyIndex == 0xFF)
	{
		KeyIndex = AdcChannelKeyGet(ADC_KEY_PORT_CH2);
#ifdef ADC_KEY_PORT_CH1
		if(KeyIndex != 0xFF)
		{
			KeyIndex += ADC_KEY_COUNT;
		}
#endif
	}
#endif

	return KeyIndex;
}


// adc key scan
// return: 0---no key, else---key msg
uint16_t AdcKeyScan(void)
{
	static 	uint8_t 	PreKeyIndex = 0xFF;
	uint8_t				KeyIndex;

	if(!IsTimeOut(&AdcKeyScanTimer))
	{
		return MSG_NONE;
	}
	TimeOutSet(&AdcKeyScanTimer, ADC_KEY_SCAN_TIME);

	KeyIndex = GetAdcKeyIndex();

	switch(AdcKeyState)
	{
		case ADC_KEY_STATE_IDLE:
			if(KeyIndex == 0xFF)
			{
				return MSG_NONE;
			}

			PreKeyIndex = KeyIndex;
			TimeOutSet(&AdcKeyWaitTimer, ADC_KEY_JITTER_TIME);
			//DBG("GOTO JITTER!\n");
			AdcKeyState = ADC_KEY_STATE_JITTER;

		case ADC_KEY_STATE_JITTER:
			if(PreKeyIndex != KeyIndex)
			{
				//DBG("GOTO IDLE Because jitter!\n");
				AdcKeyState = ADC_KEY_STATE_IDLE;
			}
			else if(IsTimeOut(&AdcKeyWaitTimer))
			{
				//DBG("GOTO PRESS_DOWN!\n");
				TimeOutSet(&AdcKeyWaitTimer, ADC_KEY_CP_TIME);
				AdcKeyState = ADC_KEY_STATE_PRESS_DOWN;
				return AdcKeyEvent[PreKeyIndex][0];
			}
			break;

		case ADC_KEY_STATE_PRESS_DOWN:
			if(PreKeyIndex != KeyIndex)
			{
				//return key sp value
				//DBG("ADC KEY SP%bu\n", PreKeyIndex);
				AdcKeyState = ADC_KEY_STATE_IDLE;
				return AdcKeyEvent[PreKeyIndex][1];
			}
			else if(IsTimeOut(&AdcKeyWaitTimer))
			{
				//return key cp value
				//DBG("ADC KEY CPS!\n");
				TimeOutSet(&AdcKeyWaitTimer, ADC_KEY_CPH_TIME);
				AdcKeyState = ADC_KEY_STATE_CP;
				return AdcKeyEvent[PreKeyIndex][2];
			}
			break;

		case ADC_KEY_STATE_CP:
			if(PreKeyIndex != KeyIndex)
			{
				//return key cp value
				//DBG("ADC KEY CPR!\n");
				AdcKeyState = ADC_KEY_STATE_IDLE;
				return AdcKeyEvent[PreKeyIndex][4];
			}
			else if(IsTimeOut(&AdcKeyWaitTimer))
			{
				//return key cph value
				//DBG("ADC KEY CPH!\n");
				TimeOutSet(&AdcKeyWaitTimer, ADC_KEY_CPH_TIME);
				return AdcKeyEvent[PreKeyIndex][3];
			}
			break;

		default:
			AdcKeyState = ADC_KEY_STATE_IDLE;
			break;
	}

	return MSG_NONE;
}

int32_t AdcKeyInit(void)
{
	DBG("AdcKeyInit()\n");

	AdcKeyState = ADC_KEY_STATE_IDLE;
	TimeOutSet(&AdcKeyScanTimer, 0);

#ifdef ADC_KEY_PORT_CH1
	SarAdcGpioSel(ADC_KEY_PORT_CH1);
#endif

#ifdef ADC_KEY_PORT_CH2
	SarAdcGpioSel(ADC_KEY_PORT_CH2);
#endif

	return 0;
}

#endif


