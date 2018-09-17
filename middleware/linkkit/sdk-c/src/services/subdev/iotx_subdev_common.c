/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#include "iot_import.h"
#include "subdev_debug.h"
#include "iotx_utils.h"
#include "utils_timer.h"
#include "utils_list.h"
#include "iotx_system.h"
#include "utils_hmac.h"
#include "iot_export_mqtt.h"

#include "iotx_subdev_common.h"

#ifndef SUBDEV_VIA_CLOUD_CONN
extern void iotx_gateway_event_handle(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg);
#endif


char *iotx_gateway_splice_common_packet(const char *product_key,
        const char* device_name,
        const char* method,
        uint32_t* msg_id)
{
#define COMMON_PACKET_FMT     "{\"id\":%d,\"version\":\"1.0\",\"params\":[{\"productKey\":\"%s\",\"deviceName\":\"%s\"}],\"method\":\"%s\"}"

    int len, ret;
    char* msg = NULL;
    uint32_t id = 0;

    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(product_key, NULL);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(device_name, NULL);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(method, NULL);

    /* sum the string length */
    len = strlen(COMMON_PACKET_FMT) + strlen(product_key) + strlen(device_name) + strlen(method) + 12;
    MALLOC_MEMORY_WITH_RESULT(msg, len, NULL);
    id = IOT_Gateway_Generate_Message_ID();
    ret = HAL_Snprintf(msg,
                   len,
                   COMMON_PACKET_FMT,
                   id,
                   product_key,
                   device_name,
                   method);
    if (ret < 0) {
        subdev_err("splice packet error!");
        LITE_free(msg);
        return NULL;
    }

    *msg_id = id;

    return msg;
}



char *iotx_gateway_splice_topo_get_packet(uint32_t* msg_id)
{
#define TOPOADD_PACKET_FMT     "{\"id\":%d,\"version\":\"1.0\",\"params\":{},\"method\":\"thing.topo.get\"}"

    int len, ret;
    char* msg = NULL;
    uint32_t id = 0;


    /* sum the string length */
    len = strlen(TOPOADD_PACKET_FMT) + 12;
    MALLOC_MEMORY_WITH_RESULT(msg, len, NULL);
    id = IOT_Gateway_Generate_Message_ID();
    ret = HAL_Snprintf(msg,
                   len,
                   TOPOADD_PACKET_FMT,
                   id);
    if (ret < 0) {
        subdev_err("splice packet error!");
        LITE_free(msg);
        return NULL;
    }

    *msg_id = id;

    return msg;
}

char *iotx_gateway_splice_config_get_packet(uint32_t* msg_id)
{
#define CONFIGGET_PACKET_FMT     "{\"id\":%d,\"version\":\"1.0\",\"params\":{\"configScope\":\"product\",\"getType\":\"file\"},\"method\":\"thing.config.get\"}"

    int len, ret;
    char* msg = NULL;
    uint32_t id = 0;


    /* sum the string length */
    len = strlen(CONFIGGET_PACKET_FMT) + 12;
    MALLOC_MEMORY_WITH_RESULT(msg, len, NULL);
    id = IOT_Gateway_Generate_Message_ID();
    ret = HAL_Snprintf(msg,
                   len,
                   CONFIGGET_PACKET_FMT,
                   id);
    if (ret < 0) {
        subdev_err("splice packet error!");
        LITE_free(msg);
        return NULL;
    }

    *msg_id = id;

    return msg;
}


char *iotx_gateway_splice_logout_packet(const char *product_key,
        const char* device_name,
        uint32_t* msg_id)
{
#define LOGOUT_PACKET_FMT     "{\"id\":%d,\"version\":\"1.0\",\"params\":{\"productKey\":\"%s\",\"deviceName\":\"%s\"}}"

    int len, ret;
    char* msg = NULL;
    uint32_t id = 0;

    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(product_key, NULL);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(device_name, NULL);

    /* sum the string length */
    len = strlen(LOGOUT_PACKET_FMT) + strlen(product_key) + strlen(device_name) + 12;
    MALLOC_MEMORY_WITH_RESULT(msg, len, NULL);
    id = IOT_Gateway_Generate_Message_ID();
    ret = HAL_Snprintf(msg,
                   len,
                   LOGOUT_PACKET_FMT,
                   id,
                   product_key,
                   device_name);
    if (ret < 0) {
        subdev_err("splice packet error!");
        LITE_free(msg);
        return NULL;
    }

    *msg_id = id;

    return msg;
}


char *iotx_gateway_splice_login_packet(const char *product_key,
        const char* device_name,
        const char* clientId,
        const char* timestamp,
        const char* sign_method,
        const char* sign,
        const char* cleanSession,
        uint32_t* msg_id)
{
#define LOGIN_PACKET_FMT     "{\"id\":%d,\"params\":{\"productKey\":\"%s\",\"deviceName\":\"%s\",\"clientId\":\"%s\",\"timestamp\":\"%s\",\"signMethod\":\"%s\",\"sign\":\"%s\",\"cleanSession\":\"%s\"}}"

    int len, ret;
    char* msg = NULL;
    uint32_t id = 0;

    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(product_key, NULL);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(device_name, NULL);
    PARAMETER_NULL_CHECK_WITH_RESULT(clientId, NULL);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(timestamp, NULL);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(sign_method, NULL);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(sign, NULL);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(cleanSession, NULL);

    /* sum the string length */
    len = strlen(LOGIN_PACKET_FMT) + strlen(product_key) + strlen(device_name) + strlen(clientId)
            + strlen(timestamp) + strlen(sign_method) + strlen(sign) +strlen(cleanSession) + 12;
    MALLOC_MEMORY_WITH_RESULT(msg, len, NULL);
    id = IOT_Gateway_Generate_Message_ID();
    ret = HAL_Snprintf(msg,
                   len,
                   LOGIN_PACKET_FMT,
                   id,
                   product_key,
                   device_name,
                   clientId,
                   timestamp,
                   sign_method,
                   sign,
                   cleanSession);
    if (ret < 0) {
        subdev_err("splice packet error!");
        LITE_free(msg);
        return NULL;
    }

    *msg_id = id;

    return msg;
}

char *iotx_gateway_splice_topo_add_packet(const char *product_key,
        const char* device_name,
        const char* clientId,
        const char* timestamp,
        const char* sign_method,
        const char* sign,
        const char* method,
        uint32_t* msg_id)
{
#define TODO_ADD_PACKET_FMT     "{\"id\":%d,\"params\":[{\"productKey\":\"%s\",\"deviceName\":\"%s\",\"clientId\":\"%s\",\"timestamp\":\"%s\",\"signMethod\":\"%s\",\"sign\":\"%s\"}],\"method\":\"%s\"}"

    int len, ret;
    char* msg = NULL;
    uint32_t id = 0;

    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(product_key, NULL);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(device_name, NULL);
    PARAMETER_NULL_CHECK_WITH_RESULT(clientId, NULL);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(timestamp, NULL);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(sign_method, NULL);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(sign, NULL);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(method, NULL);

    /* sum the string length */
    len = strlen(TODO_ADD_PACKET_FMT) + strlen(product_key) + strlen(device_name) + strlen(clientId)
            + strlen(timestamp) + strlen(sign_method) + strlen(sign) +strlen(method) + 12;
    MALLOC_MEMORY_WITH_RESULT(msg, len, NULL);
    id = IOT_Gateway_Generate_Message_ID();
    ret = HAL_Snprintf(msg,
                   len,
                   TODO_ADD_PACKET_FMT,
                   id,
                   product_key,
                   device_name,
                   clientId,
                   timestamp,
                   sign_method,
                   sign,
                   method);
    if (ret < 0) {
        subdev_err("splice packet error!");
        LITE_free(msg);
        return NULL;
    }

    *msg_id = id;

    return msg;
}

static void iotx_gateway_splice_device_cloud_id(char* device_cloud_id,
        const char* product_key,
        const char* device_name)
{
    PARAMETER_STRING_NULL_CHECK(product_key);
    PARAMETER_STRING_NULL_CHECK(device_name);
    PARAMETER_NULL_CHECK(device_cloud_id);

    HAL_Snprintf(device_cloud_id, DEVICE_ID_LEN, "%s.%s", product_key, device_name);
}


int iotx_gateway_calc_sign(const char* product_key,
        const char* device_name,
        const char* device_secret,
        char* hmac_sigbuf,
        const int hmac_buflen,
        iotx_subdev_sign_method_types_t sign_method,
        const char *client_id,
        const char *timestamp_str)
{
    char signature[64];
    char hmac_source[256];

    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(product_key, FAIL_RETURN);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(device_name, FAIL_RETURN);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(device_secret, FAIL_RETURN);
    PARAMETER_NULL_CHECK_WITH_RESULT(hmac_sigbuf, FAIL_RETURN);
    PARAMETER_NULL_CHECK_WITH_RESULT(client_id, FAIL_RETURN);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(timestamp_str, FAIL_RETURN);

    memset(signature, 0, sizeof(signature));
    memset(hmac_source, 0, sizeof(hmac_source));
    HAL_Snprintf(hmac_source,
                      sizeof(hmac_source),
                      "clientId%s" "deviceName%s" "productKey%s" "timestamp%s",
                      client_id,
                      device_name,
                      product_key,
                      timestamp_str);

    if (sign_method == IOTX_SUBDEV_SIGN_METHOD_TYPE_SHA) {
        utils_hmac_sha1(hmac_source, strlen(hmac_source),
                    signature,
                    device_secret,
                    strlen(device_secret));
    } else if (sign_method == IOTX_SUBDEV_SIGN_METHOD_TYPE_MD5) {
        utils_hmac_md5(hmac_source, strlen(hmac_source),
                   signature,
                   device_secret,
                   strlen(device_secret));
    }

    memcpy(hmac_sigbuf, signature, hmac_buflen);
    return 0;
}

static int iotx_gateway_get_secure_mode(void)
{
#ifdef MQTT_DIRECT
#ifdef IOTX_WITHOUT_TLS
    return 3;                        /* MODE_TCP_DIRECT_PLAIN */
#else
    return 2;                        /* MODE_TLS_DIRECT */
#endif  /* IOTX_WITHOUT_TLS */
#else   /* MQTT_DIRECT */
#ifdef IOTX_WITHOUT_TLS
    return 0;                        /* MODE_TCP_GUIDER_PLAIN */
#else
    return -1;                       /* MODE_TLS_GUIDER */
#endif  /* IOTX_WITHOUT_TLS */

#endif  /* MQTT_DIRECT */

    return  -1;                      /* MODE_TLS_GUIDER */
}


void iotx_subdevice_calc_client_id(char* client_id,
        const char* product_key,
        const char* device_name)
{
    PARAMETER_NULL_CHECK(client_id);
    PARAMETER_STRING_NULL_CHECK(product_key);
    PARAMETER_STRING_NULL_CHECK(device_name);

    HAL_Snprintf(client_id,
            IOT_SUBDEVICE_CLIENT_ID_LEN,
            "%s.%s",
            product_key,
            device_name);
    subdev_info("client_id %s", client_id);
}

void iotx_gateway_calc_client_id(char* client_id,
        const char* device_cloud_id,
        const char* timestamp_str,
        const char* sign_method)
{
    int secure_mode = iotx_gateway_get_secure_mode();
    char partner_id[64 + 16] = {0};
    char temp[64] = {0};

    PARAMETER_NULL_CHECK(client_id);
    PARAMETER_STRING_NULL_CHECK(device_cloud_id);
    PARAMETER_STRING_NULL_CHECK(timestamp_str);
    PARAMETER_STRING_NULL_CHECK(sign_method);

    /* partner_id */
    memset(temp, 0, sizeof(temp));
    HAL_GetPartnerID(temp);
    if (strlen(temp)) {
        HAL_Snprintf(partner_id, strlen(partner_id), ",partner_id=%s", temp);
    } else {
        strcpy(partner_id, "");
    }

     /* clientId */
     HAL_Snprintf(client_id,
            CLIENT_ID_LEN,
            "%s|securemode=%d,timestamp=%s,signmethod=%s,gw=0%s|",
            device_cloud_id,
            secure_mode,
            timestamp_str,
            sign_method,
            partner_id);
    subdev_info("client_id %s", client_id);
}

int iotx_gateway_subscribe_unsubscribe_topic(iotx_gateway_pt gateway,
        const char* product_key,
        const char* device_name,
        const char* topic_fmt,
        const char* params,
        int is_subscribe)
{
    int ret = 0;
    int yiled_count = 0;
    char topic[GATEWAY_TOPIC_LEN_MAX] = {0};

    PARAMETER_GATEWAY_CHECK(gateway, FAIL_RETURN);

    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(product_key, FAIL_RETURN);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(device_name, FAIL_RETURN);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(topic_fmt, FAIL_RETURN);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(params, FAIL_RETURN);

    /* topic */
    ret = HAL_Snprintf(topic,
                GATEWAY_TOPIC_LEN_MAX,
                topic_fmt,
                product_key,
                device_name,
                params);
    if (ret < 0) {
        return FAIL_RETURN;
    }

    if (is_subscribe) {
        /* subscribe */
    #ifdef SUBDEV_VIA_CLOUD_CONN
        iotx_cloud_connection_msg_t msg = {0};
        msg.type = IOTX_CLOUD_CONNECTION_MESSAGE_TYPE_SUBSCRIBE;
        msg.QoS = IOTX_MESSAGE_QOS0;
        msg.URI = topic;
        msg.URI_length = strlen(msg.URI);
        msg.payload = NULL;
        msg.payload_length = 0;
        msg.response_handler = _response_handle;
        msg.response_pcontext = gateway;
        msg.content_type = IOTX_MESSAGE_CONTENT_TYPE_JSON;
        msg.message_type = IOTX_MESSAGE_CONFIRMABLE;

        IOT_Cloud_Connection_Send_Message(gateway->mqtt, &msg);

        ret = 1;
    #else /* SUBDEV_VIA_CLOUD_CONN */
        ret = IOT_MQTT_Subscribe(gateway->mqtt,
                 topic,
                 IOTX_MQTT_QOS0,
                 (iotx_mqtt_event_handle_func_fpt)iotx_gateway_event_handle,
                 gateway);
    #endif /* SUBDEV_VIA_CLOUD_CONN */
    } else {
        /* unsubscribe */
    #ifdef SUBDEV_VIA_CLOUD_CONN
        iotx_cloud_connection_msg_t msg = {0};
        msg.type = IOTX_CLOUD_CONNECTION_MESSAGE_TYPE_UNSUBSCRIBE;
        msg.URI = topic;
        msg.URI_length = strlen(msg.URI);
        msg.payload = NULL;
        msg.payload_length = 0;
        msg.response_handler = _response_handle;
        msg.response_pcontext = gateway;
        msg.content_type = IOTX_MESSAGE_CONTENT_TYPE_JSON;
        msg.message_type = IOTX_MESSAGE_CONFIRMABLE;

        if (SUCCESS_RETURN == IOT_Cloud_Connection_Send_Message(gateway->mqtt, &msg)) {
            ret = -4;  /* special value */
        }
    #else /* SUBDEV_VIA_CLOUD_CONN */
        ret = IOT_MQTT_Unsubscribe(gateway->mqtt, topic);
    #endif /* SUBDEV_VIA_CLOUD_CONN */
    }

#ifdef IOT_GATEWAY_SUPPORT_MULTI_THREAD
    HAL_MutexLock(gateway->gateway_data.lock_sync_enter);
    HAL_MutexLock(gateway->gateway_data.lock_sync);
#endif
    gateway->gateway_data.sync_status = ret;
#ifdef IOT_GATEWAY_SUPPORT_MULTI_THREAD
    HAL_MutexUnlock(gateway->gateway_data.lock_sync);
#endif

    while (ret == gateway->gateway_data.sync_status) {
        if (yiled_count > IOT_GATEWAY_YIELD_MAX_COUNT) {
            subdev_info("yiled max count, time out");
#ifdef IOT_GATEWAY_SUPPORT_MULTI_THREAD
            HAL_MutexUnlock(gateway->gateway_data.lock_sync_enter);
#endif
            return FAIL_RETURN;
        }

        IOT_Gateway_Yield(gateway, 200);
        yiled_count++;
    }
#ifdef IOT_GATEWAY_SUPPORT_MULTI_THREAD
    HAL_MutexUnlock(gateway->gateway_data.lock_sync_enter);
#endif

    if (0 == gateway->gateway_data.sync_status) {
        subdev_info("subscribe or unsubscribe [%s] successfully", topic);
    } else {
        subdev_info("subscribe or unsubscribe [%s] error!", topic);
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

int iotx_gateway_subscribe_unsubscribe_default(iotx_gateway_pt gateway,
        int is_subscribe)
{
    iotx_device_info_pt pdevice_info = iotx_device_info_get();

    PARAMETER_GATEWAY_CHECK(gateway, FAIL_RETURN);

    /* register_reply */
    if (FAIL_RETURN == iotx_gateway_subscribe_unsubscribe_topic(gateway,
                            pdevice_info->product_key,
                            pdevice_info->device_name,
                            TOPIC_SESSION_SUB_FMT,
                            "register_reply",
                            is_subscribe)){
        return FAIL_RETURN;
    }

    /* unregister_reply */
    if (FAIL_RETURN == iotx_gateway_subscribe_unsubscribe_topic(gateway,
                            pdevice_info->product_key,
                            pdevice_info->device_name,
                            TOPIC_SESSION_SUB_FMT,
                            "unregister_reply",
                            is_subscribe)){
        return FAIL_RETURN;
    }

    /* topo_add_reply */
    if (FAIL_RETURN == iotx_gateway_subscribe_unsubscribe_topic(gateway,
                            pdevice_info->product_key,
                            pdevice_info->device_name,
                            TOPIC_SESSION_TOPO_FMT,
                            "add_reply",
                            is_subscribe)){
        return FAIL_RETURN;
    }

    /* topo_delete_reply */
    if (FAIL_RETURN == iotx_gateway_subscribe_unsubscribe_topic(gateway,
                            pdevice_info->product_key,
                            pdevice_info->device_name,
                            TOPIC_SESSION_TOPO_FMT,
                            "delete_reply",
                            is_subscribe)){
        return FAIL_RETURN;
    }

    /* topo_get_reply */
    if (FAIL_RETURN == iotx_gateway_subscribe_unsubscribe_topic(gateway,
                            pdevice_info->product_key,
                            pdevice_info->device_name,
                            TOPIC_SESSION_TOPO_FMT,
                            "get_reply",
                            is_subscribe)){
        return FAIL_RETURN;
    }

    /* config_get_reply */
    if (FAIL_RETURN == iotx_gateway_subscribe_unsubscribe_topic(gateway,
                            pdevice_info->product_key,
                            pdevice_info->device_name,
                            TOPIC_SESSION_CONFIG_FMT,
                            "get_reply",
                            is_subscribe)){
        return FAIL_RETURN;
    }

    /* list_found_reply */
    if (FAIL_RETURN == iotx_gateway_subscribe_unsubscribe_topic(gateway,
                            pdevice_info->product_key,
                            pdevice_info->device_name,
                            TOPIC_SESSION_LIST_FOUND_FMT,
                            "found_reply",
                            is_subscribe)){
        return FAIL_RETURN;
    }

    /* login_reply */
    if (FAIL_RETURN == iotx_gateway_subscribe_unsubscribe_topic(gateway,
                            pdevice_info->product_key,
                            pdevice_info->device_name,
                            TOPIC_SESSION_COMBINE_FMT,
                            "login_reply",
                            is_subscribe)){
        return FAIL_RETURN;
    }

    /* logout_reply */
    if (FAIL_RETURN == iotx_gateway_subscribe_unsubscribe_topic(gateway,
                            pdevice_info->product_key,
                            pdevice_info->device_name,
                            TOPIC_SESSION_COMBINE_FMT,
                            "logout_reply",
                            is_subscribe)){
        return FAIL_RETURN;
    }

    /* RRPC request */
    if (FAIL_RETURN == iotx_gateway_subscribe_unsubscribe_topic(gateway,
                            pdevice_info->product_key,
                            pdevice_info->device_name,
                            TOPIC_SYS_RRPC_FMT,
                            "request",
                            is_subscribe)){
        return FAIL_RETURN;
    }

	/* DSL Template */
	if (FAIL_RETURN == iotx_gateway_subscribe_unsubscribe_topic(gateway,
                            pdevice_info->product_key,
                            pdevice_info->device_name,
                            TOPIC_SYS_DSL_TEMPLATE_FMT,
                            "get_reply",
                            is_subscribe)){
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

iotx_subdevice_session_pt iotx_subdevice_find_session(iotx_gateway_pt gateway,
        const char* product_key,
        const char* device_name)
{
    iotx_subdevice_session_pt session = NULL;

    PARAMETER_GATEWAY_CHECK(gateway, NULL);

    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(product_key, NULL);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(device_name, NULL);

    session = gateway->session_list;

    /* session is exist */
    while(session) {
        if (0 == strncmp(session->product_key, product_key, strlen(product_key)) &&
           0 == strncmp(session->device_name, device_name, strlen(device_name))) {
            return session;
        }
        session = session->next;
    }

    return NULL;
}


iotx_subdevice_session_pt iotx_subdevice_add_session(iotx_gateway_pt gateway,
        const char* product_key,
        const char* device_name,
        const char* device_cloud_secret,
        const char* sign,
        const char* timestamp,
        const char* client_id,
        iotx_subdev_sign_method_types_t sign_method_type,
        iotx_subdev_clean_session_types_t clean_session_type)
{
    iotx_subdevice_session_pt session = NULL;

    PARAMETER_GATEWAY_CHECK(gateway, NULL);

    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(product_key, NULL);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(device_name, NULL);

    /* create a new subdev session  */
    MALLOC_MEMORY_WITH_RESULT(session, sizeof(iotx_subdevice_session_t), NULL);

    /* add session to list */
    session->next = gateway->session_list;
    gateway->session_list = session;

#ifdef IOT_GATEWAY_SUPPORT_MULTI_THREAD
    HAL_MutexLock(session->lock_generic);
#endif
    strncpy(session->product_key, product_key, strlen(product_key));
    strncpy(session->device_name, device_name, strlen(device_name));
    if (device_cloud_secret)
        strncpy(session->device_cloud_secret, device_cloud_secret, strlen(device_cloud_secret));
    if (timestamp)
        strncpy(session->timestamp, timestamp, strlen(timestamp));
    if (sign)
        strncpy(session->sign, sign, strlen(sign));
    if (client_id)
        strncpy(session->client_id, client_id, strlen(client_id));
    session->sign_method = sign_method_type;
    session->clean_session = clean_session_type;
    session->dynamic_register = 0;
    iotx_gateway_splice_device_cloud_id(session->device_cloud_id, product_key, device_name);
#ifdef IOT_GATEWAY_SUPPORT_MULTI_THREAD
    HAL_MutexUnlock(session->lock_generic);
    HAL_MutexLock(session->lock_status);
#endif
    session->session_status = IOTX_SUBDEVICE_SEESION_STATUS_INIT;
#ifdef IOT_GATEWAY_SUPPORT_MULTI_THREAD
    HAL_MutexUnlock(session->lock_status);
#endif

    return session;
}

int iotx_subdevice_remove_session(iotx_gateway_pt gateway,
        const char* product_key,
        const char* device_name)
{
    iotx_subdevice_session_pt cur_session = NULL;
    iotx_subdevice_session_pt pre_session = NULL;

    PARAMETER_GATEWAY_CHECK(gateway, FAIL_RETURN);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(product_key, FAIL_RETURN);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(device_name, FAIL_RETURN);

    pre_session = cur_session = gateway->session_list;

    if (NULL == cur_session) {
        subdev_err("session list is empty");
        return SUCCESS_RETURN;
    }

    /* session is exist */
    while(cur_session) {
        if (0 == strncmp(cur_session->product_key, product_key, strlen(product_key)) &&
            0 == strncmp(cur_session->device_name, device_name, strlen(device_name)) ) {
            if (cur_session == gateway->session_list) {
                gateway->session_list = cur_session->next;
            } else {
                pre_session->next = cur_session->next;
            }
        #ifdef IOT_GATEWAY_SUPPORT_MULTI_THREAD
            HAL_MutexDestroy(cur_session->lock_generic);
            HAL_MutexDestroy(cur_session->lock_status);
        #endif
            LITE_free(cur_session);
            return SUCCESS_RETURN;
        }
        pre_session = cur_session;
        cur_session = cur_session->next;
    }

    return FAIL_RETURN;
}


int iotx_gateway_publish_sync(iotx_gateway_t* gateway,
        iotx_mqtt_qos_t qos,
        const char* topic,
        const char* packet,
        uint32_t message_id,
        iotx_common_reply_data_pt reply_data,
        iotx_gateway_publish_t publish_type)
{
#ifdef SUBDEV_VIA_CLOUD_CONN
    iotx_cloud_connection_msg_t msg;
#else
    iotx_mqtt_topic_info_t topic_msg;
#endif

    int yiled_count = 0;

    PARAMETER_GATEWAY_CHECK(gateway, FAIL_RETURN);

    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(topic, FAIL_RETURN);
    PARAMETER_STRING_NULL_CHECK_WITH_RESULT(packet, FAIL_RETURN);

#ifdef SUBDEV_VIA_CLOUD_CONN
    memset(&msg, 0x0, sizeof(iotx_cloud_connection_msg_t));
    msg.type = IOTX_CLOUD_CONNECTION_MESSAGE_TYPE_PUBLISH;
    msg.QoS = (iotx_message_qos_t)qos;
    msg.URI = (char*)topic;
    msg.URI_length = strlen(msg.URI);
    msg.payload = (char*)packet;
    msg.payload_length = strlen(packet);
    msg.content_type = IOTX_MESSAGE_CONTENT_TYPE_JSON;
    msg.message_type = IOTX_MESSAGE_CONFIRMABLE;
    msg.response_handler = _response_handle;
    msg.response_pcontext = gateway;
    if (FAIL_RETURN == IOT_Cloud_Connection_Send_Message(gateway->mqtt, &msg)) {
        return FAIL_RETURN;
    }

#else
    memset(&topic_msg, 0x0, sizeof(iotx_mqtt_topic_info_t));
    topic_msg.qos = qos;
    topic_msg.retain = 0;
    topic_msg.dup = 0;
    topic_msg.payload = (void *)packet;
    topic_msg.payload_len = strlen(packet);
    topic_msg.packet_id = 0;

    if (IOT_MQTT_Publish(gateway->mqtt, topic, &topic_msg) < 0) {
        subdev_err("publish failed\n");
        return FAIL_RETURN;
    }
#endif

    subdev_info("iotx_gateway_publish_sync topic [%s]\n", topic);

    /* sync id */
    reply_data->id = message_id;

    /* wait for response */
    while (message_id == reply_data->id) {
        if (yiled_count > IOT_GATEWAY_YIELD_MAX_COUNT) {
            subdev_info("yiled max count, time out");
            return FAIL_RETURN;
        }

        IOT_Gateway_Yield(gateway, 200);
        yiled_count++;
    }

    if (0 == reply_data->id) {
        if (200 == reply_data->code) {
            subdev_info("%s successfully", topic);
        } else {
            subdev_info("%s error!code:%d", topic, reply_data->code);
            return (~reply_data->code + 1);
        }
    } else {
        subdev_info("%s time out!", topic);
        return ERROR_REPLY_TIMEOUT;
    }

    return SUCCESS_RETURN;
}


int iotx_subdevice_set_session_status(iotx_subdevice_session_pt session, iotx_subdevice_session_status_t status)
{
    PARAMETER_NULL_CHECK_WITH_RESULT(session, FAIL_RETURN);

    session->session_status = status;

    return SUCCESS_RETURN;
}

iotx_subdevice_session_status_t iotx_subdevice_get_session_status(iotx_subdevice_session_pt session)
{
    PARAMETER_NULL_CHECK_WITH_RESULT(session, IOTX_SUBDEVICE_SEESION_STATUS_MAX);

    return session->session_status;
}

int iotx_subdevice_set_session_dynamic_register(iotx_subdevice_session_pt session)
{
    PARAMETER_NULL_CHECK_WITH_RESULT(session, FAIL_RETURN);

    session->dynamic_register = 1;

    return SUCCESS_RETURN;
}

int iotx_subdevice_get_session_dynamic_register(iotx_subdevice_session_pt session)
{
    PARAMETER_NULL_CHECK_WITH_RESULT(session, FAIL_RETURN);

    return session->dynamic_register;
}


