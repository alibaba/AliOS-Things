/******************************************************************************
 *
 * Freescale Semiconductor Inc.
 * (c) Copyright 2004-2009 Freescale Semiconductor, Inc.
 * ALL RIGHTS RESERVED.
 *
 **************************************************************************//*!
 *
 * @file usb_descriptor.h
 *
 * @author
 *
 * @version
 *
 * @date May-28-2009
 *
 * @brief The file is a header file for USB Descriptors required for Mouse
 *        Application
 *****************************************************************************/

#ifndef _USB_DESCRIPTOR_MOUSE_H
#define _USB_DESCRIPTOR_MOUSE_H

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "types.h"
#include "usb_class.h"
#include "usb_descriptor.h"

/******************************************************************************
 * Constants - None
 *****************************************************************************/

/******************************************************************************
 * Macro's
 *****************************************************************************/
#define REMOTE_WAKEUP_SHIFT              (5)
#define REMOTE_WAKEUP_SUPPORT            (TRUE)

/* Various descriptor sizes */
#define DEVICE_DESCRIPTOR_SIZE            (18)
#define CONFIG_DESC_SIZE                  (34)
#define DEVICE_QUALIFIER_DESCRIPTOR_SIZE  (10)
#define REPORT_DESC_SIZE                  (52)
#define CONFIG_ONLY_DESC_SIZE             (9)
#define IFACE_ONLY_DESC_SIZE              (9)
#define HID_ONLY_DESC_SIZE                (9)
#define ENDP_ONLY_DESC_SIZE               (7)

/* HID buffer size */
#define HID_BUFFER_SIZE                   (8)


#define HID_DESC_ENDPOINT_COUNT (1)
#define HID_ENDPOINT (1)
#define HID_ENDPOINT_PACKET_SIZE (8)


/* string descriptors sizes */
// #define USB_STR_DESC_SIZE (2)

#define USB_STR_0_SIZE  (2)
#define USB_STR_1_SIZE  (56)
#define USB_STR_2_SIZE  (34)
#define USB_STR_n_SIZE  (32)


#endif
