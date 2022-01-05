#ifndef AOS_HAL_UART_H
#define AOS_HAL_UART_H

#include <aos_hal_uart_internal.h>

#ifndef HAL_UART_H
typedef aos_hal_uart_data_width_t hal_uart_data_width_t;
typedef aos_hal_uart_stop_bits_t hal_uart_stop_bits_t;
typedef aos_hal_uart_flow_control_t hal_uart_flow_control_t;
typedef aos_hal_uart_parity_t hal_uart_parity_t;
typedef aos_hal_uart_mode_t hal_uart_mode_t;
typedef aos_hal_uart_config_t uart_config_t;
typedef aos_hal_uart_dev_t uart_dev_t;
#endif

#endif /* AOS_HAL_UART_H */
