/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _SYS_CONFIG_H_
#define _SYS_CONFIG_H_

#include "os.h"
#if 1
#if defined(__cplusplus) /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif
#endif
#define CONFIG_MAGIC_STR        "Alink+"
#define CONFIG_VERSION          0x01
#define CONFIG_LEN              ((long)(&((device_config_t  *)0)->crc))


typedef struct device_flag {
    union {
        struct {
            uint8_t  fatal: 1;
            uint8_t  error: 1;
            uint8_t  warn: 1;
            uint8_t  debug: 1;
            uint8_t  info: 1;
            uint8_t  trace: 1;
        } lb;
        uint8_t log;
    } ulog;
    union {
        struct {
            uint8_t  memused: 1;
            uint8_t  disconcounter: 1;
            uint8_t  disapcounter: 1;
            uint8_t  malformedpackets: 1;
            uint8_t  latestrtt: 1;
            uint8_t  averagertt: 1;
            uint8_t  wifistrength: 1;
        } sb;
        uint8_t stats;
    } ustats;
    union {
        struct {
            uint8_t  factoryreset: 1;
            uint8_t  otareboot: 1;
        } fb;
        uint8_t flag;
    } uflag;
    uint8_t resv;
} device_flag_t;

#define TOKEN_LEN       64
#define SERVER_LEN      64
#define UUID_LEN        32
#define ATTRS_LEN       128
#define SERVERADDR_LEN  80

#define STR_SHORT_LEN                   (32)
#define STR_LONG_LEN                    (128)
#define PRODUCT_SN_LEN          (64 + 1)
#define PRODUCT_MODEL_LEN       (80 + 1)
#define PRODUCT_KEY_LEN         (20 + 1)
#define PRODUCT_SECRET_LEN      (40 + 1)
#define PRODUCT_UUID_LEN        (32 + 1)
//#define PRODUCT_VERSION_LEN     (32 + 1)
#define PRODUCT_NAME_LEN        (32 + 1)
#define PLATFORM_CID_LEN (64 + 1)
#define PLATFORM_MAC_LEN    (17 + 1)

typedef struct device_config {
    char magic[8];
    uint16_t version;
    device_flag_t df;
    char token[TOKEN_LEN + 4];
    char uuid[UUID_LEN + 4];
    char alinkserver[SERVER_LEN + 4];
    char attrsfilter[ATTRS_LEN + 4];
    char dev_version[STR_SHORT_LEN];//FIXME:
    char ota_version[STR_SHORT_LEN];
    uint8_t heartbeat_interval;
    uint8_t reqtimeout_interval;
    uint8_t st_flag;
    uint8_t padding;
    uint8_t test_mode;
    char test_id[STR_SHORT_LEN];
    uint16_t crc;
} device_config_t;

extern device_config_t main_config;

int config_init(void);
void config_exit(void);
int config_reset(int unregister_flag);
int config_set_test_mode(const char *test_mode, int len);
int config_update(void);
int config_get_log_level(void);
int config_set_log_level(int value);
int config_get_server_info(char server_name[STR_LONG_LEN], int *server_port);
int config_get_heartbeat_interval(void);
int config_set_heartbeat_interval(int value);
int config_get_reqtimeout_interval(void);
int config_set_reqtimeout_interval(int value);
char *config_get_main_uuid(void);
int config_set_main_uuid(char *uuid);

int config_get_unregister_flag(void);
int config_set_unregister_flag(int flag);

int config_set_stats_flag(uint8_t st_flag);
uint8_t config_get_stats_flag(void);
uint8_t config_get_test_mode(void);
char *config_get_attrsfilter(void);
char *config_get_alinkserver(void);
int config_set_alinkserver(const char *server);
int config_set_device_flag(uint32_t df);
uint32_t config_get_device_flag(void);
int config_set_dev_version(char *str);
char *config_get_dev_version(void);
int config_set_ota_version(char *buffer);
char *config_get_ota_version(void);

int device_statisticsdata_save(char *buff, int len);
int device_statisticsdata_load(char *buff, int len);

void config_reset_main_uuid();
extern const char *default_online_server_with_port;
extern const char *default_sandbox_server_with_port;
extern const char *default_daily_server_with_port;
#if 1
#if defined(__cplusplus) /* If this is a C++ compiler, use C linkage */
}
#endif
#endif
#endif
