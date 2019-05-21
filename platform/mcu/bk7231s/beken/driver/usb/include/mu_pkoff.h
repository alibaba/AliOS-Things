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
 * Turn structure packing off.
 * This must be done in a header because some compilers do it
 * with #pragma's.
 * $Revision: 1.2 $
 */

/* not needed for 8051 */
#ifndef  C8051

#ifdef _MSC_VER

/* Microsoft compiler */
#pragma warning(disable:4103)
#pragma pack(pop)

#endif /* MSC_VER */

#endif /* C8051 */
