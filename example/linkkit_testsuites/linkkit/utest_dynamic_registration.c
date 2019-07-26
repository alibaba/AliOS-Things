#ifdef DEVICE_MODEL_ENABLED
/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifdef DEPRECATED_LINKKIT
#else

#include "stdio.h"
#include "iot_export.h"
#include "iot_import.h"
#include "exports/iot_export_linkkit.h"
//#include "app_entry.h"
//#include "utest_mqtt.h"
#include "cut.h"

#if defined(OTA_ENABLED) && defined(BUILD_AOS)
    #include "ota_service.h"
#endif

#define PRODUCT_KEY "a1J3wtoxPRb"
#define PRODUCT_SECRET "1OfYApeDKUfdbPhE"
static char DYNAMIC_TEST_DEVICE_NAME[128] = "locao_tes1";
#define DEVICE_SECRET "null"


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

static void set_iotx_info_dynamic()
{
    memset(DYNAMIC_TEST_DEVICE_NAME, 0, 128);
    strncpy(DYNAMIC_TEST_DEVICE_NAME, "mytestauto002", 128);
    char cmd[256] = {0};
    int ret1 =
                system("rm -f dynamic.sh* && wget http://gitlab.alibaba-inc.com/xicai.cxc/test_wiki/raw/master/device-test-client.sh -O dynamic.sh");
    int ret2 = system("chmod +x ./dynamic.sh");
    sprintf(cmd, "./dynamic.sh device create a1J3wtoxPRb  %s", DYNAMIC_TEST_DEVICE_NAME);
    int ret = system(cmd);
    ASSERT_TRUE(ret == 0);
    ASSERT_TRUE(ret1 == 0);
    ASSERT_TRUE(ret2 == 0);
    HAL_SleepMs(1000);
}


static void delete_device()
{
    char cmd[256] = {0};
    sprintf(cmd, "./dynamic.sh device delete a1J3wtoxPRb  %s", DYNAMIC_TEST_DEVICE_NAME);
    int ret = system(cmd);
    ASSERT_TRUE(ret == 0);
    ret = system("rm -f ./dynamic.sh*");
    ASSERT_TRUE(ret == 0);
}


/* use already registrated device to do dynamic registration, should FAIL */
CASE(DYNAMIC_REG, case_01_exit_due_to_already_registeded)
{
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();
    memset(user_example_ctx, 0, sizeof(user_example_ctx_t));
    /* Register Callback */
    iotx_linkkit_dev_meta_info_t master_meta_info;
    memset(&master_meta_info, 0, sizeof(iotx_linkkit_dev_meta_info_t));
    memcpy(master_meta_info.product_key, PRODUCT_KEY, strlen(PRODUCT_KEY));
    memcpy(master_meta_info.product_secret, PRODUCT_SECRET, strlen(PRODUCT_SECRET));
    memcpy(master_meta_info.device_name, DYNAMIC_TEST_DEVICE_NAME, strlen(DYNAMIC_TEST_DEVICE_NAME));
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
    int dynamic_register = 1;
    IOT_Ioctl(IOTX_IOCTL_SET_DYNAMIC_REGISTER, (void *)&dynamic_register);

    /* Create Master Device Resources */
    user_example_ctx->master_devid = IOT_Linkkit_Open(IOTX_LINKKIT_DEV_TYPE_MASTER, &master_meta_info);
    if (user_example_ctx->master_devid < 0) {
        ASSERT_TRUE(user_example_ctx->master_devid >= 0);
    }
    int res = IOT_Linkkit_Connect(user_example_ctx->master_devid);
    ASSERT_TRUE(res <  0);

    /* Start Connect Aliyun Server */
    IOT_Linkkit_Close(user_example_ctx->master_devid);
}

/* create new device, dynamic registrate it, delete this device. should PASS */
CASE(DYNAMIC_REG, case_02_new_device_dynamic_reg)
{
#if !defined(WIFI_PROVISION_ENABLED) || !defined(BUILD_AOS)
    set_iotx_info_dynamic();
#endif
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();
    memset(user_example_ctx, 0, sizeof(user_example_ctx_t));
    /* Register Callback */
    iotx_linkkit_dev_meta_info_t master_meta_info;
    memset(&master_meta_info, 0, sizeof(iotx_linkkit_dev_meta_info_t));
    memcpy(master_meta_info.product_key, PRODUCT_KEY, strlen(PRODUCT_KEY));
    memcpy(master_meta_info.product_secret, PRODUCT_SECRET, strlen(PRODUCT_SECRET));
    memcpy(master_meta_info.device_name, DYNAMIC_TEST_DEVICE_NAME, strlen(DYNAMIC_TEST_DEVICE_NAME));
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
    int dynamic_register = 1;
    IOT_Ioctl(IOTX_IOCTL_SET_DYNAMIC_REGISTER, (void *)&dynamic_register);

    /* Create Master Device Resources */
    user_example_ctx->master_devid = IOT_Linkkit_Open(IOTX_LINKKIT_DEV_TYPE_MASTER, &master_meta_info);
    if (user_example_ctx->master_devid < 0) {
        ASSERT_TRUE(user_example_ctx->master_devid >= 0);
    }
    int res = IOT_Linkkit_Connect(user_example_ctx->master_devid);
    delete_device();
    ASSERT_TRUE(res >= 0);

    /* Start Connect Aliyun Server */
    IOT_Linkkit_Close(user_example_ctx->master_devid);
}


SUITE(DYNAMIC_REG) = {
    ADD_CASE(DYNAMIC_REG, case_01_exit_due_to_already_registeded),
    ADD_CASE(DYNAMIC_REG, case_02_new_device_dynamic_reg),
    ADD_CASE_NULL
};
#endif
#endif  /* #ifdef DEVICE_MODEL_ENABLED */
