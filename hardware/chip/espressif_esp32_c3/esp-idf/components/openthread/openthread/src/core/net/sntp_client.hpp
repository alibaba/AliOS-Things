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

#ifndef SNTP_CLIENT_HPP_
#define SNTP_CLIENT_HPP_

#include "openthread-core-config.h"

#if OPENTHREAD_CONFIG_SNTP_CLIENT_ENABLE

#include <openthread/sntp.h>

#include "common/message.hpp"
#include "common/non_copyable.hpp"
#include "common/timer.hpp"
#include "net/ip6.hpp"
#include "net/netif.hpp"

/**
 * @file
 *   This file includes definitions for the SNTP client.
 */

namespace ot {
namespace Sntp {

using ot::Encoding::BigEndian::HostSwap32;

/**
 * This class implements SNTP header generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class Header
{
public:
    /**
     * Default constructor for SNTP Header.
     *
     */
    Header(void);

    /**
     * Defines supported SNTP modes.
     *
     */
    enum Mode : uint8_t
    {
        kModeClient = 3,
        kModeServer = 4,
    };

    static constexpr uint8_t kKissCodeLength = 4; ///< Length of the kiss code in ASCII format

    /**
     * This method returns the flags field value.
     *
     * @returns Value of the flags field (LI, VN and Mode).
     *
     */
    uint8_t GetFlags(void) const { return mFlags; }

    /**
     * This method sets the flags field.
     *
     * @param[in]  aFlags  The value of the flags field.
     *
     */
    void SetFlags(uint8_t aFlags) { mFlags = aFlags; }

    /**
     * This method returns the SNTP operational mode.
     *
     * @returns SNTP operational mode.
     *
     */
    Mode GetMode(void) const { return static_cast<Mode>((mFlags & kModeMask) >> kModeOffset); }

    /**
     * This method returns the packet stratum field value.
     *
     * @returns Value of the packet stratum.
     *
     */
    uint8_t GetStratum(void) const { return mStratum; }

    /**
     * This method sets the packet stratum field value.
     *
     * @param[in]  aStratum  The value of the packet stratum field.
     *
     */
    void SetStratum(uint8_t aStratum) { mStratum = aStratum; }

    /**
     * This method returns the poll field value.
     *
     * @returns Value of the poll field.
     *
     */
    uint8_t GetPoll(void) const { return mPoll; }

    /**
     * This method sets the poll field.
     *
     * @param[in]  aPoll  The value of the poll field.
     *
     */
    void SetPoll(uint8_t aPoll) { mPoll = aPoll; }

    /**
     * This method returns the precision field value.
     *
     * @returns Value of the precision field.
     *
     */
    uint8_t GetPrecision(void) const { return mPrecision; }

    /**
     * This method sets the precision field.
     *
     * @param[in]  aPrecision  The value of the precision field.
     *
     */
    void SetPrecision(uint8_t aPrecision) { mPrecision = aPrecision; }

    /**
     * This method returns the root delay field value.
     *
     * @returns Value of the root delay field.
     *
     */
    uint32_t GetRootDelay(void) const { return HostSwap32(mRootDelay); }

    /**
     * This method sets the root delay field.
     *
     * @param[in]  aRootDelay  The value of the root delay field.
     *
     */
    void SetRootDelay(uint32_t aRootDelay) { mRootDelay = HostSwap32(aRootDelay); }

    /**
     * This method returns the root dispersion field value.
     *
     * @returns Value of the root dispersion field.
     *
     */
    uint32_t GetRootDispersion(void) const { return HostSwap32(mRootDispersion); }

    /**
     * This method sets the root dispersion field.
     *
     * @param[in]  aRootDispersion  The value of the root dispersion field.
     *
     */
    void SetRootDispersion(uint32_t aRootDispersion) { mRootDispersion = HostSwap32(aRootDispersion); }

    /**
     * This method returns the reference identifier field value.
     *
     * @returns Value of the reference identifier field.
     *
     */
    uint32_t GetReferenceId(void) const { return HostSwap32(mReferenceId); }

    /**
     * This method sets the reference identifier field.
     *
     * @param[in]  aReferenceId  The value of the reference identifier field.
     *
     */
    void SetReferenceId(uint32_t aReferenceId) { mReferenceId = HostSwap32(aReferenceId); }

    /**
     * This method returns the kiss code in ASCII format.
     *
     * @returns Value of the reference identifier field in ASCII format.
     *
     */
    char *GetKissCode(void) { return reinterpret_cast<char *>(&mReferenceId); }

    /**
     * This method returns the reference timestamp seconds field.
     *
     * @returns Value of the reference timestamp seconds field.
     *
     */
    uint32_t GetReferenceTimestampSeconds(void) const { return HostSwap32(mReferenceTimestampSeconds); }

    /**
     * This method sets the reference timestamp seconds field.
     *
     * @param[in]  aReferenceTimestampSeconds  Value of the reference timestamp seconds field.
     *
     */
    void SetReferenceTimestampSeconds(uint32_t aReferenceTimestampSeconds)
    {
        mReferenceTimestampSeconds = HostSwap32(aReferenceTimestampSeconds);
    }

    /**
     * This method returns the reference timestamp fraction field.
     *
     * @returns Value of the reference timestamp fraction field.
     *
     */
    uint32_t GetReferenceTimestampFraction(void) const { return HostSwap32(mReferenceTimestampFraction); }

    /**
     * This method sets the reference timestamp fraction field.
     *
     * @param[in]  aReferenceTimestampFraction  Value of the reference timestamp fraction field.
     *
     */
    void SetReferenceTimestampFraction(uint32_t aReferenceTimestampFraction)
    {
        mReferenceTimestampFraction = HostSwap32(aReferenceTimestampFraction);
    }

    /**
     * This method returns the originate timestamp seconds field.
     *
     * @returns Value of the originate timestamp seconds field.
     *
     */
    uint32_t GetOriginateTimestampSeconds(void) const { return HostSwap32(mOriginateTimestampSeconds); }

    /**
     * This method sets the originate timestamp seconds field.
     *
     * @param[in]  aOriginateTimestampSeconds  Value of the originate timestamp seconds field.
     *
     */
    void SetOriginateTimestampSeconds(uint32_t aOriginateTimestampSeconds)
    {
        mOriginateTimestampSeconds = HostSwap32(aOriginateTimestampSeconds);
    }

    /**
     * This method returns the originate timestamp fraction field.
     *
     * @returns Value of the originate timestamp fraction field.
     *
     */
    uint32_t GetOriginateTimestampFraction(void) const { return HostSwap32(mOriginateTimestampFraction); }

    /**
     * This method sets the originate timestamp fraction field.
     *
     * @param[in]  aOriginateTimestampFraction  Value of the originate timestamp fraction field.
     *
     */
    void SetOriginateTimestampFraction(uint32_t aOriginateTimestampFraction)
    {
        mOriginateTimestampFraction = HostSwap32(aOriginateTimestampFraction);
    }

    /**
     * This method returns the receive timestamp seconds field.
     *
     * @returns Value of the receive timestamp seconds field.
     *
     */
    uint32_t GetReceiveTimestampSeconds(void) const { return HostSwap32(mReceiveTimestampSeconds); }

    /**
     * This method sets the receive timestamp seconds field.
     *
     * @param[in]  aReceiveTimestampSeconds  Value of the receive timestamp seconds field.
     *
     */
    void SetReceiveTimestampSeconds(uint32_t aReceiveTimestampSeconds)
    {
        mReceiveTimestampSeconds = HostSwap32(aReceiveTimestampSeconds);
    }

    /**
     * This method returns the receive timestamp fraction field.
     *
     * @returns Value of the receive timestamp fraction field.
     *
     */
    uint32_t GetReceiveTimestampFraction(void) const { return HostSwap32(mReceiveTimestampFraction); }

    /**
     * This method sets the receive timestamp fraction field.
     *
     * @param[in]  aReceiveTimestampFraction  Value of the receive timestamp fraction field.
     *
     */
    void SetReceiveTimestampFraction(uint32_t aReceiveTimestampFraction)
    {
        mReceiveTimestampFraction = HostSwap32(aReceiveTimestampFraction);
    }

    /**
     * This method returns the transmit timestamp seconds field.
     *
     * @returns Value of the transmit timestamp seconds field.
     *
     */
    uint32_t GetTransmitTimestampSeconds(void) const { return HostSwap32(mTransmitTimestampSeconds); }

    /**
     * This method sets the transmit timestamp seconds field.
     *
     * @param[in]  aTransmitTimestampSeconds  Value of the transmit timestamp seconds field.
     *
     */
    void SetTransmitTimestampSeconds(uint32_t aTransmitTimestampSeconds)
    {
        mTransmitTimestampSeconds = HostSwap32(aTransmitTimestampSeconds);
    }

    /**
     * This method returns the transmit timestamp fraction field.
     *
     * @returns Value of the transmit timestamp fraction field.
     *
     */
    uint32_t GetTransmitTimestampFraction(void) const { return HostSwap32(mTransmitTimestampFraction); }

    /**
     * This method sets the transmit timestamp fraction field.
     *
     * @param[in]  aTransmitTimestampFraction  Value of the transmit timestamp fraction field.
     *
     */
    void SetTransmitTimestampFraction(uint32_t aTransmitTimestampFraction)
    {
        mTransmitTimestampFraction = HostSwap32(aTransmitTimestampFraction);
    }

private:
    static constexpr uint8_t kNtpVersion    = 4;                      // Current NTP version.
    static constexpr uint8_t kLeapOffset    = 6;                      // Leap Indicator field offset.
    static constexpr uint8_t kLeapMask      = 0x03 << kLeapOffset;    // Leap Indicator field mask.
    static constexpr uint8_t kVersionOffset = 3;                      // Version field offset.
    static constexpr uint8_t kVersionMask   = 0x07 << kVersionOffset; // Version field mask.
    static constexpr uint8_t kModeOffset    = 0;                      // Mode field offset.
    static constexpr uint8_t kModeMask      = 0x07 << kModeOffset;    // Mode filed mask.

    uint8_t  mFlags;                      // SNTP flags: LI Leap Indicator, VN Version Number and Mode.
    uint8_t  mStratum;                    // Packet Stratum.
    uint8_t  mPoll;                       // Maximum interval between successive messages, in log2 seconds.
    uint8_t  mPrecision;                  // The precision of the system clock, in log2 seconds.
    uint32_t mRootDelay;                  // Total round-trip delay to the reference clock, in NTP short format.
    uint32_t mRootDispersion;             // Total dispersion to the reference clock.
    uint32_t mReferenceId;                // ID identifying the particular server or reference clock.
    uint32_t mReferenceTimestampSeconds;  // Time the system clock was last set or corrected (NTP format).
    uint32_t mReferenceTimestampFraction; // Fraction part of above value.
    uint32_t mOriginateTimestampSeconds;  // Time at the client when the request departed for the server (NTP format).
    uint32_t mOriginateTimestampFraction; // Fraction part of above value.
    uint32_t mReceiveTimestampSeconds;    // Time at the server when the request arrived from the client (NTP format).
    uint32_t mReceiveTimestampFraction;   // Fraction part of above value.
    uint32_t mTransmitTimestampSeconds;   // Time at the server when the response left for the client (NTP format).
    uint32_t mTransmitTimestampFraction;  // Fraction part of above value.
} OT_TOOL_PACKED_END;

/**
 * This class implements metadata required for SNTP retransmission.
 *
 */
class QueryMetadata
{
    friend class Client;

public:
    /**
     * Default constructor for the object.
     *
     */
    QueryMetadata(void);

    /**
     * This constructor initializes the object with specific values.
     *
     * @param[in]  aHandler  Pointer to a handler function for the response.
     * @param[in]  aContext  Context for the handler function.
     *
     */
    QueryMetadata(otSntpResponseHandler aHandler, void *aContext);

    /**
     * This method appends request data to the message.
     *
     * @param[in]  aMessage  A reference to the message.
     *
     * @retval kErrorNone    Successfully appended the bytes.
     * @retval kErrorNoBufs  Insufficient available buffers to grow the message.
     *
     */
    Error AppendTo(Message &aMessage) const { return aMessage.Append(*this); }

    /**
     * This method reads request data from the message.
     *
     * @param[in]  aMessage  A reference to the message.
     *
     */
    void ReadFrom(const Message &aMessage)
    {
        Error error = aMessage.Read(aMessage.GetLength() - sizeof(*this), *this);

        OT_ASSERT(error == kErrorNone);
        OT_UNUSED_VARIABLE(error);
    }

    /**
     * This method updates request data in the message.
     *
     * @param[in]  aMessage  A reference to the message.
     *
     */
    void UpdateIn(Message &aMessage) const { aMessage.Write(aMessage.GetLength() - sizeof(*this), *this); }

private:
    uint32_t              mTransmitTimestamp;   ///< Time at the client when the request departed for the server.
    otSntpResponseHandler mResponseHandler;     ///< A function pointer that is called on response reception.
    void *                mResponseContext;     ///< A pointer to arbitrary context information.
    TimeMilli             mTransmissionTime;    ///< Time when the timer should shoot for this message.
    Ip6::Address          mSourceAddress;       ///< IPv6 address of the message source.
    Ip6::Address          mDestinationAddress;  ///< IPv6 address of the message destination.
    uint16_t              mDestinationPort;     ///< UDP port of the message destination.
    uint8_t               mRetransmissionCount; ///< Number of retransmissions.
};

/**
 * This class implements SNTP client.
 *
 */
class Client : private NonCopyable
{
public:
    /**
     * This constructor initializes the object.
     *
     * @param[in]  aInstance     A reference to the OpenThread instance.
     *
     */
    explicit Client(Instance &aInstance);

    /**
     * This method starts the SNTP client.
     *
     * @retval kErrorNone     Successfully started the SNTP client.
     * @retval kErrorAlready  The socket is already open.
     */
    Error Start(void);

    /**
     * This method stops the SNTP client.
     *
     * @retval kErrorNone  Successfully stopped the SNTP client.
     *
     */
    Error Stop(void);

    /**
     * This method returns the unix era number.
     *
     * @returns The unix era number.
     *
     */
    uint32_t GetUnixEra(void) const { return mUnixEra; }

    /**
     * This method sets the unix era number.
     *
     * @param[in]  aUnixEra  The unix era number.
     *
     */
    void SetUnixEra(uint32_t aUnixEra) { mUnixEra = aUnixEra; }

    /**
     * This method sends an SNTP query.
     *
     * @param[in]  aQuery    A pointer to specify SNTP query parameters.
     * @param[in]  aHandler  A function pointer that shall be called on response reception or time-out.
     * @param[in]  aContext  A pointer to arbitrary context information.
     *
     * @retval kErrorNone         Successfully sent SNTP query.
     * @retval kErrorNoBufs       Failed to allocate retransmission data.
     * @retval kErrorInvalidArgs  Invalid arguments supplied.
     *
     */
    Error Query(const otSntpQuery *aQuery, otSntpResponseHandler aHandler, void *aContext);

private:
    static constexpr uint32_t kTimeAt1970 = 2208988800UL; // num seconds between 1st Jan 1900 and 1st Jan 1970.

    static constexpr uint32_t kResponseTimeout = OPENTHREAD_CONFIG_SNTP_CLIENT_RESPONSE_TIMEOUT;
    static constexpr uint8_t  kMaxRetransmit   = OPENTHREAD_CONFIG_SNTP_CLIENT_MAX_RETRANSMIT;

    Message *NewMessage(const Header &aHeader);
    Message *CopyAndEnqueueMessage(const Message &aMessage, const QueryMetadata &aQueryMetadata);
    void     DequeueMessage(Message &aMessage);
    Error    SendMessage(Message &aMessage, const Ip6::MessageInfo &aMessageInfo);
    void     SendCopy(const Message &aMessage, const Ip6::MessageInfo &aMessageInfo);

    Message *FindRelatedQuery(const Header &aResponseHeader, QueryMetadata &aQueryMetadata);
    void FinalizeSntpTransaction(Message &aQuery, const QueryMetadata &aQueryMetadata, uint64_t aTime, Error aResult);

    static void HandleRetransmissionTimer(Timer &aTimer);
    void        HandleRetransmissionTimer(void);

    static void HandleUdpReceive(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo);
    void        HandleUdpReceive(Message &aMessage, const Ip6::MessageInfo &aMessageInfo);

    Ip6::Udp::Socket mSocket;

    MessageQueue mPendingQueries;
    TimerMilli   mRetransmissionTimer;

    uint32_t mUnixEra;
};

} // namespace Sntp
} // namespace ot

#endif // OPENTHREAD_CONFIG_SNTP_CLIENT_ENABLE

#endif // SNTP_CLIENT_HPP_
