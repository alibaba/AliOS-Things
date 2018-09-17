/*
 * Copyright © 2018 alibaba. All rights reserved.
 */

#include <hal.h>
#include <stdio.h>
#include <string.h>
#include <be_osal.h>
#include <be_jse_module.h>
#include <be_osal.h>
#include <hal.h>
#include "be_jse_api.h"
#include "module_wifi.h"

#define JS_WIFI_TAG "WIFI"
#define TEST_TRACK_PRINT printf("%s,%d\n\r",__FUNCTION__,__LINE__)
#define JS_ARRRY_START "["
#define JS_ARRAY_END "]"
#define JS_ARRAY_SPLIT ","

typedef struct {
    void  *arg;
    int    id;
    be_jse_symbol_t* func;
} schedule_msg_t;

typedef enum {
    SCAN = 1,
    CONNECT,
    ERROR,
} schedule_msg_id_t;

static be_jse_symbol_t* js_scan_cb_func = NULL;
static be_jse_symbol_t* js_conc_cb_func = NULL;


static void wifi_event_callback(be_osal_input_event_t *event, void *data)
{
	be_jse_symbol_t* argv[1];
	char result[16]={0};

    if (event->type != EV_WIFI)
        return;

	be_debug(JS_WIFI_TAG,"wifi_event_cb type=%d\n\r",event->code);
    if (event->code && event->code < (CODE_WIFI_ON_GOT_IP + 1)) {
     	if (event->code == CODE_WIFI_ON_GOT_IP){
			strcpy(result,"CONNECTED");
		}else if(event->code == CODE_WIFI_ON_DISCONNECT){
			strcpy(result,"DISCONNECT");
		}

		if ((strlen(result) > 0) && (js_conc_cb_func != NULL)){
			argv[0] = new_str_symbol(result);
			be_jse_execute_func(bone_engine_get_executor(), js_conc_cb_func, 1, argv);
			DEC_SYMBL_REF(js_conc_cb_func);
			js_conc_cb_func = NULL;
			be_osal_unregister_event_filter(EV_WIFI, wifi_event_callback, NULL);
			be_debug(JS_WIFI_TAG,"WIFI.connect cb taskname=%s\n\r",be_osal_task_name());
		}
    }
}

static int wifi_scan_result_cb(be_wifi_scan_result_t **p_result)
{
	int buffer_size = (MAX_SCAN_NUM * WIFI_SCAN_SIZE) * sizeof(char);
	char *p_json_buff = NULL;
	char *p_json_string = NULL;
	int index = 0, ret = -1;
	int buffer_left = buffer_size;

	/*copy all wifi scan result to json buffer for js callback*/
	p_json_buff = (char *)malloc(buffer_size);
	if (p_json_buff == NULL){
		be_error(JS_WIFI_TAG,"%s:malloc fail 0\n\r",__FUNCTION__);
		goto done;
	}

	memset(p_json_buff, 0, buffer_size);
	p_json_string = p_json_buff;
	for (index = 0; index < MAX_SCAN_NUM; index++)
	{
		if (p_result[index] == NULL) {
			sprintf(p_json_string,"%s",JS_ARRAY_END);
			break;
		}

		if (index == 0){
			sprintf(p_json_string,"%s",JS_ARRRY_START);
			p_json_string = p_json_string + strlen(JS_ARRRY_START);
		}else{
			sprintf(p_json_string,"%s",JS_ARRAY_SPLIT);
			p_json_string = p_json_string + strlen(JS_ARRAY_SPLIT);
		}

		sprintf(p_json_string, "{\"ssid\":\"%s\",\"channel\":\"%d\",\"rssi\":\"%d\",\"auth\":\"%d\",\"encry\":\"%d\"}", \
				p_result[index]->ssid, p_result[index]->channel,\
				p_result[index]->rssi,p_result[index]->auth,p_result[index]->encry);

		p_json_string = p_json_string + strlen(p_json_string);
		buffer_left = buffer_left - strlen(p_json_string);
		if (buffer_left < WIFI_SCAN_SIZE){
			be_error(JS_WIFI_TAG,"%s:no enough buffer\n\r",__FUNCTION__);
			break;
		}
	}

	be_debug(JS_WIFI_TAG,"%s: scan[%d]=%s,%p\n\r",__FUNCTION__,index,p_json_buff,js_scan_cb_func);

	/*call js callback function*/
	if ((p_result != NULL) && (js_scan_cb_func != NULL) )
	{
		if ((p_json_buff != NULL) && strlen(p_json_buff) > 0){
			ret = 0;
			be_jse_symbol_t* argv[1];
			argv[0] = new_json_symbol(p_json_buff, strlen(p_json_buff));
			be_jse_execute_func(bone_engine_get_executor(), js_scan_cb_func, 1, argv);
			be_debug(JS_WIFI_TAG,"WIFI.scan cb taskname=%s\n\r",be_osal_task_name());
		}
	}

	done:
	if (ret == -1){
		/*if wifi scan fail,scan cb function return null object*/
		char p_json_buff[]={"[]"};
		be_jse_symbol_t* argv[1];
		argv[0] = new_json_symbol(p_json_buff, strlen(p_json_buff));
		be_jse_execute_func(bone_engine_get_executor(), js_scan_cb_func, 1, argv);
	}
   	DEC_SYMBL_REF(js_scan_cb_func);
	free(p_json_buff);
	return 0;
}

static void wifi_schedule_call(void* arg)
{
    schedule_msg_t* schedule_msg = (schedule_msg_t*)arg;
    int ret;

	if (schedule_msg == NULL) return;

	if (schedule_msg->id == SCAN) {
		ret = (int)be_osal_wifi_scan(wifi_scan_result_cb);
		if (ret != BE_WIFI_RET_OK) //scan fail and exception handle
		{
			char p_json_buff[]={"[]"};//if wifi scan fail,scan cb function return null object
			be_jse_symbol_t* argv[1];
			argv[0] = new_json_symbol(p_json_buff, strlen(p_json_buff));
			be_jse_execute_func(bone_engine_get_executor(), js_scan_cb_func, 1, argv);
			DEC_SYMBL_REF(js_scan_cb_func);
			free(schedule_msg);
			schedule_msg = NULL;
			return;
		}

	}else if (schedule_msg->id == CONNECT){
		be_osal_register_event_filter(EV_WIFI, wifi_event_callback, NULL);
   		be_osal_wifi_connect((be_osal_wifi_config_t *)(schedule_msg->arg));
		if (schedule_msg->arg){
			free(schedule_msg->arg); //free p_wifi_config calloc
		}
	}

	free(schedule_msg);
	schedule_msg = NULL;
    return;
}

static be_jse_symbol_t *module_wifi_init()
{
	int ret = -1;

	ret = be_osal_wifi_init();
	return new_int_symbol(ret);
}


/*****************************************************************************
*Function:    module_wifi_scan
*Description: js native addon for WIFI.scan(cb) to scan the wifi hotspot 
*Called by:   js api
*Input:       cb(result)：result is a json array like this 
			  {
			   		result[i].ssid, 
               		result[i].channel,
               		result[i].rssi, 
               		result[i].auth, 
              		result[i].encry
              }
*Output:      None
*****************************************************************************/
static be_jse_symbol_t *module_wifi_scan()
{
	int ret = -1;
	be_jse_symbol_t *callback;

	be_jse_handle_function(0, &callback, NULL, NULL, NULL);
    be_jse_symbol_t *func = get_symbol_value(callback);
	if (!symbol_is_function(func) || (func == NULL)){
		be_error(JS_WIFI_TAG,"%s: bad param\n\r",__FUNCTION__);
		goto done;
	}

	js_scan_cb_func = func;
    schedule_msg_t* schedule_msg = (schedule_msg_t*)calloc(1, sizeof(schedule_msg_t));
    schedule_msg->id = SCAN;
	INC_SYMBL_REF(js_scan_cb_func);
    ret = be_osal_schedule_call(wifi_schedule_call, schedule_msg);

	done:
	symbol_unlock(func);
	symbol_unlock(callback);

	return new_int_symbol(ret);
}


/*****************************************************************************
*Function:    module_wifi_connect
*Description: js native addon for WIFI.connect(ssid,pwd,cb)
*Called by:   js api
*Input:       ssid & pwd is STRING object, cb is a function which is the callback
              cb（State）{} :the State could be "CONNECTED or "DISCONNECT"
*Output:      None
*****************************************************************************/
static be_jse_symbol_t *module_wifi_connect()
{
	int ret = -1;
	be_jse_symbol_t *arg0 = NULL,*arg1 = NULL,*arg2 = NULL;
	be_jse_symbol_t *func = NULL;
	char *ssid=NULL;
	char *passwd=NULL;
	be_osal_wifi_config_t *p_wifi_config = NULL;

	be_jse_handle_function(0, &arg0, &arg1, &arg2, NULL);
	if (arg0 == NULL || arg1 == NULL || !symbol_is_string(arg0) || !symbol_is_string(arg1)) {
		be_error(JS_WIFI_TAG,"module_wifi_connect arg invalid\n\r");
		goto Fail;
	}

	func = get_symbol_value(arg2);
	if (!symbol_is_function(func) || (func == NULL)
		|| (symbol_str_len(arg0) > MAX_SSID_SIZE)
		|| (symbol_str_len(arg1) > MAX_PWD_SIZE) )
	{
		be_error(JS_WIFI_TAG,"module_wifi_connect arg lend exceed\n\r");
		goto Fail;
	}


	p_wifi_config = calloc(1,sizeof(be_osal_wifi_config_t));
	if (p_wifi_config == NULL) goto Fail;

    symbol_to_str(arg0, p_wifi_config->ssid, symbol_str_len(arg0));
    symbol_to_str(arg1, p_wifi_config->pwd, symbol_str_len(arg1));

	js_conc_cb_func = func;
    schedule_msg_t* schedule_msg = (schedule_msg_t*)calloc(1, sizeof(schedule_msg_t));
    schedule_msg->id = CONNECT;
	schedule_msg->arg = p_wifi_config;
	INC_SYMBL_REF(js_conc_cb_func);
    ret = be_osal_schedule_call(wifi_schedule_call, schedule_msg);
	return new_int_symbol(ret);

	Fail:
	symbol_unlock(arg0);
	symbol_unlock(arg1);
	symbol_unlock(arg2);
	return new_int_symbol(ret);
}


/*****************************************************************************
*Function:    module_wifi_getip
*Description: js native addon for WIFI.getip()
*Called by:   js api
*Input:       no input			  
*Output:      return a string object to js api,if it is NULL means can't get ip 
*****************************************************************************/
static be_jse_symbol_t *module_wifi_getip()
{
	char ip[32] = {0};

	be_jse_handle_function(0, 0, 0, 0, 0);
    be_osal_wifi_get_ip(ip);
	if( strcmp(ip,"0.0.0.0")==0 ){
        return new_symbol(BE_SYM_NULL);
	}else{
		return new_str_symbol(ip);
	}
}

/*****************************************************************************
*Function:    module_wifi_getmac
*Description: js native addon for WIFI.getmac()
*Called by:   js api
*Input:       no input			  
*Output:      return a string object to js api what the mac is 
*****************************************************************************/
static be_jse_symbol_t *module_wifi_getmac()
{
	hal_wifi_module_t *module = NULL;
	char mac[6]={0};
	char mac_str[32]={0};

	be_jse_handle_function(0, 0, 0, 0, 0);
	module = hal_wifi_get_default_module();
	if (module == NULL) return new_str_symbol(mac);

	hal_wifi_get_mac_addr(module, mac);
	snprintf(mac_str,sizeof(mac_str),"%02X:%02X:%02X:%02X:%02X:%02X",\
			 mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	return new_str_symbol(mac);
}

/*****************************************************************************
*Function:    module_wifi_getchannel
*Description: js native addon for WIFI.getchannel()
*Called by:   js api
*Input:       no input			  
*Output:      return to js api which the channel is
*****************************************************************************/
static be_jse_symbol_t *module_wifi_getchannel()
{
	hal_wifi_module_t *module = NULL;
	int channel = 0;

	be_jse_handle_function(0, 0, 0, 0, 0);
	module = hal_wifi_get_default_module();
	if (module == NULL) return new_int_symbol(-1);

	channel = hal_wifi_get_channel(module);
	return new_int_symbol(channel);
}

/*****************************************************************************
*Function:    module_wifi_poweroff
*Description: js native addon for WIFI.poweroff()
*Called by:   js api
*Input:       no input			  
*Output:      0 poweroff wifi ok ,other poweroff wifi fail
*****************************************************************************/
static be_jse_symbol_t *module_wifi_poweroff()
{
	hal_wifi_module_t *module = NULL;
	int ret = -1;

	be_jse_handle_function(0, 0, 0, 0, 0);
	module = hal_wifi_get_default_module();
	if (module == NULL) return new_int_symbol(-1);

	ret = hal_wifi_power_off(module);
	return new_int_symbol(ret);
}

/*****************************************************************************
*Function:    module_wifi_poweron
*Description: js native addon for WIFI.poweron()
*Called by:   js api
*Input:       no input			  
*Output:      0 poweron wifi ok ,other poweron wifi fail
*****************************************************************************/
static be_jse_symbol_t *module_wifi_poweron()
{
	hal_wifi_module_t *module = NULL;
	int ret = -1;

	be_jse_handle_function(0, 0, 0, 0, 0);
	module = hal_wifi_get_default_module();
	if (module == NULL) return new_int_symbol(-1);

	ret = hal_wifi_power_on(module);
	return new_int_symbol(ret);
}


/*****************************************************************************
*Function:    module_wifi_setchannel
*Description: js native addon for WIFI.setchannel(channel)
*Called by:   js api
*Input:       JS API WIFI.setchannel(channel) need a channel input			  
*Output:      0 setchannel ok ,other setchannel fail
*****************************************************************************/
static be_jse_symbol_t *module_wifi_setchannel()
{
	hal_wifi_module_t *module = NULL;
	be_jse_symbol_t *arg0;
	int ret = 0, channel = 0;

	arg0 = be_jse_handle_single_arg_function();
	if(arg0 == NULL) {
	    symbol_unlock(arg0);
	    be_error(JS_WIFI_TAG,"%s:params error\n\r",__FUNCTION__);
	    return new_int_symbol(-1);
	}

	channel = get_symbol_int(arg0);
	module = hal_wifi_get_default_module();
	if (module == NULL) return new_int_symbol(-1);

	ret = hal_wifi_set_channel(module,channel);
	return new_int_symbol(ret);
}


be_jse_symbol_t* module_handle_cb(be_jse_vm_ctx_t* execInfo,
											  be_jse_symbol_t* var,
											  const char* name)
{
	be_debug(JS_WIFI_TAG,"%s,name=%s\n\r",__FUNCTION__,name);

	if (strcmp(name,"scan") == 0) 		return module_wifi_scan();
	if (strcmp(name,"connect") == 0) 	return module_wifi_connect();
	if (strcmp(name,"getip") == 0) 		return module_wifi_getip();
	if (strcmp(name,"getmac") == 0) 	return module_wifi_getmac();
	if (strcmp(name,"getchannel") == 0) return module_wifi_getchannel();
	if (strcmp(name,"setchannel") == 0) return module_wifi_setchannel();
	if (strcmp(name,"poweron") == 0) 	return module_wifi_poweron();
	if (strcmp(name,"poweroff") == 0) 	return module_wifi_poweroff();

	return BE_JSE_FUNC_UNHANDLED;
}


/*Use JS WIFI object,must call this function first*/
void module_wifi_load(void)
{
	int ret = -1;

	//ret = be_osal_wifi_init();
    be_jse_module_load(JS_WIFI_TAG, module_handle_cb);
}

/*the api for when bone_engine restart then reload the wifi native module */
void module_wifi_reload(void)
{
	be_jse_module_load(JS_WIFI_TAG, module_handle_cb);
}
