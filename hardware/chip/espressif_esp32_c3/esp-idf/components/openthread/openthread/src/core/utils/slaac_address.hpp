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
 *   This file includes definitions for Thread global IPv6 address configuration with SLAAC.
 */

#ifndef SLAAC_ADDRESS_HPP_
#define SLAAC_ADDRESS_HPP_

#include "openthread-core-config.h"

#if OPENTHREAD_CONFIG_IP6_SLAAC_ENABLE

#include "common/locator.hpp"
#include "common/non_copyable.hpp"
#include "common/notifier.hpp"
#include "net/netif.hpp"
#include "thread/network_data.hpp"

namespace ot {
namespace Utils {

/**
 * @addtogroup core-slaac-address
 *
 * @brief
 *   This module includes definitions for Thread global IPv6 address configuration with SLAAC.
 *
 * @{
 */

/**
 * This class implements the SLAAC utility for Thread protocol.
 *
 */
class Slaac : public InstanceLocator, private NonCopyable
{
    friend class ot::Notifier;

public:
    /**
     * This type represents the secret key used for generating semantically opaque IID (per RFC 7217).
     *
     */
    struct IidSecretKey
    {
        static constexpr uint16_t kSize = 32; ///< Secret key size for generating semantically opaque IID.

        uint8_t m8[kSize];
    };

    /**
     * This constructor initializes the SLAAC manager object.
     *
     * Note that SLAAC module starts enabled.
     *
     * @param[in]  aInstance  A reference to the OpenThread instance.
     *
     */
    explicit Slaac(Instance &aInstance);

    /**
     * This method enables the SLAAC module.
     *
     * When enabled, new SLAAC addresses are generated and added from on-mesh prefixes in network data.
     *
     */
    void Enable(void);

    /**
     * This method disables the SLAAC module.
     *
     * When disabled, any previously added SLAAC address by this module is removed.
     *
     */
    void Disable(void);

    /**
     * This method indicates whether SLAAC module is enabled or not.
     *
     * @retval TRUE    SLAAC module is enabled.
     * @retval FALSE   SLAAC module is disabled.
     *
     */
    bool IsEnabled(void) const { return mEnabled; }

    /**
     * This method sets a SLAAC prefix filter handler.
     *
     * The handler is invoked by SLAAC module when it is about to add a SLAAC address based on a prefix. The return
     * boolean value from handler determines whether the address is filtered or added (TRUE to filter the address,
     * FALSE to add address).
     *
     * The filter can be set to `nullptr` to disable filtering (i.e., allow SLAAC addresses for all prefixes).
     *
     */
    void SetFilter(otIp6SlaacPrefixFilter aFilter);

    /**
     * This method generates the IID of an IPv6 address.
     *
     * @param[inout]  aAddress             A reference to the address that will be filled with the IID generated.
     *                                     Note the prefix of the address must already be filled and will be used
     *                                     to generate the IID.
     * @param[in]     aNetworkId           A pointer to a byte array of Network_ID to generate IID.
     * @param[in]     aNetworkIdLength     The size of array @p aNetworkId.
     * @param[inout]  aDadCounter          A pointer to the DAD_Counter that is employed to resolve Duplicate
     *                                     Address Detection connflicts.
     *
     * @retval kErrorNone    If successfully generated the IID.
     * @retval kErrorFailed  If no valid IID was generated.
     *
     */
    Error GenerateIid(Ip6::Netif::UnicastAddress &aAddress,
                      uint8_t *                   aNetworkId       = nullptr,
                      uint8_t                     aNetworkIdLength = 0,
                      uint8_t *                   aDadCounter      = nullptr) const;

private:
    static constexpr uint16_t kMaxIidCreationAttempts = 256; // Maximum number of attempts when generating IID.

    typedef uint8_t UpdateMode;

    // Values for `UpdateMode` input parameter in `Update()`.

    static constexpr UpdateMode kModeNone = 0x0;    // No action.
    static constexpr UpdateMode kModeAdd  = 1 << 0; // Add new SLAAC addresses for new prefixes in network data.

    // Remove SLAAC addresses.
    // - When SLAAC is enabled, remove addresses with no matching prefix in network data,
    // - When SLAAC is disabled, remove all previously added addresses.
    static constexpr UpdateMode kModeRemove = 1 << 1;

    bool        ShouldFilter(const Ip6::Prefix &aPrefix) const;
    void        Update(UpdateMode aMode);
    void        GetIidSecretKey(IidSecretKey &aKey) const;
    void        HandleNotifierEvents(Events aEvents);
    static bool DoesConfigMatchNetifAddr(const NetworkData::OnMeshPrefixConfig &aConfig,
                                         const Ip6::Netif::UnicastAddress &     aAddr);

    bool                       mEnabled;
    otIp6SlaacPrefixFilter     mFilter;
    Ip6::Netif::UnicastAddress mAddresses[OPENTHREAD_CONFIG_IP6_SLAAC_NUM_ADDRESSES];
};

/**
 * @}
 */

} // namespace Utils
} // namespace ot

#endif // OPENTHREAD_CONFIG_IP6_SLAAC_ENABLE

#endif // SLAAC_ADDRESS_HPP_
