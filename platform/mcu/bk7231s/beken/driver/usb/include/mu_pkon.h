/******************************************************************
 *                                                                *
 *        Copyright Mentor Graphics Corporation 2003              *
 *                                                                *
 *                All Rights Reserved.                            *
 *                                                                *
 *    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION *
 *  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS   *
 *  LICENSORS AND IS SUBJECT TO LICENSE TERMS.                    *
 *                                                                *
 ******************************************************************/

/*
 * Turn structure packing on.
 * This must be done in a header because some compilers do it
 * with #pragma's.
 * $Revision: 1.2 $
 */

/* not needed for 8051 */
#ifndef  C8051

#ifdef _MSC_VER

/* Microsoft compiler */
#pragma warning(disable:4103)
#pragma pack(push)
#pragma pack(1)

#endif /* _MSC_VER */

#ifdef __ARMCC_VERSION

/* Assume ARM compiler */
__packed

#endif /* __ARMCC_VERSION */

#endif /* C8051 */
