/******************************************************************
*                                                                *
*        Copyright Mentor Graphics Corporation 2004              *
*                                                                *
*                All Rights Reserved.                            *
*                                                                *
*    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION *
*  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS   *
*  LICENSORS AND IS SUBJECT TO LICENSE TERMS.                    *
*                                                                *
******************************************************************/

/*
 * Device Framework (USB 2.0 chapter 9) definitions.
 * $Revision: 1.4 $
 */

#ifndef __MUSB_DEVICE_H__
#define __MUSB_DEVICE_H__

/* USB 2.0 device framework definitions */

#include "mu_tools.h"
#include "compiler.h"

/*
* Device and/or Interface Class codes
*/
#define MUSB_CLASS_PER_INTERFACE 0	/* for DeviceClass */
#define MUSB_CLASS_AUDIO         1
#define MUSB_CLASS_COMM          2
#define MUSB_CLASS_HID           3
#define MUSB_CLASS_PHYSICAL      5
#define MUSB_CLASS_IMAGE         6
#define MUSB_CLASS_PRINTER       7
#define MUSB_CLASS_MASS_STORAGE  8
#define MUSB_CLASS_HUB           9
#define MUSB_CLASS_DATA          10
#define MUSB_CLASS_VIDEO         0x0E
#define MUSB_CLASS_DIAGNOSTIC    0xDC
#define MUSB_CLASS_MISC          0xEF
#define MUSB_CLASS_APP_SPEC      0xfe
#define MUSB_CLASS_VENDOR_SPEC   0xff

/*
* USB request types
*/
#define MUSB_TYPE_MASK			(0x03 << 5)
#define MUSB_TYPE_STANDARD		(0x00 << 5)
#define MUSB_TYPE_CLASS			(0x01 << 5)
#define MUSB_TYPE_VENDOR		(0x02 << 5)
#define MUSB_TYPE_RESERVED		(0x03 << 5)

/*
* USB recipients
*/
#define MUSB_RECIP_MASK			0x1f
#define MUSB_RECIP_DEVICE		0x00
#define MUSB_RECIP_INTERFACE		0x01
#define MUSB_RECIP_ENDPOINT		0x02
#define MUSB_RECIP_OTHER		0x03

/*
* USB directions
*/
#define MUSB_DIR_OUT			0
#define MUSB_DIR_IN			0x80

/*
* Descriptor types
*/
#define MUSB_DT_DEVICE			0x01
#define MUSB_DT_CONFIG			0x02
#define MUSB_DT_STRING			0x03
#define MUSB_DT_INTERFACE		0x04
#define MUSB_DT_ENDPOINT		0x05
#define	MUSB_DT_DEVICE_QUALIFIER	0x06
#define	MUSB_DT_OTHER_SPEED		    0x07
#define	MUSB_DT_INTERFACE_POWER		0x08
#define	MUSB_DT_OTG			    	0x09
#define	MUSB_DT_BOS			    	0x0F
#define	MUSB_DT_HID_REPORT			0x22

/*
* Descriptor sizes per descriptor type
*/
#define MUSB_DT_DEVICE_SIZE		18
#define MUSB_DT_DEVICE_QUALIFIER_SIZE	10
#define MUSB_DT_CONFIG_SIZE		9
#define MUSB_DT_INTERFACE_SIZE		9
#define MUSB_DT_ENDPOINT_SIZE		7
#define MUSB_DT_HUB_NONVAR_SIZE		7
#define	MUSB_DT_OTG_SIZE		3

/*
* Endpoints
*/
#define MUSB_ENDPOINT_NUMBER_MASK	0x0f	/* in bEndpointAddress */
#define MUSB_ENDPOINT_DIR_MASK		0x80

/*
 * Alternatset
 */
#define INTERFACE_ALTERSET			0x01/*0x00 Is BULK Interface*/ /*0x01 Is ISOC Interface*/

/*
 *ISOC pack head
 */
#define ISOC_PACK_HEAD_LEN			12

/* Traffic types in bmAttributes */
#define MUSB_ENDPOINT_XFERTYPE_MASK	0x03
#define MUSB_ENDPOINT_XFER_CONTROL	0
#define MUSB_ENDPOINT_XFER_ISOC		1
#define MUSB_ENDPOINT_XFER_BULK		2
#define MUSB_ENDPOINT_XFER_INT		3

/* Synchronization type in bmAttributes (ISO only) */
#define MUSB_M_ENDPOINT_SYNC_TYPE	0x0c
#define MUSB_S_ENDPOINT_SYNC_TYPE	2
#define MUSB_ENDPOINT_TYPE_NONE		0
#define MUSB_ENDPOINT_TYPE_ASYNC	1
#define MUSB_ENDPOINT_TYPE_ADAPTIVE	2
#define MUSB_ENDPOINT_TYPE_SYNC		3

/* Usage type in bmAttributes (ISO only) */
#define MUSB_M_ENDPOINT_USAGE_TYPE	0x30
#define MUSB_S_ENDPOINT_USAGE_TYPE	4
#define MUSB_ENDPOINT_USAGE_DATA	0
#define MUSB_ENDPOINT_USAGE_FEEDBACK	1
#define MUSB_ENDPOINT_USAGE_IMPLICIT	2
#define MUSB_ENDPOINT_USAGE_RESERVED	3

/* wMaxPacketSize */
#define MUSB_M_ENDPOINT_MAX_PACKET_SIZE	    0x07ff
#define MUSB_M_ENDPOINT_PACKETS_PER_FRAME   0x1800  /* high-speed, high-bandwidth */
#define MUSB_S_ENDPOINT_PACKETS_PER_FRAME   11	    /* high-speed, high-bandwidth */

/*
* Standard requests
*/
#define MUSB_REQ_GET_STATUS		0x00
#define MUSB_REQ_CLEAR_FEATURE		0x01
#define MUSB_REQ_SET_FEATURE		0x03
#define MUSB_REQ_SET_ADDRESS		0x05
#define MUSB_REQ_GET_DESCRIPTOR		0x06
#define MUSB_REQ_SET_DESCRIPTOR		0x07
#define MUSB_REQ_GET_CONFIGURATION	0x08
#define MUSB_REQ_SET_CONFIGURATION	0x09
#define MUSB_REQ_GET_INTERFACE		0x0A
#define MUSB_REQ_SET_INTERFACE		0x0B
#define MUSB_REQ_SYNCH_FRAME		0x0C

/*
* Class requests
*/
#define MUSB_CLASS_GET_IDLE         0x02   // Code for Get Idle
#define MUSB_CLASS_GET_PROTOCOL     0x03   // Code for Get Protocol
#define MUSB_CLASS_SET_REPORT       0x09   // Code for Set Report
#define MUSB_CLASS_SET_IDLE         0x0A   // Code for Set Idle
#define MUSB_CLASS_SET_PROTOCOL     0x0B   // Code for Set Protocol

#define MUSB_CLASS_SET_CURRENT      0x01
#define MUSB_CLASS_GET_CURRENT      0x81
#define MUSB_CLASS_GET_MINIUM       0x82
#define MUSB_CLASS_GET_MAXIUM       0x83
#define MUSB_CLASS_GET_RES          0x84
#define MUSB_CLASS_GET_LEN          0x85
#define MUSB_CLASS_GET_INFO         0x86
#define MUSB_CLASS_GET_DEF          0x87

#define MUSB_CLASS_MUTE_CTRL        0x01
#define MUSB_CLASS_VOLUME_CTRL      0x02
#define MUSB_CLASS_BASS_BOOST_CTRL  0x09

/*
* OTG requests
*/
#define	MUSB_OTG_SRP			0x01	/* bit 0 of bmAttributes */
#define	MUSB_OTG_HNP			0x02	/* bit 1 of bmAttributes */

/*
* USB Set Features
*/
#define	MUSB_FEATURE_ENDPOINT_HALT		0x00
#define	MUSB_FEATURE_DEVICE_REMOTE_WAKEUP	0x01
#define	MUSB_FEATURE_TEST_MODE			0x02

/*
* OTG Set Feature parameters
*/
#define	MUSB_b_hnp_enable		0x03
#define	MUSB_a_hnp_support		0x04
#define	MUSB_a_alt_hnp_support		0x05

/*
* Test modes
*/
#define MUSB_TESTMODE_TEST_J		0x01
#define MUSB_TESTMODE_TEST_K		0x02
#define MUSB_TESTMODE_TEST_SE0_NAK	0x03
#define MUSB_TESTMODE_TEST_PACKET	0x04
#define MUSB_TESTMODE_TEST_FORCE_ENABLE	0x05

/* USB structures */

/* Standard device request (USB 2.0) */
#include "mu_pkon.h"
typedef struct
{
    uint8_t bmRequestType;
    uint8_t bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} __attribute__ ((packed))   MUSB_DeviceRequest;
#include "mu_pkoff.h"

/* All standard descriptors have these 2 fields in common */
#include "mu_pkon.h"
typedef	struct
{
    uint8_t bLength;
    uint8_t bDescriptorType;
} MUSB_DescriptorHeader;
#include "mu_pkoff.h"

/* Device descriptor (USB 2.0) */
#include "mu_pkon.h"
typedef struct
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t bcdUSB;
    uint8_t bDeviceClass;
    uint8_t bDeviceSubClass;
    uint8_t bDeviceProtocol;
    uint8_t bMaxPacketSize0;
    uint16_t idVendor;
    uint16_t idProduct;
    uint16_t bcdDevice;
    uint8_t iManufacturer;
    uint8_t iProduct;
    uint8_t iSerialNumber;
    uint8_t bNumConfigurations;
} __attribute__ ((packed))  MUSB_DeviceDescriptor;
#include "mu_pkoff.h"

/* Endpoint descriptor (USB 2.0) */
#include "mu_pkon.h"
typedef	struct
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bEndpointAddress;
    uint8_t bmAttributes;
    uint16_t wMaxPacketSize;
    uint8_t bInterval;
} __attribute__ ((packed))   MUSB_EndpointDescriptor;
#include "mu_pkoff.h"

/* Interface descriptor (USB 2.0) */
#include "mu_pkon.h"
typedef	struct
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bInterfaceNumber;
    uint8_t bAlternateSetting;
    uint8_t bNumEndpoints;
    uint8_t bInterfaceClass;
    uint8_t bInterfaceSubClass;
    uint8_t bInterfaceProtocol;
    uint8_t iInterface;
} __attribute__ ((packed))  MUSB_InterfaceDescriptor;
#include "mu_pkoff.h"

/* Configuration descriptor (USB 2.0) */
#include "mu_pkon.h"
typedef	struct
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t wTotalLength;
    uint8_t bNumInterfaces;
    uint8_t bConfigurationValue;
    uint8_t iConfiguration;
    uint8_t bmAttributes;
    uint8_t bMaxPower;
} __attribute__ ((packed)) MUSB_ConfigurationDescriptor;
#include "mu_pkoff.h"

/* String descriptor (USB 2.0) */
#include "mu_pkon.h"
typedef	struct
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t wData[1];
} __attribute__ ((packed))   MUSB_StringDescriptor;
#include "mu_pkoff.h"

/* OTG descriptor (OTG 1.0a) */
#include "mu_pkon.h"
typedef	struct
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bmAttributes;	/* bit 0=SRP; bit 1=HNP */
} __attribute__ ((packed))   MUSB_OtgDescriptor;
#include "mu_pkoff.h"

/* Qualifier descriptor */
#include "mu_pkon.h"
typedef struct
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t bcdUSB;
    uint8_t bDeviceClass;
    uint8_t bDeviceSubClass;
    uint8_t bDeviceProtocol;
    uint8_t bMaxPacketSize0;
    uint8_t bNumConfigurations;
    uint8_t bRESERVED;
} __attribute__ ((packed))   MUSB_QualifierDescriptor;
#include "mu_pkoff.h"


#endif	/* multiple inclusion protection */
