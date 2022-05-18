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
 *   This file includes definitions for generating and processing MLE TLVs.
 */

#ifndef TLVS_HPP_
#define TLVS_HPP_

#include "openthread-core-config.h"

#include <openthread/thread.h>
#include <openthread/platform/toolchain.h>

#include "common/encoding.hpp"
#include "common/error.hpp"
#include "common/type_traits.hpp"

namespace ot {

using ot::Encoding::BigEndian::HostSwap16;

class Message;

/**
 * This class implements TLV generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class Tlv
{
public:
    /**
     * The maximum length of the Base TLV format.
     *
     */
    static constexpr uint8_t kBaseTlvMaxLength = OT_NETWORK_BASE_TLV_MAX_LENGTH;

    /**
     * This method returns the Type value.
     *
     * @returns The Type value.
     *
     */
    uint8_t GetType(void) const { return mType; }

    /**
     * This method sets the Type value.
     *
     * @param[in]  aType  The Type value.
     *
     */
    void SetType(uint8_t aType) { mType = aType; }

    /**
     * This method indicates whether the TLV is an Extended TLV.
     *
     * @retval TRUE  If the TLV is an Extended TLV.
     * @retval FALSE If the TLV is not an Extended TLV.
     *
     */
    bool IsExtended(void) const { return (mLength == kExtendedLength); }

    /**
     * This method returns the Length value.
     *
     * @note This method should be used when TLV is not an Extended TLV, otherwise the returned length from this method
     * would not be correct. When TLV is an Extended TLV, the TLV should be down-casted to the `ExtendedTlv` type and
     * the `ExtendedTlv::GetLength()` should be used instead.
     *
     * @returns The Length value.
     *
     */
    uint8_t GetLength(void) const { return mLength; }

    /**
     * This method sets the Length value.
     *
     * @param[in]  aLength  The Length value.
     *
     */
    void SetLength(uint8_t aLength) { mLength = aLength; }

    /**
     * This method returns the TLV's total size (number of bytes) including Type, Length, and Value fields.
     *
     * This method correctly returns the TLV size independent of whether the TLV is an Extended TLV or not.
     *
     * @returns The total size include Type, Length, and Value fields.
     *
     */
    uint32_t GetSize(void) const;

    /**
     * This method returns a pointer to the Value.
     *
     * This method can be used independent of whether the TLV is an Extended TLV or not.
     *
     * @returns A pointer to the value.
     *
     */
    uint8_t *GetValue(void);

    /**
     * This method returns a pointer to the Value.
     *
     * This method can be used independent of whether the TLV is an Extended TLV or not.
     *
     * @returns A pointer to the value.
     *
     */
    const uint8_t *GetValue(void) const;

    /**
     * This method returns a pointer to the next TLV.
     *
     * This method correctly returns the next TLV independent of whether the current TLV is an Extended TLV or not.
     *
     * @returns A pointer to the next TLV.
     *
     */
    Tlv *GetNext(void) { return reinterpret_cast<Tlv *>(reinterpret_cast<uint8_t *>(this) + GetSize()); }

    /**
     * This method returns a pointer to the next TLV.
     *
     * This method correctly returns the next TLV independent of whether the current TLV is an Extended TLV or not.
     *
     * @returns A pointer to the next TLV.
     *
     */
    const Tlv *GetNext(void) const
    {
        return reinterpret_cast<const Tlv *>(reinterpret_cast<const uint8_t *>(this) + GetSize());
    }

    /**
     * This method appends a TLV to the end of the message.
     *
     * On success, this method grows the message by the size of the TLV.
     *
     * @param[in]  aMessage      A reference to the message to append to.
     *
     * @retval kErrorNone     Successfully appended the TLV to the message.
     * @retval kErrorNoBufs   Insufficient available buffers to grow the message.
     *
     */
    Error AppendTo(Message &aMessage) const;

    /**
     * This static method reads a TLV in a message at a given offset expecting a minimum length for the value.
     *
     * @param[in]   aMessage    The message to read from.
     * @param[in]   aOffset     The offset into the message pointing to the start of the TLV.
     * @param[out]  aValue      A buffer to output the TLV's value, must contain (at least) @p aMinLength bytes.
     * @param[in]   aMinLength  The minimum expected length of TLV and number of bytes to copy into @p aValue buffer.
     *
     * @retval kErrorNone        Successfully read the TLV and copied @p aMinLength into @p aValue.
     * @retval kErrorParse       The TLV was not well-formed and could not be parsed.
     *
     */
    static Error ReadTlv(const Message &aMessage, uint16_t aOffset, void *aValue, uint8_t aMinLength);

    /**
     * This static method reads a simple TLV with a single non-integral value in a message at a given offset.
     *
     * @tparam      SimpleTlvType   The simple TLV type to read (must be a sub-class of `SimpleTlvInfo`).
     *
     * @param[in]   aMessage        The message to read from.
     * @param[in]   aOffset         The offset into the message pointing to the start of the TLV.
     * @param[out]  aValue          A reference to the value object to output the read value.
     *
     * @retval kErrorNone        Successfully read the TLV and updated the @p aValue.
     * @retval kErrorParse       The TLV was not well-formed and could not be parsed.
     *
     */
    template <typename SimpleTlvType>
    static Error Read(const Message &aMessage, uint16_t aOffset, typename SimpleTlvType::ValueType &aValue)
    {
        return ReadTlv(aMessage, aOffset, &aValue, sizeof(aValue));
    }

    /**
     * This static method reads a simple TLV with a single integral value in a message at a given offset.
     *
     * @tparam      UintTlvType     The simple TLV type to read (must be a sub-class of `UintTlvInfo`).
     *
     * @param[in]   aMessage        The message to read from.
     * @param[in]   aOffset         The offset into the message pointing to the start of the TLV.
     * @param[out]  aValue          A reference to an unsigned int to output the read value.
     *
     * @retval kErrorNone        Successfully read the TLV and updated the @p aValue.
     * @retval kErrorParse       The TLV was not well-formed and could not be parsed.
     *
     */
    template <typename UintTlvType>
    static Error Read(const Message &aMessage, uint16_t aOffset, typename UintTlvType::UintValueType &aValue)
    {
        return ReadUintTlv(aMessage, aOffset, aValue);
    }

    /**
     * This static method searches for and reads a requested TLV out of a given message.
     *
     * This method can be used independent of whether the read TLV (from message) is an Extended TLV or not.
     *
     * @param[in]   aMessage    A reference to the message.
     * @param[in]   aType       The Type value to search for.
     * @param[in]   aMaxSize    Maximum number of bytes to read.
     * @param[out]  aTlv        A reference to the TLV that will be copied to.
     *
     * @retval kErrorNone       Successfully copied the TLV.
     * @retval kErrorNotFound   Could not find the TLV with Type @p aType.
     *
     */
    static Error FindTlv(const Message &aMessage, uint8_t aType, uint16_t aMaxSize, Tlv &aTlv);

    /**
     * This static method searches for and reads a requested TLV out of a given message.
     *
     * This method can be used independent of whether the read TLV (from message) is an Extended TLV or not.
     *
     * @tparam      TlvType     The TlvType to search for (must be a sub-class of `Tlv`).
     *
     * @param[in]   aMessage    A reference to the message.
     * @param[out]  aTlv        A reference to the TLV that will be copied to.
     *
     * @retval kErrorNone       Successfully copied the TLV.
     * @retval kErrorNotFound   Could not find the TLV with Type @p aType.
     *
     */
    template <typename TlvType> static Error FindTlv(const Message &aMessage, TlvType &aTlv)
    {
        return FindTlv(aMessage, TlvType::kType, sizeof(TlvType), aTlv);
    }

    /**
     * This static method obtains the offset of a TLV within @p aMessage.
     *
     * This method can be used independent of whether the read TLV (from message) is an Extended TLV or not.
     *
     * @param[in]   aMessage    A reference to the message.
     * @param[in]   aType       The Type value to search for.
     * @param[out]  aOffset     A reference to the offset of the TLV.
     *
     * @retval kErrorNone       Successfully copied the TLV.
     * @retval kErrorNotFound   Could not find the TLV with Type @p aType.
     *
     */
    static Error FindTlvOffset(const Message &aMessage, uint8_t aType, uint16_t &aOffset);

    /**
     * This static method finds the offset and length of a given TLV type.
     *
     * This method can be used independent of whether the read TLV (from message) is an Extended TLV or not.
     *
     * @param[in]   aMessage      A reference to the message.
     * @param[in]   aType         The Type value to search for.
     * @param[out]  aValueOffset  The offset where the value starts.
     * @param[out]  aLength       The length of the value.
     *
     * @retval kErrorNone       Successfully found the TLV.
     * @retval kErrorNotFound   Could not find the TLV with Type @p aType.
     *
     */
    static Error FindTlvValueOffset(const Message &aMessage, uint8_t aType, uint16_t &aValueOffset, uint16_t &aLength);

    /**
     * This static method searches for a TLV with a given type in a message, ensures its length is same or larger than
     * an expected minimum value, and then reads its value into a given buffer.
     *
     * If the TLV length is smaller than the minimum length @p aLength, the TLV is considered invalid. In this case,
     * this method returns `kErrorParse` and the @p aValue buffer is not updated.
     *
     * If the TLV length is larger than @p aLength, the TLV is considered valid, but only the first @p aLength bytes
     * of the value are read and copied into the @p aValue buffer.
     *
     * @tparam       TlvType     The TLV type to find.
     *
     * @param[in]    aMessage    A reference to the message.
     * @param[out]   aValue      A buffer to output the value (must contain at least @p aLength bytes).
     * @param[in]    aLength     The expected (minimum) length of the TLV value.
     *
     * @retval kErrorNone       The TLV was found and read successfully. @p aValue is updated.
     * @retval kErrorNotFound   Could not find the TLV with Type @p aType.
     * @retval kErrorParse      TLV was found but it was not well-formed and could not be parsed.
     *
     */
    template <typename TlvType> static Error Find(const Message &aMessage, void *aValue, uint8_t aLength)
    {
        return FindTlv(aMessage, TlvType::kType, aValue, aLength);
    }

    /**
     * This static method searches for a simple TLV with a single non-integral value in a message, ensures its length is
     * same or larger than the expected `ValueType` object size, and then reads its value into a value object reference.
     *
     * If the TLV length is smaller than the size of @p aValue, the TLV is considered invalid. In this case, this
     * method returns `kErrorParse` and the @p aValue is not updated.
     *
     * If the TLV length is larger than the size of @p aValue, the TLV is considered valid, but the size of
     * `ValueType` bytes are read and copied into the @p aValue.
     *
     * @tparam       SimpleTlvType   The simple TLV type to find (must be a sub-class of `SimpleTlvInfo`)
     *
     * @param[in]    aMessage        A reference to the message.
     * @param[in]    aType           The TLV type to search for.
     * @param[out]   aValue          A reference to the value object to output the read value.
     *
     * @retval kErrorNone         The TLV was found and read successfully. @p aValue is updated.
     * @retval kErrorNotFound     Could not find the TLV with Type @p aType.
     * @retval kErrorParse        TLV was found but it was not well-formed and could not be parsed.
     *
     */
    template <typename SimpleTlvType>
    static Error Find(const Message &aMessage, typename SimpleTlvType::ValueType &aValue)
    {
        return FindTlv(aMessage, SimpleTlvType::kType, &aValue, sizeof(aValue));
    }

    /**
     * This static method searches for a simple TLV with a single integral value in a message, and then reads its value
     * into a given `uint` reference variable.
     *
     * If the TLV length is smaller than size of integral value, the TLV is considered invalid. In this case, this
     * method returns `kErrorParse` and the @p aValue is not updated.
     *
     * @tparam       UintTlvType     The simple TLV type to find (must be a sub-class of `UintTlvInfo`)
     *
     * @param[in]    aMessage        A reference to the message.
     * @param[out]   aValue          A reference to an unsigned int value to output the TLV's value.
     *
     * @retval kErrorNone         The TLV was found and read successfully. @p aValue is updated.
     * @retval kErrorNotFound     Could not find the TLV with Type @p aType.
     * @retval kErrorParse        TLV was found but it was not well-formed and could not be parsed.
     *
     */
    template <typename UintTlvType>
    static Error Find(const Message &aMessage, typename UintTlvType::UintValueType &aValue)
    {
        return FindUintTlv(aMessage, UintTlvType::kType, aValue);
    }

    /**
     * This static method appends a TLV with a given type and value to a message.
     *
     * On success this method grows the message by the size of the TLV.
     *
     * @tparam     TlvType       The TLV type to append.
     *
     * @param[in]  aMessage      A reference to the message to append to.
     * @param[in]  aValue        A buffer containing the TLV value.
     * @param[in]  aLength       The value length (in bytes).
     *
     * @retval kErrorNone     Successfully appended the TLV to the message.
     * @retval kErrorNoBufs   Insufficient available buffers to grow the message.
     *
     */
    template <typename TlvType> static Error Append(Message &aMessage, const void *aValue, uint8_t aLength)
    {
        return AppendTlv(aMessage, TlvType::kType, aValue, aLength);
    }

    /**
     * This static method appends a simple TLV with a single (non-integral) value to a message.
     *
     * On success this method grows the message by the size of the TLV.
     *
     * @tparam     SimpleTlvType The simple TLV type to append (must be a sub-class of `SimpleTlvInfo`)
     *
     * @param[in]  aMessage      A reference to the message to append to.
     * @param[in]  aValue        A reference to the object containing TLV's value.
     *
     * @retval kErrorNone     Successfully appended the TLV to the message.
     * @retval kErrorNoBufs   Insufficient available buffers to grow the message.
     *
     */
    template <typename SimpleTlvType>
    static Error Append(Message &aMessage, const typename SimpleTlvType::ValueType &aValue)
    {
        return AppendTlv(aMessage, SimpleTlvType::kType, &aValue, sizeof(aValue));
    }

    /**
     * This static method appends a simple TLV with a single integral value to a message.
     *
     * On success this method grows the message by the size of the TLV.
     *
     * @tparam     UintTlvType   The simple TLV type to append (must be a sub-class of `UintTlvInfo`)
     *
     * @param[in]  aMessage      A reference to the message to append to.
     * @param[in]  aValue        An unsigned int value to use as TLV's value.
     *
     * @retval kErrorNone     Successfully appended the TLV to the message.
     * @retval kErrorNoBufs   Insufficient available buffers to grow the message.
     *
     */
    template <typename UintTlvType> static Error Append(Message &aMessage, typename UintTlvType::UintValueType aValue)
    {
        return AppendUintTlv(aMessage, UintTlvType::kType, aValue);
    }

protected:
    static const uint8_t kExtendedLength = 255; // Extended Length value.

private:
    static Error Find(const Message &aMessage, uint8_t aType, uint16_t *aOffset, uint16_t *aSize, bool *aIsExtendedTlv);
    static Error FindTlv(const Message &aMessage, uint8_t aType, void *aValue, uint8_t aLength);
    static Error AppendTlv(Message &aMessage, uint8_t aType, const void *aValue, uint8_t aLength);
    template <typename UintType> static Error ReadUintTlv(const Message &aMessage, uint16_t aOffset, UintType &aValue);
    template <typename UintType> static Error FindUintTlv(const Message &aMessage, uint8_t aType, UintType &aValue);
    template <typename UintType> static Error AppendUintTlv(Message &aMessage, uint8_t aType, UintType aValue);

    uint8_t mType;
    uint8_t mLength;
} OT_TOOL_PACKED_END;

OT_TOOL_PACKED_BEGIN
class ExtendedTlv : public Tlv
{
public:
    /**
     * This method returns the Length value.
     *
     */
    uint16_t GetLength(void) const { return HostSwap16(mLength); }

    /**
     * This method sets the Length value.
     *
     * @param[in]  aLength  The Length value.
     *
     */
    void SetLength(uint16_t aLength)
    {
        Tlv::SetLength(kExtendedLength);
        mLength = HostSwap16(aLength);
    }

private:
    uint16_t mLength;
} OT_TOOL_PACKED_END;

/**
 * This class defines constants for a TLV.
 *
 * @tparam kTlvTypeValue   The TLV Type value.
 *
 */
template <uint8_t kTlvTypeValue> class TlvInfo
{
public:
    static constexpr uint8_t kType = kTlvTypeValue; ///< The TLV Type value.
};

/**
 * This class defines constants and types for a simple TLV with an unsigned int value type.
 *
 * This class and its sub-classes are intended to be used as the template type in `Tlv::Append<UintTlvType>()`, and
 * the related `Tlv::Find<UintTlvType>()` and `Tlv::Read<UintTlvType>()`.
 *
 * @tparam kTlvTypeValue   The TLV Type value.
 * @tparam UintType        The TLV Value's type (must be an unsigned int, i.e. uint8_t, uint16_t, or uint32_t).
 *
 */
template <uint8_t kTlvTypeValue, typename UintType> class UintTlvInfo : public TlvInfo<kTlvTypeValue>
{
public:
    static_assert(TypeTraits::IsSame<UintType, uint8_t>::kValue || TypeTraits::IsSame<UintType, uint16_t>::kValue ||
                      TypeTraits::IsSame<UintType, uint32_t>::kValue,
                  "UintTlv must be used used with unsigned int value type");

    typedef UintType UintValueType; ///< The TLV Value unsigned int type.
};

/**
 * This class defines constants and types for a simple TLV with a single value.
 *
 * This class and its sub-classes are intended to be used as the template type in `Tlv::Append<SimpleTlvType>()`,
 * and the related `Tlv::Find<SimpleTlvType>()` and `Tlv::Read<SimpleTlvType>()`.
 *
 * @tparam kTlvTypeValue   The TLV Type value.
 * @tparam TlvValueType    The TLV Value's type (must not be an integral type).
 *
 */
template <uint8_t kTlvTypeValue, typename TlvValueType> class SimpleTlvInfo : public TlvInfo<kTlvTypeValue>
{
public:
    static_assert(!TypeTraits::IsPointer<TlvValueType>::kValue, "TlvValueType must not be a pointer");
    static_assert(!TypeTraits::IsSame<TlvValueType, uint8_t>::kValue, "SimpleTlv must not use int value type");
    static_assert(!TypeTraits::IsSame<TlvValueType, uint16_t>::kValue, "SimpleTlv must not use int value type");
    static_assert(!TypeTraits::IsSame<TlvValueType, uint32_t>::kValue, "SimpleTlv must not use int value type");
    static_assert(!TypeTraits::IsSame<TlvValueType, int8_t>::kValue, "SimpleTlv must not use int value type");
    static_assert(!TypeTraits::IsSame<TlvValueType, int16_t>::kValue, "SimpleTlv must not use int value type");
    static_assert(!TypeTraits::IsSame<TlvValueType, int32_t>::kValue, "SimpleTlv must not use int value type");

    typedef TlvValueType ValueType; ///< The TLV Value type.
};

} // namespace ot

#endif // TLVS_HPP_
