/*
 *  Copyright (c) 2016-2017, The OpenThread Authors.
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
 *   This file implements the OpenThread Server API.
 */

#include "openthread-core-config.h"

#if OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE

#include <openthread/server.h>

#include "common/as_core_type.hpp"
#include "common/locator_getters.hpp"

using namespace ot;

otError otServerGetNetDataLocal(otInstance *aInstance, bool aStable, uint8_t *aData, uint8_t *aDataLength)
{
    return AsCoreType(aInstance).Get<NetworkData::Local>().CopyNetworkData(aStable, aData, *aDataLength);
}

otError otServerAddService(otInstance *aInstance, const otServiceConfig *aConfig)
{
    NetworkData::ServiceData serviceData;
    NetworkData::ServerData  serverData;

    serviceData.Init(&aConfig->mServiceData[0], aConfig->mServiceDataLength);
    serverData.Init(&aConfig->mServerConfig.mServerData[0], aConfig->mServerConfig.mServerDataLength);

    return AsCoreType(aInstance).Get<NetworkData::Local>().AddService(aConfig->mEnterpriseNumber, serviceData,
                                                                      aConfig->mServerConfig.mStable, serverData);
}

otError otServerRemoveService(otInstance *   aInstance,
                              uint32_t       aEnterpriseNumber,
                              const uint8_t *aServiceData,
                              uint8_t        aServiceDataLength)
{
    NetworkData::ServiceData serviceData;

    serviceData.Init(aServiceData, aServiceDataLength);

    return AsCoreType(aInstance).Get<NetworkData::Local>().RemoveService(aEnterpriseNumber, serviceData);
}

otError otServerGetNextService(otInstance *aInstance, otNetworkDataIterator *aIterator, otServiceConfig *aConfig)
{
    Error error = kErrorNone;

    VerifyOrExit(aIterator && aConfig, error = kErrorInvalidArgs);

    error = AsCoreType(aInstance).Get<NetworkData::Local>().GetNextService(*aIterator, AsCoreType(aConfig));

exit:
    return error;
}

otError otServerRegister(otInstance *aInstance)
{
    AsCoreType(aInstance).Get<NetworkData::Notifier>().HandleServerDataUpdated();

    return kErrorNone;
}

#endif // OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE
