/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#include "iotx_dm_internal.h"
#include "dm_dispatch.h"
#include "dm_manager.h"
#include "dm_message.h"
#include "dm_cm_wrapper.h"
#include "dm_message_cache.h"
#include "dm_conn.h"
#include "dm_subscribe.h"
#include "dm_opt.h"
#include "dm_ota.h"
#include "dm_cota.h"
#include "dm_fota.h"
#include "iotx_dm.h"

/*****************************Internal Definition*****************************/

const char DM_DISP_SYS_PREFIX[]                        DM_READ_ONLY = "/sys/%s/%s/";
const char DM_DISP_EXT_SESSION_PREFIX[]                DM_READ_ONLY = "/ext/session/%s/%s/";
const char DM_DISP_EXT_NTP_PREFIX[]                    DM_READ_ONLY = "/ext/ntp/%s/%s/";
const char DM_DISP_REPLY_SUFFIX[]                      DM_READ_ONLY = "_reply";
const char DM_DISP_EXT_ERROR_PREFIX[]                  DM_READ_ONLY = "/ext/error/%s/%s/";

const char DM_DISP_RRPC_RESPONSE_OLD_VERSION[]         DM_READ_ONLY =
            "/sys/%.*s/%.*s/rrpc/response/%d"; /* product key, device name, response id */
const char DM_DISP_RRPC_RESPONSE_NEW_VERSION[]         DM_READ_ONLY =
            "/ext/rrpc/%d/sys/%.*s/%.*s/thing/service/%.*s"; /* response id, product key, device name, service id */

const char DM_DISP_RRPC_REQUEST_PLUS[]                 DM_READ_ONLY = "rrpc/request/+";
const char DM_DOSP_RRPC_REQUEST[]                      DM_READ_ONLY = "rrpc/request";

/* From Cloud To Local Request And Response*/
const char DM_DISP_THING_SERVICE_PROPERTY_SET[]        DM_READ_ONLY = "thing/service/property/set";
const char DM_DISP_THING_SERVICE_PROPERTY_SET_REPLY[]  DM_READ_ONLY = "thing/service/property/set_reply";
const char DM_DISP_THING_SERVICE_PROPERTY_GET[]        DM_READ_ONLY = "thing/service/property/get";
const char DM_DISP_THING_SERVICE_PROPERTY_GET_REPLY[]  DM_READ_ONLY = "thing/service/property/get_reply";
const char DM_DISP_THING_SERVICE_REQUEST_WILDCARD[]    DM_READ_ONLY = "thing/service/+";
const char DM_DISP_THING_SERVICE_REQUEST[]             DM_READ_ONLY = "thing/service/%s";
const char DM_DISP_THING_SERVICE_RESPONSE[]            DM_READ_ONLY = "thing/service/%.*s_reply";
const char DM_DISP_THING_MODEL_DOWN_RAW[]              DM_READ_ONLY = "thing/model/down_raw";
const char DM_DISP_THING_MODEL_DOWN_RAW_REPLY[]        DM_READ_ONLY = "thing/model/down_raw_reply";

#ifdef CONFIG_DM_DEVTYPE_GATEWAY
const char DM_DISP_THING_TOPO_ADD_NOTIFY[]             DM_READ_ONLY = "thing/topo/add/notify";
const char DM_DISP_THING_TOPO_ADD_NOTIFY_REPLY[]       DM_READ_ONLY = "thing/topo/add/notify_reply";
const char DM_DISP_THING_DELETE[]                      DM_READ_ONLY = "thing/delete";
const char DM_DISP_THING_DELETE_REPLY[]                DM_READ_ONLY = "thing/delete_reply";
const char DM_DISP_THING_DISABLE[]                     DM_READ_ONLY = "thing/disable";
const char DM_DISP_THING_DISABLE_REPLY[]               DM_READ_ONLY = "thing/disable_reply";
const char DM_DISP_THING_ENABLE[]                      DM_READ_ONLY = "thing/enable";
const char DM_DISP_THING_ENABLE_REPLY[]                DM_READ_ONLY = "thing/enable_reply";
const char DM_DISP_THING_GATEWAY_PERMIT[]              DM_READ_ONLY = "thing/gateway/permit";
const char DM_DISP_THING_GATEWAY_PERMIT_REPLY[]        DM_READ_ONLY = "thing/gateway/permit_reply";
#endif

/* From Local To Cloud Request And Response*/
const char DM_DISP_THING_EVENT_PROPERTY_POST[]         DM_READ_ONLY = "thing/event/property/post";
const char DM_DISP_THING_EVENT_PROPERTY_POST_REPLY[]   DM_READ_ONLY = "thing/event/property/post_reply";
const char DM_DISP_THING_EVENT_POST[]                  DM_READ_ONLY = "thing/event/%.*s/post";
const char DM_DISP_THING_EVENT_POST_REPLY[]            DM_READ_ONLY = "thing/event/%s/post_reply";
const char DM_DISP_THING_EVENT_POST_REPLY_WILDCARD[]   DM_READ_ONLY = "thing/event/+/post_reply";
const char DM_DISP_THING_DEVICEINFO_UPDATE[]           DM_READ_ONLY = "thing/deviceinfo/update";
const char DM_DISP_THING_DEVICEINFO_UPDATE_REPLY[]     DM_READ_ONLY = "thing/deviceinfo/update_reply";
const char DM_DISP_THING_DEVICEINFO_DELETE[]           DM_READ_ONLY = "thing/deviceinfo/delete";
const char DM_DISP_THING_DEVICEINFO_DELETE_REPLY[]     DM_READ_ONLY = "thing/deviceinfo/delete_reply";
const char DM_DISP_THING_DSLTEMPLATE_GET[]             DM_READ_ONLY = "thing/dsltemplate/get";
const char DM_DISP_THING_DSLTEMPLATE_GET_REPLY[]       DM_READ_ONLY = "thing/dsltemplate/get_reply";
const char DM_DISP_THING_DYNAMICTSL_GET[]              DM_READ_ONLY = "thing/dynamicTsl/get";
const char DM_DISP_THING_DYNAMICTSL_GET_REPLY[]        DM_READ_ONLY = "thing/dynamicTsl/get_reply";
const char DM_DISP_THING_MODEL_UP_RAW[]                DM_READ_ONLY = "thing/model/up_raw";
const char DM_DISP_THING_MODEL_UP_RAW_REPLY[]          DM_READ_ONLY = "thing/model/up_raw_reply";
const char DM_DISP_NTP_REQUEST[]                       DM_READ_ONLY = "request";
const char DM_DISP_NTP_RESPONSE[]                      DM_READ_ONLY = "response";
const char DM_DISP_DEV_CORE_SERVICE_DEV[]              DM_READ_ONLY = "/dev/core/service/dev";
const char DM_DISP_THING_LAN_PREFIX_GET[]              DM_READ_ONLY = "thing/lan/prefix/get";
const char DM_DISP_THING_LAN_PREFIX_GET_REPLY[]        DM_READ_ONLY = "thing/lan/prefix/get_reply";
const char DM_DISP_THING_LAN_PREFIX_UPDATE[]           DM_READ_ONLY = "thing/lan/prefix/update";
const char DM_DISP_THING_LAN_PREFIX_UPDATE_REPLY[]     DM_READ_ONLY = "thing/lan/prefix/update_reply";
const char DM_DISP_THING_LAN_BLACKLIST_UPDATE[]        DM_READ_ONLY = "thing/lan/blacklist/update";
const char DM_DISP_THING_LAN_BLACKLIST_UPDATE_REPLY[]  DM_READ_ONLY = "thing/lan/blacklist/update_reply";

#ifdef CONFIG_DM_DEVTYPE_GATEWAY
const char DM_DISP_THING_SUB_REGISTER[]                DM_READ_ONLY = "thing/sub/register";
const char DM_DISP_THING_SUB_REGISTER_REPLY[]          DM_READ_ONLY = "thing/sub/register_reply";
const char DM_DISP_THING_SUB_UNREGISTER[]              DM_READ_ONLY = "thing/sub/unregister";
const char DM_DISP_THING_SUB_UNREGISTER_REPLY[]        DM_READ_ONLY = "thing/sub/unregister_reply";
const char DM_DISP_THING_TOPO_ADD[]                    DM_READ_ONLY = "thing/topo/add";
const char DM_DISP_THING_TOPO_ADD_REPLY[]              DM_READ_ONLY = "thing/topo/add_reply";
const char DM_DISP_THING_TOPO_DELETE[]                 DM_READ_ONLY = "thing/topo/delete";
const char DM_DISP_THING_TOPO_DELETE_REPLY[]           DM_READ_ONLY = "thing/topo/delete_reply";
const char DM_DISP_THING_TOPO_GET[]                    DM_READ_ONLY = "thing/topo/get";
const char DM_DISP_THING_TOPO_GET_REPLY[]              DM_READ_ONLY = "thing/topo/get_reply";
const char DM_DISP_THING_LIST_FOUND[]                  DM_READ_ONLY = "thing/list/found";
const char DM_DISP_THING_LIST_FOUND_REPLY[]            DM_READ_ONLY = "thing/list/found_reply";
const char DM_DISP_COMBINE_LOGIN[]                     DM_READ_ONLY = "combine/login";
const char DM_DISP_COMBINE_LOGIN_REPLY[]               DM_READ_ONLY = "combine/login_reply";
const char DM_DISP_COMBINE_LOGOUT[]                    DM_READ_ONLY = "combine/logout";
const char DM_DISP_COMBINE_LOGOUT_REPLY[]              DM_READ_ONLY = "combine/logout_reply";
#endif

static const dm_disp_topic_mapping_t g_dm_disp_topic_mapping[] DM_READ_ONLY = {
    {DM_DISP_THING_SERVICE_PROPERTY_SET,       DM_DISP_SYS_PREFIX,         IOTX_DM_DEVICE_ALL,     IOTX_DM_SERVICE_ALL,     dm_disp_thing_service_property_set         },
    {DM_DISP_THING_SERVICE_PROPERTY_GET,       DM_DISP_SYS_PREFIX,         IOTX_DM_DEVICE_ALL,     IOTX_DM_SERVICE_ALL,     dm_disp_thing_service_property_get         },
    {DM_DISP_THING_EVENT_PROPERTY_POST,        DM_DISP_SYS_PREFIX,         IOTX_DM_DEVICE_ALL,     IOTX_DM_LOCAL_AUTH,      dm_disp_thing_service_property_post        },
    {DM_DISP_THING_MODEL_DOWN_RAW,             DM_DISP_SYS_PREFIX,         IOTX_DM_DEVICE_ALL,     IOTX_DM_SERVICE_ALL,     dm_disp_thing_model_down_raw               },
    {DM_DISP_THING_EVENT_PROPERTY_POST_REPLY,  DM_DISP_SYS_PREFIX,         IOTX_DM_DEVICE_ALL,     IOTX_DM_SERVICE_ALL,     dm_disp_thing_event_property_post_reply    },
    {DM_DISP_THING_DEVICEINFO_UPDATE_REPLY,    DM_DISP_SYS_PREFIX,         IOTX_DM_DEVICE_ALL,     IOTX_DM_SERVICE_CLOUD,   dm_disp_thing_deviceinfo_update_reply      },
    {DM_DISP_THING_DEVICEINFO_DELETE_REPLY,    DM_DISP_SYS_PREFIX,         IOTX_DM_DEVICE_ALL,     IOTX_DM_SERVICE_CLOUD,   dm_disp_thing_deviceinfo_delete_reply      },
    {DM_DISP_THING_DSLTEMPLATE_GET_REPLY,      DM_DISP_SYS_PREFIX,         IOTX_DM_DEVICE_ALL,     IOTX_DM_SERVICE_CLOUD,   dm_disp_thing_dsltemplate_get_reply        },
    {DM_DISP_THING_DYNAMICTSL_GET_REPLY,       DM_DISP_SYS_PREFIX,         IOTX_DM_DEVICE_ALL,     IOTX_DM_SERVICE_CLOUD,   dm_disp_thing_dynamictsl_get_reply         },
    {DM_DISP_THING_MODEL_UP_RAW_REPLY,         DM_DISP_SYS_PREFIX,         IOTX_DM_DEVICE_ALL,     IOTX_DM_SERVICE_ALL,     dm_disp_thing_model_up_raw_reply           },
    {DM_DISP_RRPC_REQUEST_PLUS,                DM_DISP_SYS_PREFIX,         IOTX_DM_DEVICE_ALL,     IOTX_DM_SERVICE_CLOUD,   dm_disp_rrpc_request_plus                  },
    {DM_DISP_NTP_RESPONSE,                     DM_DISP_EXT_NTP_PREFIX,     IOTX_DM_DEVICE_MAIN,    IOTX_DM_SERVICE_CLOUD,   dm_disp_ntp_response                       },
#ifdef CONFIG_DM_DEVTYPE_GATEWAY
    {DM_DISP_THING_TOPO_ADD_NOTIFY,            DM_DISP_SYS_PREFIX,         IOTX_DM_DEVICE_GATEWAY, IOTX_DM_SERVICE_CLOUD,   dm_disp_thing_topo_add_notify              },
    {DM_DISP_THING_GATEWAY_PERMIT,             DM_DISP_SYS_PREFIX,         IOTX_DM_DEVICE_GATEWAY, IOTX_DM_SERVICE_CLOUD,   dm_disp_thing_gateway_permit               },
    {DM_DISP_THING_SUB_REGISTER_REPLY,         DM_DISP_SYS_PREFIX,         IOTX_DM_DEVICE_GATEWAY, IOTX_DM_SERVICE_CLOUD,   dm_disp_thing_sub_register_reply           },
    {DM_DISP_THING_SUB_UNREGISTER_REPLY,       DM_DISP_SYS_PREFIX,         IOTX_DM_DEVICE_GATEWAY, IOTX_DM_SERVICE_CLOUD,   dm_disp_thing_sub_unregister_reply         },
    {DM_DISP_THING_TOPO_ADD_REPLY,             DM_DISP_SYS_PREFIX,         IOTX_DM_DEVICE_GATEWAY, IOTX_DM_SERVICE_CLOUD,   dm_disp_thing_topo_add_reply               },
    {DM_DISP_THING_TOPO_DELETE_REPLY,          DM_DISP_SYS_PREFIX,         IOTX_DM_DEVICE_GATEWAY, IOTX_DM_SERVICE_CLOUD,   dm_disp_thing_topo_delete_reply            },
    {DM_DISP_THING_TOPO_GET_REPLY,             DM_DISP_SYS_PREFIX,         IOTX_DM_DEVICE_GATEWAY, IOTX_DM_SERVICE_CLOUD,   dm_disp_thing_topo_get_reply               },
    {DM_DISP_THING_LIST_FOUND_REPLY,           DM_DISP_SYS_PREFIX,         IOTX_DM_DEVICE_GATEWAY, IOTX_DM_SERVICE_CLOUD,   dm_disp_thing_list_found_reply             },
    {DM_DISP_COMBINE_LOGIN_REPLY,              DM_DISP_EXT_SESSION_PREFIX, IOTX_DM_DEVICE_GATEWAY, IOTX_DM_SERVICE_CLOUD,   dm_disp_combine_login_reply                },
    {DM_DISP_COMBINE_LOGOUT_REPLY,             DM_DISP_EXT_SESSION_PREFIX, IOTX_DM_DEVICE_GATEWAY, IOTX_DM_SERVICE_CLOUD,   dm_disp_combine_logout_reply               },
    {DM_DISP_THING_DISABLE,                    DM_DISP_SYS_PREFIX,         IOTX_DM_DEVICE_ALL,     IOTX_DM_SERVICE_CLOUD,   dm_disp_thing_disable                      },
    {DM_DISP_THING_ENABLE,                     DM_DISP_SYS_PREFIX,         IOTX_DM_DEVICE_ALL,     IOTX_DM_SERVICE_CLOUD,   dm_disp_thing_enable                       },
    {DM_DISP_THING_DELETE,                     DM_DISP_SYS_PREFIX,         IOTX_DM_DEVICE_ALL,     IOTX_DM_SERVICE_CLOUD,   dm_disp_thing_delete                       },
#endif
#ifndef DEPRECATED_LINKKIT
    {DM_DISP_THING_SERVICE_REQUEST_WILDCARD,   DM_DISP_SYS_PREFIX,         IOTX_DM_DEVICE_ALL,     IOTX_DM_SERVICE_CLOUD,   NULL                                       },
    {DM_DISP_THING_EVENT_POST_REPLY_WILDCARD,  DM_DISP_SYS_PREFIX,         IOTX_DM_DEVICE_ALL,     IOTX_DM_SERVICE_CLOUD,   NULL                                       },
#endif
    {DM_DISP_DEV_CORE_SERVICE_DEV,             NULL,                       IOTX_DM_DEVICE_MAIN,    IOTX_DM_LOCAL_NO_AUTH,   dm_disp_thing_dev_core_service_dev         },
    //{DM_DISP_THING_LAN_PREFIX_GET_REPLY,       DM_DISP_SYS_PREFIX,         IOTX_DM_DEVICE_MAIN,    IOTX_DM_SERVICE_ALL,   dm_disp_thing_lan_prefix_get_reply         },
    //{DM_DISP_THING_LAN_PREFIX_UPDATE_REPLY,    DM_DISP_SYS_PREFIX,         IOTX_DM_DEVICE_MAIN,    IOTX_DM_SERVICE_ALL,   dm_disp_thing_lan_prefix_update_reply      },
    //{DM_DISP_THING_LAN_BLACKLIST_UPDATE_REPLY, DM_DISP_SYS_PREFIX,         IOTX_DM_DEVICE_MAIN,    IOTX_DM_SERVICE_ALL,   dm_disp_thing_lan_blacklist_update_reply   }
};

static const dm_disp_event_mapping_t g_dm_disp_event_mapping[] DM_READ_ONLY = {
    {IOTX_CM_EVENT_CLOUD_CONNECTED,         dm_disp_event_cloud_connected_handler      },
    {IOTX_CM_EVENT_CLOUD_DISCONNECT,        dm_disp_event_cloud_disconnect_handler     },
    {IOTX_CM_EVENT_CLOUD_RECONNECT,         dm_disp_event_cloud_reconnect_handler      },
    {IOTX_CM_EVENT_LOCAL_CONNECTED,         dm_disp_event_local_connected_handler      },
    {IOTX_CM_EVENT_LOCAL_DISCONNECT,        dm_disp_event_local_disconnect_handler     },
    {IOTX_CM_EVENT_LOCAL_RECONNECT,         dm_disp_event_local_reconnect_handler      },
    {IOTX_CM_EVENT_FOUND_DEVICE,            dm_disp_event_found_device_handler         },
    {IOTX_CM_EVENT_REMOVE_DEVICE,           dm_disp_event_remove_device_handler        },
    {IOTX_CM_EVENT_REGISTER_RESULT,         dm_disp_event_register_result_handler      },
    {IOTX_CM_EVENT_UNREGISTER_RESULT,       dm_disp_event_unregister_result_handler    },
    {IOTX_CM_EVENT_SEND_RESULT,             dm_disp_event_send_result_handler          },
    {IOTX_CM_EVENT_ADD_SERVICE_RESULT,      dm_disp_event_add_service_result_handler   },
    {IOTX_CM_EVENT_REMOVE_SERVICE_RESULT,   dm_disp_event_remove_service_result_handler},
    {IOTX_CM_EVENT_NEW_DATA_RECEIVED,       dm_disp_event_new_data_received_handler    }
};

/*****************************************************************************/

int dm_disp_uri_prefix_sys_split(_IN_ char *uri, _IN_ int uri_len, _OU_ int *start, _OU_ int *end)
{
    int res = 0, offset = 0;

    if (uri == NULL || uri_len <= 0) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    /*"/sys/%s/%s/"*/
    res = dm_utils_memtok(uri, uri_len, DM_DISP_SERVICE_DELIMITER, 2, &offset);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    if (memcmp(DM_DISP_SYS_PREFIX, uri, offset + 1) != 0) {
        return FAIL_RETURN;
    }

    if (start) {
        *start = offset;
    }
    if (end) {
        *end = uri_len - offset - 1;
    }

    return SUCCESS_RETURN;
}

int dm_disp_uri_prefix_ext_session_split(_IN_ char *uri, _IN_ int uri_len, _OU_ int *start, _OU_ int *end)
{
    int res = 0, offset = 0;

    if (uri == NULL || uri_len <= 0) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    /*"/ext/session/%s/%s/"*/
    res = dm_utils_memtok(uri, uri_len, DM_DISP_SERVICE_DELIMITER, 3, &offset);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    if (memcmp(DM_DISP_EXT_SESSION_PREFIX, uri, offset + 1) != 0) {
        return FAIL_RETURN;
    }

    if (start) {
        *start = offset;
    }
    if (end) {
        *end = uri_len - offset - 1;
    }

    return SUCCESS_RETURN;
}

int dm_disp_uri_prefix_ext_ntp_split(_IN_ char *uri, _IN_ int uri_len, _OU_ int *start, _OU_ int *end)
{
    int res = 0, offset = 0;

    if (uri == NULL || uri_len <= 0) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    /*"/ext/ntp/%s/%s/"*/
    res = dm_utils_memtok(uri, uri_len, DM_DISP_SERVICE_DELIMITER, 3, &offset);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    if (memcmp(DM_DISP_EXT_NTP_PREFIX, uri, offset + 1) != 0) {
        return FAIL_RETURN;
    }

    if (start) {
        *start = offset;
    }
    if (end) {
        *end = uri_len - offset - 1;
    }

    return SUCCESS_RETURN;
}

int dm_disp_uri_prefix_ext_error_split(_IN_ char *uri, _IN_ int uri_len, _OU_ int *start, _OU_ int *end)
{
    int res = 0, offset = 0;

    if (uri == NULL || uri_len <= 0) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    /*"/ext/error/%s/%s/"*/
    res = dm_utils_memtok(uri, uri_len, DM_DISP_SERVICE_DELIMITER, 3, &offset);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    if (memcmp(DM_DISP_EXT_ERROR_PREFIX, uri, offset + 1) != 0) {
        return FAIL_RETURN;
    }

    if (start) {
        *start = offset;
    }
    if (end) {
        *end = uri_len - offset - 1;
    }

    return SUCCESS_RETURN;
}

int dm_disp_uri_pkdn_split(_IN_ char *uri, _IN_ int uri_len, _OU_ int *start, _OU_ int *end)
{
    int res = 0, offset = 0;

    if (uri == NULL || uri_len <= 0 || start == NULL || end == NULL) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    /* "/pk/dn/" */
    res = dm_utils_memtok(uri, uri_len, DM_DISP_SERVICE_DELIMITER, 3, &offset);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    *start = offset;
    *end = uri_len - offset - 1;

    return SUCCESS_RETURN;
}

int dm_disp_uri_service_specific_split(_IN_ char *uri, _IN_ int uri_len, _OU_ int *start, _OU_ int *end)
{
    int res = 0, offset = 0;
    const char *prefix = "/thing/service/";

    if (uri == NULL || uri_len <= 0 || start == NULL || end == NULL) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    /* "/thing/service/%s" */
    res = dm_utils_memtok(uri, uri_len, DM_DISP_SERVICE_DELIMITER, 3, &offset);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Check "thing/service" */
    if (memcmp(DM_DISP_THING_SERVICE_REQUEST, uri + 1, offset) != 0) {
        return FAIL_RETURN;
    }

    if (uri_len <= strlen(prefix)) {
        return FAIL_RETURN;
    }

    *start = offset;
    *end = uri_len - 1;

    return SUCCESS_RETURN;
}

int dm_disp_uri_rrpc_request_split(_IN_ char *uri, _IN_ int uri_len, _OU_ int *start, _OU_ int *end)
{
    int res = 0, offset = 0;
    const char *prefix = "/rrpc/request/";

    if (uri == NULL || uri_len <= 0 || start == NULL || end == NULL) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    /* "/rrpc/request/%s" */
    res = dm_utils_memtok(uri, uri_len, DM_DISP_SERVICE_DELIMITER, 3, &offset);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Check "rrpc/request" */
    if (memcmp(DM_DISP_RRPC_REQUEST_PLUS, uri + 1, offset) != 0) {
        return FAIL_RETURN;
    }

    if (uri_len <= strlen(prefix)) {
        return FAIL_RETURN;
    }

    *start = offset;
    *end = uri_len - 1;

    return SUCCESS_RETURN;
}

int dm_disp_uri_event_specific_split(_IN_ char *uri, _IN_ int uri_len, _OU_ int *start, _OU_ int *end)
{
    int res = 0, offset = 0;
    const char *suffix = "post_reply";

    if (uri == NULL || uri_len <= 0 || start == NULL || end == NULL) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    /* "/thing/event/%s/post_reply" */
    res = dm_utils_memtok(uri, uri_len, DM_DISP_SERVICE_DELIMITER, 3, &offset);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Check "thing/event/" */
    if (memcmp(DM_DISP_THING_EVENT_POST_REPLY, uri + 1, offset) != 0) {
        return FAIL_RETURN;
    }

    /* Check "post_reply" */
    if (uri_len <= strlen(suffix)) {
        return FAIL_RETURN;
    }
    if (memcmp(uri + (uri_len - strlen(suffix)), suffix, strlen(suffix)) != 0) {
        return FAIL_RETURN;
    }

    *start = offset;
    *end = uri_len - strlen(suffix) - 1;

    return SUCCESS_RETURN;
}

dm_disp_topic_mapping_t *dm_disp_get_topic_mapping(void)
{
    return (dm_disp_topic_mapping_t *)g_dm_disp_topic_mapping;
}

int dm_disp_get_topic_mapping_size(void)
{
    return sizeof(g_dm_disp_topic_mapping) / sizeof(dm_disp_topic_mapping_t);
}

int dm_disp_get_topic_mapping_dev_type_mask(_IN_ int index, _OU_ int *dev_type_mask)
{
    dm_disp_topic_mapping_t *dcs_mapping = (dm_disp_topic_mapping_t *)g_dm_disp_topic_mapping;

    if (index < 0 || dev_type_mask == NULL ||
        index >= sizeof(g_dm_disp_topic_mapping) / sizeof(dm_disp_topic_mapping_t)) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    *dev_type_mask = dcs_mapping[index].dev_type;

    return SUCCESS_RETURN;
}

dm_disp_event_mapping_t *dm_disp_get_event_mapping(void)
{
    return (dm_disp_event_mapping_t *)g_dm_disp_event_mapping;
}

int dm_disp_get_event_mapping_size(void)
{
    return sizeof(g_dm_disp_event_mapping) / sizeof(dm_disp_event_mapping_t);
}

void dm_disp_thing_service_property_set(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data)
{
    int res = 0, devid = 0;
    char product_key[PRODUCT_KEY_MAXLEN] = {0};
    char device_name[DEVICE_NAME_MAXLEN] = {0};
    dm_msg_request_payload_t request;
    dm_msg_response_t response;

    dm_log_info(DM_DISP_THING_SERVICE_PROPERTY_SET);

    memset(&request, 0, sizeof(dm_msg_request_payload_t));
    memset(&response, 0, sizeof(dm_msg_response_t));

    /* Request */
    res = dm_msg_uri_parse_pkdn(msg->URI, msg->URI_length, 2, 4, product_key, device_name);
    if (res != SUCCESS_RETURN) {
        return;
    }

    res = dm_mgr_search_device_by_pkdn(product_key, device_name, &devid);
    if (res != SUCCESS_RETURN) {
        return;
    }

    res = dm_msg_request_parse(msg->payload, msg->payload_length, &request);
    if (res != SUCCESS_RETURN) {
        return;
    }

    /* Operation */
    res = dm_msg_property_set(devid, &request);

    /* Response */
    response.service_prefix = DM_DISP_SYS_PREFIX;
    response.service_name = DM_DISP_THING_SERVICE_PROPERTY_SET_REPLY;
    memcpy(response.product_key, product_key, strlen(product_key));
    memcpy(response.device_name, device_name, strlen(device_name));
    response.code = (res == SUCCESS_RETURN) ? (IOTX_DM_ERR_CODE_SUCCESS) : (IOTX_DM_ERR_CODE_REQUEST_ERROR);

    int prop_set_reply_opt = 0;
    res = dm_opt_get(DM_OPT_UPSTREAM_PROPERTY_SET_REPLY, &prop_set_reply_opt);
    if (res == SUCCESS_RETURN) {
        if (prop_set_reply_opt) {
            dm_msg_response_without_data(&request, &response);
        }
    }

    dm_msg_response_local_without_data(&request, &response, msg->conn_ctx);
}

void dm_disp_thing_service_property_get(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data)
{
    int res = 0, devid = 0;
    char product_key[PRODUCT_KEY_MAXLEN] = {0};
    char device_name[DEVICE_NAME_MAXLEN] = {0};
    dm_msg_request_payload_t request;
    dm_msg_response_t response;

    /* Never Used */
    dm_log_info("Serivce Property Get, Payload: %.*s", msg->payload_length, msg->payload);

    memset(&request, 0, sizeof(dm_msg_request_payload_t));
    memset(&response, 0, sizeof(dm_msg_response_t));

    /* Request */
    res = dm_msg_uri_parse_pkdn(msg->URI, msg->URI_length, 2, 4, product_key, device_name);
    if (res != SUCCESS_RETURN) {
        return;
    }

    res = dm_mgr_search_device_by_pkdn(product_key, device_name, &devid);
    if (res != SUCCESS_RETURN) {
        return;
    }

    res = dm_msg_request_parse(msg->payload, msg->payload_length, &request);
    if (res != SUCCESS_RETURN) {
        return;
    }

    /* Operation */
#ifndef DEPRECATED_LINKKIT
    res = dm_msg_property_get(devid, &request, msg->conn_ctx);
#else
    int payload_len = 0;
    char *payload = NULL;
    res = dm_msg_property_get(devid, &request, &payload, &payload_len);
#endif

    if (res != SUCCESS_RETURN) {
        dm_log_err("DM Property Get Failed");
    }

#ifdef DEPRECATED_LINKKIT
    /* Response */
    response.service_prefix = DM_DISP_SYS_PREFIX;
    response.service_name = DM_DISP_THING_SERVICE_PROPERTY_GET;
    memcpy(response.product_key, product_key, strlen(product_key));
    memcpy(response.device_name, device_name, strlen(device_name));
    response.code = (res == SUCCESS_RETURN) ? (IOTX_DM_ERR_CODE_SUCCESS) : (IOTX_DM_ERR_CODE_REQUEST_ERROR);

    if (res != SUCCESS_RETURN) {
        payload = "{}";
        payload_len = strlen(payload);
    }

    dm_msg_response_local_with_data(&request, &response, payload, payload_len, msg->conn_ctx);
    if (response.code == IOTX_DM_ERR_CODE_SUCCESS) {
        DM_free(payload);
    }
#endif
}

void dm_disp_thing_service_property_post(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data)
{
    int res = 0;
    char product_key[PRODUCT_KEY_MAXLEN] = {0};
    char device_name[DEVICE_NAME_MAXLEN] = {0};
    dm_msg_request_payload_t request;
    dm_msg_response_t response;

    dm_log_info(DM_DISP_THING_EVENT_PROPERTY_POST);

    memset(&request, 0, sizeof(dm_msg_request_payload_t));
    memset(&response, 0, sizeof(dm_msg_response_t));

    /* Request */
    res = dm_msg_uri_parse_pkdn(msg->URI, msg->URI_length, 2, 4, product_key, device_name);
    if (res != SUCCESS_RETURN) {
        return;
    }

    res = dm_msg_request_parse(msg->payload, msg->payload_length, &request);

    /* Response */
    response.service_prefix = DM_DISP_SYS_PREFIX;
    response.service_name = DM_DISP_THING_EVENT_PROPERTY_POST;
    memcpy(response.product_key, product_key, strlen(product_key));
    memcpy(response.device_name, device_name, strlen(device_name));
    response.code = (res == SUCCESS_RETURN) ? (IOTX_DM_ERR_CODE_SUCCESS) : (IOTX_DM_ERR_CODE_REQUEST_ERROR);

    dm_msg_response_local_without_data(&request, &response, msg->conn_ctx);
}

void dm_disp_thing_service_request(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, char *identifier,
                                   int identifier_len, void *user_data)
{
    int res = 0;
    char product_key[PRODUCT_KEY_MAXLEN] = {0};
    char device_name[DEVICE_NAME_MAXLEN] = {0};
    dm_msg_request_payload_t request;

    dm_log_info(DM_DISP_THING_SERVICE_REQUEST, "{Identifier}");
    dm_log_info("Current URI: %.*s", msg->URI_length, msg->URI);
    dm_log_info("Current Identifier: %.*s", identifier_len, identifier);

    /* Parse Product Key And Device Name */
    res = dm_msg_uri_parse_pkdn(msg->URI, msg->URI_length, 2, 4, product_key, device_name);
    if (res != SUCCESS_RETURN) {
        return;
    }

    /* Request */
    res = dm_msg_request_parse(msg->payload, msg->payload_length, &request);
    if (res != SUCCESS_RETURN) {
        return;
    }

    /* Operation */
    res = dm_msg_thing_service_request(product_key, device_name, identifier, identifier_len, &request);
    if (res != SUCCESS_RETURN) {
        return;
    }

    /* Send Message To Local */
    dm_cmw_send_to_local(msg->URI, msg->URI_length, msg->payload, msg->payload_length, NULL);
}

void dm_disp_rrpc_request(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, char *messageid,
                          int messageid_len, void *user_data)
{
    int res = 0;
    char product_key[PRODUCT_KEY_MAXLEN] = {0};
    char device_name[DEVICE_NAME_MAXLEN] = {0};
    dm_msg_request_payload_t request;

    dm_log_info(DM_DISP_THING_SERVICE_REQUEST, "{Identifier}");
    dm_log_info("Current URI: %.*s", msg->URI_length, msg->URI);
    dm_log_info("Current Identifier: %.*s", messageid_len, messageid);

    /* Parse Product Key And Device Name */
    res = dm_msg_uri_parse_pkdn(msg->URI, msg->URI_length, 2, 4, product_key, device_name);
    if (res != SUCCESS_RETURN) {
        return;
    }

    /* Request */
    res = dm_msg_request_parse(msg->payload, msg->payload_length, &request);
    if (res != SUCCESS_RETURN) {
        return;
    }

    /* Operation */
    res = dm_msg_rrpc_request(product_key, device_name, messageid, messageid_len, &request);
    if (res != SUCCESS_RETURN) {
        return;
    }

    /* Send Message To Local */
    dm_cmw_send_to_local(msg->URI, msg->URI_length, msg->payload, msg->payload_length, NULL);
}

void dm_disp_thing_model_down_raw(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data)
{
    int res = 0;
    char product_key[PRODUCT_KEY_MAXLEN] = {0};
    char device_name[DEVICE_NAME_MAXLEN] = {0};

    dm_log_info(DM_DISP_THING_MODEL_DOWN_RAW);

    res = dm_msg_uri_parse_pkdn(msg->URI, msg->URI_length, 2, 4, product_key, device_name);
    if (res != SUCCESS_RETURN) {
        return;
    }

    dm_log_debug("Product Key: %s, Device Name: %s", product_key, device_name);

    dm_msg_thing_model_down_raw(product_key, device_name, msg->payload, msg->payload_length);
}

void dm_disp_thing_event_property_post_reply(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data)
{
    int res = 0;
    dm_msg_response_payload_t response;
    char int_id[DM_UTILS_UINT32_STRLEN] = {0};

    dm_log_info(DM_DISP_THING_EVENT_PROPERTY_POST_REPLY);

    /* Response */
    res = dm_msg_response_parse(msg->payload, msg->payload_length, &response);
    if (res != SUCCESS_RETURN) {
        return;
    }

    /* Operation */
    dm_msg_thing_event_property_post_reply(&response);

    /* Remove Message From Cache */
    memcpy(int_id, response.id.value, response.id.value_length);
    dm_msg_cache_remove(atoi(int_id));
}

void dm_disp_thing_event_post_reply(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, char *identifier,
                                    int identifier_len, void *user_data)
{
    int res = 0;
    dm_msg_response_payload_t response;
    char int_id[DM_UTILS_UINT32_STRLEN] = {0};

    dm_log_info(DM_DISP_THING_EVENT_POST_REPLY, "{Identifier}");
    dm_log_info("Current URI: %.*s", msg->URI_length, msg->URI);
    dm_log_info("Current Identifier: %.*s", identifier_len, identifier);

    /* Response */
    res = dm_msg_response_parse(msg->payload, msg->payload_length, &response);
    if (res != SUCCESS_RETURN) {
        return;
    }

    /* Operation */
    dm_msg_thing_event_post_reply(identifier, identifier_len, &response);

    /* Remove Message From Cache */
    memcpy(int_id, response.id.value, response.id.value_length);
    dm_msg_cache_remove(atoi(int_id));
}

void dm_disp_thing_deviceinfo_update_reply(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data)
{
    int res = 0;
    dm_msg_response_payload_t response;
    char int_id[DM_UTILS_UINT32_STRLEN] = {0};

    dm_log_info(DM_DISP_THING_DEVICEINFO_UPDATE_REPLY);

    /* Response */
    res = dm_msg_response_parse(msg->payload, msg->payload_length, &response);
    if (res != SUCCESS_RETURN) {
        return;
    }

    /* Operation */
    dm_msg_thing_deviceinfo_update_reply(&response);

    /* Remove Message From Cache */
    memcpy(int_id, response.id.value, response.id.value_length);
    dm_msg_cache_remove(atoi(int_id));
}

void dm_disp_thing_deviceinfo_delete_reply(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data)
{
    int res = 0;
    dm_msg_response_payload_t response;
    char int_id[DM_UTILS_UINT32_STRLEN] = {0};

    dm_log_info(DM_DISP_THING_DEVICEINFO_DELETE_REPLY);

    /* Response */
    res = dm_msg_response_parse(msg->payload, msg->payload_length, &response);
    if (res != SUCCESS_RETURN) {
        return;
    }

    /* Operation */
    dm_msg_thing_deviceinfo_delete_reply(&response);

    /* Remove Message From Cache */
    memcpy(int_id, response.id.value, response.id.value_length);
    dm_msg_cache_remove(atoi(int_id));
}

void dm_disp_thing_dsltemplate_get_reply(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data)
{
    int res = 0;
    dm_msg_response_payload_t response;
    char int_id[DM_UTILS_UINT32_STRLEN] = {0};

    dm_log_info(DM_DISP_THING_DSLTEMPLATE_GET_REPLY);

    /* Response */
    res = dm_msg_response_parse(msg->payload, msg->payload_length, &response);
    if (res != SUCCESS_RETURN) {
        return;
    }

    /* Operation */
    dm_msg_thing_dsltemplate_get_reply(&response);

    /* Remove Message From Cache */
    memcpy(int_id, response.id.value, response.id.value_length);
    dm_msg_cache_remove(atoi(int_id));
}

void dm_disp_thing_dynamictsl_get_reply(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data)
{
    int res = 0;
    dm_msg_response_payload_t response;
    char int_id[DM_UTILS_UINT32_STRLEN] = {0};

    dm_log_info(DM_DISP_THING_DYNAMICTSL_GET_REPLY);

    /* Response */
    res = dm_msg_response_parse(msg->payload, msg->payload_length, &response);
    if (res != SUCCESS_RETURN) {
        return;
    }

    /* Operation */
    dm_msg_thing_dynamictsl_get_reply(&response);

    /* Remove Message From Cache */
    memcpy(int_id, response.id.value, response.id.value_length);
    dm_msg_cache_remove(atoi(int_id));
}

void dm_disp_thing_model_up_raw_reply(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data)
{
    int res = 0;
    char product_key[PRODUCT_KEY_MAXLEN] = {0};
    char device_name[DEVICE_NAME_MAXLEN] = {0};

    dm_log_info(DM_DISP_THING_MODEL_UP_RAW_REPLY);

    res = dm_msg_uri_parse_pkdn(msg->URI, msg->URI_length, 2, 4, product_key, device_name);
    if (res != SUCCESS_RETURN) {
        return;
    }

    dm_log_debug("Product Key: %s, Device Name: %s", product_key, device_name);

    /* Operation */
    res = dm_msg_thing_model_up_raw_reply(product_key, device_name, msg->payload, msg->payload_length);
    if (res != SUCCESS_RETURN) {
        return;
    }
}

void dm_disp_rrpc_request_plus(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data)
{

}

void dm_disp_ntp_response(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data)
{
    int res = 0;

    dm_log_info(DM_DISP_NTP_RESPONSE);

    /* Operation */
    res = dm_msg_ntp_response(msg->payload, msg->payload_length);
    if (res != SUCCESS_RETURN) {
        return;
    }
}

void dm_disp_ext_error_response(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data)
{
    int res = 0;
    dm_msg_response_payload_t response;
    /* char int_id[DM_UTILS_UINT32_STRLEN] = {0}; */

    /* Response */
    res = dm_msg_response_parse(msg->payload, msg->payload_length, &response);
    if (res != SUCCESS_RETURN) {
        return;
    }

    /* Operation */
    res = dm_msg_ext_error_reply(&response);
    if (res != SUCCESS_RETURN) {
        return;
    }

    /* Remove Message From Cache */
    /* memcpy(int_id, response.id.value, response.id.value_length);
    dm_msg_cache_remove(atoi(int_id)); */
}

#ifdef CONFIG_DM_DEVTYPE_GATEWAY
void dm_disp_thing_topo_add_notify(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data)
{
    int res = 0;
    dm_msg_request_payload_t request;
    dm_msg_response_t response;

    dm_log_info(DM_DISP_THING_TOPO_ADD_NOTIFY);

    /* Request */
    res = dm_msg_request_parse(msg->payload, msg->payload_length, &request);
    if (res == SUCCESS_RETURN) {
        res = dm_msg_topo_add_notify(request.params.value, request.params.value_length);
    }

    /* Response */
    response.service_prefix = DM_DISP_SYS_PREFIX;
    response.service_name = DM_DISP_THING_TOPO_ADD_NOTIFY_REPLY;
    memcpy(response.product_key, source->product_key, strlen(source->product_key));
    memcpy(response.device_name, source->device_name, strlen(source->device_name));
    response.code = (res == SUCCESS_RETURN) ? (IOTX_DM_ERR_CODE_SUCCESS) : (IOTX_DM_ERR_CODE_REQUEST_ERROR);
    dm_msg_response_without_data(&request, &response);
}

void dm_disp_thing_disable(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data)
{
    int res = 0;
    dm_msg_request_payload_t request;
    dm_msg_response_t response;

    dm_log_info(DM_DISP_THING_DISABLE);

    memset(&request, 0, sizeof(dm_msg_request_payload_t));
    memset(&response, 0, sizeof(dm_msg_response_t));

    /* Request */
    res = dm_msg_request_parse(msg->payload, msg->payload_length, &request);
    if (res != SUCCESS_RETURN) {
        return;
    }

    /* Operation */
    res = dm_msg_thing_disable(source->product_key, source->device_name);

    /* Response */
    response.service_prefix = DM_DISP_SYS_PREFIX;
    response.service_name = DM_DISP_THING_DISABLE_REPLY;
    memcpy(response.product_key, source->product_key, strlen(source->product_key));
    memcpy(response.device_name, source->device_name, strlen(source->device_name));
    response.code = (res == SUCCESS_RETURN) ? (IOTX_DM_ERR_CODE_SUCCESS) : (IOTX_DM_ERR_CODE_REQUEST_ERROR);
    dm_msg_response_without_data(&request, &response);
}

void dm_disp_thing_enable(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data)
{
    int res = 0;
    dm_msg_request_payload_t request;
    dm_msg_response_t response;

    dm_log_info(DM_DISP_THING_ENABLE);

    memset(&request, 0, sizeof(dm_msg_request_payload_t));
    memset(&response, 0, sizeof(dm_msg_response_t));

    /* Request */
    res = dm_msg_request_parse(msg->payload, msg->payload_length, &request);
    if (res != SUCCESS_RETURN) {
        return;
    }

    /* Operation */
    res = dm_msg_thing_enable(source->product_key, source->device_name);

    /* Response */
    response.service_prefix = DM_DISP_SYS_PREFIX;
    response.service_name = DM_DISP_THING_ENABLE_REPLY;
    memcpy(response.product_key, source->product_key, strlen(source->product_key));
    memcpy(response.device_name, source->device_name, strlen(source->device_name));
    response.code = (res == SUCCESS_RETURN) ? (IOTX_DM_ERR_CODE_SUCCESS) : (IOTX_DM_ERR_CODE_REQUEST_ERROR);
    dm_msg_response_without_data(&request, &response);
}

void dm_disp_thing_delete(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data)
{
    int res = 0;
    dm_msg_request_payload_t request;
    dm_msg_response_t response;

    dm_log_info(DM_DISP_THING_DELETE);

    memset(&request, 0, sizeof(dm_msg_request_payload_t));
    memset(&response, 0, sizeof(dm_msg_response_t));

    /* Request */
    res = dm_msg_request_parse(msg->payload, msg->payload_length, &request);
    if (res != SUCCESS_RETURN) {
        return;
    }

    /* Operation */
    res = dm_msg_thing_delete(source->product_key, source->device_name);

    /* Response */
    response.service_prefix = DM_DISP_SYS_PREFIX;
    response.service_name = DM_DISP_THING_DELETE_REPLY;
    memcpy(response.product_key, source->product_key, strlen(source->product_key));
    memcpy(response.device_name, source->device_name, strlen(source->device_name));
    response.code = (res == SUCCESS_RETURN) ? (IOTX_DM_ERR_CODE_SUCCESS) : (IOTX_DM_ERR_CODE_REQUEST_ERROR);
    dm_msg_response_without_data(&request, &response);

}

void dm_disp_thing_gateway_permit(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data)
{
    int res = 0;
    dm_msg_request_payload_t request;
    dm_msg_response_t response;

    dm_log_info(DM_DISP_THING_GATEWAY_PERMIT);

    memset(&request, 0, sizeof(dm_msg_request_payload_t));
    memset(&response, 0, sizeof(dm_msg_response_t));

    /* Request */
    res = dm_msg_request_parse(msg->payload, msg->payload_length, &request);
    if (res != SUCCESS_RETURN) {
        return;
    }

    /* Operation */
    res = dm_msg_thing_gateway_permit(request.params.value, request.params.value_length);

    /* Response */
    response.service_prefix = DM_DISP_SYS_PREFIX;
    response.service_name = DM_DISP_THING_GATEWAY_PERMIT_REPLY;
    memcpy(response.product_key, source->product_key, strlen(source->product_key));
    memcpy(response.device_name, source->device_name, strlen(source->device_name));
    response.code = (res == SUCCESS_RETURN) ? (IOTX_DM_ERR_CODE_SUCCESS) : (IOTX_DM_ERR_CODE_REQUEST_ERROR);
    dm_msg_response_without_data(&request, &response);
}

void dm_disp_thing_sub_register_reply(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data)
{
    int res = 0;
    dm_msg_response_payload_t response;
    char int_id[DM_UTILS_UINT32_STRLEN] = {0};

    dm_log_info(DM_DISP_THING_SUB_REGISTER_REPLY);

    memset(&response, 0, sizeof(dm_msg_response_payload_t));

    /* Response */
    res = dm_msg_response_parse(msg->payload, msg->payload_length, &response);
    if (res != SUCCESS_RETURN) {
        return;
    }

    /* Operation */
    dm_msg_thing_sub_register_reply(&response);

    /* Remove Message From Cache */
    memcpy(int_id, response.id.value, response.id.value_length);
    dm_msg_cache_remove(atoi(int_id));
}

void dm_disp_thing_sub_unregister_reply(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data)
{
    int res = 0;
    dm_msg_response_payload_t response;
    char int_id[DM_UTILS_UINT32_STRLEN] = {0};

    dm_log_info(DM_DISP_THING_SUB_UNREGISTER_REPLY);

    memset(&response, 0, sizeof(dm_msg_response_payload_t));

    /* Response */
    res = dm_msg_response_parse(msg->payload, msg->payload_length, &response);
    if (res != SUCCESS_RETURN) {
        return;
    }

    /* Operation */
    dm_msg_thing_sub_unregister_reply(&response);

    /* Remove Message From Cache */
    memcpy(int_id, response.id.value, response.id.value_length);
    dm_msg_cache_remove(atoi(int_id));
}

void dm_disp_thing_topo_add_reply(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data)
{
    int res = 0;
    dm_msg_response_payload_t response;
    char int_id[DM_UTILS_UINT32_STRLEN] = {0};

    dm_log_info(DM_DISP_THING_TOPO_ADD_REPLY);

    memset(&response, 0, sizeof(dm_msg_response_payload_t));

    /* Response */
    res = dm_msg_response_parse(msg->payload, msg->payload_length, &response);
    if (res != SUCCESS_RETURN) {
        return;
    }

    /* Operation */
    dm_msg_thing_topo_add_reply(&response);

    /* Remove Message From Cache */
    memcpy(int_id, response.id.value, response.id.value_length);
    dm_msg_cache_remove(atoi(int_id));
}

void dm_disp_thing_topo_delete_reply(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data)
{
    int res = 0;
    dm_msg_response_payload_t response;
    char int_id[DM_UTILS_UINT32_STRLEN] = {0};

    dm_log_info(DM_DISP_THING_TOPO_DELETE_REPLY);

    /* Response */
    res = dm_msg_response_parse(msg->payload, msg->payload_length, &response);
    if (res != SUCCESS_RETURN) {
        return;
    }

    /* Operation */
    dm_msg_thing_topo_delete_reply(&response);

    /* Remove Message From Cache */
    memcpy(int_id, response.id.value, response.id.value_length);
    dm_msg_cache_remove(atoi(int_id));
}

void dm_disp_thing_topo_get_reply(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data)
{
    int res = 0;
    dm_msg_response_payload_t response;
    char int_id[DM_UTILS_UINT32_STRLEN] = {0};

    dm_log_info(DM_DISP_THING_TOPO_GET_REPLY);

    /* Response */
    res = dm_msg_response_parse(msg->payload, msg->payload_length, &response);
    if (res != SUCCESS_RETURN) {
        return;
    }

    /* Operation */
    dm_msg_topo_get_reply(&response);

    /* Remove Message From Cache */
    memcpy(int_id, response.id.value, response.id.value_length);
    dm_msg_cache_remove(atoi(int_id));
}

void dm_disp_thing_list_found_reply(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data)
{
    int res = 0;
    dm_msg_response_payload_t response;

    dm_log_info(DM_DISP_THING_LIST_FOUND_REPLY);

    /* Response */
    res = dm_msg_response_parse(msg->payload, msg->payload_length, &response);
    if (res != SUCCESS_RETURN) {
        return;
    }

    /* Operation */
    dm_msg_thing_list_found_reply(&response);
}

void dm_disp_combine_login_reply(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data)
{
    int res = 0;
    dm_msg_response_payload_t response;
    char int_id[DM_UTILS_UINT32_STRLEN] = {0};

    dm_log_info(DM_DISP_COMBINE_LOGIN_REPLY);

    /* Response */
    res = dm_msg_response_parse(msg->payload, msg->payload_length, &response);
    if (res != SUCCESS_RETURN) {
        return;
    }

    /* Operation */
    res = dm_msg_combine_login_reply(&response);
    if (res != SUCCESS_RETURN) {
        return;
    }

    /* Remove Message From Cache */
    memcpy(int_id, response.id.value, response.id.value_length);
    dm_msg_cache_remove(atoi(int_id));
}

void dm_disp_combine_logout_reply(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data)
{
    int res = 0;
    dm_msg_response_payload_t response;
    char int_id[DM_UTILS_UINT32_STRLEN] = {0};

    dm_log_info(DM_DISP_COMBINE_LOGOUT_REPLY);

    /* Response */
    res = dm_msg_response_parse(msg->payload, msg->payload_length, &response);
    if (res != SUCCESS_RETURN) {
        return;
    }

    /* Operation */
    res = dm_msg_combine_logout_reply(&response);
    if (res != SUCCESS_RETURN) {
        return;
    }

    /* Remove Message From Cache */
    memcpy(int_id, response.id.value, response.id.value_length);
    dm_msg_cache_remove(atoi(int_id));
}
#endif

void dm_disp_thing_dev_core_service_dev(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data)
{
    int res = 0, payload_len;
    char *payload = NULL;
    dm_msg_request_payload_t request;
    dm_msg_response_t response;

    dm_log_info(DM_DISP_DEV_CORE_SERVICE_DEV);

    memset(&request, 0, sizeof(dm_msg_request_payload_t));
    memset(&response, 0, sizeof(dm_msg_response_t));

    res = dm_msg_request_parse(msg->payload, msg->payload_length, &request);
    if (res != SUCCESS_RETURN) {
        return;
    }

    res = dm_msg_dev_core_service_dev(&payload, &payload_len);
    
    /* Response */
    response.service_prefix = NULL;
    response.service_name = DM_DISP_DEV_CORE_SERVICE_DEV;
    response.code = (res == SUCCESS_RETURN) ? (IOTX_DM_ERR_CODE_SUCCESS) : (IOTX_DM_ERR_CODE_REQUEST_ERROR);

    if (res != SUCCESS_RETURN) {
        payload = "{}";
        payload_len = strlen(payload);
    }

    dm_msg_response_local_with_data(&request, &response, payload, payload_len, msg->conn_ctx);

    if (response.code == IOTX_DM_ERR_CODE_SUCCESS) {
        DM_free(payload);
    }
}

void dm_disp_thing_lan_prefix_get_reply(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data)
{
    int res = 0;
    dm_msg_response_payload_t response;

    dm_log_info(DM_DISP_THING_LAN_PREFIX_GET_REPLY);

    /* Response */
    res = dm_msg_response_parse(msg->payload, msg->payload_length, &response);
    if (res != SUCCESS_RETURN) {
        return;
    }

    /* Operation */
    //res = dm_msg_thing_lan_prefix_get_reply(&response);
    //if (res != SUCCESS_RETURN) {return;}
}

void dm_disp_thing_lan_prefix_update_reply(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data)
{

}

void dm_disp_thing_lan_blacklist_update_reply(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data)
{

}

void dm_disp_user_defined_handler(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data)
{
    dm_log_info("USER Defined Handler");
    dm_log_info("URI: %.*s", msg->URI_length, msg->URI);
    dm_log_info("Payload: %.*s", msg->payload_length, msg->payload);
}

void dm_disp_event_cloud_connected_handler(void *pcontext, iotx_cm_event_msg_t *msg, void *user_data)
{
    dm_log_info("IOTX_CM_EVENT_CLOUD_CONNECTED");

    dm_conn_set_cloud_conn_state(1);

    /* Re-Subscribe Topic */
    /* Start From Subscribe Generic Topic */
    dm_sub_multi_next(IOTX_DM_LOCAL_NODE_DEVID, 0);

    /* Set Service Event Topic Index To DM_MGR_DEV_SUB_START */
    /* Service Event Topic Subscribe Will Be Execute After All Generic Topic Subscribed */
    dm_mgr_set_dev_sub_service_event_index(IOTX_DM_LOCAL_NODE_DEVID, DM_MGR_DEV_SUB_START);

#ifdef CONFIG_DM_SUPPORT_LOCAL_CONN
    dm_log_info("DM ALCS INIT SECOND");
    dm_cmw_local_init_second(dm_conn_get_local_conn());
#endif

#ifdef DEPRECATED_LINKKIT
    int res = 0;

    /* DM OTA Module Init */
    res = dm_ota_init();
    if (res != SUCCESS_RETURN) {
        dm_log_err(DM_UTILS_LOG_OTA_INIT_FAILED);
    }

    /* DM Config OTA Module Init */
    res = dm_cota_init();
    if (res != SUCCESS_RETURN) {
        dm_log_err(DM_UTILS_LOG_COTA_INIT_FAILED);
    }

    /* DM Firmware OTA Mudule Init */
    res = dm_fota_init();
    if (res != SUCCESS_RETURN) {
        dm_log_err(DM_UTILS_LOG_FOTA_INIT_FAILED);
    }
#endif
    dm_msg_cloud_connected();
}

void dm_disp_event_cloud_disconnect_handler(void *pcontext, iotx_cm_event_msg_t *msg, void *user_data)
{
    dm_log_info("IOTX_CM_EVENT_CLOUD_DISCONNECT");

    dm_conn_set_cloud_conn_state(0);

    dm_msg_cloud_disconnect();
}

void dm_disp_event_cloud_reconnect_handler(void *pcontext, iotx_cm_event_msg_t *msg, void *user_data)
{
    dm_log_info("IOTX_CM_EVENT_CLOUD_RECONNECT");

    dm_conn_set_cloud_conn_state(1);

    dm_msg_cloud_reconnect();
}

void dm_disp_event_local_connected_handler(void *pcontext, iotx_cm_event_msg_t *msg, void *user_data)
{
    int res = 0;

    dm_log_info("IOTX_CM_EVENT_LOCAL_CONNECTED");

    dm_conn_set_local_conn_state(1);

#ifdef CONFIG_DM_SUPPORT_LOCAL_CONN
    dm_log_info("DM ALCS INIT SECOND");
    if (dm_conn_get_cloud_conn_state()) {
        dm_cmw_local_init_second(dm_conn_get_local_conn());
    }
    dm_sub_local_register();
#endif

    res = _dm_msg_send_to_user(IOTX_DM_EVENT_LOCAL_CONNECTED, NULL);
    if (res != SUCCESS_RETURN) {
        return;
    }
}

void dm_disp_event_local_disconnect_handler(void *pcontext, iotx_cm_event_msg_t *msg, void *user_data)
{
    int res = 0;

    dm_log_info("IOTX_CM_EVENT_LOCAL_DISCONNECT");

    dm_conn_set_local_conn_state(0);

    res = _dm_msg_send_to_user(IOTX_DM_EVENT_LOCAL_DISCONNECT, NULL);
    if (res != SUCCESS_RETURN) {
        return;
    }
}

void dm_disp_event_local_reconnect_handler(void *pcontext, iotx_cm_event_msg_t *msg, void *user_data)
{
    int res = 0;

    dm_log_info("IOTX_CM_EVENT_LOCAL_RECONNECT");

    dm_conn_set_local_conn_state(1);

    res = _dm_msg_send_to_user(IOTX_DM_EVENT_LOCAL_RECONNECT, NULL);
    if (res != SUCCESS_RETURN) {
        return;
    }
}

void dm_disp_event_found_device_handler(void *pcontext, iotx_cm_event_msg_t *msg, void *user_data)
{
    dm_log_info("IOTX_CM_EVENT_FOUND_DEVICE");

    iotx_cm_send_peer_t *cm_send_peer = (iotx_cm_send_peer_t *)msg->msg;

    dm_msg_found_device(cm_send_peer->product_key, cm_send_peer->device_name);
}

void dm_disp_event_remove_device_handler(void *pcontext, iotx_cm_event_msg_t *msg, void *user_data)
{
    dm_log_info("IOTX_CM_EVENT_REMOVE_DEVICE");

    iotx_cm_send_peer_t *cm_send_peer = (iotx_cm_send_peer_t *)msg->msg;

    dm_msg_remove_device(cm_send_peer->product_key, cm_send_peer->device_name);
}

void dm_disp_event_register_result_handler(void *pcontext, iotx_cm_event_msg_t *msg, void *user_data)
{
    dm_log_info("IOTX_CM_EVENT_REGISTER_RESULT");

    iotx_cm_event_result_t *cm_event_result = (iotx_cm_event_result_t *)msg->msg;

    dm_log_info("Topic Subscribe Result: %d, URI: %s", cm_event_result->result, cm_event_result->URI);

    dm_msg_register_result(cm_event_result->URI, cm_event_result->result);
}

void dm_disp_event_unregister_result_handler(void *pcontext, iotx_cm_event_msg_t *msg, void *user_data)
{
    dm_log_info("IOTX_CM_EVENT_UNREGISTER_RESULT");

    //iotx_cm_event_result_t *cm_event_result = (iotx_cm_event_result_t *)msg->msg;

    //dm_msg_unregister_result(cm_event_result->URI,cm_event_result->result);
}

void dm_disp_event_send_result_handler(void *pcontext, iotx_cm_event_msg_t *msg, void *user_data)
{
    dm_log_info("IOTX_CM_EVENT_SEND_RESULT");

    iotx_cm_event_result_t *cm_event_result = (iotx_cm_event_result_t *)msg->msg;

    dm_msg_send_result(cm_event_result->URI, cm_event_result->result);
}

void dm_disp_event_add_service_result_handler(void *pcontext, iotx_cm_event_msg_t *msg, void *user_data)
{
    dm_log_info("IOTX_CM_EVENT_ADD_SERVICE_RESULT");

    iotx_cm_event_result_t *cm_event_result = (iotx_cm_event_result_t *)msg->msg;

    dm_msg_add_service_result(cm_event_result->URI, cm_event_result->result);
}

void dm_disp_event_remove_service_result_handler(void *pcontext, iotx_cm_event_msg_t *msg, void *user_data)
{
    dm_log_info("IOTX_CM_EVENT_REMOVE_SERVICE_RESULT");

    iotx_cm_event_result_t *cm_event_result = (iotx_cm_event_result_t *)msg->msg;

    dm_msg_remove_service_result(cm_event_result->URI, cm_event_result->result);
}

void dm_disp_event_new_data_received_handler(void *pcontext, iotx_cm_event_msg_t *msg, void *user_data)
{
    dm_log_info("IOTX_CM_EVENT_NEW_DATA_RECEIVED");

    iotx_cm_new_data_t *cm_new_data = msg->msg;
    dm_cmw_topic_callback(cm_new_data->peer, cm_new_data->message_info, user_data);
}
