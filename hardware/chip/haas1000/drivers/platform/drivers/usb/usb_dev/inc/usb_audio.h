/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __USB_AUDIO_H__
#define __USB_AUDIO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "hal_usb.h"

enum USB_AUDIO_ITF_ID_T {
    USB_AUDIO_ITF_ID_RECV,
    USB_AUDIO_ITF_ID_SEND,
#ifdef USB_AUDIO_MULTIFUNC
    USB_AUDIO_ITF_ID_RECV2,
#endif

    USB_AUDIO_ITF_ID_QTY
};

enum USB_AUDIO_ITF_CMD_T {
    USB_AUDIO_ITF_STOP,
    USB_AUDIO_ITF_START_16BIT,
    USB_AUDIO_ITF_START_24BIT,
    USB_AUDIO_ITF_START_32BIT,

    USB_AUDIO_ITF_CMD_QTY
};

enum USB_AUDIO_STATE_EVENT_T {
    USB_AUDIO_STATE_RESET, // RESET event should be processed as quickly as possible
    USB_AUDIO_STATE_DISCONNECT,
    USB_AUDIO_STATE_SLEEP,
    USB_AUDIO_STATE_WAKEUP,
    USB_AUDIO_STATE_CONFIG,
    USB_AUDIO_STATE_RECV_PAUSE,
    USB_AUDIO_STATE_RECV_CONTINUE,
    USB_AUDIO_STATE_SET_RECV_RATE,
    USB_AUDIO_STATE_SET_SEND_RATE,
    USB_AUDIO_STATE_RECV2_PAUSE,
    USB_AUDIO_STATE_RECV2_CONTINUE,

    USB_AUDIO_STATE_EVENT_QTY
};

enum USB_AUDIO_HID_EVENT_T {
    USB_AUDIO_HID_VOL_UP        = (1 << 0),
    USB_AUDIO_HID_VOL_DOWN      = (1 << 1),
    USB_AUDIO_HID_PLAY_PAUSE    = (1 << 2),
    USB_AUDIO_HID_SCAN_NEXT     = (1 << 3),
    USB_AUDIO_HID_SCAN_PREV     = (1 << 4),
    USB_AUDIO_HID_STOP          = (1 << 5),
    USB_AUDIO_HID_FAST_FWD      = (1 << 6),
    USB_AUDIO_HID_REWIND        = (1 << 7),
    USB_AUDIO_HID_VOL_MUTE      = (1 << 8),
    USB_AUDIO_HID_VOICE_CMD     = (1 << 9),
    USB_AUDIO_HID_REDIAL        = (1 << 10),
    USB_AUDIO_HID_HOOK_SWITCH   = (1 << 11),
    USB_AUDIO_HID_MIC_MUTE      = (1 << 12),
    USB_AUDIO_HID_VOICE_MAIL    = (1 << 13),
    USB_AUDIO_HID_RESERVED_14   = (1 << 14),
    USB_AUDIO_HID_RESERVED_15   = (1 << 15),
};

struct USB_AUDIO_XFER_INFO_T {
    const uint8_t *data;
    uint32_t size;
    uint32_t next_size;
    bool pool_enabled;
    int8_t cur_compl_err;
    int8_t next_xfer_err;
};

struct USB_AUDIO_VENDOR_MSG_T {
    const struct SETUP_PACKET *pkt;
    uint8_t *data;
    uint16_t length;
};

typedef void (*USB_AUDIO_ITF_CALLBACK)(enum USB_AUDIO_ITF_ID_T id, enum USB_AUDIO_ITF_CMD_T cmd);
typedef void (*USB_AUDIO_MUTE_CALLBACK)(enum USB_AUDIO_ITF_ID_T id, uint32_t mute);
typedef void (*USB_AUDIO_SET_VOLUME)(enum USB_AUDIO_ITF_ID_T id, uint32_t percent);
typedef uint32_t (*USB_AUDIO_GET_VOLUME)(enum USB_AUDIO_ITF_ID_T id);
typedef void (*USB_AUDIO_XFER_CALLBACK)(enum USB_AUDIO_ITF_ID_T id, const struct USB_AUDIO_XFER_INFO_T *info);
typedef void (*USB_AUDIO_STATE_CALLBACK)(enum USB_AUDIO_STATE_EVENT_T event, uint32_t param);
typedef void (*HID_XFER_CALLBACK)(enum USB_AUDIO_HID_EVENT_T event, int error);
typedef int (*USB_AUDIO_VENDOR_MSG_CALLBACK)(struct USB_AUDIO_VENDOR_MSG_T *msg);

struct USB_AUDIO_CFG_T {
    uint32_t recv_sample_rate;
    uint32_t send_sample_rate;
    uint8_t *vendor_rx_buf;
    uint16_t vendor_rx_size;
    USB_AUDIO_ITF_CALLBACK itf_callback;
    USB_AUDIO_MUTE_CALLBACK mute_callback;
    USB_AUDIO_SET_VOLUME set_volume;
    USB_AUDIO_GET_VOLUME get_volume;
    USB_AUDIO_STATE_CALLBACK state_callback;
    USB_AUDIO_XFER_CALLBACK xfer_callback;
    HID_XFER_CALLBACK hid_send_callback;
    USB_AUDIO_VENDOR_MSG_CALLBACK vendor_msg_callback;
};

int usb_audio_open(const struct USB_AUDIO_CFG_T *cfg);

void usb_audio_close(void);

int usb_audio_recv_ready(void);

int usb_audio_send_ready(void);

int usb_audio_start_recv(uint8_t *buf, uint32_t pos, uint32_t size);

void usb_audio_stop_recv(void);

int usb_audio_start_send(const uint8_t *buf, uint32_t pos, uint32_t size);

void usb_audio_stop_send(void);

void usb_audio_pause_send(void);

int usb_audio_continue_send(uint32_t pos);

int usb_audio_set_recv_pos(uint32_t pos);

int usb_audio_set_send_pos(uint32_t pos);

void usb_audio_hid_set_event(enum USB_AUDIO_HID_EVENT_T event, int state);

const char *usb_audio_get_hid_event_name(enum USB_AUDIO_HID_EVENT_T event);

int usb_audio_start_recv2(uint8_t *buf, uint32_t pos, uint32_t size);

void usb_audio_stop_recv2(void);

int usb_audio_set_recv2_pos(uint32_t pos);

#ifdef __cplusplus
}
#endif

#endif
