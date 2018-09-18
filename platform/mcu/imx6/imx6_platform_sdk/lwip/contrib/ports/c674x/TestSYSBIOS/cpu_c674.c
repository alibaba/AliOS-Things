/************************************************************************/
/**	\file	cpu_c674.c
 *	\brief	CPU functions for C674x architecture
 *
 *	\date	2011/04/19
 *	\author	Pierre Audenard - ATEIS
 *
 */
/************************************************************************/

/* Includes */
#include "cpu_c674.h"
#include <ti/pspiom/cslr/cslr_tmr.h>
#include <ti/pspiom/cslr/soc_OMAPL137.h>

/* Externals */

/* Macros */

/* Variables */
unsigned CPU_AUXCLK = 25000000;
unsigned CPU_SYSCLK = 300000000;

/* Functions */

#pragma FUNC_NEVER_RETURNS(CPU_Reset);
void CPU_Reset(void)
{
	CSL_TmrRegsOvly tmr1Regs = (CSL_TmrRegsOvly) CSL_TMR_1_REGS;

	/* Bring timers out of reset in watchdog mode */
	tmr1Regs->TGCR = CSL_FMKT(TMR_TGCR_TIM12RS, NO_RESET) |
					 CSL_FMKT(TMR_TGCR_TIM34RS, NO_RESET) |
					 CSL_FMKT(TMR_TGCR_TIMMODE, 64BIT_WDT);

	tmr1Regs->TIM34 = 0x00000000;
	tmr1Regs->TIM12 = 0x00000000;

	/* Max out PRD so that the only possible reset is from bad WD sequence */
	tmr1Regs->PRD34 = 0xFFFFFFFF;
	tmr1Regs->PRD12 = 0xFFFFFFFF;

	tmr1Regs->WDTCR = 0xA5C64000;
	tmr1Regs->WDTCR = 0xDA7E4000;
	tmr1Regs->WDTCR = 0x12344000; // Bad WD service sequence should reset device

	/* Should reset before getting here */
	while (1);
}

void CPU_Wait(unsigned int ticks)
{
	uint32_t start = TSCL;
	uint32_t ncycles = (CPU_SYSCLK/10000)*ticks;
	while (TSCL-start < ncycles);
}

void CPU_Clocks(unsigned cclk, unsigned pllmul, unsigned plldiv)
{
	/* Compute clock frequencies */
	CPU_AUXCLK = cclk;
	CPU_SYSCLK = cclk*pllmul/plldiv;  
}
