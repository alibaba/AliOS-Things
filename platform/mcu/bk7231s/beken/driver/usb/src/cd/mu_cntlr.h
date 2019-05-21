/******************************************************************
 *                                                                *
 *      Copyright (c)  Mentor Graphics Corporation 2004           *
 *                                                                *
 *                All Rights Reserved.                            *
 *                                                                *
 *    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION *
 *  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS   *
 *  LICENSORS AND IS SUBJECT TO LICENSE TERMS.                    *
 *                                                                *
 ******************************************************************/

/*
 * Controller definitions.
 * $Revision: 1.3 $
 */

#ifndef __MUSB_CONTROLLER_H__
#define __MUSB_CONTROLLER_H__

#include "plat_cnf.h"

/* Set feature types per controller type */
#ifdef MUSB_FDRC
#define MGC_OTG
#endif

#ifdef MUSB_HDRC
#define MGC_OTG
#endif

#ifdef MUSB_MHDRC
#define MGC_OTG
#endif

#ifdef MUSB_HSHC
#define MGC_HOST
#endif

#ifdef MUSB_HSFC
#define MGC_FUNCTION
#endif

#if defined(MGC_OTG) && !defined(MUSB_OTG)
#define MUSB_OTG
#endif

#if defined(MGC_HOST) && !defined(MUSB_HOST)
#define MUSB_HOST
#endif

#if defined(MGC_FUNCTION) && !defined(MUSB_FUNCTION)
#define MUSB_FUNCTION
#endif

#if defined(MUSB_FDRC_DMA) && !defined(MUSB_DMA)
#define MUSB_DMA
#endif

#if defined(MUSB_HDRC_DMA) && !defined(MUSB_DMA)
#define MUSB_DMA
#endif

#if defined(MUSB_MHDRC_DMA) && !defined(MUSB_DMA)
#define MUSB_DMA
#endif

#endif	/* multiple inclusion protection */
