/*******************************************************************************
  Interrupt System Service Interface Definition

  Company:
    Microchip Technology Inc.

  File Name:
    sys_int_private.h

  Summary:
    Interrupt System Service.
    
  Description:
    This file contains the interface definition for the Interrupt System
    Service.  It provides a way to interact with the interrupt sub-system to
    manage the occurance of interrupts for sources supported by the system.
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

#ifndef _SYS_INT_PRIVATE_H
#define _SYS_INT_PRIVATE_H


// *****************************************************************************
// *****************************************************************************
// Section: Includes
// *****************************************************************************
// *****************************************************************************

#ifdef __PIC32MX__

#include "peripheral/int/plib_int_private_pic32.h"

#endif


// *****************************************************************************
// *****************************************************************************
// Section: Extern Declarations
// *****************************************************************************
// *****************************************************************************

extern SYS_INT_TASKS_POINTER _SYS_INT_isr[INT_IRQ_MAX];

extern SYS_MODULE_OBJ _SYS_INT_taskparam_source[INT_IRQ_MAX];



#endif // _SYS_INT_PRIVATE_H

