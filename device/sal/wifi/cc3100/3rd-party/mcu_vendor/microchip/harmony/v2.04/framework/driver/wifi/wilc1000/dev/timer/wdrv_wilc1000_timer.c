/*******************************************************************************
  WILC1000 Wireless Driver Timer Functions

  File Name:
    wdrv_wilc1000_timer.c

  Summary:
    Timer functions for WILC1000 wireless driver.

  Description:
    Timer functions for WILC1000 wireless driver.
 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/

#include "wdrv_wilc1000_api.h"

void WDRV_STUB_HardDelay(uint16_t delay)
{
#define TICKS_PER_SECOND 1000

	uint32_t now = SYS_TMR_TickCountGet();
	uint32_t tickToElapse;

	if (delay == 0 || delay > TICKS_PER_SECOND)
		WDRV_ASSERT(false, "Invalid delay range.");

	tickToElapse = SYS_TMR_TickCounterFrequencyGet() / TICKS_PER_SECOND;
	tickToElapse *= delay;

	while (!(SYS_TMR_TickCountGet() - now >= tickToElapse))
		;
}

//DOM-IGNORE-END
