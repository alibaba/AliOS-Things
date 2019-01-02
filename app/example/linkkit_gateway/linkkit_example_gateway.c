/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#ifndef DEPRECATED_LINKKIT
#include "newapi/gateway.c"
#else
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "cJSON.h"
#include "linkkit_gateway_export.h"
#include "iot_import.h"
#ifdef LINKKIT_GATEWAY_TEST_CMD
    #include "simulate_subdev/testcmd.h"
#endif

#if defined(OTA_ENABLED)
#include "ota_service.h"
#endif

// for demo only
#define PRODUCT_KEY      "a16UKrlKekO"
#define PRODUCT_SECRET   "RDluqbn3LQazrdqM"
#define DEVICE_NAME      "gateway_test01"
#define DEVICE_SECRET    "AT2XFOPOIbJaKfXsKeaEhabJ8TLhMQYp"

#define EXAMPLE_TRACE(...)                                  \
    do {                                                    \
        printf("\033[1;31;40m%s.%d: ", __func__, __LINE__); \
        printf(__VA_ARGS__);                                \
        printf("\033[0m");                                  \
    } while (0)

typedef struct {
    int ZB_Band;
    int ZB_Channel;

    char ZB_CO_MAC[32 + 1];
    char ZB_PAN_ID[32 + 1];
    char EXT_PAN_ID[32 + 1];
    char NETWORK_KEY[32 + 1];

    int connected;
    int register_completed;
    int lk_dev;
} gateway_t;

gateway_t  gateway;
static int gateway_register_complete(void *ctx);
static int gateway_get_property(char *in, char *out, int out_len, void *ctx);
static int gateway_set_property(char *in, void *ctx);
static int gateway_call_service(char *identifier, char *in, char *out,
                                int out_len, void *ctx);


/* callback function */
static linkkit_cbs_t linkkit_cbs = {
    .register_complete = gateway_register_complete,
    .get_property      = gateway_get_property,
    .set_property      = gateway_set_property,
    .call_service      = gateway_call_service,
};

#ifdef LINKKIT_GATEWAY_TEST_CMD
static int      disable_join = 1; /* not allow to join */
static char     permit_productKey[PRODUCT_KEY_MAXLEN];
static uint64_t permit_timeout_ms;

int linkkit_gateway_check_permit(int devtype, const char *productKey)
{
    uint64_t uptime_ms = HAL_UptimeMs();

    EXAMPLE_TRACE("devtype = %d\n", devtype);
    if (1 != devtype) {
        return 1;
    }

    EXAMPLE_TRACE("disable_join = %d\n", disable_join);
    if (disable_join) {
        return 0;
    }

    EXAMPLE_TRACE("permit_productKey = %s\n", permit_productKey);
    if (permit_productKey[0] && strcmp(permit_productKey, productKey)) {
        return 0;
    }

    EXAMPLE_TRACE("uptime_ms = %llu, permit_timeout_ms = %llu\n", uptime_ms,
                  permit_timeout_ms);
    if (uptime_ms < permit_timeout_ms) {
        return 1;
    }

    disable_join = 1;

    return 0;
}

int linkkit_gateway_testcmd_post_properties(const char *properties)
{
    if (linkkit_gateway_post_property_json_sync(gateway.lk_dev,
            (char *)properties, 10000) < 0) {
        return -1;
    }
    return 0;
}

int linkkit_gateway_testcmd_post_event(const char *identifier,
                                       const char *events)
{
    if (linkkit_gateway_trigger_event_json_sync(
                    gateway.lk_dev, (char *)identifier, (char *)events, 10000) < 0) {
        return -1;
    }

    return 0;
}

#endif

static int gateway_register_complete(void *ctx)
{
    gateway_t *gw = (gateway_t *)ctx;

    gw->register_completed = 1;

    return 0;
}

/*
 * the handler property get
 * alink method: thing.service.property.get
 */
static int gateway_get_property(char *in, char *out, int out_len, void *ctx)
{
    gateway_t *gw = ctx;
    cJSON     *rJson, *pJson;
    char      *p;
    int        iSize, i;

    EXAMPLE_TRACE("in: %s\n", in);

    if (!gw) {
        EXAMPLE_TRACE("gateway not found\n");
        return -1;
    }

    /* parse input json */
    rJson = cJSON_Parse(in);
    if (!rJson) {
        return -1;
    }

    iSize = cJSON_GetArraySize(rJson);
    if (iSize <= 0) {
        cJSON_Delete(rJson);
        return -1;
    }

    pJson = cJSON_CreateObject();
    if (!pJson) {
        cJSON_Delete(rJson);
        return -1;
    }

    /*
     * follow TSL to parse input json, and generate output json.
     * please modify this logic in user's case follow as user's TSL.
     */
    for (i = 0; i < iSize; i++) {
        cJSON *pSub = cJSON_GetArrayItem(rJson, i);

        if (strcmp(pSub->valuestring, "ZB_Band") == 0) {
            cJSON_AddNumberToObject(pJson, "ZB_Band", gw->ZB_Band);
        } else if (strcmp(pSub->valuestring, "ZB_Channel") == 0) {
            cJSON_AddNumberToObject(pJson, "ZB_Channel", gw->ZB_Channel);
        } else if (strcmp(pSub->valuestring, "ZB_CO_MAC") == 0) {
            cJSON_AddStringToObject(pJson, "ZB_CO_MAC", gw->ZB_CO_MAC);
        } else if (strcmp(pSub->valuestring, "ZB_PAN_ID") == 0) {
            cJSON_AddStringToObject(pJson, "ZB_PAN_ID", gw->ZB_PAN_ID);
        } else if (strcmp(pSub->valuestring, "EXT_PAN_ID") == 0) {
            cJSON_AddStringToObject(pJson, "EXT_PAN_ID", gw->EXT_PAN_ID);
        } else if (strcmp(pSub->valuestring, "NETWORK_KEY") == 0) {
            cJSON_AddStringToObject(pJson, "NETWORK_KEY", gw->NETWORK_KEY);
        }
    }

    p = cJSON_PrintUnformatted(pJson);
    if (!p) {
        cJSON_Delete(rJson);
        cJSON_Delete(pJson);
        return -1;
    }

    if (strlen(p) >= out_len) {
        cJSON_Delete(rJson);
        cJSON_Delete(pJson);
        HAL_Free(p);
        return -1;
    }

    strcpy(out, p);

    EXAMPLE_TRACE("out: %s\n", out);

    cJSON_Delete(rJson);
    cJSON_Delete(pJson);
    HAL_Free(p);

    return 0;
}

/*
 * the handler property set
 * alink method: thing.service.property.set
 */
static int gateway_set_property(char *in, void *ctx)
{
    gateway_t *gw = ctx;
    cJSON *rJson, *ZB_Band, *ZB_Channel, *ZB_PAN_ID, *EXT_PAN_ID, *ZB_CO_MAC,
          *NETWORK_KEY;

    EXAMPLE_TRACE("in: %s\n", in);

    rJson = cJSON_Parse(in);
    if (!rJson) {
        return -1;
    }

    /* parse input json, set the gateway value */
    ZB_Band = cJSON_GetObjectItem(rJson, "ZB_Band");
    if (ZB_Band) {
        gw->ZB_Band = ZB_Band->valueint;
    }

    ZB_Channel = cJSON_GetObjectItem(rJson, "ZB_Channel");
    if (ZB_Channel) {
        gw->ZB_Channel = ZB_Channel->valueint;
    }

    ZB_PAN_ID = cJSON_GetObjectItem(rJson, "ZB_PAN_ID");
    if (ZB_PAN_ID)
        strncpy(gw->ZB_PAN_ID, ZB_PAN_ID->valuestring,
                sizeof(gw->ZB_PAN_ID) - 1);

    EXT_PAN_ID = cJSON_GetObjectItem(rJson, "EXT_PAN_ID");
    if (EXT_PAN_ID)
        strncpy(gw->EXT_PAN_ID, EXT_PAN_ID->valuestring,
                sizeof(gw->EXT_PAN_ID) - 1);

    ZB_CO_MAC = cJSON_GetObjectItem(rJson, "ZB_CO_MAC");
    if (ZB_CO_MAC)
        strncpy(gw->ZB_CO_MAC, ZB_CO_MAC->valuestring,
                sizeof(gw->ZB_CO_MAC) - 1);

    NETWORK_KEY = cJSON_GetObjectItem(rJson, "NETWORK_KEY");
    if (NETWORK_KEY)
        strncpy(gw->NETWORK_KEY, NETWORK_KEY->valuestring,
                sizeof(gw->NETWORK_KEY) - 1);

    linkkit_gateway_post_property_json_sync(gw->lk_dev, in, 5000);
    cJSON_Delete(rJson);

    return 0;
}


/*
 * the handler of service which is defined by identifier, not property
 * alink method: thing.service.{tsl.service.identifier}
 */
static int gateway_call_service(char *identifier, char *in, char *out,
                                int out_len, void *ctx)
{
    /*
     * please follow user's TSL to mofidy this id - SetTimerTask and TimeReset.
     */
    if (strcmp(identifier, "SetTimerTask") == 0) {
        snprintf(out, out_len, "{\"SetTimer\": \"hello, gateway!\"}");
    } else if (strcmp(identifier, "TimeReset") == 0) {
        EXAMPLE_TRACE("TimeReset params: %s\n", in);
    }

    return 0;
}


/*
 * post all properties
 */
static int post_all_properties(gateway_t *gw)
{
    cJSON *pJson = cJSON_CreateObject();
    char *p     = NULL;
    if (!pJson) {
        return -1;
    }

    cJSON_AddNumberToObject(pJson, "ZB_Band", gw->ZB_Band);
    cJSON_AddNumberToObject(pJson, "ZB_Channel", gw->ZB_Channel);
    cJSON_AddStringToObject(pJson, "ZB_CO_MAC", gw->ZB_CO_MAC);
    cJSON_AddStringToObject(pJson, "ZB_PAN_ID", gw->ZB_PAN_ID);
    cJSON_AddStringToObject(pJson, "EXT_PAN_ID", gw->EXT_PAN_ID);
    cJSON_AddStringToObject(pJson, "NETWORK_KEY", gw->NETWORK_KEY);

    p = cJSON_PrintUnformatted(pJson);
    if (!p) {
        cJSON_Delete(pJson);
        return -1;
    }

    EXAMPLE_TRACE("property: %s\n", p);

    linkkit_gateway_post_property_json_sync(gw->lk_dev, p, 5000);

    cJSON_Delete(pJson);
    HAL_Free(p);

    return 0;
}

/* event handler for gateway */
static int event_handler(linkkit_event_t *ev, void *ctx)
{
    gateway_t *gw = ctx;

    switch (ev->event_type) {
        /* cloud connected */
        case LINKKIT_EVENT_CLOUD_CONNECTED: {
            EXAMPLE_TRACE("cloud connected\n");
#if defined(OTA_ENABLED)
            ota_service_init(NULL);
#endif
            /* modify user's logic in there */
            /* example case just post all property */
            post_all_properties(gw); /* sync to cloud */
            gw->connected = 1;
        }
        break;

        /* cloud disconnected */
        case LINKKIT_EVENT_CLOUD_DISCONNECTED: {
            gw->connected = 0;
            EXAMPLE_TRACE("cloud disconnected\n");
        }
        break;

        /* subdev delete */
        case LINKKIT_EVENT_SUBDEV_DELETED: {
            char *productKey = ev->event_data.subdev_deleted.productKey;
            char *deviceName = ev->event_data.subdev_deleted.deviceName;
            EXAMPLE_TRACE("delete subdev %s<%s>\n", productKey, deviceName);

#ifdef LINKKIT_GATEWAY_TEST_CMD
            if (testcmd_delete_device(productKey, deviceName)) {
                EXAMPLE_TRACE("Delete subdevice %s<%s> failed!\n", productKey,
                              deviceName);
            }
#endif
        }
        break;

        /* between timeoutSec, subdev of productKey can be register */
        case LINKKIT_EVENT_SUBDEV_PERMITED: {
            char *productKey = ev->event_data.subdev_permited.productKey;
            int   timeoutSec = ev->event_data.subdev_permited.timeoutSec;
            EXAMPLE_TRACE("permit subdev %s in %d seconds\n", productKey,
                          timeoutSec);
#ifdef LINKKIT_GATEWAY_TEST_CMD
            disable_join = 0;
            if (NULL == productKey) {
                memset(permit_productKey, 0, sizeof(permit_productKey));
            } else {
                strcpy(permit_productKey, productKey);
            }
            permit_timeout_ms = HAL_UptimeMs() + timeoutSec * 1000;
#endif
            /* please enter user's logic in there */
        }
        break;
    }

    return 0;
}

void set_iotx_info()
{
    HAL_SetProductKey(PRODUCT_KEY);
    HAL_SetProductSecret(PRODUCT_SECRET);
    HAL_SetDeviceName(DEVICE_NAME);
    HAL_SetDeviceSecret(DEVICE_SECRET);
}

int linkkit_main(void *p)
{
    linkkit_params_t *initParams = NULL;
    int maxMsgSize, maxMsgQueueSize, prop_post_reply, event_post_reply;

    IOT_OpenLog("linkkit_gw");
    IOT_SetLogLevel(IOT_LOG_DEBUG);
#ifndef WIFI_AWSS_ENABLED
    set_iotx_info();
#endif
    memset(&gateway, 0, sizeof(gateway_t));

    /* fill fake zigbee network info */
    /*
     * this example is a zigbee subdev.
     * please modify this logic follow as user's case.
     */
    gateway.ZB_Band    = 25;
    gateway.ZB_Channel = 16;

    strcpy(gateway.ZB_PAN_ID, "8215");
    strcpy(gateway.EXT_PAN_ID, "000D6F000ED34E34");
    strcpy(gateway.ZB_CO_MAC, "000D6F000ED34E34");
    strcpy(gateway.NETWORK_KEY, "21B9F385F114B1C4AE07D5753B95355D");

    /*
     * please set init parameter
     */
    initParams = linkkit_gateway_get_default_params();
    if (!initParams) {
        return -1;
    }
    /* LINKKIT_OPT_MAX_MSG_SIZE: max size of message */
    maxMsgSize = 4096;
    linkkit_gateway_setopt(initParams, LINKKIT_OPT_MAX_MSG_SIZE, &maxMsgSize,
                           sizeof(int));
    /* LINKKIT_OPT_MAX_MSG_QUEUE_SIZE: max size of message queue */
    maxMsgQueueSize = 8;
    linkkit_gateway_setopt(initParams, LINKKIT_OPT_MAX_MSG_QUEUE_SIZE,
                           &maxMsgQueueSize, sizeof(int));

    prop_post_reply = 0;
    linkkit_gateway_setopt(initParams, LINKKIT_OPT_PROPERTY_POST_REPLY,
                           &prop_post_reply, sizeof(int));

    event_post_reply = 0;
    linkkit_gateway_setopt(initParams, LINKKIT_OPT_EVENT_POST_REPLY,
                           &event_post_reply, sizeof(int));

    int stacksize = 1024 * 4;
    linkkit_gateway_setopt(initParams, LINKKIT_OPT_THREAD_STACK_SIZE,
                           &stacksize, sizeof(int));

    /* set event handler */
    linkkit_gateway_set_event_callback(initParams, event_handler, &gateway);

    /* set init parameter into gateway */
    if (linkkit_gateway_init(initParams) < 0) {
        EXAMPLE_TRACE("linkkit_gateway_init failed\n");
        return -1;
    }

    /* start */
    gateway.lk_dev = linkkit_gateway_start(&linkkit_cbs, &gateway);
    if (gateway.lk_dev < 0) {
        EXAMPLE_TRACE("linkkit_gateway_start failed\n");
        return -1;
    }

    while (gateway.register_completed == 0) {
        HAL_SleepMs(1000);
    }


    while (1) {
        /*
         * gateway trigger event
         * please follow user's case, modify this logic
         */
        linkkit_gateway_trigger_event_json_sync(gateway.lk_dev, "Error",
                                                "{\"ErrorCode\": 0}", 10000);
        HAL_SleepMs(1000);
    }

    /* gateway stop */
    linkkit_gateway_stop(gateway.lk_dev);
    /* gateway exit */
    linkkit_gateway_exit();

    IOT_DumpMemoryStats(IOT_LOG_DEBUG);
    IOT_CloseLog();

    EXAMPLE_TRACE("out of sample!\n");
    return 0;
}
#endif