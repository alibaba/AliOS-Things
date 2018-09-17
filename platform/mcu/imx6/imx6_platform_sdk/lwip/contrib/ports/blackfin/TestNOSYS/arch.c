/************************************************************************/
/**	\file	arch.c
 *	\brief	Architecture init functions for LwIP test
 *
 *	\date	2011/10/06
 *	\author	Stephane Lesage for LwIP
 *
 */
/************************************************************************/

/* Includes */
#include "cpu.h"
#include <blackfin.h>

/* Macros */

/* Types */

/* Externals */

/* Variables */

/* Functions */

void CPU_Ports(void)
{
	// Port H pins
	// 0-15: Peripheral MAC
	*pPORTH_FER = 0xFFFF;
}

void CPU_Init()
{
	CPU_Clocks(25000000, 12, 3, VLEV_100);		// 300 / 100 MHz @ 1.0 V
	CPU_SDRAM_100();
	CPU_Ports();
}
