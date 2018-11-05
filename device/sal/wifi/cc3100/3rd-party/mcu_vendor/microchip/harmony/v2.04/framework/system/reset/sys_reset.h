/*******************************************************************************
  Reset System Service Interface Definition

  Company:
    Microchip Technology Inc.

  File Name:
    sys_reset.h

  Summary:
    Reset System Service interface definition.

  Description:
    This file contains the interface definition for the Reset System
    Service.  It provides a way to interact with the Reset subsystem to
    manage the system resets and their status.
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

#ifndef SYS_RESET_H
#define SYS_RESET_H


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************


#include "system/system.h"
#include "peripheral/reset/plib_reset.h"

#ifdef __cplusplus  // Provide C++ Compatibility
extern "C" {
#endif

// *****************************************************************************
/* Macro: _SYS_RESET_NMI_SUPPORT

  Summary:
    This is for the SYS_RESET_NMIDelayCountSet feature existence.

  Description:
    This throws a warning whenever SYS_RESET_NMIDelayCountSet
	is used for the unsupported devices..
*/

#if !defined(PLIB_RESET_ExistsNmiCounter)
    #undef SYS_RESET_PART_SPECIFIC
    #define SYS_RESET_PART_SPECIFIC   __attribute__((unsupported("The microcontroller selected does not implement this feature.")))
#endif
#ifndef _SYS_RESET_PART_SPECIFIC
    #undef SYS_RESET_PART_SPECIFIC
    #define SYS_RESET_PART_SPECIFIC
#endif

// *****************************************************************************
// *****************************************************************************
// Section: SYS RESET Data Types
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
// *****************************************************************************
// Section: SYS RESET Module Routines
// *****************************************************************************
// *****************************************************************************


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

RESET_REASON SYS_RESET_ReasonGet( void );


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

void SYS_RESET_ReasonClear( RESET_REASON reason );


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

void SYS_RESET_SoftwareReset( void );


//******************************************************************************
/* Function:
   void SYS_RESET_NMIDelayCountSet( RESET_NMI_COUNT_TYPE nmi_count )

  Summary:
    Sets the Delay for WDT/DMT NMI reset events.

  Description:
    This function sets the NMI counter value, that is equivalent to
    the number of SYSCLK cycles needed for the device Reset to occur
    when a WDT/DMT NMI event is triggered.
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

void /*DOM-IGNORE-BEGIN*/ SYS_RESET_PART_SPECIFIC /*DOM-IGNORE-END*/ SYS_RESET_NMIDelayCountSet( RESET_NMI_COUNT_TYPE nmi_count );

#ifdef __cplusplus  // Provide C++ Compatibility
}
#endif

#endif /* SYS_RESET_H */

/*******************************************************************************
 End of File
*/
