/*
 * Copyright (c) 2016-2017 Texas Instruments Incorporated
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

#include <ti/devices/DeviceFamily.h>

#include <ti/drivers/ADC.h>
#include <ti/drivers/adc/ADCMSP432.h>
#include <ti/drivers/dpl/DebugP.h>
#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/dpl/SemaphoreP.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerMSP432.h>

/* driverlib header files */
#include <ti/devices/msp432p4xx/driverlib/rom.h>
#include <ti/devices/msp432p4xx/driverlib/rom_map.h>
#include <ti/devices/msp432p4xx/driverlib/adc14.h>
#include <ti/devices/msp432p4xx/driverlib/gpio.h>
#include <ti/devices/msp432p4xx/driverlib/interrupt.h>
#include <ti/devices/msp432p4xx/driverlib/ref_a.h>

#define ALL_INTERRUPTS  (0xFFFFFFFFFFFFFFFF)

#define PinConfigChannel(config) (((config) >> 10) & 0x1F)
#define PinConfigModuleFunction(config) (((config) >> 8) & 0x3)
#define PinConfigPort(config) (((config) >> 4) & 0xF)
#define PinConfigPin(config) (1 << ((config) & 0x7))

void ADCMSP432_close(ADC_Handle handle);
int_fast16_t ADCMSP432_control(ADC_Handle handle, uint_fast16_t cmd, void *arg);
int_fast16_t ADCMSP432_convert(ADC_Handle handle, uint16_t *value);
uint32_t ADCMSP432_convertToMicroVolts(ADC_Handle handle,
    uint16_t adcValue);
void ADCMSP432_init(ADC_Handle handle);
ADC_Handle ADCMSP432_open(ADC_Handle handle, ADC_Params *params);

/* Keep track of the amount of adc handle instances */
static uint_fast16_t adcInstance = 0;

/* Global mutex ensuring exclusive access to ADC during conversions */
static SemaphoreP_Handle globalMutex = NULL;

/* ADC function table for ADCMSP432 implementation */
const ADC_FxnTable ADCMSP432_fxnTable = {
    ADCMSP432_close,
    ADCMSP432_control,
    ADCMSP432_convert,
    ADCMSP432_convertToMicroVolts,
    ADCMSP432_init,
    ADCMSP432_open
};

/*
 *  ======== initHW ========
 *
 *  Configures ADC peripheral
 */
static void initHw(ADCMSP432_Object *object,
        ADCMSP432_HWAttrsV1 const *hwAttrs)
{
    /* Initializing ADC (MODCLK/1/1) */
    MAP_ADC14_enableModule();
    MAP_ADC14_initModule(ADC_CLOCKSOURCE_ADCOSC, ADC_PREDIVIDER_1,
        ADC_DIVIDER_1, 0);

    /* Set trigger source */
    MAP_ADC14_setSampleHoldTrigger(ADC_TRIGGER_ADCSC, false);

    /* Set sample/hold time */
    MAP_ADC14_setSampleHoldTime(ADC_PULSE_WIDTH_64, ADC_PULSE_WIDTH_64);

    /* No repeat mode */
    MAP_ADC14_configureSingleSampleMode(ADC_MEM0, false);

    /* Configuring Sample Timer */
    MAP_ADC14_enableSampleTimer(ADC_MANUAL_ITERATION);
}

/*
 *  ======== ADCMSP432_close ========
 */
void ADCMSP432_close(ADC_Handle handle)
{
    uintptr_t         key;
    ADCMSP432_Object *object = handle->object;

    key = HwiP_disable();

    adcInstance--;
    if (adcInstance == 0) {
        /* Disable interrupts & the ADC */
        MAP_ADC14_disableInterrupt(ALL_INTERRUPTS);
        MAP_ADC14_disableModule();

    }
    object->isOpen = false;

    HwiP_restore(key);

    DebugP_log0("ADC: Object closed");
}

/*
 *  ======== ADCMSP432_control ========
 */
int_fast16_t ADCMSP432_control(ADC_Handle handle, uint_fast16_t cmd, void *arg)
{
    /* No implementation yet */
    return (ADC_STATUS_UNDEFINEDCMD);
}

/*
 *  ======== ADCMSP432_convert ========
 */
int_fast16_t ADCMSP432_convert(ADC_Handle handle, uint16_t *value)
{
    ADCMSP432_Object          *object = handle->object;
    ADCMSP432_HWAttrsV1 const *hwAttrs = handle->hwAttrs;

    if (object->isProtected) {
        /* Acquire the lock for this particular ADC handle */
        SemaphoreP_pend(globalMutex, SemaphoreP_WAIT_FOREVER);
    }

    /*
     * Set power constraints to keep peripheral active during convert
     * and to prevent performance level changes
     */
    Power_setConstraint(PowerMSP432_DISALLOW_DEEPSLEEP_0);
    Power_setConstraint(PowerMSP432_DISALLOW_PERF_CHANGES);

    /* Set and enable internal reference voltage if being used */
    if ((hwAttrs->refVoltage == ADCMSP432_REF_VOLTAGE_INT_1_2V) ||
        (hwAttrs->refVoltage == ADCMSP432_REF_VOLTAGE_INT_1_45V) ||
        (hwAttrs->refVoltage == ADCMSP432_REF_VOLTAGE_INT_2_5V))
    {
        MAP_REF_A_setReferenceVoltage(hwAttrs->refVoltage);
        MAP_REF_A_enableReferenceVoltage();
    }

    /* Interrupt is kept disabled since using polling mode */
    MAP_ADC14_clearInterruptFlag(ADC_INT0);

    /* Set the ADC resolution */
    MAP_ADC14_setResolution(hwAttrs->resolution);

    /* Config the ADC memory0 with specified channel */
    switch(hwAttrs->refVoltage) {
        case ADCMSP432_REF_VOLTAGE_VDD:
            MAP_ADC14_configureConversionMemory(ADC_MEM0,
                    ADC_VREFPOS_AVCC_VREFNEG_VSS,
                    PinConfigChannel(hwAttrs->adcPin), false);
            break;
        case ADCMSP432_REF_VOLTAGE_INT_1_2V:
        case ADCMSP432_REF_VOLTAGE_INT_1_45V:
        case ADCMSP432_REF_VOLTAGE_INT_2_5V:
            MAP_ADC14_configureConversionMemory(ADC_MEM0,
                    ADC_VREFPOS_INTBUF_VREFNEG_VSS,
                    PinConfigChannel(hwAttrs->adcPin), false);
            break;
        case ADCMSP432_REF_VOLTAGE_EXT:
            MAP_ADC14_configureConversionMemory(ADC_MEM0,
                    ADC_VREFPOS_EXTPOS_VREFNEG_EXTNEG,
                    PinConfigChannel(hwAttrs->adcPin), false);
            break;
        case ADCMSP432_REF_VOLTAGE_EXT_BUF:
            MAP_ADC14_configureConversionMemory(ADC_MEM0,
                    ADC_VREFPOS_EXTBUF_VREFNEG_EXTNEG,
                    PinConfigChannel(hwAttrs->adcPin), false);
            break;
    }

    /* Enabling/Toggling Conversion */
    MAP_ADC14_enableConversion();

    /* Trigger conversion either from ADC14SC bit or Timer PWM */
    MAP_ADC14_toggleConversionTrigger();

    /* Interrupt polling */
    while (!(MAP_ADC14_getInterruptStatus() & ADC_INT0));
    MAP_ADC14_clearInterruptFlag(ADC_INT0);

    /* Disabling Conversion */
    MAP_ADC14_disableConversion();

    /* Store the result into object */
    *value = MAP_ADC14_getResult(ADC_MEM0);

    /* Remove constraints set after ADC conversion complete */
    Power_releaseConstraint(PowerMSP432_DISALLOW_DEEPSLEEP_0);
    Power_releaseConstraint(PowerMSP432_DISALLOW_PERF_CHANGES);

    if (object->isProtected) {
        /* Release the lock for this particular ADC handle */
        SemaphoreP_post(globalMutex);
    }

    DebugP_log0("ADC: Convert completed");

    /* Return the number of bytes transfered by the ADC */
    return (ADC_STATUS_SUCCESS);
}

/*
 *  ======== ADCMSP432_convertToMicroVolts ========
 */
uint32_t ADCMSP432_convertToMicroVolts(ADC_Handle handle,
    uint16_t adcValue)
{
    uint32_t                   refMicroVolts;
    ADCMSP432_HWAttrsV1 const *hwAttrs = handle->hwAttrs;
    uint32_t retVal = 0;

    switch (hwAttrs->refVoltage) {
        case REF_A_VREF1_45V:
            refMicroVolts = 1450000;
            break;

        case REF_A_VREF1_2V:
            refMicroVolts = 1200000;
            break;

        case REF_A_VREF2_5V:
        default:
            refMicroVolts = 2500000;
    }

    if (adcValue == 0x3FFF) {
        retVal = refMicroVolts;
    }
    else if (adcValue == 0) {
        retVal = adcValue;
    }
    else {

        /* To keep decimal resolution during division */
        refMicroVolts = refMicroVolts * 10;

        switch (hwAttrs->resolution) {
            case ADC14_CTL1_RES_0:
                retVal = (adcValue * (refMicroVolts / 0x100));
                break;
            case ADC14_CTL1_RES_1:
                retVal = (adcValue * (refMicroVolts / 0x400));
                break;
            case ADC14_CTL1_RES_2:
                retVal = (adcValue * (refMicroVolts / 0x1000));
                break;
            default:
                retVal = (adcValue * (refMicroVolts / 0x4000));
                break;
        }

        retVal = retVal / 10;
    }

    return retVal;
}

/*
 *  ======== ADCMSP432_init ========
 */
void ADCMSP432_init(ADC_Handle handle)
{
    uintptr_t         key;
    SemaphoreP_Handle sem;

    /* Create a binary semaphore for thread safety */
    sem = SemaphoreP_createBinary(1);
    /* sem == NULL will be detected in 'open' */

    key = HwiP_disable();

    /* Create Semaphore for ADC0 */
    if (globalMutex == NULL) {
        /* use the binary sem created above */
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
}

/*
 *  ======== ADCMSP432_open ========
 */
ADC_Handle ADCMSP432_open(ADC_Handle handle, ADC_Params *params)
{
    uintptr_t                  key;
    ADCMSP432_Object          *object = handle->object;
    ADCMSP432_HWAttrsV1 const *hwAttrs = handle->hwAttrs;

    if (globalMutex == NULL){
        ADCMSP432_init(handle);
        if (globalMutex == NULL) {
            DebugP_log0("ADC: mutex Semaphore_create() failed:.");
            ADCMSP432_close(handle);
            return (NULL);
        }
    }

    /* Determine if the driver was already opened */
    key = HwiP_disable();

    if (object->isOpen) {
        HwiP_restore(key);

        DebugP_log0("ADC: Error! Already in use.");
        return (NULL);
    }

    if (adcInstance == 0) {
        /* Initialize peripheral */
        initHw(object, hwAttrs);
    }
    adcInstance++;
    object->isOpen = true;

    /* remember thread safety protection setting */
    object->isProtected = params->isProtected;

    HwiP_restore(key);

    /* Config GPIO for ADC channel analog input */
     MAP_GPIO_setAsPeripheralModuleFunctionInputPin(
         PinConfigPort(hwAttrs->adcPin),
         PinConfigPin(hwAttrs->adcPin),
         PinConfigModuleFunction(hwAttrs->adcPin));

    DebugP_log0("ADC: Object opened");

    return (handle);
}
