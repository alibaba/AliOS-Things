/*
 *  Copyright (c) 2019, The OpenThread Authors.
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
 *   This file includes definitions for Thread Radio Encapsulation Link (TREL) Packet
 */

#ifndef TREL_PACKET_HPP_
#define TREL_PACKET_HPP_

#include "openthread-core-config.h"

#if OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE

#include "common/encoding.hpp"
#include "common/locator.hpp"
#include "common/string.hpp"
#include "mac/mac_types.hpp"

namespace ot {
namespace Trel {

/**
 * This class represents a TREL radio link packet encapsulation header.
 *
 */
OT_TOOL_PACKED_BEGIN
class Header
{
public:
    /**
     * This enumeration defines packet types.
     *
     */
    enum Type : uint8_t
    {
        kTypeBroadcast = 0, ///< A TREL broadcast packet.
        kTypeUnicast   = 1, ///< A TREL unicast packet.
        kTypeAck       = 2, ///< A TREL Ack packet.
    };

    /**
     * This enumeration represents Ack Mode field in TREL header.
     *
     */
    enum AckMode : uint8_t
    {
        kNoAck,        ///< No TREL Ack is requested.
        kAckRequested, ///< A TREL Ack is requested.
    };

    static constexpr uint16_t kInfoStringSize = 128; ///< Max chars for the info string (@sa ToInfoString()).

    /**
     * This type defines the fixed-length `String` object returned from `ToString()` method.
     *
     */
    typedef String<kInfoStringSize> InfoString;

    /**
     * This method initializes the header.
     *
     * @param[in] aType     The header type.
     *
     */
    void Init(Type aType) { mControl = aType + kVersion; }

    /**
     * This method checks whether the version field in header is valid or not
     *
     * @returns TRUE if the version field is valid, FALSE otherwise.
     *
     */
    bool IsVersionValid(void) const { return (mControl & kVersionMask) == kVersion; }

    /**
     * This method gets the packet type.
     *
     * @returns The packet type.
     *
     */
    Type GetType(void) const { return static_cast<Type>(mControl & kTypeMask); }

    /**
     * This method gets the header length based on its type.
     *
     * @returns the header length (number of bytes).
     *
     */
    uint16_t GetLength(void) const { return GetSize(GetType()); }

    /**
     * This method gets the Ack Mode field from the header.
     *
     * @returns The Ack Mode field.
     *
     */
    AckMode GetAckMode(void) const { return (mControl & kAckModeFlag) ? kAckRequested : kNoAck; }

    /**
     * This method sets the Ack Mode field in the header.
     *
     * @param[in] aAckMode  The Ack Mode field
     *
     */
    void SetAckMode(AckMode aAckMode);

    /**
     * This method gets the channel field from the header.
     *
     * @returns The channel field.
     *
     */
    uint8_t GetChannel(void) const { return mChannel; }

    /**
     * This method sets the channel field in the header.
     *
     * @param[in] aChannel   A channel.
     *
     */
    void SetChannel(uint8_t aChannel) { mChannel = aChannel; }

    /**
     * This method gets the PAN Identifier field from the header.
     *
     * @returns The PAN Identifier field.
     *
     */
    Mac::PanId GetPanId(void) const { return Encoding::BigEndian::HostSwap16(mPanId); }

    /**
     * This method sets the PAN Identifier field in the header.
     *
     * @param[in] aPanId   A PAN Identifier.
     *
     */
    void SetPanId(Mac::PanId aPanId) { mPanId = Encoding::BigEndian::HostSwap16(aPanId); }

    /**
     * This method gets the packet number field from the header.
     *
     * @returns The packet number field.
     *
     */
    uint32_t GetPacketNumber(void) const { return Encoding::BigEndian::HostSwap32(mPacketNumber); }

    /**
     * This method sets the packet number field in the header.
     *
     * @param[in] aPacketNumber  The packet number.
     *
     */
    void SetPacketNumber(uint32_t aPacketNumber) { mPacketNumber = Encoding::BigEndian::HostSwap32(aPacketNumber); }

    /**
     * This method gets the source MAC address field from the header.
     *
     * @returns The source MAC address field.
     *
     */
    const Mac::ExtAddress &GetSource(void) const { return mSource; }

    /**
     * This method sets the source MAC address filed in the header.
     *
     * @param[in] aSource   A MAC extended address to set as source.
     *
     */
    void SetSource(const Mac::ExtAddress &aSource) { mSource = aSource; }

    /**
     * This method gets the destination MAC address field from the header.
     *
     * This method MUST be used with a unicast of ack type packet, otherwise its behavior is undefined.
     *
     * @returns The destination MAC address field.
     *
     */
    const Mac::ExtAddress &GetDestination(void) const { return mDestination; }

    /**
     * This method sets the destination MAC address field in the header.
     *
     * This method MUST be used with a unicast of ack type packet, otherwise its behavior is undefined.
     *
     * @param[in] aDest   A MAC extended address to set as destination.
     *
     */
    void SetDestination(const Mac::ExtAddress &aDest) { mDestination = aDest; }

    /**
     * This static method gets the size (number of bytes) in header of given packet type.
     *
     * @param[in] aType   The packet type.
     *
     * @returns The fixed header size (number of bytes) for @p aType packet.
     *
     */
    static uint16_t GetSize(Type aType);

    /**
     * This method returns a string representation of header.
     *
     * @returns An `InfoString` representation of header.
     *
     */
    InfoString ToString(void) const;

private:
    static constexpr uint8_t kTypeMask    = (3 << 0); // Bits 0-1 specify packet `Type`
    static constexpr uint8_t kAckModeFlag = (1 << 2); // Bit 2 indicate "ack mode" (TREL ack requested or not).
    static constexpr uint8_t kVersionMask = (7 << 5); // Bits 5-7 specify the version.
    static constexpr uint8_t kVersion     = (0 << 5); // Current TREL header version.

    // All header fields are big-endian.

    uint8_t         mControl;
    uint8_t         mChannel;
    uint16_t        mPanId;
    uint32_t        mPacketNumber;
    Mac::ExtAddress mSource;
    Mac::ExtAddress mDestination; // Present in `kTypeAck` or `kTypeUnicast` packet types.
} OT_TOOL_PACKED_END;

/**
 * This class represent a TREL radio link packet.
 *
 */
class Packet
{
public:
    /**
     * This method initializes the `Packet` with a given buffer and length.
     *
     * @param[in] aBuffer  A pointer to a buffer containing the entire packet (header and payload).
     * @param[in] aLength  Length (number of bytes) of the packet (including header and payload).
     *
     */
    void Init(uint8_t *aBuffer, uint16_t aLength);

    /**
     * This method initializes the `Packet` with a specified header type and given a payload.
     *
     * The payload buffer @p aPayload should have space reserved before the start of payload for the packet header.
     * This method will initialize the header with the given type @p aType. Rest of header fields can be updated after
     * initializing the packet.
     *
     * @param[in] aType          The packet type.
     * @param[in] aPayload       A pointer to a buffer containing the packet payload. Buffer should have space reserved
     *                           for header before the payload.
     * @param[in] aPayloadLength The length (number of bytes) in the payload only (not including the header).
     *
     */
    void Init(Header::Type aType, uint8_t *aPayload, uint16_t aPayloadLength);

    /**
     * This method gets a pointer to buffer containing the packet.
     *
     * @returns A pointer to buffer containing the packet.
     *
     */
    uint8_t *GetBuffer(void) { return mBuffer; }

    /**
     * This method gets a pointer to buffer containing the packet.
     *
     * @returns A pointer to buffer containing the packet.
     *
     */
    const uint8_t *GetBuffer(void) const { return mBuffer; }

    /**
     * This method gets the length of packet.
     *
     * @returns The length (number of bytes) of packet (header and payload).
     *
     */
    uint16_t GetLength(void) const { return mLength; }

    /**
     * This method checks whether or not the packet header is valid.
     *
     * @retval TRUE   The packet header is valid and well-formed.
     * @retval FALSE  The packet header is not valid.
     *
     */
    bool IsHeaderValid(void) const;

    /**
     * This method gets the packet header.
     *
     * @returns A reference to the packet header as `Header`.
     *
     */
    Header &GetHeader(void) { return *reinterpret_cast<Header *>(mBuffer); }

    /**
     * This method gets the packet header.
     *
     * @returns A reference to the packet header as `Header`.
     *
     */
    const Header &GetHeader(void) const { return *reinterpret_cast<const Header *>(mBuffer); }

    /**
     * This method gets a pointer to start of packet payload.
     *
     * @returns A pointer to start of packet payload (after header).
     *
     */
    uint8_t *GetPayload(void) { return mBuffer + GetHeader().GetLength(); }

    /**
     * This method gets a pointer to start of packet payload.
     *
     * @returns A pointer to start of packet payload (after header).
     *
     */
    const uint8_t *GetPayload(void) const { return mBuffer + GetHeader().GetLength(); }

    /**
     * This method gets the payload length.
     *
     * @returns The packet payload length (number of bytes).
     *
     */
    uint16_t GetPayloadLength(void) const { return mLength - GetHeader().GetLength(); }

private:
    uint8_t *mBuffer;
    uint16_t mLength;
};

} // namespace Trel
} // namespace ot

#endif // #if OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE

#endif // TREL_PACKET_HPP_
