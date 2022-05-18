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
 *   This file includes definitions for manipulating local Thread Network Data.
 */

#ifndef NETWORK_DATA_LOCAL_HPP_
#define NETWORK_DATA_LOCAL_HPP_

#include "openthread-core-config.h"

#if OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE || OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE

#include "common/non_copyable.hpp"
#include "thread/network_data.hpp"

namespace ot {

/**
 * @addtogroup core-netdata-local
 *
 * @brief
 *   This module includes definitions for manipulating local Thread Network Data.
 *
 * @{
 */

namespace NetworkData {

/**
 * This class implements the Thread Network Data contributed by the local device.
 *
 */
class Local : public MutableNetworkData, private NonCopyable
{
public:
    /**
     * This constructor initializes the local Network Data.
     *
     * @param[in]  aInstance     A reference to the OpenThread instance.
     *
     */
    explicit Local(Instance &aInstance)
        : MutableNetworkData(aInstance, mTlvBuffer, 0, sizeof(mTlvBuffer))
        , mOldRloc(Mac::kShortAddrInvalid)
    {
    }

#if OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE
    /**
     * This method adds a Border Router entry to the Thread Network Data.
     *
     * @param[in]  aConfig  A reference to the on mesh prefix configuration.
     *
     * @retval kErrorNone         Successfully added the Border Router entry.
     * @retval kErrorNoBufs       Insufficient space to add the Border Router entry.
     * @retval kErrorInvalidArgs  The prefix is mesh local prefix.
     *
     */
    Error AddOnMeshPrefix(const OnMeshPrefixConfig &aConfig);

    /**
     * This method removes a Border Router entry from the Thread Network Data.
     *
     * @param[in]  aPrefix        The Prefix to remove.
     *
     * @retval kErrorNone       Successfully removed the Border Router entry.
     * @retval kErrorNotFound   Could not find the Border Router entry.
     *
     */
    Error RemoveOnMeshPrefix(const Ip6::Prefix &aPrefix);

    /**
     * This method adds a Has Route entry to the Thread Network data.
     *
     * @param[in]  aConfig       A reference to the external route configuration.
     *
     * @retval kErrorNone         Successfully added the Has Route entry.
     * @retval kErrorInvalidArgs  One or more parameters in @p aConfig were invalid.
     * @retval kErrorNoBufs       Insufficient space to add the Has Route entry.
     *
     */
    Error AddHasRoutePrefix(const ExternalRouteConfig &aConfig);

    /**
     * This method removes a Border Router entry from the Thread Network Data.
     *
     * @param[in]  aPrefix        The Prefix to remove.
     *
     * @retval kErrorNone       Successfully removed the Border Router entry.
     * @retval kErrorNotFound   Could not find the Border Router entry.
     *
     */
    Error RemoveHasRoutePrefix(const Ip6::Prefix &aPrefix);
#endif // OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE

#if OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE
    /**
     * This method adds a Service entry to the Thread Network local data.
     *
     * @param[in]  aEnterpriseNumber  Enterprise Number (IANA-assigned) for Service TLV.
     * @param[in]  aServiceData       The Service Data.
     * @param[in]  aServerStable      The Stable flag value for Server TLV.
     * @param[in]  aServerData        The Server Data.
     *
     * @retval kErrorNone     Successfully added the Service entry.
     * @retval kErrorNoBufs   Insufficient space to add the Service entry.
     *
     */
    Error AddService(uint32_t           aEnterpriseNumber,
                     const ServiceData &aServiceData,
                     bool               aServerStable,
                     const ServerData & aServerData);

    /**
     * This method removes a Service entry from the Thread Network local data.
     *
     * @param[in]  aEnterpriseNumber   Enterprise Number of the service to be deleted.
     * @param[in]  aServiceData        The service data.
     *
     * @retval kErrorNone       Successfully removed the Service entry.
     * @retval kErrorNotFound   Could not find the Service entry.
     *
     */
    Error RemoveService(uint32_t aEnterpriseNumber, const ServiceData &aServiceData);
#endif // OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE

    /**
     * This method sends a Server Data Notification message to the Leader.
     *
     * @param[in]  aHandler  A function pointer that is called when the transaction ends.
     * @param[in]  aContext  A pointer to arbitrary context information.
     *
     * @retval kErrorNone          Successfully enqueued the notification message.
     * @retval kErrorNoBufs        Insufficient message buffers to generate the notification message.
     * @retval kErrorInvalidState  Device is a REED and is in the process of becoming a Router.
     * @retval kErrorNotFound      Server Data is already consistent with network data.
     *
     */
    Error UpdateInconsistentServerData(Coap::ResponseHandler aHandler, void *aContext);

private:
    void UpdateRloc(void);
#if OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE
    Error AddPrefix(const Ip6::Prefix &aPrefix, NetworkDataTlv::Type aSubTlvType, uint16_t aFlags, bool aStable);
    Error RemovePrefix(const Ip6::Prefix &aPrefix, NetworkDataTlv::Type aSubTlvType);
    void  UpdateRloc(PrefixTlv &aPrefixTlv);
    bool  IsOnMeshPrefixConsistent(void) const;
    bool  IsExternalRouteConsistent(void) const;
#endif

#if OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE
    void UpdateRloc(ServiceTlv &aService);
    bool IsServiceConsistent(void) const;
#endif

    uint8_t  mTlvBuffer[kMaxSize];
    uint16_t mOldRloc;
};

} // namespace NetworkData

/**
 * @}
 */

} // namespace ot

#endif // OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE || OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE

#endif // NETWORK_DATA_LOCAL_HPP_
