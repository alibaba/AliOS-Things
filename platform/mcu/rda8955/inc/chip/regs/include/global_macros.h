/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
* All rights reserved.
*
* This software is supplied "AS IS" without any warranties.
* RDA assumes no responsibility or liability for the use of the software,
* conveys no license or title under any patent, copyright, or mask work
* right to the product. RDA reserves the right to make changes in the
* software without notification.  RDA also make no representation or
* warranty that such application will be suitable for the specified use
* without further testing or modification.
*/


#ifndef _GLOBAL_MACROS_H_
#define _GLOBAL_MACROS_H_



// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================




#define KSEG0(addr)     ( (addr) | 0x80000000 )
#define KSEG1(addr)     ( (addr) | 0xa0000000 )
#define KSEG01_PHY_ADDR(addr)       ((UINT32)(addr) & 0x0FFFFFFF)


/* Define access cached or uncached */
#define MEM_ACCESS_CACHED(addr)     ((UINT32*)((UINT32)(addr)&0xdfffffff))
#define MEM_ACCESS_UNCACHED(addr)   ((UINT32*)((UINT32)(addr)|0x20000000))

/* Register access for assembly */
#define BASE_HI(val) (((0xa0000000 | val) & 0xffff8000) + (val & 0x8000))
#define BASE_LO(val) (((val) & 0x7fff) - (val & 0x8000))


/* to extract bitfield from register value */
#define GET_BITFIELD(dword, bitfield) (((dword) & (bitfield ## _MASK)) >> (bitfield ## _SHIFT))

#define EXP2(n) (1<<(n))

#define IS_IN_RANGE(value, base, size)	(((value) >= (base)) && ((value) < (base) + (size)))
#define ADDRESS_IN_HWP(address, hwp)	IS_IN_RANGE((UINT32)(address) & 0x0FFFFFFF, (UINT32)(hwp) & 0x0FFFFFFF, sizeof(*(hwp)))


#endif

