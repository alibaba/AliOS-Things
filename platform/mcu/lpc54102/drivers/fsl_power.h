/*
 * Copyright 2017 NXP
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
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
#ifndef _FSL_POWER_H_
#define _FSL_POWER_H_

#include "fsl_common.h"

/*! @addtogroup power */
/*! @{ */

/*! @file */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
/*! @brief power driver version 2.0.0. */
#define FSL_POWER_DRIVER_VERSION (MAKE_VERSION(2, 0, 0))
/*@}*/

#define MAKE_PD_BITS(reg, slot) ((reg << 8) | slot)
#define PDRCFG0 0x0U

typedef enum pd_bits
{
    kPDRUNCFG_PD_IRC_OSC = MAKE_PD_BITS(PDRCFG0, 3U),
    kPDRUNCFG_PD_IRC = MAKE_PD_BITS(PDRCFG0, 4U),
    kPDRUNCFG_PD_FLASH = MAKE_PD_BITS(PDRCFG0, 5U),
    kPDRUNCFG_PD_BOD_RST = MAKE_PD_BITS(PDRCFG0, 7U),
    kPDRUNCFG_PD_BOD_INTR = MAKE_PD_BITS(PDRCFG0, 8U),
    kPDRUNCFG_PD_ADC0 = MAKE_PD_BITS(PDRCFG0, 10U),
    kPDRUNCFG_PD_SRAM0A = MAKE_PD_BITS(PDRCFG0, 13U),
    kPDRUNCFG_PD_SRAM0B = MAKE_PD_BITS(PDRCFG0, 14U),
    kPDRUNCFG_PD_SRAM1 = MAKE_PD_BITS(PDRCFG0, 15U),
    kPDRUNCFG_PD_SRAM2 = MAKE_PD_BITS(PDRCFG0, 16U),
    kPDRUNCFG_PD_ROM = MAKE_PD_BITS(PDRCFG0, 17U),
    kPDRUNCFG_PD_VDDA = MAKE_PD_BITS(PDRCFG0, 19U),
    kPDRUNCFG_PD_WDT_OSC = MAKE_PD_BITS(PDRCFG0, 20U),
    kPDRUNCFG_PD_SYS_PLL = MAKE_PD_BITS(PDRCFG0, 22U),
    kPDRUNCFG_PD_VREFP = MAKE_PD_BITS(PDRCFG0, 23U),
    kPDRUNCFG_PD_32K_OSC = MAKE_PD_BITS(PDRCFG0, 24U),
    kPDRUNCFG_ForceUnsigned = 0x80000000U

} pd_bit_t;

/* Power mode configuration API parameter */
typedef enum _power_mode_config
{
    kPmu_Sleep = 0U,
    kPmu_Deep_Sleep = 1U,
    kPmu_Power_Down = 2U,
    kPmu_Deep_PowerDown = 3U,
} power_mode_cfg_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*!
* @name Power Configuration
* @{
*/

/*!
 * @brief API to enable PDRUNCFG bit in the Syscon. Note that enabling the bit powers down the peripheral
 *
 * @param en    peripheral for which to enable the PDRUNCFG bit
 * @return none
 */
static inline void POWER_EnablePD(pd_bit_t en)
{
    /* PDRUNCFGSET */
    SYSCON->PDRUNCFGSET = (1UL << (en & 0xffU));
}

/*!
 * @brief API to disable PDRUNCFG bit in the Syscon. Note that disabling the bit powers up the peripheral
 *
 * @param en    peripheral for which to disable the PDRUNCFG bit
 * @return none
 */
static inline void POWER_DisablePD(pd_bit_t en)
{
    /* PDRUNCFGCLR */
    SYSCON->PDRUNCFGCLR = (1UL << (en & 0xffU));
}

/*!
 * @brief API to enable deep sleep bit in the ARM Core.
 *
 * @return none
 */
static inline void POWER_EnableDeepSleep(void)
{
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
}

/*!
 * @brief API to disable deep sleep bit in the ARM Core.
 *
 * @return none
 */
static inline void POWER_DisableDeepSleep(void)
{
    SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;
}

/*!
 * @brief API in power lib to power down flash.
 *
 * @return none
 */
void Chip_POWER_SetFLASHPower(uint32_t new_power_mode);

/*!
 * @brief API to power down flash controller.
 *
 * @return none
 */
static inline void POWER_PowerDownFlash(void)
{
    Chip_POWER_SetFLASHPower(0U);
    /* TURN OFF clock for Flash Controller (only needed for FLASH programming, will be turned on by ROM API) */
    CLOCK_DisableClock(kCLOCK_Flash);

    /* TURN OFF clock for Flash Accelerator */
    CLOCK_DisableClock(kCLOCK_Fmc);
}

/*!
 * @brief API to power up flash controller.
 *
 * @return none
 */
static inline void POWER_PowerUpFlash(void)
{
    Chip_POWER_SetFLASHPower(1U);
    /* TURN ON clock for flash controller */
    CLOCK_EnableClock(kCLOCK_Fmc);
}

/*!
 * @brief Power Library API to enter different power mode.
 *
 * @param exclude_from_pd  Bit mask of the PDRUNCFG bits that needs to be powered on during deep sleep
 * @return none
 */
void POWER_EnterPowerMode(power_mode_cfg_t mode, uint64_t exclude_from_pd);

/*!
 * @brief Power Library API to enter sleep mode.
 *
 * @return none
 */
void POWER_EnterSleep(void);

/*!
 * @brief Power Library API to enter deep sleep mode.
 *
 * @param exclude_from_pd  Bit mask of the PDRUNCFG bits that needs to be powered on during deep sleep
 * @return none
 */
void POWER_EnterDeepSleep(uint64_t exclude_from_pd);

/*!
 * @brief Power Library API to enter power down mode.
 *
 * @param exclude_from_pd  Bit mask of the PDRUNCFG bits that needs to be powered on during deep sleep
 * @return none
 */
void POWER_EnterPowerDown(uint64_t exclude_from_pd);

/*!
 * @brief Power Library API to enter deep power down mode.
 *
 * @param exclude_from_pd  Bit mask of the PDRUNCFG bits that needs to be powered on during deep power down mode, 
 *                         but this is has no effect as the voltages are cut off.
 * @return none
 */
void POWER_EnterDeepPowerDown(uint64_t exclude_from_pd);

/*!
 * @brief Power Library API to choose normal regulation and set the voltage for the desired operating frequency.
 *
 * @param freq  - The desired frequency at which the part would like to operate, 
 *                note that the voltage and flash wait states should be set before changing frequency
 * @return none
 */
void POWER_SetVoltageForFreq(uint32_t freq);


/*!
 * @brief Power Library API to return the library version.
 *
 * @return version number of the power library
 */
uint32_t POWER_GetLibVersion(void);

/* @} */

#ifdef __cplusplus
}
#endif

/*! @} */

#endif /* _FSL_POWER_H_ */
