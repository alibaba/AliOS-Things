/*
 *  Copyright (c) 2018, The OpenThread Authors.
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
 *   This file contains definitions for a simple CLI CoAP Secure server and client.
 */

#ifndef CLI_COAP_SECURE_HPP_
#define CLI_COAP_SECURE_HPP_

#include "openthread-core-config.h"

#if OPENTHREAD_CONFIG_COAP_SECURE_API_ENABLE

#include <mbedtls/ssl.h>

#include <openthread/coap_secure.h>

#include "cli/cli_output.hpp"
#include "utils/lookup_table.hpp"
#include "utils/parse_cmdline.hpp"

#ifndef CLI_COAP_SECURE_USE_COAP_DEFAULT_HANDLER
#define CLI_COAP_SECURE_USE_COAP_DEFAULT_HANDLER 0
#endif

namespace ot {
namespace Cli {

/**
 * This class implements the CLI CoAP Secure server and client.
 *
 */
class CoapSecure : private OutputWrapper
{
public:
    typedef Utils::CmdLineParser::Arg Arg;

    /**
     * Constructor
     *
     * @param[in]  aOutput The CLI console output context
     *
     */
    explicit CoapSecure(Output &aOutput);

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
        kMaxUriLength   = 32,
        kMaxBufferSize  = 16,
        kPskMaxLength   = 32,
        kPskIdMaxLength = 32
    };

    struct Command
    {
        const char *mName;
        otError (CoapSecure::*mHandler)(Arg aArgs[]);
    };

#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
    enum BlockType : uint8_t{
        kBlockType1,
        kBlockType2,
    };
#endif

    void PrintPayload(otMessage *aMessage);

    otError ProcessConnect(Arg aArgs[]);
    otError ProcessDelete(Arg aArgs[]);
    otError ProcessDisconnect(Arg aArgs[]);
    otError ProcessGet(Arg aArgs[]);
    otError ProcessHelp(Arg aArgs[]);
    otError ProcessPost(Arg aArgs[]);
    otError ProcessPsk(Arg aArgs[]);
    otError ProcessPut(Arg aArgs[]);
    otError ProcessResource(Arg aArgs[]);
    otError ProcessSet(Arg aArgs[]);
    otError ProcessStart(Arg aArgs[]);
    otError ProcessStop(Arg aArgs[]);
    otError ProcessX509(Arg aArgs[]);

    otError ProcessRequest(Arg aArgs[], otCoapCode aCoapCode);

    void Stop(void);

    static void HandleRequest(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo);
    void        HandleRequest(otMessage *aMessage, const otMessageInfo *aMessageInfo);

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

#if CLI_COAP_SECURE_USE_COAP_DEFAULT_HANDLER
    static void DefaultHandler(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo);
    void        DefaultHandler(otMessage *aMessage, const otMessageInfo *aMessageInfo);
#endif // CLI_COAP_SECURE_USE_COAP_DEFAULT_HANDLER

    static void HandleConnected(bool aConnected, void *aContext);
    void        HandleConnected(bool aConnected);

    static constexpr Command sCommands[] = {
        {"connect", &CoapSecure::ProcessConnect},
        {"delete", &CoapSecure::ProcessDelete},
        {"disconnect", &CoapSecure::ProcessDisconnect},
        {"get", &CoapSecure::ProcessGet},
        {"help", &CoapSecure::ProcessHelp},
        {"post", &CoapSecure::ProcessPost},
#ifdef MBEDTLS_KEY_EXCHANGE_PSK_ENABLED
        {"psk", &CoapSecure::ProcessPsk},
#endif
        {"put", &CoapSecure::ProcessPut},
        {"resource", &CoapSecure::ProcessResource},
        {"set", &CoapSecure::ProcessSet},
        {"start", &CoapSecure::ProcessStart},
        {"stop", &CoapSecure::ProcessStop},
#ifdef MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED
        {"x509", &CoapSecure::ProcessX509},
#endif
    };

    static_assert(Utils::LookupTable::IsSorted(sCommands), "Command Table is not sorted");

#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
    otCoapBlockwiseResource mResource;
#else
    otCoapResource mResource;
#endif
    char mUriPath[kMaxUriLength];
    char mResourceContent[kMaxBufferSize];

    bool    mShutdownFlag;
    bool    mUseCertificate;
    uint8_t mPsk[kPskMaxLength];
    uint8_t mPskLength;
    uint8_t mPskId[kPskIdMaxLength];
    uint8_t mPskIdLength;
#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
    uint32_t mBlockCount;
#endif
};

} // namespace Cli
} // namespace ot

#endif // OPENTHREAD_CONFIG_COAP_SECURE_API_ENABLE

#endif // CLI_COAP_SECURE_HPP_
