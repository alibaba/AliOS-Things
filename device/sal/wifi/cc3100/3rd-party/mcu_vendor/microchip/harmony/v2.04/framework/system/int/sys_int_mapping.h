/*******************************************************************************
  Interrupt System Service Mapping File

  Company:
    Microchip Technology Inc.

  File Name:
    sys_int_mapping.h

  Summary:
    Interrupt System Service mapping file.

  Description:
    This header file contains the mapping of the APIs defined in the API header
    to either the function implementations or macro implementation or the
    specific variant implementation.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/******************************************************************************
Copyright (c) 2013-2015 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED 'AS IS' WITHOUT WARRANTY OF ANY KIND,
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

#ifndef _SYS_INT_MAPPING_H
#define _SYS_INT_MAPPING_H

#define _SYS_UNSUPPORTED _PLIB_UNSUPPORTED



// *****************************************************************************
// *****************************************************************************
// Section: INTERRUPT Controller Peripheral Library Interface Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Get the status of the interrupt

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

    #define SYS_INT_SourceStatusGet(source) \
        PLIB_INT_SourceFlagGet( INT_ID_0, source )



// *****************************************************************************
/* Set the status of the interrupt.

  Summary:
    Sets the status of the interrupt flag for the selected source.

  Description:
    Sets the status of the interrupt flag for the selected source. This function
    will not be used during normal operation of the system. It is used to
    generate test interrupts for debug and testing purposes.

  Remarks:
    This feature is not supported on all devices.  Refer to the specific device
    data sheet or family reference manual to determine whether this feature is
    supported.
 */


    #define SYS_INT_SourceStatusSet( source ) \
        PLIB_INT_SourceFlagSet( INT_ID_0,source )



// *****************************************************************************
/* Clear the interrupt status flag.

  Summary:
    Clears the status of the interrupt flag for the selected source.

  Description:
    Clears the status of the interrupt flag for the selected source. The flag
    is set when the interrupt request is sampled. The pending interrupt
    request will not cause further processing if the interrupt is not enabled
    using the function PLIB_INT_SourceEnable.

  Remarks:
    This feature is not supported on all devices.  Refer to the specific device
    data sheet or family reference manual to determine whether this feature is
    supported.
 */

    #define SYS_INT_SourceStatusClear( source ) \
        PLIB_INT_SourceFlagClear( INT_ID_0,source )


//*****************************************************************************
/* Enable the interrupt source

  Summary:
    Enables the interrupt source.

  Description:
    Enables the interrupt source. The interrupt flag is set when the interrupt
    request is sampled. The pending interrupt request will not cause further
    processing if the interrupt is not enabled using this function.

  Remarks:
    This feature is not supported on all devices.  Refer to the specific device
    data sheet or family reference manual to determine whether this feature is
    supported.
 */


    #define SYS_INT_SourceEnable( source ) \
        PLIB_INT_SourceEnable( INT_ID_0,source )


// *****************************************************************************
/* Identifies if the specified source is enabled or disabled.

  Summary:
    Identifies if the specified source is enabled or disabled.

  Description:
    This function identifies if the specified source is currently enabled or is
    currently disabled.

  Remarks:
    None.

*/


    #define SYS_INT_SourceIsEnabled( source ) \
        PLIB_INT_SourceIsEnabled(INT_ID_0,source)



// *****************************************************************************
/* Set the sub priority of the interrupt vector.

  Summary:
    Sets the sub-priority of the interrupt vector.

  Description:
    Sets the sub-priority of the interrupt vector. The priority is one of the
    possible values from PLIB_INT_SUBPRIORITY_LEVELS

  Remarks:
    This feature is not supported on all devices.  Refer to the specific device
    data sheet or family reference manual to determine whether this feature is
    supported.
 */


    #define SYS_INT_VectorSubprioritySet(  vector,  subpriority ) \
        PLIB_INT_VectorSubPrioritySet(INT_ID_0,vector,subpriority)



// *****************************************************************************
/* Set the priority of the interrupt vector.

  Summary:
    Sets the priority of the interrupt vector.

  Description:
    This function sets the priority of the interrupt vector. The priority is one of the
    possible values from PLIB_INT_PRIORITY_LEVEL

  Remarks:
    None.


 */

    #define SYS_INT_VectorPrioritySet( vector, priority ) \
        PLIB_INT_VectorPrioritySet( INT_ID_0,vector, priority)


// *****************************************************************************
/*  Enable the generation of interrupts to the CPU

  Summary:
    Enables the generation of interrupts to the CPU .

  Description:
    This function enables the generation of interrupts to the CPU.

  Remarks:
    This feature is not supported on all devices.  Refer to the specific device
    data sheet or family reference manual to determine whether this feature is
    supported.
 */


    #define SYS_INT_Enable( ) \
        __builtin_mtc0(12, 0,(__builtin_mfc0(12, 0) | 0x0001))



// *****************************************************************************
/*  Get the global interrupt Enable/Disable status.

  Summary:
     Gets the global interrupt Enable/Disable status.

  Description:
     This function Gets the global interrupt Enable/Disable status.

  Remarks:
    None.
 */


    #define SYS_INT_IsEnabled( ) \
	    ((bool)(_CP0_GET_STATUS() & 0x01))


// *****************************************************************************
/* Shadow register set assign.

  Summary:
    Assigns a shadow register set for an interrupt priority level.

  Description:
    Assigns a shadow register set for an interrupt priority level.

  Remarks:
    This feature is not supported on all devices.  Refer to the specific device
    data sheet or family reference manual to determine whether this feature is
    supported.
 */


#define SYS_INT_ShadowRegisterAssign(priority, shadowRegister) \
        PLIB_INT_ShadowRegisterAssign(INT_ID_0, priority, shadowRegister)


// *****************************************************************************
/* Shadow register set get.

  Summary:
    Gets the shadow register set assigned for an interrupt priority level.

  Description:
    Gets the shadow register set assigned for an interrupt priority level.

  Remarks:
    This feature is not supported on all devices.  Refer to the specific device
    data sheet or family reference manual to determine whether this feature is
    supported.
 */


#define SYS_INT_ShadowRegisterGet(priority) \
        PLIB_INT_ShadowRegisterGet(INT_ID_0,priority)


/* currently dynamic register function is not supported */
#ifndef SYS_INT_DYNAMIC
#define  SYS_INT_DynamicRegister(  source, tasks, object )
#endif

/* currently dynamic deregister function is not supported */
#ifndef SYS_INT_DYNAMIC
#define  SYS_INT_DynamicDeregister( source )
#endif


#endif // _SYS_INT_MAPPING_PIC32_H

/*******************************************************************************
 End of File
*/