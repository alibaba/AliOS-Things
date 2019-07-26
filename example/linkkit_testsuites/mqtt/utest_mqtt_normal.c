#if defined(MQTT_COMM_ENABLED)
#include "sdk-testsuites_internal.h"
#include "cut.h"

#include "utest_mqtt.h"

/**************************************************************************************/
/* normal */
/**************************************************************************************/
DATA(MQTT_NORMAL)
{
    void *mqtt;
    iotx_mqtt_topic_info_t *topic;
};

/* utest_mqtt_normal每个case都会调用CASE_SETUP和CASE_TEARDOWN */
SETUP(MQTT_NORMAL)
{
    memset(msg_buf, 0, sizeof(msg_buf));
    memset(msg_readbuf, 0, sizeof(msg_readbuf));
    memset(msg_pub, 0, sizeof(msg_pub));

    TEST_REPLACE_DEVCERT(&UTEST_MQTT_PRODUCT_KEY, &UTEST_MQTT_PRODUCT_SECRET, &UTEST_MQTT_DEVICE_NAME,
                         &UTEST_MQTT_DEVICE_SECRET);
    utest_mqtt_update_topic();

    if (data->mqtt != NULL) {
        IOT_MQTT_Destroy(&(data->mqtt));
        data->mqtt = NULL;
    }
    data->mqtt = mqtt_init_by_dev(0);

    if (data->topic != NULL) {
        HAL_Free(data->topic);
        data->topic = NULL;
    }

    data->topic = (iotx_mqtt_topic_info_t *)HAL_Malloc(sizeof(iotx_mqtt_topic_info_t));
    memset(data->topic, 0, sizeof(iotx_mqtt_topic_info_t));
}

TEARDOWN(MQTT_NORMAL)
{
    if (data->mqtt != NULL) {
        IOT_MQTT_Destroy(&(data->mqtt));
        data->mqtt = NULL;
    }
    if (data->topic != NULL) {
        HAL_Free(data->topic);
        data->topic = NULL;
    }
}
#if 1
/* 自发自收 */
CASEs(MQTT_NORMAL, case_01)
{
    int ret = -1;
    int msg_len = 0;
    long cnt = 0;
    int wait_time = 20;
    void *pclient = data->mqtt;
    ASSERT_NOT_NULL(pclient);
    iotx_mqtt_topic_info_t *topic = (iotx_mqtt_topic_info_t *)HAL_Malloc(sizeof(iotx_mqtt_topic_info_t));
    ret = IOT_MQTT_Subscribe(pclient, UTEST_TOPIC_DATA0, IOTX_MQTT_QOS1, utest_mqtt_recvmsg_callback, NULL);
    ASSERT_TRUE(ret >= 0);

    topic->qos = IOTX_MQTT_QOS1;
    topic->retain = 0;
    topic->dup = 0;
    recvmsg_cnt = 0;
    IOT_MQTT_Yield(pclient, 1000);

    do {
        cnt++;
        msg_len = HAL_Snprintf(msg_pub, sizeof(msg_pub), "{\"attr_name\":\"temperature\", \"attr_value\":\"%d\"}", cnt);
        if (msg_len < 0) {
            break;
        }

        topic->payload = (void *)msg_pub;
        topic->payload_len = 100;
        HAL_Printf("entering IOT_MQTT_Publish\n");
        ret = IOT_MQTT_Publish(pclient, UTEST_TOPIC_DATA0, topic);
        HAL_Printf("leaving IOT_MQTT_Publish, TX=%ld, RX=%ld\n", cnt, recvmsg_cnt);

        /* handle the MQTT packet received from TCP or SSL connection */
        HAL_Printf("entering IOT_MQTT_Yield\n");
        IOT_MQTT_Yield(pclient, 1000);
        HAL_Printf("leaving IOT_MQTT_Yield\n\n");
    } while (cnt < 5);

    while (recvmsg_cnt < cnt && --wait_time) {
        IOT_MQTT_Yield(pclient, 1000);
        HAL_Printf("TX=%ld, RX=%ld\n", cnt, recvmsg_cnt);
    }
    HAL_Free(topic);
    ASSERT_EQ(cnt, recvmsg_cnt);
}
#endif

#if 1
/* 心跳 */
CASEs(MQTT_NORMAL, case_02)
{
    void *pclient = data->mqtt;
    char count = 3;
    int ret = -1;
    do {
        IOT_MQTT_Yield(pclient, 2000);
        ret = IOT_MQTT_CheckStateNormal(pclient);
        ASSERT_TRUE(ret >= 0);
    } while (count--);
}
#endif

#if 1
/* 间隔的sub unsub */
CASEs(MQTT_NORMAL, case_03)
{
    int ret = -1;
    int msg_len = 0;
    int cnt = 0;
    void *pclient = data->mqtt;
    iotx_mqtt_topic_info_t *topic = data->topic;

    ASSERT_NOT_NULL(pclient);

    /* Initialize topic information */
    strcpy(msg_pub, "message: hello! start!");
    topic->qos = IOTX_MQTT_QOS1;
    topic->retain = 0;
    topic->dup = 0;

    /* null payload is no longer supported, should return failure */
    ret = IOT_MQTT_Publish(pclient, UTEST_TOPIC_DATA0, topic);
    ASSERT_TRUE(ret < 0);

    do {
        cnt++;
        recvmsg_cnt = 0;

        if (cnt % 5 == 0) {
            ret = IOT_MQTT_Subscribe(pclient, UTEST_TOPIC_DATA0, IOTX_MQTT_QOS1, utest_mqtt_recvmsg_callback, NULL);
            ASSERT_TRUE(ret >= 0);
        }
        if (cnt % 10 == 0) {
            ret = IOT_MQTT_Unsubscribe(pclient, UTEST_TOPIC_DATA0);
            ASSERT_TRUE(ret >= 0);
        }

        msg_len = HAL_Snprintf(msg_pub, sizeof(msg_pub), "{\"attr_name\":\"temperature\", \"attr_value\":\"%d\"}", cnt);
        if (msg_len < 0) {
            break;
        }

        topic->payload = (void *)msg_pub;
        topic->payload_len = strlen(msg_pub);
        ret = IOT_MQTT_Publish(pclient, UTEST_TOPIC_DATA0, topic);
        IOT_MQTT_Yield(pclient, 1000);

        if ((cnt %  5 == 0) && (cnt % 10 != 0)) {
            /* package should be received if NOT unsubscribed */
            ASSERT_TRUE(recvmsg_cnt > 0);
        } else if (cnt % 10 == 0) {
            /* package should NOT be received if unsubscribed */
            ASSERT_TRUE(recvmsg_cnt == 0);
        }
    } while (cnt < 20);
    ret = IOT_MQTT_Unsubscribe(pclient, UTEST_TOPIC_DATA0);
    ASSERT_TRUE(ret >= 0);
}
#endif

/* 同时订阅多个topic */
CASEs(MQTT_NORMAL, case_04)
{
    int ret = -1;
    int msg_len = 0;
    int cnt = 10;
    void *pclient = data->mqtt;
    iotx_mqtt_topic_info_t *topic = data->topic;

    ASSERT_NOT_NULL(pclient);

    ret = IOT_MQTT_Subscribe(pclient, UTEST_TOPIC_DATA0, IOTX_MQTT_QOS1, utest_mqtt_recvmsg_callback, NULL);
    ASSERT_TRUE(ret >= 0);

    ret = IOT_MQTT_Subscribe(pclient, UTEST_TOPIC_DATA1, IOTX_MQTT_QOS1, utest_mqtt_recvmsg_callback, NULL);
    ASSERT_TRUE(ret >= 0);

    ret = IOT_MQTT_Subscribe(pclient, UTEST_TOPIC_DATA2, IOTX_MQTT_QOS1, utest_mqtt_recvmsg_callback, NULL);
    ASSERT_TRUE(ret >= 0);

    ret = IOT_MQTT_Subscribe(pclient, UTEST_TOPIC_DATA3, IOTX_MQTT_QOS1, utest_mqtt_recvmsg_callback, NULL);
    ASSERT_TRUE(ret >= 0);

    /* Initialize topic information */
    strcpy(msg_pub, "message: hello! start!");
    topic->qos = IOTX_MQTT_QOS1;
    topic->retain = 0;
    topic->dup = 0;

    while (--cnt) {
        /* Generate topic message */
        msg_len = HAL_Snprintf(msg_pub, sizeof(msg_pub), "{\"attr_name\":\"temperature\", \"attr_value\":\"%d\"}", cnt);
        if (msg_len < 0) {
            break;
        }

        topic->payload = (void *)msg_pub;
        topic->payload_len = strlen(msg_pub);

        ret = IOT_MQTT_Publish(pclient, UTEST_TOPIC_DATA0, topic);
        ASSERT_TRUE(ret >= 0);

        ret = IOT_MQTT_Publish(pclient, UTEST_TOPIC_DATA1, topic);
        ASSERT_TRUE(ret >= 0);

        ret = IOT_MQTT_Publish(pclient, UTEST_TOPIC_DATA2, topic);
        ASSERT_TRUE(ret >= 0);

        ret = IOT_MQTT_Publish(pclient, UTEST_TOPIC_DATA3, topic);
        ASSERT_TRUE(ret >= 0);

        IOT_MQTT_Yield(pclient, 200);
        HAL_SleepMs(500);
    };
    ret = IOT_MQTT_Unsubscribe(pclient, UTEST_TOPIC_DATA0);
    ASSERT_TRUE(ret >= 0);
    ret = IOT_MQTT_Unsubscribe(pclient, UTEST_TOPIC_DATA1);
    ASSERT_TRUE(ret >= 0);
    ret = IOT_MQTT_Unsubscribe(pclient, UTEST_TOPIC_DATA2);
    ASSERT_TRUE(ret >= 0);
    ret = IOT_MQTT_Unsubscribe(pclient, UTEST_TOPIC_DATA3);
    ASSERT_TRUE(ret >= 0);
}

#if 1
/* 重复订阅同一个topic */
CASEs(MQTT_NORMAL, case_05)
{
    int ret = -1;
    int msg_len = 0;
    int cnt = 2;
    void *pclient = data->mqtt;
    iotx_mqtt_topic_info_t *topic = data->topic;

    ASSERT_NOT_NULL(pclient);
    recvmsg_cnt = 0;

    while (cnt--) {
        ret = IOT_MQTT_Subscribe(pclient, UTEST_TOPIC_DATA0, IOTX_MQTT_QOS1, utest_mqtt_recvmsg_callback, NULL);
        ASSERT_TRUE(ret >= 0);
        IOT_MQTT_Yield(pclient, 500);
    }

    topic->qos = IOTX_MQTT_QOS1;
    topic->retain = 0;
    topic->dup = 0;

    cnt = 2;
    while (cnt--) {
        msg_len = HAL_Snprintf(msg_pub, sizeof(msg_pub), "{\"attr_name\":\"temperature\", \"attr_value\":\"%d\"}", cnt);
        if (msg_len < 0) {
            break;
        }

        topic->payload = (void *)msg_pub;
        topic->payload_len = strlen(msg_pub);

        ret = IOT_MQTT_Publish(pclient, UTEST_TOPIC_DATA0, topic);
        ASSERT_TRUE(ret >= 0);

        IOT_MQTT_Yield(pclient, 1000);
    };
    cnt = 5;
    printf("before: recvmsg_cnt = %ld\n", recvmsg_cnt);
    while (cnt-- && recvmsg_cnt != 4) {
        IOT_MQTT_Yield(pclient, 500);
    }
    IOT_MQTT_Unsubscribe(pclient, UTEST_TOPIC_DATA0);
    printf("recvmsg_cnt = %ld\n", recvmsg_cnt);
    ASSERT_TRUE(recvmsg_cnt == 2);
}
#endif

SUITE(MQTT_NORMAL) = {
    ADD_CASE(MQTT_NORMAL, case_01),
    ADD_CASE(MQTT_NORMAL, case_02),
    ADD_CASE(MQTT_NORMAL, case_03),
    ADD_CASE(MQTT_NORMAL, case_04),
    ADD_CASE(MQTT_NORMAL, case_05),
    ADD_CASE_NULL
};
#endif
