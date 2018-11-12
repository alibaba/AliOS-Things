/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef OTA_SERVICE_H_
#define OTA_SERVICE_H_

#define UOTA_VERSION   "1.0.0"
#define OTA_MAX_VER_LEN (64)

#define OTA_MSG_REPORT_LEN (256)
#define OTA_MSG_INFORM_LEN (128)
#define OTA_MSG_LEN_MAX    (2048)

#define OTA_COAP_URI_MAX_LEN  (135)
#define OTA_MQTT_TOPIC_LEN    (136)
#define OTA_URL_LEN (512)
#define OTA_RESP_MAX_LEN (OTA_URL_LEN + 512)
#define OTA_SIGN_LEN 256
#define OTA_HASH_LEN 66

typedef enum{
   OTA_PROTCOL_MQTT=0,
   OTA_PROTCOL_COAP,
   OTA_PROTCOL_HTTP,
   OTA_PROTCOL_HTTPS,
}OTA_PROTCOL_E;

typedef enum {
    OTA_REBOOT_FAIL = -15,
    OTA_UPGRADE_DIFF_FAIL = -14,
    OTA_UPGRADE_FAIL = -13,
    OTA_VERIFY_RSA_FAIL = -12,
    OTA_VERIFY_HASH_FAIL = -11,
    OTA_VERIFY_FAIL = -10,
    OTA_DOWNLOAD_WRITE_FAIL = -9,
    OTA_DOWNLOAD_READ_FAIL = -8,
    OTA_DOWNLOAD_CON_FAIL = -7,
    OTA_DOWNLOAD_IP_FAIL = -6,
    OTA_DOWNLOAD_URL_FAIL = -5,
    OTA_DOWNLOAD_FAIL = -4,
    OTA_TRANSPORT_FAIL = -3,
    OTA_INIT_VER_FAIL = -2,
    OTA_INIT_FAIL = -1,
    OTA_INIT = 0,
    OTA_CANCEL = 1,
    OTA_TRANSPORT = 2,
    OTA_DOWNLOAD = 3,
    OTA_VERIFY = 4,
    OTA_UPGRADE = 5,
    OTA_REBOOT = 6,
    OTA_MAX
} OTA_STATUS_E;

typedef enum
{
    COAP_CONTENT_TYPE_JSON,
    COAP_CONTENT_TYPE_CBOR,
}coap_content_type_e;

typedef enum
{
    COAP_MESSAGE_CON  = 0,       /* confirmable message */
    COAP_MESSAGE_NON  = 1,       /* non-confirmable message */
}coap_msg_type_e;

typedef void (*coap_cb_t)(void *p_arg, void *p_message);

typedef struct
{
    unsigned char           *p_payload;
    unsigned short           payload_len;
    coap_content_type_e      content_type;
    coap_msg_type_e          msg_type;
    void                     *user_data;
    coap_cb_t                resp_callback;
}ota_coap_message_t;

typedef struct {
    unsigned short packet_id;
    unsigned char  qos;
    unsigned char  dup;
    unsigned char  retain;
    unsigned short topic_len;
    unsigned int   payload_len;
    const char     *ptopic;
    const char     *payload;
} ota_mqtt_topic_t;

typedef enum {
    OTA_MQTT_EVENT_SUB_SUCCESS = 3,
    OTA_MQTT_EVENT_PUB_RECEIVED = 12,
    OTA_MQTT_EVENT_BUF_OVERFLOW = 13,
} ota_mqtt_event_t;

typedef struct {
    ota_mqtt_event_t  event;
    ota_mqtt_topic_t  *topic;
} ota_mqtt_msg_t;

typedef void (*ota_cloud_cb_t)(char *buf);

typedef struct
{
    char  hash_type;
    char  sign_en;
    char  sign_type;
    int   sign_len;
    char* url;         /*Dowdload URL*/
    char* hash;        /*Dowdload hash*/
    char* sign;        /*Dowdload signatue*/
    int  (*init)(void);
    int  (*inform)(const char* ver, char* pk, char* dn);
    int  (*response)(const char *buf);
    int  (*upgrade)(ota_cloud_cb_t msg_cb, char* pk, char* dn);
    int  (*status)(int per, char* pk, char* dn);
    int  (*deinit)(void);
} ota_transport_t;

typedef struct {
    int (*start)(char *url); /*start download*/
    int (*stop)(void);       /*stop download*/
}ota_download_t;

typedef struct {
    unsigned char  inited;  /*If is inted*/
    char  pk[20+1];/*Product Key*/
    char  ps[64+1];/*Product secret*/
    char  dn[32+1];/*Device name*/
    char  ds[64+1];/*Device secret*/
    OTA_PROTCOL_E  trans_protcol;  /*default:0--> MQTT 1-->COAP*/
    OTA_PROTCOL_E  dl_protcol;     /*default:3--> HTTPS 1-->COAP 2-->HTTP*/
    unsigned char  sign_type;      /*default:0--> sha256 1--> md5 2-->RSA*/
    char           ota_ver[OTA_MAX_VER_LEN];  /*OTA FW version*/
    char           sys_ver[OTA_MAX_VER_LEN];  /*OTA System version*/
    ota_transport_t *h_tr;    /*OTA tansport manager*/
    ota_download_t  *h_dl;    /*OTA download manager*/
    void*    h_ver;           /*OTA verify manager*/
    void*    h_ch;            /*OTA channel handle:mqtt,coap*/
} ota_service_t;

/*OTA export service APIs*/
int ota_service_init(ota_service_t* ctx);
int ota_service_deinit(ota_service_t* ctx);

/*OTA intenal APIs*/
ota_service_t *ota_get_service(void);
ota_transport_t *ota_get_transport_mqtt(void);
ota_transport_t *ota_get_transport_coap(void);
ota_download_t *ota_get_download_http(void);
ota_download_t *ota_get_download_coap(void);
void ota_set_splict_size(unsigned int size);
const char *ota_to_capital(char *value, int len);
int ota_hex_str2buf(const char* src, char* dest, unsigned int dest_len);
void ota_set_ota_version(char* ver);
void ota_set_firm_size(unsigned int size);
void ota_set_upg_flag(unsigned int upd_flag);
char ota_get_status(void);
unsigned int ota_get_firm_size(void);
#endif /* OTA_SERVICE_H_ */
