#ifndef ATCMD_UTIL_H
#define ATCMD_UTIL_H

#include <at/at.h>

#define CMD_SUCCESS_RSP "OK\r\n"
#define CMD_FAIL_RSP "ERROR\r\n"

#define AT_WIFI_CONN_PREFIX  "AT+WJAP"
#define AT_WIFI_CONN_FORMAT  "%s=%s,%s"

#define AT_WIFI_OBTAIN_IP     "AT+WJAPIP?"
#define AT_WIFI_RSP_IP_PREFIX "+WJAPIP:"

#define AT_AUTH_PREFIX "AT+IDMAU"
#define AT_AUTH_FORMAT "%s=\"%s\",\"%s\",\"%s\",\"%s\""

#define AT_DISCON_PREFIX      "AT+IDMCLS"
#define AT_CONN_PREFIX        "AT+IDMCON"
#define AT_QUERY_CONN_PREFIX  "AT+IDMSTA?"
#define AT_CONN_RSP_PREFIX    "+IDMSTA:"

#define AT_PROP_POST_PREFIX       "AT+IDMPP"
#define AT_PROP_POST_RSP_REFIX    "+IDMPP:"

#define AT_EVENT_POST_PREFIX      "AT+IDMEP"
#define AT_EVENT_POST_RSP_REFIX   "+IDMEP:"

#define AT_SERV_SET_REPLY_PREFIX  "AT+IDMSSR"

#define AT_OTA_START_PREFTIX      "AT+OTASTART"

#define AT_DM_PREFIX      "+IDM"
#define AT_CME_ERR_PREFIX "+CME ERROR:"
#define AT_ISUB_PREFIX    "+ISUB"
#define AT_OTA_PREFIX     "+OTA"

#define  AT_AWSS_START_PREFIX  "AT+IWSSTART"
#define  AT_AWSS_STOP_PREFIX   "AT+IWSSTOP"

#define AT_MAX_PREFIX_LEN  20

#define AT_RECV_POSFIX    "\r\n"

enum {
   SERV_SET_MSG,
   PROP_SET_MSG,
   OTA_INFO_MSG,
};

enum {
   WAIT_NONE = 0,
   WAIT_PACKET_ID,
   WAIT_NET_STATUS,
};

typedef struct {
    int type;
    int dev_id;
    char serv_id[30];
    char msg_id[30];
    char *payload;
    int payload_len;
} req_msg_t;

#define SEND_WAIT_REPLY_TIMEOUT_MS  1000

typedef int (*at_data_check_cb_t)(char data);


int atcmd_integer_check(char data);


int atcmd_info_from_stream(char *buf, unsigned int buflen, at_data_check_cb_t valuecheck);


int atcmd_get_state(void);


void atcmd_set_state(int state);


int atcmd_get_ret_val(void);


void atcmd_set_ret_val(int val);


void atcmd_notify_sender(void);


int atcmd_wait_notify(int timeout_ms);


int atcmd_insert_msg(void *msg_ptr);


int atcmd_tryfetch_msg(void *msg_ptr);


void atcmd_worker_halt(void);


void atcmd_worker_resume(void);


int atcmd_read(char *outbuf, int readsize);


int atcmd_send_wait_reply(const char *cmd, int cmdlen, bool delimiter,
                          const char *data, int datalen,
                          char *replybuf, int bufsize,
                          const at_reply_config_t *atcmdconfig);

int atcmd_send_no_reply(const char *data, int datalen, bool delimiter);

int atcmd_send_wait_ret_val(const char *data, int datalen, bool delimiter, int state, int timeout_ms);

int atcmd_add_slash_quote(char *dest, int size, char *payload, int payload_len);

int at_device_init(void);

#endif
