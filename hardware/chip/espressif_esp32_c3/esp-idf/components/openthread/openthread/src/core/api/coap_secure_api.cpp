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
 *   This file implements the OpenThread CoAP Secure API.
 */

#include "openthread-core-config.h"

#if OPENTHREAD_CONFIG_COAP_SECURE_API_ENABLE

#include <openthread/coap_secure.h>
#include <openthread/ip6.h>

#include "coap/coap_message.hpp"
#include "coap/coap_secure.hpp"
#include "common/as_core_type.hpp"
#include "common/locator_getters.hpp"

using namespace ot;

otError otCoapSecureStart(otInstance *aInstance, uint16_t aPort)
{
    return AsCoreType(aInstance).GetApplicationCoapSecure().Start(aPort);
}

#ifdef MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED
void otCoapSecureSetCertificate(otInstance *   aInstance,
                                const uint8_t *aX509Cert,
                                uint32_t       aX509Length,
                                const uint8_t *aPrivateKey,
                                uint32_t       aPrivateKeyLength)
{
    OT_ASSERT(aX509Cert != nullptr && aX509Length != 0 && aPrivateKey != nullptr && aPrivateKeyLength != 0);

    AsCoreType(aInstance).GetApplicationCoapSecure().SetCertificate(aX509Cert, aX509Length, aPrivateKey,
                                                                    aPrivateKeyLength);
}

void otCoapSecureSetCaCertificateChain(otInstance *   aInstance,
                                       const uint8_t *aX509CaCertificateChain,
                                       uint32_t       aX509CaCertChainLength)
{
    OT_ASSERT(aX509CaCertificateChain != nullptr && aX509CaCertChainLength != 0);

    AsCoreType(aInstance).GetApplicationCoapSecure().SetCaCertificateChain(aX509CaCertificateChain,
                                                                           aX509CaCertChainLength);
}
#endif // MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED

#ifdef MBEDTLS_KEY_EXCHANGE_PSK_ENABLED
void otCoapSecureSetPsk(otInstance *   aInstance,
                        const uint8_t *aPsk,
                        uint16_t       aPskLength,
                        const uint8_t *aPskIdentity,
                        uint16_t       aPskIdLength)
{
    OT_ASSERT(aPsk != nullptr && aPskLength != 0 && aPskIdentity != nullptr && aPskIdLength != 0);

    AsCoreType(aInstance).GetApplicationCoapSecure().SetPreSharedKey(aPsk, aPskLength, aPskIdentity, aPskIdLength);
}
#endif // MBEDTLS_KEY_EXCHANGE_PSK_ENABLED

#if defined(MBEDTLS_BASE64_C) && defined(MBEDTLS_SSL_KEEP_PEER_CERTIFICATE)
otError otCoapSecureGetPeerCertificateBase64(otInstance *   aInstance,
                                             unsigned char *aPeerCert,
                                             size_t *       aCertLength,
                                             size_t         aCertBufferSize)
{
    return AsCoreType(aInstance).GetApplicationCoapSecure().GetPeerCertificateBase64(aPeerCert, aCertLength,
                                                                                     aCertBufferSize);
}
#endif // defined(MBEDTLS_BASE64_C) && defined(MBEDTLS_SSL_KEEP_PEER_CERTIFICATE)

void otCoapSecureSetSslAuthMode(otInstance *aInstance, bool aVerifyPeerCertificate)
{
    AsCoreType(aInstance).GetApplicationCoapSecure().SetSslAuthMode(aVerifyPeerCertificate);
}

otError otCoapSecureConnect(otInstance *                    aInstance,
                            const otSockAddr *              aSockAddr,
                            otHandleCoapSecureClientConnect aHandler,
                            void *                          aContext)
{
    return AsCoreType(aInstance).GetApplicationCoapSecure().Connect(AsCoreType(aSockAddr), aHandler, aContext);
}

void otCoapSecureDisconnect(otInstance *aInstance)
{
    AsCoreType(aInstance).GetApplicationCoapSecure().Disconnect();
}

bool otCoapSecureIsConnected(otInstance *aInstance)
{
    return AsCoreType(aInstance).GetApplicationCoapSecure().IsConnected();
}

bool otCoapSecureIsConnectionActive(otInstance *aInstance)
{
    return AsCoreType(aInstance).GetApplicationCoapSecure().IsConnectionActive();
}

void otCoapSecureStop(otInstance *aInstance)
{
    AsCoreType(aInstance).GetApplicationCoapSecure().Stop();
}

#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
otError otCoapSecureSendRequestBlockWise(otInstance *                aInstance,
                                         otMessage *                 aMessage,
                                         otCoapResponseHandler       aHandler,
                                         void *                      aContext,
                                         otCoapBlockwiseTransmitHook aTransmitHook,
                                         otCoapBlockwiseReceiveHook  aReceiveHook)
{
    return AsCoreType(aInstance).GetApplicationCoapSecure().SendMessage(AsCoapMessage(aMessage), aHandler, aContext,
                                                                        aTransmitHook, aReceiveHook);
}
#endif

otError otCoapSecureSendRequest(otInstance *          aInstance,
                                otMessage *           aMessage,
                                otCoapResponseHandler aHandler,
                                void *                aContext)
{
    return AsCoreType(aInstance).GetApplicationCoapSecure().SendMessage(AsCoapMessage(aMessage), aHandler, aContext);
}

#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
void otCoapSecureAddBlockWiseResource(otInstance *aInstance, otCoapBlockwiseResource *aResource)
{
    AsCoreType(aInstance).GetApplicationCoapSecure().AddBlockWiseResource(AsCoreType(aResource));
}

void otCoapSecureRemoveBlockWiseResource(otInstance *aInstance, otCoapBlockwiseResource *aResource)
{
    AsCoreType(aInstance).GetApplicationCoapSecure().RemoveBlockWiseResource(AsCoreType(aResource));
}
#endif

void otCoapSecureAddResource(otInstance *aInstance, otCoapResource *aResource)
{
    AsCoreType(aInstance).GetApplicationCoapSecure().AddResource(AsCoreType(aResource));
}

void otCoapSecureRemoveResource(otInstance *aInstance, otCoapResource *aResource)
{
    AsCoreType(aInstance).GetApplicationCoapSecure().RemoveResource(AsCoreType(aResource));
}

void otCoapSecureSetClientConnectedCallback(otInstance *                    aInstance,
                                            otHandleCoapSecureClientConnect aHandler,
                                            void *                          aContext)
{
    AsCoreType(aInstance).GetApplicationCoapSecure().SetClientConnectedCallback(aHandler, aContext);
}

void otCoapSecureSetDefaultHandler(otInstance *aInstance, otCoapRequestHandler aHandler, void *aContext)
{
    AsCoreType(aInstance).GetApplicationCoapSecure().SetDefaultHandler(aHandler, aContext);
}

#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
otError otCoapSecureSendResponseBlockWise(otInstance *                aInstance,
                                          otMessage *                 aMessage,
                                          const otMessageInfo *       aMessageInfo,
                                          void *                      aContext,
                                          otCoapBlockwiseTransmitHook aTransmitHook)
{
    return AsCoreType(aInstance).GetApplicationCoapSecure().SendMessage(
        AsCoapMessage(aMessage), AsCoreType(aMessageInfo), nullptr, aContext, aTransmitHook);
}
#endif

otError otCoapSecureSendResponse(otInstance *aInstance, otMessage *aMessage, const otMessageInfo *aMessageInfo)
{
    return AsCoreType(aInstance).GetApplicationCoapSecure().SendMessage(AsCoapMessage(aMessage),
                                                                        AsCoreType(aMessageInfo));
}

#endif // OPENTHREAD_CONFIG_COAP_SECURE_API_ENABLE
