/************************************************************************/
/**	\file	ATypes.h
 *	\brief	ACCESS types
 *
 *	\date	2011/04/14
 *	\author	Stephane Lesage - ATEIS
 *
 */
/************************************************************************/
#ifndef _ATYPES_H_
#define _ATYPES_H_

/* Includes */
#include <stddef.h>
#include <stdint.h>
#if !defined(__cplusplus) && defined(_TMS320C6X)
#include <stdbool.h>
#endif

/* Macros */

/* Types */
typedef char* txt;							//!< Short for C string
typedef const char* ctxt;					//!< Short for const C string
typedef unsigned char uchar;				//!< Short for unsigned char
typedef unsigned short ushort;				//!< Short for unsigned short
typedef unsigned int uint;					//!< Short for unsigned int

/* Externals */

/* Functions */

/* Class */

/* Include Guard */
#endif
