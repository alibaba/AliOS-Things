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
 * MUSB-MicroSW buffer-overrun-safe micro-string library API.
 * $Revision: 1.3 $
 */

#ifndef __MUSB_STRING_H__
#define __MUSB_STRING_H__

#include "mu_tools.h"

/**
 * Compare strings.
 * Lexically compare two strings.
 * @param pString1 first NUL-terminated ASCII string
 * @param pString2 second NUL-terminated ASCII string
 * @param bCaseSensitive TRUE for case-sensitive letter comparison;
 * FALSE to ignore case
 * @return 0 if strings are identical
 */
extern int8_t MUSB_StringCompare(const char *pString1,
                                 const char *pString2, uint8_t bCaseSensitive);

/**
 * strncat-analogue.
 * Concatenate a string to the end of another.
 * @param pResult the NUL-terminated ASCII string serving as the
 * first string and result buffer
 * @param wResultLength length of result buffer
 * @param pConcat NUL-terminated ASCII string to append
 * @return TRUE on success
 * @return FALSE on failure (result would overflow buffer)
 */
extern uint8_t MUSB_StringConcat(char *pResult,
                                 uint16_t wResultLength, const char *pConcat);

/**
 * strstr-analogue.
 * Find the first occurence of a pattern in a string.
 * @param pString the NUL-terminated ASCII string in which to search
 * @param pPattern the NUL-terminated ASCII pattern for which to search
 * @param bCaseSensitive TRUE for case sensite letter comparison;
 * FALSE to ignore case
 * @return pointer to first occurence if found; NULL if not found
 */
extern const char *MUSB_StringFind(const char *pString,
                                   const char *pPattern, uint8_t bCaseSensitive);

/**
 * Integer-to-string.
 * Convert an unsigned integer to a string.
 * @param pResult the result buffer
 * @param wResultLength length of result buffer
 * @param dwNumber the unsigned integer
 * @param bBase 2, 8, 10, or 16 for binary, octal, decimal, or hex
 * @param bJustification if non-zero, pad with leading 0s as necessary
 * to output this length
 * @return TRUE on success
 * @return FALSE on failure (result would overflow buffer)
 */
extern uint8_t MUSB_Stringize(char *pResult, uint16_t wResultLength,
                              uint32_t dwNumber, uint8_t bBase,
                              uint8_t bJustification);

/**
 * strlen-analogue.
 * Return the length of a NUL-terminated ASCII string.
 * @param pString the NUL-terminated ASCII string
 * @return Length, in characters, of the string
 */
extern uint16_t MUSB_StringLength(const char *pString);

/**
 * strtol-analogue.
 * Parse a string in a given base (or any base using a limited set of specifiers).
 * @param pString the NUL-terminated ASCII string
 * @param pEnd filled with a pointer to the character immediately following
 * the last parsed character
 * @param bBase 2, 8, 10, or 16 for a specific base, or 0 to determine the base
 * from the string (i.e. leading 0x or 0X denotes base 16; 0 for octal, % for binary)
 * @return the number, or 0 on failure (on failure, pEnd==pString)
 */
extern int32_t MUSB_StringParse(const char *pString, const char **pEnd,
                                uint8_t bBase);

#endif	/* multiple inclusion protection */
