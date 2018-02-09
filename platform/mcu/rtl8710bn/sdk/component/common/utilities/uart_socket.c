#include "lwip/api.h"
#include "PinNames.h"
#include "sockets.h"
#include "uart_socket.h"
#include "autoconf.h"
#define UART_SOCKET_USE_DMA_TX 1
/***********************************************************************
 *                                Macros                               *
 ***********************************************************************/
#define uart_printf printf
#define uart_print_data(x, d, l) \
do{\
	int i;\
	uart_printf("\n%s: Len=%d\n", (x), (l));\
	for(i = 0; i < (l); i++)\
		uart_printf("%02x ", (d)[i]);\
	uart_printf("\n");\
}while(0);

#if defined(CONFIG_PLATFORM_8195A)
#define UART_TX  PA_7	//PA_4
#define UART_RX  PA_6	//PA_0
#elif defined(CONFIG_PLATFORM_8711B)
#define UART_TX  PA_23
#define UART_RX  PA_18
#endif
/************************************************************************
 *                        extern funtions                               *
 ************************************************************************/
extern void lwip_selectevindicate(int fd);
extern void lwip_setsockrcvevent(int fd, int rcvevent);
extern int lwip_allocsocketsd();

/*************************************************************************
*                               uart releated  fuantions                          *
*************************************************************************/
static void uart_irq(uint32_t id, SerialIrq event)
{
	uart_socket_t *u = (uart_socket_t *)id;

	if(event == RxIrq) {
		if( u->rx_start == 0 ){
			rtw_up_sema_from_isr(&u->action_sema);	//up action semaphore 
			u->rx_start = 1; // set this flag in uart_irq to indicate data recved
		}
		u->recv_buf[u->prxwrite++] = serial_getc(&u->sobj);
		if(u->prxwrite > (UART_RECV_BUFFER_LEN -1)){	//restart from  head if  reach tail
			u->prxwrite = 0;
			u->rxoverlap = 1;							//set overlap indicated that overlaped 
		}
		if(u->rxoverlap && (u->prxwrite + 1) > u->prxread ){
			u->prxread = u->prxwrite;		//if pwrite overhead pread ,pread is always flow rwrite 
		}
		u->last_update =  xTaskGetTickCountFromISR();	// update tick everytime recved data
	}

	if(event == TxIrq){
	}
}

static void uart_send_stream_done(uint32_t id)
{
	uart_socket_t *u = (uart_socket_t *)id;
	
	//u->tx_start = 0;
	memset(u->send_buf,0, UART_SEND_BUFFER_LEN);	//zero set uart_send_buf
	rtw_up_sema_from_isr(&u->tx_sema);
	rtw_up_sema_from_isr(&u->dma_tx_sema);
}

static int uart_send_stream(uart_socket_t *u, char* pbuf, int len)
{
	int ret;
	
	if(!len || (!pbuf) || !u){
		uart_printf("input error,size should not be null\r\n");
		return -1;
	}

#if UART_SOCKET_USE_DMA_TX
	while(rtw_down_sema(&u->dma_tx_sema) == pdTRUE){			
    	ret = serial_send_stream_dma(&u->sobj, pbuf, len);
    	if(ret != HAL_OK){
			rtw_up_sema(&u->dma_tx_sema);
			return -1;
		}else{
			return 0;
    	}
	}
#else
	while (len){
		serial_putc(&u->sobj, *pbuf);
       	len--;
		pbuf++;
	}
#endif	

	return 0;
}

static s32 uart_wait_rx_complete(uart_socket_t *u)
{
	s32 tick_current = xTaskGetTickCount();

	while((tick_current -u->last_update) < UART_MAX_DELAY_TIME ){
		vTaskDelay(5);
		tick_current = xTaskGetTickCount();
	}	
	return 0;
}

static void uart_action_handler(void* param)
{
	uart_socket_t *u = (uart_socket_t*)param;
	if(!u)
		goto Exit;
	
	while(rtw_down_sema(&u->action_sema) == pdTRUE) {
		if(u->fd == -1)
			goto Exit;
		if(u->rx_start){
			/* Blocked here to wait uart rx data completed */
			uart_wait_rx_complete(u);

			/* As we did not register netconn callback function.,so call lwip_selectevindicate unblocking select */
			lwip_setsockrcvevent(u->fd, 1);
			lwip_selectevindicate(u->fd);	//unblocking select()
			u->rx_start = 0;
		}
		if(u->tx_start){
#if 1			
			if (u->tx_bytes < 128) {
				uart_print_data("TX:", u->send_buf, u->tx_bytes);
			} else {
				uart_printf("\nTX:: Len=%d\n", u->tx_bytes);
			}
#endif
			u->tx_start = 0;
			if(uart_send_stream(u, (char*)u->send_buf, u->tx_bytes) == -1){
				uart_printf("uart send data error!");
			} else {
#if (UART_SOCKET_USE_DMA_TX == 0)
				memset(u->send_buf,0, UART_SEND_BUFFER_LEN);	//zero set uart_send_buf
				rtw_up_sema(&u->tx_sema);
#endif				
			}
		}
	}
Exit:	
	vTaskDelete(NULL);
}


uart_socket_t* uart_open(uart_set_str *puartpara)
{
	PinName uart_tx = UART_TX;
	PinName uart_rx = UART_RX;
	uart_socket_t *u;

	u = (uart_socket_t *)rtw_zmalloc(sizeof(uart_socket_t));
	if(!u){
		uart_printf("%s(): Alloc memory for uart_socket failed!\n", __func__);
		return NULL;
	}
	
	/*initial uart */
	serial_init(&u->sobj, uart_tx,uart_rx);
	serial_baud(&u->sobj,puartpara->BaudRate);
	serial_format(&u->sobj, puartpara->number, (SerialParity)puartpara->parity, puartpara->StopBits);

	/*uart irq handle*/
	serial_irq_handler(&u->sobj, uart_irq, (int)u);
	serial_irq_set(&u->sobj, RxIrq, 1);
	serial_irq_set(&u->sobj, TxIrq, 1);

#if UART_SOCKET_USE_DMA_TX
   	serial_send_comp_handler(&u->sobj, (void*)uart_send_stream_done, (uint32_t)u);
#endif

	/*alloc a socket*/
 	u->fd = lwip_allocsocketsd();
	if(u->fd == -1){
		uart_printf("Failed to alloc uart socket!\n");
		goto Exit2;
	}
	/*init uart related semaphore*/
	rtw_init_sema(&u->action_sema, 0);
	rtw_init_sema(&u->tx_sema, 1);
	rtw_init_sema(&u->dma_tx_sema, 1);
	
	/*create uart_thread to handle send&recv data*/
	{
#define	UART_ACTION_STACKSIZE 256 //USE_MIN_STACK_SIZE modify from 512 to 256
#define	UART_ACTION_PRIORITY	1
		if(xTaskCreate(uart_action_handler, ((const char*)"uart_action"), UART_ACTION_STACKSIZE, u, UART_ACTION_PRIORITY, NULL) != pdPASS){
			uart_printf("%s xTaskCreate(uart_action) failed", __FUNCTION__);
			goto Exit1;
		}
	}
	return u;
Exit1:
	/* Free uart related semaphore */
	rtw_free_sema(&u->action_sema);
	rtw_free_sema(&u->tx_sema);	
	rtw_free_sema(&u->dma_tx_sema);
Exit2:
	rtw_mfree((u8*)u, sizeof(uart_socket_t));
	return NULL;
}

int uart_close(uart_socket_t *u)
{
	if(!u){
		uart_printf("uart_close(): u is NULL!\r\n");
		return -1;
	}
	/* Close uart socket */
	if(lwip_close(u->fd) == -1){
		uart_printf("%s(): close uart failed!", __func__);
	}
	/* Delete uart_action task */
	u->fd = -1;
	rtw_up_sema(&u->action_sema);
	rtw_msleep_os(20);

	/* Free uart related semaphore */
	rtw_free_sema(&u->action_sema);
	rtw_free_sema(&u->tx_sema);
	rtw_free_sema(&u->dma_tx_sema);
	
	/* Free serial */
	serial_free(&u->sobj);
	
	rtw_mfree((u8 *)u, sizeof(uart_socket_t));

	return 0;
}

int uart_read(uart_socket_t *u, void *read_buf, size_t size)
{
	/*the same as socket*/
	int read_bytes = 0;
	int pread_local,pwrite_local;
	char *ptr = (char *)read_buf;

	uart_printf("==>uart_read()\n");
	if(!size || !read_buf || !u){
		uart_printf("uart_read(): input error,size should not be null\r\n");
		return -1;
	}
	
	pread_local = u->prxread;
	pwrite_local = u->prxwrite;
	/*calculate how much data not read */
	if(!u->rxoverlap){
		read_bytes = pwrite_local - pread_local;
	} else {
		read_bytes = (UART_RECV_BUFFER_LEN - pread_local) + pwrite_local;
	}
	/*decide how much data shoule copy to application*/
	if(size < read_bytes)
		read_bytes = size;
	
	if(!u->rxoverlap){
		memcpy(ptr, (u->recv_buf+ pread_local), read_bytes );
	} else {
		uart_printf("uart recv buf is write through!!\n");
		if((pread_local + read_bytes) > UART_RECV_BUFFER_LEN){
			memcpy(ptr,(u->recv_buf+ pread_local), (UART_RECV_BUFFER_LEN-pread_local));
			memcpy(ptr+(UART_RECV_BUFFER_LEN-pread_local), u->recv_buf, read_bytes-(UART_RECV_BUFFER_LEN- pread_local));
		} else
			memcpy(ptr,(u->recv_buf+ pread_local), read_bytes);
	}
	lwip_setsockrcvevent(u->fd, 0);
	
	if((pread_local + read_bytes) >= UART_RECV_BUFFER_LEN){		//update pread
		u->prxread = (pread_local + read_bytes) - UART_RECV_BUFFER_LEN;
		u->rxoverlap = 0;		//clean overlap flags
	} else
		u->prxread = pread_local + read_bytes;
	
	return read_bytes;

}


int uart_write(uart_socket_t *u, void *pbuf, size_t size)
{
	if(!size || !pbuf || !u){
		uart_printf("input error,please check!");
		return -1;
	}
	if(rtw_down_sema(&u->tx_sema)){
		//uart_printf("[%d]:uart_write %d!\n", xTaskGetTickCount(), size);
		memcpy(u->send_buf, pbuf, size);
		u->tx_bytes = size;
		u->tx_start = 1;	//set uart tx start 
		rtw_up_sema(&u->action_sema);	// let uart_handle_run through
	} else {
		uart_printf("uart write buf error!");
		return -1;
	}

	return size;
}

void uart_socket_example(void *param)
{
	char tx_data[] = {0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
	uart_set_str uartset;
	struct timeval tv;
	fd_set readfds;
	int read_len = 0, count = 0;
	int ret = 0;
	char rxbuf[512];
	int uart_fd;
	uart_socket_t *uart_socket = NULL;

	uartset.BaudRate = 9600;
	uartset.number = 8;
	uartset.StopBits = 0;
	uartset.FlowControl = 0;
	uartset.parity = 0;
	strcpy(uartset.UartName, "uart0");

	uart_socket = uart_open(&uartset);
	if(uart_socket == NULL){
		uart_printf("Init uart socket failed!\n");
		goto Exit;
	}
	uart_fd = uart_socket->fd;
	uart_printf("\nOpen uart socket: %d\n", uart_fd);
	while(1)
	{
		FD_ZERO(&readfds);
		FD_SET(uart_fd, &readfds);
		tv.tv_sec = 0;
		tv.tv_usec = 20000;
		if(count++ == 50){
			uart_write(uart_socket, tx_data, sizeof(tx_data));
			//uart_print_data("TX:", tx_data, sizeof(tx_data));
			count = 0;
		}
		ret = select(uart_fd + 1, &readfds, NULL, NULL, &tv);
		//uart_printf("[%d] select ret = %x count=%d\n", xTaskGetTickCount(), ret, count);	
		if(ret > 0)
		{
			if(FD_ISSET(uart_fd, &readfds))
			{
				read_len = uart_read(uart_socket, rxbuf, sizeof(rxbuf));
				if(read_len > 0)
				{
					uart_print_data("RX:", rxbuf, read_len);
					if(rtl_strncmp(rxbuf, "close", 5) == 0)
						break;
				}
			}
			//else for other sockets
		}
	}
	uart_printf("Exit uart socket example!\n");
	uart_close(uart_socket);
Exit:
	vTaskDelete(NULL);
}

void uart_socket()
{
#define 	UART_SOCKET_STACK_SIZE		512
#define 	UART_SOCKET_PRIORITY		1
	if(xTaskCreate(uart_socket_example, "uart_socket", UART_SOCKET_STACK_SIZE, NULL, UART_SOCKET_PRIORITY, NULL) != pdPASS)
		uart_printf("%s xTaskCreate failed", __FUNCTION__);
}

