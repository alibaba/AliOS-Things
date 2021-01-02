#ifndef _USB_SLAVE_TYPES_H
#define _USB_SLAVE_TYPES_H

#include "usb_host_types.h"
#include "usb_slave_default_config.h"

#define USB_SETUP_SIZE  8

struct _usb_slave_class;
struct _usb_slave_endpoint;

typedef struct _usb_slave_dcd
{
    uint8_t         name[32];
    uint8_t         status;
    uint8_t         controller_type;
    uint8_t         otg_capabilities;
    uint8_t         irq;
    uint32_t        io;
    uint32_t        address;
    uint32_t        (*function) (struct _usb_slave_dcd *, uint32_t, void *);
    void            *controller_hardware;
} usb_slave_dcd_t;

typedef struct _usb_slave_transfer
{
    uint32_t                     request_status;
    uint32_t                     request_type;
    struct _usb_slave_endpoint  *request_endpoint;
    uint8_t                     *data_pointer;
    uint8_t                     *request_current_data_pointer;
    uint32_t                     request_requested_length;
    uint32_t                     request_actual_length;
    uint32_t                     request_in_transfer_length;
    uint32_t                     request_completion_code;
    uint32_t                     request_phase;
    void  (*request_completion_function) (struct _usb_slave_transfer *);
    void                         *semaphore;
    uint32_t                     request_timeout;
    uint32_t                     request_force_zlp;
    uint8_t                      request_setup[USB_SETUP_SIZE];
    uint32_t                     request_status_phase_ignore;
} usb_slave_transfer_t;

typedef struct _usb_slave_endpoint
{
    uint32_t          status;
    uint32_t          state;
    void              *ed;  
    struct _usb_endpoint_descriptor desc;
    struct _usb_slave_endpoint      *next;
    struct _usb_slave_interface     *interface;
    struct _usb_slave_device        *device;
    struct _usb_slave_transfer       transfer_request;
} usb_slave_endpoint_t;

typedef struct _usb_slave_interface
{                        
    uint32_t                          status;
    struct _usb_slave_class          *class;
    void                             *class_instance;
    struct _usb_interface_descriptor   desc;
    struct _usb_slave_interface      *next_interface;
    struct _usb_slave_endpoint       *first_endpoint;
} usb_slave_interface_t;

typedef struct _usb_slave_class
{
    uint8_t         name[MAX_CLASS_NAME_LEN + 1]; /* "+1" for string null-terminator */
    uint8_t         status;
    uint8_t         (*entry_function) (struct UX_SLAVE_CLASS_COMMAND_STRUCT *) ;
    void            *instance;
    void            *client;
    void            *task;
    void            *task_stack;
    void            *interface_parameter;                    
    uint32_t         interface_number;                    
    uint32_t         configuration_number;                    
    struct usb_slave_interface      
                     *interface;
} usb_slave_class_t;

typedef struct _usb_slave_class_cmd
{

    uint32_t         request;
    void            *container;
    void            *interface;
    uint32_t         pid;
    uint32_t         vid;
    uint32_t         class;
    uint32_t         subclass;
    uint32_t         protocol;
    struct _usb_slave_class     
                    *class_ptr;
    void            *parameter;
    void            *interface_number;
} usb_slave_class_cmd_t;

typedef struct _usb_slave_device
{                
    uint32_t                             state;   
    struct _usb_device_descriptor        desc;
    struct _usb_slave_endpoint           control_endpoint;
    uint32_t                             configuration_selected;
    struct _usb_configuration_descriptor configuration_descriptor;
    struct _usb_slave_interface          *first_interface;
    struct _usb_slave_interface          *interfaces_pool;
    uint32_t                              interfaces_pool_number;
    struct _usb_slave_endpoint           *endpoints_pool;
    uint32_t                              endpoints_pool_number;
    uint32_t                              power_state;
} usb_slave_device_t;

typedef struct _usb_slave
{
    struct _usb_slave_dcd    dcd;
    struct _usb_slave_device device;
    uint8_t           * device_framework;
    uint32_t            device_framework_length;
    uint8_t           * device_framework_full_speed;
    uint32_t            device_framework_length_full_speed;
    uint8_t           * device_framework_high_speed;
    uint32_t            device_framework_length_high_speed;
    uint8_t           * string_framework;
    uint32_t            string_framework_length;
    uint8_t           * language_id_framework;
    uint32_t            language_id_framework_length;
    uint8_t           * dfu_framework;
    uint32_t            dfu_framework_length;
    uint32_t             max_class;
    uint32_t             registered_class;
    struct _usb_slave_class  * class_array;
    struct _usb_slave_class  * interface_class_array[USB_MAX_SLAVE_INTERFACES];
    uint32_t            speed;
    uint32_t            power_state;
    uint32_t            remote_wakeup_capability;
    uint32_t            remote_wakeup_enabled;
    uint32_t            device_dfu_capabilities;
    uint32_t            device_dfu_detach_timeout;
    uint32_t            device_dfu_transfer_size;
    uint32_t            device_dfu_state_machine;
    uint32_t            device_dfu_mode;
    uint32_t            (* change_function) (uint32_t);
    uint32_t            device_vendor_request;
    uint32_t            (* device_vendor_request_function) (uint32_t, uint32_t, uint32_t, uint32_t, uint8_t *, uint32_t *);
} usb_slave_t;

#endif /* _USB_DEVICE_TYPES_H */
