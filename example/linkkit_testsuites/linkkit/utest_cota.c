/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifdef DEVICE_MODEL_ENABLED
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

char PRODUCT_KEY[128] = "a1ybTpus98a";
char PRODUCT_SECRET[128] = "7jluWm1zql7bt8qK";
char DEVICE_NAME[128] = "k5_high_end";
char DEVICE_SECRET[128] = "IKrDsu23971EslKMo2jQxi8jrOuBipOQ";

char PRODUCT_KEY_NO_REMOTE_COTA[128] = "a1Bdy0GVqdH";
char PRODUCT_SECRET_NO_REMOTE_COTA[128] = "P65pUVv3KcQXba8N";
char DEVICE_NAME_NO_REMOTE_COTA[128] = "my_gw_prod_001";
char DEVICE_SECRET_NO_REMOTE_COTA[128] = "HLptqtpGLeXpYfiC1ZyH5l3iLr4kAtp9";

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

/** type:
  *
  * 0 - new config exist
  *
  */
char cota_buffer[128] = {0};
static int user_cota_event_handler(int type, const char *config_id, int config_size, const char *get_type,
                                   const char *sign, const char *sign_method, const char *url)
{
    int buffer_length = 128;
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();

    if (type == 0) {
        IOT_Linkkit_Query(user_example_ctx->master_devid, ITM_MSG_QUERY_COTA_DATA, (unsigned char *)cota_buffer, buffer_length);

    }
    return 0;
}

static void set_iotx_info()
{
    HAL_SetProductKey(PRODUCT_KEY);
    HAL_SetProductSecret(PRODUCT_SECRET);
    HAL_SetDeviceName(DEVICE_NAME);
    HAL_SetDeviceSecret(DEVICE_SECRET);
}

static void set_iotx_info_no_remote_cota()
{
    HAL_SetProductKey(PRODUCT_KEY_NO_REMOTE_COTA);
    HAL_SetProductSecret(PRODUCT_SECRET_NO_REMOTE_COTA);
    HAL_SetDeviceName(DEVICE_NAME_NO_REMOTE_COTA);
    HAL_SetDeviceSecret(DEVICE_SECRET_NO_REMOTE_COTA);
    memset(PRODUCT_KEY, 0, 128);
    memcpy(PRODUCT_KEY, PRODUCT_KEY_NO_REMOTE_COTA, strlen(PRODUCT_KEY_NO_REMOTE_COTA));
    memset(PRODUCT_SECRET, 0, 128);
    memcpy(PRODUCT_SECRET, PRODUCT_SECRET_NO_REMOTE_COTA, strlen(PRODUCT_SECRET_NO_REMOTE_COTA));
    memset(DEVICE_NAME, 0, 128);
    memcpy(DEVICE_NAME, DEVICE_NAME_NO_REMOTE_COTA, strlen(DEVICE_NAME_NO_REMOTE_COTA));
    memset(DEVICE_SECRET, 0, 128);
    memcpy(DEVICE_SECRET, DEVICE_SECRET_NO_REMOTE_COTA, strlen(DEVICE_SECRET_NO_REMOTE_COTA));
}


static void cota_common()
{
    int res = 0;
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();
    memset(user_example_ctx, 0, sizeof(user_example_ctx_t));
    /* Register Callback */
    IOT_RegisterCallback(ITE_COTA, user_cota_event_handler);
    iotx_linkkit_dev_meta_info_t master_meta_info;
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
    IOT_Linkkit_Query(user_example_ctx->master_devid, ITM_MSG_REQUEST_COTA, NULL, 0);
    IOT_Linkkit_Yield(USER_EXAMPLE_YIELD_TIMEOUT_MS);
    IOT_Linkkit_Yield(USER_EXAMPLE_YIELD_TIMEOUT_MS);
    IOT_Linkkit_Close(user_example_ctx->master_devid);
}

/* this product has remote cota, request it */
CASE(COTA, case_01_has_remote_cota)
{
#if !defined(WIFI_PROVISION_ENABLED) || !defined(BUILD_AOS)
    set_iotx_info();
#endif
    memset(cota_buffer, 0, 128);
    cota_common();
    const char *ref = "{\"hi\":\"QNameooo\"}";
    int ret = strcmp(cota_buffer, ref);
    ASSERT_TRUE(ret == 0);
}

/* this product has NO remote cota, request it */
CASE(COTA, case_02_no_remote_cota)
{
    set_iotx_info_no_remote_cota();
    memset(cota_buffer, 0, 128);
    cota_common();
    const char *ref = "{\"hi\":\"QNameooo\"}";
    int ret = strcmp(cota_buffer, ref);
    ASSERT_TRUE(ret != 0);
}

SUITE(COTA) = {
    ADD_CASE(COTA, case_01_has_remote_cota),
    ADD_CASE(COTA, case_02_no_remote_cota),
    ADD_CASE_NULL
};
#endif
#endif /* #ifdef DEVICE_MODEL_ENABLED */
