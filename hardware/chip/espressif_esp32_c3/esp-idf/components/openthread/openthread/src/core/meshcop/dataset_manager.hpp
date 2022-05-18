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

#ifndef MESHCOP_DATASET_MANAGER_HPP_
#define MESHCOP_DATASET_MANAGER_HPP_

#include "openthread-core-config.h"

#include "coap/coap.hpp"
#include "common/locator.hpp"
#include "common/non_copyable.hpp"
#include "common/timer.hpp"
#include "mac/channel_mask.hpp"
#include "meshcop/dataset.hpp"
#include "meshcop/dataset_local.hpp"
#include "net/udp6.hpp"

namespace ot {

namespace MeshCoP {

class DatasetManager : public InstanceLocator
{
public:
    /**
     * This method returns a pointer to the Timestamp.
     *
     * @returns A pointer to the Timestamp.
     *
     */
    const Timestamp *GetTimestamp(void) const;

    /**
     * This method restores the Operational Dataset from non-volatile memory.
     *
     * @retval kErrorNone      Successfully restore the dataset.
     * @retval kErrorNotFound  There is no corresponding dataset stored in non-volatile memory.
     *
     */
    Error Restore(void);

    /**
     * This method compares @p aTimestamp to the dataset's timestamp value.
     *
     * @param[in]  aCompare  A reference to the timestamp to compare.
     *
     * @retval -1  if @p aCompare is older than this dataset.
     * @retval  0  if @p aCompare is equal to this dataset.
     * @retval  1  if @p aCompare is newer than this dataset.
     *
     */
    int Compare(const Timestamp &aTimestamp) const;

    /**
     * This method retrieves the dataset from non-volatile memory.
     *
     * @param[out]  aDataset  Where to place the dataset.
     *
     * @retval kErrorNone      Successfully retrieved the dataset.
     * @retval kErrorNotFound  There is no corresponding dataset stored in non-volatile memory.
     *
     */
    Error Read(Dataset &aDataset) const { return mLocal.Read(aDataset); }

    /**
     * This method retrieves the dataset from non-volatile memory.
     *
     * @param[out]  aDatasetInfo  Where to place the dataset (as `Dataset::Info`).
     *
     * @retval kErrorNone      Successfully retrieved the dataset.
     * @retval kErrorNotFound  There is no corresponding dataset stored in non-volatile memory.
     *
     */
    Error Read(Dataset::Info &aDatasetInfo) const { return mLocal.Read(aDatasetInfo); }

    /**
     * This method retrieves the dataset from non-volatile memory.
     *
     * @param[out]  aDataset  Where to place the dataset.
     *
     * @retval kErrorNone      Successfully retrieved the dataset.
     * @retval kErrorNotFound  There is no corresponding dataset stored in non-volatile memory.
     *
     */
    Error Read(otOperationalDatasetTlvs &aDataset) const { return mLocal.Read(aDataset); }

    /**
     * This method retrieves the channel mask from local dataset.
     *
     * @param[out]  aChannelMask  A reference to the channel mask.
     *
     * @retval kErrorNone      Successfully retrieved the channel mask.
     * @retval kErrorNotFound  There is no valid channel mask stored in local dataset.
     *
     */
    Error GetChannelMask(Mac::ChannelMask &aChannelMask) const;

    /**
     * This method applies the Active or Pending Dataset to the Thread interface.
     *
     * @retval kErrorNone   Successfully applied configuration.
     * @retval kErrorParse  The dataset has at least one TLV with invalid format.
     *
     */
    Error ApplyConfiguration(void) const;

    /**
     * This method updates the Operational Dataset when detaching from the network.
     *
     * On detach, the Operational Dataset is restored from non-volatile memory.
     *
     */
    void HandleDetach(void);

    /**
     * This method sends a MGMT_SET request to the Leader.
     *
     * @param[in]  aDatasetInfo  The Operational Dataset.
     * @param[in]  aTlvs         Any additional raw TLVs to include.
     * @param[in]  aLength       Number of bytes in @p aTlvs.
     * @param[in]  aCallback     A pointer to a function that is called on response reception or timeout.
     * @param[in]  aContext      A pointer to application-specific context for @p aCallback.
     *
     * @retval kErrorNone    Successfully send the meshcop dataset command.
     * @retval kErrorNoBufs  Insufficient buffer space to send.
     * @retval kErrorBusy    A previous request is ongoing.
     *
     */
    Error SendSetRequest(const Dataset::Info &    aDatasetInfo,
                         const uint8_t *          aTlvs,
                         uint8_t                  aLength,
                         otDatasetMgmtSetCallback aCallback,
                         void *                   aContext);

    /**
     * This method sends a MGMT_GET request.
     *
     * @param[in]  aDatasetComponents  An Operational Dataset components structure specifying components to request.
     * @param[in]  aTlvTypes           A pointer to array containing additional raw TLV types to be requested.
     * @param[in]  aLength             Number of bytes in @p aTlvTypes.
     * @param[in]  aAddress            The IPv6 destination address for the MGMT_GET request.
     *
     * @retval kErrorNone     Successfully send the meshcop dataset command.
     * @retval kErrorNoBufs   Insufficient buffer space to send.
     *
     */
    Error SendGetRequest(const Dataset::Components &aDatasetComponents,
                         const uint8_t *            aTlvTypes,
                         uint8_t                    aLength,
                         const otIp6Address *       aAddress) const;
#if OPENTHREAD_FTD
    /**
     * This method appends the MLE Dataset TLV but excluding MeshCoP Sub Timestamp TLV.
     *
     * @param[in] aMessage       The message to append the TLV to.
     *
     * @retval kErrorNone    Successfully append MLE Dataset TLV without MeshCoP Sub Timestamp TLV.
     * @retval kErrorNoBufs  Insufficient available buffers to append the message with MLE Dataset TLV.
     *
     */
    Error AppendMleDatasetTlv(Message &aMessage) const;
#endif

protected:
    /**
     * This class defines a generic Dataset TLV to read from a message.
     *
     */
    OT_TOOL_PACKED_BEGIN
    class DatasetTlv : public Tlv
    {
    public:
        /**
         * This method reads the Dataset TLV from a given message at a given offset.
         *
         * @param[in]  aMessage  A message to read the TLV from.
         * @param[in]  aOffset   An offset into the message to read from.
         *
         * @retval kErrorNone    The TLV was read successfully.
         * @retval kErrorParse   The TLV was not well-formed and could not be parsed.
         *
         */
        Error ReadFromMessage(const Message &aMessage, uint16_t aOffset);

    private:
        uint8_t mValue[Dataset::kMaxValueSize];
    } OT_TOOL_PACKED_END;

    /**
     * This constructor initializes the object.
     *
     * @param[in]  aInstance      A reference to the OpenThread instance.
     * @param[in]  aType          Dataset type, Active or Pending.
     * @param[in]  aTimerHandler  The registration timer handler.
     *
     */
    DatasetManager(Instance &aInstance, Dataset::Type aType, TimerMilli::Handler aTimerHandler);

    /**
     * This method gets the Operational Dataset type (Active or Pending).
     *
     * @returns The Operational Dataset type.
     *
     */
    Dataset::Type GetType(void) const { return mLocal.GetType(); }

    /**
     * This method clears the Operational Dataset.
     *
     */
    void Clear(void);

    /**
     * This method saves the Operational Dataset in non-volatile memory.
     *
     * @param[in]  aDataset  The Operational Dataset.
     *
     * @retval kErrorNone   Successfully applied configuration.
     * @retval kErrorParse  The dataset has at least one TLV with invalid format.
     *
     */
    Error Save(const Dataset &aDataset);

    /**
     * This method saves the Operational Dataset in non-volatile memory.
     *
     * @param[in]  aDatasetInfo  The Operational Dataset as `Dataset::Info`.
     *
     * @retval kErrorNone             Successfully saved the dataset.
     * @retval kErrorNotImplemented   The platform does not implement settings functionality.
     *
     */
    Error Save(const Dataset::Info &aDatasetInfo);

    /**
     * This method saves the Operational Dataset in non-volatile memory.
     *
     * @param[in]  aDataset  The Operational Dataset.
     *
     * @retval kErrorNone             Successfully saved the dataset.
     * @retval kErrorNotImplemented   The platform does not implement settings functionality.
     *
     */
    Error Save(const otOperationalDatasetTlvs &aDataset);

    /**
     * This method sets the Operational Dataset for the partition.
     *
     * This method also updates the non-volatile version if the partition's Operational Dataset is newer.
     *
     * @param[in]  aTimestamp  The timestamp for the Operational Dataset.
     * @param[in]  aMessage    The message buffer.
     * @param[in]  aOffset     The offset where the Operational Dataset begins.
     * @param[in]  aLength     The length of the Operational Dataset.
     *
     */
    Error Save(const Timestamp &aTimestamp, const Message &aMessage, uint16_t aOffset, uint8_t aLength);

    /**
     * This method saves the Operational Dataset in non-volatile memory.
     *
     * @param[in]  aDataset  The Operational Dataset.
     *
     * @retval kErrorNone   Successfully applied configuration.
     * @retval kErrorParse  The dataset has at least one TLV with invalid format.
     *
     */
    Error SaveLocal(const Dataset &aDataset);

    /**
     * This method handles a MGMT_GET request message.
     *
     * @param[in]  aMessage      The CoAP message buffer.
     * @param[in]  aMessageInfo  The message info.
     *
     */
    void HandleGet(const Coap::Message &aMessage, const Ip6::MessageInfo &aMessageInfo) const;

    /**
     * This method compares the partition's Operational Dataset with that stored in non-volatile memory.
     *
     * If the partition's Operational Dataset is newer, the non-volatile storage is updated.
     * If the partition's Operational Dataset is older, the registration process is started.
     *
     */
    void HandleNetworkUpdate(void);

    /**
     * This method initiates a network data registration message with the Leader.
     *
     */
    void HandleTimer(void);

#if OPENTHREAD_FTD
    /**
     * This method handles the MGMT_SET request message.
     *
     * @param[in]  aMessage      The CoAP message buffer.
     * @param[in]  aMessageInfo  The message info.
     *
     * @retval kErrorNone  The MGMT_SET request message was handled successfully.
     * @retval kErrorDrop  The MGMT_SET request message was dropped.
     *
     */
    Error HandleSet(Coap::Message &aMessage, const Ip6::MessageInfo &aMessageInfo);
#endif

    DatasetLocal mLocal;
    Timestamp    mTimestamp;
    bool         mTimestampValid : 1;

private:
    static void HandleMgmtSetResponse(void *               aContext,
                                      otMessage *          aMessage,
                                      const otMessageInfo *aMessageInfo,
                                      Error                aError);
    void        HandleMgmtSetResponse(Coap::Message *aMessage, const Ip6::MessageInfo *aMessageInfo, Error aError);

    bool  IsActiveDataset(void) const { return GetType() == Dataset::kActive; }
    bool  IsPendingDataset(void) const { return GetType() == Dataset::kPending; }
    void  SignalDatasetChange(void) const;
    void  HandleDatasetUpdated(void);
    Error AppendDatasetToMessage(const Dataset::Info &aDatasetInfo, Message &aMessage) const;
    void  SendSet(void);
    void  SendGetResponse(const Coap::Message &   aRequest,
                          const Ip6::MessageInfo &aMessageInfo,
                          uint8_t *               aTlvs,
                          uint8_t                 aLength) const;

#if OPENTHREAD_FTD
    void SendSetResponse(const Coap::Message &aRequest, const Ip6::MessageInfo &aMessageInfo, StateTlv::State aState);
#endif

    static constexpr uint8_t  kMaxDatasetTlvs = 16;   // Maximum number of TLVs in a Dataset.
    static constexpr uint32_t kSendSetDelay   = 5000; // Milliseconds

    bool       mMgmtPending : 1;
    TimerMilli mTimer;

    otDatasetMgmtSetCallback mMgmtSetCallback;
    void *                   mMgmtSetCallbackContext;
};

class ActiveDataset : public DatasetManager, private NonCopyable
{
public:
    /**
     * This constructor initializes the ActiveDataset object.
     *
     * @param[in]  aInstance  A reference to the OpenThread instance.
     *
     */
    explicit ActiveDataset(Instance &aInstance);

    /**
     * This method indicates whether the Active Dataset is partially complete.
     *
     * This method is primarily used to determine whether a user has supplied a partial Active Dataset for use
     * with joining a network.
     *
     * @retval TRUE   if an Active Dataset is saved but does not include an Active Timestamp.
     * @retval FALSE  if an Active Dataset is not saved or does include an Active Timestamp.
     *
     */
    bool IsPartiallyComplete(void) const;

    /**
     * This method indicates whether or not a valid network is present in the Active Operational Dataset.
     *
     * @retval TRUE if a valid network is present in the Active Dataset.
     * @retval FALSE if a valid network is not present in the Active Dataset.
     *
     */
    bool IsCommissioned(void) const;

    /**
     * This method clears the Active Operational Dataset.
     *
     */
    void Clear(void) { DatasetManager::Clear(); }

    /**
     * This method saves the Operational Dataset in non-volatile memory.
     *
     * This method also reconfigures the Thread interface.
     *
     * @param[in]  aDataset  The Operational Dataset.
     *
     */
    void Save(const Dataset &aDataset) { IgnoreError(DatasetManager::Save(aDataset)); }

    /**
     * This method sets the Operational Dataset for the partition.
     *
     * This method also reconfigures the Thread interface.
     * This method also updates the non-volatile version if the partition's Operational Dataset is newer.
     *
     * @param[in]  aTimestamp  The timestamp for the Operational Dataset.
     * @param[in]  aMessage    The message buffer.
     * @param[in]  aOffset     The offset where the Operational Dataset begins.
     * @param[in]  aLength     The length of the Operational Dataset.
     *
     */
    Error Save(const Timestamp &aTimestamp, const Message &aMessage, uint16_t aOffset, uint8_t aLength);

    /**
     * This method sets the Operational Dataset in non-volatile memory.
     *
     * @param[in]  aDatasetInfo  The Operational Dataset as `Dataset::Info`.
     *
     * @retval kErrorNone            Successfully saved the dataset.
     * @retval kErrorNotImplemented  The platform does not implement settings functionality.
     *
     */
    Error Save(const Dataset::Info &aDatasetInfo) { return DatasetManager::Save(aDatasetInfo); }

    /**
     * This method sets the Operational Dataset in non-volatile memory.
     *
     * @param[in]  aDataset  The Operational Dataset.
     *
     * @retval kErrorNone            Successfully saved the dataset.
     * @retval kErrorNotImplemented  The platform does not implement settings functionality.
     *
     */
    Error Save(const otOperationalDatasetTlvs &aDataset) { return DatasetManager::Save(aDataset); }

#if OPENTHREAD_FTD

    /**
     * This method creates a new Operational Dataset to use when forming a new network.
     *
     * @param[out]  aDatasetInfo  The Operational Dataset as `Dataset::Info`.
     *
     * @retval kErrorNone    Successfully created a new Operational Dataset.
     * @retval kErrorFailed  Failed to generate random values for new parameters.
     *
     */
    Error CreateNewNetwork(Dataset::Info &aDatasetInfo) { return aDatasetInfo.GenerateRandom(GetInstance()); }

    /**
     * This method starts the Leader functions for maintaining the Active Operational Dataset.
     *
     */
    void StartLeader(void);

    /**
     * This method stops the Leader functions for maintaining the Active Operational Dataset.
     *
     */
    void StopLeader(void);

    /**
     * This method generate a default Active Operational Dataset.
     *
     * @retval kErrorNone          Successfully generated an Active Operational Dataset.
     * @retval kErrorAlready       A valid Active Operational Dataset already exists.
     * @retval kErrorInvalidState  Device is not currently attached to a network.
     *
     */
    Error GenerateLocal(void);
#endif

private:
    static void HandleTimer(Timer &aTimer);
    void        HandleTimer(void) { DatasetManager::HandleTimer(); }

    static void HandleGet(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo);
    void        HandleGet(Coap::Message &aMessage, const Ip6::MessageInfo &aMessageInfo) const;

#if OPENTHREAD_FTD
    static void HandleSet(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo);
    void        HandleSet(Coap::Message &aMessage, const Ip6::MessageInfo &aMessageInfo);
#endif

    Coap::Resource mResourceGet;

#if OPENTHREAD_FTD
    Coap::Resource mResourceSet;
#endif
};

class PendingDataset : public DatasetManager, private NonCopyable
{
public:
    /**
     * This constructor initializes the PendingDataset object.
     *
     * @param[in]  aInstance     A reference to the OpenThread instance.
     *
     */
    explicit PendingDataset(Instance &aInstance);

    /**
     * This method clears the Pending Operational Dataset.
     *
     * This method also stops the Delay Timer if it was active.
     *
     */
    void Clear(void);

    /**
     * This method clears the network Pending Operational Dataset.
     *
     * This method also stops the Delay Timer if it was active.
     *
     */
    void ClearNetwork(void);

    /**
     * This method saves the Operational Dataset in non-volatile memory.
     *
     * This method also starts the Delay Timer.
     *
     * @param[in]  aDatasetInfo  The Operational Dataset as `Dataset::Info`.
     *
     * @retval kErrorNone            Successfully saved the dataset.
     * @retval kErrorNotImplemented  The platform does not implement settings functionality.
     *
     */
    Error Save(const Dataset::Info &aDatasetInfo);

    /**
     * This method saves the Operational Dataset in non-volatile memory.
     *
     * This method also starts the Delay Timer.
     *
     * @param[in]  aDataset  The Operational Dataset.
     *
     * @retval kErrorNone            Successfully saved the dataset.
     * @retval kErrorNotImplemented  The platform does not implement settings functionality.
     *
     */
    Error Save(const otOperationalDatasetTlvs &aDataset);

    /**
     * This method sets the Operational Dataset for the partition.
     *
     * This method also updates the non-volatile version if the partition's Operational Dataset is newer.
     *
     * This method also starts the Delay Timer.
     *
     * @param[in]  aTimestamp  The timestamp for the Operational Dataset.
     * @param[in]  aMessage    The message buffer.
     * @param[in]  aOffset     The offset where the Operational Dataset begins.
     * @param[in]  aLength     The length of the Operational Dataset.
     *
     */
    Error Save(const Timestamp &aTimestamp, const Message &aMessage, uint16_t aOffset, uint8_t aLength);

    /**
     * This method saves the Operational Dataset in non-volatile memory.
     *
     * @param[in]  aDataset  The Operational Dataset.
     *
     * @retval kErrorNone   Successfully applied configuration.
     * @retval kErrorParse  The dataset has at least one TLV with invalid format.
     *
     */
    Error Save(const Dataset &aDataset);

#if OPENTHREAD_FTD
    /**
     * This method starts the Leader functions for maintaining the Active Operational Dataset.
     *
     */
    void StartLeader(void);

    /**
     * This method stops the Leader functions for maintaining the Active Operational Dataset.
     *
     */
    void StopLeader(void);

    /**
     * This method generates a Pending Dataset from an Active Dataset.
     *
     * @param[in]  aTimestamp  The Active Dataset Timestamp.
     * @param[in]  aMessage    The MGMT_SET message that contains an Active Dataset.
     *
     */
    void ApplyActiveDataset(const Timestamp &aTimestamp, Coap::Message &aMessage);
#endif

private:
    void StartDelayTimer(void);

    static void HandleTimer(Timer &aTimer);
    void        HandleTimer(void) { DatasetManager::HandleTimer(); }

    static void HandleDelayTimer(Timer &aTimer);
    void        HandleDelayTimer(void);

    static void HandleGet(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo);
    void        HandleGet(Coap::Message &aMessage, const Ip6::MessageInfo &aMessageInfo) const;

#if OPENTHREAD_FTD
    static void HandleSet(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo);
    void        HandleSet(Coap::Message &aMessage, const Ip6::MessageInfo &aMessageInfo);
#endif

    TimerMilli mDelayTimer;

    Coap::Resource mResourceGet;

#if OPENTHREAD_FTD
    Coap::Resource mResourceSet;
#endif
};

} // namespace MeshCoP
} // namespace ot

#endif // MESHCOP_DATASET_MANAGER_HPP_
