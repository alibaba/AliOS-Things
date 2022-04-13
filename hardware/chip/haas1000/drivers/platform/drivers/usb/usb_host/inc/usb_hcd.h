/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef _USB_HCD_H
#define _USB_HCD_H
#include "usb_host.h"

#ifdef __cplusplus
extern "C" {
#endif

#define USB_HCD_NAME  "BES_CTROL"
#define USB_HCD_ADDR  0x01
#define USB_HCD_VERSION  0x00010001
#define USB_HCD_MAX_PACKET_LEN (64)
#define USB_HCD_AVAILABLE_BANDWIDTH (1024*1024)

#define USB_HCD_NAME_LEN 16
#define USB_HCD_ADDR_LEN 16
#define USB_HCD_PORT_SIGNAL_LEN  16

#define USB_HCD_TOKEN_SETUP 0
#define USB_HCD_TOKEN_IN  1
#define USB_HCD_TOKEN_OUT 2

#define USB_HCD_MAX_ENDPOINTS 6
#if 0
// register GINTSTS
// USB HCD Interrupt Status
#define USB_HCD_STS_INT		    0x0001   	/* USB Interrupt */  //PrtInt
#define USB_HCD_STS_ERRINT      0x0002	    /* USB Error Interrupt */
#define USB_HCD_STS_PCD         0x0004	    /* Port Change Detect */ //ConIDStsChng
#define USB_HCD_STS_FLR         0x0008	    /* Frame List Rellover */
#define USB_HCD_STS_HSE	        0x00010	    /* Host System Error */
#define USB_HCD_STS_IAA	        0x00020	    /* Interrupt on Async Advance */
#define USB_HCD_STS_SRI         0x00080     /* SOF - ECHI Derivation */ //Sof
#define USB_HCD_STS_HCH   		0x01000	    /* Host Controller has Halted */
#define USB_HCD_STS_RECL    	0x02000  	/* Reclamation */
#define USB_HCD_STS_PSS         0x04000     /* Periodic Schedule Status */
#define USB_HCD_STS_ASS         0x08000     /* Asynchronous Schedule Status */
#define USB_HCD_STS_UAI         0x40000     /* USB Asynchronous Interrupt -EHCI derivation */
#define USB_HCD_STS_UPI         0x80000     /* USB Period Interrupt - EHCI derivation */
#endif

typedef enum {
    USB_HCD_STATUS_HALT = 0,
    USB_HCD_STATUS_OPER = 1,
    USB_HCD_STATUS_DEAD = 2,
}USB_HCD_STATUS_T;

typedef enum {
    USB_HCD_TYPE_FULL_SPEED = 0,
    USB_HCD_TYPE_HIGH_SPEED = 1,
}USB_HCD_TYPE_T;

// Register GINTSTS
typedef enum {
    USB_HCD_INTSTS_UNKNOWN       =  0,
    // Current Mode of Operation (CurMod)
    // 1'b0: Device mode
    // 1'b1: Host mode
    USB_HCD_INTSTS_CURMOD        = (0x1<<0),

    // *
    // RxFIFO Non-Empty (RxFLvl)
    USB_HCD_INTSTS_RXFLVL        = (0x1<<4),

    // *
    // Incomplete Periodic Transfer (incomplP)
    USB_HCD_INTSTS_FETSUSP       = (0x1<<22),

    // *
    // Host Port Interrupt (PrtInt)
    USB_HCD_INTSTS_PRTINT        = (0x1<<24),

    // *
    // Host Channels Interrupt (HChInt)
    USB_HCD_INTSTS_HCHINT        = (0x1<<25),

    // Periodic TxFIFO Empty (PTxFEmp)
    USB_HCD_INTSTS_PTXFEMP       = (0x1<<26),

    // *
    // Connector ID Status Change (ConIDStsChng)
    USB_HCD_INTSTS_CONLDSTSCHNG  = (0x1<<28),

    // *
    // Disconnect Detected Interrupt (DisconnInt)
    USB_HCD_INTSTS_DISCONNINT    = (0x1<<29),

    // Session Request/New Session Detected Interrupt (SessReqInt)
    USB_HCD_INTSTS_SESSREQINT    = (0x1<<30),

    // Resume/Remote Wakeup Detected Interrupt (WkUpInt)
    USB_HCD_INTSTS_WKUPINT       = (0x1<<31),
}USB_HCD_INTSTS_T;

// Register HPRT
typedef enum {
   USB_PORT_STATUS_UNKNOWN    =     0x00,
   USB_PORT_STATUS_CONNECTION =     (0x01 << 0),
   USB_PORT_STATUS_ENABLE =         (0x01 << 1),
   USB_PORT_STAT_SUSPEND =          (0x01 << 2),
   USB_PORT_STATUS_OVERCURRENT =    (0x01 << 3),
   USB_PORT_STATUS_RESET =          (0x01 << 4),
   USB_PORT_STATUS_POWER =          (0x01 << 5),
   USB_PORT_STATUS_FULL_SPEED =     (0x01 << 6),
   USB_PORT_STATUS_HIGH_SPEED =     (0x01 << 7),
}USB_PORT_STATUS_T;


typedef enum {
    USB_HCD_PLUG_STATUS_UNKNOWN,
    USB_HCD_PLUG_STATUS_IN,
    USB_HCD_PLUG_STATUS_OUT,
}USB_HCD_PLUG_STATUS_T;

typedef enum {
    USB_HCD_EP_TYPE_CTRL,
    USB_HCD_EP_TYPE_ISO,
    USB_HCD_EP_TYPE_BULK,
    USB_HCD_EP_TYPE_INT,
    USB_HCD_EP_TYPE_QTY
}USB_HCD_EP_TYPE_T;

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
    HCD_CMD_CONTROL_TRANSFER_REQUEST = 12,      // Transfer Request.
    HCD_CMD_TRANSFER_ABORT  = 13,       // Transfer Abort.
    HCD_CMD_CREATE_ENDPOINT = 14,       // Create a endpoint.
    HCD_CMD_DESTROY_ENDPOINT = 15,      // Destroy a endpoint.
    HCD_CMD_RESET_ENDPOINT = 16,        // Reset endpoint.
    HCD_CMD_PROCESS_DONE_QUEUE = 17,        // Process a transmission that has already occurred
}HCD_CMD_T;

typedef  void (*HCD_INTERRUPT_HANDLE_CB) (void);

typedef  void (*HCD_PLUG_HANDLE_CB) (void);
typedef void (*HCD_TRANSFER_CALLBACK)(int32_t device,\
                                int32_t endpoint,\
                                int32_t status,\
                                uint8_t* data,\
                                int32_t length,\
                                void* user_data);

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
typedef struct _HCD_DEVICE_PORT_T
{
    uint8_t device;
    uint8_t port;
}HCD_DEVICE_PORT_T;

typedef struct _HCD_DEVICE_EP_T
{
    uint8_t device;
    uint8_t ep;
}HCD_DEVICE_EP_T;

typedef struct _HCD_HUB_EP_T
{
    uint8_t hub;
    uint8_t ep;
}HCD_HUB_EP_T;


// cmd HCD_CMD_TRANSFER_REQUEST Parameter.
typedef struct _HCD_TRANSFER_T
{
    uint8_t  device;
    uint8_t  ep;
    uint8_t  flags;
    uint8_t* data;
    uint32_t data_len;
    HCD_TRANSFER_CALLBACK callback;
    void* user_data;
}HCD_TRANSFER_T;

// cmd HCD_CMD_TRANSFER_REQUEST(CONTROL) Parameter.
typedef struct _HCD_CONTROL_TRANSFER_T
{
    uint8_t  device;
    uint8_t  ep;
    int request_type;
    int request;
    int value;
    int index;
    uint8_t* data;
    int length;
    int actual_len;
    USBHOST_CALLBACK callback;
    void * user_data;
}HCD_CONTROL_TRANSFER_T;


// cmd  HCD_CMD_CREATE_ENDPOINT Parameter
typedef struct _HCD_CREATE_ENDPOINT_T
{
    uint8_t device;
    uint8_t ep;
    uint8_t attributes;
    uint32_t max_packet_size;
    uint32_t interval;
}HCD_CREATE_ENDPOINT_T;

typedef  void (*HCD_INTERRUPT_HANDLE_CB) (void);

typedef  void (*HCD_PLUG_HANDLE_CB) (void);

int usb_hcd_init(USB_HCD_T * hcd, HCD_INTERRUPT_HANDLE_CB int_handle, HCD_PLUG_HANDLE_CB plug_handle);

int usb_register_interrupt_cb(HCD_INTERRUPT_HANDLE_CB handle);

int usb_hcd_cmd(USB_HCD_T * hcd,HCD_CMD_T cmd, void* param);

// Get interrupt status, return USB HCD Interrupt Status.
int usb_hcd_get_interrupt_status(void);

// Get port status, return USB HCD Port Status.
int usb_hcd_get_port_status(void);

// Get usb plug status, return USB HCD Plug Status.
int usb_hcd_get_plug_status(void);

// Detect usb device plug staus.
int usb_hcd_detect(USB_HCD_PLUG_STATUS_T plug_status);

void usb_hcd_set_timeout(uint32_t ms);

void usb_hcd_log_disable_for_debug(bool disable);

#ifdef __cplusplus
}
#endif
#endif //_USB_HCD_H

