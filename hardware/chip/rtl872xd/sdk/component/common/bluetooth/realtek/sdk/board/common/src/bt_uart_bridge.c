#include <platform_stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include "serial_api.h"
#include "semphr.h"
#include "log_service.h"
#include <stdbool.h>
#include "hci_uart.h"
#include "osdep_service.h"

#define KEY_NL			0xa // '\n'
#define KEY_ENTER		0xd // '\r'

extern char log_buf[LOG_SERVICE_BUFLEN];
extern xSemaphoreHandle log_rx_interrupt_sema;

#ifdef CONFIG_PLATFORM_8710C
extern hal_uart_adapter_t log_uart;
#endif	

static uint32_t baud_0 = 0;
//static uint32_t baud_1 = 0;
static uint8_t uart_parity = 0;
static uint8_t check_byte_num =0;

#ifdef CONFIG_PLATFORM_8721D
	static void * UartBkFunc=NULL;
#endif	

void bt_uart_tx(uint8_t tx_data)
{
#ifdef CONFIG_PLATFORM_8710C
	while (!hal_uart_writeable(&log_uart));
	hal_uart_putc(&log_uart, (u8)tx_data);
#endif


#ifdef CONFIG_PLATFORM_8721D
	while (!UART_Writable(UART2_DEV));
	UART_CharPut(UART2_DEV, (u8)tx_data);
#endif	

}

char bt_uart_bridge_getc(void)
{
#ifdef CONFIG_PLATFORM_8710C
	while(!hal_uart_readable(&log_uart));
	return (char)(hal_uart_getc (&log_uart));
#endif
#ifdef CONFIG_PLATFORM_8721D
	u8 Byte;
	while(!UART_Readable(UART2_DEV));
	UART_CharGet(UART2_DEV, &Byte);
	return Byte;
#endif

}

#ifdef CONFIG_PLATFORM_8710C
static int log_flag = 0;
extern uint32_t btc_set_single_tone_tx(uint8_t bStart);
static void _bt_uart_bridge_irq(u32 id,u32 event)
{
	unsigned char rc = 0;
	char const close_cmd_buf[]="ATM2=bridge,close";
	if(event==RxIrq){
		rc = bt_uart_bridge_getc();
		
		//if(log_flag)
			//printf("~~~~~~~~~~~set = %02x, check_byte_num = %d\r\n",rc, check_byte_num);
		//bt_uart_tx((u8)rc);//Test UART rx
		if(check_byte_num!=17){
			switch (rc)
			{				
				case 'A':
						check_byte_num=1;
					break;
				case 'T':
					if(check_byte_num==1)
						check_byte_num=2;
					else
						check_byte_num=0;
					break;
				case 'M':
					if(check_byte_num==2)
						check_byte_num=3;
					else
						check_byte_num=0;
					break;
			 	case '2':
					if(check_byte_num==3)
						check_byte_num=4;
					else
						check_byte_num=0;
					break;
				case '=':
					if(check_byte_num==4)
						check_byte_num=5;
					else
						check_byte_num=0;
					break;
				case 'b':
					if(check_byte_num==5)
						check_byte_num=6;
					else
						check_byte_num=0;
					break;
				case 'r':
					if(check_byte_num==6)
						check_byte_num=7;
					else
						check_byte_num=0;
					break;
				case 'i':
					if(check_byte_num==7)
						check_byte_num=8;
					else
						check_byte_num=0;
					break;
				case 'd':
					if(check_byte_num==8)
						check_byte_num=9;
					else
						check_byte_num=0;
					break;
				case 'g':
					if(check_byte_num==9)
						check_byte_num=10;
					else
						check_byte_num=0;
					break;
				case 'e':
					if(check_byte_num==10)
						check_byte_num=11;
					else if(check_byte_num==16)
						check_byte_num=17;
					else
						check_byte_num=0;
					break;
				case ',':
					if(check_byte_num==11)
						check_byte_num=12;
					else
						check_byte_num=0;
					break;
				case 'c':
					if(check_byte_num==12)
						check_byte_num=13;
					else
						check_byte_num=0;
					break;
				case 'l':
					if(check_byte_num==13)
						check_byte_num=14;
					else
						check_byte_num=0;
					break;
				case 'o':
					if(check_byte_num==14)
						check_byte_num=15;
					else
						check_byte_num=0;
					break;
				case 's':
					if(check_byte_num==15)
						check_byte_num=16;
					else
						check_byte_num=0;
					break;
                case 0x00: //single tone command 00
                    if(check_byte_num==23)
						check_byte_num=25;
					else
						check_byte_num=0;
                    break;
                case 0x01: //single tone command 01
                    if(check_byte_num==23)
						check_byte_num=24;
                    else if(check_byte_num==0)
                        check_byte_num=20;
					else
						check_byte_num=0;
                    break;
                case 0xfc: //single tone command fc
                    if(check_byte_num==21)
						check_byte_num=22;
					else
						check_byte_num=0;
                    break;
                case 'x': //single tone command 78
                    if(check_byte_num==20)
						check_byte_num=21;
					else
						check_byte_num=0;
					break;
                case 0x04: //single tone command 04
					if(check_byte_num==22)
						check_byte_num=23;
					else
						check_byte_num=0;
                    break;
                case 'L':
					if(check_byte_num==5)
						check_byte_num=30;
					else
						check_byte_num=0;
					break;
                case 'O':
					if(check_byte_num==30)
						check_byte_num=31;
					else
						check_byte_num=0;
					break;
                case 'G':
					if(check_byte_num==31)
						check_byte_num=32;
					else
						check_byte_num=0;
					break;
				default:
					check_byte_num=0;
					break;
			}
		}
		if(check_byte_num==17){
			
			if(rc==KEY_ENTER){
                log_flag=0;
				memset(log_buf,'\0',LOG_SERVICE_BUFLEN);
				strncpy(log_buf,close_cmd_buf,strlen(close_cmd_buf));
				check_byte_num=0;
				set_hci_uart_out(false);
				rtw_up_sema_from_isr((_sema*)&log_rx_interrupt_sema);
			}

		}
		else if(check_byte_num==32){
			log_flag += 1;
		}
		else if(check_byte_num==24){
			btc_set_single_tone_tx(1);
		}
		else if(check_byte_num==25){
			btc_set_single_tone_tx(0);
		}
		else
		{
			hci_uart_tx_bridge(rc);
		}
	}
}
#endif

#ifdef CONFIG_PLATFORM_8721D
static void _bt_uart_bridge_irq(void * data)
{
	(void)data;
	volatile u8 reg_iir;
	u8 IntId;
	u32 RegValue;
	UART_TypeDef * pLOG_UART = UART2_DEV;
	unsigned char rc = 0;
	const char close_cmd_buf[]="ATM2=bridge,close";

	reg_iir = UART_IntStatus(pLOG_UART);
	if ((reg_iir & RUART_IIR_INT_PEND) != 0) {
		// No pending IRQ
		return ;
	}

	IntId = (reg_iir & RUART_IIR_INT_ID) >> 1;

	if(IntId==RUART_RECEIVER_DATA_AVAILABLE || IntId==RUART_TIME_OUT_INDICATION) {
		RegValue = (UART_LineStatusGet(pLOG_UART));
		(void)RegValue;
		rc = bt_uart_bridge_getc();
		if(check_byte_num!=4){
			switch (rc)
			{
				case 'A':
						check_byte_num=1;
					break;
				case 'T':
					if(check_byte_num==1)
						check_byte_num=2;
					else
						check_byte_num=0;
					break;
				case 'M':
					if(check_byte_num==2)
						check_byte_num=3;
					else
						check_byte_num=0;
					break;
			 	case '2':
					if(check_byte_num==3)
						check_byte_num=4;
					else
						check_byte_num=0;
					break;
				default:
					check_byte_num=0;
					break;
			}
		}
		if(check_byte_num==4){
			
			if(rc==KEY_ENTER){
				memset(log_buf,'\0',LOG_SERVICE_BUFLEN);
				strncpy(log_buf,close_cmd_buf,strlen(close_cmd_buf));
				check_byte_num=0;
				set_hci_uart_out(false);
				UserIrqFunTable[UART_LOG_IRQ_LP] = (IRQ_FUN)UartBkFunc;
				rtw_up_sema_from_isr((_sema*)&log_rx_interrupt_sema);
			}
		}
		else
		{
			hci_uart_tx_bridge(rc);
		}
	}

	return ;
}
#endif


void bt_uart_bridge_set(uint32_t baudrate_0, uint32_t baudrate_1, uint8_t parity, uint8_t debug)
{
	(void) baudrate_1;
	(void) debug;
	baud_0 = baudrate_0;
	//baud_1 = baudrate_1;
	uart_parity = parity;
}

void bt_uart_bridge_close(void)
{
	baud_0 = 0;
	//baud_1 = 0;
	uart_parity = 0;
#ifdef CONFIG_PLATFORM_8721D
	ConfigDebugClose=0;
#endif

}

void bt_uart_bridge_open(PinName tx, PinName rx)
{
	(void)tx;
	(void)rx;
#ifdef CONFIG_PLATFORM_8710C
	//serial_init(&uart_0_sobj, tx, rx);
	hal_uart_reset_rx_fifo(&log_uart);
	
	if(baud_0 != 0)
		hal_uart_set_baudrate(&log_uart, baud_0);
	if(uart_parity == 1)
		hal_uart_set_format(&log_uart, 8, ParityOdd, 1);
	else if(uart_parity == 2)
		hal_uart_set_format(&log_uart, 8, ParityEven, 1);
	
	hal_uart_rxind_hook(&log_uart,_bt_uart_bridge_irq,0,0);
#endif

#ifdef CONFIG_PLATFORM_8721D
	#if 1
	UART_TypeDef * LOG_UART=UART2_DEV;

	/*reset rx fifo*/
	UART_ClearRxFifo(UART2_DEV);

	if(baud_0 != 0)
		UART_SetBaud(UART2_DEV, baud_0);
	if(uart_parity==0) {
		LOG_UART->LCR &= ~ BIT(3);
	}else if(uart_parity == 1) {
		LOG_UART->LCR |=  BIT(3);
		LOG_UART->LCR &= ~ BIT(4);
	}else if(uart_parity == 2) {
		LOG_UART->LCR |=  BIT(3);
		LOG_UART->LCR |= BIT(4);
	}

	UartBkFunc = (void *)UserIrqFunTable[UART_LOG_IRQ_LP];
	ConfigDebugClose=1;
	/* Register Log Uart Callback function */
	InterruptRegister((IRQ_FUN) _bt_uart_bridge_irq, UART_LOG_IRQ_LP, (u32)NULL, 10);
	InterruptEn(UART_LOG_IRQ_LP, 10);
	#endif
//#error //add UART init and hook _bt_uart_bridge_irq function here, use same API:_bt_uart_bridge_irq
#endif
	set_hci_uart_out(true);
}

