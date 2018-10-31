 /*******************************************************************************
  Clock System Service Implementation

  Company:
    Microchip Technology Inc.

  File Name:
    sys_clk.c

  Summary:
    Clock System Service implementation.

  Description:
    The Clock System Service provides a simple interface to manage the clock
    on Microchip microcontrollers. This file implements the core interface routines 
    for the Clock System Service. While building the system service from source, 
    ALWAYS include this file in the build.
*******************************************************************************/

//DOM-IGNORE-BEGIN
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
//DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Include Files
// *****************************************************************************
// *****************************************************************************

#include "system/clk/src/sys_clk_local.h"


SYS_CLK_OBJECT clkObject;


// *****************************************************************************
// *****************************************************************************
// Section: File Scope Functions
// *****************************************************************************
// *****************************************************************************

/******************************************************************************
  Function:
    void SYS_CLK_Initialize ( const SYS_CLK_INIT *clkInit )

  Summary:
    Initializes hardware and internal data structure for the general features
    of the clock.

  Description:
    This function initializes hardware and internal data structure for the general
    features of the clock. Preferably, this API should be called erly in the 
    initialization (before other modules are initialized so that the clock gets 
    the necessary settling time before normal execution of the code starts).

  Remarks:
    None.
*/

void SYS_CLK_Initialize ( const SYS_CLK_INIT *clkInit )
{
    CLK_SOURCES_SYSTEM systemSource;
    uint32_t clockClosest = 0;

    /* If the user has not passed anything that means he want to retain the
     * configuration bit settings  */
    if ( clkInit != NULL )
    {
        systemSource = _SYS_CLK_ClockTypeMap(PLIB_OSC_CurrentSysClockGet ( OSC_PLIB_ID ));
        
        if (!_SYS_CLK_SystemClockSet ( systemSource, clkInit->systemClockFrequencyHz,
                clkInit->waitTillComplete, &clockClosest ))
        {
            SYS_ASSERT(false, "Critical failure when setting system clock");
            return;
        }
        
        #if defined(PLIB_OSC_ExistsOnWaitAction)
        if ( PLIB_OSC_ExistsOnWaitAction ( OSC_PLIB_ID ) )
        {
            /* Sets the oscillator's response to a 'Wait' instruction */
            PLIB_OSC_OnWaitActionSet ( OSC_PLIB_ID, clkInit->onWaitInstruction );
        }
        #endif
    }
    else
    {
        clkObject.systemClock = _SYS_CLK_SystemClockRead ();
    }

    clkObject.callback=NULL;
}

/******************************************************************************
  Function:
    void SYS_CLK_ClockFailureCallbackRegister ( SYS_CLK_ERROR_HANDLER callback )

  Summary:
    Registers the call back function that will be triggered on a clock failure.

  Description:
    This function registers the call back function that will be triggered on a 
    clock failure.

  Remarks:
    None.
*/

void SYS_CLK_ClockFailureCallbackRegister ( SYS_CLK_ERROR_HANDLER callback )
{
    clkObject.callback = callback;
}

/******************************************************************************
  Function:
    uint32_t SYS_CLK_SystemFrequencySet ( CLK_SOURCES_SYSTEM systemSource,
				uint32_t systemClockHz, bool waitUntilComplete )

  Summary:
    Sets the system clock of the device to the value specified.
    
  Description:
    This function configures the clock multipliers and divisors to achieve requested
    System clock frequency. Initially it checks the difference between the requested
    value and possible value. If it is not within 'SYS_CLK_CONFIG_FREQ_ERROR_LIMIT',
    the registers values will not be changed and a value '0' will be returned to let
    user know that the operation was unsuccessful. If the value requested is acceptable,
    then it writes to the oscillator registers and return with the newly set frequency.
    If the operation is a failure, SYS_CLK_SystemClosestFrequencyGet function will give
    the closest possible frequency. If the closest possible value is acceptable, user
    can reconfigure with that value.

  Remarks:
    None.
*/

uint32_t SYS_CLK_SystemFrequencySet ( CLK_SOURCES_SYSTEM systemSource,
				uint32_t systemClockHz, bool waitUntilComplete )
{
    uint32_t clockClosest = 0;

    if ( _SYS_CLK_SystemClockSet ( systemSource, systemClockHz,
                waitUntilComplete, &clockClosest ) )
    {
        return clkObject.systemClock;
    }
    else 
    {
        /* Update the closest in any case */
        clkObject.systemClosestClock = clockClosest;

        return 0;
    }
}


/******************************************************************************
  Function:
    uint32_t SYS_CLK_SystemFrequencyGet ( void )

  Summary:
    Gets the system clock frequency in Hertz.

  Description:
    This function gets the System clock frequency in Hertz.

  Remarks:
    None.
*/

uint32_t SYS_CLK_SystemFrequencyGet ( void )
{
    return clkObject.systemClock;
}


/******************************************************************************
  Function:
    uint32_t SYS_CLK_PeripheralFrequencySet ( CLK_BUSES_PERIPHERAL peripheralBus,
                CLK_SOURCES_PERIPHERAL peripheralSource, uint32_t peripheralClockHz,
                bool waitUntilComplete )

  Summary:
    Configures the Peripheral clock of the device to the value specified.

  Description:
    This function configures the clock multipliers and divisors to achieve requested
    Peripheral clock frequency. Initially it checks the difference between the requested
    value and possible value. If it is not within 'SYS_CLK_CONFIG_FREQ_ERROR_LIMIT',
    the registers values will not be changed and a value '0' will be returned to 
    let user know that the operation was unsuccessful. If the value requested is
    acceptable, then it writes to the oscillator registers and return with the newly
    set frequency. If the operation is a failure, SYS_CLK_PeripheralClosestFrequencyGet
    function will give the closest possible frequency. If the closest possible value
    is acceptable, user can reconfigure with that value.

  Remarks:
    None.
*/

uint32_t SYS_CLK_PeripheralFrequencySet ( CLK_BUSES_PERIPHERAL peripheralBus,
            CLK_SOURCES_PERIPHERAL peripheralSource, uint32_t peripheralClockHz,
            bool waitUntilComplete )
{
    uint32_t clockClosest = 0;

    if ( _SYS_CLK_PeripheralFrequencySet ( peripheralBus, peripheralClockHz, &clockClosest ) )
    {
        /* System clock update is successful. Update the data structures */
        clkObject.peripheralClock[peripheralBus] = clockClosest;

        /* Update the closest in any case */
        clkObject.peripheralClosestClock[peripheralBus] = clockClosest;

    }
    else
    {
        /* Update the closest in any case */
        clkObject.peripheralClosestClock[peripheralBus] = clockClosest;

        return 0;
    }
    return clockClosest;
}


/******************************************************************************
  Function:
    uint32_t SYS_CLK_PeripheralFrequencyGet ( CLK_BUSES_PERIPHERAL peripheralBus )

  Summary:
    Gets the Peripheral clock frequency in Hertz.

  Description:
    This function gets the Peripheral clock frequency in Hertz.

  Remarks:
    None.
*/

uint32_t SYS_CLK_PeripheralFrequencyGet ( CLK_BUSES_PERIPHERAL peripheralBus )
{
    return _SYS_CLK_PeripheralClockRead (peripheralBus, clkObject.systemClock );
}


/******************************************************************************
  Function:
    uint32_t SYS_CLK_ReferenceFrequencySet ( CLK_BUSES_USB usbBus, CLK_SOURCES_USB usbSource,
					uint32_t usbClockHz, bool waitUntilComplete )

  Summary:
    Configures the Reference clock of the device to the value specified.

  Description:
    This function configures the clock multipliers and divisors to achieve requested
    Reference clock frequency. Initially it checks the difference between the requested
    value and possible value. If it is not within 'SYS_CLK_CONFIG_FREQ_ERROR_LIMIT',
    the registers values will not be changed and a value '0' will be returned to let
    user know that the operation was unsuccessful. If the value requested is acceptable,
    then it writes to the oscillator registers and return with the newly set frequency.
    If the operation is a failure, SYS_CLK_USBClosestFrequencyGet function will give
    the closest possible frequency. If the closest possible value is acceptable, user
    can reconfigure with that value.

  Remarks:
    None.
*/

uint32_t SYS_CLK_ReferenceFrequencySet ( CLK_BUSES_REFERENCE referenceBus,
        CLK_SOURCES_REFERENCE referenceSource, uint32_t referenceClockHz,
        bool waitUntilComplete )
{
    uint32_t clockClosest = 0;

    if ( _SYS_CLK_ReferenceFrequencySet ( referenceBus, referenceSource, referenceClockHz,
                waitUntilComplete, &clockClosest ) )
    {
        /* System clock update is successful. Update the data structures */
        clkObject.referenceClock[referenceBus] = clockClosest;

        /* Update the closest with the calculated value */
        clkObject.referenceClosestClock[referenceBus] = clockClosest;

    }
    else
    {
        /* Update the closest in any case */
        clkObject.referenceClosestClock[referenceBus] = clockClosest;

        return 0;
    }
    return clockClosest;
}


/******************************************************************************
  Function:
    uint32_t SYS_CLK_ReferenceFrequencyGet ( CLK_BUSES_REFERENCE referenceBus )

  Summary:
    Gets the Reference clock frequency in Hertz.

  Description:
    This function gets the Reference clock frequency in Hertz.

  Remarks:
    None.
*/

uint32_t SYS_CLK_ReferenceFrequencyGet ( CLK_BUSES_REFERENCE referenceBus )
{
    return clkObject.referenceClock[referenceBus];
}


/******************************************************************************
  Function:
    void SYS_CLK_TaskError ( void )

  Summary:
    Informs the user on a clock failure by invoking the registered call back
    function.

  Description:
    This function informs the user on a clock failure by invoking the registered
    call back function. This must be called from the Fail Safe Clock Monitor (FSCM)
    interrupt service routine.
    
  Remarks:
    None.
*/

void SYS_CLK_TaskError ( void )
{
    if (clkObject.callback != NULL)
    {
        clkObject.callback ( clkObject.systemClockSource, clkObject.systemClock );
    }
    
}


/******************************************************************************
  Function:
    void SYS_CLK_ReferenceClockSetup ( CLK_BUSES_REFERENCE referenceBus,
                                        SYS_CLK_REFERENCE_SETUP *refSetup )

  Summary:
    Initializes the Reference clock module. 

  Description:
    This function initializes the Reference clock module.
 *
  Remarks:
    None.
*/

void SYS_CLK_ReferenceClockSetup ( CLK_BUSES_REFERENCE referenceBus,
                                        SYS_CLK_REFERENCE_SETUP *refSetup )
{
    _SYS_CLK_ReferenceClockSetup(referenceBus, refSetup);
}


/******************************************************************************
  Function:
    void SYS_CLK_FRCTune ( SYS_CLK_FRC_TUNING_TYPE tuningData )

  Summary:
    Triggers the direct value-based FRC oscillator tuning.

  Description:
    This function tunes the FRC as per the given value. FRC
    tuning functionality has been provided to help customers compensate for
    temperature effects on the FRC frequency over a wide range of temperatures.

  Remarks:
    None.
*/

void SYS_CLK_FRCTune ( SYS_CLK_FRC_TUNING_TYPE tuningData )
{
    PLIB_OSC_FRCTuningSelect(OSC_ID_0, tuningData);
}

/******************************************************************************
  Function:
    void SYS_CLK_SecondaryOscillatorEnable ( void )

  Summary:
    Enables the secondary oscillator.

  Description:
    This function enables the secondary oscillator.

  Remarks:
    For more details refer sys_clk.h.
*/

void SYS_CLK_SecondaryOscillatorEnable ( void )
{
    /* Check for secondary oscillator status */
    if (!PLIB_OSC_SecondaryIsEnabled(OSC_ID_0))
    {    
        /* Unlock and enable secondary oscillator */
        SYS_DEVCON_SystemUnlock();
        
        PLIB_OSC_SecondaryEnable(OSC_ID_0);
        
        SYS_DEVCON_SystemLock();
    }
}

/******************************************************************************
  Function:
    void SYS_CLK_SecondaryOscillatorDisable ( void )

  Summary:
    Disables the secondary oscillator.

  Description:
    This function disables the secondary oscillator.

  Remarks:
    For more details refer sys_clk.h.
*/

void SYS_CLK_SecondaryOscillatorDisable ( void )
{
    /* Check for secondary oscillator status */
    if (PLIB_OSC_SecondaryIsEnabled(OSC_ID_0))
    {    
        /* Unlock and disable secondary oscillator*/
        SYS_DEVCON_SystemUnlock();
        
        PLIB_OSC_SecondaryDisable(OSC_ID_0);
        
        SYS_DEVCON_SystemLock();
    }
}

/******************************************************************************
  Function:
    bool SYS_CLK_SecondaryOscillatorIsEnabled ( void )

  Summary:
    Identifies whether secondary oscillator is enabled or disabled.

  Description:
    This function identifies whether the secondary oscillator is enabled or 
    disabled.
    
  Remarks:
    For more details refer sys_clk.h.
*/

bool SYS_CLK_SecondaryOscillatorIsEnabled ( void )
{
    return (PLIB_OSC_SecondaryIsEnabled(OSC_ID_0));
}

/*******************************************************************************
End of File
*/

