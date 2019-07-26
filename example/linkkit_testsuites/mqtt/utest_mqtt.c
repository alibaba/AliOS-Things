#include "utest_mqtt.h"
#include "cut.h"

#if defined(MQTT_COMM_ENABLED)
// static char msg_buf[MSG_LEN_MAX];
// static char msg_readbuf[MSG_LEN_MAX];
long recvmsg_cnt;
char msg_buf[MSG_LEN_MAX];
char msg_readbuf[MSG_LEN_MAX];
char msg_pub[MSG_LEN_MAX];

char *UTEST_MQTT_PRODUCT_KEY = "a1qev6AHaVv";
char *UTEST_MQTT_PRODUCT_SECRET = "";
char *UTEST_MQTT_DEVICE_NAME = "aos_test_mqtt_01";
char *UTEST_MQTT_DEVICE_SECRET = "p0oOjwflClumpfQBzvSCUgJmUQhU7p9C";


const char *UTEST_TOPIC_UPDATE_FMT    = "/%s/%s/update";
const char *UTEST_TOPIC_ERROR_FMT     = "/%s/%s/update/error";
const char *UTEST_TOPIC_GET_FMT       = "/%s/%s/get";
const char *UTEST_TOPIC_DATA_FMT      = "/%s/%s/data";
const char *UTEST_TOPIC_DATA0_FMT     = "/%s/%s/data0";
const char *UTEST_TOPIC_DATA1_FMT     = "/%s/%s/data1";
const char *UTEST_TOPIC_DATA2_FMT     = "/%s/%s/data2";
const char *UTEST_TOPIC_DATA3_FMT     = "/%s/%s/data3";
const char *UTEST_TOPIC_NOTAUTHED_FMT = "/%s/%s/aaaaaaa";

char UTEST_TOPIC_UPDATE[TOPIC_MAXLEN]    = {0};
char UTEST_TOPIC_ERROR[TOPIC_MAXLEN]     = {0};
char UTEST_TOPIC_GET[TOPIC_MAXLEN]       = {0};
char UTEST_TOPIC_DATA[TOPIC_MAXLEN]      = {0};
char UTEST_TOPIC_DATA0[TOPIC_MAXLEN]     = {0};
char UTEST_TOPIC_DATA1[TOPIC_MAXLEN]     = {0};
char UTEST_TOPIC_DATA2[TOPIC_MAXLEN]     = {0};
char UTEST_TOPIC_DATA3[TOPIC_MAXLEN]     = {0};
char UTEST_TOPIC_NOTAUTHED[TOPIC_MAXLEN] = {0};

void utest_mqtt_update_topic(void)
{
    printf("UTEST_MQTT_PRODUCT_KEY   : %s\r\n", UTEST_MQTT_PRODUCT_KEY);
    printf("UTEST_MQTT_PRODUCT_SECRET: %s\r\n", UTEST_MQTT_PRODUCT_SECRET);
    printf("UTEST_MQTT_DEVICE_NAME   : %s\r\n", UTEST_MQTT_DEVICE_NAME);
    printf("UTEST_MQTT_DEVICE_SECRET : %s\r\n", UTEST_MQTT_DEVICE_SECRET);

    memset(UTEST_TOPIC_UPDATE, 0, TOPIC_MAXLEN);
    snprintf(UTEST_TOPIC_UPDATE, TOPIC_MAXLEN, UTEST_TOPIC_UPDATE_FMT, UTEST_MQTT_PRODUCT_KEY, UTEST_MQTT_DEVICE_NAME);

    memset(UTEST_TOPIC_ERROR, 0, TOPIC_MAXLEN);
    snprintf(UTEST_TOPIC_ERROR, TOPIC_MAXLEN, UTEST_TOPIC_ERROR_FMT, UTEST_MQTT_PRODUCT_KEY, UTEST_MQTT_DEVICE_NAME);

    memset(UTEST_TOPIC_GET, 0, TOPIC_MAXLEN);
    snprintf(UTEST_TOPIC_GET, TOPIC_MAXLEN, UTEST_TOPIC_GET_FMT, UTEST_MQTT_PRODUCT_KEY, UTEST_MQTT_DEVICE_NAME);

    memset(UTEST_TOPIC_DATA, 0, TOPIC_MAXLEN);
    snprintf(UTEST_TOPIC_DATA, TOPIC_MAXLEN, UTEST_TOPIC_DATA_FMT, UTEST_MQTT_PRODUCT_KEY, UTEST_MQTT_DEVICE_NAME);

    memset(UTEST_TOPIC_DATA0, 0, TOPIC_MAXLEN);
    snprintf(UTEST_TOPIC_DATA0, TOPIC_MAXLEN, UTEST_TOPIC_DATA0_FMT, UTEST_MQTT_PRODUCT_KEY, UTEST_MQTT_DEVICE_NAME);

    memset(UTEST_TOPIC_DATA1, 0, TOPIC_MAXLEN);
    snprintf(UTEST_TOPIC_DATA1, TOPIC_MAXLEN, UTEST_TOPIC_DATA1_FMT, UTEST_MQTT_PRODUCT_KEY, UTEST_MQTT_DEVICE_NAME);

    memset(UTEST_TOPIC_DATA2, 0, TOPIC_MAXLEN);
    snprintf(UTEST_TOPIC_DATA2, TOPIC_MAXLEN, UTEST_TOPIC_DATA2_FMT, UTEST_MQTT_PRODUCT_KEY, UTEST_MQTT_DEVICE_NAME);

    memset(UTEST_TOPIC_DATA3, 0, TOPIC_MAXLEN);
    snprintf(UTEST_TOPIC_DATA3, TOPIC_MAXLEN, UTEST_TOPIC_DATA3_FMT, UTEST_MQTT_PRODUCT_KEY, UTEST_MQTT_DEVICE_NAME);

    memset(UTEST_TOPIC_NOTAUTHED, 0, TOPIC_MAXLEN);
    snprintf(UTEST_TOPIC_NOTAUTHED, TOPIC_MAXLEN, UTEST_TOPIC_NOTAUTHED_FMT, UTEST_MQTT_PRODUCT_KEY,
             UTEST_MQTT_DEVICE_NAME);
}

void utest_mqtt_event_handle(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    uintptr_t packet_id = (uintptr_t)msg->msg;
    iotx_mqtt_topic_info_pt topic_info = (iotx_mqtt_topic_info_pt)msg->msg;

    switch (msg->event_type) {
        case IOTX_MQTT_EVENT_UNDEF:
            EXAMPLE_TRACE("undefined event occur.");
            break;

        case IOTX_MQTT_EVENT_DISCONNECT:
            EXAMPLE_TRACE("MQTT disconnect.");
            break;

        case IOTX_MQTT_EVENT_RECONNECT:
            EXAMPLE_TRACE("MQTT reconnect.");
            break;

        case IOTX_MQTT_EVENT_SUBCRIBE_SUCCESS:
            EXAMPLE_TRACE("subscribe success, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_SUBCRIBE_TIMEOUT:
            EXAMPLE_TRACE("subscribe wait ack timeout, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_SUBCRIBE_NACK:
            EXAMPLE_TRACE("subscribe nack, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_UNSUBCRIBE_SUCCESS:
            EXAMPLE_TRACE("unsubscribe success, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_UNSUBCRIBE_TIMEOUT:
            EXAMPLE_TRACE("unsubscribe timeout, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_UNSUBCRIBE_NACK:
            EXAMPLE_TRACE("unsubscribe nack, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_SUCCESS:
            EXAMPLE_TRACE("publish success, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_TIMEOUT:
            EXAMPLE_TRACE("publish timeout, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_NACK:
            EXAMPLE_TRACE("publish nack, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_RECEIVED:
            EXAMPLE_TRACE("topic message arrived but without any related handle: topic=%.*s, topic_msg=%.*s",
                          topic_info->topic_len,
                          topic_info->ptopic,
                          topic_info->payload_len,
                          topic_info->payload);
            break;

        default:
            EXAMPLE_TRACE("Should NOT arrive here.");
            break;
    }
}

void utest_mqtt_recvmsg_callback(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    iotx_mqtt_topic_info_pt ptopic_info = (iotx_mqtt_topic_info_pt) msg->msg;
    recvmsg_cnt++;

    // print topic name and topic message
    EXAMPLE_TRACE("----");
    EXAMPLE_TRACE("Topic: '%.*s' (Length: %d)",
                  ptopic_info->topic_len,
                  ptopic_info->ptopic,
                  ptopic_info->topic_len);
    EXAMPLE_TRACE("Payload: '%.*s' (Length: %d)",
                  ptopic_info->payload_len,
                  ptopic_info->payload,
                  ptopic_info->payload_len);
    EXAMPLE_TRACE("recvmsg_cnt is : %ld", recvmsg_cnt);
    EXAMPLE_TRACE("----");
}

void *utest_mqtt_init(void)
{
    void             *pclient = NULL;
    iotx_conn_info_pt pconn_info = NULL;
    iotx_mqtt_param_t mqtt_params;
    int start_time = 0;
    int ret = 0;

    start_time = HAL_UptimeMs();
    ret = IOT_SetupConnInfo(UTEST_MQTT_PRODUCT_KEY, UTEST_MQTT_DEVICE_NAME, UTEST_MQTT_DEVICE_SECRET, (void **)&pconn_info);
    if (0 != ret) {
        return NULL;
    }
    printf("setup connection...%ld ms\n", HAL_UptimeMs() - start_time);

    /* Initialize MQTT parameter */
    memset(&mqtt_params, 0x0, sizeof(mqtt_params));
    mqtt_params.port = pconn_info->port;
    mqtt_params.host = pconn_info->host_name;
    mqtt_params.client_id = pconn_info->client_id;
    mqtt_params.username = pconn_info->username;
    mqtt_params.password = pconn_info->password;
    mqtt_params.pub_key = pconn_info->pub_key;
    mqtt_params.request_timeout_ms = 2000;
    mqtt_params.clean_session = 1;
    mqtt_params.keepalive_interval_ms = 60000;
    mqtt_params.read_buf_size = MSG_LEN_MAX;
    mqtt_params.write_buf_size = MSG_LEN_MAX;
    mqtt_params.handle_event.h_fp = utest_mqtt_event_handle;
    mqtt_params.handle_event.pcontext = NULL;

    printf("mqtt_params.port = %d\n", mqtt_params.port);
    printf("mqtt_params.host = %s\n", mqtt_params.host);
    printf("mqtt_params.client_id = %s\n", mqtt_params.client_id);
    printf("mqtt_params.username = %s\n", mqtt_params.username);
    printf("mqtt_params.password = %s\n", mqtt_params.password);
    printf("mqtt_params.pub_key = %s\n", mqtt_params.pub_key);
    printf("mqtt_params.request_timeout_ms = %d\n", mqtt_params.request_timeout_ms);
    printf("mqtt_params.clean_session = %d\n", mqtt_params.clean_session);
    printf("mqtt_params.keepalive_interval_ms = %d\n", mqtt_params.keepalive_interval_ms);
    printf("mqtt_params.read_buf_size = %d\n", mqtt_params.read_buf_size);
    printf("mqtt_params.write_buf_size = %d\n", mqtt_params.write_buf_size);

    /* Construct a MQTT client with specify parameter */
    start_time = HAL_UptimeMs();
    pclient = IOT_MQTT_Construct(&mqtt_params);
    printf("construct mqtt client...%ld ms\n", HAL_UptimeMs() - start_time);

    return pclient;
}

void *mqtt_init_by_dev(int clean_session)
{
    void *pclient = NULL;
    iotx_conn_info_pt pconn_info = NULL;
    iotx_mqtt_param_t mqtt_params;
    int start_time = 0;
    int ret = 0;

    start_time = HAL_UptimeMs();
    ret = IOT_SetupConnInfo(UTEST_MQTT_PRODUCT_KEY, UTEST_MQTT_DEVICE_NAME, UTEST_MQTT_DEVICE_SECRET, (void **)&pconn_info);
    if (0 != ret) {
        ASSERT_FAIL();
        return NULL;
    }
    HAL_Printf("setup connection...%d ms\n", HAL_UptimeMs() - start_time);

    /* Initialize MQTT parameter */
    memset(&mqtt_params, 0x0, sizeof(mqtt_params));
    mqtt_params.port = pconn_info->port;
    mqtt_params.host = pconn_info->host_name;
    mqtt_params.client_id = pconn_info->client_id;
    mqtt_params.username = pconn_info->username;
    mqtt_params.password = pconn_info->password;
    mqtt_params.pub_key = pconn_info->pub_key;
    mqtt_params.request_timeout_ms = 2000;
    mqtt_params.clean_session = clean_session;
    mqtt_params.keepalive_interval_ms = 60000;
    //mqtt_params.pread_buf = msg_readbuf;
    mqtt_params.read_buf_size = MSG_LEN_MAX;
    //mqtt_params.pwrite_buf = msg_buf;
    mqtt_params.write_buf_size = MSG_LEN_MAX;
    mqtt_params.handle_event.h_fp = utest_mqtt_event_handle;// iotx_event_handle_t;// mqtt_event_handle;
    mqtt_params.handle_event.pcontext = NULL;
    start_time = HAL_UptimeMs();
    pclient = IOT_MQTT_Construct(&mqtt_params);
    ASSERT_NOT_NULL(pclient);
    HAL_Printf("construct mqtt client...%d ms\n", HAL_UptimeMs() - start_time);

    return pclient;
}

#endif // MQTT_COMM_ENABLED

