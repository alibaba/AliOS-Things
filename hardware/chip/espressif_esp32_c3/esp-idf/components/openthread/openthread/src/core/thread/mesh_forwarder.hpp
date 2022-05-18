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
 *   This file includes definitions for forwarding IPv6 datagrams across the Thread mesh.
 */

#ifndef MESH_FORWARDER_HPP_
#define MESH_FORWARDER_HPP_

#include "openthread-core-config.h"

#include "common/clearable.hpp"
#include "common/locator.hpp"
#include "common/non_copyable.hpp"
#include "common/tasklet.hpp"
#include "common/time_ticker.hpp"
#include "mac/channel_mask.hpp"
#include "mac/data_poll_sender.hpp"
#include "mac/mac.hpp"
#include "mac/mac_frame.hpp"
#include "net/ip6.hpp"
#include "thread/address_resolver.hpp"
#include "thread/indirect_sender.hpp"
#include "thread/lowpan.hpp"
#include "thread/network_data_leader.hpp"
#include "thread/topology.hpp"

namespace ot {

namespace Mle {
class DiscoverScanner;
}

namespace Utils {
class HistoryTracker;
}

/**
 * @addtogroup core-mesh-forwarding
 *
 * @brief
 *   This module includes definitions for mesh forwarding within Thread.
 *
 * @{
 */

/**
 * This class represents link-specific information for messages received from the Thread radio.
 *
 */
class ThreadLinkInfo : public otThreadLinkInfo, public Clearable<ThreadLinkInfo>
{
public:
    /**
     * This method returns the IEEE 802.15.4 Source PAN ID.
     *
     * @returns The IEEE 802.15.4 Source PAN ID.
     *
     */
    Mac::PanId GetPanId(void) const { return mPanId; }

    /**
     * This method returns the IEEE 802.15.4 Channel.
     *
     * @returns The IEEE 802.15.4 Channel.
     *
     */
    uint8_t GetChannel(void) const { return mChannel; }

    /**
     * This method indicates whether or not link security is enabled.
     *
     * @retval TRUE   If link security is enabled.
     * @retval FALSE  If link security is not enabled.
     *
     */
    bool IsLinkSecurityEnabled(void) const { return mLinkSecurity; }

    /**
     * This method returns the Received Signal Strength (RSS) in dBm.
     *
     * @returns The Received Signal Strength (RSS) in dBm.
     *
     */
    int8_t GetRss(void) const { return mRss; }

    /**
     * This method returns the frame/radio Link Quality Indicator (LQI) value.
     *
     * @returns The Link Quality Indicator value.
     *
     */
    uint8_t GetLqi(void) const { return mLqi; }

#if OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
    /**
     * This method returns the Time Sync Sequence.
     *
     * @returns The Time Sync Sequence.
     *
     */
    uint8_t GetTimeSyncSeq(void) const { return mTimeSyncSeq; }

    /**
     * This method returns the time offset to the Thread network time (in microseconds).
     *
     * @returns The time offset to the Thread network time (in microseconds).
     *
     */
    int64_t GetNetworkTimeOffset(void) const { return mNetworkTimeOffset; }
#endif

    /**
     * This method sets the `ThreadLinkInfo` from a given received frame.
     *
     * @param[in] aFrame  A received frame.
     *
     */
    void SetFrom(const Mac::RxFrame &aFrame);
};

/**
 * This class implements mesh forwarding within Thread.
 *
 */
class MeshForwarder : public InstanceLocator, private NonCopyable
{
    friend class Mac::Mac;
    friend class Instance;
    friend class DataPollSender;
    friend class IndirectSender;
    friend class Mle::DiscoverScanner;
    friend class TimeTicker;
    friend class Utils::HistoryTracker;

public:
    /**
     * This constructor initializes the object.
     *
     * @param[in]  aInstance     A reference to the OpenThread instance.
     *
     */
    explicit MeshForwarder(Instance &aInstance);

    /**
     * This method enables mesh forwarding and the IEEE 802.15.4 MAC layer.
     *
     */
    void Start(void);

    /**
     * This method disables mesh forwarding and the IEEE 802.15.4 MAC layer.
     *
     */
    void Stop(void);

    /**
     * This method submits a message to the mesh forwarder for forwarding.
     *
     * @param[in]  aMessage  A reference to the message.
     *
     * @retval kErrorNone     Successfully enqueued the message.
     * @retval kErrorAlready  The message was already enqueued.
     * @retval kErrorDrop     The message could not be sent and should be dropped.
     *
     */
    Error SendMessage(Message &aMessage);

#if OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
    /**
     * This method sends an empty data frame to the parent.
     *
     * @retval kErrorNone          Successfully enqueued an empty message.
     * @retval kErrorInvalidState  Device is not in Rx-Off-When-Idle mode or it has no parent.
     * @retval kErrorNoBufs        Insufficient message buffers available.
     *
     */
    Error SendEmptyMessage(void);
#endif

    /**
     * This method is called by the address resolver when an EID-to-RLOC mapping has been resolved.
     *
     * @param[in]  aEid    A reference to the EID that has been resolved.
     * @param[in]  aError  kErrorNone on success and kErrorDrop otherwise.
     *
     */
    void HandleResolved(const Ip6::Address &aEid, Error aError);

    /**
     * This method indicates whether or not rx-on-when-idle mode is enabled.
     *
     * @retval TRUE   The rx-on-when-idle mode is enabled.
     * @retval FALSE  The rx-on-when-idle-mode is disabled.
     *
     */
    bool GetRxOnWhenIdle(void) const;

    /**
     * This method sets the rx-on-when-idle mode
     *
     * @param[in]  aRxOnWhenIdle  TRUE to enable, FALSE otherwise.
     *
     */
    void SetRxOnWhenIdle(bool aRxOnWhenIdle);

    /**
     * This method sets the scan parameters for MLE Discovery Request messages.
     *
     * @param[in]  aScanChannels  A reference to channel mask indicating which channels to scan.
     *                            If @p aScanChannels is empty, then all channels are used instead.
     *
     */
    void SetDiscoverParameters(const Mac::ChannelMask &aScanChannels);

#if OPENTHREAD_FTD
    /**
     * This method frees any messages queued for an existing child.
     *
     * @param[in]  aChild    A reference to the child.
     * @param[in]  aSubType  The message sub-type to remove.
     *                       Use Message::kSubTypeNone remove all messages for @p aChild.
     *
     */
    void RemoveMessages(Child &aChild, Message::SubType aSubType);
#endif

    /**
     * This method frees unicast/multicast MLE Data Responses from Send Message Queue if any.
     *
     */
    void RemoveDataResponseMessages(void);

    /**
     * This method evicts the message with lowest priority in the send queue.
     *
     * @param[in]  aPriority  The highest priority level of the evicted message.
     *
     * @retval kErrorNone       Successfully evicted a low priority message.
     * @retval kErrorNotFound   No low priority messages available to evict.
     *
     */
    Error EvictMessage(Message::Priority aPriority);

    /**
     * This method returns a reference to the send queue.
     *
     * @returns  A reference to the send queue.
     *
     */
    const PriorityQueue &GetSendQueue(void) const { return mSendQueue; }

    /**
     * This method returns a reference to the reassembly queue.
     *
     * @returns  A reference to the reassembly queue.
     *
     */
    const MessageQueue &GetReassemblyQueue(void) const { return mReassemblyList; }

    /**
     * This method returns a reference to the IP level counters.
     *
     * @returns A reference to the IP level counters.
     *
     */
    const otIpCounters &GetCounters(void) const { return mIpCounters; }

    /**
     * This method resets the IP level counters.
     *
     */
    void ResetCounters(void) { memset(&mIpCounters, 0, sizeof(mIpCounters)); }

#if OPENTHREAD_FTD
    /**
     * This method returns a reference to the resolving queue.
     *
     * @returns  A reference to the resolving queue.
     *
     */
    const PriorityQueue &GetResolvingQueue(void) const { return mResolvingQueue; }
#endif
#if OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE
    /**
     * This method handles a deferred ack.
     *
     * Some radio links can use deferred ack logic, where a tx request always report `HandleSentFrame()` quickly. The
     * link layer would wait for the ack and report it at a later time using this method.
     *
     * The link layer is expected to call `HandleDeferredAck()` (with success or failure status) for every tx request
     * on the radio link.
     *
     * @param[in] aNeighbor  The neighbor for which the deferred ack status is being reported.
     * @param[in] aError     The deferred ack error status: `kErrorNone` to indicate a deferred ack was received,
     *                       `kErrorNoAck` to indicate an ack timeout.
     *
     */
    void HandleDeferredAck(Neighbor &aNeighbor, Error aError);
#endif

private:
    static constexpr uint8_t kReassemblyTimeout      = OPENTHREAD_CONFIG_6LOWPAN_REASSEMBLY_TIMEOUT; // in seconds.
    static constexpr uint8_t kMeshHeaderFrameMtu     = OT_RADIO_FRAME_MAX_SIZE; // Max MTU with a Mesh Header frame.
    static constexpr uint8_t kMeshHeaderFrameFcsSize = sizeof(uint16_t);        // Frame FCS size for Mesh Header frame.

    enum MessageAction : uint8_t
    {
        kMessageReceive,         // Indicates that the message was received.
        kMessageTransmit,        // Indicates that the message was sent.
        kMessagePrepareIndirect, // Indicates that the message is being prepared for indirect tx.
        kMessageDrop,            // Indicates that the outbound message is being dropped (e.g., dst unknown).
        kMessageReassemblyDrop,  // Indicates that the message is being dropped from reassembly list.
        kMessageEvict,           // Indicates that the message was evicted.
    };

    enum AnycastType : uint8_t
    {
        kAnycastDhcp6Agent,
        kAnycastNeighborDiscoveryAgent,
        kAnycastService,
    };

#if OPENTHREAD_FTD
    class FragmentPriorityList : public Clearable<FragmentPriorityList>
    {
    public:
        class Entry : public Clearable<Entry>
        {
            friend class FragmentPriorityList;

        public:
            Message::Priority GetPriority(void) const { return mPriority; }
            bool              IsExpired(void) const { return (mLifetime == 0); }
            void              DecrementLifetime(void) { mLifetime--; }
            void              ResetLifetime(void) { mLifetime = kReassemblyTimeout; }

            bool Matches(uint16_t aSrcRloc16, uint16_t aTag) const
            {
                return (mSrcRloc16 == aSrcRloc16) && (mDatagramTag == aTag);
            }

        private:
            uint16_t          mSrcRloc16;
            uint16_t          mDatagramTag;
            Message::Priority mPriority;
            uint8_t           mLifetime;
        };

        Entry *AllocateEntry(uint16_t aSrcRloc16, uint16_t aTag, Message::Priority aPriority);
        Entry *FindEntry(uint16_t aSrcRloc16, uint16_t aTag);
        bool   UpdateOnTimeTick(void);

    private:
        static constexpr uint16_t kNumEntries = OPENTHREAD_CONFIG_NUM_FRAGMENT_PRIORITY_ENTRIES;

        Entry mEntries[kNumEntries];
    };
#endif // OPENTHREAD_FTD

    void  SendIcmpErrorIfDstUnreach(const Message &     aMessage,
                                    const Mac::Address &aMacSource,
                                    const Mac::Address &aMacDest);
    Error CheckReachability(const uint8_t *     aFrame,
                            uint16_t            aFrameLength,
                            const Mac::Address &aMeshSource,
                            const Mac::Address &aMeshDest);
    void  UpdateRoutes(const uint8_t *     aFrame,
                       uint16_t            aFrameLength,
                       const Mac::Address &aMeshSource,
                       const Mac::Address &aMeshDest);

    Error    DecompressIp6Header(const uint8_t *     aFrame,
                                 uint16_t            aFrameLength,
                                 const Mac::Address &aMacSource,
                                 const Mac::Address &aMacDest,
                                 Ip6::Header &       aIp6Header,
                                 uint8_t &           aHeaderLength,
                                 bool &              aNextHeaderCompressed);
    Error    FrameToMessage(const uint8_t *     aFrame,
                            uint16_t            aFrameLength,
                            uint16_t            aDatagramSize,
                            const Mac::Address &aMacSource,
                            const Mac::Address &aMacDest,
                            Message *&          aMessage);
    Error    GetIp6Header(const uint8_t *     aFrame,
                          uint16_t            aFrameLength,
                          const Mac::Address &aMacSource,
                          const Mac::Address &aMacDest,
                          Ip6::Header &       aIp6Header);
    void     GetMacDestinationAddress(const Ip6::Address &aIp6Addr, Mac::Address &aMacAddr);
    void     GetMacSourceAddress(const Ip6::Address &aIp6Addr, Mac::Address &aMacAddr);
    Message *GetDirectTransmission(void);
    void     HandleMesh(uint8_t *             aFrame,
                        uint16_t              aFrameLength,
                        const Mac::Address &  aMacSource,
                        const ThreadLinkInfo &aLinkInfo);
    void     HandleFragment(const uint8_t *       aFrame,
                            uint16_t              aFrameLength,
                            const Mac::Address &  aMacSource,
                            const Mac::Address &  aMacDest,
                            const ThreadLinkInfo &aLinkInfo);
    void     HandleLowpanHC(const uint8_t *       aFrame,
                            uint16_t              aFrameLength,
                            const Mac::Address &  aMacSource,
                            const Mac::Address &  aMacDest,
                            const ThreadLinkInfo &aLinkInfo);
    uint16_t PrepareDataFrame(Mac::TxFrame &      aFrame,
                              Message &           aMessage,
                              const Mac::Address &aMacSource,
                              const Mac::Address &aMacDest,
                              bool                aAddMeshHeader = false,
                              uint16_t            aMeshSource    = 0xffff,
                              uint16_t            aMeshDest      = 0xffff,
                              bool                aAddFragHeader = false);
    void     PrepareEmptyFrame(Mac::TxFrame &aFrame, const Mac::Address &aMacDest, bool aAckRequest);

    void  SendMesh(Message &aMessage, Mac::TxFrame &aFrame);
    void  SendDestinationUnreachable(uint16_t aMeshSource, const Message &aMessage);
    Error UpdateIp6Route(Message &aMessage);
    Error UpdateIp6RouteFtd(Ip6::Header &ip6Header, Message &aMessage);
    void  EvaluateRoutingCost(uint16_t aDest, uint8_t &aBestCost, uint16_t &aBestDest) const;
    Error AnycastRouteLookup(uint8_t aServiceId, AnycastType aType, uint16_t &aMeshDest) const;
    Error UpdateMeshRoute(Message &aMessage);
    bool  UpdateReassemblyList(void);
    void  UpdateFragmentPriority(Lowpan::FragmentHeader &aFragmentHeader,
                                 uint16_t                aFragmentLength,
                                 uint16_t                aSrcRloc16,
                                 Message::Priority       aPriority);
    Error HandleDatagram(Message &aMessage, const ThreadLinkInfo &aLinkInfo, const Mac::Address &aMacSource);
    void  ClearReassemblyList(void);
    void  RemoveMessage(Message &aMessage);
    void  HandleDiscoverComplete(void);

    void          HandleReceivedFrame(Mac::RxFrame &aFrame);
    Mac::TxFrame *HandleFrameRequest(Mac::TxFrames &aTxFrames);
    Neighbor *    UpdateNeighborOnSentFrame(Mac::TxFrame &aFrame, Error aError, const Mac::Address &aMacDest);
    void          UpdateNeighborLinkFailures(Neighbor &aNeighbor,
                                             Error     aError,
                                             bool      aAllowNeighborRemove,
                                             uint8_t   aFailLimit = Mle::kFailedRouterTransmissions);
    void          HandleSentFrame(Mac::TxFrame &aFrame, Error aError);
    void          UpdateSendMessage(Error aFrameTxError, Mac::Address &aMacDest, Neighbor *aNeighbor);
    void          RemoveMessageIfNoPendingTx(Message &aMessage);

    void        HandleTimeTick(void);
    static void ScheduleTransmissionTask(Tasklet &aTasklet);
    void        ScheduleTransmissionTask(void);

    Error GetFramePriority(const uint8_t *     aFrame,
                           uint16_t            aFrameLength,
                           const Mac::Address &aMacSource,
                           const Mac::Address &aMacDest,
                           Message::Priority & aPriority);
    Error GetFragmentPriority(Lowpan::FragmentHeader &aFragmentHeader,
                              uint16_t                aSrcRloc16,
                              Message::Priority &     aPriority);
    void  GetForwardFramePriority(const uint8_t *     aFrame,
                                  uint16_t            aFrameLength,
                                  const Mac::Address &aMeshSource,
                                  const Mac::Address &aMeshDest,
                                  Message::Priority & aPriority);

    bool     CalcIePresent(const Message *aMessage);
    uint16_t CalcFrameVersion(const Neighbor *aNeighbor, bool aIePresent);
#if OPENTHREAD_CONFIG_MAC_HEADER_IE_SUPPORT
    void AppendHeaderIe(const Message *aMessage, Mac::TxFrame &aFrame);
#endif

    void PauseMessageTransmissions(void) { mTxPaused = true; }
    void ResumeMessageTransmissions(void);

    void LogMessage(MessageAction aAction, const Message &aMessage, const Mac::Address *aAddress, Error aError);
    void LogFrame(const char *aActionText, const Mac::Frame &aFrame, Error aError);
    void LogFragmentFrameDrop(Error                         aError,
                              uint16_t                      aFrameLength,
                              const Mac::Address &          aMacSource,
                              const Mac::Address &          aMacDest,
                              const Lowpan::FragmentHeader &aFragmentHeader,
                              bool                          aIsSecure);
    void LogLowpanHcFrameDrop(Error               aError,
                              uint16_t            aFrameLength,
                              const Mac::Address &aMacSource,
                              const Mac::Address &aMacDest,
                              bool                aIsSecure);

    static Error ParseIp6UdpTcpHeader(const Message &aMessage,
                                      Ip6::Header &  aIp6Header,
                                      uint16_t &     aChecksum,
                                      uint16_t &     aSourcePort,
                                      uint16_t &     aDestPort);

#if (OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_NOTE) && (OPENTHREAD_CONFIG_LOG_MAC == 1)
    const char *MessageActionToString(MessageAction aAction, Error aError);
    const char *MessagePriorityToString(const Message &aMessage);

#if OPENTHREAD_FTD
    Error DecompressIp6UdpTcpHeader(const Message &     aMessage,
                                    uint16_t            aOffset,
                                    const Mac::Address &aMeshSource,
                                    const Mac::Address &aMeshDest,
                                    Ip6::Header &       aIp6Header,
                                    uint16_t &          aChecksum,
                                    uint16_t &          aSourcePort,
                                    uint16_t &          aDestPort);
    Error LogMeshFragmentHeader(MessageAction       aAction,
                                const Message &     aMessage,
                                const Mac::Address *aMacAddress,
                                Error               aError,
                                uint16_t &          aOffset,
                                Mac::Address &      aMeshSource,
                                Mac::Address &      aMeshDest,
                                otLogLevel          aLogLevel);
    void  LogMeshIpHeader(const Message &     aMessage,
                          uint16_t            aOffset,
                          const Mac::Address &aMeshSource,
                          const Mac::Address &aMeshDest,
                          otLogLevel          aLogLevel);
    void  LogMeshMessage(MessageAction       aAction,
                         const Message &     aMessage,
                         const Mac::Address *aAddress,
                         Error               aError,
                         otLogLevel          aLogLevel);
#endif
    void LogIp6SourceDestAddresses(Ip6::Header &aIp6Header,
                                   uint16_t     aSourcePort,
                                   uint16_t     aDestPort,
                                   otLogLevel   aLogLevel);
    void LogIp6Message(MessageAction       aAction,
                       const Message &     aMessage,
                       const Mac::Address *aAddress,
                       Error               aError,
                       otLogLevel          aLogLevel);
#endif // #if (OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_NOTE) && (OPENTHREAD_CONFIG_LOG_MAC == 1)

    PriorityQueue mSendQueue;
    MessageQueue  mReassemblyList;
    uint16_t      mFragTag;
    uint16_t      mMessageNextOffset;

    Message *mSendMessage;

    Mac::Address mMacSource;
    Mac::Address mMacDest;
    uint16_t     mMeshSource;
    uint16_t     mMeshDest;
    bool         mAddMeshHeader : 1;
    bool         mEnabled : 1;
    bool         mTxPaused : 1;
    bool         mSendBusy : 1;

    Tasklet mScheduleTransmissionTask;

    otIpCounters mIpCounters;

#if OPENTHREAD_FTD
    FragmentPriorityList mFragmentPriorityList;
    PriorityQueue        mResolvingQueue;
    IndirectSender       mIndirectSender;
#endif

    DataPollSender mDataPollSender;
};

/**
 * @}
 *
 */

} // namespace ot

#endif // MESH_FORWARDER_HPP_
