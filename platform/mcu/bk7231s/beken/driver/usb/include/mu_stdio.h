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
 * MUSB-MicroSW buffer-overrun-safe micro-stdio library API.
 * $Revision: 1.3 $
 */

#ifndef __MUSB_STDIO_H__
#define __MUSB_STDIO_H__

#include "mu_tools.h"

/**
 * gets-analogue.
 * Get a line from whatever is the default input source,
 * blocking if necessary until one is available.
 * @param pBuffer buffer to store a NUL-terminated string (without newline)
 * @param wBufferSize size of line buffer
 * @return the number of characters stored in pBuffer, not including the NUL
 */
extern uint16_t MUSB_GetLine(char *pBuffer, uint16_t wBufferSize);

/**
 * puts-analogue.
 * Print a string to whatever is the default output source.
 * @param pBuffer NUL-terminated ASCII string
 * @return TRUE on success
 * @return FALSE on failure
 */
extern uint8_t MUSB_PrintString(const char *pBuffer);

/**
 * Convenience.
 * Print a string, followed by a newline,
 * to whatever is the default output source.
 * @param pBuffer NUL-terminated ASCII string
 * @return TRUE on success
 * @return FALSE on failure
 */
extern uint8_t MUSB_PrintLine(const char *pBuffer);

#endif	/* multiple inclusion protection */
