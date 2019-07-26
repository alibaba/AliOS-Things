#include "cut.h"
#include "utest_mqtt.h"

#if defined(MQTT_COMM_ENABLED)

static void recvmsg_topic0_callback(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    iotx_mqtt_topic_info_pt ptopic_info = (iotx_mqtt_topic_info_pt) msg->msg;

    printf("----callback 0\n");
    printf("Topic: '%.*s' (Length: %d)\n",
           ptopic_info->topic_len,
           ptopic_info->ptopic,
           ptopic_info->topic_len);
    printf("Payload: '%.*s' (Length: %d)\n",
           ptopic_info->payload_len,
           ptopic_info->payload,
           ptopic_info->payload_len);
    printf("----\n");
    recvmsg_cnt++;
}


/* utest_mqtt_sub每个case都会调用CASE_SETUP和CASE_TEARDOWN */
DATA(MQTT_SUB)
{
    /* disable compiler error */
    void *null;
};

SETUP(MQTT_SUB)
{
    recvmsg_cnt = 0;
    memset(msg_buf, 0, MSG_LEN_MAX);
    memset(msg_readbuf, 0, MSG_LEN_MAX);
    memset(msg_pub, 0, MSG_LEN_MAX);

    TEST_REPLACE_DEVCERT(&UTEST_MQTT_PRODUCT_KEY, &UTEST_MQTT_PRODUCT_SECRET, &UTEST_MQTT_DEVICE_NAME,
                         &UTEST_MQTT_DEVICE_SECRET);
    utest_mqtt_update_topic();
}

TEARDOWN(MQTT_SUB)
{
    recvmsg_cnt = 0;
    /* In order to unsubscribe all the topics that were subscribed offline */
    void *tmp_pclient = utest_mqtt_init();
    IOT_MQTT_Destroy(&tmp_pclient);
}


CASEs(MQTT_SUB, case_01_presub)
{
    int ret = -1;
    ret = IOT_MQTT_Subscribe(NULL, UTEST_TOPIC_DATA, IOTX_MQTT_QOS1, utest_mqtt_recvmsg_callback, NULL);
    ASSERT_TRUE(ret >= 0);
}

CASEs(MQTT_SUB, case_02_presub)
{
    int ret = -1;
    ret = IOT_MQTT_Subscribe(NULL, NULL, IOTX_MQTT_QOS1, utest_mqtt_recvmsg_callback, NULL);
    ASSERT_TRUE(ret < 0);
}

CASEs(MQTT_SUB, case_03_presub)
{
    int ret = -1;
    ret = IOT_MQTT_Subscribe(NULL, UTEST_TOPIC_DATA, IOTX_MQTT_QOS1, NULL, NULL);
    ASSERT_TRUE(ret < 0);
}

// 提前离线订阅
CASEs(MQTT_SUB, case_04_presub)
{
    int ret = -1;
    void *pclient = NULL;
    int cnt = 0;
    iotx_mqtt_topic_info_t topic_msg;

    ret = IOT_MQTT_Subscribe(pclient, UTEST_TOPIC_DATA, IOTX_MQTT_QOS1, recvmsg_topic0_callback, NULL);
    ASSERT_TRUE(ret >= 0);

    pclient = utest_mqtt_init();
    ASSERT_NOT_NULL(pclient);

    recvmsg_cnt = 0;
    do {
        cnt++;
        int msg_len = snprintf(msg_pub, sizeof(msg_pub), "{\"attr_name\":\"temperature\", \"attr_value\":\"%d\"}", cnt);
        if (msg_len < 0) {
            break;
        }

        /* Initialize topic information */
        memset(&topic_msg, 0x0, sizeof(iotx_mqtt_topic_info_t));

        topic_msg.qos = IOTX_MQTT_QOS1;
        topic_msg.retain = 0;
        topic_msg.dup = 0;
        topic_msg.payload = (void *)msg_pub;
        topic_msg.payload_len = strlen(msg_pub);
        ret = IOT_MQTT_Publish(pclient, UTEST_TOPIC_DATA, &topic_msg);

        /* handle the MQTT packet received from TCP or SSL connection */
        IOT_MQTT_Yield(pclient, 200);
        printf("TX=%d, RX=%ld\n", cnt, recvmsg_cnt);
    } while (cnt < 10);
    int wait_time = 10;
    while (recvmsg_cnt < cnt && --wait_time) {
        IOT_MQTT_Yield(pclient, 2000);
    }
    ASSERT_EQ(cnt, recvmsg_cnt);
    IOT_MQTT_Unsubscribe(pclient, UTEST_TOPIC_DATA);
    IOT_MQTT_Destroy(&pclient);
}

// 同步订阅, 简易发送
CASEs(MQTT_SUB, case_05_presub)
{
    int ret = -1;
    void *pclient = NULL;
    int cnt = 0;
    pclient = utest_mqtt_init();
    ASSERT_NOT_NULL(pclient);

    ret = IOT_MQTT_Subscribe_Sync(pclient, UTEST_TOPIC_DATA, IOTX_MQTT_QOS1, recvmsg_topic0_callback, NULL, 5000);
    ASSERT_TRUE(ret >= 0);
    recvmsg_cnt = 0;
    do {
        cnt++;
        int msg_len = snprintf(msg_pub, sizeof(msg_pub), "{\"attr_name\":\"temperature\", \"attr_value\":\"%d\"}", cnt);
        if (msg_len < 0) {
            break;
        }
        ret = IOT_MQTT_Publish_Simple(pclient, UTEST_TOPIC_DATA, 1, msg_pub, msg_len);

        IOT_MQTT_Yield(pclient, 200);
        printf("TX=%d, RX=%ld\n", cnt, recvmsg_cnt);
    } while (cnt < 10);
    int wait_time = 10;
    while (recvmsg_cnt < cnt && --wait_time) {
        IOT_MQTT_Yield(pclient, 1000);
    }
    ASSERT_EQ(cnt, recvmsg_cnt);
    IOT_MQTT_Unsubscribe(pclient, UTEST_TOPIC_DATA);
    IOT_MQTT_Destroy(&pclient);
}

/* IOT_MQTT_Subscribe:handle=NULL */
CASEs(MQTT_SUB, case_06_presub)
{
    int ret = -1;
    ret = IOT_MQTT_Subscribe(NULL, UTEST_TOPIC_DATA, IOTX_MQTT_QOS1, utest_mqtt_recvmsg_callback, NULL);
    ASSERT_TRUE(ret == 0);
}

/* IOT_MQTT_Subscribe:topic=NULL */
CASEs(MQTT_SUB, case_07)
{
    int ret = -1;
    void *pclient = NULL;
    pclient = utest_mqtt_init();
    ASSERT_NOT_NULL(pclient);
    ret = IOT_MQTT_Subscribe(pclient, NULL, IOTX_MQTT_QOS1, utest_mqtt_recvmsg_callback, NULL);
    if (ret >= 0) {
        IOT_MQTT_Unsubscribe(pclient, UTEST_TOPIC_DATA);
        IOT_MQTT_Destroy(&pclient);
        ASSERT_FAIL();
    }
    IOT_MQTT_Destroy(&pclient);
}
/* IOT_MQTT_Subscribe:topic非法字符串 */
CASEs(MQTT_SUB, case_08)
{
    int ret = -1;
    void *pclient = NULL;

    pclient = utest_mqtt_init();
    ASSERT_NOT_NULL(pclient);
    ret = IOT_MQTT_Subscribe(pclient, "invalid_topic_filter", IOTX_MQTT_QOS1, utest_mqtt_recvmsg_callback, NULL);
    if (ret >= 0) {
        IOT_MQTT_Unsubscribe(pclient, UTEST_TOPIC_DATA);
        IOT_MQTT_Destroy(&pclient);
        ASSERT_FAIL();
    }
    IOT_MQTT_Destroy(&pclient);
}

/* IOT_MQTT_Subscribe:topic长字符串 */
CASEs(MQTT_SUB, case_09)
{
    int ret = -1;
    void *pclient = NULL;

    pclient = utest_mqtt_init();
    ASSERT_NOT_NULL(pclient);

    ret = IOT_MQTT_Subscribe(pclient,
                             "/long_topic_filter_long_topic_filter_long_topic_filter_"
                             "/long_topic_filter_long_topic_filter_long_topic_filter_"
                             "/long_topic_filter_long_topic_filter_long_topic_filter_"
                             "long_topic_filter_long_topic_filter_long_topic_filter_",
                             IOTX_MQTT_QOS1, utest_mqtt_recvmsg_callback, NULL);
    if (ret >= 0) {
        IOT_MQTT_Unsubscribe(pclient, UTEST_TOPIC_DATA);
        IOT_MQTT_Destroy(&pclient);
        ASSERT_FAIL();
    }
    IOT_MQTT_Destroy(&pclient);
}
/* IOT_MQTT_Subscribe:qos整形非法 */
/* illegal values would be transformed into default value 0 */
/* Therefore, illegal qos values would succeeded anyway*/
CASEs(MQTT_SUB, case_12)
{
    int ret = -1;
    void *pclient = NULL;
    pclient = utest_mqtt_init();
    ASSERT_NOT_NULL(pclient);

    ret = IOT_MQTT_Subscribe(pclient, UTEST_TOPIC_DATA, (iotx_mqtt_qos_t) - 1, utest_mqtt_recvmsg_callback, NULL);
    if (ret < 0) {
        IOT_MQTT_Unsubscribe(pclient, UTEST_TOPIC_DATA);
        IOT_MQTT_Destroy(&pclient);
        ASSERT_FAIL();
    }
    IOT_MQTT_Destroy(&pclient);
}

/* IOT_MQTT_Subscribe:重复订阅 */
CASEs(MQTT_SUB, case_10)
{
    int ret = -1;
    void *pclient = NULL;

    pclient = utest_mqtt_init();
    ASSERT_NOT_NULL(pclient);

    ret = IOT_MQTT_Subscribe(pclient, UTEST_TOPIC_DATA, IOTX_MQTT_QOS1, utest_mqtt_recvmsg_callback, NULL);
    if (ret < 0) {
        IOT_MQTT_Destroy(&pclient);
        ASSERT_FAIL();
    }

    ret = IOT_MQTT_Subscribe(pclient, UTEST_TOPIC_DATA, IOTX_MQTT_QOS0, utest_mqtt_recvmsg_callback, NULL);
    if (ret < 0) {
        IOT_MQTT_Unsubscribe(pclient, UTEST_TOPIC_DATA);
        IOT_MQTT_Destroy(&pclient);
        ASSERT_FAIL();
    }
    IOT_MQTT_Unsubscribe(pclient, UTEST_TOPIC_DATA);
    IOT_MQTT_Destroy(&pclient);
}

/* IOT_MQTT_Subscribe:不存在的topic */
CASEs(MQTT_SUB, case_11)
{
    int ret = -1;
    void *pclient = NULL;
    int cnt = 10;
    iotx_mqtt_topic_info_t topic_msg;

    pclient = utest_mqtt_init();
    ASSERT_NOT_NULL(pclient);

    ret = IOT_MQTT_Subscribe(pclient, UTEST_TOPIC_NOTAUTHED, IOTX_MQTT_QOS1, utest_mqtt_recvmsg_callback, NULL);
    if (ret < 0) {
        IOT_MQTT_Unsubscribe(pclient, UTEST_TOPIC_NOTAUTHED);
        IOT_MQTT_Destroy(&pclient);
        ASSERT_FAIL();
    }

    topic_msg.qos = IOTX_MQTT_QOS1;
    topic_msg.retain = 0;
    topic_msg.dup = 0;
    topic_msg.payload = (void *)msg_pub;

    while (--cnt) {
        IOT_MQTT_Yield(pclient, 1000);

        HAL_Snprintf(msg_pub, sizeof(msg_pub), "{\"attr_name\":\"temperature\", \"attr_value\":\"%d\"}", 1);
        topic_msg.payload_len = strlen(msg_pub);
        ret = IOT_MQTT_Publish(pclient, UTEST_TOPIC_NOTAUTHED, &topic_msg);
        ASSERT_TRUE(ret >= 0);
        /* no message should be received since the topic is not authorized */
        ASSERT_TRUE(recvmsg_cnt == 0);
    }

    ret = IOT_MQTT_Unsubscribe(pclient, UTEST_TOPIC_NOTAUTHED);
    if (ret < 0) {
        IOT_MQTT_Destroy(&pclient);
        ASSERT_FAIL();
    }
    IOT_MQTT_Destroy(&pclient);
}

SUITE(MQTT_SUB) = {
    ADD_CASE(MQTT_SUB, case_01_presub),
    ADD_CASE(MQTT_SUB, case_02_presub),
    ADD_CASE(MQTT_SUB, case_03_presub),
    ADD_CASE(MQTT_SUB, case_04_presub),
    ADD_CASE(MQTT_SUB, case_05_presub),
    ADD_CASE(MQTT_SUB, case_06_presub),
    ADD_CASE(MQTT_SUB, case_07),
    ADD_CASE(MQTT_SUB, case_08),
    ADD_CASE(MQTT_SUB, case_09),
    ADD_CASE(MQTT_SUB, case_10),
    ADD_CASE(MQTT_SUB, case_11),
    ADD_CASE(MQTT_SUB, case_12),
    ADD_CASE_NULL
};


#endif
