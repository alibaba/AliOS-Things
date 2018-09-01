#include "usb_ch9.h"
#include "usb_defs.h"
#include "usb_gadget.h"

//     <i> Enable high-speed functionality (if device supports it)
#define USBD_HS_ENABLE              1


// define string index
#define STRING_MANUFACTURER		1
#define STRING_PRODUCT			2
#define STRING_SERIALNUMBER		3
#define STRING_INTERFACE		4
#define STRING_MSC				5


#define DEV_CONFIG_VALUE	1

#define DRIVER_DESC		"USB Mass Storage"
#define DRIVER_VERSION	"Feb 2016"

#define MANUFACTURER 	"Realtek Singapore Semiconductor"

static char	string_manufacturer [50] = MANUFACTURER;
static char	string_product [40] = DRIVER_DESC;
static char	string_serial [20] = "0123456789";

struct usb_string		
usbd_msc_strings [] = {
	{ STRING_MANUFACTURER,	string_manufacturer, },        
	{ STRING_PRODUCT,	string_product, },
	{ STRING_SERIALNUMBER,	string_serial, },
	{ STRING_INTERFACE,		"USB MSC Interface", },
	{ STRING_MSC,		"USB MSC", },
};

struct usb_gadget_strings	msc_stringtab = {
	.language	= 0x0409,	/* en-us */
	.strings	= usbd_msc_strings,
};

struct usb_gadget_strings *dev_msc_strings[] = {
	&msc_stringtab,
	NULL,
};

static struct usb_device_descriptor
usbd_msc_device_desc = {
	.bLength =		sizeof usbd_msc_device_desc,
	.bDescriptorType =	USB_DT_DEVICE,

	.bcdUSB =		    (0x0200),

	.bDeviceClass =		0x00,// define in interface descriptor
	.bDeviceSubClass =	0x00,
	.bDeviceProtocol =	0x00,

	.bMaxPacketSize0 = 	64,	// this will be set automatically depends on ep0 setting
	.idVendor =			0x0BDA,
	.idProduct =		0x8195,
//	.bcdDevice = 		,
	.iManufacturer =	STRING_MANUFACTURER,
	.iProduct =			STRING_PRODUCT,
	.iSerialNumber =	STRING_SERIALNUMBER,
	.bNumConfigurations=0x01,
};
#if 0
struct usb_config_descriptor
usbd_msc_config_desc = {
	.bLength =		sizeof usbd_msc_config_desc,
	.bDescriptorType =	USB_DT_CONFIG,

	/* compute wTotalLength on the fly */
	.bNumInterfaces =	1,
	.bConfigurationValue =	DEV_CONFIG_VALUE,
	.iConfiguration =	STRING_MSC,
	.bmAttributes =		USB_CONFIG_ATT_ONE | USB_CONFIG_ATT_SELFPOWER,
	.bMaxPower =		0x32,
};
#endif

#if USBD_HS_ENABLE
/* USB Device Qualifier Descriptor (for Full Speed) */
static struct usb_qualifier_descriptor
usbd_msc_qualifier_desc_FS = {
	.bLength = sizeof usbd_msc_qualifier_desc_FS,
	.bDescriptorType = USB_DT_DEVICE_QUALIFIER,
	.bcdUSB = 0x0200,
	.bDeviceClass = 0x00,
	.bDeviceSubClass = 0x00,
	.bDeviceProtocol = 0x00,
	.bMaxPacketSize0 = 64,
	.bNumConfigurations = 0x01,
	.bRESERVED = 0x00,
};

/* USB Device Qualifier Descriptor for High Speed */
static struct usb_qualifier_descriptor
usbd_msc_qualifier_desc_HS = {
	.bLength = sizeof usbd_msc_qualifier_desc_HS,
	.bDescriptorType = USB_DT_DEVICE_QUALIFIER,
	.bcdUSB = 0x0200,
	.bDeviceClass = 0x00,
	.bDeviceSubClass = 0x00,
	.bDeviceProtocol = 0x00,
	.bMaxPacketSize0 = 64,
	.bNumConfigurations = 0x01,
	.bRESERVED = 0x00,
};
#else
/* USB Device Qualifier Descriptor (for Full Speed) */
static struct usb_qualifier_descriptor
usbd_msc_qualifier_desc_FS    = { 0 };

/* USB Device Qualifier Descriptor for High Speed */
static struct usb_qualifier_descriptor
usbd_msc_qualifier_desc_HS = { 0 };
#endif

/* MSC Interface, Alternate Setting 0*/  
struct usb_interface_descriptor
usbd_msc_intf_desc = {
	.bLength =		sizeof usbd_msc_intf_desc,
	.bDescriptorType =	USB_DT_INTERFACE,

	.bInterfaceNumber =	0x00, // this will be assign automatically
	.bAlternateSetting =0x00,
	.bNumEndpoints =	0x02,
	.bInterfaceClass =	USB_CLASS_MASS_STORAGE,
	.bInterfaceSubClass = US_SC_SCSI,
	.bInterfaceProtocol = US_PR_BULK,
	.iInterface =		STRING_INTERFACE,
};

/* MSC Endpoints for Low-speed/Full-speed */
/* Endpoint, EP Bulk IN */ 
struct usb_endpoint_descriptor
usbd_msc_source_desc_FS = {
	.bLength =		USB_DT_ENDPOINT_SIZE,
	.bDescriptorType =	USB_DT_ENDPOINT,

	.bEndpointAddress =	USB_DIR_IN,
	.bmAttributes =		USB_ENDPOINT_XFER_BULK,
	.wMaxPacketSize =	(64), 
	.bInterval = 0x00,
	
};
/* Endpoint, EP Bulk OUT */ 
struct usb_endpoint_descriptor
usbd_msc_sink_desc_FS = {
	.bLength =		USB_DT_ENDPOINT_SIZE,
	.bDescriptorType =	USB_DT_ENDPOINT,

	.bEndpointAddress =	USB_DIR_OUT,
	.bmAttributes =		USB_ENDPOINT_XFER_BULK,
	.wMaxPacketSize =	(64), 
	.bInterval = 0x00,
};

/* MSC Endpoints for High-speed */
/* Endpoint, EP Bulk IN */ 
struct usb_endpoint_descriptor
usbd_msc_source_desc_HS = {
	.bLength =		USB_DT_ENDPOINT_SIZE,
	.bDescriptorType =	USB_DT_ENDPOINT,
	.bEndpointAddress = USB_DIR_IN,
	.bmAttributes =		USB_ENDPOINT_XFER_BULK,
	.wMaxPacketSize =	(512), 
	.bInterval = 0x00,
};

/* Endpoint, EP Bulk OUT */ 
struct usb_endpoint_descriptor
usbd_msc_sink_desc_HS = {
	.bLength =		USB_DT_ENDPOINT_SIZE,
	.bDescriptorType =	USB_DT_ENDPOINT,
	.bEndpointAddress = USB_DIR_OUT,
	.bmAttributes =		USB_ENDPOINT_XFER_BULK,
	.wMaxPacketSize =	(512),
	.bInterval = 0x00,
};

struct usb_descriptor_header *usbd_msc_descriptors_FS [] = {
	/* data interface has no altsetting */
	(struct usb_descriptor_header *) &usbd_msc_intf_desc,
	(struct usb_descriptor_header *) &usbd_msc_source_desc_FS,
	(struct usb_descriptor_header *) &usbd_msc_sink_desc_FS,
	NULL,
};
struct usb_descriptor_header *usbd_msc_descriptors_HS [] = {
	/* data interface has no altsetting */
	(struct usb_descriptor_header *) &usbd_msc_intf_desc,
	(struct usb_descriptor_header *) &usbd_msc_source_desc_HS,
	(struct usb_descriptor_header *) &usbd_msc_sink_desc_HS,
	NULL,
};