/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include "activation.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "lwip/dns.h"
#include "lwip/err.h"
#include "lwip/ip_addr.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#include "py/mphal.h"

#ifdef AOS_COMP_KV
#include "aos/kv.h"
#endif

#define TAG "ACTIVATION_REPORT"

/* ACTIVATION_MSG_MAX_LEN + payload prefix length(24) */
static char g_report_data[ACTIVATION_MSG_MAX_LEN + 24];

/* ACTIVATION_MSG_MAX_LEN + payload prefix length(30) + http header(170) */
char g_report_http_data[ACTIVATION_MSG_MAX_LEN + 200];
char g_response_data[400];

static char g_report_msg[ACTIVATION_MSG_MAX_LEN + 1];

extern uint8_t message_count;

extern void tcp_activation_client_connect(const ip_addr_t *dest_ipaddr, u16_t dest_port);

int32_t activation_get_report_msg(char *report_msg, int32_t len)
{
    uint8_t mac[6];
    char mac_format[18] = { 0 };
    int32_t ret;

    memset(report_msg, 0, len);

#ifdef ACTIVATION_AOS_VERSION
    /* kernel version */
    if ((strstr(ACTIVATION_AOS_VERSION, ACTIVATION_AOS_VERSION_PREFIX)) != NULL) {
        char version[ACTIVATION_AOS_VERSION_POSTFIX_MAX_LEN] = { 0 };
        char *kernel_version = ACTIVATION_AOS_VERSION;
        if (strlen(kernel_version) <=
            strlen(ACTIVATION_AOS_VERSION_PREFIX) + ACTIVATION_AOS_VERSION_POSTFIX_MAX_LEN - 1) {
            memcpy(version, kernel_version + strlen(ACTIVATION_AOS_VERSION_PREFIX),
                   strlen(kernel_version) - strlen(ACTIVATION_AOS_VERSION_PREFIX));
            snprintf(report_msg, len, "V=%s", version);
        } else {
            ACTIVATION_ERR("%s:%d kernel version error\n", __func__, __LINE__);
        }
    }
    snprintf(report_msg + strlen(report_msg), len - strlen(report_msg), "&O=%s", "Anolis Things");
#else
    snprintf(report_msg, len, "V=%s", "invalid");
    snprintf(report_msg + strlen(report_msg), len - strlen(report_msg), "&O=%s", "rtos");
#endif

#ifdef ACTIVATION_PARTNER
    /* partner */
    if ((len >= (strlen(report_msg) + strlen("&P=") + strlen(ACTIVATION_PARTNER) + 1))) {
        snprintf(report_msg + strlen(report_msg), len - strlen(report_msg), "&P=%s", ACTIVATION_PARTNER);
    } else {
        if (ACTIVATION_PARTNER != NULL) {
            ACTIVATION_ERR("%s:%d message is too large to report partner:%s\n", __func__, __LINE__, ACTIVATION_PARTNER);
        }
    }
#endif

#ifdef ACTIVATION_FRAMEWORK
    /* app_name */
    if (len >= strlen(report_msg) + strlen("&A=") + strlen(ACTIVATION_FRAMEWORK) + 1) {
        snprintf(report_msg + strlen(report_msg), len - strlen(report_msg), "&A=%s", ACTIVATION_FRAMEWORK);
    } else {
        ACTIVATION_ERR("%s:%d message is too large to report app:%s\n", __func__, __LINE__, ACTIVATION_FRAMEWORK);
    }
#endif

#ifdef ACTIVATION_BOARD
    /* board name */
    char *board_name = ACTIVATION_BOARD;
    if (len >= strlen(report_msg) + strlen("&B=") + strlen(board_name) + 1) {
        snprintf(report_msg + strlen(report_msg), len - strlen(report_msg), "&B=%s", board_name);
    } else {
        ACTIVATION_ERR("%s:%d message is too large to report board:%s\n", __func__, __LINE__, board_name);
    }
#endif

#ifdef ACTIVATION_MCU
    /* chip type */
    char *mcu_name = ACTIVATION_MCU;
    if (len >= strlen(report_msg) + strlen("&C=") + strlen(mcu_name) + 1) {
        snprintf(report_msg + strlen(report_msg), len - strlen(report_msg), "&C=%s", mcu_name);
    } else {
        ACTIVATION_ERR("%s:%d message is too large to report mcu:%s\n", __func__, __LINE__, mcu_name);
    }
#endif

#ifdef ACTIVATION_NETWORK
    /* network type */
    if (len >= strlen(report_msg) + strlen("&N=") + strlen(ACTIVATION_NETWORK) + 1) {
        snprintf(report_msg + strlen(report_msg), len - strlen(report_msg), "&N=%s", ACTIVATION_NETWORK);
    } else {
        ACTIVATION_ERR("%s:%d message is too large to report net:wifi\n", __func__, __LINE__);
    }
#endif

#ifdef ACTIVATION_PROJECT
    /* project name */
    if ((ACTIVATION_PROJECT != NULL) && (len >= strlen(report_msg) + strlen("&X=") + strlen(ACTIVATION_PROJECT) + 1)) {
        snprintf(report_msg + strlen(report_msg), len - strlen(report_msg), "&X=%s", ACTIVATION_PROJECT);
    } else {
        if (ACTIVATION_PROJECT != NULL) {
            ACTIVATION_ERR("%s:%d message is too large to report project:%s\n", __func__, __LINE__, ACTIVATION_PROJECT);
        }
    }
#endif

#ifdef ACTIVATION_APP_NET
    /* app net type */
    if ((ACTIVATION_APP_NET != NULL) && (len >= strlen(report_msg) + strlen("&S=") + strlen(ACTIVATION_APP_NET) + 1)) {
        snprintf(report_msg + strlen(report_msg), len - strlen(report_msg), "&S=%s", ACTIVATION_APP_NET);
    } else {
        if (ACTIVATION_APP_NET != NULL) {
            ACTIVATION_ERR("%s:%d message is too large to report app_net:%s\n", __func__, __LINE__, ACTIVATION_APP_NET);
        }
    }
#endif

#ifdef ACTIVATION_DEVICE_TYPE
    /* type: solo, gateway, subdevice */
    if ((ACTIVATION_DEVICE_TYPE != NULL) &&
        (len >= strlen(report_msg) + strlen("&T=") + strlen(ACTIVATION_DEVICE_TYPE) + 1)) {
        snprintf(report_msg + strlen(report_msg), len - strlen(report_msg), "&T=%s", ACTIVATION_DEVICE_TYPE);
    } else {
        if (ACTIVATION_DEVICE_TYPE != NULL) {
            ACTIVATION_ERR("%s:%d message is too large to report type:%s\n", __func__, __LINE__,
                           ACTIVATION_DEVICE_TYPE);
        }
    }
#endif

    /* mac address */
    mp_hal_get_mac(MP_HAL_MAC_ETH0, mac);
    snprintf(mac_format, sizeof(mac_format), "%02x-%02x-%02x-%02x-%02x-%02x", mac[0] & 0xff, mac[1] & 0xff,
             mac[2] & 0xff, mac[3] & 0xff, mac[4] & 0xff, mac[5] & 0xff);
    if (len >= (strlen(report_msg) + strlen("&M=") + strlen(mac_format) + 1)) {
        snprintf(report_msg + strlen(report_msg), len - strlen(report_msg), "&M=%s", mac_format);
    } else {
        ACTIVATION_ERR("%s:%d message is too large to report mac: %s\n", __func__, __LINE__, mac_format);
    }

    /* cloud type */
#ifdef ACTIVATION_CLOUD_TYPE
    if ((ACTIVATION_CLOUD_TYPE != NULL) &&
        (len >= (strlen(report_msg) + strlen("&Y=") + strlen(ACTIVATION_CLOUD_TYPE) + 1))) {
        snprintf(report_msg + strlen(report_msg), len - strlen(report_msg), "&Y=%s", ACTIVATION_CLOUD_TYPE);
    } else {
        if (ACTIVATION_CLOUD_TYPE != NULL) {
            ACTIVATION_ERR("%s:%d message is too large to report cloud type:%s\n", __func__, __LINE__,
                           ACTIVATION_CLOUD_TYPE);
        }
    }
#endif

    ACTIVATION_INFO("%s:%d report_msg: %s\n", __func__, __LINE__, report_msg);
    return ERR_OK;
}

int32_t activation_parse_data(char *response_data)
{
    char response_result[6];
    char response_msg[18];
    char *result_start;
    char *result_end;
    int32_t len;

    ACTIVATION_ERR("response data: %s\n", response_data);
    result_start = strstr(response_data, ACTIVATION_RESPONSE_RESULT_START);
    result_end = strstr(result_start, ACTIVATION_RESPONSE_RESULT_END);
    if ((result_start != NULL) && (result_end != NULL)) {
        result_start += strlen(ACTIVATION_RESPONSE_RESULT_START);
        len = result_end - result_start;
        if (len >= sizeof(response_result)) {
            return ERR_BUF;
        }
        strncpy(response_result, result_start, len);
        response_result[len] = '\0';
        ACTIVATION_DEBUG("response result: %s\n", response_result);
    } else {
        ACTIVATION_ERR("parse reponse result failed\n");
        return ERR_ARG;
    }

    result_start = strstr(response_data, ACTIVATION_RESPONSE_MESSAGE_START);
    result_end = strstr(result_start, ACTIVATION_RESPONSE_MESSAGE_END);
    if ((result_start != NULL) && (result_end != NULL)) {
        result_start += strlen(ACTIVATION_RESPONSE_MESSAGE_START);
        len = result_end - result_start;
        if (len >= sizeof(response_msg))
            return ERR_BUF;
        strncpy(response_msg, result_start, len);
        response_msg[len] = '\0';
        ACTIVATION_DEBUG("response_msg: %s\n", response_msg);
    } else {
        ACTIVATION_ERR("parse reponse msg failed\n");
        return ERR_BUF;
    }

    if ((strcmp(response_result, "true") == 0) || (strcmp(response_msg, "success") == 0)) {
        ACTIVATION_DEBUG("activation success\n");
        return ERR_OK;
    } else {
        ACTIVATION_ERR("activation failed : %s\n", response_data);
        return ERR_ARG;
    }
}

// Callback for incoming DNS requests.
static void lwip_getaddrinfo_cb(const char *name, const ip_addr_t *ipaddr, void *callback_arg)
{
    LWIP_UNUSED_ARG(name);

    ip_addr_t *ip_addr_dest = (ip_addr_t *)callback_arg;
    ip_addr_dest->addr = ipaddr->addr;
}

int activation_report(void)
{
#ifdef AOS_COMP_KV
    int32_t activation_len = sizeof(g_activation_report);
    if (!aos_kv_get("activation_report", &g_activation_report, &activation_len)) {
        return 0;
    }
#endif

    activation_get_report_msg(g_report_msg, sizeof(g_report_msg));

    snprintf(g_report_data, sizeof(g_report_data), ACTIVATION_REPORT_PAYLOAD_FORMAT, g_report_msg);
    ACTIVATION_ERR("%s:%d report_data:%s\n", __func__, __LINE__, g_report_data);

    snprintf(g_report_http_data, sizeof(g_report_http_data), ACTIVATION_REPORT_FORMAT, strlen(g_report_data),
             g_report_data);

    ACTIVATION_DEBUG("%s:%d report http data:%s\n", __func__, __LINE__, g_report_http_data);

    /* 1. get host ip address*/
    ip_addr_t ip_addr_dest = { 0 };
    err_t ret = dns_gethostbyname(REPORT_SERVER_ADDR, &ip_addr_dest, lwip_getaddrinfo_cb, &ip_addr_dest);
    if (ret == ERR_ARG) {
        ACTIVATION_ERR("getaddrinfo failed, ret=%d\n", ret);
        return ret;
    } else {
        for (int loop = 0; loop < MAX_WAIT_MS; loop += DELAY_PERIOD_MS) {
            mp_hal_delay_ms(DELAY_PERIOD_MS);
            if (ip_addr_dest.addr != 0) {
                goto get_ip;
            }
        }
        ACTIVATION_ERR("getaddrinfo failed within %dms\n", MAX_WAIT_MS);
        return ERR_TIMEOUT;
    }

get_ip:
    // 2. create and connection to server
    tcp_activation_client_connect(&ip_addr_dest, REPORT_SERVER_PORT);

    // 4. recv from server
    for (int loop = 0; loop < MAX_WAIT_MS; loop += DELAY_PERIOD_MS) {
        if (message_count == 0) {
            mp_hal_delay_ms(DELAY_PERIOD_MS);
        } else {
            message_count = 0;
            goto recv_from_serv;
        }
    }
    ACTIVATION_ERR("failed to receive from server within %dms\n", MAX_WAIT_MS);
    return ERR_TIMEOUT;

recv_from_serv:
    if (1) {
        ret = activation_parse_data(g_response_data);
        if (ret == ERR_OK) {
#ifdef AOS_COMP_KV
            g_activation_report = 1;
            if (aos_kv_set("activation_report", &g_activation_report, sizeof(g_activation_report), 1)) {
                ACTIVATION_ERR("set report_device_info kv failed\n");
            }
#endif
            ACTIVATION_ERR("activation report success\n");
        } else {
            ACTIVATION_ERR("activation report failed\n");
            return ERR_VAL;
        }
    }

    return ERR_OK;
}
