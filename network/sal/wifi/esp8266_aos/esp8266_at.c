/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */


#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <aos/aos.h>
#include <sal_arch.h>
#include <sal_ipaddr.h>
#include <sal.h>
#include "config_esp8266.h"
#include "hal_uart_event.h"

#define TAG "esp8266_at"

uart_dev_t uart_wifi_esp8266;

#ifdef ESP8266_USE_HARD_RESET
gpio_dev_t* esp8266_reset_pin=NULL;
gpio_dev_t* esp8266_fw_update_pin=NULL;   	
gpio_dev_t* esp8266_pwr_pin=NULL; 
#endif

char net_ipaddr[16], net_mask[16], net_gw[16];

#define DEF_MAX_NETMQ_MSG_LENGTH	3
static kbuf_queue_t g_netm_queue;
static kbuf_queue_t g_netm2app_queue;

static uint8_t g_u8MsgQNetmBuf[ DEF_MAX_NETMQ_MSG_LENGTH*sizeof(netm_msg_t) ];
static uint8_t g_u8MsgQNetm2AppBuf[ DEF_MAX_NETMQ_MSG_LENGTH*sizeof(netm_msg_t) ];

static kmutex_t g_cmd_mutex;
static ktimer_t timer_atmsg;


netm_status_e g_netm_status = NETM_STATUS_UNKNOW;

typedef struct
{
    int   type;
    char *cmd_name;
    char *resp_name;
} at_resp_cmd_t;

static ktask_t tid;
#define DEF_NETM_TASK_STACK_SIZE	(1536/2)

static cpu_stack_t netm_task_buf[DEF_NETM_TASK_STACK_SIZE];

int              g_cmd_inprogress;
netm_msg_t       netm_resp_msg;
netm_dev_t       g_netm_dev;
mux_send_param_t g_mux_send_frame;
rdata_cb         g_rdata_handle=NULL;
linkstat_cb      g_link_status=NULL;

static char         at_dev_ringbuf[CONFIG_NETM_RDBUFSIZE];
static ringbuffer_t uart_rb;

 char *strlwr(char *str)
{
    char *orig = str;

    for (; *str != '\0'; str++) {
        *str = tolower(*str);
    }

    return orig;
}

static kstat_t at_wait_cmd_done(netm_msg_t *resp, at_cmd_t cmd)
{
    kstat_t ret=-(RHINO_SYS_FATAL_ERR);
		int received_msgsize=0;
    /* wait CMD finish */
		ret = krhino_buf_queue_recv(&g_netm2app_queue, AOS_WAIT_FOREVER, (void *)resp, &received_msgsize );
    if (ret == RHINO_SUCCESS) {

        if (resp->result == -1) {
            ret = -(RHINO_SYS_FATAL_ERR);
            LOGE(TAG, "cmd=%d timeout", cmd );
        }

        if (resp->result >= 1) { // response data parse
            // LOGD(TAG, "cmd=%d result: OK", cmd);
            ret = RHINO_SUCCESS;
        } else {
            LOGD(TAG, "cmd=%d result: ERROR", cmd);
            ret = -(RHINO_SYS_FATAL_ERR);
        }

    } else {
        LOGE(TAG, "krhino_buf_queue_recv ret=%d\n", ret);
    }

    return ret;
}

#define DEFAULT_CMD_TIMEOUT (3000)
static int at_exec_command(netm_msg_t *cmd, netm_msg_t *resp)
{
    kstat_t ret;

	ret = krhino_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER  );
	if (ret != RHINO_SUCCESS)
		goto exit_at_exec_command;

    cmd->msg_src = SRC_APP;

    if (!cmd->timeout)
        cmd->timeout = DEFAULT_CMD_TIMEOUT;

    ret = krhino_buf_queue_send(&g_netm_queue, (void *)cmd, sizeof(netm_msg_t) );
    if ( ret != RHINO_SUCCESS < 0) {
				krhino_mutex_unlock (&g_cmd_mutex);
        goto exit_at_exec_command;
    }

    ret = at_wait_cmd_done ( resp, cmd->cmd );
    
    krhino_mutex_unlock (&g_cmd_mutex);

exit_at_exec_command:

		return (ret==RHINO_SUCCESS)?0:-1;
}

static void set_link_status(netm_status_e status, uint8_t flag)
{

    if (flag == 0) {
        if (g_netm_status == NETM_STATUS_CONFIGING) {
            if ((status == NETM_STATUS_LINKDOWN) ||
                (status == NETM_STATUS_SOCKET_DISCONNECT)) {
                return;
            }
        } else if (g_netm_status == NETM_STATUS_LINKDOWN) {
            if (status == NETM_STATUS_SOCKET_DISCONNECT) {
                return;
            }
        } else if (g_netm_status == NETM_STATUS_LINKUP) {
            if (status == NETM_STATUS_LINKDOWN) {
                return;
            }
        }
    }

    g_netm_status = status;
    LOGI(TAG, "netm status change to %d", status);
}

static timer_cb_t at_send_timeout_callback(uint32_t arg)
{
    static uint8_t retry = 0;
    kstat_t ret;

    retry++;

		printf("%s %d\r\n", __func__, __LINE__ );
	
    if (retry > 10) {
        // Do: reset
    }
		
    netm_resp_msg.result = -1;
    g_cmd_inprogress     = 0;
		ret = krhino_buf_queue_send(&g_netm2app_queue, (void *)&netm_resp_msg, sizeof(netm_resp_msg));
		if (ret != RHINO_SUCCESS)
			LOGD(TAG, "krhino_buf_queue_send result: %d\n", ret );

    return NULL;
}

#if 1
void atresp_dump_buf(uint8_t* data, int len, int mode )
{
//	if(!mode) 
		return 0;
		int i=0;
	#if 0
		if (mode)
			printf("\n<----");
		else
			printf("\n---->");
		
		for(i=0;i<len;i++)
			printf("%02x ", data[i]);
		printf("\n");
	#endif
		printf("\n");
		if (!mode)
			printf("AT Resp(%d):", len);
		else
			printf("AT Req(%d):", len);
		
		for(i=0;i<len;i++)
			printf("%c", data[i]);
		printf("\n");
		
		//printf("[\n");
		//	printf("%s\n", (char*)data);
		//printf("]\n");
	
}

static void CBFunUartEvent(int32_t idx, E_HAL_UART_EVENT event)
{
    netm_dev_t    *udev   = &g_netm_dev;
    uart_dev_t*		 handle = udev->hal_uart_handle;
    netm_msg_t     netm_read_msg;

    switch (event) {
        case eUART_EVENT_SEND_COMPLETE:
						krhino_sem_give(&udev->ksem_write);
			
            break;

        case eUART_EVENT_RECEIVED: {

						kbuf_queue_info_t info;
						krhino_buf_queue_info_get ( &g_netm_queue, &info);
						
						if ( info.cur_num == 0 ) 
						{
								netm_read_msg.msg_src = SRC_DRIVER;
								krhino_buf_queue_send ( &g_netm_queue, &netm_read_msg, sizeof(netm_msg_t) );
						}

        } break;

        case eUART_EVENT_RX_OVERFLOW:
            LOGE(TAG, "uart fifo rx overflow!!");
            break;

        default:
            break;
    }
}
#endif

static int netm_uart_init(void)
{	
    memset(&g_netm_dev, 0x00, sizeof(g_netm_dev));
    g_netm_dev.block       		 = 1;
    
   	if ( krhino_sem_create(&g_netm_dev.ksem_write, "ksem_write", 0) != RHINO_SUCCESS )
				goto exit_netm_uart_init;
		
   	if ( krhino_sem_create(&g_netm_dev.ksem_read, "ksem_read", 0) != RHINO_SUCCESS )
				goto exit_netm_uart_init;
		 
    //g_netm_dev.usart_handle = usart_handle; // will be used in callback
		g_netm_dev.hal_uart_handle = (void*)&uart_wifi_esp8266;
		uart_wifi_esp8266.priv = (void*) CBFunUartEvent;
		
    g_netm_dev.rb = &uart_rb;
		
    int ret = ringbuffer_create(g_netm_dev.rb, at_dev_ringbuf, CONFIG_NETM_RDBUFSIZE);

    if (ret < 0) {
        LOGE(TAG, "netm_uart rb_create error\n");
        return -1;
    }

		if ( hal_uart_init(&uart_wifi_esp8266) != 0 )
		{
				LOGE(TAG, "uart_init error");
				return -1;
		}
		
#ifdef CONFIG_NETM_FLOW_CONTROL
    //ret = csi_usart_config_flowctrl(usart_handle, USART_FLOWCTRL_CTS_RTS);

    //if (ret == 0) {

    //    g_netm_dev.flowctrl = 1;
    //    LOGD(TAG, "set flowctrl success");

    //} else if (ret < 0) {
    //    LOGE(TAG, "set flowctrl error %x", ret);
    //    return -1;
    //}
#endif

    return 0;
    
exit_netm_uart_init:
	
	krhino_sem_del(&g_netm_dev.ksem_write);
	krhino_sem_del(&g_netm_dev.ksem_read);
	
	return -1;
	
}

static int netm_uart_write(const char *buf, uint32_t nbytes)
{
    kstat_t ret;
	
		if (buf)	atresp_dump_buf(buf, nbytes, 1 );

		if ( hal_uart_send ( &uart_wifi_esp8266, buf, nbytes, AOS_WAIT_FOREVER ) != 0 )
			return -1;

    // wait transimit done
    // ret = krhino_sem_take(&g_netm_dev.ksem_write, AOS_WAIT_FOREVER);
    // if (ret < 0)
    //    return ret;

    return nbytes;
}

int netm_uart_config(uint32_t baud, uint8_t databits, uint8_t stopbits,
                     uint8_t parity, uint8_t flow_control, uint8_t writetoflash)
{
    int        ret = -1;
    netm_msg_t cmd;
    netm_msg_t resp;

    memset(&cmd, 0, sizeof(cmd));
    memset(&resp, 0, sizeof(resp));

    cmd.cmd     = AT_CMD_UART_CONFIG;
    cmd.timeout = 2000;

    snprintf(cmd.param, 64, "%d,%d,%d,%d,%d\r\n", baud, databits, stopbits,
             parity, flow_control);

    ret = at_exec_command(&cmd, &resp);

    if (ret == 0) {
        if (resp.result == 1) {
            return 0;
        } else if (resp.result == -1) {
            return -1;
        }
    }

    return ret;
}

int netm_softreset(void)
{
    int        ret = -1;
    netm_msg_t cmd;
    netm_msg_t resp;
    memset(&cmd, 0, sizeof(cmd));
    memset(&resp, 0, sizeof(resp));

		#ifdef ESP8266_USE_HARD_RESET
		ret = netm_hardreset();    
		#endif

    cmd.cmd     = AT_CMD_RST;
    cmd.timeout = 5000;

    ret = at_exec_command(&cmd, &resp);

    return ret;
}

#ifdef ESP8266_USE_HARD_RESET
int netm_hardreset(void)
{
	printf("[%s %d]\r\n", __func__, __LINE__ );

	if ( esp8266_reset_pin )
			hal_gpio_init(esp8266_reset_pin);

	if ( esp8266_fw_update_pin )
			hal_gpio_init(esp8266_fw_update_pin);

	if ( esp8266_pwr_pin )
			hal_gpio_init(esp8266_pwr_pin);

	if ( esp8266_reset_pin )
	{
			printf("[%s %d]\r\n", __func__, __LINE__ );
			hal_gpio_output_high(esp8266_reset_pin);
			aos_msleep(100);
			hal_gpio_output_low(esp8266_reset_pin);
			aos_msleep(200);
			hal_gpio_output_high(esp8266_reset_pin);
			return 0;
	} else {
	
			if ( esp8266_pwr_pin )
					hal_gpio_output_high(esp8266_pwr_pin);

			if ( esp8266_fw_update_pin )
					hal_gpio_output_high(esp8266_fw_update_pin);

			aos_msleep(300);

			// Set 0 to enable WIFI module firmware update.
			// Set 1 to Disable WIFI module firmware update.
			if ( esp8266_fw_update_pin )
					hal_gpio_output_high(esp8266_fw_update_pin);

			aos_msleep(100);

			if ( esp8266_pwr_pin )
					hal_gpio_output_low(esp8266_pwr_pin);
						
		return 0;
	}
	return -1;
}
#endif

int netm_close_echo(uint8_t isAsync)
{
    int        ret = -1;
    netm_msg_t cmd;
    netm_msg_t resp;

    memset(&cmd, 0, sizeof(cmd));
    memset(&resp, 0, sizeof(resp));

    cmd.cmd     = AT_CMD_NOECHO;
    cmd.timeout = 1000;

    ret = at_exec_command(&cmd, &resp);

    return ret;
}

int netm_set_mode(uint8_t mode)
{
    int        ret = -1;
    netm_msg_t cmd;
    netm_msg_t resp;

    memset(&cmd, 0, sizeof(cmd));
    memset(&resp, 0, sizeof(resp));

    cmd.cmd     = AT_CMD_MODE;
    cmd.timeout = 2000;

    snprintf(cmd.param, 4, "%d\r\n", mode);

    ret = at_exec_command(&cmd, &resp);

    return ret;
}

int netm_set_mux_mode(uint8_t mode, uint8_t isAsync)
{
    int        ret = -1;
    netm_msg_t cmd;
    netm_msg_t resp;

    memset(&cmd, 0, sizeof(cmd));
    memset(&resp, 0, sizeof(resp));

    cmd.cmd     = AT_CMD_MUX_MODE;
    cmd.timeout = 1000;

    snprintf(cmd.param, 4, "%d\r\n", mode);

    ret = at_exec_command(&cmd, &resp);

    return ret;
}

int netm_connect(int id, at_conn_t type, char *srvname, uint16_t port)
{
    /*
    AT+CIPSTART="TCP","192.168.103.121",6000
    CONNECT
    */

    int        ret = -1;
    netm_msg_t cmd;
    netm_msg_t resp;

    memset(&cmd, 0, sizeof(cmd));
    memset(&resp, 0, sizeof(resp));

    /* check parameter */
    if (id > 5 || id < 0) {
        LOGE(TAG, "id=%d err!", id);
        return -1;
    }

    if (type <= AT_TYPE_NULL || type >= AT_TYPE_MAX) {
        LOGE(TAG, "type=%d err!", type);
        return -1;
    }

    cmd.cmd     = AT_CMD_CONN;
    cmd.timeout = 10000;

    switch (type) {
        case AT_TYPE_TCP_SERVER:
            /* TCP Server can NOT ignore lport */
            // snprintf(cmd.param, 64, "%d,\"TCP\",%s,%d\r", id, srvname, port);
            break;

        case AT_TYPE_TCP_CLIENT:
            snprintf(cmd.param, 64, "%d,\"TCP\",\"%s\",%d\r\n", id, srvname,
                     port);

            break;

        case AT_TYPE_UDP_UNICAST:
            snprintf(cmd.param, 64, "%d,\"UDP\",\"%s\",%d\r\n", id, srvname,
                     port);

            break;

        default:
            LOGE(TAG, "type=%d err!", type);
            return -1;
    }

    ret = at_exec_command(&cmd, &resp);

    return ret;
}

int netm_mux_send(int id, const uint8_t *pdata, int len)
{
    int              ret   = -1;
    static uint8_t   retry = 0;
    netm_msg_t       cmd;
    netm_msg_t       resp;
    mux_send_param_t param;

    memset(&cmd, 0, sizeof(cmd));
    memset(&resp, 0, sizeof(resp));

    if (id >= NETM_SOCKET_NUM) {
        LOGE(TAG, "id(%d) err", id);
        return -1;
    }

    if (len <= 0 || len > 1460) {
        LOGE(TAG, "len(%d) err");
        return -1;
    }

    cmd.cmd     = AT_CMD_MUX_SEND;
    cmd.timeout = 5000;
    snprintf(cmd.param, 16, "%d,%d\r\n", id, len);

    cmd.param_len = strlen(cmd.param);

    param.id   = id;
    param.data = (uint8_t *)pdata;
    param.len  = len;

    memcpy(cmd.param + cmd.param_len, &param, sizeof(param));

    ret = at_exec_command(&cmd, &resp);

    if (ret == 0) {

        if (resp.result > 0) { // response data parse
            ret = resp.result;
        } else {
            if (resp.result == -1) {
                ret = -1;
                LOGE(TAG, "send timeout");
            } else {
                ret = -1;
            }

            retry++;

            if (retry > 3) {
                // DO : reset
            }
        }
    }

    return ret;
}

int netm_auto_dnsserver(void)
{
    int        ret = -1;
    netm_msg_t cmd;
    netm_msg_t resp;

    memset(&cmd, 0, sizeof(cmd));
    memset(&resp, 0, sizeof(resp));

    cmd.cmd     = AT_CMD_AUTO_DNS;
    cmd.timeout = 2000;

    ret = at_exec_command(&cmd, &resp);

    return ret;
}

int netm_query_dnsserver(void)
{
		char ip[16]={0};
    int        ret = -1;
    netm_msg_t cmd;
    netm_msg_t resp;

    memset(&cmd, 0, sizeof(cmd));
    memset(&resp, 0, sizeof(netm_msg_t));

    cmd.cmd     = AT_CMD_INFO_DNS;
    cmd.timeout = 2000;

    ret = at_exec_command(&cmd, &resp);

    if (ret == 0) {
        if (resp.result == 1) { // response data parse
            strcpy((char *)ip, resp.param);
            ret = 0;
        }
    }
		printf("%s %s\r\n", __func__, ip);

    return ret;
}

int netm_parse_domain(const char *servername, char *ip)
{
    int        ret = -1;
    netm_msg_t cmd;
    netm_msg_t resp;

		netm_auto_dnsserver();
		netm_query_dnsserver();
		
    memset(&cmd, 0, sizeof(cmd));
    memset(&resp, 0, sizeof(netm_msg_t));

    cmd.cmd     = AT_CMD_DOMAIN;
    cmd.timeout = 5000;

    snprintf(cmd.param, 72, "\"%s\"\r\n", strlwr((char *)servername));
		printf("%s %s\r\n", __func__, cmd.param);
    ret = at_exec_command(&cmd, &resp);

    if (ret == 0) {
        if (resp.result == -1) {
            ret = -1;
            LOGE(TAG, "domain timeout");
        } else if (resp.result == 1) { // response data parse
            strcpy(ip, resp.param);
            ret = 0;

        } else {
            ret = -1;
        }
    }

    return ret;
}

int netm_disconnect(int id)
{
    int        ret = -1;
    netm_msg_t cmd;
    netm_msg_t resp;

    memset(&cmd, 0, sizeof(cmd));
    memset(&resp, 0, sizeof(resp));

    cmd.cmd     = AT_CMD_DISC;
    cmd.timeout = 1000;

    snprintf(cmd.param, 4, "%d\r\n", id);

    ret = at_exec_command(&cmd, &resp);

    return ret;
}

int netm_get_version( )
{
    int        ret = -1;
    netm_msg_t cmd;
    netm_msg_t resp;

    memset(&cmd, 0, sizeof(cmd));
    memset(&resp, 0, sizeof(resp));

    cmd.cmd     = AT_CMD_VERSION;
    cmd.timeout = 2000;

    ret = at_exec_command(&cmd, &resp);

    if (ret == 0) {
        if (resp.result == 1) { // response data parse
            ret = 0;
        }
    }

    return ret;
}

int netm_get_local_ip(char *buf, size_t len)
{
    int        ret = -1;
    netm_msg_t cmd;
    netm_msg_t resp;

    memset(&cmd, 0, sizeof(cmd));
    memset(&resp, 0, sizeof(resp));

    cmd.cmd     = AT_CMD_INFO;
    cmd.timeout = 2000;

    ret = at_exec_command(&cmd, &resp);

    if (ret == 0) {
        if (resp.result == 1) { // response data parse
            strcpy((char *)buf, resp.param);
            ret = 0;
        }
    }

    return ret;
}


int netm_get_link_status(void)
{
    int        ret = -1;
    netm_msg_t cmd;
    netm_msg_t resp;

    memset(&cmd, 0, sizeof(cmd));
    memset(&resp, 0, sizeof(resp));

    cmd.cmd     = AT_CMD_STAT;
    cmd.timeout = 2000;

    ret = at_exec_command(&cmd, &resp);

    if (ret == 0) {
        if (resp.result == 1) { // response data parse
            ret = resp.param[0];
        } else {
            ret = -1;
        }
    }

    return ret;
}

int netm_connect_to_ap(const char *ssid, const char *psw)
{
    /*
    AT+CWJAP_DEF="TP-LINK_ZTW","pass.work"
    OK
    */

    int        ret = -1;
    netm_msg_t cmd;
    netm_msg_t resp;

    memset(&cmd, 0, sizeof(cmd));
    memset(&resp, 0, sizeof(resp));

    cmd.cmd     = AT_CMD_SET_AP;
    cmd.timeout = 30000;

    snprintf(cmd.param, 64, "\"%s\",\"%s\"\r\n", ssid, psw);

    ret = at_exec_command(&cmd, &resp);

    return ret;
}

/***********************
 * channel interface
 ************************/
int at_read_cb_handle(int idx, rdata_cb rcb, linkstat_cb lcb)
{
    g_rdata_handle = rcb;

    if (g_rdata_handle == NULL) {
        LOGE(TAG, "rdata_cb is NULL");
        return -1;
    }

    g_link_status = lcb;

    if (g_link_status == NULL) {
        LOGE(TAG, "linkstat_cb is NULL");
        return -1;
    }

    return 0;
}

static int read_esp8266_one_line(char *line, int nbytes)
{
    char ch       = 0;
    int  line_len = 0;
    char ch_pre   = 0;
    int  data_in_rb=1;
    int  sleep_cnt = 0;

    if (line == NULL || nbytes <= 0) {
        return -1;
    }

    if (ringbuffer_available_read_space(g_netm_dev.rb) <= 0) {
        return -1;
    }

    while (1) {
        if ((data_in_rb = ringbuffer_read(g_netm_dev.rb, (uint8_t *)&ch, 1)) >  0) {
            sleep_cnt = 0;

            if ((ch_pre == '\r') && (ch == '\n')) {
                if (line_len <= 1) {
                    line_len -= 1; // remove continued \r\n
                } else {
                    line_len -= 1; // remove '\r'
                    line[line_len] = '\0';
                    break;
                }
            } else {
                line[line_len++] = ch;
                ch_pre           = ch;

                if (line_len == nbytes) {
                    // dont find '\r\n' in nbytes, maybe data received
                    break;
                }
            }
        } else {
            aos_msleep(10);

            if (sleep_cnt++ >= 2) { // 128 bytes should be received in 20ms
                line[line_len] = '\0';
                break;
            }
        }
    }

    return line_len;
}

#define DEF_DYNAMIC_ALLOCATION	1
#if DEF_DYNAMIC_ALLOCATION
char       *recv_buf=NULL;
#else
#define DEF_IPD_MAX_SIZE	(1024*3)
char       recv_buf[DEF_IPD_MAX_SIZE];
#endif

static int process_at_resp_event(char *line, int line_len,
                                 rdata_cb recv_handle_cb)
{
    int   data_len   = 0;
    int   prefix_len = 0;
    int   remain_len;
    int   data_off  = 0;
    int   id        = -1;
    int   got_frame = 1;
    int   sleep_cnt = 0;
    char *recvdata  = NULL;
	
		int 	available_len=0;

    if (strstr(line, "+IPD")) {
        char *str1 = line;
        char *str2;

        prefix_len += strlen("+IPD,");
        str1 += prefix_len;
        str2 = strtok(str1, ",");
        prefix_len += strlen(str2);
        prefix_len += 1; // ','
        id = atoi(str2);

        if (id >= NETM_SOCKET_NUM) {
            LOGE(TAG, "socket_id %d error", id);
            return -1;
        }

        str2 = strtok(NULL, ":");
        prefix_len += strlen(str2);
        prefix_len += 1; // ':'
        data_len = atoi(str2);

        if ( (data_len == 0) 
				#if !DEF_DYNAMIC_ALLOCATION
				|| (data_len >= DEF_IPD_MAX_SIZE)
				#endif
				) {
            LOGE(TAG, "socket[%d] recv %d data", id, data_len);
            return -1;
        }

        // memcpy data to app_buf
        //printf("id = %d, prefix =%d, data_len =%d\r\n", id, prefix_len, data_len);
        LOGD(TAG, "+IPD,%d,%d:\n", id, data_len);

				#if DEF_DYNAMIC_ALLOCATION
        recvdata = (char *)aos_malloc(data_len + 1);
				if (!recvdata) {
            printf(TAG, "Error: %s %d malloc failure.", __func__, __LINE__);
            return -1;
        }
				#else
        //memset(recv_buf, 0, sizeof(recv_buf));
        recvdata = (char *)&recv_buf; // aos_malloc(data_len + 1);
				#endif

        memset(recvdata, 0, data_len + 1);

        /* write head to socket buf |len|data */
        memcpy(recvdata, (line + prefix_len), line_len - prefix_len);
        data_off = line_len - prefix_len;

        remain_len = data_len - (line_len - prefix_len);

        if ((remain_len > 0) && (line_len != LINE_LEN)) {
            line[0] = '\r';
            line[1] = '\n';

            memcpy(recvdata + data_off, (uint8_t *)line, 2);
            data_off += 2;

            remain_len -= 2;
        }

        while ( remain_len < ringbuffer_available_read_space(g_netm_dev.rb) ) {
			  
				    aos_msleep(10);

            if (sleep_cnt++ > 10) {
                break;
            }
        }
				
				//printf("remain_len=%d (%d)\r\n", remain_len, sleep_cnt );

        // read remained data, try to reduce memory size by reading repeatly
        while (remain_len > 0) {
            int read_len;

            if (remain_len >= LINE_LEN) {
                read_len = LINE_LEN;
            } else {
                read_len = remain_len;
            }

            if (read_len == ringbuffer_read(g_netm_dev.rb, (uint8_t *)line, read_len)) {    
								memcpy(recvdata + data_off, (uint8_t *)line, read_len);
                data_off += read_len;
                remain_len -= read_len;
            } else {
                got_frame = 0;
                LOGE(TAG, "don't got frame !!!!");
                break;
            }
        }

        if (got_frame == 1) {
						if(recv_handle_cb)
							recv_handle_cb(id, recvdata, data_len);
        }
				
				#if DEF_DYNAMIC_ALLOCATION
        aos_free(recvdata);
				#endif
    }

    return 0;
}

static int process_at_resp_cmd(char *line, int line_len)
{
    int ret = -1;
    int end = 0;

    switch (g_cmd_inprogress) {

        case AT_CMD_RST:

            if (strcmp(line, "ready") == 0) {
                set_link_status(NETM_STATUS_READY, 0);
                netm_resp_msg.result = 1;
                end                  = 1;
                LOGI(TAG, "wifi %s", line);
            }

            break;

        case AT_CMD_INFO:
            if (strstr(line, "+CIPSTA:ip:")) {
                char *str1 = line;

                str1 += strlen("+CIPSTA:ip:");

                if (strlen(str1) + 1 <= sizeof(netm_resp_msg.param)) {
                    strcpy(netm_resp_msg.param, str1);
                } else {
                    LOGE(TAG, "%s %d message too long %s\n", __func__, __LINE__,
                         str1);
                    return -1;
                }

                strncpy(net_ipaddr, str1 + 1, (strlen(str1) - 2));
            }

            if (strstr(line, "+CIPSTA:gateway:")) {
                char *str1 = line;

                str1 += strlen("+CIPSTA:gateway:");

                strncpy(net_gw, str1 + 1, (strlen(str1) - 2));
            }

            if (strstr(line, "+CIPSTA:netmask:")) {
                char *str1 = line;

                str1 += strlen("+CIPSTA:netmask:");

                strncpy(net_mask, str1 + 1, (strlen(str1) - 2));

            } else if (strcmp(line, "OK") == 0) {
                netm_resp_msg.result = 1;
                end                  = 1;
            } else if (strcmp(line, "ERROR") == 0) {
                netm_resp_msg.result = 0;
                end                  = 1;
            }

            break;

        case AT_CMD_INFO_MAC:

            if (strstr(line, "+CIPSTAMAC:")) {
                char *str1 = line;

                str1 += strlen("+CIPSTAMAC:");

                if (strlen(str1) + 1 <= sizeof(netm_resp_msg.param)) {
                    strcpy(netm_resp_msg.param, str1);
                } else {
                    LOGE(TAG, "%s %d message too long %s\n", __func__, __LINE__,
                         str1);
                    return -1;
                }
            } else if (strcmp(line, "OK") == 0) {
                netm_resp_msg.result = 1;
                end                  = 1;
            } else if (strcmp(line, "ERROR") == 0) {
                netm_resp_msg.result = 0;
                end                  = 1;
            }

            break;
						
        case AT_CMD_INFO_DNS:

            if (strstr(line, "+CIPDNS_CUR:")) {
                char *str1 = line;

                str1 += strlen("+CIPDNS_CUR:");

                if (strlen(str1) + 1 <= sizeof(netm_resp_msg.param)) {
                    strcpy(netm_resp_msg.param, str1);
                } else {
                    LOGE(TAG, "%s %d message too long %s\n", __func__, __LINE__,
                         str1);
                    return -1;
                }
            } else if (strcmp(line, "OK") == 0) {
                netm_resp_msg.result = 1;
                end                  = 1;
            } else if (strcmp(line, "ERROR") == 0) {
                netm_resp_msg.result = 0;
                end                  = 1;
            }

            break;

        case AT_CMD_INFO_LINK:

            if (strstr(line, "+CWJAP:")) {
                char *str1 = line;

                str1 += strlen("+CWJAP:");

                if (strlen(str1) + 1 <= sizeof(netm_resp_msg.param)) {
                    strcpy(netm_resp_msg.param, str1);
                } else {
                    LOGE(TAG, "%s %d message too long %s\n", __func__, __LINE__,
                         str1);
                    return -1;
                }

            } else if (strcmp(line, "OK") == 0) {
                netm_resp_msg.result = 1;
                end                  = 1;
            } else if (strcmp(line, "ERROR") == 0) {
                netm_resp_msg.result = 0;
                end                  = 1;
            }

            break;

				case AT_CMD_AUTO_DNS:
        case AT_CMD_SET_AP:
            if (strcmp(line, "OK") == 0) {
                netm_resp_msg.result = 1;
                end                  = 1;
            } else if (strcmp(line, "FAIL") == 0) {
                netm_resp_msg.result = 0;
                end                  = 1;
            }

            break;

        case AT_CMD_AP_DISC:

            if (strcmp(line, "OK") == 0) {
                netm_resp_msg.result = 1;

            } else if (strstr(line, "WIFI DISCONNECT")) {
                set_link_status(NETM_STATUS_LINKDOWN, 0);
                end = 1;
            }

            break;

        case AT_CMD_MODE:
            if (strcmp(line, "OK") == 0) {
                netm_resp_msg.result = 1;
                end                  = 1;
            }

            break;

        case AT_CMD_RESTORE:
            if (strcmp(line, "OK") == 0) {
                netm_resp_msg.result = 1;
            } else if (strcmp(line, "ready") == 0) {
                set_link_status(NETM_STATUS_READY, 0);
                end = 1;
            }

            break;

        case AT_CMD_UART_CONFIG:
					
            if (strcmp(line, "OK") == 0) {
                netm_resp_msg.result = 1;
                end                  = 1;
            } else if (strcmp(line, "ERROR") == 0) {
                netm_resp_msg.result = 0;
                end                  = 1;
            }

            break;

        case AT_CMD_NOECHO:

            if (strcmp(line, "OK") == 0) {
                netm_resp_msg.result = 1;
                end                  = 1;
            }

            break;

        case AT_CMD_CONN:

            if (strstr(line, "CONNECT")) {

                LOGD(TAG, "connected\n");
                char *str1             = line;
                netm_resp_msg.param[0] = atoi(str1);

            } else if (strcmp(line, "OK") == 0) {
                netm_resp_msg.result = 1;
                end                  = 1;
            } else if (strcmp(line, "ERROR") == 0) {
                netm_resp_msg.result = 0;
                end                  = 1;
            }

            break;

        case AT_CMD_MUX_SEND:

            if (strstr(line, ">")) {
                if (g_mux_send_frame.len > 0 && g_mux_send_frame.len <= 1460) {
                    ret = netm_uart_write(g_mux_send_frame.data,
                                          g_mux_send_frame.len);
                } else {
                    ret = -1;
                }

            } else if (strstr(line, "SEND OK")) {
                netm_resp_msg.result = g_mux_send_frame.len; // wait sendok
                end                  = 1;
            } else if (strstr(line, "SEND FAIL")) {
                netm_resp_msg.result = 0; // send fail
                end                  = 1;
            } else if (strstr(line, "link is not valid")) {
                netm_resp_msg.result = 0; // send fail
            } else if (strcmp(line, "ERROR") == 0) {
                netm_resp_msg.result = 0;
                end                  = 1;
            }

            break;

        case AT_CMD_DISC:

            if (strstr(line, "CLOSED")) {
                char *str1 = line;
                char *str2;

                str2                   = strtok(str1, ",");
                netm_resp_msg.param[0] = atoi(str2); // id
            } else if (strcmp(line, "OK") == 0) {
                netm_resp_msg.result = 1;
                end                  = 1;
            } else if (strcmp(line, "ERROR") == 0) {
                netm_resp_msg.result = 1;
                end                  = 1;
            }

            break;

        case AT_CMD_MUX_MODE:

            if (strcmp(line, "OK") == 0) {
                netm_resp_msg.result = 1;
                end                  = 1;
            } else if (strcmp(line, "ERROR") == 0) {
                netm_resp_msg.result = 0;
                end                  = 1;
            }

            break;

        case AT_CMD_STAT:

            if (strcmp(line, "OK") == 0) {
                netm_resp_msg.result = 1;
                end                  = 1;
            } else if (strcmp(line, "ERROR") == 0) {
                netm_resp_msg.result = 0;
                end                  = 1;
            } else if (strstr(line, "STATUS:")) {
                char *str1 = line;
                int   status;

                str1 += strlen("STATUS:");
                status = atoi(str1);

                if (status == 2) {
                    netm_resp_msg.param[0] = NETM_STATUS_GOTIP;
                    set_link_status(NETM_STATUS_GOTIP, 0);
                } else if (status == 3) {
                    netm_resp_msg.param[0] = NETM_STATUS_SOCKET_CONNECTED;
                    set_link_status(NETM_STATUS_SOCKET_CONNECTED, 0);
                } else if (status == 4) {
                    netm_resp_msg.param[0] = NETM_STATUS_SOCKET_DISCONNECT;
                    set_link_status(NETM_STATUS_SOCKET_DISCONNECT, 0);
                } else if (status == 5) {
                    netm_resp_msg.param[0] = NETM_STATUS_LINKDOWN;
                    set_link_status(NETM_STATUS_LINKDOWN, 0);
                }
            }

            break;

        case AT_CMD_DOMAIN:

            if (strcmp(line, "OK") == 0) {
                end = 1;

            } else if (strcmp(line, "ERROR") == 0) {
                netm_resp_msg.result = 0;
                end                  = 1;
            } else if (strstr(line, "+CIPDOMAIN:")) {

                char *str1 = line;

                str1 += strlen("+CIPDOMAIN:");

                memset(netm_resp_msg.param, 0, sizeof(netm_resp_msg.param));
                if (strlen(str1) + 1 <= sizeof(netm_resp_msg.param)) {
                    strcpy(netm_resp_msg.param, str1);
                } else {
                    LOGE(TAG, "%s %d message too long %s\n", __func__, __LINE__,
                         str1);
                    return -1;
                }
                netm_resp_msg.result = 1;
            }

            break;
						
				case AT_CMD_VERSION:
				
            if (strcmp(line, "OK") == 0) {
                end = 1;

            } else if (strcmp(line, "ERROR") == 0) {
                netm_resp_msg.result = 0;
                end                  = 1;
							
            } else  {
							
								printf("%s\r\n", line);
                netm_resp_msg.result = 1;
            }

            break;
				
        case AT_CMD_SHUT:

            if (strstr(line, "SHUT OK")) {
                netm_resp_msg.result = 1;
                end                  = 1;
            } else if (strcmp(line, "ERROR") == 0) {
                netm_resp_msg.result = 0;
                end                  = 1;
            }

            break;

        case AT_CMD_PING:

            if ((line[0] == '+') && (strlen(line) >= 2)) {
                netm_resp_msg.param[0] = atoi(&line[1]);

            } else if (strcmp(line, "OK") == 0) {
                netm_resp_msg.result = 1;
                end                  = 1;
            } else if (strcmp(line, "ERROR") == 0) {
                netm_resp_msg.result = 0;
                end                  = 1;
            }

            break;

        case AT_CMD_RAW:

            if (strcmp(line, "OK") == 0) {
                netm_resp_msg.result = 1;
                end                  = 1;
            } else if (strcmp(line, "ERROR") == 0) {
                netm_resp_msg.result = 0;
                end                  = 1;
            } else {
                if (strlen(line) + 1 <= sizeof(netm_resp_msg.param)) {
                    strcpy(netm_resp_msg.param, line);
                } else {
                    LOGE(TAG, "%s %d message too long %s\n", __func__, __LINE__,
                         line);
                    return -1;
                }
            }

            break;

        default:
            break;
    }

    if (strstr(line, "WIFI DISCONNECT") != NULL) {
        set_link_status(NETM_STATUS_LINKDOWN, 0);
    }

    if (strstr(line, "WIFI CONNECTED") != NULL) {
        set_link_status(NETM_STATUS_LINKUP, 0);
    }

    if (strstr(line, "WIFI GOT IP") != NULL) {
        set_link_status(NETM_STATUS_GOTIP, 0);
    }

    if (strstr(line, "Smart get wifi info") != NULL) {
        set_link_status(NETM_STATUS_CONFIGING, 0);
    }

    if (strstr(line, "smartconfig connected wifi") != NULL) {
        set_link_status(NETM_STATUS_GOTIP, 0);
    }

    if ((end != 1) && strcmp(line, "ready") == 0) {
        set_link_status(NETM_STATUS_READY, 0);
        LOGI(TAG, "%s", line);
    }

    if ((g_netm_status > NETM_STATUS_LINKUP) && (end != 1) &&
        (strstr(line, "CLOSED") != NULL)) {
        char *str1 = line;
        char *str2;

        str2 = strtok(str1, ",");
        LOGI(TAG, "server closed socket[%d]", atoi(str2));
    }

    if (end) {
			
				krhino_timer_stop(&timer_atmsg);
        g_cmd_inprogress = 0;
        ret = krhino_buf_queue_send(&g_netm2app_queue, (void *)&netm_resp_msg, sizeof(netm_msg_t) );
        if ( ret != 0 )
        {
            LOGE(TAG, "driver msgq_put error");
            return ret;
        }
    }

    return 0;
}

static at_resp_cmd_t g_at_cmd[] = {
    { AT_CMD_NOECHO, "ATE0\r\n", "OK" },
    { AT_CMD_RST, "AT+RST\r\n", "ready" },
    { AT_CMD_VERSION, "AT+GMR\r\n", "OK" },
    { AT_CMD_RESTORE, "AT+RESTORE\r\n", "OK" },
    { AT_CMD_STAT, "AT+CIPSTATUS\r\n", "+CIPSTATUS:" },
    { AT_CMD_INFO_LINK, "AT+CWJAP?\r\n", "+CWJAP:" },
    { AT_CMD_INFO_MAC, "AT+CIPSTAMAC?\r\n", "+CIPSTAMAC:" },
		{ AT_CMD_INFO_DNS, "AT+CIPDNS_CUR?\r\n", "+CIPDNS_CUR:" },
		{ AT_CMD_AUTO_DNS, "AT+CIPDNS_DEF=0\r\n", "OK" },
    { AT_CMD_INFO, "AT+CIPSTA?\r\n", "+CIPSTA:" },
    { AT_CMD_SET_AP, "AT+CWJAP_DEF=", "+CWJAP_DEF:" },
    { AT_CMD_MUX_MODE, "AT+CIPMUX=", "+CIPMUX:" },
    { AT_CMD_MODE, "AT+CWMODE_DEF=", "OK" },
    { AT_CMD_CONN, "AT+CIPSTART=", "OK" },
    { AT_CMD_MUX_SEND, "AT+CIPSEND=", "OK" },
    { AT_CMD_DISC, "AT+CIPCLOSE=", "+OK" },
    { AT_CMD_DOMAIN, "AT+CIPDOMAIN=", "+CIPDOMAIN:" },
    { AT_CMD_PING, "AT+PING=", "+" },
    { AT_CMD_UART_CONFIG, "AT+UART_DEF=", "OK" },
};

#define MAX_AT_CMD_LEN (128)
static char g_send_cmd[MAX_AT_CMD_LEN];

static int handle_at_cmd(netm_msg_t *msg)
{
    int i;
    int cmd_len;

    memset(g_send_cmd, 0, MAX_AT_CMD_LEN);

    if (g_cmd_inprogress) {
        LOGE(TAG, "pre cmd=%d is in progress!", g_cmd_inprogress);
        return -1;
    }

    g_cmd_inprogress = msg->cmd;

    for (i = 0; i < ARRAY_SIZE(g_at_cmd); i++) {
        if (msg->cmd == g_at_cmd[i].type) {
            break;
        }
    }

    if (i == ARRAY_SIZE(g_at_cmd)) {
        LOGE(TAG, "msg->cmd %d not support!", msg->cmd);

        krhino_timer_stop(&timer_atmsg);
				krhino_timer_change(&timer_atmsg, krhino_ms_to_ticks(msg->timeout), 0 );
        krhino_timer_start(&timer_atmsg);
			
        return -1;
    }

    cmd_len = strlen(g_at_cmd[i].cmd_name);

    strncpy(g_send_cmd, g_at_cmd[i].cmd_name, cmd_len);

    /* paramter passed in msg */
    if (g_at_cmd[i].cmd_name[cmd_len - 2] != '\r') {

        /* str mode */
        if (msg->param_len == 0) {
            strncpy(g_send_cmd + cmd_len, msg->param, strlen(msg->param));
            cmd_len += strlen(msg->param);
        } else {
            memcpy(g_send_cmd + cmd_len, msg->param, msg->param_len);
            cmd_len += msg->param_len;

            memcpy(&g_mux_send_frame, msg->param + msg->param_len,
                   sizeof(mux_send_param_t));
        }
    }

    LOGD(TAG, "write_cmd: %s", g_send_cmd);

    if (netm_uart_write(g_send_cmd, cmd_len) > 0) {
					
			krhino_timer_stop(&timer_atmsg);
			krhino_timer_change(&timer_atmsg, krhino_ms_to_ticks(msg->timeout), 0 );
			
			if ( krhino_timer_start(&timer_atmsg) != RHINO_SUCCESS) 
				LOGE(TAG, "start timer failure");

        return 0;

    } else {

        LOGE(TAG, "uart write timeout");
        g_cmd_inprogress     = 0;
        netm_resp_msg.result = -1;
				krhino_buf_queue_send(&g_netm2app_queue, (void *)&netm_resp_msg, sizeof(netm_msg_t));
    
    }

    LOGE(TAG, "handle_at_cmd error!");

    return -1;
}

static int handle_at_response(netm_msg_t *msg)
{
    char line[LINE_LEN];
    int  line_len;

    memset(line, 0, LINE_LEN);

    while ((line_len = read_esp8266_one_line(line, sizeof(line))) > 0) {
				atresp_dump_buf(line, line_len, 0 );

        if (strstr(line, "+IPD") ) {
            process_at_resp_event(line, line_len, g_rdata_handle);
        } else {
            LOGD(TAG, "read lines {%s}\n", line);
            process_at_resp_cmd(line, line_len);
        }

        memset(line, 0, LINE_LEN);
    }

    return 0;
}


static int handle_at_readdata (void)
{
    netm_dev_t    *udev   = &g_netm_dev;
    uart_dev_t*		 handle = udev->hal_uart_handle;
		#define DEF_HANDLE_READ_DATA_SIZE		128
  	uint8_t data[DEF_HANDLE_READ_DATA_SIZE];
    int32_t ret=0;
		int32_t count=0;

		memset(data, 0, DEF_HANDLE_READ_DATA_SIZE);

    if (udev->rb == NULL) {
        return 0;
    }

    /* flow ctrl */
    if (udev->flowctrl) {
        //ret = ringbuffer_available_write_space(udev->rb);
        //if (ret <= 64) {
        //    csi_usart_set_interrupt(handle, USART_INTR_READ, 0);
        //    break;
        //}
    }
		
		while ( 1 )
		{
			hal_uart_recv_II( handle, data, DEF_HANDLE_READ_DATA_SIZE, &ret, 10 );
			if (  ret > 0 )
			{
					atresp_dump_buf(data, ret, 1 );
					//FIXME: Maybe drop data here.
					ringbuffer_write(udev->rb, (uint8_t *)data, ret);
			} else
				break;
			count += ret;
		}
		
		//printf("[%s] received: %d\r\n", __func__, count);
		
		
		return count;
}

static void netm_task(void *arg)
{
    netm_msg_t msgbuf;
    int        ret = 0;
		int				 received_msgsize=0;
	
    while (1) {

        memset(&msgbuf, 0, sizeof(msgbuf));
        ret = krhino_buf_queue_recv(&g_netm_queue, AOS_WAIT_FOREVER, (void *)&msgbuf, &received_msgsize );
        if ( ret !=  RHINO_SUCCESS ) {
            LOGE(TAG, "recv msg fail,error:%x", ret);
            continue;
        }

        switch (msgbuf.msg_src) {
            case SRC_APP:
                handle_at_response(&msgbuf);
                handle_at_cmd(&msgbuf);
                break;

            case SRC_DRIVER:
								if ( handle_at_readdata () > 0 )
									handle_at_response(&msgbuf);
                break;

            default:
                LOGE(TAG, "error msg type");
                break;
        }
    } /* while */
}
extern void esp8266_uart_config(void);

int netm_init(void)
{
    kstat_t kret;
    int ret = netm_uart_init();
    if (ret != 0)
        return ret;
 
    //Wayne esp8266_uart_config();

/**
 * This function will create a mutex
 * @param[in] mutex  pointer to the mutex(the space is provided by user)
 * @param[in] name   name of the mutex
 * @return the operation status, RHINO_SUCCESS is OK, others is error
 */
	if ( (kret=krhino_mutex_create(&g_cmd_mutex, "g_cmd_mutex")) != RHINO_SUCCESS )
    {
		LOGE(TAG, "Creating cmd mutex failed (%s %d %x).", __func__, __LINE__, kret);
		goto err;
	}

/**
 * This function will create a buf-queue
 * @param[in]  queue    pointer to the queue(the space is provided by user)
 * @param[in]  name     name of the queue
 * @param[in]  buf      pointer to the buf
 * @param[in]  size     size of the buf
 * @param[in]  max_msg  max size of one msg
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 * kstat_t krhino_buf_queue_create(kbuf_queue_t *queue, const name_t *name,
 *                               void *buf,
 *                               size_t size, size_t max_msg);
 */                         	
	if ( (kret=krhino_buf_queue_create( &g_netm_queue, "g_netm_queue", &g_u8MsgQNetmBuf[0], sizeof(g_u8MsgQNetmBuf), sizeof(netm_msg_t)) ) != RHINO_SUCCESS )
    {
		LOGE(TAG, "Creating netm queue failed (%s %d).", __func__, __LINE__);
		goto err;
	}

	if ( (kret=krhino_buf_queue_create( &g_netm2app_queue, "g_netm2app_queue", &g_u8MsgQNetm2AppBuf[0], sizeof(g_u8MsgQNetm2AppBuf), sizeof(netm_msg_t) )) != RHINO_SUCCESS )
    {
		LOGE(TAG, "Creating netm2app queue failed (%s %d).", __func__, __LINE__);
		goto err;
	}

/**
 * This function will create a timer
 * @param[in]  timer     pointer to the timer(the space is provided by user)
 * @param[in]  name      name of the timer
 * @param[in]  cb        callbak of the timer
 * @param[in]  first     ticks of the first timer triger
 * @param[in]  round     ticks of the normal timer triger
 * @param[in]  arg       the argument of the callback
 * @param[in]  auto_run  auto run or not when the timer is created
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 * kstat_t krhino_timer_create(ktimer_t *timer, const name_t *name, timer_cb_t cb,
 *                           sys_time_t first, sys_time_t round, void *arg, uint8_t auto_run);
 */
    kret = krhino_timer_create(&timer_atmsg, "timer_atmsg", at_send_timeout_callback, 1, RHINO_CONFIG_TICKS_PER_SECOND, NULL, 0);
    if( kret != RHINO_SUCCESS)
    {
        LOGE(TAG, "Creating timer failed (%s %d).", __func__, __LINE__);
				goto err;
		}

    LOGD(TAG, "start at_esp8266 task");
    
#ifndef AOS_NO_WIFI

/**
 * This function will initialize a task
 * @param[in]  task       the task to be created
 * @param[in]  name       the name of task, which shall be unique
 * @param[in]  arg        the parameter of task enter function
 * @param[in]  pri        the prio of task
 * @param[in]  ticks      the time slice if there are same prio task
 * @param[in]  stack_buf  the start address of task stack
 * @param[in]  stack      the size of thread stack
 * @param[in]  entry      the entry function of task
 * @param[in]  autorun    the autorunning flag of task
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 * 
 * kstat_t krhino_task_create(ktask_t *task, const name_t *name, void *arg,
 *                          uint8_t prio, tick_t ticks, cpu_stack_t *stack_buf,
 *                          size_t stack_size, task_entry_t entry, uint8_t autorun);
 */

    kret = krhino_task_create(&tid, "netm_task", 0, AOS_DEFAULT_APP_PRI, 0,
								netm_task_buf, DEF_NETM_TASK_STACK_SIZE, netm_task, 1);
    if( kret != RHINO_SUCCESS)
    {
        LOGE(TAG, "Creating netm task failed (%s %d).", __func__, __LINE__);
			goto err;
		}
#endif

    return 0;

err:

/**
 * This function will delete a timer
 * @param[in]  timer  pointer to a timer
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
	kret = krhino_timer_del(&timer_atmsg);
    if( kret != RHINO_SUCCESS)
        LOGE(TAG, "Delete timer_atmsg failed (%s %d).", __func__, __LINE__);

/**
 * This function will delete a queue
 * @param[in]  queue  pointer to the queue
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */	
	kret = krhino_buf_queue_del(&g_netm_queue);
    if( kret != RHINO_SUCCESS)
		LOGE(TAG, "Delete g_netm_queue failed (%s %d).", __func__, __LINE__);

	kret = krhino_buf_queue_del(&g_netm2app_queue);
    if( kret != RHINO_SUCCESS)
		LOGE(TAG, "Delete g_netm2app_queue failed (%s %d).", __func__, __LINE__);

/**
 * This function will delete a mutex
 * @param[in] mutex pointer to the mutex
 * @return the operation status, RHINO_SUCCESS is OK, others is error
 */
	kret = krhino_mutex_del(&g_cmd_mutex);
    if( kret != RHINO_SUCCESS)
			LOGE(TAG, "Delete g_cmd_mutex failed (%s %d).", __func__, __LINE__);

    return -1;
}
