#ifndef _USB_SLAVE_DEFINES_H
#define _USB_SLAVE_DEFINES_H
                                                                        
#define USB_DCD_STATUS_HALTED         0
#define USB_DCD_STATUS_OPERATIONAL    1
#define USB_DCD_STATUS_DEAD           2

#define USB_SLAVE_CLASS_COMMAND_QUERY                                    1
#define USB_SLAVE_CLASS_COMMAND_ACTIVATE                                 2
#define USB_SLAVE_CLASS_COMMAND_DEACTIVATE                               3
#define USB_SLAVE_CLASS_COMMAND_REQUEST                                  4
#define USB_SLAVE_CLASS_COMMAND_INITIALIZE                               5
#define USB_SLAVE_CLASS_COMMAND_CHANGE                                   6
#define USB_SLAVE_CLASS_COMMAND_UNINITIALIZE                             7

/* DCD command */                                                                 
#define USB_DCD_DISABLE_CONTROLLER                                       1
#define USB_DCD_GET_PORT_STATUS                                          2
#define USB_DCD_ENABLE_PORT                                              3
#define USB_DCD_DISABLE_PORT                                             4
#define USB_DCD_POWER_ON_PORT                                            5
#define USB_DCD_POWER_DOWN_PORT                                          6
#define USB_DCD_SUSPEND_PORT                                             7
#define USB_DCD_RESUME_PORT                                              8
#define USB_DCD_RESET_PORT                                               9
#define USB_DCD_GET_FRAME_NUMBER                                         10
#define USB_DCD_SET_FRAME_NUMBER                                         11
#define USB_DCD_TRANSFER_REQUEST                                         12
#define USB_DCD_TRANSFER_ABORT                                           13
#define USB_DCD_CREATE_ENDPOINT                                          14
#define USB_DCD_DESTROY_ENDPOINT                                         15
#define USB_DCD_RESET_ENDPOINT                                           16
#define USB_DCD_SET_DEVICE_ADDRESS                                       17
#define USB_DCD_ISR_PENDING                                              18
#define USB_DCD_CHANGE_STATE                                             19
#define USB_DCD_STALL_ENDPOINT                                           20
#define USB_DCD_ENDPOINT_STATUS                                          21

/* Define feature selector constants.  */
#define USB_REQUEST_FEATURE_ENDPOINT_HALT 0
#define USB_REQUEST_FEATURE_DEVICE_REMOTE_WAKEUP 1
#define USB_REQUEST_FEATURE_TEST_MODE 2

/* device control driver function */
enum {
    DCD_DISABLE_CONTROLLER  =                                 1,
    DCD_GET_PORT_STATUS  =                                    2,
    DCD_ENABLE_PORT      =                                    3,
    DCD_DISABLE_PORT     =                                    4,
    DCD_POWER_ON_PORT    =                                    5,
    DCD_POWER_DOWN_PORT  =                                    6,
    DCD_SUSPEND_PORT     =                                    7,
    DCD_RESUME_PORT      =                                    8,
    DCD_RESET_PORT       =                                    9,
    DCD_GET_FRAME_NUMBER =                                   10,
    DCD_SET_FRAME_NUMBER =                                   11,
    DCD_TRANSFER_REQUEST =                                   12,
    DCD_TRANSFER_ABORT   =                                   13,
    DCD_CREATE_ENDPOINT   =                                  14,
    DCD_DESTROY_ENDPOINT  =                                  15,
    DCD_RESET_ENDPOINT     =                                 16,
    DCD_SET_DEVICE_ADDRESS =                                 17,
    DCD_ISR_PENDING        =                                 18,
    DCD_CHANGE_STATE       =                                 19,
    DCD_STALL_ENDPOINT     =                                 20,
    DCD_ENDPOINT_STATUS    =                                 21,
};
                                                                        
      

#endif

