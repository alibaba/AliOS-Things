/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <aos/errno.h>
#include <aos/kernel.h>
#include "aos/init.h"
#include "board.h"
#include <k_api.h>
#include "aos/hal/uart.h"
#include "k_trace.h"
#include "SEGGER_SYSVIEW.h"
#include "SEGGER_RTT.h"

#define SYSVIEW_SINGLE_TX 1024
#define UART_TX_TIMEOUT 100

#define TASK_SENDDATA_NAME      "senddata"
#define TASK_SENDDATA_STACKSIZE 4096
#define TASK_SENDDATA_PRI       50

#define TASK_RECVDATA_NAME      "revdata"
#define TASK_RECVDATA_STACKSIZE 1024
#define TASK_RECVDATA_PRI       50

#define _SERVER_HELLO_SIZE        (4)
#define _TARGET_HELLO_SIZE        (4)

static const U8 _abHelloMsg[_TARGET_HELLO_SIZE] = { 'S', 'V', (SEGGER_SYSVIEW_VERSION / 10000), (SEGGER_SYSVIEW_VERSION / 1000) % 10 };
// "Hello" message expected by SysView: [ 'S', 'V', <PROTOCOL_MAJOR>, <PROTOCOL_MINOR> ]

static uart_dev_t uart_demo;

/* task entry */
static void task_senddata_entry(void *arg)
{
  int ret = 0;
  int channel_id = SEGGER_SYSVIEW_GetChannelID();
  uint32_t num = 0;
  uint8_t  bytes = 0;
  uint8_t tx_buf[SYSVIEW_SINGLE_TX];
  int length = 0;
  int len_read = 0;

  SEGGER_SYSVIEW_Start();
  ret = hal_uart_send(&uart_demo, _abHelloMsg, _SERVER_HELLO_SIZE, UART_TX_TIMEOUT);
  if (ret != 0) {
    printf("uart data send error!\n");
  }

  while (1)
  {
    bytes = SEGGER_RTT_GetBytesInBuffer(channel_id);
    if(bytes >= SYSVIEW_SINGLE_TX) {
      printf("bytes:%d!\n", bytes);
    }
    if(bytes == 0) {
      printf("bytes is 0!\n");
      aos_msleep(1);
      break;
    }

    num = SEGGER_RTT_ReadUpBufferNoLock(channel_id, tx_buf, bytes);

    hal_trace_output_block(tx_buf, num);
    usleep(1);
  }
}

static void task_recvdata_entry(void *arg)
{
    int  i;
    int  ret;
    int  rev_length;
    char rev_buf[1] = {0};
    int channel_id = SEGGER_SYSVIEW_GetChannelID();

    while (1) {
        ret = hal_uart_recv_II(&uart_demo, rev_buf, sizeof(rev_buf), &rev_length, HAL_WAIT_FOREVER);
        if (ret == 0) {
          SEGGER_RTT_WriteDownBuffer(channel_id, rev_buf, 1);
        }
    }
}

int uart_port_init(void)
{
  int ret = 0;
	/* task handle */
	aos_task_t task_recvdata;
  aos_task_t task_senddata;

  uart_demo.port                = 0;
  uart_demo.config.baud_rate    = 1500000;
  uart_demo.config.mode         = MODE_TX_RX;
  uart_demo.config.flow_control = FLOW_CONTROL_DISABLED;
  uart_demo.config.stop_bits    = STOP_BITS_1;
  uart_demo.config.parity       = NO_PARITY;
  uart_demo.config.data_width   = DATA_WIDTH_8BIT;

  ret = hal_uart_init(&uart_demo);
  if(ret != 0) {
      printf("init uart error\r\n");
      return -1;
  }
  /* Create the task to receive data */
  ret = aos_task_new_ext(&task_recvdata, TASK_RECVDATA_NAME, task_recvdata_entry, NULL,
                          TASK_RECVDATA_STACKSIZE, TASK_RECVDATA_PRI);
  if (ret != 0) {
      hal_uart_finalize(&uart_demo);
      printf("create uart data recv task error\r\n");
      return;
  }
  /* Create the task to send data */
  ret = aos_task_new_ext(&task_senddata, TASK_SENDDATA_NAME, task_senddata_entry, NULL,
                          TASK_SENDDATA_STACKSIZE, TASK_SENDDATA_PRI);
  if (ret != 0) {
      hal_uart_finalize(&uart_demo);
      printf("create uart data recv task error\r\n");
      return;
  }

  return ret;
}
