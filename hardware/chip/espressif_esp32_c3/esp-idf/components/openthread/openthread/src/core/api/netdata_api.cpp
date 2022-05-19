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
 *   This file implements the OpenThread Network Data API.
 */

#include "openthread-core-config.h"

#include <openthread/netdata.h>

#include "common/as_core_type.hpp"
#include "common/locator_getters.hpp"

using namespace ot;

otError otNetDataGet(otInstance *aInstance, bool aStable, uint8_t *aData, uint8_t *aDataLength)
{
    return AsCoreType(aInstance).Get<NetworkData::Leader>().CopyNetworkData(aStable, aData, *aDataLength);
}

otError otNetDataGetNextOnMeshPrefix(otInstance *           aInstance,
                                     otNetworkDataIterator *aIterator,
                                     otBorderRouterConfig * aConfig)
{
    Error error = kErrorNone;

    VerifyOrExit(aIterator && aConfig, error = kErrorInvalidArgs);

    error = AsCoreType(aInstance).Get<NetworkData::Leader>().GetNextOnMeshPrefix(*aIterator, AsCoreType(aConfig));

exit:
    return error;
}

otError otNetDataGetNextRoute(otInstance *aInstance, otNetworkDataIterator *aIterator, otExternalRouteConfig *aConfig)
{
    Error error = kErrorNone;

    VerifyOrExit(aIterator && aConfig, error = kErrorInvalidArgs);

    error = AsCoreType(aInstance).Get<NetworkData::Leader>().GetNextExternalRoute(*aIterator, AsCoreType(aConfig));

exit:
    return error;
}

otError otNetDataGetNextService(otInstance *aInstance, otNetworkDataIterator *aIterator, otServiceConfig *aConfig)
{
    Error error = kErrorNone;

    VerifyOrExit(aIterator && aConfig, error = kErrorInvalidArgs);

    error = AsCoreType(aInstance).Get<NetworkData::Leader>().GetNextService(*aIterator, AsCoreType(aConfig));

exit:
    return error;
}

uint8_t otNetDataGetVersion(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<Mle::MleRouter>().GetLeaderData().GetDataVersion();
}

uint8_t otNetDataGetStableVersion(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<Mle::MleRouter>().GetLeaderData().GetStableDataVersion();
}

otError otNetDataSteeringDataCheckJoiner(otInstance *aInstance, const otExtAddress *aEui64)
{
    return AsCoreType(aInstance).Get<NetworkData::Leader>().SteeringDataCheckJoiner(AsCoreType(aEui64));
}

otError otNetDataSteeringDataCheckJoinerWithDiscerner(otInstance *aInstance, const otJoinerDiscerner *aDiscerner)
{
    return AsCoreType(aInstance).Get<NetworkData::Leader>().SteeringDataCheckJoiner(AsCoreType(aDiscerner));
}
