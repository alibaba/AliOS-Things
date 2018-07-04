#include "iot_import.h"
#include "utils_hmac.h"
#include "utils_sha256.h"
#include "lite-utils.h"
#include "lite-cjson.h"
#include "iotx_dm_common.h"
#include "iotx_dm_message.h"
#include "iotx_dm_manager.h"
#include "iotx_dm_cm_wrapper.h"
#include "iotx_dm_shadow.h"
#include "iotx_dm_ipc.h"
#include "iotx_dm_msg_dispatch.h"
#include "iotx_dm_message_cache.h"
#include "utils_hmac.h"

static iotx_dmsg_ctx_t g_iotx_dmsg_ctx;

static int _iotx_dmsg_property_set_number(int devid, char *key, lite_cjson_t *root);
static int _iotx_dmsg_property_set_string(int devid, char *key, lite_cjson_t *root);
static int _iotx_dmsg_property_set_object(int devid, char *key, lite_cjson_t *root);
static int _iotx_dmsg_property_set_array(int devid, char *key, lite_cjson_t *root);

static int _iotx_dmsg_service_set_number(int devid, char *key, lite_cjson_t *root);
static int _iotx_dmsg_service_set_string(int devid, char *key, lite_cjson_t *root);
static int _iotx_dmsg_service_set_object(int devid, char *key, lite_cjson_t *root);
static int _iotx_dmsg_service_set_array(int devid, char *key, lite_cjson_t *root);

static iotx_dmsg_ctx_t* _iotx_dmsg_get_ctx(void)
{
	return &g_iotx_dmsg_ctx;
}

int iotx_dmsg_init(void)
{
	iotx_dmsg_ctx_t *ctx = _iotx_dmsg_get_ctx();
	memset(ctx,0,sizeof(iotx_dmsg_ctx_t));

	return SUCCESS_RETURN;
}

int iotx_dmsg_deinit(void)
{
	iotx_dmsg_ctx_t *ctx = _iotx_dmsg_get_ctx();
	memset(ctx,0,sizeof(iotx_dmsg_ctx_t));

	return SUCCESS_RETURN;
}

int iotx_dmsg_get_id(void)
{
	iotx_dmsg_ctx_t *ctx = _iotx_dmsg_get_ctx();

	ctx->id++;
	if (ctx->id < 0) {ctx->id = 1;}

	return ctx->id;
}

int _iotx_dmsg_send_to_user(iotx_dm_event_types_t type, char *message)
{
	int res = 0;
	iotx_dipc_msg_t *dipc_msg = NULL;

	dipc_msg = DM_malloc(sizeof(iotx_dipc_msg_t));
	if (dipc_msg == NULL) {
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(dipc_msg,0,sizeof(iotx_dipc_msg_t));

	dipc_msg->type = type;
	dipc_msg->data = message;

	res = iotx_dipc_msg_insert((void *)dipc_msg);
	if (res != SUCCESS_RETURN) {
		DM_free(dipc_msg);
	}

	return SUCCESS_RETURN;
}

const char IOTX_DMSG_SEND_MSG_TIMEOUT_FMT[] DM_READ_ONLY = "{\"id\":%d,\"code\":%d,\"devid\":%d}";
int iotx_dmsg_send_msg_timeout_to_user(int msg_id, int devid, iotx_dm_event_types_t type)
{
	int res = 0, message_len = 0;
	char *message = NULL;

	message_len = strlen(IOTX_DMSG_SEND_MSG_TIMEOUT_FMT) + IOTX_DCM_UINT32_STRLEN * 3 + 1;
	message = DM_malloc(message_len + 1);
	if (message == NULL) {
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(message,0,message_len);
	HAL_Snprintf(message,message_len,IOTX_DMSG_SEND_MSG_TIMEOUT_FMT,msg_id,IOTX_DM_ERR_CODE_TIMEOUT,devid);

	res = _iotx_dmsg_send_to_user(type,message);
	if (res != SUCCESS_RETURN) {DM_free(message);return FAIL_RETURN;}

	return SUCCESS_RETURN;
}

int iotx_dmsg_uri_parse_pkdn(_IN_ char *uri, _IN_ int uri_len, _IN_ int start_deli, _IN_ int end_deli, _OU_ char product_key[PRODUCT_KEY_MAXLEN], _OU_ char device_name[DEVICE_NAME_MAXLEN])
{
	int res = 0, start = 0, end = 0, slice = 0;

	if (uri == NULL || uri_len <= 0 || product_key == NULL || device_name == NULL ||
		(strlen(product_key) >= PRODUCT_KEY_MAXLEN) || (strlen(device_name) >= DEVICE_NAME_MAXLEN)) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	res = iotx_dcm_memtok(uri,uri_len,IOTX_DCS_SERVICE_DELIMITER,start_deli,&start);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	res = iotx_dcm_memtok(uri,uri_len,IOTX_DCS_SERVICE_DELIMITER,start_deli + 1,&slice);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	res = iotx_dcm_memtok(uri,uri_len,IOTX_DCS_SERVICE_DELIMITER,end_deli,&end);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	dm_log_debug("URI Product Key: %.*s, Device Name: %.*s",slice - start - 1,uri + start + 1,end - slice - 1,uri + slice + 1);

	memcpy(product_key,uri + start + 1,slice - start - 1);
	memcpy(device_name,uri + slice + 1,end - slice - 1);

	return SUCCESS_RETURN;
}

int iotx_dmsg_request_parse(_IN_ char *payload, _IN_ int payload_len, _OU_ iotx_dmsg_request_payload_t *request)
{
	int res = 0;
	lite_cjson_t lite;

	if (payload == NULL || payload_len <= 0 || request == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	//Parse Root
	memset(&lite,0,sizeof(lite_cjson_t));
	res = lite_cjson_parse(payload,payload_len,&lite);
	if (res != SUCCESS_RETURN || !lite_cjson_is_object(&lite)) {
		dm_log_err(IOTX_DM_LOG_JSON_PARSE_FAILED,payload_len,payload);
		return FAIL_RETURN;
	}

	//Parse Id
	memset(&request->id,0,sizeof(lite_cjson_t));
	res = lite_cjson_object_item(&lite,IOTX_DMSG_KEY_ID,strlen(IOTX_DMSG_KEY_ID),&request->id);
	if (res != SUCCESS_RETURN || !lite_cjson_is_string(&request->id)) {
		dm_log_err(IOTX_DM_LOG_JSON_PARSE_FAILED,payload_len,payload);
		return FAIL_RETURN;
	}
	dm_log_debug("Current Request Message ID: %.*s",request->id.value_length,request->id.value);

	//Parse Version
	memset(&request->version,0,sizeof(lite_cjson_t));
	res = lite_cjson_object_item(&lite,IOTX_DMSG_KEY_VERSION,strlen(IOTX_DMSG_KEY_VERSION),&request->version);
	if (res != SUCCESS_RETURN || !lite_cjson_is_string(&request->version)) {
		dm_log_err(IOTX_DM_LOG_JSON_PARSE_FAILED,payload_len,payload);
		return FAIL_RETURN;
	}
	dm_log_debug("Current Request Message Version: %.*s",request->version.value_length,request->version.value);


	//Parse Method
	memset(&request->method,0,sizeof(lite_cjson_t));
	res = lite_cjson_object_item(&lite,IOTX_DMSG_KEY_METHOD,strlen(IOTX_DMSG_KEY_METHOD),&request->method);
	if (res != SUCCESS_RETURN || !lite_cjson_is_string(&request->method)) {
		dm_log_err(IOTX_DM_LOG_JSON_PARSE_FAILED,payload_len,payload);
		return FAIL_RETURN;
	}
	dm_log_debug("Current Request Message Method: %.*s",request->method.value_length,request->method.value);

	//Parse Params
	memset(&request->params,0,sizeof(lite_cjson_t));
	res = lite_cjson_object_item(&lite,IOTX_DMSG_KEY_PARAMS,strlen(IOTX_DMSG_KEY_PARAMS),&request->params);
	if (res != SUCCESS_RETURN || !lite_cjson_is_object(&request->params)) {
		dm_log_err(IOTX_DM_LOG_JSON_PARSE_FAILED,payload_len,payload);
		return FAIL_RETURN;
	}
	dm_log_debug("Current Request Message Params: %.*s",request->params.value_length,request->params.value);

	return SUCCESS_RETURN;
}

int iotx_dmsg_response_parse(_IN_ char *payload, _IN_ int payload_len, _OU_ iotx_dmsg_response_payload_t *response)
{
	int res = 0;
	lite_cjson_t lite;

	if (payload == NULL || payload_len <= 0 || response == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	//Parse Root
	memset(&lite,0,sizeof(lite_cjson_t));
	res = lite_cjson_parse(payload,payload_len,&lite);
	if (res != SUCCESS_RETURN || !lite_cjson_is_object(&lite)) {
		dm_log_err(IOTX_DM_LOG_JSON_PARSE_FAILED,payload_len,payload);
		return FAIL_RETURN;
	}

	//Parse Id
	memset(&response->id,0,sizeof(lite_cjson_t));
	res = lite_cjson_object_item(&lite,IOTX_DMSG_KEY_ID,strlen(IOTX_DMSG_KEY_ID),&response->id);
	if (res != SUCCESS_RETURN || !lite_cjson_is_string(&response->id)) {
		dm_log_err(IOTX_DM_LOG_JSON_PARSE_FAILED,payload_len,payload);
		return FAIL_RETURN;
	}
	dm_log_debug("Current Request Message ID: %.*s",response->id.value_length,response->id.value);

	//Parse code
	memset(&response->code,0,sizeof(lite_cjson_t));
	res = lite_cjson_object_item(&lite,IOTX_DMSG_KEY_CODE,strlen(IOTX_DMSG_KEY_CODE),&response->code);
	if (res != SUCCESS_RETURN || !lite_cjson_is_number(&response->code)) {
		dm_log_err(IOTX_DM_LOG_JSON_PARSE_FAILED,payload_len,payload);
		return FAIL_RETURN;
	}
	dm_log_debug("Current Request Message Code: %d",response->code.value_int);

	//Parse data
	memset(&response->data,0,sizeof(lite_cjson_t));
	res = lite_cjson_object_item(&lite,IOTX_DMSG_KEY_DATA,strlen(IOTX_DMSG_KEY_DATA),&response->data);
	if (res != SUCCESS_RETURN) {
		dm_log_err(IOTX_DM_LOG_JSON_PARSE_FAILED,payload_len,payload);
		return FAIL_RETURN;
	}
	dm_log_debug("Current Request Message Data: %.*s",response->data.value_length,response->data.value);

	return SUCCESS_RETURN;
}

const char IOTX_DMSG_REQUEST[] DM_READ_ONLY = "{\"id\":\"%d\",\"version\":\"%s\",\"params\":%s,\"method\":\"%s\"}";
int iotx_dmsg_request(_IN_ iotx_dmsg_request_t *request)
{
	int res = 0, payload_len = 0;
	char *payload = NULL, *uri = NULL;

	if (request == NULL || request->params == NULL || request->method == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	/* Request URI */
	res = iotx_dcm_service_name(request->service_prefix,request->service_name,
								request->product_key,request->device_name,&uri);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	payload_len = strlen(IOTX_DMSG_REQUEST) + 10 + strlen(IOTX_DMSG_VERSION) + strlen(request->params) + strlen(request->method) + 1;
	payload = DM_malloc(payload_len);
	if (payload == NULL) {
		DM_free(uri);
		dm_log_err(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(payload,0,payload_len);
	HAL_Snprintf(payload,payload_len,IOTX_DMSG_REQUEST,request->msgid,
					IOTX_DMSG_VERSION,request->params,request->method);

	res = iotx_dcw_send_to_cloud(uri,payload,NULL);
	if (res != SUCCESS_RETURN) {
		DM_free(uri);DM_free(payload);
		dm_log_err(IOTX_DM_LOG_CM_SEND_MESSAGE_FAILED);
		return FAIL_RETURN;
	}

	DM_free(uri);DM_free(payload);
	return SUCCESS_RETURN;
}

const char IOTX_DMSG_RESPONSE_WITH_DATA[] DM_READ_ONLY = "{\"id\":\"%.*s\",\"code\":%d,\"data\":%.*s}";
int iotx_dmsg_response_with_data(_IN_ iotx_dmsg_request_payload_t *request, _IN_ iotx_dmsg_response_t *response, _IN_ char *data, _IN_ int data_len)
{
	int res = 0, payload_len = 0;
	char *uri = NULL, *payload = NULL;

	if (request == NULL || response == NULL || data == NULL || data_len <= 0) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	/* Response URI */
	res = iotx_dcm_service_name(response->service_prefix,response->service_name,
								response->product_key,response->device_name,&uri);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	/* Response Payload */
	payload_len = strlen(IOTX_DMSG_RESPONSE_WITH_DATA) + request->id.value_length + IOTX_DCM_UINT32_STRLEN + data_len + 1;
	payload = DM_malloc(payload_len);
	if (payload == NULL) {
		DM_free(uri);
		dm_log_err(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(payload,0,payload_len);
	HAL_Snprintf(payload,payload_len,IOTX_DMSG_RESPONSE_WITH_DATA,
					request->id.value_length,request->id.value,response->code,data_len,data);

	dm_log_debug("Send URI: %s, Payload: %s",uri,payload);

	res = iotx_dcw_send_to_cloud(uri,payload,NULL);
	if (res != SUCCESS_RETURN) {
		DM_free(uri);DM_free(payload);
		dm_log_err(IOTX_DM_LOG_CM_SEND_MESSAGE_FAILED);
		return FAIL_RETURN;
	}

	DM_free(uri);DM_free(payload);
	return SUCCESS_RETURN;
}

const char IOTX_DMSG_RESPONSE_WITHOUT_DATA[] DM_READ_ONLY = "{\"id\":\"%.*s\",\"code\":%d,\"data\":{}}";
int iotx_dmsg_response_without_data(_IN_ iotx_dmsg_request_payload_t *request, _IN_ iotx_dmsg_response_t *response)
{
	int res = 0, payload_len = 0;
	char *uri = NULL, *payload = NULL;

	if (request == NULL || response == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	/* Response URI */
	res = iotx_dcm_service_name(response->service_prefix,response->service_name,
								response->product_key,response->device_name,&uri);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	/* Response Payload */
	payload_len = strlen(IOTX_DMSG_RESPONSE_WITHOUT_DATA) + request->id.value_length + IOTX_DCM_UINT32_STRLEN + 1;
	payload = DM_malloc(payload_len);
	if (payload == NULL) {
		DM_free(uri);
		dm_log_err(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(payload,0,payload_len);
	HAL_Snprintf(payload,payload_len,IOTX_DMSG_RESPONSE_WITHOUT_DATA,
					request->id.value_length,request->id.value,response->code);

	res = iotx_dcw_send_to_cloud(uri,payload,NULL);
	if (res != SUCCESS_RETURN) {
		DM_free(uri);DM_free(payload);
		dm_log_err(IOTX_DM_LOG_CM_SEND_MESSAGE_FAILED);
		return FAIL_RETURN;
	}

	DM_free(uri);DM_free(payload);
	return SUCCESS_RETURN;
}

const char IOTX_DMSG_PROPERTY_SET_USER_PAYLOAD[] DM_READ_ONLY = "{\"result\":%d,\"identifier\":\"%s\"}";
static int _iotx_dmsg_property_set_number(int devid, char *key, lite_cjson_t *root)
{
	int res = 0, message_len = 0;
	void *data = NULL;
	iotx_dsw_data_type_e type;
	char *message = NULL;

	dm_log_debug("Current Key: %s",key);

	res = iotx_dmgr_get_property_data(devid,key,strlen(key),&data);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	res = iotx_dmgr_get_data_type(data,&type);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	dm_log_debug("Current Type: %d",type);
	switch (type) {
		case IOTX_DSW_DATA_TYPE_INT:
		case IOTX_DSW_DATA_TYPE_ENUM:
		case IOTX_DSW_DATA_TYPE_BOOL:
			{
				res = iotx_dmgr_set_property_value(devid,key,strlen(key),&root->value_int,0);
			}
			break;
		case IOTX_DSW_DATA_TYPE_FLOAT:
			{
				float value_float = (float)root->value_double;
				res = iotx_dmgr_set_property_value(devid,key,strlen(key),&value_float,0);
			}
			break;
		case IOTX_DSW_DATA_TYPE_DOUBLE:
			{
				res = iotx_dmgr_set_property_value(devid,key,strlen(key),&root->value_double,0);
			}
			break;
		default:
			dm_log_warning("Unkonwn Number Type");
			break;
	}

	/* Send To User */
	message_len = strlen(IOTX_DMSG_PROPERTY_SET_USER_PAYLOAD) + 10 + strlen(key) + 1;
	message = DM_malloc(message_len);
	if (message == NULL) {
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return res;
	}
	memset(message,0,message_len);
	HAL_Snprintf(message,message_len,IOTX_DMSG_PROPERTY_SET_USER_PAYLOAD,res,key);
	dm_log_debug("Send To User: %s",message);
	res = _iotx_dmsg_send_to_user(IOTX_DM_EVENT_PROPERTY_SET,message);
	if (res != SUCCESS_RETURN) {DM_free(message);}

	return res;
}

static int _iotx_dmsg_property_set_string(int devid, char *key, lite_cjson_t *root)
{
	int res = 0, message_len = 0;
	void *data = NULL;
	iotx_dsw_data_type_e type;
	char *message = NULL;

	dm_log_debug("Current Key: %s",key);

	res = iotx_dmgr_get_property_data(devid,key,strlen(key),&data);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	res = iotx_dmgr_get_data_type(data,&type);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	dm_log_debug("Current Type: %d",type);

	switch(type) {
		case IOTX_DSW_DATA_TYPE_TEXT:
		case IOTX_DSW_DATA_TYPE_DATE:
			{
				res = iotx_dmgr_set_property_value(devid,key,strlen(key),root->value,root->value_length);
			}
			break;
		default:
			dm_log_warning("Unkonwn String Type");
			break;
	}

	/* Send To User */
	message_len = strlen(IOTX_DMSG_PROPERTY_SET_USER_PAYLOAD) + 10 + strlen(key) + 1;
	message = DM_malloc(message_len);
	if (message == NULL) {
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return res;
	}
	memset(message,0,message_len);
	HAL_Snprintf(message,message_len,IOTX_DMSG_PROPERTY_SET_USER_PAYLOAD,res,key);
	res = _iotx_dmsg_send_to_user(IOTX_DM_EVENT_PROPERTY_SET,message);
	if (res != SUCCESS_RETURN) {DM_free(message);}

	return res;
}

static int _iotx_dmsg_property_set_object(int devid, char *key, lite_cjson_t *root)
{
	int res = 0, index = 0;
	lite_cjson_t lite_item_key;
	lite_cjson_t lite_item_value;
	char *new_key = NULL;
	int new_key_len = 0;

	for (index = 0;index < root->size;index++) {
		res = lite_cjson_object_item_by_index(root,index,&lite_item_key,&lite_item_value);
		if (res != SUCCESS_RETURN) {continue;}

		dm_log_debug("Current Key: %.*s, Value: %.*s",
						lite_item_key.value_length,lite_item_key.value,
						lite_item_value.value_length,lite_item_value.value);
		//new_key_len = lite_item_key.value_length + 1;
		new_key_len = ((key == NULL)?(0):(strlen(key) + 1)) + lite_item_key.value_length + 1;
		dm_log_debug("new_key_len: %d",new_key_len);
		new_key = DM_malloc(new_key_len);
		if (new_key == NULL) {
			dm_log_err(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
			return FAIL_RETURN;
		}
		memset(new_key,0,new_key_len);
		if (key) {memcpy(new_key,key,strlen(key));new_key[strlen(new_key)] = IOTX_DSW_KEY_DELIMITER;}
		memcpy(new_key + strlen(new_key),lite_item_key.value,lite_item_key.value_length);
		dm_log_debug("New Key: %s",new_key);

		if (lite_cjson_is_object(&lite_item_value)) {res = _iotx_dmsg_property_set_object(devid,new_key,&lite_item_value);}
		if (lite_cjson_is_array(&lite_item_value)) {res = _iotx_dmsg_property_set_array(devid,new_key,&lite_item_value);}
		if (lite_cjson_is_number(&lite_item_value)) {res = _iotx_dmsg_property_set_number(devid,new_key,&lite_item_value);}
		if (lite_cjson_is_string(&lite_item_value)) {res = _iotx_dmsg_property_set_string(devid,new_key,&lite_item_value);}

		DM_free(new_key);
		if (res != SUCCESS_RETURN) {return FAIL_RETURN;}
	}
	return SUCCESS_RETURN;
}

static int _iotx_dmsg_property_set_array(int devid, char *key, lite_cjson_t *root)
{
	int res = 0, index = 0;
	lite_cjson_t lite_item_value;
	char *ascii_index = NULL;
	char *new_key = NULL;
	int new_key_len = 0;

	for (index = 0;index < root->size;index++) {

		res = lite_cjson_array_item(root,index,&lite_item_value);
		if (res != SUCCESS_RETURN) {continue;}

		dm_log_debug("Current Value: %.*s",lite_item_value.value_length,lite_item_value.value);

		res = iotx_dcm_itoa(index,&ascii_index);
		if (res != SUCCESS_RETURN) {continue;}

		/*                         Original Key              '['         Index         ']'*/
		new_key_len = ((key == NULL)?(0):(strlen(key) + 1)) + 1 + strlen(ascii_index) + 1 + 1;
		new_key = DM_malloc(new_key_len);
		if (new_key == NULL) {
			DM_free(ascii_index);
			dm_log_err(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
			return FAIL_RETURN;
		}
		memset(new_key,0,new_key_len);
		if (key) {memcpy(new_key,key,strlen(key));}
		new_key[strlen(new_key)] = '[';
		memcpy(new_key + strlen(new_key),ascii_index,strlen(ascii_index));
		new_key[strlen(new_key)] = ']';
		dm_log_debug("New Key: %s",new_key);
		DM_free(ascii_index);

		if (lite_cjson_is_object(&lite_item_value)) {res = _iotx_dmsg_property_set_object(devid,new_key,&lite_item_value);}
		if (lite_cjson_is_array(&lite_item_value)) {res = _iotx_dmsg_property_set_array(devid,new_key,&lite_item_value);}
		if (lite_cjson_is_number(&lite_item_value)) {res = _iotx_dmsg_property_set_number(devid,new_key,&lite_item_value);}
		if (lite_cjson_is_string(&lite_item_value)) {res = _iotx_dmsg_property_set_string(devid,new_key,&lite_item_value);}

		DM_free(new_key);
		if (res != SUCCESS_RETURN) {return FAIL_RETURN;}
	}

	return SUCCESS_RETURN;
}

#if defined (CONFIG_DM_DEVTYPE_SINGLE)
const char IOTX_DMSG_PROPERTY_SET_FMT[] DM_READ_ONLY = "{\"devid\":%d,\"propertyid\":\"%.*s\"}";
#elif defined (CONFIG_DM_DEVTYPE_GATEWAY)
const char IOTX_DMSG_PROPERTY_SET_FMT[] DM_READ_ONLY = "{\"devid\":%d,\"payload\":\"%.*s\"}";
#endif
int iotx_dmsg_property_set(int devid,iotx_dmsg_request_payload_t *request)
{
	int res = 0, message_len = 0;
	char *message = NULL;
	lite_cjson_t lite;

	if (request == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	/* Parse Root */
	memset(&lite,0,sizeof(lite_cjson_t));
	res = lite_cjson_parse(request->params.value,request->params.value_length,&lite);
	if (res != SUCCESS_RETURN || (!lite_cjson_is_object(&lite) && !lite_cjson_is_array(&lite))) {
		dm_log_err(IOTX_DM_LOG_JSON_PARSE_FAILED,request->params.value_length,request->params.value);
		return FAIL_RETURN;
	}
	dm_log_info("Property Set, Size: %d",lite.size);

	if (lite_cjson_is_object(&lite)) {res = _iotx_dmsg_property_set_object(devid,NULL,&lite);}

	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

#if defined (CONFIG_DM_DEVTYPE_SINGLE)
	int index = 0;
	lite_cjson_t lite_item_key, lite_item_value;
	for (index = 0;index < lite.size;index++) {
		memset(&lite_item_key,0,sizeof(lite_cjson_t));
		memset(&lite_item_value,0,sizeof(lite_cjson_t));

		res = lite_cjson_object_item_by_index(&lite,index,&lite_item_key,&lite_item_value);
		if (res != SUCCESS_RETURN) {continue;}

		message_len = strlen(IOTX_DMSG_PROPERTY_SET_FMT) + IOTX_DCM_UINT32_STRLEN + lite_item_key.value_length + 1;
		message = DM_malloc(message_len);
		if (message == NULL) {
			dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
			return FAIL_RETURN;
		}
		memset(message,0,message_len);
		HAL_Snprintf(message,message_len,IOTX_DMSG_PROPERTY_SET_FMT,devid,lite_item_key.value_length,lite_item_key.value);

		res = _iotx_dmsg_send_to_user(IOTX_DM_EVENT_PROPERTY_SET,message);
		if (res != SUCCESS_RETURN) {
			DM_free(message);
		}
	}
#elif defined (CONFIG_DM_DEVTYPE_GATEWAY)
	message_len = strlen(IOTX_DMSG_PROPERTY_SET_FMT) + IOTX_DCM_UINT32_STRLEN + request->params.value_length + 1;
	message = DM_malloc(message_len);
	if (message == NULL) {
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(message,0,message_len);
	HAL_Snprintf(message,message_len,IOTX_DMSG_PROPERTY_SET_FMT,devid,request->params.value_length,request->params.value);

	res = _iotx_dmsg_send_to_user(IOTX_DM_EVENT_PROPERTY_SET,message);
	if (res != SUCCESS_RETURN) {
		DM_free(message);
	}
#endif

	return SUCCESS_RETURN;
}

const char IOTX_DMSG_TOPO_ADD_NOTIFY_USER_PAYLOAD[] DM_READ_ONLY = "{\"result\":%d,\"devid\":%d,\"product_key\":\"%s\",\"device_name\":\"%s\"}";
int iotx_dmsg_topo_add_notify(_IN_ char *payload, _IN_ int payload_len)
{
	int ret = SUCCESS_RETURN, res = 0, index = 0, devid = 0, message_len = 0;
	lite_cjson_t lite, lite_item, lite_item_pk, lite_item_dn;
	char product_key[PRODUCT_KEY_MAXLEN] = {0};
	char device_name[DEVICE_NAME_MAXLEN] = {0};
	char *message = NULL;

	memset(&lite,0,sizeof(lite_cjson_t));
	res = lite_cjson_parse(payload,payload_len,&lite);
	if (res != SUCCESS_RETURN || !lite_cjson_is_array(&lite)) {
		dm_log_err(IOTX_DM_LOG_JSON_PARSE_FAILED,payload_len,payload);
		return FAIL_RETURN;
	}

	for (index = 0;index < lite.size;index++) {
		devid = 0;message_len = 0;message = NULL;
		memset(&lite_item,0,sizeof(lite_cjson_t));
		memset(&lite_item_pk,0,sizeof(lite_cjson_t));
		memset(&lite_item_dn,0,sizeof(lite_cjson_t));
		memset(product_key,0,PRODUCT_KEY_MAXLEN);
		memset(device_name,0,DEVICE_NAME_MAXLEN);

		res = lite_cjson_array_item(&lite,index,&lite_item);
		if (res != SUCCESS_RETURN) {ret = FAIL_RETURN;continue;}

		res = lite_cjson_object_item(&lite_item,IOTX_DMSG_KEY_PRODUCT_KEY,strlen(IOTX_DMSG_KEY_PRODUCT_KEY),&lite_item_pk);
		if (res != SUCCESS_RETURN) {ret = FAIL_RETURN;continue;}

		res = lite_cjson_object_item(&lite_item,IOTX_DMSG_KEY_DEVICE_NAME,strlen(IOTX_DMSG_KEY_DEVICE_NAME),&lite_item_dn);
		if (res != SUCCESS_RETURN) {ret = FAIL_RETURN;continue;}

		dm_log_debug("Current Product Key: %.*s, Device Name: %.*s",
						lite_item_pk.value_length,lite_item_pk.value,
						lite_item_dn.value_length,lite_item_dn.value);

		if (lite_item_pk.value_length >= PRODUCT_KEY_MAXLEN ||
			lite_item_dn.value_length >= DEVICE_NAME_MAXLEN) {
			ret = FAIL_RETURN;
			continue;
		}
		memcpy(product_key,lite_item_pk.value,lite_item_pk.value_length);
		memcpy(device_name,lite_item_dn.value,lite_item_dn.value_length);

		res = iotx_dmgr_device_create(IOTX_DM_DEVICE_SUBDEV,product_key,device_name,&devid);
		if (res != SUCCESS_RETURN) {ret = FAIL_RETURN;}

		/* Send To User */
		message_len = strlen(IOTX_DMSG_TOPO_ADD_NOTIFY_USER_PAYLOAD) + 20 +
							strlen(product_key) + strlen(device_name) + 1;
		message = DM_malloc(message_len);
		if (message == NULL) {
			dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
			ret = FAIL_RETURN;
			continue;
		}
		memset(message,0,message_len);
		HAL_Snprintf(message,message_len,IOTX_DMSG_TOPO_ADD_NOTIFY_USER_PAYLOAD,res,devid,product_key,device_name);
		res = _iotx_dmsg_send_to_user(IOTX_DM_EVENT_TOPO_ADD_NOTIFY,message);
		if (res != SUCCESS_RETURN) {ret = FAIL_RETURN;DM_free(message);}

	}

	return ret;
}

const char IOTX_DMSG_SERVICE_SET_USER_PAYLOAD[] DM_READ_ONLY = "{\"result\":%d,\"identifier\":\"%s\"}";
static int _iotx_dmsg_service_set_number(int devid, char *key, lite_cjson_t *root)
{
	int res = 0;
	void *data = NULL;
	iotx_dsw_data_type_e type;

	dm_log_debug("Current Key: %s",key);

	res = iotx_dmgr_get_service_input_data(devid,key,strlen(key),&data);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	res = iotx_dmgr_get_data_type(data,&type);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	dm_log_debug("Current Type: %d",type);
	switch (type) {
		case IOTX_DSW_DATA_TYPE_INT:
		case IOTX_DSW_DATA_TYPE_ENUM:
		case IOTX_DSW_DATA_TYPE_BOOL:
			{
				res = iotx_dmgr_set_service_input_value(devid,key,strlen(key),&root->value_int,0);
			}
			break;
		case IOTX_DSW_DATA_TYPE_FLOAT:
			{
				float value_float = (float)root->value_double;
				res = iotx_dmgr_set_service_input_value(devid,key,strlen(key),&value_float,0);
			}
			break;
		case IOTX_DSW_DATA_TYPE_DOUBLE:
			{
				res = iotx_dmgr_set_service_input_value(devid,key,strlen(key),&root->value_double,0);
			}
			break;
		default:
			dm_log_warning("Unkonwn Number Type");
			break;
	}

	/* Send To User */
	#if 0
	int message_len = 0;
	char *message = NULL;
	message_len = strlen(IOTX_DMSG_SERVICE_SET_USER_PAYLOAD) + 10 + strlen(key) + 1;
	message = DM_malloc(message_len);
	if (message == NULL) {
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return res;
	}
	memset(message,0,message_len);
	HAL_Snprintf(message,message_len,IOTX_DMSG_SERVICE_SET_USER_PAYLOAD,res,key);
	dm_log_debug("Send To User: %s",message);
	res = _iotx_dmsg_send_to_user(IOTX_DM_EVENT_THING_SERVICE_REQUEST,message);
	if (res != SUCCESS_RETURN) {DM_free(message);}
	#endif

	return res;
}

static int _iotx_dmsg_service_set_string(int devid, char *key, lite_cjson_t *root)
{
	int res = 0;
	void *data = NULL;
	iotx_dsw_data_type_e type;

	dm_log_debug("Current Key: %s",key);

	res = iotx_dmgr_get_service_input_data(devid,key,strlen(key),&data);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	res = iotx_dmgr_get_data_type(data,&type);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	dm_log_debug("Current Type: %d",type);

	switch(type) {
		case IOTX_DSW_DATA_TYPE_TEXT:
		case IOTX_DSW_DATA_TYPE_DATE:
			{
				res = iotx_dmgr_set_service_input_value(devid,key,strlen(key),root->value,root->value_length);
			}
			break;
		default:
			dm_log_warning("Unkonwn String Type");
			break;
	}

	/* Send To User */
	#if 0
	int message_len = 0;
	char *message = NULL;
	message_len = strlen(IOTX_DMSG_SERVICE_SET_USER_PAYLOAD) + 10 + strlen(key) + 1;
	message = DM_malloc(message_len);
	if (message == NULL) {
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return res;
	}
	memset(message,0,message_len);
	HAL_Snprintf(message,message_len,IOTX_DMSG_SERVICE_SET_USER_PAYLOAD,res,key);
	res = _iotx_dmsg_send_to_user(IOTX_DM_EVENT_THING_SERVICE_REQUEST,message);
	if (res != SUCCESS_RETURN) {DM_free(message);}
	#endif

	return res;
}

static int _iotx_dmsg_service_set_object(int devid, char *key, lite_cjson_t *root)
{
	int res = 0, index = 0;
	lite_cjson_t lite_item_key;
	lite_cjson_t lite_item_value;
	char *new_key = NULL;
	int new_key_len = 0;

	for (index = 0;index < root->size;index++) {
		res = lite_cjson_object_item_by_index(root,index,&lite_item_key,&lite_item_value);
		if (res != SUCCESS_RETURN) {continue;}

		dm_log_debug("Current Key: %.*s, Value: %.*s",
						lite_item_key.value_length,lite_item_key.value,
						lite_item_value.value_length,lite_item_value.value);
		//new_key_len = lite_item_key.value_length + 1;
		new_key_len = ((key == NULL)?(0):(strlen(key) + 1)) + lite_item_key.value_length + 1;
		dm_log_debug("new_key_len: %d",new_key_len);
		new_key = DM_malloc(new_key_len);
		if (new_key == NULL) {
			dm_log_err(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
			return FAIL_RETURN;
		}
		memset(new_key,0,new_key_len);
		if (key) {memcpy(new_key,key,strlen(key));new_key[strlen(new_key)] = IOTX_DSW_KEY_DELIMITER;}
		memcpy(new_key + strlen(new_key),lite_item_key.value,lite_item_key.value_length);
		dm_log_debug("New Key: %s",new_key);

		if (lite_cjson_is_object(&lite_item_value)) {res = _iotx_dmsg_service_set_object(devid,new_key,&lite_item_value);}
		if (lite_cjson_is_array(&lite_item_value)) {res = _iotx_dmsg_service_set_array(devid,new_key,&lite_item_value);}
		if (lite_cjson_is_number(&lite_item_value)) {res = _iotx_dmsg_service_set_number(devid,new_key,&lite_item_value);}
		if (lite_cjson_is_string(&lite_item_value)) {res = _iotx_dmsg_service_set_string(devid,new_key,&lite_item_value);}

		DM_free(new_key);
		if (res != SUCCESS_RETURN) {return FAIL_RETURN;}
	}
	return SUCCESS_RETURN;
}

static int _iotx_dmsg_service_set_array(int devid, char *key, lite_cjson_t *root)
{
	int res = 0, index = 0;
	lite_cjson_t lite_item_value;
	char *ascii_index = NULL;
	char *new_key = NULL;
	int new_key_len = 0;

	for (index = 0;index < root->size;index++) {

		res = lite_cjson_array_item(root,index,&lite_item_value);
		if (res != SUCCESS_RETURN) {continue;}

		dm_log_debug("Current Value: %.*s",lite_item_value.value_length,lite_item_value.value);

		res = iotx_dcm_itoa(index,&ascii_index);
		if (res != SUCCESS_RETURN) {continue;}

		/*                         Original Key              '['         Index         ']'*/
		new_key_len = ((key == NULL)?(0):(strlen(key) + 1)) + 1 + strlen(ascii_index) + 1 + 1;
		new_key = DM_malloc(new_key_len);
		if (new_key == NULL) {
			DM_free(ascii_index);
			dm_log_err(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
			return FAIL_RETURN;
		}
		memset(new_key,0,new_key_len);
		if (key) {memcpy(new_key,key,strlen(key));}
		new_key[strlen(new_key)] = '[';
		memcpy(new_key + strlen(new_key),ascii_index,strlen(ascii_index));
		new_key[strlen(new_key)] = ']';
		dm_log_debug("New Key: %s",new_key);
		DM_free(ascii_index);

		if (lite_cjson_is_object(&lite_item_value)) {res = _iotx_dmsg_service_set_object(devid,new_key,&lite_item_value);}
		if (lite_cjson_is_array(&lite_item_value)) {res = _iotx_dmsg_service_set_array(devid,new_key,&lite_item_value);}
		if (lite_cjson_is_number(&lite_item_value)) {res = _iotx_dmsg_service_set_number(devid,new_key,&lite_item_value);}
		if (lite_cjson_is_string(&lite_item_value)) {res = _iotx_dmsg_service_set_string(devid,new_key,&lite_item_value);}

		DM_free(new_key);
		if (res != SUCCESS_RETURN) {return FAIL_RETURN;}
	}

	return SUCCESS_RETURN;
}

#if defined (CONFIG_DM_DEVTYPE_SINGLE)
const char IOTX_DMSG_SERVICE_REQUEST_FMT[] DM_READ_ONLY = "{\"id\":%d,\"devid\":%d,\"serviceid\":\"%.*s\"}";
#elif defined (CONFIG_DM_DEVTYPE_GATEWAY)
const char IOTX_DMSG_SERVICE_REQUEST_FMT[] DM_READ_ONLY = "{\"id\":%d,\"devid\":%d,\"serviceid\":\"%.*s\",\"payload\":%.*s}";
#endif
int iotx_dmsg_thing_service_request(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN], char *identifier, int identifier_len, iotx_dmsg_request_payload_t *request)
{
	int res = 0, id = 0, devid = 0, message_len = 0;
	lite_cjson_t lite;
	char *key = NULL, *message = NULL;;
	char int_id[IOTX_DCM_UINT32_STRLEN] = {0};

	/* Message ID */
	memcpy(int_id,request->id.value,request->id.value_length);
	id = atoi(int_id);

	dm_log_debug("Current ID: %d",id);

	if (product_key == NULL || device_name == NULL ||
		(strlen(product_key) >= PRODUCT_KEY_MAXLEN) ||
		(strlen(device_name) >= DEVICE_NAME_MAXLEN) ||
		identifier == NULL || identifier_len == 0 || request == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	res = iotx_dmgr_search_device_by_pkdn(product_key,device_name,&devid);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	key = DM_malloc(identifier_len + 1);
	if (key == NULL) {
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(key,0,identifier_len + 1);
	memcpy(key,identifier,identifier_len);

	/* Parse Root */
	memset(&lite,0,sizeof(lite_cjson_t));
	res = lite_cjson_parse(request->params.value,request->params.value_length,&lite);
	if (res != SUCCESS_RETURN || (!lite_cjson_is_object(&lite) && !lite_cjson_is_array(&lite))) {
		dm_log_err(IOTX_DM_LOG_JSON_PARSE_FAILED,request->params.value_length,request->params.value);
		return FAIL_RETURN;
	}
	dm_log_info("Service Request, Size: %d",lite.size);

	if (lite_cjson_is_object(&lite)) {res = _iotx_dmsg_service_set_object(devid,key,&lite);DM_free(key)}

	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

#if defined (CONFIG_DM_DEVTYPE_SINGLE)
	message_len = strlen(IOTX_DMSG_SERVICE_REQUEST_FMT) + IOTX_DCM_UINT32_STRLEN*2 + identifier_len + 1;
	message = DM_malloc(message_len);
	if (message == NULL) {
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(message,0,message_len);
	HAL_Snprintf(message,message_len,IOTX_DMSG_SERVICE_REQUEST_FMT,id,devid,identifier_len,identifier);
#elif defined (CONFIG_DM_DEVTYPE_GATEWAY)
	message_len = strlen(IOTX_DMSG_SERVICE_REQUEST_FMT) + IOTX_DCM_UINT32_STRLEN*2 + identifier_len + request->params.value_length + 1;
	message = DM_malloc(message_len);
	if (message == NULL) {
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(message,0,message_len);
	HAL_Snprintf(message,message_len,IOTX_DMSG_SERVICE_REQUEST_FMT,id,devid,identifier_len,identifier,request->params.value_length,request->params.value);
#endif
	res = _iotx_dmsg_send_to_user(IOTX_DM_EVENT_THING_SERVICE_REQUEST,message);
	if (res != SUCCESS_RETURN) {
		DM_free(message);
		return FAIL_RETURN;
	}

	return SUCCESS_RETURN;
}

const char IOTX_DMSG_EVENT_THING_DISABLE_FMT[] DM_READ_ONLY = "{\"devid\":%d}";
int iotx_dmsg_thing_disable(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN])
{
	int res = 0, devid = 0, message_len = 0;
	char *message = NULL;

	if (product_key == NULL || device_name == NULL ||
		(strlen(product_key) >= PRODUCT_KEY_MAXLEN) ||
		(strlen(device_name) >= DEVICE_NAME_MAXLEN)) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	res = iotx_dmgr_search_device_by_pkdn(product_key,device_name,&devid);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	res = iotx_dmgr_set_dev_disable(devid);

	message_len = strlen(IOTX_DMSG_EVENT_THING_DISABLE_FMT) + IOTX_DCM_UINT32_STRLEN + 1;
	message = DM_malloc(message_len);
	if (message == NULL) {
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(message,0,message_len);
	HAL_Snprintf(message,message_len,IOTX_DMSG_EVENT_THING_DISABLE_FMT,devid);

	res = _iotx_dmsg_send_to_user(IOTX_DM_EVENT_THING_DISABLE,message);
	if (res != SUCCESS_RETURN) {
		DM_free(message);
		return FAIL_RETURN;
	}

	return SUCCESS_RETURN;
}

const char IOTX_DMSG_EVENT_THING_ENABLE_FMT[] DM_READ_ONLY = "{\"devid\":%d}";
int iotx_dmsg_thing_enable(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN])
{
	int res = 0, devid = 0, message_len = 0;
	char *message = NULL;

	if (product_key == NULL || device_name == NULL ||
		(strlen(product_key) >= PRODUCT_KEY_MAXLEN) ||
		(strlen(device_name) >= DEVICE_NAME_MAXLEN)) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	res = iotx_dmgr_search_device_by_pkdn(product_key,device_name,&devid);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	res = iotx_dmgr_set_dev_enable(devid);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	message_len = strlen(IOTX_DMSG_EVENT_THING_ENABLE_FMT) + IOTX_DCM_UINT32_STRLEN + 1;
	message = DM_malloc(message_len);
	if (message == NULL) {
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(message,0,message_len);
	HAL_Snprintf(message,message_len,IOTX_DMSG_EVENT_THING_ENABLE_FMT,devid);

	res = _iotx_dmsg_send_to_user(IOTX_DM_EVENT_THING_ENABLE,message);
	if (res != SUCCESS_RETURN) {
		DM_free(message);
		return FAIL_RETURN;
	}

	return SUCCESS_RETURN;
}

const char IOTX_DMSG_EVENT_THING_DELETE_FMT[] DM_READ_ONLY = "{\"res\":%d,\"productKey\":\"%s\",\"deviceName\":\"%s\",\"devid\":%d}";
int iotx_dmsg_thing_delete(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN])
{
	int res = 0, message_len = 0, devid = 0;
	char *message = NULL;

	if (product_key == NULL || device_name == NULL ||
		(strlen(product_key) >= PRODUCT_KEY_MAXLEN) ||
		(strlen(device_name) >= DEVICE_NAME_MAXLEN)) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	res = iotx_dmgr_search_device_by_pkdn(product_key,device_name,&devid);
	if (res == SUCCESS_RETURN) {return FAIL_RETURN;}

	res = iotx_dmgr_device_destroy(devid);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	message_len = strlen(IOTX_DMSG_EVENT_THING_DELETE_FMT) + strlen(product_key) + strlen(device_name) + 1;
	message = DM_malloc(message_len);
	if (message == NULL) {
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(message,0,message_len);
	HAL_Snprintf(message,message_len,IOTX_DMSG_EVENT_THING_DELETE_FMT,res,product_key,device_name,devid);

	res = _iotx_dmsg_send_to_user(IOTX_DM_EVENT_THING_DELETE,message);
	if (res != SUCCESS_RETURN) {
		DM_free(message);
		return FAIL_RETURN;
	}

	return SUCCESS_RETURN;
}

const char IOTX_DMSG_THING_MODEL_DOWN_FMT[] DM_READ_ONLY = "{\"devid\":%d,\"payload\":\"%.*s\"}";
int iotx_dmsg_thing_model_down_raw(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN], _IN_ char *payload, _IN_ int payload_len)
{
	int res = 0, devid = 0, message_len = 0;
	char *message = NULL;

	if (product_key == NULL || device_name == NULL ||
		(strlen(product_key) >= PRODUCT_KEY_MAXLEN) ||
		(strlen(device_name) >= DEVICE_NAME_MAXLEN) ||
		payload == NULL || payload_len <= 0) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	res = iotx_dmgr_search_device_by_pkdn(product_key,device_name,&devid);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	message_len = strlen(IOTX_DMSG_THING_MODEL_DOWN_FMT) + IOTX_DCM_UINT32_STRLEN + payload_len + 1;
	message = DM_malloc(message_len);
	if (message == NULL) {
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(message,0,message_len);
	HAL_Snprintf(message,message_len,IOTX_DMSG_THING_MODEL_DOWN_FMT,devid,payload_len,payload);

	res = _iotx_dmsg_send_to_user(IOTX_DM_EVENT_MODEL_DOWN_RAW,message);
	if (res != SUCCESS_RETURN) {
		DM_free(message);
		return FAIL_RETURN;
	}

	return SUCCESS_RETURN;
}

int iotx_dmsg_thing_gateway_permit(_IN_ char *payload, _IN_ int payload_len)
{
	int res = 0, message_len = 0;
	char *message = NULL;
	lite_cjson_t lite;

	if (payload == NULL || payload_len <= 0) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	res = lite_cjson_parse(payload,payload_len,&lite);
	if (res != SUCCESS_RETURN || !lite_cjson_is_object(&lite)) {
		dm_log_err(IOTX_DM_LOG_JSON_PARSE_FAILED,payload_len,payload);
		return FAIL_RETURN;
	}

	message_len = payload_len + 1;
	message = DM_malloc(message_len);
	if (message == NULL) {
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memcpy(message,payload,payload_len);

	res = _iotx_dmsg_send_to_user(IOTX_DM_EVENT_GATEWAY_PERMIT,message);
	if (res != SUCCESS_RETURN) {
		DM_free(message);
		return FAIL_RETURN;
	}

	return SUCCESS_RETURN;
}

const char IOTX_DMSG_EVENT_SUBDEV_REGISTER_REPLY_FMT[] DM_READ_ONLY = "{\"id\":%d,\"code\":%d,\"devid\":%d}";
int iotx_dmsg_thing_sub_register_reply(iotx_dmsg_response_payload_t *response)
{
	int res = 0, index = 0, message_len = 0, devid = 0;
	lite_cjson_t lite, lite_item, lite_item_pk, lite_item_dn, lite_item_ds;
	char *message = NULL;
	char product_key[PRODUCT_KEY_MAXLEN] = {0};
	char device_name[DEVICE_NAME_MAXLEN] = {0};
	char device_secret[DEVICE_SECRET_MAXLEN] = {0};
	char temp_id[IOTX_DCM_UINT32_STRLEN] = {0};

	if (response == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	res = lite_cjson_parse(response->data.value,response->data.value_length,&lite);
	if (res != SUCCESS_RETURN || !lite_cjson_is_array(&lite)) {
		dm_log_err(IOTX_DM_LOG_JSON_PARSE_FAILED,response->data.value_length,response->data.value);
		return FAIL_RETURN;
	}

	for (index = 0;index < lite.size;index++) {
		devid = 0;message_len = 0;message = NULL;
		memset(temp_id,0,IOTX_DCM_UINT32_STRLEN);
		memset(product_key,0,PRODUCT_KEY_MAXLEN);
		memset(device_name,0,DEVICE_NAME_MAXLEN);
		memset(&lite_item,0,sizeof(lite_cjson_t));
		memset(&lite_item_pk,0,sizeof(lite_cjson_t));
		memset(&lite_item_dn,0,sizeof(lite_cjson_t));
		memset(&lite_item_ds,0,sizeof(lite_cjson_t));

		dm_log_debug("Current Index: %d",index);
		/* Item */
		res = lite_cjson_array_item(&lite,index,&lite_item);
		if (res != SUCCESS_RETURN || !lite_cjson_is_object(&lite_item)) {continue;}

		/* Product Key */
		res = lite_cjson_object_item(&lite_item,IOTX_DMSG_KEY_PRODUCT_KEY,strlen(IOTX_DMSG_KEY_PRODUCT_KEY),&lite_item_pk);
		if (res != SUCCESS_RETURN || !lite_cjson_is_string(&lite_item_pk)) {continue;}
		dm_log_debug("Current Product Key: %.*s",lite_item_pk.value_length,lite_item_pk.value);

		/* Device Name */
		res = lite_cjson_object_item(&lite_item,IOTX_DMSG_KEY_DEVICE_NAME,strlen(IOTX_DMSG_KEY_DEVICE_NAME),&lite_item_dn);
		if (res != SUCCESS_RETURN || !lite_cjson_is_string(&lite_item_dn)) {continue;}
		dm_log_debug("Current Device Name: %.*s",lite_item_dn.value_length,lite_item_dn.value);

		/* Device Name */
		res = lite_cjson_object_item(&lite_item,IOTX_DMSG_KEY_DEVICE_SECRET,strlen(IOTX_DMSG_KEY_DEVICE_SECRET),&lite_item_ds);
		if (res != SUCCESS_RETURN || !lite_cjson_is_string(&lite_item_ds)) {continue;}
		dm_log_debug("Current Device Secret: %.*s",lite_item_ds.value_length,lite_item_ds.value);

		/* Get Device ID */
		memcpy(product_key,lite_item_pk.value,lite_item_pk.value_length);
		memcpy(device_name,lite_item_dn.value,lite_item_dn.value_length);
		memcpy(device_secret,lite_item_ds.value,lite_item_ds.value_length);
		res = iotx_dmgr_search_device_by_pkdn(product_key,device_name,&devid);
		if (res != SUCCESS_RETURN) {continue;}

		/* Update State Machine */
		if (response->code.value_int == IOTX_DM_ERR_CODE_SUCCESS)
			iotx_dmgr_set_dev_status(devid,IOTX_DMGR_DEV_STATUS_REGISTERED);

		/* Set Device Secret */
		res = iotx_dmgr_set_device_secret(devid,device_secret);
		if (res != SUCCESS_RETURN) {continue;}

		/* Send Message To User */
		memcpy(temp_id,response->id.value,response->id.value_length);
 		message_len = strlen(IOTX_DMSG_EVENT_SUBDEV_REGISTER_REPLY_FMT) + IOTX_DCM_UINT32_STRLEN*2 + 1;
		message = DM_malloc(message_len);
		if (message == NULL) {
			dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
			continue;
		}
		memset(message,0,message_len);
		HAL_Snprintf(message,message_len,IOTX_DMSG_EVENT_SUBDEV_REGISTER_REPLY_FMT,atoi(temp_id),response->code.value_int,devid);

		res = _iotx_dmsg_send_to_user(IOTX_DM_EVENT_SUBDEV_REGISTER_REPLY,message);
		if (res != SUCCESS_RETURN) {DM_free(message);}
	}

	return SUCCESS_RETURN;
}

const char IOTX_DMSG_EVENT_SUBDEV_UNREGISTER_REPLY_FMT[] DM_READ_ONLY = "{\"id\":%d,\"code\":%d,\"devid\":%d}";
int iotx_dmsg_thing_sub_unregister_reply(iotx_dmsg_response_payload_t *response)
{
	int res = 0, id, message_len = 0;
	char int_id[IOTX_DCM_UINT32_STRLEN] = {0};
	char *message = NULL;
	iotx_dmc_node_t *node = NULL;

	if (response == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}
	
	memcpy(int_id,response->id.value,response->id.value_length);
	id = atoi(int_id);

	dm_log_debug("Current ID: %d",id);

	res = iotx_dmc_msg_search(id,&node);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	message_len = strlen(IOTX_DMSG_EVENT_SUBDEV_UNREGISTER_REPLY_FMT) + IOTX_DCM_UINT32_STRLEN*3 + 1;
	message = DM_malloc(message_len);
	if (message == NULL) {
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(message,0,message_len);
	HAL_Snprintf(message,message_len,IOTX_DMSG_EVENT_SUBDEV_UNREGISTER_REPLY_FMT,id,response->code.value_int,node->devid);

	res = _iotx_dmsg_send_to_user(IOTX_DM_EVENT_SUBDEV_UNREGISTER_REPLY,message);
	if (res != SUCCESS_RETURN) {DM_free(message);}

	return SUCCESS_RETURN;
}

const char IOTX_DMSG_EVENT_THING_TOPO_ADD_REPLY_FMT[] DM_READ_ONLY = "{\"id\":%d,\"code\":%d,\"devid\":%d}";
int iotx_dmsg_thing_topo_add_reply(iotx_dmsg_response_payload_t *response)
{
	int res = 0, id = 0, message_len = 0;
	char int_id[IOTX_DCM_UINT32_STRLEN] = {0};
	iotx_dmc_node_t *node = NULL;
	char *message = NULL;

	memcpy(int_id,response->id.value,response->id.value_length);
	id = atoi(int_id);

	dm_log_debug("Current ID: %d",id);

	res = iotx_dmc_msg_search(id,&node);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	/* Update State Machine */
	if (response->code.value_int == IOTX_DM_ERR_CODE_SUCCESS)
		iotx_dmgr_set_dev_status(node->devid,IOTX_DMGR_DEV_STATUS_ATTACHED);

	message_len = strlen(IOTX_DMSG_EVENT_THING_TOPO_ADD_REPLY_FMT) + IOTX_DCM_UINT32_STRLEN*3 + 1;
	message = DM_malloc(message_len);
	if (message == NULL) {
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(message,0,message_len);
	HAL_Snprintf(message,message_len,IOTX_DMSG_EVENT_THING_TOPO_ADD_REPLY_FMT,id,response->code.value_int,node->devid);

	res = _iotx_dmsg_send_to_user(IOTX_DM_EVENT_TOPO_ADD_REPLY,message);
	if (res != SUCCESS_RETURN) {DM_free(message);return FAIL_RETURN;}

	return SUCCESS_RETURN;
}

const char IOTX_DMSG_EVENT_THING_TOPO_DELETE_REPLY_FMT[] DM_READ_ONLY = "{\"id\":%d,\"code\":%d,\"devid\":%d}";
int iotx_dmsg_thing_topo_delete_reply(iotx_dmsg_response_payload_t *response)
{
	int res = 0, id = 0, message_len = 0;
	char int_id[IOTX_DCM_UINT32_STRLEN] = {0};
	iotx_dmc_node_t *node = NULL;
	char *message = NULL;

	memcpy(int_id,response->id.value,response->id.value_length);
	id = atoi(int_id);

	dm_log_debug("Current ID: %d",id);

	res = iotx_dmc_msg_search(id,&node);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	/* Update State Machine */
	if (response->code.value_int == IOTX_DM_ERR_CODE_SUCCESS)
		iotx_dmgr_set_dev_status(node->devid,IOTX_DMGR_DEV_STATUS_ATTACHED);

	message_len = strlen(IOTX_DMSG_EVENT_THING_TOPO_DELETE_REPLY_FMT) + IOTX_DCM_UINT32_STRLEN*3 + 1;
	message = DM_malloc(message_len);
	if (message == NULL) {
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(message,0,message_len);
	HAL_Snprintf(message,message_len,IOTX_DMSG_EVENT_THING_TOPO_DELETE_REPLY_FMT,id,response->code.value_int,node->devid);

	res = _iotx_dmsg_send_to_user(IOTX_DM_EVENT_TOPO_DELETE_REPLY,message);
	if (res != SUCCESS_RETURN) {DM_free(message);return FAIL_RETURN;}

	return SUCCESS_RETURN;
}

const char IOTX_DMSG_TOPO_GET_REPLY_FMT[] DM_READ_ONLY = "{\"id\":%d,\"code\":%d,\"topo\":%.*s}";
int iotx_dmsg_topo_get_reply(iotx_dmsg_response_payload_t *response)
{
	int res = 0, id = 0, message_len = 0;
	char *message = NULL;
	char int_id[IOTX_DCM_UINT32_STRLEN] = {0};

	if (response == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	/* Message ID */
	memcpy(int_id,response->id.value,response->id.value_length);
	id = atoi(int_id);

	dm_log_debug("Current ID: %d",id);

	message_len = strlen(IOTX_DMSG_TOPO_GET_REPLY_FMT) + IOTX_DCM_UINT32_STRLEN*2 + response->data.value_length + 1;
	message = DM_malloc(message_len);
	if (message == NULL) {
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(message,0,message_len);
	HAL_Snprintf(message,message_len,IOTX_DMSG_TOPO_GET_REPLY_FMT,id,response->code.value_int,response->data.value_length,response->data.value);

	res = _iotx_dmsg_send_to_user(IOTX_DM_EVENT_TOPO_GET_REPLY,message);
	if (res != SUCCESS_RETURN) {
		DM_free(message);
		return FAIL_RETURN;
	}

	return SUCCESS_RETURN;
}

int iotx_dmsg_thing_list_found_reply(iotx_dmsg_response_payload_t *response)
{
	return SUCCESS_RETURN;
}

const char IOTX_DMSG_EVENT_PROPERTY_POST_REPLY_FMT[] DM_READ_ONLY = "{\"id\":%d,\"code\":%d,\"devid\":%d}";
int iotx_dmsg_thing_event_property_post_reply(iotx_dmsg_response_payload_t *response)
{
	int res = 0, id = 0, message_len = 0;
	char *message = NULL;
	char int_id[IOTX_DCM_UINT32_STRLEN] = {0};
	iotx_dmc_node_t *node = NULL;

	/* Message ID */
	memcpy(int_id,response->id.value,response->id.value_length);
	id = atoi(int_id);

	dm_log_debug("Current ID: %d",id);

	res = iotx_dmc_msg_search(id,&node);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	message_len = strlen(IOTX_DMSG_EVENT_PROPERTY_POST_REPLY_FMT) + IOTX_DCM_UINT32_STRLEN*3 + 1;
	message = DM_malloc(message_len);
	if (message == NULL) {
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(message,0,message_len);
	HAL_Snprintf(message,message_len,IOTX_DMSG_EVENT_PROPERTY_POST_REPLY_FMT,id,response->code.value_int,node->devid);

	res = _iotx_dmsg_send_to_user(IOTX_DM_EVENT_EVENT_PROPERTY_POST_REPLY,message);
	if (res != SUCCESS_RETURN) {
		DM_free(message);
		return FAIL_RETURN;
	}

	return SUCCESS_RETURN;
}

const char IOTX_DMSG_EVENT_SPECIFIC_POST_REPLY_FMT[] DM_READ_ONLY = "{\"id\":%d,\"code\":%d,\"devid\":%d,\"eventid\":\"%.*s\"}";
int iotx_dmsg_thing_event_post_reply(_IN_ char *identifier, _IN_ int identifier_len, _IN_ iotx_dmsg_response_payload_t *response)
{
	int res = 0, id = 0, message_len = 0;
	char *message = NULL;
	char int_id[IOTX_DCM_UINT32_STRLEN] = {0};
	iotx_dmc_node_t *node = NULL;

	/* Message ID */
	memcpy(int_id,response->id.value,response->id.value_length);
	id = atoi(int_id);

	dm_log_debug("Current ID: %d",id);

	res = iotx_dmc_msg_search(id,&node);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	message_len = strlen(IOTX_DMSG_EVENT_SPECIFIC_POST_REPLY_FMT) + IOTX_DCM_UINT32_STRLEN*3 + strlen(identifier) + 1;
	message = DM_malloc(message_len);
	if (message == NULL) {
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(message,0,message_len);
	HAL_Snprintf(message,message_len,IOTX_DMSG_EVENT_SPECIFIC_POST_REPLY_FMT,id,response->code.value_int,node->devid,identifier_len,identifier);

	res = _iotx_dmsg_send_to_user(IOTX_DM_EVENT_EVENT_SPECIFIC_POST_REPLY,message);
	if (res != SUCCESS_RETURN) {
		DM_free(message);
		return FAIL_RETURN;
	}

	return SUCCESS_RETURN;
}

int iotx_dmsg_thing_deviceinfo_update_reply(iotx_dmsg_response_payload_t *response)
{
	return SUCCESS_RETURN;
}

int iotx_dmsg_thing_deviceinfo_delete_reply(iotx_dmsg_response_payload_t *response)
{
	return SUCCESS_RETURN;
}

int iotx_dmsg_thing_dsltemplate_get_reply(iotx_dmsg_response_payload_t *response)
{
	int res = 0, id = 0;
	char int_id[IOTX_DCM_UINT32_STRLEN] = {0};
	iotx_dmc_node_t *node = NULL;

	if (response == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	/* Message ID */
	memcpy(int_id,response->id.value,response->id.value_length);
	id = atoi(int_id);

	dm_log_debug("Current ID: %d",id);

	res = iotx_dmc_msg_search(id,&node);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	iotx_dmgr_set_tsl(node->devid,IOTX_DM_TSL_TYPE_ALINK,(const char *)response->data.value,response->data.value_length);

	iotx_dcs_topic_service_event_destroy(node->devid);
	iotx_dcs_topic_service_event_create(node->devid);

	return SUCCESS_RETURN;
}

int iotx_dmsg_thing_dynamictsl_get_reply(iotx_dmsg_response_payload_t *response)
{
	int res = 0, id = 0;
	char int_id[IOTX_DCM_UINT32_STRLEN] = {0};
	iotx_dmc_node_t *node = NULL;

	if (response == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	/* Message ID */
	memcpy(int_id,response->id.value,response->id.value_length);
	id = atoi(int_id);

	dm_log_debug("Current ID: %d",id);

	res = iotx_dmc_msg_search(id,&node);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	iotx_dmgr_set_tsl(node->devid,IOTX_DM_TSL_TYPE_ALINK,(const char *)response->data.value,response->data.value_length);

	iotx_dcs_topic_service_event_destroy(node->devid);
	iotx_dcs_topic_service_event_create(node->devid);

	return SUCCESS_RETURN;
}

const char IOTX_DMSG_EVENT_COMBINE_LOGIN_REPLY_FMT[] DM_READ_ONLY = "{\"id\":%d,\"code\":%d,\"devid\":%d}";
int iotx_dmsg_combine_login_reply(iotx_dmsg_response_payload_t *response)
{
	int res = 0, message_len = 0, devid = 0;
	char *message = NULL;
	lite_cjson_t lite, lite_item_pk, lite_item_dn;
	char product_key[PRODUCT_KEY_MAXLEN] = {0};
	char device_name[DEVICE_NAME_MAXLEN] = {0};
	char temp_id[IOTX_DCM_UINT32_STRLEN] = {0};

	if (response == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	/* Parse JSON */
	memset(&lite,0,sizeof(lite_cjson_t));
	res = lite_cjson_parse(response->data.value,response->data.value_length,&lite);
	if (res != SUCCESS_RETURN) {
		dm_log_err(IOTX_DM_LOG_JSON_PARSE_FAILED,response->data.value_length,response->data.value);
		return FAIL_RETURN;
	}

	/* Parse Product Key */
	res = lite_cjson_object_item(&lite,IOTX_DMSG_KEY_PRODUCT_KEY,strlen(IOTX_DMSG_KEY_PRODUCT_KEY),&lite_item_pk);
	if (res != SUCCESS_RETURN || !lite_cjson_is_string(&lite_item_pk) || lite_item_pk.value_length >= PRODUCT_KEY_MAXLEN) {
		dm_log_err(IOTX_DM_LOG_JSON_PARSE_FAILED,strlen(IOTX_DMSG_KEY_PRODUCT_KEY),IOTX_DMSG_KEY_PRODUCT_KEY);
		return FAIL_RETURN;
	}
	memcpy(product_key,lite_item_pk.value,lite_item_pk.value_length);

	/* Parse Device Name */
	res = lite_cjson_object_item(&lite,IOTX_DMSG_KEY_DEVICE_NAME,strlen(IOTX_DMSG_KEY_DEVICE_NAME),&lite_item_dn);
	if (res != SUCCESS_RETURN || !lite_cjson_is_string(&lite_item_dn) || lite_item_dn.value_length >= DEVICE_NAME_MAXLEN) {
		dm_log_err(IOTX_DM_LOG_JSON_PARSE_FAILED,strlen(IOTX_DMSG_KEY_DEVICE_NAME),IOTX_DMSG_KEY_DEVICE_NAME);
		return FAIL_RETURN;
	}
	memcpy(device_name,lite_item_dn.value,lite_item_dn.value_length);

	/* Get Device Id */
	res = iotx_dmgr_search_device_by_pkdn(product_key,device_name,&devid);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	/* Update State Machine */
	if (response->code.value_int == IOTX_DM_ERR_CODE_SUCCESS)
		iotx_dmgr_set_dev_status(devid,IOTX_DMGR_DEV_STATUS_LOGINED);

	/* Message ID */
	memcpy(temp_id,response->id.value,response->id.value_length);

	message_len = strlen(IOTX_DMSG_EVENT_COMBINE_LOGIN_REPLY_FMT) + IOTX_DCM_UINT32_STRLEN*3 + 1;
	message = DM_malloc(message_len);
	if (message == NULL) {
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(message,0,message_len);
	HAL_Snprintf(message,message_len,IOTX_DMSG_EVENT_COMBINE_LOGIN_REPLY_FMT,atoi(temp_id),response->code.value_int,devid);

	res = _iotx_dmsg_send_to_user(IOTX_DM_EVENT_COMBINE_LOGIN_REPLY,message);
	if (res != SUCCESS_RETURN) {
		DM_free(message);
		return FAIL_RETURN;
	}

	if (response->code.value_int != IOTX_DM_ERR_CODE_SUCCESS) {return SUCCESS_RETURN;}
	
	/* Re-Subscribe Topic */
	/* Start From Subscribe Generic Topic */
	res = iotx_dcs_topic_generic_subscribe(devid,0);
	if (res < SUCCESS_RETURN) {return FAIL_RETURN;}

	/* Set Service Event Topic Index To IOTX_DMGR_DEV_SUB_START */
	/* Service Event Topic Subscribe Will Be Execute After All Generic Topic Subscribed */
	res = iotx_dmgr_set_dev_sub_service_event_index(devid,IOTX_DMGR_DEV_SUB_START);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	return SUCCESS_RETURN;
}

const char IOTX_DMSG_EVENT_COMBINE_LOGOUT_REPLY_FMT[] DM_READ_ONLY = "{\"id\":%d,\"code\":%d,\"devid\":%d}";
int iotx_dmsg_combine_logout_reply(iotx_dmsg_response_payload_t *response)
{
	int res = 0, message_len = 0, devid = 0;
	char *message = NULL;
	lite_cjson_t lite, lite_item_pk, lite_item_dn;
	char product_key[PRODUCT_KEY_MAXLEN] = {0};
	char device_name[DEVICE_NAME_MAXLEN] = {0};
	char temp_id[IOTX_DCM_UINT32_STRLEN] = {0};

	if (response == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	/* Parse JSON */
	memset(&lite,0,sizeof(lite_cjson_t));
	res = lite_cjson_parse(response->data.value,response->data.value_length,&lite);
	if (res != SUCCESS_RETURN) {
		dm_log_err(IOTX_DM_LOG_JSON_PARSE_FAILED,response->data.value_length,response->data.value);
		return FAIL_RETURN;
	}

	/* Parse Product Key */
	res = lite_cjson_object_item(&lite,IOTX_DMSG_KEY_PRODUCT_KEY,strlen(IOTX_DMSG_KEY_PRODUCT_KEY),&lite_item_pk);
	if (res != SUCCESS_RETURN || !lite_cjson_is_string(&lite_item_pk) || lite_item_pk.value_length >= PRODUCT_KEY_MAXLEN) {
		dm_log_err(IOTX_DM_LOG_JSON_PARSE_FAILED,strlen(IOTX_DMSG_KEY_PRODUCT_KEY),IOTX_DMSG_KEY_PRODUCT_KEY);
		return FAIL_RETURN;
	}
	memcpy(product_key,lite_item_pk.value,lite_item_pk.value_length);

	/* Parse Device Name */
	res = lite_cjson_object_item(&lite,IOTX_DMSG_KEY_DEVICE_NAME,strlen(IOTX_DMSG_KEY_DEVICE_NAME),&lite_item_dn);
	if (res != SUCCESS_RETURN || !lite_cjson_is_string(&lite_item_dn) || lite_item_dn.value_length >= DEVICE_NAME_MAXLEN) {
		dm_log_err(IOTX_DM_LOG_JSON_PARSE_FAILED,strlen(IOTX_DMSG_KEY_DEVICE_NAME),IOTX_DMSG_KEY_DEVICE_NAME);
		return FAIL_RETURN;
	}
	memcpy(device_name,lite_item_dn.value,lite_item_dn.value_length);

	/* Get Device Id */
	res = iotx_dmgr_search_device_by_pkdn(product_key,device_name,&devid);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	/* Update State Machine */
	if (response->code.value_int == IOTX_DM_ERR_CODE_SUCCESS)
		iotx_dmgr_set_dev_status(devid,IOTX_DMGR_DEV_STATUS_ATTACHED);

	/* Message ID */
	memcpy(temp_id,response->id.value,response->id.value_length);

	message_len = strlen(IOTX_DMSG_EVENT_COMBINE_LOGOUT_REPLY_FMT) + IOTX_DCM_UINT32_STRLEN*3 + 1;
	message = DM_malloc(message_len);
	if (message == NULL) {
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(message,0,message_len);
	HAL_Snprintf(message,message_len,IOTX_DMSG_EVENT_COMBINE_LOGOUT_REPLY_FMT,atoi(temp_id),response->code.value_int,devid);

	res = _iotx_dmsg_send_to_user(IOTX_DM_EVENT_COMBINE_LOGOUT_REPLY,message);
	if (res != SUCCESS_RETURN) {
		DM_free(message);
		return FAIL_RETURN;
	}
	
	return SUCCESS_RETURN;
}

const char IOTX_DMSG_THING_MODEL_UP_RAW_REPLY_FMT[] DM_READ_ONLY = "{\"devid\":%d,\"payload\":\"%.*s\"}";
int iotx_dmsg_thing_model_up_raw_reply(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN],iotx_dmsg_response_payload_t *response)
{
	int res = 0, devid = 0, message_len = 0;
	char *message = NULL;

	if (product_key == NULL || device_name == NULL ||
		(strlen(product_key) >= PRODUCT_KEY_MAXLEN) ||
		(strlen(device_name) >= DEVICE_NAME_MAXLEN) ||
		response == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	res = iotx_dmgr_search_device_by_pkdn(product_key,device_name,&devid);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	message_len = strlen(IOTX_DMSG_THING_MODEL_DOWN_FMT) + IOTX_DCM_UINT32_STRLEN + response->data.value_length + 1;
	message = DM_malloc(message_len);
	if (message == NULL) {
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(message,0,message_len);
	HAL_Snprintf(message,message_len,IOTX_DMSG_THING_MODEL_DOWN_FMT,devid,response->data.value_length,response->data.value);

	res = _iotx_dmsg_send_to_user(IOTX_DM_EVENT_MODEL_UP_RAW_REPLY,message);
	if (res != SUCCESS_RETURN) {
		DM_free(message);
		return FAIL_RETURN;
	}

	return SUCCESS_RETURN;
}

int iotx_dmsg_cloud_connected(void)
{
	char product_key[PRODUCT_KEY_MAXLEN] = {0};
	char device_name[DEVICE_NAME_MAXLEN] = {0};

	HAL_GetProductKey(product_key);
	HAL_GetDeviceName(device_name);

	/* Send To User */
	_iotx_dmsg_send_to_user(IOTX_DM_EVENT_CLOUD_CONNECTED,NULL);

	return SUCCESS_RETURN;
}

int iotx_dmsg_cloud_disconnect(void)
{
	return _iotx_dmsg_send_to_user(IOTX_DM_EVENT_CLOUD_DISCONNECT,NULL);
}

int iotx_dmsg_cloud_reconnect(void)
{
	char product_key[PRODUCT_KEY_MAXLEN] = {0};
	char device_name[DEVICE_NAME_MAXLEN] = {0};

	HAL_GetProductKey(product_key);
	HAL_GetDeviceName(device_name);

	/* Send To User */
	_iotx_dmsg_send_to_user(IOTX_DM_EVENT_CLOUD_RECONNECT,NULL);

	return SUCCESS_RETURN;
}

const char IOTX_DMSG_EVENT_FOUND_DEVICE_FMT[] DM_READ_ONLY = "{\"product_key\":\"%s\",\"device_name\":\"%s\"}";
int iotx_dmsg_found_device(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN])
{
	int res = 0, message_len = 0;
	char *message = NULL;

	if (product_key == NULL || device_name == NULL ||
		(strlen(product_key) >= PRODUCT_KEY_MAXLEN) ||
		(strlen(device_name) >= DEVICE_NAME_MAXLEN)) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	message_len = strlen(IOTX_DMSG_EVENT_FOUND_DEVICE_FMT) + strlen(product_key) + strlen(device_name) + 1;
	message = DM_malloc(message_len);
	if (message == NULL) {
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(message,0,message_len);
	HAL_Snprintf(message,message_len,IOTX_DMSG_EVENT_FOUND_DEVICE_FMT,product_key,device_name);

	res = _iotx_dmsg_send_to_user(IOTX_DM_EVENT_FOUND_DEVICE,message);
	if (res != SUCCESS_RETURN) {
		DM_free(message);
		return FAIL_RETURN;
	}

	return SUCCESS_RETURN;
}

const char IOTX_DMSG_EVENT_REMOVE_DEVICE_FMT[] DM_READ_ONLY = "{\"product_key\":\"%s\",\"device_name\":\"%s\"}";
int iotx_dmsg_remove_device(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN])
{
	int res = 0, message_len = 0;
	char *message = NULL;

	if (product_key == NULL || device_name == NULL ||
		(strlen(product_key) >= PRODUCT_KEY_MAXLEN) ||
		(strlen(device_name) >= DEVICE_NAME_MAXLEN)) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	message_len = strlen(IOTX_DMSG_EVENT_REMOVE_DEVICE_FMT) + strlen(product_key) + strlen(device_name) + 1;
	message = DM_malloc(message_len);
	if (message == NULL) {
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(message,0,message_len);
	HAL_Snprintf(message,message_len,IOTX_DMSG_EVENT_REMOVE_DEVICE_FMT,product_key,device_name);

	res = _iotx_dmsg_send_to_user(IOTX_DM_EVENT_REMOVE_DEVICE,message);
	if (res != SUCCESS_RETURN) {
		DM_free(message);
		return FAIL_RETURN;
	}

	return SUCCESS_RETURN;
}

const char IOTX_DMSG_EVENT_REGISTER_COMPLETED_FMT[] DM_READ_ONLY = "{\"devid\":%d}";
const char IOTX_DMSG_EVENT_REGISTER_RESULT_FMT[] DM_READ_ONLY = "{\"result\":%d,\"uri\":%s}";
int iotx_dmsg_register_result(_IN_ char *uri,_IN_ int result)
{
	int res = 0, devid = 0, index = 0, number = 0, message_len = 0;
	char product_key[PRODUCT_KEY_MAXLEN] = {0};
	char device_name[DEVICE_NAME_MAXLEN] = {0};
	char *message = NULL;

	if (result != SUCCESS_RETURN) {return FAIL_RETURN;}

	res = iotx_dcs_uri_prefix_sys_split(uri,strlen(uri),NULL,NULL);
	if (res == SUCCESS_RETURN) {
		res = iotx_dmsg_uri_parse_pkdn(uri,strlen(uri),2,4,product_key,device_name);
		if(res != SUCCESS_RETURN) {return FAIL_RETURN;}
	}

	res = iotx_dcs_uri_prefix_ext_session_split(uri,strlen(uri),NULL,NULL);
	if (res == SUCCESS_RETURN) {
		res = iotx_dmsg_uri_parse_pkdn(uri,strlen(uri),3,5,product_key,device_name);
		if(res != SUCCESS_RETURN) {return FAIL_RETURN;}
	}

	res = iotx_dmgr_search_device_by_pkdn(product_key,device_name,&devid);
	if(res != SUCCESS_RETURN) {return FAIL_RETURN;}

	res = iotx_dmgr_get_dev_sub_generic_index(devid,&index);
	if(res != SUCCESS_RETURN) {return FAIL_RETURN;}

	dm_log_debug("Current Generic Index: %d",index);

	if (index >= 0 && index + 1 < iotx_dcs_get_topic_mapping_size()) {
		res = iotx_dcs_topic_generic_subscribe(devid,index + 1);
		if (res != iotx_dcs_get_topic_mapping_size()) {return res;}
	}
	if ((((index + 1) >= iotx_dcs_get_topic_mapping_size()) || (res == iotx_dcs_get_topic_mapping_size())) && index != IOTX_DMGR_DEV_SUB_END) {
		dm_log_debug("Devid %d Subscribe Completed",devid);
	
		message_len = strlen(IOTX_DMSG_EVENT_REGISTER_COMPLETED_FMT) + IOTX_DCM_UINT32_STRLEN + 1;
		message = DM_malloc(message_len);
		if (message == NULL) {
			dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
			return FAIL_RETURN;
		}
		memset(message,0,message_len);
		HAL_Snprintf(message,message_len,IOTX_DMSG_EVENT_REGISTER_COMPLETED_FMT,devid);

		res = _iotx_dmsg_send_to_user(IOTX_DM_EVENT_REGISTER_COMPLETED,message);
		if (res != SUCCESS_RETURN) {
			DM_free(message);
			return FAIL_RETURN;
		}
	
	}
	iotx_dmgr_set_dev_sub_generic_index(devid,IOTX_DMGR_DEV_SUB_END);

	/* Check TSL Source And If Shadow Is Exist */
	iotx_dm_tsl_source_t source;
	void *shadow = NULL;

	res = iotx_dmgr_get_shadow(devid,&shadow);
	if(res != SUCCESS_RETURN) {return FAIL_RETURN;}
	
	res = iotx_dmgr_get_tsl_source(devid,&source);
	if(res != SUCCESS_RETURN) {return FAIL_RETURN;}

	if (source == IOTX_DM_TSL_SOURCE_CLOUD && shadow == NULL) {
		iotx_dcs_thing_dsltemplate_get(product_key,device_name);
	}
	
	res = iotx_dmgr_get_dev_sub_service_event_index(devid,&index);
	if(res != SUCCESS_RETURN) {return FAIL_RETURN;}

	dm_log_debug("Current Service Event Index: %d",index);

	res = iotx_dmgr_get_dev_sub_service_event_number(devid,&number);
	if(res != SUCCESS_RETURN) {return FAIL_RETURN;}

	dm_log_debug("Current Service Event Number: %d",number);

	if (index >= IOTX_DMGR_DEV_SUB_START && index + 1 < number) {
		res = iotx_dcs_topic_service_event_subscribe(devid,index + 1);
		return res;
	}
	iotx_dmgr_set_dev_sub_service_event_index(devid,IOTX_DMGR_DEV_SUB_END);
	iotx_dmgr_clear_dev_sub_service_event(devid);
	iotx_dmgr_set_dev_status(devid,IOTX_DMGR_DEV_STATUS_ONLINE);
	
	return SUCCESS_RETURN;
}

const char IOTX_DMSG_EVENT_UNREGISTER_RESULT_FMT[] DM_READ_ONLY = "{\"result\":%d,\"uri\":%s}";
int iotx_dmsg_unregister_result(_IN_ char *uri,_IN_ int result)
{
	int res = 0, message_len = 0;
	char *message = NULL;
	if (uri == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	message_len = strlen(IOTX_DMSG_EVENT_UNREGISTER_RESULT_FMT) + 10 + strlen(uri) + 1;
	message = DM_malloc(message_len);
	if (message == NULL) {
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(message,0,message_len);
	HAL_Snprintf(message,message_len,IOTX_DMSG_EVENT_UNREGISTER_RESULT_FMT,result,uri);

	res = _iotx_dmsg_send_to_user(IOTX_DM_EVENT_UNREGISTER_RESULT,message);
	if (res != SUCCESS_RETURN) {
		DM_free(message);
		return FAIL_RETURN;
	}

	return SUCCESS_RETURN;
}

const char IOTX_DMSG_EVENT_SEND_RESULT_FMT[] DM_READ_ONLY = "{\"result\":%d,\"uri\":%s}";
int iotx_dmsg_send_result(_IN_ char *uri,_IN_ int result)
{
	int res = 0, message_len = 0;
	char *message = NULL;
	if (uri == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	message_len = strlen(IOTX_DMSG_EVENT_SEND_RESULT_FMT) + 10 + strlen(uri) + 1;
	message = DM_malloc(message_len);
	if (message == NULL) {
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(message,0,message_len);
	HAL_Snprintf(message,message_len,IOTX_DMSG_EVENT_SEND_RESULT_FMT,result,uri);

	res = _iotx_dmsg_send_to_user(IOTX_DM_EVENT_SEND_RESULT,message);
	if (res != SUCCESS_RETURN) {
		DM_free(message);
		return FAIL_RETURN;
	}

	return SUCCESS_RETURN;
}

const char IOTX_DMSG_EVENT_ADD_SERVICE_RESULT_FMT[] DM_READ_ONLY = "{\"result\":%d,\"uri\":%s}";
int iotx_dmsg_add_service_result(_IN_ char *uri,_IN_ int result)
{
	int res = 0, message_len = 0;
	char *message = NULL;
	if (uri == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	message_len = strlen(IOTX_DMSG_EVENT_ADD_SERVICE_RESULT_FMT) + 10 + strlen(uri) + 1;
	message = DM_malloc(message_len);
	if (message == NULL) {
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(message,0,message_len);
	HAL_Snprintf(message,message_len,IOTX_DMSG_EVENT_ADD_SERVICE_RESULT_FMT,result,uri);

	res = _iotx_dmsg_send_to_user(IOTX_DM_EVENT_ADD_SERVICE_RESULT,message);
	if (res != SUCCESS_RETURN) {
		DM_free(message);
		return FAIL_RETURN;
	}

	return SUCCESS_RETURN;
}

const char IOTX_DMSG_EVENT_REMOVE_SERVICE_RESULT_FMT[] DM_READ_ONLY = "{\"result\":%d,\"uri\":%s}";
int iotx_dmsg_remove_service_result(_IN_ char *uri,_IN_ int result)
{
	int res = 0, message_len = 0;
	char *message = NULL;
	if (uri == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	message_len = strlen(IOTX_DMSG_EVENT_REMOVE_SERVICE_RESULT_FMT) + 10 + strlen(uri) + 1;
	message = DM_malloc(message_len);
	if (message == NULL) {
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(message,0,message_len);
	HAL_Snprintf(message,message_len,IOTX_DMSG_EVENT_REMOVE_SERVICE_RESULT_FMT,result,uri);

	res = _iotx_dmsg_send_to_user(IOTX_DM_EVENT_REMOVE_SERVICE_RESULT,message);
	if (res != SUCCESS_RETURN) {
		DM_free(message);
		return FAIL_RETURN;
	}

	return SUCCESS_RETURN;
}

const char IOTX_DMSG_THING_SUB_REGISTER_METHOD[] DM_READ_ONLY = "thing.sub.register";
const char IOTX_DMSG_THING_SUB_REGISTER_PARAMS[] DM_READ_ONLY = "[{\"productKey\":\"%s\",\"deviceName\":\"%s\"}]";
int iotx_dmsg_thing_sub_register(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN], _OU_ iotx_dmsg_request_t *request)
{
	int params_len = 0;
	char *params = NULL;

	if (request == NULL || product_key == NULL || device_name == NULL ||
		(strlen(product_key) >= PRODUCT_KEY_MAXLEN) ||
		(strlen(device_name) >= DEVICE_NAME_MAXLEN) ||
		request->product_key == NULL || request->device_name == NULL ||
		(strlen(request->product_key) >= PRODUCT_KEY_MAXLEN) ||
		(strlen(request->device_name) >= DEVICE_NAME_MAXLEN)) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	params_len = strlen(IOTX_DMSG_THING_SUB_REGISTER_PARAMS) + strlen(product_key) + strlen(device_name) + 1;
	params = DM_malloc(params_len);
	if (params == NULL) {
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(params,0,params_len);
	HAL_Snprintf(params,params_len,IOTX_DMSG_THING_SUB_REGISTER_PARAMS,product_key,device_name);

	/* Get Params */
	request->params = params;

	/* Get Method */
	request->method = (char *)IOTX_DMSG_THING_SUB_REGISTER_METHOD;

	return SUCCESS_RETURN;
}

const char IOTX_DMSG_THING_SUB_UNREGISTER_METHOD[] DM_READ_ONLY = "thing.sub.unregister";
const char IOTX_DMSG_THING_SUB_UNREGISTER_PARAMS[] DM_READ_ONLY = "[{\"productKey\":\"%s\",\"deviceName\":\"%s\"}]";
int iotx_dmsg_thing_sub_unregister(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN], _OU_ iotx_dmsg_request_t *request)
{
	int params_len = 0;
	char *params = NULL;

	if (request == NULL || product_key == NULL || device_name == NULL ||
		(strlen(product_key) >= PRODUCT_KEY_MAXLEN) ||
		(strlen(device_name) >= DEVICE_NAME_MAXLEN) ||
		request->product_key == NULL || request->device_name == NULL ||
		(strlen(request->product_key) >= PRODUCT_KEY_MAXLEN) ||
		(strlen(request->device_name) >= DEVICE_NAME_MAXLEN)) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	params_len = strlen(IOTX_DMSG_THING_SUB_UNREGISTER_PARAMS) + strlen(product_key) + strlen(device_name) + 1;
	params = DM_malloc(params_len);
	if (params == NULL) {
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(params,0,params_len);
	HAL_Snprintf(params,params_len,IOTX_DMSG_THING_SUB_UNREGISTER_PARAMS,product_key,device_name);

	/* Get Params */
	request->params = params;

	/* Get Method */
	request->method = (char *)IOTX_DMSG_THING_SUB_UNREGISTER_METHOD;

	return SUCCESS_RETURN;
}

const char IOTX_DMSG_THING_TOPO_ADD_SIGN_SOURCE[] DM_READ_ONLY = "clientId%sdeviceName%sproductKey%stimestamp%s";
const char IOTX_DMSG_THING_TOPO_ADD_METHOD[] DM_READ_ONLY = "thing.topo.add";
const char IOTX_DMSG_THING_TOPO_ADD_PARAMS[] DM_READ_ONLY = "[{\"productKey\":\"%s\",\"deviceName\":\"%s\",\"signmethod\":\"%s\",\"sign\":\"%s\",\"timestamp\":\"%s\",\"clientId\":\"%s\"}]";
int iotx_dmsg_thing_topo_add(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN], _IN_ char device_secret[DEVICE_SECRET_MAXLEN], _OU_ iotx_dmsg_request_t *request)
{
	char *params = NULL;
	int params_len = 0;
	char timestamp[IOTX_DCM_UINT64_STRLEN] = {0};
	char client_id[PRODUCT_KEY_MAXLEN + DEVICE_NAME_MAXLEN + 1] = {0};
	char *sign_source = NULL;
	int sign_source_len = 0;
	char *sign_method = IOTX_DMSG_SIGN_METHOD_HMACSHA1;
	char sign[64] = {0};


	if (request == NULL || product_key == NULL ||
		device_name == NULL || device_secret == NULL ||
		(strlen(product_key) >= PRODUCT_KEY_MAXLEN) ||
		(strlen(device_name) >= DEVICE_NAME_MAXLEN) ||
		(strlen(device_secret) >= DEVICE_SECRET_MAXLEN) ||
		request->product_key == NULL || request->device_name == NULL ||
		(strlen(request->product_key) >= PRODUCT_KEY_MAXLEN) ||
		(strlen(request->device_name) >= DEVICE_NAME_MAXLEN)) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	/* TimeStamp */
	HAL_Snprintf(timestamp,IOTX_DCM_UINT64_STRLEN,"%llu",HAL_UptimeMs());
	dm_log_debug("Time Stamp: %s",timestamp);

	/* Client ID */
	HAL_Snprintf(client_id,PRODUCT_KEY_MAXLEN + DEVICE_NAME_MAXLEN + 1,"%s.%s",product_key,device_name);

	/* Sign */
	sign_source_len = strlen(IOTX_DMSG_THING_TOPO_ADD_SIGN_SOURCE) + strlen(client_id) +
					strlen(device_name) + strlen(product_key) + strlen(timestamp) + 1;
	sign_source = DM_malloc(sign_source_len);
	if (sign_source == NULL) {
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(sign_source,0,sign_source_len);
	HAL_Snprintf(sign_source,sign_source_len,IOTX_DMSG_THING_TOPO_ADD_SIGN_SOURCE,client_id,
					device_name,product_key,timestamp);

	dm_log_debug("Sign Srouce: %s",sign_source);

	if (strcmp(sign_method,IOTX_DMSG_SIGN_METHOD_HMACMD5) == 0) {
		utils_hmac_md5(sign_source,strlen(sign_source),sign,device_secret,strlen(device_secret));
	}else if (strcmp(sign_method,IOTX_DMSG_SIGN_METHOD_HMACSHA1) == 0) {
		utils_hmac_sha1(sign_source,strlen(sign_source),sign,device_secret,strlen(device_secret));
	}else if (strcmp(sign_method,IOTX_DMSG_SIGN_METHOD_HMACSHA256) == 0) {
		utils_hmac_sha256(sign_source,strlen(sign_source),sign,device_secret,strlen(device_secret));
	}else{
		DM_free(sign_source);
		dm_log_err(IOTX_DM_LOG_SIGN_METHOD_NOT_FOUND);
		return FAIL_RETURN;
	}
	DM_free(sign_source);
	dm_log_debug("Sign : %s",sign);

	/* Params */
	request->method = (char *)IOTX_DMSG_THING_TOPO_ADD_METHOD;
	params_len = strlen(IOTX_DMSG_THING_TOPO_ADD_PARAMS) + strlen(product_key) + strlen(device_name) +
						strlen(sign_method) + strlen(sign) + strlen(timestamp) + strlen(client_id) + 1;
	params = DM_malloc(params_len);

	if (params == NULL) {
		dm_log_err(IOTX_DM_LOG_SIGN_METHOD_NOT_FOUND);
		return FAIL_RETURN;
	}
	memset(params,0,params_len);
	HAL_Snprintf(params,params_len,IOTX_DMSG_THING_TOPO_ADD_PARAMS,product_key,device_name,
					sign_method,sign,timestamp,client_id);

	request->params = params;

	return SUCCESS_RETURN;
}

const char IOTX_DMSG_THING_TOPO_DELETE_METHOD[] DM_READ_ONLY = "thing.topo.delete";
const char IOTX_DMSG_THING_TOPO_DELETE_PARAMS[] DM_READ_ONLY = "[{\"productKey\":\"%s\",\"deviceName\":\"%s\"}]";
int iotx_dmsg_thing_topo_delete(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN], _OU_ iotx_dmsg_request_t *request)
{
	char *params = NULL;
	int params_len = 0;

	if (request == NULL || product_key == NULL ||
		device_name == NULL ||
		(strlen(product_key) >= PRODUCT_KEY_MAXLEN) ||
		(strlen(device_name) >= DEVICE_NAME_MAXLEN) ||
		request->product_key == NULL || request->device_name == NULL ||
		(strlen(request->product_key) >= PRODUCT_KEY_MAXLEN) ||
		(strlen(request->device_name) >= DEVICE_NAME_MAXLEN)) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	/* Params */
	request->method = (char *)IOTX_DMSG_THING_TOPO_DELETE_METHOD;
	params_len = strlen(IOTX_DMSG_THING_TOPO_DELETE_PARAMS) + strlen(product_key) + strlen(device_name) + 1;
	params = DM_malloc(params_len);
	if (params == NULL) {
		dm_log_err(IOTX_DM_LOG_SIGN_METHOD_NOT_FOUND);
		return FAIL_RETURN;
	}
	memset(params,0,params_len);
	HAL_Snprintf(params,params_len,IOTX_DMSG_THING_TOPO_DELETE_PARAMS,product_key,device_name);

	request->params = params;

	return SUCCESS_RETURN;
}

const char IOTX_DMSG_THING_TOPO_GET_METHOD[] DM_READ_ONLY = "thing.topo.get";
const char IOTX_DMSG_THING_TOPO_GET_PARAMS[] DM_READ_ONLY = "{}";
int iotx_dmsg_thing_topo_get(_OU_ iotx_dmsg_request_t *request)
{
	char *params = NULL;
	int params_len = 0;

	/* Params */
	request->method = (char *)IOTX_DMSG_THING_TOPO_GET_METHOD;
	params_len = strlen(IOTX_DMSG_THING_TOPO_GET_PARAMS) + 1;
	params = DM_malloc(params_len);
	if (params == NULL) {
		dm_log_err(IOTX_DM_LOG_SIGN_METHOD_NOT_FOUND);
		return FAIL_RETURN;
	}
	memset(params,0,params_len);
	memcpy(params,IOTX_DMSG_THING_TOPO_GET_PARAMS,strlen(IOTX_DMSG_THING_TOPO_GET_PARAMS));

	request->params = params;

	return SUCCESS_RETURN;
}

const char IOTX_DMSG_THING_LIST_FOUND_METHOD[] DM_READ_ONLY = "thing.list.found";
const char IOTX_DMSG_THING_LIST_FOUND_PARAMS[] DM_READ_ONLY = "[{\"productKey\":\"%s\",\"deviceName\":\"%s\"}]";
int iotx_dmsg_thing_list_found(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN], _OU_ iotx_dmsg_request_t *request)
{
	char *params = NULL;
	int params_len = 0;

	if (product_key == NULL || device_name == NULL ||
		(strlen(product_key) >= PRODUCT_KEY_MAXLEN) ||
		(strlen(device_name) >= DEVICE_NAME_MAXLEN) ||
		request == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	/* Params */
	request->method = (char *)IOTX_DMSG_THING_LIST_FOUND_METHOD;
	params_len = strlen(IOTX_DMSG_THING_LIST_FOUND_PARAMS) + strlen(product_key) + strlen(device_name) + 1;
	params = DM_malloc(params_len);
	if (params == NULL) {
		dm_log_err(IOTX_DM_LOG_SIGN_METHOD_NOT_FOUND);
		return FAIL_RETURN;
	}
	memset(params,0,params_len);
	HAL_Snprintf(params,params_len,IOTX_DMSG_THING_LIST_FOUND_PARAMS,product_key,device_name);

	request->params = params;

	return SUCCESS_RETURN;
}

const char IOTX_DMSG_THING_PROPERTY_POST_METHOD[] DM_READ_ONLY = "thing.event.property.post";
int iotx_dmsg_thing_property_post(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN], _IN_ char *payload, _OU_ iotx_dmsg_request_t *request)
{
	char *params = NULL;
	int params_len = 0;

	if (product_key == NULL || device_name == NULL ||
		(strlen(product_key) >= PRODUCT_KEY_MAXLEN) ||
		(strlen(device_name) >= DEVICE_NAME_MAXLEN) ||
		payload == NULL || request == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	/* Params */
	request->method = (char *)IOTX_DMSG_THING_PROPERTY_POST_METHOD;
	params_len = strlen(payload) + 1;
	params = DM_malloc(params_len);
	if (params == NULL) {
		dm_log_err(IOTX_DM_LOG_SIGN_METHOD_NOT_FOUND);
		return FAIL_RETURN;
	}
	memset(params,0,params_len);
	memcpy(params,payload,strlen(payload));

	request->params = params;

	return SUCCESS_RETURN;
}

int iotx_dmsg_thing_event_post(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN], _IN_ char *method, _IN_ char *payload, _OU_ iotx_dmsg_request_t *request)
{
	char *params = NULL;
	int params_len = 0;

	if (product_key == NULL || device_name == NULL ||
		(strlen(product_key) >= PRODUCT_KEY_MAXLEN) ||
		(strlen(device_name) >= DEVICE_NAME_MAXLEN) ||
		payload == NULL || request == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	/* Params */
	request->method = method;
	params_len = strlen(payload) + 1;
	params = DM_malloc(params_len);
	if (params == NULL) {
		dm_log_err(IOTX_DM_LOG_SIGN_METHOD_NOT_FOUND);
		return FAIL_RETURN;
	}
	memset(params,0,params_len);
	memcpy(params,payload,strlen(payload));

	request->params = params;

	return SUCCESS_RETURN;
}

const char IOTX_DMSG_THING_DEVICEINFO_UPDATE_METHOD[] DM_READ_ONLY = "thing.deviceinfo.update";
/* const char IOTX_DMSG_THING_DEVICEINFO_UPDATE_PARAMS[] DM_READ_ONLY = "{}"; */
int iotx_dmsg_thing_deviceinfo_update(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN], _IN_ char *payload, _OU_ iotx_dmsg_request_t *request)
{
	char *params = NULL;
	int params_len = 0;

	if (product_key == NULL || device_name == NULL ||
		(strlen(product_key) >= PRODUCT_KEY_MAXLEN) ||
		(strlen(device_name) >= DEVICE_NAME_MAXLEN) ||
		payload == NULL || request == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	/* Params */
	request->method = (char *)IOTX_DMSG_THING_DEVICEINFO_UPDATE_METHOD;
	params_len = strlen(payload) + 1;
	params = DM_malloc(params_len);
	if (params == NULL) {
		dm_log_err(IOTX_DM_LOG_SIGN_METHOD_NOT_FOUND);
		return FAIL_RETURN;
	}
	memset(params,0,params_len);
	memcpy(params,payload,strlen(payload));

	request->params = params;

	return SUCCESS_RETURN;
}

const char IOTX_DMSG_THING_DEVICEINFO_DELETE_METHOD[] DM_READ_ONLY = "thing.deviceinfo.delete";
int iotx_dmsg_thing_deviceinfo_delete(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN], _IN_ char *payload, _OU_ iotx_dmsg_request_t *request)
{
	char *params = NULL;
	int params_len = 0;

	if (product_key == NULL || device_name == NULL ||
		(strlen(product_key) >= PRODUCT_KEY_MAXLEN) ||
		(strlen(device_name) >= DEVICE_NAME_MAXLEN) ||
		payload == NULL || request == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	/* Params */
	request->method = (char *)IOTX_DMSG_THING_DEVICEINFO_DELETE_METHOD;
	params_len = strlen(payload) + 1;
	params = DM_malloc(params_len);
	if (params == NULL) {
		dm_log_err(IOTX_DM_LOG_SIGN_METHOD_NOT_FOUND);
		return FAIL_RETURN;
	}
	memset(params,0,params_len);
	memcpy(params,payload,strlen(payload));

	request->params = params;

	return SUCCESS_RETURN;
}

const char IOTX_DMSG_THING_DSLTEMPLATE_GET_METHOD[] DM_READ_ONLY = "thing.dsltemplate.get";
const char IOTX_DMSG_THING_DSLTEMPLATE_GET_PARAMS[] DM_READ_ONLY = "{}";
int iotx_dmsg_thing_dsltemplate_get(_OU_ iotx_dmsg_request_t *request)
{
	char *params = NULL;
	int params_len = 0;

	if (request == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	/* Params */
	request->method = (char *)IOTX_DMSG_THING_DSLTEMPLATE_GET_METHOD;
	params_len = strlen(IOTX_DMSG_THING_DSLTEMPLATE_GET_PARAMS) + 1;
	params = DM_malloc(params_len);
	if (params == NULL) {
		dm_log_err(IOTX_DM_LOG_SIGN_METHOD_NOT_FOUND);
		return FAIL_RETURN;
	}
	memset(params,0,params_len);
	memcpy(params,IOTX_DMSG_THING_DSLTEMPLATE_GET_PARAMS,strlen(IOTX_DMSG_THING_DSLTEMPLATE_GET_PARAMS));

	request->params = params;

	return SUCCESS_RETURN;
}

const char IOTX_DMSG_THING_DYNAMICTSL_GET_METHOD[] DM_READ_ONLY = "thing.dynamicTsl.get";
const char IOTX_DMSG_THING_DYNAMICTSL_GET_PARAMS[] DM_READ_ONLY = "{\"nodes\":[\"type\",\"identifier\"],\"addDefault\":false}";
int iotx_dmsg_thing_dynamictsl_get(_OU_ iotx_dmsg_request_t *request)
{
	char *params = NULL;
	int params_len = 0;

	if (request == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	/* Params */
	request->method = (char *)IOTX_DMSG_THING_DYNAMICTSL_GET_METHOD;
	params_len = strlen(IOTX_DMSG_THING_DYNAMICTSL_GET_PARAMS) + 1;
	params = DM_malloc(params_len);
	if (params == NULL) {
		dm_log_err(IOTX_DM_LOG_SIGN_METHOD_NOT_FOUND);
		return FAIL_RETURN;
	}
	memset(params,0,params_len);
	memcpy(params,IOTX_DMSG_THING_DYNAMICTSL_GET_PARAMS,strlen(IOTX_DMSG_THING_DYNAMICTSL_GET_PARAMS));

	request->params = params;

	return SUCCESS_RETURN;
}

const char IOTX_DMSG_COMBINE_LOGIN_SIGN_SOURCE[] DM_READ_ONLY = "clientId%sdeviceName%sproductKey%stimestamp%s";
const char IOTX_DMSG_COMBINE_LOGIN_METHOD[] DM_READ_ONLY = "combine.login";
const char IOTX_DMSG_COMBINE_LOGIN_PARAMS[] DM_READ_ONLY = "{\"productKey\":\"%s\",\"deviceName\":\"%s\",\"clientId\":\"%s\",\"timestamp\":\"%s\",\"signMethod\":\"%s\",\"sign\":\"%s\",\"cleanSession\":\"%s\"}";
int iotx_dmsg_combine_login(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN], _IN_ char device_secret[DEVICE_SECRET_MAXLEN], _OU_ iotx_dmsg_request_t *request)
{
	char *params = NULL;
	int params_len = 0;
	char timestamp[IOTX_DCM_UINT64_STRLEN] = {0};
	char client_id[PRODUCT_KEY_MAXLEN + DEVICE_NAME_MAXLEN + 1] = {0};
	char *sign_source = NULL;
	int sign_source_len = 0;
	char *sign_method = IOTX_DMSG_SIGN_METHOD_HMACSHA1;
	char sign[64] = {0};


	if (request == NULL || product_key == NULL ||
		device_name == NULL || device_secret == NULL ||
		(strlen(product_key) >= PRODUCT_KEY_MAXLEN) ||
		(strlen(device_name) >= DEVICE_NAME_MAXLEN) ||
		(strlen(device_secret) >= DEVICE_SECRET_MAXLEN) ||
		request->product_key == NULL || request->device_name == NULL ||
		(strlen(request->product_key) >= PRODUCT_KEY_MAXLEN) ||
		(strlen(request->device_name) >= DEVICE_NAME_MAXLEN)) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	/* TimeStamp */
	HAL_Snprintf(timestamp,IOTX_DCM_UINT64_STRLEN,"%llu",HAL_UptimeMs());
	dm_log_debug("Time Stamp: %s",timestamp);

	/* Client ID */
	HAL_Snprintf(client_id,PRODUCT_KEY_MAXLEN + DEVICE_NAME_MAXLEN + 1,"%s.%s",product_key,device_name);

	/* Sign */
	sign_source_len = strlen(IOTX_DMSG_COMBINE_LOGIN_SIGN_SOURCE) + strlen(client_id) +
					strlen(device_name) + strlen(product_key) + strlen(timestamp) + 1;
	sign_source = DM_malloc(sign_source_len);
	if (sign_source == NULL) {
		dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(sign_source,0,sign_source_len);
	HAL_Snprintf(sign_source,sign_source_len,IOTX_DMSG_COMBINE_LOGIN_SIGN_SOURCE,client_id,
					device_name,product_key,timestamp);

	dm_log_debug("Sign Srouce: %s",sign_source);

	if (strcmp(sign_method,IOTX_DMSG_SIGN_METHOD_HMACMD5) == 0) {
		utils_hmac_md5(sign_source,strlen(sign_source),sign,device_secret,strlen(device_secret));
	}else if (strcmp(sign_method,IOTX_DMSG_SIGN_METHOD_HMACSHA1) == 0) {
		utils_hmac_sha1(sign_source,strlen(sign_source),sign,device_secret,strlen(device_secret));
	}else if (strcmp(sign_method,IOTX_DMSG_SIGN_METHOD_HMACSHA256) == 0) {
		utils_hmac_sha256(sign_source,strlen(sign_source),sign,device_secret,strlen(device_secret));
	}else{
		DM_free(sign_source);
		dm_log_err(IOTX_DM_LOG_SIGN_METHOD_NOT_FOUND);
		return FAIL_RETURN;
	}
	DM_free(sign_source);
	dm_log_debug("Sign : %s",sign);

	/* Params */
	request->method = (char *)IOTX_DMSG_COMBINE_LOGIN_METHOD;
	params_len = strlen(IOTX_DMSG_COMBINE_LOGIN_PARAMS) + strlen(product_key) + strlen(device_name) +
						strlen(sign_method) + strlen(sign) + strlen(timestamp) + strlen(client_id) + 1;
	params = DM_malloc(params_len);

	if (params == NULL) {
		dm_log_err(IOTX_DM_LOG_SIGN_METHOD_NOT_FOUND);
		return FAIL_RETURN;
	}
	memset(params,0,params_len);
	HAL_Snprintf(params,params_len,IOTX_DMSG_COMBINE_LOGIN_PARAMS,product_key,device_name,
					client_id,timestamp,sign_method,sign,"true");

	request->params = params;

	return SUCCESS_RETURN;
}

const char IOTX_DMSG_COMBINE_LOGOUT_METHOD[] DM_READ_ONLY = "combine.logout";
const char IOTX_DMSG_COMBINE_LOGOUT_PARAMS[] DM_READ_ONLY = "{\"productKey\":\"%s\",\"deviceName\":\"%s\"}";
int iotx_dmsg_combine_logout(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN], _OU_ iotx_dmsg_request_t *request)
{
	char *params = NULL;
	int params_len = 0;

	if (product_key == NULL || device_name == NULL ||
		(strlen(product_key) >= PRODUCT_KEY_MAXLEN) ||
		(strlen(device_name) >= DEVICE_NAME_MAXLEN) ||
		request == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	/* Params */
	request->method = (char *)IOTX_DMSG_COMBINE_LOGOUT_METHOD;
	params_len = strlen(IOTX_DMSG_COMBINE_LOGOUT_PARAMS) + strlen(product_key) + strlen(device_name) + 1;
	params = DM_malloc(params_len);

	if (params == NULL) {
		dm_log_err(IOTX_DM_LOG_SIGN_METHOD_NOT_FOUND);
		return FAIL_RETURN;
	}
	memset(params,0,params_len);
	HAL_Snprintf(params,params_len,IOTX_DMSG_COMBINE_LOGOUT_PARAMS,product_key,device_name);

	request->params = params;

	return SUCCESS_RETURN;
}
