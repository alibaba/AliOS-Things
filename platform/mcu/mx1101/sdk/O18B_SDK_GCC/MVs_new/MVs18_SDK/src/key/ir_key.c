////////////////////////////////////////////////////////////////////////////////
//                   Mountain View Silicon Tech. Inc.
//		Copyright 2013, Mountain View Silicon Tech. Inc., ShangHai, China
//                   All rights reserved.
//
//		Filename	:ir_key_driver.c
//
//            maintainer: Halley
//
//		Description	:
//					ir keyboard driver
//
//		Changelog	:
//					2013-01-23	Created by Robert
///////////////////////////////////////////////////////////////////////////////
#include "type.h"
#include "ir.h"
#include "app_config.h"
#include "os.h"
#include "gpio.h"
#include "irqs.h"
#include "msgq.h"
#include "app_msg.h"

#ifdef	FUNC_IR_KEY_EN

#define 	IR_KEY_SCAN_TIME			5
#define 	IR_KEY_JITTER_TIME			30
#define		IR_KEY_CP_TIME				1000	//CP condition is 1s
#define 	IR_KEY_CPH_TIME				350

#define		IR_KEY_SUM					21		//IR KEY SUM

typedef enum _IR_KEY_STATE
{
    IR_KEY_STATE_IDLE,
    IR_KEY_STATE_JITTER,
    IR_KEY_STATE_PRESS_DOWN,
    IR_KEY_STATE_CP

} IR_KEY_STATE;

static TIMER			IrKeyHoldTimer;
static TIMER			IrKeyWaitTimer;
static TIMER			IrKeyScanTimer;
static IR_KEY_STATE		IrKeyState;

static uint32_t 		IrKeyVal;


//IrKeyVal[31:16]: key value
//IrKeyVal[15:0]: manu id
#define IR_MANU_ID		0x7F80

// Customer IR key value table, fix it by case.
static const uint8_t gIrVal[IR_KEY_SUM] =
{
	0xED,		// POWER
	0xE5,		// MODE
	0xE1,		// MUTE

	0xFE,		// PLAY/PAUSE
	0xFD,		// PRE
	0xFC,		// NEXT

	0xFB,		// EQ
	0xFA,		// VOL-
	0xF9,		// VOL+

	0xF8,		// 0
	0xF7,		// REPEAT
	0xF6,		// SCN

	0xF5,		// 1
	0xE4,		// 2
	0xE0,		// 3

	0xF3,		// 4
	0xF2,		// 5
	0xF1,		// 6

	0xFF,		// 7
	0xF0,		// 8
	0xE6,		// 9
};

static const uint16_t IrKeyEvent[][5] = 
{
//	PDS(按键开始)		SPR(短按松开)		CPS(长按开始)		CPH(长按保持)		CPR(长按松开)
	{MSG_NONE,			MSG_POWER, 			MSG_NONE, 			MSG_NONE, 			MSG_NONE		},	//POWER
	{MSG_NONE,			MSG_MODE, 			MSG_POWER, 			MSG_NONE, 			MSG_NONE		},	//MODE
	{MSG_NONE,			MSG_MUTE, 			MSG_REPEAT_AB, 		MSG_NONE, 			MSG_NONE		},	//MUTE	

	{MSG_NONE,			MSG_PLAY_PAUSE, 	MSG_STOP, 			MSG_NONE,	 		MSG_NONE		},	//PLAY/PAUSE
	{MSG_NONE,			MSG_PRE, 			MSG_FB_START, 		MSG_FB_START,	 	MSG_FF_FB_END	},	//PRE																						
	{MSG_NONE,			MSG_NEXT, 			MSG_FF_START,		MSG_FF_START, 		MSG_FF_FB_END	},	//NEXT

	{MSG_NONE,			MSG_EQ, 			MSG_3D, 			MSG_NONE, 			MSG_NONE		},	//EQ
	{MSG_NONE,			MSG_VOL_DW, 		MSG_VOL_DW, 		MSG_VOL_DW, 		MSG_VOL_DW		},	//VOL-
	{MSG_NONE,			MSG_VOL_UP, 		MSG_VOL_UP, 		MSG_VOL_UP, 		MSG_VOL_UP		},	//VOL+

	{MSG_NONE,			MSG_NUM_0, 			MSG_NONE, 			MSG_NONE, 			MSG_NONE		},  //0	
	{MSG_NONE,			MSG_REPEAT, 		MSG_REPEAT_AB,		MSG_NONE, 			MSG_NONE		},	//REPEAT
	{MSG_NONE,			MSG_REMIND, 		MSG_LANG, 			MSG_NONE, 			MSG_NONE		},	//USB/SD

	{MSG_NONE,			MSG_NUM_1,			MSG_REMIND,			MSG_NONE, 			MSG_NONE		},	//1
	{MSG_NONE,			MSG_NUM_2,			MSG_NONE, 			MSG_NONE, 			MSG_NONE		},	//2
	{MSG_NONE,			MSG_NUM_3,			MSG_NONE, 			MSG_NONE, 			MSG_NONE		},	//3

	{MSG_NONE,			MSG_NUM_4,			MSG_NONE, 			MSG_NONE,			MSG_NONE		},	//4
	{MSG_NONE,			MSG_NUM_5,			MSG_NONE, 			MSG_NONE,			MSG_NONE		},	//5
	{MSG_NONE,			MSG_NUM_6, 			MSG_NONE, 			MSG_NONE, 			MSG_NONE		},	//6
	
	{MSG_NONE,			MSG_NUM_7,			MSG_NONE, 			MSG_NONE, 			MSG_NONE		},	//7
	{MSG_NONE,			MSG_NUM_8, 			MSG_NONE, 			MSG_NONE, 			MSG_NONE		},	//8
	{MSG_NONE,			MSG_NUM_9, 			MSG_NONE, 			MSG_NONE, 			MSG_NONE		},	//9
};	


//get ir key index
static uint8_t GetIrKeyIndex(void)
{
	static uint8_t KeyIndex = 0xFF;
	static uint8_t IrCphCnt = 0;
	bool IsIrShortPrs, IsIrContinuePrs;

	//DBG("[IR] Enter GetIrKeyIndex()\n");
	if(IrIsKeyCome())
	{
		IsIrShortPrs = TRUE;
		IsIrContinuePrs = FALSE;
		IrKeyVal = IrGetKeyCode();
		DBG("Key: %08LX\n", IrKeyVal);
	}
	else if(IrIsContinuePrs())
	{
		IsIrShortPrs = FALSE;
		IsIrContinuePrs = TRUE;
	}
	else
	{
		IsIrShortPrs = FALSE;
		IsIrContinuePrs = FALSE;
	}
	if(IsIrShortPrs || IsIrContinuePrs)
	{
		//fast response
		if(IsIrShortPrs)
		{
			IrCphCnt = 0;
		}
		if(IrCphCnt < 5)
		{
			IrCphCnt++;
		}
		TimeOutSet(&IrKeyHoldTimer, 70 * IrCphCnt);

		if(((IrKeyVal >> 24) & 0x000000FF) + ((IrKeyVal >> 16) & 0x000000FF) != 0xFF)
		{
			KeyIndex = 0xFF;
			return 0xFF;
		}

		if((IrKeyVal & 0x0000FFFF) == IR_MANU_ID)
		{
			for(KeyIndex = 0; KeyIndex < IR_KEY_SUM; KeyIndex++)
			{
				if(((IrKeyVal >> 24) & 0x000000FF) == gIrVal[KeyIndex])
				{
					return KeyIndex;
				}
			}
		}
		KeyIndex = 0xFF;
		return 0xFF;
	}
	else if(!IsTimeOut(&IrKeyHoldTimer))
	{
		return KeyIndex;
	}
	else
	{
		IrKeyVal = 0;
		return 0xFF;
	}
}


// check ir key input
// return : low 14 bits -- keyid，high 2 bits -- keystate
uint16_t IrKeyScan(void)
{
	static 	uint8_t 	PreKeyIndex = 0xFF;
	uint8_t				KeyIndex;

	if(!IsTimeOut(&IrKeyScanTimer))
	{
		return MSG_NONE;
	}
	TimeOutSet(&IrKeyScanTimer, IR_KEY_SCAN_TIME);

	KeyIndex = GetIrKeyIndex();

	switch(IrKeyState)
	{
		case IR_KEY_STATE_IDLE:
			if(KeyIndex == 0xFF)
			{
				return MSG_NONE;
			}

			//DBG("IR KEY PDS!\n");
			PreKeyIndex = KeyIndex;
			TimeOutSet(&IrKeyWaitTimer, IR_KEY_CP_TIME);
			IrKeyState = IR_KEY_STATE_PRESS_DOWN;
			return IrKeyEvent[PreKeyIndex][0];

		case IR_KEY_STATE_PRESS_DOWN:
			if(PreKeyIndex != KeyIndex)
			{
				//return key sp value
				//DBG("IR KEY SP!%bu\n", PreKeyIndex);
				IrKeyState = IR_KEY_STATE_IDLE;
				return IrKeyEvent[PreKeyIndex][1];
			}
			else if(IsTimeOut(&IrKeyWaitTimer))
			{
				//return key cp value
				//DBG("IR KEY CPS!\n");
				TimeOutSet(&IrKeyWaitTimer, IR_KEY_CPH_TIME);
				IrKeyState = IR_KEY_STATE_CP;
				return IrKeyEvent[PreKeyIndex][2];
			}
			break;

		case IR_KEY_STATE_CP:
			if(PreKeyIndex != KeyIndex)
			{
				//return key cp value
				//DBG("IR CPR\n");
				IrKeyState = IR_KEY_STATE_IDLE;
				return IrKeyEvent[PreKeyIndex][4];
			}
			else if(IsTimeOut(&IrKeyWaitTimer))
			{
				TimeOutSet(&IrKeyWaitTimer, IR_KEY_CPH_TIME);
				return IrKeyEvent[PreKeyIndex][3];
			}
			break;

		default:
			IrKeyState = IR_KEY_STATE_IDLE;
			break;
	}
	//DBG("%d, %8X\n", IrIsKeyCome(), IrGetKeyCode());
	return MSG_NONE;
}

int32_t IrKeyInit(void)
{
	IrKeyState = IR_KEY_STATE_IDLE;
	TimeOutSet(&IrKeyScanTimer, 0);
	IrInit();
	IrGpioSel(IR_KEY_PORT);
	IrIgnoreLeadHeader(1);
	return 0;
}
#endif


