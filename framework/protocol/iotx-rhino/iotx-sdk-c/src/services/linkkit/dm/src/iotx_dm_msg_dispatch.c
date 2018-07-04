#include "iot_import.h"
#include "lite-utils.h"
#include "lite-list.h"
#include "iotx_dm_common.h"
#include "iotx_dm_msg_dispatch.h"
#include "iotx_dm_manager.h"
#include "iotx_dm_message.h"
#include "iotx_dm_cm_wrapper.h"
#include "iotx_dm_message_cache.h"
#include "iot_export_dm.h"

/*****************************Internal Definition*****************************/

const char IOTX_DCS_SYS_PREFIX[]                        DM_READ_ONLY = "/sys/%s/%s/";
const char IOTX_DCS_EXT_SESSION_PREFIX[]                DM_READ_ONLY = "/ext/session/%s/%s/";
const char IOTX_DCS_REPLY_SUFFIX[]                      DM_READ_ONLY = "_reply";

/* From Cloud To Local Request And Response*/
const char IOTX_DCS_THING_TOPO_ADD_NOTIFY[]             DM_READ_ONLY = "thing/topo/add/notify";
const char IOTX_DCS_THING_TOPO_ADD_NOTIFY_REPLY[]       DM_READ_ONLY = "thing/topo/add/notify_reply";
const char IOTX_DCS_THING_SERVICE_PROPERTY_SET[]        DM_READ_ONLY = "thing/service/property/set";
const char IOTX_DCS_THING_SERVICE_PROPERTY_SET_REPLY[]  DM_READ_ONLY = "thing/service/property/set_reply";
const char IOTX_DCS_THING_SERVICE_PROPERTY_GET[]        DM_READ_ONLY = "thing/service/property/get";
const char IOTX_DCS_THING_SERVICE_REQUEST[]             DM_READ_ONLY = "thing/service/%s";
const char IOTX_DCS_THING_SERVICE_RESPONSE[]            DM_READ_ONLY = "thing/service/%.*s_reply";
const char IOTX_DCS_THING_DISABLE[]                     DM_READ_ONLY = "thing/disable";
const char IOTX_DCS_THING_DISABLE_REPLY[]               DM_READ_ONLY = "thing/disable_reply";
const char IOTX_DCS_THING_ENABLE[]                      DM_READ_ONLY = "thing/enable";
const char IOTX_DCS_THING_ENABLE_REPLY[]                DM_READ_ONLY = "thing/enable_reply";
const char IOTX_DCS_THING_DELETE[]                      DM_READ_ONLY = "thing/delete";
const char IOTX_DCS_THING_DELETE_REPLY[]                DM_READ_ONLY = "thing/delete_reply";
const char IOTX_DCS_THING_MODEL_DOWN_RAW[]              DM_READ_ONLY = "thing/model/down_raw";
const char IOTX_DCS_THING_MODEL_DOWN_RAW_REPLY[]        DM_READ_ONLY = "thing/model/down_raw_reply";
const char IOTX_DCS_THING_GATEWAY_PERMIT[]              DM_READ_ONLY = "thing/gateway/permit";
const char IOTX_DCS_THING_GATEWAY_PERMIT_REPLY[]        DM_READ_ONLY = "thing/gateway/permit_reply";

/* From Local To Cloud Request And Response*/
const char IOTX_DCS_THING_SUB_REGISTER[]                DM_READ_ONLY = "thing/sub/register";
const char IOTX_DCS_THING_SUB_REGISTER_REPLY[]          DM_READ_ONLY = "thing/sub/register_reply";
const char IOTX_DCS_THING_SUB_UNREGISTER[]              DM_READ_ONLY = "thing/sub/unregister";
const char IOTX_DCS_THING_SUB_UNREGISTER_REPLY[]        DM_READ_ONLY = "thing/sub/unregister_reply";
const char IOTX_DCS_THING_TOPO_ADD[]                    DM_READ_ONLY = "thing/topo/add";
const char IOTX_DCS_THING_TOPO_ADD_REPLY[]              DM_READ_ONLY = "thing/topo/add_reply";
const char IOTX_DCS_THING_TOPO_DELETE[]                 DM_READ_ONLY = "thing/topo/delete";
const char IOTX_DCS_THING_TOPO_DELETE_REPLY[]           DM_READ_ONLY = "thing/topo/delete_reply";
const char IOTX_DCS_THING_TOPO_GET[]                    DM_READ_ONLY = "thing/topo/get";
const char IOTX_DCS_THING_TOPO_GET_REPLY[]              DM_READ_ONLY = "thing/topo/get_reply";
const char IOTX_DCS_THING_LIST_FOUND[]                  DM_READ_ONLY = "thing/list/found";
const char IOTX_DCS_THING_LIST_FOUND_REPLY[]            DM_READ_ONLY = "thing/list/found_reply";
const char IOTX_DCS_THING_EVENT_PROPERTY_POST[]         DM_READ_ONLY = "thing/event/property/post";
const char IOTX_DCS_THING_EVENT_PROPERTY_POST_REPLY[]   DM_READ_ONLY = "thing/event/property/post_reply";
const char IOTX_DCS_THING_EVENT_POST[]                  DM_READ_ONLY = "thing/event/%.*s/post";
const char IOTX_DCS_THING_EVENT_POST_REPLY[]            DM_READ_ONLY = "thing/event/%s/post_reply";
const char IOTX_DCS_THING_DEVICEINFO_UPDATE[]           DM_READ_ONLY = "thing/deviceinfo/update";
const char IOTX_DCS_THING_DEVICEINFO_UPDATE_REPLY[]     DM_READ_ONLY = "thing/deviceinfo/update_reply";
const char IOTX_DCS_THING_DEVICEINFO_DELETE[]           DM_READ_ONLY = "thing/deviceinfo/delete";
const char IOTX_DCS_THING_DEVICEINFO_DELETE_REPLY[]     DM_READ_ONLY = "thing/deviceinfo/delete_reply";
const char IOTX_DCS_THING_DSLTEMPLATE_GET[]             DM_READ_ONLY = "thing/dsltemplate/get";
const char IOTX_DCS_THING_DSLTEMPLATE_GET_REPLY[]       DM_READ_ONLY = "thing/dsltemplate/get_reply";
const char IOTX_DCS_THING_DYNAMICTSL_GET[]              DM_READ_ONLY = "thing/dynamicTsl/get";
const char IOTX_DCS_THING_DYNAMICTSL_GET_REPLY[]        DM_READ_ONLY = "thing/dynamicTsl/get_reply";
const char IOTX_DCS_COMBINE_LOGIN[]                     DM_READ_ONLY = "combine/login";
const char IOTX_DCS_COMBINE_LOGIN_REPLY[]               DM_READ_ONLY = "combine/login_reply";
const char IOTX_DCS_COMBINE_LOGOUT[]                    DM_READ_ONLY = "combine/logout";
const char IOTX_DCS_COMBINE_LOGOUT_REPLY[]              DM_READ_ONLY = "combine/logout_reply";
const char IOTX_DCS_THING_MODEL_UP_RAW[]                DM_READ_ONLY = "thing/model/up_raw";
const char IOTX_DCS_THING_MODEL_UP_RAW_REPLY[]          DM_READ_ONLY = "thing/model/up_raw_reply";
const char IOTX_DCS_DEV_CORE_SERVICE_DEV[]              DM_READ_ONLY = "/dev/core/service/dev";
const char IOTX_DCS_THING_LAN_PREFIX_GET[]              DM_READ_ONLY = "thing/lan/prefix/get";
const char IOTX_DCS_THING_LAN_PREFIX_GET_REPLY[]        DM_READ_ONLY = "thing/lan/prefix/get_reply";
const char IOTX_DCS_THING_LAN_PREFIX_UPDATE[]           DM_READ_ONLY = "thing/lan/prefix/update";
const char IOTX_DCS_THING_LAN_PREFIX_UPDATE_REPLY[]     DM_READ_ONLY = "thing/lan/prefix/update_reply";

static const iotx_dcs_resource_mapping_t g_iotx_dcs_resource_mapping[] DM_READ_ONLY = {
	{IOTX_DCS_DEV_CORE_SERVICE_DEV,            NULL,                        IOTX_DM_DEVICE_ALL,     IOTX_DM_LOCAL_NO_AUTH, iotx_dcs_thing_dev_core_service_dev         },
	{IOTX_DCS_THING_LAN_PREFIX_GET_REPLY,      IOTX_DCS_SYS_PREFIX,         IOTX_DM_DEVICE_ALL,     IOTX_DM_LOCAL_AUTH,    iotx_dcs_thing_lan_prefix_get_reply         },
	{IOTX_DCS_THING_LAN_PREFIX_UPDATE_REPLY,   IOTX_DCS_SYS_PREFIX,         IOTX_DM_DEVICE_ALL,     IOTX_DM_LOCAL_AUTH,    iotx_dcs_thing_lan_prefix_update_reply      }
};

static const iotx_dcs_topic_mapping_t g_iotx_dcs_topic_mapping[] DM_READ_ONLY = {
	{IOTX_DCS_THING_TOPO_ADD_NOTIFY,           IOTX_DCS_SYS_PREFIX,         IOTX_DM_DEVICE_GATEWAY, IOTX_DM_SERVICE_ALL,   iotx_dcs_thing_topo_add_notify              },
	{IOTX_DCS_THING_SERVICE_PROPERTY_SET,      IOTX_DCS_SYS_PREFIX,         IOTX_DM_DEVICE_ALL,     IOTX_DM_SERVICE_ALL,   iotx_dcs_thing_service_property_set         },
	{IOTX_DCS_THING_SERVICE_PROPERTY_GET,      IOTX_DCS_SYS_PREFIX,         IOTX_DM_DEVICE_ALL,     IOTX_DM_SERVICE_ALL,   iotx_dcs_thing_service_property_get         },
	{IOTX_DCS_THING_DISABLE,                   IOTX_DCS_SYS_PREFIX,         IOTX_DM_DEVICE_ALL,     IOTX_DM_SERVICE_ALL,   iotx_dcs_thing_disable                      },
	{IOTX_DCS_THING_ENABLE,                    IOTX_DCS_SYS_PREFIX,         IOTX_DM_DEVICE_ALL,     IOTX_DM_SERVICE_ALL,   iotx_dcs_thing_enable                       },
	{IOTX_DCS_THING_DELETE,                    IOTX_DCS_SYS_PREFIX,         IOTX_DM_DEVICE_ALL,     IOTX_DM_SERVICE_ALL,   iotx_dcs_thing_delete                       },
	{IOTX_DCS_THING_MODEL_DOWN_RAW,            IOTX_DCS_SYS_PREFIX,         IOTX_DM_DEVICE_ALL,     IOTX_DM_SERVICE_ALL,   iotx_dcs_thing_model_down_raw               },
	{IOTX_DCS_THING_GATEWAY_PERMIT,            IOTX_DCS_SYS_PREFIX,         IOTX_DM_DEVICE_GATEWAY, IOTX_DM_SERVICE_ALL,   iotx_dcs_thing_gateway_permit               },
	{IOTX_DCS_THING_SUB_REGISTER_REPLY,        IOTX_DCS_SYS_PREFIX,         IOTX_DM_DEVICE_GATEWAY, IOTX_DM_SERVICE_ALL,   iotx_dcs_thing_sub_register_reply           },
	{IOTX_DCS_THING_SUB_UNREGISTER_REPLY,      IOTX_DCS_SYS_PREFIX,         IOTX_DM_DEVICE_GATEWAY, IOTX_DM_SERVICE_ALL,   iotx_dcs_thing_sub_unregister_reply         },
	{IOTX_DCS_THING_TOPO_ADD_REPLY,            IOTX_DCS_SYS_PREFIX,         IOTX_DM_DEVICE_GATEWAY, IOTX_DM_SERVICE_ALL,   iotx_dcs_thing_topo_add_reply               },
	{IOTX_DCS_THING_TOPO_DELETE_REPLY,         IOTX_DCS_SYS_PREFIX,         IOTX_DM_DEVICE_GATEWAY, IOTX_DM_SERVICE_ALL,   iotx_dcs_thing_topo_delete_reply            },
	{IOTX_DCS_THING_TOPO_GET_REPLY,            IOTX_DCS_SYS_PREFIX,         IOTX_DM_DEVICE_GATEWAY, IOTX_DM_SERVICE_ALL,   iotx_dcs_thing_topo_get_reply               },
	{IOTX_DCS_THING_LIST_FOUND_REPLY,          IOTX_DCS_SYS_PREFIX,         IOTX_DM_DEVICE_GATEWAY, IOTX_DM_SERVICE_ALL,   iotx_dcs_thing_list_found_reply             },
	{IOTX_DCS_THING_EVENT_PROPERTY_POST_REPLY, IOTX_DCS_SYS_PREFIX,         IOTX_DM_DEVICE_ALL,     IOTX_DM_SERVICE_ALL,   iotx_dcs_thing_event_property_post_reply    },
	{IOTX_DCS_THING_DEVICEINFO_UPDATE_REPLY,   IOTX_DCS_SYS_PREFIX,         IOTX_DM_DEVICE_ALL,     IOTX_DM_SERVICE_ALL,   iotx_dcs_thing_deviceinfo_update_reply      },
	{IOTX_DCS_THING_DEVICEINFO_DELETE_REPLY,   IOTX_DCS_SYS_PREFIX,         IOTX_DM_DEVICE_ALL,     IOTX_DM_SERVICE_ALL,   iotx_dcs_thing_deviceinfo_delete_reply      },
	{IOTX_DCS_THING_DSLTEMPLATE_GET_REPLY,     IOTX_DCS_SYS_PREFIX,         IOTX_DM_DEVICE_ALL,     IOTX_DM_SERVICE_ALL,   iotx_dcs_thing_dsltemplate_get_reply        },
	{IOTX_DCS_THING_DYNAMICTSL_GET_REPLY,      IOTX_DCS_SYS_PREFIX,         IOTX_DM_DEVICE_ALL,     IOTX_DM_SERVICE_ALL,   iotx_dcs_thing_dynamictsl_get_reply         },
	{IOTX_DCS_COMBINE_LOGIN_REPLY,             IOTX_DCS_EXT_SESSION_PREFIX, IOTX_DM_DEVICE_GATEWAY, IOTX_DM_SERVICE_ALL,   iotx_dcs_combine_login_reply                },
	{IOTX_DCS_COMBINE_LOGOUT_REPLY,            IOTX_DCS_EXT_SESSION_PREFIX, IOTX_DM_DEVICE_GATEWAY, IOTX_DM_SERVICE_ALL,   iotx_dcs_combine_logout_reply               },
	{IOTX_DCS_THING_MODEL_UP_RAW_REPLY,        IOTX_DCS_SYS_PREFIX,         IOTX_DM_DEVICE_ALL,     IOTX_DM_SERVICE_ALL,   iotx_dcs_thing_model_up_raw_reply           }
};

static const iotx_dcs_event_mapping_t g_iotx_dcs_event_mapping[] DM_READ_ONLY = {
	{IOTX_CM_EVENT_CLOUD_CONNECTED,         iotx_dcs_event_cloud_connected_handler      },
	{IOTX_CM_EVENT_CLOUD_DISCONNECT,        iotx_dcs_event_cloud_disconnect_handler     },
	{IOTX_CM_EVENT_CLOUD_RECONNECT,         iotx_dcs_event_cloud_reconnect_handler      },
	{IOTX_CM_EVENT_LOCAL_CONNECTED,         iotx_dcs_event_local_connected_handler      },
	{IOTX_CM_EVENT_LOCAL_DISCONNECT,        iotx_dcs_event_local_disconnect_handler     },
	{IOTX_CM_EVENT_LOCAL_RECONNECT,         iotx_dcs_event_local_reconnect_handler      },
	{IOTX_CM_EVENT_FOUND_DEVICE,            iotx_dcs_event_found_device_handler         },
	{IOTX_CM_EVENT_REMOVE_DEVICE,           iotx_dcs_event_remove_device_handler        },
	{IOTX_CM_EVENT_REGISTER_RESULT,         iotx_dcs_event_register_result_handler      },
	{IOTX_CM_EVENT_UNREGISTER_RESULT,       iotx_dcs_event_unregister_result_handler    },
	{IOTX_CM_EVENT_SEND_RESULT,             iotx_dcs_event_send_result_handler          },
	{IOTX_CM_EVENT_ADD_SERVICE_RESULT,      iotx_dcs_event_add_service_result_handler   },
	{IOTX_CM_EVENT_REMOVE_SERVICE_RESULT,   iotx_dcs_event_remove_service_result_handler},
	{IOTX_CM_EVENT_NEW_DATA_RECEIVED,       iotx_dcs_event_new_data_received_handler    }
};

static iotx_dcs_ctx_t g_iotx_dcs_ctx = {0};

/*****************************************************************************/

static iotx_dcs_ctx_t* _iotx_dcs_get_ctx(void)
{
	return &g_iotx_dcs_ctx;
}

#if 0
static void _iotx_dcs_mutex_lock(void)
{
	iotx_dcs_ctx_t *ctx = _iotx_dcs_get_ctx();
	if (ctx->mutex) {HAL_MutexLock(ctx->mutex);}
}

static void _iotx_dcs_mutex_unlock(void)
{
	iotx_dcs_ctx_t *ctx = _iotx_dcs_get_ctx();
	if (ctx->mutex) {HAL_MutexUnlock(ctx->mutex);}
}
#endif

int iotx_dcs_uri_prefix_sys_split(_IN_ char *uri, _IN_ int uri_len, _OU_ int *start, _OU_ int *end)
{
	int res = 0, offset = 0;

	if (uri == NULL || uri_len <= 0) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	/*"/sys/%s/%s/"*/
	res = iotx_dcm_memtok(uri,uri_len,IOTX_DCS_SERVICE_DELIMITER,2,&offset);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	if (memcmp(IOTX_DCS_SYS_PREFIX,uri,offset+1) != 0) {return FAIL_RETURN;}

	if (start) {*start = offset;}
	if (end) {*end = uri_len - offset - 1;}

	return SUCCESS_RETURN;
}

int iotx_dcs_uri_prefix_ext_session_split(_IN_ char *uri, _IN_ int uri_len, _OU_ int *start, _OU_ int *end)
{
	int res = 0, offset = 0;

	if (uri == NULL || uri_len <= 0) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	/*"/ext/session/%s/%s/"*/
	res = iotx_dcm_memtok(uri,uri_len,IOTX_DCS_SERVICE_DELIMITER,3,&offset);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	if (memcmp(IOTX_DCS_EXT_SESSION_PREFIX,uri,offset+1) != 0) {return FAIL_RETURN;}
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	if (start) {*start = offset;}
	if (end) {*end = uri_len - offset - 1;}

	return SUCCESS_RETURN;
}

int iotx_dcs_uri_pkdn_split(_IN_ char *uri, _IN_ int uri_len, _OU_ int *start, _OU_ int *end)
{
	int res = 0, offset = 0;

	if (uri == NULL || uri_len <= 0 || start == NULL || end == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	/* "/pk/dn/" */
	res = iotx_dcm_memtok(uri,uri_len,IOTX_DCS_SERVICE_DELIMITER,3,&offset);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	*start = offset;
	*end = uri_len - offset - 1;

	return SUCCESS_RETURN;
}

int iotx_dcs_uri_service_specific_split(_IN_ char *uri, _IN_ int uri_len, _OU_ int *start, _OU_ int *end)
{
	int res = 0, offset = 0;
	const char *prefix = "/thing/service/";

	if (uri == NULL || uri_len <= 0 || start == NULL || end == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	/* "/thing/service/%s" */
	res = iotx_dcm_memtok(uri,uri_len,IOTX_DCS_SERVICE_DELIMITER,3,&offset);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	/* Check "thing/service" */
	if (memcmp(IOTX_DCS_THING_SERVICE_REQUEST,uri + 1,offset) != 0) {return FAIL_RETURN;}

	if (uri_len <= strlen(prefix)) {return FAIL_RETURN;}

	*start = offset;
	*end = uri_len - 1;

	return SUCCESS_RETURN;
}

int iotx_dcs_uri_event_specific_split(_IN_ char *uri, _IN_ int uri_len, _OU_ int *start, _OU_ int *end)
{
	int res = 0, offset = 0;
	const char *suffix = "post_reply";

	if (uri == NULL || uri_len <= 0 || start == NULL || end == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	/* "/thing/event/%s/post_reply" */
	res = iotx_dcm_memtok(uri,uri_len,IOTX_DCS_SERVICE_DELIMITER,3,&offset);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	/* Check "thing/event/" */
	if (memcmp(IOTX_DCS_THING_EVENT_POST_REPLY,uri + 1,offset) != 0) {return FAIL_RETURN;}

	/* Check "post_reply" */
	if (uri_len <= strlen(suffix)) {return FAIL_RETURN;}
	if (memcmp(uri + (uri_len - strlen(suffix)),suffix,strlen(suffix)) != 0) {return FAIL_RETURN;}

	*start = offset;
	*end = uri_len - strlen(suffix) - 1;

	return SUCCESS_RETURN;
}

int iotx_dcs_topic_subscribe(char *uri)
{
	int res = 0;
	iotx_dcs_ctx_t *ctx = _iotx_dcs_get_ctx();

	if (uri == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	dm_log_debug("Current Subscribe Topic: %s",uri);

	res = iotx_dcw_cloud_register(ctx->cloud_connectivity,uri,ctx);
	if (res == FAIL_RETURN) {dm_log_warning(IOTX_DM_LOG_DMGR_SERVICE_CLOUD_REGISTER_FAILED,strlen(uri),uri);}

	return SUCCESS_RETURN;
}

int iotx_dcs_topic_generic_subscribe(int devid, int index)
{
	int res = 0, dev_type = 0, search_index = 0;
	char product_key[PRODUCT_KEY_MAXLEN] = {0};
	char device_name[DEVICE_NAME_MAXLEN] = {0};
	char *service_name = NULL;
	iotx_dcs_ctx_t *ctx = _iotx_dcs_get_ctx();
	iotx_dcs_topic_mapping_t *dcs_mapping = (iotx_dcs_topic_mapping_t *)g_iotx_dcs_topic_mapping;

	if (devid < 0 || index < 0 || index >= sizeof(g_iotx_dcs_topic_mapping)/sizeof(iotx_dcs_topic_mapping_t)) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	res = iotx_dmgr_get_product_key(devid,product_key);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	res = iotx_dmgr_get_device_name(devid,device_name);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	res = iotx_dmgr_get_dev_type(devid,&dev_type);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	for (search_index = index;search_index < sizeof(g_iotx_dcs_topic_mapping)/sizeof(iotx_dcs_topic_mapping_t);search_index++) {
		if (dcs_mapping[search_index].dev_type & dev_type) {break;}
	}

	if (search_index == sizeof(g_iotx_dcs_topic_mapping)/sizeof(iotx_dcs_topic_mapping_t)) {
		/* No More Topic Need To Be Subscribed */
		return search_index;
	}

	res = iotx_dcm_service_name((char *)dcs_mapping[search_index].service_prefix,(char *)dcs_mapping[search_index].service_name,product_key,device_name,&service_name);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	/* Subscribe Cloud Service */
	dm_log_debug("Current Subscribe Topic: %s",service_name);
	res = iotx_dcw_cloud_register(ctx->cloud_connectivity,service_name,ctx);
	if (res == FAIL_RETURN) {dm_log_warning(IOTX_DM_LOG_DMGR_SERVICE_CLOUD_REGISTER_FAILED,strlen(service_name),service_name);}
	DM_free(service_name);

	iotx_dmgr_set_dev_sub_generic_index(devid,search_index);

	return SUCCESS_RETURN;
}

int iotx_dcs_topic_service_event_subscribe(int devid, int index)
{
	int res = 0;
	char *service_event = NULL;
	iotx_dcs_ctx_t *ctx = _iotx_dcs_get_ctx();

	res = iotx_dmgr_get_dev_sub_service_event(devid,index,&service_event);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	/* Subscribe Cloud Service */
	dm_log_debug("Current Subscribe Topic: %s",service_event);
	res = iotx_dcw_cloud_register(ctx->cloud_connectivity,service_event,ctx);
	if (res == FAIL_RETURN) {dm_log_warning(IOTX_DM_LOG_DMGR_SERVICE_CLOUD_REGISTER_FAILED,strlen(service_event),service_event);}

	DM_free(service_event);

	iotx_dmgr_set_dev_sub_service_event_index(devid,index);

	return SUCCESS_RETURN;
}

int iotx_dcs_topic_service_event_create(int devid)
{
	int res = 0, index = 0, service_number = 0, event_number = 0;
	char product_key[PRODUCT_KEY_MAXLEN] = {0};
	char device_name[DEVICE_NAME_MAXLEN] = {0};
	void *reference = NULL;
	char *method = NULL, *method_reply = NULL;
	char **service_event = NULL;
	int service_event_index = 0;

	if (devid < 0) {return FAIL_RETURN;}

	res = iotx_dmgr_get_product_key(devid,product_key);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}
	dm_log_debug("Current Shadow Product Key: %s",product_key);

	res = iotx_dmgr_get_device_name(devid,device_name);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}
	dm_log_debug("Current Shadow Device Name: %s",device_name);

	res = iotx_dmgr_get_service_number(devid,&service_number);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	res = iotx_dmgr_get_event_number(devid,&event_number);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	if (service_number + event_number == 0) {return SUCCESS_RETURN;}

	service_event = DM_malloc((service_number + event_number)*sizeof(char *));
	if (service_event == NULL) {
		dm_log_err(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(service_event,0,(service_number + event_number)*sizeof(char *));

	dm_log_debug("Current Shadow Service Number: %d",service_number);
	for (index = 0;index < service_number;index++) {
		reference = NULL;method = NULL;

		res = iotx_dmgr_get_service_by_index(devid,index,&reference);
		if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

		res = iotx_dmgr_get_service_method(reference,&method);
		if (res != SUCCESS_RETURN) {return FAIL_RETURN;}
		dm_log_debug("Service Index: %d, Method: %s",index,method);

		res = iotx_dcm_replace_char(method,strlen(method),'.','/');
		if (res != SUCCESS_RETURN) {DM_free(method);return FAIL_RETURN;}

		res = iotx_dcm_service_name((char *)IOTX_DCS_SYS_PREFIX,method,product_key,device_name,(service_event + service_event_index));
		if (res != SUCCESS_RETURN) {DM_free(method);return FAIL_RETURN;}

		dm_log_debug("Current Service Event Generate: %s",*(service_event + service_event_index));

		DM_free(method);service_event_index++;
	}

	dm_log_debug("Current Shadow TSL Event Number: %d",event_number);
	for (index = 0;index < event_number;index++) {
		reference = NULL;method = NULL;method_reply = NULL;

		res = iotx_dmgr_get_event_by_index(devid,index,&reference);
		if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

		res = iotx_dmgr_get_event_method(reference,&method);
		if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

		dm_log_debug("TSL Event Index: %d, Method: %s",index,method);

		res = iotx_dcm_replace_char(method,strlen(method),'.','/');
		if (res != SUCCESS_RETURN) {DM_free(method);return FAIL_RETURN;}

		method_reply = DM_malloc(strlen(method) + strlen(IOTX_DCS_REPLY_SUFFIX) + 1);
		if (method_reply == NULL) {
			DM_free(method);
			dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
			return FAIL_RETURN;
		}
		memset(method_reply,0,strlen(method) + strlen(IOTX_DCS_REPLY_SUFFIX) + 1);
		memcpy(method_reply,method,strlen(method));
		memcpy(method_reply + strlen(method_reply),IOTX_DCS_REPLY_SUFFIX,strlen(IOTX_DCS_REPLY_SUFFIX));
		
		res = iotx_dcm_service_name((char *)IOTX_DCS_SYS_PREFIX,method_reply,product_key,device_name,(service_event + service_event_index));
		if (res != SUCCESS_RETURN) {DM_free(method);DM_free(method_reply);return FAIL_RETURN;}

		dm_log_debug("Current Service Event Generate: %s",*(service_event + service_event_index));

		DM_free(method);DM_free(method_reply);service_event_index++;
	}

	res = iotx_dmgr_set_dev_sub_service_event(devid,(service_number + event_number),service_event);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	res = iotx_dmgr_get_dev_sub_generic_index(devid,&index);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	if (index == IOTX_DMGR_DEV_SUB_END) {
		dm_log_debug("Current Device %d Has Already Subscribe All Generic Topic");
		iotx_dcs_topic_service_event_subscribe(devid,0);
	}
	iotx_dmgr_set_dev_sub_service_event_index(devid,IOTX_DMGR_DEV_SUB_START);

	return SUCCESS_RETURN;
}

int iotx_dcs_topic_service_event_destroy(int devid)
{
	return iotx_dmgr_clear_dev_sub_service_event(devid);
}

int iotx_dcs_topic_generic_unsubscribe(int devid)
{
	int res = 0, index = 0;
	char product_key[PRODUCT_KEY_MAXLEN] = {0};
	char device_name[DEVICE_NAME_MAXLEN] = {0};
	char *service_name = NULL;
	iotx_dcs_ctx_t *ctx = _iotx_dcs_get_ctx();
	iotx_dcs_topic_mapping_t *dcs_mapping = (iotx_dcs_topic_mapping_t *)g_iotx_dcs_topic_mapping;

	if (devid < 0 || index < 0 || index >= sizeof(g_iotx_dcs_topic_mapping)/sizeof(iotx_dcs_topic_mapping_t)) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	res = iotx_dmgr_get_product_key(devid,product_key);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	res = iotx_dmgr_get_device_name(devid,device_name);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	for (index = 0;index < sizeof(g_iotx_dcs_topic_mapping)/sizeof(iotx_dcs_topic_mapping_t);index++) {
		service_name = NULL;

		res = iotx_dcm_service_name((char *)dcs_mapping[index].service_prefix,(char *)dcs_mapping[index].service_name,product_key,device_name,&service_name);
		if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

		/* Unsubscribe Cloud Service */
		dm_log_debug("Current Unsubscribe Topic: %s",service_name);
		res = iotx_dcw_cloud_unregister(ctx->cloud_connectivity,service_name);
		if (res == FAIL_RETURN) {dm_log_warning(IOTX_DM_LOG_DMGR_SERVICE_CLOUD_REGISTER_FAILED,strlen(service_name),service_name);}
		DM_free(service_name);
	}

	return SUCCESS_RETURN;
}


int iotx_dcs_topic_service_event_unsubscribe(int devid)
{
	int res = 0, index = 0, service_number = 0, event_number = 0;
	char product_key[PRODUCT_KEY_MAXLEN] = {0};
	char device_name[DEVICE_NAME_MAXLEN] = {0};
	iotx_dcs_ctx_t *ctx = _iotx_dcs_get_ctx();
	void *reference = NULL;
	char *service_name = NULL, *method = NULL;

	if (devid < 0) {return FAIL_RETURN;}

	res = iotx_dmgr_get_product_key(devid,product_key);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}
	dm_log_debug("Current Shadow Product Key: %s",product_key);

	res = iotx_dmgr_get_device_name(devid,device_name);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}
	dm_log_debug("Current Shadow Device Name: %s",device_name);

	res = iotx_dmgr_get_service_number(devid,&service_number);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	res = iotx_dmgr_get_event_number(devid,&event_number);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	dm_log_debug("Current Shadow Service Number: %d",service_number);
	for (index = 0;index < service_number;index++) {
		service_name = NULL;reference = NULL;method = NULL;

		res = iotx_dmgr_get_service_by_index(devid,index,&reference);
		if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

		res = iotx_dmgr_get_service_method(reference,&method);
		if (res != SUCCESS_RETURN) {return FAIL_RETURN;}
		dm_log_debug("Service Index: %d, Method: %s",index,method);

		res = iotx_dcm_replace_char(method,strlen(method),'.','/');
		if (res != SUCCESS_RETURN) {DM_free(method);return FAIL_RETURN;}

		res = iotx_dcm_service_name((char *)IOTX_DCS_SYS_PREFIX,method,product_key,device_name,&service_name);
		if (res != SUCCESS_RETURN) {DM_free(method);return FAIL_RETURN;}

		dm_log_debug("Current Unsubscribe Topic: %s",service_name);

		res = iotx_dcw_cloud_unregister(ctx->cloud_connectivity,service_name);
		if (res == FAIL_RETURN) {dm_log_warning(IOTX_DM_LOG_DMGR_SERVICE_CLOUD_REGISTER_FAILED,strlen(service_name),service_name);}
		DM_free(service_name);DM_free(method);
	}

	dm_log_debug("Current Shadow TSL Event Number: %d",event_number);
	for (index = 0;index < event_number;index++) {
		service_name = NULL;reference = NULL;method = NULL;

		res = iotx_dmgr_get_event_by_index(devid,index,&reference);
		if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

		res = iotx_dmgr_get_event_method(reference,&method);
		if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

		dm_log_debug("TSL Event Index: %d, Method: %s",index,method);

		res = iotx_dcm_replace_char(method,strlen(method),'.','/');
		if (res != SUCCESS_RETURN) {DM_free(method);return FAIL_RETURN;}

		res = iotx_dcm_service_name((char *)IOTX_DCS_SYS_PREFIX,method,product_key,device_name,&service_name);
		if (res != SUCCESS_RETURN) {DM_free(method);return FAIL_RETURN;}

		dm_log_debug("Current Unsubscribe Topic: %s",service_name);

		res = iotx_dcw_cloud_unregister(ctx->cloud_connectivity,service_name);
		if (res == FAIL_RETURN) {dm_log_warning(IOTX_DM_LOG_DMGR_SERVICE_CLOUD_REGISTER_FAILED,strlen(service_name),service_name);}
		DM_free(service_name);DM_free(method);
	}

	return SUCCESS_RETURN;
}

int iotx_dcs_topic_local_generic_register(void)
{
	int res = 0, index = 0;
	iotx_dm_message_auth_types_t service_auth;
	char product_key[PRODUCT_KEY_MAXLEN] = {0};
	char device_name[DEVICE_NAME_MAXLEN] = {0};
	char *service_name = NULL;
	iotx_dcs_ctx_t *ctx = _iotx_dcs_get_ctx();
	iotx_dcs_topic_mapping_t *dcs_mapping = iotx_dcs_get_topic_mapping();

	HAL_GetProductKey(product_key);
	HAL_GetDeviceName(device_name);

	for (index = 0;index < iotx_dcs_get_topic_mapping_size();index++)
	{
		service_name = NULL;
		if (!(dcs_mapping[index].service_type & IOTX_DM_SERVICE_LOCAL)) {continue;}
		service_auth = (dcs_mapping[index].service_type & IOTX_DM_SERVICE_LOCAL_AUTH)?(IOTX_DM_MESSAGE_AUTH):(IOTX_DM_MESSAGE_NO_AUTH);
		res = iotx_dcm_service_name((char *)dcs_mapping[index].service_prefix,(char *)dcs_mapping[index].service_name,product_key,device_name,&service_name);
		if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

		/* Subscribe Cloud Service */
		res = iotx_dcw_local_add_service(ctx->local_connectivity,service_name,service_auth,ctx);
		if (res == FAIL_RETURN) {dm_log_warning(IOTX_DM_LOG_DMGR_SERVICE_LOCAL_REGISTER_FAILED,strlen(service_name),service_name);}
		DM_free(service_name);
	}

	return SUCCESS_RETURN;
}

int iotx_dcs_init(void)
{
	int res = 0;
	iotx_dcs_ctx_t *ctx = _iotx_dcs_get_ctx();

	memset(ctx,0,sizeof(iotx_dcs_ctx_t));

	/* Create Mutex */
	ctx->mutex = HAL_MutexCreate();
	if (ctx->mutex == NULL) {
		dm_log_err(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}

	/* Create Cloud Connectivity */
	res = iotx_dcw_conn_cloud_mqtt_init(&ctx->cloud_connectivity);
	if (res == ERROR_NO_MEM) {goto ERROR;}

	/* Create Local Connectivity */
#ifdef CONFIG_DM_LOCAL_ENABLE
	res = iotx_dcw_conn_local_alcs_init(&ctx->local_connectivity);
	if (res == ERROR_NO_MEM) {goto ERROR;}
#endif

	return SUCCESS_RETURN;
ERROR:
#ifdef CONFIG_DM_LOCAL_ENABLE
	if (ctx->local_connectivity) {iotx_dcw_conn_destroy(&ctx->local_connectivity);}
#endif
	if (ctx->cloud_connectivity) {iotx_dcw_conn_destroy(&ctx->cloud_connectivity);}
	if (ctx->mutex) {HAL_MutexDestroy(ctx->mutex);}

	return FAIL_RETURN;
}

int iotx_dcs_deinit(void)
{
	iotx_dcs_ctx_t *ctx = _iotx_dcs_get_ctx();

	#ifdef CONFIG_DM_LOCAL_ENABLE
	if (ctx->local_connectivity) {iotx_dcw_conn_destroy(&ctx->local_connectivity);}
#endif
	if (ctx->cloud_connectivity) {iotx_dcw_conn_destroy(&ctx->cloud_connectivity);}
	if (ctx->mutex) {HAL_MutexDestroy(ctx->mutex);}

	return SUCCESS_RETURN;
}

iotx_dcs_resource_mapping_t* iotx_dcs_get_resource_mapping(void)
{
	return (iotx_dcs_resource_mapping_t *)g_iotx_dcs_resource_mapping;
}

int iotx_dcs_get_resource_mapping_size(void)
{
	return sizeof(g_iotx_dcs_resource_mapping)/sizeof(iotx_dcs_resource_mapping_t);
}

iotx_dcs_topic_mapping_t* iotx_dcs_get_topic_mapping(void)
{
	return (iotx_dcs_topic_mapping_t *)g_iotx_dcs_topic_mapping;
}

int iotx_dcs_get_topic_mapping_size(void)
{
	return sizeof(g_iotx_dcs_topic_mapping)/sizeof(iotx_dcs_topic_mapping_t);
}

int iotx_dcs_get_topic_mapping_dev_type_mask(_IN_ int index, _OU_ int *dev_type_mask)
{
	iotx_dcs_topic_mapping_t *dcs_mapping = (iotx_dcs_topic_mapping_t *)g_iotx_dcs_topic_mapping;

	if (index < 0 || dev_type_mask == NULL ||
		index >= sizeof(g_iotx_dcs_topic_mapping)/sizeof(iotx_dcs_topic_mapping_t)) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	*dev_type_mask = dcs_mapping[index].dev_type;

	return SUCCESS_RETURN;
}

iotx_dcs_event_mapping_t* iotx_dcs_get_event_mapping(void)
{
	return (iotx_dcs_event_mapping_t *)g_iotx_dcs_event_mapping;
}

int iotx_dcs_get_event_mapping_size(void)
{
	return sizeof(g_iotx_dcs_event_mapping)/sizeof(iotx_dcs_event_mapping_t);
}

void iotx_dcs_thing_topo_add_notify(iotx_cm_send_peer_t* source, iotx_cm_message_info_t* msg, void* user_data)
{
	int res = 0;
	iotx_dmsg_request_payload_t request;
	iotx_dmsg_response_t response;

	dm_log_debug(IOTX_DCS_THING_TOPO_ADD_NOTIFY);

	/* Request */
	res = iotx_dmsg_request_parse(msg->payload,msg->payload_length,&request);
	if (res == SUCCESS_RETURN) {
		res = iotx_dmsg_topo_add_notify(request.params.value,request.params.value_length);
	}

	/* Response */
	response.service_prefix = IOTX_DCS_SYS_PREFIX;
	response.service_name = IOTX_DCS_THING_TOPO_ADD_NOTIFY_REPLY;
	memcpy(response.product_key,source->product_key,strlen(source->product_key));
	memcpy(response.device_name,source->device_name,strlen(source->device_name));
	response.code = (res == SUCCESS_RETURN)?(IOTX_DM_ERR_CODE_SUCCESS):(IOTX_DM_ERR_CODE_REQUEST_ERROR);
	iotx_dmsg_response_without_data(&request,&response);
}

void iotx_dcs_thing_service_property_set(iotx_cm_send_peer_t* source, iotx_cm_message_info_t* msg, void* user_data)
{
	int res = 0, devid = 0;
	char product_key[PRODUCT_KEY_MAXLEN] = {0};
	char device_name[DEVICE_NAME_MAXLEN] = {0};
	iotx_dmsg_request_payload_t request;
	iotx_dmsg_response_t response;

	memset(&request,0,sizeof(iotx_dmsg_request_payload_t));
	memset(&response,0,sizeof(iotx_dmsg_response_t));

	/* Request */
	res = iotx_dmsg_uri_parse_pkdn(msg->URI,msg->URI_length,2,4,product_key,device_name);
	if (res != SUCCESS_RETURN) {return;}

	res = iotx_dmgr_search_device_by_pkdn(product_key,device_name,&devid);
	if (res != SUCCESS_RETURN) {return;}

	res = iotx_dmsg_request_parse(msg->payload,msg->payload_length,&request);
	if (res != SUCCESS_RETURN) {return;}

	/* Operation */
	res = iotx_dmsg_property_set(devid,&request);

	/* Response */
	response.service_prefix = IOTX_DCS_SYS_PREFIX;
	response.service_name = IOTX_DCS_THING_SERVICE_PROPERTY_SET_REPLY;
	memcpy(response.product_key,product_key,strlen(product_key));
	memcpy(response.device_name,device_name,strlen(device_name));
	response.code = (res == SUCCESS_RETURN)?(IOTX_DM_ERR_CODE_SUCCESS):(IOTX_DM_ERR_CODE_REQUEST_ERROR);
	iotx_dmsg_response_without_data(&request,&response);

}

void iotx_dcs_thing_service_property_get(iotx_cm_send_peer_t* source, iotx_cm_message_info_t* msg, void* user_data)
{
	/* Never Used */
	dm_log_debug("Serivce Property Get, Payload: %.*s",msg->payload_length,msg->payload);
}

void iotx_dcs_thing_service_request(iotx_cm_send_peer_t* source, iotx_cm_message_info_t* msg, char *identifier, int identifier_len, void* user_data)
{
	int res = 0;
	char product_key[PRODUCT_KEY_MAXLEN] = {0};
	char device_name[DEVICE_NAME_MAXLEN] = {0};
	iotx_dmsg_request_payload_t request;

	dm_log_debug(IOTX_DCS_THING_SERVICE_REQUEST,"{Identifier}");
	dm_log_debug("Current URI: %.*s",msg->URI_length,msg->URI);
	dm_log_debug("Current Identifier: %.*s",identifier_len,identifier);

	/* Parse Product Key And Device Name */
	res = iotx_dmsg_uri_parse_pkdn(msg->URI,msg->URI_length,2,4,product_key,device_name);
	if (res != SUCCESS_RETURN) {return;}

	/* Request */
	res = iotx_dmsg_request_parse(msg->payload,msg->payload_length,&request);
	if (res != SUCCESS_RETURN) {return;}

	/* Operation */
	res = iotx_dmsg_thing_service_request(product_key,device_name,identifier,identifier_len,&request);
	if (res != SUCCESS_RETURN) {return;}
}

void iotx_dcs_thing_disable(iotx_cm_send_peer_t* source, iotx_cm_message_info_t* msg, void* user_data)
{
	int res = 0;
	iotx_dmsg_request_payload_t request;
	iotx_dmsg_response_t response;

	dm_log_debug(IOTX_DCS_THING_DISABLE);

	memset(&request,0,sizeof(iotx_dmsg_request_payload_t));
	memset(&response,0,sizeof(iotx_dmsg_response_t));

	/* Request */
	res = iotx_dmsg_request_parse(msg->payload,msg->payload_length,&request);
	if (res != SUCCESS_RETURN) {return;}

	/* Operation */
	res = iotx_dmsg_thing_disable(source->product_key,source->device_name);

	/* Response */
	response.service_prefix = IOTX_DCS_SYS_PREFIX;
	response.service_name = IOTX_DCS_THING_DISABLE_REPLY;
	memcpy(response.product_key,source->product_key,strlen(source->product_key));
	memcpy(response.device_name,source->device_name,strlen(source->device_name));
	response.code = (res == SUCCESS_RETURN)?(IOTX_DM_ERR_CODE_SUCCESS):(IOTX_DM_ERR_CODE_REQUEST_ERROR);
	iotx_dmsg_response_without_data(&request,&response);
}

void iotx_dcs_thing_enable(iotx_cm_send_peer_t* source, iotx_cm_message_info_t* msg, void* user_data)
{
	int res = 0;
	iotx_dmsg_request_payload_t request;
	iotx_dmsg_response_t response;

	dm_log_debug(IOTX_DCS_THING_ENABLE);

	memset(&request,0,sizeof(iotx_dmsg_request_payload_t));
	memset(&response,0,sizeof(iotx_dmsg_response_t));

	/* Request */
	res = iotx_dmsg_request_parse(msg->payload,msg->payload_length,&request);
	if (res != SUCCESS_RETURN) {return;}

	/* Operation */
	res = iotx_dmsg_thing_enable(source->product_key,source->device_name);

	/* Response */
	response.service_prefix = IOTX_DCS_SYS_PREFIX;
	response.service_name = IOTX_DCS_THING_ENABLE_REPLY;
	memcpy(response.product_key,source->product_key,strlen(source->product_key));
	memcpy(response.device_name,source->device_name,strlen(source->device_name));
	response.code = (res == SUCCESS_RETURN)?(IOTX_DM_ERR_CODE_SUCCESS):(IOTX_DM_ERR_CODE_REQUEST_ERROR);
	iotx_dmsg_response_without_data(&request,&response);
}

void iotx_dcs_thing_delete(iotx_cm_send_peer_t* source, iotx_cm_message_info_t* msg, void* user_data)
{
	int res = 0;
	iotx_dmsg_request_payload_t request;
	iotx_dmsg_response_t response;

	dm_log_debug(IOTX_DCS_THING_DELETE);

	memset(&request,0,sizeof(iotx_dmsg_request_payload_t));
	memset(&response,0,sizeof(iotx_dmsg_response_t));

	/* Request */
	res = iotx_dmsg_request_parse(msg->payload,msg->payload_length,&request);
	if (res != SUCCESS_RETURN) {return;}

	/* Operation */
	res = iotx_dmsg_thing_delete(source->product_key,source->device_name);

	/* Response */
	response.service_prefix = IOTX_DCS_SYS_PREFIX;
	response.service_name = IOTX_DCS_THING_DELETE_REPLY;
	memcpy(response.product_key,source->product_key,strlen(source->product_key));
	memcpy(response.device_name,source->device_name,strlen(source->device_name));
	response.code = (res == SUCCESS_RETURN)?(IOTX_DM_ERR_CODE_SUCCESS):(IOTX_DM_ERR_CODE_REQUEST_ERROR);
	iotx_dmsg_response_without_data(&request,&response);

}

void iotx_dcs_thing_model_down_raw(iotx_cm_send_peer_t* source, iotx_cm_message_info_t* msg, void* user_data)
{
	int res = 0;
	char product_key[PRODUCT_KEY_MAXLEN] = {0};
	char device_name[DEVICE_NAME_MAXLEN] = {0};

	dm_log_debug(IOTX_DCS_THING_MODEL_DOWN_RAW);

	res = iotx_dmsg_uri_parse_pkdn(msg->URI,msg->URI_length,2,4,product_key,device_name);
	if (res != SUCCESS_RETURN) {return;}

	dm_log_debug("Product Key: %s, Device Name: %s",product_key,device_name);

	iotx_dmsg_thing_model_down_raw(product_key,device_name,msg->payload,msg->payload_length);
}

void iotx_dcs_thing_gateway_permit(iotx_cm_send_peer_t* source, iotx_cm_message_info_t* msg, void* user_data)
{
	int res = 0;
	iotx_dmsg_request_payload_t request;
	iotx_dmsg_response_t response;

	dm_log_debug(IOTX_DCS_THING_GATEWAY_PERMIT);

	memset(&request,0,sizeof(iotx_dmsg_request_payload_t));
	memset(&response,0,sizeof(iotx_dmsg_response_t));

	/* Request */
	res = iotx_dmsg_request_parse(msg->payload,msg->payload_length,&request);
	if (res != SUCCESS_RETURN) {return;}

	/* Operation */
	res = iotx_dmsg_thing_gateway_permit(request.params.value,request.params.value_length);

	/* Response */
	response.service_prefix = IOTX_DCS_SYS_PREFIX;
	response.service_name = IOTX_DCS_THING_GATEWAY_PERMIT_REPLY;
	memcpy(response.product_key,source->product_key,strlen(source->product_key));
	memcpy(response.device_name,source->device_name,strlen(source->device_name));
	response.code = (res == SUCCESS_RETURN)?(IOTX_DM_ERR_CODE_SUCCESS):(IOTX_DM_ERR_CODE_REQUEST_ERROR);
	iotx_dmsg_response_without_data(&request,&response);
}

void iotx_dcs_thing_sub_register_reply(iotx_cm_send_peer_t* source, iotx_cm_message_info_t* msg, void* user_data)
{
	int res = 0;
	iotx_dmsg_response_payload_t response;
	char int_id[IOTX_DCM_UINT32_STRLEN] = {0};

	dm_log_debug(IOTX_DCS_THING_SUB_REGISTER_REPLY);

	memset(&response,0,sizeof(iotx_dmsg_response_payload_t));

	/* Response */
	res = iotx_dmsg_response_parse(msg->payload,msg->payload_length,&response);
	if (res != SUCCESS_RETURN) {return;}

	/* Operation */
	iotx_dmsg_thing_sub_register_reply(&response);

	/* Remove Message From Cache */
	memcpy(int_id,response.id.value,response.id.value_length);
	iotx_dmc_msg_remove(atoi(int_id));
}

void iotx_dcs_thing_sub_unregister_reply(iotx_cm_send_peer_t* source, iotx_cm_message_info_t* msg, void* user_data)
{
	int res = 0;
	iotx_dmsg_response_payload_t response;
	char int_id[IOTX_DCM_UINT32_STRLEN] = {0};

	dm_log_debug(IOTX_DCS_THING_SUB_UNREGISTER_REPLY);

	memset(&response,0,sizeof(iotx_dmsg_response_payload_t));

	/* Response */
	res = iotx_dmsg_response_parse(msg->payload,msg->payload_length,&response);
	if (res != SUCCESS_RETURN) {return;}

	/* Operation */
	iotx_dmsg_thing_sub_unregister_reply(&response);

	/* Remove Message From Cache */
	memcpy(int_id,response.id.value,response.id.value_length);
	iotx_dmc_msg_remove(atoi(int_id));
}

void iotx_dcs_thing_topo_add_reply(iotx_cm_send_peer_t* source, iotx_cm_message_info_t* msg, void* user_data)
{
	int res = 0;
	iotx_dmsg_response_payload_t response;
	char int_id[IOTX_DCM_UINT32_STRLEN] = {0};

	dm_log_debug(IOTX_DCS_THING_TOPO_ADD_REPLY);

	memset(&response,0,sizeof(iotx_dmsg_response_payload_t));

	/* Response */
	res = iotx_dmsg_response_parse(msg->payload,msg->payload_length,&response);
	if (res != SUCCESS_RETURN) {return;}

	/* Operation */
	iotx_dmsg_thing_topo_add_reply(&response);

	/* Remove Message From Cache */
	memcpy(int_id,response.id.value,response.id.value_length);
	iotx_dmc_msg_remove(atoi(int_id));
}

void iotx_dcs_thing_topo_delete_reply(iotx_cm_send_peer_t* source, iotx_cm_message_info_t* msg, void* user_data)
{
	int res = 0;
	iotx_dmsg_response_payload_t response;
	char int_id[IOTX_DCM_UINT32_STRLEN] = {0};

	dm_log_debug(IOTX_DCS_THING_TOPO_DELETE_REPLY);


	/* Response */
	res = iotx_dmsg_response_parse(msg->payload,msg->payload_length,&response);
	if (res != SUCCESS_RETURN) {return;}

	/* Operation */
	iotx_dmsg_thing_topo_delete_reply(&response);

	/* Remove Message From Cache */
	memcpy(int_id,response.id.value,response.id.value_length);
	iotx_dmc_msg_remove(atoi(int_id));
}

void iotx_dcs_thing_topo_get_reply(iotx_cm_send_peer_t* source, iotx_cm_message_info_t* msg, void* user_data)
{
	int res = 0;
	iotx_dmsg_response_payload_t response;
	char int_id[IOTX_DCM_UINT32_STRLEN] = {0};

	dm_log_debug(IOTX_DCS_THING_TOPO_GET_REPLY);

	/* Response */
	res = iotx_dmsg_response_parse(msg->payload,msg->payload_length,&response);
	if (res != SUCCESS_RETURN) {return;}

	/* Operation */
	iotx_dmsg_topo_get_reply(&response);

	/* Remove Message From Cache */
	memcpy(int_id,response.id.value,response.id.value_length);
	iotx_dmc_msg_remove(atoi(int_id));
}

void iotx_dcs_thing_list_found_reply(iotx_cm_send_peer_t* source, iotx_cm_message_info_t* msg, void* user_data)
{
	int res = 0;
	iotx_dmsg_response_payload_t response;

	dm_log_debug(IOTX_DCS_THING_LIST_FOUND_REPLY);

	/* Response */
	res = iotx_dmsg_response_parse(msg->payload,msg->payload_length,&response);
	if (res != SUCCESS_RETURN) {return;}

	/* Operation */
	iotx_dmsg_thing_list_found_reply(&response);
}

void iotx_dcs_thing_event_property_post_reply(iotx_cm_send_peer_t* source, iotx_cm_message_info_t* msg, void* user_data)
{
	int res = 0;
	iotx_dmsg_response_payload_t response;
	char int_id[IOTX_DCM_UINT32_STRLEN] = {0};

	dm_log_debug(IOTX_DCS_THING_EVENT_PROPERTY_POST_REPLY);

	/* Response */
	res = iotx_dmsg_response_parse(msg->payload,msg->payload_length,&response);
	if (res != SUCCESS_RETURN) {return;}

	/* Operation */
	iotx_dmsg_thing_event_property_post_reply(&response);

	/* Remove Message From Cache */
	memcpy(int_id,response.id.value,response.id.value_length);
	iotx_dmc_msg_remove(atoi(int_id));
}

void iotx_dcs_thing_event_post_reply(iotx_cm_send_peer_t* source, iotx_cm_message_info_t* msg, char *identifier, int identifier_len, void* user_data)
{
	int res = 0;
	iotx_dmsg_response_payload_t response;
	char int_id[IOTX_DCM_UINT32_STRLEN] = {0};

	dm_log_debug(IOTX_DCS_THING_EVENT_POST_REPLY,"{Identifier}");
	dm_log_debug("Current URI: %.*s",msg->URI_length,msg->URI);
	dm_log_debug("Current Identifier: %.*s",identifier_len,identifier);

	/* Response */
	res = iotx_dmsg_response_parse(msg->payload,msg->payload_length,&response);
	if (res != SUCCESS_RETURN) {return;}

	/* Operation */
	iotx_dmsg_thing_event_post_reply(identifier,identifier_len,&response);

	/* Remove Message From Cache */
	memcpy(int_id,response.id.value,response.id.value_length);
	iotx_dmc_msg_remove(atoi(int_id));
}

void iotx_dcs_thing_deviceinfo_update_reply(iotx_cm_send_peer_t* source, iotx_cm_message_info_t* msg, void* user_data)
{
	int res = 0;
	iotx_dmsg_response_payload_t response;
	char int_id[IOTX_DCM_UINT32_STRLEN] = {0};

	dm_log_debug(IOTX_DCS_THING_DEVICEINFO_UPDATE_REPLY);

	/* Response */
	res = iotx_dmsg_response_parse(msg->payload,msg->payload_length,&response);
	if (res != SUCCESS_RETURN) {return;}

	/* Operation */
	iotx_dmsg_thing_deviceinfo_update_reply(&response);

	/* Remove Message From Cache */
	memcpy(int_id,response.id.value,response.id.value_length);
	iotx_dmc_msg_remove(atoi(int_id));
}

void iotx_dcs_thing_deviceinfo_delete_reply(iotx_cm_send_peer_t* source, iotx_cm_message_info_t* msg, void* user_data)
{
	int res = 0;
	iotx_dmsg_response_payload_t response;
	char int_id[IOTX_DCM_UINT32_STRLEN] = {0};

	dm_log_debug(IOTX_DCS_THING_DEVICEINFO_DELETE_REPLY);

	/* Response */
	res = iotx_dmsg_response_parse(msg->payload,msg->payload_length,&response);
	if (res != SUCCESS_RETURN) {return;}

	/* Operation */
	iotx_dmsg_thing_deviceinfo_delete_reply(&response);

	/* Remove Message From Cache */
	memcpy(int_id,response.id.value,response.id.value_length);
	iotx_dmc_msg_remove(atoi(int_id));
}

void iotx_dcs_thing_dsltemplate_get_reply(iotx_cm_send_peer_t* source, iotx_cm_message_info_t* msg, void* user_data)
{
	int res = 0;
	iotx_dmsg_response_payload_t response;
	char int_id[IOTX_DCM_UINT32_STRLEN] = {0};

	dm_log_debug(IOTX_DCS_THING_DSLTEMPLATE_GET_REPLY);

	/* Response */
	res = iotx_dmsg_response_parse(msg->payload,msg->payload_length,&response);
	if (res != SUCCESS_RETURN) {return;}

	/* Operation */
	iotx_dmsg_thing_dsltemplate_get_reply(&response);

	/* Remove Message From Cache */
	memcpy(int_id,response.id.value,response.id.value_length);
	iotx_dmc_msg_remove(atoi(int_id));
}

void iotx_dcs_thing_dynamictsl_get_reply(iotx_cm_send_peer_t* source, iotx_cm_message_info_t* msg, void* user_data)
{
	int res = 0;
	iotx_dmsg_response_payload_t response;
	char int_id[IOTX_DCM_UINT32_STRLEN] = {0};

	dm_log_debug(IOTX_DCS_THING_DYNAMICTSL_GET_REPLY);

	/* Response */
	res = iotx_dmsg_response_parse(msg->payload,msg->payload_length,&response);
	if (res != SUCCESS_RETURN) {return;}

	/* Operation */
	iotx_dmsg_thing_dynamictsl_get_reply(&response);

	/* Remove Message From Cache */
	memcpy(int_id,response.id.value,response.id.value_length);
	iotx_dmc_msg_remove(atoi(int_id));
}

void iotx_dcs_combine_login_reply(iotx_cm_send_peer_t* source, iotx_cm_message_info_t* msg, void* user_data)
{
	int res = 0;
	iotx_dmsg_response_payload_t response;
	char int_id[IOTX_DCM_UINT32_STRLEN] = {0};

	dm_log_debug(IOTX_DCS_COMBINE_LOGIN_REPLY);


	/* Response */
	res = iotx_dmsg_response_parse(msg->payload,msg->payload_length,&response);
	if (res != SUCCESS_RETURN) {return;}

	/* Operation */
	res = iotx_dmsg_combine_login_reply(&response);
	if (res != SUCCESS_RETURN) {return;}

	/* Remove Message From Cache */
	memcpy(int_id,response.id.value,response.id.value_length);
	iotx_dmc_msg_remove(atoi(int_id));
}

void iotx_dcs_combine_logout_reply(iotx_cm_send_peer_t* source, iotx_cm_message_info_t* msg, void* user_data)
{
	int res = 0;
	iotx_dmsg_response_payload_t response;
	char int_id[IOTX_DCM_UINT32_STRLEN] = {0};

	dm_log_debug(IOTX_DCS_COMBINE_LOGOUT_REPLY);


	/* Response */
	res = iotx_dmsg_response_parse(msg->payload,msg->payload_length,&response);
	if (res != SUCCESS_RETURN) {return;}

	/* Operation */
	res = iotx_dmsg_combine_logout_reply(&response);
	if (res != SUCCESS_RETURN) {return;}

	/* Remove Message From Cache */
	memcpy(int_id,response.id.value,response.id.value_length);
	iotx_dmc_msg_remove(atoi(int_id));
}

void iotx_dcs_thing_model_up_raw_reply(iotx_cm_send_peer_t* source, iotx_cm_message_info_t* msg, void* user_data)
{
	int res = 0;
	iotx_dmsg_response_payload_t response;
	char product_key[PRODUCT_KEY_MAXLEN] = {0};
	char device_name[DEVICE_NAME_MAXLEN] = {0};

	dm_log_debug(IOTX_DCS_THING_MODEL_UP_RAW_REPLY);

	res = iotx_dmsg_uri_parse_pkdn(msg->URI,msg->URI_length,2,4,product_key,device_name);
	if (res != SUCCESS_RETURN) {return;}

	dm_log_debug("Product Key: %s, Device Name: %s",product_key,device_name);

	/* Response */
	res = iotx_dmsg_response_parse(msg->payload,msg->payload_length,&response);
	if (res != SUCCESS_RETURN) {return;}

	/* Operation */
	res = iotx_dmsg_thing_model_up_raw_reply(product_key,device_name,&response);
	if (res != SUCCESS_RETURN) {return;}
}

void iotx_dcs_thing_dev_core_service_dev(iotx_cm_send_peer_t* source, iotx_cm_message_info_t* msg, void* user_data)
{

}

void iotx_dcs_thing_lan_prefix_get_reply(iotx_cm_send_peer_t* source, iotx_cm_message_info_t* msg, void* user_data)
{

}

void iotx_dcs_thing_lan_prefix_update_reply(iotx_cm_send_peer_t* source, iotx_cm_message_info_t* msg, void* user_data)
{

}

void iotx_dcs_user_defined_handler(iotx_cm_send_peer_t* source, iotx_cm_message_info_t* msg, void* user_data)
{
	dm_log_info("USER Defined Handler");
	dm_log_info("URI: %.*s",msg->URI_length,msg->URI);
	dm_log_info("Payload: %.*s",msg->payload_length,msg->payload);
}

void iotx_dcs_event_cloud_connected_handler(void* pcontext, iotx_cm_event_msg_t* msg, void* user_data)
{
	dm_log_info("IOTX_CM_EVENT_CLOUD_CONNECTED");

	/* Re-Subscribe Topic */
	/* Start From Subscribe Generic Topic */
	iotx_dcs_topic_generic_subscribe(IOTX_DMGR_LOCAL_NODE_DEVID,0);

	/* Set Service Event Topic Index To IOTX_DMGR_DEV_SUB_START */
	/* Service Event Topic Subscribe Will Be Execute After All Generic Topic Subscribed */
	iotx_dmgr_set_dev_sub_service_event_index(IOTX_DMGR_LOCAL_NODE_DEVID,IOTX_DMGR_DEV_SUB_START);

	iotx_dmsg_cloud_connected();
}

void iotx_dcs_event_cloud_disconnect_handler(void* pcontext, iotx_cm_event_msg_t* msg, void* user_data)
{
	dm_log_info("IOTX_CM_EVENT_CLOUD_DISCONNECT");

	iotx_dmsg_cloud_disconnect();
}

void iotx_dcs_event_cloud_reconnect_handler(void* pcontext, iotx_cm_event_msg_t* msg, void* user_data)
{
	dm_log_info("IOTX_CM_EVENT_CLOUD_RECONNECT");

	iotx_dmsg_cloud_reconnect();
}

void iotx_dcs_event_local_connected_handler(void* pcontext, iotx_cm_event_msg_t* msg, void* user_data)
{
	dm_log_info("IOTX_CM_EVENT_LOCAL_CONNECTED");

	iotx_dcs_topic_local_generic_register();

	_iotx_dmsg_send_to_user(IOTX_DM_EVENT_LOCAL_CONNECTED,NULL);
}

void iotx_dcs_event_local_disconnect_handler(void* pcontext, iotx_cm_event_msg_t* msg, void* user_data)
{
	dm_log_info("IOTX_CM_EVENT_LOCAL_DISCONNECT");

	_iotx_dmsg_send_to_user(IOTX_DM_EVENT_LOCAL_DISCONNECT,NULL);
}

void iotx_dcs_event_local_reconnect_handler(void* pcontext, iotx_cm_event_msg_t* msg, void* user_data)
{
	dm_log_info("IOTX_CM_EVENT_LOCAL_RECONNECT");

	_iotx_dmsg_send_to_user(IOTX_DM_EVENT_LOCAL_RECONNECT,NULL);
}

void iotx_dcs_event_found_device_handler(void* pcontext, iotx_cm_event_msg_t* msg, void* user_data)
{
	dm_log_info("IOTX_CM_EVENT_FOUND_DEVICE");

	iotx_cm_send_peer_t *cm_send_peer = (iotx_cm_send_peer_t *)msg->msg;

	iotx_dmsg_found_device(cm_send_peer->product_key,cm_send_peer->device_name);
}

void iotx_dcs_event_remove_device_handler(void* pcontext, iotx_cm_event_msg_t* msg, void* user_data)
{
	dm_log_info("IOTX_CM_EVENT_REMOVE_DEVICE");

	iotx_cm_send_peer_t *cm_send_peer = (iotx_cm_send_peer_t *)msg->msg;

	iotx_dmsg_remove_device(cm_send_peer->product_key,cm_send_peer->device_name);
}

void iotx_dcs_event_register_result_handler(void* pcontext, iotx_cm_event_msg_t* msg, void* user_data)
{
	dm_log_info("IOTX_CM_EVENT_REGISTER_RESULT");

	iotx_cm_event_result_t *cm_event_result = (iotx_cm_event_result_t *)msg->msg;

	dm_log_debug("Topic Subscribe Result: %d, URI: %s",cm_event_result->result,cm_event_result->URI);

	iotx_dmsg_register_result(cm_event_result->URI,cm_event_result->result);
}

void iotx_dcs_event_unregister_result_handler(void* pcontext, iotx_cm_event_msg_t* msg, void* user_data)
{
	dm_log_info("IOTX_CM_EVENT_UNREGISTER_RESULT");

	//iotx_cm_event_result_t *cm_event_result = (iotx_cm_event_result_t *)msg->msg;

	//iotx_dmsg_unregister_result(cm_event_result->URI,cm_event_result->result);
}

void iotx_dcs_event_send_result_handler(void* pcontext, iotx_cm_event_msg_t* msg, void* user_data)
{
	dm_log_info("IOTX_CM_EVENT_SEND_RESULT");

	iotx_cm_event_result_t *cm_event_result = (iotx_cm_event_result_t *)msg->msg;

	iotx_dmsg_send_result(cm_event_result->URI,cm_event_result->result);
}

void iotx_dcs_event_add_service_result_handler(void* pcontext, iotx_cm_event_msg_t* msg, void* user_data)
{
	dm_log_info("IOTX_CM_EVENT_ADD_SERVICE_RESULT");

	iotx_cm_event_result_t *cm_event_result = (iotx_cm_event_result_t *)msg->msg;

	iotx_dmsg_add_service_result(cm_event_result->URI,cm_event_result->result);
}

void iotx_dcs_event_remove_service_result_handler(void* pcontext, iotx_cm_event_msg_t* msg, void* user_data)
{
	dm_log_info("IOTX_CM_EVENT_REMOVE_SERVICE_RESULT");

	iotx_cm_event_result_t *cm_event_result = (iotx_cm_event_result_t *)msg->msg;

	iotx_dmsg_remove_service_result(cm_event_result->URI,cm_event_result->result);
}

void iotx_dcs_event_new_data_received_handler(void* pcontext, iotx_cm_event_msg_t* msg, void* user_data)
{
	dm_log_info("IOTX_CM_EVENT_NEW_DATA_RECEIVED");

	iotx_cm_new_data_t *cm_new_data = msg->msg;
	iotx_dcw_topic_callback(cm_new_data->peer,cm_new_data->message_info,user_data);
}

int iotx_dcs_thing_sub_register(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN])
{
	int res = 0;
	iotx_dmsg_request_t request;

	if (product_key == NULL || device_name == NULL ||
		(strlen(product_key) >= PRODUCT_KEY_MAXLEN) ||
		(strlen(device_name) >= DEVICE_NAME_MAXLEN)) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	memset(&request,0,sizeof(iotx_dmsg_request_t));
	request.service_prefix = IOTX_DCS_SYS_PREFIX;
	request.service_name = IOTX_DCS_THING_SUB_REGISTER;
	HAL_GetProductKey(request.product_key);
	HAL_GetDeviceName(request.device_name);

	/* Get Params And Method */
	res = iotx_dmsg_thing_sub_register(product_key,device_name,&request);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	/* Get Msg ID */
	request.msgid = iotx_dmsg_get_id();

	/* Get Dev ID */
	res = iotx_dmgr_search_device_by_pkdn(product_key,device_name,&request.devid);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	/* Send Message To Cloud */
	res = iotx_dmsg_request(&request);
	if (res == SUCCESS_RETURN) {
		iotx_dmc_msg_insert(request.msgid,request.devid,IOTX_DM_EVENT_SUBDEV_REGISTER_REPLY,NULL);
		res = request.msgid;
	}

	DM_free(request.params);

	return res;
}

int iotx_dcs_thing_sub_unregister(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN])
{
	int res = 0;
	iotx_dmsg_request_t request;

	if (product_key == NULL || device_name == NULL ||
		(strlen(product_key) >= PRODUCT_KEY_MAXLEN) ||
		(strlen(device_name) >= DEVICE_NAME_MAXLEN)) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	memset(&request,0,sizeof(iotx_dmsg_request_t));
	request.service_prefix = IOTX_DCS_SYS_PREFIX;
	request.service_name = IOTX_DCS_THING_SUB_UNREGISTER;
	HAL_GetProductKey(request.product_key);
	HAL_GetDeviceName(request.device_name);

	/* Get Params And Method */
	res = iotx_dmsg_thing_sub_unregister(product_key,device_name,&request);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	/* Get Msg ID */
	request.msgid = iotx_dmsg_get_id();

	/* Get Dev ID */
	res = iotx_dmgr_search_device_by_pkdn(product_key,device_name,&request.devid);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	/* Send Message To Cloud */
	res = iotx_dmsg_request(&request);
	if (res == SUCCESS_RETURN) {
		iotx_dmc_msg_insert(request.msgid,request.devid,IOTX_DM_EVENT_SUBDEV_UNREGISTER_REPLY,NULL);
		res = request.msgid;
	}

	DM_free(request.params);

	return res;
}
int iotx_dcs_thing_topo_add(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN], _IN_ char device_secret[DEVICE_SECRET_MAXLEN])
{
	int res = 0;
	iotx_dmsg_request_t request;

	if (product_key == NULL || device_name == NULL || device_secret == NULL ||
		(strlen(product_key) >= PRODUCT_KEY_MAXLEN) ||
		(strlen(device_name) >= DEVICE_NAME_MAXLEN) ||
		(strlen(device_secret) >= DEVICE_SECRET_MAXLEN)) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	memset(&request,0,sizeof(iotx_dmsg_request_t));
	request.service_prefix = IOTX_DCS_SYS_PREFIX;
	request.service_name = IOTX_DCS_THING_TOPO_ADD;
	HAL_GetProductKey(request.product_key);
	HAL_GetDeviceName(request.device_name);

	/* Get Params And Method */
	res = iotx_dmsg_thing_topo_add(product_key,device_name,device_secret,&request);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	/* Get Msg ID */
	request.msgid = iotx_dmsg_get_id();

	/* Get Dev ID */
	res = iotx_dmgr_search_device_by_pkdn(product_key,device_name,&request.devid);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	/* Send Message To Cloud */
	res = iotx_dmsg_request(&request);
	if (res == SUCCESS_RETURN) {
		iotx_dmc_msg_insert(request.msgid,request.devid,IOTX_DM_EVENT_TOPO_ADD_REPLY,NULL);
		res = request.msgid;
	}

	DM_free(request.params);

	return res;
}

int iotx_dcs_thing_topo_delete(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN])
{
	int res = 0;
	iotx_dmsg_request_t request;

	if (product_key == NULL || device_name == NULL ||
		(strlen(product_key) >= PRODUCT_KEY_MAXLEN) ||
		(strlen(device_name) >= DEVICE_NAME_MAXLEN)) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	memset(&request,0,sizeof(iotx_dmsg_request_t));
	request.service_prefix = IOTX_DCS_SYS_PREFIX;
	request.service_name = IOTX_DCS_THING_TOPO_DELETE;
	HAL_GetProductKey(request.product_key);
	HAL_GetDeviceName(request.device_name);

	/* Get Params And Method */
	res = iotx_dmsg_thing_topo_delete(product_key,device_name,&request);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	/* Get Msg ID */
	request.msgid = iotx_dmsg_get_id();

	/* Get Dev ID */
	res = iotx_dmgr_search_device_by_pkdn(product_key,device_name,&request.devid);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	/* Send Message To Cloud */
	res = iotx_dmsg_request(&request);
	if (res == SUCCESS_RETURN) {
		iotx_dmc_msg_insert(request.msgid,request.devid,IOTX_DM_EVENT_TOPO_DELETE_REPLY,NULL);
		res = request.msgid;
	}

	DM_free(request.params);

	return res;
}

int iotx_dcs_thing_topo_get(void)
{
	int res = 0;
	iotx_dmsg_request_t request;

	memset(&request,0,sizeof(iotx_dmsg_request_t));
	request.service_prefix = IOTX_DCS_SYS_PREFIX;
	request.service_name = IOTX_DCS_THING_TOPO_GET;
	HAL_GetProductKey(request.product_key);
	HAL_GetDeviceName(request.device_name);

	/* Get Params And Method */
	res = iotx_dmsg_thing_topo_get(&request);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	/* Get Msg ID */
	request.msgid = iotx_dmsg_get_id();

	/* Get Dev ID */
	res = iotx_dmgr_search_device_by_pkdn(request.product_key,request.device_name,&request.devid);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	/* Send Message To Cloud */
	res = iotx_dmsg_request(&request);
	if (res == SUCCESS_RETURN) {
		iotx_dmc_msg_insert(request.msgid,request.devid,IOTX_DM_EVENT_TOPO_GET_REPLY,NULL);
		res = request.msgid;
	}

	DM_free(request.params);

	return res;
}

int iotx_dcs_thing_list_found(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN])
{
	int res = 0;
	iotx_dmsg_request_t request;

	if (product_key == NULL || device_name == NULL ||
		(strlen(product_key) >= PRODUCT_KEY_MAXLEN) ||
		(strlen(device_name) >= DEVICE_NAME_MAXLEN)) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	memset(&request,0,sizeof(iotx_dmsg_request_t));
	request.service_prefix = IOTX_DCS_SYS_PREFIX;
	request.service_name = IOTX_DCS_THING_LIST_FOUND;
	HAL_GetProductKey(request.product_key);
	HAL_GetDeviceName(request.device_name);

	/* Get Params And Method */
	res = iotx_dmsg_thing_list_found(product_key,device_name,&request);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	/* Get Msg ID */
	request.msgid = iotx_dmsg_get_id();

	/* Get Dev ID */
	res = iotx_dmgr_search_device_by_pkdn(product_key,device_name,&request.devid);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	/* Send Message To Cloud */
	res = iotx_dmsg_request(&request);
	if (res == SUCCESS_RETURN) {
		iotx_dmc_msg_insert(request.msgid,request.devid,IOTX_DM_EVENT_TOPO_ADD_NOTIFY_REPLY,NULL);
		res = request.msgid;
	}

	DM_free(request.params);

	return res;
}

/* TODO */
int iotx_dcs_thing_property_post(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN], _IN_ char *payload, _IN_ int payload_len)
{
	int res = 0;
	iotx_dmsg_request_t request;

	if (product_key == NULL || device_name == NULL ||
		(strlen(product_key) >= PRODUCT_KEY_MAXLEN) ||
		(strlen(device_name) >= DEVICE_NAME_MAXLEN) ||
		payload == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	memset(&request,0,sizeof(iotx_dmsg_request_t));
	request.service_prefix = IOTX_DCS_SYS_PREFIX;
	request.service_name = IOTX_DCS_THING_EVENT_PROPERTY_POST;
	memcpy(request.product_key,product_key,strlen(product_key));
	memcpy(request.device_name,device_name,strlen(device_name));

	/* Get Params And Method */
	res = iotx_dmsg_thing_property_post(product_key,device_name,payload,&request);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	/* Get Msg ID */
	request.msgid = iotx_dmsg_get_id();

	/* Get Dev ID */
	res = iotx_dmgr_search_device_by_pkdn(product_key,device_name,&request.devid);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	/* Send Message To Cloud */
	res = iotx_dmsg_request(&request);
	if (res == SUCCESS_RETURN) {
		iotx_dmc_msg_insert(request.msgid,request.devid,IOTX_DM_EVENT_EVENT_PROPERTY_POST_REPLY,NULL);
		res = request.msgid;
	}

	DM_free(request.params);

	return res;
}

int iotx_dcs_thing_event_post(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN], _IN_ char *identifier, _IN_ int identifier_len, _IN_ char *method, _IN_ char *payload, _IN_ int payload_len)
{
	int res = 0, service_name_len = 0;
	char *service_name = NULL;
	iotx_dmsg_request_t request;

	if (product_key == NULL || device_name == NULL ||
		(strlen(product_key) >= PRODUCT_KEY_MAXLEN) ||
		(strlen(device_name) >= DEVICE_NAME_MAXLEN) ||
		identifier == NULL || identifier_len == 0 || payload == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	service_name_len = strlen(IOTX_DCS_THING_EVENT_POST) + identifier_len + 1;
	service_name = DM_malloc(service_name_len);
	if (service_name == NULL) {
		dm_log_err(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(service_name,0,service_name_len);
	HAL_Snprintf(service_name,service_name_len,IOTX_DCS_THING_EVENT_POST,identifier_len,identifier);

	memset(&request,0,sizeof(iotx_dmsg_request_t));
	request.service_prefix = IOTX_DCS_SYS_PREFIX;
	request.service_name = service_name;
	memcpy(request.product_key,product_key,strlen(product_key));
	memcpy(request.device_name,device_name,strlen(device_name));

	/* Get Params And Method */
	res = iotx_dmsg_thing_event_post(product_key,device_name,method,payload,&request);
	if (res != SUCCESS_RETURN) {DM_free(service_name);return FAIL_RETURN;}

	/* Get Msg ID */
	request.msgid = iotx_dmsg_get_id();

	/* Get Dev ID */
	res = iotx_dmgr_search_device_by_pkdn(product_key,device_name,&request.devid);
	if (res != SUCCESS_RETURN) {DM_free(service_name);return FAIL_RETURN;}

	/* Send Message To Cloud */
	res = iotx_dmsg_request(&request);
	if (res == SUCCESS_RETURN) {
		iotx_dmc_msg_insert(request.msgid,request.devid,IOTX_DM_EVENT_EVENT_PROPERTY_POST_REPLY,NULL);
		res = request.msgid;
	}

	DM_free(service_name);
	DM_free(request.params);

	return res;
}

int iotx_dcs_thing_deviceinfo_update(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN], _IN_ char *payload, _IN_ int payload_len)
{
	int res = 0;
	iotx_dmsg_request_t request;

	if (product_key == NULL || device_name == NULL ||
		(strlen(product_key) >= PRODUCT_KEY_MAXLEN) ||
		(strlen(device_name) >= DEVICE_NAME_MAXLEN) ||
		payload == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	memset(&request,0,sizeof(iotx_dmsg_request_t));
	request.service_prefix = IOTX_DCS_SYS_PREFIX;
	request.service_name = IOTX_DCS_THING_DEVICEINFO_UPDATE;
	memcpy(request.product_key,product_key,strlen(product_key));
	memcpy(request.device_name,device_name,strlen(device_name));

	/* Get Params And Method */
	res = iotx_dmsg_thing_deviceinfo_update(product_key,device_name,payload,&request);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	/* Get Msg ID */
	request.msgid = iotx_dmsg_get_id();

	/* Get Dev ID */
	res = iotx_dmgr_search_device_by_pkdn(product_key,device_name,&request.devid);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	/* Send Message To Cloud */
	res = iotx_dmsg_request(&request);
	if (res == SUCCESS_RETURN) {
		iotx_dmc_msg_insert(request.msgid,request.devid,IOTX_DM_EVENT_DEVICEINFO_UPDATE_REPLY,NULL);
		res = request.msgid;
	}

	DM_free(request.params);

	return res;
}

int iotx_dcs_thing_deviceinfo_delete(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN], _IN_ char *payload, _IN_ int payload_len)
{
	int res = 0;
	iotx_dmsg_request_t request;

	if (product_key == NULL || device_name == NULL ||
		(strlen(product_key) >= PRODUCT_KEY_MAXLEN) ||
		(strlen(device_name) >= DEVICE_NAME_MAXLEN) ||
		payload == NULL) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	memset(&request,0,sizeof(iotx_dmsg_request_t));
	request.service_prefix = IOTX_DCS_SYS_PREFIX;
	request.service_name = IOTX_DCS_THING_DEVICEINFO_DELETE;
	memcpy(request.product_key,product_key,strlen(product_key));
	memcpy(request.device_name,device_name,strlen(device_name));

	/* Get Params And Method */
	res = iotx_dmsg_thing_deviceinfo_delete(product_key,device_name,payload,&request);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	/* Get Msg ID */
	request.msgid = iotx_dmsg_get_id();

	/* Get Dev ID */
	res = iotx_dmgr_search_device_by_pkdn(product_key,device_name,&request.devid);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	/* Send Message To Cloud */
	res = iotx_dmsg_request(&request);
	if (res == SUCCESS_RETURN) {
		iotx_dmc_msg_insert(request.msgid,request.devid,IOTX_DM_EVENT_DEVICEINFO_DELETE_REPLY,NULL);
		res = request.msgid;
	}

	DM_free(request.params);

	return res;
}

int iotx_dcs_thing_dsltemplate_get(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN])
{
	int res = 0;
	iotx_dmsg_request_t request;

	if (product_key == NULL || device_name == NULL ||
		(strlen(product_key) >= PRODUCT_KEY_MAXLEN) ||
		(strlen(device_name) >= DEVICE_NAME_MAXLEN)) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	memset(&request,0,sizeof(iotx_dmsg_request_t));
	request.service_prefix = IOTX_DCS_SYS_PREFIX;
	request.service_name = IOTX_DCS_THING_DSLTEMPLATE_GET;
	memcpy(request.product_key,product_key,strlen(product_key));
	memcpy(request.device_name,device_name,strlen(device_name));

	/* Get Params And Method */
	res = iotx_dmsg_thing_dsltemplate_get(&request);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	/* Get Msg ID */
	request.msgid = iotx_dmsg_get_id();

	/* Get Dev ID */
	res = iotx_dmgr_search_device_by_pkdn(product_key,device_name,&request.devid);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	/* Send Message To Cloud */
	res = iotx_dmsg_request(&request);
	if (res == SUCCESS_RETURN) {
		iotx_dmc_msg_insert(request.msgid,request.devid,IOTX_DM_EVENT_DSLTEMPLATE_GET_REPLY,NULL);
		res = request.msgid;
	}

	DM_free(request.params);

	return res;
}

int iotx_dcs_thing_dynamictsl_get(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN])
{
	int res = 0;
	iotx_dmsg_request_t request;

	if (product_key == NULL || device_name == NULL ||
		(strlen(product_key) >= PRODUCT_KEY_MAXLEN) ||
		(strlen(device_name) >= DEVICE_NAME_MAXLEN)) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	memset(&request,0,sizeof(iotx_dmsg_request_t));
	request.service_prefix = IOTX_DCS_SYS_PREFIX;
	request.service_name = IOTX_DCS_THING_DYNAMICTSL_GET;
	memcpy(request.product_key,product_key,strlen(product_key));
	memcpy(request.device_name,device_name,strlen(device_name));

	/* Get Params And Method */
	res = iotx_dmsg_thing_dynamictsl_get(&request);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	/* Get Msg ID */
	request.msgid = iotx_dmsg_get_id();

	/* Get Dev ID */
	res = iotx_dmgr_search_device_by_pkdn(product_key,device_name,&request.devid);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	/* Send Message To Cloud */
	res = iotx_dmsg_request(&request);
	if (res == SUCCESS_RETURN) {
		iotx_dmc_msg_insert(request.msgid,request.devid,IOTX_DM_EVENT_DSLTEMPLATE_GET_REPLY,NULL);
		res = request.msgid;
	}

	DM_free(request.params);

	return res;
}

int iotx_dcs_combine_login(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN], _IN_ char device_secret[DEVICE_SECRET_MAXLEN])
{
	int res = 0;
	iotx_dmsg_request_t request;

	if (product_key == NULL || device_name == NULL || device_secret == NULL ||
		(strlen(product_key) >= PRODUCT_KEY_MAXLEN) ||
		(strlen(device_name) >= DEVICE_NAME_MAXLEN) ||
		(strlen(device_secret) >= DEVICE_SECRET_MAXLEN)) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	memset(&request,0,sizeof(iotx_dmsg_request_t));
	request.service_prefix = IOTX_DCS_EXT_SESSION_PREFIX;
	request.service_name = IOTX_DCS_COMBINE_LOGIN;
	HAL_GetProductKey(request.product_key);
	HAL_GetDeviceName(request.device_name);

	/* Get Params And Method */
	res = iotx_dmsg_combine_login(product_key,device_name,device_secret,&request);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	/* Get Msg ID */
	request.msgid = iotx_dmsg_get_id();

	/* Get Dev ID */
	res = iotx_dmgr_search_device_by_pkdn(product_key,device_name,&request.devid);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	/* Send Message To Cloud */
	res = iotx_dmsg_request(&request);
	if (res == SUCCESS_RETURN) {
		iotx_dmc_msg_insert(request.msgid,request.devid,IOTX_DM_EVENT_COMBINE_LOGIN_REPLY,NULL);
		res = request.msgid;
	}

	DM_free(request.params);

	return res;
}

int iotx_dcs_combine_logout(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN])
{
	int res = 0;
	iotx_dmsg_request_t request;

	if (product_key == NULL || device_name == NULL ||
		(strlen(product_key) >= PRODUCT_KEY_MAXLEN) ||
		(strlen(device_name) >= DEVICE_NAME_MAXLEN)) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	memset(&request,0,sizeof(iotx_dmsg_request_t));
	request.service_prefix = IOTX_DCS_EXT_SESSION_PREFIX;
	request.service_name = IOTX_DCS_COMBINE_LOGOUT;
	HAL_GetProductKey(request.product_key);
	HAL_GetDeviceName(request.device_name);

	/* Get Params And Method */
	res = iotx_dmsg_combine_logout(product_key,device_name,&request);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	/* Get Msg ID */
	request.msgid = iotx_dmsg_get_id();

	/* Get Dev ID */
	res = iotx_dmgr_search_device_by_pkdn(product_key,device_name,&request.devid);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	/* Send Message To Cloud */
	res = iotx_dmsg_request(&request);
	if (res == SUCCESS_RETURN) {
		iotx_dmc_msg_insert(request.msgid,request.devid,IOTX_DM_EVENT_COMBINE_LOGOUT_REPLY,NULL);
		res = request.msgid;
	}

	DM_free(request.params);

	return res;
}

int iotx_dcs_thing_model_up_raw(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN], _IN_ char *payload, _IN_ int payload_len)
{
	int res = 0;
	char *cloud_payload = NULL, *uri = NULL;
	iotx_dmsg_request_t request;

	if (product_key == NULL || device_name == NULL ||
		(strlen(product_key) >= PRODUCT_KEY_MAXLEN) ||
		(strlen(device_name) >= DEVICE_NAME_MAXLEN) ||
		payload == NULL || payload_len <= 0) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	memset(&request,0,sizeof(iotx_dmsg_request_t));
	request.service_prefix = IOTX_DCS_SYS_PREFIX;
	request.service_name = IOTX_DCS_THING_MODEL_UP_RAW;
	memcpy(request.product_key,product_key,strlen(product_key));
	memcpy(request.device_name,device_name,strlen(device_name));

	/* Request URI */
	res = iotx_dcm_service_name(request.service_prefix,request.service_name,
								request.product_key,request.device_name,&uri);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	cloud_payload = DM_malloc(payload_len + 1);
	if (cloud_payload == NULL) {
		DM_free(uri);
		dm_log_err(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(cloud_payload,0,payload_len + 1);
	memcpy(cloud_payload,payload,payload_len);

	res = iotx_dcw_send_to_cloud(uri,cloud_payload,NULL);
	if (res != SUCCESS_RETURN) {
		DM_free(uri);DM_free(cloud_payload);
		dm_log_err(IOTX_DM_LOG_CM_SEND_MESSAGE_FAILED);
		return FAIL_RETURN;
	}

	DM_free(uri);DM_free(cloud_payload);
	return SUCCESS_RETURN;
}

int iotx_dcs_thing_service_response(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN], _IN_ int msgid, _IN_ iotx_dm_error_code_t code, _IN_ char *identifier, _IN_ int identifier_len, _IN_ char *payload, _IN_ int payload_len)
{
	int res = 0, service_name_len = 0;
	char *msgid_str = NULL, *service_name = NULL;
	iotx_dmsg_request_payload_t request;
	iotx_dmsg_response_t response;

	memset(&request,0,sizeof(iotx_dmsg_request_payload_t));
	memset(&response,0,sizeof(iotx_dmsg_response_t));

	if (product_key == NULL || device_name == NULL ||
		(strlen(product_key) >= PRODUCT_KEY_MAXLEN) ||
		(strlen(device_name) >= DEVICE_NAME_MAXLEN) ||
		msgid < 0 || identifier == NULL || identifier_len <= 0 ||
		payload == NULL || payload_len <= 0) {
		dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
	}

	/* Response Msg ID */
	res = iotx_dcm_itoa(msgid,&msgid_str);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}
	request.id.value = msgid_str;
	request.id.value_length = strlen(msgid_str);

	/* Service Name */
	service_name_len = strlen(IOTX_DCS_THING_SERVICE_RESPONSE) + identifier_len + 1;
	service_name = DM_malloc(service_name_len);
	if (service_name == NULL) {
		DM_free(msgid_str);
		dm_log_err(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
		return FAIL_RETURN;
	}
	memset(service_name,0,service_name_len);
	HAL_Snprintf(service_name,service_name_len,IOTX_DCS_THING_SERVICE_RESPONSE,identifier_len,identifier);

	/* Response */
	response.service_prefix = IOTX_DCS_SYS_PREFIX;
	response.service_name = service_name;
	memcpy(response.product_key,product_key,strlen(product_key));
	memcpy(response.device_name,device_name,strlen(device_name));
	response.code = code;

	dm_log_debug("Current Service Name: %s",service_name);
	iotx_dmsg_response_with_data(&request,&response,payload,payload_len);

	DM_free(msgid_str);DM_free(service_name);
	return SUCCESS_RETURN;
}

