/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef _USB_HCD_H
#define _USB_HCD_H
#include "usb_controller.h"

#ifdef __cplusplus
extern "C" {
#endif

#define USB_HCD_NAME  "BES Controller"
#define USB_HCD_ADDR  0x01
#define USB_HCD_VERSION  0x00010001
#define USB_HCD_MAX_PACKET_LEN (64)
#define USB_HCD_AVAILABLE_BANDWIDTH (1024*1024)

#define USB_HCD_NAME_LEN 16
#define USB_HCD_ADDR_LEN 16
#define USB_HCD_PORT_SIGNAL_LEN  16
// USB HCD Port Status
#define USB_HCD_PORT_STATUS_CONN     0x00
#define USB_HCD_PORT_STATUS_EN_LS    0x01
#define USB_HCD_PORT_STATUS_EN_FS    0x02
#define USB_HCD_PORT_EN_HS           0x03
#define USB_HCD_PORT_EN_BAD          0x04
#define USB_HCD_PORT_DISCONN         0x05

// USB HCD Interrupt Status
#define USB_HCD_STS_INT		    0x0001   	/* USB Interrupt */
#define USB_HCD_STS_ERRINT      0x0002	    /* USB Error Interrupt */
#define USB_HCD_STS_PCD         0x0004	    /* Port Change Detect */
#define USB_HCD_STS_FLR         0x0008	    /* Frame List Rellover */
#define USB_HCD_STS_HSE	        0x00010	    /* Host System Error */
#define USB_HCD_STS_IAA	        0x00020	    /* Interrupt on Async Advance */
#define USB_HCD_STS_SRI         0x00080     /* SOF - ECHI Derivation */
#define USB_HCD_STS_HCH   		0x01000	    /* Host Controller has Halted */
#define USB_HCD_STS_RECL    	0x02000  	/* Reclamation */
#define USB_HCD_STS_PSS         0x04000     /* Periodic Schedule Status */
#define USB_HCD_STS_ASS         0x08000     /* Asynchronous Schedule Status */
#define USB_HCD_STS_UAI         0x40000     /* USB Asynchronous Interrupt -EHCI derivation */
#define USB_HCD_STS_UPI         0x80000     /* USB Period Interrupt - EHCI derivation */

typedef enum
{
    HCD_CMD_DISABLE_CONTROLLER = 0,     // USB Host Controller disable.
    HCD_CMD_GET_PORT_STATUS = 1,        // Get USB Port status.
    HCD_CMD_ENABLE_PORT  = 2,           // USB Port enable.
    HCD_CMD_DISABLE_PORT = 3,           // USB Port disable.
    HCD_CMD_POWER_ON_PORT = 4,          // USB Host power on.
    HCD_CMD_POWER_OFF_PORT = 5,         // USB Host power off.
    HCD_CMD_RESUME_PORT = 6,            // USB Port resume.
    HCD_CMD_SUSPEND_PORT = 7,           // USB Port suspend.
    HCD_CMD_RESET_PORT = 8,             // USB Port reset.
    HCD_CMD_GET_FRAME_NUMBER = 9,       // Get Frame Number, for Isochronous Transfer.
    HCD_CMD_SET_FRAME_NUMBER = 10,      // Set Frame Number, for Isochronous Transfer.
    HCD_CMD_TRANSFER_REQUEST = 11,      // Transfer Request.
    HCD_CMD_TRANSFER_ABORT  = 12,       // Transfer Abort.
    HCD_CMD_CREATE_ENDPOINT = 13,       // Create a endpoint.
    HCD_CMD_DESTROY_ENDPOINT = 14,      // Destroy a endpoint.
    HCD_CMD_RESET_ENDPOINT = 15,        // Reset endpoint.
    HCD_CMD_PROCESS_DONE_QUEUE = 16,        // Process a transmission that has already occurred
}HCD_CMD;

typedef enum {
    USB_HCD_STATUS_HALT = 0,
    USB_HCD_STATUS_OPER = 1,
    USB_HCD_STATUS_DEAD = 2,
}USB_HCD_STATUS;

typedef enum {
    USB_HCD_TYPE_FULL_SPEED = 0,
    USB_HCD_TYPE_HIGH_SPEED = 1,
}USB_HCD_TYPE;

typedef enum {
   USB_PORT_STATUS_CONNECTION =     0x01,
   USB_PORT_STATUS_ENABLE =         0x02,
   USB_PORT_STAT_SUSPEND =          0x04,
   USB_PORT_STATUS_OVERCURRENT =    0x08,
   USB_PORT_STATUS_RESET =          0x10,
   USB_PORT_STATUS_L1  =            0x20,
   USB_PORT_STATUS_POWER =          0x100,
   USB_PORT_STATUS_LOW_SPEED =      0x200,
   USB_PORT_STATUS_HIGH_SPEED =     0x400,
   USB_PORT_STATUS_TEST =           0x800,
   USB_PORT_STATUS_INDICATOR  =     0x1000,
}USB_PORT_STATUS;

typedef struct _USB_HCD {
    char              name[USB_HCD_NAME_LEN + 1];
    uint8_t           address[USB_HCD_ADDR_LEN];
    uint32_t          state;
    uint32_t          type;
    uint32_t          otg_capabilities;
    uint32_t          root_hub_port_num;
    uint32_t          root_hub_port_signal[USB_HCD_PORT_SIGNAL_LEN];
    uint32_t          task_signal;
    uint32_t          (*hcd_entry_func)(struct _USB_HCD *, uint32_t, void *);
    uint32_t           root_hub_device_connection;
    uint32_t           device_num;
    uint32_t           io;
    uint32_t           power_switch;
    uint32_t           available_bandwidth;
    uint32_t           maximum_transfer_request_size;
    uint32_t           version;
    void               *controller_hardware;
    void               *hcd_op;
} USB_HCD_T;

// cmd  HCD_CMD_GET_PORT_STATUS Parameter
typedef struct _HCD_GET_PORT_STATUS
{
    uint8_t device;
    uint8_t ep;
}HCD_GET_PORT_STATUS;

typedef struct _HCD_HUB_EP
{
    uint8_t hub;
    uint8_t ep;
}HCD_HUB_EP;


// cmd HCD_CMD_TRANSFER_REQUEST Parameter.
typedef struct _HCD_TRANSFER
{
    uint8_t  device;
    uint8_t  ep;
    uint8_t  flags;
    uint8_t* data;
    uint32_t dat_len;
    USB_CALLBACK callback;
    void* user_data;
}HCD_TRANSFER;

// cmd HCD_CMD_TRANSFER_ABORT Parameter.
typedef struct _HCD_TRANSFER_ABORT
{
    uint8_t  ep;
    uint32_t token;
}HCD_TRANSFER_ABORT;

// cmd  HCD_CMD_CREATE_ENDPOINT Parameter
typedef struct _HCD_CREATE_ENDPOINT
{
    uint8_t device;
    uint8_t ep;
    uint8_t attributes;
    uint32_t max_packet_size;
    uint32_t interval;
}HCD_CREATE_ENDPOINT;

// cmd  HCD_CMD_DESTROY_ENDPOINT Parameter
typedef struct _HCD_DESTROY_ENDPOINT
{
    uint8_t device;
    uint8_t ep;
}HCD_DESTROY_ENDPOINT;

// cmd  HCD_CMD_PROCESS_DONE_QUEUE Parameter
typedef struct _HCD_PROCESS_DONE_QUEUE
{
    uint8_t ep;
    uint32_t status;
}HCD_PROCESS_DONE_QUEUE;


// cmd  HCD_CMD_PROCESS_DONE_QUEUE Parameter

typedef  void (*HCD_INTERRUPT_HANDLE_CB) (void);

int usb_hcd_init(USB_HCD_T * hcd);

int usb_register_interrupt_cb(HCD_INTERRUPT_HANDLE_CB handle);

int usb_hcd_cmd(USB_HCD_T * hcd,HCD_CMD cmd, void* param);

// Get interrupt status, return USB HCD Interrupt Status.
int usb_hcd_get_interrupt_status(void);

// Get port status, return USB HCD Port Status.
int usb_hcd_get_port_status(void);


#ifdef __cplusplus
}
#endif
#endif //_USB_HCD_H

