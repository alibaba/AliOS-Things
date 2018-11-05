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
Copyright (c) 2013-2017 released Microchip Technology Inc.  All rights reserved.

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

#ifndef SYS_RESET_H
#define SYS_RESET_H


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************


#include "system/system.h"
<#if CONFIG_ARCH_ARM == false>
#include "peripheral/reset/plib_reset.h"
</#if>

#ifdef __cplusplus  // Provide C++ Compatibility
extern "C" {
#endif

<#if CONFIG_ARCH_ARM == false>
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
</#if>

// *****************************************************************************
// *****************************************************************************
// Section: SYS RESET Data Types
// *****************************************************************************
// *****************************************************************************

<#if CONFIG_ARCH_ARM == true>
typedef enum
{
    RESET_REASON_NONE        = 0x00000000,
    RESET_REASON_POWERON     = 0x00000001,
    RESET_REASON_BROWNOUT    = 0x00000002,
    RESET_REASON_WDT_TIMEOUT = 0x00000004,
    RESET_REASON_USER        = 0x00000008,
    RESET_REASON_SOFTWARE    = 0x00000010,
    RESET_REASON_BACKUP_EXIT = 0x00000020,
    RESET_REASON_ALL         = 0x0000003F,
    RESET_REASON_INVALID     = 0xFFFFFFFF
} RESET_REASON;

/* RESET_NMI_COUNT_TYPE data type

  Summary:
    Defines NMI counter data type

  Description:
    NMI counter data type definition.
*/

typedef unsigned int RESET_NMI_COUNT_TYPE;
</#if>

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

<#if CONFIG_ARCH_ARM == false>
void /*DOM-IGNORE-BEGIN*/ SYS_RESET_PART_SPECIFIC /*DOM-IGNORE-END*/ SYS_RESET_NMIDelayCountSet( RESET_NMI_COUNT_TYPE nmi_count );
</#if>
<#if CONFIG_ARCH_ARM == true>
void SYS_RESET_NMIDelayCountSet( RESET_NMI_COUNT_TYPE nmi_count );
</#if>

#ifdef __cplusplus  // Provide C++ Compatibility
}
#endif

#endif /* SYS_RESET_H */

/*******************************************************************************
 End of File
*/
