/************************************************************************/
/**	\file	cpu_c674.h
 *	\brief	CPU functions for C674x architecture
 *
 *	\date	2011/04/19
 *	\author	Pierre Audenard - ATEIS
 *
 */
/************************************************************************/
#ifndef _CPU_C674_H_
#define _CPU_C674_H_

/* Includes */
#include <c6x.h>

/* Macros */
#define	ACCESS_LITTLE_ENDIAN
#undef	ACCESS_BIG_ENDIAN
#define CPU_SYSCLK2 (CPU_SYSCLK/2)
#define CPU_SYSCLK4 (CPU_SYSCLK/4)
#define CPU_MAINCLK	CPU_AUXCLK

#define CPU_SwapShort(x)	((uint16_t)_swap4((uint32_t)x))
#define CPU_SwapLong(x)		(_rotl(_swap4((uint32_t)x), 16))

/* Types */

#ifdef __cplusplus
extern "C" {
#endif

/* Externals */
extern unsigned CPU_AUXCLK;
extern unsigned CPU_SYSCLK;

/* Functions */

/**	Configure Core/System Clocks

\param[in]	cclk	Crystal Clock Frequency in Hz
\param[in]	pllmul	PLL multiplier
\param[in]	plldiv	PLL post divider
*/
void CPU_Clocks(unsigned cclk, unsigned pllmul, unsigned plldiv);

#ifdef __cplusplus
}
#endif

/* Include Guard */
#endif
