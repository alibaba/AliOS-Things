/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _SAL_CONFIG_MODULE
#define _SAL_CONFIG_MODULE

#include "aos/hal/uart.h"
#include "drv_usart.h"
#include <csi_kernel.h>
#include "ringbuffer.h"

typedef enum {
    AT_CMD_NOECHO = 1,
    AT_CMD_RST,
    AT_CMD_CPIN,
    AT_CMD_CSQ,
    AT_CMD_CREG,
    AT_CMD_CGATT,
    AT_CMD_CSTT,
    AT_CMD_CIICR,
    AT_CMD_GETIP,
    AT_CMD_CONN,
    AT_CMD_OPEN,
    AT_CMD_DISC,
    AT_CMD_SEND,
    AT_CMD_MUX_SEND,
    AT_CMD_CIPHEAD,
    AT_CMD_STAT,
    AT_CMD_MODE,
    AT_CMD_RESTORE,
    AT_CMD_PING,
    AT_CMD_INFO_LINK,
    AT_CMD_INFO_MAC,
    AT_CMD_INFO,
    AT_CMD_DOMAIN,
    AT_CMD_SHUT,
    AT_CMD_PDPC,
    AT_CMD_GPRS_MODE,
    AT_CMD_SET_AP,
    AT_CMD_AP_DISC,
    AT_CMD_MUX_MODE,
    AT_CMD_SMART_CFG,
    AT_CMD_UART_CONFIG,
    AT_CMD_NET_ACK,
    AT_CMD_RAW,
} at_cmd_t;

typedef enum {
    AT_TYPE_NULL = 0,
    AT_TYPE_TCP_SERVER,
    AT_TYPE_TCP_CLIENT,
    AT_TYPE_UDP_UNICAST,
    AT_TYPE_MAX
} at_conn_t;

typedef struct netm_dev_s {
    uint8_t                block;       /*1: block 0: nonblock */
    usart_handle_t usart_handle;

    k_sem_handle_t         ksem_write;
    k_sem_handle_t         ksem_read;

    /* I/O buffers */
    ringbuffer_t *rb;

    usart_event_e          usart_txevent;
    usart_event_e          usart_rxevent;
    uint8_t                stat_txmit;
    uint8_t                stat_rxmit;
    uint32_t               read_num;
    uint8_t                flowctrl;
} netm_dev_t;

typedef enum {
    NETM_STATUS_UNKNOW = 0,
    NETM_STATUS_READY,
    NETM_STATUS_LINKDOWN,
    NETM_STATUS_CONFIGING,
    NETM_STATUS_LINKUP,
    NETM_STATUS_GOTIP,
    NETM_STATUS_SOCKET_DISCONNECT,
    NETM_STATUS_SOCKET_CONNECTED,
} netm_status_e;

typedef enum {
    MODE_STATION = 1,
    MODE_SOFTAP,
    MODE_STATION_SOFTAP
} esp_wifi_mode_t;

#define ISMUXMODE  1
/* Change to include data slink for each link id respectively. <TODO> */
typedef struct link_s {
    int fd;
    aos_sem_t sem_start;
    aos_sem_t sem_close;
} link_t;

typedef enum {
    NETM_CONN_TCP = 1,
    NETM_CONN_UDP,
    NETM_CONN_RAW
} netm_conn_e;

typedef struct {
    int msg_src;
    int cmd;
    char param[128];
    int param_len;
    int result;
    uint8_t isAsync;
    uint32_t timeout;
} netm_msg_t;

typedef enum {
    SRC_APP = 1,
    SRC_DRIVER,
} msg_src_t;

typedef struct {
    void *data;
    uint32_t len;
} send_param_t;

typedef struct {
    int id;
    void *data;
    uint32_t len;
} mux_send_param_t;

typedef struct {
    int id;
    netm_conn_e type;
    char *hostip;
    uint16_t port;
} connect_param_t;

typedef struct {
    char *ssid;
    char *psw;
} ap_set_param_t;

typedef struct {
    uint32_t baud;
    uint8_t databits;
    uint8_t stopbits;
    uint8_t parity;
    uint8_t flow_control;
    uint8_t writetoflash;
} uart_config_param_t;

typedef struct {
    int rssi;
    int ber;
} sig_qual_resp_t;

typedef struct {
    int n;
    int stat;
} regs_stat_resp_t;

#define NETM_SOCKET_NUM 5

#define FRONT 1
#define BACK  0
#define NO_WAIT 0
#define WAIT_FOREVER  -1
#define SKTPT_SOCKET_OFFSET     34
#define CONFIG_NETM_RDBUFSIZE   4096
#define CONFIG_NETM_BAUD        115200
#define LINE_LEN 128
#define ARRAY_SIZE(array) (sizeof(array)/sizeof(array[0]))

typedef void (*rdata_cb)(int fd, void *data, size_t len);
typedef void (*linkstat_cb)(int linkid, int status);

extern int at_read_cb_handle(int idx, rdata_cb rcb, linkstat_cb lcb);

int netm_init(void);

int netm_softreset(void);

int netm_close_echo(uint8_t isAsync);

int netm_set_mode(uint8_t mode);

int netm_set_mux_mode(uint8_t mode, uint8_t isAsync);

int netm_connect(int id, at_conn_t type, char *srvname, uint16_t port);

int netm_mux_send(int id, const uint8_t *pdata, int len);

int netm_parse_domain(const char *servername, char *ip);

int netm_disconnect(int id);

int netm_get_local_ip(char *buf, size_t len);

int netm_get_link_status(void);

int netm_connect_to_ap(const char *ssid, const char *psw);

#endif /*_SAL_CONFIG_MODULE*/
