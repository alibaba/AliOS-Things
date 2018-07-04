#include "lite-utils.h"
#include "lite-cjson.h"
#include "iotx_dm_common.h"
#include "iotx_dm_manager.h"
#include "iotx_dm_shadow.h"
#include "iotx_dm_ipc.h"
#include "iotx_dm_msg_dispatch.h"
#include "iotx_dm_message.h"
#include "iotx_dm_cm_wrapper.h"

#define LINKKIT_KEY_ID        "id"
#define LINKKIT_KEY_CODE      "code"
#define LINKKIT_KEY_DEVID     "devid"
#define LINKKIT_KEY_SERVICEID "serviceid"
#define LINKKIT_KEY_PAYLOAD   "payload"

#if 1
const char LINKKIT_TSL_STRING_TEST[] = "{\"schema\":\"https://iot-tsl.oss-cn-shanghai.aliyuncs.com/schema.json\",\"profile\":{\"productKey\":\"b1ZFYKi3UG3\"},\"services\":[{\"outputData\":[],\"identifier\":\"set\",\"inputData\":[{\"identifier\":\"GameListString\",\"dataType\":{\"specs\":{\"item\":{\"type\":\"text\"},\"size\":\"128\"},\"type\":\"array\"},\"name\":\"GameListStringName\"},{\"identifier\":\"GameListInt\",\"dataType\":{\"specs\":{\"item\":{\"type\":\"int\"},\"size\":\"128\"},\"type\":\"array\"},\"name\":\"GameListIntName\"},{\"identifier\":\"GameListFloat\",\"dataType\":{\"specs\":{\"item\":{\"type\":\"float\"},\"size\":\"128\"},\"type\":\"array\"},\"name\":\"GameListFloatName\"},{\"identifier\":\"GameListDouble\",\"dataType\":{\"specs\":{\"item\":{\"type\":\"double\"},\"size\":\"128\"},\"type\":\"array\"},\"name\":\"GameListDoubleName\"},{\"identifier\":\"WIFI_Band\",\"dataType\":{\"specs\":{\"length\":\"1024\"},\"type\":\"text\"},\"name\":\"WIFI_Band_Name\"},{\"identifier\":\"EnumTypeItem\",\"dataType\":{\"specs\":{\"0\":\"EnumTypeItem1\",\"1\":\"EnumTypeItem2\",\"2\":\"EnumTypeItem3\",\"3\":\"EnumTypeItem4\"},\"type\":\"enum\"},\"name\":\"EnumTypeItemName\"},{\"identifier\":\"PowerSwitch\",\"dataType\":{\"specs\":{\"0\":\"off\",\"1\":\"on\"},\"type\":\"bool\"},\"name\":\"PowerSwitchName\"},{\"identifier\":\"HeatSwitch\",\"dataType\":{\"specs\":{\"0\":\"off\",\"1\":\"on\"},\"type\":\"bool\"},\"name\":\"HeatSwitchName\"},{\"identifier\":\"TempPwmDuty\",\"dataType\":{\"specs\":{\"min\":\"0\",\"max\":\"100\"},\"type\":\"int\"},\"name\":\"TempPwmDutyName\"},{\"identifier\":\"liquidRemain\",\"dataType\":{\"specs\":{\"min\":\"0\",\"max\":\"100\"},\"type\":\"double\"},\"name\":\"liquidRemainName\"},{\"identifier\":\"WaterReplace\",\"dataType\":{\"specs\":{\"0\":\"off\",\"1\":\"on\"},\"type\":\"bool\"},\"name\":\"WaterReplaceName\"},{\"identifier\":\"NickName\",\"dataType\":{\"specs\":{\"length\":\"128\"},\"type\":\"text\"},\"name\":\"NickNameName\"},{\"identifier\":\"WorkPlan" 
"s\",\"dataType\":{\"specs\":[{\"identifier\":\"PlantSwitch\",\"dataType\":{\"specs\":{\"0\":\"off\",\"1\":\"on\"},\"type\":\"bool\"},\"name\":\"PlantSwitchName\"},{\"identifier\":\"SelectIndex\",\"dataType\":{\"specs\":{\"unit\":\"0\",\"min\":\"0\",\"max\":\"3\"},\"type\":\"int\"},\"name\":\"SelectIndexName\"},{\"identifier\":\"PlantInfo1\",\"dataType\":{\"specs\":{\"length\":\"1024\"},\"type\":\"text\"},\"name\":\"PlantInfo1Name\"},{\"identifier\":\"PlantInfo2\",\"dataType\":{\"specs\":{\"length\":\"1024\"},\"type\":\"text\"},\"name\":\"PlantInfo2Name\"},{\"identifier\":\"PlantInfo3\",\"dataType\":{\"specs\":{\"length\":\"1024\"},\"type\":\"text\"},\"name\":\"PlantInfo3Name\"}],\"type\":\"struct\"},\"name\":\"WorkPlansName\"}],\"method\":\"thing.service.property.set\",\"name\":\"set\",\"required\":true,\"callType\":\"sync\",\"desc\":\"属性设置\"},{\"outputData\":[{\"identifier\":\"GameListString\",\"dataType\":{\"specs\":{\"item\":{\"type\":\"text\"},\"size\":\"128\"},\"type\":\"array\"},\"name\":\"GameListStringName\"},{\"identifier\":\"GameListInt\",\"dataType\":{\"specs\":{\"item\":{\"type\":\"int\"},\"size\":\"128\"},\"type\":\"array\"},\"name\":\"GameListIntName\"},{\"identifier\":\"GameListFloat\",\"dataType\":{\"specs\":{\"item\":{\"type\":\"float\"},\"size\":\"128\"},\"type\":\"array\"},\"name\":\"GameListFloatName\"},{\"identifier\":\"GameListDouble\",\"dataType\":{\"specs\":{\"item\":{\"type\":\"double\"},\"size\":\"128\"},\"type\":\"array\"},\"name\":\"GameListDoubleName\"},{\"identifier\":\"WIFI_Band\",\"dataType\":{\"specs\":{\"length\":\"1024\"},\"type\":\"text\"},\"name\":\"WIFI_Band_Name\"},{\"identifier\":\"EnumTypeItem\",\"dataType\":{\"specs\":{\"0\":\"EnumTypeItem1\",\"1\":\"EnumTypeItem2\",\"2\":\"EnumTypeItem3\",\"3\":\"EnumTypeItem4\"},\"type\":\"enum\"},\"name\":\"EnumTypeItemName\"},{\"identifier\":\"PowerSwitch\",\"dataType\":{\"specs\":{\"0\":\"off\",\"1\":\"on\"},\"type\":\"bool\"},\"name\":\"PowerSwitchName\"},{\"identifier\":\"HeatSwitch\",\"dataTyp" 
"e\":{\"specs\":{\"0\":\"off\",\"1\":\"on\"},\"type\":\"bool\"},\"name\":\"HeatSwitchName\"},{\"identifier\":\"TempPwmDuty\",\"dataType\":{\"specs\":{\"min\":\"0\",\"max\":\"100\"},\"type\":\"int\"},\"name\":\"TempPwmDutyName\"},{\"identifier\":\"liquidRemain\",\"dataType\":{\"specs\":{\"min\":\"0\",\"max\":\"100\"},\"type\":\"double\"},\"name\":\"liquidRemainName\"},{\"identifier\":\"WaterReplace\",\"dataType\":{\"specs\":{\"0\":\"off\",\"1\":\"on\"},\"type\":\"bool\"},\"name\":\"WaterReplaceName\"},{\"identifier\":\"NickName\",\"dataType\":{\"specs\":{\"length\":\"128\"},\"type\":\"text\"},\"name\":\"NickNameName\"},{\"identifier\":\"WorkPlans\",\"dataType\":{\"specs\":[{\"identifier\":\"PlantSwitch\",\"dataType\":{\"specs\":{\"0\":\"off\",\"1\":\"on\"},\"type\":\"bool\"},\"name\":\"PlantSwitchName\"},{\"identifier\":\"SelectIndex\",\"dataType\":{\"specs\":{\"unit\":\"0\",\"min\":\"0\",\"max\":\"3\"},\"type\":\"int\"},\"name\":\"SelectIndexName\"},{\"identifier\":\"PlantInfo1\",\"dataType\":{\"specs\":{\"length\":\"1024\"},\"type\":\"text\"},\"name\":\"PlantInfo1Name\"},{\"identifier\":\"PlantInfo2\",\"dataType\":{\"specs\":{\"length\":\"1024\"},\"type\":\"text\"},\"name\":\"PlantInfo2Name\"},{\"identifier\":\"PlantInfo3\",\"dataType\":{\"specs\":{\"length\":\"1024\"},\"type\":\"text\"},\"name\":\"PlantInfo3Name\"}],\"type\":\"struct\"},\"name\":\"WorkPlansName\"}],\"identifier\":\"get\",\"inputData\":[\"GameListString\",\"GameListInt\",\"GameListFloat\",\"GameListDouble\",\"WIFI_Band\",\"EnumTypeItem\",\"PowerSwitch\",\"HeatSwitch\",\"TempPwmDuty\",\"liquidRemain\",\"WaterReplace\",\"NickName\",\"WorkPlans\"],\"method\":\"thing.service.property.get\",\"name\":\"get\",\"required\":true,\"callType\":\"sync\",\"desc\":\"属性获取\"}],\"properties\":[{\"identifier\":\"GameListString\",\"dataType\":{\"specs\":{\"item\":{\"type\":\"text\"},\"size\":\"128\"},\"type\":\"array\"},\"name\":\"GameListStringName\",\"accessMode\":\"rw\",\"required\":true},{\"identifier\":\"GameListInt\",\"da" 
"taType\":{\"specs\":{\"item\":{\"type\":\"int\"},\"size\":\"128\"},\"type\":\"array\"},\"name\":\"GameListIntName\",\"accessMode\":\"rw\",\"required\":true},{\"identifier\":\"GameListFloat\",\"dataType\":{\"specs\":{\"item\":{\"type\":\"float\"},\"size\":\"128\"},\"type\":\"array\"},\"name\":\"GameListFloatName\",\"accessMode\":\"rw\",\"required\":true},{\"identifier\":\"GameListDouble\",\"dataType\":{\"specs\":{\"item\":{\"type\":\"double\"},\"size\":\"128\"},\"type\":\"array\"},\"name\":\"GameListDoubleName\",\"accessMode\":\"rw\",\"required\":true},{\"identifier\":\"WIFI_Band\",\"dataType\":{\"specs\":{\"length\":\"1024\"},\"type\":\"text\"},\"name\":\"WIFI_Band_Name\",\"accessMode\":\"rw\",\"required\":false},{\"identifier\":\"EnumTypeItem\",\"dataType\":{\"specs\":{\"0\":\"EnumTypeItem1\",\"1\":\"EnumTypeItem2\",\"2\":\"EnumTypeItem3\",\"3\":\"EnumTypeItem4\"},\"type\":\"enum\"},\"name\":\"EnumTypeItemName\",\"accessMode\":\"rw\",\"required\":false},{\"identifier\":\"PowerSwitch\",\"dataType\":{\"specs\":{\"0\":\"off\",\"1\":\"on\"},\"type\":\"bool\"},\"name\":\"PowerSwitchName\",\"accessMode\":\"rw\",\"required\":false},{\"identifier\":\"HeatSwitch\",\"dataType\":{\"specs\":{\"0\":\"off\",\"1\":\"on\"},\"type\":\"bool\"},\"name\":\"HeatSwitchName\",\"accessMode\":\"rw\",\"required\":false},{\"identifier\":\"TempPwmDuty\",\"dataType\":{\"specs\":{\"min\":\"0\",\"max\":\"100\"},\"type\":\"int\"},\"name\":\"TempPwmDutyName\",\"accessMode\":\"rw\",\"required\":false},{\"identifier\":\"liquidRemain\",\"dataType\":{\"specs\":{\"min\":\"0\",\"max\":\"100\"},\"type\":\"double\"},\"name\":\"liquidRemainName\",\"accessMode\":\"rw\",\"required\":false},{\"identifier\":\"WaterReplace\",\"dataType\":{\"specs\":{\"0\":\"off\",\"1\":\"on\"},\"type\":\"bool\"},\"name\":\"WaterReplaceName\",\"accessMode\":\"rw\",\"required\":false},{\"identifier\":\"NickName\",\"dataType\":{\"specs\":{\"length\":\"128\"},\"type\":\"text\"},\"name\":\"NickNameName\",\"accessMode\":\"rw\",\"required\":false}," 
"{\"identifier\":\"WorkPlans\",\"dataType\":{\"specs\":[{\"identifier\":\"PlantSwitch\",\"dataType\":{\"specs\":{\"0\":\"off\",\"1\":\"on\"},\"type\":\"bool\"},\"name\":\"PlantSwitchName\"},{\"identifier\":\"SelectIndex\",\"dataType\":{\"specs\":{\"unit\":\"0\",\"min\":\"0\",\"max\":\"3\"},\"type\":\"int\"},\"name\":\"SelectIndexName\"},{\"identifier\":\"PlantInfo1\",\"dataType\":{\"specs\":{\"length\":\"1024\"},\"type\":\"text\"},\"name\":\"PlantInfo1Name\"},{\"identifier\":\"PlantInfo2\",\"dataType\":{\"specs\":{\"length\":\"1024\"},\"type\":\"text\"},\"name\":\"PlantInfo2Name\"},{\"identifier\":\"PlantInfo3\",\"dataType\":{\"specs\":{\"length\":\"1024\"},\"type\":\"text\"},\"name\":\"PlantInfo3Name\"}],\"type\":\"struct\"},\"name\":\"WorkPlansName\",\"accessMode\":\"rw\",\"required\":false}],\"events\":[{\"outputData\":[{\"identifier\":\"GameListString\",\"dataType\":{\"specs\":{\"item\":{\"type\":\"text\"},\"size\":\"128\"},\"type\":\"array\"},\"name\":\"GameListStringName\"},{\"identifier\":\"GameListInt\",\"dataType\":{\"specs\":{\"item\":{\"type\":\"int\"},\"size\":\"128\"},\"type\":\"array\"},\"name\":\"GameListIntName\"},{\"identifier\":\"GameListFloat\",\"dataType\":{\"specs\":{\"item\":{\"type\":\"float\"},\"size\":\"128\"},\"type\":\"array\"},\"name\":\"GameListFloatName\"},{\"identifier\":\"GameListDouble\",\"dataType\":{\"specs\":{\"item\":{\"type\":\"double\"},\"size\":\"128\"},\"type\":\"array\"},\"name\":\"GameListDoubleName\"},{\"identifier\":\"WIFI_Band\",\"dataType\":{\"specs\":{\"length\":\"1024\"},\"type\":\"text\"},\"name\":\"WIFI_Band_Name\"},{\"identifier\":\"EnumTypeItem\",\"dataType\":{\"specs\":{\"0\":\"EnumTypeItem1\",\"1\":\"EnumTypeItem2\",\"2\":\"EnumTypeItem3\",\"3\":\"EnumTypeItem4\"},\"type\":\"enum\"},\"name\":\"EnumTypeItemName\"},{\"identifier\":\"PowerSwitch\",\"dataType\":{\"specs\":{\"0\":\"off\",\"1\":\"on\"},\"type\":\"bool\"},\"name\":\"PowerSwitchName\"},{\"identifier\":\"HeatSwitch\",\"dataType\":{\"specs\":{\"0\":\"off\",\"1\":\"on\"}" 
",\"type\":\"bool\"},\"name\":\"HeatSwitchName\"},{\"identifier\":\"TempPwmDuty\",\"dataType\":{\"specs\":{\"min\":\"0\",\"max\":\"100\"},\"type\":\"int\"},\"name\":\"TempPwmDutyName\"},{\"identifier\":\"liquidRemain\",\"dataType\":{\"specs\":{\"min\":\"0\",\"max\":\"100\"},\"type\":\"double\"},\"name\":\"liquidRemainName\"},{\"identifier\":\"WaterReplace\",\"dataType\":{\"specs\":{\"0\":\"off\",\"1\":\"on\"},\"type\":\"bool\"},\"name\":\"WaterReplaceName\"},{\"identifier\":\"NickName\",\"dataType\":{\"specs\":{\"length\":\"128\"},\"type\":\"text\"},\"name\":\"NickNameName\"},{\"identifier\":\"WorkPlans\",\"dataType\":{\"specs\":[{\"identifier\":\"PlantSwitch\",\"dataType\":{\"specs\":{\"0\":\"off\",\"1\":\"on\"},\"type\":\"bool\"},\"name\":\"PlantSwitchName\"},{\"identifier\":\"SelectIndex\",\"dataType\":{\"specs\":{\"unit\":\"0\",\"min\":\"0\",\"max\":\"3\"},\"type\":\"int\"},\"name\":\"SelectIndexName\"},{\"identifier\":\"PlantInfo1\",\"dataType\":{\"specs\":{\"length\":\"1024\"},\"type\":\"text\"},\"name\":\"PlantInfo1Name\"},{\"identifier\":\"PlantInfo2\",\"dataType\":{\"specs\":{\"length\":\"1024\"},\"type\":\"text\"},\"name\":\"PlantInfo2Name\"},{\"identifier\":\"PlantInfo3\",\"dataType\":{\"specs\":{\"length\":\"1024\"},\"type\":\"text\"},\"name\":\"PlantInfo3Name\"}],\"type\":\"struct\"},\"name\":\"WorkPlansName\"}],\"identifier\":\"post\",\"method\":\"thing.event.property.post\",\"name\":\"post\",\"type\":\"info\",\"required\":true,\"desc\":\"属性上报\"},{\"outputData\":[{\"identifier\":\"ErrorCode\",\"dataType\":{\"specs\":{\"min\":\"0\",\"max\":\"100\"},\"type\":\"int\"},\"name\":\"ErrorCodeName\"}],\"identifier\":\"Error\",\"method\":\"thing.event.Error.post\",\"name\":\"ErrorName\",\"type\":\"error\",\"required\":false}]}";
#endif
#if 1
static void _linkkit_event_thing_service_request(char *payload)
{
	int res = 0;
	lite_cjson_t lite, lite_item_id, lite_item_devid, lite_item_serviceid, lite_item_payload;

	/* Parse JSON */
	memset(&lite,0,sizeof(lite_cjson_t));
	res = lite_cjson_parse(payload,strlen(payload),&lite);
	if (res != SUCCESS_RETURN || !lite_cjson_is_object(&lite)) {return;}

	/* Parse Message ID */
	memset(&lite_item_id,0,sizeof(lite_cjson_t));
	res = lite_cjson_object_item(&lite,LINKKIT_KEY_ID,strlen(LINKKIT_KEY_ID),&lite_item_id);
	if (res != SUCCESS_RETURN || !lite_cjson_is_number(&lite_item_id)) {return;}
	log_info("Current Msg ID: %d",lite_item_id.value_int);

	/* Parse Devid */
	memset(&lite_item_devid,0,sizeof(lite_cjson_t));
	res = lite_cjson_object_item(&lite,LINKKIT_KEY_DEVID,strlen(LINKKIT_KEY_DEVID),&lite_item_devid);
	if (res != SUCCESS_RETURN || !lite_cjson_is_number(&lite_item_devid)) {return;}
	log_info("Current Devid: %d",lite_item_devid.value_int);

	/* Parse Serviceid */
	memset(&lite_item_serviceid,0,sizeof(lite_cjson_t));
	res = lite_cjson_object_item(&lite,LINKKIT_KEY_SERVICEID,strlen(LINKKIT_KEY_SERVICEID),&lite_item_serviceid);
	if (res != SUCCESS_RETURN || !lite_cjson_is_string(&lite_item_serviceid)) {return;}
	log_info("Current Serviceid: %.*s",lite_item_serviceid.value_length,lite_item_serviceid.value);

	/* Parse Serviceid */
	memset(&lite_item_payload,0,sizeof(lite_cjson_t));
	res = lite_cjson_object_item(&lite,LINKKIT_KEY_PAYLOAD,strlen(LINKKIT_KEY_PAYLOAD),&lite_item_payload);
	if (res != SUCCESS_RETURN) {return;}
	log_info("Current Payload: %.*s",lite_item_payload.value_length,lite_item_payload.value);

	IOT_DM_Send_Service_Response(lite_item_devid.value_int,lite_item_id.value_int,IOTX_DM_ERR_CODE_SUCCESS,lite_item_serviceid.value,lite_item_serviceid.value_length);
}

static void _linkkit_event_subdev_register_reply(char *payload)
{
	int res = 0;
	lite_cjson_t lite, lite_item_id, lite_item_code, lite_item_devid;
	
	if (payload == NULL) {return;}

	/* Parse JSON */
	res = lite_cjson_parse(payload,strlen(payload),&lite);
	if (res != SUCCESS_RETURN || !lite_cjson_is_object(&lite)) {return;}

	/* Parse Message ID */
	res = lite_cjson_object_item(&lite,LINKKIT_KEY_ID,strlen(LINKKIT_KEY_ID),&lite_item_id);
	if (res != SUCCESS_RETURN || !lite_cjson_is_number(&lite_item_id)) {return;}
	log_info("Current Msg ID: %d",lite_item_id.value_int);

	/* Parse Message Code */
	res = lite_cjson_object_item(&lite,LINKKIT_KEY_CODE,strlen(LINKKIT_KEY_CODE),&lite_item_code);
	if (res != SUCCESS_RETURN || !lite_cjson_is_number(&lite_item_code)) {return;}
	log_info("Current Code: %d",lite_item_code.value_int);

	/* Parse Devid */
	res = lite_cjson_object_item(&lite,LINKKIT_KEY_DEVID,strlen(LINKKIT_KEY_DEVID),&lite_item_devid);
	if (res != SUCCESS_RETURN || !lite_cjson_is_number(&lite_item_devid)) {return;}
	log_info("Current devid: %d",lite_item_devid.value_int);

	IOT_DM_Subdev_Topo_Add(lite_item_devid.value_int);
}

static void _linkkit_event_topo_add_reply(char *payload)
{
	int res = 0;
	lite_cjson_t lite, lite_item_id, lite_item_code, lite_item_devid;
	
	if (payload == NULL) {return;}

	/* Parse JSON */
	res = lite_cjson_parse(payload,strlen(payload),&lite);
	if (res != SUCCESS_RETURN || !lite_cjson_is_object(&lite)) {return;}

	/* Parse Message ID */
	res = lite_cjson_object_item(&lite,LINKKIT_KEY_ID,strlen(LINKKIT_KEY_ID),&lite_item_id);
	if (res != SUCCESS_RETURN || !lite_cjson_is_number(&lite_item_id)) {return;}
	log_info("Current Msg ID: %d",lite_item_id.value_int);

	/* Parse Message Code */
	res = lite_cjson_object_item(&lite,LINKKIT_KEY_CODE,strlen(LINKKIT_KEY_CODE),&lite_item_code);
	if (res != SUCCESS_RETURN || !lite_cjson_is_number(&lite_item_code)) {return;}
	log_info("Current Code: %d",lite_item_code.value_int);

	/* Parse Devid */
	res = lite_cjson_object_item(&lite,LINKKIT_KEY_DEVID,strlen(LINKKIT_KEY_DEVID),&lite_item_devid);
	if (res != SUCCESS_RETURN || !lite_cjson_is_number(&lite_item_devid)) {return;}
	log_info("Current devid: %d",lite_item_devid.value_int);

	IOT_DM_Subdev_Login(lite_item_devid.value_int);
}

void linkkit_event_callback(iotx_dm_event_types_t type, char *payload)
{
	log_info("Receive Message Type: %d",type);
	if (payload) {log_info("Receive Message: %s",payload);}

	switch(type) {
		case IOTX_DM_EVENT_THING_SERVICE_REQUEST:
			{
				_linkkit_event_thing_service_request(payload);
			}
			break;
		case IOTX_DM_EVENT_SUBDEV_REGISTER_REPLY:
			{
				_linkkit_event_subdev_register_reply(payload);
			}
			break;
		case IOTX_DM_EVENT_SUBDEV_UNREGISTER_REPLY:
			{
			
			}
			break;
		case IOTX_DM_EVENT_TOPO_ADD_REPLY:
			{
				_linkkit_event_topo_add_reply(payload);
			}
			break;
		case IOTX_DM_EVENT_COMBINE_LOGIN_REPLY:
			{

			}
			break;
		default:
			log_info("Not Found Type For Now, Smile");
			break;
	}
}


int main(int argc, char *argv[])
{
	int res = 0, devid = 0, devid1 = 0;
	static int time = 0;
	
	LITE_openlog("linkkit");
	LITE_set_loglevel(LOG_DEBUG_LEVEL);

	HAL_SetProductKey("a1BUNdoKJs7");
	HAL_SetDeviceName("gw-type-001");
	HAL_SetDeviceSecret("ayxXAAJYaJl4g1bI726LLm7qw4y8FY5P");

	/* DM Construct */
	iotx_dm_init_params_t dm_init_params;
	memset(&dm_init_params,0,sizeof(iotx_dm_init_params_t));
	dm_init_params.secret_type = IOTX_DM_DEVICE_SECRET_DEVICE;
	dm_init_params.domain_type = IOTX_DM_CLOUD_DOMAIN_SHANGHAI;
	dm_init_params.event_callback = linkkit_event_callback;
	
	res = IOT_DM_Construct(&dm_init_params);
	if (res != SUCCESS_RETURN) {
		log_err("IOT_DM_Construct Failed");
		return FAIL_RETURN;
	}

	IOT_DM_Set_TSL(IOTX_DMGR_LOCAL_NODE_DEVID, IOTX_DM_TSL_SOURCE_CLOUD, LINKKIT_TSL_STRING_TEST,strlen(LINKKIT_TSL_STRING_TEST));
	//IOT_DM_Set_TSL(IOTX_DMGR_LOCAL_NODE_DEVID, IOTX_DM_TSL_SOURCE_LOCAL, LINKKIT_TSL_STRING_TEST,strlen(LINKKIT_TSL_STRING_TEST));

	//iotx_dcs_topic_local_generic_register();
	char *testpk = "b1ZFYKi3UG3";
	char *testdn = "type-001";
	char *testdn1 = "type-unregister-001";
	//char *testpayload = "this is test";
	IOT_DM_Subdev_Create(testpk,testdn,&devid);
	IOT_DM_Subdev_Create(testpk,testdn1,&devid1);
	while(1) {
		//HAL_SleepMs(1000);
		//if (time > 10000) break;
		//if(time == 5000) {IOT_DM_Subdev_Topo_Del(devid);}
		//if (time == 5000) {iotx_dcs_thing_topo_get();}
		//if (time == 5000) {iotx_dcs_thing_list_found(testpk,testdn);}
		if (time == 5000) {IOT_DM_Subdev_Register(devid);}
		//if (time == 3000) {IOT_DM_Subdev_Topo_Add(devid1);}
		//if (time == 8000) {IOT_DM_Subdev_Topo_Del(devid1);}
		//if (time == 7000) {IOT_DM_Subdev_Login(devid);}
		//if (time == 8000) {iotx_dcs_thing_model_up_raw(testpk,testdn,testpayload,strlen(testpayload));}
		//if (time == 8000) {IOT_DM_Subdev_Unregister(devid);}
		#if 0
		if (time == 10000) {
			int res = 0;
			void *handle = NULL;
			res = IOT_DM_Post_Property_Start(devid,&handle);
			if (res != SUCCESS_RETURN) {
				dm_log_debug("IOT_DM_Post_Property_Start Error");
			}

			#if 1
			res = IOT_DM_Post_Property_Add(handle,"WorkPlans");
			if (res != SUCCESS_RETURN) {
				dm_log_debug("IOT_DM_Post_Property_Add Error");
			}
			#endif
			#if 0
			res = IOT_DM_Post_Property_Add(handle,"WaterReplace");
			if (res != SUCCESS_RETURN) {
				dm_log_debug("IOT_DM_Post_Property_Add Error");
			}
			
			res = IOT_DM_Post_Property_Add(handle,IOTX_DM_POST_PROPERTY_ALL);
			if (res != SUCCESS_RETURN) {
				dm_log_debug("IOT_DM_Post_Property_Add Error");
			}
			#endif
			res = IOT_DM_Post_Property_End(&handle);
			
			if (res != SUCCESS_RETURN) {
				dm_log_debug("IOT_DM_Post_Property_End Error");
			}
		}
		#endif
		//if (time == 8000) {break;}
		#if 0
		if (time == 8000) {
			IOT_DM_Post_Event(0,"Error");
		}
		#endif
		IOT_DM_Yield(100);
		time+=100;
		IOT_DM_Dispatch();
		//log_info("While Loop");
	}
	
	IOT_DM_Destroy();
	log_info("sizeof(int): %d",sizeof(int));
	log_info("sizeof(float): %d",sizeof(float));
	log_info("sizeof(double): %d",sizeof(double));
	
	LITE_dump_malloc_free_stats(LOG_DEBUG_LEVEL);
}
#endif

#if 0
int main(int argc, char *argv[])
{
	//void *shadow = NULL;
	////static int time = 0;
	
	LITE_openlog("linkkit");
	LITE_set_loglevel(LOG_DEBUG_LEVEL);

	HAL_SetProductKey("a1BUNdoKJs7");
	HAL_SetDeviceName("gw-type-001");
	HAL_SetDeviceSecret("ayxXAAJYaJl4g1bI726LLm7qw4y8FY5P");

	iotx_dsw_t *dtsl = NULL;
	
	iotx_dsw_create(IOTX_DM_TSL_TYPE_ALINK,LINKKIT_TSL_STRING_TEST,strlen(LINKKIT_TSL_STRING_TEST),&dtsl);

	#if 0 /* Struct Bool Set */
	char *key = "WorkPlans.PlantSwitch";
	int value = 1;
	iotx_dsw_set_property_value(&dtsl,key,strlen(key),&value,0);
	#endif
	
	#if 0 /* Struct Bool Get */
	int res = 0;
	char *key = "WorkPlans.PlantSwitch";
	iotx_dsw_data_t *property = NULL;
	res = iotx_dsw_get_property_data(&dtsl,key,strlen(key),(void **)&property);
	if (res == SUCCESS_RETURN) {
		dm_log_debug("Property Found, Identifier: %s",property->identifier);
	}
	#endif

	#if 0 /* Struct Int Set */
	char *key = "WorkPlans.SelectIndex";
	int value = 8;
	iotx_dsw_set_property_value(&dtsl,key,strlen(key),&value,0);
	#endif
	
	#if 0 /* Struct Int Get */
	int res = 0;
	char *key = "WorkPlans.SelectIndex";
	iotx_dsw_data_t *property = NULL;
	res = iotx_dsw_get_property_data(&dtsl,key,strlen(key),(void **)&property);
	if (res == SUCCESS_RETURN) {
		dm_log_debug("Property Found, Identifier: %s",property->identifier);
	}
	#endif

	#if 0 /* Struct Text Set */
	char *key_set = "WorkPlans.PlantInfo1";
	char *value_set = "Hello, World";
	iotx_dsw_set_property_value(&dtsl,key_set,strlen(key_set),value_set,strlen(value_set));
	#endif
	
	#if 0 /* Struct Text Get */
	int res = 0;
	char *key = "WorkPlans.PlantInfo1";
	char *value = NULL;
	iotx_dsw_data_t *property = NULL;
	res = iotx_dsw_get_property_data(&dtsl,key,strlen(key),(void **)&property);
	if (res == SUCCESS_RETURN) {
		dm_log_debug("Property Found, Identifier: %s",property->identifier);
	}
	res = iotx_dsw_get_property_value(&dtsl,key,strlen(key),(void *)&value);
	if (res == SUCCESS_RETURN) {
		dm_log_debug("Property Found,Value: %s",value);
		DM_free(value);
	}
	#endif
	
	#if 0 /* Struct Array Int Set */
	char *key = "WorkPlans.PlantInfo4[3]";
	int value = 5;
	iotx_dsw_set_property_value(&dtsl,key,strlen(key),&value,0);
	#endif
	
	#if 0 /* Struct Array Int Get */
	int res = 0;
	char *key = "WorkPlans.PlantInfo4[3]";
	iotx_dsw_data_t *property = NULL;
	res = iotx_dsw_get_property_data(&dtsl,key,strlen(key),(void **)&property);
	if (res == SUCCESS_RETURN) {
		dm_log_debug("Property Found, Identifier: %s",property->identifier);
	}
	#endif

	#if 0 /* Event Int Set */
	char *key_set = "Error.ErrorCode";
	int value_set = 18;
	iotx_dsw_set_event_output_value(&dtsl,key_set,strlen(key_set),&value_set,0);
	#endif
	
	#if 0 /* Event Int Get */
	int res = 0;
	char *key_get = "Error.ErrorCode";
	int value_get = 0;
	res = iotx_dsw_get_event_output_value(&dtsl,key_get,strlen(key_get),&value_get);
	if (res == SUCCESS_RETURN) {
		dm_log_debug("Event Found, Identifier: %d",value_get);
	}
	#endif
	
	iotx_dsw_destroy(&dtsl);
	LITE_dump_malloc_free_stats(LOG_DEBUG_LEVEL);
}
#endif

#if 0
int main(int argc, char *argv[])
{
	LITE_openlog("linkkit");
	LITE_set_loglevel(LOG_DEBUG_LEVEL);
	
	iotx_cm_send_peer_t source;
	iotx_cm_message_info_t msg;
	char *uri = "/ext/session/product_key/device_name/combine/login_reply";

	memset(&source,0,sizeof(iotx_cm_send_peer_t));
	memset(&msg,0,sizeof(iotx_cm_message_info_t));

	memcpy(source.product_key,"hello",strlen("hello"));
	memcpy(source.device_name,"eden",strlen("eden"));

	msg.URI = uri;
	msg.URI_length = strlen(uri);
	iotx_dcw_topic_callback(&source, &msg, NULL);
}
#endif
