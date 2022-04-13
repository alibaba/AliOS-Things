/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __USB_CDC_H__
#define __USB_CDC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "plat_types.h"

enum USB_SERIAL_API_MODE {
    USB_SERIAL_API_NONBLOCKING,
    USB_SERIAL_API_BLOCKING,
};

enum USB_SERIAL_STATE_EVENT_T {
    USB_SERIAL_STATE_RESET, // RESET event should be processed as quickly as possible
    USB_SERIAL_STATE_DISCONNECT,
    USB_SERIAL_STATE_SLEEP,
    USB_SERIAL_STATE_WAKEUP,
    USB_SERIAL_STATE_CONFIG,
    USB_SERIAL_STATE_STALL_RECV,
    USB_SERIAL_STATE_STALL_SEND,
    USB_SERIAL_STATE_STALL_INTR,
    USB_SERIAL_STATE_UNSTALL_RECV,
    USB_SERIAL_STATE_UNSTALL_SEND,
    USB_SERIAL_STATE_UNSTALL_INTR,
};

enum USB_SERIAL_RET_VALUE {
    USB_ERR_OK          = 0,
    USB_ERR_RXTX_DATA   = -1,
    USB_ERR_NOT_DONE    = -2,
    USB_ERR_NOT_IDLE    = -3,
    USB_ERR_NOT_LOCK    = -9,
    USB_ERR_NOT_CONNECT = -10,
    USB_ERR_INV_PARAM   = -11,
    USB_ERR_UNALIGNED   = -12,
    USB_ERR_RX_SIZE     = -13,
    USB_ERR_RXTX_CANCEL = -14,
};

enum USB_SERIAL_STOP_BITS_T {
    USB_SERIAL_STOP_BITS_1      = 0,
    USB_SERIAL_STOP_BITS_1P5    = 1,
    USB_SERIAL_STOP_BITS_2      = 2,
};

enum USB_SERIAL_PARITY_T {
    USB_SERIAL_PARITY_NONE      = 0,
    USB_SERIAL_PARITY_ODD       = 1,
    USB_SERIAL_PARITY_EVEN      = 2,
    USB_SERIAL_PARITY_MARK      = 3,
    USB_SERIAL_PARITY_SPACE     = 4,
};

struct USB_SERIAL_LINE_CODING_T {
    uint32_t rate;
    uint8_t stop_bits;
    uint8_t parity;
    uint8_t data_bits;
};

typedef void (*USB_SERIAL_STATE_CALLBACK)(enum USB_SERIAL_STATE_EVENT_T event);
typedef void (*USB_SERIAL_BREAK_CALLBACK)(uint16_t ms);
typedef int (*USB_SERIAL_GET_LINE_CODING_CALLBACK)(struct USB_SERIAL_LINE_CODING_T *coding);
typedef int (*USB_SERIAL_SET_LINE_CODING_CALLBACK)(const struct USB_SERIAL_LINE_CODING_T *coding);
typedef void (*USB_SERIAL_XFER_CALLBACK)(const uint8_t *data, uint32_t size, int error);

struct USB_SERIAL_CFG_T {
    USB_SERIAL_STATE_CALLBACK state_callback;
    USB_SERIAL_BREAK_CALLBACK break_callback;
    USB_SERIAL_GET_LINE_CODING_CALLBACK get_line_coding_callback;
    USB_SERIAL_SET_LINE_CODING_CALLBACK set_line_coding_callback;
    enum USB_SERIAL_API_MODE mode;
};

int usb_serial_open(const struct USB_SERIAL_CFG_T *cfg);

int usb_serial_reopen(USB_SERIAL_BREAK_CALLBACK break_handler);

void usb_serial_close(void);

int usb_serial_ready(void);

int usb_serial_connected(void);

void usb_serial_init_xfer(void);

void usb_serial_cancel_recv(void);

int usb_serial_flush_recv_buffer(void);

int usb_serial_recv(uint8_t *buf, uint32_t size);

int usb_serial_direct_recv(uint8_t *dma_buf, uint32_t size, uint32_t expect, uint32_t *recv,
                           USB_SERIAL_XFER_CALLBACK callback);

void usb_serial_cancel_send(void);

int usb_serial_send(const uint8_t *buf, uint32_t size);

int usb_serial_direct_send(const uint8_t *dma_buf, uint32_t size, USB_SERIAL_XFER_CALLBACK callback);

int usb_serial_send_break(void);

#ifdef __cplusplus
}
#endif

#endif

