/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include "product.h"

//TODO: update these product info
#define light_model           "ALINKTEST_LIVING_LIGHT_ALINK_TEST"
#define light_key             "5gPFl8G4GyFZ1fPWk20m"
#define light_secret          "ngthgTlZ65bX5LpViKIWNsDPhOf2As9ChnoL9gQb"
#define light_debug_key       "dpZZEpm9eBfqzK7yVeLq"
#define light_debug_secret    "THnfRRsU5vu6g6m9X6uFyAjUWflgZ0iyGjdEneKm"

#define gateway_model           "ALINKTEST_SECURITY_GATEWAY_QUANWU_001"
#define gateway_key             "V2hpRG0k7Pbr1bmxDCat"
#define gateway_secret          "O71DlsrrTkImG0NxowxaA5oFFjyxTj1n8FwWzOJv"
#define gateway_debug_key       "dpZZEpm9eBfqzK7yVeLq"
#define gateway_debug_secret    "THnfRRsU5vu6g6m9X6uFyAjUWflgZ0iyGjdEneKm"

#define atalk_model           "ALINKTEST_ENTERTAINMENT_ATALK_SDS_TEST"
#define atalk_key             "JeiQyf0m9WjwwZ5J1XXM"
#define atalk_secret          "YXEIV7wOPtwyotVehpx6RchDI8IbmyzMiKx0fEqJ"
#define atalk_debug_key       "dpZZEpm9eBfqzK7yVeLq"
#define atalk_debug_secret    "THnfRRsU5vu6g6m9X6uFyAjUWflgZ0iyGjdEneKm"

static const char *product_model = light_model;
static const char *product_key = light_key;
static const char *product_secret = light_secret;
static const char *product_debug_key = light_debug_key;
static const char *product_debug_secret = light_debug_secret;
#ifdef ALINK_PRODUCT_ATALK  
#define device_key              "Nt3DBTznRFAX9fYrpEan"
#define device_secret           "KlkoyDKdsEFU1dy5Lv8tZmmlltABY3qB"
#else    
#define device_key              "1hldEkvAUMhdtyKeqUD1"
#define device_secret           "u531CJi3mDmeDELKFPML6EzdSDpmedEF"
#endif  
void product_init(void)
{
#ifdef ALINK_PRODUCT_ATALK  
    char model[PRODUCT_MODEL_LEN] = "atalk";
#else    
    char model[PRODUCT_MODEL_LEN] = "light";
#endif    
    int  model_len = sizeof(model);
    aos_kv_get("model", model, &model_len);
    if (!strcmp(model, "gateway")) {
        product_model = gateway_model;
        product_key = gateway_key;
        product_secret = gateway_secret;
        product_debug_key = gateway_debug_key;
        product_debug_secret = gateway_debug_secret;
    } else if (!strcmp(model, "atalk")) {
        product_model = atalk_model;
        product_key = atalk_key;
        product_secret = atalk_secret;
        product_debug_key = atalk_debug_key;
        product_debug_secret = atalk_debug_secret;
    } else {
        product_model = light_model;
        product_key = light_key;
        product_secret = light_secret;
        product_debug_key = light_debug_key;
        product_debug_secret = light_debug_secret;
    }
}


char *product_get_device_key(char key_str[DEVICE_KEY_LEN])
{
    int key_len = DEVICE_KEY_LEN;
    if (aos_kv_get("sds_key", key_str, &key_len) < 0) {
        strncpy(key_str, device_key, DEVICE_KEY_LEN);
    }
    return key_str;
}

char *product_get_device_secret(char secret_str[DEVICE_SECRET_LEN])
{
    int sec_len = DEVICE_SECRET_LEN;
    if (aos_kv_get("sds_secret", secret_str, &sec_len) < 0) {
        strncpy(secret_str, device_secret, DEVICE_SECRET_LEN);
    }
    return secret_str;
}


char *product_get_name(char name_str[PRODUCT_NAME_LEN])
{
    return strncpy(name_str, "alink_product", PRODUCT_NAME_LEN);
}

char *product_get_version(char ver_str[PRODUCT_VERSION_LEN])
{
    return strncpy(ver_str, (const char *)aos_get_os_version(),
                   PRODUCT_VERSION_LEN);
}

char *product_get_model(char model_str[PRODUCT_MODEL_LEN])
{
    return strncpy(model_str, product_model, PRODUCT_MODEL_LEN);
}

char *product_get_key(char key_str[PRODUCT_KEY_LEN])
{
    return strncpy(key_str, product_key, PRODUCT_KEY_LEN);
}

char *product_get_secret(char secret_str[PRODUCT_SECRET_LEN])
{
    return strncpy(secret_str, product_secret, PRODUCT_SECRET_LEN);
}

char *product_get_debug_key(char key_str[PRODUCT_KEY_LEN])
{
    return strncpy(key_str, product_debug_key, PRODUCT_KEY_LEN);
}

char *product_get_debug_secret(char secret_str[PRODUCT_SECRET_LEN])
{
    return strncpy(secret_str, product_debug_secret, PRODUCT_SECRET_LEN);
}

char *product_get_sn(char sn_str[PRODUCT_SN_LEN])
{
    char *p = sn_str;
    int i = 0;

    os_wifi_get_mac_str(sn_str);
    while (*p != '\0' && i < (PRODUCT_SN_LEN - 1)) {
        if (*p == ':') {
            *p = '0';
        }
        p++;
        i++;
    }

    return sn_str;
}
