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
 *   This file includes definitions for MeshCoP.
 *
 */

#ifndef MESHCOP_HPP_
#define MESHCOP_HPP_

#include "openthread-core-config.h"

#include <limits.h>

#include <openthread/commissioner.h>
#include <openthread/instance.h>
#include <openthread/joiner.h>

#include "coap/coap.hpp"
#include "common/clearable.hpp"
#include "common/equatable.hpp"
#include "common/message.hpp"
#include "common/string.hpp"
#include "mac/mac_types.hpp"
#include "meshcop/meshcop_tlvs.hpp"

namespace ot {

class ThreadNetif;

namespace MeshCoP {

/**
 * This type represents a Joiner PSKd.
 *
 */
class JoinerPskd : public otJoinerPskd, public Clearable<JoinerPskd>, public Unequatable<JoinerPskd>
{
public:
    static constexpr uint8_t kMinLength = 6;                         ///< Min PSKd string length (excludes null char)
    static constexpr uint8_t kMaxLength = OT_JOINER_MAX_PSKD_LENGTH; ///< Max PSKd string length (excludes null char)

    /**
     * This method indicates whether the PSKd if well-formed and valid.
     *
     * Per Thread specification, a Joining Device Credential is encoded as uppercase alphanumeric characters
     * (base32-thread: 0-9, A-Z excluding I, O, Q, and Z for readability) with a minimum length of 6 such characters
     * and a maximum length of 32 such characters.
     *
     * @returns TRUE if the PSKd is valid, FALSE otherwise.
     *
     */
    bool IsValid(void) const { return IsPskdValid(m8); }

    /**
     * This method sets the joiner PSKd from a given C string.
     *
     * @param[in] aPskdString   A pointer to the PSKd C string array.
     *
     * @retval kErrorNone          The PSKd was updated successfully.
     * @retval kErrorInvalidArgs   The given PSKd C string is not valid.
     *
     */
    Error SetFrom(const char *aPskdString);

    /**
     * This method gets the PSKd as a null terminated C string.
     *
     * This method must be used after the PSKd is validated, otherwise its behavior is undefined.
     *
     * @returns The PSKd as a C string.
     *
     */
    const char *GetAsCString(void) const { return m8; }

    /**
     * This method gets the PSKd string length.
     *
     * This method must be used after the PSKd is validated, otherwise its behavior is undefined.
     *
     * @returns The PSKd string length.
     *
     */
    uint8_t GetLength(void) const { return static_cast<uint8_t>(StringLength(m8, kMaxLength + 1)); }

    /**
     * This method overloads operator `==` to evaluate whether or not two PSKds are equal.
     *
     * @param[in]  aOther  The other PSKd to compare with.
     *
     * @retval TRUE   If the two are equal.
     * @retval FALSE  If the two are not equal.
     *
     */
    bool operator==(const JoinerPskd &aOther) const;

    /**
     * This static method indicates whether a given PSKd string if well-formed and valid.
     *
     * @param[in] aPskdString  A pointer to a PSKd string array.
     *
     * @sa IsValid()
     *
     * @returns TRUE if @p aPskdString is valid, FALSE otherwise.
     *
     */
    static bool IsPskdValid(const char *aPskdString);
};

/**
 * This type represents a Joiner Discerner.
 *
 */
class JoinerDiscerner : public otJoinerDiscerner, public Unequatable<JoinerDiscerner>
{
    friend class SteeringData;

public:
    static constexpr uint8_t kMaxLength = OT_JOINER_MAX_DISCERNER_LENGTH; ///< Max length of a Discerner in bits.

    static constexpr uint16_t kInfoStringSize = 45; ///< Size of `InfoString` to use with `ToString()

    /**
     * This type defines the fixed-length `String` object returned from `ToString()`.
     *
     */
    typedef String<kInfoStringSize> InfoString;

    /**
     * This method clears the Joiner Discerner.
     *
     */
    void Clear(void) { mLength = 0; }

    /**
     * This method indicates whether the Joiner Discerner is empty (no value set).
     *
     * @returns TRUE if empty, FALSE otherwise.
     *
     */
    bool IsEmpty(void) const { return mLength == 0; }

    /**
     * This method gets the Joiner Discerner's value.
     *
     * @returns The Joiner Discerner value.
     *
     */
    uint64_t GetValue(void) const { return mValue; }

    /**
     * This method gets the Joiner Discerner's length (in bits).
     *
     * @return The Joiner Discerner length.
     *
     */
    uint8_t GetLength(void) const { return mLength; }

    /**
     * This method indicates whether the Joiner Discerner is valid (i.e. it not empty and its length is within
     * valid range).
     *
     * @returns TRUE if Joiner Discerner is valid, FALSE otherwise.
     *
     */
    bool IsValid(void) const { return (0 < mLength) && (mLength <= kMaxLength); }

    /**
     * This method generates a Joiner ID from the Discerner.
     *
     * @param[out] aJoinerId   A reference to `Mac::ExtAddress` to output the generated Joiner ID.
     *
     */
    void GenerateJoinerId(Mac::ExtAddress &aJoinerId) const;

    /**
     * This method indicates whether a given Joiner ID matches the Discerner.
     *
     * @param[in] aJoiner  A Joiner ID to match with the Discerner.
     *
     * @returns TRUE if the Joiner ID matches the Discerner, FALSE otherwise.
     *
     */
    bool Matches(const Mac::ExtAddress &aJoinerId) const;

    /**
     * This method overloads operator `==` to evaluate whether or not two Joiner Discerner instances are equal.
     *
     * @param[in]  aOther  The other Joiner Discerner to compare with.
     *
     * @retval TRUE   If the two are equal.
     * @retval FALSE  If the two are not equal.
     *
     */
    bool operator==(const JoinerDiscerner &aOther) const;

    /**
     * This method converts the Joiner Discerner to a string.
     *
     * @returns An `InfoString` representation of Joiner Discerner.
     *
     */
    InfoString ToString(void) const;

private:
    uint64_t GetMask(void) const { return (static_cast<uint64_t>(1ULL) << mLength) - 1; }
    void     CopyTo(Mac::ExtAddress &aExtAddress) const;
};

/**
 * This type represents Steering Data (bloom filter).
 *
 */
class SteeringData : public otSteeringData
{
public:
    static constexpr uint8_t kMaxLength = OT_STEERING_DATA_MAX_LENGTH; ///< Maximum Steering Data length (in bytes).

    /**
     * This structure represents the hash bit index values for the bloom filter calculated from a Joiner ID.
     *
     * The first hash bit index is derived using CRC16-CCITT and second one using CRC16-ANSI.
     *
     */
    struct HashBitIndexes
    {
        static constexpr uint8_t kNumIndexes = 2; ///< Number of hash bit indexes.

        uint16_t mIndex[kNumIndexes]; ///< The hash bit index array.
    };

    /**
     * This method initializes the Steering Data and clears the bloom filter.
     *
     * @param[in]  aLength   The Steering Data length (in bytes) - MUST be smaller than or equal to `kMaxLength`.
     *
     */
    void Init(uint8_t aLength = kMaxLength);

    /**
     * This method clears the bloom filter (all bits are cleared and no Joiner Id is accepted)..
     *
     * The Steering Data length (bloom filter length) is set to one byte with all bits cleared.
     *
     */
    void Clear(void) { Init(1); }

    /**
     * This method sets the bloom filter to permit all Joiner IDs.
     *
     * To permit all Joiner IDs, The Steering Data length (bloom filter length) is set to one byte with all bits set.
     *
     */
    void SetToPermitAllJoiners(void);

    /**
     * This method returns the Steering Data length (in bytes).
     *
     * @returns The Steering Data length (in bytes).
     *
     */
    uint8_t GetLength(void) const { return mLength; }

    /**
     * This method gets the Steering Data buffer (bloom filter).
     *
     * @returns A pointer to the Steering Data buffer.
     *
     */
    const uint8_t *GetData(void) const { return m8; }

    /**
     * This method gets the Steering Data buffer (bloom filter).
     *
     * @returns A pointer to the Steering Data buffer.
     *
     */
    uint8_t *GetData(void) { return m8; }

    /**
     * This method updates the bloom filter adding the given Joiner ID.
     *
     * @param[in]  aJoinerId  The Joiner ID to add to bloom filter.
     *
     */
    void UpdateBloomFilter(const Mac::ExtAddress &aJoinerId);

    /**
     * This method updates the bloom filter adding a given Joiner Discerner.
     *
     * @param[in]  aDiscerner  The Joiner Discerner to add to bloom filter.
     *
     */
    void UpdateBloomFilter(const JoinerDiscerner &aDiscerner);

    /**
     * This method indicates whether the bloom filter is empty (all the bits are cleared).
     *
     * @returns TRUE if the bloom filter is empty, FALSE otherwise.
     *
     */
    bool IsEmpty(void) const { return DoesAllMatch(0); }

    /**
     * This method indicates whether the bloom filter permits all Joiner IDs (all the bits are set).
     *
     * @returns TRUE if the bloom filter permits all Joiners IDs, FALSE otherwise.
     *
     */
    bool PermitsAllJoiners(void) const { return (mLength > 0) && DoesAllMatch(kPermitAll); }

    /**
     * This method indicates whether the bloom filter contains a given Joiner ID.
     *
     * @param[in] aJoinerId  A Joiner ID.
     *
     * @returns TRUE if the bloom filter contains @p aJoinerId, FALSE otherwise.
     *
     */
    bool Contains(const Mac::ExtAddress &aJoinerId) const;

    /**
     * This method indicates whether the bloom filter contains a given Joiner Discerner.
     *
     * @param[in] aDiscerner   A Joiner Discerner.
     *
     * @returns TRUE if the bloom filter contains @p aDiscerner, FALSE otherwise.
     *
     */
    bool Contains(const JoinerDiscerner &aDiscerner) const;

    /**
     * This method indicates whether the bloom filter contains the hash bit indexes (derived from a Joiner ID).
     *
     * @param[in]  aIndexes   A hash bit index structure (derived from a Joiner ID).
     *
     * @returns TRUE if the bloom filter contains the Joiner ID mapping to @p aIndexes, FALSE otherwise.
     *
     */
    bool Contains(const HashBitIndexes &aIndexes) const;

    /**
     * This static method calculates the bloom filter hash bit indexes from a given Joiner ID.
     *
     * The first hash bit index is derived using CRC16-CCITT and second one using CRC16-ANSI.
     *
     * @param[in]  aJoinerId  The Joiner ID to calculate the hash bit indexes.
     * @param[out] aIndexes   A reference to a `HashBitIndexes` structure to output the calculated index values.
     *
     */
    static void CalculateHashBitIndexes(const Mac::ExtAddress &aJoinerId, HashBitIndexes &aIndexes);

    /**
     * This static method calculates the bloom filter hash bit indexes from a given Joiner Discerner.
     *
     * The first hash bit index is derived using CRC16-CCITT and second one using CRC16-ANSI.
     *
     * @param[in]  aDiscerner     The Joiner Discerner to calculate the hash bit indexes.
     * @param[out] aIndexes       A reference to a `HashBitIndexes` structure to output the calculated index values.
     *
     */
    static void CalculateHashBitIndexes(const JoinerDiscerner &aDiscerner, HashBitIndexes &aIndexes);

private:
    static constexpr uint8_t kPermitAll = 0xff;

    uint8_t GetNumBits(void) const { return (mLength * CHAR_BIT); }

    uint8_t BitIndex(uint8_t aBit) const { return (mLength - 1 - (aBit / CHAR_BIT)); }
    uint8_t BitFlag(uint8_t aBit) const { return static_cast<uint8_t>(1U << (aBit % CHAR_BIT)); }

    bool GetBit(uint8_t aBit) const { return (m8[BitIndex(aBit)] & BitFlag(aBit)) != 0; }
    void SetBit(uint8_t aBit) { m8[BitIndex(aBit)] |= BitFlag(aBit); }
    void ClearBit(uint8_t aBit) { m8[BitIndex(aBit)] &= ~BitFlag(aBit); }

    bool DoesAllMatch(uint8_t aMatch) const;
    void UpdateBloomFilter(const HashBitIndexes &aIndexes);
};

/**
 * This function creates Message for MeshCoP.
 *
 */
inline Coap::Message *NewMeshCoPMessage(Coap::CoapBase &aCoap)
{
    return aCoap.NewMessage(Message::Settings(Message::kWithLinkSecurity, Message::kPriorityNet));
}

/**
 * This function generates PSKc.
 *
 * PSKc is used to establish the Commissioner Session.
 *
 * @param[in]  aPassPhrase   The commissioning passphrase.
 * @param[in]  aNetworkName  The network name for PSKc computation.
 * @param[in]  aExtPanId     The extended PAN ID for PSKc computation.
 * @param[out] aPskc         A reference to a PSKc where the generated PSKc will be placed.
 *
 * @retval kErrorNone          Successfully generate PSKc.
 * @retval kErrorInvalidArgs   If the length of passphrase is out of range.
 *
 */
Error GeneratePskc(const char *              aPassPhrase,
                   const Mac::NetworkName &  aNetworkName,
                   const Mac::ExtendedPanId &aExtPanId,
                   Pskc &                    aPskc);

/**
 * This function computes the Joiner ID from a factory-assigned IEEE EUI-64.
 *
 * @param[in]   aEui64     The factory-assigned IEEE EUI-64.
 * @param[out]  aJoinerId  The Joiner ID.
 *
 */
void ComputeJoinerId(const Mac::ExtAddress &aEui64, Mac::ExtAddress &aJoinerId);

/**
 * This function gets the border agent RLOC.
 *
 * @param[in]   aNetif  A reference to the thread interface.
 * @param[out]  aRloc   Border agent RLOC.
 *
 * @retval kErrorNone       Successfully got the Border Agent Rloc.
 * @retval kErrorNotFound   Border agent is not available.
 *
 */
Error GetBorderAgentRloc(ThreadNetif &aNetIf, uint16_t &aRloc);

#if (OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_WARN) && (OPENTHREAD_CONFIG_LOG_MESHCOP == 1)
/**
 * This function emits a log message indicating an error during a MeshCoP action.
 *
 * Note that log message is emitted only if there is an error, i.e. @p aError is not `kErrorNone`. The log
 * message will have the format "Failed to {aActionText} : {ErrorString}".
 *
 * @param[in] aActionText   A string representing the failed action.
 * @param[in] aError        The error in sending the message.
 *
 */
void LogError(const char *aActionText, Error aError);
#else
inline void LogError(const char *, Error)
{
}
#endif

} // namespace MeshCoP

} // namespace ot

#endif // MESHCOP_HPP_
