///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: idle_control.c
//  maintainer: Halley
///////////////////////////////////////////////////////////////////////////////
#include "app_config.h"
#include "idle_control.h"
#include "sys_app.h"
#include "sys_vol.h"

void IdleControl(void)
{
	uint16_t Msg = 0;

	APP_DBG("Enter Idle\n");
    
#ifndef FUNC_MIXER_SRC_EN
	AudioSampleRateSet(44100);
#endif
	SetModeSwitchState(MODE_SWITCH_STATE_DONE);
	while(Msg != MSG_COMMON_CLOSE)
	{
		Msg = MsgRecv(20);// 消息接收，无消息阻塞20ms，有消息立即返回
		switch(Msg)
		{
			case MSG_MODE:        //working mode changing
				Msg = MSG_COMMON_CLOSE;
				break;
				
			case MSG_POWER:
				gSys.NextModuleID = MODULE_ID_STANDBY;
				Msg = MSG_COMMON_CLOSE;
				break;
			
			default:
				CommonMsgProccess(Msg);
				break;
		}
	}
	APP_DBG("Exit Idle\n");
}
