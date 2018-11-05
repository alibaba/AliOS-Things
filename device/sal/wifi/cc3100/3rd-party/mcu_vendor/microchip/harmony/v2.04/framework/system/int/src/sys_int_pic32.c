/*******************************************************************************
  Interrupt System Service

  Company:
    Microchip Technology Inc.

  File Name:
    sys_int_pic32.c

  Summary:
    Interrupt System Service APIs.

  Description:
    This file contains functions related to the Interrupt System Service for PIC32
    devices.
*******************************************************************************/

// DOM-IGNORE-BEGIN
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
// DOM-IGNORE-END

#include "system/int/sys_int.h"

// *****************************************************************************
// *****************************************************************************
// Section: Function Definitions
// *****************************************************************************
// *****************************************************************************


/******************************************************************************

  Function:
    void SYS_INT_Initialize ( void )

  Summary:
    Configures and initializes the interrupt sub-system.

  Description:
    This function appropriately configures and initializes the interrupt sub-system
    for the current system design.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.

  Example:
  <code>
  SYS_INT_Initialize();
  </code>

  Remarks:
    This function is not implemented in the System Interrupt library.  It is
    implemented by the board support package (using the processor-specific
    interrupt peripheral library) because it requires knowledge of the specific
    interrupt requirements for each system.
*/

void SYS_INT_Initialize ( void )

{
    /* enable the multi vector */
    PLIB_INT_MultiVectorSelect( INT_ID_0 );
}

//*******************************************************************************
/*  Function:
     SYS_INT_PROCESSOR_STATUS  SYS_INT_StatusGetAndDisable ( void )

  Summary:
    Disables interrupts to the processor and return the previois status.
    
  Description:
    This function disables interrupts to the processor at the top level. This
    function can be called to prevent any source from being able to generate an
    interrupt. It returns the processor status (which includes global interrupt
    status and Interrupt Priority status) before disabling the interrupts.

  Remarks:
    See sys_int.h file for more details.
*/

SYS_INT_PROCESSOR_STATUS  SYS_INT_StatusGetAndDisable ( void )
{
    SYS_INT_PROCESSOR_STATUS processorStatus;

    /* Save the processor status and then Disable the global interrupt */
    processorStatus = ( SYS_INT_PROCESSOR_STATUS )__builtin_disable_interrupts();

    /* return the processor status */
    return processorStatus;
}

//*******************************************************************************
/* Function:
    void SYS_INT_StatusRestore ( SYS_INT_PROCESSOR_STATUS processorStatus )

  Summary:
    Restores the processor status.

  Description:
    This function sets the processor status based on the 32 bit value passed as 
    a parameter. 0th bit of the status is for Master Interrupt status.

  Remarks:
    See sys_int.h file for more details.
*/

void SYS_INT_StatusRestore ( SYS_INT_PROCESSOR_STATUS processorStatus )
{
    __builtin_mtc0(12, 0, processorStatus);
}

// *****************************************************************************
/*  Disable the generation of interrupts to the CPU

  Summary:
     Disables all interrupts

  Description:
     This function disables all interrupts.

  Remarks:
    This API will be depricated soon. Use "SYS_INT_StatusGetAndDisable" instead.
 */

bool SYS_INT_Disable( void )
{
    SYS_INT_PROCESSOR_STATUS processorStatus;

    /* Save the processor status and then Disable the global interrupt */
    processorStatus = ( SYS_INT_PROCESSOR_STATUS )__builtin_disable_interrupts();

    /* return the interrupt status */
    return (bool)(processorStatus & 0x01);
}

// *****************************************************************************
/* Function:
    bool SYS_INT_SourceDisable ( INT_SOURCE source )

  Summary:
    Disables the interrupt source.

  Description:
    This routine disables the given source from generating interrupts the 
    processor when events occur.It returns the interrupt source enable/disable 
	status before disabling the interrupt source.

  Remarks:
    See sys_int.h file for more details.
 */


bool SYS_INT_SourceDisable ( INT_SOURCE source )
{
    bool intSrcStatus;
    SYS_INT_PROCESSOR_STATUS processorStatus;

    /* Save the processor status and then Disable the global interrupt */
    processorStatus = ( SYS_INT_PROCESSOR_STATUS )__builtin_disable_interrupts();
    
    /* get the interrupt status of this source before disable is called */
    intSrcStatus = PLIB_INT_SourceIsEnabled (INT_ID_0 , source);

    /* disable the interrupts */
    PLIB_INT_SourceDisable (INT_ID_0 , source);

    /* restore the state of CP0 Status register before the disable occurred*/
    __builtin_mtc0(12, 0, processorStatus);

    /* return the source status */
    return intSrcStatus;
}


// *****************************************************************************
/* Sets the external interrupt trigger type.

  Summary:
    Sets the external interrupt trigger type.

  Description:
    This function sets the External interrupt trigger type.

  Remarks:
    Refer the datasheet to know the external interrupt sources supported.
 */

void SYS_INT_ExternalInterruptTriggerSet ( 	INT_EXTERNAL_SOURCES source,
										INT_EXTERNAL_EDGE_TRIGGER edgeTrigger )
{
	if ( edgeTrigger == INT_EDGE_TRIGGER_RISING )
	{
		PLIB_INT_ExternalRisingEdgeSelect ( INT_ID_0, source );
	}
	else
	{
		PLIB_INT_ExternalFallingEdgeSelect ( INT_ID_0, source );
	}
}

