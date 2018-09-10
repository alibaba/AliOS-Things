/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */


#ifndef USB_DEVICE_TYPES_H
#define USB_DEVICE_TYPES_H


#ifdef __cplusplus
 extern "C" {
#endif

#include "usb_device_os.h"


/* Standard requests */
#define GET_STATUS        (0)
#define CLEAR_FEATURE     (1)
#define SET_FEATURE       (3)
#define SET_ADDRESS       (5)
#define GET_DESCRIPTOR    (6)
#define SET_DESCRIPTOR    (7)
#define GET_CONFIGURATION (8)
#define SET_CONFIGURATION (9)
#define GET_INTERFACE     (10)
#define SET_INTERFACE     (11)

/* Request type for data transfer direction */
#define HOST_TO_DEVICE (0)
#define DEVICE_TO_HOST (1)

/* Request type */
#define STANDARD_TYPE  (0)
#define CLASS_TYPE     (1)
#define VENDOR_TYPE    (2)
#define RESERVED_TYPE  (3)

/* Request type for recipient */
#define DEVICE_RECIPIENT    (0)
#define INTERFACE_RECIPIENT (1)
#define ENDPOINT_RECIPIENT  (2)
#define OTHER_RECIPIENT     (3)

/* Descriptors */
#define DESCRIPTOR_TYPE(wValue)  (wValue >> 8)
#define DESCRIPTOR_INDEX(wValue) (wValue & 0xff)

typedef struct {
    struct {
		uint8_t data_transfer_direction;
        uint8_t Type;
        uint8_t Recipient;
    } bmRequestType;
    uint8_t  bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} setup_packet_t;

typedef struct {
    setup_packet_t setup;
    uint8_t *ptr;
    uint32_t remaining;
    uint8_t direction;
    bool zlp;
    bool notify;
} control_transfer_t;

typedef enum {ATTACHED, POWERED, DEFAULT, ADDRESS, CONFIGURED} DEVICE_STATE;

typedef struct {
    volatile DEVICE_STATE state;
    uint8_t configuration;
    bool suspended;
} usbd_info_t;

#ifdef __cplusplus
}
#endif

#endif /* USB_DEVICE_TYPES_H */
