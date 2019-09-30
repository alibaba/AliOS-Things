/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 * 
 * 
 *   this is a uart sample from hal uart for esp8266, and the same as other chips;
 *   for esp8266 , when the port set is 1 ,then the uart1 ande uart2 is the same baud_rate , but the uart1（gpio2） is for log ;
 * 
 *   这sample是一个从hal层适配esp8266的，也许同样适配其他芯片
 *   对于esp8266，如果设置串口为1，则串口一和串口零都是这个波特率，如果需要不一样，请自行修改driver ，注意串口一（gpio2）是作为日志打印。
 * 
 *   Contributor:  https://github.com/xuhongv
 * 
 * 
 */


#include "aos/kernel.h"
#include "aos/hal/uart.h"

#include "driver/uart.h"

#define HAL_WAIT_FOREVER 0xFFFFFFFFU

int application_start(int argc, char *argv[])
{

    printf("uart sample application started...\n");


    uart_config_t uartConfig;
    uartConfig.baud_rate = 9600; 
    uart_dev_t uart;
    uart.port = 1; //for esp8266 , when the port set is 1 ,then the uart1 ande uart2 is the same baud_rate , but the uart1 is for log 
    uart.config = uartConfig;
    hal_uart_init(&uart);

    uint8_t receive_bytes[15];
    int32_t ret = -1;
    uint32_t i, recv_size = 0;
    while (1)
    {
        ret = hal_uart_recv_II(&uart, &receive_bytes, 15, &recv_size, HAL_WAIT_FOREVER);
        if ((ret == 0))
        {
            for (i = 0; i < recv_size; i++)
                printf("hal_uart_recv_II ch = %d ,recv_size= %d \n", receive_bytes[i], recv_size);
            // en:return by the way you came  ch: 原路返回数据
            hal_uart_send(&uart,receive_bytes,recv_size,1000); 

        }
    }
    aos_loop_run();
    return 0;
}
