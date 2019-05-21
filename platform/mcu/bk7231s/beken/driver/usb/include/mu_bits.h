/*****************************************************************************
 *                                                                           *
 *      Copyright Mentor Graphics Corporation 2006                      *
 *                                                                           *
 *                All Rights Reserved.                                       *
 *                                                                           *
 *    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION            *
 *  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS              *
 *  LICENSORS AND IS SUBJECT TO LICENSE TERMS.                               *
 *                                                                           *
 ****************************************************************************/

/*
 * MUSB-MicroSW USB Software Platform bit-manipulation library API.
 * $Revision: 1.3 $
 */

#ifndef __MUSB_BITS_H__
#define __MUSB_BITS_H__

#include "mu_tools.h"

/**
 * Extract bits.
 * Extract a range of up to 32 bits from an arbitrary bit position in a buffer,
 * assumed to be in USB byte order.
 * @param pBuffer buffer pointer
 * @param bOffset the bit offset into the buffer's first byte
 * @param bCount how many bits to extract
 * @param pdwResult where to store the 32-bit extracted result
 * @return 0 on success
 * @return -1 if pBuffer==NULL or pdwResult==NULL
 * @return -2 if illegal offset/count [((bOffset+bCount) > 32) or (bOffset > 7)]
 */
/*
extern int MUSB_BitsGet(const uint8_t* pBuffer, uint8_t bOffset, uint8_t bCount,
uint32_t* pdwResult);*/

/**
 * Set bits.
 * Set a range of up to 32 bits in an arbitrary bit position in a buffer,
 * assumed to be in USB byte order.
 * @param pBuffer buffer pointer
 * @param bOffset the bit offset into the buffer's first byte
 * @param bCount how many bits to set
 * @param dwValue the bits to set
 * @return 0 on success
 * @return -1 if pBuffer==NULL
 * @return -2 if illegal offset/count [(bCount==0) or ((bOffset+bCount) > 32) or (bOffset > 7)]
 */
/*
extern int MUSB_BitsSet(uint8_t* pBuffer, uint8_t bOffset, uint8_t bCount,
uint32_t dwValue);
*/
#endif	/* multiple inclusion protection */
