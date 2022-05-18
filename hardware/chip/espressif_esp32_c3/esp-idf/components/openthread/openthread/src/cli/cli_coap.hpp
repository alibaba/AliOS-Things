/*
 *  Copyright (c) 2017, The OpenThread Authors.
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
 *   This file contains definitions for a simple CLI CoAP server and client.
 */

#ifndef CLI_COAP_HPP_
#define CLI_COAP_HPP_

#include "openthread-core-config.h"

#if OPENTHREAD_CONFIG_COAP_API_ENABLE

#include <openthread/coap.h>

#include "cli/cli_output.hpp"
#include "utils/lookup_table.hpp"
#include "utils/parse_cmdline.hpp"

namespace ot {
namespace Cli {

/**
 * This class implements the CLI CoAP server and client.
 *
 */
class Coap : private OutputWrapper
{
public:
    typedef Utils::CmdLineParser::Arg Arg;

    /**
     * Constructor
     *
     * @param[in]  aOutput The CLI console output context
     *
     */
    explicit Coap(Output &aOutput);

    /**
     * This method interprets a list of CLI arguments.
     *
     * @param[in]  aArgs        An array of command line arguments.
     *
     */
    otError Process(Arg aArgs[]);

private:
    enum
    {
        kMaxUriLength  = 32,
        kMaxBufferSize = 16
    };

    struct Command
    {
        const char *mName;
        otError (Coap::*mHandler)(Arg aArgs[]);
    };

#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
    enum BlockType : uint8_t{
        kBlockType1,
        kBlockType2,
    };
#endif

#if OPENTHREAD_CONFIG_COAP_OBSERVE_API_ENABLE
    otError CancelResourceSubscription(void);
    void    CancelSubscriber(void);
#endif

    void PrintPayload(otMessage *aMessage);

    otError ProcessHelp(Arg aArgs[]);
#if OPENTHREAD_CONFIG_COAP_OBSERVE_API_ENABLE
    otError ProcessCancel(Arg aArgs[]);
#endif
    otError ProcessDelete(Arg aArgs[]);
    otError ProcessGet(Arg aArgs[]);
#if OPENTHREAD_CONFIG_COAP_OBSERVE_API_ENABLE
    otError ProcessObserve(Arg aArgs[]);
#endif
    otError ProcessParameters(Arg aArgs[]);
    otError ProcessPost(Arg aArgs[]);
    otError ProcessPut(Arg aArgs[]);
    otError ProcessResource(Arg aArgs[]);
    otError ProcessSet(Arg aArgs[]);
    otError ProcessStart(Arg aArgs[]);
    otError ProcessStop(Arg aArgs[]);

#if OPENTHREAD_CONFIG_COAP_OBSERVE_API_ENABLE
    otError ProcessRequest(Arg aArgs[], otCoapCode aCoapCode, bool aCoapObserve = false);
#else
    otError        ProcessRequest(Arg aArgs[], otCoapCode aCoapCode);
#endif

    static void HandleRequest(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo);
    void        HandleRequest(otMessage *aMessage, const otMessageInfo *aMessageInfo);

#if OPENTHREAD_CONFIG_COAP_OBSERVE_API_ENABLE
    static void HandleNotificationResponse(void *               aContext,
                                           otMessage *          aMessage,
                                           const otMessageInfo *aMessageInfo,
                                           otError              aError);
    void        HandleNotificationResponse(otMessage *aMessage, const otMessageInfo *aMessageInfo, otError aError);
#endif

    static void HandleResponse(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo, otError aError);
    void        HandleResponse(otMessage *aMessage, const otMessageInfo *aMessageInfo, otError aError);

#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE

    static otError BlockwiseReceiveHook(void *         aContext,
                                        const uint8_t *aBlock,
                                        uint32_t       aPosition,
                                        uint16_t       aBlockLength,
                                        bool           aMore,
                                        uint32_t       aTotalLength);
    otError        BlockwiseReceiveHook(const uint8_t *aBlock,
                                        uint32_t       aPosition,
                                        uint16_t       aBlockLength,
                                        bool           aMore,
                                        uint32_t       aTotalLength);
    static otError BlockwiseTransmitHook(void *    aContext,
                                         uint8_t * aBlock,
                                         uint32_t  aPosition,
                                         uint16_t *aBlockLength,
                                         bool *    aMore);
    otError        BlockwiseTransmitHook(uint8_t *aBlock, uint32_t aPosition, uint16_t *aBlockLength, bool *aMore);
#endif

    const otCoapTxParameters *GetRequestTxParameters(void) const
    {
        return mUseDefaultRequestTxParameters ? nullptr : &mRequestTxParameters;
    }

    const otCoapTxParameters *GetResponseTxParameters(void) const
    {
        return mUseDefaultResponseTxParameters ? nullptr : &mResponseTxParameters;
    }

    static constexpr Command sCommands[] = {
#if OPENTHREAD_CONFIG_COAP_OBSERVE_API_ENABLE
        {"cancel", &Coap::ProcessCancel},
#endif
        {"delete", &Coap::ProcessDelete},
        {"get", &Coap::ProcessGet},
        {"help", &Coap::ProcessHelp},
#if OPENTHREAD_CONFIG_COAP_OBSERVE_API_ENABLE
        {"observe", &Coap::ProcessObserve},
#endif
        {"parameters", &Coap::ProcessParameters},
        {"post", &Coap::ProcessPost},
        {"put", &Coap::ProcessPut},
        {"resource", &Coap::ProcessResource},
        {"set", &Coap::ProcessSet},
        {"start", &Coap::ProcessStart},
        {"stop", &Coap::ProcessStop},
    };

    static_assert(Utils::LookupTable::IsSorted(sCommands), "Command Table is not sorted");

    bool mUseDefaultRequestTxParameters;
    bool mUseDefaultResponseTxParameters;

    otCoapTxParameters mRequestTxParameters;
    otCoapTxParameters mResponseTxParameters;

#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
    otCoapBlockwiseResource mResource;
#else
    otCoapResource mResource;
#endif
#if OPENTHREAD_CONFIG_COAP_OBSERVE_API_ENABLE
    otIp6Address mRequestAddr;
    otSockAddr   mSubscriberSock;
    char         mRequestUri[kMaxUriLength];
    uint8_t      mRequestToken[OT_COAP_MAX_TOKEN_LENGTH];
    uint8_t      mSubscriberToken[OT_COAP_MAX_TOKEN_LENGTH];
#endif
    char mUriPath[kMaxUriLength];
    char mResourceContent[kMaxBufferSize];
#if OPENTHREAD_CONFIG_COAP_OBSERVE_API_ENABLE
    uint32_t mObserveSerial;
    uint8_t  mRequestTokenLength;
    uint8_t  mSubscriberTokenLength;
    bool     mSubscriberConfirmableNotifications;
#endif
#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
    uint32_t mBlockCount;
#endif
};

} // namespace Cli
} // namespace ot

#endif // OPENTHREAD_CONFIG_COAP_API_ENABLE

#endif // CLI_COAP_HPP_
