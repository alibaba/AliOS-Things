/*
 * The Clear BSD License
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright (c) 2016 - 2017 , NXP
 * All rights reserved.
 *
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 * that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "fsl_common.h"
#include "fsl_clock.h"
#include "fsl_power.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.clock"
#endif
#define NVALMAX (0x100U)
#define PVALMAX (0x20U)
#define MVALMAX (0x8000U)

#define USB_NVALMAX (0x4U)
#define USB_PVALMAX (0x8U)
#define USB_MVALMAX (0x100U)

#define PLL_MAX_N_DIV 0x100U
#define USB_PLL_MAX_N_DIV 0x100U

#define PLL_MDEC_VAL_P (0U)                                      /*!<  MDEC is in bits  16 downto 0 */
#define PLL_MDEC_VAL_M (0x1FFFFUL << PLL_MDEC_VAL_P)             /*!<  NDEC is in bits  9 downto 0 */
#define PLL_NDEC_VAL_P (0U)                                      /*!<  NDEC is in bits  9:0 */
#define PLL_NDEC_VAL_M (0x3FFUL << PLL_NDEC_VAL_P)
#define PLL_PDEC_VAL_P (0U)                                      /*!<  PDEC is in bits 6:0 */
#define PLL_PDEC_VAL_M (0x7FUL << PLL_PDEC_VAL_P)

#define PLL_MIN_CCO_FREQ_MHZ (275000000U)
#define PLL_MAX_CCO_FREQ_MHZ (550000000U)
#define PLL_LOWER_IN_LIMIT (4000U)                               /*!<  Minimum PLL input rate */
#define PLL_MIN_IN_SSMODE (2000000U)
#define PLL_MAX_IN_SSMODE (4000000U)

/*!<  Middle of the range values for spread-spectrum */
#define PLL_SSCG_MF_FREQ_VALUE 4U
#define PLL_SSCG_MC_COMP_VALUE 2U
#define PLL_SSCG_MR_DEPTH_VALUE 4U
#define PLL_SSCG_DITHER_VALUE 0U

/*!<  USB PLL CCO MAX AND MIN FREQ */
#define USB_PLL_MIN_CCO_FREQ_MHZ (156000000U)
#define USB_PLL_MAX_CCO_FREQ_MHZ (320000000U)
#define USB_PLL_LOWER_IN_LIMIT (1000000U)                             /*!<  Minimum PLL input rate */

#define USB_PLL_MSEL_VAL_P (0U)                                       /*!<  MSEL is in bits  7 downto 0 */
#define USB_PLL_MSEL_VAL_M (0xFFU)
#define USB_PLL_PSEL_VAL_P (8U)                                       /*!<  PDEC is in bits 9:8 */
#define USB_PLL_PSEL_VAL_M (0x3U)
#define USB_PLL_NSEL_VAL_P (10U)                                      /*!<  NDEC is in bits  11:10 */
#define USB_PLL_NSEL_VAL_M (0x3U)

/*!<  SYS PLL NDEC reg */
#define PLL_NDEC_VAL_SET(value) (((unsigned long)(value) << PLL_NDEC_VAL_P) & PLL_NDEC_VAL_M)
/*!<  SYS PLL PDEC reg */
#define PLL_PDEC_VAL_SET(value) (((unsigned long)(value) << PLL_PDEC_VAL_P) & PLL_PDEC_VAL_M)
/*!<  SYS PLL MDEC reg */
#define PLL_MDEC_VAL_SET(value) (((unsigned long)(value) << PLL_MDEC_VAL_P) & PLL_MDEC_VAL_M)

/*!<  SYS PLL NSEL reg */
#define USB_PLL_NSEL_VAL_SET(value) (((unsigned long)(value) & USB_PLL_NSEL_VAL_M) << USB_PLL_NSEL_VAL_P)
/*!<  SYS PLL PSEL reg */
#define USB_PLL_PSEL_VAL_SET(value) (((unsigned long)(value) & USB_PLL_PSEL_VAL_M) << USB_PLL_PSEL_VAL_P)
/*!<  SYS PLL MSEL reg */
#define USB_PLL_MSEL_VAL_SET(value) (((unsigned long)(value) & USB_PLL_MSEL_VAL_M) << USB_PLL_MSEL_VAL_P)

/*!<  FRAC control */
#define AUDIO_PLL_FRACT_MD_P (0U)
#define AUDIO_PLL_FRACT_MD_INT_P (15U)
#define AUDIO_PLL_FRACT_MD_M (0x7FFFUL << AUDIO_PLL_FRACT_MD_P)
#define AUDIO_PLL_FRACT_MD_INT_M (0x7FUL << AUDIO_PLL_FRACT_MD_INT_P)

#define AUDIO_PLL_MD_FRACT_SET(value) (((unsigned long)(value) << AUDIO_PLL_FRACT_MD_P) & PLL_FRAC_MD_FRACT_M)
#define AUDIO_PLL_MD_INT_SET(value) (((unsigned long)(value) << AUDIO_PLL_FRACT_MD_INT_P) & AUDIO_PLL_FRACT_MD_INT_M)

/* Saved value of PLL output rate, computed whenever needed to save run-time
   computation on each call to retrive the PLL rate. */
static uint32_t s_Pll_Freq;
static uint32_t s_Usb_Pll_Freq;
static uint32_t s_Audio_Pll_Freq;


/** External clock rate on the CLKIN pin in Hz. If not used,
    set this to 0. Otherwise, set it to the exact rate in Hz this pin is
    being driven at. */
static const uint32_t s_I2S_Mclk_Freq = 0U;
static const uint32_t s_Ext_Clk_Freq = 12000000U;
static const uint32_t s_Lcd_Clk_In_Freq = 0U;

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/* Find encoded NDEC value for raw N value, max N = NVALMAX */
static uint32_t pllEncodeN(uint32_t N);
/* Find decoded N value for raw NDEC value */
static uint32_t pllDecodeN(uint32_t NDEC);
/* Find encoded PDEC value for raw P value, max P = PVALMAX */
static uint32_t pllEncodeP(uint32_t P);
/* Find decoded P value for raw PDEC value */
static uint32_t pllDecodeP(uint32_t PDEC);
/* Find encoded MDEC value for raw M value, max M = MVALMAX */
static uint32_t pllEncodeM(uint32_t M);
/* Find decoded M value for raw MDEC value */
static uint32_t pllDecodeM(uint32_t MDEC);
/* Find SELP, SELI, and SELR values for raw M value, max M = MVALMAX */
static void pllFindSel(uint32_t M, uint32_t *pSelP, uint32_t *pSelI, uint32_t *pSelR);
/* Get predivider (N) from PLL NDEC setting */
static uint32_t findPllPreDiv(uint32_t ctrlReg, uint32_t nDecReg);
/* Get postdivider (P) from PLL PDEC setting */
static uint32_t findPllPostDiv(uint32_t ctrlReg, uint32_t pDecReg);
/* Get multiplier (M) from PLL MDEC and BYPASS_FBDIV2 settings */
static uint32_t findPllMMult(uint32_t ctrlReg, uint32_t mDecReg);
/* Convert the binary to fractional part */
static double Binary2Fractional(uint32_t binaryPart);
/* Get the greatest common divisor */
static uint32_t FindGreatestCommonDivisor(uint32_t m, uint32_t n);
/* Set PLL output based on desired output rate */
static pll_error_t CLOCK_GetPllConfig(
    uint32_t finHz, uint32_t foutHz, pll_setup_t *pSetup);

/* Update local PLL rate variable */
static void CLOCK_GetSystemPLLOutFromSetupUpdate(pll_setup_t *pSetup);
static void CLOCK_GetAudioPLLOutFromSetupUpdate(pll_setup_t *pSetup);

static const uint8_t wdtFreqLookup[32] = {0, 8, 12, 15, 18, 20, 24, 26, 28, 30, 32, 34, 36, 38, 40, 41, 42, 44, 45, 46,
                                            48, 49, 50, 52, 53, 54, 56, 57, 58, 59, 60, 61};
/*******************************************************************************
 * Code
 ******************************************************************************/

/* Clock Selection for IP */
void CLOCK_AttachClk(clock_attach_id_t connection)
{
    uint8_t mux;
    uint8_t sel;
    uint16_t item;
    uint32_t i;
    volatile uint32_t *pClkSel;

    pClkSel = &(SYSCON->MAINCLKSELA);

    if (connection != kNONE_to_NONE)
    {
        for (i = 0U; i < 2U; i++)
        {
            if (connection == 0U)
            {
                break;
            }
            item = (uint16_t)GET_ID_ITEM(connection);
            if (item)
            {
                mux = GET_ID_ITEM_MUX(item);
                sel = GET_ID_ITEM_SEL(item);
                if (mux == CM_ASYNCAPB)
                {
                    SYSCON->ASYNCAPBCTRL = SYSCON_ASYNCAPBCTRL_ENABLE(1);
                    ASYNC_SYSCON->ASYNCAPBCLKSELA = sel;
                }
                else
                {
                    pClkSel[mux] = sel;
                }
            }
            connection = GET_ID_NEXT_ITEM(connection); /* pick up next descriptor */
        }
    }
}

/* Return the actual clock attach id */
clock_attach_id_t CLOCK_GetClockAttachId(clock_attach_id_t attachId)
{
    uint8_t mux;
    uint8_t actualSel;
    uint32_t i;
    uint32_t actualAttachId = 0U;
    uint32_t selector = GET_ID_SELECTOR(attachId);
    volatile uint32_t *pClkSel;

    pClkSel = &(SYSCON->MAINCLKSELA);

    if (attachId == kNONE_to_NONE)
    {
        return kNONE_to_NONE;
    }

    for (i = 0U; i < 2U; i++)
    {
        mux = GET_ID_ITEM_MUX(attachId);
        if (attachId)
        {
            if (mux == CM_ASYNCAPB)
            {
                actualSel = ASYNC_SYSCON->ASYNCAPBCLKSELA;
            }
            else
            {
                actualSel = pClkSel[mux];
            }

            /* Consider the combination of two registers */
            actualAttachId |= CLK_ATTACH_ID(mux, actualSel, i);
        }
        attachId = GET_ID_NEXT_ITEM(attachId); /*!<  pick up next descriptor */
    }
		
    /* Clean unmeaningful selection */
    if((GET_ID_ITEM_SEL(actualAttachId >> 12U) != (selector >> 24U)) && GET_ID_NEXT_ITEM(actualAttachId))
    {
        CLR_ID_ITEM_SEL(actualAttachId);
    }
    actualAttachId |= selector;
    
    return (clock_attach_id_t)actualAttachId;
}

/* Set IP Clock Divider */
void CLOCK_SetClkDiv(clock_div_name_t div_name, uint32_t divided_by_value, bool reset)
{
    volatile uint32_t *pClkDiv;

    pClkDiv = &(SYSCON->SYSTICKCLKDIV);
    if (reset)
    {
        pClkDiv[div_name] = 1U << 29U;
    }
    if (divided_by_value == 0U) /*!<  halt */
    {
        pClkDiv[div_name] = 1U << 30U;
    }
    else
    {
        pClkDiv[div_name] = (divided_by_value - 1U);
    }
}

/* Set FRO Clocking */
status_t CLOCK_SetupFROClocking(uint32_t iFreq)
{
    uint32_t usb_adj;
    if ((iFreq != 12000000U) && (iFreq != 48000000U) && (iFreq != 96000000U))
    {
        return kStatus_Fail;
    }
    /* Power up the FRO and set this as the base clock */
    POWER_DisablePD(kPDRUNCFG_PD_FRO_EN);
    /* back up the value of whether USB adj is selected, in which case we will have a value of 1 else 0 */
    usb_adj = ((SYSCON->FROCTRL) & SYSCON_FROCTRL_USBCLKADJ_MASK) >> SYSCON_FROCTRL_USBCLKADJ_SHIFT;

    if (iFreq > 12000000U)
    {
        /* Call ROM API to set FRO */
        set_fro_frequency(iFreq);
        if (iFreq == 96000000U)
        {
            SYSCON->FROCTRL |= (SYSCON_FROCTRL_SEL(1) | SYSCON_FROCTRL_WRTRIM(1) | SYSCON_FROCTRL_USBCLKADJ(usb_adj) | SYSCON_FROCTRL_HSPDCLK(1));
        }
        else
        {
            SYSCON->FROCTRL |= (SYSCON_FROCTRL_SEL(0) | SYSCON_FROCTRL_WRTRIM(1) | SYSCON_FROCTRL_USBCLKADJ(usb_adj) | SYSCON_FROCTRL_HSPDCLK(1));
        }
    }
    else
    {
        SYSCON->FROCTRL &= ~SYSCON_FROCTRL_HSPDCLK(1);
    }

    return 0U;
}

/* Get CLOCK OUT Clk */
uint32_t CLOCK_GetClockOutClkFreq(void)
{
    uint32_t freq = 0U;

    switch(SYSCON->CLKOUTSELA)
    {
        case 0U:
            freq = CLOCK_GetCoreSysClkFreq();
            break;

        case 1U:
            freq = CLOCK_GetExtClkFreq();
            break;

        case 2U:
            freq = CLOCK_GetWdtOscFreq();
            break;

        case 3U:
            freq = CLOCK_GetFroHfFreq();
            break;

        case 4U:
            freq = CLOCK_GetPllOutFreq();
            break;

        case 5U:
            freq = CLOCK_GetUsbPllOutFreq();
            break;

        case 6U:
            freq = CLOCK_GetAudioPllOutFreq();
            break;

        case 7U:
            freq = CLOCK_GetOsc32KFreq();
            break;

        default:
            break;
    }
    return freq / ((SYSCON->CLKOUTDIV&0xffU)+1U);
}

/* Get SPIFI Clk */
uint32_t CLOCK_GetSpifiClkFreq(void)
{
    uint32_t freq = 0U;

    switch(SYSCON->SPIFICLKSEL)
    {
        case 0U:
          freq = CLOCK_GetCoreSysClkFreq();
          break;
        case 1U:
          freq = CLOCK_GetPllOutFreq();
          break;
        case 2U:
          freq = CLOCK_GetUsbPllOutFreq();
          break;
        case 3U:
          freq = CLOCK_GetFroHfFreq();
          break;
        case 4U:
          freq = CLOCK_GetAudioPllOutFreq();
          break;
        case 7U:
          freq = 0U;
          break;
        default:
          break;
    }

    return freq / ((SYSCON->SPIFICLKDIV & 0xffU) + 1U);
}

/* Get ADC Clk */
uint32_t CLOCK_GetAdcClkFreq(void)
{
    uint32_t freq = 0U;

    switch(SYSCON->ADCCLKSEL)
    {
        case 0U:
          freq = CLOCK_GetFroHfFreq();
          break;
        case 1U:
          freq = CLOCK_GetPllOutFreq();
          break;
        case 2U:
          freq = CLOCK_GetUsbPllOutFreq();
          break;
        case 3U:
          freq = CLOCK_GetAudioPllOutFreq();
          break;
        case 7U:
          freq = 0U;
          break;
        default:
          break;
    }

    return freq / ((SYSCON->ADCCLKDIV & 0xffU) + 1U);
}

/* Get USB0 Clk */
uint32_t CLOCK_GetUsb0ClkFreq(void)
{
    uint32_t freq = 0U;

    switch(SYSCON->USB0CLKSEL)
    {
        case 0U:
          freq = CLOCK_GetFroHfFreq();
          break;
        case 1U:
          freq = CLOCK_GetPllOutFreq();
          break;
        case 2U:
          freq = CLOCK_GetUsbPllOutFreq();
          break;
        case 7U:
          freq = 0U;
          break;

        default:
          break;
    }

    return freq / ((SYSCON->USB0CLKDIV & 0xffU) + 1U);
}

/* Get USB1 Clk */
uint32_t CLOCK_GetUsb1ClkFreq(void)
{
    uint32_t freq = 0U;

    switch(SYSCON->USB1CLKSEL)
    {
        case 0U:
          freq = CLOCK_GetCoreSysClkFreq();
          break;
        case 1U:
          freq = CLOCK_GetPllOutFreq();
          break;
        case 2U:
          freq = CLOCK_GetUsbPllOutFreq();
          break;
        case 7U:
          freq = 0U;
          break;

        default:
          break;
    }

    return freq / ((SYSCON->USB1CLKDIV & 0xffU) + 1U);
}

/* Get MCLK Clk */
uint32_t CLOCK_GetMclkClkFreq(void)
{
    uint32_t freq = 0U;

    switch(SYSCON->MCLKCLKSEL)
    {
        case 0U:
          freq = CLOCK_GetFroHfFreq() / ((SYSCON->FROHFCLKDIV & 0xffu) + 1U);
          break;
        case 1U:
          freq = CLOCK_GetAudioPllOutFreq();
          break;
        case 7U:
          freq = 0U;
          break;

        default:
          break;
    }

    return freq / ((SYSCON->MCLKDIV & 0xffU) + 1U);
}

/* Get SCTIMER Clk */
uint32_t CLOCK_GetSctClkFreq(void)
{
    uint32_t freq = 0U;

    switch(SYSCON->SCTCLKSEL)
    {
        case 0U:
          freq = CLOCK_GetCoreSysClkFreq();
          break;
        case 1U:
          freq = CLOCK_GetPllOutFreq();
          break;
        case 2U:
          freq = CLOCK_GetFroHfFreq();
          break;
        case 3U:
          freq = CLOCK_GetAudioPllOutFreq();
          break;
        case 7U:
          freq = 0U;
          break;

        default:
          break;
    }

    return freq / ((SYSCON->SCTCLKDIV & 0xffU) + 1U);
}

/* Get SDIO Clk */
uint32_t CLOCK_GetSdioClkFreq(void)
{
    uint32_t freq = 0U;

    switch(SYSCON->SDIOCLKSEL)
    {
        case 0U:
          freq = CLOCK_GetCoreSysClkFreq();
          break;
        case 1U:
          freq = CLOCK_GetPllOutFreq();
          break;
        case 2U:
          freq = CLOCK_GetUsbPllOutFreq();
          break;
        case 3U:
          freq = CLOCK_GetFroHfFreq();
          break;
        case 4U:
          freq = CLOCK_GetAudioPllOutFreq();
          break;
        case 7U:
          freq = 0U;
          break;
        default:
          break;
    }

    return freq / ((SYSCON->SDIOCLKDIV & 0xffU) + 1U);
}

/* Get LCD Clk */
uint32_t CLOCK_GetLcdClkFreq(void)
{
    uint32_t freq = 0U;

    switch(SYSCON->LCDCLKSEL)
    {
        case 0U:
          freq = CLOCK_GetCoreSysClkFreq();
          break;
        case 1U:
          freq = CLOCK_GetLcdClkIn();
          break;
        case 2U:
          freq = CLOCK_GetFroHfFreq();
          break;
        case 3U:
          freq = 0U;
          break;

        default:
          break;
    }

    return freq / ((SYSCON->LCDCLKDIV & 0xffU) + 1U);
}

/* Get LCD CLK IN Clk */
uint32_t CLOCK_GetLcdClkIn(void)
{
  return s_Lcd_Clk_In_Freq;
}

/* Get FRO 12M Clk */
uint32_t CLOCK_GetFro12MFreq(void)
{
    return (SYSCON->PDRUNCFG[0] & SYSCON_PDRUNCFG_PDEN_FRO_MASK) ? 0U : 12000000U;
}

/* Get EXT OSC Clk */
uint32_t CLOCK_GetExtClkFreq(void)
{
    return s_Ext_Clk_Freq;
}

/* Get WATCH DOG Clk */
uint32_t CLOCK_GetWdtOscFreq(void)
{
    uint8_t freq_sel, div_sel;
    if (SYSCON->PDRUNCFG[0] & SYSCON_PDRUNCFG_PDEN_WDT_OSC_MASK)
    {
        return 0U;
    }
    else
    {
        div_sel = ((SYSCON->WDTOSCCTRL & 0x1f) + 1) << 1;
        freq_sel = wdtFreqLookup[((SYSCON->WDTOSCCTRL & SYSCON_WDTOSCCTRL_FREQSEL_MASK) >> SYSCON_WDTOSCCTRL_FREQSEL_SHIFT)];
        return ((uint32_t) freq_sel * 50000U)/((uint32_t)div_sel);
    }
}

/* Get HF FRO Clk */
uint32_t CLOCK_GetFroHfFreq(void)
{
    if ((SYSCON->PDRUNCFG[0] & SYSCON_PDRUNCFG_PDEN_FRO_MASK) || (!(SYSCON->FROCTRL & SYSCON_FROCTRL_HSPDCLK_MASK)))
    {
        return 0U;
    }

    if(SYSCON->FROCTRL & SYSCON_FROCTRL_SEL_MASK)
    {
        return 96000000U;
    }
    else
    {
        return 48000000U;
    }
}

/* Get SYSTEM PLL Clk */
uint32_t CLOCK_GetPllOutFreq(void)
{
    return s_Pll_Freq;
}

/* Get AUDIO PLL Clk */
uint32_t CLOCK_GetAudioPllOutFreq(void)
{
    return s_Audio_Pll_Freq;
}

/* Get USB PLL Clk */
uint32_t CLOCK_GetUsbPllOutFreq(void)
{
    return s_Usb_Pll_Freq;
}

/* Get RTC OSC Clk */
uint32_t CLOCK_GetOsc32KFreq(void)
{
    return CLK_RTC_32K_CLK;               /* Needs to be corrected to check that RTC Clock is enabled */
}

/* Get MAIN Clk */
uint32_t CLOCK_GetCoreSysClkFreq(void)
{
    uint32_t freq = 0U;

    switch(SYSCON->MAINCLKSELB)
    {
        case 0U:
            if(SYSCON->MAINCLKSELA == 0U)
            {
                freq = CLOCK_GetFro12MFreq();
            }
            else if(SYSCON->MAINCLKSELA == 1U)
            {
                freq = CLOCK_GetExtClkFreq();
            }
            else if(SYSCON->MAINCLKSELA == 2U)
            {
                freq = CLOCK_GetWdtOscFreq();
            }
            else if(SYSCON->MAINCLKSELA == 3U)
            {
                freq = CLOCK_GetFroHfFreq();
            }
            else
            {}
            break;
        case 2U:
            freq = CLOCK_GetPllOutFreq();
            break;

        case 3U:
            freq = CLOCK_GetOsc32KFreq();
            break;

        default:
            break;

    }

    return freq;
}

/* Get I2S MCLK Clk */
uint32_t CLOCK_GetI2SMClkFreq(void)
{
    return s_I2S_Mclk_Freq;
}

/* Get ASYNC APB Clk */
uint32_t CLOCK_GetAsyncApbClkFreq(void)
{
    async_clock_src_t clkSrc;
    uint32_t clkRate;

    clkSrc = CLOCK_GetAsyncApbClkSrc();

    switch (clkSrc)
    {
        case kCLOCK_AsyncMainClk:
            clkRate = CLOCK_GetCoreSysClkFreq();
            break;
        case kCLOCK_AsyncFro12Mhz:
            clkRate = CLK_FRO_12MHZ;
            break;
        default:
            clkRate = 0U;
            break;
    }

    return clkRate;
}

/* Get FLEXCOMM Clk */
uint32_t CLOCK_GetFlexCommClkFreq(uint32_t id)
{
    uint32_t freq = 0U;

    switch(SYSCON->FCLKSEL[id])
    {
        case 0U:
          freq = CLOCK_GetFro12MFreq();
          break;
        case 1U:
          freq = CLOCK_GetFroHfFreq();
          break;
        case 2U:
          freq = CLOCK_GetPllOutFreq();
          break;
        case 3U:
          freq = CLOCK_GetI2SMClkFreq();
          break;
        case 4U:
          freq = CLOCK_GetFrgClkFreq();
          break;

        default:
          break;
    }

    return freq;
}

/* Get FRG Clk */
uint32_t CLOCK_GetFRGInputClock(void)
{
    uint32_t freq = 0U;

    switch(SYSCON->FRGCLKSEL)
    {
        case 0U:
          freq = CLOCK_GetCoreSysClkFreq();
          break;
        case 1U:
          freq = CLOCK_GetPllOutFreq();
          break;
        case 2U:
          freq = CLOCK_GetFro12MFreq();
          break;
        case 3U:
          freq = CLOCK_GetFroHfFreq();
          break;

        default:
          break;
    }

    return freq;
}

/* Get FRG Clk */
uint32_t CLOCK_GetFrgClkFreq(void)
{
    uint32_t freq = 0U;

    switch (SYSCON->FRGCLKSEL)
    {
        case 0U:
            freq = CLOCK_GetCoreSysClkFreq();
            break;
        case 1U:
            freq = CLOCK_GetPllOutFreq();
            break;
        case 2U:
            freq = CLOCK_GetFro12MFreq();
            break;
        case 3U:
            freq = CLOCK_GetFroHfFreq();
            break;
        default:
            break;
    }

    return freq;
}

/* Get DMIC Clk */
uint32_t CLOCK_GetDmicClkFreq(void)
{
    uint32_t freq = 0U;

    switch (SYSCON->DMICCLKSEL)
    {
        case 0U:
            freq = CLOCK_GetFro12MFreq();
            break;
        case 1U:
            freq = CLOCK_GetFroHfFreq();
            break;
        case 2U:
            freq = CLOCK_GetPllOutFreq();
            break;
        case 3U:
            freq = CLOCK_GetI2SMClkFreq();
            break;
        case 4U:
            freq = CLOCK_GetCoreSysClkFreq();
            break;
        case 5U:
            freq = CLOCK_GetWdtOscFreq();
            break;
        default:
            break;
    }

    return freq / ((SYSCON->DMICCLKDIV & 0xffU) + 1U);
}

/* Set FRG Clk */
uint32_t CLOCK_SetFRGClock(uint32_t freq)
{
    assert(freq);

    uint32_t input = CLOCK_GetFRGInputClock();
    uint32_t mul;

    if ((freq > 48000000) || (freq > input) || (input / freq >= 2))
    {
        /* FRG output frequency should be less than equal to 48MHz */
        return 0;
    }
    else
    {
        mul = ((uint64_t)(input - freq) * 256) / ((uint64_t)freq);
        SYSCON->FRGCTRL = (mul << SYSCON_FRGCTRL_MULT_SHIFT) | SYSCON_FRGCTRL_DIV_MASK;
        return 1;
    }
}

/* Set IP Clk */
uint32_t CLOCK_GetFreq(clock_name_t clockName)
{
    uint32_t freq;
    switch (clockName)
    {
        case kCLOCK_CoreSysClk:
            freq = CLOCK_GetCoreSysClkFreq();
            break;
        case kCLOCK_BusClk:
            freq = CLOCK_GetCoreSysClkFreq() / ((SYSCON->AHBCLKDIV & 0xffU) + 1U);
            break;
        case kCLOCK_ClockOut:
            freq = CLOCK_GetClockOutClkFreq();
            break;
        case kCLOCK_SpiFi:
            freq = CLOCK_GetSpifiClkFreq();
            break;
        case kCLOCK_Adc:
            freq = CLOCK_GetAdcClkFreq();
            break;
        case kCLOCK_Usb0:
            freq = CLOCK_GetUsb0ClkFreq();
            break;
        case kCLOCK_Usb1:
            freq = CLOCK_GetUsb1ClkFreq();
            break;
        case kCLOCK_Mclk:
            freq = CLOCK_GetMclkClkFreq();
            break;
        case kCLOCK_FroHf:
            freq = CLOCK_GetFroHfFreq();
            break;
        case kCLOCK_Fro12M:
            freq = CLOCK_GetFro12MFreq();
            break;
        case kCLOCK_ExtClk:
            freq = CLOCK_GetExtClkFreq();
            break;
        case kCLOCK_PllOut:
            freq = CLOCK_GetPllOutFreq();
            break;
        case kCLOCK_WdtOsc:
            freq = CLOCK_GetWdtOscFreq();
            break;
        case kCLOCK_Frg:
            freq = CLOCK_GetFrgClkFreq();
            break;
        case kCLOCK_Dmic:
            freq = CLOCK_GetDmicClkFreq();
            break;
        case kCLOCK_AsyncApbClk:
            freq = CLOCK_GetAsyncApbClkFreq();
            break;
        case kCLOCK_Sct:
            freq = CLOCK_GetSctClkFreq();
            break;
        case kCLOCK_SDio:
            freq = CLOCK_GetSdioClkFreq();
            break;
        case kCLOCK_EMC:
            freq = CLOCK_GetCoreSysClkFreq() / ((SYSCON->AHBCLKDIV & 0xffU) + 1U) / ((SYSCON->EMCCLKDIV & 0xffU) + 1U);
            break;
        case kCLOCK_LCD:
            freq = CLOCK_GetLcdClkFreq();
            break;
        case kCLOCK_MCAN0:
            freq = CLOCK_GetCoreSysClkFreq() / ((SYSCON->CAN0CLKDIV & 0xffU) + 1U);
            break;
        case kCLOCK_MCAN1:
            freq = CLOCK_GetCoreSysClkFreq() / ((SYSCON->CAN1CLKDIV & 0xffU) + 1U);
            break;
        case kCLOCK_FlexI2S:
            freq = CLOCK_GetI2SMClkFreq();
            break;
        case kCLOCK_Flexcomm0:
            freq = CLOCK_GetFlexCommClkFreq(0U);
            break;
        case kCLOCK_Flexcomm1:
            freq = CLOCK_GetFlexCommClkFreq(1U);
            break;
        case kCLOCK_Flexcomm2:
            freq = CLOCK_GetFlexCommClkFreq(2U);
            break;
        case kCLOCK_Flexcomm3:
            freq = CLOCK_GetFlexCommClkFreq(3U);
            break;
        case kCLOCK_Flexcomm4:
            freq = CLOCK_GetFlexCommClkFreq(4U);
            break;
        case kCLOCK_Flexcomm5:
            freq = CLOCK_GetFlexCommClkFreq(5U);
            break;
        case kCLOCK_Flexcomm6:
            freq = CLOCK_GetFlexCommClkFreq(6U);
            break;
        case kCLOCK_Flexcomm7:
            freq = CLOCK_GetFlexCommClkFreq(7U);
            break;
        case kCLOCK_Flexcomm8:
            freq = CLOCK_GetFlexCommClkFreq(8U);
            break;
        case kCLOCK_Flexcomm9:
            freq = CLOCK_GetFlexCommClkFreq(9U);
            break;
        default:
            freq = 0U;
            break;
    }

    return freq;
}

/* Set the FLASH wait states for the passed frequency */
void CLOCK_SetFLASHAccessCyclesForFreq(uint32_t iFreq)
{
    if (iFreq <= 12000000U)
    {
        CLOCK_SetFLASHAccessCycles(kCLOCK_Flash1Cycle);
    }
    else if (iFreq <= 24000000U)
    {
        CLOCK_SetFLASHAccessCycles(kCLOCK_Flash2Cycle);
    }
    else if (iFreq <= 36000000U)
    {
        CLOCK_SetFLASHAccessCycles(kCLOCK_Flash3Cycle);
    }
    else if (iFreq <= 60000000U)
    {
        CLOCK_SetFLASHAccessCycles(kCLOCK_Flash4Cycle);
    }
    else if (iFreq <= 96000000U)
    {
        CLOCK_SetFLASHAccessCycles(kCLOCK_Flash5Cycle);
    }
    else if (iFreq <= 120000000U)
    {
        CLOCK_SetFLASHAccessCycles(kCLOCK_Flash6Cycle);
    }
    else if (iFreq <= 144000000U)
    {
        CLOCK_SetFLASHAccessCycles(kCLOCK_Flash7Cycle);
    }
    else if (iFreq <= 168000000U)
    {
        CLOCK_SetFLASHAccessCycles(kCLOCK_Flash8Cycle);
    }
    else
    {
        CLOCK_SetFLASHAccessCycles(kCLOCK_Flash9Cycle);
    }
}

/* Find encoded NDEC value for raw N value, max N = NVALMAX */
static uint32_t pllEncodeN(uint32_t N)
{
    uint32_t x, i;

    /* Find NDec */
    switch (N)
    {
        case 0U:
            x = 0x3FFU;
            break;

        case 1U:
            x = 0x302U;
            break;

        case 2U:
            x = 0x202U;
            break;

        default:
            x = 0x080U;
            for (i = N; i <= NVALMAX; i++)
            {
                x = (((x ^ (x >> 2U) ^ (x >> 3U) ^ (x >> 4U)) & 1U) << 7U) | ((x >> 1U) & 0x7FU);
            }
            break;
    }

    return x & (PLL_NDEC_VAL_M >> PLL_NDEC_VAL_P);
}

/* Find decoded N value for raw NDEC value */
static uint32_t pllDecodeN(uint32_t NDEC)
{
    uint32_t n, x, i;

    /* Find NDec */
    switch (NDEC)
    {
        case 0x3FFU:
            n = 0U;
            break;

        case 0x302U:
            n = 1U;
            break;

        case 0x202U:
            n = 2U;
            break;

        default:
            x = 0x080U;
            n = 0xFFFFFFFFU;
            for (i = NVALMAX; ((i >= 3U) && (n == 0xFFFFFFFFU)); i--)
            {
                x = (((x ^ (x >> 2U) ^ (x >> 3U) ^ (x >> 4U)) & 1U) << 7U) | ((x >> 1U) & 0x7FU);
                if ((x & (PLL_NDEC_VAL_M >> PLL_NDEC_VAL_P)) == NDEC)
                {
                    /* Decoded value of NDEC */
                    n = i;
                }
            }
            break;
    }

    return n;
}

/* Find encoded PDEC value for raw P value, max P = PVALMAX */
static uint32_t pllEncodeP(uint32_t P)
{
    uint32_t x, i;

    /* Find PDec */
    switch (P)
    {
        case 0U:
            x = 0x7FU;
            break;

        case 1U:
            x = 0x62U;
            break;

        case 2U:
            x = 0x42U;
            break;

        default:
            x = 0x10U;
            for (i = P; i <= PVALMAX; i++)
            {
                x = (((x ^ (x >> 2U)) & 1U) << 4U) | ((x >> 1U) & 0xFU);
            }
            break;
    }

    return x & (PLL_PDEC_VAL_M >> PLL_PDEC_VAL_P);
}

/* Find decoded P value for raw PDEC value */
static uint32_t pllDecodeP(uint32_t PDEC)
{
    uint32_t p, x, i;

    /* Find PDec */
    switch (PDEC)
    {
        case 0x7FU:
            p = 0U;
            break;

        case 0x62U:
            p = 1U;
            break;

        case 0x42U:
            p = 2U;
            break;

        default:
            x = 0x10U;
            p = 0xFFFFFFFFU;
            for (i = PVALMAX; ((i >= 3U) && (p == 0xFFFFFFFFU)); i--)
            {
                x = (((x ^ (x >> 2U)) & 1U) << 4U) | ((x >> 1U) & 0xFU);
                if ((x & (PLL_PDEC_VAL_M >> PLL_PDEC_VAL_P)) == PDEC)
                {
                    /* Decoded value of PDEC */
                    p = i;
                }
            }
            break;
    }

    return p;
}

/* Find encoded MDEC value for raw M value, max M = MVALMAX */
static uint32_t pllEncodeM(uint32_t M)
{
    uint32_t i, x;

    /* Find MDec */
    switch (M)
    {
        case 0U:
            x = 0x1FFFFU;
            break;

        case 1U:
            x = 0x18003U;
            break;

        case 2U:
            x = 0x10003U;
            break;

        default:
            x = 0x04000U;
            for (i = M; i <= MVALMAX; i++)
            {
                x = (((x ^ (x >> 1U)) & 1U) << 14U) | ((x >> 1U) & 0x3FFFU);
            }
            break;
    }

    return x & (PLL_MDEC_VAL_M >> PLL_MDEC_VAL_P);
}

/* Find decoded M value for raw MDEC value */
static uint32_t pllDecodeM(uint32_t MDEC)
{
    uint32_t m, i, x;

    /* Find MDec */
    switch (MDEC)
    {
        case 0x1FFFFU:
            m = 0U;
            break;

        case 0x18003U:
            m = 1U;
            break;

        case 0x10003U:
            m = 2U;
            break;

        default:
            x = 0x04000U;
            m = 0xFFFFFFFFU;
            for (i = MVALMAX; ((i >= 3U) && (m == 0xFFFFFFFFU)); i--)
            {
                x = (((x ^ (x >> 1U)) & 1) << 14U) | ((x >> 1U) & 0x3FFFU);
                if ((x & (PLL_MDEC_VAL_M >> PLL_MDEC_VAL_P)) == MDEC)
                {
                    /* Decoded value of MDEC */
                    m = i;
                }
            }
            break;
    }

    return m;
}

/* Find SELP, SELI, and SELR values for raw M value, max M = MVALMAX */
static void pllFindSel(uint32_t M, uint32_t *pSelP, uint32_t *pSelI, uint32_t *pSelR)
{
    /* bandwidth: compute selP from Multiplier */
    if (M < 60U)
    {
        *pSelP = (M >> 1U) + 1U;
    }
    else
    {
        *pSelP = PVALMAX - 1U;
    }

    /* bandwidth: compute selI from Multiplier */
    if (M > 16384U)
    {
        *pSelI = 1U;
    }
    else if (M > 8192U)
    {
        *pSelI = 2U;
    }
    else if (M > 2048U)
    {
        *pSelI = 4U;
    }
    else if (M >= 501U)
    {
        *pSelI = 8U;
    }
    else if (M >= 60U)
    {
        *pSelI = 4U * (1024U / (M + 9U));
    }
    else
    {
        *pSelI = (M & 0x3CU) + 4U;
    }

    if (*pSelI > ((0x3FUL << SYSCON_SYSPLLCTRL_SELI_SHIFT) >> SYSCON_SYSPLLCTRL_SELI_SHIFT))
    {
        *pSelI = ((0x3FUL << SYSCON_SYSPLLCTRL_SELI_SHIFT) >> SYSCON_SYSPLLCTRL_SELI_SHIFT);
    }

    *pSelR = 0U;
}

/* Get predivider (N) from PLL NDEC setting */
static uint32_t findPllPreDiv(uint32_t ctrlReg, uint32_t nDecReg)
{
    uint32_t preDiv = 1;

    /* Direct input is not used? */
    if ((ctrlReg & (1UL << SYSCON_SYSPLLCTRL_DIRECTI_SHIFT)) == 0U)
    {
        /* Decode NDEC value to get (N) pre divider */
        preDiv = pllDecodeN(nDecReg & 0x3FFU);
        if (preDiv == 0U)
        {
            preDiv = 1U;
        }
    }

    /* Adjusted by 1, directi is used to bypass */
    return preDiv;
}

/* Get postdivider (P) from PLL PDEC setting */
static uint32_t findPllPostDiv(uint32_t ctrlReg, uint32_t pDecReg)
{
    uint32_t postDiv = 1U;

    /* Direct input is not used? */
    if ((ctrlReg & SYSCON_SYSPLLCTRL_DIRECTO_MASK) == 0U)
    {
        /* Decode PDEC value to get (P) post divider */
        postDiv = 2U * pllDecodeP(pDecReg & 0x7FU);
        if (postDiv == 0U)
        {
            postDiv = 2U;
        }
    }

    /* Adjusted by 1, directo is used to bypass */
    return postDiv;
}

/* Get multiplier (M) from PLL MDEC and BYPASS_FBDIV2 settings */
static uint32_t findPllMMult(uint32_t ctrlReg, uint32_t mDecReg)
{
    uint32_t mMult = 1U;

    /* Decode MDEC value to get (M) multiplier */
    mMult = pllDecodeM(mDecReg & 0x1FFFFU);

    if (mMult == 0U)
    {
        mMult = 1U;
    }

    return mMult;
}

/* Convert the binary to fractional part */
static double Binary2Fractional(uint32_t binaryPart)
{
    double fractional = 0;
    for (uint32_t i = 0; i <= 14; i++)
    {
        fractional += (double)((binaryPart >> i) & 0x1U) / (double)(1 << (15 - i));
    }
    return fractional;
}

/* Find greatest common divisor between m and n */
static uint32_t FindGreatestCommonDivisor(uint32_t m, uint32_t n)
{
    uint32_t tmp;

    while (n != 0U)
    {
        tmp = n;
        n = m % n;
        m = tmp;
    }

    return m;
}

/*
 * Set PLL output based on desired output rate.
 * In this function, the it calculates the PLL setting for output frequency from input clock
 * frequency. The calculation would cost a few time. So it is not recommaned to use it frequently.
 * the "pllctrl", "pllndec", "pllpdec", "pllmdec" would updated in this function.
 */
static pll_error_t CLOCK_GetPllConfigInternal(
    uint32_t finHz, uint32_t foutHz, pll_setup_t *pSetup)
{
    uint32_t nDivOutHz, fccoHz, multFccoDiv;
    uint32_t pllPreDivider, pllMultiplier, pllPostDivider;
    uint32_t pllDirectInput, pllDirectOutput;
    uint32_t pllSelP, pllSelI, pllSelR, uplimoff;

    /* Baseline parameters (no input or output dividers) */
    pllPreDivider = 1U;  /* 1 implies pre-divider will be disabled */
    pllPostDivider = 0U; /* 0 implies post-divider will be disabled */
    pllDirectOutput = 1U;
    multFccoDiv = 2U;

    /* Verify output rate parameter */
    if (foutHz > PLL_MAX_CCO_FREQ_MHZ)
    {
        /* Maximum PLL output with post divider=1 cannot go above this frequency */
        return kStatus_PLL_OutputTooHigh;
    }
    if (foutHz < (PLL_MIN_CCO_FREQ_MHZ / (PVALMAX << 1U)))
    {
        /* Minmum PLL output with maximum post divider cannot go below this frequency */
        return kStatus_PLL_OutputTooLow;
    }

    /* Verify input rate parameter */
    if (finHz < PLL_LOWER_IN_LIMIT)
    {
        /* Input clock into the PLL cannot be lower than this */
        return kStatus_PLL_InputTooLow;
    }

    /* Find the optimal CCO frequency for the output and input that
       will keep it inside the PLL CCO range. This may require
       tweaking the post-divider for the PLL. */
    fccoHz = foutHz;
    while (fccoHz < PLL_MIN_CCO_FREQ_MHZ)
    {
        /* CCO output is less than minimum CCO range, so the CCO output
           needs to be bumped up and the post-divider is used to bring
           the PLL output back down. */
        pllPostDivider++;
        if (pllPostDivider > PVALMAX)
        {
            return kStatus_PLL_OutsideIntLimit;
        }

        /* Target CCO goes up, PLL output goes down */
        fccoHz = foutHz * (pllPostDivider * 2U);
        pllDirectOutput = 0U;
    }

    /* Determine if a pre-divider is needed to get the best frequency */
    if ((finHz > PLL_LOWER_IN_LIMIT) && (fccoHz >= finHz))
    {
        uint32_t a = FindGreatestCommonDivisor(fccoHz, (multFccoDiv * finHz));

        if (a > 20000U)
        {
            a = (multFccoDiv * finHz) / a;
            if ((a != 0U) && (a < PLL_MAX_N_DIV))
            {
                pllPreDivider = a;
            }
        }
    }

    /* Bypass pre-divider hardware if pre-divider is 1 */
    if (pllPreDivider > 1U)
    {
        pllDirectInput = 0U;
    }
    else
    {
        pllDirectInput = 1U;
    }

    /* Determine PLL multipler */
    nDivOutHz = (finHz / pllPreDivider);
    pllMultiplier = (fccoHz / nDivOutHz) / multFccoDiv;

    /* Find optimal values for filter */
    /* Will bumping up M by 1 get us closer to the desired CCO frequency? */
    if ((nDivOutHz * ((multFccoDiv * pllMultiplier * 2U) + 1U)) < (fccoHz * 2U))
    {
        pllMultiplier++;
    }

    /* Setup filtering */
    pllFindSel(pllMultiplier, &pllSelP, &pllSelI, &pllSelR);
    uplimoff = 0U;

    /* Get encoded value for M (mult) and use manual filter, disable SS mode */
    pSetup->pllmdec =
        PLL_MDEC_VAL_SET(pllEncodeM(pllMultiplier)) ;

    /* Get encoded values for N (prediv) and P (postdiv) */
    pSetup->pllndec = PLL_NDEC_VAL_SET(pllEncodeN(pllPreDivider));
    pSetup->pllpdec = PLL_PDEC_VAL_SET(pllEncodeP(pllPostDivider));

    /* PLL control */
    pSetup->pllctrl = (pllSelR << SYSCON_SYSPLLCTRL_SELR_SHIFT) |                  /* Filter coefficient */
                         (pllSelI << SYSCON_SYSPLLCTRL_SELI_SHIFT) |                  /* Filter coefficient */
                         (pllSelP << SYSCON_SYSPLLCTRL_SELP_SHIFT) |                  /* Filter coefficient */
                         (0 << SYSCON_SYSPLLCTRL_BYPASS_SHIFT) |                      /* PLL bypass mode disabled */
                         (uplimoff << SYSCON_SYSPLLCTRL_UPLIMOFF_SHIFT) |             /* SS/fractional mode disabled */
                         (pllDirectInput << SYSCON_SYSPLLCTRL_DIRECTI_SHIFT) | /* Bypass pre-divider? */
                         (pllDirectOutput << SYSCON_SYSPLLCTRL_DIRECTO_SHIFT); /* Bypass post-divider? */

    return kStatus_PLL_Success;
}

#if (defined(CLOCK_USR_CFG_PLL_CONFIG_CACHE_COUNT) && CLOCK_USR_CFG_PLL_CONFIG_CACHE_COUNT)
/* Alloct the static buffer for cache. */
static pll_setup_t s_PllSetupCacheStruct[CLOCK_USR_CFG_PLL_CONFIG_CACHE_COUNT];
static uint32_t s_FinHzCache[CLOCK_USR_CFG_PLL_CONFIG_CACHE_COUNT] = {0};
static uint32_t s_FoutHzCache[CLOCK_USR_CFG_PLL_CONFIG_CACHE_COUNT] = {0};
static uint32_t s_PllSetupCacheIdx = 0U;
#endif /* CLOCK_USR_CFG_PLL_CONFIG_CACHE_COUNT */

/*
 * Calculate the PLL setting values from input clock freq to output freq.
 */
static pll_error_t CLOCK_GetPllConfig(
    uint32_t finHz, uint32_t foutHz, pll_setup_t *pSetup)
{
    pll_error_t retErr;
#if (defined(CLOCK_USR_CFG_PLL_CONFIG_CACHE_COUNT) && CLOCK_USR_CFG_PLL_CONFIG_CACHE_COUNT)
    uint32_t i;

    for (i = 0U; i < CLOCK_USR_CFG_PLL_CONFIG_CACHE_COUNT; i++)
    {
        if ( (finHz == s_FinHzCache[i]) && (foutHz == s_FoutHzCache[i]) )
        {
            /* Hit the target in cache buffer. */
            pSetup->pllctrl = s_PllSetupCacheStruct[i].pllctrl;
            pSetup->pllndec = s_PllSetupCacheStruct[i].pllndec;
            pSetup->pllpdec = s_PllSetupCacheStruct[i].pllpdec;
            pSetup->pllmdec = s_PllSetupCacheStruct[i].pllmdec;
            retErr = kStatus_PLL_Success;
        }
    }
#endif /* CLOCK_USR_CFG_PLL_CONFIG_CACHE_COUNT */

    /* No cache or did not hit the cache. */
    retErr = CLOCK_GetPllConfigInternal(finHz, foutHz, pSetup);

#if (defined(CLOCK_USR_CFG_PLL_CONFIG_CACHE_COUNT) && CLOCK_USR_CFG_PLL_CONFIG_CACHE_COUNT)
    if (kStatus_PLL_Success == retErr)
    {
        /* Cache the most recent calulation result into buffer. */
        s_FinHzCache[s_PllSetupCacheIdx] = finHz;
        s_FoutHzCache[s_PllSetupCacheIdx] = foutHz;
    
        s_PllSetupCacheStruct[s_PllSetupCacheIdx].pllctrl = pSetup->pllctrl;
        s_PllSetupCacheStruct[s_PllSetupCacheIdx].pllndec = pSetup->pllndec;
        s_PllSetupCacheStruct[s_PllSetupCacheIdx].pllpdec = pSetup->pllpdec;
        s_PllSetupCacheStruct[s_PllSetupCacheIdx].pllmdec = pSetup->pllmdec;
        /* Update the index for next available buffer. */
        s_PllSetupCacheIdx = (s_PllSetupCacheIdx + 1U) % CLOCK_USR_CFG_PLL_CONFIG_CACHE_COUNT;
    }
#endif /* CLOCK_USR_CFG_PLL_CONFIG_CACHE_COUNT */

    return retErr;
}

/* Update SYSTEM PLL rate variable */
static void CLOCK_GetSystemPLLOutFromSetupUpdate(pll_setup_t *pSetup)
{
    s_Pll_Freq = CLOCK_GetSystemPLLOutFromSetup(pSetup);
}

/* Update AUDIO PLL rate variable */
static void CLOCK_GetAudioPLLOutFromSetupUpdate(pll_setup_t *pSetup)
{
    s_Audio_Pll_Freq = CLOCK_GetAudioPLLOutFromSetup(pSetup);
}

/* Update AUDIO Fractional PLL rate variable */
static void CLOCK_GetAudioPLLOutFromAudioFracSetupUpdate(pll_setup_t *pSetup)
{
    s_Audio_Pll_Freq = CLOCK_GetAudioPLLOutFromFractSetup(pSetup);
}

/* Update USB PLL rate variable */
static void CLOCK_GetUsbPLLOutFromSetupUpdate(const usb_pll_setup_t *pSetup)
{
    s_Usb_Pll_Freq = CLOCK_GetUsbPLLOutFromSetup(pSetup);
}

/* Return System PLL input clock rate */
uint32_t CLOCK_GetSystemPLLInClockRate(void)
{
    uint32_t clkRate = 0U;

    switch ((SYSCON->SYSPLLCLKSEL & SYSCON_SYSPLLCLKSEL_SEL_MASK))
    {
        case 0x00U:
            clkRate = CLK_FRO_12MHZ;
            break;

        case 0x01U:
            clkRate = CLOCK_GetExtClkFreq();
            break;

        case 0x02U:
            clkRate = CLOCK_GetWdtOscFreq();
            break;

        case 0x03U:
            clkRate = CLOCK_GetOsc32KFreq();
            break;

        default:
            clkRate = 0U;
            break;
    }

    return clkRate;
}

/* Return Audio PLL input clock rate */
uint32_t CLOCK_GetAudioPLLInClockRate(void)
{
    uint32_t clkRate = 0U;

    switch ((SYSCON->AUDPLLCLKSEL & SYSCON_AUDPLLCLKSEL_SEL_MASK))
    {
        case 0x00U:
            clkRate = CLK_FRO_12MHZ;
            break;

        case 0x01U:
            clkRate = CLOCK_GetExtClkFreq();
            break;
            
        default:
            clkRate = 0U;
            break;
    }

    return clkRate;
}

/* Return System PLL output clock rate from setup structure */
uint32_t CLOCK_GetSystemPLLOutFromSetup(pll_setup_t *pSetup)
{
    uint32_t prediv, postdiv, mMult, inPllRate;
    uint64_t workRate;

    inPllRate = CLOCK_GetSystemPLLInClockRate();
    /* If the PLL is bypassed, PLL would not be used and the output of PLL module would just be the input clock*/
    if ((pSetup->pllctrl & (SYSCON_SYSPLLCTRL_BYPASS_MASK)) == 0U)
    {
        /* PLL is not in bypass mode, get pre-divider, and M divider, post-divider. */
        /*
         * 1. Pre-divider
         * Pre-divider is only available when the DIRECTI is disabled.
         */
        if (0U == (pSetup->pllctrl & SYSCON_SYSPLLCTRL_DIRECTI_MASK))
        {
            prediv = findPllPreDiv(pSetup->pllctrl, pSetup->pllndec);
        }
        else
        {
            prediv = 1U; /* The pre-divider is bypassed. */
        }
        /*
         * 2. Post-divider
         * Post-divider is only available when the DIRECTO is disabled.
         */
        if (0U == (pSetup->pllctrl & SYSCON_SYSPLLCTRL_DIRECTO_MASK))
        {
            postdiv = findPllPostDiv(pSetup->pllctrl, pSetup->pllpdec);
        }
        else
        {
            postdiv = 1U;           /* The post-divider is bypassed. */
        }
        /* Adjust input clock */
        inPllRate = inPllRate / prediv;

        /* MDEC used for rate */
        mMult = findPllMMult(pSetup->pllctrl, pSetup->pllmdec);
        workRate = (uint64_t)inPllRate * (uint64_t)mMult;

        workRate = workRate / ((uint64_t)postdiv);
        workRate = workRate * 2U; /* SYS PLL hardware cco is divide by 2 before to M-DIVIDER*/

    }
    else
    {
        /* In bypass mode */
        workRate = (uint64_t)inPllRate;
    }

    return (uint32_t)workRate;
}

/* Return Usb PLL output clock rate from setup structure */
uint32_t CLOCK_GetUsbPLLOutFromSetup(const usb_pll_setup_t *pSetup)
{
    uint32_t nsel, psel, msel, inPllRate;
    uint64_t workRate;
    inPllRate = CLOCK_GetExtClkFreq();
    msel = pSetup->msel;
    psel = pSetup->psel;
    nsel = pSetup->nsel;

    if (pSetup->fbsel == 1U)
       {   
           /*integer_mode: Fout = M*(Fin/N),  Fcco = 2*P*M*(Fin/N) */
           workRate = (uint64_t)(inPllRate) * (msel + 1U) / (nsel + 1U);
       }
       else
       {
           /* non integer_mode: Fout = M*(Fin/N)/(2*P), Fcco = M * (Fin/N) */
           workRate = (uint64_t)(inPllRate / (nsel + 1U)) * (msel + 1U) / (2U * (1U << (psel & 3U)));
       }
   
    return (uint32_t)workRate;
}

/* Return Audio PLL output clock rate from setup structure */
uint32_t CLOCK_GetAudioPLLOutFromSetup(pll_setup_t *pSetup)
{
    uint32_t prediv, postdiv, mMult, inPllRate;
    uint64_t workRate;

    inPllRate = CLOCK_GetAudioPLLInClockRate();
    if ((pSetup->pllctrl & (1UL << SYSCON_SYSPLLCTRL_BYPASS_SHIFT)) == 0U)
    {
        /* PLL is not in bypass mode, get pre-divider, and M divider, post-divider. */
        /*
         * 1. Pre-divider
         * Pre-divider is only available when the DIRECTI is disabled.
         */
        if (0U == (pSetup->pllctrl & SYSCON_AUDPLLCTRL_DIRECTI_MASK))
        {
            prediv = findPllPreDiv(pSetup->pllctrl, pSetup->pllndec);
        }
        else
        {
            prediv = 1U; /* The pre-divider is bypassed. */
        }
        /*
         * 2. Post-divider
         * Post-divider is only available when the DIRECTO is disabled.
         */
        if (0U == (pSetup->pllctrl & SYSCON_AUDPLLCTRL_DIRECTO_MASK))
        {
            postdiv = findPllPostDiv(pSetup->pllctrl, pSetup->pllpdec);
        }
        else
        {
            postdiv = 1U;           /* The post-divider is bypassed. */
        }
        /* Adjust input clock */
        inPllRate = inPllRate / prediv;

        /* MDEC used for rate */
        mMult = findPllMMult(pSetup->pllctrl, pSetup->pllmdec);
        workRate = (uint64_t)inPllRate * (uint64_t)mMult;

        workRate = workRate / ((uint64_t)postdiv);
        workRate = workRate * 2U; /* SYS PLL hardware cco is divide by 2 before to M-DIVIDER*/
    }
    else
    {
        /* In bypass mode */
        workRate = (uint64_t)inPllRate;
    }

    return (uint32_t)workRate;
}

/* Return Audio PLL output clock rate from audio fractioanl setup structure */
uint32_t CLOCK_GetAudioPLLOutFromFractSetup(pll_setup_t *pSetup)
{
    uint32_t prediv, postdiv, inPllRate;
    double workRate, mMultFactional;

    inPllRate = CLOCK_GetAudioPLLInClockRate();
    if ((pSetup->pllctrl & (1UL << SYSCON_SYSPLLCTRL_BYPASS_SHIFT)) == 0U)
    {
        /* PLL is not in bypass mode, get pre-divider, and M divider, post-divider. */
        /*
         * 1. Pre-divider
         * Pre-divider is only available when the DIRECTI is disabled.
         */
        if (0U == (pSetup->pllctrl & SYSCON_AUDPLLCTRL_DIRECTI_MASK))
        {
            prediv = findPllPreDiv(pSetup->pllctrl, pSetup->pllndec);
        }
        else
        {
            prediv = 1U; /* The pre-divider is bypassed. */
        }
        /*
         * 2. Post-divider
         * Post-divider is only available when the DIRECTO is disabled.
         */
        if (0U == (pSetup->pllctrl & SYSCON_AUDPLLCTRL_DIRECTO_MASK))
        {
            postdiv = findPllPostDiv(pSetup->pllctrl, pSetup->pllpdec);
        }
        else
        {
            postdiv = 1U;           /* The post-divider is bypassed. */
        }
        /* Adjust input clock */
        inPllRate = inPllRate / prediv;

        mMultFactional = (double)(pSetup->audpllfrac >> 15) + (double)Binary2Fractional(pSetup->audpllfrac & 0x7FFFU);
        workRate = (double)inPllRate * (double)mMultFactional;

        workRate = workRate / ((double)postdiv);
        workRate = workRate * 2U; /* SYS PLL hardware cco is divide by 2 before to M-DIVIDER*/
    }
    else
    {
        /* In bypass mode */
        workRate = (uint64_t)inPllRate;
    }

    return (uint32_t)workRate;
}

/* Set the current PLL Rate */
void CLOCK_SetStoredPLLClockRate(uint32_t rate)
{
    s_Pll_Freq = rate;
}

/* Set the current Audio PLL Rate */
void CLOCK_SetStoredAudioPLLClockRate(uint32_t rate)
{
    s_Audio_Pll_Freq = rate;
}

/* Set the current Usb PLL Rate */
void CLOCK_SetStoredUsbPLLClockRate(uint32_t rate)
{
    s_Usb_Pll_Freq = rate;
}

/* Return System PLL output clock rate */
uint32_t CLOCK_GetSystemPLLOutClockRate(bool recompute)
{
    pll_setup_t Setup;
    uint32_t rate;

    if ((recompute) || (s_Pll_Freq == 0U))
    {
        Setup.pllctrl = SYSCON->SYSPLLCTRL;
        Setup.pllndec = SYSCON->SYSPLLNDEC;
        Setup.pllpdec = SYSCON->SYSPLLPDEC;
        Setup.pllmdec = SYSCON->SYSPLLMDEC;

        CLOCK_GetSystemPLLOutFromSetupUpdate(&Setup);
    }

    rate = s_Pll_Freq;

    return rate;
}

/* Return AUDIO PLL output clock rate */
uint32_t CLOCK_GetAudioPLLOutClockRate(bool recompute)
{
    pll_setup_t Setup;
    uint32_t rate;

    if ((recompute) || (s_Audio_Pll_Freq == 0U))
    {
        Setup.pllctrl = SYSCON->AUDPLLCTRL;
        Setup.pllndec = SYSCON->AUDPLLNDEC;
        Setup.pllpdec = SYSCON->AUDPLLPDEC;
        Setup.pllmdec = SYSCON->AUDPLLMDEC;

        CLOCK_GetAudioPLLOutFromSetupUpdate(&Setup);
    }

    rate = s_Audio_Pll_Freq;
    return rate;
}

/* Return USB PLL output clock rate */
uint32_t CLOCK_GetUsbPLLOutClockRate(bool recompute)
{
    usb_pll_setup_t Setup;
    uint32_t rate;

    if ((recompute) || (s_Usb_Pll_Freq == 0U))
    {
        Setup.msel = (SYSCON->USBPLLCTRL >> SYSCON_USBPLLCTRL_MSEL_SHIFT) & SYSCON_USBPLLCTRL_MSEL_MASK;
        Setup.psel = (SYSCON->USBPLLCTRL >> SYSCON_USBPLLCTRL_PSEL_SHIFT) & SYSCON_USBPLLCTRL_PSEL_MASK;
        Setup.nsel = (SYSCON->USBPLLCTRL >> SYSCON_USBPLLCTRL_NSEL_SHIFT) & SYSCON_USBPLLCTRL_NSEL_MASK;
        Setup.fbsel = (SYSCON->USBPLLCTRL >> SYSCON_USBPLLCTRL_FBSEL_SHIFT) & SYSCON_USBPLLCTRL_FBSEL_MASK;
        Setup.bypass = (SYSCON->USBPLLCTRL >> SYSCON_USBPLLCTRL_BYPASS_SHIFT) & SYSCON_USBPLLCTRL_BYPASS_MASK;
        Setup.direct = (SYSCON->USBPLLCTRL >> SYSCON_USBPLLCTRL_DIRECT_SHIFT) & SYSCON_USBPLLCTRL_DIRECT_MASK; 
        CLOCK_GetUsbPLLOutFromSetupUpdate(&Setup);
    }

    rate = s_Usb_Pll_Freq;
    return rate;
}

/* Set PLL output based on the passed PLL setup data */
pll_error_t CLOCK_SetupPLLData(pll_config_t *pControl, pll_setup_t *pSetup)
{
    uint32_t inRate;
    pll_error_t pllError;

    /* Determine input rate for the PLL */
    if ((pControl->flags & PLL_CONFIGFLAG_USEINRATE) != 0U)
    {
        inRate = pControl->inputRate;
    }
    else
    {
        inRate = CLOCK_GetSystemPLLInClockRate();
    }

    /* PLL flag options */
    pllError = CLOCK_GetPllConfig(inRate, pControl->desiredRate, pSetup);
    pSetup->pllRate = pControl->desiredRate;
    return pllError;
}

/* Set PLL output from PLL setup structure */
pll_error_t CLOCK_SetupSystemPLLPrec(pll_setup_t *pSetup, uint32_t flagcfg)
{
    if ((SYSCON->SYSPLLCLKSEL & SYSCON_SYSPLLCLKSEL_SEL_MASK) == 0x01U)
    {
       /* Turn on the ext clock if system pll input select clk_in */
       CLOCK_Enable_SysOsc(true);
    }
    /* Enable power for PLLs */
    POWER_SetPLL();
    /* Power off PLL during setup changes */
    POWER_EnablePD(kPDRUNCFG_PD_SYS_PLL0);
    /*!< Set FLASH waitstates for core */
    CLOCK_SetFLASHAccessCyclesForFreq(pSetup->pllRate);
    pSetup->flags = flagcfg;

    /* Write PLL setup data */
    SYSCON->SYSPLLCTRL = pSetup->pllctrl;
    SYSCON->SYSPLLNDEC = pSetup->pllndec;
    SYSCON->SYSPLLNDEC = pSetup->pllndec | (1U << SYSCON_SYSPLLNDEC_NREQ_SHIFT); /* latch */
    SYSCON->SYSPLLPDEC = pSetup->pllpdec;
    SYSCON->SYSPLLPDEC = pSetup->pllpdec | (1U << SYSCON_SYSPLLPDEC_PREQ_SHIFT); /* latch */
    SYSCON->SYSPLLMDEC = pSetup->pllmdec;
    SYSCON->SYSPLLMDEC = pSetup->pllmdec | (1U << SYSCON_SYSPLLMDEC_MREQ_SHIFT); /* latch */

    /* Flags for lock or power on */
    if ((pSetup->flags & (PLL_SETUPFLAG_POWERUP | PLL_SETUPFLAG_WAITLOCK)) != 0U)
    {
        /* If turning the PLL back on, perform the following sequence to accelerate PLL lock */
        volatile uint32_t delayX;
        uint32_t maxCCO = (1U << 18U) | 0x5dd2U; /* CCO = 1.6Ghz + MDEC enabled*/
        uint32_t curSSCTRL = SYSCON->SYSPLLMDEC & ~(1U << 17U);

        /* Initialize  and power up PLL */
        SYSCON->SYSPLLMDEC = maxCCO;
        POWER_DisablePD(kPDRUNCFG_PD_SYS_PLL0);

        /* Set mreq to activate */
        SYSCON->SYSPLLMDEC = maxCCO | (1U << 17U);

        /* Delay for 72 uSec @ 12Mhz */
        for (delayX = 0U; delayX < 172U; ++delayX)
        {
        }

        /* clear mreq to prepare for restoring mreq */
        SYSCON->SYSPLLMDEC = curSSCTRL;

        /* set original value back and activate */
        SYSCON->SYSPLLMDEC = curSSCTRL | (1U << 17U);

        /* Enable peripheral states by setting low */
        POWER_DisablePD(kPDRUNCFG_PD_SYS_PLL0);
    }
    if ((pSetup->flags & PLL_SETUPFLAG_WAITLOCK) != 0U)
    {
        while (CLOCK_IsSystemPLLLocked() == false)
        {
        }
    }

    /* Update current programmed PLL rate var */
    CLOCK_GetSystemPLLOutFromSetupUpdate(pSetup);

    /* System voltage adjustment, occurs prior to setting main system clock */
    if ((pSetup->flags & PLL_SETUPFLAG_ADGVOLT) != 0U)
    {
        POWER_SetVoltageForFreq(s_Pll_Freq);
    }

    return kStatus_PLL_Success;
}


/* Set AUDIO PLL output from AUDIO PLL setup structure */
pll_error_t CLOCK_SetupAudioPLLPrec(pll_setup_t *pSetup, uint32_t flagcfg)
{
    if ((SYSCON->AUDPLLCLKSEL & SYSCON_AUDPLLCLKSEL_SEL_MASK) == 0x01U)
    {
       /* Turn on the ext clock if system pll input select clk_in */
       CLOCK_Enable_SysOsc(true);
    }
    /* Enable power VD3 for PLLs */
    POWER_SetPLL();
    /* Power off PLL during setup changes */
    POWER_EnablePD(kPDRUNCFG_PD_AUDIO_PLL);

    pSetup->flags = flagcfg;

    /* Write PLL setup data */
    SYSCON->AUDPLLCTRL = pSetup->pllctrl;
    SYSCON->AUDPLLNDEC = pSetup->pllndec;
    SYSCON->AUDPLLNDEC = pSetup->pllndec | (1U << SYSCON_SYSPLLNDEC_NREQ_SHIFT); /* latch */
    SYSCON->AUDPLLPDEC = pSetup->pllpdec;
    SYSCON->AUDPLLPDEC = pSetup->pllpdec | (1U << SYSCON_SYSPLLPDEC_PREQ_SHIFT); /* latch */
    SYSCON->AUDPLLMDEC = pSetup->pllmdec;
    SYSCON->AUDPLLMDEC = pSetup->pllmdec | (1U << SYSCON_SYSPLLMDEC_MREQ_SHIFT); /* latch */
    SYSCON->AUDPLLFRAC = SYSCON_AUDPLLFRAC_SEL_EXT(1); /* disable fractional function */

    /* Flags for lock or power on */
    if ((pSetup->flags & (PLL_SETUPFLAG_POWERUP | PLL_SETUPFLAG_WAITLOCK)) != 0U)
    {
        /* If turning the PLL back on, perform the following sequence to accelerate PLL lock */
        volatile uint32_t delayX;
        uint32_t maxCCO = (1U << 18U) | 0x5dd2U; /* CCO = 1.6Ghz + MDEC enabled*/
        uint32_t curSSCTRL = SYSCON->AUDPLLMDEC & ~(1U << 17U);

        /* Initialize  and power up PLL */
        SYSCON->AUDPLLMDEC = maxCCO;
        POWER_DisablePD(kPDRUNCFG_PD_AUDIO_PLL);

        /* Set mreq to activate */
        SYSCON->AUDPLLMDEC = maxCCO | (1U << 17U);

        /* Delay for 72 uSec @ 12Mhz */
        for (delayX = 0U; delayX < 172U; ++delayX)
        {
        }

        /* clear mreq to prepare for restoring mreq */
        SYSCON->AUDPLLMDEC = curSSCTRL;

        /* set original value back and activate */
        SYSCON->AUDPLLMDEC = curSSCTRL | (1U << 17U);

        /* Enable peripheral states by setting low */
        POWER_DisablePD(kPDRUNCFG_PD_AUDIO_PLL);
    }
    if ((pSetup->flags & PLL_SETUPFLAG_WAITLOCK) != 0U)
    {
        while (CLOCK_IsAudioPLLLocked() == false)
        {
        }
    }

    /* Update current programmed PLL rate var */
    CLOCK_GetAudioPLLOutFromSetupUpdate(pSetup);

    return kStatus_PLL_Success;
}

/* Set AUDIO PLL output from AUDIO PLL fractional setup structure */
pll_error_t CLOCK_SetupAudioPLLPrecFract(pll_setup_t *pSetup, uint32_t flagcfg)
{
    if ((SYSCON->AUDPLLCLKSEL & SYSCON_AUDPLLCLKSEL_SEL_MASK) == 0x01U)
    {
       /* Turn on the ext clock if system pll input select clk_in */
       CLOCK_Enable_SysOsc(true);
    }
    /* Enable power VD3 for PLLs */
    POWER_SetPLL();
    /* Power off PLL during setup changes */
    POWER_EnablePD(kPDRUNCFG_PD_AUDIO_PLL);

    pSetup->flags = flagcfg;

    /* Write PLL setup data */
    SYSCON->AUDPLLCTRL = pSetup->pllctrl;
    SYSCON->AUDPLLNDEC = pSetup->pllndec;
    SYSCON->AUDPLLNDEC = pSetup->pllndec | (1U << SYSCON_SYSPLLNDEC_NREQ_SHIFT); /* latch */
    SYSCON->AUDPLLPDEC = pSetup->pllpdec;
    SYSCON->AUDPLLPDEC = pSetup->pllpdec | (1U << SYSCON_SYSPLLPDEC_PREQ_SHIFT); /* latch */
    SYSCON->AUDPLLMDEC = pSetup->pllmdec;
    SYSCON->AUDPLLFRAC = SYSCON_AUDPLLFRAC_SEL_EXT(0); /* enable fractional function */
    SYSCON->AUDPLLFRAC = pSetup->audpllfrac;
    SYSCON->AUDPLLFRAC = pSetup->audpllfrac | (1U << SYSCON_AUDPLLFRAC_REQ_SHIFT);

    /* Enable peripheral states by setting low */
    POWER_DisablePD(kPDRUNCFG_PD_AUDIO_PLL);

    if ((pSetup->flags & PLL_SETUPFLAG_WAITLOCK) != 0U)
    {
        while (CLOCK_IsAudioPLLLocked() == false)
        {
        }
    }

    /* Update current programmed PLL rate var */
    CLOCK_GetAudioPLLOutFromAudioFracSetupUpdate(pSetup);

    return kStatus_PLL_Success;
}

/* Set Audio PLL output based on the passed Audio PLL setup data */
pll_error_t CLOCK_SetupAudioPLLData(pll_config_t *pControl, pll_setup_t *pSetup)
{
    uint32_t inRate;
    pll_error_t pllError;

    /* Determine input rate for the PLL */
    if ((pControl->flags & PLL_CONFIGFLAG_USEINRATE) != 0U)
    {
        inRate = pControl->inputRate;
    }
    else
    {
        inRate = CLOCK_GetAudioPLLInClockRate();
    }

    /* PLL flag options */
    pllError = CLOCK_GetPllConfig(inRate, pControl->desiredRate, pSetup);
    pSetup->pllRate = pControl->desiredRate;
    return pllError;
}



/* Setup PLL Frequency from pre-calculated value */
pll_error_t CLOCK_SetPLLFreq(const pll_setup_t *pSetup)
{
    if ((SYSCON->SYSPLLCLKSEL & SYSCON_SYSPLLCLKSEL_SEL_MASK) == 0x01U)
    {
       /* Turn on the ext clock if system pll input select clk_in */
       CLOCK_Enable_SysOsc(true);
    }
    /* Enable power VD3 for PLLs */
    POWER_SetPLL();
    /* Power off PLL during setup changes */
    POWER_EnablePD(kPDRUNCFG_PD_SYS_PLL0);

    /* Write PLL setup data */
    SYSCON->SYSPLLCTRL = pSetup->pllctrl;
    SYSCON->SYSPLLNDEC = pSetup->pllndec;
    SYSCON->SYSPLLNDEC = pSetup->pllndec | (1U << SYSCON_SYSPLLNDEC_NREQ_SHIFT); /* latch */
    SYSCON->SYSPLLPDEC = pSetup->pllpdec;
    SYSCON->SYSPLLPDEC = pSetup->pllpdec | (1U << SYSCON_SYSPLLPDEC_PREQ_SHIFT); /* latch */
    SYSCON->SYSPLLMDEC = pSetup->pllmdec;
    SYSCON->SYSPLLMDEC = pSetup->pllmdec | (1U << SYSCON_SYSPLLMDEC_MREQ_SHIFT); /* latch */

    /* Flags for lock or power on */
    if ((pSetup->flags & (PLL_SETUPFLAG_POWERUP | PLL_SETUPFLAG_WAITLOCK)) != 0)
    {
        /* If turning the PLL back on, perform the following sequence to accelerate PLL lock */
        volatile uint32_t delayX;
        uint32_t maxCCO = (1U << 18U) | 0x5dd2U; /* CCO = 1.6Ghz + MDEC enabled*/
        uint32_t curSSCTRL = SYSCON->SYSPLLMDEC & ~(1U << 17U);

        /* Initialize  and power up PLL */
        SYSCON->SYSPLLMDEC = maxCCO;
        POWER_DisablePD(kPDRUNCFG_PD_SYS_PLL0);

        /* Set mreq to activate */
        SYSCON->SYSPLLMDEC = maxCCO | (1U << 17U);

        /* Delay for 72 uSec @ 12Mhz */
        for (delayX = 0U; delayX < 172U; ++delayX)
        {
        }

        /* clear mreq to prepare for restoring mreq */
        SYSCON->SYSPLLMDEC = curSSCTRL;

        /* set original value back and activate */
        SYSCON->SYSPLLMDEC = curSSCTRL | (1U << 17U);

        /* Enable peripheral states by setting low */
        POWER_DisablePD(kPDRUNCFG_PD_SYS_PLL0);
    }
    if ((pSetup->flags & PLL_SETUPFLAG_WAITLOCK) != 0U)
    {
        while (CLOCK_IsSystemPLLLocked() == false)
        {
        }
    }

    /* Update current programmed PLL rate var */
    s_Pll_Freq = pSetup->pllRate;

    return kStatus_PLL_Success;
}

/* Setup Audio PLL Frequency from pre-calculated value */
pll_error_t CLOCK_SetAudioPLLFreq(const pll_setup_t *pSetup)
{
    if ((SYSCON->AUDPLLCLKSEL & SYSCON_AUDPLLCLKSEL_SEL_MASK) == 0x01U)
    {
       /* Turn on the ext clock if system pll input select clk_in */
       CLOCK_Enable_SysOsc(true);
    }
    /* Enable power VD3 for PLLs */
    POWER_SetPLL();
    /* Power off Audio PLL during setup changes */
    POWER_EnablePD(kPDRUNCFG_PD_AUDIO_PLL);

    /* Write Audio PLL setup data */
    SYSCON->AUDPLLCTRL = pSetup->pllctrl;
    SYSCON->AUDPLLFRAC = pSetup->audpllfrac;
    SYSCON->AUDPLLFRAC = pSetup->audpllfrac | (1U << SYSCON_AUDPLLFRAC_REQ_SHIFT);  /* latch */
    SYSCON->AUDPLLNDEC = pSetup->pllndec;
    SYSCON->AUDPLLNDEC = pSetup->pllndec | (1U << SYSCON_AUDPLLNDEC_NREQ_SHIFT);    /* latch */
    SYSCON->AUDPLLPDEC = pSetup->pllpdec;
    SYSCON->AUDPLLPDEC = pSetup->pllpdec | (1U << SYSCON_AUDPLLPDEC_PREQ_SHIFT);    /* latch */
    SYSCON->AUDPLLMDEC = pSetup->pllmdec;
    SYSCON->AUDPLLMDEC = pSetup->pllmdec | (1U << SYSCON_AUDPLLMDEC_MREQ_SHIFT);    /* latch */
    SYSCON->AUDPLLFRAC = SYSCON_AUDPLLFRAC_SEL_EXT(1);                              /* disable fractional function */

    /* Flags for lock or power on */
    if ((pSetup->flags & (PLL_SETUPFLAG_POWERUP | PLL_SETUPFLAG_WAITLOCK)) != 0)
    {
        /* If turning the PLL back on, perform the following sequence to accelerate PLL lock */
        volatile uint32_t delayX;
        uint32_t maxCCO = (1U << 18U) | 0x5dd2U; /* CCO = 1.6Ghz + MDEC enabled*/
        uint32_t curSSCTRL = SYSCON->SYSPLLMDEC & ~(1U << 17U);

        /* Initialize  and power up PLL */
        SYSCON->SYSPLLMDEC = maxCCO;
        POWER_DisablePD(kPDRUNCFG_PD_AUDIO_PLL);

        /* Set mreq to activate */
        SYSCON->SYSPLLMDEC = maxCCO | (1U << 17U);

        /* Delay for 72 uSec @ 12Mhz */
        for (delayX = 0U; delayX < 172U; ++delayX)
        {
        }

        /* clear mreq to prepare for restoring mreq */
        SYSCON->SYSPLLMDEC = curSSCTRL;

        /* set original value back and activate */
        SYSCON->SYSPLLMDEC = curSSCTRL | (1U << 17U);

        /* Enable peripheral states by setting low */
        POWER_DisablePD(kPDRUNCFG_PD_AUDIO_PLL);
    }
    if ((pSetup->flags & PLL_SETUPFLAG_WAITLOCK) != 0U)
    {
        while (CLOCK_IsAudioPLLLocked() == false)
        {
        }
    }

    /* Update current programmed PLL rate var */
    s_Audio_Pll_Freq = pSetup->pllRate;

    return kStatus_PLL_Success;
}

/* Setup USB PLL Frequency from pre-calculated value */
pll_error_t CLOCK_SetUsbPLLFreq(const usb_pll_setup_t *pSetup)
{       
    uint32_t usbpllctrl, fccoHz;
    uint8_t msel, psel, nsel;
    bool pllDirectInput, pllDirectOutput, pllfbsel;
    volatile uint32_t delayX;

    msel = pSetup->msel;
    psel = pSetup->psel;
    nsel = pSetup->nsel;
    pllDirectOutput = pSetup->direct;
    pllDirectInput = pSetup->bypass;
    pllfbsel = pSetup->fbsel;
    
    /* Input clock into the PLL cannot be lower than this */
    if (pSetup->inputRate < USB_PLL_LOWER_IN_LIMIT )
    {
        return kStatus_PLL_InputTooLow;
    }
    
    if (pllfbsel == 1U)
    {   
        /*integer_mode: Fout = M*(Fin/N),  Fcco = 2*P*M*(Fin/N) */
        fccoHz = (pSetup->inputRate / (nsel + 1U)) * 2 * (msel + 1U) * (1U <<(psel & 3U));
        
        /* USB PLL CCO out rate cannot be lower than this */        
        if (fccoHz < USB_PLL_MIN_CCO_FREQ_MHZ)
        {       
            return kStatus_PLL_CCOTooLow;
        }
        /* USB PLL CCO out rate cannot be Higher than this */
        if (fccoHz > USB_PLL_MAX_CCO_FREQ_MHZ)
        { 
            return kStatus_PLL_CCOTooHigh;
        }
    }
    else
    {
        /* non integer_mode: Fout = M*(Fin/N)/(2*P), Fcco = M * (Fin/N) */
        fccoHz = pSetup->inputRate / (nsel + 1U) * (msel + 1U);
        
        /* USB PLL CCO out rate cannot be lower than this */        
        if (fccoHz < USB_PLL_MIN_CCO_FREQ_MHZ)
        {       
            return kStatus_PLL_CCOTooLow;
        }
        /* USB PLL CCO out rate cannot be Higher than this */
        if (fccoHz > USB_PLL_MAX_CCO_FREQ_MHZ)
        { 
            return kStatus_PLL_CCOTooHigh;
        }       
    }
    
    /* If configure the USB HOST clock, VD5 power for USB PHY should be enable 
       before the PLL is working */
    /* Turn on the ext clock for usb pll input */
    CLOCK_Enable_SysOsc(true);
    
    /* Enable power VD3 for PLLs */
    POWER_SetPLL();
    
    /* Power on the VD5 for USB PHY */    
    POWER_SetUsbPhy();

    /* Power off USB PLL during setup changes */
    POWER_EnablePD(kPDRUNCFG_PD_USB_PLL);
      
    /* Write USB PLL setup data */
    usbpllctrl = USB_PLL_NSEL_VAL_SET(nsel)  |                  /* NSEL VALUE */
                 USB_PLL_PSEL_VAL_SET(psel)  |                  /* PSEL VALUE */
                 USB_PLL_MSEL_VAL_SET(msel)  |                  /* MSEL VALUE */
                 (uint32_t)pllDirectInput << SYSCON_USBPLLCTRL_BYPASS_SHIFT  |            /* BYPASS DISABLE */
                 (uint32_t)pllDirectOutput << SYSCON_USBPLLCTRL_DIRECT_SHIFT |            /* DIRECTO DISABLE */
                 (uint32_t)pllfbsel << SYSCON_USBPLLCTRL_FBSEL_SHIFT;                     /* FBSEL SELECT */   
    
    SYSCON->USBPLLCTRL = usbpllctrl;
    
    POWER_DisablePD(kPDRUNCFG_PD_USB_PLL);
  
    /* Delay for 72 uSec @ 12Mhz for the usb pll to lock */
    for (delayX = 0U; delayX < 172U; ++delayX)
    {
    }
    if (false == pllDirectInput)
    {
      while (CLOCK_IsUsbPLLLocked() == false)
      {
      }
    }
    CLOCK_GetUsbPLLOutFromSetupUpdate(pSetup);
    return kStatus_PLL_Success;
}

/* Set System PLL clock based on the input frequency and multiplier */
void CLOCK_SetupSystemPLLMult(uint32_t multiply_by, uint32_t input_freq)
{
    uint32_t cco_freq = input_freq * multiply_by;
    uint32_t pdec = 1U;
    uint32_t selr;
    uint32_t seli;
    uint32_t selp;
    uint32_t mdec, ndec;

    uint32_t directo = SYSCON_SYSPLLCTRL_DIRECTO(1);

    while (cco_freq < 275000000U)
    {
        multiply_by <<= 1U; /* double value in each iteration */
        pdec <<= 1U;        /* correspondingly double pdec to cancel effect of double msel */
        cco_freq = input_freq * multiply_by;
    }
    selr = 0U;
    if (multiply_by < 60U)
    {
        seli = (multiply_by & 0x3cU) + 4U;
        selp = (multiply_by >> 1U) + 1U;
    }
    else
    {
        selp = 31U;
        if (multiply_by > 16384U)
        {
            seli = 1U;
        }
        else if (multiply_by > 8192U)
        {
            seli = 2U;
        }
        else if (multiply_by > 2048U)
        {
            seli = 4U;
        }
        else if (multiply_by >= 501U)
        {
            seli = 8U;
        }
        else
        {
            seli = 4U * (1024U / (multiply_by + 9U));
        }
    }

    if (pdec > 1U)
    {
        directo = 0U;     /* use post divider */
        pdec = pdec / 2U; /* Account for minus 1 encoding */
                          /* Translate P value */
        switch (pdec)
        {
            case 1U:
                pdec = 0x62U; /* 1  * 2 */
                break;
            case 2U:
                pdec = 0x42U; /* 2  * 2 */
                break;
            case 4U:
                pdec = 0x02U; /* 4  * 2 */
                break;
            case 8U:
                pdec = 0x0bU; /* 8  * 2 */
                break;
            case 16U:
                pdec = 0x11U; /* 16 * 2 */
                break;
            case 32U:
                pdec = 0x08U; /* 32 * 2 */
                break;
            default:
                pdec = 0x08U;
                break;
        }
    }

    mdec = PLL_MDEC_VAL_SET(pllEncodeM(multiply_by));
    ndec = 0x302U; /* pre divide by 1 (hardcoded) */

    SYSCON->SYSPLLCTRL = directo |
                         (selr << SYSCON_SYSPLLCTRL_SELR_SHIFT) | (seli << SYSCON_SYSPLLCTRL_SELI_SHIFT) |
                         (selp << SYSCON_SYSPLLCTRL_SELP_SHIFT);
    SYSCON->SYSPLLPDEC = pdec | (1U << 7U);  /* set Pdec value and assert preq */
    SYSCON->SYSPLLNDEC = ndec | (1U << 10U); /* set Pdec value and assert preq */
    SYSCON->SYSPLLMDEC = (1U << 17U) | mdec; /* select non sscg MDEC value, assert mreq and select mdec value */
}

/* Enable USB DEVICE FULL SPEED clock */
bool CLOCK_EnableUsbfs0DeviceClock(clock_usb_src_t src, uint32_t freq)
{
    bool ret = true;

    CLOCK_DisableClock(kCLOCK_Usbd0);

    if (kCLOCK_UsbSrcFro == src)
    {
        switch (freq)
        {
            case 96000000U:
                CLOCK_SetClkDiv(kCLOCK_DivUsb0Clk, 2, false); /*!< Div by 2 to get 48MHz, no divider reset */
                break;
            
            case 48000000U:
                CLOCK_SetClkDiv(kCLOCK_DivUsb0Clk, 1, false); /*!< Div by 1 to get 48MHz, no divider reset */
                break;
            
            default:
                ret = false;
                break;
        }
        /* Turn ON FRO HF and let it adjust TRIM value based on USB SOF */
        SYSCON->FROCTRL = (SYSCON->FROCTRL & ~((0x01U << 15U) | (0xFU << 26U))) | SYSCON_FROCTRL_HSPDCLK_MASK |
                          SYSCON_FROCTRL_USBCLKADJ_MASK;
        /* Select FRO 96 or 48 MHz */
        CLOCK_AttachClk(kFRO_HF_to_USB0_CLK);
    }
    else
    {
        /*Set the USB PLL as the Usb0 CLK*/
        POWER_DisablePD(kPDRUNCFG_PD_USB_PLL);
    
        usb_pll_setup_t pll_setup = { 0x3FU, 0x01U, 0x03U, false, false, false, 12000000U };

        CLOCK_SetUsbPLLFreq(&pll_setup);
        CLOCK_SetClkDiv(kCLOCK_DivUsb0Clk,1U, false);
        CLOCK_AttachClk(kUSB_PLL_to_USB0_CLK);
        uint32_t delay = 100000;
        while (delay --)
        {
            __ASM("nop");
        }
    }
    CLOCK_EnableClock(kCLOCK_Usbd0);
    CLOCK_EnableClock(kCLOCK_UsbRam1);
    
    return ret;
}

/* Enable USB HOST FULL SPEED clock */
bool CLOCK_EnableUsbfs0HostClock(clock_usb_src_t src, uint32_t freq)
{
    bool ret = true;

    CLOCK_DisableClock(kCLOCK_Usbhmr0);
    CLOCK_DisableClock(kCLOCK_Usbhsl0);

    if (kCLOCK_UsbSrcFro == src)
    {
        switch (freq)
        {
            case 96000000U:
                CLOCK_SetClkDiv(kCLOCK_DivUsb0Clk, 2, false); /*!< Div by 2 to get 48MHz, no divider reset */
                break;
            
            case 48000000U:
                CLOCK_SetClkDiv(kCLOCK_DivUsb0Clk, 1, false); /*!< Div by 1 to get 48MHz, no divider reset */
                break;
            
            default:
                ret = false;
                break;
        }
        /* Turn ON FRO HF and let it adjust TRIM value based on USB SOF */
        SYSCON->FROCTRL = (SYSCON->FROCTRL & ~((0x01U << 15U) | (0xFU << 26U))) | SYSCON_FROCTRL_HSPDCLK_MASK |
                          SYSCON_FROCTRL_USBCLKADJ_MASK;
        /* Select FRO 96 or 48 MHz */
        CLOCK_AttachClk(kFRO_HF_to_USB0_CLK);
    }
    else
    {
        /*Set the USB PLL as the Usb0 CLK*/
        POWER_DisablePD(kPDRUNCFG_PD_USB_PLL);
    
        usb_pll_setup_t pll_setup = { 0x3FU, 0x01U, 0x03U, false, false, false, 12000000U };

        CLOCK_SetUsbPLLFreq(&pll_setup);
        CLOCK_SetClkDiv(kCLOCK_DivUsb0Clk,1U, false);
        CLOCK_AttachClk(kUSB_PLL_to_USB0_CLK);
        uint32_t delay = 100000;
        while (delay --)
        {
            __ASM("nop");
        }
    }
    CLOCK_EnableClock(kCLOCK_Usbhmr0);
    CLOCK_EnableClock(kCLOCK_Usbhsl0);
    CLOCK_EnableClock(kCLOCK_UsbRam1); 

    return ret;
}

/* Enable USB DEVICE HIGH SPEED clock */
bool CLOCK_EnableUsbhs0DeviceClock(clock_usb_src_t src, uint32_t freq)
{
    bool ret = true;
    uint32_t delay;
    CLOCK_DisableClock(kCLOCK_Usbd1);
    /* Power on the VD5 for USB PHY */    
    POWER_SetUsbPhy();
    if (kCLOCK_UsbSrcFro == src)
    {
        switch (freq)
        {
            case 96000000U:
                CLOCK_SetClkDiv(kCLOCK_DivUsb1Clk, 2, false); /*!< Div by 2 to get 48MHz, no divider reset */
                break;
            
            case 48000000U:
                CLOCK_SetClkDiv(kCLOCK_DivUsb1Clk, 1, false); /*!< Div by 1 to get 48MHz, no divider reset */
                break;
            
            default:
                ret = false;
                break;
        }
        /* Turn ON FRO HF and let it adjust TRIM value based on USB SOF */
        SYSCON->FROCTRL = (SYSCON->FROCTRL & ~((0x01U << 15U) | (0xFU << 26U))) | SYSCON_FROCTRL_HSPDCLK_MASK |
                          SYSCON_FROCTRL_USBCLKADJ_MASK;
        /* Select FRO 96 or 48 MHz */
        CLOCK_AttachClk(kFRO_HF_to_USB1_CLK);
    }
    else
    {    
        delay = 100000;
        while (delay --)
        {
            __ASM("nop");
        }    
        usb_pll_setup_t pll_setup = { 0x3FU, 0x01U, 0x03U, false, false, false, 12000000U };
      
        CLOCK_SetUsbPLLFreq(&pll_setup);
        
        /* Select USB PLL output as USB clock src */
        CLOCK_SetClkDiv(kCLOCK_DivUsb1Clk,1U, false);
        CLOCK_AttachClk(kUSB_PLL_to_USB1_CLK);  
    }

    delay = 100000;
    while (delay --)
    {
        __ASM("nop");
    }
    /* Enable USB1D and USB1RAM */
    CLOCK_EnableClock(kCLOCK_Usbd1);
    CLOCK_EnableClock(kCLOCK_UsbRam1); 
    POWER_DisablePD(kPDRUNCFG_PD_USB1_PHY); /* Turn on power for USB PHY */
    return ret;
}


/* Enable USB HOST HIGH SPEED clock */
bool CLOCK_EnableUsbhs0HostClock(clock_usb_src_t src, uint32_t freq)
{
    bool ret = true;
    uint32_t delay;
    CLOCK_DisableClock(kCLOCK_Usbh1);
    /* Power on the VD5 for USB PHY */    
    POWER_SetUsbPhy();
    if (kCLOCK_UsbSrcFro == src)
    {
        switch (freq)
        {
            case 96000000U:
                CLOCK_SetClkDiv(kCLOCK_DivUsb1Clk, 2, false); /*!< Div by 2 to get 48MHz, no divider reset */
                break;
            
            case 48000000U:
                CLOCK_SetClkDiv(kCLOCK_DivUsb1Clk, 1, false); /*!< Div by 1 to get 48MHz, no divider reset */
                break;
            
            default:
                ret = false;
                break;
        }
        /* Turn ON FRO HF and let it adjust TRIM value based on USB SOF */
        SYSCON->FROCTRL = (SYSCON->FROCTRL & ~((0x01U << 15U) | (0xFU << 26U))) | SYSCON_FROCTRL_HSPDCLK_MASK |
                          SYSCON_FROCTRL_USBCLKADJ_MASK;
        /* Select FRO 96 or 48 MHz */
        CLOCK_AttachClk(kFRO_HF_to_USB1_CLK);
    }
    else
    {
        delay = 100000;
        while (delay --)
        {
            __ASM("nop");
        }    
        usb_pll_setup_t pll_setup = { 0x3FU, 0x01U, 0x03U, false, false, false, 12000000U };

        CLOCK_SetUsbPLLFreq(&pll_setup);
        
        /* Select USB PLL output as USB clock src */
        CLOCK_SetClkDiv(kCLOCK_DivUsb1Clk,1U, false);
        CLOCK_AttachClk(kUSB_PLL_to_USB1_CLK);
    }

    delay = 100000;
    while (delay --)
    {
        __ASM("nop");
    }
    /* Enable USBh1 and USB1RAM */
    CLOCK_EnableClock(kCLOCK_Usbh1);
    CLOCK_EnableClock(kCLOCK_UsbRam1); 
    POWER_DisablePD(kPDRUNCFG_PD_USB1_PHY); /* Turn on power for USB PHY */
    return ret;
}
