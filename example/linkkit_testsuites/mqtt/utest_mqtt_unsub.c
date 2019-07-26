#if defined(MQTT_COMM_ENABLED)
#include "sdk-testsuites_internal.h"
#include "cut.h"

#include "utest_mqtt.h"

/**************************************************************************************/
/* iot_mqtt_unsubscribe */
/**************************************************************************************/

/* IOT_MQTT_Unsubscribe:正常取消订阅 */
CASE(MQTT_UNSUB, case_01)
{
    int ret = -1;
    void *pclient = NULL;

    TEST_REPLACE_DEVCERT(&UTEST_MQTT_PRODUCT_KEY, &UTEST_MQTT_PRODUCT_SECRET, &UTEST_MQTT_DEVICE_NAME,
                         &UTEST_MQTT_DEVICE_SECRET);
    utest_mqtt_update_topic();

    pclient = mqtt_init_by_dev(0);
    ASSERT_NOT_NULL(pclient);

    ret = IOT_MQTT_Subscribe(pclient, UTEST_TOPIC_DATA, IOTX_MQTT_QOS1, utest_mqtt_recvmsg_callback, NULL);
    if (ret < 0) {
        IOT_MQTT_Unsubscribe(pclient, UTEST_TOPIC_DATA);
        IOT_MQTT_Destroy(&pclient);
        ASSERT_FAIL();
    }
    ret = IOT_MQTT_Unsubscribe(pclient, UTEST_TOPIC_DATA);
    if (ret < 0) {
        IOT_MQTT_Destroy(&pclient);
        ASSERT_FAIL();
    }
    IOT_MQTT_Destroy(&pclient);
}

/* IOT_MQTT_Unsubscribe:handle=NULL */
CASE(MQTT_UNSUB, case_02)
{
    int ret = -1;
    void *pclient = NULL;

    TEST_REPLACE_DEVCERT(&UTEST_MQTT_PRODUCT_KEY, &UTEST_MQTT_PRODUCT_SECRET, &UTEST_MQTT_DEVICE_NAME,
                         &UTEST_MQTT_DEVICE_SECRET);
    utest_mqtt_update_topic();

    ret = IOT_MQTT_Unsubscribe(pclient, UTEST_TOPIC_DATA);
    if (ret >= 0) {
        ASSERT_FAIL();
    }
}

/* IOT_MQTT_Unsubscribe:topic_filter=NULL */
CASE(MQTT_UNSUB, case_03)
{
    int ret = -1;
    void *pclient = NULL;

    TEST_REPLACE_DEVCERT(&UTEST_MQTT_PRODUCT_KEY, &UTEST_MQTT_PRODUCT_SECRET, &UTEST_MQTT_DEVICE_NAME,
                         &UTEST_MQTT_DEVICE_SECRET);
    utest_mqtt_update_topic();

    pclient = mqtt_init_by_dev(0);
    ASSERT_NOT_NULL(pclient);

    ret = IOT_MQTT_Unsubscribe(pclient, NULL);
    if (ret >= 0) {
        IOT_MQTT_Destroy(&pclient);
        ASSERT_FAIL();
    }
    IOT_MQTT_Destroy(&pclient);
}

/* IOT_MQTT_Unsubscribe:topic_filter='' */
CASE(MQTT_UNSUB, case_04)
{
    int ret = -1;
    void *pclient = NULL;

    TEST_REPLACE_DEVCERT(&UTEST_MQTT_PRODUCT_KEY, &UTEST_MQTT_PRODUCT_SECRET, &UTEST_MQTT_DEVICE_NAME,
                         &UTEST_MQTT_DEVICE_SECRET);
    utest_mqtt_update_topic();

    pclient = mqtt_init_by_dev(0);
    ASSERT_NOT_NULL(pclient);

    ret = IOT_MQTT_Unsubscribe(pclient, "");
    if (ret >= 0) {
        IOT_MQTT_Destroy(&pclient);
        ASSERT_FAIL();
    }
    IOT_MQTT_Destroy(&pclient);
}

/* IOT_MQTT_Unsubscribe:topic_filter=UTEST_TOPIC_DATA */
CASE(MQTT_UNSUB, case_05)
{
    int ret = -1;
    void *pclient = NULL;

    TEST_REPLACE_DEVCERT(&UTEST_MQTT_PRODUCT_KEY, &UTEST_MQTT_PRODUCT_SECRET, &UTEST_MQTT_DEVICE_NAME,
                         &UTEST_MQTT_DEVICE_SECRET);
    utest_mqtt_update_topic();

    pclient = mqtt_init_by_dev(0);
    ASSERT_NOT_NULL(pclient);

    ret = IOT_MQTT_Unsubscribe(pclient, UTEST_TOPIC_DATA);
    if (ret < 0) {
        IOT_MQTT_Destroy(&pclient);
        ASSERT_FAIL();
    }
    IOT_MQTT_Destroy(&pclient);
}

/* unsub using null handle test */
CASE(MQTT_UNSUB, case_06_unsub)
{
    int ret = -1;

    TEST_REPLACE_DEVCERT(&UTEST_MQTT_PRODUCT_KEY, &UTEST_MQTT_PRODUCT_SECRET, &UTEST_MQTT_DEVICE_NAME,
                         &UTEST_MQTT_DEVICE_SECRET);
    utest_mqtt_update_topic();

    ret = IOT_MQTT_Unsubscribe(NULL, UTEST_TOPIC_DATA);
    ASSERT_TRUE(ret < 0);
    void *pclient = utest_mqtt_init();
    ASSERT_NOT_NULL(pclient);
    ret = IOT_MQTT_Unsubscribe(NULL, UTEST_TOPIC_DATA);
    ASSERT_TRUE(ret >= 0);
    IOT_MQTT_Destroy(&pclient);
}

SUITE(MQTT_UNSUB) = {
    ADD_CASE(MQTT_UNSUB, case_01),
    ADD_CASE(MQTT_UNSUB, case_02),
    ADD_CASE(MQTT_UNSUB, case_03),
    ADD_CASE(MQTT_UNSUB, case_04),
    ADD_CASE(MQTT_UNSUB, case_05),
    ADD_CASE(MQTT_UNSUB, case_06_unsub),
    ADD_CASE_NULL
};
#endif
