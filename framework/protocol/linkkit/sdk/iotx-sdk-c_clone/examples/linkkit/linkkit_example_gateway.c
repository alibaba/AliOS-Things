/*
 * Copyright (c) 2014-2016 Alibaba Group. All rights reserved.
 * License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */


#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "cJSON.h"
#include "light.h"

#include "linkkit_gateway_export.h"
#include "iot_import.h"

#define LINKKIT_OTA_BUFFER_SIZE (512)

#define EXAMPLE_TRACE(...)                                      \
do {                                                     \
    printf("\033[1;31;40m%s.%d: ", __func__, __LINE__);  \
    printf(__VA_ARGS__);                                 \
    printf("\033[0m");                                   \
} while (0)

typedef struct {
    int  ZB_Band;
    int  ZB_Channel;

    char ZB_CO_MAC[32 + 1];
    char ZB_PAN_ID[32 + 1];
    char EXT_PAN_ID[32 + 1];
    char NETWORK_KEY[32 + 1];

    int connected;
	int register_completed;
    int lk_dev;
} gateway_t;

static int gateway_register_complete(void *ctx);
static int gateway_get_property(char *in, char *out, int out_len, void *ctx);
static int gateway_set_property(char *in, void *ctx);
static int gateway_call_service(char *identifier, char *in, char *out, int out_len, void *ctx);


/* callback function */
static linkkit_cbs_t linkkit_cbs = {
	.register_complete = gateway_register_complete,
    .get_property = gateway_get_property,
    .set_property = gateway_set_property,
    .call_service = gateway_call_service,
};

static int gateway_register_complete(void *ctx)
{
	gateway_t *gw = (gateway_t *)ctx;
    
    if (gw == NULL) {
        return -1;
    }

	gw->register_completed = 1;
    EXAMPLE_TRACE("Current Device %d Register ALL Service Completed\n",gw->lk_dev);
	return 0;
}

/*
 * the handler property get
 * alink method: thing.service.property.get
 */
static int gateway_get_property(char *in, char *out, int out_len, void *ctx)
{
    gateway_t *gw = ctx;
    cJSON *rJson, *pJson;
    char *p;
    int iSize, i;

    EXAMPLE_TRACE("in: %s\n", in);

    if (!gw) {
        EXAMPLE_TRACE("gateway not found\n");
        return -1;
    }

    /* parse input json */
    rJson = cJSON_Parse(in);
    if (!rJson)
        return -1;

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
    cJSON *rJson, *ZB_Band, *ZB_Channel, *ZB_PAN_ID, *EXT_PAN_ID, *ZB_CO_MAC, *NETWORK_KEY;

    EXAMPLE_TRACE("in: %s\n", in);

    rJson = cJSON_Parse(in);
    if (!rJson)
        return -1;

    /* parse input json, set the gateway value */
    ZB_Band = cJSON_GetObjectItem(rJson, "ZB_Band");
    if (ZB_Band)
        gw->ZB_Band = ZB_Band->valueint;

    ZB_Channel = cJSON_GetObjectItem(rJson, "ZB_Channel");
    if (ZB_Channel)
        gw->ZB_Channel = ZB_Channel->valueint;

    ZB_PAN_ID = cJSON_GetObjectItem(rJson, "ZB_PAN_ID");
    if (ZB_PAN_ID)
        strncpy(gw->ZB_PAN_ID, ZB_PAN_ID->valuestring, sizeof(gw->ZB_PAN_ID) - 1);

    EXT_PAN_ID = cJSON_GetObjectItem(rJson, "EXT_PAN_ID");
    if (EXT_PAN_ID)
        strncpy(gw->EXT_PAN_ID, EXT_PAN_ID->valuestring, sizeof(gw->EXT_PAN_ID) - 1);

    ZB_CO_MAC = cJSON_GetObjectItem(rJson, "ZB_CO_MAC");
    if (ZB_CO_MAC)
        strncpy(gw->ZB_CO_MAC, ZB_CO_MAC->valuestring, sizeof(gw->ZB_CO_MAC) - 1);

    NETWORK_KEY = cJSON_GetObjectItem(rJson, "NETWORK_KEY");
    if (NETWORK_KEY)
        strncpy(gw->NETWORK_KEY, NETWORK_KEY->valuestring, sizeof(gw->NETWORK_KEY) - 1);

    linkkit_gateway_post_property_json_sync(gw->lk_dev, in, 5000);
    cJSON_Delete(rJson);

    return 0;
}


/*
 * the handler of service which is defined by identifier, not property
 * alink method: thing.service.{tsl.service.identifier}
 */
static int gateway_call_service(char *identifier, char *in, char *out, int out_len, void *ctx)
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
    char* p = NULL;
    if (!pJson)
        return -1;

    cJSON_AddNumberToObject(pJson, "ZB_Band",     gw->ZB_Band);
    cJSON_AddNumberToObject(pJson, "ZB_Channel",  gw->ZB_Channel);
    cJSON_AddStringToObject(pJson, "ZB_CO_MAC",   gw->ZB_CO_MAC);
    cJSON_AddStringToObject(pJson, "ZB_PAN_ID",   gw->ZB_PAN_ID);
    cJSON_AddStringToObject(pJson, "EXT_PAN_ID",  gw->EXT_PAN_ID);
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

        /* modify user's logic in there */
        /* example case just post all property */
        post_all_properties(gw);    /* sync to cloud */
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
    }
        break;

        /* between timeoutSec, subdev of productKey can be register */
    case LINKKIT_EVENT_SUBDEV_PERMITED:
        {
            char *productKey = ev->event_data.subdev_permited.productKey;
            int   timeoutSec = ev->event_data.subdev_permited.timeoutSec;
            EXAMPLE_TRACE("permit subdev %s in %d seconds\n", productKey, timeoutSec);

            /* please enter user's logic in there */
        }
        break;
    }

    return 0;
}

void linkkit_fota_callback(service_fota_callback_type_t callback_type, const char* version)
{
    char fota_buffer[LINKKIT_OTA_BUFFER_SIZE] = {0};

    EXAMPLE_TRACE("Fota Version: %s\n",version);

    linkkit_gateway_invoke_fota_service(fota_buffer,LINKKIT_OTA_BUFFER_SIZE);
}

void trigger_event_callback(int ret,void *ctx)
{
    EXAMPLE_TRACE("Trigger Event Result: %d\n",ret);
}

int main(void)
{
    gateway_t gateway;
    gateway_t subdev;
    linkkit_params_t *initParams = NULL;
    int maxMsgSize, maxMsgQueueSize, prop_post_reply, event_post_reply;

    IOT_OpenLog("linkkit_gw");
    IOT_SetLogLevel(IOT_LOG_DEBUG);

    HAL_SetProductKey("a1RIsMLz2BJ");
    HAL_SetDeviceName("example1");
    HAL_SetDeviceSecret("RDXf67itLqZCwdMCRrw0N5FHbv5D7jrE");

    memset(&gateway, 0, sizeof(gateway_t));
    memset(&subdev, 0, sizeof(gateway_t));

    /* fill fake zigbee network info */
    /*
     * this example is a zigbee subdev.
     * please modify this logic follow as user's case.
     */
    gateway.ZB_Band = 25;
    gateway.ZB_Channel = 16;

    strcpy(gateway.ZB_PAN_ID,   "8215");
    strcpy(gateway.EXT_PAN_ID,  "000D6F000ED34E34"); 
    strcpy(gateway.ZB_CO_MAC,   "000D6F000ED34E34");
    strcpy(gateway.NETWORK_KEY, "21B9F385F114B1C4AE07D5753B95355D");

    /*
     * please set init parameter
     */
    initParams = linkkit_gateway_get_default_params();
    if (!initParams)
        return -1;
    /* LINKKIT_OPT_MAX_MSG_SIZE: max size of message */
    maxMsgSize = 20 * 1024;
    linkkit_gateway_setopt(initParams, LINKKIT_OPT_MAX_MSG_SIZE, &maxMsgSize, sizeof(int));    
    /* LINKKIT_OPT_MAX_MSG_QUEUE_SIZE: max size of message queue */
    maxMsgQueueSize = 8;
    linkkit_gateway_setopt(initParams, LINKKIT_OPT_MAX_MSG_QUEUE_SIZE, &maxMsgQueueSize, sizeof(int));    

	prop_post_reply = 1;
	linkkit_gateway_setopt(initParams, LINKKIT_OPT_PROPERTY_POST_REPLY, &prop_post_reply, sizeof(int));
		
	event_post_reply = 1;
	linkkit_gateway_setopt(initParams, LINKKIT_OPT_EVENT_POST_REPLY, &event_post_reply, sizeof(int));
	
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

    while (gateway.register_completed == 0)
        HAL_SleepMs(1000);

    linkkit_gateway_fota_init(linkkit_fota_callback);

    /*
     * subdev start
     */
    int res = 0;
    char *subdev_pk = "a1NGqAVowRX";
    char *subdev_dn = "example1";
    char *subdev_ds = "HSbPuKvf0ekZff5ARWJDWKuyPTdQh5wb";
    int devid = 0;

    while (1) {
        /*
         * gateway trigger event
         * please follow user's case, modify this logic
         */
        /* linkkit_gateway_trigger_event_json(gateway.lk_dev,"testEventODmkIIcDwj","{\"output\":0}",10000,trigger_event_callback,NULL); */
        /* linkkit_gateway_trigger_event_json_sync(gateway.lk_dev, "testEventODmkIIcDwj", "{\"output\":0}", 10000); */
        EXAMPLE_TRACE("=================================linkkit_gateway_subdev_create=======================================\n");
        HAL_SleepMs(2000);
        res = linkkit_gateway_subdev_create(subdev_pk,subdev_dn,&linkkit_cbs,(void *)&subdev);
        if (res < SUCCESS_RETURN) {break;}
        devid = res;
        subdev.lk_dev = devid;
        EXAMPLE_TRACE("linkkit_gateway_subdev_create, devid: %d\n",devid);
        
        EXAMPLE_TRACE("=================================linkkit_gateway_subdev_register=======================================\n");
        HAL_SleepMs(2000);
        res = linkkit_gateway_subdev_register(subdev_pk,subdev_dn,subdev_ds);
        if (res != SUCCESS_RETURN) {break;}
        
        EXAMPLE_TRACE("=================================linkkit_gateway_subdev_login=======================================\n");
        HAL_SleepMs(2000);
        res = linkkit_gateway_subdev_login(devid);
        if (res != SUCCESS_RETURN) {break;}

        EXAMPLE_TRACE("=================================linkkit_gateway_post_property_json_sync=======================================\n");
        HAL_SleepMs(2000);
        res = linkkit_gateway_post_property_json_sync(devid,"{\"LightSwitch\":1}",10000);
        if (res != SUCCESS_RETURN) {break;}

        EXAMPLE_TRACE("=================================linkkit_gateway_trigger_event_json_sync=======================================\n");
        HAL_SleepMs(2000);
        res = linkkit_gateway_trigger_event_json_sync(devid,"Error","{\"ErrorCode\":0}",10000);
        if (res != SUCCESS_RETURN) {break;}

        EXAMPLE_TRACE("=================================linkkit_gateway_subdev_logout=======================================\n");
        HAL_SleepMs(2000);
        res = linkkit_gateway_subdev_logout(devid);
        if (res != SUCCESS_RETURN) {break;}

        EXAMPLE_TRACE("=================================linkkit_gateway_subdev_unregister=======================================\n");
        HAL_SleepMs(2000);
        res = linkkit_gateway_subdev_unregister(subdev_pk,subdev_dn);
        if (res != SUCCESS_RETURN) {break;}

        EXAMPLE_TRACE("=================================linkkit_gateway_subdev_destroy=======================================\n");
        HAL_SleepMs(2000);
        res = linkkit_gateway_subdev_destroy(devid);

        HAL_SleepMs(5000);
        /* break; */
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
