/*******************************************************************************
  WILC1000 Wireless Driver External Interrupt Handler

  File Name:
    wilc1000_eint.c

  Summary:
    External interrupt handler for WILC1000 wireless driver.

  Description:
    External interrupt handler for WILC1000 wireless driver.
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

static void (*s_isr)(void) = NULL;

void WDRV_STUB_INTR_SourceEnable(void)
{
    bool pinStatus = SYS_PORTS_PinRead(PORTS_ID_0, WDRV_IRQN_PORT_CHANNEL, WDRV_IRQN_BIT_POS);

    // If INT line is low, we might have missed an interrupt, hence force an interrupt to
    // clear the status
    if (pinStatus == 0) {
#if defined(PLIB_INT_ExistsSourceFlag)
        if (PLIB_INT_ExistsSourceFlag(INT_ID_0)) {
            PLIB_INT_SourceFlagSet(INT_ID_0, WILC1000_INT_SOURCE);
        }
#endif
    }

    /* enable the external interrupt */
    SYS_INT_SourceEnable(WILC1000_INT_SOURCE);
}

void WDRV_STUB_INTR_SourceDisable(void)
{
    SYS_INT_SourceDisable(WILC1000_INT_SOURCE);
}

void WDRV_STUB_INTR_Init(void (*isr)(void))
{
    /* disable the external interrupt */
    SYS_INT_SourceDisable(WILC1000_INT_SOURCE);

    /* clear and enable the interrupt */
    SYS_INT_SourceStatusClear(WILC1000_INT_SOURCE); // clear status
	s_isr = isr;
}

void WDRV_STUB_INTR_Deinit(void)
{
    SYS_INT_SourceDisable(WILC1000_INT_SOURCE);
	s_isr = NULL;
}

void WDRV_WILC1000_ISR()
{
    SYS_INT_SourceDisable(WILC1000_INT_SOURCE); // disable further interrupts

	if (s_isr)
		s_isr();
}

//DOM-IGNORE-END
