#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "iotx_dm_internal.h"
#include "iot_export.h"
#include "linkkit_export.h"
#include "dm_utils.h"
#include "iotx_log.h"
#include "linked_list.h"

#define LINKKIT_API_KEY_ID          "id"
#define LINKKIT_API_KEY_CODE        "code"
#define LINKKIT_API_KEY_DEVID       "devid"
#define LINKKIT_API_KEY_SERVICEID   "serviceid"
#define LINKKIT_API_KEY_PROPERTYID  "propertyid"
#define LINKKIT_API_KEY_EVENTID     "eventid"
#define LINKKIT_API_KEY_PAYLOAD     "payload"
#define LINKKIT_API_KEY_CONFIG_ID   "configId"
#define LINKKIT_API_KEY_CONFIG_SIZE "configSize"
#define LINKKIT_API_KEY_GET_TYPE    "getType"
#define LINKKIT_API_KEY_SIGN        "sign"
#define LINKKIT_API_KEY_SIGN_METHOD "signMethod"
#define LINKKIT_API_KEY_URL         "url"
#define LINKKIT_API_KEY_VERSION     "version"

#define LINKKIT_MAX_POST_CB_NUMBER 20
typedef struct _post_cb {
    int                    id_expected;
    void                  *cb;
    unsigned long long     time;
} post_cb_t;

static int g_linkkit_inited = 0;
static int g_linkkit_leave = 0;

static void *g_linkkit_mutex = NULL;
void *g_list_post_cb = NULL;
static linkkit_ops_t *g_linkkit_ops = NULL;
static void *g_user_ctx = NULL;

static handle_service_cota_callback_fp_t g_cota_callback = NULL;
static handle_service_fota_callback_fp_t g_fota_callback = NULL;

#if (CONFIG_SDK_THREAD_COST == 1)
    void *thread_process_dispatch;
    int   thread_process_disatch_exit = 0;
#endif /* CONFIG_SDK_THREAD_COST */

static void _linkkit_event_callback(iotx_dm_event_types_t type, char *payload);

#if (CONFIG_SDK_THREAD_COST == 1)
void *linkkit_dispatch(void *params)
{
    while (1) {
        if (thread_process_disatch_exit) {
            return NULL;
        }
        iotx_dm_dispatch();
        HAL_SleepMs(10);
    }
    return NULL;
}
#else
void *linkkit_dispatch()
{
    iotx_dm_dispatch();
    return NULL;
}
#endif

void linkkit_try_leave()
{
    g_linkkit_leave = 1;
}

int linkkit_is_try_leave()
{
    return g_linkkit_leave;
}

int linkkit_is_end()
{
    return !g_linkkit_inited;
}

static void _find_oldest_post_cb(void *_post_cb, va_list *params)
{
    post_cb_t *post_cb = _post_cb;
    post_cb_t **post_cb_oldest;
    unsigned long long current_time_ms;
    unsigned long long time_duration_ms;
    unsigned long long time_duration_ms_oldest;

    post_cb_oldest = va_arg(*params, post_cb_t **);
    current_time_ms = va_arg(*params, int);

    if (current_time_ms == post_cb->time) {
        time_duration_ms = 0;
    } else {
        time_duration_ms = current_time_ms > post_cb->time ? current_time_ms - post_cb->time : current_time_ms +
                           __LONG_LONG_MAX__ * 2ULL + 1ULL - post_cb->time;
    }

    if (*post_cb_oldest == NULL) {
        *post_cb_oldest = post_cb;
    }

    time_duration_ms_oldest = current_time_ms > (*post_cb_oldest)->time ? current_time_ms -
                              (*post_cb_oldest)->time : current_time_ms + __LONG_LONG_MAX__ * 2ULL + 1ULL - (*post_cb_oldest)->time;

    if (time_duration_ms > time_duration_ms_oldest) {
        *post_cb_oldest = post_cb;
    }
}


static void _insert_post_cb(int id_send, linked_list_t *list, void *cb)
{
    int list_size;
    post_cb_t *post_cb;
    post_cb_t *post_cb_oldest = NULL;
    unsigned long long current_time_ms;

    post_cb = calloc(1, sizeof(post_cb_t));
    if (!post_cb) {
        return;
    }

    current_time_ms = HAL_UptimeMs();
    post_cb->id_expected = id_send;
    post_cb->cb = cb;
    post_cb->time = current_time_ms;

    linked_list_insert(list, post_cb);

    list_size = linked_list_get_size(list);
    if (list_size > LINKKIT_MAX_POST_CB_NUMBER) {
        /* find oldest cb and remove */
        HAL_MutexLock(g_linkkit_mutex);
        linked_list_iterator(list, _find_oldest_post_cb, &post_cb_oldest, current_time_ms);
        if (post_cb_oldest) {linked_list_remove(list, post_cb_oldest);}
        HAL_MutexUnlock(g_linkkit_mutex);
        if (post_cb_oldest) {
            free(post_cb_oldest);
        }
    }
}

static void _find_post_cb_by_id(void *_post_cb, va_list *params)
{
    post_cb_t *post_cb = _post_cb;
    post_cb_t **post_cb_ret;
    int id_expected;

    id_expected = va_arg(*params, int);
    post_cb_ret = va_arg(*params, post_cb_t **);

    if (post_cb->id_expected == id_expected) {
        *post_cb_ret = post_cb;
    }
}

static void _linkkit_event_callback(iotx_dm_event_types_t type, char *payload)
{
    linkkit_ops_t *linkkit_ops = g_linkkit_ops;
    void *context = g_user_ctx;
    dm_log_info("Receive Message Type: %d", type);
    if (payload) {
        dm_log_info("Receive Message: %s", payload);
    }

    switch (type) {
        case IOTX_DM_EVENT_CLOUD_CONNECTED: {
            linkkit_ops->on_connect(context);
            break;
        }
        case IOTX_DM_EVENT_CLOUD_DISCONNECT: {
            linkkit_ops->on_disconnect(context);
            break;
        }
        case IOTX_DM_EVENT_MODEL_DOWN_RAW: {
            int res = 0, raw_data_len = 0;
            void *thing_id = NULL;
            unsigned char *raw_data = NULL;
            lite_cjson_t lite, lite_item_devid, lite_item_rawdata;

            /* Parse Payload */
            memset(&lite, 0, sizeof(lite_cjson_t));
            res = lite_cjson_parse(payload, strlen(payload), &lite);
            if (res != SUCCESS_RETURN) {
                return;
            }

            /* Parse Devid */
            memset(&lite_item_devid, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_API_KEY_DEVID, strlen(LINKKIT_API_KEY_DEVID), &lite_item_devid);
            if (res != SUCCESS_RETURN) {
                return;
            }
            dm_log_debug("Current Devid: %d", lite_item_devid.value_int);

            /* Parse Raw Data */
            memset(&lite_item_rawdata, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_API_KEY_PAYLOAD, strlen(LINKKIT_API_KEY_PAYLOAD), &lite_item_rawdata);
            if (res != SUCCESS_RETURN) {
                return;
            }
            dm_log_debug("Current Raw Data: %.*s", lite_item_rawdata.value_length, lite_item_rawdata.value);

            res = iotx_dm_legacy_get_thingid_by_devid(lite_item_devid.value_int, &thing_id);
            if (res != SUCCESS_RETURN) {
                return;
            }

            res = dm_utils_str_to_hex(lite_item_rawdata.value, lite_item_rawdata.value_length, &raw_data, &raw_data_len);
            if (res != SUCCESS_RETURN) {
                dm_log_err(DM_UTILS_LOG_MEMORY_NOT_ENOUGH);
                return;
            }
            HEXDUMP_DEBUG(raw_data,raw_data_len);
            linkkit_ops->raw_data_arrived(thing_id, raw_data, raw_data_len, context);

            DM_free(raw_data);
        }
        break;
        case IOTX_DM_EVENT_THING_SERVICE_REQUEST: {
            int res = 0;
            void *thing_id = NULL;
            char *service = NULL;
            lite_cjson_t lite, lite_item_id, lite_item_devid, lite_item_serviceid;

            /* Parse Payload */
            memset(&lite, 0, sizeof(lite_cjson_t));
            res = lite_cjson_parse(payload, strlen(payload), &lite);
            if (res != SUCCESS_RETURN) {
                return;
            }

            /* Parse Id */
            memset(&lite_item_id, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_API_KEY_ID, strlen(LINKKIT_API_KEY_ID), &lite_item_id);
            if (res != SUCCESS_RETURN) {
                return;
            }
            dm_log_debug("Current Id: %d", lite_item_id.value_int);

            /* Parse Devid */
            memset(&lite_item_devid, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_API_KEY_DEVID, strlen(LINKKIT_API_KEY_DEVID), &lite_item_devid);
            if (res != SUCCESS_RETURN) {
                return;
            }
            dm_log_debug("Current Devid: %d", lite_item_devid.value_int);

            /* Parse Serviceid */
            memset(&lite_item_serviceid, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_API_KEY_SERVICEID, strlen(LINKKIT_API_KEY_SERVICEID), &lite_item_serviceid);
            if (res != SUCCESS_RETURN) {
                return;
            }
            dm_log_debug("Current ServiceID: %.*s", lite_item_serviceid.value_length, lite_item_serviceid.value);

            res = iotx_dm_legacy_get_thingid_by_devid(lite_item_devid.value_int, &thing_id);
            if (res != SUCCESS_RETURN) {
                return;
            }

            service = DM_malloc(lite_item_serviceid.value_length + 1);
            if (service == NULL) {
                dm_log_warning(DM_UTILS_LOG_MEMORY_NOT_ENOUGH);
                return;
            }
            memset(service, 0, lite_item_serviceid.value_length + 1);
            memcpy(service, lite_item_serviceid.value, lite_item_serviceid.value_length);

            linkkit_ops->thing_call_service(thing_id, (const char *)service, lite_item_id.value_int, context);
        }
        break;
        case IOTX_DM_EVENT_LEGACY_THING_CREATED: {
            int res = 0;
            void *thing_id = NULL;
            lite_cjson_t lite, lite_item_devid;

            /* Parse Payload */
            memset(&lite, 0, sizeof(lite_cjson_t));
            res = lite_cjson_parse(payload, strlen(payload), &lite);
            if (res != SUCCESS_RETURN) {
                return;
            }

            /* Parse Id */
            memset(&lite_item_devid, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_API_KEY_DEVID, strlen(LINKKIT_API_KEY_DEVID), &lite_item_devid);
            if (res != SUCCESS_RETURN) {
                return;
            }
            dm_log_debug("Current Devid: %d", lite_item_devid.value_int);

            res = iotx_dm_legacy_get_thingid_by_devid(lite_item_devid.value_int, &thing_id);
            if (res != SUCCESS_RETURN) {
                return;
            }

            linkkit_ops->thing_create(thing_id, context);
        }
        break;
        case IOTX_DM_EVENT_THING_DISABLE: {
            int res = 0;
            void *thing_id = NULL;
            lite_cjson_t lite, lite_item_devid;

            /* Parse Payload */
            memset(&lite, 0, sizeof(lite_cjson_t));
            res = lite_cjson_parse(payload, strlen(payload), &lite);
            if (res != SUCCESS_RETURN) {
                return;
            }

            /* Parse Id */
            memset(&lite_item_devid, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_API_KEY_DEVID, strlen(LINKKIT_API_KEY_DEVID), &lite_item_devid);
            if (res != SUCCESS_RETURN) {
                return;
            }
            dm_log_debug("Current Devid: %d", lite_item_devid.value_int);

            res = iotx_dm_legacy_get_thingid_by_devid(lite_item_devid.value_int, &thing_id);
            if (res != SUCCESS_RETURN) {
                return;
            }

            linkkit_ops->thing_disable(thing_id, context);
        }
        break;
        case IOTX_DM_EVENT_THING_ENABLE: {
            int res = 0;
            void *thing_id = NULL;
            lite_cjson_t lite, lite_item_devid;

            /* Parse Payload */
            memset(&lite, 0, sizeof(lite_cjson_t));
            res = lite_cjson_parse(payload, strlen(payload), &lite);
            if (res != SUCCESS_RETURN) {
                return;
            }

            /* Parse Id */
            memset(&lite_item_devid, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_API_KEY_DEVID, strlen(LINKKIT_API_KEY_DEVID), &lite_item_devid);
            if (res != SUCCESS_RETURN) {
                return;
            }
            dm_log_debug("Current Devid: %d", lite_item_devid.value_int);

            res = iotx_dm_legacy_get_thingid_by_devid(lite_item_devid.value_int, &thing_id);
            if (res != SUCCESS_RETURN) {
                return;
            }

            linkkit_ops->thing_enable(thing_id, context);
        }
        break;
        case IOTX_DM_EVENT_PROPERTY_SET: {
            int res = 0;
            void *thing_id = NULL;
            char *propertyid = NULL;
            lite_cjson_t lite, lite_item_devid, lite_item_propertyid;

            /* Parse Payload */
            memset(&lite, 0, sizeof(lite_cjson_t));
            res = lite_cjson_parse(payload, strlen(payload), &lite);
            if (res != SUCCESS_RETURN) {
                return;
            }

            /* Parse Devid */
            memset(&lite_item_devid, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_API_KEY_DEVID, strlen(LINKKIT_API_KEY_DEVID), &lite_item_devid);
            if (res != SUCCESS_RETURN) {
                return;
            }
            dm_log_debug("Current Devid: %d", lite_item_devid.value_int);

            /* Parse Property ID */
            memset(&lite_item_propertyid, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_API_KEY_PROPERTYID, strlen(LINKKIT_API_KEY_PROPERTYID),
                                         &lite_item_propertyid);
            if (res != SUCCESS_RETURN) {
                return;
            }
            dm_log_debug("Current PropertyID: %.*s", lite_item_propertyid.value_length, lite_item_propertyid.value);

            res = iotx_dm_legacy_get_thingid_by_devid(lite_item_devid.value_int, &thing_id);
            if (res != SUCCESS_RETURN) {
                return;
            }

            propertyid = DM_malloc(lite_item_propertyid.value_length + 1);
            if (propertyid == NULL) {
                dm_log_warning(DM_UTILS_LOG_MEMORY_NOT_ENOUGH);
                return;
            }
            memset(propertyid, 0, lite_item_propertyid.value_length + 1);
            memcpy(propertyid, lite_item_propertyid.value, lite_item_propertyid.value_length);

            linkkit_ops->thing_prop_changed(thing_id, propertyid, context);
            DM_free(propertyid);
        }
        break;
        case IOTX_DM_EVENT_EVENT_PROPERTY_POST_REPLY: {
            int res = 0;
            void *thing_id = NULL;
            linked_list_t *list = g_list_post_cb;
            post_cb_t *post_cb = NULL;
            handle_post_cb_fp_t handle_post_cb_fp;
            lite_cjson_t lite, lite_item_id, lite_item_code, lite_item_devid;

            /* Parse Payload */
            memset(&lite, 0, sizeof(lite_cjson_t));
            res = lite_cjson_parse(payload, strlen(payload), &lite);
            if (res != SUCCESS_RETURN) {
                return;
            }

            /* Parse Id */
            memset(&lite_item_id, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_API_KEY_ID, strlen(LINKKIT_API_KEY_ID), &lite_item_id);
            if (res != SUCCESS_RETURN) {
                return;
            }
            dm_log_debug("Current Id: %d", lite_item_id.value_int);

            /* Parse Code */
            memset(&lite_item_code, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_API_KEY_CODE, strlen(LINKKIT_API_KEY_CODE), &lite_item_code);
            if (res != SUCCESS_RETURN) {
                return;
            }
            dm_log_debug("Current Code: %d", lite_item_code.value_int);

            /* Parse Devid */
            memset(&lite_item_devid, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_API_KEY_DEVID, strlen(LINKKIT_API_KEY_DEVID), &lite_item_devid);
            if (res != SUCCESS_RETURN) {
                return;
            }
            dm_log_debug("Current Devid: %d", lite_item_devid.value_int);

            res = iotx_dm_legacy_get_thingid_by_devid(lite_item_devid.value_int, &thing_id);
            if (res != SUCCESS_RETURN) {
                return;
            }

            if (list) {
                HAL_MutexLock(g_linkkit_mutex);
                linked_list_iterator(list, _find_post_cb_by_id, lite_item_id.value_int, &post_cb);
                if (post_cb) {linked_list_remove(list, post_cb);}
                HAL_MutexUnlock(g_linkkit_mutex);
                if (post_cb && post_cb->cb) {
                    handle_post_cb_fp = post_cb->cb;
                    handle_post_cb_fp(thing_id, lite_item_id.value_int, lite_item_code.value_int, NULL, context);

                    free(post_cb);
                }
            }
        }
        break;
        case IOTX_DM_EVENT_EVENT_SPECIFIC_POST_REPLY: {
            int res = 0;
            void *thing_id = NULL;
            char *eventid = NULL;
            linked_list_t *list = g_list_post_cb;
            post_cb_t *post_cb = NULL;
            handle_post_cb_fp_t handle_post_cb_fp;
            lite_cjson_t lite, lite_item_id, lite_item_code, lite_item_devid, lite_item_eventid;

            /* Parse Payload */
            memset(&lite, 0, sizeof(lite_cjson_t));
            res = lite_cjson_parse(payload, strlen(payload), &lite);
            if (res != SUCCESS_RETURN) {
                return;
            }

            /* Parse Id */
            memset(&lite_item_id, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_API_KEY_ID, strlen(LINKKIT_API_KEY_ID), &lite_item_id);
            if (res != SUCCESS_RETURN) {
                return;
            }
            dm_log_debug("Current Id: %d", lite_item_id.value_int);

            /* Parse Code */
            memset(&lite_item_code, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_API_KEY_CODE, strlen(LINKKIT_API_KEY_CODE), &lite_item_code);
            if (res != SUCCESS_RETURN) {
                return;
            }
            dm_log_debug("Current Code: %d", lite_item_code.value_int);

            /* Parse Devid */
            memset(&lite_item_devid, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_API_KEY_DEVID, strlen(LINKKIT_API_KEY_DEVID), &lite_item_devid);
            if (res != SUCCESS_RETURN) {
                return;
            }
            dm_log_debug("Current Devid: %d", lite_item_devid.value_int);

            /* Parse Property ID */
            memset(&lite_item_eventid, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_API_KEY_EVENTID, strlen(LINKKIT_API_KEY_EVENTID), &lite_item_eventid);
            if (res != SUCCESS_RETURN) {
                return;
            }
            dm_log_debug("Current EventID: %.*s", lite_item_eventid.value_length, lite_item_eventid.value);

            res = iotx_dm_legacy_get_thingid_by_devid(lite_item_devid.value_int, &thing_id);
            if (res != SUCCESS_RETURN) {
                return;
            }

            eventid = DM_malloc(lite_item_eventid.value_length + 1);
            if (eventid == NULL) {
                dm_log_warning(DM_UTILS_LOG_MEMORY_NOT_ENOUGH);
                return;
            }
            memset(eventid, 0, lite_item_eventid.value_length + 1);
            memcpy(eventid, lite_item_eventid.value, lite_item_eventid.value_length);

            if (list) {
                HAL_MutexLock(g_linkkit_mutex);
                linked_list_iterator(list, _find_post_cb_by_id, lite_item_id.value_int, &post_cb);
                if (post_cb) {linked_list_remove(list, post_cb);}
                HAL_MutexUnlock(g_linkkit_mutex);
                if (post_cb && post_cb->cb) {
                    handle_post_cb_fp = post_cb->cb;
                    handle_post_cb_fp(thing_id, lite_item_id.value_int, lite_item_code.value_int, NULL, context);

                    free(post_cb);
                }
            }

            DM_free(eventid);
        }
        break;
        case IOTX_DM_EVENT_COTA_NEW_CONFIG: {
            int res = 0;
            lite_cjson_t lite, lite_item_configid, lite_item_configsize, lite_item_gettype;
            lite_cjson_t lite_item_sign, lite_item_signmethod, lite_item_url;
            char *config_id = NULL, *get_type = NULL, *sign = NULL, *sign_method = NULL, *url = NULL;

            /* Parse Payload */
            memset(&lite, 0, sizeof(lite_cjson_t));
            res = lite_cjson_parse(payload, strlen(payload), &lite);
            if (res != SUCCESS_RETURN || !lite_cjson_is_object(&lite)) {
                return;
            }

            /* Parse Config ID */
            memset(&lite_item_configid, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_API_KEY_CONFIG_ID, strlen(LINKKIT_API_KEY_CONFIG_ID), &lite_item_configid);
            if (res != SUCCESS_RETURN || !lite_cjson_is_string(&lite_item_configid)) {
                return;
            }
            dm_log_debug("Current Config ID: %.*s", lite_item_configid.value_length,lite_item_configid.value);

            /* Parse Config Size */
            memset(&lite_item_configsize, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_API_KEY_CONFIG_SIZE, strlen(LINKKIT_API_KEY_CONFIG_SIZE), &lite_item_configsize);
            if (res != SUCCESS_RETURN || !lite_cjson_is_number(&lite_item_configsize)) {
                return;
            }
            dm_log_debug("Current Config Size: %d", lite_item_configsize.value_int);

            /* Parse Get Type */
            memset(&lite_item_gettype, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_API_KEY_GET_TYPE, strlen(LINKKIT_API_KEY_GET_TYPE), &lite_item_gettype);
            if (res != SUCCESS_RETURN || !lite_cjson_is_string(&lite_item_gettype)) {
                return;
            }
            dm_log_debug("Current Get Type: %.*s", lite_item_gettype.value_length,lite_item_gettype.value);

            /* Parse Sign */
            memset(&lite_item_sign, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_API_KEY_SIGN, strlen(LINKKIT_API_KEY_SIGN), &lite_item_sign);
            if (res != SUCCESS_RETURN || !lite_cjson_is_string(&lite_item_sign)) {
                return;
            }
            dm_log_debug("Current Sign: %.*s", lite_item_sign.value_length,lite_item_sign.value);

            /* Parse Sign Method*/
            memset(&lite_item_signmethod, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_API_KEY_SIGN_METHOD, strlen(LINKKIT_API_KEY_SIGN_METHOD), &lite_item_signmethod);
            if (res != SUCCESS_RETURN || !lite_cjson_is_string(&lite_item_signmethod)) {
                return;
            }
            dm_log_debug("Current Sign Method: %.*s", lite_item_signmethod.value_length,lite_item_signmethod.value);

            /* Parse URL */
            memset(&lite_item_url, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_API_KEY_URL, strlen(LINKKIT_API_KEY_URL), &lite_item_url);
            if (res != SUCCESS_RETURN || !lite_cjson_is_string(&lite_item_url)) {
                return;
            }
            dm_log_debug("Current URL: %.*s", lite_item_url.value_length,lite_item_url.value);

            dm_utils_copy(lite_item_configid.value,lite_item_configid.value_length,(void **)&config_id,lite_item_configid.value_length + 1);
            dm_utils_copy(lite_item_gettype.value,lite_item_gettype.value_length,(void **)&get_type,lite_item_gettype.value_length + 1);
            dm_utils_copy(lite_item_sign.value,lite_item_sign.value_length,(void **)&sign,lite_item_sign.value_length + 1);
            dm_utils_copy(lite_item_signmethod.value,lite_item_signmethod.value_length,(void **)&sign_method,lite_item_signmethod.value_length + 1);
            dm_utils_copy(lite_item_url.value,lite_item_url.value_length,(void **)&url,lite_item_url.value_length + 1);

            if (config_id == NULL || get_type == NULL || sign == NULL || sign_method == NULL || url == NULL) {
                dm_log_err(DM_UTILS_LOG_MEMORY_NOT_ENOUGH);
                if (config_id) {DM_free(config_id);}
                if (get_type) {DM_free(get_type);}
                if (sign) {DM_free(sign);}
                if (sign_method) {DM_free(sign_method);}
                if (url) {DM_free(url);}
                return;
            }

            if (g_cota_callback){
                g_cota_callback(service_cota_callback_type_new_version_detected,config_id,lite_item_configsize.value_int,get_type,sign,sign_method,url);
            }

            if (config_id) {DM_free(config_id);}
            if (get_type) {DM_free(get_type);}
            if (sign) {DM_free(sign);}
            if (sign_method) {DM_free(sign_method);}
            if (url) {DM_free(url);}
        }
        break;
        case IOTX_DM_EVENT_FOTA_NEW_FIRMWARE: {
            int res = 0;
            lite_cjson_t lite, lite_item_version;
            char *version = NULL;

            /* Parse Payload */
            memset(&lite, 0, sizeof(lite_cjson_t));
            res = lite_cjson_parse(payload, strlen(payload), &lite);
            if (res != SUCCESS_RETURN || !lite_cjson_is_object(&lite)) {
                return;
            }

            /* Parse Version */
            memset(&lite_item_version, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_API_KEY_VERSION, strlen(LINKKIT_API_KEY_VERSION), &lite_item_version);
            if (res != SUCCESS_RETURN || !lite_cjson_is_string(&lite_item_version)) {
                return;
            }
            dm_log_debug("Current Firmware Version: %.*s", lite_item_version.value_length,lite_item_version.value);

            dm_utils_copy(lite_item_version.value,lite_item_version.value_length,(void **)&version,lite_item_version.value_length + 1);
            if (version == NULL) {
                dm_log_err(DM_UTILS_LOG_MEMORY_NOT_ENOUGH);
                return;
            }

            if (g_fota_callback) {
                g_fota_callback(service_fota_callback_type_new_version_detected,version);
            }

            if (version) {DM_free(version);}
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

int linkkit_set_opt(linkkit_opt_t opt, void *data)
{
    if (data == NULL) {return FAIL_RETURN;}

    return iotx_dm_set_opt(opt, data);
}

int linkkit_start(int max_buffered_msg, int get_tsl_from_cloud, linkkit_loglevel_t log_level, linkkit_ops_t *ops,
                  linkkit_cloud_domain_type_t domain_type, void *user_context)
{
    int ret = -1;
    iotx_dm_init_params_t dm_init_params;
#if (CONFIG_SDK_THREAD_COST == 1)
    int stack_used;
#endif /* CONFIG_SDK_THREAD_COST */

    if (g_linkkit_inited) {
        return FAIL_RETURN;
    }

    if (!ops || max_buffered_msg <= 0) {
        return ret;
    }
    g_linkkit_ops = ops;
    g_user_ctx = user_context;
    memset(&dm_init_params, 0, sizeof(iotx_dm_init_params_t));
    dm_init_params.secret_type = IOTX_DM_DEVICE_SECRET_DEVICE;
    dm_init_params.domain_type = domain_type;
    dm_init_params.event_callback = _linkkit_event_callback;
    
    /* Setup log level */
    if ((LOGLEVEL)log_level > LOG_DEBUG_LEVEL) {
        return FAIL_RETURN;
    }
    LITE_set_loglevel(log_level); 
    
#if (CONFIG_SDK_THREAD_COST == 1)
    HAL_ThreadCreate(&thread_process_dispatch, linkkit_dispatch, NULL, NULL, &stack_used);
    HAL_ThreadDetach(thread_process_dispatch);
#endif /* CONFIG_SDK_THREAD_COST */

    ret = iotx_dm_construct(&dm_init_params);
    if (ret != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Get TSL from cloud if need */
    if (get_tsl_from_cloud != 0) {
        ret = iotx_dm_set_tsl(IOTX_DM_LOCAL_NODE_DEVID, IOTX_DM_TSL_SOURCE_CLOUD, NULL, 0);
        if (ret < SUCCESS_RETURN) {
            return FAIL_RETURN;
        }
    }
    g_list_post_cb = linked_list_create("post cb", 1);
    if (!g_list_post_cb) {
        return -1;
    }

    g_linkkit_mutex = HAL_MutexCreate();
    if (g_linkkit_mutex == NULL) {
        return -1;
    }
    
    g_linkkit_inited = 1;
    g_linkkit_leave = 0;
    return SUCCESS_RETURN;
}

static void _free_post_cb(void *_post_cb, va_list *params)
{
    void *post_cb = _post_cb;

    if (post_cb) {
        free(post_cb);
    }
}

int linkkit_end(void)
{
    linked_list_t *list;

    if (!g_linkkit_inited) {return FAIL_RETURN;}

    g_linkkit_inited = 0;
    
#if (CONFIG_SDK_THREAD_COST == 1)
    thread_process_disatch_exit = 1;
#endif
    list = g_list_post_cb;
    if (list) {
        linked_list_iterator(list, _free_post_cb);
        linked_list_destroy(list);
    }
    
    return iotx_dm_destroy();
}

void *linkkit_set_tsl(const char *tsl, int tsl_len)
{
    void *thing = NULL;
    int res = 0;

    if(!g_linkkit_inited) {return NULL;}

    res = iotx_dm_set_tsl(IOTX_DM_LOCAL_NODE_DEVID, IOTX_DM_TSL_SOURCE_LOCAL, tsl, tsl_len);
    if (res != SUCCESS_RETURN) {
        return NULL;
    }
    //ToDo: get ptr of device id
    iotx_dm_legacy_get_thingid_by_devid(IOTX_DM_LOCAL_NODE_DEVID, &thing);
    if (res != SUCCESS_RETURN) {
        dm_log_err("get thing id fail");
    }
    return thing;
}


int linkkit_set_value(linkkit_method_set_t method_set, const void *thing_id, const char *identifier,
                      const void *value, const char *value_str)
{
    int res = FAIL_RETURN;
    int devid;

    if(!g_linkkit_inited) {return FAIL_RETURN;}

    if (thing_id == NULL || identifier == NULL || ((value == NULL && value_str == NULL))) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = iotx_dm_legacy_get_devid_by_thingid((void *)thing_id, &devid);
    if (res == SUCCESS_RETURN) {
        switch (method_set) {
            case linkkit_method_set_property_value: {
                res = iotx_dm_legacy_set_property_value(devid, (char *)identifier, strlen(identifier), (void *)value, (char *)value_str);
                dm_log_info(DM_UTILS_LOG_OPERATOR_RES, 0, res);
                break;
            }
            case linkkit_method_set_event_output_value: {
                res = iotx_dm_legacy_set_event_output_value(devid, (char *)identifier, strlen(identifier), (void *)value, (char *)value_str);
                dm_log_info(DM_UTILS_LOG_OPERATOR_RES, 0, res);
                break;
            }
            case linkkit_method_set_service_output_value: {
                res = iotx_dm_legacy_set_service_output_value(devid, (char *)identifier, strlen(identifier), (void *)value, (char *)value_str);
                dm_log_info(DM_UTILS_LOG_OPERATOR_RES, 0, res);
                break;
            }
            default: {
                res = FAIL_RETURN;
                dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
                break;
            }
        }
    } else {
        dm_log_err(DM_UTILS_LOG_GET_INVALID_DEV_ID);
    }
    
    return res;
}

int linkkit_get_value(linkkit_method_get_t method_get, const void *thing_id, const char *identifier,
                      void *value, char **value_str)
{
    int res = FAIL_RETURN;
    int devid;

    if(!g_linkkit_inited) {return FAIL_RETURN;}

    if (thing_id == NULL || identifier == NULL || ((value == NULL && value_str == NULL))) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = iotx_dm_legacy_get_devid_by_thingid((void *)thing_id, &devid);
    if (res == SUCCESS_RETURN) {
        switch (method_get) {
            case linkkit_method_get_property_value: {
                res = iotx_dm_legacy_get_property_value(devid, (char *)identifier, strlen(identifier), value, value_str);
                dm_log_info(DM_UTILS_LOG_OPERATOR_RES, 1, res);
                break;
            }
            case linkkit_method_get_event_output_value: {
                res = iotx_dm_legacy_get_event_output_value(devid, (char *)identifier, strlen(identifier), value, value_str);
                dm_log_info(DM_UTILS_LOG_OPERATOR_RES, 1, res);
                break;
            }
            case linkkit_method_get_service_input_value: {
                res = iotx_dm_legacy_get_service_input_value(devid, (char *)identifier, strlen(identifier), value, value_str);
                dm_log_info(DM_UTILS_LOG_OPERATOR_RES, 1, res);
                break;
            }
            case linkkit_method_get_service_output_value: {
                res = iotx_dm_legacy_get_service_output_value(devid, (char *)identifier, strlen(identifier), value, value_str);
                dm_log_info(DM_UTILS_LOG_OPERATOR_RES, 1, res);
                break;
            }
            default: {
                dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
                break;
            }
        }
    } else {
        dm_log_err(DM_UTILS_LOG_GET_INVALID_DEV_ID);
    }
    return res;
}

int linkkit_answer_service(const void *thing_id, const char *service_identifier, int response_id, int code)
{
    int res = 0, devid = 0;

    if(!g_linkkit_inited) {return FAIL_RETURN;}

    if (thing_id == NULL || service_identifier == NULL) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = iotx_dm_legacy_get_devid_by_thingid((void *)thing_id, &devid);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    //ToDo: need to analyze
    res = iotx_dm_send_service_response(devid, response_id, (iotx_dm_error_code_t)code, (char *)service_identifier,
                                       strlen(service_identifier));

    return res;
}

int linkkit_invoke_raw_service(const void *thing_id, int is_up_raw, void *raw_data, int raw_data_length)
{
    int res = FAIL_RETURN;
    int devid;

    if(!g_linkkit_inited) {return FAIL_RETURN;}

    if (thing_id == NULL || raw_data == NULL || raw_data_length <= 0) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = iotx_dm_legacy_get_devid_by_thingid((void *)thing_id, &devid);
    if (res == SUCCESS_RETURN && raw_data != NULL) {
        res = iotx_dm_post_rawdata(devid, raw_data, raw_data_length);
    }
    return res;
}

int linkkit_trigger_extended_info_operate(const void *thing_id, const char *params,
        linkkit_extended_info_operate_t linkkit_extended_info_operation)
{
    int res = FAIL_RETURN;
    int devid;

    if(!g_linkkit_inited) {return FAIL_RETURN;}

    if (thing_id == NULL || params == NULL) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = iotx_dm_legacy_get_devid_by_thingid((void *)thing_id, &devid);
    if (res == SUCCESS_RETURN && params != NULL) {
        int len  = strlen(params);
        if (linkkit_extended_info_operation == linkkit_extended_info_operate_update) {
            res = iotx_dm_deviceinfo_update(devid, (char *)params, len);
            dm_log_info(DM_UTILS_LOG_OPERATOR_RES, 2, res);
        } else if (linkkit_extended_info_operation == linkkit_extended_info_operate_delete) {
            res = iotx_dm_deviceinfo_delete(devid, (char *)params, len);
            dm_log_info(DM_UTILS_LOG_OPERATOR_RES, 2, res);
        } else {
            dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        }
    } else {
        dm_log_err(DM_UTILS_LOG_GET_INVALID_DEV_ID);
    }
    return res;
}

int linkkit_trigger_event(const void *thing_id, const char *event_identifier, handle_post_cb_fp_t cb)
{
    int res = FAIL_RETURN;
    int devid;
    int id_send = 0;
    linked_list_t *list = g_list_post_cb;

    if(!g_linkkit_inited) {return FAIL_RETURN;}

    if (thing_id == NULL || event_identifier == NULL) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = iotx_dm_legacy_get_devid_by_thingid((void *)thing_id, &devid);
    if (res == SUCCESS_RETURN && event_identifier != NULL) {
        res = iotx_dm_post_event(devid, (char *)event_identifier, strlen((char *)event_identifier));
        dm_log_info(DM_UTILS_LOG_OPERATOR_RES, 3, res);
    }

    if (res > 0) {
        id_send = res;
        if (list && cb) {
            _insert_post_cb(id_send, list, cb);
        }
    } else {
        dm_log_err("DM", "%d", res);
        log_err_online("DM", "%d", res);
    }
    return res;
}

int linkkit_post_property(const void *thing_id, const char *property_identifier, handle_post_cb_fp_t cb)
{
    int res = FAIL_RETURN, ret = FAIL_RETURN;
    int devid;
    int id_send = 0;
    linked_list_t *list = g_list_post_cb;

    if(!g_linkkit_inited) {return FAIL_RETURN;}

    if (thing_id == NULL) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = iotx_dm_legacy_get_devid_by_thingid((void *)thing_id, &devid);
    if (res == SUCCESS_RETURN) {
        void *handle = NULL;
        res = iotx_dm_post_property_start(devid, &handle);
        if (res != SUCCESS_RETURN) {
            return res;
        }
        if (property_identifier != NULL) {
            res = iotx_dm_post_property_add(handle, (char *)property_identifier, strlen((char *)property_identifier));
        } else {
            res = iotx_dm_post_property_add(handle, IOTX_DM_POST_PROPERTY_ALL, 0);
        }
        ret = res;
        res = iotx_dm_post_property_end(&handle);
    }

    if (res > 0) {
        id_send = res;
        if (list && cb) {
            _insert_post_cb(id_send, list, cb);
        }
    } else {
        dm_log_err("DM", "%d", res);
        log_err_online("DM", "%d", res);
    }
    return ret;
}

int linkkit_yield(int timeout_ms)
{
    if(!g_linkkit_inited) {return FAIL_RETURN;}

    if (timeout_ms <= 0) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    return iotx_dm_yield(timeout_ms);
}

int linkkit_cota_init(handle_service_cota_callback_fp_t callback_fp)
{
    g_cota_callback = callback_fp;

    return 0;
}

int linkkit_invoke_cota_service(void* data_buf, int data_buf_length)
{
    if(!g_linkkit_inited) {return FAIL_RETURN;}

    if (data_buf == NULL || data_buf_length <= 0) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    return iotx_dm_cota_perform_sync(data_buf,data_buf_length);
}

int linkkit_invoke_cota_get_config(const char* config_scope, const char* get_type, const char* attribute_Keys, void* option)
{
    if(!g_linkkit_inited) {return FAIL_RETURN;}

    if (config_scope == NULL || get_type == NULL || attribute_Keys == NULL) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    return iotx_dm_cota_get_config(config_scope,get_type,attribute_Keys);
}

int linkkit_fota_init(handle_service_fota_callback_fp_t callback_fp)
{
    g_fota_callback = callback_fp;

    return 0;
}

int linkkit_invoke_fota_service(void* data_buf, int data_buf_length)
{
    if(!g_linkkit_inited) {return FAIL_RETURN;}

    if (data_buf == NULL || data_buf_length <= 0) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }
    
    return iotx_dm_fota_perform_sync(data_buf,data_buf_length);
}
