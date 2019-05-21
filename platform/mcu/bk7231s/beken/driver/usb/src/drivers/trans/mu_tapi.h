/******************************************************************
*                                                                *
*        Copyright Mentor Graphics Corporation 2004              *
*                                                                *
*                All Rights Reserved.                            *
*                                                                *
*    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION *
*  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS   *
*  LICENSORS AND IS SUBJECT TO LICENSE TERMS.                    *
*                                                                *
******************************************************************/

#ifndef	__MUSB_TRANSPORT_API_H__
#define	__MUSB_TRANSPORT_API_H__

/*****************************************************************************
 * FILE NAME    : mu_tapi.h
 * DATE         :
 * DESCRIPTION  : It Contains Function prototypes of all functions exported
 *                by Transport device driver to other layers.
 *
 * $Revision: 1.15 $
 ****************************************************************************/

/**
 * Fill an array with the targetted peripheral list entry appropriate
 * for the transport driver, ending with the MUSB_TARGET_ACCEPT.
 * @param pList array
 * @param wListLength how many bytes are available in the array
 * @return how many bytes were filled in the array.
 * If this equals bListLength, the caller should assume there is insufficient space
 * in the array and the list entry is incomplete.
 */
extern uint16_t MGC_FillTransportPeripheralList(uint8_t *pList, uint16_t wListLength);

/**
 * Get a pointer to the transport device driver
 */
extern MUSB_DeviceDriver *MGC_GetTransportDeviceDriver(void);

#endif
