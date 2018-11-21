/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __USB_HOST_SPEC_H
#define __USB_HOST_SPEC_H

#ifdef __cplusplus
 extern "C" {
#endif

/*
 * Data Phase Transfer Direction
 */
#define USB_HOST_TO_DEVICE 0x00
#define USB_DEVICE_TO_HOST 0x80
#define USB_DIRECTION_MASK 0x80

/*
 * USB Endpoint Type
 */
#define USB_HOST_ENDPOINT_CONTROL       0
#define USB_HOST_ENDPOINT_ISOCHRONOUS   1
#define USB_HOST_ENDPOINT_BULK          2
#define USB_HOST_ENDPOINT_INTERRUPT     3

/*
 * USB Spec: Standard Request Codes (Table 9-4)
 */
#define USB_REQUEST_GET_STATUS          0x00
#define USB_REQUEST_CLEAR_FEATURE       0x01
#define USB_REQUEST_SET_FEATURE         0x03
#define USB_REQUEST_SET_ADDRESS         0x05
#define USB_REQUEST_GET_DESCRIPTOR      0x06
#define USB_REQUEST_SET_DESCRIPTOR      0x07
#define USB_REQUEST_GET_CONFIGURATION   0x08
#define USB_REQUEST_SET_CONFIGURATION   0x09
#define USB_REQUEST_GET_INTERFACE       0x0A
#define USB_REQUEST_SET_INTERFACE       0x0B
#define USB_REQUEST_SYNCH_FRAME         0x0C

/*
 * USB Request Type: bmRequestType D6..5
 */
#define USB_REQUEST_TYPE_STANDARD       0x00
#define USB_REQUEST_TYPE_CLASS          0x20
#define USB_REQUEST_TYPE_VENDOR         0x40

/*
 * USB Request Recipient: bmRequestType D4..0
 */
#define USB_REQUEST_RECIPIENT_DEVICE    0x00
#define USB_REQUEST_RECIPIENT_INTERFACE 0x01
#define USB_REQUEST_RECIPIENT_ENDPOINT  0x02
#define USB_REQUEST_RECIPIENT_OTHER     0x03

/*
 * USB Descriptor Type
 */
#define USB_DESC_TYPE_DEVICE              0x01
#define USB_DESC_TYPE_CONFIGURATION       0x02
#define USB_DESC_TYPE_STRING              0x03
#define USB_DESC_TYPE_INTERFACE           0x04
#define USB_DESC_TYPE_ENDPOINT            0x05
#define USB_DESC_TYPE_DEVICE_QUALIFIER    0x06
#define USB_DESC_TYPE_OTHER               0x07

/*
 * USB Feature Type
 */
#define USB_FEATURE_SELECTOR_ENDPOINT   0x00
#define USB_FEATURE_SELECTOR_DEVICE     0x01

/*
 * USB Descriptor Length
 */
#define USB_DEVICE_DESCRIPTOR_LENGTH        0x12
#define USB_CONFIGURATION_DESCRIPTOR_LENGTH 0x09

/*
 * USB Descriptor Header
 */
typedef struct {
    uint8_t bLength;
    uint8_t bDescriptorType;
} usb_desc_header_t;

/*
 * USB Device Descriptor
 */
typedef struct {
    uint8_t     bLength;
    uint8_t     bDescriptorType;
    uint16_t    bcdUSB;
    uint8_t     bDeviceClass;
    uint8_t     bDeviceSubClass;
    uint8_t     bDeviceProtocol;
    uint8_t     bMaxPacketSize;
    uint16_t    idVendor;
    uint16_t    idProduct;
    uint16_t    bcdDevice;
    uint8_t     iManufacturer;
    uint8_t     iProduct;
    uint8_t     iSerialNumber;
    uint8_t     bNumConfigurations;
} usb_dev_desc_t;

/*
 * USB Configuration Descriptor
 */
typedef struct {
    uint8_t     bLength;
    uint8_t     bDescriptorType;
    uint16_t    wTotalLength;
    uint8_t     bNumInterfaces;
    uint8_t     bConfigurationValue;
    uint8_t     iConfiguration;
    uint8_t     bmAttributes;
    uint8_t     bMaxPower;
} usb_cfg_desc_t;

/*
 * USB Interface Descriptor
 */
typedef struct {
    uint8_t     bLength;
    uint8_t     bDescriptorType;
    uint8_t     bInterfaceNumber;
    uint8_t     bAlternateSetting;
    uint8_t     bNumEndpoints;
    uint8_t     bInterfaceClass;
    uint8_t     bInterfaceSubClass;
    uint8_t     bInterfaceProtocol;
    uint8_t     iInterface;
} usb_itf_desc_t;

/*
 * USB Endpoint Descriptor
 */
typedef struct {
    uint8_t     bLength;
    uint8_t     bDescriptorType;
    uint8_t     bEndpointAddress;
    uint8_t     bmAttributes;
    uint16_t    wMaxPacketSize;
    uint8_t     bInterval;
} usb_ep_desc_t;

/*
 * USB Setup Packet
 */
typedef struct usb_setup_packet {
    uint8_t  bmRequestType;
    uint8_t  bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} usb_setup_pkt_t;

#ifdef __cplusplus
}
#endif

#endif /* __USB_HOST_SPEC_H */