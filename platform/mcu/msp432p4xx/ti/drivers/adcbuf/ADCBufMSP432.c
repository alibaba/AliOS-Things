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

#include <stdint.h>
#include <stdbool.h>

/*
 * By default disable both asserts and log for this module.
 * This must be done before DebugP.h is included.
 */
#ifndef DebugP_ASSERT_ENABLED
#define DebugP_ASSERT_ENABLED 0
#endif
#ifndef DebugP_LOG_ENABLED
#define DebugP_LOG_ENABLED 0
#endif

#define MAX_ADC_FREQ 25000000

#include <ti/devices/DeviceFamily.h>

#include <ti/drivers/ADCBuf.h>
#include <ti/drivers/adcbuf/ADCBufMSP432.h>
#include <ti/drivers/timer/TimerMSP432.h>
#include <ti/drivers/dpl/DebugP.h>
#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/dpl/SemaphoreP.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerMSP432.h>

/* driverlib header files */
#include <ti/devices/msp432p4xx/driverlib/adc14.h>
#include <ti/devices/msp432p4xx/driverlib/cs.h>
#include <ti/devices/msp432p4xx/driverlib/dma.h>
#include <ti/devices/msp432p4xx/driverlib/gpio.h>
#include <ti/devices/msp432p4xx/driverlib/interrupt.h>
#include <ti/devices/msp432p4xx/driverlib/ref_a.h>
#include <ti/devices/msp432p4xx/driverlib/rom.h>
#include <ti/devices/msp432p4xx/driverlib/rom_map.h>
#include <ti/devices/msp432p4xx/driverlib/timer_a.h>
#if DeviceFamily_ID == DeviceFamily_ID_MSP432P401x
/* MSP432P401xx devices */
#include <ti/devices/msp432p4xx/driverlib/sysctl.h>
#else
#include <ti/devices/msp432p4xx/driverlib/sysctl_a.h>
#endif
#define ALL_INTERRUPTS  (0xFFFFFFFFFFFFFFFF)
#define MAX_ADC_TRIGGER_SOURCE (7)

#define PinConfigChannel(config) (((config) >> 10) & 0x1F)
#define PinConfigModuleFunction(config) (((config) >> 8) & 0x3)
#define PinConfigPort(config) (((config) >> 4) & 0xF)
#define PinConfigPin(config) (1 << ((config) & 0x7))

void ADCBufMSP432_close(ADCBuf_Handle handle);
int_fast16_t ADCBufMSP432_control(ADCBuf_Handle handle, uint_fast16_t cmd, void * arg);
void ADCBufMSP432_init(ADCBuf_Handle handle);
ADCBuf_Handle ADCBufMSP432_open(ADCBuf_Handle handle, const ADCBuf_Params *params);
int_fast16_t ADCBufMSP432_convert(ADCBuf_Handle handle, ADCBuf_Conversion *conversions, uint_fast8_t channelCount);
int_fast16_t ADCBufMSP432_convertCancel(ADCBuf_Handle handle);
uint_fast8_t ADCBufMSP432_getResolution(ADCBuf_Handle handle);
int_fast16_t ADCBufMSP432_adjustRawValues(ADCBuf_Handle handle, void *sampleBuffer,
        uint_fast16_t sampleCount, uint32_t adcChannel);
int_fast16_t ADCBufMSP432_convertAdjustedToMicroVolts(ADCBuf_Handle handle, uint32_t adcChannel,
        void *adjustedSampleBuffer, uint32_t outputMicroVoltBuffer[], uint_fast16_t sampleCount);
static void blockingConvertCallback(ADCBuf_Handle handle, ADCBuf_Conversion *conversion,
        void *activeADCBuffer, uint32_t completedChannel);
static bool initHw(ADCBufMSP432_Object *object, ADCBufMSP432_HWAttrs const *hwAttrs);
static int_fast16_t configDMA(ADCBuf_Handle handle,
        ADCBufMSP432_HWAttrs const *hwAttrs, ADCBuf_Conversion *conversions);
static void completeConversion(ADCBuf_Handle handle);
static int_fast16_t primeConvert(ADCBuf_Handle handle,
        ADCBufMSP432_HWAttrs const *hwAttrs, ADCBuf_Conversion *conversions,
        uint_fast8_t channelCount);
void ADCBufMSP432DMA_hwiIntFxn(uintptr_t arg);
void ADCBufMSP432_hwiIntFxn(uintptr_t arg);

/* Semaphore to synchronize ADC access */
static SemaphoreP_Handle globalMutex;

/* ADC trigger source table */
static uint32_t adcTriggerTable[MAX_ADC_TRIGGER_SOURCE] = {
    TIMER_A0_BASE + TIMER_A_CAPTURECOMPARE_REGISTER_1,
    TIMER_A0_BASE + TIMER_A_CAPTURECOMPARE_REGISTER_2,
    TIMER_A1_BASE + TIMER_A_CAPTURECOMPARE_REGISTER_1,
    TIMER_A1_BASE + TIMER_A_CAPTURECOMPARE_REGISTER_2,
    TIMER_A2_BASE + TIMER_A_CAPTURECOMPARE_REGISTER_1,
    TIMER_A2_BASE + TIMER_A_CAPTURECOMPARE_REGISTER_2,
    TIMER_A3_BASE + TIMER_A_CAPTURECOMPARE_REGISTER_1
};

/* ADC trigger source table */
static uint32_t adcSampleHoldTrigger[MAX_ADC_TRIGGER_SOURCE] = {
    ADC_TRIGGER_SOURCE1,
    ADC_TRIGGER_SOURCE2,
    ADC_TRIGGER_SOURCE3,
    ADC_TRIGGER_SOURCE4,
    ADC_TRIGGER_SOURCE5,
    ADC_TRIGGER_SOURCE6,
    ADC_TRIGGER_SOURCE7
};

/* ADC function table for ADCBufMSP432 implementation */
const ADCBuf_FxnTable ADCBufMSP432_fxnTable = {
    ADCBufMSP432_close,
    ADCBufMSP432_control,
    ADCBufMSP432_init,
    ADCBufMSP432_open,
    ADCBufMSP432_convert,
    ADCBufMSP432_convertCancel,
    ADCBufMSP432_getResolution,
    ADCBufMSP432_adjustRawValues,
    ADCBufMSP432_convertAdjustedToMicroVolts
};

extern const ADCBuf_Params ADCBuf_defaultParams;


/*
 *  ======== blockingConvertCallback ========
 */
static void blockingConvertCallback(ADCBuf_Handle handle,
        ADCBuf_Conversion *conversion, void *activeADCBuffer,
   uint32_t completedChannel)
{
    ADCBufMSP432_Object *object = handle->object;

    DebugP_log0("ADCBuf: posting transferComplete semaphore");

    /* Indicate transfer complete */
    SemaphoreP_post(object->convertComplete);
}

/*
 *  ======== completeConversion ========
 */
static void completeConversion(ADCBuf_Handle handle)
{
    ADCBufMSP432_Object        *object = handle->object;
    uint_fast8_t               i;

    /* If it is multiple channels sampling, the callback function is invoked
     * until all the channels sampling is finished and callback is called for
     * each channel.
     */
    /* Perform callback in a HWI context. The callback ideally is invoked in
     * SWI instead of HWI */
    for (i = 0; i < object->channelCount; i++) {
        object->callBackFxn(handle, &object->conversions[i],
            (!object->pingpongFlag) ? object->conversions[i].sampleBuffer :
                object->conversions[i].sampleBufferTwo,
            object->conversions[i].adcChannel);
    }

    /* Reset sample index */
    object->conversionSampleIdx = object->conversionSampleCount;

    if (object->recurrenceMode == ADCBuf_RECURRENCE_MODE_ONE_SHOT) {
        /* Clear the object conversions if in the one shot mode */
        Power_releaseConstraint(PowerMSP432_DISALLOW_DEEPSLEEP_0);
        object->conversions = NULL;
    }
    else {
        /* Toggle the pingpong flag */
        object->pingpongFlag ^= 1;

        if (!object->pingpongFlag) {
            object->conversionSampleBuf = object->conversions->sampleBuffer;
        }
        else {
            object->conversionSampleBuf = object->conversions->sampleBufferTwo;
        }
    }
}

/*
 *  ======== completeDMAConversion ========
 */
static void completeDMAConversion(ADCBuf_Handle handle)
{
    ADCBufMSP432_Object        *object = handle->object;
    uint_fast8_t               i;

    /* If it is multiple channels sampling, the callback function is invoked
     * until all the channels sampling is finished and callback is called for
     * each channel.
     */
    /* Perform callback in a HWI context. The callback ideally is invoked in
     * SWI instead of HWI */
    for (i = 0; i < object->channelCount; i++) {
        object->callBackFxn(handle, &object->conversions[i],
            (!object->pingpongFlag) ? object->conversions[i].sampleBuffer :
                object->conversions[i].sampleBufferTwo,
            object->conversions[i].adcChannel);
    }

    if (object->recurrenceMode == ADCBuf_RECURRENCE_MODE_CONTINUOUS) {
        /* Toggle the pingpong flag */
        object->pingpongFlag ^= 1;

        /* Reset sample index */
        object->conversionSampleIdx = object->conversionSampleCount;

        /* Toggle the pingpong flag */
        if (!object->pingpongFlag) {
            object->conversionSampleBuf = object->conversions->sampleBuffer;
        }
        else {
            object->conversionSampleBuf = object->conversions->sampleBufferTwo;
        }
    }
    else {
        /* Clear the object conversions if in the one shot mode */
        object->conversions = NULL;
        Power_releaseConstraint(PowerMSP432_DISALLOW_DEEPSLEEP_0);
    }
}

/*
 *  ======== configDMA ========
 *  This functions configures the DMA to automatically transfer ADC
 *  output data into a provided array
 *
 *  @pre    ADCBufMSP432_open() has to be called first.
 *
 *  @pre    There must not currently be a conversion in progress
 *
 *  @pre    Function assumes that the handle and transaction is not NULL
 *
 *  @param  object An ADCBufMSP432 handle->object returned from
 *                 ADCBufMSP432_open()
 *
 *  @param  hwAttrs An ADCBufMSP432 handle->hwAttrs from board file
 *
 *  @param  conversion A pointer to an ADCBuf_Conversion
 *
 */
static int_fast16_t configDMA(ADCBuf_Handle handle,
    ADCBufMSP432_HWAttrs const *hwAttrs, ADCBuf_Conversion *conversion)
{
    ADCBufMSP432_Object         *object = handle->object;
    UDMAMSP432_PingPongTransfer *pingpongTransfer =
                                            &object->pingpongDMATransfer;
    UDMAMSP432_Transfer *transfer = &object->dmaTransfer;

    /*
     *  If using one-shot mode, set the transfer mode to basic.
     *  Otherwise, continue using ping-pong mode for continuous transfer.
     */
    if (object->recurrenceMode == ADCBuf_RECURRENCE_MODE_CONTINUOUS) {

        /*
         *  Setting Control Indexes for DMA transfer object. In this case
         *  we will set the source of the DMA transfer to ADC14 Memory 0
         *  and the destination to the destination data array.
         */
        pingpongTransfer->dmaChannel = DMA_CH7_ADC14;
        pingpongTransfer->ctlOptions = UDMA_SIZE_16 | UDMA_SRC_INC_NONE |
            UDMA_DST_INC_16 | UDMA_ARB_1;
        pingpongTransfer->dmaTransferSource = (void *) &ADC14->MEM[0];
        pingpongTransfer->dmaPrimaryDestination = conversion->sampleBuffer;
        pingpongTransfer->dmaAlternateDestination =
            conversion->sampleBufferTwo;
        pingpongTransfer->transferSize = conversion->samplesRequestedCount;
        pingpongTransfer->transferMode = UDMA_MODE_PINGPONG;
        /* Initialize PingPong Transfer and return error if unsuccessful */
        if (!UDMAMSP432_setupPingPongTransfer(pingpongTransfer)) {
            return (ADCBuf_STATUS_ERROR);
        }
    }
    else {
        /*
         * Set control indexes for basic DMA transfer object
         */
        transfer->dmaChannel = DMA_CH7_ADC14;
        transfer->ctlOptions = UDMA_SIZE_16 | UDMA_SRC_INC_NONE |
            UDMA_DST_INC_16 | UDMA_ARB_1;
        transfer->dmaTransferSource = (void *) &ADC14->MEM[0];
        transfer->dmaTransferDestination = conversion->sampleBuffer;
        transfer->transferSize = conversion->samplesRequestedCount;
        transfer->structSelect = UDMA_PRI_SELECT;
        transfer->transferMode = UDMA_MODE_BASIC;
        if (!UDMAMSP432_setupTransfer((UDMAMSP432_Transfer *) transfer)) {
            return (ADCBuf_STATUS_ERROR);
        }
    }

    return (ADCBuf_STATUS_SUCCESS);
}

/*
 *  ======== initHW ========
 *
 *  Configures ADC peripheral
 */
static bool initHw(ADCBufMSP432_Object *object,
        ADCBufMSP432_HWAttrs const *hwAttrs)
{
    uint32_t timerAddr;
    uint32_t timerCCRAddr;
    PowerMSP432_Freqs         powerFreqs;

    PowerMSP432_getFreqs(Power_getPerformanceLevel(), &powerFreqs);

    /* Fail if clock frequency is greater than 25MHz */
    if (((hwAttrs->clockSource == ADCBufMSP432_HSMCLK_CLOCK)
            && (powerFreqs.HSMCLK > MAX_ADC_FREQ))
        || ((hwAttrs->clockSource == ADCBufMSP432_SMCLK_CLOCK)
            && (powerFreqs.SMCLK > MAX_ADC_FREQ))
        || ((hwAttrs->clockSource == ADCBufMSP432_MCLK_CLOCK)
            && (powerFreqs.MCLK > MAX_ADC_FREQ))
        || ((hwAttrs->clockSource == ADCBufMSP432_ACLK_CLOCK)
            && (powerFreqs.ACLK > MAX_ADC_FREQ))) {
        return (ADCBuf_STATUS_ERROR);
    }

    object->internalSourceMask = 0;
    /* Initializing ADC */
    MAP_ADC14_enableModule();
    MAP_ADC14_initModule(hwAttrs->clockSource, ADC_PREDIVIDER_1, ADC_DIVIDER_1,
        object->internalSourceMask);
    if (hwAttrs->adcTriggerSource == ADCBufMSP432_TIMER_TRIGGER) {
        timerAddr = (adcTriggerTable[hwAttrs->adcTimerTriggerSource]
            & 0xFFFFFF00);
        timerCCRAddr = (adcTriggerTable[hwAttrs->adcTimerTriggerSource]
            & 0xFF);

        /* Set trigger source */
        MAP_ADC14_setSampleHoldTrigger(
            adcSampleHoldTrigger[hwAttrs->adcTimerTriggerSource],
            false);

        if (!TimerMSP432_allocateTimerResource(timerAddr)) {
            return (ADCBuf_STATUS_ERROR);
        }

        /* Init the timer CCR0 for up mode*/
        Timer_A_UpModeConfig upConfig = {0};
        upConfig.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;
        upConfig.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
        upConfig.timerPeriod = powerFreqs.SMCLK / object->samplingFrequency;
        upConfig.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE;
        upConfig.captureCompareInterruptEnable_CCR0_CCIE =
            TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE;
        upConfig.timerClear = TIMER_A_DO_CLEAR;

        /* Init compare mode to generate PWM1 */
        Timer_A_CompareModeConfig compConfig = {0};
        compConfig.compareRegister = timerCCRAddr;
        compConfig.compareInterruptEnable =
            TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE;
        compConfig.compareOutputMode = TIMER_A_OUTPUTMODE_RESET_SET;
        if ((hwAttrs->timerDutyCycle != 0) &&
            (hwAttrs->timerDutyCycle) < 100) {
            /* duty cycle */
            compConfig.compareValue = upConfig.timerPeriod *
                (hwAttrs->timerDutyCycle) / 100;
        }
        else {
            /* default duty cycle 50% */
            compConfig.compareValue = upConfig.timerPeriod / 2;
        }

        /* timer has additional clock for transition from N to 0. */
        upConfig.timerPeriod--;
        MAP_Timer_A_configureUpMode(timerAddr, &upConfig);
        MAP_Timer_A_initCompare(timerAddr, &compConfig);

        object->timerAddr = timerAddr;
    }
    else {
        /* Set trigger source */
        MAP_ADC14_setSampleHoldTrigger(ADC_TRIGGER_ADCSC, false);
    }
    if (hwAttrs->adcTriggerSource == ADCBufMSP432_SOFTWARE_AUTOMATIC_TRIGGER) {
        /* Set sample/hold time */
        MAP_ADC14_setSampleHoldTime(object->samplingDuration, object->samplingDuration);
    }

    return (ADCBuf_STATUS_SUCCESS);
}

/*
 *  ======== primeConvert ========
 */
static int_fast16_t primeConvert(ADCBuf_Handle handle,
        ADCBufMSP432_HWAttrs const *hwAttrs, ADCBuf_Conversion *conversions,
        uint_fast8_t channelCount)
{
    ADCBufMSP432_Object  *object = handle->object;
    uint32_t channelInt;
    uint32_t memory = ADC_MEM0;
    uint_fast8_t i = 0;

    /* Store the conversions struct array into object */
    object->conversions = conversions;
    /* Store the channel count into object */
    object->channelCount = channelCount;

    /* For multiple channels sampling, ref source and sampling duration should be same */
    uint32_t refSource =
        hwAttrs->channelSetting[conversions[0].adcChannel].refSource;

    for (i = 0; i < channelCount; i++){
        if (refSource !=
            hwAttrs->channelSetting[conversions[i].adcChannel].refSource) {
            return (ADCBuf_STATUS_ERROR);
        }
    }

    /* Set reference voltage for current conversion */
    uint32_t refVolts =
        hwAttrs->channelSetting[conversions[0].adcChannel].refVoltage;
    uint16_t refVoltsDef = 0;
    uint32_t sysRef;
    int_fast16_t ret = ADCBuf_STATUS_SUCCESS;
    if (refSource == ADCBufMSP432_VREFPOS_INTBUF_VREFNEG_VSS) {

        switch(refVolts) {
        case 1200000:
            refVoltsDef = REF_A_VREF1_2V;
#if DeviceFamily_ID == DeviceFamily_ID_MSP432P401x
            sysRef = SYSCTL_1_2V_REF;
#else
            sysRef = SYSCTL_A_1_2V_REF;
#endif
            break;
        case 1450000:
            refVoltsDef = REF_A_VREF1_45V;
#if DeviceFamily_ID == DeviceFamily_ID_MSP432P401x
            sysRef = SYSCTL_1_45V_REF;
#else
            sysRef = SYSCTL_A_1_45V_REF;
#endif
            break;
        case 2500000:
            refVoltsDef = REF_A_VREF2_5V;
#if DeviceFamily_ID == DeviceFamily_ID_MSP432P401x
            sysRef = SYSCTL_2_5V_REF;
#else
            sysRef = SYSCTL_A_2_5V_REF;
#endif
            break;
        default:
			return (ADCBuf_STATUS_ERROR);
            //break;
        }

        MAP_REF_A_setReferenceVoltage(refVoltsDef);
        MAP_REF_A_enableReferenceVoltage();
    }
    else {
#if DeviceFamily_ID == DeviceFamily_ID_MSP432P401x
        sysRef = SYSCTL_2_5V_REF;
#else
        sysRef = SYSCTL_A_2_5V_REF;
#endif
    }

    object->internalSourceMask = 0;

    /* Config GPIOs and special modes for ADC channel analog in */
    for (i = 0; i < channelCount; i++) {
        /* Check if in temperature mode and initialize accordingly */
        if (hwAttrs->channelSetting[conversions[i].adcChannel].adcInternalSource
                == ADCBufMSP432_TEMPERATURE_MODE) {
            object->internalSourceMask |= ADCBufMSP432_TEMPERATURE_MODE;
            MAP_REF_A_enableTempSensor();
#if DeviceFamily_ID == DeviceFamily_ID_MSP432P401x
            object->tempCal30 = MAP_SysCtl_getTempCalibrationConstant(sysRef,
                SYSCTL_30_DEGREES_C);
            object->tempCal85 = MAP_SysCtl_getTempCalibrationConstant(sysRef,
                SYSCTL_85_DEGREES_C);
#else
            object->tempCal30 = MAP_SysCtl_A_getTempCalibrationConstant(sysRef,
                SYSCTL_A_30_DEGREES_C);
            object->tempCal85 = MAP_SysCtl_A_getTempCalibrationConstant(sysRef,
                SYSCTL_A_85_DEGREES_C);
#endif
            object->tempCalDifference = object->tempCal85 - object->tempCal30;
        }
        /* Check if in battery monitor mode and set channel mask
           for initialization */
        else if (hwAttrs->
                channelSetting[conversions[i].adcChannel].adcInternalSource ==
                ADCBufMSP432_BATTERY_MONITOR_MODE) {
            object->internalSourceMask |= ADCBufMSP432_BATTERY_MONITOR_MODE;
        }
        /* Initialize differential pin if in differential mode */
        else if (hwAttrs->
                channelSetting[conversions[i].adcChannel].adcDifferentialPin !=
                ADCBufMSP432_PIN_NONE) {
            MAP_GPIO_setAsPeripheralModuleFunctionInputPin(
                PinConfigPort(hwAttrs->
                    channelSetting[conversions[i].adcChannel].adcPin),
                PinConfigPin(hwAttrs->
                    channelSetting[conversions[i].adcChannel].adcPin)
                    | PinConfigPin(hwAttrs->
                    channelSetting[conversions[i].adcChannel].adcDifferentialPin
                ),
                GPIO_TERTIARY_MODULE_FUNCTION);
        }
        else {
            /* Config GPIO for ADC channel analog input */
            MAP_GPIO_setAsPeripheralModuleFunctionInputPin(
                PinConfigPort(hwAttrs->
                    channelSetting[conversions[i].adcChannel].adcPin),
                PinConfigPin(hwAttrs->
                    channelSetting[conversions[i].adcChannel].adcPin),
                PinConfigModuleFunction(hwAttrs->
                    channelSetting[conversions[i].adcChannel].adcPin));
        }
   }
    /* Update CTL registers for temperature and/or battery monitor modes */
    MAP_ADC14_initModule(hwAttrs->clockSource, ADC_PREDIVIDER_1, ADC_DIVIDER_1,
        object->internalSourceMask);

    /* ADC single channel sampling */
    if (channelCount == 1) {
        if (conversions->samplesRequestedCount > 1) {
            MAP_ADC14_configureSingleSampleMode(ADC_MEM0, true);
        }
        else {
            MAP_ADC14_configureSingleSampleMode(ADC_MEM0, false);
        }

        refSource = hwAttrs->channelSetting[conversions->adcChannel].refSource;

        /* Configure channels for temperature mode */
        if (hwAttrs->channelSetting[conversions->adcChannel].adcInternalSource
                == ADCBufMSP432_TEMPERATURE_MODE) {
            MAP_ADC14_configureConversionMemory(ADC_MEM0,
                refSource,
                ADC_INPUT_A22,
                hwAttrs->channelSetting[conversions[0].adcChannel].adcInputMode
            );
        }
        /* Configure channels for battery monitor mode */
        else if (hwAttrs->
                channelSetting[conversions->adcChannel].adcInternalSource ==
                ADCBufMSP432_BATTERY_MONITOR_MODE) {
            MAP_ADC14_configureConversionMemory(ADC_MEM0,
                refSource,
                ADC_INPUT_A23,
                hwAttrs->channelSetting[conversions[0].adcChannel].adcInputMode
            );
        }
        /* Configure channels for regular ADC operation */
        else {
            MAP_ADC14_configureConversionMemory(ADC_MEM0,
                refSource,
                PinConfigChannel(hwAttrs->
                    channelSetting[conversions[0].adcChannel].adcPin),
                hwAttrs->channelSetting[conversions[0].adcChannel].adcInputMode
            );
        }

        channelInt = ADC_INT0;
        if (hwAttrs->channelSetting[conversions[0].adcChannel].adcInputMode ==
                ADCBufMSP432_DIFFERENTIAL) {
            MAP_ADC14_setResultFormat(ADC_SIGNED_BINARY);
        }

        /* Store the samples count into object */
        object->conversionSampleBuf = conversions->sampleBuffer;
        object->conversionSampleCount = conversions->samplesRequestedCount;
        object->conversionSampleIdx = conversions->samplesRequestedCount;
    }
    else {
        /* ADC multiple channel sampling */
        if (conversions->samplesRequestedCount > 1) {
            MAP_ADC14_configureMultiSequenceMode(memory,
                memory << (channelCount - 1), true);
        }
        else {
            MAP_ADC14_configureMultiSequenceMode(memory,
                memory << (channelCount - 1), false);
        }

        for (i = 0; i < channelCount; i++, memory = memory << 1) {

            refSource =
                hwAttrs->channelSetting[conversions[i].adcChannel].refSource;

            /* If channel in temperature mode, set input channel to A22 */
            if (hwAttrs->
                    channelSetting[conversions[i].adcChannel].adcInternalSource
                    == ADCBufMSP432_TEMPERATURE_MODE) {
                MAP_ADC14_configureConversionMemory(memory, refSource,
                    ADC_INPUT_A22,
                    hwAttrs->
                        channelSetting[conversions[i].adcChannel].adcInputMode
                );
            }
            /* If channel in battery monitor mode, set input channel to A23 */
            else if (hwAttrs->
                    channelSetting[conversions[i].adcChannel].adcInternalSource
                    == ADCBufMSP432_BATTERY_MONITOR_MODE) {
                MAP_ADC14_configureConversionMemory(memory,
                    refSource,
                    ADC_INPUT_A23,
                    hwAttrs->
                        channelSetting[conversions[i].adcChannel].adcInputMode
                );
            }
            /* Otherwise use user specified ADC channel */
            else {
                MAP_ADC14_configureConversionMemory(memory, refSource,
                    PinConfigChannel(
                    hwAttrs->channelSetting[conversions[i].adcChannel].adcPin),
                    hwAttrs->
                        channelSetting[conversions[i].adcChannel].adcInputMode
                );
            }

            /* If using differential mode, set the results to signed format */
            if (hwAttrs->channelSetting[conversions[i].adcChannel].adcInputMode
                    == ADCBufMSP432_DIFFERENTIAL) {
                MAP_ADC14_setResultFormat(ADC_SIGNED_BINARY);
            }
        }

        channelInt = 1 << (channelCount - 1);

        /* Store the samples count into object */
        object->conversionSampleCount = conversions[0].samplesRequestedCount;
        object->conversionSampleIdx = conversions[0].samplesRequestedCount;
    }

    /* Configuring Sample Timer */
    if (ADCBufMSP432_SOFTWARE_AUTOMATIC_TRIGGER == hwAttrs->adcTriggerSource) {
        MAP_ADC14_enableSampleTimer(ADC_AUTOMATIC_ITERATION);
    }
    else {
        MAP_ADC14_disableSampleTimer();
    }

    /* Enabling interrupts */
    if (!hwAttrs->useDMA) {
        MAP_ADC14_clearInterruptFlag(channelInt);
        MAP_ADC14_enableInterrupt(channelInt);
    }
    else {
        ret = configDMA(handle, hwAttrs, conversions);
    }
    /* Enabling Conversion */
    MAP_ADC14_enableConversion();
    if (ADCBufMSP432_SOFTWARE_AUTOMATIC_TRIGGER ==
            hwAttrs->adcTriggerSource) {
        MAP_ADC14_toggleConversionTrigger();
    }

    /* Enabling Interrupts */
    if (!hwAttrs->useDMA) {
        MAP_Interrupt_enableInterrupt(INT_ADC14);
    }
    MAP_Interrupt_enableMaster();

    if (hwAttrs->adcTriggerSource == ADCBufMSP432_TIMER_TRIGGER) {
    /* Trigger conversion either from Timer PWM */
    MAP_Timer_A_startCounter(object->timerAddr,
            TIMER_A_UP_MODE);
    }

    return (ret);
}

/*
 *  ======== ADCBufCCMSP432_adjustRawValues ========
 */
int_fast16_t ADCBufMSP432_adjustRawValues(ADCBuf_Handle handle,
    void *sampleBuffer, uint_fast16_t sampleCount,
    uint32_t adcChannel)
{
    /* This hardware peripheral does not support Calibration */
    return (ADCBuf_STATUS_UNSUPPORTED);
}

/*
 *  ======== ADCBufMSP432_close ========
 */
void ADCBufMSP432_close(ADCBuf_Handle handle)
{
    uintptr_t         key;
    ADCBufMSP432_Object        *object = handle->object;
    ADCBufMSP432_HWAttrs const *hwAttrs = handle->hwAttrs;
    uint32_t timerAddr = (adcTriggerTable[hwAttrs->adcTimerTriggerSource]
        & 0xFFFFFF00);

    key = HwiP_disable();
    /* Disable interrupts & the ADC */
    MAP_ADC14_disableInterrupt(ALL_INTERRUPTS);
    MAP_ADC14_disableModule();

    HwiP_restore(key);

    /* Destruct driver resources */
    if (object->hwiHandle) {
        HwiP_delete(object->hwiHandle);
    }

    if (object->convertComplete) {
        SemaphoreP_delete(object->convertComplete);
    }

    if (hwAttrs->useDMA) {
        if (object->dmaHandle) {
            UDMAMSP432_close(object->dmaHandle,
                DMA_CH7_ADC14, hwAttrs->dmaIntNum);
        }
    }
    /* Freeing up the resource with the Timer driver */
    if (hwAttrs->adcTriggerSource == ADCBufMSP432_TIMER_TRIGGER) {
        TimerMSP432_freeTimerResource(timerAddr);
    }

    object->isOpen = false;

    DebugP_log0("ADCBuf: Object closed.");
}

/*
 *  ======== ADCBufMSP432_control ========
 */
int_fast16_t ADCBufMSP432_control(ADCBuf_Handle handle,
    uint_fast16_t cmd, void * arg)
{
    ADCBufMSP432_HWAttrs const *hwAttrs = handle->hwAttrs;
    int_fast16_t status = ADCBuf_STATUS_ERROR;
    ADCBufMSP432_Object *object = handle->object;

    DebugP_assert(handle);

    switch (cmd) {
        case ADCBufMSP432_CMD_ENTER_ADC_ULTRA_LOW_POWER_MODE:
            if (MAP_ADC14_initModule(ADC_CLOCKSOURCE_ADCOSC,
                    ADC_PREDIVIDER_1, ADC_DIVIDER_1,
                    object->internalSourceMask)) {
                if (MAP_ADC14_enableReferenceBurst()) {
                    if (MAP_ADC14_setPowerMode(ADC_ULTRA_LOW_POWER_MODE)) {
                        MAP_ADC14_setResolution(ADC_8BIT);
                        status = ADCBuf_STATUS_SUCCESS;
                    }
                }
            }
            if(ADCBuf_STATUS_SUCCESS != status) {
                MAP_ADC14_initModule(hwAttrs->clockSource, ADC_PREDIVIDER_1,
                    ADC_DIVIDER_1, object->internalSourceMask);
                MAP_ADC14_disableReferenceBurst();
            }
            break;
        case ADCBufMSP432_CMD_EXIT_ADC_ULTRA_LOW_POWER_MODE:
            if (MAP_ADC14_initModule(hwAttrs->clockSource, ADC_PREDIVIDER_1,
                    ADC_DIVIDER_1, object->internalSourceMask)) {
                if (MAP_ADC14_disableReferenceBurst()) {
                    if (MAP_ADC14_setPowerMode(ADC_UNRESTRICTED_POWER_MODE)) {
                        MAP_ADC14_setResolution(ADC_14BIT);
                        status = ADCBuf_STATUS_SUCCESS;
                    }
                }
            }
            if(ADCBuf_STATUS_SUCCESS != status) {
                MAP_ADC14_initModule(ADC_CLOCKSOURCE_ADCOSC, ADC_PREDIVIDER_1,
                    ADC_DIVIDER_1, object->internalSourceMask);
                MAP_ADC14_enableReferenceBurst();
            }
            break;
        default:
            status = ADCBuf_STATUS_UNDEFINEDCMD;
            break;
    }
    return (status);
}

/*
 *  ======== ADCBufMSP432_convert ========
 */
int_fast16_t ADCBufMSP432_convert(ADCBuf_Handle handle,
    ADCBuf_Conversion *conversions,
    uint_fast8_t channelCount)
{
    ADCBufMSP432_Object        *object = handle->object;
    ADCBufMSP432_HWAttrs const *hwAttrs = handle->hwAttrs;
    int_fast16_t ret;

    /* Acquire the lock for this particular ADC handle */
    SemaphoreP_pend(globalMutex, SemaphoreP_WAIT_FOREVER);

    /*
     * Set power constraints to keep peripheral active during transfer
     * and to prevent a performance level change
     */
    Power_setConstraint(PowerMSP432_DISALLOW_DEEPSLEEP_0);
    Power_setConstraint(PowerMSP432_DISALLOW_PERF_CHANGES);

    /* Execute core conversion */
    ret = primeConvert(handle, hwAttrs, conversions, channelCount);
    if (ret == ADCBuf_STATUS_ERROR) {
        return (ret);
    }

    if (object->returnMode == ADCBuf_RETURN_MODE_BLOCKING) {
        DebugP_log0("ADCBuf: Pending on transferComplete semaphore");
        /*
         * Wait for the transfer to complete here.
         * It's OK to block from here because the ADC's Hwi will unblock
         * upon errors
         */
        if (SemaphoreP_OK != SemaphoreP_pend(object->convertComplete,
                    object->semaphoreTimeout)) {
            DebugP_log0("ADCBuf: Convert timeout");
            ret = ADCBuf_STATUS_ERROR;
        }
        else {
            DebugP_log0("ADCBuf: Convert completed");
            ret = ADCBuf_STATUS_SUCCESS;
        }
    }
    else {
        /* Always return true if in Asynchronous mode */
        ret = ADCBuf_STATUS_SUCCESS;
    }

    /* Release the lock for this particular ADC handle */
    SemaphoreP_post(globalMutex);

    /* Return the number of bytes transfered by the ADC */
    return (ret);
}


/*
 *  ======== ADCBufMSP432_convertAdjustedToMicroVolts ========
 */
int_fast16_t ADCBufMSP432_convertAdjustedToMicroVolts(ADCBuf_Handle handle,
    uint32_t adcChannel, void *adjustedSampleBuffer,
    uint32_t outputMicroVoltBuffer[], uint_fast16_t sampleCount)
{
    ADCBufMSP432_HWAttrs const *hwAttrs = handle->hwAttrs;
    uint32_t refVoltage = hwAttrs->channelSetting[adcChannel].refVoltage;
    uint32_t i;
    uint16_t *adjustedRawSampleBuf = (uint16_t *) adjustedSampleBuffer;

    for (i = 0; i < sampleCount; i++) {
        if (adjustedRawSampleBuf[i] == 0x3FFF) {
            outputMicroVoltBuffer[i] = refVoltage;
        }
        else if (adjustedRawSampleBuf[i] == 0) {
            outputMicroVoltBuffer[i] = adjustedRawSampleBuf[i];
        }
        else {
            outputMicroVoltBuffer[i] = ((uint32_t)adjustedRawSampleBuf[i] *
                (refVoltage * 10 / 0x4000)) / 10;
        }
    }

    return (ADCBuf_STATUS_SUCCESS);
}

/*
 *  ======== ADCBufMSP432_convertCancel ========
 */
int_fast16_t ADCBufMSP432_convertCancel(ADCBuf_Handle handle)
{
    MAP_ADC14_disableConversion();

    MAP_ADC14_clearInterruptFlag(ALL_INTERRUPTS);

    completeConversion(handle);
    Power_releaseConstraint(PowerMSP432_DISALLOW_DEEPSLEEP_0);

    return (ADCBuf_STATUS_SUCCESS);
}

/*
 *  ======== ADCBufMSP432_getResolution ========
 */
uint_fast8_t ADCBufMSP432_getResolution(ADCBuf_Handle handle)
{
    return (14);
}

/*
 *  ======== ADCBufMSP432_hwiIntFxn ========
 */
void ADCBufMSP432_hwiIntFxn(uintptr_t arg)
{
    uint64_t                  intStatus;
    uint_fast8_t              i;
    ADCBufMSP432_Object       *object = ((ADCBuf_Handle) arg)->object;
    uint16_t                  *sampleBuffer;
    ADCBufMSP432_HWAttrs const *hwAttrs = ((ADCBuf_Handle) arg)->hwAttrs;

    /* Get the interrupt status of the ADC controller */
    intStatus = MAP_ADC14_getEnabledInterruptStatus();
    MAP_ADC14_clearInterruptFlag(intStatus);

    if (object->channelCount == 1) {
        if (ADC_INT0 & intStatus) {
            *(object->conversionSampleBuf) = MAP_ADC14_getResult(ADC_MEM0);
            object->conversionSampleBuf++;
            object->conversionSampleIdx--;
        }
    }
    else {
        /* Multiple channels sampling */
        if ((1 << (object->channelCount - 1)) & intStatus) {
            /* Current round of sequential sampling */
            uint_fast16_t sampleIdx = object->conversionSampleCount -
                object->conversionSampleIdx;

            for (i = 0; i < object->channelCount; i++) {
                sampleBuffer = (!object->pingpongFlag) ?
                    (uint16_t *)(object->conversions[i].sampleBuffer) :
                    (uint16_t *)(object->conversions[i].sampleBufferTwo);
                sampleBuffer[sampleIdx] = MAP_ADC14_getResult(ADC_MEM0 << i);
            }
            object->conversionSampleIdx--;
        }
    }

    /* ADC conversion complete */
    if (object->conversionSampleIdx == 0) {
        if (object->recurrenceMode == ADCBuf_RECURRENCE_MODE_ONE_SHOT) {
            /* Stop sampling */
            MAP_ADC14_disableConversion();

            /* Remedy race condition causing potential unintended re-entry
                into the hwiIntFxn */
            HwiP_clearInterrupt(INT_ADC14);

            intStatus = MAP_ADC14_getEnabledInterruptStatus();
            MAP_ADC14_clearInterruptFlag(intStatus);

            if (hwAttrs->adcTriggerSource == ADCBufMSP432_TIMER_TRIGGER) {
                /* Trigger conversion either from Timer PWM */
                MAP_Timer_A_stopTimer(object->timerAddr);
            }
        }
        completeConversion((ADCBuf_Handle) arg);
    }
}
/*
 *  ======== ADCBufMSP432DMA_hwiIntFxn ========
 */
void ADCBufMSP432DMA_hwiIntFxn(uintptr_t arg)
{
    ADCBufMSP432_Object       *object = ((ADCBuf_Handle) arg)->object;
    ADCBufMSP432_HWAttrs const *hwAttrs = ((ADCBuf_Handle) arg)->hwAttrs;

    completeDMAConversion((ADCBuf_Handle) arg);
    if (object->recurrenceMode == ADCBuf_RECURRENCE_MODE_CONTINUOUS) {

        /* Switch between primary and alternate buffers with DMA's PingPong
            mode */
        UDMAMSP432_PingPongToggleBuffer(&object->pingpongDMATransfer);

        MAP_DMA_enableChannel(DMA_CH7_ADC14 & 0x3f);
    }
    else {
        /* Stop sampling if in One-Shot Mode */
        MAP_DMA_disableChannel(DMA_CH7_ADC14 & 0x3f);
        MAP_ADC14_disableModule();
        MAP_ADC14_disableConversion();
        if (hwAttrs->adcTriggerSource == ADCBufMSP432_TIMER_TRIGGER) {
            /* Trigger conversion either from Timer PWM */
            MAP_Timer_A_stopTimer(object->timerAddr);
        }
    }
}

/*
 *  ======== ADCBufMSP432_init ========
 */
void ADCBufMSP432_init(ADCBuf_Handle handle)
{
    ADCBufMSP432_Object        *object;

    ADCBufMSP432_HWAttrs const *hwAttrs = handle->hwAttrs;
    uintptr_t                   key;
    SemaphoreP_Handle           sem;

    /* Speculatively create a binary semaphore for thread safety */
    sem = SemaphoreP_createBinary(1);
    /* sem == NULL will be detected in 'open' */

    key = HwiP_disable();

    /* Create Semaphore for ADC0 */
    if (globalMutex == NULL) {
        /* Use the binary sem created above */
        globalMutex = sem;

        HwiP_restore(key);
    }
    else {
        /* Init already called */
        HwiP_restore(key);

        if (sem) {
            /* Delete unused Semaphore */
            SemaphoreP_delete(sem);
        }
    }

    /* Get the pointer to the object */
    object = handle->object;
    /* Mark the object as available */
    object->isOpen = false;
    if (hwAttrs->useDMA) {
        UDMAMSP432_init();
    }
}

/*
 *  ======== ADCBufMSP432_open ========
 */
ADCBuf_Handle ADCBufMSP432_open(ADCBuf_Handle handle,
                                const ADCBuf_Params *params)
{
    uintptr_t                key;
    HwiP_Params          hwiParams;

    ADCBufMSP432_Object        *object = handle->object;
    ADCBufMSP432_HWAttrs const *hwAttrs = handle->hwAttrs;

    if (globalMutex == NULL){
        ADCBufMSP432_init(handle);
        if (globalMutex == NULL) {
            DebugP_log0("ADCBuf: mutex Semaphore_create() failed:.");
            ADCBufMSP432_close(handle);
            return (NULL);
        }
    }

    /* Use defaults if params are NULL. */
    if (params == NULL) {
        params = (ADCBuf_Params *) &ADCBuf_defaultParams;
    }

    /* Check that a callback is provided if using callback mode */
    DebugP_assert(params->returnMode == ADCBuf_RETURN_MODE_CALLBACK &&
        params->callbackFxn != NULL);

    if (ADCBuf_RETURN_MODE_CALLBACK == params->returnMode) {
        if (params->callbackFxn == NULL) {
            return (NULL);
        }
    }
    /* Check that if it is the callback mode when using continuous mode */
    DebugP_assert((ADCBuf_RECURRENCE_MODE_CONTINUOUS == params->recurrenceMode)
        && (ADCBuf_RETURN_MODE_CALLBACK == params->returnMode));

    if (params->recurrenceMode == ADCBuf_RECURRENCE_MODE_CONTINUOUS) {
        if (params->returnMode != ADCBuf_RETURN_MODE_CALLBACK) {
            return (NULL);
        }
    }

    /* Determine if the driver was already opened */
    key = HwiP_disable();

    if (object->isOpen) {
        HwiP_restore(key);
        DebugP_log0("ADCBuf:Error! Already in use.");
        return (NULL);
    }
    object->isOpen = true;

    HwiP_restore(key);

    /*
     * Add power management support - Disable performance transitions while
     * opening the driver.
     */
    Power_setConstraint(PowerMSP432_DISALLOW_PERF_CHANGES);

    /* Create Hwi object for ADC */
    if (!hwAttrs->useDMA) {
        HwiP_Params_init(&hwiParams);
        hwiParams.arg = (uintptr_t) handle;
        hwiParams.priority = hwAttrs->intPriority;
        object->hwiHandle = HwiP_create(INT_ADC14, ADCBufMSP432_hwiIntFxn,
            &hwiParams);
        if (!object->hwiHandle) {
            DebugP_log0("ADCBuf: HwiP_create() failed.");
            Power_releaseConstraint(PowerMSP432_DISALLOW_PERF_CHANGES);
            ADCBufMSP432_close(handle);
            return (NULL);
        }
    }

    if (hwAttrs->useDMA) {
        object->dmaHandle = UDMAMSP432_open(
            DMA_CH7_ADC14,
            hwAttrs->dmaIntNum,
            hwAttrs->intPriority,
            ADCBufMSP432DMA_hwiIntFxn,
            (uintptr_t) handle
        );
        if (object->dmaHandle == NULL) {
            DebugP_log1("ADCBuf:(%p) UDMAMSP432_open() failed.",
                hwAttrs->baseAddr);
            Power_releaseConstraint(PowerMSP432_DISALLOW_PERF_CHANGES);
            ADCBufMSP432_close(handle);
            return (NULL);
        }
    }

    /* Configure driver to Callback or Blocking operating mode */
    if (params->returnMode == ADCBuf_RETURN_MODE_CALLBACK) {
        object->callBackFxn = params->callbackFxn;
        DebugP_log0("ADCBuf: in ADC_MODE_CALLBACK mode");
    }
    else {
        /* Semaphore to block task for the duration of the ADC convert */
        object->convertComplete = SemaphoreP_createBinary(0);
        if (!object->convertComplete) {
            DebugP_log0("ADCBuf: convert SemaphoreP_create() failed.");
            Power_releaseConstraint(PowerMSP432_DISALLOW_PERF_CHANGES);
            ADCBufMSP432_close(handle);
            return (NULL);
        }
        object->callBackFxn = blockingConvertCallback;
        DebugP_log0("ADCBuf: in ADC_MODE_BLOCKING mode");
    }

    /*
     * Store ADC parameters & initialize peripheral. These are used to
     * re/initialize the peripheral when opened or changing performance level.
     */
    object->pingpongFlag = 0;
    object->returnMode = params->returnMode;
    object->recurrenceMode = params->recurrenceMode;
    object->semaphoreTimeout = params->blockingTimeout;
    object->samplingFrequency = params->samplingFrequency;

    /* Check the ExtensionParam is set */
    if (params->custom) {
        /* If MSP432 specific params were specified, use them */
        object->samplingDuration =
            ((ADCBufMSP432_ParamsExtension *)(params->custom))->
            samplingDuration;
    }
    else {
        /* Initialise MSP432 specific settings to defaults */
        object->samplingDuration = ADCBufMSP432_SamplingDuration_PULSE_WIDTH_4;
    }

    /* Initialize ADC related hardware */
    if (ADCBuf_STATUS_SUCCESS != initHw(object, hwAttrs)) {
        if (hwAttrs->useDMA) {
            UDMAMSP432_close(object->dmaHandle,
                DMA_CH7_ADC14,
                hwAttrs->dmaIntNum);
            object->dmaHandle = NULL;
        }
        else {
            HwiP_delete(object->hwiHandle);
            object->hwiHandle = NULL;
        }

        /* Allow performance level changes */
        Power_releaseConstraint(PowerMSP432_DISALLOW_PERF_CHANGES);
        return (NULL);
    }

    /* Allow performance level changes */
    Power_releaseConstraint(PowerMSP432_DISALLOW_PERF_CHANGES);

    DebugP_log0("ADCBuf: Object opened.");

    return (handle);
}
