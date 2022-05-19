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
 *   This file includes definitions for a MeshCoP Leader.
 */

#ifndef MESHCOP_LEADER_HPP_
#define MESHCOP_LEADER_HPP_

#include "openthread-core-config.h"

#if OPENTHREAD_FTD

#include "coap/coap.hpp"
#include "common/locator.hpp"
#include "common/non_copyable.hpp"
#include "common/timer.hpp"
#include "meshcop/meshcop_tlvs.hpp"
#include "net/udp6.hpp"
#include "thread/mle.hpp"

namespace ot {
namespace MeshCoP {

OT_TOOL_PACKED_BEGIN
class CommissioningData
{
public:
    uint8_t GetLength(void) const
    {
        return sizeof(Tlv) + mBorderAgentLocator.GetLength() + sizeof(Tlv) + mCommissionerSessionId.GetLength() +
               sizeof(Tlv) + mSteeringData.GetLength();
    }

    BorderAgentLocatorTlv    mBorderAgentLocator;
    CommissionerSessionIdTlv mCommissionerSessionId;
    SteeringDataTlv          mSteeringData;
} OT_TOOL_PACKED_END;

class Leader : public InstanceLocator, private NonCopyable
{
public:
    /**
     * This constructor initializes the Leader object.
     *
     * @param[in]  aInstance     A reference to the OpenThread instance.
     *
     */
    explicit Leader(Instance &aInstance);

    /**
     * This method sends a MGMT_DATASET_CHANGED message to commissioner.
     *
     * @param[in]  aAddress   The IPv6 address of destination.
     *
     */
    void SendDatasetChanged(const Ip6::Address &aAddress);

    /**
     * This method sets minimal delay timer.
     *
     * @param[in]  aDelayTimerMinimal The value of minimal delay timer (in ms).
     *
     * @retval  kErrorNone         Successfully set the minimal delay timer.
     * @retval  kErrorInvalidArgs  If @p aDelayTimerMinimal is not valid.
     *
     */
    Error SetDelayTimerMinimal(uint32_t aDelayTimerMinimal);

    /**
     * This method gets minimal delay timer.
     *
     * @retval the minimal delay timer (in ms).
     *
     */
    uint32_t GetDelayTimerMinimal(void) const;

    /**
     * This method sets empty Commissioner Data TLV in the Thread Network Data.
     *
     */
    void SetEmptyCommissionerData(void);

private:
    static constexpr uint32_t kTimeoutLeaderPetition = 50; // TIMEOUT_LEAD_PET (seconds)

    static void HandleTimer(Timer &aTimer);
    void        HandleTimer(void);

    static void HandlePetition(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo);
    void        HandlePetition(Coap::Message &aMessage, const Ip6::MessageInfo &aMessageInfo);
    void        SendPetitionResponse(const Coap::Message &   aRequest,
                                     const Ip6::MessageInfo &aMessageInfo,
                                     StateTlv::State         aState);

    static void HandleKeepAlive(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo);
    void        HandleKeepAlive(Coap::Message &aMessage, const Ip6::MessageInfo &aMessageInfo);
    void        SendKeepAliveResponse(const Coap::Message &   aRequest,
                                      const Ip6::MessageInfo &aMessageInfo,
                                      StateTlv::State         aState);

    static void HandleUdpReceive(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo);

    void ResignCommissioner(void);

    Coap::Resource mPetition;
    Coap::Resource mKeepAlive;
    TimerMilli     mTimer;

    uint32_t mDelayTimerMinimal;

    CommissionerIdTlv mCommissionerId;
    uint16_t          mSessionId;
};

} // namespace MeshCoP
} // namespace ot

#endif // OPENTHREAD_FTD

#endif // MESHCOP_LEADER_HPP_
