/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef _USB_CONTROLLER_H
#define _USB_CONTROLLER_H


#include "plat_types.h"
#include "hal_usbhost.h"
//	Config (default uses x bytes)
#define MAX_DEVICES 8				// Max number of devices
#define MAX_ENDPOINTS_TOTAL 16		// Max number of endpoints total
#define MAX_ENDPOINTS_PER_DEVICE 8	// Max number of endpoints for any one device

// USB host structures

#define USB_RAM_SIZE 16*1024	// AHB SRAM block 1 TODO MACHINE DEPENDENT
#define USB_RAM_BASE 0x2007C000

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
/*
#define ENDPOINT_CONTROL 0
#define ENDPOINT_ISOCRONOUS 1
#define ENDPOINT_BULK 2
#define ENDPOINT_INTERRUPT 3

#define  DESCRIPTOR_TYPE_DEVICE			1
#define  DESCRIPTOR_TYPE_CONFIGURATION	2
#define  DESCRIPTOR_TYPE_STRING			3
#define  DESCRIPTOR_TYPE_INTERFACE		4
#define  DESCRIPTOR_TYPE_ENDPOINT		5

#define DESCRIPTOR_TYPE_HID         0x21
#define DESCRIPTOR_TYPE_REPORT      0x22
#define DESCRIPTOR_TYPE_PHYSICAL    0x23
#define DESCRIPTOR_TYPE_HUB         0x29
*/
//===================================================================
//===================================================================
//	Hardware defines

//	HcControl
#define PeriodicListEnable	0x00000004
#define	IsochronousEnable	0x00000008
#define	ControlListEnable	0x00000010
#define	BulkListEnable		0x00000020
#define	OperationalMask		0x00000080
#define	HostControllerFunctionalState	0x000000C0

//	HcCommandStatus
#define HostControllerReset	0x00000001
#define ControlListFilled	0x00000002
#define BulkListFilled		0x00000004

//	HcInterruptStatus Register
#define	WritebackDoneHead		0x00000002
#define	StartofFrame			0x00000004
#define ResumeDetected			0x00000008
#define UnrecoverableError		0x00000010
#define FrameNumberOverflow		0x00000020
#define RootHubStatusChange		0x00000040
#define OwnershipChange			0x00000080
#define MasterInterruptEnable	0x80000000

//	HcRhStatus
#define SetGlobalPower			0x00010000
#define DeviceRemoteWakeupEnable	0x00008000

//	HcRhPortStatus (hub 0, port 1)
#define CurrentConnectStatus	0x00000001
#define	PortEnableStatus		0x00000002
#define PortSuspendStatus		0x00000004
#define PortOverCurrentIndicator	0x00000008
#define PortResetStatus			0x00000010

#define PortPowerStatus			0x00000100
#define LowspeedDevice			0x00000200
#define HighspeedDevice			0x00000400

#define CONNECT_STATUS_CHANGE	(CurrentConnectStatus << 16)
#define PORT_RESET_STATUS_CHANGE	(PortResetStatus << 16)

#define  TD_ROUNDING		(u32)0x00040000
#define  TD_SETUP			(u32)0x00000000
#define  TD_IN				(u32)0x00100000
#define  TD_OUT				(u32)0x00080000
#define  TD_DELAY_INT(x)	(u32)((x) << 21)
#define  TD_TOGGLE_0		(u32)0x02000000
#define  TD_TOGGLE_1		(u32)0x03000000
#define  TD_CC				(u32)0xF0000000


#define SKIP_CLEANUP_HW_CHAN                (1 << 0)
#define SKIP_CLEANUP_CLOSE                  (1 << 1)
#define SKIP_CLEANUP_FILE_SYSTEM            (1 << 2)

#define USB_ENDPORT_DMA_BUFFER_LEN  4096
enum USB_ENDPORT_STATE
{
	USB_ENDPORT_STATE_FREE,
	USB_ENDPORT_STATE_NOTQUEUED,
	USB_ENDPORT_STATE_IDLE,
	USB_ENDPORT_STATE_SETUPQUEUED,
	USB_ENDPORT_STATE_DATAQUEUED,
	USB_ENDPORT_STATE_STATUSQUEUED,
	USB_ENDPORT_STATE_CALLBACK_PENDING,
};

//	HostController EndPoint Descriptor
typedef struct {
	volatile uint32_t	control;
	volatile uint32_t	tail_td;
	volatile uint32_t	head_td;
	volatile uint32_t	next;
} HCED;

// HostController Transfer Descriptor
typedef struct {
	volatile uint32_t	control;
	volatile uint32_t	curr_buf_ptr;
	volatile uint32_t	next;
	volatile uint32_t	bufend;
} HCTD;

// Host Controller Communication Area
typedef struct {
	volatile u32	InterruptTable[32];
	volatile u16	FrameNumber;
	volatile u16	FrameNumberPad;
	volatile u32	DoneHead;
	volatile u8		Reserved[120];
} HCCA;

enum USB_CLASS_CODE
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

enum USB_DESCRIPTOR_TYPE
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

enum USB_ENDPOINT_DIRECTION
{
    USB_ENDPOINT_DIRECTION_IN = 0x80,
    USB_ENDPOINT_DIRECTION_OUT = 0x0,
};

enum USB_TRANSFER_TYPE
{
    USB_TRANSFER_TYPE_CONTROL = 0,
    USB_TRANSFER_TYPE_ISOCHRONOUS = 1,
    USB_TRANSFER_TYPE_BULK = 2,
    USB_TRANSFER_TYPE_INTERRUPT = 3,
};

enum USB_REQUEST
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

enum USB_REQUEST_RECIPIENT
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
} USB_DEVICE_DESCRIPTOR;

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
} USB_CONFIGURATION_DICRIPTOR;

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
} USB_INTERFACE_DESCRIPTOR;

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
} USB_ENDPOINT_DESCRIPTOR;

typedef struct {
  uint8_t length;
  uint8_t descriptor_type;
  uint16_t bcdhid;
  uint8_t country_code;
  uint8_t num_descriptors;
  uint8_t descriptor_type2;
  uint16_t descriptor_length;
} USB_HID_DESCRIPTOR;

typedef  void (*USB_INTERRUPT_HANDLE_CB) (void);

typedef void (*USB_CALLBACK)(int32_t device,\
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

	HCED	descriptor;
	HCTD	td_head;
	volatile uint8_t current_state;
	uint8_t		flags;
	uint16_t		length;
	uint8_t*		data;
	USB_CALLBACK callback;
	void*  user_data;
}USB_ENDPOINT;

typedef struct {
	uint8_t bm_request_type;
	uint8_t b_request;
	uint16_t w_value;
	uint16_t w_index;
	uint16_t w_length;
} USB_SETUP;

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

	USB_SETUP setup_buffer;
}USB_DEVICE;

typedef struct {
    HCCA		communication_area;
    USB_ENDPOINT	endpoints[MAX_ENDPOINTS_TOTAL];
    USB_ENDPOINT	endpoint_zero;
    HCTD		common_tail;
    USB_SETUP	setup_zero;

    USB_DEVICE	devices[MAX_DEVICES];
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
    USB_INTERRUPT_HANDLE_CB port_cb;
}USB_CONTROLLER;


int controller_add_endpoint(int device,
                                int ep,
                                int attributes,
                                int max_packet_size,
                                int interval);

int controller_add_endpoint_by_desc(int device,
                                 USB_ENDPOINT_DESCRIPTOR* ed);

int controller_remove_endpoint(int device, int ep);

USB_ENDPOINT* controller_get_endpoint(int device, int ep);

void controller_set_interrupt_cb(void *cb);
//int controller_transfer(USB_ENDPOINT* endpoint, int token, u8* data, int len, int state);

int controller_release(USB_ENDPOINT* endpoint);

void controller_process_done_queue(USB_ENDPOINT* endpoint, int status);

int controller_add_device(int hub, int port, bool isLowSpeed);

int controller_init(void);

int controller_deinit(void);

USB_CONTROLLER* controller_get_op(void);

int32_t controller_get_frame_number(uint32_t *number);

int32_t controller_set_frame_number(uint32_t number);

int32_t controller_reset_port(int hub, int port);

int32_t controller_resume_port(int hub, int port);

int32_t controller_suspend_port(int hub, int port);

int32_t controller_disconnect(int hub, int port);

// called after reset
void controller_connect(int hub, int port, bool lowspeed);

void controller_reconnect_cleanup(int hub, int port);

// Called from interrupt
void controller_hub_status_change(int hub, int port, uint32_t status);

int controller_wait_io_done(int device, int ep);

int32_t controller_transfer(int device, int ep, u8 flags, u8* data, int length, USB_CALLBACK callback, void* user_data);

int32_t controller_control_transfer(int device, int request_type, int request, int value, int index, u8* data, int length, USB_CALLBACK callback, void * user_data);

int32_t  controller_interrupt_transfer(int device, int ep, u8* data, int length, USB_CALLBACK callback, void* user_data);

int32_t  controller_bulk_transfer(int device, int ep, u8* data, int length, USB_CALLBACK callback, void* user_data);

int32_t controller_transfer_abort(int device, int ep);

int32_t controller_getdescriptor(int device, int descType,int descIndex, u8* data, int length);

int32_t controller_get_string(int device, int index, char* dst, int length);

int32_t controller_set_address(int device, int new_addr);

int32_t controller_set_configuration(int device, int configNum);

int32_t controller_set_interface(int device, int ifNum, int altNum);

//	HUB stuff
int32_t controller_set_port_feature(int device, int feature, int index);

int32_t controller_clear_port_feature(int device, int feature, int index);

int32_t controller_set_port_power(int device, int port);

int32_t controller_set_port_reset(int device, int port);

int32_t controller_get_port_status(int device, int port, uint32_t* status);

int32_t controller_set_port_enable(int device, int port);

#define ERR_IO_PENDING             -100
#define ERR_ENDPOINT_NONE_LEFT     -101
#define ERR_ENDPOINT_NOT_FOUND     -102
#define ERR_DEVICE_NOT_FOUND       -103
#define ERR_DEVICE_NONE_LEFT       -104
#define ERR_HUB_INIT_FAILED        -105
#define ERR_INTERFACE_NOT_FOUND    -106

#endif
