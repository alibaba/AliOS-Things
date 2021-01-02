/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef _USB_HOST_DEFINES_H
#define _USB_HOST_DEFINES_H

#define USB_FALSE 0
#define USB_TRUE  1

/* return value */
enum {
   USB_SUCCESS = 0,
   USB_FAILED  = -1,
   USB_NO_MEM  = -2,
   USB_TOO_MANY_DEV = -3,
   USB_SEM_ERROR = -4,
};

#define USB_UNUSED                    0
#define USB_USED                      1

#define  ROOT_HUB_ENUMERATION_RETRY               3
#define  ROOT_HUB_ENUMERATION_RETRY_DELAY_MS      100

#define USB_MAX_DEVICE_NUM                        127

#define USB_DEVICE_BUS_POWERED                                           1
#define USB_DEVICE_SELF_POWERED                                          2
#define USB_MAX_SELF_POWER                                               (500/2)
#define USB_MAX_BUS_POWER                                                (100/2)
#define USB_CONFIGURATION_DEVICE_BUS_POWERED                             0x80
#define USB_CONFIGURATION_DEVICE_SELF_POWERED                            0x40
#define USB_STATUS_DEVICE_SELF_POWERED                                   1



#define USB_CONFIGURATION_DESCRIPTOR_LEN  9
                                                                     
#define USB_ENDPOINT_HALT                                                0

#define USB_MAX_PACKET_SIZE_MASK                                         0x7ff
#define USB_MAX_NUMBER_OF_TRANSACTIONS_MASK                              0x1800
#define USB_MAX_NUMBER_OF_TRANSACTIONS_SHIFT                             11

#define USB_OTG_BM_ATTRIBUTES                                            2
#define USB_OTG_SRP_SUPPORT                                              1
#define USB_OTG_HNP_SUPPORT                                              2
#define USB_HCD_OTG_CAPABLE                                              1
#define USB_DCD_OTG_CAPABLE                                              1


#define USB_SETUP_REQUEST_TYPE                                           0
#define USB_SETUP_REQUEST                                                1
#define USB_SETUP_VALUE                                                  2
#define USB_SETUP_INDEX                                                  4
#define USB_SETUP_LENGTH                                                 6
#define USB_SETUP_SIZE                                                   8

#ifndef USB_TIMER_EXTENSION_PTR_GET
#define USB_TIMER_EXTENSION_PTR_GET(a, b, c)             (a) = (b *)(c);
#endif

#ifndef USB_THREAD_EXTENSION_PTR_GET
#define USB_THREAD_EXTENSION_PTR_GET(a, b, c)            (a) = (b *)(c);
#endif


#define USB_OTG_DESCRIPTOR_ENTRIES                          4
#define USB_OTG_DESCRIPTOR_LENGTH                           5

#define USB_DEVICE_QUALIFIER_DESCRIPTOR_ENTRIES                          9
#define USB_DEVICE_QUALIFIER_DESCRIPTOR_LENGTH                           10


enum {
   USB_REQUEST_DIRECTION =  0x80,
   USB_REQUEST_TYPE = 0x60,
   USB_REQUEST_TARGET = 0x03,
};

enum {
   USB_REQUEST_IN = 0x80,
   USB_REQUEST_OUT = 0x00,
};

enum {
   USB_REQUEST_TYPE_STANDARD = 0x00,
   USB_REQUEST_TYPE_CLASS = 0x20,
   USB_REQUEST_TYPE_VENDOR = 0x40,
};

enum {
   USB_REQUEST_TARGET_DEVICE = 0x00,
   USB_REQUEST_TARGET_INTERFACE = 0x01,
   USB_REQUEST_TARGET_ENDPOINT = 0x02,
   USB_REQUEST_TARGET_OTHER = 0x03,
};
                                                 
#define USB_ENDPOINT_DIRECTION_MASK   0x80
enum {
   USB_ENDPOINT_IN  = 0x80,
   USB_ENDPOINT_OUT = 0x00,
};


#define USB_DEVICE_DESCRIPTOR_LENGTH  18
#define USB_DEVICE_DESCRIPTOR_ENTRIES 14

#define USB_CONFIGURATION_DESCRIPTOR_ENTRIES  8
#define USB_CONFIGURATION_DESCRIPTOR_LENGTH  9

#define USB_INTERFACE_DESCRIPTOR_ENTRIES  9
#define USB_INTERFACE_DESCRIPTOR_LENGTH   9

#define USB_ENDPOINT_DESCRIPTOR_ENTRIES    6
#define USB_ENDPOINT_DESCRIPTOR_LENGTH     7

#define USB_INTERFACE_ASSOCIATION_DESCRIPTOR_ENTRIES         8
#define USB_INTERFACE_ASSOCIATION_DESCRIPTOR_LENGTH          8

#define USB_OTG_FEATURE_B_HNP_ENABLE                                     3           
#define USB_OTG_FEATURE_A_HNP_SUPPORT                                    4           
#define USB_OTG_FEATURE_A_ALT_HNP_SUPPORT                                5           
#define USB_OTG_STATUS_SELECTOR                                          0xF000
#define USB_OTG_HOST_REQUEST_FLAG                                        0x01


/* Descriptor item */
enum {
   USB_DEVICE_DESCRIPTOR_ITEM = 1,
   USB_CONFIGURATION_DESCRIPTOR_ITEM = 2,
   USB_STRING_DESCRIPTOR_ITEM = 3,
   USB_INTERFACE_DESCRIPTOR_ITEM = 4,
   USB_ENDPOINT_DESCRIPTOR_ITEM = 5,
   USB_DEVICE_QUALIFIER_DESCRIPTOR_ITEM = 6,
   USB_OTHER_SPEED_DESCRIPTOR_ITEM = 7,
   USB_OTG_DESCRIPTOR_ITEM = 9,
   USB_INTERFACE_ASSOCIATION_DESCRIPTOR_ITEM = 11,
   USB_DFU_FUNCTIONAL_DESCRIPTOR_ITEM = 0x21,
   USB_HUB_DESCRIPTOR_ITEM = 0x29,
};

/* transfer status */
enum {
   USB_TRANSFER_STATUS_NOT_PENDING  = 0,
   USB_TRANSFER_STATUS_PENDING = 1,
   USB_TRANSFER_STATUS_COMPLETED = 2,
   USB_TRANSFER_STATUS_ABORT = 4,
};

/* USB commands. */                                   
enum {
   USB_GET_STATUS = 0,
   USB_CLEAR_FEATURE = 1,
   USB_SET_FEATURE = 3,
   USB_SET_ADDRESS = 5,
   USB_GET_DESCRIPTOR = 6,
   USB_SET_DESCRIPTOR = 7,
   USB_GET_CONFIGURATION = 8,
   USB_SET_CONFIGURATION = 9,
   USB_GET_INTERFACE = 10,
   USB_SET_INTERFACE = 11,
   USB_SYNCH_FRAME = 12,
}; 

/* host class commands */
enum {
   HOST_CLASS_CMD_USAGE_PIDVID = 1,
   HOST_CLASS_CMD_USAGE_CSP = 2,
};

enum {
   HOST_CLASS_CMD_QUERY = 1,
   HOST_CLASS_CMD_ACTIVATE  = 2,
   HOST_CLASS_CMD_DEACTIVATE = 3,
};

enum {
   HOST_CLASS_INSTANCE_FREE = 0,
   HOST_CLASS_INSTANCE_LIVE = 1,
   HOST_CLASS_INSTANCE_SHUTDOWN = 2,
   HOST_CLASS_INSTANCE_MOUNTING = 3,
};

/* speed constant */
enum {  
   LOW_SPEED_DEVICE = 0,
   FULL_SPEED_DEVICE = 1,
   HIGH_SPEED_DEVICE = 2, 
};

enum {
   ENDPOINT_STATUS_RESET = 0,
   ENDPOINT_STATUS_RUNNING = 1,
   ENDPOINT_STATUS_HALTED = 2, 
};

enum {
   DEFAULT_MPS  = 8,
   DEFAULT_HIGH_SPEED_MPS = 64,
};

enum {
   HCD_STATUS_HALT = 0,
   HCD_STATUS_OPER = 1,
   HCD_STATUS_DEAD = 2,
};

/* HCD control */
enum {
   HCD_DISABLE_CONTROLLER = 0,
   HCD_GET_PORT_STATUS = 1,
   HCD_ENABLE_PORT  = 2,
   HCD_DISABLE_PORT = 3,
   HCD_POWER_ON_PORT = 4,
   HCD_POWER_OFF_PORT = 5,
   HCD_RESUME_PORT = 6,
   HCD_SUSPEND_PORT = 7,
   HCD_RESET_PORT = 8,
   HCD_GET_FRAME_NUMBER = 9,
   HCD_SET_FRAME_NUMBER = 10,
   HCD_TRANSFER_REQUEST = 11, 
   HCD_TRANSFER_ABORT  = 12,
   HCD_CREATE_ENDPOINT = 13,
   HCD_DESTROY_ENDPOINT = 14,
   HCD_RESET_ENDPOINT = 15,
   HCD_PROCESS_DONE_QUEUE = 16,
};
    
/* port status */
enum {
   PORT_STATUS_CCS =   0x01,
   PORT_STATUS_CPE =   0x01,
   PORT_STATUS_PES =   0x02,
   PORT_STATUS_PSS =   0x04,
   PORT_STATUS_POCI =  0x08,
   PORT_STATUS_PRS  =  0x10,
   PORT_STATUS_PPS =   0x20,
   PORT_STATUS_DS_LS = 0x00,
   PORT_STATUS_DS_FS = 0x40,
   PORT_STATUS_DS_HS = 0x80,
   PORT_STATUS_DS  =  6,
};

/* device status */
enum {
   DEV_RESET = 0,
   DEV_ATTACHED = 1,
   DEV_ADDRESSED = 2,
   DEV_CONFIGURED = 3,
   DEV_SUSPENED = 4,
   DEV_RESUMED = 5,
   DEV_SELF_POWERED_STATE = 6,
   DEV_BUS_POWERED_STATE = 7,
   DEV_REMOTE_WAKEUP = 8,
   DEV_BUS_RESET_COMPLETED = 9,
   DEV_REMOVED = 10,
   DEV_FORCE_DISCON = 11,
};

#define USB_DEVICE_INSERTION 1
#define USB_DEVICE_REMOVAL   2    

/* error code */
enum {
    MEMORY_ARRAY_FULL = 0x1a,
    NO_TD_AVAILABLE = 0x13,
    NO_ED_AVAILABLE  = 0x14,
    SEMAPHORE_ERROR = 0x15,

    TRANSFER_STALLED = 0x21,
    TRANSFER_NO_ANSWER = 0x22,
    TRANSFER_ERROR = 0x23,
    TRANSFER_MISSED_FRAME = 0x24,
    TRANSFER_NOT_READY = 0x25,
    TRANSFER_BUS_RESET = 0x26,
    TRANSFER_BUFFER_OVERFLOW = 0x27,
    TRANSFER_APPLICATION_RESET = 0x28,
    TRANSFER_DATA_LESS_THAN_EXPECTED = 0x29,
   
    NO_BANDWIDTH_AVAILABLE =  0x41,
    DESCRIPTOR_CORRUPTED = 0x42,
    OVER_CURRENT_CONDITION =  0x43,
    DEVICE_ENUMERATION_FAIL = 0x44,
    DEVICE_HANDLE_UNKNOWN =   0x50,
    CONFIGURATION_HANDLE_UNKNOWN = 0x51,
    INTERFACE_HANDLE_UNKNOWN = 0x52,
    ENDPOINT_HANDLE_UNKNOWN = 0x53,
    FUNCTION_NOT_SUPPORTED = 0x54,
    CONTROLLER_UNKNOWN  = 0x55,
    PORT_INDEX_UNKNOWN = 0x56,
    NO_CLASS_MATCH = 0x57,
    HOST_CLASS_ALREADY_INSTALLED = 0x58,
    HOST_CLASS_UNKNOWN = 0x59,
    CONNECTION_INCOMPATIBLE = 0x5a,
    HOST_CLASS_INSTANCE_UNKNOWN = 0x5b, 
    TRANSFER_TIMEOUT = 0x5c,   
    BUFFER_OVERFLOW  = 0x5d,                         
};

#define MASK_ENDPOINT_TYPE  3
enum {
   ENDPOINT_TYPE_CONTROL = 0,
   ENDPOINT_TYPE_ISOCHRONOUS = 1,
   ENDPOINT_TYPE_BULK = 2,
   ENDPOINT_TYPE_INTERRUPT = 3,
};

enum {
   TRANSFER_PHASE_SETUP = 1,
   TRANSFER_PHASE_DATA_IN = 2,
   TRANSFER_PHASE_DATA_OUT = 3,
   TRANSFER_PHASE_STATUS_IN = 4,
   TRANSFER_PHASE_STATUS_OUT = 5,
};

#define USB_CONTROL_TRANSFER_TIMEOUT                                     1000
#define USB_NON_CONTROL_TRANSFER_TIMEOUT                                 5000
#define USB_PORT_ENABLE_WAIT                                             50 
#define USB_DEVICE_ADDRESS_SET_WAIT                                      50
#define USB_HIGH_SPEED_DETECTION_HANDSHAKE_SUSPEND_WAIT                  200
#define USB_ENUMERATION_THREAD_WAIT                                      200


#endif /* _USB_HOST_DEFINES */
