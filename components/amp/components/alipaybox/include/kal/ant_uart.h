/**
 * ant_uart.h
 *
 * 串口读写和控制接口
 */
#ifndef __ANT_UART_H__
#define __ANT_UART_H__

#include "ant_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    UART_PORT1 = 0,
    UART_PORT2,
    UART_PORT3,
    UART_PORT4
} uart_port_enum;

typedef enum {
    DB_5BIT = 5,
    DB_6BIT,
    DB_7BIT,
    DB_8BIT
} uart_data_bits_enum;

typedef enum {
    SB_ONE = 1,
    SB_TWO,
    SB_ONE_HALF
} uart_stop_bits_enum;

typedef enum {
    PB_NONE = 0,
    PB_ODD,
    PB_EVEN,
    PB_SPACE,
    PB_MARK
} uart_parity_bits_enum;

typedef enum {
    UART_FC_NONE = 1,
    UART_FC_HW,
    UART_FC_SW
} uart_flow_ctrl;

typedef enum {
    UART_EVENT_READY_TO_READ  = 0,
    UART_EVENT_READY_TO_WRITE,
    UART_EVENT_FE_IND,
    UART_EVENT_RI_IND,
    UART_EVENT_DCD_IND,
    UART_EVENT_DTR_IND
} uart_event_type_enum;

typedef struct {
    ant_u32               baudrate;
    uart_data_bits_enum   data_bits;
    uart_stop_bits_enum   stop_bits;
    uart_parity_bits_enum parity_bits;
    uart_flow_ctrl        flow_ctrl;
} uart_ctrl_param_t;

typedef void (* uart_rx_callback)(uart_port_enum port);
/**
 * 注册串口接收数据的回调函数
 *
 * @param port 串口端口，取值参见uart_port_enum
 * @param cb 接收数据的回调函数
 * @return  0: 注册成功
 *         -1: 注册失败
 */
ant_s32 ant_uart_register_rx_callback(uart_port_enum port, uart_rx_callback cb);

/**
 * 串口模块初始化，在系统初始化时调用，如果不需要初始化，接口返回0
 * @return  0: 初始化成功
 *         -1: 初始化失败
 */
ant_s32 ant_uart_init(void);

/**
 * 打开并独占串口。读写串口需要执行此函数。
 * @param port 串口端口，取值参见 uart_port_enum
 * @return  0: 打开成功
 *         -1: 打开失败
 */
ant_s32 ant_uart_open(uart_port_enum port);

/**
 * 关闭并释放串口。
 * @param port 串口端口，取值参见 uart_port_enum
 * @return  0: 关闭成功
 *         -1: 关闭失败
 */
ant_s32 ant_uart_close(uart_port_enum port);

/**
 * 向串口写数据
 *
 * @param port 串口端口，取值参见 uart_port_enum
 * @param data 写入数据的缓存指针
 * @param len 写入数据的长度
 * @return 大于等于0: 写入成功，返回写入数据的长度
 *            小于0: 写入失败，取值参见ANT_ERROR_XXX宏定义
 */
ant_s32 ant_uart_write(uart_port_enum port, ant_u8 *data, ant_u32 len);

/**
 * 从串口读数据
 *
 * @param port 串口端口，取值参见 uart_port_enum
 * @param data 读取数据的缓存指针
 * @param len 读取数据的缓存长度
 * @return 大于等于0: 读取成功，返回读取数据的长度
 *            小于0: 读取失败，取值参见ANT_ERROR_XXX宏定义
 */
ant_s32 ant_uart_read(uart_port_enum port, ant_u8 *data, ant_u32 len);

/**
 * 配置串口参数
 *
 * @param port  串口端口，取值参见 uart_port_enum
 * @param param 串口参数，参见uart_ctrl_param_t类型定义
 * @return    0: 配置成功
 *         小于0: 配置失败，取值参见ANT_ERROR_XXX宏定义
 */
ant_s32 ant_uart_set_ctrl_param(uart_port_enum port, uart_ctrl_param_t *param);

/**
 * 清除串口接收缓存
 *
 * @param port 串口端口，取值参见 uart_port_enum
 * @return    0: 清除成功
 *         小于0: 清除失败，取值参见ANT_ERROR_XXX宏定义
 */
ant_s32 ant_uart_clear_rx_buffer(uart_port_enum port);

/**
 * 清除串口发送缓存
 *
 * @param port 串口端口，取值参见 uart_port_enum
 * @return    0: 清除成功
 *         小于0: 清除失败，取值参见ANT_ERROR_XXX宏定义
 */
ant_s32 ant_uart_clear_tx_buffer(uart_port_enum port);

/**
 * 获取串口发送缓存剩余大小
 *
 * @param port 串口端口，取值参见 uart_port_enum
 * @return 大于等于0: 获取成功，返回串口发送缓存剩余大小
 *            小于0: 获取失败，取值参见ANT_ERROR_XXX宏定义
 */
ant_u16 ant_uart_get_tx_roomleft(uart_port_enum port);

/**
 * 获取串口可接收剩余数据长度
 *
 * @param port 串口端口，取值参见 uart_port_enum
 * @return 大于等于0: 获取成功，返回串口可接收剩余数据长度
 *            小于0: 获取失败，取值参见ANT_ERROR_XXX宏定义
 */
ant_s32 ant_uart_get_rx_avail_bytes(uart_port_enum port);

/**
 * 从串口获取一个字节
 *
 * @param port 串口端口，取值参见 uart_port_enum
 * @return 返回获取的字节
 */
ant_u8  ant_uart_get_byte(uart_port_enum port);

#ifdef __cplusplus
}
#endif

#endif

