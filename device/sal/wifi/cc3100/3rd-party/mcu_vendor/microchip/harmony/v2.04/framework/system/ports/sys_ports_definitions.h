/*******************************************************************************
  PORTS System Service Library Interface Definition

  Company:
    Microchip Technology Inc.

  File Name:
    sys_ports_definitions.h

  Summary:
    PORTS System Service data type definitions header.

  Description:
    This file contains data type definitions header.
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

#ifndef _SYS_PORTS_DEFINITIONS_H
#define _SYS_PORTS_DEFINITIONS_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************
#include "system_config.h"
#include "system/common/sys_common.h"
#include "system/common/sys_module.h"
#include "peripheral/ports/plib_ports.h"
#if defined(SYS_INT)
#include "system/int/sys_int.h"
#endif
// *****************************************************************************
// *****************************************************************************
// Section: SYS PORTS Data Types
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
/* SYS PORTS PULLUP status

  Summary:
    Provides the pull-up and pull-down status.

  Description:
    These constants provide the pull-up or pull-down status definitions.

  Remarks:
    None.

*/

typedef enum
{
    /* PULLUP Disable */
    SYS_PORTS_PULLUP_DISABLE
        /*DOM-IGNORE-BEGIN*/  = 0 /*DOM-IGNORE-END*/,
    /* PULLUP Enable */
    SYS_PORTS_PULLUP_ENABLE
        /*DOM-IGNORE-BEGIN*/  = 1 /*DOM-IGNORE-END*/,

} SYS_PORTS_PULLUP_PULLDOWN_STATUS;

// *****************************************************************************
/* SYS PORTS PIN DIRECTION

  Summary:
    Defines the direction of the port pins.

  Description:
    These constants provide the port pin direction definitions.

  Remarks:
    None.

*/

typedef enum
{
    /* Direction as output */
    SYS_PORTS_DIRECTION_OUTPUT
        /*DOM-IGNORE-BEGIN*/  = 0 /*DOM-IGNORE-END*/,
    /* Direction as input */
    SYS_PORTS_DIRECTION_INPUT
        /*DOM-IGNORE-BEGIN*/  = 1 /*DOM-IGNORE-END*/,

} SYS_PORTS_PIN_DIRECTION;


typedef enum {

    PORTS_PIN_INTERRUPT_NONE

} PORTS_PIN_INTERRUPT_TYPE;

#endif // #ifndef _SYS_PORTS_DEFINITIONS_H
/*******************************************************************************
 End of File
*/