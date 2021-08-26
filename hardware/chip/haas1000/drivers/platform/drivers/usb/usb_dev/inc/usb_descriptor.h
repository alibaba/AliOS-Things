/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __USB_DESCRIPTOR_H__
#define __USB_DESCRIPTOR_H__

#ifdef __cplusplus
extern "C" {
#endif

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

/* bmRequestType.direction */
#define HOST_TO_DEVICE (0)
#define DEVICE_TO_HOST (1)

/* bmRequestType.type*/
#define STANDARD_TYPE  (0)
#define CLASS_TYPE     (1)
#define VENDOR_TYPE    (2)
#define RESERVED_TYPE  (3)

/* bmRequestType.recipient */
#define DEVICE_RECIPIENT    (0)
#define INTERFACE_RECIPIENT (1)
#define ENDPOINT_RECIPIENT  (2)
#define OTHER_RECIPIENT     (3)

/* Descriptors */
#define DESCRIPTOR_TYPE(wValue)  (wValue >> 8)
#define DESCRIPTOR_INDEX(wValue) (wValue & 0xff)

/* Device status */
#define DEVICE_STATUS_SELF_POWERED  (1U<<0)
#define DEVICE_STATUS_REMOTE_WAKEUP (1U<<1)

/* Endpoint status */
#define ENDPOINT_STATUS_HALT        (1U<<0)

/* Standard feature selectors */
#define DEVICE_REMOTE_WAKEUP        (1)
#define ENDPOINT_HALT               (0)
#define TEST_MODE                   (2)

/* Test mode selectors */
#define TEST_MODE_TEST_J            (1)
#define TEST_MODE_TEST_K            (2)
#define TEST_MODE_TEST_SE0_NAK      (3)
#define TEST_MODE_TEST_PACKET       (4)
#define TEST_MODE_FORCE_ENABLE      (5)

/* Standard descriptor types */
#define DEVICE_DESCRIPTOR           (1)
#define CONFIGURATION_DESCRIPTOR    (2)
#define STRING_DESCRIPTOR           (3)
#define INTERFACE_DESCRIPTOR        (4)
#define ENDPOINT_DESCRIPTOR         (5)
#define QUALIFIER_DESCRIPTOR        (6)
#define BOS_DESCRIPTOR              (0xF)

/* Standard descriptor lengths */
#define DEVICE_DESCRIPTOR_LENGTH        (0x12)
#define CONFIGURATION_DESCRIPTOR_LENGTH (0x09)
#define INTERFACE_DESCRIPTOR_LENGTH     (0x09)
#define ENDPOINT_DESCRIPTOR_LENGTH      (0x07)
#define QUALIFIER_DESCRIPTOR_LENGTH     (0x0A)
#define BOS_DESCRIPTOR_LENGTH           (0x05)

/*string offset*/
#define STRING_OFFSET_LANGID            (0)
#define STRING_OFFSET_IMANUFACTURER     (1)
#define STRING_OFFSET_IPRODUCT          (2)
#define STRING_OFFSET_ISERIAL           (3)
#define STRING_OFFSET_ICONFIGURATION    (4)
#define STRING_OFFSET_IINTERFACE        (5)

/* USB Specification Release Number */
#define USB_VERSION_2_0 (0x0200)
#define USB_VERSION_1_1 (0x0101)

/* Least/Most significant byte of short integer */
#define LSB(n)  ((n)&0xff)
#define MSB(n)  (((n)&0xff00)>>8)

#define LSB0(n) ((n) & 0xFF)
#define LSB1(n) (((n) >> 8) & 0xFF)
#define LSB2(n) (((n) >> 16) & 0xFF)
#define LSB3(n) (((n) >> 24) & 0xFF)

/* Convert physical endpoint number to descriptor endpoint number */
#define PHY_TO_DESC(endpoint, dir) (((endpoint) & 0xF) | ((dir == EP_OUT) ? 0 : 0x80))

/* bmAttributes in configuration descriptor */
/* C_RESERVED must always be set */
#define C_RESERVED      (1U<<7)
#define C_SELF_POWERED  (1U<<6)
#define C_REMOTE_WAKEUP (1U<<5)

/* bMaxPower in configuration descriptor */
#define C_POWER(mA)     ((mA)/2)

/* bmAttributes in endpoint descriptor */
#define E_CONTROL       (0x00)
#define E_ISOCHRONOUS   (0x01)
#define E_BULK          (0x02)
#define E_INTERRUPT     (0x03)

/* For isochronous endpoints only: */
#define E_NO_SYNCHRONIZATION    (0x00)
#define E_ASYNCHRONOUS          (0x04)
#define E_ADAPTIVE              (0x08)
#define E_SYNCHRONOUS           (0x0C)
#define E_DATA                  (0x00)
#define E_FEEDBACK              (0x10)
#define E_IMPLICIT_FEEDBACK     (0x20)

#ifdef __cplusplus
}
#endif

#endif

