/*****************************************************************************
 *                                                                           *
 *      Copyright Mentor Graphics Corporation 2006                           *
 *                                                                           *
 *                All Rights Reserved.                                       *
 *                                                                           *
 *    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION            *
 *  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS              *
 *  LICENSORS AND IS SUBJECT TO LICENSE TERMS.                               *
 *                                                                           *
 ****************************************************************************/

/*
 * MUSB-MicroSW communications class definitions.
 * $Revision: 1.5 $
 */

#ifndef __MUSB_CCD_H__
#define __MUSB_CCD_H__

/* bInterfaceClass */
#define MGC_CCD_CLASS_COMM	                     0x02
#define MGC_CCD_CLASS_DATA	                     0x0a

/* bInterfaceSubClass */
#define MGC_CCD_SUBCLASS_COMM_DIRECT_LINE        0x01
#define MGC_CCD_SUBCLASS_COMM_ABSTRACT           0x02
#define MGC_CCD_SUBCLASS_COMM_TELEPHONE          0x03
#define MGC_CCD_SUBCLASS_COMM_MULTICHANNEL       0x04
#define MGC_CCD_SUBCLASS_COMM_CAPI               0x05
#define MGC_CCD_SUBCLASS_COMM_ETHERNET           0x06
#define MGC_CCD_SUBCLASS_COMM_ATM                0x07
#define MGC_CCD_SUBCLASS_COMM_WIRELESS_HANDSET   0x08
#define MGC_CCD_SUBCLASS_COMM_DEVICE_MGMT        0x09
#define MGC_CCD_SUBCLASS_COMM_MOBILE_DIRECT_LINE 0x0a
#define MGC_CCD_SUBCLASS_COMM_OBEX               0x0b
#define MGC_CCD_SUBCLASS_COMM_EEM                0x0c

/* bInterfaceProtocol */
#define MGC_CCD_PROTOCOL_EEM               0x07

#define MGC_CCD_PROTOCOL_COMM_GENERIC      0x00
#define MGC_CCD_PROTOCOL_COMM_V25ter       0x01
#define MGC_CCD_PROTOCOL_COMM_PCCA101      0x02
#define MGC_CCD_PROTOCOL_COMM_PCCA101_101O 0x03
#define MGC_CCD_PROTOCOL_COMM_GSM0707      0x04
#define MGC_CCD_PROTOCOL_COMM_3GPP27007    0x05
#define MGC_CCD_PROTOCOL_COMM_CS00170      0x06
#define MGC_CCD_PROTOCOL_COMM_EXTERNAL     0xfe

#define MGC_CCD_PROTOCOL_DATA_GENERIC      0x00

/** physical interface protocol for ISDN BRI */
#define MGC_CCD_PROTOCOL_DATA_I430         0x30

/** ISO/IEC 3309-1993 */
#define MGC_CCD_PROTOCOL_DATA_HDLC         0x31

#define MGC_CCD_PROTOCOL_DATA_TRANSPARENT  0x32

/** Management protocol for Q.921 data link protocol */
#define MGC_CCD_PROTOCOL_DATA_Q921M        0x50

/** Data link protocol for Q.931 */
#define MGC_CCD_PROTOCOL_DATA_Q921         0x51

/** TEI-multiplexor for Q.921 data link protocol */
#define MGC_CCD_PROTOCOL_DATA_Q921TM       0x52

#define MGC_CCD_PROTOCOL_DATA_V42bis       0x90

/** Euro-ISDN control protocol */
#define MGC_CCD_PROTOCOL_DATA_Q931         0x91

/** V.24 rate adaptation to ISDN */
#define MGC_CCD_PROTOCOL_DATA_V120         0x92

/** CAPI commands */
#define MGC_CCD_PROTOCOL_DATA_CAPI20       0x93

#define MGC_CCD_PROTOCOL_DATA_HOST_DRIVER  0xFD

/** Described using Protocol Functional Unit descriptor */
#define MGC_CCD_PROTOCOL_DATA_CDC          0xFE

#define MGC_CCD_PROTOCOL_DATA_VENDOR       0xFF

/* request types (bmRequestType values) for OUTs and INs (wIndex=interface): */
#define MGC_CCD_REQTYPE_OUT       (MUSB_TYPE_CLASS | MUSB_RECIP_INTERFACE)
#define MGC_CCD_REQTYPE_IN        (MUSB_DIR_IN | MUSB_TYPE_CLASS | MUSB_RECIP_INTERFACE)

/*
 * Protocol Data Wrapper:
 * uint16_t wSize wrapper size (including data???)
 * uint8_t bDestProtocol
 * uint8_t bSrcProtocol
 * (data)
 */

/* requests (wIndex=interface) */
/* data in data phase */
#define MGC_CCD_REQ_SEND_ENCAPSULATED_CMD              0x00
/* data in data phase */
#define MGC_CCD_REQ_GET_ENCAPSULATED_RESPONSE          0x01
/* wValue=feature selector, data=state */
#define MGC_CCD_REQ_SET_COMM_FEATURE                   0x02
/* wValue=feature selector, data=state */
#define MGC_CCD_REQ_GET_COMM_FEATURE                   0x03
/* wValue=feature selector */
#define MGC_CCD_REQ_CLEAR_COMM_FEATURE                 0x04
/* wValue=0=>disconnect, 1=>connect */
#define MGC_CCD_REQ_SET_AUX_LINE_STATE                 0x10
/* wValue=relay config */
#define MGC_CCD_REQ_SET_HOOK_STATE                     0x11
/* wValue=enable/disable */
#define MGC_CCD_REQ_PULSE_SETUP                        0x12
/* wValue=cycles */
#define MGC_CCD_REQ_SEND_PULSE                         0x13
/* wValue=timing */
#define MGC_CCD_REQ_SET_PULSE_TIME                     0x14
/* wValue=number of rings */
#define MGC_CCD_REQ_RING_AUX_JACK                      0x15
/* data phase=line coding struct */
#define MGC_CCD_REQ_SET_LINE_CODING                    0x20
/* data phase=line coding struct */
#define MGC_CCD_REQ_GET_LINE_CODING                    0x21
/* wValue=control signal bitmap */
#define MGC_CCD_REQ_SET_CONTROL_LINE_STATE             0x22
/* wValue=duration of break */
#define MGC_CCD_REQ_SEND_BREAK                         0x23
/* wLength=4, data phase=ringer config bitmap */
#define MGC_CCD_REQ_SET_RINGER_PARAMS                  0x30
/* wLength=4, data phase=ringer config bitmap */
#define MGC_CCD_REQ_GET_RINGER_PARAMS                  0x31
/* wValue=operation mode */
#define MGC_CCD_REQ_SET_OPERATION_PARAMS               0x32
/* wLenght=2, data phase=operation mode */
#define MGC_CCD_REQ_GET_OPERATION_PARAMS               0x33
/* wValue=line state change */
#define MGC_CCD_REQ_SET_LINE_PARAMS                    0x34
/* data phase=line status struct */
#define MGC_CCD_REQ_GET_LINE_PARAMS                    0x35
/* data phase=dialing string */
#define MGC_CCD_REQ_DIAL_DIGITS                        0x36
/* wValue=unit param struct, data phase=unit param */
#define MGC_CCD_REQ_SET_UNIT_PARAMETER                 0x37
/* wValue=unit param struct, data phase=unit param */
#define MGC_CCD_REQ_GET_UNIT_PARAMETER                 0x38
/* wValue=unit param struct */
#define MGC_CCD_REQ_CLEAR_UNIT_PARAMETER               0x39
/* wLength=64, data phase=profile info */
#define MGC_CCD_REQ_GET_PROFILE                        0x3a
/* wValue=number of filters, wLength=N*6, data phase=multicast addresses */
#define MGC_CCD_REQ_SET_ETHER_MULTICAST_FILTERS        0x40
/* wValue=filter number, data phase=power mgmt filter struct */
#define MGC_CCD_REQ_SET_ETHER_POWER_MANAGEMENT_FILTER  0x41
/* wValue=filter number, data phase=pattern active boolean */
#define MGC_CCD_REQ_GET_ETHER_POWER_MANAGEMENT_FILTER  0x42
/* wValue=packet filter bitmap */
#define MGC_CCD_REQ_SET_ETHER_PACKET_FILTER            0x43
/* wValue=feature selector, wLength=4, data phase=unsigned int */
#define MGC_CCD_REQ_GET_ETHER_STATISTIC                0x44
/* wValue=data format */
#define MGC_CCD_REQ_SET_ATM_DATA_FORMAT                0x50
/* wValue=feature selector, wLength=4, data phase=unsigned int */
#define MGC_CCD_REQ_GET_ATM_DEVICE_STATISTICS          0x51
/* wLength=3, data phase=1B VPI, 2B VCI */
#define MGC_CCD_REQ_SET_ATM_DEFAULT_VC                 0x52
/* wValue=feature selector, wLength=4, data phase=unsigned int */
#define MGC_CCD_REQ_GET_ATM_VC_STATISTICS              0x53

/* notifications (wIndex=interface) */
/* wValue: 0=disconnected, 1=connected */
#define MGC_CCD_NOTIF_NETWORK_CONNECTION               0x00
#define MGC_CCD_NOTIF_RESPONSE_AVAILABLE               0x01
/* wValue: 0=on-hook, 1=off-hook */
#define MGC_CCD_NOTIF_AUX_JACK_HOOK_STATE              0x08
#define MGC_CCD_NOTIF_RING_DETECT                      0x09
#define MGC_CCD_NOTIF_SERIAL_STATE                     0x20
#define MGC_CCD_NOTIF_CALL_STATE_CHANGE                0x28
#define MGC_CCD_NOTIF_LINE_STATE_CHANGE                0x29
/* data phase: speed change struct (4B upstream bit rate, 4B ds bit rate) */
#define MGC_CCD_NOTIF_CONNECTION_SPEED_CHANGE          0x2a

/**
 * MGC_CdcLineCoding.
 * Line coding struct
 * @field wBaudRate baud rate
 * @field bCharFormat 0=1 stop bit, 1=1-1.5 stops, 2=2 stops
 * @field bParityType 0=none, 1=odd, 2=even, 3=mark, 4=space
 * @field bDataBits 5, 6, 7, 8, or 16
 */
typedef struct
{
    uint32_t wBaudRate;
    uint8_t bCharFormat;
    uint8_t bParityType;
    uint8_t bDataBits;
} MGC_CdcLineCoding;

/* class-specific descriptor types */
#define MGC_CCD_FUNC_DESC_TYPE_CS_INTERFACE            0x24
#define MGC_CCD_FUNC_DESC_TYPE_CS_ENDPOINT             0x25

/* descriptor subtypes */
#define MGC_CCD_FUNC_DESC_SUBTYPE_HEADER               0x00
#define MGC_CCD_FUNC_DESC_SUBTYPE_CALL_MGMT            0x01
#define MGC_CCD_FUNC_DESC_SUBTYPE_ABSTRACT_CNTRL_MGMT  0x02
#define MGC_CCD_FUNC_DESC_SUBTYPE_DIRECT_LINE_MGMT     0x03
#define MGC_CCD_FUNC_DESC_SUBTYPE_TELEPHONE_RINGER     0x04
#define MGC_CCD_FUNC_DESC_SUBTYPE_TELEPHONE_REPORTING  0x05
#define MGC_CCD_FUNC_DESC_SUBTYPE_UNION                0x06
#define MGC_CCD_FUNC_DESC_SUBTYPE_COUNTRY_SELECTION    0x07
#define MGC_CCD_FUNC_DESC_SUBTYPE_TELEPHONE_OPMODES    0x08
#define MGC_CCD_FUNC_DESC_SUBTYPE_USB_TERMINAL         0x09
#define MGC_CCD_FUNC_DESC_SUBTYPE_NET_CHANNEL_TERMINAL 0x0a
#define MGC_CCD_FUNC_DESC_SUBTYPE_PROTOCOL_UNIT        0x0b
#define MGC_CCD_FUNC_DESC_SUBTYPE_EXTENSION_UNIT       0x0c
#define MGC_CCD_FUNC_DESC_SUBTYPE_MULTICHANNEL_MGMT    0x0d
#define MGC_CCD_FUNC_DESC_SUBTYPE_CAPI_CNTRL_MGMT      0x0e
#define MGC_CCD_FUNC_DESC_SUBTYPE_ETHERNET             0x0f
#define MGC_CCD_FUNC_DESC_SUBTYPE_ATM                  0x10
#define MGC_CCD_FUNC_DESC_SUBTYPE_WIRELESS_HANDSET_CTL 0x11
#define MGC_CCD_FUNC_DESC_SUBTYPE_MOBILE_DIRECT_LINE   0x12
#define MGC_CCD_FUNC_DESC_SUBTYPE_MDLM_DETAIL          0x13
#define MGC_CCD_FUNC_DESC_SUBTYPE_DEVICE_MGMT          0x14
#define MGC_CCD_FUNC_DESC_SUBTYPE_OBEX                 0x15
#define MGC_CCD_FUNC_DESC_SUBTYPE_COMMAND_SET          0x16
#define MGC_CCD_FUNC_DESC_SUBTYPE_COMMAND_SET_DETAIL   0x17
#define MGC_CCD_FUNC_DESC_SUBTYPE_TELEPHONE_CONTROL    0x18

/**
 * MGC_CdcHeaderDescriptor.
 * Header descriptor
 * @field bFunctionLength descriptor length
 * @field bDescriptorType MGC_CCD_DESC_TYPE_CS_INTERFACE
 * @field bDescriptorSubtype MGC_CCD_FUNC_DESC_SUBTYPE_HEADER
 * @field bcdCDC CDC revision in BCD
 */
typedef struct
{
    uint8_t bFunctionLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint16_t bcdCDC;
} MGC_CdcHeaderDescriptor;

/**
 * MGC_CdcCallMgmtDescriptor.
 * Header descriptor
 * @field bFunctionLength descriptor length
 * @field bDescriptorType MGC_CCD_DESC_TYPE_CS_INTERFACE
 * @field bDescriptorSubtype MGC_CCD_FUNC_DESC_SUBTYPE_CALL_MGMT
 * @field bmCapabilities capabilities
 * @field bDataInterface interface number of data class interface
 */
typedef struct
{
    uint8_t bFunctionLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint8_t bmCapabilities;
    uint8_t bDataInterface;
} MGC_CdcCallMgmtDescriptor;
/** set if device handles call management itself */
#define MGC_M_CCD_CALL_MGMT_HANDLES_CALL_MGMT 0x01
/** set if device can accept call management commands on data interface */
#define MGC_M_CCD_CALL_MGMT_CALL_MGMT_DATA_IF 0x02

/**
 * MGC_CdcAbstractCntrlDescriptor.
 * Header descriptor
 * @field bFunctionLength descriptor length
 * @field bDescriptorType MGC_CCD_DESC_TYPE_CS_INTERFACE
 * @field bDescriptorSubtype MGC_CCD_FUNC_DESC_SUBTYPE_ABSTRACT_CNTRL_MGMT
 * @field bmCapabilities capabilities
 */
typedef struct
{
    uint8_t bFunctionLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint8_t bmCapabilities;
} MGC_CdcAbstractCntrlDescriptor;
/** set if device supports _COMM_FEATURE commands */
#define MGC_M_CCD_ABSTRACT_CNTRL_SUPP_COMM_FEATURE 0x01
/** set if device supports line coding cmds & serial state notification */
#define MGC_M_CCD_ABSTRACT_CNTRL_SUPP_SERIAL_LINE  0x02
/** set if device supports SEND_BREAK */
#define MGC_M_CCD_ABSTRACT_CNTRL_SUPP_SEND_BREAK   0x04
/** set if device supports NETWORK_CONNECTION notification */
#define MGC_M_CCD_ABSTRACT_CNTRL_NET_CONNECTION    0x08

/**
 * MGC_CdcDirectLineMgmtDescriptor.
 * Direct line management descriptor
 * @field bFunctionLength descriptor length
 * @field bDescriptorType MGC_CCD_DESC_TYPE_CS_INTERFACE
 * @field bDescriptorSubtype MGC_CCD_FUNC_DESC_SUBTYPE_DIRECT_LINE_MGMT
 * @field bmCapabilities capabilities
 */
typedef struct
{
    uint8_t bFunctionLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint8_t bmCapabilities;
} MGC_CdcDirectLineMgmtDescriptor;
/** set if device supports _PULSE_ commands */
#define MGC_M_CCD_DIRECT_LINE_SUPP_PULSE        0x01
/** set if device supports aux line commands */
#define MGC_M_CCD_DIRECT_LINE_SUPP_AUX          0x02
/** set if device requires extra PULSE_SETUP command during dialing */
#define MGC_M_CCD_DIRECT_LINE_EXTRA_PULSE_SETUP 0x04

/**
 * MGC_CdcUnionDescriptor.
 * Union descriptor
 * @field bFunctionLength descriptor length
 * @field bDescriptorType MGC_CCD_DESC_TYPE_CS_INTERFACE
 * @field bDescriptorSubtype MGC_CCD_FUNC_DESC_SUBTYPE_UNION
 * @field bMasterInterface master interface
 * @field abSlaveInterfaces slave interface(s)
 */
typedef struct
{
    uint8_t bFunctionLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint8_t bMasterInterface;
    uint8_t abSlaveInterfaces[1];
} MGC_CdcUnionDescriptor;

/**
 * MGC_CdcTerminalDescriptor.
 * Terminal descriptor
 * @field bFunctionLength descriptor length
 * @field bDescriptorType MGC_CCD_DESC_TYPE_CS_INTERFACE
 * @field bDescriptorSubtype MGC_CCD_FUNC_DESC_SUBTYPE_USB_TERMINAL
 * @field bEntityId unique ID of this terminal
 * @field bInInterfaceNo the input interface number of associated USB interface
 * @field bOutInterfaceNo the output interface number of associated USB interface
 * @field bmOptions options (bit 0: set if protocol wrapper is used)
 * @field abChildIds array of IDs of lower terminals/units to which this is connected
 */
typedef struct
{
    uint8_t bFunctionLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint8_t bEntityId;
    uint8_t bInInterfaceNo;
    uint8_t bOutInterfaceNo;
    uint8_t bmOptions;
    uint8_t abChildIds[1];
} MGC_CdcTerminalDescriptor;

/**
 * MGC_CdcNetworkChannelDescriptor.
 * Terminal descriptor
 * @field bFunctionLength descriptor length
 * @field bDescriptorType MGC_CCD_DESC_TYPE_CS_INTERFACE
 * @field bDescriptorSubtype MGC_CCD_FUNC_DESC_SUBTYPE_NET_CHANNEL_TERMINAL
 * @field bEntityId unique ID of this terminal
 * @field iName index of string descriptor for name of channel
 * @field bChannelIndex the channel index
 * @field bPhysicalInterface type (0=none, 1=ISDN, 201-255=vendor-specific)
 */
typedef struct
{
    uint8_t bFunctionLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint8_t bEntityId;
    uint8_t iName;
    uint8_t bChannelIndex;
    uint8_t bPhysicalInterface;
} MGC_CdcNetworkChannelDescriptor;

/**
 * MGC_CdcProtocolUnitDescriptor.
 * Protocol Unit descriptor
 * @field bFunctionLength descriptor length
 * @field bDescriptorType MGC_CCD_DESC_TYPE_CS_INTERFACE
 * @field bDescriptorSubtype MGC_CCD_FUNC_DESC_SUBTYPE_USB_PROTOCOL_UNIT
 * @field bEntityId unique ID of this terminal
 * @field bProtocol protocol ID (MGC_CCD_PROTOCOL_*)
 * @field abChildIds array of IDs of lower terminals/units to which this is connected
 */
typedef struct
{
    uint8_t bFunctionLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint8_t bEntityId;
    uint8_t bProtocol;
    uint8_t abChildIds[1];
} MGC_CdcProtocolUnitDescriptor;

/**
 * MGC_CdcEthernetDescriptor.
 * Ethernet descriptor
 * @field bFunctionLength descriptor length
 * @field bDescriptorType MGC_CCD_DESC_TYPE_CS_INTERFACE
 * @field bDescriptorSubtype MGC_CCD_FUNC_DESC_SUBTYPE_ETHERNET
 * @field iMACAddress index of string of MAC address in Unicode (MSN first)
 * @field bmEthernetStatistics bitmask of statistics supported
 * @field wMaxSegmentSize typically 1514
 * @field wNumberMCFilters bit 15 set means imperfect filtering
 * @field bNumberPowerFilters number of power filters supported
 */
typedef struct
{
    uint8_t bFunctionLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint8_t iMACAddress;
    uint32_t bmEthernetStatistics;
    uint16_t wMaxSegmentSize;
    uint16_t wNumberMCFilters;
    uint8_t bNumberPowerFilters;
} MGC_CdcEthernetDescriptor;

#endif	/* multiple inclusion protection */
