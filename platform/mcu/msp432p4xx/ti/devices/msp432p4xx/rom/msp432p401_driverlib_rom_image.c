/* --COPYRIGHT--,BSD
 * Copyright (c) 2014, Texas Instruments Incorporated
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
 * --/COPYRIGHT--*/
//##### AUTO GENERATED FILE...DO NOT EDIT #####
#undef DEBUG
#define BUILD_FOR_ROM           1

#include <ti/devices/msp432p4xx/inc/msp.h>
#include <src/adc14.h>
#include <src/aes256.h>
#include <src/comp_e.h>
#include <src/cpu.h>
#include <src/crc32.h>
#include <src/cs.h>
#include <src/debug.h>
#include <src/dma.h>
#include <src/eusci.h>
#include <src/flash.h>
#include <src/fpu.h>
#include <src/gpio.h>
#include <src/i2c.h>
#include <src/interrupt.h>
#include <src/mpu.h>
#include <src/pcm.h>
#include <src/pmap.h>
#include <src/pss.h>
#include <src/ref_a.h>
#include <src/reset.h>
#include <src/rtc_c.h>
#include <src/spi.h>
#include <src/sysctl.h>
#include <src/systick.h>
#include <src/timer_a.h>
#include <src/timer32.h>
#include <src/uart.h>
#include <src/wdt_a.h>



static const uint32_t MAX_PROGRAM_NO_TLV = 5;
static const uint32_t MAX_ERASE_NO_TLV = 50;
//*****************************************************************************
//
//!
//! Returns a boolean value that tells if conversion is active/running or is
//! not acMSP432 ted.
//!
//! Originally a public function, but moved to static. External customers should
//! use the ADC14_isBusy function.
//!
//! \return true if conversion is active, false otherwise
//
//*****************************************************************************
static bool ADCIsConversionRunning(void)
{
    return BITBAND_PERI(ADC14->rCTL0.r, ADC14BUSY_OFS);
}


void ADC14_enableModule(void)
{
    BITBAND_PERI(ADC14->rCTL0.r, ADC14ON_OFS) = 1;
}


bool ADC14_disableModule(void)
{
    if (ADCIsConversionRunning())
        return false;

    BITBAND_PERI(ADC14->rCTL0.r, ADC14ON_OFS) = 0;

    return true;
}


static volatile uint32_t* const _ctlRegs[32] =
{ &ADC14->rMCTL0.r, &ADC14->rMCTL1.r, &ADC14->rMCTL2.r, &ADC14->rMCTL3.r,
        &ADC14->rMCTL4.r, &ADC14->rMCTL5.r, &ADC14->rMCTL6.r, &ADC14->rMCTL7.r,
        &ADC14->rMCTL8.r, &ADC14->rMCTL9.r, &ADC14->rMCTL10.r,
        &ADC14->rMCTL11.r, &ADC14->rMCTL12.r, &ADC14->rMCTL13.r,
        &ADC14->rMCTL14.r, &ADC14->rMCTL15.r, &ADC14->rMCTL16.r,
        &ADC14->rMCTL17.r, &ADC14->rMCTL18.r, &ADC14->rMCTL19.r,
        &ADC14->rMCTL20.r, &ADC14->rMCTL21.r, &ADC14->rMCTL22.r,
        &ADC14->rMCTL23.r, &ADC14->rMCTL24.r, &ADC14->rMCTL25.r,
        &ADC14->rMCTL26.r, &ADC14->rMCTL27.r, &ADC14->rMCTL28.r,
        &ADC14->rMCTL29.r, &ADC14->rMCTL30.r, &ADC14->rMCTL31.r };


static uint_fast8_t _getIndexForMemRegister(uint32_t reg)
{
    switch (reg)
    {
    case ADC_MEM0:
        return 0;
    case ADC_MEM1:
        return 1;
    case ADC_MEM2:
        return 2;
    case ADC_MEM3:
        return 3;
    case ADC_MEM4:
        return 4;
    case ADC_MEM5:
        return 5;
    case ADC_MEM6:
        return 6;
    case ADC_MEM7:
        return 7;
    case ADC_MEM8:
        return 8;
    case ADC_MEM9:
        return 9;
    case ADC_MEM10:
        return 10;
    case ADC_MEM11:
        return 11;
    case ADC_MEM12:
        return 12;
    case ADC_MEM13:
        return 13;
    case ADC_MEM14:
        return 14;
    case ADC_MEM15:
        return 15;
    case ADC_MEM16:
        return 16;
    case ADC_MEM17:
        return 17;
    case ADC_MEM18:
        return 18;
    case ADC_MEM19:
        return 19;
    case ADC_MEM20:
        return 20;
    case ADC_MEM21:
        return 21;
    case ADC_MEM22:
        return 22;
    case ADC_MEM23:
        return 23;
    case ADC_MEM24:
        return 24;
    case ADC_MEM25:
        return 25;
    case ADC_MEM26:
        return 26;
    case ADC_MEM27:
        return 27;
    case ADC_MEM28:
        return 28;
    case ADC_MEM29:
        return 29;
    case ADC_MEM30:
        return 30;
    case ADC_MEM31:
        return 31;
    default:
        ASSERT(false);
        return ADC_INVALID_MEM;

    }
}


bool ADC14_initModule(uint32_t clockSource, uint32_t clockPredivider,
        uint32_t clockDivider, uint32_t internalChannelMask)
{
    ASSERT(
            clockSource == ADC_CLOCKSOURCE_ADCOSC
            || clockSource == ADC_CLOCKSOURCE_SYSOSC
            || clockSource == ADC_CLOCKSOURCE_ACLK
            || clockSource == ADC_CLOCKSOURCE_MCLK
            || clockSource == ADC_CLOCKSOURCE_SMCLK
            || clockSource == ADC_CLOCKSOURCE_HSMCLK);

    ASSERT(
            clockPredivider == ADC_PREDIVIDER_1
            || clockPredivider == ADC_PREDIVIDER_4
            || clockPredivider == ADC_PREDIVIDER_32
            || clockPredivider == ADC_PREDIVIDER_64);

    ASSERT(
            clockDivider == ADC_DIVIDER_1 || clockDivider == ADC_DIVIDER_2
            || clockDivider == ADC_DIVIDER_3
            || clockDivider == ADC_DIVIDER_4
            || clockDivider == ADC_DIVIDER_5
            || clockDivider == ADC_DIVIDER_6
            || clockDivider == ADC_DIVIDER_7
            || clockDivider == ADC_DIVIDER_8);

    ASSERT(
            !(internalChannelMask
                    & ~(ADC_MAPINTCH3 | ADC_MAPINTCH2 | ADC_MAPINTCH1
                            | ADC_MAPINTCH0 | ADC_TEMPSENSEMAP | ADC_BATTMAP)));

    if (ADCIsConversionRunning())
        return false;

    ADC14->rCTL0.r = (ADC14->rCTL0.r
            & ~(ADC14PDIV_M | ADC14DIV_M | ADC14SSEL_M))
            | clockDivider | clockPredivider | clockSource;

    ADC14->rCTL1.r = (ADC14->rCTL1.r
            & ~(ADC_MAPINTCH3 | ADC_MAPINTCH2 | ADC_MAPINTCH1 | ADC_MAPINTCH0
                    | ADC_TEMPSENSEMAP | ADC_BATTMAP)) | internalChannelMask;

    return true;
}


void ADC14_setResolution(uint32_t resolution)
{
    ASSERT(
            resolution == ADC_8BIT || resolution == ADC_10BIT
            || resolution == ADC_12BIT || resolution == ADC_14BIT);

    ADC14->rCTL1.r = (ADC14->rCTL1.r & ~ADC14RES_M) |  resolution;
}


uint_fast32_t ADC14_getResolution(void)
{
    return ADC14->rCTL1.r & ADC14RES_M;
}


bool ADC14_setSampleHoldTrigger(uint32_t source, bool invertSignal)
{

    ASSERT(
            source == ADC_TRIGGER_ADCSC || source == ADC_TRIGGER_SOURCE1
            || source == ADC_TRIGGER_SOURCE2
            || source == ADC_TRIGGER_SOURCE3
            || source == ADC_TRIGGER_SOURCE4
            || source == ADC_TRIGGER_SOURCE5
            || source == ADC_TRIGGER_SOURCE6
            || source == ADC_TRIGGER_SOURCE7);

    if (ADCIsConversionRunning())
        return false;

    if (invertSignal)
    {
        ADC14->rCTL0.r = (ADC14->rCTL0.r
                & ~(ADC14ISSH | ADC14SHS_M)) | source
                | ADC14ISSH;
    } else
    {
        ADC14->rCTL0.r = (ADC14->rCTL0.r
                & ~(ADC14ISSH | ADC14SHS_M)) | source;
    }

    return true;
}


bool ADC14_setSampleHoldTime(uint32_t firstPulseWidth,
        uint32_t secondPulseWidth)
{
    ASSERT(
            firstPulseWidth == ADC_PULSE_WIDTH_4
            || firstPulseWidth == ADC_PULSE_WIDTH_8
            || firstPulseWidth == ADC_PULSE_WIDTH_16
            || firstPulseWidth == ADC_PULSE_WIDTH_32
            || firstPulseWidth == ADC_PULSE_WIDTH_64
            || firstPulseWidth == ADC_PULSE_WIDTH_96
            || firstPulseWidth == ADC_PULSE_WIDTH_128
            || firstPulseWidth == ADC_PULSE_WIDTH_192);

    ASSERT(
            secondPulseWidth == ADC_PULSE_WIDTH_4
            || secondPulseWidth == ADC_PULSE_WIDTH_8
            || secondPulseWidth == ADC_PULSE_WIDTH_16
            || secondPulseWidth == ADC_PULSE_WIDTH_32
            || secondPulseWidth == ADC_PULSE_WIDTH_64
            || secondPulseWidth == ADC_PULSE_WIDTH_96
            || secondPulseWidth == ADC_PULSE_WIDTH_128
            || secondPulseWidth == ADC_PULSE_WIDTH_192);

    if (ADCIsConversionRunning())
        return false;

    ADC14->rCTL0.r = (ADC14->rCTL0.r
            & ~(ADC14SHT0_M | ADC14SHT1_M)) | secondPulseWidth
            | (firstPulseWidth >> 4);

    return true;
}


bool ADC14_configureMultiSequenceMode(uint32_t memoryStart, uint32_t memoryEnd,
bool repeatMode)
{
    uint32_t ii;

    ASSERT(
            _getIndexForMemRegister(memoryStart) != ADC_INVALID_MEM
            && _getIndexForMemRegister(memoryEnd) != ADC_INVALID_MEM);

    if (ADCIsConversionRunning())
        return false;

    /* Clearing out any lingering EOS */
    for (ii = 0; ii < 32; ii++)
    {
        BITBAND_PERI(*(_ctlRegs[ii]), ADC14EOS_OFS) = 0;
    }

    /* Setting Start/Stop locations */
    BITBAND_PERI(
            (*(_ctlRegs[_getIndexForMemRegister(memoryEnd)])),
            ADC14EOS_OFS) = 1;

    ADC14->rCTL1.r = (ADC14->rCTL1.r & ~(ADC14CSTARTADD_M))
                    | (_getIndexForMemRegister(memoryStart) << 16);

    /* Setting multiple sample mode */
    if (!repeatMode)
    {
        ADC14->rCTL0.r = (ADC14->rCTL0.r & ~(ADC14CONSEQ_M))
                | (ADC14CONSEQ_1);
    } else
    {
        ADC14->rCTL0.r = (ADC14->rCTL0.r & ~(ADC14CONSEQ_M))
                | (ADC14CONSEQ_3);
    }

    return true;
}


bool ADC14_configureSingleSampleMode(uint32_t memoryDestination,
bool repeatMode)
{
    ASSERT(_getIndexForMemRegister(memoryDestination) != 32);

    if (ADCIsConversionRunning())
        return false;

    /* Setting the destination register */
    ADC14->rCTL1.r = (ADC14->rCTL1.r & ~(ADC14CSTARTADD_M))
            | (_getIndexForMemRegister(memoryDestination) << 16);

    /* Setting single sample mode */
    if (!repeatMode)
    {
        ADC14->rCTL0.r = (ADC14->rCTL0.r & ~(ADC14CONSEQ_M))
                | (ADC14CONSEQ_0);
    } else
    {
        ADC14->rCTL0.r = (ADC14->rCTL0.r & ~(ADC14CONSEQ_M))
                | (ADC14CONSEQ_2);
    }

    return true;
}


bool ADC14_enableConversion(void)
{
    if (ADCIsConversionRunning() || !BITBAND_PERI(ADC14->rCTL0.r, ADC14ON_OFS))
        return false;

    ADC14->rCTL0.r |= (ADC14ENC);

    return true;
}


void ADC14_disableConversion(void)
{
    ADC14->rCTL0.r &= ~(ADC14SC | ADC14ENC);
}


bool ADC14_isBusy(void)
{
    return BITBAND_PERI(ADC14->rCTL0.r, ADC14BUSY_OFS);
}


bool ADC14_configureConversionMemory(uint32_t memorySelect, uint32_t refSelect,
        uint32_t channelSelect, bool differntialMode)
{
    uint32_t currentReg, ii;
    uint32_t *curReg;

    /* Initialization */
    ii = 1;
    currentReg = 0x01;

    if (ADCIsConversionRunning())
        return false;

    while (memorySelect != 0)
    {
        if (!(memorySelect & ii))
        {
            ii = ii << 1;
            continue;
        }

        currentReg = memorySelect & ii;
        memorySelect &= ~ii;
        ii = ii << 1;

        curReg = (uint32_t*) _ctlRegs[_getIndexForMemRegister(currentReg)];

        if (differntialMode)
        {
            (*curReg) = ((*curReg)
                    & ~(ADC14VRSEL_M | ADC14INCH_M
                            | ADC14DIF))
                    | (channelSelect | refSelect | ADC14DIF);
        } else
        {
            (*curReg) = ((*curReg)
                    & ~(ADC14VRSEL_M | ADC14INCH_M
                            | ADC14DIF)) | (channelSelect | refSelect);
        }

    }

    return true;
}


bool ADC14_enableComparatorWindow(uint32_t memorySelect, uint32_t windowSelect)
{
    uint32_t currentReg, ii;
    uint32_t *curRegPoint;

    /* Initialization */
    ii = 1;
    currentReg = 0x01;

    if (ADCIsConversionRunning())
        return false;

    while (memorySelect != 0)
    {
        if (!(memorySelect & ii))
        {
            ii = ii << 1;
            continue;
        }

        currentReg = memorySelect & ii;
        memorySelect &= ~ii;
        ii = ii << 1;

        curRegPoint =
                (uint32_t*) _ctlRegs[_getIndexForMemRegister(currentReg)];

        if (windowSelect == ADC_COMP_WINDOW0)
        {
            (*curRegPoint) = ((*curRegPoint)
                    & ~(ADC14WINC | ADC14WINCTH))
                    | (ADC14WINC);
        } else if (windowSelect == ADC_COMP_WINDOW1)
        {
            (*curRegPoint) |= ADC14WINC | ADC14WINCTH;
        }

    }

    return true;
}


bool ADC14_disableComparatorWindow(uint32_t memorySelect)
{
    uint32_t currentReg, ii;

    /* Initialization */
    ii = 1;
    currentReg = 0x01;

    if (ADCIsConversionRunning())
        return false;

    while (memorySelect != 0)
    {
        if (!(memorySelect & ii))
        {
            ii = ii << 1;
            continue;
        }

        currentReg = memorySelect & ii;
        memorySelect &= ~ii;
        ii = ii << 1;

        (*(_ctlRegs[_getIndexForMemRegister(currentReg)])) &=
                ~ADC14WINC;

    }

    return true;
}


bool ADC14_setComparatorWindowValue(uint32_t window, int16_t low, int16_t high)
{
    if (ADCIsConversionRunning())
        return false;
        
    if(BITBAND_PERI(ADC14->rCTL1.r, ADC14DF_OFS))
    {
        low = ((low << 2) | (0x8000 & low)) & 0xFFFC;
        high = ((high << 2) | (0x8000 & high)) & 0xFFFC;
    }

    if (window == ADC_COMP_WINDOW0)
    {
        ADC14->rHI0.r = (high);
        ADC14->rLO0.r = (low);

    } else if (window == ADC_COMP_WINDOW1)
    {
        ADC14->rHI1.r = (high);
        ADC14->rLO1.r = (low);

    } else
    {
        ASSERT(false);
        return false;
    }

    return true;
}


bool ADC14_setResultFormat(uint32_t resultFormat)
{
    if (ADCIsConversionRunning())
        return false;

    if (resultFormat == ADC_UNSIGNED_BINARY)
    {
        BITBAND_PERI(ADC14->rCTL1.r, ADC14DF_OFS) = 0;
    } else if (resultFormat == ADC_SIGNED_BINARY)
    {
        BITBAND_PERI(ADC14->rCTL1.r, ADC14DF_OFS) = 1;
    } else
    {
        ASSERT(false);
    }

    return true;
}


uint_fast16_t ADC14_getResult(uint32_t memorySelect)
{
    return *((uint16_t*) (_ctlRegs[_getIndexForMemRegister(memorySelect)]
            + 0x20));
}


void ADC14_getMultiSequenceResult(uint16_t* res)
{
    uint32_t *startAddr, *curAddr;
    uint32_t ii;

    startAddr = (uint32_t*) _ctlRegs[(ADC14->rCTL1.r & ADC14CSTARTADD_M)
            >> 16];

    curAddr = startAddr;

    for (ii = 0; ii < 32; ii++)
    {
        res[ii] = *(((uint16_t*) curAddr) + 0x40);

        if (BITBAND_PERI((*curAddr), ADC14EOS_OFS))
            break;

        if (curAddr == _ctlRegs[31])
            curAddr = (uint32_t*) _ctlRegs[0];
        else
            curAddr += 0x04;
    }

}


void ADC14_getResultArray(uint32_t memoryStart, uint32_t memoryEnd,
        uint16_t* res)
{
    uint32_t ii = 0;
    uint32_t *firstPoint, *secondPoint;

    bool foundEnd = false;

    ASSERT(
            _getIndexForMemRegister(memoryStart) != ADC_INVALID_MEM
            && _getIndexForMemRegister(memoryEnd) != ADC_INVALID_MEM);

    firstPoint = (uint32_t*) _ctlRegs[_getIndexForMemRegister(memoryStart)];
    secondPoint = (uint32_t*) _ctlRegs[_getIndexForMemRegister(memoryEnd)];

    while (!foundEnd)
    {
        if (firstPoint == secondPoint)
        {
            foundEnd = true;
        }

        res[ii] = *(((uint16_t*) firstPoint) + 0x40);

        if (firstPoint == _ctlRegs[31])
            firstPoint = (uint32_t*) _ctlRegs[0];
        else
            firstPoint += 0x04;
    }
}


bool ADC14_enableReferenceBurst(void)
{
    if (ADCIsConversionRunning())
        return false;

    BITBAND_PERI(ADC14->rCTL1.r, ADC14REFBURST_OFS) = 1;

    return true;
}


bool ADC14_disableReferenceBurst(void)
{
    if (ADCIsConversionRunning())
        return false;

    BITBAND_PERI(ADC14->rCTL1.r, ADC14REFBURST_OFS) = 0;

    return true;
}


bool ADC14_setPowerMode(uint32_t adcPowerMode)
{
    if (ADCIsConversionRunning())
        return false;

    switch (adcPowerMode)
    {
    case ADC_UNRESTRICTED_POWER_MODE:
        ADC14->rCTL1.r = (ADC14->rCTL1.r & ~(ADC14PWRMD_M))
                | (ADC14PWRMD_0);
        break;
    case ADC_ULTRA_LOW_POWER_MODE:
        ADC14->rCTL1.r = (ADC14->rCTL1.r & ~(ADC14PWRMD_M))
                | (ADC14PWRMD_2);
        break;
    default:
        ASSERT(false);
        return false;
    }

    return true;
}


void ADC14_enableInterrupt(uint_fast64_t mask)
{
    uint32_t stat = mask & 0xFFFFFFFF;

    ADC14->rIER0.r |= stat;
    stat = (mask >> 32);
    ADC14->rIER1.r |= (stat);
}


void ADC14_disableInterrupt(uint_fast64_t mask)
{
    uint32_t stat = mask & 0xFFFFFFFF;

    ADC14->rIER0.r &= ~stat;
    stat = (mask >> 32);
    ADC14->rIER1.r &= ~(stat);
}


uint_fast64_t ADC14_getInterruptStatus(void)
{
    uint_fast64_t status = ADC14->rIFGR1.r;
    return ((status << 32) | ADC14->rIFGR0.r);
}


uint_fast64_t ADC14_getEnabledInterruptStatus(void)
{
    uint_fast64_t stat = ADC14->rIER1.r;

    return ADC14_getInterruptStatus() & ((stat << 32) | ADC14->rIER0.r);

}


bool ADC14_toggleConversionTrigger(void)
{
    if (!BITBAND_PERI(ADC14->rCTL0.r, ADC14ON_OFS))
        return false;

    if (BITBAND_PERI(ADC14->rCTL0.r, ADC14SC_OFS))
    {
        BITBAND_PERI(ADC14->rCTL0.r, ADC14SC_OFS) = 0;
    } else
    {
        BITBAND_PERI(ADC14->rCTL0.r, ADC14SC_OFS) = 1;
    }

    return true;
}


bool ADC14_enableSampleTimer(uint32_t multiSampleConvert)
{
    if (ADCIsConversionRunning())
        return false;

    BITBAND_PERI(ADC14->rCTL0.r, ADC14SHP_OFS) = 1;

    if (multiSampleConvert == ADC_MANUAL_ITERATION)
    {
        BITBAND_PERI(ADC14->rCTL0.r, ADC14MSC_OFS) = 0;
    } else
    {
        BITBAND_PERI(ADC14->rCTL0.r, ADC14MSC_OFS) = 1;
    }

    return true;
}


bool ADC14_disableSampleTimer(void)
{
    if (ADCIsConversionRunning())
        return false;

    BITBAND_PERI(ADC14->rCTL0.r, ADC14SHP_OFS) = 0;

    return true;
}


void ADC14_clearInterruptFlag(uint_fast64_t mask)
{
    uint32_t stat = mask & 0xFFFFFFFF;

    ADC14->rCLRIFGR0.r |= stat;
    stat = (mask >> 32);
    ADC14->rCLRIFGR1.r |= (stat);
}


uint32_t AES256_getInterruptStatus(uint32_t moduleInstance)
{
    return AES256_getInterruptFlagStatus(moduleInstance);
}


bool AES256_setCipherKey(uint32_t moduleInstance, const uint8_t * cipherKey,
        uint_fast16_t keyLength)
{
    uint_fast8_t i;
    uint16_t sCipherKey;

    AES256_CMSIS(moduleInstance)->rCTL0.r |= 0;

    switch (keyLength)
    {
    case AES256_KEYLENGTH_128BIT:
        AES256_CMSIS(moduleInstance)->rCTL0.r |= AESKL__128BIT;
        break;

    case AES256_KEYLENGTH_192BIT:
        AES256_CMSIS(moduleInstance)->rCTL0.r |= AESKL__192BIT;
        break;

    case AES256_KEYLENGTH_256BIT:
        AES256_CMSIS(moduleInstance)->rCTL0.r |= AESKL__256BIT;
        break;
    default:
        return false;
    }

    keyLength = keyLength / 8;

    for (i = 0; i < keyLength; i = i + 2)
    {
        sCipherKey = (uint16_t) (cipherKey[i]);
        sCipherKey = sCipherKey | ((uint16_t) (cipherKey[i + 1]) << 8);
        AES256_CMSIS(moduleInstance)->rKEY.r = sCipherKey;
    }

    // Wait until key is written
    while (!BITBAND_PERI(AES256_CMSIS(moduleInstance)->rSTAT.r, AESKEYWR_OFS))
        ;

    return true;
}


void AES256_encryptData(uint32_t moduleInstance, const uint8_t * data,
        uint8_t * encryptedData)
{
    uint_fast8_t i;
    uint16_t tempData = 0;
    uint16_t tempVariable = 0;

    // Set module to encrypt mode
    AES256_CMSIS(moduleInstance)->rCTL0.r &= ~AESOP_M;

    // Write data to encrypt to module
    for (i = 0; i < 16; i = i + 2)
    {
        tempVariable = (uint16_t) (data[i]);
        tempVariable = tempVariable | ((uint16_t) (data[i + 1]) << 8);
        AES256_CMSIS(moduleInstance)->rDIN.r = tempVariable;
    }

    // Key that is already written shall be used
    // Encryption is initialized by setting AESKEYWR to 1
    BITBAND_PERI(AES256_CMSIS(moduleInstance)->rSTAT.r, AESKEYWR_OFS) = 1;

    // Wait unit finished ~167 MCLK
    while (BITBAND_PERI(AES256_CMSIS(moduleInstance)->rSTAT.r, AESBUSY_OFS))
        ;

    // Write encrypted data back to variable
    for (i = 0; i < 16; i = i + 2)
    {
        tempData = AES256_CMSIS(moduleInstance)->rDOUT.r;
        *(encryptedData + i) = (uint8_t) tempData;
        *(encryptedData + i + 1) = (uint8_t) (tempData >> 8);
    }
}


void AES256_decryptData(uint32_t moduleInstance, const uint8_t * data,
        uint8_t * decryptedData)
{
    uint_fast8_t i;
    uint16_t tempData = 0;
    uint16_t tempVariable = 0;

    // Set module to decrypt mode
    AES256_CMSIS(moduleInstance)->rCTL0.r |= (AESOP_3);

    // Write data to decrypt to module
    for (i = 0; i < 16; i = i + 2)
    {
        tempVariable = (uint16_t) (data[i + 1] << 8);
        tempVariable = tempVariable | ((uint16_t) (data[i]));
        AES256_CMSIS(moduleInstance)->rDIN.r = tempVariable;
    }

    // Key that is already written shall be used
    // Now decryption starts
    BITBAND_PERI(AES256_CMSIS(moduleInstance)->rSTAT.r, AESKEYWR_OFS) = 1;

    // Wait unit finished ~167 MCLK
    while (BITBAND_PERI(AES256_CMSIS(moduleInstance)->rSTAT.r, AESBUSY_OFS))
        ;

    // Write encrypted data back to variable
    for (i = 0; i < 16; i = i + 2)
    {
        tempData = AES256_CMSIS(moduleInstance)->rDOUT.r;
        *(decryptedData + i) = (uint8_t) tempData;
        *(decryptedData + i + 1) = (uint8_t) (tempData >> 8);
    }
}


void AES256_clearInterruptFlag(uint32_t moduleInstance)
{
    BITBAND_PERI(AES256_CMSIS(moduleInstance)->rCTL0.r,AESRDYIFG_OFS) = 0;
}


uint32_t AES256_getInterruptFlagStatus(uint32_t moduleInstance)
{
    return BITBAND_PERI(AES256_CMSIS(moduleInstance)->rCTL0.r, AESRDYIFG_OFS);
}


void AES256_enableInterrupt(uint32_t moduleInstance)
{
    BITBAND_PERI(AES256_CMSIS(moduleInstance)->rCTL0.r,AESRDYIE_OFS) = 1;
}


void AES256_disableInterrupt(uint32_t moduleInstance)
{
    BITBAND_PERI(AES256_CMSIS(moduleInstance)->rCTL0.r,AESRDYIE_OFS) = 0;
}


void AES256_reset(uint32_t moduleInstance)
{
    BITBAND_PERI(AES256_CMSIS(moduleInstance)->rCTL0.r,AESSWRST_OFS) = 1;
}


void AES256_startEncryptData(uint32_t moduleInstance, const uint8_t * data)
{
    uint8_t i;
    uint16_t tempVariable = 0;

    // Set module to encrypt mode
    AES256_CMSIS(moduleInstance)->rCTL0.r &= ~AESOP_M;

    // Write data to encrypt to module
    for (i = 0; i < 16; i = i + 2)
    {
        tempVariable = (uint16_t) (data[i]);
        tempVariable = tempVariable | ((uint16_t) (data[i + 1]) << 8);
        AES256_CMSIS(moduleInstance)->rDIN.r = tempVariable;
    }

    // Key that is already written shall be used
    // Encryption is initialized by setting AESKEYWR to 1
    BITBAND_PERI(AES256_CMSIS(moduleInstance)->rSTAT.r, AESKEYWR_OFS) = 1;
}


void AES256_startDecryptData(uint32_t moduleInstance, const uint8_t * data)
{
    uint_fast8_t i;
    uint16_t tempVariable = 0;

    // Set module to decrypt mode
    AES256_CMSIS(moduleInstance)->rCTL0.r |= (AESOP_3);

    // Write data to decrypt to module
    for (i = 0; i < 16; i = i + 2)
    {
        tempVariable = (uint16_t) (data[i + 1] << 8);
        tempVariable = tempVariable | ((uint16_t) (data[i]));
        AES256_CMSIS(moduleInstance)->rDIN.r = tempVariable;
    }

    // Key that is already written shall be used
    // Now decryption starts
    BITBAND_PERI(AES256_CMSIS(moduleInstance)->rSTAT.r, AESKEYWR_OFS) = 1;
}


bool AES256_getDataOut(uint32_t moduleInstance, uint8_t *outputData)
{
    uint8_t i;
    uint16_t tempData = 0;

    // If module is busy, exit and return failure
    if (BITBAND_PERI(AES256_CMSIS(moduleInstance)->rSTAT.r, AESBUSY_OFS))
        return false;

    // Write encrypted data back to variable
    for (i = 0; i < 16; i = i + 2)
    {
        tempData = AES256_CMSIS(moduleInstance)->rDOUT.r;
        *(outputData + i) = (uint8_t) tempData;
        *(outputData + i + 1) = (uint8_t) (tempData >> 8);
    }

    return true;
}


bool AES256_isBusy(uint32_t moduleInstance)
{
    return BITBAND_PERI(AES256_CMSIS(moduleInstance)->rSTAT.r, AESBUSY_OFS);
}


void AES256_clearErrorFlag(uint32_t moduleInstance)
{
    BITBAND_PERI(AES256_CMSIS(moduleInstance)->rCTL0.r, AESERRFG_OFS) = 0;
}


bool AES256_setDecipherKey(uint32_t moduleInstance, const uint8_t * cipherKey,
        uint_fast16_t keyLength)
{
    uint8_t i;
    uint16_t tempVariable = 0;

    // Set module to decrypt mode
    AES256_CMSIS(moduleInstance)->rCTL0.r =
            (AES256_CMSIS(moduleInstance)->rCTL0.r & ~AESOP_M) | AESOP1;

    switch (keyLength)
    {
    case AES256_KEYLENGTH_128BIT:
        AES256_CMSIS(moduleInstance)->rCTL0.r |= AESKL__128BIT;
        break;

    case AES256_KEYLENGTH_192BIT:
        AES256_CMSIS(moduleInstance)->rCTL0.r |= AESKL__192BIT;
        break;

    case AES256_KEYLENGTH_256BIT:
        AES256_CMSIS(moduleInstance)->rCTL0.r |= AESKL__256BIT;
        break;

    default:
        return false;
    }

    keyLength = keyLength / 8;

    // Write cipher key to key register
    for (i = 0; i < keyLength; i = i + 2)
    {
        tempVariable = (uint16_t) (cipherKey[i]);
        tempVariable = tempVariable | ((uint16_t) (cipherKey[i + 1]) << 8);
        AES256_CMSIS(moduleInstance)->rKEY.r = tempVariable;
    }

    // Wait until key is processed ~52 MCLK
    while (BITBAND_PERI(AES256_CMSIS(moduleInstance)->rSTAT.r, AESBUSY_OFS))
        ;

    return true;
}


bool AES256_startSetDecipherKey(uint32_t moduleInstance,
        const uint8_t * cipherKey, uint_fast16_t keyLength)
{
    uint_fast8_t i;
    uint16_t tempVariable = 0;

    AES256_CMSIS(moduleInstance)->rCTL0.r =
            (AES256_CMSIS(moduleInstance)->rCTL0.r & ~AESOP_M) | AESOP1;

    switch (keyLength)
    {
    case AES256_KEYLENGTH_128BIT:
        AES256_CMSIS(moduleInstance)->rCTL0.r |= AESKL__128BIT;
        break;

    case AES256_KEYLENGTH_192BIT:
        AES256_CMSIS(moduleInstance)->rCTL0.r |= AESKL__192BIT;
        break;

    case AES256_KEYLENGTH_256BIT:
        AES256_CMSIS(moduleInstance)->rCTL0.r |= AESKL__256BIT;
        break;

    default:
        return false;
    }

    keyLength = keyLength / 8;

    // Write cipher key to key register
    for (i = 0; i < keyLength; i = i + 2)
    {
        tempVariable = (uint16_t) (cipherKey[i]);
        tempVariable = tempVariable | ((uint16_t) (cipherKey[i + 1]) << 8);
        AES256_CMSIS(moduleInstance)->rKEY.r = tempVariable;
    }

    return true;
}


uint32_t AES256_getErrorFlagStatus(uint32_t moduleInstance)
{
    return BITBAND_PERI(AES256_CMSIS(moduleInstance)->rCTL0.r, AESERRFG_OFS);
}


static uint16_t __getRegisterSettingForInput(uint32_t input)
{
    switch (input)
    {
    case COMP_E_INPUT0:
        return CEIPSEL_0;
    case COMP_E_INPUT1:
        return CEIPSEL_1;
    case COMP_E_INPUT2:
        return CEIPSEL_2;
    case COMP_E_INPUT3:
        return CEIPSEL_3;
    case COMP_E_INPUT4:
        return CEIPSEL_4;
    case COMP_E_INPUT5:
        return CEIPSEL_5;
    case COMP_E_INPUT6:
        return CEIPSEL_6;
    case COMP_E_INPUT7:
        return CEIPSEL_7;
    case COMP_E_INPUT8:
        return CEIPSEL_8;
    case COMP_E_INPUT9:
        return CEIPSEL_9;
    case COMP_E_INPUT10:
        return CEIPSEL_10;
    case COMP_E_INPUT11:
        return CEIPSEL_11;
    case COMP_E_INPUT12:
        return CEIPSEL_12;
    case COMP_E_INPUT13:
        return CEIPSEL_13;
    case COMP_E_INPUT14:
        return CEIPSEL_14;
    case COMP_E_INPUT15:
        return CEIPSEL_15;
    case COMP_E_VREF:
        return COMP_E_VREF;
    default:
        ASSERT(false);
        return 0x11;
    }

}


bool COMP_E_initModule(uint32_t comparator, const COMP_E_Config *config)
{
    uint_fast8_t positiveTerminalInput = __getRegisterSettingForInput(
            config->positiveTerminalInput);
    uint_fast8_t negativeTerminalInput = __getRegisterSettingForInput(
            config->negativeTerminalInput);
    bool retVal = true;

    ASSERT(positiveTerminalInput < 0x10); ASSERT(negativeTerminalInput < 0x10);
    ASSERT(positiveTerminalInput != negativeTerminalInput);
    ASSERT(
            config->outputFilterEnableAndDelayLevel
            <= COMP_E_FILTEROUTPUT_DLYLVL4);

    /* Reset COMPE Control 1 & Interrupt Registers for initialization */
    COMP_E_CMSIS(comparator)->rCTL0.r = 0;
    COMP_E_CMSIS(comparator)->rINT.r = 0;

    // Set the Positive Terminal
    if (COMP_E_VREF != positiveTerminalInput)
    {
        // Enable Positive Terminal Input Mux and Set to the appropriate input
        COMP_E_CMSIS(comparator)->rCTL0.r |= CEIPEN + positiveTerminalInput;

        // Disable the input buffer
        COMP_E_CMSIS(comparator)->rCTL3.r |= (1 << positiveTerminalInput);
    } else
    {
        //  Reset and Set COMPE Control 2 Register
        BITBAND_PERI(COMP_E_CMSIS(comparator)->rCTL2.r,CERSEL_OFS) = 0;
    }

    // Set the Negative Terminal
    if (COMP_E_VREF != negativeTerminalInput)
    {
        // Enable Negative Terminal Input Mux and Set  to the appropriate input
        COMP_E_CMSIS(comparator)->rCTL0.r |= CEIMEN
                + (negativeTerminalInput << 8);

        // Disable the input buffer
        COMP_E_CMSIS(comparator)->rCTL3.r |= (1 << negativeTerminalInput);
    } else
    {
        // Reset and Set COMPE Control 2 Register
        BITBAND_PERI(COMP_E_CMSIS(comparator)->rCTL2.r, CERSEL_OFS) = 1;
    }

    // Reset and Set COMPE Control 1 Register
    COMP_E_CMSIS(comparator)->rCTL1.r = config->powerMode
            + config->outputFilterEnableAndDelayLevel
            + config->invertedOutputPolarity;

    return retVal;
}


void COMP_E_setReferenceVoltage(uint32_t comparator,
        uint_fast16_t supplyVoltageReferenceBase,
        uint_fast16_t lowerLimitSupplyVoltageFractionOf32,
        uint_fast16_t upperLimitSupplyVoltageFractionOf32)
{
    ASSERT(supplyVoltageReferenceBase <= COMP_E_VREFBASE2_5V);
    ASSERT(upperLimitSupplyVoltageFractionOf32 <= 32);
    ASSERT(lowerLimitSupplyVoltageFractionOf32 <= 32); ASSERT(
            upperLimitSupplyVoltageFractionOf32
            >= lowerLimitSupplyVoltageFractionOf32);

    BITBAND_PERI(COMP_E_CMSIS(comparator)->rCTL1.r, CEMRVS_OFS) = 0;
    COMP_E_CMSIS(comparator)->rCTL2.r &= CERSEL;

    // Set Voltage Source(Vcc | Vref, resistor ladder or not)
    if (COMP_E_REFERENCE_AMPLIFIER_DISABLED == supplyVoltageReferenceBase)
    {
        COMP_E_CMSIS(comparator)->rCTL2.r |= CERS_1;
    } else if (lowerLimitSupplyVoltageFractionOf32 == 32)
    {
        COMP_E_CMSIS(comparator)->rCTL2.r |= CERS_3;
    } else
    {
        COMP_E_CMSIS(comparator)->rCTL2.r |= CERS_2;
    }

    // Set COMPE Control 2 Register
    COMP_E_CMSIS(comparator)->rCTL2.r |= supplyVoltageReferenceBase
            + ((upperLimitSupplyVoltageFractionOf32 - 1) << 8)
            + (lowerLimitSupplyVoltageFractionOf32 - 1);
}


void COMP_E_setReferenceAccuracy(uint32_t comparator,
        uint_fast16_t referenceAccuracy)
{
    ASSERT(
            (referenceAccuracy == COMP_E_ACCURACY_STATIC)
            || (referenceAccuracy == COMP_E_ACCURACY_CLOCKED));

    if (referenceAccuracy)
        BITBAND_PERI(COMP_E_CMSIS(comparator)->rCTL2.r, CEREFACC_OFS) = 1;
    else
        BITBAND_PERI(COMP_E_CMSIS(comparator)->rCTL2.r, CEREFACC_OFS) = 0;

}


void COMP_E_setPowerMode(uint32_t comparator, uint_fast16_t powerMode)
{
    COMP_E_CMSIS(comparator)->rCTL1.r = (COMP_E_CMSIS(comparator)->rCTL1.r
            & ~(CEPWRMD_M)) | powerMode;
}


void COMP_E_enableModule(uint32_t comparator)
{
    BITBAND_PERI(COMP_E_CMSIS(comparator)->rCTL1.r, CEON_OFS) = 1;
}


void COMP_E_disableModule(uint32_t comparator)
{
    BITBAND_PERI(COMP_E_CMSIS(comparator)->rCTL1.r, CEON_OFS) = 0;
}


void COMP_E_shortInputs(uint32_t comparator)
{
    BITBAND_PERI(COMP_E_CMSIS(comparator)->rCTL1.r, CESHORT_OFS) = 1;
}


void COMP_E_unshortInputs(uint32_t comparator)
{
    BITBAND_PERI(COMP_E_CMSIS(comparator)->rCTL1.r, CESHORT_OFS) = 0;
}


void COMP_E_disableInputBuffer(uint32_t comparator, uint_fast16_t inputPort)
{
    ASSERT(inputPort <= COMP_E_INPUT15);

    COMP_E_CMSIS(comparator)->rCTL3.r |= (inputPort);
}


void COMP_E_enableInputBuffer(uint32_t comparator, uint_fast16_t inputPort)
{
    ASSERT(inputPort <= COMP_E_INPUT15);

    COMP_E_CMSIS(comparator)->rCTL3.r &= ~(inputPort);
}


void COMP_E_swapIO(uint32_t comparator)
{
    COMP_E_CMSIS(comparator)->rCTL1.r ^= CEEX; // Toggle CEEX bit
}


uint8_t COMP_E_outputValue(uint32_t comparator)
{
    return COMP_E_CMSIS(comparator)->rCTL1.r & CEOUT;
}


void COMP_E_enableInterrupt(uint32_t comparator, uint_fast16_t mask)
{
    // Set the Interrupt enable bit
    COMP_E_CMSIS(comparator)->rINT.r |= mask;
}


void COMP_E_disableInterrupt(uint32_t comparator, uint_fast16_t mask)
{
    COMP_E_CMSIS(comparator)->rINT.r &= ~(mask);
}


void COMP_E_clearInterruptFlag(uint32_t comparator, uint_fast16_t mask)
{
    COMP_E_CMSIS(comparator)->rINT.r &= ~(mask);
}


uint_fast16_t COMP_E_getInterruptStatus(uint32_t comparator)
{
    return (COMP_E_CMSIS(comparator)->rINT.r & (COMP_E_OUTPUT_INTERRUPT_FLAG |
    COMP_E_INTERRUPT_FLAG_INVERTED_POLARITY |
    COMP_E_INTERRUPT_FLAG_READY));
}


uint_fast16_t COMP_E_getEnabledInterruptStatus(uint32_t comparator)
{
    return COMP_E_getInterruptStatus(comparator) &
    COMP_E_CMSIS(comparator)->rINT.r;
}


void COMP_E_setInterruptEdgeDirection(uint32_t comparator,
        uint_fast8_t edgeDirection)
{
    ASSERT(edgeDirection <= COMP_E_RISINGEDGE);

    // Set the edge direction that will trigger an interrupt
    if (COMP_E_RISINGEDGE == edgeDirection)
        BITBAND_PERI(COMP_E_CMSIS(comparator)->rCTL1.r, CEIES_OFS) = 1;
    else if (COMP_E_FALLINGEDGE == edgeDirection)
        BITBAND_PERI(COMP_E_CMSIS(comparator)->rCTL1.r, CEIES_OFS) = 0;
}


void COMP_E_toggleInterruptEdgeDirection(uint32_t comparator)
{
    COMP_E_CMSIS(comparator)->rCTL1.r ^= CEIES;
}


void CRC32_setSeed(uint32_t seed, uint_fast8_t crcType)
{
    ASSERT((CRC16_MODE == crcType) || (CRC32_MODE == crcType));

    if (CRC16_MODE == crcType)
        CRC32->rCRC16INIRES = seed;
    else
    {
        CRC32->rCRC32INIRES_HI = ((seed & 0xFFFF0000) >> 16);
        CRC32->rCRC32INIRES_LO = (seed & 0xFFFF);
    }
}


void CRC32_set8BitData(uint8_t dataIn, uint_fast8_t crcType)
{
    ASSERT((CRC16_MODE == crcType) || (CRC32_MODE == crcType));

    if (CRC16_MODE == crcType)
        HWREG8(CRC32_BASE + OFS_CRC16DI) = dataIn;
    else
        HWREG8(CRC32_BASE + OFS_CRC32DI) = dataIn;
}


void CRC32_set16BitData(uint16_t dataIn, uint_fast8_t crcType)
{
    ASSERT((CRC16_MODE == crcType) || (CRC32_MODE == crcType));

    if (CRC16_MODE == crcType)
        CRC32->rCRC16DI = dataIn;
    else
        CRC32->rCRC32DI = dataIn;
}


void CRC32_set32BitData(uint32_t dataIn)
{
    //CRC32->rCRC32DI = dataIn & 0xFFFF;
    //CRC32->rCRC32DI = (uint16_t) ((dataIn & 0xFFFF0000) >> 16);

    HWREG16(CRC32_BASE + OFS_CRC32DI) = dataIn & 0xFFFF;
    HWREG16(CRC32_BASE + OFS_CRC32DI) = (uint16_t)(
            (dataIn & 0xFFFF0000) >> 16);
}


void CRC32_set8BitDataReversed(uint8_t dataIn, uint_fast8_t crcType)
{
    ASSERT((CRC16_MODE == crcType) || (CRC32_MODE == crcType));

    if (CRC16_MODE == crcType)
        HWREG8(CRC32_BASE + OFS_CRC16DIRB) = dataIn;
    else
        HWREG8(CRC32_BASE + OFS_CRC32DIRB) = dataIn;
}


void CRC32_set16BitDataReversed(uint16_t dataIn, uint_fast8_t crcType)
{
    ASSERT((CRC16_MODE == crcType) || (CRC32_MODE == crcType));

    if (CRC16_MODE == crcType)
        CRC32->rCRC16DIRB = dataIn;
    else
        CRC32->rCRC32DIRB = dataIn;
}


void CRC32_set32BitDataReversed(uint32_t dataIn)
{
    HWREG16(CRC32_BASE + OFS_CRC32DIRB) = dataIn & 0xFFFF;
    HWREG16(CRC32_BASE + OFS_CRC32DIRB) = (uint16_t)(
            (dataIn & 0xFFFF0000) >> 16);
    //CRC32->rCRC32DIRB = dataIn & 0xFFFF;
    //CRC32->rCRC32DIRB = (uint16_t) ((dataIn & 0xFFFF0000) >> 16);
}


uint32_t CRC32_getResult(uint_fast8_t crcType)
{
    uint32_t result;
    ASSERT((CRC16_MODE == crcType) || (CRC32_MODE == crcType));

    if (CRC16_MODE == crcType)
        return CRC32->rCRC16INIRES;
    else
    {
        result = CRC32->rCRC32INIRES_HI;
        result = (result << 16);
        result |= CRC32->rCRC32INIRES_LO;
        return (result);
    }
}


uint32_t CRC32_getResultReversed(uint_fast8_t crcType)
{
    uint32_t result;
    ASSERT((CRC16_MODE == crcType) || (CRC32_MODE == crcType));

    if (CRC16_MODE == crcType)
        return CRC32->rCRC16RESR;
    else
    {
        result = CRC32->rCRC32RESR_HI;
        result = (result << 16);
        result |= CRC32->rCRC32RESR_LO;
        return (result);
    }
}


#include <ti/devices/msp432p4xx/driverlib/cpu.h>
#include <ti/devices/msp432p4xx/inc/msp.h>
#include <stdint.h>

//*****************************************************************************
//
// Wrapper function for the CPSID instruction.  Returns the state of PRIMASK
// on entry.
//
//*****************************************************************************
#if defined(__GNUC__)
uint32_t __attribute__((naked)) CPU_cpsid(void)
{
    uint32_t ret;

    //
    // Read PRIMASK and disable interrupts.
    //
    __asm("    mrs     r0, PRIMASK\n"
            "    cpsid   i\n"
            "    bx      lr\n"
            : "=r" (ret));

    //
    // The return is handled in the inline assembly, but the compiler will
    // still complain if there is not an explicit return here (despite the fact
    // that this does not result in any code being produced because of the
    // naked attribute).
    //
    return(ret);
}
#endif
#if defined(__ICCARM__)
uint32_t CPU_cpsid(void)
{
    //
    // Read PRIMASK and disable interrupts.
    //
    __asm("    mrs     r0, PRIMASK\n"
            "    cpsid   i\n");

    //
    // "Warning[Pe940]: missing return statement at end of non-void function"
    // is suppressed here to avoid putting a "bx lr" in the inline assembly
    // above and a superfluous return statement here.
    //
#pragma diag_suppress=Pe940
}
#pragma diag_default=Pe940
#endif
#if defined(__CC_ARM)
__asm uint32_t CPU_cpsid(void)
{
    //
    // Read PRIMASK and disable interrupts.
    //
    mrs r0, PRIMASK;
    cpsid i;
    bx lr
}
#endif
#if defined(__TI_ARM__)
uint32_t CPU_cpsid(void)
{
    //
    // Read PRIMASK and disable interrupts.
    //
    __asm("    mrs     r0, PRIMASK\n"
            "    cpsid   i\n"
            "    bx      lr\n");

    //
    // The following keeps the compiler happy, because it wants to see a
    // return value from this function.  It will generate code to return
    // a zero.  However, the real return is the "bx lr" above, so the
    // return(0) is never executed and the function returns with the value
    // you expect in R0.
    //
    return(0);
}
#endif

//*****************************************************************************
//
// Wrapper function returning the state of PRIMASK (indicating whether
// interrupts are enabled or disabled).
//
//*****************************************************************************
#if defined(__GNUC__)
uint32_t __attribute__((naked)) CPU_primask(void)
{
    uint32_t ret;

    //
    // Read PRIMASK and disable interrupts.
    //
    __asm("    mrs     r0, PRIMASK\n"
            "    bx      lr\n"
            : "=r" (ret));

    //
    // The return is handled in the inline assembly, but the compiler will
    // still complain if there is not an explicit return here (despite the fact
    // that this does not result in any code being produced because of the
    // naked attribute).
    //
    return(ret);
}
#endif
#if defined(__ICCARM__)
uint32_t CPU_primask(void)
{
    //
    // Read PRIMASK and disable interrupts.
    //
    __asm("    mrs     r0, PRIMASK\n");

    //
    // "Warning[Pe940]: missing return statement at end of non-void function"
    // is suppressed here to avoid putting a "bx lr" in the inline assembly
    // above and a superfluous return statement here.
    //
#pragma diag_suppress=Pe940
}
#pragma diag_default=Pe940
#endif
#if defined(__CC_ARM)
__asm uint32_t CPU_primask(void)
{
    //
    // Read PRIMASK and disable interrupts.
    //
    mrs r0, PRIMASK;
    bx lr
}
#endif
#if defined(__TI_ARM__)
uint32_t CPU_primask(void)
{
    //
    // Read PRIMASK and disable interrupts.
    //
    __asm("    mrs     r0, PRIMASK\n"
            "    bx      lr\n");

    //
    // The following keeps the compiler happy, because it wants to see a
    // return value from this function.  It will generate code to return
    // a zero.  However, the real return is the "bx lr" above, so the
    // return(0) is never executed and the function returns with the value
    // you expect in R0.
    //
    return(0);
}
#endif

//*****************************************************************************
//
// Wrapper function for the CPSIE instruction.  Returns the state of PRIMASK
// on entry.
//
//*****************************************************************************
#if defined(__GNUC__)
uint32_t __attribute__((naked)) CPU_cpsie(void)
{
    uint32_t ret;

    //
    // Read PRIMASK and enable interrupts.
    //
    __asm("    mrs     r0, PRIMASK\n"
            "    cpsie   i\n"
            "    bx      lr\n"
            : "=r" (ret));

    //
    // The return is handled in the inline assembly, but the compiler will
    // still complain if there is not an explicit return here (despite the fact
    // that this does not result in any code being produced because of the
    // naked attribute).
    //
    return(ret);
}
#endif
#if defined(__ICCARM__)
uint32_t CPU_cpsie(void)
{
    //
    // Read PRIMASK and enable interrupts.
    //
    __asm("    mrs     r0, PRIMASK\n"
            "    cpsie   i\n");

    //
    // "Warning[Pe940]: missing return statement at end of non-void function"
    // is suppressed here to avoid putting a "bx lr" in the inline assembly
    // above and a superfluous return statement here.
    //
#pragma diag_suppress=Pe940
}
#pragma diag_default=Pe940
#endif
#if defined(__CC_ARM)
__asm uint32_t CPU_cpsie(void)
{
    //
    // Read PRIMASK and enable interrupts.
    //
    mrs r0, PRIMASK;
    cpsie i;
    bx lr
}
#endif
#if defined(__TI_ARM__)
uint32_t CPU_cpsie(void)
{
    //
    // Read PRIMASK and enable interrupts.
    //
    __asm("    mrs     r0, PRIMASK\n"
            "    cpsie   i\n"
            "    bx      lr\n");

    //
    // The following keeps the compiler happy, because it wants to see a
    // return value from this function.  It will generate code to return
    // a zero.  However, the real return is the "bx lr" above, so the
    // return(0) is never executed and the function returns with the value
    // you expect in R0.
    //
    return(0);
}
#endif

//*****************************************************************************
//
// Wrapper function for the CPUWFI instruction.
//
//*****************************************************************************
#if defined(__GNUC__)
void __attribute__((naked)) CPU_wfi(void)
{
    //
    // Wait for the next interrupt.
    //
    __asm("    wfi\n"
            "    bx      lr\n");
}
#endif
#if defined(__ICCARM__)
void CPU_wfi(void)
{
    //
    // Wait for the next interrupt.
    //
    __asm("    wfi\n");
}
#endif
#if defined(__CC_ARM)
__asm void CPU_wfi(void)
{
    //
    // Wait for the next interrupt.
    //
    wfi;
    bx lr
}
#endif
#if defined(__TI_ARM__)
void CPU_wfi(void)
{
    //
    // Wait for the next interrupt.
    //
    __asm("    wfi\n");
}
#endif

//*****************************************************************************
//
// Wrapper function for writing the BASEPRI register.
//
//*****************************************************************************
#if defined(__GNUC__)
void __attribute__((naked)) CPU_basepriSet(uint32_t newBasepri)
{
    //
    // Set the BASEPRI register
    //
    __asm("    msr     BASEPRI, r0\n"
            "    bx      lr\n");
}
#endif
#if defined(__ICCARM__)
void CPU_basepriSet(uint32_t newBasepri)
{
    //
    // Set the BASEPRI register
    //
    __asm("    msr     BASEPRI, r0\n");
}
#endif
#if defined(__CC_ARM)
__asm void CPU_basepriSet(uint32_t newBasepri)
{
    //
    // Set the BASEPRI register
    //
    msr BASEPRI, r0;
    bx lr
}
#endif
#if defined(__TI_ARM__)
void CPU_basepriSet(uint32_t newBasepri)
{
    //
    // Set the BASEPRI register
    //
    __asm("    msr     BASEPRI, r0\n");
}
#endif

//*****************************************************************************
//
// Wrapper function for reading the BASEPRI register.
//
//*****************************************************************************
#if defined(__GNUC__)
uint32_t __attribute__((naked)) CPU_basepriGet(void)
{
    uint32_t ret;

    //
    // Read BASEPRI
    //
    __asm("    mrs     r0, BASEPRI\n"
            "    bx      lr\n"
            : "=r" (ret));

    //
    // The return is handled in the inline assembly, but the compiler will
    // still complain if there is not an explicit return here (despite the fact
    // that this does not result in any code being produced because of the
    // naked attribute).
    //
    return(ret);
}
#endif
#if defined(__ICCARM__)
uint32_t CPU_basepriGet(void)
{
    //
    // Read BASEPRI
    //
    __asm("    mrs     r0, BASEPRI\n");

    //
    // "Warning[Pe940]: missing return statement at end of non-void function"
    // is suppressed here to avoid putting a "bx lr" in the inline assembly
    // above and a superfluous return statement here.
    //
#pragma diag_suppress=Pe940
}
#pragma diag_default=Pe940
#endif
#if defined(__CC_ARM)
__asm uint32_t CPU_basepriGet(void)
{
    //
    // Read BASEPRI
    //
    mrs r0, BASEPRI;
    bx lr
}
#endif
#if defined(__TI_ARM__)
uint32_t CPU_basepriGet(void)
{
    //
    // Read BASEPRI
    //
    __asm("    mrs     r0, BASEPRI\n"
            "    bx      lr\n");

    //
    // The following keeps the compiler happy, because it wants to see a
    // return value from this function.  It will generate code to return
    // a zero.  However, the real return is the "bx lr" above, so the
    // return(0) is never executed and the function returns with the value
    // you expect in R0.
    //
    return(0);
}
#endif
void CS_initClockSignal(uint32_t selectedClockSignal, uint32_t clockSource,
        uint32_t clockSourceDivider)
{
    ASSERT(_CSIsClockDividerValid(clockSourceDivider));

    /* Unlocking the CS Module */
    CS->rKEY.r = CS_KEY;

    switch (selectedClockSignal)
    {
    case CS_ACLK:
    {
        /* Making sure that the clock signal for ACLK isn't set to anything
         * invalid
         */
        ASSERT(
                (selectedClockSignal != CS_DCOCLK_SELECT)
                && (selectedClockSignal != CS_MODOSC_SELECT)
                && (selectedClockSignal != CS_HFXTCLK_SELECT));

        /* Waiting for the clock source ready bit to be valid before
         * changing */
        while (!BITBAND_PERI(CS->rSTAT.r, ACLK_READY_OFS))
            ;

        /* Setting the divider and source */
        CS->rCTL1.r = ((clockSourceDivider >> CS_ACLK_DIV_BITPOS)
                | (clockSource << CS_ACLK_SRC_BITPOS))
                | (CS->rCTL1.r & ~(SELA_M | DIVA_M));

        /* Waiting for ACLK to be ready again */
        while (!BITBAND_PERI(CS->rSTAT.r, ACLK_READY_OFS))
            ;

        break;
    }
    case CS_MCLK:
    {

        /* Waiting for the clock source ready bit to be valid before
         * changing */
        while (!BITBAND_PERI(CS->rSTAT.r, MCLK_READY_OFS))
            ;

        CS->rCTL1.r = ((clockSourceDivider >> CS_MCLK_DIV_BITPOS)
                | (clockSource << CS_MCLK_SRC_BITPOS))
                | (CS->rCTL1.r & ~(SELM_M | DIVM_M));

        /* Waiting for MCLK to be ready */
        while (!BITBAND_PERI(CS->rSTAT.r, MCLK_READY_OFS))
            ;

        break;
    }
    case CS_SMCLK:
    {
        /* Waiting for the clock source ready bit to be valid before
         * changing */
        while (!BITBAND_PERI(CS->rSTAT.r, SMCLK_READY_OFS))
            ;

        CS->rCTL1.r = ((clockSourceDivider >> CS_SMCLK_DIV_BITPOS)
                | (clockSource << CS_HSMCLK_SRC_BITPOS))
                | (CS->rCTL1.r & ~(DIVS_M | SELS_M));

        /* Waiting for SMCLK to be ready */
        while (!BITBAND_PERI(CS->rSTAT.r, SMCLK_READY_OFS))
            ;

        break;
    }
    case CS_HSMCLK:
    {
        /* Waiting for the clock source ready bit to be valid before
         * changing */
        while (!BITBAND_PERI(CS->rSTAT.r, HSMCLK_READY_OFS))
            ;

        CS->rCTL1.r = ((clockSourceDivider >> CS_HSMCLK_DIV_BITPOS)
                | (clockSource << CS_HSMCLK_SRC_BITPOS))
                | (CS->rCTL1.r & ~(DIVHS_M | SELS_M));

        /* Waiting for HSMCLK to be ready */
        while (!BITBAND_PERI(CS->rSTAT.r, HSMCLK_READY_OFS))
            ;

        break;
    }
    case CS_BCLK:
    {

        /* Waiting for the clock source ready bit to be valid before
         * changing */
        while (!BITBAND_PERI(CS->rSTAT.r, BCLK_READY_OFS))
            ;

        /* Setting the clock source and then returning
         * (cannot divide CLK)
         */
        if (clockSource == CS_LFXTCLK_SELECT)
            BITBAND_PERI(CS->rCTL1.r, SELB_OFS) = 0;
        else if (clockSource == CS_REFOCLK_SELECT)
            BITBAND_PERI(CS->rCTL1.r, SELB_OFS) = 1;
        else
            ASSERT(false);

        /* Waiting for BCLK to be ready */
        while (!BITBAND_PERI(CS->rSTAT.r, BCLK_READY_OFS))
            ;

        break;
    }
    default:
    {
        /* Should never get here */
        ASSERT(false);
    }
    }

    /* Locking the module */
    BITBAND_PERI(CS->rKEY.r, CSKEY_OFS) = 1;
}


void CS_setReferenceOscillatorFrequency(uint8_t referenceFrequency)
{
    ASSERT(
            referenceFrequency == CS_REFO_32KHZ
            || referenceFrequency == CS_REFO_128KHZ);

    /* Unlocking the module */
    CS->rKEY.r = CS_KEY;

    BITBAND_PERI(CS->rCLKEN.r, REFOFSEL_OFS) = referenceFrequency;

    /* Locking the module */
    BITBAND_PERI(CS->rKEY.r, CSKEY_OFS) = 1;
}


void CS_enableClockRequest(uint32_t selectClock)
{
    ASSERT(
            selectClock == CS_ACLK || selectClock == CS_HSMCLK
            || selectClock == CS_SMCLK || selectClock == CS_MCLK);

    /* Unlocking the module */
    CS->rKEY.r = CS_KEY;

    CS->rCLKEN.r |= selectClock;

    /* Locking the module */
    BITBAND_PERI(CS->rKEY.r, CSKEY_OFS) = 1;
}


//******************************************************************************
// Internal function for getting DCO nominal frequency
//******************************************************************************
static uint32_t _CSGetDOCFrequency(void)
{
    uint32_t dcoFreq;

    switch (CS->rCTL0.r & DCORSEL_M)
    {
    case DCORSEL_0:
        dcoFreq = 1500000;
        break;
    case DCORSEL_1:
        dcoFreq = 3000000;
        break;
    case DCORSEL_2:
        dcoFreq = 6000000;
        break;
    case DCORSEL_3:
        dcoFreq = 12000000;
        break;
    case DCORSEL_4:
        dcoFreq = 24000000;
        break;
    case DCORSEL_5:
        dcoFreq = 48000000;
        break;
    default:
        dcoFreq = 0;
    }

    return (dcoFreq);
}


void CS_disableClockRequest(uint32_t selectClock)
{
    ASSERT(
            selectClock == CS_ACLK || selectClock == CS_HSMCLK
            || selectClock == CS_SMCLK || selectClock == CS_MCLK);

    /* Unlocking the module */
    CS->rKEY.r = CS_KEY;

    CS->rCLKEN.r &= ~selectClock;

    /* Locking the module */
    BITBAND_PERI(CS->rKEY.r, CSKEY_OFS) = 1;
}


void CS_setDCOCenteredFrequency(uint32_t dcoFreq)
{
    ASSERT(
            dcoFreq == CS_DCO_FREQUENCY_1_5 || dcoFreq == CS_DCO_FREQUENCY_3
            || dcoFreq == CS_DCO_FREQUENCY_6
            || dcoFreq == CS_DCO_FREQUENCY_12
            || dcoFreq == CS_DCO_FREQUENCY_24
            || dcoFreq == CS_DCO_FREQUENCY_48);

    /* Unlocking the CS Module */
    CS->rKEY.r = CS_KEY;

    /* Resetting Tuning Parameters and Setting the frequency */
    CS->rCTL0.r = ((CS->rCTL0.r & ~DCORSEL_M) | dcoFreq);

    /* Locking the CS Module */
    BITBAND_PERI(CS->rKEY.r, CSKEY_OFS) = 1;
}


void CS_tuneDCOFrequency(int16_t tuneParameter)
{
    CS->rKEY.r = CS_KEY;

    if (tuneParameter < 0)
    {
         CS->rCTL0.r = ((CS->rCTL0.r & ~DCOTUNE_M) | (tuneParameter & DCOTUNE_M)
                 | 0x200);
    } 
    else
    {
        CS->rCTL0.r =
                ((CS->rCTL0.r & ~DCOTUNE_M) | (tuneParameter & DCOTUNE_M));

    }
     
     BITBAND_PERI(CS->rKEY.r, CSKEY_OFS) = 1;
}


void CS_enableDCOExternalResistor(void)
{
    /* Unlocking the module */
    CS->rKEY.r = CS_KEY;

    BITBAND_PERI(CS->rCTL0.r,DCORES_OFS) = 1;

    /* Locking the module */
    BITBAND_PERI(CS->rKEY.r, CSKEY_OFS) = 1;
}


void CS_disableDCOExternalResistor(void)
{
    /* Unlocking the module */
    CS->rKEY.r = CS_KEY;

    BITBAND_PERI(CS->rCTL0.r,DCORES_OFS) = 0;

    /* Locking the module */
    BITBAND_PERI(CS->rKEY.r, CSKEY_OFS) = 1;
}


void CS_enableInterrupt(uint32_t flags)
{
    /* Unlocking the module */
    CS->rKEY.r = CS_KEY;

    CS->rIE.r |= flags;

    /* Locking the module */
    BITBAND_PERI(CS->rKEY.r, CSKEY_OFS) = 1;
}


void CS_disableInterrupt(uint32_t flags)
{
    /* Unlocking the module */
    CS->rKEY.r = CS_KEY;

    CS->rIE.r &= ~flags;

    /* Locking the module */
    BITBAND_PERI(CS->rKEY.r, CSKEY_OFS) = 1;
}


uint32_t CS_getEnabledInterruptStatus(void)
{
    return CS_getInterruptStatus() & CS->rIE.r;
}


uint32_t CS_getInterruptStatus(void)
{
    return CS->rIFG.r;
}


uint32_t CS_getDCOFrequency(void)
{
    float dcoConst;
    int32_t calVal;
    uint32_t centeredFreq;
    int16_t dcoTune;
    uint_fast8_t tlvLength;
    SysCtl_CSCalTLV_Info *csInfo;

    dcoTune = CS->rCTL0.b.bDCOTUNE;
    centeredFreq = _CSGetDOCFrequency();

    /* Parsing the TLV and getting the maximum erase pulses */
    SysCtl_getTLVInfo(TLV_TAG_CS, 0, &tlvLength, (uint32_t**)&csInfo);

    if(tlvLength == 0)
    {
        return centeredFreq;
    }

    if (dcoTune == 0)
        return (uint32_t) centeredFreq;

    /* Checking to see if we need to do signed conversion */
    if (dcoTune & 0x200)
    {
        dcoTune = dcoTune | 0xFE00;
    }
    
    /* DCORSEL = 5  */
    if (centeredFreq == 48000000)
    {
         /* External Resistor */
        if (BITBAND_PERI(CS->rCTL0.r, DCORES_OFS))
        {
            dcoConst = *((float *) &csInfo->rDCOER_CONSTK_RSEL5);
            calVal = csInfo->rDCOER_FCAL_RSEL5;
        }
        /* Internal Resistor */
        else
        {
            dcoConst = *((float *) &csInfo->rDCOIR_CONSTK_RSEL5);
            calVal = csInfo->rDCOIR_FCAL_RSEL5;
        }
    }
    /* DCORSEL = 4 */
    else
    {
        /* External Resistor */
        if (BITBAND_PERI(CS->rCTL0.r, DCORES_OFS))
        {
            dcoConst = *((float *) &csInfo->rDCOER_CONSTK_RSEL04);
            calVal = csInfo->rDCOER_FCAL_RSEL04;
        }
        /* Internal Resistor */
        else
        {
            dcoConst = *((float *) &csInfo->rDCOIR_CONSTK_RSEL04);
            calVal = csInfo->rDCOIR_FCAL_RSEL04;
        }
    }

    return (uint32_t) ((centeredFreq)
            / (1
                    - ((dcoConst * dcoTune)
                            / ((1 + dcoConst * (768 - calVal))))));
}


void CS_setDCOFrequency(uint32_t dcoFrequency)
{
    int32_t nomFreq, calVal, dcoSigned;
    int16_t dcoTune;
    float dcoConst;
    bool rsel5 = false;
    dcoSigned = (int32_t) dcoFrequency;
    uint_fast8_t tlvLength;
     SysCtl_CSCalTLV_Info *csInfo;

    if (dcoFrequency < 2000000)
    {
        nomFreq = CS_15MHZ;
        CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_1_5);
    } else if (dcoFrequency < 4000000)
    {
        nomFreq = CS_3MHZ;
        CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_3);
    } else if (dcoFrequency < 8000000)
    {
        nomFreq = CS_6MHZ;
        CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_6);
    } else if (dcoFrequency < 16000000)
    {
        nomFreq = CS_12MHZ;
        CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_12);
    } else if (dcoFrequency < 32000000)
    {
        nomFreq = CS_24MHZ;
        CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_24);
    } else if (dcoFrequency < 640000001)
    {
        nomFreq = CS_48MHZ;
        CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_48);
        rsel5 = true;
    } else
    {
        ASSERT(false);
        return;
    }
    
    /* Parsing the TLV and getting the maximum erase pulses */
    SysCtl_getTLVInfo(TLV_TAG_CS, 0, &tlvLength, (uint32_t**)&csInfo);

    if(dcoFrequency == nomFreq || tlvLength == 0)
    {
       CS_tuneDCOFrequency(0);
       return;
    }

    if (rsel5)
    {
        /* External Resistor*/
        if (BITBAND_PERI(CS->rCTL0.r, DCORES_OFS))
        {
            dcoConst = *((float *) &csInfo->rDCOER_CONSTK_RSEL5);
            calVal = csInfo->rDCOER_FCAL_RSEL5;
        }
        /* Internal Resistor */
        else
        {
            dcoConst = *((float *) &csInfo->rDCOIR_CONSTK_RSEL5);
            calVal = csInfo->rDCOIR_FCAL_RSEL5;
        }
    }
    /* DCORSEL = 4 */
    else
    {
        /* External Resistor */
        if (BITBAND_PERI(CS->rCTL0.r, DCORES_OFS))
        {
            dcoConst = *((float *) &csInfo->rDCOER_CONSTK_RSEL04);
            calVal = csInfo->rDCOER_FCAL_RSEL04;
        }
        /* Internal Resistor */
        else
        {
            dcoConst = *((float *) &csInfo->rDCOIR_CONSTK_RSEL04);
            calVal = csInfo->rDCOIR_FCAL_RSEL04;
        }
    }

    dcoTune = (int16_t) (((dcoSigned - nomFreq)
            * (1.0f + dcoConst * (768.0f - calVal)))
            / (dcoSigned * dcoConst));

    CS_tuneDCOFrequency(dcoTune);

}


void CS_setDCOExternalResistorCalibration(uint_fast8_t calData, 
                                            uint_fast8_t freqRange)
{
    uint_fast8_t rselVal;

    /* Unlocking the module */
    CS->rKEY.r = CS_KEY;

    rselVal = CS->rCTL0.b.bDCORSEL;

    CS->rCTL0.b.bDCORSEL = DCORSEL_0;

    if(freqRange == CS_OVER32MHZ)
    {
        CS->rDCOERCAL1.b.bDCO_FCAL_RSEL5 = (calData);
    }
    else
    {
        CS->rDCOERCAL0.b.bDCO_FCAL_RSEL04 = (calData);
    }

    CS->rCTL0.b.bDCORSEL = rselVal;

    /* Locking the module */
    BITBAND_PERI(CS->rKEY.r, CSKEY_OFS) = 1;

}


void CS_enableFaultCounter(uint_fast8_t counterSelect)
{
    ASSERT(counterSelect == CS_HFXT_FAULT_COUNTER ||
            counterSelect == CS_HFXT_FAULT_COUNTER);

    /* Unlocking the module */
    CS->rKEY.r = CS_KEY;

    if (counterSelect == CS_HFXT_FAULT_COUNTER)
    {
        BITBAND_PERI(CS->rCTL3.r, FCNTHF_EN_OFS) = 1;
    } else
    {
        BITBAND_PERI(CS->rCTL3.r, FCNTLF_EN_OFS) = 1;
    }

    /* Locking the module */
    BITBAND_PERI(CS->rKEY.r, CSKEY_OFS) = 1;
}


void CS_disableFaultCounter(uint_fast8_t counterSelect)
{
    ASSERT(counterSelect == CS_HFXT_FAULT_COUNTER ||
            counterSelect == CS_HFXT_FAULT_COUNTER);

    /* Unlocking the module */
    CS->rKEY.r = CS_KEY;

    if (counterSelect == CS_HFXT_FAULT_COUNTER)
    {
        BITBAND_PERI(CS->rCTL3.r, FCNTHF_EN_OFS) = 0;
    } else
    {
        BITBAND_PERI(CS->rCTL3.r, FCNTLF_EN_OFS) = 0;
    }

    /* Locking the module */
    BITBAND_PERI(CS->rKEY.r, CSKEY_OFS) = 1;
}


void CS_resetFaultCounter(uint_fast8_t counterSelect)
{
    ASSERT(counterSelect == CS_HFXT_FAULT_COUNTER ||
            counterSelect == CS_HFXT_FAULT_COUNTER);

    /* Unlocking the module */
    CS->rKEY.r = CS_KEY;

    if (counterSelect == CS_HFXT_FAULT_COUNTER)
    {
        BITBAND_PERI(CS->rCTL3.r, RFCNTHF_OFS) = 1;
    } else
    {
        BITBAND_PERI(CS->rCTL3.r, RFCNTLF_OFS) = 1;
    }

    /* Locking the module */
    BITBAND_PERI(CS->rKEY.r, CSKEY_OFS) = 1;
}


void CS_startFaultCounter(uint_fast8_t counterSelect, uint_fast8_t countValue)
{
    ASSERT(counterSelect == CS_HFXT_FAULT_COUNTER ||
            counterSelect == CS_HFXT_FAULT_COUNTER);

    ASSERT(countValue == CS_FAULT_COUNTER_4096_CYCLES ||
            countValue == CS_FAULT_COUNTER_8192_CYCLES ||
            countValue == CS_FAULT_COUNTER_16384_CYCLES ||
            countValue == CS_FAULT_COUNTER_32768_CYCLES);

    /* Unlocking the module */
    CS->rKEY.r = CS_KEY;

    if (counterSelect == CS_HFXT_FAULT_COUNTER)
    {
        CS->rCTL3.r = ((CS->rCTL3.r & ~FCNTHF_M) | (countValue << 4));
    } else
    {
        CS->rCTL3.r = ((CS->rCTL3.r & ~FCNTLF_M) | (countValue));
    }

    /* Locking the module */
    BITBAND_PERI(CS->rKEY.r, CSKEY_OFS) = 1;
}


void CS_clearInterruptFlag(uint32_t flags)
{
    /* Unlocking the module */
    CS->rKEY.r = CS_KEY;

    CS->rCLRIFG.r |= flags;

    /* Locking the module */
    BITBAND_PERI(CS->rKEY.r, CSKEY_OFS) = 1;
}


void DMA_enableModule(void)
{
    //
    // Set the master enable bit in the config register.
    //
    DMA->rCFG.r = DMA_CFG_MASTEN;
}


void DMA_disableModule(void)
{
    //
    // Clear the master enable bit in the config register.
    //
    DMA->rCFG.r = 0;
}


uint32_t DMA_getErrorStatus(void)
{
    //
    // Return the DMA error status.
    //
    return DMA->rERRCLR.r;
}


void DMA_clearErrorStatus(void)
{
    //
    // Clear the DMA error interrupt.
    //
    DMA->rERRCLR.r = 1;
}


void DMA_enableChannel(uint32_t channelNum)
{
    //
    // Check the arguments.
    //
    ASSERT((channelNum & 0xffff) < 8);

    //
    // Set the bit for this channel in the enable set register.
    //
    DMA->rENASET = 1 << (channelNum & 0x0F);
}


void DMA_disableChannel(uint32_t channelNum)
{
    //
    // Check the arguments.
    //
    ASSERT((channelNum & 0xffff) < 8);

    //
    // Set the bit for this channel in the enable clear register.
    //
    DMA->rENACLR = 1 << (channelNum & 0x0F);
}


bool DMA_isChannelEnabled(uint32_t channelNum)
{
    //
    // Check the arguments.
    //
    ASSERT((channelNum & 0xffff) < 8);

    //
    // AND the specified channel bit with the enable register and return the
    // result.
    //
    return ((DMA->rENASET & (1 << (channelNum & 0x0F))) ? true : false);
}


void DMA_setControlBase(void *controlTable)
{
    //
    // Check the arguments.
    //
    ASSERT(((uint32_t) controlTable & ~0x3FF) == (uint32_t) controlTable);
    ASSERT((uint32_t) controlTable >= 0x20000000);

    //
    // Program the base address into the register.
    //
    DMA->rCTLBASE.r = (uint32_t) controlTable;
}


void* DMA_getControlBase(void)
{
    //
    // Read the current value of the control base register and return it to
    // the caller.
    //
    return ((void *) DMA->rCTLBASE.r);
}


void* DMA_getControlAlternateBase(void)
{
    //
    // Read the current value of the control base register and return it to
    // the caller.
    //
    return ((void *) DMA->rALTBASE);
}


void DMA_requestChannel(uint32_t channelNum)
{
    //
    // Check the arguments.
    //
    ASSERT((channelNum & 0xffff) < 8);

    //
    // Set the bit for this channel in the software DMA request register.
    //
    DMA->rSWREQ = 1 << (channelNum & 0x0F);
}


void DMA_enableChannelAttribute(uint32_t channelNum, uint32_t attr)
{
    //
    // Check the arguments.
    //
    ASSERT((channelNum & 0xffff) < 8);
    ASSERT(
            (attr
                    & ~(UDMA_ATTR_USEBURST | UDMA_ATTR_ALTSELECT
                            | UDMA_ATTR_HIGH_PRIORITY | UDMA_ATTR_REQMASK))
            == 0);

    //
    // In case a channel selector macro (like UDMA_CH0_USB0EP1RX) was
    // passed as the channelNum parameter, extract just the channel number
    // from this parameter.
    //
    channelNum &= 0x0F;

    //
    // Set the useburst bit for this channel if set in config.
    //
    if (attr & UDMA_ATTR_USEBURST)
    {
        DMA->rUSEBURSTSET = 1 << channelNum;
    }

    //
    // Set the alternate control select bit for this channel,
    // if set in config.
    //
    if (attr & UDMA_ATTR_ALTSELECT)
    {
        DMA->rALTSET = 1 << channelNum;
    }

    //
    // Set the high priority bit for this channel, if set in config.
    //
    if (attr & UDMA_ATTR_HIGH_PRIORITY)
    {
        DMA->rPRIOSET = 1 << channelNum;
    }

    //
    // Set the request mask bit for this channel, if set in config.
    //
    if (attr & UDMA_ATTR_REQMASK)
    {
        DMA->rREQMASKSET = 1 << channelNum;
    }
}


void DMA_disableChannelAttribute(uint32_t channelNum, uint32_t attr)
{
    //
    // Check the arguments.
    //
    ASSERT((channelNum & 0xffff) < 8);
    ASSERT(
            (attr
                    & ~(UDMA_ATTR_USEBURST | UDMA_ATTR_ALTSELECT
                            | UDMA_ATTR_HIGH_PRIORITY | UDMA_ATTR_REQMASK))
            == 0);

    //
    // In case a channel selector macro (like UDMA_CH0_USB0EP1RX) was
    // passed as the channelNum parameter, extract just the channel number
    // from this parameter.
    //
    channelNum &= 0x0F;

    //
    // Clear the useburst bit for this channel if set in config.
    //
    if (attr & UDMA_ATTR_USEBURST)
    {
        DMA->rUSEBURSTCLR = 1 << channelNum;
    }

    //
    // Clear the alternate control select bit for this channel, if set in
    // config.
    //
    if (attr & UDMA_ATTR_ALTSELECT)
    {
        DMA->rALTCLR = 1 << channelNum;
    }

    //
    // Clear the high priority bit for this channel, if set in config.
    //
    if (attr & UDMA_ATTR_HIGH_PRIORITY)
    {
        DMA->rPRIOCLR = 1 << channelNum;
    }

    //
    // Clear the request mask bit for this channel, if set in config.
    //
    if (attr & UDMA_ATTR_REQMASK)
    {
        DMA->rREQMASKCLR = 1 << channelNum;
    }
}


uint32_t DMA_getChannelAttribute(uint32_t channelNum)
{
    uint32_t attr = 0;

    //
    // Check the arguments.
    //
    ASSERT((channelNum & 0xffff) < 8);

    //
    // In case a channel selector macro (like UDMA_CH0_USB0EP1RX) was
    // passed as the channelNum parameter, extract just the channel number
    // from this parameter.
    //
    channelNum &= 0x0F;

    //
    // Check to see if useburst bit is set for this channel.
    //
    if (DMA->rUSEBURSTSET & (1 << channelNum))
    {
        attr |= UDMA_ATTR_USEBURST;
    }

    //
    // Check to see if the alternate control bit is set for this channel.
    //
    if (DMA->rALTSET & (1 << channelNum))
    {
        attr |= UDMA_ATTR_ALTSELECT;
    }

    //
    // Check to see if the high priority bit is set for this channel.
    //
    if (DMA->rPRIOSET & (1 << channelNum))
    {
        attr |= UDMA_ATTR_HIGH_PRIORITY;
    }

    //
    // Check to see if the request mask bit is set for this channel.
    //
    if (DMA->rREQMASKSET & (1 << channelNum))
    {
        attr |= UDMA_ATTR_REQMASK;
    }

    //
    // Return the configuration flags.
    //
    return (attr);
}


void DMA_setChannelControl(uint32_t channelStructIndex, uint32_t control)
{
    DMA_ControlTable *pCtl;

    //
    // Check the arguments.
    //
    ASSERT((channelStructIndex & 0xffff) < 64);
    ASSERT(DMA->rCTLBASE != 0);

    //
    // In case a channel selector macro (like UDMA_CH0_USB0EP1RX) was
    // passed as the channelStructIndex parameter, extract just the channel
    // index from this parameter.
    //
    channelStructIndex &= 0x3f;

    //
    // Get the base address of the control table.
    //
    pCtl = (DMA_ControlTable *) DMA->rCTLBASE.r;

    //
    // Get the current control word value and mask off the fields to be
    // changed, then OR in the new settings.
    //
    pCtl[channelStructIndex].control = ((pCtl[channelStructIndex].control
            & ~(UDMA_CHCTL_DSTINC_M | UDMA_CHCTL_DSTSIZE_M | UDMA_CHCTL_SRCINC_M
                    | UDMA_CHCTL_SRCSIZE_M | UDMA_CHCTL_ARBSIZE_M
                    | UDMA_CHCTL_NXTUSEBURST)) | control);
}


void DMA_setChannelTransfer(uint32_t channelStructIndex, uint32_t mode,
        void *srcAddr, void *dstAddr, uint32_t transferSize)
{
    DMA_ControlTable *controlTable;
    uint32_t control;
    uint32_t increment;
    uint32_t bufferBytes;

    //
    // Check the arguments.
    //
    ASSERT((channelStructIndex & 0xffff) < 64);
    ASSERT(DMA->rCTLBASE != 0);
    ASSERT(mode <= UDMA_MODE_PER_SCATTER_GATHER);
    ASSERT((transferSize != 0) && (transferSize <= 1024));

    //
    // In case a channel selector macro (like UDMA_CH0_USB0EP1RX) was
    // passed as the channelStructIndex parameter, extract just the channel
    // index from this parameter.
    //
    channelStructIndex &= 0x3f;

    //
    // Get the base address of the control table.
    //
    controlTable = (DMA_ControlTable *) DMA->rCTLBASE.r;

    //
    // Get the current control word value and mask off the mode and size
    // fields.
    //
    control = (controlTable[channelStructIndex].control
            & ~(UDMA_CHCTL_XFERSIZE_M | UDMA_CHCTL_XFERMODE_M));

    //
    // Adjust the mode if the alt control structure is selected.
    //
    if (channelStructIndex & UDMA_ALT_SELECT)
    {
        if ((mode == UDMA_MODE_MEM_SCATTER_GATHER)
                || (mode == UDMA_MODE_PER_SCATTER_GATHER))
        {
            mode |= UDMA_MODE_ALT_SELECT;
        }
    }

    //
    // Set the transfer size and mode in the control word (but don't write the
    // control word yet as it could kick off a transfer).
    //
    control |= mode | ((transferSize - 1) << 4);

    //
    // Get the address increment value for the source, from the control word.
    //
    increment = (control & UDMA_CHCTL_SRCINC_M);

    //
    // Compute the ending source address of the transfer.  If the source
    // increment is set to none, then the ending address is the same as the
    // beginning.
    //
    if (increment != UDMA_SRC_INC_NONE)
    {
        increment = increment >> 26;
        bufferBytes = transferSize << increment;
        srcAddr = (void *) ((uint32_t) srcAddr + bufferBytes - 1);
    }

    //
    // Load the source ending address into the control block.
    //
    controlTable[channelStructIndex].srcEndAddr = srcAddr;

    //
    // Get the address increment value for the destination, from the control
    // word.
    //
    increment = control & UDMA_CHCTL_DSTINC_M;

    //
    // Compute the ending destination address of the transfer.  If the
    // destination increment is set to none, then the ending address is the
    // same as the beginning.
    //
    if (increment != UDMA_DST_INC_NONE)
    {
        //
        // There is a special case if this is setting up a scatter-gather
        // transfer.  The destination pointer must point to the end of
        // the alternate structure for this channel instead of calculating
        // the end of the buffer in the normal way.
        //
        if ((mode == UDMA_MODE_MEM_SCATTER_GATHER)
                || (mode == UDMA_MODE_PER_SCATTER_GATHER))
        {
            dstAddr = (void *) &controlTable[channelStructIndex
                    | UDMA_ALT_SELECT].spare;
        }
        //
        // Not a scatter-gather transfer, calculate end pointer normally.
        //
        else
        {
            increment = increment >> 30;
            bufferBytes = transferSize << increment;
            dstAddr = (void *) ((uint32_t) dstAddr + bufferBytes - 1);
        }
    }

    //
    // Load the destination ending address into the control block.
    //
    controlTable[channelStructIndex].dstEndAddr = dstAddr;

    //
    // Write the new control word value.
    //
    controlTable[channelStructIndex].control = control;
}


void DMA_setChannelScatterGather(uint32_t channelNum, uint32_t taskCount,
        void *taskList, uint32_t isPeriphSG)
{
    DMA_ControlTable *controlTable;
    DMA_ControlTable *pTaskTable;

    //
    // Check the parameters
    //
    ASSERT((channelNum & 0xffff) < 8);
    ASSERT(DMA->rCTLBASE != 0);
    ASSERT(taskList != 0);
    ASSERT(taskCount <= 1024);
    ASSERT(taskCount != 0);

    //
    // In case a channel selector macro (like UDMA_CH0_USB0EP1RX) was
    // passed as the channelNum parameter, extract just the channel number
    // from this parameter.
    //
    channelNum &= 0x0F;

    //
    // Get the base address of the control table.
    //
    controlTable = (DMA_ControlTable *) DMA->rCTLBASE.r;

    //
    // Get a handy pointer to the task list
    //
    pTaskTable = (DMA_ControlTable *) taskList;

    //
    // Compute the ending address for the source pointer.  This address is the
    // last element of the last task in the task table
    //
    controlTable[channelNum].srcEndAddr = &pTaskTable[taskCount - 1].spare;

    //
    // Compute the ending address for the destination pointer.  This address
    // is the end of the alternate structure for this channel.
    //
    controlTable[channelNum].dstEndAddr = &controlTable[channelNum
            | UDMA_ALT_SELECT].spare;

    //
    // Compute the control word.  Most configurable items are fixed for
    // scatter-gather.  Item and increment sizes are all 32-bit and arb
    // size must be 4.  The count is the number of items in the task list
    // times 4 (4 words per task).
    //
    controlTable[channelNum].control = (UDMA_CHCTL_DSTINC_32
            | UDMA_CHCTL_DSTSIZE_32 | UDMA_CHCTL_SRCINC_32
            | UDMA_CHCTL_SRCSIZE_32 | UDMA_CHCTL_ARBSIZE_4
            | (((taskCount * 4) - 1) << UDMA_CHCTL_XFERSIZE_S)
            | (isPeriphSG ?
            UDMA_CHCTL_XFERMODE_PER_SG :
                            UDMA_CHCTL_XFERMODE_MEM_SG));

    //
    // Scatter-gather operations can leave the alt bit set.  So if doing
    // back to back scatter-gather transfers, the second attempt may not
    // work correctly because the alt bit is set.  Therefore, clear the
    // alt bit here to ensure that it is always cleared before a new SG
    // transfer is started.
    //
    DMA->rALTCLR = 1 << channelNum;
}


uint32_t DMA_getChannelSize(uint32_t channelStructIndex)
{
    DMA_ControlTable *controlTable;
    uint32_t control;

    //
    // Check the arguments.
    //
    ASSERT((channelStructIndex & 0xffff) < 16);
    ASSERT(DMA->rCTLBASE != 0);

    //
    // In case a channel selector macro (like UDMA_CH0_USB0EP1RX) was
    // passed as the channelStructIndex parameter, extract just the channel
    // index from this parameter.
    //
    channelStructIndex &= 0x3f;

    //
    // Get the base address of the control table.
    //
    controlTable = (DMA_ControlTable *) DMA->rCTLBASE.r;

    //
    // Get the current control word value and mask off all but the size field
    // and the mode field.
    //
    control = (controlTable[channelStructIndex].control
            & (UDMA_CHCTL_XFERSIZE_M | UDMA_CHCTL_XFERMODE_M));

    //
    // If the size field and mode field are 0 then the transfer is finished
    // and there are no more items to transfer
    //
    if (control == 0)
    {
        return (0);
    }

    //
    // Otherwise, if either the size field or more field is non-zero, then
    // not all the items have been transferred.
    //
    else
    {
        //
        // Shift the size field and add one, then return to user.
        //
        return ((control >> 4) + 1);
    }
}


uint32_t DMA_getChannelMode(uint32_t channelStructIndex)
{
    DMA_ControlTable *controlTable;
    uint32_t control;

    //
    // Check the arguments.
    //
    ASSERT((channelStructIndex & 0xffff) < 64);
    ASSERT(DMA->rCTLBASE != 0);

    //
    // In case a channel selector macro (like UDMA_CH0_USB0EP1RX) was
    // passed as the channelStructIndex parameter, extract just the channel
    // index from this parameter.
    //
    channelStructIndex &= 0x3f;

    //
    // Get the base address of the control table.
    //
    controlTable = (DMA_ControlTable *) DMA->rCTLBASE.r;

    //
    // Get the current control word value and mask off all but the mode field.
    //
    control =
            (controlTable[channelStructIndex].control & UDMA_CHCTL_XFERMODE_M);

    //
    // Check if scatter/gather mode, and if so, mask off the alt bit.
    //
    if (((control & ~UDMA_MODE_ALT_SELECT) == UDMA_MODE_MEM_SCATTER_GATHER)
            || ((control & ~UDMA_MODE_ALT_SELECT)
                    == UDMA_MODE_PER_SCATTER_GATHER))
    {
        control &= ~UDMA_MODE_ALT_SELECT;
    }

    //
    // Return the mode to the caller.
    //
    return (control);
}


void DMA_assignChannel(uint32_t mapping)
{
    switch (mapping)
    {
    case DMA_CH0_RESERVED0:
    case DMA_CH0_EUSCIA0TX:
    case DMA_CH0_EUSCIB0TX0:
    case DMA_CH0_EUSCIB3TX1:
    case DMA_CH0_EUSCIB2TX2:
    case DMA_CH0_EUSCIB1TX3:
    case DMA_CH0_TIMERA0CCR0:
    case DMA_CH0_AESTRIGGER0:
        DMA->rCH0_SRCCFG.r = (mapping >> 24) & 0x1F;
        break;
    case DMA_CH1_RESERVED0:
    case DMA_CH1_EUSCIA0RX:
    case DMA_CH1_EUSCIB0RX0:
    case DMA_CH1_EUSCIB3RX1:
    case DMA_CH1_EUSCIB2RX2:
    case DMA_CH1_EUSCIB1RX3:
    case DMA_CH1_TIMERA0CCR2:
    case DMA_CH1_AESTRIGGER1:
        DMA->rCH1_SRCCFG.r = (mapping >> 24) & 0x1F;
        break;
    case DMA_CH2_RESERVED0:
    case DMA_CH2_EUSCIA1TX:
    case DMA_CH2_EUSCIB1TX0:
    case DMA_CH2_EUSCIB0TX1:
    case DMA_CH2_EUSCIB3TX2:
    case DMA_CH2_EUSCIB2TX3:
    case DMA_CH2_TIMERA1CCR0:
    case DMA_CH2_AESTRIGGER2:
        DMA->rCH2_SRCCFG.r = (mapping >> 24) & 0x1F;
        break;
    case DMA_CH3_RESERVED0:
    case DMA_CH3_EUSCIA1RX:
    case DMA_CH3_EUSCIB1RX0:
    case DMA_CH3_EUSCIB0RX1:
    case DMA_CH3_EUSCIB3RX2:
    case DMA_CH3_EUSCIB2RX3:
    case DMA_CH3_TIMERA1CCR2:
    case DMA_CH3_RESERVED1:
        DMA->rCH3_SRCCFG.r = (mapping >> 24) & 0x1F;
        break;
    case DMA_CH4_RESERVED0:
    case DMA_CH4_EUSCIA2TX:
    case DMA_CH4_EUSCIB2TX0:
    case DMA_CH4_EUSCIB1TX1:
    case DMA_CH4_EUSCIB0TX2:
    case DMA_CH4_EUSCIB3TX3:
    case DMA_CH4_TIMERA2CCR0:
    case DMA_CH4_RESERVED1:
        DMA->rCH4_SRCCFG.r = (mapping >> 24) & 0x1F;
        break;
    case DMA_CH5_RESERVED0:
    case DMA_CH5_EUSCIA2RX:
    case DMA_CH5_EUSCIB2RX0:
    case DMA_CH5_EUSCIB1RX1:
    case DMA_CH5_EUSCIB0RX2:
    case DMA_CH5_EUSCIB3RX3:
    case DMA_CH5_TIMERA2CCR2:
    case DMA_CH5_RESERVED1:
        DMA->rCH5_SRCCFG.r = (mapping >> 24) & 0x1F;
        break;
    case DMA_CH6_RESERVED0:
    case DMA_CH6_EUSCIA3TX:
    case DMA_CH6_EUSCIB3TX0:
    case DMA_CH6_EUSCIB2TX1:
    case DMA_CH6_EUSCIB1TX2:
    case DMA_CH6_EUSCIB0TX3:
    case DMA_CH6_TIMERA3CCR0:
    case DMA_CH6_EXTERNALPIN:
        DMA->rCH6_SRCCFG.r = (mapping >> 24) & 0x1F;
        break;
    case DMA_CH7_RESERVED0:
    case DMA_CH7_EUSCIA3RX:
    case DMA_CH7_EUSCIB3RX0:
    case DMA_CH7_EUSCIB2RX1:
    case DMA_CH7_EUSCIB1RX2:
    case DMA_CH7_EUSCIB0RX3:
    case DMA_CH7_TIMERA3CCR2:
    case DMA_CH7_ADC12C:
        DMA->rCH7_SRCCFG.r = (mapping >> 24) & 0x1F;
        break;
    default:
        ASSERT(false);
    }

}


void DMA_requestSoftwareTransfer(uint32_t channel)
{
    DMA->rSW_CHTRIG.r |= (1 << channel);
}


void DMA_assignInterrupt(uint32_t interruptNumber, uint32_t channel)
{
    ASSERT(
            interruptNumber == DMA_INT1 || interruptNumber == DMA_INT2
            || interruptNumber == DMA_INT3);

    if (interruptNumber == DMA_INT1)
    {
        DMA->rINT1_SRCCFG.r = (DMA->rINT1_SRCCFG.r & ~DMA_INT1_SRCCFG_INT_SRC_M)
                | channel;
    } else if (interruptNumber == DMA_INT2)
    {
        DMA->rINT2_SRCCFG.r = (DMA->rINT2_SRCCFG.r & ~DMA_INT1_SRCCFG_INT_SRC_M)
                | channel;
    } else if (interruptNumber == DMA_INT3)
    {
        DMA->rINT3_SRCCFG.r = (DMA->rINT3_SRCCFG.r & ~DMA_INT1_SRCCFG_INT_SRC_M)
                | channel;
    }

    /* Enabling the assigned interrupt */
    DMA_enableInterrupt(interruptNumber);
}


void DMA_enableInterrupt(uint32_t interruptNumber)
{
    ASSERT(
            (interruptNumber == DMA_INT0) || (interruptNumber == DMA_INT1)
            || (interruptNumber == DMA_INT2)
            || (interruptNumber == DMA_INT3));

    if (interruptNumber == DMA_INT1)
    {
        DMA->rINT1_SRCCFG.r |= DMA_INT1_SRCCFG_EN;
    } else if (interruptNumber == DMA_INT2)
    {
        DMA->rINT2_SRCCFG.r |= DMA_INT2_SRCCFG_EN;
    } else if (interruptNumber == DMA_INT3)
    {
        DMA->rINT3_SRCCFG.r |= DMA_INT3_SRCCFG_EN;
    }

}


void DMA_disableInterrupt(uint32_t interruptNumber)
{
    ASSERT(
            (interruptNumber == DMA_INT0) || (interruptNumber == DMA_INT1)
            || (interruptNumber == DMA_INT2)
            || (interruptNumber == DMA_INT3));

    if (interruptNumber == DMA_INT1)
    {
        DMA->rINT1_SRCCFG.r &= ~DMA_INT1_SRCCFG_EN;
    } else if (interruptNumber == DMA_INT2)
    {
        DMA->rINT2_SRCCFG.r &= ~DMA_INT2_SRCCFG_EN;
    } else if (interruptNumber == DMA_INT3)
    {
        DMA->rINT3_SRCCFG.r &= ~DMA_INT3_SRCCFG_EN;
    }
}


uint32_t DMA_getInterruptStatus(void)
{
    return DMA->rINT0_SRCFLG.r;
}


void DMA_clearInterruptFlag(uint32_t channel)
{
    DMA->rINT0_CLRFLG.r |= (1 << channel);
}


static uint32_t getUserFlashSector(uint32_t addr)
{
    if (addr > 0x1ffff)
    {
        addr = addr - 0x20000;
    }

    switch (addr)
    {
    case 0:
        return FLASH_SECTOR0;
    case 0x1000:
        return FLASH_SECTOR1;
    case 0x2000:
        return FLASH_SECTOR2;
    case 0x3000:
        return FLASH_SECTOR3;
    case 0x4000:
        return FLASH_SECTOR4;
    case 0x5000:
        return FLASH_SECTOR5;
    case 0x6000:
        return FLASH_SECTOR6;
    case 0x7000:
        return FLASH_SECTOR7;
    case 0x8000:
        return FLASH_SECTOR8;
    case 0x9000:
        return FLASH_SECTOR9;
    case 0xA000:
        return FLASH_SECTOR10;
    case 0xB000:
        return FLASH_SECTOR11;
    case 0xC000:
        return FLASH_SECTOR12;
    case 0xD000:
        return FLASH_SECTOR13;
    case 0xE000:
        return FLASH_SECTOR14;
    case 0xF000:
        return FLASH_SECTOR15;
    case 0x10000:
        return FLASH_SECTOR16;
    case 0x11000:
        return FLASH_SECTOR17;
    case 0x12000:
        return FLASH_SECTOR18;
    case 0x13000:
        return FLASH_SECTOR19;
    case 0x14000:
        return FLASH_SECTOR20;
    case 0x15000:
        return FLASH_SECTOR21;
    case 0x16000:
        return FLASH_SECTOR22;
    case 0x17000:
        return FLASH_SECTOR23;
    case 0x18000:
        return FLASH_SECTOR24;
    case 0x19000:
        return FLASH_SECTOR25;
    case 0x1A000:
        return FLASH_SECTOR26;
    case 0x1B000:
        return FLASH_SECTOR27;
    case 0x1C000:
        return FLASH_SECTOR28;
    case 0x1D000:
        return FLASH_SECTOR29;
    case 0x1E000:
        return FLASH_SECTOR30;
    case 0x1F000:
        return FLASH_SECTOR31;
    default:
        ASSERT(false);
        return 0;
    }
}


static const uint32_t __getBurstProgramRegs[16] =
{ OFS_FLCTL_PRGBRST_DATA0_0, OFS_FLCTL_PRGBRST_DATA0_1,
OFS_FLCTL_PRGBRST_DATA0_2, OFS_FLCTL_PRGBRST_DATA0_3,
OFS_FLCTL_PRGBRST_DATA1_0, OFS_FLCTL_PRGBRST_DATA1_1,
OFS_FLCTL_PRGBRST_DATA1_2, OFS_FLCTL_PRGBRST_DATA1_3,
OFS_FLCTL_PRGBRST_DATA2_0, OFS_FLCTL_PRGBRST_DATA2_1,
OFS_FLCTL_PRGBRST_DATA2_2, OFS_FLCTL_PRGBRST_DATA2_3,
OFS_FLCTL_PRGBRST_DATA3_0, OFS_FLCTL_PRGBRST_DATA3_1,
OFS_FLCTL_PRGBRST_DATA3_2, OFS_FLCTL_PRGBRST_DATA3_3 };


uint8_t __FlashCtl_remaskData8Post(uint8_t data, uint32_t addr)
{
    uint32_t readMode, waitState, bankProgram, bankOneStart;

    /* Changing the waitstate and read mode of whichever bank we are in */
    /* Finding out which bank we are in */
    if(addr >  SysCtl_getFlashSize())
    {
        bankOneStart = __INFO_FLASH_TECH_MIDDLE__;
    }
    else
    {
        bankOneStart = SysCtl_getFlashSize() / 2;
    }

    bankProgram =
            addr < (bankOneStart) ? FLASH_BANK0 : FLASH_BANK1;

    /* Saving the current wait states and read mode */
    waitState = FlashCtl_getWaitState(bankProgram);
    readMode = FlashCtl_getReadMode(bankProgram);

    /* Setting the wait state to account for the mode */
    FlashCtl_setWaitState(bankProgram, (2 * waitState) + 1);

    /* Changing to PROGRAM VERIFY mode */
    FlashCtl_setReadMode(bankProgram, FLASH_PROGRAM_VERIFY_READ_MODE);

    data = ~(~(data) & HWREG8(addr));

    /* Setting the wait state to account for the mode */
    FlashCtl_setReadMode(bankProgram, readMode);
    FlashCtl_setWaitState(bankProgram, waitState);

    return data;
}


uint8_t __FlashCtl_remaskData8Pre(uint8_t data, uint32_t addr)
{
    uint32_t readMode, waitState, bankProgram, bankOneStart;

    /* Changing the waitstate and read mode of whichever bank we are in */
    /* Finding out which bank we are in */
    if(addr >  SysCtl_getFlashSize())
    {
        bankOneStart = __INFO_FLASH_TECH_MIDDLE__;
    }
    else
    {
        bankOneStart = SysCtl_getFlashSize() / 2;
    }

    bankProgram =
            addr < (bankOneStart) ? FLASH_BANK0 : FLASH_BANK1;

    /* Saving the current wait states and read mode */
    waitState = FlashCtl_getWaitState(bankProgram);
    readMode = FlashCtl_getReadMode(bankProgram);

    /* Setting the wait state to account for the mode */
    FlashCtl_setWaitState(bankProgram, (2 * waitState) + 1);

    /* Changing to PROGRAM VERIFY mode */
    FlashCtl_setReadMode(bankProgram, FLASH_PROGRAM_VERIFY_READ_MODE);

    data |= ~(HWREG8(addr) | data);

    /* Setting the wait state to account for the mode */
    FlashCtl_setReadMode(bankProgram, readMode);
    FlashCtl_setWaitState(bankProgram, waitState);

    return data;
}


uint32_t __FlashCtl_remaskData32Post(uint32_t data, uint32_t addr)
{
    uint32_t bankProgramStart, bankProgramEnd, bank1Start;
    uint32_t b0WaitState, b0ReadMode, b1WaitState, b1ReadMode;

    /* Changing the waitstate and read mode of whichever bank we are in */
    /* Finding out which bank we are in */
    if(addr >  SysCtl_getFlashSize())
    {
        bank1Start = __INFO_FLASH_TECH_MIDDLE__;
    }
    else
    {
        bank1Start = SysCtl_getFlashSize() / 2;
    }

    bankProgramStart = addr < bank1Start ? FLASH_BANK0 : FLASH_BANK1;
    bankProgramEnd = (addr + 4) < bank1Start ? FLASH_BANK0 : FLASH_BANK1;

    /* Saving the current wait states and read mode */
    b0WaitState = FlashCtl_getWaitState(bankProgramStart);
    b0ReadMode = FlashCtl_getReadMode(bankProgramStart);
    FlashCtl_setWaitState(bankProgramStart, (2 * b0WaitState) + 1);
    FlashCtl_setReadMode(bankProgramStart, FLASH_PROGRAM_VERIFY_READ_MODE);

    if (bankProgramStart != bankProgramEnd)
    {
        b1WaitState = FlashCtl_getWaitState(bankProgramEnd);
        b1ReadMode = FlashCtl_getReadMode(bankProgramEnd);
        FlashCtl_setWaitState(bankProgramEnd, (2 * b1WaitState) + 1);
        FlashCtl_setReadMode(bankProgramEnd, FLASH_PROGRAM_VERIFY_READ_MODE);
    }

    data = ~(~(data) & HWREG32(addr));

    /* Setting the wait state to account for the mode */
    FlashCtl_setReadMode(bankProgramStart, b0ReadMode);
    FlashCtl_setWaitState(bankProgramStart, b0WaitState);

    if (bankProgramStart != bankProgramEnd)
    {
        FlashCtl_setReadMode(bankProgramEnd, b1ReadMode);
        FlashCtl_setWaitState(bankProgramEnd, b1WaitState);
    }

    return data;
}


uint32_t __FlashCtl_remaskData32Pre(uint32_t data, uint32_t addr)
{
    uint32_t bankProgramStart, bankProgramEnd, bank1Start;
    uint32_t b0WaitState, b0ReadMode, b1WaitState, b1ReadMode;

    /* Changing the waitstate and read mode of whichever bank we are in */
    /* Finding out which bank we are in */
    if(addr >  SysCtl_getFlashSize())
    {
        bank1Start = __INFO_FLASH_TECH_MIDDLE__;
    }
    else
    {
        bank1Start = SysCtl_getFlashSize() / 2;
    }

    bankProgramStart = addr < bank1Start ? FLASH_BANK0 : FLASH_BANK1;
    bankProgramEnd = (addr + 4) < bank1Start ? FLASH_BANK0 : FLASH_BANK1;

    /* Saving the current wait states and read mode */
    b0WaitState = FlashCtl_getWaitState(bankProgramStart);
    b0ReadMode = FlashCtl_getReadMode(bankProgramStart);
    FlashCtl_setWaitState(bankProgramStart, (2 * b0WaitState) + 1);
    FlashCtl_setReadMode(bankProgramStart, FLASH_PROGRAM_VERIFY_READ_MODE);

    if (bankProgramStart != bankProgramEnd)
    {
        b1WaitState = FlashCtl_getWaitState(bankProgramEnd);
        b1ReadMode = FlashCtl_getReadMode(bankProgramEnd);
        FlashCtl_setWaitState(bankProgramEnd, (2 * b1WaitState) + 1);
        FlashCtl_setReadMode(bankProgramEnd, FLASH_PROGRAM_VERIFY_READ_MODE);
    }

    data |= ~(HWREG32(addr) | data);

    /* Setting the wait state to account for the mode */
    FlashCtl_setReadMode(bankProgramStart, b0ReadMode);
    FlashCtl_setWaitState(bankProgramStart, b0WaitState);

    if (bankProgramStart != bankProgramEnd)
    {
        FlashCtl_setReadMode(bankProgramEnd, b1ReadMode);
        FlashCtl_setWaitState(bankProgramEnd, b1WaitState);
    }

    return data;
}


void __FlashCtl_remaskBurstDataPost(uint32_t addr, uint32_t size)
{
    uint32_t bankProgramStart, bankProgramEnd, bank1Start, ii;
    uint32_t b0WaitState, b0ReadMode, b1WaitState, b1ReadMode;

    /* Waiting for idle status */
    while ((FLCTL->rPRGBRST_CTLSTAT.r & FLCTL_PRGBRST_CTLSTAT_BURST_STATUS_M)
            != FLCTL_PRGBRST_CTLSTAT_BURST_STATUS_0)
    {
        BITBAND_PERI(FLCTL->rPRGBRST_CTLSTAT.r,
                FLCTL_PRGBRST_CTLSTAT_CLR_STAT_OFS) = 1;
    }

    /* Changing the waitstate and read mode of whichever bank we are in */
    /* Finding out which bank we are in */
    if(addr >  SysCtl_getFlashSize())
    {
        bank1Start = __INFO_FLASH_TECH_MIDDLE__;
    }
    else
    {
        bank1Start = SysCtl_getFlashSize() / 2;
    }

    bankProgramStart = addr < bank1Start ? FLASH_BANK0 : FLASH_BANK1;
    bankProgramEnd = (addr + size) < bank1Start ? FLASH_BANK0 : FLASH_BANK1;

    /* Saving the current wait states and read mode */
    b0WaitState = FlashCtl_getWaitState(bankProgramStart);
    b0ReadMode = FlashCtl_getReadMode(bankProgramStart);
    FlashCtl_setWaitState(bankProgramStart, (2 * b0WaitState) + 1);
    FlashCtl_setReadMode(bankProgramStart, FLASH_PROGRAM_VERIFY_READ_MODE);

    if (bankProgramStart != bankProgramEnd)
    {
        b1WaitState = FlashCtl_getWaitState(bankProgramEnd);
        b1ReadMode = FlashCtl_getReadMode(bankProgramEnd);
        FlashCtl_setWaitState(bankProgramEnd, (2 * b1WaitState) + 1);
        FlashCtl_setReadMode(bankProgramEnd, FLASH_PROGRAM_VERIFY_READ_MODE);
    }

    /* Going through each BURST program register and masking out for post
     * verifcation if needed
     */
    size = (size / 4);
    for (ii = 0; ii < size; ii++)
    {
        HWREG32(FLCTL_BASE + __getBurstProgramRegs[ii]) = ~(~(HWREG32(
                FLCTL_BASE + __getBurstProgramRegs[ii])) & HWREG32(addr));

        addr += 4;
    }

    /* Setting the wait state to account for the mode */
    FlashCtl_setReadMode(bankProgramStart, b0ReadMode);
    FlashCtl_setWaitState(bankProgramStart, b0WaitState);

    if (bankProgramStart != bankProgramEnd)
    {
        FlashCtl_setReadMode(bankProgramEnd, b1ReadMode);
        FlashCtl_setWaitState(bankProgramEnd, b1WaitState);
    }
}


void __FlashCtl_remaskBurstDataPre(uint32_t addr, uint32_t size)
{

    uint32_t bankProgramStart, bankProgramEnd, bank1Start, ii;
    uint32_t b0WaitState, b0ReadMode, b1WaitState, b1ReadMode;

    /* Waiting for idle status */
    while ((FLCTL->rPRGBRST_CTLSTAT.r & FLCTL_PRGBRST_CTLSTAT_BURST_STATUS_M)
            != FLCTL_PRGBRST_CTLSTAT_BURST_STATUS_0)
    {
        BITBAND_PERI(FLCTL->rPRGBRST_CTLSTAT.r,
                FLCTL_PRGBRST_CTLSTAT_CLR_STAT_OFS) = 1;
    }

    /* Changing the waitstate and read mode of whichever bank we are in */
    /* Finding out which bank we are in */
    if(addr >  SysCtl_getFlashSize())
    {
        bank1Start = __INFO_FLASH_TECH_MIDDLE__;
    }
    else
    {
        bank1Start = SysCtl_getFlashSize() / 2;
    }

    bankProgramStart = addr < bank1Start ? FLASH_BANK0 : FLASH_BANK1;
    bankProgramEnd = (addr + size) < bank1Start ? FLASH_BANK0 : FLASH_BANK1;

    /* Saving the current wait states and read mode */
    b0WaitState = FlashCtl_getWaitState(bankProgramStart);
    b0ReadMode = FlashCtl_getReadMode(bankProgramStart);
    FlashCtl_setWaitState(bankProgramStart, (2 * b0WaitState) + 1);
    FlashCtl_setReadMode(bankProgramStart, FLASH_PROGRAM_VERIFY_READ_MODE);

    if (bankProgramStart != bankProgramEnd)
    {
        b1WaitState = FlashCtl_getWaitState(bankProgramEnd);
        b1ReadMode = FlashCtl_getReadMode(bankProgramEnd);
        FlashCtl_setWaitState(bankProgramEnd, (2 * b1WaitState) + 1);
        FlashCtl_setReadMode(bankProgramEnd, FLASH_PROGRAM_VERIFY_READ_MODE);
    }

    /* Going through each BURST program register and masking out for pre
     * verifcation
     */
    size = (size / 4);
    for (ii = 0; ii < size; ii++)
    {
        HWREG32(FLCTL_BASE + __getBurstProgramRegs[ii]) |=
                ~(HWREG32(FLCTL_BASE + __getBurstProgramRegs[ii])
                        | HWREG32(addr));
        addr += 4;
    }

    /* Setting the wait state to account for the mode */
    FlashCtl_setReadMode(bankProgramStart, b0ReadMode);
    FlashCtl_setWaitState(bankProgramStart, b0WaitState);

    if (bankProgramStart != bankProgramEnd)
    {
        FlashCtl_setReadMode(bankProgramEnd, b1ReadMode);
        FlashCtl_setWaitState(bankProgramEnd, b1WaitState);
    }

}


static bool _FlashCtl_Program8(uint32_t src, uint32_t dest, uint32_t mTries)
{
    uint32_t ii;
    uint8_t data;

    /* Enabling the correct verification settings  */
    FlashCtl_setProgramVerification(FLASH_REGPRE | FLASH_REGPOST);
    FlashCtl_clearProgramVerification(FLASH_BURSTPOST | FLASH_BURSTPRE);

    data = HWREG8(src);

    for (ii = 0; ii < mTries; ii++)
    {
        /* Clearing flags */
        FLCTL->rCLRIFG.r |= (FLASH_PROGRAM_ERROR | FLASH_POSTVERIFY_FAILED
                | FLASH_PREVERIFY_FAILED | FLASH_WRDPRGM_COMPLETE);

        HWREG8(dest) = data;

        while (!(FlashCtl_getInterruptStatus() & FLASH_WRDPRGM_COMPLETE))
        {
            __no_operation();
        }

        /* Pre-Verify */
        if ((BITBAND_PERI(FLCTL->rPRG_CTLSTAT.r, FLCTL_PRG_CTLSTAT_VER_PRE_OFS)
                && BITBAND_PERI(FLCTL->rIFG.r, FLCTL_IFG_AVPRE_OFS)))
        {
            data = __FlashCtl_remaskData8Pre(data, dest);

            if (data != 0xFF)
            {
                FlashCtl_clearProgramVerification(FLASH_REGPRE);
                continue;
            }

        }

        /* Post Verify */
        if ((BITBAND_PERI(FLCTL->rIFG.r, FLCTL_IFG_AVPST_OFS)))
        {
            data = __FlashCtl_remaskData8Post(data, dest);

            /* Seeing if we actually need to do another pulse */
            if (data == 0xFF)
                return true;

            FlashCtl_setProgramVerification(FLASH_REGPRE | FLASH_REGPOST);
            continue;
        }

        /* If we got this far, return true */
        return true;

    }

    return false;

}


static bool _FlashCtl_Program32(uint32_t src, uint32_t dest, uint32_t mTries)
{
    uint32_t ii;
    uint32_t data;

    /* Enabling the correct verification settings  */
    FlashCtl_setProgramVerification(FLASH_REGPRE | FLASH_REGPOST);
    FlashCtl_clearProgramVerification(FLASH_BURSTPOST | FLASH_BURSTPRE);

    data = HWREG32(src);

    for (ii = 0; ii < mTries; ii++)
    {
        /* Clearing flags */
        FLCTL->rCLRIFG.r |= (FLASH_PROGRAM_ERROR | FLASH_POSTVERIFY_FAILED
                | FLASH_PREVERIFY_FAILED | FLASH_WRDPRGM_COMPLETE);

        HWREG32(dest) = data;

        while (!(FlashCtl_getInterruptStatus() & FLASH_WRDPRGM_COMPLETE))
        {
            __no_operation();
        }

        /* Pre-Verify */
        if ((BITBAND_PERI(FLCTL->rPRG_CTLSTAT.r, FLCTL_PRG_CTLSTAT_VER_PRE_OFS)
                && BITBAND_PERI(FLCTL->rIFG.r, FLCTL_IFG_AVPRE_OFS)))
        {
            data = __FlashCtl_remaskData32Pre(data, dest);

            if (data != 0xFFFFFFFF)
            {

                FlashCtl_clearProgramVerification(FLASH_REGPRE);
                continue;
            }

        }

        /* Post Verify */
        if ((BITBAND_PERI(FLCTL->rIFG.r, FLCTL_IFG_AVPST_OFS)))
        {
            data = __FlashCtl_remaskData32Post(data, dest);

            /* Seeing if we actually need to do another pulse */
            if (data == 0xFFFFFFFF)
                return true;

            FlashCtl_setProgramVerification(FLASH_REGPRE | FLASH_REGPOST);
            continue;
        }

        /* If we got this far, return true */
        return true;

    }

    return false;

}


static bool _FlashCtl_ProgramBurst(uint32_t src, uint32_t dest, uint32_t length,
        uint32_t mTries)
{
    uint32_t bCalc, otpOffset, ii, jj;
    bool res;

    /* Setting verification */
    FlashCtl_clearProgramVerification(FLASH_REGPRE | FLASH_REGPOST);
    FlashCtl_setProgramVerification(FLASH_BURSTPOST | FLASH_BURSTPRE);

    /* Assume Failure */
    res = false;

    /* Waiting for idle status */
    while ((FLCTL->rPRGBRST_CTLSTAT.r & FLCTL_PRGBRST_CTLSTAT_BURST_STATUS_M)
            != FLCTL_PRGBRST_CTLSTAT_BURST_STATUS_0)
    {
        BITBAND_PERI(FLCTL->rPRGBRST_CTLSTAT.r,
                FLCTL_PRGBRST_CTLSTAT_CLR_STAT_OFS) = 1;
    }

    /* Setting/clearing INFO flash flags as appropriate */
    if (dest > __MAIN_MEMORY_END__)
    {
        FLCTL->rPRGBRST_CTLSTAT.r = (FLCTL->rPRGBRST_CTLSTAT.r
                & ~FLCTL_PRGBRST_CTLSTAT_TYPE_M) | FLCTL_PRGBRST_CTLSTAT_TYPE_1;
        otpOffset = __INFO_FLASH_TECH_START__;
    } else
    {
        FLCTL->rPRGBRST_CTLSTAT.r = (FLCTL->rPRGBRST_CTLSTAT.r
                & ~FLCTL_PRGBRST_CTLSTAT_TYPE_M) | FLCTL_PRGBRST_CTLSTAT_TYPE_0;
        otpOffset = __MAIN_MEMORY_START__;
    }

    bCalc = 0;
    FLCTL->rPRGBRST_STARTADDR.r = (dest - otpOffset);

    /* Initially populating the burst registers */
    while (bCalc < 16 && length != 0)
    {
        HWREG32(FLCTL_BASE + __getBurstProgramRegs[bCalc]) = HWREG32(src);
        bCalc++;
        length -= 4;
        src += 4;
    }

    for (ii = 0; ii < mTries; ii++)
    {
        /* Clearing Flags */
        FLCTL->rCLRIFG.r |= (FLASH_BRSTPRGM_COMPLETE | FLASH_POSTVERIFY_FAILED
                | FLASH_PREVERIFY_FAILED);

        /* Waiting for idle status */
        while ((FLCTL->rPRGBRST_CTLSTAT.r & FLCTL_PRGBRST_CTLSTAT_BURST_STATUS_M)
                != FLCTL_PRGBRST_CTLSTAT_BURST_STATUS_0)
        {
            BITBAND_PERI(FLCTL->rPRGBRST_CTLSTAT.r,
                    FLCTL_PRGBRST_CTLSTAT_CLR_STAT_OFS) = 1;
        }

        /* Start the burst program */
        FLCTL->rPRGBRST_CTLSTAT.r = (FLCTL->rPRGBRST_CTLSTAT.r
                & ~(FLCTL_PRGBRST_CTLSTAT_LEN_M))
                | ((bCalc / 4) << FLASH_BURST_PRG_BIT)
                | FLCTL_PRGBRST_CTLSTAT_START;

        /* Waiting for the burst to complete */
        while ((FLCTL->rPRGBRST_CTLSTAT.r &
        FLCTL_PRGBRST_CTLSTAT_BURST_STATUS_M)
                != FLASH_PRGBRSTCTLSTAT_BURSTSTATUS_COMPLETE)
        {
            __no_operation();
        }

        /* Checking for errors and clearing/masking */

        /* Address Error */
        if (BITBAND_PERI(FLCTL->rPRGBRST_CTLSTAT.r,
                FLCTL_PRGBRST_CTLSTAT_ADDR_ERR_OFS))
        {
            goto BurstCleanUp;
        }

        /* Pre-Verify Error */
        if (BITBAND_PERI(FLCTL->rPRGBRST_CTLSTAT.r,
                FLCTL_PRGBRST_CTLSTAT_AUTO_PRE_OFS) && BITBAND_PERI(FLCTL->rPRGBRST_CTLSTAT.r,
                        FLCTL_PRGBRST_CTLSTAT_PRE_ERR_OFS))
        {
            __FlashCtl_remaskBurstDataPre(dest, bCalc * 4);

            for (jj = 0; jj < bCalc; jj++)
            {
                if (HWREG32(FLCTL_BASE + __getBurstProgramRegs[jj])
                        != 0xFFFFFFFF)
                {
                    FlashCtl_clearProgramVerification(FLASH_BURSTPRE);
                    break;
                }
            }

            if (jj != bCalc)
                continue;
        }

        /* Post-Verify Error */
        if (BITBAND_PERI(FLCTL->rPRGBRST_CTLSTAT.r,
                FLCTL_PRGBRST_CTLSTAT_PST_ERR_OFS))
        {
            __FlashCtl_remaskBurstDataPost(dest, bCalc * 4);

            for (jj = 0; jj < bCalc; jj++)
            {
                if ((HWREG32(FLCTL_BASE + __getBurstProgramRegs[jj]))
                        != 0xFFFFFFFF)
                {
                    FlashCtl_setProgramVerification(
                            FLASH_BURSTPOST | FLASH_BURSTPRE);
                    break;
                }
            }

            if (jj != bCalc)
                continue;

        }

        /* If we got this far, the program happened */
        res = true;
        goto BurstCleanUp;
    }

    BurstCleanUp:
    /* Waiting for idle status */
    while ((FLCTL->rPRGBRST_CTLSTAT.r & FLCTL_PRGBRST_CTLSTAT_BURST_STATUS_M)
            != FLCTL_PRGBRST_CTLSTAT_BURST_STATUS_0)
    {
        BITBAND_PERI(FLCTL->rPRGBRST_CTLSTAT.r,
                FLCTL_PRGBRST_CTLSTAT_CLR_STAT_OFS) = 1;
    }
    return res;
}


void FlashCtl_enableReadBuffering(uint_fast8_t memoryBank,
        uint_fast8_t accessMethod)
{
    if (memoryBank == FLASH_BANK0 && accessMethod == FLASH_DATA_READ)
        BITBAND_PERI(FLCTL->rBANK0_RDCTL.r, FLCTL_BANK0_RDCTL_BUFD_OFS) = 1;
    else if (memoryBank == FLASH_BANK1 && accessMethod == FLASH_DATA_READ)
        BITBAND_PERI(FLCTL->rBANK1_RDCTL.r, FLCTL_BANK1_RDCTL_BUFD_OFS) = 1;
    else if (memoryBank == FLASH_BANK0
            && accessMethod == FLASH_INSTRUCTION_FETCH)
        BITBAND_PERI(FLCTL->rBANK0_RDCTL.r, FLCTL_BANK0_RDCTL_BUFI_OFS) = 1;
    else if (memoryBank == FLASH_BANK1
            && accessMethod == FLASH_INSTRUCTION_FETCH)
        BITBAND_PERI(FLCTL->rBANK1_RDCTL.r, FLCTL_BANK1_RDCTL_BUFI_OFS) = 1;
    else
        ASSERT(false);
}


void FlashCtl_disableReadBuffering(uint_fast8_t memoryBank,
        uint_fast8_t accessMethod)
{
    if (memoryBank == FLASH_BANK0 && accessMethod == FLASH_DATA_READ)
        BITBAND_PERI(FLCTL->rBANK0_RDCTL.r, FLCTL_BANK0_RDCTL_BUFD_OFS) = 0;
    else if (memoryBank == FLASH_BANK1 && accessMethod == FLASH_DATA_READ)
        BITBAND_PERI(FLCTL->rBANK1_RDCTL.r, FLCTL_BANK1_RDCTL_BUFD_OFS) = 0;
    else if (memoryBank == FLASH_BANK0
            && accessMethod == FLASH_INSTRUCTION_FETCH)
        BITBAND_PERI(FLCTL->rBANK0_RDCTL.r, FLCTL_BANK0_RDCTL_BUFI_OFS) = 0;
    else if (memoryBank == FLASH_BANK1
            && accessMethod == FLASH_INSTRUCTION_FETCH)
        BITBAND_PERI(FLCTL->rBANK1_RDCTL.r, FLCTL_BANK1_RDCTL_BUFI_OFS) = 0;
    else
        ASSERT(false);
}


bool FlashCtl_unprotectSector(uint_fast8_t memorySpace, uint32_t sectorMask)
{
    switch (memorySpace)
    {
    case FLASH_MAIN_MEMORY_SPACE_BANK0:
        FLCTL->rBANK0_MAIN_WEPROT.r &= ~sectorMask;
        break;
    case FLASH_MAIN_MEMORY_SPACE_BANK1:
        FLCTL->rBANK1_MAIN_WEPROT.r &= ~sectorMask;
        break;
    case FLASH_INFO_MEMORY_SPACE_BANK0:
        ASSERT(sectorMask <= 0x04);
        FLCTL->rBANK0_INFO_WEPROT.r &= ~sectorMask;
        break;
    case FLASH_INFO_MEMORY_SPACE_BANK1:
        ASSERT(sectorMask <= 0x04);
        FLCTL->rBANK1_INFO_WEPROT.r &= ~sectorMask;
        break;

    default:
        ASSERT(false);

    }

    return !FlashCtl_isSectorProtected(memorySpace, sectorMask);
}


bool FlashCtl_protectSector(uint_fast8_t memorySpace, uint32_t sectorMask)
{
    switch (memorySpace)
    {
    case FLASH_MAIN_MEMORY_SPACE_BANK0:
        FLCTL->rBANK0_MAIN_WEPROT.r |= sectorMask;
        break;
    case FLASH_MAIN_MEMORY_SPACE_BANK1:
        FLCTL->rBANK1_MAIN_WEPROT.r |= sectorMask;
        break;
    case FLASH_INFO_MEMORY_SPACE_BANK0:
        ASSERT(sectorMask <= 0x04);
        FLCTL->rBANK0_INFO_WEPROT.r |= sectorMask;
        break;
    case FLASH_INFO_MEMORY_SPACE_BANK1:
        ASSERT(sectorMask <= 0x04);
        FLCTL->rBANK1_INFO_WEPROT.r |= sectorMask;
        break;

    default:
        ASSERT(false);

    }

    return FlashCtl_isSectorProtected(memorySpace, sectorMask);
}


bool FlashCtl_isSectorProtected(uint_fast8_t memorySpace, uint32_t sector)
{
    switch (memorySpace)
    {
    case FLASH_MAIN_MEMORY_SPACE_BANK0:
        return FLCTL->rBANK0_MAIN_WEPROT.r & sector;
    case FLASH_MAIN_MEMORY_SPACE_BANK1:
        return FLCTL->rBANK1_MAIN_WEPROT.r & sector;
    case FLASH_INFO_MEMORY_SPACE_BANK0:
        ASSERT(sector <= 0x04);
        return FLCTL->rBANK0_INFO_WEPROT.r & sector;
    case FLASH_INFO_MEMORY_SPACE_BANK1:
        ASSERT(sector <= 0x04);
        return FLCTL->rBANK1_INFO_WEPROT.r & sector;
    default:
        return false;
    }
}


bool FlashCtl_verifyMemory(void* verifyAddr, uint32_t length,
        uint_fast8_t pattern)
{
    uint32_t memoryPattern, addr, otpOffset;
    uint32_t b0WaitState, b1WaitState, intStatus;
    uint32_t bankOneStart, startBank, endBank;
    uint_fast8_t b0readMode, b1readMode;
    uint_fast8_t memoryType;
    bool res;

    ASSERT(pattern == FLASH_0_PATTERN || pattern == FLASH_1_PATTERN);

    /* Saving interrupt context and disabling interrupts for program
     * operation
     */
    intStatus = CPU_primask();
    Interrupt_disableMaster();

    /* Casting and determining the memory that we need to use */
    addr = (uint32_t) verifyAddr;
    memoryType =
            (addr > __MAIN_MEMORY_END__) ? FLASH_INFO_SPACE : FLASH_MAIN_SPACE;

    /* Assuming Failure */
    res = false;

    /* Finding out which bank we are in */
    if(addr >  SysCtl_getFlashSize())
    {
        bankOneStart = __INFO_FLASH_TECH_MIDDLE__;
    }
    else
    {
        bankOneStart = SysCtl_getFlashSize() / 2;
    }
    startBank = addr < (bankOneStart) ? FLASH_BANK0 : FLASH_BANK1;
    endBank = (addr + length) < (bankOneStart) ? FLASH_BANK0 : FLASH_BANK1;

    /* Saving context and changing read modes */
    b0WaitState = FlashCtl_getWaitState(startBank);
    b0readMode = FlashCtl_getReadMode(startBank);

    /* Setting the wait state to account for the mode */
    FlashCtl_setWaitState(startBank, (2 * b0WaitState) + 1);

    if(startBank != endBank)
    {
        b1WaitState = FlashCtl_getWaitState(endBank);
        b1readMode = FlashCtl_getReadMode(endBank);
        FlashCtl_setWaitState(endBank, (2 * b1WaitState) + 1);
    }

    /* Changing to the relevant VERIFY mode */
    if (pattern == FLASH_1_PATTERN)
    {
        FlashCtl_setReadMode(startBank, FLASH_ERASE_VERIFY_READ_MODE);

        if(startBank != endBank)
        {
            FlashCtl_setReadMode(endBank, FLASH_ERASE_VERIFY_READ_MODE);
        }

        memoryPattern = 0xFFFFFFFF;
    } else
    {
        FlashCtl_setReadMode(startBank, FLASH_PROGRAM_VERIFY_READ_MODE);

        if(startBank != endBank)
        {
            FlashCtl_setReadMode(endBank, FLASH_PROGRAM_VERIFY_READ_MODE);
        }

        memoryPattern = 0;
    }

    /* Taking care of byte accesses */
    while ((addr & 0x03) && (length > 0))
    {
        if (HWREG8(addr++) != ((uint8_t) memoryPattern))
            goto FlashVerifyCleanup;
        length--;
    }

    /* Making sure we are aligned by 128-bit address */
    while (((addr & 0x0F)) && (length > 3))
    {
        if (HWREG32(addr) != memoryPattern)
            goto FlashVerifyCleanup;

        addr = addr + 4;
        length = length - 4;
    }

    /* Burst Verify */
    if (length > 63)
    {
        /* Setting/clearing INFO flash flags as appropriate */
        if (addr > __MAIN_MEMORY_END__)
        {
            FLCTL->rRDBRST_CTLSTAT.r = (FLCTL->rRDBRST_CTLSTAT.r
                    & ~FLCTL_RDBRST_CTLSTAT_MEM_TYPE_M)
                    | FLCTL_RDBRST_CTLSTAT_MEM_TYPE_1;
            otpOffset = __INFO_FLASH_TECH_START__;
        } else
        {
            FLCTL->rRDBRST_CTLSTAT.r = (FLCTL->rRDBRST_CTLSTAT.r
                    & ~FLCTL_RDBRST_CTLSTAT_MEM_TYPE_M)
                    | FLCTL_RDBRST_CTLSTAT_MEM_TYPE_0;
            otpOffset = __MAIN_MEMORY_START__;
        }

        /* Clearing any lingering fault flags  and preparing burst verify*/
        BITBAND_PERI(FLCTL->rRDBRST_CTLSTAT.r,
                FLCTL_RDBRST_CTLSTAT_CLR_STAT_OFS) = 1;
        FLCTL->rRDBRST_FAILCNT.r = 0;
        FLCTL->rRDBRST_STARTADDR.r = addr - otpOffset;
        FLCTL->rRDBRST_LEN.r = (length & 0xFFFFFFF0);
        addr += FLCTL->rRDBRST_LEN.r;
        length = length & 0xF;

        /* Starting Burst Verify */
        FLCTL->rRDBRST_CTLSTAT.r = (FLCTL_RDBRST_CTLSTAT_STOP_FAIL | pattern
                | memoryType | FLCTL_RDBRST_CTLSTAT_START);

        /* While the burst read hasn't finished */
        while ((FLCTL->rRDBRST_CTLSTAT.r & FLCTL_RDBRST_CTLSTAT_BRST_STAT_M)
                != FLCTL_RDBRST_CTLSTAT_BRST_STAT_3)
        {
            __no_operation();
        }

        /* Checking  for a verification/access error/failure */
        if (BITBAND_PERI(FLCTL->rRDBRST_CTLSTAT.r,
                FLCTL_RDBRST_CTLSTAT_CMP_ERR_OFS)
                || BITBAND_PERI(FLCTL->rRDBRST_CTLSTAT.r,
                        FLCTL_RDBRST_CTLSTAT_ADDR_ERR_OFS)
                || FLCTL->rRDBRST_FAILCNT.r)
        {
            goto FlashVerifyCleanup;
        }
    }

    /* Remaining Words */
    while (length > 3)
    {
        if (HWREG32(addr) != memoryPattern)
            goto FlashVerifyCleanup;

        addr = addr + 4;
        length = length - 4;
    }

    /* Remaining Bytes */
    while (length > 0)
    {
        if (HWREG8(addr++) != ((uint8_t) memoryPattern))
            goto FlashVerifyCleanup;
        length--;
    }

    /* If we got this far, that means it no failure happened */
    res = true;

    FlashVerifyCleanup:

    /* Clearing the Read Burst flag and returning */
    BITBAND_PERI(FLCTL->rRDBRST_CTLSTAT.r,
            FLCTL_RDBRST_CTLSTAT_CLR_STAT_OFS) = 1;

    FlashCtl_setReadMode(startBank, b0readMode);
    FlashCtl_setWaitState(startBank, b0WaitState);

    if(startBank != endBank)
    {
        FlashCtl_setReadMode(endBank, b1readMode);
        FlashCtl_setWaitState(endBank, b1WaitState);
    }

    if(intStatus == 0)
        Interrupt_enableMaster();

    return res;
}


bool FlashCtl_performMassErase(void)
{
    uint32_t userFlash, ii, sector, intStatus;
    bool res;

    /* Saving interrupt context and disabling interrupts for program
     * operation
     */
    intStatus = CPU_primask();
    Interrupt_disableMaster();

    /* Assume Failure */
    res = false;

    /* Clearing old mass erase flags */
    BITBAND_PERI(FLCTL->rERASE_CTLSTAT.r, FLCTL_ERASE_CTLSTAT_CLR_STAT_OFS) = 1;

    /* Performing the mass erase */
    FLCTL->rERASE_CTLSTAT.r |= (FLCTL_ERASE_CTLSTAT_MODE
            | FLCTL_ERASE_CTLSTAT_START);

    while ((FLCTL->rERASE_CTLSTAT.r & FLCTL_ERASE_CTLSTAT_STATUS_M)
            == FLCTL_ERASE_CTLSTAT_STATUS_1
            || (FLCTL->rERASE_CTLSTAT.r & FLCTL_ERASE_CTLSTAT_STATUS_M)
                    == FLCTL_ERASE_CTLSTAT_STATUS_2)
    {
        __no_operation();
    }

    /* Return false if an address error */
    if (BITBAND_PERI(FLCTL->rERASE_CTLSTAT.r, FLCTL_ERASE_CTLSTAT_ADDR_ERR_OFS))
        goto MassEraseCleanup;

    /* Changing to erase verify */
    userFlash = SysCtl_getFlashSize() / 2;

    for (ii = __MAIN_MEMORY_START__; ii < userFlash; ii += 4096)
    {
        sector = getUserFlashSector(ii);

        if (!((FLCTL->rBANK0_MAIN_WEPROT.r) & sector))
        {
            if (!FlashCtl_verifyMemory((void*) ii, 4096, FLASH_1_PATTERN))
            {
                if (!FlashCtl_eraseSector(ii))
                    goto MassEraseCleanup;
            }
        }

        if (!(FLCTL->rBANK1_MAIN_WEPROT.r & sector))
        {
            if (!FlashCtl_verifyMemory((void*) (ii + userFlash), 4096,
            FLASH_1_PATTERN))
            {
                if (!FlashCtl_eraseSector(ii + userFlash))
                    goto MassEraseCleanup;
            }
        }

        if (sector < FLCTL_BANK0_MAIN_WEPROT_PROT2)
        {
            if (!(FLCTL->rBANK0_INFO_WEPROT.r & sector))
            {
                if (!FlashCtl_verifyMemory(
                        (void*) (ii + __INFO_FLASH_TECH_START__), 4096,
                        FLASH_1_PATTERN))
                {
                    if (!FlashCtl_eraseSector(ii + __INFO_FLASH_TECH_START__))
                        goto MassEraseCleanup;
                }
            }

            if (!(FLCTL->rBANK1_INFO_WEPROT.r & sector))
            {
                if (!FlashCtl_verifyMemory((void*) (ii + (0x202000)), 4096,
                FLASH_1_PATTERN))
                {
                    if (!FlashCtl_eraseSector(ii + (0x202000)))
                        goto MassEraseCleanup;
                }
            }

        }
    }

    /* If we got this far, the mass erase happened */
    res = true;

    MassEraseCleanup:
    BITBAND_PERI(FLCTL->rERASE_CTLSTAT.r, FLCTL_ERASE_CTLSTAT_CLR_STAT_OFS) = 1;

    if(intStatus == 0)
        Interrupt_enableMaster();

    return res;
}


bool FlashCtl_eraseSector(uint32_t addr)
{
    uint_fast8_t memoryType, ii;
    uint32_t otpOffset = 0;
    uint32_t intStatus;
    uint_fast8_t mTries, tlvLength;
    SysCtl_FlashTLV_Info *flInfo;
    bool res;

    /* Saving interrupt context and disabling interrupts for program
     * operation
     */
    intStatus = CPU_primask();
    Interrupt_disableMaster();
    
    /* Assuming Failure */
    res = false;

    memoryType =
            addr > __MAIN_MEMORY_END__ ? FLASH_INFO_SPACE : FLASH_MAIN_SPACE;

    /* Parsing the TLV and getting the maximum erase pulses */
    SysCtl_getTLVInfo(TLV_TAG_FLASHCTL, 0, &tlvLength, (uint32_t**) &flInfo);

    if (tlvLength == 0 || flInfo->maxErasePulses == 0)
    {
        mTries = MAX_ERASE_NO_TLV;
    } else
    {
        mTries = flInfo->maxErasePulses;
    }

    /* We can only erase on 4KB boundaries */
    while (addr & 0xFFF)
    {
        addr--;
    }

    /* Clearing the status */
    BITBAND_PERI(FLCTL->rERASE_CTLSTAT.r, FLCTL_ERASE_CTLSTAT_CLR_STAT_OFS) = 1;

    if (memoryType == FLASH_INFO_SPACE)
    {
        otpOffset = __INFO_FLASH_TECH_START__;
        FLCTL->rERASE_CTLSTAT.r = (FLCTL->rERASE_CTLSTAT.r
                & ~(FLCTL_ERASE_CTLSTAT_TYPE_M)) | FLCTL_ERASE_CTLSTAT_TYPE_1;

    } else
    {
        otpOffset = __MAIN_MEMORY_START__;
        FLCTL->rERASE_CTLSTAT.r = (FLCTL->rERASE_CTLSTAT.r
                & ~(FLCTL_ERASE_CTLSTAT_TYPE_M)) | FLCTL_ERASE_CTLSTAT_TYPE_0;
    }

    /* Clearing old flags  and setting up the erase */
    BITBAND_PERI(FLCTL->rERASE_CTLSTAT.r, FLCTL_ERASE_CTLSTAT_MODE_OFS) = 0;
    FLCTL->rERASE_SECTADDR.r = addr - otpOffset;

    for (ii = 0; ii < mTries; ii++)
    {
        /* Clearing the status */
        BITBAND_PERI(FLCTL->rERASE_CTLSTAT.r, FLCTL_ERASE_CTLSTAT_CLR_STAT_OFS) =
                1;

        /* Starting the erase */
        BITBAND_PERI(FLCTL->rERASE_CTLSTAT.r,
                FLCTL_ERASE_CTLSTAT_START_OFS) = 1;

        while ((FLCTL->rERASE_CTLSTAT.r & FLCTL_ERASE_CTLSTAT_STATUS_M)
                == FLCTL_ERASE_CTLSTAT_STATUS_1
                || (FLCTL->rERASE_CTLSTAT.r & FLCTL_ERASE_CTLSTAT_STATUS_M)
                        == FLCTL_ERASE_CTLSTAT_STATUS_2)
        {
            __no_operation();
        }

        /* Return false if an address error */
        if (BITBAND_PERI(FLCTL->rERASE_CTLSTAT.r,
                FLCTL_ERASE_CTLSTAT_ADDR_ERR_OFS))
        {
            goto SectorEraseCleanup;
        }
        /* Erase verifying */
        if (FlashCtl_verifyMemory((void*) addr, 4096, FLASH_1_PATTERN))
        {
            res = true;
            goto SectorEraseCleanup;
        }

    }

SectorEraseCleanup:

    BITBAND_PERI(FLCTL->rERASE_CTLSTAT.r, FLCTL_ERASE_CTLSTAT_CLR_STAT_OFS) = 1;
    
    if(intStatus == 0)
        Interrupt_enableMaster();

    return res;
}


bool FlashCtl_programMemory(void* src, void* dest, uint32_t length)
{
    uint32_t destAddr, srcAddr, burstLength, intStatus;
    bool res;
    uint_fast8_t mTries, tlvLength;
    SysCtl_FlashTLV_Info *flInfo;

    /* Saving interrupt context and disabling interrupts for program
     * operation
     */
    intStatus = CPU_primask();
    Interrupt_disableMaster();

    /* Parsing the TLV and getting the maximum erase pulses */
    SysCtl_getTLVInfo(TLV_TAG_FLASHCTL, 0, &tlvLength, (uint32_t**) &flInfo);

    if (tlvLength == 0 || flInfo->maxProgramPulses == 0)
    {
        mTries = MAX_PROGRAM_NO_TLV;
    } else
    {
        mTries = flInfo->maxProgramPulses;
    }

    /* Casting to integers */
    srcAddr = (uint32_t) src;
    destAddr = (uint32_t) dest;

    /* Enabling word programming */
    FlashCtl_enableWordProgramming(FLASH_IMMEDIATE_WRITE_MODE);

    /* Assume failure */
    res = false;

    /* Taking care of byte accesses */
    while ((destAddr & 0x03) && length > 0)
    {
        if (!_FlashCtl_Program8(srcAddr, destAddr, mTries))
        {
            goto FlashProgramCleanUp;
        } else
        {
            srcAddr++;
            destAddr++;
            length--;
        }
    }

    /* Taking care of word accesses */
    while ((destAddr & 0x0F) && (length > 3))
    {
        if (!_FlashCtl_Program32(srcAddr, destAddr, mTries))
        {
            goto FlashProgramCleanUp;
        } else
        {
            srcAddr += 4;
            destAddr += 4;
            length -= 4;
        }
    }

    /* Taking care of burst programs */
    while (length > 16)
    {
        burstLength = length > 63 ? 64 : length & 0xFFFFFFF0;

        if (!_FlashCtl_ProgramBurst(srcAddr, destAddr, burstLength, mTries))
        {
            goto FlashProgramCleanUp;
        } else
        {
            srcAddr += burstLength;
            destAddr += burstLength;
            length -= burstLength;
        }
    }

    /* Remaining word accesses */
    while (length > 3)
    {
        if (!_FlashCtl_Program32(srcAddr, destAddr, mTries))
        {
            goto FlashProgramCleanUp;
        } else
        {
            srcAddr+=4;
            destAddr+=4;
            length-=4;
        }
    }

    /* Remaining byte accesses */
    while (length > 0)
    {
        if (!_FlashCtl_Program8(srcAddr, destAddr, mTries))
        {
            goto FlashProgramCleanUp;
        } else
        {
            srcAddr++;
            destAddr++;
            length--;
        }
    }

    /* If we got this far that means that we succeeded  */
    res = true;

    FlashProgramCleanUp:

    if(intStatus == 0)
        Interrupt_enableMaster();

    FlashCtl_disableWordProgramming();
    return res;

}


void FlashCtl_setWaitState(uint32_t flashBank, uint32_t waitState)
{
    if (flashBank == FLASH_BANK0)
    {
        FLCTL->rBANK0_RDCTL.r = (FLCTL->rBANK0_RDCTL.r
                & ~FLCTL_BANK0_RDCTL_WAIT_M) | (waitState << 12);
    } else if (flashBank == FLASH_BANK1)
    {
        FLCTL->rBANK1_RDCTL.r = (FLCTL->rBANK1_RDCTL.r
                & ~FLCTL_BANK1_RDCTL_WAIT_M) | (waitState << 12);
    } else
    {
        ASSERT(false);
    }
}


uint32_t FlashCtl_getWaitState(uint32_t flashBank)
{
    if (flashBank == FLASH_BANK0)
    {
        return FLCTL->rBANK0_RDCTL.b.bWAIT;
    } else if (flashBank == FLASH_BANK1)
    {
        return FLCTL->rBANK1_RDCTL.b.bWAIT;
    } else
    {
        ASSERT(false);
        return 0;
    }
}


void FlashCtl_setProgramVerification(uint32_t verificationSetting)
{
    if ((verificationSetting & FLASH_BURSTPOST))
        BITBAND_PERI(FLCTL->rPRGBRST_CTLSTAT.r,
                FLCTL_PRGBRST_CTLSTAT_AUTO_PST_OFS) = 1;

    if ((verificationSetting & FLASH_BURSTPRE))
        BITBAND_PERI(FLCTL->rPRGBRST_CTLSTAT.r,
                FLCTL_PRGBRST_CTLSTAT_AUTO_PRE_OFS) = 1;

    if ((verificationSetting & FLASH_REGPRE))
        BITBAND_PERI(FLCTL->rPRG_CTLSTAT.r, FLCTL_PRG_CTLSTAT_VER_PRE_OFS) = 1;

    if ((verificationSetting & FLASH_REGPOST))
        BITBAND_PERI(FLCTL->rPRG_CTLSTAT.r, FLCTL_PRG_CTLSTAT_VER_PST_OFS) = 1;
}


void FlashCtl_clearProgramVerification(uint32_t verificationSetting)
{
    if ((verificationSetting & FLASH_BURSTPOST))
        BITBAND_PERI(FLCTL->rPRGBRST_CTLSTAT.r,
                FLCTL_PRGBRST_CTLSTAT_AUTO_PST_OFS) = 0;

    if ((verificationSetting & FLASH_BURSTPRE))
        BITBAND_PERI(FLCTL->rPRGBRST_CTLSTAT.r,
                FLCTL_PRGBRST_CTLSTAT_AUTO_PRE_OFS) = 0;

    if ((verificationSetting & FLASH_REGPRE))
        BITBAND_PERI(FLCTL->rPRG_CTLSTAT.r, FLCTL_PRG_CTLSTAT_VER_PRE_OFS) = 0;

    if ((verificationSetting & FLASH_REGPOST))
        BITBAND_PERI(FLCTL->rPRG_CTLSTAT.r, FLCTL_PRG_CTLSTAT_VER_PST_OFS) = 0;

}


void FlashCtl_enableWordProgramming(uint32_t mode)
{
    if (mode == FLASH_IMMEDIATE_WRITE_MODE)
    {
        BITBAND_PERI(FLCTL->rPRG_CTLSTAT.r, FLCTL_PRG_CTLSTAT_ENABLE_OFS) = 1;
        BITBAND_PERI(FLCTL->rPRG_CTLSTAT.r, FLCTL_PRG_CTLSTAT_MODE_OFS) = 0;

    } else if (mode == FLASH_COLLATED_WRITE_MODE)
    {
        BITBAND_PERI(FLCTL->rPRG_CTLSTAT.r, FLCTL_PRG_CTLSTAT_ENABLE_OFS) = 1;
        BITBAND_PERI(FLCTL->rPRG_CTLSTAT.r, FLCTL_PRG_CTLSTAT_MODE_OFS) = 1;
    }
}


void FlashCtl_disableWordProgramming(void)
{
    BITBAND_PERI(FLCTL->rPRG_CTLSTAT.r, FLCTL_PRG_CTLSTAT_ENABLE_OFS) = 0;
}


uint32_t FlashCtl_getReadMode(uint32_t flashBank)
{
    if (flashBank == FLASH_BANK0)
    {
        return FLCTL->rBANK0_RDCTL.b.bRD_MODE;
    } else if (flashBank == FLASH_BANK1)
    {
        return FLCTL->rBANK1_RDCTL.b.bRD_MODE;
    } else
    {
        ASSERT(false);
        return 0;
    }
}


uint32_t FlashCtl_isWordProgrammingEnabled(void)
{
    if (!BITBAND_PERI(FLCTL->rPRG_CTLSTAT.r, FLCTL_PRG_CTLSTAT_ENABLE_OFS))
    {
        return 0;
    } else if (BITBAND_PERI(FLCTL->rPRG_CTLSTAT.r, FLCTL_PRG_CTLSTAT_MODE_OFS))
        return FLASH_COLLATED_WRITE_MODE;
    else
        return FLASH_IMMEDIATE_WRITE_MODE;
}


void FlashCtl_enableInterrupt(uint32_t flags)
{
    FLCTL->rIE.r |= flags;
}


void FlashCtl_disableInterrupt(uint32_t flags)
{
    FLCTL->rIE.r &= ~flags;
}


uint32_t FlashCtl_getEnabledInterruptStatus(void)
{
    return FlashCtl_getInterruptStatus() & FLCTL->rIE.r;
}


void FlashCtl_initiateSectorErase(uint32_t addr)
{
    uint_fast8_t memoryType;
    uint32_t otpOffset = 0;

    memoryType =
            addr > __MAIN_MEMORY_END__ ? FLASH_INFO_SPACE : FLASH_MAIN_SPACE;

    /* We can only erase on 4KB boundaries */
    while (addr & 0xFFF)
    {
        addr--;
    }

    /* Clearing the status */
    BITBAND_PERI(FLCTL->rERASE_CTLSTAT.r, FLCTL_ERASE_CTLSTAT_CLR_STAT_OFS) = 1;

    if (memoryType == FLASH_INFO_SPACE)
    {
        otpOffset = __INFO_FLASH_TECH_START__;
        FLCTL->rERASE_CTLSTAT.r = (FLCTL->rERASE_CTLSTAT.r
                & ~(FLCTL_ERASE_CTLSTAT_TYPE_M)) | FLCTL_ERASE_CTLSTAT_TYPE_1;

    } else
    {
        otpOffset = __MAIN_MEMORY_START__;
        FLCTL->rERASE_CTLSTAT.r = (FLCTL->rERASE_CTLSTAT.r
                & ~(FLCTL_ERASE_CTLSTAT_TYPE_M)) | FLCTL_ERASE_CTLSTAT_TYPE_0;
    }

    /* Clearing old flags  and setting up the erase */
    BITBAND_PERI(FLCTL->rERASE_CTLSTAT.r, FLCTL_ERASE_CTLSTAT_MODE_OFS) = 0;
    FLCTL->rERASE_SECTADDR.r = addr - otpOffset;

    /* Starting the erase */
    BITBAND_PERI(FLCTL->rERASE_CTLSTAT.r,
            FLCTL_ERASE_CTLSTAT_START_OFS) = 1;

}


void FlashCtl_initiateMassErase(void)
{
    /* Clearing old mass erase flags */
    BITBAND_PERI(FLCTL->rERASE_CTLSTAT.r, FLCTL_ERASE_CTLSTAT_CLR_STAT_OFS) = 1;

    /* Performing the mass erase */
    FLCTL->rERASE_CTLSTAT.r |= (FLCTL_ERASE_CTLSTAT_MODE
            | FLCTL_ERASE_CTLSTAT_START);
}


uint32_t FlashCtl_getInterruptStatus(void)
{
    return FLCTL->rIFG.r;
}


bool FlashCtl_setReadMode(uint32_t flashBank, uint32_t readMode)
{

    if (FLCTL->rPOWER_STAT.r & FLCTL_POWER_STAT_RD_2T)
        return false;

    if (flashBank == FLASH_BANK0)
    {
        FLCTL->rBANK0_RDCTL.r = (FLCTL->rBANK0_RDCTL.r
                & ~FLCTL_BANK0_RDCTL_RD_MODE_M) | readMode;
        while (FLCTL->rBANK0_RDCTL.b.bRD_MODE_STATUS != readMode)
            ;
    } else if (flashBank == FLASH_BANK1)
    {
        FLCTL->rBANK1_RDCTL.r = (FLCTL->rBANK1_RDCTL.r
                & ~FLCTL_BANK1_RDCTL_RD_MODE_M) | readMode;
        while (FLCTL->rBANK1_RDCTL.b.bRD_MODE_STATUS != readMode)
            ;
    } else
    {
        ASSERT(false);
        return false;
    }

    return true;
}


void FlashCtl_clearInterruptFlag(uint32_t flags)
{
    FLCTL->rCLRIFG.r |= flags;
}


void FPU_enableModule(void)
{
    //
    // Enable the coprocessors used by the floating-point unit.
    //
    SCB->CPACR = ((SCB->CPACR & ~(SCB_CPACR_CP11_M | SCB_CPACR_CP10_M))
            | SCB_CPACR_CP11_M | SCB_CPACR_CP10_M);
}


void FPU_disableModule(void)
{
    //
    // Disable the coprocessors used by the floating-point unit.
    //
    SCB->CPACR = ((SCB->CPACR & ~(SCB_CPACR_CP10_M | SCB_CPACR_CP11_M)));
}


void FPU_enableStacking(void)
{
    //
    // Enable automatic state preservation for the floating-point unit, and
    // disable lazy state preservation (meaning that the floating-point state
    // is always stacked when floating-point instructions are used).
    //
    FPU->FPCCR = (FPU->FPCCR & ~FPU_FPCCR_LSPEN) | FPU_FPCCR_ASPEN;
}


void FPU_enableLazyStacking(void)
{
    //
    // Enable automatic and lazy state preservation for the floating-point
    // unit.
    //
    FPU->FPCCR |= FPU_FPCCR_ASPEN | FPU_FPCCR_LSPEN;
}


void FPU_disableStacking(void)
{
    //
    // Disable automatic and lazy state preservation for the floating-point
    // unit.
    //
    FPU->FPCCR &= ~(FPU_FPCCR_ASPEN | FPU_FPCCR_LSPEN);
}


void FPU_setHalfPrecisionMode(uint32_t mode)
{
    //
    // Set the half-precision floating-point format.
    //
    FPU->FPDSCR = (FPU->FPDSCR & ~(FPU_FPDSCR_AHP_Msk)) | mode;
}


void FPU_setNaNMode(uint32_t mode)
{
    //
    // Set the NaN mode.
    //
    FPU->FPDSCR = (FPU->FPDSCR & ~(FPU_FPDSCR_DN_Msk)) | mode;
}


void FPU_setFlushToZeroMode(uint32_t mode)
{
    //
    // Set the flush-to-zero mode.
    //
    FPU->FPDSCR = (FPU->FPDSCR & ~(FPU_FPDSCR_FZ_Msk)) | mode;
}


void FPU_setRoundingMode(uint32_t mode)
{
    //
    // Set the rounding mode.
    //
    FPU->FPDSCR = (FPU->FPDSCR & ~(FPU_FPDSCR_RMode_Msk)) | mode;
}


static const uint32_t GPIO_PORT_TO_BASE[] =
{   0x00,
    0x40004C00,
    0x40004C01,
    0x40004C20,
    0x40004C21,
    0x40004C40,
    0x40004C41,
    0x40004C60,
    0x40004C61,
    0x40004C80,
    0x40004C81,
    0x40004D20
    };


void GPIO_setAsInputPinWithPullDownResistor(uint_fast8_t selectedPort,
        uint_fast16_t selectedPins)
{

    uint32_t baseAddress = GPIO_PORT_TO_BASE[selectedPort];

    HWREG16(baseAddress + OFS_PASEL0) &= ~selectedPins;
    HWREG16(baseAddress + OFS_PASEL1) &= ~selectedPins;

    HWREG16(baseAddress + OFS_PADIR) &= ~selectedPins;
    HWREG16(baseAddress + OFS_PAREN) |= selectedPins;
    HWREG16(baseAddress + OFS_PAOUT) &= ~selectedPins;
}


uint_fast16_t GPIO_getEnabledInterruptStatus(uint_fast8_t selectedPort)
{
    uint_fast16_t pendingInts;
    uint32_t baseAddr;

    pendingInts = GPIO_getInterruptStatus(selectedPort, 0xFFFF);
    baseAddr = GPIO_PORT_TO_BASE[selectedPort];

    ASSERT(baseAddr != 0xFFFF);

    switch (selectedPort)
    {
    case GPIO_PORT_P1:
    case GPIO_PORT_P3:
    case GPIO_PORT_P5:
    case GPIO_PORT_P7:
    case GPIO_PORT_P9:
        return (HWREG8(baseAddr + OFS_P1IE) & pendingInts);
    case GPIO_PORT_P2:
    case GPIO_PORT_P4:
    case GPIO_PORT_P6:
    case GPIO_PORT_P8:
    case GPIO_PORT_P10:
        return (HWREG8(baseAddr + OFS_P2IE) & pendingInts);
    case GPIO_PORT_PJ:
        return (HWREG16(baseAddr + OFS_PAIE) & pendingInts);
    default:
        return 0;
    }
}


void GPIO_setDriveStrengthHigh(uint_fast8_t selectedPort,
        uint_fast8_t selectedPins)
{
    uint32_t baseAddr;

    baseAddr = GPIO_PORT_TO_BASE[selectedPort];

    HWREG8(baseAddr + OFS_PADS) |= selectedPins;

}


void GPIO_setDriveStrengthLow(uint_fast8_t selectedPort,
        uint_fast8_t selectedPins)
{
    uint32_t baseAddr;

    baseAddr = GPIO_PORT_TO_BASE[selectedPort];

    HWREG8(baseAddr + OFS_PADS) &= ~selectedPins;

}


void GPIO_setAsInputPinWithPullUpResistor(uint_fast8_t selectedPort,
        uint_fast16_t selectedPins)
{

    uint32_t baseAddress = GPIO_PORT_TO_BASE[selectedPort];

    HWREG16(baseAddress + OFS_PASEL0) &= ~selectedPins;
    HWREG16(baseAddress + OFS_PASEL1) &= ~selectedPins;
    HWREG16(baseAddress + OFS_PADIR) &= ~selectedPins;
    HWREG16(baseAddress + OFS_PAREN) |= selectedPins;
    HWREG16(baseAddress + OFS_PAOUT) |= selectedPins;
}


void GPIO_setAsOutputPin(uint_fast8_t selectedPort, uint_fast16_t selectedPins)
{
    uint32_t baseAddress = GPIO_PORT_TO_BASE[selectedPort];

    HWREG16(baseAddress + OFS_PASEL0) &= ~selectedPins;
    HWREG16(baseAddress + OFS_PASEL1) &= ~selectedPins;
    HWREG16(baseAddress + OFS_PADIR) |= selectedPins;

    return;
}


void GPIO_setAsInputPin(uint_fast8_t selectedPort, uint_fast16_t selectedPins)
{

    uint32_t baseAddress = GPIO_PORT_TO_BASE[selectedPort];

    HWREG16(baseAddress + OFS_PASEL0) &= ~selectedPins;
    HWREG16(baseAddress + OFS_PASEL1) &= ~selectedPins;
    HWREG16(baseAddress + OFS_PADIR) &= ~selectedPins;
    HWREG16(baseAddress + OFS_PAREN) &= ~selectedPins;
}


void GPIO_setOutputHighOnPin(uint_fast8_t selectedPort,
        uint_fast16_t selectedPins)
{

    uint32_t baseAddress = GPIO_PORT_TO_BASE[selectedPort];

    HWREG16(baseAddress + OFS_PAOUT) |= selectedPins;
}


void GPIO_setOutputLowOnPin(uint_fast8_t selectedPort,
        uint_fast16_t selectedPins)
{

    uint32_t baseAddress = GPIO_PORT_TO_BASE[selectedPort];

    HWREG16(baseAddress + OFS_PAOUT) &= ~selectedPins;
}


void GPIO_toggleOutputOnPin(uint_fast8_t selectedPort,
        uint_fast16_t selectedPins)
{

    uint32_t baseAddress = GPIO_PORT_TO_BASE[selectedPort];

    HWREG16(baseAddress + OFS_PAOUT) ^= selectedPins;
}


void GPIO_enableInterrupt(uint_fast8_t selectedPort, uint_fast16_t selectedPins)
{

    uint32_t baseAddress = GPIO_PORT_TO_BASE[selectedPort];

    HWREG16(baseAddress + OFS_PAIE) |= selectedPins;
}


void GPIO_disableInterrupt(uint_fast8_t selectedPort,
        uint_fast16_t selectedPins)
{

    uint32_t baseAddress = GPIO_PORT_TO_BASE[selectedPort];

    HWREG16(baseAddress + OFS_PAIE) &= ~selectedPins;
}


void GPIO_clearInterruptFlag(uint_fast8_t selectedPort,
        uint_fast16_t selectedPins)
{

    uint32_t baseAddress = GPIO_PORT_TO_BASE[selectedPort];


    HWREG16(baseAddress + OFS_PAIFG) &= ~selectedPins;
}


uint_fast16_t GPIO_getInterruptStatus(uint_fast8_t selectedPort,
        uint_fast16_t selectedPins)
{

    uint32_t baseAddress = GPIO_PORT_TO_BASE[selectedPort];

    return HWREG16(baseAddress + OFS_PAIFG) & selectedPins;
}


void GPIO_interruptEdgeSelect(uint_fast8_t selectedPort,
        uint_fast16_t selectedPins, uint_fast8_t edgeSelect)
{

    uint32_t baseAddress = GPIO_PORT_TO_BASE[selectedPort];


    if (GPIO_LOW_TO_HIGH_TRANSITION == edgeSelect)
        HWREG16(baseAddress + OFS_PAIES) &= ~selectedPins;
    else
        HWREG16(baseAddress + OFS_PAIES) |= selectedPins;
}


uint8_t GPIO_getInputPinValue(uint_fast8_t selectedPort,
        uint_fast16_t selectedPins)
{
    uint_fast16_t inputPinValue;
    uint32_t baseAddress = GPIO_PORT_TO_BASE[selectedPort];

    inputPinValue = HWREG16(baseAddress + OFS_PAIN) & (selectedPins);

    if (inputPinValue > 0)
        return GPIO_INPUT_PIN_HIGH;
    return GPIO_INPUT_PIN_LOW;
}


void GPIO_setAsPeripheralModuleFunctionOutputPin(uint_fast8_t selectedPort,
        uint_fast16_t selectedPins, uint_fast8_t mode)
{

    uint32_t baseAddress = GPIO_PORT_TO_BASE[selectedPort];

    HWREG16(baseAddress + OFS_PADIR) |= selectedPins;
    switch (mode)
    {
    case GPIO_PRIMARY_MODULE_FUNCTION:
        HWREG16(baseAddress + OFS_PASEL0) |= selectedPins;
        HWREG16(baseAddress + OFS_PASEL1) &= ~selectedPins;
        break;
    case GPIO_SECONDARY_MODULE_FUNCTION:
        HWREG16(baseAddress + OFS_PASEL0) &= ~selectedPins;
        HWREG16(baseAddress + OFS_PASEL1) |= selectedPins;
        break;
    case GPIO_TERTIARY_MODULE_FUNCTION:
        HWREG16(baseAddress + OFS_PASEL0) |= selectedPins;
        HWREG16(baseAddress + OFS_PASEL1) |= selectedPins;
        break;
    }
}


void GPIO_setAsPeripheralModuleFunctionInputPin(uint_fast8_t selectedPort,
        uint_fast16_t selectedPins, uint_fast8_t mode)
{
    uint32_t baseAddress = GPIO_PORT_TO_BASE[selectedPort];

    HWREG16(baseAddress + OFS_PADIR) &= ~selectedPins;
    switch (mode)
    {
    case GPIO_PRIMARY_MODULE_FUNCTION:
        HWREG16(baseAddress + OFS_PASEL0) |= selectedPins;
        HWREG16(baseAddress + OFS_PASEL1) &= ~selectedPins;
        break;
    case GPIO_SECONDARY_MODULE_FUNCTION:
        HWREG16(baseAddress + OFS_PASEL0) &= ~selectedPins;
        HWREG16(baseAddress + OFS_PASEL1) |= selectedPins;
        break;
    case GPIO_TERTIARY_MODULE_FUNCTION:
        HWREG16(baseAddress + OFS_PASEL0) |= selectedPins;
        HWREG16(baseAddress + OFS_PASEL1) |= selectedPins;
        break;
    }
}


void I2C_initMaster(uint32_t moduleInstance, const eUSCI_I2C_MasterConfig *config)
{
    uint_fast16_t preScalarValue;

    ASSERT(
            (EUSCI_B_I2C_CLOCKSOURCE_ACLK == config->selectClockSource)
            || (EUSCI_B_I2C_CLOCKSOURCE_SMCLK
                    == config->selectClockSource));

    ASSERT(
            (EUSCI_B_I2C_SET_DATA_RATE_400KBPS == config->dataRate)
            || (EUSCI_B_I2C_SET_DATA_RATE_100KBPS == config->dataRate));

    ASSERT(
            (EUSCI_B_I2C_NO_AUTO_STOP == config->autoSTOPGeneration)
            || (EUSCI_B_I2C_SET_BYTECOUNT_THRESHOLD_FLAG
                    == config->autoSTOPGeneration)
            || (EUSCI_B_I2C_SEND_STOP_AUTOMATICALLY_ON_BYTECOUNT_THRESHOLD
                    == config->autoSTOPGeneration));

    /* Disable the USCI module and clears the other bits of control register */
    BITBAND_PERI(EUSCI_B_CMSIS(moduleInstance)->rCTLW0.r,UCSWRST_OFS) = 1;

    /* Configure Automatic STOP condition generation */
    EUSCI_B_CMSIS(moduleInstance)->rCTLW1.r =
            (EUSCI_B_CMSIS(moduleInstance)->rCTLW1.r & ~UCASTP_M)
                    | (config->autoSTOPGeneration);

    /* Byte Count Threshold */
    EUSCI_B_CMSIS(moduleInstance)->rTBCNT.r = config->byteCounterThreshold;

    /*
     * Configure as I2C master mode.
     * UCMST = Master mode
     * UCMODE_3 = I2C mode
     * UCSYNC = Synchronous mode
     */
    EUSCI_B_CMSIS(moduleInstance)->rCTLW0.r =
            (EUSCI_B_CMSIS(moduleInstance)->rCTLW0.r & ~UCSSEL_M)
                    | (config->selectClockSource | UCMST | UCMODE_3 | UCSYNC
                            | UCSWRST);

    /*
     * Compute the clock divider that achieves the fastest speed less than or
     * equal to the desired speed.  The numerator is biased to favor a larger
     * clock divider so that the resulting clock is always less than or equal
     * to the desired clock, never greater.
     */
    preScalarValue = (uint16_t) (config->i2cClk / config->dataRate);

    EUSCI_B_CMSIS(moduleInstance)->rBRW = preScalarValue;
}


void I2C_initSlave(uint32_t moduleInstance, uint_fast16_t slaveAddress,
        uint_fast8_t slaveAddressOffset, uint32_t slaveOwnAddressEnable)
{
    ASSERT(
            (EUSCI_B_I2C_OWN_ADDRESS_OFFSET0 == slaveAddressOffset)
            || (EUSCI_B_I2C_OWN_ADDRESS_OFFSET1 == slaveAddressOffset)
            || (EUSCI_B_I2C_OWN_ADDRESS_OFFSET2 == slaveAddressOffset)
            || (EUSCI_B_I2C_OWN_ADDRESS_OFFSET3 == slaveAddressOffset));

    /* Disable the USCI module */
    BITBAND_PERI(EUSCI_B_CMSIS(moduleInstance)->rCTLW0.r,UCSWRST_OFS) = 1;

    /* Clear USCI master mode */
    EUSCI_B_CMSIS(moduleInstance)->rCTLW0.r =
            (EUSCI_B_CMSIS(moduleInstance)->rCTLW0.r & (~UCMST))
                    | (UCMODE_3 + UCSYNC);

    /* Set up the slave address. */
    HWREG16(moduleInstance + OFS_UCB0I2COA0 + slaveAddressOffset) = slaveAddress
            + slaveOwnAddressEnable;
}


void I2C_enableModule(uint32_t moduleInstance)
{
    /* Reset the UCSWRST bit to enable the USCI Module */
    BITBAND_PERI(EUSCI_B_CMSIS(moduleInstance)->rCTLW0.r,UCSWRST_OFS) = 0;
}


void I2C_disableModule(uint32_t moduleInstance)
{
    /* Set the UCSWRST bit to disable the USCI Module */
    BITBAND_PERI(EUSCI_B_CMSIS(moduleInstance)->rCTLW0.r,UCSWRST_OFS) = 1;
    ;
}


void I2C_setSlaveAddress(uint32_t moduleInstance, uint_fast16_t slaveAddress)
{
    /* Set the address of the slave with which the master will communicate */
    EUSCI_B_CMSIS(moduleInstance)->rI2CSA.r = (slaveAddress);
}


void I2C_setMode(uint32_t moduleInstance, uint_fast8_t mode)
{
    ASSERT(
            (EUSCI_B_I2C_TRANSMIT_MODE == mode)
            || (EUSCI_B_I2C_RECEIVE_MODE == mode));

    EUSCI_B_CMSIS(moduleInstance)->rCTLW0.r =
            (EUSCI_B_CMSIS(moduleInstance)->rCTLW0.r
                    & (~EUSCI_B_I2C_TRANSMIT_MODE)) | mode;

}


void I2C_slavePutData(uint32_t moduleInstance, uint8_t transmitData)
{
    //Send single byte data.
    EUSCI_B_CMSIS(moduleInstance)->rTXBUF.r = transmitData;
}


uint8_t I2C_slaveGetData(uint32_t moduleInstance)
{
    //Read a byte.
    return EUSCI_B_CMSIS(moduleInstance)->rRXBUF.b.bRXBUF;
}


uint8_t I2C_isBusBusy(uint32_t moduleInstance)
{
    //Return the bus busy status.
    return EUSCI_B_CMSIS(moduleInstance)->rSTATW.b.bBBUSY;
}


void I2C_masterSendSingleByte(uint32_t moduleInstance, uint8_t txData)
{
    //Store current TXIE status
    uint16_t txieStatus = EUSCI_B_CMSIS(moduleInstance)->rIE.r & UCTXIE;

    //Disable transmit interrupt enable
    BITBAND_PERI(EUSCI_B_CMSIS(moduleInstance)->rIE.r,UCTXIE_OFS) = 0;

    //Send start condition.
    EUSCI_B_CMSIS(moduleInstance)->rCTLW0.r |= UCTR + UCTXSTT;

    //Poll for transmit interrupt flag.
    while (!(EUSCI_B_CMSIS(moduleInstance)->rIFG.r & UCTXIFG))
        ;

    //Send single byte data.
    EUSCI_B_CMSIS(moduleInstance)->rTXBUF.r = txData;

    //Poll for transmit interrupt flag.
    while (!(EUSCI_B_CMSIS(moduleInstance)->rIFG.r & UCTXIFG))
        ;

    //Send stop condition.
    EUSCI_B_CMSIS(moduleInstance)->rCTLW0.r |= UCTXSTP;

    //Clear transmit interrupt flag before enabling interrupt again
    EUSCI_B_CMSIS(moduleInstance)->rIFG.r &= ~(UCTXIFG);

    //Reinstate transmit interrupt enable
    EUSCI_B_CMSIS(moduleInstance)->rIE.r |= txieStatus;
}


bool I2C_masterSendSingleByteWithTimeout(uint32_t moduleInstance,
        uint8_t txData, uint32_t timeout)
{
    uint_fast16_t txieStatus;
    uint32_t timeout2 = timeout;

    ASSERT(timeout > 0);

    //Store current TXIE status
    txieStatus = EUSCI_B_CMSIS(moduleInstance)->rIE.r & UCTXIE;

    //Disable transmit interrupt enable
    BITBAND_PERI(EUSCI_B_CMSIS(moduleInstance)->rIE.r,UCTXIE_OFS) = 0;

    //Send start condition.
    EUSCI_B_CMSIS(moduleInstance)->rCTLW0.r |= UCTR + UCTXSTT;

    //Poll for transmit interrupt flag.
    while ((!(EUSCI_B_CMSIS(moduleInstance)->rIFG.r & UCTXIFG)) && --timeout)
        ;

    //Check if transfer timed out
    if (timeout == 0)
        return false;

    //Send single byte data.
    EUSCI_B_CMSIS(moduleInstance)->rTXBUF.r = txData;

    //Poll for transmit interrupt flag.
    while ((!BITBAND_PERI(EUSCI_B_CMSIS(moduleInstance)->rIFG.r, UCTXIFG_OFS))
            && --timeout2)
        ;

    //Check if transfer timed out
    if (timeout2 == 0)
        return false;

    //Send stop condition.
    BITBAND_PERI(EUSCI_B_CMSIS(moduleInstance)->rCTLW0.r,UCTXSTP_OFS) = 1;

    //Clear transmit interrupt flag before enabling interrupt again
    BITBAND_PERI(EUSCI_B_CMSIS(moduleInstance)->rIFG.r,UCTXIFG_OFS) = 0;

    //Reinstate transmit interrupt enable
    EUSCI_B_CMSIS(moduleInstance)->rIE.r |= txieStatus;

    return true;
}


void I2C_masterSendMultiByteStart(uint32_t moduleInstance, uint8_t txData)
{
    //Store current transmit interrupt enable
    uint16_t txieStatus = EUSCI_B_CMSIS(moduleInstance)->rIE.r & UCTXIE;

    //Disable transmit interrupt enable
    BITBAND_PERI(EUSCI_B_CMSIS(moduleInstance)->rIE.r, UCTXIE_OFS) = 0;

    //Send start condition.
    EUSCI_B_CMSIS(moduleInstance)->rCTLW0.r |= UCTR + UCTXSTT;

    //Poll for transmit interrupt flag.
    while (!BITBAND_PERI(EUSCI_B_CMSIS(moduleInstance)->rIFG.r, UCTXIFG_OFS))
        ;

    //Send single byte data.
    EUSCI_B_CMSIS(moduleInstance)->rTXBUF.r = txData;

    //Reinstate transmit interrupt enable
    EUSCI_B_CMSIS(moduleInstance)->rIE.r |= txieStatus;
}


bool I2C_masterSendMultiByteStartWithTimeout(uint32_t moduleInstance,
        uint8_t txData, uint32_t timeout)
{
    uint_fast16_t txieStatus;

    ASSERT(timeout > 0);

    //Store current transmit interrupt enable
    txieStatus = EUSCI_B_CMSIS(moduleInstance)->rIE.r & UCTXIE;

    //Disable transmit interrupt enable
    BITBAND_PERI(EUSCI_B_CMSIS(moduleInstance)->rIE.r,UCTXIE_OFS) = 0;

    //Send start condition.
    EUSCI_B_CMSIS(moduleInstance)->rCTLW0.r |= UCTR + UCTXSTT;

    //Poll for transmit interrupt flag.
    while ((!(BITBAND_PERI(EUSCI_B_CMSIS(moduleInstance)->rIFG.r, UCTXIFG_OFS))
            && --timeout))
        ;

    //Check if transfer timed out
    if (timeout == 0)
        return false;

    //Send single byte data.
    EUSCI_B_CMSIS(moduleInstance)->rTXBUF.r = txData;

    //Reinstate transmit interrupt enable
    EUSCI_B_CMSIS(moduleInstance)->rIE.r |= txieStatus;

    return true;
}


void I2C_masterSendMultiByteNext(uint32_t moduleInstance, uint8_t txData)
{
    //If interrupts are not used, poll for flags
    if (!BITBAND_PERI(EUSCI_B_CMSIS(moduleInstance)->rIE.r, UCTXIE_OFS))
    {
        //Poll for transmit interrupt flag.
        while
            (!BITBAND_PERI(EUSCI_B_CMSIS(moduleInstance)->rIFG.r, UCTXIFG_OFS))
            ;
    }

    //Send single byte data.
    EUSCI_B_CMSIS(moduleInstance)->rTXBUF.r = txData;
}


bool I2C_masterSendMultiByteNextWithTimeout(uint32_t moduleInstance,
        uint8_t txData, uint32_t timeout)
{
    ASSERT(timeout > 0);

    //If interrupts are not used, poll for flags
    if (!BITBAND_PERI(EUSCI_B_CMSIS(moduleInstance)->rIE.r, UCTXIE_OFS))
    {
        //Poll for transmit interrupt flag.
        while ((!BITBAND_PERI(EUSCI_B_CMSIS(moduleInstance)->rIFG.r,
                UCTXIFG_OFS)) && --timeout)
            ;

        //Check if transfer timed out
        if (timeout == 0)
            return false;
    }

    //Send single byte data.
    EUSCI_B_CMSIS(moduleInstance)->rTXBUF.r = txData;

    return true;
}


void I2C_masterSendMultiByteFinish(uint32_t moduleInstance, uint8_t txData)
{
    //If interrupts are not used, poll for flags
    if (!BITBAND_PERI(EUSCI_B_CMSIS(moduleInstance)->rIE.r, UCTXIE_OFS))
    {
        //Poll for transmit interrupt flag.
        while
            (!BITBAND_PERI(EUSCI_B_CMSIS(moduleInstance)->rIFG.r, UCTXIFG_OFS))
            ;
    }

    //Send single byte data.
    EUSCI_B_CMSIS(moduleInstance)->rTXBUF.r = txData;

    //Poll for transmit interrupt flag.
    while (!BITBAND_PERI(EUSCI_B_CMSIS(moduleInstance)->rIFG.r, UCTXIFG_OFS))
        ;

    //Send stop condition.
    BITBAND_PERI(EUSCI_B_CMSIS(moduleInstance)->rCTLW0.r,UCTXSTP_OFS) = 1;
}


bool I2C_masterSendMultiByteFinishWithTimeout(uint32_t moduleInstance,
        uint8_t txData, uint32_t timeout)
{
    uint32_t timeout2 = timeout;

    ASSERT(timeout > 0);

    //If interrupts are not used, poll for flags
    if (!BITBAND_PERI(EUSCI_B_CMSIS(moduleInstance)->rIE.r, UCTXIE_OFS))
    {
        //Poll for transmit interrupt flag.
        while ((!BITBAND_PERI(EUSCI_B_CMSIS(moduleInstance)->rIFG.r,
                UCTXIFG_OFS)) && --timeout)
            ;

        //Check if transfer timed out
        if (timeout == 0)
            return false;
    }

    //Send single byte data.
    EUSCI_B_CMSIS(moduleInstance)->rTXBUF.r = txData;

    //Poll for transmit interrupt flag.
    while ((!BITBAND_PERI(EUSCI_B_CMSIS(moduleInstance)->rIFG.r, UCTXIFG_OFS))
            && --timeout2)
        ;

    //Check if transfer timed out
    if (timeout2 == 0)
        return false;

    //Send stop condition.
    BITBAND_PERI(EUSCI_B_CMSIS(moduleInstance)->rCTLW0.r,UCTXSTP_OFS) = 1;

    return true;
}


void I2C_masterSendMultiByteStop(uint32_t moduleInstance)
{
    //If interrupts are not used, poll for flags
    if (!BITBAND_PERI(EUSCI_B_CMSIS(moduleInstance)->rIE.r, UCTXIE_OFS))
    {
        //Poll for transmit interrupt flag.
        while
            (!BITBAND_PERI(EUSCI_B_CMSIS(moduleInstance)->rIFG.r, UCTXIFG_OFS))
            ;
    }

    //Send stop condition.
    BITBAND_PERI(EUSCI_B_CMSIS(moduleInstance)->rCTLW0.r,UCTXSTP_OFS) = 1;
}


bool I2C_masterSendMultiByteStopWithTimeout(uint32_t moduleInstance,
        uint32_t timeout)
{
    ASSERT(timeout > 0);

    //If interrupts are not used, poll for flags
    if (!BITBAND_PERI(EUSCI_B_CMSIS(moduleInstance)->rIE.r, UCTXIE_OFS))
    {
        //Poll for transmit interrupt flag.
        while ((!BITBAND_PERI(EUSCI_B_CMSIS(moduleInstance)->rIFG.r,
                UCTXIFG_OFS)) && --timeout)
            ;

        //Check if transfer timed out
        if (timeout == 0)
            return false;
    }

    //Send stop condition.
    BITBAND_PERI(EUSCI_B_CMSIS(moduleInstance)->rCTLW0.r,UCTXSTP_OFS) = 1;

    return 0x01;
}


void I2C_masterReceiveStart(uint32_t moduleInstance)
{
    //Set USCI in Receive mode
    EUSCI_B_CMSIS(moduleInstance)->rCTLW0.r =
            (EUSCI_B_CMSIS(moduleInstance)->rCTLW0.r & (~UCTR)) | UCTXSTT;
}


uint8_t I2C_masterReceiveMultiByteNext(uint32_t moduleInstance)
{
    return EUSCI_B_CMSIS(moduleInstance)->rRXBUF.a.bRXBUF;
}


uint8_t I2C_masterReceiveMultiByteFinish(uint32_t moduleInstance)
{
    //Send stop condition.
    BITBAND_PERI(EUSCI_B_CMSIS(moduleInstance)->rCTLW0.r,UCTXSTP_OFS) = 1;

    //Wait for Stop to finish
    while (BITBAND_PERI(EUSCI_B_CMSIS(moduleInstance)->rCTLW0.r, UCTXSTP_OFS))
    {
        // Wait for RX buffer
        while (!BITBAND_PERI(EUSCI_B_CMSIS(moduleInstance)->rIFG, UCRXIFG_OFS))
            ;
    }

    /* Capture data from receive buffer after setting stop bit due to
     MSP430 I2C critical timing. */
    return EUSCI_B_CMSIS(moduleInstance)->rRXBUF.b.bRXBUF;
}


bool I2C_masterReceiveMultiByteFinishWithTimeout(uint32_t moduleInstance,
        uint8_t *txData, uint32_t timeout)
{
    uint32_t timeout2 = timeout;

    ASSERT(timeout > 0);

    //Send stop condition.
    BITBAND_PERI(EUSCI_B_CMSIS(moduleInstance)->rCTLW0.r,UCTXSTP_OFS) = 1;

    //Wait for Stop to finish
    while (BITBAND_PERI(EUSCI_B_CMSIS(moduleInstance)->rCTLW0.r, UCTXSTP_OFS)
            && --timeout)
        ;

    //Check if transfer timed out
    if (timeout == 0)
        return false;

    // Wait for RX buffer
    while ((!BITBAND_PERI(EUSCI_B_CMSIS(moduleInstance)->rIFG.r, UCRXIFG_OFS))
            && --timeout2)
        ;

    //Check if transfer timed out
    if (timeout2 == 0)
        return false;

    //Capture data from receive buffer after setting stop bit due to
    //MSP430 I2C critical timing.
    *txData = (EUSCI_B_CMSIS(moduleInstance)->rRXBUF.b.bRXBUF);

    return true;
}


void I2C_masterReceiveMultiByteStop(uint32_t moduleInstance)
{
    //Send stop condition.
    BITBAND_PERI(EUSCI_B_CMSIS(moduleInstance)->rCTLW0.r,UCTXSTP_OFS) = 1;
}


uint8_t I2C_masterReceiveSingleByte(uint32_t moduleInstance)
{
    //Set USCI in Receive mode
    BITBAND_PERI(EUSCI_B_CMSIS(moduleInstance)->rCTLW0.r,UCTR_OFS) = 0;

    //Send start
    EUSCI_B_CMSIS(moduleInstance)->rCTLW0.r |= (UCTXSTT + UCTXSTP);

    //Poll for receive interrupt flag.
    while (!BITBAND_PERI(EUSCI_B_CMSIS(moduleInstance)->rIFG.r, UCRXIFG_OFS))
        ;

    //Send single byte data.
    return EUSCI_B_CMSIS(moduleInstance)->rRXBUF.b.bRXBUF;
}


uint8_t I2C_masterReceiveSingle(uint32_t moduleInstance)
{
    //Polling RXIFG0 if RXIE is not enabled
    if (!BITBAND_PERI(EUSCI_B_CMSIS(moduleInstance)->rIE.r, UCRXIE0_OFS))
    {
        while (!BITBAND_PERI(EUSCI_B_CMSIS(moduleInstance)->rIFG.r,
                UCRXIFG0_OFS))
            ;
    }

    //Read a byte.
    return EUSCI_B_CMSIS(moduleInstance)->rRXBUF.b.bRXBUF;
}


uint32_t I2C_getReceiveBufferAddressForDMA(uint32_t moduleInstance)
{
    return moduleInstance + OFS_UCB0RXBUF;
}


uint32_t I2C_getTransmitBufferAddressForDMA(uint32_t moduleInstance)
{
    return moduleInstance + OFS_UCB0TXBUF;
}


uint8_t I2C_masterIsStopSent(uint32_t moduleInstance)
{
    return BITBAND_PERI(EUSCI_B_CMSIS(moduleInstance)->rCTLW0.r, UCTXSTP_OFS);
}


bool I2C_masterIsStartSent(uint32_t moduleInstance)
{
    return BITBAND_PERI(EUSCI_B_CMSIS(moduleInstance)->rCTLW0.r, UCTXSTT_OFS);
}


void I2C_masterSendStart(uint32_t moduleInstance)
{
    BITBAND_PERI(EUSCI_B_CMSIS(moduleInstance)->rCTLW0.r,UCTXSTT_OFS) = 1;
}


void I2C_enableMultiMasterMode(uint32_t moduleInstance)
{
    BITBAND_PERI(EUSCI_B_CMSIS(moduleInstance)->rCTLW0.r,UCSWRST_OFS) = 1;
    BITBAND_PERI(EUSCI_B_CMSIS(moduleInstance)->rCTLW0.r,UCMM_OFS) = 1;
}


void I2C_disableMultiMasterMode(uint32_t moduleInstance)
{
    BITBAND_PERI(EUSCI_B_CMSIS(moduleInstance)->rCTLW0.r,UCSWRST_OFS) = 1;
    BITBAND_PERI(EUSCI_B_CMSIS(moduleInstance)->rCTLW0.r,UCMM_OFS) = 0;
}


uint_fast16_t I2C_getMode(uint32_t moduleInstance)
{
    //Read the I2C mode.
    return (EUSCI_B_CMSIS(moduleInstance)->rCTLW0.r & UCTR);
}


void I2C_enableInterrupt(uint32_t moduleInstance, uint_fast16_t mask)
{
    ASSERT(
            0x00
            == (mask
                    & ~(EUSCI_B_I2C_STOP_INTERRUPT
                            + EUSCI_B_I2C_START_INTERRUPT
                            + EUSCI_B_I2C_NAK_INTERRUPT
                            + EUSCI_B_I2C_ARBITRATIONLOST_INTERRUPT
                            + EUSCI_B_I2C_BIT9_POSITION_INTERRUPT
                            + EUSCI_B_I2C_CLOCK_LOW_TIMEOUT_INTERRUPT
                            + EUSCI_B_I2C_BYTE_COUNTER_INTERRUPT
                            + EUSCI_B_I2C_TRANSMIT_INTERRUPT0
                            + EUSCI_B_I2C_TRANSMIT_INTERRUPT1
                            + EUSCI_B_I2C_TRANSMIT_INTERRUPT2
                            + EUSCI_B_I2C_TRANSMIT_INTERRUPT3
                            + EUSCI_B_I2C_RECEIVE_INTERRUPT0
                            + EUSCI_B_I2C_RECEIVE_INTERRUPT1
                            + EUSCI_B_I2C_RECEIVE_INTERRUPT2
                            + EUSCI_B_I2C_RECEIVE_INTERRUPT3)));

    //Enable the interrupt masked bit
    EUSCI_B_CMSIS(moduleInstance)->rIE.r |= mask;
}


void I2C_disableInterrupt(uint32_t moduleInstance, uint_fast16_t mask)
{
    ASSERT(
            0x00
            == (mask
                    & ~(EUSCI_B_I2C_STOP_INTERRUPT
                            + EUSCI_B_I2C_START_INTERRUPT
                            + EUSCI_B_I2C_NAK_INTERRUPT
                            + EUSCI_B_I2C_ARBITRATIONLOST_INTERRUPT
                            + EUSCI_B_I2C_BIT9_POSITION_INTERRUPT
                            + EUSCI_B_I2C_CLOCK_LOW_TIMEOUT_INTERRUPT
                            + EUSCI_B_I2C_BYTE_COUNTER_INTERRUPT
                            + EUSCI_B_I2C_TRANSMIT_INTERRUPT0
                            + EUSCI_B_I2C_TRANSMIT_INTERRUPT1
                            + EUSCI_B_I2C_TRANSMIT_INTERRUPT2
                            + EUSCI_B_I2C_TRANSMIT_INTERRUPT3
                            + EUSCI_B_I2C_RECEIVE_INTERRUPT0
                            + EUSCI_B_I2C_RECEIVE_INTERRUPT1
                            + EUSCI_B_I2C_RECEIVE_INTERRUPT2
                            + EUSCI_B_I2C_RECEIVE_INTERRUPT3)));

    //Disable the interrupt masked bit
    EUSCI_B_CMSIS(moduleInstance)->rIE.r &= ~(mask);
}


void I2C_clearInterruptFlag(uint32_t moduleInstance, uint_fast16_t mask)
{
    ASSERT(
            0x00
            == (mask
                    & ~(EUSCI_B_I2C_STOP_INTERRUPT
                            + EUSCI_B_I2C_START_INTERRUPT
                            + EUSCI_B_I2C_NAK_INTERRUPT
                            + EUSCI_B_I2C_ARBITRATIONLOST_INTERRUPT
                            + EUSCI_B_I2C_BIT9_POSITION_INTERRUPT
                            + EUSCI_B_I2C_CLOCK_LOW_TIMEOUT_INTERRUPT
                            + EUSCI_B_I2C_BYTE_COUNTER_INTERRUPT
                            + EUSCI_B_I2C_TRANSMIT_INTERRUPT0
                            + EUSCI_B_I2C_TRANSMIT_INTERRUPT1
                            + EUSCI_B_I2C_TRANSMIT_INTERRUPT2
                            + EUSCI_B_I2C_TRANSMIT_INTERRUPT3
                            + EUSCI_B_I2C_RECEIVE_INTERRUPT0
                            + EUSCI_B_I2C_RECEIVE_INTERRUPT1
                            + EUSCI_B_I2C_RECEIVE_INTERRUPT2
                            + EUSCI_B_I2C_RECEIVE_INTERRUPT3)));
    //Clear the I2C interrupt source.
    EUSCI_B_CMSIS(moduleInstance)->rIFG.r &= ~(mask);
}


uint_fast16_t I2C_getInterruptStatus(uint32_t moduleInstance, uint16_t mask)
{
    ASSERT(
            0x00
            == (mask
                    & ~(EUSCI_B_I2C_STOP_INTERRUPT
                            + EUSCI_B_I2C_START_INTERRUPT
                            + EUSCI_B_I2C_NAK_INTERRUPT
                            + EUSCI_B_I2C_ARBITRATIONLOST_INTERRUPT
                            + EUSCI_B_I2C_BIT9_POSITION_INTERRUPT
                            + EUSCI_B_I2C_CLOCK_LOW_TIMEOUT_INTERRUPT
                            + EUSCI_B_I2C_BYTE_COUNTER_INTERRUPT
                            + EUSCI_B_I2C_TRANSMIT_INTERRUPT0
                            + EUSCI_B_I2C_TRANSMIT_INTERRUPT1
                            + EUSCI_B_I2C_TRANSMIT_INTERRUPT2
                            + EUSCI_B_I2C_TRANSMIT_INTERRUPT3
                            + EUSCI_B_I2C_RECEIVE_INTERRUPT0
                            + EUSCI_B_I2C_RECEIVE_INTERRUPT1
                            + EUSCI_B_I2C_RECEIVE_INTERRUPT2
                            + EUSCI_B_I2C_RECEIVE_INTERRUPT3)));
    //Return the interrupt status of the request masked bit.
    return EUSCI_B_CMSIS(moduleInstance)->rIFG.r & mask;
}


uint_fast16_t I2C_getEnabledInterruptStatus(uint32_t moduleInstance)
{
    return I2C_getInterruptStatus(moduleInstance,
    EUSCI_B_CMSIS(moduleInstance)->rIE.r);
}


//*****************************************************************************
//
// This is a mapping between priority grouping encodings and the number of
// preemption priority bits.
//
//*****************************************************************************
static const uint32_t g_pulPriority[] =
{ NVIC_APINT_PRIGROUP_0_8, NVIC_APINT_PRIGROUP_1_7, NVIC_APINT_PRIGROUP_2_6,
NVIC_APINT_PRIGROUP_3_5, NVIC_APINT_PRIGROUP_4_4,
NVIC_APINT_PRIGROUP_5_3, NVIC_APINT_PRIGROUP_6_2,
NVIC_APINT_PRIGROUP_7_1 };


//*****************************************************************************
//
// This is a mapping between interrupt number and the register that contains
// the priority encoding for that interrupt.
//
//*****************************************************************************
static const uint32_t g_pulRegs[] =
{ 0, NVIC_SYS_PRI1_R, NVIC_SYS_PRI2_R, NVIC_SYS_PRI3_R, NVIC_PRI0_R,
NVIC_PRI1_R, NVIC_PRI2_R, NVIC_PRI3_R, NVIC_PRI4_R, NVIC_PRI5_R,
NVIC_PRI6_R, NVIC_PRI7_R, NVIC_PRI8_R, NVIC_PRI9_R, NVIC_PRI10_R,
NVIC_PRI11_R, NVIC_PRI12_R, NVIC_PRI13_R, NVIC_PRI14_R, NVIC_PRI15_R };


//*****************************************************************************
//
// This is a mapping between interrupt number (for the peripheral interrupts
// only) and the register that contains the interrupt enable for that
// interrupt.
//
//*****************************************************************************
static const uint32_t g_pulEnRegs[] =
{ NVIC_EN0_R, NVIC_EN1_R };


//*****************************************************************************
//
// This is a mapping between interrupt number (for the peripheral interrupts
// only) and the register that contains the interrupt disable for that
// interrupt.
//
//*****************************************************************************
static const uint32_t g_pulDisRegs[] =
{ NVIC_DIS0_R, NVIC_DIS1_R };


//*****************************************************************************
//
// This is a mapping between interrupt number (for the peripheral interrupts
// only) and the register that contains the interrupt pend for that interrupt.
//
//*****************************************************************************
static const uint32_t g_pulPendRegs[] =
{ NVIC_PEND0_R, NVIC_PEND1_R };


//*****************************************************************************
//
// This is a mapping between interrupt number (for the peripheral interrupts
// only) and the register that contains the interrupt unpend for that
// interrupt.
//
//*****************************************************************************
static const uint32_t g_pulUnpendRegs[] =
{ NVIC_UNPEND0_R, NVIC_UNPEND1_R };


bool Interrupt_enableMaster(void)
{
    //
    // Enable processor interrupts.
    //
    return (CPU_cpsie());
}


bool Interrupt_disableMaster(void)
{
    //
    // Disable processor interrupts.
    //
    return (CPU_cpsid());
}


void Interrupt_setPriorityGrouping(uint32_t bits)
{
    //
    // Check the arguments.
    //
    ASSERT(bits < NUM_PRIORITY);

    //
    // Set the priority grouping.
    //
    SCB->AIRCR = SCB_AIRCR_VECTKEY_M | g_pulPriority[bits];
}


uint32_t Interrupt_getPriorityGrouping(void)
{
    uint32_t ulLoop, ulValue;

    //
    // Read the priority grouping.
    //
    ulValue = SCB->AIRCR & NVIC_APINT_PRIGROUP_M;

    //
    // Loop through the priority grouping values.
    //
    for (ulLoop = 0; ulLoop < NUM_PRIORITY; ulLoop++)
    {
        //
        // Stop looping if this value matches.
        //
        if (ulValue == g_pulPriority[ulLoop])
        {
            break;
        }
    }

    //
    // Return the number of priority bits.
    //
    return (ulLoop);
}


void Interrupt_setPriority(uint32_t interruptNumber, uint8_t priority)
{
    uint32_t ulTemp;

    //
    // Check the arguments.
    //
    ASSERT((interruptNumber >= 4) && (interruptNumber < (NUM_INTERRUPTS+1)));

    //
    // Set the interrupt priority.
    //
    ulTemp = HWREG32(g_pulRegs[interruptNumber >> 2]);
    ulTemp &= ~(0xFF << (8 * (interruptNumber & 3)));
    ulTemp |= priority << (8 * (interruptNumber & 3));
    HWREG32 (g_pulRegs[interruptNumber >> 2]) = ulTemp;
}


uint8_t Interrupt_getPriority(uint32_t interruptNumber)
{
    //
    // Check the arguments.
    //
    ASSERT((interruptNumber >= 4) && (interruptNumber < (NUM_INTERRUPTS+1)));

    //
    // Return the interrupt priority.
    //
    return ((HWREG32(g_pulRegs[interruptNumber >> 2])
            >> (8 * (interruptNumber & 3))) & 0xFF);
}


void Interrupt_enableInterrupt(uint32_t interruptNumber)
{
    //
    // Check the arguments.
    //
    ASSERT(interruptNumber < (NUM_INTERRUPTS+1));

    //
    // Determine the interrupt to enable.
    //
    if (interruptNumber == FAULT_MPU)
    {
        //
        // Enable the MemManage interrupt.
        //
        SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA;
    } else if (interruptNumber == FAULT_BUS)
    {
        //
        // Enable the bus fault interrupt.
        //
        SCB->SHCSR |= SCB_SHCSR_BUSFAULTENA;
    } else if (interruptNumber == FAULT_USAGE)
    {
        //
        // Enable the usage fault interrupt.
        //
        SCB->SHCSR |= SCB_SHCSR_USGFAULTENA;
    } else if (interruptNumber == FAULT_SYSTICK)
    {
        //
        // Enable the System Tick interrupt.
        //
        SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    } else if (interruptNumber >= 16)
    {
        //
        // Enable the general interrupt.
        //
        HWREG32 (g_pulEnRegs[(interruptNumber - 16) / 32]) = 1
                << ((interruptNumber - 16) & 31);
    }
}


void Interrupt_disableInterrupt(uint32_t interruptNumber)
{
    //
    // Check the arguments.
    //
    ASSERT(interruptNumber < (NUM_INTERRUPTS+1));

    //
    // Determine the interrupt to disable.
    //
    if (interruptNumber == FAULT_MPU)
    {
        //
        // Disable the MemManage interrupt.
        //
        SCB->SHCSR &= ~(SCB_SHCSR_MEMFAULTENA);
    } else if (interruptNumber == FAULT_BUS)
    {
        //
        // Disable the bus fault interrupt.
        //
        SCB->SHCSR &= ~(SCB_SHCSR_BUSFAULTENA);
    } else if (interruptNumber == FAULT_USAGE)
    {
        //
        // Disable the usage fault interrupt.
        //
        SCB->SHCSR &= ~(SCB_SHCSR_USGFAULTENA);
    } else if (interruptNumber == FAULT_SYSTICK)
    {
        //
        // Disable the System Tick interrupt.
        //
        SysTick->CTRL &= ~(SysTick_CTRL_ENABLE_Msk);
    } else if (interruptNumber >= 16)
    {
        //
        // Disable the general interrupt.
        //
        HWREG32 (g_pulDisRegs[(interruptNumber - 16) / 32]) = 1
                << ((interruptNumber - 16) & 31);
    }
}


bool Interrupt_isEnabled(uint32_t interruptNumber)
{
    uint32_t ulRet;

    //
    // Check the arguments.
    //
    ASSERT(interruptNumber < (NUM_INTERRUPTS+1));

    //
    // Initialize the return value.
    //
    ulRet = 0;

    //
    // Determine the interrupt to disable.
    //
    if (interruptNumber == FAULT_MPU)
    {
        //
        // Check the MemManage interrupt.
        //
        ulRet = SCB->SHCSR & SCB_SHCSR_MEMFAULTENA;
    } else if (interruptNumber == FAULT_BUS)
    {
        //
        // Check the bus fault interrupt.
        //
        ulRet = SCB->SHCSR & SCB_SHCSR_BUSFAULTENA;
    } else if (interruptNumber == FAULT_USAGE)
    {
        //
        // Check the usage fault interrupt.
        //
        ulRet = SCB->SHCSR & SCB_SHCSR_USGFAULTENA;
    } else if (interruptNumber == FAULT_SYSTICK)
    {
        //
        // Check the System Tick interrupt.
        //
        ulRet = SysTick->CTRL & SysTick_CTRL_ENABLE_Msk;
    } else if (interruptNumber >= 16)
    {
        //
        // Check the general interrupt.
        //
        ulRet = HWREG32(g_pulEnRegs[(interruptNumber - 16) / 32])
                & (1 << ((interruptNumber - 16) & 31));
    }
    return (ulRet);
}


void Interrupt_pendInterrupt(uint32_t interruptNumber)
{
    //
    // Check the arguments.
    //
    ASSERT(interruptNumber < (NUM_INTERRUPTS+1));

    //
    // Determine the interrupt to pend.
    //
    if (interruptNumber == FAULT_NMI)
    {
        //
        // Pend the NMI interrupt.
        //
        SCB->ICSR |= SCB_ICSR_NMIPENDSET;
    } else if (interruptNumber == FAULT_PENDSV)
    {
        //
        // Pend the PendSV interrupt.
        //
        SCB->ICSR |= SCB_ICSR_PENDSVSET;
    } else if (interruptNumber == FAULT_SYSTICK)
    {
        //
        // Pend the SysTick interrupt.
        //
        SCB->ICSR |= SCB_ICSR_PENDSTSET;
    } else if (interruptNumber >= 16)
    {
        //
        // Pend the general interrupt.
        //
        HWREG32 (g_pulPendRegs[(interruptNumber - 16) / 32]) = 1
                << ((interruptNumber - 16) & 31);
    }
}


void Interrupt_unpendInterrupt(uint32_t interruptNumber)
{
    //
    // Check the arguments.
    //
    ASSERT(interruptNumber < (NUM_INTERRUPTS+1));

    //
    // Determine the interrupt to unpend.
    //
    if (interruptNumber == FAULT_PENDSV)
    {
        //
        // Unpend the PendSV interrupt.
        //
        SCB->ICSR |= SCB_ICSR_PENDSVCLR;
    } else if (interruptNumber == FAULT_SYSTICK)
    {
        //
        // Unpend the SysTick interrupt.
        //
        SCB->ICSR |= SCB_ICSR_PENDSTCLR;
    } else if (interruptNumber >= 16)
    {
        //
        // Unpend the general interrupt.
        //
        HWREG32 (g_pulUnpendRegs[(interruptNumber - 16) / 32]) = 1
                << ((interruptNumber - 16) & 31);
    }
}


void Interrupt_setVectorTableAddress(uint32_t addr)
{
    SCB->VTOR = addr;
}


uint32_t Interrupt_getVectorTableAddress(void)
{
    return SCB->VTOR;
}


void Interrupt_setPriorityMask(uint8_t priorityMask)
{
    CPU_basepriSet(priorityMask);
}


void Interrupt_enableSleepOnIsrExit(void)
{
    SCB->SCR |= SCB_SCR_SLEEPONEXIT;
}


void Interrupt_disableSleepOnIsrExit(void)
{
    SCB->SCR &= ~SCB_SCR_SLEEPONEXIT;
}


uint8_t Interrupt_getPriorityMask(void)
{
    return (CPU_basepriGet());
}


void MPU_enableModule(uint32_t mpuConfig)
{
    //
    // Check the arguments.
    //
    ASSERT(!(mpuConfig & ~(MPU_CONFIG_PRIV_DEFAULT | MPU_CONFIG_HARDFLT_NMI)));

    //
    // Set the MPU control bits according to the flags passed by the user,
    // and also set the enable bit.
    //
    MPU->CTRL = mpuConfig | MPU_CTRL_ENABLE;
}


void MPU_disableModule(void)
{
    //
    // Turn off the MPU enable bit.
    //
    MPU->CTRL &= ~MPU_CTRL_ENABLE;

}


uint32_t MPU_getRegionCount(void)
{
    //
    // Read the DREGION field of the MPU type register and mask off
    // the bits of interest to get the count of regions.
    //
    return ((MPU->TYPE & MPU_TYPE_DREGION_M) >> NVIC_MPU_TYPE_DREGION_S);
}


void MPU_enableRegion(uint32_t region)
{
    //
    // Check the arguments.
    //
    ASSERT(region < 8);

    //
    // Select the region to modify.
    //
    MPU->RNR = region;

    //
    // Modify the enable bit in the region attributes.
    //
    MPU->RASR |= MPU_RASR_ENABLE;
}


void MPU_disableRegion(uint32_t region)
{
    //
    // Check the arguments.
    //
    ASSERT(region < 8);

    //
    // Select the region to modify.
    //
    MPU->RNR = region;

    //
    // Modify the enable bit in the region attributes.
    //
    MPU->RASR &= ~MPU_RASR_ENABLE;
}


void MPU_setRegion(uint32_t region, uint32_t addr, uint32_t flags)
{
    //
    // Check the arguments.
    //
    ASSERT(region < 8);

    //
    // Program the base address, use the region field to select the
    // region at the same time.
    //
    MPU->RBAR = addr | region | MPU_RBAR_VALID;

    //
    // Program the region attributes.  Set the TEX field and the S, C,
    // and B bits to fixed values that are suitable for all Stellaris
    // memory.
    //
    MPU->RASR = (flags & ~(MPU_RASR_TEX_M | MPU_RASR_C)) | MPU_RASR_S
            | MPU_RASR_B;
}


void MPU_enableInterrupt(void)
{

    //
    // Enable the memory management fault.
    //
    Interrupt_enableInterrupt(FAULT_MPU);

}


void MPU_disableInterrupt(void)
{
    //
    // Disable the interrupt.
    //
    Interrupt_disableInterrupt(FAULT_MPU);
}


void MPU_getRegion(uint32_t region, uint32_t *addr, uint32_t *pflags)
{
    //
    // Check the arguments.
    //
    ASSERT(region < 8);
    ASSERT(addr);
    ASSERT(pflags);

    //
    // Select the region to get.
    //
    MPU->RNR = region;

    //
    // Read and store the base address for the region.
    //
    *addr = MPU->RBAR & MPU_RBAR_ADDR_M;

    //
    // Read and store the region attributes.
    //
    *pflags = MPU->RASR;
}


void PMAP_configurePorts(const uint8_t *portMapping, uint8_t pxMAPy,
        uint8_t numberOfPorts, uint8_t portMapReconfigure)
{
      uint_fast16_t i;

    ASSERT(
            (portMapReconfigure == PMAP_ENABLE_RECONFIGURATION)
                    || (portMapReconfigure == PMAP_DISABLE_RECONFIGURATION));

    //Get write-access to port mapping registers:
    PMAP->rKEYID = PMAP_KEYID_VAL;

    //Enable/Disable reconfiguration during runtime
    PMAP->rCTL.r = (PMAP->rCTL.r & ~PMAPRECFG) | portMapReconfigure;

    //Configure Port Mapping:
    
    for (i = 0; i < numberOfPorts * 8; i++)
    {
        HWREG8(PMAP_BASE + i + pxMAPy) = portMapping[i];
    }

    //Disable write-access to port mapping registers:
    PMAP->rKEYID = 0;
}


static bool __PCM_setCoreVoltageLevelAdvanced(uint_fast8_t voltageLevel,
        uint32_t timeOut, bool blocking)
{
    uint8_t powerMode, bCurrentVoltageLevel;
    uint32_t regValue;
    bool boolTimeout;

    ASSERT(voltageLevel == PCM_VCORE0 || voltageLevel == PCM_VCORE1);

    /* Getting current power mode and level */
    powerMode = PCM_getPowerMode();
    bCurrentVoltageLevel = PCM_getCoreVoltageLevel();

    boolTimeout = timeOut > 0 ? true : false;

    /* If we are already at the power mode they requested, return */
    if (bCurrentVoltageLevel == voltageLevel)
        return true;

    while (bCurrentVoltageLevel != voltageLevel)
    {
        regValue = PCM->rCTL0.r;

        switch (PCM_getPowerState())
        {
        case PCM_AM_LF_VCORE1:
        case PCM_AM_DCDC_VCORE1:
        case PCM_AM_LDO_VCORE0:
            PCM->rCTL0.r = (PCM_KEY | (PCM_AM_LDO_VCORE1)
                    | (regValue & ~(PCMKEY_M | AMR_M)));
            break;
        case PCM_AM_LF_VCORE0:
        case PCM_AM_DCDC_VCORE0:
        case PCM_AM_LDO_VCORE1:
            PCM->rCTL0.r = (PCM_KEY | (PCM_AM_LDO_VCORE0)
                    | (regValue & ~(PCMKEY_M | AMR_M)));
            break;
        default:
            ASSERT(false);
        }

        if(blocking)
        {
            while (BITBAND_PERI(PCM->rCTL1.r, PMR_BUSY_OFS))
            {
                if (boolTimeout && !(--timeOut))
                    return false;

            }
        }
        else
        {
            return true;
        }

        bCurrentVoltageLevel = PCM_getCoreVoltageLevel();
    }

    /* Changing the power mode if we are stuck in LDO mode */
    if (powerMode != PCM_getPowerMode())
    {
        if (powerMode == PCM_DCDC_MODE)
            return PCM_setPowerMode(PCM_DCDC_MODE);
        else
            return PCM_setPowerMode(PCM_LF_MODE);
    }

    return true;

}


bool PCM_setCoreVoltageLevelNonBlocking(uint_fast8_t voltageLevel)
{
    return __PCM_setCoreVoltageLevelAdvanced(voltageLevel, 0, false);
}


bool PCM_setCoreVoltageLevel(uint_fast8_t voltageLevel)
{
    return __PCM_setCoreVoltageLevelAdvanced(voltageLevel, 0, true);
}


uint8_t PCM_getCoreVoltageLevel(void)
{
    uint8_t currentPowerState = PCM_getPowerState();

    switch (currentPowerState)
    {
    case PCM_AM_LDO_VCORE0:
    case PCM_AM_DCDC_VCORE0:
    case PCM_AM_LF_VCORE0:
    case PCM_LPM0_LDO_VCORE0:
    case PCM_LPM0_DCDC_VCORE0:
    case PCM_LPM0_LF_VCORE0:
        return PCM_VCORE0;
    case PCM_AM_LDO_VCORE1:
    case PCM_AM_DCDC_VCORE1:
    case PCM_AM_LF_VCORE1:
    case PCM_LPM0_LDO_VCORE1:
    case PCM_LPM0_DCDC_VCORE1:
    case PCM_LPM0_LF_VCORE1:
        return PCM_VCORE1;
    case PCM_LPM3:
        return PCM_VCORELPM3;
    default:
        ASSERT(false);
        return false;

    }
}


bool PCM_setCoreVoltageLevelWithTimeout(uint_fast8_t voltageLevel,
        uint32_t timeOut)
{
    return __PCM_setCoreVoltageLevelAdvanced(voltageLevel, timeOut, true);
}


static bool __PCM_setPowerModeAdvanced(uint_fast8_t powerMode, uint32_t timeOut,
bool blocking)
{
    uint8_t bCurrentPowerMode, bCurrentPowerState;
    uint32_t regValue;
    bool boolTimeout;

    ASSERT(
            powerMode == PCM_LDO_MODE || powerMode == PCM_DCDC_MODE
            || powerMode == PCM_LF_MODE);

    /* Getting Current Power Mode */
    bCurrentPowerMode = PCM_getPowerMode();

    /* If the power mode being set it the same as the current mode, return */
    if (powerMode == bCurrentPowerMode)
        return true;

    bCurrentPowerState = PCM_getPowerState();

    boolTimeout = timeOut > 0 ? true : false;

    /* Go through the while loop while we haven't achieved the power mode */
    while (bCurrentPowerMode != powerMode)
    {
        regValue = PCM->rCTL0.r;

        switch (bCurrentPowerState)
        {
        case PCM_AM_DCDC_VCORE0:
        case PCM_AM_LF_VCORE0:
            PCM->rCTL0.r = (PCM_KEY | PCM_AM_LDO_VCORE0
                    | (regValue & ~(PCMKEY_M | AMR_M)));
            break;
        case PCM_AM_LF_VCORE1:
        case PCM_AM_DCDC_VCORE1:
            PCM->rCTL0.r = (PCM_KEY | PCM_AM_LDO_VCORE1
                    | (regValue & ~(PCMKEY_M | AMR_M)));
            break;
        case PCM_AM_LDO_VCORE1:
        {
            if (powerMode == PCM_DCDC_MODE)
            {
                PCM->rCTL0.r = (PCM_KEY | PCM_AM_DCDC_VCORE1
                        | (regValue & ~(PCMKEY_M | AMR_M)));
            } else if (powerMode == PCM_LF_MODE)
            {
                PCM->rCTL0.r = (PCM_KEY | PCM_AM_LF_VCORE1
                        | (regValue & ~(PCMKEY_M | AMR_M)));
            } else
                ASSERT(false);

            break;
        }
        case PCM_AM_LDO_VCORE0:
        {
            if (powerMode == PCM_DCDC_MODE)
            {
                PCM->rCTL0.r = (PCM_KEY | PCM_AM_DCDC_VCORE0
                        | (regValue & ~(PCMKEY_M | AMR_M)));
            } else if (powerMode == PCM_LF_MODE)
            {
                PCM->rCTL0.r = (PCM_KEY | PCM_AM_LF_VCORE0
                        | (regValue & ~(PCMKEY_M | AMR_M)));
            } else
                ASSERT(false);

            break;
        }
        default:
            ASSERT(false);
        }

        if (blocking)
        {
            while (BITBAND_PERI(PCM->rCTL1.r, PMR_BUSY_OFS))
            {
                if (boolTimeout && !(--timeOut))
                    return false;

            }
        } else
            return true;

        bCurrentPowerMode = PCM_getPowerMode();
        bCurrentPowerState = PCM_getPowerState();
    }

    return true;

}


bool PCM_setPowerModeNonBlocking(uint_fast8_t powerMode)
{
    return __PCM_setPowerModeAdvanced(powerMode, 0, false);
}


bool PCM_setPowerMode(uint_fast8_t powerMode)
{
    return __PCM_setPowerModeAdvanced(powerMode, 0, true);
}


bool PCM_setPowerModeWithTimeout(uint_fast8_t powerMode, uint32_t timeOut)
{
    return __PCM_setPowerModeAdvanced(powerMode, timeOut, true);
}


uint8_t PCM_getPowerMode(void)
{
    uint8_t currentPowerState;

    currentPowerState = PCM_getPowerState();

    switch (currentPowerState)
    {
    case PCM_AM_LDO_VCORE0:
    case PCM_AM_LDO_VCORE1:
    case PCM_LPM0_LDO_VCORE0:
    case PCM_LPM0_LDO_VCORE1:
        return PCM_LDO_MODE;
    case PCM_AM_DCDC_VCORE0:
    case PCM_AM_DCDC_VCORE1:
    case PCM_LPM0_DCDC_VCORE0:
    case PCM_LPM0_DCDC_VCORE1:
        return PCM_DCDC_MODE;
    case PCM_LPM0_LF_VCORE0:
    case PCM_LPM0_LF_VCORE1:
    case PCM_AM_LF_VCORE1:
    case PCM_AM_LF_VCORE0:
        return PCM_LF_MODE;
    default:
        ASSERT(false);
        return false;

    }
}


static bool __PCM_setPowerStateAdvanced(uint_fast8_t powerState,
        uint32_t timeout,
        bool blocking)
{
    uint8_t bCurrentPowerState;
    bCurrentPowerState = PCM_getPowerState();

    ASSERT(
            powerState == PCM_AM_LDO_VCORE0 || powerState == PCM_AM_LDO_VCORE1
            || powerState == PCM_AM_DCDC_VCORE0 || powerState == PCM_AM_DCDC_VCORE1
            || powerState == PCM_AM_LF_VCORE0 || powerState == PCM_AM_LF_VCORE1
            || powerState == PCM_LPM0_LDO_VCORE0 || powerState == PCM_LPM0_LDO_VCORE1
            || powerState == PCM_LPM0_DCDC_VCORE0 || powerState == PCM_LPM0_DCDC_VCORE1
            || powerState == PCM_LPM3 || powerState == PCM_LPM35_VCORE0
            || powerState == PCM_LPM45 || powerState == PCM_LPM4);

    if (bCurrentPowerState == powerState)
        return true;

    switch (powerState)
    {
    case PCM_AM_LDO_VCORE0:
        return (__PCM_setCoreVoltageLevelAdvanced(PCM_VCORE0, timeout, blocking)
                && __PCM_setPowerModeAdvanced(PCM_LDO_MODE, timeout, blocking));
    case PCM_AM_LDO_VCORE1:
        return (__PCM_setCoreVoltageLevelAdvanced(PCM_VCORE1, timeout, blocking)
                && __PCM_setPowerModeAdvanced(PCM_LDO_MODE, timeout, blocking));
    case PCM_AM_DCDC_VCORE0:
        return (__PCM_setCoreVoltageLevelAdvanced(PCM_VCORE0, timeout, blocking)
                && __PCM_setPowerModeAdvanced(PCM_DCDC_MODE, timeout, blocking));
    case PCM_AM_DCDC_VCORE1:
        return (__PCM_setCoreVoltageLevelAdvanced(PCM_VCORE1, timeout, blocking)
                && __PCM_setPowerModeAdvanced(PCM_DCDC_MODE, timeout, blocking));
    case PCM_AM_LF_VCORE0:
        return (__PCM_setCoreVoltageLevelAdvanced(PCM_VCORE0, timeout, blocking)
                && __PCM_setPowerModeAdvanced(PCM_LF_MODE, timeout, blocking));
    case PCM_AM_LF_VCORE1:
        return (__PCM_setCoreVoltageLevelAdvanced(PCM_VCORE1, timeout, blocking)
                && __PCM_setPowerModeAdvanced(PCM_LF_MODE, timeout, blocking));
    case PCM_LPM0_LDO_VCORE0:
        if (!__PCM_setCoreVoltageLevelAdvanced(PCM_VCORE0, timeout, blocking)
                || !__PCM_setPowerModeAdvanced(PCM_LDO_MODE, timeout, blocking))
            break;
        return PCM_gotoLPM0();
    case PCM_LPM0_LDO_VCORE1:
        if (!__PCM_setCoreVoltageLevelAdvanced(PCM_VCORE1, timeout, blocking)
                || !__PCM_setPowerModeAdvanced(PCM_LDO_MODE, timeout, blocking))
            break;
        return PCM_gotoLPM0();
    case PCM_LPM0_DCDC_VCORE0:
        if (!__PCM_setCoreVoltageLevelAdvanced(PCM_VCORE0, timeout, blocking)
                || !__PCM_setPowerModeAdvanced(PCM_DCDC_MODE, timeout,
                        blocking))
            break;
        return PCM_gotoLPM0();
    case PCM_LPM0_DCDC_VCORE1:
        if (!__PCM_setCoreVoltageLevelAdvanced(PCM_VCORE1, timeout, blocking)
                || !__PCM_setPowerModeAdvanced(PCM_DCDC_MODE, timeout,
                        blocking))
            break;
        return PCM_gotoLPM0();
    case PCM_LPM0_LF_VCORE0:
        if (!__PCM_setCoreVoltageLevelAdvanced(PCM_VCORE0, timeout, blocking)
                || !__PCM_setPowerModeAdvanced(PCM_LF_MODE, timeout, blocking))
            break;
        return PCM_gotoLPM0();
    case PCM_LPM0_LF_VCORE1:
        if (!__PCM_setCoreVoltageLevelAdvanced(PCM_VCORE1, timeout, blocking)
                || !__PCM_setPowerModeAdvanced(PCM_LF_MODE, timeout, blocking))
            break;
        return PCM_gotoLPM0();
    case PCM_LPM3:
        return PCM_gotoLPM3();
    case PCM_LPM4:
        return PCM_gotoLPM4();
    case PCM_LPM45:
        return PCM_shutdownDevice(PCM_LPM45);
    case PCM_LPM35_VCORE0:
        return PCM_shutdownDevice(PCM_LPM35_VCORE0);
    default:
        ASSERT(false);
        return false;
    }

    return false;

}


bool PCM_setPowerStateNonBlocking(uint_fast8_t powerState)
{
    return __PCM_setPowerStateAdvanced(powerState, 0, false);
}


bool PCM_setPowerState(uint_fast8_t powerState)
{
    return __PCM_setPowerStateAdvanced(powerState, 0, true);
}


bool PCM_setPowerStateWithTimeout(uint_fast8_t powerState, uint32_t timeout)
{
    return __PCM_setPowerStateAdvanced(powerState, timeout, true);
}


uint8_t PCM_getPowerState(void)
{
    return PCM->rCTL0.b.bCPM;
}


bool PCM_shutdownDevice(uint32_t shutdownMode)
{
    uint32_t shutdownModeBits = (shutdownMode == PCM_LPM45) ? LPMR_12 : LPMR_10;

    ASSERT(
            shutdownMode == PCM_SHUTDOWN_PARTIAL
            || shutdownMode == PCM_SHUTDOWN_COMPLETE);

    /* If a power transition is occuring, return false */
    if (BITBAND_PERI(PCM->rCTL1.r, PMR_BUSY_OFS))
        return false;

    /* Initiating the shutdown */
    HWREG32(SCS_BASE + OFS_SCB_SCR) |= (SCB_SCR_SLEEPDEEP);
    PCM->rCTL0.r = (PCM_KEY | shutdownModeBits
            | (PCM->rCTL0.r & ~(PCMKEY_M | LPMR_M)));

    CPU_wfi();

    return true;
}


bool PCM_gotoLPM0(void)
{
    /* If we are in the middle of a state transition, return false */
    if (BITBAND_PERI(PCM->rCTL1.r, PMR_BUSY_OFS))
        return false;

    HWREG32(SCS_BASE + OFS_SCB_SCR) &= ~(SCB_SCR_SLEEPDEEP);

    CPU_wfi();

    return true;
}


bool PCM_gotoLPM3(void)
{
    uint_fast8_t bCurrentPowerState;
    uint_fast8_t currentPowerMode;

    /* If we are in the middle of a state transition, return false */
    if (BITBAND_PERI(PCM->rCTL1.r, PMR_BUSY_OFS))
        return false;

    /* If we are in the middle of a shutdown, return false */
    if ((PCM->rCTL0.r & LPMR_M) == LPMR_10 || (PCM->rCTL0.r & LPMR_M) == LPMR_12)
        return false;

    currentPowerMode = PCM_getPowerMode();
    bCurrentPowerState = PCM_getPowerState();

    if (currentPowerMode == PCM_DCDC_MODE || currentPowerMode == PCM_LF_MODE)
        PCM_setPowerMode(PCM_LDO_MODE);

    /* Clearing the SDR */
    PCM->rCTL0.r = (PCM->rCTL0.r & ~(PCMKEY_M | LPMR_M)) | PCM_KEY;

    /* Setting the sleep deep bit */
    HWREG32(SCS_BASE + OFS_SCB_SCR) |= (SCB_SCR_SLEEPDEEP);

    CPU_wfi();

    HWREG32(SCS_BASE + OFS_SCB_SCR) &= ~(SCB_SCR_SLEEPDEEP);

    return PCM_setPowerState(bCurrentPowerState);
}


bool PCM_gotoLPM4(void)
{
    /* Disabling RTC_C and WDT_A */
    WDT_A_holdTimer();
    RTC_C_holdClock();

    /* LPM4 is just LPM3 with WDT_A/RTC_C disabled... */
    return PCM_gotoLPM3();
}


void PCM_enableInterrupt(uint32_t flags)
{
    PCM->rIE.r |= flags;
}


void PCM_disableInterrupt(uint32_t flags)
{
    PCM->rIE.r &= ~flags;
}


uint32_t PCM_getInterruptStatus(void)
{
    return PCM->rIFG.r;
}


uint32_t PCM_getEnabledInterruptStatus(void)
{
    return PCM_getInterruptStatus() & PCM->rIE.r;
}


void PCM_enableRudeMode(void)
{

    PCM->rCTL1.r = (PCM->rCTL1.r & ~(PCMKEY_M)) | PCM_KEY | FORCE_LPM_ENTRY;
}


void PCM_disableRudeMode(void)
{
    PCM->rCTL1.r = (PCM->rCTL1.r & ~(PCMKEY_M | FORCE_LPM_ENTRY)) | PCM_KEY;
}


bool PCM_gotoLPM3InterruptSafe(void)
{
    bool lpmHappenedCorrect;

    /* Disabling master interrupts. In Cortex M, if an interrupt is enabled but 
     master interrupts are disabled and a WFI happens the WFI will
     immediately exit. */
    Interrupt_disableMaster();

    lpmHappenedCorrect = PCM_gotoLPM3();

    /* Enabling and Disabling Interrupts very quickly so that the
     processor catches any pending interrupts */
    Interrupt_enableMaster();
    Interrupt_disableMaster();

    return lpmHappenedCorrect;
}


bool PCM_gotoLPM4InterruptSafe(void)
{
    bool slHappenedCorrect;

    /* Disabling master interrupts. In Cortex M, if an interrupt is enabled but 
     master interrupts are disabled and a WFI happens the WFI will
     immediately exit. */
    Interrupt_disableMaster();

    slHappenedCorrect = PCM_gotoLPM4();

    /* Enabling and Disabling Interrupts very quickly so that the
     processor catches any pending interrupts */
    Interrupt_enableMaster();
    Interrupt_disableMaster();

    return slHappenedCorrect;
}


bool PCM_gotoLPM0InterruptSafe(void)
{
    bool slHappenedCorrect;

    /* Disabling master interrupts. In Cortex M, if an interrupt is enabled but 
     master interrupts are disabled and a WFI happens the WFI will
     immediately exit. */
    Interrupt_disableMaster();

    slHappenedCorrect = PCM_gotoLPM0();

    /* Enabling and Disabling Interrupts very quickly so that the
     processor catches any pending interrupts */
    Interrupt_enableMaster();
    Interrupt_disableMaster();

    return slHappenedCorrect;
}


void PCM_clearInterruptFlag(uint32_t flags)
{
    PCM->rCLRIFG.r |= flags;
}


static void __PSSUnlock()
{
    PSS->rKEY.r = PSS_KEY_VALUE;
}


static void __PSSLock()
{
    PSS->rKEY.r = 0;
}


void PSS_enableHighSidePinToggle(bool activeLow)
{
    __PSSUnlock();

    if (activeLow)
        PSS->rCTL0.r |= (SVMHOE | SVMHOUTPOLAL);
    else
    {
        BITBAND_PERI(PSS->rCTL0.r, SVMHOUTPOLAL_OFS) = 0;
        BITBAND_PERI(PSS->rCTL0.r, SVMHOE_OFS) = 1;
    }

    __PSSLock();
}


void PSS_disableHighSidePinToggle(void)
{
    __PSSUnlock();

    BITBAND_PERI(PSS->rCTL0.r, SVMHOE_OFS) = 0;

    __PSSLock();
}


void PSS_enableHighSide(void)
{
    __PSSUnlock();

    BITBAND_PERI(PSS->rCTL0.r, SVSMHOFF_OFS) = 0;

    __PSSLock();
}


void PSS_disableHighSide(void)
{
    __PSSUnlock();

    BITBAND_PERI(PSS->rCTL0.r, SVSMHOFF_OFS) = 1;

    __PSSLock();
}


void PSS_setHighSidePerformanceMode(uint_fast8_t powerMode)
{
    __PSSUnlock();

    if (powerMode == PSS_FULL_PERFORMANCE_MODE)
        BITBAND_PERI(PSS->rCTL0.r, SVSMHLP_OFS) = 0;
    else
        BITBAND_PERI(PSS->rCTL0.r, SVSMHLP_OFS) = 1;

    __PSSLock();
}


uint_fast8_t PSS_getHighSidePerformanceMode(void)
{
    if (BITBAND_PERI(PSS->rCTL0.r, SVSMHLP_OFS))
        return PSS_NORMAL_PERFORMANCE_MODE;
    else
        return PSS_FULL_PERFORMANCE_MODE;
}


void PSS_enableHighSideMonitor(void)
{
    __PSSUnlock();

    BITBAND_PERI(PSS->rCTL0.r, SVSMHS_OFS) = 1;

    __PSSLock();
}


void PSS_disableHighSideMonitor(void)
{
    __PSSUnlock();

    BITBAND_PERI(PSS->rCTL0.r, SVSMHS_OFS) = 0;

    __PSSLock();
}


void PSS_setHighSideVoltageTrigger(uint_fast8_t triggerVoltage)
{
    __PSSUnlock();

    ASSERT(!(triggerVoltage & 0xF8))

    PSS->rCTL0.b.bSVSMHTH = triggerVoltage & 0x07;

    __PSSLock();
}


uint_fast8_t PSS_getHighSideVoltageTrigger(void)
{
    return PSS->rCTL0.b.bSVSMHTH;
}


void PSS_enableInterrupt(void)
{
    __PSSUnlock();
    BITBAND_PERI(PSS->rIE.r,SVSMHIE_OFS) = 1;
    __PSSLock();
}


void PSS_disableInterrupt(void)
{
    __PSSUnlock();
    BITBAND_PERI(PSS->rIE.r,SVSMHIE_OFS) = 0;
    __PSSLock();
}


uint32_t PSS_getInterruptStatus(void)
{
    return PSS->rIFG.r;
}


void PSS_enableForcedDCDCOperation(void)
{
    __PSSUnlock();

    BITBAND_PERI(PSS->rCTL0.r, DCDC_FORCE_OFS) = 1;

    __PSSLock();
}


void PSS_disableForcedDCDCOperation(void)
{
    __PSSUnlock();

    BITBAND_PERI(PSS->rCTL0.r, DCDC_FORCE_OFS) = 0;

    __PSSLock();

}


void PSS_clearInterruptFlag(void)
{
    __PSSUnlock();
    BITBAND_PERI(PSS->rCLRIFG.r,CLRSVSMHIFG_OFS) = 0;
    __PSSLock();
}


void REF_A_setReferenceVoltage(uint_fast8_t referenceVoltageSelect)
{
    ASSERT(referenceVoltageSelect <= REF_A_VREF2_5V);

    REF_A->rCTL0.r = (REF_A->rCTL0.r &  ~REFVSEL_3) | referenceVoltageSelect;
}


void REF_A_disableTempSensor(void)
{
    BITBAND_PERI(REF_A->rCTL0.r,REFTCOFF_OFS) = 1;
}


void REF_A_enableTempSensor(void)
{
    BITBAND_PERI(REF_A->rCTL0.r,REFTCOFF_OFS) = 0;
}


void REF_A_enableReferenceVoltageOutput(void)
{
    BITBAND_PERI(REF_A->rCTL0.r,REFOUT_OFS) = 1;
}


void REF_A_disableReferenceVoltageOutput(void)
{
    BITBAND_PERI(REF_A->rCTL0.r,REFOUT_OFS) = 0;
}


void REF_A_enableReferenceVoltage(void)
{
    BITBAND_PERI(REF_A->rCTL0.r,REFON_OFS) = 1;
}


void REF_A_disableReferenceVoltage(void)
{
    BITBAND_PERI(REF_A->rCTL0.r,REFON_OFS) = 0;
}


uint_fast8_t REF_A_getBandgapMode(void)
{
    return (REF_A->rCTL0.r & BGMODE);
}


bool REF_A_isBandgapActive(void)
{
    return BITBAND_PERI(REF_A->rCTL0.r,REFBGACT_OFS);
}


bool REF_A_isRefGenBusy(void)
{
    return BITBAND_PERI(REF_A->rCTL0.r,REFGENBUSY_OFS);
}


bool REF_A_isRefGenActive(void)
{
    return BITBAND_PERI(REF_A->rCTL0.r,REFGENACT_OFS);
}


bool REF_A_getBufferedBandgapVoltageStatus(void)
{
    return BITBAND_PERI(REF_A->rCTL0.r,REFBGRDY_OFS);
}


bool REF_A_getVariableReferenceVoltageStatus(void)
{
    return BITBAND_PERI(REF_A->rCTL0.r,REFGENRDY_OFS);
}


void REF_A_setReferenceVoltageOneTimeTrigger(void)
{
    BITBAND_PERI(REF_A->rCTL0.r,REFGENOT_OFS) = 1;
}


void REF_A_setBufferedBandgapVoltageOneTimeTrigger(void)
{
    BITBAND_PERI(REF_A->rCTL0.r,REFBGOT_OFS) = 1;
}


void ResetCtl_initiateSoftReset(void)
{
    RSTCTL->rRESET_REQ.r |= (RESET_KEY | RESET_SOFT_RESET);
}


void ResetCtl_initiateSoftResetWithSource(uint32_t source)
{
    RSTCTL->rSOFTRESET_SET.r |= (source);
}


uint32_t ResetCtl_getSoftResetSource(void)
{
    return RSTCTL->rSOFTRESET_STAT.r;
}


void ResetCtl_clearSoftResetSource(uint32_t mask)
{
    RSTCTL->rSOFTRESET_CLR.r |= mask;
}


void ResetCtl_initiateHardReset(void)
{
    RSTCTL->rRESET_REQ.r |= (RESET_KEY | RESET_HARD_RESET);
}


void ResetCtl_initiateHardResetWithSource(uint32_t source)
{
    RSTCTL->rHARDRESET_SET.r |= (source);
}


uint32_t ResetCtl_getHardResetSource(void)
{
    return RSTCTL->rHARDRESET_STAT.r;
}


void ResetCtl_clearHardResetSource(uint32_t mask)
{
    RSTCTL->rHARDRESET_CLR.r |= mask;
}


uint32_t ResetCtl_getPSSSource(void)
{
    return RSTCTL->rPSSRESET_STAT.r;
}


void ResetCtl_clearPSSFlags(void)
{
    RSTCTL->rPSSRESET_CLR.r |= RSTCTL_PSSRESET_CLR_CLR;
}


uint32_t ResetCtl_getPCMSource(void)
{
    return RSTCTL->rPCMRESET_STAT.r;
}


void ResetCtl_clearPCMFlags(void)
{
    RSTCTL->rPCMRESET_CLR.r |= RSTCTL_PCMRESET_CLR_CLR;
}


void RTC_C_startClock(void)
{
    RTC_C->rCTL0.b.bKEY = RTCKEY_H;
    BITBAND_PERI(RTC_C->rCTL13.r, RTCHOLD_OFS) = 0;
    BITBAND_PERI(RTC_C->rCTL0.r, RTCKEY_OFS) = 0;
}


void RTC_C_holdClock(void)
{
    RTC_C->rCTL0.b.bKEY = RTCKEY_H;
    BITBAND_PERI(RTC_C->rCTL13.r, RTCHOLD_OFS) = 1;
    BITBAND_PERI(RTC_C->rCTL0.r, RTCKEY_OFS) = 0;
}


void RTC_C_setCalibrationFrequency(uint_fast16_t frequencySelect)
{
    RTC_C->rCTL0.b.bKEY = RTCKEY_H;
    RTC_C->rCTL13.r = (RTC_C->rCTL13.r & ~(RTCCALF_3)) | frequencySelect;
    BITBAND_PERI(RTC_C->rCTL0.r, RTCKEY_OFS) = 0;
}


void RTC_C_setCalibrationData(uint_fast8_t offsetDirection,
        uint_fast8_t offsetValue)
{
    RTC_C->rCTL0.b.bKEY = RTCKEY_H;
    RTC_C->rOCAL.r = offsetValue + offsetDirection;
    BITBAND_PERI(RTC_C->rCTL0.r, RTCKEY_OFS) = 0;
}


bool RTC_C_setTemperatureCompensation(uint_fast16_t offsetDirection,
        uint_fast8_t offsetValue)
{
    while (!BITBAND_PERI(RTC_C->rTCMP.r, RTCTCRDY_OFS))
        ;

    RTC_C->rTCMP.r = offsetValue + offsetDirection;

    if (BITBAND_PERI(RTC_C->rTCMP.r, RTCTCOK_OFS))
        return true;
    else
        return false;
}


void RTC_C_initCalendar(const RTC_C_Calendar *calendarTime,
        uint_fast16_t formatSelect)
{
    RTC_C->rCTL0.b.bKEY = RTCKEY_H;

    BITBAND_PERI(RTC_C->rCTL13.r, RTCHOLD_OFS) = 1;

    if (formatSelect)
        BITBAND_PERI(RTC_C->rCTL13.r, RTCBCD_OFS) = 1;
    else
        BITBAND_PERI(RTC_C->rCTL13.r, RTCBCD_OFS) = 0;

    RTC_C->rTIM0.b.bSEC = calendarTime->seconds;
    RTC_C->rTIM0.b.bMIN = calendarTime->minutes;
    RTC_C->rTIM1.b.bHOUR = calendarTime->hours;
    RTC_C->rTIM1.b.bDOW = calendarTime->dayOfWeek;
    RTC_C->rDATE.b.bDAY = calendarTime->dayOfmonth;
    RTC_C->rDATE.b.bMON = calendarTime->month;
    RTC_C->rYEAR.r = calendarTime->year;

    BITBAND_PERI(RTC_C->rCTL0.r, RTCKEY_OFS) = 0;
}


RTC_C_Calendar RTC_C_getCalendarTime(void)
{
    RTC_C_Calendar tempCal;

    while (!(BITBAND_PERI(RTC_C->rCTL13.r, RTCRDY_OFS)))
        ;

    tempCal.seconds = RTC_C->rTIM0.b.bSEC;
    tempCal.minutes = RTC_C->rTIM0.b.bMIN;
    tempCal.hours = RTC_C->rTIM1.b.bHOUR;
    tempCal.dayOfWeek = RTC_C->rTIM1.b.bDOW;
    tempCal.dayOfmonth = RTC_C->rDATE.b.bDAY;
    tempCal.month = RTC_C->rDATE.b.bMON;
    tempCal.year = RTC_C->rYEAR.r;

    return (tempCal);
}


void RTC_C_setCalendarAlarm(uint_fast8_t minutesAlarm, uint_fast8_t hoursAlarm,
        uint_fast8_t dayOfWeekAlarm, uint_fast8_t dayOfmonthAlarm)
{
    //Each of these is XORed with 0x80 to turn on if an integer is passed,
    //or turn OFF if RTC_ALARM_OFF (0x80) is passed.
    HWREG8(RTC_C_BASE + OFS_RTCAMINHR) = (minutesAlarm ^ 0x80);
    HWREG8(RTC_C_BASE + OFS_RTCAMINHR + 1) = (hoursAlarm ^ 0x80);
    HWREG8(RTC_C_BASE + OFS_RTCADOWDAY) = (dayOfWeekAlarm ^ 0x80);
    HWREG8(RTC_C_BASE + OFS_RTCADOWDAY + 1) = (dayOfmonthAlarm ^ 0x80);
}


void RTC_C_setCalendarEvent(uint_fast16_t eventSelect)
{
    RTC_C->rCTL0.b.bKEY = RTCKEY_H;
    RTC_C->rCTL13.r = (RTC_C->rCTL13.r & ~(RTCTEV_3)) | eventSelect;
    BITBAND_PERI(RTC_C->rCTL0.r, RTCKEY_OFS) = 0;
}


void RTC_C_definePrescaleEvent(uint_fast8_t prescaleSelect,
        uint_fast8_t prescaleEventDivider)
{
    HWREG8(RTC_C_BASE + OFS_RTCPS0CTL + prescaleSelect) &= ~(RT0IP_7);
    HWREG8(RTC_C_BASE + OFS_RTCPS0CTL + prescaleSelect) |=
            prescaleEventDivider;
}


uint_fast8_t RTC_C_getPrescaleValue(uint_fast8_t prescaleSelect)
{
    if (RTC_C_PRESCALE_0 == prescaleSelect)
    {
        return (RTC_C->rPS.b.bRT0PS);
    } else if (RTC_C_PRESCALE_1 == prescaleSelect)
    {
        return (RTC_C->rPS.b.bRT1PS);
    } else
    {
        return (0);
    }
}


void RTC_C_setPrescaleValue(uint_fast8_t prescaleSelect,
        uint_fast8_t prescaleCounterValue)
{
    RTC_C->rCTL0.b.bKEY = RTCKEY_H;

    if (RTC_C_PRESCALE_0 == prescaleSelect)
    {
        RTC_C->rPS.b.bRT0PS = prescaleCounterValue;
    } else if (RTC_C_PRESCALE_1 == prescaleSelect)
    {
        RTC_C->rPS.b.bRT1PS = prescaleCounterValue;
    }

    BITBAND_PERI(RTC_C->rCTL0.r, RTCKEY_OFS) = 0;
}


uint16_t RTC_C_convertBCDToBinary(uint16_t valueToConvert)
{
    RTC_C->rBCD2BIN = valueToConvert;
    return (RTC_C->rBCD2BIN);
}


uint16_t RTC_C_convertBinaryToBCD(uint16_t valueToConvert)
{
    RTC_C->rBIN2BCD = valueToConvert;
    return (RTC_C->rBIN2BCD);
}


void RTC_C_enableInterrupt(uint8_t interruptMask)
{
    if (interruptMask & (RTCOFIE + RTCTEVIE + RTCAIE + RTCRDYIE))
    {
        RTC_C->rCTL0.r = RTCKEY | (interruptMask
                & (RTCOFIE + RTCTEVIE + RTCAIE + RTCRDYIE));
        BITBAND_PERI(RTC_C->rCTL0.r, RTCKEY_OFS) = 0;
    }

    if (interruptMask & RTC_C_PRESCALE_TIMER0_INTERRUPT)
    {
        BITBAND_PERI(RTC_C->rPS0CTL.r,RT0PSIE_OFS) = 1;
    }

    if (interruptMask & RTC_C_PRESCALE_TIMER1_INTERRUPT)
    {
        BITBAND_PERI(RTC_C->rPS1CTL.r,RT1PSIE_OFS) = 1;
    }
}


void RTC_C_disableInterrupt(uint8_t interruptMask)
{
    if (interruptMask & (RTCOFIE + RTCTEVIE + RTCAIE + RTCRDYIE))
    {
        RTC_C->rCTL0.r = RTCKEY
                | (RTC_C->rCTL0.r
                        & ~((interruptMask | RTCKEY_M)
                                & (RTCOFIE + RTCTEVIE + RTCAIE + RTCRDYIE)));
        BITBAND_PERI(RTC_C->rCTL0.r, RTCKEY_OFS) = 0;
    }

    if (interruptMask & RTC_C_PRESCALE_TIMER0_INTERRUPT)
    {
        BITBAND_PERI(RTC_C->rPS0CTL.r,RT0PSIE_OFS) = 0;
    }

    if (interruptMask & RTC_C_PRESCALE_TIMER1_INTERRUPT)
    {
        BITBAND_PERI(RTC_C->rPS1CTL.r,RT1PSIE_OFS) = 0;
    }
}


uint_fast8_t RTC_C_getInterruptStatus(void)
{
    uint_fast8_t tempInterruptFlagMask = 0x00;
    uint_fast8_t interruptFlagMask = RTC_C_TIME_EVENT_INTERRUPT
            | RTC_C_CLOCK_ALARM_INTERRUPT | RTC_C_CLOCK_READ_READY_INTERRUPT
            | RTC_C_PRESCALE_TIMER0_INTERRUPT | RTC_C_PRESCALE_TIMER1_INTERRUPT
            | RTC_C_OSCILLATOR_FAULT_INTERRUPT;

    tempInterruptFlagMask |= (RTC_C->rCTL0.r & (interruptFlagMask >> 4));

    tempInterruptFlagMask = tempInterruptFlagMask << 4;

    if (interruptFlagMask & RTC_C_PRESCALE_TIMER0_INTERRUPT)
    {
        if (BITBAND_PERI(RTC_C->rPS0CTL.r, RT0PSIFG_OFS))
        {
            tempInterruptFlagMask |= RTC_C_PRESCALE_TIMER0_INTERRUPT;
        }
    }

    if (interruptFlagMask & RTC_C_PRESCALE_TIMER1_INTERRUPT)
    {
        if (BITBAND_PERI(RTC_C->rPS1CTL.r, RT1PSIFG_OFS))
        {
            tempInterruptFlagMask |= RTC_C_PRESCALE_TIMER1_INTERRUPT;
        }
    }

    return (tempInterruptFlagMask);
}


void RTC_C_clearInterruptFlag(uint_fast8_t interruptFlagMask)
{
    if (interruptFlagMask
            & (RTC_C_TIME_EVENT_INTERRUPT + RTC_C_CLOCK_ALARM_INTERRUPT
                    + RTC_C_CLOCK_READ_READY_INTERRUPT
                    + RTC_C_OSCILLATOR_FAULT_INTERRUPT))
    {
        RTC_C->rCTL0.r = RTCKEY
                | (RTC_C->rCTL0.r & ~((interruptFlagMask >> 4) | RTCKEY_M));
        BITBAND_PERI(RTC_C->rCTL0.r, RTCKEY_OFS) = 0;
    }

    if (interruptFlagMask & RTC_C_PRESCALE_TIMER0_INTERRUPT)
    {
        BITBAND_PERI(RTC_C->rPS0CTL.r,RT0PSIFG_OFS) = 0;
    }

    if (interruptFlagMask & RTC_C_PRESCALE_TIMER1_INTERRUPT)
    {
        BITBAND_PERI(RTC_C->rPS1CTL.r, RT1PSIFG_OFS) = 0;
    }
}


uint_fast8_t RTC_C_getEnabledInterruptStatus(void)
{

    uint32_t intStatus = RTC_C_getInterruptStatus();

    if (!BITBAND_PERI(RTC_C->rCTL0.r, RTCOFIE_OFS))
    {
        intStatus &= ~RTC_C_OSCILLATOR_FAULT_INTERRUPT;
    }

    if (!BITBAND_PERI(RTC_C->rCTL0.r, RTCTEVIE_OFS))
    {
        intStatus &= ~RTC_C_TIME_EVENT_INTERRUPT;
    }

    if (!BITBAND_PERI(RTC_C->rCTL0.r, RTCAIE_OFS))
    {
        intStatus &= ~RTC_C_CLOCK_ALARM_INTERRUPT;
    }

    if (!BITBAND_PERI(RTC_C->rCTL0.r, RTCRDYIE_OFS))
    {
        intStatus &= ~RTC_C_CLOCK_READ_READY_INTERRUPT;
    }

    if (!BITBAND_PERI(RTC_C->rPS0CTL, RT0PSIE_OFS))
    {
        intStatus &= ~RTC_C_PRESCALE_TIMER0_INTERRUPT;
    }

    if (!BITBAND_PERI(RTC_C->rPS1CTL.r, RT1PSIE_OFS))
    {
        intStatus &= ~RTC_C_PRESCALE_TIMER1_INTERRUPT;
    }

    return intStatus;
}


static bool is_A_Module(uint32_t module)
{
    if (module == EUSCI_A0_MODULE || module == EUSCI_A1_MODULE
#ifdef EUSCI_A2_MODULE
            || module == EUSCI_A2_MODULE
#endif
#ifdef EUSCI_A3_MODULE
            || module == EUSCI_A3_MODULE
#endif
    )
        return true;
    else
        return false;
}


bool SPI_initMaster(uint32_t moduleInstance, const eUSCI_SPI_MasterConfig *config)
{
    /* Returning false if we are not divisible */
    if((config->clockSourceFrequency
                % config->desiredSpiClock) != 0)
    {
        return false;
    }
    
    if (is_A_Module(moduleInstance))
    {
        ASSERT(
                (EUSCI_A_SPI_CLOCKSOURCE_ACLK == config->selectClockSource)
                || (EUSCI_A_SPI_CLOCKSOURCE_SMCLK
                        == config->selectClockSource));

        ASSERT(
                (EUSCI_A_SPI_MSB_FIRST == config->msbFirst)
                || (EUSCI_A_SPI_LSB_FIRST == config->msbFirst));

        ASSERT(
                (EUSCI_A_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT
                        == config->clockPhase)
                || (EUSCI_A_SPI_PHASE_DATA_CAPTURED_ONFIRST_CHANGED_ON_NEXT
                        == config->clockPhase));

        ASSERT(
                (EUSCI_A_SPI_CLOCKPOLARITY_INACTIVITY_HIGH
                        == config->clockPolarity)
                || (EUSCI_A_SPI_CLOCKPOLARITY_INACTIVITY_LOW
                        == config->clockPolarity));

        ASSERT(
                (EUSCI_A_SPI_3PIN == config->spiMode)
                || (EUSCI_A_SPI_4PIN_UCxSTE_ACTIVE_HIGH
                        == config->spiMode)
                || (EUSCI_A_SPI_4PIN_UCxSTE_ACTIVE_LOW
                        == config->spiMode));
                        
        //Disable the USCI Module
        BITBAND_PERI(EUSCI_B_CMSIS(moduleInstance)->rCTLW0.r, UCSWRST_OFS) = 1;

        /*
         * Configure as SPI master mode.
         * Clock phase select, polarity, msb
         * UCMST = Master mode
         * UCSYNC = Synchronous mode
         * UCMODE_0 = 3-pin SPI
         */
        EUSCI_A_CMSIS(moduleInstance)->rCTLW0.r =
                (EUSCI_A_CMSIS(moduleInstance)->rCTLW0.r
                        & ~(UCSSEL_3 + UCCKPH + UCCKPL + UC7BIT + UCMSB + UCMST
                                + UCMODE_3 + UCSYNC))
                        | (config->selectClockSource + config->msbFirst
                                + config->clockPhase + config->clockPolarity
                                + UCMST + UCSYNC + config->spiMode);
        
        EUSCI_A_CMSIS(moduleInstance)->rBRW =
                (uint16_t) (config->clockSourceFrequency
                        / config->desiredSpiClock);

        //No modulation
        EUSCI_A_CMSIS(moduleInstance)->rMCTLW.r = 0;

        return true;
    } else
    {
        ASSERT(
                (EUSCI_B_SPI_CLOCKSOURCE_ACLK == config->selectClockSource)
                || (EUSCI_B_SPI_CLOCKSOURCE_SMCLK
                        == config->selectClockSource));

        ASSERT(
                (EUSCI_B_SPI_MSB_FIRST == config->msbFirst)
                || (EUSCI_B_SPI_LSB_FIRST == config->msbFirst));

        ASSERT(
                (EUSCI_B_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT
                        == config->clockPhase)
                || (EUSCI_B_SPI_PHASE_DATA_CAPTURED_ONFIRST_CHANGED_ON_NEXT
                        == config->clockPhase));

        ASSERT(
                (EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_HIGH
                        == config->clockPolarity)
                || (EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_LOW
                        == config->clockPolarity));

        ASSERT(
                (EUSCI_B_SPI_3PIN == config->spiMode)
                || (EUSCI_B_SPI_4PIN_UCxSTE_ACTIVE_HIGH
                        == config->spiMode)
                || (EUSCI_B_SPI_4PIN_UCxSTE_ACTIVE_LOW
                        == config->spiMode));

        //Disable the USCI Module
        BITBAND_PERI(EUSCI_B_CMSIS(moduleInstance)->rCTLW0.r, UCSWRST_OFS) = 1;

        /*
         * Configure as SPI master mode.
         * Clock phase select, polarity, msb
         * UCMST = Master mode
         * UCSYNC = Synchronous mode
         * UCMODE_0 = 3-pin SPI
         */
        EUSCI_B_CMSIS(moduleInstance)->rCTLW0.r =
                (EUSCI_B_CMSIS(moduleInstance)->rCTLW0.r
                        & ~(UCSSEL_3 + UCCKPH + UCCKPL + UC7BIT + UCMSB + UCMST
                                + UCMODE_3 + UCSYNC))
                        | (config->selectClockSource + config->msbFirst
                                + config->clockPhase + config->clockPolarity
                                + UCMST + UCSYNC + config->spiMode);

        EUSCI_B_CMSIS(moduleInstance)->rBRW =
                (uint16_t) (config->clockSourceFrequency
                        / config->desiredSpiClock);

        return true;
    }

}


void SPI_selectFourPinFunctionality(uint32_t moduleInstance,
        uint_fast8_t select4PinFunctionality)
{
    if (is_A_Module(moduleInstance))
    {
        EUSCI_A_SPI_select4PinFunctionality(moduleInstance,
                select4PinFunctionality);
    } else
    {
        EUSCI_B_SPI_select4PinFunctionality(moduleInstance,
                select4PinFunctionality);
    }

}


void SPI_changeMasterClock(uint32_t moduleInstance,
        uint32_t clockSourceFrequency, uint32_t desiredSpiClock)
{
    if (is_A_Module(moduleInstance))
    {
        EUSCI_A_SPI_masterChangeClock(moduleInstance, clockSourceFrequency,
                desiredSpiClock);
    } else
    {
        EUSCI_B_SPI_masterChangeClock(moduleInstance, clockSourceFrequency,
                desiredSpiClock);
    }

}


bool SPI_initSlave(uint32_t moduleInstance, const eUSCI_SPI_SlaveConfig *config)
{
    if (is_A_Module(moduleInstance))
    {
        ASSERT(
                (EUSCI_A_SPI_MSB_FIRST == config->msbFirst)
                || (EUSCI_A_SPI_LSB_FIRST == config->msbFirst));

        ASSERT(
                (EUSCI_A_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT
                        == config->clockPhase)
                || (EUSCI_A_SPI_PHASE_DATA_CAPTURED_ONFIRST_CHANGED_ON_NEXT
                        == config->clockPhase));

        ASSERT(
                (EUSCI_A_SPI_CLOCKPOLARITY_INACTIVITY_HIGH
                        == config->clockPolarity)
                || (EUSCI_A_SPI_CLOCKPOLARITY_INACTIVITY_LOW
                        == config->clockPolarity));

        ASSERT(
                (EUSCI_A_SPI_3PIN == config->spiMode)
                || (EUSCI_A_SPI_4PIN_UCxSTE_ACTIVE_HIGH
                        == config->spiMode)
                || (EUSCI_A_SPI_4PIN_UCxSTE_ACTIVE_LOW
                        == config->spiMode));

        //Disable USCI Module
        BITBAND_PERI(EUSCI_A_CMSIS(moduleInstance)->rCTLW0.r, UCSWRST_OFS) = 1;

        //Reset OFS_UCAxCTLW0 register
        EUSCI_A_CMSIS(moduleInstance)->rCTLW0.r =
                (EUSCI_A_CMSIS(moduleInstance)->rCTLW0.r
                        & ~(UCMSB + UC7BIT + UCMST + UCCKPL + UCCKPH + UCMODE_3))
                        | (config->clockPhase + config->clockPolarity
                                + config->msbFirst + UCSYNC + config->spiMode);

        return true;
    } else
    {
        ASSERT(
                (EUSCI_B_SPI_MSB_FIRST == config->msbFirst)
                || (EUSCI_B_SPI_LSB_FIRST == config->msbFirst));

        ASSERT(
                (EUSCI_B_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT
                        == config->clockPhase)
                || (EUSCI_B_SPI_PHASE_DATA_CAPTURED_ONFIRST_CHANGED_ON_NEXT
                        == config->clockPhase));

        ASSERT(
                (EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_HIGH
                        == config->clockPolarity)
                || (EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_LOW
                        == config->clockPolarity));

        ASSERT(
                (EUSCI_B_SPI_3PIN == config->spiMode)
                || (EUSCI_B_SPI_4PIN_UCxSTE_ACTIVE_HIGH
                        == config->spiMode)
                || (EUSCI_B_SPI_4PIN_UCxSTE_ACTIVE_LOW
                        == config->spiMode));

        //Disable USCI Module
        BITBAND_PERI(EUSCI_B_CMSIS(moduleInstance)->rCTLW0.r, UCSWRST_OFS) = 1;

        //Reset OFS_UCBxCTLW0 register
        EUSCI_B_CMSIS(moduleInstance)->rCTLW0.r =
                (EUSCI_B_CMSIS(moduleInstance)->rCTLW0.r
                        & ~(UCMSB + UC7BIT + UCMST + UCCKPL + UCCKPH + UCMODE_3))
                        | (config->clockPhase + config->clockPolarity
                                + config->msbFirst + UCSYNC + config->spiMode);

        return true;
    }

}


void SPI_changeClockPhasePolarity(uint32_t moduleInstance,
        uint_fast16_t clockPhase, uint_fast16_t clockPolarity)
{
    if (is_A_Module(moduleInstance))
    {
        EUSCI_A_SPI_changeClockPhasePolarity(moduleInstance, clockPhase,
                clockPolarity);
    } else
    {
        EUSCI_B_SPI_changeClockPhasePolarity(moduleInstance, clockPhase,
                clockPolarity);
    }

}


void SPI_transmitData(uint32_t moduleInstance, uint_fast8_t transmitData)
{
    if (is_A_Module(moduleInstance))
    {
        EUSCI_A_SPI_transmitData(moduleInstance, transmitData);
    } else
    {
        EUSCI_B_SPI_transmitData(moduleInstance, transmitData);
    }

}


uint8_t SPI_receiveData(uint32_t moduleInstance)
{
    if (is_A_Module(moduleInstance))
    {
        return EUSCI_A_SPI_receiveData(moduleInstance);
    } else
    {
        return EUSCI_B_SPI_receiveData(moduleInstance);
    }

}


void SPI_enableModule(uint32_t moduleInstance)
{
    if (is_A_Module(moduleInstance))
    {
        EUSCI_A_SPI_enable(moduleInstance);
    } else
    {
        EUSCI_B_SPI_enable(moduleInstance);
    }

}


void SPI_disableModule(uint32_t moduleInstance)
{
    if (is_A_Module(moduleInstance))
    {
        EUSCI_A_SPI_disable(moduleInstance);
    } else
    {
        EUSCI_B_SPI_disable(moduleInstance);
    }

}


uint32_t SPI_getReceiveBufferAddressForDMA(uint32_t moduleInstance)
{
    if (is_A_Module(moduleInstance))
    {
        return EUSCI_A_SPI_getReceiveBufferAddressForDMA(moduleInstance);
    } else
    {
        return EUSCI_B_SPI_getReceiveBufferAddressForDMA(moduleInstance);
    }

}


uint32_t SPI_getTransmitBufferAddressForDMA(uint32_t moduleInstance)
{
    if (is_A_Module(moduleInstance))
    {
        return EUSCI_A_SPI_getTransmitBufferAddressForDMA(moduleInstance);
    } else
    {
        return EUSCI_B_SPI_getTransmitBufferAddressForDMA(moduleInstance);
    }

}


uint_fast8_t SPI_isBusy(uint32_t moduleInstance)
{
    if (is_A_Module(moduleInstance))
    {
        return EUSCI_A_SPI_isBusy(moduleInstance);
    } else
    {
        return EUSCI_B_SPI_isBusy(moduleInstance);
    }

}


void SPI_enableInterrupt(uint32_t moduleInstance, uint_fast8_t mask)
{
    if (is_A_Module(moduleInstance))
    {
        EUSCI_A_SPI_enableInterrupt(moduleInstance, mask);
    } else
    {
        EUSCI_B_SPI_enableInterrupt(moduleInstance, mask);
    }

}


void SPI_disableInterrupt(uint32_t moduleInstance, uint_fast8_t mask)
{
    if (is_A_Module(moduleInstance))
    {
        EUSCI_A_SPI_disableInterrupt(moduleInstance, mask);
    } else
    {
        EUSCI_B_SPI_disableInterrupt(moduleInstance, mask);
    }

}


uint_fast8_t SPI_getInterruptStatus(uint32_t moduleInstance, uint16_t mask)
{
    if (is_A_Module(moduleInstance))
    {
        return EUSCI_A_SPI_getInterruptStatus(moduleInstance, mask);
    } else
    {
        return EUSCI_B_SPI_getInterruptStatus(moduleInstance, mask);
    }

}


uint_fast8_t SPI_getEnabledInterruptStatus(uint32_t moduleInstance)
{
    if (is_A_Module(moduleInstance))
    {
        return SPI_getInterruptStatus(moduleInstance,
                EUSCI_SPI_TRANSMIT_INTERRUPT | EUSCI_SPI_RECEIVE_INTERRUPT)
                & HWREG16(moduleInstance + OFS_UCA0IE);
    } else
    {
        return SPI_getInterruptStatus(moduleInstance,
                EUSCI_SPI_TRANSMIT_INTERRUPT | EUSCI_SPI_RECEIVE_INTERRUPT)
                & HWREG16(moduleInstance + OFS_UCB0IE);

    }
}


void SPI_clearInterruptFlag(uint32_t moduleInstance, uint_fast8_t mask)
{
    if (is_A_Module(moduleInstance))
    {
        EUSCI_A_SPI_clearInterruptFlag(moduleInstance, mask);
    } else
    {
        EUSCI_B_SPI_clearInterruptFlag(moduleInstance, mask);
    }

}


//*****************************************************************************
//
//! \brief Selects 4Pin Functionality
//!
//! This function should be invoked only in 4-wire mode. Invoking this function
//! has no effect in 3-wire mode.
//!
//! \param baseAddress is the base address of the EUSCI_A_SPI module.
//! \param select4PinFunctionality selects 4 pin functionality
//!        Valid values are:
//!        - \b EUSCI_A_SPI_PREVENT_CONFLICTS_WITH_OTHER_MASTERS
//!        - \b EUSCI_A_SPI_ENABLE_SIGNAL_FOR_4WIRE_SLAVE
//!
//! Modified bits are \b UCSTEM of \b UCAxCTLW0 register.
//!
//! \return None
//
//*****************************************************************************
void EUSCI_A_SPI_select4PinFunctionality(uint32_t baseAddress,
        uint8_t select4PinFunctionality)
{
    ASSERT(
            (EUSCI_A_SPI_PREVENT_CONFLICTS_WITH_OTHER_MASTERS
                    == select4PinFunctionality)
            || (EUSCI_A_SPI_ENABLE_SIGNAL_FOR_4WIRE_SLAVE
                    == select4PinFunctionality));

    EUSCI_A_CMSIS(baseAddress)->rCTLW0.r = (EUSCI_A_CMSIS(baseAddress)->rCTLW0.r
            & ~UCSTEM) | select4PinFunctionality;
}


//*****************************************************************************
//
//! \brief Initializes the SPI Master clock. At the end of this function call,
//! SPI module is left enabled.
//!
//! \param baseAddress is the base address of the EUSCI_A_SPI module.
//! \param clockSourceFrequency is the frequency of the slected clock source
//! \param desiredSpiClock is the desired clock rate for SPI communication
//!
//! Modified bits are \b UCSWRST of \b UCAxCTLW0 register.
//!
//! \return None
//
//*****************************************************************************
void EUSCI_A_SPI_masterChangeClock(uint32_t baseAddress,
        uint32_t clockSourceFrequency, uint32_t desiredSpiClock)
{
    //Disable the USCI Module
    BITBAND_PERI(EUSCI_A_CMSIS(baseAddress)->rCTLW0.r, UCSWRST_OFS) = 1;

    EUSCI_A_CMSIS(baseAddress)->rBRW = (uint16_t) (clockSourceFrequency
            / desiredSpiClock);

    //Reset the UCSWRST bit to enable the USCI Module
    BITBAND_PERI(EUSCI_A_CMSIS(baseAddress)->rCTLW0.r, UCSWRST_OFS) = 0;
}


//*****************************************************************************
//
//! \brief Initializes the SPI Slave block.
//!
//! Upon successful initialization of the SPI slave block, this function will
//! have initailized the slave block, but the SPI Slave block still remains
//! disabled and must be enabled with EUSCI_A_SPI_enable()
//!
//! \param baseAddress is the base address of the EUSCI_A_SPI Slave module.
//! \param msbFirst controls the direction of the receive and transmit shift
//!        register.
//!        Valid values are:
//!        - \b EUSCI_A_SPI_MSB_FIRST
//!        - \b EUSCI_A_SPI_LSB_FIRST [Default]
//! \param clockPhase is clock phase select.
//!        Valid values are:
//!        - \b EUSCI_A_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT
//!           [Default]
//!        - \b EUSCI_A_SPI_PHASE_DATA_CAPTURED_ONFIRST_CHANGED_ON_NEXT
//! \param clockPolarity is clock polarity select
//!        Valid values are:
//!        - \b EUSCI_A_SPI_CLOCKPOLARITY_INACTIVITY_HIGH
//!        - \b EUSCI_A_SPI_CLOCKPOLARITY_INACTIVITY_LOW [Default]
//! \param spiMode is SPI mode select
//!        Valid values are:
//!        - \b EUSCI_A_SPI_3PIN
//!        - \b EUSCI_A_SPI_4PIN_UCxSTE_ACTIVE_HIGH
//!        - \b EUSCI_A_SPI_4PIN_UCxSTE_ACTIVE_LOW
//!
//! Modified bits are \b UCMSB, \b UCMST, \b UC7BIT, \b UCCKPL, \b UCCKPH, \b
//! UCMODE and \b UCSWRST of \b UCAxCTLW0 register.
//!
//! \return STATUS_SUCCESS
//
//*****************************************************************************
bool EUSCI_A_SPI_slaveInit(uint32_t baseAddress, uint16_t msbFirst,
        uint16_t clockPhase, uint16_t clockPolarity, uint16_t spiMode)
{
    ASSERT(
            (EUSCI_A_SPI_MSB_FIRST == msbFirst)
            || (EUSCI_A_SPI_LSB_FIRST == msbFirst));

    ASSERT(
            (EUSCI_A_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT
                    == clockPhase)
            || (EUSCI_A_SPI_PHASE_DATA_CAPTURED_ONFIRST_CHANGED_ON_NEXT
                    == clockPhase));

    ASSERT(
            (EUSCI_A_SPI_CLOCKPOLARITY_INACTIVITY_HIGH == clockPolarity)
            || (EUSCI_A_SPI_CLOCKPOLARITY_INACTIVITY_LOW
                    == clockPolarity));

    ASSERT(
            (EUSCI_A_SPI_3PIN == spiMode)
            || (EUSCI_A_SPI_4PIN_UCxSTE_ACTIVE_HIGH == spiMode)
            || (EUSCI_A_SPI_4PIN_UCxSTE_ACTIVE_LOW == spiMode));

    //Disable USCI Module
    BITBAND_PERI(EUSCI_A_CMSIS(baseAddress)->rCTLW0.r, UCSWRST_OFS) = 1;

    //Reset OFS_UCAxCTLW0 register
    EUSCI_A_CMSIS(baseAddress)->rCTLW0.r = (EUSCI_A_CMSIS(baseAddress)->rCTLW0.r
            & ~(UCMSB + UC7BIT + UCMST + UCCKPL + UCCKPH + UCMODE_3))
            | (clockPhase + clockPolarity + msbFirst + UCSYNC + spiMode);

    return true;
}


//*****************************************************************************
//
//! \brief Changes the SPI colock phase and polarity. At the end of this
//! function call, SPI module is left enabled.
//!
//! \param baseAddress is the base address of the EUSCI_A_SPI module.
//! \param clockPhase is clock phase select.
//!        Valid values are:
//!        - \b EUSCI_A_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT
//!           [Default]
//!        - \b EUSCI_A_SPI_PHASE_DATA_CAPTURED_ONFIRST_CHANGED_ON_NEXT
//! \param clockPolarity is clock polarity select
//!        Valid values are:
//!        - \b EUSCI_A_SPI_CLOCKPOLARITY_INACTIVITY_HIGH
//!        - \b EUSCI_A_SPI_CLOCKPOLARITY_INACTIVITY_LOW [Default]
//!
//! Modified bits are \b UCCKPL, \b UCCKPH and \b UCSWRST of \b UCAxCTLW0
//! register.
//!
//! \return None
//
//*****************************************************************************
void EUSCI_A_SPI_changeClockPhasePolarity(uint32_t baseAddress,
        uint16_t clockPhase, uint16_t clockPolarity)
{

    ASSERT(
            (EUSCI_A_SPI_CLOCKPOLARITY_INACTIVITY_HIGH == clockPolarity)
            || (EUSCI_A_SPI_CLOCKPOLARITY_INACTIVITY_LOW
                    == clockPolarity));

    ASSERT(
            (EUSCI_A_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT
                    == clockPhase)
            || (EUSCI_A_SPI_PHASE_DATA_CAPTURED_ONFIRST_CHANGED_ON_NEXT
                    == clockPhase));

    //Disable the USCI Module
    BITBAND_PERI(EUSCI_A_CMSIS(baseAddress)->rCTLW0.r, UCSWRST_OFS) = 1;

    EUSCI_A_CMSIS(baseAddress)->rCTLW0.r = (EUSCI_A_CMSIS(baseAddress)->rCTLW0.r
            & ~(UCCKPH + UCCKPL)) | (clockPhase + clockPolarity);

    //Reset the UCSWRST bit to enable the USCI Module
    BITBAND_PERI(EUSCI_A_CMSIS(baseAddress)->rCTLW0.r, UCSWRST_OFS) = 0;
}


//*****************************************************************************
//
//! \brief Transmits a byte from the SPI Module.
//!
//! This function will place the supplied data into SPI trasmit data register
//! to start transmission.
//!
//! \param baseAddress is the base address of the EUSCI_A_SPI module.
//! \param transmitData data to be transmitted from the SPI module
//!
//! \return None
//
//*****************************************************************************
void EUSCI_A_SPI_transmitData(uint32_t baseAddress, uint8_t transmitData)
{
    EUSCI_A_CMSIS(baseAddress)->rTXBUF.r = transmitData;
}


//*****************************************************************************
//
//! \brief Receives a byte that has been sent to the SPI Module.
//!
//! This function reads a byte of data from the SPI receive data Register.
//!
//! \param baseAddress is the base address of the EUSCI_A_SPI module.
//!
//! \return Returns the byte received from by the SPI module, cast as an
//!         uint8_t.
//
//*****************************************************************************
uint8_t EUSCI_A_SPI_receiveData(uint32_t baseAddress)
{
    return EUSCI_A_CMSIS(baseAddress)->rRXBUF.r;
}


//*****************************************************************************
//
//! \brief Enables individual SPI interrupt sources.
//!
//! Enables the indicated SPI interrupt sources.  Only the sources that are
//! enabled can be reflected to the processor interrupt; disabled sources have
//! no effect on the processor. Does not clear interrupt flags.
//!
//! \param baseAddress is the base address of the EUSCI_A_SPI module.
//! \param mask is the bit mask of the interrupt sources to be enabled.
//!        Mask value is the logical OR of any of the following:
//!        - \b EUSCI_A_SPI_TRANSMIT_INTERRUPT
//!        - \b EUSCI_A_SPI_RECEIVE_INTERRUPT
//!
//! Modified bits of \b UCAxIFG register and bits of \b UCAxIE register.
//!
//! \return None
//
//*****************************************************************************
void EUSCI_A_SPI_enableInterrupt(uint32_t baseAddress, uint8_t mask)
{
    ASSERT(
            !(mask
                    & ~(EUSCI_A_SPI_RECEIVE_INTERRUPT
                            | EUSCI_A_SPI_TRANSMIT_INTERRUPT)));

    EUSCI_A_CMSIS(baseAddress)->rIE.r |= mask;
}


//*****************************************************************************
//
//! \brief Disables individual SPI interrupt sources.
//!
//! Disables the indicated SPI interrupt sources. Only the sources that are
//! enabled can be reflected to the processor interrupt; disabled sources have
//! no effect on the processor.
//!
//! \param baseAddress is the base address of the EUSCI_A_SPI module.
//! \param mask is the bit mask of the interrupt sources to be disabled.
//!        Mask value is the logical OR of any of the following:
//!        - \b EUSCI_A_SPI_TRANSMIT_INTERRUPT
//!        - \b EUSCI_A_SPI_RECEIVE_INTERRUPT
//!
//! Modified bits of \b UCAxIE register.
//!
//! \return None
//
//*****************************************************************************
void EUSCI_A_SPI_disableInterrupt(uint32_t baseAddress, uint8_t mask)
{
    ASSERT(
            !(mask
                    & ~(EUSCI_A_SPI_RECEIVE_INTERRUPT
                            | EUSCI_A_SPI_TRANSMIT_INTERRUPT)));

    EUSCI_A_CMSIS(baseAddress)->rIE.r &= ~mask;
}


//*****************************************************************************
//
//! \brief Gets the current SPI interrupt status.
//!
//! This returns the interrupt status for the SPI module based on which flag is
//! passed.
//!
//! \param baseAddress is the base address of the EUSCI_A_SPI module.
//! \param mask is the masked interrupt flag status to be returned.
//!        Mask value is the logical OR of any of the following:
//!        - \b EUSCI_A_SPI_TRANSMIT_INTERRUPT
//!        - \b EUSCI_A_SPI_RECEIVE_INTERRUPT
//!
//! \return Logical OR of any of the following:
//!         - \b EUSCI_A_SPI_TRANSMIT_INTERRUPT
//!         - \b EUSCI_A_SPI_RECEIVE_INTERRUPT
//!         \n indicating the status of the masked interrupts
//
//*****************************************************************************
uint8_t EUSCI_A_SPI_getInterruptStatus(uint32_t baseAddress, uint8_t mask)
{
    ASSERT(
            !(mask
                    & ~(EUSCI_A_SPI_RECEIVE_INTERRUPT
                            | EUSCI_A_SPI_TRANSMIT_INTERRUPT)));

    return EUSCI_A_CMSIS(baseAddress)->rIFG.r & mask;
}


//*****************************************************************************
//
//! \brief Enables the SPI block.
//!
//! This will enable operation of the SPI block.
//!
//! \param baseAddress is the base address of the EUSCI_A_SPI module.
//!
//! Modified bits are \b UCSWRST of \b UCAxCTLW0 register.
//!
//! \return None
//
//*****************************************************************************
void EUSCI_A_SPI_enable(uint32_t baseAddress)
{
    //Reset the UCSWRST bit to enable the USCI Module
    BITBAND_PERI(EUSCI_A_CMSIS(baseAddress)->rCTLW0.r, UCSWRST_OFS) = 0;
}


//*****************************************************************************
//
//! \brief Disables the SPI block.
//!
//! This will disable operation of the SPI block.
//!
//! \param baseAddress is the base address of the EUSCI_A_SPI module.
//!
//! Modified bits are \b UCSWRST of \b UCAxCTLW0 register.
//!
//! \return None
//
//*****************************************************************************
void EUSCI_A_SPI_disable(uint32_t baseAddress)
{
    //Set the UCSWRST bit to disable the USCI Module
    BITBAND_PERI(EUSCI_A_CMSIS(baseAddress)->rCTLW0.r, UCSWRST_OFS) = 1;
}


//*****************************************************************************
//
//! \brief Returns the address of the RX Buffer of the SPI for the DMA module.
//!
//! Returns the address of the SPI RX Buffer. This can be used in conjunction
//! with the DMA to store the received data directly to memory.
//!
//! \param baseAddress is the base address of the EUSCI_A_SPI module.
//!
//! \return the address of the RX Buffer
//
//*****************************************************************************
uint32_t EUSCI_A_SPI_getReceiveBufferAddressForDMA(uint32_t baseAddress)
{
    return baseAddress + OFS_UCA0RXBUF;
}


//*****************************************************************************
//
//! \brief Returns the address of the TX Buffer of the SPI for the DMA module.
//!
//! Returns the address of the SPI TX Buffer. This can be used in conjunction
//! with the DMA to obtain transmitted data directly from memory.
//!
//! \param baseAddress is the base address of the EUSCI_A_SPI module.
//!
//! \return the address of the TX Buffer
//
//*****************************************************************************
uint32_t EUSCI_A_SPI_getTransmitBufferAddressForDMA(uint32_t baseAddress)
{
    return baseAddress + OFS_UCA0TXBUF;
}


//*****************************************************************************
//
//! \brief Indicates whether or not the SPI bus is busy.
//!
//! This function returns an indication of whether or not the SPI bus is
//! busy.This function checks the status of the bus via UCBBUSY bit
//!
//! \param baseAddress is the base address of the EUSCI_A_SPI module.
//!
//! \return true if busy, false otherwise
//*****************************************************************************
bool EUSCI_A_SPI_isBusy(uint32_t baseAddress)
{
    //Return the bus busy status.
    return BITBAND_PERI(EUSCI_A_CMSIS(baseAddress)->rSTATW.r, UCBBUSY_OFS);
}


//*****************************************************************************
//
//! \brief Clears the selected SPI interrupt status flag.
//!
//! \param baseAddress is the base address of the EUSCI_A_SPI module.
//! \param mask is the masked interrupt flag to be cleared.
//!        Mask value is the logical OR of any of the following:
//!        - \b EUSCI_A_SPI_TRANSMIT_INTERRUPT
//!        - \b EUSCI_A_SPI_RECEIVE_INTERRUPT
//!
//! Modified bits of \b UCAxIFG register.
//!
//! \return None
//
//*****************************************************************************
void EUSCI_A_SPI_clearInterruptFlag(uint32_t baseAddress, uint8_t mask)
{
    ASSERT(
            !(mask
                    & ~(EUSCI_A_SPI_RECEIVE_INTERRUPT
                            | EUSCI_A_SPI_TRANSMIT_INTERRUPT)));

    EUSCI_A_CMSIS(baseAddress)->rIFG.r &= ~mask;
}


//*****************************************************************************
//
//! \brief Selects 4Pin Functionality
//!
//! This function should be invoked only in 4-wire mode. Invoking this function
//! has no effect in 3-wire mode.
//!
//! \param baseAddress is the base address of the EUSCI_B_SPI module.
//! \param select4PinFunctionality selects 4 pin functionality
//!        Valid values are:
//!        - \b EUSCI_B_SPI_PREVENT_CONFLICTS_WITH_OTHER_MASTERS
//!        - \b EUSCI_B_SPI_ENABLE_SIGNAL_FOR_4WIRE_SLAVE
//!
//! Modified bits are \b UCSTEM of \b UCAxCTLW0 register.
//!
//! \return None
//
//*****************************************************************************
void EUSCI_B_SPI_select4PinFunctionality(uint32_t baseAddress,
        uint8_t select4PinFunctionality)
{
    ASSERT(
            (EUSCI_B_SPI_PREVENT_CONFLICTS_WITH_OTHER_MASTERS
                    == select4PinFunctionality)
            || (EUSCI_B_SPI_ENABLE_SIGNAL_FOR_4WIRE_SLAVE
                    == select4PinFunctionality));

    EUSCI_B_CMSIS(baseAddress)->rCTLW0.r = (EUSCI_B_CMSIS(baseAddress)->rCTLW0.r
            & ~UCSTEM) | select4PinFunctionality;
}


//*****************************************************************************
//
//! \brief Initializes the SPI Master clock. At the end of this function call,
//! SPI module is left enabled.
//!
//! \param baseAddress is the base address of the EUSCI_B_SPI module.
//! \param clockSourceFrequency is the frequency of the slected clock source
//! \param desiredSpiClock is the desired clock rate for SPI communication
//!
//! Modified bits are \b UCSWRST of \b UCAxCTLW0 register.
//!
//! \return None
//
//*****************************************************************************
void EUSCI_B_SPI_masterChangeClock(uint32_t baseAddress,
        uint32_t clockSourceFrequency, uint32_t desiredSpiClock)
{
    //Disable the USCI Module
    BITBAND_PERI(EUSCI_B_CMSIS(baseAddress)->rCTLW0.r, UCSWRST_OFS) = 1;

    EUSCI_B_CMSIS(baseAddress)->rBRW = (uint16_t) (clockSourceFrequency
            / desiredSpiClock);

    //Reset the UCSWRST bit to enable the USCI Module
    BITBAND_PERI(EUSCI_B_CMSIS(baseAddress)->rCTLW0.r, UCSWRST_OFS) = 0;
}


//*****************************************************************************
//
//! \brief Initializes the SPI Slave block.
//!
//! Upon successful initialization of the SPI slave block, this function will
//! have initailized the slave block, but the SPI Slave block still remains
//! disabled and must be enabled with EUSCI_B_SPI_enable()
//!
//! \param baseAddress is the base address of the EUSCI_B_SPI Slave module.
//! \param msbFirst controls the direction of the receive and transmit shift
//!        register.
//!        Valid values are:
//!        - \b EUSCI_B_SPI_MSB_FIRST
//!        - \b EUSCI_B_SPI_LSB_FIRST [Default]
//! \param clockPhase is clock phase select.
//!        Valid values are:
//!        - \b EUSCI_B_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT
//!           [Default]
//!        - \b EUSCI_B_SPI_PHASE_DATA_CAPTURED_ONFIRST_CHANGED_ON_NEXT
//! \param clockPolarity is clock polarity select
//!        Valid values are:
//!        - \b EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_HIGH
//!        - \b EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_LOW [Default]
//! \param spiMode is SPI mode select
//!        Valid values are:
//!        - \b EUSCI_B_SPI_3PIN
//!        - \b EUSCI_B_SPI_4PIN_UCxSTE_ACTIVE_HIGH
//!        - \b EUSCI_B_SPI_4PIN_UCxSTE_ACTIVE_LOW
//!
//! Modified bits are \b UCMSB, \b UCMST, \b UC7BIT, \b UCCKPL, \b UCCKPH, \b
//! UCMODE and \b UCSWRST of \b UCAxCTLW0 register.
//!
//! \return STATUS_SUCCESS
//
//*****************************************************************************
bool EUSCI_B_SPI_slaveInit(uint32_t baseAddress, uint16_t msbFirst,
        uint16_t clockPhase, uint16_t clockPolarity, uint16_t spiMode)
{
    ASSERT(
            (EUSCI_B_SPI_MSB_FIRST == msbFirst)
            || (EUSCI_B_SPI_LSB_FIRST == msbFirst));

    ASSERT(
            (EUSCI_B_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT
                    == clockPhase)
            || (EUSCI_B_SPI_PHASE_DATA_CAPTURED_ONFIRST_CHANGED_ON_NEXT
                    == clockPhase));

    ASSERT(
            (EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_HIGH == clockPolarity)
            || (EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_LOW
                    == clockPolarity));

    ASSERT(
            (EUSCI_B_SPI_3PIN == spiMode)
            || (EUSCI_B_SPI_4PIN_UCxSTE_ACTIVE_HIGH == spiMode)
            || (EUSCI_B_SPI_4PIN_UCxSTE_ACTIVE_LOW == spiMode));

    //Disable USCI Module
    BITBAND_PERI(EUSCI_B_CMSIS(baseAddress)->rCTLW0.r, UCSWRST_OFS) = 1;

    //Reset OFS_UCBxCTLW0 register
    EUSCI_B_CMSIS(baseAddress)->rCTLW0.r = (EUSCI_B_CMSIS(baseAddress)->rCTLW0.r
            & ~(UCMSB + UC7BIT + UCMST + UCCKPL + UCCKPH + UCMODE_3))
            | (clockPhase + clockPolarity + msbFirst + UCSYNC + spiMode);

    return true;
}


//*****************************************************************************
//
//! \brief Changes the SPI colock phase and polarity. At the end of this
//! function call, SPI module is left enabled.
//!
//! \param baseAddress is the base address of the EUSCI_B_SPI module.
//! \param clockPhase is clock phase select.
//!        Valid values are:
//!        - \b EUSCI_B_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT
//!           [Default]
//!        - \b EUSCI_B_SPI_PHASE_DATA_CAPTURED_ONFIRST_CHANGED_ON_NEXT
//! \param clockPolarity is clock polarity select
//!        Valid values are:
//!        - \b EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_HIGH
//!        - \b EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_LOW [Default]
//!
//! Modified bits are \b UCCKPL, \b UCCKPH and \b UCSWRST of \b UCAxCTLW0
//! register.
//!
//! \return None
//
//*****************************************************************************
void EUSCI_B_SPI_changeClockPhasePolarity(uint32_t baseAddress,
        uint16_t clockPhase, uint16_t clockPolarity)
{

    ASSERT(
            (EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_HIGH == clockPolarity)
            || (EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_LOW
                    == clockPolarity));

    ASSERT(
            (EUSCI_B_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT
                    == clockPhase)
            || (EUSCI_B_SPI_PHASE_DATA_CAPTURED_ONFIRST_CHANGED_ON_NEXT
                    == clockPhase));

    //Disable the USCI Module
    BITBAND_PERI(EUSCI_B_CMSIS(baseAddress)->rCTLW0.r, UCSWRST_OFS) = 1;

    EUSCI_B_CMSIS(baseAddress)->rCTLW0.r = (EUSCI_B_CMSIS(baseAddress)->rCTLW0.r
            & ~(UCCKPH + UCCKPL)) | (clockPhase + clockPolarity);

    //Reset the UCSWRST bit to enable the USCI Module
    BITBAND_PERI(EUSCI_B_CMSIS(baseAddress)->rCTLW0.r, UCSWRST_OFS) = 0;
}


//*****************************************************************************
//
//! \brief Transmits a byte from the SPI Module.
//!
//! This function will place the supplied data into SPI trasmit data register
//! to start transmission.
//!
//! \param baseAddress is the base address of the EUSCI_B_SPI module.
//! \param transmitData data to be transmitted from the SPI module
//!
//! \return None
//
//*****************************************************************************
void EUSCI_B_SPI_transmitData(uint32_t baseAddress, uint8_t transmitData)
{
    EUSCI_B_CMSIS(baseAddress)->rTXBUF.r = transmitData;
}


//*****************************************************************************
//
//! \brief Receives a byte that has been sent to the SPI Module.
//!
//! This function reads a byte of data from the SPI receive data Register.
//!
//! \param baseAddress is the base address of the EUSCI_B_SPI module.
//!
//! \return Returns the byte received from by the SPI module, cast as an
//!         uint8_t.
//
//*****************************************************************************
uint8_t EUSCI_B_SPI_receiveData(uint32_t baseAddress)
{
    return EUSCI_B_CMSIS(baseAddress)->rRXBUF.r;
}


//*****************************************************************************
//
//! \brief Enables individual SPI interrupt sources.
//!
//! Enables the indicated SPI interrupt sources.  Only the sources that are
//! enabled can be reflected to the processor interrupt; disabled sources have
//! no effect on the processor. Does not clear interrupt flags.
//!
//! \param baseAddress is the base address of the EUSCI_B_SPI module.
//! \param mask is the bit mask of the interrupt sources to be enabled.
//!        Mask value is the logical OR of any of the following:
//!        - \b EUSCI_B_SPI_TRANSMIT_INTERRUPT
//!        - \b EUSCI_B_SPI_RECEIVE_INTERRUPT
//!
//! Modified bits of \b UCAxIFG register and bits of \b UCAxIE register.
//!
//! \return None
//
//*****************************************************************************
void EUSCI_B_SPI_enableInterrupt(uint32_t baseAddress, uint8_t mask)
{
    ASSERT(
            !(mask
                    & ~(EUSCI_B_SPI_RECEIVE_INTERRUPT
                            | EUSCI_B_SPI_TRANSMIT_INTERRUPT)));

    EUSCI_B_CMSIS(baseAddress)->rIE.r |= mask;
}


//*****************************************************************************
//
//! \brief Disables individual SPI interrupt sources.
//!
//! Disables the indicated SPI interrupt sources. Only the sources that are
//! enabled can be reflected to the processor interrupt; disabled sources have
//! no effect on the processor.
//!
//! \param baseAddress is the base address of the EUSCI_B_SPI module.
//! \param mask is the bit mask of the interrupt sources to be disabled.
//!        Mask value is the logical OR of any of the following:
//!        - \b EUSCI_B_SPI_TRANSMIT_INTERRUPT
//!        - \b EUSCI_B_SPI_RECEIVE_INTERRUPT
//!
//! Modified bits of \b UCAxIE register.
//!
//! \return None
//
//*****************************************************************************
void EUSCI_B_SPI_disableInterrupt(uint32_t baseAddress, uint8_t mask)
{
    ASSERT(
            !(mask
                    & ~(EUSCI_B_SPI_RECEIVE_INTERRUPT
                            | EUSCI_B_SPI_TRANSMIT_INTERRUPT)));

    EUSCI_B_CMSIS(baseAddress)->rIE.r &= ~mask;
}


//*****************************************************************************
//
//! \brief Gets the current SPI interrupt status.
//!
//! This returns the interrupt status for the SPI module based on which flag is
//! passed.
//!
//! \param baseAddress is the base address of the EUSCI_B_SPI module.
//! \param mask is the masked interrupt flag status to be returned.
//!        Mask value is the logical OR of any of the following:
//!        - \b EUSCI_B_SPI_TRANSMIT_INTERRUPT
//!        - \b EUSCI_B_SPI_RECEIVE_INTERRUPT
//!
//! \return Logical OR of any of the following:
//!         - \b EUSCI_B_SPI_TRANSMIT_INTERRUPT
//!         - \b EUSCI_B_SPI_RECEIVE_INTERRUPT
//!         \n indicating the status of the masked interrupts
//
//*****************************************************************************
uint8_t EUSCI_B_SPI_getInterruptStatus(uint32_t baseAddress, uint8_t mask)
{
    ASSERT(
            !(mask
                    & ~(EUSCI_B_SPI_RECEIVE_INTERRUPT
                            | EUSCI_B_SPI_TRANSMIT_INTERRUPT)));

    return EUSCI_B_CMSIS(baseAddress)->rIFG.r & mask;
}


//*****************************************************************************
//
//! \brief Enables the SPI block.
//!
//! This will enable operation of the SPI block.
//!
//! \param baseAddress is the base address of the EUSCI_B_SPI module.
//!
//! Modified bits are \b UCSWRST of \b UCBxCTLW0 register.
//!
//! \return None
//
//*****************************************************************************
void EUSCI_B_SPI_enable(uint32_t baseAddress)
{
    //Reset the UCSWRST bit to enable the USCI Module
    BITBAND_PERI(EUSCI_B_CMSIS(baseAddress)->rCTLW0.r, UCSWRST_OFS) = 0;
}


//*****************************************************************************
//
//! \brief Disables the SPI block.
//!
//! This will disable operation of the SPI block.
//!
//! \param baseAddress is the base address of the EUSCI_B_SPI module.
//!
//! Modified bits are \b UCSWRST of \b UCBxCTLW0 register.
//!
//! \return None
//
//*****************************************************************************
void EUSCI_B_SPI_disable(uint32_t baseAddress)
{
    //Set the UCSWRST bit to disable the USCI Module
    BITBAND_PERI(EUSCI_B_CMSIS(baseAddress)->rCTLW0.r, UCSWRST_OFS) = 1;
}


//*****************************************************************************
//
//! \brief Returns the address of the RX Buffer of the SPI for the DMA module.
//!
//! Returns the address of the SPI RX Buffer. This can be used in conjunction
//! with the DMA to store the received data directly to memory.
//!
//! \param baseAddress is the base address of the EUSCI_B_SPI module.
//!
//! \return the address of the RX Buffer
//
//*****************************************************************************
uint32_t EUSCI_B_SPI_getReceiveBufferAddressForDMA(uint32_t baseAddress)
{
    return baseAddress + OFS_UCB0RXBUF;
}


//*****************************************************************************
//
//! \brief Returns the address of the TX Buffer of the SPI for the DMA module.
//!
//! Returns the address of the SPI TX Buffer. This can be used in conjunction
//! with the DMA to obtain transmitted data directly from memory.
//!
//! \param baseAddress is the base address of the EUSCI_B_SPI module.
//!
//! \return the address of the TX Buffer
//
//*****************************************************************************
uint32_t EUSCI_B_SPI_getTransmitBufferAddressForDMA(uint32_t baseAddress)
{
    return baseAddress + OFS_UCB0TXBUF;
}


//*****************************************************************************
//
//! \brief Indicates whether or not the SPI bus is busy.
//!
//! This function returns an indication of whether or not the SPI bus is
//! busy.This function checks the status of the bus via UCBBUSY bit
//!
//! \param baseAddress is the base address of the EUSCI_B_SPI module.
//!
//! \return true if busy, false otherwise
//
//*****************************************************************************
bool EUSCI_B_SPI_isBusy(uint32_t baseAddress)
{
    //Return the bus busy status.
    return BITBAND_PERI(EUSCI_B_CMSIS(baseAddress)->rSTATW.r, UCBBUSY_OFS);
}


//*****************************************************************************
//
//! \brief Clears the selected SPI interrupt status flag.
//!
//! \param baseAddress is the base address of the EUSCI_B_SPI module.
//! \param mask is the masked interrupt flag to be cleared.
//!        Mask value is the logical OR of any of the following:
//!        - \b EUSCI_B_SPI_TRANSMIT_INTERRUPT
//!        - \b EUSCI_B_SPI_RECEIVE_INTERRUPT
//!
//! Modified bits of \b UCAxIFG register.
//!
//! \return None
//
//*****************************************************************************
void EUSCI_B_SPI_clearInterruptFlag(uint32_t baseAddress, uint8_t mask)
{
    ASSERT(
            !(mask
                    & ~(EUSCI_B_SPI_RECEIVE_INTERRUPT
                            | EUSCI_B_SPI_TRANSMIT_INTERRUPT)));

    EUSCI_B_CMSIS(baseAddress)->rIFG.r &= ~mask;
}


uint_least32_t SysCtl_getSRAMSize(void)
{
    return SYSCTL->rSRAM_SIZE;
}


uint_least32_t SysCtl_getFlashSize(void)
{
    return SYSCTL->rFLASH_SIZE;
}


void SysCtl_rebootDevice(void)
{
    SYSCTL->rREBOOT_CTL.r = (SYSCTL_REBOOT_CTL_REBOOT | SYSCTL_REBOOT_KEY);
}


void SysCtl_enableSRAMBank(uint_fast8_t sramBank)
{
    ASSERT(SysCtlSRAMBankValid(sramBank));

    /* Waiting for SRAM Ready Bit to be set */
    while (!SYSCTL->rSRAM_BANKEN.b.bSRAM_RDY)
        ;

    SYSCTL->rSRAM_BANKEN.r = (sramBank | SYSCTL_SRAM_BANKEN_BNK0_EN);
}


void SysCtl_disableSRAMBank(uint_fast8_t sramBank)
{
    ASSERT(SysCtlSRAMBankValid(sramBank));

    /* Waiting for SRAM Ready Bit to be set */
    while (!SYSCTL->rSRAM_BANKEN.b.bSRAM_RDY)
        ;      

    switch (sramBank)
    {
    case SYSCTL_SRAM_BANK7:
        sramBank = SYSCTL_SRAM_BANK6 +  SYSCTL_SRAM_BANK5 + SYSCTL_SRAM_BANK4
                        + SYSCTL_SRAM_BANK3 + SYSCTL_SRAM_BANK2 
                        + SYSCTL_SRAM_BANK1;
        break;
    case SYSCTL_SRAM_BANK6:
        sramBank =   SYSCTL_SRAM_BANK5 + SYSCTL_SRAM_BANK4
                        + SYSCTL_SRAM_BANK3 + SYSCTL_SRAM_BANK2 
                        + SYSCTL_SRAM_BANK1;
        break;
    case SYSCTL_SRAM_BANK5:
        sramBank =   SYSCTL_SRAM_BANK4 + SYSCTL_SRAM_BANK3  
                + SYSCTL_SRAM_BANK2 + SYSCTL_SRAM_BANK1;
        break;
    case SYSCTL_SRAM_BANK4:
        sramBank =   SYSCTL_SRAM_BANK3 + SYSCTL_SRAM_BANK2 
                        + SYSCTL_SRAM_BANK1;
        break;
    case SYSCTL_SRAM_BANK3:
        sramBank =   SYSCTL_SRAM_BANK2 + SYSCTL_SRAM_BANK1;
        break;
    case SYSCTL_SRAM_BANK2:
        sramBank = SYSCTL_SRAM_BANK1;
        break;
    case SYSCTL_SRAM_BANK1:
        sramBank = 0;
        break;
    default:
        return;
    }

    SYSCTL->rSRAM_BANKEN.r = (sramBank | SYSCTL_SRAM_BANKEN_BNK0_EN);
}


void SysCtl_enableSRAMBankRetention(uint_fast8_t sramBank)
{
    ASSERT(SysCtlSRAMBankValidRet(sramBank));

    /* Waiting for SRAM Ready Bit to be set */
    while (!SYSCTL->rSRAM_BANKRET.b.bSRAM_RDY)
        ;

    SYSCTL->rSRAM_BANKRET.r |= sramBank;
}


void SysCtl_disableSRAMBankRetention(uint_fast8_t sramBank)
{
    ASSERT(SysCtlSRAMBankValidRet(sramBank));

    /* Waiting for SRAM Ready Bit to be set */
    while (!SYSCTL->rSRAM_BANKRET.b.bSRAM_RDY)
        ;

    SYSCTL->rSRAM_BANKRET.r &= ~sramBank;
}


void SysCtl_enablePeripheralAtCPUHalt(uint_fast16_t devices)
{
    ASSERT(SysCtlPeripheralIsValid(devices));
    SYSCTL->rPERIHALT_CTL.r &= ~devices;
}


void SysCtl_disablePeripheralAtCPUHalt(uint_fast16_t devices)
{
    ASSERT(SysCtlPeripheralIsValid(devices));
    SYSCTL->rPERIHALT_CTL.r |= devices;
}


void SysCtl_setWDTTimeoutResetType(uint_fast8_t resetType)
{
    if (resetType)
        SYSCTL->rWDTRESET_CTL.r |=
                SYSCTL_WDTRESET_CTL_TIMEOUT;
    else
        SYSCTL->rWDTRESET_CTL.r &= ~SYSCTL_WDTRESET_CTL_TIMEOUT;
}


void SysCtl_setWDTPasswordViolationResetType(uint_fast8_t resetType)
{
    ASSERT(resetType <= SYSCTL_HARD_RESET);

    if (resetType)
        SYSCTL->rWDTRESET_CTL.r |=
                SYSCTL_WDTRESET_CTL_VIOLATION;
    else
        SYSCTL->rWDTRESET_CTL.r &= ~SYSCTL_WDTRESET_CTL_VIOLATION;
}


uint_fast16_t SysCtl_getTempCalibrationConstant(uint32_t refVoltage,
        uint32_t temperature)
{
    return HWREG16(TLV_BASE + refVoltage + temperature);
}


void SysCtl_enableGlitchFilter(void)
{
    SYSCTL->rDIO_GLTFLT_CTL.r |= SYSCTL_DIO_GLTFLT_CTL_GLTCH_EN;
}


void SysCtl_disableGlitchFilter(void)
{
    SYSCTL->rDIO_GLTFLT_CTL.r &= ~SYSCTL_DIO_GLTFLT_CTL_GLTCH_EN;
}


void SysCtl_getTLVInfo(uint_fast8_t tag, uint_fast8_t instance,
        uint_fast8_t *length, uint32_t **data_address)
{
    /* TLV Structure Start Address */
    uint32_t *TLV_address = (uint32_t *) TLV_START;

    while (((*TLV_address != tag)) // check for tag and instance
            && (*TLV_address != TLV_TAGEND))         // do range check first
    {
        if (*TLV_address == tag)
        {
            if(instance == 0)
            {
                break;
            }

            /* Repeat until requested instance is reached */
            instance--;
        }

        TLV_address += (*(TLV_address + 1)) + 2;
    }

    /* Check if Tag match happened... */
    if (*TLV_address == tag)
    {
        /* Return length = Address + 1 */
        *length = (*(TLV_address + 1))*4;
        /* Return address of first data/value info = Address + 2 */
        *data_address = (uint32_t *) (TLV_address + 2);
    }
    // If there was no tag match and the end of TLV structure was reached..
    else
    {
        // Return 0 for TAG not found
        *length = 0;
        // Return 0 for TAG not found
        *data_address = 0;
    }
}


void SysCtl_disableNMISource(uint_fast8_t flags)
{
    SYSCTL->rNMI_CTLSTAT.r &= ~(flags);
}


void SysCtl_enableNMISource(uint_fast8_t flags)
{
    SYSCTL->rNMI_CTLSTAT.r |= flags;
}


uint_fast8_t SysCtl_getNMISourceStatus(void)
{
    return SYSCTL->rNMI_CTLSTAT.r;
}


void SysTick_enableModule(void)
{
    //
    // Enable SysTick.
    //
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
}


void SysTick_disableModule(void)
{
    //
    // Disable SysTick.
    //
    SysTick->CTRL &= ~(SysTick_CTRL_ENABLE_Msk);
}


void SysTick_enableInterrupt(void)
{
    //
    // Enable the SysTick interrupt.
    //
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
}


void SysTick_disableInterrupt(void)
{
    //
    // Disable the SysTick interrupt.
    //
    SysTick->CTRL &= ~(SysTick_CTRL_TICKINT_Msk);
}


void SysTick_setPeriod(uint32_t period)
{
    //
    // Check the arguments.
    //
    ASSERT((period > 0) && (period <= 16777216));

    //
    // Set the period of the SysTick counter.
    //
    SysTick->LOAD = period - 1;
}


uint32_t SysTick_getPeriod(void)
{
    //
    // Return the period of the SysTick counter.
    //
    return (SysTick->LOAD + 1);
}


uint32_t SysTick_getValue(void)
{
    //
    // Return the current value of the SysTick counter.
    //
    return (SysTick->VAL);
}


static void privateTimer_AProcessClockSourceDivider(uint32_t timer,
        uint16_t clockSourceDivider)
{
    TIMER_A_CMSIS(timer)->rCTL.r &= ~ID__8;
    TIMER_A_CMSIS(timer)->rEX0.r &= ~TAIDEX_7;

    switch (clockSourceDivider)
    {
    case TIMER_A_CLOCKSOURCE_DIVIDER_1:
    case TIMER_A_CLOCKSOURCE_DIVIDER_2:
        TIMER_A_CMSIS(timer)->rCTL.r |= ((clockSourceDivider - 1) << 6);
        TIMER_A_CMSIS(timer)->rEX0.r = TAIDEX_0;
        break;
    case TIMER_A_CLOCKSOURCE_DIVIDER_4:
        TIMER_A_CMSIS(timer)->rCTL.r |= ID__4;
        TIMER_A_CMSIS(timer)->rEX0.r = TAIDEX_0;
        break;
    case TIMER_A_CLOCKSOURCE_DIVIDER_8:
        TIMER_A_CMSIS(timer)->rCTL.r |= ID__8;
        TIMER_A_CMSIS(timer)->rEX0.r = TAIDEX_0;
        break;
    case TIMER_A_CLOCKSOURCE_DIVIDER_3:
    case TIMER_A_CLOCKSOURCE_DIVIDER_5:
    case TIMER_A_CLOCKSOURCE_DIVIDER_6:
    case TIMER_A_CLOCKSOURCE_DIVIDER_7:
        TIMER_A_CMSIS(timer)->rCTL.r |= ID__1;
        TIMER_A_CMSIS(timer)->rEX0.r = (clockSourceDivider - 1);
        break;

    case TIMER_A_CLOCKSOURCE_DIVIDER_10:
    case TIMER_A_CLOCKSOURCE_DIVIDER_12:
    case TIMER_A_CLOCKSOURCE_DIVIDER_14:
    case TIMER_A_CLOCKSOURCE_DIVIDER_16:
        TIMER_A_CMSIS(timer)->rCTL.r |= ID__2;
        TIMER_A_CMSIS(timer)->rEX0.r = (clockSourceDivider / 2 - 1);
        break;

    case TIMER_A_CLOCKSOURCE_DIVIDER_20:
    case TIMER_A_CLOCKSOURCE_DIVIDER_24:
    case TIMER_A_CLOCKSOURCE_DIVIDER_28:
    case TIMER_A_CLOCKSOURCE_DIVIDER_32:
        TIMER_A_CMSIS(timer)->rCTL.r |= ID__4;
        TIMER_A_CMSIS(timer)->rEX0.r = (clockSourceDivider / 4 - 1);
        break;
    case TIMER_A_CLOCKSOURCE_DIVIDER_40:
    case TIMER_A_CLOCKSOURCE_DIVIDER_48:
    case TIMER_A_CLOCKSOURCE_DIVIDER_56:
    case TIMER_A_CLOCKSOURCE_DIVIDER_64:
        TIMER_A_CMSIS(timer)->rCTL.r |= ID__8;
        TIMER_A_CMSIS(timer)->rEX0.r = (clockSourceDivider / 8 - 1);
        break;
    }
}


uint16_t Timer_A_getCounterValue(uint32_t timer)
{
    uint_fast16_t voteOne, voteTwo, res;

    voteTwo = TIMER_A_CMSIS(timer)->rR;

    do
    {
        voteOne = voteTwo;
        voteTwo = TIMER_A_CMSIS(timer)->rR;

        if (voteTwo > voteOne)
            res = voteTwo - voteOne;
        else if (voteOne > voteTwo)
            res = voteOne - voteTwo;
        else
            res = 0;

    } while (res > TIMER_A_THRESHOLD);

    return voteTwo;

}


void Timer_A_startCounter(uint32_t timer, uint_fast16_t timerMode)
{
    ASSERT(
            (TIMER_A_UPDOWN_MODE == timerMode)
            || (TIMER_A_CONTINUOUS_MODE == timerMode)
            || (TIMER_A_UP_MODE == timerMode));

    TIMER_A_CMSIS(timer)->rCTL.r |= timerMode;
}


void Timer_A_configureContinuousMode(uint32_t timer,
        const Timer_A_ContinuousModeConfig *config)
{
    ASSERT(
            (TIMER_A_CLOCKSOURCE_EXTERNAL_TXCLK == config->clockSource)
            || (TIMER_A_CLOCKSOURCE_ACLK == config->clockSource)
            || (TIMER_A_CLOCKSOURCE_SMCLK == config->clockSource)
            || (TIMER_A_CLOCKSOURCE_INVERTED_EXTERNAL_TXCLK
                    == config->clockSource));

    ASSERT(
            (TIMER_A_DO_CLEAR == config->timerClear)
            || (TIMER_A_SKIP_CLEAR == config->timerClear));

    ASSERT(
            (TIMER_A_TAIE_INTERRUPT_ENABLE == config->timerInterruptEnable_TAIE)
            || (TIMER_A_TAIE_INTERRUPT_DISABLE
                    == config->timerInterruptEnable_TAIE));

    ASSERT(
            (TIMER_A_CLOCKSOURCE_DIVIDER_1 == config->clockSourceDivider)
            || (TIMER_A_CLOCKSOURCE_DIVIDER_2
                    == config->clockSourceDivider)
            || (TIMER_A_CLOCKSOURCE_DIVIDER_4
                    == config->clockSourceDivider)
            || (TIMER_A_CLOCKSOURCE_DIVIDER_8
                    == config->clockSourceDivider)
            || (TIMER_A_CLOCKSOURCE_DIVIDER_3
                    == config->clockSourceDivider)
            || (TIMER_A_CLOCKSOURCE_DIVIDER_5
                    == config->clockSourceDivider)
            || (TIMER_A_CLOCKSOURCE_DIVIDER_6
                    == config->clockSourceDivider)
            || (TIMER_A_CLOCKSOURCE_DIVIDER_7
                    == config->clockSourceDivider)
            || (TIMER_A_CLOCKSOURCE_DIVIDER_10
                    == config->clockSourceDivider)
            || (TIMER_A_CLOCKSOURCE_DIVIDER_12
                    == config->clockSourceDivider)
            || (TIMER_A_CLOCKSOURCE_DIVIDER_14
                    == config->clockSourceDivider)
            || (TIMER_A_CLOCKSOURCE_DIVIDER_16
                    == config->clockSourceDivider)
            || (TIMER_A_CLOCKSOURCE_DIVIDER_20
                    == config->clockSourceDivider)
            || (TIMER_A_CLOCKSOURCE_DIVIDER_24
                    == config->clockSourceDivider)
            || (TIMER_A_CLOCKSOURCE_DIVIDER_28
                    == config->clockSourceDivider)
            || (TIMER_A_CLOCKSOURCE_DIVIDER_32
                    == config->clockSourceDivider)
            || (TIMER_A_CLOCKSOURCE_DIVIDER_40
                    == config->clockSourceDivider)
            || (TIMER_A_CLOCKSOURCE_DIVIDER_48
                    == config->clockSourceDivider)
            || (TIMER_A_CLOCKSOURCE_DIVIDER_56
                    == config->clockSourceDivider)
            || (TIMER_A_CLOCKSOURCE_DIVIDER_64
                    == config->clockSourceDivider));

    privateTimer_AProcessClockSourceDivider(timer, config->clockSourceDivider);

    TIMER_A_CMSIS(timer)->rCTL.r = (TIMER_A_CMSIS(timer)->rCTL.r
            & ~(TIMER_A_CLOCKSOURCE_INVERTED_EXTERNAL_TXCLK
                    + TIMER_A_UPDOWN_MODE + TIMER_A_DO_CLEAR
                    + TIMER_A_TAIE_INTERRUPT_ENABLE))
            | (config->clockSource + config->timerClear
                    + config->timerInterruptEnable_TAIE);
}


void Timer_A_configureUpMode(uint32_t timer, const Timer_A_UpModeConfig *config)
{
    ASSERT(
            (TIMER_A_CLOCKSOURCE_EXTERNAL_TXCLK == config->clockSource)
            || (TIMER_A_CLOCKSOURCE_ACLK == config->clockSource)
            || (TIMER_A_CLOCKSOURCE_SMCLK == config->clockSource)
            || (TIMER_A_CLOCKSOURCE_INVERTED_EXTERNAL_TXCLK
                    == config->clockSource));

    ASSERT(
            (TIMER_A_DO_CLEAR == config->timerClear)
            || (TIMER_A_SKIP_CLEAR == config->timerClear));

    ASSERT(
            (TIMER_A_DO_CLEAR == config->timerClear)
            || (TIMER_A_SKIP_CLEAR == config->timerClear));

    privateTimer_AProcessClockSourceDivider(timer, config->clockSourceDivider);

    TIMER_A_CMSIS(timer)->rCTL.r &=
            ~(TIMER_A_CLOCKSOURCE_INVERTED_EXTERNAL_TXCLK + TIMER_A_UPDOWN_MODE
                    + TIMER_A_DO_CLEAR + TIMER_A_TAIE_INTERRUPT_ENABLE);

    TIMER_A_CMSIS(timer)->rCTL.r |= (config->clockSource + config->timerClear
            + config->timerInterruptEnable_TAIE);

    if (TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE
            == config->captureCompareInterruptEnable_CCR0_CCIE)
        BITBAND_PERI(TIMER_A_CMSIS(timer)->rCCTL0.r,CCIE_OFS) = 1;
    else
        BITBAND_PERI(TIMER_A_CMSIS(timer)->rCCTL0.r,CCIE_OFS) = 0;

    TIMER_A_CMSIS(timer)->rCCR0 = config->timerPeriod;
}


void Timer_A_configureUpDownMode(uint32_t timer,
        const Timer_A_UpDownModeConfig *config)
{
    ASSERT(
            (TIMER_A_CLOCKSOURCE_EXTERNAL_TXCLK == config->clockSource)
            || (TIMER_A_CLOCKSOURCE_ACLK == config->clockSource)
            || (TIMER_A_CLOCKSOURCE_SMCLK == config->clockSource)
            || (TIMER_A_CLOCKSOURCE_INVERTED_EXTERNAL_TXCLK
                    == config->clockSource));

    ASSERT(
            (TIMER_A_DO_CLEAR == config->timerClear)
            || (TIMER_A_SKIP_CLEAR == config->timerClear));

    ASSERT(
            (TIMER_A_DO_CLEAR == config->timerClear)
            || (TIMER_A_SKIP_CLEAR == config->timerClear));

    privateTimer_AProcessClockSourceDivider(timer, config->clockSourceDivider);

    TIMER_A_CMSIS(timer)->rCTL.r &=
            ~(TIMER_A_CLOCKSOURCE_INVERTED_EXTERNAL_TXCLK + TIMER_A_UPDOWN_MODE
                    + TIMER_A_DO_CLEAR + TIMER_A_TAIE_INTERRUPT_ENABLE);

    TIMER_A_CMSIS(timer)->rCTL.r |= (config->clockSource + TIMER_A_STOP_MODE
            + config->timerClear + config->timerInterruptEnable_TAIE);
    if (TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE
            == config->captureCompareInterruptEnable_CCR0_CCIE)
        BITBAND_PERI(TIMER_A_CMSIS(timer)->rCCTL0.r,CCIE_OFS) = 1;
    else
        BITBAND_PERI(TIMER_A_CMSIS(timer)->rCCTL0.r,CCIE_OFS) = 0;

    TIMER_A_CMSIS(timer)->rCCR0 = config->timerPeriod;
}


void Timer_A_initCapture(uint32_t timer,
        const Timer_A_CaptureModeConfig *config)
{
    ASSERT(
            (TIMER_A_CAPTURECOMPARE_REGISTER_0 == config->captureRegister)
            || (TIMER_A_CAPTURECOMPARE_REGISTER_1
                    == config->captureRegister)
            || (TIMER_A_CAPTURECOMPARE_REGISTER_2
                    == config->captureRegister)
            || (TIMER_A_CAPTURECOMPARE_REGISTER_3
                    == config->captureRegister)
            || (TIMER_A_CAPTURECOMPARE_REGISTER_4
                    == config->captureRegister)
            || (TIMER_A_CAPTURECOMPARE_REGISTER_5
                    == config->captureRegister)
            || (TIMER_A_CAPTURECOMPARE_REGISTER_6
                    == config->captureRegister));

    ASSERT(
            (TIMER_A_CAPTUREMODE_NO_CAPTURE == config->captureMode)
            || (TIMER_A_CAPTUREMODE_RISING_EDGE == config->captureMode)
            || (TIMER_A_CAPTUREMODE_FALLING_EDGE == config->captureMode)
            || (TIMER_A_CAPTUREMODE_RISING_AND_FALLING_EDGE
                    == config->captureMode));

    ASSERT(
            (TIMER_A_CAPTURE_INPUTSELECT_CCIxA == config->captureInputSelect)
            || (TIMER_A_CAPTURE_INPUTSELECT_CCIxB
                    == config->captureInputSelect)
            || (TIMER_A_CAPTURE_INPUTSELECT_GND
                    == config->captureInputSelect)
            || (TIMER_A_CAPTURE_INPUTSELECT_Vcc
                    == config->captureInputSelect));

    ASSERT(
            (TIMER_A_CAPTURE_ASYNCHRONOUS == config->synchronizeCaptureSource)
            || (TIMER_A_CAPTURE_SYNCHRONOUS
                    == config->synchronizeCaptureSource));

    ASSERT(
            (TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE
                    == config->captureInterruptEnable)
            || (TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE
                    == config->captureInterruptEnable));

    ASSERT(
            (TIMER_A_OUTPUTMODE_OUTBITVALUE == config->captureOutputMode)
            || (TIMER_A_OUTPUTMODE_SET == config->captureOutputMode)
            || (TIMER_A_OUTPUTMODE_TOGGLE_RESET
                    == config->captureOutputMode)
            || (TIMER_A_OUTPUTMODE_SET_RESET
                    == config->captureOutputMode)
            || (TIMER_A_OUTPUTMODE_TOGGLE == config->captureOutputMode)
            || (TIMER_A_OUTPUTMODE_RESET == config->captureOutputMode)
            || (TIMER_A_OUTPUTMODE_TOGGLE_SET
                    == config->captureOutputMode)
            || (TIMER_A_OUTPUTMODE_RESET_SET
                    == config->captureOutputMode));

    if (TIMER_A_CAPTURECOMPARE_REGISTER_0 == config->captureRegister)
    {
        //CaptureCompare register 0 only supports certain modes
        ASSERT(
                (TIMER_A_OUTPUTMODE_OUTBITVALUE == config->captureOutputMode)
                || (TIMER_A_OUTPUTMODE_SET == config->captureOutputMode)
                || (TIMER_A_OUTPUTMODE_TOGGLE
                        == config->captureOutputMode)
                || (TIMER_A_OUTPUTMODE_RESET
                        == config->captureOutputMode));
    }

    HWREG16(timer + config->captureRegister) =
            (HWREG16(timer + config->captureRegister)
                    & ~(TIMER_A_CAPTUREMODE_RISING_AND_FALLING_EDGE
                            | TIMER_A_CAPTURE_INPUTSELECT_Vcc
                            | TIMER_A_CAPTURE_SYNCHRONOUS | TIMER_A_DO_CLEAR
                            | TIMER_A_TAIE_INTERRUPT_ENABLE | CM_3))
                    | (config->captureMode | config->captureInputSelect
                            | config->synchronizeCaptureSource
                            | config->captureInterruptEnable
                            | config->captureOutputMode | CAP);
}


void Timer_A_initCompare(uint32_t timer,
        const Timer_A_CompareModeConfig *config)
{
    ASSERT(
            (TIMER_A_CAPTURECOMPARE_REGISTER_0 == config->compareRegister)
            || (TIMER_A_CAPTURECOMPARE_REGISTER_1
                    == config->compareRegister)
            || (TIMER_A_CAPTURECOMPARE_REGISTER_2
                    == config->compareRegister)
            || (TIMER_A_CAPTURECOMPARE_REGISTER_3
                    == config->compareRegister)
            || (TIMER_A_CAPTURECOMPARE_REGISTER_4
                    == config->compareRegister)
            || (TIMER_A_CAPTURECOMPARE_REGISTER_5
                    == config->compareRegister)
            || (TIMER_A_CAPTURECOMPARE_REGISTER_6
                    == config->compareRegister));

    ASSERT(
            (TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE
                    == config->compareInterruptEnable)
            || (TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE
                    == config->compareInterruptEnable));

    ASSERT(
            (TIMER_A_OUTPUTMODE_OUTBITVALUE == config->compareOutputMode)
            || (TIMER_A_OUTPUTMODE_SET == config->compareOutputMode)
            || (TIMER_A_OUTPUTMODE_TOGGLE_RESET
                    == config->compareOutputMode)
            || (TIMER_A_OUTPUTMODE_SET_RESET
                    == config->compareOutputMode)
            || (TIMER_A_OUTPUTMODE_TOGGLE == config->compareOutputMode)
            || (TIMER_A_OUTPUTMODE_RESET == config->compareOutputMode)
            || (TIMER_A_OUTPUTMODE_TOGGLE_SET
                    == config->compareOutputMode)
            || (TIMER_A_OUTPUTMODE_RESET_SET
                    == config->compareOutputMode));

    if (TIMER_A_CAPTURECOMPARE_REGISTER_0 == config->compareRegister)
    {
        //CaptureCompare register 0 only supports certain modes
        ASSERT(
                (TIMER_A_OUTPUTMODE_OUTBITVALUE == config->compareOutputMode)
                || (TIMER_A_OUTPUTMODE_SET == config->compareOutputMode)
                || (TIMER_A_OUTPUTMODE_TOGGLE
                        == config->compareOutputMode)
                || (TIMER_A_OUTPUTMODE_RESET
                        == config->compareOutputMode));
    }

    HWREG16(timer + config->compareRegister) =
            (HWREG16(timer + config->compareRegister)
                    & ~(TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE
                            | TIMER_A_OUTPUTMODE_RESET_SET | CAP))
                    | (config->compareInterruptEnable
                            + config->compareOutputMode);

    HWREG16(timer + config->compareRegister + OFS_TA0R) = config->compareValue;
}


void Timer_A_clearTimer(uint32_t timer)
{
    BITBAND_PERI(TIMER_A_CMSIS(timer)->rCTL.r , TACLR_OFS) = 1;
}


uint_fast8_t Timer_A_getSynchronizedCaptureCompareInput(uint32_t timer,
        uint_fast16_t captureCompareRegister, uint_fast16_t synchronizedSetting)
{
    ASSERT(
            (TIMER_A_CAPTURECOMPARE_REGISTER_0 == captureCompareRegister)
            || (TIMER_A_CAPTURECOMPARE_REGISTER_1
                    == captureCompareRegister)
            || (TIMER_A_CAPTURECOMPARE_REGISTER_2
                    == captureCompareRegister)
            || (TIMER_A_CAPTURECOMPARE_REGISTER_3
                    == captureCompareRegister)
            || (TIMER_A_CAPTURECOMPARE_REGISTER_4
                    == captureCompareRegister)
            || (TIMER_A_CAPTURECOMPARE_REGISTER_5
                    == captureCompareRegister)
            || (TIMER_A_CAPTURECOMPARE_REGISTER_6
                    == captureCompareRegister));

    ASSERT(
            (TIMER_A_READ_CAPTURE_COMPARE_INPUT == synchronizedSetting)
            || (TIMER_A_READ_SYNCHRONIZED_CAPTURECOMPAREINPUT
                    == synchronizedSetting));

    if (HWREG16(timer + captureCompareRegister) & synchronizedSetting)
        return TIMER_A_CAPTURECOMPARE_INPUT_HIGH;
    else
        return TIMER_A_CAPTURECOMPARE_INPUT_LOW;
}


uint_fast8_t Timer_A_getOutputForOutputModeOutBitValue(uint32_t timer,
        uint_fast16_t captureCompareRegister)
{
    ASSERT(
            (TIMER_A_CAPTURECOMPARE_REGISTER_0 == captureCompareRegister)
            || (TIMER_A_CAPTURECOMPARE_REGISTER_1
                    == captureCompareRegister)
            || (TIMER_A_CAPTURECOMPARE_REGISTER_2
                    == captureCompareRegister)
            || (TIMER_A_CAPTURECOMPARE_REGISTER_3
                    == captureCompareRegister)
            || (TIMER_A_CAPTURECOMPARE_REGISTER_4
                    == captureCompareRegister)
            || (TIMER_A_CAPTURECOMPARE_REGISTER_5
                    == captureCompareRegister)
            || (TIMER_A_CAPTURECOMPARE_REGISTER_6
                    == captureCompareRegister));

    if (HWREGBIT16(timer + captureCompareRegister, OUT_OFS))
        return TIMER_A_OUTPUTMODE_OUTBITVALUE_HIGH;
    else
        return TIMER_A_OUTPUTMODE_OUTBITVALUE_LOW;
}


uint_fast16_t Timer_A_getCaptureCompareCount(uint32_t timer,
        uint_fast16_t captureCompareRegister)
{
    ASSERT(
            (TIMER_A_CAPTURECOMPARE_REGISTER_0 == captureCompareRegister)
            || (TIMER_A_CAPTURECOMPARE_REGISTER_1
                    == captureCompareRegister)
            || (TIMER_A_CAPTURECOMPARE_REGISTER_2
                    == captureCompareRegister)
            || (TIMER_A_CAPTURECOMPARE_REGISTER_3
                    == captureCompareRegister)
            || (TIMER_A_CAPTURECOMPARE_REGISTER_4
                    == captureCompareRegister)
            || (TIMER_A_CAPTURECOMPARE_REGISTER_5
                    == captureCompareRegister)
            || (TIMER_A_CAPTURECOMPARE_REGISTER_6
                    == captureCompareRegister));

    return HWREG16(timer + OFS_TA0R + captureCompareRegister);
}


void Timer_A_setOutputForOutputModeOutBitValue(uint32_t timer,
        uint_fast16_t captureCompareRegister,
        uint_fast8_t outputModeOutBitValue)
{
    TIMER_A_setOutputForOutputModeOutBitValue(timer, captureCompareRegister,
            outputModeOutBitValue);
}


void Timer_A_generatePWM(uint32_t timer, const Timer_A_PWMConfig *config)
{
    ASSERT(
            (TIMER_A_CLOCKSOURCE_EXTERNAL_TXCLK == config->clockSource)
            || (TIMER_A_CLOCKSOURCE_ACLK == config->clockSource)
            || (TIMER_A_CLOCKSOURCE_SMCLK == config->clockSource)
            || (TIMER_A_CLOCKSOURCE_INVERTED_EXTERNAL_TXCLK
                    == config->clockSource));

    ASSERT(
            (TIMER_A_CAPTURECOMPARE_REGISTER_0 == config->compareRegister)
            || (TIMER_A_CAPTURECOMPARE_REGISTER_1
                    == config->compareRegister)
            || (TIMER_A_CAPTURECOMPARE_REGISTER_2
                    == config->compareRegister)
            || (TIMER_A_CAPTURECOMPARE_REGISTER_3
                    == config->compareRegister)
            || (TIMER_A_CAPTURECOMPARE_REGISTER_4
                    == config->compareRegister)
            || (TIMER_A_CAPTURECOMPARE_REGISTER_5
                    == config->compareRegister)
            || (TIMER_A_CAPTURECOMPARE_REGISTER_6
                    == config->compareRegister));

    ASSERT(
            (TIMER_A_OUTPUTMODE_OUTBITVALUE == config->compareOutputMode)
            || (TIMER_A_OUTPUTMODE_SET == config->compareOutputMode)
            || (TIMER_A_OUTPUTMODE_TOGGLE_RESET
                    == config->compareOutputMode)
            || (TIMER_A_OUTPUTMODE_SET_RESET
                    == config->compareOutputMode)
            || (TIMER_A_OUTPUTMODE_TOGGLE == config->compareOutputMode)
            || (TIMER_A_OUTPUTMODE_RESET == config->compareOutputMode)
            || (TIMER_A_OUTPUTMODE_TOGGLE_SET
                    == config->compareOutputMode)
            || (TIMER_A_OUTPUTMODE_RESET_SET
                    == config->compareOutputMode));

    privateTimer_AProcessClockSourceDivider(timer, config->clockSourceDivider);

    TIMER_A_CMSIS(timer)->rCTL.r &=
            ~(TIMER_A_CLOCKSOURCE_INVERTED_EXTERNAL_TXCLK + TIMER_A_UPDOWN_MODE
                    + TIMER_A_DO_CLEAR + TIMER_A_TAIE_INTERRUPT_ENABLE);

    TIMER_A_CMSIS(timer)->rCTL.r |= (config->clockSource + TIMER_A_UP_MODE
            + TIMER_A_DO_CLEAR);

    TIMER_A_CMSIS(timer)->rCCR0 = config->timerPeriod;

    HWREG16(timer + OFS_TA0CCTL0) &= ~(TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE
            + TIMER_A_OUTPUTMODE_RESET_SET);
    HWREG16(timer + config->compareRegister) |= config->compareOutputMode;

    HWREG16(timer + config->compareRegister + OFS_TA0R) = config->dutyCycle;
}


void Timer_A_stopTimer(uint32_t timer)
{
    TIMER_A_CMSIS(timer)->rCTL.r &= ~MC_3;
}


void Timer_A_setCompareValue(uint32_t timer, uint_fast16_t compareRegister,
        uint_fast16_t compareValue)
{
    ASSERT(
            (TIMER_A_CAPTURECOMPARE_REGISTER_0 == compareRegister)
            || (TIMER_A_CAPTURECOMPARE_REGISTER_1 == compareRegister)
            || (TIMER_A_CAPTURECOMPARE_REGISTER_2 == compareRegister)
            || (TIMER_A_CAPTURECOMPARE_REGISTER_3 == compareRegister)
            || (TIMER_A_CAPTURECOMPARE_REGISTER_4 == compareRegister)
            || (TIMER_A_CAPTURECOMPARE_REGISTER_5 == compareRegister)
            || (TIMER_A_CAPTURECOMPARE_REGISTER_6 == compareRegister));

    HWREG16(timer + compareRegister + OFS_TA0R) = compareValue;
}


void Timer_A_clearInterruptFlag(uint32_t timer)
{
    BITBAND_PERI(TIMER_A_CMSIS(timer)->rCTL.r,TAIFG_OFS) = 0;
}


void Timer_A_clearCaptureCompareInterrupt(uint32_t timer,
        uint_fast16_t captureCompareRegister)
{
    ASSERT(
            (TIMER_A_CAPTURECOMPARE_REGISTER_0 == captureCompareRegister)
            || (TIMER_A_CAPTURECOMPARE_REGISTER_1
                    == captureCompareRegister)
            || (TIMER_A_CAPTURECOMPARE_REGISTER_2
                    == captureCompareRegister)
            || (TIMER_A_CAPTURECOMPARE_REGISTER_3
                    == captureCompareRegister)
            || (TIMER_A_CAPTURECOMPARE_REGISTER_4
                    == captureCompareRegister)
            || (TIMER_A_CAPTURECOMPARE_REGISTER_5
                    == captureCompareRegister)
            || (TIMER_A_CAPTURECOMPARE_REGISTER_6
                    == captureCompareRegister));

    HWREGBIT16(timer + captureCompareRegister, CCIFG_OFS) = 0;
}


void Timer_A_enableInterrupt(uint32_t timer)
{
    BITBAND_PERI(TIMER_A_CMSIS(timer)->rCTL.r,TAIE_OFS) = 1;
}


void Timer_A_disableInterrupt(uint32_t timer)
{
    BITBAND_PERI(TIMER_A_CMSIS(timer)->rCTL.r,TAIE_OFS) = 0;
}


uint32_t Timer_A_getInterruptStatus(uint32_t timer)
{
    return TIMER_A_CMSIS(timer)->rCTL.b.bIFG;
}


uint32_t Timer_A_getEnabledInterruptStatus(uint32_t timer)
{
    if (TIMER_A_CMSIS(timer)->rCTL.r & TAIE)
    {
        return Timer_A_getInterruptStatus(timer);
    } else
    {
        return 0;
    }

}


void Timer_A_enableCaptureCompareInterrupt(uint32_t timer,
        uint_fast16_t captureCompareRegister)
{
    ASSERT(
            (TIMER_A_CAPTURECOMPARE_REGISTER_0 == captureCompareRegister)
            || (TIMER_A_CAPTURECOMPARE_REGISTER_1
                    == captureCompareRegister)
            || (TIMER_A_CAPTURECOMPARE_REGISTER_2
                    == captureCompareRegister)
            || (TIMER_A_CAPTURECOMPARE_REGISTER_3
                    == captureCompareRegister)
            || (TIMER_A_CAPTURECOMPARE_REGISTER_4
                    == captureCompareRegister)
            || (TIMER_A_CAPTURECOMPARE_REGISTER_5
                    == captureCompareRegister)
            || (TIMER_A_CAPTURECOMPARE_REGISTER_6
                    == captureCompareRegister));

    HWREGBIT16(timer + captureCompareRegister, CCIE_OFS) = 1;
}


void Timer_A_disableCaptureCompareInterrupt(uint32_t timer,
        uint_fast16_t captureCompareRegister)
{
    ASSERT(
            (TIMER_A_CAPTURECOMPARE_REGISTER_0 == captureCompareRegister)
            || (TIMER_A_CAPTURECOMPARE_REGISTER_1
                    == captureCompareRegister)
            || (TIMER_A_CAPTURECOMPARE_REGISTER_2
                    == captureCompareRegister)
            || (TIMER_A_CAPTURECOMPARE_REGISTER_3
                    == captureCompareRegister)
            || (TIMER_A_CAPTURECOMPARE_REGISTER_4
                    == captureCompareRegister)
            || (TIMER_A_CAPTURECOMPARE_REGISTER_5
                    == captureCompareRegister)
            || (TIMER_A_CAPTURECOMPARE_REGISTER_6
                    == captureCompareRegister));
                    
    HWREGBIT16(timer + captureCompareRegister, CCIE_OFS) = 0;
}


uint32_t Timer_A_getCaptureCompareInterruptStatus(uint32_t timer,
        uint_fast16_t captureCompareRegister, uint_fast16_t mask)
{
    return HWREG16(timer + captureCompareRegister) & mask;
}


uint32_t Timer_A_getCaptureCompareEnabledInterruptStatus(uint32_t timer,
        uint_fast16_t captureCompareRegister)
{
    if (HWREGBIT16(timer + captureCompareRegister, CCIE_OFS))
        return Timer_A_getCaptureCompareInterruptStatus(timer,
                captureCompareRegister,
                TIMER_A_CAPTURE_OVERFLOW |
                TIMER_A_CAPTURECOMPARE_INTERRUPT_FLAG);
    else
        return 0;
}


void Timer32_initModule(uint32_t timer, uint32_t preScaler, uint32_t resolution,
        uint32_t mode)
{
    /* Setting up one shot or continuous mode */
    if (mode == TIMER32_PERIODIC_MODE)
        HWREGBIT8(timer + OFS_TIMER32_CONTROL1, TIMER32_CONTROL1_MODE_OFS) = 1;
    else if (mode == TIMER32_FREE_RUN_MODE)
        HWREGBIT8(timer + OFS_TIMER32_CONTROL1, TIMER32_CONTROL1_MODE_OFS) = 0;
    else
        ASSERT(false);

    /* Setting the resolution of the timer */
    if (resolution == TIMER32_1_MODULE6BIT)
        HWREGBIT8(timer + OFS_TIMER32_CONTROL1, TIMER32_CONTROL1_SIZE_OFS) = 0;
    else if (resolution == TIMER32_32BIT)
        HWREGBIT8(timer + OFS_TIMER32_CONTROL1, TIMER32_CONTROL1_SIZE_OFS) = 1;
    else
        ASSERT(false);

    /* Setting the PreScaler */
    ASSERT(
            resolution == TIMER32_PRESCALER_1
            || resolution == TIMER32_PRESCALER_16
            || resolution == TIMER32_PRESCALER_256);

    HWREG32(timer + OFS_TIMER32_CONTROL1) =
            (HWREG32(timer + OFS_TIMER32_CONTROL1)
                    & ~TIMER32_CONTROL1_PRESCALE_M) | preScaler;

}


void Timer32_setCount(uint32_t timer, uint32_t count)
{
    if (!HWREGBIT32(timer + OFS_TIMER32_CONTROL1,
            TIMER32_CONTROL1_SIZE_OFS) && (count > UINT16_MAX))
        HWREG32(timer + OFS_TIMER32_LOAD1) = UINT16_MAX;
    else
        HWREG32(timer + OFS_TIMER32_LOAD1) = count;
}


void Timer32_setCountInBackground(uint32_t timer, uint32_t count)
{
    if (!HWREGBIT32(timer + OFS_TIMER32_CONTROL1,
            TIMER32_CONTROL1_SIZE_OFS) && (count > UINT16_MAX))
        HWREG32(timer + OFS_TIMER32_BGLOAD1) = UINT16_MAX;
    else
        HWREG32(timer + OFS_TIMER32_BGLOAD1) = count;
}


uint32_t Timer32_getValue(uint32_t timer)
{
    return HWREG32(timer + OFS_TIMER32_VALUE1);
}


void Timer32_startTimer(uint32_t timer, bool oneShot)
{
    ASSERT(timer == TIMER32_0_MODULE || timer == TIMER32_1_MODULE);

    if (oneShot)
        HWREGBIT8(timer + OFS_TIMER32_CONTROL1, TIMER32_CONTROL1_ONESHOT_OFS) =
                1;
    else
        HWREGBIT8(timer + OFS_TIMER32_CONTROL1, TIMER32_CONTROL1_ONESHOT_OFS) =
                0;

    HWREG32(timer + OFS_TIMER32_CONTROL1) |= TIMER32_CONTROL1_ENABLE;
}


void Timer32_haltTimer(uint32_t timer)
{
    ASSERT(timer == TIMER32_0_MODULE || timer == TIMER32_1_MODULE);

    HWREG32(timer + OFS_TIMER32_CONTROL1) &= ~TIMER32_CONTROL1_ENABLE;
}


void Timer32_enableInterrupt(uint32_t timer)
{
    HWREG32(timer + OFS_TIMER32_CONTROL1) |= TIMER32_CONTROL1_IE;
}


void Timer32_disableInterrupt(uint32_t timer)
{
    HWREG32(timer + OFS_TIMER32_CONTROL1) &= ~TIMER32_CONTROL1_IE;
}


void Timer32_clearInterruptFlag(uint32_t timer)
{
    HWREG32(timer + OFS_TIMER32_INTCLR1) |= 0x01;
}


uint32_t Timer32_getInterruptStatus(uint32_t timer)
{
    return HWREG32(timer + OFS_TIMER32_MIS1);
}


bool UART_initModule(uint32_t moduleInstance, const eUSCI_UART_Config *config)
{
    bool retVal = true;

    ASSERT(
            (EUSCI_A_UART_MODE == config->uartMode)
            || (EUSCI_A_UART_IDLE_LINE_MULTI_PROCESSOR_MODE
                    == config->uartMode)
            || (EUSCI_A_UART_ADDRESS_BIT_MULTI_PROCESSOR_MODE
                    == config->uartMode)
            || (EUSCI_A_UART_AUTOMATIC_BAUDRATE_DETECTION_MODE
                    == config->uartMode));

    ASSERT(
            (EUSCI_A_UART_CLOCKSOURCE_ACLK == config->selectClockSource)
            || (EUSCI_A_UART_CLOCKSOURCE_SMCLK
                    == config->selectClockSource));

    ASSERT(
            (EUSCI_A_UART_MSB_FIRST == config->msborLsbFirst)
            || (EUSCI_A_UART_LSB_FIRST == config->msborLsbFirst));

    ASSERT(
            (EUSCI_A_UART_ONE_STOP_BIT == config->numberofStopBits)
            || (EUSCI_A_UART_TWO_STOP_BITS == config->numberofStopBits));

    ASSERT(
            (EUSCI_A_UART_NO_PARITY == config->parity)
            || (EUSCI_A_UART_ODD_PARITY == config->parity)
            || (EUSCI_A_UART_EVEN_PARITY == config->parity));

    /* Disable the USCI Module */
    BITBAND_PERI(EUSCI_A_CMSIS(moduleInstance)->rCTLW0.r, UCSWRST_OFS) = 1;

    /* Clock source select */
    EUSCI_A_CMSIS(moduleInstance)->rCTLW0.r =
            (EUSCI_A_CMSIS(moduleInstance)->rCTLW0.r & ~UCSSEL_3)
                    | config->selectClockSource;

    /* MSB, LSB select */
    if (config->msborLsbFirst)
        BITBAND_PERI(EUSCI_A_CMSIS(moduleInstance)->rCTLW0.r, UCMSB_OFS) = 1;
    else
        BITBAND_PERI(EUSCI_A_CMSIS(moduleInstance)->rCTLW0.r, UCMSB_OFS) = 0;

    /* UCSPB = 0(1 stop bit) OR 1(2 stop bits) */
    if (config->numberofStopBits)
        BITBAND_PERI(EUSCI_A_CMSIS(moduleInstance)->rCTLW0.r, UCSPB_OFS) = 1;
    else
        BITBAND_PERI(EUSCI_A_CMSIS(moduleInstance)->rCTLW0.r, UCSPB_OFS) = 0;

    /* Parity */
    switch (config->parity)
    {
    case EUSCI_A_UART_NO_PARITY:
        BITBAND_PERI(EUSCI_A_CMSIS(moduleInstance)->rCTLW0.r, UCPEN_OFS) = 0;
        break;
    case EUSCI_A_UART_ODD_PARITY:
        BITBAND_PERI(EUSCI_A_CMSIS(moduleInstance)->rCTLW0.r, UCPEN_OFS) = 1;
        BITBAND_PERI(EUSCI_A_CMSIS(moduleInstance)->rCTLW0.r, UCPAR_OFS) = 0;
        break;
    case EUSCI_A_UART_EVEN_PARITY:
        BITBAND_PERI(EUSCI_A_CMSIS(moduleInstance)->rCTLW0.r, UCPEN_OFS) = 1;
        BITBAND_PERI(EUSCI_A_CMSIS(moduleInstance)->rCTLW0.r, UCPAR_OFS) = 1;
        break;
    }

    /* BaudRate Control Register */
    EUSCI_A_CMSIS(moduleInstance)->rBRW = config->clockPrescalar;
    EUSCI_A_CMSIS(moduleInstance)->rMCTLW.r = ((config->secondModReg << 8)
            + (config->firstModReg << 4) + config->overSampling);

    /* Asynchronous mode & 8 bit character select & clear mode */
    EUSCI_A_CMSIS(moduleInstance)->rCTLW0.r =
            (EUSCI_A_CMSIS(moduleInstance)->rCTLW0.r
                    & ~(UCSYNC | UC7BIT | UCMODE_3 | UCRXEIE | UCBRKIE | UCDORM
                            | UCTXADDR | UCTXBRK)) | config->uartMode;

    return retVal;
}


void UART_transmitData(uint32_t moduleInstance, uint_fast8_t transmitData)
{
    /* If interrupts are not used, poll for flags */
    if (!BITBAND_PERI(EUSCI_A_CMSIS(moduleInstance)->rIE.r, UCTXIE_OFS))
        while (!BITBAND_PERI(EUSCI_A_CMSIS(moduleInstance)->rIFG.r, UCTXIFG_OFS))
            ;

    EUSCI_A_CMSIS(moduleInstance)->rTXBUF.r = transmitData;
}


uint8_t UART_receiveData(uint32_t moduleInstance)
{
    /* If interrupts are not used, poll for flags */
    if (!BITBAND_PERI(EUSCI_A_CMSIS(moduleInstance)->rIE.r, UCRXIE_OFS))
        while (!BITBAND_PERI(EUSCI_A_CMSIS(moduleInstance)->rIFG.r, UCRXIFG_OFS))
            ;

    return EUSCI_A_CMSIS(moduleInstance)->rRXBUF.r;
}


void UART_enableModule(uint32_t moduleInstance)
{
    /* Reset the UCSWRST bit to enable the USCI Module */
    BITBAND_PERI(EUSCI_A_CMSIS(moduleInstance)->rCTLW0.r, UCSWRST_OFS) = 0;
}


void UART_disableModule(uint32_t moduleInstance)
{
    /* Set the UCSWRST bit to disable the USCI Module */
    BITBAND_PERI(EUSCI_A_CMSIS(moduleInstance)->rCTLW0.r, UCSWRST_OFS) = 1;
}


uint_fast8_t UART_queryStatusFlags(uint32_t moduleInstance, uint_fast8_t mask)
{
    ASSERT(
            0x00 != mask
            && (EUSCI_A_UART_LISTEN_ENABLE + EUSCI_A_UART_FRAMING_ERROR
                    + EUSCI_A_UART_OVERRUN_ERROR
                    + EUSCI_A_UART_PARITY_ERROR
                    + EUSCI_A_UART_BREAK_DETECT
                    + EUSCI_A_UART_RECEIVE_ERROR
                    + EUSCI_A_UART_ADDRESS_RECEIVED
                    + EUSCI_A_UART_IDLELINE + EUSCI_A_UART_BUSY));

    return EUSCI_A_CMSIS(moduleInstance)->rSTATW.r & mask;
}


void UART_setDormant(uint32_t moduleInstance)
{
    BITBAND_PERI(EUSCI_A_CMSIS(moduleInstance)->rCTLW0.r, UCDORM_OFS) = 1;
}


void UART_resetDormant(uint32_t moduleInstance)
{
    BITBAND_PERI(EUSCI_A_CMSIS(moduleInstance)->rCTLW0.r, UCDORM_OFS) = 0;
}


void UART_transmitAddress(uint32_t moduleInstance, uint_fast8_t transmitAddress)
{
    /* Set UCTXADDR bit */
    BITBAND_PERI(EUSCI_A_CMSIS(moduleInstance)->rCTLW0.r, UCTXADDR_OFS) = 1;

    /* Place next byte to be sent into the transmit buffer */
    EUSCI_A_CMSIS(moduleInstance)->rTXBUF.r = transmitAddress;
}


void UART_transmitBreak(uint32_t moduleInstance)
{
    /* Set UCTXADDR bit */
    BITBAND_PERI(EUSCI_A_CMSIS(moduleInstance)->rCTLW0.r, UCTXBRK_OFS) = 1;

    /* If current mode is automatic baud-rate detection */
    if (EUSCI_A_UART_AUTOMATIC_BAUDRATE_DETECTION_MODE
            == (EUSCI_A_CMSIS(moduleInstance)->rCTLW0.r
                    & EUSCI_A_UART_AUTOMATIC_BAUDRATE_DETECTION_MODE))
        EUSCI_A_CMSIS(moduleInstance)->rTXBUF.r =
        EUSCI_A_UART_AUTOMATICBAUDRATE_SYNC;
    else
        EUSCI_A_CMSIS(moduleInstance)->rTXBUF.r = DEFAULT_SYNC;

    /* If interrupts are not used, poll for flags */
    if (!BITBAND_PERI(EUSCI_A_CMSIS(moduleInstance)->rIE.r, UCTXIE_OFS))
        while (!BITBAND_PERI(EUSCI_A_CMSIS(moduleInstance)->rIFG.r, UCTXIFG_OFS))
            ;
}


uint32_t UART_getReceiveBufferAddressForDMA(uint32_t moduleInstance)
{
    return moduleInstance + OFS_UCA0RXBUF;
}


uint32_t UART_getTransmitBufferAddressForDMA(uint32_t moduleInstance)
{
    return moduleInstance + OFS_UCA0TXBUF;
}


void UART_selectDeglitchTime(uint32_t moduleInstance, uint32_t deglitchTime)
{
    ASSERT(
            (EUSCI_A_UART_DEGLITCH_TIME_2ns == deglitchTime)
            || (EUSCI_A_UART_DEGLITCH_TIME_50ns == deglitchTime)
            || (EUSCI_A_UART_DEGLITCH_TIME_100ns == deglitchTime)
            || (EUSCI_A_UART_DEGLITCH_TIME_200ns == deglitchTime));

    EUSCI_A_CMSIS(moduleInstance)->rCTLW1.r =
            (EUSCI_A_CMSIS(moduleInstance)->rCTLW1.r & ~(UCGLIT_M))
                    | deglitchTime;

}


void UART_enableInterrupt(uint32_t moduleInstance, uint_fast8_t mask)
{
    uint_fast8_t locMask;

    ASSERT(
            !(mask
                    & ~(EUSCI_A_UART_RECEIVE_INTERRUPT
                            | EUSCI_A_UART_TRANSMIT_INTERRUPT
                            | EUSCI_A_UART_RECEIVE_ERRONEOUSCHAR_INTERRUPT
                            | EUSCI_A_UART_BREAKCHAR_INTERRUPT
                            | EUSCI_A_UART_STARTBIT_INTERRUPT
                            | EUSCI_A_UART_TRANSMIT_COMPLETE_INTERRUPT)));

    locMask = (mask
            & (EUSCI_A_UART_RECEIVE_INTERRUPT | EUSCI_A_UART_TRANSMIT_INTERRUPT
                    | EUSCI_A_UART_STARTBIT_INTERRUPT
                    | EUSCI_A_UART_TRANSMIT_COMPLETE_INTERRUPT));

    EUSCI_A_CMSIS(moduleInstance)->rIE.r |= locMask;

    locMask = (mask
            & (EUSCI_A_UART_RECEIVE_ERRONEOUSCHAR_INTERRUPT
                    | EUSCI_A_UART_BREAKCHAR_INTERRUPT));
    EUSCI_A_CMSIS(moduleInstance)->rCTLW0.r |= locMask;
}


void UART_disableInterrupt(uint32_t moduleInstance, uint_fast8_t mask)
{
    uint_fast8_t locMask;

    ASSERT(
            !(mask
                    & ~(EUSCI_A_UART_RECEIVE_INTERRUPT
                            | EUSCI_A_UART_TRANSMIT_INTERRUPT
                            | EUSCI_A_UART_RECEIVE_ERRONEOUSCHAR_INTERRUPT
                            | EUSCI_A_UART_BREAKCHAR_INTERRUPT
                            | EUSCI_A_UART_STARTBIT_INTERRUPT
                            | EUSCI_A_UART_TRANSMIT_COMPLETE_INTERRUPT)));

    locMask = (mask
            & (EUSCI_A_UART_RECEIVE_INTERRUPT | EUSCI_A_UART_TRANSMIT_INTERRUPT
                    | EUSCI_A_UART_STARTBIT_INTERRUPT
                    | EUSCI_A_UART_TRANSMIT_COMPLETE_INTERRUPT));
    EUSCI_A_CMSIS(moduleInstance)->rIE.r &= ~locMask;

    locMask = (mask
            & (EUSCI_A_UART_RECEIVE_ERRONEOUSCHAR_INTERRUPT
                    | EUSCI_A_UART_BREAKCHAR_INTERRUPT));
    EUSCI_A_CMSIS(moduleInstance)->rCTLW0.r &= ~locMask;
}


uint_fast8_t UART_getInterruptStatus(uint32_t moduleInstance, uint8_t mask)
{
    ASSERT(
            !(mask
                    & ~(EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG
                            | EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG
                            | EUSCI_A_UART_STARTBIT_INTERRUPT_FLAG
                            | EUSCI_A_UART_TRANSMIT_COMPLETE_INTERRUPT_FLAG)));

    return EUSCI_A_CMSIS(moduleInstance)->rIFG.r & mask;
}


uint_fast8_t UART_getEnabledInterruptStatus(uint32_t moduleInstance)
{
    uint_fast8_t intStatus = UART_getInterruptStatus(moduleInstance,
    EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG | EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG);
    uint_fast8_t intEnabled = EUSCI_A_CMSIS(moduleInstance)->rIE.r;

    if (!(intEnabled & EUSCI_A_UART_RECEIVE_INTERRUPT))
    {
        intStatus &= ~EUSCI_A_UART_RECEIVE_INTERRUPT;
    }

    if (!(intEnabled & EUSCI_A_UART_TRANSMIT_INTERRUPT))
    {
        intStatus &= ~EUSCI_A_UART_TRANSMIT_INTERRUPT;
    }

    intEnabled = EUSCI_A_CMSIS(moduleInstance)->rCTLW0.r;

    if (!(intEnabled & EUSCI_A_UART_RECEIVE_ERRONEOUSCHAR_INTERRUPT))
    {
        intStatus &= ~EUSCI_A_UART_RECEIVE_ERRONEOUSCHAR_INTERRUPT;
    }

    if (!(intEnabled & EUSCI_A_UART_BREAKCHAR_INTERRUPT))
    {
        intStatus &= ~EUSCI_A_UART_BREAKCHAR_INTERRUPT;
    }

    return intStatus;
}


void UART_clearInterruptFlag(uint32_t moduleInstance, uint_fast8_t mask)
{
    ASSERT(
            !(mask
                    & ~(EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG
                            | EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG
                            | EUSCI_A_UART_STARTBIT_INTERRUPT_FLAG
                            | EUSCI_A_UART_TRANSMIT_COMPLETE_INTERRUPT_FLAG)));

    //Clear the UART interrupt source.
    EUSCI_A_CMSIS(moduleInstance)->rIFG.r &= ~(mask);
}


void WDT_A_holdTimer(void)
{
    //Set Hold bit
    uint8_t newWDTStatus = (WDT_A->rCTL.r | WDTHOLD);

    WDT_A->rCTL.r = WDTPW + newWDTStatus;
}


void WDT_A_startTimer(void)
{
    //Reset Hold bit
    uint8_t newWDTStatus = (WDT_A->rCTL.r & ~(WDTHOLD));

    WDT_A->rCTL.r = WDTPW + newWDTStatus;
}


void WDT_A_clearTimer(void)
{
    //Set Counter Clear bit
    uint8_t newWDTStatus = (WDT_A->rCTL.r | WDTCNTCL);

    WDT_A->rCTL.r = WDTPW + newWDTStatus;
}


void WDT_A_setPasswordViolationReset(uint_fast8_t resetType)
{
    SysCtl_setWDTPasswordViolationResetType(resetType);
}


void WDT_A_setTimeoutReset(uint_fast8_t resetType)
{
    SysCtl_setWDTTimeoutResetType(resetType);
}


void WDT_A_initWatchdogTimer(uint_fast8_t clockSelect,
        uint_fast8_t clockIterations)
{
    WDT_A->rCTL.r = WDTPW + WDTCNTCL + WDTHOLD +
            clockSelect + clockIterations;
}


void WDT_A_initIntervalTimer(uint_fast8_t clockSelect,
        uint_fast8_t clockIterations)
{

    WDT_A->rCTL.r = WDTPW + WDTCNTCL + WDTHOLD + WDTTMSEL
            + clockSelect + clockIterations;
}


