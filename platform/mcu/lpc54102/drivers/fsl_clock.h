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

#ifndef _FSL_CLOCK_H_
#define _FSL_CLOCK_H_

#include "fsl_device_registers.h"
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

/*! @addtogroup clock */
/*! @{ */

/*! @file */

/*******************************************************************************
 * Definitions
 *****************************************************************************/

/*! @name Driver version */
/*@{*/
/*! @brief CLOCK driver version 2.0.1. */
#define FSL_CLOCK_DRIVER_VERSION (MAKE_VERSION(2, 0, 1))
/*@}*/

/*!
 * @brief User-defined the size of cache for CLOCK_PllGetConfig() function.
 *
 * Once define this MACRO to be non-zero value, CLOCK_PllGetConfig() function
 * would cache the recent calulation and accelerate the execution to get the
 * right settings.
 */

/*! @brief Clock ip name array for ROM. */
#define ROM_CLOCKS  \
    {               \
        kCLOCK_Rom, \
    }

/*! @brief Clock ip name array for SRAM. */
#define SRAM_CLOCKS                 \
    {                               \
        kCLOCK_Sram1, kCLOCK_Sram2, \
    }

/*! @brief Clock ip name array for FLASH. */
#define FLASH_CLOCKS  \
    {                 \
        kCLOCK_Flash, \
    }

/*! @brief Clock ip name array for FMC. */
#define FMC_CLOCKS  \
    {               \
        kCLOCK_Fmc, \
    }

/*! @brief Clock ip name array for INPUTMUX. */
#define INPUTMUX_CLOCKS  \
    {                    \
        kCLOCK_InputMux, \
    }

/*! @brief Clock ip name array for IOCON. */
#define IOCON_CLOCKS  \
    {                 \
        kCLOCK_Iocon, \
    }

/*! @brief Clock ip name array for GPIO. */
#define GPIO_CLOCKS                 \
    {                               \
        kCLOCK_Gpio0, kCLOCK_Gpio1, \
    }

/*! @brief Clock ip name array for PINT. */
#define PINT_CLOCKS  \
    {                \
        kCLOCK_Pint, \
    }

/*! @brief Clock ip name array for GINT. */
#define GINT_CLOCKS             \
    {                           \
        kCLOCK_Gint,kCLOCK_Gint \
    }

/*! @brief Clock ip name array for DMA. */
#define DMA_CLOCKS  \
    {               \
        kCLOCK_Dma, \
    }

/*! @brief Clock ip name array for CRC. */
#define CRC_CLOCKS  \
    {               \
        kCLOCK_Crc, \
    }

/*! @brief Clock ip name array for WWDT. */
#define WWDT_CLOCKS  \
    {                \
        kCLOCK_Wwdt, \
    }

/*! @brief Clock ip name array for RTC. */
#define RTC_CLOCKS  \
    {               \
        kCLOCK_Rtc, \
    }

/*! @brief Clock ip name array for MAILBOX. */
#define MAILBOX_CLOCKS  \
    {                   \
        kCLOCK_Mailbox, \
    }

/*! @brief Clock ip name array for ADC. */
#define ADC_CLOCKS   \
    {                \
        kCLOCK_Adc0, \
    }

/*! @brief Clock ip name array for MRT. */
#define MRT_CLOCKS  \
    {               \
        kCLOCK_Mrt, \
    }

/*! @brief Clock ip name array for RIT. */
#define RIT_CLOCKS  \
    {               \
        kCLOCK_Rit, \
    }

/*! @brief Clock ip name array for SCT. */
#define SCT_CLOCKS   \
    {                \
        kCLOCK_Sct0, \
    }

/*! @brief Clock ip name array for FIFO. */
#define FIFO_CLOCKS  \
    {                \
        kCLOCK_Fifo, \
    }

/*! @brief Clock ip name array for UTICK. */
#define UTICK_CLOCKS  \
    {                 \
        kCLOCK_Utick, \
    }

/*! @brief Clock ip name array for CT32B. */
#define CTIMER_CLOCKS                                                             \
    {                                                                             \
        kCLOCK_Ct32b0, kCLOCK_Ct32b1, kCLOCK_Ct32b2, kCLOCK_Ct32b3, kCLOCK_Ct32b4 \
    }

/*! @brief Clock ip name array for USART. */
#define USART_CLOCKS                                                \
    {                                                               \
        kCLOCK_Usart0, kCLOCK_Usart1, kCLOCK_Usart2, kCLOCK_Usart3, \
    }

/*! @brief Clock ip name array for I2C. */
#define I2C_CLOCKS                             \
    {                                          \
        kCLOCK_I2c0, kCLOCK_I2c1, kCLOCK_I2c2, \
    }

/*! @brief Clock ip name array for SPI. */
#define SPI_CLOCKS                \
    {                             \
        kCLOCK_Spi0, kCLOCK_Spi1, \
    }

/*! @brief Clock ip name array for FRG. */
#define FRG_CLOCKS  \
    {               \
        kCLOCK_Frg, \
    }

/*! @brief Clock gate name used for CLOCK_EnableClock/CLOCK_DisableClock. */
/*------------------------------------------------------------------------------
 clock_ip_name_t definition:
------------------------------------------------------------------------------*/

#define CLK_GATE_REG_OFFSET_SHIFT 8U
#define CLK_GATE_REG_OFFSET_MASK 0xFFFFFF00U
#define CLK_GATE_BIT_SHIFT_SHIFT 0U
#define CLK_GATE_BIT_SHIFT_MASK 0x000000FFU

#define CLK_GATE_DEFINE(reg_offset, bit_shift)                                  \
    ((((reg_offset) << CLK_GATE_REG_OFFSET_SHIFT) & CLK_GATE_REG_OFFSET_MASK) | \
     (((bit_shift) << CLK_GATE_BIT_SHIFT_SHIFT) & CLK_GATE_BIT_SHIFT_MASK))

#define CLK_GATE_ABSTRACT_REG_OFFSET(x) (((uint32_t)(x)&CLK_GATE_REG_OFFSET_MASK) >> CLK_GATE_REG_OFFSET_SHIFT)
#define CLK_GATE_ABSTRACT_BITS_SHIFT(x) (((uint32_t)(x)&CLK_GATE_BIT_SHIFT_MASK) >> CLK_GATE_BIT_SHIFT_SHIFT)

#define AHB_CLK_CTRL0 0
#define AHB_CLK_CTRL1 1
#define ASYNC_CLK_CTRL0 2

/*! @brief Clock gate name used for CLOCK_EnableClock/CLOCK_DisableClock. */
typedef enum _clock_ip_name
{
    /* AHBCLKCTRL0 */
    kCLOCK_IpInvalid = 0U,
    kCLOCK_Rom = CLK_GATE_DEFINE(AHB_CLK_CTRL0, 1),
    kCLOCK_Sram1 = CLK_GATE_DEFINE(AHB_CLK_CTRL0, 3),
    kCLOCK_Sram2 = CLK_GATE_DEFINE(AHB_CLK_CTRL0, 4),
    kCLOCK_Flash = CLK_GATE_DEFINE(AHB_CLK_CTRL0, 7),
    kCLOCK_Fmc = CLK_GATE_DEFINE(AHB_CLK_CTRL0, 8),
    kCLOCK_InputMux = CLK_GATE_DEFINE(AHB_CLK_CTRL0, 11),
    kCLOCK_Iocon = CLK_GATE_DEFINE(AHB_CLK_CTRL0, 13),
    kCLOCK_Gpio0 = CLK_GATE_DEFINE(AHB_CLK_CTRL0, 14),
    kCLOCK_Gpio1 = CLK_GATE_DEFINE(AHB_CLK_CTRL0, 15),
    kCLOCK_Pint = CLK_GATE_DEFINE(AHB_CLK_CTRL0, 18),
    kCLOCK_Gint = CLK_GATE_DEFINE(AHB_CLK_CTRL0, 19),
    kCLOCK_Dma = CLK_GATE_DEFINE(AHB_CLK_CTRL0, 20),
    kCLOCK_Crc = CLK_GATE_DEFINE(AHB_CLK_CTRL0, 21),
    kCLOCK_Wwdt = CLK_GATE_DEFINE(AHB_CLK_CTRL0, 22),
    kCLOCK_Rtc = CLK_GATE_DEFINE(AHB_CLK_CTRL0, 23),
    kCLOCK_Mailbox = CLK_GATE_DEFINE(AHB_CLK_CTRL0, 26),
    kCLOCK_Adc0 = CLK_GATE_DEFINE(AHB_CLK_CTRL0, 27),
    /* AHBCLKCTRL1 */
    kCLOCK_Mrt = CLK_GATE_DEFINE(AHB_CLK_CTRL1, 0),
    kCLOCK_Rit = CLK_GATE_DEFINE(AHB_CLK_CTRL1, 1),
    kCLOCK_Sct0 = CLK_GATE_DEFINE(AHB_CLK_CTRL1, 2),
    kCLOCK_Fifo = CLK_GATE_DEFINE(AHB_CLK_CTRL1, 9),
    kCLOCK_Utick = CLK_GATE_DEFINE(AHB_CLK_CTRL1, 10),
    kCLOCK_Ct32b2 = CLK_GATE_DEFINE(AHB_CLK_CTRL1, 22),
    kCLOCK_Ct32b3 = CLK_GATE_DEFINE(AHB_CLK_CTRL1, 26),
    kCLOCK_Ct32b4 = CLK_GATE_DEFINE(AHB_CLK_CTRL1, 27),
    /* ASYNCPRESETCTRL */
    kCLOCK_Usart0 = CLK_GATE_DEFINE(ASYNC_CLK_CTRL0, 1),
    kCLOCK_Usart1 = CLK_GATE_DEFINE(ASYNC_CLK_CTRL0, 2),
    kCLOCK_Usart2 = CLK_GATE_DEFINE(ASYNC_CLK_CTRL0, 3),
    kCLOCK_Usart3 = CLK_GATE_DEFINE(ASYNC_CLK_CTRL0, 4),
    kCLOCK_I2c0 = CLK_GATE_DEFINE(ASYNC_CLK_CTRL0, 5),
    kCLOCK_I2c1 = CLK_GATE_DEFINE(ASYNC_CLK_CTRL0, 6),
    kCLOCK_I2c2 = CLK_GATE_DEFINE(ASYNC_CLK_CTRL0, 7),
    kCLOCK_Spi0 = CLK_GATE_DEFINE(ASYNC_CLK_CTRL0, 9),
    kCLOCK_Spi1 = CLK_GATE_DEFINE(ASYNC_CLK_CTRL0, 10),
    kCLOCK_Ct32b0 = CLK_GATE_DEFINE(ASYNC_CLK_CTRL0, 13),
    kCLOCK_Ct32b1 = CLK_GATE_DEFINE(ASYNC_CLK_CTRL0, 14),
    kCLOCK_Frg = CLK_GATE_DEFINE(ASYNC_CLK_CTRL0, 15),
} clock_ip_name_t;

/*! @brief Clock name used to get clock frequency. */
typedef enum _clock_name
{
    kCLOCK_MainClk,     /*!< Main clock                                              */
    kCLOCK_CoreSysClk,  /*!< Core/system clock                                       */
    kCLOCK_BusClk,      /*!< Bus clock (AHB clock)                                   */
    kCLOCK_Irc,         /*!< Internal IRC                                            */
    kCLOCK_ExtClk,      /*!< External Clock                                          */
    kCLOCK_PllOut,      /*!< PLL Output                                              */
    kClock_WdtOsc,      /*!< Watchdog Oscillator                                     */
    kCLOCK_FRG,         /*!< Frg Clock                                               */
    kCLOCK_AsyncApbClk, /*!< Async APB clock			 		     */
    kCLOCK_Adc,         /*!< ADC clock                                               */
    kCLOCK_ClockOut,    /*!< Clockout clock                                          */
    kCLOCK_Usart,       /*!< USART clock                                             */
    kCLOCK_Spi,         /*!< SPI clock                                               */
    kCLOCK_I2c,         /*!< I2C clock                                               */
} clock_name_t;

/*! @brief Clock Mux Switches
*  The encoding is as follows each connection identified is 64bits wide
*  starting from LSB upwards
*
*  [4 bits for choice, where 1 is A, 2 is B, 3 is C and 4 is D, 0 means end of descriptor] [8 bits mux ID]*
*
*/

#define MUX_A(m, choice) (((m) << 0) | ((choice + 1) << 8))
#define MUX_B(m, choice) (((m) << 12) | ((choice + 1) << 20))
#define MUX_C(m, choice) (((m) << 24) | ((choice + 1) << 32))
#define MUX_D(m, choice) (((m) << 36) | ((choice + 1) << 44))
#define MUX_E(m, choice) (((m) << 48) | ((choice + 1) << 56))

#define CM_MAINCLKSELA 0
#define CM_MAINCLKSELB 1
#define CM_ADCCLKSEL 3
#define CM_CLKOUTCLKSELA 5
#define CM_CLKOUTCLKSELB 6
#define CM_SYSPLLCLKSEL 8

#define CM_ASYNCAPA 30
#define CM_ASYNCAPB 31

typedef enum _clock_attach_id
{
    kIRC12M_to_MAIN_CLK = MUX_A(CM_MAINCLKSELA, 0) | MUX_B(CM_MAINCLKSELB, 0),
    kCLKIN_to_MAIN_CLK = MUX_A(CM_MAINCLKSELA, 1) | MUX_B(CM_MAINCLKSELB, 0),
    kWDT_OSC_to_MAIN_CLK = MUX_A(CM_MAINCLKSELA, 2) | MUX_B(CM_MAINCLKSELB, 0),
    kSYS_PLL_IN_to_MAIN_CLK = MUX_A(CM_MAINCLKSELB, 1),
    kSYS_PLL_OUT_to_MAIN_CLK = MUX_A(CM_MAINCLKSELB, 2),
    kRTC_OSC_to_MAIN_CLK = MUX_A(CM_MAINCLKSELB, 3),

    kMAIN_CLK_to_ADC_CLK = MUX_A(CM_ADCCLKSEL, 0),
    kSYS_PLL_OUT_to_ADC_CLK = MUX_A(CM_ADCCLKSEL, 1),
    kIRC12M_to_ADC_CLK = MUX_A(CM_ADCCLKSEL, 2),

    kMAIN_CLK_to_CLKOUT = MUX_A(CM_CLKOUTCLKSELA, 0) | MUX_B(CM_CLKOUTCLKSELB, 0),
    kCLKIN_to_CLKOUT = MUX_A(CM_CLKOUTCLKSELA, 1) | MUX_B(CM_CLKOUTCLKSELB, 0),
    kWDT_OSC_to_CLKOUT = MUX_A(CM_CLKOUTCLKSELA, 2) | MUX_B(CM_CLKOUTCLKSELB, 0),
    kIRC12M_to_CLKOUT = MUX_A(CM_CLKOUTCLKSELA, 3) | MUX_B(CM_CLKOUTCLKSELB, 0),
    kRTC_OSC_to_CLKOUT = MUX_A(CM_CLKOUTCLKSELB, 3),

    kIRC12M_to_SYS_PLL = MUX_A(CM_SYSPLLCLKSEL, 0),
    kCLKIN_to_SYS_PLL = MUX_A(CM_SYSPLLCLKSEL, 1),
    kRTC_OSC_to_SYS_PLL = MUX_A(CM_SYSPLLCLKSEL, 3),

    kIRC12M_to_ASYNC_APB = MUX_A(CM_ASYNCAPA, 0) | MUX_B(CM_ASYNCAPB, 3),
    kWDT_OSC_to_ASYNC_APB = MUX_A(CM_ASYNCAPA, 1) | MUX_B(CM_ASYNCAPB, 3),
    kMAIN_CLK_OUT_to_ASYNC_APB = MUX_A(CM_ASYNCAPB, 0),
    kCLKIN_to_ASYNC_APB = MUX_A(CM_ASYNCAPB, 1),
    kSYS_PLL_OUT_to_ASYNC_APB = MUX_A(CM_ASYNCAPB, 2),

    kIRC12M_to_USART = MUX_A(CM_ASYNCAPA, 0) | MUX_B(CM_ASYNCAPB, 3),
    kWDT_OSC_to_USART = MUX_A(CM_ASYNCAPA, 1) | MUX_B(CM_ASYNCAPB, 3),
    kMAIN_CLK_OUT_to_USART = MUX_A(CM_ASYNCAPB, 0),
    kCLKIN_to_USART = MUX_A(CM_ASYNCAPB, 1),
    kSYS_PLL_OUT_to_USART = MUX_A(CM_ASYNCAPB, 2),

    kIRC12M_to_SPI = MUX_A(CM_ASYNCAPA, 0) | MUX_B(CM_ASYNCAPB, 3),
    kWDT_OSC_to_SPI = MUX_A(CM_ASYNCAPA, 1) | MUX_B(CM_ASYNCAPB, 3),
    kMAIN_CLK_OUT_to_SPI = MUX_A(CM_ASYNCAPB, 0),
    kCLKIN_to_SPI = MUX_A(CM_ASYNCAPB, 1),
    kSYS_PLL_OUT_to_SPI = MUX_A(CM_ASYNCAPB, 2),

    kIRC12M_to_I2C = MUX_A(CM_ASYNCAPA, 0) | MUX_B(CM_ASYNCAPB, 3),
    kWDT_OSC_to_I2C = MUX_A(CM_ASYNCAPA, 1) | MUX_B(CM_ASYNCAPB, 3),
    kMAIN_CLK_OUT_to_I2C = MUX_A(CM_ASYNCAPB, 0),
    kCLKIN_to_I2C = MUX_A(CM_ASYNCAPB, 1),
    kSYS_PLL_OUT_to_I2C = MUX_A(CM_ASYNCAPB, 2),

    kNONE_to_NONE = 0x80000000U,
} clock_attach_id_t;

/*  Clock dividers */
typedef enum _clock_div_name
{
    kCLOCK_DivSystickClk = 0,
    kCLOCK_DivAhbClk = 8,
    kCLOCK_DivAdcClk = 10,
    kCLOCK_DivClkOut = 11,
} clock_div_name_t;

/**
 * @brief FLASH Access time definitions
 */
typedef enum _clock_flashtim
{
    kCLOCK_Flash1Cycle = 0, /*!< Flash accesses use 1 CPU clock */
    kCLOCK_Flash2Cycle,     /*!< Flash accesses use 2 CPU clocks */
    kCLOCK_Flash3Cycle,     /*!< Flash accesses use 3 CPU clocks */
    kCLOCK_Flash4Cycle,     /*!< Flash accesses use 4 CPU clocks */
    kCLOCK_Flash5Cycle,     /*!< Flash accesses use 5 CPU clocks */
    kCLOCK_Flash6Cycle,     /*!< Flash accesses use 6 CPU clocks */
} clock_flashtim_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*! @brief  Set Asyc clock divider
 *  @param  divided_by_value  : Asyc clock divider
 */
static inline void Clock_SetAsyncClkDiv(uint32_t divided_by_value)
{
    ASYNC_SYSCON->ASYNCCLKDIV = divided_by_value; /* if divided_by_value is 0, clock will be disable*/
}

/*  @brief Enable the clock for specific IP.
 *  @param name  Which clock to enable, see clock_ip_name_t.
 */
static inline void CLOCK_EnableClock(clock_ip_name_t clk)
{
    uint32_t index = CLK_GATE_ABSTRACT_REG_OFFSET(clk);
    if (index < 2)
    {
        SYSCON->AHBCLKCTRLSET[index] = (1U << CLK_GATE_ABSTRACT_BITS_SHIFT(clk));
    }
    else
    {
        ASYNC_SYSCON->ASYNCAPBCLKCTRLSET = (1U << CLK_GATE_ABSTRACT_BITS_SHIFT(clk));
    }
}

/*  @brief Disable the clock for specific IP.
 *  @param name  Which clock to enable, see clock_ip_name_t.
 */
static inline void CLOCK_DisableClock(clock_ip_name_t clk)
{
    uint32_t index = CLK_GATE_ABSTRACT_REG_OFFSET(clk);
    if (index < 2)
    {
        SYSCON->AHBCLKCTRLCLR[index] = (1U << CLK_GATE_ABSTRACT_BITS_SHIFT(clk));
    }
    else
    {
        ASYNC_SYSCON->ASYNCAPBCLKCTRLCLR = (1U << CLK_GATE_ABSTRACT_BITS_SHIFT(clk));
    }
}

/*! @brief  Enables and disables PLL bypass mode
 *  @brief  bypass  : true to bypass PLL (PLL output = PLL input, false to disable bypass
 *  @return System PLL output clock rate
 */
static inline void CLOCK_SetBypassPLL(bool bypass)
{
    if (bypass)
    {
        SYSCON->SYSPLLCTRL |= (1UL << SYSCON_SYSPLLCTRL_BYPASS_SHIFT);
    }
    else
    {
        SYSCON->SYSPLLCTRL &= ~(1UL << SYSCON_SYSPLLCTRL_BYPASS_SHIFT);
    }
}

/*! @brief  Check if PLL is locked or not
 *  @return true if the PLL is locked, false if not locked
 */
static inline bool CLOCK_IsSystemPLLLocked(void)
{
    return (bool)((SYSCON->SYSPLLSTAT & SYSCON_SYSPLLSTAT_LOCK_MASK) != 0U);
}

/**
 * @brief   Set FLASH memory access time in clocks
 * @param   clks    : Clock cycles for FLASH access
 * @return  Nothing
 */
static inline void CLOCK_SetFLASHAccessCycles(clock_flashtim_t clks)
{
    uint32_t tmp;
    tmp = SYSCON->FLASHCFG & ~(SYSCON_FLASHCFG_FLASHTIM_MASK);
    /* Don't alter lower bits */
    SYSCON->FLASHCFG = tmp | ((uint32_t)clks << SYSCON_FLASHCFG_FLASHTIM_SHIFT);
}

/**
 * @brief	Configure the clock selection muxes.
 * @param	connection	: Clock to be configured.
 * @return	Nothing
 */
void CLOCK_AttachClk(clock_attach_id_t connection);

/**
 * @brief	Setup peripheral clock dividers.
 * @param	div_name	: Clock divider name
 * @param divided_by_value: Value to be divided
   @param reset, not used
 * @return	Nothing
 */
void CLOCK_SetClkDiv(clock_div_name_t div_name, uint32_t divided_by_value, bool reset);

/**
 * @brief	Set the flash wait states for the input freuqency.
 * @param	iFreq	: Input frequency
 * @return	Nothing
 */
void CLOCK_SetFLASHAccessCyclesForFreq(uint32_t iFreq);

/*! @brief  Return Frequency of Core clock
 *  @return Frequency of Core clock
 */
uint32_t CLOCK_GetCoreClkFreq(void);

/*! @brief  Return Frequency of Bus clock
 *  @return Frequency of Bus clock
 */
uint32_t CLOCK_GetBusClkFreq(void);

/*! @brief	Return Frequency of selected clock
 *  @return	Frequency of selected clock
 */
uint32_t CLOCK_GetFreq(clock_name_t clockName);

/*! @brief	Return Frequency of External Clock
 *  @return	Frequency of External Clock. If no external clock is used returns 0.
 */
uint32_t CLOCK_GetExtClkFreq(void);

/*! @brief	Return Frequency of Watchdog Oscillator
 *  @return	Frequency of Watchdog Oscillator
 */
uint32_t CLOCK_GetWdtOscFreq(void);

/*! @brief	Return Frequency of PLL
 *  @return	Frequency of PLL
 */
uint32_t CLOCK_GetPllOutFreq(void);

/*! @brief	Return Frequency of 32kHz osc
 *  @return	Frequency of 32kHz osc
 */
uint32_t CLOCK_GetOsc32KFreq(void);

/*! @brief  Return Frequency of ADC
 *  @return Frequency of ADC
 */
uint32_t CLOCK_GetAdcClkFreq(void);

/*! @brief  Return Frequency of clock out
 *  @return Frequency of clock out
 */
uint32_t CLOCK_GetClockOutClkFreq(void);

/*! @brief  Return Frequency of IRC
 *  @return Frequency of IRC
 */
uint32_t CLOCK_GetIrc12MFreq(void);

/*! @brief  Return Frequency of PLL input
 *  @return Frequency of PLL input
 */
uint32_t CLOCK_GetPllInFreq(void);

/*! @brief	Return Frequency of Core System
 *  @return	Frequency of Core System
 */
uint32_t CLOCK_GetMainClkFreq(void);

/*! @brief	Return Frequency of Asynchronous APB Clock
 *  @return	Frequency of Asynchronous APB Clock Clock
 */
uint32_t CLOCK_GetAsyncApbClkFreq(void);

/*! @brief  Set Frequency of FRG
 *  @return status of the setting, true: setting successful, false: setting fail
 */
bool CLOCK_SetFRGClock(uint32_t freq);

/*! @brief	Return System PLL input clock rate
 *  @return	System PLL input clock rate
 */
uint32_t CLOCK_GetSystemPLLInClockRate(void);

/*! @brief	Return System PLL output clock rate
 *  @param	recompute	: Forces a PLL rate recomputation if true
 *  @return	System PLL output clock rate
 *  @note	The PLL rate is cached in the driver in a variable as
 *  the rate computation function can take some time to perform. It
 *  is recommended to use 'false' with the 'recompute' parameter.
 */
uint32_t CLOCK_GetSystemPLLOutClockRate(bool recompute);

/*! @brief Store the current PLL rate
 *  @param	rate: Current rate of the PLL
 *  @return	Nothing
 **/
void CLOCK_SetStoredPLLClockRate(uint32_t rate);

/*! @brief PLL configuration structure flags for 'flags' field
 * These flags control how the PLL configuration function sets up the PLL setup structure.
 *
 * When the PLL_CONFIGFLAG_USEINRATE flag is selected, the 'InputRate' field in the
 * configuration structure must be assigned with the expected PLL frequency. If the
 * PLL_CONFIGFLAG_USEINRATE is not used, 'InputRate' is ignored in the configuration
 * function and the driver will determine the PLL rate from the currently selected
 * PLL source. This flag might be used to configure the PLL input clock more accurately
 * when using the WDT oscillator or a more dyanmic CLKIN source.
 *
 * When the PLL_CONFIGFLAG_FORCENOFRACT flag is selected, the PLL hardware for the
 * automatic bandwidth selection, Spread Spectrum (SS) support, and fractional M-divider
 * are not used
 */
#define PLL_CONFIGFLAG_USEINRATE (1U << 0U) /*!< Flag to use InputRate in PLL configuration structure for setup */
#define PLL_CONFIGFLAG_FORCENOFRACT                                                                                   \
    (1U << 2U) /*!< Force non-fractional output mode, PLL output will not use the fractional, automatic bandwidth, or \
                  \  \                                                                                                \
                    \  \ \ \                                                                                                                  \
                      \ \ \ \ \ \ \                                                                                                                    \
                         SS hardware */

/*! @brief PLL Spread Spectrum (SS) Programmable modulation frequency
 * See (MF) field in the SYSPLLSSCTRL1 register in the UM.
 */
typedef enum _ss_progmodfm
{
    kSS_MF_512 = (0U << 20U), /*!< Nss = 512 (fm ?= 3.9 - 7.8 kHz) */
    kSS_MF_384 = (1U << 20U), /*!< Nss = 384 (fm ?= 5.2 - 10.4 kHz) */
    kSS_MF_256 = (2U << 20U), /*!< Nss = 256 (fm ?= 7.8 - 15.6 kHz) */
    kSS_MF_128 = (3U << 20U), /*!< Nss = 128 (fm ?= 15.6 - 31.3 kHz) */
    kSS_MF_64 = (4U << 20U),  /*!< Nss = 64 (fm ?= 32.3 - 64.5 kHz) */
    kSS_MF_32 = (5U << 20U),  /*!< Nss = 32 (fm ?= 62.5- 125 kHz) */
    kSS_MF_24 = (6U << 20U),  /*!< Nss = 24 (fm ?= 83.3- 166.6 kHz) */
    kSS_MF_16 = (7U << 20U)   /*!< Nss = 16 (fm ?= 125- 250 kHz) */
} ss_progmodfm_t;

/*! @brief PLL Spread Spectrum (SS) Programmable frequency modulation depth
 * See (MR) field in the SYSPLLSSCTRL1 register in the UM.
 */
typedef enum _ss_progmoddp
{
    kSS_MR_K0 = (0U << 23U),   /*!< k = 0 (no spread spectrum) */
    kSS_MR_K1 = (1U << 23U),   /*!< k = 1 */
    kSS_MR_K1_5 = (2U << 23U), /*!< k = 1.5 */
    kSS_MR_K2 = (3U << 23U),   /*!< k = 2 */
    kSS_MR_K3 = (4U << 23U),   /*!< k = 3 */
    kSS_MR_K4 = (5U << 23U),   /*!< k = 4 */
    kSS_MR_K6 = (6U << 23U),   /*!< k = 6 */
    kSS_MR_K8 = (7U << 23U)    /*!< k = 8 */
} ss_progmoddp_t;

/*! @brief PLL Spread Spectrum (SS) Modulation waveform control
 * See (MC) field in the SYSPLLSSCTRL1 register in the UM.<br>
 * Compensation for low pass filtering of the PLL to get a triangular
 * modulation at the output of the PLL, giving a flat frequency spectrum.
 */
typedef enum _ss_modwvctrl
{
    kSS_MC_NOC = (0U << 26U),  /*!< no compensation */
    kSS_MC_RECC = (2U << 26U), /*!< recommended setting */
    kSS_MC_MAXC = (3U << 26U), /*!< max. compensation */
} ss_modwvctrl_t;

/*! @brief PLL configuration structure
 *
 * This structure can be used to configure the settings for a PLL
 * setup structure. Fill in the desired configuration for the PLL
 * and call the PLL setup function to fill in a PLL setup structure.
 */
typedef struct _pll_config
{
    uint32_t desiredRate; /*!< Desired PLL rate in Hz */
    uint32_t inputRate;   /*!< PLL input clock in Hz, only used if PLL_CONFIGFLAG_USEINRATE flag is set */
    uint32_t flags;       /*!< PLL configuration flags, Or'ed value of PLL_CONFIGFLAG_* definitions */
    ss_progmodfm_t ss_mf; /*!< SS Programmable modulation frequency, only applicable when not using
                             PLL_CONFIGFLAG_FORCENOFRACT flag */
    ss_progmoddp_t ss_mr; /*!< SS Programmable frequency modulation depth, only applicable when not using
                             PLL_CONFIGFLAG_FORCENOFRACT flag */
    ss_modwvctrl_t
        ss_mc; /*!< SS Modulation waveform control, only applicable when not using PLL_CONFIGFLAG_FORCENOFRACT flag */
    bool mfDither; /*!< false for fixed modulation frequency or true for dithering, only applicable when not using
                      PLL_CONFIGFLAG_FORCENOFRACT flag */

} pll_config_t;

/*! @brief PLL setup structure flags for 'flags' field
* These flags control how the PLL setup function sets up the PLL
*/
#define PLL_SETUPFLAG_POWERUP (1U << 0U)  /*!< Setup will power on the PLL after setup */
#define PLL_SETUPFLAG_WAITLOCK (1U << 1U) /*!< Setup will wait for PLL lock, implies the PLL will be pwoered on */
#define PLL_SETUPFLAG_ADGVOLT (1U << 2U)  /*!< Optimize system voltage for the new PLL rate */
#define PLL_SETUPFLAG_USEFEEDBACKDIV2 (1U << 3U) /*!< Use feedback divider by 2 in divider path */

/*! @brief PLL setup structure
* This structure can be used to pre-build a PLL setup configuration
* at run-time and quickly set the PLL to the configuration. It can be
* populated with the PLL setup function. If powering up or waiting
* for PLL lock, the PLL input clock source should be configured prior
* to PLL setup.
*/
typedef struct _pll_setup
{
    uint32_t syspllctrl;      /*!< PLL control register SYSPLLCTRL */
    uint32_t syspllndec;      /*!< PLL NDEC register SYSPLLNDEC */
    uint32_t syspllpdec;      /*!< PLL PDEC register SYSPLLPDEC */
    uint32_t syspllssctrl[2]; /*!< PLL SSCTL registers SYSPLLSSCTRL */
    uint32_t pllRate;         /*!< Acutal PLL rate */
    uint32_t flags;           /*!< PLL setup flags, Or'ed value of PLL_SETUPFLAG_* definitions */
} pll_setup_t;

/*! @brief PLL status definitions
 */
typedef enum _pll_error
{
    kStatus_PLL_Success = MAKE_STATUS(kStatusGroup_Generic, 0),        /*!< PLL operation was successful */
    kStatus_PLL_OutputTooLow = MAKE_STATUS(kStatusGroup_Generic, 1),   /*!< PLL output rate request was too low */
    kStatus_PLL_OutputTooHigh = MAKE_STATUS(kStatusGroup_Generic, 2),  /*!< PLL output rate request was too high */
    kStatus_PLL_InputTooLow = MAKE_STATUS(kStatusGroup_Generic, 3),    /*!< PLL input rate is too low */
    kStatus_PLL_InputTooHigh = MAKE_STATUS(kStatusGroup_Generic, 4),   /*!< PLL input rate is too high */
    kStatus_PLL_OutsideIntLimit = MAKE_STATUS(kStatusGroup_Generic, 5) /*!< Requested output rate isn't possible */
} pll_error_t;

/*! @brief	Return System PLL output clock rate from setup structure
 *  @param	pSetup	: Pointer to a PLL setup structure
 *  @return	System PLL output clock rate calculated from the setup structure
 */
uint32_t CLOCK_GetSystemPLLOutFromSetup(pll_setup_t *pSetup);

/*! @brief	Set PLL output based on the passed PLL setup data
 *  @param	pControl	: Pointer to populated PLL control structure to generate setup with
 *  @param	pSetup		: Pointer to PLL setup structure to be filled
 *  @return	PLL_ERROR_SUCCESS on success, or PLL setup error code
 *  @note	Actual frequency for setup may vary from the desired frequency based on the
 *  accuracy of input clocks, rounding, non-fractional PLL mode, etc.
 */
pll_error_t CLOCK_SetupPLLData(pll_config_t *pControl, pll_setup_t *pSetup);

/*! @brief	Set PLL output from PLL setup structure (precise frequency)
 *  @param	pSetup	: Pointer to populated PLL setup structure
 *  @param  flagcfg : Flag configuration for PLL config structure
 *  @return	PLL_ERROR_SUCCESS on success, or PLL setup error code
 *  @note	This function will power off the PLL, setup the PLL with the
 *  new setup data, and then optionally powerup the PLL, wait for PLL lock,
 *  and adjust system voltages to the new PLL rate. The function will not
 *  alter any source clocks (ie, main systen clock) that may use the PLL,
 *  so these should be setup prior to and after exiting the function.
 */
pll_error_t CLOCK_SetupSystemPLLPrec(pll_setup_t *pSetup, uint32_t flagcfg);

/*! @brief	Set PLL output from PLL setup structure (precise frequency)
 *  @param	pSetup	: Pointer to populated PLL setup structure
 *  @return	kStatus_PLL_Success on success, or PLL setup error code
 *  @note	This function will power off the PLL, setup the PLL with the
 *  new setup data, and then optionally powerup the PLL, wait for PLL lock,
 *  and adjust system voltages to the new PLL rate. The function will not
 *  alter any source clocks (ie, main systen clock) that may use the PLL,
 *  so these should be setup prior to and after exiting the function.
 */
pll_error_t CLOCK_SetPLLFreq(const pll_setup_t *pSetup);

/*! @brief	Set PLL output based on the multiplier and input frequency
 *  @param	multiply_by	: multiplier
 *  @param	input_freq	: Clock input frequency of the PLL
 *  @return	Nothing
 *  @note	Unlike the Chip_Clock_SetupSystemPLLPrec() function, this
 *  function does not disable or enable PLL power, wait for PLL lock,
 *  or adjust system voltages. These must be done in the application.
 *  The function will not alter any source clocks (ie, main systen clock)
 *  that may use the PLL, so these should be setup prior to and after
 *  exiting the function.
 */
void CLOCK_SetupSystemPLLMult(uint32_t multiply_by, uint32_t input_freq);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

/*! @} */

#endif /* _FSL_CLOCK_H_ */
