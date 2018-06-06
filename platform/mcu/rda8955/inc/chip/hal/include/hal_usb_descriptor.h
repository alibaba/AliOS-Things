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


#ifndef _HAL_USB_DESCRIPTOR_H_
#define _HAL_USB_DESCRIPTOR_H_

#include "cs_types.h"
#include "hal_usb.h"

// =============================================================================
// MACROS
// =============================================================================


// =============================================================================
// TYPES
// =============================================================================


// =============================================================================
// GLOBAL VARIABLES
// =============================================================================


// =============================================================================
// FUNCTIONS
// =============================================================================

// =============================================================================
// hal_UsbDescriptorNewCmdEpDescriptor
// -----------------------------------------------------------------------------
/// Create a command endpoint descriptor
/// @param ep Define the endpoint index for the direction
/// use #HAL_USB_EP_DIRECTION_IN and use #HAL_USB_EP_DIRECTION_OUT
/// @param callback Endpoint callback. See #HAL_USB_CALLBACK_EP_CMD_T.
/// @return Endpoint descriptor
// =============================================================================
PUBLIC HAL_USB_EP_DESCRIPTOR_T*
hal_UsbDescriptorNewCmdEpDescriptor(UINT8 ep, HAL_USB_CALLBACK_T callback);

// =============================================================================
// hal_UsbDescriptorNewBulkEpDescriptor
// -----------------------------------------------------------------------------
/// Create a bulk endpoint descriptor
/// @param ep Define the endpoint index for the direction
/// use #HAL_USB_EP_DIRECTION_IN and use #HAL_USB_EP_DIRECTION_OUT
/// @param callback Endpoint callback. See #HAL_USB_CALLBACK_EP_CMD_T.
/// @return Endpoint descriptor
// =============================================================================
PUBLIC HAL_USB_EP_DESCRIPTOR_T*
hal_UsbDescriptorNewBulkEpDescriptor(UINT8 ep, HAL_USB_CALLBACK_T callback);

// =============================================================================
// hal_UsbDescriptorNewInterruptEpDescriptor
// -----------------------------------------------------------------------------
/// Create a Interrupt endpoint descriptor
/// @param ep Define the endpoint index for the direction
/// use #HAL_USB_EP_DIRECTION_IN and use #HAL_USB_EP_DIRECTION_OUT
/// @param interval Polling intervale
/// @param callback Endpoint callback. See #HAL_USB_CALLBACK_EP_CMD_T.
/// @return Endpoint descriptor
// =============================================================================
PUBLIC HAL_USB_EP_DESCRIPTOR_T*
hal_UsbDescriptorNewInterruptEpDescriptor(UINT8 ep, UINT8 interval,
        HAL_USB_CALLBACK_T callback);

// =============================================================================
// hal_UsbDescriptorNewIsoEpDescriptor
// -----------------------------------------------------------------------------
/// Create a ISO endpoint descriptor
/// @param ep Define the endpoint index for the direction
/// use #HAL_USB_EP_DIRECTION_IN and use #HAL_USB_EP_DIRECTION_OUT
/// @param callback Endpoint callback. See #HAL_USB_CALLBACK_EP_CMD_T.
/// @return Endpoint descriptor
// =============================================================================
PUBLIC HAL_USB_EP_DESCRIPTOR_T*
hal_UsbDescriptorNewIsoEpDescriptor(UINT8 ep, HAL_USB_CALLBACK_T callback);

// =============================================================================
// hal_UsbDescriptorNewInterfaceDescriptor
// -----------------------------------------------------------------------------
/// Create a interface descriptor
/// @param usbClass Interface class defined by usb.org
/// @param usbSubClass Interface subclass defined by usb.org
/// @param usbProto Interface protocol
/// @param interfaceIdx Interface index. If 0 the interface index is generate
/// by HAL
/// @param epList List of endpoint linked of interface. The list must finish
/// with NULL pointer
/// @param description Interface description
/// @param callback interface callback. See #HAL_USB_CALLBACK_INTERFACE_CMD_T.
/// @return Interface descriptor
// =============================================================================
PUBLIC HAL_USB_INTERFACE_DESCRIPTOR_T*
hal_UsbDescriptorNewInterfaceDescriptor(UINT8                            usbClass,
                                        UINT8                            usbSubClass,
                                        UINT8                            usbProto,
                                        UINT8                            interfaceIdx,
                                        HAL_USB_EP_DESCRIPTOR_T**        epList,
                                        UINT8*                           description,
                                        HAL_USB_CALLBACK_T               callback);

PUBLIC HAL_USB_CS_INTERFACE_DESCRIPTOR_T*
hal_UsbDescriptorNewCSInterfaceDescriptor(HAL_USB_CS_CONSTRUCTOR_T constructor,
        HAL_USB_CALLBACK_T       callback);

// =============================================================================
// hal_UsbDescriptorNewConfigDescriptor
// -----------------------------------------------------------------------------
/// Create a configuration descriptor
/// @param maxPower Power use in configuration 1 unit = 2mA
/// @param attributes Describe on #HAL_USB_CONFIG_ATTRIB_T
/// @param interfaceList List of interface linked of interface. The list must
/// finish with NULL pointer see #hal_UsbDescriptorNewInterfaceDescriptor
/// @param description Configuration description
/// @return Configuration descriptor
// =============================================================================
PUBLIC HAL_USB_CONFIG_DESCRIPTOR_T*
hal_UsbDescriptorNewConfigDescriptor(UINT8 maxPower, UINT8 attributes,
                                     HAL_USB_INTERFACE_DESCRIPTOR_T**    interfaceList,
                                     UINT8*                              description);

// =============================================================================
// hal_UsbDescriptorNewDeviceDescriptor
// -----------------------------------------------------------------------------
/// Create a device descriptor
/// @param usbClass Device usb class. If equal 0 the class is describe in
/// interface
/// @param usbSubClass Device usb subclass
/// @param usbProto Device protocol
/// @param vendor VendorID
/// @param product ProductID
/// @param version Version of product (format BCD)
/// @param configList All config include in device
/// #see hal_UsbDescriptorNewConfigDescriptor
/// @param serialNumber Serial number
/// @param manufacturer Manufacturer name
/// @return Device descriptor
// =============================================================================
PUBLIC HAL_USB_DEVICE_DESCRIPTOR_T*
hal_UsbDescriptorNewDeviceDescriptor(UINT8  usbClass, UINT8  usbSubClass,
                                     UINT8  usbProto, UINT16 vendor,
                                     UINT16 product,  UINT16 version,
                                     HAL_USB_CONFIG_DESCRIPTOR_T* configList[],
                                     UINT8*                       serialnumber,
                                     UINT8*                       manufacturer);

// =============================================================================
// hal_UsbDescriptorCleanDeviceDescritor
// -----------------------------------------------------------------------------
/// Free all memory allocated for device and sub structure
/// @param dev Descriptor of the usb device
// =============================================================================
PUBLIC VOID
hal_UsbDescriptorCleanDeviceDescriptor(HAL_USB_DEVICE_DESCRIPTOR_T* dev);

// =============================================================================
// hal_UsbDescriptorCleanInterfaceListDescritor
// -----------------------------------------------------------------------------
/// Free all memory allocated for interface list and sub structure
/// @param dev Descriptor of the usb interface list
// =============================================================================
PUBLIC VOID
hal_UsbDescriptorCleanInterfaceListDescriptor(HAL_USB_INTERFACE_DESCRIPTOR_T** interfaces);

// =============================================================================
// hal_UsbDescriptorCopyInterfaceDescritor
// -----------------------------------------------------------------------------
/// Copy a interface descriptor
/// @param interface interface must be copy
/// @return copy of the interface
// =============================================================================
PUBLIC HAL_USB_INTERFACE_DESCRIPTOR_T*
hal_UsbDescriptorCopyInterfaceDescriptor(CONST HAL_USB_INTERFACE_DESCRIPTOR_T* interface);

#endif // _HAL_USB_DESCRIPTOR_H_
