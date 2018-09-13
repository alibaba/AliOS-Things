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

    return SUCCESS_RETURN;
}

int dm_sub_multi(_IN_ char **subscribe, _IN_ int count)
{
    int res = 0;
    void *conn = dm_conn_get_cloud_conn();

    if (subscribe == NULL) {
        return DM_INVALID_PARAMETER;
    }

    dm_log_debug("Current Subscribe Topic: %s", *subscribe);

    res = dm_cmw_cloud_register(conn, subscribe, count, NULL);

    return res;
}

int dm_sub_multi_next(_IN_ dm_sub_multi_type_t type, _IN_ int devid, _IN_ int index)
{
    int res = 0;
    int search_index = 0, sub_index = 0, sub_count = 0, sub_number = 0;
    char product_key[PRODUCT_KEY_MAXLEN] = {0};
    char device_name[DEVICE_NAME_MAXLEN] = {0};
    char device_secret[DEVICE_SECRET_MAXLEN] = {0};
    char **subscribe = 0;
    dm_disp_topic_mapping_t *dcs_mapping = dm_disp_get_topic_mapping();
    int dcs_mapping_size = dm_disp_get_topic_mapping_size();
    void *conn = dm_conn_get_cloud_conn();

    if (devid < 0) {
        return DM_INVALID_PARAMETER;
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
        return DM_MEMORY_NOT_ENOUGH;
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
    dm_cmw_cloud_register(conn, subscribe, sub_count, NULL);

    for (sub_index = 0; sub_index < sub_count; sub_index++) {
        if (*(subscribe + sub_index)) {
            DM_free(*(subscribe + sub_index));
        }
    }

    DM_free(subscribe);

    if (type == DM_SUB_MULTI_TYPE_NEW) {
        dm_log_debug("Subscribe New Topic");
        dm_mgr_set_dev_sub_generic_index(devid, search_index);
        dm_mgr_dev_sub_ctl(DM_MGR_GET_SUB_NUMBER, devid, (void *)&sub_number);
        sub_number += sub_count;
        dm_mgr_dev_sub_ctl(DM_MGR_SET_SUB_NUMBER, devid, (void *)&sub_number);
    }

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

        DM_free(service_name);
    }

    return SUCCESS_RETURN;
}