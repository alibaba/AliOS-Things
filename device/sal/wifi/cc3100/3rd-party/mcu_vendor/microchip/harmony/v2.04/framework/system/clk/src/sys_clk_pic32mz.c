/*******************************************************************************
  SYS CLK Static Functions for Clock System Service

  Company:
    Microchip Technology Inc.

  File Name:
    sys_clk_static.c

  Summary:
    SYS CLK static function implementations for the Clock System Service.

  Description:
    The Clock System Service provides a simple interface to manage the oscillators
    on Microchip microcontrollers. This file defines the static implementation for the 
    Clock System Service.
    
  Remarks:
    Static functions incorporate all system clock configuration settings as
    determined by the user via the Microchip Harmony Configurator GUI.  It provides 
    static version of the routines, eliminating the need for an object ID or 
    object handle.
    
    Static single-open interfaces also eliminate the need for the open handle.
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
// Section: Include Files
// *****************************************************************************
// *****************************************************************************

#include "system_config.h"
#include "system_definitions.h"
#include "peripheral/osc/plib_osc.h"
#include "system/devcon/sys_devcon.h"

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Functions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    void SYS_CLK_Static_Initialize ( const SYS_CLK_INIT const * clkInit )

  Summary:
    Initializes hardware and internal data structure of the System Clock.

  Description:
    This function initializes the hardware and internal data structure of System
    Clock Service.

  Remarks:
    This is configuration values for the static version of the Clock System Service 
    module is determined by the user via the Microchip Harmony Configurator GUI.
    This template will build a sys_clk_static.h and sys_clk_static.c file with 
    the configuration per the user's choice.

    The objective is to eliminate the user's need to be knowledgeable in the function of
    the 'configuration bits' to configure the system oscillators. 
*/

void SYS_CLK_Initialize( const SYS_CLK_INIT const * clkInit )
{
    SYS_DEVCON_SystemUnlock ( );
    
    PLIB_OSC_FRCDivisorSelect( OSC_ID_0, OSC_FRC_DIV_1);

    /* Enable Peripheral Bus 1 */
    PLIB_OSC_PBClockDivisorSet (OSC_ID_0, 0, 2 );
    PLIB_OSC_PBOutputClockEnable (OSC_ID_0, 0 );

    /* Enable Peripheral Bus 2 */
    PLIB_OSC_PBClockDivisorSet (OSC_ID_0, 1, 2 );
    PLIB_OSC_PBOutputClockEnable (OSC_ID_0, 1 );
    /* Enable Peripheral Bus 3 */
    PLIB_OSC_PBClockDivisorSet (OSC_ID_0, 2, 2 );
    PLIB_OSC_PBOutputClockEnable (OSC_ID_0, 2 );
    /* Enable Peripheral Bus 4 */
    PLIB_OSC_PBClockDivisorSet (OSC_ID_0, 3, 2 );
    PLIB_OSC_PBOutputClockEnable (OSC_ID_0, 3 );
    /* Enable Peripheral Bus 5 */
    PLIB_OSC_PBClockDivisorSet (OSC_ID_0, 4, 2 );
    PLIB_OSC_PBOutputClockEnable (OSC_ID_0, 4 );
    /* Enable Peripheral Bus 7 */
    PLIB_OSC_PBClockDivisorSet (OSC_ID_0, 6, 1 );
    PLIB_OSC_PBOutputClockEnable (OSC_ID_0, 6 );
    /* Enable Peripheral Bus 8 */
    PLIB_OSC_PBClockDivisorSet (OSC_ID_0, 7, 1 );
    PLIB_OSC_PBOutputClockEnable (OSC_ID_0, 7 );
  
 

    /* Disable REFCLKO1*/
    PLIB_OSC_ReferenceOscDisable ( OSC_ID_0, OSC_REFERENCE_1 );
    /* Disable REFCLK1_OE*/
    PLIB_OSC_ReferenceOutputDisable ( OSC_ID_0, OSC_REFERENCE_1 );
    /* Disable REFCLKO2*/
    PLIB_OSC_ReferenceOscDisable ( OSC_ID_0, OSC_REFERENCE_2 );
    /* Disable REFCLK2_OE*/
    PLIB_OSC_ReferenceOutputDisable ( OSC_ID_0, OSC_REFERENCE_2 );
    /* Disable REFCLKO3*/
    PLIB_OSC_ReferenceOscDisable ( OSC_ID_0, OSC_REFERENCE_3 );
    /* Disable REFCLK3_OE*/
    PLIB_OSC_ReferenceOutputDisable ( OSC_ID_0, OSC_REFERENCE_3 );
    /* Disable REFCLKO4*/
    PLIB_OSC_ReferenceOscDisable ( OSC_ID_0, OSC_REFERENCE_4 );
    /* Disable REFCLK4_OE*/
    PLIB_OSC_ReferenceOutputDisable ( OSC_ID_0, OSC_REFERENCE_4 );

    SYS_DEVCON_SystemLock ( );
}

//******************************************************************************
/* Function:
    inline uint32_t SYS_CLK_SystemFrequencyGet ( void )

  Summary:
    Gets the system clock frequency in Hertz.

  Description:
    This function gets the System clock frequency in Hertz.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    System clock frequency in Hertz.

  Example:
    <code>
    uint32_t sysClockHz;

    sysClockHz = SYS_CLK_SystemFrequencyGet ( );
    </code>

  Remarks:
 */

inline uint32_t SYS_CLK_SystemFrequencyGet ( void )
{
    return SYS_CLK_FREQ;
}

//******************************************************************************
/* Function:
    inline uint32_t SYS_CLK_PeripheralFrequencyGet ( CLK_BUSES_PERIPHERAL peripheralBus )

  Summary:
    Gets the selected clock peripheral bus frequency in Hertz.

  Description:
    This function gets the selected peripheral bus clock frequency in Hertz.

  Precondition:
    None.

  Parameters:
	peripheralBus - Reference clock bus selection. One of the possible value from
				CLK_BUSES_PERIPHERAL enum. For devices that do not have multiple
				clock channels for Reference clock, CLK_BUS_PERIPHERAL_1 should be
				the selection.

  Returns:
    Clock frequency in Hertz.

  Example:
    <code>
    unsigned long peripheralClockHz;

    peripheralClockHz = SYS_CLK_PeripheralFrequencyGet ( CLK_BUS_PERIPHERAL_5 );
    </code>

  Remarks:
	Most of the devices doesn't have multiple Peripheral clock buses. In that case, 
	pass CLK_USB_PERIPHERAL_1 as the bus number.
 */

inline uint32_t SYS_CLK_PeripheralFrequencyGet ( CLK_BUSES_PERIPHERAL peripheralBus )
{
       uint32_t freq = 0;

    switch (peripheralBus)
    {
        case CLK_BUS_PERIPHERAL_1:
                freq = SYS_CLK_BUS_PERIPHERAL_1;
            break;
        case CLK_BUS_PERIPHERAL_2:
                freq = SYS_CLK_BUS_PERIPHERAL_2;
            break;
        case CLK_BUS_PERIPHERAL_3:
                freq = SYS_CLK_BUS_PERIPHERAL_3;
            break;
        case CLK_BUS_PERIPHERAL_4:
                freq = SYS_CLK_BUS_PERIPHERAL_4;
            break;
        case CLK_BUS_PERIPHERAL_5:
                freq = SYS_CLK_BUS_PERIPHERAL_5;
            break;
        case CLK_BUS_PERIPHERAL_6:
            break;
        case CLK_BUS_PERIPHERAL_7:
                freq = SYS_CLK_BUS_PERIPHERAL_7;
            break;
        case CLK_BUS_PERIPHERAL_8:
                freq = SYS_CLK_BUS_PERIPHERAL_8;
            break;
        default:
            break;
    }
    
    return freq;
}


//******************************************************************************
/* Function:
    inline uint32_t SYS_CLK_ReferenceFrequencyGet ( CLK_BUSES_REFERENCE referenceBus )

  Summary:
    Gets the selected Reference clock bus frequency in Hertz.

  Description:
    This function gets frequency of the selected Reference clock bus in Hertz.

  Precondition:
    None.

  Parameters:
	peripheralBus - Reference clock bus selection. One of the possible value from
				CLK_BUSES_REFERENCE enum. For devices that do not have multiple
				clock channels for Reference clock, CLK_BUS_REFERENCE_1 should be
				the selection.

  Returns:
    Clock frequency in Hz.

  Example:
    <code>
    unsigned long sysClockOutputHz;

    sysClockOutputHz = SYS_CLK_ReferenceFrequencyGet ( CLK_BUS_REFERENCE_3 );
    </code>

  Remarks:
    None.
 */

inline uint32_t SYS_CLK_ReferenceFrequencyGet ( CLK_BUSES_REFERENCE referenceBus )
{
       uint32_t freq = 0;

    switch (referenceBus)
    {
        case CLK_BUS_REFERENCE_1:
            break;
        case CLK_BUS_REFERENCE_2:
            break;
        case CLK_BUS_REFERENCE_3:
            break;
        case CLK_BUS_REFERENCE_4:
            break;
        case CLK_BUS_REFERENCE_5:
            break;
        default:
            break;
    }
    
    return freq;
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
