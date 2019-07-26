#ifdef DEVICE_MODEL_ENABLED
/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifdef DEPRECATED_LINKKIT
#else

#include "stdio.h"
#include "iot_export.h"
#include "iot_import.h"
//#include "app_entry.h"
//#include "utest_mqtt.h"
#include "cut.h"

#if defined(OTA_ENABLED) && defined(BUILD_AOS)
    #include "ota_service.h"
#endif

#define PRODUCT_KEY      "a1K1YKPvdRn"
#define PRODUCT_SECRET   "bdUZd0qmIppC5xBc"
#define DEVICE_NAME      "SDK-C-TEST_advance_02_kj"
#define DEVICE_SECRET    "ZH8eknnUSjCiqTeSr0mg1ngrG5920eeT"

#define USE_CUSTOME_DOMAIN              (0)

#if USE_CUSTOME_DOMAIN
    #define CUSTOME_DOMAIN_MQTT         "iot-as-mqtt.cn-shanghai.aliyuncs.com"
    #define CUSTOME_DOMAIN_HTTP         "iot-auth.cn-shanghai.aliyuncs.com"
#endif

#define USER_EXAMPLE_YIELD_TIMEOUT_MS   (1000)

typedef struct {
    int     master_devid;
    int     cloud_connected;
    int     master_initialized;
} user_example_ctx_t;

static user_example_ctx_t g_user_example_ctx;

static user_example_ctx_t *user_example_get_ctx(void)
{
    return &g_user_example_ctx;
}

int fota_ret = -1;
static int user_fota_event_handler(int type, const char *version)
{
    char buffer[128] = {0};
    int buffer_length = 128;
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();

    HAL_Printf("New Firmware Version: %s", version);

    fota_ret = IOT_Linkkit_Query(user_example_ctx->master_devid, ITM_MSG_QUERY_FOTA_DATA, (unsigned char *)buffer,
                                 buffer_length);
    return 0;
}

static void set_iotx_info()
{
    HAL_SetProductKey(PRODUCT_KEY);
    HAL_SetProductSecret(PRODUCT_SECRET);
    HAL_SetDeviceName(DEVICE_NAME);
    HAL_SetDeviceSecret(DEVICE_SECRET);
}

CASE(FOTA, case_03)
{
    int res = 0;
    fota_ret = -1;
    int ret1 =
                system(" rm -rf fota.sh* && wget http://gitlab.alibaba-inc.com/xicai.cxc/test_wiki/raw/master/device-test-client.sh -O fota.sh ");
    int ret2 = system("chmod +x ./fota.sh");
    int ret3 =
                system("./fota.sh fota upgrade a1K1YKPvdRn SDK-C-TEST_advance_02_kj app-1.0.0-20180101.1000 app-1.0.0-20180101.1001 app-1.0.0-20180101.1002");
    int ret4 = system("rm -rf  ./fota.sh*");
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();
    iotx_linkkit_dev_meta_info_t master_meta_info;

#if !defined(WIFI_PROVISION_ENABLED) || !defined(BUILD_AOS)
    set_iotx_info();
#endif

    memset(user_example_ctx, 0, sizeof(user_example_ctx_t));

    /* Register Callback */
    IOT_RegisterCallback(ITE_FOTA, user_fota_event_handler);

    memset(&master_meta_info, 0, sizeof(iotx_linkkit_dev_meta_info_t));
    memcpy(master_meta_info.product_key, PRODUCT_KEY, strlen(PRODUCT_KEY));
    memcpy(master_meta_info.product_secret, PRODUCT_SECRET, strlen(PRODUCT_SECRET));
    memcpy(master_meta_info.device_name, DEVICE_NAME, strlen(DEVICE_NAME));
    memcpy(master_meta_info.device_secret, DEVICE_SECRET, strlen(DEVICE_SECRET));

    /* Choose Login Server, domain should be configured before IOT_Linkkit_Open() */
#if USE_CUSTOME_DOMAIN
    IOT_Ioctl(IOTX_IOCTL_SET_MQTT_DOMAIN, (void *)CUSTOME_DOMAIN_MQTT);
    IOT_Ioctl(IOTX_IOCTL_SET_HTTP_DOMAIN, (void *)CUSTOME_DOMAIN_HTTP);
#else
    int domain_type = IOTX_CLOUD_REGION_SHANGHAI;
    IOT_Ioctl(IOTX_IOCTL_SET_DOMAIN, (void *)&domain_type);
#endif

    /* Choose Login Method */
    int dynamic_register = 0;
    IOT_Ioctl(IOTX_IOCTL_SET_DYNAMIC_REGISTER, (void *)&dynamic_register);

    /* Create Master Device Resources */
    user_example_ctx->master_devid = IOT_Linkkit_Open(IOTX_LINKKIT_DEV_TYPE_MASTER, &master_meta_info);
    if (user_example_ctx->master_devid < 0) {
        ASSERT_TRUE(user_example_ctx->master_devid >= 0);
    }

    /* Start Connect Aliyun Server */
    res = IOT_Linkkit_Connect(user_example_ctx->master_devid);
    if (res < 0) {
        ASSERT_TRUE(res >= 0);
    }
    IOT_Linkkit_Yield(6 * USER_EXAMPLE_YIELD_TIMEOUT_MS);
    IOT_Linkkit_Close(user_example_ctx->master_devid);
    ASSERT_TRUE(fota_ret == 0);
    ASSERT_TRUE(ret1 == 0);
    ASSERT_TRUE(ret2 == 0);
    ASSERT_TRUE(ret3 == 0);
    ASSERT_TRUE(ret4 == 0);
}

CASE(FOTA, case_02)
{
    int res = 0;
    fota_ret = -1;
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();
    iotx_linkkit_dev_meta_info_t master_meta_info;

#if !defined(WIFI_PROVISION_ENABLED) || !defined(BUILD_AOS)
    set_iotx_info();
#endif

    memset(user_example_ctx, 0, sizeof(user_example_ctx_t));

    /* Register Callback */
    IOT_RegisterCallback(ITE_FOTA, user_fota_event_handler);

    memset(&master_meta_info, 0, sizeof(iotx_linkkit_dev_meta_info_t));
    memcpy(master_meta_info.product_key, PRODUCT_KEY, strlen(PRODUCT_KEY));
    memcpy(master_meta_info.product_secret, PRODUCT_SECRET, strlen(PRODUCT_SECRET));
    memcpy(master_meta_info.device_name, DEVICE_NAME, strlen(DEVICE_NAME));
    memcpy(master_meta_info.device_secret, DEVICE_SECRET, strlen(DEVICE_SECRET));

    /* Choose Login Server, domain should be configured before IOT_Linkkit_Open() */
#if USE_CUSTOME_DOMAIN
    IOT_Ioctl(IOTX_IOCTL_SET_MQTT_DOMAIN, (void *)CUSTOME_DOMAIN_MQTT);
    IOT_Ioctl(IOTX_IOCTL_SET_HTTP_DOMAIN, (void *)CUSTOME_DOMAIN_HTTP);
#else
    int domain_type = IOTX_CLOUD_REGION_SHANGHAI;
    IOT_Ioctl(IOTX_IOCTL_SET_DOMAIN, (void *)&domain_type);
#endif

    /* Choose Login Method */
    int dynamic_register = 0;
    IOT_Ioctl(IOTX_IOCTL_SET_DYNAMIC_REGISTER, (void *)&dynamic_register);

    /* Create Master Device Resources */
    user_example_ctx->master_devid = IOT_Linkkit_Open(IOTX_LINKKIT_DEV_TYPE_MASTER, &master_meta_info);
    if (user_example_ctx->master_devid < 0) {
        ASSERT_TRUE(user_example_ctx->master_devid >= 0);
    }

    /* Start Connect Aliyun Server */
    res = IOT_Linkkit_Connect(user_example_ctx->master_devid);
    if (res < 0) {
        ASSERT_TRUE(res >= 0);
    }
    IOT_Linkkit_Yield(3 * USER_EXAMPLE_YIELD_TIMEOUT_MS);
    IOT_Linkkit_Close(user_example_ctx->master_devid);
    ASSERT_TRUE(fota_ret < 0);
}
#if 1
CASE(FOTA, case_01)
{
    int res = 0;
    fota_ret = -1;
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();
    iotx_linkkit_dev_meta_info_t master_meta_info;

#if !defined(WIFI_PROVISION_ENABLED) || !defined(BUILD_AOS)
    set_iotx_info();
#endif

    memset(user_example_ctx, 0, sizeof(user_example_ctx_t));

    /* Register Callback */
    IOT_RegisterCallback(ITE_FOTA, user_fota_event_handler);

    memset(&master_meta_info, 0, sizeof(iotx_linkkit_dev_meta_info_t));
    memcpy(master_meta_info.product_key, PRODUCT_KEY, strlen(PRODUCT_KEY));
    memcpy(master_meta_info.product_secret, PRODUCT_SECRET, strlen(PRODUCT_SECRET));
    memcpy(master_meta_info.device_name, DEVICE_NAME, strlen(DEVICE_NAME));
    memcpy(master_meta_info.device_secret, DEVICE_SECRET, strlen(DEVICE_SECRET));

    /* Choose Login Server, domain should be configured before IOT_Linkkit_Open() */
#if USE_CUSTOME_DOMAIN
    IOT_Ioctl(IOTX_IOCTL_SET_MQTT_DOMAIN, (void *)CUSTOME_DOMAIN_MQTT);
    IOT_Ioctl(IOTX_IOCTL_SET_HTTP_DOMAIN, (void *)CUSTOME_DOMAIN_HTTP);
#else
    int domain_type = IOTX_CLOUD_REGION_SHANGHAI;
    IOT_Ioctl(IOTX_IOCTL_SET_DOMAIN, (void *)&domain_type);
#endif

    /* Choose Login Method */
    int dynamic_register = 0;
    IOT_Ioctl(IOTX_IOCTL_SET_DYNAMIC_REGISTER, (void *)&dynamic_register);

    /* Create Master Device Resources */
    user_example_ctx->master_devid = IOT_Linkkit_Open(IOTX_LINKKIT_DEV_TYPE_MASTER, &master_meta_info);
    if (user_example_ctx->master_devid < 0) {
        ASSERT_TRUE(user_example_ctx->master_devid >= 0);
    }

    /* Start Connect Aliyun Server */
    res = IOT_Linkkit_Connect(user_example_ctx->master_devid);
    if (res < 0) {
        ASSERT_TRUE(res >= 0);
    }
    IOT_Linkkit_Query(user_example_ctx->master_devid, ITM_MSG_REQUEST_FOTA_IMAGE,
                      (unsigned char *)("app-1.0.0-20180101.1001"), 30);
    IOT_Linkkit_Yield(3 * USER_EXAMPLE_YIELD_TIMEOUT_MS);
    IOT_Linkkit_Close(user_example_ctx->master_devid);
    ASSERT_TRUE(fota_ret < 0);
}
#endif


SUITE(FOTA) = {
    ADD_CASE(FOTA, case_01),
    ADD_CASE(FOTA, case_02),
    ADD_CASE(FOTA, case_03),
    ADD_CASE_NULL
};
#endif
#endif  /* #ifdef DEVICE_MODEL_ENABLED */
