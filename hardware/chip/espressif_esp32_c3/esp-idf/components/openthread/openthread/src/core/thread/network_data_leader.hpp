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
 *   This file includes definitions for manipulating Thread Network Data managed by the Thread Leader.
 */

#ifndef NETWORK_DATA_LEADER_HPP_
#define NETWORK_DATA_LEADER_HPP_

#include "openthread-core-config.h"

#include <stdint.h>

#include "coap/coap.hpp"
#include "common/const_cast.hpp"
#include "common/timer.hpp"
#include "net/ip6_address.hpp"
#include "thread/mle_router.hpp"
#include "thread/network_data.hpp"

namespace ot {

namespace NetworkData {

/**
 * @addtogroup core-netdata-leader
 *
 * @brief
 *   This module includes definitions for manipulating Thread Network Data managed by the Thread Leader.
 *
 * @{
 *
 */

/**
 * This class implements the Thread Network Data maintained by the Leader.
 *
 */
class LeaderBase : public MutableNetworkData
{
public:
    /**
     * This constructor initializes the object.
     *
     * @param[in]  aInstance     A reference to the OpenThread instance.
     *
     */
    explicit LeaderBase(Instance &aInstance)
        : MutableNetworkData(aInstance, mTlvBuffer, 0, sizeof(mTlvBuffer))
    {
        Reset();
    }

    /**
     * This method reset the Thread Network Data.
     *
     */
    void Reset(void);

    /**
     * This method returns the Thread Network Data version.
     *
     * @returns The Thread Network Data version.
     *
     */
    uint8_t GetVersion(void) const { return mVersion; }

    /**
     * This method returns the Thread Network Data stable version.
     *
     * @returns The Thread Network Data stable version.
     *
     */
    uint8_t GetStableVersion(void) const { return mStableVersion; }

    /**
     * This method retrieves the 6LoWPAN Context information based on a given IPv6 address.
     *
     * @param[in]   aAddress  A reference to an IPv6 address.
     * @param[out]  aContext  A reference to 6LoWPAN Context information.
     *
     * @retval kErrorNone       Successfully retrieved 6LoWPAN Context information.
     * @retval kErrorNotFound   Could not find the 6LoWPAN Context information.
     *
     */
    Error GetContext(const Ip6::Address &aAddress, Lowpan::Context &aContext) const;

    /**
     * This method retrieves the 6LoWPAN Context information based on a given Context ID.
     *
     * @param[in]   aContextId  The Context ID value.
     * @param[out]  aContext    A reference to the 6LoWPAN Context information.
     *
     * @retval kErrorNone       Successfully retrieved 6LoWPAN Context information.
     * @retval kErrorNotFound   Could not find the 6LoWPAN Context information.
     *
     */
    Error GetContext(uint8_t aContextId, Lowpan::Context &aContext) const;

    /**
     * This method indicates whether or not the given IPv6 address is on-mesh.
     *
     * @param[in]  aAddress  A reference to an IPv6 address.
     *
     * @retval TRUE   If @p aAddress is on-link.
     * @retval FALSE  If @p aAddress if not on-link.
     *
     */
    bool IsOnMesh(const Ip6::Address &aAddress) const;

    /**
     * This method performs a route lookup using the Network Data.
     *
     * @param[in]   aSource             A reference to the IPv6 source address.
     * @param[in]   aDestination        A reference to the IPv6 destination address.
     * @param[out]  aPrefixMatchLength  A pointer to output the longest prefix match length in bits.
     * @param[out]  aRloc16             A pointer to the RLOC16 for the selected route.
     *
     * @retval kErrorNone      Successfully found a route.
     * @retval kErrorNoRoute   No valid route was found.
     *
     */
    Error RouteLookup(const Ip6::Address &aSource,
                      const Ip6::Address &aDestination,
                      uint8_t *           aPrefixMatchLength,
                      uint16_t *          aRloc16) const;

    /**
     * This method is used by non-Leader devices to set newly received Network Data from the Leader.
     *
     * @param[in]  aVersion        The Version value.
     * @param[in]  aStableVersion  The Stable Version value.
     * @param[in]  aStableOnly     TRUE if storing only the stable data, FALSE otherwise.
     * @param[in]  aMessage        A reference to the MLE message.
     * @param[in]  aMessageOffset  The offset in @p aMessage for the Network Data TLV.
     *
     * @retval kErrorNone   Successfully set the network data.
     * @retval kErrorParse  Network Data TLV in @p aMessage is not valid.
     *
     */
    Error SetNetworkData(uint8_t        aVersion,
                         uint8_t        aStableVersion,
                         bool           aStableOnly,
                         const Message &aMessage,
                         uint16_t       aMessageOffset);

    /**
     * This method returns a pointer to the Commissioning Data.
     *
     * @returns A pointer to the Commissioning Data or nullptr if no Commissioning Data exists.
     *
     */
    CommissioningDataTlv *GetCommissioningData(void) { return AsNonConst(AsConst(this)->GetCommissioningData()); }

    /**
     * This method returns a pointer to the Commissioning Data.
     *
     * @returns A pointer to the Commissioning Data or nullptr if no Commissioning Data exists.
     *
     */
    const CommissioningDataTlv *GetCommissioningData(void) const;

    /**
     * This method returns a pointer to the Commissioning Data Sub-TLV.
     *
     * @param[in]  aType  The TLV type value.
     *
     * @returns A pointer to the Commissioning Data Sub-TLV or nullptr if no Sub-TLV exists.
     *
     */
    MeshCoP::Tlv *GetCommissioningDataSubTlv(MeshCoP::Tlv::Type aType)
    {
        return AsNonConst(AsConst(this)->GetCommissioningDataSubTlv(aType));
    }

    /**
     * This method returns a pointer to the Commissioning Data Sub-TLV.
     *
     * @param[in]  aType  The TLV type value.
     *
     * @returns A pointer to the Commissioning Data Sub-TLV or nullptr if no Sub-TLV exists.
     *
     */
    const MeshCoP::Tlv *GetCommissioningDataSubTlv(MeshCoP::Tlv::Type aType) const;

    /**
     * This method indicates whether or not the Commissioning Data TLV indicates Joining is enabled.
     *
     * Joining is enabled if a Border Agent Locator TLV exist and the Steering Data TLV is non-zero.
     *
     * @returns TRUE if the Commissioning Data TLV says Joining is enabled, FALSE otherwise.
     *
     */
    bool IsJoiningEnabled(void) const;

    /**
     * This method adds Commissioning Data to the Thread Network Data.
     *
     * @param[in]  aValue        A pointer to the Commissioning Data value.
     * @param[in]  aValueLength  The length of @p aValue.
     *
     * @retval kErrorNone     Successfully added the Commissioning Data.
     * @retval kErrorNoBufs   Insufficient space to add the Commissioning Data.
     *
     */
    Error SetCommissioningData(const uint8_t *aValue, uint8_t aValueLength);

    /**
     * This method checks if the steering data includes a Joiner.
     *
     * @param[in]  aEui64             A reference to the Joiner's IEEE EUI-64.
     *
     * @retval kErrorNone          @p aEui64 is in the bloom filter.
     * @retval kErrorInvalidState  No steering data present.
     * @retval kErrorNotFound      @p aEui64 is not in the bloom filter.
     *
     */
    Error SteeringDataCheckJoiner(const Mac::ExtAddress &aEui64) const;

    /**
     * This method checks if the steering data includes a Joiner with a given discerner value.
     *
     * @param[in]  aDiscerner         A reference to the Joiner Discerner.
     *
     * @retval kErrorNone          @p aDiscerner is in the bloom filter.
     * @retval kErrorInvalidState  No steering data present.
     * @retval kErrorNotFound      @p aDiscerner is not in the bloom filter.
     *
     */
    Error SteeringDataCheckJoiner(const MeshCoP::JoinerDiscerner &aDiscerner) const;

    /**
     * This method gets the Service ID for the specified service.
     *
     * @param[in]  aEnterpriseNumber  Enterprise Number (IANA-assigned) for Service TLV
     * @param[in]  aServiceData       The Service Data.
     * @param[in]  aServerStable      The Stable flag value for Server TLV.
     * @param[out] aServiceId         A reference where to put the Service ID.
     *
     * @retval kErrorNone       Successfully got the Service ID.
     * @retval kErrorNotFound   The specified service was not found.
     *
     */
    Error GetServiceId(uint32_t           aEnterpriseNumber,
                       const ServiceData &aServiceData,
                       bool               aServerStable,
                       uint8_t &          aServiceId) const;

protected:
    uint8_t mStableVersion;
    uint8_t mVersion;

private:
    using FilterIndexes = MeshCoP::SteeringData::HashBitIndexes;

    const PrefixTlv *FindNextMatchingPrefix(const Ip6::Address &aAddress, const PrefixTlv *aPrevTlv) const;

    void RemoveCommissioningData(void);

    Error ExternalRouteLookup(uint8_t             aDomainId,
                              const Ip6::Address &aDestination,
                              uint8_t *           aPrefixMatchLength,
                              uint16_t *          aRloc16) const;
    Error DefaultRouteLookup(const PrefixTlv &aPrefix, uint16_t *aRloc16) const;
    Error SteeringDataCheck(const FilterIndexes &aFilterIndexes) const;

    uint8_t mTlvBuffer[kMaxSize];
};

/**
 * @}
 */

} // namespace NetworkData
} // namespace ot

#if OPENTHREAD_MTD
namespace ot {
namespace NetworkData {
class Leader : public LeaderBase
{
public:
    using LeaderBase::LeaderBase;
};
} // namespace NetworkData
} // namespace ot
#elif OPENTHREAD_FTD
#include "network_data_leader_ftd.hpp"
#else
#error "Please define OPENTHREAD_MTD=1 or OPENTHREAD_FTD=1"
#endif

#endif // NETWORK_DATA_LEADER_HPP_
