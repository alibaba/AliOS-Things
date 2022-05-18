/*
 *  Copyright (c) 2021, The OpenThread Authors.
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
 *   This file implements the OpenThread SRP client buffers and service pool APIs.
 */

#include "openthread-core-config.h"

#include <openthread/srp_client_buffers.h>

#include "common/as_core_type.hpp"
#include "common/locator_getters.hpp"

using namespace ot;

#if OPENTHREAD_CONFIG_SRP_CLIENT_BUFFERS_ENABLE

char *otSrpClientBuffersGetHostNameString(otInstance *aInstance, uint16_t *aSize)
{
    return AsCoreType(aInstance).Get<Utils::SrpClientBuffers>().GetHostNameString(*aSize);
}

otIp6Address *otSrpClientBuffersGetHostAddressesArray(otInstance *aInstance, uint8_t *aArrayLength)
{
    return AsCoreType(aInstance).Get<Utils::SrpClientBuffers>().GetHostAddressesArray(*aArrayLength);
}

otSrpClientBuffersServiceEntry *otSrpClientBuffersAllocateService(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<Utils::SrpClientBuffers>().AllocateService();
}

void otSrpClientBuffersFreeService(otInstance *aInstance, otSrpClientBuffersServiceEntry *aService)
{
    AsCoreType(aInstance).Get<Utils::SrpClientBuffers>().FreeService(AsCoreType(aService));
}

void otSrpClientBuffersFreeAllServices(otInstance *aInstance)
{
    AsCoreType(aInstance).Get<Utils::SrpClientBuffers>().FreeAllServices();
}

char *otSrpClientBuffersGetServiceEntryServiceNameString(otSrpClientBuffersServiceEntry *aEntry, uint16_t *aSize)
{
    return AsCoreType(aEntry).GetServiceNameString(*aSize);
}

char *otSrpClientBuffersGetServiceEntryInstanceNameString(otSrpClientBuffersServiceEntry *aEntry, uint16_t *aSize)
{
    return AsCoreType(aEntry).GetInstanceNameString(*aSize);
}

uint8_t *otSrpClientBuffersGetServiceEntryTxtBuffer(otSrpClientBuffersServiceEntry *aEntry, uint16_t *aSize)
{
    return AsCoreType(aEntry).GetTxtBuffer(*aSize);
}

const char **otSrpClientBuffersGetSubTypeLabelsArray(otSrpClientBuffersServiceEntry *aEntry, uint16_t *aArrayLength)
{
    return AsCoreType(aEntry).GetSubTypeLabelsArray(*aArrayLength);
}

#endif // OPENTHREAD_CONFIG_SRP_CLIENT_BUFFERS_ENABLE
