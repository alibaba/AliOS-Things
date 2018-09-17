/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#include "iotx_dm_internal.h"
#include "dm_cm_wrapper.h"
#include "dm_dispatch.h"
#include "dm_manager.h"
#include "dm_conn.h"
#include "iotx_cm.h"

void dm_cmw_topic_callback(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data)
{
    int res = 0, index = 0;
    dm_disp_topic_mapping_t *dcs_mapping = dm_disp_get_topic_mapping();

    dm_log_info("DMGR TOPIC CALLBACK");

    if (source == NULL || msg == NULL || msg->URI == NULL) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return;
    }

    dm_log_info("DMGR Receive Message: %s", (msg->URI == NULL) ? ("NULL") : (msg->URI));

    int prefix_end = 0, prefix_uri_end = 0;
    res = dm_disp_uri_prefix_sys_split(msg->URI, msg->URI_length, &prefix_end, &prefix_uri_end);
    if (res == SUCCESS_RETURN) {
        /* URI Start With /sys/ */
        dm_log_debug("Current URI Without /sys: %.*s", prefix_uri_end + 1, msg->URI + prefix_end);

        int pkdn_end = 0, pkdn_uri_end = 0;
        res = dm_disp_uri_pkdn_split(msg->URI + prefix_end, prefix_uri_end + 1, &pkdn_end, &pkdn_uri_end);
        if (res == SUCCESS_RETURN) {
            /* pkdn_end At /sys/%s/%s/ */
            dm_log_debug("Current URI Without /sys/pk/dn/: %.*s", pkdn_uri_end, msg->URI + prefix_end + pkdn_end + 1);
            for (index = 0; index < dm_disp_get_topic_mapping_size(); index++) {
                if ((strlen(dcs_mapping[index].service_name) == pkdn_uri_end) &&
                    (memcmp(dcs_mapping[index].service_name, msg->URI + prefix_end + pkdn_end + 1, pkdn_uri_end) == 0) &&
                    (memcmp(dcs_mapping[index].service_prefix, DM_DISP_SYS_PREFIX, strlen(DM_DISP_SYS_PREFIX)) == 0)) {
                    dcs_mapping[index].service_handler(source, msg, user_data);
                    return;
                }
            }

            int identifier_start = 0, identifier_end = 0;

            /* Check URI Match /sys/pk/dn/thing/service/{service_id} */
            res = dm_disp_uri_service_specific_split(msg->URI + prefix_end + pkdn_end, pkdn_uri_end + 1, &identifier_start,
                    &identifier_end);
            if (res == SUCCESS_RETURN) {
                dm_log_debug("identifier_start: %d, identifier_end: %d", identifier_start, identifier_end);
                dm_log_debug("Current Service Identifier: %.*s", identifier_end - identifier_start,
                             msg->URI + prefix_end + pkdn_end + identifier_start + 1);
                dm_disp_thing_service_request(source, msg, msg->URI + prefix_end + pkdn_end + identifier_start + 1,
                                              identifier_end - identifier_start, user_data);
                return;
            }

            identifier_start = 0, identifier_end = 0;
            /* Check URI Match /sys/pk/dn/thing/event/{event_id}/post_reply */
            res = dm_disp_uri_event_specific_split(msg->URI + prefix_end + pkdn_end, pkdn_uri_end + 1, &identifier_start,
                                                   &identifier_end);
            if (res == SUCCESS_RETURN) {
                dm_log_debug("identifier_start: %d, identifier_end: %d", identifier_start, identifier_end);
                dm_log_debug("Current Event Identifier: %.*s", identifier_end - identifier_start - 1,
                             msg->URI + prefix_end + pkdn_end + identifier_start + 1);
                dm_disp_thing_event_post_reply(source, msg, msg->URI + prefix_end + pkdn_end + identifier_start + 1,
                                               identifier_end - identifier_start - 1, user_data);
                return;
            }

            identifier_start = 0, identifier_end = 0;
            /* Check URI Match /sys/pk/dn/rrpc/request/{message_id}} */
            res = dm_disp_uri_rrpc_request_split(msg->URI + prefix_end + pkdn_end, pkdn_uri_end + 1, &identifier_start,
                                                 &identifier_end);
            if (res == SUCCESS_RETURN) {
                dm_log_debug("identifier_start: %d, identifier_end: %d", identifier_start, identifier_end);
                dm_log_debug("Current RRPC Request Message ID: %.*s", identifier_end - identifier_start,
                             msg->URI + prefix_end + pkdn_end + identifier_start + 1);
                dm_disp_rrpc_request(source, msg, msg->URI + prefix_end + pkdn_end + identifier_start + 1,
                                     identifier_end - identifier_start, user_data);
                return;
            }
        }
    }

    prefix_end = 0, prefix_uri_end = 0;
    res = dm_disp_uri_prefix_ext_session_split(msg->URI, msg->URI_length, &prefix_end, &prefix_uri_end);
    if (res == SUCCESS_RETURN) {
        /* URI Start With /ext/session/ */
        dm_log_debug("Current URI Without /ext/session: %.*s", prefix_uri_end + 1, msg->URI + prefix_end);
        int pkdn_end = 0, pkdn_uri_end = 0;
        res = dm_disp_uri_pkdn_split(msg->URI + prefix_end, prefix_uri_end + 1, &pkdn_end, &pkdn_uri_end);
        if (res == SUCCESS_RETURN) {
            /* pkdn_end At /sys/%s/%s/ */
            dm_log_debug("Current URI Without /sys/pk/dn/: %.*s", pkdn_uri_end, msg->URI + prefix_end + pkdn_end + 1);
            for (index = 0; index < dm_disp_get_topic_mapping_size(); index++) {
                if ((strlen(dcs_mapping[index].service_name) == pkdn_uri_end) &&
                    (memcmp(dcs_mapping[index].service_name, msg->URI + prefix_end + pkdn_end + 1, pkdn_uri_end) == 0) &&
                    (memcmp(dcs_mapping[index].service_prefix, DM_DISP_EXT_SESSION_PREFIX, strlen(DM_DISP_EXT_SESSION_PREFIX)) == 0)) {
                    dcs_mapping[index].service_handler(source, msg, user_data);
                    return;
                }
            }
        }
    }

    prefix_end = 0, prefix_uri_end = 0;
    res = dm_disp_uri_prefix_ext_ntp_split(msg->URI, msg->URI_length, &prefix_end, &prefix_uri_end);
    if (res == SUCCESS_RETURN) {
        /* URI Start With /ext/ntp/ */
        dm_log_debug("Current URI Without /ext/ntp: %.*s", prefix_uri_end + 1, msg->URI + prefix_end);
        int pkdn_end = 0, pkdn_uri_end = 0;
        res = dm_disp_uri_pkdn_split(msg->URI + prefix_end, prefix_uri_end + 1, &pkdn_end, &pkdn_uri_end);
        if (res == SUCCESS_RETURN) {
            /* pkdn_end At /sys/%s/%s/ */
            dm_log_debug("Current URI Without /ext/ntp/: %.*s", pkdn_uri_end, msg->URI + prefix_end + pkdn_end + 1);
            for (index = 0; index < dm_disp_get_topic_mapping_size(); index++) {
                if ((strlen(dcs_mapping[index].service_name) == pkdn_uri_end) &&
                    (memcmp(dcs_mapping[index].service_name, msg->URI + prefix_end + pkdn_end + 1, pkdn_uri_end) == 0) &&
                    (memcmp(dcs_mapping[index].service_prefix, DM_DISP_EXT_NTP_PREFIX, strlen(DM_DISP_EXT_NTP_PREFIX)) == 0)) {
                    dcs_mapping[index].service_handler(source, msg, user_data);
                    return;
                }
            }
        }
    }

    prefix_end = 0, prefix_uri_end = 0;
    res = dm_disp_uri_prefix_ext_error_split(msg->URI, msg->URI_length, &prefix_end, &prefix_uri_end);
    if (res == SUCCESS_RETURN) {
        /* URI Start with /ext/error */
        dm_log_debug("Current URI Without /ext/error: %.*s", prefix_uri_end + 1, msg->URI + prefix_end);

        dm_disp_ext_error_response(source, msg, user_data);
    }


    for (index = 0; index < dm_disp_get_topic_mapping_size(); index++) {
        if ((strlen(dcs_mapping[index].service_name) == msg->URI_length) &&
            (memcmp(dcs_mapping[index].service_name, msg->URI, msg->URI_length) == 0)) {
            dcs_mapping[index].service_handler(source, msg, user_data);
            return;
        }
    }

    /* TSL Topic Or User Dynamic Add Topic */
    dm_disp_user_defined_handler(source, msg, user_data);

}

void dm_cmw_event_callback(void *pcontext, iotx_cm_event_msg_t *msg, void *user_data)
{
    dm_disp_event_mapping_t *dcw_event_mapping = dm_disp_get_event_mapping();

    if (msg == NULL) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return;
    }

    if (dcw_event_mapping[msg->event_id].handler == NULL) {
        dm_log_err(DM_UTILS_LOG_CM_EVENT_UNKNOWN, msg->event_id);
        return;
    }

    dcw_event_mapping[msg->event_id].handler(pcontext, msg, user_data);
}

int dm_cmw_init(iotx_dm_device_secret_types_t secret_type, iotx_dm_cloud_domain_types_t domain_type)
{
    iotx_cm_init_param_t cm_init_param;

    memset(&cm_init_param, 0, sizeof(iotx_cm_init_param_t));
    cm_init_param.secret_type = (iotx_cm_device_secret_types_t)secret_type;
    cm_init_param.domain_type = (iotx_cm_cloud_domain_types_t)domain_type;
    cm_init_param.event_func = dm_cmw_event_callback;
    cm_init_param.user_data = NULL;

    return iotx_cm_init(&cm_init_param, NULL);
}

int dm_cmw_deinit(void)
{
    return iotx_cm_deinit(NULL);
}

int dm_cmw_yield(int timeout_ms)
{
#if (CONFIG_SDK_THREAD_COST == 0)
    return iotx_cm_yield(timeout_ms, NULL);
#else
    return FAIL_RETURN;
#endif
}

static int _dm_cmw_conn_cloud_mqtt_create(void **conn_handle)
{
    iotx_cm_connectivity_param_t cm_connectivity_param;
    iotx_cm_connectivity_cloud_param_t cm_connectivity_cloud_param;

    memset(&cm_connectivity_cloud_param, 0, sizeof(iotx_cm_connectivity_cloud_param_t));
    cm_connectivity_cloud_param.protocol_type = IOTX_CM_CONNECTION_PROTOCOL_TYPE_MQTT;

    memset(&cm_connectivity_param, 0, sizeof(iotx_cm_connectivity_param_t));
    cm_connectivity_param.connectivity_type = IOTX_CM_CONNECTIVITY_TYPE_CLOUD;
    cm_connectivity_param.cloud_param = &cm_connectivity_cloud_param;

    *conn_handle = iotx_cm_open(&cm_connectivity_param, NULL);
    if (*conn_handle == NULL) {
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

static int _dm_cmw_conn_local_alcs_create(void **conn_handle)
{
    iotx_cm_connectivity_param_t cm_connectivity_param;
    iotx_cm_connectivity_alcs_param_t cm_connectivity_alcs_param;

    if (conn_handle == NULL || *conn_handle != NULL) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    memset(&cm_connectivity_alcs_param, 0, sizeof(iotx_cm_connectivity_alcs_param_t));
    cm_connectivity_alcs_param.protocol_type = IOTX_CM_CONNECTION_PROTOCOL_TYPE_ALCS;

    memset(&cm_connectivity_param, 0, sizeof(iotx_cm_connectivity_param_t));
    cm_connectivity_param.connectivity_type = IOTX_CM_CONNECTIVITY_TYPE_LOCAL;
    cm_connectivity_param.alcs_param = &cm_connectivity_alcs_param;

    *conn_handle = iotx_cm_open(&cm_connectivity_param, NULL);
    if (*conn_handle == NULL) {
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

static int _dm_cmw_conn_connect(void *conn_handle)
{
    if (conn_handle == NULL) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    return iotx_cm_connect(conn_handle, NULL);
}

int dm_cmw_conn_cloud_mqtt_init(void **conn_handle)
{
    int res = 0;

    if (conn_handle == NULL || *conn_handle != NULL) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = _dm_cmw_conn_cloud_mqtt_create(conn_handle);
    if (res != SUCCESS_RETURN) {
        dm_log_warning(DM_UTILS_LOG_CM_CLOUD_CONNECTIVITY_CREATE_FAILED);
        return ERROR_NO_MEM;
    }

    res = _dm_cmw_conn_connect(*conn_handle);
    if (res != SUCCESS_RETURN) {
        dm_log_warning(DM_UTILS_LOG_CM_CLOUD_CONNECTIVITY_CONNECT_FAILED);
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

int dm_cmw_conn_local_alcs_init(void **conn_handle)
{
    int res = 0;

    if (conn_handle == NULL || *conn_handle != NULL) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    res = _dm_cmw_conn_local_alcs_create(conn_handle);
    if (res != SUCCESS_RETURN) {
        dm_log_warning(DM_UTILS_LOG_CM_LOCAL_CONNECTIVITY_CREATE_FAILED);
        return ERROR_NO_MEM;
    }

    res = _dm_cmw_conn_connect(*conn_handle);
    if (res != SUCCESS_RETURN) {
        dm_log_warning(DM_UTILS_LOG_CM_LOCAL_CONNECTIVITY_CONNECT_FAILED);
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

int dm_cmw_conn_get_prototol_handle(void *conn_handle, void **protocol_handle)
{
    if (conn_handle == NULL || protocol_handle == NULL || *protocol_handle != NULL) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    *protocol_handle = iotx_cm_get_protocol_handle(conn_handle);
    if (*protocol_handle == NULL) {
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

int dm_cmw_conn_destroy(void **conn_handle)
{
    if (conn_handle == NULL) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    return iotx_cm_close(conn_handle, NULL);
}

int dm_cmw_cloud_register(void *conn_handle, char **uri, int count, void *user_data)
{
    int res = 0, index = 0;
    iotx_cm_register_param_t *cm_register_param = NULL;
    iotx_cm_register_param_t *cm_register_param_item = NULL;

    if (conn_handle == NULL || uri == NULL || count <= 0) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    for (index = 0; index < count; index++) {
        if (*(uri + index) == NULL) {
            return FAIL_RETURN;
        } else {
            dm_log_debug("URI %d: %s", index, *(uri + index));
        }
    }

    cm_register_param = DM_malloc(count * sizeof(iotx_cm_register_param_t));
    if (cm_register_param == NULL) {
        dm_log_err(DM_UTILS_LOG_MEMORY_NOT_ENOUGH);
        return FAIL_RETURN;
    }
    memset(cm_register_param, 0, count * sizeof(iotx_cm_register_param_t));

    for (index = 0; index < count; index++) {
        cm_register_param_item = cm_register_param + index;
        cm_register_param_item->URI = *(uri + index);
        cm_register_param_item->register_func = dm_cmw_topic_callback;
        cm_register_param_item->user_data = user_data;
        cm_register_param_item->mail_box = NULL;
    }

    res =  iotx_cm_serv_reg(conn_handle, cm_register_param, count, NULL);

    DM_free(cm_register_param);

    return res;

}

int dm_cmw_cloud_unregister(void *conn_handle, char *uri)
{
    iotx_cm_unregister_param_t cm_unregister_param;

    if (conn_handle == NULL || uri == NULL) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    memset(&cm_unregister_param, 0, sizeof(iotx_cm_unregister_param_t));
    cm_unregister_param.URI = uri;

    return iotx_cm_serv_unreg(conn_handle, &cm_unregister_param, NULL);
}

int dm_cmw_local_init_second(void *conn_handle)
{
    return iotx_cm_postinit(conn_handle);
}

int dm_cmw_local_add_service(void *conn_handle, char *uri, iotx_dm_message_auth_types_t auth_type, void *user_data)
{
    iotx_cm_add_service_param_t cm_add_service_param;

    if (conn_handle == NULL || uri == NULL) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    memset(&cm_add_service_param, 0, sizeof(iotx_cm_add_service_param_t));
    cm_add_service_param.URI = uri;
    cm_add_service_param.auth_type = auth_type;
    cm_add_service_param.service_func = dm_cmw_topic_callback;
    cm_add_service_param.user_data = NULL;
    cm_add_service_param.mail_box = NULL;

    return iotx_cm_serv_add(conn_handle, &cm_add_service_param, NULL);
}

int dm_cmw_local_remove_service(void *conn_handle, char *uri)
{
    iotx_cm_remove_service_param_t cm_remove_servie_param;

    if (conn_handle == NULL || uri == NULL) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    memset(&cm_remove_servie_param, 0, sizeof(iotx_cm_remove_service_param_t));
    cm_remove_servie_param.URI = uri;

    return iotx_cm_serv_del(conn_handle, &cm_remove_servie_param, NULL);
}

int dm_cmw_local_add_subdev(void *conn_handle, const char *product_key, const char *device_name)
{
    if (conn_handle == NULL ||
        product_key == NULL || (strlen(product_key) >= PRODUCT_KEY_MAXLEN) ||
        device_name == NULL || (strlen(device_name) >= DEVICE_NAME_MAXLEN)) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    return iotx_cm_subdev_add(conn_handle, product_key, device_name, NULL);
}

int dm_cmw_local_remove_subdev(void *conn_handle, char *product_key, char *device_name)
{
    if (conn_handle == NULL ||
        product_key == NULL || (strlen(product_key) >= PRODUCT_KEY_MAXLEN) ||
        device_name == NULL || (strlen(device_name) >= DEVICE_NAME_MAXLEN)) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    return iotx_cm_subdev_del(conn_handle, product_key, device_name, NULL);
}


int dm_cmw_send_to_all(char *uri, char *payload, int payload_len, void *user_data)
{
    int res = 0;
    iotx_cm_message_info_t cm_message_info;
    iotx_cm_send_peer_t cm_send_peer;

    if (uri == NULL || payload == NULL) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    memset(&cm_message_info, 0, sizeof(iotx_cm_message_info_t));
    cm_message_info.ack_type = IOTX_CM_MESSAGE_NO_ACK;
    cm_message_info.URI = uri;
    cm_message_info.URI_length = strlen(uri);
    cm_message_info.payload = payload;
    cm_message_info.payload_length = payload_len;
    cm_message_info.conn_ctx = user_data;

    memset(&cm_send_peer, 0, sizeof(iotx_cm_send_peer_t));
    //HAL_GetProductKey(cm_send_peer.product_key);
    //HAL_GetDeviceName(cm_send_peer.device_name);

    res = iotx_cm_send(NULL, &cm_send_peer, &cm_message_info, NULL);
    dm_log_info(DM_UTILS_LOG_CM_SEND_RESULT, res);

    return res;
}

int dm_cmw_send_to_cloud(char *uri, char *payload, void *user_data)
{
    int res = 0;
    iotx_cm_message_info_t cm_message_info;
    iotx_cm_send_peer_t cm_send_peer;

    if (uri == NULL || payload == NULL) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    memset(&cm_message_info, 0, sizeof(iotx_cm_message_info_t));
    cm_message_info.ack_type = IOTX_CM_MESSAGE_NO_ACK;
    cm_message_info.URI = uri;
    cm_message_info.URI_length = strlen(uri);
    cm_message_info.payload = payload;
    cm_message_info.payload_length = strlen(payload);
    cm_message_info.conn_ctx = user_data;

    memset(&cm_send_peer, 0, sizeof(iotx_cm_send_peer_t));
    HAL_GetProductKey(cm_send_peer.product_key);
    HAL_GetDeviceName(cm_send_peer.device_name);

    res = iotx_cm_send(NULL, &cm_send_peer, &cm_message_info, NULL);
    dm_log_info(DM_UTILS_LOG_CM_SEND_RESULT, res);

    return res;
}

int dm_cmw_send_to_local(char *uri, int uri_len, char *payload, int payload_len, void *user_data)
{
    int res = 0;
    char *cm_uri = NULL, *cm_payload = NULL;
    iotx_cm_message_info_t cm_message_info;
    void *local_conn = NULL;

    if (uri == NULL || uri_len <= 0 || payload == NULL || payload_len <= 0) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    local_conn = dm_conn_get_local_conn();
    if (local_conn == NULL) {
        return FAIL_RETURN;
    }

    cm_uri = DM_malloc(uri_len + 1);
    if (cm_uri == NULL) {
        dm_log_err(DM_UTILS_LOG_MEMORY_NOT_ENOUGH);
        return FAIL_RETURN;
    }
    memset(cm_uri, 0, uri_len + 1);
    memcpy(cm_uri, uri, uri_len);

    cm_payload = DM_malloc(payload_len + 1);
    if (cm_payload == NULL) {
        dm_log_err(DM_UTILS_LOG_MEMORY_NOT_ENOUGH);
        DM_free(cm_uri);
        return FAIL_RETURN;
    }
    memset(cm_payload, 0, payload_len + 1);
    memcpy(cm_payload, payload, payload_len);

    memset(&cm_message_info, 0, sizeof(iotx_cm_message_info_t));
    cm_message_info.ack_type = IOTX_CM_MESSAGE_NO_ACK;
    cm_message_info.URI = cm_uri;
    cm_message_info.URI_length = uri_len;
    cm_message_info.payload = cm_payload;
    cm_message_info.payload_length = payload_len;
    cm_message_info.conn_ctx = user_data;

    res = iotx_cm_send(local_conn, NULL, &cm_message_info, NULL);
    dm_log_info(DM_UTILS_LOG_CM_SEND_RESULT, res);

    DM_free(cm_uri);
    DM_free(cm_payload);
    return res;
}

int dm_cmw_send_to_device(void *conn_handle, char *product_key, char *device_name, char *uri, char *payload,
                          void *user_data)
{
    iotx_cm_message_info_t cm_message_info;
    iotx_cm_send_peer_t cm_send_peer;

    if (conn_handle == NULL ||
        product_key == NULL || (strlen(product_key) >= PRODUCT_KEY_MAXLEN) ||
        device_name == NULL || (strlen(device_name) >= DEVICE_NAME_MAXLEN) ||
        uri == NULL || payload == NULL) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    memset(&cm_message_info, 0, sizeof(iotx_cm_message_info_t));
    cm_message_info.ack_type = IOTX_CM_MESSAGE_NO_ACK;
    cm_message_info.URI = uri;
    cm_message_info.URI_length = strlen(uri);
    cm_message_info.payload = payload;
    cm_message_info.payload_length = strlen(payload);
    cm_message_info.conn_ctx = user_data;

    memset(&cm_send_peer, 0, sizeof(iotx_cm_send_peer_t));
    memcpy(cm_send_peer.product_key, product_key, strlen(product_key));
    memcpy(cm_send_peer.device_name, device_name, strlen(device_name));

    return iotx_cm_send(conn_handle, &cm_send_peer, &cm_message_info, NULL);
}
