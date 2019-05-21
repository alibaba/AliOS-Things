/******************************************************************
 *                                                                *
 *        Copyright Mentor Graphics Corporation 2006              *
 *                                                                *
 *                All Rights Reserved.                            *
 *                                                                *
 *    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION *
 *  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS   *
 *  LICENSORS AND IS SUBJECT TO LICENSE TERMS.                    *
 *                                                                *
 ******************************************************************/

/*
 * MUSB-MicroSW Screen Pointer Interface (SPI).
 * $Revision: 1.6 $
 */

#ifndef __MUSB_SPI_H__
#define __MUSB_SPI_H__

#include "mu_tools.h"

/**
 * Introduction to Screen Pointer Interface.
 * The functions herein can be implemented by an OS to allow
 * the Mentor USB pointing-device driver to be hooked into the OS'
 * GUI technology, thus allowing the Mentor driver to be
 * used without additional effort.
 * The callbacks herein are implemented by the Mentor driver,
 * exposing the functionality needed by an OS to use the pointing device.
 * The above could also be applied to a custom, non-OS environment.
 * Finally, an implementation can be created solely for demonstration purposes
 * (to avoid relying on platforms with pointing device support,
 * and/or to allow the Mentor USB pointing-device driver to be tested).
 */

/************************* SPI CONSTANTS **************************/

/**
 * MUSB_SpiNameQuery.
 * SPI name query.
 */
typedef enum
{
    /** Device vendor name */
    MUSB_SPI_NAME_DEVICE_VENDOR,
    /** Device product name */
    MUSB_SPI_NAME_DEVICE_PRODUCT,
    /** A button */
    MUSB_SPI_NAME_BUTTON,
    /** A wheel */
    MUSB_SPI_NAME_WHEEL
} MUSB_SpiNameQuery;

/**
 * MUSB_SpiWheelType.
 * SPI wheel type.
 */
typedef enum
{
    /** The required wheel intended for horizontal position */
    MUSB_SPI_WHEEL_X,
    /** The required wheel intended for vertical position */
    MUSB_SPI_WHEEL_Y,
    /** An optional wheel intended for fast (vertical) scrolling */
    MUSB_SPI_WHEEL_SCROLL,
    /** An optional wheel intended for Z position for a 3-D GUI or game */
    MUSB_SPI_WHEEL_Z,
    /** The base of unknown wheels (for masking purposes) */
    MUSB_SPI_WHEEL_UNKNOWN = 8
} MUSB_SpiWheelType;

/**
 * MUSB_SpiStatus.
 * SPI Operation Status.
 */
typedef enum
{
    /** success */
    MUSB_SPI_SUCCESS,
    /** error: device characteristics not supported */
    MUSB_SPI_ERROR_UNSUPPORTED_DEVICE,
    /** error: device was removed before operation could complete */
    MUSB_SPI_ERROR_DEVICE_REMOVED,
    /** error: transfer to/from device failed */
    MUSB_SPI_ERROR_TRANSFER
} MUSB_SpiStatus;

/*************************** SPI TYPES ****************************/

/**
 * MUSB_SpiDeviceInfo.
 * SPI device information.
 * A pointing-device driver fills this with any information
 * it knows before calling MUSB_SpiAddDevice.
 *
 * @field bButtonCount how many buttons the device has
 *
 * @field bmWheels a bitmask of 1's shifted left by the values
 * of the MUSB_SPI_WHEEL_* constants, representing the wheels present.
 * This supports zero or one of each "well-known" wheel type;
 * up to 24 other wheels can be supported with values starting
 * at MUSB_SPI_WHEEL_OTHER
 *
 * @field wVendorId USB VID,
 * for the HFI implementation's use in generating a volume name
 *
 * @field wProductId USB PID,
 * for the HFI implementation's use in generating a volume name
 *
 * @field bcdDevice USB bcdDevice,
 * for the HFI implementation's use in generating a volume name
 *
 * @field bBusAddress USB device address,
 * for the HFI implementation's use in generating a volume name
 */
typedef struct
{
    uint8_t bButtonCount;
    uint32_t bmWheels;
    uint16_t wVendorId;
    uint16_t wProductId;
    uint16_t bcdDevice;
    uint8_t bBusAddress;
} MUSB_SpiDeviceInfo;

/**
 * Get an SPI entity's name.
 * Get a name associated with an SPI device, button, or wheel.
 * @param pDeviceData pPrivateData from associated MUSB_SpiDevice
 * @param NameQuery indicates which name is requested
 * @param bIndex the index of the button or wheel whose name is requested
 * @param wLanguageCode a language/country code as per USB 2.0
 * @param pName storage for a UNICODE name,
 * filled and terminated with a wide-NUL on success
 * @param bNameLength size, in 16-bit UNICODE characters, of the pName buffer
 * @return status code
 */
typedef MUSB_SpiStatus (*MUSB_pfSpiGetName)(void *pDeviceData,
        MUSB_SpiNameQuery NameQuery, uint8_t bIndex,
        uint16_t wLanguageCode, uint16_t *pName, uint8_t bNameLength);

/**
 * MUSB_SpiDevice.
 * Ddevice driver callbacks (dispatch table).
 * @field pPrivateData device driver private data;
 * not to be interpreted by the SPI implementation
 * @field pfGetName get-name callback
 */
typedef struct
{
    void *pPrivateData;
    MUSB_pfSpiGetName pfGetName;
} MUSB_SpiDevice;

/** Handle created by the SPI implementation */
typedef void *MUSB_SpiDeviceHandle;

/************************* SPI FUNCTIONS **************************/

/**
 * Add a pointing device.
 * A pointing-device driver calls this to inform the SPI implementation
 * that a new device is being used.
 * The SPI implementation uses the device information to determine if
 * it can support the device,
 * and prepare the system to accept position information from it.
 * Error messages on failure are the SPI implementation's responsibility,
 * since the error messaging mechanism is OS-dependent.
 *
 * @param phDevice where to store a device handle on success
 * @param pInfo pointer to device information struct
 * @param pDevice pointer to struct with callbacks to access the device
 * @return MUSB_SPI_SUCCESS on success
 * @return MUSB_SPI_ERROR_UNSUPPORTED_DEVICE if the device cannot
 * be supported (e.g. not enough buttons)
 */
extern MUSB_SpiStatus MUSB_SpiAddDevice(MUSB_SpiDeviceHandle *phDevice,
                                        const MUSB_SpiDeviceInfo *pInfo, MUSB_SpiDevice *pDevice);

/**
 * A pointing-device button has been pressed.
 * After a successful device add, the pointing-device driver calls
 * this to inform the SPI implementation that a button has been pressed.
 * @param hDevice a device handle as filled by a successful MUSB_SpiAddDevice
 * @param bButtonIndex the index of the button
 */
extern void MUSB_SpiButtonPressed(MUSB_SpiDeviceHandle hDevice,
                                  uint8_t bButtonIndex);

/**
 * A pointing-device button has been pressed.
 * After a successful device add, the pointing-device driver calls
 * this to inform the SPI implementation that a button has been released.
 * @param hDevice a device handle as filled by a successful MUSB_SpiAddDevice
 * @param bButtonIndex the index of the button
 */
extern void MUSB_SpiButtonReleased(MUSB_SpiDeviceHandle hDevice,
                                   uint8_t bButtonIndex);

/**
 * Pointing-device wheels have moved.
 * After a successful device add, the pointing-device driver calls
 * this to inform the SPI implementation that one or more wheel
 * movements have occured.
 * @param hDevice a device handle as filled by a successful MUSB_SpiAddDevice
 * @param pMotions pointer to signed bytes representing the relative
 * motion of each wheel in abstract units.
 * The callee should make a copy of the motion data if it is needed
 * beyond the lifetime of this call.
 * The buffer is "compressed" meaning that there are no unused bytes
 * representing absent wheels.
 * For example, if the required X & Y wheels, and one "other" wheel
 * is present, the buffer will represent x, y, and "other" wheel motion
 * in offsets 0, 1 and 2.
 */
extern void MUSB_SpiWheelsMoved(MUSB_SpiDeviceHandle hDevice,
                                const int8_t *pMotions);

/**
 * A pointing device was removed.
 * A pointing-device driver calls this to inform the SPI implementation
 * that the device has been removed
 * @param hDevice a device handle as filled by a successful MUSB_SpiAddDevice
 */
extern void MUSB_SpiDeviceRemoved(MUSB_SpiDeviceHandle hDevice);

#endif	/* multiple inclusion protection */
