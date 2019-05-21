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
 * MUSB-MicroSW Keyboard Input Interface (KII).
 * $Revision: 1.5 $
 */

#ifndef __MUSB_KII_H__
#define __MUSB_KII_H__

#include "mu_tools.h"

/**
 * Introduction to Keyboard Input Interface.
 * The functions herein can be implemented by an OS to allow
 * the Mentor USB HID keyboard driver to be hooked into the OS'
 * GUI/console technology, thus allowing the Mentor driver to be
 * used without additional effort.
 * The callbacks herein are implemented by the Mentor driver,
 * exposing the functionality needed by an OS to use the keyboard.
 * The above could also be applied to a custom, non-OS environment.
 * Finally, an implementation can be created solely for demonstration purposes
 * (to avoid relying on platforms with keyboard support,
 * and/or to allow the Mentor USB HID keyboard driver to be tested).
 * <p>
 * Features:
 * <ul>
 * <li>Precise state of pre-defined "modifier" keys
 * <li>Press/release notification for a single character key at a time,
 * in UNICODE
 * <li>Press/release notification for a single special key at a time
 * </ul>
 */

/************************* KII CONSTANTS **************************/

/**
 * MUSB_KiiModifierKey.
 * KII modifier key.
 */
typedef enum
{
    /** Left control */
    MUSB_KII_KEY_L_CONTROL,
    /** Right control */
    MUSB_KII_KEY_R_CONTROL,
    /** Left shift */
    MUSB_KII_KEY_L_SHIFT,
    /** Right shift */
    MUSB_KII_KEY_R_SHIFT,
    /** Left Alt */
    MUSB_KII_KEY_L_ALT,
    /** Right alt */
    MUSB_KII_KEY_R_ALT,
    /** Alt-Graph */
    MUSB_KII_KEY_ALT_GRAPH
} MUSB_KiiModifierKey;

/**
 * MUSB_KiiSpecialKey.
 * KII special key.
 */
typedef enum
{
    /** F1 */
    MUSB_KII_KEY_F1,
    /** F2 */
    MUSB_KII_KEY_F2,
    /** F3 */
    MUSB_KII_KEY_F3,
    /** F4 */
    MUSB_KII_KEY_F4,
    /** F5 */
    MUSB_KII_KEY_F5,
    /** F6 */
    MUSB_KII_KEY_F6,
    /** F7 */
    MUSB_KII_KEY_F7,
    /** F8 */
    MUSB_KII_KEY_F8,
    /** F9 */
    MUSB_KII_KEY_F9,
    /** F10 */
    MUSB_KII_KEY_F10,
    /** F11 */
    MUSB_KII_KEY_F11,
    /** F12 */
    MUSB_KII_KEY_F12,
    /** Insert */
    MUSB_KII_KEY_INSERT,
    /** Delete */
    MUSB_KII_KEY_DELETE,
    /** Home */
    MUSB_KII_KEY_HOME,
    /** End */
    MUSB_KII_KEY_END,
    /** PageUp */
    MUSB_KII_KEY_PAGE_UP,
    /** PageDn */
    MUSB_KII_KEY_PAGE_DOWN,
    /** Up */
    MUSB_KII_KEY_UP,
    /** Down */
    MUSB_KII_KEY_DOWN,
    /** Left */
    MUSB_KII_KEY_LEFT,
    /** Right */
    MUSB_KII_KEY_RIGHT,
    /** Caps Lock */
    MUSB_KII_KEY_CAPS_LOCK,
    /** Num Lock */
    MUSB_KII_KEY_NUM_LOCK,
    /** Scroll Lock */
    MUSB_KII_KEY_SCROLL_LOCK,
    /** PrintScreen */
    MUSB_KII_KEY_PRINT_SCREEN,
    /** Break */
    MUSB_KII_KEY_BREAK
} MUSB_KiiSpecialKey;

/**
 * MUSB_KiiLed.
 * KII LED.
 */
typedef enum
{
    /** The "Num Lock" LED */
    MUSB_KII_LED_NUM_LOCK,
    /** The "Caps Lock" LED */
    MUSB_KII_LED_CAPS_LOCK,
    /** The "Scroll Lock" LED */
    MUSB_KII_LED_SCROLL_LOCK,
    /** The "Compose" LED */
    MUSB_KII_LED_COMPOSE
} MUSB_KiiLed;

/**
 * MUSB_KiiStatus.
 * KII Operation Status.
 */
typedef enum
{
    /** success */
    MUSB_KII_SUCCESS,
    /** error: device characteristics not supported */
    MUSB_KII_ERROR_UNSUPPORTED_DEVICE,
    /** error: device was removed before operation could complete */
    MUSB_KII_ERROR_DEVICE_REMOVED,
    /** error: transfer to/from device failed */
    MUSB_KII_ERROR_TRANSFER
} MUSB_KiiStatus;

/*************************** KII TYPES ****************************/

/**
 * MUSB_KiiDeviceInfo.
 * Device information.
 * A HID keyboard driver fills this with any information
 * it knows before calling MUSB_KiiAddDevice.
 *
 * @field wLanguage USB language ID for the keyboard's layout
 *
 * @field wVendorId USB VID,
 * for the KII implementation's use in generating a device name
 *
 * @field wProductId USB PID,
 * for the KII implementation's use in generating a device name
 *
 * @field bcdDevice USB bcdDevice,
 * for the KII implementation's use in generating a device name
 *
 * @field bBusAddress USB device address,
 * for the KII implementation's use in generating a device name
 */
typedef struct
{
    uint16_t wLanguage;
    uint16_t wVendorId;
    uint16_t wProductId;
    uint16_t bcdDevice;
    uint8_t bBusAddress;
} MUSB_KiiDeviceInfo;

/**
 * Set LED status.
 * Set whether an LED should be lighted.
 * @param pDeviceData pPrivateData from associated MUSB_KiiDevice
 * @param bIndex the index of the LED
 * @param bLight TRUE to light the LED, FALSE to unlight it
 * @return status code
 */
typedef MUSB_KiiStatus (*MUSB_pfKiiSetLed)(void *pDeviceData,
        uint8_t bIndex, uint8_t bLight);

/**
 * MUSB_KiiDevice.
 * Device driver callbacks.
 * @field pPrivateData device driver private data;
 * not to be interpreted by the KII implementation
 * @field pfSetLed set-LED callback
 */
typedef struct
{
    void *pPrivateData;
    MUSB_pfKiiSetLed pfSetLed;
} MUSB_KiiDevice;

/** Handle created by the KII implementation */
typedef void *MUSB_KiiDeviceHandle;

/************************* KII FUNCTIONS **************************/

/**
 * Add a keyboard.
 * A HID keyboard driver calls this to inform the KII implementation
 * that a new device is being used.
 * The KII implementation uses the device information to determine if
 * it can support the device,
 * and prepare the system to accept position information from it.
 * Error messages on failure are the KII implementation's responsibility,
 * since the error messaging mechanism is OS-dependent.
 *
 * @param phDevice where to store a device handle on success
 * @param pInfo pointer to device information struct
 * @param pDevice pointer to struct with callbacks to access the device
 * @return MUSB_KII_SUCCESS on success
 * @return MUSB_KII_ERROR_UNSUPPORTED_DEVICE if the device cannot
 * be supported (e.g. not enough buttons)
 */
extern MUSB_KiiStatus MUSB_KiiAddDevice(MUSB_KiiDeviceHandle *phDevice,
                                        const MUSB_KiiDeviceInfo *pInfo, MUSB_KiiDevice *pDevice);

/**
 * The state of modifier keys has changed.
 * After a successful device add, the keyboard driver calls
 * this to inform the KII implementation that the state of modifier keys has changed.
 * @param hDevice a device handle as filled by a successful MUSB_KiiAddDevice
 * @param dwModifierMask a bitmask of 1's shifted left by the constants
 * from MUSB_KiiModifierKey, for modifier keys that are currently down
 * (depressed)
 * @see #MUSB_KiiModifierKey
 */
extern void MUSB_KiiModifiersChanged(MUSB_KiiDeviceHandle hDevice,
                                     uint32_t dwModifierMask);

/**
 * A character-key has been pressed.
 * After a successful device add, the keyboard driver calls
 * this to inform the KII implementation that a character-key has been pressed.
 * @param hDevice a device handle as filled by a successful MUSB_KiiAddDevice
 * @param wCode UNICODE character
 */
extern void MUSB_KiiCharacterKeyPressed(MUSB_KiiDeviceHandle hDevice,
                                        uint16_t wCode);

/**
 * A character-key has been released.
 * After a successful device add, the keyboard driver calls
 * this to inform the KII implementation that a character-key has been released.
 * @param hDevice a device handle as filled by a successful MUSB_KiiAddDevice
 * @param wCode UNICODE character
 */
extern void MUSB_KiiCharacterKeyReleased(MUSB_KiiDeviceHandle hDevice,
        uint16_t wCode);

/**
 * A special key has been pressed.
 * After a successful device add, the keyboard driver calls
 * this to inform the KII implementation that a special key has been pressed.
 * @param hDevice a device handle as filled by a successful MUSB_KiiAddDevice
 * @param wCode UNICODE character
 */
extern void MUSB_KiiSpecialKeyPressed(MUSB_KiiDeviceHandle hDevice,
                                      MUSB_KiiSpecialKey Key);

/**
 * A special key has been released.
 * After a successful device add, the keyboard driver calls
 * this to inform the KII implementation that a special key has been released.
 * @param hDevice a device handle as filled by a successful MUSB_KiiAddDevice
 * @param wCode UNICODE character
 */
extern void MUSB_KiiSpecialKeyReleased(MUSB_KiiDeviceHandle hDevice,
                                       MUSB_KiiSpecialKey Key);

/**
 * A keyboard was removed.
 * A keyboard driver calls this to inform the KII implementation
 * that the device has been removed
 * @param hDevice a device handle as filled by a successful MUSB_KiiAddDevice
 */
extern void MUSB_KiiDeviceRemoved(MUSB_KiiDeviceHandle hDevice);

#endif	/* multiple inclusion protection */
