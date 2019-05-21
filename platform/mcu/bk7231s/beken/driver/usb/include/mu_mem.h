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
 * MUSB-MicroSW memory abstraction.
 * $Revision: 1.7 $
 */

#ifndef __MUSB_MEMORY_H__
#define __MUSB_MEMORY_H__

#include "mu_tools.h"

/* Allow platform to define */
#include "plat_mem.h"

/* Be sure we have something for each one */

#ifndef MUSB_MemAlloc
#define MUSB_MemAlloc       os_malloc
#endif

#ifndef MUSB_MemRealloc
#define MUSB_MemRealloc     os_realloc
#endif

#ifndef MUSB_MemFree
#define MUSB_MemFree        os_free
#endif

#ifndef MUSB_MemCopy
#define MUSB_MemCopy        os_memcpy
#endif

#ifndef MUSB_MemSet
#define MUSB_MemSet          os_memset
#endif

#endif	/* multiple inclusion protection */
