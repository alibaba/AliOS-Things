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
 * API for communications class driver
 * $Revision: 1.1 $
 */

#ifndef _MUSB_CCD_API_H_
#define _MUSB_CCD_API_H_

#include "mu_cdi.h"

/**
 * Fill an array with the targetted peripheral list entry appropriate
 * for the CCD driver.
 * @param bDriverIndex the driver's index for use with MUSB_TARGET_ACCEPT
 * @param pList array
 * @param wListLength how many bytes are available in the array
 * @return how many bytes were filled in the array.
 * If this equals wListLength, the caller should assume there is insufficient space
 * in the array and the list entry is incomplete.
 */
extern uint16_t MUSB_FillCommPeripheralList(uint8_t bDriverIndex, uint8_t *pList,
        uint16_t wListRemain);

/**
 * Get a pointer to the communications class driver
 */
extern MUSB_DeviceDriver *MUSB_GetCommClassDriver(void);

#endif /* multiple inclusion protection */
