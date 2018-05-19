/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _SAL_CONFIG_MODULE
#define _SAL_CONFIG_MODULE

#include <hal/soc/uart.h>
#include "drv_usart.h"
#include <csi_kernel.h>
#include "ringbuffer.h"

typedef enum {
    NETM_CMD_INIT = 1,
    NETM_CMD_RST,
    NETM_CMD_CPIN,
    NETM_CMD_CSQ,
    NETM_CMD_CREG,
    NETM_CMD_CGATT,
    NETM_CMD_CSTT,
    NETM_CMD_CIICR,
    NETM_CMD_GETIP,
    NETM_CMD_CONN,
    NETM_CMD_OPEN,
    NETM_CMD_DISC,
    NETM_CMD_SEND,
    NETM_CMD_MUX_SEND,
    NETM_CMD_CIPHEAD,
    NETM_CMD_STAT,
    NETM_CMD_MODE,
    NETM_CMD_RESTORE,
    NETM_CMD_PING,
    NETM_CMD_INFO_LINK,
    NETM_CMD_INFO_MAC,
    NETM_CMD_INFO,
    NETM_CMD_DOMAIN,
    NETM_CMD_SHUT,
    NETM_CMD_PDPC,
    NETM_CMD_GPRS_MODE,
    NETM_CMD_SET_AP,
    NETM_CMD_AP_DISC,
    NETM_CMD_MUX_MODE,
    NETM_CMD_SMART_CFG,
    NETM_CMD_UART_CONFIG,
    NETM_CMD_NET_ACK,
    NETM_CMD_RAW,
} netm_cmd_t;

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
    char param[64];
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

k_msgq_handle_t g_netm_queue;
k_msgq_handle_t g_netm2app_queue;
k_mutex_handle_t g_cmd_mutex;
k_mutex_handle_t g_link_mutex;
k_timer_handle_t timer_atmsg;

netm_status_e g_netm_status = NETM_STATUS_UNKNOW;

int g_cmd_inprogress;
netm_msg_t netm_resp_msg;
netm_dev_t g_netm_dev;
mux_send_param_t g_mux_send_frame;
typedef void (*rdata_cb)(int fd, void *data, size_t len, bool enable);

static int netm_uart_init(void);
static int netm_uart_write(const char *buf, uint32_t nbytes);
static void esp8266_wifi_module_socket_data_handle(int fd, void *data, size_t len, bool enable);


#endif /*_SAL_CONFIG_MODULE*/
