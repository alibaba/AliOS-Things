/******************************************************************
 *                                                                *
 *         Copyright Mentor Graphics Corporation 2006             *
 *                                                                *
 *                All Rights Reserved.                            *
 *                                                                *
 *    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION *
 *  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS   *
 *  LICENSORS AND IS SUBJECT TO LICENSE TERMS.                    *
 *                                                                *
 ******************************************************************/

/*
 * MUSB-MicroSW handling of toolset-specific means to define
 * C'99 types, TRUE, FALSE, NULL, MUSB_MIN and MUSB_MAX.
 * $Revision: 1.5 $
 */

#ifndef __MUSB_TOOLSET_H__
#define __MUSB_TOOLSET_H__

/*
 * Include the C '99 header for definite-size data types.
 * If build env. doesn't provide this, make one with definitions
 * for your platform's types for signed/unsigned 8, 16, 32-bit
 * integers (int8_t, uint8_t, int16_t, uint16_t, etc.)
 * Also, if your compiler doesn't support const,
 * #define const
 * or preprocess with COMPILER_NO_CONST defined
 */
#ifdef GNU
#include <stdint.h>
#else
#include "stdint.h"
#endif

#ifdef COMPILER_NO_CONST
#define const
#endif

#ifndef TRUE
#define TRUE                      1
#endif

#ifndef FALSE
#define FALSE                     0
#endif

#ifndef NULL
#define NULL                     (0L)
#endif

/*
 *  ABS - get Absolute value
 */
#define MUSB_ABS(x)  (((x) >= 0) ? (x) : ((x) * -1))

/*
 *  MIN - get Minimum value
 */
#define MUSB_MIN(x,y)    (((x) < (y)) ? (x) : (y))

/*
 *  MAX - get Maximum value
 */
#define MUSB_MAX(x,y)    (((x) < (y)) ? (y) : (x))

#ifdef __ARMCC_VERSION
/* ADS */

#define MUSB_INT_VECTOR

#endif

#ifdef _MSC_VER

/* Microsoft compiler */
#define MUSB_INT_VECTOR
#define MUSB_STDLIB

#endif	/* Microsoft compiler */

#endif	/* multiple inclusion protection */
// eof

