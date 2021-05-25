/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include "aos/kv.h"

#if AOS_COMP_CLI
#include "aos/cli.h"


/*
kv_init has been called in aos_components_init, but can called repeatedly.
#ifdef AOS_COMP_KV
    kv_init();
#endif
*/

static void handle_aos_kv_clear(int argc, char **argv)
{
    int res;
    int idx;
    char *key_to_del[] = {"product_key", "device_name", "device_secret", "wifi_ssid", "wifi_password"};

    for (idx = 0; idx < sizeof(key_to_del) / sizeof(key_to_del[0]); idx++)
    {
        res = aos_kv_del(key_to_del[idx]);
        printf("aos_kv_clear '%s' %s\r\n", key_to_del[idx], res == 0 ? "success." : "fail!");
    }
}

/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(handle_aos_kv_clear, kvclear, kv info clear)
#endif

/* 字符串长度限制 */
#define LIMIT_PRODUCT_KEY       32
#define LIMIT_DEVICE_NAME       64
#define LIMIT_DEVICE_SECRTE     64

#define LIMIT_WIFI_SSID         256
#define LIMIT_WIFI_PASSWORD     256

int32_t kv_devinfo_set(char *pk, char *dn, char *ds)
{
    int32_t ret;
    int len;
    char product_key[LIMIT_PRODUCT_KEY];
    char device_name[LIMIT_DEVICE_NAME];
    char device_secret[LIMIT_DEVICE_SECRTE];

    if ( pk == NULL || strlen(pk) >= LIMIT_PRODUCT_KEY
      || dn == NULL || strlen(dn) >= LIMIT_DEVICE_NAME
      || ds == NULL || strlen(ds) >= LIMIT_DEVICE_SECRTE ) {
        printf("[%s]: input para error!\r\n", __func__);
        return -1;
    }

    len = LIMIT_PRODUCT_KEY;
    ret = aos_kv_get("product_key", product_key, &len);
    if(ret){
        product_key[0] = 0;
    }

    len = LIMIT_DEVICE_NAME;
    ret = aos_kv_get("device_name", device_name, &len);
    if(ret){
        device_name[0] = 0;
    }

    len = LIMIT_DEVICE_SECRTE;
    ret = aos_kv_get("device_secret", device_secret, &len);
    if(ret){
        device_secret[0] = 0;
    }

    if ( strcmp(product_key, pk) == 0
      && strcmp(device_name, dn) == 0
      && strcmp(device_secret, ds) == 0) {
        return -1;
    }

    ret = aos_kv_set("product_key", pk, strlen(pk) + 1, 1);
    if ( ret ){
        printf("[%s]: set 'product_key' fail! ret = %d\r\n", __func__, ret);
        return ret;
    }

    ret = aos_kv_set("device_name", dn, strlen(dn) + 1, 1);
    if ( ret ) {
        printf("[%s]: set 'device_name' fail! ret = %d\r\n", __func__, ret);
        return ret;
    }

    ret = aos_kv_set("device_secret", ds, strlen(ds) + 1, 1);
    if ( ret ) {
        printf("[%s]: set 'device_secret' fail! ret = %d\r\n", __func__, ret);
        return ret;
    }

    return 0;
}

int32_t kv_devinfo_get(char **pk, char **dn, char **ds)
{
    int32_t ret;
    int len;
    static char product_key[LIMIT_PRODUCT_KEY];
    static char device_name[LIMIT_DEVICE_NAME];
    static char device_secret[LIMIT_DEVICE_SECRTE];

    len = LIMIT_PRODUCT_KEY;
    ret = aos_kv_get("product_key", product_key, &len);
    if(ret){
        //printf("get 'product_key' fail! ret = %d\r\n", ret);
        return ret;
    }
    printf("[%s]: product_key '%s'\r\n", __func__, product_key);

    len = LIMIT_DEVICE_NAME;
    ret = aos_kv_get("device_name", device_name, &len);
    if(ret){
        //printf("get 'device_name' fail! ret = %d\r\n", ret);
        return ret;
    }
    printf("[%s]: device_name '%s'\r\n", __func__, device_name);

    len = LIMIT_DEVICE_SECRTE;
    ret = aos_kv_get("device_secret", device_secret, &len);
    if(ret){
        //printf("get 'device_secret' fail! ret = %d\r\n", ret);
        return ret;
    }
    printf("[%s]: device_secret '%s'\r\n", __func__, device_secret);

    *pk = product_key;
    *dn = device_name;
    *ds = device_secret;

    return 0;
}

int32_t kv_wifi_set(char *ssid, char *password)
{
    int32_t ret;
    int len;
    char wifi_ssid[LIMIT_WIFI_SSID];
    char wifi_password[LIMIT_WIFI_PASSWORD];

    if ( ssid == NULL || strlen(ssid) >= LIMIT_WIFI_SSID
      || password == NULL || strlen(password) >= LIMIT_WIFI_PASSWORD ) {
        printf("[%s]: input para error!\r\n", __func__);
        return -1;
    }

    len = LIMIT_WIFI_SSID;
    ret = aos_kv_get("wifi_ssid", wifi_ssid, &len);
    if(ret){
        wifi_ssid[0] = 0;
    }
    len = LIMIT_WIFI_PASSWORD;
    ret = aos_kv_get("wifi_password", wifi_password, &len);
    if(ret){
        wifi_password[0] = 0;
    }

    if ( strcmp(wifi_ssid, ssid) == 0
      && strcmp(wifi_password, password) == 0 ) {
        return -1;
    }

    ret = aos_kv_set("wifi_ssid", ssid, strlen(ssid)+1, 1);
    if ( ret ){
        printf("[%s]: set 'wifi_ssid' fail! ret = %d\r\n", __func__, ret);
        return ret;
    }

    ret = aos_kv_set("wifi_password", password, strlen(password)+1, 1);
    if ( ret ) {
        printf("[%s]: set 'wifi_password' fail! ret = %d\r\n", __func__, ret);
        return ret;
    }

    return 0;
}

int32_t kv_wifi_get(char **ssid, char **password)
{
    int32_t ret;
    int len;
    static char wifi_ssid[LIMIT_WIFI_SSID];
    static char wifi_password[LIMIT_WIFI_PASSWORD];

    len = LIMIT_WIFI_SSID;
    ret = aos_kv_get("wifi_ssid", wifi_ssid, &len);
    if(ret){
        //printf("get 'wifi_ssid' fail! ret = %d\r\n", ret);
        return ret;
    }
    printf("[%s]: wifi_ssid '%s'\r\n", __func__, wifi_ssid);

    len = LIMIT_WIFI_PASSWORD;
    ret = aos_kv_get("wifi_password", wifi_password, &len);
    if(ret){
        //printf("get 'wifi_password' fail! ret = %d\r\n", ret);
        return ret;
    }
    printf("[%s]: wifi_password '%s'\r\n", __func__, wifi_password);

    *ssid = wifi_ssid;
    *password = wifi_password;

    return 0;
}
