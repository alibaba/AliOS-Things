/**
 * @file uart.h
 * @copyright Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef AOS_HAL_UART_INTERNAL_H
#define AOS_HAL_UART_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup hal_uart UART
 *  uart hal API.
 *
 *  @{
 */

#include <stdint.h>

/* Define the wait forever timeout macro */
#ifndef HAL_WAIT_FOREVER
#define HAL_WAIT_FOREVER 0xFFFFFFFFU
#endif

#ifdef HAL_UART_H
typedef hal_uart_data_width_t aos_hal_uart_data_width_t;
typedef hal_uart_stop_bits_t aos_hal_uart_stop_bits_t;
typedef hal_uart_flow_control_t aos_hal_uart_flow_control_t;
typedef hal_uart_parity_t aos_hal_uart_parity_t;
typedef hal_uart_mode_t aos_hal_uart_mode_t;
typedef uart_config_t aos_hal_uart_config_t;
typedef uart_dev_t aos_hal_uart_dev_t;
#else /* HAL_UART_H */
/*
 * UART data width
 */
typedef enum {
    DATA_WIDTH_5BIT,
    DATA_WIDTH_6BIT,
    DATA_WIDTH_7BIT,
    DATA_WIDTH_8BIT,
    DATA_WIDTH_9BIT
} aos_hal_uart_data_width_t;

/*
 * UART stop bits
 */
typedef enum {
    STOP_BITS_1,
    STOP_BITS_2
} aos_hal_uart_stop_bits_t;

/*
 * UART flow control
 */
typedef enum {
    FLOW_CONTROL_DISABLED,  /**< Flow control disabled */
    FLOW_CONTROL_CTS,       /**< Clear to send, yet to send data */
    FLOW_CONTROL_RTS,       /**< Require to send, yet to receive data */
    FLOW_CONTROL_CTS_RTS    /**< Both CTS and RTS flow control */
} aos_hal_uart_flow_control_t;

/*
 * UART parity
 */
typedef enum {
    NO_PARITY,      /**< No parity check */
    ODD_PARITY,     /**< Odd parity check */
    EVEN_PARITY     /**< Even parity check */
} aos_hal_uart_parity_t;

/*
 * UART mode
 */
typedef enum {
    MODE_TX,        /**< Uart in send mode */
    MODE_RX,        /**< Uart in receive mode */
    MODE_TX_RX      /**< Uart in send and receive mode */
} aos_hal_uart_mode_t;

/*
 * UART configuration
 */
typedef struct {
    uint32_t                    baud_rate;      /**< Uart baud rate */
    aos_hal_uart_data_width_t   data_width;     /**< Uart data width */
    aos_hal_uart_parity_t       parity;         /**< Uart parity check mode */
    aos_hal_uart_stop_bits_t    stop_bits;      /**< Uart stop bit mode */
    aos_hal_uart_flow_control_t flow_control;   /**< Uart flow control mode */
    aos_hal_uart_mode_t         mode;           /**< Uart send/receive mode */
} aos_hal_uart_config_t;

/*
 * UART dev handle
 */
typedef struct {
    uint8_t                 port;   /**< uart port */
    aos_hal_uart_config_t   config; /**< uart config */
    void                    *priv;  /**< priv data */
    void (*cb)(int, void *, uint16_t, void *);
    void                    *userdata;
} aos_hal_uart_dev_t;
#endif /* HAL_UART_H */

typedef int32_t (*uart_rx_cb)(aos_hal_uart_dev_t *uart);

/**
 * Initialises a UART interface
 *
 *
 * @param[in]  uart  the interface which should be initialised
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t aos_hal_uart_init(aos_hal_uart_dev_t *uart);

/**
 * Transmit data on a UART interface
 *
 * @param[in]  uart     the UART interface
 * @param[in]  data     pointer to the start of data
 * @param[in]  size     number of bytes to transmit
 * @param[in]  timeout  timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                      if you want to wait forever
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t aos_hal_uart_send(aos_hal_uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout);

/**
 * Transmit data on a UART interface with polling
 *
 * @param[in]  uart     the UART interface
 * @param[in]  data     pointer to the start of data
 * @param[in]  size     number of bytes to transmit
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t aos_hal_uart_send_poll(aos_hal_uart_dev_t *uart, const void *data, uint32_t size);

/**
 * Receive data on a UART interface
 *
 * @param[in]   uart         the UART interface
 * @param[out]  data         pointer to the buffer which will store incoming data
 * @param[in]   expect_size  number of bytes to receive
 * @param[in]   timeout      timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                           if you want to wait forever
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t aos_hal_uart_recv(aos_hal_uart_dev_t *uart, void *data, uint32_t expect_size, uint32_t timeout);


/**
 * Receive data on a UART interface with polling
 *
 * @param[in]   uart         the UART interface
 * @param[out]  data         pointer to the buffer which will store incoming data
 * @param[in]   expect_size  number of bytes to receive
 * @param[in]   timeout      timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                           if you want to wait forever
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t aos_hal_uart_recv_poll(aos_hal_uart_dev_t *uart, void *data, uint32_t expect_size, uint32_t *recv_size);


/**
 * Receive data on a UART interface
 *
 * @param[in]   uart         the UART interface
 * @param[out]  data         pointer to the buffer which will store incoming data
 * @param[in]   expect_size  number of bytes to receive
 * @param[out]  recv_size    number of bytes trully received
 * @param[in]   timeout      timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                           if you want to wait forever
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t aos_hal_uart_recv_II(aos_hal_uart_dev_t *uart, void *data, uint32_t expect_size,
                             uint32_t *recv_size, uint32_t timeout);

/*
 *
 * @param [in]   uart         the UART interface
 * @param [in]   rx_cb        Non-zero pointer is the rx callback handler;
 *                            NULL pointer for rx_cb unregister operation
 *                            uart in rx_cb must be the same pointer with uart pointer passed to hal_uart_recv_cb_reg
 *                            driver must notify upper layer by calling rx_cb if data is available in UART's hw or rx buffer
 * @return 0: on success, negative no.: if an error occured with any step
 */
int32_t aos_hal_uart_recv_cb_reg(aos_hal_uart_dev_t *uart, uart_rx_cb cb);

/**
 * Deinitialises a UART interface
 *
 * @param[in]  uart  the interface which should be deinitialised
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t aos_hal_uart_finalize(aos_hal_uart_dev_t *uart);


/**
 * Config a UART callback function
 *
 * @param[in]  uart  the interface which should be deinitialised
 * @param[in]  cb the interface which should be callback
 * @param[in]  args the parameters of the calllback function
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t aos_hal_uart_callback(aos_hal_uart_dev_t *uart, void (*cb)(int, void *, uint16_t, void *), void *args);

int aos_hal_uart_rx_sem_take(int uartid, int timeout);
int aos_hal_uart_rx_sem_give(int port);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* AOS_HAL_UART_INTERNAL_H */
