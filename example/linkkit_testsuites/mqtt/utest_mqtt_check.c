#if defined(MQTT_COMM_ENABLED)
#include "sdk-testsuites_internal.h"
#include "cut.h"

#include "utest_mqtt.h"


/**************************************************************************************/
/* iot_mqtt_checkstate */
/**************************************************************************************/
CASE(MQTT_CHECK, case_01)
{
    int ret = 0;
    ret = IOT_MQTT_CheckStateNormal(NULL);
    ASSERT_TRUE(ret < 0);
}

/* IOT_MQTT_CheckStateNormal: 正常流程 */
CASE(MQTT_CHECK, case_02)
{
    int ret = 0;
    void *pclient = NULL;

    TEST_REPLACE_DEVCERT(&UTEST_MQTT_PRODUCT_KEY, &UTEST_MQTT_PRODUCT_SECRET, &UTEST_MQTT_DEVICE_NAME,
                         &UTEST_MQTT_DEVICE_SECRET);
    utest_mqtt_update_topic();

    pclient = mqtt_init_by_dev(0);

    ret = IOT_MQTT_CheckStateNormal(pclient);
    ASSERT_TRUE(ret);

    IOT_MQTT_Destroy(&pclient);
}


SUITE(MQTT_CHECK) = {
    ADD_CASE(MQTT_CHECK, case_01),
    ADD_CASE(MQTT_CHECK, case_02),
    ADD_CASE_NULL
};

#endif
