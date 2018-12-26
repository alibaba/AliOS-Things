#include <stdio.h>
#include <string.h>
#include "iot_import.h"
#include "iot_export.h"
#include "iot_export_mqtt.h"

#define PRODUCT_KEY    "a1X2bEnP82z"
#define DEVICE_NAME    "example_zc"
#define DEVICE_SECRET  "XZvZ1295n3mzGFYWHUnjy1xkdHb919C8"

void example_message_arrive(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    iotx_mqtt_topic_info_t     *topic_info = (iotx_mqtt_topic_info_pt) msg->msg;

    switch (msg->event_type) {
        case IOTX_MQTT_EVENT_PUBLISH_RECEIVED:
            /* print topic name and topic message */
            printf("Message Arrived: \n");
            printf("Topic  : %.*s\n",topic_info->topic_len,topic_info->ptopic);
            printf("Payload: %.*s\n",topic_info->payload_len,topic_info->payload);
            printf("\n");
            break;
        default:
            break;
    }
}

int example_subscribe(void *handle)
{
    int res = 0;
    const char *fmt = "/sys/%s/%s/thing/event/+/post_reply";
    char *topic = NULL;
    int topic_len = 0;

    topic_len = strlen(fmt) + strlen(PRODUCT_KEY) + strlen(DEVICE_NAME) + 1;
    topic = HAL_Malloc(topic_len);
    if (topic == NULL) {
        printf("memory not enough\n");
        return -1;
    }
    memset(topic,0,topic_len);
    HAL_Snprintf(topic,topic_len,fmt,PRODUCT_KEY,DEVICE_NAME);

    res = IOT_MQTT_Subscribe(handle, topic, IOTX_MQTT_QOS0, example_message_arrive, NULL);
    if (res < 0) {
        printf("subscribe failed\n");
        HAL_Free(topic);
        return -1;
    }

    HAL_Free(topic);
    return 0;
}

int example_publish(void *handle)
{
    int res = 0;
    iotx_mqtt_topic_info_t topic_msg;
    const char *fmt = "/sys/%s/%s/thing/event/property/post";
    char *topic = NULL;
    int topic_len = 0;
    char *payload = "{\"id\":\"1\",\"version\":\"1.0\",\"params\":{\"LightSwitch\":1},\"method\":\"thing.event.property.post\"}";

    topic_len = strlen(fmt) + strlen(PRODUCT_KEY) + strlen(DEVICE_NAME) + 1;
    topic = HAL_Malloc(topic_len);
    if (topic == NULL) {
        printf("memory not enough\n");
        return -1;
    }
    memset(topic,0,topic_len);
    HAL_Snprintf(topic,topic_len,fmt,PRODUCT_KEY,DEVICE_NAME);


    memset(&topic_msg, 0x0, sizeof(iotx_mqtt_topic_info_t));
    topic_msg.qos = IOTX_MQTT_QOS0;
    topic_msg.retain = 0;
    topic_msg.dup = 0;
    topic_msg.payload = (void *)payload;
    topic_msg.payload_len = strlen(payload);

    res = IOT_MQTT_Publish(handle, topic, &topic_msg);
    if (res < 0) {
        printf("publish failed\n");
        HAL_Free(topic);
        return -1;
    }

    HAL_Free(topic);
    return 0;
}

void example_event_handle(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    printf("msg->event_type : %d\n",msg->event_type);
}

int main(int argc, char *argv[])
{
    int res = 0;
    void *pclient;
    uint64_t time_prev = 0;
    iotx_mqtt_param_t mqtt_params;
    iotx_conn_info_pt pconn_info;

    /* Device AUTH */
    if (0 != IOT_SetupConnInfo(PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET, (void **)&pconn_info)) {
        printf("AUTH request failed!\n");
        return -1;
    }

    /* Initialize MQTT parameter */
    memset(&mqtt_params, 0x0, sizeof(mqtt_params));

    mqtt_params.port = pconn_info->port;
    mqtt_params.host = pconn_info->host_name;
    mqtt_params.client_id = pconn_info->client_id;
    mqtt_params.username = pconn_info->username;
    mqtt_params.password = pconn_info->password;
    mqtt_params.pub_key = pconn_info->pub_key;

    mqtt_params.request_timeout_ms = 2000;
    mqtt_params.clean_session = 0;
    mqtt_params.keepalive_interval_ms = 60000;
    mqtt_params.read_buf_size = 1024;
    mqtt_params.write_buf_size = 1024;

    mqtt_params.handle_event.h_fp = example_event_handle;
    mqtt_params.handle_event.pcontext = NULL;


    /* Construct a MQTT client with specify parameter */
    pclient = IOT_MQTT_Construct(&mqtt_params);
    if (NULL == pclient) {
        printf("MQTT construct failed\n");
        return -1;
    }

    res = example_subscribe(pclient);
    if (res < 0) {
        IOT_MQTT_Destroy(&pclient);
        return -1;
    }

    while(1) {
        if (HAL_UptimeMs() - time_prev > 5000) {
            example_publish(pclient);
            time_prev = HAL_UptimeMs();
        }

        IOT_MQTT_Yield(pclient, 200);
    }

    return 0;
}