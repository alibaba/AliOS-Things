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
 *   This file includes definitions for using mbedTLS.
 */

#ifndef DTLS_HPP_
#define DTLS_HPP_

#include "openthread-core-config.h"

#include <mbedtls/net_sockets.h>
#include <mbedtls/ssl.h>
#include <mbedtls/ssl_cookie.h>
#include <mbedtls/version.h>

#if OPENTHREAD_CONFIG_COAP_SECURE_API_ENABLE
#ifdef MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED
#include <mbedtls/base64.h>
#include <mbedtls/x509.h>
#include <mbedtls/x509_crl.h>
#include <mbedtls/x509_crt.h>
#include <mbedtls/x509_csr.h>
#endif
#endif

#include "common/locator.hpp"
#include "common/message.hpp"
#include "common/random.hpp"
#include "common/timer.hpp"
#include "crypto/sha256.hpp"
#include "meshcop/meshcop_tlvs.hpp"
#include "net/socket.hpp"
#include "net/udp6.hpp"

namespace ot {

namespace MeshCoP {

class Dtls : public InstanceLocator
{
public:
    static constexpr uint8_t kPskMaxLength = 32; ///< Maximum PSK length.

    /**
     * This constructor initializes the DTLS object.
     *
     * @param[in]  aNetif               A reference to the Thread network interface.
     * @param[in]  aLayerTwoSecurity    Specifies whether to use layer two security or not.
     *
     */
    explicit Dtls(Instance &aInstance, bool aLayerTwoSecurity);

    /**
     * This function pointer is called when a connection is established or torn down.
     *
     * @param[in]  aContext    A pointer to application-specific context.
     * @param[in]  aConnected  TRUE if a connection was established, FALSE otherwise.
     *
     */
    typedef void (*ConnectedHandler)(void *aContext, bool aConnected);

    /**
     * This function pointer is called when data is received from the DTLS session.
     *
     * @param[in]  aContext  A pointer to application-specific context.
     * @param[in]  aBuf      A pointer to the received data buffer.
     * @param[in]  aLength   Number of bytes in the received data buffer.
     *
     */
    typedef void (*ReceiveHandler)(void *aContext, uint8_t *aBuf, uint16_t aLength);

    /**
     * This function pointer is called when secure CoAP server want to send encrypted message.
     *
     * @param[in]  aContext      A pointer to arbitrary context information.
     * @param[in]  aMessage      A reference to the message to send.
     * @param[in]  aMessageInfo  A reference to the message info associated with @p aMessage.
     *
     */
    typedef Error (*TransportCallback)(void *aContext, ot::Message &aMessage, const Ip6::MessageInfo &aMessageInfo);

    /**
     * This method opens the DTLS socket.
     *
     * @param[in]  aReceiveHandler      A pointer to a function that is called to receive DTLS payload.
     * @param[in]  aConnectedHandler    A pointer to a function that is called when connected or disconnected.
     * @param[in]  aContext             A pointer to arbitrary context information.
     *
     * @retval kErrorNone     Successfully opened the socket.
     * @retval kErrorAlready  The DTLS is already open.
     *
     */
    Error Open(ReceiveHandler aReceiveHandler, ConnectedHandler aConnectedHandler, void *aContext);

    /**
     * This method binds this DTLS to a UDP port.
     *
     * @param[in]  aPort              The port to bind.
     *
     * @retval kErrorNone           Successfully bound the DTLS socket.
     * @retval kErrorInvalidState   The DTLS socket is not open.
     * @retval kErrorAlready        Already bound.
     *
     */
    Error Bind(uint16_t aPort);

    /**
     * This method gets the UDP port of this session.
     *
     * @returns  UDP port number.
     *
     */
    uint16_t GetUdpPort(void) const;

    /**
     * This method binds this DTLS with a transport callback.
     *
     * @param[in]  aCallback  A pointer to a function for sending messages.
     * @param[in]  aContext   A pointer to arbitrary context information.
     *
     * @retval kErrorNone           Successfully bound the DTLS socket.
     * @retval kErrorInvalidState   The DTLS socket is not open.
     * @retval kErrorAlready        Already bound.
     *
     */
    Error Bind(TransportCallback aCallback, void *aContext);

    /**
     * This method establishes a DTLS session.
     *
     * For CoAP Secure API do first:
     * Set X509 Pk and Cert for use DTLS mode ECDHE ECDSA with AES 128 CCM 8 or
     * set PreShared Key for use DTLS mode PSK with AES 128 CCM 8.
     *
     * @param[in]  aSockAddr               A reference to the remote sockaddr.
     *
     * @retval kErrorNone          Successfully started DTLS handshake.
     * @retval kErrorInvalidState  The DTLS socket is not open.
     *
     */
    Error Connect(const Ip6::SockAddr &aSockAddr);

    /**
     * This method indicates whether or not the DTLS session is active.
     *
     * @retval TRUE  If DTLS session is active.
     * @retval FALSE If DTLS session is not active.
     *
     */
    bool IsConnectionActive(void) const { return mState >= kStateConnecting; }

    /**
     * This method indicates whether or not the DTLS session is connected.
     *
     * @retval TRUE   The DTLS session is connected.
     * @retval FALSE  The DTLS session is not connected.
     *
     */
    bool IsConnected(void) const { return mState == kStateConnected; }

    /**
     * This method disconnects the DTLS session.
     *
     */
    void Disconnect(void);

    /**
     * This method closes the DTLS socket.
     *
     */
    void Close(void);

    /**
     * This method sets the PSK.
     *
     * @param[in]  aPsk  A pointer to the PSK.
     *
     * @retval kErrorNone          Successfully set the PSK.
     * @retval kErrorInvalidArgs   The PSK is invalid.
     *
     */
    Error SetPsk(const uint8_t *aPsk, uint8_t aPskLength);

#if OPENTHREAD_CONFIG_COAP_SECURE_API_ENABLE
#ifdef MBEDTLS_KEY_EXCHANGE_PSK_ENABLED
    /**
     * This method sets the Pre-Shared Key (PSK) for DTLS sessions-
     * identified by a PSK.
     *
     * DTLS mode "PSK with AES 128 CCM 8" for Application CoAPS.
     *
     * @param[in]  aPsk          A pointer to the PSK.
     * @param[in]  aPskLength    The PSK char length.
     * @param[in]  aPskIdentity  The Identity Name for the PSK.
     * @param[in]  aPskIdLength  The PSK Identity Length.
     *
     * @retval kErrorNone  Successfully set the PSK.
     *
     */
    void SetPreSharedKey(const uint8_t *aPsk, uint16_t aPskLength, const uint8_t *aPskIdentity, uint16_t aPskIdLength);

#endif

#ifdef MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED
    /**
     * This method sets a reference to the own x509 certificate with corresponding private key.
     *
     * DTLS mode "ECDHE ECDSA with AES 128 CCM 8" for Application CoAPS.
     *
     * @param[in]  aX509Certificate   A pointer to the PEM formatted X509 certificate.
     * @param[in]  aX509CertLength    The length of certificate.
     * @param[in]  aPrivateKey        A pointer to the PEM formatted private key.
     * @param[in]  aPrivateKeyLength  The length of the private key.
     *
     */
    void SetCertificate(const uint8_t *aX509Certificate,
                        uint32_t       aX509CertLength,
                        const uint8_t *aPrivateKey,
                        uint32_t       aPrivateKeyLength);

    /**
     * This method sets the trusted top level CAs. It is needed for validate the
     * certificate of the peer.
     *
     * DTLS mode "ECDHE ECDSA with AES 128 CCM 8" for Application CoAPS.
     *
     * @param[in]  aX509CaCertificateChain  A pointer to the PEM formatted X509 CA chain.
     * @param[in]  aX509CaCertChainLength   The length of chain.
     *
     */
    void SetCaCertificateChain(const uint8_t *aX509CaCertificateChain, uint32_t aX509CaCertChainLength);
#endif // MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED

#if defined(MBEDTLS_BASE64_C) && defined(MBEDTLS_SSL_KEEP_PEER_CERTIFICATE)
    /**
     * This method returns the peer x509 certificate base64 encoded.
     *
     * DTLS mode "ECDHE ECDSA with AES 128 CCM 8" for Application CoAPS.
     *
     * @param[out]  aPeerCert        A pointer to the base64 encoded certificate buffer.
     * @param[out]  aCertLength      The length of the base64 encoded peer certificate.
     * @param[in]   aCertBufferSize  The buffer size of aPeerCert.
     *
     * @retval kErrorInvalidState   Not connected yet.
     * @retval kErrorNone           Successfully get the peer certificate.
     * @retval kErrorNoBufs         Can't allocate memory for certificate.
     *
     */
    Error GetPeerCertificateBase64(unsigned char *aPeerCert, size_t *aCertLength, size_t aCertBufferSize);
#endif // defined(MBEDTLS_BASE64_C) && defined(MBEDTLS_SSL_KEEP_PEER_CERTIFICATE)

    /**
     * This method set the authentication mode for a dtls connection.
     *
     * Disable or enable the verification of peer certificate.
     * Must called before start.
     *
     * @param[in]  aVerifyPeerCertificate  true, if the peer certificate should verify.
     *
     */
    void SetSslAuthMode(bool aVerifyPeerCertificate) { mVerifyPeerCertificate = aVerifyPeerCertificate; }
#endif // OPENTHREAD_CONFIG_COAP_SECURE_API_ENABLE

#ifdef MBEDTLS_SSL_SRV_C
    /**
     * This method sets the Client ID used for generating the Hello Cookie.
     *
     * @param[in]  aClientId  A pointer to the Client ID.
     * @param[in]  aLength    Number of bytes in the Client ID.
     *
     * @retval kErrorNone  Successfully set the Client ID.
     *
     */
    Error SetClientId(const uint8_t *aClientId, uint8_t aLength);
#endif

    /**
     * This method sends data within the DTLS session.
     *
     * @param[in]  aMessage  A message to send via DTLS.
     * @param[in]  aLength   Number of bytes in the data buffer.
     *
     * @retval kErrorNone     Successfully sent the data via the DTLS session.
     * @retval kErrorNoBufs   A message is too long.
     *
     */
    Error Send(Message &aMessage, uint16_t aLength);

    /**
     * This method provides a received DTLS message to the DTLS object.
     *
     * @param[in]  aMessage  A reference to the message.
     *
     */
    void Receive(Message &aMessage);

    /**
     * This method sets the default message sub-type that will be used for all messages without defined
     * sub-type.
     *
     * @param[in]  aMessageSubType  The default message sub-type.
     *
     */
    void SetDefaultMessageSubType(Message::SubType aMessageSubType) { mMessageDefaultSubType = aMessageSubType; }

    /**
     * This method returns the DTLS session's peer address.
     *
     * @return DTLS session's message info.
     *
     */
    const Ip6::MessageInfo &GetMessageInfo(void) const { return mMessageInfo; }

    void HandleUdpReceive(Message &aMessage, const Ip6::MessageInfo &aMessageInfo);

private:
    enum State : uint8_t
    {
        kStateClosed,       // UDP socket is closed.
        kStateOpen,         // UDP socket is open.
        kStateInitializing, // The DTLS service is initializing.
        kStateConnecting,   // The DTLS service is establishing a connection.
        kStateConnected,    // The DTLS service has a connection established.
        kStateCloseNotify,  // The DTLS service is closing a connection.
    };

    static constexpr uint32_t kGuardTimeNewConnectionMilli = 2000;

#if !OPENTHREAD_CONFIG_COAP_SECURE_API_ENABLE
    static constexpr uint16_t kApplicationDataMaxLength = 1152;
#else
    static constexpr uint16_t kApplicationDataMaxLength = OPENTHREAD_CONFIG_DTLS_APPLICATION_DATA_MAX_LENGTH;
#endif

    static constexpr size_t kDtlsKeyBlockSize     = 40;
    static constexpr size_t kDtlsRandomBufferSize = 32;

    void  FreeMbedtls(void);
    Error Setup(bool aClient);

#if OPENTHREAD_CONFIG_COAP_SECURE_API_ENABLE
    /**
     * Set keys and/or certificates for dtls session dependent of used cipher suite.
     *
     * @retval mbedtls error, 0 if successfully.
     *
     */
    int SetApplicationCoapSecureKeys(void);
#endif

    static void HandleMbedtlsDebug(void *aContext, int aLevel, const char *aFile, int aLine, const char *aStr);
    void        HandleMbedtlsDebug(int aLevel, const char *aFile, int aLine, const char *aStr);

    static int HandleMbedtlsGetTimer(void *aContext);
    int        HandleMbedtlsGetTimer(void);

    static void HandleMbedtlsSetTimer(void *aContext, uint32_t aIntermediate, uint32_t aFinish);
    void        HandleMbedtlsSetTimer(uint32_t aIntermediate, uint32_t aFinish);

    static int HandleMbedtlsReceive(void *aContext, unsigned char *aBuf, size_t aLength);
    int        HandleMbedtlsReceive(unsigned char *aBuf, size_t aLength);

    static int HandleMbedtlsTransmit(void *aContext, const unsigned char *aBuf, size_t aLength);
    int        HandleMbedtlsTransmit(const unsigned char *aBuf, size_t aLength);

#ifdef MBEDTLS_SSL_EXPORT_KEYS
#if (MBEDTLS_VERSION_NUMBER >= 0x03000000)

    static void HandleMbedtlsExportKeys(void *                      aContext,
                                        mbedtls_ssl_key_export_type aType,
                                        const unsigned char *       aMasterSecret,
                                        size_t                      aMasterSecretLen,
                                        const unsigned char         aClientRandom[32],
                                        const unsigned char         aServerRandom[32],
                                        mbedtls_tls_prf_types       aTlsPrfType);

    void HandleMbedtlsExportKeys(mbedtls_ssl_key_export_type aType,
                                 const unsigned char *       aMasterSecret,
                                 size_t                      aMasterSecretLen,
                                 const unsigned char         aClientRandom[32],
                                 const unsigned char         aServerRandom[32],
                                 mbedtls_tls_prf_types       aTlsPrfType);

#else

    static int HandleMbedtlsExportKeys(void *               aContext,
                                       const unsigned char *aMasterSecret,
                                       const unsigned char *aKeyBlock,
                                       size_t               aMacLength,
                                       size_t               aKeyLength,
                                       size_t               aIvLength);
    int        HandleMbedtlsExportKeys(const unsigned char *aMasterSecret,
                                       const unsigned char *aKeyBlock,
                                       size_t               aMacLength,
                                       size_t               aKeyLength,
                                       size_t               aIvLength);

#endif // (MBEDTLS_VERSION_NUMBER >= 0x03000000)
#endif // MBEDTLS_SSL_EXPORT_KEYS

    static void HandleTimer(Timer &aTimer);
    void        HandleTimer(void);

    static void HandleUdpReceive(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo);

    void  HandleDtlsReceive(const uint8_t *aBuf, uint16_t aLength);
    Error HandleDtlsSend(const uint8_t *aBuf, uint16_t aLength, Message::SubType aMessageSubType);

    void Process(void);

    State mState;

    int     mCipherSuites[2];
    uint8_t mPsk[kPskMaxLength];
    uint8_t mPskLength;

    static const mbedtls_ecp_group_id sCurves[];
#if defined(MBEDTLS_KEY_EXCHANGE__WITH_CERT__ENABLED) || defined(MBEDTLS_KEY_EXCHANGE_WITH_CERT_ENABLED)
    static const int sHashes[];
#endif

#if OPENTHREAD_CONFIG_COAP_SECURE_API_ENABLE
#ifdef MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED
    const uint8_t *    mCaChainSrc;
    uint32_t           mCaChainLength;
    const uint8_t *    mOwnCertSrc;
    uint32_t           mOwnCertLength;
    const uint8_t *    mPrivateKeySrc;
    uint32_t           mPrivateKeyLength;
    mbedtls_x509_crt   mCaChain;
    mbedtls_x509_crt   mOwnCert;
    mbedtls_pk_context mPrivateKey;
#endif
#ifdef MBEDTLS_KEY_EXCHANGE_PSK_ENABLED
    const uint8_t *mPreSharedKey;
    const uint8_t *mPreSharedKeyIdentity;
    uint16_t       mPreSharedKeyLength;
    uint16_t       mPreSharedKeyIdLength;
#endif
#endif

    bool mVerifyPeerCertificate;

    mbedtls_ssl_context mSsl;
    mbedtls_ssl_config  mConf;

#ifdef MBEDTLS_SSL_COOKIE_C
    mbedtls_ssl_cookie_ctx mCookieCtx;
#endif

    TimerMilliContext mTimer;

    TimeMilli mTimerIntermediate;
    bool      mTimerSet : 1;

    bool mLayerTwoSecurity : 1;

    Message *mReceiveMessage;

    ConnectedHandler mConnectedHandler;
    ReceiveHandler   mReceiveHandler;
    void *           mContext;

    Ip6::MessageInfo mMessageInfo;
    Ip6::Udp::Socket mSocket;

    TransportCallback mTransportCallback;
    void *            mTransportContext;

    Message::SubType mMessageSubType;
    Message::SubType mMessageDefaultSubType;
};

} // namespace MeshCoP
} // namespace ot

#endif // DTLS_HPP_
