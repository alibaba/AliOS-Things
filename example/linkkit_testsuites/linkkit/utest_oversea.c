/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef DEPRECATED_LINKKIT
#ifdef DEVICE_MODEL_ENABLED
#include "sdk-testsuites_internal.h"
#include "cut.h"
#include "iotx_utils.h"
#include "iot_export_linkkit.h"

#define EXAMPLE_TRACE(...)                               \
    do {                                                     \
        HAL_Printf("\033[1;32;40m%s.%d: ", __func__, __LINE__);  \
        HAL_Printf(__VA_ARGS__);                                 \
        HAL_Printf("\033[0m\r\n");                                   \
    } while (0)

typedef struct _sample_context {
    const void *thing;
    int         cloud_connected;
    int         local_connected;
    int         thing_enabled;
} sample_context_t;

int login(const char *pk, const char *dn, const char *ds, int domain_type)
{
    iotx_linkkit_dev_meta_info_t master_meta_info;
    memset(&master_meta_info, 0, sizeof(iotx_linkkit_dev_meta_info_t));
    memcpy(master_meta_info.product_key, pk, strlen(pk));
    memcpy(master_meta_info.device_name, dn, strlen(dn));
    memcpy(master_meta_info.device_secret, ds, strlen(ds));
    /* linkkit Construct */
    IOT_Ioctl(IOTX_IOCTL_SET_DOMAIN, (void *)&domain_type);

    int dynamic_register = 0;
    IOT_Ioctl(IOTX_IOCTL_SET_DYNAMIC_REGISTER, (void *)&dynamic_register);
    /* Create Master Device Resources */
    int master_devid = IOT_Linkkit_Open(IOTX_LINKKIT_DEV_TYPE_MASTER, &master_meta_info);
    if (master_devid < 0) {
        EXAMPLE_TRACE("IOT_Linkkit_Open Failed\n");
        return -1;
    }

    /* Start Connect Aliyun Server */
    int res = IOT_Linkkit_Connect(master_devid);
    if (res < 0) {
        EXAMPLE_TRACE("IOT_Linkkit_Connect Failed\n");
    }
    IOT_Linkkit_Close(master_devid);
    return res;
}

CASE(OVERSEA_TEST, iotx_oversea_sinapore_001)
{
    const char *PRODUCT_KEY = "a2YWQhDz7oD";
    const char *DEVICE_NAME = "oversea_vt";
    const char *DEVICE_SECRET = "zRyw5nhYGoV6lMjneSJaeUrOYBpE6Wj7";
    int domain_type = IOTX_CLOUD_REGION_SINGAPORE;
    int res = login(PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET, domain_type);
    ASSERT_EQ(res, SUCCESS_RETURN);
}
//JAPAN
CASE(OVERSEA_TEST, iotx_oversea_japan_002)
{
    const char *PRODUCT_KEY = "a6MTFO36u6z";
    const char *DEVICE_NAME = "testjp_001";
    const char *DEVICE_SECRET = "zGKtOXYuwbw6eHLFQfqwH3qSqSmCeUWy";
    int domain_type = IOTX_CLOUD_REGION_JAPAN;
    /* linkkit Construct */
    int res = login(PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET, domain_type);
    ASSERT_EQ(res, SUCCESS_RETURN);
}

// USA West
CASE(OVERSEA_TEST, iotx_oversea_usa_west_003)
{
    const char *PRODUCT_KEY = "a3IzajNUU7w";
    const char *DEVICE_NAME = "testoversea_us_silicon_001";
    const char *DEVICE_SECRET = "rYZ3Wi5V1ym949fccXuo48Fy8OgQwbKE";
    int domain_type = IOTX_CLOUD_REGION_USA_WEST;
    /* linkkit Construct */
    int res = login(PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET, domain_type);
    ASSERT_EQ(res, SUCCESS_RETURN);
}

//germany
CASE(OVERSEA_TEST, iotx_oversea_germany_004)
{
    const char *PRODUCT_KEY = "a5ESHSVnt94";
    const char *DEVICE_NAME = "testoversea_ger_001";
    const char *DEVICE_SECRET = "TPCpfVJ7QxQtjWoKKvIehrlyK7JQLvzc";

    int domain_type = IOTX_CLOUD_REGION_GERMANY;
    /* linkkit Construct */
    int res = login(PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET, domain_type);
    ASSERT_EQ(res, SUCCESS_RETURN);
}

SUITE(OVERSEA_TEST) = {
    ADD_CASE(OVERSEA_TEST, iotx_oversea_sinapore_001),
    ADD_CASE(OVERSEA_TEST, iotx_oversea_japan_002),
    ADD_CASE(OVERSEA_TEST, iotx_oversea_usa_west_003),
    ADD_CASE(OVERSEA_TEST, iotx_oversea_germany_004),
    ADD_CASE_NULL
};

#endif
#endif

