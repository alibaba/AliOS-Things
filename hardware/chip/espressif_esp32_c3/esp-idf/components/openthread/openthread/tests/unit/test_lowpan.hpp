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

#ifndef TEST_LOWPAN_HPP
#define TEST_LOWPAN_HPP

#include <stdint.h>

#include "common/code_utils.hpp"
#include "common/instance.hpp"
#include "mac/mac.hpp"
#include "net/ip6_headers.hpp"
#include "thread/lowpan.hpp"
#include "thread/thread_netif.hpp"

namespace ot {

class TestIphcVector
{
public:
    enum
    {
        kContextUnused    = 255,
        kPayloadMaxLength = 512
    };

    struct Payload
    {
        uint8_t  mData[kPayloadMaxLength];
        uint16_t mLength;
    };

    /**
     * Default constructor for the object.
     *
     */
    explicit TestIphcVector(const char *aTestName)
    {
        memset(reinterpret_cast<void *>(this), 0, sizeof(TestIphcVector));
        mTestName              = aTestName;
        mSrcContext.mContextId = kContextUnused;
        mDstContext.mContextId = kContextUnused;
    }

    /**
     * This method sets long MAC source address.
     *
     * @param aAddress Pointer to the long MAC address.
     *
     */
    void SetMacSource(const uint8_t *aAddress) { mMacSource.SetExtended(aAddress); }

    /**
     * This method sets short MAC source address.
     *
     * @param aAddress Short MAC address.
     *
     */
    void SetMacSource(uint16_t aAddress) { mMacSource.SetShort(aAddress); }

    /**
     * This method sets long MAC destination address.
     *
     * @param aAddress Pointer to the long MAC address.
     *
     */
    void SetMacDestination(const uint8_t *aAddress) { mMacDestination.SetExtended(aAddress); }

    /**
     * This method sets short MAC destination address.
     *
     * @param aAddress Short MAC address.
     *
     */
    void SetMacDestination(uint16_t aAddress) { mMacDestination.SetShort(aAddress); }

    /**
     * This method initializes IPv6 Header.
     *
     * @param aVersionClassFlow Value of the Version, Traffic class and Flow control fields.
     * @param aPayloadLength    Value of the payload length field.
     * @param aNextHeader       Value of the next header field.
     * @param aHopLimit         Value of the hop limit field.
     * @param aSource           String represents IPv6 source address.
     * @param aDestination      String represents IPv6 destination address.
     *
     */
    void SetIpHeader(uint32_t    aVersionClassFlow,
                     uint16_t    aPayloadLength,
                     uint8_t     aNextHeader,
                     uint8_t     aHopLimit,
                     const char *aSource,
                     const char *aDestination)
    {
        mIpHeader.Init(aVersionClassFlow);
        mIpHeader.SetPayloadLength(aPayloadLength);
        mIpHeader.SetNextHeader(aNextHeader);
        mIpHeader.SetHopLimit(aHopLimit);
        IgnoreError(mIpHeader.GetSource().FromString(aSource));
        IgnoreError(mIpHeader.GetDestination().FromString(aDestination));
    }

    /**
     * This method initializes IPv6 Encapsulated Header.
     *
     * @param aVersionClassFlow Value of the Version, Traffic class and Flow control fields.
     * @param aPayloadLength    Value of the payload length field.
     * @param aNextHeader       Value of the next header field.
     * @param aHopLimit         Value of the hop limit field.
     * @param aSource           String represents IPv6 source address.
     * @param aDestination      String represents IPv6 destination address.
     *
     */
    void SetIpTunneledHeader(uint32_t    aVersionClassFlow,
                             uint16_t    aPayloadLength,
                             uint8_t     aNextHeader,
                             uint8_t     aHopLimit,
                             const char *aSource,
                             const char *aDestination)
    {
        mIpTunneledHeader.Init(aVersionClassFlow);
        mIpTunneledHeader.SetPayloadLength(aPayloadLength);
        mIpTunneledHeader.SetNextHeader(aNextHeader);
        mIpTunneledHeader.SetHopLimit(aHopLimit);
        IgnoreError(mIpTunneledHeader.GetSource().FromString(aSource));
        IgnoreError(mIpTunneledHeader.GetDestination().FromString(aDestination));
    }

    /**
     * This method initializes IPv6 Extension Header.
     *
     * @param aExtHeader        Pointer to the extension header data.
     * @param aExtHeaderLength  Length of the extension header data.
     *
     */
    void SetExtHeader(const uint8_t *aExtHeader, uint16_t aExtHeaderLength)
    {
        memcpy(mExtHeader.mData, aExtHeader, aExtHeaderLength);
        mExtHeader.mLength = aExtHeaderLength;
    }

    /**
     * This method initializes UDP Header.
     *
     * @param aSource       Value of the source port.
     * @param aDestination  Value of the destination port.
     * @param aLength       Value of the length field.
     * @param aChecksum     Value of the checksum field.
     *
     */
    void SetUDPHeader(uint16_t aSource, uint16_t aDestination, uint16_t aLength, uint16_t aChecksum)
    {
        mUdpHeader.SetSourcePort(aSource);
        mUdpHeader.SetDestinationPort(aDestination);
        mUdpHeader.SetLength(aLength);
        mUdpHeader.SetChecksum(aChecksum);
    }

    /**
     * This method initializes LOWPAN_IPHC Header.
     *
     * @param aIphc        Pointer to the LOWPAN_IPHC header.
     * @param aIphcLength  Length of the LOWPAN_IPHC header.
     *
     */
    void SetIphcHeader(const uint8_t *aIphc, uint16_t aIphcLength)
    {
        memcpy(mIphcHeader.mData, aIphc, aIphcLength);
        mIphcHeader.mLength = aIphcLength;
    }

    /**
     * This method sets the expect result of the compression / decompression procedure.
     *
     * @param aError  Expected result.
     *
     */
    void SetError(Error aError) { mError = aError; }

    /**
     * This method initializes IPv6 Payload (uncompressed data).
     *
     * @param aPayload  Pointer to the payload data.
     * @param aLength   Length of the payload data.
     *
     */
    void SetPayload(const uint8_t *aPayload, uint16_t aLength)
    {
        memcpy(mPayload.mData, aPayload, aLength);
        mPayload.mLength = aLength;
    }

    /**
     * This method sets the offset from the beginning of the IPv6 header to the uncompressed
     * payload.
     *
     * @param aPayloadOffset  The offset from the beginning of the IPv6 header to the uncompressed
     *                        payload.
     *
     */
    void SetPayloadOffset(uint16_t aPayloadOffset) { mPayloadOffset = aPayloadOffset; }

    /**
     * This method returns compressed LOWPAN_IPHC frame.
     *
     * @returns The compressed stream.
     *
     */
    void GetCompressedStream(uint8_t *aIphc, uint16_t &aIphcLength);

    /**
     * This method returns message object with the uncompressed IPv6 packet.
     *
     * @returns The message object with the uncompressed IPv6 packet.
     *
     */
    void GetUncompressedStream(Message &aMessage);

    /**
     * This method returns data with the uncompressed IPv6 packet.
     *
     * @returns The data with the uncompressed IPv6 packet.
     *
     */
    void GetUncompressedStream(uint8_t *aIp6, uint16_t &aIp6Length);

    /**
     * This fields represent uncompressed IPv6 packet.
     *
     */
    Mac::Address     mMacSource;
    Mac::Address     mMacDestination;
    Ip6::Header      mIpHeader;
    Payload          mExtHeader;
    Ip6::Header      mIpTunneledHeader;
    Ip6::Udp::Header mUdpHeader;

    /**
     * This fields represent compressed IPv6 packet.
     *
     */
    Payload         mIphcHeader;
    uint16_t        mPayloadOffset;
    Lowpan::Context mSrcContext;
    Lowpan::Context mDstContext;

    /**
     * General purpose fields.
     *
     */
    Payload     mPayload;
    Error       mError;
    const char *mTestName;
};

} // namespace ot

#endif // TEST_LOWPAN_HPP
