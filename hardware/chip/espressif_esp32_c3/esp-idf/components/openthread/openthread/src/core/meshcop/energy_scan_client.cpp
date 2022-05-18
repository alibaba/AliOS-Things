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
 *   This file implements the Energy Scan Client.
 */

#include "energy_scan_client.hpp"

#if OPENTHREAD_CONFIG_COMMISSIONER_ENABLE && OPENTHREAD_FTD

#include "coap/coap_message.hpp"
#include "common/code_utils.hpp"
#include "common/debug.hpp"
#include "common/encoding.hpp"
#include "common/instance.hpp"
#include "common/locator_getters.hpp"
#include "common/logging.hpp"
#include "meshcop/meshcop.hpp"
#include "meshcop/meshcop_tlvs.hpp"
#include "thread/thread_netif.hpp"
#include "thread/uri_paths.hpp"

namespace ot {

EnergyScanClient::EnergyScanClient(Instance &aInstance)
    : InstanceLocator(aInstance)
    , mCallback(nullptr)
    , mContext(nullptr)
    , mEnergyScan(UriPath::kEnergyReport, &EnergyScanClient::HandleReport, this)
{
    Get<Tmf::Agent>().AddResource(mEnergyScan);
}

Error EnergyScanClient::SendQuery(uint32_t                           aChannelMask,
                                  uint8_t                            aCount,
                                  uint16_t                           aPeriod,
                                  uint16_t                           aScanDuration,
                                  const Ip6::Address &               aAddress,
                                  otCommissionerEnergyReportCallback aCallback,
                                  void *                             aContext)
{
    Error                   error = kErrorNone;
    MeshCoP::ChannelMaskTlv channelMask;
    Ip6::MessageInfo        messageInfo;
    Coap::Message *         message = nullptr;

    VerifyOrExit(Get<MeshCoP::Commissioner>().IsActive(), error = kErrorInvalidState);
    VerifyOrExit((message = MeshCoP::NewMeshCoPMessage(Get<Tmf::Agent>())) != nullptr, error = kErrorNoBufs);

    SuccessOrExit(error = message->InitAsPost(aAddress, UriPath::kEnergyScan));
    SuccessOrExit(error = message->SetPayloadMarker());

    SuccessOrExit(
        error = Tlv::Append<MeshCoP::CommissionerSessionIdTlv>(*message, Get<MeshCoP::Commissioner>().GetSessionId()));

    channelMask.Init();
    channelMask.SetChannelMask(aChannelMask);
    SuccessOrExit(error = channelMask.AppendTo(*message));

    SuccessOrExit(error = Tlv::Append<MeshCoP::CountTlv>(*message, aCount));
    SuccessOrExit(error = Tlv::Append<MeshCoP::PeriodTlv>(*message, aPeriod));
    SuccessOrExit(error = Tlv::Append<MeshCoP::ScanDurationTlv>(*message, aScanDuration));

    messageInfo.SetSockAddr(Get<Mle::MleRouter>().GetMeshLocal16());
    messageInfo.SetPeerAddr(aAddress);
    messageInfo.SetPeerPort(Tmf::kUdpPort);
    SuccessOrExit(error = Get<Tmf::Agent>().SendMessage(*message, messageInfo));

    otLogInfoMeshCoP("sent energy scan query");

    mCallback = aCallback;
    mContext  = aContext;

exit:
    FreeMessageOnError(message, error);
    return error;
}

void EnergyScanClient::HandleReport(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo)
{
    static_cast<EnergyScanClient *>(aContext)->HandleReport(*static_cast<Coap::Message *>(aMessage),
                                                            *static_cast<const Ip6::MessageInfo *>(aMessageInfo));
}

void EnergyScanClient::HandleReport(Coap::Message &aMessage, const Ip6::MessageInfo &aMessageInfo)
{
    uint32_t mask;

    OT_TOOL_PACKED_BEGIN
    struct
    {
        MeshCoP::EnergyListTlv tlv;
        uint8_t                list[OPENTHREAD_CONFIG_TMF_ENERGY_SCAN_MAX_RESULTS];
    } OT_TOOL_PACKED_END energyList;

    VerifyOrExit(aMessage.IsConfirmablePostRequest());

    otLogInfoMeshCoP("received energy scan report");

    VerifyOrExit((mask = MeshCoP::ChannelMaskTlv::GetChannelMask(aMessage)) != 0);

    SuccessOrExit(MeshCoP::Tlv::FindTlv(aMessage, MeshCoP::Tlv::kEnergyList, sizeof(energyList), energyList.tlv));
    VerifyOrExit(energyList.tlv.IsValid());

    if (mCallback != nullptr)
    {
        mCallback(mask, energyList.list, energyList.tlv.GetLength(), mContext);
    }

    SuccessOrExit(Get<Tmf::Agent>().SendEmptyAck(aMessage, aMessageInfo));

    otLogInfoMeshCoP("sent energy scan report response");

exit:
    return;
}

} // namespace ot

#endif // OPENTHREAD_CONFIG_COMMISSIONER_ENABLE && OPENTHREAD_FTD
