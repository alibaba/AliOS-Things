/*
 * USB Video Class definitions.
 *
 * This file holds USB constants and structures defined by the USB Device
 * Class Definition for Video Devices. Unless otherwise stated, comments
 * below reference relevant sections of the USB Video Class 1.1 specification
 * available at
 *
 * http://www.usb.org/developers/devclass_docs/USB_Video_Class_1_1.zip
 */

#ifndef _USB_VIDEO_H_
#define _USB_VIDEO_H_

/* --------------------------------------------------------------------------
 * UVC constants
 */
/* A.2. Video Interface Subclass Codes */
#define UVC_SC_UNDEFINED				    0x00
#define UVC_SC_VIDEOCONTROL				    0x01
#define UVC_SC_VIDEOSTREAMING				0x02
#define UVC_SC_VIDEO_INTERFACE_COLLECTION	0x03

/* A.3. Video Interface Protocol Codes */
#define UVC_PC_PROTOCOL_UNDEFINED			0x00
#define UVC_PC_PROTOCOL_15				    0x01

/* A.5. Video Class-Specific VC Interface Descriptor Subtypes */
#define UVC_VC_DESCRIPTOR_UNDEFINED			0x00
#define UVC_VC_HEADER					    0x01
#define UVC_VC_INPUT_TERMINAL				0x02
#define UVC_VC_OUTPUT_TERMINAL				0x03
#define UVC_VC_SELECTOR_UNIT				0x04
#define UVC_VC_PROCESSING_UNIT				0x05
#define UVC_VC_EXTENSION_UNIT				0x06

/* A.6. Video Class-Specific VS Interface Descriptor Subtypes */
#define UVC_VS_UNDEFINED				    0x00
#define UVC_VS_INPUT_HEADER				    0x01
#define UVC_VS_OUTPUT_HEADER				0x02
#define UVC_VS_STILL_IMAGE_FRAME			0x03
#define UVC_VS_FORMAT_UNCOMPRESSED			0x04
#define UVC_VS_FRAME_UNCOMPRESSED			0x05
#define UVC_VS_FORMAT_MJPEG				    0x06
#define UVC_VS_FRAME_MJPEG				    0x07
#define UVC_VS_FORMAT_MPEG2TS				0x0a
#define UVC_VS_FORMAT_DV				    0x0c
#define UVC_VS_COLORFORMAT				    0x0d
#define UVC_VS_FORMAT_FRAME_BASED			0x10
#define UVC_VS_FRAME_FRAME_BASED			0x11
#define UVC_VS_FORMAT_STREAM_BASED			0x12

/* A.7. Video Class-Specific Endpoint Descriptor Subtypes */
#define UVC_EP_UNDEFINED				    0x00
#define UVC_EP_GENERAL					    0x01
#define UVC_EP_ENDPOINT					    0x02
#define UVC_EP_INTERRUPT				    0x03

/* A.8. Video Class-Specific Request Codes
    ? Current setting attribute (GET_CUR)
    ? Minimum setting attribute (GET_MIN)
    ? Maximum setting attribute (GET_MAX)
    ? Default setting attribute (GET_DEF)
    ? Resolution attribute      (GET_RES)
    ? Data length attribute     (GET_LEN)
    ? Information attribute     (GET_INFO)
    ? Current setting attribute (SET_CUR)
*/
#define UVC_RC_UNDEFINED		        	0x00
#define UVC_SET_CUR				        	0x01
#define UVC_GET_CUR				        	0x81
#define UVC_GET_MIN				        	0x82
#define UVC_GET_MAX				        	0x83
#define UVC_GET_RES				        	0x84
#define UVC_GET_LEN				        	0x85
#define UVC_GET_INFO			        	0x86
#define UVC_GET_DEF				        	0x87

/* A.9.1. VideoControl Interface Control Selectors */
#define UVC_VC_CONTROL_UNDEFINED			    0x00
#define UVC_VC_VIDEO_POWER_MODE_CONTROL			0x01
#define UVC_VC_REQUEST_ERROR_CODE_CONTROL		0x02

/* A.9.2. Terminal Control Selectors */
#define UVC_TE_CONTROL_UNDEFINED			    0x00

/* A.9.3. Selector Unit Control Selectors */
#define UVC_SU_CONTROL_UNDEFINED			    0x00
#define UVC_SU_INPUT_SELECT_CONTROL			    0x01

/* A.9.4. Camera Terminal Control Selectors */
#define UVC_CT_CONTROL_UNDEFINED			    0x00
#define UVC_CT_SCANNING_MODE_CONTROL			0x01
#define UVC_CT_AE_MODE_CONTROL				    0x02
#define UVC_CT_AE_PRIORITY_CONTROL			    0x03
#define UVC_CT_EXPOSURE_TIME_ABSOLUTE_CONTROL	0x04
#define UVC_CT_EXPOSURE_TIME_RELATIVE_CONTROL	0x05
#define UVC_CT_FOCUS_ABSOLUTE_CONTROL			0x06
#define UVC_CT_FOCUS_RELATIVE_CONTROL			0x07
#define UVC_CT_FOCUS_AUTO_CONTROL			    0x08
#define UVC_CT_IRIS_ABSOLUTE_CONTROL			0x09
#define UVC_CT_IRIS_RELATIVE_CONTROL			0x0a
#define UVC_CT_ZOOM_ABSOLUTE_CONTROL			0x0b
#define UVC_CT_ZOOM_RELATIVE_CONTROL			0x0c
#define UVC_CT_PANTILT_ABSOLUTE_CONTROL			0x0d
#define UVC_CT_PANTILT_RELATIVE_CONTROL			0x0e
#define UVC_CT_ROLL_ABSOLUTE_CONTROL			0x0f
#define UVC_CT_ROLL_RELATIVE_CONTROL			0x10
#define UVC_CT_PRIVACY_CONTROL				    0x11

/* A.9.5. Processing Unit Control Selectors */
#define UVC_PU_CONTROL_UNDEFINED			        0x00
#define UVC_PU_BACKLIGHT_COMPENSATION_CONTROL		0x01
#define UVC_PU_BRIGHTNESS_CONTROL			        0x02
#define UVC_PU_CONTRAST_CONTROL				        0x03
#define UVC_PU_GAIN_CONTROL				            0x04
#define UVC_PU_POWER_LINE_FREQUENCY_CONTROL	    	0x05
#define UVC_PU_HUE_CONTROL				            0x06
#define UVC_PU_SATURATION_CONTROL			        0x07
#define UVC_PU_SHARPNESS_CONTROL			        0x08
#define UVC_PU_GAMMA_CONTROL				        0x09
#define UVC_PU_WHITE_BALANCE_TEMPERATURE_CONTROL	0x0a
#define UVC_PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL	0x0b
#define UVC_PU_WHITE_BALANCE_COMPONENT_CONTROL		    0x0c
#define UVC_PU_WHITE_BALANCE_COMPONENT_AUTO_CONTROL	    0x0d
#define UVC_PU_DIGITAL_MULTIPLIER_CONTROL		        0x0e
#define UVC_PU_DIGITAL_MULTIPLIER_LIMIT_CONTROL	    	0x0f
#define UVC_PU_HUE_AUTO_CONTROL				            0x10
#define UVC_PU_ANALOG_VIDEO_STANDARD_CONTROL	    	0x11
#define UVC_PU_ANALOG_LOCK_STATUS_CONTROL		        0x12

/* A.9.7. VideoStreaming Interface Control Selectors */
#define UVC_VS_CONTROL_UNDEFINED			    0x00
#define UVC_VS_PROBE_CONTROL				    0x01
#define UVC_VS_COMMIT_CONTROL				    0x02
#define UVC_VS_STILL_PROBE_CONTROL			    0x03
#define UVC_VS_STILL_COMMIT_CONTROL		    	0x04
#define UVC_VS_STILL_IMAGE_TRIGGER_CONTROL		0x05
#define UVC_VS_STREAM_ERROR_CODE_CONTROL		0x06
#define UVC_VS_GENERATE_KEY_FRAME_CONTROL		0x07
#define UVC_VS_UPDATE_FRAME_SEGMENT_CONTROL		0x08
#define UVC_VS_SYNC_DELAY_CONTROL			    0x09

/* B.1. USB Terminal Types */
#define UVC_TT_VENDOR_SPECIFIC			    	0x0100
#define UVC_TT_STREAMING				        0x0101

/* B.2. Input Terminal Types */
#define UVC_ITT_VENDOR_SPECIFIC			    	0x0200
#define UVC_ITT_CAMERA					        0x0201
#define UVC_ITT_MEDIA_TRANSPORT_INPUT			0x0202

/* B.3. Output Terminal Types */
#define UVC_OTT_VENDOR_SPECIFIC			    	0x0300
#define UVC_OTT_DISPLAY					        0x0301
#define UVC_OTT_MEDIA_TRANSPORT_OUTPUT			0x0302

/* B.4. External Terminal Types */
#define UVC_EXTERNAL_VENDOR_SPECIFIC			0x0400
#define UVC_COMPOSITE_CONNECTOR			    	0x0401
#define UVC_SVIDEO_CONNECTOR				    0x0402
#define UVC_COMPONENT_CONNECTOR				    0x0403

/* 2.4.2.2. Status Packet Type */
#define UVC_STATUS_TYPE_CONTROL				1
#define UVC_STATUS_TYPE_STREAMING			2

/* 2.4.3.3. Payload Header Information */
#define UVC_STREAM_EOH					(1 << 7)
#define UVC_STREAM_ERR					(1 << 6)
#define UVC_STREAM_STI					(1 << 5)
#define UVC_STREAM_RES					(1 << 4)
#define UVC_STREAM_SCR					(1 << 3)
#define UVC_STREAM_PTS					(1 << 2)
#define UVC_STREAM_EOF					(1 << 1)
#define UVC_STREAM_FID					(1 << 0)

/* 4.1.2. Control Capabilities */
#define UVC_CONTROL_CAP_GET				        (1 << 0)
#define UVC_CONTROL_CAP_SET				        (1 << 1)
#define UVC_CONTROL_CAP_DISABLED			    (1 << 2)
#define UVC_CONTROL_CAP_AUTOUPDATE			    (1 << 3)
#define UVC_CONTROL_CAP_ASYNCHRONOUS			(1 << 4)

/* ------------------------------------------------------------------------
 * UVC structures
 */
/* All UVC descriptors have these 3 fields at the beginning */
struct uvc_descriptor_header
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubType;
} __attribute__((packed));

/* 3.7.2. Video Control Interface Header Descriptor */
struct uvc_header_descriptor
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubType;
    uint16_t bcdUVC;
    uint16_t wTotalLength;
    uint32_t dwClockFrequency;
    uint8_t  bInCollection;
    uint8_t  baInterfaceNr[];
} __attribute__((__packed__));

#define UVC_DT_HEADER_SIZE(n)				(12+(n))

#define UVC_HEADER_DESCRIPTOR(n) \
	uvc_header_descriptor_##n

#define DECLARE_UVC_HEADER_DESCRIPTOR(n)		\
struct UVC_HEADER_DESCRIPTOR(n) {			\
	uint8_t  bLength;					\
	uint8_t  bDescriptorType;				\
	uint8_t  bDescriptorSubType;			\
	uint16_t bcdUVC;					\
	uint16_t wTotalLength;				\
	uint32_t dwClockFrequency;				\
	uint8_t  bInCollection;				\
	uint8_t  baInterfaceNr[n];				\
} __attribute__ ((packed))

/* 3.7.2.1. Input Terminal Descriptor */
struct uvc_input_terminal_descriptor
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubType;
    uint8_t  bTerminalID;
    uint16_t wTerminalType;
    uint8_t  bAssocTerminal;
    uint8_t  iTerminal;
} __attribute__((__packed__));

#define UVC_DT_INPUT_TERMINAL_SIZE			8

/* 3.7.2.2. Output Terminal Descriptor */
struct uvc_output_terminal_descriptor
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubType;
    uint8_t  bTerminalID;
    uint16_t wTerminalType;
    uint8_t  bAssocTerminal;
    uint8_t  bSourceID;
    uint8_t  iTerminal;
} __attribute__((__packed__));

#define UVC_DT_OUTPUT_TERMINAL_SIZE			9

/* 3.7.2.3. Camera Terminal Descriptor */
struct uvc_camera_terminal_descriptor
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubType;
    uint8_t  bTerminalID;
    uint16_t wTerminalType;
    uint8_t  bAssocTerminal;
    uint8_t  iTerminal;
    uint16_t wObjectiveFocalLengthMin;
    uint16_t wObjectiveFocalLengthMax;
    uint16_t wOcularFocalLength;
    uint8_t  bControlSize;
    uint8_t  bmControls[3];
} __attribute__((__packed__));

#define UVC_DT_CAMERA_TERMINAL_SIZE(n)			(15+(n))

/* 3.7.2.4. Selector Unit Descriptor */
struct uvc_selector_unit_descriptor
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubType;
    uint8_t  bUnitID;
    uint8_t  bNrInPins;
    uint8_t  baSourceID[1];
    uint8_t  iSelector;
} __attribute__((__packed__));

#define UVC_DT_SELECTOR_UNIT_SIZE(n)			(6+(n))

#define UVC_SELECTOR_UNIT_DESCRIPTOR(n)	\
	uvc_selector_unit_descriptor_##n

#define DECLARE_UVC_SELECTOR_UNIT_DESCRIPTOR(n)	\
struct UVC_SELECTOR_UNIT_DESCRIPTOR(n) {		\
	uint8_t  bLength;					\
	uint8_t  bDescriptorType;				\
	uint8_t  bDescriptorSubType;			\
	uint8_t  bUnitID;					\
	uint8_t  bNrInPins;				\
	uint8_t  baSourceID[n];				\
	uint8_t  iSelector;				\
} __attribute__ ((packed))

/* 3.7.2.5. Processing Unit Descriptor */
struct uvc_processing_unit_descriptor
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubType;
    uint8_t  bUnitID;
    uint8_t  bSourceID;
    uint16_t wMaxMultiplier;
    uint8_t  bControlSize;
    uint8_t  bmControls[2];
    uint8_t  iProcessing;
} __attribute__((__packed__));

#define UVC_DT_PROCESSING_UNIT_SIZE(n)			(9+(n))

/* 3.7.2.6. Extension Unit Descriptor */
struct uvc_extension_unit_descriptor
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubType;
    uint8_t  bUnitID;
    uint8_t  guidExtensionCode[16];
    uint8_t  bNumControls;
    uint8_t  bNrInPins;
    uint8_t  baSourceID[1];
    uint8_t  bControlSize;
    uint8_t  bmControls[1];
    uint8_t  iExtension;
} __attribute__((__packed__));

#define UVC_DT_EXTENSION_UNIT_SIZE(p, n)		(24+(p)+(n))

#define UVC_EXTENSION_UNIT_DESCRIPTOR(p, n) \
	uvc_extension_unit_descriptor_##p_##n

#define DECLARE_UVC_EXTENSION_UNIT_DESCRIPTOR(p, n)	\
struct UVC_EXTENSION_UNIT_DESCRIPTOR(p, n) {		\
	uint8_t  bLength;					\
	uint8_t  bDescriptorType;				\
	uint8_t  bDescriptorSubType;			\
	uint8_t  bUnitID;					\
	uint8_t  guidExtensionCode[16];			\
	uint8_t  bNumControls;				\
	uint8_t  bNrInPins;				\
	uint8_t  baSourceID[p];				\
	uint8_t  bControlSize;				\
	uint8_t  bmControls[n];				\
	uint8_t  iExtension;				\
} __attribute__ ((packed))

/* 3.8.2.2. Video Control Interrupt Endpoint Descriptor */
struct uvc_control_endpoint_descriptor
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubType;
    uint16_t wMaxTransferSize;
} __attribute__((__packed__));

#define UVC_DT_CONTROL_ENDPOINT_SIZE			5

/* 3.9.2.1. Input Header Descriptor */
struct uvc_input_header_descriptor
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubType;
    uint8_t  bNumFormats;
    uint16_t wTotalLength;
    uint8_t  bEndpointAddress;
    uint8_t  bmInfo;
    uint8_t  bTerminalLink;
    uint8_t  bStillCaptureMethod;
    uint8_t  bTriggerSupport;
    uint8_t  bTriggerUsage;
    uint8_t  bControlSize;
    uint8_t  bmaControls[];
} __attribute__((__packed__));

#define UVC_DT_INPUT_HEADER_SIZE(n, p)			(13+(n*p))

#define UVC_INPUT_HEADER_DESCRIPTOR(n, p) \
	uvc_input_header_descriptor_##n_##p

#define DECLARE_UVC_INPUT_HEADER_DESCRIPTOR(n, p)	\
struct UVC_INPUT_HEADER_DESCRIPTOR(n, p) {		\
	uint8_t  bLength;					\
	uint8_t  bDescriptorType;				\
	uint8_t  bDescriptorSubType;			\
	uint8_t  bNumFormats;				\
	uint16_t wTotalLength;				\
	uint8_t  bEndpointAddress;				\
	uint8_t  bmInfo;					\
	uint8_t  bTerminalLink;				\
	uint8_t  bStillCaptureMethod;			\
	uint8_t  bTriggerSupport;				\
	uint8_t  bTriggerUsage;				\
	uint8_t  bControlSize;				\
	uint8_t  bmaControls[p][n];			\
} __attribute__ ((packed))

/* 3.9.2.2. Output Header Descriptor */
struct uvc_output_header_descriptor
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubType;
    uint8_t  bNumFormats;
    uint16_t wTotalLength;
    uint8_t  bEndpointAddress;
    uint8_t  bTerminalLink;
    uint8_t  bControlSize;
    uint8_t  bmaControls[];
} __attribute__((__packed__));

#define UVC_DT_OUTPUT_HEADER_SIZE(n, p)			(9+(n*p))

#define UVC_OUTPUT_HEADER_DESCRIPTOR(n, p) \
	uvc_output_header_descriptor_##n_##p

#define DECLARE_UVC_OUTPUT_HEADER_DESCRIPTOR(n, p)	\
struct UVC_OUTPUT_HEADER_DESCRIPTOR(n, p) {		\
	uint8_t  bLength;					\
	uint8_t  bDescriptorType;				\
	uint8_t  bDescriptorSubType;			\
	uint8_t  bNumFormats;				\
	uint16_t wTotalLength;				\
	uint8_t  bEndpointAddress;				\
	uint8_t  bTerminalLink;				\
	uint8_t  bControlSize;				\
	uint8_t  bmaControls[p][n];			\
} __attribute__ ((packed))

/* 3.9.2.6. Color matching descriptor */
struct uvc_color_matching_descriptor
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubType;
    uint8_t  bColorPrimaries;
    uint8_t  bTransferCharacteristics;
    uint8_t  bMatrixCoefficients;
} __attribute__((__packed__));

#define UVC_DT_COLOR_MATCHING_SIZE			6

/* 4.3.1.1. Video Probe and Commit Controls */
struct uvc_streaming_control
{
    uint16_t bmHint;
    uint8_t  bFormatIndex;
    uint8_t  bFrameIndex;
    uint32_t dwFrameInterval;
    uint16_t wKeyFrameRate;
    uint16_t wPFrameRate;
    uint16_t wCompQuality;
    uint16_t wCompWindowSize;
    uint16_t wDelay;
    uint32_t dwMaxVideoFrameSize;
    uint32_t dwMaxPayloadTransferSize;
    uint32_t dwClockFrequency;
    uint8_t  bmFramingInfo;
    uint8_t  bPreferedVersion;
    uint8_t  bMinVersion;
    uint8_t  bMaxVersion;
} __attribute__((__packed__));

/* Uncompressed Payload - 3.1.1. Uncompressed Video Format Descriptor */
struct uvc_format_uncompressed
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubType;
    uint8_t  bFormatIndex;
    uint8_t  bNumFrameDescriptors;
    uint8_t  guidFormat[16];
    uint8_t  bBitsPerPixel;
    uint8_t  bDefaultFrameIndex;
    uint8_t  bAspectRatioX;
    uint8_t  bAspectRatioY;
    uint8_t  bmInterfaceFlags;
    uint8_t  bCopyProtect;
} __attribute__((__packed__));

#define UVC_DT_FORMAT_UNCOMPRESSED_SIZE			27

/* Uncompressed Payload - 3.1.2. Uncompressed Video Frame Descriptor */
struct uvc_frame_uncompressed
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubType;
    uint8_t  bFrameIndex;
    uint8_t  bmCapabilities;
    uint16_t wWidth;
    uint16_t wHeight;
    uint32_t dwMinBitRate;
    uint32_t dwMaxBitRate;
    uint32_t dwMaxVideoFrameBufferSize;
    uint32_t dwDefaultFrameInterval;
    uint8_t  bFrameIntervalType;
    uint32_t dwFrameInterval[];
} __attribute__((__packed__));

#define UVC_DT_FRAME_UNCOMPRESSED_SIZE(n)		(26+4*(n))

#define UVC_FRAME_UNCOMPRESSED(n) \
	uvc_frame_uncompressed_##n

#define DECLARE_UVC_FRAME_UNCOMPRESSED(n)		\
struct UVC_FRAME_UNCOMPRESSED(n) {			\
	uint8_t  bLength;					\
	uint8_t  bDescriptorType;				\
	uint8_t  bDescriptorSubType;			\
	uint8_t  bFrameIndex;				\
	uint8_t  bmCapabilities;				\
	uint16_t wWidth;					\
	uint16_t wHeight;					\
	uint32_t dwMinBitRate;				\
	uint32_t dwMaxBitRate;				\
	uint32_t dwMaxVideoFrameBufferSize;		\
	uint32_t dwDefaultFrameInterval;			\
	uint8_t  bFrameIntervalType;			\
	uint32_t dwFrameInterval[n];			\
} __attribute__ ((packed))

/* MJPEG Payload - 3.1.1. MJPEG Video Format Descriptor */
struct uvc_format_mjpeg
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubType;
    uint8_t  bFormatIndex;
    uint8_t  bNumFrameDescriptors;
    uint8_t  bmFlags;
    uint8_t  bDefaultFrameIndex;
    uint8_t  bAspectRatioX;
    uint8_t  bAspectRatioY;
    uint8_t  bmInterfaceFlags;
    uint8_t  bCopyProtect;
} __attribute__((__packed__));

#define UVC_DT_FORMAT_MJPEG_SIZE			11

/* MJPEG Payload - 3.1.2. MJPEG Video Frame Descriptor */
struct uvc_frame_mjpeg
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubType;
    uint8_t  bFrameIndex;
    uint8_t  bmCapabilities;
    uint16_t wWidth;
    uint16_t wHeight;
    uint32_t dwMinBitRate;
    uint32_t dwMaxBitRate;
    uint32_t dwMaxVideoFrameBufferSize;
    uint32_t dwDefaultFrameInterval;
    uint8_t  bFrameIntervalType;
    uint32_t dwFrameInterval[];
} __attribute__((__packed__));

#define UVC_DT_FRAME_MJPEG_SIZE(n)			(26+4*(n))

#define UVC_FRAME_MJPEG(n) \
	uvc_frame_mjpeg_##n

#define DECLARE_UVC_FRAME_MJPEG(n)			\
struct UVC_FRAME_MJPEG(n) {				\
	uint8_t  bLength;					\
	uint8_t  bDescriptorType;				\
	uint8_t  bDescriptorSubType;			\
	uint8_t  bFrameIndex;				\
	uint8_t  bmCapabilities;				\
	uint16_t wWidth;					\
	uint16_t wHeight;					\
	uint32_t dwMinBitRate;				\
	uint32_t dwMaxBitRate;				\
	uint32_t dwMaxVideoFrameBufferSize;		\
	uint32_t dwDefaultFrameInterval;			\
	uint8_t  bFrameIntervalType;			\
	uint32_t dwFrameInterval[n];			\
} __attribute__ ((packed))

#endif /* _USB_VIDEO_H_ */
//eof

