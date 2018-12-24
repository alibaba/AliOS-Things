/**
 ******************************************************************************
 * @file    emh_wlan.c
 * @author  William Xu
 * @version V1.0.0
 * @date    9-Apr-2018
 * @brief   wlan management AT commands
 ******************************************************************************
 *
 * Copyright (c) 2009-2018 MXCHIP Co.,Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************
 */


#include "ATCmdParser/ATCmdParser.h"
#include "emh_api.h"

/******************************************************************************
 *                              Function Definitions
 ******************************************************************************/

mx_status emh_wlan_get_para(char ssid[33], char pwd[33])
{
	char args[100], *arg_list[2];

	if (!(ATCmdParser_send("AT+WJAP=?")
	   && ATCmdParser_recv("+WJAP:%100[^\r]\r\n",args)
	   && ATCmdParser_recv("OK\r\n"))) {
		return kReadErr;
	}
	
	if (2 != ATCmdParser_analyse_args(args, arg_list, 2)) {
		return kMalformedErr;
	};
	if (ssid) strcpy(ssid, arg_list[0]);
	if (pwd)  strcpy(pwd,  arg_list[1]);
	
	return kNoErr;
}

emh_arg_wlan_sta_t emh_wlan_get_sta_status(void)
{
	char arg[20];

	if (!(ATCmdParser_send("AT+WJAPS")
	   && ATCmdParser_recv("+WJAPS:%20[^\r]\r\n",arg)
	   && ATCmdParser_recv("OK\r\n"))) {
		return EMH_ARG_ERR;
	}
	
	return emh_arg_for_arg( EMH_ARG_WLAN_STA, arg);
}

void emh_wlan_event_handler(void)
{
	mx_status err = kNoErr;
	char arg[15];

	// parse out the packet
	require_action(ATCmdParser_recv("%100[^\r]\r\n", arg), exit, err = kMalformedErr);
		
	emh_arg_wlan_ev_t event = emh_arg_for_arg(EMH_ARG_WLAN_EV, arg);
	require_action(event != EMH_ARG_ERR, exit,  err = kMalformedErr);
	
	emh_ev_wlan(event);
	
exit:
	if (err == kMalformedErr) emh_ev_unknown();
	return;
}
