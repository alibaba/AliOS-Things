#include "stdio.h"
#include "iot_import.h"
#include "iot_export_linkkit.h"
#include "cut.h"

const char *host = "iot-as-mqtt.cn-shanghai.aliyuncs.com";
CASE(HAL_TCP, case_01_eastablish)
{
    uint16_t port = 1883;
    uintptr_t t1 = HAL_TCP_Establish(host, port);
    ASSERT_TRUE(t1 != 0);
    int t2 = HAL_TCP_Destroy(t1);
    ASSERT_TRUE(t2 == 0);
}


SUITE(HAL_TCP) = {
    ADD_CASE(HAL_TCP, case_01_eastablish),
    ADD_CASE_NULL
};



