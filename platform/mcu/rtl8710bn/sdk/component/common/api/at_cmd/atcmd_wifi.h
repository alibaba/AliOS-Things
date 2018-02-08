#ifndef __ATCMD_WIFI_H__
#define __ATCMD_WIFI_H__
#include "main.h"
#include "lwip_netconf.h"

#ifndef WLAN0_NAME
  #define WLAN0_NAME		"wlan0"
#endif
#ifndef WLAN1_NAME
  #define WLAN1_NAME		"wlan1"
#endif
/* Give default value if not defined */
#ifndef NET_IF_NUM
  #ifdef CONFIG_CONCURRENT_MODE
    #define NET_IF_NUM ((CONFIG_ETHERNET) + (CONFIG_WLAN) + 1)
  #else
    #define NET_IF_NUM ((CONFIG_ETHERNET) + (CONFIG_WLAN))
  #endif  // end of CONFIG_CONCURRENT_MODE
#endif  // end of NET_IF_NUM

/*Static IP ADDRESS*/
#ifndef IP_ADDR0
#define IP_ADDR0   192
#define IP_ADDR1   168
#define IP_ADDR2   1
#define IP_ADDR3   80
#endif

/*NETMASK*/
#ifndef NETMASK_ADDR0
#define NETMASK_ADDR0   255
#define NETMASK_ADDR1   255
#define NETMASK_ADDR2   255
#define NETMASK_ADDR3   0
#endif

/*Gateway Address*/
#ifndef GW_ADDR0
#define GW_ADDR0   192
#define GW_ADDR1   168
#define GW_ADDR2   1
#define GW_ADDR3   1
#endif

/*Static IP ADDRESS*/
#ifndef AP_IP_ADDR0
#define AP_IP_ADDR0   192
#define AP_IP_ADDR1   168
#define AP_IP_ADDR2   43
#define AP_IP_ADDR3   1
#endif
   
/*NETMASK*/
#ifndef AP_NETMASK_ADDR0
#define AP_NETMASK_ADDR0   255
#define AP_NETMASK_ADDR1   255
#define AP_NETMASK_ADDR2   255
#define AP_NETMASK_ADDR3   0
#endif

/*Gateway Address*/
#ifndef AP_GW_ADDR0
#define AP_GW_ADDR0   192
#define AP_GW_ADDR1   168
#define AP_GW_ADDR2   43
#define AP_GW_ADDR3   1  
#endif

#if CONFIG_EXAMPLE_UART_ATCMD
#include "wifi_structures.h"
#include <wlan_fast_connect/example_wlan_fast_connect.h>
typedef struct _UART_LOG_CONF_{
	u32 BaudRate;
	u8 DataBits;
	u8 StopBits;
	u8 Parity;
	u8 FlowControl;
}UART_LOG_CONF, *PUART_LOG_CONF;

#define ATCMD_WIFI_CONN_STORE_MAX_NUM (1)
struct atcmd_wifi_conf{
	int32_t auto_enable;
	rtw_wifi_setting_t setting;
	int32_t reconn_num;
	int32_t reconn_last_index;	
	struct wlan_fast_reconnect reconn[ATCMD_WIFI_CONN_STORE_MAX_NUM];
};

#define ATCMD_LWIP_CONN_STORE_MAX_NUM (1)
struct atcmd_lwip_conn_info{
	int32_t role; //client, server or seed
	uint32_t protocol; //tcp or udp
	uint32_t remote_addr; //remote ip
	uint32_t remote_port; //remote port
	uint32_t local_addr; //locale ip, not used yet
	uint32_t local_port; //locale port, not used yet
	uint32_t reserved; //reserve for further use
};
struct atcmd_lwip_conf {
	int32_t enable; //enable or not
	int32_t conn_num;
	int32_t last_index;
	int32_t reserved; //reserve for further use
	struct atcmd_lwip_conn_info conn[ATCMD_LWIP_CONN_STORE_MAX_NUM];
};

typedef enum {
	AT_PARTITION_ALL = 0,
	AT_PARTITION_UART = 1,
	AT_PARTITION_WIFI = 2,
	AT_PARTITION_LWIP = 3
} AT_PARTITION;

typedef enum {
	AT_PARTITION_READ = 0,
	AT_PARTITION_WRITE = 1,
	AT_PARTITION_ERASE = 2
} AT_PARTITION_OP;

//first segment for uart
#define UART_SETTING_BACKUP_SECTOR		(0x8000)
#define UART_CONF_DATA_OFFSET			(0)
#define UART_CONF_DATA_SIZE				((((sizeof(UART_LOG_CONF)-1)>>2) + 1)<<2)

//second segment for wifi config
#define WIFI_CONF_DATA_OFFSET			(UART_CONF_DATA_OFFSET+UART_CONF_DATA_SIZE)
#define WIFI_CONF_DATA_SIZE				((((sizeof(struct atcmd_wifi_conf)-1)>>2) + 1)<<2)

//fouth segment for lwip config
#define LWIP_CONF_DATA_OFFSET			(WIFI_CONF_DATA_OFFSET+WIFI_CONF_DATA_SIZE)
#define LWIP_CONF_DATA_SIZE				((((sizeof(struct atcmd_lwip_conf)-1)>>2) + 1)<<2)

extern void atcmd_update_partition_info(AT_PARTITION id, AT_PARTITION_OP ops, u8 *data, u16 len);

#define ATSTRING_LEN 	(LOG_SERVICE_BUFLEN)
extern char at_string[ATSTRING_LEN];

extern unsigned char gAT_Echo; // default echo on
//extern void uart_at_lock(void);
//extern void uart_at_unlock(void);
extern void uart_at_send_string(char *str);
extern void uart_at_send_buf(u8 *buf, u32 len);

#define at_printf(fmt, args...)  do{\
			/*uart_at_lock();*/\
			snprintf(at_string, ATSTRING_LEN, fmt, ##args); \
			uart_at_send_string(at_string);\
			/*uart_at_unlock();*/\
	}while(0)
#define at_print_data(data, size)  do{\
			/*uart_at_lock();*/\
			uart_at_send_buf(data, size);\
			/*uart_at_unlock();*/\
	}while(0)

#else
#define at_printf(fmt, args...) do{printf(fmt, ##args);}while(0)
#define at_print_data(data, size) do{__rtl_memDump(data, size, NULL);}while(0)
#endif//#if CONFIG_EXAMPLE_UART_ATCMD

#endif
