/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include "aos/kernel.h"
#include "wrappers_defs.h"


#define PLATFORM_WAIT_INFINITE (~0)

#ifdef DYNAMIC_REGISTER
    char _product_key[IOTX_PRODUCT_KEY_LEN + 1]       = "a1ZETBPbycq";
    char _product_secret[IOTX_PRODUCT_SECRET_LEN + 1] = "L68wCVXYUaNg1Ey9";
    char _device_name[IOTX_DEVICE_NAME_LEN + 1]       = "example1";
    char _device_secret[IOTX_DEVICE_SECRET_LEN + 1]   = "";
#else
    #ifdef DEVICE_MODEL_ENABLED
        char _product_key[IOTX_PRODUCT_KEY_LEN + 1]       = "a1RIsMLz2BJ";
        char _product_secret[IOTX_PRODUCT_SECRET_LEN + 1] = "fSAF0hle6xL0oRWd";
        char _device_name[IOTX_DEVICE_NAME_LEN + 1]       = "example1";
        char _device_secret[IOTX_DEVICE_SECRET_LEN + 1]   = "RDXf67itLqZCwdMCRrw0N5FHbv5D7jrE";
    #else
        char _product_key[IOTX_PRODUCT_KEY_LEN + 1]       = "a1MZxOdcBnO";
        char _product_secret[IOTX_PRODUCT_SECRET_LEN + 1] = "h4I4dneEFp7EImTv";
        char _device_name[IOTX_DEVICE_NAME_LEN + 1]       = "test_01";
        char _device_secret[IOTX_DEVICE_SECRET_LEN + 1]   = "t9GmMf2jb3LgWfXBaZD2r3aJrfVWBv56";
    #endif
#endif

int HAL_SetProductKey(char *product_key)
{
    int len = strlen(product_key);

    if (len > IOTX_PRODUCT_KEY_LEN) {
        return -1;
    }
    memset(_product_key, 0x0, IOTX_PRODUCT_KEY_LEN + 1);
    strncpy(_product_key, product_key, len);

    return len;
}


int HAL_SetDeviceName(char *device_name)
{
    int len = strlen(device_name);

    if (len > IOTX_DEVICE_NAME_LEN) {
        return -1;
    }
    memset(_device_name, 0x0, IOTX_DEVICE_NAME_LEN + 1);
    strncpy(_device_name, device_name, len);

    return len;
}

int HAL_SetProductSecret(char *product_secret)
{
    int len = strlen(product_secret);

    if (len > IOTX_PRODUCT_SECRET_LEN) {
        return -1;
    }
    memset(_product_secret, 0x0, IOTX_PRODUCT_SECRET_LEN + 1);
    strncpy(_product_secret, product_secret, len);

    return len;
}

int HAL_SetDeviceSecret(char *device_secret)
{
    int len = strlen(device_secret);

    if (len > IOTX_DEVICE_SECRET_LEN) {
        return -1;
    }
    memset(_device_secret, 0x0, IOTX_DEVICE_SECRET_LEN + 1);
    strncpy(_device_secret, device_secret, len);

    return len;
}

int HAL_GetProductKey(char product_key[IOTX_PRODUCT_KEY_LEN + 1])
{
    int len = strlen(_product_key);
    memset(product_key, 0x0, IOTX_PRODUCT_KEY_LEN + 1);

    strncpy(product_key, _product_key, len);

    return len;
}

int HAL_GetProductSecret(char product_secret[IOTX_PRODUCT_SECRET_LEN + 1])
{
    int len = strlen(_product_secret);
    memset(product_secret, 0x0, IOTX_PRODUCT_SECRET_LEN + 1);

    strncpy(product_secret, _product_secret, len);

    return len;
}

int HAL_GetDeviceName(char device_name[IOTX_DEVICE_NAME_LEN + 1])
{
    int len = strlen(_device_name);
    memset(device_name, 0x0, IOTX_DEVICE_NAME_LEN + 1);

    strncpy(device_name, _device_name, len);

    return strlen(device_name);
}

int HAL_GetDeviceSecret(char device_secret[IOTX_DEVICE_SECRET_LEN + 1])
{
    int len = strlen(_device_secret);
    memset(device_secret, 0x0, IOTX_DEVICE_SECRET_LEN + 1);

    strncpy(device_secret, _device_secret, len);

    return len;
}

int HAL_GetFirmwareVersion(_OU_ char version[IOTX_FIRMWARE_VERSION_LEN])
{
    memset(version, 0x0, IOTX_FIRMWARE_VERSION_LEN);

    strncpy(version, (const char *)SYSINFO_APP_VERSION, IOTX_FIRMWARE_VERSION_LEN - 1);
    version[IOTX_FIRMWARE_VERSION_LEN - 1] = '\0';

    return strlen(version);
}

