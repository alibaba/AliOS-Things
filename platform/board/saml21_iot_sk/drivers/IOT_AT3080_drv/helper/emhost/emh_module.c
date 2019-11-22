/**
 ******************************************************************************
 * @file    emh_module.c
 * @author  William Xu
 * @version V1.0.0
 * @date    9-Apr-2018
 * @brief   EMW module operation AT commands
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
 *                              Variable Definitions
 ******************************************************************************/

static char _fw_version[32];

/******************************************************************************
 *                             Function Declarations
 ******************************************************************************/

extern void emh_alisds_event_handler(void);
extern void emh_wlan_event_handler(void);

/******************************************************************************
 *                              Function Definitions
 ******************************************************************************/

mx_status emh_module_reset(void)
{
	if (!(ATCmdParser_send("AT+REBOOT")
	&&  ATCmdParser_recv("OK\r\n"))) {
		return kGeneralErr;
	}
	for (int i = 0; i < 5; i++) {
		if (ATCmdParser_send("AT")
		&&  ATCmdParser_recv("OK\r\n")
		&&  ATCmdParser_send("AT+UARTE=OFF")
		&&  ATCmdParser_recv("OK\r\n")) {
			return kNoErr;
		}
	}
	return kGeneralErr;
}

mx_status emh_module_restore_settings(void)
{
	if (!(ATCmdParser_send("AT+FACTORY")
	&&  ATCmdParser_recv("OK\r\n"))) {
		return kGeneralErr;
	}
	
	for (int i = 0; i < 5; i++) {
		if (ATCmdParser_send("AT")
		&&  ATCmdParser_recv("OK\r\n")
		&&  ATCmdParser_send("AT+UARTE=OFF")
		&&  ATCmdParser_recv("OK\r\n")) {
			return kNoErr;
		}
	}

	return kGeneralErr;
}

mx_status emh_init(void)
{
	ATCmdParser_init("\r","\r\n", 1000, false);
		
	for (int i = 0; i < 2; i++) {
		if ( kNoErr == emh_module_reset()) {
			ATCmdParser_add_oob("+ALINKEVENT:",	emh_alisds_event_handler);
			ATCmdParser_add_oob("+WEVENT:",		emh_wlan_event_handler);
			return kNoErr;
		}
	}
		
	return kGeneralErr;
}



const char *emh_module_get_fw_version(void)
{
	if (!(ATCmdParser_send("AT+FWVER?")
       && ATCmdParser_recv("+FWVER:%32[^\r]\r\nOK\r\n", _fw_version))) {
        return NULL;
    }
	return _fw_version;
}

uint32_t emh_module_get_tick(void)
{
	uint32_t tick;
	if (!(ATCmdParser_send("AT+SYSTIME?")
	   && ATCmdParser_recv("+SYSTIME:%d\r\nOK\r\n", &tick))) {
		return 0;
	}
	return tick;	
}

void emh_runloop(void) {
	while (ATCmdParser_process_oob());
}
