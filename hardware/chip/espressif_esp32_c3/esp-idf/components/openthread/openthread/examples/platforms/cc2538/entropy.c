/*
 *  Copyright (c) 2019, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file implements an entropy source based on ADC.
 *
 */

#include <openthread/platform/entropy.h>

#include <openthread/platform/radio.h>

#include "platform-cc2538.h"
#include "utils/code_utils.h"

static void generateRandom(uint8_t *aOutput, uint16_t aOutputLength)
{
    uint32_t frmctrl0;

    HWREG(SOC_ADC_ADCCON1) &= ~(SOC_ADC_ADCCON1_RCTRL1 | SOC_ADC_ADCCON1_RCTRL0);
    HWREG(SYS_CTRL_RCGCRFC) = SYS_CTRL_RCGCRFC_RFC0;

    while (HWREG(SYS_CTRL_RCGCRFC) != SYS_CTRL_RCGCRFC_RFC0)
        ;

    frmctrl0                    = HWREG(RFCORE_XREG_FRMCTRL0);
    HWREG(RFCORE_XREG_FRMCTRL0) = RFCORE_XREG_FRMCTRL0_INFINITY_RX;
    HWREG(RFCORE_SFR_RFST)      = RFCORE_SFR_RFST_INSTR_RXON;

    while (!HWREG(RFCORE_XREG_RSSISTAT) & RFCORE_XREG_RSSISTAT_RSSI_VALID)
        ;

    for (uint16_t index = 0; index < aOutputLength; index++)
    {
        aOutput[index] = 0;

        for (uint8_t offset = 0; offset < 8 * sizeof(uint8_t); offset++)
        {
            aOutput[index] <<= 1;
            aOutput[index] |= (HWREG(RFCORE_XREG_RFRND) & RFCORE_XREG_RFRND_IRND);
        }
    }

    HWREG(RFCORE_SFR_RFST)      = RFCORE_SFR_RFST_INSTR_RFOFF;
    HWREG(RFCORE_XREG_FRMCTRL0) = frmctrl0;
}

void cc2538RandomInit(void)
{
    uint16_t seed = 0;

    while (seed == 0x0000 || seed == 0x8003)
    {
        generateRandom((uint8_t *)&seed, sizeof(seed));
    }

    HWREG(SOC_ADC_RNDL) = (seed >> 8) & 0xff;
    HWREG(SOC_ADC_RNDL) = seed & 0xff;
}

otError otPlatEntropyGet(uint8_t *aOutput, uint16_t aOutputLength)
{
    otError error   = OT_ERROR_NONE;
    uint8_t channel = 0;

    otEXPECT_ACTION(aOutput, error = OT_ERROR_INVALID_ARGS);

    if (sInstance && otPlatRadioIsEnabled(sInstance))
    {
        channel = 11 + (HWREG(RFCORE_XREG_FREQCTRL) - 11) / 5;
        otPlatRadioSleep(sInstance);
        otPlatRadioDisable(sInstance);
    }

    generateRandom(aOutput, aOutputLength);

    if (channel)
    {
        cc2538RadioInit();
        otPlatRadioEnable(sInstance);
        otPlatRadioReceive(sInstance, channel);
    }

exit:
    return error;
}
