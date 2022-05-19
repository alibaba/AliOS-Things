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
 *   This file implements the Energy Scan Server.
 */

#include "energy_scan_server.hpp"

#include "coap/coap_message.hpp"
#include "common/code_utils.hpp"
#include "common/debug.hpp"
#include "common/instance.hpp"
#include "common/locator_getters.hpp"
#include "common/logging.hpp"
#include "meshcop/meshcop.hpp"
#include "meshcop/meshcop_tlvs.hpp"
#include "thread/thread_netif.hpp"
#include "thread/uri_paths.hpp"

namespace ot {

EnergyScanServer::EnergyScanServer(Instance &aInstance)
    : InstanceLocator(aInstance)
    , mChannelMask(0)
    , mChannelMaskCurrent(0)
    , mPeriod(0)
    , mScanDuration(0)
    , mCount(0)
    , mActive(false)
    , mScanResultsLength(0)
    , mTimer(aInstance, EnergyScanServer::HandleTimer)
    , mEnergyScan(UriPath::kEnergyScan, &EnergyScanServer::HandleRequest, this)
{
    Get<Tmf::Agent>().AddResource(mEnergyScan);
}

void EnergyScanServer::HandleRequest(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo)
{
    static_cast<EnergyScanServer *>(aContext)->HandleRequest(*static_cast<Coap::Message *>(aMessage),
                                                             *static_cast<const Ip6::MessageInfo *>(aMessageInfo));
}

void EnergyScanServer::HandleRequest(Coap::Message &aMessage, const Ip6::MessageInfo &aMessageInfo)
{
    uint8_t          count;
    uint16_t         period;
    uint16_t         scanDuration;
    Ip6::MessageInfo responseInfo(aMessageInfo);
    uint32_t         mask;

    VerifyOrExit(aMessage.IsPostRequest());

    SuccessOrExit(Tlv::Find<MeshCoP::CountTlv>(aMessage, count));
    SuccessOrExit(Tlv::Find<MeshCoP::PeriodTlv>(aMessage, period));
    SuccessOrExit(Tlv::Find<MeshCoP::ScanDurationTlv>(aMessage, scanDuration));

    VerifyOrExit((mask = MeshCoP::ChannelMaskTlv::GetChannelMask(aMessage)) != 0);

    mChannelMask        = mask;
    mChannelMaskCurrent = mChannelMask;
    mCount              = count;
    mPeriod             = period;
    mScanDuration       = scanDuration;
    mScanResultsLength  = 0;
    mActive             = true;
    mTimer.Start(kScanDelay);

    mCommissioner = aMessageInfo.GetPeerAddr();

    if (aMessage.IsConfirmable() && !aMessageInfo.GetSockAddr().IsMulticast())
    {
        SuccessOrExit(Get<Tmf::Agent>().SendEmptyAck(aMessage, responseInfo));
        otLogInfoMeshCoP("sent energy scan query response");
    }

exit:
    return;
}

void EnergyScanServer::HandleTimer(Timer &aTimer)
{
    aTimer.Get<EnergyScanServer>().HandleTimer();
}

void EnergyScanServer::HandleTimer(void)
{
    VerifyOrExit(mActive);

    if (mCount)
    {
        // grab the lowest channel to scan
        uint32_t channelMask = mChannelMaskCurrent & ~(mChannelMaskCurrent - 1);
        IgnoreError(Get<Mac::Mac>().EnergyScan(channelMask, mScanDuration, HandleScanResult, this));
    }
    else
    {
        SendReport();
    }

exit:
    return;
}

void EnergyScanServer::HandleScanResult(Mac::EnergyScanResult *aResult, void *aContext)
{
    static_cast<EnergyScanServer *>(aContext)->HandleScanResult(aResult);
}

void EnergyScanServer::HandleScanResult(Mac::EnergyScanResult *aResult)
{
    VerifyOrExit(mActive);

    if (aResult)
    {
        VerifyOrExit(mScanResultsLength < OPENTHREAD_CONFIG_TMF_ENERGY_SCAN_MAX_RESULTS);
        mScanResults[mScanResultsLength++] = aResult->mMaxRssi;
    }
    else
    {
        // clear the lowest channel to scan
        mChannelMaskCurrent &= mChannelMaskCurrent - 1;

        if (mChannelMaskCurrent == 0)
        {
            mChannelMaskCurrent = mChannelMask;
            mCount--;
        }

        if (mCount)
        {
            mTimer.Start(mPeriod);
        }
        else
        {
            mTimer.Start(kReportDelay);
        }
    }

exit:
    return;
}

void EnergyScanServer::SendReport(void)
{
    Error                   error = kErrorNone;
    MeshCoP::ChannelMaskTlv channelMask;
    MeshCoP::EnergyListTlv  energyList;
    Ip6::MessageInfo        messageInfo;
    Coap::Message *         message;

    VerifyOrExit((message = MeshCoP::NewMeshCoPMessage(Get<Tmf::Agent>())) != nullptr, error = kErrorNoBufs);

    SuccessOrExit(error = message->InitAsConfirmablePost(UriPath::kEnergyReport));
    SuccessOrExit(error = message->SetPayloadMarker());

    channelMask.Init();
    channelMask.SetChannelMask(mChannelMask);
    SuccessOrExit(error = channelMask.AppendTo(*message));

    energyList.Init();
    energyList.SetLength(mScanResultsLength);
    SuccessOrExit(error = message->Append(energyList));
    SuccessOrExit(error = message->AppendBytes(mScanResults, mScanResultsLength));

    messageInfo.SetSockAddr(Get<Mle::MleRouter>().GetMeshLocal16());
    messageInfo.SetPeerAddr(mCommissioner);
    messageInfo.SetPeerPort(Tmf::kUdpPort);
    SuccessOrExit(error = Get<Tmf::Agent>().SendMessage(*message, messageInfo));

    otLogInfoMeshCoP("sent scan results");

exit:
    FreeMessageOnError(message, error);
    MeshCoP::LogError("send scan results", error);
    mActive = false;
}

void EnergyScanServer::HandleNotifierEvents(Events aEvents)
{
    if (aEvents.Contains(kEventThreadNetdataChanged) && !mActive &&
        Get<NetworkData::Leader>().GetCommissioningData() == nullptr)
    {
        mActive = false;
        mTimer.Stop();
    }
}

} // namespace ot
