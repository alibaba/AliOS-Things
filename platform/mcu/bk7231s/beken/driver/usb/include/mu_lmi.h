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
 * MUSB-MicroSW Live-Media Interface (LMI).
 * $Revision: 1.4 $
 */

#ifndef __MUSB_LMI_H__
#define __MUSB_LMI_H__

#include "mu_tools.h"

/**
 * Introduction to Live Media Interface.
 * The functions herein can be implemented by an OS to allow
 * the Mentor USB live media drivers to be hooked into the OS'
 * "multimedia" mechanisms, thus allowing the Mentor drivers to be
 * used without additional effort.
 * The callbacks herein are implemented by the Mentor drivers,
 * exposing the functionality needed by an OS to use the device.
 * The above could also be applied to a custom, non-OS environment.
 * Finally, an implementation can be created solely for demonstration purposes
 * (to avoid relying on platforms with live media support,
 * and/or to allow the Mentor USB drivers to be tested).
 */

/************************* LMI CONSTANTS **************************/

/**
 * MUSB_LmiMediaType.
 * LMI Media Type.
 */
typedef enum
{
    /** Unknown media type */
    MUSB_LMI_MEDIA_UNKNOWN,
    /** Audio source */
    MUSB_LMI_MEDIA_AUDIO_SOURCE,
    /** Audio sink */
    MUSB_LMI_MEDIA_AUDIO_SINK,
    /** Video source */
    MUSB_LMI_MEDIA_VIDEO_SOURCE,
    /** Video sink */
    MUSB_LMI_MEDIA_VIDEO_SINK
} MUSB_LmiMediaType;

/**
 * MUSB_LmiChannelType.
 * LMI Channel Type.
 */
typedef enum
{
    /** Unknown channel type */
    MUSB_LMI_CHANNEL_UNKNOWN,
    /** Sole channel, or center of e.g. a 5-speaker set */
    MUSB_LMI_CHANNEL_CENTER,
    /** Left channel of a stereo or better arrangement */
    MUSB_LMI_CHANNEL_LEFT,
    /** Right channel of a stereo or better arrangement */
    MUSB_LMI_CHANNEL_RIGHT,
    /** Left forward channel of e.g. a 5- or 6-speaker set */
    MUSB_LMI_CHANNEL_LEFT_FORWARD,
    /** Right forward channel of e.g. a 5- or 6-speaker set */
    MUSB_LMI_CHANNEL_RIGHT_FORWARD,
    /** Left rear channel of e.g. a 5- or 6-speaker set */
    MUSB_LMI_CHANNEL_LEFT_REAR,
    /** Right rear channel of e.g. a 5- or 6-speaker set */
    MUSB_LMI_CHANNEL_RIGHT_REAR,
    /** Upper channel of e.g. a triangular camera arrangement */
    MUSB_LMI_CHANNEL_UPPER
} MUSB_LmiChannelType;

/**
 * MUSB_LmiControlType.
 * LMI control type.
 */
typedef enum
{
    /** Control type not enumerated; use GetName for presentation */
    MUSB_LMI_CONTROL_UNKNOWN,
    /** Volume */
    MUSB_LMI_CONTROL_VOLUME,
    /** Balance */
    MUSB_LMI_CONTROL_BALANCE,
    /** Front/rear fade */
    MUSB_LMI_CONTROL_FADER,
    /** Delay */
    MUSB_LMI_CONTROL_DELAY,
    /** Loudness */
    MUSB_LMI_CONTROL_LOUDNESS,
    /** Contrast */
    MUSB_LMI_CONTROL_CONTRAST,
    /** Brightness */
    MUSB_LMI_CONTROL_BRIGHTNESS,
    /** Hue */
    MUSB_LMI_CONTROL_HUE,
    /** Saturation */
    MUSB_LMI_CONTROL_SATURATION
} MUSB_LmiControlType;

/**
 * MUSB_LmiColorEncoding.
 * LMI color encoding.
 */
typedef enum
{
    /** No color information available */
    MUSB_LMI_COLOR_NONE,
    /** RGB (Red-Green-Blue) encoding (TODO: MSB/LSB) */
    MUSB_LMI_COLOR_RGB,
    /** YUV (TODO: define) */
    MUSB_LMI_COLOR_YUV
} MUSB_LmiColorEncoding;

/**
 * MUSB_LmiNameQuery.
 * LMI name query.
 */
typedef enum
{
    /** Device vendor name */
    MUSB_LMI_NAME_DEVICE_VENDOR,
    /** Device product name */
    MUSB_LMI_NAME_DEVICE_PRODUCT,
    /** A global audio control */
    MUSB_LMI_NAME_AUDIO_CONTROL,
    /** An audio channel */
    MUSB_LMI_NAME_AUDIO_CHANNEL,
    /** A channel-specific audio control */
    MUSB_LMI_NAME_AUDIO_CHANNEL_CONTROL,
    /** An audio sampling format */
    MUSB_LMI_NAME_AUDIO_FORMAT,
    /** A global video control */
    MUSB_LMI_NAME_VIDEO_CONTROL,
    /** A video channel */
    MUSB_LMI_NAME_VIDEO_CHANNEL,
    /** A channel-specific video control */
    MUSB_LMI_NAME_VIDEO_CHANNEL_CONTROL,
    /** A video sampling format */
    MUSB_LMI_NAME_VIDEO_FORMAT
} MUSB_LmiNameQuery;

/**
 * MUSB_LmiStatus.
 * LMI Operation Status.
 */
typedef enum
{
    /** success */
    MUSB_LMI_SUCCESS,
    /** error: unsupported name */
    MUSB_LMI_ERROR_UNSUPPORTED_NAME,
    /** error: unsupported language */
    MUSB_LMI_ERROR_UNSUPPORTED_LANGUAGE,
    /** error: device was removed before operation could complete */
    MUSB_LMI_ERROR_DEVICE_REMOVED,
    /** error: transfer to/from device failed */
    MUSB_LMI_ERROR_TRANSFER,
    /** error: device busy */
    MUSB_LMI_ERROR_BUSY
} MUSB_LmiStatus;

/*************************** LMI TYPES ****************************/

/** Handle created by the LMI implementation */
typedef void *MUSB_LmiDeviceHandle;

/**
 * MUSB_LmiControlCapability.
 * A capability of a Control.
 * @field ControlType type of control
 * @field bMagnitude the number of significant bits in the control's value.
 * Control values are in abstract scaled units, expressed as unsigned numbers.
 * If a control has a natural "center" (e.g. balance),
 * its value corresponds to the number with MSb=0 and other bits=1
 * (e.g. 127 for an 8-bit-magnitude control).
 */
typedef struct
{
    MUSB_LmiControlType ControlType;
    uint8_t bMagnitude;
} MUSB_LmiControlCapability;

/**
 * MUSB_LmiChannelInfo.
 * Channel information.
 * @field MediaType the underlying medium's type
 * @field ChannelType the channel type
 * @field bControlCount number of controls particular to this channel
 * @field aControlCapabilities array of bControlCount elements,
 * each giving the capabilities of a particular control
 */
typedef struct
{
    MUSB_LmiMediaType MediaType;
    MUSB_LmiChannelType ChannelType;
    uint8_t bControlCount;
    const MUSB_LmiControlCapability *aControlCapabilities;
} MUSB_LmiChannelInfo;

/**
 * MUSB_LmiAudioSamplingFormat.
 * Audio sampling format.
 * @field dwSamplingRate samples per second for all channels
 * @field bSampleMagnitude bits per sample per channel
 */
typedef struct
{
    uint32_t dwSamplingRate;
    uint8_t bSampleMagnitude;
} MUSB_LmiAudioSamplingFormat;

/**
 * MUSB_LmiAudioSamplingCapabilities.
 * Capabilities for transferring real-time audio samples.
 * @field bChannelCount number of channels
 * @field aChannelInfo array with bChannelCount elements,
 * each with information about a channel
 * @field bSamplingFormatCount the number of sampling formats supported
 * @field aSamplingFormats array of bSamplingFormatCount elements,
 * each giving a possible sampling configuration for the channel cluster
 * @field bControlCount the number of controls which apply to the cluster
 * @field aControlCapabilities array of bControlCount elements,
 * each giving the capabilities of a particular control
 */
typedef struct
{
    uint8_t bChannelCount;
    const MUSB_LmiChannelInfo *aChannelInfo;
    uint8_t bSamplingFormatCount;
    const MUSB_LmiAudioSamplingFormat *aSamplingFormats;
    uint8_t bControlCount;
    const MUSB_LmiControlCapability *aControlCapabilities;
} MUSB_LmiAudioSamplingCapabilities;

/**
 * MUSB_LmiVideoSamplingFormat.
 * Video sampling format.
 * @field wFrameRate frames per second
 * @field dwHorizontalPixels how many horizontal pixels are sampled
 * @field dwVerticalPixels how many vertical pixels are sampled
 * @field ColorEncoding the color encoding scheme
 * @field bPixelMagnitude bits per pixel
 * @field bElementMagnitude bits per color element
 */
typedef struct
{
    uint16_t wFrameRate;
    uint32_t dwHorizontalPixels;
    uint32_t dwVerticalPixels;
    MUSB_LmiColorEncoding ColorEncoding;
    uint8_t bPixelMagnitude;
    uint8_t bElementMagnitude;
} MUSB_LmiVideoSamplingFormat;

/**
 * MUSB_LmiVideoSamplingCapabilities.
 * Capabilities for transferring real-time video samples.
 * @field bChannelCount number of channels
 * @field aChannelInfo array with bChannelCount elements,
 * each with information about a channel
 * @field bSamplingFormatCount the number of sampling formats supported
 * @field aSamplingFormats array of bSamplingFormatCount elements,
 * each giving a possible sampling configuration for the channel cluster
 * @field bControlCount the number of controls which apply to the cluster
 * @field aControlCapabilities array of bControlCount elements,
 * each giving the capabilities of a particular control
 */
typedef struct
{
    uint8_t bChannelCount;
    const MUSB_LmiChannelInfo *aChannelInfo;
    uint8_t bSamplingRateCount;
    const MUSB_LmiVideoSamplingFormat *aSamplingFormats;
    uint8_t bControlCount;
    const MUSB_LmiControlCapability *aControlCapabilities;
} MUSB_LmiVideoSamplingCapabilities;

/**
 * Transfer progress callback.
 * @param hDevice LMI implementation handle
 * @param Status on any error that effectively stops the transfer,
 * the associated status code; otherwise the success code
 * @param wSamplesTransferred the number of samples transferred
 */
typedef void (*MUSB_pfLmiTransferProgress)(MUSB_LmiDeviceHandle hDevice,
        MUSB_LmiStatus Status, uint16_t wSamplesTransferred);

/**
 * MUSB_LmiTransfer.
 * Sample transfer control/status.
 * @field wSampleCount the number of samples/frames intended for transfer to/from the buffer
 * @field wCallbackInterval how often, in samples, to issue the callback
 * (it is always issued when all samples are transferred or on a fatal error)
 * @field pfTransferProgress callback
 * @field pBuffer the buffer
 */
typedef struct
{
    uint16_t wSampleCount;
    uint16_t wCallbackInterval;
    MUSB_pfLmiTransferProgress pfTransferProgress;
    uint8_t *pBuffer;
} MUSB_LmiTransfer;

/**
 * MUSB_LmiDeviceInfo.
 * Device information.
 * A live media device driver fills this with any information
 * it knows before calling MUSB_LmiAddDevice.
 *
 * @field pAudioSamplingCapabilities pointer to struct giving
 * audio sampling capabilities, or NULL if the device has no audio capability
 *
 * @field pVideoSamplingCapabilities pointer to struct giving
 * video sampling capabilities, or NULL if the device has no video capability
 *
 * @field wVendorId USB VID,
 * for the LMI implementation's use in generating channel names
 *
 * @field wProductId USB PID,
 * for the LMI implementation's use in generating channel names
 *
 * @field bcdDevice USB bcdDevice,
 * for the LMI implementation's use in generating channel names
 *
 * @field bBusAddress USB device address,
 * for the LMI implementation's use in generating channel names
 */
typedef struct
{
    const MUSB_LmiAudioSamplingCapabilities *pAudioSamplingCapabilities;
    const MUSB_LmiVideoSamplingCapabilities *pVideoSamplingCapabilities;
    uint16_t wVendorId;
    uint16_t wProductId;
    uint16_t bcdDevice;
    uint8_t bBusAddress;
} MUSB_LmiDeviceInfo;

/**
 * Get an LMI entity's name.
 * Get a name associated with an LMI device, channel, sampling format, etc.
 * @param pDeviceData pPrivateData from associated MUSB_LmiDevice
 * @param NameQuery indicates which name is requested
 * @param bIndex the index of the control or format whose name is requested
 * @param bChannel for channel-specific queries,
 * the index of the associated channel
 * @param wLanguageCode a language/country code as per USB 2.0
 * @param pName storage for a UNICODE name,
 * filled and terminated with a wide-NUL on success
 * @param bNameLength size, in 16-bit UNICODE characters, of the pName buffer
 * @return status code
 */
typedef MUSB_LmiStatus (*MUSB_pfLmiGetName)(void *pDeviceData,
        MUSB_LmiNameQuery NameQuery, uint8_t bIndex, uint8_t bChannel,
        uint16_t wLanguageCode, uint16_t *pName, uint8_t bNameLength);

/**
 * Get the current value for an audio control
 * @param pDeviceData pPrivateData from associated MUSB_LmiDevice
 * @param bIndex the index of the global or channel-specific control
 * @param bChannel for channel-specific controls,
 * the index of the associated channel
 * @param pValue pointer to the number of bytes required to contain
 * the number of bits given in the control capability's bMagnitude.
 * For example, a 9-bit-magnitude control needs two bytes; no more, no less.
 * On success, an unsigned number in the host's byte order is stored here.
 */
typedef MUSB_LmiStatus (*MUSB_pfLmiGetAudioControl)(void *pDeviceData,
        uint8_t bIndex, uint8_t bChannel, uint8_t *pValue);

/**
 * Set a new value for an audio control
 * @param pDeviceData pPrivateData from associated MUSB_LmiDevice
 * @param bIndex the index of the global or channel-specific control
 * @param bChannel for channel-specific controls,
 * the index of the associated channel
 * @param pValue pointer to the new value in host byte order
 */
typedef MUSB_LmiStatus (*MUSB_pfLmiSetAudioControl)(void *pDeviceData,
        uint8_t bIndex, uint8_t bChannel, const uint8_t *pValue);

/**
 * Get the current value for a video control
 * @param pDeviceData pPrivateData from associated MUSB_LmiDevice
 * @param bIndex the index of the global or channel-specific control
 * @param bChannel for channel-specific controls,
 * the index of the associated channel
 * @param pValue pointer to the number of bytes required to contain
 * the number of bits given in the control capability's bMagnitude.
 * For example, a 9-bit-magnitude control needs two bytes; no more, no less.
 * On success, an unsigned number in the host's byte order is stored here.
 */
typedef MUSB_LmiStatus (*MUSB_pfLmiGetVideoControl)(void *pDeviceData,
        uint8_t bIndex, uint8_t bChannel, uint8_t *pValue);

/**
 * Set a new value for a video control
 * @param pDeviceData pPrivateData from associated MUSB_LmiDevice
 * @param bIndex the index of the global or channel-specific control
 * @param bChannel for channel-specific controls,
 * the index of the associated channel
 * @param pValue pointer to the new value in host byte order
 */
typedef MUSB_LmiStatus (*MUSB_pfLmiSetVideoControl)(void *pDeviceData,
        uint8_t bIndex, uint8_t bChannel, const uint8_t *pValue);

/**
 * Begin transfers to/from a live-media device.
 * @param pDeviceData pPrivateData from associated MUSB_LmiDevice
 * @return status code
 */
typedef MUSB_LmiStatus (*MUSB_pfLmiStartTransfer)(void *pDeviceData,
        MUSB_LmiTransfer *pAudioTransfer, MUSB_LmiTransfer *pVideoTransfer);

/**
 * Stop any pending transfer to/from a live-media device.
 * @param pDeviceData pPrivateData from associated MUSB_LmiDevice
 * @return status code
 */
typedef MUSB_LmiStatus (*MUSB_pfLmiStopTransfer)(void *pDeviceData);

/**
 * MUSB_LmiDevice.
 * Device driver callbacks (dispatch table).
 * @field pPrivateData device driver private data;
 * not to be interpreted by the LMI implementation
 * @field pfGetName get-name callback
 * @field pfGetAudioControl get-audio-control callback
 * @field pfSetAudioControl set-audio-control callback
 * @field pfGetVideoControl get-video-control callback
 * @field pfSetVideoControl set-video-control callback
 * @field pfLmiStartTransfer transfer-start callback
 * @field pfLmiStartTransfer transfer-stop callback
 */
typedef struct
{
    void *pPrivateData;
    MUSB_pfLmiGetName pfGetName;
    MUSB_pfLmiGetAudioControl pfGetAudioControl;
    MUSB_pfLmiSetAudioControl pfSetAudioControl;
    MUSB_pfLmiGetVideoControl pfGetVideoControl;
    MUSB_pfLmiSetVideoControl pfSetVideoControl;
    MUSB_pfLmiStartTransfer pfLmiStartTransfer;
    MUSB_pfLmiStopTransfer pfLmiStopTransfer;
} MUSB_LmiDevice;

/************************* LMI FUNCTIONS **************************/

/**
 * Add a live media device.
 * An live media device driver calls this to inform the LMI implementation
 * that a new device is available.
 * The LMI implementation uses the device information to determine if
 * it can support the device, and make its channels known to the system.
 * Error messages on failure are the LMI implementation's responsibility,
 * since the error messaging mechanism is OS-dependent.
 *
 * @param phDevice where to store a device handle on success
 * @param pInfo pointer to device information struct
 * @param pDevice pointer to struct with callbacks to access the device
 * @return status code
 */
extern MUSB_LmiStatus MUSB_LmiAddDevice(MUSB_LmiDeviceHandle *phDevice,
                                        const MUSB_LmiDeviceInfo *pInfo, MUSB_LmiDevice *pDevice);

/**
 * A live media device was removed.
 * A live media device driver calls this to inform the LMI implementation
 * that a device has been removed
 * @param hDevice a device handle as filled by a successful MUSB_LmiAddDevice
 */
extern void MUSB_LmiDeviceRemoved(MUSB_LmiDeviceHandle hDevice);

#endif	/* multiple inclusion protection */
