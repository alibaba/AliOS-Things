/*
 *    Copyright (c) 2017, The OpenThread Authors.
 *    All rights reserved.
 *
 *    Redistribution and use in source and binary forms, with or without
 *    modification, are permitted provided that the following conditions are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *    3. Neither the name of the copyright holder nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 *    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
 *    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file contains the definitions of a spinel decoder.
 */

#ifndef SPINEL_DECODER_HPP_
#define SPINEL_DECODER_HPP_

#include "openthread-spinel-config.h"

#include <openthread/ip6.h>
#include <openthread/ncp.h>

#include "spinel.h"

namespace ot {
namespace Spinel {

/**
 * This class defines a spinel decoder.
 *
 */
class Decoder
{
public:
    enum
    {
        kMaxNestedStructs = 4, ///< Maximum number of nested structs.
    };

    /**
     * This constructor initializes a `Decoder` object.
     *
     */
    Decoder(void);

    /**
     * This method initializes the decoder to start decoding a new frame.
     *
     * It sets the read position to the start of the frame and also erases/voids any saved positions (see
     * `SavePosition()` and `ResetToSaved()` methods).
     *
     * @param[in] aFrame                Pointer to the buffer containing the frame to be decoded.
     * @param[in] aLength               Length (number of bytes) of the frame.
     *
     */
    void Init(const uint8_t *aFrame, uint16_t aLength);

    /**
     * This method returns the pointer to the start of the frame.
     *
     * @returns A pointer to buffer containing current frame being decoded.
     *
     */
    const uint8_t *GetFrame(void) const { return mFrame; }

    /**
     * This method returns the total length of current frame being decoded.
     *
     * @returns The length of current frame being decoded.
     *
     */
    uint16_t GetLength(void) const { return mLength; }

    /**
     * This method returns the number of bytes that are already read/decoded from the frame.
     *
     * @returns The number of bytes already read from frame.
     *
     */
    uint16_t GetReadLength(void) const { return mIndex; }

    /**
     * This method returns the number of remaining (not yet read/decoded) bytes in the frame.
     *
     * @returns The number of remaining unread bytes in the frame.
     *
     */
    uint16_t GetRemainingLength(void) const { return mLength - mIndex; }

    /**
     * This method indicates whether or not all the bytes in the frame are read.
     *
     * @returns TRUE if all the bytes in the buffer are read, FALSE otherwise.
     *
     */
    bool IsAllRead(void) const { return (mIndex == mLength); }

    /**
     * This method resets the read position to beginning of frame. It will also void/erase any previously saved
     * position using `SavePosition()` method.
     *
     */
    void Reset(void);

    /**
     * This method decodes and reads a boolean value form the frame.
     *
     * On success, the read position gets updated.
     *
     * @param[out] aBool                Reference to variable to output the read value.
     *
     * @retval OT_ERROR_NONE            Successfully read the value.
     * @retval OT_ERROR_PARSE           Failed to parse/decode the value.
     *
     */
    otError ReadBool(bool &aBool);

    /**
     * This method decodes and reads an `int8_t` value form the frame.
     *
     * On success, the read position get updated.
     *
     * @param[out] aInt8                Reference to variable to output the read value.
     *
     * @retval OT_ERROR_NONE            Successfully read the value.
     * @retval OT_ERROR_PARSE           Failed to parse/decode the value.
     *
     */
    otError ReadInt8(int8_t &aInt8);

    /**
     * This method decodes and reads an `uint8_t` value form the frame.
     *
     * On success, the read position gets updated.
     *
     * @param[out] aUint8               Reference to variable to output the read value.
     *
     * @retval OT_ERROR_NONE            Successfully read the value.
     * @retval OT_ERROR_PARSE           Failed to parse/decode the value.
     *
     */
    otError ReadUint8(uint8_t &aUint8);

    /**
     * This method decodes and reads an `int16_t` value form the frame.
     *
     * On success, the read position gets updated.
     *
     * @param[out] aInt16               Reference to variable to output the read value.
     *
     * @retval OT_ERROR_NONE            Successfully read the value.
     * @retval OT_ERROR_PARSE           Failed to parse/decode the value.
     *
     */
    otError ReadInt16(int16_t &aInt16);

    /**
     * This method decodes and reads an `uint16_t` value form the frame.
     *
     * On success, the read position gets updated.
     *
     * @param[out] aUint16              Reference to variable to output the read value.
     *
     * @retval OT_ERROR_NONE            Successfully read the value.
     * @retval OT_ERROR_PARSE           Failed to parse/decode the value.
     *
     */
    otError ReadUint16(uint16_t &aUint16);

    /**
     * This method decodes and reads an `int32_t` value form the frame.
     *
     * On success, the read position gets updated.
     *
     * @param[out] aInt32               Reference to variable to output the read value.
     *
     * @retval OT_ERROR_NONE            Successfully read the value.
     * @retval OT_ERROR_PARSE           Failed to parse/decode the value.
     *
     */
    otError ReadInt32(int32_t &aInt32);

    /**
     * This method decodes and reads an `uint32_t` value form the frame.
     *
     * On success, the read position gets updated.
     *
     * @param[out] aUint32              Reference to variable to output the read value.
     *
     * @retval OT_ERROR_NONE            Successfully read the value.
     * @retval OT_ERROR_PARSE           Failed to parse/decode the value.
     *
     */
    otError ReadUint32(uint32_t &aUint32);

    /**
     * This method decodes and reads an `int64_t` value form the frame.
     *
     * On success, the read position gets updated.
     *
     * @param[out] aInt64               Reference to variable to output the read value.
     *
     * @retval OT_ERROR_NONE            Successfully read the value.
     * @retval OT_ERROR_PARSE           Failed to parse/decode the value.
     *
     */
    otError ReadInt64(int64_t &aInt64);

    /**
     * This method decodes and reads an `uint64_t` value form the frame.
     *
     * On success, the read position gets updated.
     *
     * @param[out] aUint64              Reference to variable to output the read value.
     *
     * @retval OT_ERROR_NONE            Successfully read the value.
     * @retval OT_ERROR_PARSE           Failed to parse/decode the value.
     *
     */
    otError ReadUint64(uint64_t &aUint64);

    /**
     * This method decodes (using spinel packed integer format) and reads an unsigned integer value form the frame.
     *
     * On success, the read position gets updated.
     *
     * @param[out] aUint                Reference to variable to output the read value.
     *
     * @retval OT_ERROR_NONE            Successfully read the value.
     * @retval OT_ERROR_PARSE           Failed to parse/decode the value.
     *
     */
    otError ReadUintPacked(unsigned int &aUint);

    /**
     * This method decodes and reads an IPv6 address form the frame.
     *
     * On success, the read position gets updated.
     *
     * @param[out] aIp6AddrPtr          Reference to IPv6 address pointer to output the value (as `spinel_ipv6addr_t`).
     *                                  On success, the pointer variable is updated.
     *
     * @retval OT_ERROR_NONE            Successfully read the value.
     * @retval OT_ERROR_PARSE           Failed to parse/decode the value.
     *
     */
    otError ReadIp6Address(const spinel_ipv6addr_t *&aIp6AddrPtr)
    {
        return ReadItem(reinterpret_cast<const uint8_t **>(&aIp6AddrPtr), sizeof(spinel_ipv6addr_t));
    }

    /**
     * This method decodes and reads an IPv6 address form the frame.
     *
     * On success, the read position gets updated.
     *
     * @param[out] aIp6AddrPtr          Reference to IPv6 address pointer to output the value (as `otIp6Address`).
     *                                  On success, the pointer variable is updated.
     *
     * @retval OT_ERROR_NONE            Successfully read the value.
     * @retval OT_ERROR_PARSE           Failed to parse/decode the value.
     *
     */
    otError ReadIp6Address(const otIp6Address *&aIp6AddrPtr)
    {
        return ReadItem(reinterpret_cast<const uint8_t **>(&aIp6AddrPtr), sizeof(spinel_ipv6addr_t));
    }

    /**
     * This method decodes and reads an IPv6 address form the frame.
     *
     * On success, the read position gets updated.
     *
     * @param[out] aIp6AddrBufPtr       Reference to a buffer pointer to output the value.
     *                                  On success, the pointer variable is updated.
     *
     * @retval OT_ERROR_NONE            Successfully read the value.
     * @retval OT_ERROR_PARSE           Failed to parse/decode the value.
     *
     */
    otError ReadIp6Address(const uint8_t *&aIp6AddrBufPtr)
    {
        return ReadItem(&aIp6AddrBufPtr, sizeof(spinel_ipv6addr_t));
    }

    /**
     * This method decodes and reads an IPv6 address form the frame.
     *
     * On success, the read position gets updated and the IP address is copied into the given output variable.
     *
     * @param[out] aIp6Addr             Reference to IPv6 address variable to output the value (as `spinel_ipv6addr_t`).
     *                                  On success, the address is copied into the output variable.
     *
     * @retval OT_ERROR_NONE            Successfully read the value.
     * @retval OT_ERROR_PARSE           Failed to parse/decode the value.
     *
     */
    otError ReadIp6Address(spinel_ipv6addr_t &aIp6Addr);

    /**
     * This method decodes and reads an IPv6 address form the frame.
     *
     * On success, the read position gets updated and the IP address is copied into the given output variable.
     *
     * @param[out] aIp6Addr             Reference to IPv6 address variable to output the value (as `otIp6Address`).
     *                                  On success, the address is copied into the output variable.
     *
     * @retval OT_ERROR_NONE            Successfully read the value.
     * @retval OT_ERROR_PARSE           Failed to parse/decode the value.
     *
     */
    otError ReadIp6Address(otIp6Address &aIp6Addr);

    /**
     * This method decodes and reads an EUI64 value form the frame.
     *
     * On success, the read position gets updated.
     *
     * @param[out] aEui64Ptr            Reference to an EUI64 pointer to output the value (as `spinel_eui64_t`).
     *                                  On success, the pointer variable is updated.
     *
     * @retval OT_ERROR_NONE            Successfully read the value.
     * @retval OT_ERROR_PARSE           Failed to parse/decode the value.
     *
     */
    otError ReadEui64(const spinel_eui64_t *&aEui64Ptr)
    {
        return ReadItem(reinterpret_cast<const uint8_t **>(&aEui64Ptr), sizeof(spinel_eui64_t));
    }

    /**
     * This method decodes and reads an EUI64 value form the frame.
     *
     * On success, the read position gets updated.
     *
     * @param[out] aEui64Ptr            Reference to an EUI64 pointer to output the value (as `otExtAddress`).
     *                                  On success, the pointer variable is updated.
     *
     * @retval OT_ERROR_NONE            Successfully read the value.
     * @retval OT_ERROR_PARSE           Failed to parse/decode the value.
     *
     */
    otError ReadEui64(const otExtAddress *&aEui64Ptr)
    {
        return ReadItem(reinterpret_cast<const uint8_t **>(&aEui64Ptr), sizeof(spinel_eui64_t));
    }

    /**
     * This method decodes and reads an EUI64 value form the frame.
     *
     * On success, the read position gets updated.
     *
     * @param[out] aEui64BufPtr         Reference to a buffer pointer to output the value.
     *                                  On success, the pointer variable is updated.
     *
     * @retval OT_ERROR_NONE            Successfully read the value.
     * @retval OT_ERROR_PARSE           Failed to parse/decode the value.
     *
     */
    otError ReadEui64(const uint8_t *&aEui64BufPtr) { return ReadItem(&aEui64BufPtr, sizeof(spinel_eui64_t)); }

    /**
     * This method decodes and reads an EUI64 value form the frame.
     *
     * On success, the read position gets updated and the EUI64 value is copied into the given output variable.
     *
     * @param[out] aEui64               Reference to EUI64 variable to output the value (as `spinel_eui64_t`).
     *                                  On success, the address is copied into the output variable.
     *
     * @retval OT_ERROR_NONE            Successfully read the value.
     * @retval OT_ERROR_PARSE           Failed to parse/decode the value.
     *
     */
    otError ReadEui64(spinel_eui64_t &aEui64);

    /**
     * This method decodes and reads an EUI64 value form the frame.
     *
     * On success, the read position gets updated and the EUI64 value is copied into the given output variable.
     *
     * @param[out] aEui64               Reference to EUI64 variable to output the value (as `otExtAddress`).
     *                                  On success, the address is copied into the output variable.
     *
     * @retval OT_ERROR_NONE            Successfully read the value.
     * @retval OT_ERROR_PARSE           Failed to parse/decode the value.
     *
     */
    otError ReadEui64(otExtAddress &aEui64);

    /**
     * This method decodes and reads an EUI48 value form the frame.
     *
     * On success, the read position gets updated.
     *
     * @param[out] aEui48Ptr            Reference to an EUI48 pointer to output the value (as `spinel_eui48_t`).
     *                                  On success, the pointer variable is updated.
     *
     * @retval OT_ERROR_NONE            Successfully read the value.
     * @retval OT_ERROR_PARSE           Failed to parse/decode the value.
     *
     */
    otError ReadEui48(const spinel_eui48_t *&aEui48Ptr)
    {
        return ReadItem(reinterpret_cast<const uint8_t **>(&aEui48Ptr), sizeof(spinel_eui48_t));
    }

    /**
     * This method decodes and reads an EUI48 value form the frame.
     *
     * On success, the read position gets updated.
     *
     * @param[out] aEui48BufPtr         Reference to a buffer pointer to output the value.
     *                                  On success, the pointer variable is updated.
     *
     * @retval OT_ERROR_NONE            Successfully read the value.
     * @retval OT_ERROR_PARSE           Failed to parse/decode the value.
     *
     */
    otError ReadEui48(const uint8_t *&aEui48BufPtr) { return ReadItem(&aEui48BufPtr, sizeof(spinel_eui48_t)); }

    /**
     * This method decodes and reads an EUI48 value form the frame.
     *
     * On success, the read position gets updated and the EUI48 value is copied into the given output variable.
     *
     * @param[out] aEui48               Reference to EUI48 variable to output the value (as `spinel_eui48_t`).
     *                                  On success, value is copied into the output variable.
     *
     * @retval OT_ERROR_NONE            Successfully read the value.
     * @retval OT_ERROR_PARSE           Failed to parse/decode the value.
     *
     */
    otError ReadEui48(spinel_eui48_t &aEui48);

    /**
     * This method decodes and reads a UTF8 string form the frame.
     *
     * On success, the read position gets updated.
     *
     * @param[out] aUtf8                Reference to a `char` pointer to output the string.
     *                                  On success, the pointer variable is updated.
     *
     * @retval OT_ERROR_NONE            Successfully read the value.
     * @retval OT_ERROR_PARSE           Failed to parse/decode the value.
     *
     */
    otError ReadUtf8(const char *&aUtf8);

    /**
     * This method decodes and reads a data blob (sequence of bytes) form the frame.
     *
     * On success, the read position gets updated.
     *
     * @param[out] aData                Reference to pointer variable to output the data.
     *                                  On success, the pointer variable is updated.
     * @param[out] aDataLength          Reference to variable to output the data length (number of bytes).
     *
     * @retval OT_ERROR_NONE            Successfully read the value.
     * @retval OT_ERROR_PARSE           Failed to parse/decode the value.
     *
     */
    otError ReadData(const uint8_t *&aData, uint16_t &aDataLen);

    /**
     * This method decodes and reads a data blob (sequence of bytes) with data length.
     *
     * The data length is assumed to be prepended before the data content (encoded as a `uint16_t`). The size of the
     * length field should not be included in the length value. This method corresponds to `SPINEL_DATATYPE_DATA_WLEN`
     * type.
     *
     * @param[out] aData                Reference to pointer variable to output the data.
     *                                  On success, the pointer variable is updated.
     * @param[out] aDataLength          Reference to variable to out the data length (number of bytes).
     *
     * @retval OT_ERROR_NONE            Successfully read the value.
     * @retval OT_ERROR_PARSE           Failed to parse/decode the value.
     *
     */
    otError ReadDataWithLen(const uint8_t *&aData, uint16_t &aDataLen);

    /**
     * This method opens a struct in the frame.
     *
     * After a successful call to this method, all the subsequent `Read{SomeType}()` methods decode and read the
     * field/value from the current open struct until the struct is closed using `CloseStruct()` method. Structures can
     * be nested. Up to `kMaxNestedStructs` nested structs can be opened at the same time.
     *
     * @retval OT_ERROR_NONE            Successfully opened a struct.
     * @retval OT_ERROR_PARSE           Failed to parse/open a struct.
     * @retval OT_ERROR_INVALID_STATE   Already at the maximum number of nested open structures.
     *
     */
    otError OpenStruct(void);

    /**
     * This method closes the most recently opened struct (using `OpenStruct()`) in the frame.
     *
     * On success, the read position is moved to end of the struct skipping any unread bytes within the struct.
     *
     * @retval OT_ERROR_NONE            Successfully closed the struct.
     * @retval OT_ERROR_INVALID_STATE   There is no current open struct to close.
     *
     */
    otError CloseStruct(void);

    /**
     * This method returns the number of remaining/unread bytes in the current inner-most open structure.
     *
     * If there is no currently open structure the number of remaining bytes in whole frame is returned instead.
     *
     * @returns The number of remaining unread bytes in the inner-most open structure.
     *
     */
    uint16_t GetRemainingLengthInStruct(void) const { return mEnd - mIndex; }

    /**
     * This method indicates whether or not all the bytes in inner-most open structure are read.
     *
     * If there is no currently open structure, the whole frame is considered instead.
     *
     * @returns TRUE if all the bytes are read, FALSE otherwise.
     *
     */
    bool IsAllReadInStruct(void) const { return (mIndex == mEnd); }

    /**
     * This method saves the current read position in the frame.
     *
     * A subsequent call to `SavePosition()` will overwrite the previously saved position. The saved position can be
     * used to move the read position back (using `ResetToSaved()`) and re-read the same content.
     *
     * Saved position can be within an open struct, and it remembers its enclosing struct. When the enclosing struct is
     * closed, the saved position will be voided and can no longer be used. This ensures that we cannot jump back to
     * middle an already fully decoded/read and closed struct.
     *
     */
    void SavePosition(void);

    /**
     * This method resets/moves the read position to a previously saved position.
     *
     * The saved position remembers its enclosing structure. When `ResetToSaved()` is called, the current open
     * structure will be the same as when position was saved.
     *
     * @retval OT_ERROR_NONE            Successfully reset the read position.
     * @retval OT_ERROR_INVALID_STATE   The saved position is not valid (there is no saved position or the saved
     *                                  position was voided since its enclosing struct was closed).
     *
     */
    otError ResetToSaved(void);

private:
    otError ReadItem(const uint8_t **aPtr, uint16_t aSize);
    void    ClearSavedPosition(void) { mSavedIndex = mLength; }
    bool    IsSavedPositionValid(void) const { return (mSavedIndex < mLength); }

    const uint8_t *mFrame;          // Frame buffer.
    uint16_t       mLength;         // Frame length (number of bytes).
    uint16_t       mIndex;          // Current read index.
    uint16_t       mEnd;            // Current end index (end of struct if in a struct, or end of buffer otherwise).
    uint8_t        mNumOpenStructs; // Number of open structs.

    uint8_t  mSavedNumOpenStructs; // Number of open structs when read position was saved.
    uint16_t mSavedIndex;          // Read index when position was saved.
    uint16_t mSavedEnd;            // End index when position was saved.

    uint16_t mPrevEnd[kMaxNestedStructs];
};

} // namespace Spinel
} // namespace ot

#endif // SPINEL_DECODER_HPP_
