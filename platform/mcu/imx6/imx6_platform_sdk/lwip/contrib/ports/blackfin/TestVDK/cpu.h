/************************************************************************/
/**	\file	cpu.h
 *	\brief	Basic CPU functions for LwIP test
 *
 *	\date	2011/10/06
 *	\author	Stephane Lesage for LwIP
 *
 */
/************************************************************************/
#ifndef _CPU_H_
#define _CPU_H_

/* Includes */

/* Macros */

/* Types */

/* Externals */
extern unsigned CPU_MCLK;
extern unsigned CPU_CCLK;
extern unsigned CPU_SCLK;

/* Functions */
extern "C"
{

/**	Perform CPU initializations at start-up.

This should be the first function called by main().

- set Core and System clocks
- set external memory timings
- configure ports for peripheral or GPIO function
- call SPI_Init() and I2C_Init() if these buses are used

\remark Application-dependent function to be implemented in arch.c 

*/
void CPU_Init(void);

/**	Wait function

\param[in]	ticks	number of 100us ticks to wait for

*/
void CPU_Wait(unsigned ticks);

/**	Reset the processor

This function of course never returns.

*/
#pragma noreturn
void CPU_Reset(void);

/**	Configure Core/System Clocks and Internal Voltage Regulator

\param[in]	mclk	Master Clock Frequency in Hz
\param[in]	msel	Multiplier for Core Clock
\param[in]	ssel	Divider for System Clock
\param[in]	vlev	Voltage Level, use VLEV_xxx macros
*/
void CPU_Clocks(unsigned mclk, unsigned msel, unsigned ssel, unsigned vlev);

/**	Configure the SDRAM for 133 MHz system clock
*/
void CPU_SDRAM_133(void);

/**	Configure the SDRAM for 100 MHz system clock
*/
void CPU_SDRAM_100(void);

}

/* Include Guard */
#endif
