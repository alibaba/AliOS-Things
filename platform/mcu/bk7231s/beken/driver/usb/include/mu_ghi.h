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
 * MUSB-MicroSW Generic HID Interface (GHI).
 * $Revision: 1.9 $
 */

#ifndef __MUSB_GHI_H__
#define __MUSB_GHI_H__

#include "mu_tools.h"

/**
 * Introduction to Generic HID Interface.
 * The functions herein can be implemented by an OS or application to allow
 * the Mentor USB HID driver to be hooked into the OS'
 * technology or application, thus allowing the Mentor driver to be
 * used without additional effort.
 * The callbacks herein are implemented by the Mentor driver,
 * exposing the functionality needed by an OS or application to use the device.
 * The above could also be applied to a custom, non-OS environment.
 * <p>
 * Features:
 * <ul>
 * <li>Report descriptor parsing, making generic "item" information
 * available to the client
 * <li>Getting input reports
 * <li>Setting feature and output reports
 * </ul>
 * Clients must implement MUSB_GhiAddDevice and MUSB_GhiDeviceRemoved,
 * and may make use of the information and services provided
 * in the parameters to these functions.
 * @see MUSB_GhiAddDevice
 * @see MUSB_GhiDeviceRemoved
 */

/************************* GHI CONSTANTS **************************/

/**
 * MUSB_GhiItemType.
 * GHI item type.
 */
typedef enum
{
    /** Unknown (other or vendor-defined) */
    MUSB_GHI_ITEM_UNKNOWN,
    /** Generic desktop controls */
    MUSB_GHI_ITEM_GENERIC_DESKTOP,
    /** Generic device controls */
    MUSB_GHI_ITEM_GENERIC_DEVICE,
    /** Keyboard/keypad */
    MUSB_GHI_ITEM_KEYBOARD,
    /** LED */
    MUSB_GHI_ITEM_LED,
    /** Button */
    MUSB_GHI_ITEM_BUTTON,
    /** Ordinal */
    MUSB_GHI_ITEM_ORDINAL,
    /** Physical collection (group of axes) */
    MUSB_GHI_ITEM_COLLECTION_PHYSICAL,
    /** Application collection */
    MUSB_GHI_ITEM_COLLECTION_APPLICATION,
    /** Logical collection */
    MUSB_GHI_ITEM_COLLECTION_LOGICAL,
    /** Report collection */
    MUSB_GHI_ITEM_COLLECTION_REPORT,
    /** Named array */
    MUSB_GHI_ITEM_COLLECTION_NAMED_ARRAY,
    /** Usage switch */
    MUSB_GHI_ITEM_COLLECTION_USAGE_SWITCH,
    /** Usage modifier */
    MUSB_GHI_ITEM_COLLECTION_USAGE_MODIFIER
} MUSB_GhiItemType;

/**
 * MUSB_GhiStatus.
 * GHI Operation Status.
 */
typedef enum
{
    /** success */
    MUSB_GHI_SUCCESS,
    /** error: device characteristics not supported */
    MUSB_GHI_ERROR_UNSUPPORTED_DEVICE,
    /** error: device was removed before operation could complete */
    MUSB_GHI_ERROR_DEVICE_REMOVED,
    /** error: transfer to/from device failed */
    MUSB_GHI_ERROR_TRANSFER
} MUSB_GhiStatus;

/*************************** GHI TYPES ****************************/

/** Handle created by the GHI implementation */
typedef void *MUSB_GhiDeviceHandle;

struct _MUSB_GhiItemInfo;

/**
 * Item information struct.
 * Information about a particular item in a report, control or feature.
 * The bit-manipulation library can be used to insert/extract item data.
 *
 * @field pParentItem pointer to this item's parent,
 * or NULL if this is a top-level item
 *
 * @field wSize item size, in bits
 * [not valid for collection types]
 *
 * @field bRepeatCount how many identical items are present
 * (should be concatenated in report data)
 * [not valid for collection types]
 *
 * @field Type item type
 *
 * @field bId for normal items, this is the report ID.
 * For collection items, this is a driver-generated number
 * so the client can identify different collections.
 *
 * @field bConstant TRUE if constant; FALSE if data
 * [not valid for collection types]
 *
 * @field bVariable TRUE if variable; FALSE if array
 * [not valid for collection types]
 *
 * @field bRelative TRUE if relative; FALSE if absolute
 * [not valid for collection types]
 *
 * @field bWrap TRUE if value wraps; FALSE otherwise
 * [not valid for collection types]
 *
 * @field bNonLinear TRUE if non-linear; FALSE if linear
 * [not valid for collection types]
 *
 * @field bPreferredState TRUE if a preferred state exists; FALSE otherwise
 * [not valid for collection types]
 *
 * @field bNullState TRUE if a null state exists; FALSE otherwise
 * [not valid for collection types]
 *
 * @field bVolatile TRUE if volatile (output/feature only); FALSE otherwise
 * [not valid for collection types]
 *
 * @field bBufferedBytes TRUE if buffered bytes; FALSE if bitfield
 * [not valid for collection types]
 */
struct _MUSB_GhiItemInfo
{
    const struct _MUSB_GhiItemInfo *pParentItem;
    uint16_t wSize;
    uint8_t bId;
    uint8_t bRepeatCount;
    MUSB_GhiItemType Type;
    uint8_t bConstant;
    uint8_t bVariable;
    uint8_t bRelative;
    uint8_t bWrap;
    uint8_t bNonLinear;
    uint8_t bPreferredState;
    uint8_t bNullState;
    uint8_t bVolatile;
    uint8_t bBufferedBytes;
};

/** Typename for _MUSB_GhiItemInfo */
typedef struct _MUSB_GhiItemInfo MUSB_GhiItemInfo;

/**
 * MUSB_GhiDeviceInfo.
 * Device information.
 * A generic HID driver fills this with any information
 * it knows before calling MUSB_GhiAddDevice.
 * The HID report is parsed and "expanded" into individual items.
 *
 * @field aInputItem array of input items
 * @field aOutputItem array of output items
 * @field aFeatureItem array of feature items
 * @field wInputItemCount how many input items are in the array
 * @field wOutputItemCount how many output items are in the array
 * @field wFeatureItemCount how many feature items are in the array
 *
 * @field wLanguage USB language ID
 *
 * @field wVendorId USB VID,
 * for the GHI implementation's use in generating a device name
 *
 * @field wProductId USB PID,
 * for the GHI implementation's use in generating a device name
 *
 * @field bcdDevice USB bcdDevice,
 * for the GHI implementation's use in generating a device name
 *
 * @field bBusAddress USB device address,
 * for the GHI implementation's use in generating a device name
 */
typedef struct
{
    const MUSB_GhiItemInfo *aInputItem;
    const MUSB_GhiItemInfo *aOutputItem;
    const MUSB_GhiItemInfo *aFeatureItem;
    uint16_t wInputItemCount;
    uint16_t wOutputItemCount;
    uint16_t wFeatureItemCount;
    uint16_t wLanguage;
    uint16_t wVendorId;
    uint16_t wProductId;
    uint16_t bcdDevice;
    uint8_t bBusAddress;
} MUSB_GhiDeviceInfo;

/**
 * Input report handler.
 * GHI implementations register this type of handler function
 * for being notified of input reports.
 * The handler may be called frequently,
 * so it should not perform long-running operations.
 * @param hDevice a device handle as filled by a successful MUSB_GhiAddDevice
 * @param pInput input data, with each item's data aligned to a byte boundary
 * @param dwLength actual input data length, in bytes
 */
typedef void (*MUSB_pfGhiInputReport)(MUSB_GhiDeviceHandle hDevice,
                                      const uint8_t *pInput, uint32_t dwLength);

/**
 * Output completion handler.
 * GHI implementations register this type of handler function
 * for being notified of completion of output or feature reports.
 * The handler may be called frequently,
 * so it should not perform long-running operations.
 * @param hDevice a device handle as filled by a successful MUSB_GhiAddDevice
 * @param dwLength actual output data length, in bytes
 */
typedef void (*MUSB_pfGhiOutputComplete)(MUSB_GhiDeviceHandle hDevice,
        uint32_t dwLength);

/**
 * Set the input report handler for a device.
 * This will cause the input pipe to the device to be opened.
 * @param pDeviceData pPrivateData from associated MUSB_GhiDevice
 * @param pfInputHandler pointer to input handler function
 */
typedef void (*MUSB_pfGhiSetInputHandler)(void *pDeviceData,
        MUSB_pfGhiInputReport pfInputHandler);

/**
 * Set the output completion handler for a device.
 * @param pDeviceData pPrivateData from associated MUSB_GhiDevice
 * @param pfOutputCompletionHandler pointer to input handler function
 */
typedef void (*MUSB_pfGhiSetOutputCompletionHandler)(void *pDeviceData,
        MUSB_pfGhiOutputComplete pfOutputCompletionHandler);

/**
 * Set output.
 * Set an output report to the device.
 * @param pDeviceData pPrivateData from associated MUSB_GhiDevice
 * @param bReportId report ID or 0 if not used
 * @param pInputBuffer buffer for input data
 * @param wLength maximum number of bytes to transfer
 * @return status code
 */
typedef MUSB_GhiStatus (*MUSB_pfGhiGetInput)(void *pDeviceData,
        uint8_t bReportId, uint8_t *pInputBuffer, uint16_t wLength);

/**
 * Set output.
 * Set an output report to the device.
 * Only one output or feature request may be pending at any time.
 * @param pDeviceData pPrivateData from associated MUSB_GhiDevice
 * @param bReportId report ID or 0 if not used
 * @param pOutput output data buffer
 * @param wLength number of bytes to transfer
 * @return status code
 */
typedef MUSB_GhiStatus (*MUSB_pfGhiSetOutput)(void *pDeviceData,
        uint8_t bReportId, const uint8_t *pOutput, uint16_t wLength);

/**
 * Set feature.
 * Set a feature to the device.
 * Only one output or feature request may be pending at any time.
 * @param pDeviceData pPrivateData from associated MUSB_GhiDevice
 * @param bReportId report ID or 0 if not used
 * @param pOutput output data buffer
 * @param wLength number of bytes to transfer
 * @return status code
 */
typedef MUSB_GhiStatus (*MUSB_pfGhiSetFeature)(void *pDeviceData,
        uint8_t bReportId, const uint8_t *pOutput, uint16_t wLength);

/**
 * MUSB_GhiDevice.
 * Device driver callbacks.
 * @field pPrivateData device driver private data;
 * not to be interpreted by the GHI implementation
 * @field pfSetInputHandler driver's service to set a device's input handler
 * @field pfGetInput driver's service to request an input report from device
 * @field pfSetOutput driver's service to perform a set-output to a device
 * @field pfSetFeature driver's service to perform a set-feature to a device
 */
typedef struct
{
    void *pPrivateData;
    MUSB_pfGhiSetInputHandler pfSetInputHandler;
    MUSB_pfGhiSetOutputCompletionHandler pfSetOutputCompletionHandler;
    MUSB_pfGhiGetInput pfGetInput;
    MUSB_pfGhiSetOutput pfSetOutput;
    MUSB_pfGhiSetFeature pfSetFeature;
} MUSB_GhiDevice;

/************************* GHI FUNCTIONS **************************/

/**
 * Add a generic HID device.
 * A generic HID driver calls this to inform the GHI implementation
 * that a new device is being used.
 * The GHI implementation uses the device information to determine if
 * it can support the device,
 * and prepare the system to accept position information from it.
 * Error messages on failure are the GHI implementation's responsibility,
 * since the error messaging mechanism is OS-dependent.
 *
 * @param phDevice where to store a device handle on success
 * @param pInfo pointer to device information struct
 * @param pDevice pointer to struct with callbacks to access the device
 * @return MUSB_GHI_SUCCESS on success
 * @return MUSB_GHI_ERROR_UNSUPPORTED_DEVICE if the device cannot
 * be supported (e.g. not enough buttons)
 */
extern MUSB_GhiStatus MUSB_GhiAddDevice(MUSB_GhiDeviceHandle *phDevice,
                                        const MUSB_GhiDeviceInfo *pInfo, MUSB_GhiDevice *pDevice);

/**
 * A generic HID device was removed.
 * A generic HID driver calls this to inform the GHI implementation
 * that the device has been removed.
 * By this time, the driver will have closed the input pipe to the device
 * (and therefore stopped calling the implementation's input report handler).
 * @param hDevice a device handle as filled by a successful MUSB_GhiAddDevice
 */
extern void MUSB_GhiDeviceRemoved(MUSB_GhiDeviceHandle hDevice);

#endif	/* multiple inclusion protection */
