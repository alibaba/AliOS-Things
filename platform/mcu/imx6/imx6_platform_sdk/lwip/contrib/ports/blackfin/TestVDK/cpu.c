/************************************************************************/
/**	\file	cpu.c
 *	\brief	Basic CPU functions for LwIP test
 *
 *	\date	2011/10/06
 *	\author	Stephane Lesage for LwIP
 *
 */
/************************************************************************/

/* Includes */
#include "cpu.h"
#include <blackfin.h>
#include <ccblkfn.h>

/* Externals */

/* Macros */

/* Variables */
unsigned CPU_MCLK = 25000000;
unsigned CPU_CCLK = 250000000;
unsigned CPU_SCLK = 100000000;

/* Functions */

void CPU_Clocks(unsigned mclk, unsigned msel, unsigned ssel, unsigned vlev)
{
	// Compute clock frequencies
	CPU_MCLK = mclk;
	CPU_CCLK = CPU_MCLK*msel;
	CPU_SCLK = CPU_CCLK/ssel;
	
	// DMA priority over Core
	*pEBIU_AMGCTL = CDPRIO;

	// Enable wake-up for PLL interrupt
	*pSIC_IWR |= IRQ_PLL_WAKEUP;

////////////	Boot Default Voltage Regulator Settings		////////////

	// VLEV = 1.2 V
	// GAIN = 20
	// FREQ = 333 kHz

	unsigned vrctl = CLKBUFOE|vlev|GAIN_20|FREQ_333;
	if (*pVR_CTL != vrctl)
	{
		unsigned imask = cli();
		*pVR_CTL = vrctl;
		idle();						// drain pipeline, enter idled state, wait for PLL wakeup
		sti(imask);
	}

////////////	Boot Defaut PLL Timings		////////////

	// CLKIN = 25 MHz (Quartz)
	// VCO = 10 x CLKIN = 250 MHz (PLL)
	// CCLK = VCO / 1 = 250 MHz (Core Clock)
	// SCLK = VCO / 5 = 50 MHz (System Clock)

	unsigned pllctl = SET_MSEL(msel) | SPORT_HYS;
	if (*pPLL_CTL != pllctl)
	{
		unsigned imask = cli();
		*pPLL_CTL = pllctl;
		idle();						// drain pipeline, enter idled state, wait for PLL wakeup
		sti(imask);
	}

	*pPLL_DIV = CSEL_DIV1|SET_SSEL(ssel);
}

void CPU_SDRAM_133(void)
{
	// SCLK = 133 MHz -> cycle = 7.5ns
	
	// MT48LC8M16A2
	// 4096 rows x 512 columns x 16 bits = 16 MegaBytes
	// CAS Latency = 2
	// tRAS = 44ns -> 6 cycles
	// tRP = 20ns -> 3 cycles
	// tRCD = 20ns -> 3 cycles
	// tWR = 15ns -> 2 cycles
	
	// Refresh Rate Control Register
	*pEBIU_SDRRC = (short)(133333333*64e-3/4096)-(6+3);	// FSCLK*tREF/NRA - (tRAS+tRP)
	ssync();
	// Bank Control Register
	*pEBIU_SDBCTL = EBCAW_9|EBSZ_16|EBE;			// 9 bits column, 16 MB (minimum)
	ssync();
	// Global Control Register
	*pEBIU_SDGCTL = PSS|TWR_2|TRCD_3|TRP_3|TRAS_6|PASR_ALL|CL_2|SCTLE;
	ssync();
}

void CPU_SDRAM_100(void)
{
	// SCLK = 100 MHz -> cycle = 10ns
	
	// MT48LC8M16A2
	// 4096 rows x 512 columns x 16 bits = 16 MegaBytes
	// CAS Latency = 2
	// tRAS = 44ns -> 5 cycles
	// tRP = 20ns -> 2 cycles
	// tRCD = 20ns -> 2 cycles
	// tWR = 15ns -> 2 cycles
	
	// Refresh Rate Control Register
	*pEBIU_SDRRC = (short)(100e6*64e-3/4096)-(5+2);	// FSCLK*tREF/NRA - (tRAS+tRP)
	ssync();
	// Bank Control Register
	*pEBIU_SDBCTL = EBCAW_9|EBSZ_16|EBE;			// 9 bits column, 16 MB (minimum)
	ssync();
	// Global Control Register
	*pEBIU_SDGCTL = PSS|TWR_2|TRCD_2|TRP_2|TRAS_5|PASR_ALL|CL_2|SCTLE;
	ssync();
}
