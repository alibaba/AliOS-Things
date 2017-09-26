/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include "config.h"
#include "aos/aos.h"
#include "crc.h"
#include "os.h"
#ifdef ASR_SDK
#include "asr.h"
#endif

#define CONFIG_DEBUG 1
#if (DEVICE_DEBUG == 1)
#define config_debug LOGE
#else
#define config_debug
#endif

#define MODULE_NAME "alink config"
device_config_t main_config;

const char *default_online_server_with_port = "online/alink.tcp.aliyun.com:443";
const char *default_sandbox_server_with_port =
    "sandbox/wsf.smart.tbsandbox.com:80";
const char *default_daily_server_with_port =
    "daily/wsf.smart.aliyun-inc.com:9999";
const char *default_server = "alink.tcp.aliyun.com";
const int default_server_port = 443;
const int default_heartbeat_interval = CONFIG_HEARTBEAT_INTERVAL;
const int default_reqtimeout_interval = CONFIG_REQTIMEOUT_INTERVAL;

static void config_dump(void);

static int init;

#ifdef ASR_SDK
static void config_set_asr_server(void)
{
    if (!strcmp(main_config.alinkserver,
                default_online_server_with_port)) {
        asr_init_server(DEFAULT_ONLINE_SERVER,
                        DEFAULT_ONLINE_PORT,
                        DEFAULT_ONLINE_SSL);
    } else if (!strcmp(main_config.alinkserver,
                       default_daily_server_with_port)) {
        asr_init_server(DEFAULT_DAILY_SERVER,
                        DEFAULT_DAILY_PORT,
                        DEFAULT_DAILY_SSL);
    }
}
#endif

int config_init(void)
{
    int ret = 0;

    if (!init) {
        memset(&main_config, 0, sizeof(main_config));
        if (!os_config_read((char *)&main_config, sizeof(main_config))
            && !strncmp(main_config.magic, CONFIG_MAGIC_STR,
                        strlen(CONFIG_MAGIC_STR))
            && main_config.crc == utils_crc16((uint8_t *) &main_config,
                                              CONFIG_LEN)) {
            LOGI(MODULE_NAME, "config init ok");
        } else {
            LOGW(MODULE_NAME, "config init fail, reset...");
            ret = config_reset(0);
        }

#ifdef ASR_SDK
        config_set_asr_server();
#endif
        config_dump();
        init = 0xff;
    }

    return ret;
}

void config_exit(void)
{
    init = 0;
}

static void config_dump(void)
{
    LOGI(MODULE_NAME, "~~~~~dump device config~~~~~");
    LOGI(MODULE_NAME, "magic: %s", main_config.magic);
    LOGI(MODULE_NAME, "version: %d", main_config.version);
    LOGI(MODULE_NAME, "log: %d", main_config.df.ulog.log);
    LOGI(MODULE_NAME, "uuid: %s", main_config.uuid);
    LOGI(MODULE_NAME, "token: %s", main_config.token);
    LOGI(MODULE_NAME, "alinkserver: %s", main_config.alinkserver);
    LOGI(MODULE_NAME, "heartbeat: %d", main_config.heartbeat_interval);
    LOGI(MODULE_NAME, "reqtimeout: %d", main_config.reqtimeout_interval);
    LOGI(MODULE_NAME, "dev_version: %s", main_config.dev_version);
    LOGI(MODULE_NAME, "ota_version: %s", main_config.ota_version);
    LOGI(MODULE_NAME, "crc: %d", main_config.crc);
    LOGI(MODULE_NAME, "~~~~~~~~~~~~~~~~~~~~~~~~~~");
}

int config_update(void)
{
    main_config.crc = utils_crc16((uint8_t *) &main_config, CONFIG_LEN);

    LOGI(MODULE_NAME, "we will update local config");
    return os_config_write((const char *)&main_config, sizeof(main_config));
}

int config_reset(int unregister_flag)
{
    memset(&main_config, 0, sizeof(main_config));

    strcpy(main_config.magic, CONFIG_MAGIC_STR);
    main_config.version = CONFIG_VERSION;
    main_config.df.ulog.log = AOS_LL_INFO | AOS_LL_WARN | AOS_LL_ERROR;
    main_config.df.uflag.fb.factoryreset = !!unregister_flag;
    strcpy(main_config.alinkserver, default_online_server_with_port);
    main_config.heartbeat_interval = default_heartbeat_interval;
    main_config.reqtimeout_interval = default_reqtimeout_interval;
    main_config.crc = utils_crc16((uint8_t *) &main_config, CONFIG_LEN);
    return config_update();
}

int config_get_log_level(void)
{
    if (!init) {
        config_init();
    }
    return main_config.df.ulog.log;
}

int config_set_log_level(int value)
{
    if (!init) {
        config_init();
    }
    main_config.df.ulog.log = value;

    return config_update();
}

static int parser_server_addr(char *addr, char *ip, int *port)
{
    char *p_server = addr, *p_port;

    OS_CHECK_PARAMS(addr && ip && port);

    if ((p_server = strchr(addr, '/')) == NULL) {
        p_server = addr;    /* addr don't prefix with online/, sandbox/ or daily/ */
    } else {
        p_server++;    /* eating '/' */
    }

    if ((p_port = strchr(p_server, ':')) == NULL) {
        return -1;
    }

    memcpy(ip, p_server, p_port - p_server);
    ip[p_port - p_server] = '\0';

    *port = atoi(++p_port);
    return 0;
}

int config_get_server_info(char server_name[STR_LONG_LEN], int *server_port)
{
    OS_CHECK_PARAMS(server_name && server_port);
    if (!init) {
        config_init();
    }

    if (parser_server_addr(main_config.alinkserver,
                           server_name, server_port) != 0) {
        LOGW(MODULE_NAME, "get server info error, use default value!");
        strncpy(server_name, default_server, STR_LONG_LEN);
        *server_port = default_server_port;
    }

    LOGD(MODULE_NAME, "ip %s port %d", server_name, *server_port);

    return 0;
}

int config_get_heartbeat_interval(void)
{
    if (!init) {
        config_init();
    }

    return main_config.heartbeat_interval;
}

int config_set_heartbeat_interval(int value)
{
    OS_CHECK_PARAMS(value > 0);
    if (!init) {
        config_init();
    }

    main_config.heartbeat_interval = value;

    return config_update();
}

int config_get_reqtimeout_interval(void)
{
    if (!init) {
        config_init();
    }
    return main_config.reqtimeout_interval;
}

int config_set_reqtimeout_interval(int value)
{
    OS_CHECK_PARAMS(value > 0);
    if (!init) {
        config_init();
    }

    main_config.reqtimeout_interval = value;

    return config_update();
}

char *config_get_main_uuid(void)
{
    if (!init) {
        config_init();
    }
    return main_config.uuid;
}

void config_reset_main_uuid()
{
    if (!init) {
        config_init();
    }
    memset(main_config.uuid, 0, PRODUCT_UUID_LEN);
}

int config_set_main_uuid(char *uuid)
{
    OS_CHECK_PARAMS(uuid);
    if (!init) {
        config_init();
    }

    if (strcmp(main_config.uuid, uuid)) {
        strncpy(main_config.uuid, uuid, PRODUCT_UUID_LEN);
        return config_update();
    }

    return 0;
}

int config_get_unregister_flag(void)
{
    if (!init) {
        config_init();
    }

    return main_config.df.uflag.fb.factoryreset;
}

int config_set_unregister_flag(int flag)
{
    if (!init) {
        config_init();
    }

    main_config.df.uflag.fb.factoryreset = !!flag;

    return config_update();
}

/* for ota */
char *config_get_ota_version(void)
{
    if (!init) {
        config_init();
    }
    return main_config.ota_version;
}

int config_set_ota_version(char *buffer)
{
    OS_CHECK_PARAMS(buffer);
    if (!init) {
        config_init();
    }

    memset(main_config.ota_version, 0, sizeof(main_config.ota_version));
    strncpy(main_config.ota_version, buffer,
            sizeof(main_config.ota_version) - 1);

    return config_update();
}

/* for ota */
char *config_get_dev_version(void)
{
    if (!init) {
        config_init();
    }
    return main_config.dev_version;
}

int config_set_dev_version(char *str)
{
    OS_CHECK_PARAMS(str);
    if (!init) {
        config_init();
    }

    snprintf(main_config.dev_version, STR_SHORT_LEN, "%s", str);
    return config_update();
}

/* for cota */
uint32_t config_get_device_flag(void)
{
    return *(uint32_t *) & (main_config.df);
}

int config_set_device_flag(uint32_t df)
{
    *(uint32_t *) &(main_config.df) = df;
    return config_update();
}

uint8_t config_get_test_mode(void)
{
    return main_config.test_mode;
}

int config_set_test_mode(const char *test_mode, int len)
{
    OS_CHECK_PARAMS(test_mode && strlen(test_mode));

    if (!init) {
        config_init();
    }

    if (!strncmp("true", test_mode, len)) {
        LOGD(MODULE_NAME, "testMode enabled\n");
        main_config.test_mode = 1;
    } else {
        LOGD(MODULE_NAME, "testMode disabled\n");
        main_config.test_mode = 0;
    }

    return config_update();

}

char *config_get_alinkserver(void)
{
    return main_config.alinkserver;
}

int config_set_alinkserver(const char *server)
{
    OS_CHECK_PARAMS(server && strlen(server));

    if (!init) {
        config_init();
    }

    strncpy(main_config.alinkserver, server,
            sizeof(main_config.alinkserver));

    return config_update();
}

char *config_get_attrsfilter(void)
{
    return main_config.attrsfilter;
}

uint8_t config_get_stats_flag(void)
{
    return main_config.st_flag;
}

int config_set_stats_flag(uint8_t st_flag)
{
    main_config.st_flag = st_flag;
    return config_update();
}

//TODO:
int device_statisticsdata_save(char *buff, int len)
{
    return 0;
}

int device_statisticsdata_load(char *buff, int len)
{
    return 0;
}
