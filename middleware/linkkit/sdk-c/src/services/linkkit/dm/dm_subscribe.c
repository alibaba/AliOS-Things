/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#include "iotx_dm_internal.h"
#include "dm_conn.h"
#include "dm_manager.h"
#include "dm_subscribe.h"
#include "dm_dispatch.h"
#include "dm_cm_wrapper.h"
#include "dm_opt.h"

static int _dm_sub_filter(int devid, int index, int unsub)
{
    int res = 0, dev_type = 0;
    char product_key[PRODUCT_KEY_MAXLEN] = {0};
    char device_name[DEVICE_NAME_MAXLEN] = {0};
    char device_secret[DEVICE_SECRET_MAXLEN] = {0};
    void *conn = dm_conn_get_cloud_conn();
    dm_disp_topic_mapping_t *dcs_mapping = dm_disp_get_topic_mapping();

    res = dm_mgr_search_device_by_devid(devid, product_key, device_name, device_secret);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    res = dm_mgr_get_dev_type(devid, &dev_type);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Check Device Type */
    if ((dcs_mapping[index].dev_type & dev_type) == 0) {
        return FAIL_RETURN;
    }
    dm_log_debug("Device Type Matched");

    /* Check Service Type */
    if ((dcs_mapping[index].service_type & IOTX_DM_SERVICE_CLOUD) == 0) {
        return FAIL_RETURN;
    }
    dm_log_debug("Service Type Matched");

    /* Check Property Post Reply Opt */
    int prop_post_reply_opt = 0;
    res = dm_opt_get(DM_OPT_DOWNSTREAM_PROPERTY_POST_REPLY, &prop_post_reply_opt);
    if (res == SUCCESS_RETURN) {
        if (strcmp(dcs_mapping[index].service_name, DM_DISP_THING_EVENT_PROPERTY_POST_REPLY) == 0) {
            if (prop_post_reply_opt == 0) {
                /*Unsubscribe This Topic*/
                if (unsub) {
                    char *unsubscribe = NULL;
                    res = dm_utils_service_name((char *)dcs_mapping[index].service_prefix,
                                                (char *)dcs_mapping[index].service_name, product_key, device_name, &unsubscribe);
                    if (res == SUCCESS_RETURN) {
                        dm_cmw_cloud_unregister(conn, unsubscribe);
                        DM_free(unsubscribe);
                    }
                }
                return FAIL_RETURN;
            }
        }
    }

#ifndef DEPRECATED_LINKKIT
    /* Check Event Post Reply Opt */
    int event_post_reply_opt = 0;
    res = dm_opt_get(DM_OPT_DOWNSTREAM_EVENT_POST_REPLY, &event_post_reply_opt);
    if (res == SUCCESS_RETURN) {
        if (strcmp(dcs_mapping[index].service_name, DM_DISP_THING_EVENT_POST_REPLY_WILDCARD) == 0) {
            if (event_post_reply_opt == 0) {
                /*Unsubscribe This Topic*/
                if (unsub) {
                    char *unsubscribe = NULL;
                    res = dm_utils_service_name((char *)dcs_mapping[index].service_prefix,
                                                (char *)dcs_mapping[index].service_name, product_key, device_name, &unsubscribe);
                    if (res == SUCCESS_RETURN) {
                        dm_cmw_cloud_unregister(conn, unsubscribe);
                        DM_free(unsubscribe);
                    }
                }
                return FAIL_RETURN;
            }
        }
    }
#endif

    return SUCCESS_RETURN;
}

int dm_sub_multi(_IN_ char **subscribe, _IN_ int count)
{
    int res = 0;
    void *conn = dm_conn_get_cloud_conn();

    if (subscribe == NULL) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    dm_log_debug("Current Subscribe Topic: %s", *subscribe);

    res = dm_cmw_cloud_register(conn, subscribe, count, NULL);
    if (res == FAIL_RETURN) {
        dm_log_warning(DM_UTILS_LOG_DMGR_SERVICE_CLOUD_REGISTER_FAILED, strlen(*subscribe), *subscribe);
    }

    return res;
}

int dm_sub_multi_next(_IN_ int devid, _IN_ int index)
{
    int res = 0;
    int search_index = 0, sub_index = 0, sub_count = 0;
    char product_key[PRODUCT_KEY_MAXLEN] = {0};
    char device_name[DEVICE_NAME_MAXLEN] = {0};
    char device_secret[DEVICE_SECRET_MAXLEN] = {0};
    char **subscribe = 0;
    dm_disp_topic_mapping_t *dcs_mapping = dm_disp_get_topic_mapping();
    int dcs_mapping_size = dm_disp_get_topic_mapping_size();
    void *conn = dm_conn_get_cloud_conn();

    if (devid < 0) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    /* Search Device Product Key And Device Name */
    res = dm_mgr_search_device_by_devid(devid, product_key, device_name, device_secret);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Find Out How Many Topic Will Be Subscribe This Time */
    for (search_index = index; search_index < dcs_mapping_size; search_index++) {
        if (_dm_sub_filter(devid, search_index, 1) == SUCCESS_RETURN) {
            sub_count++;
            if (sub_count == DM_DISP_MULTI_SUBSCRIBE_MAX) {
                break;
            }
        }
    }

    dm_log_debug("Subscribe Search, Count: %d", sub_count);

    /* Check If There is Any Topic Need To Be Subscribe */
    if (search_index == dcs_mapping_size && sub_count == 0) {
        /* No More Topic Need To Be Subscribed, Return DCS Mapping Size */
        return search_index;
    }

    /* Malloc Memory For Multi-Subscribe */
    subscribe = DM_malloc(sub_count * sizeof(char *));
    if (subscribe == NULL) {
        dm_log_err(DM_UTILS_LOG_MEMORY_NOT_ENOUGH);
        return FAIL_RETURN;
    }
    memset(subscribe, 0, sub_count * sizeof(char *));

    /* Find Out Topics Which Will Be Subscribe */
    for (search_index = index, sub_index = 0; search_index < dcs_mapping_size; search_index++) {
        if (_dm_sub_filter(devid, search_index, 0) == SUCCESS_RETURN) {
            res = dm_utils_service_name((char *)dcs_mapping[search_index].service_prefix,
                                        (char *)dcs_mapping[search_index].service_name, product_key, device_name, subscribe + sub_index);
            if (res != SUCCESS_RETURN) {
                goto ERROR;
            }
            sub_index++;
            if (sub_index >= sub_count) {
                break;
            }
        }
    }
    dm_log_debug("Subscribe Topic Ready, Count: %d", sub_count);

    /* Multi-Subscribe Topic */
    res = dm_cmw_cloud_register(conn, subscribe, sub_count, NULL);
    if (res == FAIL_RETURN) {
        dm_log_warning(DM_UTILS_LOG_DMGR_SERVICE_CLOUD_REGISTER_FAILED, strlen(*(subscribe)), *(subscribe));
    }

    for (sub_index = 0; sub_index < sub_count; sub_index++) {
        if (*(subscribe + sub_index)) {
            DM_free(*(subscribe + sub_index));
        }
    }

    DM_free(subscribe);

    dm_mgr_set_dev_sub_generic_index(devid, search_index);

    return SUCCESS_RETURN;
ERROR:
    for (sub_index = 0; sub_index < sub_count; sub_index++) {
        if (*(subscribe + sub_index)) {
            DM_free(*(subscribe + sub_index));
        }
    }
    DM_free(subscribe);
    return FAIL_RETURN;
}

int dm_sub_shadow_destroy(int devid)
{
    return dm_mgr_clear_dev_sub_service_event(devid);
}

int dm_sub_shadow_next(int devid, int index)
{
    int res = 0;
    char *service_event = NULL;
    void *conn = dm_conn_get_cloud_conn();
    void *local_conn = dm_conn_get_local_conn();

    if (devid < 0) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = dm_mgr_get_dev_sub_service_event(devid, index, &service_event);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Subscribe Cloud Service */
    dm_log_debug("Current Subscribe Topic: %s", service_event);
    res = dm_cmw_cloud_register(conn, &service_event, 1, NULL);
    if (res == FAIL_RETURN) {
        dm_log_warning(DM_UTILS_LOG_DMGR_SERVICE_CLOUD_REGISTER_FAILED, strlen(service_event), service_event);
    }

    /* Regiter Resource To Local */
    dm_cmw_local_add_service(local_conn, service_event, IOTX_DM_MESSAGE_AUTH, NULL);

    DM_free(service_event);

    dm_mgr_set_dev_sub_service_event_index(devid, index);

    return SUCCESS_RETURN;
}

int dm_sub_local_register(void)
{
    int res = 0, index = 0;
    iotx_dm_message_auth_types_t service_auth;
    char product_key[PRODUCT_KEY_MAXLEN] = {0};
    char device_name[DEVICE_NAME_MAXLEN] = {0};
    char *service_name = NULL;
    void *conn = dm_conn_get_local_conn();
    dm_disp_topic_mapping_t *dcs_topic_mapping = dm_disp_get_topic_mapping();

    HAL_GetProductKey(product_key);
    HAL_GetDeviceName(device_name);

    for (index = 0; index < dm_disp_get_topic_mapping_size(); index++) {
        service_name = NULL;
        if (!(dcs_topic_mapping[index].service_type & IOTX_DM_SERVICE_LOCAL)) {
            continue;
        }
        service_auth = (dcs_topic_mapping[index].service_type & IOTX_DM_SERVICE_LOCAL_AUTH) ? (IOTX_DM_MESSAGE_AUTH) :
                       (IOTX_DM_MESSAGE_NO_AUTH);
        res = dm_utils_service_name((char *)dcs_topic_mapping[index].service_prefix,
                                    (char *)dcs_topic_mapping[index].service_name, product_key, device_name, &service_name);
        if (res != SUCCESS_RETURN) {
            return FAIL_RETURN;
        }

        /* Subscribe Cloud Service */
        res = dm_cmw_local_add_service(conn, service_name, service_auth, NULL);
        if (res == FAIL_RETURN) {
            dm_log_warning(DM_UTILS_LOG_DMGR_SERVICE_LOCAL_REGISTER_FAILED, strlen(service_name), service_name);
        }
        DM_free(service_name);
    }

    return SUCCESS_RETURN;
}

#ifdef DEPRECATED_LINKKIT
static int _dm_sub_deprecated_shadow_service_filter(int devid, char *method, int unsub)
{
    return SUCCESS_RETURN;
}

static int _dm_sub_deprecated_shadow_event_filter(int devid, char *method, int unsub)
{
    int res = 0;
    char product_key[PRODUCT_KEY_MAXLEN] = {0};
    char device_name[DEVICE_NAME_MAXLEN] = {0};
    char device_secret[DEVICE_SECRET_MAXLEN] = {0};
    void *conn = dm_conn_get_cloud_conn();

    res = dm_mgr_search_device_by_devid(devid, product_key, device_name, device_secret);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Check Event Post Reply Opt */
    int event_post_reply_opt = 0;
    res = dm_opt_get(DM_OPT_DOWNSTREAM_EVENT_POST_REPLY, &event_post_reply_opt);
    if (res == SUCCESS_RETURN) {
        if (event_post_reply_opt == 0) {
            if (unsub) {
                char *method_reply = NULL, *unsubscribe = NULL;

                method_reply = DM_malloc(strlen(method) + strlen(DM_DISP_REPLY_SUFFIX) + 1);
                if (method_reply == NULL) {
                    DM_free(method);
                    dm_log_err(DM_UTILS_LOG_MEMORY_NOT_ENOUGH);
                    return FAIL_RETURN;
                }
                memset(method_reply, 0, strlen(method) + strlen(DM_DISP_REPLY_SUFFIX) + 1);
                memcpy(method_reply, method, strlen(method));
                memcpy(method_reply + strlen(method_reply), DM_DISP_REPLY_SUFFIX, strlen(DM_DISP_REPLY_SUFFIX));

                res = dm_utils_service_name((char *)DM_DISP_SYS_PREFIX, method_reply, product_key, device_name, &unsubscribe);
                DM_free(method_reply);
                if (res == SUCCESS_RETURN) {
                    dm_cmw_cloud_unregister(conn, unsubscribe);
                    DM_free(unsubscribe);
                }
            }
            return FAIL_RETURN;
        }
    }

    return SUCCESS_RETURN;
}

int dm_sub_deprecated_shadow_create(int devid)
{
    int res = 0, service_number = 0, event_number = 0;
    int index = 0, service_event_index = 0, service_count = 0, event_count = 0;
    char product_key[PRODUCT_KEY_MAXLEN] = {0};
    char device_name[DEVICE_NAME_MAXLEN] = {0};
    char device_secret[DEVICE_SECRET_MAXLEN] = {0};
    void *service = NULL, *event = NULL;
    char *method = NULL, *method_reply = NULL;
    char **subscribe = NULL;

    if (devid < 0) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = dm_mgr_search_device_by_devid(devid, product_key, device_name, device_secret);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    res = dm_mgr_deprecated_get_service_number(devid, &service_number);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    res = dm_mgr_deprecated_get_event_number(devid, &event_number);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Find Out Topic Which Need To Be Subscribe */
    for (index = 0; index < service_number; index++) {
        service = NULL;
        method = NULL;
        res  = dm_mgr_deprecated_get_service_by_index(devid, index, &service);
        if (res != SUCCESS_RETURN) {
            continue;
        }

        res = dm_mgr_deprecated_get_service_method(service, &method);
        if (res != SUCCESS_RETURN) {
            continue;
        }

        res = dm_utils_replace_char(method, strlen(method), '.', '/');
        if (res != SUCCESS_RETURN) {
            DM_free(method);
            return FAIL_RETURN;
        }

        res = _dm_sub_deprecated_shadow_service_filter(devid, method, 1);
        if (res != SUCCESS_RETURN) {
            DM_free(method);
            continue;
        }

        service_count++;
        DM_free(method);
    }

    for (index = 0; index < event_number; index++) {
        event = NULL;
        method = NULL;
        res  = dm_mgr_deprecated_get_event_by_index(devid, index, &event);
        if (res != SUCCESS_RETURN) {
            continue;
        }

        res = dm_mgr_deprecated_get_event_method(event, &method);
        if (res != SUCCESS_RETURN) {
            continue;
        }

        res = dm_utils_replace_char(method, strlen(method), '.', '/');
        if (res != SUCCESS_RETURN) {
            DM_free(method);
            return FAIL_RETURN;
        }

        res = _dm_sub_deprecated_shadow_event_filter(devid, method, 1);
        if (res != SUCCESS_RETURN) {
            DM_free(method);
            continue;
        }

        event_count++;
        DM_free(method);
    }

    if (service_count + event_count == 0) {
        return SUCCESS_RETURN;
    }

    /* Allocate Memory For Service And Event Topic*/
    subscribe = DM_malloc((service_count + event_count) * sizeof(char *));
    if (subscribe == NULL) {
        dm_log_err(DM_UTILS_LOG_MEMORY_NOT_ENOUGH);
        return FAIL_RETURN;
    }
    memset(subscribe, 0, (service_count + event_count)*sizeof(char *));

    for (index = 0; index < service_number; index++) {
        service = NULL;
        method = NULL;
        res  = dm_mgr_deprecated_get_service_by_index(devid, index, &service);
        if (res != SUCCESS_RETURN) {
            continue;
        }

        res = dm_mgr_deprecated_get_service_method(service, &method);
        if (res != SUCCESS_RETURN) {
            continue;
        }

        res = dm_utils_replace_char(method, strlen(method), '.', '/');
        if (res != SUCCESS_RETURN) {
            DM_free(method);
            return FAIL_RETURN;
        }

        res = _dm_sub_deprecated_shadow_service_filter(devid, method, 0);
        if (res != SUCCESS_RETURN) {
            DM_free(method);
            continue;
        }

        res = dm_utils_service_name((char *)DM_DISP_SYS_PREFIX, method, product_key, device_name,
                                    (subscribe + service_event_index));
        if (res != SUCCESS_RETURN) {
            DM_free(method);
            return FAIL_RETURN;
        }

        dm_log_debug("Current Service Event Generate: %s", *(subscribe + service_event_index));

        DM_free(method);
        service_event_index++;
    }

    for (index = 0; index < event_number; index++) {
        event = NULL;
        method = NULL, method_reply = NULL;
        res  = dm_mgr_deprecated_get_event_by_index(devid, index, &event);
        if (res != SUCCESS_RETURN) {
            continue;
        }

        res = dm_mgr_deprecated_get_event_method(event, &method);
        if (res != SUCCESS_RETURN) {
            continue;
        }

        res = dm_utils_replace_char(method, strlen(method), '.', '/');
        if (res != SUCCESS_RETURN) {
            DM_free(method);
            return FAIL_RETURN;
        }

        res = _dm_sub_deprecated_shadow_event_filter(devid, method, 0);
        if (res != SUCCESS_RETURN) {
            DM_free(method);
            continue;
        }

        method_reply = DM_malloc(strlen(method) + strlen(DM_DISP_REPLY_SUFFIX) + 1);
        if (method_reply == NULL) {
            DM_free(method);
            dm_log_err(DM_UTILS_LOG_MEMORY_NOT_ENOUGH);
            continue;
        }
        memset(method_reply, 0, strlen(method) + strlen(DM_DISP_REPLY_SUFFIX) + 1);
        memcpy(method_reply, method, strlen(method));
        memcpy(method_reply + strlen(method_reply), DM_DISP_REPLY_SUFFIX, strlen(DM_DISP_REPLY_SUFFIX));

        res = dm_utils_service_name((char *)DM_DISP_SYS_PREFIX, method_reply, product_key, device_name,
                                    (subscribe + service_event_index));
        if (res != SUCCESS_RETURN) {
            DM_free(method);
            return FAIL_RETURN;
        }

        dm_log_debug("Current Service Event Generate: %s", *(subscribe + service_event_index));

        DM_free(method);
        DM_free(method_reply);
        service_event_index++;
    }

    res = dm_mgr_set_dev_sub_service_event(devid, (service_count + event_count), subscribe);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    res = dm_mgr_get_dev_sub_generic_index(devid, &index);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    if (index == DM_MGR_DEV_SUB_END) {
        dm_log_debug("Current Device %d Has Already Subscribe All Generic Topic");
        dm_sub_shadow_next(devid, 0);
    }
    dm_mgr_set_dev_sub_service_event_index(devid, DM_MGR_DEV_SUB_START);

    return SUCCESS_RETURN;
}
#endif