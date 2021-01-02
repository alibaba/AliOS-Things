/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef _USB_HOST_TYPES_H
#define _USB_HOST_TYPES_H

#include <stdint.h>

#include "k_api.h"
#include "aos/kernel.h"
#include "usb_host_hcd_hal.h"

#define  MAX_HCD_NAME_LEN      63
#define  MAX_CLASS_NAME_LEN    63
#define  USB_MAX_TT              8

struct _usb_host_class;
struct _usb_host_class_cmd;
struct _usb_device_descriptor;
struct _usb_hub_tt;
struct _usb_device;
struct _usb_endpoint;
struct _usb_interface;
struct _usb_endpoint_descriptor;
struct _usb_interface_descriptor;
struct _usb_configuration;
struct _usb_configuration_descriptor;
struct _usb_interface_association_descriptor;
struct _usb_transfer;
struct _usb_host_controller_device;
struct _usb_host;

typedef struct _usb_host_class {
   char                name[MAX_CLASS_NAME_LEN + 1];
   uint32_t            status;
   uint32_t            (*entry_function) (struct _usb_host_class_cmd *) ;
   uint32_t            owned_devices_num;
   void                *first_instance;
   void                *client;
   //TX_THREAD         ux_host_class_thread;
   void                *thread_stack;
   void                *class_media;
} usb_host_class_t;

typedef struct _usb_host_class_cmd {
    uint32_t         request;
    void             *container;
    void             *instance;
    uint32_t          usage;
    uint32_t          pid;
    uint32_t          vid;
    uint32_t          class;
    uint32_t          subclass;
    uint32_t          protocol;
    uint32_t          iad_class;
    uint32_t          iad_subclass;
    uint32_t          iad_protocol;
    struct _usb_host_class  *class_ptr;
} usb_host_class_cmd_t;

typedef struct _usb_device_descriptor
{
    uint32_t           bLength;
    uint32_t           bDescriptorType;
    uint32_t           bcdUSB;
    uint32_t           bDeviceClass;
    uint32_t           bDeviceSubClass;
    uint32_t           bDeviceProtocol;
    uint32_t           bMaxPacketSize0;
    uint32_t           idVendor;
    uint32_t           idProduct;
    uint32_t           bcdDevice;
    uint32_t           iManufacturer;
    uint32_t           iProduct;
    uint32_t           iSerialNumber;
    uint32_t           bNumConfigurations;
} usb_device_descriptor_t;

typedef struct _usb_hub_tt
{
    uint32_t           port_mapping;
    uint32_t           max_bandwidth;
} usb_hub_tt_t;

typedef struct _usb_endpoint_descriptor
{
   uint32_t     bLength;
   uint32_t     bDescriptorType;
   uint32_t     bEndpointAddress;
   uint32_t     bmAttributes;
   uint32_t     wMaxPacketSize;
   uint32_t     bInterval;
} usb_endpoint_descriptor_t;

typedef struct _usb_transfer
{
    uint32_t          request_status;
    struct _usb_endpoint   *endpoint;
    char             *data_pointer;
    uint32_t          requested_length;
    uint32_t          actual_length;
    uint32_t          type;
    uint32_t          function;
    uint32_t          value;
    uint32_t          index;
    void            (*completion_function) (struct _usb_transfer *);
    void *      semaphore;
    void            *class_instance;
    uint32_t          maximum_length;
    uint32_t          timeout_value;
    uint32_t          completion_code;
    uint32_t          packet_length;
    struct _usb_transfer   *next;
    void               *user_specific;
} usb_transfer_t;

typedef struct _usb_endpoint {
   uint32_t      endpoint;
   uint32_t      state;
   void          *ed;
   struct _usb_endpoint_descriptor desc;
   struct _usb_interface   *interface;           
   struct _usb_device      *device;
   struct _usb_transfer    transfer_request;
   struct _usb_endpoint    *next;
} usb_endpoint_t;

typedef struct _usb_device {
   uint32_t             handle;
   uint32_t             type;
   uint32_t             state;
   uint32_t             address;
   uint32_t             speed;
   uint32_t             port_location;
   uint32_t             max_power;
   uint32_t             power_source;
   uint32_t             current_configuration;
   void *            protection_semaphore;
   struct _usb_device           *parent;
   struct _usb_host_class        *class;
   void                          *class_instance;
   struct _usb_hcd               *hcd;
   struct _usb_configuration     *first_configuration;
   struct _usb_device_descriptor  desc;
   struct _usb_endpoint           control_endpoint;
   struct _usb_hub_tt             hub_tt[USB_MAX_TT];
   struct _usb_device             *next;
} usb_device_t;

typedef struct _usb_interface_descriptor
{
    uint32_t           bLength;
    uint32_t           bDescriptorType;
    uint32_t           bInterfaceNumber;
    uint32_t           bAlternateSetting;
    uint32_t           bNumEndpoints;
    uint32_t           bInterfaceClass;
    uint32_t           bInterfaceSubClass;
    uint32_t           bInterfaceProtocol;
    uint32_t           iInterface;
} usb_interface_descriptor_t;

typedef struct _usb_interface {
    uint32_t        handle;
    uint32_t        state;
    struct _usb_host_class     *class;
    struct _usb_endpoint *first_endpoint;
    struct _usb_interface *next;
    uint32_t        curr_alt_setting;
    usb_interface_descriptor_t   desc;
    void                 *class_instance;
    struct _usb_configuration  *config;
    uint32_t           iad_class;
    uint32_t           iad_subclass;
    uint32_t           iad_protocol;
} usb_interface_t;

typedef struct _usb_configuration_descriptor
{
    uint32_t           bLength;
    uint32_t           bDescriptorType;
    uint32_t           wTotalLength;
    uint32_t           bNumInterfaces;
    uint32_t           bConfigurationValue;
    uint32_t           iConfiguration;
    uint32_t           bmAttributes;
    uint32_t           MaxPower;
} usb_configuration_descriptor_t;

typedef struct _usb_configuration
{
   uint32_t        handle;
   uint32_t        state;
   uint32_t        otg_capabilities;
   usb_configuration_descriptor_t   config_desc;
   struct _usb_interface      *first_interface;                          
   struct _usb_configuration  *next;            
   struct _usb_device         *device;
   uint32_t            iad_class;
   uint32_t            iad_subclass;
   uint32_t            iad_protocol;
} usb_configuration_t;

typedef struct _usb_host_controller_device {
   char            name[MAX_HCD_NAME_LEN + 1];    /* hcd name */
   uint8_t         address[16];                   /* hcd address */
   uint32_t        state;                         /* hcd state */
   uint32_t        type;                          /* hcd type */
   uint32_t        otg_capabilities;              /* otg capability */
   uint32_t        root_hub_port_num;             /* number of root hub port */
   uint32_t        root_hub_port_signal[16];      /* port signal of root hub */
   uint32_t        task_signal;                   /* task signal count */
   uint32_t      (*hcd_entry_func)(struct _usb_hcd *, uint32_t, void *);   /* hcd command function */
   uint32_t        root_hub_device_connection;    /* connection status of root hub  */
   uint32_t        device_num;                    /* number of device connected */
   uint32_t        io;                            /* io */
   uint32_t        power_switch;                  /* power switch support */
   uint32_t        available_bandwidth;           /* max bandwidth  */
   uint32_t        maximum_transfer_request_size; /* max transfer request size */
   uint32_t        version;                       /* version */
   void           *controller_hardware;           /* pointer to context maintained by HCD layer */
   usb_host_hcd_op_t  *hcd_op;
} usb_hcd_t;

typedef struct _usb_host {
    /* host class */
    uint32_t max_class_num;
    uint32_t reg_class_num;
    struct _usb_host_class  *class_array;
    
    /* host controller device */
    uint32_t max_hcd_num;
    uint32_t reg_hcd_num;
    usb_hcd_t *hcd_array;

    /* host device */
    struct _usb_device  *device_array;
    uint32_t      max_device_num;

    uint32_t      max_endpoint_descriptor_num; 
    uint32_t      max_transfer_descrpipter_num;
    uint32_t      max_iso_transfer_descrpipter_num;

    /* host worker */
    void *       enum_sem;
    void            (*enum_hub_function) (void);
    void *      enum_task;

    void *       hcd_sem; 
    void *      hcd_task;

    int            (*change_function) (uint32_t, struct _usb_host_class *, void *);
    void *         poll_task;
} usb_host_t;

/* Interface Association Descriptor structure.  */
typedef struct _usb_interface_association_descriptor
{
    uint32_t           bLength;
    uint32_t           bDescriptorType;
    uint32_t           bFirstInterface;
    uint32_t           bInterfaceCount;
    uint32_t           bFunctionClass;
    uint32_t           bFunctionSubClass;
    uint32_t           bFunctionProtocol;
    uint32_t           iFunction;
} usb_interface_association_descriptor_t;

#endif /* _USB_HOST_TYPES_H */
