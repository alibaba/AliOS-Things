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
 *  This file includes definitions for the Joiner role.
 */

#ifndef JOINER_HPP_
#define JOINER_HPP_

#include "openthread-core-config.h"

#if OPENTHREAD_CONFIG_JOINER_ENABLE

#include <openthread/joiner.h>

#include "coap/coap.hpp"
#include "coap/coap_message.hpp"
#include "coap/coap_secure.hpp"
#include "common/locator.hpp"
#include "common/logging.hpp"
#include "common/message.hpp"
#include "common/non_copyable.hpp"
#include "mac/mac_types.hpp"
#include "meshcop/dtls.hpp"
#include "meshcop/meshcop.hpp"
#include "meshcop/meshcop_tlvs.hpp"
#include "thread/discover_scanner.hpp"

namespace ot {

namespace MeshCoP {

class Joiner : public InstanceLocator, private NonCopyable
{
public:
    /**
     * This enumeration type defines the Joiner State.
     *
     */
    enum State : uint8_t
    {
        kStateIdle      = OT_JOINER_STATE_IDLE,
        kStateDiscover  = OT_JOINER_STATE_DISCOVER,
        kStateConnect   = OT_JOINER_STATE_CONNECT,
        kStateConnected = OT_JOINER_STATE_CONNECTED,
        kStateEntrust   = OT_JOINER_STATE_ENTRUST,
        kStateJoined    = OT_JOINER_STATE_JOINED,
    };

    /**
     * This constructor initializes the Joiner object.
     *
     * @param[in]  aInstance     A reference to the OpenThread instance.
     *
     */
    explicit Joiner(Instance &aInstance);

    /**
     * This method starts the Joiner service.
     *
     * @param[in]  aPskd             A pointer to the PSKd.
     * @param[in]  aProvisioningUrl  A pointer to the Provisioning URL (may be nullptr).
     * @param[in]  aVendorName       A pointer to the Vendor Name (may be nullptr).
     * @param[in]  aVendorModel      A pointer to the Vendor Model (may be nullptr).
     * @param[in]  aVendorSwVersion  A pointer to the Vendor SW Version (may be nullptr).
     * @param[in]  aVendorData       A pointer to the Vendor Data (may be nullptr).
     * @param[in]  aCallback         A pointer to a function that is called when the join operation completes.
     * @param[in]  aContext          A pointer to application-specific context.
     *
     * @retval kErrorNone          Successfully started the Joiner service.
     * @retval kErrorBusy          The previous attempt is still on-going.
     * @retval kErrorInvalidState  The IPv6 stack is not enabled or Thread stack is fully enabled.
     *
     */
    Error Start(const char *     aPskd,
                const char *     aProvisioningUrl,
                const char *     aVendorName,
                const char *     aVendorModel,
                const char *     aVendorSwVersion,
                const char *     aVendorData,
                otJoinerCallback aCallback,
                void *           aContext);

    /**
     * This method stops the Joiner service.
     *
     */
    void Stop(void);

    /**
     * This method gets the Joiner State.
     *
     * @returns The Joiner state (see `State`).
     *
     */
    State GetState(void) const { return mState; }

    /**
     * This method retrieves the Joiner ID.
     *
     * @returns The Joiner ID.
     *
     */
    const Mac::ExtAddress &GetId(void) const { return mId; }

    /**
     * This method gets the Jointer Discerner.
     *
     * @returns A pointer to the current Joiner Discerner or `nullptr` if none is set.
     *
     */
    const JoinerDiscerner *GetDiscerner(void) const;

    /**
     * This method sets the Joiner Discerner.
     *
     * The Joiner Discerner is used to calculate the Joiner ID used during commissioning/joining process.
     *
     * By default (when a discerner is not provided or cleared), Joiner ID is derived as first 64 bits of the
     * result of computing SHA-256 over factory-assigned IEEE EUI-64. Note that this is the main behavior expected by
     * Thread specification.
     *
     * @param[in]   aDiscerner  A Joiner Discerner
     *
     * @retval kErrorNone          The Joiner Discerner updated successfully.
     * @retval kErrorInvalidArgs   @p aDiscerner is not valid (specified length is not within valid range).
     * @retval kErrorInvalidState  There is an ongoing Joining process so Joiner Discerner could not be changed.
     *
     */
    Error SetDiscerner(const JoinerDiscerner &aDiscerner);

    /**
     * This method clears any previously set Joiner Discerner.
     *
     * When cleared, Joiner ID is derived as first 64 bits of SHA-256 of factory-assigned IEEE EUI-64.
     *
     * @retval kErrorNone          The Joiner Discerner cleared and Joiner ID updated.
     * @retval kErrorInvalidState  There is an ongoing Joining process so Joiner Discerner could not be changed.
     *
     */
    Error ClearDiscerner(void);

private:
    static constexpr uint16_t kJoinerUdpPort = OPENTHREAD_CONFIG_JOINER_UDP_PORT;

    static constexpr uint32_t kConfigExtAddressDelay = 100;  // in msec.
    static constexpr uint32_t kReponseTimeout        = 4000; ///< Max wait time to receive response (in msec).

    struct JoinerRouter
    {
        Mac::ExtAddress mExtAddr;
        Mac::PanId      mPanId;
        uint16_t        mJoinerUdpPort;
        uint8_t         mChannel;
        uint8_t         mPriority;
    };

    static void HandleDiscoverResult(Mle::DiscoverScanner::ScanResult *aResult, void *aContext);
    void        HandleDiscoverResult(Mle::DiscoverScanner::ScanResult *aResult);

    static void HandleSecureCoapClientConnect(bool aConnected, void *aContext);
    void        HandleSecureCoapClientConnect(bool aConnected);

    static void HandleJoinerFinalizeResponse(void *               aContext,
                                             otMessage *          aMessage,
                                             const otMessageInfo *aMessageInfo,
                                             Error                aResult);
    void HandleJoinerFinalizeResponse(Coap::Message *aMessage, const Ip6::MessageInfo *aMessageInfo, Error aResult);

    static void HandleJoinerEntrust(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo);
    void        HandleJoinerEntrust(Coap::Message &aMessage, const Ip6::MessageInfo &aMessageInfo);

    static void HandleTimer(Timer &aTimer);
    void        HandleTimer(void);

    static const char *StateToString(State aState);

    void    SetState(State aState);
    void    SetIdFromIeeeEui64(void);
    void    SaveDiscoveredJoinerRouter(const Mle::DiscoverScanner::ScanResult &aResult);
    void    TryNextJoinerRouter(Error aPrevError);
    Error   Connect(JoinerRouter &aRouter);
    void    Finish(Error aError);
    uint8_t CalculatePriority(int8_t aRssi, bool aSteeringDataAllowsAny);

    Error PrepareJoinerFinalizeMessage(const char *aProvisioningUrl,
                                       const char *aVendorName,
                                       const char *aVendorModel,
                                       const char *aVendorSwVersion,
                                       const char *aVendorData);
    void  FreeJoinerFinalizeMessage(void);
    void  SendJoinerFinalize(void);
    void  SendJoinerEntrustResponse(const Coap::Message &aRequest, const Ip6::MessageInfo &aRequestInfo);

#if OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
    void LogCertMessage(const char *aText, const Coap::Message &aMessage) const;
#endif

    Mac::ExtAddress mId;
    JoinerDiscerner mDiscerner;

    State mState;

    otJoinerCallback mCallback;
    void *           mContext;

    JoinerRouter mJoinerRouters[OPENTHREAD_CONFIG_JOINER_MAX_CANDIDATES];
    uint16_t     mJoinerRouterIndex;

    Coap::Message *mFinalizeMessage;

    TimerMilli     mTimer;
    Coap::Resource mJoinerEntrust;
};

} // namespace MeshCoP
} // namespace ot

#endif // OPENTHREAD_CONFIG_JOINER_ENABLE

#endif // JOINER_HPP_
