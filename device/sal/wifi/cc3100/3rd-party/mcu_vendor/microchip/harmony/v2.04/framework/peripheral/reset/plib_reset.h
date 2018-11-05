/*******************************************************************************
  Reset Peripheral Library Interface Header

  Company:
    Microchip Technology Inc.

  File Name:
    plib_reset.h

  Summary:
    Defines the Reset Peripheral Library interface.

  Description:
    This header file contains the function prototypes and definitions of
    the data types and constants that make up the interface to the Reset
    Peripheral Library for Microchip microcontrollers.  The definitions in 
    this file are for the Reset Controller module.
*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright 2013-2015 released Microchip Technology Inc.  All rights reserved.

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

#ifndef _PLIB_RESET_H
#define _PLIB_RESET_H

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Constants & Data Types
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
/* RESET_NMI_COUNT_TYPE data type

  Summary:
    Defines NMI counter data type

  Description:
    NMI counter data type definition.
*/

typedef 	unsigned int 	RESET_NMI_COUNT_TYPE;



// *****************************************************************************
// *****************************************************************************
// Section: Included Files (continued at end of file)
// *****************************************************************************
// *****************************************************************************
/*  This section lists the other files that are included in this file.  However,
    please see the end of the file for additional implementation header files
    that are also included
*/

#include "peripheral/reset/processor/reset_processor.h"



// *****************************************************************************
// *****************************************************************************
// Section: Peripheral Library Interface Routines
// *****************************************************************************
// *****************************************************************************

//******************************************************************************
/* Function:
    RESET_REASON PLIB_RESET_ReasonGet ( RESET_MODULE_ID index )
    
  Summary:
    Returns the reason for a reset.
    
  Description:
    This function returns the reason a reset has occurred for the selected
    device.
	
  Precondition:
    None.

  Parameters:
    index    - Identifier for the device instance to be configured

  Returns:
    RESET_REASON - Type of reset (when there is more than one reason for a reset, 
    this function will logically OR (bitwise) the reasons and return the value.
    
  Example:
    <code>
    RESET_REASON type;

    type = PLIB_RESET_ReasonGet ( RESET_ID_0 );
    </code>

  Remarks:
    This function implements an operation of the ResetReasonStatus feature. This 
    feature may not be available on all devices. Please refer to the specific 
    device data sheet to determine availability or include the 
    PLIB_RESET_ExistsResetReasonStatus function in your application to determine 
    whether this feature is available.
*/

RESET_REASON PLIB_RESET_ReasonGet ( RESET_MODULE_ID index );


//******************************************************************************
/* Function:
   void PLIB_RESET_ReasonClear( RESET_MODULE_ID index, RESET_REASON reason )
    
  Summary:
    Clears the RCON register.
    
  Description:
    This function clears the particular reset bit in the RCON register. Multiple 
    reasons for a reset can be ORed together and given as an input parameter to 
    clear them simultaneously.
	
  Precondition:
    None.

  Parameters:
    index    - Identifier for the device instance to be configured
    reason   - Select the reset type from enum RESET_REASON
	
  Returns:
    None
    
  Example:
    <code>
    PLIB_RESET_ReasonClear( RESET_ID_0, RESET_REASON_MCLR | RESET_REASON_POWERON );
    </code>

  Remarks:
    This function implements an operation of the ResetReasonStatus feature. This
    feature may not be available on all devices. Please refer to the specific 
    device data sheet to determine availability or include the 
    PLIB_RESET_ExistsResetReasonStatus function in your application to determine
    whether this feature is available.
*/

void PLIB_RESET_ReasonClear(RESET_MODULE_ID index, RESET_REASON reason);


//******************************************************************************
/* Function:
   void PLIB_RESET_SoftwareResetEnable ( RESET_MODULE_ID index )

  Summary:
    Enables the software reset.
	
  Description:
    This function triggers a software reset.

  Precondition:
    The system unlock sequence must be performed before calling 
	PLIB_RESET_SoftwareResetEnable. 

  Parameters:
    index    - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
	//Call system service to unlock the system
    PLIB_RESET_SoftwareResetEnable( RESET_ID_0 );
    </code>
	
  Remarks:
    This function implements an operation of the SoftwareResetTrigger feature. This
    feature may not be available on all devices. Please refer to the specific 
    device data sheet to determine availability or include the 
    PLIB_RESET_ExistsSoftwareResetTrigger function in your application to determine
    whether this feature is available.
 */

void PLIB_RESET_SoftwareResetEnable ( RESET_MODULE_ID index );


//******************************************************************************
/* Function:
   RESET_CONFIG_REG_READ_ERROR PLIB_RESET_ConfigRegReadErrorGet( RESET_MODULE_ID index );

  Summary:
    Gets the Configuration register read error.
	
  Description:
    This function returns the Configuration register read error, if any, after the reset.

  Precondition:
    None.

  Parameters:
    index    - Identifier for the device instance to be configured

  Returns:
    RESET_CONFIG_REG_READ_ERROR - Type of Configuration Register Read Error

  Example:
    <code>
    if (PLIB_RESET_ConfigRegReadErrorGet( RESET_ID_0 )== PRIMARY_CONFIG_REG_READ_ERROR )
	{
		//Do Something
	}
    </code>
	
  Remarks:
    This function implements an operation of the ConfigRegReadError feature. This
    feature may not be available on all devices. Refer to the specific 
    device data sheet to determine availability or include the 
    PLIB_RESET_ExistsConfigRegReadError function in your application to determine
    whether this feature is available.
 */

RESET_CONFIG_REG_READ_ERROR PLIB_RESET_ConfigRegReadErrorGet( RESET_MODULE_ID index );


//******************************************************************************
/* Function:
   RESET_NMI_REASON PLIB_RESET_NmiReasonGet ( RESET_MODULE_ID index )

  Summary:
    Returns the reason for the Non-Maskable Interrupt (NMI).
	
  Description:
    This function returns the reason that caused the NMI.

  Precondition:
    None.

  Parameters:
    index    - Identifier for the device instance to be configured

  Returns:
   RESET_NMI_REASON - Sets of reasons that can cause a NMI.

  Example:
    <code>
    if (PLIB_RESET_NmiReasonGet( RESET_ID_0 )== WDTO_NMI )
	{
		//Do Something
	}
    </code>
	
  Remarks:
    This function implements an operation of the NmiControl feature. This
    feature may not be available on all devices. Please refer to the specific 
    device data sheet to determine availability or include the 
    PLIB_RESET_ExistsNmiControl function in your application to determine
    whether this feature is available.
 */

RESET_NMI_REASON PLIB_RESET_NmiReasonGet ( RESET_MODULE_ID index );


//******************************************************************************
/* Function:
   void PLIB_RESET_NmiEventTrigger ( RESET_MODULE_ID index, RESET_NMI_REASON nmi_reason )

  Summary:
    Triggers the NMI event. 
	
  Description:
    This function triggers the NMI. In case of a Deadman Timer (DMT) or Watchdog Timer (WDT)
	NMI event, it will also trigger the NMI counter to start the countdown.

  Precondition:
    The system unlock sequence must be performed before calling 
	PLIB_RESET_NmiEventTrigger.

  Parameters:
    index      - Identifier for the device instance to be configured
	nmi_reason - Sets of reasons which can cause NMI
	
  Returns:
    None

  Example:
    <code>
	//Call system service to unlock the system
    PLIB_RESET_NmiEventTrigger( RESET_ID_0, SOFTWARE_NMI );
	//Call system service to lock the system
    </code>
	
  Remarks:
    This function implements an operation of the NmiControl feature. This
    feature may not be available on all devices. Please refer to the specific 
    device data sheet to determine availability or include the 
    PLIB_RESET_ExistsNmiControl function in your application to determine
    whether this feature is available.
 */

void PLIB_RESET_NmiEventTrigger ( RESET_MODULE_ID index, RESET_NMI_REASON nmi_reason );

//******************************************************************************
/* Function:
   void PLIB_RESET_NmiEventClear ( RESET_MODULE_ID index, RESET_NMI_REASON nmi_reason )

  Summary:
    Clears the NMI event 
	
  Description:
    This function clears the NMI event. If a WDTO or DMTO NMI event is cleared before 
	the NMI counter reaches zero, no device Reset is asserted.

  Precondition:
    The system unlock sequence must be performed before calling 
	PLIB_RESET_NmiEventTrigger.

  Parameters:
    index      - Identifier for the device instance to be configured
	nmi_reason - Sets of reasons that can cause a NMI
	
  Returns:
    None

  Example:
    <code>
	//Call system service to unlock the system	
    PLIB_RESET_NmiEventClear( RESET_ID_0, SOFTWARE_NMI );
	//Call system service to lock the system	
    </code>
	
  Remarks:
    This function implements an operation of the NmiControl feature. This
    feature may not be available on all devices. Please refer to the specific 
    device data sheet to determine availability or include the 
    PLIB_RESET_ExistsNmiControl function in your application to determine
    whether this feature is available.
 */

void PLIB_RESET_NmiEventClear ( RESET_MODULE_ID index, RESET_NMI_REASON nmi_reason );


//******************************************************************************
/* Function:
   bool PLIB_RESET_WdtTimeOutHasOccurredInSleep ( RESET_MODULE_ID index )

  Summary:
    Returns the state of the device when WDT time-out occurred
	
  Description:
    This function returns whether or not the the device was in Sleep mode when 
	a WDT time-out event  occurred.

  Precondition:
    None.

  Parameters:
    index    - Identifier for the device instance to be configured

  Returns:
	- true  - The device was in Sleep mode when a WDT time-out occurred
	- false - The device was not in Sleep mode when a WDT time-out occurred
  Example:
    <code>
    if (PLIB_RESET_WdtTimeOutHasOccurredInSleep( RESET_ID_0 ))
	{
		//Do Something
	}
    </code>
	
  Remarks:
    This function implements an operation of the WdtoInSleep feature. This
    feature may not be available on all devices. Please refer to the specific 
    device data sheet to determine availability or include the 
    PLIB_RESET_ExistsWdtoInSleep function in your application to determine
    whether this feature is available.
 */

bool PLIB_RESET_WdtTimeOutHasOccurredInSleep ( RESET_MODULE_ID index );

//******************************************************************************
/* Function:
   void PLIB_RESET_NmiCounterValueSet ( RESET_MODULE_ID index, RESET_NMI_COUNT_TYPE nmi_count )

  Summary:
    Sets the NMI counter.
	
  Description:
    This function sets the NMI counter to be expired for a WDT or DMT reset.

  Precondition:
    The system unlock sequence must be performed before calling
	PLIB_RESET_NmiCounterValueSet

  Parameters:
    index     - Identifier for the device instance to be configured
	nmi_count - NMI counter value
	
  Returns:
    None.

  Example:
    <code>
	//Call system service to unlock the system
    PLIB_RESET_NmiCounterValueSet( RESET_ID_0,  0x54);
	//Call system service to lock the system
    </code>
	
  Remarks:
	NMI counter value range may vary between devices. Please refer to the 
	specific device data sheet.
    This function implements an operation of the NmiCounter feature. This
    feature may not be available on all devices. Please refer to the specific 
    device data sheet to determine availability or include the 
    PLIB_RESET_ExistsNmiCounter function in your application to determine
    whether this feature is available.
 */

void PLIB_RESET_NmiCounterValueSet ( RESET_MODULE_ID index, RESET_NMI_COUNT_TYPE nmi_count );

//******************************************************************************
/* Function:
   RESET_NMI_COUNT_TYPE PLIB_RESET_NmiCounterValueGet ( RESET_MODULE_ID index )

  Summary:
    Gets the NMI counter value.
	
  Description:
    This function returns the NMI Reset counter value. 

  Precondition:
    None.

  Parameters:
    index    - Identifier for the device instance to be configured
	
  Returns:
    nmi_count - NMI counter value

  Example:
    <code>
	RESET_NMI_COUNT_TYPE nmi_count;
    nmi_count = PLIB_RESET_NmiCounterValueGet( RESET_ID_0);
    </code>
	
  Remarks:
    This function implements an operation of the NmiCounter feature. This
    feature may not be available on all devices. Please refer to the specific 
    device data sheet to determine availability or include the 
    PLIB_RESET_ExistsNmiCounter function in your application to determine
    whether this feature is available.
 */

RESET_NMI_COUNT_TYPE PLIB_RESET_NmiCounterValueGet ( RESET_MODULE_ID index );

// *****************************************************************************
// *****************************************************************************
// Section: RESET Peripheral Library Exists Functions
// *****************************************************************************
// *****************************************************************************
/* The following functions indicate the existence of the features on the device. 
*/

//******************************************************************************
/* Function:
    PLIB_RESET_ExistsResetReasonStatus( RESET_MODULE_ID index )

  Summary:
    Identifies whether the ResetReasonStatus feature exists on the Reset module. 

  Description:
    This function identifies whether the ResetReasonStatus feature is available 
	on the Reset module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_RESET_ReasonGet
    - PLIB_RESET_ReasonClear

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ResetReasonStatus feature is supported on the device
    - false  - The ResetReasonStatus feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_RESET_ExistsResetReasonStatus( RESET_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_RESET_ExistsSoftwareResetTrigger( RESET_MODULE_ID index )

  Summary:
    Identifies whether the SoftwareResetTrigger feature exists on the Reset 
	module. 

  Description:
    This function identifies whether the SoftwareResetTrigger feature is available 
	on the Reset module.
    When this function returns true, this function is supported on the device: 
    - PLIB_RESET_SoftwareResetEnable

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The SoftwareResetTrigger feature is supported on the device
    - false  - The SoftwareResetTrigger feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_RESET_ExistsSoftwareResetTrigger( RESET_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_RESET_ExistsConfigRegReadError( RESET_MODULE_ID index )

  Summary:
    Identifies whether the ConfigRegReadError feature exists on the Reset module. 

  Description:
    This function identifies whether the ConfigRegReadError feature is available 
	on the Reset module.
    When this function returns true, this function is supported on the device: 
    - PLIB_RESET_ConfigRegReadErrorGet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ConfigRegReadError feature is supported on the device
    - false  - The ConfigRegReadError feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_RESET_ExistsConfigRegReadError( RESET_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_RESET_ExistsNmiControl( RESET_MODULE_ID index )

  Summary:
    Identifies whether the NmiControl feature exists on the Reset module. 

  Description:
    This function identifies whether the NmiControl feature is available on the 
	Reset module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_RESET_NmiReasonGet
    - PLIB_RESET_NmiEventTrigger
    - PLIB_RESET_NmiEventClear

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The NmiControl feature is supported on the device
    - false  - The NmiControl feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_RESET_ExistsNmiControl( RESET_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_RESET_ExistsWdtoInSleep( RESET_MODULE_ID index )

  Summary:
    Identifies whether the WdtoInSleep feature exists on the Reset module. 

  Description:
    This function identifies whether the WdtoInSleep feature is available on the 
	Reset module.
    When this function returns true, this function is supported on the device: 
    - PLIB_RESET_WdtTimeOutHasOccurredInSleep

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The WdtoInSleep feature is supported on the device
    - false  - The WdtoInSleep feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_RESET_ExistsWdtoInSleep( RESET_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_RESET_ExistsNmiCounter( RESET_MODULE_ID index )

  Summary:
    Identifies whether the NmiCounter feature exists on the Reset module. 

  Description:
    This function identifies whether the NmiCounter feature is available on the 
	Reset module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_RESET_NmiCounterValueSet
    - PLIB_RESET_NmiCounterValueGet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The NmiCounter feature is supported on the device
    - false  - The NmiCounter feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_RESET_ExistsNmiCounter( RESET_MODULE_ID index );

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif //#ifndef _PLIB_RESET_H
/*******************************************************************************
 End of File
*/