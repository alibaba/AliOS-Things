/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "network/network.h"
#include "activation.h"
#include "linkkit/infra/infra_report.h"
#include "linkkit/wrappers/wrappers_defs.h"
#include "linkkit/wrappers/wrappers_os.h"
#include "ulog/ulog.h"
#include "aos/kernel.h"
#include "hal/wifi.h"

#ifdef AOS_COMP_KV
#include "aos/kv.h"
#endif

#define TAG    "ACTIVATION_REPORT"

#define ACTIVATION_ERR(fmt, ...)    LOGE(TAG, fmt, ##__VA_ARGS__)
#define ACTIVATION_DEBUG(fmt, ...)  LOGD(TAG, fmt, ##__VA_ARGS__)

#ifdef AOS_COMP_KV
static int g_activation_report = 0;
#endif

#define ACTIVATION_REPORT_MQTT_PAYLOAD_FORMAT "{\"id\": 0, \"version\": \"1.0\", \"params\": [{\"attrKey\": \"SYS_ALIOS_ACTIVATION\", \"attrValue\": \"%s\", \"domain\":\"SYSTEM\"}], \"method\": \"thing.deviceinfo.update\"}"

#ifdef HTTP_ACTIVATION_REPORT
#define ACTIVATION_REPORT_FORMAT "POST /report HTTP/1.1\r\nHost:os-activation.iot.aliyun.com\r\n\
User-Agent: AliOS-Things\r\nContent-Length:%d\r\nAccept: */*\r\nContent-Type:application/json\r\n\
Connection: Keep-Alive\r\n\r\n%s\r\n"

#define ACTIVATION_RESPONSE_RESULT_START "{\"success\":"
#define ACTIVATION_RESPONSE_RESULT_END ","

#define ACTIVATION_RESPONSE_MESSAGE_START ",\"message\":\""
#define ACTIVATION_RESPONSE_MESSAGE_END "\"}"
#endif /* HTTP_ACTIVATION_REPORT */

#define ACTIVATION_REPORT_PAYLOAD_FORMAT "{\r\n\"activationStr\":\"%s\"\r\n}\r\n"

/* The message length is limited by Aliyun */
#define ACTIVATION_MSG_MAX_LEN 160

#define REPORT_SERVER_ADDR   "os-activation.iot.aliyun.com"
#define REPORT_SERVER_PORT   "80"

#define SYSINFO_KERNEL_VERSION_PREFIX "AOS-R-"
#define SYSINFO_KERNEL_VERSION_POSTFIX_MAX_LEN  8

#ifdef HTTP_ACTIVATION_REPORT
static        char g_report_http_data[ACTIVATION_MSG_MAX_LEN + 170];
static        char g_response_data[400];
#endif /* HTTP_ACTIVATION_REPORT */

int activation_get_report_msg(char* report_msg, int len)
{
    uint8_t mac[16];
    char mac_format[18];
    hal_wifi_module_t* module = hal_wifi_get_default_module();
    const char *partner = module->base.partner;
    const char *project = module->base.project;
    const char *app_net = module->base.app_net;
    const char *cloud   = module->base.cloud;
    const char *os      = module->base.os;
    const char *type    = module->base.type;

    memset(report_msg, 0, len);
#ifdef SYSINFO_KERNEL_VERSION
    if((strstr(SYSINFO_KERNEL_VERSION, SYSINFO_KERNEL_VERSION_PREFIX)) != NULL) {
        char version[SYSINFO_KERNEL_VERSION_POSTFIX_MAX_LEN];
        memcpy(version, SYSINFO_KERNEL_VERSION+strlen(SYSINFO_KERNEL_VERSION_PREFIX), SYSINFO_KERNEL_VERSION_POSTFIX_MAX_LEN);
        snprintf(report_msg, len, "V=%s", version);
    }
#endif
    if((partner != NULL) && ((len - strlen(report_msg) - strlen("&P=") - strlen(partner) - 1) >= 0)) {
        snprintf(report_msg + strlen(report_msg), len - strlen(report_msg), "&P=%s", partner);
    }
    else {
        if(partner != NULL) {
            ACTIVATION_ERR("%s:%d message is too large to report partner:%s\n", __func__, __LINE__, partner);
        }
    }
#ifdef AOS_BUILD_APP
    if((len - strlen(report_msg) - strlen("&A=") - strlen(AOS_BUILD_APP) - 1) >= 0) {
        snprintf(report_msg + strlen(report_msg), len - strlen(report_msg), "&A=%s", AOS_BUILD_APP);
    }
    else {
        ACTIVATION_ERR("%s:%d message is too large to report app:%s\n", __func__, __LINE__, AOS_BUILD_APP);
    }
#endif
#if defined(SYSINFO_BOARD) || defined(AOS_BUILD_BOARD)
#ifdef SYSINFO_BOARD
    char* board_name = SYSINFO_BOARD;
#else
    char* board_name = AOS_BUILD_BOARD;
#endif
    if((len - strlen(report_msg) - strlen("&B=") - strlen(board_name) - 1) >= 0) {
        snprintf(report_msg + strlen(report_msg), len - strlen(report_msg), "&B=%s", board_name);
    }
    else {
        ACTIVATION_ERR("%s:%d message is too large to report board:%s\n", __func__, __LINE__, board_name);
    }
#endif
#if defined(SYSINFO_MCU) || defined(MCU_FAMILY)
#ifdef SYSINFO_MCU
    char * mcu_name = SYSINFO_MCU;
#else
    char * mcu_name = MCU_FAMILY;
#endif
    if((len - strlen(report_msg) - strlen("&C=") - strlen(mcu_name) - 1) >= 0) {
        snprintf(report_msg + strlen(report_msg), len - strlen(report_msg), "&C=%s", mcu_name);
    }
    else {
        ACTIVATION_ERR("%s:%d message is too large to report mcu:%s\n", __func__, __LINE__, mcu_name);
    }
#endif
#ifdef AOS_NET_WITH_WIFI
    if((len - strlen(report_msg) - strlen("&N=") - strlen("Wi-Fi") - 1) >= 0) {
        snprintf(report_msg + strlen(report_msg), len - strlen(report_msg), "&N=%s", "Wi-Fi");
    }
    else {
        ACTIVATION_ERR("%s:%d message is too large to report net:wifi\n", __func__, __LINE__);
    }
#endif
    if((project != NULL) && ((len - strlen(report_msg) - strlen("&X=") - strlen(project) - 1) >= 0)) {
        snprintf(report_msg + strlen(report_msg), len - strlen(report_msg), "&X=%s", project);
    }
    else {
        if(project != NULL) {
            ACTIVATION_ERR("%s:%d message is too large to report project:%s\n", __func__, __LINE__, project);
        }
    }

    if((app_net != NULL) && ((len - strlen(report_msg) - strlen("&S=") - strlen(app_net) - 1) >= 0)) {
        snprintf(report_msg + strlen(report_msg), len - strlen(report_msg), "&S=%s", app_net);
    }
    else {
        if(app_net != NULL) {
            ACTIVATION_ERR("%s:%d message is too large to report app_net:%s\n", __func__, __LINE__, app_net);
        }
    }

    if((os != NULL) && ((len - strlen(report_msg) - strlen("&O=") - strlen(os) - 1) >= 0)) {
        snprintf(report_msg + strlen(report_msg), len - strlen(report_msg), "&O=%s", os);
    }
    else {
        if(os != NULL) {
            ACTIVATION_ERR("%s:%d message is too large to report os:%s\n", __func__, __LINE__, app_net);
        }
    }

    if((type != NULL) && ((len - strlen(report_msg) - strlen("&T=") - strlen(type) - 1) >= 0)) {
        snprintf(report_msg + strlen(report_msg), len - strlen(report_msg), "&T=%s", type);
    }
    else {
        if(type != NULL) {
            ACTIVATION_ERR("%s:%d message is too large to report type:%s\n", __func__, __LINE__, app_net);
        }
    }

    if(0 == hal_wifi_get_mac_addr(NULL, mac)) {
        snprintf(mac_format, sizeof(mac_format), "%02x-%02x-%02x-%02x-%02x-%02x",
              mac[0] & 0xff,
              mac[1] & 0xff,
              mac[2] & 0xff,
              mac[3] & 0xff,
              mac[4] & 0xff,
              mac[5] & 0xff);
        if((len - strlen(report_msg) - strlen("&M=") - strlen(mac_format) - 1) >= 0) {
            snprintf(report_msg + strlen(report_msg), len - strlen(report_msg), "&M=%s", mac_format);
        }
        else {
            ACTIVATION_ERR("%s:%d message is too large to report mac: %s\n", __func__, __LINE__, mac_format);
        }
    }

    if((cloud != NULL) && ((len - strlen(report_msg) - strlen("&Y=") - strlen(cloud) - 1) >= 0)) {
        snprintf(report_msg + strlen(report_msg), len - strlen(report_msg), "&Y=%s", cloud);
    }
    else {
        if(cloud != NULL) {
            ACTIVATION_ERR("%s:%d message is too large to report cloud type:%s\n", __func__, __LINE__, cloud);
        }
    }

    ACTIVATION_DEBUG("%s:%d report_msg: %s\n", __func__, __LINE__, report_msg);

    return 0;
}

int iotx_report_ext_msg(info_report_func_pt info_report_func, void * handle)
{
    int ret = 0;
    char topic_name[IOTX_URI_MAX_LEN + 1] = {0};
    char _product_key[IOTX_PRODUCT_KEY_LEN + 1] = {0};
    char _device_name[IOTX_DEVICE_NAME_LEN + 1] = {0};
    /* ACTIVATION_MSG_MAX_LEN + data prefix/postfix length(150) */
    char report_data[ACTIVATION_MSG_MAX_LEN + 150];
    char report_msg[ACTIVATION_MSG_MAX_LEN + 1];

    if (info_report_func == NULL) {
        ACTIVATION_ERR("report func not register!");
        return -1;
    }
    HAL_GetProductKey(_product_key);
    HAL_GetDeviceName(_device_name);

    ACTIVATION_DEBUG("devinfo report");

    /* devinfo update topic name */
    ret = snprintf(topic_name,
                       IOTX_URI_MAX_LEN,
                       "/sys/%s/%s/thing/deviceinfo/update",
                       _product_key,
                       _device_name);
    if (ret <= 0) {
        ACTIVATION_ERR("topic generate err");
        return FAIL_RETURN;
    }
    ACTIVATION_DEBUG("devinfo report topic: %s", topic_name);

    /* devinfo update message */
    ret = activation_get_report_msg(report_msg, sizeof(report_msg));
    if (ret < 0) {
        ACTIVATION_ERR("report data generate err");
        return FAIL_RETURN;
    }
    ACTIVATION_DEBUG("devinfo report msg: %s", report_msg);

    snprintf(report_data, sizeof(report_data), ACTIVATION_REPORT_MQTT_PAYLOAD_FORMAT, report_msg);
    ACTIVATION_DEBUG("%s:%d report_data:%s\n", __func__, __LINE__, report_data);

    if (info_report_func != NULL) {
        info_report_func(handle, topic_name, 1, report_data, strlen(report_data));
    }

    ACTIVATION_DEBUG("devinfo report succeed");

    return SUCCESS_RETURN;
}

#ifdef HTTP_ACTIVATION_REPORT
int activation_parse_data(char *response_data)
{
    char response_result[6];
    char response_msg[18];
    char* result_start;
    char* result_end;
    int len;

    ACTIVATION_DEBUG("response data: %s\n", response_data);
    if(((result_start = strstr(response_data, ACTIVATION_RESPONSE_RESULT_START)) != NULL)
     && ((result_end = strstr(result_start, ACTIVATION_RESPONSE_RESULT_END)) != NULL)) {
        result_start += strlen(ACTIVATION_RESPONSE_RESULT_START);
        len = result_end - result_start;
        strncpy(response_result, result_start, len);
        response_result[len] = '\0';
        ACTIVATION_DEBUG("response result: %s\n", response_result);
    } else {
        ACTIVATION_ERR("parse reponse result failed\n");
    }

    if(((result_start = strstr(response_data, ACTIVATION_RESPONSE_MESSAGE_START)) != NULL)
     && ((result_end = strstr(result_start, ACTIVATION_RESPONSE_MESSAGE_END)) != NULL)) {
        result_start += strlen(ACTIVATION_RESPONSE_MESSAGE_START);
        len = result_end - result_start;
        strncpy(response_msg, result_start, len);
        response_msg[len] = '\0';
        ACTIVATION_DEBUG("response_msg: %s\n", response_msg);
    } else {
        ACTIVATION_ERR("parse reponse msg failed\n");
    }

    if((strcmp(response_result, "true") == 0)
      || (strcmp(response_msg, "success") == 0)) {
        ACTIVATION_DEBUG("activation success\n");
        return 0;
    } else {
        ACTIVATION_ERR("activation failed result: %s, msg: %s\n",
                                response_result, response_msg);
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
    if(!aos_kv_get("activation_report",
        &g_activation_report, &activation_len)) {
        return 0;
    }
#endif

    activation_get_report_msg(g_report_msg, sizeof(g_report_msg));

    snprintf(g_report_data, sizeof(g_report_data), ACTIVATION_REPORT_PAYLOAD_FORMAT, g_report_msg);
    ACTIVATION_DEBUG("%s:%d report_data:%s\n", __func__, __LINE__, g_report_data);

    snprintf(g_report_http_data, sizeof(g_report_http_data), ACTIVATION_REPORT_FORMAT, sizeof(g_report_data),g_report_data);

    ACTIVATION_DEBUG("%s:%d report http data:%s\n", __func__, __LINE__, g_report_http_data);
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(REPORT_SERVER_ADDR, REPORT_SERVER_PORT , &hints, &addr_list) != 0 ) {
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

        if (connect( sockfd, cur->ai_addr, (int)cur->ai_addrlen) == 0 ) {
            ACTIVATION_ERR("socket connect success\n");
            break;
        }
        close(sockfd);
    }

    ret = send(sockfd, g_report_http_data, strlen(g_report_http_data), 0);
    if(-1 == ret){
        ACTIVATION_ERR("socket send failed, errno=%d\n", errno);
        close( sockfd );
        return -1;
    }

    while(1) {
        if((ret = recv(sockfd, g_response_data, sizeof(g_response_data), 0)) == -1) {
            if((errno != EINTR) && (errno != EAGAIN) && (errno != EWOULDBLOCK)) {
                ACTIVATION_ERR("socket recv result failed errno=%d\n", errno);
                break;
            }
            aos_msleep(100);
        }
        else if(ret == 0) {
            ACTIVATION_DEBUG("connection closed by peer\n");
            break;
        }
        else {
            ACTIVATION_DEBUG("recv data len =%d\n", ret);
            break;
        }
    }

    if( ret != -1 ) {
        ret = activation_parse_data(g_response_data);
        if(ret == 0) {
#ifdef AOS_COMP_KV
            g_activation_report = 1;
            if(aos_kv_set("activation_report",
             &g_activation_report, sizeof(g_activation_report), 1)) {
                ACTIVATION_ERR("set report_device_info kv failed\n");
            }
#endif
        }
        else {
            ACTIVATION_ERR("activation report failed\n");
        }
    }

    close( sockfd );

    return 0;
}
#endif /* HTTP_ACTIVATION_REPORT */
