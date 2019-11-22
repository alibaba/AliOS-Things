/**
 ******************************************************************************
 * @file    emh_alisds.c
 * @author  William Xu
 * @version V1.0.0
 * @date    9-Apr-2018
 * @brief   Alicloud SDS service AT commands API
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


#include <stdlib.h>
#include <string.h>

#include "emh_api.h"
#include "ATCmdParser/ATCmdParser.h"

/******************************************************************************
 *                              Function Definitions
 ******************************************************************************/

mx_status emh_alisds_config(const emh_alisds_config_t* config)
{
	char args[200], *arg_list[5];
	const char* format_arg = emh_arg_for_type(EMH_ARG_ALISDS_FORMAT, config->product_info.format);
	
	/* Check product info*/
	if (!(ATCmdParser_send("AT+ALINKPRODUCT?")
	   && ATCmdParser_recv("+ALINKPRODUCT:%200[^\r]\r\n",args)
	   && ATCmdParser_recv("OK\r\n"))) {
		return kReadErr;
	}

	if (4 != ATCmdParser_analyse_args(args, arg_list, 4)) {
		return kMalformedErr;
	}

	if (strcmp(arg_list[0], config->product_info.name)
	 || strcmp(arg_list[1], config->product_info.module)
	 || strcmp(arg_list[2], config->product_info.key)
	 || strcmp(arg_list[3], config->product_info.secret) ){
		 
		if (!(ATCmdParser_send("AT+ALINKPRODUCT=%s,%s,%s,%s,%s", 
							 config->product_info.name, config->product_info.module, 
	                         config->product_info.key,  config->product_info.secret, format_arg )
		  && ATCmdParser_recv("OK\r\n"))) {
			return kGeneralErr;
		}
	 }
						
	/* Check device info*/
	if (!(ATCmdParser_send("AT+ALINKDEV?")
	   && ATCmdParser_recv("+ALINKDEV:%40[^\r]\r\n",args)
	   && ATCmdParser_recv("OK\r\n"))) {
		return kReadErr;
	}
	
	if (3 != ATCmdParser_analyse_args(args, arg_list, 3)) {
		return kMalformedErr;
	}

	if ((strcmp(arg_list[0], config->dev_info.type)
	  || strcmp(arg_list[1], config->dev_info.category)
	  || strcmp(arg_list[2], config->dev_info.manufacture))){
		 
		if (!(ATCmdParser_send("AT+ALINKDEV=%s,%s,%s", 
							   config->dev_info.type, config->dev_info.category,
	                           config->dev_info.manufacture )
		  && ATCmdParser_recv("OK\r\n"))) {
			return kGeneralErr;
		}
	}
	  
	return kNoErr; 
}

mx_status emh_alisds_set_key (const char *dev_key, const char *dev_sec)
{
	if (ATCmdParser_send("AT+ALINKSDS=%s,%s", dev_key, dev_sec)
	 && ATCmdParser_recv("OK\r\n")) {
		return kNoErr;
	}
	return kGeneralErr;
}

mx_status emh_alisds_start_service(void)
{
	if (ATCmdParser_send("AT+ALINKSTART")
     && ATCmdParser_recv("OK\r\n")) {
        return kNoErr;
    }
	return kGeneralErr;
}

emh_arg_alisds_status_t emh_alisds_get_status(void)
{
	char arg[20];

	if (!(ATCmdParser_send("AT+ALINKSTATUS?")
	   && ATCmdParser_recv("+ALINKSTATUS:%20[^\r]\r\n",arg)
	   && ATCmdParser_recv("OK\r\n"))) {
		return EMH_ARG_ERR;
	}
	
	return emh_arg_for_arg( EMH_ARG_ALISDS_STATUS, arg);
}

mx_status emh_alisds_provision(bool on)
{
	if ((on? ATCmdParser_send("AT+ALINKAWSSTART"):
		     ATCmdParser_send("AT+ALINKAWSSTOP"))
		  && ATCmdParser_recv("OK\r\n")) {
		return kNoErr;
	}
	return kGeneralErr;
}

mx_status emh_ali_start_provision(void)
{
	if (ATCmdParser_send("AT+ALINKAWSSTART")
     && ATCmdParser_recv("OK\r\n")) {
        return kNoErr;
    }
	return kGeneralErr;
}

mx_status emh_alisds_unbound(void)
{
	if (ATCmdParser_send("AT+ALINKUNBIND")
     && ATCmdParser_recv("OK\r\n")) {
        return kNoErr;
    }
	return kGeneralErr;
}

mx_status emh_ali_stop_provision(void)
{
	if (ATCmdParser_send("AT+ALINKAWSSTOP")
     && ATCmdParser_recv("OK\r\n")) {
        return kNoErr;
    }
	return kGeneralErr;
}

mx_status emh_alisds_set_cloud_atts(emh_arg_alisds_format_t format, uint8_t *data, int32_t len)
{
	if (ATCmdParser_send("AT+ALINKSEND=%d", (int)len)
	 && ATCmdParser_recv(">")
	 && ATCmdParser_write((char *)data, len) == len
	 && ATCmdParser_recv("OK\r\n")) {
		return kNoErr;
	}
	return kGeneralErr;
}

void emh_alisds_event_handler(void)
{
	mx_status err = kNoErr;
	char arg1[10], arg2[10];
	emh_arg_alisds_format_t format;
	emh_arg_alisds_conn_t conn;
	emh_alisds_msg attrs;

	// parse out the packet
	require_action(ATCmdParser_recv("%10[^,],", arg1), exit, err = kMalformedErr);
		
	emh_arg_alisds_ev_t event = emh_arg_for_arg(EMH_ARG_ALISDS_EV, arg1);
	require_action(event != EMH_ARG_ERR, exit,  err = kMalformedErr);

	/* ALINK Server connection event */
	if (event == EMH_ARG_ALISDS_EV_ALINK) {
		require_action(ATCmdParser_recv("%10[^\r]\r\n", arg2), exit, err = kMalformedErr);
		conn = emh_arg_for_arg(EMH_ARG_ALISDS_CONN, arg2);
		require_action(conn != EMH_ARG_ERR, exit, err = kMalformedErr);
		emh_ev_alisds_connection(conn);
	}
	/* ALINK server <=== attribute value=== device */
	else if (event == EMH_ARG_ALISDS_EV_GET) {
		require_action(ATCmdParser_recv("%10[^\r]\r\n", arg2), exit, err = kMalformedErr);
		format =  emh_arg_for_arg(EMH_ARG_ALISDS_FORMAT, arg2);
		require_action(format != EMH_ARG_ERR, exit, err = kMalformedErr);
		
		attrs.format = format;
		attrs.data = NULL;
		attrs.len = 0;
		emh_ev_alisds_get_local_atts(&attrs);
	}
	/* ALINK server === attribute value===> device */
	else if (event == EMH_ARG_ALISDS_EV_SET) {
		require_action(ATCmdParser_recv("%10[^,],", arg2), exit, err = kMalformedErr);
		format = emh_arg_for_arg(EMH_ARG_ALISDS_FORMAT, arg2);
		require_action(format != EMH_ARG_ERR, exit, err = kMalformedErr);
		
		/* Read package data */
		int32_t count;
		require_action(ATCmdParser_recv("%d,", &count), exit, err = kMalformedErr);

		uint8_t *data = malloc(count);
		require_action(data, exit, err = kNoMemoryErr);
		require_action(ATCmdParser_read((char *)data, count) == count, exit, err = kTimeoutErr);

		attrs.data = data;
		attrs.format = format;
		attrs.len = count;
		emh_ev_alisds_set_local_atts(&attrs);
		free(data);
	}
	
exit:
	if (err == kMalformedErr) emh_ev_unknown();
	return;
}

