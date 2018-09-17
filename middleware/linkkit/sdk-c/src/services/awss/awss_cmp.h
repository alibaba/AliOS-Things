/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#ifndef AWSS_CMP_H
#define AWSS_CMP_H

#ifdef __cplusplus
extern "C" {
#endif

#define MSG_REQ_ID_LEN                 (16)
#define TOPIC_LEN_MAX                  (128)
#define ILOP_VER                       "1.0"

#define TOPIC_AWSS_WIFILIST             "/sys/%s/%s/awss/event/wifilist/get"
#define TOPIC_AWSS_SWITCHAP             "/sys/%s/%s/awss/device/switchap"
#define TOPIC_AWSS_GETDEVICEINFO_MCAST  "/sys/awss/device/info/get"
#define TOPIC_AWSS_GETDEVICEINFO_UCAST  "/sys/%s/%s/awss/device/info/get"
#define TOPIC_GETDEVICEINFO_MCAST       "/sys/device/info/get"
#define TOPIC_GETDEVICEINFO_UCAST       "/sys/%s/%s/device/info/get"
#define TOPIC_AWSS_NOTIFY               "/sys/awss/device/info/notify"
#define TOPIC_AWSS_CONNECTAP_NOTIFY     "/sys/awss/event/connectap/notify"
#define TOPIC_NOTIFY                    "/sys/device/info/notify"
#define TOPIC_SWITCHAP                  "/sys/%s/%s/thing/awss/device/switchap"
#define TOPIC_SWITCHAP_REPLY            "/sys/%s/%s/thing/awss/device/switchap_reply"
#define TOPIC_ZC_ENROLLEE               "/sys/%s/%s/thing/awss/enrollee/found"
#define TOPIC_ZC_ENROLLEE_REPLY         "/sys/%s/%s/thing/awss/enrollee/found_reply"
#define TOPIC_ZC_CHECKIN                "/sys/%s/%s/thing/awss/enrollee/checkin"
#define TOPIC_ZC_CHECKIN_REPLY          "/sys/%s/%s/thing/awss/enrollee/checkin_reply"
#define TOPIC_ZC_CIPHER                 "/sys/%s/%s/thing/cipher/get"
#define TOPIC_ZC_CIPHER_REPLY           "/sys/%s/%s/thing/cipher/get_reply"
#define TOPIC_MATCH_REPORT              "/sys/%s/%s/thing/awss/enrollee/match"
#define TOPIC_MATCH_REPORT_REPLY        "/sys/%s/%s/thing/awss/enrollee/match_reply"
#define TOPIC_RESET_REPORT              "/sys/%s/%s/thing/reset"
#define TOPIC_RESET_REPORT_REPLY        "/sys/%s/%s/thing/reset_reply"

#define METHOD_DEV_INFO_NOTIFY          "device.info.notify"
#define METHOD_AWSS_DEV_INFO_NOTIFY     "awss.device.info.notify"
#define METHOD_AWSS_CONNECTAP_NOTIFY    "awss.event.connectap.notify"
#define METHOD_EVENT_ZC_SWITCHAP        "thing.awss.device.switchap"
#define METHOD_EVENT_ZC_ENROLLEE        "thing.awss.enrollee.found"
#define METHOD_EVENT_ZC_CHECKIN         "thing.awss.enrollee.checkin"
#define METHOD_EVENT_ZC_CIPHER          "thing.cipher.get"
#define METHOD_MATCH_REPORT             "thing.awss.enrollee.match"
#define METHOD_RESET_REPORT             "thing.reset"

#define AWSS_ACK_FMT                    "{\"id\":%s, \"code\":%d, \"data\":%s}"
#define AWSS_REQ_FMT                    "{\"id\":%s, \"version\":\"%s\", \"method\":\"%s\", \"params\":%s}"
#define AWSS_JSON_PARAM                 "params"
#define AWSS_JSON_CODE                  "code"
#define AWSS_JSON_ID                    "id"

#define AWSS_KV_RST                     "awss.rst"

#if(AWSS_CMP_DEBUG==1)
#define awss_cmp_debug(fmt, args...) log_debug(fmt, ##args)
#else
#define awss_cmp_debug(fmt, args...)
#endif

/**
 * @brief this is a network address structure, including host(ip or host name) and port.
 */
typedef struct {
    char host[16]; /**< host ip(dotted-decimal notation) or host name(string) */
    unsigned short port; /**< udp port or tcp port */
} platform_netaddr_t;

struct awss_cmp_couple {
    char *topic;
    void *cb;
};

struct coap_session_ctx_t {
    void *request;
    void *remote;
    char is_mcast;
};

enum {
    AWSS_CMP_PKT_TYPE_REQ = 1,
    AWSS_CMP_PKT_TYPE_RSP,
};

int awss_cmp_local_init();
int awss_cmp_local_deinit();
int awss_cmp_online_init();
int awss_cmp_online_deinit();
int awss_token_remain_time();
int awss_token_timeout();
int awss_update_token();
int awss_report_token();
int awss_report_reset();
int awss_check_reset();

int awss_cmp_coap_loop(void *param);
int awss_cmp_coap_register_cb(char *topic, void *cb);
int awss_cmp_coap_send(void *buf, unsigned int len, void *sa, const char *uri, void *cb, unsigned short *msgid);
int awss_cmp_coap_send_resp(void *buf, unsigned int len, void *sa, const char *uri, void *cb);
int awss_cmp_coap_ob_send(void *buf, unsigned int len, void *sa, const char *uri, void *cb);
int awss_cmp_coap_deinit();

int awss_cmp_mqtt_register_cb(char *topic, void *cb);
int awss_cmp_mqtt_unregister_cb(char *topic);
int awss_cmp_mqtt_send(char *topic, void *pkt, int pkt_len, int qos);

int awss_release_coap_ctx(void *session);
void *awss_cpy_coap_ctx(void *request, void *remote, char mcast);

char *awss_cmp_get_coap_payload(void *request, int *payload_len);
char *awss_build_sign_src(char *sign_src, int *sign_src_len);
const char *awss_build_topic(const char *topic_fmt, char *topic, unsigned int tlen);
unsigned char awss_cmp_get_coap_code(void *request);

int awss_build_packet(int type, void *id, void *ver, void *method, void *data, int code, void *pkt, int *pkt_len);

int online_connectap_monitor(void *ctx, void *resource, void *remote, void *request);
int awss_enrollee_checkin(char *topic, int topic_len, void *payload, int payload_len, void *ctx);
int awss_online_switchap(char *topic, int topic_len, void *payload, int payload_len, void *ctx);
int awss_report_enrollee_reply(char *topic, int topic_len, void *payload, int payload_len, void *ctx);
int awss_get_cipher_reply(char *topic, int topic_len, void *payload, int payload_len, void *ctx);
int awss_report_token_reply(char *topic, int topic_len, void *payload, int payload_len, void *ctx);
int awss_report_reset_reply(char *topic, int topic_len, void *payload, int payload_len, void *ctx);

#ifdef __cplusplus
}
#endif

#endif
