/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "activation.h"
#include "ulog/ulog.h"
#include "aos/kernel.h"
#include <netdb.h>
#include <sys/socket.h>

#ifdef AOS_COMP_KV
#include "aos/kv.h"
#endif

#ifdef SYSINFO_MCU_ESP32
#include "esp_log.h"
#else
#include <vfsdev/wifi_dev.h>
#endif

#define TAG    "ACTIVATION_REPORT"

#define ACTIVATION_ERR(fmt, ...)    LOGE(TAG, fmt, ##__VA_ARGS__)
#define ACTIVATION_INFO(fmt, ...)   LOGI(TAG, fmt, ##__VA_ARGS__)
#define ACTIVATION_DEBUG(fmt, ...)  LOGD(TAG, fmt, ##__VA_ARGS__)

#ifdef AOS_COMP_KV
static int g_activation_report = 0;
#endif

#define ACTIVATION_REPORT_FORMAT "POST /report HTTP/1.1\r\nHost:os-activation.iot.aliyun.com\r\n\
User-Agent: AliOS-Things\r\nContent-Length:%d\r\nAccept: */*\r\nContent-Type:application/json\r\n\
Connection: Keep-Alive\r\n\r\n%s\r\n"

#define ACTIVATION_REPORT_PAYLOAD_FORMAT "{\"activationStr\":\"%s\"}"

#define ACTIVATION_RESPONSE_RESULT_START "{\"success\":"
#define ACTIVATION_RESPONSE_RESULT_END ","

#define ACTIVATION_RESPONSE_MESSAGE_START ",\"message\":\""
#define ACTIVATION_RESPONSE_MESSAGE_END "\"}"

/* The message length is limited by Aliyun */
#define ACTIVATION_MSG_MAX_LEN 160

#define REPORT_SERVER_ADDR   "os-activation.iot.aliyun.com"
#define REPORT_SERVER_PORT   "80"

#define SYSINFO_KERNEL_VERSION_PREFIX "AOS-R-"
#define SYSINFO_KERNEL_VERSION_POSTFIX_MAX_LEN  8


/* ACTIVATION_MSG_MAX_LEN + payload prefix length(24) */
static        char g_report_data[ACTIVATION_MSG_MAX_LEN + 24];
/* ACTIVATION_MSG_MAX_LEN + payload prefix length(30) + http header(170) */
static        char g_report_http_data[ACTIVATION_MSG_MAX_LEN + 200];
static        char g_report_msg[ACTIVATION_MSG_MAX_LEN + 1];
static        char g_response_data[400];
int activation_get_report_msg(char *report_msg, int len)
{
    uint8_t mac[16] = {0};
    char mac_format[18];
    int fd;
    int ret;

    memset(report_msg, 0, len);

#ifdef SYSINFO_KERNEL_VERSION
    /* kernel version */
    if ((strstr(SYSINFO_KERNEL_VERSION, SYSINFO_KERNEL_VERSION_PREFIX)) != NULL) {
        char version[SYSINFO_KERNEL_VERSION_POSTFIX_MAX_LEN] = {0};
        char *kernel_version = SYSINFO_KERNEL_VERSION;
        if (strlen(kernel_version) <= strlen(SYSINFO_KERNEL_VERSION_PREFIX) + SYSINFO_KERNEL_VERSION_POSTFIX_MAX_LEN - 1) {
            memcpy(version, kernel_version + strlen(SYSINFO_KERNEL_VERSION_PREFIX), strlen(kernel_version) - strlen(SYSINFO_KERNEL_VERSION_PREFIX));
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

#ifdef SYSINFO_PARTNER
    /* partner */
    if ((len >= (strlen(report_msg) + strlen("&P=") + strlen(SYSINFO_PARTNER) + 1))) {
        snprintf(report_msg + strlen(report_msg), len - strlen(report_msg), "&P=%s", SYSINFO_PARTNER);
    } else {
        if (SYSINFO_PARTNER != NULL) {
            ACTIVATION_ERR("%s:%d message is too large to report partner:%s\n", __func__, __LINE__, SYSINFO_PARTNER);
        }
    }
#endif

#ifdef SYSINFO_FRAMEWORK
    /* app_name */
    if (len >= strlen(report_msg) + strlen("&A=") + strlen(SYSINFO_FRAMEWORK) + 1) {
        snprintf(report_msg + strlen(report_msg), len - strlen(report_msg), "&A=%s", SYSINFO_FRAMEWORK);
    } else {
        ACTIVATION_ERR("%s:%d message is too large to report app:%s\n", __func__, __LINE__, SYSINFO_FRAMEWORK);
    }
#endif

#ifdef SYSINFO_BOARD
    /* board name */
    char *board_name = SYSINFO_BOARD;
    if (len >= strlen(report_msg) + strlen("&B=") + strlen(board_name) + 1) {
        snprintf(report_msg + strlen(report_msg), len - strlen(report_msg), "&B=%s", board_name);
    } else {
        ACTIVATION_ERR("%s:%d message is too large to report board:%s\n", __func__, __LINE__, board_name);
    }
#endif

#ifdef SYSINFO_MCU
    /* chip type */
    char *mcu_name = SYSINFO_MCU;
    if (len >= strlen(report_msg) + strlen("&C=") + strlen(mcu_name) + 1) {
        snprintf(report_msg + strlen(report_msg), len - strlen(report_msg), "&C=%s", mcu_name);
    } else {
        ACTIVATION_ERR("%s:%d message is too large to report mcu:%s\n", __func__, __LINE__, mcu_name);
    }
#endif

#ifdef SYSINFO_NETWORK
    /* network type */
    if (len >= strlen(report_msg) + strlen("&N=") + strlen(SYSINFO_NETWORK) + 1) {
        snprintf(report_msg + strlen(report_msg), len - strlen(report_msg), "&N=%s", SYSINFO_NETWORK);
    } else {
        ACTIVATION_ERR("%s:%d message is too large to report net:wifi\n", __func__, __LINE__);
    }
#endif

#ifdef SYSINFO_PROJECT
    /* project name */
    if ((SYSINFO_PROJECT != NULL) && (len >= strlen(report_msg) + strlen("&X=") + strlen(SYSINFO_PROJECT) + 1)) {
        snprintf(report_msg + strlen(report_msg), len - strlen(report_msg), "&X=%s", SYSINFO_PROJECT);
    } else {
        if (SYSINFO_PROJECT != NULL) {
            ACTIVATION_ERR("%s:%d message is too large to report project:%s\n", __func__, __LINE__, SYSINFO_PROJECT);
        }
    }
#endif

#ifdef SYSINFO_APP_NET
    /* app net type */
    if ((SYSINFO_APP_NET != NULL) && (len >= strlen(report_msg) + strlen("&S=") + strlen(SYSINFO_APP_NET) + 1)) {
        snprintf(report_msg + strlen(report_msg), len - strlen(report_msg), "&S=%s", SYSINFO_APP_NET);
    } else {
        if (SYSINFO_APP_NET != NULL) {
            ACTIVATION_ERR("%s:%d message is too large to report app_net:%s\n", __func__, __LINE__, SYSINFO_APP_NET);
        }
    }
#endif

#ifdef SYSINFO_DEVICE_TYPE
    /* type: solo, gateway, subdevice */
    if ((SYSINFO_DEVICE_TYPE != NULL) && (len >= strlen(report_msg) + strlen("&T=") + strlen(SYSINFO_DEVICE_TYPE) + 1)) {
        snprintf(report_msg + strlen(report_msg), len - strlen(report_msg), "&T=%s", SYSINFO_DEVICE_TYPE);
    } else {
        if (SYSINFO_DEVICE_TYPE != NULL) {
            ACTIVATION_ERR("%s:%d message is too large to report type:%s\n", __func__, __LINE__, SYSINFO_DEVICE_TYPE);
        }
    }
#endif

    /* mac address */
#ifdef SYSINFO_MCU_ESP32
    extern esp_err_t esp_efuse_mac_get_custom(uint8_t *mac);
    esp_efuse_mac_get_custom(mac);
#else
    fd = open("/dev/wifi0", O_RDWR);
    if ((fd <= 0) || (0 != ioctl(fd, WIFI_DEV_CMD_GET_MAC, mac))) {
        ACTIVATION_ERR("%s:%d WIFI_DEV_CMD_GET_MAC cmd failed!!\n", __func__, __LINE__);
        memset(mac, 0xff, sizeof(mac));
    }
#endif
    snprintf(mac_format, sizeof(mac_format), "%02x-%02x-%02x-%02x-%02x-%02x",
            mac[0] & 0xff,
            mac[1] & 0xff,
            mac[2] & 0xff,
            mac[3] & 0xff,
            mac[4] & 0xff,
            mac[5] & 0xff);
    if (len >= (strlen(report_msg) + strlen("&M=") + strlen(mac_format) + 1)) {
        snprintf(report_msg + strlen(report_msg), len - strlen(report_msg), "&M=%s", mac_format);
    } else {
        ACTIVATION_ERR("%s:%d message is too large to report mac: %s\n", __func__, __LINE__, mac_format);
    }

    /* cloud type */
#ifdef SYSINFO_CLOUD_TYPE
    if ((SYSINFO_CLOUD_TYPE != NULL) && (len >= (strlen(report_msg) + strlen("&Y=") + strlen(SYSINFO_CLOUD_TYPE) + 1))) {
        snprintf(report_msg + strlen(report_msg), len - strlen(report_msg), "&Y=%s", SYSINFO_CLOUD_TYPE);
    } else {
        if (SYSINFO_CLOUD_TYPE != NULL) {
            ACTIVATION_ERR("%s:%d message is too large to report cloud type:%s\n", __func__, __LINE__, SYSINFO_CLOUD_TYPE);
        }
    }
#endif

    ACTIVATION_INFO("%s:%d report_msg: %s\n", __func__, __LINE__, report_msg);

    close(fd);

    return 0;
}

int activation_parse_data(char *response_data)
{
    char response_result[6];
    char response_msg[18];
    char *result_start;
    char *result_end;
    int len;

    ACTIVATION_DEBUG("response data: %s\n", response_data);
    result_start = strstr(response_data, ACTIVATION_RESPONSE_RESULT_START);
    result_end = strstr(result_start, ACTIVATION_RESPONSE_RESULT_END);
    if ((result_start != NULL)
        && (result_end != NULL)) {
            result_start += strlen(ACTIVATION_RESPONSE_RESULT_START);
            len = result_end - result_start;
            if (len >= sizeof(response_result)) {
                return -1;
            }
            strncpy(response_result, result_start, len);
            response_result[len] = '\0';
            ACTIVATION_DEBUG("response result: %s\n", response_result);
        } else {
            ACTIVATION_ERR("parse reponse result failed\n");
            return -1;
        }

        result_start = strstr(response_data, ACTIVATION_RESPONSE_MESSAGE_START);
        result_end = strstr(result_start, ACTIVATION_RESPONSE_MESSAGE_END);
        if ((result_start != NULL) && (result_end != NULL)) {
            result_start += strlen(ACTIVATION_RESPONSE_MESSAGE_START);
            len = result_end - result_start;
            if (len >= sizeof(response_msg))
                return -1;
            strncpy(response_msg, result_start, len);
            response_msg[len] = '\0';
            ACTIVATION_DEBUG("response_msg: %s\n", response_msg);
        } else {
            ACTIVATION_ERR("parse reponse msg failed\n");
            return -1;
        }

        if ((strcmp(response_result, "true") == 0)
            || (strcmp(response_msg, "success") == 0)) {
            ACTIVATION_DEBUG("activation success\n");
            return 0;
    } else {
        ACTIVATION_ERR("activation failed : %s\n", response_data);
        return -1;
    }
}

int activation_report(void)
{
    int sockfd = -1;
    int ret = -1;
    struct addrinfo hints, *addr_list, *cur;

#ifdef AOS_COMP_KV
    int activation_len = sizeof(g_activation_report);
    if (!aos_kv_get("activation_report",
        &g_activation_report, &activation_len)) {
        return 0;
    }
#endif

    activation_get_report_msg(g_report_msg, sizeof(g_report_msg));

    snprintf(g_report_data, sizeof(g_report_data), ACTIVATION_REPORT_PAYLOAD_FORMAT, g_report_msg);
    ACTIVATION_ERR("%s:%d report_data:%s\n", __func__, __LINE__, g_report_data);

    snprintf(g_report_http_data, sizeof(g_report_http_data), ACTIVATION_REPORT_FORMAT, strlen(g_report_data), g_report_data);

    ACTIVATION_ERR("%s:%d report http data:%s\n", __func__, __LINE__, g_report_http_data);
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(REPORT_SERVER_ADDR, REPORT_SERVER_PORT , &hints, &addr_list) != 0) {
        ACTIVATION_ERR("getaddrinfo failed, errno=%d\n", errno);
        return -1;
    }

    for (cur = addr_list; cur != NULL; cur = cur->ai_next) {
        sockfd = (int) socket(cur->ai_family, cur->ai_socktype,
                               cur->ai_protocol);
        if (sockfd < 0) {
            ACTIVATION_ERR("socket create failed, errno=%d\n", errno);
            continue;
        }

        if (connect(sockfd, cur->ai_addr, (int)cur->ai_addrlen) == 0) {
            ACTIVATION_ERR("socket connect success\n");
            break;
        } else {
            ACTIVATION_ERR("socket connect failed, errno=%d\n", errno);
            close(sockfd);
        }
    }

    if (cur == NULL) {
        ACTIVATION_ERR("No available socket found\n");
        return -1;
    }

    ret = send(sockfd, g_report_http_data, strlen(g_report_http_data), 0);
    if (-1 == ret) {
        ACTIVATION_ERR("socket send failed, errno=%d\n", errno);
        close(sockfd);
        return -1;
    }

    while (1) {
        ret = recv(sockfd, g_response_data, sizeof(g_response_data), 0);
        if (ret == -1) {
            if ((errno != EINTR) && (errno != EAGAIN) && (errno != EWOULDBLOCK)) {
                ACTIVATION_ERR("socket recv result failed errno=%d\n", errno);
                break;
            }
            aos_msleep(100);
        } else if (ret == 0) {
            ACTIVATION_DEBUG("connection closed by peer\n");
            break;
        } else {
            ACTIVATION_DEBUG("recv data len =%d\n", ret);
            break;
        }
    }

    if ((ret != -1) && (ret != 0)) {
        ret = activation_parse_data(g_response_data);
        if (ret == 0) {
#ifdef AOS_COMP_KV
            g_activation_report = 1;
            if (aos_kv_set("activation_report", &g_activation_report, sizeof(g_activation_report), 1)) {
                ACTIVATION_ERR("set report_device_info kv failed\n");
            }
#endif
        } else {
            ACTIVATION_ERR("activation report failed\n");
        }
    }

    close(sockfd);

    return 0;
}
