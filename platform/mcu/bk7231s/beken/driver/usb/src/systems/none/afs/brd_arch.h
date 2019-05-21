/******************************************************************
 *                                                                *
 *        Copyright Mentor Graphics Corporation 2005              *
 *                                                                *
 *                All Rights Reserved.                            *
 *                                                                *
 *    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION *
 *  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS   *
 *  LICENSORS AND IS SUBJECT TO LICENSE TERMS.                    *
 *                                                                *
 ******************************************************************/

#ifndef __MUSB_NONE_BOARD_ARCH_H__
#define __MUSB_NONE_BOARD_ARCH_H__

/*
 * AFS-specific board architecture macros
 * $Revision: 1.5 $
 */

#include "mu_tools.h"

#if MUSB_DIAG >= 3

#undef MGC_Read8
extern uint8_t MGC_NoneRead8(uint8_t *pBase, uint16_t wOffset);
#define MGC_Read8(_pBase, _offset) MGC_NoneRead8(_pBase, _offset)

#undef MGC_Read16
extern uint16_t MGC_NoneRead16(uint8_t *pBase, uint16_t wOffset);
#define MGC_Read16(_pBase, _offset) MGC_NoneRead16(_pBase, _offset)

#undef MGC_Read32
extern uint32_t MGC_NoneRead32(uint8_t *pBase, uint16_t wOffset);
#define MGC_Read32(_pBase, _offset) MGC_NoneRead32(_pBase, _offset)

#undef MGC_Write8
extern void MGC_NoneWrite8(uint8_t *pBase, uint16_t wOffset, uint8_t bDatum);
#define MGC_Write8(_pBase, _offset, _data) MGC_NoneWrite8(_pBase, _offset, _data)

#undef MGC_Write16
extern void MGC_NoneWrite16(uint8_t *pBase, uint16_t wOffset, uint16_t wDatum);
#define MGC_Write16(_pBase, _offset, _data) MGC_NoneWrite16(_pBase, _offset, _data)

#undef MGC_Write32
extern void MGC_NoneWrite32(uint8_t *pBase, uint16_t wOffset, uint32_t dwDatum);
#define MGC_Write32(_pBase, _offset, _data) MGC_NoneWrite32(_pBase, _offset, _data)

#endif

#endif	/* multiple inclusion protection */
