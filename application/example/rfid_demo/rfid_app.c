/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <aos/kernel.h>
#include "aos/hal/gpio.h"
#include "ulog/ulog.h"
#include "hal_iomux_haas1000.h"
#include "rfid_app.h"
#include "k_api.h"
#include "aos/cli.h"
#include "aos/hal/uart.h"

extern void user_post_deviceid(char *str);
extern void user_post_data(char type_t,uint8_t value);
extern uint32_t hal_sys_timer_systick_hz();

extern uint32_t hal_fast_sys_timer_get();
extern uint32_t hal_cmu_get_crystal_freq();

#define CONFIG_FAST_SYSTICK_HZ      (hal_cmu_get_crystal_freq() / 4)
#define FAST_TICKS_TO_US(tick)      ((uint32_t)(tick) * 10 / (CONFIG_FAST_SYSTICK_HZ / 1000 / 100))

#define TASK_RECVDATA_NAME      "revdata"
#define TASK_RECVDATA_STACKSIZE 512
#define TASK_RECVDATA_PRI       50

static uart_dev_t uart_demo;

static unsigned char rfid_data_buf[128];

static char rfid_deviceid[12];

static int data_index = 0;

void get_deviceid(void)
{
    int i;

    if (data_index > 13) {
        for (i = 0; i < sizeof(rfid_data_buf); i++) {
            if((rfid_data_buf[i] == 0x20) &&(rfid_data_buf[i+1] == 0x0) &&(rfid_data_buf[i+2] == 0x0) &&(rfid_data_buf[i+3] == 0x8)){
                snprintf(rfid_deviceid, sizeof(rfid_deviceid), "%02x%02x%02x%02x", rfid_data_buf[i+8],rfid_data_buf[i+9],rfid_data_buf[i+10],rfid_data_buf[i+11]);
                memset(rfid_data_buf,0,sizeof(rfid_data_buf));
                data_index = 0;
                printf("rfid_deviceid:%s\n",rfid_deviceid);
				user_post_deviceid(rfid_deviceid);
            }
        }
    }
}

/* task entry */
static void task_recvdata_entry(void *arg)
{
    int  i;
    int  ret;
    char rev_buf[1];
    int  rev_length;

    while (1) {
        ret = hal_uart_recv_II(&uart_demo, rev_buf, sizeof(rev_buf), &rev_length, AOS_WAIT_FOREVER);
        if (ret != 0) {
            printf("recv data from uart error\r\n");
            break;
        }

        for(i = 0; i < rev_length; i++) {
            rfid_data_buf[data_index] = (unsigned char)rev_buf[i]&0xff;
            data_index++;
        }
        get_deviceid();
    }
}
void report_event2cloud(void)
{

}

#if (RHINO_CONFIG_HW_COUNT > 0)
void _udelay(unsigned long x) {
	unsigned long now,t;

	t = FAST_TICKS_TO_US(hal_fast_sys_timer_get());
	now = t;
	while ((now - t) < x) {
		now = FAST_TICKS_TO_US(hal_fast_sys_timer_get());
	}
}

void _msdelay(unsigned long x) {
	_udelay(x * 1000);
}
#else
#error "RHINO_CONFIG_HW_COUNT should be configured to get us level delay"
#endif

int rfid_uart_init(void)
{
    int ret = 0;
	/* task handle */
	aos_task_t task_recvdata;

    uart_demo.port                = 2;
    uart_demo.config.baud_rate    = 9600;
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
    return ret;
}
