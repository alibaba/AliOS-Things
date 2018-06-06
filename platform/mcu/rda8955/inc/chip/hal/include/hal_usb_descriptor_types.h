/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
* All rights reserved.
*
* This software is supplied "AS IS" without any warranties.
* RDA assumes no responsibility or liability for the use of the software,
* conveys no license or title under any patent, copyright, or mask work
* right to the product. RDA reserves the right to make changes in the
* software without notification.  RDA also make no representation or
* warranty that such application will be suitable for the specified use
* without further testing or modification.
*/


#ifndef _HAL_USB_DESCRIPTOR_TYPES_H_
#define _HAL_USB_DESCRIPTOR_TYPES_H_



// =============================================================================
//  MACROS
// =============================================================================

// ============================================================================
// HAL_USB_CONFIG_ATTRIB_T
// -----------------------------------------------------------------------------
/// All attributes of config descriptor
// =============================================================================
typedef enum
{
/// Device use is self power
    HAL_USB_CONFIG_ATTRIB_SELF_POWER            = 0x00000040
} HAL_USB_CONFIG_ATTRIB_T;


// ============================================================================
// HAL_USB_EP_TYPE_T
// -----------------------------------------------------------------------------
/// EndPoint type
// =============================================================================
typedef enum
{
    HAL_USB_EP_TYPE_CONTROL                     = 0x00000000,
    HAL_USB_EP_TYPE_ISO                         = 0x00000001,
    HAL_USB_EP_TYPE_BULK                        = 0x00000002,
    HAL_USB_EP_TYPE_INTERRUPT                   = 0x00000003
} HAL_USB_EP_TYPE_T;


// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// HAL_USB_EP_DESCRIPTOR_T
// -----------------------------------------------------------------------------
/// This struct use to define an EndPoint descriptor
// =============================================================================
typedef struct
{
    /// EndPoint index
    UINT8                          ep;                           //0x00000000
    /// Define the transfert type (ISO, Bulk, Cmd, Int)
    HAL_USB_EP_TYPE_T              type;                         //0x00000004
    /// Polling interval for interrupt
    UINT16                         interval;                     //0x00000008
    HAL_USB_CALLBACK_T             callback;                     //0x0000000A
} HAL_USB_EP_DESCRIPTOR_T; //Size : 0x10



// ============================================================================
// HAL_USB_EP_DESCRIPTOR_T*
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef HAL_USB_EP_DESCRIPTOR_T* HAL_USB_EP_LIST_DESCRIPTOR_T;


// ============================================================================
// HAL_USB_INTERFACE_DESCRIPTOR_T
// -----------------------------------------------------------------------------
/// This struct use to define an Interface descriptor
// =============================================================================
typedef struct
{
    /// USB class defined by usb.org
    UINT8                          usbClass;                     //0x00000000
    /// USB subclass defined by usb.org
    UINT8                          usbSubClass;                  //0x00000001
    /// USB protocol
    UINT8                          usbProto;                     //0x00000002
    /// List of endpoints linked with the interface
    HAL_USB_EP_LIST_DESCRIPTOR_T*  epList;                       //0x00000004
    /// Description of interface
    UINT8*                         description;                  //0x00000008
    HAL_USB_CALLBACK_T             callback;                     //0x0000000C
    /// Interface index. If 0 the interface index is generate
    UINT8                          interfaceIdx;                 //0x00000010
} HAL_USB_INTERFACE_DESCRIPTOR_T; //Size : 0x14



// ============================================================================
// HAL_USB_CS_INTERFACE_DESCRIPTOR_T
// -----------------------------------------------------------------------------
/// This struct use to define an class specific Interface descriptor
// =============================================================================
typedef struct
{
    UINT32                         reserved[2];                  //0x00000000
    HAL_USB_CS_CONSTRUCTOR_T       constructor;                  //0x00000008
    HAL_USB_CALLBACK_T             callback;                     //0x0000000C
    /// Interface index. Must be 0xFF for CS_INTERFACE
    UINT8                          interfaceIdx;                 //0x00000010
} HAL_USB_CS_INTERFACE_DESCRIPTOR_T; //Size : 0x14



// ============================================================================
// HAL_USB_INTERFACE_ASSOCIATION_DESCRIPTOR_REAL_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef struct
{
    UINT8                          bLength;                      //0x00000000
    UINT8                          bDescriptorType;              //0x00000001
    UINT8                          bFirstInterface;              //0x00000002
    UINT8                          bInterfaceCount;              //0x00000003
    UINT8                          bFunctionClass;               //0x00000004
    UINT8                          bFunctionSubClass;            //0x00000005
    UINT8                          bFunctionProtocol;            //0x00000006
    UINT8                          iFunction;                    //0x00000007
} HAL_USB_INTERFACE_ASSOCIATION_DESCRIPTOR_REAL_T; //Size : 0x8



// ============================================================================
// HAL_USB_INTERFACE_DESCRIPTOR_T*
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef HAL_USB_INTERFACE_DESCRIPTOR_T* HAL_USB_INTERFACE_LIST_DESCRIPTOR_T;


// ============================================================================
// HAL_USB_CONFIG_DESCRIPTOR_T
// -----------------------------------------------------------------------------
/// This struct use to define a Configuration descriptor
// =============================================================================
typedef struct
{
    /// Config attribues. Describe on #HAL_USB_CONFIG_ATTRIB_T
    UINT8                          attrib;                       //0x00000000
    /// Power use in this configuration 1 unit = 2mA
    UINT8                          maxPower;                     //0x00000001
    /// Interface association descriptor
    CONST HAL_USB_INTERFACE_ASSOCIATION_DESCRIPTOR_REAL_T* iad;  //0x00000004
    /// All interface including in this configuration
    HAL_USB_INTERFACE_LIST_DESCRIPTOR_T* interfaceList;          //0x00000008
    /// Description of configuration
    UINT8*                         description;                  //0x0000000C
    /// Config index
    UINT8                          configIdx;                    //0x00000010
} HAL_USB_CONFIG_DESCRIPTOR_T; //Size : 0x14



// ============================================================================
// HAL_USB_CONFIG_DESCRIPTOR_T*
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef HAL_USB_CONFIG_DESCRIPTOR_T* HAL_USB_CONFIG_LIST_DESCRIPTOR_T;


// ============================================================================
// HAL_USB_DEVICE_DESCRIPTOR_T
// -----------------------------------------------------------------------------
/// This struct use to define a Device descriptor
// =============================================================================
typedef struct
{
    /// Device class defined by usb.org
    UINT8                          usbClass;                     //0x00000000
    /// Device subclass defined by usb.org
    UINT8                          usbSubClass;                  //0x00000001
    /// Protocol
    UINT8                          usbProto;                     //0x00000002
    /// Vendor id defined by usb.org
    UINT16                         vendor;                       //0x00000004
    /// Product id
    UINT16                         product;                      //0x00000006
    /// Version (format BCD)
    UINT16                         version;                      //0x00000008
    /// List of all configuration
    HAL_USB_CONFIG_LIST_DESCRIPTOR_T* configList;                //0x0000000C
    /// Description of device
    UINT8*                         description;                  //0x00000010
    /// Serial Number
    UINT8*                         serialNumber;                 //0x00000014
    /// Usb mode use to generate product id
    UINT8                          usbMode;                      //0x00000018
} HAL_USB_DEVICE_DESCRIPTOR_T; //Size : 0x1C





#endif

