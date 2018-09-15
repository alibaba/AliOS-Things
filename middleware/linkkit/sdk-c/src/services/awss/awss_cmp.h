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

/**
 * @brief this is a network address structure, including host(ip or host name) and port.
 */
typedef struct {
    char host[16]; /**< host ip(dotted-decimal notation) or host name(string) */
    uint16_t port; /**< udp port or tcp port */
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
int awss_cmp_coap_send(void *buf, uint32_t len, void *sa, const char *uri, void *cb, uint16_t *msgid);
int awss_cmp_coap_send_resp(void *buf, uint32_t len, void *sa, const char *uri, void *cb);
int awss_cmp_coap_ob_send(void *buf, uint32_t len, void *sa, const char *uri, void *cb);
int awss_cmp_coap_deinit();

int awss_cmp_mqtt_register_cb(char *topic, void *cb);
int awss_cmp_mqtt_unregister_cb(char *topic);
int awss_cmp_mqtt_send(char *topic, void *pkt, int pkt_len, int qos);

int awss_release_coap_ctx(void *session);
void *awss_cpy_coap_ctx(void *request, void *remote, char mcast);

char *awss_cmp_get_coap_payload(void *request, int *payload_len);
char *awss_build_sign_src(char *sign_src, int *sign_src_len);
const char *awss_build_topic(const char *topic_fmt, char *topic, uint32_t tlen);
uint8_t awss_cmp_get_coap_code(void *request);

int awss_build_packet(int type, void *id, void *ver, void *method, void *data, int code, void *pkt, int *pkt_len);

int online_connectap_monitor(void *ctx, void *resource, void *remote, void *request);

void awss_enrollee_checkin(void *pcontext, void *pclient, void *msg);
void awss_online_switchap(void *pcontext, void *pclient, void *msg);
void awss_report_enrollee_reply(void *pcontext, void *pclient, void *msg);
void awss_get_cipher_reply(void *pcontext, void *pclient, void *msg);
void awss_report_token_reply(void *pcontext, void *pclient, void *msg);
void awss_report_reset_reply(void *pcontext, void *pclient, void *msg);
int awss_cmp_mqtt_get_payload(void *mesg, char **payload, uint32_t *playload_len);

#ifdef __cplusplus
}
#endif

#endif
