/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2004-2017 ARM Germany GmbH. All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    USBD_Config_CustomClass_0.h
 * Purpose: USB Device Custom Class Configuration
 * Rev.:    V5.2.0
 *----------------------------------------------------------------------------*/

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

// <h>USB Device: Custom Class 0
// <i>Custom Class can be used to make support for Standard or Vendor-Specific Class
//   <o>Assign Device Class to USB Device # <0-3>
//   <i>Select USB Device that is used for this Device Class instance
#define USBD_CUSTOM_CLASS0_DEV                                     0

//   <e0.0>Interface Association
//   <i>Used for grouping of multiple interfaces to a single class.
#define USBD_CUSTOM_CLASS0_IAD_EN                                  0

//       <o.0..7>Class Code
//         <i>Class Codes are defined by USB-IF. For more information refer to
//         <i>http://www.usb.org/developers/defined_class.
//         <0x00=>0x00: Indicate a Null Class Code triple
//         <0x01=>0x01: Audio
//         <0x02=>0x02: Communications and CDC Control
//         <0x03=>0x03: HID (Human Interface Device)
//         <0x05=>0x05: Physical
//         <0x06=>0x06: Image
//         <0x07=>0x07: Printer
//         <0x08=>0x08: Mass Storage
//         <0x0A=>0x0A: CDC-Data
//         <0x0B=>0x0B: Smart Card
//         <0x0D=>0x0D: Content Security
//         <0x0E=>0x0E: Video
//         <0x0F=>0x0F: Personal Healthcare
//         <0x10=>0x10: Audio/Video Devices
//         <0xDC=>0xDC: Diagnostic Device
//         <0xE0=>0xE0: Wireless Controller
//         <0xEF=>0xEF: Miscellaneous
//         <0xFE=>0xFE: Application Specific
//         <0xFF=>0xFF: Vendor Specific
#define USBD_CUSTOM_CLASS0_IAD_CLASS                               0xFF

//       <o.0..7>Subclass Code <0x00-0xFF>
//       <i>The possible values depend on the Class Code:
//       <i>Class Code 0x00: Subclass Code must be 0
//       <i>Class Code 0x01 .. 0xFE: Subclass Code is defined by USB-IF
//       <i>Class Code 0xFF: Subclass Code can be 0x00 .. 0xFF
#define USBD_CUSTOM_CLASS0_IAD_SUBCLASS                            0x00

//       <o.0..7>Protocol Code <0x00-0xFF>
//       <i>The Protocol Code value defines the protocol used on this interface:
//       <i>Protocol Code 0x00: class-specific protocol not used
//       <i>Protocol Code 0x01 .. 0xFE: class-specific protocol used
//       <i>Protocol Code 0xFF: vendor-specific protocol used
#define USBD_CUSTOM_CLASS0_IAD_PROTOCOL                            0x00

//   </e>


//   <e>Interface
#define USBD_CUSTOM_CLASS0_IF0_EN                                  1

//     <h>Interface Settings
//     <i>The Interface Settings are used to create the Interface Descriptor.
//     <i>Refer to USB - USB Concepts - USB Descriptor in the MDK Components
//     <i>User's Guide for more information about the Interface Descriptor.

//       <o>Interface Number <0-255>
//       <i>Defines the value for bInterfaceNumber
//       <i>Each USB Device Interface has a sequential Interface Number starting with 0.
//       <i>Several Interfaces may have the same Interface Number; in this case the value
//       <i>of Alternate Setting is used to differ between the Interfaces. For a
//       <i>composite device the Interface Numbers of the custom classes must be contiguous.
#define USBD_CUSTOM_CLASS0_IF0_NUM                                 0

//       <o>Alternate Setting <0=>0 <1=>1 <2=>2 <3=>3
//       <i>Defines the value for bAlternateSetting
//       <i>A sequential number starting with 0 to identify the Interface Descriptors
//       <i>that share the same value for Interface Number.
#define USBD_CUSTOM_CLASS0_IF0_ALT                                 0

//       <o.0..7>Class Code
//         <i>Class Codes are defined by USB-IF. For more information refer to
//         <i>http://www.usb.org/developers/defined_class.
//         <0x00=>0x00: Indicate a Null Class Code triple
//         <0x01=>0x01: Audio
//         <0x02=>0x02: Communications and CDC Control
//         <0x03=>0x03: HID (Human Interface Device)
//         <0x05=>0x05: Physical
//         <0x06=>0x06: Image
//         <0x07=>0x07: Printer
//         <0x08=>0x08: Mass Storage
//         <0x0A=>0x0A: CDC-Data
//         <0x0B=>0x0B: Smart Card
//         <0x0D=>0x0D: Content Security
//         <0x0E=>0x0E: Video
//         <0x0F=>0x0F: Personal Healthcare
//         <0x10=>0x10: Audio/Video Devices
//         <0xDC=>0xDC: Diagnostic Device
//         <0xE0=>0xE0: Wireless Controller
//         <0xEF=>0xEF: Miscellaneous
//         <0xFE=>0xFE: Application Specific
//         <0xFF=>0xFF: Vendor Specific
#define USBD_CUSTOM_CLASS0_IF0_CLASS                               0xFF

//       <o.0..7>Subclass Code <0x00-0xFF>
//       <i>The possible values depend on the Class Code:
//       <i>Class Code 0x00: Subclass Code must be 0
//       <i>Class Code 0x01 .. 0xFE: Subclass Code is defined by USB-IF
//       <i>Class Code 0xFF: Subclass Code can be 0x00 .. 0xFF
#define USBD_CUSTOM_CLASS0_IF0_SUBCLASS                            0x00

//       <o.0..7>Protocol Code <0x00-0xFF>
//       <i>The Protocol Code value defines the protocol used on this interface:
//       <i>Protocol Code 0x00: class-specific protocol not used
//       <i>Protocol Code 0x01 .. 0xFE: class-specific protocol used
//       <i>Protocol Code 0xFF: vendor-specific protocol used
#define USBD_CUSTOM_CLASS0_IF0_PROTOCOL                            0x00

//     </h>

//     <h>Endpoint Settings
//     <i>Following settings are used to create the Endpoint Descriptors.
//     <i>Refer to USB - USB Concepts - USB Descriptor in the MDK Components
//     <i>User's Guide for more information about Endpoint Descriptors.

//       <e>Endpoint
//       <i>Enable Endpoint for this interface.
#define USBD_CUSTOM_CLASS0_IF0_EP0_EN                              1

//         <o.0..1>Type
//           <i>Select Endpoint Type.
//           <i>Endpoint Descriptor: bmAttributes field bits 0 .. 1.
//           <i>If required, for Isochronous Endpoint, Synchronization and Usage Type 
//           <i>can be set by manually editing define value of BMATTRIBUTES.
//           <2=>Bulk
//           <3=>Interrupt
//           <1=>Isochronous
#define USBD_CUSTOM_CLASS0_IF0_EP0_BMATTRIBUTES                    0x02

//         <o.0..3>Number
//           <i>Select Endpoint Number.
//           <i>Endpoint Descriptor: bEndpointAddress field bits 0 .. 3.
//                   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
//           <8=>8   <9=>9   <10=>10 <11=>11 <12=>12 <13=>13 <14=>14 <15=>15
//         <o.7>Direction
//           <i>Select Endpoint Direction.
//           <i>Endpoint Descriptor: bEndpointAddress field bit 7.
//           <0=>OUT
//           <1=>IN
#define USBD_CUSTOM_CLASS0_IF0_EP0_BENDPOINTADDRESS                0x01

//         <h>Speed Settings
//           <i>Settings that are different depending on device operating speed.
//
//           <h>Full/Low-speed
//             <i>Parameters apply when device operates in Full/Low-speed.
//
//             <o.0..9>Maximum Packet Size <0-1023>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 8, 16, 32 or 64.
//               <i>For Interrupt Endpoint set value to 1 .. 64.
//               <i>For Isochronous Endpoint set value to 1 .. 1023.
#define USBD_CUSTOM_CLASS0_IF0_EP0_FS_WMAXPACKETSIZE               64

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in ms).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>Setting is not used for Bulk Endpoint (set value to 0).
//               <i>For Interrupt Endpoint set value to 1 .. 255 (polling interval).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF0_EP0_FS_BINTERVAL                    0

//           </h>

//           <h>High-speed
//             <i>Parameters apply when High-speed is enabled in corresponding USBD_Config_n.c
//             <i>(n is the index of device on which this interface will be used) and when
//             <i>device operates in High-speed.
//
//             <o.0..10>Maximum Packet Size <0-1024>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 512.
//               <i>For Interrupt Endpoint set value to 1 .. 1024.
//               <i>For Isochronous Endpoint set value to 1 .. 1024.
//             <o.11..12>Additional Transactions per Microframe
//               <i>Specifies additional transactions per microframe to improve communication performance.
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 11 .. 12.
//               <i>Relevant only if Endpoint Type is Isochronous or Interrupt.
//               <i>Value: None = 1 transaction per microframe
//               <i>Value: 1 additional = 2 transaction per microframe
//               <i>Value: 2 additional = 3 transaction per microframe
//                 <0=>None
//                 <1=>1 additional
//                 <2=>2 additional
#define USBD_CUSTOM_CLASS0_IF0_EP0_HS_WMAXPACKETSIZE               512

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in 125 us units).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>For Bulk Endpoint this setting represents maximum NAK rate, set value to 0 .. 255.
//               <i>For Interrupt Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF0_EP0_HS_BINTERVAL                    0

//           </h>
//         </h>
//       </e>

//       <e>Endpoint
//       <i>Enable Endpoint for this interface.
#define USBD_CUSTOM_CLASS0_IF0_EP1_EN                              1

//         <o.0..1>Type
//           <i>Select Endpoint Type.
//           <i>Endpoint Descriptor: bmAttributes field bits 0 .. 1.
//           <i>If required, for Isochronous Endpoint, Synchronization and Usage Type 
//           <i>can be set by manually editing define value of BMATTRIBUTES.
//           <2=>Bulk
//           <3=>Interrupt
//           <1=>Isochronous
#define USBD_CUSTOM_CLASS0_IF0_EP1_BMATTRIBUTES                    0x02

//         <o.0..3>Number
//           <i>Select Endpoint Number.
//           <i>Endpoint Descriptor: bEndpointAddress field bits 0 .. 3.
//                   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
//           <8=>8   <9=>9   <10=>10 <11=>11 <12=>12 <13=>13 <14=>14 <15=>15
//         <o.7>Direction
//           <i>Select Endpoint Direction.
//           <i>Endpoint Descriptor: bEndpointAddress field bit 7.
//           <0=>OUT
//           <1=>IN
#define USBD_CUSTOM_CLASS0_IF0_EP1_BENDPOINTADDRESS                0x81

//         <h>Speed Settings
//           <i>Settings that are different depending on device operating speed.
//
//           <h>Full/Low-speed
//             <i>Parameters apply when device operates in Full/Low-speed.
//
//             <o.0..9>Maximum Packet Size <0-1023>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 8, 16, 32 or 64.
//               <i>For Interrupt Endpoint set value to 1 .. 64.
//               <i>For Isochronous Endpoint set value to 1 .. 1023.
#define USBD_CUSTOM_CLASS0_IF0_EP1_FS_WMAXPACKETSIZE               64

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in ms).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>Setting is not used for Bulk Endpoint (set value to 0).
//               <i>For Interrupt Endpoint set value to 1 .. 255 (polling interval).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF0_EP1_FS_BINTERVAL                    0

//           </h>

//           <h>High-speed
//             <i>Parameters apply when High-speed is enabled in corresponding USBD_Config_n.c
//             <i>(n is the index of device on which this interface will be used) and when
//             <i>device operates in High-speed.
//
//             <o.0..10>Maximum Packet Size <0-1024>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 512.
//               <i>For Interrupt Endpoint set value to 1 .. 1024.
//               <i>For Isochronous Endpoint set value to 1 .. 1024.
//             <o.11..12>Additional Transactions per Microframe
//               <i>Specifies additional transactions per microframe to improve communication performance.
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 11 .. 12.
//               <i>Relevant only if Endpoint Type is Isochronous or Interrupt.
//               <i>Value: None = 1 transaction per microframe
//               <i>Value: 1 additional = 2 transaction per microframe
//               <i>Value: 2 additional = 3 transaction per microframe
//                 <0=>None
//                 <1=>1 additional
//                 <2=>2 additional
#define USBD_CUSTOM_CLASS0_IF0_EP1_HS_WMAXPACKETSIZE               512

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in 125 us units).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>For Bulk Endpoint this setting represents maximum NAK rate, set value to 0 .. 255.
//               <i>For Interrupt Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF0_EP1_HS_BINTERVAL                    0

//           </h>
//         </h>
//       </e>

//       <e>Endpoint
//       <i>Enable Endpoint for this interface.
#define USBD_CUSTOM_CLASS0_IF0_EP2_EN                              1

//         <o.0..1>Type
//           <i>Select Endpoint Type.
//           <i>Endpoint Descriptor: bmAttributes field bits 0 .. 1.
//           <i>If required, for Isochronous Endpoint, Synchronization and Usage Type 
//           <i>can be set by manually editing define value of BMATTRIBUTES.
//           <2=>Bulk
//           <3=>Interrupt
//           <1=>Isochronous
#define USBD_CUSTOM_CLASS0_IF0_EP2_BMATTRIBUTES                    0x02

//         <o.0..3>Number
//           <i>Select Endpoint Number.
//           <i>Endpoint Descriptor: bEndpointAddress field bits 0 .. 3.
//                   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
//           <8=>8   <9=>9   <10=>10 <11=>11 <12=>12 <13=>13 <14=>14 <15=>15
//         <o.7>Direction
//           <i>Select Endpoint Direction.
//           <i>Endpoint Descriptor: bEndpointAddress field bit 7.
//           <0=>OUT
//           <1=>IN
#define USBD_CUSTOM_CLASS0_IF0_EP2_BENDPOINTADDRESS                0x82

//         <h>Speed Settings
//           <i>Settings that are different depending on device operating speed.
//
//           <h>Full/Low-speed
//             <i>Parameters apply when device operates in Full/Low-speed.
//
//             <o.0..9>Maximum Packet Size <0-1023>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 8, 16, 32 or 64.
//               <i>For Interrupt Endpoint set value to 1 .. 64.
//               <i>For Isochronous Endpoint set value to 1 .. 1023.
#define USBD_CUSTOM_CLASS0_IF0_EP2_FS_WMAXPACKETSIZE               64

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in ms).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>Setting is not used for Bulk Endpoint (set value to 0).
//               <i>For Interrupt Endpoint set value to 1 .. 255 (polling interval).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF0_EP2_FS_BINTERVAL                    0

//           </h>

//           <h>High-speed
//             <i>Parameters apply when High-speed is enabled in corresponding USBD_Config_n.c
//             <i>(n is the index of device on which this interface will be used) and when
//             <i>device operates in High-speed.
//
//             <o.0..10>Maximum Packet Size <0-1024>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 512.
//               <i>For Interrupt Endpoint set value to 1 .. 1024.
//               <i>For Isochronous Endpoint set value to 1 .. 1024.
//             <o.11..12>Additional Transactions per Microframe
//               <i>Specifies additional transactions per microframe to improve communication performance.
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 11 .. 12.
//               <i>Relevant only if Endpoint Type is Isochronous or Interrupt.
//               <i>Value: None = 1 transaction per microframe
//               <i>Value: 1 additional = 2 transaction per microframe
//               <i>Value: 2 additional = 3 transaction per microframe
//                 <0=>None
//                 <1=>1 additional
//                 <2=>2 additional
#define USBD_CUSTOM_CLASS0_IF0_EP2_HS_WMAXPACKETSIZE               512

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in 125 us units).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>For Bulk Endpoint this setting represents maximum NAK rate, set value to 0 .. 255.
//               <i>For Interrupt Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF0_EP2_HS_BINTERVAL                    0

//           </h>
//         </h>
//       </e>

//       <e>Endpoint
//       <i>Enable Endpoint for this interface.
#define USBD_CUSTOM_CLASS0_IF0_EP3_EN                              0

//         <o.0..1>Type
//           <i>Select Endpoint Type.
//           <i>Endpoint Descriptor: bmAttributes field bits 0 .. 1.
//           <i>If required, for Isochronous Endpoint, Synchronization and Usage Type 
//           <i>can be set by manually editing define value of BMATTRIBUTES.
//           <2=>Bulk
//           <3=>Interrupt
//           <1=>Isochronous
#define USBD_CUSTOM_CLASS0_IF0_EP3_BMATTRIBUTES                    0x03

//         <o.0..3>Number
//           <i>Select Endpoint Number.
//           <i>Endpoint Descriptor: bEndpointAddress field bits 0 .. 3.
//                   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
//           <8=>8   <9=>9   <10=>10 <11=>11 <12=>12 <13=>13 <14=>14 <15=>15
//         <o.7>Direction
//           <i>Select Endpoint Direction.
//           <i>Endpoint Descriptor: bEndpointAddress field bit 7.
//           <0=>OUT
//           <1=>IN
#define USBD_CUSTOM_CLASS0_IF0_EP3_BENDPOINTADDRESS                0x82

//         <h>Speed Settings
//           <i>Settings that are different depending on device operating speed.
//
//           <h>Full/Low-speed
//             <i>Parameters apply when device operates in Full/Low-speed.
//
//             <o.0..9>Maximum Packet Size <0-1023>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 8, 16, 32 or 64.
//               <i>For Interrupt Endpoint set value to 1 .. 64.
//               <i>For Isochronous Endpoint set value to 1 .. 1023.
#define USBD_CUSTOM_CLASS0_IF0_EP3_FS_WMAXPACKETSIZE               64

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in ms).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>Setting is not used for Bulk Endpoint (set value to 0).
//               <i>For Interrupt Endpoint set value to 1 .. 255 (polling interval).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF0_EP3_FS_BINTERVAL                    1

//           </h>

//           <h>High-speed
//             <i>Parameters apply when High-speed is enabled in corresponding USBD_Config_n.c
//             <i>(n is the index of device on which this interface will be used) and when
//             <i>device operates in High-speed.
//
//             <o.0..10>Maximum Packet Size <0-1024>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 512.
//               <i>For Interrupt Endpoint set value to 1 .. 1024.
//               <i>For Isochronous Endpoint set value to 1 .. 1024.
//             <o.11..12>Additional Transactions per Microframe
//               <i>Specifies additional transactions per microframe to improve communication performance.
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 11 .. 12.
//               <i>Relevant only if Endpoint Type is Isochronous or Interrupt.
//               <i>Value: None = 1 transaction per microframe
//               <i>Value: 1 additional = 2 transaction per microframe
//               <i>Value: 2 additional = 3 transaction per microframe
//                 <0=>None
//                 <1=>1 additional
//                 <2=>2 additional
#define USBD_CUSTOM_CLASS0_IF0_EP3_HS_WMAXPACKETSIZE               1024

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in 125 us units).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>For Bulk Endpoint this setting represents maximum NAK rate, set value to 0 .. 255.
//               <i>For Interrupt Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF0_EP3_HS_BINTERVAL                    1

//           </h>
//         </h>
//       </e>

//       <e>Endpoint
//       <i>Enable Endpoint for this interface.
#define USBD_CUSTOM_CLASS0_IF0_EP4_EN                              0

//         <o.0..1>Type
//           <i>Select Endpoint Type.
//           <i>Endpoint Descriptor: bmAttributes field bits 0 .. 1.
//           <i>If required, for Isochronous Endpoint, Synchronization and Usage Type 
//           <i>can be set by manually editing define value of BMATTRIBUTES.
//           <2=>Bulk
//           <3=>Interrupt
//           <1=>Isochronous
#define USBD_CUSTOM_CLASS0_IF0_EP4_BMATTRIBUTES                    0x01

//         <o.0..3>Number
//           <i>Select Endpoint Number.
//           <i>Endpoint Descriptor: bEndpointAddress field bits 0 .. 3.
//                   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
//           <8=>8   <9=>9   <10=>10 <11=>11 <12=>12 <13=>13 <14=>14 <15=>15
//         <o.7>Direction
//           <i>Select Endpoint Direction.
//           <i>Endpoint Descriptor: bEndpointAddress field bit 7.
//           <0=>OUT
//           <1=>IN
#define USBD_CUSTOM_CLASS0_IF0_EP4_BENDPOINTADDRESS                0x03

//         <h>Speed Settings
//           <i>Settings that are different depending on device operating speed.
//
//           <h>Full/Low-speed
//             <i>Parameters apply when device operates in Full/Low-speed.
//
//             <o.0..9>Maximum Packet Size <0-1023>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 8, 16, 32 or 64.
//               <i>For Interrupt Endpoint set value to 1 .. 64.
//               <i>For Isochronous Endpoint set value to 1 .. 1023.
#define USBD_CUSTOM_CLASS0_IF0_EP4_FS_WMAXPACKETSIZE               1023

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in ms).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>Setting is not used for Bulk Endpoint (set value to 0).
//               <i>For Interrupt Endpoint set value to 1 .. 255 (polling interval).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF0_EP4_FS_BINTERVAL                    1

//           </h>

//           <h>High-speed
//             <i>Parameters apply when High-speed is enabled in corresponding USBD_Config_n.c
//             <i>(n is the index of device on which this interface will be used) and when
//             <i>device operates in High-speed.
//
//             <o.0..10>Maximum Packet Size <0-1024>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 512.
//               <i>For Interrupt Endpoint set value to 1 .. 1024.
//               <i>For Isochronous Endpoint set value to 1 .. 1024.
//             <o.11..12>Additional Transactions per Microframe
//               <i>Specifies additional transactions per microframe to improve communication performance.
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 11 .. 12.
//               <i>Relevant only if Endpoint Type is Isochronous or Interrupt.
//               <i>Value: None = 1 transaction per microframe
//               <i>Value: 1 additional = 2 transaction per microframe
//               <i>Value: 2 additional = 3 transaction per microframe
//                 <0=>None
//                 <1=>1 additional
//                 <2=>2 additional
#define USBD_CUSTOM_CLASS0_IF0_EP4_HS_WMAXPACKETSIZE               1024

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in 125 us units).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>For Bulk Endpoint this setting represents maximum NAK rate, set value to 0 .. 255.
//               <i>For Interrupt Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF0_EP4_HS_BINTERVAL                    1

//           </h>
//         </h>
//       </e>

//       <e>Endpoint
//       <i>Enable Endpoint for this interface.
#define USBD_CUSTOM_CLASS0_IF0_EP5_EN                              0

//         <o.0..1>Type
//           <i>Select Endpoint Type.
//           <i>Endpoint Descriptor: bmAttributes field bits 0 .. 1.
//           <i>If required, for Isochronous Endpoint, Synchronization and Usage Type 
//           <i>can be set by manually editing define value of BMATTRIBUTES.
//           <2=>Bulk
//           <3=>Interrupt
//           <1=>Isochronous
#define USBD_CUSTOM_CLASS0_IF0_EP5_BMATTRIBUTES                    0x01

//         <o.0..3>Number
//           <i>Select Endpoint Number.
//           <i>Endpoint Descriptor: bEndpointAddress field bits 0 .. 3.
//                   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
//           <8=>8   <9=>9   <10=>10 <11=>11 <12=>12 <13=>13 <14=>14 <15=>15
//         <o.7>Direction
//           <i>Select Endpoint Direction.
//           <i>Endpoint Descriptor: bEndpointAddress field bit 7.
//           <0=>OUT
//           <1=>IN
#define USBD_CUSTOM_CLASS0_IF0_EP5_BENDPOINTADDRESS                0x83

//         <h>Speed Settings
//           <i>Settings that are different depending on device operating speed.
//
//           <h>Full/Low-speed
//             <i>Parameters apply when device operates in Full/Low-speed.
//
//             <o.0..9>Maximum Packet Size <0-1023>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 8, 16, 32 or 64.
//               <i>For Interrupt Endpoint set value to 1 .. 64.
//               <i>For Isochronous Endpoint set value to 1 .. 1023.
#define USBD_CUSTOM_CLASS0_IF0_EP5_FS_WMAXPACKETSIZE               1023

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in ms).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>Setting is not used for Bulk Endpoint (set value to 0).
//               <i>For Interrupt Endpoint set value to 1 .. 255 (polling interval).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF0_EP5_FS_BINTERVAL                    1

//           </h>

//           <h>High-speed
//             <i>Parameters apply when High-speed is enabled in corresponding USBD_Config_n.c
//             <i>(n is the index of device on which this interface will be used) and when
//             <i>device operates in High-speed.
//
//             <o.0..10>Maximum Packet Size <0-1024>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 512.
//               <i>For Interrupt Endpoint set value to 1 .. 1024.
//               <i>For Isochronous Endpoint set value to 1 .. 1024.
//             <o.11..12>Additional Transactions per Microframe
//               <i>Specifies additional transactions per microframe to improve communication performance.
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 11 .. 12.
//               <i>Relevant only if Endpoint Type is Isochronous or Interrupt.
//               <i>Value: None = 1 transaction per microframe
//               <i>Value: 1 additional = 2 transaction per microframe
//               <i>Value: 2 additional = 3 transaction per microframe
//                 <0=>None
//                 <1=>1 additional
//                 <2=>2 additional
#define USBD_CUSTOM_CLASS0_IF0_EP5_HS_WMAXPACKETSIZE               1024

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in 125 us units).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>For Bulk Endpoint this setting represents maximum NAK rate, set value to 0 .. 255.
//               <i>For Interrupt Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF0_EP5_HS_BINTERVAL                    1

//           </h>
//         </h>
//       </e>

//       <e>Endpoint
//       <i>Enable Endpoint for this interface.
#define USBD_CUSTOM_CLASS0_IF0_EP6_EN                              0

//         <o.0..1>Type
//           <i>Select Endpoint Type.
//           <i>Endpoint Descriptor: bmAttributes field bits 0 .. 1.
//           <i>If required, for Isochronous Endpoint, Synchronization and Usage Type 
//           <i>can be set by manually editing define value of BMATTRIBUTES.
//           <2=>Bulk
//           <3=>Interrupt
//           <1=>Isochronous
#define USBD_CUSTOM_CLASS0_IF0_EP6_BMATTRIBUTES                    0x02

//         <o.0..3>Number
//           <i>Select Endpoint Number.
//           <i>Endpoint Descriptor: bEndpointAddress field bits 0 .. 3.
//                   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
//           <8=>8   <9=>9   <10=>10 <11=>11 <12=>12 <13=>13 <14=>14 <15=>15
//         <o.7>Direction
//           <i>Select Endpoint Direction.
//           <i>Endpoint Descriptor: bEndpointAddress field bit 7.
//           <0=>OUT
//           <1=>IN
#define USBD_CUSTOM_CLASS0_IF0_EP6_BENDPOINTADDRESS                0x04

//         <h>Speed Settings
//           <i>Settings that are different depending on device operating speed.
//
//           <h>Full/Low-speed
//             <i>Parameters apply when device operates in Full/Low-speed.
//
//             <o.0..9>Maximum Packet Size <0-1023>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 8, 16, 32 or 64.
//               <i>For Interrupt Endpoint set value to 1 .. 64.
//               <i>For Isochronous Endpoint set value to 1 .. 1023.
#define USBD_CUSTOM_CLASS0_IF0_EP6_FS_WMAXPACKETSIZE               64

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in ms).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>Setting is not used for Bulk Endpoint (set value to 0).
//               <i>For Interrupt Endpoint set value to 1 .. 255 (polling interval).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF0_EP6_FS_BINTERVAL                    0

//           </h>

//           <h>High-speed
//             <i>Parameters apply when High-speed is enabled in corresponding USBD_Config_n.c
//             <i>(n is the index of device on which this interface will be used) and when
//             <i>device operates in High-speed.
//
//             <o.0..10>Maximum Packet Size <0-1024>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 512.
//               <i>For Interrupt Endpoint set value to 1 .. 1024.
//               <i>For Isochronous Endpoint set value to 1 .. 1024.
//             <o.11..12>Additional Transactions per Microframe
//               <i>Specifies additional transactions per microframe to improve communication performance.
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 11 .. 12.
//               <i>Relevant only if Endpoint Type is Isochronous or Interrupt.
//               <i>Value: None = 1 transaction per microframe
//               <i>Value: 1 additional = 2 transaction per microframe
//               <i>Value: 2 additional = 3 transaction per microframe
//                 <0=>None
//                 <1=>1 additional
//                 <2=>2 additional
#define USBD_CUSTOM_CLASS0_IF0_EP6_HS_WMAXPACKETSIZE               512

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in 125 us units).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>For Bulk Endpoint this setting represents maximum NAK rate, set value to 0 .. 255.
//               <i>For Interrupt Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF0_EP6_HS_BINTERVAL                    0

//           </h>
//         </h>
//       </e>

//       <e>Endpoint
//       <i>Enable Endpoint for this interface.
#define USBD_CUSTOM_CLASS0_IF0_EP7_EN                              0

//         <o.0..1>Type
//           <i>Select Endpoint Type.
//           <i>Endpoint Descriptor: bmAttributes field bits 0 .. 1.
//           <i>If required, for Isochronous Endpoint, Synchronization and Usage Type 
//           <i>can be set by manually editing define value of BMATTRIBUTES.
//           <2=>Bulk
//           <3=>Interrupt
//           <1=>Isochronous
#define USBD_CUSTOM_CLASS0_IF0_EP7_BMATTRIBUTES                    0x02

//         <o.0..3>Number
//           <i>Select Endpoint Number.
//           <i>Endpoint Descriptor: bEndpointAddress field bits 0 .. 3.
//                   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
//           <8=>8   <9=>9   <10=>10 <11=>11 <12=>12 <13=>13 <14=>14 <15=>15
//         <o.7>Direction
//           <i>Select Endpoint Direction.
//           <i>Endpoint Descriptor: bEndpointAddress field bit 7.
//           <0=>OUT
//           <1=>IN
#define USBD_CUSTOM_CLASS0_IF0_EP7_BENDPOINTADDRESS                0x84

//         <h>Speed Settings
//           <i>Settings that are different depending on device operating speed.
//
//           <h>Full/Low-speed
//             <i>Parameters apply when device operates in Full/Low-speed.
//
//             <o.0..9>Maximum Packet Size <0-1023>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 8, 16, 32 or 64.
//               <i>For Interrupt Endpoint set value to 1 .. 64.
//               <i>For Isochronous Endpoint set value to 1 .. 1023.
#define USBD_CUSTOM_CLASS0_IF0_EP7_FS_WMAXPACKETSIZE               64

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in ms).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>Setting is not used for Bulk Endpoint (set value to 0).
//               <i>For Interrupt Endpoint set value to 1 .. 255 (polling interval).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF0_EP7_FS_BINTERVAL                    0

//           </h>

//           <h>High-speed
//             <i>Parameters apply when High-speed is enabled in corresponding USBD_Config_n.c
//             <i>(n is the index of device on which this interface will be used) and when
//             <i>device operates in High-speed.
//
//             <o.0..10>Maximum Packet Size <0-1024>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 512.
//               <i>For Interrupt Endpoint set value to 1 .. 1024.
//               <i>For Isochronous Endpoint set value to 1 .. 1024.
//             <o.11..12>Additional Transactions per Microframe
//               <i>Specifies additional transactions per microframe to improve communication performance.
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 11 .. 12.
//               <i>Relevant only if Endpoint Type is Isochronous or Interrupt.
//               <i>Value: None = 1 transaction per microframe
//               <i>Value: 1 additional = 2 transaction per microframe
//               <i>Value: 2 additional = 3 transaction per microframe
//                 <0=>None
//                 <1=>1 additional
//                 <2=>2 additional
#define USBD_CUSTOM_CLASS0_IF0_EP7_HS_WMAXPACKETSIZE               512

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in 125 us units).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>For Bulk Endpoint this setting represents maximum NAK rate, set value to 0 .. 255.
//               <i>For Interrupt Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF0_EP7_HS_BINTERVAL                    0

//           </h>
//         </h>
//       </e>
//     </h>

//     <h>String Settings
//     <i>Following settings are used to create String Descriptor(s)

//       <e.0>Interface String Enable
//       <i>Enable Interface String.
//       <i>If disabled Interface String will not be assigned to USB Device Custom Class Interface 0.
#define USBD_CUSTOM_CLASS0_IF0_STR_EN                              1

//         <s.126>Interface String
#define USBD_CUSTOM_CLASS0_IF0_STR                                 L"LPC-Link-II CMSIS-DAP"

//       </e>
//     </h>

//     <h>Microsoft OS Descriptor Settings
//     <i>Following settings are used to create Extended Compat ID OS Feature Descriptor

//       <e.0>Extended Compat ID OS Feature Descriptor Function Section
//       <i>Enable creation of function section in Extended Compat ID OS Feature Descriptor for this interface.
#define USBD_CUSTOM_CLASS0_IF0_OS_EXT_COMPAT_ID_EN                 1

//         <s.7>compatibleID
//         <i>compatibleID field of function section in Extended Compat ID OS Feature Descriptor for this interface.
#define USBD_CUSTOM_CLASS0_IF0_OS_EXT_COMPAT_ID                    "WINUSB"

//         <s.7>subCompatibleID
//         <i>subCompatibleID field of function section in Extended Compat ID OS Feature Descriptor for this interface.
#define USBD_CUSTOM_CLASS0_IF0_OS_EXT_SUBCOMPAT_ID                 ""

//       </e>

//       <h>Extended Properties OS Feature Descriptor
//         <e.0>Custom Property Section 0
//         <i>Enable creation of custom property 0 section in Extended Properties OS Feature Descriptor.
#define USBD_CUSTOM_CLASS0_IF0_OS_EXT_PROP0_EN                     1

//           <o>Data Type
//           <i>Specifies the dwPropertyDataType field of custom property 0 section in Extended Properties OS Feature Descriptor.
//           <i>Values 3 (Free-form binary) and 7 (Multiple Unicode Strings) are not supported.
//             <1=>Unicode String (REG_SZ)
//             <2=>Unicode String with environment variables (REG_EXPAND_SZ)
//             <4=>Little-endian 32-bit integer (REG_DWORD_LITTLE_ENDIAN)
//             <5=>Big-endian 32-bit integer (REG_DWORD_BIG_ENDIAN)
//             <6=>Unicode String with symbolic link (REG_LINK)
#define USBD_CUSTOM_CLASS0_IF0_OS_EXT_PROP0_DATA_TYP               1

//           <s.512>Name
#define USBD_CUSTOM_CLASS0_IF0_OS_EXT_PROP0_NAME                   L"DeviceInterfaceGUID"

//           <h>Data
//             <s.1024>Unicode String
//             <i>Property Data in case Data Type is selected as Unicode String.
#define USBD_CUSTOM_CLASS0_IF0_OS_EXT_PROP0_DATA_STR               L"{CDB3B5AD-293B-4663-AA36-1AAE46463776}"

//             <o>32-bit Integer
//             <i>Property Data in case Data Type is selected as Little/Big-endian 32-bit Integer.
#define USBD_CUSTOM_CLASS0_IF0_OS_EXT_PROP0_DATA_INT               0

//           </h>
//         </e>

//         <e.0>Custom Property Section 1
//         <i>Enable creation of custom property 1 section in Extended Properties OS Feature Descriptor.
#define USBD_CUSTOM_CLASS0_IF0_OS_EXT_PROP1_EN                     0

//           <o>Data Type
//           <i>Specifies the dwPropertyDataType field of custom property 1 section in Extended Properties OS Feature Descriptor.
//           <i>Values 3 (Free-form binary) and 7 (Multiple Unicode Strings) are not supported.
//             <1=>Unicode String (REG_SZ)
//             <2=>Unicode String with environment variables (REG_EXPAND_SZ)
//             <4=>Little-endian 32-bit integer (REG_DWORD_LITTLE_ENDIAN)
//             <5=>Big-endian 32-bit integer (REG_DWORD_BIG_ENDIAN)
//             <6=>Unicode String with symbolic link (REG_LINK)
#define USBD_CUSTOM_CLASS0_IF0_OS_EXT_PROP1_DATA_TYP               1

//           <s.512>Name
#define USBD_CUSTOM_CLASS0_IF0_OS_EXT_PROP1_NAME                   L""

//           <h>Data
//             <s.1024>Unicode String
//             <i>Property Data in case Data Type is selected as Unicode String.
#define USBD_CUSTOM_CLASS0_IF0_OS_EXT_PROP1_DATA_STR               L""

//             <o>32-bit Integer
//             <i>Property Data in case Data Type is selected as Little/Big-endian 32-bit Integer.
#define USBD_CUSTOM_CLASS0_IF0_OS_EXT_PROP1_DATA_INT               0

//           </h>
//         </e>

//         <e.0>Custom Property Section 2
//         <i>Enable creation of custom property 2 section in Extended Properties OS Feature Descriptor.
#define USBD_CUSTOM_CLASS0_IF0_OS_EXT_PROP2_EN                     0

//           <o>Data Type
//           <i>Specifies the dwPropertyDataType field of custom property 2 section in Extended Properties OS Feature Descriptor.
//           <i>Values 3 (Free-form binary) and 7 (Multiple Unicode Strings) are not supported.
//             <1=>Unicode String (REG_SZ)
//             <2=>Unicode String with environment variables (REG_EXPAND_SZ)
//             <4=>Little-endian 32-bit integer (REG_DWORD_LITTLE_ENDIAN)
//             <5=>Big-endian 32-bit integer (REG_DWORD_BIG_ENDIAN)
//             <6=>Unicode String with symbolic link (REG_LINK)
#define USBD_CUSTOM_CLASS0_IF0_OS_EXT_PROP2_DATA_TYP               1

//           <s.512>Name
#define USBD_CUSTOM_CLASS0_IF0_OS_EXT_PROP2_NAME                   L""

//           <h>Data
//             <s.1024>Unicode String
//             <i>Property Data in case Data Type is selected as Unicode String.
#define USBD_CUSTOM_CLASS0_IF0_OS_EXT_PROP2_DATA_STR               L""

//             <o>32-bit Integer
//             <i>Property Data in case Data Type is selected as Little/Big-endian 32-bit Integer.
#define USBD_CUSTOM_CLASS0_IF0_OS_EXT_PROP2_DATA_INT               0

//           </h>
//         </e>

//         <e.0>Custom Property Section 3
//         <i>Enable creation of custom property 3 section in Extended Properties OS Feature Descriptor.
#define USBD_CUSTOM_CLASS0_IF0_OS_EXT_PROP3_EN                     0

//           <o>Data Type
//           <i>Specifies the dwPropertyDataType field of custom property 3 section in Extended Properties OS Feature Descriptor.
//           <i>Values 3 (Free-form binary) and 7 (Multiple Unicode Strings) are not supported.
//             <1=>Unicode String (REG_SZ)
//             <2=>Unicode String with environment variables (REG_EXPAND_SZ)
//             <4=>Little-endian 32-bit integer (REG_DWORD_LITTLE_ENDIAN)
//             <5=>Big-endian 32-bit integer (REG_DWORD_BIG_ENDIAN)
//             <6=>Unicode String with symbolic link (REG_LINK)
#define USBD_CUSTOM_CLASS0_IF0_OS_EXT_PROP3_DATA_TYP               1

//           <s.512>Name
#define USBD_CUSTOM_CLASS0_IF0_OS_EXT_PROP3_NAME                   L""

//           <h>Data
//             <s.1024>Unicode String
//             <i>Property Data in case Data Type is selected as Unicode String.
#define USBD_CUSTOM_CLASS0_IF0_OS_EXT_PROP3_DATA_STR               L""

//             <o>32-bit Integer
//             <i>Property Data in case Data Type is selected as Little/Big-endian 32-bit Integer.
#define USBD_CUSTOM_CLASS0_IF0_OS_EXT_PROP3_DATA_INT               0

//           </h>
//         </e>
//       </h>
//     </h>
//   </e>


//   <e>Interface
#define USBD_CUSTOM_CLASS0_IF1_EN                                  0

//     <h>Interface Settings
//     <i>The Interface Settings are used to create the Interface Descriptor.
//     <i>Refer to USB - USB Concepts - USB Descriptor in the MDK Components
//     <i>User's Guide for more information about the Interface Descriptor.

//       <o>Interface Number <0-255>
//       <i>Defines the value for bInterfaceNumber
//       <i>Each USB Device Interface has a sequential Interface Number starting with 0.
//       <i>Several Interfaces may have the same Interface Number; in this case the value
//       <i>of Alternate Setting is used to differ between the Interfaces. For a
//       <i>composite device the Interface Numbers of the custom classes must be contiguous.
#define USBD_CUSTOM_CLASS0_IF1_NUM                                 1

//       <o>Alternate Setting <0=>0 <1=>1 <2=>2 <3=>3
//       <i>Defines the value for bAlternateSetting
//       <i>A sequential number starting with 0 to identify the Interface Descriptors
//       <i>that share the same value for Interface Number.
#define USBD_CUSTOM_CLASS0_IF1_ALT                                 0

//       <o.0..7>Class Code
//         <i>Class Codes are defined by USB-IF. For more information refer to
//         <i>http://www.usb.org/developers/defined_class.
//         <0x00=>0x00: Indicate a Null Class Code triple
//         <0x01=>0x01: Audio
//         <0x02=>0x02: Communications and CDC Control
//         <0x03=>0x03: HID (Human Interface Device)
//         <0x05=>0x05: Physical
//         <0x06=>0x06: Image
//         <0x07=>0x07: Printer
//         <0x08=>0x08: Mass Storage
//         <0x0A=>0x0A: CDC-Data
//         <0x0B=>0x0B: Smart Card
//         <0x0D=>0x0D: Content Security
//         <0x0E=>0x0E: Video
//         <0x0F=>0x0F: Personal Healthcare
//         <0x10=>0x10: Audio/Video Devices
//         <0xDC=>0xDC: Diagnostic Device
//         <0xE0=>0xE0: Wireless Controller
//         <0xEF=>0xEF: Miscellaneous
//         <0xFE=>0xFE: Application Specific
//         <0xFF=>0xFF: Vendor Specific
#define USBD_CUSTOM_CLASS0_IF1_CLASS                               0xFF

//       <o.0..7>Subclass Code <0x00-0xFF>
//       <i>The possible values depend on the Class Code:
//       <i>Class Code 0x00: Subclass Code must be 0
//       <i>Class Code 0x01 .. 0xFE: Subclass Code is defined by USB-IF
//       <i>Class Code 0xFF: Subclass Code can be 0x00 .. 0xFF
#define USBD_CUSTOM_CLASS0_IF1_SUBCLASS                            0x00

//       <o.0..7>Protocol Code <0x00-0xFF>
//       <i>The Protocol Code value defines the protocol used on this interface:
//       <i>Protocol Code 0x00: class-specific protocol not used
//       <i>Protocol Code 0x01 .. 0xFE: class-specific protocol used
//       <i>Protocol Code 0xFF: vendor-specific protocol used
#define USBD_CUSTOM_CLASS0_IF1_PROTOCOL                            0x00

//     </h>

//     <h>Endpoint Settings
//     <i>Following settings are used to create the Endpoint Descriptors.
//     <i>Refer to USB - USB Concepts - USB Descriptor in the MDK Components
//     <i>User's Guide for more information about Endpoint Descriptors.

//       <e>Endpoint
//       <i>Enable Endpoint for this interface.
#define USBD_CUSTOM_CLASS0_IF1_EP0_EN                              0

//         <o.0..1>Type
//           <i>Select Endpoint Type.
//           <i>Endpoint Descriptor: bmAttributes field bits 0 .. 1.
//           <i>If required, for Isochronous Endpoint, Synchronization and Usage Type 
//           <i>can be set by manually editing define value of BMATTRIBUTES.
//           <2=>Bulk
//           <3=>Interrupt
//           <1=>Isochronous
#define USBD_CUSTOM_CLASS0_IF1_EP0_BMATTRIBUTES                    0x02

//         <o.0..3>Number
//           <i>Select Endpoint Number.
//           <i>Endpoint Descriptor: bEndpointAddress field bits 0 .. 3.
//                   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
//           <8=>8   <9=>9   <10=>10 <11=>11 <12=>12 <13=>13 <14=>14 <15=>15
//         <o.7>Direction
//           <i>Select Endpoint Direction.
//           <i>Endpoint Descriptor: bEndpointAddress field bit 7.
//           <0=>OUT
//           <1=>IN
#define USBD_CUSTOM_CLASS0_IF1_EP0_BENDPOINTADDRESS                0x01

//         <h>Speed Settings
//           <i>Settings that are different depending on device operating speed.
//
//           <h>Full/Low-speed
//             <i>Parameters apply when device operates in Full/Low-speed.
//
//             <o.0..9>Maximum Packet Size <0-1023>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 8, 16, 32 or 64.
//               <i>For Interrupt Endpoint set value to 1 .. 64.
//               <i>For Isochronous Endpoint set value to 1 .. 1023.
#define USBD_CUSTOM_CLASS0_IF1_EP0_FS_WMAXPACKETSIZE               64

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in ms).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>Setting is not used for Bulk Endpoint (set value to 0).
//               <i>For Interrupt Endpoint set value to 1 .. 255 (polling interval).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF1_EP0_FS_BINTERVAL                    0

//           </h>

//           <h>High-speed
//             <i>Parameters apply when High-speed is enabled in corresponding USBD_Config_n.c
//             <i>(n is the index of device on which this interface will be used) and when
//             <i>device operates in High-speed.
//
//             <o.0..10>Maximum Packet Size <0-1024>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 512.
//               <i>For Interrupt Endpoint set value to 1 .. 1024.
//               <i>For Isochronous Endpoint set value to 1 .. 1024.
//             <o.11..12>Additional Transactions per Microframe
//               <i>Specifies additional transactions per microframe to improve communication performance.
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 11 .. 12.
//               <i>Relevant only if Endpoint Type is Isochronous or Interrupt.
//               <i>Value: None = 1 transaction per microframe
//               <i>Value: 1 additional = 2 transaction per microframe
//               <i>Value: 2 additional = 3 transaction per microframe
//                 <0=>None
//                 <1=>1 additional
//                 <2=>2 additional
#define USBD_CUSTOM_CLASS0_IF1_EP0_HS_WMAXPACKETSIZE               512

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in 125 us units).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>For Bulk Endpoint this setting represents maximum NAK rate, set value to 0 .. 255.
//               <i>For Interrupt Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF1_EP0_HS_BINTERVAL                    0

//           </h>
//         </h>
//       </e>

//       <e>Endpoint
//       <i>Enable Endpoint for this interface.
#define USBD_CUSTOM_CLASS0_IF1_EP1_EN                              0

//         <o.0..1>Type
//           <i>Select Endpoint Type.
//           <i>Endpoint Descriptor: bmAttributes field bits 0 .. 1.
//           <i>If required, for Isochronous Endpoint, Synchronization and Usage Type 
//           <i>can be set by manually editing define value of BMATTRIBUTES.
//           <2=>Bulk
//           <3=>Interrupt
//           <1=>Isochronous
#define USBD_CUSTOM_CLASS0_IF1_EP1_BMATTRIBUTES                    0x02

//         <o.0..3>Number
//           <i>Select Endpoint Number.
//           <i>Endpoint Descriptor: bEndpointAddress field bits 0 .. 3.
//                   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
//           <8=>8   <9=>9   <10=>10 <11=>11 <12=>12 <13=>13 <14=>14 <15=>15
//         <o.7>Direction
//           <i>Select Endpoint Direction.
//           <i>Endpoint Descriptor: bEndpointAddress field bit 7.
//           <0=>OUT
//           <1=>IN
#define USBD_CUSTOM_CLASS0_IF1_EP1_BENDPOINTADDRESS                0x81

//         <h>Speed Settings
//           <i>Settings that are different depending on device operating speed.
//
//           <h>Full/Low-speed
//             <i>Parameters apply when device operates in Full/Low-speed.
//
//             <o.0..9>Maximum Packet Size <0-1023>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 8, 16, 32 or 64.
//               <i>For Interrupt Endpoint set value to 1 .. 64.
//               <i>For Isochronous Endpoint set value to 1 .. 1023.
#define USBD_CUSTOM_CLASS0_IF1_EP1_FS_WMAXPACKETSIZE               64

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in ms).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>Setting is not used for Bulk Endpoint (set value to 0).
//               <i>For Interrupt Endpoint set value to 1 .. 255 (polling interval).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF1_EP1_FS_BINTERVAL                    0

//           </h>

//           <h>High-speed
//             <i>Parameters apply when High-speed is enabled in corresponding USBD_Config_n.c
//             <i>(n is the index of device on which this interface will be used) and when
//             <i>device operates in High-speed.
//
//             <o.0..10>Maximum Packet Size <0-1024>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 512.
//               <i>For Interrupt Endpoint set value to 1 .. 1024.
//               <i>For Isochronous Endpoint set value to 1 .. 1024.
//             <o.11..12>Additional Transactions per Microframe
//               <i>Specifies additional transactions per microframe to improve communication performance.
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 11 .. 12.
//               <i>Relevant only if Endpoint Type is Isochronous or Interrupt.
//               <i>Value: None = 1 transaction per microframe
//               <i>Value: 1 additional = 2 transaction per microframe
//               <i>Value: 2 additional = 3 transaction per microframe
//                 <0=>None
//                 <1=>1 additional
//                 <2=>2 additional
#define USBD_CUSTOM_CLASS0_IF1_EP1_HS_WMAXPACKETSIZE               512

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in 125 us units).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>For Bulk Endpoint this setting represents maximum NAK rate, set value to 0 .. 255.
//               <i>For Interrupt Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF1_EP1_HS_BINTERVAL                    0

//           </h>
//         </h>
//       </e>

//       <e>Endpoint
//       <i>Enable Endpoint for this interface.
#define USBD_CUSTOM_CLASS0_IF1_EP2_EN                              0

//         <o.0..1>Type
//           <i>Select Endpoint Type.
//           <i>Endpoint Descriptor: bmAttributes field bits 0 .. 1.
//           <i>If required, for Isochronous Endpoint, Synchronization and Usage Type 
//           <i>can be set by manually editing define value of BMATTRIBUTES.
//           <2=>Bulk
//           <3=>Interrupt
//           <1=>Isochronous
#define USBD_CUSTOM_CLASS0_IF1_EP2_BMATTRIBUTES                    0x03

//         <o.0..3>Number
//           <i>Select Endpoint Number.
//           <i>Endpoint Descriptor: bEndpointAddress field bits 0 .. 3.
//                   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
//           <8=>8   <9=>9   <10=>10 <11=>11 <12=>12 <13=>13 <14=>14 <15=>15
//         <o.7>Direction
//           <i>Select Endpoint Direction.
//           <i>Endpoint Descriptor: bEndpointAddress field bit 7.
//           <0=>OUT
//           <1=>IN
#define USBD_CUSTOM_CLASS0_IF1_EP2_BENDPOINTADDRESS                0x02

//         <h>Speed Settings
//           <i>Settings that are different depending on device operating speed.
//
//           <h>Full/Low-speed
//             <i>Parameters apply when device operates in Full/Low-speed.
//
//             <o.0..9>Maximum Packet Size <0-1023>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 8, 16, 32 or 64.
//               <i>For Interrupt Endpoint set value to 1 .. 64.
//               <i>For Isochronous Endpoint set value to 1 .. 1023.
#define USBD_CUSTOM_CLASS0_IF1_EP2_FS_WMAXPACKETSIZE               64

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in ms).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>Setting is not used for Bulk Endpoint (set value to 0).
//               <i>For Interrupt Endpoint set value to 1 .. 255 (polling interval).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF1_EP2_FS_BINTERVAL                    1

//           </h>

//           <h>High-speed
//             <i>Parameters apply when High-speed is enabled in corresponding USBD_Config_n.c
//             <i>(n is the index of device on which this interface will be used) and when
//             <i>device operates in High-speed.
//
//             <o.0..10>Maximum Packet Size <0-1024>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 512.
//               <i>For Interrupt Endpoint set value to 1 .. 1024.
//               <i>For Isochronous Endpoint set value to 1 .. 1024.
//             <o.11..12>Additional Transactions per Microframe
//               <i>Specifies additional transactions per microframe to improve communication performance.
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 11 .. 12.
//               <i>Relevant only if Endpoint Type is Isochronous or Interrupt.
//               <i>Value: None = 1 transaction per microframe
//               <i>Value: 1 additional = 2 transaction per microframe
//               <i>Value: 2 additional = 3 transaction per microframe
//                 <0=>None
//                 <1=>1 additional
//                 <2=>2 additional
#define USBD_CUSTOM_CLASS0_IF1_EP2_HS_WMAXPACKETSIZE               1024

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in 125 us units).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>For Bulk Endpoint this setting represents maximum NAK rate, set value to 0 .. 255.
//               <i>For Interrupt Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF1_EP2_HS_BINTERVAL                    1

//           </h>
//         </h>
//       </e>

//       <e>Endpoint
//       <i>Enable Endpoint for this interface.
#define USBD_CUSTOM_CLASS0_IF1_EP3_EN                              0

//         <o.0..1>Type
//           <i>Select Endpoint Type.
//           <i>Endpoint Descriptor: bmAttributes field bits 0 .. 1.
//           <i>If required, for Isochronous Endpoint, Synchronization and Usage Type 
//           <i>can be set by manually editing define value of BMATTRIBUTES.
//           <2=>Bulk
//           <3=>Interrupt
//           <1=>Isochronous
#define USBD_CUSTOM_CLASS0_IF1_EP3_BMATTRIBUTES                    0x03

//         <o.0..3>Number
//           <i>Select Endpoint Number.
//           <i>Endpoint Descriptor: bEndpointAddress field bits 0 .. 3.
//                   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
//           <8=>8   <9=>9   <10=>10 <11=>11 <12=>12 <13=>13 <14=>14 <15=>15
//         <o.7>Direction
//           <i>Select Endpoint Direction.
//           <i>Endpoint Descriptor: bEndpointAddress field bit 7.
//           <0=>OUT
//           <1=>IN
#define USBD_CUSTOM_CLASS0_IF1_EP3_BENDPOINTADDRESS                0x82

//         <h>Speed Settings
//           <i>Settings that are different depending on device operating speed.
//
//           <h>Full/Low-speed
//             <i>Parameters apply when device operates in Full/Low-speed.
//
//             <o.0..9>Maximum Packet Size <0-1023>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 8, 16, 32 or 64.
//               <i>For Interrupt Endpoint set value to 1 .. 64.
//               <i>For Isochronous Endpoint set value to 1 .. 1023.
#define USBD_CUSTOM_CLASS0_IF1_EP3_FS_WMAXPACKETSIZE               64

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in ms).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>Setting is not used for Bulk Endpoint (set value to 0).
//               <i>For Interrupt Endpoint set value to 1 .. 255 (polling interval).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF1_EP3_FS_BINTERVAL                    1

//           </h>

//           <h>High-speed
//             <i>Parameters apply when High-speed is enabled in corresponding USBD_Config_n.c
//             <i>(n is the index of device on which this interface will be used) and when
//             <i>device operates in High-speed.
//
//             <o.0..10>Maximum Packet Size <0-1024>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 512.
//               <i>For Interrupt Endpoint set value to 1 .. 1024.
//               <i>For Isochronous Endpoint set value to 1 .. 1024.
//             <o.11..12>Additional Transactions per Microframe
//               <i>Specifies additional transactions per microframe to improve communication performance.
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 11 .. 12.
//               <i>Relevant only if Endpoint Type is Isochronous or Interrupt.
//               <i>Value: None = 1 transaction per microframe
//               <i>Value: 1 additional = 2 transaction per microframe
//               <i>Value: 2 additional = 3 transaction per microframe
//                 <0=>None
//                 <1=>1 additional
//                 <2=>2 additional
#define USBD_CUSTOM_CLASS0_IF1_EP3_HS_WMAXPACKETSIZE               1024

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in 125 us units).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>For Bulk Endpoint this setting represents maximum NAK rate, set value to 0 .. 255.
//               <i>For Interrupt Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF1_EP3_HS_BINTERVAL                    1

//           </h>
//         </h>
//       </e>

//       <e>Endpoint
//       <i>Enable Endpoint for this interface.
#define USBD_CUSTOM_CLASS0_IF1_EP4_EN                              0

//         <o.0..1>Type
//           <i>Select Endpoint Type.
//           <i>Endpoint Descriptor: bmAttributes field bits 0 .. 1.
//           <i>If required, for Isochronous Endpoint, Synchronization and Usage Type 
//           <i>can be set by manually editing define value of BMATTRIBUTES.
//           <2=>Bulk
//           <3=>Interrupt
//           <1=>Isochronous
#define USBD_CUSTOM_CLASS0_IF1_EP4_BMATTRIBUTES                    0x01

//         <o.0..3>Number
//           <i>Select Endpoint Number.
//           <i>Endpoint Descriptor: bEndpointAddress field bits 0 .. 3.
//                   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
//           <8=>8   <9=>9   <10=>10 <11=>11 <12=>12 <13=>13 <14=>14 <15=>15
//         <o.7>Direction
//           <i>Select Endpoint Direction.
//           <i>Endpoint Descriptor: bEndpointAddress field bit 7.
//           <0=>OUT
//           <1=>IN
#define USBD_CUSTOM_CLASS0_IF1_EP4_BENDPOINTADDRESS                0x03

//         <h>Speed Settings
//           <i>Settings that are different depending on device operating speed.
//
//           <h>Full/Low-speed
//             <i>Parameters apply when device operates in Full/Low-speed.
//
//             <o.0..9>Maximum Packet Size <0-1023>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 8, 16, 32 or 64.
//               <i>For Interrupt Endpoint set value to 1 .. 64.
//               <i>For Isochronous Endpoint set value to 1 .. 1023.
#define USBD_CUSTOM_CLASS0_IF1_EP4_FS_WMAXPACKETSIZE               1023

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in ms).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>Setting is not used for Bulk Endpoint (set value to 0).
//               <i>For Interrupt Endpoint set value to 1 .. 255 (polling interval).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF1_EP4_FS_BINTERVAL                    1

//           </h>

//           <h>High-speed
//             <i>Parameters apply when High-speed is enabled in corresponding USBD_Config_n.c
//             <i>(n is the index of device on which this interface will be used) and when
//             <i>device operates in High-speed.
//
//             <o.0..10>Maximum Packet Size <0-1024>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 512.
//               <i>For Interrupt Endpoint set value to 1 .. 1024.
//               <i>For Isochronous Endpoint set value to 1 .. 1024.
//             <o.11..12>Additional Transactions per Microframe
//               <i>Specifies additional transactions per microframe to improve communication performance.
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 11 .. 12.
//               <i>Relevant only if Endpoint Type is Isochronous or Interrupt.
//               <i>Value: None = 1 transaction per microframe
//               <i>Value: 1 additional = 2 transaction per microframe
//               <i>Value: 2 additional = 3 transaction per microframe
//                 <0=>None
//                 <1=>1 additional
//                 <2=>2 additional
#define USBD_CUSTOM_CLASS0_IF1_EP4_HS_WMAXPACKETSIZE               1024

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in 125 us units).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>For Bulk Endpoint this setting represents maximum NAK rate, set value to 0 .. 255.
//               <i>For Interrupt Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF1_EP4_HS_BINTERVAL                    1

//           </h>
//         </h>
//       </e>

//       <e>Endpoint
//       <i>Enable Endpoint for this interface.
#define USBD_CUSTOM_CLASS0_IF1_EP5_EN                              0

//         <o.0..1>Type
//           <i>Select Endpoint Type.
//           <i>Endpoint Descriptor: bmAttributes field bits 0 .. 1.
//           <i>If required, for Isochronous Endpoint, Synchronization and Usage Type 
//           <i>can be set by manually editing define value of BMATTRIBUTES.
//           <2=>Bulk
//           <3=>Interrupt
//           <1=>Isochronous
#define USBD_CUSTOM_CLASS0_IF1_EP5_BMATTRIBUTES                    0x01

//         <o.0..3>Number
//           <i>Select Endpoint Number.
//           <i>Endpoint Descriptor: bEndpointAddress field bits 0 .. 3.
//                   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
//           <8=>8   <9=>9   <10=>10 <11=>11 <12=>12 <13=>13 <14=>14 <15=>15
//         <o.7>Direction
//           <i>Select Endpoint Direction.
//           <i>Endpoint Descriptor: bEndpointAddress field bit 7.
//           <0=>OUT
//           <1=>IN
#define USBD_CUSTOM_CLASS0_IF1_EP5_BENDPOINTADDRESS                0x83

//         <h>Speed Settings
//           <i>Settings that are different depending on device operating speed.
//
//           <h>Full/Low-speed
//             <i>Parameters apply when device operates in Full/Low-speed.
//
//             <o.0..9>Maximum Packet Size <0-1023>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 8, 16, 32 or 64.
//               <i>For Interrupt Endpoint set value to 1 .. 64.
//               <i>For Isochronous Endpoint set value to 1 .. 1023.
#define USBD_CUSTOM_CLASS0_IF1_EP5_FS_WMAXPACKETSIZE               1023

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in ms).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>Setting is not used for Bulk Endpoint (set value to 0).
//               <i>For Interrupt Endpoint set value to 1 .. 255 (polling interval).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF1_EP5_FS_BINTERVAL                    1

//           </h>

//           <h>High-speed
//             <i>Parameters apply when High-speed is enabled in corresponding USBD_Config_n.c
//             <i>(n is the index of device on which this interface will be used) and when
//             <i>device operates in High-speed.
//
//             <o.0..10>Maximum Packet Size <0-1024>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 512.
//               <i>For Interrupt Endpoint set value to 1 .. 1024.
//               <i>For Isochronous Endpoint set value to 1 .. 1024.
//             <o.11..12>Additional Transactions per Microframe
//               <i>Specifies additional transactions per microframe to improve communication performance.
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 11 .. 12.
//               <i>Relevant only if Endpoint Type is Isochronous or Interrupt.
//               <i>Value: None = 1 transaction per microframe
//               <i>Value: 1 additional = 2 transaction per microframe
//               <i>Value: 2 additional = 3 transaction per microframe
//                 <0=>None
//                 <1=>1 additional
//                 <2=>2 additional
#define USBD_CUSTOM_CLASS0_IF1_EP5_HS_WMAXPACKETSIZE               1024

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in 125 us units).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>For Bulk Endpoint this setting represents maximum NAK rate, set value to 0 .. 255.
//               <i>For Interrupt Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF1_EP5_HS_BINTERVAL                    1

//           </h>
//         </h>
//       </e>

//       <e>Endpoint
//       <i>Enable Endpoint for this interface.
#define USBD_CUSTOM_CLASS0_IF1_EP6_EN                              0

//         <o.0..1>Type
//           <i>Select Endpoint Type.
//           <i>Endpoint Descriptor: bmAttributes field bits 0 .. 1.
//           <i>If required, for Isochronous Endpoint, Synchronization and Usage Type 
//           <i>can be set by manually editing define value of BMATTRIBUTES.
//           <2=>Bulk
//           <3=>Interrupt
//           <1=>Isochronous
#define USBD_CUSTOM_CLASS0_IF1_EP6_BMATTRIBUTES                    0x02

//         <o.0..3>Number
//           <i>Select Endpoint Number.
//           <i>Endpoint Descriptor: bEndpointAddress field bits 0 .. 3.
//                   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
//           <8=>8   <9=>9   <10=>10 <11=>11 <12=>12 <13=>13 <14=>14 <15=>15
//         <o.7>Direction
//           <i>Select Endpoint Direction.
//           <i>Endpoint Descriptor: bEndpointAddress field bit 7.
//           <0=>OUT
//           <1=>IN
#define USBD_CUSTOM_CLASS0_IF1_EP6_BENDPOINTADDRESS                0x04

//         <h>Speed Settings
//           <i>Settings that are different depending on device operating speed.
//
//           <h>Full/Low-speed
//             <i>Parameters apply when device operates in Full/Low-speed.
//
//             <o.0..9>Maximum Packet Size <0-1023>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 8, 16, 32 or 64.
//               <i>For Interrupt Endpoint set value to 1 .. 64.
//               <i>For Isochronous Endpoint set value to 1 .. 1023.
#define USBD_CUSTOM_CLASS0_IF1_EP6_FS_WMAXPACKETSIZE               64

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in ms).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>Setting is not used for Bulk Endpoint (set value to 0).
//               <i>For Interrupt Endpoint set value to 1 .. 255 (polling interval).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF1_EP6_FS_BINTERVAL                    0

//           </h>

//           <h>High-speed
//             <i>Parameters apply when High-speed is enabled in corresponding USBD_Config_n.c
//             <i>(n is the index of device on which this interface will be used) and when
//             <i>device operates in High-speed.
//
//             <o.0..10>Maximum Packet Size <0-1024>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 512.
//               <i>For Interrupt Endpoint set value to 1 .. 1024.
//               <i>For Isochronous Endpoint set value to 1 .. 1024.
//             <o.11..12>Additional Transactions per Microframe
//               <i>Specifies additional transactions per microframe to improve communication performance.
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 11 .. 12.
//               <i>Relevant only if Endpoint Type is Isochronous or Interrupt.
//               <i>Value: None = 1 transaction per microframe
//               <i>Value: 1 additional = 2 transaction per microframe
//               <i>Value: 2 additional = 3 transaction per microframe
//                 <0=>None
//                 <1=>1 additional
//                 <2=>2 additional
#define USBD_CUSTOM_CLASS0_IF1_EP6_HS_WMAXPACKETSIZE               512

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in 125 us units).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>For Bulk Endpoint this setting represents maximum NAK rate, set value to 0 .. 255.
//               <i>For Interrupt Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF1_EP6_HS_BINTERVAL                    0

//           </h>
//         </h>
//       </e>

//       <e>Endpoint
//       <i>Enable Endpoint for this interface.
#define USBD_CUSTOM_CLASS0_IF1_EP7_EN                              0

//         <o.0..1>Type
//           <i>Select Endpoint Type.
//           <i>Endpoint Descriptor: bmAttributes field bits 0 .. 1.
//           <i>If required, for Isochronous Endpoint, Synchronization and Usage Type 
//           <i>can be set by manually editing define value of BMATTRIBUTES.
//           <2=>Bulk
//           <3=>Interrupt
//           <1=>Isochronous
#define USBD_CUSTOM_CLASS0_IF1_EP7_BMATTRIBUTES                    0x02

//         <o.0..3>Number
//           <i>Select Endpoint Number.
//           <i>Endpoint Descriptor: bEndpointAddress field bits 0 .. 3.
//                   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
//           <8=>8   <9=>9   <10=>10 <11=>11 <12=>12 <13=>13 <14=>14 <15=>15
//         <o.7>Direction
//           <i>Select Endpoint Direction.
//           <i>Endpoint Descriptor: bEndpointAddress field bit 7.
//           <0=>OUT
//           <1=>IN
#define USBD_CUSTOM_CLASS0_IF1_EP7_BENDPOINTADDRESS                0x84

//         <h>Speed Settings
//           <i>Settings that are different depending on device operating speed.
//
//           <h>Full/Low-speed
//             <i>Parameters apply when device operates in Full/Low-speed.
//
//             <o.0..9>Maximum Packet Size <0-1023>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 8, 16, 32 or 64.
//               <i>For Interrupt Endpoint set value to 1 .. 64.
//               <i>For Isochronous Endpoint set value to 1 .. 1023.
#define USBD_CUSTOM_CLASS0_IF1_EP7_FS_WMAXPACKETSIZE               64

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in ms).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>Setting is not used for Bulk Endpoint (set value to 0).
//               <i>For Interrupt Endpoint set value to 1 .. 255 (polling interval).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF1_EP7_FS_BINTERVAL                    0

//           </h>

//           <h>High-speed
//             <i>Parameters apply when High-speed is enabled in corresponding USBD_Config_n.c
//             <i>(n is the index of device on which this interface will be used) and when
//             <i>device operates in High-speed.
//
//             <o.0..10>Maximum Packet Size <0-1024>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 512.
//               <i>For Interrupt Endpoint set value to 1 .. 1024.
//               <i>For Isochronous Endpoint set value to 1 .. 1024.
//             <o.11..12>Additional Transactions per Microframe
//               <i>Specifies additional transactions per microframe to improve communication performance.
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 11 .. 12.
//               <i>Relevant only if Endpoint Type is Isochronous or Interrupt.
//               <i>Value: None = 1 transaction per microframe
//               <i>Value: 1 additional = 2 transaction per microframe
//               <i>Value: 2 additional = 3 transaction per microframe
//                 <0=>None
//                 <1=>1 additional
//                 <2=>2 additional
#define USBD_CUSTOM_CLASS0_IF1_EP7_HS_WMAXPACKETSIZE               512

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in 125 us units).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>For Bulk Endpoint this setting represents maximum NAK rate, set value to 0 .. 255.
//               <i>For Interrupt Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF1_EP7_HS_BINTERVAL                    0

//           </h>
//         </h>
//       </e>
//     </h>

//     <h>String Settings
//     <i>Following settings are used to create String Descriptor(s)

//       <e.0>Interface String Enable
//       <i>Enable Interface String.
//       <i>If disabled Interface String will not be assigned to USB Device Custom Class Interface 1.
#define USBD_CUSTOM_CLASS0_IF1_STR_EN                              0

//         <s.126>Interface String
#define USBD_CUSTOM_CLASS0_IF1_STR                                 L"USB_CUSTOM_CLASS0_IF1"

//       </e>
//     </h>

//     <h>Microsoft OS Descriptor Settings
//     <i>Following settings are used to create Extended Compat ID OS Feature Descriptor

//       <e.0>Extended Compat ID OS Feature Descriptor Function Section
//       <i>Enable creation of function section in Extended Compat ID OS Feature Descriptor for this interface.
#define USBD_CUSTOM_CLASS0_IF1_OS_EXT_COMPAT_ID_EN                 0

//         <s.7>compatibleID
//         <i>compatibleID field of function section in Extended Compat ID OS Feature Descriptor for this interface.
#define USBD_CUSTOM_CLASS0_IF1_OS_EXT_COMPAT_ID                    "WINUSB"

//         <s.7>subCompatibleID
//         <i>subCompatibleID field of function section in Extended Compat ID OS Feature Descriptor for this interface.
#define USBD_CUSTOM_CLASS0_IF1_OS_EXT_SUBCOMPAT_ID                 ""

//       </e>

//       <h>Extended Properties OS Feature Descriptor
//         <e.0>Custom Property Section 0
//         <i>Enable creation of custom property 0 section in Extended Properties OS Feature Descriptor.
#define USBD_CUSTOM_CLASS0_IF1_OS_EXT_PROP0_EN                     0

//           <o>Data Type
//           <i>Specifies the dwPropertyDataType field of custom property 0 section in Extended Properties OS Feature Descriptor.
//           <i>Values 3 (Free-form binary) and 7 (Multiple Unicode Strings) are not supported.
//             <1=>Unicode String (REG_SZ)
//             <2=>Unicode String with environment variables (REG_EXPAND_SZ)
//             <4=>Little-endian 32-bit integer (REG_DWORD_LITTLE_ENDIAN)
//             <5=>Big-endian 32-bit integer (REG_DWORD_BIG_ENDIAN)
//             <6=>Unicode String with symbolic link (REG_LINK)
#define USBD_CUSTOM_CLASS0_IF1_OS_EXT_PROP0_DATA_TYP               1

//           <s.512>Name
#define USBD_CUSTOM_CLASS0_IF1_OS_EXT_PROP0_NAME                   L"DeviceInterfaceGUID"

//           <h>Data
//             <s.1024>Unicode String
//             <i>Property Data in case Data Type is selected as Unicode String.
#define USBD_CUSTOM_CLASS0_IF1_OS_EXT_PROP0_DATA_STR               L"{7D9ADCFC-E570-4B38-BF4E-8F81F68964E0}"

//             <o>32-bit Integer
//             <i>Property Data in case Data Type is selected as Little/Big-endian 32-bit Integer.
#define USBD_CUSTOM_CLASS0_IF1_OS_EXT_PROP0_DATA_INT               0

//           </h>
//         </e>

//         <e.0>Custom Property Section 1
//         <i>Enable creation of custom property 1 section in Extended Properties OS Feature Descriptor.
#define USBD_CUSTOM_CLASS0_IF1_OS_EXT_PROP1_EN                     0

//           <o>Data Type
//           <i>Specifies the dwPropertyDataType field of custom property 1 section in Extended Properties OS Feature Descriptor.
//           <i>Values 3 (Free-form binary) and 7 (Multiple Unicode Strings) are not supported.
//             <1=>Unicode String (REG_SZ)
//             <2=>Unicode String with environment variables (REG_EXPAND_SZ)
//             <4=>Little-endian 32-bit integer (REG_DWORD_LITTLE_ENDIAN)
//             <5=>Big-endian 32-bit integer (REG_DWORD_BIG_ENDIAN)
//             <6=>Unicode String with symbolic link (REG_LINK)
#define USBD_CUSTOM_CLASS0_IF1_OS_EXT_PROP1_DATA_TYP               1

//           <s.512>Name
#define USBD_CUSTOM_CLASS0_IF1_OS_EXT_PROP1_NAME                   L""

//           <h>Data
//             <s.1024>Unicode String
//             <i>Property Data in case Data Type is selected as Unicode String.
#define USBD_CUSTOM_CLASS0_IF1_OS_EXT_PROP1_DATA_STR               L""

//             <o>32-bit Integer
//             <i>Property Data in case Data Type is selected as Little/Big-endian 32-bit Integer.
#define USBD_CUSTOM_CLASS0_IF1_OS_EXT_PROP1_DATA_INT               0

//           </h>
//         </e>

//         <e.0>Custom Property Section 2
//         <i>Enable creation of custom property 2 section in Extended Properties OS Feature Descriptor.
#define USBD_CUSTOM_CLASS0_IF1_OS_EXT_PROP2_EN                     0

//           <o>Data Type
//           <i>Specifies the dwPropertyDataType field of custom property 2 section in Extended Properties OS Feature Descriptor.
//           <i>Values 3 (Free-form binary) and 7 (Multiple Unicode Strings) are not supported.
//             <1=>Unicode String (REG_SZ)
//             <2=>Unicode String with environment variables (REG_EXPAND_SZ)
//             <4=>Little-endian 32-bit integer (REG_DWORD_LITTLE_ENDIAN)
//             <5=>Big-endian 32-bit integer (REG_DWORD_BIG_ENDIAN)
//             <6=>Unicode String with symbolic link (REG_LINK)
#define USBD_CUSTOM_CLASS0_IF1_OS_EXT_PROP2_DATA_TYP               1

//           <s.512>Name
#define USBD_CUSTOM_CLASS0_IF1_OS_EXT_PROP2_NAME                   L""

//           <h>Data
//             <s.1024>Unicode String
//             <i>Property Data in case Data Type is selected as Unicode String.
#define USBD_CUSTOM_CLASS0_IF1_OS_EXT_PROP2_DATA_STR               L""

//             <o>32-bit Integer
//             <i>Property Data in case Data Type is selected as Little/Big-endian 32-bit Integer.
#define USBD_CUSTOM_CLASS0_IF1_OS_EXT_PROP2_DATA_INT               0

//           </h>
//         </e>

//         <e.0>Custom Property Section 3
//         <i>Enable creation of custom property 3 section in Extended Properties OS Feature Descriptor.
#define USBD_CUSTOM_CLASS0_IF1_OS_EXT_PROP3_EN                     0

//           <o>Data Type
//           <i>Specifies the dwPropertyDataType field of custom property 3 section in Extended Properties OS Feature Descriptor.
//           <i>Values 3 (Free-form binary) and 7 (Multiple Unicode Strings) are not supported.
//             <1=>Unicode String (REG_SZ)
//             <2=>Unicode String with environment variables (REG_EXPAND_SZ)
//             <4=>Little-endian 32-bit integer (REG_DWORD_LITTLE_ENDIAN)
//             <5=>Big-endian 32-bit integer (REG_DWORD_BIG_ENDIAN)
//             <6=>Unicode String with symbolic link (REG_LINK)
#define USBD_CUSTOM_CLASS0_IF1_OS_EXT_PROP3_DATA_TYP               1

//           <s.512>Name
#define USBD_CUSTOM_CLASS0_IF1_OS_EXT_PROP3_NAME                   L""

//           <h>Data
//             <s.1024>Unicode String
//             <i>Property Data in case Data Type is selected as Unicode String.
#define USBD_CUSTOM_CLASS0_IF1_OS_EXT_PROP3_DATA_STR               L""

//             <o>32-bit Integer
//             <i>Property Data in case Data Type is selected as Little/Big-endian 32-bit Integer.
#define USBD_CUSTOM_CLASS0_IF1_OS_EXT_PROP3_DATA_INT               0

//           </h>
//         </e>
//       </h>
//     </h>
//   </e>


//   <e>Interface
#define USBD_CUSTOM_CLASS0_IF2_EN                                  0

//     <h>Interface Settings
//     <i>The Interface Settings are used to create the Interface Descriptor.
//     <i>Refer to USB - USB Concepts - USB Descriptor in the MDK Components
//     <i>User's Guide for more information about the Interface Descriptor.

//       <o>Interface Number <0-255>
//       <i>Defines the value for bInterfaceNumber
//       <i>Each USB Device Interface has a sequential Interface Number starting with 0.
//       <i>Several Interfaces may have the same Interface Number; in this case the value
//       <i>of Alternate Setting is used to differ between the Interfaces. For a
//       <i>composite device the Interface Numbers of the custom classes must be contiguous.
#define USBD_CUSTOM_CLASS0_IF2_NUM                                 2

//       <o>Alternate Setting <0=>0 <1=>1 <2=>2 <3=>3
//       <i>Defines the value for bAlternateSetting
//       <i>A sequential number starting with 0 to identify the Interface Descriptors
//       <i>that share the same value for Interface Number.
#define USBD_CUSTOM_CLASS0_IF2_ALT                                 0

//       <o.0..7>Class Code
//         <i>Class Codes are defined by USB-IF. For more information refer to
//         <i>http://www.usb.org/developers/defined_class.
//         <0x00=>0x00: Indicate a Null Class Code triple
//         <0x01=>0x01: Audio
//         <0x02=>0x02: Communications and CDC Control
//         <0x03=>0x03: HID (Human Interface Device)
//         <0x05=>0x05: Physical
//         <0x06=>0x06: Image
//         <0x07=>0x07: Printer
//         <0x08=>0x08: Mass Storage
//         <0x0A=>0x0A: CDC-Data
//         <0x0B=>0x0B: Smart Card
//         <0x0D=>0x0D: Content Security
//         <0x0E=>0x0E: Video
//         <0x0F=>0x0F: Personal Healthcare
//         <0x10=>0x10: Audio/Video Devices
//         <0xDC=>0xDC: Diagnostic Device
//         <0xE0=>0xE0: Wireless Controller
//         <0xEF=>0xEF: Miscellaneous
//         <0xFE=>0xFE: Application Specific
//         <0xFF=>0xFF: Vendor Specific
#define USBD_CUSTOM_CLASS0_IF2_CLASS                               0xFF

//       <o.0..7>Subclass Code <0x00-0xFF>
//       <i>The possible values depend on the Class Code:
//       <i>Class Code 0x00: Subclass Code must be 0
//       <i>Class Code 0x01 .. 0xFE: Subclass Code is defined by USB-IF
//       <i>Class Code 0xFF: Subclass Code can be 0x00 .. 0xFF
#define USBD_CUSTOM_CLASS0_IF2_SUBCLASS                            0x00

//       <o.0..7>Protocol Code <0x00-0xFF>
//       <i>The Protocol Code value defines the protocol used on this interface:
//       <i>Protocol Code 0x00: class-specific protocol not used
//       <i>Protocol Code 0x01 .. 0xFE: class-specific protocol used
//       <i>Protocol Code 0xFF: vendor-specific protocol used
#define USBD_CUSTOM_CLASS0_IF2_PROTOCOL                            0x00

//     </h>

//     <h>Endpoint Settings
//     <i>Following settings are used to create the Endpoint Descriptors.
//     <i>Refer to USB - USB Concepts - USB Descriptor in the MDK Components
//     <i>User's Guide for more information about Endpoint Descriptors.

//       <e>Endpoint
//       <i>Enable Endpoint for this interface.
#define USBD_CUSTOM_CLASS0_IF2_EP0_EN                              0

//         <o.0..1>Type
//           <i>Select Endpoint Type.
//           <i>Endpoint Descriptor: bmAttributes field bits 0 .. 1.
//           <i>If required, for Isochronous Endpoint, Synchronization and Usage Type 
//           <i>can be set by manually editing define value of BMATTRIBUTES.
//           <2=>Bulk
//           <3=>Interrupt
//           <1=>Isochronous
#define USBD_CUSTOM_CLASS0_IF2_EP0_BMATTRIBUTES                    0x02

//         <o.0..3>Number
//           <i>Select Endpoint Number.
//           <i>Endpoint Descriptor: bEndpointAddress field bits 0 .. 3.
//                   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
//           <8=>8   <9=>9   <10=>10 <11=>11 <12=>12 <13=>13 <14=>14 <15=>15
//         <o.7>Direction
//           <i>Select Endpoint Direction.
//           <i>Endpoint Descriptor: bEndpointAddress field bit 7.
//           <0=>OUT
//           <1=>IN
#define USBD_CUSTOM_CLASS0_IF2_EP0_BENDPOINTADDRESS                0x01

//         <h>Speed Settings
//           <i>Settings that are different depending on device operating speed.
//
//           <h>Full/Low-speed
//             <i>Parameters apply when device operates in Full/Low-speed.
//
//             <o.0..9>Maximum Packet Size <0-1023>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 8, 16, 32 or 64.
//               <i>For Interrupt Endpoint set value to 1 .. 64.
//               <i>For Isochronous Endpoint set value to 1 .. 1023.
#define USBD_CUSTOM_CLASS0_IF2_EP0_FS_WMAXPACKETSIZE               64

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in ms).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>Setting is not used for Bulk Endpoint (set value to 0).
//               <i>For Interrupt Endpoint set value to 1 .. 255 (polling interval).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF2_EP0_FS_BINTERVAL                    0

//           </h>

//           <h>High-speed
//             <i>Parameters apply when High-speed is enabled in corresponding USBD_Config_n.c
//             <i>(n is the index of device on which this interface will be used) and when
//             <i>device operates in High-speed.
//
//             <o.0..10>Maximum Packet Size <0-1024>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 512.
//               <i>For Interrupt Endpoint set value to 1 .. 1024.
//               <i>For Isochronous Endpoint set value to 1 .. 1024.
//             <o.11..12>Additional Transactions per Microframe
//               <i>Specifies additional transactions per microframe to improve communication performance.
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 11 .. 12.
//               <i>Relevant only if Endpoint Type is Isochronous or Interrupt.
//               <i>Value: None = 1 transaction per microframe
//               <i>Value: 1 additional = 2 transaction per microframe
//               <i>Value: 2 additional = 3 transaction per microframe
//                 <0=>None
//                 <1=>1 additional
//                 <2=>2 additional
#define USBD_CUSTOM_CLASS0_IF2_EP0_HS_WMAXPACKETSIZE               512

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in 125 us units).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>For Bulk Endpoint this setting represents maximum NAK rate, set value to 0 .. 255.
//               <i>For Interrupt Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF2_EP0_HS_BINTERVAL                    0

//           </h>
//         </h>
//       </e>

//       <e>Endpoint
//       <i>Enable Endpoint for this interface.
#define USBD_CUSTOM_CLASS0_IF2_EP1_EN                              0

//         <o.0..1>Type
//           <i>Select Endpoint Type.
//           <i>Endpoint Descriptor: bmAttributes field bits 0 .. 1.
//           <i>If required, for Isochronous Endpoint, Synchronization and Usage Type 
//           <i>can be set by manually editing define value of BMATTRIBUTES.
//           <2=>Bulk
//           <3=>Interrupt
//           <1=>Isochronous
#define USBD_CUSTOM_CLASS0_IF2_EP1_BMATTRIBUTES                    0x02

//         <o.0..3>Number
//           <i>Select Endpoint Number.
//           <i>Endpoint Descriptor: bEndpointAddress field bits 0 .. 3.
//                   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
//           <8=>8   <9=>9   <10=>10 <11=>11 <12=>12 <13=>13 <14=>14 <15=>15
//         <o.7>Direction
//           <i>Select Endpoint Direction.
//           <i>Endpoint Descriptor: bEndpointAddress field bit 7.
//           <0=>OUT
//           <1=>IN
#define USBD_CUSTOM_CLASS0_IF2_EP1_BENDPOINTADDRESS                0x81

//         <h>Speed Settings
//           <i>Settings that are different depending on device operating speed.
//
//           <h>Full/Low-speed
//             <i>Parameters apply when device operates in Full/Low-speed.
//
//             <o.0..9>Maximum Packet Size <0-1023>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 8, 16, 32 or 64.
//               <i>For Interrupt Endpoint set value to 1 .. 64.
//               <i>For Isochronous Endpoint set value to 1 .. 1023.
#define USBD_CUSTOM_CLASS0_IF2_EP1_FS_WMAXPACKETSIZE               64

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in ms).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>Setting is not used for Bulk Endpoint (set value to 0).
//               <i>For Interrupt Endpoint set value to 1 .. 255 (polling interval).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF2_EP1_FS_BINTERVAL                    0

//           </h>

//           <h>High-speed
//             <i>Parameters apply when High-speed is enabled in corresponding USBD_Config_n.c
//             <i>(n is the index of device on which this interface will be used) and when
//             <i>device operates in High-speed.
//
//             <o.0..10>Maximum Packet Size <0-1024>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 512.
//               <i>For Interrupt Endpoint set value to 1 .. 1024.
//               <i>For Isochronous Endpoint set value to 1 .. 1024.
//             <o.11..12>Additional Transactions per Microframe
//               <i>Specifies additional transactions per microframe to improve communication performance.
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 11 .. 12.
//               <i>Relevant only if Endpoint Type is Isochronous or Interrupt.
//               <i>Value: None = 1 transaction per microframe
//               <i>Value: 1 additional = 2 transaction per microframe
//               <i>Value: 2 additional = 3 transaction per microframe
//                 <0=>None
//                 <1=>1 additional
//                 <2=>2 additional
#define USBD_CUSTOM_CLASS0_IF2_EP1_HS_WMAXPACKETSIZE               512

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in 125 us units).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>For Bulk Endpoint this setting represents maximum NAK rate, set value to 0 .. 255.
//               <i>For Interrupt Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF2_EP1_HS_BINTERVAL                    0

//           </h>
//         </h>
//       </e>

//       <e>Endpoint
//       <i>Enable Endpoint for this interface.
#define USBD_CUSTOM_CLASS0_IF2_EP2_EN                              0

//         <o.0..1>Type
//           <i>Select Endpoint Type.
//           <i>Endpoint Descriptor: bmAttributes field bits 0 .. 1.
//           <i>If required, for Isochronous Endpoint, Synchronization and Usage Type 
//           <i>can be set by manually editing define value of BMATTRIBUTES.
//           <2=>Bulk
//           <3=>Interrupt
//           <1=>Isochronous
#define USBD_CUSTOM_CLASS0_IF2_EP2_BMATTRIBUTES                    0x03

//         <o.0..3>Number
//           <i>Select Endpoint Number.
//           <i>Endpoint Descriptor: bEndpointAddress field bits 0 .. 3.
//                   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
//           <8=>8   <9=>9   <10=>10 <11=>11 <12=>12 <13=>13 <14=>14 <15=>15
//         <o.7>Direction
//           <i>Select Endpoint Direction.
//           <i>Endpoint Descriptor: bEndpointAddress field bit 7.
//           <0=>OUT
//           <1=>IN
#define USBD_CUSTOM_CLASS0_IF2_EP2_BENDPOINTADDRESS                0x02

//         <h>Speed Settings
//           <i>Settings that are different depending on device operating speed.
//
//           <h>Full/Low-speed
//             <i>Parameters apply when device operates in Full/Low-speed.
//
//             <o.0..9>Maximum Packet Size <0-1023>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 8, 16, 32 or 64.
//               <i>For Interrupt Endpoint set value to 1 .. 64.
//               <i>For Isochronous Endpoint set value to 1 .. 1023.
#define USBD_CUSTOM_CLASS0_IF2_EP2_FS_WMAXPACKETSIZE               64

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in ms).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>Setting is not used for Bulk Endpoint (set value to 0).
//               <i>For Interrupt Endpoint set value to 1 .. 255 (polling interval).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF2_EP2_FS_BINTERVAL                    1

//           </h>

//           <h>High-speed
//             <i>Parameters apply when High-speed is enabled in corresponding USBD_Config_n.c
//             <i>(n is the index of device on which this interface will be used) and when
//             <i>device operates in High-speed.
//
//             <o.0..10>Maximum Packet Size <0-1024>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 512.
//               <i>For Interrupt Endpoint set value to 1 .. 1024.
//               <i>For Isochronous Endpoint set value to 1 .. 1024.
//             <o.11..12>Additional Transactions per Microframe
//               <i>Specifies additional transactions per microframe to improve communication performance.
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 11 .. 12.
//               <i>Relevant only if Endpoint Type is Isochronous or Interrupt.
//               <i>Value: None = 1 transaction per microframe
//               <i>Value: 1 additional = 2 transaction per microframe
//               <i>Value: 2 additional = 3 transaction per microframe
//                 <0=>None
//                 <1=>1 additional
//                 <2=>2 additional
#define USBD_CUSTOM_CLASS0_IF2_EP2_HS_WMAXPACKETSIZE               1024

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in 125 us units).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>For Bulk Endpoint this setting represents maximum NAK rate, set value to 0 .. 255.
//               <i>For Interrupt Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF2_EP2_HS_BINTERVAL                    1

//           </h>
//         </h>
//       </e>

//       <e>Endpoint
//       <i>Enable Endpoint for this interface.
#define USBD_CUSTOM_CLASS0_IF2_EP3_EN                              0

//         <o.0..1>Type
//           <i>Select Endpoint Type.
//           <i>Endpoint Descriptor: bmAttributes field bits 0 .. 1.
//           <i>If required, for Isochronous Endpoint, Synchronization and Usage Type 
//           <i>can be set by manually editing define value of BMATTRIBUTES.
//           <2=>Bulk
//           <3=>Interrupt
//           <1=>Isochronous
#define USBD_CUSTOM_CLASS0_IF2_EP3_BMATTRIBUTES                    0x03

//         <o.0..3>Number
//           <i>Select Endpoint Number.
//           <i>Endpoint Descriptor: bEndpointAddress field bits 0 .. 3.
//                   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
//           <8=>8   <9=>9   <10=>10 <11=>11 <12=>12 <13=>13 <14=>14 <15=>15
//         <o.7>Direction
//           <i>Select Endpoint Direction.
//           <i>Endpoint Descriptor: bEndpointAddress field bit 7.
//           <0=>OUT
//           <1=>IN
#define USBD_CUSTOM_CLASS0_IF2_EP3_BENDPOINTADDRESS                0x82

//         <h>Speed Settings
//           <i>Settings that are different depending on device operating speed.
//
//           <h>Full/Low-speed
//             <i>Parameters apply when device operates in Full/Low-speed.
//
//             <o.0..9>Maximum Packet Size <0-1023>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 8, 16, 32 or 64.
//               <i>For Interrupt Endpoint set value to 1 .. 64.
//               <i>For Isochronous Endpoint set value to 1 .. 1023.
#define USBD_CUSTOM_CLASS0_IF2_EP3_FS_WMAXPACKETSIZE               64

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in ms).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>Setting is not used for Bulk Endpoint (set value to 0).
//               <i>For Interrupt Endpoint set value to 1 .. 255 (polling interval).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF2_EP3_FS_BINTERVAL                    1

//           </h>

//           <h>High-speed
//             <i>Parameters apply when High-speed is enabled in corresponding USBD_Config_n.c
//             <i>(n is the index of device on which this interface will be used) and when
//             <i>device operates in High-speed.
//
//             <o.0..10>Maximum Packet Size <0-1024>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 512.
//               <i>For Interrupt Endpoint set value to 1 .. 1024.
//               <i>For Isochronous Endpoint set value to 1 .. 1024.
//             <o.11..12>Additional Transactions per Microframe
//               <i>Specifies additional transactions per microframe to improve communication performance.
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 11 .. 12.
//               <i>Relevant only if Endpoint Type is Isochronous or Interrupt.
//               <i>Value: None = 1 transaction per microframe
//               <i>Value: 1 additional = 2 transaction per microframe
//               <i>Value: 2 additional = 3 transaction per microframe
//                 <0=>None
//                 <1=>1 additional
//                 <2=>2 additional
#define USBD_CUSTOM_CLASS0_IF2_EP3_HS_WMAXPACKETSIZE               1024

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in 125 us units).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>For Bulk Endpoint this setting represents maximum NAK rate, set value to 0 .. 255.
//               <i>For Interrupt Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF2_EP3_HS_BINTERVAL                    1

//           </h>
//         </h>
//       </e>

//       <e>Endpoint
//       <i>Enable Endpoint for this interface.
#define USBD_CUSTOM_CLASS0_IF2_EP4_EN                              0

//         <o.0..1>Type
//           <i>Select Endpoint Type.
//           <i>Endpoint Descriptor: bmAttributes field bits 0 .. 1.
//           <i>If required, for Isochronous Endpoint, Synchronization and Usage Type 
//           <i>can be set by manually editing define value of BMATTRIBUTES.
//           <2=>Bulk
//           <3=>Interrupt
//           <1=>Isochronous
#define USBD_CUSTOM_CLASS0_IF2_EP4_BMATTRIBUTES                    0x01

//         <o.0..3>Number
//           <i>Select Endpoint Number.
//           <i>Endpoint Descriptor: bEndpointAddress field bits 0 .. 3.
//                   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
//           <8=>8   <9=>9   <10=>10 <11=>11 <12=>12 <13=>13 <14=>14 <15=>15
//         <o.7>Direction
//           <i>Select Endpoint Direction.
//           <i>Endpoint Descriptor: bEndpointAddress field bit 7.
//           <0=>OUT
//           <1=>IN
#define USBD_CUSTOM_CLASS0_IF2_EP4_BENDPOINTADDRESS                0x03

//         <h>Speed Settings
//           <i>Settings that are different depending on device operating speed.
//
//           <h>Full/Low-speed
//             <i>Parameters apply when device operates in Full/Low-speed.
//
//             <o.0..9>Maximum Packet Size <0-1023>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 8, 16, 32 or 64.
//               <i>For Interrupt Endpoint set value to 1 .. 64.
//               <i>For Isochronous Endpoint set value to 1 .. 1023.
#define USBD_CUSTOM_CLASS0_IF2_EP4_FS_WMAXPACKETSIZE               1023

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in ms).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>Setting is not used for Bulk Endpoint (set value to 0).
//               <i>For Interrupt Endpoint set value to 1 .. 255 (polling interval).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF2_EP4_FS_BINTERVAL                    1

//           </h>

//           <h>High-speed
//             <i>Parameters apply when High-speed is enabled in corresponding USBD_Config_n.c
//             <i>(n is the index of device on which this interface will be used) and when
//             <i>device operates in High-speed.
//
//             <o.0..10>Maximum Packet Size <0-1024>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 512.
//               <i>For Interrupt Endpoint set value to 1 .. 1024.
//               <i>For Isochronous Endpoint set value to 1 .. 1024.
//             <o.11..12>Additional Transactions per Microframe
//               <i>Specifies additional transactions per microframe to improve communication performance.
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 11 .. 12.
//               <i>Relevant only if Endpoint Type is Isochronous or Interrupt.
//               <i>Value: None = 1 transaction per microframe
//               <i>Value: 1 additional = 2 transaction per microframe
//               <i>Value: 2 additional = 3 transaction per microframe
//                 <0=>None
//                 <1=>1 additional
//                 <2=>2 additional
#define USBD_CUSTOM_CLASS0_IF2_EP4_HS_WMAXPACKETSIZE               1024

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in 125 us units).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>For Bulk Endpoint this setting represents maximum NAK rate, set value to 0 .. 255.
//               <i>For Interrupt Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF2_EP4_HS_BINTERVAL                    1

//           </h>
//         </h>
//       </e>

//       <e>Endpoint
//       <i>Enable Endpoint for this interface.
#define USBD_CUSTOM_CLASS0_IF2_EP5_EN                              0

//         <o.0..1>Type
//           <i>Select Endpoint Type.
//           <i>Endpoint Descriptor: bmAttributes field bits 0 .. 1.
//           <i>If required, for Isochronous Endpoint, Synchronization and Usage Type 
//           <i>can be set by manually editing define value of BMATTRIBUTES.
//           <2=>Bulk
//           <3=>Interrupt
//           <1=>Isochronous
#define USBD_CUSTOM_CLASS0_IF2_EP5_BMATTRIBUTES                    0x01

//         <o.0..3>Number
//           <i>Select Endpoint Number.
//           <i>Endpoint Descriptor: bEndpointAddress field bits 0 .. 3.
//                   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
//           <8=>8   <9=>9   <10=>10 <11=>11 <12=>12 <13=>13 <14=>14 <15=>15
//         <o.7>Direction
//           <i>Select Endpoint Direction.
//           <i>Endpoint Descriptor: bEndpointAddress field bit 7.
//           <0=>OUT
//           <1=>IN
#define USBD_CUSTOM_CLASS0_IF2_EP5_BENDPOINTADDRESS                0x83

//         <h>Speed Settings
//           <i>Settings that are different depending on device operating speed.
//
//           <h>Full/Low-speed
//             <i>Parameters apply when device operates in Full/Low-speed.
//
//             <o.0..9>Maximum Packet Size <0-1023>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 8, 16, 32 or 64.
//               <i>For Interrupt Endpoint set value to 1 .. 64.
//               <i>For Isochronous Endpoint set value to 1 .. 1023.
#define USBD_CUSTOM_CLASS0_IF2_EP5_FS_WMAXPACKETSIZE               1023

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in ms).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>Setting is not used for Bulk Endpoint (set value to 0).
//               <i>For Interrupt Endpoint set value to 1 .. 255 (polling interval).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF2_EP5_FS_BINTERVAL                    1

//           </h>

//           <h>High-speed
//             <i>Parameters apply when High-speed is enabled in corresponding USBD_Config_n.c
//             <i>(n is the index of device on which this interface will be used) and when
//             <i>device operates in High-speed.
//
//             <o.0..10>Maximum Packet Size <0-1024>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 512.
//               <i>For Interrupt Endpoint set value to 1 .. 1024.
//               <i>For Isochronous Endpoint set value to 1 .. 1024.
//             <o.11..12>Additional Transactions per Microframe
//               <i>Specifies additional transactions per microframe to improve communication performance.
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 11 .. 12.
//               <i>Relevant only if Endpoint Type is Isochronous or Interrupt.
//               <i>Value: None = 1 transaction per microframe
//               <i>Value: 1 additional = 2 transaction per microframe
//               <i>Value: 2 additional = 3 transaction per microframe
//                 <0=>None
//                 <1=>1 additional
//                 <2=>2 additional
#define USBD_CUSTOM_CLASS0_IF2_EP5_HS_WMAXPACKETSIZE               1024

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in 125 us units).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>For Bulk Endpoint this setting represents maximum NAK rate, set value to 0 .. 255.
//               <i>For Interrupt Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF2_EP5_HS_BINTERVAL                    1

//           </h>
//         </h>
//       </e>

//       <e>Endpoint
//       <i>Enable Endpoint for this interface.
#define USBD_CUSTOM_CLASS0_IF2_EP6_EN                              0

//         <o.0..1>Type
//           <i>Select Endpoint Type.
//           <i>Endpoint Descriptor: bmAttributes field bits 0 .. 1.
//           <i>If required, for Isochronous Endpoint, Synchronization and Usage Type 
//           <i>can be set by manually editing define value of BMATTRIBUTES.
//           <2=>Bulk
//           <3=>Interrupt
//           <1=>Isochronous
#define USBD_CUSTOM_CLASS0_IF2_EP6_BMATTRIBUTES                    0x02

//         <o.0..3>Number
//           <i>Select Endpoint Number.
//           <i>Endpoint Descriptor: bEndpointAddress field bits 0 .. 3.
//                   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
//           <8=>8   <9=>9   <10=>10 <11=>11 <12=>12 <13=>13 <14=>14 <15=>15
//         <o.7>Direction
//           <i>Select Endpoint Direction.
//           <i>Endpoint Descriptor: bEndpointAddress field bit 7.
//           <0=>OUT
//           <1=>IN
#define USBD_CUSTOM_CLASS0_IF2_EP6_BENDPOINTADDRESS                0x04

//         <h>Speed Settings
//           <i>Settings that are different depending on device operating speed.
//
//           <h>Full/Low-speed
//             <i>Parameters apply when device operates in Full/Low-speed.
//
//             <o.0..9>Maximum Packet Size <0-1023>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 8, 16, 32 or 64.
//               <i>For Interrupt Endpoint set value to 1 .. 64.
//               <i>For Isochronous Endpoint set value to 1 .. 1023.
#define USBD_CUSTOM_CLASS0_IF2_EP6_FS_WMAXPACKETSIZE               64

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in ms).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>Setting is not used for Bulk Endpoint (set value to 0).
//               <i>For Interrupt Endpoint set value to 1 .. 255 (polling interval).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF2_EP6_FS_BINTERVAL                    0

//           </h>

//           <h>High-speed
//             <i>Parameters apply when High-speed is enabled in corresponding USBD_Config_n.c
//             <i>(n is the index of device on which this interface will be used) and when
//             <i>device operates in High-speed.
//
//             <o.0..10>Maximum Packet Size <0-1024>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 512.
//               <i>For Interrupt Endpoint set value to 1 .. 1024.
//               <i>For Isochronous Endpoint set value to 1 .. 1024.
//             <o.11..12>Additional Transactions per Microframe
//               <i>Specifies additional transactions per microframe to improve communication performance.
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 11 .. 12.
//               <i>Relevant only if Endpoint Type is Isochronous or Interrupt.
//               <i>Value: None = 1 transaction per microframe
//               <i>Value: 1 additional = 2 transaction per microframe
//               <i>Value: 2 additional = 3 transaction per microframe
//                 <0=>None
//                 <1=>1 additional
//                 <2=>2 additional
#define USBD_CUSTOM_CLASS0_IF2_EP6_HS_WMAXPACKETSIZE               512

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in 125 us units).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>For Bulk Endpoint this setting represents maximum NAK rate, set value to 0 .. 255.
//               <i>For Interrupt Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF2_EP6_HS_BINTERVAL                    0

//           </h>
//         </h>
//       </e>

//       <e>Endpoint
//       <i>Enable Endpoint for this interface.
#define USBD_CUSTOM_CLASS0_IF2_EP7_EN                              0

//         <o.0..1>Type
//           <i>Select Endpoint Type.
//           <i>Endpoint Descriptor: bmAttributes field bits 0 .. 1.
//           <i>If required, for Isochronous Endpoint, Synchronization and Usage Type 
//           <i>can be set by manually editing define value of BMATTRIBUTES.
//           <2=>Bulk
//           <3=>Interrupt
//           <1=>Isochronous
#define USBD_CUSTOM_CLASS0_IF2_EP7_BMATTRIBUTES                    0x02

//         <o.0..3>Number
//           <i>Select Endpoint Number.
//           <i>Endpoint Descriptor: bEndpointAddress field bits 0 .. 3.
//                   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
//           <8=>8   <9=>9   <10=>10 <11=>11 <12=>12 <13=>13 <14=>14 <15=>15
//         <o.7>Direction
//           <i>Select Endpoint Direction.
//           <i>Endpoint Descriptor: bEndpointAddress field bit 7.
//           <0=>OUT
//           <1=>IN
#define USBD_CUSTOM_CLASS0_IF2_EP7_BENDPOINTADDRESS                0x84

//         <h>Speed Settings
//           <i>Settings that are different depending on device operating speed.
//
//           <h>Full/Low-speed
//             <i>Parameters apply when device operates in Full/Low-speed.
//
//             <o.0..9>Maximum Packet Size <0-1023>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 8, 16, 32 or 64.
//               <i>For Interrupt Endpoint set value to 1 .. 64.
//               <i>For Isochronous Endpoint set value to 1 .. 1023.
#define USBD_CUSTOM_CLASS0_IF2_EP7_FS_WMAXPACKETSIZE               64

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in ms).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>Setting is not used for Bulk Endpoint (set value to 0).
//               <i>For Interrupt Endpoint set value to 1 .. 255 (polling interval).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF2_EP7_FS_BINTERVAL                    0

//           </h>

//           <h>High-speed
//             <i>Parameters apply when High-speed is enabled in corresponding USBD_Config_n.c
//             <i>(n is the index of device on which this interface will be used) and when
//             <i>device operates in High-speed.
//
//             <o.0..10>Maximum Packet Size <0-1024>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 512.
//               <i>For Interrupt Endpoint set value to 1 .. 1024.
//               <i>For Isochronous Endpoint set value to 1 .. 1024.
//             <o.11..12>Additional Transactions per Microframe
//               <i>Specifies additional transactions per microframe to improve communication performance.
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 11 .. 12.
//               <i>Relevant only if Endpoint Type is Isochronous or Interrupt.
//               <i>Value: None = 1 transaction per microframe
//               <i>Value: 1 additional = 2 transaction per microframe
//               <i>Value: 2 additional = 3 transaction per microframe
//                 <0=>None
//                 <1=>1 additional
//                 <2=>2 additional
#define USBD_CUSTOM_CLASS0_IF2_EP7_HS_WMAXPACKETSIZE               512

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in 125 us units).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>For Bulk Endpoint this setting represents maximum NAK rate, set value to 0 .. 255.
//               <i>For Interrupt Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF2_EP7_HS_BINTERVAL                    0

//           </h>
//         </h>
//       </e>
//     </h>

//     <h>String Settings
//     <i>Following settings are used to create String Descriptor(s)

//       <e.0>Interface String Enable
//       <i>Enable Interface String.
//       <i>If disabled Interface String will not be assigned to USB Device Custom Class Interface 2.
#define USBD_CUSTOM_CLASS0_IF2_STR_EN                              0

//         <s.126>Interface String
#define USBD_CUSTOM_CLASS0_IF2_STR                                 L"USB_CUSTOM_CLASS0_IF2"

//       </e>
//     </h>

//     <h>Microsoft OS Descriptor Settings
//     <i>Following settings are used to create Extended Compat ID OS Feature Descriptor

//       <e.0>Extended Compat ID OS Feature Descriptor Function Section
//       <i>Enable creation of function section in Extended Compat ID OS Feature Descriptor for this interface.
#define USBD_CUSTOM_CLASS0_IF2_OS_EXT_COMPAT_ID_EN                 0

//         <s.7>compatibleID
//         <i>compatibleID field of function section in Extended Compat ID OS Feature Descriptor for this interface.
#define USBD_CUSTOM_CLASS0_IF2_OS_EXT_COMPAT_ID                    "WINUSB"

//         <s.7>subCompatibleID
//         <i>subCompatibleID field of function section in Extended Compat ID OS Feature Descriptor for this interface.
#define USBD_CUSTOM_CLASS0_IF2_OS_EXT_SUBCOMPAT_ID                 ""

//       </e>

//       <h>Extended Properties OS Feature Descriptor
//         <e.0>Custom Property Section 0
//         <i>Enable creation of custom property 0 section in Extended Properties OS Feature Descriptor.
#define USBD_CUSTOM_CLASS0_IF2_OS_EXT_PROP0_EN                     0

//           <o>Data Type
//           <i>Specifies the dwPropertyDataType field of custom property 0 section in Extended Properties OS Feature Descriptor.
//           <i>Values 3 (Free-form binary) and 7 (Multiple Unicode Strings) are not supported.
//             <1=>Unicode String (REG_SZ)
//             <2=>Unicode String with environment variables (REG_EXPAND_SZ)
//             <4=>Little-endian 32-bit integer (REG_DWORD_LITTLE_ENDIAN)
//             <5=>Big-endian 32-bit integer (REG_DWORD_BIG_ENDIAN)
//             <6=>Unicode String with symbolic link (REG_LINK)
#define USBD_CUSTOM_CLASS0_IF2_OS_EXT_PROP0_DATA_TYP               1

//           <s.512>Name
#define USBD_CUSTOM_CLASS0_IF2_OS_EXT_PROP0_NAME                   L"DeviceInterfaceGUID"

//           <h>Data
//             <s.1024>Unicode String
//             <i>Property Data in case Data Type is selected as Unicode String.
#define USBD_CUSTOM_CLASS0_IF2_OS_EXT_PROP0_DATA_STR               L"{7D9ADCFC-E570-4B38-BF4E-8F81F68964E0}"

//             <o>32-bit Integer
//             <i>Property Data in case Data Type is selected as Little/Big-endian 32-bit Integer.
#define USBD_CUSTOM_CLASS0_IF2_OS_EXT_PROP0_DATA_INT               0

//           </h>
//         </e>

//         <e.0>Custom Property Section 1
//         <i>Enable creation of custom property 1 section in Extended Properties OS Feature Descriptor.
#define USBD_CUSTOM_CLASS0_IF2_OS_EXT_PROP1_EN                     0

//           <o>Data Type
//           <i>Specifies the dwPropertyDataType field of custom property 1 section in Extended Properties OS Feature Descriptor.
//           <i>Values 3 (Free-form binary) and 7 (Multiple Unicode Strings) are not supported.
//             <1=>Unicode String (REG_SZ)
//             <2=>Unicode String with environment variables (REG_EXPAND_SZ)
//             <4=>Little-endian 32-bit integer (REG_DWORD_LITTLE_ENDIAN)
//             <5=>Big-endian 32-bit integer (REG_DWORD_BIG_ENDIAN)
//             <6=>Unicode String with symbolic link (REG_LINK)
#define USBD_CUSTOM_CLASS0_IF2_OS_EXT_PROP1_DATA_TYP               1

//           <s.512>Name
#define USBD_CUSTOM_CLASS0_IF2_OS_EXT_PROP1_NAME                   L""

//           <h>Data
//             <s.1024>Unicode String
//             <i>Property Data in case Data Type is selected as Unicode String.
#define USBD_CUSTOM_CLASS0_IF2_OS_EXT_PROP1_DATA_STR               L""

//             <o>32-bit Integer
//             <i>Property Data in case Data Type is selected as Little/Big-endian 32-bit Integer.
#define USBD_CUSTOM_CLASS0_IF2_OS_EXT_PROP1_DATA_INT               0

//           </h>
//         </e>

//         <e.0>Custom Property Section 2
//         <i>Enable creation of custom property 2 section in Extended Properties OS Feature Descriptor.
#define USBD_CUSTOM_CLASS0_IF2_OS_EXT_PROP2_EN                     0

//           <o>Data Type
//           <i>Specifies the dwPropertyDataType field of custom property 2 section in Extended Properties OS Feature Descriptor.
//           <i>Values 3 (Free-form binary) and 7 (Multiple Unicode Strings) are not supported.
//             <1=>Unicode String (REG_SZ)
//             <2=>Unicode String with environment variables (REG_EXPAND_SZ)
//             <4=>Little-endian 32-bit integer (REG_DWORD_LITTLE_ENDIAN)
//             <5=>Big-endian 32-bit integer (REG_DWORD_BIG_ENDIAN)
//             <6=>Unicode String with symbolic link (REG_LINK)
#define USBD_CUSTOM_CLASS0_IF2_OS_EXT_PROP2_DATA_TYP               1

//           <s.512>Name
#define USBD_CUSTOM_CLASS0_IF2_OS_EXT_PROP2_NAME                   L""

//           <h>Data
//             <s.1024>Unicode String
//             <i>Property Data in case Data Type is selected as Unicode String.
#define USBD_CUSTOM_CLASS0_IF2_OS_EXT_PROP2_DATA_STR               L""

//             <o>32-bit Integer
//             <i>Property Data in case Data Type is selected as Little/Big-endian 32-bit Integer.
#define USBD_CUSTOM_CLASS0_IF2_OS_EXT_PROP2_DATA_INT               0

//           </h>
//         </e>

//         <e.0>Custom Property Section 3
//         <i>Enable creation of custom property 3 section in Extended Properties OS Feature Descriptor.
#define USBD_CUSTOM_CLASS0_IF2_OS_EXT_PROP3_EN                     0

//           <o>Data Type
//           <i>Specifies the dwPropertyDataType field of custom property 3 section in Extended Properties OS Feature Descriptor.
//           <i>Values 3 (Free-form binary) and 7 (Multiple Unicode Strings) are not supported.
//             <1=>Unicode String (REG_SZ)
//             <2=>Unicode String with environment variables (REG_EXPAND_SZ)
//             <4=>Little-endian 32-bit integer (REG_DWORD_LITTLE_ENDIAN)
//             <5=>Big-endian 32-bit integer (REG_DWORD_BIG_ENDIAN)
//             <6=>Unicode String with symbolic link (REG_LINK)
#define USBD_CUSTOM_CLASS0_IF2_OS_EXT_PROP3_DATA_TYP               1

//           <s.512>Name
#define USBD_CUSTOM_CLASS0_IF2_OS_EXT_PROP3_NAME                   L""

//           <h>Data
//             <s.1024>Unicode String
//             <i>Property Data in case Data Type is selected as Unicode String.
#define USBD_CUSTOM_CLASS0_IF2_OS_EXT_PROP3_DATA_STR               L""

//             <o>32-bit Integer
//             <i>Property Data in case Data Type is selected as Little/Big-endian 32-bit Integer.
#define USBD_CUSTOM_CLASS0_IF2_OS_EXT_PROP3_DATA_INT               0

//           </h>
//         </e>
//       </h>
//     </h>
//   </e>


//   <e>Interface
#define USBD_CUSTOM_CLASS0_IF3_EN                                  0

//     <h>Interface Settings
//     <i>The Interface Settings are used to create the Interface Descriptor.
//     <i>Refer to USB - USB Concepts - USB Descriptor in the MDK Components
//     <i>User's Guide for more information about the Interface Descriptor.

//       <o>Interface Number <0-255>
//       <i>Defines the value for bInterfaceNumber
//       <i>Each USB Device Interface has a sequential Interface Number starting with 0.
//       <i>Several Interfaces may have the same Interface Number; in this case the value
//       <i>of Alternate Setting is used to differ between the Interfaces. For a
//       <i>composite device the Interface Numbers of the custom classes must be contiguous.
#define USBD_CUSTOM_CLASS0_IF3_NUM                                 3

//       <o>Alternate Setting <0=>0 <1=>1 <2=>2 <3=>3
//       <i>Defines the value for bAlternateSetting
//       <i>A sequential number starting with 0 to identify the Interface Descriptors
//       <i>that share the same value for Interface Number.
#define USBD_CUSTOM_CLASS0_IF3_ALT                                 0

//       <o.0..7>Class Code
//         <i>Class Codes are defined by USB-IF. For more information refer to
//         <i>http://www.usb.org/developers/defined_class.
//         <0x00=>0x00: Indicate a Null Class Code triple
//         <0x01=>0x01: Audio
//         <0x02=>0x02: Communications and CDC Control
//         <0x03=>0x03: HID (Human Interface Device)
//         <0x05=>0x05: Physical
//         <0x06=>0x06: Image
//         <0x07=>0x07: Printer
//         <0x08=>0x08: Mass Storage
//         <0x0A=>0x0A: CDC-Data
//         <0x0B=>0x0B: Smart Card
//         <0x0D=>0x0D: Content Security
//         <0x0E=>0x0E: Video
//         <0x0F=>0x0F: Personal Healthcare
//         <0x10=>0x10: Audio/Video Devices
//         <0xDC=>0xDC: Diagnostic Device
//         <0xE0=>0xE0: Wireless Controller
//         <0xEF=>0xEF: Miscellaneous
//         <0xFE=>0xFE: Application Specific
//         <0xFF=>0xFF: Vendor Specific
#define USBD_CUSTOM_CLASS0_IF3_CLASS                               0xFF

//       <o.0..7>Subclass Code <0x00-0xFF>
//       <i>The possible values depend on the Class Code:
//       <i>Class Code 0x00: Subclass Code must be 0
//       <i>Class Code 0x01 .. 0xFE: Subclass Code is defined by USB-IF
//       <i>Class Code 0xFF: Subclass Code can be 0x00 .. 0xFF
#define USBD_CUSTOM_CLASS0_IF3_SUBCLASS                            0x00

//       <o.0..7>Protocol Code <0x00-0xFF>
//       <i>The Protocol Code value defines the protocol used on this interface:
//       <i>Protocol Code 0x00: class-specific protocol not used
//       <i>Protocol Code 0x01 .. 0xFE: class-specific protocol used
//       <i>Protocol Code 0xFF: vendor-specific protocol used
#define USBD_CUSTOM_CLASS0_IF3_PROTOCOL                            0x00

//     </h>

//     <h>Endpoint Settings
//     <i>Following settings are used to create the Endpoint Descriptors.
//     <i>Refer to USB - USB Concepts - USB Descriptor in the MDK Components
//     <i>User's Guide for more information about Endpoint Descriptors.

//       <e>Endpoint
//       <i>Enable Endpoint for this interface.
#define USBD_CUSTOM_CLASS0_IF3_EP0_EN                              0

//         <o.0..1>Type
//           <i>Select Endpoint Type.
//           <i>Endpoint Descriptor: bmAttributes field bits 0 .. 1.
//           <i>If required, for Isochronous Endpoint, Synchronization and Usage Type 
//           <i>can be set by manually editing define value of BMATTRIBUTES.
//           <2=>Bulk
//           <3=>Interrupt
//           <1=>Isochronous
#define USBD_CUSTOM_CLASS0_IF3_EP0_BMATTRIBUTES                    0x02

//         <o.0..3>Number
//           <i>Select Endpoint Number.
//           <i>Endpoint Descriptor: bEndpointAddress field bits 0 .. 3.
//                   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
//           <8=>8   <9=>9   <10=>10 <11=>11 <12=>12 <13=>13 <14=>14 <15=>15
//         <o.7>Direction
//           <i>Select Endpoint Direction.
//           <i>Endpoint Descriptor: bEndpointAddress field bit 7.
//           <0=>OUT
//           <1=>IN
#define USBD_CUSTOM_CLASS0_IF3_EP0_BENDPOINTADDRESS                0x01

//         <h>Speed Settings
//           <i>Settings that are different depending on device operating speed.
//
//           <h>Full/Low-speed
//             <i>Parameters apply when device operates in Full/Low-speed.
//
//             <o.0..9>Maximum Packet Size <0-1023>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 8, 16, 32 or 64.
//               <i>For Interrupt Endpoint set value to 1 .. 64.
//               <i>For Isochronous Endpoint set value to 1 .. 1023.
#define USBD_CUSTOM_CLASS0_IF3_EP0_FS_WMAXPACKETSIZE               64

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in ms).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>Setting is not used for Bulk Endpoint (set value to 0).
//               <i>For Interrupt Endpoint set value to 1 .. 255 (polling interval).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF3_EP0_FS_BINTERVAL                    0

//           </h>

//           <h>High-speed
//             <i>Parameters apply when High-speed is enabled in corresponding USBD_Config_n.c
//             <i>(n is the index of device on which this interface will be used) and when
//             <i>device operates in High-speed.
//
//             <o.0..10>Maximum Packet Size <0-1024>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 512.
//               <i>For Interrupt Endpoint set value to 1 .. 1024.
//               <i>For Isochronous Endpoint set value to 1 .. 1024.
//             <o.11..12>Additional Transactions per Microframe
//               <i>Specifies additional transactions per microframe to improve communication performance.
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 11 .. 12.
//               <i>Relevant only if Endpoint Type is Isochronous or Interrupt.
//               <i>Value: None = 1 transaction per microframe
//               <i>Value: 1 additional = 2 transaction per microframe
//               <i>Value: 2 additional = 3 transaction per microframe
//                 <0=>None
//                 <1=>1 additional
//                 <2=>2 additional
#define USBD_CUSTOM_CLASS0_IF3_EP0_HS_WMAXPACKETSIZE               512

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in 125 us units).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>For Bulk Endpoint this setting represents maximum NAK rate, set value to 0 .. 255.
//               <i>For Interrupt Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF3_EP0_HS_BINTERVAL                    0

//           </h>
//         </h>
//       </e>

//       <e>Endpoint
//       <i>Enable Endpoint for this interface.
#define USBD_CUSTOM_CLASS0_IF3_EP1_EN                              0

//         <o.0..1>Type
//           <i>Select Endpoint Type.
//           <i>Endpoint Descriptor: bmAttributes field bits 0 .. 1.
//           <i>If required, for Isochronous Endpoint, Synchronization and Usage Type 
//           <i>can be set by manually editing define value of BMATTRIBUTES.
//           <2=>Bulk
//           <3=>Interrupt
//           <1=>Isochronous
#define USBD_CUSTOM_CLASS0_IF3_EP1_BMATTRIBUTES                    0x02

//         <o.0..3>Number
//           <i>Select Endpoint Number.
//           <i>Endpoint Descriptor: bEndpointAddress field bits 0 .. 3.
//                   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
//           <8=>8   <9=>9   <10=>10 <11=>11 <12=>12 <13=>13 <14=>14 <15=>15
//         <o.7>Direction
//           <i>Select Endpoint Direction.
//           <i>Endpoint Descriptor: bEndpointAddress field bit 7.
//           <0=>OUT
//           <1=>IN
#define USBD_CUSTOM_CLASS0_IF3_EP1_BENDPOINTADDRESS                0x81

//         <h>Speed Settings
//           <i>Settings that are different depending on device operating speed.
//
//           <h>Full/Low-speed
//             <i>Parameters apply when device operates in Full/Low-speed.
//
//             <o.0..9>Maximum Packet Size <0-1023>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 8, 16, 32 or 64.
//               <i>For Interrupt Endpoint set value to 1 .. 64.
//               <i>For Isochronous Endpoint set value to 1 .. 1023.
#define USBD_CUSTOM_CLASS0_IF3_EP1_FS_WMAXPACKETSIZE               64

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in ms).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>Setting is not used for Bulk Endpoint (set value to 0).
//               <i>For Interrupt Endpoint set value to 1 .. 255 (polling interval).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF3_EP1_FS_BINTERVAL                    0

//           </h>

//           <h>High-speed
//             <i>Parameters apply when High-speed is enabled in corresponding USBD_Config_n.c
//             <i>(n is the index of device on which this interface will be used) and when
//             <i>device operates in High-speed.
//
//             <o.0..10>Maximum Packet Size <0-1024>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 512.
//               <i>For Interrupt Endpoint set value to 1 .. 1024.
//               <i>For Isochronous Endpoint set value to 1 .. 1024.
//             <o.11..12>Additional Transactions per Microframe
//               <i>Specifies additional transactions per microframe to improve communication performance.
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 11 .. 12.
//               <i>Relevant only if Endpoint Type is Isochronous or Interrupt.
//               <i>Value: None = 1 transaction per microframe
//               <i>Value: 1 additional = 2 transaction per microframe
//               <i>Value: 2 additional = 3 transaction per microframe
//                 <0=>None
//                 <1=>1 additional
//                 <2=>2 additional
#define USBD_CUSTOM_CLASS0_IF3_EP1_HS_WMAXPACKETSIZE               512

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in 125 us units).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>For Bulk Endpoint this setting represents maximum NAK rate, set value to 0 .. 255.
//               <i>For Interrupt Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF3_EP1_HS_BINTERVAL                    0

//           </h>
//         </h>
//       </e>

//       <e>Endpoint
//       <i>Enable Endpoint for this interface.
#define USBD_CUSTOM_CLASS0_IF3_EP2_EN                              0

//         <o.0..1>Type
//           <i>Select Endpoint Type.
//           <i>Endpoint Descriptor: bmAttributes field bits 0 .. 1.
//           <i>If required, for Isochronous Endpoint, Synchronization and Usage Type 
//           <i>can be set by manually editing define value of BMATTRIBUTES.
//           <2=>Bulk
//           <3=>Interrupt
//           <1=>Isochronous
#define USBD_CUSTOM_CLASS0_IF3_EP2_BMATTRIBUTES                    0x03

//         <o.0..3>Number
//           <i>Select Endpoint Number.
//           <i>Endpoint Descriptor: bEndpointAddress field bits 0 .. 3.
//                   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
//           <8=>8   <9=>9   <10=>10 <11=>11 <12=>12 <13=>13 <14=>14 <15=>15
//         <o.7>Direction
//           <i>Select Endpoint Direction.
//           <i>Endpoint Descriptor: bEndpointAddress field bit 7.
//           <0=>OUT
//           <1=>IN
#define USBD_CUSTOM_CLASS0_IF3_EP2_BENDPOINTADDRESS                0x02

//         <h>Speed Settings
//           <i>Settings that are different depending on device operating speed.
//
//           <h>Full/Low-speed
//             <i>Parameters apply when device operates in Full/Low-speed.
//
//             <o.0..9>Maximum Packet Size <0-1023>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 8, 16, 32 or 64.
//               <i>For Interrupt Endpoint set value to 1 .. 64.
//               <i>For Isochronous Endpoint set value to 1 .. 1023.
#define USBD_CUSTOM_CLASS0_IF3_EP2_FS_WMAXPACKETSIZE               64

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in ms).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>Setting is not used for Bulk Endpoint (set value to 0).
//               <i>For Interrupt Endpoint set value to 1 .. 255 (polling interval).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF3_EP2_FS_BINTERVAL                    1

//           </h>

//           <h>High-speed
//             <i>Parameters apply when High-speed is enabled in corresponding USBD_Config_n.c
//             <i>(n is the index of device on which this interface will be used) and when
//             <i>device operates in High-speed.
//
//             <o.0..10>Maximum Packet Size <0-1024>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 512.
//               <i>For Interrupt Endpoint set value to 1 .. 1024.
//               <i>For Isochronous Endpoint set value to 1 .. 1024.
//             <o.11..12>Additional Transactions per Microframe
//               <i>Specifies additional transactions per microframe to improve communication performance.
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 11 .. 12.
//               <i>Relevant only if Endpoint Type is Isochronous or Interrupt.
//               <i>Value: None = 1 transaction per microframe
//               <i>Value: 1 additional = 2 transaction per microframe
//               <i>Value: 2 additional = 3 transaction per microframe
//                 <0=>None
//                 <1=>1 additional
//                 <2=>2 additional
#define USBD_CUSTOM_CLASS0_IF3_EP2_HS_WMAXPACKETSIZE               1024

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in 125 us units).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>For Bulk Endpoint this setting represents maximum NAK rate, set value to 0 .. 255.
//               <i>For Interrupt Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF3_EP2_HS_BINTERVAL                    1

//           </h>
//         </h>
//       </e>

//       <e>Endpoint
//       <i>Enable Endpoint for this interface.
#define USBD_CUSTOM_CLASS0_IF3_EP3_EN                              0

//         <o.0..1>Type
//           <i>Select Endpoint Type.
//           <i>Endpoint Descriptor: bmAttributes field bits 0 .. 1.
//           <i>If required, for Isochronous Endpoint, Synchronization and Usage Type 
//           <i>can be set by manually editing define value of BMATTRIBUTES.
//           <2=>Bulk
//           <3=>Interrupt
//           <1=>Isochronous
#define USBD_CUSTOM_CLASS0_IF3_EP3_BMATTRIBUTES                    0x03

//         <o.0..3>Number
//           <i>Select Endpoint Number.
//           <i>Endpoint Descriptor: bEndpointAddress field bits 0 .. 3.
//                   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
//           <8=>8   <9=>9   <10=>10 <11=>11 <12=>12 <13=>13 <14=>14 <15=>15
//         <o.7>Direction
//           <i>Select Endpoint Direction.
//           <i>Endpoint Descriptor: bEndpointAddress field bit 7.
//           <0=>OUT
//           <1=>IN
#define USBD_CUSTOM_CLASS0_IF3_EP3_BENDPOINTADDRESS                0x82

//         <h>Speed Settings
//           <i>Settings that are different depending on device operating speed.
//
//           <h>Full/Low-speed
//             <i>Parameters apply when device operates in Full/Low-speed.
//
//             <o.0..9>Maximum Packet Size <0-1023>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 8, 16, 32 or 64.
//               <i>For Interrupt Endpoint set value to 1 .. 64.
//               <i>For Isochronous Endpoint set value to 1 .. 1023.
#define USBD_CUSTOM_CLASS0_IF3_EP3_FS_WMAXPACKETSIZE               64

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in ms).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>Setting is not used for Bulk Endpoint (set value to 0).
//               <i>For Interrupt Endpoint set value to 1 .. 255 (polling interval).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF3_EP3_FS_BINTERVAL                    1

//           </h>

//           <h>High-speed
//             <i>Parameters apply when High-speed is enabled in corresponding USBD_Config_n.c
//             <i>(n is the index of device on which this interface will be used) and when
//             <i>device operates in High-speed.
//
//             <o.0..10>Maximum Packet Size <0-1024>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 512.
//               <i>For Interrupt Endpoint set value to 1 .. 1024.
//               <i>For Isochronous Endpoint set value to 1 .. 1024.
//             <o.11..12>Additional Transactions per Microframe
//               <i>Specifies additional transactions per microframe to improve communication performance.
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 11 .. 12.
//               <i>Relevant only if Endpoint Type is Isochronous or Interrupt.
//               <i>Value: None = 1 transaction per microframe
//               <i>Value: 1 additional = 2 transaction per microframe
//               <i>Value: 2 additional = 3 transaction per microframe
//                 <0=>None
//                 <1=>1 additional
//                 <2=>2 additional
#define USBD_CUSTOM_CLASS0_IF3_EP3_HS_WMAXPACKETSIZE               1024

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in 125 us units).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>For Bulk Endpoint this setting represents maximum NAK rate, set value to 0 .. 255.
//               <i>For Interrupt Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF3_EP3_HS_BINTERVAL                    1

//           </h>
//         </h>
//       </e>

//       <e>Endpoint
//       <i>Enable Endpoint for this interface.
#define USBD_CUSTOM_CLASS0_IF3_EP4_EN                              0

//         <o.0..1>Type
//           <i>Select Endpoint Type.
//           <i>Endpoint Descriptor: bmAttributes field bits 0 .. 1.
//           <i>If required, for Isochronous Endpoint, Synchronization and Usage Type 
//           <i>can be set by manually editing define value of BMATTRIBUTES.
//           <2=>Bulk
//           <3=>Interrupt
//           <1=>Isochronous
#define USBD_CUSTOM_CLASS0_IF3_EP4_BMATTRIBUTES                    0x01

//         <o.0..3>Number
//           <i>Select Endpoint Number.
//           <i>Endpoint Descriptor: bEndpointAddress field bits 0 .. 3.
//                   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
//           <8=>8   <9=>9   <10=>10 <11=>11 <12=>12 <13=>13 <14=>14 <15=>15
//         <o.7>Direction
//           <i>Select Endpoint Direction.
//           <i>Endpoint Descriptor: bEndpointAddress field bit 7.
//           <0=>OUT
//           <1=>IN
#define USBD_CUSTOM_CLASS0_IF3_EP4_BENDPOINTADDRESS                0x03

//         <h>Speed Settings
//           <i>Settings that are different depending on device operating speed.
//
//           <h>Full/Low-speed
//             <i>Parameters apply when device operates in Full/Low-speed.
//
//             <o.0..9>Maximum Packet Size <0-1023>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 8, 16, 32 or 64.
//               <i>For Interrupt Endpoint set value to 1 .. 64.
//               <i>For Isochronous Endpoint set value to 1 .. 1023.
#define USBD_CUSTOM_CLASS0_IF3_EP4_FS_WMAXPACKETSIZE               1023

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in ms).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>Setting is not used for Bulk Endpoint (set value to 0).
//               <i>For Interrupt Endpoint set value to 1 .. 255 (polling interval).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF3_EP4_FS_BINTERVAL                    1

//           </h>

//           <h>High-speed
//             <i>Parameters apply when High-speed is enabled in corresponding USBD_Config_n.c
//             <i>(n is the index of device on which this interface will be used) and when
//             <i>device operates in High-speed.
//
//             <o.0..10>Maximum Packet Size <0-1024>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 512.
//               <i>For Interrupt Endpoint set value to 1 .. 1024.
//               <i>For Isochronous Endpoint set value to 1 .. 1024.
//             <o.11..12>Additional Transactions per Microframe
//               <i>Specifies additional transactions per microframe to improve communication performance.
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 11 .. 12.
//               <i>Relevant only if Endpoint Type is Isochronous or Interrupt.
//               <i>Value: None = 1 transaction per microframe
//               <i>Value: 1 additional = 2 transaction per microframe
//               <i>Value: 2 additional = 3 transaction per microframe
//                 <0=>None
//                 <1=>1 additional
//                 <2=>2 additional
#define USBD_CUSTOM_CLASS0_IF3_EP4_HS_WMAXPACKETSIZE               1024

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in 125 us units).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>For Bulk Endpoint this setting represents maximum NAK rate, set value to 0 .. 255.
//               <i>For Interrupt Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF3_EP4_HS_BINTERVAL                    1

//           </h>
//         </h>
//       </e>

//       <e>Endpoint
//       <i>Enable Endpoint for this interface.
#define USBD_CUSTOM_CLASS0_IF3_EP5_EN                              0

//         <o.0..1>Type
//           <i>Select Endpoint Type.
//           <i>Endpoint Descriptor: bmAttributes field bits 0 .. 1.
//           <i>If required, for Isochronous Endpoint, Synchronization and Usage Type 
//           <i>can be set by manually editing define value of BMATTRIBUTES.
//           <2=>Bulk
//           <3=>Interrupt
//           <1=>Isochronous
#define USBD_CUSTOM_CLASS0_IF3_EP5_BMATTRIBUTES                    0x01

//         <o.0..3>Number
//           <i>Select Endpoint Number.
//           <i>Endpoint Descriptor: bEndpointAddress field bits 0 .. 3.
//                   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
//           <8=>8   <9=>9   <10=>10 <11=>11 <12=>12 <13=>13 <14=>14 <15=>15
//         <o.7>Direction
//           <i>Select Endpoint Direction.
//           <i>Endpoint Descriptor: bEndpointAddress field bit 7.
//           <0=>OUT
//           <1=>IN
#define USBD_CUSTOM_CLASS0_IF3_EP5_BENDPOINTADDRESS                0x83

//         <h>Speed Settings
//           <i>Settings that are different depending on device operating speed.
//
//           <h>Full/Low-speed
//             <i>Parameters apply when device operates in Full/Low-speed.
//
//             <o.0..9>Maximum Packet Size <0-1023>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 8, 16, 32 or 64.
//               <i>For Interrupt Endpoint set value to 1 .. 64.
//               <i>For Isochronous Endpoint set value to 1 .. 1023.
#define USBD_CUSTOM_CLASS0_IF3_EP5_FS_WMAXPACKETSIZE               1023

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in ms).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>Setting is not used for Bulk Endpoint (set value to 0).
//               <i>For Interrupt Endpoint set value to 1 .. 255 (polling interval).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF3_EP5_FS_BINTERVAL                    1

//           </h>

//           <h>High-speed
//             <i>Parameters apply when High-speed is enabled in corresponding USBD_Config_n.c
//             <i>(n is the index of device on which this interface will be used) and when
//             <i>device operates in High-speed.
//
//             <o.0..10>Maximum Packet Size <0-1024>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 512.
//               <i>For Interrupt Endpoint set value to 1 .. 1024.
//               <i>For Isochronous Endpoint set value to 1 .. 1024.
//             <o.11..12>Additional Transactions per Microframe
//               <i>Specifies additional transactions per microframe to improve communication performance.
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 11 .. 12.
//               <i>Relevant only if Endpoint Type is Isochronous or Interrupt.
//               <i>Value: None = 1 transaction per microframe
//               <i>Value: 1 additional = 2 transaction per microframe
//               <i>Value: 2 additional = 3 transaction per microframe
//                 <0=>None
//                 <1=>1 additional
//                 <2=>2 additional
#define USBD_CUSTOM_CLASS0_IF3_EP5_HS_WMAXPACKETSIZE               1024

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in 125 us units).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>For Bulk Endpoint this setting represents maximum NAK rate, set value to 0 .. 255.
//               <i>For Interrupt Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF3_EP5_HS_BINTERVAL                    1

//           </h>
//         </h>
//       </e>

//       <e>Endpoint
//       <i>Enable Endpoint for this interface.
#define USBD_CUSTOM_CLASS0_IF3_EP6_EN                              0

//         <o.0..1>Type
//           <i>Select Endpoint Type.
//           <i>Endpoint Descriptor: bmAttributes field bits 0 .. 1.
//           <i>If required, for Isochronous Endpoint, Synchronization and Usage Type 
//           <i>can be set by manually editing define value of BMATTRIBUTES.
//           <2=>Bulk
//           <3=>Interrupt
//           <1=>Isochronous
#define USBD_CUSTOM_CLASS0_IF3_EP6_BMATTRIBUTES                    0x02

//         <o.0..3>Number
//           <i>Select Endpoint Number.
//           <i>Endpoint Descriptor: bEndpointAddress field bits 0 .. 3.
//                   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
//           <8=>8   <9=>9   <10=>10 <11=>11 <12=>12 <13=>13 <14=>14 <15=>15
//         <o.7>Direction
//           <i>Select Endpoint Direction.
//           <i>Endpoint Descriptor: bEndpointAddress field bit 7.
//           <0=>OUT
//           <1=>IN
#define USBD_CUSTOM_CLASS0_IF3_EP6_BENDPOINTADDRESS                0x04

//         <h>Speed Settings
//           <i>Settings that are different depending on device operating speed.
//
//           <h>Full/Low-speed
//             <i>Parameters apply when device operates in Full/Low-speed.
//
//             <o.0..9>Maximum Packet Size <0-1023>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 8, 16, 32 or 64.
//               <i>For Interrupt Endpoint set value to 1 .. 64.
//               <i>For Isochronous Endpoint set value to 1 .. 1023.
#define USBD_CUSTOM_CLASS0_IF3_EP6_FS_WMAXPACKETSIZE               64

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in ms).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>Setting is not used for Bulk Endpoint (set value to 0).
//               <i>For Interrupt Endpoint set value to 1 .. 255 (polling interval).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF3_EP6_FS_BINTERVAL                    0

//           </h>

//           <h>High-speed
//             <i>Parameters apply when High-speed is enabled in corresponding USBD_Config_n.c
//             <i>(n is the index of device on which this interface will be used) and when
//             <i>device operates in High-speed.
//
//             <o.0..10>Maximum Packet Size <0-1024>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 512.
//               <i>For Interrupt Endpoint set value to 1 .. 1024.
//               <i>For Isochronous Endpoint set value to 1 .. 1024.
//             <o.11..12>Additional Transactions per Microframe
//               <i>Specifies additional transactions per microframe to improve communication performance.
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 11 .. 12.
//               <i>Relevant only if Endpoint Type is Isochronous or Interrupt.
//               <i>Value: None = 1 transaction per microframe
//               <i>Value: 1 additional = 2 transaction per microframe
//               <i>Value: 2 additional = 3 transaction per microframe
//                 <0=>None
//                 <1=>1 additional
//                 <2=>2 additional
#define USBD_CUSTOM_CLASS0_IF3_EP6_HS_WMAXPACKETSIZE               512

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in 125 us units).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>For Bulk Endpoint this setting represents maximum NAK rate, set value to 0 .. 255.
//               <i>For Interrupt Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF3_EP6_HS_BINTERVAL                    0

//           </h>
//         </h>
//       </e>

//       <e>Endpoint
//       <i>Enable Endpoint for this interface.
#define USBD_CUSTOM_CLASS0_IF3_EP7_EN                              0

//         <o.0..1>Type
//           <i>Select Endpoint Type.
//           <i>Endpoint Descriptor: bmAttributes field bits 0 .. 1.
//           <i>If required, for Isochronous Endpoint, Synchronization and Usage Type 
//           <i>can be set by manually editing define value of BMATTRIBUTES.
//           <2=>Bulk
//           <3=>Interrupt
//           <1=>Isochronous
#define USBD_CUSTOM_CLASS0_IF3_EP7_BMATTRIBUTES                    0x02

//         <o.0..3>Number
//           <i>Select Endpoint Number.
//           <i>Endpoint Descriptor: bEndpointAddress field bits 0 .. 3.
//                   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
//           <8=>8   <9=>9   <10=>10 <11=>11 <12=>12 <13=>13 <14=>14 <15=>15
//         <o.7>Direction
//           <i>Select Endpoint Direction.
//           <i>Endpoint Descriptor: bEndpointAddress field bit 7.
//           <0=>OUT
//           <1=>IN
#define USBD_CUSTOM_CLASS0_IF3_EP7_BENDPOINTADDRESS                0x84

//         <h>Speed Settings
//           <i>Settings that are different depending on device operating speed.
//
//           <h>Full/Low-speed
//             <i>Parameters apply when device operates in Full/Low-speed.
//
//             <o.0..9>Maximum Packet Size <0-1023>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 8, 16, 32 or 64.
//               <i>For Interrupt Endpoint set value to 1 .. 64.
//               <i>For Isochronous Endpoint set value to 1 .. 1023.
#define USBD_CUSTOM_CLASS0_IF3_EP7_FS_WMAXPACKETSIZE               64

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in ms).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>Setting is not used for Bulk Endpoint (set value to 0).
//               <i>For Interrupt Endpoint set value to 1 .. 255 (polling interval).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF3_EP7_FS_BINTERVAL                    0

//           </h>

//           <h>High-speed
//             <i>Parameters apply when High-speed is enabled in corresponding USBD_Config_n.c
//             <i>(n is the index of device on which this interface will be used) and when
//             <i>device operates in High-speed.
//
//             <o.0..10>Maximum Packet Size <0-1024>
//               <i>Specifies the physical packet size used for information exchange (in bytes).
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 0 .. 10.
//               <i>For Bulk Endpoint set value to 512.
//               <i>For Interrupt Endpoint set value to 1 .. 1024.
//               <i>For Isochronous Endpoint set value to 1 .. 1024.
//             <o.11..12>Additional Transactions per Microframe
//               <i>Specifies additional transactions per microframe to improve communication performance.
//               <i>Endpoint Descriptor: wMaxPacketSize field bits 11 .. 12.
//               <i>Relevant only if Endpoint Type is Isochronous or Interrupt.
//               <i>Value: None = 1 transaction per microframe
//               <i>Value: 1 additional = 2 transaction per microframe
//               <i>Value: 2 additional = 3 transaction per microframe
//                 <0=>None
//                 <1=>1 additional
//                 <2=>2 additional
#define USBD_CUSTOM_CLASS0_IF3_EP7_HS_WMAXPACKETSIZE               512

//             <o.0..7>Endpoint Polling Interval <0-255>
//               <i>Specifies the frequency of requests initiated by USB Host (in 125 us units).
//               <i>Endpoint Descriptor: bInterval field.
//               <i>For Bulk Endpoint this setting represents maximum NAK rate, set value to 0 .. 255.
//               <i>For Interrupt Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
//               <i>For Isochronous Endpoint set value to 1 .. 16 (polling interval is 2^(value-1)).
#define USBD_CUSTOM_CLASS0_IF3_EP7_HS_BINTERVAL                    0

//           </h>
//         </h>
//       </e>
//     </h>

//     <h>String Settings
//     <i>Following settings are used to create String Descriptor(s)

//       <e.0>Interface String Enable
//       <i>Enable Interface String.
//       <i>If disabled Interface String will not be assigned to USB Device Custom Class Interface 3.
#define USBD_CUSTOM_CLASS0_IF3_STR_EN                              0

//         <s.126>Interface String
#define USBD_CUSTOM_CLASS0_IF3_STR                                 L"USB_CUSTOM_CLASS0_IF3"

//       </e>
//     </h>

//     <h>Microsoft OS Descriptor Settings
//     <i>Following settings are used to create Extended Compat ID OS Feature Descriptor

//       <e.0>Extended Compat ID OS Feature Descriptor Function Section
//       <i>Enable creation of function section in Extended Compat ID OS Feature Descriptor for this interface.
#define USBD_CUSTOM_CLASS0_IF3_OS_EXT_COMPAT_ID_EN                 0

//         <s.7>compatibleID
//         <i>compatibleID field of function section in Extended Compat ID OS Feature Descriptor for this interface.
#define USBD_CUSTOM_CLASS0_IF3_OS_EXT_COMPAT_ID                    "WINUSB"

//         <s.7>subCompatibleID
//         <i>subCompatibleID field of function section in Extended Compat ID OS Feature Descriptor for this interface.
#define USBD_CUSTOM_CLASS0_IF3_OS_EXT_SUBCOMPAT_ID                 ""

//       </e>

//       <h>Extended Properties OS Feature Descriptor
//         <e.0>Custom Property Section 0
//         <i>Enable creation of custom property 0 section in Extended Properties OS Feature Descriptor.
#define USBD_CUSTOM_CLASS0_IF3_OS_EXT_PROP0_EN                     0

//           <o>Data Type
//           <i>Specifies the dwPropertyDataType field of custom property 0 section in Extended Properties OS Feature Descriptor.
//           <i>Values 3 (Free-form binary) and 7 (Multiple Unicode Strings) are not supported.
//             <1=>Unicode String (REG_SZ)
//             <2=>Unicode String with environment variables (REG_EXPAND_SZ)
//             <4=>Little-endian 32-bit integer (REG_DWORD_LITTLE_ENDIAN)
//             <5=>Big-endian 32-bit integer (REG_DWORD_BIG_ENDIAN)
//             <6=>Unicode String with symbolic link (REG_LINK)
#define USBD_CUSTOM_CLASS0_IF3_OS_EXT_PROP0_DATA_TYP               1

//           <s.512>Name
#define USBD_CUSTOM_CLASS0_IF3_OS_EXT_PROP0_NAME                   L"DeviceInterfaceGUID"

//           <h>Data
//             <s.1024>Unicode String
//             <i>Property Data in case Data Type is selected as Unicode String.
#define USBD_CUSTOM_CLASS0_IF3_OS_EXT_PROP0_DATA_STR               L"{7D9ADCFC-E570-4B38-BF4E-8F81F68964E0}"

//             <o>32-bit Integer
//             <i>Property Data in case Data Type is selected as Little/Big-endian 32-bit Integer.
#define USBD_CUSTOM_CLASS0_IF3_OS_EXT_PROP0_DATA_INT               0

//           </h>
//         </e>

//         <e.0>Custom Property Section 1
//         <i>Enable creation of custom property 1 section in Extended Properties OS Feature Descriptor.
#define USBD_CUSTOM_CLASS0_IF3_OS_EXT_PROP1_EN                     0

//           <o>Data Type
//           <i>Specifies the dwPropertyDataType field of custom property 1 section in Extended Properties OS Feature Descriptor.
//           <i>Values 3 (Free-form binary) and 7 (Multiple Unicode Strings) are not supported.
//             <1=>Unicode String (REG_SZ)
//             <2=>Unicode String with environment variables (REG_EXPAND_SZ)
//             <4=>Little-endian 32-bit integer (REG_DWORD_LITTLE_ENDIAN)
//             <5=>Big-endian 32-bit integer (REG_DWORD_BIG_ENDIAN)
//             <6=>Unicode String with symbolic link (REG_LINK)
#define USBD_CUSTOM_CLASS0_IF3_OS_EXT_PROP1_DATA_TYP               1

//           <s.512>Name
#define USBD_CUSTOM_CLASS0_IF3_OS_EXT_PROP1_NAME                   L""

//           <h>Data
//             <s.1024>Unicode String
//             <i>Property Data in case Data Type is selected as Unicode String.
#define USBD_CUSTOM_CLASS0_IF3_OS_EXT_PROP1_DATA_STR               L""

//             <o>32-bit Integer
//             <i>Property Data in case Data Type is selected as Little/Big-endian 32-bit Integer.
#define USBD_CUSTOM_CLASS0_IF3_OS_EXT_PROP1_DATA_INT               0

//           </h>
//         </e>

//         <e.0>Custom Property Section 2
//         <i>Enable creation of custom property 2 section in Extended Properties OS Feature Descriptor.
#define USBD_CUSTOM_CLASS0_IF3_OS_EXT_PROP2_EN                     0

//           <o>Data Type
//           <i>Specifies the dwPropertyDataType field of custom property 2 section in Extended Properties OS Feature Descriptor.
//           <i>Values 3 (Free-form binary) and 7 (Multiple Unicode Strings) are not supported.
//             <1=>Unicode String (REG_SZ)
//             <2=>Unicode String with environment variables (REG_EXPAND_SZ)
//             <4=>Little-endian 32-bit integer (REG_DWORD_LITTLE_ENDIAN)
//             <5=>Big-endian 32-bit integer (REG_DWORD_BIG_ENDIAN)
//             <6=>Unicode String with symbolic link (REG_LINK)
#define USBD_CUSTOM_CLASS0_IF3_OS_EXT_PROP2_DATA_TYP               1

//           <s.512>Name
#define USBD_CUSTOM_CLASS0_IF3_OS_EXT_PROP2_NAME                   L""

//           <h>Data
//             <s.1024>Unicode String
//             <i>Property Data in case Data Type is selected as Unicode String.
#define USBD_CUSTOM_CLASS0_IF3_OS_EXT_PROP2_DATA_STR               L""

//             <o>32-bit Integer
//             <i>Property Data in case Data Type is selected as Little/Big-endian 32-bit Integer.
#define USBD_CUSTOM_CLASS0_IF3_OS_EXT_PROP2_DATA_INT               0

//           </h>
//         </e>

//         <e.0>Custom Property Section 3
//         <i>Enable creation of custom property 3 section in Extended Properties OS Feature Descriptor.
#define USBD_CUSTOM_CLASS0_IF3_OS_EXT_PROP3_EN                     0

//           <o>Data Type
//           <i>Specifies the dwPropertyDataType field of custom property 3 section in Extended Properties OS Feature Descriptor.
//           <i>Values 3 (Free-form binary) and 7 (Multiple Unicode Strings) are not supported.
//             <1=>Unicode String (REG_SZ)
//             <2=>Unicode String with environment variables (REG_EXPAND_SZ)
//             <4=>Little-endian 32-bit integer (REG_DWORD_LITTLE_ENDIAN)
//             <5=>Big-endian 32-bit integer (REG_DWORD_BIG_ENDIAN)
//             <6=>Unicode String with symbolic link (REG_LINK)
#define USBD_CUSTOM_CLASS0_IF3_OS_EXT_PROP3_DATA_TYP               1

//           <s.512>Name
#define USBD_CUSTOM_CLASS0_IF3_OS_EXT_PROP3_NAME                   L""

//           <h>Data
//             <s.1024>Unicode String
//             <i>Property Data in case Data Type is selected as Unicode String.
#define USBD_CUSTOM_CLASS0_IF3_OS_EXT_PROP3_DATA_STR               L""

//             <o>32-bit Integer
//             <i>Property Data in case Data Type is selected as Little/Big-endian 32-bit Integer.
#define USBD_CUSTOM_CLASS0_IF3_OS_EXT_PROP3_DATA_INT               0

//           </h>
//         </e>
//       </h>
//     </h>
//   </e>


//   <h>OS Resources Settings
//   <i>These settings are used to optimize usage of OS resources.
//     <o>Endpoint 1 Thread Stack Size <64-65536>
//     <i>This setting is used if Endpoint 1 is enabled.
#define USBD_CUSTOM_CLASS0_EP1_THREAD_STACK_SIZE                   512

//        Endpoint 1 Thread Priority
#define USBD_CUSTOM_CLASS0_EP1_THREAD_PRIORITY                     osPriorityAboveNormal

//     <o>Endpoint 2 Thread Stack Size <64-65536>
//     <i>This setting is used if Endpoint 2 is enabled.
#define USBD_CUSTOM_CLASS0_EP2_THREAD_STACK_SIZE                   512

//        Endpoint 2 Thread Priority
#define USBD_CUSTOM_CLASS0_EP2_THREAD_PRIORITY                     osPriorityAboveNormal

//     <o>Endpoint 3 Thread Stack Size <64-65536>
//     <i>This setting is used if Endpoint 3 is enabled.
#define USBD_CUSTOM_CLASS0_EP3_THREAD_STACK_SIZE                   512

//        Endpoint 3 Thread Priority
#define USBD_CUSTOM_CLASS0_EP3_THREAD_PRIORITY                     osPriorityAboveNormal

//     <o>Endpoint 4 Thread Stack Size <64-65536>
//     <i>This setting is used if Endpoint 4 is enabled.
#define USBD_CUSTOM_CLASS0_EP4_THREAD_STACK_SIZE                   512

//        Endpoint 4 Thread Priority
#define USBD_CUSTOM_CLASS0_EP4_THREAD_PRIORITY                     osPriorityAboveNormal

//     <o>Endpoint 5 Thread Stack Size <64-65536>
//     <i>This setting is used if Endpoint 5 is enabled.
#define USBD_CUSTOM_CLASS0_EP5_THREAD_STACK_SIZE                   512

//        Endpoint 5 Thread Priority
#define USBD_CUSTOM_CLASS0_EP5_THREAD_PRIORITY                     osPriorityAboveNormal

//     <o>Endpoint 6 Thread Stack Size <64-65536>
//     <i>This setting is used if Endpoint 6 is enabled.
#define USBD_CUSTOM_CLASS0_EP6_THREAD_STACK_SIZE                   512

//        Endpoint 6 Thread Priority
#define USBD_CUSTOM_CLASS0_EP6_THREAD_PRIORITY                     osPriorityAboveNormal

//     <o>Endpoint 7 Thread Stack Size <64-65536>
//     <i>This setting is used if Endpoint 7 is enabled.
#define USBD_CUSTOM_CLASS0_EP7_THREAD_STACK_SIZE                   512

//        Endpoint 7 Thread Priority
#define USBD_CUSTOM_CLASS0_EP7_THREAD_PRIORITY                     osPriorityAboveNormal

//     <o>Endpoint 8 Thread Stack Size <64-65536>
//     <i>This setting is used if Endpoint 8 is enabled.
#define USBD_CUSTOM_CLASS0_EP8_THREAD_STACK_SIZE                   512

//        Endpoint 8 Thread Priority
#define USBD_CUSTOM_CLASS0_EP8_THREAD_PRIORITY                     osPriorityAboveNormal

//     <o>Endpoint 9 Thread Stack Size <64-65536>
//     <i>This setting is used if Endpoint 9 is enabled.
#define USBD_CUSTOM_CLASS0_EP9_THREAD_STACK_SIZE                   512

//        Endpoint 9 Thread Priority
#define USBD_CUSTOM_CLASS0_EP9_THREAD_PRIORITY                     osPriorityAboveNormal

//     <o>Endpoint 10 Thread Stack Size <64-65536>
//     <i>This setting is used if Endpoint 10 is enabled.
#define USBD_CUSTOM_CLASS0_EP10_THREAD_STACK_SIZE                  512

//        Endpoint 10 Thread Priority
#define USBD_CUSTOM_CLASS0_EP10_THREAD_PRIORITY                    osPriorityAboveNormal

//     <o>Endpoint 11 Thread Stack Size <64-65536>
//     <i>This setting is used if Endpoint 11 is enabled.
#define USBD_CUSTOM_CLASS0_EP11_THREAD_STACK_SIZE                  512

//        Endpoint 11 Thread Priority
#define USBD_CUSTOM_CLASS0_EP11_THREAD_PRIORITY                    osPriorityAboveNormal

//     <o>Endpoint 12 Thread Stack Size <64-65536>
//     <i>This setting is used if Endpoint 12 is enabled.
#define USBD_CUSTOM_CLASS0_EP12_THREAD_STACK_SIZE                  512

//        Endpoint 12 Thread Priority
#define USBD_CUSTOM_CLASS0_EP12_THREAD_PRIORITY                    osPriorityAboveNormal

//     <o>Endpoint 13 Thread Stack Size <64-65536>
//     <i>This setting is used if Endpoint 13 is enabled.
#define USBD_CUSTOM_CLASS0_EP13_THREAD_STACK_SIZE                  512

//        Endpoint 13 Thread Priority
#define USBD_CUSTOM_CLASS0_EP13_THREAD_PRIORITY                    osPriorityAboveNormal

//     <o>Endpoint 14 Thread Stack Size <64-65536>
//     <i>This setting is used if Endpoint 14 is enabled.
#define USBD_CUSTOM_CLASS0_EP14_THREAD_STACK_SIZE                  512

//        Endpoint 14 Thread Priority
#define USBD_CUSTOM_CLASS0_EP14_THREAD_PRIORITY                    osPriorityAboveNormal

//     <o>Endpoint 15 Thread Stack Size <64-65536>
//     <i>This setting is used if Endpoint 15 is enabled.
#define USBD_CUSTOM_CLASS0_EP15_THREAD_STACK_SIZE                  512

//        Endpoint 15 Thread Priority
#define USBD_CUSTOM_CLASS0_EP15_THREAD_PRIORITY                    osPriorityAboveNormal

//   </h>
// </h>
