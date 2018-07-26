#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include "aos/aos.h"
#include "json.h"
#include "linkkit_gateway_export.h"
#include "iot_import.h"

#define ALINK_KEY_MAX_NUMBER 5

typedef enum ALINK_LOCK_VOLUME_TYPES {
	ALINK_LOCK_VOLUME_MUTE,/*����*/
	ALINK_LOCK_VOLUME_LOW,/*����*/
	ALINK_LOCK_VOLUME_MIDDLE,/*����*/
	ALINK_LOCK_VOLUME_HIGI,/*����*/
    ALINK_LOCK_VOLUME_MAX
} alink_lock_volume_types_t;

/*������*/
typedef enum ALINK_LOCK_LANGUAGE_TYPES {
	ALINK_LOCK_LANGUAGE_CN,
	ALINK_LOCK_LANGUAGE_EN,
    ALINK_LOCK_LANGUAGE_MAX
} alink_lock_language_types_t;

/*��״̬*/
typedef enum ALINK_LOCK_STATE_TYPES {
	ALINK_LOCK_STATE_UNLOCK,
	ALINK_LOCK_STATE_LOCK,
} alink_lock_state_types_t;


/*Կ������*/
typedef enum ALINK_KEY_TYPES {
	ALINK_KEY_TYPES_FINGER_PRINTF=1,/*ָ��*/
	ALINK_KEY_TYPES_PW,/*����*/
	ALINK_KEY_TYPES_CARD,/*��*/
	ALINK_KEY_TYPES_KEY,/*��еԿ��*/
    ALINK_KEY_TYPES_MAX
} alink_key_types_t;
	
/*�û�Ȩ��*/
typedef enum ALINK_KEY_LIMIT_TYPES {
	ALINK_KEY_LIMIT_NORMAL=1,/*ָ��*/
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
    lock_property *lock_prop = (lock_property *)ctx;
    AJsonObject root;
	if (AJson_Parse(&root, in, strlen(in)) < 0)
		return -1;

	char *prop = NULL;
	int   len = 0;

	char *ptr = out;
	char *end = out + out_len;

	strncat(ptr, "{", end - ptr);
	ptr += strlen(ptr);

	int count = 0;
	AJson_ForEachObjectInArray(&root, obj) {
		if (AJson_GetString(obj, &prop, &len) < 0)
			break;

		if (strncmp(prop, LOCK_PROPERTY_BATTERY, len) == 0) {
			if (count > 0) {
				strncat(ptr, ", ", end - ptr);
				ptr += strlen(ptr);
			}
			ptr += snprintf(ptr, end - ptr, "\"%s\": %d", LOCK_PROPERTY_BATTERY, lock_prop->BatteryPercentage);
			count++;
		} else if (strncmp(prop, LOCK_PROPERTY_VOLUME, len) == 0) {
            if (count > 0) {
				strncat(ptr, ", ", end - ptr);
				ptr += strlen(ptr);
			}
			ptr += snprintf(ptr, end - ptr, "\"%s\": %d", LOCK_PROPERTY_VOLUME, lock_prop->Volume);
			count++;
        } else if (strncmp(prop, LOCK_PROPERTY_LANGUAGE, len) == 0) {
			if (count > 0) {
				strncat(ptr, ", ", end - ptr);
				ptr += strlen(ptr);
			}
			ptr += snprintf(ptr, end - ptr, "\"%s\": %d",LOCK_PROPERTY_LANGUAGE, lock_prop->Language);
			count++;
		} else if (strncmp(prop, LOCK_PROPERTY_STATE, len) == 0) {
			if (count > 0) {
				strncat(ptr, ", ", end - ptr);
				ptr += strlen(ptr);
			}
			ptr += snprintf(ptr, end - ptr, "\"%s\": %d", LOCK_PROPERTY_STATE, lock_prop->LockState);
			count++;
		} else if (strncmp(prop, LOCK_PROPERTY_SN, len) == 0) {
			if (count > 0) {
				strncat(ptr, ", ", end - ptr);
				ptr += strlen(ptr);
			}
			ptr += snprintf(ptr, end - ptr, "\"%s\": \"%s\"", LOCK_PROPERTY_SN, lock_prop->SN);
			count++;
		}
	}

	strncat(ptr, "}", end - ptr);
	ptr += strlen(ptr);

	if (count == 0)
		out[0] = '\0';

	LOG("out: %s\n", out);
    return 0;
}
int testcmd_lock_set_property(char *in, void *ctx)
{
    lock_property *prop = (lock_property *)ctx;

	char *str=NULL;
	int len=0;
	
	AJsonObject root;
	if (AJson_Parse(&root, in, strlen(in)) < 0)
		return -1;

	AJson_ForEachObject(&root, k, k_len, obj) {
		if (strncmp(k, LOCK_PROPERTY_BATTERY, k_len) == 0) {
			if (AJson_GetInt(obj, &prop->BatteryPercentage) < 0)
				break;
		} else if (strncmp(k, LOCK_PROPERTY_VOLUME, k_len) == 0) {
			if (AJson_GetInt(obj, (int *)&prop->Volume) < 0)
				break;
		} else if (strncmp(k, LOCK_PROPERTY_LANGUAGE, k_len) == 0) {
			if (AJson_GetInt(obj, (int *)&prop->Language) < 0)
				break;
		}else if (strncmp(k, LOCK_PROPERTY_STATE, k_len) == 0) {
			if (AJson_GetInt(obj, (int *)&prop->LockState) < 0)
				break;
		} else if (strncmp(k, LOCK_PROPERTY_SN, k_len) == 0) {
			if (AJson_GetString(obj, &str, &len) < 0)
				break;
			snprintf(prop->SN, len+1, "%.*s", len, str);
		}
	}
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
	char *pvalue=NULL;
    alink_key_t *key;

	if (strcmp(identifier, SERVICE_KEY_ADD) == 0) {
        pvalue = LITE_json_value_of(KEY_PROPERTY_TYPE, in);
		if (!pvalue){
			LOG("No Type section found!\n");
            return -1;
        }
		type = atoi(pvalue);
		HAL_Free(pvalue);

		pvalue = LITE_json_value_of(KEY_PROPERTY_LIMIT, in);
        if (!pvalue) {
            LOG("No Limit section found!\n");
            return -1;
		}
        limit  = atoi(pvalue);
        HAL_Free(pvalue);

        LOG("type = %d, limit = %d\n", type, limit);

        key = lock_key_add(prop, type, limit);
        if (!key) {
            LOG("add key failed!\n");
            return -1;
        }
        lock_key_add_event(devid, key);

        snprintf(out, out_len,"{\"%s\": %d, \"%s\": %d}", KEY_PROPERTY_TYPE, type, KEY_PROPERTY_TYPE, limit);
    } else if (strcmp(identifier, SERVICE_KEY_DEL) == 0) {
        pvalue = LITE_json_value_of(KEY_PROPERTY_ID, in);
        if (!pvalue) {
            LOG("No KeyID section found!\n");
            return -1;
        }
        strcpy(keyid, pvalue);
        HAL_Free(pvalue);
        LOG("keyid = %s, pvalue=%s, strlen=%d\n", keyid, pvalue, strlen(pvalue));

        pvalue = LITE_json_value_of(KEY_PROPERTY_TYPE, in);
        if (!pvalue) {
            LOG("No Type section found!\n");
            return -1;
        }
        type = atoi(pvalue);
        HAL_Free(pvalue);

        LOG("keyname=%s, type=%d\n", keyid, type);

        key = lock_key_del(prop,keyid);
        if (!key) {
            LOG("delete key failed!\n");
            return -1;
        }
        lock_key_delete_event(devid, key);

        snprintf(out, sizeof(json),"{\"%s\": \"%s\", \"%s\": %d}", KEY_PROPERTY_ID, keyid, KEY_PROPERTY_TYPE, type);
    } else if (strcmp(identifier, SERVICE_KEY_LIST) == 0) {
        char json[64];
        int j;
        int count = 0, sec_len = 0;
        pvalue = LITE_json_value_of(KEY_PROPERTY_TYPE, in);
        if (!pvalue) {
            LOG("No LockType section found!\n");
            return -1;
        }
        type = atoi(pvalue);
		HAL_Free(pvalue);
        
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
                    return -1;
                }
                count++;
            }
        }
        if (sec_len + 1 < out_len) {
            strcat(out, "]");
        } else {
            LOG("Buffer is not enough!\n");
            return -1;
        }
    } else if (strcmp(identifier, "SetKeyPermission")) {
        pvalue = LITE_json_value_of(KEY_PROPERTY_TYPE, in);
		if (!pvalue){
			LOG("No Type section found!\n");
            return -1;
        }
		type = atoi(pvalue);
		HAL_Free(pvalue);

		pvalue = LITE_json_value_of(KEY_PROPERTY_LIMIT, in);
        if (!pvalue) {
            LOG("No Limit section found!\n");
            return -1;
		}
        limit  = atoi(pvalue);
        HAL_Free(pvalue);
        pvalue = LITE_json_value_of(KEY_PROPERTY_ID, in);
        if (!pvalue) {
            LOG("No KeyID section found!\n");
            return -1;
		}
        strcpy(keyid, pvalue);
        HAL_Free(pvalue);

        lock_set_key_permission(prop, keyid, type, limit); 
    }

    return 0;
}
