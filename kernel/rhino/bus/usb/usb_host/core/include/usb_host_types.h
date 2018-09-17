/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __USB_HOST_TYPES_H
#define __USB_HOST_TYPES_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "usb_host_conf.h"
#include "usb_host_os.h"
#include "usb_host_debug.h"
#include "usb_host_spec.h"
#include "usb_hcd.h"

/*
 * Return Value Assert
 */
#define RETURN_ASSERT(res, msg)            \
    do {                                   \
        if (res != USBH_OK) {              \
            usb_err("%s : %d", msg, res);  \
            return res;                    \
        }                                  \
    } while (0);

#define MIN(a, b) ((a > b) ? b : a)
#define LE16(addr) (((uint16_t)(*((uint8_t *)(addr)))) \
                    + (((uint16_t)(*(((uint8_t *)(addr)) + 1))) << 8))

/*
 * USB Host Status
 */
#define USBH_OK     0
#define USBH_FAIL   1

/*
 * USB URB Transfer Status
 */
#define USB_URB_IDLE        0
#define USB_URB_DONE        1
#define USB_URB_NOTREADY    2
#define USB_URB_NYET        3
#define USB_URB_ERROR       4
#define USB_URB_STALL       5

/*
 * USB Transmition Token
 */
#define USBH_TOKEN_SETUP   0
#define USBH_TOKEN_DATA    1

#define USB_HOST_INUSE 1
#define USB_HOST_UNUSE 0

/* 
 * Default Device Address
 */
#define USB_HOST_DEVICE_ADDRESS     1

#define USB_HOST_WAIT_FOREVER ((uint32_t)-1)

/*
 * USB Host Pipe Managment Structure (Endpoint)
 */
typedef struct usb_host_pipe {
    uint8_t inuse;
    uint8_t index;
    uint8_t ep_addr;
    uint8_t dev_addr;
    uint8_t ep_type; 
    uint8_t speed;
    uint16_t mps;
} usbh_pipe_t;

/*
 * USB Interface Management Structure
 */
typedef struct usb_host_interface {
    uint8_t inuse;
    uint8_t intf_class;
    uint8_t intf_subclass;
    uint8_t intf_protocol;
    usbh_pipe_t *pipe[USBH_MAX_PIPE_PER_INTERFACE];
} usbh_intf_t;

/*
 * USB Device Management Structure
 */
typedef struct usb_host_device {
    uint8_t attached;
    uint8_t enumerated;
    uint8_t nb_intf;
    uint8_t dev_class;
    uint8_t dev_subclass;
    uint8_t dev_protocol;
    uint8_t speed;
    uint8_t address;
    uint16_t vid;
    uint16_t pid;
    uint8_t data[USBH_MAX_DATA_BUFFER];
    usbh_intf_t intf[USBH_MAX_NUM_INTERFACE];
    void (*disconnect_cb)(void);
} usbh_device_t;

/*
 * USB Host Management Handler
 */
typedef struct usb_host_handle {
    uint8_t         inited;
    usbh_device_t   device;
    usbh_pipe_t     pipes[USBH_MAX_PIPE_NUM];
    usbh_os_queue_t queue;
    usbh_os_task_t  task;
    usbh_os_lock_t  lock;
    usbh_pipe_t*    control;
    void*           hcd;
} usb_host_t;

/*
 * Enumerate Helper Function Handler
 */
typedef struct enumerator_helper {
    void (*set_vid_pid) (uint16_t vid, uint16_t pid);

    int  (*parse_interface) (uint8_t intf_nb, uint8_t intf_class, 
                             uint8_t intf_subclass, uint8_t intf_protocol);

    int  (*parse_endpoint) (uint8_t intf_nb, uint8_t ep_type, uint8_t ep_addr);
} enum_helper_t;

#ifdef __cplusplus
}
#endif

#endif /* __USB_HOST_TYPES_H */
