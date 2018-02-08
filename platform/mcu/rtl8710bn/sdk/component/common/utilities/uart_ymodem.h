/****************************************uart_ymodem.h**************************************************/

#ifndef __YMODEM_H_
#define __YMODEM_H_

//#include "osdep_api.h"
#include "osdep_service.h"
#include "serial_api.h"
#include "flash_api.h"
#include "device_lock.h"
/***********************************************************************
 *                                Macros                               *
 ***********************************************************************/
#if defined(CONFIG_PLATFORM_8711B)
// 8710B
#define UART_TX PA_23
#define UART_RX PA_18
#else
// 8711AM
#define UART_TX PA_7
#define UART_RX PA_6
//8711AF
//#define UART_TX PA_4
//#define UART_RX PA_0
#endif

#define UART_BAUDRATE 115200
#define UART_YMODEM_TASK_PRIORITY	5
#define UART_YMODEM_TASK_DEPTH	512

#define CONFIG_CALC_FILE_SIZE 1
#define CRC_CHECK	1
#define AUTO_REBOOT 0
#define DUMP_DATA 0

#define OFFSET_DATA		FLASH_SYSTEM_DATA_ADDR
#define IMAGE_TWO			(0x80000)
//Y-modem related
#define MODEM_MAX_RETRIES   1
#define MODEM_CRC_RETRIES   51
#define MODEM_CAN_COUNT     3
#define MODEM_EOT_COUNT     1

// ymodem protocol definition
#define MODEM_SOH  0x01
#define MODEM_STX  0x02
#define MODEM_EOT  0x04
#define MODEM_ACK  0x06
#define MODEM_NAK  0x15
#define MODEM_CAN  0x18
#define MODEM_C    0x43
// 1 block size byte + 2 block number bytes + 1024 data body + 2 crc bytes
#define RCV_BUF_SIZE ((1)+(2)+(1024)+(2))
/******************************** data struct **********************************/
typedef struct _uart_ymodem_t
{
	serial_t sobj;
	flash_t  flash;

	/* Used for UART RX */
	u8 uart_rcv_buf[RCV_BUF_SIZE];
	u8 uart_irq_buf[RCV_BUF_SIZE];
	//_Sema uart_rx_sema;	
	_sema uart_rx_sema;
	u32 image_address;
	
	u32 tick_last_update;
	u32 tick_current;
	u32 uart_recv_index;
	u32 uart_recv_buf_index;
	/* uart ymodem related*/
	u32 modemtype;
    u32 crc_mode;
    u32 nxt_num;	//next block num
    u32 cur_num;	//current block num
    u32 len;
    u32 rec_err;	//blcok data recv status
    u32 filelen;	//Ymodem file length
    u8 *buf;		//data buf
    u8 *filename;	//file name
}uart_ymodem_t;


#ifdef __cplusplus
    extern "C"{
#endif
    
extern int uart_ymodem(void);

#ifdef __cplusplus
    }
#endif

#endif
