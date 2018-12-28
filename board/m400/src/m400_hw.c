/*
 / _____)             _              | |
 ( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
 (______/|_____)_|_|_| \__)_____)\____)_| |_|
 (C)2013 Semtech

 Description: Target board general functions implementation

 License: Revised BSD License, see LICENSE.TXT file include in the project

 Maintainer: Miguel Luis and Gregory Cristian
 */
/*******************************************************************************
 * @file    eml3047_hw.c
 * @author  MCD Application Team
 * @version V1.1.1
 * @date    01-June-2017
 * @brief   system hardware driver
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V.
 * All rights reserved.</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted, provided that the following conditions are met:
 *
 * 1. Redistribution of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of STMicroelectronics nor the names of other
 *    contributors to this software may be used to endorse or promote products
 *    derived from this software without specific written permission.
 * 4. This software, including modifications and/or derivative works of this
 *    software, must execute solely and exclusively on microcontroller or
 *    microprocessor devices manufactured by or for STMicroelectronics.
 * 5. Redistribution and use of this software other than as permitted under
 *    this license is void and will automatically terminate your rights under
 *    this license.
 *
 * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
 * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
 * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "hw.h"
#include "radio.h"
#include "debug.h"
#include "uart-board.h"
#include "sx1276.h"

/* Force include of hal adc in order to inherite HAL_ADC_STATE_xxx */
#include "stm32l0xx_hal_dma.h"
#include "stm32l0xx_hal_adc.h"

#include <k_api.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/**
 * @brief  Unique Devices IDs register set ( STM32L0xxx )
 */
#define ID1 (UID_BASE)
#define ID2 (UID_BASE + 0x4)
#define ID3 (UID_BASE + 0x14)

/**
 * @brief  ADC Vbat measurement constants
 */
/* Internal voltage reference, parameter VREFINT_CAL*/
#define VREFINT_CAL ((uint16_t *)((uint32_t)0x1FF80078))
#define LORAWAN_MAX_BAT 254

/**
 * @brief  Delay for ADC stabilization time. Maximum delay is 1us
 *         (refer to device datasheet, parameter tSTART).
 * @note   Unit: us
 */
#define ADC_STAB_DELAY_US ((uint32_t)1U)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/**
 * @brief  State of the ADC
 */
__IO uint32_t adc_state;

/**
 * @brief  Flag to indicate if the ADC is Initialized
 */
static FlagStatus AdcInitialized = RESET;

/**
 * @brief  Flag to indicate if the MCU is Initialized
 */
static FlagStatus McuInitialized = RESET;

/**
 * @brief Fix values of the ADC configuration
 */
static const uint32_t adcinit_ClockPrescaler = LL_ADC_CLOCK_SYNC_PCLK_DIV2;
static const uint32_t adcinit_LowPowerAutoPowerOff = LL_ADC_LP_MODE_NONE;
static const uint32_t adcinit_LowPowerFrequencyMode = LL_ADC_CLOCK_FREQ_MODE_LOW;
static const uint32_t adcinit_LowPowerAutoWait = LL_ADC_LP_MODE_NONE;
static const uint32_t adcinit_Resolution = LL_ADC_RESOLUTION_12B;
static const uint32_t adcinit_SamplingTime = LL_ADC_SAMPLINGTIME_160CYCLES_5;
static const uint32_t adcinit_ScanConvMode = LL_ADC_REG_SEQ_SCAN_DIR_FORWARD;
static const uint32_t adcinit_DataAlign = LL_ADC_DATA_ALIGN_RIGHT;
static const uint32_t adcinit_ContinuousConvMode = LL_ADC_REG_CONV_SINGLE;
static const uint32_t adcinit_ExternalTrigConvEdge = 0x0; /* 0 if no IT, LL_ADC_REG_TRIG_EXT_RISING... otherwise */
static const uint32_t adcinit_EOCSelection = LL_ADC_FLAG_EOC; /* same as ADC_EOC_SINGLE_CONV */
static const uint32_t adcinit_DMAContinuousRequests = ADC_CFGR1_DMACFG;
static const uint32_t adcinit_Overrun = LL_ADC_REG_OVR_DATA_PRESERVED;
static const uint32_t adcinit_ExternalTrigConv = LL_ADC_REG_TRIG_SOFTWARE;
static ADC_TypeDef *adcinit_Instance = ADC1;
#ifdef USE_FULL_ASSERT
static const uint32_t adcinit_OversamplingMode = DISABLE;
static const uint32_t adcinit_DiscontinuousConvMode = DISABLE;
#endif

/* Private function prototypes -----------------------------------------------*/
/* following provided LL function lacks a header in a .h file */
uint32_t RCC_PLL_GetFreqDomain_SYS( void );

/**
 * @brief  This function Initializes the hardware Ios
 * @param  None
 * @retval None
 */
static void HW_IoInit( void );

/**
 * @brief  This function Deinitializes the hardware Ios
 * @param  None
 * @retval None
 */
static void HW_IoDeInit( void );

/**
 * @brief  Initializes the RCC Oscillators according to the following fixed parameters:
 *            RCC_OscInitTypeDef RCC_OscInitStruct;
 *            RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSI;
 *            RCC_OscInitStruct.HSEState            = RCC_HSE_OFF;
 *            RCC_OscInitStruct.HSIState            = RCC_HSI_ON;
 *            RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
 *            RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_ON;
 *            RCC_OscInitStruct.PLL.PLLSource       = RCC_PLLSOURCE_HSI;
 *            RCC_OscInitStruct.PLL.PLLMUL          = RCC_PLLMUL_6;
 *            RCC_OscInitStruct.PLL.PLLDIV          = RCC_PLLDIV_3;
 * @param  None
 * @retval None
 * @note   LL implementation of HAL_RCC_OscConfig() for a specific implementation
 */
static void HW_RCC_OscConfig( void );

/**
 * @brief  Initializes the CPU, AHB and APB busses clocks according to the following fixed parameters:
 *            RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
 *            RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
 *            RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
 *            RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
 *            RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
 * @param  None
 * @retval None
 * @note   LL implementation of HAL_RCC_ClockConfig() for a specific implementation
 */
static void HW_RCC_ClockConfig( void );

/**
 * @brief  Return whether or not the ADC is enabled
 * @param  None
 * @retval 1 if enabled, 0 otherwise
 */
static uint32_t HW_ADC_IS_ENABLE( void );

/**
 * @brief  Initialize the ADC according to the adcinit_Instance parameters
 *         It follows the HAL_ADC_Init() function
 * @param  None
 * @retval None
 */
static void HW_ADC_Init( void );

/**
 * @brief  Delay during given micro seconds
 * @param  The number of micro seconds to wait
 * @retval None
 */
static void HW_ADC_DelayMicroSecond( uint32_t microSecond );

/**
 * @brief  Deselect all the channels.
 *         This is the same as HAL_ADC_ConfigChannel(), with channel=ADC_CHANNEL_MASK
 *         and rank=ADC_RANK_NONE
 * @param  none
 * @retval none
 */
static void HW_ADC_DeselectChannel( void );

/**
 * @brief  Select all the provided channel.
 *         This is the same as HAL_ADC_ConfigChannel(), with rank=ADC_RANK_CHANNEL_NUMBER
 * @param  The channel to select
 * @retval None
 */
static void HW_ADC_SelectChannel( uint32_t channel );

/**
 * @brief  Enable the ADC.
 * @note   Prerequisite condition to use this function: ADC must be disabled
 *         and voltage regulator must be enabled (done into HAL_ADC_Init()).
 * @note   If low power mode AutoPowerOff is enabled, power-on/off phases are
 * @param  None
 * @retval None
 */
static ErrorStatus HW_ADC_Enable( void );

/**
 * @brief  Enables ADC, starts conversion of regular group.
 *         Interruptions enabled in this function: None.
 * @param  None
 * @retval None
 */
static void HW_ADC_Start( void );

/**
 * @brief  Wait for regular group conversion to be completed.
 * @note   ADC conversion flags EOS (end of sequence) and EOC (end of
 *         conversion) are cleared by this function, with an exception:
 *         if low power feature "LowPowerAutoWait" is enabled, flags are
 *         not cleared to not interfere with this feature until data register
 *         is read using function HAL_ADC_GetValue().
 * @note   This function cannot be used in a particular setup: ADC configured
 *         in DMA mode and polling for end of each conversion (ADC init
 *         parameter "EOCSelection" set to ADC_EOC_SINGLE_CONV).
 *         In this case, DMA resets the flag EOC and polling cannot be
 *         performed on each conversion. Nevertheless, polling can still
 *         be performed on the complete sequence (ADC init
 *         parameter "EOCSelection" set to ADC_EOC_SEQ_CONV).
 * @param  none
 * @retval none
 */
static void HW_ADC_PollForConversion( void );

/**
 * @brief  This function initializes the ADC
 * @param  none
 * @retval none
 */
static void HW_AdcInit( void );

/**
 * @brief  This function reads a given channel
 * @param  Channel to read
 * @retval Value that was read
 */
static uint16_t HW_AdcReadChannel( uint32_t Channel );

/* Exported functions ---------------------------------------------------------*/
Gpio_t       g_stLedGpio;

#define SYS_LED_PORT                   APP_LED_PORT
#define SYS_LED_PIN                    APP_LED_PIN

#define SYS_LED_GPIO                   PB_0

void SYS_LED_ON( void )
{
    SYS_LED_PORT->BSRR = SYS_LED_PIN;
}

void SYS_LED_OFF( void )
{
    SYS_LED_PORT->BRR = SYS_LED_PIN;
}

void SYS_LED_Deinit( void )
{
    GpioInit( &g_stLedGpio, SYS_LED_GPIO, PIN_ANALOGIC, PIN_NO_PULL , PIN_NO_PULL, 0);
}

void SYS_LED_Init( void )
{
    GpioInit( &g_stLedGpio, SYS_LED_GPIO, PIN_OUTPUT, PIN_PUSH_PULL , PIN_PUSH_PULL, 0);
    SYS_LED_OFF( );
}

void HW_Reset( void )
{
    NVIC_SystemReset();
}

void HW_Init( void )
{
    if ( McuInitialized == RESET )
    {
        HW_GpioInit( );
#if defined(USE_BOOTLOADER)
        /* Set the Vector Table base location at 0x3000 */
        NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x3000);
#endif

        SX1276IoInit( );
        SpiInit(&SX1276.Spi, SPI_1, RADIO_MOSI, RADIO_MISO, RADIO_SCLK, RADIO_NSS);
        RtcInit( );

#ifdef LORA_DEBUG
        vcom_Init( );
        SYS_LED_Init( );
#endif

        McuInitialized = SET;
    }
}

void HW_DeInit( void )
{
    HW_SPI_DeInit( );
    SX1276IoDeInit( );

#ifdef LORA_DEBUG
    vcom_DeInit( );
    SYS_LED_Deinit( );
#endif

    McuInitialized = RESET;
}

void HW_GpioInit( void )
{
    Gpio_t      st_gpioInit = {0};
    uint32_t    i;

    /*  Configure all GPIO as analog to reduce current consumption on non used IOs */
    /*  Clocks are enabled in HW_GPIO_Init on GPIO A, B, C and H*/

    /*  All GPIOs except debug pins (SWCLK and SWD) */
    for (i = PA_0; i <PA_13; i++) {
        GpioInit(&st_gpioInit, i, PIN_ANALOGIC, PIN_PUSH_PULL, PIN_NO_PULL, 0);
    }

    /*  All GPIOs */
    GpioInit(&st_gpioInit, PA_15, PIN_ANALOGIC, PIN_PUSH_PULL, PIN_NO_PULL, 0);

    for (i = PB_0; i <= PB_15; i++) {
        GpioInit(&st_gpioInit, i, PIN_ANALOGIC, PIN_PUSH_PULL, PIN_NO_PULL, 0);
    }
    for (i = PC_0; i <= PC_15; i++) {
        GpioInit(&st_gpioInit, i, PIN_ANALOGIC, PIN_PUSH_PULL, PIN_NO_PULL, 0);
    }
    for (i = PH_0; i <= PH_15; i++) {
        GpioInit(&st_gpioInit, i, PIN_ANALOGIC, PIN_PUSH_PULL, PIN_NO_PULL, 0);
    }

    /* Disable GPIOs clock */
    LL_IOP_GRP1_DisableClock( LL_IOP_GRP1_PERIPH_GPIOA );
    LL_IOP_GRP1_DisableClock( LL_IOP_GRP1_PERIPH_GPIOB );
    LL_IOP_GRP1_DisableClock( LL_IOP_GRP1_PERIPH_GPIOC );
    LL_IOP_GRP1_DisableClock( LL_IOP_GRP1_PERIPH_GPIOH );
}

void SystemClock_Config( void )
{
    HW_RCC_OscConfig( );

    /* Set Voltage scale1 as MCU will run at 32MHz */
    LL_APB1_GRP1_EnableClock( LL_APB1_GRP1_PERIPH_PWR );
    LL_PWR_SetRegulVoltageScaling( LL_PWR_REGU_VOLTAGE_SCALE1 );

    /* Poll VOSF bit of in PWR_CSR. Wait until it is reset to 0 */
    while ( LL_PWR_IsActiveFlag_VOSF( ) != RESET )
    {
    };

    /*
     * Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     * clocks dividers
     */
    HW_RCC_ClockConfig( );
}

uint32_t HW_GetRandomSeed( void )
{
    return ((*(uint32_t *) ID1) ^ (*(uint32_t *) ID2) ^ (*(uint32_t *) ID3));
}

void HW_GetUniqueId( uint8_t *id )
{
    uint32_t id1 = *((uint32_t *) ID1);
    uint32_t id2 = *((uint32_t *) ID2);
    uint32_t id3 = *((uint32_t *) ID3);
    uint32_t id13 = id1 + id3;

    id[7] = id13 >> 24;
    id[6] = id13 >> 16;
    id[5] = id13 >> 8;
    id[4] = id13;
    id[3] = id2 >> 24;
    id[2] = id2 >> 16;
    id[1] = id2 >> 8;
    id[0] = id2;
}

uint8_t HW_GetBatteryLevel( void )
{
    uint8_t batteryLevel = 0;
    uint16_t measuredLevel = 0;
    uint32_t batteryLevelmV;

    measuredLevel = HW_AdcReadChannel( LL_ADC_CHANNEL_VREFINT );

    if ( measuredLevel == 0 )
    {
        batteryLevelmV = 0;
    }
    else
    {
        batteryLevelmV = (((uint32_t) VDDA_VREFINT_CAL * (*VREFINT_CAL)) / measuredLevel);
    }
    if ( batteryLevelmV > VDD_BAT )
    {
        batteryLevel = LORAWAN_MAX_BAT;
    }
    else if ( batteryLevelmV < VDD_MIN )
    {
        batteryLevel = 0;
    }
    else
    {
        batteryLevel = (((uint32_t) (batteryLevelmV - VDD_MIN) * LORAWAN_MAX_BAT) / (VDD_BAT - VDD_MIN));
    }
    return batteryLevel;
}

void HW_EnterStopMode( void )
{
    uint32_t tmpreg = 0U;
    CPSR_ALLOC();
    RHINO_CPU_INTRPT_DISABLE();

    HW_IoDeInit( );

    /*clear wake up flag*/
    LL_PWR_ClearFlag_WU( );

    /* Disable the UART Data Register not empty Interrupt */
    //LL_LPUART_DisableIT_RXNE( UARTX );

    RHINO_CPU_INTRPT_ENABLE();

    /* Enter Stop Mode - is a LL implementatin of HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI) */
    /* Select the regulator state in Stop mode ---------------------------------*/
    tmpreg = PWR->CR;

    /* Clear PDDS and LPDS bits */
    CLEAR_BIT( tmpreg, (PWR_CR_PDDS | PWR_CR_LPSDSR) );

    /* Set LPSDSR bit according to PWR_Regulator value */
    SET_BIT( tmpreg, LL_PWR_REGU_LPMODES_LOW_POWER );

    /* Store the new value */
    PWR->CR = tmpreg;

    /* Set SLEEPDEEP bit of Cortex System Control Register */
    LL_LPM_EnableDeepSleep( );

    /* Select Stop mode entry --------------------------------------------------*/
    __WFI( );

    /* Reset SLEEPDEEP bit of Cortex System Control Register */
    LL_LPM_EnableSleep( );
}

void HW_ExitStopMode( void )
{
    /* Disable IRQ while the MCU is not running on HSI */
    CPSR_ALLOC();
    RHINO_CPU_INTRPT_DISABLE();

    /* After wake-up from STOP reconfigure the system clock */
    /* Enable HSI */
    LL_RCC_HSI_Enable( );

    /* Wait till HSI is ready */
    while ( LL_RCC_HSI_IsReady( ) == RESET )
    {
        ;
    }

    /* Enable PLL */
    LL_RCC_PLL_Enable( );
    /* Wait till PLL is ready */
    while ( LL_RCC_PLL_IsReady( ) == RESET )
    {
        ;
    }

    /* Select PLL as system clock source */
    LL_RCC_SetSysClkSource( LL_RCC_SYS_CLKSOURCE_PLL );

    /* Wait till PLL is used as system clock source */
    while ( LL_RCC_GetSysClkSource( ) != LL_RCC_SYS_CLKSOURCE_STATUS_PLL )
    {
        ;
    }

    /*initilizes the peripherals*/
    HW_IoInit( );

    RHINO_CPU_INTRPT_ENABLE();
}

void HW_EnterSleepMode( void )
{
    /* Is an implementation of HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI) */
    uint32_t tmpreg = 0U;

    /* Select the regulator state in Sleep mode ---------------------------------*/
    tmpreg = PWR->CR;

    /* Clear PDDS and LPDS bits */
    CLEAR_BIT( tmpreg, (PWR_CR_PDDS | PWR_CR_LPSDSR) );

    /* Set LPSDSR bit according to PWR_Regulator value */
    SET_BIT( tmpreg, LL_PWR_REGU_LPMODES_MAIN );

    /* Store the new value */
    PWR->CR = tmpreg;

    /* Clear SLEEPDEEP bit of Cortex System Control Register */
    LL_LPM_EnableSleep( );

    /* Select SLEEP mode entry -------------------------------------------------*/
    __WFI( );
}

/* Private functions ---------------------------------------------------------*/

void HW_SPI_IoInit( void )
{
    GpioInit(&SX1276.Spi.Sclk, RADIO_SCLK, PIN_ALTERNATE_FCT, PIN_PUSH_PULL, PIN_PULL_UP, 0);
    GpioInit(&SX1276.Spi.Miso, RADIO_MISO, PIN_ALTERNATE_FCT, PIN_PUSH_PULL, PIN_PULL_UP, 0);
    GpioInit(&SX1276.Spi.Mosi, RADIO_MOSI, PIN_ALTERNATE_FCT, PIN_PUSH_PULL, PIN_PULL_UP, 0);
    GpioInit(&SX1276.Spi.Nss, RADIO_NSS, PIN_OUTPUT, PIN_PUSH_PULL, PIN_PULL_UP, 1);
}

void HW_SPI_IoDeInit( void )
{
    GpioInit(&SX1276.Spi.Sclk, RADIO_SCLK, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0);
    GpioInit(&SX1276.Spi.Miso, RADIO_MISO, PIN_OUTPUT, PIN_PUSH_PULL, PIN_PULL_DOWN, 0);
    GpioInit(&SX1276.Spi.Mosi, RADIO_MOSI, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0);
    GpioInit(&SX1276.Spi.Nss, RADIO_NSS, PIN_OUTPUT, PIN_PUSH_PULL, PIN_PULL_UP, 1);

    GpioInit(&SX1276.Reset, RADIO_RESET, PIN_ANALOGIC, PIN_PUSH_PULL, PIN_NO_PULL, 0);
}


static void HW_IoInit( void )
{
    HW_SPI_IoInit( );
    SX1276IoInit( );
#ifdef LORA_DEBUG
    vcom_Init( );
    SYS_LED_Init( );
#endif
    //vcom_IoInit();
}

static void HW_IoDeInit( void )
{
    SX1276IoDeInit( );
#ifdef LORA_DEBUG
    vcom_DeInit( );
    SYS_LED_Deinit( );
#endif
    //vcom_IoDeInit();

    SpiDeInit(&SX1276.Spi);
}

static void HW_RCC_OscConfig( void )
{
    uint32_t sys_clk;

    /* HSI configuration as the oscillator type */
    /* When the HSI is used as system clock it will not disabled */
    sys_clk = LL_RCC_GetSysClkSource( );
    if ( (sys_clk == LL_RCC_SYS_CLKSOURCE_STATUS_HSI) ||
         ((sys_clk == LL_RCC_SYS_CLKSOURCE_STATUS_PLL) && (LL_RCC_PLL_GetMainSource( ) == LL_RCC_PLLSOURCE_HSI)) )
    {
        /* nothing to do apart the calibration which is performed later on */
    }
    else
    {
        /* Enable the Internal High Speed oscillator (HSI or HSIdiv4 */
        LL_RCC_HSI_Enable( );

        /* Wait till HSI is ready */
        while ( LL_RCC_HSI_IsReady( ) == RESET )
        {
            ;
        }
    }

    /* Adjusts the Internal High Speed oscillator (HSI) calibration value.*/
    LL_RCC_HSI_SetCalibTrimming( RCC_HSICALIBRATION_DEFAULT );

    /*-------------------------------- PLL Configuration -----------------------*/
    /* Check if the PLL is used as system clock or not */
    if ( LL_RCC_GetSysClkSource( ) != LL_RCC_SYS_CLKSOURCE_STATUS_PLL )
    {
        {
            /* Disable the main PLL. */
            LL_RCC_PLL_Disable( );

            /* Wait till PLL is ready */
            while ( LL_RCC_PLL_IsReady( ) != RESET )
            {
                ;
            }

            /* Configure the main PLL clock source, multiplication and division factors. */
            LL_RCC_PLL_ConfigDomain_SYS( LL_RCC_PLLSOURCE_HSI, RCC_CFGR_PLLMUL6, LL_RCC_PLL_DIV_3 );

            /* Enable the main PLL. */
            LL_RCC_PLL_Enable( );

            /* Wait till PLL is ready */
            while ( LL_RCC_PLL_IsReady( ) == RESET )
            {
                ;
            }
        }
    }
    else
    {
        Error_Handler( );
    }
}

static void HW_RCC_ClockConfig( void )
{
    /*
     * To correctly read data from FLASH memory, the number of wait states (LATENCY)
     * must be correctly programmed according to the frequency of the CPU clock
     * (HCLK) and the supply voltage of the device.
     */

    /* Increasing the CPU frequency */
    if ( LL_FLASH_LATENCY_1 > LL_FLASH_GetLatency( ) )
    {
        /* Program the new number of wait states to the LATENCY bits in the FLASH_ACR register */
        LL_FLASH_SetLatency( LL_FLASH_LATENCY_1 );

        /*
         * Check that the new number of wait states is taken into account to access the Flash
         * memory by reading the FLASH_ACR register
         */
        if ( LL_FLASH_GetLatency( ) != LL_FLASH_LATENCY_1 )
        {
            Error_Handler( );
        }
    }

    /*-------------------------- HCLK Configuration --------------------------*/
    LL_RCC_SetAHBPrescaler( LL_RCC_SYSCLK_DIV_1 );

    /*------------------------- SYSCLK Configuration ---------------------------*/
    /* Check the PLL ready flag */
    if ( LL_RCC_PLL_IsReady( ) == RESET )
    {
        Error_Handler( );
    }
    LL_RCC_SetSysClkSource( LL_RCC_SYS_CLKSOURCE_PLL );
    while ( LL_RCC_GetSysClkSource( ) != LL_RCC_SYS_CLKSOURCE_STATUS_PLL )
    {
        ;
    }

    if ( LL_FLASH_LATENCY_1 <= LL_FLASH_GetLatency( ) )
    {
        /* Program the new number of wait states to the LATENCY bits in the FLASH_ACR register */
        LL_FLASH_SetLatency( LL_FLASH_LATENCY_1 );

        /*
         * Check that the new number of wait states is taken into account to access the Flash
         * memory by reading the FLASH_ACR register
         */
        if ( LL_FLASH_GetLatency( ) != LL_FLASH_LATENCY_1 )
        {
            Error_Handler( );
        }
    }

    /*-------------------------- PCLK1 Configuration ---------------------------*/
    LL_RCC_SetAPB1Prescaler( LL_RCC_APB1_DIV_1 );

    /*-------------------------- PCLK2 Configuration ---------------------------*/
    LL_RCC_SetAPB2Prescaler( LL_RCC_APB2_DIV_1 );

    /* Update the SystemCoreClock global variable */
    SystemCoreClock = RCC_PLL_GetFreqDomain_SYS( ) >> AHBPrescTable[LL_RCC_GetAHBPrescaler( ) >> 4];

    /* Configure the source of time base considering new system clocks settings*/
    /* No need to call HAL_InitTick(TICK_INT_PRIORITY) as not used */
}

static uint32_t HW_ADC_IS_ENABLE( void )
{
    return (LL_ADC_IsActiveFlag_ADRDY( adcinit_Instance ) &&
            LL_ADC_IsEnabled( adcinit_Instance )
            &&
            !LL_ADC_IsDisableOngoing( adcinit_Instance ));
}

static void HW_ADC_Init( void )
{
    /* Check the parameters */
    assert_param( IS_ADC_ALL_INSTANCE(adcinit_Instance) );
    assert_param( (adcinit_ClockPrescaler == LL_ADC_CLOCK_SYNC_PCLK_DIV1) ||
                  (adcinit_ClockPrescaler == LL_ADC_CLOCK_SYNC_PCLK_DIV2) ||
                  (adcinit_ClockPrescaler == LL_ADC_CLOCK_SYNC_PCLK_DIV4) );
    assert_param( (adcinit_Resolution == LL_ADC_RESOLUTION_12B) ||
                  (adcinit_Resolution == LL_ADC_RESOLUTION_10B) ||
                  (adcinit_Resolution == LL_ADC_RESOLUTION_8B) ||
                  (adcinit_Resolution == LL_ADC_RESOLUTION_6B) );
    assert_param( (adcinit_SamplingTime == LL_ADC_SAMPLINGTIME_1CYCLE_5) ||
                  (adcinit_SamplingTime == LL_ADC_SAMPLINGTIME_7CYCLES_5) ||
                  (adcinit_SamplingTime == LL_ADC_SAMPLINGTIME_12CYCLES_5) ||
                  (adcinit_SamplingTime == LL_ADC_SAMPLINGTIME_19CYCLES_5) ||
                  (adcinit_SamplingTime == LL_ADC_SAMPLINGTIME_39CYCLES_5) ||
                  (adcinit_SamplingTime == LL_ADC_SAMPLINGTIME_79CYCLES_5) ||
                  (adcinit_SamplingTime == LL_ADC_SAMPLINGTIME_160CYCLES_5) );
    assert_param( (adcinit_ScanConvMode == LL_ADC_REG_SEQ_SCAN_DIR_FORWARD) ||
                  (adcinit_ScanConvMode == LL_ADC_REG_SEQ_SCAN_DIR_BACKWARD) );
    assert_param( (adcinit_DataAlign == LL_ADC_DATA_ALIGN_RIGHT) ||
                  (adcinit_DataAlign == LL_ADC_DATA_ALIGN_LEFT) );
    assert_param( (adcinit_ContinuousConvMode == LL_ADC_REG_CONV_SINGLE) ||
                  (adcinit_ContinuousConvMode == LL_ADC_REG_CONV_CONTINUOUS) );
    assert_param( IS_FUNCTIONAL_STATE(adcinit_DiscontinuousConvMode) );
    assert_param( (adcinit_ExternalTrigConvEdge == 0) ||
                  (adcinit_ExternalTrigConvEdge == LL_ADC_REG_TRIG_EXT_RISING) ||
                  (adcinit_ExternalTrigConvEdge == LL_ADC_REG_TRIG_EXT_FALLING) ||
                  (adcinit_ExternalTrigConvEdge == LL_ADC_REG_TRIG_EXT_RISINGFALLING) );
    assert_param( adcinit_ExternalTrigConv == LL_ADC_REG_TRIG_SOFTWARE );
    /* note that DMA configuration should be LL_ADC_REG_DMA_TRANSFER_NONE,
     LL_ADC_REG_DMA_TRANSFER_LIMITED, or LL_ADC_REG_DMA_TRANSFER_UNLIMITED */
    assert_param( (adcinit_DMAContinuousRequests == ADC_CFGR1_DMACFG) );
    assert_param( (adcinit_Overrun == LL_ADC_REG_OVR_DATA_PRESERVED) ||
                  (adcinit_Overrun == LL_ADC_REG_OVR_DATA_OVERWRITTEN) );
    assert_param( (adcinit_LowPowerAutoWait == LL_ADC_LP_MODE_NONE) ||
                  (adcinit_LowPowerAutoWait == LL_ADC_LP_AUTOWAIT) );
    assert_param( adcinit_LowPowerFrequencyMode == LL_ADC_CLOCK_FREQ_MODE_LOW ||
                  adcinit_LowPowerFrequencyMode == LL_ADC_CLOCK_FREQ_MODE_HIGH );
    assert_param( (adcinit_LowPowerAutoPowerOff == LL_ADC_LP_MODE_NONE) ||
                  (adcinit_LowPowerAutoPowerOff == LL_ADC_LP_AUTOPOWEROFF) );
    assert_param( IS_FUNCTIONAL_STATE(adcinit_OversamplingMode) );

    /* As prerequisite, into HAL_ADC_MspInit(), ADC clock must be configured    */
    /* at RCC top level depending on both possible clock sources:               */
    /* APB clock or HSI clock.                                                  */
    /* Refer to header of this file for more details on clock enabling procedure*/

    /* Actions performed only if ADC is coming from state reset:                */
    /* - Initialization of ADC MSP                                              */
    /* - ADC voltage regulator enable                                           */
    if ( adc_state == HAL_ADC_STATE_RESET )
    {
        /* Init the low level hardware */
        /* no call to HAL_ADC_MspInit(); is performed as no action is required */
    }

    /* Configuration of ADC parameters if previous preliminary actions are      */
    /* correctly completed.                                                     */
    /* and if there is no conversion on going on regular group (ADC can be      */
    /* enabled anyway, in case of call of this function to update a parameter   */
    /* on the fly).                                                             */
    if ( (READ_BIT( adc_state, HAL_ADC_STATE_ERROR_INTERNAL )) ||
         (LL_ADC_REG_IsConversionOngoing( adcinit_Instance ) != RESET) )
    {
        /* Update ADC state machine to error */
        SET_BIT( adc_state, HAL_ADC_STATE_ERROR_INTERNAL );

        return;
    }

    /* Set ADC state */
    CLEAR_BIT( adc_state, HAL_ADC_STATE_REG_BUSY );
    SET_BIT( adc_state, HAL_ADC_STATE_ERROR_INTERNAL );

    /* Parameters update conditioned to ADC state:                            */
    /* Parameters that can be updated only when ADC is disabled:              */
    /*  - ADC clock mode                                                      */
    /*  - ADC clock prescaler                                                 */
    /*  - ADC Resolution                                                      */
    if ( !HW_ADC_IS_ENABLE( ) )
    {
        /* Some parameters of this register are not reset, since they are set   */
        /* by other functions and must be kept in case of usage of this         */
        /* without needing to reconfigure all other ADC groups/channels         */
        /* parameters):                                                         */
        /*   - internal measurement paths: Vbat, temperature sensor, Vref       */
        /*     (set into HAL_ADC_ConfigChannel() )                              */

        /*
         * Configuration of ADC clock: clock source PCLK or asynchronous with
         * selectable prescaler - same as __HAL_ADC_CLOCK_PRESCALER()
         */
        assert_param( (adcinit_ClockPrescaler == LL_ADC_CLOCK_SYNC_PCLK_DIV1) ||
                      (adcinit_ClockPrescaler == LL_ADC_CLOCK_SYNC_PCLK_DIV2) ||
                      (adcinit_ClockPrescaler == LL_ADC_CLOCK_SYNC_PCLK_DIV4) );
        LL_ADC_SetClock( adcinit_Instance, adcinit_ClockPrescaler );

        /* Configuration of ADC:                                                */
        /*  - Resolution                                                        */
        LL_ADC_SetResolution( adcinit_Instance, adcinit_Resolution );
    }

    /* Set the Low Frequency mode */
    LL_ADC_SetCommonFrequencyMode( ADC, adcinit_LowPowerFrequencyMode );

    /* Enable voltage regulator (if disabled at this step) */
    if ( !LL_ADC_IsInternalRegulatorEnabled( adcinit_Instance ) )
    {
        /* Set ADVREGEN bit */
        LL_ADC_EnableInternalRegulator( adcinit_Instance );

        /*
         * Note that this is not the original code which was:
         *    adcinit_Instance->CR |= ADC_CR_ADVREGEN;
         */
    }

    /* Configuration of ADC:                                                    */
    /*  - Resolution                                                            */
    /*  - Data alignment                                                        */
    /*  - Scan direction                                                        */
    /*  - External trigger to start conversion                                  */
    /*  - External trigger polarity                                             */
    /*  - Continuous conversion mode                                            */
    /*  - DMA continuous request                                                */
    /*  - Overrun                                                               */
    /*  - AutoDelay feature                                                     */
    /*  - Discontinuous mode                                                    */
    MODIFY_REG( adcinit_Instance->CFGR1,
                (ADC_CFGR1_ALIGN |
                ADC_CFGR1_SCANDIR |
                ADC_CFGR1_EXTSEL |
                ADC_CFGR1_EXTEN |
                ADC_CFGR1_CONT |
                ADC_CFGR1_DMACFG |
                ADC_CFGR1_OVRMOD |
                ADC_CFGR1_AUTDLY |
                ADC_CFGR1_AUTOFF |
                ADC_CFGR1_DISCEN),
                (adcinit_DataAlign |
                 adcinit_ScanConvMode
                 |
                 adcinit_ContinuousConvMode
                 |
                 adcinit_DMAContinuousRequests
                 |
                 adcinit_Overrun
                 |
                 adcinit_LowPowerAutoWait
                 |
                 adcinit_LowPowerAutoPowerOff) );

    /* Enable external trigger if trigger selection is different of software  */
    /* start.                                                                 */
    /* Note: This configuration keeps the hardware feature of parameter       */
    /*       ExternalTrigConvEdge "trigger edge none" equivalent to           */
    /*       software start.                                                  */
    if ( adcinit_ExternalTrigConv != LL_ADC_REG_TRIG_SOFTWARE )
    {
        LL_ADC_REG_SetTriggerSource( adcinit_Instance, adcinit_ExternalTrigConv );
        LL_ADC_REG_SetTriggerEdge( adcinit_Instance, adcinit_ExternalTrigConvEdge );
    }

    assert_param( adcinit_DiscontinuousConvMode != ENABLE );
    assert_param( adcinit_OversamplingMode != ENABLE );
    if ( LL_ADC_GetOverSamplingScope( adcinit_Instance ) != LL_ADC_OVS_DISABLE )
    {
        /* Disable OverSampling mode if needed */
        LL_ADC_SetOverSamplingScope( adcinit_Instance, LL_ADC_OVS_DISABLE );
    }

    /* Set the new sample time */
    LL_ADC_SetSamplingTimeCommonChannels( adcinit_Instance, adcinit_SamplingTime );

    /* Set the ADC state */
    CLEAR_BIT( adc_state, HAL_ADC_STATE_BUSY_INTERNAL );
    SET_BIT( adc_state, HAL_ADC_STATE_READY );
}

static void HW_ADC_DelayMicroSecond( uint32_t microSecond )
{
    /* Compute number of CPU cycles to wait for */
    __IO uint32_t waitLoopIndex = ((microSecond * SystemCoreClock) / 1000000U);

    while ( waitLoopIndex != 0U )
    {
        waitLoopIndex--;
    }
}

static void HW_ADC_DeselectChannel( void )
{
    /* Parameters update conditioned to ADC state:                              */
    /* Parameters that can be updated when ADC is disabled or enabled without   */
    /* conversion on going on regular group:                                    */
    /*  - Channel number                                                        */
    /*  - Management of internal measurement channels: Vbat/VrefInt/TempSensor  */
    if ( LL_ADC_REG_IsConversionOngoing( adcinit_Instance ) )
    {
        /* Update ADC state machine to error */
        SET_BIT( adc_state, HAL_ADC_STATE_ERROR_CONFIG );
        return;
    }

    /* Regular sequence configuration */
    /* Reset the channel selection register from the selected channel */
    LL_ADC_REG_SetSequencerChRem( adcinit_Instance, ADC_AWD_CR_ALL_CHANNEL_MASK );
    LL_ADC_SetCommonPathInternalCh( ADC, LL_ADC_PATH_INTERNAL_NONE );
}

static void HW_ADC_SelectChannel( uint32_t channel )
{
    uint32_t value = LL_ADC_PATH_INTERNAL_NONE;
    uint32_t delay = 0;

    /* Parameters update conditioned to ADC state:                              */
    /* Parameters that can be updated when ADC is disabled or enabled without   */
    /* conversion on going on regular group:                                    */
    /*  - Channel number                                                        */
    /*  - Management of internal measurement channels: Vbat/VrefInt/TempSensor  */
    if ( LL_ADC_REG_IsConversionOngoing( adcinit_Instance ) )
    {
        /* Update ADC state machine to error */
        SET_BIT( adc_state, HAL_ADC_STATE_ERROR_CONFIG );
        return;
    }

    /* Enable selected channels */
    LL_ADC_REG_SetSequencerChAdd( adcinit_Instance, channel );

    /* If Temperature sensor channel is selected, then enable the internal      */
    /* buffers and path  */
    channel = channel & ADC_AWD_CR_ALL_CHANNEL_MASK;
    if ( (channel & LL_ADC_CHANNEL_TEMPSENSOR) == (LL_ADC_CHANNEL_TEMPSENSOR & ADC_AWD_CR_ALL_CHANNEL_MASK) )
    {
        value |= LL_ADC_PATH_INTERNAL_TEMPSENSOR;
        delay = LL_ADC_DELAY_TEMPSENSOR_STAB_US; /* Delay for temperature sensor stabilization time */
    }

    /* If VRefInt channel is selected, then enable the internal buffers and path   */
    if ( (channel & LL_ADC_CHANNEL_VREFINT) == (LL_ADC_CHANNEL_VREFINT & ADC_AWD_CR_ALL_CHANNEL_MASK) )
    {
        value |= LL_ADC_PATH_INTERNAL_VREFINT;
    }

#if defined(STM32L053xx) || defined(STM32L063xx) || defined(STM32L073xx) || defined(STM32L083xx)
    /* If Vlcd channel is selected, then enable the internal buffers and path   */
    if ( (channel & LL_ADC_CHANNEL_VLCD) == (LL_ADC_CHANNEL_VLCD & ADC_AWD_CR_ALL_CHANNEL_MASK) )
    {
        value |= LL_ADC_PATH_INTERNAL_VLCD;
    }
#endif

    LL_ADC_SetCommonPathInternalCh( ADC, value );
    if ( delay )
    {
        HW_ADC_DelayMicroSecond( delay );
    }
}

static ErrorStatus HW_ADC_Enable( void )
{
    /* ADC enable and wait for ADC ready (in case of ADC is disabled or         */
    /* enabling phase not yet completed: flag ADC ready not yet set).           */
    /* Timeout implemented to not be stuck if ADC cannot be enabled (possible   */
    /* causes: ADC clock not running, ...).                                     */
    if ( !HW_ADC_IS_ENABLE( ) )
    {
        /* Check if conditions to enable the ADC are fulfilled */
        /* same as "if (ADC_ENABLING_CONDITIONS() == RESET)" */
        if ( READ_BIT(adcinit_Instance->CR, ADC_CR_BITS_PROPERTY_RS) == RESET )
        {
            /* Update ADC state machine to error */
            SET_BIT( adc_state, HAL_ADC_STATE_ERROR_INTERNAL );
            return ERROR;
        }

        /* Enable the ADC peripheral */
        LL_ADC_Enable( adcinit_Instance );

        /* Delay for ADC stabilization time. */
        HW_ADC_DelayMicroSecond( ADC_STAB_DELAY_US );

        /* Wait for ADC effectively enabled */
        while ( !LL_ADC_IsActiveFlag_ADRDY( adcinit_Instance ) )
        {
            ;
        }
    }

    return SUCCESS;
}

static void HW_ADC_Start( )
{
    uint32_t status = SUCCESS;

    /* Perform ADC enable and conversion start if no conversion is on going */
    if ( !LL_ADC_REG_IsConversionOngoing( adcinit_Instance ) )
    {
        /* Enable the ADC peripheral */
        /* If low power mode AutoPowerOff is enabled, power-on/off phases are       */
        /* performed automatically by hardware.                                     */
        if ( adcinit_LowPowerAutoPowerOff != LL_ADC_LP_AUTOPOWEROFF )
        {
            status = HW_ADC_Enable( );
        }

        /* Start conversion if ADC is effectively enabled */
        if ( status == SUCCESS )
        {
            /* Set ADC state                                                        */
            /* - Clear state bitfield related to regular group conversion results   */
            /* - Set state bitfield related to regular operation                    */
            CLEAR_BIT( adc_state,
                       HAL_ADC_STATE_READY | HAL_ADC_STATE_REG_EOC | HAL_ADC_STATE_REG_OVR | HAL_ADC_STATE_REG_EOSMP );
            SET_BIT( adc_state, HAL_ADC_STATE_REG_BUSY );

            /* Clear regular group conversion flag and overrun flag */
            /* (To ensure of no unknown state from potential previous ADC           */
            /* operations)                                                          */
            WRITE_REG( adcinit_Instance->ISR, (LL_ADC_FLAG_EOC | LL_ADC_FLAG_EOS | LL_ADC_FLAG_OVR) );

            /* Enable conversion of regular group.                                  */
            /* If software start has been selected, conversion starts immediately.  */
            /* If external trigger has been selected, conversion will start at next */
            /* trigger event.                                                       */
            LL_ADC_REG_StartConversion( adcinit_Instance );
        }
    }
}

static void HW_ADC_PollForConversion( )
{
    /* Check the parameters */
    assert_param( IS_ADC_ALL_INSTANCE(adcinit_Instance) );
    assert_param( (adcinit_EOCSelection == LL_ADC_FLAG_EOC) || (adcinit_EOCSelection == LL_ADC_FLAG_EOS) );

    /* If end of conversion selected to end of sequence */
    if ( adcinit_EOCSelection == LL_ADC_FLAG_EOS )
    {
        Error_Handler( ); /* EOS is not implemented */
    }
    /* If end of conversion selected to end of each conversion */
    else /* ADC_EOC_SINGLE_CONV */
    {
        /* Verification that ADC configuration is compliant with polling for      */
        /* each conversion:                                                       */
        /* Particular case is ADC configured in DMA mode and ADC sequencer with   */
        /* several ranks and polling for end of each conversion.                  */
        /* For code simplicity sake, this particular case is generalized to       */
        /* ADC configured in DMA mode and and polling for end of each conversion. */
        if ( READ_BIT( adcinit_Instance->CFGR1, ADC_CFGR1_DMAEN ) )
        {
            /* Update ADC state machine to error */
            SET_BIT( adc_state, HAL_ADC_STATE_ERROR_CONFIG );
            return;
        }
    }

    /* Wait until End of Conversion flag is raised */
    while ( LL_ADC_IsActiveFlag_EOC( adcinit_Instance ) == 0 )
    {
        ;
    }
    while ( LL_ADC_REG_IsConversionOngoing( adcinit_Instance ) )
    {
        ;
    }

    /* Update ADC state machine */
    SET_BIT( adc_state, HAL_ADC_STATE_REG_EOC );

    /* Determine whether any further conversion upcoming on group regular       */
    /* by external trigger, continuous mode or scan sequence on going.          */
    if ( LL_ADC_REG_IsTriggerSourceSWStart( adcinit_Instance ) &&
         (adcinit_ContinuousConvMode == LL_ADC_REG_CONV_SINGLE) )
    {
        if ( LL_ADC_IsActiveFlag_EOS( adcinit_Instance ) )
        {
            /* Allowed to modify bits ADC_IT_EOC/ADC_IT_EOS only if bit             */
            /* ADSTART==0 (no conversion on going)                                  */
            if ( !LL_ADC_REG_IsConversionOngoing( adcinit_Instance ) )
            {
                /* Disable ADC end of single conversion interrupt on group regular */
                /* Note: Overrun interrupt was enabled with EOC interrupt in          */
                /* HAL_Start_IT(), but is not disabled here because can be used       */
                /* by overrun IRQ process below.                                      */
                LL_ADC_DisableIT_EOS( adcinit_Instance );
                LL_ADC_DisableIT_EOC( adcinit_Instance );

                /* Set ADC state */
                CLEAR_BIT( adc_state, HAL_ADC_STATE_REG_BUSY );
                SET_BIT( adc_state, HAL_ADC_STATE_READY );
            }
            else
            {
                /* Change ADC state to error state */
                SET_BIT( adc_state, HAL_ADC_STATE_ERROR_CONFIG );
            }
        }
    }

    /*
     * Clear end of conversion flag of regular group if low power feature
     * "LowPowerAutoWait " is disabled, to not interfere with this feature
     * until data register is read using function HAL_ADC_GetValue().
     */
    if ( adcinit_LowPowerAutoWait == LL_ADC_LP_MODE_NONE )
    {
        /* Clear regular group conversion flag */
        LL_ADC_ClearFlag_EOC( adcinit_Instance );
    }
}

static void HW_AdcInit( void )
{
    if ( AdcInitialized == RESET )
    {
        AdcInitialized = SET;

        LL_APB2_GRP1_EnableClock( LL_APB2_GRP1_PERIPH_ADC1 );
        HW_ADC_Init( );
        LL_ADC_StartCalibration( adcinit_Instance );
        while ( LL_ADC_IsCalibrationOnGoing( adcinit_Instance ) == 0 )
        {
            ;
        }
        LL_APB2_GRP1_DisableClock( LL_APB2_GRP1_PERIPH_ADC1 );
    }
}

static uint16_t HW_AdcReadChannel( uint32_t Channel )
{
    uint16_t adcData = 0;
    if ( AdcInitialized == SET )
    {
        LL_APB2_GRP1_EnableClock( LL_APB2_GRP1_PERIPH_ADC1 );
        /* wait the the Vrefint used by adc is set */
        while ( LL_PWR_IsActiveFlag_VREFINTRDY( ) == RESET )
        {
            ;
        }

        /*calibrate ADC if any calibraiton hardware*/
        LL_ADC_StartCalibration( adcinit_Instance );
        while ( LL_ADC_IsCalibrationOnGoing( adcinit_Instance ) == 0 )
        {
            ;
        }

        /* Enable the ADC, but after few cycles once the calibration is completed */
        volatile int i = 32 * LL_ADC_DELAY_CALIB_ENABLE_ADC_CYCLES;
        while ( i )
        {
            i--;
        }
        LL_ADC_Enable( adcinit_Instance );
        while ( LL_ADC_IsActiveFlag_ADRDY( adcinit_Instance ) == 0 )
        {
            ;
        }

        /* Deselects all channels*/
        HW_ADC_DeselectChannel( );

        /* configure adc channel */
        HW_ADC_SelectChannel( Channel );

        /* Start the conversion process */
        HW_ADC_Start( );

        /* Wait for the end of conversion */
        HW_ADC_PollForConversion( );

        /* Get the converted value of regular channel */
        adcData = LL_ADC_REG_ReadConversionData12( adcinit_Instance );

        /* Implementation of __HAL_ADC_DISABLE(__HANDLE__) */
        LL_ADC_Disable( adcinit_Instance );
        LL_ADC_ClearFlag_EOSMP( adcinit_Instance );
        LL_ADC_ClearFlag_ADRDY( adcinit_Instance );

        LL_APB2_GRP1_DisableClock( LL_APB2_GRP1_PERIPH_ADC1 );
    }
    return adcData;
}

uint32_t HW_Get_MFT_ID(void)
{
    return 0x1234;
}

uint32_t HW_Get_MFT_Model(void)
{
    return 0x4321;
}

uint32_t HW_Get_MFT_Rev(void)
{
    return 0x0001;
}

uint32_t HW_Get_MFT_SN(void)
{
    return 0xffff;
}

bool HW_Set_MFT_Baud(uint32_t baud)
{
    return true;
}

uint32_t HW_Get_MFT_Baud(void)
{
    return 115200;
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
