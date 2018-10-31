/*******************************************************************************
  Interrupt System Service

  Company:
    Microchip Technology Inc.

  File Name:
    sys_int_pic32c.c

  Summary:
    Interrupt System Service APIs.

  Description:
    This file contains functions related to the Interrupt System Service for PIC32
    devices.
*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2016 released Microchip Technology Inc.  All rights reserved.

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
    NVIC_SetPriorityGrouping( 0x04 );
}

//*******************************************************************************
/*  Function:
     SYS_INT_PROCESSOR_STATUS  SYS_INT_StatusGetAndDisable ( void )

  Summary:
    Disables interrupts to the processor and return the previous status.
    
  Description:
    This function disables interrupts to the processor at the top level. This
    function can be called to prevent any source from being able to generate an
    interrupt. It returns the processor status (which includes global interrupt
    status) before disabling the interrupts.

  Remarks:
    See sys_int.h file for more details.
*/

SYS_INT_PROCESSOR_STATUS  SYS_INT_StatusGetAndDisable ( void )
{   
    SYS_INT_PROCESSOR_STATUS processorStatus;

    processorStatus = (SYS_INT_PROCESSOR_STATUS) (__get_PRIMASK() == 0);

    __disable_irq();
	__DMB();
    
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
    if( processorStatus )
    {
		__DMB();
		__enable_irq();
    }
}

//*******************************************************************************
/*  Function:
     void SYS_INT_Enable ( void )

  Summary:
    Enables global interrupts to the processor.
    
  Description:
    This function enables interrupts to the processor at the top level, allowing
    any currently enabled source to generate an interrupt.  This function must be
    called before any source will be able to generate an interrupt.

  Remarks:
        See sys_int.h file for more details.
*/

void SYS_INT_Enable ( void )
{
    __DMB();		
    __enable_irq();
}

// *****************************************************************************
/*  Function:
     bool SYS_INT_Disable( void )

  Summary:
     Disables all interrupts

  Description:
     This function disables all interrupts.

  Remarks:
    This API will be depricated soon. Use "SYS_INT_StatusGetAndDisable" instead.
 */

bool SYS_INT_Disable( void )
{
    bool interrupts_were_enabled;

    interrupts_were_enabled = (bool)(__get_PRIMASK() == 0);

    __disable_irq();
	__DMB();

    /* return the interrupt status */
    return interrupts_were_enabled;
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
    SYS_INT_PROCESSOR_STATUS processorStatus;
	bool intSrcStatus;

    processorStatus = SYS_INT_StatusGetAndDisable();

    intSrcStatus = (NVIC->ISER[(uint32_t)((int32_t)source) >> 5] & 
        ((uint32_t)(1 << ((uint32_t)((int32_t)source) & (uint32_t)0x1F))))?1:0;

    NVIC_DisableIRQ( source );

    SYS_INT_StatusRestore( processorStatus );
    
    /* return the source status */
    return intSrcStatus;
}

// *****************************************************************************
/* Function:
    bool SYS_INT_SourceIsEnabled( INT_SOURCE source )

  Summary:
    Identifies if the specified source is enabled or disabled.

  Description:
    This function identifies if the specified source is currently enabled or is
    currently disabled.

  Remarks:
    None.

*/

bool SYS_INT_SourceIsEnabled( INT_SOURCE source )
{        
    return ((NVIC->ISER[(uint32_t)((int32_t)source) >> 5] &
            (uint32_t)(1 << ((uint32_t)((int32_t)source) & (uint32_t)0x1F)))?1:0);
}

// *****************************************************************************
/* Function:
    bool SYS_INT_SourceStatusGet ( INT_SOURCE source )

  Summary:
    Returns the status of the interrupt flag for the selected source.

  Description:
    Returns the status of the interrupt flag for the selected source. The flag
    is set when the interrupt request is sampled. The pending interrupt
    request will not cause further processing if the interrupt is not enabled
    using the function PLIB_INT_SourceEnable.

  Remarks:
    This feature is not supported on all devices.  Refer to the specific device
    data sheet or family reference manual to determine whether this feature is
    supported.
 */

bool SYS_INT_SourceStatusGet ( INT_SOURCE source )
{
    return (NVIC_GetPendingIRQ( source )?1:0);
}

// *****************************************************************************
/* Function:
    void SYS_INT_VectorPrioritySet( INT_VECTOR vector, INT_PRIORITY_LEVEL priority )

  Summary:
    Sets the priority of the interrupt source.

  Description:
    This function sets the priority of the interrupt source. The priority is one of the
    possible values from INT_PRIORITY_LEVEL

  Remarks:
    None.


 */

void SYS_INT_VectorPrioritySet( INT_VECTOR vector, INT_PRIORITY_LEVEL priority )
{
    SYS_INT_PROCESSOR_STATUS processorStatus;
    
	processorStatus = SYS_INT_StatusGetAndDisable();

    NVIC_SetPriority( vector, priority );

    SYS_INT_StatusRestore( processorStatus );
}

/*******************************************************************************
 End of File
*/


