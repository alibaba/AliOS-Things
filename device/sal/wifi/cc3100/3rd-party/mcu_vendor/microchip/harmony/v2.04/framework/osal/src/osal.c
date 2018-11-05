/**************************************************************************
  Basic OSAL compatibility layer definitions
  
  Company:
    Microchip Technology Inc.

  File Name:
    osal.c
	
  Summary:
    Bare metal implementation of OSAL functions
	
  Description:
    Basic OSAL compatibility layer.
    
    This file contains functional implementations of the bare metal OSAL.
    Where it is logical and possible to implement non-threading versions of
    the common function calls they have been implemented here directly or
    in the osal.h file as macros. If it is not possible, or logical, to
    implement a feature WITHOUT an RTOS present, the matching
    definition in osal.h will be set to a NULL (or empty) implementation.  
  **************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2011-2013 released Microchip Technology Inc.  All rights reserved.

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
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************
/*  This section lists the other files that are included in this file.  
 */

#include <xc.h>
#include "osal/osal.h"


/* this file is only used in a OSAL_USE_BASIC version of code */
#if defined(OSAL_USE_BASIC)

// *****************************************************************************
// *****************************************************************************
// Section: File Scope or Global Constants
// *****************************************************************************
// *****************************************************************************
/*  A brief description of a section can be given directly below the section
    banner.
*/

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Data Types
// *****************************************************************************
// *****************************************************************************
/*  A brief description of a section can be given directly below the section
    banner.
*/

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Helper Macros
// *****************************************************************************
// *****************************************************************************
/*  A brief description of a section can be given directly below the section
    banner.
*/


#endif // defined(OSAL_USE_BASE)

/*******************************************************************************
 End of File
*/
