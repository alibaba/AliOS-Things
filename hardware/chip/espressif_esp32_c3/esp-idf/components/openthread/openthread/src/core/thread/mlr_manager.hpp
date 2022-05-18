/*
 *  Copyright (c) 2020, The OpenThread Authors.
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
 *   This file includes definitions for managing Multicast Listener Registration feature defined in Thread 1.2.
 */

#ifndef MLR_MANAGER_HPP_
#define MLR_MANAGER_HPP_

#include "openthread-core-config.h"

#if OPENTHREAD_CONFIG_MLR_ENABLE || (OPENTHREAD_FTD && OPENTHREAD_CONFIG_TMF_PROXY_MLR_ENABLE)

#if OPENTHREAD_CONFIG_MLR_ENABLE && (OPENTHREAD_CONFIG_THREAD_VERSION < OT_THREAD_VERSION_1_2)
#error "Thread 1.2 or higher version is required for OPENTHREAD_CONFIG_MLR_ENABLE"
#endif

#include "backbone_router/bbr_leader.hpp"
#include "coap/coap_message.hpp"
#include "common/locator.hpp"
#include "common/non_copyable.hpp"
#include "common/notifier.hpp"
#include "common/time_ticker.hpp"
#include "common/timer.hpp"
#include "net/netif.hpp"
#include "thread/thread_tlvs.hpp"
#include "thread/topology.hpp"

namespace ot {

/**
 * @addtogroup core-mlr
 *
 * @brief
 *   This module includes definitions for Multicast Listener Registration.
 *
 * @{
 *
 * @defgroup core-mlr Mlr
 *
 * @}
 *
 */

/**
 * This class implements MLR management.
 *
 */
class MlrManager : public InstanceLocator, private NonCopyable
{
    friend class ot::Notifier;
    friend class ot::TimeTicker;

public:
    /**
     * This constructor initializes the object.
     *
     * @param[in]  aInstance     A reference to the OpenThread instance.
     *
     */
    explicit MlrManager(Instance &aInstance);

    /**
     * This method notifies Primary Backbone Router status.
     *
     * @param[in]  aState   The state or state change of Primary Backbone Router.
     * @param[in]  aConfig  The Primary Backbone Router service.
     *
     */
    void HandleBackboneRouterPrimaryUpdate(BackboneRouter::Leader::State               aState,
                                           const BackboneRouter::BackboneRouterConfig &aConfig);

#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_TMF_PROXY_MLR_ENABLE
    /**
     * This method updates the Multicast Subscription Table according to the Child information.
     *
     * @param[in]  aChild                       A reference to the child information.
     * @param[in]  aOldMlrRegisteredAddresses   Pointer to an array of the Child's previously registered IPv6 addresses.
     * @param[in]  aOldMlrRegisteredAddressNum  The number of previously registered IPv6 addresses.
     *
     */
    void UpdateProxiedSubscriptions(Child &             aChild,
                                    const Ip6::Address *aOldMlrRegisteredAddresses,
                                    uint16_t            aOldMlrRegisteredAddressNum);
#endif

#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_TMF_PROXY_MLR_ENABLE && OPENTHREAD_CONFIG_COMMISSIONER_ENABLE
    /**
     * This method registers Multicast Listeners to Primary Backbone Router.
     *
     * Note: only available when both `(OPENTHREAD_FTD && OPENTHREAD_CONFIG_TMF_PROXY_MLR_ENABLE)` and
     * `OPENTHREAD_CONFIG_COMMISSIONER_ENABLE` are enabled)
     *
     * @param aAddresses   A pointer to IPv6 multicast addresses to register.
     * @param aAddressNum  The number of IPv6 multicast addresses.
     * @param aTimeout     A pointer to the timeout (in seconds), or nullptr to use the default MLR timeout.
     *                     A timeout of 0 seconds removes the Multicast Listener addresses.
     * @param aCallback    A callback function.
     * @param aContext     A user context pointer.
     *
     * @retval kErrorNone          Successfully sent MLR.req. The @p aCallback will be called iff this method
     *                             returns kErrorNone.
     * @retval kErrorBusy          If a previous registration was ongoing.
     * @retval kErrorInvalidArgs   If one or more arguments are invalid.
     * @retval kErrorInvalidState  If the device was not in a valid state to send MLR.req (e.g. Commissioner not
     *                             started, Primary Backbone Router not found).
     * @retval kErrorNoBufs        If insufficient message buffers available.
     *
     */
    Error RegisterMulticastListeners(const otIp6Address *                    aAddresses,
                                     uint8_t                                 aAddressNum,
                                     const uint32_t *                        aTimeout,
                                     otIp6RegisterMulticastListenersCallback aCallback,
                                     void *                                  aContext);
#endif

private:
    void HandleNotifierEvents(Events aEvents);

    void  SendMulticastListenerRegistration(void);
    Error SendMulticastListenerRegistrationMessage(const otIp6Address *  aAddresses,
                                                   uint8_t               aAddressNum,
                                                   const uint32_t *      aTimeout,
                                                   Coap::ResponseHandler aResponseHandler,
                                                   void *                aResponseContext);

    static void  HandleMulticastListenerRegistrationResponse(void *               aContext,
                                                             otMessage *          aMessage,
                                                             const otMessageInfo *aMessageInfo,
                                                             Error                aResult);
    void         HandleMulticastListenerRegistrationResponse(Coap::Message *         aMessage,
                                                             const Ip6::MessageInfo *aMessageInfo,
                                                             Error                   aResult);
    static Error ParseMulticastListenerRegistrationResponse(Error          aResult,
                                                            Coap::Message *aMessage,
                                                            uint8_t &      aStatus,
                                                            Ip6::Address * aFailedAddresses,
                                                            uint8_t &      aFailedAddressNum);

#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_COMMISSIONER_ENABLE
    static void HandleRegisterMulticastListenersResponse(void *               aContext,
                                                         otMessage *          aMessage,
                                                         const otMessageInfo *aMessageInfo,
                                                         Error                aResult);
    void        HandleRegisterMulticastListenersResponse(otMessage *          aMessage,
                                                         const otMessageInfo *aMessageInfo,
                                                         Error                aResult);
#endif

#if OPENTHREAD_CONFIG_MLR_ENABLE
    void UpdateLocalSubscriptions(void);
    bool IsAddressMlrRegisteredByNetif(const Ip6::Address &aAddress) const;
#endif

#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_TMF_PROXY_MLR_ENABLE
    bool IsAddressMlrRegisteredByAnyChild(const Ip6::Address &aAddress) const
    {
        return IsAddressMlrRegisteredByAnyChildExcept(aAddress, nullptr);
    }
    bool IsAddressMlrRegisteredByAnyChildExcept(const Ip6::Address &aAddress, const Child *aExceptChild) const;
#endif

    void SetMulticastAddressMlrState(MlrState aFromState, MlrState aToState);
    void FinishMulticastListenerRegistration(bool                aSuccess,
                                             const Ip6::Address *aFailedAddresses,
                                             uint8_t             aFailedAddressNum);

    void        AppendToUniqueAddressList(Ip6::Address (&aAddresses)[kIp6AddressesNumMax],
                                          uint8_t &           aAddressNum,
                                          const Ip6::Address &aAddress);
    static bool AddressListContains(const Ip6::Address *aAddressList,
                                    uint8_t             aAddressListSize,
                                    const Ip6::Address &aAddress);

    void ScheduleSend(uint16_t aDelay);
    void UpdateTimeTickerRegistration(void);
    void UpdateReregistrationDelay(bool aRereg);
    void Reregister(void);
    void HandleTimeTick(void);

    void        LogMulticastAddresses(void);
    void        CheckInvariants(void) const;
    static void LogMlrResponse(Error               aResult,
                               Error               aError,
                               uint8_t             aStatus,
                               const Ip6::Address *aFailedAddresses,
                               uint8_t             aFailedAddressNum);

#if (OPENTHREAD_FTD && OPENTHREAD_CONFIG_TMF_PROXY_MLR_ENABLE) && OPENTHREAD_CONFIG_COMMISSIONER_ENABLE
    otIp6RegisterMulticastListenersCallback mRegisterMulticastListenersCallback;
    void *                                  mRegisterMulticastListenersContext;
#endif

    uint32_t mReregistrationDelay;
    uint16_t mSendDelay;

    bool mMlrPending : 1;
#if (OPENTHREAD_FTD && OPENTHREAD_CONFIG_TMF_PROXY_MLR_ENABLE) && OPENTHREAD_CONFIG_COMMISSIONER_ENABLE
    bool mRegisterMulticastListenersPending : 1;
#endif
};

} // namespace ot

#endif // OPENTHREAD_CONFIG_MLR_ENABLE || (OPENTHREAD_FTD && OPENTHREAD_CONFIG_TMF_PROXY_MLR_ENABLE)
#endif // MLR_MANAGER_HPP_
