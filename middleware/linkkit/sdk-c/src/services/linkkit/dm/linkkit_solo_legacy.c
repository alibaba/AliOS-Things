/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#include "iotx_dm_internal.h"
#include "linkkit_export.h"
#include "linkkit_solo_legacy.h"
#ifdef DEPRECATED_LINKKIT
linkkit_solo_legacy_ctx_t g_linkkit_solo_legacy_ctx = {0};

static deprecated linkkit_solo_legacy_ctx_t *_linkkit_solo_legacy_get_ctx(void)
{
    return &g_linkkit_solo_legacy_ctx;
}

static void _linkkit_solo_mutex_lock(void)
{
    linkkit_solo_legacy_ctx_t *linkkit_solo_ctx = _linkkit_solo_legacy_get_ctx();
    if (linkkit_solo_ctx->mutex) {
        HAL_MutexLock(linkkit_solo_ctx->mutex);
    }
}

static void _linkkit_solo_mutex_unlock(void)
{
    linkkit_solo_legacy_ctx_t *linkkit_solo_ctx = _linkkit_solo_legacy_get_ctx();
    if (linkkit_solo_ctx->mutex) {
        HAL_MutexUnlock(linkkit_solo_ctx->mutex);
    }
}

static void _linkkit_solo_upstream_mutex_lock(void)
{
    linkkit_solo_legacy_ctx_t *linkkit_solo_ctx = _linkkit_solo_legacy_get_ctx();
    if (linkkit_solo_ctx->upstream_mutex) {
        HAL_MutexLock(linkkit_solo_ctx->upstream_mutex);
    }
}

static void _linkkit_solo_upstream_mutex_unlock(void)
{
    linkkit_solo_legacy_ctx_t *linkkit_solo_ctx = _linkkit_solo_legacy_get_ctx();
    if (linkkit_solo_ctx->upstream_mutex) {
        HAL_MutexUnlock(linkkit_solo_ctx->upstream_mutex);
    }
}

static int _linkkit_solo_upstream_callback_list_insert(int msgid, handle_post_cb_fp_t callback)
{
    int count = 0;
    linkkit_solo_legacy_ctx_t *linkkit_solo_ctx = _linkkit_solo_legacy_get_ctx();
    linkkit_solo_upstream_callback_node_t *search_node = NULL;

    list_for_each_entry(search_node, &linkkit_solo_ctx->callback_list, linked_list, linkkit_solo_upstream_callback_node_t) {
        count++;
        if (search_node->msgid == msgid) {
            dm_log_info("Message ID Already Exist: %d", msgid);
            return FAIL_RETURN;
        }
    }

    dm_log_info("linkkit_solo_upstream_callback_list node count: %d", count);

    search_node = DM_malloc(sizeof(linkkit_solo_upstream_callback_node_t));
    if (search_node == NULL) {
        return FAIL_RETURN;
    }
    memset(search_node, 0, sizeof(linkkit_solo_upstream_callback_node_t));

    search_node->msgid = msgid;
    search_node->callback = callback;
    search_node->callback = callback;
    INIT_LIST_HEAD(&search_node->linked_list);

    list_add(&search_node->linked_list, &linkkit_solo_ctx->callback_list);

    return SUCCESS_RETURN;
}

static int _linkkit_solo_upstream_callback_list_remove(int msgid)
{
    linkkit_solo_legacy_ctx_t *linkkit_solo_ctx = _linkkit_solo_legacy_get_ctx();
    linkkit_solo_upstream_callback_node_t *search_node = NULL;

    list_for_each_entry(search_node, &linkkit_solo_ctx->callback_list, linked_list, linkkit_solo_upstream_callback_node_t) {
        if (search_node->msgid == msgid) {
            list_del(&search_node->linked_list);
            DM_free(search_node);
            return SUCCESS_RETURN;
        }
    }

    return FAIL_RETURN;
}

static int _linkkit_solo_upstream_callback_list_search(int msgid, linkkit_solo_upstream_callback_node_t **node)
{
    linkkit_solo_legacy_ctx_t *linkkit_solo_ctx = _linkkit_solo_legacy_get_ctx();
    linkkit_solo_upstream_callback_node_t *search_node = NULL;

    list_for_each_entry(search_node, &linkkit_solo_ctx->callback_list, linked_list, linkkit_solo_upstream_callback_node_t) {
        if (search_node->msgid == msgid) {
            *node = search_node;
            return SUCCESS_RETURN;
        }
    }

    return FAIL_RETURN;
}

static int _linkkit_solo_upstream_callback_list_destroy(void)
{
    linkkit_solo_legacy_ctx_t *linkkit_solo_ctx = _linkkit_solo_legacy_get_ctx();
    linkkit_solo_upstream_callback_node_t *search_node = NULL, *next_node = NULL;

    list_for_each_entry_safe(search_node, next_node, &linkkit_solo_ctx->callback_list, linked_list,
                             linkkit_solo_upstream_callback_node_t) {
        list_del(&search_node->linked_list);
        DM_free(search_node);
    }

    return FAIL_RETURN;
}
#endif

void *linkkit_dispatch(void)
{
#ifdef DEPRECATED_LINKKIT
    iotx_dm_dispatch();
#endif
    return NULL;
}

void deprecated linkkit_try_leave(void)
{
#ifdef DEPRECATED_LINKKIT
    linkkit_solo_legacy_ctx_t *linkkit_solo_ctx = _linkkit_solo_legacy_get_ctx();
    linkkit_solo_ctx->is_leaved = 1;
#endif
}

int deprecated linkkit_is_try_leave(void)
{
#ifdef DEPRECATED_LINKKIT
    linkkit_solo_legacy_ctx_t *linkkit_solo_ctx = _linkkit_solo_legacy_get_ctx();
    return linkkit_solo_ctx->is_leaved;
#else
    return SUCCESS_RETURN;
#endif
}

int deprecated linkkit_is_end(void)
{
#ifdef DEPRECATED_LINKKIT
    linkkit_solo_legacy_ctx_t *linkkit_solo_ctx = _linkkit_solo_legacy_get_ctx();
    return (linkkit_solo_ctx->is_started == 0);
#else
    return SUCCESS_RETURN;
#endif
}

int deprecated linkkit_set_opt(linkkit_opt_t opt, void *data)
{
#ifdef DEPRECATED_LINKKIT
    int res = 0;

    if (data == NULL) {
        return FAIL_RETURN;
    }

    _linkkit_solo_mutex_lock();
    res = iotx_dm_set_opt(opt, data);
    _linkkit_solo_mutex_unlock();

    return res;
#else
    return SUCCESS_RETURN;
#endif
}

#ifdef DEPRECATED_LINKKIT
static void _linkkit_solo_event_callback(iotx_dm_event_types_t type, char *payload)
{
    linkkit_solo_legacy_ctx_t *linkkit_solo_ctx = _linkkit_solo_legacy_get_ctx();
    dm_log_info("Receive Message Type: %d", type);
    if (payload) {
        dm_log_info("Receive Message: %s", payload);
    }

    switch (type) {
        case IOTX_DM_EVENT_CLOUD_CONNECTED: {
            if (linkkit_solo_ctx->user_callback->on_connect) {
                linkkit_solo_ctx->user_callback->on_connect(linkkit_solo_ctx->user_context);
            }
        }
        break;
        case IOTX_DM_EVENT_CLOUD_DISCONNECT: {
            if (linkkit_solo_ctx->user_callback->on_disconnect) {
                linkkit_solo_ctx->user_callback->on_disconnect(linkkit_solo_ctx->user_context);
            }
        }
        break;
        case IOTX_DM_EVENT_MODEL_DOWN_RAW: {
            int res = 0, raw_data_len = 0;
            void *thing_id = NULL;
            unsigned char *raw_data = NULL;
            lite_cjson_t lite, lite_item_devid, lite_item_rawdata;

            if (payload == NULL) {
                return;
            }

            /* Parse Payload */
            memset(&lite, 0, sizeof(lite_cjson_t));
            res = lite_cjson_parse(payload, strlen(payload), &lite);
            if (res != SUCCESS_RETURN) {
                return;
            }

            /* Parse Devid */
            memset(&lite_item_devid, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_SOLO_LEGACY_KEY_DEVID, strlen(LINKKIT_SOLO_LEGACY_KEY_DEVID),
                                         &lite_item_devid);
            if (res != SUCCESS_RETURN) {
                return;
            }
            dm_log_debug("Current Devid: %d", lite_item_devid.value_int);

            /* Parse Raw Data */
            memset(&lite_item_rawdata, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_SOLO_LEGACY_KEY_PAYLOAD, strlen(LINKKIT_SOLO_LEGACY_KEY_PAYLOAD),
                                         &lite_item_rawdata);
            if (res != SUCCESS_RETURN) {
                return;
            }
            dm_log_debug("Current Raw Data: %.*s", lite_item_rawdata.value_length, lite_item_rawdata.value);

            res = iotx_dm_deprecated_legacy_get_thingid_by_devid(lite_item_devid.value_int, &thing_id);
            if (res != SUCCESS_RETURN) {
                return;
            }

            res = dm_utils_str_to_hex(lite_item_rawdata.value, lite_item_rawdata.value_length, &raw_data, &raw_data_len);
            if (res != SUCCESS_RETURN) {
                dm_log_err(DM_UTILS_LOG_MEMORY_NOT_ENOUGH);
                return;
            }
            HEXDUMP_DEBUG(raw_data, raw_data_len);

            if (linkkit_solo_ctx->user_callback->raw_data_arrived) {
                linkkit_solo_ctx->user_callback->raw_data_arrived(thing_id, raw_data, raw_data_len, linkkit_solo_ctx->user_context);
            }

            DM_free(raw_data);
        }
        break;
        case IOTX_DM_EVENT_THING_SERVICE_REQUEST: {
            int res = 0;
            void *thing_id = NULL;
            char *service = NULL;
            lite_cjson_t lite, lite_item_id, lite_item_devid, lite_item_serviceid;

            if (payload == NULL) {
                return;
            }

            /* Parse Payload */
            memset(&lite, 0, sizeof(lite_cjson_t));
            res = lite_cjson_parse(payload, strlen(payload), &lite);
            if (res != SUCCESS_RETURN) {
                return;
            }

            /* Parse Id */
            memset(&lite_item_id, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_SOLO_LEGACY_KEY_ID, strlen(LINKKIT_SOLO_LEGACY_KEY_ID), &lite_item_id);
            if (res != SUCCESS_RETURN) {
                return;
            }
            dm_log_debug("Current Id: %d", lite_item_id.value_int);

            /* Parse Devid */
            memset(&lite_item_devid, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_SOLO_LEGACY_KEY_DEVID, strlen(LINKKIT_SOLO_LEGACY_KEY_DEVID),
                                         &lite_item_devid);
            if (res != SUCCESS_RETURN) {
                return;
            }
            dm_log_debug("Current Devid: %d", lite_item_devid.value_int);

            /* Parse Serviceid */
            memset(&lite_item_serviceid, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_SOLO_LEGACY_KEY_SERVICEID, strlen(LINKKIT_SOLO_LEGACY_KEY_SERVICEID),
                                         &lite_item_serviceid);
            if (res != SUCCESS_RETURN) {
                return;
            }
            dm_log_debug("Current ServiceID: %.*s", lite_item_serviceid.value_length, lite_item_serviceid.value);

            res = iotx_dm_deprecated_legacy_get_thingid_by_devid(lite_item_devid.value_int, &thing_id);
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

            if (linkkit_solo_ctx->user_callback->thing_call_service) {
                linkkit_solo_ctx->user_callback->thing_call_service(thing_id, (const char *)service, lite_item_id.value_int,
                        linkkit_solo_ctx->user_context);
            }

            DM_free(service);
        }
        break;
        case IOTX_DM_EVENT_LEGACY_THING_CREATED: {
            int res = 0;
            void *thing_id = NULL;
            lite_cjson_t lite, lite_item_devid;

            if (payload == NULL) {
                return;
            }

            /* Parse Payload */
            memset(&lite, 0, sizeof(lite_cjson_t));
            res = lite_cjson_parse(payload, strlen(payload), &lite);
            if (res != SUCCESS_RETURN) {
                return;
            }

            /* Parse Id */
            memset(&lite_item_devid, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_SOLO_LEGACY_KEY_DEVID, strlen(LINKKIT_SOLO_LEGACY_KEY_DEVID),
                                         &lite_item_devid);
            if (res != SUCCESS_RETURN) {
                return;
            }
            dm_log_debug("Current Devid: %d", lite_item_devid.value_int);

            res = iotx_dm_deprecated_legacy_get_thingid_by_devid(lite_item_devid.value_int, &thing_id);
            if (res != SUCCESS_RETURN) {
                return;
            }

            if (linkkit_solo_ctx->user_callback->thing_create) {
                linkkit_solo_ctx->user_callback->thing_create(thing_id, linkkit_solo_ctx->user_context);
            }
        }
        break;
        case IOTX_DM_EVENT_THING_DISABLE: {
            int res = 0;
            void *thing_id = NULL;
            lite_cjson_t lite, lite_item_devid;

            if (payload == NULL) {
                return;
            }

            /* Parse Payload */
            memset(&lite, 0, sizeof(lite_cjson_t));
            res = lite_cjson_parse(payload, strlen(payload), &lite);
            if (res != SUCCESS_RETURN) {
                return;
            }

            /* Parse Id */
            memset(&lite_item_devid, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_SOLO_LEGACY_KEY_DEVID, strlen(LINKKIT_SOLO_LEGACY_KEY_DEVID),
                                         &lite_item_devid);
            if (res != SUCCESS_RETURN) {
                return;
            }
            dm_log_debug("Current Devid: %d", lite_item_devid.value_int);

            res = iotx_dm_deprecated_legacy_get_thingid_by_devid(lite_item_devid.value_int, &thing_id);
            if (res != SUCCESS_RETURN) {
                return;
            }

            if (linkkit_solo_ctx->user_callback->thing_disable) {
                linkkit_solo_ctx->user_callback->thing_disable(thing_id, linkkit_solo_ctx->user_context);
            }
        }
        break;
        case IOTX_DM_EVENT_THING_ENABLE: {
            int res = 0;
            void *thing_id = NULL;
            lite_cjson_t lite, lite_item_devid;

            if (payload == NULL) {
                return;
            }

            /* Parse Payload */
            memset(&lite, 0, sizeof(lite_cjson_t));
            res = lite_cjson_parse(payload, strlen(payload), &lite);
            if (res != SUCCESS_RETURN) {
                return;
            }

            /* Parse Id */
            memset(&lite_item_devid, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_SOLO_LEGACY_KEY_DEVID, strlen(LINKKIT_SOLO_LEGACY_KEY_DEVID),
                                         &lite_item_devid);
            if (res != SUCCESS_RETURN) {
                return;
            }
            dm_log_debug("Current Devid: %d", lite_item_devid.value_int);

            res = iotx_dm_deprecated_legacy_get_thingid_by_devid(lite_item_devid.value_int, &thing_id);
            if (res != SUCCESS_RETURN) {
                return;
            }

            if (linkkit_solo_ctx->user_callback->thing_enable) {
                linkkit_solo_ctx->user_callback->thing_enable(thing_id, linkkit_solo_ctx->user_context);
            }
        }
        break;
        case IOTX_DM_EVENT_PROPERTY_SET: {
            int res = 0;
            void *thing_id = NULL;
            char *propertyid = NULL;
            lite_cjson_t lite, lite_item_devid, lite_item_propertyid;

            if (payload == NULL) {
                return;
            }

            /* Parse Payload */
            memset(&lite, 0, sizeof(lite_cjson_t));
            res = lite_cjson_parse(payload, strlen(payload), &lite);
            if (res != SUCCESS_RETURN) {
                return;
            }

            /* Parse Devid */
            memset(&lite_item_devid, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_SOLO_LEGACY_KEY_DEVID, strlen(LINKKIT_SOLO_LEGACY_KEY_DEVID),
                                         &lite_item_devid);
            if (res != SUCCESS_RETURN) {
                return;
            }
            dm_log_debug("Current Devid: %d", lite_item_devid.value_int);

            /* Parse Property ID */
            memset(&lite_item_propertyid, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_SOLO_LEGACY_KEY_PROPERTYID, strlen(LINKKIT_SOLO_LEGACY_KEY_PROPERTYID),
                                         &lite_item_propertyid);
            if (res != SUCCESS_RETURN) {
                return;
            }
            dm_log_debug("Current PropertyID: %.*s", lite_item_propertyid.value_length, lite_item_propertyid.value);

            res = iotx_dm_deprecated_legacy_get_thingid_by_devid(lite_item_devid.value_int, &thing_id);
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

            if (linkkit_solo_ctx->user_callback->thing_prop_changed) {
                linkkit_solo_ctx->user_callback->thing_prop_changed(thing_id, propertyid, linkkit_solo_ctx->user_context);
            }

            DM_free(propertyid);
        }
        break;
        case IOTX_DM_EVENT_EVENT_PROPERTY_POST_REPLY: {
            int res = 0;
            void *thing_id = NULL;
            linkkit_solo_upstream_callback_node_t *node = NULL;
            handle_post_cb_fp_t callback = NULL;
            lite_cjson_t lite, lite_item_id, lite_item_code, lite_item_devid;

            if (payload == NULL) {
                return;
            }

            /* Parse Payload */
            memset(&lite, 0, sizeof(lite_cjson_t));
            res = lite_cjson_parse(payload, strlen(payload), &lite);
            if (res != SUCCESS_RETURN) {
                return;
            }

            /* Parse Id */
            memset(&lite_item_id, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_SOLO_LEGACY_KEY_ID, strlen(LINKKIT_SOLO_LEGACY_KEY_ID), &lite_item_id);
            if (res != SUCCESS_RETURN) {
                return;
            }
            dm_log_debug("Current Id: %d", lite_item_id.value_int);

            /* Parse Code */
            memset(&lite_item_code, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_SOLO_LEGACY_KEY_CODE, strlen(LINKKIT_SOLO_LEGACY_KEY_CODE),
                                         &lite_item_code);
            if (res != SUCCESS_RETURN) {
                return;
            }
            dm_log_debug("Current Code: %d", lite_item_code.value_int);

            /* Parse Devid */
            memset(&lite_item_devid, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_SOLO_LEGACY_KEY_DEVID, strlen(LINKKIT_SOLO_LEGACY_KEY_DEVID),
                                         &lite_item_devid);
            if (res != SUCCESS_RETURN) {
                return;
            }
            dm_log_debug("Current Devid: %d", lite_item_devid.value_int);

            res = iotx_dm_deprecated_legacy_get_thingid_by_devid(lite_item_devid.value_int, &thing_id);
            if (res != SUCCESS_RETURN) {
                return;
            }

            _linkkit_solo_upstream_mutex_lock();
            res = _linkkit_solo_upstream_callback_list_search(lite_item_id.value_int, &node);
            if (res == SUCCESS_RETURN) {
                callback = node->callback;
            }
            _linkkit_solo_upstream_mutex_unlock();
            if (res == SUCCESS_RETURN) {
                if (callback) {
                    callback(thing_id, lite_item_id.value_int, lite_item_code.value_int, NULL, linkkit_solo_ctx->user_context);
                }
                _linkkit_solo_upstream_mutex_lock();
                _linkkit_solo_upstream_callback_list_remove(lite_item_id.value_int);
                _linkkit_solo_upstream_mutex_unlock();
            }
        }
        break;
        case IOTX_DM_EVENT_EVENT_SPECIFIC_POST_REPLY: {
            int res = 0;
            void *thing_id = NULL;
            linkkit_solo_upstream_callback_node_t *node = NULL;
            handle_post_cb_fp_t callback = NULL;
            lite_cjson_t lite, lite_item_id, lite_item_code, lite_item_devid, lite_item_eventid;

            if (payload == NULL) {
                return;
            }

            /* Parse Payload */
            memset(&lite, 0, sizeof(lite_cjson_t));
            res = lite_cjson_parse(payload, strlen(payload), &lite);
            if (res != SUCCESS_RETURN) {
                return;
            }

            /* Parse Id */
            memset(&lite_item_id, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_SOLO_LEGACY_KEY_ID, strlen(LINKKIT_SOLO_LEGACY_KEY_ID), &lite_item_id);
            if (res != SUCCESS_RETURN) {
                return;
            }
            dm_log_debug("Current Id: %d", lite_item_id.value_int);

            /* Parse Code */
            memset(&lite_item_code, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_SOLO_LEGACY_KEY_CODE, strlen(LINKKIT_SOLO_LEGACY_KEY_CODE),
                                         &lite_item_code);
            if (res != SUCCESS_RETURN) {
                return;
            }
            dm_log_debug("Current Code: %d", lite_item_code.value_int);

            /* Parse Devid */
            memset(&lite_item_devid, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_SOLO_LEGACY_KEY_DEVID, strlen(LINKKIT_SOLO_LEGACY_KEY_DEVID),
                                         &lite_item_devid);
            if (res != SUCCESS_RETURN) {
                return;
            }
            dm_log_debug("Current Devid: %d", lite_item_devid.value_int);

            /* Parse Property ID */
            memset(&lite_item_eventid, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_SOLO_LEGACY_KEY_EVENTID, strlen(LINKKIT_SOLO_LEGACY_KEY_EVENTID),
                                         &lite_item_eventid);
            if (res != SUCCESS_RETURN) {
                return;
            }
            dm_log_debug("Current EventID: %.*s", lite_item_eventid.value_length, lite_item_eventid.value);

            res = iotx_dm_deprecated_legacy_get_thingid_by_devid(lite_item_devid.value_int, &thing_id);
            if (res != SUCCESS_RETURN) {
                return;
            }

            _linkkit_solo_upstream_mutex_lock();
            res = _linkkit_solo_upstream_callback_list_search(lite_item_id.value_int, &node);
            if (res == SUCCESS_RETURN) {
                callback = node->callback;
            }
            _linkkit_solo_upstream_mutex_unlock();
            if (res == SUCCESS_RETURN) {
                if (callback) {
                    callback(thing_id, lite_item_id.value_int, lite_item_code.value_int, NULL, linkkit_solo_ctx->user_context);
                }
                _linkkit_solo_upstream_mutex_lock();
                _linkkit_solo_upstream_callback_list_remove(lite_item_id.value_int);
                _linkkit_solo_upstream_mutex_unlock();
            }
        }
        break;
        case IOTX_DM_EVENT_COTA_NEW_CONFIG: {
            int res = 0;
            lite_cjson_t lite, lite_item_configid, lite_item_configsize, lite_item_gettype;
            lite_cjson_t lite_item_sign, lite_item_signmethod, lite_item_url;
            char *config_id = NULL, *get_type = NULL, *sign = NULL, *sign_method = NULL, *url = NULL;

            if (payload == NULL) {
                return;
            }

            /* Parse Payload */
            memset(&lite, 0, sizeof(lite_cjson_t));
            res = lite_cjson_parse(payload, strlen(payload), &lite);
            if (res != SUCCESS_RETURN || !lite_cjson_is_object(&lite)) {
                return;
            }

            /* Parse Config ID */
            memset(&lite_item_configid, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_SOLO_LEGACY_KEY_CONFIG_ID, strlen(LINKKIT_SOLO_LEGACY_KEY_CONFIG_ID),
                                         &lite_item_configid);
            if (res != SUCCESS_RETURN || !lite_cjson_is_string(&lite_item_configid)) {
                return;
            }
            dm_log_debug("Current Config ID: %.*s", lite_item_configid.value_length, lite_item_configid.value);

            /* Parse Config Size */
            memset(&lite_item_configsize, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_SOLO_LEGACY_KEY_CONFIG_SIZE, strlen(LINKKIT_SOLO_LEGACY_KEY_CONFIG_SIZE),
                                         &lite_item_configsize);
            if (res != SUCCESS_RETURN || !lite_cjson_is_number(&lite_item_configsize)) {
                return;
            }
            dm_log_debug("Current Config Size: %d", lite_item_configsize.value_int);

            /* Parse Get Type */
            memset(&lite_item_gettype, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_SOLO_LEGACY_KEY_GET_TYPE, strlen(LINKKIT_SOLO_LEGACY_KEY_GET_TYPE),
                                         &lite_item_gettype);
            if (res != SUCCESS_RETURN || !lite_cjson_is_string(&lite_item_gettype)) {
                return;
            }
            dm_log_debug("Current Get Type: %.*s", lite_item_gettype.value_length, lite_item_gettype.value);

            /* Parse Sign */
            memset(&lite_item_sign, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_SOLO_LEGACY_KEY_SIGN, strlen(LINKKIT_SOLO_LEGACY_KEY_SIGN),
                                         &lite_item_sign);
            if (res != SUCCESS_RETURN || !lite_cjson_is_string(&lite_item_sign)) {
                return;
            }
            dm_log_debug("Current Sign: %.*s", lite_item_sign.value_length, lite_item_sign.value);

            /* Parse Sign Method*/
            memset(&lite_item_signmethod, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_SOLO_LEGACY_KEY_SIGN_METHOD, strlen(LINKKIT_SOLO_LEGACY_KEY_SIGN_METHOD),
                                         &lite_item_signmethod);
            if (res != SUCCESS_RETURN || !lite_cjson_is_string(&lite_item_signmethod)) {
                return;
            }
            dm_log_debug("Current Sign Method: %.*s", lite_item_signmethod.value_length, lite_item_signmethod.value);

            /* Parse URL */
            memset(&lite_item_url, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_SOLO_LEGACY_KEY_URL, strlen(LINKKIT_SOLO_LEGACY_KEY_URL), &lite_item_url);
            if (res != SUCCESS_RETURN || !lite_cjson_is_string(&lite_item_url)) {
                return;
            }
            dm_log_debug("Current URL: %.*s", lite_item_url.value_length, lite_item_url.value);

            dm_utils_copy(lite_item_configid.value, lite_item_configid.value_length, (void **)&config_id,
                          lite_item_configid.value_length + 1);
            dm_utils_copy(lite_item_gettype.value, lite_item_gettype.value_length, (void **)&get_type,
                          lite_item_gettype.value_length + 1);
            dm_utils_copy(lite_item_sign.value, lite_item_sign.value_length, (void **)&sign, lite_item_sign.value_length + 1);
            dm_utils_copy(lite_item_signmethod.value, lite_item_signmethod.value_length, (void **)&sign_method,
                          lite_item_signmethod.value_length + 1);
            dm_utils_copy(lite_item_url.value, lite_item_url.value_length, (void **)&url, lite_item_url.value_length + 1);

            if (config_id == NULL || get_type == NULL || sign == NULL || sign_method == NULL || url == NULL) {
                dm_log_err(DM_UTILS_LOG_MEMORY_NOT_ENOUGH);
                if (config_id) {
                    DM_free(config_id);
                }
                if (get_type) {
                    DM_free(get_type);
                }
                if (sign) {
                    DM_free(sign);
                }
                if (sign_method) {
                    DM_free(sign_method);
                }
                if (url) {
                    DM_free(url);
                }
                return;
            }

            if (linkkit_solo_ctx->cota_callback) {
                linkkit_solo_ctx->cota_callback(service_cota_callback_type_new_version_detected, config_id,
                                                lite_item_configsize.value_int, get_type, sign, sign_method, url);
            }

            if (config_id) {
                DM_free(config_id);
            }
            if (get_type) {
                DM_free(get_type);
            }
            if (sign) {
                DM_free(sign);
            }
            if (sign_method) {
                DM_free(sign_method);
            }
            if (url) {
                DM_free(url);
            }
        }
        break;
        case IOTX_DM_EVENT_FOTA_NEW_FIRMWARE: {
            int res = 0;
            lite_cjson_t lite, lite_item_version;
            char *version = NULL;

            if (payload == NULL) {
                return;
            }

            /* Parse Payload */
            memset(&lite, 0, sizeof(lite_cjson_t));
            res = lite_cjson_parse(payload, strlen(payload), &lite);
            if (res != SUCCESS_RETURN || !lite_cjson_is_object(&lite)) {
                return;
            }

            /* Parse Version */
            memset(&lite_item_version, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_SOLO_LEGACY_KEY_VERSION, strlen(LINKKIT_SOLO_LEGACY_KEY_VERSION),
                                         &lite_item_version);
            if (res != SUCCESS_RETURN || !lite_cjson_is_string(&lite_item_version)) {
                return;
            }
            dm_log_debug("Current Firmware Version: %.*s", lite_item_version.value_length, lite_item_version.value);

            dm_utils_copy(lite_item_version.value, lite_item_version.value_length, (void **)&version,
                          lite_item_version.value_length + 1);
            if (version == NULL) {
                dm_log_err(DM_UTILS_LOG_MEMORY_NOT_ENOUGH);
                return;
            }

            if (linkkit_solo_ctx->fota_callback) {
                linkkit_solo_ctx->fota_callback(service_fota_callback_type_new_version_detected, version);
            }

            if (version) {
                DM_free(version);
            }
        }
        break;
#ifdef LOCAL_CONN_ENABLE
        case IOTX_DM_EVENT_LOCAL_CONNECTED: {
            if (linkkit_solo_ctx->on_connect) {
                linkkit_solo_ctx->on_connect(context, 0);
            }
        }
        break;
        case IOTX_DM_EVENT_LOCAL_DISCONNECT: {
            if (linkkit_solo_ctx->on_connect) {
                linkkit_solo_ctx->on_connect(context, 0);
            }
        }
        break;
#endif
        default:
            break;
    }
}
#endif

int deprecated linkkit_start(int max_buffered_msg, int get_tsl_from_cloud, linkkit_loglevel_t log_level,
                             linkkit_ops_t *ops,
                             linkkit_cloud_domain_type_t domain_type, void *user_context)
{
#ifdef DEPRECATED_LINKKIT
    int res = 0;
    linkkit_solo_legacy_ctx_t *linkkit_solo_ctx = _linkkit_solo_legacy_get_ctx();
    iotx_dm_init_params_t dm_init_params;

    if (linkkit_solo_ctx->is_started == 1) {
        return FAIL_RETURN;
    }
    linkkit_solo_ctx->is_started = 1;

    if (max_buffered_msg <= 0 || ops == NULL || log_level > LOG_DEBUG_LEVEL ||
        domain_type < 0 || domain_type >= linkkit_cloud_domain_max) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        linkkit_solo_ctx->is_started = 0;
        return FAIL_RETURN;
    }

    /* Create Mutex */
    linkkit_solo_ctx->mutex = HAL_MutexCreate();
    if (linkkit_solo_ctx->mutex == NULL) {
        dm_log_err(DM_UTILS_LOG_MEMORY_NOT_ENOUGH);
        linkkit_solo_ctx->is_started = 0;
        return FAIL_RETURN;
    }

    linkkit_solo_ctx->upstream_mutex = HAL_MutexCreate();
    if (linkkit_solo_ctx->upstream_mutex == NULL) {
        dm_log_err(DM_UTILS_LOG_MEMORY_NOT_ENOUGH);
        HAL_MutexDestroy(linkkit_solo_ctx->mutex);
        linkkit_solo_ctx->is_started = 0;
        return FAIL_RETURN;
    }

    /* Set Linkkit Log Level */
    LITE_set_loglevel(log_level);

    /* Initialize Device Manager */
    memset(&dm_init_params, 0, sizeof(iotx_dm_init_params_t));
    dm_init_params.secret_type = IOTX_DM_DEVICE_SECRET_DEVICE;
    dm_init_params.domain_type = domain_type;
    dm_init_params.event_callback = _linkkit_solo_event_callback;

    res = iotx_dm_deprecated_construct(&dm_init_params);
    if (res != SUCCESS_RETURN) {
        dm_log_err(DM_UTILS_LOG_DM_INIT_FAILED);
        HAL_MutexDestroy(linkkit_solo_ctx->mutex);
        HAL_MutexDestroy(linkkit_solo_ctx->upstream_mutex);
        linkkit_solo_ctx->is_started = 0;
        return FAIL_RETURN;
    }

    /* Get TSL From Cloud If Need */
    if (get_tsl_from_cloud != 0) {
        res = iotx_dm_deprecated_set_tsl(IOTX_DM_LOCAL_NODE_DEVID, IOTX_DM_TSL_SOURCE_CLOUD, NULL, 0);
        if (res < SUCCESS_RETURN) {
            HAL_MutexDestroy(linkkit_solo_ctx->mutex);
            HAL_MutexDestroy(linkkit_solo_ctx->upstream_mutex);
            linkkit_solo_ctx->is_started = 0;
            return FAIL_RETURN;
        }
    }

    /* Init Message Callback List */
    INIT_LIST_HEAD(&linkkit_solo_ctx->callback_list);

    linkkit_solo_ctx->user_callback = ops;
    linkkit_solo_ctx->user_context = user_context;
    linkkit_solo_ctx->is_leaved = 0;
#endif
    return SUCCESS_RETURN;
}

int deprecated linkkit_end(void)
{
#ifdef DEPRECATED_LINKKIT
    linkkit_solo_legacy_ctx_t *linkkit_solo_ctx = _linkkit_solo_legacy_get_ctx();

    if (linkkit_solo_ctx->is_started == 0) {
        return FAIL_RETURN;
    }

    _linkkit_solo_mutex_lock();
    _linkkit_solo_upstream_mutex_lock();
    linkkit_solo_ctx->is_started = 0;
    _linkkit_solo_upstream_callback_list_destroy();
    _linkkit_solo_upstream_mutex_unlock();

    iotx_dm_deprecated_destroy();
    _linkkit_solo_mutex_unlock();

    HAL_MutexDestroy(linkkit_solo_ctx->upstream_mutex);
    HAL_MutexDestroy(linkkit_solo_ctx->mutex);

    linkkit_solo_ctx->mutex = NULL;
    linkkit_solo_ctx->upstream_mutex = NULL;
    linkkit_solo_ctx->user_callback = NULL;
    linkkit_solo_ctx->user_context = NULL;
    linkkit_solo_ctx->cota_callback = NULL;
    linkkit_solo_ctx->fota_callback = NULL;
    INIT_LIST_HEAD(&linkkit_solo_ctx->callback_list);
#endif
    return SUCCESS_RETURN;
}

void *linkkit_set_tsl(const char *tsl, int tsl_len)
{
#ifdef DEPRECATED_LINKKIT
    int res = 0;
    void *thing_id = NULL;
    linkkit_solo_legacy_ctx_t *linkkit_solo_ctx = _linkkit_solo_legacy_get_ctx();

    if (tsl == NULL || tsl_len <= 0) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return NULL;
    }

    if (linkkit_solo_ctx->is_started == 0) {
        return NULL;
    }

    _linkkit_solo_mutex_lock();
    res = iotx_dm_deprecated_set_tsl(IOTX_DM_LOCAL_NODE_DEVID, IOTX_DM_TSL_SOURCE_LOCAL, tsl, tsl_len);
    if (res != SUCCESS_RETURN) {
        _linkkit_solo_mutex_unlock();
        return NULL;
    }

    res = iotx_dm_deprecated_legacy_get_thingid_by_devid(IOTX_DM_LOCAL_NODE_DEVID, &thing_id);
    if (res != SUCCESS_RETURN) {
        _linkkit_solo_mutex_unlock();
        return NULL;
    }

    _linkkit_solo_mutex_unlock();
    return thing_id;
#else
    return NULL;
#endif
}

int deprecated linkkit_set_value(linkkit_method_set_t method_set, const void *thing_id, const char *identifier,
                                 const void *value,
                                 const char *value_str)
{
#ifdef DEPRECATED_LINKKIT
    int res = 0, devid = 0;
    linkkit_solo_legacy_ctx_t *linkkit_solo_ctx = _linkkit_solo_legacy_get_ctx();

    if (thing_id == NULL || identifier == NULL || (value == NULL && value_str == NULL)) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    if (linkkit_solo_ctx->is_started == 0) {
        return FAIL_RETURN;
    }

    _linkkit_solo_mutex_lock();
    res = iotx_dm_deprecated_legacy_get_devid_by_thingid((void *)thing_id, &devid);
    if (res != SUCCESS_RETURN) {
        dm_log_err(DM_UTILS_LOG_GET_INVALID_DEV_ID);
        _linkkit_solo_mutex_unlock();
        return FAIL_RETURN;
    }

    switch (method_set) {
        case linkkit_method_set_property_value: {
            res = iotx_dm_deprecated_legacy_set_property_value(devid, (char *)identifier, strlen(identifier), (void *)value,
                    (char *)value_str);
            dm_log_info(DM_UTILS_LOG_OPERATOR_RES, 0, res);
        }
        break;
        case linkkit_method_set_event_output_value: {
            res = iotx_dm_deprecated_legacy_set_event_output_value(devid, (char *)identifier, strlen(identifier), (void *)value,
                    (char *)value_str);
            dm_log_info(DM_UTILS_LOG_OPERATOR_RES, 0, res);
        }
        break;
        case linkkit_method_set_service_output_value: {
            res = iotx_dm_deprecated_legacy_set_service_output_value(devid, (char *)identifier, strlen(identifier), (void *)value,
                    (char *)value_str);
            dm_log_info(DM_UTILS_LOG_OPERATOR_RES, 0, res);
        }
        break;
        default: {
            dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
            res = FAIL_RETURN;
        }
        break;
    }

    if (res < SUCCESS_RETURN) {
        _linkkit_solo_mutex_unlock();
        return FAIL_RETURN;
    }

    _linkkit_solo_mutex_unlock();
#endif
    return SUCCESS_RETURN;
}

int deprecated linkkit_get_value(linkkit_method_get_t method_get, const void *thing_id, const char *identifier,
                                 void *value,
                                 char **value_str)
{
#ifdef DEPRECATED_LINKKIT
    int res = 0, devid = 0;
    linkkit_solo_legacy_ctx_t *linkkit_solo_ctx = _linkkit_solo_legacy_get_ctx();

    if (thing_id == NULL || identifier == NULL || (value == NULL && value_str == NULL)) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    if (linkkit_solo_ctx->is_started == 0) {
        return FAIL_RETURN;
    }

    _linkkit_solo_mutex_lock();
    res = iotx_dm_deprecated_legacy_get_devid_by_thingid((void *)thing_id, &devid);
    if (res != SUCCESS_RETURN) {
        dm_log_err(DM_UTILS_LOG_GET_INVALID_DEV_ID);
        _linkkit_solo_mutex_unlock();
        return FAIL_RETURN;
    }

    switch (method_get) {
        case linkkit_method_get_property_value: {
            res = iotx_dm_deprecated_legacy_get_property_value(devid, (char *)identifier, strlen(identifier), value, value_str);
            dm_log_info(DM_UTILS_LOG_OPERATOR_RES, 1, res);
        }
        break;
        case linkkit_method_get_event_output_value: {
            res = iotx_dm_deprecated_legacy_get_event_output_value(devid, (char *)identifier, strlen(identifier), value, value_str);
            dm_log_info(DM_UTILS_LOG_OPERATOR_RES, 1, res);
        }
        break;
        case linkkit_method_get_service_input_value: {
            res = iotx_dm_deprecated_legacy_get_service_input_value(devid, (char *)identifier, strlen(identifier), value,
                    value_str);
            dm_log_info(DM_UTILS_LOG_OPERATOR_RES, 1, res);
        }
        break;
        case linkkit_method_get_service_output_value: {
            res = iotx_dm_deprecated_legacy_get_service_output_value(devid, (char *)identifier, strlen(identifier), value,
                    value_str);
            dm_log_info(DM_UTILS_LOG_OPERATOR_RES, 1, res);
        }
        break;
        default: {
            dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
            res = FAIL_RETURN;
        }
        break;
    }

    if (res < SUCCESS_RETURN) {
        _linkkit_solo_mutex_unlock();
        return FAIL_RETURN;
    }

    _linkkit_solo_mutex_unlock();
#endif
    return SUCCESS_RETURN;
}

int deprecated linkkit_answer_service(const void *thing_id, const char *service_identifier, int response_id, int code)
{
#ifdef DEPRECATED_LINKKIT
    int res = 0, devid = 0;

    linkkit_solo_legacy_ctx_t *linkkit_solo_ctx = _linkkit_solo_legacy_get_ctx();

    if (thing_id == NULL || service_identifier == NULL) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    if (linkkit_solo_ctx->is_started == 0) {
        return FAIL_RETURN;
    }

    _linkkit_solo_mutex_lock();
    res = iotx_dm_deprecated_legacy_get_devid_by_thingid((void *)thing_id, &devid);
    if (res != SUCCESS_RETURN) {
        dm_log_err(DM_UTILS_LOG_GET_INVALID_DEV_ID);
        _linkkit_solo_mutex_unlock();
        return FAIL_RETURN;
    }

    res = iotx_dm_deprecated_send_service_response(devid, response_id, (iotx_dm_error_code_t)code,
            (char *)service_identifier, strlen(service_identifier));
    if (res < SUCCESS_RETURN) {
        _linkkit_solo_mutex_unlock();
        return FAIL_RETURN;
    }

    _linkkit_solo_mutex_unlock();
#endif
    return SUCCESS_RETURN;
}

int deprecated linkkit_invoke_raw_service(const void *thing_id, int is_up_raw, void *raw_data, int raw_data_length)
{
#ifdef DEPRECATED_LINKKIT
    int res = 0, devid = 0;
    linkkit_solo_legacy_ctx_t *linkkit_solo_ctx = _linkkit_solo_legacy_get_ctx();

    if (thing_id == NULL || raw_data == NULL || raw_data_length <= 0) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    if (linkkit_solo_ctx->is_started == 0) {
        return FAIL_RETURN;
    }

    _linkkit_solo_mutex_lock();
    res = iotx_dm_deprecated_legacy_get_devid_by_thingid((void *)thing_id, &devid);
    if (res != SUCCESS_RETURN) {
        dm_log_err(DM_UTILS_LOG_GET_INVALID_DEV_ID);
        _linkkit_solo_mutex_unlock();
        return FAIL_RETURN;
    }

    res = iotx_dm_post_rawdata(devid, raw_data, raw_data_length);
    if (res < SUCCESS_RETURN) {
        _linkkit_solo_mutex_unlock();
        return FAIL_RETURN;
    }

    _linkkit_solo_mutex_unlock();
#endif
    return SUCCESS_RETURN;
}

int deprecated linkkit_trigger_extended_info_operate(const void *thing_id, const char *params,
        linkkit_extended_info_operate_t linkkit_extended_info_operation)
{
#ifdef DEPRECATED_LINKKIT
    int res = 0, devid = 0;
    linkkit_solo_legacy_ctx_t *linkkit_solo_ctx = _linkkit_solo_legacy_get_ctx();

    if (thing_id == NULL || params == NULL) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    if (linkkit_solo_ctx->is_started == 0) {
        return FAIL_RETURN;
    }

    _linkkit_solo_mutex_lock();
    res = iotx_dm_deprecated_legacy_get_devid_by_thingid((void *)thing_id, &devid);
    if (res != SUCCESS_RETURN) {
        dm_log_err(DM_UTILS_LOG_GET_INVALID_DEV_ID);
        _linkkit_solo_mutex_unlock();
        return FAIL_RETURN;
    }

    switch (linkkit_extended_info_operation) {
        case linkkit_extended_info_operate_update: {
            res = iotx_dm_deviceinfo_update(devid, (char *)params, strlen(params));
        }
        break;
        case linkkit_extended_info_operate_delete: {
            res = iotx_dm_deviceinfo_delete(devid, (char *)params, strlen(params));
        }
        break;
        default: {
            dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
            res = FAIL_RETURN;
        }
        break;
    }

    if (res < SUCCESS_RETURN) {
        _linkkit_solo_mutex_unlock();
        return FAIL_RETURN;
    }

    _linkkit_solo_mutex_unlock();
#endif
    return SUCCESS_RETURN;
}

int deprecated linkkit_trigger_event(const void *thing_id, const char *event_identifier, handle_post_cb_fp_t cb)
{
#ifdef DEPRECATED_LINKKIT
    int res = 0, devid = 0, msgid = 0, post_event_reply = 0;
    linkkit_solo_legacy_ctx_t *linkkit_solo_ctx = _linkkit_solo_legacy_get_ctx();

    if (thing_id == NULL || event_identifier == NULL) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    if (linkkit_solo_ctx->is_started == 0) {
        return FAIL_RETURN;
    }

    _linkkit_solo_mutex_lock();
    res = iotx_dm_deprecated_legacy_get_devid_by_thingid((void *)thing_id, &devid);
    if (res != SUCCESS_RETURN) {
        dm_log_err(DM_UTILS_LOG_GET_INVALID_DEV_ID);
        _linkkit_solo_mutex_unlock();
        return FAIL_RETURN;
    }

    res = iotx_dm_deprecated_post_event(devid, (char *)event_identifier, strlen((char *)event_identifier));
    if (res < SUCCESS_RETURN) {
        _linkkit_solo_mutex_unlock();
        return FAIL_RETURN;
    }
    msgid = res;

    res = iotx_dm_get_opt(linkkit_opt_event_post_reply, &post_event_reply);
    if (cb != NULL && post_event_reply) {
        /* Insert Message ID Into Linked List */
        _linkkit_solo_upstream_mutex_lock();
        res = _linkkit_solo_upstream_callback_list_insert(msgid, cb);
        _linkkit_solo_upstream_mutex_unlock();
        if (res != SUCCESS_RETURN) {
            dm_log_err(DM_UTILS_LOG_MEMORY_NOT_ENOUGH);
            _linkkit_solo_mutex_unlock();
            return FAIL_RETURN;
        }
    }

    _linkkit_solo_mutex_unlock();
#endif
    return SUCCESS_RETURN;
}

int deprecated linkkit_post_property(const void *thing_id, const char *property_identifier, handle_post_cb_fp_t cb)
{
#ifdef DEPRECATED_LINKKIT
    int res = 0, devid = 0, msgid = 0, property_identifier_len = 0, post_property_reply = 0;
    void *property_handle = NULL;
    linkkit_solo_legacy_ctx_t *linkkit_solo_ctx = _linkkit_solo_legacy_get_ctx();

    if (thing_id == NULL) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    if (linkkit_solo_ctx->is_started == 0) {
        return FAIL_RETURN;
    }

    _linkkit_solo_mutex_lock();
    res = iotx_dm_deprecated_legacy_get_devid_by_thingid((void *)thing_id, &devid);
    if (res != SUCCESS_RETURN) {
        dm_log_err(DM_UTILS_LOG_GET_INVALID_DEV_ID);
        _linkkit_solo_mutex_unlock();
        return FAIL_RETURN;
    }

    res = iotx_dm_deprecated_post_property_start(devid, &property_handle);
    if (res != SUCCESS_RETURN) {
        dm_log_err(DM_UTILS_LOG_MEMORY_NOT_ENOUGH);
        _linkkit_solo_mutex_unlock();
        return FAIL_RETURN;
    }

    property_identifier_len = (property_identifier) ? (strlen((char *)property_identifier)) : (0);
    res = iotx_dm_deprecated_post_property_add(property_handle, (char *)property_identifier, property_identifier_len);
    if (res != SUCCESS_RETURN) {
        iotx_dm_deprecated_post_property_end(&property_handle);
        _linkkit_solo_mutex_unlock();
        return FAIL_RETURN;
    }

    res = iotx_dm_deprecated_post_property_end(&property_handle);
    if (res < SUCCESS_RETURN) {
        _linkkit_solo_mutex_unlock();
        return FAIL_RETURN;
    }
    msgid = res;

    res = iotx_dm_get_opt(linkkit_opt_property_post_reply, &post_property_reply);
    if (cb != NULL && post_property_reply) {
        /* Insert Message ID Into Linked List */
        _linkkit_solo_upstream_mutex_lock();
        res = _linkkit_solo_upstream_callback_list_insert(msgid, cb);
        _linkkit_solo_upstream_mutex_unlock();
        if (res != SUCCESS_RETURN) {
            dm_log_err(DM_UTILS_LOG_MEMORY_NOT_ENOUGH);
            _linkkit_solo_mutex_unlock();
            return FAIL_RETURN;
        }
    }

    _linkkit_solo_mutex_unlock();
#endif
    return SUCCESS_RETURN;
}

int deprecated linkkit_yield(int timeout_ms)
{
#ifdef DEPRECATED_LINKKIT
    linkkit_solo_legacy_ctx_t *linkkit_solo_ctx = _linkkit_solo_legacy_get_ctx();

    if (timeout_ms <= 0) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    if (linkkit_solo_ctx->is_started == 0) {
        return FAIL_RETURN;
    }

    return iotx_dm_yield(timeout_ms);
#else
    return SUCCESS_RETURN;
#endif
}

int deprecated linkkit_cota_init(handle_service_cota_callback_fp_t callback_fp)
{
#ifdef DEPRECATED_LINKKIT
    linkkit_solo_legacy_ctx_t *linkkit_solo_ctx = _linkkit_solo_legacy_get_ctx();

    if (linkkit_solo_ctx->is_started == 0) {
        return FAIL_RETURN;
    }

    linkkit_solo_ctx->cota_callback = callback_fp;
#endif
    return SUCCESS_RETURN;
}

int deprecated linkkit_invoke_cota_service(void *data_buf, int data_buf_length)
{
#ifdef DEPRECATED_LINKKIT
    int res = 0;
    linkkit_solo_legacy_ctx_t *linkkit_solo_ctx = _linkkit_solo_legacy_get_ctx();

    if (data_buf == NULL || data_buf_length <= 0) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    if (linkkit_solo_ctx->is_started == 0) {
        return FAIL_RETURN;
    }

    _linkkit_solo_mutex_lock();
    res = iotx_dm_deprecated_cota_perform_sync(data_buf, data_buf_length);
    _linkkit_solo_mutex_unlock();

    return res;
#else
    return SUCCESS_RETURN;
#endif
}

int deprecated linkkit_invoke_cota_get_config(const char *config_scope, const char *get_type,
        const char *attribute_Keys,
        void *option)
{
#ifdef DEPRECATED_LINKKIT
    int res = 0;
    linkkit_solo_legacy_ctx_t *linkkit_solo_ctx = _linkkit_solo_legacy_get_ctx();

    if (config_scope == NULL || get_type == NULL || attribute_Keys == NULL) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    if (linkkit_solo_ctx->is_started == 0) {
        return FAIL_RETURN;
    }

    _linkkit_solo_mutex_lock();
    res = iotx_dm_deprecated_cota_get_config(config_scope, get_type, attribute_Keys);
    _linkkit_solo_mutex_unlock();

    return res;
#else
    return SUCCESS_RETURN;
#endif
}

int deprecated linkkit_fota_init(handle_service_fota_callback_fp_t callback_fp)
{
#ifdef DEPRECATED_LINKKIT
    linkkit_solo_legacy_ctx_t *linkkit_solo_ctx = _linkkit_solo_legacy_get_ctx();

    if (linkkit_solo_ctx->is_started == 0) {
        return FAIL_RETURN;
    }

    linkkit_solo_ctx->fota_callback = callback_fp;
#endif
    return SUCCESS_RETURN;
}

int deprecated linkkit_invoke_fota_service(void *data_buf, int data_buf_length)
{
#ifdef DEPRECATED_LINKKIT
    int res = 0;
    linkkit_solo_legacy_ctx_t *linkkit_solo_ctx = _linkkit_solo_legacy_get_ctx();

    if (data_buf == NULL || data_buf_length <= 0) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    if (linkkit_solo_ctx->is_started == 0) {
        return FAIL_RETURN;
    }

    _linkkit_solo_mutex_lock();
    res = iotx_dm_deprecated_fota_perform_sync(data_buf, data_buf_length);
    _linkkit_solo_mutex_unlock();

    return res;
#else
    return SUCCESS_RETURN;
#endif
}
