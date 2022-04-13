/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <string.h>
#include "infra_compat.h"
#include "dev_sign_api.h"
#include "mqtt_api.h"
#include "wrappers.h"

char g_product_key[IOTX_PRODUCT_KEY_LEN + 1]       = "a1WGzkI1GJR";
char g_product_secret[IOTX_PRODUCT_SECRET_LEN + 1] = "GE0zUPUMJpaTDrzG";
char g_device_name[IOTX_DEVICE_NAME_LEN + 1]       = "engine_010";

#define EXAMPLE_TRACE(fmt, ...)  \
    do { \
        HAL_Printf("%s|%03d :: ", __func__, __LINE__); \
        HAL_Printf(fmt, ##__VA_ARGS__); \
        HAL_Printf("%s", "\r\n"); \
    } while(0)

void example_message_arrive(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    iotx_mqtt_topic_info_t     *topic_info = (iotx_mqtt_topic_info_pt) msg->msg;

    switch (msg->event_type) {
        case IOTX_MQTT_EVENT_PUBLISH_RECEIVED:
            /* print topic name and topic message */
            EXAMPLE_TRACE("Message Arrived:");
            EXAMPLE_TRACE("Topic  : %.*s", topic_info->topic_len, topic_info->ptopic);
            EXAMPLE_TRACE("Payload: %.*s", topic_info->payload_len, topic_info->payload);
            EXAMPLE_TRACE("\n");
            break;
        default:
            break;
    }
}

int example_subscribe(void *handle)
{
    int res = 0;
    const char *fmt = "/%s/%s/user/get";
    char *topic = NULL;
    int topic_len = 0;

    topic_len = strlen(fmt) + strlen(g_product_key) + strlen(g_device_name) + 1;
    topic = HAL_Malloc(topic_len);
    if (topic == NULL) {
        EXAMPLE_TRACE("memory not enough");
        return -1;
    }
    memset(topic, 0, topic_len);
    HAL_Snprintf(topic, topic_len, fmt, g_product_key, g_device_name);

    res = IOT_MQTT_Subscribe(handle, topic, IOTX_MQTT_QOS0, example_message_arrive, NULL);
    if (res < 0) {
        EXAMPLE_TRACE("subscribe failed");
        HAL_Free(topic);
        return -1;
    }

    HAL_Free(topic);
    return 0;
}

int example_publish(void *handle)
{
    int             res = 0;
    const char     *fmt = "/%s/%s/user/get";
    char           *topic = NULL;
    int             topic_len = 0;
    char           *payload = "{\"message\":\"hello!\"}";

    topic_len = strlen(fmt) + strlen(g_product_key) + strlen(g_device_name) + 1;
    topic = HAL_Malloc(topic_len);
    if (topic == NULL) {
        EXAMPLE_TRACE("memory not enough");
        return -1;
    }
    memset(topic, 0, topic_len);
    HAL_Snprintf(topic, topic_len, fmt, g_product_key, g_device_name);

    res = IOT_MQTT_Publish_Simple(0, topic, IOTX_MQTT_QOS0, payload, strlen(payload));
    if (res < 0) {
        EXAMPLE_TRACE("publish failed, res = %d", res);
        HAL_Free(topic);
        return -1;
    }

    HAL_Free(topic);
    return 0;
}

void example_event_handle(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    EXAMPLE_TRACE("msg->event_type : %d", msg->event_type);
}

extern const char *IOT_Extension_StateDesc(const int);
int everything_state_handle(const int state_code, const char *state_message)
{
    /*
     * NOTE: Uncomment below to demonstrate how to dump code descriptions
     *
     * After invoking IOT_Extension_StateDesc(), integer code will be translated into strings
     *
     * It looks like:
     *
     * ...
     * everything_state_handle|102 :: recv -0x0329(pub - '/a1MZxOdcBnO/example1/user/get': 0), means 'Report publish relative parameters such as topic string'
     * ...
     *
     */

    /*
        EXAMPLE_TRACE("recv -0x%04X(%s), means '%s'",
                      -state_code,
                      state_message,
                      IOT_Extension_StateDesc(state_code));
    */
    EXAMPLE_TRACE("recv -0x%04X(%s)",
                  -state_code,
                  state_message);
    return 0;
}

int identity_response_handle(const char *payload)
{
    EXAMPLE_TRACE("identify: %s", payload);

    return 0;
}

/*
 *  NOTE: About demo topic of /${productKey}/${deviceName}/user/get
 *
 *  The demo device has been configured in IoT console (https://iot.console.aliyun.com)
 *  so that its /${productKey}/${deviceName}/user/get can both be subscribed and published
 *
 *  We design this to completely demonstrate publish & subscribe process, in this way
 *  MQTT client can receive original packet sent by itself
 *
 *  For new devices created by yourself, pub/sub privilege also requires being granted
 *  to its /${productKey}/${deviceName}/user/get for successfully running whole example
 */

int main(int argc, char *argv[])
{
    void                   *pclient = NULL;
    int                     res = 0;
    int                     loop_cnt = 0;
    iotx_mqtt_param_t       mqtt_params;
    iotx_dev_meta_info_t    meta_info;

    memset(&meta_info, 0x0, sizeof(meta_info));

    memcpy(meta_info.product_key, g_product_key, IOTX_PRODUCT_KEY_LEN);
    memcpy(meta_info.product_secret, g_product_secret, IOTX_PRODUCT_SECRET_LEN);
    memcpy(meta_info.device_name, g_device_name, IOTX_DEVICE_NAME_LEN);

    IOT_RegisterCallback(ITE_IDENTITY_RESPONSE, identity_response_handle);
    IOT_RegisterCallback(ITE_STATE_EVERYTHING, everything_state_handle);

    EXAMPLE_TRACE("mqtt example");

    /* Initialize MQTT parameter */
    /*
     * Note:
     *
     * If you did NOT set value for members of mqtt_params, SDK will use their default values
     * If you wish to customize some parameter, just un-comment value assigning expressions below
     *
     **/
    memset(&mqtt_params, 0x0, sizeof(mqtt_params));

    /**
     *
     *  MQTT connect hostname string
     *
     *  MQTT server's hostname can be customized here
     *
     *  default value is ${productKey}.iot-as-mqtt.cn-shanghai.aliyuncs.com
     */
    /* mqtt_params.host = "something.iot-as-mqtt.cn-shanghai.aliyuncs.com"; */

    /**
     *
     *  MQTT connect port number
     *
     *  TCP/TLS port which can be 443 or 1883 or 80 or etc, you can customize it here
     *
     *  default value is 1883 in TCP case, and 443 in TLS case
     */
    /* mqtt_params.port = 1883; */

    /**
     *
     * MQTT request timeout interval
     *
     * MQTT message request timeout for waiting ACK in MQTT Protocol
     *
     * default value is 2000ms.
     */
    /* mqtt_params.request_timeout_ms = 2000; */

    /**
     *
     * MQTT clean session flag
     *
     * If CleanSession is set to 0, the Server MUST resume communications with the Client based on state from
     * the current Session (as identified by the Client identifier).
     *
     * If CleanSession is set to 1, the Client and Server MUST discard any previous Session and Start a new one.
     *
     * default value is 0.
     */
    /* mqtt_params.clean_session = 0; */

    /**
     *
     * MQTT keepAlive interval
     *
     * KeepAlive is the maximum time interval that is permitted to elapse between the point at which
     * the Client finishes transmitting one Control Packet and the point it starts sending the next.
     *
     * default value is 60000.
     */
    /* mqtt_params.keepalive_interval_ms = 60000; */

    /**
     *
     * MQTT write buffer size
     *
     * Write buffer is allocated to place upstream MQTT messages, MQTT client will be limitted
     * to send packet no longer than this to Cloud
     *
     * default value is 1024.
     *
     */
    /* mqtt_params.write_buf_size = 1024; */

    /**
     *
     * MQTT read buffer size
     *
     * Write buffer is allocated to place downstream MQTT messages, MQTT client will be limitted
     * to recv packet no longer than this from Cloud
     *
     * default value is 1024.
     *
     */
    /* mqtt_params.read_buf_size = 1024; */

    /**
     *
     * MQTT event callback function
     *
     * Event callback function will be called by SDK when it want to notify user what is happening inside itself
     *
     * default value is NULL, which means PUB/SUB event won't be exposed.
     *
     */
    mqtt_params.handle_event.h_fp = example_event_handle;

    pclient = IOT_MQTT_Construct_Interal(&mqtt_params, meta_info);
    if (NULL == pclient) {
        EXAMPLE_TRACE("MQTT construct failed");
        return -1;
    }

    // IOT_Ioctl(IOTX_IOCTL_GET_PRODUCT_KEY, g_product_key);
    // IOT_Ioctl(IOTX_IOCTL_GET_DEVICE_NAME, g_device_name);

    res = example_subscribe(pclient);
    if (res < 0) {
        IOT_MQTT_Destroy(&pclient);
        return -1;
    }

    while (1) {
        if (0 == loop_cnt % 20) {
            example_publish(pclient);
        }

        IOT_MQTT_Yield(pclient, 200);

        loop_cnt += 1;
    }

    return 0;
}

