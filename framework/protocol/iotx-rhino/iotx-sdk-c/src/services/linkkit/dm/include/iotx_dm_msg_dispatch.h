#ifndef _IOTX_DM_MSG_DISPATCH_H_
#define _IOTX_DM_MSG_DISPATCH_H_

#include "lite-list.h"
#include "iot_export_cm.h"
#include "iot_export_dm.h"

#define IOTX_DCS_SERVICE_DELIMITER '/'

extern const char IOTX_DCS_SYS_PREFIX[]          DM_READ_ONLY;
extern const char IOTX_DCS_EXT_SESSION_PREFIX[]  DM_READ_ONLY;
extern const char IOTX_DCS_REPLY_SUFFIX[]        DM_READ_ONLY;

/* From Cloud To Local Request And Response*/
extern const char IOTX_DCS_THING_TOPO_ADD_NOTIFY[]             DM_READ_ONLY;
extern const char IOTX_DCS_THING_TOPO_ADD_NOTIFY_REPLY[]       DM_READ_ONLY;
extern const char IOTX_DCS_THING_SERVICE_PROPERTY_SET[]        DM_READ_ONLY;
extern const char IOTX_DCS_THING_SERVICE_PROPERTY_SET_REPLY[]  DM_READ_ONLY;
extern const char IOTX_DCS_THING_SERVICE_PROPERTY_GET[]        DM_READ_ONLY;
extern const char IOTX_DCS_THING_SERVICE_REQUEST[]             DM_READ_ONLY;
extern const char IOTX_DCS_THING_SERVICE_RESPONSE[]            DM_READ_ONLY;
extern const char IOTX_DCS_THING_DISABLE[]                     DM_READ_ONLY;
extern const char IOTX_DCS_THING_DISABLE_REPLY[]               DM_READ_ONLY;
extern const char IOTX_DCS_THING_ENABLE[]                      DM_READ_ONLY;
extern const char IOTX_DCS_THING_ENABLE_REPLY[]                DM_READ_ONLY;
extern const char IOTX_DCS_THING_DELETE[]                      DM_READ_ONLY;
extern const char IOTX_DCS_THING_DELETE_REPLY[]                DM_READ_ONLY;
extern const char IOTX_DCS_THING_MODEL_DOWN_RAW[]              DM_READ_ONLY;
extern const char IOTX_DCS_THING_MODEL_DOWN_RAW_REPLY[]        DM_READ_ONLY;
extern const char IOTX_DCS_THING_GATEWAY_PERMIT[]              DM_READ_ONLY;
extern const char IOTX_DCS_THING_GATEWAY_PERMIT_REPLY[]        DM_READ_ONLY;

/* From Local To Cloud Request And Response*/
extern const char IOTX_DCS_THING_SUB_REGISTER[]                DM_READ_ONLY;
extern const char IOTX_DCS_THING_SUB_REGISTER_REPLY[]          DM_READ_ONLY;
extern const char IOTX_DCS_THING_SUB_UNREGISTER[]              DM_READ_ONLY;
extern const char IOTX_DCS_THING_SUB_UNREGISTER_REPLY[]        DM_READ_ONLY;
extern const char IOTX_DCS_THING_TOPO_ADD[]                    DM_READ_ONLY;
extern const char IOTX_DCS_THING_TOPO_ADD_REPLY[]              DM_READ_ONLY;
extern const char IOTX_DCS_THING_TOPO_DELETE[]                 DM_READ_ONLY;
extern const char IOTX_DCS_THING_TOPO_DELETE_REPLY[]           DM_READ_ONLY;
extern const char IOTX_DCS_THING_TOPO_GET[]                    DM_READ_ONLY;
extern const char IOTX_DCS_THING_TOPO_GET_REPLY[]              DM_READ_ONLY;
extern const char IOTX_DCS_THING_LIST_FOUND[]                  DM_READ_ONLY;
extern const char IOTX_DCS_THING_LIST_FOUND_REPLY[]            DM_READ_ONLY;
extern const char IOTX_DCS_THING_EVENT_PROPERTY_POST[]         DM_READ_ONLY;
extern const char IOTX_DCS_THING_EVENT_PROPERTY_POST_REPLY[]   DM_READ_ONLY;
extern const char IOTX_DCS_THING_EVENT_POST[]                  DM_READ_ONLY;
extern const char IOTX_DCS_THING_EVENT_POST_REPLY[]            DM_READ_ONLY;
extern const char IOTX_DCS_THING_DEVICEINFO_UPDATE[]           DM_READ_ONLY;
extern const char IOTX_DCS_THING_DEVICEINFO_UPDATE_REPLY[]     DM_READ_ONLY;
extern const char IOTX_DCS_THING_DEVICEINFO_DELETE[]           DM_READ_ONLY;
extern const char IOTX_DCS_THING_DEVICEINFO_DELETE_REPLY[]     DM_READ_ONLY;
extern const char IOTX_DCS_THING_DSLTEMPLATE_GET[]             DM_READ_ONLY;
extern const char IOTX_DCS_THING_DSLTEMPLATE_GET_REPLY[]       DM_READ_ONLY;
extern const char IOTX_DCS_THING_DYNAMICTSL_GET[]              DM_READ_ONLY;
extern const char IOTX_DCS_THING_DYNAMICTSL_GET_REPLY[]        DM_READ_ONLY;
extern const char IOTX_DCS_COMBINE_LOGIN[]                     DM_READ_ONLY;
extern const char IOTX_DCS_COMBINE_LOGIN_REPLY[]               DM_READ_ONLY;
extern const char IOTX_DCS_COMBINE_LOGOUT[]                    DM_READ_ONLY;
extern const char IOTX_DCS_COMBINE_LOGOUT_REPLY[]              DM_READ_ONLY;
extern const char IOTX_DCS_THING_MODEL_UP_RAW[]                DM_READ_ONLY;
extern const char IOTX_DCS_THING_MODEL_UP_RAW_REPLY[]          DM_READ_ONLY;
extern const char IOTX_DCS_DEV_CORE_SERVICE_DEV[]              DM_READ_ONLY;
extern const char IOTX_DCS_THING_LAN_PREFIX_GET[]              DM_READ_ONLY;
extern const char IOTX_DCS_THING_LAN_PREFIX_GET_REPLY[]        DM_READ_ONLY;
extern const char IOTX_DCS_THING_LAN_PREFIX_UPDATE[]           DM_READ_ONLY;
extern const char IOTX_DCS_THING_LAN_PREFIX_UPDATE_REPLY[]     DM_READ_ONLY;

typedef void (*iotx_dcs_topic_handler)(iotx_cm_send_peer_t* source, iotx_cm_message_info_t* msg, void* user_data);

typedef struct {
	const char *service_name;
	const char *service_prefix;
	int dev_type;
	int service_type;
	iotx_dcs_topic_handler service_handler;
}iotx_dcs_topic_mapping_t;

typedef struct {
	const char *service_name;
	const char *service_prefix;
	int dev_type;
	int service_type;
	iotx_dcs_topic_handler service_handler;
}iotx_dcs_resource_mapping_t;

typedef struct {
	iotx_cm_event_types_t type;
	iotx_cm_event_handle_fp_t handler;
}iotx_dcs_event_mapping_t;

typedef struct {
	void *mutex;
	void *cloud_connectivity;
	void *local_connectivity;
}iotx_dcs_ctx_t;

int iotx_dcs_init(void);
int iotx_dcs_deinit(void);

iotx_dcs_resource_mapping_t* iotx_dcs_get_resource_mapping(void);
int iotx_dcs_get_resource_mapping_size(void);
iotx_dcs_topic_mapping_t* iotx_dcs_get_topic_mapping(void);
int iotx_dcs_get_topic_mapping_size(void);
int iotx_dcs_get_topic_mapping_dev_type_mask(_IN_ int index, _OU_ int *dev_type_mask);
iotx_dcs_event_mapping_t* iotx_dcs_get_event_mapping(void);
int iotx_dcs_get_event_mapping_size(void);

void iotx_dcs_msg_tick(void);
int iotx_dcs_uri_prefix_sys_split(_IN_ char *uri, _IN_ int uri_len, _OU_ int *start, _OU_ int *end);
int iotx_dcs_uri_prefix_ext_session_split(_IN_ char *uri, _IN_ int uri_len, _OU_ int *start, _OU_ int *end);
int iotx_dcs_uri_pkdn_split(_IN_ char *uri, _IN_ int uri_len, _OU_ int *start, _OU_ int *end);
int iotx_dcs_uri_service_specific_split(_IN_ char *uri, _IN_ int uri_len, _OU_ int *start, _OU_ int *end);
int iotx_dcs_uri_event_specific_split(_IN_ char *uri, _IN_ int uri_len, _OU_ int *start, _OU_ int *end);

int iotx_dcs_topic_subscribe(char *uri);
int iotx_dcs_topic_generic_subscribe(int devid, int index);
int iotx_dcs_topic_service_event_subscribe(int devid, int index);
int iotx_dcs_topic_service_event_create(int devid);
int iotx_dcs_topic_service_event_destroy(int devid);
int iotx_dcs_topic_generic_unsubscribe(int devid);
int iotx_dcs_topic_service_event_unsubscribe(int devid);

void iotx_dcs_thing_topo_add_notify(iotx_cm_send_peer_t* source, iotx_cm_message_info_t* msg, void* user_data);
void iotx_dcs_thing_service_property_set(iotx_cm_send_peer_t* source, iotx_cm_message_info_t* msg, void* user_data);
void iotx_dcs_thing_service_property_get(iotx_cm_send_peer_t* source, iotx_cm_message_info_t* msg, void* user_data);
void iotx_dcs_thing_service_request(iotx_cm_send_peer_t* source, iotx_cm_message_info_t* msg, char *identifier, int identifier_len, void* user_data);
void iotx_dcs_thing_disable(iotx_cm_send_peer_t* source, iotx_cm_message_info_t* msg, void* user_data);
void iotx_dcs_thing_enable(iotx_cm_send_peer_t* source, iotx_cm_message_info_t* msg, void* user_data);
void iotx_dcs_thing_delete(iotx_cm_send_peer_t* source, iotx_cm_message_info_t* msg, void* user_data);
void iotx_dcs_thing_model_down_raw(iotx_cm_send_peer_t* source, iotx_cm_message_info_t* msg, void* user_data);
void iotx_dcs_thing_gateway_permit(iotx_cm_send_peer_t* source, iotx_cm_message_info_t* msg, void* user_data);
void iotx_dcs_thing_sub_register_reply(iotx_cm_send_peer_t* source, iotx_cm_message_info_t* msg, void* user_data);
void iotx_dcs_thing_sub_unregister_reply(iotx_cm_send_peer_t* source, iotx_cm_message_info_t* msg, void* user_data);
void iotx_dcs_thing_topo_add_reply(iotx_cm_send_peer_t* source, iotx_cm_message_info_t* msg, void* user_data);
void iotx_dcs_thing_topo_delete_reply(iotx_cm_send_peer_t* source, iotx_cm_message_info_t* msg, void* user_data);
void iotx_dcs_thing_topo_get_reply(iotx_cm_send_peer_t* source, iotx_cm_message_info_t* msg, void* user_data);
void iotx_dcs_thing_list_found_reply(iotx_cm_send_peer_t* source, iotx_cm_message_info_t* msg, void* user_data);
void iotx_dcs_thing_event_property_post_reply(iotx_cm_send_peer_t* source, iotx_cm_message_info_t* msg, void* user_data);
void iotx_dcs_thing_event_post_reply(iotx_cm_send_peer_t* source, iotx_cm_message_info_t* msg, char *identifier, int identifier_len, void* user_data);
void iotx_dcs_thing_deviceinfo_update_reply(iotx_cm_send_peer_t* source, iotx_cm_message_info_t* msg, void* user_data);
void iotx_dcs_thing_deviceinfo_delete_reply(iotx_cm_send_peer_t* source, iotx_cm_message_info_t* msg, void* user_data);
void iotx_dcs_thing_dsltemplate_get_reply(iotx_cm_send_peer_t* source, iotx_cm_message_info_t* msg, void* user_data);
void iotx_dcs_thing_dynamictsl_get_reply(iotx_cm_send_peer_t* source, iotx_cm_message_info_t* msg, void* user_data);
void iotx_dcs_combine_login_reply(iotx_cm_send_peer_t* source, iotx_cm_message_info_t* msg, void* user_data);
void iotx_dcs_combine_logout_reply(iotx_cm_send_peer_t* source, iotx_cm_message_info_t* msg, void* user_data);
void iotx_dcs_thing_model_up_raw_reply(iotx_cm_send_peer_t* source, iotx_cm_message_info_t* msg, void* user_data);
void iotx_dcs_thing_dev_core_service_dev(iotx_cm_send_peer_t* source, iotx_cm_message_info_t* msg, void* user_data);
void iotx_dcs_thing_lan_prefix_get_reply(iotx_cm_send_peer_t* source, iotx_cm_message_info_t* msg, void* user_data);
void iotx_dcs_thing_lan_prefix_update_reply(iotx_cm_send_peer_t* source, iotx_cm_message_info_t* msg, void* user_data);

void iotx_dcs_event_cloud_connected_handler(void* pcontext, iotx_cm_event_msg_t* msg, void* user_data);
void iotx_dcs_event_cloud_disconnect_handler(void* pcontext, iotx_cm_event_msg_t* msg, void* user_data);
void iotx_dcs_event_cloud_reconnect_handler(void* pcontext, iotx_cm_event_msg_t* msg, void* user_data);
void iotx_dcs_event_local_connected_handler(void* pcontext, iotx_cm_event_msg_t* msg, void* user_data);
void iotx_dcs_event_local_disconnect_handler(void* pcontext, iotx_cm_event_msg_t* msg, void* user_data);
void iotx_dcs_event_local_reconnect_handler(void* pcontext, iotx_cm_event_msg_t* msg, void* user_data);
void iotx_dcs_event_found_device_handler(void* pcontext, iotx_cm_event_msg_t* msg, void* user_data);
void iotx_dcs_event_remove_device_handler(void* pcontext, iotx_cm_event_msg_t* msg, void* user_data);
void iotx_dcs_event_register_result_handler(void* pcontext, iotx_cm_event_msg_t* msg, void* user_data);
void iotx_dcs_event_unregister_result_handler(void* pcontext, iotx_cm_event_msg_t* msg, void* user_data);
void iotx_dcs_event_send_result_handler(void* pcontext, iotx_cm_event_msg_t* msg, void* user_data);
void iotx_dcs_event_add_service_result_handler(void* pcontext, iotx_cm_event_msg_t* msg, void* user_data);
void iotx_dcs_event_remove_service_result_handler(void* pcontext, iotx_cm_event_msg_t* msg, void* user_data);
void iotx_dcs_event_new_data_received_handler(void* pcontext, iotx_cm_event_msg_t* msg, void* user_data);
void iotx_dcs_user_defined_handler(iotx_cm_send_peer_t* source, iotx_cm_message_info_t* msg, void* user_data);

int iotx_dcs_thing_sub_register(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN]);
int iotx_dcs_thing_sub_unregister(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN]);
int iotx_dcs_thing_topo_add(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN], _IN_ char device_secret[DEVICE_SECRET_MAXLEN]);
int iotx_dcs_thing_topo_delete(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN]);
int iotx_dcs_thing_topo_get(void);
int iotx_dcs_thing_list_found(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN]);
int iotx_dcs_thing_property_post(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN], _IN_ char *payload, _IN_ int payload_len);
int iotx_dcs_thing_event_post(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN], _IN_ char *identifier, _IN_ int identifier_len, _IN_ char *method, _IN_ char *payload, _IN_ int payload_len);
int iotx_dcs_thing_deviceinfo_update(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN], _IN_ char *payload, _IN_ int payload_len);
int iotx_dcs_thing_deviceinfo_delete(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN], _IN_ char *payload, _IN_ int payload_len);
int iotx_dcs_thing_dsltemplate_get(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN]);
int iotx_dcs_thing_dynamictsl_get(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN]);
int iotx_dcs_combine_login(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN], _IN_ char device_secret[DEVICE_SECRET_MAXLEN]);
int iotx_dcs_combine_logout(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN]);
int iotx_dcs_thing_model_up_raw(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN], _IN_ char *payload, _IN_ int payload_len);
int iotx_dcs_thing_service_response(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN], _IN_ int msgid, _IN_ iotx_dm_error_code_t code, _IN_ char *identifier, _IN_ int identifier_len, _IN_ char *payload, _IN_ int payload_len);

#endif
