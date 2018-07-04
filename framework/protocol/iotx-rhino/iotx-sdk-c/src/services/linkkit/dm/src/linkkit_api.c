#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "iot_import.h"
#include "iot_export.h"
#include "lite-utils.h"
#include "lite-cjson.h"
#include "linkkit_export.h"
#include "iotx_dm_common.h"
#include "lite-log.h"
#include "linked_list.h"

#define LINKKIT_API_KEY_ID         "id"
#define LINKKIT_API_KEY_CODE       "code"
#define LINKKIT_API_KEY_DEVID      "devid"
#define LINKKIT_API_KEY_SERVICEID  "serviceid"
#define LINKKIT_API_KEY_PROPERTYID "propertyid"
#define LINKKIT_API_KEY_EVENTID    "eventid"
#define LINKKIT_API_KEY_PAYLOAD    "payload"

#define LINKKIT_MAX_POST_CB_NUMBER 20
typedef struct _post_cb
{
    int                    id_expected;
    void*                  cb;
    unsigned long long     time;
} post_cb_t;

void* g_list_post_cb = NULL;
static linkkit_ops_t* g_linkkit_ops = NULL;
static void* g_user_ctx = NULL;
#ifdef CM_SUPPORT_MULTI_THREAD
void* thread_process_dispatch;
int   thread_process_disatch_exit = 0;
#endif /* CM_SUPPORT_MULTI_THREAD */

static void _linkkit_event_callback(iotx_dm_event_types_t type, char *payload);


#ifdef CM_SUPPORT_MULTI_THREAD
void* linkkit_dispatch(void* params)
{
    while (1) {
        if (thread_process_disatch_exit) return NULL;
        IOT_DM_Dispatch();
        HAL_SleepMs(10);
    }
    return NULL;
}
#else
void* linkkit_dispatch()
{
    IOT_DM_Dispatch();
    return NULL;
}
#endif

static void _find_oldest_post_cb(void* _post_cb, va_list* params)
{
    post_cb_t* post_cb = _post_cb;
    post_cb_t** post_cb_oldest;
    unsigned long long current_time_ms;
    unsigned long long time_duration_ms;
    unsigned long long time_duration_ms_oldest;

    post_cb_oldest = va_arg(*params, post_cb_t**);
    current_time_ms = va_arg(*params, int);

    if (current_time_ms == post_cb->time) {
        time_duration_ms = 0;
    } else {
        time_duration_ms = current_time_ms > post_cb->time ? current_time_ms - post_cb->time : current_time_ms + __LONG_LONG_MAX__ * 2ULL + 1ULL - post_cb->time;
    }

    if (*post_cb_oldest == NULL) {
        *post_cb_oldest = post_cb;
    }

    time_duration_ms_oldest = current_time_ms > (*post_cb_oldest)->time ? current_time_ms - (*post_cb_oldest)->time : current_time_ms + __LONG_LONG_MAX__ * 2ULL + 1ULL - (*post_cb_oldest)->time;

    if (time_duration_ms > time_duration_ms_oldest) {
        *post_cb_oldest = post_cb;
    }
}


static void _insert_post_cb(int id_send, linked_list_t* list, void* cb)
{
    int list_size;
    post_cb_t* post_cb;
    post_cb_t* post_cb_oldest = NULL;
    unsigned long long current_time_ms;

    post_cb = calloc(1, sizeof(post_cb_t));
    if (!post_cb) return;

    current_time_ms = HAL_UptimeMs();
    post_cb->id_expected = id_send;
    post_cb->cb = cb;
    post_cb->time = current_time_ms;

    linked_list_insert(list, post_cb);

    list_size = linked_list_get_size(list);
    if (list_size > LINKKIT_MAX_POST_CB_NUMBER) {
        /* find oldest cb and remove */
        linked_list_iterator(list, _find_oldest_post_cb, &post_cb_oldest, current_time_ms);
        if (post_cb_oldest) linked_list_remove(list, post_cb_oldest);
    }
}

static void _find_post_cb_by_id(void* _post_cb, va_list* params)
{
    post_cb_t* post_cb = _post_cb;
    post_cb_t** post_cb_ret;
    int id_expected;

    id_expected = va_arg(*params, int);
    post_cb_ret = va_arg(*params, post_cb_t**);

    if (post_cb->id_expected == id_expected) {
        *post_cb_ret = post_cb;
    }
}

static void _linkkit_event_callback(iotx_dm_event_types_t type, char *payload)
{
    linkkit_ops_t* linkkit_ops = g_linkkit_ops;
    void *context = g_user_ctx;
    dm_log_info("Receive Message Type: %d",type);
    if (payload) {dm_log_info("Receive Message: %s",payload);}

    switch(type) {
        case IOTX_DM_EVENT_CLOUD_CONNECTED: {
            linkkit_ops->on_connect(context);
            break;
        }
        case IOTX_DM_EVENT_CLOUD_DISCONNECT: {
            linkkit_ops->on_disconnect(context);
            break;
        }
		case IOTX_DM_EVENT_MODEL_DOWN_RAW: {
			int res = 0;
			void *thing_id = NULL;
			lite_cjson_t lite, lite_item_devid, lite_item_rawdata;

			/* Parse Payload */
			memset(&lite,0,sizeof(lite_cjson_t));
			res = lite_cjson_parse(payload,strlen(payload),&lite);
			if (res != SUCCESS_RETURN) {return;}

			/* Parse Devid */
			memset(&lite_item_devid,0,sizeof(lite_cjson_t));
			res = lite_cjson_object_item(&lite,LINKKIT_API_KEY_DEVID,strlen(LINKKIT_API_KEY_DEVID),&lite_item_devid);
			if (res != SUCCESS_RETURN) {return;}
			dm_log_debug("Current Devid: %d",lite_item_devid.value_int);

			/* Parse Raw Data */
			memset(&lite_item_rawdata,0,sizeof(lite_cjson_t));
			res = lite_cjson_object_item(&lite,LINKKIT_API_KEY_PAYLOAD,strlen(LINKKIT_API_KEY_PAYLOAD),&lite_item_rawdata);
			if (res != SUCCESS_RETURN) {return;}
			dm_log_debug("Current Raw Data: %.*s",lite_item_rawdata.value_length,lite_item_rawdata.value);

			res = IOT_DM_Legacy_Get_ThingId_By_Devid(lite_item_devid.value_int,&thing_id);
			if (res != SUCCESS_RETURN) {return;}

			linkkit_ops->raw_data_arrived(thing_id,lite_item_rawdata.value,lite_item_rawdata.value_length,context);
		}
		break;
	case IOTX_DM_EVENT_THING_SERVICE_REQUEST: {
			int res = 0;
			void *thing_id = NULL;
			char *service = NULL;
			lite_cjson_t lite, lite_item_id, lite_item_devid, lite_item_serviceid;

			/* Parse Payload */
			memset(&lite,0,sizeof(lite_cjson_t));
			res = lite_cjson_parse(payload,strlen(payload),&lite);
			if (res != SUCCESS_RETURN) {return;}

			/* Parse Id */
			memset(&lite_item_id,0,sizeof(lite_cjson_t));
			res = lite_cjson_object_item(&lite,LINKKIT_API_KEY_ID,strlen(LINKKIT_API_KEY_ID),&lite_item_id);
			if (res != SUCCESS_RETURN) {return;}
			dm_log_debug("Current Id: %d",lite_item_id.value_int);

			/* Parse Devid */
			memset(&lite_item_devid,0,sizeof(lite_cjson_t));
			res = lite_cjson_object_item(&lite,LINKKIT_API_KEY_DEVID,strlen(LINKKIT_API_KEY_DEVID),&lite_item_devid);
			if (res != SUCCESS_RETURN) {return;}
			dm_log_debug("Current Devid: %d",lite_item_devid.value_int);

			/* Parse Serviceid */
			memset(&lite_item_serviceid,0,sizeof(lite_cjson_t));
			res = lite_cjson_object_item(&lite,LINKKIT_API_KEY_SERVICEID,strlen(LINKKIT_API_KEY_SERVICEID),&lite_item_serviceid);
			if (res != SUCCESS_RETURN) {return;}
			dm_log_debug("Current ServiceID: %.*s",lite_item_serviceid.value_length,lite_item_serviceid.value);

			res = IOT_DM_Legacy_Get_ThingId_By_Devid(lite_item_devid.value_int,&thing_id);
			if (res != SUCCESS_RETURN) {return;}

			service = DM_malloc(lite_item_serviceid.value_length + 1);
			if (service == NULL) {
				dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
				return;
			}
			memset(service,0,lite_item_serviceid.value_length + 1);
			memcpy(service,lite_item_serviceid.value,lite_item_serviceid.value_length);

			linkkit_ops->thing_call_service(thing_id,(const char *)service,lite_item_id.value_int,context);
		}
		break;
		case IOTX_DM_EVENT_LEGACY_THING_CREATED: {
			int res = 0;
			void *thing_id = NULL;
			lite_cjson_t lite,lite_item_devid;

			/* Parse Payload */
			memset(&lite,0,sizeof(lite_cjson_t));
			res = lite_cjson_parse(payload,strlen(payload),&lite);
			if (res != SUCCESS_RETURN) {return;}

			/* Parse Id */
			memset(&lite_item_devid,0,sizeof(lite_cjson_t));
			res = lite_cjson_object_item(&lite,LINKKIT_API_KEY_DEVID,strlen(LINKKIT_API_KEY_DEVID),&lite_item_devid);
			if (res != SUCCESS_RETURN) {return;}
			dm_log_debug("Current Devid: %d",lite_item_devid.value_int);

			res = IOT_DM_Legacy_Get_ThingId_By_Devid(lite_item_devid.value_int,&thing_id);
			if (res != SUCCESS_RETURN) {return;}

			linkkit_ops->thing_create(thing_id,context);
		}
		break;
		case IOTX_DM_EVENT_THING_DISABLE: {
			int res = 0;
			void *thing_id = NULL;
			lite_cjson_t lite,lite_item_devid;

			/* Parse Payload */
			memset(&lite,0,sizeof(lite_cjson_t));
			res = lite_cjson_parse(payload,strlen(payload),&lite);
			if (res != SUCCESS_RETURN) {return;}

			/* Parse Id */
			memset(&lite_item_devid,0,sizeof(lite_cjson_t));
			res = lite_cjson_object_item(&lite,LINKKIT_API_KEY_DEVID,strlen(LINKKIT_API_KEY_DEVID),&lite_item_devid);
			if (res != SUCCESS_RETURN) {return;}
			dm_log_debug("Current Devid: %d",lite_item_devid.value_int);

			res = IOT_DM_Legacy_Get_ThingId_By_Devid(lite_item_devid.value_int,&thing_id);
			if (res != SUCCESS_RETURN) {return;}

			linkkit_ops->thing_disable(thing_id,context);
		}
		break;
		case IOTX_DM_EVENT_THING_ENABLE: {
			int res = 0;
			void *thing_id = NULL;
			lite_cjson_t lite,lite_item_devid;

			/* Parse Payload */
			memset(&lite,0,sizeof(lite_cjson_t));
			res = lite_cjson_parse(payload,strlen(payload),&lite);
			if (res != SUCCESS_RETURN) {return;}

			/* Parse Id */
			memset(&lite_item_devid,0,sizeof(lite_cjson_t));
			res = lite_cjson_object_item(&lite,LINKKIT_API_KEY_DEVID,strlen(LINKKIT_API_KEY_DEVID),&lite_item_devid);
			if (res != SUCCESS_RETURN) {return;}
			dm_log_debug("Current Devid: %d",lite_item_devid.value_int);

			res = IOT_DM_Legacy_Get_ThingId_By_Devid(lite_item_devid.value_int,&thing_id);
			if (res != SUCCESS_RETURN) {return;}

			linkkit_ops->thing_enable(thing_id,context);
		}
		break;
		case IOTX_DM_EVENT_PROPERTY_SET: {
			int res = 0;
			void *thing_id = NULL;
			char *propertyid = NULL;
			lite_cjson_t lite,lite_item_devid,lite_item_propertyid;

			/* Parse Payload */
			memset(&lite,0,sizeof(lite_cjson_t));
			res = lite_cjson_parse(payload,strlen(payload),&lite);
			if (res != SUCCESS_RETURN) {return;}

			/* Parse Devid */
			memset(&lite_item_devid,0,sizeof(lite_cjson_t));
			res = lite_cjson_object_item(&lite,LINKKIT_API_KEY_DEVID,strlen(LINKKIT_API_KEY_DEVID),&lite_item_devid);
			if (res != SUCCESS_RETURN) {return;}
			dm_log_debug("Current Devid: %d",lite_item_devid.value_int);

			/* Parse Property ID */
			memset(&lite_item_propertyid,0,sizeof(lite_cjson_t));
			res = lite_cjson_object_item(&lite,LINKKIT_API_KEY_PROPERTYID,strlen(LINKKIT_API_KEY_PROPERTYID),&lite_item_propertyid);
			if (res != SUCCESS_RETURN) {return;}
			dm_log_debug("Current PropertyID: %.*s",lite_item_propertyid.value_length,lite_item_propertyid.value);

			res = IOT_DM_Legacy_Get_ThingId_By_Devid(lite_item_devid.value_int,&thing_id);
			if (res != SUCCESS_RETURN) {return;}

			propertyid = DM_malloc(lite_item_propertyid.value_length + 1);
			if (propertyid == NULL) {
				dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
				return;
			}
			memset(propertyid,0,lite_item_propertyid.value_length + 1);
			memcpy(propertyid,lite_item_propertyid.value,lite_item_propertyid.value_length);

			linkkit_ops->thing_prop_changed(thing_id,propertyid,context);
			DM_free(propertyid);
		}
		break;
		case IOTX_DM_EVENT_EVENT_PROPERTY_POST_REPLY: {
			int res = 0;
			void *thing_id = NULL;
			linked_list_t* list = g_list_post_cb;
			post_cb_t* post_cb = NULL;
    		handle_post_cb_fp_t handle_post_cb_fp;
			lite_cjson_t lite, lite_item_id, lite_item_code, lite_item_devid;

			/* Parse Payload */
			memset(&lite,0,sizeof(lite_cjson_t));
			res = lite_cjson_parse(payload,strlen(payload),&lite);
			if (res != SUCCESS_RETURN) {return;}

			/* Parse Id */
			memset(&lite_item_id,0,sizeof(lite_cjson_t));
			res = lite_cjson_object_item(&lite,LINKKIT_API_KEY_ID,strlen(LINKKIT_API_KEY_ID),&lite_item_id);
			if (res != SUCCESS_RETURN) {return;}
			dm_log_debug("Current Id: %d",lite_item_id.value_int);

			/* Parse Code */
			memset(&lite_item_code,0,sizeof(lite_cjson_t));
			res = lite_cjson_object_item(&lite,LINKKIT_API_KEY_CODE,strlen(LINKKIT_API_KEY_CODE),&lite_item_code);
			if (res != SUCCESS_RETURN) {return;}
			dm_log_debug("Current Code: %d",lite_item_code.value_int);

			/* Parse Devid */
			memset(&lite_item_devid,0,sizeof(lite_cjson_t));
			res = lite_cjson_object_item(&lite,LINKKIT_API_KEY_DEVID,strlen(LINKKIT_API_KEY_DEVID),&lite_item_devid);
			if (res != SUCCESS_RETURN) {return;}
			dm_log_debug("Current Devid: %d",lite_item_devid.value_int);

			res = IOT_DM_Legacy_Get_ThingId_By_Devid(lite_item_devid.value_int,&thing_id);
			if (res != SUCCESS_RETURN) {return;}

	        if (list) {
	            linked_list_iterator(list, _find_post_cb_by_id, lite_item_id.value_int, &post_cb);
	            if (post_cb && post_cb->cb) {
	                handle_post_cb_fp = post_cb->cb;
	                handle_post_cb_fp(thing_id, lite_item_id.value_int, lite_item_code.value_int, NULL, context);

	                linked_list_remove(list, post_cb);
	            }
	        }
		}
		break;
		case IOTX_DM_EVENT_EVENT_SPECIFIC_POST_REPLY: {
			int res = 0;
			void *thing_id = NULL;
			char *eventid = NULL;
			linked_list_t* list = g_list_post_cb;
			post_cb_t* post_cb = NULL;
    		handle_post_cb_fp_t handle_post_cb_fp;
			lite_cjson_t lite, lite_item_id, lite_item_code, lite_item_devid, lite_item_eventid;

			/* Parse Payload */
			memset(&lite,0,sizeof(lite_cjson_t));
			res = lite_cjson_parse(payload,strlen(payload),&lite);
			if (res != SUCCESS_RETURN) {return;}

			/* Parse Id */
			memset(&lite_item_id,0,sizeof(lite_cjson_t));
			res = lite_cjson_object_item(&lite,LINKKIT_API_KEY_ID,strlen(LINKKIT_API_KEY_ID),&lite_item_id);
			if (res != SUCCESS_RETURN) {return;}
			dm_log_debug("Current Id: %d",lite_item_id.value_int);

			/* Parse Code */
			memset(&lite_item_code,0,sizeof(lite_cjson_t));
			res = lite_cjson_object_item(&lite,LINKKIT_API_KEY_CODE,strlen(LINKKIT_API_KEY_CODE),&lite_item_code);
			if (res != SUCCESS_RETURN) {return;}
			dm_log_debug("Current Code: %d",lite_item_code.value_int);

			/* Parse Devid */
			memset(&lite_item_devid,0,sizeof(lite_cjson_t));
			res = lite_cjson_object_item(&lite,LINKKIT_API_KEY_DEVID,strlen(LINKKIT_API_KEY_DEVID),&lite_item_devid);
			if (res != SUCCESS_RETURN) {return;}
			dm_log_debug("Current Devid: %d",lite_item_devid.value_int);

			/* Parse Property ID */
			memset(&lite_item_eventid,0,sizeof(lite_cjson_t));
			res = lite_cjson_object_item(&lite,LINKKIT_API_KEY_EVENTID,strlen(LINKKIT_API_KEY_EVENTID),&lite_item_eventid);
			if (res != SUCCESS_RETURN) {return;}
			dm_log_debug("Current EventID: %.*s",lite_item_eventid.value_length,lite_item_eventid.value);

			res = IOT_DM_Legacy_Get_ThingId_By_Devid(lite_item_devid.value_int,&thing_id);
			if (res != SUCCESS_RETURN) {return;}

			eventid = DM_malloc(lite_item_eventid.value_length + 1);
			if (eventid == NULL) {
				dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
				return;
			}
			memset(eventid,0,lite_item_eventid.value_length + 1);
			memcpy(eventid,lite_item_eventid.value,lite_item_eventid.value_length);

	        if (list) {
	            linked_list_iterator(list, _find_post_cb_by_id, lite_item_id.value_int, &post_cb);
	            if (post_cb && post_cb->cb) {
	                handle_post_cb_fp = post_cb->cb;
	                handle_post_cb_fp(thing_id, lite_item_id.value_int, lite_item_code.value_int, eventid, context);

	                linked_list_remove(list, post_cb);
	            }
	        }
			DM_free(eventid);
		}
		break;
    #ifdef LOCAL_CONN_ENABLE
        case IOTX_DM_EVENT_LOCAL_CONNECTED: {
            linkkit_ops->on_connect(context, 0);
            break;
        }
        case IOTX_DM_EVENT_LOCAL_DISCONNECT: {
            linkkit_ops->on_disconnect(context, 0);
            break;
        }
    #endif
        default:
           break;
    }
}


/**
 * @brief start linkkit routines, and install callback funstions(async type for cloud connecting).
 *
 * @param max_buffered_msg, specify max buffered message size.
 * @param ops, callback function struct to be installed.
 * @param get_tsl_from_cloud, config if device need to get tsl from cloud(!0) or local(0), if local selected, must invoke linkkit_set_tsl to tell tsl to dm after start complete.
 * @param log_level, config log level.
 * @param user_context, user context pointer.
 * @param domain_type, specify the could server domain.
 *
 * @return int, 0 when success, -1 when fail.
 */
int linkkit_start(int max_buffered_msg, int get_tsl_from_cloud, linkkit_loglevel_t log_level, linkkit_ops_t *ops, linkkit_cloud_domain_type_t domain_type, void *user_context)
{
    int ret = -1;
    iotx_dm_init_params_t dm_init_params;
#ifdef CM_SUPPORT_MULTI_THREAD
    int stack_used;
#endif /* CM_SUPPORT_MULTI_THREAD */

    if (!ops || !user_context || max_buffered_msg <= 0) return ret;
    g_linkkit_ops = ops;
    g_user_ctx = user_context;
    memset(&dm_init_params,0,sizeof(iotx_dm_init_params_t));
    dm_init_params.secret_type = IOTX_DM_DEVICE_SECRET_DEVICE;
    dm_init_params.domain_type = domain_type;
    dm_init_params.event_callback = _linkkit_event_callback;
#ifdef CM_SUPPORT_MULTI_THREAD
    HAL_ThreadCreate(&thread_process_dispatch, linkkit_dispatch, NULL, NULL, &stack_used);
    HAL_ThreadDetach(thread_process_dispatch);
#endif /* CM_SUPPORT_MULTI_THREAD */

    ret = IOT_DM_Construct(&dm_init_params);
	if (ret != SUCCESS_RETURN) {return FAIL_RETURN;}

	if (get_tsl_from_cloud != 0) {
		ret = IOT_DM_Set_TSL(IOTX_DMGR_LOCAL_NODE_DEVID,IOTX_DM_TSL_SOURCE_CLOUD,NULL,0);
		if (ret < SUCCESS_RETURN) {return FAIL_RETURN;}
	}
    g_list_post_cb = linked_list_create("post cb", 0);
    if (!g_list_post_cb) return -1;

	return SUCCESS_RETURN;
}

#ifdef SERVICE_OTA_ENABLED
#endif /* SERVICE_OTA_ENABLED */


static void _free_post_cb(void* _post_cb, va_list* params)
{
    void* post_cb = _post_cb;

    if (post_cb) free(post_cb);
}

int linkkit_end()
{
    linked_list_t* list;

#ifdef CM_SUPPORT_MULTI_THREAD
    thread_process_disatch_exit = 1;
#endif
    list = g_list_post_cb;
    if (list) {
        linked_list_iterator(list, _free_post_cb);
        linked_list_destroy(list);
    }
    return IOT_DM_Destroy();
}

/**
 * @brief install user tsl.
 *
 * @param tsl, tsl string that contains json description for thing object.
 * @param tsl_len, tsl string length.
 *
 * @return pointer to thing object, NULL when fails.
 */
void* linkkit_set_tsl(const char* tsl, int tsl_len)
{
    void* thing = NULL;
    int res = 0;
    res = IOT_DM_Set_TSL(IOTX_DMGR_LOCAL_NODE_DEVID, IOTX_DM_TSL_SOURCE_LOCAL, tsl, tsl_len);
    if (res != SUCCESS_RETURN) {
        return NULL;
    }
    //ToDo: get ptr of device id
    IOT_DM_Legacy_Get_ThingId_By_Devid(IOTX_DMGR_LOCAL_NODE_DEVID, &thing);
    if (res != SUCCESS_RETURN) {
        dm_log_err("get thing id fail");
    }
    return thing;
}


int linkkit_set_value(linkkit_method_set_t method_set, const void* thing_id, const char* identifier,
                             const void* value, const char* value_str)
{
    int res = FAIL_RETURN;
    int devid;

    res = IOT_DM_Legacy_Get_Devid_By_ThingId((void *)thing_id, &devid);
    if (res == SUCCESS_RETURN) {
        switch (method_set) {
            case linkkit_method_set_property_value: {
                res = IOT_DM_Legacy_Set_Property_Value(devid, (char *)identifier, strlen(identifier), (void *)value);
                dm_log_info(IOTX_DM_LOG_OPERATOR_RES, 0, res);
                break;
            }
            case linkkit_method_set_event_output_value: {
                res = IOT_DM_Legacy_Set_Event_Output_Value(devid, (char *)identifier, strlen(identifier), (void *)value);
                dm_log_info(IOTX_DM_LOG_OPERATOR_RES, 0, res);
                break;
            }
            case linkkit_method_set_service_output_value: {
                res = IOT_DM_Legacy_Set_Service_Output_Value(devid, (char *)identifier, strlen(identifier), (void *)value);
                dm_log_info(IOTX_DM_LOG_OPERATOR_RES, 0, res);
                break;
            }
            default: {
                dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
                break;
            }
        }
    } else {
            dm_log_err(IOTX_DM_LOG_GET_INVALID_DEV_ID);
    }
    return res;
}

int linkkit_get_value(linkkit_method_get_t method_get, const void* thing_id, const char* identifier,
                             void* value, char** value_str)
{
    int res = FAIL_RETURN;
    int devid;

    res = IOT_DM_Legacy_Get_Devid_By_ThingId((void *)thing_id, &devid);
    if (res == SUCCESS_RETURN) {
        switch (method_get) {
            case linkkit_method_get_property_value: {
                res = IOT_DM_Get_Property_Value(devid, (char *)identifier, strlen(identifier), value);
                dm_log_info(IOTX_DM_LOG_OPERATOR_RES, 1, res);
                break;
            }
            case linkkit_method_get_event_output_value: {
                res = IOT_DM_Get_Event_Output_Value(devid, (char *)identifier, strlen(identifier), value);
                dm_log_info(IOTX_DM_LOG_OPERATOR_RES, 1, res);
                break;
            }
            case linkkit_method_get_service_input_value: {
                res = IOT_DM_Get_Service_Input_Value(devid, (char *)identifier, strlen(identifier), value);
                dm_log_info(IOTX_DM_LOG_OPERATOR_RES, 1, res);
                break;
            }
            case linkkit_method_get_service_output_value: {
                res = IOT_DM_Get_Service_Output_Value(devid, (char *)identifier, strlen(identifier), value);
                dm_log_info(IOTX_DM_LOG_OPERATOR_RES, 1, res);
                break;
            }
            default: {
                dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
                break;
            }
        }
    } else {
            dm_log_err(IOTX_DM_LOG_GET_INVALID_DEV_ID);
    }
    return res;
}



int linkkit_answer_service(const void* thing_id, const char* service_identifier, int response_id, int code)
{
	int res = 0, devid = 0;

	res = IOT_DM_Legacy_Get_Devid_By_ThingId((void *)thing_id,&devid);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

    //ToDo: need to analyze
    res = IOT_DM_Send_Service_Response(devid,response_id,(iotx_dm_error_code_t)code,(char *)service_identifier,strlen(service_identifier));

    return res;
}
/**
 * @brief answer a down raw service when a raw service requested by cloud, or invoke a up raw service to cloud.
 *
 * @param thing_id, pointer to thing object.
 * @param is_up_raw, specify up raw(not 0) or down raw reply(0).
 * @param raw_data, raw data that sent to cloud.
 * @param raw_data_length, raw data length that sent to cloud.
 *
 * @return 0 when success, -1 when fail.
 */
int linkkit_invoke_raw_service(const void* thing_id, int is_up_raw, void* raw_data, int raw_data_length)
{
    int res = FAIL_RETURN;
    int devid;

    res = IOT_DM_Legacy_Get_Devid_By_ThingId((void *)thing_id, &devid);
    if (res == SUCCESS_RETURN && raw_data != NULL) {
        res = IOT_DM_Post_Rawdata(devid, raw_data, raw_data_length);
    }
    return res;
}

#ifdef EXTENDED_INFO_ENABLED
/**
 * @brief trigger extended info update procedure.
 *
 * @param thing_id, pointer to thing object.
 * @param params, json type string that user to send to cloud.
 * @param linkkit_extended_info_operation, specify update type or delete type.
 *
 * @return 0 when success, -1 when fail.
 */

int linkkit_trigger_extended_info_operate(const void* thing_id, const char* params, linkkit_extended_info_operate_t linkkit_extended_info_operation)
{
    int res = FAIL_RETURN;
    int devid;

    res = IOT_DM_Legacy_Get_Devid_By_ThingId((void *)thing_id, &devid);
    if (res == SUCCESS_RETURN && params != NULL) {
        int len  = strlen(params);
        if (linkkit_extended_info_operation == linkkit_extended_info_operate_update) {
            res = IOT_DM_DeviceInfo_Update(devid, (char *)params, len);
            dm_log_info(IOTX_DM_LOG_OPERATOR_RES, 2, res);
        } else if (linkkit_extended_info_operation == linkkit_extended_info_operate_delete) {
            res = IOT_DM_DeviceInfo_Delete(devid, (char *)params, len);
            dm_log_info(IOTX_DM_LOG_OPERATOR_RES, 2, res);
        } else {
            dm_log_err(IOTX_DM_LOG_INVALID_PARAMETER);
        }
    } else {
        dm_log_err(IOTX_DM_LOG_GET_INVALID_DEV_ID);
    }
    return res;
}
#endif /* EXTENDED_INFO_ENABLED */

/**
 * @brief trigger a event to post to cloud.
 *
 * @param thing_id, pointer to thing object.
 * @param event_identifier, event identifier to trigger.
 *
 * @return 0 when success, -1 when fail.
 */
int linkkit_trigger_event(const void* thing_id, const char* event_identifier, handle_post_cb_fp_t cb)
{
    int res = FAIL_RETURN;
    int devid;
    int id_send = 0;
    linked_list_t* list = g_list_post_cb;

    res = IOT_DM_Legacy_Get_Devid_By_ThingId((void *)thing_id, &devid);
    if (res == SUCCESS_RETURN && event_identifier != NULL) {
        res = IOT_DM_Post_Event(devid, (char *)event_identifier,strlen((char *)event_identifier));
        dm_log_info(IOTX_DM_LOG_OPERATOR_RES, 3, res);
    }

    if (res > 0) {
        id_send = res;
        if (list && cb) _insert_post_cb(id_send, list, cb);
    } else {
        dm_log_err("DM", "%d", res);
        log_err_online("DM", "%d", res);
    }
    return res;
}

/**
 * @brief post property to cloud.
 *
 * @param thing_id, pointer to thing object.
 * @param property_identifier, used when trigger event with method "event.property.post", if set, post specified property, if NULL, post all.
 *
 * @return 0 when success, -1 when fail.
 */
int linkkit_post_property(const void* thing_id, const char* property_identifier, handle_post_cb_fp_t cb)
{
    int res = FAIL_RETURN;
    int devid;
    int id_send = 0;
    linked_list_t* list = g_list_post_cb;

    res = IOT_DM_Legacy_Get_Devid_By_ThingId((void *)thing_id, &devid);
    if (res == SUCCESS_RETURN) {
        void *handle = NULL;
        res = IOT_DM_Post_Property_Start(devid, &handle);
        if (res != SUCCESS_RETURN) {
            return res;
        }
        if (property_identifier != NULL)
            res = IOT_DM_Post_Property_Add(handle, (char *)property_identifier,strlen((char *)property_identifier));
        else
            res = IOT_DM_Post_Property_Add(handle, IOTX_DM_POST_PROPERTY_ALL, 0);
        res = IOT_DM_Post_Property_End(&handle);
    }

    if (res > 0) {
        id_send = res;
        if (list && cb) _insert_post_cb(id_send, list, cb);
    } else {
        dm_log_err("DM", "%d", res);
        log_err_online("DM", "%d", res);
    }
    return res;
}

#ifndef CM_SUPPORT_MULTI_THREAD
int linkkit_yield(int timeout_ms)
{
    return IOT_DM_Yield(timeout_ms);
}
#endif
