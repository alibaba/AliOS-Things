/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef _USB_CONTROLLER_H
#define _USB_CONTROLLER_H

#include "plat_types.h"
#include "hal_usbhost.h"

//	Config (default uses x bytes)
#define MAX_DEVICES 1				// Max number of devices
#define MAX_ENDPOINTS_TOTAL 8		// Max number of endpoints total
#define MAX_ENDPOINTS_PER_DEVICE (MAX_ENDPOINTS_TOTAL/MAX_DEVICES)// Max number of endpoints for any one device

// USB host structures

#define TOKEN_SETUP 0
#define TOKEN_IN  1
#define TOKEN_OUT 2

//	Status flags from hub
#define PORT_CONNECTION 0
#define PORT_ENABLE  1
#define PORT_SUSPEND  2
#define PORT_OVER_CURRENT 3
#define PORT_RESET 4
#define PORT_POWER 8
#define PORT_LOW_SPEED 9

#define C_PORT_CONNECTION 16
#define C_PORT_ENABLE 17
#define C_PORT_SUSPEND 18
#define C_PORT_OVER_CURRENT 19
#define C_PORT_RESET 20

#define  DEVICE_TO_HOST         0x80
#define  HOST_TO_DEVICE         0x00
#define  REQUEST_TYPE_CLASS     0x20
#define  RECIPIENT_DEVICE       0x00
#define  RECIPIENT_INTERFACE    0x01
#define  RECIPIENT_ENDPOINT		0x02
#define  RECIPIENT_OTHER		0x03

#define  GET_STATUS				0
#define  CLEAR_FEATURE			1
#define  SET_FEATURE			3
#define  SET_ADDRESS			5
#define  GET_DESCRIPTOR			6
#define  SET_DESCRIPTOR			7
#define  GET_CONFIGURATION		8
#define  SET_CONFIGURATION		9
#define  GET_INTERFACE			10
#define  SET_INTERFACE			11
#define  SYNCH_FRAME			11

//===================================================================
//===================================================================
//	Hardware defines
#define USBHOST_CURRENT_CONNECT_STATUS 0x00000001
#define USBHOST_PORT_RESET_STATUS 0x00000010
#define CONNECT_STATUS_CHANGE	(USBHOST_CURRENT_CONNECT_STATUS << 16)
#define PORT_RESET_STATUS_CHANGE	(USBHOST_PORT_RESET_STATUS << 16)

#define  TD_ROUNDING		(uint32_t)0x00040000
#define  TD_SETUP			(uint32_t)0x00000000
#define  TD_IN				(uint32_t)0x00100000
#define  TD_OUT				(uint32_t)0x00080000
#define  TD_DELAY_INT(x)	(uint32_t)((x) << 21)
#define  TD_TOGGLE_0		(uint32_t)0x02000000
#define  TD_TOGGLE_1		(uint32_t)0x03000000
#define  TD_CC				(uint32_t)0xF0000000


#define SKIP_CLEANUP_HW_CHAN                (1 << 0)
#define SKIP_CLEANUP_CLOSE                  (1 << 1)
#define SKIP_CLEANUP_FILE_SYSTEM            (1 << 2)

#define USB_ENDPORT_DMA_BUFFER_LEN  4096
enum USB_ENDPORT_STATE_T
{
	USB_ENDPORT_STATE_FREE,
	USB_ENDPORT_STATE_NOTQUEUED,
	USB_ENDPORT_STATE_IDLE,
	USB_ENDPORT_STATE_SETUPQUEUED,
	USB_ENDPORT_STATE_DATAQUEUED,
	USB_ENDPORT_STATE_STATUSQUEUED,
	USB_ENDPORT_STATE_CALLBACK_PENDING,
	// USB_ENDPORT_STATE_WAIT_PROCESS_DONE,
};

//	HostController EndPoint Descriptor
typedef struct {
	volatile uint32_t	control;
	volatile uint32_t	tail_td;
	volatile uint32_t	head_td;
	volatile uint32_t	next;
} HCED_T;

// HostController Transfer Descriptor
typedef struct {
	volatile uint32_t	control;
	volatile uint32_t	curr_buf_ptr;
	volatile uint32_t	next;
	volatile uint32_t	bufend;
} HCTD_T;

// Host Controller Communication Area
typedef struct {
	volatile uint32_t	InterruptTable[32];
	volatile uint16_t	FrameNumber;
	volatile uint16_t	FrameNumberPad;
	volatile uint32_t	DoneHead;
	volatile uint8_t Reserved[120];
} HCCA;

enum USBHOST_CLASS_CODE_T
{
	USB_CLASS_DEVICE = 0x0,
	USB_CLASS_AUDIO = 0x01,
	USB_CLASS_COMM = 0x02,
	USB_CLASS_HID = 0x03,
	USB_CLASS_PHYSICAL = 0x05,
	USB_CLASS_IMAGING = 0x06,
	USB_CLASS_PRINTER = 0x07,
	USB_CLASS_MASS_STORAGE = 0x08,
	USB_CLASS_HUB = 0x09,
	USB_CLASS_DATA = 0x0a,
	USB_CLASS_SMART_CARD = 0x0b,
	USB_CLASS_CONTENT_SECURITY = 0x0d,
	USB_CLASS_VIDEO = 0x0e,
	USB_CALSS_PERSONAL_HEALTHCARE = 0x0f,
	USB_CLASS_DIAGNOSTIC_DEVICE = 0xdc,
	USB_CLASS_WIRELESS = 0xe0,
	USB_CLASS_MISCELLANEOUS = 0xef,
	USB_CLASS_APP_SPECIFIC = 0xfe,
	USB_CLASS_VENDOR_SPECIFIC = 0xff
};

enum USBHOST_DESCRIPTOR_TYPE
{
    DESCRIPTOR_TYPE_DEVICE = 0x01,
    DESCRIPTOR_TYPE_CONFIG = 0x02,
    DESCRIPTOR_TYPE_STRING = 0x03,
    DESCRIPTOR_TYPE_INTERFACE = 0x04,
    DESCRIPTOR_TYPE_ENDPOINT = 0x05,
    DESCRIPTOR_TYPE_BOS = 0x0f,
    DESCRIPTOR_TYPE_DEVICE_CAPABILITY = 0x10,
    DESCRIPTOR_TYPE_DEVICE_HID = 0x21,
    DESCRIPTOR_TYPE_DEVICE_REPORT = 0x22,
    DESCRIPTOR_TYPE_DEVICE_PHYSICAL = 0x23,
    DESCRIPTOR_TYPE_DEVICE_HUB = 0x29,
    DESCRIPTOR_TYPE_DEVICE_SUPERSPEED_HUB = 0x2a,
    DESCRIPTOR_TYPE_DEVICE_ENDPOINT_COMPANION = 0x2a,
};

enum USBHOST_ENDPOINT_DIRECTION_T
{
    USB_ENDPOINT_DIRECTION_IN = 0x80,
    USB_ENDPOINT_DIRECTION_OUT = 0x0,
};

enum USBHOST_TRANSFER_TYPE_T
{
    USB_TRANSFER_TYPE_CONTROL = 0,
    USB_TRANSFER_TYPE_ISOCHRONOUS = 1,
    USB_TRANSFER_TYPE_BULK = 2,
    USB_TRANSFER_TYPE_INTERRUPT = 3,
};

enum USBHOST_REQUEST_T
{
    USB_REQUEST_GET_STATUS = 0x00,
    USB_REQUEST_CLEAR_FEATURE = 0x01,
    USB_REQUEST_SET_FEATURE = 0x03,
    USB_REQUEST_SET_ADDRESS = 0x05,
    USB_REQUEST_GET_DESCRIPTOR = 0x06,
    USB_REQUEST_SET_DESCRIPTOR = 0x07,
    USB_REQUEST_GET_CONFIGURATION = 0x08,
    USB_REQUEST_SET_CONFIGURATION = 0x09,
    USB_REQUEST_GET_INTERFACE = 0x0a,
    USB_REQUEST_SET_INTERFACE = 0x0b,
    USB_REQUEST_SYNCH_FRAME = 0x0c,
    USB_REQUEST_SET_SEL = 0x30,
    USB_REQUEST_SET_ISOCH_DELAY = 0x31,
};

enum USBHOST_REQUEST_RECIPIENT_T
{
    USB_REQUEST_RECIPIENT_DEVICE = 0x00,
    USB_REQUEST_RECIPIENT_INTERFACE = 0x01,
    USB_REQUEST_RECIPIENT_ENDPOINT = 0x02,
    USB_REQUEST_RECIPIENT_OTHER = 0x03,
};


typedef struct
{
	uint8_t	length;
	uint8_t	descriptor_type;
	uint16_t cdusb;
	uint8_t device_class;
	uint8_t device_sub_class;
	uint8_t device_protocol;
	uint8_t max_packet_size;
	uint16_t vendor;
	uint16_t dproduct;
	uint16_t cddevice;
	uint8_t manu_facturer;
	uint8_t product;
	uint8_t serial_number;
	uint8_t num_configurations;
} USBHOST_DEVICE_DESCRIPTOR_T;

typedef struct
{
	uint8_t	length;
	uint8_t	descriptor_type;
	uint16_t total_length;
	uint8_t	num_interface;
	uint8_t	configuration_value;
	uint8_t	configuration;
	uint8_t	attributes;
	uint8_t	max_power;
} USBHOST_CONFIGURATION_DICRIPTOR_T;

typedef struct
{
	uint8_t	length;
	uint8_t	descriptor_type;
	uint8_t interface_number;
	uint8_t	alternate_setting;
	uint8_t	num_endpoints;
	uint8_t	interface_class;
	uint8_t	interface_sub_class;
	uint8_t	interface_protocol;
	uint8_t	interface;
} USBHOST_INTERFACE_DESCRIPTOR_T;

typedef struct
{
	uint8_t	length;
	uint8_t	descriptor_type;
	uint8_t	endpoint_address;
	uint8_t	attributes;
    uint16_t max_packet_size;
	uint8_t	interval;
    /* NOTE:  these two are _only_ in audio endpoints. */
	/* use USB_DT_ENDPOINT*_SIZE in bLength, not sizeof. */
	uint8_t  refresh;
	uint8_t  synch_address;
} USBHOST_ENDPOINT_DESCRIPTOR_T;

typedef struct {
  uint8_t length;
  uint8_t descriptor_type;
  uint16_t bcdhid;
  uint8_t country_code;
  uint8_t num_descriptors;
  uint8_t descriptor_type2;
  uint16_t descriptor_length;
} USBHOST_HID_DESCRIPTOR_T;

typedef struct {
    uint8_t length;
    uint8_t descriptor_type;
    uint8_t string[1];
} USBHOST_STRING_DESCRIPTION_T;

typedef  void (*USBHOST_INTERRUPT_HANDLE_CB) (void);
typedef  void (*USBHOST_PLUG_HANDLE_CB) (void);

typedef void (*USBHOST_CALLBACK)(int32_t device,\
                                int32_t endpoint,\
                                int32_t status,\
                                uint8_t* data,\
                                int32_t len,\
                                void* user_data);

typedef struct {
    uint8_t chan;
    uint8_t chan_ctrl_in;
    uint32_t dma_buff[USB_ENDPORT_DMA_BUFFER_LEN];
    uint32_t dma_buff_len;
    bool first_xfer;

	HCED_T	descriptor;
	HCTD_T	td_head;
	volatile uint8_t current_state;
	bool wait_process_done_queue;
	uint8_t		flags;
	uint16_t	length;
	uint8_t*		data;
	USBHOST_CALLBACK callback;
	void*  user_data;
}USBHOST_ENDPOINT_T;

typedef struct {
	uint8_t bm_request_type;
	uint8_t b_request;
	uint16_t w_value;
	uint16_t w_index;
	uint16_t w_length;
} USBHOST_SETUP_T;

typedef struct {
    uint8_t	endpoint_map[MAX_ENDPOINTS_PER_DEVICE*2];
	uint8_t	hub;
	uint8_t	port;
	uint8_t	addr;
	uint8_t	pad;

	//	Only if this device is a hub
	uint8_t	hub_port_count;	// nonzero if this is a hub
	uint8_t	hub_interrupt_data;
	uint8_t	hub_map;
	uint8_t	hub_mask;

	int32_t flags;		// 1 = Disconnected

	USBHOST_SETUP_T setup_buffer;
}USBHOST_DEVICE_T;

typedef struct {
    HCCA		communication_area;
    USBHOST_ENDPOINT_T	endpoints[MAX_ENDPOINTS_TOTAL];
    USBHOST_ENDPOINT_T	endpoint_zero;
    HCTD_T		common_tail;
    USBHOST_SETUP_T	setup_zero;

    USBHOST_DEVICE_T	devices[MAX_DEVICES];
    uint32_t	frame_number;			// 32 bit ms counter

    uint8_t	callbacks_pending;		//	Endpoints with callbacks are pending, set from ISR via ProcessDoneQueue
    uint8_t	root_hub_status_change;	//	Root hub status has changed, set from ISR
    enum HAL_USBHOST_PORT_EVENT_T root_hub_event;
    uint8_t	unused0;
    uint8_t	unused1;

    uint8_t	connect_pending;	//	Reset has initiated a connect
    uint32_t	connect_countdown;	//	Number of ms left after port is enabled before we can connect
    uint32_t	disconn_countdown;	//	Number of ms left after reset in case of no events, so that we can disconnect
    uint8_t	connect_hub;		//	Will connect on this hub
    uint8_t	connect_port;		//	... and this port
    uint8_t  skip_cleanup_bitmap;
    uint8_t  plug_status_change;
    enum HAL_USBHOST_PLUG_STATUS_T plug_status;
#ifdef USB_SLEEP_TEST
    uint32_t sleep_count_down;
    uint32_t wakeup_count_down;
    uint32_t fs_test_count_down;
#endif
    USBHOST_INTERRUPT_HANDLE_CB interrupt_cb;
    USBHOST_PLUG_HANDLE_CB plug_cb;
}USBHOST_T;


int usbhost_add_endpoint(int device,
                                int ep,
                                int attributes,
                                int max_packet_size,
                                int interval);

int usbhost_add_endpoint_by_desc(int device,
                                 USBHOST_ENDPOINT_DESCRIPTOR_T* ed);

int usbhost_remove_endpoint(int device, int ep);

USBHOST_ENDPOINT_T* usbhost_get_endpoint(int device, int ep);

void usbhost_set_interrupt_cb(void *cb);

int usbhost_release(USBHOST_ENDPOINT_T* endpoint);

void usbhost_process_done_queue(void);

int usbhost_add_device(int hub, int port, bool isLowSpeed);

int usbhost_init(USBHOST_INTERRUPT_HANDLE_CB interrupt_cb,USBHOST_PLUG_HANDLE_CB plug_cb);

int usbhost_deinit(void);

void usbhost_loop(void);

USBHOST_T* usbhost_get_op(void);

int32_t usbhost_get_frame_number(uint32_t *number);

int32_t usbhost_set_frame_number(uint32_t number);

int32_t usbhost_reset_port(int hub, int port);

int32_t usbhost_resume_port(int hub, int port);

int32_t usbhost_suspend_port(int hub, int port);

int32_t usbhost_disconnect(int hub, int port);

// called after reset
void usbhost_connect(int hub, int port, bool lowspeed);

void usbhost_reconnect_cleanup(int hub, int port);

// Called from interrupt
void usbhost_hub_status_change(int hub, int port, uint32_t status);

int usbhost_wait_io_done(int device, int ep);

int32_t usbhost_transfer(int device, int ep, uint8_t flags, uint8_t* data, int length, USBHOST_CALLBACK callback, void* user_data);

int32_t usbhost_control_transfer(int device, int ep, int request_type, int request, int value, int index, uint8_t* data, int length, int *actual_len, USBHOST_CALLBACK callback, void * user_data);

int32_t  usbhost_interrupt_transfer(int device, int ep, uint8_t* data, int length, USBHOST_CALLBACK callback, void* user_data);

int32_t  usbhost_bulk_transfer(int device, int ep, uint8_t* data, int length, USBHOST_CALLBACK callback, void* user_data);

int32_t usbhost_transfer_abort(int device, int ep);

int32_t usbhost_getdescriptor(int device, int descType,int descIndex, uint8_t* data, int length);

int32_t usbhost_get_string(int device, int index, char* dst, int length);

int32_t usbhost_set_address(int device, int new_addr);

int32_t usbhost_set_configuration(int device, int configNum);

int32_t usbhost_set_interface(int device, int ifNum, int altNum);

//	HUB stuff
int32_t usbhost_set_port_feature(int device, int feature, int index);

int32_t usbhost_clear_port_feature(int device, int feature, int index);

int32_t usbhost_set_port_power(int device, int port);

int32_t usbhost_set_port_reset(int device, int port);

int32_t usbhost_get_port_status(int device, int port, uint32_t* status);

uint32_t usbhost_hcd_get_port_status(void);

uint32_t usbhost_hcd_get_interrupt_status(void);

uint32_t usbhost_hcd_get_plug_status(void);

uint32_t usbhost_hcd_usb_usb_detect(uint32_t plug_status);

int32_t usbhost_hcd_set_port_enable(void);

int32_t usbhost_hcd_set_port_disable(void);

int32_t usbhost_hcd_set_port_poweron(void);

int32_t usbhost_hcd_set_port_poweroff(void);

int32_t usbhost_hcd_set_port_resume(void);

int32_t usbhost_hcd_set_port_suspend(void);

int32_t usbhost_hcd_set_port_reset(void);

int32_t usbhost_plug_thread_start(void);

int32_t usbhost_hcd_set_timeout(uint32_t ms);

#define ERR_IO_PENDING              -100
#define ERR_ENDPOINT_NONE_LEFT      -101
#define ERR_ENDPOINT_NOT_FOUND      -102
#define ERR_DEVICE_NOT_FOUND        -103
#define ERR_DEVICE_NONE_LEFT        -104
#define ERR_HUB_INIT_FAILED         -105
#define ERR_INTERFACE_NOT_FOUND     -106
#define ERR_DATA_SIZE_IS_ZERO       -107
#define ERR_DATA_SIZE_TOO_LARGE     -108
#define ERR_DATA_SIZE_NOT_ALIGNMENT -109

#endif
