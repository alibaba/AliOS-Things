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
#include <string.h>
#include <stdlib.h>
#include <string.h>

#include "linkkit_gateway_export.h"
#include "light.h"
#include "cJSON.h"

#define NELEMS(x)   (sizeof(x) / sizeof((x)[0]))

#define EXAMPLE_TRACE(...)                                      \
do {                                                     \
    printf("\033[1;31;40m%s.%d: ", __func__, __LINE__);  \
    printf(__VA_ARGS__);                                 \
    printf("\033[0m");                                   \
} while (0)

/* subdev max number
 * please modify this setting to support more subdev, but please note the memory.
 */
#define LIGHT_MAX_NUM         (2)

/* subdev - light's preproty */
typedef struct {
    int  devid;
    char productKey[32];
    char deviceName[64];
    char deviceSecret[64];

    int LightSwitch;
    int NightLightSwitch;
    int ColorTemperature;
} light_t;


/* subdev - light's element */
typedef struct {
    char *productKey;
    char *deviceName;
    char *deviceSecret;
} light_conf_t;


/* light's pk, dn, and ds */
static const light_conf_t light_maps[] = {
    {"a1vj9KBli0Y", "light_1", "WoMEzdQEsfnsVOytvENSyFYNryQngqS8"},
    {"a1vj9KBli0Y", "light_2", "XIMo1UAyDjLbzf0MjhErWYizNKWc8Md3"},
};

static light_t *lights[LIGHT_MAX_NUM];

static int light_get_property(char *in, char *out, int out_len, void *ctx);
static int light_set_property(char *in, void *ctx);
static int light_call_service(char *identifier, char *in, char *out, int out_len, void *ctx);


/* callback function */
static linkkit_cbs_t light_cbs = {
    .get_property = light_get_property,
    .set_property = light_set_property,
    .call_service = light_call_service,
};


/*
 * the handler property get
 * alink method: thing.service.property.get
 */
static int light_get_property(char *in, char *out, int out_len, void *ctx)
{
    light_t *light = ctx;
    cJSON *rJson, *pJson;
    int iSize, i;
    char *p = NULL;

    EXAMPLE_TRACE("in: %s\n", in);

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

        if (strcmp(pSub->valuestring, "LightSwitch") == 0) {
            cJSON_AddNumberToObject(pJson, "LightSwitch", light->LightSwitch);
        } else if (strcmp(pSub->valuestring, "NightLightSwitch") == 0) {
            cJSON_AddNumberToObject(pJson, "NightLightSwitch", light->NightLightSwitch);
        } else if (strcmp(pSub->valuestring, "ColorTemperature") == 0) {
            cJSON_AddNumberToObject(pJson, "ColorTemperature", light->ColorTemperature);
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
        free(p);
        return -1;
    }

    strcpy(out, p);

    EXAMPLE_TRACE("out: %s\n", out);

    cJSON_Delete(rJson);
    cJSON_Delete(pJson);
    free(p);

    return 0;
}

/*
 * the handler property set
 * alink method: thing.service.property.set
 */
static int light_set_property(char *in, void *ctx)
{
    light_t *light = ctx;
    cJSON *rJson, *LightSwitch, *NightLightSwitch, *ColorTemperature;

    EXAMPLE_TRACE("%s.%s: in %s\n", light->productKey, light->deviceName, in);

    /* parse input json, set the gateway value */
    rJson = cJSON_Parse(in);
    if (!rJson)
        return -1;

    LightSwitch = cJSON_GetObjectItem(rJson, "LightSwitch");
    if (LightSwitch)
        light->LightSwitch = LightSwitch->valueint;

    NightLightSwitch = cJSON_GetObjectItem(rJson, "NightLightSwitch");
    if (NightLightSwitch)
        light->NightLightSwitch = NightLightSwitch->valueint;

    ColorTemperature = cJSON_GetObjectItem(rJson, "ColorTemperature");
    if (ColorTemperature)
        light->ColorTemperature = ColorTemperature->valueint;

    cJSON_Delete(rJson);

    linkkit_gateway_post_property_json_sync(light->devid, in, 10000);

    return 0;
}


/*
 * the handler of service which is defined by identifier, not property
 * alink method: thing.service.{tsl.service.identifier}
 */
static int light_call_service(char *identifier, char *in, char *out, int out_len, void *ctx)
{
    light_t *light = ctx;

    EXAMPLE_TRACE("%s.%s: in %s\n", light->productKey, light->deviceName, in);

    /*
     * please follow user's TSL to mofidy this id - SetTimerTask and TimeReset.
     * exmaple just reply a "hello world".
     */
    linkkit_gateway_post_property_json_sync(light->devid, "{\"SetTimer\": \"hello, world!\"}", 5000);

    return 0;
}


/*
 * subdev init function
 * please follow this function to add different subdev
 */
int light_init(void)
{
    int i;
    for (i = 0; i < LIGHT_MAX_NUM; i++) {
        light_t *light = malloc(sizeof(light_t));
        if (!light)
            break;
        memset(light, 0, sizeof(light_t));

        const light_conf_t *conf = &light_maps[i];

        strncpy(light->productKey,   conf->productKey,   sizeof(light->productKey) - 1);
        strncpy(light->deviceName,   conf->deviceName,   sizeof(light->deviceName) - 1);
        strncpy(light->deviceSecret, conf->deviceSecret, sizeof(light->deviceSecret) - 1);

        light->LightSwitch = 0;
        light->NightLightSwitch = 0;
        light->ColorTemperature = 4500;

        /*
         * create subdev
         * return the device id.
         */
        light->devid = linkkit_gateway_subdev_create(light->productKey, light->deviceName, &light_cbs, light);
        if (light->devid < 0) {
            EXAMPLE_TRACE("linkkit_gateway_subdev_create %s<%s> failed\n", light->deviceName, light->productKey);
            linkkit_gateway_subdev_unregister(light->productKey, light->deviceName);
            free(light);
            break;
        }

        /*
         * register subdev
         */
        if (linkkit_gateway_subdev_register(light->productKey, light->deviceName, light->deviceSecret) < 0) {
            free(light);
            break;
        }

        /*
         * login subdev
         */
        #if (0)
        if (linkkit_gateway_subdev_login(light->devid) < 0) {
            EXAMPLE_TRACE("linkkit_gateway_subdev_login %s<%s> failed\n", light->deviceName, light->productKey);
            linkkit_gateway_subdev_destroy(light->devid);
            linkkit_gateway_subdev_unregister(light->productKey, light->deviceName);
            free(light);
            break;
        }
        #endif

        lights[i] = light;
    }

    return 0;
}


/* subdev exit */
int light_exit(void)
{
    int i;
    for (i = 0; i < LIGHT_MAX_NUM; i++) {
        light_t *light = lights[i];
        if (!light)
            continue;

        /*
         * logout subdev
         */
        linkkit_gateway_subdev_logout(light->devid);
        /*
         * destory subdev
         */
        linkkit_gateway_subdev_destroy(light->devid);
        /*
         * unregister subdev
         */
        linkkit_gateway_subdev_unregister(light->productKey, light->deviceName);
        free(light);

        lights[i] = NULL;
    }

    return 0;
}

