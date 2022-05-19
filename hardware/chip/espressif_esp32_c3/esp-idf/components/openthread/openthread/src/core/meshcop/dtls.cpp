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
 *   This file implements the necessary hooks for mbedTLS.
 */

#include "dtls.hpp"

#include <mbedtls/debug.h>
#ifdef MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED
#include <mbedtls/pem.h>
#endif

#include <openthread/platform/radio.h>

#include "common/code_utils.hpp"
#include "common/debug.hpp"
#include "common/encoding.hpp"
#include "common/instance.hpp"
#include "common/locator_getters.hpp"
#include "common/logging.hpp"
#include "common/timer.hpp"
#include "crypto/mbedtls.hpp"
#include "crypto/sha256.hpp"
#include "thread/thread_netif.hpp"

#if OPENTHREAD_CONFIG_DTLS_ENABLE

namespace ot {
namespace MeshCoP {

const mbedtls_ecp_group_id Dtls::sCurves[] = {MBEDTLS_ECP_DP_SECP256R1, MBEDTLS_ECP_DP_NONE};
#if defined(MBEDTLS_KEY_EXCHANGE__WITH_CERT__ENABLED) || defined(MBEDTLS_KEY_EXCHANGE_WITH_CERT_ENABLED)
const int Dtls::sHashes[] = {MBEDTLS_MD_SHA256, MBEDTLS_MD_NONE};
#endif

Dtls::Dtls(Instance &aInstance, bool aLayerTwoSecurity)
    : InstanceLocator(aInstance)
    , mState(kStateClosed)
    , mPskLength(0)
    , mVerifyPeerCertificate(true)
    , mTimer(aInstance, Dtls::HandleTimer, this)
    , mTimerIntermediate(0)
    , mTimerSet(false)
    , mLayerTwoSecurity(aLayerTwoSecurity)
    , mReceiveMessage(nullptr)
    , mConnectedHandler(nullptr)
    , mReceiveHandler(nullptr)
    , mContext(nullptr)
    , mSocket(aInstance)
    , mTransportCallback(nullptr)
    , mTransportContext(nullptr)
    , mMessageSubType(Message::kSubTypeNone)
    , mMessageDefaultSubType(Message::kSubTypeNone)
{
#if OPENTHREAD_CONFIG_COAP_SECURE_API_ENABLE
#ifdef MBEDTLS_KEY_EXCHANGE_PSK_ENABLED
    mPreSharedKey         = nullptr;
    mPreSharedKeyIdentity = nullptr;
    mPreSharedKeyIdLength = 0;
    mPreSharedKeyLength   = 0;
#endif

#ifdef MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED
    mCaChainSrc       = nullptr;
    mCaChainLength    = 0;
    mOwnCertSrc       = nullptr;
    mOwnCertLength    = 0;
    mPrivateKeySrc    = nullptr;
    mPrivateKeyLength = 0;
    memset(&mCaChain, 0, sizeof(mCaChain));
    memset(&mOwnCert, 0, sizeof(mOwnCert));
    memset(&mPrivateKey, 0, sizeof(mPrivateKey));
#endif
#endif

    memset(mCipherSuites, 0, sizeof(mCipherSuites));
    memset(mPsk, 0, sizeof(mPsk));
    memset(&mSsl, 0, sizeof(mSsl));
    memset(&mConf, 0, sizeof(mConf));

#ifdef MBEDTLS_SSL_COOKIE_C
    memset(&mCookieCtx, 0, sizeof(mCookieCtx));
#endif
}

void Dtls::FreeMbedtls(void)
{
#if defined(MBEDTLS_SSL_SRV_C) && defined(MBEDTLS_SSL_COOKIE_C)
    mbedtls_ssl_cookie_free(&mCookieCtx);
#endif
#if OPENTHREAD_CONFIG_COAP_SECURE_API_ENABLE
#ifdef MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED
    mbedtls_x509_crt_free(&mCaChain);
    mbedtls_x509_crt_free(&mOwnCert);
    mbedtls_pk_free(&mPrivateKey);
#endif
#endif
    mbedtls_ssl_config_free(&mConf);
    mbedtls_ssl_free(&mSsl);
}

Error Dtls::Open(ReceiveHandler aReceiveHandler, ConnectedHandler aConnectedHandler, void *aContext)
{
    Error error;

    VerifyOrExit(mState == kStateClosed, error = kErrorAlready);

    SuccessOrExit(error = mSocket.Open(&Dtls::HandleUdpReceive, this));

    mReceiveHandler   = aReceiveHandler;
    mConnectedHandler = aConnectedHandler;
    mContext          = aContext;
    mState            = kStateOpen;

exit:
    return error;
}

Error Dtls::Connect(const Ip6::SockAddr &aSockAddr)
{
    Error error;

    VerifyOrExit(mState == kStateOpen, error = kErrorInvalidState);

    mMessageInfo.SetPeerAddr(aSockAddr.GetAddress());
    mMessageInfo.SetPeerPort(aSockAddr.mPort);

    error = Setup(true);

exit:
    return error;
}

void Dtls::HandleUdpReceive(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo)
{
    static_cast<Dtls *>(aContext)->HandleUdpReceive(*static_cast<Message *>(aMessage),
                                                    *static_cast<const Ip6::MessageInfo *>(aMessageInfo));
}

void Dtls::HandleUdpReceive(Message &aMessage, const Ip6::MessageInfo &aMessageInfo)
{
    switch (mState)
    {
    case MeshCoP::Dtls::kStateClosed:
        ExitNow();

    case MeshCoP::Dtls::kStateOpen:
        IgnoreError(mSocket.Connect(Ip6::SockAddr(aMessageInfo.GetPeerAddr(), aMessageInfo.GetPeerPort())));

        mMessageInfo.SetPeerAddr(aMessageInfo.GetPeerAddr());
        mMessageInfo.SetPeerPort(aMessageInfo.GetPeerPort());
        mMessageInfo.SetIsHostInterface(aMessageInfo.IsHostInterface());

        if (Get<ThreadNetif>().HasUnicastAddress(aMessageInfo.GetSockAddr()))
        {
            mMessageInfo.SetSockAddr(aMessageInfo.GetSockAddr());
        }

        mMessageInfo.SetSockPort(aMessageInfo.GetSockPort());

        SuccessOrExit(Setup(false));
        break;

    default:
        // Once DTLS session is started, communicate only with a peer.
        VerifyOrExit((mMessageInfo.GetPeerAddr() == aMessageInfo.GetPeerAddr()) &&
                     (mMessageInfo.GetPeerPort() == aMessageInfo.GetPeerPort()));
        break;
    }

#ifdef MBEDTLS_SSL_SRV_C
    if (mState == MeshCoP::Dtls::kStateConnecting)
    {
        IgnoreError(SetClientId(mMessageInfo.GetPeerAddr().mFields.m8, sizeof(mMessageInfo.GetPeerAddr().mFields)));
    }
#endif

    Receive(aMessage);

exit:
    return;
}

uint16_t Dtls::GetUdpPort(void) const
{
    return mSocket.GetSockName().GetPort();
}

Error Dtls::Bind(uint16_t aPort)
{
    Error error;

    VerifyOrExit(mState == kStateOpen, error = kErrorInvalidState);
    VerifyOrExit(mTransportCallback == nullptr, error = kErrorAlready);

    SuccessOrExit(error = mSocket.Bind(aPort, OT_NETIF_UNSPECIFIED));

exit:
    return error;
}

Error Dtls::Bind(TransportCallback aCallback, void *aContext)
{
    Error error = kErrorNone;

    VerifyOrExit(mState == kStateOpen, error = kErrorInvalidState);
    VerifyOrExit(!mSocket.IsBound(), error = kErrorAlready);
    VerifyOrExit(mTransportCallback == nullptr, error = kErrorAlready);

    mTransportCallback = aCallback;
    mTransportContext  = aContext;

exit:
    return error;
}

Error Dtls::Setup(bool aClient)
{
    int rval;

    // do not handle new connection before guard time expired
    VerifyOrExit(mState == kStateOpen, rval = MBEDTLS_ERR_SSL_TIMEOUT);

    mState = kStateInitializing;

    mbedtls_ssl_init(&mSsl);
    mbedtls_ssl_config_init(&mConf);
#if OPENTHREAD_CONFIG_COAP_SECURE_API_ENABLE
#ifdef MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED
    mbedtls_x509_crt_init(&mCaChain);
    mbedtls_x509_crt_init(&mOwnCert);
    mbedtls_pk_init(&mPrivateKey);
#endif
#endif
#if defined(MBEDTLS_SSL_SRV_C) && defined(MBEDTLS_SSL_COOKIE_C)
    mbedtls_ssl_cookie_init(&mCookieCtx);
#endif

    rval = mbedtls_ssl_config_defaults(&mConf, aClient ? MBEDTLS_SSL_IS_CLIENT : MBEDTLS_SSL_IS_SERVER,
                                       MBEDTLS_SSL_TRANSPORT_DATAGRAM, MBEDTLS_SSL_PRESET_DEFAULT);
    VerifyOrExit(rval == 0);

#if OPENTHREAD_CONFIG_COAP_SECURE_API_ENABLE
    if (mVerifyPeerCertificate && mCipherSuites[0] == MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8)
    {
        mbedtls_ssl_conf_authmode(&mConf, MBEDTLS_SSL_VERIFY_REQUIRED);
    }
    else
    {
        mbedtls_ssl_conf_authmode(&mConf, MBEDTLS_SSL_VERIFY_NONE);
    }
#else
    OT_UNUSED_VARIABLE(mVerifyPeerCertificate);
#endif

    mbedtls_ssl_conf_rng(&mConf, mbedtls_ctr_drbg_random, Random::Crypto::MbedTlsContextGet());
    mbedtls_ssl_conf_min_version(&mConf, MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_3);
    mbedtls_ssl_conf_max_version(&mConf, MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_3);

    OT_ASSERT(mCipherSuites[1] == 0);
    mbedtls_ssl_conf_ciphersuites(&mConf, mCipherSuites);
    if (mCipherSuites[0] == MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8)
    {
        mbedtls_ssl_conf_curves(&mConf, sCurves);
#if defined(MBEDTLS_KEY_EXCHANGE__WITH_CERT__ENABLED) || defined(MBEDTLS_KEY_EXCHANGE_WITH_CERT_ENABLED)
        mbedtls_ssl_conf_sig_hashes(&mConf, sHashes);
#endif
    }

#if (MBEDTLS_VERSION_NUMBER >= 0x03000000)
    mbedtls_ssl_set_export_keys_cb(&mSsl, HandleMbedtlsExportKeys, this);
#else
    mbedtls_ssl_conf_export_keys_cb(&mConf, HandleMbedtlsExportKeys, this);
#endif

    mbedtls_ssl_conf_handshake_timeout(&mConf, 8000, 60000);
    mbedtls_ssl_conf_dbg(&mConf, HandleMbedtlsDebug, this);

#if defined(MBEDTLS_SSL_SRV_C) && defined(MBEDTLS_SSL_COOKIE_C)
    if (!aClient)
    {
        rval = mbedtls_ssl_cookie_setup(&mCookieCtx, mbedtls_ctr_drbg_random, Random::Crypto::MbedTlsContextGet());
        VerifyOrExit(rval == 0);

        mbedtls_ssl_conf_dtls_cookies(&mConf, mbedtls_ssl_cookie_write, mbedtls_ssl_cookie_check, &mCookieCtx);
    }
#endif

    rval = mbedtls_ssl_setup(&mSsl, &mConf);
    VerifyOrExit(rval == 0);

    mbedtls_ssl_set_bio(&mSsl, this, &Dtls::HandleMbedtlsTransmit, HandleMbedtlsReceive, nullptr);
    mbedtls_ssl_set_timer_cb(&mSsl, this, &Dtls::HandleMbedtlsSetTimer, HandleMbedtlsGetTimer);

    if (mCipherSuites[0] == MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8)
    {
        rval = mbedtls_ssl_set_hs_ecjpake_password(&mSsl, mPsk, mPskLength);
    }
#if OPENTHREAD_CONFIG_COAP_SECURE_API_ENABLE
    else
    {
        rval = SetApplicationCoapSecureKeys();
    }
#endif
    VerifyOrExit(rval == 0);

    mReceiveMessage = nullptr;
    mMessageSubType = Message::kSubTypeNone;
    mState          = kStateConnecting;

    if (mCipherSuites[0] == MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8)
    {
        otLogInfoMeshCoP("DTLS started");
    }
#if OPENTHREAD_CONFIG_COAP_SECURE_API_ENABLE
    else
    {
        otLogInfoCoap("Application Coap Secure DTLS started");
    }
#endif

    mState = kStateConnecting;

    Process();

exit:
    if ((mState == kStateInitializing) && (rval != 0))
    {
        mState = kStateOpen;
        FreeMbedtls();
    }

    return Crypto::MbedTls::MapError(rval);
}

#if OPENTHREAD_CONFIG_COAP_SECURE_API_ENABLE
int Dtls::SetApplicationCoapSecureKeys(void)
{
    int rval = 0;

    switch (mCipherSuites[0])
    {
    case MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8:
#ifdef MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED
        if (mCaChainSrc != nullptr)
        {
            rval = mbedtls_x509_crt_parse(&mCaChain, static_cast<const unsigned char *>(mCaChainSrc),
                                          static_cast<size_t>(mCaChainLength));
            VerifyOrExit(rval == 0);
            mbedtls_ssl_conf_ca_chain(&mConf, &mCaChain, nullptr);
        }

        if (mOwnCertSrc != nullptr && mPrivateKeySrc != nullptr)
        {
            rval = mbedtls_x509_crt_parse(&mOwnCert, static_cast<const unsigned char *>(mOwnCertSrc),
                                          static_cast<size_t>(mOwnCertLength));
            VerifyOrExit(rval == 0);

#if (MBEDTLS_VERSION_NUMBER >= 0x03000000)
            rval = mbedtls_pk_parse_key(&mPrivateKey, static_cast<const unsigned char *>(mPrivateKeySrc),
                                        static_cast<size_t>(mPrivateKeyLength), nullptr, 0, mbedtls_ctr_drbg_random,
                                        Random::Crypto::MbedTlsContextGet());
#else
            rval = mbedtls_pk_parse_key(&mPrivateKey, static_cast<const unsigned char *>(mPrivateKeySrc),
                                        static_cast<size_t>(mPrivateKeyLength), nullptr, 0);
#endif
            VerifyOrExit(rval == 0);
            rval = mbedtls_ssl_conf_own_cert(&mConf, &mOwnCert, &mPrivateKey);
            VerifyOrExit(rval == 0);
        }
#endif
        break;

    case MBEDTLS_TLS_PSK_WITH_AES_128_CCM_8:
#ifdef MBEDTLS_KEY_EXCHANGE_PSK_ENABLED
        rval = mbedtls_ssl_conf_psk(&mConf, static_cast<const unsigned char *>(mPreSharedKey), mPreSharedKeyLength,
                                    static_cast<const unsigned char *>(mPreSharedKeyIdentity), mPreSharedKeyIdLength);
        VerifyOrExit(rval == 0);
#endif
        break;

    default:
        otLogCritCoap("Application Coap Secure DTLS: Not supported cipher.");
        rval = MBEDTLS_ERR_SSL_BAD_INPUT_DATA;
        ExitNow();
        break;
    }

exit:
    return rval;
}

#endif // OPENTHREAD_CONFIG_COAP_SECURE_API_ENABLE

void Dtls::Close(void)
{
    Disconnect();

    mState             = kStateClosed;
    mTransportCallback = nullptr;
    mTransportContext  = nullptr;
    mTimerSet          = false;

    IgnoreError(mSocket.Close());
    mTimer.Stop();
}

void Dtls::Disconnect(void)
{
    VerifyOrExit(mState == kStateConnecting || mState == kStateConnected);

    mbedtls_ssl_close_notify(&mSsl);
    mState = kStateCloseNotify;
    mTimer.Start(kGuardTimeNewConnectionMilli);

    mMessageInfo.Clear();
    IgnoreError(mSocket.Connect());

    FreeMbedtls();

exit:
    return;
}

Error Dtls::SetPsk(const uint8_t *aPsk, uint8_t aPskLength)
{
    Error error = kErrorNone;

    VerifyOrExit(aPskLength <= sizeof(mPsk), error = kErrorInvalidArgs);

    memcpy(mPsk, aPsk, aPskLength);
    mPskLength       = aPskLength;
    mCipherSuites[0] = MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8;
    mCipherSuites[1] = 0;

exit:
    return error;
}

#if OPENTHREAD_CONFIG_COAP_SECURE_API_ENABLE
#ifdef MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED

void Dtls::SetCertificate(const uint8_t *aX509Certificate,
                          uint32_t       aX509CertLength,
                          const uint8_t *aPrivateKey,
                          uint32_t       aPrivateKeyLength)
{
    OT_ASSERT(aX509CertLength > 0);
    OT_ASSERT(aX509Certificate != nullptr);

    OT_ASSERT(aPrivateKeyLength > 0);
    OT_ASSERT(aPrivateKey != nullptr);

    mOwnCertSrc       = aX509Certificate;
    mOwnCertLength    = aX509CertLength;
    mPrivateKeySrc    = aPrivateKey;
    mPrivateKeyLength = aPrivateKeyLength;

    mCipherSuites[0] = MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8;
    mCipherSuites[1] = 0;
}

void Dtls::SetCaCertificateChain(const uint8_t *aX509CaCertificateChain, uint32_t aX509CaCertChainLength)
{
    OT_ASSERT(aX509CaCertChainLength > 0);
    OT_ASSERT(aX509CaCertificateChain != nullptr);

    mCaChainSrc    = aX509CaCertificateChain;
    mCaChainLength = aX509CaCertChainLength;
}

#endif // MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED

#ifdef MBEDTLS_KEY_EXCHANGE_PSK_ENABLED
void Dtls::SetPreSharedKey(const uint8_t *aPsk, uint16_t aPskLength, const uint8_t *aPskIdentity, uint16_t aPskIdLength)
{
    OT_ASSERT(aPsk != nullptr);
    OT_ASSERT(aPskIdentity != nullptr);
    OT_ASSERT(aPskLength > 0);
    OT_ASSERT(aPskIdLength > 0);

    mPreSharedKey         = aPsk;
    mPreSharedKeyLength   = aPskLength;
    mPreSharedKeyIdentity = aPskIdentity;
    mPreSharedKeyIdLength = aPskIdLength;

    mCipherSuites[0] = MBEDTLS_TLS_PSK_WITH_AES_128_CCM_8;
    mCipherSuites[1] = 0;
}
#endif

#if defined(MBEDTLS_BASE64_C) && defined(MBEDTLS_SSL_KEEP_PEER_CERTIFICATE)
Error Dtls::GetPeerCertificateBase64(unsigned char *aPeerCert, size_t *aCertLength, size_t aCertBufferSize)
{
    Error error = kErrorNone;

    VerifyOrExit(mState == kStateConnected, error = kErrorInvalidState);

    VerifyOrExit(mbedtls_base64_encode(aPeerCert, aCertBufferSize, aCertLength, mSsl.session->peer_cert->raw.p,
                                       mSsl.session->peer_cert->raw.len) == 0,
                 error = kErrorNoBufs);

exit:
    return error;
}
#endif // defined(MBEDTLS_BASE64_C) && defined(MBEDTLS_SSL_KEEP_PEER_CERTIFICATE)

#endif // OPENTHREAD_CONFIG_COAP_SECURE_API_ENABLE

#ifdef MBEDTLS_SSL_SRV_C
Error Dtls::SetClientId(const uint8_t *aClientId, uint8_t aLength)
{
    int rval = mbedtls_ssl_set_client_transport_id(&mSsl, aClientId, aLength);
    return Crypto::MbedTls::MapError(rval);
}
#endif

Error Dtls::Send(Message &aMessage, uint16_t aLength)
{
    Error   error = kErrorNone;
    uint8_t buffer[kApplicationDataMaxLength];

    VerifyOrExit(aLength <= kApplicationDataMaxLength, error = kErrorNoBufs);

    // Store message specific sub type.
    if (aMessage.GetSubType() != Message::kSubTypeNone)
    {
        mMessageSubType = aMessage.GetSubType();
    }

    aMessage.ReadBytes(0, buffer, aLength);

    SuccessOrExit(error = Crypto::MbedTls::MapError(mbedtls_ssl_write(&mSsl, buffer, aLength)));

    aMessage.Free();

exit:
    return error;
}

void Dtls::Receive(Message &aMessage)
{
    mReceiveMessage = &aMessage;

    Process();

    mReceiveMessage = nullptr;
}

int Dtls::HandleMbedtlsTransmit(void *aContext, const unsigned char *aBuf, size_t aLength)
{
    return static_cast<Dtls *>(aContext)->HandleMbedtlsTransmit(aBuf, aLength);
}

int Dtls::HandleMbedtlsTransmit(const unsigned char *aBuf, size_t aLength)
{
    Error error;
    int   rval = 0;

    if (mCipherSuites[0] == MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8)
    {
        otLogDebgMeshCoP("Dtls::HandleMbedtlsTransmit");
    }
#if OPENTHREAD_CONFIG_COAP_SECURE_API_ENABLE
    else
    {
        otLogDebgCoap("Dtls::ApplicationCoapSecure HandleMbedtlsTransmit");
    }
#endif

    error = HandleDtlsSend(aBuf, static_cast<uint16_t>(aLength), mMessageSubType);

    // Restore default sub type.
    mMessageSubType = mMessageDefaultSubType;

    switch (error)
    {
    case kErrorNone:
        rval = static_cast<int>(aLength);
        break;

    case kErrorNoBufs:
        rval = MBEDTLS_ERR_SSL_WANT_WRITE;
        break;

    default:
        otLogWarnMeshCoP("Dtls::HandleMbedtlsTransmit: %s error", ErrorToString(error));
        rval = MBEDTLS_ERR_NET_SEND_FAILED;
        break;
    }

    return rval;
}

int Dtls::HandleMbedtlsReceive(void *aContext, unsigned char *aBuf, size_t aLength)
{
    return static_cast<Dtls *>(aContext)->HandleMbedtlsReceive(aBuf, aLength);
}

int Dtls::HandleMbedtlsReceive(unsigned char *aBuf, size_t aLength)
{
    int rval;

    if (mCipherSuites[0] == MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8)
    {
        otLogDebgMeshCoP("Dtls::HandleMbedtlsReceive");
    }
#if OPENTHREAD_CONFIG_COAP_SECURE_API_ENABLE
    else
    {
        otLogDebgCoap("Dtls:: ApplicationCoapSecure HandleMbedtlsReceive");
    }
#endif

    VerifyOrExit(mReceiveMessage != nullptr && (rval = mReceiveMessage->GetLength() - mReceiveMessage->GetOffset()) > 0,
                 rval = MBEDTLS_ERR_SSL_WANT_READ);

    if (aLength > static_cast<size_t>(rval))
    {
        aLength = static_cast<size_t>(rval);
    }

    rval = mReceiveMessage->ReadBytes(mReceiveMessage->GetOffset(), aBuf, static_cast<uint16_t>(aLength));
    mReceiveMessage->MoveOffset(rval);

exit:
    return rval;
}

int Dtls::HandleMbedtlsGetTimer(void *aContext)
{
    return static_cast<Dtls *>(aContext)->HandleMbedtlsGetTimer();
}

int Dtls::HandleMbedtlsGetTimer(void)
{
    int rval;

    if (mCipherSuites[0] == MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8)
    {
        otLogDebgMeshCoP("Dtls::HandleMbedtlsGetTimer");
    }
#if OPENTHREAD_CONFIG_COAP_SECURE_API_ENABLE
    else
    {
        otLogDebgCoap("Dtls:: ApplicationCoapSecure HandleMbedtlsGetTimer");
    }
#endif

    if (!mTimerSet)
    {
        rval = -1;
    }
    else if (!mTimer.IsRunning())
    {
        rval = 2;
    }
    else if (mTimerIntermediate <= TimerMilli::GetNow())
    {
        rval = 1;
    }
    else
    {
        rval = 0;
    }

    return rval;
}

void Dtls::HandleMbedtlsSetTimer(void *aContext, uint32_t aIntermediate, uint32_t aFinish)
{
    static_cast<Dtls *>(aContext)->HandleMbedtlsSetTimer(aIntermediate, aFinish);
}

void Dtls::HandleMbedtlsSetTimer(uint32_t aIntermediate, uint32_t aFinish)
{
    if (mCipherSuites[0] == MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8)
    {
        otLogDebgMeshCoP("Dtls::SetTimer");
    }
#if OPENTHREAD_CONFIG_COAP_SECURE_API_ENABLE
    else
    {
        otLogDebgCoap("Dtls::ApplicationCoapSecure SetTimer");
    }
#endif

    if (aFinish == 0)
    {
        mTimerSet = false;
        mTimer.Stop();
    }
    else
    {
        mTimerSet = true;
        mTimer.Start(aFinish);
        mTimerIntermediate = TimerMilli::GetNow() + aIntermediate;
    }
}

#if (MBEDTLS_VERSION_NUMBER >= 0x03000000)

void Dtls::HandleMbedtlsExportKeys(void *                      aContext,
                                   mbedtls_ssl_key_export_type aType,
                                   const unsigned char *       aMasterSecret,
                                   size_t                      aMasterSecretLen,
                                   const unsigned char         aClientRandom[32],
                                   const unsigned char         aServerRandom[32],
                                   mbedtls_tls_prf_types       aTlsPrfType)
{
    static_cast<Dtls *>(aContext)->HandleMbedtlsExportKeys(aType, aMasterSecret, aMasterSecretLen, aClientRandom,
                                                           aServerRandom, aTlsPrfType);
}

void Dtls::HandleMbedtlsExportKeys(mbedtls_ssl_key_export_type aType,
                                   const unsigned char *       aMasterSecret,
                                   size_t                      aMasterSecretLen,
                                   const unsigned char         aClientRandom[32],
                                   const unsigned char         aServerRandom[32],
                                   mbedtls_tls_prf_types       aTlsPrfType)
{
    Crypto::Sha256::Hash kek;
    Crypto::Sha256       sha256;
    unsigned char        keyBlock[kDtlsKeyBlockSize];
    unsigned char        randBytes[2 * kDtlsRandomBufferSize];

    VerifyOrExit(mCipherSuites[0] == MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8);
    VerifyOrExit(aType == MBEDTLS_SSL_KEY_EXPORT_TLS12_MASTER_SECRET);

    memcpy(randBytes, aServerRandom, kDtlsRandomBufferSize);
    memcpy(randBytes + kDtlsRandomBufferSize, aClientRandom, kDtlsRandomBufferSize);

    // Retrieve the Key block from Master secret
    mbedtls_ssl_tls_prf(aTlsPrfType, aMasterSecret, aMasterSecretLen, "key expansion", randBytes, sizeof(randBytes),
                        keyBlock, sizeof(keyBlock));

    sha256.Start();
    sha256.Update(keyBlock, kDtlsKeyBlockSize);
    sha256.Finish(kek);

    otLogDebgMeshCoP("Generated KEK");
    Get<KeyManager>().SetKek(kek.GetBytes());

exit:
    return;
}

#else

int Dtls::HandleMbedtlsExportKeys(void *aContext,
                                  const unsigned char *aMasterSecret,
                                  const unsigned char *aKeyBlock,
                                  size_t aMacLength,
                                  size_t aKeyLength,
                                  size_t aIvLength)
{
    return static_cast<Dtls *>(aContext)->HandleMbedtlsExportKeys(aMasterSecret, aKeyBlock, aMacLength, aKeyLength,
                                                                  aIvLength);
}

int Dtls::HandleMbedtlsExportKeys(const unsigned char *aMasterSecret,
                                  const unsigned char *aKeyBlock,
                                  size_t aMacLength,
                                  size_t aKeyLength,
                                  size_t aIvLength)
{
    OT_UNUSED_VARIABLE(aMasterSecret);

    Crypto::Sha256::Hash kek;
    Crypto::Sha256 sha256;

    VerifyOrExit(mCipherSuites[0] == MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8);

    sha256.Start();
    sha256.Update(aKeyBlock, 2 * static_cast<uint16_t>(aMacLength + aKeyLength + aIvLength));
    sha256.Finish(kek);

    otLogDebgMeshCoP("Generated KEK");
    Get<KeyManager>().SetKek(kek.GetBytes());

exit:
    return 0;
}

#endif // (MBEDTLS_VERSION_NUMBER >= 0x03000000)

void Dtls::HandleTimer(Timer &aTimer)
{
    static_cast<Dtls *>(static_cast<TimerMilliContext &>(aTimer).GetContext())->HandleTimer();
}

void Dtls::HandleTimer(void)
{
    switch (mState)
    {
    case kStateConnecting:
    case kStateConnected:
        Process();
        break;

    case kStateCloseNotify:
        mState = kStateOpen;
        mTimer.Stop();

        if (mConnectedHandler != nullptr)
        {
            mConnectedHandler(mContext, false);
        }
        break;

    default:
        OT_ASSERT(false);
        OT_UNREACHABLE_CODE(break);
    }
}

void Dtls::Process(void)
{
    uint8_t buf[OPENTHREAD_CONFIG_DTLS_MAX_CONTENT_LEN];
    bool    shouldDisconnect = false;
    int     rval;

    while ((mState == kStateConnecting) || (mState == kStateConnected))
    {
        if (mState == kStateConnecting)
        {
            rval = mbedtls_ssl_handshake(&mSsl);

            if (mSsl.MBEDTLS_PRIVATE(state) == MBEDTLS_SSL_HANDSHAKE_OVER)
            {
                mState = kStateConnected;

                if (mConnectedHandler != nullptr)
                {
                    mConnectedHandler(mContext, true);
                }
            }
        }
        else
        {
            rval = mbedtls_ssl_read(&mSsl, buf, sizeof(buf));
        }

        if (rval > 0)
        {
            if (mReceiveHandler != nullptr)
            {
                mReceiveHandler(mContext, buf, static_cast<uint16_t>(rval));
            }
        }
        else if (rval == 0 || rval == MBEDTLS_ERR_SSL_WANT_READ || rval == MBEDTLS_ERR_SSL_WANT_WRITE)
        {
            break;
        }
        else
        {
            switch (rval)
            {
            case MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY:
                mbedtls_ssl_close_notify(&mSsl);
                ExitNow(shouldDisconnect = true);
                OT_UNREACHABLE_CODE(break);

            case MBEDTLS_ERR_SSL_HELLO_VERIFY_REQUIRED:
                break;

            case MBEDTLS_ERR_SSL_FATAL_ALERT_MESSAGE:
                mbedtls_ssl_close_notify(&mSsl);
                ExitNow(shouldDisconnect = true);
                OT_UNREACHABLE_CODE(break);

            case MBEDTLS_ERR_SSL_INVALID_MAC:
                if (mSsl.MBEDTLS_PRIVATE(state) != MBEDTLS_SSL_HANDSHAKE_OVER)
                {
                    mbedtls_ssl_send_alert_message(&mSsl, MBEDTLS_SSL_ALERT_LEVEL_FATAL,
                                                   MBEDTLS_SSL_ALERT_MSG_BAD_RECORD_MAC);
                    ExitNow(shouldDisconnect = true);
                }

                break;

            default:
                if (mSsl.MBEDTLS_PRIVATE(state) != MBEDTLS_SSL_HANDSHAKE_OVER)
                {
                    mbedtls_ssl_send_alert_message(&mSsl, MBEDTLS_SSL_ALERT_LEVEL_FATAL,
                                                   MBEDTLS_SSL_ALERT_MSG_HANDSHAKE_FAILURE);
                    ExitNow(shouldDisconnect = true);
                }

                break;
            }

            mbedtls_ssl_session_reset(&mSsl);
            if (mCipherSuites[0] == MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8)
            {
                mbedtls_ssl_set_hs_ecjpake_password(&mSsl, mPsk, mPskLength);
            }
            break;
        }
    }

exit:

    if (shouldDisconnect)
    {
        Disconnect();
    }
}

void Dtls::HandleMbedtlsDebug(void *aContext, int aLevel, const char *aFile, int aLine, const char *aStr)
{
    static_cast<Dtls *>(aContext)->HandleMbedtlsDebug(aLevel, aFile, aLine, aStr);
}

void Dtls::HandleMbedtlsDebug(int aLevel, const char *aFile, int aLine, const char *aStr)
{
    OT_UNUSED_VARIABLE(aStr);
    OT_UNUSED_VARIABLE(aFile);
    OT_UNUSED_VARIABLE(aLine);

    switch (aLevel)
    {
    case 1:
        otLogCritMbedTls("[%hu] %s", mSocket.GetSockName().mPort, aStr);
        break;

    case 2:
        otLogWarnMbedTls("[%hu] %s", mSocket.GetSockName().mPort, aStr);
        break;

    case 3:
        otLogInfoMbedTls("[%hu] %s", mSocket.GetSockName().mPort, aStr);
        break;

    case 4:
    default:
        otLogDebgMbedTls("[%hu] %s", mSocket.GetSockName().mPort, aStr);
        break;
    }
}

Error Dtls::HandleDtlsSend(const uint8_t *aBuf, uint16_t aLength, Message::SubType aMessageSubType)
{
    Error        error   = kErrorNone;
    ot::Message *message = nullptr;

    VerifyOrExit((message = mSocket.NewMessage(0)) != nullptr, error = kErrorNoBufs);
    message->SetSubType(aMessageSubType);
    message->SetLinkSecurityEnabled(mLayerTwoSecurity);

    SuccessOrExit(error = message->AppendBytes(aBuf, aLength));

    // Set message sub type in case Joiner Finalize Response is appended to the message.
    if (aMessageSubType != Message::kSubTypeNone)
    {
        message->SetSubType(aMessageSubType);
    }

    if (mTransportCallback)
    {
        SuccessOrExit(error = mTransportCallback(mTransportContext, *message, mMessageInfo));
    }
    else
    {
        SuccessOrExit(error = mSocket.SendTo(*message, mMessageInfo));
    }

exit:
    FreeMessageOnError(message, error);
    return error;
}

} // namespace MeshCoP
} // namespace ot

#endif // OPENTHREAD_CONFIG_DTLS_ENABLE
