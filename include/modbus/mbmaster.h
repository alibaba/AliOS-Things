/**
 * @file      mbmaster_api.h
 * @copyright Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef MBMASTER_API_H
#define MBMASTER_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>

#include "k_api.h"

#include "mbmaster_default_config.h"
#include "adu/adu.h"
#include "auxiliary/auxiliary.h"

/**
 * @addtogroup aos_mbmaster mbmaster
 * modbus master protocol.
 *
 * @{
 */

/* This enum is used to define the parity */
typedef enum {
    MB_PAR_NONE,
    MB_PAR_ODD,
    MB_PAR_EVEN
} mb_parity_t;

/* This enum is used to define the return value */
typedef enum mb_status {
    MB_SUCCESS = 0u,
    MB_MUTEX_ERROR,
    MB_INVALID_SLAVE_ADDR,
    MB_INVALID_PARAM,
    MB_RESPOND_EXCEPTION,  /**< remote device respond exception */
    MB_RESPOND_LENGTH_ERR, /**< The length field is not consistent with the data length */
    MB_RESPOND_FRAME_ERR,  /**< frame is too short or CRC error */
    MB_RESPOND_TIMEOUT,
    MB_CANNOT_GET_HANDLER, /**< alloc handler failed. check whether MBMASTER_CONFIG_HANDLER_MAX is set large enough */
    MB_SLAVE_NO_RESPOND,
    MB_FRAME_SEND_ERR,
    MB_SERIAL_INIT_FAILED,
    MB_SERIAL_UNINIT_FAILED,
    MB_FUNCTION_CODE_NOT_SUPPORT,
} mb_status_t;

/* This enum is used to define the modbus mode */
typedef enum {
    MB_RTU,
    MB_ASCII,
    MB_TCP
} mb_mode_t;

/* forward declaration */
typedef struct mb_handler mb_handler_t;

/* Define the callback function type */
typedef mb_status_t (*frame_send_func_t)(mb_handler_t *handler, uint32_t timeout);
typedef mb_status_t (*frame_recv_func_t)(mb_handler_t *handler);
typedef mb_status_t (*adu_assemble_func_t)(mb_handler_t *handler);
typedef mb_status_t (*adu_disassemble_func_t)(mb_handler_t *handler);

typedef struct mb_handler {
    uint8_t used;       /**< The value is 1 when used */
    uint8_t slave_addr;
    uint8_t mode;

    uint8_t  mb_frame_buff[ADU_BUF_MAX_LENGTH]; /**< Buffer used to send and receive frame*/
    uint8_t  pdu_offset;                        /**< Offset of the pdu on mb_frame_buff[] */
    uint8_t  pdu_length;                        /**< length of the pdu on mb_frame_buff[] */
    uint32_t mb_frame_length;                   /**< The length of the whole frame */

    uint32_t respond_timeout;

    frame_send_func_t      frame_send;      /**< The callback funciton to send the frame */
    frame_recv_func_t      frame_recv;      /**< The callback funciton to receive the frame */
    adu_assemble_func_t    adu_assemble;    /**< The callback funciton to assemble frame at ADU layer */
    adu_disassemble_func_t adu_disassemble; /**< The callback funciton to disassemble frame at ADU layer */

    MB_MUTEX_T mb_mutex;

    void *private;
} mb_handler_t;

/**
 * This function will alloc a handler and initializes the port for communication
 * @param[out]  handler     returns a pointer to the assigned handler
 * @param[in]   port        port usend to communication
 * @param[in]   baud_rate   baud rate used by port
 * @param[in]   parity      parity used by port
 * @return  the operation status, MB_SUCCESS is OK, other is error
 */
mb_status_t mbmaster_rtu_init(mb_handler_t **handler, uint8_t port, uint32_t baud_rate, mb_parity_t parity);

/**
 * This function will free the handler and uninitializes the port for communication
 * @param[in]  req_handler  the handler pointer returned from mbmaster_rtu_init
 * @return  the operation status, MB_SUCCESS is OK, other is error
 */
mb_status_t mbmaster_rtu_uninit(mb_handler_t *req_handler);

/**
 * This function will initiate a read coils request(function code 0x01)
 * @param[in]   req_handler    the handler pointer returned from mbmaster_rtu_init
 * @param[in]   slave_addr     slave address of the remote device
 * @param[in]   start_addr     starting address
 * @param[in]   quantity       quantity of coils
 * @param[out]  respond_buf    data receive from remote device, length must >= (quantity / 8 + 1) bytes
 * @param[out]  respond_count  the number of bytes of data received
 * @param[in]   timeout        the timeout time(ms) for sending the request, 0xFFFFFFFF means waiting for ever
 * @return  the operation status, MB_SUCCESS is OK, other is error
 * @note  when MB_RESPOND_EXCEPTION is returned, the first byte of respond_buf holds the exception code
 */
mb_status_t mbmaster_read_coils(mb_handler_t *req_handler, uint8_t slave_addr, uint16_t start_addr,
                                uint16_t quantity, uint8_t *respond_buf, uint8_t *respond_count,
                                uint32_t timeout);

/**
 * This function will initiate a read discrete inputs request(function code 0x02)
 * @param[in]   req_handler    the handler pointer returned from mbmaster_rtu_init
 * @param[in]   slave_addr     slave address of the remote device
 * @param[in]   start_addr     starting address
 * @param[in]   quantity       quantity of inputs
 * @param[out]  respond_buf    data receive from remote device, length must >= (quantity / 8 + 1) bytes
 * @param[out]  respond_count  the number of bytes of data received
 * @param[in]   timeout        the timeout time(ms) for sending the request, 0xFFFFFFFF means waiting for ever
 * @return  the operation status, MB_SUCCESS is OK, other is error
 * @note  when MB_RESPOND_EXCEPTION is returned, the first byte of respond_buf holds the exception code
 */
mb_status_t mbmaster_read_discrete_inputs(mb_handler_t *req_handler, uint8_t slave_addr, uint16_t start_addr,
                                          uint16_t quantity, uint8_t *respond_buf, uint8_t *respond_count,
                                          uint32_t timeout);

/**
 * This function will initiate a read holding registers request(function code 0x03)
 * @param[in]   req_handler    the handler pointer returned from mbmaster_rtu_init
 * @param[in]   slave_addr     slave address of the remote device
 * @param[in]   start_addr     starting address
 * @param[in]   quantity       quantity of registers
 * @param[out]  respond_buf    data receive from remote device, length must >= (quantity * 2) bytes
 * @param[out]  respond_count  the number of bytes of data received
 * @param[in]   timeout        the timeout time(ms) for sending the request, 0xFFFFFFFF means waiting for ever
 * @return  the operation status, MB_SUCCESS is OK, other is error
 * @note    when MB_RESPOND_EXCEPTION is returned, the first byte of respond_buf holds the exception code
 */
mb_status_t mbmaster_read_holding_registers(mb_handler_t *req_handler, uint8_t slave_addr,
                                           uint16_t start_addr, uint16_t quantity,
                                           uint8_t *respond_buf, uint8_t *respond_count, uint32_t timeout);

/**
 * This function will initiate a read input registers request(function code 0x04)
 * @param[in]   req_handler    the handler pointer returned from mbmaster_rtu_init
 * @param[in]   slave_addr     slave address of the remote device
 * @param[in]   start_addr     starting address
 * @param[in]   quantity       quantity of input registers
 * @param[out]  respond_buf    data receive from remote device, length must >= (quantity * 2) bytes
 * @param[out]  respond_count  the number of bytes of data received
 * @param[in]   timeout        the timeout time(ms) for sending the request, 0xFFFFFFFF means waiting for ever
 * @return  the operation status, MB_SUCCESS is OK, other is error
 * @note    when MB_RESPOND_EXCEPTION is returned, the first byte of respond_buf holds the exception code
 */
mb_status_t mbmaster_read_input_registers(mb_handler_t *req_handler, uint8_t slave_addr,
                                         uint16_t start_addr, uint16_t quantity,
                                         uint8_t *respond_buf, uint8_t *respond_count, uint32_t timeout);

/**
 * This function will initiate a write single coil request(function code 0x05)
 * @param[in]   req_handler     the handler pointer returned from mbmaster_rtu_init
 * @param[in]   slave_addr      slave address of the remote device
 * @param[in]   register_addr   coil address
 * @param[in]   register_value  coil value, 0x0000 for off, 0xFF00 for on
 * @param[out]  resp_addr       coil address responded from remote device
 * @param[out]  resp_value      coil value responded from remote device
 * @param[out]  excepiton_code  exception code, when MB_RESPOND_EXCEPTION is returned
 * @param[in]   timeout        the timeout time(ms) for sending the request, 0xFFFFFFFF means waiting for ever
 * @return  the operation status, MB_SUCCESS is OK, other is error
 */
mb_status_t mbmaster_write_single_coil(mb_handler_t *req_handler, uint8_t slave_addr, uint16_t coil_addr,
                                       uint16_t coil_value, uint16_t *resp_addr, uint16_t *resp_value,
                                       uint8_t *exception_code, uint32_t timeout);

/**
 * This function will initiate a write single register request(function code 0x06)
 * @param[in]   req_handler     the handler pointer returned from mbmaster_rtu_init
 * @param[in]   slave_addr      slave address of the remote device
 * @param[in]   register_addr   register address
 * @param[in]   register_value  regitter value
 * @param[out]  resp_addr       register address responded from remote device
 * @param[out]  resp_value      register value responded from remote device
 * @param[out]  excepiton_code  exception code, when MB_RESPOND_EXCEPTION is returned
 * @param[in]   timeout        the timeout time(ms) for sending the request, 0xFFFFFFFF means waiting for ever
 * @return  the operation status, MB_SUCCESS is OK, other is error
 */
mb_status_t mbmaster_write_single_register(mb_handler_t *req_handler, uint8_t slave_addr, uint16_t register_addr,
                                           uint16_t register_value, uint16_t *resp_addr, uint16_t *resp_value,
                                           uint8_t *exception_code, uint32_t timeout);

/**
 * This function will initiate a write mutiple coils request(function code 0x0F)
 * @param[in]   req_handler     the handler pointer returned from mbmaster_rtu_init
 * @param[in]   slave_addr      slave address of the remote device
 * @param[in]   start_addr      start address
 * @param[in]   quantity        quantity of outputs
 * @param[in]   outputs_buf     outputs value(bit array, 0 to be OFF. 1 to be ON)
 * @param[out]  resp_addr       start address responded from remote device
 * @param[out]  resp_quantity   quantity responded from remote device
 * @param[out]  excepiton_code  exception code, when MB_RESPOND_EXCEPTION is returned
 * @param[in]   timeout        the timeout time(ms) for sending the request, 0xFFFFFFFF means waiting for ever
 * @return  the operation status, MB_SUCCESS is OK, other is error
 */
mb_status_t mbmaster_write_multiple_coils(mb_handler_t *req_handler, uint8_t slave_addr, uint16_t start_addr,
                                          uint16_t quantity, uint8_t *outputs_buf, uint16_t *resp_addr,
                                          uint16_t *resp_quantity, uint8_t *exception_code, uint32_t timeout);
/**
 * This function will initiate a write mutiple registers request(function code 0x10)
 * @param[in]   req_handler     the handler pointer returned from mbmaster_rtu_init
 * @param[in]   slave_addr      slave address of the remote device
 * @param[in]   start_addr      start address
 * @param[in]   quantity        quantity of registers
 * @param[in]   outputs_buf     registers value
 * @param[out]  resp_addr       start address responded from remote device
 * @param[out]  resp_quantity   registers quantity responded from remote device
 * @param[out]  excepiton_code  exception code, when MB_RESPOND_EXCEPTION is returned
 * @param[in]   timeout        the timeout time(ms) for sending the request, 0xFFFFFFFF means waiting for ever
 * @return  the operation status, MB_SUCCESS is OK, other is error
 */
mb_status_t mbmaster_write_multiple_registers(mb_handler_t *req_handler, uint8_t slave_addr, uint16_t start_addr,
                                              uint16_t quantity, uint8_t *outputs_buf, uint16_t *resp_addr,
                                              uint16_t *resp_quantity, uint8_t *exception_code, uint32_t timeout);
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* MBMASTER_API_H */

