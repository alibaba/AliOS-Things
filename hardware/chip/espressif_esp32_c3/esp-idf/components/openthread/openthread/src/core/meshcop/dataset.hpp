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
 *   This file includes definitions for managing MeshCoP Datasets.
 *
 */

#ifndef MESHCOP_DATASET_HPP_
#define MESHCOP_DATASET_HPP_

#include "openthread-core-config.h"

#include <openthread/dataset.h>

#include "common/clearable.hpp"
#include "common/const_cast.hpp"
#include "common/locator.hpp"
#include "common/message.hpp"
#include "common/timer.hpp"
#include "common/type_traits.hpp"
#include "meshcop/meshcop_tlvs.hpp"
#include "thread/mle_types.hpp"

namespace ot {
namespace MeshCoP {

/**
 * This class represents MeshCop Dataset.
 *
 */
class Dataset
{
    friend class DatasetLocal;

public:
    static constexpr uint8_t kMaxSize      = OT_OPERATIONAL_DATASET_MAX_LENGTH; ///< Max size of MeshCoP Dataset (bytes)
    static constexpr uint8_t kMaxValueSize = 16;                                ///< Max size of a TLV value (bytes)
    static constexpr uint8_t kMaxGetTypes  = 64;                                ///< Max number of types in MGMT_GET.req

    /**
     * This enumeration represents the Dataset type (active or pending).
     *
     */
    enum Type : uint8_t
    {
        kActive,  ///< Active Dataset
        kPending, ///< Pending Dataset
    };

    /**
     * This class represents presence of different components in Active or Pending Operational Dataset.
     *
     */
    class Components : public otOperationalDatasetComponents, public Clearable<Components>
    {
    public:
        /**
         * This method indicates whether or not the Active Timestamp is present in the Dataset.
         *
         * @returns TRUE if Active Timestamp is present, FALSE otherwise.
         *
         */
        bool IsActiveTimestampPresent(void) const { return mIsActiveTimestampPresent; }

        /**
         * This method indicates whether or not the Pending Timestamp is present in the Dataset.
         *
         * @returns TRUE if Pending Timestamp is present, FALSE otherwise.
         *
         */
        bool IsPendingTimestampPresent(void) const { return mIsPendingTimestampPresent; }

        /**
         * This method indicates whether or not the Network Key is present in the Dataset.
         *
         * @returns TRUE if Network Key is present, FALSE otherwise.
         *
         */
        bool IsNetworkKeyPresent(void) const { return mIsNetworkKeyPresent; }

        /**
         * This method indicates whether or not the Network Name is present in the Dataset.
         *
         * @returns TRUE if Network Name is present, FALSE otherwise.
         *
         */
        bool IsNetworkNamePresent(void) const { return mIsNetworkNamePresent; }

        /**
         * This method indicates whether or not the Extended PAN ID is present in the Dataset.
         *
         * @returns TRUE if Extended PAN ID is present, FALSE otherwise.
         *
         */
        bool IsExtendedPanIdPresent(void) const { return mIsExtendedPanIdPresent; }

        /**
         * This method indicates whether or not the Mesh Local Prefix is present in the Dataset.
         *
         * @returns TRUE if Mesh Local Prefix is present, FALSE otherwise.
         *
         */
        bool IsMeshLocalPrefixPresent(void) const { return mIsMeshLocalPrefixPresent; }

        /**
         * This method indicates whether or not the Delay Timer is present in the Dataset.
         *
         * @returns TRUE if Delay Timer is present, FALSE otherwise.
         *
         */
        bool IsDelayPresent(void) const { return mIsDelayPresent; }

        /**
         * This method indicates whether or not the PAN ID is present in the Dataset.
         *
         * @returns TRUE if PAN ID is present, FALSE otherwise.
         *
         */
        bool IsPanIdPresent(void) const { return mIsPanIdPresent; }

        /**
         * This method indicates whether or not the Channel is present in the Dataset.
         *
         * @returns TRUE if Channel is present, FALSE otherwise.
         *
         */
        bool IsChannelPresent(void) const { return mIsChannelPresent; }

        /**
         * This method indicates whether or not the PSKc is present in the Dataset.
         *
         * @returns TRUE if PSKc is present, FALSE otherwise.
         *
         */
        bool IsPskcPresent(void) const { return mIsPskcPresent; }

        /**
         * This method indicates whether or not the Security Policy is present in the Dataset.
         *
         * @returns TRUE if Security Policy is present, FALSE otherwise.
         *
         */
        bool IsSecurityPolicyPresent(void) const { return mIsSecurityPolicyPresent; }

        /**
         * This method indicates whether or not the Channel Mask is present in the Dataset.
         *
         * @returns TRUE if Channel Mask is present, FALSE otherwise.
         *
         */
        bool IsChannelMaskPresent(void) const { return mIsChannelMaskPresent; }
    };

    /**
     * This type represents the information about the fields contained an Active or Pending Operational Dataset.
     *
     */
    class Info : public otOperationalDataset, public Clearable<Info>
    {
    public:
        /**
         * This method indicates whether or not the Active Timestamp is present in the Dataset.
         *
         * @returns TRUE if Active Timestamp is present, FALSE otherwise.
         *
         */
        bool IsActiveTimestampPresent(void) const { return mComponents.mIsActiveTimestampPresent; }

        /**
         * This method gets the Active Timestamp in the Dataset.
         *
         * This method MUST be used when Active Timestamp component is present in the Dataset, otherwise its behavior is
         * undefined.
         *
         * @returns The Active Timestamp in the Dataset.
         *
         */
        uint64_t GetActiveTimestamp(void) const { return mActiveTimestamp; }

        /**
         * This method sets the Active Timestamp in the Dataset.
         *
         * @param[in] aTimestamp   A Timestamp value.
         *
         */
        void SetActiveTimestamp(uint64_t aTimestamp)
        {
            mActiveTimestamp                      = aTimestamp;
            mComponents.mIsActiveTimestampPresent = true;
        }

        /**
         * This method indicates whether or not the Pending Timestamp is present in the Dataset.
         *
         * @returns TRUE if Pending Timestamp is present, FALSE otherwise.
         *
         */
        bool IsPendingTimestampPresent(void) const { return mComponents.mIsPendingTimestampPresent; }

        /**
         * This method gets the Pending Timestamp in the Dataset.
         *
         * This method MUST be used when Pending Timestamp component is present in the Dataset, otherwise its behavior
         * is undefined.
         *
         * @returns The Pending Timestamp in the Dataset.
         *
         */
        uint64_t GetPendingTimestamp(void) const { return mPendingTimestamp; }

        /**
         * This method sets the Pending Timestamp in the Dataset.
         *
         * @param[in] aTimestamp   A Timestamp value.
         *
         */
        void SetPendingTimestamp(uint64_t aTimestamp)
        {
            mPendingTimestamp                      = aTimestamp;
            mComponents.mIsPendingTimestampPresent = true;
        }

        /**
         * This method indicates whether or not the Network Key is present in the Dataset.
         *
         * @returns TRUE if Network Key is present, FALSE otherwise.
         *
         */
        bool IsNetworkKeyPresent(void) const { return mComponents.mIsNetworkKeyPresent; }

        /**
         * This method gets the Network Key in the Dataset.
         *
         * This method MUST be used when Network Key component is present in the Dataset, otherwise its behavior
         * is undefined.
         *
         * @returns The Network Key in the Dataset.
         *
         */
        const NetworkKey &GetNetworkKey(void) const { return static_cast<const NetworkKey &>(mNetworkKey); }

        /**
         * This method sets the Network Key in the Dataset.
         *
         * @param[in] aNetworkKey  A Network Key.
         *
         */
        void SetNetworkKey(const NetworkKey &aNetworkKey)
        {
            mNetworkKey                      = aNetworkKey;
            mComponents.mIsNetworkKeyPresent = true;
        }

        /**
         * This method returns a reference to the Network Key in the Dataset to be updated by caller.
         *
         * @returns A reference to the Network Key in the Dataset.
         *
         */
        NetworkKey &UpdateNetworkKey(void)
        {
            mComponents.mIsNetworkKeyPresent = true;
            return static_cast<NetworkKey &>(mNetworkKey);
        }

        /**
         * This method indicates whether or not the Network Name is present in the Dataset.
         *
         * @returns TRUE if Network Name is present, FALSE otherwise.
         *
         */
        bool IsNetworkNamePresent(void) const { return mComponents.mIsNetworkNamePresent; }

        /**
         * This method gets the Network Name in the Dataset.
         *
         * This method MUST be used when Network Name component is present in the Dataset, otherwise its behavior is
         * undefined.
         *
         * @returns The Network Name in the Dataset.
         *
         */
        const Mac::NetworkName &GetNetworkName(void) const
        {
            return static_cast<const Mac::NetworkName &>(mNetworkName);
        }

        /**
         * This method sets the Network Name in the Dataset.
         *
         * @param[in] aNetworkNameData   A Network Name Data.
         *
         */
        void SetNetworkName(const Mac::NameData &aNetworkNameData)
        {
            IgnoreError(static_cast<Mac::NetworkName &>(mNetworkName).Set(aNetworkNameData));
            mComponents.mIsNetworkNamePresent = true;
        }

        /**
         * This method indicates whether or not the Extended PAN ID is present in the Dataset.
         *
         * @returns TRUE if Extended PAN ID is present, FALSE otherwise.
         *
         */
        bool IsExtendedPanIdPresent(void) const { return mComponents.mIsExtendedPanIdPresent; }

        /**
         * This method gets the Extended PAN ID in the Dataset.
         *
         * This method MUST be used when Extended PAN ID component is present in the Dataset, otherwise its behavior is
         * undefined.
         *
         * @returns The Extended PAN ID in the Dataset.
         *
         */
        const Mac::ExtendedPanId &GetExtendedPanId(void) const
        {
            return static_cast<const Mac::ExtendedPanId &>(mExtendedPanId);
        }

        /**
         * This method sets the Extended PAN ID in the Dataset.
         *
         * @param[in] aExtendedPanId   An Extended PAN ID.
         *
         */
        void SetExtendedPanId(const Mac::ExtendedPanId &aExtendedPanId)
        {
            mExtendedPanId                      = aExtendedPanId;
            mComponents.mIsExtendedPanIdPresent = true;
        }

        /**
         * This method indicates whether or not the Mesh Local Prefix is present in the Dataset.
         *
         * @returns TRUE if Mesh Local Prefix is present, FALSE otherwise.
         *
         */
        bool IsMeshLocalPrefixPresent(void) const { return mComponents.mIsMeshLocalPrefixPresent; }

        /**
         * This method gets the Mesh Local Prefix in the Dataset.
         *
         * This method MUST be used when Mesh Local Prefix component is present in the Dataset, otherwise its behavior
         * is undefined.
         *
         * @returns The Mesh Local Prefix in the Dataset.
         *
         */
        const Mle::MeshLocalPrefix &GetMeshLocalPrefix(void) const
        {
            return static_cast<const Mle::MeshLocalPrefix &>(mMeshLocalPrefix);
        }

        /**
         * This method sets the Mesh Local Prefix in the Dataset.
         *
         * @param[in] aMeshLocalPrefix   A Mesh Local Prefix.
         *
         */
        void SetMeshLocalPrefix(const Mle::MeshLocalPrefix &aMeshLocalPrefix)
        {
            mMeshLocalPrefix                      = aMeshLocalPrefix;
            mComponents.mIsMeshLocalPrefixPresent = true;
        }

        /**
         * This method indicates whether or not the Delay Timer is present in the Dataset.
         *
         * @returns TRUE if Delay Timer is present, FALSE otherwise.
         *
         */
        bool IsDelayPresent(void) const { return mComponents.mIsDelayPresent; }

        /**
         * This method gets the Delay Timer in the Dataset.
         *
         * This method MUST be used when Delay Timer component is present in the Dataset, otherwise its behavior is
         * undefined.
         *
         * @returns The Delay Timer in the Dataset.
         *
         */
        uint32_t GetDelay(void) const { return mDelay; }

        /**
         * This method sets the Delay Timer in the Dataset.
         *
         * @param[in] aDely   A Delay value.
         *
         */
        void SetDelay(uint32_t aDelay)
        {
            mDelay                      = aDelay;
            mComponents.mIsDelayPresent = true;
        }

        /**
         * This method indicates whether or not the PAN ID is present in the Dataset.
         *
         * @returns TRUE if PAN ID is present, FALSE otherwise.
         *
         */
        bool IsPanIdPresent(void) const { return mComponents.mIsPanIdPresent; }

        /**
         * This method gets the PAN ID in the Dataset.
         *
         * This method MUST be used when PAN ID component is present in the Dataset, otherwise its behavior is
         * undefined.
         *
         * @returns The PAN ID in the Dataset.
         *
         */
        Mac::PanId GetPanId(void) const { return mPanId; }

        /**
         * This method sets the PAN ID in the Dataset.
         *
         * @param[in] aPanId  A PAN ID.
         *
         */
        void SetPanId(Mac::PanId aPanId)
        {
            mPanId                      = aPanId;
            mComponents.mIsPanIdPresent = true;
        }

        /**
         * This method indicates whether or not the Channel is present in the Dataset.
         *
         * @returns TRUE if Channel is present, FALSE otherwise.
         *
         */
        bool IsChannelPresent(void) const { return mComponents.mIsChannelPresent; }

        /**
         * This method gets the Channel in the Dataset.
         *
         * This method MUST be used when Channel component is present in the Dataset, otherwise its behavior is
         * undefined.
         *
         * @returns The Channel in the Dataset.
         *
         */
        uint16_t GetChannel(void) const { return mChannel; }

        /**
         * This method sets the Channel in the Dataset.
         *
         * @param[in] aChannel  A Channel.
         *
         */
        void SetChannel(uint16_t aChannel)
        {
            mChannel                      = aChannel;
            mComponents.mIsChannelPresent = true;
        }

        /**
         * This method indicates whether or not the PSKc is present in the Dataset.
         *
         * @returns TRUE if PSKc is present, FALSE otherwise.
         *
         */
        bool IsPskcPresent(void) const { return mComponents.mIsPskcPresent; }

        /**
         * This method gets the PSKc in the Dataset.
         *
         * This method MUST be used when PSKc component is present in the Dataset, otherwise its behavior is undefined.
         *
         * @returns The PSKc in the Dataset.
         *
         */
        const Pskc &GetPskc(void) const { return static_cast<const Pskc &>(mPskc); }

        /**
         * This method set the PSKc in the Dataset.
         *
         * @param[in] aPskc  A PSKc value.
         *
         */
        void SetPskc(const Pskc &aPskc)
        {
            mPskc                      = aPskc;
            mComponents.mIsPskcPresent = true;
        }

        /**
         * This method indicates whether or not the Security Policy is present in the Dataset.
         *
         * @returns TRUE if Security Policy is present, FALSE otherwise.
         *
         */
        bool IsSecurityPolicyPresent(void) const { return mComponents.mIsSecurityPolicyPresent; }

        /**
         * This method gets the Security Policy in the Dataset.
         *
         * This method MUST be used when Security Policy component is present in the Dataset, otherwise its behavior is
         * undefined.
         *
         * @returns The Security Policy in the Dataset.
         *
         */
        const SecurityPolicy &GetSecurityPolicy(void) const
        {
            return static_cast<const SecurityPolicy &>(mSecurityPolicy);
        }

        /**
         * This method sets the Security Policy in the Dataset.
         *
         * @param[in] aSecurityPolicy  A Security Policy to set in Dataset.
         *
         */
        void SetSecurityPolicy(const SecurityPolicy &aSecurityPolicy)
        {
            mSecurityPolicy                      = aSecurityPolicy;
            mComponents.mIsSecurityPolicyPresent = true;
        }

        /**
         * This method indicates whether or not the Channel Mask is present in the Dataset.
         *
         * @returns TRUE if Channel Mask is present, FALSE otherwise.
         *
         */
        bool IsChannelMaskPresent(void) const { return mComponents.mIsChannelMaskPresent; }

        /**
         * This method gets the Channel Mask in the Dataset.
         *
         * This method MUST be used when Channel Mask component is present in the Dataset, otherwise its behavior is
         * undefined.
         *
         * @returns The Channel Mask in the Dataset.
         *
         */
        otChannelMask GetChannelMask(void) const { return mChannelMask; }

        /**
         * This method sets the Channel Mask in the Dataset.
         *
         * @param[in] aChannelMask   A Channel Mask value.
         *
         */
        void SetChannelMask(otChannelMask aChannelMask)
        {
            mChannelMask                      = aChannelMask;
            mComponents.mIsChannelMaskPresent = true;
        }

        /**
         * This method populates the Dataset with random fields.
         *
         * The Network Key, PSKc, Mesh Local Prefix, PAN ID, and Extended PAN ID are generated randomly (crypto-secure)
         * with Network Name set to "OpenThread-%04x" with PAN ID appended as hex. The Channel is chosen randomly from
         * radio's preferred channel mask, Channel Mask is set from radio's supported mask, and Security Policy Flags
         * from current `KeyManager` value.
         *
         * @param[in] aInstance    The OpenThread instance.
         *
         * @retval kErrorNone If the Dataset was generated successfully.
         *
         */
        Error GenerateRandom(Instance &aInstance);

        /**
         * This method checks whether the Dataset is a subset of another one, i.e., all the components in the current
         * Dataset are also present in the @p aOther and the component values fully match.
         *
         * The matching of components in the two Datasets excludes Active/Pending Timestamp and Delay components.
         *
         * @param[in] aOther   The other Dataset to check against.
         *
         * @retval TRUE   The current dataset is a subset of @p aOther.
         * @retval FALSE  The current Dataset is not a subset of @p aOther.
         *
         */
        bool IsSubsetOf(const Info &aOther) const;
    };

    /**
     * This constructor initializes the object.
     *
     */
    Dataset(void);

    /**
     * This method clears the Dataset.
     *
     */
    void Clear(void);

    /**
     * This method indicates whether or not the dataset appears to be well-formed.
     *
     * @returns TRUE if the dataset appears to be well-formed, FALSE otherwise.
     *
     */
    bool IsValid(void) const;

    /**
     * This method returns a pointer to the TLV with a given type.
     *
     * @param[in] aType  A TLV type.
     *
     * @returns A pointer to the TLV or nullptr if none is found.
     *
     */
    Tlv *GetTlv(Tlv::Type aType) { return AsNonConst(AsConst(this)->GetTlv(aType)); }

    /**
     * This method returns a pointer to the TLV with a given type.
     *
     * @param[in] aType  The TLV type.
     *
     * @returns A pointer to the TLV or nullptr if none is found.
     *
     */
    const Tlv *GetTlv(Tlv::Type aType) const;

    /**
     * This template method returns a pointer to the TLV with a given template type `TlvType`
     *
     * @returns A pointer to the TLV or nullptr if none is found.
     *
     */
    template <typename TlvType> TlvType *GetTlv(void)
    {
        return static_cast<TlvType *>(GetTlv(static_cast<Tlv::Type>(TlvType::kType)));
    }

    /**
     * This template method returns a pointer to the TLV with a given template type `TlvType`
     *
     * @returns A pointer to the TLV or nullptr if none is found.
     *
     */
    template <typename TlvType> const TlvType *GetTlv(void) const
    {
        return static_cast<const TlvType *>(GetTlv(static_cast<Tlv::Type>(TlvType::kType)));
    }

    /**
     * This method returns a pointer to the byte representation of the Dataset.
     *
     * @returns A pointer to the byte representation of the Dataset.
     *
     */
    uint8_t *GetBytes(void) { return mTlvs; }

    /**
     * This method returns a pointer to the byte representation of the Dataset.
     *
     * @returns A pointer to the byte representation of the Dataset.
     *
     */
    const uint8_t *GetBytes(void) const { return mTlvs; }

    /**
     * This method converts the TLV representation to structure representation.
     *
     * @param[out] aDatasetInfo  A reference to `Info` object to output the Dataset.
     *
     */
    void ConvertTo(Info &aDatasetInfo) const;

    /**
     * This method converts the TLV representation to structure representation.
     *
     * @param[out] aDataset  A reference to `otOperationalDatasetTlvs` to output the Dataset.
     *
     */
    void ConvertTo(otOperationalDatasetTlvs &aDataset) const;

    /**
     * This method returns the Dataset size in bytes.
     *
     * @returns The Dataset size in bytes.
     *
     */
    uint16_t GetSize(void) const { return mLength; }

    /**
     * This method sets the Dataset size in bytes.
     *
     * @param[in] aSize  The Dataset size in bytes.
     *
     */
    void SetSize(uint16_t aSize) { mLength = aSize; }

    /**
     * This method returns the local time the dataset was last updated.
     *
     * @returns The local time the dataset was last updated.
     *
     */
    TimeMilli GetUpdateTime(void) const { return mUpdateTime; }

    /**
     * This method gets the Timestamp (Active or Pending).
     *
     * @param[in]  aType       The type: active or pending.
     * @param[out] aTimestamp  A reference to a `Timestamp` to output the value.
     *
     * @retval kErrorNone      Timestamp was read successfully. @p aTimestamp is updated.
     * @retval kErrorNotFound  Could not find the requested Timestamp TLV.
     *
     */
    Error GetTimestamp(Type aType, Timestamp &aTimestamp) const;

    /**
     * This method sets the Timestamp value.
     *
     * @param[in] aType        The type: active or pending.
     * @param[in] aTimestamp   A Timestamp.
     *
     */
    void SetTimestamp(Type aType, const Timestamp &aTimestamp);

    /**
     * This method sets a TLV in the Dataset.
     *
     * @param[in]  aTlv  A reference to the TLV.
     *
     * @retval kErrorNone    Successfully set the TLV.
     * @retval kErrorNoBufs  Could not set the TLV due to insufficient buffer space.
     *
     */
    Error SetTlv(const Tlv &aTlv);

    /**
     * This method sets a TLV with a given TLV Type and Value.
     *
     * @param[in] aType     The TLV Type.
     * @param[in] aValue    A pointer to TLV Value.
     * @param[in] aLength   The TLV Length in bytes (length of @p aValue).
     *
     * @retval kErrorNone    Successfully set the TLV.
     * @retval kErrorNoBufs  Could not set the TLV due to insufficient buffer space.
     *
     */
    Error SetTlv(Tlv::Type aType, const void *aValue, uint8_t aLength);

    /**
     * This template method sets a TLV with a given TLV Type and Value.
     *
     * @tparam ValueType    The type of TLV's Value.
     *
     * @param[in] aType     The TLV Type.
     * @param[in] aValue    The TLV Value (of type `ValueType`).
     *
     * @retval kErrorNone    Successfully set the TLV.
     * @retval kErrorNoBufs  Could not set the TLV due to insufficient buffer space.
     *
     */
    template <typename ValueType> Error SetTlv(Tlv::Type aType, const ValueType &aValue)
    {
        static_assert(!TypeTraits::IsPointer<ValueType>::kValue, "ValueType must not be a pointer");

        return SetTlv(aType, &aValue, sizeof(ValueType));
    }

    /**
     * This method sets the Dataset using TLVs stored in a message buffer.
     *
     * @param[in]  aMessage  The message buffer.
     * @param[in]  aOffset   The message buffer offset where the dataset starts.
     * @param[in]  aLength   The TLVs length in the message buffer in bytes.
     *
     * @retval kErrorNone         Successfully set the Dataset.
     * @retval kErrorInvalidArgs  The values of @p aOffset and @p aLength are not valid for @p aMessage.
     *
     */
    Error Set(const Message &aMessage, uint16_t aOffset, uint8_t aLength);

    /**
     * This method sets the Dataset using an existing Dataset.
     *
     * If this Dataset is an Active Dataset, any Pending Timestamp and Delay Timer TLVs will be omitted in the copy
     * from @p aDataset.
     *
     * @param[in]  aType     The type of the dataset, active or pending.
     * @param[in]  aDataset  The input Dataset.
     *
     */
    void Set(Type aType, const Dataset &aDataset);

    /**
     * This method sets the Dataset from a given structure representation.
     *
     * @param[in]  aDatasetInfo  The input Dataset as `Dataset::Info`.
     *
     * @retval kErrorNone         Successfully set the Dataset.
     * @retval kErrorInvalidArgs  Dataset is missing Active and/or Pending Timestamp.
     *
     */
    Error SetFrom(const Info &aDatasetInfo);

    /**
     * This method sets the Dataset using @p aDataset.
     *
     * @param[in]  aDataset  The input Dataset as otOperationalDatasetTlvs.
     *
     */
    void SetFrom(const otOperationalDatasetTlvs &aDataset);

    /**
     * This method removes a TLV from the Dataset.
     *
     * @param[in] aType The type of a specific TLV.
     *
     */
    void RemoveTlv(Tlv::Type aType);

    /**
     * This method appends the MLE Dataset TLV but excluding MeshCoP Sub Timestamp TLV.
     *
     * @param[in] aType          The type of the dataset, active or pending.
     * @param[in] aMessage       A message to append to.
     *
     * @retval kErrorNone    Successfully append MLE Dataset TLV without MeshCoP Sub Timestamp TLV.
     * @retval kErrorNoBufs  Insufficient available buffers to append the message with MLE Dataset TLV.
     *
     */
    Error AppendMleDatasetTlv(Type aType, Message &aMessage) const;

    /**
     * This method applies the Active or Pending Dataset to the Thread interface.
     *
     * @param[in]  aInstance            A reference to the OpenThread instance.
     * @param[out] aIsNetworkKeyUpdated A pointer to where to place whether network key was updated.
     *
     * @retval kErrorNone   Successfully applied configuration.
     * @retval kErrorParse  The dataset has at least one TLV with invalid format.
     *
     */
    Error ApplyConfiguration(Instance &aInstance, bool *aIsNetworkKeyUpdated = nullptr) const;

    /**
     * This method converts a Pending Dataset to an Active Dataset.
     *
     * This method removes the Delay Timer and Pending Timestamp TLVs
     *
     */
    void ConvertToActive(void);

    /**
     * This method returns a pointer to the start of Dataset TLVs sequence.
     *
     * @return  A pointer to the start of Dataset TLVs sequence.
     *
     */
    Tlv *GetTlvsStart(void) { return reinterpret_cast<Tlv *>(mTlvs); }

    /**
     * This method returns a pointer to the start of Dataset TLVs sequence.
     *
     * @return  A pointer to start of Dataset TLVs sequence.
     *
     */
    const Tlv *GetTlvsStart(void) const { return reinterpret_cast<const Tlv *>(mTlvs); }

    /**
     * This method returns a pointer to the past-the-end of Dataset TLVs sequence.
     *
     * Note that past-the-end points to the byte after the end of the last TLV in Dataset TLVs sequence.
     *
     * @return  A pointer to past-the-end of Dataset TLVs sequence.
     *
     */
    Tlv *GetTlvsEnd(void) { return reinterpret_cast<Tlv *>(mTlvs + mLength); }

    /**
     * This method returns a pointer to the past-the-end of Dataset TLVs sequence.
     *
     * Note that past-the-end points to the byte after the end of the last TLV in Dataset TLVs sequence.
     *
     * @return  A pointer to past-the-end of Dataset TLVs sequence.
     *
     */
    const Tlv *GetTlvsEnd(void) const { return reinterpret_cast<const Tlv *>(mTlvs + mLength); }

    /**
     * This static method converts a Dataset Type to a string.
     *
     * @param[in]  aType   A Dataset type.
     *
     */
    static const char *TypeToString(Type aType);

private:
    void RemoveTlv(Tlv *aTlv);

    uint8_t   mTlvs[kMaxSize]; ///< The Dataset buffer
    TimeMilli mUpdateTime;     ///< Local time last updated
    uint16_t  mLength;         ///< The number of valid bytes in @var mTlvs
};

/**
 * This is a template specialization of `SetTlv<ValueType>` with a `uint16_t` value type.
 *
 * @param[in] aType     The TLV Type.
 * @param[in] aValue    The TLV value (as `uint16_t`).
 *
 * @retval kErrorNone    Successfully set the TLV.
 * @retval kErrorNoBufs  Could not set the TLV due to insufficient buffer space.
 *
 */
template <> inline Error Dataset::SetTlv(Tlv::Type aType, const uint16_t &aValue)
{
    uint16_t value = Encoding::BigEndian::HostSwap16(aValue);

    return SetTlv(aType, &value, sizeof(uint16_t));
}

/**
 * This is a template specialization of `SetTlv<ValueType>` with a `uint32_t` value type
 *
 * @param[in] aType     The TLV Type.
 * @param[in] aValue    The TLV value (as `uint32_t`).
 *
 * @retval kErrorNone    Successfully set the TLV.
 * @retval kErrorNoBufs  Could not set the TLV due to insufficient buffer space.
 *
 */
template <> inline Error Dataset::SetTlv(Tlv::Type aType, const uint32_t &aValue)
{
    uint32_t value = Encoding::BigEndian::HostSwap32(aValue);

    return SetTlv(aType, &value, sizeof(uint32_t));
}

} // namespace MeshCoP
} // namespace ot

#endif // MESHCOP_DATASET_HPP_
