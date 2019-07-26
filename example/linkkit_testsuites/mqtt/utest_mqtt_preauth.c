#if !defined(MQTT_DIRECT)
#if defined(MQTT_COMM_ENABLED)
#include "sdk-testsuites_internal.h"
#include "cut.h"

#include "utest_mqtt.h"
/**************************************************************************************/
/* IOT_MQTT_Publish */
/**************************************************************************************/
#if 1
/* IOT_MQTT_Publish:正常 */
CASE(MQTT_PREAUTH, case_01)
{
    int ret = -1;
    void *pclient = NULL;
    iotx_mqtt_topic_info_t topic;

    TEST_REPLACE_DEVCERT(&UTEST_MQTT_PRODUCT_KEY, &UTEST_MQTT_PRODUCT_SECRET, &UTEST_MQTT_DEVICE_NAME,
                         &UTEST_MQTT_DEVICE_SECRET);
    utest_mqtt_update_topic();

    pclient = mqtt_init_by_dev(0);
    ASSERT_NOT_NULL(pclient);

    HAL_Snprintf(msg_pub, MSG_LEN_MAX, "{\"attr_name\":\"temperature\", \"attr_value\":\"199\"}");
    memset(&topic, 0x0, sizeof(iotx_mqtt_topic_info_t));
    topic.qos = IOTX_MQTT_QOS1;
    topic.retain = 0;
    topic.dup = 0;
    topic.payload = msg_buf;
    topic.payload_len = strlen(msg_buf);

    ret = IOT_MQTT_Publish(pclient, UTEST_TOPIC_DATA, &topic);
    if (NULL != pclient) {
        IOT_MQTT_Destroy(&pclient);
    }
    ASSERT_TRUE(ret >= 0);
}
#endif

/* IOT_MQTT_Publish:handle=NULL */
CASE(MQTT_PREAUTH, case_02)
{
    int ret = -1;
    void *pclient = NULL;
    iotx_mqtt_topic_info_t topic;

    TEST_REPLACE_DEVCERT(&UTEST_MQTT_PRODUCT_KEY, &UTEST_MQTT_PRODUCT_SECRET, &UTEST_MQTT_DEVICE_NAME,
                         &UTEST_MQTT_DEVICE_SECRET);
    utest_mqtt_update_topic();

    HAL_Snprintf(msg_pub, MSG_LEN_MAX, "{\"attr_name\":\"temperature\", \"attr_value\":\"199\"}");
    memset(&topic, 0x0, sizeof(iotx_mqtt_topic_info_t));
    topic.qos = IOTX_MQTT_QOS1;
    topic.retain = 0;
    topic.dup = 0;
    topic.payload = msg_buf;
    topic.payload_len = strlen(msg_buf);

    ret = IOT_MQTT_Publish(pclient, UTEST_TOPIC_DATA, &topic);
    ASSERT_TRUE(ret < 0);
}

/* IOT_MQTT_Publish:payload_len has some margine */
/* the payload_len is larger than the actual size, small than critial value */
CASE(MQTT_PREAUTH, case_03)
{
    int ret = -1;
    void *pclient = NULL;
    iotx_mqtt_topic_info_t topic;

    TEST_REPLACE_DEVCERT(&UTEST_MQTT_PRODUCT_KEY, &UTEST_MQTT_PRODUCT_SECRET, &UTEST_MQTT_DEVICE_NAME,
                         &UTEST_MQTT_DEVICE_SECRET);
    utest_mqtt_update_topic();

    pclient = mqtt_init_by_dev(0);
    ASSERT_NOT_NULL(pclient);

    HAL_Snprintf(msg_pub, MSG_LEN_MAX, "{\"attr_name\":\"temperature\", \"attr_value\":\"199\"}");
    memset(&topic, 0x0, sizeof(iotx_mqtt_topic_info_t));
    topic.qos = IOTX_MQTT_QOS1;
    topic.retain = 0;
    topic.dup = 0;
    topic.payload = msg_buf;
    topic.payload_len = MSG_LEN_MAX - strlen(UTEST_TOPIC_DATA) - 4 - 1 - 2 - 30;

    ret = IOT_MQTT_Publish(pclient, UTEST_TOPIC_DATA, &topic);
    if (NULL != pclient) {
        IOT_MQTT_Destroy(&pclient);
    }
    ASSERT_TRUE(ret >= 0);
}
#if 1
/* IOT_MQTT_Publish:payload_len is too large*/
CASE(MQTT_PREAUTH, case_04)
{
    int ret = -1;
    void *pclient = NULL;
    iotx_mqtt_topic_info_t topic;

    TEST_REPLACE_DEVCERT(&UTEST_MQTT_PRODUCT_KEY, &UTEST_MQTT_PRODUCT_SECRET, &UTEST_MQTT_DEVICE_NAME,
                         &UTEST_MQTT_DEVICE_SECRET);
    utest_mqtt_update_topic();

    pclient = mqtt_init_by_dev(0);
    ASSERT_NOT_NULL(pclient);

    HAL_Snprintf(msg_pub, MSG_LEN_MAX,  "{\"attr_name\":\"temperature\", \"attr_value\":\"199\"}");
    memset(&topic, 0x0, sizeof(iotx_mqtt_topic_info_t));
    topic.qos = IOTX_MQTT_QOS1;
    topic.retain = 0;
    topic.dup = 0;
    topic.payload = msg_buf;
    topic.payload_len = MSG_LEN_MAX;

    ret = IOT_MQTT_Publish(pclient, UTEST_TOPIC_DATA, &topic);
    if (NULL != pclient) {
        IOT_MQTT_Destroy(&pclient);
    }
    ASSERT_TRUE(ret == MQTT_PUBLISH_PACKET_ERROR);
}
#endif

#if 1
/* IOT_MQTT_Publish:payload=NULL */
CASE(MQTT_PREAUTH, case_05)
{
    int ret = -1;
    void *pclient = NULL;
    iotx_mqtt_topic_info_t topic;

    TEST_REPLACE_DEVCERT(&UTEST_MQTT_PRODUCT_KEY, &UTEST_MQTT_PRODUCT_SECRET, &UTEST_MQTT_DEVICE_NAME,
                         &UTEST_MQTT_DEVICE_SECRET);
    utest_mqtt_update_topic();

    pclient = mqtt_init_by_dev(0);
    ASSERT_NOT_NULL(pclient);

    HAL_Snprintf(msg_pub, MSG_LEN_MAX,  "{\"attr_name\":\"temperature\", \"attr_value\":\"199\"}");
    memset(&topic, 0x0, sizeof(iotx_mqtt_topic_info_t));
    topic.qos = IOTX_MQTT_QOS1;
    topic.retain = 0;
    topic.dup = 0;
    topic.payload = NULL;
    topic.payload_len = MSG_LEN_MAX - strlen(UTEST_TOPIC_DATA) - 4 - 3;

    ret = IOT_MQTT_Publish(pclient, UTEST_TOPIC_DATA, &topic);
    if (NULL != pclient) {
        IOT_MQTT_Destroy(&pclient);
    }

    ASSERT_TRUE(ret < 0);
}
#endif

//the payload len is critical
CASE(MQTT_PREAUTH, case_06)
{
    int ret = -1;
    void *pclient = NULL;
    iotx_mqtt_topic_info_t topic;

    TEST_REPLACE_DEVCERT(&UTEST_MQTT_PRODUCT_KEY, &UTEST_MQTT_PRODUCT_SECRET, &UTEST_MQTT_DEVICE_NAME,
                         &UTEST_MQTT_DEVICE_SECRET);
    utest_mqtt_update_topic();

    pclient = mqtt_init_by_dev(0);
    ASSERT_NOT_NULL(pclient);

    HAL_Snprintf(msg_pub, MSG_LEN_MAX,  "{\"attr_name\":\"temperature\", \"attr_value\":\"199\"}");
    memset(&topic, 0x0, sizeof(iotx_mqtt_topic_info_t));
    topic.qos = IOTX_MQTT_QOS1;
    topic.retain = 0;
    topic.dup = 0;
    topic.payload = msg_buf;
    //here 4 means lens of idenfiter, topic len, header flags; 3 mins msg len(1 byte) + 2 byte margin
    topic.payload_len = MSG_LEN_MAX - strlen(UTEST_TOPIC_DATA) - 4 - 3;

    ret = IOT_MQTT_Publish(pclient, UTEST_TOPIC_DATA, &topic);
    if (NULL != pclient) {
        IOT_MQTT_Destroy(&pclient);
    }
    ASSERT_TRUE(ret >= 0);
}

/* if the len is smaller than critical value */
CASE(MQTT_PREAUTH, case_07)
{
    int ret = -1;
    void *pclient = NULL;
    iotx_mqtt_topic_info_t topic;

    TEST_REPLACE_DEVCERT(&UTEST_MQTT_PRODUCT_KEY, &UTEST_MQTT_PRODUCT_SECRET, &UTEST_MQTT_DEVICE_NAME,
                         &UTEST_MQTT_DEVICE_SECRET);
    utest_mqtt_update_topic();

    pclient = mqtt_init_by_dev(0);
    ASSERT_NOT_NULL(pclient);

    HAL_Snprintf(msg_pub, MSG_LEN_MAX,  "{\"attr_name\":\"temperature\", \"attr_value\":\"199\"}");
    memset(&topic, 0x0, sizeof(iotx_mqtt_topic_info_t));
    topic.qos = IOTX_MQTT_QOS1;
    topic.retain = 0;
    topic.dup = 0;
    topic.payload = msg_buf;
    //here 4 means lens of idenfiter, topic len, header flags; 3 mins msg len(1 byte) + 2 byte margin
    //here remove the 2 byte margin, which will relult in error
    topic.payload_len = MSG_LEN_MAX - strlen(UTEST_TOPIC_DATA) - 4 - 3 + 1;

    ret = IOT_MQTT_Publish(pclient, UTEST_TOPIC_DATA, &topic);
    if (NULL != pclient) {
        IOT_MQTT_Destroy(&pclient);
    }
    ASSERT_TRUE(ret < 0);
}

SUITE(MQTT_PREAUTH) = {
    ADD_CASE(MQTT_PREAUTH, case_01),
    ADD_CASE(MQTT_PREAUTH, case_02),
    ADD_CASE(MQTT_PREAUTH, case_03),
    ADD_CASE(MQTT_PREAUTH, case_04),
    ADD_CASE(MQTT_PREAUTH, case_05),
    ADD_CASE(MQTT_PREAUTH, case_06),
    ADD_CASE(MQTT_PREAUTH, case_07),
    ADD_CASE_NULL
};
#endif
#endif
