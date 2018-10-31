/*******************************************************************************
  Reset Service Implementation

  Company:
    Microchip Technology Inc.

  File Name:
    sys_reset.c

  Summary:
    Reset Service interface implementation.

  Description:
    The RESET system service provides a simple interface to manage the DEVICE
    RESET on Microchip microcontrollers. This file Implements the core
    interface routines for the RESET system service. While building the system
    service from source, ALWAYS include this file in the build.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright 2017 Microchip Technology Incorporated and its subsidiaries.

Permission is hereby granted, free of charge, to any person obtaining a copy of 
this software and associated documentation files (the "Software"), to deal in 
the Software without restriction, including without limitation the rights to 
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies 
of the Software, and to permit persons to whom the Software is furnished to do 
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all 
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
SOFTWARE
*******************************************************************************/
//DOM-IGNORE-END
// *****************************************************************************
// *****************************************************************************
// Section: Macro Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "system/reset/sys_reset.h"
#include <sys/appio.h>
#include "system/system.h"
#include "system/devcon/sys_devcon.h"
#include "peripheral/devcon/plib_devcon.h"

// *****************************************************************************
/* Function:
    RESET_REASON SYS_RESET_ReasonGet( void )

  Summary:
    Returns the reason for the reset.

  Description:
    This function identifies the reason that the most recent reset occurred.

  PreCondition:
    None.

  Parameters:
    None.

  Returns:
    A value identifying the reason for the most recent reset.

  Example:
    <code>
    RESET_REASON reasonType;
    reasonType = SYS_RESET_ReasonGet();
    </code>

  Remarks:
    None.
*/

RESET_REASON SYS_RESET_ReasonGet( void )
{
	RESET_REASON resetReason;
	resetReason = PLIB_RESET_ReasonGet ( RESET_ID_0 );
	return resetReason;
}


// *****************************************************************************
/* Function:
     void SYS_RESET_ReasonClear( RESET_REASON reason )

   Summary:
    Clears the status flag for the specified reset reason.

   Description:
    This function clears the specified status flag(s) that were previously set
    by the processor to identify the reason for the most recent reset.

  PreCondition:
    Hardware should have set the reset reason.

  Parameters:
    reason          - One of the possible values of the enum RESET_REASON

   Returns:
     None.

   Example:
    <code>
    // To clear a single reset reason,
    SYS_RESET_ReasonClear( RESET_REASON_MCLR );

    // To clear more than one reason,
    SYS_RESET_ReasonClear( RESET_REASON_MCLR|RESET_REASON_POWERON );

    // To clear all the reasons,
    SYS_RESET_ReasonClear( RESET_REASON_ALL );
    </code>

   Remarks:
    None.
*/

void SYS_RESET_ReasonClear( RESET_REASON reason )
{
	PLIB_RESET_ReasonClear( RESET_ID_0, reason );
}

// *****************************************************************************
/* Function:
    void SYS_RESET_SoftwareReset( void )

  Summary:
    Triggers a software reset.

  Description:
    This function triggers a processor.

  PreCondition:
    None.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    SYS_RESET_SoftwareReset();
    </code>

  Remarks:
    The software reset feature must be supported by the processor in use.
*/
void SYS_RESET_SoftwareReset( void )
{
    PLIB_DEVCON_SystemUnlock(DEVCON_ID_0);
	#if defined(PLIB_RESET_ExistsSoftwareResetTrigger)
    if (PLIB_RESET_ExistsSoftwareResetTrigger(RESET_ID_0))
	{
        PLIB_RESET_SoftwareResetEnable(RESET_ID_0);
	}
	#endif
}

// *****************************************************************************
/* Function:
   void SYS_RESET_NMIDelayCountSet( RESET_NMI_COUNT_TYPE nmi_count )

  Summary:
    Sets the Delay for WDT/DMT NMI reset events.

  Description:
    This function sets the NMI counter value, that is equivalent to number of
    SYSCLK cycles before a device Reset to occur when a WDT/DMT NMI event is triggered.
    This NMI reset counter is only applicable to these two specific NMI events only.

  Precondition:
    None.

  Parameters:
	nmi_count - NMI counter value.

  Returns:
    None

  Example:
    <code>
     SYS_RESET_NMIDelayCountSet( 0x54 );
    </code>

  Remarks:
    The NMI feature must be supported by the processor in use.
 */

void  SYS_RESET_NMIDelayCountSet( RESET_NMI_COUNT_TYPE nmi_count )
{
    PLIB_DEVCON_SystemUnlock(DEVCON_ID_0);

    #if defined(PLIB_RESET_ExistsNmiCounter)
    if (PLIB_RESET_ExistsNmiCounter(RESET_ID_0))
        PLIB_RESET_NmiCounterValueSet( RESET_ID_0,  nmi_count);
    #endif

    PLIB_DEVCON_SystemLock(DEVCON_ID_0);

}
