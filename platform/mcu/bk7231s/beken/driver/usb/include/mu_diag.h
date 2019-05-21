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
 * MUSB-MicroSW diagnostic messaging macros.
 * $Revision: 1.4 $
 */

#ifndef __MUSB_DIAG_H__
#define __MUSB_DIAG_H__

#include "include.h"
#include "uart_pub.h"

#define MUSB_DEBUG

#ifdef MUSB_DEBUG
#define MUSB_PRT                 os_printf
#define MUSB_NPRT                os_null_printf
#define MUSB_WPRT                warning_prf
#else
#define MUSB_PRT                 os_null_printf
#define MUSB_WPRT                os_null_printf
#endif

#define MUSB_DIAG_STRING(_bLevel, _pMsg)
#define MUSB_DIAG1(_bLevel, _pText1, _dwNum1, _bBase, _bJust)
#define MUSB_DIAG2(_bLevel, _pText1, _dwNum1, _pText2, _dwNum2, _bBase, _bJust)

#endif	/* multiple inclusion protection */
