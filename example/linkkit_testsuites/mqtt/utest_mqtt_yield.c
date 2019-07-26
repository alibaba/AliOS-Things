#if defined(MQTT_COMM_ENABLED)
#include "sdk-testsuites_internal.h"
#include "cut.h"

#include "utest_mqtt.h"


/**************************************************************************************/
/* iot_mqtt_yield */
/**************************************************************************************/

/* IOT_MQTT_Yield: handle=NULL */
CASE(MQTT_YIELD, case_01)
{
    int ret = IOT_MQTT_Yield(NULL, 1000);
    ASSERT_TRUE(ret < 0);
}

/* IOT_MQTT_Yield: 期望时间与真实时间对比 */
CASE(MQTT_YIELD, case_02)
{
    void *pclient = NULL;
    int32_t start;
    int32_t wait_actual = 0;
    int32_t wait_expect = 3000;

    TEST_REPLACE_DEVCERT(&UTEST_MQTT_PRODUCT_KEY, &UTEST_MQTT_PRODUCT_SECRET, &UTEST_MQTT_DEVICE_NAME,
                         &UTEST_MQTT_DEVICE_SECRET);
    utest_mqtt_update_topic();

    pclient = mqtt_init_by_dev(0);
    ASSERT_NOT_NULL(pclient);

    start = HAL_UptimeMs();
    IOT_MQTT_Yield(pclient, wait_expect);
    wait_actual = HAL_UptimeMs() - start;

    IOT_MQTT_Destroy(&pclient);
    HAL_Printf("wait_expect = %d\n", wait_expect);
    HAL_Printf("wait_actual = %d\n", wait_actual);
    ASSERT_IN(wait_expect - 100, wait_actual, wait_expect + 100);
}

/* IOT_MQTT_Yield: timeout=0 */
CASE(MQTT_YIELD, case_03)
{
    void *pclient = NULL;
    int32_t start;
    int32_t wait_actual = 0;
    int32_t wait_expect = 0;

    TEST_REPLACE_DEVCERT(&UTEST_MQTT_PRODUCT_KEY, &UTEST_MQTT_PRODUCT_SECRET, &UTEST_MQTT_DEVICE_NAME,
                         &UTEST_MQTT_DEVICE_SECRET);
    utest_mqtt_update_topic();

    pclient = mqtt_init_by_dev(0);
    ASSERT_NOT_NULL(pclient);

    start = HAL_UptimeMs();
    IOT_MQTT_Yield(pclient, wait_expect);
    wait_actual = HAL_UptimeMs() - start;

    IOT_MQTT_Destroy(&pclient);
    HAL_Printf("wait_expect = %d\n", wait_expect);
    HAL_Printf("wait_actual = %d\n", wait_actual);
    ASSERT_IN(wait_expect - 100, wait_actual, wait_expect + 100);
}

/* IOT_MQTT_Yield: timeout=-1 */
CASE(MQTT_YIELD, case_04)
{
    void *pclient = NULL;
    int32_t wait_expect = -1;

    TEST_REPLACE_DEVCERT(&UTEST_MQTT_PRODUCT_KEY, &UTEST_MQTT_PRODUCT_SECRET, &UTEST_MQTT_DEVICE_NAME,
                         &UTEST_MQTT_DEVICE_SECRET);
    utest_mqtt_update_topic();

    pclient = mqtt_init_by_dev(0);
    ASSERT_NOT_NULL(pclient);

    int ret = IOT_MQTT_Yield(pclient, wait_expect);
    ASSERT_TRUE(ret < 0);
    IOT_MQTT_Destroy(&pclient);
}

SUITE(MQTT_YIELD) = {
    ADD_CASE(MQTT_YIELD, case_01),
    ADD_CASE(MQTT_YIELD, case_02),
    ADD_CASE(MQTT_YIELD, case_03),
    ADD_CASE(MQTT_YIELD, case_04),
    ADD_CASE_NULL
};

#endif
