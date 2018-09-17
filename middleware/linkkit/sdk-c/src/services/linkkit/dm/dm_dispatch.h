/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#ifndef _DM_DISPATCH_H_
#define _DM_DISPATCH_H_

#include "iotx_dm_internal.h"
#include "iotx_cm.h"

#define DM_DISP_SERVICE_DELIMITER '/'

extern const char DM_DISP_SYS_PREFIX[]          DM_READ_ONLY;
extern const char DM_DISP_EXT_SESSION_PREFIX[]  DM_READ_ONLY;
extern const char DM_DISP_EXT_NTP_PREFIX[]      DM_READ_ONLY;
extern const char DM_DISP_REPLY_SUFFIX[]        DM_READ_ONLY;

extern const char DM_DISP_RRPC_RESPONSE_OLD_VERSION[]         DM_READ_ONLY;
extern const char DM_DISP_RRPC_RESPONSE_NEW_VERSION[]         DM_READ_ONLY;

/* From Cloud To Local Request And Response*/
extern const char DM_DISP_THING_SERVICE_PROPERTY_SET[]        DM_READ_ONLY;
extern const char DM_DISP_THING_SERVICE_PROPERTY_SET_REPLY[]  DM_READ_ONLY;
extern const char DM_DISP_THING_SERVICE_PROPERTY_GET[]        DM_READ_ONLY;
extern const char DM_DISP_THING_SERVICE_REQUEST_WILDCARD[]    DM_READ_ONLY;
extern const char DM_DISP_THING_SERVICE_REQUEST[]             DM_READ_ONLY;
extern const char DM_DISP_THING_SERVICE_RESPONSE[]            DM_READ_ONLY;
extern const char DM_DISP_THING_MODEL_DOWN_RAW[]              DM_READ_ONLY;
extern const char DM_DISP_THING_MODEL_DOWN_RAW_REPLY[]        DM_READ_ONLY;
#ifdef CONFIG_DM_DEVTYPE_GATEWAY
extern const char DM_DISP_THING_TOPO_ADD_NOTIFY[]             DM_READ_ONLY;
extern const char DM_DISP_THING_TOPO_ADD_NOTIFY_REPLY[]       DM_READ_ONLY;
extern const char DM_DISP_THING_DISABLE[]                     DM_READ_ONLY;
extern const char DM_DISP_THING_DISABLE_REPLY[]               DM_READ_ONLY;
extern const char DM_DISP_THING_ENABLE[]                      DM_READ_ONLY;
extern const char DM_DISP_THING_ENABLE_REPLY[]                DM_READ_ONLY;
extern const char DM_DISP_THING_DELETE[]                      DM_READ_ONLY;
extern const char DM_DISP_THING_DELETE_REPLY[]                DM_READ_ONLY;
extern const char DM_DISP_THING_GATEWAY_PERMIT[]              DM_READ_ONLY;
extern const char DM_DISP_THING_GATEWAY_PERMIT_REPLY[]        DM_READ_ONLY;
#endif
/* From Local To Cloud Request And Response*/
extern const char DM_DISP_THING_EVENT_PROPERTY_POST[]         DM_READ_ONLY;
extern const char DM_DISP_THING_EVENT_PROPERTY_POST_REPLY[]   DM_READ_ONLY;
extern const char DM_DISP_THING_EVENT_POST[]                  DM_READ_ONLY;
extern const char DM_DISP_THING_EVENT_POST_REPLY[]            DM_READ_ONLY;
extern const char DM_DISP_THING_EVENT_POST_REPLY_WILDCARD[]   DM_READ_ONLY;
extern const char DM_DISP_THING_DEVICEINFO_UPDATE[]           DM_READ_ONLY;
extern const char DM_DISP_THING_DEVICEINFO_UPDATE_REPLY[]     DM_READ_ONLY;
extern const char DM_DISP_THING_DEVICEINFO_DELETE[]           DM_READ_ONLY;
extern const char DM_DISP_THING_DEVICEINFO_DELETE_REPLY[]     DM_READ_ONLY;
extern const char DM_DISP_THING_DSLTEMPLATE_GET[]             DM_READ_ONLY;
extern const char DM_DISP_THING_DSLTEMPLATE_GET_REPLY[]       DM_READ_ONLY;
extern const char DM_DISP_THING_DYNAMICTSL_GET[]              DM_READ_ONLY;
extern const char DM_DISP_THING_DYNAMICTSL_GET_REPLY[]        DM_READ_ONLY;
extern const char DM_DISP_THING_MODEL_UP_RAW[]                DM_READ_ONLY;
extern const char DM_DISP_THING_MODEL_UP_RAW_REPLY[]          DM_READ_ONLY;
extern const char DM_DISP_NTP_REQUEST[]                       DM_READ_ONLY;
extern const char DM_DISP_NTP_RESPONSE[]                      DM_READ_ONLY;
extern const char DM_DISP_DEV_CORE_SERVICE_DEV[]              DM_READ_ONLY;
extern const char DM_DISP_THING_LAN_PREFIX_GET[]              DM_READ_ONLY;
extern const char DM_DISP_THING_LAN_PREFIX_GET_REPLY[]        DM_READ_ONLY;
extern const char DM_DISP_THING_LAN_PREFIX_UPDATE[]           DM_READ_ONLY;
extern const char DM_DISP_THING_LAN_PREFIX_UPDATE_REPLY[]     DM_READ_ONLY;
extern const char DM_DISP_THING_LAN_PREFIX_UPDATE_REPLY[]     DM_READ_ONLY;
extern const char DM_DISP_THING_LAN_BLACKLIST_UPDATE[]        DM_READ_ONLY;
extern const char DM_DISP_THING_LAN_BLACKLIST_UPDATE_REPLY[]  DM_READ_ONLY;
#ifdef CONFIG_DM_DEVTYPE_GATEWAY
extern const char DM_DISP_THING_SUB_REGISTER[]                DM_READ_ONLY;
extern const char DM_DISP_THING_SUB_REGISTER_REPLY[]          DM_READ_ONLY;
extern const char DM_DISP_THING_SUB_UNREGISTER[]              DM_READ_ONLY;
extern const char DM_DISP_THING_SUB_UNREGISTER_REPLY[]        DM_READ_ONLY;
extern const char DM_DISP_THING_TOPO_ADD[]                    DM_READ_ONLY;
extern const char DM_DISP_THING_TOPO_ADD_REPLY[]              DM_READ_ONLY;
extern const char DM_DISP_THING_TOPO_DELETE[]                 DM_READ_ONLY;
extern const char DM_DISP_THING_TOPO_DELETE_REPLY[]           DM_READ_ONLY;
extern const char DM_DISP_THING_TOPO_GET[]                    DM_READ_ONLY;
extern const char DM_DISP_THING_TOPO_GET_REPLY[]              DM_READ_ONLY;
extern const char DM_DISP_THING_LIST_FOUND[]                  DM_READ_ONLY;
extern const char DM_DISP_THING_LIST_FOUND_REPLY[]            DM_READ_ONLY;
extern const char DM_DISP_COMBINE_LOGIN[]                     DM_READ_ONLY;
extern const char DM_DISP_COMBINE_LOGIN_REPLY[]               DM_READ_ONLY;
extern const char DM_DISP_COMBINE_LOGOUT[]                    DM_READ_ONLY;
extern const char DM_DISP_COMBINE_LOGOUT_REPLY[]              DM_READ_ONLY;
#endif

#define DM_DISP_MULTI_SUBSCRIBE_MAX (1)

typedef void (*dm_disp_topic_handler)(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data);

typedef struct {
    const char *service_name;
    const char *service_prefix;
    int dev_type;
    int service_type;
    dm_disp_topic_handler service_handler;
} dm_disp_topic_mapping_t;

typedef struct {
    const char *service_name;
    const char *service_prefix;
    int dev_type;
    int service_type;
    dm_disp_topic_handler service_handler;
} dm_disp_resource_mapping_t;

typedef struct {
    iotx_cm_event_types_t type;
    iotx_cm_event_handle_fp_t handler;
} dm_disp_event_mapping_t;

dm_disp_topic_mapping_t *dm_disp_get_topic_mapping(void);
int dm_disp_get_topic_mapping_size(void);
int dm_disp_get_topic_mapping_dev_type_mask(_IN_ int index, _OU_ int *dev_type_mask);
dm_disp_event_mapping_t *dm_disp_get_event_mapping(void);
int dm_disp_get_event_mapping_size(void);

void dm_disp_msg_tick(void);
int dm_disp_uri_prefix_sys_split(_IN_ char *uri, _IN_ int uri_len, _OU_ int *start, _OU_ int *end);
int dm_disp_uri_prefix_ext_session_split(_IN_ char *uri, _IN_ int uri_len, _OU_ int *start, _OU_ int *end);
int dm_disp_uri_prefix_ext_ntp_split(_IN_ char *uri, _IN_ int uri_len, _OU_ int *start, _OU_ int *end);
int dm_disp_uri_prefix_ext_error_split(_IN_ char *uri, _IN_ int uri_len, _OU_ int *start, _OU_ int *end);
int dm_disp_uri_pkdn_split(_IN_ char *uri, _IN_ int uri_len, _OU_ int *start, _OU_ int *end);
int dm_disp_uri_service_specific_split(_IN_ char *uri, _IN_ int uri_len, _OU_ int *start, _OU_ int *end);
int dm_disp_uri_rrpc_request_split(_IN_ char *uri, _IN_ int uri_len, _OU_ int *start, _OU_ int *end);
int dm_disp_uri_event_specific_split(_IN_ char *uri, _IN_ int uri_len, _OU_ int *start, _OU_ int *end);


void dm_disp_thing_service_property_set(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data);
void dm_disp_thing_service_property_get(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data);
void dm_disp_thing_service_property_post(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data);
void dm_disp_thing_service_request(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, char *identifier,
                                   int identifier_len, void *user_data);
void dm_disp_rrpc_request(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, char *messageid,
                          int messageid_len, void *user_data);
void dm_disp_thing_model_down_raw(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data);
void dm_disp_thing_event_property_post_reply(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data);
void dm_disp_thing_event_post_reply(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, char *identifier,
                                    int identifier_len, void *user_data);
void dm_disp_thing_deviceinfo_update_reply(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data);
void dm_disp_thing_deviceinfo_delete_reply(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data);
void dm_disp_thing_dsltemplate_get_reply(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data);
void dm_disp_thing_dynamictsl_get_reply(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data);
void dm_disp_thing_model_up_raw_reply(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data);
void dm_disp_rrpc_request_plus(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data);
void dm_disp_ntp_response(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data);
void dm_disp_ext_error_response(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data);
#ifdef CONFIG_DM_DEVTYPE_GATEWAY
void dm_disp_thing_topo_add_notify(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data);
void dm_disp_thing_disable(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data);
void dm_disp_thing_enable(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data);
void dm_disp_thing_delete(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data);
void dm_disp_thing_gateway_permit(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data);
void dm_disp_thing_sub_register_reply(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data);
void dm_disp_thing_sub_unregister_reply(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data);
void dm_disp_thing_topo_add_reply(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data);
void dm_disp_thing_topo_delete_reply(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data);
void dm_disp_thing_topo_get_reply(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data);
void dm_disp_thing_list_found_reply(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data);
void dm_disp_combine_login_reply(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data);
void dm_disp_combine_logout_reply(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data);
#endif
void dm_disp_thing_dev_core_service_dev(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data);
void dm_disp_thing_lan_prefix_get_reply(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data);
void dm_disp_thing_lan_prefix_update_reply(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data);
void dm_disp_thing_lan_blacklist_update_reply(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg,
        void *user_data);

void dm_disp_event_cloud_connected_handler(void *pcontext, iotx_cm_event_msg_t *msg, void *user_data);
void dm_disp_event_cloud_disconnect_handler(void *pcontext, iotx_cm_event_msg_t *msg, void *user_data);
void dm_disp_event_cloud_reconnect_handler(void *pcontext, iotx_cm_event_msg_t *msg, void *user_data);
void dm_disp_event_local_connected_handler(void *pcontext, iotx_cm_event_msg_t *msg, void *user_data);
void dm_disp_event_local_disconnect_handler(void *pcontext, iotx_cm_event_msg_t *msg, void *user_data);
void dm_disp_event_local_reconnect_handler(void *pcontext, iotx_cm_event_msg_t *msg, void *user_data);
void dm_disp_event_found_device_handler(void *pcontext, iotx_cm_event_msg_t *msg, void *user_data);
void dm_disp_event_remove_device_handler(void *pcontext, iotx_cm_event_msg_t *msg, void *user_data);
void dm_disp_event_register_result_handler(void *pcontext, iotx_cm_event_msg_t *msg, void *user_data);
void dm_disp_event_unregister_result_handler(void *pcontext, iotx_cm_event_msg_t *msg, void *user_data);
void dm_disp_event_send_result_handler(void *pcontext, iotx_cm_event_msg_t *msg, void *user_data);
void dm_disp_event_add_service_result_handler(void *pcontext, iotx_cm_event_msg_t *msg, void *user_data);
void dm_disp_event_remove_service_result_handler(void *pcontext, iotx_cm_event_msg_t *msg, void *user_data);
void dm_disp_event_new_data_received_handler(void *pcontext, iotx_cm_event_msg_t *msg, void *user_data);
void dm_disp_user_defined_handler(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data);

#endif
