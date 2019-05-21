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
 * MUSB-MicroSW HID (Human Interface Device) definitions.
 * $Revision: 1.4 $
 */

#ifndef __MUSB_HID_H__
#define __MUSB_HID_H__

#include "mu_dev.h"
#include "mu_tools.h"

/*
 * HID Class Descriptor Types
 */

#define MUSB_HID_DESCRIPTOR			0x21
#define MUSB_REPORT_DESCRIPTOR		0x22
#define MUSB_PHYSICAL_DESCRIPTOR	0x23

/*
 * HID requests
 */

#define MUSB_REQ_GET_REPORT		0x01
#define MUSB_REQ_GET_IDLE		0x02
#define MUSB_REQ_GET_PROTOCOL	0x03
#define MUSB_REQ_SET_REPORT		0x09
#define MUSB_REQ_SET_IDLE		0x0A
#define MUSB_REQ_SET_PROTOCOL	0x0B

/*
 *  HID Report Types
 */

#define MUSB_HID_INPUT_REPORT		0x01
#define MUSB_HID_OUTPUT_REPORT		0x02
#define MUSB_HID_FEATURE_REPORT		0x03

/* HID descriptor */
#include "mu_pkon.h"
typedef	struct
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t bcdHID;
    uint8_t bCountryCode;
    uint8_t bNumDescriptors;
    uint8_t bClassDescriptorType;
    uint16_t wDescriptorLength;
} MUSB_HidDescriptor;
#include "mu_pkoff.h"

/* HID optional descriptor */
#include "mu_pkon.h"
typedef	struct
{
    uint8_t bDescriptorType;
    uint16_t wDescriptorLength;
} MUSB_HidOptionalDescriptor;
#include "mu_pkoff.h"

/*
 * Item tags (first byte of each item in a report)
 */

/* size field: size of associated data which follows */
#define MUSB_S_HID_REPORT_ITEMTAG_SIZE	0
#define MUSB_M_HID_REPORT_ITEMTAG_SIZE	0x03
#define MUSB_HID_REPORT_ITEMTAG_SIZE_0	0x00
#define MUSB_HID_REPORT_ITEMTAG_SIZE_1	0x01
#define MUSB_HID_REPORT_ITEMTAG_SIZE_2	0x02
#define MUSB_HID_REPORT_ITEMTAG_SIZE_4	0x03

/* type field: type of tag */
#define MUSB_S_HID_REPORT_ITEMTAG_TYPE	2
#define MUSB_M_HID_REPORT_ITEMTAG_TYPE	0x03
#define MUSB_HID_REPORT_ITEMTAG_TYPE_MAIN	0x00
#define MUSB_HID_REPORT_ITEMTAG_TYPE_GLOBAL	0x01
#define MUSB_HID_REPORT_ITEMTAG_TYPE_LOCAL	0x02

/* tag field */
#define MUSB_S_HID_REPORT_ITEMTAG_TAG	4
#define MUSB_M_HID_REPORT_ITEMTAG_TAG	0x0f

/* tag values when type=main */
#define MUSB_HID_REPORT_ITEMTAG_MAIN_INPUT			0x08
#define MUSB_HID_REPORT_ITEMTAG_MAIN_OUTPUT			0x09
#define MUSB_HID_REPORT_ITEMTAG_MAIN_COLLECTION		0x0a
#define MUSB_HID_REPORT_ITEMTAG_MAIN_FEATURE		0x0b
#define MUSB_HID_REPORT_ITEMTAG_MAIN_END_COLLECTION	0x0c

/* tag values when type=global */
#define MUSB_HID_REPORT_ITEMTAG_GLOBAL_USAGE_PAGE		0x00
#define MUSB_HID_REPORT_ITEMTAG_GLOBAL_LOGICAL_MIN		0x01
#define MUSB_HID_REPORT_ITEMTAG_GLOBAL_LOGICAL_MAX		0x02
#define MUSB_HID_REPORT_ITEMTAG_GLOBAL_PHYSICAL_MIN		0x03
#define MUSB_HID_REPORT_ITEMTAG_GLOBAL_PHYSICAL_MAX		0x04
#define MUSB_HID_REPORT_ITEMTAG_GLOBAL_UNIT_EXPONENT	0x05
#define MUSB_HID_REPORT_ITEMTAG_GLOBAL_UNIT				0x06
#define MUSB_HID_REPORT_ITEMTAG_GLOBAL_REPORT_SIZE		0x07
#define MUSB_HID_REPORT_ITEMTAG_GLOBAL_REPORT_ID		0x08
#define MUSB_HID_REPORT_ITEMTAG_GLOBAL_REPORT_COUNT		0x09
#define MUSB_HID_REPORT_ITEMTAG_GLOBAL_PUSH				0x0a
#define MUSB_HID_REPORT_ITEMTAG_GLOBAL_POP				0x0b

/* tag values when type=local */
#define MUSB_HID_REPORT_ITEMTAG_LOCAL_USAGE				0x00
#define MUSB_HID_REPORT_ITEMTAG_LOCAL_USAGE_MIN			0x01
#define MUSB_HID_REPORT_ITEMTAG_LOCAL_USAGE_MAX			0x02
#define MUSB_HID_REPORT_ITEMTAG_LOCAL_DESIGNATOR_INDEX	0x03
#define MUSB_HID_REPORT_ITEMTAG_LOCAL_DESIGNATOR_MIN	0x04
#define MUSB_HID_REPORT_ITEMTAG_LOCAL_DESIGNATOR_MAX	0x05
#define MUSB_HID_REPORT_ITEMTAG_LOCAL_STRING_INDEX		0x07
#define MUSB_HID_REPORT_ITEMTAG_LOCAL_STRING_MIN		0x08
#define MUSB_HID_REPORT_ITEMTAG_LOCAL_STRING_MAX		0x09
#define MUSB_HID_REPORT_ITEMTAG_LOCAL_DELIMITER			0x0a

/* data for main input, output & feature items */
#define MUSB_S_HID_MAIN_IOF_DATA_CONSTANT			0	/* otherwise data */
#define MUSB_S_HID_MAIN_IOF_DATA_VARIABLE			1	/* otherwise array */
#define MUSB_S_HID_MAIN_IOF_DATA_RELATIVE			2	/* otherwise absolute */
#define MUSB_S_HID_MAIN_IOF_DATA_WRAP				3	/* otherwise no wrap */
#define MUSB_S_HID_MAIN_IOF_DATA_NONLINEAR			4	/* otherwise linear */
#define MUSB_S_HID_MAIN_IOF_DATA_NO_PREFERRED		5	/* otherwise preferred state */
#define MUSB_S_HID_MAIN_IOF_DATA_NULL_STATE			6	/* otherwise no null position */
#define MUSB_S_HID_MAIN_IOF_DATA_VOLATILE			7	/* otherwise non-volatile */
#define MUSB_S_HID_MAIN_IOF_DATA_BUFFERED_BYTES		8	/* otherwise bitfield */

/* data for main collection items */
#define MUSB_HID_MAIN_COLL_DATA_PHYSICAL		0x00	/* group of axes */
#define MUSB_HID_MAIN_COLL_DATA_APPLICATION		0x01	/* mouse, keyboard */
#define MUSB_HID_MAIN_COLL_DATA_LOGICAL			0x02	/* interrelated data */
#define MUSB_HID_MAIN_COLL_DATA_REPORT			0x03	/* report */
#define MUSB_HID_MAIN_COLL_DATA_NAMED_ARRAY		0x04	/* named array */
#define MUSB_HID_MAIN_COLL_DATA_USAGE_SWITCH	0x05	/* usage switch */
#define MUSB_HID_MAIN_COLL_DATA_USAGE_MODIFIER	0x06	/* usage modifier */

/* usage pages */
#define MUSB_HID_USAGE_PAGE_GENERIC_DESKTOP_CONTROLS	0x01
#define MUSB_HID_USAGE_PAGE_SIMULATION_CONTROLS			0x02
#define MUSB_HID_USAGE_PAGE_VR_CONTROLS					0x03
#define MUSB_HID_USAGE_PAGE_SPORTS_CONTROLS				0x04
#define MUSB_HID_USAGE_PAGE_GAME_CONTROLS				0x05
#define MUSB_HID_USAGE_PAGE_GENERIC_DEVICE_CONTROLS		0x06
#define MUSB_HID_USAGE_PAGE_KEYBOARD_KEYPAD				0x07
#define MUSB_HID_USAGE_PAGE_LEDS						0x08
#define MUSB_HID_USAGE_PAGE_BUTTON						0x09
#define MUSB_HID_USAGE_PAGE_ORDINAL						0x0a
#define MUSB_HID_USAGE_PAGE_TELEPHONY					0x0b
#define MUSB_HID_USAGE_PAGE_CONSUMER					0x0c
#define MUSB_HID_USAGE_PAGE_DIGITIZER					0x0d
#define MUSB_HID_USAGE_PAGE_PID							0x0f	/* Physical Interface Device (force feedback) */
#define MUSB_HID_USAGE_PAGE_UNICODE						0x10
#define MUSB_HID_USAGE_PAGE_ALPHANUMERIC_DISPLAY		0x14
#define MUSB_HID_USAGE_PAGE_MEDICAL_INSTRUMENTS			0x40
#define MUSB_HID_USAGE_PAGE_MONITOR_LOW					0x80
#define MUSB_HID_USAGE_PAGE_MONITOR_HIGH				0x83
#define MUSB_HID_USAGE_PAGE_POWER_LOW					0x84
#define MUSB_HID_USAGE_PAGE_POWER_HIGH					0x87
#define MUSB_HID_USAGE_PAGE_BAR_CODE_SCANNER			0x8c
#define MUSB_HID_USAGE_PAGE_SCALE						0x8d
#define MUSB_HID_USAGE_PAGE_MAGNETIC_STRIPE_READERS		0x8e
#define MUSB_HID_USAGE_PAGE_RESERVED_POINT_OF_SALE		0x8f
#define MUSB_HID_USAGE_PAGE_CAMERA_CONTROL				0x90
#define MUSB_HID_USAGE_PAGE_ARCADE						0x91

#endif	/* multiple inclusion protection */
