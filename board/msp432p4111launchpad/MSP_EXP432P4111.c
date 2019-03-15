/*
 * Copyright (c) 2017-2018, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== MSP_EXP432P4111.c ========
 *  This file is responsible for setting up the board specific items for the
 *  MSP_EXP432P4111 board.
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <k_api.h>
#include <aos/kernel.h>

#ifndef DeviceFamily_MSP432P4x1xI
#define DeviceFamily_MSP432P4x1xI
#endif

#include <ti/devices/DeviceFamily.h>

#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerMSP432.h>

#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/rom.h>
#include <ti/devices/msp432p4xx/driverlib/rom_map.h>
#include <ti/devices/msp432p4xx/driverlib/adc14.h>
#include <ti/devices/msp432p4xx/driverlib/dma.h>
#include <ti/devices/msp432p4xx/driverlib/gpio.h>
#include <ti/devices/msp432p4xx/driverlib/i2c.h>
#include <ti/devices/msp432p4xx/driverlib/interrupt.h>
#include <ti/devices/msp432p4xx/driverlib/pmap.h>
#include <ti/devices/msp432p4xx/driverlib/ref_a.h>
#include <ti/devices/msp432p4xx/driverlib/spi.h>
#include <ti/devices/msp432p4xx/driverlib/timer_a.h>
#include <ti/devices/msp432p4xx/driverlib/timer32.h>
#include <ti/devices/msp432p4xx/driverlib/uart.h>
#include <ti/devices/msp432p4xx/driverlib/wdt_a.h>

#include "MSP_EXP432P4111.h"

/*
 *  =============================== ADC ===============================
 */
#include <ti/drivers/ADC.h>
#include <ti/drivers/adc/ADCMSP432.h>

/* ADC objects */
ADCMSP432_Object adcMSP432Objects[MSP_EXP432P4111_ADCCOUNT];

/* ADC configuration structure */
const ADCMSP432_HWAttrsV1 adcMSP432HWAttrs[MSP_EXP432P4111_ADCCOUNT] = {
    {
        .adcPin = ADCMSP432_P5_5_A0,
        .refVoltage = ADCMSP432_REF_VOLTAGE_INT_2_5V,
        .resolution = ADC_14BIT
    },
    {
        .adcPin = ADCMSP432_P5_4_A1,
        .refVoltage = ADCMSP432_REF_VOLTAGE_INT_1_45V,
        .resolution = ADC_8BIT
    }
};

const ADC_Config ADC_config[MSP_EXP432P4111_ADCCOUNT] = {
    {
        .fxnTablePtr = &ADCMSP432_fxnTable,
        .object = &adcMSP432Objects[MSP_EXP432P4111_ADC0],
        .hwAttrs = &adcMSP432HWAttrs[MSP_EXP432P4111_ADC0]
    },
    {
        .fxnTablePtr = &ADCMSP432_fxnTable,
        .object = &adcMSP432Objects[MSP_EXP432P4111_ADC1],
        .hwAttrs = &adcMSP432HWAttrs[MSP_EXP432P4111_ADC1]
    }
};

const uint_least8_t ADC_count = MSP_EXP432P4111_ADCCOUNT;

/*
 *  =============================== ADCBuf ===============================
 */
#include <ti/drivers/ADCBuf.h>
#include <ti/drivers/adcbuf/ADCBufMSP432.h>

/* ADC objects */
ADCBufMSP432_Object adcbufMSP432Objects[MSP_EXP432P4111_ADCBUFCOUNT];

ADCBufMSP432_Channels adcBuf0MSP432Channels[MSP_EXP432P4111_ADCBUF0CHANNELCOUNT] = {
    {
        .adcPin = ADCBufMSP432_P5_5_A0,
        .refSource = ADCBufMSP432_VREFPOS_AVCC_VREFNEG_VSS,
        .refVoltage = 3300000,
        .adcInputMode = ADCBufMSP432_SINGLE_ENDED,
        .adcDifferentialPin = ADCBufMSP432_PIN_NONE,
        .adcInternalSource = ADCBufMSP432_INTERNAL_SOURCE_MODE_OFF
    },
    {
        .adcPin = ADCBufMSP432_P5_4_A1,
        .refSource = ADCBufMSP432_VREFPOS_INTBUF_VREFNEG_VSS,
        .refVoltage = 2500000,
        .adcInputMode = ADCBufMSP432_SINGLE_ENDED,
        .adcDifferentialPin = ADCBufMSP432_PIN_NONE,
        .adcInternalSource = ADCBufMSP432_INTERNAL_SOURCE_MODE_OFF
    }
};

/* ADC configuration structure */
const ADCBufMSP432_HWAttrs adcbufMSP432HWAttrs[MSP_EXP432P4111_ADCBUFCOUNT] = {
    {
        .intPriority =  ~0,
        .channelSetting = adcBuf0MSP432Channels,
        .adcTimerTriggerSource = ADCBufMSP432_TIMERA1_CAPTURECOMPARE2,
        .useDMA = 1,
        .dmaIntNum = DMA_INT0,
        .adcTriggerSource = ADCBufMSP432_TIMER_TRIGGER,
        .timerDutyCycle = 50,
        .clockSource = ADCBufMSP432_ADC_CLOCK,
    }
};

const ADCBuf_Config ADCBuf_config[MSP_EXP432P4111_ADCBUFCOUNT] = {
    {
        .fxnTablePtr = &ADCBufMSP432_fxnTable,
        .object = &adcbufMSP432Objects[MSP_EXP432P4111_ADCBUF0],
        .hwAttrs = &adcbufMSP432HWAttrs[MSP_EXP432P4111_ADCBUF0]
    }
};

const uint_least8_t ADCBuf_count = MSP_EXP432P4111_ADCBUFCOUNT;

/*
 *  ============================= Capture =============================
 */
#include <ti/drivers/Capture.h>
#include <ti/drivers/capture/CaptureMSP432.h>

CaptureMSP432_Object captureMSP432Objects[MSP_EXP432P4111_CAPTURECOUNT];

const CaptureMSP432_HWAttrs captureMSP432HWAttrs[MSP_EXP432P4111_CAPTURECOUNT] = {
    /* Timer_A1 */
    {
        .timerBaseAddress = TIMER_A1_BASE,
        .clockSource = TIMER_A_CLOCKSOURCE_ACLK,
        .clockDivider = TIMER_A_CLOCKSOURCE_DIVIDER_64,
        .capturePort = CaptureMSP432_P7_7_TA1,
        .intPriority = ~0
    },
    /* Timer_A2 */
    {
        .timerBaseAddress = TIMER_A2_BASE,
        .clockSource = TIMER_A_CLOCKSOURCE_ACLK,
        .clockDivider = TIMER_A_CLOCKSOURCE_DIVIDER_64,
        .capturePort = CaptureMSP432_P6_7_TA2,
        .intPriority = ~0
    },
    /* Timer_A3 */
    {
        .timerBaseAddress = TIMER_A3_BASE,
        .clockSource = TIMER_A_CLOCKSOURCE_ACLK,
        .clockDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1,
        .capturePort = CaptureMSP432_P8_2_TA3,
        .intPriority = ~0
    }
};

const Capture_Config Capture_config[MSP_EXP432P4111_CAPTURECOUNT] = {
    {
        .fxnTablePtr = &CaptureMSP432_captureFxnTable,
        .object = &captureMSP432Objects[MSP_EXP432P4111_CAPTURE_TA1],
        .hwAttrs = &captureMSP432HWAttrs[MSP_EXP432P4111_CAPTURE_TA1]
    },
    {
        .fxnTablePtr = &CaptureMSP432_captureFxnTable,
        .object = &captureMSP432Objects[MSP_EXP432P4111_CAPTURE_TA2],
        .hwAttrs = &captureMSP432HWAttrs[MSP_EXP432P4111_CAPTURE_TA2]
    },
    {
        .fxnTablePtr = &CaptureMSP432_captureFxnTable,
        .object = &captureMSP432Objects[MSP_EXP432P4111_CAPTURE_TA3],
        .hwAttrs = &captureMSP432HWAttrs[MSP_EXP432P4111_CAPTURE_TA3]
    }
};

const uint_least8_t Capture_count = MSP_EXP432P4111_CAPTURECOUNT;

/*
 *  =============================== DMA ===============================
 */
#include <ti/drivers/dma/UDMAMSP432.h>

#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_ALIGN(dmaControlTable, 256)
#elif defined(__IAR_SYSTEMS_ICC__)
#pragma data_alignment=256
#elif defined(__GNUC__)
__attribute__ ((aligned (256)))
#endif
static DMA_ControlTable dmaControlTable[16];

/*
 *  ======== dmaErrorHwi ========
 *  This is the handler for the uDMA error interrupt.
 */
static void dmaErrorHwi(uintptr_t arg)
{
    int status = MAP_DMA_getErrorStatus();
    MAP_DMA_clearErrorStatus();

    /* Suppress unused variable warning */
    (void)status;

    while (1);
}

UDMAMSP432_Object udmaMSP432Object;

const UDMAMSP432_HWAttrs udmaMSP432HWAttrs = {
    .controlBaseAddr = (void *)dmaControlTable,
    .dmaErrorFxn = (UDMAMSP432_ErrorFxn)dmaErrorHwi,
    .intNum = INT_DMA_ERR,
    .intPriority = (~0)
};

const UDMAMSP432_Config UDMAMSP432_config = {
    .object = &udmaMSP432Object,
    .hwAttrs = &udmaMSP432HWAttrs
};

/*
 *  ============================= Display =============================
 */
#include <ti/display/Display.h>
#include <ti/display/DisplayUart.h>
#include <ti/display/DisplaySharp.h>
#define MAXPRINTLEN 1024

#ifndef BOARD_DISPLAY_SHARP_SIZE
#define BOARD_DISPLAY_SHARP_SIZE    96
#endif

DisplayUart_Object displayUartObject;
DisplaySharp_Object    displaySharpObject;

static char displayBuf[MAXPRINTLEN];
static uint_least8_t sharpDisplayBuf[BOARD_DISPLAY_SHARP_SIZE * BOARD_DISPLAY_SHARP_SIZE / 8];

const DisplayUart_HWAttrs displayUartHWAttrs = {
    .uartIdx = MSP_EXP432P4111_UARTA0,
    .baudRate = 115200,
    .mutexTimeout = (unsigned int)(-1),
    .strBuf = displayBuf,
    .strBufLen = MAXPRINTLEN
};

const DisplaySharp_HWAttrsV1 displaySharpHWattrs = {
    .spiIndex    = MSP_EXP432P4111_SPIB0,
    .csPin       = MSP_EXP432P4111_LCD_CS,
    .powerPin    = MSP_EXP432P4111_LCD_POWER,
    .enablePin   = MSP_EXP432P4111_LCD_ENABLE,
    .pixelWidth  = BOARD_DISPLAY_SHARP_SIZE,
    .pixelHeight = BOARD_DISPLAY_SHARP_SIZE,
    .displayBuf  = sharpDisplayBuf,
};

/*
 * This #if/#else is needed to workaround a problem with the
 * IAR compiler. The IAR compiler doesn't like the empty array
 * initialization. (IAR Error[Pe1345])
 */
#ifndef BOARD_DISPLAY_USE_UART
#define BOARD_DISPLAY_USE_UART 1
#endif
#ifndef BOARD_DISPLAY_USE_UART_ANSI
#define BOARD_DISPLAY_USE_UART_ANSI 0
#endif
#ifndef BOARD_DISPLAY_USE_LCD
#define BOARD_DISPLAY_USE_LCD 0
#endif
#if 0
const Display_Config Display_config[] = {
    {
#  if (BOARD_DISPLAY_USE_UART_ANSI)
        .fxnTablePtr = &DisplayUartAnsi_fxnTable,
#  else /* Default to minimal UART with no cursor placement */
        .fxnTablePtr = &DisplayUartMin_fxnTable,
#  endif
        .object = &displayUartObject,
        .hwAttrs = &displayUartHWAttrs
    },
#if (BOARD_DISPLAY_USE_LCD)
    {
        .fxnTablePtr = &DisplaySharp_fxnTable,
        .object      = &displaySharpObject,
        .hwAttrs     = &displaySharpHWattrs
    },
#endif
};

const uint_least8_t Display_count = sizeof(Display_config) / sizeof(Display_Config);
#endif
/*
 *  ======== MSP_EXP432P4111_initGeneral ========
 */
void MSP_EXP432P4111_initGeneral(void)
{
    Power_init();
}

/*
 *  =============================== GPIO ===============================
 */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/gpio/GPIOMSP432.h>

/*
 * Array of Pin configurations
 * NOTE: The order of the pin configurations must coincide with what was
 *       defined in MSP_EXP432P4111.h
 * NOTE: Pins not used for interrupts should be placed at the end of the
 *       array.  Callback entries can be omitted from callbacks array to
 *       reduce memory usage.
 */
GPIO_PinConfig gpioPinConfigs[] = {
    /* Input pins */
    /*
     * NOTE: Specifying FALLING edge triggering for these buttons to ensure the
     * interrupts are signaled immediately.  See the description of the
     * PowerMSP432 driver's automatic pin parking feature for this rationale.
     */
    /* MSP_EXP432P4111_GPIO_S1 */
    GPIOMSP432_P1_1 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING,
    /* MSP_EXP432P4111_GPIO_S2 */
    GPIOMSP432_P1_4 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING,

    /* MSP_EXP432P4111_SPI_MASTER_READY */
    GPIOMSP432_P5_7 | GPIO_DO_NOT_CONFIG,
    /* MSP_EXP432P4111_SPI_SLAVE_READY */
    GPIOMSP432_P6_0 | GPIO_DO_NOT_CONFIG,

    /* Output pins */
    /* MSP_EXP432P4111_GPIO_LED1 */
    GPIOMSP432_P1_0 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW,
    /* MSP_EXP432P4111_GPIO_LED_RED */
    GPIOMSP432_P2_0 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,

    /*
     * MSP_EXP432P4111_GPIO_LED_GREEN & MSP_EXP432P4111_GPIO_LED_BLUE are used for
     * PWM examples.  Uncomment the following lines if you would like to control
     * the LEDs with the GPIO driver.
     */
    /* MSP_EXP432P4111_GPIO_LED_GREEN */
    /* GPIOMSP432_P2_1 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW, */
    /* MSP_EXP432P4111_GPIO_LED_BLUE */
    /* GPIOMSP432_P2_2 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW */
    /* MSP_EXP432P4111_SPI_CS1 */
    GPIOMSP432_P5_4 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_HIGH,
    /* MSP_EXP432P4111_SPI_CS2 */
    GPIOMSP432_P5_5 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_HIGH,

    /* MSP_EXP432P4111_SDSPI_CS */
    GPIOMSP432_P4_6 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_HIGH,

    /* Sharp Display - GPIO configurations will be done in the Display files */
    GPIOMSP432_P4_3 | GPIO_DO_NOT_CONFIG, /* SPI chip select */
    GPIOMSP432_P4_1 | GPIO_DO_NOT_CONFIG, /* LCD power control */
    GPIOMSP432_P6_0 | GPIO_DO_NOT_CONFIG, /*LCD enable */
};

/*
 * Array of callback function pointers
 * NOTE: The order of the pin configurations must coincide with what was
 *       defined in MSP_EXP432P4111.h
 * NOTE: Pins not used for interrupts can be omitted from callbacks array to
 *       reduce memory usage (if placed at end of gpioPinConfigs array).
 */
GPIO_CallbackFxn gpioCallbackFunctions[] = {
    /* MSP_EXP432P4111_GPIO_S1 */
    NULL,
    /* MSP_EXP432P4111_GPIO_S2 */
    NULL,
    /* MSP_EXP432P4111_SPI_MASTER_READY */
    NULL,
    /* MSP_EXP432P4111_SPI_SLAVE_READY */
    NULL
};

const GPIOMSP432_Config GPIOMSP432_config = {
    .pinConfigs = (GPIO_PinConfig *)gpioPinConfigs,
    .callbacks = (GPIO_CallbackFxn *)gpioCallbackFunctions,
    .numberOfPinConfigs = sizeof(gpioPinConfigs)/sizeof(GPIO_PinConfig),
    .numberOfCallbacks = sizeof(gpioCallbackFunctions)/sizeof(GPIO_CallbackFxn),
    .intPriority = (~0)
};

/*
 *  =============================== I2C ===============================
 */
#include <ti/drivers/I2C.h>
#include <ti/drivers/i2c/I2CMSP432.h>

I2CMSP432_Object i2cMSP432Objects[MSP_EXP432P4111_I2CCOUNT];

const I2CMSP432_HWAttrsV1 i2cMSP432HWAttrs[MSP_EXP432P4111_I2CCOUNT] = {
    {
        .baseAddr = EUSCI_B0_BASE,
        .intNum = INT_EUSCIB0,
        .intPriority = (~0),
        .clockSource = EUSCI_B_I2C_CLOCKSOURCE_SMCLK,
        .dataPin = I2CMSP432_P1_6_UCB0SDA,
        .clkPin = I2CMSP432_P1_7_UCB0SCL
    },
    {
        .baseAddr = EUSCI_B1_BASE,
        .intNum = INT_EUSCIB1,
        .intPriority = (~0),
        .clockSource = EUSCI_B_I2C_CLOCKSOURCE_SMCLK,
        .dataPin = I2CMSP432_P6_4_UCB1SDA,
        .clkPin = I2CMSP432_P6_5_UCB1SCL
    }
};

const I2C_Config I2C_config[MSP_EXP432P4111_I2CCOUNT] = {
    {
        .fxnTablePtr = &I2CMSP432_fxnTable,
        .object = &i2cMSP432Objects[MSP_EXP432P4111_I2CB0],
        .hwAttrs = &i2cMSP432HWAttrs[MSP_EXP432P4111_I2CB0]
    },
    {
        .fxnTablePtr = &I2CMSP432_fxnTable,
        .object = &i2cMSP432Objects[MSP_EXP432P4111_I2CB1],
        .hwAttrs = &i2cMSP432HWAttrs[MSP_EXP432P4111_I2CB1]
    }
};

const uint_least8_t I2C_count = MSP_EXP432P4111_I2CCOUNT;

/*
 *  =============================== I2CSlave ===============================
 */
#include <ti/drivers/I2CSlave.h>
#include <ti/drivers/i2cslave/I2CSlaveMSP432.h>

I2CSlaveMSP432_Object i2cSlaveMSP432Objects[MSP_EXP432P4111_I2CSLAVECOUNT];

const I2CSlaveMSP432_HWAttrs i2cSlaveMSP432HWAttrs[MSP_EXP432P4111_I2CSLAVECOUNT] = {
    {
        .baseAddr = EUSCI_B0_BASE,
        .intNum = INT_EUSCIB0,
        .intPriority = ~0,
        .slaveAddress = 0x48,
        .dataPin = I2CSLAVEMSP432_P1_6_UCB0SDA,
        .clkPin = I2CSLAVEMSP432_P1_7_UCB0SCL
    }
};

const I2CSlave_Config I2CSlave_config[MSP_EXP432P4111_I2CSLAVECOUNT] = {
    {
        .fxnTablePtr = &I2CSlaveMSP432_fxnTable,
        .object = &i2cSlaveMSP432Objects[MSP_EXP432P4111_I2CSLAVEB0],
        .hwAttrs = &i2cSlaveMSP432HWAttrs[MSP_EXP432P4111_I2CSLAVEB0]
    }
};

const uint_least8_t I2CSlave_count = MSP_EXP432P4111_I2CSLAVECOUNT;

/*
 *  =============================== NVS ===============================
 */
#include <ti/drivers/NVS.h>
#include <ti/drivers/nvs/NVSMSP432.h>

#define SECTORSIZE       0x1000
#define NVS_REGIONS_BASE 0x0000
//#define REGIONSIZE       (SECTORSIZE * 16)
#define REGIONSIZE       0x200000

/*
 * Reserve flash sectors for NVS driver use
 * by placing an uninitialized byte array
 * at the desired flash address.
 */
#if defined(__TI_COMPILER_VERSION__)

/*
 * Place uninitialized array at NVS_REGIONS_BASE
 */
#pragma LOCATION(flashBuf, NVS_REGIONS_BASE);
#pragma NOINIT(flashBuf);
static char flashBuf[REGIONSIZE];

#elif defined(__IAR_SYSTEMS_ICC__)

/*
 * Place uninitialized array at NVS_REGIONS_BASE
 */
static __no_init char flashBuf[REGIONSIZE] @ NVS_REGIONS_BASE;

#elif defined(__GNUC__)

/*
 * Place the flash buffers in the .nvs section created in the gcc linker file.
 * The .nvs section enforces alignment on a sector boundary but may
 * be placed anywhere in flash memory.  If desired the .nvs section can be set
 * to a fixed address by changing the following in the gcc linker file:
 *
 * .nvs (FIXED_FLASH_ADDR) (NOLOAD) : AT (FIXED_FLASH_ADDR) {
 *      *(.nvs)
 * } > REGION_TEXT
 */
__attribute__ ((section (".nvs")))
static char flashBuf[REGIONSIZE];

#endif

NVSMSP432_Object nvsMSP432Objects[MSP_EXP432P4111_NVSCOUNT];

const NVSMSP432_HWAttrs nvsMSP432HWAttrs[MSP_EXP432P4111_NVSCOUNT] = {
    {
        .regionBase = (void *)flashBuf,
        .regionSize = REGIONSIZE,
    },
};

const NVS_Config NVS_config[MSP_EXP432P4111_NVSCOUNT] = {
    {
        .fxnTablePtr = &NVSMSP432_fxnTable,
        .object = &nvsMSP432Objects[MSP_EXP432P4111_NVSMSP4320],
        .hwAttrs = &nvsMSP432HWAttrs[MSP_EXP432P4111_NVSMSP4320],
    },
};

const uint_least8_t NVS_count = MSP_EXP432P4111_NVSCOUNT;

/*
 *  =============================== Power ===============================
 */
void PowerMSP432_initPolicy(void)
{
    return;
}

void PowerMSP432_sleepPolicy(void)
{
    return;
}

const PowerMSP432_ConfigV1 PowerMSP432_config = {
    .policyInitFxn = &PowerMSP432_initPolicy,
    .policyFxn = &PowerMSP432_sleepPolicy,
    .initialPerfLevel = 2,
    .enablePolicy = true,
    .enablePerf = true,
    .enableParking = true
};

/*
 *  =============================== PWM ===============================
 */
#include <ti/drivers/PWM.h>
#include <ti/drivers/pwm/PWMTimerMSP432.h>

PWMTimerMSP432_Object pwmTimerMSP432Objects[MSP_EXP432P4111_PWMCOUNT];

const PWMTimerMSP432_HWAttrsV2 pwmTimerMSP432HWAttrs[MSP_EXP432P4111_PWMCOUNT] = {
    {
        .clockSource = TIMER_A_CLOCKSOURCE_SMCLK,
        .pwmPin = PWMTimerMSP432_P2_1_TA1CCR1A
    },
    {
        .clockSource = TIMER_A_CLOCKSOURCE_SMCLK,
        .pwmPin = PWMTimerMSP432_P2_2_TA1CCR2A
    }
};

const PWM_Config PWM_config[MSP_EXP432P4111_PWMCOUNT] = {
    {
        .fxnTablePtr = &PWMTimerMSP432_fxnTable,
        .object = &pwmTimerMSP432Objects[MSP_EXP432P4111_PWM_TA1_1],
        .hwAttrs = &pwmTimerMSP432HWAttrs[MSP_EXP432P4111_PWM_TA1_1]
    },
    {
        .fxnTablePtr = &PWMTimerMSP432_fxnTable,
        .object = &pwmTimerMSP432Objects[MSP_EXP432P4111_PWM_TA1_2],
        .hwAttrs = &pwmTimerMSP432HWAttrs[MSP_EXP432P4111_PWM_TA1_2]
    }
};

const uint_least8_t PWM_count = MSP_EXP432P4111_PWMCOUNT;

#if 0
/*
 *  =============================== SDFatFS ===============================
 */
#include <ti/drivers/SD.h>
#include <ti/drivers/SDFatFS.h>

/*
 * Note: The SDFatFS driver provides interface functions to enable FatFs
 * but relies on the SD driver to communicate with SD cards.  Opening a
 * SDFatFs driver instance will internally try to open a SD driver instance
 * reusing the same index number (opening SDFatFs driver at index 0 will try to
 * open SD driver at index 0).  This requires that all SDFatFs driver instances
 * have an accompanying SD driver instance defined with the same index.  It is
 * acceptable to have more SD driver instances than SDFatFs driver instances
 * but the opposite is not supported & the SDFatFs will fail to open.
 */
SDFatFS_Object sdfatfsObjects[MSP_EXP432P4111_SDFatFSCOUNT];

const SDFatFS_Config SDFatFS_config[MSP_EXP432P4111_SDFatFSCOUNT] = {
    {
        .object = &sdfatfsObjects[MSP_EXP432P4111_SDFatFS0]
    }
};

const uint_least8_t SDFatFS_count = MSP_EXP432P4111_SDFatFSCOUNT;
#endif
/*
 *  =============================== SD ===============================
 */
#include <ti/drivers/SD.h>
#include <ti/drivers/sd/SDSPI.h>

SDSPI_Object sdspiObjects[MSP_EXP432P4111_SDCOUNT];

const SDSPI_HWAttrs sdspiHWAttrs[MSP_EXP432P4111_SDCOUNT] = {
    {
        .spiIndex = MSP_EXP432P4111_SPIB0,
        .spiCsGpioIndex = MSP_EXP432P4111_SDSPI_CS
    }
};

const SD_Config SD_config[MSP_EXP432P4111_SDCOUNT] = {
    {
        .fxnTablePtr = &SDSPI_fxnTable,
        .object = &sdspiObjects[MSP_EXP432P4111_SDSPI0],
        .hwAttrs = &sdspiHWAttrs[MSP_EXP432P4111_SDSPI0]
    },
};

const uint_least8_t SD_count = MSP_EXP432P4111_SDCOUNT;

/*
 *  =============================== SPI ===============================
 */
#include <ti/drivers/SPI.h>
#include <ti/drivers/spi/SPIMSP432DMA.h>

SPIMSP432DMA_Object spiMSP432DMAObjects[MSP_EXP432P4111_SPICOUNT];

/*
 * NOTE: The SPI instances below can be used by the SD driver to communicate
 * with a SD card via SPI.  The 'defaultTxBufValue' fields below are set to 0xFF
 * to satisfy the SDSPI driver requirement.
 */
const SPIMSP432DMA_HWAttrsV1 spiMSP432DMAHWAttrs[MSP_EXP432P4111_SPICOUNT] = {
    {
        .baseAddr = EUSCI_B0_BASE,
        .bitOrder = EUSCI_B_SPI_MSB_FIRST,
        .clockSource = EUSCI_B_SPI_CLOCKSOURCE_SMCLK,
        .defaultTxBufValue = 0xFF,
        .dmaIntNum = INT_DMA_INT1,
        .intPriority = (~0),
        .rxDMAChannelIndex = DMA_CH1_EUSCIB0RX0,
        .txDMAChannelIndex = DMA_CH0_EUSCIB0TX0,
        .clkPin  = SPIMSP432DMA_P1_5_UCB0CLK,
        .simoPin = SPIMSP432DMA_P1_6_UCB0SIMO,
        .somiPin = SPIMSP432DMA_P1_7_UCB0SOMI,
        .stePin  = SPIMSP432DMA_P1_4_UCB0STE,
        .pinMode  = EUSCI_SPI_3PIN,
        .minDmaTransferSize = 10
    },
    {
        .baseAddr = EUSCI_B2_BASE,
        .bitOrder = EUSCI_B_SPI_MSB_FIRST,
        .clockSource = EUSCI_B_SPI_CLOCKSOURCE_SMCLK,
        .defaultTxBufValue = 0xFF,
        .dmaIntNum = INT_DMA_INT2,
        .intPriority = (~0),
        .rxDMAChannelIndex = DMA_CH5_EUSCIB2RX0,
        .txDMAChannelIndex = DMA_CH4_EUSCIB2TX0,
        .clkPin  = SPIMSP432DMA_P3_5_UCB2CLK,
        .simoPin = SPIMSP432DMA_P3_6_UCB2SIMO,
        .somiPin = SPIMSP432DMA_P3_7_UCB2SOMI,
        .stePin  = SPIMSP432DMA_P3_4_UCB2STE,
        .pinMode  = EUSCI_SPI_3PIN,
        .minDmaTransferSize = 10
    },
    {
        .baseAddr = EUSCI_A1_BASE,
        .bitOrder = EUSCI_A_SPI_MSB_FIRST,
        .clockSource = EUSCI_A_SPI_CLOCKSOURCE_SMCLK,
        .defaultTxBufValue = 0xFF,
        .dmaIntNum = INT_DMA_INT2,
        .intPriority = (~0),
        .rxDMAChannelIndex = DMA_CH3_EUSCIA1RX,
        .txDMAChannelIndex = DMA_CH2_EUSCIA1TX,
        .clkPin  = SPIMSP432DMA_P2_5_UCA1CLK,
        .simoPin = SPIMSP432DMA_P2_6_UCA1SIMO,
        .somiPin = SPIMSP432DMA_P2_7_UCA1SOMI,
        .stePin  = SPIMSP432DMA_P2_3_UCA1STE,
        .pinMode  = EUSCI_SPI_4PIN_UCxSTE_ACTIVE_LOW,
        .minDmaTransferSize = 10
    },
    {
        .baseAddr = EUSCI_B2_BASE,
        .bitOrder = EUSCI_B_SPI_MSB_FIRST,
        .clockSource = EUSCI_B_SPI_CLOCKSOURCE_SMCLK,
        .defaultTxBufValue = 0xFF,
        .dmaIntNum = INT_DMA_INT3,
        .intPriority = (~0),
        .rxDMAChannelIndex = DMA_CH5_EUSCIB2RX0,
        .txDMAChannelIndex = DMA_CH4_EUSCIB2TX0,
        .clkPin  = SPIMSP432DMA_P3_5_UCB2CLK,
        .simoPin = SPIMSP432DMA_P3_6_UCB2SIMO,
        .somiPin = SPIMSP432DMA_P3_7_UCB2SOMI,
        .stePin  = SPIMSP432DMA_P2_4_UCB2STE,
        .pinMode  = EUSCI_SPI_4PIN_UCxSTE_ACTIVE_LOW,
        .minDmaTransferSize = 10
    }
};

const SPI_Config SPI_config[MSP_EXP432P4111_SPICOUNT] = {
    {
        .fxnTablePtr = &SPIMSP432DMA_fxnTable,
        .object = &spiMSP432DMAObjects[MSP_EXP432P4111_SPIB0],
        .hwAttrs = &spiMSP432DMAHWAttrs[MSP_EXP432P4111_SPIB0]
    },
    {
        .fxnTablePtr = &SPIMSP432DMA_fxnTable,
        .object = &spiMSP432DMAObjects[MSP_EXP432P4111_SPIB2],
        .hwAttrs = &spiMSP432DMAHWAttrs[MSP_EXP432P4111_SPIB2]
    },
    {
        .fxnTablePtr = &SPIMSP432DMA_fxnTable,
        .object = &spiMSP432DMAObjects[MSP_EXP432P4111_SPIB3],
        .hwAttrs = &spiMSP432DMAHWAttrs[MSP_EXP432P4111_SPIB3]
    },
    {
        .fxnTablePtr = &SPIMSP432DMA_fxnTable,
        .object = &spiMSP432DMAObjects[MSP_EXP432P4111_SPIB4],
        .hwAttrs = &spiMSP432DMAHWAttrs[MSP_EXP432P4111_SPIB4]
    }
};

const uint_least8_t SPI_count = MSP_EXP432P4111_SPICOUNT;

/*
 *  =============================== Timer ===============================
 */
#include <ti/drivers/Timer.h>
#include <ti/drivers/timer/TimerMSP432.h>

TimerMSP432_Object timerMSP432Objects[MSP_EXP432P4111_TIMERCOUNT];

const TimerMSP432_HWAttrs timerMSP432HWAttrs[MSP_EXP432P4111_TIMERCOUNT] = {
    /* Timer32_0 */
    {
        .timerBaseAddress = TIMER32_0_BASE,
        .clockSource = TIMER_A_CLOCKSOURCE_SMCLK,
        .intNum = INT_T32_INT1,
        .intPriority = ~0
    },
    {
        .timerBaseAddress = TIMER32_1_BASE,
        .clockSource = TIMER_A_CLOCKSOURCE_SMCLK,
        .intNum = INT_T32_INT2,
        .intPriority = ~0
    },
    /* Timer_A1 */
    {
        .timerBaseAddress = TIMER_A1_BASE,
        .clockSource = TIMER_A_CLOCKSOURCE_ACLK,
        .intNum = INT_TA1_0,
        .intPriority = ~0
    },
    /* Timer_A2 */
    {
        .timerBaseAddress = TIMER_A2_BASE,
        .clockSource = TIMER_A_CLOCKSOURCE_ACLK,
        .intNum = INT_TA2_0,
        .intPriority = ~0
    },
    /* Timer_A3 */
    {
        .timerBaseAddress = TIMER_A3_BASE,
        .clockSource = TIMER_A_CLOCKSOURCE_ACLK,
        .intNum = INT_TA3_0,
        .intPriority = ~0
    }
};

const Timer_Config Timer_config[MSP_EXP432P4111_TIMERCOUNT] = {
    {
        .fxnTablePtr = &TimerMSP432_Timer32_fxnTable,
        .object = &timerMSP432Objects[MSP_EXP432P4111_TIMER_T32_0],
        .hwAttrs = &timerMSP432HWAttrs[MSP_EXP432P4111_TIMER_T32_0]
    },
    {
        .fxnTablePtr = &TimerMSP432_Timer32_fxnTable,
        .object = &timerMSP432Objects[MSP_EXP432P4111_TIMER_T32_1],
        .hwAttrs = &timerMSP432HWAttrs[MSP_EXP432P4111_TIMER_T32_1]
    },
    {
        .fxnTablePtr = &TimerMSP432_Timer_A_fxnTable,
        .object = &timerMSP432Objects[MSP_EXP432P4111_TIMER_TA_1],
        .hwAttrs = &timerMSP432HWAttrs[MSP_EXP432P4111_TIMER_TA_1]
    },
    {
        .fxnTablePtr = &TimerMSP432_Timer_A_fxnTable,
        .object = &timerMSP432Objects[MSP_EXP432P4111_TIMER_TA_2],
        .hwAttrs = &timerMSP432HWAttrs[MSP_EXP432P4111_TIMER_TA_2]
    },
    {
        .fxnTablePtr = &TimerMSP432_Timer_A_fxnTable,
        .object = &timerMSP432Objects[MSP_EXP432P4111_TIMER_TA_3],
        .hwAttrs = &timerMSP432HWAttrs[MSP_EXP432P4111_TIMER_TA_3]
    }
};

const uint_least8_t Timer_count = MSP_EXP432P4111_TIMERCOUNT;

/*
 *  =============================== UART ===============================
 */
#include <ti/drivers/UART.h>
#include <ti/drivers/uart/UARTMSP432.h>

UARTMSP432_Object uartMSP432Objects[MSP_EXP432P4111_UARTCOUNT];
/* bufferQueue for uart */

kbuf_queue_t g_buf_queue_uart[MSP_EXP432P4111_UARTCOUNT];
char *g_pc_buf_queue_uart[MSP_EXP432P4111_UARTCOUNT] = {0};
aos_mutex_t uart_tx_mutex[MSP_EXP432P4111_UARTCOUNT];
aos_mutex_t uart_rx_mutex[MSP_EXP432P4111_UARTCOUNT];
/* handle for uart */
UART_Handle uart_handle[MSP_EXP432P4111_UARTCOUNT];

/*
 * The baudrate dividers were determined by using the MSP432 baudrate
 * calculator
 * http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
 */
const UARTMSP432_BaudrateConfig uartMSP432Baudrates[] = {
    /* {baudrate, input clock, prescalar, UCBRFx, UCBRSx, oversampling} */
    {
        .outputBaudrate = 115200,
        .inputClockFreq = 24000000,
        .prescalar = 13,
        .hwRegUCBRFx = 0,
        .hwRegUCBRSx = 37,
        .oversampling = 1
    },
    {115200, 12000000,  6,  8,  32, 1},
    {115200, 6000000,   3,  4,   2, 1},
    {115200, 3000000,   1, 10,   0, 1},
    {9600,   24000000, 156,  4,   0, 1},
    {9600,   12000000, 78,  2,   0, 1},
    {9600,   6000000,  39,  1,   0, 1},
    {9600,   3000000,  19,  8,  85, 1},
    {9600,   32768,     3,  0, 146, 0}
};

const UARTMSP432_HWAttrsV1 uartMSP432HWAttrs[MSP_EXP432P4111_UARTCOUNT] = {
    {
        .baseAddr = EUSCI_A0_BASE,
        .intNum = INT_EUSCIA0,
        .intPriority = (~0),
        .clockSource = EUSCI_A_UART_CLOCKSOURCE_SMCLK,
        .bitOrder = EUSCI_A_UART_LSB_FIRST,
        .numBaudrateEntries = sizeof(uartMSP432Baudrates) /
            sizeof(UARTMSP432_BaudrateConfig),
        .baudrateLUT = uartMSP432Baudrates,
        .bufQueuePtr  = &g_buf_queue_uart[MSP_EXP432P4111_UARTA0],
        .bufQueueSize = MAX_BUF_UART_BYTES,
        .rxPin = UARTMSP432_P1_2_UCA0RXD,
        .txPin = UARTMSP432_P1_3_UCA0TXD,
        .errorFxn = NULL
    },
    {
        .baseAddr = EUSCI_A2_BASE,
        .intNum = INT_EUSCIA2,
        .intPriority = (~0),
        .clockSource = EUSCI_A_UART_CLOCKSOURCE_SMCLK,
        .bitOrder = EUSCI_A_UART_LSB_FIRST,
        .numBaudrateEntries = sizeof(uartMSP432Baudrates) /
            sizeof(UARTMSP432_BaudrateConfig),
        .baudrateLUT = uartMSP432Baudrates,
        .bufQueuePtr  = &g_buf_queue_uart[MSP_EXP432P4111_UARTA2],
        .bufQueueSize = MAX_BUF_UART_BYTES,
        .rxPin = UARTMSP432_P3_2_UCA2RXD,
        .txPin = UARTMSP432_P3_3_UCA2TXD,
        .errorFxn = NULL
    }
};

const UART_Config UART_config[MSP_EXP432P4111_UARTCOUNT] = {
    {
        .fxnTablePtr = &UARTMSP432_fxnTable,
        .object = &uartMSP432Objects[MSP_EXP432P4111_UARTA0],
        .hwAttrs = &uartMSP432HWAttrs[MSP_EXP432P4111_UARTA0]
    },
    {
        .fxnTablePtr = &UARTMSP432_fxnTable,
        .object = &uartMSP432Objects[MSP_EXP432P4111_UARTA2],
        .hwAttrs = &uartMSP432HWAttrs[MSP_EXP432P4111_UARTA2]
    }
};

const uint_least8_t UART_count = MSP_EXP432P4111_UARTCOUNT;

/*
 *  =============================== Watchdog ===============================
 */
#include <ti/drivers/Watchdog.h>
#include <ti/drivers/watchdog/WatchdogMSP432.h>

WatchdogMSP432_Object watchdogMSP432Objects[MSP_EXP432P4111_WATCHDOGCOUNT];

const WatchdogMSP432_HWAttrs
    watchdogMSP432HWAttrs[MSP_EXP432P4111_WATCHDOGCOUNT] = {
    {
        .baseAddr = WDT_A_BASE,
        .intNum = INT_WDT_A,
        .intPriority = (~0),
        .clockSource = WDT_A_CLOCKSOURCE_SMCLK,
        .clockDivider = WDT_A_CLOCKDIVIDER_8192K
    }
};

const Watchdog_Config Watchdog_config[MSP_EXP432P4111_WATCHDOGCOUNT] = {
    {
        .fxnTablePtr = &WatchdogMSP432_fxnTable,
        .object = &watchdogMSP432Objects[MSP_EXP432P4111_WATCHDOG],
        .hwAttrs = &watchdogMSP432HWAttrs[MSP_EXP432P4111_WATCHDOG]
    }
};

const uint_least8_t Watchdog_count = MSP_EXP432P4111_WATCHDOGCOUNT;
