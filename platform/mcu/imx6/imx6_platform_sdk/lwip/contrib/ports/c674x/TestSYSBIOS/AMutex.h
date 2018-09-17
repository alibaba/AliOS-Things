/************************************************************************/
/**	\file	AMutex.h
 *	\brief	ACCESS Mutex
 *
 *	\date	2011/11/17
 *	\author	Stephane Lesage - ATEIS
 *
 */
/************************************************************************/
#ifndef _AMUTEX_H_
#define _AMUTEX_H_

/* Includes */
#if defined(_TMS320C6X)
#include "AMutex_SYSBIOS.h"
#elif defined(__ADSPBLACKFIN__)
#include "AMutex_VDK.h"
#else
#error "Platform not detected"
#endif

/* Types */

/* Macros */

/* Externals */

/* Functions */

/* Include Guard */
#endif
