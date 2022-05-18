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
 *   This file includes definitions for generating and processing MeshCoP TLVs.
 *
 */

#ifndef MESHCOP_TLVS_HPP_
#define MESHCOP_TLVS_HPP_

#include "openthread-core-config.h"

#include <openthread/commissioner.h>
#include <openthread/dataset.h>
#include <openthread/platform/radio.h>

#include "common/const_cast.hpp"
#include "common/encoding.hpp"
#include "common/message.hpp"
#include "common/string.hpp"
#include "common/tlvs.hpp"
#include "mac/mac_types.hpp"
#include "meshcop/timestamp.hpp"
#include "net/ip6_address.hpp"
#include "radio/radio.hpp"
#include "thread/key_manager.hpp"
#include "thread/mle_types.hpp"

namespace ot {
namespace MeshCoP {

using ot::Encoding::BigEndian::HostSwap16;
using ot::Encoding::BigEndian::HostSwap32;
using ot::Encoding::BigEndian::ReadUint24;
using ot::Encoding::BigEndian::WriteUint24;

/**
 * This class implements MeshCoP TLV generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class Tlv : public ot::Tlv
{
public:
    /**
     * MeshCoP TLV Types.
     *
     */
    enum Type : uint8_t
    {
        kChannel                 = OT_MESHCOP_TLV_CHANNEL,                  ///< Channel TLV
        kPanId                   = OT_MESHCOP_TLV_PANID,                    ///< PAN ID TLV
        kExtendedPanId           = OT_MESHCOP_TLV_EXTPANID,                 ///< Extended PAN ID TLV
        kNetworkName             = OT_MESHCOP_TLV_NETWORKNAME,              ///< Network Name TLV
        kPskc                    = OT_MESHCOP_TLV_PSKC,                     ///< PSKc TLV
        kNetworkKey              = OT_MESHCOP_TLV_NETWORKKEY,               ///< Network Network Key TLV
        kNetworkKeySequence      = OT_MESHCOP_TLV_NETWORK_KEY_SEQUENCE,     ///< Network Key Sequence TLV
        kMeshLocalPrefix         = OT_MESHCOP_TLV_MESHLOCALPREFIX,          ///< Mesh Local Prefix TLV
        kSteeringData            = OT_MESHCOP_TLV_STEERING_DATA,            ///< Steering Data TLV
        kBorderAgentLocator      = OT_MESHCOP_TLV_BORDER_AGENT_RLOC,        ///< Border Agent Locator TLV
        kCommissionerId          = OT_MESHCOP_TLV_COMMISSIONER_ID,          ///< Commissioner ID TLV
        kCommissionerSessionId   = OT_MESHCOP_TLV_COMM_SESSION_ID,          ///< Commissioner Session ID TLV
        kSecurityPolicy          = OT_MESHCOP_TLV_SECURITYPOLICY,           ///< Security Policy TLV
        kGet                     = OT_MESHCOP_TLV_GET,                      ///< Get TLV
        kActiveTimestamp         = OT_MESHCOP_TLV_ACTIVETIMESTAMP,          ///< Active Timestamp TLV
        kCommissionerUdpPort     = OT_MESHCOP_TLV_COMMISSIONER_UDP_PORT,    ///< Commissioner UDP Port TLV
        kState                   = OT_MESHCOP_TLV_STATE,                    ///< State TLV
        kJoinerDtlsEncapsulation = OT_MESHCOP_TLV_JOINER_DTLS,              ///< Joiner DTLS Encapsulation TLV
        kJoinerUdpPort           = OT_MESHCOP_TLV_JOINER_UDP_PORT,          ///< Joiner UDP Port TLV
        kJoinerIid               = OT_MESHCOP_TLV_JOINER_IID,               ///< Joiner IID TLV
        kJoinerRouterLocator     = OT_MESHCOP_TLV_JOINER_RLOC,              ///< Joiner Router Locator TLV
        kJoinerRouterKek         = OT_MESHCOP_TLV_JOINER_ROUTER_KEK,        ///< Joiner Router KEK TLV
        kProvisioningUrl         = OT_MESHCOP_TLV_PROVISIONING_URL,         ///< Provisioning URL TLV
        kVendorName              = OT_MESHCOP_TLV_VENDOR_NAME_TLV,          ///< meshcop Vendor Name TLV
        kVendorModel             = OT_MESHCOP_TLV_VENDOR_MODEL_TLV,         ///< meshcop Vendor Model TLV
        kVendorSwVersion         = OT_MESHCOP_TLV_VENDOR_SW_VERSION_TLV,    ///< meshcop Vendor SW Version TLV
        kVendorData              = OT_MESHCOP_TLV_VENDOR_DATA_TLV,          ///< meshcop Vendor Data TLV
        kVendorStackVersion      = OT_MESHCOP_TLV_VENDOR_STACK_VERSION_TLV, ///< meshcop Vendor Stack Version TLV
        kUdpEncapsulation        = OT_MESHCOP_TLV_UDP_ENCAPSULATION_TLV,    ///< meshcop UDP encapsulation TLV
        kIp6Address              = OT_MESHCOP_TLV_IPV6_ADDRESS_TLV,         ///< meshcop IPv6 address TLV
        kPendingTimestamp        = OT_MESHCOP_TLV_PENDINGTIMESTAMP,         ///< Pending Timestamp TLV
        kDelayTimer              = OT_MESHCOP_TLV_DELAYTIMER,               ///< Delay Timer TLV
        kChannelMask             = OT_MESHCOP_TLV_CHANNELMASK,              ///< Channel Mask TLV
        kCount                   = OT_MESHCOP_TLV_COUNT,                    ///< Count TLV
        kPeriod                  = OT_MESHCOP_TLV_PERIOD,                   ///< Period TLV
        kScanDuration            = OT_MESHCOP_TLV_SCAN_DURATION,            ///< Scan Duration TLV
        kEnergyList              = OT_MESHCOP_TLV_ENERGY_LIST,              ///< Energy List TLV
        kDiscoveryRequest        = OT_MESHCOP_TLV_DISCOVERYREQUEST,         ///< Discovery Request TLV
        kDiscoveryResponse       = OT_MESHCOP_TLV_DISCOVERYRESPONSE,        ///< Discovery Response TLV
        kJoinerAdvertisement     = OT_MESHCOP_TLV_JOINERADVERTISEMENT,      ///< Joiner Advertisement TLV
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

    /**
     * This method returns a pointer to the next TLV.
     *
     * @returns A pointer to the next TLV.
     *
     */
    Tlv *GetNext(void) { return static_cast<Tlv *>(ot::Tlv::GetNext()); }

    /**
     * This method returns a pointer to the next TLV.
     *
     * @returns A pointer to the next TLV.
     *
     */
    const Tlv *GetNext(void) const { return static_cast<const Tlv *>(ot::Tlv::GetNext()); }

    /**
     * This static method reads the requested TLV out of @p aMessage.
     *
     * @param[in]   aMessage    A reference to the message.
     * @param[in]   aType       The Type value to search for.
     * @param[in]   aMaxLength  Maximum number of bytes to read.
     * @param[out]  aTlv        A reference to the TLV that will be copied to.
     *
     * @retval kErrorNone      Successfully copied the TLV.
     * @retval kErrorNotFound  Could not find the TLV with Type @p aType.
     *
     */
    static Error FindTlv(const Message &aMessage, Type aType, uint16_t aMaxLength, Tlv &aTlv)
    {
        return ot::Tlv::FindTlv(aMessage, static_cast<uint8_t>(aType), aMaxLength, aTlv);
    }

    /**
     * This static method reads the requested TLV out of @p aMessage.
     *
     * This method can be used independent of whether the read TLV (from message) is an Extended TLV or not.
     *
     * @tparam      TlvType     The TlvType to search for (must be a sub-class of `Tlv`).
     *
     * @param[in]   aMessage    A reference to the message.
     * @param[out]  aTlv        A reference to the TLV that will be copied to.
     *
     * @retval kErrorNone      Successfully copied the TLV.
     * @retval kErrorNotFound  Could not find the TLV with Type @p aType.
     *
     */

    template <typename TlvType> static Error FindTlv(const Message &aMessage, TlvType &aTlv)
    {
        return ot::Tlv::FindTlv(aMessage, aTlv);
    }

    /**
     * This static method indicates whether a TLV appears to be well-formed.
     *
     * @param[in]  aTlv  A reference to the TLV.
     *
     * @returns TRUE if the TLV appears to be well-formed, FALSE otherwise.
     *
     */
    static bool IsValid(const Tlv &aTlv);

    /**
     * This static method searches in a given sequence of TLVs to find the first TLV with a given template Type.
     *
     * @param[in]  aTlvsStart  A pointer to the start of the sequence of TLVs to search within.
     * @param[in]  aTlvsLength The length (number of bytes) in TLV sequence.
     * @param[in]  aType       The TLV Type to search for.
     *
     * @returns A pointer to the TLV if found, or nullptr if not found.
     *
     */
    static Tlv *FindTlv(uint8_t *aTlvsStart, uint16_t aTlvsLength, Type aType)
    {
        return AsNonConst(FindTlv(AsConst(aTlvsStart), aTlvsLength, aType));
    }

    /**
     * This static method searches in a given sequence of TLVs to find the first TLV with a given template Type.
     *
     * @param[in]  aTlvsStart  A pointer to the start of the sequence of TLVs to search within.
     * @param[in]  aTlvsLength The length (number of bytes) in TLV sequence.
     * @param[in]  aType       The TLV Type to search for.
     *
     * @returns A pointer to the TLV if found, or nullptr if not found.
     *
     */
    static const Tlv *FindTlv(const uint8_t *aTlvsStart, uint16_t aTlvsLength, Type aType);

    /**
     * This static template method searches in a given sequence of TLVs to find the first TLV with a give template
     * `TlvType`.
     *
     * @param[in]  aTlvsStart  A pointer to the start of the sequence of TLVs to search within.
     * @param[in]  aTlvsLength The length (number of bytes) in TLV sequence.
     *
     * @returns A pointer to the TLV if found, or nullptr if not found.
     *
     */
    template <typename TlvType> static TlvType *FindTlv(uint8_t *aTlvsStart, uint16_t aTlvsLength)
    {
        return static_cast<TlvType *>(FindTlv(aTlvsStart, aTlvsLength, static_cast<Tlv::Type>(TlvType::kType)));
    }

    /**
     * This static template method searches in a given sequence of TLVs to find the first TLV with a give template
     * `TlvType`.
     *
     * @param[in]  aTlvsStart  A pointer to the start of the sequence of TLVs to search within.
     * @param[in]  aTlvsLength The length (number of bytes) in TLV sequence.
     *
     * @returns A pointer to the TLV if found, or nullptr if not found.
     *
     */
    template <typename TlvType> static const TlvType *FindTlv(const uint8_t *aTlvsStart, uint16_t aTlvsLength)
    {
        return static_cast<const TlvType *>(FindTlv(aTlvsStart, aTlvsLength, static_cast<Tlv::Type>(TlvType::kType)));
    }

} OT_TOOL_PACKED_END;

/**
 * This class implements extended MeshCoP TLV generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class ExtendedTlv : public ot::ExtendedTlv
{
public:
    /**
     * This method returns the Type value.
     *
     * @returns The Type value.
     *
     */
    MeshCoP::Tlv::Type GetType(void) const { return static_cast<MeshCoP::Tlv::Type>(ot::ExtendedTlv::GetType()); }

    /**
     * This method sets the Type value.
     *
     * @param[in]  aType  The Type value.
     *
     */
    void SetType(MeshCoP::Tlv::Type aType) { ot::ExtendedTlv::SetType(static_cast<uint8_t>(aType)); }
} OT_TOOL_PACKED_END;

/**
 * This class defines Commissioner UDP Port TLV constants and types.
 *
 */
typedef UintTlvInfo<Tlv::kCommissionerUdpPort, uint16_t> CommissionerUdpPortTlv;

/**
 * This class defines IPv6 Address TLV constants and types.
 *
 */
typedef SimpleTlvInfo<Tlv::kIp6Address, Ip6::Address> Ip6AddressTlv;

/**
 * This class defines Joiner IID TLV constants and types.
 *
 */
typedef SimpleTlvInfo<Tlv::kJoinerIid, Ip6::InterfaceIdentifier> JoinerIidTlv;

/**
 * This class defines Joiner Router Locator TLV constants and types.
 *
 */
typedef UintTlvInfo<Tlv::kJoinerRouterLocator, uint16_t> JoinerRouterLocatorTlv;

/**
 * This class defines Joiner Router KEK TLV constants and types.
 *
 */
typedef SimpleTlvInfo<Tlv::kJoinerRouterKek, Kek> JoinerRouterKekTlv;

/**
 * This class defines Count TLV constants and types.
 *
 */
typedef UintTlvInfo<Tlv::kCount, uint8_t> CountTlv;

/**
 * This class defines Period TLV constants and types.
 *
 */
typedef UintTlvInfo<Tlv::kPeriod, uint16_t> PeriodTlv;

/**
 * This class defines Scan Duration TLV constants and types.
 *
 */
typedef UintTlvInfo<Tlv::kScanDuration, uint16_t> ScanDurationTlv;

/**
 * This class implements Channel TLV generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class ChannelTlv : public Tlv, public TlvInfo<Tlv::kChannel>
{
public:
    /**
     * This method initializes the TLV.
     *
     */
    void Init(void)
    {
        SetType(kChannel);
        SetLength(sizeof(*this) - sizeof(Tlv));
    }

    /**
     * This method indicates whether or not the TLV appears to be well-formed.
     *
     * @retval TRUE   If the TLV appears to be well-formed.
     * @retval FALSE  If the TLV does not appear to be well-formed.
     *
     */
    bool IsValid(void) const;

    /**
     * This method returns the ChannelPage value.
     *
     * @returns The ChannelPage value.
     *
     */
    uint8_t GetChannelPage(void) const { return mChannelPage; }

    /**
     * This method sets the ChannelPage value.
     *
     * @param[in]  aChannelPage  The ChannelPage value.
     *
     */
    void SetChannelPage(uint8_t aChannelPage) { mChannelPage = aChannelPage; }

    /**
     * This method returns the Channel value.
     *
     * @returns The Channel value.
     *
     */
    uint16_t GetChannel(void) const { return HostSwap16(mChannel); }

    /**
     * This method sets the Channel value.
     * Note: This method also sets the channel page according to the channel value.
     *
     * @param[in]  aChannel  The Channel value.
     *
     */
    void SetChannel(uint16_t aChannel);

private:
    uint8_t  mChannelPage;
    uint16_t mChannel;
} OT_TOOL_PACKED_END;

/**
 * This class implements PAN ID TLV generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class PanIdTlv : public Tlv, public UintTlvInfo<Tlv::kPanId, uint16_t>
{
public:
    /**
     * This method initializes the TLV.
     *
     */
    void Init(void)
    {
        SetType(kPanId);
        SetLength(sizeof(*this) - sizeof(Tlv));
    }

    /**
     * This method indicates whether or not the TLV appears to be well-formed.
     *
     * @retval TRUE   If the TLV appears to be well-formed.
     * @retval FALSE  If the TLV does not appear to be well-formed.
     *
     */
    bool IsValid(void) const { return GetLength() >= sizeof(*this) - sizeof(Tlv); }

    /**
     * This method returns the PAN ID value.
     *
     * @returns The PAN ID value.
     *
     */
    uint16_t GetPanId(void) const { return HostSwap16(mPanId); }

    /**
     * This method sets the PAN ID value.
     *
     * @param[in]  aPanId  The PAN ID value.
     *
     */
    void SetPanId(uint16_t aPanId) { mPanId = HostSwap16(aPanId); }

private:
    uint16_t mPanId;
} OT_TOOL_PACKED_END;

/**
 * This class implements Extended PAN ID TLV generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class ExtendedPanIdTlv : public Tlv, public SimpleTlvInfo<Tlv::kExtendedPanId, Mac::ExtendedPanId>
{
public:
    /**
     * This method initializes the TLV.
     *
     */
    void Init(void)
    {
        SetType(kExtendedPanId);
        SetLength(sizeof(*this) - sizeof(Tlv));
    }

    /**
     * This method indicates whether or not the TLV appears to be well-formed.
     *
     * @retval TRUE   If the TLV appears to be well-formed.
     * @retval FALSE  If the TLV does not appear to be well-formed.
     *
     */
    bool IsValid(void) const { return GetLength() >= sizeof(*this) - sizeof(Tlv); }

    /**
     * This method returns the Extended PAN ID value.
     *
     * @returns The Extended PAN ID value.
     *
     */
    const Mac::ExtendedPanId &GetExtendedPanId(void) const { return mExtendedPanId; }

    /**
     * This method sets the Extended PAN ID value.
     *
     * @param[in]  aExtendedPanId  An Extended PAN ID value.
     *
     */
    void SetExtendedPanId(const Mac::ExtendedPanId &aExtendedPanId) { mExtendedPanId = aExtendedPanId; }

private:
    Mac::ExtendedPanId mExtendedPanId;
} OT_TOOL_PACKED_END;

/**
 * This class implements Network Name TLV generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class NetworkNameTlv : public Tlv, public TlvInfo<Tlv::kNetworkName>
{
public:
    /**
     * This method initializes the TLV.
     *
     */
    void Init(void)
    {
        SetType(kNetworkName);
        SetLength(sizeof(*this) - sizeof(Tlv));
    }

    /**
     * This method indicates whether or not the TLV appears to be well-formed.
     *
     * @retval TRUE   If the TLV appears to be well-formed.
     * @retval FALSE  If the TLV does not appear to be well-formed.
     *
     */
    bool IsValid(void) const;

    /**
     * This method gets the Network Name value.
     *
     * @returns The Network Name value (as `NameData`).
     *
     */
    Mac::NameData GetNetworkName(void) const;

    /**
     * This method sets the Network Name value.
     *
     * @param[in] aNameData   A Network Name value (as `NameData`).
     *
     */
    void SetNetworkName(const Mac::NameData &aNameData);

private:
    char mNetworkName[Mac::NetworkName::kMaxSize];
} OT_TOOL_PACKED_END;

/**
 * This class implements PSKc TLV generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class PskcTlv : public Tlv, public SimpleTlvInfo<Tlv::kPskc, Pskc>
{
public:
    /**
     * This method initializes the TLV.
     *
     */
    void Init(void)
    {
        SetType(kPskc);
        SetLength(sizeof(*this) - sizeof(Tlv));
    }

    /**
     * This method indicates whether or not the TLV appears to be well-formed.
     *
     * @retval TRUE   If the TLV appears to be well-formed.
     * @retval FALSE  If the TLV does not appear to be well-formed.
     *
     */
    bool IsValid(void) const { return GetLength() >= sizeof(*this) - sizeof(Tlv); }

    /**
     * This method returns the PSKc value.
     *
     * @returns The PSKc value.
     *
     */
    const Pskc &GetPskc(void) const { return mPskc; }

    /**
     * This method sets the PSKc value.
     *
     * @param[in]  aPskc  A pointer to the PSKc value.
     *
     */
    void SetPskc(const Pskc &aPskc) { mPskc = aPskc; }

private:
    Pskc mPskc;
} OT_TOOL_PACKED_END;

/**
 * This class implements Network Network Key TLV generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class NetworkKeyTlv : public Tlv, public SimpleTlvInfo<Tlv::kNetworkKey, NetworkKey>
{
public:
    /**
     * This method initializes the TLV.
     *
     */
    void Init(void)
    {
        SetType(kNetworkKey);
        SetLength(sizeof(*this) - sizeof(Tlv));
    }

    /**
     * This method indicates whether or not the TLV appears to be well-formed.
     *
     * @retval TRUE   If the TLV appears to be well-formed.
     * @retval FALSE  If the TLV does not appear to be well-formed.
     *
     */
    bool IsValid(void) const { return GetLength() >= sizeof(*this) - sizeof(Tlv); }

    /**
     * This method returns the Network Network Key value.
     *
     * @returns The Network Network Key value.
     *
     */
    const NetworkKey &GetNetworkKey(void) const { return mNetworkKey; }

    /**
     * This method sets the Network Network Key value.
     *
     * @param[in]  aNetworkKey  The Network Network Key.
     *
     */
    void SetNetworkKey(const NetworkKey &aNetworkKey) { mNetworkKey = aNetworkKey; }

private:
    NetworkKey mNetworkKey;
} OT_TOOL_PACKED_END;

/**
 * This class implements Network Key Sequence TLV generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class NetworkKeySequenceTlv : public Tlv, public UintTlvInfo<Tlv::kNetworkKeySequence, uint32_t>
{
public:
    /**
     * This method initializes the TLV.
     *
     */
    void Init(void)
    {
        SetType(kNetworkKeySequence);
        SetLength(sizeof(*this) - sizeof(Tlv));
    }

    /**
     * This method indicates whether or not the TLV appears to be well-formed.
     *
     * @retval TRUE   If the TLV appears to be well-formed.
     * @retval FALSE  If the TLV does not appear to be well-formed.
     *
     */
    bool IsValid(void) const { return GetLength() >= sizeof(*this) - sizeof(Tlv); }

    /**
     * This method returns the Network Key Sequence value.
     *
     * @returns The Network Key Sequence value.
     *
     */
    uint32_t GetNetworkKeySequence(void) const { return HostSwap32(mNetworkKeySequence); }

    /**
     * This method sets the Network Key Sequence value.
     *
     * @param[in]  aNetworkKeySequence  The Network Key Sequence value.
     *
     */
    void SetNetworkKeySequence(uint32_t aNetworkKeySequence) { mNetworkKeySequence = HostSwap32(aNetworkKeySequence); }

private:
    uint32_t mNetworkKeySequence;
} OT_TOOL_PACKED_END;

/**
 * This class implements Mesh Local Prefix TLV generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class MeshLocalPrefixTlv : public Tlv, public SimpleTlvInfo<Tlv::kMeshLocalPrefix, Mle::MeshLocalPrefix>
{
public:
    /**
     * This method initializes the TLV.
     *
     */
    void Init(void)
    {
        SetType(kMeshLocalPrefix);
        SetLength(sizeof(*this) - sizeof(Tlv));
    }

    /**
     * This method indicates whether or not the TLV appears to be well-formed.
     *
     * @retval TRUE   If the TLV appears to be well-formed.
     * @retval FALSE  If the TLV does not appear to be well-formed.
     *
     */
    bool IsValid(void) const { return GetLength() >= sizeof(*this) - sizeof(Tlv); }

    /**
     * This method returns the size (in bytes) of the Mesh Local Prefix field.
     *
     * @returns The size (in bytes) of the Mesh Local Prefix field (8 bytes).
     *
     */
    uint8_t GetMeshLocalPrefixLength(void) const { return sizeof(mMeshLocalPrefix); }

    /**
     * This method returns the Mesh Local Prefix value.
     *
     * @returns The Mesh Local Prefix value.
     *
     */
    const Mle::MeshLocalPrefix &GetMeshLocalPrefix(void) const { return mMeshLocalPrefix; }

    /**
     * This method sets the Mesh Local Prefix value.
     *
     * @param[in]  aMeshLocalPrefix  A pointer to the Mesh Local Prefix value.
     *
     */
    void SetMeshLocalPrefix(const Mle::MeshLocalPrefix &aMeshLocalPrefix) { mMeshLocalPrefix = aMeshLocalPrefix; }

private:
    Mle::MeshLocalPrefix mMeshLocalPrefix;
} OT_TOOL_PACKED_END;

class SteeringData;

/**
 * This class implements Steering Data TLV generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class SteeringDataTlv : public Tlv, public TlvInfo<Tlv::kSteeringData>
{
public:
    /**
     * This method initializes the TLV.
     *
     */
    void Init(void)
    {
        SetType(kSteeringData);
        SetLength(sizeof(*this) - sizeof(Tlv));
        Clear();
    }

    /**
     * This method indicates whether or not the TLV appears to be well-formed.
     *
     * @retval TRUE   If the TLV appears to be well-formed.
     * @retval FALSE  If the TLV does not appear to be well-formed.
     *
     */
    bool IsValid(void) const { return GetLength() > 0; }

    /**
     * This method returns the Steering Data length.
     *
     * @returns The Steering Data length.
     *
     */
    uint8_t GetSteeringDataLength(void) const
    {
        return GetLength() <= sizeof(mSteeringData) ? GetLength() : sizeof(mSteeringData);
    }

    /**
     * This method sets all bits in the Bloom Filter to zero.
     *
     */
    void Clear(void) { memset(mSteeringData, 0, GetSteeringDataLength()); }

    /**
     * This method copies the Steering Data from the TLV into a given `SteeringData` variable.
     *
     * @param[out]  aSteeringData   A reference to a `SteeringData` to copy into.
     *
     */
    void CopyTo(SteeringData &aSteeringData) const;

private:
    uint8_t mSteeringData[OT_STEERING_DATA_MAX_LENGTH];
} OT_TOOL_PACKED_END;

/**
 * This class implements Border Agent Locator TLV generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class BorderAgentLocatorTlv : public Tlv, public UintTlvInfo<Tlv::kBorderAgentLocator, uint16_t>
{
public:
    /**
     * This method initializes the TLV.
     *
     */
    void Init(void)
    {
        SetType(kBorderAgentLocator);
        SetLength(sizeof(*this) - sizeof(Tlv));
    }

    /**
     * This method indicates whether or not the TLV appears to be well-formed.
     *
     * @retval TRUE   If the TLV appears to be well-formed.
     * @retval FALSE  If the TLV does not appear to be well-formed.
     *
     */
    bool IsValid(void) const { return GetLength() >= sizeof(*this) - sizeof(Tlv); }

    /**
     * This method returns the Border Agent Locator value.
     *
     * @returns The Border Agent Locator value.
     *
     */
    uint16_t GetBorderAgentLocator(void) const { return HostSwap16(mLocator); }

    /**
     * This method sets the Border Agent Locator value.
     *
     * @param[in]  aBorderAgentLocator  The Border Agent Locator value.
     *
     */
    void SetBorderAgentLocator(uint16_t aLocator) { mLocator = HostSwap16(aLocator); }

private:
    uint16_t mLocator;
} OT_TOOL_PACKED_END;

/**
 * This class implements the Commissioner ID TLV generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class CommissionerIdTlv : public Tlv, public TlvInfo<Tlv::kCommissionerId>
{
public:
    static constexpr uint8_t kMaxLength = 64; ///< maximum length (bytes)

    /**
     * This method initializes the TLV.
     *
     */
    void Init(void)
    {
        SetType(kCommissionerId);
        SetLength(sizeof(*this) - sizeof(Tlv));
    }

    /**
     * This method returns the Commissioner ID length.
     *
     * @returns The Commissioner ID length.
     *
     */
    uint8_t GetCommissionerIdLength(void) const
    {
        return GetLength() <= sizeof(mCommissionerId) ? GetLength() : sizeof(mCommissionerId);
    }

    /**
     * This method returns the Commissioner ID value.
     *
     * @returns The Commissioner ID value.
     *
     */
    const char *GetCommissionerId(void) const { return mCommissionerId; }

    /**
     * This method sets the Commissioner ID value.
     *
     * @param[in]  aCommissionerId  A pointer to the Commissioner ID value.
     *
     */
    void SetCommissionerId(const char *aCommissionerId)
    {
        uint16_t length = StringLength(aCommissionerId, sizeof(mCommissionerId));
        memcpy(mCommissionerId, aCommissionerId, length);
        SetLength(static_cast<uint8_t>(length));
    }

private:
    char mCommissionerId[kMaxLength];
} OT_TOOL_PACKED_END;

/**
 * This class implements Commissioner Session ID TLV generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class CommissionerSessionIdTlv : public Tlv, public UintTlvInfo<Tlv::kCommissionerSessionId, uint16_t>
{
public:
    /**
     * This method initializes the TLV.
     *
     */
    void Init(void)
    {
        SetType(kCommissionerSessionId);
        SetLength(sizeof(*this) - sizeof(Tlv));
    }

    /**
     * This method indicates whether or not the TLV appears to be well-formed.
     *
     * @retval TRUE   If the TLV appears to be well-formed.
     * @retval FALSE  If the TLV does not appear to be well-formed.
     *
     */
    bool IsValid(void) const { return GetLength() >= sizeof(*this) - sizeof(Tlv); }

    /**
     * This method returns the Commissioner Session ID value.
     *
     * @returns The Commissioner Session ID value.
     *
     */
    uint16_t GetCommissionerSessionId(void) const { return HostSwap16(mSessionId); }

    /**
     * This method sets the Commissioner Session ID value.
     *
     * @param[in]  aCommissionerSessionId  The Commissioner Session ID value.
     *
     */
    void SetCommissionerSessionId(uint16_t aSessionId) { mSessionId = HostSwap16(aSessionId); }

private:
    uint16_t mSessionId;
} OT_TOOL_PACKED_END;

/**
 * This class implements Security Policy TLV generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class SecurityPolicyTlv : public Tlv, public TlvInfo<Tlv::kSecurityPolicy>
{
public:
    /**
     * This method initializes the TLV.
     *
     */
    void Init(void)
    {
        SetType(kSecurityPolicy);
        SetLength(sizeof(*this) - sizeof(Tlv));
    }

    /**
     * This method indicates whether or not the TLV appears to be well-formed.
     *
     * @retval TRUE   If the TLV appears to be well-formed.
     * @retval FALSE  If the TLV does not appear to be well-formed.
     *
     */
    bool IsValid(void) const;

    /**
     * This method returns the Security Policy.
     *
     * @returns  The Security Policy.
     *
     */
    SecurityPolicy GetSecurityPolicy(void) const;

    /**
     * This method sets the Security Policy.
     *
     * @param[in]  aSecurityPolicy  The Security Policy which will be set.
     *
     */
    void SetSecurityPolicy(const SecurityPolicy &aSecurityPolicy);

private:
    static constexpr uint8_t kThread11FlagsLength = 1; // The Thread 1.1 Security Policy Flags length.
    static constexpr uint8_t kThread12FlagsLength = 2; // The Thread 1.2 Security Policy Flags length.

    void     SetRotationTime(uint16_t aRotationTime) { mRotationTime = HostSwap16(aRotationTime); }
    uint16_t GetRotationTime(void) const { return HostSwap16(mRotationTime); }
    uint8_t  GetFlagsLength(void) const { return GetLength() - sizeof(mRotationTime); }

    uint16_t mRotationTime;
#if OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2
    uint8_t mFlags[kThread12FlagsLength];
#else
    uint8_t mFlags[kThread11FlagsLength];
#endif
} OT_TOOL_PACKED_END;

/**
 * This class implements Active Timestamp TLV generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class ActiveTimestampTlv : public Tlv, public SimpleTlvInfo<Tlv::kActiveTimestamp, Timestamp>
{
public:
    /**
     * This method initializes the TLV.
     *
     */
    void Init(void)
    {
        SetType(kActiveTimestamp);
        SetLength(sizeof(*this) - sizeof(Tlv));
        mTimestamp.Clear();
    }

    /**
     * This method indicates whether or not the TLV appears to be well-formed.
     *
     * @retval TRUE   If the TLV appears to be well-formed.
     * @retval FALSE  If the TLV does not appear to be well-formed.
     *
     */
    bool IsValid(void) const { return GetLength() >= sizeof(*this) - sizeof(Tlv); }

    /**
     * This method gets the timestamp.
     *
     * @returns The timestamp.
     *
     */
    const Timestamp &GetTimestamp(void) const { return mTimestamp; }

    /**
     * This method returns a reference to the timestamp.
     *
     * @returns A reference to the timestamp.
     *
     */
    Timestamp &GetTimestamp(void) { return mTimestamp; }

    /**
     * This method sets the timestamp.
     *
     * @param[in] aTimestamp   The new timestamp.
     *
     */
    void SetTimestamp(const Timestamp &aTimestamp) { mTimestamp = aTimestamp; }

private:
    Timestamp mTimestamp;
} OT_TOOL_PACKED_END;

/**
 * This class implements State TLV generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class StateTlv : public Tlv, public UintTlvInfo<Tlv::kState, uint8_t>
{
public:
    /**
     * This method initializes the TLV.
     *
     */
    void Init(void)
    {
        SetType(kState);
        SetLength(sizeof(*this) - sizeof(Tlv));
    }

    /**
     * This method indicates whether or not the TLV appears to be well-formed.
     *
     * @retval TRUE   If the TLV appears to be well-formed.
     * @retval FALSE  If the TLV does not appear to be well-formed.
     *
     */
    bool IsValid(void) const { return GetLength() >= sizeof(*this) - sizeof(Tlv); }

    /**
     * State values.
     *
     */
    enum State : uint8_t
    {
        kReject  = 0xff, ///< Reject (-1)
        kPending = 0,    ///< Pending
        kAccept  = 1,    ///< Accept
    };

    /**
     * This method returns the State value.
     *
     * @returns The State value.
     *
     */
    State GetState(void) const { return static_cast<State>(mState); }

    /**
     * This method sets the State value.
     *
     * @param[in]  aState  The State value.
     *
     */
    void SetState(State aState) { mState = static_cast<uint8_t>(aState); }

private:
    uint8_t mState;
} OT_TOOL_PACKED_END;

/**
 * This class implements Joiner UDP Port TLV generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class JoinerUdpPortTlv : public Tlv, public UintTlvInfo<Tlv::kJoinerUdpPort, uint16_t>
{
public:
    /**
     * This method initializes the TLV.
     *
     */
    void Init(void)
    {
        SetType(kJoinerUdpPort);
        SetLength(sizeof(*this) - sizeof(Tlv));
    }

    /**
     * This method indicates whether or not the TLV appears to be well-formed.
     *
     * @retval TRUE   If the TLV appears to be well-formed.
     * @retval FALSE  If the TLV does not appear to be well-formed.
     *
     */
    bool IsValid(void) const { return GetLength() >= sizeof(*this) - sizeof(Tlv); }

    /**
     * This method returns the UDP Port value.
     *
     * @returns The UDP Port value.
     *
     */
    uint16_t GetUdpPort(void) const { return HostSwap16(mUdpPort); }

    /**
     * This method sets the UDP Port value.
     *
     * @param[in]  aUdpPort  The UDP Port value.
     *
     */
    void SetUdpPort(uint16_t aUdpPort) { mUdpPort = HostSwap16(aUdpPort); }

private:
    uint16_t mUdpPort;
} OT_TOOL_PACKED_END;

/**
 * This class implements Pending Timestamp TLV generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class PendingTimestampTlv : public Tlv, public SimpleTlvInfo<Tlv::kPendingTimestamp, Timestamp>
{
public:
    /**
     * This method initializes the TLV.
     *
     */
    void Init(void)
    {
        SetType(kPendingTimestamp);
        SetLength(sizeof(*this) - sizeof(Tlv));
        mTimestamp.Clear();
    }

    /**
     * This method indicates whether or not the TLV appears to be well-formed.
     *
     * @retval TRUE   If the TLV appears to be well-formed.
     * @retval FALSE  If the TLV does not appear to be well-formed.
     *
     */
    bool IsValid(void) const { return GetLength() >= sizeof(*this) - sizeof(Tlv); }

    /**
     * This method gets the timestamp.
     *
     * @returns The timestamp.
     *
     */
    const Timestamp &GetTimestamp(void) const { return mTimestamp; }

    /**
     * This method returns a reference to the timestamp.
     *
     * @returns A reference to the timestamp.
     *
     */
    Timestamp &GetTimestamp(void) { return mTimestamp; }

    /**
     * This method sets the timestamp.
     *
     * @param[in] aTimestamp   The new timestamp.
     *
     */
    void SetTimestamp(const Timestamp &aTimestamp) { mTimestamp = aTimestamp; }

private:
    Timestamp mTimestamp;
} OT_TOOL_PACKED_END;

/**
 * This class implements Delay Timer TLV generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class DelayTimerTlv : public Tlv, public UintTlvInfo<Tlv::kDelayTimer, uint32_t>
{
public:
    /**
     * This method initializes the TLV.
     *
     */
    void Init(void)
    {
        SetType(kDelayTimer);
        SetLength(sizeof(*this) - sizeof(Tlv));
    }

    /**
     * This method indicates whether or not the TLV appears to be well-formed.
     *
     * @retval TRUE   If the TLV appears to be well-formed.
     * @retval FALSE  If the TLV does not appear to be well-formed.
     *
     */
    bool IsValid(void) const { return GetLength() >= sizeof(*this) - sizeof(Tlv); }

    /**
     * This method returns the Delay Timer value.
     *
     * @returns The Delay Timer value.
     *
     */
    uint32_t GetDelayTimer(void) const { return HostSwap32(mDelayTimer); }

    /**
     * This method sets the Delay Timer value.
     *
     * @param[in]  aDelayTimer  The Delay Timer value.
     *
     */
    void SetDelayTimer(uint32_t aDelayTimer) { mDelayTimer = HostSwap32(aDelayTimer); }

    static constexpr uint32_t kMaxDelayTimer = 259200; ///< Maximum delay timer value for a Pending Dataset in seconds

    /**
     * Minimum Delay Timer value for a Pending Operational Dataset (ms)
     *
     */
    static constexpr uint32_t kDelayTimerMinimal = OPENTHREAD_CONFIG_TMF_PENDING_DATASET_MINIMUM_DELAY;

    /**
     * Default Delay Timer value for a Pending Operational Dataset (ms)
     *
     */
    static constexpr uint32_t kDelayTimerDefault = OPENTHREAD_CONFIG_TMF_PENDING_DATASET_DEFAULT_DELAY;

private:
    uint32_t mDelayTimer;
} OT_TOOL_PACKED_END;

// forward declare ChannelMaskTlv
class ChannelMaskTlv;

/**
 * This class implements Channel Mask Entry generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class ChannelMaskEntryBase
{
public:
    /**
     * This method gets the ChannelPage value.
     *
     * @returns The ChannelPage value.
     *
     */
    uint8_t GetChannelPage(void) const { return mChannelPage; }

    /**
     * This method sets the ChannelPage value.
     *
     * @param[in]  aChannelPage  The ChannelPage value.
     *
     */
    void SetChannelPage(uint8_t aChannelPage) { mChannelPage = aChannelPage; }

    /**
     * This method gets the MaskLength value.
     *
     * @returns The MaskLength value.
     *
     */
    uint8_t GetMaskLength(void) const { return mMaskLength; }

    /**
     * This method sets the MaskLength value.
     *
     * @param[in]  aMaskLength  The MaskLength value.
     *
     */
    void SetMaskLength(uint8_t aMaskLength) { mMaskLength = aMaskLength; }

    /**
     * This method returns the total size of this Channel Mask Entry including the mask.
     *
     * @returns The total size of this entry (number of bytes).
     *
     */
    uint16_t GetEntrySize(void) const { return sizeof(ChannelMaskEntryBase) + mMaskLength; }

    /**
     * This method clears the bit corresponding to @p aChannel in ChannelMask.
     *
     * @param[in]  aChannel  The channel in ChannelMask to clear.
     *
     */
    void ClearChannel(uint8_t aChannel)
    {
        uint8_t *mask = reinterpret_cast<uint8_t *>(this) + sizeof(*this);
        mask[aChannel / 8] &= ~(0x80 >> (aChannel % 8));
    }

    /**
     * This method sets the bit corresponding to @p aChannel in ChannelMask.
     *
     * @param[in]  aChannel  The channel in ChannelMask to set.
     *
     */
    void SetChannel(uint8_t aChannel)
    {
        uint8_t *mask = reinterpret_cast<uint8_t *>(this) + sizeof(*this);
        mask[aChannel / 8] |= 0x80 >> (aChannel % 8);
    }

    /**
     * This method indicates whether or not the bit corresponding to @p aChannel in ChannelMask is set.
     *
     * @param[in]  aChannel  The channel in ChannelMask to get.
     *
     */
    bool IsChannelSet(uint8_t aChannel) const
    {
        const uint8_t *mask = reinterpret_cast<const uint8_t *>(this) + sizeof(*this);
        return (aChannel < (mMaskLength * 8)) ? ((mask[aChannel / 8] & (0x80 >> (aChannel % 8))) != 0) : false;
    }

    /**
     * This method gets the next Channel Mask Entry in a Channel Mask TLV.
     *
     * @returns A pointer to next Channel Mask Entry.
     *
     */
    const ChannelMaskEntryBase *GetNext(void) const
    {
        return reinterpret_cast<const ChannelMaskEntryBase *>(reinterpret_cast<const uint8_t *>(this) + GetEntrySize());
    }

    /**
     * This method gets the next Channel Mask Entry in a Channel Mask TLV.
     *
     * @returns A pointer to next Channel Mask Entry.
     *
     */
    ChannelMaskEntryBase *GetNext(void) { return AsNonConst(AsConst(this)->GetNext()); }

private:
    uint8_t mChannelPage;
    uint8_t mMaskLength;
} OT_TOOL_PACKED_END;

/**
 * This class implements Channel Mask Entry Page 0 generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class ChannelMaskEntry : public ChannelMaskEntryBase
{
public:
    /**
     * This method initializes the entry.
     *
     */
    void Init(void)
    {
        SetChannelPage(0);
        SetMaskLength(sizeof(mMask));
    }

    /**
     * This method indicates whether or not the entry appears to be well-formed.
     *
     * @retval TRUE   If the entry appears to be well-formed.
     * @retval FALSE  If the entry does not appear to be well-formed.
     *
     */
    bool IsValid(void) const { return GetMaskLength() == sizeof(mMask); }

    /**
     * This method returns the Channel Mask value as a `uint32_t` bit mask.
     *
     * @returns The Channel Mask value.
     *
     */
    uint32_t GetMask(void) const { return Encoding::Reverse32(HostSwap32(mMask)); }

    /**
     * This method sets the Channel Mask value.
     *
     * @param[in]  aMask  The Channel Mask value.
     *
     */
    void SetMask(uint32_t aMask) { mMask = HostSwap32(Encoding::Reverse32(aMask)); }

private:
    uint32_t mMask;
} OT_TOOL_PACKED_END;

/**
 * This class implements Channel Mask TLV generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class ChannelMaskBaseTlv : public Tlv, public TlvInfo<Tlv::kChannelMask>
{
public:
    /**
     * This method initializes the TLV.
     *
     */
    void Init(void)
    {
        SetType(kChannelMask);
        SetLength(sizeof(*this) - sizeof(Tlv));
    }

    /**
     * This method indicates whether or not the TLV appears to be well-formed.
     *
     * @retval TRUE   If the TLV appears to be well-formed.
     * @retval FALSE  If the TLV does not appear to be well-formed.
     *
     */
    bool IsValid(void) const;

    /**
     * This method gets the first Channel Mask Entry in the Channel Mask TLV.
     *
     * @returns A pointer to first Channel Mask Entry or nullptr if not found.
     *
     */
    const ChannelMaskEntryBase *GetFirstEntry(void) const;

    /**
     * This method gets the first Channel Mask Entry in the Channel Mask TLV.
     *
     * @returns A pointer to first Channel Mask Entry or nullptr if not found.
     *
     */
    ChannelMaskEntryBase *GetFirstEntry(void);
} OT_TOOL_PACKED_END;

/**
 * This class implements Channel Mask TLV generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class ChannelMaskTlv : public ChannelMaskBaseTlv
{
public:
    /**
     * This method initializes the TLV.
     *
     */
    void Init(void)
    {
        SetType(kChannelMask);
        SetLength(sizeof(*this) - sizeof(Tlv));
        memset(mEntries, 0, sizeof(mEntries));
    }

    /**
     * This method sets the Channel Mask Entries.
     *
     * @param[in]  aMask  The Channel Mask value.
     *
     */
    void SetChannelMask(uint32_t aChannelMask);

    /**
     * This method returns the Channel Mask value as a `uint32_t` bit mask.
     *
     * @returns The Channel Mask or 0 if not found.
     *
     */
    uint32_t GetChannelMask(void) const;

    /**
     * This method reads message and returns the Channel Mask value as a `uint32_t` bit mask.
     *
     * @param[in]   aMessage     A reference to the message.
     *
     * @returns The Channel Mask or 0 if not found.
     *
     */
    static uint32_t GetChannelMask(const Message &aMessage);

private:
    static constexpr uint8_t kNumMaskEntries = Radio::kNumChannelPages;

    ChannelMaskEntry mEntries[kNumMaskEntries];
} OT_TOOL_PACKED_END;

/**
 * This class implements Energy List TLV generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class EnergyListTlv : public Tlv, public TlvInfo<Tlv::kEnergyList>
{
public:
    /**
     * This method initializes the TLV.
     *
     */
    void Init(void)
    {
        SetType(kEnergyList);
        SetLength(sizeof(*this) - sizeof(Tlv));
    }

    /**
     * This method indicates whether or not the TLV appears to be well-formed.
     *
     * @retval TRUE   If the TLV appears to be well-formed.
     * @retval FALSE  If the TLV does not appear to be well-formed.
     *
     */
    bool IsValid(void) const { return true; }
} OT_TOOL_PACKED_END;

/**
 * This class implements Provisioning URL TLV generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class ProvisioningUrlTlv : public Tlv, public TlvInfo<Tlv::kProvisioningUrl>
{
public:
    /**
     * Maximum number of chars in the Provisioning URL string.
     *
     */
    static constexpr uint16_t kMaxLength = OT_PROVISIONING_URL_MAX_SIZE;

    /**
     * This method initializes the TLV.
     *
     */
    void Init(void)
    {
        SetType(kProvisioningUrl);
        SetLength(0);
    }

    /*
     * This method returns the Provisioning URL length.
     *
     * @returns The Provisioning URL length.
     *
     */
    uint8_t GetProvisioningUrlLength(void) const
    {
        return GetLength() <= sizeof(mProvisioningUrl) ? GetLength() : sizeof(mProvisioningUrl);
    }

    /**
     * This method indicates whether or not the TLV appears to be well-formed.
     *
     * @retval TRUE   If the TLV appears to be well-formed.
     * @retval FALSE  If the TLV does not appear to be well-formed.
     *
     */
    bool IsValid(void) const
    {
        return GetType() == kProvisioningUrl && mProvisioningUrl[GetProvisioningUrlLength()] == '\0';
    }

    /**
     * This method returns the Provisioning URL value.
     *
     * @returns The Provisioning URL value.
     *
     */
    const char *GetProvisioningUrl(void) const { return mProvisioningUrl; }

    /**
     * This method sets the Provisioning URL value.
     *
     * @param[in]  aProvisioningUrl  A pointer to the Provisioning URL value.
     *
     */
    void SetProvisioningUrl(const char *aProvisioningUrl)
    {
        uint16_t len = aProvisioningUrl ? StringLength(aProvisioningUrl, kMaxLength) : 0;

        SetLength(static_cast<uint8_t>(len));

        if (len > 0)
        {
            memcpy(mProvisioningUrl, aProvisioningUrl, len);
        }
    }

private:
    char mProvisioningUrl[kMaxLength];
} OT_TOOL_PACKED_END;

/**
 * This class implements Vendor Name TLV generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class VendorNameTlv : public Tlv, public TlvInfo<Tlv::kVendorName>
{
public:
    /**
     * This method initializes the TLV.
     *
     */
    void Init(void)
    {
        SetType(kVendorName);
        SetLength(0);
    }

    /**
     * This method returns the Vendor Name length.
     *
     * @returns The Vendor Name length.
     *
     */
    uint8_t GetVendorNameLength(void) const
    {
        return GetLength() <= sizeof(mVendorName) ? GetLength() : sizeof(mVendorName);
    }

    /**
     * This method returns the Vendor Name value.
     *
     * @returns The Vendor Name value.
     *
     */
    const char *GetVendorName(void) const { return mVendorName; }

    /**
     * This method sets the Vendor Name value.
     *
     * @param[in]  aVendorName  A pointer to the Vendor Name value.
     *
     */
    void SetVendorName(const char *aVendorName)
    {
        uint16_t len = (aVendorName == nullptr) ? 0 : StringLength(aVendorName, sizeof(mVendorName));

        SetLength(static_cast<uint8_t>(len));

        if (len > 0)
        {
            memcpy(mVendorName, aVendorName, len);
        }
    }

private:
    static constexpr uint8_t kMaxLength = 32;

    char mVendorName[kMaxLength];
} OT_TOOL_PACKED_END;

/**
 * This class implements Vendor Model TLV generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class VendorModelTlv : public Tlv, public TlvInfo<Tlv::kVendorModel>
{
public:
    /**
     * This method initializes the TLV.
     *
     */
    void Init(void)
    {
        SetType(kVendorModel);
        SetLength(0);
    }

    /**
     * This method returns the Vendor Model length.
     *
     * @returns The Vendor Model length.
     *
     */
    uint8_t GetVendorModelLength(void) const
    {
        return GetLength() <= sizeof(mVendorModel) ? GetLength() : sizeof(mVendorModel);
    }

    /**
     * This method returns the Vendor Model value.
     *
     * @returns The Vendor Model value.
     *
     */
    const char *GetVendorModel(void) const { return mVendorModel; }

    /**
     * This method sets the Vendor Model value.
     *
     * @param[in]  aVendorModel  A pointer to the Vendor Model value.
     *
     */
    void SetVendorModel(const char *aVendorModel)
    {
        uint16_t len = (aVendorModel == nullptr) ? 0 : StringLength(aVendorModel, sizeof(mVendorModel));

        SetLength(static_cast<uint8_t>(len));

        if (len > 0)
        {
            memcpy(mVendorModel, aVendorModel, len);
        }
    }

private:
    static constexpr uint8_t kMaxLength = 32;

    char mVendorModel[kMaxLength];
} OT_TOOL_PACKED_END;

/**
 * This class implements Vendor SW Version TLV generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class VendorSwVersionTlv : public Tlv, public TlvInfo<Tlv::kVendorSwVersion>
{
public:
    /**
     * This method initializes the TLV.
     *
     */
    void Init(void)
    {
        SetType(kVendorSwVersion);
        SetLength(0);
    }

    /**
     * This method returns the Vendor SW Version length.
     *
     * @returns The Vendor SW Version length.
     *
     */
    uint8_t GetVendorSwVersionLength(void) const
    {
        return GetLength() <= sizeof(mVendorSwVersion) ? GetLength() : sizeof(mVendorSwVersion);
    }

    /**
     * This method returns the Vendor SW Version value.
     *
     * @returns The Vendor SW Version value.
     *
     */
    const char *GetVendorSwVersion(void) const { return mVendorSwVersion; }

    /**
     * This method sets the Vendor SW Version value.
     *
     * @param[in]  aVendorSwVersion  A pointer to the Vendor SW Version value.
     *
     */
    void SetVendorSwVersion(const char *aVendorSwVersion)
    {
        uint16_t len = (aVendorSwVersion == nullptr) ? 0 : StringLength(aVendorSwVersion, sizeof(mVendorSwVersion));

        SetLength(static_cast<uint8_t>(len));

        if (len > 0)
        {
            memcpy(mVendorSwVersion, aVendorSwVersion, len);
        }
    }

private:
    static constexpr uint8_t kMaxLength = 16;

    char mVendorSwVersion[kMaxLength];
} OT_TOOL_PACKED_END;

/**
 * This class implements Vendor Data TLV generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class VendorDataTlv : public Tlv, public TlvInfo<Tlv::kVendorData>
{
public:
    /**
     * This method initializes the TLV.
     *
     */
    void Init(void)
    {
        SetType(kVendorData);
        SetLength(0);
    }

    /**
     * This method returns the Vendor Data length.
     *
     * @returns The Vendor Data length.
     *
     */
    uint8_t GetVendorDataLength(void) const
    {
        return GetLength() <= sizeof(mVendorData) ? GetLength() : sizeof(mVendorData);
    }

    /**
     * This method returns the Vendor Data value.
     *
     * @returns The Vendor Data value.
     *
     */
    const char *GetVendorData(void) const { return mVendorData; }

    /**
     * This method sets the Vendor Data value.
     *
     * @param[in]  aVendorData  A pointer to the Vendor Data value.
     *
     */
    void SetVendorData(const char *aVendorData)
    {
        uint16_t len = (aVendorData == nullptr) ? 0 : StringLength(aVendorData, sizeof(mVendorData));

        SetLength(static_cast<uint8_t>(len));

        if (len > 0)
        {
            memcpy(mVendorData, aVendorData, len);
        }
    }

private:
    static constexpr uint8_t kMaxLength = 64;

    char mVendorData[kMaxLength];
} OT_TOOL_PACKED_END;

/**
 * This class implements Vendor Stack Version TLV generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class VendorStackVersionTlv : public Tlv, public TlvInfo<Tlv::kVendorStackVersion>
{
public:
    /**
     * Default constructor.
     *
     */
    VendorStackVersionTlv(void)
        : mBuildRevision(0)
        , mMinorMajor(0)
    {
    }

    /**
     * This method initializes the TLV.
     *
     */
    void Init(void)
    {
        SetType(kVendorStackVersion);
        SetLength(sizeof(*this) - sizeof(Tlv));
    }

    /**
     * This method indicates whether or not the TLV appears to be well-formed.
     *
     * @retval TRUE   If the TLV appears to be well-formed.
     * @retval FALSE  If the TLV does not appear to be well-formed.
     *
     */
    bool IsValid(void) const { return GetLength() >= sizeof(*this) - sizeof(Tlv); }

    /**
     * This method returns the Stack Vendor OUI value.
     *
     * @returns The Vendor Stack Vendor OUI value.
     *
     */
    uint32_t GetOui(void) const { return ReadUint24(mOui); }

    /**
     * This method returns the Stack Vendor OUI value.
     *
     * @param[in]  aOui  The Vendor Stack Vendor OUI value.
     *
     */
    void SetOui(uint32_t aOui) { WriteUint24(aOui, mOui); }

    /**
     * This method returns the Build value.
     *
     * @returns The Build value.
     *
     */
    uint16_t GetBuild(void) const { return (HostSwap16(mBuildRevision) & kBuildMask) >> kBuildOffset; }

    /**
     * This method sets the Build value.
     *
     * @param[in]  aBuild  The Build value.
     *
     */
    void SetBuild(uint16_t aBuild)
    {
        mBuildRevision =
            HostSwap16((HostSwap16(mBuildRevision) & ~kBuildMask) | ((aBuild << kBuildOffset) & kBuildMask));
    }

    /**
     * This method returns the Revision value.
     *
     * @returns The Revision value.
     *
     */
    uint8_t GetRevision(void) const { return (HostSwap16(mBuildRevision) & kRevMask) >> kRevOffset; }

    /**
     * This method sets the Revision value.
     *
     * @param[in]  aRevision  The Revision value.
     *
     */
    void SetRevision(uint8_t aRevision)
    {
        mBuildRevision = HostSwap16((HostSwap16(mBuildRevision) & ~kRevMask) | ((aRevision << kRevOffset) & kRevMask));
    }

    /**
     * This method returns the Minor value.
     *
     * @returns The Minor value.
     *
     */
    uint8_t GetMinor(void) const { return (mMinorMajor & kMinorMask) >> kMinorOffset; }

    /**
     * This method sets the Minor value.
     *
     * @param[in]  aMinor  The Minor value.
     *
     */
    void SetMinor(uint8_t aMinor)
    {
        mMinorMajor = (mMinorMajor & ~kMinorMask) | ((aMinor << kMinorOffset) & kMinorMask);
    }

    /**
     * This method returns the Major value.
     *
     * @returns The Major value.
     *
     */
    uint8_t GetMajor(void) const { return (mMinorMajor & kMajorMask) >> kMajorOffset; }

    /**
     * This method sets the Major value.
     *
     * @param[in] aMajor  The Major value.
     *
     */
    void SetMajor(uint8_t aMajor)
    {
        mMinorMajor = (mMinorMajor & ~kMajorMask) | ((aMajor << kMajorOffset) & kMajorMask);
    }

private:
    // For `mBuildRevision`
    static constexpr uint8_t  kBuildOffset = 4;
    static constexpr uint16_t kBuildMask   = 0xfff << kBuildOffset;
    static constexpr uint8_t  kRevOffset   = 0;
    static constexpr uint16_t kRevMask     = 0xf << kBuildOffset;

    // For `mMinorMajor`
    static constexpr uint8_t kMinorOffset = 4;
    static constexpr uint8_t kMinorMask   = 0xf << kMinorOffset;
    static constexpr uint8_t kMajorOffset = 0;
    static constexpr uint8_t kMajorMask   = 0xf << kMajorOffset;

    uint8_t  mOui[3];
    uint16_t mBuildRevision;
    uint8_t  mMinorMajor;
} OT_TOOL_PACKED_END;

/**
 * This class implements UDP Encapsulation TLV generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class UdpEncapsulationTlv : public ExtendedTlv, public TlvInfo<MeshCoP::Tlv::kUdpEncapsulation>
{
public:
    /**
     * This method initializes the TLV.
     *
     */
    void Init(void)
    {
        SetType(MeshCoP::Tlv::kUdpEncapsulation);
        SetLength(sizeof(*this) - sizeof(ExtendedTlv));
    }

    /**
     * This method indicates whether or not the TLV appears to be well-formed.
     *
     * @retval TRUE   If the TLV appears to be well-formed.
     * @retval FALSE  If the TLV does not appear to be well-formed.
     *
     */
    bool IsValid(void) const { return GetLength() >= sizeof(*this) - sizeof(ExtendedTlv); }

    /**
     * This method returns the source port.
     *
     * @returns The source port.
     *
     */
    uint16_t GetSourcePort(void) const { return HostSwap16(mSourcePort); }

    /**
     * This method updates the source port.
     *
     * @param[in]   aSourcePort     The source port.
     *
     */
    void SetSourcePort(uint16_t aSourcePort) { mSourcePort = HostSwap16(aSourcePort); }

    /**
     * This method returns the destination port.
     *
     * @returns The destination port.
     *
     */
    uint16_t GetDestinationPort(void) const { return HostSwap16(mDestinationPort); }

    /**
     * This method updates the destination port.
     *
     * @param[in]   aDestinationPort    The destination port.
     *
     */
    void SetDestinationPort(uint16_t aDestinationPort) { mDestinationPort = HostSwap16(aDestinationPort); }

    /**
     * This method returns the calculated UDP length.
     *
     * @returns The calculated UDP length.
     *
     */
    uint16_t GetUdpLength(void) const { return GetLength() - sizeof(mSourcePort) - sizeof(mDestinationPort); }

    /**
     * This method updates the UDP length.
     *
     * @param[in]   aLength     The length of UDP payload in bytes.
     *
     */
    void SetUdpLength(uint16_t aLength) { SetLength(sizeof(mSourcePort) + sizeof(mDestinationPort) + aLength); }

private:
    uint16_t mSourcePort;
    uint16_t mDestinationPort;
} OT_TOOL_PACKED_END;

/**
 * This class implements Discovery Request TLV generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class DiscoveryRequestTlv : public Tlv, public TlvInfo<Tlv::kDiscoveryRequest>
{
public:
    /**
     * This method initializes the TLV.
     *
     */
    void Init(void)
    {
        SetType(kDiscoveryRequest);
        SetLength(sizeof(*this) - sizeof(Tlv));
        mFlags    = 0;
        mReserved = 0;
    }

    /**
     * This method indicates whether or not the TLV appears to be well-formed.
     *
     * @retval TRUE   If the TLV appears to be well-formed.
     * @retval FALSE  If the TLV does not appear to be well-formed.
     *
     */
    bool IsValid(void) const { return GetLength() >= sizeof(*this) - sizeof(Tlv); }

    /**
     * This method returns the Version value.
     *
     * @returns The Version value.
     *
     */
    uint8_t GetVersion(void) const { return mFlags >> kVersionOffset; }

    /**
     * This method sets the Version value.
     *
     * @param[in]  aVersion  The Version value.
     *
     */
    void SetVersion(uint8_t aVersion)
    {
        mFlags = (mFlags & ~kVersionMask) | ((aVersion << kVersionOffset) & kVersionMask);
    }

    /**
     * This method indicates whether or not the Joiner flag is set.
     *
     * @retval TRUE   If the Joiner flag is set.
     * @retval FALSE  If the Joiner flag is not set.
     *
     */
    bool IsJoiner(void) const { return (mFlags & kJoinerMask) != 0; }

    /**
     * This method sets the Joiner flag.
     *
     * @param[in]  aJoiner  TRUE if set, FALSE otherwise.
     *
     */
    void SetJoiner(bool aJoiner)
    {
        if (aJoiner)
        {
            mFlags |= kJoinerMask;
        }
        else
        {
            mFlags &= ~kJoinerMask;
        }
    }

private:
    static constexpr uint8_t kVersionOffset = 4;
    static constexpr uint8_t kVersionMask   = 0xf << kVersionOffset;
    static constexpr uint8_t kJoinerOffset  = 3;
    static constexpr uint8_t kJoinerMask    = 1 << kJoinerOffset;

    uint8_t mFlags;
    uint8_t mReserved;
} OT_TOOL_PACKED_END;

/**
 * This class implements Discovery Response TLV generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class DiscoveryResponseTlv : public Tlv, public TlvInfo<Tlv::kDiscoveryResponse>
{
public:
    /**
     * This method initializes the TLV.
     *
     */
    void Init(void)
    {
        SetType(kDiscoveryResponse);
        SetLength(sizeof(*this) - sizeof(Tlv));
        mFlags    = 0;
        mReserved = 0;
    }

    /**
     * This method indicates whether or not the TLV appears to be well-formed.
     *
     * @retval TRUE   If the TLV appears to be well-formed.
     * @retval FALSE  If the TLV does not appear to be well-formed.
     *
     */
    bool IsValid(void) const { return GetLength() >= sizeof(*this) - sizeof(Tlv); }

    /**
     * This method returns the Version value.
     *
     * @returns The Version value.
     *
     */
    uint8_t GetVersion(void) const { return mFlags >> kVersionOffset; }

    /**
     * This method sets the Version value.
     *
     * @param[in]  aVersion  The Version value.
     *
     */
    void SetVersion(uint8_t aVersion)
    {
        mFlags = (mFlags & ~kVersionMask) | ((aVersion << kVersionOffset) & kVersionMask);
    }

    /**
     * This method indicates whether or not the Native Commissioner flag is set.
     *
     * @retval TRUE   If the Native Commissioner flag is set.
     * @retval FALSE  If the Native Commissioner flag is not set.
     *
     */
    bool IsNativeCommissioner(void) const { return (mFlags & kNativeMask) != 0; }

    /**
     * This method sets the Native Commissioner flag.
     *
     * @param[in]  aNativeCommissioner  TRUE if set, FALSE otherwise.
     *
     */
    void SetNativeCommissioner(bool aNativeCommissioner)
    {
        if (aNativeCommissioner)
        {
            mFlags |= kNativeMask;
        }
        else
        {
            mFlags &= ~kNativeMask;
        }
    }

    /**
     * This method indicates whether or not the Commercial Commissioning Mode flag is set.
     *
     * @retval TRUE   If the Commercial Commissioning Mode flag is set.
     * @retval FALSE  If the Commercial Commissioning Mode flag is not set.
     *
     */
    bool IsCommercialCommissioningMode(void) const { return (mFlags & kCCMMask) != 0; }

    /**
     * This method sets the Commercial Commissioning Mode flag.
     *
     * @param[in]  aCCM  TRUE if set, FALSE otherwise.
     *
     */
    void SetCommercialCommissioningMode(bool aCCM)
    {
        if (aCCM)
        {
            mFlags |= kCCMMask;
        }
        else
        {
            mFlags &= ~kCCMMask;
        }
    }

private:
    static constexpr uint8_t kVersionOffset = 4;
    static constexpr uint8_t kVersionMask   = 0xf << kVersionOffset;
    static constexpr uint8_t kNativeOffset  = 3;
    static constexpr uint8_t kNativeMask    = 1 << kNativeOffset;
    static constexpr uint8_t kCCMOffset     = 2;
    static constexpr uint8_t kCCMMask       = 1 << kCCMOffset;

    uint8_t mFlags;
    uint8_t mReserved;
} OT_TOOL_PACKED_END;

/**
 * This class implements Joiner Advertisement TLV generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class JoinerAdvertisementTlv : public Tlv, public TlvInfo<Tlv::kJoinerAdvertisement>
{
public:
    static constexpr uint8_t kAdvDataMaxLength = OT_JOINER_ADVDATA_MAX_LENGTH; ///< The Max Length of AdvData

    /**
     * This method initializes the TLV.
     *
     */
    void Init(void)
    {
        SetType(kJoinerAdvertisement);
        SetLength(sizeof(*this) - sizeof(Tlv));
    }

    /**
     * This method indicates whether or not the TLV appears to be well-formed.
     *
     * @retval TRUE   If the TLV appears to be well-formed.
     * @retval FALSE  If the TLV does not appear to be well-formed.
     *
     */
    bool IsValid(void) const { return GetLength() >= sizeof(mOui) && GetLength() <= sizeof(mOui) + sizeof(mAdvData); }

    /**
     * This method returns the Vendor OUI value.
     *
     * @returns The Vendor OUI value.
     *
     */
    uint32_t GetOui(void) const { return ReadUint24(mOui); }

    /**
     * This method sets the Vendor OUI value.
     *
     * @param[in]  aOui The Vendor OUI value.
     *
     */
    void SetOui(uint32_t aOui) { return WriteUint24(aOui, mOui); }

    /**
     * This method returns the Adv Data length.
     *
     * @returns The AdvData length.
     *
     */
    uint8_t GetAdvDataLength(void) const { return GetLength() - sizeof(mOui); }

    /**
     * This method returns the Adv Data value.
     *
     * @returns A pointer to the Adv Data value.
     *
     */
    const uint8_t *GetAdvData(void) const { return mAdvData; }

    /**
     * This method sets the Adv Data value.
     *
     * @param[in]  aAdvData        A pointer to the AdvData value.
     * @param[in]  aAdvDataLength  The length of AdvData in bytes.
     *
     */
    void SetAdvData(const uint8_t *aAdvData, uint8_t aAdvDataLength)
    {
        OT_ASSERT((aAdvData != nullptr) && (aAdvDataLength > 0) && (aAdvDataLength <= kAdvDataMaxLength));

        SetLength(aAdvDataLength + sizeof(mOui));
        memcpy(mAdvData, aAdvData, aAdvDataLength);
    }

private:
    uint8_t mOui[3];
    uint8_t mAdvData[kAdvDataMaxLength];
} OT_TOOL_PACKED_END;

} // namespace MeshCoP

} // namespace ot

#endif // MESHCOP_TLVS_HPP_
