/*******************************************************************************
  Timer Device Driver Definition

  Company:
    Microchip Technology Inc.

  File Name:
    timer_pic32cz.h

  Summary:
    Timer Driver Dynamic Implementation

  Description:
    The timer driver provides a simple interface to manage the
    timer modules on Microchip microcontrollers.  This file Implements
    the prototypes and defines necessary for the core interface of timer driver.

*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2015 released Microchip Technology Inc.  All rights reserved.

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
//DOM-IGNORE-END

#ifndef _TIMER_PIC32CZ_H
#define _TIMER_PIC32CZ_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
/*  This section Includes other configuration headers necessary to completely
    define this configuration.
*/


// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
#include "arch/arm/devices_pic32c.h" /* PIC32C system header. */
#include "driver/tmr/drv_tmr.h"

#define NUMBER_OF_TIMER_MODULES		4

#define CHANNEL_PER_MODULE			3

//This structure is used to the base address of the Timer counter module
typedef struct
{
	tc_registers_t *hw;

}TC_Module_Table;

void tmr_stop
(
	TMR_MODULE_ID id
);

void tmr_32bit_stop
(
	TMR_MODULE_ID id
);

void tmr_start
(
	TMR_MODULE_ID id
);

void tmr_32bit_start
(
	TMR_MODULE_ID id
);

void tmr_ClockSourceSelect
(
	TMR_MODULE_ID id,
	DRV_TMR_CLK_SOURCES clockSource
);

void tmr_32bit_ClockSourceSelect
(
	TMR_MODULE_ID id,
	DRV_TMR_CLK_SOURCES clockSource
);

bool tmr_interrupt_status
(
	TMR_MODULE_ID id
);

uint32_t tmr_counter_get
(
	TMR_MODULE_ID id
);

uint32_t tmr_counter_32bit_get
(
	TMR_MODULE_ID id
);

void tmr_counter_clear
(
	TMR_MODULE_ID id
);


void tmr_counter_32bit_clear
(
	TMR_MODULE_ID id
);

void tmr_period_set
(
	TMR_MODULE_ID id,
	uint16_t period
);


void tmr_period_32bit_set
(
	TMR_MODULE_ID id,
	uint32_t period
);

void tmr_setup
(
	TMR_MODULE_ID id
);
void tmr_32bit_setup
(
    TMR_MODULE_ID id
);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END


#endif // _SYSTEM_CONFIG_H
/*******************************************************************************
 End of File
*/
