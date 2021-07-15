#include "sl_config.h"

#ifdef MQTT_COMM_ENABLED

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include "iot_import.h"
#include "iot_export.h"

#include "iotx_utils.h"
#include "iotx_log.h"
#include "string_utils.h"
#include "iotx_mqtt_config.h"
#include "iotx_system_internal.h"

static iotx_conn_info_t     iotx_redir_info = {0, 0, NULL, NULL, NULL, NULL, NULL};
static int redirect_region_flag = 0;
static int redirect_region_retry_cnt = 0;


//#define REDIRECT_DEBUG
//#define REDIRECT_DEBUG_TOPIC

#define TOPIC_LENGTH (128)
#define DATA_LENGTH (48)
#define REDIRECT_IOT_ID_LEN           (128)
#define REDIRECT_IOT_TOKEN_LEN        (64)
#define REDIRECT_MAX_RETRY_CNT         (3)

#ifdef REDIRECT_DEBUG_TOPIC
#define REDIRECT_SUB_TOPIC "/%s/%s/user/thing/bootstrap/config/push"
#define REDIRECT_PUB_TOPIC "/%s/%s/user/thing/bootstrap/config/push_reply"
#else
#define REDIRECT_SUB_TOPIC "/sys/%s/%s/thing/bootstrap/config/push"
#define REDIRECT_PUB_TOPIC "/sys/%s/%s/thing/bootstrap/config/push_reply"
#endif
#define REDIRECT_MSG_ID "id"
#define REDIRECT_MSG_IOTID "params.iotId"
#define REDIRECT_MSG_HOST "params.endpoint.mqtt.host"
#define REDIRECT_MSG_PORT "params.endpoint.mqtt.port"
#define REDIRECT_MSG_TOKEN "params.endpoint.mqtt.token"

#define PUB_DATA_STR "{\"id\":%d,\"code\":%d,\"data\":{}}"

#define redirect_err(...) log_err("redirect", __VA_ARGS__)
#define redirect_warn(...) log_warning("redirect", __VA_ARGS__)
//#ifdef REDIRECT_DEBUG
#define redirect_info(...) log_info("redirect", __VA_ARGS__)
// #else
// #define redirect_info(...)
// #endif

enum _response_code
{
    SUCCESS = 200,
    PARAM_ERROR = 2000,
    JSON_PARSE_ERROR
};

iotx_conn_info_pt iotx_redirect_info_get(void)
{
    return &iotx_redir_info;
}

void iotx_redirect_info_release(void)
{
    redirect_region_flag = 0;

    if (iotx_redir_info.host_name != NULL) {
        LITE_free(iotx_redir_info.host_name);
    }
    if (iotx_redir_info.username != NULL) {
        LITE_free(iotx_redir_info.username);
    }
    if (iotx_redir_info.password != NULL) {
        LITE_free(iotx_redir_info.password);
    }
    if (iotx_redir_info.client_id != NULL) {
        LITE_free(iotx_redir_info.client_id);
    }
    memset(&iotx_redir_info, 0, sizeof(iotx_redir_info));
}

int iotx_redirect_set_flag(int flag)
{
    redirect_region_flag = flag;
    redirect_region_retry_cnt = 0;

    return redirect_region_flag;
}

int iotx_redirect_get_flag(void)
{
    if(redirect_region_flag == 1){
        if(redirect_region_retry_cnt <= REDIRECT_MAX_RETRY_CNT){
            redirect_region_retry_cnt++;
        }else{
            redirect_region_retry_cnt = 0;
            redirect_region_flag = 0;
        }
    }

    return redirect_region_flag;
}
/*
static int _response_cloud(int id, int code)
{
    int ret = 0;
    char *p_topic = NULL;
    char data[DATA_LENGTH] = {0};
    iotx_device_info_t *p_device = NULL;

    p_device = LITE_malloc(sizeof(iotx_device_info_t));
    if (!p_device)
    {
        redirect_err("no mem");
        goto ERROR;
    }

    ret = iotx_device_info_get(p_device);
    if (ret < 0)
    {
        redirect_err("get device info err");
        goto ERROR;
    }

    p_topic = LITE_malloc(TOPIC_LENGTH);
    if (!p_topic)
    {
        redirect_err("no mem");
        goto ERROR;
    }

    memset(p_topic, 0, TOPIC_LENGTH);
    snprintf(p_topic, TOPIC_LENGTH, REDIRECT_PUB_TOPIC, p_device->product_key, p_device->device_name);
    redirect_info("pub p_topic:%s", p_topic);

    memset(data, 0, DATA_LENGTH);
    snprintf(data, DATA_LENGTH, PUB_DATA_STR, id, code);

    ret = IOT_MQTT_Publish_Simple(NULL, p_topic, IOTX_MQTT_QOS1, data, strlen(data));
ERROR:
    if (p_topic)
        LITE_free(p_topic);
    if (p_device)
        LITE_free(p_device);

    return ret;        
}
*/
static void redirect_msg_cb(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    int id = 0;
    int code = 200;
    char *pvalue = NULL;
    iotx_mqtt_topic_info_pt ptopic_info = NULL;

    char            port_str[6] = {0};
    iotx_conn_info_pt conn = NULL;
    char *payload = NULL;
    void *callback = NULL;

    redirect_info("-----------------------");
    if (msg == NULL){
        redirect_err("params error");
        code = PARAM_ERROR;
        goto ERROR;
    }

    ptopic_info = (iotx_mqtt_topic_info_pt)msg->msg;

    if (ptopic_info == NULL){
        redirect_err("params error");
        code = PARAM_ERROR;
        goto ERROR;
    }

    payload = (char *)ptopic_info->payload;
    if(payload == NULL){
        redirect_err("params error");
        goto ERROR;
    }

    // print topic name and topic message
    redirect_info("Event(%d)", msg->event_type);
    if (msg->event_type != IOTX_MQTT_EVENT_PUBLISH_RECEIVED){
        redirect_info("do nothing");
        return;
    }

    #ifdef REDIRECT_DEBUG
    redirect_info("Topic: '%.*s' (Length: %d)",
                  ptopic_info->topic_len,
                  ptopic_info->ptopic,
                  ptopic_info->topic_len);
    redirect_info("Payload: '%.*s' (Length: %d)",
                  ptopic_info->payload_len,
                  ptopic_info->payload,
                  ptopic_info->payload_len);
    #endif

    iotx_redirect_info_release();

    conn = iotx_redirect_info_get();
    if(conn == NULL){
        goto ERROR;
    }


    pvalue = LITE_json_value_of(REDIRECT_MSG_ID, payload, MEM_MAGIC,"redirect");
    if (!pvalue){
        redirect_err("id parse error");
        code = JSON_PARSE_ERROR;
        goto ERROR;
    }

    id = atoi(pvalue);
    redirect_info("id=%d", id);
    
    LITE_free(pvalue);

    pvalue = LITE_json_value_of(REDIRECT_MSG_IOTID, payload, MEM_MAGIC,"redirect");
    if (!pvalue){
        redirect_err("REDIRECT_MSG_IOTID parse error");
        code = JSON_PARSE_ERROR;
        goto ERROR;
    }
    redirect_info("iotid = %s", pvalue);
    conn->username = pvalue;

    pvalue = LITE_json_value_of(REDIRECT_MSG_TOKEN, payload, MEM_MAGIC,"redirect");
    if (NULL == pvalue){
        redirect_err("REDIRECT_MSG_TOKEN parse error");
        code = JSON_PARSE_ERROR;
        goto ERROR;
    }
    redirect_info("token=%s", pvalue);
    conn->password = pvalue;

    pvalue = LITE_json_value_of(REDIRECT_MSG_HOST, payload, MEM_MAGIC,"redirect");
    if (NULL == pvalue){
        redirect_err("REDIRECT_MSG_HOST parse error");
        code = JSON_PARSE_ERROR;
        goto ERROR;
    }
    redirect_info("host = %s", pvalue);
    conn->host_name = pvalue;

    pvalue = LITE_json_value_of(REDIRECT_MSG_PORT, payload, MEM_MAGIC,"redirect");
    if (NULL == pvalue){
        redirect_err("REDIRECT_MSG_PORT parse error");
        code = JSON_PARSE_ERROR;
        goto ERROR;
    }
    redirect_info("port=%s", pvalue);
    strcpy(port_str, pvalue);
    pvalue = NULL;
    conn->port = atoi(port_str);
    LITE_free(pvalue);
    pvalue = NULL;

    iotx_redirect_set_flag(1);
    redirect_info("set redirect region flag");

    callback = iotx_event_callback(ITE_REDIRECT);
    if (callback) {
        ((int (*)(void))callback)();
    }

ERROR:
    if (code != 200){
        redirect_err("redirect parse error");
    }

    //_response_cloud(id, code);
}

int iotx_redirect_region_subscribe(void)
{
    int ret = 0;
    char *p_topic = NULL;
    iotx_device_info_t p_device;

    ret = iotx_device_info_get(&p_device);
    if (ret < 0){
        redirect_err("get device info err");
        goto ERROR;
    }

    p_topic = LITE_malloc(TOPIC_LENGTH);
    if (!p_topic){
        redirect_err("no mem");
        goto ERROR;
    }

    memset(p_topic, 0, TOPIC_LENGTH);
    snprintf(p_topic, TOPIC_LENGTH, REDIRECT_SUB_TOPIC, p_device.product_key, p_device.device_name);
    redirect_info("p_topic:%s", p_topic);

    ret = IOT_MQTT_Subscribe(NULL, p_topic, IOTX_MQTT_QOS0, redirect_msg_cb, NULL);
    if (ret < 0){
        redirect_err("sub failed");
    }else{
        redirect_info("sub success");
    }
    return ret;

ERROR:
    if (p_topic){
        LITE_free(p_topic);
    }

    iotx_redirect_info_release();

    return -1;
}

int iotx_redirect_get_iotId_iotToken(char *iot_id,char *iot_token,char *host,uint16_t *pport)
{
    iotx_conn_info_t *redir_conn = NULL;
    
    redir_conn = iotx_redirect_info_get();

    if(redir_conn == NULL || 
        redir_conn->username == NULL ||
        redir_conn->password == NULL ||
        redir_conn->host_name == NULL ||
        iot_id == NULL ||
        iot_token == NULL ||
        host == NULL||
        pport == NULL){
            return -1;
        }
 
    strcpy(iot_id, redir_conn->username);
    strcpy(iot_token, redir_conn->password);
    strcpy(host, redir_conn->host_name);
    *pport = redir_conn->port;  

    return 0;
}

#endif