/*
 *  Copyright (c) 2016, The OpenThread Authors.
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
 *   This file implements the OpenThread Link Raw API.
 */

#include "openthread-core-config.h"

#if OPENTHREAD_RADIO || OPENTHREAD_CONFIG_LINK_RAW_ENABLE

#include <string.h>
#include <openthread/diag.h>
#include <openthread/thread.h>
#include <openthread/platform/diag.h>
#include <openthread/platform/time.h>

#include "common/as_core_type.hpp"
#include "common/debug.hpp"
#include "common/locator_getters.hpp"
#include "common/random.hpp"
#include "mac/mac_frame.hpp"

using namespace ot;

otError otLinkRawSetReceiveDone(otInstance *aInstance, otLinkRawReceiveDone aCallback)
{
    return AsCoreType(aInstance).Get<Mac::LinkRaw>().SetReceiveDone(aCallback);
}

bool otLinkRawIsEnabled(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<Mac::LinkRaw>().IsEnabled();
}

otError otLinkRawSetShortAddress(otInstance *aInstance, uint16_t aShortAddress)
{
    return AsCoreType(aInstance).Get<Mac::LinkRaw>().SetShortAddress(aShortAddress);
}

bool otLinkRawGetPromiscuous(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<Radio>().GetPromiscuous();
}

otError otLinkRawSetPromiscuous(otInstance *aInstance, bool aEnable)
{
    Error     error    = kErrorNone;
    Instance &instance = AsCoreType(aInstance);

    VerifyOrExit(instance.Get<Mac::LinkRaw>().IsEnabled(), error = kErrorInvalidState);
    instance.Get<Radio>().SetPromiscuous(aEnable);

exit:
    return error;
}

otError otLinkRawSleep(otInstance *aInstance)
{
    Error     error    = kErrorNone;
    Instance &instance = AsCoreType(aInstance);

    VerifyOrExit(instance.Get<Mac::LinkRaw>().IsEnabled(), error = kErrorInvalidState);

    error = instance.Get<Radio>().Sleep();

exit:
    return error;
}

otError otLinkRawReceive(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<Mac::LinkRaw>().Receive();
}

otRadioFrame *otLinkRawGetTransmitBuffer(otInstance *aInstance)
{
    return &AsCoreType(aInstance).Get<Mac::LinkRaw>().GetTransmitFrame();
}

otError otLinkRawTransmit(otInstance *aInstance, otLinkRawTransmitDone aCallback)
{
    return AsCoreType(aInstance).Get<Mac::LinkRaw>().Transmit(aCallback);
}

int8_t otLinkRawGetRssi(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<Radio>().GetRssi();
}

otRadioCaps otLinkRawGetCaps(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<Mac::LinkRaw>().GetCaps();
}

otError otLinkRawEnergyScan(otInstance *            aInstance,
                            uint8_t                 aScanChannel,
                            uint16_t                aScanDuration,
                            otLinkRawEnergyScanDone aCallback)
{
    return AsCoreType(aInstance).Get<Mac::LinkRaw>().EnergyScan(aScanChannel, aScanDuration, aCallback);
}

otError otLinkRawSrcMatchEnable(otInstance *aInstance, bool aEnable)
{
    Error     error    = kErrorNone;
    Instance &instance = AsCoreType(aInstance);

    VerifyOrExit(instance.Get<Mac::LinkRaw>().IsEnabled(), error = kErrorInvalidState);

    instance.Get<Radio>().EnableSrcMatch(aEnable);

exit:
    return error;
}

otError otLinkRawSrcMatchAddShortEntry(otInstance *aInstance, uint16_t aShortAddress)
{
    Error     error    = kErrorNone;
    Instance &instance = AsCoreType(aInstance);

    VerifyOrExit(instance.Get<Mac::LinkRaw>().IsEnabled(), error = kErrorInvalidState);

    error = instance.Get<Radio>().AddSrcMatchShortEntry(aShortAddress);

exit:
    return error;
}

otError otLinkRawSrcMatchAddExtEntry(otInstance *aInstance, const otExtAddress *aExtAddress)
{
    Mac::ExtAddress address;
    Error           error    = kErrorNone;
    Instance &      instance = AsCoreType(aInstance);

    VerifyOrExit(instance.Get<Mac::LinkRaw>().IsEnabled(), error = kErrorInvalidState);

    address.Set(aExtAddress->m8, Mac::ExtAddress::kReverseByteOrder);
    error = instance.Get<Radio>().AddSrcMatchExtEntry(address);

exit:
    return error;
}

otError otLinkRawSrcMatchClearShortEntry(otInstance *aInstance, uint16_t aShortAddress)
{
    Error     error    = kErrorNone;
    Instance &instance = AsCoreType(aInstance);

    VerifyOrExit(instance.Get<Mac::LinkRaw>().IsEnabled(), error = kErrorInvalidState);
    error = instance.Get<Radio>().ClearSrcMatchShortEntry(aShortAddress);

exit:
    return error;
}

otError otLinkRawSrcMatchClearExtEntry(otInstance *aInstance, const otExtAddress *aExtAddress)
{
    Mac::ExtAddress address;
    Error           error    = kErrorNone;
    Instance &      instance = AsCoreType(aInstance);

    VerifyOrExit(instance.Get<Mac::LinkRaw>().IsEnabled(), error = kErrorInvalidState);

    address.Set(aExtAddress->m8, Mac::ExtAddress::kReverseByteOrder);
    error = instance.Get<Radio>().ClearSrcMatchExtEntry(address);

exit:
    return error;
}

otError otLinkRawSrcMatchClearShortEntries(otInstance *aInstance)
{
    Error     error    = kErrorNone;
    Instance &instance = AsCoreType(aInstance);

    VerifyOrExit(instance.Get<Mac::LinkRaw>().IsEnabled(), error = kErrorInvalidState);

    instance.Get<Radio>().ClearSrcMatchShortEntries();

exit:
    return error;
}

otError otLinkRawSrcMatchClearExtEntries(otInstance *aInstance)
{
    Error     error    = kErrorNone;
    Instance &instance = AsCoreType(aInstance);

    VerifyOrExit(instance.Get<Mac::LinkRaw>().IsEnabled(), error = kErrorInvalidState);

    instance.Get<Radio>().ClearSrcMatchExtEntries();

exit:
    return error;
}

otError otLinkRawSetMacKey(otInstance *    aInstance,
                           uint8_t         aKeyIdMode,
                           uint8_t         aKeyId,
                           const otMacKey *aPrevKey,
                           const otMacKey *aCurrKey,
                           const otMacKey *aNextKey)
{
    return AsCoreType(aInstance).Get<Mac::LinkRaw>().SetMacKey(aKeyIdMode, aKeyId, AsCoreType(aPrevKey),
                                                               AsCoreType(aCurrKey), AsCoreType(aNextKey));
}

otError otLinkRawSetMacFrameCounter(otInstance *aInstance, uint32_t aMacFrameCounter)
{
    return AsCoreType(aInstance).Get<Mac::LinkRaw>().SetMacFrameCounter(aMacFrameCounter);
}

uint64_t otLinkRawGetRadioTime(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
    return otPlatTimeGet();
}

#if OPENTHREAD_RADIO

otDeviceRole otThreadGetDeviceRole(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
    return OT_DEVICE_ROLE_DISABLED;
}

uint8_t otLinkGetChannel(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<Mac::LinkRaw>().GetChannel();
}

otError otLinkSetChannel(otInstance *aInstance, uint8_t aChannel)
{
    return AsCoreType(aInstance).Get<Mac::LinkRaw>().SetChannel(aChannel);
}

otPanId otLinkGetPanId(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<Mac::LinkRaw>().GetPanId();
}

otError otLinkSetPanId(otInstance *aInstance, uint16_t aPanId)
{
    return AsCoreType(aInstance).Get<Mac::LinkRaw>().SetPanId(aPanId);
}

const otExtAddress *otLinkGetExtendedAddress(otInstance *aInstance)
{
    return &AsCoreType(aInstance).Get<Mac::LinkRaw>().GetExtAddress();
}

otError otLinkSetExtendedAddress(otInstance *aInstance, const otExtAddress *aExtAddress)
{
    return AsCoreType(aInstance).Get<Mac::LinkRaw>().SetExtAddress(AsCoreType(aExtAddress));
}

uint16_t otLinkGetShortAddress(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<Mac::LinkRaw>().GetShortAddress();
}

void otLinkGetFactoryAssignedIeeeEui64(otInstance *aInstance, otExtAddress *aEui64)
{
    otPlatRadioGetIeeeEui64(aInstance, aEui64->m8);
}

#endif // OPENTHREAD_RADIO

#endif // OPENTHREAD_RADIO || OPENTHREAD_CONFIG_LINK_RAW_ENABLE
