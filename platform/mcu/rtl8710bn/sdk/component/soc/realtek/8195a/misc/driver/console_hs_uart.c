/******************************************************************************
 *
 * Copyright(c) 2007 - 2016 Realtek Corporation. All rights reserved.
 *
 *
 ******************************************************************************/
#include <platform_opts.h>
#include "FreeRTOS.h"
#include "task.h"
#include <platform/platform_stdlib.h>
#include "semphr.h"
#include "device.h"
#include "serial_api.h"
#include "at_cmd/log_service.h"
#include "osdep_service.h"
#include "serial_ex_api.h"
#include "pinmap.h"

char hs_uart_ready = 0; // used to switch between loguart and high speed uart
						// 0: loguart
						// 1: highspeed uart

// select uart tx/rx pin with gpio interrupt function
#define UART_TX    PA_7
#define UART_RX    PA_6

#define KEY_NL			0xa // '\n'
#define KEY_ENTER		0xd // '\r'
#define KEY_BS    		0x8
#define KEY_ESC    		0x1B
#define KEY_LBRKT  		0x5B
#define STR_END_OF_MP_FORMAT	"\r\n\r\r#"


#define CMD_HISTORY_LEN	4	// max number of executed command saved
extern char log_buf[LOG_SERVICE_BUFLEN];
extern xSemaphoreHandle log_rx_interrupt_sema;
char cmd_history[CMD_HISTORY_LEN][LOG_SERVICE_BUFLEN];
static unsigned int cmd_history_count = 0;

serial_t loguart_sobj;
//_sema at_printf_sema;
_sema hs_uart_dma_tx_sema;

#define HS_UART_USE_DMA_TX 1

void hs_uart_put_char(u8 c){
	serial_putc(&loguart_sobj, c);
}

void hs_uart_send_string(char *str)
{
	unsigned int i=0;
	while (str[i] != '\0') {
		serial_putc(&loguart_sobj, str[i]);
		i++;
	}
}

#if UART_AT_USE_DMA_TX
static void hs_uart_send_buf_done(uint32_t id)
{
	//serial_t *sobj = (serial_t *)id;

	rtw_up_sema_from_isr(&uart_at_dma_tx_sema);
}
#endif

void hs_uart_send_buf(u8 *buf, u32 len)
{
	unsigned char *st_p=buf;
	if(!len || (!buf)){
		return;
	}
#if UART_AT_USE_DMA_TX
	int ret;
	while(rtw_down_sema(&uart_at_dma_tx_sema) == _TRUE){
		ret = serial_send_stream_dma(&loguart_sobj, st_p, len);
		if(ret != HAL_OK){
			rtw_up_sema(&uart_at_dma_tx_sema);
			return;
		}else{
			return;
		}
	}
#else
	while(len){
		serial_putc(&loguart_sobj, *st_p);
		st_p++;
		len--;
	}
#endif
}

void hs_uart_irq(uint32_t id, SerialIrq event)
{
	serial_t    *sobj = (serial_t *)id;
	unsigned char rc=0;
	static unsigned char temp_buf[LOG_SERVICE_BUFLEN] = "\0";
	static unsigned char combo_key = 0;
	static unsigned short buf_count = 0;
	static unsigned char key_enter = 0;
	static char cmd_history_index = 0;
	if(event == RxIrq) {
		rc = serial_getc(sobj);

		if(key_enter && rc == KEY_NL){
			//serial_putc(sobj, rc);
			return;
		}

		if(rc == KEY_ESC){
			combo_key = 1;
		}else if(combo_key == 1){
			if(rc == KEY_LBRKT)
				combo_key = 2;
			else
				combo_key = 0;
		}else if(combo_key == 2){
			if(rc == 'A' || rc == 'B'){ // UP or Down
				if(rc == 'A'){
					cmd_history_index--;
					if(cmd_history_index < 0)
						cmd_history_index = (cmd_history_count>CMD_HISTORY_LEN)?CMD_HISTORY_LEN-1:(cmd_history_count-1)%CMD_HISTORY_LEN;
				}else{
					cmd_history_index++;
					if(cmd_history_index > (cmd_history_count>CMD_HISTORY_LEN?CMD_HISTORY_LEN-1:(cmd_history_count-1)%CMD_HISTORY_LEN))
						cmd_history_index = 0;
				}
				
				if(cmd_history_count > 0){
					buf_count = strlen(temp_buf);
					rtw_memset(temp_buf,'\0',buf_count);
					while(--buf_count >= 0){
						serial_putc(sobj, KEY_BS);
						serial_putc(sobj, ' ');
						serial_putc(sobj, KEY_BS);
					}
					hs_uart_send_string(cmd_history[cmd_history_index%CMD_HISTORY_LEN]);
					strcpy(temp_buf, cmd_history[cmd_history_index%CMD_HISTORY_LEN]);
					buf_count = strlen(temp_buf);
				}
			}
				
			// exit combo
			combo_key = 0;
		}
		else if(rc == KEY_ENTER){
			key_enter = 1;
			if(buf_count>0){
				serial_putc(sobj, KEY_NL);
				serial_putc(sobj, KEY_ENTER);
				rtw_memset(log_buf,'\0',LOG_SERVICE_BUFLEN);
				strncpy(log_buf,(char *)&temp_buf[0],buf_count);
				rtw_up_sema_from_isr(&log_rx_interrupt_sema);
				rtw_memset(temp_buf,'\0',buf_count);

				/* save command */
				rtw_memset(cmd_history[((cmd_history_count)%CMD_HISTORY_LEN)], '\0', buf_count+1);
				strcpy(cmd_history[((cmd_history_count++)%CMD_HISTORY_LEN)], log_buf);
				cmd_history_index = cmd_history_count%CMD_HISTORY_LEN;
				//cmd_history_count++;
				buf_count=0;
			}else{
				hs_uart_send_string(STR_END_OF_MP_FORMAT);
			}
		}
		else if(rc == KEY_BS){
			if(buf_count>0){
				buf_count--;
				temp_buf[buf_count] = '\0';
				
				serial_putc(sobj, rc);
				serial_putc(sobj, ' ');
				serial_putc(sobj, rc);
			}
		}
		else{
			/* cache input characters */
			if(buf_count < (LOG_SERVICE_BUFLEN - 1)){
				temp_buf[buf_count] = rc;
				buf_count++;
				serial_putc(sobj, rc);
				key_enter = 0;
			}
			else if(buf_count == (LOG_SERVICE_BUFLEN - 1)){
				temp_buf[buf_count] = '\0';
				
				hs_uart_send_string("\r\nERROR:exceed size limit"STR_END_OF_ATCMD_RET);
			}
		}
	}
}

void console_init_hs_uart(void)
{
    serial_init(&loguart_sobj,UART_TX,UART_RX);
    serial_baud(&loguart_sobj,38400);
    serial_format(&loguart_sobj, 8, ParityNone, 1);

#if UART_AT_USE_DMA_TX
	rtw_init_sema(&hs_uart_dma_tx_sema, 1);
	serial_send_comp_handler(&loguart_sobj, (void*)hs_uart_send_buf_done, (uint32_t)&loguart_sobj);
#endif
	
	serial_irq_handler(&loguart_sobj, hs_uart_irq, (uint32_t)&loguart_sobj);
	serial_irq_set(&loguart_sobj, RxIrq, 1);

	for(char i=0; i<CMD_HISTORY_LEN; i++)
		memset(cmd_history[i], '\0', LOG_SERVICE_BUFLEN);
	
	/* indicate low level layer that hs uart is ready */
	hs_uart_ready = 1;
}

int use_mode;


VOID HalSerialPutcRtl8195a( 
    IN  u8 c
    )
{
	extern char hs_uart_ready;
	extern void hs_uart_put_char(u8 c);
	if(hs_uart_ready)
		hs_uart_put_char(c);
}

void console_init(void)
{
    sys_log_uart_off();
    console_init_hs_uart();

#if !TASK_SCHEDULER_DISABLED    
    RtlConsolInitRam((u32)RAM_STAGE,(u32)0,(VOID*)NULL);
#else
    RtlConsolInitRam((u32)ROM_STAGE,(u32)0,(VOID*)NULL);
#endif

#if BUFFERED_PRINTF
    rtl_printf_init();
#endif
}




