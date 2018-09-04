/************************************************************************/
/**	\file	cpu.h
 *	\brief	Generic CPU functions and defines
 *
 *	\date	2011/10/11
 *	\author	Stephane Lesage - ATEIS
 *
 */
/************************************************************************/
#ifndef _CPU_H_
#define _CPU_H_

/* Includes */
#include "ATypes.h"
#if defined(_TMS320C6X)
#include "cpu_c674.h"
#elif defined(__ADSPBLACKFIN__)
#include "cpu_bfin.h"
#else
#error "Platform not detected"
#endif

/* Macros */
#ifdef ACCESS_LITTLE_ENDIAN
#define CPU_HostToBig(x)	CPU_SwapLong(x)
#define CPU_HostToLittle(x)	(x)
#else
#define CPU_HostToBig(x)	(x)
#define CPU_HostToLittle(x)	CPU_SwapLong(x)
#endif

/* Types */

/* Externals */

/* Generic Functions */

#ifdef __cplusplus
extern "C"
{
#endif

/**	Perform CPU initializations at start-up.

This should be the first function called by main().

- set clocks
- set external memory timings
- configure ports for peripheral or GPIO function
- call SPI_Init() and I2C_Init() if these buses are used

\remark Application-dependent function to be implemented in arch.c

*/
void CPU_Init(void);

/**	Reset the processor

This function of course never returns.

*/
void CPU_Reset(void);

/**	Wait

\param[in]	ticks	number of 0.1ms ticks to wait for

*/
void CPU_Wait(uint32_t ticks);

#ifdef __cplusplus
}
#endif

/* Include Guard */
#endif
