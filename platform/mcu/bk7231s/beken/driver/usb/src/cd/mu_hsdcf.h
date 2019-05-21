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

/*
 * High-speed DMA controller configuration definitions
 * $Revision: 1.2 $
 */

#ifndef __MUSB_HSDMA_CONFIG_H__
#define __MUSB_HSDMA_CONFIG_H__

#if defined(MUSB_HDRC) || defined(MUSB_MHDRC)
#include "hdrc_cnf.h"
#endif

#ifdef MUSB_C_DMA8
#define MUSB_HSDMA
#define MUSB_HSDMA_CHANNELS 8
#elif defined(MUSB_C_DMA7)
#define MUSB_HSDMA
#define MUSB_HSDMA_CHANNELS 7
#elif defined(MUSB_C_DMA6)
#define MUSB_HSDMA
#define MUSB_HSDMA_CHANNELS 6
#elif defined(MUSB_C_DMA5)
#define MUSB_HSDMA
#define MUSB_HSDMA_CHANNELS 5
#elif defined(MUSB_C_DMA4)
#define MUSB_HSDMA
#define MUSB_HSDMA_CHANNELS 4
#elif defined(MUSB_C_DMA3)
#define MUSB_HSDMA
#define MUSB_HSDMA_CHANNELS 3
#elif defined(MUSB_C_DMA2)
#define MUSB_HSDMA
#define MUSB_HSDMA_CHANNELS 2
#elif defined(MUSB_C_DMA)
#define MUSB_HSDMA
#define MUSB_HSDMA_CHANNELS 1
#endif

#endif	/* multiple inclusion protection */
