/***************************************************************************//**
 * @file
 * @brief USB descriptors for HID keyboard example project.
 * @author Energy Micro AS
 * @version 1.03
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2013 Energy Micro AS, http://www.energymicro.com</b>
 *******************************************************************************
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 * 4. The source and compiled code may only be used on Energy Micro "EFM32"
 *    microcontrollers and "EFR4" radios.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Energy Micro AS has no
 * obligation to support this Software. Energy Micro AS is providing the
 * Software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Energy Micro AS will not be liable for any consequential, incidental, or
 * special damages, or any other relief, or for any claim by any third party,
 * arising from your use of this Software.
 *
 *****************************************************************************/
  /**************************************************************************//**
 * Modified by Bosch Connected Devices and Solutions GmbH. Modifications are
 *		-	Bosch specific vendor ID and Product ID
 *		-	Project specific string descriptor
 *****************************************************************************/
#ifndef BCDS_DESCRIPTORS_H_
#define BCDS_DESCRIPTORS_H_

#include "em_usb.h"
#include "BCDS_USB.h"

#define USB_EP_DATA_OUT            INT32_C(0x01)        /**< Endpoint for USB data reception.       */
#define USB_EP_DATA_IN             INT32_C(0x81)        /**< Endpoint for USB data transmission.    */
#define USB_EP_NOTIFY              INT32_C(0x82)        /**< The notification endpoint (not used).  */
/** Macro to represent the delay limit(10ms) when USB transfer is not
 * done (since it takes 10ms(appx) to transmit 1kb delay limit is fixed as 10ms)    */
#define USB_DELAY_LIMIT            UINT8_C(10)
#define USB_ONE_MILLI_SECOND_DELAY UINT8_C(1)           /**< Macro to represent the one milli second delay*/
#define USB_ZERO_VALUE             UINT8_C(0)           /**< Macro to represent zero value          */

/* USB uses the below commands to save in a USB Setup request package(USB_Setup_TypeDef) to implement CDC class  */
#define USB_SETUP_VALUE             UINT8_C(0)          /* @see USB_Setup_TypeDef  */
#define USB_SETUP_INDEX             UINT8_C(0)          /* @see USB_Setup_TypeDef  */
#define USB_SETUP_NO_DATA_LENGTH    UINT8_C(0)          /* @see USB_Setup_TypeDef  */
#define USB_SETUP_LENGTH            UINT8_C(7)          /* @see USB_Setup_TypeDef  */

STATIC_CONST_STRING_DESC( USB_manufacturerName_gau, 'B','o','s','c','h',' ','C','o','n','n','e','c','t','e','d', ' ','D','e','v','i','c','e','s',' ','a','n','d',' ','S','o','l','u','t','i','o','n','s',' ','G','m','b','H'); /**< Manufacturer name */
STATIC_CONST_STRING_DESC( USB_productName_gau, BCDS_PRODUCT_NAME); /**< Product name */

/**
 Macro for creating USB compliant language string descriptors */
STATIC_CONST_STRING_DESC_LANGID(USB_langID_gau, 0x04, 0x09);

EFM32_ALIGN(4)

static const USB_DeviceDescriptor_TypeDef USB_deviceDescriptor_mds __attribute__ ((aligned(4)))=
{
    .bLength = USB_DEVICE_DESCSIZE,
    .bDescriptorType = USB_DEVICE_DESCRIPTOR,
    .bcdUSB = 0x0200,
    .bDeviceClass = USB_CLASS_CDC,
    .bDeviceSubClass = 0,
    .bDeviceProtocol = 0,
    .bMaxPacketSize0 = USB_EP0_SIZE,
    .idVendor = 0x108C,
    .idProduct = USB_PRODUCT_ID,
    .bcdDevice = 0x0000,
    .iManufacturer = 1,
    .iProduct = 2,
    .iSerialNumber = 3,
    .bNumConfigurations = 1
};

#define USB_CONFIG_TOTAL_DESCSIZE ( USB_CONFIG_DESCSIZE                   + \
                          (USB_INTERFACE_DESCSIZE * 2)          + \
                          (USB_ENDPOINT_DESCSIZE * NUM_EP_USED) + \
                          USB_CDC_HEADER_FND_DESCSIZE           + \
                          USB_CDC_CALLMNG_FND_DESCSIZE          + \
                          USB_CDC_ACM_FND_DESCSIZE              + \
                          5 )

EFM32_ALIGN(4)
static const uint8_t USB_configDescriptor_ma[] __attribute__ ((aligned(4)))=
{
    /*** Configuration descriptor ***/
    USB_CONFIG_DESCSIZE, /* bLength                                   */
    USB_CONFIG_DESCRIPTOR, /* bDescriptorType                           */
    USB_CONFIG_TOTAL_DESCSIZE, /* wTotalLength (LSB)                        */
    USB_CONFIG_DESCSIZE>>8, /* wTotalLength (MSB)                        */
    2, /* bNumInterfaces                            */
    1, /* bConfigurationValue                       */
    0, /* iConfiguration                            */
    CONFIG_DESC_BM_RESERVED_D7 | /* bmAttrib: Self powered             */
    CONFIG_DESC_BM_SELFPOWERED,
    CONFIG_DESC_MAXPOWER_mA( 500 ),/* bMaxPower: 500 mA                  */

    /*** Communication Class Interface descriptor (interface no. 0)    ***/
    USB_INTERFACE_DESCSIZE, /* bLength               */
    USB_INTERFACE_DESCRIPTOR,/* bDescriptorType      */
    0, /* bInterfaceNumber      */
    0, /* bAlternateSetting     */
    1, /* bNumEndpoints         */
    USB_CLASS_CDC, /* bInterfaceClass       */
    USB_CLASS_CDC_ACM, /* bInterfaceSubClass    */
    0, /* bInterfaceProtocol    */
    0, /* iInterface            */

    /*** CDC Header Functional descriptor ***/
    USB_CDC_HEADER_FND_DESCSIZE, /* bFunctionLength  */
    USB_CS_INTERFACE_DESCRIPTOR, /* bDescriptorType  */
    USB_CLASS_CDC_HFN, /* bDescriptorSubtype    */
    0x20, /* bcdCDC spec.no LSB    */
    0x01, /* bcdCDC spec.no MSB    */

    /*** CDC Call Management Functional descriptor ***/
    USB_CDC_CALLMNG_FND_DESCSIZE, /* bFunctionLength */
    USB_CS_INTERFACE_DESCRIPTOR, /* bDescriptorType */
    USB_CLASS_CDC_CMNGFN, /* bDescriptorSubtype    */
    0, /* bmCapabilities        */
    1, /* bDataInterface        */

    /*** CDC Abstract Control Management Functional descriptor ***/
    USB_CDC_ACM_FND_DESCSIZE, /* bFunctionLength     */
    USB_CS_INTERFACE_DESCRIPTOR, /* bDescriptorType  */
    USB_CLASS_CDC_ACMFN, /* bDescriptorSubtype    */
    0x02, /* bmCapabilities        */
    /* The capabilities that this configuration supports:                   */
    /* D7..D4: RESERVED (Reset to zero)                                     */
    /* D3: 1 - Device supports the notification Network_Connection.         */
    /* D2: 1 - Device supports the request Send_Break                       */
    /* D1: 1 - Device supports the request combination of Set_Line_Coding,  */
    /*         Set_Control_Line_State, Get_Line_Coding, and the             */
    /*         notification Serial_State.                                   */
    /* D0: 1 - Device supports the request combination of Set_Comm_Feature, */
    /*         Clear_Comm_Feature, and Get_Comm_Feature.                    */

    /*** CDC Union Functional descriptor ***/
    5, /* bFunctionLength       */
    USB_CS_INTERFACE_DESCRIPTOR, /* bDescriptorType  */
    USB_CLASS_CDC_UNIONFN, /* bDescriptorSubtype    */
    0, /* bControlInterface,      itf. no. 0 */
    1, /* bSubordinateInterface0, itf. no. 1 */

    /*** CDC Notification endpoint descriptor ***/
    USB_ENDPOINT_DESCSIZE, /* bLength               */
    USB_ENDPOINT_DESCRIPTOR,/* bDescriptorType       */
    USB_EP_NOTIFY, /* bEndpointAddress (IN) */
    USB_EPTYPE_INTR, /* bmAttributes          */
    USB_BULK_SIZE, /* wMaxPacketSize (LSB)  */
    0, /* wMaxPacketSize (MSB)  */
    0xFF, /* bInterval             */

    /*** Data Class Interface descriptor (interface no. 1)                ***/
    USB_INTERFACE_DESCSIZE, /* bLength               */
    USB_INTERFACE_DESCRIPTOR,/* bDescriptorType      */
    1, /* bInterfaceNumber      */
    0, /* bAlternateSetting     */
    2, /* bNumEndpoints         */
    USB_CLASS_CDC_DATA, /* bInterfaceClass       */
    0, /* bInterfaceSubClass    */
    0, /* bInterfaceProtocol    */
    0, /* iInterface            */

    /*** CDC Data interface endpoint descriptors ***/
    USB_ENDPOINT_DESCSIZE, /* bLength               */
    USB_ENDPOINT_DESCRIPTOR,/* bDescriptorType       */
    USB_EP_DATA_IN, /* bEndpointAddress (IN) */
    USB_EPTYPE_BULK, /* bmAttributes          */
    USB_BULK_SIZE, /* wMaxPacketSize (LSB)  */
    0, /* wMaxPacketSize (MSB)  */
    0, /* bInterval             */

    USB_ENDPOINT_DESCSIZE, /* bLength               */
    USB_ENDPOINT_DESCRIPTOR,/* bDescriptorType       */
    USB_EP_DATA_OUT, /* bEndpointAddress (OUT)*/
    USB_EPTYPE_BULK, /* bmAttributes          */
    USB_BULK_SIZE, /* wMaxPacketSize (LSB)  */
    0, /* wMaxPacketSize (MSB)  */
    0 /* bInterval             */
};

STATIC_CONST_STRING_DESC_LANGID( langID, 0x04, 0x09 );

#define USBDESC_STR_LEN 20 /**< Maximum length of USB Serial Number */

EFM32_PACK_START( 1 )
typedef struct
{
  uint8_t len;                       /**< Size of this descriptor in bytes.                 */
  uint8_t type;                      /**< Constant STRING Descriptor Type.                  */
  char16_t name[ USBDESC_STR_LEN ];  /**< The string encoded with UTF-16LE UNICODE charset. */
} __attribute__ ((packed)) stringDesc_t;
EFM32_PACK_END()

EFM32_ALIGN( 4 )
EFM32_PACK_START( 1 )
static stringDesc_t USB_serialnumber_gau __attribute__ ((aligned(4)));
EFM32_PACK_END()

static const void * const USB_strings_ma[] =
    {
        &langID,
        &USB_manufacturerName_gau,
        &USB_productName_gau,
		&USB_serialnumber_gau
    };

/* Endpoint buffer sizes */
/* 1 = single buffer, 2 = double buffering, 3 = triple buffering ...  */
/* Use double buffering on the BULK endpoints.                        */
static uint8_t USB_bufferingMultiplier_ma[NUM_EP_USED + 1] = { 1, 1, 2, 2 };
static const USBD_Callbacks_TypeDef USB_callbacks_mds =
    {
        .usbReset = NULL,
        .usbStateChange = USB_stateChange,
        .setupCmd = USB_setupCommand,
        .isSelfPowered = NULL,
        .sofInt = NULL
    };

static const USBD_Init_TypeDef USB_initStruct_mds =
    {
        .deviceDescriptor = &USB_deviceDescriptor_mds,
        .configDescriptor = USB_configDescriptor_ma,
        .stringDescriptors = USB_strings_ma,
        .numberOfStrings = sizeof(USB_strings_ma) / sizeof(void*),
        .bufferingMultiplier = USB_bufferingMultiplier_ma,
        .callbacks = &USB_callbacks_mds,
        .reserved = UINT32_C(0)
    };
#endif  /* BCDS_DESCRIPTORS_H_ */
