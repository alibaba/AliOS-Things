/*
 *  Copyright (c) 2016-2019, The OpenThread Authors.
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
 *   This file includes definitions for MAC types such as Address, Extended PAN Identifier, Network Name, etc.
 */

#ifndef MAC_TYPES_HPP_
#define MAC_TYPES_HPP_

#include "openthread-core-config.h"

#include <stdint.h>
#include <string.h>

#include <openthread/link.h>
#include <openthread/thread.h>

#include "common/clearable.hpp"
#include "common/data.hpp"
#include "common/equatable.hpp"
#include "common/string.hpp"
#include "crypto/storage.hpp"

namespace ot {
namespace Mac {

/**
 * @addtogroup core-mac
 *
 * @{
 *
 */

/**
 * This type represents the IEEE 802.15.4 PAN ID.
 *
 */
typedef otPanId PanId;

constexpr PanId kPanIdBroadcast = 0xffff; ///< Broadcast PAN ID.

/**
 * This type represents the IEEE 802.15.4 Short Address.
 *
 */
typedef otShortAddress ShortAddress;

constexpr ShortAddress kShortAddrBroadcast = 0xffff; ///< Broadcast Short Address.
constexpr ShortAddress kShortAddrInvalid   = 0xfffe; ///< Invalid Short Address.

/**
 * This function generates a random IEEE 802.15.4 PAN ID.
 *
 * @returns A randomly generated IEEE 802.15.4 PAN ID (excluding `kPanIdBroadcast`).
 *
 */
PanId GenerateRandomPanId(void);

/**
 * This structure represents an IEEE 802.15.4 Extended Address.
 *
 */
OT_TOOL_PACKED_BEGIN
class ExtAddress : public otExtAddress, public Equatable<ExtAddress>, public Clearable<ExtAddress>
{
public:
    static constexpr uint16_t kInfoStringSize = 17; ///< Max chars for the info string (`ToString()`).

    /**
     * This type defines the fixed-length `String` object returned from `ToString()`.
     *
     */
    typedef String<kInfoStringSize> InfoString;

    /**
     * This enumeration type specifies the copy byte order when Extended Address is being copied to/from a buffer.
     *
     */
    enum CopyByteOrder : uint8_t
    {
        kNormalByteOrder,  ///< Copy address bytes in normal order (as provided in array buffer).
        kReverseByteOrder, ///< Copy address bytes in reverse byte order.
    };

    /**
     * This method fills all bytes of address with a given byte value.
     *
     * @param[in] aByte A byte value to fill address with.
     *
     */
    void Fill(uint8_t aByte) { memset(this, aByte, sizeof(*this)); }

    /**
     * This method generates a random IEEE 802.15.4 Extended Address.
     *
     */
    void GenerateRandom(void);

    /**
     * This method sets the Extended Address from a given byte array.
     *
     * @param[in] aBuffer    Pointer to an array containing the Extended Address. `OT_EXT_ADDRESS_SIZE` bytes from
     *                       buffer are copied to form the Extended Address.
     * @param[in] aByteOrder The byte order to use when copying the address.
     *
     */
    void Set(const uint8_t *aBuffer, CopyByteOrder aByteOrder = kNormalByteOrder)
    {
        CopyAddress(m8, aBuffer, aByteOrder);
    }

    /**
     * This method indicates whether or not the Group bit is set.
     *
     * @retval TRUE   If the group bit is set.
     * @retval FALSE  If the group bit is not set.
     *
     */
    bool IsGroup(void) const { return (m8[0] & kGroupFlag) != 0; }

    /**
     * This method sets the Group bit.
     *
     * @param[in]  aGroup  TRUE if group address, FALSE otherwise.
     *
     */
    void SetGroup(bool aGroup)
    {
        if (aGroup)
        {
            m8[0] |= kGroupFlag;
        }
        else
        {
            m8[0] &= ~kGroupFlag;
        }
    }

    /**
     * This method toggles the Group bit.
     *
     */
    void ToggleGroup(void) { m8[0] ^= kGroupFlag; }

    /**
     * This method indicates whether or not the Local bit is set.
     *
     * @retval TRUE   If the local bit is set.
     * @retval FALSE  If the local bit is not set.
     *
     */
    bool IsLocal(void) const { return (m8[0] & kLocalFlag) != 0; }

    /**
     * This method sets the Local bit.
     *
     * @param[in]  aLocal  TRUE if locally administered, FALSE otherwise.
     *
     */
    void SetLocal(bool aLocal)
    {
        if (aLocal)
        {
            m8[0] |= kLocalFlag;
        }
        else
        {
            m8[0] &= ~kLocalFlag;
        }
    }

    /**
     * This method toggles the Local bit.
     *
     */
    void ToggleLocal(void) { m8[0] ^= kLocalFlag; }

    /**
     * This method copies the Extended Address into a given buffer.
     *
     * @param[out] aBuffer     A pointer to a buffer to copy the Extended Address into.
     * @param[in]  aByteOrder  The byte order to copy the address.
     *
     */
    void CopyTo(uint8_t *aBuffer, CopyByteOrder aByteOrder = kNormalByteOrder) const
    {
        CopyAddress(aBuffer, m8, aByteOrder);
    }

    /**
     * This method converts an address to a string.
     *
     * @returns An `InfoString` containing the string representation of the Extended Address.
     *
     */
    InfoString ToString(void) const;

private:
    static constexpr uint8_t kGroupFlag = (1 << 0);
    static constexpr uint8_t kLocalFlag = (1 << 1);

    static void CopyAddress(uint8_t *aDst, const uint8_t *aSrc, CopyByteOrder aByteOrder);
} OT_TOOL_PACKED_END;

/**
 * This class represents an IEEE 802.15.4 Short or Extended Address.
 *
 */
class Address
{
public:
    /**
     * This type defines the fixed-length `String` object returned from `ToString()`.
     *
     */
    typedef ExtAddress::InfoString InfoString;

    /**
     * This enumeration specifies the IEEE 802.15.4 Address type.
     *
     */
    enum Type : uint8_t
    {
        kTypeNone,     ///< No address.
        kTypeShort,    ///< IEEE 802.15.4 Short Address.
        kTypeExtended, ///< IEEE 802.15.4 Extended Address.
    };

    /**
     * This constructor initializes an Address.
     *
     */
    Address(void)
        : mType(kTypeNone)
    {
    }

    /**
     * This method gets the address type (Short Address, Extended Address, or none).
     *
     * @returns The address type.
     *
     */
    Type GetType(void) const { return mType; }

    /**
     * This method indicates whether or not there is an address.
     *
     * @returns TRUE if there is no address (i.e. address type is `kTypeNone`), FALSE otherwise.
     *
     */
    bool IsNone(void) const { return (mType == kTypeNone); }

    /**
     * This method indicates whether or not the Address is a Short Address.
     *
     * @returns TRUE if it is a Short Address, FALSE otherwise.
     *
     */
    bool IsShort(void) const { return (mType == kTypeShort); }

    /**
     * This method indicates whether or not the Address is an Extended Address.
     *
     * @returns TRUE if it is an Extended Address, FALSE otherwise.
     *
     */
    bool IsExtended(void) const { return (mType == kTypeExtended); }

    /**
     * This method gets the address as a Short Address.
     *
     * This method MUST be used only if the address type is Short Address.
     *
     * @returns The Short Address.
     *
     */
    ShortAddress GetShort(void) const { return mShared.mShortAddress; }

    /**
     * This method gets the address as an Extended Address.
     *
     * This method MUST be used only if the address type is Extended Address.
     *
     * @returns A constant reference to the Extended Address.
     *
     */
    const ExtAddress &GetExtended(void) const { return mShared.mExtAddress; }

    /**
     * This method gets the address as an Extended Address.
     *
     * This method MUST be used only if the address type is Extended Address.
     *
     * @returns A reference to the Extended Address.
     *
     */
    ExtAddress &GetExtended(void) { return mShared.mExtAddress; }

    /**
     * This method sets the address to none (i.e., clears the address).
     *
     * Address type will be updated to `kTypeNone`.
     *
     */
    void SetNone(void) { mType = kTypeNone; }

    /**
     * This method sets the address with a Short Address.
     *
     * The type is also updated to indicate that address is Short.
     *
     * @param[in]  aShortAddress  A Short Address
     *
     */
    void SetShort(ShortAddress aShortAddress)
    {
        mShared.mShortAddress = aShortAddress;
        mType                 = kTypeShort;
    }

    /**
     * This method sets the address with an Extended Address.
     *
     * The type is also updated to indicate that the address is Extended.
     *
     * @param[in]  aExtAddress  An Extended Address
     *
     */
    void SetExtended(const ExtAddress &aExtAddress)
    {
        mShared.mExtAddress = aExtAddress;
        mType               = kTypeExtended;
    }

    /**
     * This method sets the address with an Extended Address given as a byte array.
     *
     * The type is also updated to indicate that the address is Extended.
     *
     * @param[in] aBuffer    Pointer to an array containing the Extended Address. `OT_EXT_ADDRESS_SIZE` bytes from
     *                       buffer are copied to form the Extended Address.
     * @param[in] aByteOrder The byte order to copy the address from @p aBuffer.
     *
     */
    void SetExtended(const uint8_t *aBuffer, ExtAddress::CopyByteOrder aByteOrder = ExtAddress::kNormalByteOrder)
    {
        mShared.mExtAddress.Set(aBuffer, aByteOrder);
        mType = kTypeExtended;
    }

    /**
     * This method indicates whether or not the address is a Short Broadcast Address.
     *
     * @returns TRUE if address is Short Broadcast Address, FALSE otherwise.
     *
     */
    bool IsBroadcast(void) const { return ((mType == kTypeShort) && (GetShort() == kShortAddrBroadcast)); }

    /**
     * This method indicates whether or not the address is a Short Invalid Address.
     *
     * @returns TRUE if address is Short Invalid Address, FALSE otherwise.
     *
     */
    bool IsShortAddrInvalid(void) const { return ((mType == kTypeShort) && (GetShort() == kShortAddrInvalid)); }

    /**
     * This method converts an address to a null-terminated string
     *
     * @returns A `String` representing the address.
     *
     */
    InfoString ToString(void) const;

private:
    union
    {
        ShortAddress mShortAddress; ///< The IEEE 802.15.4 Short Address.
        ExtAddress   mExtAddress;   ///< The IEEE 802.15.4 Extended Address.
    } mShared;

    Type mType; ///< The address type (Short, Extended, or none).
};

/**
 * This class represents a MAC key.
 *
 */
OT_TOOL_PACKED_BEGIN
class Key : public otMacKey, public Equatable<Key>, public Clearable<Key>
{
public:
    static constexpr uint16_t kSize = OT_MAC_KEY_SIZE; ///< Key size in bytes.

    /**
     * This method gets a pointer to the bytes array containing the key
     *
     * @returns A pointer to the byte array containing the key.
     *
     */
    const uint8_t *GetBytes(void) const { return m8; }

} OT_TOOL_PACKED_END;

/**
 * This type represents a MAC Key Ref used by PSA.
 *
 */
typedef otMacKeyRef KeyRef;

/**
 * This class represents a MAC Key Material.
 *
 */
class KeyMaterial : public otMacKeyMaterial, public Unequatable<KeyMaterial>
{
public:
    /**
     * This constructor initializes a `KeyMaterial`.
     *
     */
    KeyMaterial(void)
    {
        GetKey().Clear();
#if OPENTHREAD_CONFIG_PLATFORM_KEY_REFERENCES_ENABLE
        SetKeyRef(kInvalidKeyRef);
#endif
    }

#if OPENTHREAD_CONFIG_PLATFORM_KEY_REFERENCES_ENABLE
    /**
     * This method overload `=` operator to assign the `KeyMaterial` from another one.
     *
     * If the `KeyMaterial` currently stores a valid and different `KeyRef`, the assignment of new value will ensure to
     * delete the previous one before using the new `KeyRef` from @p aOther.
     *
     * @param[in] aOther  aOther  The other `KeyMaterial` instance to assign from.
     *
     * @returns A reference to the current `KeyMaterial`
     *
     */
    KeyMaterial &operator=(const KeyMaterial &aOther);
#endif

    /**
     *  This method clears the `KeyMaterial`.
     *
     * Under `OPENTHREAD_CONFIG_PLATFORM_KEY_REFERENCES_ENABLE`, if the `KeyMaterial` currently stores a valid previous
     * `KeyRef`, the `Clear()` call will ensure to delete the previous `KeyRef` and set it to `kInvalidKeyRef`.
     *
     */
    void Clear(void);

#if !OPENTHREAD_CONFIG_PLATFORM_KEY_REFERENCES_ENABLE
    /**
     * This method gets the literal `Key`.
     *
     * @returns The literal `Key`
     *
     */
    const Key &GetKey(void) const { return static_cast<const Key &>(mKeyMaterial.mKey); }

#else
    /**
     * This method gets the stored `KeyRef`
     *
     * @returns The `KeyRef`
     *
     */
    KeyRef GetKeyRef(void) const { return mKeyMaterial.mKeyRef; }
#endif

    /**
     * This method sets the `KeyMaterial` from a given Key.
     *
     * If the `KeyMaterial` currently stores a valid `KeyRef`, the `SetFrom()` call will ensure to delete the previous
     * one before creating and using a new `KeyRef` associated with the new `Key`.
     *
     * @param[in] aKey           A reference to the new key.
     * @param[in] aIsExportable  Boolean indicating if the key is exportable (this is only applicable under
     *                           `OPENTHREAD_CONFIG_PLATFORM_KEY_REFERENCES_ENABLE` config).
     *
     */
    void SetFrom(const Key &aKey, bool aIsExportable = false);

    /**
     * This method extracts the literal key from `KeyMaterial`
     *
     * @param[out] aKey  A reference to the output the key.
     *
     */
    void ExtractKey(Key &aKey);

    /**
     * This method converts `KeyMaterial` to a `Crypto::Key`.
     *
     * @param[out]  A reference to a `Crypto::Key` to populate.
     *
     */
    void ConvertToCryptoKey(Crypto::Key &aCryptoKey) const;

    /**
     * This method overloads operator `==` to evaluate whether or not two `KeyMaterial` instances are equal.
     *
     * @param[in]  aOther  The other `KeyMaterial` instance to compare with.
     *
     * @retval TRUE   If the two `KeyMaterial` instances are equal.
     * @retval FALSE  If the two `KeyMaterial` instances are not equal.
     *
     */
    bool operator==(const KeyMaterial &aOther) const;

    KeyMaterial(const KeyMaterial &) = delete;

private:
#if OPENTHREAD_CONFIG_PLATFORM_KEY_REFERENCES_ENABLE
    static constexpr KeyRef kInvalidKeyRef = Crypto::Storage::kInvalidKeyRef;

    void DestroyKey(void);
    void SetKeyRef(KeyRef aKeyRef) { mKeyMaterial.mKeyRef = aKeyRef; }
#endif
    Key &GetKey(void) { return static_cast<Key &>(mKeyMaterial.mKey); }
    void SetKey(const Key &aKey) { mKeyMaterial.mKey = aKey; }
};

/**
 * This structure represents an IEEE 802.15.4 Extended PAN Identifier.
 *
 */
OT_TOOL_PACKED_BEGIN
class ExtendedPanId : public otExtendedPanId, public Equatable<ExtendedPanId>, public Clearable<ExtendedPanId>
{
public:
    static constexpr uint16_t kInfoStringSize = 17; ///< Max chars for the info string (`ToString()`).

    /**
     * This type defines the fixed-length `String` object returned from `ToString()`.
     *
     */
    typedef String<kInfoStringSize> InfoString;

    /**
     * This method converts an address to a string.
     *
     * @returns An `InfoString` containing the string representation of the Extended PAN Identifier.
     *
     */
    InfoString ToString(void) const;

} OT_TOOL_PACKED_END;

/**
 * This class represents a name string as data (pointer to a char buffer along with a length).
 *
 * @note The char array does NOT need to be null terminated.
 *
 */
class NameData : private Data<kWithUint8Length>
{
    friend class NetworkName;

public:
    /**
     * This constructor initializes the NameData object.
     *
     * @param[in] aBuffer   A pointer to a `char` buffer (does not need to be null terminated).
     * @param[in] aLength   The length (number of chars) in the buffer.
     *
     */
    NameData(const char *aBuffer, uint8_t aLength) { Init(aBuffer, aLength); }

    /**
     * This method returns the pointer to char buffer (not necessarily null terminated).
     *
     * @returns The pointer to the char buffer.
     *
     */
    const char *GetBuffer(void) const { return reinterpret_cast<const char *>(GetBytes()); }

    /**
     * This method returns the length (number of chars in buffer).
     *
     * @returns The name length.
     *
     */
    uint8_t GetLength(void) const { return Data<kWithUint8Length>::GetLength(); }

    /**
     * This method copies the name data into a given char buffer with a given size.
     *
     * The given buffer is cleared (`memset` to zero) before copying the name into it. The copied string
     * in @p aBuffer is NOT necessarily null terminated.
     *
     * @param[out] aBuffer   A pointer to a buffer where to copy the name into.
     * @param[in]  aMaxSize  Size of @p aBuffer (maximum number of chars to write into @p aBuffer).
     *
     * @returns The actual number of chars copied into @p aBuffer.
     *
     */
    uint8_t CopyTo(char *aBuffer, uint8_t aMaxSize) const;
};

/**
 * This structure represents an IEEE802.15.4 Network Name.
 *
 */
class NetworkName : public otNetworkName, public Unequatable<NetworkName>
{
public:
    /**
     * This constant specified the maximum number of chars in Network Name (excludes null char).
     *
     */
    static constexpr uint8_t kMaxSize = OT_NETWORK_NAME_MAX_SIZE;

    /**
     * This constructor initializes the IEEE802.15.4 Network Name as an empty string.
     *
     */
    NetworkName(void) { m8[0] = '\0'; }

    /**
     * This method gets the IEEE802.15.4 Network Name as a null terminated C string.
     *
     * @returns The Network Name as a null terminated C string array.
     *
     */
    const char *GetAsCString(void) const { return m8; }

    /**
     * This method gets the IEEE802.15.4 Network Name as NameData.
     *
     * @returns The Network Name as NameData.
     *
     */
    NameData GetAsData(void) const;

    /**
     * This method sets the IEEE 802.15.4 Network Name from a given null terminated C string.
     *
     * This method also validates that the given @p aNameString follows UTF-8 encoding and can fit in `kMaxSize`
     * chars.
     *
     * @param[in] aNameString      A name C string.
     *
     * @retval kErrorNone          Successfully set the IEEE 802.15.4 Network Name.
     * @retval kErrorAlready       The name is already set to the same string.
     * @retval kErrorInvalidArgs   Given name is invalid (too long or does not follow UTF-8 encoding).
     *
     */
    Error Set(const char *aNameString);

    /**
     * This method sets the IEEE 802.15.4 Network Name.
     *
     * @param[in]  aNameData           A reference to name data.
     *
     * @retval kErrorNone          Successfully set the IEEE 802.15.4 Network Name.
     * @retval kErrorAlready       The name is already set to the same string.
     * @retval kErrorInvalidArgs   Given name is too long.
     *
     */
    Error Set(const NameData &aNameData);

    /**
     * This method overloads operator `==` to evaluate whether or not two given `NetworkName` objects are equal.
     *
     * @param[in]  aOther  The other `NetworkName` to compare with.
     *
     * @retval TRUE   If the two are equal.
     * @retval FALSE  If the two are not equal.
     *
     */
    bool operator==(const NetworkName &aOther) const;
};

#if (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)
/**
 * This type represents a Thread Domain Name.
 *
 */
typedef NetworkName DomainName;
#endif

#if OPENTHREAD_CONFIG_MULTI_RADIO

/**
 * This enumeration defines the radio link types.
 *
 */
enum RadioType : uint8_t
{
#if OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE
    kRadioTypeIeee802154, ///< IEEE 802.15.4 (2.4GHz) link type.
#endif
#if OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE
    kRadioTypeTrel, ///< Thread Radio Encapsulation link type.
#endif
};

/**
 * This constant specifies the number of supported radio link types.
 *
 */
constexpr uint8_t kNumRadioTypes = (((OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE) ? 1 : 0) +
                                    ((OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE) ? 1 : 0));

/**
 * This class represents a set of radio links.
 *
 */
class RadioTypes
{
public:
    static constexpr uint16_t kInfoStringSize = 32; ///< Max chars for the info string (`ToString()`).

    /**
     * This type defines the fixed-length `String` object returned from `ToString()`.
     *
     */
    typedef String<kInfoStringSize> InfoString;

    /**
     * This static class variable defines an array containing all supported radio link types.
     *
     */
    static const RadioType kAllRadioTypes[kNumRadioTypes];

    /**
     * This constructor initializes a `RadioTypes` object as empty set
     *
     */
    RadioTypes(void)
        : mBitMask(0)
    {
    }

    /**
     * This constructor initializes a `RadioTypes` object with a given bit-mask.
     *
     * @param[in] aMask   A bit-mask representing the radio types (the first bit corresponds to radio type 0, and so on)
     *
     */
    explicit RadioTypes(uint8_t aMask)
        : mBitMask(aMask)
    {
    }

    /**
     * This method clears the set.
     *
     */
    void Clear(void) { mBitMask = 0; }

    /**
     * This method indicates whether the set is empty or not
     *
     * @returns TRUE if the set is empty, FALSE otherwise.
     *
     */
    bool IsEmpty(void) const { return (mBitMask == 0); }

    /**
     *  This method indicates whether the set contains only a single radio type.
     *
     * @returns TRUE if the set contains a single radio type, FALSE otherwise.
     *
     */
    bool ContainsSingleRadio(void) const { return !IsEmpty() && ((mBitMask & (mBitMask - 1)) == 0); }

    /**
     * This method indicates whether or not the set contains a given radio type.
     *
     * @param[in] aType  A radio link type.
     *
     * @returns TRUE if the set contains @p aType, FALSE otherwise.
     *
     */
    bool Contains(RadioType aType) const { return ((mBitMask & BitFlag(aType)) != 0); }

    /**
     * This method adds a radio type to the set.
     *
     * @param[in] aType  A radio link type.
     *
     */
    void Add(RadioType aType) { mBitMask |= BitFlag(aType); }

    /**
     * This method adds another radio types set to the current one.
     *
     * @param[in] aTypes   A radio link type set to add.
     *
     */
    void Add(RadioTypes aTypes) { mBitMask |= aTypes.mBitMask; }

    /**
     * This method adds all radio types supported by device to the set.
     *
     */
    void AddAll(void);

    /**
     * This method removes a given radio type from the set.
     *
     * @param[in] aType  A radio link type.
     *
     */
    void Remove(RadioType aType) { mBitMask &= ~BitFlag(aType); }

    /**
     * This method gets the radio type set as a bitmask.
     *
     * The first bit in the mask corresponds to first radio type (radio type with value zero), and so on.
     *
     * @returns A bitmask representing the set of radio types.
     *
     */
    uint8_t GetAsBitMask(void) const { return mBitMask; }

    /**
     * This method overloads operator `-` to return a new set which is the set difference between current set and
     * a given set.
     *
     * @param[in] aOther  Another radio type set.
     *
     * @returns A new set which is set difference between current one and @p aOther.
     *
     */
    RadioTypes operator-(const RadioTypes &aOther) const { return RadioTypes(mBitMask & ~aOther.mBitMask); }

    /**
     * This method converts the radio set to human-readable string.
     *
     * @return A string representation of the set of radio types.
     *
     */
    InfoString ToString(void) const;

private:
    static uint8_t BitFlag(RadioType aType) { return static_cast<uint8_t>(1U << static_cast<uint8_t>(aType)); }

    uint8_t mBitMask;
};

/**
 * This function converts a link type to a string
 *
 * @param[in] aRadioType  A link type value.
 *
 * @returns A string representation of the link type.
 *
 */
const char *RadioTypeToString(RadioType aRadioType);

#endif // OPENTHREAD_CONFIG_MULTI_RADIO

/**
 * This class represents Link Frame Counters for all supported radio links.
 *
 */
class LinkFrameCounters
{
public:
    /**
     * This method resets all counters (set them all to zero).
     *
     */
    void Reset(void) { SetAll(0); }

#if OPENTHREAD_CONFIG_MULTI_RADIO

    /**
     * This method gets the link Frame Counter for a given radio link.
     *
     * @param[in] aRadioType  A radio link type.
     *
     * @returns The Link Frame Counter for radio link @p aRadioType.
     *
     */
    uint32_t Get(RadioType aRadioType) const;

    /**
     * This method sets the Link Frame Counter for a given radio link.
     *
     * @param[in] aRadioType  A radio link type.
     * @param[in] aCounter    The new counter value.
     *
     */
    void Set(RadioType aRadioType, uint32_t aCounter);

#else

    /**
     * This method gets the Link Frame Counter value.
     *
     * @return The Link Frame Counter value.
     *
     */
    uint32_t Get(void) const
#if OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE
    {
        return m154Counter;
    }
#elif OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE
    {
        return mTrelCounter;
    }
#endif

    /**
     * This method sets the Link Frame Counter for a given radio link.
     *
     * @param[in] aCounter    The new counter value.
     *
     */
    void Set(uint32_t aCounter)
#if OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE
    {
        m154Counter = aCounter;
    }
#elif OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE
    {
        mTrelCounter = aCounter;
    }
#endif

#endif // OPENTHREAD_CONFIG_MULTI_RADIO

#if OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE
    /**
     * This method gets the Link Frame Counter for 802.15.4 radio link.
     *
     * @returns The Link Frame Counter for 802.15.4 radio link.
     *
     */
    uint32_t Get154(void) const { return m154Counter; }

    /**
     * This method sets the Link Frame Counter for 802.15.4 radio link.
     *
     * @param[in] aCounter   The new counter value.
     *
     */
    void Set154(uint32_t aCounter) { m154Counter = aCounter; }
#endif

#if OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE
    /**
     * This method gets the Link Frame Counter for TREL radio link.
     *
     * @returns The Link Frame Counter for TREL radio link.
     *
     */
    uint32_t GetTrel(void) const { return mTrelCounter; }

    /**
     * This method increments the Link Frame Counter for TREL radio link.
     *
     */
    void IncrementTrel(void) { mTrelCounter++; }
#endif

    /**
     * This method gets the maximum Link Frame Counter among all supported radio links.
     *
     * @return The maximum Link frame Counter among all supported radio links.
     *
     */
    uint32_t GetMaximum(void) const;

    /**
     * This method sets the Link Frame Counter value for all radio links.
     *
     * @param[in]  aCounter  The Link Frame Counter value.
     *
     */
    void SetAll(uint32_t aCounter);

private:
#if OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE
    uint32_t m154Counter;
#endif
#if OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE
    uint32_t mTrelCounter;
#endif
};

/**
 * @}
 *
 */

} // namespace Mac
} // namespace ot

#endif // MAC_TYPES_HPP_
