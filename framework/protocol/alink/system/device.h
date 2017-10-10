/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef DEVICE_H
#define DEVICE_H

#include "os.h"
#include "config.h"

#define TOKEN_LEN       64
#define SERVER_LEN      64
#define UUID_LEN        32
#define ATTRS_LEN       128
#define SERVERADDR_LEN  80

#define CONFIG_MAGIC        "Alink+"
#define CONFIG_VERSION      0x01
#define CONFIG_LEN      ((long)(&((device_config_t  *)0)->crc))

enum {
    DEVICE_OK = 0,
    DEVICE_ERR
};

typedef enum {
    MEMUSED = 0,
    DISCONNCOUNTER,
    DISAPCOUNTER,
    MALFORMEDPACKETS,
    LATESTRTT,
    AVERAGERTT,
    WIFISTRENGTH,
    MAX_STATIS
} STATISTYPE;

typedef struct device_info {
    char model[PRODUCT_MODEL_LEN]; /* <brand>_<category>_<type>_<name> */
    char key[PRODUCT_KEY_LEN];
    char secret[PRODUCT_SECRET_LEN];
#ifdef CONFIG_SDS
    char device_key[DEVICE_KEY_LEN];
    char device_secret[DEVICE_SECRET_LEN];
#endif
    char debug_key[PRODUCT_KEY_LEN];
    char debug_secret[PRODUCT_SECRET_LEN];
    char sn[PRODUCT_SN_LEN];

    char cid[PLATFORM_CID_LEN];
    char mac[PLATFORM_MAC_LEN];   /* xx:xx:xx:xx:xx:xx */

    //TODO: move this to config area
    char alink_version[STR_SHORT_LEN];
    char os_version[STR_SHORT_LEN];
    char firmware_version[PRODUCT_VERSION_LEN + 1];
} device_info_t;

typedef struct device {
    device_info_t *info;
    device_config_t *config;
} device_t;

void device_init(void);
void device_exit(void);
const device_t *get_main_dev(void);
void device_get_config(void *config);
char *devinfo_get_secret(void);
char *devinfo_get_key(void);
char *devinfo_get_version(void);
char *devinfo_get_device_key(void);
char *devinfo_get_device_secret(void);

//TODO: remove these header
void get_wifi_rssi_dbm(char *dev_stats, int length);
void get_disconncounter(char *dev_stats, int lenght);
void get_last_rtt(char *dev_stats, int len);
void get_average_rtt(char *dev_stats, int len);
#endif

