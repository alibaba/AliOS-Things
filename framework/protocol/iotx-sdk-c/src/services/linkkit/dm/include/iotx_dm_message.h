#ifndef _IOTX_DM_MESSAGE_H_
#define _IOTX_DM_MESSAGE_H_

#include "lite-utils.h"
#include "lite-list.h"
#include "lite-cjson.h"
#include "iot_export_dm.h"

#define IOTX_DMSG_KEY_ID                   "id"
#define IOTX_DMSG_KEY_VERSION              "version"
#define IOTX_DMSG_KEY_METHOD               "method"
#define IOTX_DMSG_KEY_PARAMS               "params"
#define IOTX_DMSG_KEY_CODE                 "code"
#define IOTX_DMSG_KEY_DATA                 "data"

#define IOTX_DMSG_VERSION                  "1.0"

#define IOTX_DMSG_KEY_PRODUCT_KEY          "productKey"
#define IOTX_DMSG_KEY_DEVICE_NAME          "deviceName"
#define IOTX_DMSG_KEY_DEVICE_SECRET        "deviceSecret"
#define IOTX_DMSG_KEY_TIME                 "time"

#define IOTX_DMSG_SIGN_METHOD_SHA256       "Sha256"
#define IOTX_DMSG_SIGN_METHOD_HMACMD5      "hmacMd5"
#define IOTX_DMSG_SIGN_METHOD_HMACSHA1     "hmacSha1"
#define IOTX_DMSG_SIGN_METHOD_HMACSHA256   "hmacSha256"

typedef struct {
	lite_cjson_t id;
	lite_cjson_t version;
	lite_cjson_t method;
	lite_cjson_t params;
}iotx_dmsg_request_payload_t;

typedef struct {
	lite_cjson_t id;
	lite_cjson_t code;
	lite_cjson_t data;
}iotx_dmsg_response_payload_t;

typedef struct {
	int msgid;
	int devid;
	const char *service_prefix;
	const char *service_name;
	char product_key[PRODUCT_KEY_MAXLEN];
	char device_name[DEVICE_NAME_MAXLEN];
	char *params;
	char *method;
}iotx_dmsg_request_t;

typedef struct {
	const char *service_prefix;
	const char *service_name;
	char product_key[PRODUCT_KEY_MAXLEN];
	char device_name[DEVICE_NAME_MAXLEN];
	iotx_dm_error_code_t code;
}iotx_dmsg_response_t;

typedef struct {
	int id;
}iotx_dmsg_ctx_t;

int iotx_dmsg_init(void);
int iotx_dmsg_deinit(void);
int iotx_dmsg_get_id(void);
int _iotx_dmsg_send_to_user(iotx_dm_event_types_t type, char *message);
int iotx_dmsg_send_msg_timeout_to_user(int msg_id, int devid, iotx_dm_event_types_t type);
int iotx_dmsg_uri_parse_pkdn(_IN_ char *uri, _IN_ int uri_len, _IN_ int start_deli, _IN_ int end_deli, _OU_ char product_key[PRODUCT_KEY_MAXLEN], _OU_ char device_name[DEVICE_NAME_MAXLEN]);
int iotx_dmsg_request_parse(_IN_ char *payload, _IN_ int payload_len, _OU_ iotx_dmsg_request_payload_t *request);
int iotx_dmsg_response_parse(_IN_ char *payload, _IN_ int payload_len, _OU_ iotx_dmsg_response_payload_t *response);
int iotx_dmsg_request(_IN_ iotx_dmsg_request_t *request);
int iotx_dmsg_response_with_data(_IN_ iotx_dmsg_request_payload_t *request, _IN_ iotx_dmsg_response_t *response, _IN_ char *data, _IN_ int data_len);
int iotx_dmsg_response_without_data(_IN_ iotx_dmsg_request_payload_t *request, _IN_ iotx_dmsg_response_t *response);
int iotx_dmsg_property_set(int devid,iotx_dmsg_request_payload_t *request);
int iotx_dmsg_topo_add_notify(_IN_ char *payload, _IN_ int payload_len);
int iotx_dmsg_thing_service_request(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN], char *identifier, int identifier_len, iotx_dmsg_request_payload_t *request);
int iotx_dmsg_thing_disable(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN]);
int iotx_dmsg_thing_enable(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN]);
int iotx_dmsg_thing_delete(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN]);
int iotx_dmsg_thing_model_down_raw(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN], _IN_ char *payload, _IN_ int payload_len);
int iotx_dmsg_thing_gateway_permit(_IN_ char *payload, _IN_ int payload_len);
int iotx_dmsg_thing_sub_register_reply(iotx_dmsg_response_payload_t *response);
int iotx_dmsg_thing_sub_unregister_reply(iotx_dmsg_response_payload_t *response);
int iotx_dmsg_thing_topo_add_reply(iotx_dmsg_response_payload_t *response);
int iotx_dmsg_thing_topo_delete_reply(iotx_dmsg_response_payload_t *response);
int iotx_dmsg_topo_get_reply(iotx_dmsg_response_payload_t *response);
int iotx_dmsg_thing_list_found_reply(iotx_dmsg_response_payload_t *response);
int iotx_dmsg_thing_event_property_post_reply(iotx_dmsg_response_payload_t *response);
int iotx_dmsg_thing_event_post_reply(_IN_ char *identifier, _IN_ int identifier_len, _IN_ iotx_dmsg_response_payload_t *response);
int iotx_dmsg_thing_deviceinfo_update_reply(iotx_dmsg_response_payload_t *response);
int iotx_dmsg_thing_deviceinfo_delete_reply(iotx_dmsg_response_payload_t *response);
int iotx_dmsg_thing_dsltemplate_get_reply(iotx_dmsg_response_payload_t *response);
int iotx_dmsg_thing_dynamictsl_get_reply(iotx_dmsg_response_payload_t *response);
int iotx_dmsg_combine_login_reply(iotx_dmsg_response_payload_t *response);
int iotx_dmsg_combine_logout_reply(iotx_dmsg_response_payload_t *response);
int iotx_dmsg_thing_model_up_raw_reply(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN],iotx_dmsg_response_payload_t *response);
int iotx_dmsg_cloud_connected(void);
int iotx_dmsg_cloud_disconnect(void);
int iotx_dmsg_cloud_reconnect(void);
int iotx_dmsg_found_device(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN]);
int iotx_dmsg_remove_device(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN]);
int iotx_dmsg_register_result(_IN_ char *uri,_IN_ int result);
int iotx_dmsg_unregister_result(_IN_ char *uri,_IN_ int result);
int iotx_dmsg_send_result(_IN_ char *uri,_IN_ int result);
int iotx_dmsg_add_service_result(_IN_ char *uri,_IN_ int result);
int iotx_dmsg_remove_service_result(_IN_ char *uri,_IN_ int result);

/* TODO */
int iotx_dmsg_thing_sub_register(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN], _OU_ iotx_dmsg_request_t *request);
int iotx_dmsg_thing_sub_unregister(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN], _OU_ iotx_dmsg_request_t *request);
int iotx_dmsg_thing_topo_add(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN], _IN_ char device_secret[DEVICE_SECRET_MAXLEN], _OU_ iotx_dmsg_request_t *request);
int iotx_dmsg_thing_topo_delete(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN], _OU_ iotx_dmsg_request_t *request);
int iotx_dmsg_thing_topo_get(_OU_ iotx_dmsg_request_t *request);
int iotx_dmsg_thing_list_found(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN], _OU_ iotx_dmsg_request_t *request);
int iotx_dmsg_thing_property_post(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN], _IN_ char *payload, _OU_ iotx_dmsg_request_t *request);
int iotx_dmsg_thing_event_post(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN], _IN_ char *method, _IN_ char *payload, _OU_ iotx_dmsg_request_t *request);
int iotx_dmsg_thing_deviceinfo_update(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN], _IN_ char *payload, _OU_ iotx_dmsg_request_t *request);
int iotx_dmsg_thing_deviceinfo_delete(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN], _IN_ char *payload, _OU_ iotx_dmsg_request_t *request);
int iotx_dmsg_thing_dsltemplate_get(_OU_ iotx_dmsg_request_t *request);
int iotx_dmsg_thing_dynamictsl_get(_OU_ iotx_dmsg_request_t *request);
int iotx_dmsg_combine_login(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN], _IN_ char device_secret[DEVICE_SECRET_MAXLEN], _OU_ iotx_dmsg_request_t *request);
int iotx_dmsg_combine_logout(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN], _OU_ iotx_dmsg_request_t *request);

#endif
