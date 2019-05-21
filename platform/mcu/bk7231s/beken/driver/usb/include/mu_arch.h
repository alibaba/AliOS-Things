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
 * MUSB-MicroSW handling of architecture-specific means to
 * define MUSB_SWAP16, MUSB_SWAP32, MGC_Read8, MGC_Read16,
 * MGC_Read32, MGC_Write8, MGC_Write16, and MGC_Write32.
 * $Revision: 1.9 $
 */

#ifndef __MUSB_ARCH_H__
#define __MUSB_ARCH_H__

#include "mu_tools.h"

/**
 * Architecture Macros Introduction.
 * Architecture-dependent definitions of low-level primitives
 * to perform actual controller register/FIFO accesses.
 *
 * These are only "first approximations" and may be overridden
 * in a platform-specific header (plat_arc.h) via #undef/#define,
 * since this really depends on how the controller is tied to
 * the processor.
 * <p>
 * For example, the core access macros assume a "straight" connection
 * to the processor's address lines.
 * If this is not true (e.g. a 32-bit RISC design omits A[1:0] so
 * 8-bit controller registers show up 4 bytes apart), override these
 * in your plat_arc.h to left-shift the offsets.
 */

/**
 * Read an 8-bit register from the core
 * @param _pBase core base address in memory
 * @param _offset offset into the core's register space
 * @return 8-bit datum
 */
#define MGC_Read8(_pBase, _offset) *((volatile uint8_t *)(_pBase + _offset))

/**
 * Read a 16-bit register from the core
 * @param _pBase core base address in memory
 * @param _offset offset into the core's register space
 * @return 16-bit datum
 */
#define MGC_Read16(_pBase, _offset) *((volatile uint16_t *)(_pBase + _offset))

/**
 * Read a 32-bit register from the core
 * @param _pBase core base address in memory
 * @param _offset offset into the core's register space
 * @return 32-bit datum
 */
#define MGC_Read32(_pBase, _offset) *((volatile uint32_t *)(_pBase + _offset))

/**
 * Write an 8-bit core register
 * @param _pBase core base address in memory
 * @param _offset offset into the core's register space
 * @param _data 8-bit datum
 */
#define MGC_Write8(_pBase, _offset, _data) \
	(*((volatile uint8_t *)(_pBase + _offset)) = _data)

/**
 * Write a 16-bit core register
 * @param _pBase core base address in memory
 * @param _offset offset into the core's register space
 * @param _data 16-bit datum
 */
#define MGC_Write16(_pBase, _offset, _data) \
	(*((volatile uint16_t *)(_pBase + _offset)) = _data)

/**
 * Write a 32-bit core register
 * @param _pBase core base address in memory
 * @param _offset offset into the core's register space
 * @param _data 32-bit datum
 */
#define MGC_Write32(_pBase, _offset, _data) \
	(*((volatile uint32_t *)(_pBase + _offset)) = _data)

/* allow overrides of the above */
#include "plat_arc.h"

/**
 * Swap the given 16-bit quantity from USB byte order (little-endian)
 * to host byte order.
 * @param _data the 16-bit quantity
 * @return quantity in host byte order
 */
#define MUSB_SWAP16(_data) _data

/**
 * Swap the given 32-bit quantity from USB byte order (little-endian)
 * to host byte order.
 * @param _data the 32-bit quantity
 * @return quantity in host byte order
 */
#define MUSB_SWAP32(_data) _data

/**
 * Swap the pointed-to 16-bit quantity from USB byte order (little-endian)
 * to host byte order, in an alignment-safe manner.
 * @param _pData pointer (as uint8_t*)
 * @return quantity in host byte order
 */
#define MUSB_SWAP16P(_pData) ((*(_pData+1) << 8) | *(_pData))

/**
 * Swap the pointed-to 24-bit quantity from USB byte order (little-endian)
 * to host byte order, in an alignment-safe manner.
 * @param _pData pointer (as uint8_t*)
 * @return quantity in host byte order
 */
#define MUSB_SWAP24P(_pData) ((*(_pData+2) << 16) | (*(_pData+1) << 8) | *(_pData))

/**
 * Swap the pointed-to 32-bit quantity from USB byte order (little-endian)
 * to host byte order, in an alignment-safe manner.
 * @param _pData pointer (as uint8_t*)
 * @return quantity in host byte order
 */
#define MUSB_SWAP32P(_pData) ((*(_pData+3) << 24) | (*(_pData+2) << 16) | (*(_pData+1) << 8) | *(_pData))

#endif	/* multiple inclusion protection */
