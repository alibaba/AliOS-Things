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
 * MUSB-MicroSW hub definitions.
 * $Revision: 1.4 $
 */

#ifndef __MUSB_HUB_H__
#define __MUSB_HUB_H__

#ifndef MUSB_HUB_MAX_PORTS
#define MUSB_HUB_MAX_PORTS	15
#endif

/** Hub descriptor type */
#define MUSB_DT_HUB			(MUSB_TYPE_CLASS | 0x09)

/**
 * Hub descriptor as per USB 2.0 Specification, 11.23.2.1.
 * @field bLength length of this descriptor in bytes
 * @field bDescriptorType should be MUSB_DT_HUB
 * @field bNbrPorts how many ports on the hub
 * @field wHubCharacteristics characteristics bitmask
 * @field bPwrOn2PwrGood time (in 2ms units) between power on to
 * a port is good
 * @field bHubCtrlCurrent maximum current requirements of the
 * hub controller in mA
 * @field aRemovablePowerMasks the remaining bytes are variable-length:
 * <dl>
 * <dt>DeviceRemovable
 * <dd>The number of bytes necessary to accomodate bNbrPorts+1
 * (8 ports per byte); bit 0 of first byte is reserved;
 * other bits are 0 for removable-device port, 1 for permanent port
 * <dt>PortPowerCtrlMask
 * <dd>The number of bytes necessary to accomodate bNbrPorts
 * (8 ports per byte); all bits should be 1s
 * </ol>
 */
#include "mu_pkon.h"
typedef struct
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bNbrPorts;
    uint16_t wHubCharacteristics;
    uint8_t bPwrOn2PwrGood;
    uint8_t bHubCtrlCurrent;
    uint8_t aDeviceRemovable[(MUSB_HUB_MAX_PORTS / 8) + 1];
    uint8_t aPortPwrCtrlMask[(MUSB_HUB_MAX_PORTS / 8) + 1];
    uint8_t bReserved1;
} MUSB_HubDescriptor;
#include "mu_pkoff.h"

/** Mask for the hub characteristics power field */
#define MUSB_M_HUB_CHARACTERISTICS_POWER	    0x0003
/** Shift for the hub characteristics power field */
#define MUSB_S_HUB_CHARACTERISTICS_POWER	    0
/** Ganged power switching value for the hub characteristics power field */
#define MUSB_HUB_CHARACTERISTICS_POWER_GANGED	    0
/** Individual port power switching value for the hub characteristics power field */
#define MUSB_HUB_CHARACTERISTICS_POWER_PORT	    1
#define MUSB_HUB_CHARACTERISTICS_POWER_RESERVED1    2
#define MUSB_HUB_CHARACTERISTICS_POWER_RESERVED2    3

/** Hub is part of a compound device */
#define MUSB_M_HUB_CHARACTERISTICS_COMPOUND	    0x0004

/** Mask for the hub characteristics over-current protection field */
#define MUSB_M_HUB_CHARACTERISTICS_OVERCURRENT	    0x0018
/** Shift for the hub characteristics over-current protection field */
#define MUSB_S_HUB_CHARACTERISTICS_OVERCURRENT	    3
/** Global overcurrent protection value for the hub characteristics overcurrent field */
#define MUSB_HUB_CHARACTERISTICS_OVERCURRENT_GLOBAL 0
/** Per-port overcurrent protection value for the hub characteristics overcurrent field */
#define MUSB_HUB_CHARACTERISTICS_OVERCURRENT_PORT   1

/** Mask for the hub characteristics TT think time field */
#define MUSB_M_HUB_CHARACTERISTICS_TT_TIME	    0x0060
/** Shift for the hub characteristics TT think time field */
#define MUSB_S_HUB_CHARACTERISTICS_TT_TIME	    5
/** TT requires <=8 FS bit times inter-transaction gap on full/low-speed downstream bus */
#define MUSB_HUB_CHARACTERISTICS_TT_TIME_8	    0
/** TT requires <=16 FS bit times inter-transaction gap on full/low-speed downstream bus */
#define MUSB_HUB_CHARACTERISTICS_TT_TIME_16	    1
/** TT requires <=24 FS bit times inter-transaction gap on full/low-speed downstream bus */
#define MUSB_HUB_CHARACTERISTICS_TT_TIME_24	    2
/** TT requires <=32 FS bit times inter-transaction gap on full/low-speed downstream bus */
#define MUSB_HUB_CHARACTERISTICS_TT_TIME_32	    3

/** Hub has port indicators (controlled by PORT_INDICATOR request) */
#define MUSB_M_HUB_CHARACTERISTICS_PORT_INDICATORS  0x0080

/**
 *  MUSB_HubPortStatus As per 11.24.2.7 USB 2.0 specification.
 *
 *  @field wStaus Tells about present status of the port as per
 *                MUSB_PortStatusBits.
 *
 *  @field wStatusChange Tells about which status bits have been
 *                       changed in the HUB port as per MUSB_PortChangeBits.
 */
typedef struct _MUSB_HubPortStatus_
{
    uint16_t wStatus;
    uint16_t wStatusChange;
} MUSB_HubPortStatus;

/** Feature selector used in SET and CLEAR feature request for Hub. */
typedef enum
{
    MUSB_C_HUB_LOCAL_POWER_FEATURE          = 0x00,
    MUSB_C_HUB_OVER_CURRENT_FEATURE         = 0x01
} MUSB_HubFeatureSelector;

/** Feature selector used in SET and CLEAR feature request for Port. */
typedef enum
{
    /** Port connection */
    MUSB_HUB_PORT_CONNECTION_FEATURE        = 0x0000,
    /**  Port Enable     */
    MUSB_HUB_PORT_ENABLE_FEATURE            = 0x0001,
    /** Port suspended   */
    MUSB_HUB_PORT_SUSPEND_FEATURE           = 0x0002,
    /**  Over Current port basis */
    MUSB_HUB_PORT_OVER_CURRENT_FEATURE      = 0x0003,
    /** Port reset */
    MUSB_HUB_PORT_RESET_FEATURE             = 0x0004,
    /**  Power on state. */
    MUSB_HUB_PORT_POWER_FEATURE             = 0x0008,
    /** Low speed device is connected. */
    MUSB_HUB_PORT_LOW_SPEED_FEATURE         = 0x0009,

    MUSB_HUB_C_PORT_CONNECTION_FEATURE      = 0x0010,
    MUSB_HUB_C_PORT_ENABLE_FEATURE          = 0x0011,
    MUSB_HUB_C_PORT_SUSPEND_FEATURE         = 0x0012,
    MUSB_HUB_C_PORT_OVER_CURRENT_FEATURE    = 0x0013,
    MUSB_HUB_C_PORT_RESET_FEATURE           = 0x0014,
    MUSB_HUB_PORT_TEST_FEATURE              = 0x0015,
    MUSB_HUB_PORT_INDICATOR_FEATURE         = 0x0016
} MUSB_HubPortFeatureSelector;

/**
 * MUSB_HubStatusBits (wStatus in MUSB_HubStatus As per 11.24.2.6 USB 2.0 specification.)
 */
typedef enum
{
    /** Local Power supply LOST */
    MUSB_HUB_LOCAL_POWER_SUPLY_LOST_BM    =  0x0001,
    /** Over current condition exists */
    MUSB_HUB_OVER_CURRENT_EXISTS_BM       =  0x0002
} MUSB_HubStatusBits;

/**
 * MUSB_HubChangeBits (wStatusChange in the MUSB_HubStatus As per 11.24.2.6 USB 2.0 specification).
 */
typedef enum
{
    /** Local Power supply status Changed */
    MUSB_HUB_LOCAL_POWER_STATUS_CHANGED_BM  = 0x0001,
    /** Over current condition status changed */
    MUSB_HUB_OVER_CURRENT_STATUS_CHANGED_BM = 0x0002
} MUSB_HubChangeBits;

/** MUSB_PortStatusBits (wStatus in the MUSB_HubPortStatus
 *  As per 11.24.2.7.1 USB 2.0 specification).
 */
typedef enum
{
    /**  Device is connected to the Hub's Port */
    MUSB_HUB_PORT_CONNECTION_BM    = 0x0001,
    /**  Hub Port is Enable     */
    MUSB_HUB_PORT_ENABLE_BM        = 0x0002,
    /** Hub Port is suspended   */
    MUSB_HUB_PORT_SUSPEND_BM       = 0x0004,
    /**  When Port takes extra current. */
    MUSB_HUB_PORT_OVER_CURRENT_BM  = 0x0008,
    /** When host resets the attached device.  */
    MUSB_HUB_PORT_RESET_BM         = 0x0010,
    /**  Power on state. */
    MUSB_HUB_PORT_POWER_BM         = 0x0100,
    /** Low speed device is connected. */
    MUSB_HUB_PORT_LOW_SPEED_BM     = 0x0200,
    /** High speed device is connected. */
    MUSB_HUB_PORT_HIGH_SPEED_BM    = 0x0400,
    /** Port's test mode */
    MUSB_HUB_PORT_TEST_BM          = 0x0800,
    /**  Software control of the port indicator. */
    MUSB_HUB_PORT_INDICATOR_BM     = 0x1000
} MUSB_PortStatusBits;

/**
 * wStatusChange in the MUSB_HubPortStatus structure.
 * As per 11.24.2.7.2 USB 2.0 specification.
 */
typedef enum
{
    /** Connect status changed i.e. either device is attached
     * or detached.
     */
    MUSB_HUB_C_PORT_CONNECTION_BM      = 0x0001,
    /** This bit set to 1 when port is disable */
    MUSB_HUB_C_PORT_ENABLE_BM          = 0x0002,
    /** This indicates changes of suspend state of
     *  the attached device
     */
    MUSB_HUB_C_PORT_SUSPEND_BM         = 0x0004,
    /** This field tells that over current indicator has changed.
     */
    MUSB_HUB_C_PORT_OVER_CURRENT_BM    = 0x0008,
    /** This bit is set when RESET processing in this port is complete
     */
    MUSB_HUB_C_PORT_RESET_BM           = 0x0010
} MUSB_PortChangeBits;

#endif	/* multiple inclusion protection */

