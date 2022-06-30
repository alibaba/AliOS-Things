// 串口相关头文件


#ifndef __OPENLOONGSON_UART_H
#define __OPENLOONGSON_UART_H


#include "ls1c_public.h"


// 串口模块编号
typedef enum
{
    LS1C_UART00 = 0,        // 全功能串口UART0可以分为两个四线串口UART00和UART01
    LS1C_UART01,
    LS1C_UART1,
    LS1C_UART2,
    LS1C_UART3,
    LS1C_UART4,
    LS1C_UART5,
    LS1C_UART6,
    LS1C_UART7,
    LS1C_UART8,
    LS1C_UART9,
    LS1C_UART10,
    LS1C_UART11
}ls1c_uart_t;


// 串口信息
typedef struct
{
    ls1c_uart_t UARTx;              // 串口模块编号
    unsigned int baudrate;          // 波特率
    BOOL rx_enable;                 // 是否需要使用串口接收数据(使能接收中断)，发送默认使能
}ls1c_uart_info_t;



/*
 * 获取指定串口模块的基地址
 * @UARTx 串口编号
 * @ret 基地址
 */
void *uart_get_base(ls1c_uart_t UARTx);


/*
 * 初始化指定的串口模块
 * @uart_info_p 串口模块信息
 */
void uart_init(ls1c_uart_info_t *uart_info_p);


/*
 * 初始化串口2
 */
void uart2_init(void);


/*
 * 在串口2上打印字符串
 * @str 待打印的字符串
 */
void uart2_print(const char *str);


/*
 * 在调试串口打印字符串
 * @str 待打印的字符串
 */
void uart_debug_print(const char *str);


/*
 * 在调试串口打印一个字符
 * @ch 待打印的字符
 */
void uart_debug_putc(unsigned char ch);


/*
 * 把中断号转换为串口号
 * @IRQn 中断号
 * @ret 串口号
 */
ls1c_uart_t uart_irqn_to_uartx(int IRQn);


/*
 * 发送一个字节
 * @uartx 串口号
 * @ch 待发送的字符串
 */
void uart_putc(ls1c_uart_t uartx, unsigned char ch);


/*
 * 打印一个字符串到指定串口
 * @uartx 串口号
 * @str 待打印的字符串
 */
void uart_print(ls1c_uart_t uartx, const char *str);


#endif

