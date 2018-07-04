#include "iot_import.h"
#include "lite-utils.h"
#include "lite-list.h"
#include "lite-cjson.h"
#include "iotx_dm_common.h"
#include "iotx_dm_api.h"
#include "iotx_dm_message_cache.h"
#include "iotx_dm_message.h"
#include "iotx_dm_manager.h"
#include "iotx_dm_cm_wrapper.h"
#include "iotx_dm_ipc.h"
#include "iotx_dm_msg_dispatch.h"
#include "iot_export_dm.h"

static iotx_dapi_ctx_t g_iotx_dapi_ctx;

static iotx_dapi_ctx_t* _iotx_dapi_get_ctx(void)
{
	return &g_iotx_dapi_ctx;
}

static void _iotx_dapi_lock(void)
{
	iotx_dapi_ctx_t *ctx = _iotx_dapi_get_ctx();
	if (ctx->mutex) {HAL_MutexLock(ctx->mutex);}
}

static void _iotx_dapi_unlock(void) {
	iotx_dapi_ctx_t *ctx = _iotx_dapi_get_ctx();
	if (ctx->mutex) {HAL_MutexUnlock(ctx->mutex);}
}

int IOT_DM_Construct(_IN_ iotx_dm_init_params_t *init_params)
{
	int res = 0;
	iotx_dapi_ctx_t *ctx = _iotx_dapi_get_ctx();

	memset(ctx,0,sizeof(iotx_dapi_ctx_t));

	if (init_params == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	/* DM Mutex Create*/
	ctx->mutex = HAL_MutexCreate();
	if (ctx->mutex == NULL) {
		dm_log_err(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}

	/* DM Event Callback */
	if (init_params->event_callback != NULL) {
		ctx->event_callback = init_params->event_callback;
	}

	/* DM Message Cache Init */
	res = iotx_dmc_init();
	if (res != SUCCESS_RETURN) {
		dm_log_err(IOTX_DM_LOG_MESSAGE_CACHE_INIT_FAILED);
		goto ERROR;
	}
	
	/* DM Cloud Message Parse And Assemble Module Init */
	res = iotx_dmsg_init();
	if (res != SUCCESS_RETURN) {
		dm_log_err(IOTX_DM_LOG_MESSAGE_INIT_FAILED);
		goto ERROR;
	}

	/* DM IPC Module Init */
	res = iotx_dipc_init(IOTX_DM_DIPC_MSGLIST_MAXLEN);
	if (res != SUCCESS_RETURN) {
		dm_log_err(IOTX_DM_LOG_IPC_INIT_FAILED);
		goto ERROR;
	}

	/* DM Manager Module Init */
	res = iotx_dmgr_init();
	if (res != SUCCESS_RETURN) {
		dm_log_err(IOTX_DM_LOG_DMGR_INIT_FAILED);
		goto ERROR;
	}
	
	/* DM CM Wrapper Module Init */
	res = iotx_dcw_init(init_params->secret_type,init_params->domain_type);
	if (res != SUCCESS_RETURN) {
		dm_log_err(IOTX_DM_LOG_CM_INIT_FAILED);
		goto ERROR;
	}

	/* DM Service Module Init */
	res = iotx_dcs_init();
	if (res != SUCCESS_RETURN) {
		dm_log_err(IOTX_DM_LOG_CM_INIT_FAILED);
		goto ERROR;
	}
	
	return SUCCESS_RETURN;

ERROR:
	iotx_dcs_deinit();
	iotx_dcw_deinit();
	iotx_dmgr_deinit();
	iotx_dipc_deinit();
	iotx_dmsg_deinit();
	iotx_dmc_deinit();
	if (ctx->mutex) {HAL_MutexDestroy(ctx->mutex);}
	return FAIL_RETURN;
}

int IOT_DM_Destroy(void)
{
	iotx_dapi_ctx_t *ctx = _iotx_dapi_get_ctx();
	iotx_dcs_deinit();
	iotx_dcw_deinit();
	iotx_dmgr_deinit();
	iotx_dipc_deinit();
	iotx_dmsg_deinit();
	iotx_dmc_deinit();
	if (ctx->mutex) {HAL_MutexDestroy(ctx->mutex);}
	return SUCCESS_RETURN;
}

int IOT_DM_Set_TSL(_IN_ int devid, _IN_ iotx_dm_tsl_source_t source, _IN_ const char *tsl, _IN_ int tsl_len)
{
	int res = 0;

	if (devid < 0) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	_iotx_dapi_lock();
	if (source == IOTX_DM_TSL_SOURCE_CLOUD) {
		int sub_generic_index = 0;
		char product_key[PRODUCT_KEY_MAXLEN] = {0};
		char device_name[DEVICE_NAME_MAXLEN] = {0};
		char device_secret[DEVICE_SECRET_MAXLEN] = {0};

		res = iotx_dmgr_set_tsl_source(devid,source);
		if (res != SUCCESS_RETURN) {_iotx_dapi_unlock();return FAIL_RETURN;}

		res = iotx_dmgr_get_dev_sub_generic_index(devid,&sub_generic_index);
		if (res != SUCCESS_RETURN) {_iotx_dapi_unlock();return FAIL_RETURN;}

		if (sub_generic_index != IOTX_DMGR_DEV_SUB_END) {_iotx_dapi_unlock();return SUCCESS_RETURN;}
			
		res = iotx_dmgr_search_device_by_devid(devid,product_key,device_name,device_secret);
		if (res != SUCCESS_RETURN) {_iotx_dapi_unlock();return FAIL_RETURN;}
	
		res = iotx_dcs_thing_dsltemplate_get(product_key,device_name);

		_iotx_dapi_unlock();
		return res;
	}

	if (source == IOTX_DM_TSL_SOURCE_LOCAL) {
		if (tsl == NULL || tsl_len <= 0) {
			dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
			_iotx_dapi_unlock();
			return FAIL_RETURN;
		}

		res = iotx_dmgr_set_tsl(devid,IOTX_DM_TSL_TYPE_ALINK,tsl,tsl_len);
		if (res != SUCCESS_RETURN) {_iotx_dapi_unlock();return FAIL_RETURN;}

		res = iotx_dcs_topic_service_event_destroy(devid);
		if (res != SUCCESS_RETURN) {_iotx_dapi_unlock();return FAIL_RETURN;}
		
		res = iotx_dcs_topic_service_event_create(devid);
		if (res != SUCCESS_RETURN) {_iotx_dapi_unlock();return FAIL_RETURN;}

		_iotx_dapi_unlock();
		return SUCCESS_RETURN;
	}

	_iotx_dapi_unlock();
	return FAIL_RETURN;
}

int IOT_DM_Set_Property_Value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value, _IN_ int value_len)
{
	int res = 0;
	
	if (devid < 0 || key == NULL || key_len <= 0 || value == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	_iotx_dapi_lock();
	res = iotx_dmgr_set_property_value(devid,key,key_len,value,value_len);
	if (res != SUCCESS_RETURN) {_iotx_dapi_unlock();return FAIL_RETURN;}

	_iotx_dapi_unlock();
	return SUCCESS_RETURN;
}

int IOT_DM_Get_Property_Value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value)
{
	int res = 0;

	if (devid < 0 || key == NULL || key_len <= 0 || value == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	_iotx_dapi_lock();
	res = iotx_dmgr_get_property_value(devid,key,key_len,value);
	if (res != SUCCESS_RETURN) {_iotx_dapi_unlock();return FAIL_RETURN;}

	_iotx_dapi_unlock();
	return SUCCESS_RETURN;
}

int IOT_DM_Set_Event_Output_Value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value, _IN_ int value_len)
{
	int res = 0;

	if (devid < 0 || key == NULL || key_len <= 0 || value == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	_iotx_dapi_lock();
	res = iotx_dmgr_set_event_output_value(devid,key,key_len,value,value_len);
	if (res != SUCCESS_RETURN) {_iotx_dapi_unlock();return FAIL_RETURN;}

	_iotx_dapi_unlock();
	return SUCCESS_RETURN;
}

int IOT_DM_Get_Event_Output_Value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value)
{
	int res = 0;

	if (devid < 0 || key == NULL || key_len <= 0 || value == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	_iotx_dapi_lock();
	res = iotx_dmgr_get_event_output_value(devid,key,key_len,value);
	if (res != SUCCESS_RETURN) {_iotx_dapi_unlock();return FAIL_RETURN;}

	_iotx_dapi_unlock();
	return SUCCESS_RETURN;
}

int IOT_DM_Get_Service_Input_Value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value)
{
	int res = 0;

	if (devid < 0 || key == NULL || key_len <= 0 || value == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	_iotx_dapi_lock();
	res = iotx_dmgr_get_service_input_value(devid,key,key_len,value);
	if (res != SUCCESS_RETURN) {_iotx_dapi_unlock();return FAIL_RETURN;}

	_iotx_dapi_unlock();
	return SUCCESS_RETURN;
}

int IOT_DM_Set_Service_Output_Value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value, _IN_ int value_len)
{
	int res = 0;

	if (devid < 0 || key == NULL || key_len <= 0 || value == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	_iotx_dapi_lock();
	res = iotx_dmgr_set_service_output_value(devid,key,key_len,value,value_len);
	if (res != SUCCESS_RETURN) {_iotx_dapi_unlock();return FAIL_RETURN;}

	_iotx_dapi_unlock();
	return SUCCESS_RETURN;
}

int IOT_DM_Get_Service_Output_Value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value)
{
	int res = 0;

	if (devid < 0 || key == NULL || key_len <= 0 || value == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	_iotx_dapi_lock();
	res = iotx_dmgr_get_service_output_value(devid,key,key_len,value);
	if (res != SUCCESS_RETURN) {_iotx_dapi_unlock();return FAIL_RETURN;}

	_iotx_dapi_unlock();
	return SUCCESS_RETURN;
}

int IOT_DM_Post_Property_Start(_IN_ int devid, _OU_ void **handle)
{
	iotx_dapi_property_t *dapi_property = NULL;

	if (devid < 0 || handle == NULL || *handle != NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	_iotx_dapi_lock();
	dapi_property = DM_malloc(sizeof(iotx_dapi_property_t));
	if (dapi_property == NULL) {
		dm_log_err(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		_iotx_dapi_unlock();
		return FAIL_RETURN;
	}
	memset(dapi_property,0,sizeof(iotx_dapi_property_t));


	/* Create Mutex */
	dapi_property->mutex = HAL_MutexCreate();
	if (dapi_property->mutex == NULL) {
		DM_free(dapi_property);
		dm_log_err(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		_iotx_dapi_unlock();
		return FAIL_RETURN;
	}

	/* Set Devid */
	dapi_property->devid = devid;

	/* Init Json Object */
	dapi_property->lite = lite_cjson_create_object();
	if (dapi_property->lite == NULL) {
		DM_free(dapi_property->mutex);
		DM_free(dapi_property);
		_iotx_dapi_unlock();
		return FAIL_RETURN;
	}

	*handle = (void *)dapi_property;

	_iotx_dapi_unlock();
	return SUCCESS_RETURN;
}

static int _iotx_dm_post_property_add(_IN_ void *handle, _IN_ char *identifier, _IN_ int identifier_len)
{
	int res = 0;
	iotx_dapi_property_t *dapi_property = NULL;
	
	if (handle == NULL || identifier == NULL || identifier_len <= 0) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}
	dapi_property = (iotx_dapi_property_t *)handle;
	
	/* Assemble Property Payload */
	res = iotx_dmgr_assemble_property(dapi_property->devid,identifier,identifier_len,dapi_property->lite);
	if (res != SUCCESS_RETURN) {
		dm_log_err(IOTX_DM_LOG_TSL_PROPERTY_ASSEMBLE_FAILED,identifier_len,identifier);
		return FAIL_RETURN;
	}
	
	return SUCCESS_RETURN;
}

int IOT_DM_Post_Property_Add(_IN_ void *handle, _IN_ char *identifier, _IN_ int identifier_len)
{
	int ret = SUCCESS_RETURN, res = 0, index = 0, number = 0;
	void *property_refer = NULL;
	char *identifier_refer = NULL;
	iotx_dapi_property_t *dapi_property = NULL;
	
	if (handle == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	_iotx_dapi_lock();
	dapi_property = (iotx_dapi_property_t *)handle;
	
	if (identifier != IOTX_DM_POST_PROPERTY_ALL) {
		if (identifier_len <= 0) {_iotx_dapi_unlock();return FAIL_RETURN;}
		ret = _iotx_dm_post_property_add(handle,identifier,identifier_len);

		_iotx_dapi_unlock();
		return ret;
	}
	
	res = iotx_dmgr_get_property_number(dapi_property->devid,&number);
	if (res != SUCCESS_RETURN) {_iotx_dapi_unlock();return FAIL_RETURN;}

	for (index = 0;index < number;index++) {
		property_refer = NULL;identifier_refer = NULL;
		
		res = iotx_dmgr_get_property_by_index(dapi_property->devid,index,&property_refer);
		if (res != SUCCESS_RETURN) {continue;}

		res = iotx_dmgr_get_property_identifier(property_refer,&identifier_refer);
		if (res != SUCCESS_RETURN) {continue;}

		res = _iotx_dm_post_property_add(handle,identifier_refer,strlen(identifier_refer));
		if (res != SUCCESS_RETURN) {ret = FAIL_RETURN;}
	}

	_iotx_dapi_unlock();
	return ret;
}

int IOT_DM_Post_Property_End(_IN_ void **handle)
{
	int res = 0;
	char *payload = NULL;
	char product_key[PRODUCT_KEY_MAXLEN] = {0};
	char device_name[DEVICE_NAME_MAXLEN] = {0};
	char device_secret[DEVICE_SECRET_MAXLEN] = {0};
	iotx_dapi_property_t *dapi_property = NULL;

	if (handle == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	_iotx_dapi_lock();
	dapi_property = *((iotx_dapi_property_t **)handle);

	payload = lite_cjson_print_unformatted(dapi_property->lite);
	if (payload == NULL) {
		lite_cjson_delete(dapi_property->lite);
		if (dapi_property->mutex) {HAL_MutexDestroy(dapi_property->mutex);}
		DM_free(dapi_property);
		dm_log_err(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		_iotx_dapi_unlock();
		return FAIL_RETURN;
	}

	dm_log_debug("Current Property Post Payload, Length: %d, Payload: %s",strlen(payload),payload);

	res = iotx_dmgr_search_device_by_devid(dapi_property->devid,product_key,device_name,device_secret);
	if (res == SUCCESS_RETURN) {
		res = iotx_dcs_thing_property_post(product_key,device_name,payload,strlen(payload));
	}
	
	free(payload);
	lite_cjson_delete(dapi_property->lite);
	if (dapi_property->mutex) {HAL_MutexDestroy(dapi_property->mutex);}
	DM_free(dapi_property);
	*handle = NULL;
	
	_iotx_dapi_unlock();
	return res;
}

int IOT_DM_Post_Event(_IN_ int devid, _IN_ char *identifier, _IN_ int identifier_len)
{
	int res = 0;
	char product_key[PRODUCT_KEY_MAXLEN] = {0};
	char device_name[DEVICE_NAME_MAXLEN] = {0};
	char device_secret[DEVICE_SECRET_MAXLEN] = {0};
	void *event = NULL;
	lite_cjson_item_t *lite = NULL;
	char *method = NULL, *payload = NULL;
	
	if (devid < 0 || identifier == NULL || identifier_len <= 0) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	_iotx_dapi_lock();
	lite = lite_cjson_create_object();
	if (lite == NULL) {
		dm_log_err(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		_iotx_dapi_unlock();
		return FAIL_RETURN;
	}
	
	res = iotx_dmgr_assemble_event_output(devid,identifier,identifier_len,lite);
	if (res != SUCCESS_RETURN) {
		lite_cjson_delete(lite);
		dm_log_err(IOTX_DM_LOG_TSL_EVENT_OUTPUT_ASSEMBLE_FAILED,identifier_len,identifier);
		_iotx_dapi_unlock();
		return FAIL_RETURN;
	}

	payload = lite_cjson_print_unformatted(lite);
	lite_cjson_delete(lite);
	if (payload == NULL) {
		dm_log_err(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		_iotx_dapi_unlock();
		return FAIL_RETURN;
	}

	dm_log_debug("Current Event Post Payload, Length: %d, Payload: %s",strlen(payload),payload);
	
	res = iotx_dmgr_search_device_by_devid(devid,product_key,device_name,device_secret);
	if (res != SUCCESS_RETURN) {free(payload);_iotx_dapi_unlock();return FAIL_RETURN;}

	res = iotx_dmgr_get_event_by_identifier(devid,identifier,&event);
	if (res != SUCCESS_RETURN) {free(payload);_iotx_dapi_unlock();return FAIL_RETURN;}

	res = iotx_dmgr_get_event_method(event,&method);
	if (res != SUCCESS_RETURN) {free(payload);_iotx_dapi_unlock();return FAIL_RETURN;}

	dm_log_debug("Current Event Method: %s",method);
	
	res = iotx_dcs_thing_event_post(product_key,device_name,identifier,identifier_len,method,payload,strlen(payload));
	
	free(payload);DM_free(method);

	_iotx_dapi_unlock();
	return res;
}

int IOT_DM_Post_Property_Direct(_IN_ int devid, _IN_ char *payload, _IN_ int payload_len)
{
	int res = 0;
	char product_key[PRODUCT_KEY_MAXLEN] = {0};
	char device_name[DEVICE_NAME_MAXLEN] = {0};
	char device_secret[DEVICE_SECRET_MAXLEN] = {0};

	_iotx_dapi_lock();
	res = iotx_dmgr_search_device_by_devid(devid,product_key,device_name,device_secret);
	if (res != SUCCESS_RETURN) {_iotx_dapi_unlock();return FAIL_RETURN;}
	
	res = iotx_dcs_thing_property_post(product_key,device_name,payload,payload_len);
	if (res < SUCCESS_RETURN) {_iotx_dapi_unlock();return FAIL_RETURN;}

	_iotx_dapi_unlock();
	return res;
}

int IOT_DM_Post_Event_Direct(_IN_ int devid, _IN_ char *identifier, _IN_ int identifier_len, _IN_ char *payload, _IN_ int payload_len)
{
	int res = 0, method_len = 0;
	const char *method_fmt = "thing.event.%.*s.post";
	char *method = NULL;
	char product_key[PRODUCT_KEY_MAXLEN] = {0};
	char device_name[DEVICE_NAME_MAXLEN] = {0};
	char device_secret[DEVICE_SECRET_MAXLEN] = {0};

	if (devid < 0 || identifier == NULL || identifier_len == 0 || payload == NULL || payload_len <= 0) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	_iotx_dapi_lock();
	res = iotx_dmgr_search_device_by_devid(devid,product_key,device_name,device_secret);
	if (res != SUCCESS_RETURN) {_iotx_dapi_unlock();return FAIL_RETURN;}

	method_len = strlen(method_fmt) + strlen(identifier) + 1;
	method = DM_malloc(method_len);
	if (method == NULL) {
		dm_log_err(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		_iotx_dapi_unlock();
		return FAIL_RETURN;
	}
	memset(method,0,method_len);
	HAL_Snprintf(method,method_len,method_fmt,identifier_len,identifier);
	
	res = iotx_dcs_thing_event_post(product_key,device_name,identifier,identifier_len,method,payload,strlen(payload));
	if (res < SUCCESS_RETURN) {DM_free(method);_iotx_dapi_unlock();return FAIL_RETURN;}

	DM_free(method);
	_iotx_dapi_unlock();
	return res;
}

int IOT_DM_Send_Service_Response(_IN_ int devid, _IN_ int msgid, _IN_ iotx_dm_error_code_t code, _IN_ char *identifier, _IN_ int identifier_len)
{
	int res = 0;
	char product_key[PRODUCT_KEY_MAXLEN] = {0};
	char device_name[DEVICE_NAME_MAXLEN] = {0};
	char device_secret[DEVICE_SECRET_MAXLEN] = {0};
	lite_cjson_item_t *lite = NULL;
	char *payload = NULL;
	
	if (devid < 0 || msgid < 0 || identifier == NULL || identifier_len <= 0) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	_iotx_dapi_lock();
	lite = lite_cjson_create_object();
	if (lite == NULL) {
		dm_log_err(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		_iotx_dapi_unlock();
		return FAIL_RETURN;
	}
	
	res = iotx_dmgr_assemble_service_output(devid,identifier,identifier_len,lite);
	if (res != SUCCESS_RETURN) {
		lite_cjson_delete(lite);
		dm_log_err(IOTX_DM_LOG_TSL_EVENT_OUTPUT_ASSEMBLE_FAILED,strlen(identifier),identifier);
		_iotx_dapi_unlock();
		return FAIL_RETURN;
	}

	payload = lite_cjson_print_unformatted(lite);
	lite_cjson_delete(lite);
	if (payload == NULL) {
		dm_log_err(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		_iotx_dapi_unlock();
		return FAIL_RETURN;
	}

	dm_log_debug("Current Service Response Payload, Length: %d, Payload: %s",strlen(payload),payload);
	
	res = iotx_dmgr_search_device_by_devid(devid,product_key,device_name,device_secret);
	if (res != SUCCESS_RETURN) {free(payload);_iotx_dapi_unlock();return FAIL_RETURN;}
	
	res = iotx_dcs_thing_service_response(product_key,device_name,msgid,code,identifier,identifier_len,payload,strlen(payload));
	
	free(payload);

	_iotx_dapi_unlock();
	return res;
}

int IOT_DM_DeviceInfo_Update(_IN_ int devid, _IN_ char *payload, _IN_ int payload_len)
{
	int res = 0;
	char product_key[PRODUCT_KEY_MAXLEN] = {0};
	char device_name[DEVICE_NAME_MAXLEN] = {0};
	char device_secret[DEVICE_SECRET_MAXLEN] = {0};
	
	if (devid < 0 || payload == NULL || payload_len <= 0) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	_iotx_dapi_lock();
	res = iotx_dmgr_search_device_by_devid(devid,product_key,device_name,device_secret);
	if (res != SUCCESS_RETURN) {_iotx_dapi_unlock();return FAIL_RETURN;}
	
	res = iotx_dcs_thing_deviceinfo_update(product_key,device_name,payload,payload_len);
	if (res < SUCCESS_RETURN) {_iotx_dapi_unlock();return FAIL_RETURN;}

	_iotx_dapi_unlock();
	return res;
}

int IOT_DM_DeviceInfo_Delete(_IN_ int devid, _IN_ char *payload, _IN_ int payload_len)
{
	int res = 0;
	char product_key[PRODUCT_KEY_MAXLEN] = {0};
	char device_name[DEVICE_NAME_MAXLEN] = {0};
	char device_secret[DEVICE_SECRET_MAXLEN] = {0};
	
	if (devid < 0 || payload == NULL || payload_len <= 0) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	_iotx_dapi_lock();
	res = iotx_dmgr_search_device_by_devid(devid,product_key,device_name,device_secret);
	if (res != SUCCESS_RETURN) {_iotx_dapi_unlock();return FAIL_RETURN;}
	
	res = iotx_dcs_thing_deviceinfo_delete(product_key,device_name,payload,payload_len);
	if (res < SUCCESS_RETURN) {_iotx_dapi_unlock();return FAIL_RETURN;}

	_iotx_dapi_unlock();
	return res;
}

int IOT_DM_Post_Rawdata(_IN_ int devid, _IN_ char *payload, _IN_ int payload_len)
{
	int res = 0;
	char product_key[PRODUCT_KEY_MAXLEN] = {0};
	char device_name[DEVICE_NAME_MAXLEN] = {0};
	char device_secret[DEVICE_SECRET_MAXLEN] = {0};

	if (devid < 0 || payload == NULL || payload_len <= 0) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	_iotx_dapi_lock();
	res = iotx_dmgr_search_device_by_devid(devid,product_key,device_name,device_secret);
	if (res != SUCCESS_RETURN) {_iotx_dapi_unlock();return FAIL_RETURN;}
	
	res = iotx_dcs_thing_model_up_raw(product_key,device_name,payload,payload_len);
	if (res != SUCCESS_RETURN) {_iotx_dapi_unlock();return FAIL_RETURN;}

	_iotx_dapi_unlock();
	return SUCCESS_RETURN;
}

int IOT_DM_Yield(int timeout_ms)
{
	return iotx_dcw_yield(timeout_ms);
}

void IOT_DM_Dispatch(void)
{
	iotx_dapi_ctx_t *ctx = _iotx_dapi_get_ctx();
	void *data = NULL;

	iotx_dmgr_dev_sub_status_check();
	iotx_dmc_msg_tick();
	if (iotx_dipc_msg_next(&data) == SUCCESS_RETURN) {
		iotx_dipc_msg_t *msg = (iotx_dipc_msg_t *)data;

		if (ctx->event_callback) {ctx->event_callback(msg->type,msg->data);}
		
		if (msg->data) {DM_free(msg->data);}
		DM_free(msg);data = NULL;
	}
}

int IOT_DM_Subdev_Create(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN], _OU_ int *devid)
{
	int res = 0;

	if (product_key == NULL || device_name == NULL ||
		(strlen(product_key) >= PRODUCT_KEY_MAXLEN) ||
		(strlen(device_name) >= DEVICE_NAME_MAXLEN) || 
		devid == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	_iotx_dapi_lock();
	res = iotx_dmgr_device_create(IOTX_DM_DEVICE_SUBDEV,product_key,device_name,devid);
	if (res != SUCCESS_RETURN) {_iotx_dapi_unlock();return FAIL_RETURN;}

	_iotx_dapi_unlock();
	return SUCCESS_RETURN;
}

int IOT_DM_Subdev_Destroy(_IN_ int devid)
{
	int res = 0;

	if (devid < 0) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	_iotx_dapi_lock();
	res = iotx_dmgr_device_destroy(devid);
	if (res != SUCCESS_RETURN) {_iotx_dapi_unlock();return FAIL_RETURN;}

	_iotx_dapi_unlock();
	return SUCCESS_RETURN;
}

int IOT_DM_Subdev_Number(void)
{
	int number = 0;

	_iotx_dapi_lock();
	number = iotx_dmgr_device_number();
	_iotx_dapi_unlock();

	return number;
}

int IOT_DM_Subdev_Register(_IN_ int devid)
{
	int res = 0;
	char product_key[PRODUCT_KEY_MAXLEN] = {0};
	char device_name[DEVICE_NAME_MAXLEN] = {0};
	char device_secret[DEVICE_SECRET_MAXLEN] = {0};
	
	if (devid < 0) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	_iotx_dapi_lock();
	res = iotx_dmgr_search_device_by_devid(devid,product_key,device_name,device_secret);
	if (res != SUCCESS_RETURN) {_iotx_dapi_unlock();return FAIL_RETURN;}
	
	res = iotx_dcs_thing_sub_register(product_key,device_name);

	_iotx_dapi_unlock();
	return res;
}

int IOT_DM_Subdev_Unregister(_IN_ int devid)
{
	int res = 0;
	char product_key[PRODUCT_KEY_MAXLEN] = {0};
	char device_name[DEVICE_NAME_MAXLEN] = {0};
	char device_secret[DEVICE_SECRET_MAXLEN] = {0};
	
	if (devid < 0) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	_iotx_dapi_lock();
	res = iotx_dmgr_search_device_by_devid(devid,product_key,device_name,device_secret);
	if (res != SUCCESS_RETURN) {_iotx_dapi_unlock();return FAIL_RETURN;}
	
	res = iotx_dcs_thing_sub_unregister(product_key,device_name);

	_iotx_dapi_unlock();
	return res;
}

int IOT_DM_Subdev_Topo_Add(_IN_ int devid)
{
	int res = 0;
	char product_key[PRODUCT_KEY_MAXLEN] = {0};
	char device_name[DEVICE_NAME_MAXLEN] = {0};
	char device_secret[DEVICE_SECRET_MAXLEN] = {0};
	
	if (devid < 0) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	_iotx_dapi_lock();
	res = iotx_dmgr_search_device_by_devid(devid,product_key,device_name,device_secret);
	if (res != SUCCESS_RETURN) {_iotx_dapi_unlock();return FAIL_RETURN;}

	res = iotx_dcs_thing_topo_add(product_key,device_name,device_secret);

	_iotx_dapi_unlock();
	return res;
}

int IOT_DM_Subdev_Topo_Del(_IN_ int devid)
{
	int res = 0;
	char product_key[PRODUCT_KEY_MAXLEN] = {0};
	char device_name[DEVICE_NAME_MAXLEN] = {0};
	char device_secret[DEVICE_SECRET_MAXLEN] = {0};
	
	if (devid < 0) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	_iotx_dapi_lock();
	res = iotx_dmgr_search_device_by_devid(devid,product_key,device_name,device_secret);
	if (res != SUCCESS_RETURN) {_iotx_dapi_unlock();return FAIL_RETURN;}

	res = iotx_dcs_thing_topo_delete(product_key,device_name);

	_iotx_dapi_unlock();
	return res;
}

int IOT_DM_Subdev_Login(_IN_ int devid)
{
	int res = 0;
	char product_key[PRODUCT_KEY_MAXLEN] = {0};
	char device_name[DEVICE_NAME_MAXLEN] = {0};
	char device_secret[DEVICE_SECRET_MAXLEN] = {0};
	
	if (devid < 0) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	_iotx_dapi_lock();
	res = iotx_dmgr_search_device_by_devid(devid,product_key,device_name,device_secret);
	if (res != SUCCESS_RETURN) {_iotx_dapi_unlock();return FAIL_RETURN;}
	
	res = iotx_dcs_combine_login(product_key,device_name,device_secret);

	_iotx_dapi_unlock();
	return res;
}

int IOT_DM_Subdev_Logout(_IN_ int devid)
{
	int res = 0;
	char product_key[PRODUCT_KEY_MAXLEN] = {0};
	char device_name[DEVICE_NAME_MAXLEN] = {0};
	char device_secret[DEVICE_SECRET_MAXLEN] = {0};
	
	if (devid < 0) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	_iotx_dapi_lock();
	res = iotx_dmgr_search_device_by_devid(devid,product_key,device_name,device_secret);
	if (res != SUCCESS_RETURN) {_iotx_dapi_unlock();return FAIL_RETURN;}
	
	res = iotx_dcs_combine_logout(product_key,device_name);

	_iotx_dapi_unlock();
	return res;
}

int IOT_DM_Get_Device_Type(_IN_ int devid, _OU_ int *type)
{
	int res = 0;

	if (devid < 0 || type == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	_iotx_dapi_lock();
	res = iotx_dmgr_get_dev_type(devid,type);
	if (res != SUCCESS_RETURN) {_iotx_dapi_unlock();return FAIL_RETURN;}

	_iotx_dapi_unlock();
	return SUCCESS_RETURN;
}

int IOT_DM_Get_Device_Avail_Status(_IN_ int devid, _OU_ iotx_dm_dev_avail_t *status)
{
	int res = 0;
	char product_key[PRODUCT_KEY_MAXLEN] = {0};
	char device_name[DEVICE_NAME_MAXLEN] = {0};
	char device_secret[DEVICE_SECRET_MAXLEN] = {0};

	if (devid < 0 || status == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	_iotx_dapi_lock();
	res = iotx_dmgr_search_device_by_devid(devid,product_key,device_name,device_secret);
	if (res != SUCCESS_RETURN) {_iotx_dapi_unlock();return FAIL_RETURN;}
	
	res = iotx_dmgr_get_dev_avail(product_key,device_name,status);
	if (res != SUCCESS_RETURN) {_iotx_dapi_unlock();return FAIL_RETURN;}

	_iotx_dapi_unlock();
	return SUCCESS_RETURN;
}

int IOT_DM_Get_Device_Status(_IN_ int devid, _OU_ iotx_dm_dev_status_t *status)
{
	int res = 0;
	
	if (devid < 0 || status == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	_iotx_dapi_lock();
	res = iotx_dmgr_get_dev_status(devid,status);
	_iotx_dapi_unlock();

	return res;
}

int IOT_DM_Legacy_Set_Property_Value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value)
{
	int res = 0, value_len = 0;
	void *data = NULL;
	iotx_dsw_data_type_e type;
	
	if (devid < 0 || key == NULL || key_len <= 0 || value == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	_iotx_dapi_lock();
	res = iotx_dmgr_get_property_data(devid,key,key_len,&data);
	if (res != SUCCESS_RETURN) {_iotx_dapi_unlock();return FAIL_RETURN;}

	res = iotx_dmgr_get_data_type(data,&type);
	if (res != SUCCESS_RETURN) {_iotx_dapi_unlock();return FAIL_RETURN;}

	if (type == IOTX_DSW_DATA_TYPE_TEXT || type == IOTX_DSW_DATA_TYPE_DATE) {value_len = strlen(value);}
	
	res = iotx_dmgr_set_property_value(devid,key,key_len,value,value_len);
	if (res != SUCCESS_RETURN) {_iotx_dapi_unlock();return FAIL_RETURN;}

	_iotx_dapi_unlock();
	return SUCCESS_RETURN;
}

int IOT_DM_Legacy_Set_Event_Output_Value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value)
{
	int res = 0, value_len = 0;
	void *data = NULL;
	iotx_dsw_data_type_e type;
	
	if (devid < 0 || key == NULL || key_len <= 0 || value == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	_iotx_dapi_lock();
	res = iotx_dmgr_get_event_output_data(devid,key,key_len,&data);
	if (res != SUCCESS_RETURN) {_iotx_dapi_unlock();return FAIL_RETURN;}

	res = iotx_dmgr_get_data_type(data,&type);
	if (res != SUCCESS_RETURN) {_iotx_dapi_unlock();return FAIL_RETURN;}

	if (type == IOTX_DSW_DATA_TYPE_TEXT || type == IOTX_DSW_DATA_TYPE_DATE) {value_len = strlen(value);}
	
	res = iotx_dmgr_set_property_value(devid,key,key_len,value,value_len);
	if (res != SUCCESS_RETURN) {_iotx_dapi_unlock();return FAIL_RETURN;}

	_iotx_dapi_unlock();
	return SUCCESS_RETURN;
}

int IOT_DM_Legacy_Set_Service_Output_Value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value)
{
	int res = 0, value_len = 0;
	void *data = NULL;
	iotx_dsw_data_type_e type;
	
	if (devid < 0 || key == NULL || key_len <= 0 || value == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	_iotx_dapi_lock();
	res = iotx_dmgr_get_service_output_data(devid,key,key_len,&data);
	if (res != SUCCESS_RETURN) {_iotx_dapi_unlock();return FAIL_RETURN;}

	res = iotx_dmgr_get_data_type(data,&type);
	if (res != SUCCESS_RETURN) {_iotx_dapi_unlock();return FAIL_RETURN;}

	if (type == IOTX_DSW_DATA_TYPE_TEXT || type == IOTX_DSW_DATA_TYPE_DATE) {value_len = strlen(value);}
	
	res = iotx_dmgr_set_property_value(devid,key,key_len,value,value_len);
	if (res != SUCCESS_RETURN) {_iotx_dapi_unlock();return FAIL_RETURN;}

	_iotx_dapi_unlock();
	return SUCCESS_RETURN;
}

int IOT_DM_Legacy_Get_Pkdn_By_Devid(_IN_ int devid, _OU_ char product_key[PRODUCT_KEY_MAXLEN], _OU_ char device_name[DEVICE_NAME_MAXLEN])
{
	int res = 0;

	if (devid < 0 || product_key == NULL || device_name == NULL ||
		(strlen(product_key) >= PRODUCT_KEY_MAXLEN) ||
		(strlen(device_name) >= DEVICE_NAME_MAXLEN)) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	_iotx_dapi_lock();
	res = iotx_dmgr_get_product_key(devid,product_key);
	if (res != SUCCESS_RETURN) {_iotx_dapi_unlock();return FAIL_RETURN;}
	
	res = iotx_dmgr_get_device_name(devid,device_name);
	if (res != SUCCESS_RETURN) {_iotx_dapi_unlock();return FAIL_RETURN;}

	_iotx_dapi_unlock();
	return SUCCESS_RETURN;
}

int IOT_DM_Legacy_Get_Devid_By_Pkdn(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN], _OU_ int *devid)
{
	int res = 0;

	if (devid == NULL || product_key == NULL || device_name == NULL ||
		(strlen(product_key) >= PRODUCT_KEY_MAXLEN) ||
		(strlen(device_name) >= DEVICE_NAME_MAXLEN)) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	_iotx_dapi_lock();
	res = iotx_dmgr_search_device_by_pkdn(product_key,device_name,devid);
	if (res != SUCCESS_RETURN) {_iotx_dapi_unlock();return FAIL_RETURN;}

	_iotx_dapi_unlock();
	return SUCCESS_RETURN;
}

int IOT_DM_Legacy_Get_ThingId_By_Devid(_IN_ int devid, _OU_ void **thing_id)
{
	int res = 0;

	if (devid < 0 || thing_id == NULL || *thing_id != NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	_iotx_dapi_lock();
	res = iotx_dmgr_search_device_node_by_devid(devid,thing_id);
	if (res != SUCCESS_RETURN) {_iotx_dapi_unlock();return FAIL_RETURN;}

	_iotx_dapi_unlock();
	return SUCCESS_RETURN;
}

int IOT_DM_Legacy_Get_Devid_By_ThingId(_IN_ void *thing_id, _OU_ int *devid)
{
	int res = 0;

	if (thing_id == NULL || devid == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	_iotx_dapi_lock();
	res = iotx_dmgr_search_devid_by_device_node(thing_id,devid);
	if (res != SUCCESS_RETURN) {_iotx_dapi_unlock();return FAIL_RETURN;}

	_iotx_dapi_unlock();
	return SUCCESS_RETURN;
}

int IOT_DM_Legacy_Get_Pkdn_Ptr_By_Devid(_IN_ int devid, _OU_ char **product_key, _OU_ char **device_name)
{
	int res = 0;
	iotx_dmgr_dev_node_t *node = NULL;

	if (devid < 0 || product_key == NULL || *product_key != NULL ||
		device_name == NULL || *device_name != NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	_iotx_dapi_lock();
	res = iotx_dmgr_search_device_node_by_devid(devid,(void **)&node);
	if (res != SUCCESS_RETURN) {_iotx_dapi_unlock();return FAIL_RETURN;}

	*product_key = node->product_key;
	*device_name = node->device_name;

	_iotx_dapi_unlock();
	return SUCCESS_RETURN;
}

int IOT_DM_Legacy_Send_Service_Response(_IN_ int devid, _IN_ int msgid, _IN_ iotx_dm_error_code_t code, _IN_ char *identifier, _IN_ int identifier_len, _IN_ char *payload, _IN_ int payload_len)
{
	int res = 0;
	char product_key[PRODUCT_KEY_MAXLEN] = {0};
	char device_name[DEVICE_NAME_MAXLEN] = {0};
	char device_secret[DEVICE_SECRET_MAXLEN] = {0};

	_iotx_dapi_lock();
	res = iotx_dmgr_search_device_by_devid(devid,product_key,device_name,device_secret);
	if (res != SUCCESS_RETURN) {_iotx_dapi_unlock();return FAIL_RETURN;}
	
	res = iotx_dcs_thing_service_response(product_key,device_name,msgid,code,identifier,identifier_len,payload,payload_len);

	_iotx_dapi_unlock();
	return res;
}

int IOT_DM_Legacy_Send_Rawdata(_IN_ int devid, _IN_ char *payload, _IN_ int payload_len)
{
	int res = 0;
	char product_key[PRODUCT_KEY_MAXLEN] = {0};
	char device_name[DEVICE_NAME_MAXLEN] = {0};
	char device_secret[DEVICE_SECRET_MAXLEN] = {0};

	_iotx_dapi_lock();
	res = iotx_dmgr_search_device_by_devid(devid,product_key,device_name,device_secret);
	if (res != SUCCESS_RETURN) {_iotx_dapi_unlock();return FAIL_RETURN;}
	
	res = iotx_dcs_thing_model_up_raw(product_key,device_name,payload,payload_len);

	_iotx_dapi_unlock();
	return res;
}
