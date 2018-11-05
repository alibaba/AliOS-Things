/*******************************************************************************
  Clock System Service Local Data Structures

  Company:
    Microchip Technology Inc.

  File Name:
    sys_clk_local.h

  Summary:
    Clock System Service local declarations and definitions.

  Description:
    This file contains the Clock System Service local declarations and
    definitions.
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
//DOM-IGNORE-END

#ifndef _SYS_CLK_LOCAL_H
#define _SYS_CLK_LOCAL_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "system/clk/sys_clk.h"
#include "system/clk/src/sys_clk_variant_mapping.h"
#include "system/devcon/sys_devcon.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END 

// *****************************************************************************
// *****************************************************************************
// Section: Data Type Definitions
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
/* Oscillator module ID used by the PLIB

  Summary:
    Configures the modules ID used by the peripheral library.

  Description:
    This macro configures the modules Id used by the peripheral library.

  Remarks:
    None.
*/

#define OSC_PLIB_ID                                                 OSC_ID_0


// *****************************************************************************
/* Oscillator System Service Hardware Instance Object

  Summary:
    Defines the object required for the maintenance of the hardware.

  Description:
    This defines the object required for the maintenance of the hardware.

  Remarks:
    None.
*/

typedef struct _SYS_CLK_OBJ_STRUCT
{
    /* This variable holds the system clock value. It is also
    used to calculate the other clocks which are derived from the system clock */
    uint32_t systemClock;

    /* Save the clock frequency which is closest to the previously
       requested frequency */
    uint32_t systemClosestClock;

    /* System clock source */
   CLK_SOURCES_SYSTEM systemClockSource;

    /* Status of the previous System clock Operation */
    SYS_CLK_STATUS systemClockStatus;

    /* This variable holds the peripheral clock value.  */
    uint32_t peripheralClock[PERIPHERAL_BUSES_MAX];

    SYS_CLK_STATUS peripheralClockStatus[PERIPHERAL_BUSES_MAX];

    uint32_t peripheralClosestClock[PERIPHERAL_BUSES_MAX];

    /* This variable holds the USB clock value.  */
    uint32_t usbClock[USB_BUSES_MAX];

    SYS_CLK_STATUS usbClockStatus[USB_BUSES_MAX];

    uint32_t usbClosestClock[USB_BUSES_MAX];

    /* This variable holds the Reference clock value. */
    uint32_t referenceClock[REFERENCE_BUSES_MAX];

    SYS_CLK_STATUS referenceClockStatus[REFERENCE_BUSES_MAX];

    uint32_t referenceClosestClock[REFERENCE_BUSES_MAX];

    SYS_CLK_ERROR_HANDLER callback;

} SYS_CLK_OBJECT;

extern SYS_CLK_OBJECT clkObject;


uint32_t _SYS_CLK_SystemClockRead ( void );

bool _SYS_CLK_PeripheralFrequencySet ( CLK_BUSES_PERIPHERAL peripheralBus,
        uint32_t peripheralClockHz,
        uint32_t *clockClosest );

bool _SYS_CLK_ReferenceFrequencySet ( CLK_BUSES_REFERENCE referenceBus, CLK_SOURCES_REFERENCE referenceSource,
            uint32_t referenceClockHz, bool waitUntilComplete, uint32_t *clockClosest );

void _SYS_CLK_ReferenceClockSetup ( CLK_BUSES_REFERENCE referenceBus, SYS_CLK_REFERENCE_SETUP *refSetup );

bool _SYS_CLK_SystemClockSet ( CLK_SOURCES_SYSTEM systemSource,
	uint32_t systemClockHz, bool waitUntilComplete, uint32_t *clockClosest );

uint32_t _SYS_CLK_PeripheralClockRead ( CLK_BUSES_PERIPHERAL peripheralBus, uint32_t systemClockHz);

OSC_PERIPHERAL_BUS _SYS_CLK_GetOscPeripheralBus ( CLK_BUSES_PERIPHERAL peripheralBus );

CLK_SOURCES_SYSTEM _SYS_CLK_ClockTypeMap( OSC_SYS_TYPE systemSource );

OSC_SYS_TYPE _SYS_CLK_ClockSourceMap (CLK_SOURCES_SYSTEM clockSource);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif //#ifndef _SYS_CLK_LOCAL_H

/*******************************************************************************
 End of File
*/

