/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "alink_protocol.h"
#include "stdlib.h"
#include "json_parser.h"
#include "cota.h"
#include "connectivity_manager.h"
#include "device.h"
#include "aos/aos.h"
#include "service_manager.h"
#include "wsf.h"
#include "config.h"
#include "accs.h"
#include "os.h"

#define AlinkResponseWithDataFormat "{\"result\":{\"msg\":\"%s\",\"code\":%d,\"data\":{%s}}}"
#define AlinkPutStatisDataFormat "{\"uuid\" :\"%s\",\"statastics\":[%s]}"
#define ConfigInfoFormat "\"debugSwitch\":\"%d\",\"alinkServer\":\"%s\",\"attrsFilter\":\"%s\",\"testMode\":\"%d\""

#define STATS_DATA_LEN  256
#define CONFIG_DATA_LEN 256

#define MODULE_NAME_COTA "cota"

static int disconnectcounter = 0;

static void collect_stats(void *work);
static int cota_conn_listener(int type, void *data, int dlen, void *result,
                              int *rlen);
static int cota_service_listener(int type, void *data, int dlen, void *result,
                                 int *rlen);
static int get_dev_stats(STATISTYPE es, char *dev_stats);
static int alink_set_config(char *param);
static int alink_get_config(char *param, char *result);

int cota_init(void)
{
    cm_bind_conn("wsf", cota_conn_listener);
    return sm_attach_service("accs", &cota_service_listener);
}

int cota_exit(void)
{
    //FIXME: add delete interface.
    //stop_cota_work();
    sm_detach_service("accs", &cota_service_listener);
    return cm_release_conn("wsf", &cota_conn_listener);
}

static int cota_conn_listener(int type, void *data, int dlen, void *result,
                              int *rlen)
{
    if (type == CONNECT_EVENT) {
        int st = *((int *)data);

        if (st == CONNECT_STATE_READY) {
            aos_loop_schedule_work(0, collect_stats, NULL, NULL, NULL);
        } else if (st == CONNECT_STATE_CLOSE) {
            aos_loop_schedule_work(0, collect_stats, NULL, NULL, NULL);
            disconnectcounter++;
        }
    }
    return 0;
}

static int cota_service_listener(int type, void *data, int dlen, void *result,
                                 int *rlen)
{
    if (type == SERVICE_DATA) {
        alink_data_t *p = (alink_data_t *) data;
        *rlen = 0;

        if (!strcmp(p->method, "getDebugConfig")) {
            alink_get_config(p->data, (char *)result);
            LOGW(MODULE_NAME_COTA, "result %s", result);
        } else if (!strcmp(p->method, "setDebugConfig")) {
            LOGW(MODULE_NAME_COTA, "==============> set debug config");
            alink_set_config(p->data);
        } else {
            return EVENT_IGNORE;
        }
        if (result != NULL) {
            *rlen = strlen(result);
        }
        return EVENT_CONSUMED;
    }
    return EVENT_IGNORE;
}

static int alink_set_config(char *param)
{
    int ret = SERVICE_RESULT_OK;
    char *pvalue = NULL;
    int value_len = 0;
    int test_reboot = 0;

    pvalue = json_get_value_by_name(param, strlen(param), "rebootDevice",
                                    &value_len, 0);
    if (pvalue != NULL) {
        test_reboot = 1;
    }
    pvalue = json_get_value_by_name(param, strlen(param), "testMode", &value_len,
                                    0);
    if (pvalue != NULL) {
        ret = config_set_test_mode(pvalue, value_len);
    }
    pvalue = json_get_value_by_name(param, strlen(param), "debugSwitch", &value_len,
                                    0);
    if (pvalue != NULL) {
        ret = config_set_device_flag(atoi(pvalue));
    }

    char *alinkserver = config_get_alinkserver();
    pvalue = json_get_value_by_name(param, strlen(param), "alinkServer", &value_len,
                                    0);
    if (pvalue != NULL) {
        LOGW(MODULE_NAME_COTA, "get alink server ----->  %s\n", pvalue);
        value_len = (value_len < SERVER_LEN) ? value_len : SERVER_LEN;
        if (strstr(pvalue, alinkserver) == NULL) {
            LOGW(MODULE_NAME_COTA, "reset uuid --------\n");
            config_reset_main_uuid();
        }
        memset(alinkserver, 0x00, SERVER_LEN);
        strncpy(alinkserver, pvalue, value_len);
        aos_loop_schedule_work(3000, (void (*)(void *))os_sys_reboot, NULL, NULL, NULL);
    }

    char *attrsfilter = config_get_attrsfilter();
    pvalue = json_get_value_by_name(param, strlen(param), "attrsFilter", &value_len,
                                    0);
    if (pvalue != NULL) {
        value_len = (value_len < ATTRS_LEN) ? value_len : ATTRS_LEN;
        memset(attrsfilter, 0x00, ATTRS_LEN);
        strncpy(attrsfilter, pvalue, value_len);
    } else if (attrsfilter[0] != 0x00) {
        memset(attrsfilter, 0x00, ATTRS_LEN);
    }

    ret = config_update();
    if (0 != ret) {
        LOGW(MODULE_NAME_COTA, "Write config failed");
        ret = SERVICE_RESULT_ERR;
    }

    if (test_reboot) {
        LOGW(MODULE_NAME_COTA, "RebootDevice, device will reboot 3 seconds later!");
        aos_loop_schedule_work(3000, (void (*)(void *))os_sys_reboot, NULL, NULL, NULL);
    }

    return ret;
}

static int alink_get_config(char *param, char *result)
{
    char *data = NULL;
    int ret = SERVICE_RESULT_ERR;

    data = (char *)os_malloc(CONFIG_DATA_LEN);
    OS_CHECK_MALLOC(data);
    memset(data, 0x00, CONFIG_DATA_LEN);

    snprintf(data, CONFIG_DATA_LEN, ConfigInfoFormat, config_get_device_flag(),
             config_get_alinkserver(), config_get_attrsfilter(), config_get_test_mode());
    if (result) {
        snprintf(result, strlen(AlinkResponseWithDataFormat) + CONFIG_DATA_LEN,
                 AlinkResponseWithDataFormat, "success", 1000, data);
    } else {
        os_free(data);
        return ret;
    }
    LOGW(MODULE_NAME_COTA, "config info %s", result);
    os_free(data);
    ret = SERVICE_RESULT_ERR;

    return ret;
}

const char *statsStr[] = {
    "Memused",
    "DisconnCounter",
    "DisapCounter",
    "MalformedPackets",
    "LstRTT",
    "AvgRTT",
    "Rssi",
};

void get_disconncounter(char *dev_stats, int lenght)
{
    sprintf(dev_stats, "%d", disconnectcounter);
}

static int collect_dev_stats(char *devstats)
{
    int len = 0;
    char stats[STR_SHORT_LEN] = { 0 };
    STATISTYPE es = MEMUSED;
    uint32_t df = config_get_device_flag();

    sprintf(devstats, "{\"uuid\":\"%s\", \"statastics\":{", config_get_main_uuid());
    len = strlen(devstats);
    do {
        memset(stats, 0x00, sizeof(stats));
        if ((*(device_flag_t *) & df).ustats.stats & (0x1 << es)
            && get_dev_stats(es, stats) == 0) {
            if ((len + strlen(statsStr[es]) + strlen(stats) + 16) > STATS_DATA_LEN) {
                break;
            }
            snprintf(devstats + len, STATS_DATA_LEN - len, "\"%s\":\"%s\",", statsStr[es],
                     stats);
            len = strlen(devstats);
        }
        es++;
    } while (es < MAX_STATIS);

    snprintf(devstats + len, STATS_DATA_LEN - len, "\"time\":\"%d\"}",
             os_get_time_ms() / 1000);
    sprintf(devstats + strlen(devstats), "}");

    return SERVICE_RESULT_OK;
}

static void collect_stats(void *work)
{
    char method[] = "debug.postDebugInfo";
    char *devstats;

    devstats = os_malloc(STATS_DATA_LEN);
    OS_CHECK_MALLOC(devstats);
    memset(devstats, 0x00, STATS_DATA_LEN);
    alink_data_t data = { method, devstats };

    if (config_get_stats_flag() == 1 && cloud_is_connected()) {
        device_statisticsdata_load(devstats, STATS_DATA_LEN);
        config_set_stats_flag(0);
        //sm_get_service("accs")->put((void *)&data, sizeof(data));
    }

    collect_dev_stats(devstats);

    if (cloud_is_connected()) {
        config_set_stats_flag(0);
        sm_get_service("accs")->put((void *)&data, sizeof(data));
    } else {
        LOGI(MODULE_NAME_COTA, "save stats data to flash");
        device_statisticsdata_save(devstats, strlen(devstats));
        config_set_stats_flag(1);
    }
    os_free(devstats);

    return;
}

static int replace_attr_withspace(char *p_cName, int iNameLen, char *p_cValue,
                                  int iValueLen, int iValueType, void *p_Result)
{
    int ret = SERVICE_RESULT_OK;
    char *p_cAttr = NULL, cLastChar;

    cLastChar = p_cName[iNameLen];
    p_cName[iNameLen] = '\0';
    p_cAttr = strstr((char *)p_Result, p_cName);
    if (p_cAttr != NULL && (p_cAttr[iNameLen] == ',' ||
                            p_cAttr[iNameLen] == '\0')) {
        memset(p_cName - 1, ' ',
               p_cValue + iValueLen - p_cName + ((iValueType == JSTRING) ? 2 : 1));
        p_cAttr = p_cValue + iValueLen;
        while (*p_cAttr == ' ') {
            p_cAttr++;
        }
        if (*p_cAttr == ',') {
            *p_cAttr = ' ';
        }
    } else {
        p_cName[iNameLen] = cLastChar;
    }
    return ret;
}

void alink_attrfilter(char *cJsonStr)
{
    char *attrsfilter = config_get_attrsfilter();

    if (attrsfilter[0] == '\0') {
        LOGW(MODULE_NAME_COTA, "No attrs to filter");
        return;
    }
    json_parse_name_value(cJsonStr, strlen(cJsonStr), replace_attr_withspace,
                          attrsfilter);
}

//TODO
void get_wifi_rssi(char *stats, int len)
{

}

void get_memory_status(char *stats, int len)
{
}

static int get_dev_stats(STATISTYPE es, char *dev_stats)
{
    switch (es) {
        case MEMUSED:
            get_memory_status(dev_stats, STR_SHORT_LEN);
            break;
        case DISCONNCOUNTER:
            get_disconncounter(dev_stats, STR_SHORT_LEN);
            break;
        case DISAPCOUNTER:
            strcpy(dev_stats, "0");
            break;
        case MALFORMEDPACKETS:
            strcpy(dev_stats, "0");
            break;
        case LATESTRTT:
            get_last_rtt(dev_stats, STR_SHORT_LEN);
            break;
        case AVERAGERTT:
            get_average_rtt(dev_stats, STR_SHORT_LEN);
            break;
        case WIFISTRENGTH:
            get_wifi_rssi(dev_stats, STR_SHORT_LEN);
            break;
        default:
            dev_stats[0] = '\0';
            break;
    }
    return 0;
}
