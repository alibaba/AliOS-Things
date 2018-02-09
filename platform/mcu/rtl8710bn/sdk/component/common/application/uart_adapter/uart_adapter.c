#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include <lwip/sockets.h>
#include <lwip_netconf.h>
#include <dhcp/dhcps.h>
#include "serial_api.h"
#include "serial_ex_api.h"
#include "uart_adapter.h"
#include "wifi_conf.h"
#include "gpio_api.h"   // mbed
#include "gpio_irq_api.h"   // mbed
#include "osdep_service.h"
#include "flash_api.h"
#include "device_lock.h"
#include <mDNS/mDNS.h>
#include <example_wlan_fast_connect.h>
#include <timer_api.h>
#include "wifi_simple_config.h"

/***********************************************************************
 *                                                            Macros                                                                    *
 ***********************************************************************/

/***********************************************************************
 *                                                Variables Declarations                                                           *      
 ***********************************************************************/
char ua_tcp_server_ip[16];

_sema ua_exception_sema;
_sema ua_print_sema;

int ua_started = 0;
int ua_gpio_irq_happen = 0;
int ua_debug_print_en = 0;
int ua_wifi_connected = 0;
int ua_reconnect_started = 0;
int ua_reconnect_ip_change = 0;

ua_socket_t *ua_global_socket = NULL;
gpio_irq_t gpio_rx_wake;

/************************************************************************
 *                                                  extern variables                                                                       *
 ************************************************************************/
extern struct netif xnetif[NET_IF_NUM];

extern unsigned char psk_essid[NET_IF_NUM][NDIS_802_11_LENGTH_SSID+4];
extern unsigned char psk_passphrase[NET_IF_NUM][IW_PASSPHRASE_MAX_SIZE + 1];
extern unsigned char wpa_global_PSK[NET_IF_NUM][A_SHA_DIGEST_LEN * 2];

extern wlan_init_done_ptr p_wlan_uart_adapter_callback;

extern char simple_config_terminate;
/************************************************************************
 *                                                  extern funtions                                                                       *
 ************************************************************************/
#if CONFIG_INCLUDE_SIMPLE_CONFIG
extern enum sc_result simple_config_test(rtw_network_info_t *);
extern int init_test_data(char *custom_pin_code);
extern void deinit_test_data(void);
extern void filter_add_enable();
extern void remove_filter();
extern void wifi_enter_promisc_mode();
#endif

/*************************************************************************
*                                                   uart releated                                                                         *
*************************************************************************/			
#define ____________UART__RELATED____________________
static void uartadapter_uart_irq(uint32_t id, SerialIrq event)
{
	ua_socket_t *ua_socket = (ua_socket_t *)id;

	if(event == RxIrq) {
		ua_socket->uart.recv_buf[ua_socket->uart.pwrite++] = serial_getc(&ua_socket->uart.uart_sobj);
		rtw_up_sema_from_isr(&ua_socket->uart.action_sema);	//up action semaphore 
		
		if(ua_socket->uart.pwrite > (UA_UART_RECV_BUFFER_LEN -1)){	//restart from  head if  reach tail
			ua_socket->uart.pwrite = 0;
			ua_socket->uart.overlap = 1;		
		}
		
		if(ua_socket->uart.overlap && (ua_socket->uart.pwrite > ua_socket->uart.pread) ){
 		       ua_socket->uart.miss_cnt ++;
			ua_socket->uart.pread = ua_socket->uart.pwrite;		//if pwrite overhead pread ,pread is always flow rwrite 					
		}
		ua_socket->uart.tick_last_update =  xTaskGetTickCountFromISR();	// update tick everytime recved data
		ua_socket->uart.rx_cnt ++;
	}
}

static int uartadapter_uart_recv_data(ua_socket_t *ua_socket)
{
	int uart_recv_len = 0;
	
	UA_SOCKET_CHECK_2(ua_socket);
		
	ua_socket->uart.tick_current = xTaskGetTickCount();
	while((ua_socket->uart.tick_current -ua_socket->uart.tick_last_update) < (UA_UART_MAX_DELAY_TIME/portTICK_RATE_MS) 
	|| ua_socket->uart.tick_current <= ua_socket->uart.tick_last_update){
        	if(!ua_socket->uart.overlap){
        		uart_recv_len = ua_socket->uart.pwrite - ua_socket->uart.pread;
        	}else{
        		uart_recv_len = (UA_UART_RECV_BUFFER_LEN - ua_socket->uart.pread) + ua_socket->uart.pwrite;
        	}

        	if(uart_recv_len >= UA_UART_FRAME_LEN){      	
			return 2;
		}
		//vTaskDelay(10);
		ua_socket->uart.tick_current = xTaskGetTickCount();		
	}     

	return 1;
}

int uartadapter_uart_read(ua_socket_t *ua_socket, void *read_buf, size_t size)
{
	int ret = 0;
	int read_bytes;
	int pread_local,pwrite_local;
	char *ptr;
	
	ua_printf(UA_DEBUG, "==>uart adapter read uart");

	UA_SOCKET_CHECK_2(ua_socket);
	
	if(!size || !read_buf){
		ua_printf(UA_ERROR, "inpua error,size should not be null");
		ret = -1;
		return ret;
	}
	
	pread_local = ua_socket->uart.pread;
	pwrite_local = ua_socket->uart.pwrite;
	ptr = (char *)read_buf;
	
	/*calculate how much data not read */
	if(!ua_socket->uart.overlap){
		ua_socket->uart.recv_bytes = pwrite_local - pread_local;
	}else{
		ua_socket->uart.recv_bytes = (UA_UART_RECV_BUFFER_LEN - pread_local) + pwrite_local;
	}
	
	/*decide how much data shoule copy to application*/
	if(size >=  ua_socket->uart.recv_bytes ){
		read_bytes = ua_socket->uart.recv_bytes;
		ret = ua_socket->uart.recv_bytes;
	}else{
		read_bytes = size;
		ret = size;
	}	

	if(!ua_socket->uart.overlap){
		memcpy(ptr, (ua_socket->uart.recv_buf+ pread_local), read_bytes );
	}else {
		ua_printf(UA_DEBUG, "uart recv buf is write overlap!!");
		if((pread_local + read_bytes) > UA_UART_RECV_BUFFER_LEN){
			memcpy(ptr,(ua_socket->uart.recv_buf+ pread_local),(UA_UART_RECV_BUFFER_LEN-pread_local));
			memcpy(ptr+(UA_UART_RECV_BUFFER_LEN-pread_local), ua_socket->uart.recv_buf,read_bytes-(UA_UART_RECV_BUFFER_LEN- pread_local));
		}else{
			memcpy(ptr,(ua_socket->uart.recv_buf+ pread_local),read_bytes);
		}
	}
	
	ua_socket->uart.recv_bytes = 0;
	if((pread_local + read_bytes) >= UA_UART_RECV_BUFFER_LEN){		//update pread
		ua_socket->uart.pread = (pread_local + read_bytes) - UA_UART_RECV_BUFFER_LEN;
		ua_socket->uart.overlap = 0;		//clean overlap flags
	}else{
		ua_socket->uart.pread = pread_local + read_bytes;		
	}
	
	return ret;
}

int uartadapter_uart_write(ua_socket_t *ua_socket, char *pbuf, size_t size)
{
	int ret = -1;

	UA_SOCKET_CHECK_2(ua_socket);

	if(!size || !pbuf){
		ret = -1;
		return ret;
	}	

	while(rtw_down_sema(&ua_socket->uart.dma_tx) == pdTRUE){			
	    	ret = serial_send_stream_dma(&ua_socket->uart.uart_sobj, pbuf, size);
	    	if(ret != HAL_OK){
			ua_printf(UA_ERROR, "uart dma tx error %d!!", ret);
			rtw_up_sema(&ua_socket->uart.dma_tx);
			return -1;
		}else{
			return 0;
	    	}
	}
	
	return ret;
}

void uartadapter_uart_send_stream_done(uint32_t id)
{
	ua_socket_t *ua_socket = (ua_socket_t *)id;
	
	rtw_up_sema_from_isr(&ua_socket->uart.dma_tx);
}

static void uartadapter_uart_rx_thread(void* param)
{
	ua_socket_t *ua_socket = (ua_socket_t *)param;
	char	*rxbuf = NULL;
	int ret =0;
	int read_len = 0;

	UA_SOCKET_CHECK(ua_socket);

	rxbuf = pvPortMalloc(UA_UART_FRAME_LEN);
	if(NULL == rxbuf){
		ua_printf(UA_ERROR, "TCP: Allocate rx buffer failed.\n");
		return;
	}	


	while(1){
		if(rtw_down_timeout_sema(&ua_socket->uart.action_sema, 1000) == pdTRUE){
			ret = uartadapter_uart_recv_data(ua_socket);		
			if(ret == -1){
				ua_printf(UA_ERROR, "uart recv data error!");
			}else{		
	           		read_len = uartadapter_uart_read(ua_socket, rxbuf, UA_UART_FRAME_LEN);
	            		if(read_len > 0){						
	                    	uartadapter_tcp_send_data(ua_socket, rxbuf, read_len);
	            		}else if(read_len < 0){
	                     	ua_printf(UA_ERROR, "tcp send read_len = %d", read_len);            		
	            		}
			}
		}
#if UA_PS_ENABLE			
		else{
			ua_socket->uart.uart_ps_cnt++;
			if(ua_socket->uart.uart_ps_cnt >5){
				ua_socket->uart.uart_ps_cnt = 5;
				ua_socket->uart.uart_ps = 1;
				if(ua_socket->uart.uart_ps && ua_socket->tcp.tcp_ps){
					pmu_release_wakelock(UA_WAKELOCK);		
				}
			}
		}
#endif		
	}

}

void uartadapter_uart_gpio_wakeup_callback (uint32_t id, gpio_irq_event event) {
	pmu_acquire_wakelock(UA_WAKELOCK);
	ua_socket_t *ua_socket = (ua_socket_t *)id;
	ua_socket->uart.uart_ps = 0;
	ua_socket->uart.uart_ps_cnt = 0;	
}

int uartadapter_uart_open(ua_socket_t *ua_socket, ua_uart_set_str *puartpara)
{
	PinName uart_tx,uart_rx;

	UA_SOCKET_CHECK_2(ua_socket);	

	uart_tx = UA_UART_TX_PIN;
	uart_rx = UA_UART_RX_PIN;
	ua_socket->uart.uart_param.BaudRate	= puartpara->BaudRate;
	ua_socket->uart.uart_param.FlowControl	= puartpara->FlowControl;
	ua_socket->uart.uart_param.WordLen	= puartpara->number;
	ua_socket->uart.uart_param.Parity		= puartpara->parity;
	ua_socket->uart.uart_param.StopBit		= puartpara->StopBits;

	/*initial uart */
	serial_init(&ua_socket->uart.uart_sobj, uart_tx, uart_rx);
    	serial_baud(&ua_socket->uart.uart_sobj,puartpara->BaudRate);
    	serial_format(&ua_socket->uart.uart_sobj, puartpara->number, (SerialParity)puartpara->parity, puartpara->StopBits);
	
	/*uart irq handle*/
	serial_irq_handler(&ua_socket->uart.uart_sobj, uartadapter_uart_irq, (uint32_t)ua_socket);
	serial_irq_set(&ua_socket->uart.uart_sobj, RxIrq, 1);	
	serial_irq_set(&ua_socket->uart.uart_sobj, TxIrq, 1);	

   	serial_send_comp_handler(&ua_socket->uart.uart_sobj, (void*)uartadapter_uart_send_stream_done, (uint32_t)ua_socket);

#if UA_PS_ENABLE
    	//config uart rx as gpio wakeup pin
    	gpio_irq_t gpio_rx_wake;
    	gpio_irq_init(&gpio_rx_wake, UA_GPIO_WAKEUP_PIN, uartadapter_uart_gpio_wakeup_callback, (uint32_t)ua_socket);
    	gpio_irq_set(&gpio_rx_wake, IRQ_FALL, 1);   // Falling Edge Trigger
    	gpio_irq_enable(&gpio_rx_wake);	
#endif

	return 0;
}

int uartadapter_uart_baud(ua_socket_t *ua_socket, int baud_rate)
{
	int ret = 0;

	UA_SOCKET_CHECK_2(ua_socket);
	
	ua_socket->uart.uart_param.BaudRate = baud_rate;
	
    	serial_baud(&ua_socket->uart.uart_sobj, baud_rate);
	
	return ret;
}

int uartadapter_uart_para(ua_socket_t *ua_socket, int word_len, int parity, int stop_bits)
{
	int ret = 0;

	UA_SOCKET_CHECK_2(ua_socket);

	ua_socket->uart.uart_param.WordLen = word_len;
	ua_socket->uart.uart_param.Parity = parity;
	ua_socket->uart.uart_param.StopBit = stop_bits;

    	serial_format(&ua_socket->uart.uart_sobj, word_len, (SerialParity)parity, stop_bits);
	
	return ret;
}

int uartadapter_uart_getpara(ua_socket_t *ua_socket, ua_uart_get_str *uart_para)
{
	UA_SOCKET_CHECK_2(ua_socket);

	uart_para->BaudRate		=	ua_socket->uart.uart_param.BaudRate;
	uart_para->FlowControl	=	ua_socket->uart.uart_param.FlowControl;
	uart_para->number		=	ua_socket->uart.uart_param.WordLen;
	uart_para->parity			=	ua_socket->uart.uart_param.Parity;
	uart_para->StopBits		=	ua_socket->uart.uart_param.StopBit;

	return 0;
}

void uartadapter_uart_init(ua_socket_t *ua_socket)
{
	ua_uart_set_str uartset;
	ua_uart_get_str uartget;
	char uarttest[]="uart0";

	UA_SOCKET_CHECK(ua_socket);
		
	uartset.BaudRate = 9600;
	uartset.number = 8;
	uartset.StopBits = 1;
	uartset.FlowControl = 0;
	uartset.parity = 0;
	strcpy(uartset.UartName,uarttest);

	uartadapter_uart_open(ua_socket, &uartset);

	if(uartadapter_uart_getpara(ua_socket, &uartget))
		ua_printf(UA_ERROR, "get uart failed!");
	else
		ua_printf(UA_DEBUG,"uart pata:\r\n"\
			"uart->BaudRate = %d\r\n"\
			"uart->number = %d\r\n"\
			"uart->FlowControl = %d\r\n"\
			"uart->parity = %d\r\n"\
			"uart->StopBits = %d\r\n"\
			"\r\n",\
			uartget.BaudRate,\
			uartget.number,\
			uartget.FlowControl,\
			uartget.parity,\
			uartget.StopBits\
			);
}

#define _________FLASH___RELATED________________________
int uartadapter_flashread(int flashadd, char *pbuf, int len)
{
	int ret = 0;
	flash_t flash;	

	if( len == 0){
		ua_printf(UA_ERROR, "inpua error,data length should not be null!");
		ret = -1;
		return ret;
	}else	//as 8711am only canbe r/w in words.so make len is 4-bytes aligmented.
		len += 4 - ((len%4)==0 ? 4 : (len%4));
	
	while(len){
		if(flash_read_word(&flash, flashadd, (unsigned int *)pbuf) !=1 ){
			ua_printf(UA_ERROR, "read flash error!");
			ret = -1;
			return ret;
		}
		len -= 4;
		pbuf += 4;
		flashadd += 4;
	}

	return len;
}

int uartadapter_flashwrite(int flashadd, char *pbuf, int len)
{	
	int ret = 0;
	flash_t flash;	

	if( len == 0){
		ua_printf(UA_ERROR, "inpua error,data length should not be null!");
		ret = -1;
		return ret;
	}
	else	//as 8711am only canbe r/w in words.so make len is 4-bytes aligmented.
		len += 4 - ((len%4)==0 ? 4 : (len%4));
	
	while(len){
		if(flash_write_word(&flash, flashadd, *(unsigned int *)pbuf) !=1 ){
			ua_printf(UA_ERROR, "write flash error!");
			ret = -1;
			return ret;
		}
		len -= 4;
		pbuf += 4;
		flashadd += 4;
	}

	return ret;
}

int uartadapter_flasherase(int flashadd, int erase_bytelen)
{
	int ret = 0;
    	flash_t flash;
	device_mutex_lock(RT_DEV_LOCK_FLASH);
	flash_erase_sector(&flash, flashadd);
	device_mutex_unlock(RT_DEV_LOCK_FLASH);
	return ret;
}

#define _________GPIO___RELATED________________________
void uartadapter_systemreload(void)
{
	// Cortex-M3 SCB->AIRCR
	HAL_WRITE32(0xE000ED00, 0x0C, (0x5FA << 16) |                             // VECTKEY
            (HAL_READ32(0xE000ED00, 0x0C) & (7 << 8))  |   				// PRIGROUP
                                (1 << 2));                                  						// SYSRESETREQ	
}

void uartadapter_gpio_irq (uint32_t id, gpio_irq_event event)
{
       ua_printf(UA_DEBUG, "GPIO push button!!");
	
       ua_gpio_irq_happen = 1;
	rtw_up_sema_from_isr(&ua_exception_sema);
}

void uartadapter_gtimer_timeout_handler(uint32_t id)
{
    gpio_t *gpio_led = (gpio_t *)id;

    gpio_write(gpio_led, !gpio_read(gpio_led));    
}

void uartadapter_gpio_init(ua_socket_t *ua_socket)
{
    	gpio_init(&ua_socket->gpio.gpio_led, UA_GPIO_LED_PIN);
    	gpio_dir(&ua_socket->gpio.gpio_led, PIN_OUTPUT);    // Direction: Output
    	gpio_mode(&ua_socket->gpio.gpio_led, PullNone);     // No pull

    	gpio_init(&ua_socket->gpio.gpio_btn, UA_GPIO_IRQ_PIN);
    	gpio_dir(&ua_socket->gpio.gpio_btn, PIN_INPUT);    // Direction: Output
    	gpio_mode(&ua_socket->gpio.gpio_btn, PullNone);     // No pull
    	
    	gpio_irq_init(&ua_socket->gpio.gpio_btn_irq, UA_GPIO_IRQ_PIN, uartadapter_gpio_irq, (uint32_t)(&ua_socket->gpio.gpio_btn));
    	gpio_irq_set(&ua_socket->gpio.gpio_btn_irq, IRQ_FALL, 1);   // Falling Edge Trigger
    	gpio_irq_enable(&ua_socket->gpio.gpio_btn_irq);

    	// Initial a periodical timer
    	gtimer_init(&ua_socket->gpio.gpio_timer, UA_GPIO_TIMER);
    	//gtimer_start_periodical(&ua_socket->gpio.gpio_timer, 100000, (void*)timer_timeout_handler, (uint32_t)&ua_socket->gpio.gpio_led);
}

void uartadapter_gpio_led_mode(ua_socket_t *ua_socket, ua_led_mode_t mode)
{
	gtimer_stop(&ua_socket->gpio.gpio_timer);
	switch(mode){
		case UART_ADAPTER_LED_ON:
			gpio_write(&ua_socket->gpio.gpio_led, 1);    
			break;
		case UART_ADAPTER_LED_OFF:
			gpio_write(&ua_socket->gpio.gpio_led, 0);    			
			break;
		case UART_ADAPTER_LED_FAST_TWINKLE:
    			gtimer_start_periodical(&ua_socket->gpio.gpio_timer, 100000, 
					(void*)uartadapter_gtimer_timeout_handler, (uint32_t)&ua_socket->gpio.gpio_led);			
			break;
		case UART_ADAPTER_LED_SLOW_TWINKLE:
    			gtimer_start_periodical(&ua_socket->gpio.gpio_timer, 2000000, 
					(void*)uartadapter_gtimer_timeout_handler, (uint32_t)&ua_socket->gpio.gpio_led);						
			break;
		default:
       		ua_printf(UA_ERROR, "Unknown GPIO LED mode!!");			
			break;
	}
}

#define _________CONTROL__DATA__RELATED________________________
int uartadapter_strncmp(char *cs, char *ct, size_t count)
{
    	unsigned char c1, c2;

    	while (count) {
       	c1 = *cs++;
        	c2 = *ct++;
        	if (c1 != c2)
           		return c1 < c2 ? -1 : 1;
        	if (!c1)
            		break;          
        	count--;
    	}
    
    	return 0;
}

int uartadapter_control_write_tcp_info_into_flash(ua_socket_t *ua_socket)
{	
	int ret;
	UA_SOCKET_CHECK_2(ua_socket);	

	ua_printf(UA_INFO, "\r\nWrite Uart Adapter tcp connection new profile to flash");

	uartadapter_flasherase(UA_FAST_RECONNECT_TCP_DATA, 0x1000);
	ret = uartadapter_flashwrite(UA_FAST_RECONNECT_TCP_DATA, (char *)&ua_socket->tcp, sizeof(ua_tcp_socket_t));
	return ret;
}

int uartadapter_control_read_tcp_info_and_connect(ua_socket_t *ua_socket)
{
	int ret = 0;
	ua_tcp_socket_t tcp = {0};
	
	UA_SOCKET_CHECK_2(ua_socket);	

	ua_printf(UA_INFO, "\r\nRead Uart Adapter tcp connection profile from flash");

	uartadapter_flashread(UA_FAST_RECONNECT_TCP_DATA, (u8*)&tcp, sizeof(ua_tcp_socket_t));
	if(tcp.group_id != ~0x0){
		if(tcp.group_id){
			ua_socket->tcp.group_id = tcp.group_id;
			ua_socket->tcp.server_port = tcp.server_port;
			ua_socket->tcp.client_port = tcp.client_port;
			memcpy(ua_socket->tcp.client_ip, tcp.client_ip, 16);		
			
			if(xTaskCreate(uartadapter_tcp_transmit_server_thread, ((const char*)"tserver"), 256, (void *)ua_socket->tcp.server_port, UA_UART_THREAD_PRIORITY, NULL) != pdPASS)
				ua_printf(UA_ERROR, "%s xTaskCreate(tcp server) failed", __FUNCTION__);		

			strncpy(ua_tcp_server_ip, ua_socket->tcp.client_ip, 16);
			if(xTaskCreate(uartadapter_tcp_transmit_client_forever_thread, ((const char*)"tclient"), 256, (void *)ua_socket->tcp.client_port, UA_UART_THREAD_PRIORITY, NULL) != pdPASS)
				ua_printf(UA_ERROR, "%s xTaskCreate(tcp client) failed", __FUNCTION__);				
			}
	}
	else
		ua_printf(UA_INFO, "%s : read tcp info nothing", __FUNCTION__);
	return 0;
}

int uartadapter_control_set_req_handle(ua_socket_t *ua_socket, u8 *pbuf, u32 sz)
{
	u8 *p = pbuf;
	u8 len = 0;
	u16 type = 0;
	u32 port = 0;
	u32 server_ip = 0;
	int ret = 0;	
	struct sockaddr_in server_addr;
	int server_addr_len = sizeof(server_addr);
	TXTRecordRef txtRecord;
	unsigned char txt_buf[100] = {0};	// use fixed buffer for text record to prevent malloc/free
	unsigned char txt_buf2[100] = {0};	// use fixed buffer for text record to prevent malloc/free
	

       ua_printf(UA_DEBUG, "\n===>uartadapter_control_set_req_handle()");
	   
	UA_SOCKET_CHECK_2(ua_socket);
	   
	UA_PRINT_DATA(pbuf, sz);

	while(p < (pbuf+sz)){
		type = (*p)<<8 | *(p+1);
		p = p + 2;
		len = *p++;
		ua_printf(UA_DEBUG, "type=%d len=%d\n", type, len);
		switch(type)
		{
			case UART_CTRL_TYPE_BAUD_RATE:
				ua_socket->uart.uart_param.BaudRate = *(u32 *)p;
       			ua_printf(UA_INFO, "SET UART BAUD_RATE to %d.\n", ua_socket->uart.uart_param.BaudRate);
				serial_baud(&ua_socket->uart.uart_sobj, ua_socket->uart.uart_param.BaudRate);
				break;
			case UART_CTRL_TYPE_WORD_LEN:
				ua_socket->uart.uart_param.WordLen = *p;
       			ua_printf(UA_INFO, "SET UART WORD_LEN to %d.\n", ua_socket->uart.uart_param.WordLen);
				serial_format(&ua_socket->uart.uart_sobj, 
							ua_socket->uart.uart_param.WordLen, 
							(SerialParity)ua_socket->uart.uart_param.Parity,
							ua_socket->uart.uart_param.StopBit);
				break;
			case UART_CTRL_TYPE_PARITY:
				ua_socket->uart.uart_param.Parity = *p;
       			ua_printf(UA_INFO, "SET UART PARITY to %d.\n", ua_socket->uart.uart_param.Parity);
				serial_format(&ua_socket->uart.uart_sobj, 
							ua_socket->uart.uart_param.WordLen, 
							(SerialParity)ua_socket->uart.uart_param.Parity,
							ua_socket->uart.uart_param.StopBit);
				break;
			case UART_CTRL_TYPE_STOP_BIT:
				ua_socket->uart.uart_param.StopBit = *p;
       			ua_printf(UA_INFO, "SET UART STOP_BIT to %d.\n", ua_socket->uart.uart_param.StopBit);
				serial_format(&ua_socket->uart.uart_sobj, 
							ua_socket->uart.uart_param.WordLen, 
							(SerialParity)ua_socket->uart.uart_param.Parity,
							ua_socket->uart.uart_param.StopBit);
				break;
			case UART_CTRL_TYPE_TCP_SERVER_CREATE:
				port = (*p)<<8 | *(p+1);	
				
				if(ua_socket->tcp.transmit_server_listen_socket != -1){
					ua_printf(UA_INFO, "Close old transmit server socket %d", ua_socket->tcp.transmit_server_listen_socket);				
					close(ua_socket->tcp.transmit_server_listen_socket);
				}				
				if(xTaskCreate(uartadapter_tcp_transmit_server_thread, ((const char*)"tserver"), 256, (void *)port, UA_UART_THREAD_PRIORITY, NULL) != pdPASS)
					ua_printf(UA_ERROR, "%s xTaskCreate(tcp server) failed", __FUNCTION__);		
				ua_socket->tcp.server_port = port;
				uartadapter_control_write_tcp_info_into_flash(ua_socket);
				vTaskDelay(50);
				ua_printf(UA_DEBUG, "CREATE TCP SERVER WITH PORT %d.\n", port);
				//TODO
				break;
			case UART_CTRL_TYPE_TCP_SERVER_DELETE:
				port = (*p)<<8 | *(p+1);	
				
				if(ua_socket->tcp.transmit_server_listen_socket != -1){
					getsockname (ua_socket->tcp.transmit_server_listen_socket, (struct sockaddr *)&server_addr, &server_addr_len);
					if(server_addr.sin_port == ntohs((u16)port)){
						ua_printf(UA_INFO,"uart tcp transmit server socket %d closed by control socket!", ua_socket->tcp.transmit_server_listen_socket);	
						close(ua_socket->tcp.transmit_server_listen_socket);		
						ua_socket->tcp.transmit_server_listen_socket = -1;	
       					if(ua_socket->tcp.transmit_recv_socket != -1){
							ua_printf(UA_INFO,"uart tcp transmit receive socket %d closed by control socket!", ua_socket->tcp.transmit_recv_socket);	
							close(ua_socket->tcp.transmit_recv_socket);		
							ua_socket->tcp.transmit_recv_socket = -1;
       					}	

						if(ua_socket->tcp.transmit_send_socket != -1){
							ua_printf(UA_INFO,"uart tcp transmit send socket %d closed by control socket!", ua_socket->tcp.transmit_send_socket);	
							close(ua_socket->tcp.transmit_send_socket);		
							ua_socket->tcp.transmit_send_socket = -1;	
		       				ua_printf(UA_INFO, "DISCONNECT FROM TCP SERVER.\n");	
							memset(ua_socket->tcp.client_ip, 0, 16);
							ua_socket->tcp.client_port = 0;
						}						
						ua_printf(UA_INFO, "DELETE TCP SERVER \n");	
						ua_socket->tcp.server_port = 0;
						uartadapter_control_write_tcp_info_into_flash(ua_socket);
					}else{
						ua_printf(UA_INFO, "DELETE TCP SERVER FAILED: port not match\n");	
						return -1;
					}
				}else{
					ua_printf(UA_INFO, "DELETE TCP SERVER FAILED: server not exist\n");		
					return -1;					
				}

				break;	
			case UART_CTRL_TYPE_TCP_CLIENT_CONNECT:
				server_ip = (*(p+3))<<24 | (*(p+2))<<16 | (*(p+1))<<8 | *p;
				p = p + 4;
				memcpy(ua_tcp_server_ip, inet_ntoa(server_ip), 16);
				port = (*p)<<8 | *(p+1);
       			ret = uartadapter_tcpclient(ua_socket, ua_tcp_server_ip, (unsigned short)port);
				if(ret == 0){
					ua_printf(UA_INFO, "CONNECT TO TCP SERVER, IP %s PORT %d Success.\n", ua_tcp_server_ip, port);
				}else{
					ua_printf(UA_INFO, "CONNECT TO TCP SERVER, IP %s PORT %d Failed.\n", ua_tcp_server_ip, port);				
					return -1;
				}
				memcpy(ua_socket->tcp.client_ip, ua_tcp_server_ip, 16);
				ua_socket->tcp.client_port = port;	
				uartadapter_control_write_tcp_info_into_flash(ua_socket);
				break;
			case UART_CTRL_TYPE_TCP_CLIENT_DISCONNECT:
				server_ip = (*(p+3))<<24 | (*(p+2))<<16 | (*(p+1))<<8 | *p;
				p = p + 4;
				memcpy(ua_tcp_server_ip, inet_ntoa(server_ip), 16);
				port = (*p)<<8 | *(p+1);
				
				if(ua_socket->tcp.transmit_send_socket != -1){
					getpeername(ua_socket->tcp.transmit_send_socket, (struct sockaddr *)&server_addr, &server_addr_len);
					if(server_addr.sin_port == ntohs((u16)port) && server_addr.sin_addr.s_addr == server_ip){
						ua_printf(UA_INFO,"uart tcp transmit send socket %d closed by control socket!", ua_socket->tcp.transmit_send_socket);	
						close(ua_socket->tcp.transmit_send_socket);		
						ua_socket->tcp.transmit_send_socket = -1;	
       					ua_printf(UA_INFO, "DISCONNECT FROM TCP SERVER.\n");	
						memset(ua_socket->tcp.client_ip, 0, 16);
						ua_socket->tcp.client_port = 0;
						uartadapter_control_write_tcp_info_into_flash(ua_socket);
					}else{
       					ua_printf(UA_INFO, "DISCONNECT FROM TCP SERVER FAILED: port or IP not match.\n");					
						return -1;
					}
				}else{
					ua_printf(UA_INFO, "DISCONNECT FROM TCP SERVER FAILED: connection not exist\n");		
					return -1;					
				}
				break;		
			case UART_CTRL_TYPE_TCP_GROUP_ID:
				ua_socket->tcp.group_id = *p;
				ua_printf(UA_INFO,"SET TCP GROUP ID to %d!", *p);	
				sprintf(txt_buf2, "groupid:%d, tcpserver:%d", ua_socket->tcp.group_id, ua_socket->tcp.server_port);						
				TXTRecordCreate(&txtRecord, sizeof(txt_buf), txt_buf);
				TXTRecordSetValue(&txtRecord, "groupid", strlen(txt_buf2), txt_buf2);
				mDNSUpdateService(ua_socket->dnsServiceRef, &txtRecord, 0);
				mDNSUpdateService(ua_socket->dnsServiceRef2, &txtRecord, 0);

				uartadapter_control_write_tcp_info_into_flash(ua_socket);
				
				break;				
			default:
				ua_printf(UA_DEBUG, "Unknown Type, just skip\n");		
				break;				
				
		}
		p += len;
	}
	return 0;
}

int uartadapter_control_get_req_handle(ua_socket_t *ua_socket, u16 type, u8 *prsp, u32 *sz)
{
	u8 *p = prsp;

 	ua_printf(UA_DEBUG, "===>uartadapter_control_get_req_handle()");

	UA_SOCKET_CHECK_2(ua_socket);
	
	sprintf((char *)p, UA_CONTROL_PREFIX);
	p += strlen(UA_CONTROL_PREFIX);
	*p++ = UART_CTRL_MODE_GET_RSP;

	if(type & UART_CTRL_TYPE_BAUD_RATE){
		*p++ = 0;
		*p++ = UART_CTRL_TYPE_BAUD_RATE;		
		*p++ = 4;
		*(u32*)p = ua_socket->uart.uart_param.BaudRate;
		p += 4;
	}
	if(type & UART_CTRL_TYPE_WORD_LEN){
		*p++ = 0;		
		*p++ = UART_CTRL_TYPE_WORD_LEN;
		*p++ = 1;
		*p = ua_socket->uart.uart_param.WordLen;
		p += 1;
	}
	if(type & UART_CTRL_TYPE_PARITY){
		*p++ = 0;		
		*p++ = UART_CTRL_TYPE_PARITY;
		*p++ = 1;
		*p = ua_socket->uart.uart_param.Parity;
		p += 1;
	}
	if(type & UART_CTRL_TYPE_STOP_BIT){
		*p++ = 0;				
		*p++ = UART_CTRL_TYPE_STOP_BIT;
		*p++ = 1;
		*p = ua_socket->uart.uart_param.StopBit;
		p += 1;
	}
#if 0	
	if(type & UART_CTRL_TYPE_FLOW_CTRL){
		*p++ = UART_CTRL_TYPE_FLOW_CTRL;
		*p++ = 1;
		*p = ua_socket->uart.uart_param.FlowControl;
		p += 1;
	}
#endif	
	*sz = p - prsp;

	UA_PRINT_DATA(prsp, *sz);
	return 0;		
}

int uartadapter_control_process(ua_socket_t *ua_socket, char *pbuf, size_t size)
{
	/*the same as socket*/
	int ret = 0;
	
	UA_SOCKET_CHECK_2(ua_socket);

	if(!size || !pbuf){
		//ua_printf(UA_ERROR, "control data input error,please check!");
		ret = -1;
		return ret;
	}

	UA_PRINT_DATA(pbuf, size);

       if(uartadapter_strncmp(pbuf, UA_CONTROL_PREFIX, 10) != 0)
       {
       	ua_printf(UA_ERROR, "control data prefix wrong!");
		return -1;
	}
	else
	{
		u8 *p = (u8*)pbuf + strlen(UA_CONTROL_PREFIX);
		u8 mode = *p++;
		switch(mode)
		{
			case UART_CTRL_MODE_SET_REQ: //AMEBA_UART-MODE-TYPE-LEN-VAL-TYPE-LEN-VAL...
			{
				char rsp[32] = {0};  //AMEBA_UART-MODE
				u32 sz = strlen(UA_CONTROL_PREFIX);
				ret = uartadapter_control_set_req_handle(ua_socket, p, (size - strlen(UA_CONTROL_PREFIX) - 1));
				if(0 == ret){
					sprintf(rsp, UA_CONTROL_PREFIX);
					*(rsp + sz) = UART_CTRL_MODE_SET_RSP;
					sz ++;
					sprintf(rsp + sz, "\n");
					sz ++;	
					vTaskDelay(100);
					uartadapter_tcp_send_control(ua_socket, rsp, sz);
				}
				break;
			}
			case UART_CTRL_MODE_GET_REQ: //AMEBA_UART-MODE-TYPE
			{
				char rsp[128] = {0};
				u32 sz = 0;
				u16 type = (*p)<<8 | *(p+1);
				uartadapter_control_get_req_handle(ua_socket, type, (u8*)rsp, &sz);
				sprintf(rsp + sz, "\n");
				sz ++;		
				vTaskDelay(100);				
				uartadapter_tcp_send_control(ua_socket, rsp, sz);
				break;
			}
			default:
				ua_printf(UA_ERROR, UA_CONTROL_PREFIX": Mode (%d) not support!", mode);
				break;
		}
	}
	return 0;
}

#define _________TCP__RELATED________________________
int uartadapter_tcpclient(ua_socket_t *ua_socket, const char *host_ip, unsigned short usPort)
{
       int	iAddrSize;
       int	iSockFD = -1;
       int	iStatus;
       int	enable = 1;
       struct sockaddr_in  sAddr;
	int send_timeout = 3000;	   

	UA_SOCKET_CHECK_2(ua_socket);	   

	FD_ZERO(&sAddr);
	sAddr.sin_family = AF_INET;
	sAddr.sin_port = htons(usPort);
	sAddr.sin_addr.s_addr = inet_addr(host_ip);

	iAddrSize = sizeof(struct sockaddr_in);

	iSockFD = socket(AF_INET, SOCK_STREAM, 0);
	if( iSockFD < 0 ) {
		ua_printf(UA_ERROR, "TCP ERROR: create tcp client socket fd error!");
		return -1;
	}

	ua_printf(UA_DEBUG, "TCP: ServerIP=%s port=%d.", host_ip, usPort);
	ua_printf(UA_DEBUG, "TCP: Create socket %d.", iSockFD);
	// connecting to TCP server
	iStatus = connect(iSockFD, (struct sockaddr *)&sAddr, iAddrSize);
	if (iStatus < 0) {
		ua_printf(UA_ERROR, "tcp client connect server error %d!", iStatus);
		goto Exit;
	}  

	iStatus = setsockopt(iSockFD, IPPROTO_TCP, TCP_NODELAY, &enable, sizeof(enable));
	if (iStatus < 0) {
		ua_printf(UA_ERROR, "tcp client socket set TCP_NODELAY error! ");
		goto Exit;
	}

	iStatus = setsockopt(iSockFD, SOL_SOCKET, SO_KEEPALIVE, &enable, sizeof(enable));
	if (iStatus < 0) {
		ua_printf(UA_ERROR, "tcp client socket set SO_KEEPALIVE error! ");
		goto Exit;
	}		

#if LWIP_SO_SNDTIMEO
	iStatus = setsockopt(iSockFD, SOL_SOCKET, SO_SNDTIMEO, &send_timeout, sizeof(int));
	if (iStatus < 0) {
		ua_printf(UA_ERROR, "tcp client socket set SO_SNDTIMEO error! ");
		goto Exit;
	}
#endif	

#if LWIP_SO_RCVTIMEO
	iStatus = setsockopt(iSockFD, SOL_SOCKET, SO_RCVTIMEO, &send_timeout, sizeof(int));
	if (iStatus < 0) {
		ua_printf(UA_ERROR, "tcp client socket set SO_RCVTIMEO error! ");
		goto Exit;
	}	
#endif	 
       
	if(ua_socket->tcp.transmit_send_socket != -1){
		close(ua_socket->tcp.transmit_send_socket);
		ua_printf(UA_INFO, "Close old transmit send socket %d.", ua_socket->tcp.transmit_send_socket);			
	}

	ua_printf(UA_INFO, "Connect to transmit server successfully.");	
	ua_socket->tcp.transmit_send_socket = iSockFD;	

	return 0;

Exit:
	close(iSockFD);
	return -1;
}

int uartadapter_tcpserver(ua_socket_t *ua_socket, unsigned short usPort, u8 socket_type)
{
	struct sockaddr_in  sLocalAddr;	
	int			iAddrSize;
	int			iSockFD;
	int			iStatus;
	int			enable = 1;

	UA_SOCKET_CHECK_2(ua_socket);

	iSockFD = socket(AF_INET, SOCK_STREAM, 0);
	if( iSockFD < 0 ) {
             ua_printf(UA_ERROR, "create server_socket error!");  		
		goto Exit;
	}

	iStatus = setsockopt( iSockFD, SOL_SOCKET, SO_REUSEADDR,
			(const char *) &enable, sizeof( enable ) );	
	if( iStatus < 0 ) {
		ua_printf(UA_ERROR, "set tcp server socket SO_REUSEADDR error %d! ", iStatus);
		goto Exit;
	}	

	ua_printf(UA_DEBUG, "TCP: Create Tcp server socket %d", iSockFD);

	//filling the TCP server socket address
	memset((char *)&sLocalAddr, 0, sizeof(sLocalAddr));
	sLocalAddr.sin_family      = AF_INET;
	sLocalAddr.sin_len         = sizeof(sLocalAddr);
	sLocalAddr.sin_port        = htons(usPort);
	sLocalAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	iAddrSize = sizeof(sLocalAddr);

	iStatus = bind(iSockFD, (struct sockaddr *)&sLocalAddr, iAddrSize);
	if( iStatus < 0 ) {
		ua_printf(UA_ERROR, "bind tcp server socket fd error %d! ", iStatus);
		goto Exit;
	}
	ua_printf(UA_DEBUG, "TCP: Bind successfully.");

	iStatus = listen(iSockFD, 10);
	if( iStatus != 0 ) {
		ua_printf(UA_ERROR, "listen tcp server socket fd error %d!", iStatus);
		goto Exit;
	}

	if(0 == socket_type){
		ua_socket->tcp.chat_server_listen_socket = iSockFD;
		ua_printf(UA_INFO, "TCP Chat Server: Listen on port %d", usPort);		
	}else if(1 == socket_type){
		ua_socket->tcp.control_server_listen_socket = iSockFD;
		ua_printf(UA_INFO, "TCP Control Server: Listen on port %d", usPort);				
	}else{	
		ua_socket->tcp.transmit_server_listen_socket = iSockFD;
		ua_printf(UA_INFO, "TCP Transmit Server: Listen on port %d", usPort);				
	}

	return 0;
	
Exit:	 
	close(iSockFD);
       ua_printf(UA_DEBUG, "Tcp server listen on port %d closed!", usPort);
	return 0;
}

void uartadapter_tcp_chat_server_thread(void *param)
{
	ua_socket_t *ua_socket = (ua_socket_t *)param;
	unsigned short port = UA_CHAT_SOCKET_PORT;

	UA_SOCKET_CHECK(ua_socket);	
	
	ua_printf(UA_DEBUG, "Uart Adapter: Start Tcp Data Server!");
       uartadapter_tcpserver(ua_socket, port, 0);
	
#if defined(INCLUDE_uxTaskGetStackHighWaterMark) && (INCLUDE_uxTaskGetStackHighWaterMark == 1)
	ua_printf(UA_DEBUG, "Min available stack size of %s = %d * %d bytes\n\r", __FUNCTION__, uxTaskGetStackHighWaterMark(NULL), sizeof(portBASE_TYPE));
#endif
	ua_printf(UA_DEBUG, "TCP: Tcp data server stopped!");
	vTaskDelete(NULL);
}

void uartadapter_tcp_control_server_thread(void *param)
{
	ua_socket_t *ua_socket = (ua_socket_t *)param;
	unsigned short port = UA_CONTROL_SOCKET_PORT;
	
	UA_SOCKET_CHECK(ua_socket);		
	
	ua_printf(UA_DEBUG, "Uart Adapter: Start Tcp Control Server!");
       uartadapter_tcpserver(ua_socket, port, 1);
	
#if defined(INCLUDE_uxTaskGetStackHighWaterMark) && (INCLUDE_uxTaskGetStackHighWaterMark == 1)
	ua_printf(UA_DEBUG, "Min available stack size of %s = %d * %d bytes", __FUNCTION__, uxTaskGetStackHighWaterMark(NULL), sizeof(portBASE_TYPE));
#endif
	ua_printf(UA_DEBUG, "TCP: Tcp control server stopped!");
	vTaskDelete(NULL);
}

void uartadapter_tcp_transmit_server_thread(void *param)
{
	ua_socket_t *ua_socket = ua_global_socket;
	unsigned short port = (int)param;
	
	UA_SOCKET_CHECK(ua_socket);		
	
	ua_printf(UA_DEBUG, "Uart Adapter: Start Tcp Transmit Server!");
       uartadapter_tcpserver(ua_socket, port, 2);

	uartadapter_gpio_led_mode(ua_socket, UART_ADAPTER_LED_SLOW_TWINKLE);		
	
#if defined(INCLUDE_uxTaskGetStackHighWaterMark) && (INCLUDE_uxTaskGetStackHighWaterMark == 1)
	ua_printf(UA_DEBUG, "Min available stack size of %s = %d * %d bytes", __FUNCTION__, uxTaskGetStackHighWaterMark(NULL), sizeof(portBASE_TYPE));
#endif
	ua_printf(UA_DEBUG, "TCP: Tcp transmit server thread delete!");
	vTaskDelete(NULL);
}

void uartadapter_tcp_transmit_client_thread(void *param)
{
	ua_socket_t *ua_socket = ua_global_socket;
	unsigned short port = (int)param;

	UA_SOCKET_CHECK(ua_socket);		
	
	ua_printf(UA_DEBUG, "Uart Adapter: Start Tcp Data Client!");
       uartadapter_tcpclient(ua_socket, ua_tcp_server_ip, port);

	uartadapter_gpio_led_mode(ua_socket, UART_ADAPTER_LED_SLOW_TWINKLE);		
	
#if defined(INCLUDE_uxTaskGetStackHighWaterMark) && (INCLUDE_uxTaskGetStackHighWaterMark == 1)
	ua_printf(UA_DEBUG, "Min available stack size of %s = %d * %d bytes\n\r", __FUNCTION__, uxTaskGetStackHighWaterMark(NULL), sizeof(portBASE_TYPE));
#endif
	ua_printf(UA_DEBUG, "TCP: Tcp transmit client thread delete!");
	vTaskDelete(NULL);
}

void uartadapter_tcp_transmit_client_forever_thread(void *param)
{
	ua_socket_t *ua_socket = ua_global_socket;
	unsigned short port = (int)param;
	int ret = 0;
	UA_SOCKET_CHECK(ua_socket);		
	
	ua_printf(UA_DEBUG, "Uart Adapter: Start Tcp Transmit  Client forever thread!");

	do{
		ret = uartadapter_tcpclient(ua_socket, ua_tcp_server_ip, port);
		if(ret != 0){
			ua_printf(UA_INFO, "Uart Adapter: Try to connect to TCP server again");	
			vTaskDelay(3000);
		}
	}while(ret != 0);

	uartadapter_gpio_led_mode(ua_socket, UART_ADAPTER_LED_SLOW_TWINKLE);		
	
#if defined(INCLUDE_uxTaskGetStackHighWaterMark) && (INCLUDE_uxTaskGetStackHighWaterMark == 1)
	ua_printf(UA_DEBUG, "Min available stack size of %s = %d * %d bytes\n\r", __FUNCTION__, uxTaskGetStackHighWaterMark(NULL), sizeof(portBASE_TYPE));
#endif
	ua_printf(UA_DEBUG, "TCP: Tcp transmit client thread delete!");
	vTaskDelete(NULL);
}

void uartadapter_tcp_send_data(ua_socket_t *ua_socket, char *buffer, int size)
{    
	int                 iStatus;

	UA_SOCKET_CHECK(ua_socket);		
	
	if(ua_socket->tcp.chat_socket != -1){	
		ua_socket->tcp.send_flag = 1;
		rtw_down_sema(&ua_socket->uart.tcp_tx_rx_sema);			
		iStatus = send(ua_socket->tcp.chat_socket, buffer, size, 0 );
		rtw_up_sema(&ua_socket->uart.tcp_tx_rx_sema);	
		ua_socket->tcp.send_flag = 0;				
		if( iStatus <= 0 ){
			ua_printf(UA_ERROR, "tcp chat socket send data error!  iStatus:%d!", iStatus);
		}else{
			ua_socket->tcp.tx_cnt += iStatus;							
		}	
	}

	if(ua_socket->tcp.transmit_send_socket != -1){	
		ua_socket->tcp.send_flag = 1;	
		iStatus = send(ua_socket->tcp.transmit_send_socket, buffer, size, 0 );
		ua_socket->tcp.send_flag = 0;				
		if( iStatus <= 0 ){
			ua_printf(UA_ERROR, "tcp transmit send socket send data error!  iStatus:%d!", iStatus);
		}else{
			ua_socket->tcp.tx_cnt += iStatus;							
		}			
	}		
	        
	return;
}

void uartadapter_tcp_send_control(ua_socket_t *ua_socket, char *buffer, int size)
{    
	int                 iStatus;

	UA_SOCKET_CHECK(ua_socket);		

	if(ua_socket->tcp.control_socket != -1){ 	
		ua_socket->tcp.send_flag = 1;				
		iStatus = send(ua_socket->tcp.control_socket, buffer, size, 0 );	
		ua_socket->tcp.send_flag = 0;				
		if( iStatus <= 0 ){
			ua_printf(UA_ERROR,"tcp control socket send data error!  iStatus:%d!", iStatus);
		}
		
		if(ua_debug_print_en){
			ua_printf(UA_INFO,"uart tcp control socket send %d bytes, ret %d!", size, iStatus);	
		}				
	}
	        
	return;
}

void uartadapter_tcp_except_handler(ua_socket_t *ua_socket, fd_set *exceptfds)
{
       if(ua_socket->tcp.chat_socket != -1 && FD_ISSET(ua_socket->tcp.chat_socket, exceptfds)){
		ua_printf(UA_INFO,"uart tcp chat socket %d exception happen, need to close!", ua_socket->tcp.chat_socket);	
		close(ua_socket->tcp.chat_socket);		
		ua_socket->tcp.chat_socket = -1;
	}
		
       if(ua_socket->tcp.control_socket != -1 && FD_ISSET(ua_socket->tcp.control_socket, exceptfds)){
		ua_printf(UA_INFO,"uart tcp control socket %d exception happen, need to close!", ua_socket->tcp.control_socket);	
		close(ua_socket->tcp.control_socket);		
		ua_socket->tcp.control_socket = -1;	
	}	

       if(ua_socket->tcp.transmit_recv_socket != -1 && FD_ISSET(ua_socket->tcp.transmit_recv_socket, exceptfds)){
		ua_printf(UA_INFO,"uart tcp transmit recv socket %d exception happen, need to close!", ua_socket->tcp.transmit_recv_socket);	
		close(ua_socket->tcp.transmit_recv_socket);		
		ua_socket->tcp.transmit_recv_socket = -1;
		uartadapter_gpio_led_mode(ua_socket, UART_ADAPTER_LED_FAST_TWINKLE);		
#if 0
		if(ua_socket->tcp.transmit_server_listen_socket != -1){
			ua_printf(UA_INFO,"uart tcp transmit server socket %d exception happen, need to close!", ua_socket->tcp.transmit_server_listen_socket);	
			close(ua_socket->tcp.transmit_server_listen_socket);		
			ua_socket->tcp.transmit_server_listen_socket = -1;	
		}
#endif		
	}		

       if(ua_socket->tcp.transmit_send_socket != -1 && FD_ISSET(ua_socket->tcp.transmit_send_socket, exceptfds)){
		ua_printf(UA_INFO,"uart tcp transmit send socket %d exception happen, need to close!", ua_socket->tcp.transmit_send_socket);	
		close(ua_socket->tcp.transmit_send_socket);		
		ua_socket->tcp.transmit_send_socket = -1;
		uartadapter_gpio_led_mode(ua_socket, UART_ADAPTER_LED_FAST_TWINKLE);

		strncpy(ua_tcp_server_ip, ua_socket->tcp.client_ip, 16);
		if(xTaskCreate(uartadapter_tcp_transmit_client_forever_thread, ((const char*)"tclient"), 256, (void *)ua_socket->tcp.client_port, UA_UART_THREAD_PRIORITY, NULL) != pdPASS)
			ua_printf(UA_ERROR, "%s xTaskCreate(tcp client) failed", __FUNCTION__);				
	}		   

       if(ua_socket->tcp.chat_server_listen_socket != -1 && FD_ISSET(ua_socket->tcp.chat_server_listen_socket, exceptfds)){ 
		ua_printf(UA_INFO,"uart tcp chat server socket %d exception happen, need to restart!", ua_socket->tcp.chat_server_listen_socket);	
		close(ua_socket->tcp.chat_server_listen_socket);		
		ua_socket->tcp.chat_server_listen_socket = -1;   
		uartadapter_tcpserver(ua_socket, UA_CHAT_SOCKET_PORT, 0);				
	}

       if(ua_socket->tcp.control_server_listen_socket != -1 && FD_ISSET(ua_socket->tcp.control_server_listen_socket, exceptfds)){ 
		ua_printf(UA_INFO,"uart tcp control server socket %d exception happen, need to restart!", ua_socket->tcp.control_server_listen_socket);	
		close(ua_socket->tcp.control_server_listen_socket);		
		ua_socket->tcp.control_server_listen_socket = -1;   
		uartadapter_tcpserver(ua_socket, UA_CONTROL_SOCKET_PORT, 1);				
	}     

       if(ua_socket->tcp.transmit_server_listen_socket != -1 && FD_ISSET(ua_socket->tcp.transmit_server_listen_socket, exceptfds)){ 
		ua_printf(UA_INFO,"uart tcp transmit server socket %d exception happen, need to close!", ua_socket->tcp.transmit_server_listen_socket);	
		close(ua_socket->tcp.transmit_server_listen_socket);		
		ua_socket->tcp.transmit_server_listen_socket = -1;
		//uartadapter_gpio_led_mode(ua_socket, UART_ADAPTER_LED_FAST_TWINKLE);
		uartadapter_tcpserver(ua_socket, ua_socket->tcp.server_port, 2);				
	}    

}

void uartadapter_tcp_chat_socket_handler(ua_socket_t *ua_socket, char *tcp_rxbuf)
{
     	int recv_len;
		
	UA_SOCKET_CHECK(ua_socket);	

	ua_socket->tcp.recv_flag = 1; 
	rtw_down_sema(&ua_socket->uart.tcp_tx_rx_sema);
	recv_len = recv(ua_socket->tcp.chat_socket, tcp_rxbuf, UA_UART_FRAME_LEN, MSG_DONTWAIT);
	rtw_up_sema(&ua_socket->uart.tcp_tx_rx_sema);
       ua_socket->tcp.recv_flag = 0;	
	if(recv_len < 0){
		ua_printf(UA_ERROR, "Tcp Chat Socket %d Recv Error, ret = %d", ua_socket->tcp.chat_socket, recv_len);
	}else if(recv_len == 0){
		ua_printf(UA_INFO, "Tcp Chat Socket %d closed", ua_socket->tcp.chat_socket);
		close(ua_socket->tcp.chat_socket);
		ua_socket->tcp.chat_socket = -1;		
	}else{	
		ua_printf(UA_DEBUG, "Tcp Chat Socket %d Recv %d Data", ua_socket->tcp.chat_socket, recv_len);						
		uartadapter_uart_write(ua_socket, tcp_rxbuf, recv_len);
		ua_socket->tcp.rx_cnt += recv_len;
	}
	
	return;
}

void uartadapter_tcp_control_socket_handler(ua_socket_t *ua_socket)
{
    	char tcp_rxbuf[UA_UART_FRAME_LEN];
     	int recv_len;

	UA_SOCKET_CHECK(ua_socket);	
	
	recv_len = recv(ua_socket->tcp.control_socket, tcp_rxbuf, UA_UART_FRAME_LEN, MSG_DONTWAIT);  //MSG_DONTWAIT   MSG_WAITALL
	if(recv_len<0){
		ua_printf(UA_ERROR, "Tcp Control Socket %d Recv Error", ua_socket->tcp.control_socket);
	}else if(recv_len == 0){
		ua_printf(UA_INFO, "Tcp Control Socket %d closed", ua_socket->tcp.control_socket);
		close(ua_socket->tcp.control_socket);
		ua_socket->tcp.control_socket = -1;		
	}else{   
		ua_printf(UA_DEBUG, "Tcp Control Socket %d Recv %d Data", ua_socket->tcp.control_socket, recv_len);									
		uartadapter_control_process(ua_socket, (void*)tcp_rxbuf, recv_len);
	}

	return;
}

void uartadapter_tcp_transmit_socket_handler(ua_socket_t *ua_socket, char *tcp_rxbuf)
{
     	int recv_len;		

	UA_SOCKET_CHECK(ua_socket);

	ua_socket->tcp.recv_flag = 1;  
	recv_len = recv(ua_socket->tcp.transmit_recv_socket, tcp_rxbuf, UA_UART_FRAME_LEN, MSG_DONTWAIT);
	ua_socket->tcp.recv_flag = 0;		   
	if(recv_len < 0){
		ua_printf(UA_ERROR, "Tcp Transmit Recv Socket %d Recv Error, ret = %d", ua_socket->tcp.transmit_recv_socket, recv_len);		
	}else if(recv_len == 0){
		ua_printf(UA_INFO, "Tcp Transmit Recv Socket %d closed", ua_socket->tcp.transmit_recv_socket);
		close(ua_socket->tcp.transmit_recv_socket);
		ua_socket->tcp.transmit_recv_socket = -1;	
#if 0		
		if(ua_socket->tcp.transmit_server_listen_socket != -1){
			ua_printf(UA_INFO, "Tcp Transmit Server Socket %d closed", ua_socket->tcp.transmit_server_listen_socket);
			close(ua_socket->tcp.transmit_server_listen_socket);
			ua_socket->tcp.transmit_server_listen_socket = -1;	
		}
#endif		
	}else{	
		uartadapter_uart_write(ua_socket, tcp_rxbuf, recv_len);
		ua_socket->tcp.rx_cnt += recv_len;
	}
	
	return;
}

void uartadapter_tcp_chat_listen_socket_handler(ua_socket_t *ua_socket)
{
       int old_chat_socket;
	struct sockaddr_in  sAddr;    
	int addrlen = sizeof(sAddr);
	int iStatus;
	int enable = 1;
	int send_timeout = 3000;	

	UA_SOCKET_CHECK(ua_socket);	

	old_chat_socket = ua_socket->tcp.chat_socket;
		
	ua_socket->tcp.chat_socket = accept(ua_socket->tcp.chat_server_listen_socket, (struct sockaddr *)&sAddr, (socklen_t*)&addrlen);
	if( ua_socket->tcp.chat_socket< 0 ) {
		ua_printf(UA_ERROR, "Accept tcp chat socket error");
		goto EXIT;
	} 	

	iStatus = setsockopt(ua_socket->tcp.chat_socket, IPPROTO_TCP, TCP_NODELAY, &enable, sizeof(enable));
	if (iStatus < 0) {
		ua_printf(UA_ERROR, "tcp chat socket set opt TCP_NODELAY error! ");
		goto EXIT;
	}

	iStatus = setsockopt(ua_socket->tcp.chat_socket, SOL_SOCKET, SO_KEEPALIVE, &enable, sizeof(enable));
	if (iStatus < 0) {
		ua_printf(UA_ERROR, "tcp chat socket set opt SO_KEEPALIVE error! ");
		goto EXIT;
	}	

#if LWIP_SO_SNDTIMEO
	iStatus = setsockopt(ua_socket->tcp.chat_socket, SOL_SOCKET, SO_SNDTIMEO, &send_timeout, sizeof(send_timeout));
	if (iStatus < 0) {
		ua_printf(UA_ERROR, "tcp client socket set opt error! ");
		goto EXIT;
	}
#endif

#if LWIP_SO_RCVTIMEO
	iStatus = setsockopt(ua_socket->tcp.chat_socket, SOL_SOCKET, SO_RCVTIMEO, &send_timeout, sizeof(send_timeout));
	if (iStatus < 0) {
		ua_printf(UA_ERROR, "tcp client socket set opt error! ");
		goto EXIT;
	}	
#endif

	ua_printf(UA_INFO, "Accept new chat socket %d on port %d successfully.", ua_socket->tcp.chat_socket, htons(sAddr.sin_port));				       			
	if(old_chat_socket != -1)
	{
		close(old_chat_socket);
		ua_printf(UA_INFO, "Close old chat socket %d.", old_chat_socket);	
	}

	return;

EXIT:
	if(ua_socket->tcp.chat_server_listen_socket != -1){
     	 	close(ua_socket->tcp.chat_server_listen_socket);
     	 	ua_socket->tcp.chat_server_listen_socket = -1;    	 
	}			
}

void uartadapter_tcp_control_listen_socket_handler(ua_socket_t *ua_socket)
{
	int old_control_socket;
	struct sockaddr_in  sAddr;    
	int addrlen = sizeof(sAddr);
	int iStatus;	
	int enable = 1;	
	int send_timeout = 3000;	

	UA_SOCKET_CHECK(ua_socket);	

	old_control_socket = ua_socket->tcp.control_socket;
	
	ua_socket->tcp.control_socket = accept(ua_socket->tcp.control_server_listen_socket, (struct sockaddr *)&sAddr, (socklen_t*)&addrlen);
	if( ua_socket->tcp.control_socket< 0 ) {
		ua_printf(UA_ERROR, "Accept tcp control socket error");
		goto EXIT;
	} 	

	iStatus = setsockopt(ua_socket->tcp.control_socket, IPPROTO_TCP, TCP_NODELAY, &enable, sizeof(enable));
	if (iStatus < 0) {
		ua_printf(UA_ERROR, "tcp chat socket set opt TCP_NODELAY error! ");
		goto EXIT;
	}

	iStatus = setsockopt(ua_socket->tcp.control_socket, SOL_SOCKET, SO_KEEPALIVE, &enable, sizeof(enable));
	if (iStatus < 0) {
		ua_printf(UA_ERROR, "tcp chat socket set opt SO_KEEPALIVE error! ");
		goto EXIT;
	}	

#if LWIP_SO_SNDTIMEO
	iStatus = setsockopt(ua_socket->tcp.control_socket, SOL_SOCKET, SO_SNDTIMEO, &send_timeout, sizeof(send_timeout));
	if (iStatus < 0) {
		ua_printf(UA_ERROR, "tcp client socket set opt error! ");
		goto EXIT;
	}
#endif

#if LWIP_SO_RCVTIMEO
	iStatus = setsockopt(ua_socket->tcp.control_socket, SOL_SOCKET, SO_RCVTIMEO, &send_timeout, sizeof(send_timeout));
	if (iStatus < 0) {
		ua_printf(UA_ERROR, "tcp client socket set opt error! ");
		goto EXIT;
	}	
#endif

	ua_printf(UA_INFO, "Accept new control socket %d on port %d successfully.", ua_socket->tcp.control_socket, htons(sAddr.sin_port));				       			       			
	if(old_control_socket != -1)
	{
		close(old_control_socket);
		ua_printf(UA_INFO, "Close old control socket %d.", old_control_socket);	
	}
	
	return;

EXIT:
	if(ua_socket->tcp.control_server_listen_socket!= -1){
     	 	close(ua_socket->tcp.control_server_listen_socket);
     	 	ua_socket->tcp.control_server_listen_socket= -1;    	 
	}			
}

void uartadapter_tcp_transmit_listen_socket_handler(ua_socket_t *ua_socket)
{
	int old_transmit_recv_socket;
	struct sockaddr_in  sAddr;    
	int addrlen = sizeof(sAddr);
	int iStatus;	
	int enable = 1;
	int send_timeout = 3000;	

	UA_SOCKET_CHECK(ua_socket);	

	old_transmit_recv_socket = ua_socket->tcp.transmit_recv_socket;
	
	ua_socket->tcp.transmit_recv_socket = accept(ua_socket->tcp.transmit_server_listen_socket, (struct sockaddr *)&sAddr, (socklen_t*)&addrlen);
	if( ua_socket->tcp.transmit_recv_socket < 0 ) {
		ua_printf(UA_ERROR, "Accept tcp control socket error");
		goto EXIT;
	} 		

	iStatus = setsockopt(ua_socket->tcp.transmit_recv_socket, IPPROTO_TCP, TCP_NODELAY, &enable, sizeof(enable));
	if (iStatus < 0) {
		ua_printf(UA_ERROR, "tcp transmit recv socket set opt TCP_NODELAY error! ");
		goto EXIT;
	}

	iStatus = setsockopt(ua_socket->tcp.transmit_recv_socket, SOL_SOCKET, SO_KEEPALIVE, &enable, sizeof(enable));
	if (iStatus < 0) {
		ua_printf(UA_ERROR, "tcp transmit recv socket set opt SO_KEEPALIVE error! ");
		goto EXIT;
	}		

#if LWIP_SO_SNDTIMEO
	iStatus = setsockopt(ua_socket->tcp.transmit_recv_socket, SOL_SOCKET, SO_SNDTIMEO, &send_timeout, sizeof(send_timeout));
	if (iStatus < 0) {
		ua_printf(UA_ERROR, "tcp client socket set opt error! ");
		goto EXIT;
	}
#endif	

#if LWIP_SO_RCVTIMEO
	iStatus = setsockopt(ua_socket->tcp.transmit_recv_socket, SOL_SOCKET, SO_RCVTIMEO, &send_timeout, sizeof(send_timeout));
	if (iStatus < 0) {
		ua_printf(UA_ERROR, "tcp client socket set opt error! ");
		goto EXIT;
	}	
#endif	 

	ua_printf(UA_INFO, "Accept new transmit recv socket %d on port %d successfully.", ua_socket->tcp.transmit_recv_socket, htons(sAddr.sin_port));				       			       			
	if(old_transmit_recv_socket != -1)
	{
		close(old_transmit_recv_socket);
		ua_printf(UA_INFO, "Close old transmit recv socket %d.", old_transmit_recv_socket);	
	}
	
	return;

EXIT:
	if(ua_socket->tcp.transmit_server_listen_socket!= -1){
     	 	close(ua_socket->tcp.transmit_server_listen_socket);
     	 	ua_socket->tcp.transmit_server_listen_socket= -1;    	 
	}			
}

void uartadapter_tcp_select_restart_handler(ua_socket_t *ua_socket)
{
       if(ua_socket->tcp.chat_socket != -1){
		ua_printf(UA_INFO,"IP changed, uart tcp chat socket %d need to close!", ua_socket->tcp.chat_socket);	
		close(ua_socket->tcp.chat_socket);		
		ua_socket->tcp.chat_socket = -1;
	}
		
       if(ua_socket->tcp.control_socket != -1){
		ua_printf(UA_INFO,"IP changed, uart tcp control socket %d need to close!", ua_socket->tcp.control_socket);	
		close(ua_socket->tcp.control_socket);		
		ua_socket->tcp.control_socket = -1;	
	}	

       if(ua_socket->tcp.transmit_recv_socket != -1){
		ua_printf(UA_INFO,"IP changed, uart tcp transmit recv socket %d need to close!", ua_socket->tcp.transmit_recv_socket);	
		close(ua_socket->tcp.transmit_recv_socket);		
		ua_socket->tcp.transmit_recv_socket = -1;
		uartadapter_gpio_led_mode(ua_socket, UART_ADAPTER_LED_FAST_TWINKLE);							
	}		

       if(ua_socket->tcp.transmit_send_socket != -1){
		ua_printf(UA_INFO,"IP changed, uart tcp transmit send socket %d need to close!", ua_socket->tcp.transmit_send_socket);	
		close(ua_socket->tcp.transmit_send_socket);		
		ua_socket->tcp.transmit_send_socket = -1;
		uartadapter_gpio_led_mode(ua_socket, UART_ADAPTER_LED_FAST_TWINKLE);									
	}		   

       if(ua_socket->tcp.chat_server_listen_socket!= -1){ 
		ua_printf(UA_INFO,"IP changed, uart tcp chat server socket %d need to restart!", ua_socket->tcp.chat_server_listen_socket);	
		close(ua_socket->tcp.chat_server_listen_socket);		
		ua_socket->tcp.chat_server_listen_socket = -1;   
		uartadapter_tcpserver(ua_socket, UA_CHAT_SOCKET_PORT, 0);				
	}

       if(ua_socket->tcp.control_server_listen_socket!= -1){ 
		ua_printf(UA_INFO,"IP changed, uart tcp control server socket %d need to restart!", ua_socket->tcp.control_server_listen_socket);	
		close(ua_socket->tcp.control_server_listen_socket);		
		ua_socket->tcp.control_server_listen_socket = -1;   
		uartadapter_tcpserver(ua_socket, UA_CONTROL_SOCKET_PORT, 1);		
	}     

       if(ua_socket->tcp.transmit_server_listen_socket!= -1){ 
		ua_printf(UA_INFO,"IP changed, uart tcp transmit server socket %d need to close!", ua_socket->tcp.transmit_server_listen_socket);	
		close(ua_socket->tcp.transmit_server_listen_socket);		
		ua_socket->tcp.transmit_server_listen_socket = -1;
		uartadapter_gpio_led_mode(ua_socket, UART_ADAPTER_LED_FAST_TWINKLE);									
	}    	
	   
}

void uartadapter_tcp_select_thread(void *param)
{
	ua_socket_t *ua_socket = (ua_socket_t *)param;
	int max_fd;	
	struct timeval tv;
    	fd_set readfds;
    	fd_set exceptfds;		
   	int ret = 0;
    	char *tcp_rxbuf;

	UA_SOCKET_CHECK(ua_socket);		

	tcp_rxbuf = pvPortMalloc(UA_UART_FRAME_LEN);
	if(NULL == tcp_rxbuf){
		ua_printf(UA_ERROR, "Allocate select buffer failed.\n");
		return;
	}	
	
	while(1){
		if(ua_reconnect_ip_change){
			uartadapter_tcp_select_restart_handler(ua_socket);	
			ua_reconnect_ip_change = 0;
		}
		
        	FD_ZERO(&readfds);
        	FD_ZERO(&exceptfds);					
	 	max_fd = -1; 	
		
	 	if(ua_socket->tcp.chat_socket != -1){
	         	FD_SET(ua_socket->tcp.chat_socket, &readfds);	
	         	FD_SET(ua_socket->tcp.chat_socket, &exceptfds);						
			if(ua_socket->tcp.chat_socket > max_fd)
				max_fd = ua_socket->tcp.chat_socket;				
		}	
		
	 	if(ua_socket->tcp.control_socket != -1){
	         	FD_SET(ua_socket->tcp.control_socket, &readfds);	
	         	FD_SET(ua_socket->tcp.control_socket, &exceptfds);						
			if(ua_socket->tcp.control_socket > max_fd)
				max_fd = ua_socket->tcp.control_socket;				
		}	

	 	if(ua_socket->tcp.control_server_listen_socket != -1){
	         	FD_SET(ua_socket->tcp.control_server_listen_socket, &readfds);		
	         	FD_SET(ua_socket->tcp.control_server_listen_socket, &exceptfds);						
			if(ua_socket->tcp.control_server_listen_socket > max_fd)
				max_fd = ua_socket->tcp.control_server_listen_socket;				
		}	

	 	if(ua_socket->tcp.chat_server_listen_socket != -1){
	         	FD_SET(ua_socket->tcp.chat_server_listen_socket, &readfds);	
	         	FD_SET(ua_socket->tcp.chat_server_listen_socket, &exceptfds);						
			if(ua_socket->tcp.chat_server_listen_socket > max_fd)
				max_fd = ua_socket->tcp.chat_server_listen_socket;				
		}	

	 	if(ua_socket->tcp.transmit_recv_socket != -1){
	         	FD_SET(ua_socket->tcp.transmit_recv_socket, &readfds);		
	         	FD_SET(ua_socket->tcp.transmit_recv_socket, &exceptfds);						
			if(ua_socket->tcp.transmit_recv_socket > max_fd)
				max_fd = ua_socket->tcp.transmit_recv_socket;				
		}		

	 	if(ua_socket->tcp.transmit_send_socket != -1){
	         	FD_SET(ua_socket->tcp.transmit_send_socket, &exceptfds);						
			if(ua_socket->tcp.transmit_send_socket > max_fd)
				max_fd = ua_socket->tcp.transmit_send_socket;				
		}			

	 	if(ua_socket->tcp.transmit_server_listen_socket != -1){
	         	FD_SET(ua_socket->tcp.transmit_server_listen_socket, &readfds);	
	         	FD_SET(ua_socket->tcp.transmit_server_listen_socket, &exceptfds);						
			if(ua_socket->tcp.transmit_server_listen_socket> max_fd)
				max_fd = ua_socket->tcp.transmit_server_listen_socket;				
		}			
	 
       	tv.tv_sec = 1;
       	tv.tv_usec = 0;

       	ret = select(max_fd + 1, &readfds, NULL, &exceptfds, &tv);

      		if(ua_debug_print_en){
	       	ua_printf(UA_INFO, "uart adapter test select ret = %x",ret);	
        	}	 
        	if(ret > 0){
#if UA_PS_ENABLE	
			//printf("select get lock \r\n");
			pmu_acquire_wakelock(UA_WAKELOCK);	
			ua_socket->tcp.tcp_ps = 0;
			ua_socket->tcp.tcp_ps_cnt = 0;
#endif
				
			uartadapter_tcp_except_handler(ua_socket, &exceptfds);									
				
                	if(ua_socket->tcp.chat_socket != -1 && FD_ISSET(ua_socket->tcp.chat_socket, &readfds)){
				uartadapter_tcp_chat_socket_handler(ua_socket, tcp_rxbuf);			
			}
				
                	if(ua_socket->tcp.control_socket != -1 && FD_ISSET(ua_socket->tcp.control_socket, &readfds)){
				uartadapter_tcp_control_socket_handler(ua_socket);			
			}	

                	if(ua_socket->tcp.transmit_recv_socket != -1 && FD_ISSET(ua_socket->tcp.transmit_recv_socket, &readfds)){
				uartadapter_tcp_transmit_socket_handler(ua_socket, tcp_rxbuf);			
			}					

                  	if(ua_socket->tcp.chat_server_listen_socket!= -1 && FD_ISSET(ua_socket->tcp.chat_server_listen_socket, &readfds)){ 
				uartadapter_tcp_chat_listen_socket_handler(ua_socket);
        		}

                  	if(ua_socket->tcp.control_server_listen_socket!= -1 && FD_ISSET(ua_socket->tcp.control_server_listen_socket, &readfds)){ 
				uartadapter_tcp_control_listen_socket_handler(ua_socket);
        		}     

                  	if(ua_socket->tcp.transmit_server_listen_socket!= -1 && FD_ISSET(ua_socket->tcp.transmit_server_listen_socket, &readfds)){ 
				uartadapter_tcp_transmit_listen_socket_handler(ua_socket);
        		}    	


		}
#if UA_PS_ENABLE			
		else{
			ua_socket->tcp.tcp_ps_cnt++;
			if(ua_socket->tcp.tcp_ps_cnt > 5){
				ua_socket->tcp.tcp_ps_cnt = 5;				
				ua_socket->tcp.tcp_ps = 1;		
				if(ua_socket->uart.uart_ps && ua_socket->tcp.tcp_ps){
					pmu_release_wakelock(UA_WAKELOCK);	
				}
			}
		}
#endif		
	}

	//vTaskDelete(NULL);	
}

#define _________WIFI__RELATED________________________

int uartadapter_connect_wifi(rtw_network_info_t *p_wifi, uint32_t channel, uint8_t pscan_config)
{
	int retry = 3;
	rtw_wifi_setting_t setting;	
	int ret;
	while (1) {
		if(wifi_set_pscan_chan((uint8_t *)&channel, &pscan_config, 1) < 0){
			ua_printf(UA_ERROR, "wifi set partial scan channel fail");
			ret = SC_TARGET_CHANNEL_SCAN_FAIL;
			return ret;
		}

		ret = wifi_connect((char*)p_wifi->ssid.val,
					  p_wifi->security_type,
					  (char*)p_wifi->password,
					  p_wifi->ssid.len,
					  p_wifi->password_len,
					  p_wifi->key_id,
					  NULL);

		if (ret == RTW_SUCCESS) {
			ret = LwIP_DHCP(0, DHCP_START);	
			wifi_get_setting(WLAN0_NAME, &setting);
			wifi_show_setting(WLAN0_NAME, &setting);		
			if (ret == DHCP_ADDRESS_ASSIGNED)
				return SC_SUCCESS;
			else
				return SC_DHCP_FAIL;			
		}

		if (retry == 0) {
			ret = SC_JOIN_BSS_FAIL;
			break;
		}
		retry --;
	}

	return ret;
}
	

static int uartadapter_load_wifi_config()
{
	flash_t		flash;
	struct wlan_fast_reconnect	*data;
	uint32_t	channel;
	uint8_t     pscan_config;
	char key_id;
	rtw_network_info_t wifi = {0};
	int ret = SC_SUCCESS;
	

	data = (struct wlan_fast_reconnect *)rtw_zmalloc(sizeof(struct wlan_fast_reconnect));
	device_mutex_lock(RT_DEV_LOCK_FLASH);
	flash_stream_read(&flash, FAST_RECONNECT_DATA, sizeof(struct wlan_fast_reconnect), (uint8_t *)data);
	device_mutex_unlock(RT_DEV_LOCK_FLASH);
	if(*((uint32_t *) data) != ~0x0){
	    memcpy(psk_essid, data->psk_essid, sizeof(data->psk_essid));
	    memcpy(psk_passphrase, data->psk_passphrase, sizeof(data->psk_passphrase));
	    memcpy(wpa_global_PSK, data->wpa_global_PSK, sizeof(data->wpa_global_PSK));
	    memcpy(&channel, &(data->channel), 4);
	    sprintf(&key_id,"%d",(channel >> 28));
	    channel &= 0xff;	    
	    pscan_config = PSCAN_ENABLE | PSCAN_FAST_SURVEY;
	    //set partial scan for entering to listen beacon quickly
	    //wifi_set_pscan_chan((uint8_t *)&channel, &pscan_config, 1);
	    
	    //set wifi connect
	    wifi.ssid.len = (int)strlen((char*)psk_essid);
	    memcpy(wifi.ssid.val, psk_essid, wifi.ssid.len);
	    wifi.key_id = key_id;
	    
	    //open mode
	    if(!strlen((char*)psk_passphrase)){
		wifi.security_type = RTW_SECURITY_OPEN;
	    }
	    //wep mode
	    else if( strlen((char*)psk_passphrase) == 5 || strlen((char*)psk_passphrase) == 13){
		wifi.security_type = RTW_SECURITY_WEP_PSK;
		wifi.password = (unsigned char *)psk_passphrase;
		wifi.password_len = (int)strlen((char const *)psk_passphrase);
	    }
	    //WPA/WPA2
	    else{
		wifi.security_type = RTW_SECURITY_WPA2_AES_PSK;
		wifi.password = (unsigned char *)psk_passphrase;
		wifi.password_len = (int)strlen((char const *)psk_passphrase);
	    }
	    
	    ret = uartadapter_connect_wifi(&wifi, channel, pscan_config);

	    //print_simple_config_result((enum ua_sc_result)ret);

	    if(data)
	        rtw_mfree((u8*)data, sizeof(sizeof(struct wlan_fast_reconnect)));

	    if(ret == SC_SUCCESS)
	    	return RTW_SUCCESS;
	    else
	    	return RTW_ERROR;
	}else{
	    ua_printf(UA_INFO, "No AP Profile read from FLASH, start simple configure");                     			

	    if(data)
	        rtw_mfree((u8*)data, sizeof(sizeof(struct wlan_fast_reconnect)));

	    return RTW_ERROR;
	}
}

#if CONFIG_INCLUDE_SIMPLE_CONFIG
int uartadapter_simple_config(char *pin_code){
	char *custom_pin_code = NULL;
	enum sc_result ret = SC_ERROR;

	simple_config_terminate = 0;
	
#if !UA_SC_SOFTAP_EN
    wifi_enter_promisc_mode();
#endif

	if(init_test_data(custom_pin_code) == 0){
	
#if !UA_SC_SOFTAP_EN	
	    filter_add_enable(); 
#endif	    
		ret = simple_config_test(NULL);
		deinit_test_data();
		
#if !UA_SC_SOFTAP_EN		
		remove_filter();
#endif		
		print_simple_config_result(ret);
	}
	if(ret == SC_SUCCESS){
		return RTW_SUCCESS;
	}
	else{
		return RTW_ERROR;
	}
}
#endif	

#define _________MDNS__RELATED________________________
static void uartadapter_mdns_start(ua_socket_t *ua_socket, int is_restart)
{
	TXTRecordRef txtRecord;
	unsigned char txt_buf[100] = {0};	// use fixed buffer for text record to prevent malloc/free
	unsigned char txt_buf2[100] = {0};	// use fixed buffer for text record to prevent malloc/free
	struct sockaddr_in server_addr;
	int server_addr_len = sizeof(server_addr);
	
	struct netif * pnetif = &xnetif[0];	

	if(is_restart){
		ua_printf(UA_INFO, "Uart Adapter mDNS Restart");
		mDNSResponderDeinit();
		vTaskDelay(1000);		
	}

	ua_printf(UA_DEBUG, "Uart Adapter mDNS Init");

	if(mDNSResponderInit() == 0) {
		ua_printf(UA_INFO, "mDNS Register service");
				char hostname[32] = {0};  //AMEBA_UART-MODE
				u32 prefix_len = strlen("AMEBA_");
				sprintf(hostname, "AMEBA_");
				sprintf(hostname+prefix_len, "%02x%02x%02x%02x%02x%02x",
				pnetif->hwaddr[0], pnetif->hwaddr[1], pnetif->hwaddr[2],
				pnetif->hwaddr[3], pnetif->hwaddr[4], pnetif->hwaddr[5]);

		sprintf(txt_buf2, "groupid:%d, tcpserver:%d", ua_socket->tcp.group_id, ua_socket->tcp.server_port);						
		TXTRecordCreate(&txtRecord, sizeof(txt_buf), txt_buf);
		TXTRecordSetValue(&txtRecord, "groupid", strlen(txt_buf2), txt_buf2);		
				
		ua_socket->dnsServiceRef = mDNSRegisterService(hostname, "_uart_data._tcp", "local", 5001, &txtRecord);
		if(ua_socket->dnsServiceRef == NULL)
			ua_printf(UA_ERROR, "mdns data service register failed!");
		ua_socket->dnsServiceRef2 = mDNSRegisterService(hostname, "_uart_control._tcp", "local", 6001, &txtRecord);		
		if(ua_socket->dnsServiceRef2 == NULL)
			ua_printf(UA_ERROR, "mdns control service register failed!");
		TXTRecordDeallocate(&txtRecord);		

	}else{
		ua_printf(UA_INFO, "mDNS Init Failed");	
	}

	//vTaskDelete(NULL);
}



#define _________INIT__RELATED________________________
static void uartadapter_auto_reconnect(void* param)
{
	ua_socket_t *ua_socket = (ua_socket_t *)param;
	uint32_t IPaddress, IPaddress2;
	uint8_t iptab[4], iptab2[4];	
	//int ret = 0;	

	ua_reconnect_started = 1;

#if CONFIG_AUTO_RECONNECT
	wifi_set_autoreconnect(0);
#endif		
	vTaskDelay(1000);
	while(uartadapter_load_wifi_config() != RTW_SUCCESS){
		vTaskDelay(2000);		
	}
#if CONFIG_AUTO_RECONNECT
	wifi_set_autoreconnect(1);
#endif		
	
	ua_printf(UA_INFO, "uart adapter reconnect successful");	
	//uartadapter_gpio_led_mode(ua_global_socket, UART_ADAPTER_LED_OFF);			
	
	if(0 == memcmp(&ua_socket->ip, &xnetif[0].ip_addr, sizeof(ip_addr_t))){
		IPaddress = ua_socket->ip.addr;
		iptab[0] = (uint8_t)(IPaddress >> 24);
		iptab[1] = (uint8_t)(IPaddress >> 16);
		iptab[2] = (uint8_t)(IPaddress >> 8);
		iptab[3] = (uint8_t)(IPaddress);		
		ua_printf(UA_INFO, "reconnect IP address no change, %d.%d.%d.%d", iptab[3], iptab[2], iptab[1], iptab[0]);		
	}else{
		IPaddress = ua_socket->ip.addr;
		iptab[0] = (uint8_t)(IPaddress >> 24);
		iptab[1] = (uint8_t)(IPaddress >> 16);
		iptab[2] = (uint8_t)(IPaddress >> 8);
		iptab[3] = (uint8_t)(IPaddress);	
		IPaddress2 = xnetif[0].ip_addr.addr;
		iptab2[0] = (uint8_t)(IPaddress2 >> 24);
		iptab2[1] = (uint8_t)(IPaddress2 >> 16);
		iptab2[2] = (uint8_t)(IPaddress2 >> 8);
		iptab2[3] = (uint8_t)(IPaddress2);		
		ua_printf(UA_INFO, "reconnect IP address changed from %d.%d.%d.%d to %d.%d.%d.%d", iptab[3], iptab[2], iptab[1], iptab[0], iptab2[3], iptab2[2], iptab2[1], iptab2[0]);		
		memcpy(&ua_socket->ip, &xnetif[0].ip_addr, sizeof(ip_addr_t));
		ua_reconnect_ip_change = 1;	

 		ua_printf(UA_INFO,"IP changed, remove TCP information in FLASH!");	
		ua_socket->tcp.group_id = 0;
		ua_socket->tcp.client_port = 0;	
		ua_socket->tcp.server_port = 0;	
		memset(ua_socket->tcp.client_ip, 0, 16);
		
		uartadapter_mdns_start(ua_socket, 1);
	}
	

	ua_reconnect_started = 0;	
	vTaskDelete(NULL);	
}

void uartadapter_auto_connect(void *param)
{
	int ret = 0;
	ua_socket_t *ua_socket = (ua_socket_t *)param;

	UA_SOCKET_CHECK(ua_socket);	
	
	if(wifi_is_ready_to_transceive(RTW_STA_INTERFACE) == RTW_SUCCESS) {
		ua_printf(UA_INFO, "wifi connect successfully!");                     		
	}else{
#if CONFIG_AUTO_RECONNECT
	    	wifi_set_autoreconnect(0);
#endif	
RETRY:	
		vTaskDelay(2000);	
		ret = uartadapter_load_wifi_config();
		if(ret != RTW_SUCCESS){
			vTaskDelay(2000);			
#if CONFIG_INCLUDE_SIMPLE_CONFIG
       		ret = uartadapter_simple_config(NULL);
       		if(ret != RTW_SUCCESS){
				ua_printf(UA_INFO, "Simple configure connect failed, try again!");                     		
       			goto RETRY;
       		}
#endif
		}		
#if CONFIG_AUTO_RECONNECT
	    	wifi_set_autoreconnect(1);
#endif		
	}

	ua_wifi_connected = 1;
	//uartadapter_gpio_led_mode(ua_global_socket, UART_ADAPTER_LED_OFF);			
	
	memcpy(&ua_socket->ip, &xnetif[0].ip_addr, sizeof(ip_addr_t));
	
	if(!sys_thread_new("tcp data server", uartadapter_tcp_chat_server_thread,  (void *)ua_socket, 256, UA_UART_THREAD_PRIORITY)) 
  	    ua_printf(UA_ERROR, "%s sys_thread_new data server failed\n", __FUNCTION__);

	vTaskDelay(50);
	if(!sys_thread_new("tcp control server", uartadapter_tcp_control_server_thread,  (void *)ua_socket, 256, UA_UART_THREAD_PRIORITY)) 
  	    ua_printf(UA_ERROR, "%s sys_thread_new control server failed\n", __FUNCTION__);

	vTaskDelay(50);

	if(!sys_thread_new("tcp select", uartadapter_tcp_select_thread,  (void *)ua_socket, 768, UA_UART_THREAD_PRIORITY)) 
  	    ua_printf(UA_ERROR, "%s sys_thread_new tcp select failed\n", __FUNCTION__);

	vTaskDelay(50);

      	uartadapter_control_read_tcp_info_and_connect(ua_socket);

	vTaskDelay(50);	

      	uartadapter_mdns_start(ua_socket, 0);

	vTaskDelay(50);
	
	ua_printf(UA_INFO, "[MEM] After auao connect, available heap %d", xPortGetFreeHeapSize());

	/* Kill init thread after all init tasks done */
	vTaskDelete(NULL);       
}

void uartadapter_exception_thread(void *param)
{
	int ret = 0;
	unsigned int tick_start;
	unsigned int tick_current;
	int pin_high = 0;
	int address = FAST_RECONNECT_DATA;		

	ua_socket_t *ua_socket = (ua_socket_t *)param;
	
	while(rtw_down_sema(&ua_exception_sema) == pdTRUE){	
		if(ua_gpio_irq_happen){
			pin_high = 0;
			tick_start = xTaskGetTickCount();
			tick_current = xTaskGetTickCount();			
			while(tick_current - tick_start < 3000){
				if (gpio_read(&ua_socket->gpio.gpio_btn)){
					pin_high = 1;
					break;
				}else{
					tick_current = xTaskGetTickCount();
				}
				vTaskDelay(10);			
			}
			
			ua_gpio_irq_happen = 0;
			if(!pin_high){
				ret = uartadapter_flasherase(address, sizeof(rtw_wifi_config_t));
				if(ret < 0){
					ua_printf(UA_ERROR, "flash erase error!");
				}

				ret = uartadapter_flasherase(UA_FAST_RECONNECT_TCP_DATA, sizeof(ua_tcp_socket_t));
				if(ret < 0){
					ua_printf(UA_ERROR, "flash erase error!");
				}				
		
				uartadapter_systemreload();			
			}			
		}		
	}

	vTaskDelete(NULL);       
}

ua_socket_t* uartadapter_socket_init()
{
	ua_socket_t*	ua_socket = NULL;	
	//int ret = 0;
	
	ua_socket = pvPortMalloc(sizeof(ua_socket_t));
	
	if(NULL == ua_socket){
		ua_printf(UA_ERROR, "Allocate uart adapter socket failed.");
		goto Exit2;
	}
	
	ua_socket->uart.rcv_ch = 0;
	ua_socket->uart.overlap = 0;
	ua_socket->uart.recv_bytes = 0;
	ua_socket->uart.pread = 0;
	ua_socket->uart.pwrite = 0;
	ua_socket->uart.tick_last_update = 0;
	ua_socket->uart.tick_current = 0;
	ua_socket->uart.rx_cnt = 0;
	ua_socket->uart.miss_cnt = 0;
	ua_socket->uart.tx_busy = 0;	
	rtw_init_sema(&ua_socket->uart.action_sema, 0);
	rtw_init_sema(&ua_socket->uart.tcp_tx_rx_sema, 1);
	rtw_init_sema(&ua_socket->uart.dma_tx, 1);		

	ua_socket->tcp.chat_socket= -1;
	ua_socket->tcp.control_socket= -1;
	ua_socket->tcp.chat_server_listen_socket= -1;
	ua_socket->tcp.control_server_listen_socket= -1;

	ua_socket->tcp.transmit_recv_socket = -1;
	ua_socket->tcp.transmit_send_socket = -1;
	ua_socket->tcp.transmit_server_listen_socket = -1;

	ua_socket->tcp.group_id = 0;
	ua_socket->tcp.server_port = 0;
	ua_socket->tcp.client_port = 0;
	memset(ua_socket->tcp.client_ip, 0, 16);	
	
	ua_socket->tcp.send_flag = 0;
	ua_socket->tcp.recv_flag = 0;
	ua_socket->tcp.rx_cnt = 0;	
	ua_socket->tcp.tx_cnt = 0;

	ua_socket->uart.uart_ps = 0;
	ua_socket->uart.uart_ps_cnt = 0;	
	ua_socket->tcp.tcp_ps = 0;
	ua_socket->tcp.tcp_ps_cnt = 0;

	ua_socket->dnsServiceRef = NULL;
	ua_socket->dnsServiceRef2 = NULL;

	return ua_socket;

Exit2:	
	return NULL;

}

void uartadapter_disconnect_handler(char *buf, int buf_len, int flags, void *userdata)
{
	if(!ua_wifi_connected || ua_reconnect_started)
		return;
	
	ua_printf(UA_DEBUG, "start uart adapter reconnect thread\r\n");
	//uartadapter_gpio_led_mode(ua_global_socket, UART_ADAPTER_LED_FAST_TWINKLE);			
	
	if(xTaskCreate(uartadapter_auto_reconnect, ((const char*)"reconnect"), 1024, (void *)ua_global_socket, UA_UART_THREAD_PRIORITY, NULL) != pdPASS)
		ua_printf(UA_ERROR, "%s xTaskCreate(uart_rx) failed", __FUNCTION__);

}

int uartadapter_init()
{
	int ret = 0;
	if(ua_started)
		return 0;
	
	ua_started = 1;
	
	rtw_init_sema(&ua_print_sema, 1);

	ua_printf(UA_INFO, "==============>%s()\n", __func__);

	ua_global_socket = uartadapter_socket_init();
	if(ua_global_socket == NULL){
		ua_printf(UA_ERROR, "ua_socket init failed");
		ret = -1;
		goto Exit;
	}

#if !UA_PS_ENABLE
      pmu_acquire_wakelock(UA_WAKELOCK);
#endif

	rtw_init_sema(&ua_exception_sema, 0);	
	
	uartadapter_uart_init(ua_global_socket);

	uartadapter_gpio_init(ua_global_socket);	
	//uartadapter_gpio_led_mode(ua_global_socket, UART_ADAPTER_LED_FAST_TWINKLE);			

	wifi_reg_event_handler(WIFI_EVENT_DISCONNECT, uartadapter_disconnect_handler, NULL);

	if(xTaskCreate(uartadapter_uart_rx_thread, ((const char*)"uart_rx"), 256, (void *)ua_global_socket, UA_UART_THREAD_PRIORITY, NULL) != pdPASS)
		ua_printf(UA_ERROR, "%s xTaskCreate(uart_rx) failed", __FUNCTION__);

	vTaskDelay(50);
	
	if(xTaskCreate(uartadapter_auto_connect, ((const char*)"auto connnect"), 1024, (void *)ua_global_socket, UA_UART_THREAD_PRIORITY, NULL) != pdPASS)
		ua_printf(UA_ERROR, "%s xTaskCreate(auao connnect) failed", __FUNCTION__);

	vTaskDelay(50);
	
	if(xTaskCreate(uartadapter_exception_thread, ((const char*)"uart main"), 128, (void *)ua_global_socket, UA_UART_THREAD_PRIORITY, NULL) != pdPASS)
		ua_printf(UA_ERROR, "%s xTaskCreate(auao connnect) failed", __FUNCTION__);
	
	return 0;
Exit:
	ua_printf(UA_ERROR, "%s(): Initialization failed!", __func__);
	return ret;
}

void example_uart_adapter_init()
{
	// Call back from wlan driver after wlan init done
	p_wlan_uart_adapter_callback = uartadapter_init;
}

#define _________CMD__RELATED________________________
void uartadapter_print_irq_rx_count(ua_socket_t *ua_socket)
{
	UA_SOCKET_CHECK(ua_socket);	

	ua_printf(UA_INFO, "ua_tick_last_update: %d!\n", ua_socket->uart.tick_last_update);
	ua_printf(UA_INFO, "ua_tick_current: %d!\n", ua_socket->uart.tick_current);	
	ua_printf(UA_INFO, "ua current tick: %d!\n", xTaskGetTickCount());	
	ua_printf(UA_INFO, "ua_pwrite: %d!\n", ua_socket->uart.pwrite);	
	ua_printf(UA_INFO, "ua_pread: %d!\n", ua_socket->uart.pread);	
	ua_printf(UA_INFO, "ua_overlap: %d!\n", ua_socket->uart.overlap);	
	ua_printf(UA_INFO, "ua_rcv_ch: %d!\n", ua_socket->uart.rcv_ch);	
	ua_printf(UA_INFO, "ua_uart_recv_bytes: %d!\n", ua_socket->uart.recv_bytes);	
	ua_printf(UA_INFO, "irq_rx_cnt: %d!\n", ua_socket->uart.rx_cnt);
	ua_printf(UA_INFO, "irq_miss_cnt: %d!\n", ua_socket->uart.miss_cnt);		
	ua_printf(UA_INFO, "tcp_tx_cnt: %d!\n", ua_socket->tcp.tx_cnt);
	ua_printf(UA_INFO, "tcp_rx_cnt: %d!\n", ua_socket->tcp.rx_cnt);		
	ua_printf(UA_INFO, "tcp_send_flag: %d!\n", ua_socket->tcp.send_flag);
	ua_printf(UA_INFO, "tcp_recv_flag: %d!\n", ua_socket->tcp.recv_flag);		
	ua_printf(UA_INFO, "tcp_chat_socket: %d!\n", ua_socket->tcp.chat_socket);
	ua_printf(UA_INFO, "tcp_control_socket: %d!\n", ua_socket->tcp.control_socket);
	ua_printf(UA_INFO, "tcp_transmit_recv_socket: %d!\n", ua_socket->tcp.transmit_recv_socket);
	ua_printf(UA_INFO, "tcp_transmit_send_socket: %d!\n", ua_socket->tcp.transmit_send_socket);	
	ua_printf(UA_INFO, "tcp_chat_server_listen_socket: %d!\n", ua_socket->tcp.chat_server_listen_socket);	
	ua_printf(UA_INFO, "tcp_control_server_listen_socket: %d!\n", ua_socket->tcp.control_server_listen_socket);	
	ua_printf(UA_INFO, "tcp_transmit_server_listen_socket: %d!\n", ua_socket->tcp.transmit_server_listen_socket);	

}

void uartadapter_reset_irq_rx_count(ua_socket_t *ua_socket)
{
	UA_SOCKET_CHECK(ua_socket);	

	ua_socket->uart.rx_cnt = 0;
	ua_socket->uart.miss_cnt = 0;	
	ua_socket->tcp.rx_cnt = 0;
	ua_socket->tcp.tx_cnt = 0;	
}

void uartadapter_set_debug_print(bool enable)
{
	ua_debug_print_en = enable;
}

void cmd_uart_adapter(int argc, char **argv)
{
	if(argc < 2) {
		printf("\n\r inpua error\n");
		return;
	}

	if(strcmp(argv[1], "help") == 0){
		printf("\r\nUART THROUGH COMMAND LIST:");
		printf("\r\n==============================");
		printf("\r\n\tuart_baud");		
		printf("\r\n");	
	}else if(strcmp(argv[1], "irq_rx_get") == 0){
		uartadapter_print_irq_rx_count(ua_global_socket);
       }else if(strcmp(argv[1], "debug_print_en") == 0){
		uartadapter_set_debug_print(TRUE);
	}else if(strcmp(argv[1], "debug_print_dis") == 0){
		uartadapter_set_debug_print(FALSE);
       }else if(strcmp(argv[1], "irq_rx_reset") == 0){
		uartadapter_reset_irq_rx_count(ua_global_socket);	
	}else if(strcmp(argv[1], "tcp") == 0){
		unsigned int port;
		if(strcmp(argv[2], "-c") == 0 || strcmp(argv[2], "c") == 0){
			strncpy(ua_tcp_server_ip, argv[3], (strlen(argv[3])>16)?16:strlen(argv[3]));
			port = atoi(argv[4]);
			if(xTaskCreate(uartadapter_tcp_transmit_client_thread, ((const char*)"tclient"), 256, (void *)port, UA_UART_THREAD_PRIORITY, NULL) != pdPASS)
				ua_printf(UA_ERROR, "%s xTaskCreate(tcp client) failed", __FUNCTION__);		
		}else if(strcmp(argv[2], "-s") == 0 || strcmp(argv[2], "s") == 0){
			port = atoi(argv[3]);	
			if(xTaskCreate(uartadapter_tcp_transmit_server_thread, ((const char*)"tserver"), 256, (void *)port, UA_UART_THREAD_PRIORITY, NULL) != pdPASS)
				ua_printf(UA_ERROR, "%s xTaskCreate(tcp server) failed", __FUNCTION__);					
		}
	}else if(strcmp(argv[1], "uart_baud") == 0){
		uartadapter_uart_baud(ua_global_socket, atoi(argv[2]));	
	}else if(strcmp(argv[1], "uart_para") == 0){
		uartadapter_uart_para(ua_global_socket, atoi(argv[2]), atoi(argv[3]), atoi(argv[4]));		
	}else if(strcmp(argv[1], "led") == 0){
		//uartadapter_gpio_led_mode(ua_global_socket, atoi(argv[2]));		
	}else{
		printf("\n\rCan not find the input commond!");
	}
}

