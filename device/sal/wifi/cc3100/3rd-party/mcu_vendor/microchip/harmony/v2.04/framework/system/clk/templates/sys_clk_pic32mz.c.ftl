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
<#macro SYS_CLK_PERIPHERAL_CLK_ENABLE_INSTANCE SYS_CLK_PBCLKX SYS_CLK_PBDIV SYS_CLK_PBCLKNUM>
    /* Enable Peripheral Bus ${SYS_CLK_PBCLKNUM} */
    PLIB_OSC_PBClockDivisorSet (OSC_ID_0, ${SYS_CLK_PBCLKX}, ${SYS_CLK_PBDIV} );
<#if (CONFIG_PIC32MZ == true || CONFIG_PIC32MK == true || CONFIG_PIC32WK == true || CONFIG_DS60001404 = true)>   
    PLIB_OSC_PBOutputClockEnable (OSC_ID_0, ${SYS_CLK_PBCLKX} );
</#if>
</#macro>
<#macro SYS_CLK_PERIPHERAL_CLK_DISABLE_INSTANCE SYS_CLK_PBCLKX SYS_CLK_PBCLKNUM>
<#if (CONFIG_PIC32MZ == true || CONFIG_PIC32MK == true || CONFIG_PIC32WK == true || CONFIG_DS60001404 = true)>       /* Disable Peripheral Bus ${SYS_CLK_PBCLKNUM} */
    PLIB_OSC_PBOutputClockDisable (OSC_ID_0, ${SYS_CLK_PBCLKX} );
</#if>

</#macro>
<#macro SYS_CLK_REFCLK_ENABLE_INSTANCE SYS_CLK_REFCLKX SYS_CLK_REFCLK_SOURCE SYS_CLK_REFCLKNUM 
SYS_CLK_REFCLK_ROSEL SYS_CLK_REFCLK_RODIV SYS_CLK_REFCLK_ROTRIM>
    /* Enable and configure REFCLKO${SYS_CLK_REFCLKNUM}*/
    
    /* ROSEL ${SYS_CLK_REFCLK_SOURCE} */
    PLIB_OSC_ReferenceOscBaseClockSelect ( OSC_ID_0, ${SYS_CLK_REFCLKX}, ${SYS_CLK_REFCLK_ROSEL} );
    /* RODIV */
    PLIB_OSC_ReferenceOscDivisorValueSet ( OSC_ID_0, ${SYS_CLK_REFCLKX}, ${SYS_CLK_REFCLK_RODIV} );
    /* ROTRIM */
    PLIB_OSC_ReferenceOscTrimSet ( OSC_ID_0, ${SYS_CLK_REFCLKX}, ${SYS_CLK_REFCLK_ROTRIM} );

    PLIB_OSC_ReferenceOscEnable ( OSC_ID_0, ${SYS_CLK_REFCLKX} );
</#macro>
<#macro SYS_CLK_REFCLK_DISABLE_INSTANCE SYS_CLK_REFCLKX SYS_CLK_REFCLKNUM>
    /* Disable REFCLKO${SYS_CLK_REFCLKNUM}*/
    PLIB_OSC_ReferenceOscDisable ( OSC_ID_0, ${SYS_CLK_REFCLKX} );
</#macro>
<#macro SYS_CLK_REFCLK_OUTPUT_ENABLE_INSTANCE SYS_CLK_REFCLKX SYS_CLK_REFCLKNUM>
    /* Disable REFCLK${SYS_CLK_REFCLKNUM}_OE*/
    PLIB_OSC_ReferenceOutputEnable ( OSC_ID_0, ${SYS_CLK_REFCLKX} );
</#macro>
<#macro SYS_CLK_REFCLK_OUTPUT_DISABLE_INSTANCE SYS_CLK_REFCLKX SYS_CLK_REFCLKNUM>
    /* Disable REFCLK${SYS_CLK_REFCLKNUM}_OE*/
    PLIB_OSC_ReferenceOutputDisable ( OSC_ID_0, ${SYS_CLK_REFCLKX} );
</#macro>

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
    
    PLIB_OSC_FRCDivisorSelect( OSC_ID_0, ${CONFIG_SYS_CLK_FRCDIV});

<#if CONFIG_SYS_CLK_CONFIG_SOSCEN == "ON">
    /* Enable Secondary Oscillator */
    PLIB_OSC_SecondaryEnable( OSC_ID_0 );

<#elseif CONFIG_SYS_CLK_CONFIG_SOSCEN == "OFF">
    /* Disable Secondary Oscillator */
    PLIB_OSC_SecondaryDisable( OSC_ID_0 );

</#if>
<#if CONFIG_HAVE_MPLL == true>
    /* Memory PLL */
<#if CONFIG_SYS_CLK_MPLL_ENABLE == true>
	PLIB_DEVCON_MPLLVregEnable( DEVCON_ID_0 );
	while (!PLIB_DEVCON_MPLLVregIsReady(DEVCON_ID_0));

	PLIB_DEVCON_MPLLVrefSet(DEVCON_ID_0, DEVCON_MPLL_VREF_EXT);
    PLIB_DEVCON_MPLLInputDivSet( DEVCON_ID_0, ${CONFIG_SYS_CLK_MPLLIDIV} );
    PLIB_DEVCON_MPLLMultiplierSet ( DEVCON_ID_0, ${CONFIG_SYS_CLK_MPLLMULT} );
    PLIB_DEVCON_MPLLODiv1Set( DEVCON_ID_0, DEVCON_MPLL_ODIV_${CONFIG_SYS_CLK_MPLLODIV1} );
    PLIB_DEVCON_MPLLODiv2Set( DEVCON_ID_0, DEVCON_MPLL_ODIV_${CONFIG_SYS_CLK_MPLLODIV2} );
    PLIB_DEVCON_MPLLEnable( DEVCON_ID_0 );
	while(!PLIB_DEVCON_MPLLIsReady( DEVCON_ID_0 ));
<#else>
    PLIB_DEVCON_MPLLDisable( DEVCON_ID_0 );
</#if></#if>
<#if (CONFIG_DS60001402 == true || CONFIG_DS60001404 == true) && (CONFIG_UPLLEN == "ON")>	
	/* Configure UPLL */
<#if CONFIG_DS60001402 == true>
	PLIB_OSC_UPLLInputDivisorSet(OSC_ID_0, ${CONFIG_SYS_CLK_UPLLIDIV});
	PLIB_OSC_UPLLFrequencyRangeSet(OSC_ID_0, ${CONFIG_SYS_CLK_UPLLRNG});	
</#if>
	PLIB_OSC_UPLLMultiplierSelect(OSC_ID_0, ${CONFIG_SYS_CLK_UPLLMULT});
	PLIB_OSC_UPLLOutputDivisorSet(OSC_ID_0, ${CONFIG_SYS_CLK_UPLLODIV});

<#if CONFIG_SYS_CLK_UFRCEN == "FRC">	
	PLIB_OSC_UsbClockSourceSelect (OSC_ID_0, SYS_OSC_USBCLK_FRC);
<#else>
	PLIB_OSC_UsbClockSourceSelect (OSC_ID_0, SYS_OSC_USBCLK_PRIMARY);
</#if>
</#if>
<#if CONFIG_SYS_CLK_PBCLK0_ENABLE == true>
<@SYS_CLK_PERIPHERAL_CLK_ENABLE_INSTANCE SYS_CLK_PBCLKX="0" SYS_CLK_PBCLKNUM="1" SYS_CLK_PBDIV=CONFIG_SYS_CLK_PBDIV0 />
</#if>
<#if CONFIG_DS60001404 == false>

<#if CONFIG_SYS_CLK_PBCLK1_ENABLE == true>
<@SYS_CLK_PERIPHERAL_CLK_ENABLE_INSTANCE SYS_CLK_PBCLKX="1" SYS_CLK_PBCLKNUM="2" SYS_CLK_PBDIV=CONFIG_SYS_CLK_PBDIV1 />
<#else>
<@SYS_CLK_PERIPHERAL_CLK_DISABLE_INSTANCE SYS_CLK_PBCLKX="1" SYS_CLK_PBCLKNUM="2" />
</#if>
<#if CONFIG_SYS_CLK_PBCLK2_ENABLE == true>
<@SYS_CLK_PERIPHERAL_CLK_ENABLE_INSTANCE SYS_CLK_PBCLKX="2" SYS_CLK_PBCLKNUM="3" SYS_CLK_PBDIV=CONFIG_SYS_CLK_PBDIV2 />
<#else>
<@SYS_CLK_PERIPHERAL_CLK_DISABLE_INSTANCE SYS_CLK_PBCLKX="2" SYS_CLK_PBCLKNUM="3" />
</#if>
<#if CONFIG_SYS_CLK_PBCLK3_ENABLE == true>
<@SYS_CLK_PERIPHERAL_CLK_ENABLE_INSTANCE SYS_CLK_PBCLKX="3" SYS_CLK_PBCLKNUM="4" SYS_CLK_PBDIV=CONFIG_SYS_CLK_PBDIV3 />
<#else>
<@SYS_CLK_PERIPHERAL_CLK_DISABLE_INSTANCE SYS_CLK_PBCLKX="3" SYS_CLK_PBCLKNUM="4" />
</#if>
<#if CONFIG_PIC32WK == false>
<#if CONFIG_SYS_CLK_PBCLK4_ENABLE == true>
<@SYS_CLK_PERIPHERAL_CLK_ENABLE_INSTANCE SYS_CLK_PBCLKX="4" SYS_CLK_PBCLKNUM="5" SYS_CLK_PBDIV=CONFIG_SYS_CLK_PBDIV4 />
<#else>
<@SYS_CLK_PERIPHERAL_CLK_DISABLE_INSTANCE SYS_CLK_PBCLKX="4" SYS_CLK_PBCLKNUM="5" />
</#if>
<#if CONFIG_DS60001361 == true || CONFIG_DS60001402 == true>
<#if CONFIG_SYS_CLK_PBCLK5_ENABLE == true>
<@SYS_CLK_PERIPHERAL_CLK_ENABLE_INSTANCE SYS_CLK_PBCLKX="5" SYS_CLK_PBCLKNUM="6" SYS_CLK_PBDIV=CONFIG_SYS_CLK_PBDIV5 />
<#else>
<@SYS_CLK_PERIPHERAL_CLK_DISABLE_INSTANCE SYS_CLK_PBCLKX="5" SYS_CLK_PBCLKNUM="6" />
</#if></#if>
<#if CONFIG_SYS_CLK_PBCLK6_ENABLE == true>
<@SYS_CLK_PERIPHERAL_CLK_ENABLE_INSTANCE SYS_CLK_PBCLKX="6" SYS_CLK_PBCLKNUM="7" SYS_CLK_PBDIV=CONFIG_SYS_CLK_PBDIV6 />
<#else>
<@SYS_CLK_PERIPHERAL_CLK_DISABLE_INSTANCE SYS_CLK_PBCLKX="6"SYS_CLK_PBCLKNUM="7" />
</#if>
<#if CONFIG_DS60001361 == false && CONFIG_DS60001402 == false>
<#if CONFIG_SYS_CLK_PBCLK7_ENABLE == true>
<@SYS_CLK_PERIPHERAL_CLK_ENABLE_INSTANCE SYS_CLK_PBCLKX="7" SYS_CLK_PBCLKNUM="8" SYS_CLK_PBDIV=CONFIG_SYS_CLK_PBDIV7 />
<#else>
<@SYS_CLK_PERIPHERAL_CLK_DISABLE_INSTANCE SYS_CLK_PBCLKX="7" SYS_CLK_PBCLKNUM="8" />
</#if></#if></#if> </#if> 
 
<#if CONFIG_HAVE_REFCLOCK == true>    
<#if CONFIG_PIC32MX == true>    
<#if CONFIG_SYS_CLK_REFCLK_ENABLE == true>
<@SYS_CLK_REFCLK_ENABLE_INSTANCE SYS_CLK_REFCLKX="OSC_REFERENCE_1" SYS_CLK_REFCLKNUM="1" 
SYS_CLK_REFCLK_RODIV=CONFIG_SYS_CLK_RODIV
SYS_CLK_REFCLK_SOURCE=CONFIG_SYS_CLK_REFCLK_SOURCE SYS_CLK_REFCLK_ROTRIM=CONFIG_SYS_CLK_ROTRIM
SYS_CLK_REFCLK_ROSEL=CONFIG_SYS_CLK_REFCLK_ROSEL/>
<#else>
<@SYS_CLK_REFCLK_DISABLE_INSTANCE SYS_CLK_REFCLKX="OSC_REFERENCE_1" SYS_CLK_REFCLKNUM="1" />
</#if>
<#if CONFIG_SYS_CLK_REFCLK_OE == true>
<@SYS_CLK_REFCLK_OUTPUT_ENABLE_INSTANCE SYS_CLK_REFCLKX="OSC_REFERENCE_1" SYS_CLK_REFCLKNUM="1" />
<#else>
<@SYS_CLK_REFCLK_OUTPUT_DISABLE_INSTANCE SYS_CLK_REFCLKX="OSC_REFERENCE_1" SYS_CLK_REFCLKNUM="1" />
</#if></#if>

<#if (CONFIG_PIC32MZ == true || CONFIG_PIC32MK == true || CONFIG_PIC32WK == true)>   
<#if CONFIG_SYS_CLK_REFCLK0_ENABLE == true>
<@SYS_CLK_REFCLK_ENABLE_INSTANCE SYS_CLK_REFCLKX="OSC_REFERENCE_1" SYS_CLK_REFCLKNUM="1" 
SYS_CLK_REFCLK_RODIV=CONFIG_SYS_CLK_RODIV0
SYS_CLK_REFCLK_SOURCE=CONFIG_SYS_CLK_REFCLK_SOURCE0 SYS_CLK_REFCLK_ROTRIM=CONFIG_SYS_CLK_ROTRIM0
SYS_CLK_REFCLK_ROSEL=CONFIG_SYS_CLK_REFCLK_ROSEL0/>
<#else>
<@SYS_CLK_REFCLK_DISABLE_INSTANCE SYS_CLK_REFCLKX="OSC_REFERENCE_1" SYS_CLK_REFCLKNUM="1" />
</#if>
<#if CONFIG_SYS_CLK_REFCLK0_OE == true>
<@SYS_CLK_REFCLK_OUTPUT_ENABLE_INSTANCE SYS_CLK_REFCLKX="OSC_REFERENCE_1" SYS_CLK_REFCLKNUM="1" />
<#else>
<@SYS_CLK_REFCLK_OUTPUT_DISABLE_INSTANCE SYS_CLK_REFCLKX="OSC_REFERENCE_1" SYS_CLK_REFCLKNUM="1" />
</#if>
<#if CONFIG_SYS_CLK_REFCLK1_ENABLE == true>
<@SYS_CLK_REFCLK_ENABLE_INSTANCE SYS_CLK_REFCLKX="OSC_REFERENCE_2" SYS_CLK_REFCLKNUM="2" 
SYS_CLK_REFCLK_RODIV=CONFIG_SYS_CLK_RODIV1
SYS_CLK_REFCLK_SOURCE=CONFIG_SYS_CLK_REFCLK_SOURCE1 SYS_CLK_REFCLK_ROTRIM=CONFIG_SYS_CLK_ROTRIM1
SYS_CLK_REFCLK_ROSEL=CONFIG_SYS_CLK_REFCLK_ROSEL1/>
<#else>
<@SYS_CLK_REFCLK_DISABLE_INSTANCE SYS_CLK_REFCLKX="OSC_REFERENCE_2" SYS_CLK_REFCLKNUM="2" />
</#if>
<#if CONFIG_SYS_CLK_REFCLK1_OE == true>
<@SYS_CLK_REFCLK_OUTPUT_ENABLE_INSTANCE SYS_CLK_REFCLKX="OSC_REFERENCE_2" SYS_CLK_REFCLKNUM="2" />
<#else>
<@SYS_CLK_REFCLK_OUTPUT_DISABLE_INSTANCE SYS_CLK_REFCLKX="OSC_REFERENCE_2" SYS_CLK_REFCLKNUM="2" />
</#if>
<#if CONFIG_SYS_CLK_REFCLK2_ENABLE == true>
<@SYS_CLK_REFCLK_ENABLE_INSTANCE SYS_CLK_REFCLKX="OSC_REFERENCE_3" SYS_CLK_REFCLKNUM="3" 
SYS_CLK_REFCLK_RODIV=CONFIG_SYS_CLK_RODIV2
SYS_CLK_REFCLK_SOURCE=CONFIG_SYS_CLK_REFCLK_SOURCE2 SYS_CLK_REFCLK_ROTRIM=CONFIG_SYS_CLK_ROTRIM2
SYS_CLK_REFCLK_ROSEL=CONFIG_SYS_CLK_REFCLK_ROSEL2/>
<#else>
<@SYS_CLK_REFCLK_DISABLE_INSTANCE SYS_CLK_REFCLKX="OSC_REFERENCE_3" SYS_CLK_REFCLKNUM="3" />
</#if>
<#if CONFIG_SYS_CLK_REFCLK2_OE == true>
<@SYS_CLK_REFCLK_OUTPUT_ENABLE_INSTANCE SYS_CLK_REFCLKX="OSC_REFERENCE_3" SYS_CLK_REFCLKNUM="3" />
<#else>
<@SYS_CLK_REFCLK_OUTPUT_DISABLE_INSTANCE SYS_CLK_REFCLKX="OSC_REFERENCE_3" SYS_CLK_REFCLKNUM="3" />
</#if>
<#if CONFIG_SYS_CLK_REFCLK3_ENABLE == true>
<@SYS_CLK_REFCLK_ENABLE_INSTANCE SYS_CLK_REFCLKX="OSC_REFERENCE_4" SYS_CLK_REFCLKNUM="4" 
SYS_CLK_REFCLK_RODIV=CONFIG_SYS_CLK_RODIV3
SYS_CLK_REFCLK_SOURCE=CONFIG_SYS_CLK_REFCLK_SOURCE3 SYS_CLK_REFCLK_ROTRIM=CONFIG_SYS_CLK_ROTRIM3
SYS_CLK_REFCLK_ROSEL=CONFIG_SYS_CLK_REFCLK_ROSEL3/>
<#else>
<@SYS_CLK_REFCLK_DISABLE_INSTANCE SYS_CLK_REFCLKX="OSC_REFERENCE_4" SYS_CLK_REFCLKNUM="4" />
</#if>
<#if CONFIG_SYS_CLK_REFCLK3_OE == true>
<@SYS_CLK_REFCLK_OUTPUT_ENABLE_INSTANCE SYS_CLK_REFCLKX="OSC_REFERENCE_4" SYS_CLK_REFCLKNUM="4" />
<#else>
<@SYS_CLK_REFCLK_OUTPUT_DISABLE_INSTANCE SYS_CLK_REFCLKX="OSC_REFERENCE_4" SYS_CLK_REFCLKNUM="4" />
</#if>
<#if CONFIG_HAVE_REFCLOCK5 == true>
<#if CONFIG_SYS_CLK_REFCLK4_ENABLE == true>
<@SYS_CLK_REFCLK_ENABLE_INSTANCE SYS_CLK_REFCLKX="OSC_REFERENCE_5" SYS_CLK_REFCLKNUM="5" 
SYS_CLK_REFCLK_RODIV=CONFIG_SYS_CLK_RODIV4
SYS_CLK_REFCLK_SOURCE=CONFIG_SYS_CLK_REFCLK_SOURCE4 SYS_CLK_REFCLK_ROTRIM=CONFIG_SYS_CLK_ROTRIM4
SYS_CLK_REFCLK_ROSEL=CONFIG_SYS_CLK_REFCLK_ROSEL4/>
<#else>
<@SYS_CLK_REFCLK_DISABLE_INSTANCE SYS_CLK_REFCLKX="OSC_REFERENCE_5" SYS_CLK_REFCLKNUM="5" />
</#if>
<#if CONFIG_SYS_CLK_REFCLK4_OE == true>
<@SYS_CLK_REFCLK_OUTPUT_ENABLE_INSTANCE SYS_CLK_REFCLKX="OSC_REFERENCE_5" SYS_CLK_REFCLKNUM="5" />
<#else>
<@SYS_CLK_REFCLK_OUTPUT_DISABLE_INSTANCE SYS_CLK_REFCLKX="OSC_REFERENCE_5" SYS_CLK_REFCLKNUM="5" />
</#if>
</#if></#if></#if>

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
<#if (CONFIG_PIC32MZ == true || CONFIG_PIC32MK == true || CONFIG_PIC32WK == true)>       uint32_t freq = 0;

    switch (peripheralBus)
    {
        case CLK_BUS_PERIPHERAL_1:
                freq = SYS_CLK_BUS_PERIPHERAL_1;
            break;
        case CLK_BUS_PERIPHERAL_2:
<#if CONFIG_SYS_CLK_PBCLK1_ENABLE == true>
                freq = SYS_CLK_BUS_PERIPHERAL_2;
</#if>
            break;
        case CLK_BUS_PERIPHERAL_3:
<#if CONFIG_SYS_CLK_PBCLK2_ENABLE == true>
                freq = SYS_CLK_BUS_PERIPHERAL_3;
</#if>
            break;
        case CLK_BUS_PERIPHERAL_4:
<#if CONFIG_SYS_CLK_PBCLK3_ENABLE == true>
                freq = SYS_CLK_BUS_PERIPHERAL_4;
</#if>
            break;
        case CLK_BUS_PERIPHERAL_5:
<#if CONFIG_SYS_CLK_PBCLK4_ENABLE == true>
                freq = SYS_CLK_BUS_PERIPHERAL_5;
</#if>
            break;
        case CLK_BUS_PERIPHERAL_6:
<#if CONFIG_DS60001361 == true || CONFIG_DS60001402 == true>
<#if CONFIG_SYS_CLK_PBCLK5_ENABLE == true>
                freq = SYS_CLK_BUS_PERIPHERAL_6;
</#if>
</#if>
            break;
        case CLK_BUS_PERIPHERAL_7:
<#if CONFIG_SYS_CLK_PBCLK6_ENABLE == true>
                freq = SYS_CLK_BUS_PERIPHERAL_7;
</#if>
            break;
<#if CONFIG_DS60001361 == false && CONFIG_DS60001402 == false>
        case CLK_BUS_PERIPHERAL_8:
<#if CONFIG_SYS_CLK_PBCLK7_ENABLE == true>
                freq = SYS_CLK_BUS_PERIPHERAL_8;
</#if>
            break;
</#if>
        default:
            break;
    }
    
    return freq;
</#if>
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
<#if (CONFIG_PIC32MZ == true || CONFIG_PIC32MK == true || CONFIG_PIC32WK == true || CONFIG_DS60001404 = true)>       uint32_t freq = 0;

    switch (referenceBus)
    {
        case CLK_BUS_REFERENCE_1:
<#if CONFIG_SYS_CLK_REFCLK0_ENABLE == true>
                freq = SYS_CLK_BUS_REFERENCE_1;
</#if>
            break;
        case CLK_BUS_REFERENCE_2:
<#if CONFIG_SYS_CLK_REFCLK1_ENABLE == true>
                freq = SYS_CLK_BUS_REFERENCE_2;
</#if>
            break;
        case CLK_BUS_REFERENCE_3:
<#if CONFIG_SYS_CLK_REFCLK2_ENABLE == true>
                freq = SYS_CLK_BUS_REFERENCE_3;
</#if>
            break;
        case CLK_BUS_REFERENCE_4:
<#if CONFIG_SYS_CLK_REFCLK3_ENABLE == true>
                freq = SYS_CLK_BUS_REFERENCE_4;
</#if>
            break;
        case CLK_BUS_REFERENCE_5:
<#if CONFIG_HAVE_REFCLOCK5 == true>
<#if CONFIG_SYS_CLK_REFCLK4_ENABLE == true>
                freq = SYS_CLK_BUS_REFERENCE_5;
</#if>
</#if>
            break;
        default:
            break;
    }
    
    return freq;
</#if>
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
