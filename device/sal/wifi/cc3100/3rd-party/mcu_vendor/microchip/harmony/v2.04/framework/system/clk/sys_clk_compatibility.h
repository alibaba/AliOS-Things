/*******************************************************************************
  SYS CLK Static Interface Declarations for Clock System Service

  Company:
    Microchip Technology Inc.

  File Name:
    sys_clk_compatibility.h

  Summary:
    SYS CLK interface declarations for the static system service.

  Description:
    The Clock System Service provides a simple interface to manage the oscillators
    on Microchip microcontrollers. This file defines the interface
    Declarations for the functions which are deprecated from SYS CLK system service.
    
  Remarks:
    Static interfaces incorporate all system clock configuration settings as
    determined by the user via the Microchip Harmony Configurator GUI.  It provides 
    static version of the routines, eliminating the need for an object ID or 
    object handle.
    
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2014 released Microchip Technology Inc.  All rights reserved.

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

#ifndef _SYS_CLK_COMPATIBILITY_H
#define _SYS_CLK_COMPATIBILITY_H

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END 

// *****************************************************************************
// *****************************************************************************
// Section: SYS CLK Module Deprecated Functions
// *****************************************************************************
// *****************************************************************************
//******************************************************************************
/* Function:
    uint32_t SYS_CLK_ReferenceClockFrequencyGet ( CLK_BUSES_REFERENCE referenceBus )

  Summary:
    Gets the selected Reference clock bus frequency in Hertz.
	<p><b>Implementation:</b> Static</p>

  Description:
    This function gets frequency of the selected Reference clock bus in Hertz.

  Precondition:
	The SYS_CLK_Initialize function should be called before calling this API.

  Parameters:
	peripheralBus - Reference clock bus selection. One of the possible value from
				    CLK_BUSES_REFERENCE enum. For devices that do not have multiple
				    clock channels for Reference clock, CLK_BUS_REFERENCE_1 should be
				    the selection.

  Returns:
    Clock frequency in Hertz.

  Example:
    <code>
    unsigned long sysClockOutputHz;

    sysClockOutputHz = SYS_CLK_ReferenceClockFrequencyGet ( CLK_BUS_REFERENCE_3 );
    </code>

  Remarks:
    None.
 */
#define SYS_CLK_ReferenceClockFrequencyGet(referenceBus)      SYS_CLK_ReferenceFrequencyGet(referenceBus)


//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // #ifndef _SYS_CLK_COMPATIBILITY_H
/*******************************************************************************
 End of File
*/
