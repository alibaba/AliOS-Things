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
 *   This file includes definitions and methods for generating and processing Thread Network Layer TLVs.
 */

#ifndef THREAD_TLVS_HPP_
#define THREAD_TLVS_HPP_

#include "openthread-core-config.h"

#include "common/encoding.hpp"
#include "common/message.hpp"
#include "common/tlvs.hpp"
#include "net/ip6_address.hpp"
#include "thread/mle.hpp"
#include "thread/mle_types.hpp"

namespace ot {

using ot::Encoding::BigEndian::HostSwap16;
using ot::Encoding::BigEndian::HostSwap32;

// Thread 1.2.0 5.19.13 limits the number of IPv6 addresses should be [1, 15].
constexpr uint8_t kIp6AddressesNumMin = 1;
constexpr uint8_t kIp6AddressesNumMax = 15;

/**
 * This class implements Network Layer TLV generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class ThreadTlv : public ot::Tlv
{
public:
    /**
     * Network Layer TLV Types.
     *
     */
    enum Type : uint8_t
    {
        kTarget                = 0,  ///< Target EID TLV
        kExtMacAddress         = 1,  ///< Extended MAC Address TLV
        kRloc16                = 2,  ///< RLOC16 TLV
        kMeshLocalEid          = 3,  ///< ML-EID TLV
        kStatus                = 4,  ///< Status TLV
        kLastTransactionTime   = 6,  ///< Time Since Last Transaction TLV
        kRouterMask            = 7,  ///< Router Mask TLV
        kNdOption              = 8,  ///< ND Option TLV
        kNdData                = 9,  ///< ND Data TLV
        kThreadNetworkData     = 10, ///< Thread Network Data TLV
        kTimeout               = 11, ///< Timeout TLV
        kNetworkName           = 12, ///< Network Name TLV
        kIp6Addresses          = 14, ///< IPv6 Addresses TLV
        kCommissionerSessionId = 15, ///< Commissioner Session ID TLV
    };

    /**
     * This method returns the Type value.
     *
     * @returns The Type value.
     *
     */
    Type GetType(void) const { return static_cast<Type>(ot::Tlv::GetType()); }

    /**
     * This method sets the Type value.
     *
     * @param[in]  aType  The Type value.
     *
     */
    void SetType(Type aType) { ot::Tlv::SetType(static_cast<uint8_t>(aType)); }

} OT_TOOL_PACKED_END;

/**
 * This class defines Target TLV constants and types.
 *
 */
typedef SimpleTlvInfo<ThreadTlv::kTarget, Ip6::Address> ThreadTargetTlv;

/**
 * This class defines Extended MAC Address TLV constants and types.
 *
 */
typedef SimpleTlvInfo<ThreadTlv::kExtMacAddress, Mac::ExtAddress> ThreadExtMacAddressTlv;

/**
 * This class defines RLOC16 TLV constants and types.
 *
 */
typedef UintTlvInfo<ThreadTlv::kRloc16, uint16_t> ThreadRloc16Tlv;

/**
 * This class defines ML-EID TLV constants and types.
 *
 */
typedef SimpleTlvInfo<ThreadTlv::kMeshLocalEid, Ip6::InterfaceIdentifier> ThreadMeshLocalEidTlv;

/**
 * This class defines Time Since Last Transaction TLV constants and types.
 *
 */
typedef UintTlvInfo<ThreadTlv::kLastTransactionTime, uint32_t> ThreadLastTransactionTimeTlv;

/**
 * This class defines Timeout TLV constants and types.
 *
 */
typedef UintTlvInfo<ThreadTlv::kTimeout, uint32_t> ThreadTimeoutTlv;

/**
 * This class defines Network Name TLV constants and types.
 *
 */
typedef TlvInfo<ThreadTlv::kNetworkName> ThreadNetworkNameTlv;

/**
 * This class defines Commissioner Session ID TLV constants and types.
 *
 */
typedef UintTlvInfo<ThreadTlv::kCommissionerSessionId, uint16_t> ThreadCommissionerSessionIdTlv;

/**
 * This class defines Status TLV constants and types.
 *
 */
class ThreadStatusTlv : public UintTlvInfo<ThreadTlv::kStatus, uint8_t>
{
public:
    /**
     * Status values.
     *
     */
    enum Status : uint8_t
    {
        kSuccess               = 0, ///< Success.
        kNoAddressAvailable    = 1, ///< No address available.
        kTooFewRouters         = 2, ///< Address Solicit due to too few routers.
        kHaveChildIdRequest    = 3, ///< Address Solicit due to child ID request.
        kParentPartitionChange = 4, ///< Address Solicit due to parent partition change
    };

    /**
     * Multicast Listener Registration (MLR) Status values
     *
     */
    enum MlrStatus
    {
        kMlrSuccess        = 0, ///< Successful (de)registration of all IPv6 addresses.
        kMlrInvalid        = 2, ///< Invalid IPv6 address(es) in request.
        kMlrNoPersistent   = 3, ///< This device does not support persistent registrations.
        kMlrNoResources    = 4, ///< BBR resource shortage.
        kMlrBbrNotPrimary  = 5, ///< BBR is not Primary at this moment.
        kMlrGeneralFailure = 6, ///< Reason(s) for failure are not further specified.
        kMlrStatusMax      = 6, ///< Max MLR status.
    };

    /**
     * Domain Unicast Address (DUA) Registration Status values
     *
     */
    enum DuaStatus : uint8_t
    {
        kDuaSuccess        = 0, ///< Successful registration.
        kDuaReRegister     = 1, ///< Registration was accepted but immediate reregistration is required to solve.
        kDuaInvalid        = 2, ///< Registration rejected (Fatal): Target EID is not a valid DUA.
        kDuaDuplicate      = 3, ///< Registration rejected (Fatal): DUA is already in use by another device.
        kDuaNoResources    = 4, ///< Registration rejected (Non-fatal): Backbone Router Resource shortage.
        kDuaNotPrimary     = 5, ///< Registration rejected (Non-fatal): Backbone Router is not primary at this moment.
        kDuaGeneralFailure = 6, ///< Registration failure (Non-fatal): Reason(s) not further specified.
    };
};

/**
 * This class implements Router Mask TLV generation and parsing.
 *
 */
class ThreadRouterMaskTlv : public ThreadTlv, public TlvInfo<ThreadTlv::kRouterMask>
{
public:
    /**
     * This method initializes the TLV.
     *
     */
    void Init(void)
    {
        SetType(kRouterMask);
        SetLength(sizeof(*this) - sizeof(ThreadTlv));
        mAssignedRouterIdMask.Clear();
    }

    /**
     * This method indicates whether or not the TLV appears to be well-formed.
     *
     * @retval TRUE   If the TLV appears to be well-formed.
     * @retval FALSE  If the TLV does not appear to be well-formed.
     *
     */
    bool IsValid(void) const { return GetLength() >= sizeof(*this) - sizeof(ThreadTlv); }

    /**
     * This method returns the ID Sequence value.
     *
     * @returns The ID Sequence value.
     *
     */
    uint8_t GetIdSequence(void) const { return mIdSequence; }

    /**
     * This method sets the ID Sequence value.
     *
     * @param[in]  aSequence  The ID Sequence value.
     *
     */
    void SetIdSequence(uint8_t aSequence) { mIdSequence = aSequence; }

    /**
     * This method gets the Assigned Router ID Mask.
     *
     * @returns The Assigned Router ID Mask.
     *
     */
    const Mle::RouterIdSet &GetAssignedRouterIdMask(void) const { return mAssignedRouterIdMask; }

    /**
     * This method sets the Assigned Router ID Mask.
     *
     * @param[in]  aRouterIdSet A reference to the Assigned Router ID Mask.
     *
     */
    void SetAssignedRouterIdMask(const Mle::RouterIdSet &aRouterIdSet) { mAssignedRouterIdMask = aRouterIdSet; }

private:
    uint8_t          mIdSequence;
    Mle::RouterIdSet mAssignedRouterIdMask;
};

/**
 * This class implements Thread Network Data TLV generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class ThreadNetworkDataTlv : public ThreadTlv, public TlvInfo<ThreadTlv::kThreadNetworkData>
{
public:
    /**
     * This method initializes the TLV.
     *
     */
    void Init(void)
    {
        SetType(kThreadNetworkData);
        SetLength(0);
    }

    /**
     * This method overrides same method of the base class
     *
     * @retval TRUE  the TLV appears to be well-formed.
     *
     */
    bool IsValid(void) const { return true; }

    /**
     * This method returns a pointer to the Network Data TLVs.
     *
     * @returns A pointer to the Network Data TLVs.
     *
     */
    uint8_t *GetTlvs(void) { return mTlvs; }

private:
    static constexpr uint8_t kMaxSize = 255;

    uint8_t mTlvs[kMaxSize];
} OT_TOOL_PACKED_END;

#if OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2

/**
 * This class implements IPv6 Addresses TLV generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class Ip6AddressesTlv : public ThreadTlv, public TlvInfo<ThreadTlv::kIp6Addresses>
{
public:
    /**
     * This method initializes the TLV.
     *
     */
    void Init(void) { SetType(kIp6Addresses); }

    /**
     * This method indicates whether or not the TLV appears to be well-formed.
     *
     * @retval TRUE   If the TLV appears to be well-formed.
     * @retval FALSE  If the TLV does not appear to be well-formed.
     *
     */
    bool IsValid(void) const
    {
        return GetLength() >= sizeof(Ip6::Address) * kIp6AddressesNumMin &&
               GetLength() <= sizeof(Ip6::Address) * kIp6AddressesNumMax && (GetLength() % sizeof(Ip6::Address)) == 0;
    }

    /**
     * This method returns a pointer to the IPv6 address entry.
     *
     * @param[in]  aIndex  The index into the IPv6 address list.
     *
     * @returns A reference to the IPv6 address.
     *
     */
    const Ip6::Address &GetIp6Address(uint8_t aIndex) const
    {
        return *reinterpret_cast<const Ip6::Address *>(GetValue() + (aIndex * sizeof(Ip6::Address)));
    }
} OT_TOOL_PACKED_END;

#endif // OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2

} // namespace ot

#endif // THREAD_TLVS_HPP_
