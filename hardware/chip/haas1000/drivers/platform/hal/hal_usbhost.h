/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __HAL_USB_H__
#define __HAL_USB_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "stdbool.h"

#ifdef USB_HIGH_SPEED
#define MAX_USBHOST_PACKET_SIZE                 512
#else
#define MAX_USBHOST_PACKET_SIZE                 64
#endif

#define USBHOST_EPNUM                           6

#define HAL_USBHOST_CHAN_NONE                   0xFF

enum HAL_USBHOST_EP_TYPE_T {
    HAL_USBHOST_EP_CTRL,
    HAL_USBHOST_EP_ISO,
    HAL_USBHOST_EP_BULK,
    HAL_USBHOST_EP_INT,

    HAL_USBHOST_EP_QTY
};

enum HAL_USBHOST_PID_TYPE_T {
    HAL_USBHOST_PID_DATA0 = 0,
    HAL_USBHOST_PID_DATA2 = 1,
    HAL_USBHOST_PID_DATA1 = 2,
    HAL_USBHOST_PID_MDATA = 3,
    HAL_USBHOST_PID_SETUP = 3,

    HAL_USBHOST_PID_QTY,
    HAL_USBHOST_PID_AUTO = 0xFF,
};

enum HAL_USBHOST_PORT_EVENT_T {
    HAL_USBHOST_PORT_CONN,
    HAL_USBHOST_PORT_EN_LS,
    HAL_USBHOST_PORT_EN_FS,
    HAL_USBHOST_PORT_EN_HS,
    HAL_USBHOST_PORT_EN_BAD,
    HAL_USBHOST_PORT_DISCONN,

    HAL_USBHOST_PORT_QTY
};

enum HAL_USBHOST_SETUP_STAGE_T {
    HAL_USBHOST_SETUP_DATA_IN,
    HAL_USBHOST_SETUP_DATA_OUT,
    HAL_USBHOST_SETUP_STATUS_IN,
    HAL_USBHOST_SETUP_STATUS_OUT,
    HAL_USBHOST_SETUP_DONE,
    HAL_USBHOST_SETUP_ERROR,

    HAL_USBHOST_SETUP_STAGE_QTY
};

enum HAL_USBHOST_XFER_ERR_T {
    HAL_USBHOST_XFER_ERR_NONE,
    HAL_USBHOST_XFER_ERR_AHB,
    HAL_USBHOST_XFER_ERR_STALL,
    HAL_USBHOST_XFER_ERR_TRANSACTION,
    HAL_USBHOST_XFER_ERR_BABBLE,
    HAL_USBHOST_XFER_ERR_FRAME_OVERRUN,
    HAL_USBHOST_XFER_ERR_DATA_TOGGLE,
    HAL_USBHOST_XFER_ERR_TIMEOUT,
    HAL_USBHOST_XFER_ERR_DISCONN,

    HAL_USBHOST_XFER_ERR_QTY
};

struct HAL_USBHOST_SETUP_XFER_T;

typedef void (*HAL_USBHOST_DELAY_FUNC)(uint32_t ms);
typedef void (*HAL_USBHOST_PORT_HANDLER)(enum HAL_USBHOST_PORT_EVENT_T evt);
typedef void (*HAL_USBHOST_XFER_COMPL_HANDLER)(uint8_t chan, uint8_t *buf, uint32_t len, enum HAL_USBHOST_XFER_ERR_T error);
typedef void (*HAL_USBHOST_SETUP_COMPL_HANDLER)(struct HAL_USBHOST_SETUP_XFER_T *setup, int error);

enum HAL_USBHOST_PLUG_STATUS_T {
    HAL_USBHOST_PLUG_UNKNOWN,
    HAL_USBHOST_PLUG_IN,
    HAL_USBHOST_PLUG_OUT,
};

typedef void (*HAL_USBHOST_PLUG_HANDLER)(enum HAL_USBHOST_PLUG_STATUS_T status);

struct HAL_USBHOST_CHAN_TYPE_T {
    uint16_t mps;
    uint8_t ep_num;
    uint8_t ep_in;
    enum HAL_USBHOST_EP_TYPE_T ep_type;
    uint8_t dev_addr;
};

struct HAL_USBHOST_XFER_T {
    uint8_t *buf;
    uint32_t size;
    uint8_t multi_cnt;
    enum HAL_USBHOST_PID_TYPE_T pid;
    HAL_USBHOST_XFER_COMPL_HANDLER handler;
};

struct HAL_USBHOST_SETUP_PKT_T {
    uint8_t  bmRequestType;
    uint8_t  bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
};

struct HAL_USBHOST_SETUP_XFER_T {
    struct HAL_USBHOST_SETUP_PKT_T setup_pkt;
    enum HAL_USBHOST_SETUP_STAGE_T next_stage;
    HAL_USBHOST_SETUP_COMPL_HANDLER handler;
    uint8_t *data_buf;
    uint8_t chan_out;
    uint8_t chan_in;
};

int hal_usbhost_open(HAL_USBHOST_PORT_HANDLER port_cb, HAL_USBHOST_DELAY_FUNC delay_fn);

void hal_usbhost_close(void);

void hal_usbhost_halt_chan(uint8_t chan);

void hal_usbhost_port_reset(uint32_t ms);

int hal_usbhost_get_chan(uint8_t *chan);

int hal_usbhost_free_chan(uint8_t chan);

int hal_usbhost_init_chan(uint8_t chan, const struct HAL_USBHOST_CHAN_TYPE_T *type);

int hal_usbhost_update_chan_dev_addr(uint8_t chan, uint8_t dev_addr);

int hal_usbhost_update_chan_mps(uint8_t chan, uint16_t mps);

int hal_usbhost_start_xfer(uint8_t chan, const struct HAL_USBHOST_XFER_T *xfer);

int hal_usbhost_start_setup_xfer(const struct HAL_USBHOST_SETUP_XFER_T *setup, uint32_t *recv_len);

void hal_usbhost_detect(enum HAL_USBHOST_PLUG_STATUS_T status, HAL_USBHOST_PLUG_HANDLER handler);

void hal_usbhost_sleep(void);

void hal_usbhost_wakeup(void);

uint32_t hal_usbhost_get_port_status(void);

uint32_t hal_usbhost_get_interrupt_status(void);

void hal_usbhost_set_port_enable(void);

void hal_usbhost_set_port_disable(void);

void hal_usbhost_set_port_poweron(void);

void hal_usbhost_set_port_poweroff(void);

void hal_usbhost_set_timeout(uint32_t ms);

#ifdef __cplusplus
}
#endif

#endif

