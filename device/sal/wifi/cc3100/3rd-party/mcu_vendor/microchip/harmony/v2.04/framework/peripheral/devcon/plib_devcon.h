/*******************************************************************************
  Device Control (DEVCON) Peripheral Library Interface Header

  Company:
    Microchip Technology Inc.

  File Name:
    plib_devcon.h

  Summary:
    Defines the Device Control Peripheral Library Interface.

  Description:
    This header file contains the function prototypes and definitions of
    the data types and constants that make up the interface to the Device Control
    Peripheral Library for Microchip microcontrollers. The definitions in this
    file are for the Device Control module.
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

#ifndef _PLIB_DEVCON_H
#define _PLIB_DEVCON_H

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END 

// *****************************************************************************
// *****************************************************************************
// Section: Constants and Data Types
// *****************************************************************************
// *****************************************************************************

//*****************************************************************************
/* Alternate Clock Targets Enumeration

  Summary:
    Selects Input Capture or Output Compare modules.

  Description:
    This enumeration selects the Input Capture or Output Compare module for 
	enabling or disabling the alternate clock.

  Remarks:
    None.
*/
typedef enum {

    // Input Capture
    DEVCON_ALT_CLOCK_OC  /*DOM-IGNORE-BEGIN*/ = 1 /*DOM-IGNORE-END*/,

    // Output Compare
    DEVCON_ALT_CLOCK_IC  /*DOM-IGNORE-BEGIN*/ = 2 /*DOM-IGNORE-END*/

} DEVCON_ALT_CLOCK_TARGET;


//*****************************************************************************
/* USB Clock In Sleep Mode Enumeration

  Summary:
    Selects whether the USB clock operates in Sleep mode.

  Description:
    This enumeration selects whether the USB clock operates in Sleep mode.

  Remarks:
    None.
*/
typedef enum {

    // USB PHY clock is shut down when Sleep mode is active.
    DEVCON_USB_NO_CLOCK_IN_SLEEP  /*DOM-IGNORE-BEGIN*/ = 0 /*DOM-IGNORE-END*/,

    // USP PHY clock continues to run when Sleep mode is active
    DEVCON_USB_CLOCK_IN_SLEEP     /*DOM-IGNORE-BEGIN*/ = 1 /*DOM-IGNORE-END*/

} DEVCON_USB_SLEEP_MODE;


//*****************************************************************************
/* Module Registers for Lock/Unlock enumeration.

  Summary:
    Selects module registers for write lock or unlock.

  Description:
    This enumeration selects the module registers for write lock or unlock.

  Remarks:
    Can be ORed together.
*/
typedef enum {

    // Peripheral Pin Select registers
    DEVCON_PERMISSION_GROUP_REGISTERS  /*DOM-IGNORE-BEGIN*/ = 1 /*DOM-IGNORE-END*/,

    // Peripheral Module Disable registers
    DEVCON_PMD_REGISTERS  /*DOM-IGNORE-BEGIN*/ = 2 /*DOM-IGNORE-END*/,

    // Permission Group registers
    DEVCON_PPS_REGISTERS  /*DOM-IGNORE-BEGIN*/ = 4 /*DOM-IGNORE-END*/,

    // All lockable registers
    DEVCON_ALL_REGISTERS  /*DOM-IGNORE-BEGIN*/ = 7 /*DOM-IGNORE-END*/

} DEVCON_REGISTER_SET;


//*****************************************************************************
/* Flash Error Correcting Code (ECC) Configuration enumeration

  Summary:
    Selects how ECC is applied to Flash memory.

  Description:
    This enumeration selects how ECC is applied to Flash memory.

  Remarks:
    None.
*/
typedef enum {

    // ECC and dynamic ECC are disabled
    DEVCON_ECC_DISABLED        /*DOM-IGNORE-BEGIN*/ = 3 /*DOM-IGNORE-END*/,

    // ECC and dynamic ECC are disabled and the configuration locked
    DEVCON_ECC_DISABLED_LOCKED /*DOM-IGNORE-BEGIN*/ = 2 /*DOM-IGNORE-END*/,

    // Dynamic Flash ECC is enabled and the configuration is locked
    DEVCON_DYN_ECC_ENABLED_LOCKED  /*DOM-IGNORE-BEGIN*/ = 1 /*DOM-IGNORE-END*/,

    // Flash ECC is enabled, the configuration is locked, word Flash writes disabled
    DEVCON_FLASH_ECC_ENABLED_LOCKED  /*DOM-IGNORE-BEGIN*/ = 0 /*DOM-IGNORE-END*/

} DEVCON_ECC_CONFIG;


//*****************************************************************************
/* Ignore Debugger Freeze for Peripheral Module(s) enumeration

  Summary:
    Sets modules to ignore debugger's freeze command.

  Description:
    This enumeration sets modules to ignore the debugger's freeze command.

  Remarks:
    None.
*/
typedef enum {

    // USB module
    DEVCON_DEBUG_USB   /*DOM-IGNORE-BEGIN*/ =  1 /*DOM-IGNORE-END*/,

    // UART 1
    DEVCON_DEBUG_UART1 /*DOM-IGNORE-BEGIN*/ =  2 /*DOM-IGNORE-END*/,

    // UART 2
    DEVCON_DEBUG_UART2 /*DOM-IGNORE-BEGIN*/ =  4 /*DOM-IGNORE-END*/,

    // SPI 1
    DEVCON_DEBUG_SPI1  /*DOM-IGNORE-BEGIN*/ =  8 /*DOM-IGNORE-END*/,

    // USB, UART 1, UART 2, and SPI 1
    DEVCON_DEBUG_ALL   /*DOM-IGNORE-BEGIN*/ = 15 /*DOM-IGNORE-END*/

} DEVCON_DEBUG_PERIPHERAL;


//*****************************************************************************
/* MPLL Output Divider bits enumeration

  Summary:
    Specifies the MPLL Output divider bits.

  Description:
    This enumeration specifies the MPLL Output divider bits.

  Remarks:
    None.
*/
typedef enum {

    DEVCON_MPLL_ODIV_1   /*DOM-IGNORE-BEGIN*/ =  1 /*DOM-IGNORE-END*/,

    DEVCON_MPLL_ODIV_2   /*DOM-IGNORE-BEGIN*/ =  2 /*DOM-IGNORE-END*/,

    DEVCON_MPLL_ODIV_3   /*DOM-IGNORE-BEGIN*/ =  3 /*DOM-IGNORE-END*/,

    DEVCON_MPLL_ODIV_4   /*DOM-IGNORE-BEGIN*/ =  4 /*DOM-IGNORE-END*/,

    DEVCON_MPLL_ODIV_5   /*DOM-IGNORE-BEGIN*/ =  5 /*DOM-IGNORE-END*/,

    DEVCON_MPLL_ODIV_6   /*DOM-IGNORE-BEGIN*/ =  6 /*DOM-IGNORE-END*/,

    DEVCON_MPLL_ODIV_7   /*DOM-IGNORE-BEGIN*/ =  7 /*DOM-IGNORE-END*/,

} DEVCON_MPLL_OUTPUT_DIVIDER;


//*****************************************************************************
/* MPLL VREF Control enumeration

  Summary:
    VREF control.

  Description:
    This enumeration provides VREF control.

  Remarks:
    None.
*/
typedef enum {

    DEVCON_MPLL_VREF_EXT   /*DOM-IGNORE-BEGIN*/ =  0 /*DOM-IGNORE-END*/,

    DEVCON_MPLL_VREF_VDD   /*DOM-IGNORE-BEGIN*/ =  1 /*DOM-IGNORE-END*/,

    DEVCON_MPLL_VREF_VSS   /*DOM-IGNORE-BEGIN*/ =  2 /*DOM-IGNORE-END*/,

    DEVCON_MPLL_VREF_INT   /*DOM-IGNORE-BEGIN*/ =  3 /*DOM-IGNORE-END*/,

} DEVCON_MPLL_VREF_CONTROL;


// *****************************************************************************
// *****************************************************************************
// Section: Includes
// *****************************************************************************
// *****************************************************************************

/* See the end of the file for implementation header include files. */

// DOM-IGNORE-BEGIN
#include "peripheral/devcon/processor/devcon_processor.h"
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Peripheral Library Interface Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    void PLIB_DEVCON_AlternateClockEnable(DEVCON_MODULE_ID index,
                                          DEVCON_ALT_CLOCK_TARGET targetAltClock )

  Summary:
    Selects the alternate clock source for Input Capture or Output Compare modules.

  Description:
    This function selects the alternate clock source for the Input Capture or 
	Output Compare modules.

  Precondition:
    None.

  Parameters:
    index - Always DEVCON_ID
    targetAltClock - DEVCON_ALT_CLOCK_IC or DEVCON_ALT_CLOCK_OC

  Returns:
    None.

  Example:
  <code>
    // Call system service to unlock oscillator
    PLIB_DEVCON_AlternateClockEnable(DEVCON_ID, DEVCON_ALT_CLOCK_IC || DEVCON_ALT_CLOCK_OC );
  </code>

  Remarks:
    The feature is not supported on all devices.  Refer to the specific device
	data sheet to determine availability.
    A system unlock must be performed before this function can be executed.
*/

void PLIB_DEVCON_AlternateClockEnable(DEVCON_MODULE_ID index,
                                      DEVCON_ALT_CLOCK_TARGET targetAltClock);


// *****************************************************************************
/* Function:
    void PLIB_DEVCON_AlternateClockDisable(DEVCON_MODULE_ID index,
                                           DEVCON_ALT_CLOCK_TARGET targetAltClock )

  Summary:
    Disables the alternate clock source for Input Capture or Output Compare modules,
    The primary clock source will be used instead.

  Description:
    This function disables the alternate clock source for the Input Capture or 
	Output Compare modules. The primary clock source will be used instead.

  Precondition:
    None.

  Parameters:
    index - Always DEVCON_ID
    targetAltClock - DEVCON_ALT_CLOCK_IC or DEVCON_ALT_CLOCK_OC

  Returns:
    None.

  Example:
  <code>
    // Call system service to unlock oscillator
    PLIB_DEVCON_AlternateClockDisable(DEVCON_ID, DEVCON_ALT_CLOCK_IC || DEVCON_ALT_CLOCK_OC );
  </code>

  Remarks:
    The feature is not supported on all devices.  Refer to the specific device
	data sheet to determine availability.
    A system unlock must be performed before this function can be executed.
*/

void PLIB_DEVCON_AlternateClockDisable(DEVCON_MODULE_ID index,
                                       DEVCON_ALT_CLOCK_TARGET targetAltClock);


// *****************************************************************************
/* Function:
    void PLIB_DEVCON_DeviceRegistersLock(DEVCON_MODULE_ID index,
                                         DEVCON_REGISTER_SET registersToLock )

  Summary:
    Locks device module registers, preventing modifications to the registers.

  Description:
    This function locks device module registers, preventing modifications to 
	the registers.

  Precondition:
    None.

  Parameters:
    index - Always DEVCON_ID
    registersToLock - element from DEVCON_REGISTER_SET, which can be ORed together

  Returns:
    None.

  Example:
  <code>
    // Call system service to unlock oscillator
    PLIB_DEVCON_DeviceRegistersLock(DEVCON_ID,  DEVCON_ALL_REGISTERS);
    PLIB_DEVCON_DeviceRegistersUnlock(DEVCON_ID,DEVCON_PPS_REGISTERS);
  </code>

  Remarks:
    The feature is not supported on all devices.  Refer to the specific device
	data sheet to determine availability.
    A system unlock must be performed before this function can be executed.
*/

void PLIB_DEVCON_DeviceRegistersLock(DEVCON_MODULE_ID index,
                                     DEVCON_REGISTER_SET registersToLock );


// *****************************************************************************
/* Function:
    void PLIB_DEVCON_DeviceRegistersUnlock(DEVCON_MODULE_ID index,
                                           DEVCON_REGISTER_SET registersToLock )

  Summary:
    Unlocks device module registers, allowing modifications to the registers.

  Description:
    This function unlocks device module registers, allowing modifications to 
	the registers.

  Precondition:
    None.

  Parameters:
    index - Always DEVCON_ID
    registersToLock - element from DEVCON_REGISTER_SET, which can be ORed together

  Returns:
    None.

  Example:
  <code>
    // Call system service to unlock oscillator
    PLIB_DEVCON_DeviceRegistersLock(DEVCON_ID,  DEVCON_ALL_REGISTERS);
    PLIB_DEVCON_DeviceRegistersUnlock(DEVCON_ID,DEVCON_PPS_REGISTERS);
  </code>

  Remarks:
    The feature is not supported on all devices.  Refer to the specific device
	data sheet to determine availability.
    A system unlock must be performed before this function can be executed.
*/

void PLIB_DEVCON_DeviceRegistersUnlock(DEVCON_MODULE_ID index,
                                       DEVCON_REGISTER_SET registersToLock );


// *****************************************************************************
/* Function:
    void PLIB_DEVCON_USBPHYSleepModeSet(DEVCON_MODULE_ID index,
                                        DEVCON_USB_SLEEP_MODE sleepOrRun)

  Summary:
    Selects USB PHY clocking during Sleep mode.

  Description:
    This function selects USB PHY clocking during Sleep mode.

  Precondition:
    None.

  Parameters:
    index - Always DEVCON_ID
    sleepOrRun - DEVCON_USB_NO_CLOCK_IN_SLEEP or DEVCON_USB_CLOCK_IN_SLEEP

  Returns:
    None.

  Example:
  <code>
    // Call system service to unlock oscillator
    PLIB_DEVCON_USBPHYSleepModeSet(DEVCON_ID,DEVCON_USB_NO_CLOCK_IN_SLEEP)
  </code>

  Remarks:
    The feature is not supported on all devices.  Refer to the specific device
	data sheet to determine availability.
    A system unlock must be performed before this function can be executed.

 */
void PLIB_DEVCON_USBPHYSleepModeSet(DEVCON_MODULE_ID index,
                                    DEVCON_USB_SLEEP_MODE sleepOrRun);


// *****************************************************************************
/* Function:
    void PLIB_DEVCON_FlashErrCorrectionModeSet(DEVCON_MODULE_ID index,
                                               DEVCON_ECC_CONFIG flashECC)
  Summary:
    Sets Flash error correction operation.

  Description:
    This function sets Flash error correction operation.

  Precondition:
    None.

  Parameters:
    index - Always DEVCON_ID
    flashECC - DEVCON_ECC_DISABLED, DEVCON_ECC_DISABLED_LOCKED,
               DEVCON_DYN_ECC_ENABLED_LOCKED, or DEVCON_FLASH_ECC_ENABLED_LOCKED

  Returns:
    None.

  Example:
  <code>
  </code>

  Remarks:
    The feature is not supported on all devices.  Refer to the specific device
	data sheet to determine availability.
    Once ECC has been locked, it cannot be unlocked except through a system reset.
*/
void PLIB_DEVCON_FlashErrCorrectionModeSet(DEVCON_MODULE_ID index,
                                           DEVCON_ECC_CONFIG flashECC);


// *****************************************************************************
/* Function:
    void PLIB_DEVCON_JTAGPortEnable(DEVCON_MODULE_ID index)

  Summary:
    Enables the JTAG port on the device.

  Description:
    This function enables the JTAG port on the device.

  Precondition:
    None.

  Parameters:
    index - Always DEVCON_ID

  Returns:
    None.

  Remarks:
    None.
*/

void PLIB_DEVCON_JTAGPortEnable(DEVCON_MODULE_ID index);


// *****************************************************************************
/* Function:
    void PLIB_DEVCON_JTAGPortDisable(DEVCON_MODULE_ID index)

  Summary:
    Disables the JTAG port on the device.

  Description:
    This function disables the JTAG port on the device.

  Precondition:
    None.

  Parameters:
    index - Always DEVCON_ID

  Returns:
    None.

  Remarks:
    None.
*/

void PLIB_DEVCON_JTAGPortDisable(DEVCON_MODULE_ID index);

// *****************************************************************************
/* Function:
    void PLIB_DEVCON_AnalogIOChargePumpEnable(DEVCON_MODULE_ID index)

  Summary:
    Enables the I/O Analog Charge Pump on the device.

  Description:
    This function enables the I/O Analog Charge Pump on the device.

  Precondition:
    None.

  Parameters:
    index - Always DEVCON_ID

  Returns:
    None.

  Remarks:
    None.
*/

void PLIB_DEVCON_AnalogIOChargePumpEnable(DEVCON_MODULE_ID index);

// *****************************************************************************
/* Function:
    void PLIB_DEVCON_AnalogIOChargePumpDisable(DEVCON_MODULE_ID index)

  Summary:
    Disables the I/O Analog Charge Pump on the device.

  Description:
    This function disables the I/O Analog Charge Pump on the device.

  Precondition:
    None.

  Parameters:
    index - Always DEVCON_ID

  Returns:
    None.

  Remarks:
    None.
*/

void PLIB_DEVCON_AnalogIOChargePumpDisable(DEVCON_MODULE_ID index);
// *****************************************************************************
/* Function:
    void PLIB_DEVCON_TraceOutputEnable(DEVCON_MODULE_ID index)

  Summary:
    Enables trace outputs and the start trace clock.

  Description:
    This function enables trace outputs and the start trace clock (trace probe 
	must be present).

  Precondition:
    None.

  Parameters:
    index - Always DEVCON_ID

  Returns:
    None.

  Remarks:
*/

void PLIB_DEVCON_TraceOutputEnable(DEVCON_MODULE_ID index);


// *****************************************************************************
/* Function:
    void PLIB_DEVCON_TraceOutputDisable(DEVCON_MODULE_ID index)

  Summary:
    Disables trace outputs and the stop trace clock.

  Description:
    This function disables trace outputs and the stop trace clock.

  Precondition:
    None.

  Parameters:
    index - Always DEVCON_ID

  Returns:
    None.

  Remarks:
*/

void PLIB_DEVCON_TraceOutputDisable(DEVCON_MODULE_ID index);


// *****************************************************************************
/* Function:
    void PLIB_DEVCON_2WireJTAGEnableTDO(DEVCON_MODULE_ID index)

  Summary:
    Enables 2-Wire JTAG protocol to use TDO.

  Description:
    This function enables 2-Wire JTAG protocol to use TDO.

  Precondition:
    None.

  Parameters:
    index - Always DEVCON_ID

  Returns:
    None.

  Remarks:
    The feature is not supported on all devices. Refer to the specific device
	data sheet to determine availability.
*/

void PLIB_DEVCON_2WireJTAGEnableTDO(DEVCON_MODULE_ID index);


// *****************************************************************************
/* Function:
    void PLIB_DEVCON_2WireJTAGDisableTDO(DEVCON_MODULE_ID index)

  Summary:
    Disables 2-Wire JTAG protocol use of TDO.

  Description:
    This function disables 2-Wire JTAG protocol use of TDO.

  Precondition:
    None.

  Parameters:
    index - Always DEVCON_ID

  Returns:
    None.

  Remarks:
    The feature is not supported on all devices. Refer to the specific device
	data sheet to determine availability.
*/

void PLIB_DEVCON_2WireJTAGDisableTDO(DEVCON_MODULE_ID index);


// *****************************************************************************
/* Function:
    void PLIB_DEVCON_IgnoreDebugFreezeEnable(DEVCON_MODULE_ID index,
                                             DEVCON_DEBUG_PERIPHERAL myPeripheral )

  Summary:
    Allows module to ignore FREEZE command from debugger and continue running.

  Description:
    This function allows the module to ignore the FREEZE command from the debugger 
	and continue running.

  Precondition:
    None.

  Parameters:
    index - Always DEVCON_ID
    myPeripheral - DEVCON_DEBUG_USB, DEVCON_DEBUG_UART1, DEVCON_DEBUG_UART2,
                    DEVCON_DEBUG_SPI1, or  DEVCON_DEBUG_ALL (for all modules)

  Returns:
    None.

  Example:
  <code>
    PLIB_DEVCON_DebugIgnoreFreezeEnable(DEVCON_ID,DEVCON_DEBUG_ALL);
    PLIB_DEVCON_DebugIgnoreFreezeDisable(DEVCON_ID,DEVCON_DEBUG_SPI1);
  </code>

  Remarks:
    The feature is not supported on all devices. Refer to the specific device
	data sheet to determine availability.
    Peripherals can be ORed together.
*/

void PLIB_DEVCON_IgnoreDebugFreezeEnable(DEVCON_MODULE_ID index,
                                         DEVCON_DEBUG_PERIPHERAL myPeripheral );


// *****************************************************************************
/* Function:
    void PLIB_DEVCON_IgnoreDebugFreezeDisable(DEVCON_MODULE_ID index,
                                              DEVCON_DEBUG_PERIPHERAL myPeripheral )

  Summary:
    Module stops when commanded by debugger.

  Description:
    This function stops the module when commanded by the debugger.

  Precondition:
    None.

  Parameters:
    index - Always DEVCON_ID
    myPeripheral - DEVCON_DEBUG_USB, DEVCON_DEBUG_UART1, DEVCON_DEBUG_UART2,
                    DEVCON_DEBUG_SPI1, or  DEVCON_DEBUG_ALL (for all modules)

  Returns:
    None.

  Example:
  <code>
    PLIB_DEVCON_DebugIgnoreFreezeEnable(DEVCON_ID,DEVCON_DEBUG_ALL);
    PLIB_DEVCON_DebugIgnoreFreezeDisable(DEVCON_ID,DEVCON_DEBUG_SPI1);
  </code>

  Remarks:
    The feature is not supported on all devices. Refer to the specific device
	data sheet to determine availability.
    Peripherals can be ORed together.
*/

void PLIB_DEVCON_IgnoreDebugFreezeDisable(DEVCON_MODULE_ID index,
                                          DEVCON_DEBUG_PERIPHERAL myPeripheral );


// *****************************************************************************
/* Function:
    uint8_t PLIB_DEVCON_DeviceVersionGet(DEVCON_MODULE_ID index)

  Summary:
    Gets the device version.

  Description:
    This functions returns the device version.

  Precondition:
    None.

  Parameters:
    index - Always DEVCON_ID

  Returns:
    The version of the device.

  Remarks:
    None.
*/

uint8_t PLIB_DEVCON_DeviceVersionGet(DEVCON_MODULE_ID index);


// *****************************************************************************
/* Function:
    uint32_t PLIB_DEVCON_DeviceIdGet(DEVCON_MODULE_ID index)

  Summary:
    Gets the device identifier.

  Description:
    This function returns the device identifier.

  Precondition:
    None.

  Parameters:
    index - Always DEVCON_ID

  Returns:
    The version of the device.

  Remarks:
    None.
*/

uint32_t PLIB_DEVCON_DeviceIdGet(DEVCON_MODULE_ID index);


//******************************************************************************
/* Function :  PLIB_DEVCON_SystemLock( DEVCON_MODULE_ID index )

  Summary:
    Executes the system lock sequence.

  Description:
    This function executes the system lock sequence.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    None.

  Remarks:
    Should only be called after PLIB_DEVCON_SystemUnlock and the action that
    required the unlock have been performed.
*/

void PLIB_DEVCON_SystemLock( DEVCON_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_DEVCON_SystemUnlock( DEVCON_MODULE_ID index )

  Summary:
    Executes the system unlock sequence.

  Description:
    This function executes the system unlock sequence.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    None.

  Remarks:
    None.
*/

void PLIB_DEVCON_SystemUnlock( DEVCON_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_DEVCON_MPLLIsReady( DEVCON_MODULE_ID index )

  Summary:
    Reads MPLL status.

  Description:
    This function reads MPLL status.
	
  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true - MPLL clock is stable
    - false - MPLL clock is not stable

  Remarks:
    None.
*/

bool PLIB_DEVCON_MPLLIsReady( DEVCON_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_DEVCON_MPLLEnable( DEVCON_MODULE_ID index )

  Summary:
    Enables the MPLL.

  Description:
    This function enables the MPLL.
	This function performs atomic register access.
	
  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    None.

  Remarks:
    None.
*/

void PLIB_DEVCON_MPLLEnable( DEVCON_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_DEVCON_MPLLDisable( DEVCON_MODULE_ID index )

  Summary:
    Disables the MPLL.

  Description:
    This function disables the MPLL.
	This function performs atomic register access.
	
  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    None.

  Remarks:
    None.
*/

void PLIB_DEVCON_MPLLDisable( DEVCON_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_DEVCON_MPLLODiv1Set( DEVCON_MODULE_ID index, DEVCON_MPLL_OUTPUT_DIVIDER bits )

  Summary:
    Sets the MPLL output divider 1 bits.

  Description:
    This function sets the MPLL output divider 1 bits.
	This function performs atomic register access.
	
  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance
    bits            - Enumerated divider value

  Returns:
    None.

  Remarks:
    None.
*/

void PLIB_DEVCON_MPLLODiv1Set( DEVCON_MODULE_ID index, DEVCON_MPLL_OUTPUT_DIVIDER bits );


//******************************************************************************
/* Function :  PLIB_DEVCON_MPLLODiv2Set( DEVCON_MODULE_ID index, DEVCON_MPLL_OUTPUT_DIVIDER bits )

  Summary:
    Sets the MPLL output divider 2 bits.

  Description:
    This function sets the MPLL output divider 2 bits.
	This function performs atomic register access.
	
  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance
    bits            - Enumerated divider value

  Returns:
    None.

  Remarks:
    None.
*/

void PLIB_DEVCON_MPLLODiv2Set( DEVCON_MODULE_ID index, DEVCON_MPLL_OUTPUT_DIVIDER bits );


//******************************************************************************
/* Function :  PLIB_DEVCON_MPLLVregIsReady( DEVCON_MODULE_ID index )

  Summary:
    Reads the MPLL VREG status.

  Description:
    This function reads the MPLL VREG status.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true - MPLL VREG is ready
    - false - MPLL VREG is not ready

  Remarks:
    None.
*/

bool PLIB_DEVCON_MPLLVregIsReady( DEVCON_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_DEVCON_MPLLVregEnable( DEVCON_MODULE_ID index )

  Summary:
    Enables the MPLL VREG.

  Description:
    This function enables the MPLL VREG.
	This function performs atomic register access.
	
  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    None.

  Remarks:
    None.
*/

void PLIB_DEVCON_MPLLVregEnable( DEVCON_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_DEVCON_MPLLVregDisable( DEVCON_MODULE_ID index )

  Summary:
    Disables the MPLL VREG.

  Description:
    This function disables the MPLL VREG.
	This function performs atomic register access.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    None.

  Remarks:
    None.
*/

void PLIB_DEVCON_MPLLVregDisable( DEVCON_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_DEVCON_MPLLMultiplierSet( DEVCON_MODULE_ID index, uint8_t value )

  Summary:
    Sets the MPLL Multiplier bits.

  Description:
    This function sets the MPLL Multiplier bits.
	This function performs atomic register access.
	
  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance
    value           - Multiplier value (between 16 & 160)

  Returns:
    None.

  Remarks:
    None.
*/

void PLIB_DEVCON_MPLLMultiplierSet( DEVCON_MODULE_ID index, uint8_t value );


//******************************************************************************
/* Function :  PLIB_DEVCON_MPLLInputDivSet( DEVCON_MODULE_ID index, uint8_t value )

  Summary:
    Sets the MPLL Input Divider bits.

  Description:
    This function sets the MPLL Input Divider bits.
	This function performs atomic register access.
	
  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance
    value           - Multiplier value (between 1 & 63)

  Returns:
    None.

  Remarks:
    None.
*/

void PLIB_DEVCON_MPLLInputDivSet( DEVCON_MODULE_ID index, uint8_t value );


//******************************************************************************
/* Function :  PLIB_DEVCON_MPLLVrefSet( DEVCON_MODULE_ID index, DEVCON_MPLL_VREF_CONTROL vref )

  Summary:
    Sets the VREF control setting.

  Description:
    This function sets the VREF control setting.
	This function performs atomic register access.
	
  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance
    value           - Enumerated VREF Control setting

  Returns:
    None.

  Remarks:
    None.
*/

void PLIB_DEVCON_MPLLVrefSet( DEVCON_MODULE_ID index, DEVCON_MPLL_VREF_CONTROL vref );

//******************************************************************************
/* Function : void PLIB_DEVCON_OTPConfigLock( DEVCON_MODULE_ID index, DEVCON_CFGLOCK lockType)

  Summary:
    Locks or unlocks the configuration registers.

  Description:
    This function locks or unlocks the configuration register as per the locktype
	specified.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance
    lockType        - Enumerated config lock value

  Returns:
    None.

  Remarks:
    None.
*/

void PLIB_DEVCON_OTPConfigLock( DEVCON_MODULE_ID index, DEVCON_CFGLOCK lockType);

//******************************************************************************
/* Function :  void PLIB_DEVCON_OTPConfigUnlock( DEVCON_MODULE_ID index)

  Summary:
     unlocks the configuration registers.

  Description:
    This function unlocks the configuration register provided, the
	CFGLOCK field is not to a value to locked until the reset.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    None.

  Remarks:
    None.
*/

void PLIB_DEVCON_OTPConfigUnlock( DEVCON_MODULE_ID index);

//******************************************************************************
/* Function :  void PLIB_DEVCON_BootIPFSelect( DEVCON_MODULE_ID index)

  Summary:
     Routes SPI0 pins to In-Package Flash.

  Description:
    This function routes the SPI0 pins to In-Package Flash.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    None.

  Remarks:
    None.
*/

void PLIB_DEVCON_BootIPFSelect( DEVCON_MODULE_ID index);

//******************************************************************************
/* Function :  void PLIB_DEVCON_BootExtSelect( DEVCON_MODULE_ID index)

  Summary:
     Routes SPI0 pins to the PIC32WK pads.

  Description:
    This function routes the SPI0 pins to the PIC32WK pads.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    None.

  Remarks:
    None.
*/

void PLIB_DEVCON_BootExtSelect( DEVCON_MODULE_ID index);

//******************************************************************************
/* Function :  void PLIB_DEVCON_HSUARTEnable( DEVCON_MODULE_ID index)

  Summary:
     Enables High Speed UART.

  Description:
    This function enables the high speed UART and UART 1 will be using dedicated
	UART pin.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    None.

  Remarks:
    None.
*/

void PLIB_DEVCON_HSUARTEnable( DEVCON_MODULE_ID index);

//******************************************************************************
/* Function :  void PLIB_DEVCON_HSUARTDisable( DEVCON_MODULE_ID index)

  Summary:
     Disables High Speed UART.

  Description:
    This function disables the high speed UART and UART 1 can be configured with
	PPS.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    None.

  Remarks:
    None.
*/

void PLIB_DEVCON_HSUARTDisable( DEVCON_MODULE_ID index);

// *****************************************************************************
// *****************************************************************************
// Section: DEVCON Peripheral Library Exists API Routines
// *****************************************************************************
// *****************************************************************************
/* The functions below indicate the existence of the features on the device.
*/

//******************************************************************************
/* Function :  PLIB_DEVCON_ExistsAlternateClock( DEVCON_MODULE_ID index )

  Summary:
    Identifies whether the AlternateClock feature exists on the DEVCON module.

  Description:
    This function identifies whether the AlternateClock feature is available on
	the DEVCON module.
    When this function returns true, these functions are supported on the device:
    - PLIB_DEVCON_AlternateClockEnable
    - PLIB_DEVCON_AlternateClockDisable
	
  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The AlternateClock feature is supported on the device
    - false  - The AlternateClock feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_DEVCON_ExistsAlternateClock( DEVCON_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_DEVCON_ExistsDeviceRegsLockUnlock( DEVCON_MODULE_ID index )

  Summary:
    Identifies whether the DeviceRegsLockUnlock feature exists on the DEVCON module.

  Description:
    This function identifies whether the DeviceRegsLockUnlock feature is available
	on the DEVCON module.
    When this function returns true, these functions are supported on the device:
    - PLIB_DEVCON_DeviceRegistersLock
    - PLIB_DEVCON_DeviceRegistersUnlock
	
  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The DeviceRegsLockUnlock feature is supported on the device
    - false  - The DeviceRegsLockUnlock feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_DEVCON_ExistsDeviceRegsLockUnlock( DEVCON_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_DEVCON_ExistsUSB_PHY_SleepModeSet( DEVCON_MODULE_ID index )

  Summary:
    Identifies whether the USB_PHY_SleepModeSet feature exists on the DEVCON module.

  Description:
    This function identifies whether the USB_PHY_SleepModeSet feature is available
	on the DEVCON module.
    When this function returns true, this function is supported on the device:
    - PLIB_DEVCON_USBPHYSleepModeSet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The USB_PHY_SleepModeSet feature is supported on the device
    - false  - The USB_PHY_SleepModeSet feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_DEVCON_ExistsUSB_PHY_SleepModeSet( DEVCON_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_DEVCON_ExistsECCModes( DEVCON_MODULE_ID index )

  Summary:
    Identifies whether the ECCModes feature exists on the DEVCON module.

  Description:
    This function identifies whether the ECCModes feature is available on the
	DEVCON module.
    When this function returns true, this function is supported on the device:
    - PLIB_DEVCON_FlashErrCorrectionModeSet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ECCModes feature is supported on the device
    - false  - The ECCModes feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_DEVCON_ExistsECCModes( DEVCON_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_DEVCON_ExistsJTAGEnable( DEVCON_MODULE_ID index )

  Summary:
    Identifies whether the JTAGEnable feature exists on the DEVCON module.

  Description:
    This function identifies whether the JTAGEnable feature is available on the
	DEVCON module.
    When this function returns true, these functions are supported on the device:
    - PLIB_DEVCON_JTAGPortEnable
    - PLIB_DEVCON_JTAGPortDisable

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The JTAGEnable feature is supported on the device
    - false  - The JTAGEnable feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_DEVCON_ExistsJTAGEnable( DEVCON_MODULE_ID index );

//******************************************************************************
/* Function :  PLIB_DEVCON_ExistsAnalogChargePumpControl( DEVCON_MODULE_ID index )

  Summary:
    Identifies whether the I/O Analog Charge Pump feature exists on the DEVCON module.

  Description:
    This function identifies whether the I/O Analog Charge Pump feature is available
    on the DEVCON module.
    When this function returns true, these functions are supported on the device:
    - PLIB_DEVCON_AnalogIOChargePumpEnable
    - PLIB_DEVCON_AnalogIOChargePumpDisable

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The I/O Analog Charge Pump feature feature is supported on the device
    - false  - The I/O Analog Charge Pump feature feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_DEVCON_ExistsAnalogChargePumpControl( DEVCON_MODULE_ID index );
//******************************************************************************
/* Function :  PLIB_DEVCON_ExistsTraceOutput( DEVCON_MODULE_ID index )

  Summary:
    Identifies whether the TraceOutput feature exists on the DEVCON module.

  Description:
    This function identifies whether the TraceOutput feature is available on the
	DEVCON module.
    When this function returns true, these functions are supported on the device:
    - PLIB_DEVCON_TraceOutputEnable
    - PLIB_DEVCON_TraceOutputDisable

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The TraceOutput feature is supported on the device
    - false  - The TraceOutput feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_DEVCON_ExistsTraceOutput( DEVCON_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_DEVCON_ExistsJTAGUsesTDO( DEVCON_MODULE_ID index )

  Summary:
    Identifies whether the JTAGUsesTDO feature exists on the DEVCON module.

  Description:
    This function identifies whether the JTAGUsesTDO feature is available on the
	DEVCON module.
    When this function returns true, these functions are supported on the device:
    - PLIB_DEVCON_2WireJTAGEnableTDO
    - PLIB_DEVCON_2WireJTAGDisableTDO

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The JTAGUsesTDO feature is supported on the device
    - false  - The JTAGUsesTDO feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_DEVCON_ExistsJTAGUsesTDO( DEVCON_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_DEVCON_ExistsIgnoreDebugFreeze( DEVCON_MODULE_ID index )

  Summary:
    Identifies whether the IgnoreDebugFreeze feature exists on the DEVCON module.

  Description:
    This function identifies whether the IgnoreDebugFreeze feature is available
	on the DEVCON module.
    When this function returns true, these functions are supported on the device:
    - PLIB_DEVCON_DebugIgnoreFreezeEnable
    - PLIB_DEVCON_IgnoreDebugFreezeDisable

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The IgnoreDebugFreeze feature is supported on the device
    - false  - The IgnoreDebugFreeze feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_DEVCON_ExistsIgnoreDebugFreeze( DEVCON_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_DEVCON_ExistsDeviceVerAndId( DEVCON_MODULE_ID index )

  Summary:
    Identifies whether the DeviceVerAndId feature exists on the DEVCON module.

  Description:
    This function identifies whether the DeviceVerAndId feature is available on
	the DEVCON module.
    When this function returns true, these functions are supported on the device:
    - PLIB_DEVCON_DeviceVersionGet
    - PLIB_DEVCON_DeviceIdGet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The DeviceVerAndId feature is supported on the device
    - false  - The DeviceVerAndId feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_DEVCON_ExistsDeviceVerAndId( DEVCON_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_DEVCON_ExistsSystemLockUnlock( DEVCON_MODULE_ID index )

  Summary:
    Identifies whether the SysLockUnlock feature exists on the DEVCON module.

  Description:
    This function identifies whether the SysLockUnlock feature is available on
	the DEVCON module.
    When this function returns true, these functions are supported on the device:
    - PLIB_DEVCON_SystemUnlock
    - PLIB_DEVCON_SystemLock

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The SysLockUnlock feature is supported on the device
    - false  - The SysLockUnlock feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_DEVCON_ExistsSystemLockUnlock( DEVCON_MODULE_ID index );

//******************************************************************************
/* Function :  PLIB_DEVCON_ExistsMPLL( DEVCON_MODULE_ID index )

  Summary:
    Identifies whether the MPLL feature exists on the DEVCON module. 

  Description:
    This function identifies whether the MPLL feature is available on the DEVCON module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_DEVCON_MPLLIsReady
    - PLIB_DEVCON_MPLLEnable
    - PLIB_DEVCON_MPLLDisable
    - PLIB_DEVCON_MPLLODiv1Set
    - PLIB_DEVCON_MPLLODiv2Set
    - PLIB_DEVCON_MPLLVregIsReady
    - PLIB_DEVCON_MPLLVregEnable
    - PLIB_DEVCON_MPLLVregDisable
    - PLIB_DEVCON_MPLLMultiplierSet
    - PLIB_DEVCON_MPLLInputDivSet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The MPLL feature is supported on the device
    - false  - The MPLL feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_DEVCON_ExistsMPLL( DEVCON_MODULE_ID index );

//******************************************************************************
/* Function :  PLIB_DEVCON_ExistsOTPConfigLockUnlock( DEVCON_MODULE_ID index )

  Summary:
    Identifies whether the OTPConfigLockUnlock feature exists on the DEVCON module 

  Description:
    This function identifies whether the OTPConfigLockUnlock feature is available on the DEVCON module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_DEVCON_OTPConfigLock
    - PLIB_DEVCON_OTPConfigUnlock

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The OTPConfigLockUnlock feature is supported on the device
    - false  - The OTPConfigLockUnlock feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_DEVCON_ExistsOTPConfigLockUnlock( DEVCON_MODULE_ID index );

//******************************************************************************
/* Function :  PLIB_DEVCON_ExistsHSUARTControl( DEVCON_MODULE_ID index )

  Summary:
    Identifies whether the HSUARTControl feature exists on the DEVCON module 

  Description:
    This function identifies whether the HSUARTControl feature is available on the DEVCON module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_DEVCON_HSUARTEnable
    - PLIB_DEVCON_HSUARTDisable

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The HSUARTControl feature is supported on the device
    - false  - The HSUARTControl feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_DEVCON_ExistsHSUARTControl( DEVCON_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_DEVCON_ExistsBootSelection( DEVCON_MODULE_ID index )

  Summary:
    Identifies whether the BootSelection feature exists on the DEVCON module 

  Description:
    This function identifies whether the BootSelection feature is available on the DEVCON module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_DEVCON_BootIPFSelect
    - PLIB_DEVCON_BootExtSelect
	
  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The BootSelection feature is supported on the device
    - false  - The BootSelection feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_DEVCON_ExistsBootSelection( DEVCON_MODULE_ID index );

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END


// ****************************************************************************
// ****************************************************************************
// Section: Included Files (continued)
// ****************************************************************************
// ****************************************************************************

/*  The file included below  maps the interface definitions above to appropriate
    implementations defined in the implementation (imp) file(s).
*/

//#include "peripheral/devcon/processor/devcon_processor.h"

#endif //_PLIB_DEVCON_H

//******************************************************************************
/* End of File
*/

