/*
 * The Clear BSD License
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
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

#include "fsl_clock.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.clock"
#endif

#define SCG_SIRC_LOW_RANGE_FREQ 2000000U  /* Slow IRC low range clock frequency. */
#define SCG_SIRC_HIGH_RANGE_FREQ 8000000U /* Slow IRC high range clock frequency.   */

#define SCG_FIRC_FREQ0 48000000U /* Fast IRC trimed clock frequency(48MHz). */
#define SCG_FIRC_FREQ1 52000000U /* Fast IRC trimed clock frequency(52MHz). */
#define SCG_FIRC_FREQ2 56000000U /* Fast IRC trimed clock frequency(56MHz). */
#define SCG_FIRC_FREQ3 60000000U /* Fast IRC trimed clock frequency(60MHz). */

/*
 * System PLL base divider value, it is the PLL reference clock divider
 * value when SCG_SPLLCFG[PREDIV]=0.
 */
#define SCG_SPLL_PREDIV_BASE_VALUE 1U

/*
 * System PLL base multiplier value, it is the PLL multiplier value
 * when SCG_SPLLCFG[MULT]=0.
 */
#define SCG_SPLL_MULT_BASE_VALUE 16U

#define SCG_SPLL_PREDIV_MAX_VALUE 7U /* Max value of SCG_SPLLCFG[PREDIV]. */
#define SCG_SPLL_MULT_MAX_VALUE 31U  /* Max value of SCG_SPLLCFG[MULT].   */

/*
 * System PLL reference clock after SCG_SPLLCFG[PREDIV] should be in
 * the range of SCG_SPLL_REF_MIN to SCG_SPLL_REF_MAX.
 */
#define SCG_SPLL_REF_MIN 8000000U
#define SCG_SPLL_REF_MAX 32000000U

#define SCG_CSR_SCS_VAL ((SCG->CSR & SCG_CSR_SCS_MASK) >> SCG_CSR_SCS_SHIFT)
#define SCG_SOSCDIV_SOSCDIV1_VAL ((SCG->SOSCDIV & SCG_SOSCDIV_SOSCDIV1_MASK) >> SCG_SOSCDIV_SOSCDIV1_SHIFT)
#define SCG_SOSCDIV_SOSCDIV2_VAL ((SCG->SOSCDIV & SCG_SOSCDIV_SOSCDIV2_MASK) >> SCG_SOSCDIV_SOSCDIV2_SHIFT)
#define SCG_SOSCDIV_SOSCDIV3_VAL ((SCG->SOSCDIV & SCG_SOSCDIV_SOSCDIV3_MASK) >> SCG_SOSCDIV_SOSCDIV3_SHIFT)
#define SCG_SIRCDIV_SIRCDIV1_VAL ((SCG->SIRCDIV & SCG_SIRCDIV_SIRCDIV1_MASK) >> SCG_SIRCDIV_SIRCDIV1_SHIFT)
#define SCG_SIRCDIV_SIRCDIV2_VAL ((SCG->SIRCDIV & SCG_SIRCDIV_SIRCDIV2_MASK) >> SCG_SIRCDIV_SIRCDIV2_SHIFT)
#define SCG_SIRCDIV_SIRCDIV3_VAL ((SCG->SIRCDIV & SCG_SIRCDIV_SIRCDIV3_MASK) >> SCG_SIRCDIV_SIRCDIV3_SHIFT)
#define SCG_FIRCDIV_FIRCDIV1_VAL ((SCG->FIRCDIV & SCG_FIRCDIV_FIRCDIV1_MASK) >> SCG_FIRCDIV_FIRCDIV1_SHIFT)
#define SCG_FIRCDIV_FIRCDIV2_VAL ((SCG->FIRCDIV & SCG_FIRCDIV_FIRCDIV2_MASK) >> SCG_FIRCDIV_FIRCDIV2_SHIFT)
#define SCG_FIRCDIV_FIRCDIV3_VAL ((SCG->FIRCDIV & SCG_FIRCDIV_FIRCDIV3_MASK) >> SCG_FIRCDIV_FIRCDIV3_SHIFT)

#define SCG_SPLLDIV_SPLLDIV1_VAL ((SCG->SPLLDIV & SCG_SPLLDIV_SPLLDIV1_MASK) >> SCG_SPLLDIV_SPLLDIV1_SHIFT)
#define SCG_SPLLDIV_SPLLDIV2_VAL ((SCG->SPLLDIV & SCG_SPLLDIV_SPLLDIV2_MASK) >> SCG_SPLLDIV_SPLLDIV2_SHIFT)
#define SCG_SPLLDIV_SPLLDIV3_VAL ((SCG->SPLLDIV & SCG_SPLLDIV_SPLLDIV3_MASK) >> SCG_SPLLDIV_SPLLDIV3_SHIFT)

#define SCG_SIRCCFG_RANGE_VAL ((SCG->SIRCCFG & SCG_SIRCCFG_RANGE_MASK) >> SCG_SIRCCFG_RANGE_SHIFT)
#define SCG_FIRCCFG_RANGE_VAL ((SCG->FIRCCFG & SCG_FIRCCFG_RANGE_MASK) >> SCG_FIRCCFG_RANGE_SHIFT)

#define SCG_SPLLCFG_PREDIV_VAL ((SCG->SPLLCFG & SCG_SPLLCFG_PREDIV_MASK) >> SCG_SPLLCFG_PREDIV_SHIFT)
#define SCG_SPLLCFG_MULT_VAL ((SCG->SPLLCFG & SCG_SPLLCFG_MULT_MASK) >> SCG_SPLLCFG_MULT_SHIFT)

#define PCC_PCS_VAL(reg) (((reg) & PCC_CLKCFG_PCS_MASK) >> PCC_CLKCFG_PCS_SHIFT)
#define PCC_FRAC_VAL(reg) (((reg) & PCC_CLKCFG_FRAC_MASK) >> PCC_CLKCFG_FRAC_SHIFT)
#define PCC_PCD_VAL(reg) (((reg) & PCC_CLKCFG_PCD_MASK) >> PCC_CLKCFG_PCD_SHIFT)

/*******************************************************************************
 * Variables
 ******************************************************************************/

/* External XTAL0 (OSC0) clock frequency. */
volatile uint32_t g_xtal0Freq;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*!
 * @brief Get the common System PLL frequency for both RAW SPLL output and SPLL PFD output.
 *
 * The "raw" SPLL output is the clkout divided by postdiv1-2 of SAPLL.
 * The "common" System PLL frequency is the common part for both RAW SPLL and SPLL PFD output.
 * That is the frequency calculated based on the clock source which passes through POSTDIV & MULT.
 * "Common" SPLL Frequency = Divided Reference Frequency * MULT
 *
 * @return  Clock frequency; If the clock is invalid, returns 0.
 */
static uint32_t CLOCK_GetSysPllCommonFreq(void);

/*******************************************************************************
 * Code
 ******************************************************************************/

uint32_t CLOCK_GetErClkFreq(void)
{
    if (SCG->SOSCCSR & SCG_SOSCCSR_SOSCEN_MASK)
    {
        /* Please call CLOCK_SetXtal0Freq base on board setting before using OSC0 clock. */
        assert(g_xtal0Freq);
        return g_xtal0Freq;
    }
    else
    {
        return 0U;
    }
}

uint32_t CLOCK_GetOsc32kClkFreq(void)
{
    return (CLOCK_GetErClkFreq() == 32768U) ? 32768U : 0U;
}

uint32_t CLOCK_GetFlashClkFreq(void)
{
    return CLOCK_GetSysClkFreq(kSCG_SysClkSlow);
}

uint32_t CLOCK_GetBusClkFreq(void)
{
    return CLOCK_GetSysClkFreq(kSCG_SysClkSlow);
}

uint32_t CLOCK_GetPlatClkFreq(void)
{
    return CLOCK_GetSysClkFreq(kSCG_SysClkCore);
}

uint32_t CLOCK_GetCoreSysClkFreq(void)
{
    return CLOCK_GetSysClkFreq(kSCG_SysClkCore);
}

uint32_t CLOCK_GetFreq(clock_name_t clockName)
{
    uint32_t freq;

    switch (clockName)
    {
        case kCLOCK_CoreSysClk:
        case kCLOCK_PlatClk:
            freq = CLOCK_GetSysClkFreq(kSCG_SysClkCore);
            break;
        case kCLOCK_BusClk:
        case kCLOCK_FlashClk:
            freq = CLOCK_GetSysClkFreq(kSCG_SysClkSlow);
            break;
        case kCLOCK_ScgSysOscClk:
            freq = CLOCK_GetSysOscFreq();
            break;
        case kCLOCK_ScgSircClk:
            freq = CLOCK_GetSircFreq();
            break;
        case kCLOCK_ScgFircClk:
            freq = CLOCK_GetFircFreq();
            break;
        case kCLOCK_ScgSysPllClk:
            freq = CLOCK_GetSysPllFreq();
            break;
        case kCLOCK_ScgSysOscAsyncDiv1Clk:
            freq = CLOCK_GetSysOscAsyncFreq(kSCG_AsyncDiv1Clk);
            break;
        case kCLOCK_ScgSysOscAsyncDiv2Clk:
            freq = CLOCK_GetSysOscAsyncFreq(kSCG_AsyncDiv2Clk);
            break;
        case kCLOCK_ScgSysOscAsyncDiv3Clk:
            freq = CLOCK_GetSysOscAsyncFreq(kSCG_AsyncDiv3Clk);
            break;
        case kCLOCK_ScgSircAsyncDiv1Clk:
            freq = CLOCK_GetSircAsyncFreq(kSCG_AsyncDiv1Clk);
            break;
        case kCLOCK_ScgSircAsyncDiv2Clk:
            freq = CLOCK_GetSircAsyncFreq(kSCG_AsyncDiv2Clk);
            break;
        case kCLOCK_ScgSircAsyncDiv3Clk:
            freq = CLOCK_GetSircAsyncFreq(kSCG_AsyncDiv3Clk);
            break;
        case kCLOCK_ScgFircAsyncDiv1Clk:
            freq = CLOCK_GetFircAsyncFreq(kSCG_AsyncDiv1Clk);
            break;
        case kCLOCK_ScgFircAsyncDiv2Clk:
            freq = CLOCK_GetFircAsyncFreq(kSCG_AsyncDiv2Clk);
            break;
        case kCLOCK_ScgFircAsyncDiv3Clk:
            freq = CLOCK_GetFircAsyncFreq(kSCG_AsyncDiv3Clk);
            break;
        case kCLOCK_ScgSysPllAsyncDiv1Clk:
            freq = CLOCK_GetSysPllAsyncFreq(kSCG_AsyncDiv1Clk);
            break;
        case kCLOCK_ScgSysPllAsyncDiv2Clk:
            freq = CLOCK_GetSysPllAsyncFreq(kSCG_AsyncDiv2Clk);
            break;
        case kCLOCK_ScgSysPllAsyncDiv3Clk:
            freq = CLOCK_GetSysPllAsyncFreq(kSCG_AsyncDiv3Clk);
            break;
        case kCLOCK_LpoClk:
            freq = LPO_CLK_FREQ;
            break;
        case kCLOCK_Osc32kClk:
            freq = (CLOCK_GetErClkFreq() == 32768U) ? 32768U : 0U;
            break;
        case kCLOCK_ErClk:
            freq = CLOCK_GetErClkFreq();
            break;
        default:
            freq = 0U;
            break;
    }
    return freq;
}

uint32_t CLOCK_GetIpFreq(clock_ip_name_t name)
{
    uint32_t reg = (*(volatile uint32_t *)name);

    scg_async_clk_t asycClk;
    uint32_t freq;

    assert(reg & PCC_CLKCFG_PR_MASK);

    /* USB uses SCG DIV1 clock, others use SCG DIV3 clock. */
    if (kCLOCK_Usbfs0 == name)
    {
        asycClk = kSCG_AsyncDiv1Clk;
    }
    else
    {
        asycClk = kSCG_AsyncDiv3Clk;
    }

    switch (PCC_PCS_VAL(reg))
    {
        case kCLOCK_IpSrcSysOscAsync:
            freq = CLOCK_GetSysOscAsyncFreq(asycClk);
            break;
        case kCLOCK_IpSrcSircAsync:
            freq = CLOCK_GetSircAsyncFreq(asycClk);
            break;
        case kCLOCK_IpSrcFircAsync:
            freq = CLOCK_GetFircAsyncFreq(asycClk);
            break;
        case kCLOCK_IpSrcSysPllAsync:
            freq = CLOCK_GetSysPllAsyncFreq(asycClk);
            break;
        default:
            freq = 0U;
            break;
    }

    if (reg & (PCC_CLKCFG_PCD_MASK | PCC_CLKCFG_FRAC_MASK))
    {
        return freq * (PCC_FRAC_VAL(reg) + 1U) / (PCC_PCD_VAL(reg) + 1U);
    }
    else
    {
        return freq;
    }
}

bool CLOCK_EnableUsbfs0Clock(clock_ip_src_t src, uint32_t freq)
{
    bool ret = true;

    CLOCK_DisableClock(kCLOCK_Usbfs0);

    if (kCLOCK_IpSrcNoneOrExt == src)
    {
        CLOCK_SetIpSrc(kCLOCK_Usbfs0, kCLOCK_IpSrcNoneOrExt);
    }
    else
    {
        switch (freq)
        {
            case 120000000U:
                CLOCK_SetIpSrcDiv(kCLOCK_Usbfs0, src, 4U, 1U);
                break;
            case 96000000U:
                CLOCK_SetIpSrcDiv(kCLOCK_Usbfs0, src, 1U, 0U);
                break;
            case 72000000U:
                CLOCK_SetIpSrcDiv(kCLOCK_Usbfs0, src, 2U, 1U);
                break;
            case 48000000U:
                CLOCK_SetIpSrcDiv(kCLOCK_Usbfs0, src, 0U, 0U);
                break;
            default:
                ret = false;
                break;
        }
    }

    CLOCK_EnableClock(kCLOCK_Usbfs0);

    return ret;
}

uint32_t CLOCK_GetSysClkFreq(scg_sys_clk_t type)
{
    uint32_t freq;

    scg_sys_clk_config_t sysClkConfig;

    CLOCK_GetCurSysClkConfig(&sysClkConfig);

    switch (sysClkConfig.src)
    {
        case kSCG_SysClkSrcSysOsc:
            freq = CLOCK_GetSysOscFreq();
            break;
        case kSCG_SysClkSrcSirc:
            freq = CLOCK_GetSircFreq();
            break;
        case kSCG_SysClkSrcFirc:
            freq = CLOCK_GetFircFreq();
            break;
        case kSCG_SysClkSrcSysPll:
            freq = CLOCK_GetSysPllFreq();
            break;
        default:
            freq = 0U;
            break;
    }

    freq /= (sysClkConfig.divCore + 1U);

    if (kSCG_SysClkSlow == type)
    {
        freq /= (sysClkConfig.divSlow + 1U);
    }
    else
    {
    }

    return freq;
}

status_t CLOCK_InitSysOsc(const scg_sosc_config_t *config)
{
    assert(config);
    uint8_t range = 0U; /* SCG_SOSCCFG[RANGE] */
    status_t status;
    uint8_t tmp8;

    /* If crystal oscillator used, need to get RANGE value base on frequency. */
    if (kSCG_SysOscModeExt != config->workMode)
    {
        if ((config->freq >= 32768U) && (config->freq <= 40000U))
        {
            range = 1U;
        }
        else if ((config->freq >= 1000000U) && (config->freq <= 8000000U))
        {
            range = 2U;
        }
        else if ((config->freq >= 8000000U) && (config->freq <= 32000000U))
        {
            range = 3U;
        }
        else
        {
            return kStatus_InvalidArgument;
        }
    }

    /* De-init the SOSC first. */
    status = CLOCK_DeinitSysOsc();

    if (kStatus_Success != status)
    {
        return status;
    }

    /* Now start to set up OSC clock. */
    /* Step 1. Setup dividers. */
    SCG->SOSCDIV =
        SCG_SOSCDIV_SOSCDIV1(config->div1) | SCG_SOSCDIV_SOSCDIV2(config->div2) | SCG_SOSCDIV_SOSCDIV3(config->div3);

    /* Step 2. Set OSC configuration. */
    SCG->SOSCCFG = config->capLoad | config->workMode | SCG_SOSCCFG_RANGE(range);

    /* Step 3. Enable clock. */
    /* SCG->SOSCCSR = SCG_SOSCCSR_SOSCEN_MASK | (config->enableMode); */
    tmp8 = config->enableMode;
    tmp8 |= SCG_SOSCCSR_SOSCEN_MASK;
    SCG->SOSCCSR = tmp8;

    /* Step 4. Wait for OSC clock to be valid. */
    while (!(SCG->SOSCCSR & SCG_SOSCCSR_SOSCVLD_MASK))
    {
    }

    /* Step 5. Enabe monitor. */
    SCG->SOSCCSR |= (uint32_t)config->monitorMode;

    return kStatus_Success;
}

status_t CLOCK_DeinitSysOsc(void)
{
    uint32_t reg = SCG->SOSCCSR;

    /* If clock is used by system, return error. */
    if (reg & SCG_SOSCCSR_SOSCSEL_MASK)
    {
        return kStatus_SCG_Busy;
    }

    /* If configure register is locked, return error. */
    if (reg & SCG_SOSCCSR_LK_MASK)
    {
        return kStatus_ReadOnly;
    }

    SCG->SOSCCSR = SCG_SOSCCSR_SOSCERR_MASK;

    return kStatus_Success;
}

uint32_t CLOCK_GetSysOscFreq(void)
{
    if (SCG->SOSCCSR & SCG_SOSCCSR_SOSCVLD_MASK) /* System OSC clock is valid. */
    {
        /* Please call CLOCK_SetXtal0Freq base on board setting before using OSC0 clock. */
        assert(g_xtal0Freq);
        return g_xtal0Freq;
    }
    else
    {
        return 0U;
    }
}

uint32_t CLOCK_GetSysOscAsyncFreq(scg_async_clk_t type)
{
    uint32_t oscFreq = CLOCK_GetSysOscFreq();
    uint32_t divider = 0U;

    /* Get divider. */
    if (oscFreq)
    {
        switch (type)
        {
            case kSCG_AsyncDiv3Clk: /* SOSCDIV3_CLK. */
                divider = SCG_SOSCDIV_SOSCDIV3_VAL;
                break;
            case kSCG_AsyncDiv2Clk: /* SOSCDIV2_CLK. */
                divider = SCG_SOSCDIV_SOSCDIV2_VAL;
                break;
            case kSCG_AsyncDiv1Clk: /* SOSCDIV1_CLK. */
                divider = SCG_SOSCDIV_SOSCDIV1_VAL;
                break;
            default:
                break;
        }
    }
    if (divider)
    {
        return oscFreq >> (divider - 1U);
    }
    else /* Output disabled. */
    {
        return 0U;
    }
}

status_t CLOCK_InitSirc(const scg_sirc_config_t *config)
{
    assert(config);

    status_t status;

    /* De-init the SIRC first. */
    status = CLOCK_DeinitSirc();

    if (kStatus_Success != status)
    {
        return status;
    }

    /* Now start to set up SIRC clock. */
    /* Step 1. Setup dividers. */
    SCG->SIRCDIV =
        SCG_SIRCDIV_SIRCDIV1(config->div1) | SCG_SIRCDIV_SIRCDIV2(config->div2) | SCG_SIRCDIV_SIRCDIV3(config->div3);

    /* Step 2. Set SIRC configuration. */
    SCG->SIRCCFG = SCG_SIRCCFG_RANGE(config->range);

    /* Step 3. Enable clock. */
    SCG->SIRCCSR = SCG_SIRCCSR_SIRCEN_MASK | config->enableMode;

    /* Step 4. Wait for SIRC clock to be valid. */
    while (!(SCG->SIRCCSR & SCG_SIRCCSR_SIRCVLD_MASK))
    {
    }

    return kStatus_Success;
}

status_t CLOCK_DeinitSirc(void)
{
    uint32_t reg = SCG->SIRCCSR;

    /* If clock is used by system, return error. */
    if (reg & SCG_SIRCCSR_SIRCSEL_MASK)
    {
        return kStatus_SCG_Busy;
    }

    /* If configure register is locked, return error. */
    if (reg & SCG_SIRCCSR_LK_MASK)
    {
        return kStatus_ReadOnly;
    }

    SCG->SIRCCSR = 0U;

    return kStatus_Success;
}

uint32_t CLOCK_GetSircFreq(void)
{
    static const uint32_t sircFreq[] = {SCG_SIRC_LOW_RANGE_FREQ, SCG_SIRC_HIGH_RANGE_FREQ};

    if (SCG->SIRCCSR & SCG_SIRCCSR_SIRCVLD_MASK) /* SIRC is valid. */
    {
        return sircFreq[SCG_SIRCCFG_RANGE_VAL];
    }
    else
    {
        return 0U;
    }
}

uint32_t CLOCK_GetSircAsyncFreq(scg_async_clk_t type)
{
    uint32_t sircFreq = CLOCK_GetSircFreq();
    uint32_t divider = 0U;

    /* Get divider. */
    if (sircFreq)
    {
        switch (type)
        {
            case kSCG_AsyncDiv3Clk: /* SIRCDIV3_CLK. */
                divider = SCG_SIRCDIV_SIRCDIV3_VAL;
                break;
            case kSCG_AsyncDiv2Clk: /* SIRCDIV2_CLK. */
                divider = SCG_SIRCDIV_SIRCDIV2_VAL;
                break;
            case kSCG_AsyncDiv1Clk: /* SIRCDIV2_CLK. */
                divider = SCG_SIRCDIV_SIRCDIV1_VAL;
                break;
            default:
                break;
        }
    }
    if (divider)
    {
        return sircFreq >> (divider - 1U);
    }
    else /* Output disabled. */
    {
        return 0U;
    }
}

status_t CLOCK_InitFirc(const scg_firc_config_t *config)
{
    assert(config);

    status_t status;

    /* De-init the FIRC first. */
    status = CLOCK_DeinitFirc();

    if (kStatus_Success != status)
    {
        return status;
    }

    /* Now start to set up FIRC clock. */
    /* Step 1. Setup dividers. */
    SCG->FIRCDIV =
        SCG_FIRCDIV_FIRCDIV1(config->div1) | SCG_FIRCDIV_FIRCDIV2(config->div2) | SCG_FIRCDIV_FIRCDIV3(config->div3);

    /* Step 2. Set FIRC configuration. */
    SCG->FIRCCFG = SCG_FIRCCFG_RANGE(config->range);

    /* Step 3. Set trimming configuration. */
    if (config->trimConfig)
    {
        SCG->FIRCTCFG =
            SCG_FIRCTCFG_TRIMDIV(config->trimConfig->trimDiv) | SCG_FIRCTCFG_TRIMSRC(config->trimConfig->trimSrc);

        /* TODO: Write FIRCSTAT cause bus error: TKT266932. */
        if (kSCG_FircTrimNonUpdate == config->trimConfig->trimMode)
        {
            SCG->FIRCSTAT = SCG_FIRCSTAT_TRIMCOAR(config->trimConfig->trimCoar) |
                            SCG_FIRCSTAT_TRIMFINE(config->trimConfig->trimFine);
        }

        /* trim mode. */
        SCG->FIRCCSR = config->trimConfig->trimMode;

        if (SCG->FIRCCSR & SCG_FIRCCSR_FIRCERR_MASK)
        {
            return kStatus_Fail;
        }
    }

    /* Step 4. Enable clock. */
    SCG->FIRCCSR |= (SCG_FIRCCSR_FIRCEN_MASK | config->enableMode);

    /* Step 5. Wait for FIRC clock to be valid. */
    while (!(SCG->FIRCCSR & SCG_FIRCCSR_FIRCVLD_MASK))
    {
    }

    return kStatus_Success;
}

status_t CLOCK_DeinitFirc(void)
{
    uint32_t reg = SCG->FIRCCSR;

    /* If clock is used by system, return error. */
    if (reg & SCG_FIRCCSR_FIRCSEL_MASK)
    {
        return kStatus_SCG_Busy;
    }

    /* If configure register is locked, return error. */
    if (reg & SCG_FIRCCSR_LK_MASK)
    {
        return kStatus_ReadOnly;
    }

    SCG->FIRCCSR = SCG_FIRCCSR_FIRCERR_MASK;

    return kStatus_Success;
}

uint32_t CLOCK_GetFircFreq(void)
{
    static const uint32_t fircFreq[] = {
        SCG_FIRC_FREQ0, SCG_FIRC_FREQ1, SCG_FIRC_FREQ2, SCG_FIRC_FREQ3,
    };

    if (SCG->FIRCCSR & SCG_FIRCCSR_FIRCVLD_MASK) /* FIRC is valid. */
    {
        return fircFreq[SCG_FIRCCFG_RANGE_VAL];
    }
    else
    {
        return 0U;
    }
}

uint32_t CLOCK_GetFircAsyncFreq(scg_async_clk_t type)
{
    uint32_t fircFreq = CLOCK_GetFircFreq();
    uint32_t divider = 0U;

    /* Get divider. */
    if (fircFreq)
    {
        switch (type)
        {
            case kSCG_AsyncDiv3Clk: /* FIRCDIV3_CLK. */
                divider = SCG_FIRCDIV_FIRCDIV3_VAL;
                break;
            case kSCG_AsyncDiv2Clk: /* FIRCDIV2_CLK. */
                divider = SCG_FIRCDIV_FIRCDIV2_VAL;
                break;
            case kSCG_AsyncDiv1Clk: /* FIRCDIV1_CLK. */
                divider = SCG_FIRCDIV_FIRCDIV1_VAL;
                break;
            default:
                break;
        }
    }
    if (divider)
    {
        return fircFreq >> (divider - 1U);
    }
    else /* Output disabled. */
    {
        return 0U;
    }
}

uint32_t CLOCK_GetSysPllMultDiv(uint32_t refFreq, uint32_t desireFreq, uint8_t *mult, uint8_t *prediv)
{
    uint8_t ret_prediv;          /* PREDIV to return */
    uint8_t ret_mult;            /* MULT to return */
    uint8_t prediv_min;          /* Minimal PREDIV value to make reference clock in allowed range. */
    uint8_t prediv_max;          /* Max PREDIV value to make reference clock in allowed range. */
    uint8_t prediv_cur;          /* PREDIV value for iteration. */
    uint8_t mult_cur;            /* MULT value for iteration. */
    uint32_t ret_freq = 0U;      /* Output frequency to return .*/
    uint32_t diff = 0xFFFFFFFFU; /* Difference between desireFreq and return frequency. */
    uint32_t ref_div;            /* Reference frequency after PREDIV. */

    /*
     * Steps:
     * 1. Get allowed prediv with such rules:
     *    1). refFreq / prediv >= SCG_PLL_REF_MIN.
     *    2). refFreq / prediv <= SCG_PLL_REF_MAX.
     * 2. For each allowed prediv, there are two candidate mult values:
     *    1). (desireFreq / (refFreq / prediv)).
     *    2). (desireFreq / (refFreq / prediv)) + 1.
     *    If could get the precise desired frequency, return current prediv and
     *    mult directly. Otherwise choose the one which is closer to desired
     *    frequency.
     */

    /* Reference frequency is out of range. */
    if ((refFreq < SCG_SPLL_REF_MIN) ||
        (refFreq > (SCG_SPLL_REF_MAX * (SCG_SPLL_PREDIV_MAX_VALUE + SCG_SPLL_PREDIV_BASE_VALUE))))
    {
        return 0U;
    }

    /* refFreq/PREDIV must in a range. First get the allowed PREDIV range. */
    prediv_max = refFreq / SCG_SPLL_REF_MIN;
    prediv_min = (refFreq + SCG_SPLL_REF_MAX - 1U) / SCG_SPLL_REF_MAX;

    desireFreq *= 2U;

    /* PREDIV traversal. */
    for (prediv_cur = prediv_max; prediv_cur >= prediv_min; prediv_cur--)
    {
        /*
         * For each PREDIV, the available MULT is (desireFreq*PREDIV/refFreq)
         * or (desireFreq*PREDIV/refFreq + 1U). This function chooses the closer
         * one.
         */
        /* Reference frequency after PREDIV. */
        ref_div = refFreq / prediv_cur;

        mult_cur = desireFreq / ref_div;

        if ((mult_cur < SCG_SPLL_MULT_BASE_VALUE - 1U) ||
            (mult_cur > SCG_SPLL_MULT_BASE_VALUE + SCG_SPLL_MULT_MAX_VALUE))
        {
            /* No MULT is available with this PREDIV. */
            continue;
        }

        ret_freq = mult_cur * ref_div;

        if (mult_cur >= SCG_SPLL_MULT_BASE_VALUE)
        {
            if (ret_freq == desireFreq) /* If desire frequency is got. */
            {
                *prediv = prediv_cur - SCG_SPLL_PREDIV_BASE_VALUE;
                *mult = mult_cur - SCG_SPLL_MULT_BASE_VALUE;
                return ret_freq / 2U;
            }
            if (diff > desireFreq - ret_freq) /* New PRDIV/VDIV is closer. */
            {
                diff = desireFreq - ret_freq;
                ret_prediv = prediv_cur;
                ret_mult = mult_cur;
            }
        }
        mult_cur++;
        if (mult_cur <= (SCG_SPLL_MULT_BASE_VALUE + SCG_SPLL_MULT_MAX_VALUE))
        {
            ret_freq += ref_div;
            if (diff > ret_freq - desireFreq) /* New PRDIV/VDIV is closer. */
            {
                diff = ret_freq - desireFreq;
                ret_prediv = prediv_cur;
                ret_mult = mult_cur;
            }
        }
    }

    if (0xFFFFFFFFU != diff)
    {
        /* PREDIV/MULT found. */
        *prediv = ret_prediv - SCG_SPLL_PREDIV_BASE_VALUE;
        *mult = ret_mult - SCG_SPLL_MULT_BASE_VALUE;
        return ((refFreq / ret_prediv) * ret_mult) / 2;
    }
    else
    {
        return 0U; /* No proper PREDIV/MULT found. */
    }
}

status_t CLOCK_InitSysPll(const scg_spll_config_t *config)
{
    assert(config);

    status_t status;

    /* De-init the SPLL first. */
    status = CLOCK_DeinitSysPll();

    if (kStatus_Success != status)
    {
        return status;
    }

    /* Now start to set up PLL clock. */
    /* Step 1. Setup dividers. */
    SCG->SPLLDIV =
        SCG_SPLLDIV_SPLLDIV1(config->div1) | SCG_SPLLDIV_SPLLDIV2(config->div2) | SCG_SPLLDIV_SPLLDIV3(config->div3);

    /* Step 2. Set PLL configuration. */
    SCG->SPLLCFG =
        SCG_SPLLCFG_SOURCE(config->src) | SCG_SPLLCFG_PREDIV(config->prediv) | SCG_SPLLCFG_MULT(config->mult);

    /* Step 3. Enable clock. */
    SCG->SPLLCSR = SCG_SPLLCSR_SPLLEN_MASK | config->enableMode;

    /* Step 4. Wait for PLL clock to be valid. */
    while (!(SCG->SPLLCSR & SCG_SPLLCSR_SPLLVLD_MASK))
    {
    }

    /* Step 5. Enabe monitor. */
    SCG->SPLLCSR |= (uint32_t)config->monitorMode;

    return kStatus_Success;
}

status_t CLOCK_DeinitSysPll(void)
{
    uint32_t reg = SCG->SPLLCSR;

    /* If clock is used by system, return error. */
    if (reg & SCG_SPLLCSR_SPLLSEL_MASK)
    {
        return kStatus_SCG_Busy;
    }

    /* If configure register is locked, return error. */
    if (reg & SCG_SPLLCSR_LK_MASK)
    {
        return kStatus_ReadOnly;
    }

    /* Deinit and clear the error. */
    SCG->SPLLCSR = SCG_SPLLCSR_SPLLERR_MASK;

    return kStatus_Success;
}

static uint32_t CLOCK_GetSysPllCommonFreq(void)
{
    uint32_t freq = 0U;

    if (SCG->SPLLCFG & SCG_SPLLCFG_SOURCE_MASK) /* If use FIRC */
    {
        freq = CLOCK_GetFircFreq();
    }
    else /* Use System OSC. */
    {
        freq = CLOCK_GetSysOscFreq();
    }

    if (freq) /* If source is valid. */
    {
        freq /= (SCG_SPLLCFG_PREDIV_VAL + SCG_SPLL_PREDIV_BASE_VALUE); /* Pre-divider. */
        freq *= (SCG_SPLLCFG_MULT_VAL + SCG_SPLL_MULT_BASE_VALUE);     /* Multiplier. */
    }

    return freq;
}

uint32_t CLOCK_GetSysPllFreq(void)
{
    uint32_t freq;

    if (SCG->SPLLCSR & SCG_SPLLCSR_SPLLVLD_MASK) /* System PLL is valid. */
    {
        freq = CLOCK_GetSysPllCommonFreq();

        return freq >> 1U;
    }
    else
    {
        return 0U;
    }
}

uint32_t CLOCK_GetSysPllAsyncFreq(scg_async_clk_t type)
{
    uint32_t pllFreq = CLOCK_GetSysPllFreq();
    uint32_t divider = 0U;

    /* Get divider. */
    if (pllFreq)
    {
        switch (type)
        {
            case kSCG_AsyncDiv3Clk: /* SPLLDIV3_CLK. */
                divider = SCG_SPLLDIV_SPLLDIV3_VAL;
                break;
            case kSCG_AsyncDiv2Clk: /* SPLLDIV2_CLK. */
                divider = SCG_SPLLDIV_SPLLDIV2_VAL;
                break;
            case kSCG_AsyncDiv1Clk: /* SPLLDIV1_CLK. */
                divider = SCG_SPLLDIV_SPLLDIV1_VAL;
                break;
            default:
                break;
        }
    }
    if (divider)
    {
        return pllFreq >> (divider - 1U);
    }
    else /* Output disabled. */
    {
        return 0U;
    }
}
