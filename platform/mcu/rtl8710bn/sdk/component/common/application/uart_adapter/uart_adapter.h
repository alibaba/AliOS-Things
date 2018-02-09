#include "osdep_service.h"
#include "serial_api.h"
#include <timer_api.h>
#include "freertos_pmu.h"
#include <mDNS/mDNS.h>
#include "gpio_api.h"
#include "gpio_irq_api.h"
#include "PinNames.h"
/******************************************************
 *                    Macros
 ******************************************************/
#define	UA_ERROR		0
#define	UA_WARNING		1
#define	UA_INFO			2
#define	UA_DEBUG		3
#define	UA_NONE			0xFF
#define	UA_DEBUG_LEVEL	UA_INFO

#define	UA_UART_THREAD_PRIORITY		5
#define	UA_UART_THREAD_STACKSIZE	512

#define	UA_TCP_SERVER_FD_NUM		1
#define	UA_TCP_CLIENT_FD_NUM		1

#define	UA_UART_RECV_BUFFER_LEN		8196
#define	UA_UART_FRAME_LEN			1400
#define	UA_UART_MAX_DELAY_TIME		100

#define	UA_CHAT_SOCKET_PORT			5001
#define	UA_CONTROL_SOCKET_PORT		6001

#define	UA_SC_SOFTAP_EN			1

#ifdef CONFIG_PLATFORM_8195A
#define	UA_UART_TX_PIN			PA_7
#define	UA_UART_RX_PIN			PA_6

#define	UA_GPIO_LED_PIN			PC_5
#define	UA_GPIO_IRQ_PIN			PC_4

#define	UA_GPIO_TIMER			TIMER0
#define	UA_GPIO_WAKEUP_PIN		PC_3
#endif
#ifdef CONFIG_PLATFORM_8711B
#define	UA_UART_TX_PIN		PA_23
#define	UA_UART_RX_PIN		PA_18

#define	UA_GPIO_LED_PIN        	PA_5
#define	UA_GPIO_IRQ_PIN        	PA_12

#define	UA_GPIO_TIMER 		TIMER2
#define	UA_GPIO_WAKEUP_PIN	PA_0
#endif

#define	UA_CONTROL_PREFIX     "AMEBA_UART"

#define	UA_PS_ENABLE                 0

#define	UA_WAKELOCK                 PMU_DEV_USER_BASE

#define	UA_FAST_RECONNECT_TCP_DATA (0xF5000 + 0x1000)


#if (UA_DEBUG_LEVEL== UA_NONE)
#define ua_printf(level, fmt, arg...)
#else
#define ua_printf(level, fmt, arg...)     \
do {\
	if (level <= UA_DEBUG_LEVEL) {\
		if (level <= UA_ERROR) {\
			rtw_down_sema(&ua_print_sema);\
			printf("\r\nERROR: " fmt, ##arg);\
			rtw_up_sema(&ua_print_sema);\
		} \
		else {\
			rtw_down_sema(&ua_print_sema);\
			printf("\r\n"fmt, ##arg);\
			rtw_up_sema(&ua_print_sema);\
		} \
	}\
}while(0)
#endif

#define UA_PRINT_DATA(_HexData, _HexDataLen)			\
			if(UA_DEBUG_LEVEL == UA_DEBUG)	\
			{									\
				int __i;								\
				u8	*ptr = (u8 *)_HexData;				\
				printf("--------Len=%d\n\r", _HexDataLen);						\
				for( __i=0; __i<(int)_HexDataLen; __i++ )				\
				{								\
					printf("%02X%s", ptr[__i], (((__i + 1) % 4) == 0)?"  ":" ");	\
					if (((__i + 1) % 16) == 0)	printf("\n\r");			\
				}								\
				printf("\n\r");							\
			}

#define UA_SOCKET_CHECK(_ua_socket)			\
			if(_ua_socket == NULL)	\
			{									\
				printf("ERROR: ua_socket = NULL\n\r");							\
					return;      \
			}

#define UA_SOCKET_CHECK_2(_ua_socket)			\
			if(_ua_socket == NULL)	\
			{									\
				printf("ERROR: ua_socket = NULL\n\r");							\
					return -1;      \
			}			

/******************************************************
 *                    Constants
 ******************************************************/
typedef enum
{
	UART_ADAPTER_LED_ON = 0,
	UART_ADAPTER_LED_OFF = 1,
	UART_ADAPTER_LED_FAST_TWINKLE = 2,
	UART_ADAPTER_LED_SLOW_TWINKLE = 3,
}ua_led_mode_t;

typedef enum
{
	UART_CTRL_MODE_SET_REQ = 0,
	UART_CTRL_MODE_SET_RSP = 1,
	UART_CTRL_MODE_GET_REQ = 2,
	UART_CTRL_MODE_GET_RSP = 3,
}ua_ctrl_mode_t;

typedef enum
{
	UART_CTRL_TYPE_BAUD_RATE = 0x01,
	UART_CTRL_TYPE_WORD_LEN = 0x02,
	UART_CTRL_TYPE_PARITY = 0x04,
	UART_CTRL_TYPE_STOP_BIT = 0x08,
	UART_CTRL_TYPE_TCP_SERVER_CREATE = 0x10,
	UART_CTRL_TYPE_TCP_SERVER_DELETE = 0x20,
	UART_CTRL_TYPE_TCP_CLIENT_CONNECT = 0x40,
	UART_CTRL_TYPE_TCP_CLIENT_DISCONNECT = 0x80,
	UART_CTRL_TYPE_TCP_GROUP_ID = 0x100,	
}ua_ctrl_type_t;

/******************************************************
 *                   Structures
 ******************************************************/
 typedef struct _ua_uart_param_t
{
    u8 WordLen;
    u8 Parity;
    u8 StopBit;
    u8 FlowControl;	
    int BaudRate;	
}ua_uart_param_t;

typedef struct _ua_uart_socket_t
{
	int 			fd;
	char	 		rcv_ch;
	volatile char 	overlap;
	int 			recv_bytes;
	volatile int 	pread;
	volatile int 	pwrite;
	
	volatile unsigned int 	tick_last_update;
	unsigned int 			tick_current;

	volatile int tx_busy;	

	volatile int uart_ps;
	volatile int uart_ps_cnt;
	
	char recv_buf[UA_UART_RECV_BUFFER_LEN];

	long rx_cnt;
	long miss_cnt;	

	serial_t uart_sobj;	
	ua_uart_param_t uart_param;
	
	_sema action_sema;	
	_sema tcp_tx_rx_sema;
	_sema dma_tx;	
}ua_uart_socket_t;

typedef struct _ua_tcp_socket_t
{	
	int chat_socket;  
	int control_socket;  
	int chat_server_listen_socket;  
	int control_server_listen_socket;  	

	int transmit_recv_socket;  
	int transmit_send_socket;  
	int transmit_server_listen_socket;  		

	int group_id;
	u32 server_port;
	u32 client_port;
	char client_ip[16];


	int send_flag;
	int recv_flag;
	long rx_cnt;	
	long tx_cnt;	

	volatile int tcp_ps;	
	volatile int tcp_ps_cnt;			
}ua_tcp_socket_t;

typedef struct _ua_gpio_t
{	
	gpio_t 		gpio_led;
	gpio_t 		gpio_btn;	
	gpio_irq_t 	gpio_btn_irq;
	gtimer_t 		gpio_timer;
}ua_gpio_t;

typedef struct _ua_socket_t
{
	ua_uart_socket_t uart;
	ua_tcp_socket_t tcp;
	ua_gpio_t gpio;	
	ip_addr_t ip;
	DNSServiceRef dnsServiceRef;
	DNSServiceRef dnsServiceRef2;	
}ua_socket_t;

typedef struct _ua_mbox_buffer
{
	char data[UA_UART_FRAME_LEN];
	int data_len;
}ua_mbox_buffer_t;

//Save Uart Settings when get uart information
typedef struct _ua_uart_get_str 
{ 
	int BaudRate;    //The baud rate 
	char number;     //The number of data bits 
	char parity;     //The parity(0: none, 1:odd, 2:evn, default:0)
	char StopBits;      //The number of stop bits 
	char FlowControl;    //support flow control is 1 
}ua_uart_get_str;

//Uart Setting information 
typedef struct _ua_uart_set_str 
{ 
	char UartName[8];    // the name of uart 
	int BaudRate;    //The baud rate 
	char number;     //The number of data bits 
	char parity;     //The parity(default NONE) 
	char StopBits;      //The number of stop bits 
	char FlowControl;    //support flow control is 1 
}ua_uart_set_str;


int uartadapter_init();
void uartadapter_tcp_send_data(ua_socket_t *ua_socket, char *buffer, int size);
void uartadapter_tcp_send_control(ua_socket_t *ua_socket, char *buffer, int size);
void uartadapter_tcp_transmit_server_thread(void *param);
void uartadapter_tcp_transmit_client_thread(void *param);
int uartadapter_tcpclient(ua_socket_t *ua_socket, const char *host_ip, unsigned short usPort);
void uartadapter_tcp_transmit_client_forever_thread(void *param);


void example_uart_adapter_init();
void cmd_uart_adapter(int argc, char **argv);

void uartadapter_tcp_transmit_socket_handler(ua_socket_t *ua_socket, char *tcp_rxbuf);

