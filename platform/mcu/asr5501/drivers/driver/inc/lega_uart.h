#ifndef __LEGA_UART_H
#define __LEGA_UART_H
#include <stdint.h>

/* UART BAUDRATE*/
#define UART_BAUDRATE_110                       (110)
#define UART_BAUDRATE_300                       (300)
#define UART_BAUDRATE_600                       (600)
#define UART_BAUDRATE_1200                      (1200)
#define UART_BAUDRATE_2400                      (2400)
#define UART_BAUDRATE_4800                      (4800)
#define UART_BAUDRATE_9600                      (9600)
#define UART_BAUDRATE_14400                     (14400)
#define UART_BAUDRATE_19200                     (19200)
#define UART_BAUDRATE_38400                     (38400)
#define UART_BAUDRATE_57600                     (57600)
#define UART_BAUDRATE_115200                    (115200)
#define UART_BAUDRATE_230400                    (230400)
#define UART_BAUDRATE_460800                    (460800)
#define UART_BAUDRATE_921600                    (921600)

#define LEGA_UART0_INDEX    0
#define LEGA_UART1_INDEX    1
#define LEGA_UART2_INDEX    2
#define LEGA_UART_NUM       3

typedef void (*lega_uart_callback_func)(char);
/*
 * UART data width
 */
typedef enum {
    DATA_5BIT,
    DATA_6BIT,
    DATA_7BIT,
    DATA_8BIT,
    DATA_9BIT
} lega_uart_data_width_t;

/*
 * UART stop bits
 */
typedef enum {
    STOP_1BIT,
    STOP_2BITS
} lega_uart_stop_bits_t;

/*
 * UART flow control
 */
typedef enum {
    FLOW_CTRL_DISABLED,
    FLOW_CTRL_CTS,
    FLOW_CTRL_RTS,
    FLOW_CTRL_CTS_RTS
} lega_uart_flow_control_t;

/*
 * UART parity
 */
typedef enum {
    PARITY_NO,
    PARITY_ODD,
    PARITY_EVEN
} lega_uart_parity_t;

/*
 * UART mode
 */
typedef enum {
    TX_MODE,
    RX_MODE,
    TX_RX_MODE
} lega_uart_mode_t;

/*
 * UART configuration
 */
typedef struct {
    uint32_t                baud_rate;
    lega_uart_data_width_t   data_width;
    lega_uart_parity_t       parity;
    lega_uart_stop_bits_t    stop_bits;
    lega_uart_flow_control_t flow_control;
    lega_uart_mode_t         mode;
} lega_uart_config_t;

typedef struct {
    uint8_t       port;    /* uart port */
    lega_uart_config_t config;  /* uart config */
    void         *priv;    /* priv data */
} lega_uart_dev_t;

/**
 * Initialises a UART interface
 *
 *
 * @param[in]  uart  the interface which should be initialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t lega_uart_init(lega_uart_dev_t *uart);

/**
 * Transmit data on a UART interface
 *
 * @param[in]  uart     the UART interface
 * @param[in]  data     pointer to the start of data
 * @param[in]  size     number of bytes to transmit
 * @param[in]  timeout  timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                      if you want to wait forever
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t lega_uart_send(lega_uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout);

/**
 * Deinitialises a UART interface
 *
 * @param[in]  uart  the interface which should be deinitialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t lega_uart_finalize(lega_uart_dev_t *uart);

#endif //__LEGA_UART_H
