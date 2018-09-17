/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include "aos/aos.h"
#include "cJSON.h"
#include "linkkit_gateway_export.h"
#include "iot_import.h"
extern char *LITE_json_value_of(char *key, char *src, ...);


#define ALINK_KEY_MAX_NUMBER 5

typedef enum ALINK_LOCK_VOLUME_TYPES {
	ALINK_LOCK_VOLUME_MUTE,
	ALINK_LOCK_VOLUME_LOW,
	ALINK_LOCK_VOLUME_MIDDLE,
	ALINK_LOCK_VOLUME_HIGI,
    ALINK_LOCK_VOLUME_MAX
} alink_lock_volume_types_t;

typedef enum ALINK_LOCK_LANGUAGE_TYPES {
	ALINK_LOCK_LANGUAGE_CN,
	ALINK_LOCK_LANGUAGE_EN,
    ALINK_LOCK_LANGUAGE_MAX
} alink_lock_language_types_t;

typedef enum ALINK_LOCK_STATE_TYPES {
	ALINK_LOCK_STATE_UNLOCK,
	ALINK_LOCK_STATE_LOCK,
} alink_lock_state_types_t;

typedef enum ALINK_KEY_TYPES {
	ALINK_KEY_TYPES_FINGER_PRINTF=1,
	ALINK_KEY_TYPES_PW,
	ALINK_KEY_TYPES_CARD,
	ALINK_KEY_TYPES_KEY,
    ALINK_KEY_TYPES_MAX
} alink_key_types_t;
	
typedef enum ALINK_KEY_LIMIT_TYPES {
	ALINK_KEY_LIMIT_NORMAL=1,
	ALINK_KEY_LIMIT_ADMIN,	
	ALINK_KEY_LIMIT_HIJACK,
	ALINK_KEY_LIMIT_MAX
} alink_key_limit_types_t;

typedef struct {
    char *productKey;
    char *deviceName;
    char *deviceSecret;
} lock_conf_t;

/*key info*/
typedef struct alink_key_s {

	char keyid[11];
	alink_key_types_t type;
	alink_key_limit_types_t limit;
	int use;
} alink_key_t;
typedef struct _lock_property
{
    int BatteryPercentage;
	alink_lock_volume_types_t Volume;
	alink_lock_language_types_t Language;
	alink_lock_state_types_t LockState;
	char SN[51];
	alink_key_t key[ALINK_KEY_MAX_NUMBER];
	int keyCount;
}lock_property;

#if 0 //def ON_DAILY
/* for lock */
#define LOCK_PROPERTY_BATTERY  "Battery"
#define LOCK_PROPERTY_VOLUME   "Volume"
#define LOCK_PROPERTY_LANGUAGE "Language"
#define LOCK_PROPERTY_SN       "SN"
#define LOCK_PROPERTY_STATE    "State"
#define SERVICE_KEY_ADD        "KeyAdd"
#define SERVICE_KEY_DEL        "KeyDelete"
#define SERVICE_KEY_LIST       "KeyList"
#define SERVICE_SET_KEY_PERM   "SetKeyPermission"
#define EVENT_KEY_ADD          "KeyAddAlarm"
#define EVENT_KEY_DEL          "KeyDeleteAlarm"

/* for key */
#define KEY_PROPERTY_ID        "ID"
#define KEY_PROPERTY_TYPE      "Type"
#define KEY_PROPERTY_LIMIT     "Limit"

#else
/* for lock */
#define LOCK_PROPERTY_BATTERY  "BatteryPercentage"
#define LOCK_PROPERTY_VOLUME   "Volume"
#define LOCK_PROPERTY_LANGUAGE "Language"
#define LOCK_PROPERTY_SN       "SN"
#define LOCK_PROPERTY_STATE    "LockState"
#define SERVICE_KEY_ADD        "AddKey"
#define SERVICE_KEY_DEL        "DeleteKey"
#define SERVICE_KEY_LIST       "GetKeyList"
#define SERVICE_SET_KEY_PERM   "SetKeyPermission"
#define EVENT_KEY_ADD          "KeyAddedNotification"
#define EVENT_KEY_DEL          "KeyDeletedNotification"

/* for key */
#define KEY_PROPERTY_ID        "KeyID"
#define KEY_PROPERTY_TYPE      "LockType"
#define KEY_PROPERTY_LIMIT     "UserLimit"
#endif

void *testcmd_lock_alloc_property()
{   
    lock_property *prop =  malloc(sizeof(lock_property));
    int i;
    if (prop) {
        memset(prop, 0, sizeof(*prop));
        for (i = 0; i < sizeof(prop->key) / sizeof(prop->key[0]); i++) {
            prop->key[i].use = 0;
            snprintf(prop->key[i].keyid, sizeof(prop->key[i].keyid), "KeyNO.%d", i + 1);
        }
    }
    return (void *)prop;
}

void testcmd_lock_destroy_property(void **p_prop)
{
    lock_property *prop = (lock_property *)(*p_prop);

    free(prop);

    *p_prop = NULL;
}

int testcmd_lock_get_propery(char *in, char *out, int out_len, void *ctx)
{
    lock_property *prop = ctx;
    cJSON     *rJson, *pJson;
    char      *p;
    int        iSize, i;

    LOG("in: %s\n", in);

    if (!prop) {
        LOG("lock not found\n");
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
        if (strcmp(pSub->valuestring, LOCK_PROPERTY_BATTERY) == 0) {
            cJSON_AddNumberToObject(pJson, LOCK_PROPERTY_BATTERY, prop->BatteryPercentage);
        } else if (strcmp(pSub->valuestring, LOCK_PROPERTY_VOLUME) == 0) {
            cJSON_AddNumberToObject(pJson, LOCK_PROPERTY_VOLUME, prop->Volume);
        } else if (strcmp(pSub->valuestring, LOCK_PROPERTY_LANGUAGE) == 0) {
            cJSON_AddNumberToObject(pJson, LOCK_PROPERTY_LANGUAGE, prop->Language);
        } else if (strcmp(pSub->valuestring, LOCK_PROPERTY_STATE) == 0) {
            cJSON_AddNumberToObject(pJson, LOCK_PROPERTY_STATE, prop->LockState);
        } else if (strcmp(pSub->valuestring, LOCK_PROPERTY_SN) == 0) {
            cJSON_AddStringToObject(pJson, LOCK_PROPERTY_SN, prop->SN);
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

    LOG("out: %s\n", out);

    cJSON_Delete(rJson);
    cJSON_Delete(pJson);
    HAL_Free(p);

    return 0;
}

int testcmd_lock_set_property(char *in, void *ctx)
{
    lock_property *prop = (lock_property *)ctx;
    cJSON *rJson, *barrery, *volume, *language, *lock_state, *sn;

    LOG("in: %s\n", in);

    rJson = cJSON_Parse(in);
    if (!rJson) {
        return -1;
    }

    /* parse input json, set the gateway value */
    barrery = cJSON_GetObjectItem(rJson, LOCK_PROPERTY_BATTERY);
    if (barrery) {
        prop->BatteryPercentage = barrery->valueint;
    }

    volume = cJSON_GetObjectItem(rJson, LOCK_PROPERTY_VOLUME);
    if (volume) {
        prop->Volume = volume->valueint;
    }

    language = cJSON_GetObjectItem(rJson,LOCK_PROPERTY_LANGUAGE);
    if (language)
        prop->Language = language->valueint;

    lock_state = cJSON_GetObjectItem(rJson,LOCK_PROPERTY_STATE);
    if (lock_state)
        prop->LockState = lock_state->valueint;


    sn = cJSON_GetObjectItem(rJson, LOCK_PROPERTY_SN);
    if (sn)
        strncpy(prop->SN, sn->valuestring,
                sizeof(prop->SN) - 1);

    cJSON_Delete(rJson);

	LOG("lock->BatteryPercentage = %d\n", prop->BatteryPercentage);
	LOG("lock->Volume = %d\n", prop->Volume);
	LOG("lock->Language = %d\n", prop->Language);
	LOG("lock->LockState = %d\n", prop->LockState);
	LOG("lock->SN = %s\n", prop->SN);
    return 0;
}


static alink_key_t *lock_key_add(lock_property *prop, int type, int limit)
{
    int i;
    alink_key_t *key = NULL;

    for (i = 0; i < sizeof(prop->key) / sizeof(prop->key[0]); i++) {
        if (!prop->key[i].use) {
            key = &prop->key[i];
            key->type = type;
            key->limit = limit;
            key->use = 1;
            break;
        }
    }
    return key;
}

static alink_key_t *lock_key_del(lock_property *prop, char *keyid)
{
    int i;
    alink_key_t *key = NULL;

    for (i = 0; i < sizeof(prop->key) / sizeof(prop->key[0]); i++) {
        key = &prop->key[i];
        if (key->use && !strcmp(key->keyid, keyid)) {
            key->use = 0;
            break;
        }
        key = NULL;
    }
    return key;
}

static int lock_key_add_event(int devid, const alink_key_t *key)
{
    char json[64] = {0};
    snprintf(json, sizeof(json),
            "{\"%s\": \"%s\", \"%s\": %d, \"%s\": %d}", 
            KEY_PROPERTY_ID, key->keyid, 
            KEY_PROPERTY_TYPE, key->type, 
            KEY_PROPERTY_LIMIT, key->limit);
	linkkit_gateway_trigger_event_json_sync(devid, EVENT_KEY_ADD, json, 1000);
    return 0;
}

static int lock_key_delete_event(int devid, const alink_key_t *key)
{
    char json[64] = {0};
    snprintf(json, sizeof(json),
            "{\"%s\": \"%s\", \"%s\": %d, \"%s\": %d}", 
            KEY_PROPERTY_ID, key->keyid, 
            KEY_PROPERTY_TYPE, key->type, 
            KEY_PROPERTY_LIMIT, key->limit);
	linkkit_gateway_trigger_event_json_sync(devid, EVENT_KEY_DEL, json, 1000);
    return 0;
}

static int lock_set_key_permission(lock_property *prop, const char *keyid, int type, int limit)
{
    int i;
    alink_key_t *key = NULL;
    int ret = -1;

    for (i = 0; i < sizeof(prop->key) / sizeof(prop->key[0]); i++) {
        key = &prop->key[i];
        if (key->use && !strcmp(keyid, key->keyid)) {
            key->type = type;
            key->limit = limit;
            ret = 0;
            break;
        }
    }
    
    return ret;
}

int testcmd_lock_call_service(int devid, char *identifier, char *in, char *out, int out_len, void *ctx)
{
    lock_property *prop = (lock_property *)ctx;
    char json[64] = {0};
    char keyid[11];
	alink_key_types_t type=1;
	alink_key_limit_types_t limit=1;
    alink_key_t *key;

    cJSON *rJson;
    rJson = cJSON_Parse(in);
    if (!rJson) {
        return -1;
    }


	if (strcmp(identifier, SERVICE_KEY_ADD) == 0) {
        cJSON *ptype, *plimit;
        ptype = cJSON_GetObjectItem(rJson, KEY_PROPERTY_TYPE);
        if (!ptype) {
            goto failed;    
        }

        plimit = cJSON_GetObjectItem(rJson, KEY_PROPERTY_LIMIT);
        if (plimit) {
            goto failed; 
        }
        
        type = ptype->valueint;
        limit = plimit->valueint;
        LOG("type = %d, limit = %d\n", type, limit);

        key = lock_key_add(prop, type, limit);
        if (!key) {
            LOG("add key failed!\n");
            goto failed; 
        }
        lock_key_add_event(devid, key);

        snprintf(out, out_len,"{\"%s\": %d, \"%s\": %d}", KEY_PROPERTY_TYPE, type, KEY_PROPERTY_TYPE, limit);
    } else if (strcmp(identifier, SERVICE_KEY_DEL) == 0) {
        cJSON *pkeyid, *ptype;
        pkeyid = cJSON_GetObjectItem(rJson, KEY_PROPERTY_ID);
        if (!pkeyid) {
            goto failed;
        }

        ptype = cJSON_GetObjectItem(rJson, KEY_PROPERTY_TYPE);
        if (!ptype) {
            goto failed;
        }

        strncpy(keyid, pkeyid->valuestring,
                sizeof(keyid) - 1);
        type = ptype->valueint;

        LOG("keyname=%s, type=%d\n", keyid, type);

        key = lock_key_del(prop,keyid);
        if (!key) {
            LOG("delete key failed!\n");
            goto failed;
        }
        lock_key_delete_event(devid, key);

        snprintf(out, sizeof(json),"{\"%s\": \"%s\", \"%s\": %d}", KEY_PROPERTY_ID, keyid, KEY_PROPERTY_TYPE, type);
    } else if (strcmp(identifier, SERVICE_KEY_LIST) == 0) {
        char json[64];
        int j;
        int count = 0, sec_len = 0;

        cJSON *ptype;

        ptype = cJSON_GetObjectItem(rJson, KEY_PROPERTY_TYPE);
        if (!ptype) {
            goto failed;
        }
        type = ptype->valueint;
        
        LOG("Get key list");
        snprintf(json, sizeof(json), "[");
        if (strlen(json) < out_len) {
            strcat(out, json);
            sec_len += strlen(json);
        }
        for(j=0;j<ALINK_KEY_MAX_NUMBER;j++){
            alink_key_t *key = &prop->key[j];
            if (1 == key->use && key->type == type) {
                memset(json, 0, sizeof(json));
                snprintf(json, sizeof(json),"%s{\"%s\": \"%s\", \"%s\": %d, \"%s\": %d}",
                        count == 0 ? "" : ",",
                        KEY_PROPERTY_ID, key->keyid, 
                        KEY_PROPERTY_TYPE, key->type, 
                        KEY_PROPERTY_LIMIT, key->limit);

                if (sec_len + strlen(json) < out_len) {
                    strcat(out, json);
                    sec_len += strlen(json);
                } else {
                    LOG("Buffer is not enough!\n");
                    goto failed;
                }
                count++;
            }
        }
        if (sec_len + 1 < out_len) {
            strcat(out, "]");
        } else {
            LOG("Buffer is not enough!\n");
            goto failed;
        }
    } else if (strcmp(identifier, "SetKeyPermission")) {

        cJSON *pkeyid, *ptype,*plimit;
        pkeyid = cJSON_GetObjectItem(rJson, KEY_PROPERTY_ID);
        if (!pkeyid) {
            goto failed;
        }

        ptype = cJSON_GetObjectItem(rJson, KEY_PROPERTY_TYPE);
        if (!ptype) {
            goto failed;
        }

        plimit = cJSON_GetObjectItem(rJson, KEY_PROPERTY_LIMIT);
        if (!plimit) {
            goto failed;
        }
        strncpy(keyid, pkeyid->valuestring,
                sizeof(keyid) - 1);
        type = ptype->valueint;
        limit = plimit->valueint;
    
        lock_set_key_permission(prop, keyid, type, limit); 
    }
    cJSON_Delete(rJson);
    return 0;
failed:
    cJSON_Delete(rJson);
    return -1;

}
