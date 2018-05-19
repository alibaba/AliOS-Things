#ifndef DM_THING_MANAGER_H
#define DM_THING_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "iot_export.h"
#include "iot_export_cm.h"
#include "iot_export_dm.h"
#include "iot_import.h"

#define DM_THING_MANAGER_CLASS get_dm_thing_manager_class()
#define DM_LOCAL_THING_NAME_PATTERN "lthing_%d"
#define DM_SUB_THING_NAME_PATTERN    "subthing_%d"
#define DM_REQUEST_VERSION_STRING    "1.0"

#define METHOD_NAME_SUB_REGISTER            "thing/sub/register"
#define METHOD_NAME_SUB_REGISTER_REPLY      "thing/sub/register_reply"
#define METHOD_NAME_SUB_UNREGISTER          "thing/sub/unregister"
#define METHOD_NAME_SUB_UNREGISTER_REPLY    "thing/sub/unregister_reply"
#define METHOD_NAME_SUB_LOGIN               "combine/login"
#define METHOD_NAME_SUB_LOGIN_REPLY         "combine/login_reply"
#define METHOD_NAME_LOGIN                   "thing/login"
#define METHOD_NAME_LOGIN_REPLY             "thing/login_reply"
#define METHOD_NAME_LOGOUT                  "thing/logout"
#define METHOD_NAME_LOGOUT_REPLY            "thing/logout_reply"
#define METHOD_NAME_TOPO_ADD                "thing/topo/add"
#define METHOD_NAME_TOPO_ADD_REPLY          "thing/topo/add_reply"
#define METHOD_NAME_TOPO_DEL                "thing/topo/delete"
#define METHOD_NAME_TOPO_DEL_REPLY          "thing/topo/delete_reply"
#define METHOD_NAME_THING_ENABLE            "thing/enable"
#define METHOD_NAME_THING_ENABLE_REPLY      "thing/enable_reply"
#define METHOD_NAME_THING_DELETE            "thing/delete"
#define METHOD_NAME_THING_DELETE_REPLY      "thing/delete_reply"
#define METHOD_NAME_THING_DISABLE           "thing/disable"
#define METHOD_NAME_THING_DISABLE_REPLY     "thing/disable_reply"
#define METHOD_NAME_THING_TSL_POST          "thing/keyelement/post"
#define METHOD_NAME_THING_TSL_POST_REPLY    "thing/keyelement/post_reply"
#define METHOD_NAME_THING_TSL_GET           "thing/dsltemplate/get"
#define METHOD_NAME_THING_TSL_GET_REPLY     "thing/dsltemplate/get_reply"
#define METHOD_NAME_PROPERTY_POST           "thing/event/property/post"
#define METHOD_NAME_PROPERTY_POST_REPLY     "thing/event/property/post_reply"
#define METHOD_NAME_PROPERTY_SET            "thing/service/property/set"
#define METHOD_NAME_PROPERTY_SET_REPLY      "thing/service/property/set_reply"
#define METHOD_NAME_PROPERTY_GET            "thing/service/property/get"
#define METHOD_NAME_PROPERTY_GET_REPLY      "thing/service/property/get_reply"
#define METHOD_NAME_DOWN_RAW                "thing/model/down_raw"
#define METHOD_NAME_DOWN_RAW_PEPLY          "thing/model/down_raw_reply"
#define METHOD_NAME_UP_RAW                  "thing/model/up_raw"
#define METHOD_NAME_UP_RAW_REPLY            "thing/model/up_raw_reply"

#ifdef LOCAL_CONN_ENABLE
#define METHOD_NAME_LAN_PREFIX_GET          "thing/lan/prefix/get"
#define METHOD_NAME_LAN_PREFIX_GET_REPLY    "thing/lan/prefix/get_reply"
#define METHOD_NAME_LAN_PREFIX_UPDATE       "thing/lan/prefix/update"
#define METHOD_NAME_LAN_PREFIX_UPDATE_REPLY "thing/lan/prefix/update_reply"
#endif /* LOCAL_CONN_ENABLE */

#ifdef EXTENDED_INFO_ENABLED
#define METHOD_NAME_DEVICEINFO_UPDATE       "thing/deviceinfo/update"
#define METHOD_NAME_DEVICEINFO_UPDATE_REPLY "thing/deviceinfo/update_reply"
#define METHOD_NAME_DEVICEINFO_DELETE       "thing/deviceinfo/delete"
#define METHOD_NAME_DEVICEINFO_DELETE_REPLY "thing/deviceinfo/delete_reply"
#endif /* EXTENDED_INFO_ENABLED*/
#ifdef RRPC_ENABLED
#define METHOD_NAME_RRPC_REQUEST_PLUS       "rrpc/request/+"
#define METHOD_NAME_RRPC_REQUEST            "rrpc/request"
#endif /* RRPC_ENABLED */

#define METHOD_MAX_LENGH 128
#define URI_MAX_LENGH 256

typedef struct {
    const  void* _;
    char*  _name; /* dm thing manager object name. */
    void*  _local_thing_list; /* local thing list. */
    void*  _local_thing_name_list; /* local thing list. */
    void*  _sub_thing_list; /* sub thing list. currently not use. */
#ifdef SUBDEV_ENABLE
    void*  _sub_thing_name_list; /* sub thing name list. */
    void*  _subdev_callback_list; /* subdev callback function list */
    void*  _sub_thing_id;
    int    _sub_thing_cnt;
    char*  _message;
    dm_subdev_callback_type_t _subcallback_type;
    void*  _topo_add_request_list;
    void*  _subdev_login_request_list;
    void*  _request_info;
#endif
    void*  _callback_list; /* callback function list */
    void*  _service_property_get_identifier_list; /* identifier list when method=thing.service.property.get */
    void*  _ota;
    void*  _thing_id;
    void*  _identifier;
    void*  _property_identifier_post; /* used when event = thing.event.property.post */
    void*  _property_identifier_set; /* used when event = thing.service.property.set */
    void*  _property_identifier_value_set; /* used when event = thing.service.property.set */
    void*  _service_identifier_requested; /* service identifier when requested. */
    void*  _get_value;
    void*  _set_value;
    char*  _set_value_str;
    char*  _get_value_str;
    void*  _message_info;
    void*  _cm;
    int    _local_thing_id;
    int    _ret;
    int    _code;
    char*  _dm_version;
    int    _id;
    int    _current_id;
    int    _usr_response_id;
    int    _response_id_received;
    int    _request_id;
    char*  _response_message;
    void*  _raw_data;
    int    _raw_data_length;
    void*  _param_data;
    int    _param_data_length;
    char*  _method;
    int    _cloud_connected;
#ifdef LOCAL_CONN_ENABLE
    int    _local_connected;
#endif
    int    _get_tsl_from_cloud;
    int    _destructing;
#ifdef RRPC_ENABLED
    int    _rrpc;
    int    _rrpc_message_id;
#endif /* RRPC_ENABLED */
    dm_callback_type_t _callback_type;
    dm_cloud_domain_type_t _cloud_domain;
    iotx_cm_event_handle_fp_t _cm_event_handle_func_fp;
    iotx_cm_register_fp_t _cm_register_func_fp;
#ifdef LOCAL_CONN_ENABLE
    iotx_cm_register_fp_t _cm_local_conn_register_func_fp;
    void*   _conn_ctx;
#endif
    char  _device_name[DEVICE_NAME_MAXLEN];
    char  _product_key[PRODUCT_KEY_MAXLEN];
    char  _device_secret[DEVICE_SECRET_MAXLEN];
    char  _device_id[DEVICE_ID_MAXLEN];
} dm_thing_manager_t;

extern const void* get_dm_thing_manager_class();

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DM_THING_MANAGER_H */
