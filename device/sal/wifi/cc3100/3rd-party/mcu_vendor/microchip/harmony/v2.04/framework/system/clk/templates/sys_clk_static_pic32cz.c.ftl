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
Copyright (c) 2014-2017 released Microchip Technology Inc.  All rights reserved.

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
<#macro CONFIGURE_PCR INDEX EN GCLKEN CSS DIV>
<#if EN || GCLKEN>

    /* Setup Peripheral Clock Generator ${INDEX} */
    gclkSetup.enableClockPeripheral = ${EN?c};
    gclkSetup.enableClockGeneric = ${GCLKEN?c};
<#if GCLKEN>
    gclkSetup.source = ${CSS};
    gclkSetup.divider = ${DIV};
<#else>
    gclkSetup.source = SYS_CLK_SOURCE_SLOW;
    gclkSetup.divider = 1;
</#if>
    SYS_CLK_GenericClockSetup ( ${INDEX}, &gclkSetup );
</#if>
</#macro>
<#macro CONFIGURE_PCK INDEX ENABLED CSS PRES>
<#if ENABLED>

    /* Setup Programmable Clock Generator ${INDEX} */
    pckSetup.enable = true;
    pckSetup.source = ${CSS};
    pckSetup.divider = ${PRES};
    SYS_CLK_ProgrammableClockSetup ( ${INDEX}, &pckSetup );
</#if>
</#macro>

// *****************************************************************************
// *****************************************************************************
// Section: Include Files
// *****************************************************************************
// *****************************************************************************

#include "system_config.h"
#include "system_definitions.h"
#include "system/clk/sys_clk.h"

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Functions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    void SYS_CLK_Static_Initialize ( const SYS_CLK_INIT const* clkInit )

  Summary:
    Initializes hardware and internal data structure of the System Clock.

  Description:
    This function initializes the hardware and internal data structure of System
    Clock Service.

  Remarks:
    For more details refer sys_clk.h.
*/

void SYS_CLK_Initialize( const SYS_CLK_INIT const* clkInit )
{
<#if CONFIG_SYS_CLK_SUPC_CR_XTALSEL_VALUE>
    SYS_CLK_SLOW_SETUP slowSetup;
</#if>
    SYS_CLK_MAIN_SETUP mainSetup;
<#if CONFIG_SYS_CLK_CKGR_PLLAR_DIVA0_MULA0>
    SYS_CLK_PLLA_SETUP pllaSetup;
</#if>
<#if CONFIG_SYS_CLK_CKGR_UCKR_UPLLEN>
    SYS_CLK_UPLL_SETUP upllSetup;
</#if>
<#if CONFIG_SYS_CLK_PMC_SCER_USBCLK>
    SYS_CLK_USB_CLOCK_SETUP usbClkSetup;
</#if>
    SYS_CLK_MASTER_SETUP masterSetup;
<#if CONFIG_SYS_CLK_PMC_PCR_EN0 || CONFIG_SYS_CLK_PMC_PCR_GCLKEN0 ||
     CONFIG_SYS_CLK_PMC_PCR_EN1 || CONFIG_SYS_CLK_PMC_PCR_GCLKEN1>
    SYS_CLK_GENERIC_SETUP gclkSetup;
</#if>
<#if CONFIG_SYS_CLK_PMC_SCER_PCK0 || CONFIG_SYS_CLK_PMC_SCER_PCK1 ||
     CONFIG_SYS_CLK_PMC_SCER_PCK2 || CONFIG_SYS_CLK_PMC_SCER_PCK3 ||
     CONFIG_SYS_CLK_PMC_SCER_PCK4 || CONFIG_SYS_CLK_PMC_SCER_PCK5 ||
     CONFIG_SYS_CLK_PMC_SCER_PCK6 || CONFIG_SYS_CLK_PMC_SCER_PCK7>
    SYS_CLK_PROGRAMMABLE_SETUP pckSetup;
</#if>

    /* Set maximum possible Flash Wait States value */
    _EFC_REGS->EEFC_FMR.w = EEFC_FMR_FWS(6);
<#if CONFIG_SYS_CLK_SUPC_CR_XTALSEL_VALUE>

    /* Setup slow clock source */
    slowSetup.enableXtal = true;
    slowSetup.bypassXtal = ${CONFIG_SYS_CLK_SUPC_MR_OSCBYPASS?c};
    SYS_CLK_SlowClockSetup ( &slowSetup );
</#if>

    /* Setup main clock source */
    mainSetup.enableXtal = ${CONFIG_SYS_CLK_CKGR_MOR_MOSCXTEN?c};
    mainSetup.bypassXtal = ${CONFIG_SYS_CLK_CKGR_MOR_MOSCXTBY?c};
    mainSetup.enableRcOsc = ${CONFIG_SYS_CLK_CKGR_MOR_MOSCRCEN?c};
<#if CONFIG_SYS_CLK_CKGR_MOR_MOSCRCEN >
    mainSetup.rcOscFrequency = ${CONFIG_SYS_CLK_CKGR_MOR_MOSCRCF_VALUE};
<#else>
    mainSetup.rcOscFrequency = SYS_CLK_RC_FREQUENCY_4_MHZ;
</#if>
    mainSetup.source = ${CONFIG_SYS_CLK_CKGR_MOR_MOSCSEL_VALUE};
    SYS_CLK_MainClockSetup ( &mainSetup );
<#if CONFIG_SYS_CLK_CKGR_PLLAR_DIVA0_MULA0>

    /* Setup PLLA */
    pllaSetup.enable = true;
    pllaSetup.multiplier = ${CONFIG_SYS_CLK_CKGR_PLLAR_MULA};
    pllaSetup.divider = ${CONFIG_SYS_CLK_CKGR_PLLAR_DIVA};
    SYS_CLK_PLLASetup ( &pllaSetup );
</#if>
<#if CONFIG_SYS_CLK_CKGR_UCKR_UPLLEN>

    /* Setup USB PLL */
    upllSetup.enable = true;
    upllSetup.multiplier = ${CONFIG_SYS_CLK_UTMI_CKTRIM_FREQ_VALUE};
    upllSetup.divider = ${CONFIG_SYS_CLK_PMC_MCKR_UPLLDIV2_VALUE};
    SYS_CLK_UPLLSetup ( &upllSetup );
</#if>
<#if CONFIG_SYS_CLK_PMC_SCER_USBCLK>

    /* Setup Full-Speed USB clock output */
    usbClkSetup.enable = true;
    usbClkSetup.source = ${CONFIG_SYS_CLK_PMC_USB_USBS_VALUE};
    usbClkSetup.divider = ${CONFIG_SYS_CLK_PMC_USB_USBDIV};
    SYS_CLK_USBClockSetup ( &usbClkSetup );
</#if>

    /* Configure master clock controller */
    masterSetup.source = ${CONFIG_SYS_CLK_PMC_MCKR_CSS_VALUE};
    masterSetup.prescaler = ${CONFIG_SYS_CLK_PMC_MCKR_PRES_VALUE};
    masterSetup.divider = ${CONFIG_SYS_CLK_PMC_MCKR_MDIV_VALUE};
    SYS_CLK_MasterClockSetup ( &masterSetup );

<@CONFIGURE_PCR 0 CONFIG_SYS_CLK_PMC_PCR_EN0 CONFIG_SYS_CLK_PMC_PCR_GCLKEN0 CONFIG_SYS_CLK_PMC_PCR_GCLKCSS0_VALUE CONFIG_SYS_CLK_PMC_PCR_GCLKDIV0 />
<@CONFIGURE_PCR 1 CONFIG_SYS_CLK_PMC_PCR_EN1 CONFIG_SYS_CLK_PMC_PCR_GCLKEN1 CONFIG_SYS_CLK_PMC_PCR_GCLKCSS1_VALUE CONFIG_SYS_CLK_PMC_PCR_GCLKDIV1 />
<@CONFIGURE_PCK INDEX=0 ENABLED=CONFIG_SYS_CLK_PMC_SCER_PCK0 CSS=CONFIG_SYS_CLK_PMC_PCK0_CSS_VALUE PRES=CONFIG_SYS_CLK_PMC_PCK0_PRES />
<@CONFIGURE_PCK INDEX=1 ENABLED=CONFIG_SYS_CLK_PMC_SCER_PCK1 CSS=CONFIG_SYS_CLK_PMC_PCK1_CSS_VALUE PRES=CONFIG_SYS_CLK_PMC_PCK1_PRES />
<@CONFIGURE_PCK INDEX=2 ENABLED=CONFIG_SYS_CLK_PMC_SCER_PCK2 CSS=CONFIG_SYS_CLK_PMC_PCK2_CSS_VALUE PRES=CONFIG_SYS_CLK_PMC_PCK2_PRES />
<@CONFIGURE_PCK INDEX=3 ENABLED=CONFIG_SYS_CLK_PMC_SCER_PCK3 CSS=CONFIG_SYS_CLK_PMC_PCK3_CSS_VALUE PRES=CONFIG_SYS_CLK_PMC_PCK3_PRES />
<@CONFIGURE_PCK INDEX=4 ENABLED=CONFIG_SYS_CLK_PMC_SCER_PCK4 CSS=CONFIG_SYS_CLK_PMC_PCK4_CSS_VALUE PRES=CONFIG_SYS_CLK_PMC_PCK4_PRES />
<@CONFIGURE_PCK INDEX=5 ENABLED=CONFIG_SYS_CLK_PMC_SCER_PCK5 CSS=CONFIG_SYS_CLK_PMC_PCK5_CSS_VALUE PRES=CONFIG_SYS_CLK_PMC_PCK5_PRES />
<@CONFIGURE_PCK INDEX=6 ENABLED=CONFIG_SYS_CLK_PMC_SCER_PCK6 CSS=CONFIG_SYS_CLK_PMC_PCK6_CSS_VALUE PRES=CONFIG_SYS_CLK_PMC_PCK6_PRES />
<@CONFIGURE_PCK INDEX=7 ENABLED=CONFIG_SYS_CLK_PMC_SCER_PCK7 CSS=CONFIG_SYS_CLK_PMC_PCK7_CSS_VALUE PRES=CONFIG_SYS_CLK_PMC_PCK7_PRES />

    /* - Enable the peripheral clock selected by MHC/peripheral driver */
   	_PMC_REGS->PMC_PCER0.w=${CONFIG_SYS_CLK_PMC_PCER0};
    _PMC_REGS->PMC_PCER1.w=${CONFIG_SYS_CLK_PMC_PCER1};

<#if CONFIG_USE_SYS_MEMORY_SMC == true>
    /* - Enable the SMC Clock at Power Manager level */
    _PMC_REGS->PMC_PCER0.w |= (1 << ID_SMC);
    /* - Enable the SMC PIO Clock for Mux signals at Power Manager level */
    _PMC_REGS->PMC_PCER0.w |=  ((1 << ID_PORTA )| (1 << ID_PORTB) | (1 << ID_PORTC )| (1 << ID_PORTE));
</#if>
}

//******************************************************************************
/* Function:
    inline uint32_t SYS_CLK_SystemFrequencyGet ( void )

  Summary:
    Gets the system clock frequency in Hertz.

  Description:
    This function gets the System clock frequency in Hertz.

  Remarks:
    For more details refer sys_clk.h.
 */

uint32_t SYS_CLK_SystemFrequencyGet ( void )
{
    return SYS_CLK_FrequencyGet ( SYS_CLK_PROCESSOR );
}

//******************************************************************************
/* Function:
    uint32_t SYS_CLK_FrequencyGet ( SYS_CLK_CLOCK clock )

  Summary:
    Gets the frequency of the specified clock source in Hertz.

  Description:
    This function gets the frequency of the specified clock source in Hertz.

  Remarks:
    For more details refer sys_clk.h.
 */

uint32_t SYS_CLK_FrequencyGet ( SYS_CLK_CLOCK clock )
{
    uint32_t freq;
    uint32_t slowClk;
    uint32_t mainClk;
    uint32_t pllaClk;
    uint32_t upllClk;
    uint32_t upllDivClk;
    uint32_t prescaledClk;
    uint32_t masterClk;
    int index;

    if ( _SUPC_REGS->SUPC_SR.OSCSEL == 1 )
    {
        slowClk = ${CONFIG_SYS_CLK_CONFIG_SLOWCLK_XTAL};
    }
    else
    {
        slowClk = CHIP_FREQ_SLCK_RC;
    }

    if ( 0 != (_PMC_REGS->PMC_SR.w & PMC_SR_MOSCSELS_Msk) )
    {
      mainClk = ${CONFIG_SYS_CLK_CONFIG_MAINCLK_XTAL};
    }
    else
    {
        switch (_PMC_REGS->CKGR_MOR.MOSCRCF)
        {
            case CKGR_MOR_MOSCRCF_4_MHz_Val:
                mainClk = CHIP_FREQ_MAINCK_RC_4MHZ;
                break;
            case CKGR_MOR_MOSCRCF_8_MHz_Val:
                mainClk = CHIP_FREQ_MAINCK_RC_8MHZ;
                break;
            case CKGR_MOR_MOSCRCF_12_MHz_Val:
                mainClk = CHIP_FREQ_MAINCK_RC_12MHZ;
                break;
            default:
                mainClk = 0;
                break;
        }
    }

    if ( _PMC_REGS->CKGR_PLLAR.MULA > 0 )
    {
        pllaClk = mainClk * (_PMC_REGS->CKGR_PLLAR.MULA + 1) / _PMC_REGS->CKGR_PLLAR.DIVA;
    }
    else
    {
        pllaClk = 0;
    }

    if ( _PMC_REGS->CKGR_UCKR.UPLLEN == 1 )
    {
        upllClk = CHIP_FREQ_XTAL_12M * 40;

        if ( _PMC_REGS->PMC_MCKR.UPLLDIV2 == 1 )
        {
            upllDivClk = upllClk / 2;
        }
        else
        {
            upllDivClk = upllClk;
        }
    }
    else
    {
        upllClk = 0;
        upllDivClk = 0;
    }

    switch ( _PMC_REGS->PMC_MCKR.CSS )
    {
        case PMC_MCKR_CSS_SLOW_CLK_Val:
            prescaledClk = slowClk;
            break;
        case PMC_MCKR_CSS_MAIN_CLK_Val:
            prescaledClk = mainClk;
            break;
        case PMC_MCKR_CSS_PLLA_CLK_Val:
            prescaledClk = pllaClk;
            break;
        case PMC_MCKR_CSS_UPLL_CLK_Val:
            prescaledClk = upllDivClk;
            break;
        default:
            prescaledClk = 0;
            break;
    }

    switch ( _PMC_REGS->PMC_MCKR.PRES )
    {
        case PMC_MCKR_PRES_CLK_2_Val:
            prescaledClk = prescaledClk / 2;
            break;
        case PMC_MCKR_PRES_CLK_3_Val:
            prescaledClk = prescaledClk / 3;
            break;
        case PMC_MCKR_PRES_CLK_4_Val:
            prescaledClk = prescaledClk / 4;
            break;
        case PMC_MCKR_PRES_CLK_8_Val:
            prescaledClk = prescaledClk / 8;
            break;
        case PMC_MCKR_PRES_CLK_16_Val:
            prescaledClk = prescaledClk / 16;
            break;
        case PMC_MCKR_PRES_CLK_32_Val:
            prescaledClk = prescaledClk / 32;
            break;
        case PMC_MCKR_PRES_CLK_64_Val:
            prescaledClk = prescaledClk / 64;
            break;
        default:
            prescaledClk = prescaledClk;
            break;
    }

    switch ( _PMC_REGS->PMC_MCKR.MDIV )
    {
        case PMC_MCKR_MDIV_EQ_PCK_Val:
            masterClk = prescaledClk;
            break;
        case PMC_MCKR_MDIV_PCK_DIV2_Val:
            masterClk = prescaledClk / 2;
            break;
        case PMC_MCKR_MDIV_PCK_DIV3_Val:
            masterClk = prescaledClk / 3;
            break;
        case PMC_MCKR_MDIV_PCK_DIV4_Val:
            masterClk = prescaledClk / 4;
            break;
        default:
            masterClk = 0;
            break;
    }

    switch ( clock )
    {
        case SYS_CLK_PROCESSOR:
            freq = prescaledClk;
            break;

        case SYS_CLK_MASTER:
            freq = masterClk;
            break;

        case SYS_CLK_SYSTICK:
            freq = prescaledClk / 8;
            break;

        case SYS_CLK_GENERIC_0:
        case SYS_CLK_GENERIC_1:
            if ( clock == SYS_CLK_GENERIC_0 )
            {
              _PMC_REGS->PMC_PCR.w = PMC_PCR_PID(69);
            }
            else
            {
              _PMC_REGS->PMC_PCR.w = PMC_PCR_PID(70);
            }


            if ( _PMC_REGS->PMC_PCR.GCLKEN == 1 )
            {
                switch ( _PMC_REGS->PMC_PCR.GCLKCSS )
                {
                    case PMC_PCR_GCLKCSS_SLOW_CLK_Val:
                        freq = slowClk;
                        break;
                    case PMC_PCR_GCLKCSS_MAIN_CLK_Val:
                        freq = mainClk;
                        break;
                    case PMC_PCR_GCLKCSS_PLLA_CLK_Val:
                        freq = pllaClk;
                        break;
                    case PMC_PCR_GCLKCSS_UPLL_CLK_Val:
                        freq = upllDivClk;
                        break;
                    case PMC_PCR_GCLKCSS_MCK_CLK_Val:
                        freq = masterClk;
                        break;
                    default:
                        freq = 0;
                        break;
                }

                freq = freq / (_PMC_REGS->PMC_PCR.GCLKDIV + 1);
            }
            else
            {
                freq = 0;
            }
            break;

        case SYS_CLK_PERIPHERAL_0:
        case SYS_CLK_PERIPHERAL_1:
            if ( clock  == SYS_CLK_PERIPHERAL_0 )
            {
                _PMC_REGS->PMC_PCR.w = PMC_PCR_PID(69);
            }
            else
            {
                _PMC_REGS->PMC_PCR.w = PMC_PCR_PID(70);
            }


            if ( _PMC_REGS->PMC_PCR.EN == 1 )
            {
                freq = masterClk;
            }
            else
            {
                freq = 0;
            }
            break;

        case SYS_CLK_PROGRAMMABLE_0:
        case SYS_CLK_PROGRAMMABLE_1:
        case SYS_CLK_PROGRAMMABLE_2:
        case SYS_CLK_PROGRAMMABLE_3:
        case SYS_CLK_PROGRAMMABLE_4:
        case SYS_CLK_PROGRAMMABLE_5:
        case SYS_CLK_PROGRAMMABLE_6:
        case SYS_CLK_PROGRAMMABLE_7:
            index = clock - SYS_CLK_PROGRAMMABLE_0;

            if ( 0 != (_PMC_REGS->PMC_SCSR.w & (PMC_SCSR_PCK0_Msk << index)) )
            {
                switch ( _PMC_REGS->PMC_PCK[index].CSS )
                {
                    case PMC_PCK_CSS_SLOW_CLK_Val:
                        freq = slowClk;
                        break;
                    case PMC_PCK_CSS_MAIN_CLK_Val:
                        freq = mainClk;
                        break;
                    case PMC_PCK_CSS_PLLA_CLK_Val:
                        freq = pllaClk;
                        break;
                    case PMC_PCK_CSS_UPLL_CLK_Val:
                        freq = upllDivClk;
                        break;
                    case PMC_PCK_CSS_MCK_Val:
                        freq = masterClk;
                        break;
                    default:
                        freq = 0;
                        break;
                }

                freq = freq / (_PMC_REGS->PMC_PCK[index].PRES + 1);
            }
            else
            {
                freq = 0;
            }
            break;

        case SYS_CLK_USB_FS:
            freq = upllClk;
            break;

        case SYS_CLK_USB_HS:
            if ( _PMC_REGS->PMC_SCER.USBCLK == 1 )
            {
                if ( _PMC_REGS->PMC_USB.USBS == 1 )
                {
                    freq = upllClk;
                }
                else
                {
                    freq = pllaClk;
                }

                freq = freq / (_PMC_REGS->PMC_USB.USBDIV + 1);
            }
            else
            {
                freq = 0;
            }
            break;

        default:
            freq = 0;
            break;
    }

    return freq;
}

//******************************************************************************
/* Function:
    void SYS_CLK_MainClockSetup ( const SYS_CLK_MAIN_SETUP* setup )

  Summary:
    Sets up the source for the Main Clock .

  Description:
    This function sets up the source for the Main Clock.

  Remarks:
    For more details refer sys_clk.h.
 */

void SYS_CLK_MainClockSetup ( const SYS_CLK_MAIN_SETUP* setup )
{
    uint32_t mor;

    if ( setup->enableRcOsc )
    {
        /*Enables Main RC Oscillator */
        _PMC_REGS->CKGR_MOR.w |= CKGR_MOR_KEY_PASSWD | CKGR_MOR_MOSCRCEN_Msk;

        /* Polls whether Main RC oscillator is stabilized 
           after it is enabled. */
        while( !(_PMC_REGS->PMC_SR.w & PMC_SR_MOSCRCS_Msk) )
        {
        }
    }

    /* Changes RC Oscillator frequency only if Main RC is stabilized */
    if( _PMC_REGS->PMC_SR.w & PMC_SR_MOSCRCS_Msk )
    {
        mor = _PMC_REGS->CKGR_MOR.w & ~(uint32_t)CKGR_MOR_MOSCRCF_Msk;

        switch (setup->rcOscFrequency)
        {
            case SYS_CLK_RC_FREQUENCY_4_MHZ:
                mor |= CKGR_MOR_MOSCRCF_4_MHz;
                break;
            case SYS_CLK_RC_FREQUENCY_8_MHZ:
                mor |= CKGR_MOR_MOSCRCF_8_MHz;
                break;
            case SYS_CLK_RC_FREQUENCY_12_MHZ:
                mor |= CKGR_MOR_MOSCRCF_12_MHz;
                break;
            default:
                /* 12 MHz is the default calibrated frequency after reset */
                mor |= CKGR_MOR_MOSCRCF_12_MHz;
                break;
        }

        _PMC_REGS->CKGR_MOR.w = mor;

        /* Polls whether Main RC oscillator is stabilized 
           after changing frequency. */
        while( !(_PMC_REGS->PMC_SR.w & PMC_SR_MOSCRCS_Msk) )
        {
        }
    }

    if ( setup->bypassXtal )
    {
        /* Enables Main Crystal oscillator Bypass and
           clears the Main Crystal oscillator enable bit */
        _PMC_REGS->CKGR_MOR.w = CKGR_MOR_KEY_PASSWD | CKGR_MOR_MOSCXTBY_Msk | 
                                (_PMC_REGS->CKGR_MOR.w & ~(uint32_t)CKGR_MOR_MOSCXTEN_Msk);
    }
    else if ( setup->enableXtal )
    {
        /* Enables the Main Crystal Oscillator and 
           sets the Main Crystal Oscillator Startup Time */
        _PMC_REGS->CKGR_MOR.w = CKGR_MOR_KEY_PASSWD | CKGR_MOR_MOSCXTST(${CONFIG_SYS_CLK_CKGR_MOSCXTST}) |
                                CKGR_MOR_MOSCXTEN_Msk | 
                                (_PMC_REGS->CKGR_MOR.w & ~(uint32_t)CKGR_MOR_MOSCXTST_Msk);

        while ( !(_PMC_REGS->PMC_SR.w & PMC_SR_MOSCXTS_Msk) )
        {
        }
    }

    if ( setup->source == SYS_CLK_MAIN_SOURCE_XTAL )
    {
        /* Selects the Main Crystal Oscillator */
        _PMC_REGS->CKGR_MOR.w |= CKGR_MOR_KEY_PASSWD | CKGR_MOR_MOSCSEL_Msk;

        while ( !(_PMC_REGS->PMC_SR.w & PMC_SR_MOSCSELS_Msk) )
        {
        }
    }
}

//******************************************************************************
/* Function:
    void SYS_CLK_SlowClockSetup ( const SYS_CLK_SLOW_SETUP* setup )

  Summary:
    Sets up the source for the Slow Clock .

  Description:
    This function sets up the source for the Slow Clock.

  Remarks:
    For more details refer sys_clk.h.
 */

void SYS_CLK_SlowClockSetup ( const SYS_CLK_SLOW_SETUP* setup )
{
    if ( setup->enableXtal )
    {
        if ( setup->bypassXtal )
        {
            _SUPC_REGS->SUPC_MR.w |= SUPC_MR_OSCBYPASS_BYPASS;
        }

        _SUPC_REGS->SUPC_CR.w |= SUPC_CR_KEY_PASSWD | SUPC_CR_XTALSEL_CRYSTAL_SEL;

        while (_SUPC_REGS->SUPC_SR.OSCSEL == 0)
        {
        }
    }
    else
    {
      /* Reverting to the Slow RC oscillator is only possible
         by shutting down the VDDIO power supply.
      */
    }
}

//******************************************************************************
/* Function:
    void SYS_CLK_PLLASetup ( const SYS_CLK_PLLA_SETUP* setup )

  Summary:
    Sets up the PLLA frequency.

  Description:
    This function sets up the PLLA frequency.

  Remarks:
    For more details refer sys_clk.h.
 */

void SYS_CLK_PLLASetup ( const SYS_CLK_PLLA_SETUP* setup )
{
    if (setup->enable)
    {
        _PMC_REGS->CKGR_PLLAR.w = CKGR_PLLAR_ONE_Msk | CKGR_PLLAR_PLLACOUNT(0x3f) |
            CKGR_PLLAR_MULA(setup->multiplier - 1) |
            CKGR_PLLAR_DIVA(setup->divider);

        while ( !(_PMC_REGS->PMC_SR.w & PMC_SR_LOCKA_Msk) )
        {
        }
    }
    else
    {
        _PMC_REGS->CKGR_PLLAR.w = CKGR_PLLAR_ONE_Msk | CKGR_PLLAR_PLLACOUNT(0) |
            CKGR_PLLAR_MULA(0) | CKGR_PLLAR_DIVA(0);
    }
}

//******************************************************************************
/* Function:
    void SYS_CLK_UPLLSetup ( const SYS_CLK_UPLL_SETUP* setup )

  Summary:
    Sets up the USB PLL frequency.

  Description:
    This function sets up the USB PLL frequency.

  Remarks:
    For more details refer sys_clk.h.
 */

void SYS_CLK_UPLLSetup ( const SYS_CLK_UPLL_SETUP* setup )
{
    /* Disable UPPL to get a predictable starting point */
    _PMC_REGS->CKGR_UCKR.w = 0;

    if ( setup->enable)
    {
        if ( setup->multiplier == 30 )
        {
            _UTMI_REGS->UTMI_CKTRIM.w = UTMI_CKTRIM_FREQ_XTAL16;
        }
        else
        {
            _UTMI_REGS->UTMI_CKTRIM.w = UTMI_CKTRIM_FREQ_XTAL12;
        }

        if ( setup->divider == 2 )
        {
          _PMC_REGS->PMC_MCKR.UPLLDIV2 = 1;
        }
        else
        {
          _PMC_REGS->PMC_MCKR.UPLLDIV2 = 0;
        }

        _PMC_REGS->CKGR_UCKR.w = CKGR_UCKR_UPLLEN_Msk | CKGR_UCKR_UPLLCOUNT(3);
        while (0 == (_PMC_REGS->PMC_SR.w & PMC_SR_LOCKU_Msk));
    }
}

//******************************************************************************
/* Function:
    void SYS_CLK_MasterClockSetup ( const SYS_CLK_MASTER_SETUP* setup )

  Summary:
    Sets up the Master Clock Controller.

  Description:
    This function sets up the Master Clock Controller. This includes
    configuring the source clock for the Master Clock, Processor Clock and
    SysTick timer.

  Remarks:
    For more details refer sys_clk.h.
 */

void SYS_CLK_MasterClockSetup ( const SYS_CLK_MASTER_SETUP* setup )
{
    uint32_t mckr;

    mckr = _PMC_REGS->PMC_MCKR.w & ~(uint32_t)(PMC_MCKR_PRES_Msk | PMC_MCKR_MDIV_Msk);

    switch (setup->prescaler)
    {
        case SYS_CLK_MASTER_PRESCALER_1:
            mckr |= PMC_MCKR_PRES_CLK_1;
            break;
        case SYS_CLK_MASTER_PRESCALER_2:
            mckr |= PMC_MCKR_PRES_CLK_2;
            break;
        case SYS_CLK_MASTER_PRESCALER_3:
            mckr |= PMC_MCKR_PRES_CLK_3;
            break;
        case SYS_CLK_MASTER_PRESCALER_4:
            mckr |= PMC_MCKR_PRES_CLK_4;
            break;
        case SYS_CLK_MASTER_PRESCALER_8:
            mckr |= PMC_MCKR_PRES_CLK_8;
            break;
        case SYS_CLK_MASTER_PRESCALER_16:
            mckr |= PMC_MCKR_PRES_CLK_16;
            break;
        case SYS_CLK_MASTER_PRESCALER_32:
            mckr |= PMC_MCKR_PRES_CLK_32;
            break;
        case SYS_CLK_MASTER_PRESCALER_64:
            mckr |= PMC_MCKR_PRES_CLK_64;
            break;
        default:
            mckr |= PMC_MCKR_PRES_CLK_64;
            break;
    }

    switch (setup->divider)
    {
        case SYS_CLK_MASTER_DIVIDER_1:
            mckr |= PMC_MCKR_MDIV_EQ_PCK;
            break;
        case SYS_CLK_MASTER_DIVIDER_2:
            mckr |= PMC_MCKR_MDIV_PCK_DIV2;
            break;
        case SYS_CLK_MASTER_DIVIDER_3:
            mckr |= PMC_MCKR_MDIV_PCK_DIV3;
            break;
        case SYS_CLK_MASTER_DIVIDER_4:
            mckr |= PMC_MCKR_MDIV_PCK_DIV4;
            break;
        default:
            mckr |= PMC_MCKR_MDIV_PCK_DIV4;
            break;
    }

    _PMC_REGS->PMC_MCKR.w = mckr;

    while ( !(_PMC_REGS->PMC_SR.w & PMC_SR_MCKRDY_Msk) )
    {
    }

    _PMC_REGS->PMC_MCKR.w = (mckr & ~(uint32_t)PMC_MCKR_CSS_Msk) | setup->source;

    while ( !(_PMC_REGS->PMC_SR.w & PMC_SR_MCKRDY_Msk) )
    {
    }
}

//******************************************************************************
/* Function:
    void SYS_CLK_ProgrammableClockSetup ( const SYS_CLK_PCK controller, const SYS_CLK_PROGRAMMABLE_SETUP* setup )

  Summary:
    Sets up the specified Programmable Clock.

  Description:
    This function sets up the specified Programmable Clock.

  Remarks:
    For more details refer sys_clk.h.
 */

void SYS_CLK_ProgrammableClockSetup ( const SYS_CLK_PCK controller, const SYS_CLK_PROGRAMMABLE_SETUP* setup )
{
    _PMC_REGS->PMC_SCDR.w = PMC_SCDR_PCK(controller);

    if ( setup->enable )
    {
        _PMC_REGS->PMC_PCK[controller].w = PMC_PCK_CSS(setup->source) |
            PMC_PCK_PRES(setup->divider - 1);
        _PMC_REGS->PMC_SCER.w = PMC_SCER_PCK(1 << controller);
    }
}


//******************************************************************************
/* Function:
    void SYS_CLK_GenericClockSetup ( const SYS_CLK_GCLK controller, const SYS_CLK_GENERIC_SETUP* setup )

  Summary:
    Sets up the specified Generic and Peripheral Clock.

  Description:
    This function sets up the specified Generic and Peripheral Clock.

  Remarks:
    For more details refer sys_clk.h.
 */

void SYS_CLK_GenericClockSetup ( const SYS_CLK_GCLK controller, const SYS_CLK_GENERIC_SETUP* setup )
{
    uint32_t pcr = PMC_PCR_CMD_Msk;

    if (controller == SYS_CLK_GCLK_0)
    {
        pcr |= PMC_PCR_PID(69);
    }
    else
    {
        pcr |= PMC_PCR_PID(70);
    }

    if ( setup->enableClockGeneric )
    {
      pcr |= PMC_PCR_GCLKEN_Msk;
    }

    if ( setup->enableClockPeripheral )
    {
      pcr |= PMC_PCR_EN_Msk;
    }

    pcr |= PMC_PCR_GCLKCSS(setup->source);
    pcr |= PMC_PCR_GCLKDIV(setup->divider - 1);

    _PMC_REGS->PMC_PCR.w = pcr;
}

//******************************************************************************
/* Function:
    void SYS_CLK_USBClockSetup ( const SYS_CLK_USB_CLOCK_SETUP* setup )

  Summary:
    Sets up the Full-Spee USB Clock.

  Description:
    This function sets up the Full-Speed USB Clock.

  Remarks:
    For more details refer sys_clk.h.
 */

void SYS_CLK_USBClockSetup ( const SYS_CLK_USB_CLOCK_SETUP* setup )
{
    _PMC_REGS->PMC_SCER.USBCLK = 0;

    if ( setup->enable )
    {
        uint32_t pmc_usb = PMC_USB_USBDIV(setup->divider - 1);

        if ( setup->source == SYS_CLK_SOURCE_USB_PLL )
        {
            pmc_usb |= PMC_USB_USBS_Msk;
        }

        _PMC_REGS->PMC_USB.w = pmc_usb;

        _PMC_REGS->PMC_SCER.USBCLK = 1;
    }
}

//******************************************************************************
/* Function:
    void SYS_CLK_PeripheralEnable ( int peripheralId )

  Summary:
    Enables clock to the specified peripheral.

  Description:
    This function enables clock to the specified peripheral.

  Remarks:
    For more details refer sys_clk.h.
 */

void SYS_CLK_PeripheralEnable ( int peripheralId )
{
    if (peripheralId < 32)
    {
        _PMC_REGS->PMC_PCER0.w = 1u << peripheralId;
    }
    else
    {
        _PMC_REGS->PMC_PCER1.w = 1u << (peripheralId - 32);
    }
}

//******************************************************************************
/* Function:
    void SYS_CLK_PeripheralDisable ( int peripheralId )

  Summary:
    Disables clock to the specified peripheral.

  Description:
    This function disables clock to the specified peripheral.

  Remarks:
    For more details refer sys_clk.h.
 */

void SYS_CLK_PeripheralDisable ( int peripheralId )
{
    if (peripheralId < 32)
    {
        _PMC_REGS->PMC_PCDR0.w = 1u << peripheralId;
    }
    else
    {
        _PMC_REGS->PMC_PCDR1.w = 1u << (peripheralId - 32);
    }
}

