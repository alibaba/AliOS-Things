/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "aos/kv.h"
#include "aos_system.h"
#define MOD_STR "APP_MGR"
static char app_version[32] = { 0 };

pthread_t system_reboot_thread;

void system_reboot(void *argv)
{
    usleep(1000 * 1000);
    esp_restart();
}

int pyamp_app_upgrade(char *url)
{
    char *key = "_amp_pyapp_url";
    int32_t url_len = 0;
    int32_t ret = 0;
    url_len = strlen(url);
    ret = aos_kv_set(key, url, url_len, 1);
    if (ret != 0) {
        printf("%s:aos_kv_set failed\n", __func__);
    }
    ret = aos_task_new_ext(&system_reboot_thread, "system_reboot", system_reboot, NULL, 1024, AOS_DEFAULT_APP_PRI);
    if (ret < 0) {
        printf("pthread_create g_dynreg_dev_thread failed: %d", ret);
        return -1;
    }

    return ret;
}

int save_ssid_and_password(char *ssid, char *password)
{
    char *key_ssid = "_amp_wifi_ssid";
    char *key_passwd = "_amp_wifi_passwd";
    int32_t ssid_len = 0;
    int32_t passwd_len = 0;
    int32_t ret = 0;
    ssid_len = strlen(ssid);
    passwd_len = strlen(password);
    if (ssid_len > 0 && passwd_len > 0) {
        ret = aos_kv_set(key_ssid, ssid, ssid_len, 1);
        if (ret != 0) {
            printf("%s:aos_kv_set failed,key is %s, value is %s, len is %d\n", __func__, key_ssid, ssid, ssid_len);
            return ret;
        }
        ret = aos_kv_set(key_passwd, password, passwd_len, 1);
        if (ret != 0) {
            printf("%s:aos_kv_set failed,key is %s, value is, len is %d\n", __func__, key_passwd, password, passwd_len);
            return ret;
        }
    }
}

int check_channel_enable(void) {
    int ret = 0;
    char *key = "app_upgrade_channel";
    char value[20] = {0};
    int value_len = 20;
    ret = aos_kv_get(key, value, &value_len);
    if (ret != 0)
        return 0;
    if (strcmp(value, "bt") == 0)
        return 2;
    if (strcmp(value, "disable") == 0)
        return 1;
    return 0;
}

