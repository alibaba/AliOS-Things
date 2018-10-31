/*******************************************************************************
  Clock System Service Feature Variant Implementations

  Company:
    Microchip Technology Inc.

  File Name:
    sys_clk_variant_mapping.h

  Summary:
    Clock System Service feature variant implementations.

  Description:
    This file implements the functions that differ based on different devices
    and various implementations of the same feature.
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


#ifndef _SYS_CLK_VARIANT_MAPPING_H
#define _SYS_CLK_VARIANT_MAPPING_H

// *****************************************************************************
// *****************************************************************************
// Section: Symbolic Constants
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Macro: REFERENCE_BUSES_MAX

  Summary:
    This is to avoid build error for the devices which does not have
    Reference clock module.

  Description:
    This is to avoid build error for the devices which does not have
    Reference clock module. If the reference clock module exists, the condition
    will fail because this macro will be defined in the template file.
*/

#ifndef REFERENCE_BUSES_MAX
    #define REFERENCE_BUSES_MAX     1
#endif


// *****************************************************************************
/* Macro: USB_BUSES_MAX

  Summary:
    This is to avoid build error for the devices which does not have
    USB clock module( or it is not accessible through software).

  Description:
    This is to avoid build error for the devices which does not have
    USB clock module( or it is not accessible through software). If the reference
    clock module exists, the condition will fail because this macro will be
    defined in the template file.
*/

#ifndef USB_BUSES_MAX
    #define USB_BUSES_MAX       1
#endif


// *****************************************************************************
/* Macro: CLK_REFERENCE_DIVISOR_MAX

  Summary:
    This is to avoid build error for the devices which does not have
    Reference clock module.

  Description:
    This is to avoid build error for the devices which does not have
    Reference clock module. If the reference clock module exists, the condition
    will fail because this macro will be defined in the template file.
*/

#ifndef CLK_REFERENCE_DIVISOR_MAX
    #define CLK_REFERENCE_DIVISOR_MAX   65535
#endif


// *****************************************************************************
/* Macro: CLK_REFERENCE_TRIM_MAX

  Summary:
    This is to avoid build error for the devices which does not have
    Reference clock module.

  Description:
    This is to avoid build error for the devices which does not have
    Reference clock module. If the reference clock module exists, the condition
    will fail because this macro will be defined in the template file.
*/

#ifndef CLK_REFERENCE_TRIM_MAX
    #define CLK_REFERENCE_TRIM_MAX   511
#endif


// *****************************************************************************
/* Macro: SYS_CLOCK_USB_CLOCK

  Summary:
    This is to avoid build error for the devices which has a configurable USB
    clock.

  Description:
     This is to avoid build error for the devices which has a configurable USB
    clock. 
*/

#ifndef SYS_CLK_UPLL_BEFORE_DIV2_FREQ
    #define SYS_CLK_UPLL_BEFORE_DIV2_FREQ     0
#endif


// *****************************************************************************
/* Macro: SYS_CLK_CONFIG_EXTERNAL_CLOCK

  Summary:
    This is to avoid build error for the applications which does not use the
    external clock input.

  Description:
    This is to avoid build error for the applications which does not use the
    external clock input. If the application want to use the External clock
    input he must define this macro with the value of the external clock input
    provided.
*/

#ifndef SYS_CLK_CONFIG_EXTERNAL_CLOCK
    #define SYS_CLK_CONFIG_EXTERNAL_CLOCK       0
#endif


// *****************************************************************************
/* Macro: SYS_CLK_BusNumberGet(x)

  Summary:
    This is to select bus number 1 to for devices which does not have
    multiple buses.

  Description:
     This is to select bus number 1 to for devices which does not have
    multiple buses.

*/
#if ( PERIPHERAL_BUSES_MAX > 1 )

    #define SYS_CLK_BusNumberGet(x)                     x

#else

    #define SYS_CLK_BusNumberGet(x)       		0

#endif


#endif //_SYS_CLK_VARIANT_MAPPING_H

/*******************************************************************************
 End of File
*/

