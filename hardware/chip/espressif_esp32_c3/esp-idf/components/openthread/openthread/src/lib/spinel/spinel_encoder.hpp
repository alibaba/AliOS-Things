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
 *   This file contains the definitions of a spinel encoder.
 */

#ifndef SPINEL_ENCODER_HPP_
#define SPINEL_ENCODER_HPP_

#include "openthread-spinel-config.h"

#include <openthread/ip6.h>
#include <openthread/message.h>
#include <openthread/ncp.h>

#include "spinel.h"
#include "spinel_buffer.hpp"

namespace ot {
namespace Spinel {

/**
 * This class defines a spinel encoder.
 *
 */
class Encoder
{
public:
    /**
     * This constructor initializes a `Encoder` object.
     *
     * @param[in] aNcpBuffer   A reference to a `Spinel::Buffer` where the frames are written.
     *
     */
    explicit Encoder(Spinel::Buffer &aNcpBuffer)
        : mNcpBuffer(aNcpBuffer)
        , mNumOpenStructs(0)
        , mSavedNumOpenStructs(0)
    {
    }

    /**
     * This method begins a new frame to be added/written to the frame buffer.
     *
     * If there is a previous frame being written (for which `EndFrame()` has not yet been called), calling
     * `BeginFrame()` will discard and clear the previous unfinished frame.
     *
     * @param[in] aPriority             Priority level of the new input frame.
     *
     * @retval OT_ERROR_NONE            Successfully started a new frame.
     * @retval OT_ERROR_NO_BUFS         Insufficient buffer space available to start a new frame.
     *
     */
    otError BeginFrame(Spinel::Buffer::Priority aPriority);

    /**
     * This method begins a new spinel command frame to be added/written to the frame buffer.
     *
     * If there is a previous frame being written (for which `EndFrame()` has not yet been called), calling
     * `BeginFrame()` will discard and clear the previous unfinished frame.
     *
     * The spinel transaction ID (TID) in the given spinel header is used to determine the priority level of the new
     * frame. Non-zero TID value indicates that the frame is a response and therefore it uses higher priority level.
     *
     * @param[in] aHeader               Spinel header for new the command frame.
     * @param[in] aCommand              Spinel command.
     *
     * @retval OT_ERROR_NONE            Successfully started a new frame.
     * @retval OT_ERROR_NO_BUFS         Insufficient buffer space available to start a new frame.
     *
     */
    otError BeginFrame(uint8_t aHeader, unsigned int aCommand);

    /**
     * This method begins a new spinel property update command frame to be added/written to the frame buffer.
     *
     * If there is a previous frame being written (for which `EndFrame()` has not yet been called), calling
     * `BeginFrame()` will discard and clear the previous unfinished frame.
     *
     * The spinel transaction ID (TID) in the given spinel header is used to determine the priority level of the new
     * frame. Non-zero TID value indicates that the frame is a response and therefore it uses higher priority level.
     *
     * This method saves the write position before the property key (see also `SavePosition()`) so that if fetching the
     * property fails and the property key should be switched to `LAST_STATUS` with an error status, the saved
     * position can be used to update the property key in the frame (see also `OverwriteWithLastStatusError()`)
     *
     * @param[in] aHeader               Spinel header for new the command frame.
     * @param[in] aCommand              Spinel command.
     * @param[in] aKey                  Spinel property key
     *
     * @retval OT_ERROR_NONE            Successfully started a new frame.
     * @retval OT_ERROR_NO_BUFS         Insufficient buffer space available to start a new frame.
     *
     */
    otError BeginFrame(uint8_t aHeader, unsigned int aCommand, spinel_prop_key_t aKey);

    /**
     * This method overwrites the property key with `LAST_STATUS` in a property update command frame.
     *
     * This method should be only used after a successful `BeginFrame(aHeader, aCommand, aPropertKey)`, otherwise, its
     * behavior is undefined.
     *
     * This method moves the write position back to saved position by `BeginFrame()` and replaces the property key
     * `SPINEL_PROP_LAST_STATUS` and writes the given spinel status error.
     *
     * @param[in] aStatus               Spinel error status
     *
     * @retval OT_ERROR_NONE            Successfully updated the frame.
     * @retval OT_ERROR_NO_BUFS         Insufficient buffer space available to update the frame.
     *
     */
    otError OverwriteWithLastStatusError(spinel_status_t aStatus);

    /**
     * This method finalizes/ends the current frame being written to the buffer.
     *
     * Before using this method `BeginFrame()` must be called to start and prepare a new frame. Otherwise, this method
     * does nothing and returns error status `OT_ERROR_INVALID_STATE`.
     *
     * If no buffer space is available, this method will discard and clear the frame and return error status
     * `OT_ERROR_NO_BUFS`.
     *
     * This method ensures to close any open structure (previously opened using `OpenStruct()` but not closed using
     * `CloseStruct()`).
     *
     * @retval OT_ERROR_NONE            Successfully ended the input frame.
     * @retval OT_ERROR_NO_BUFS         Insufficient buffer space available to add message.
     * @retval OT_ERROR_INVALID_STATE   `BeginFrame()` has not been called earlier to start the frame.
     *
     */
    otError EndFrame(void);

    /**
     * This method encodes and writes a boolean value to current input frame.
     *
     * Before using this method `BeginFrame()` must be called to start and prepare a new input frame. Otherwise, this
     * method does nothing and returns error status `OT_ERROR_INVALID_STATE`.
     *
     * If no buffer space is available, this method will discard and clear the current input frame and return the
     * error status `OT_ERROR_NO_BUFS`.
     *
     * @param[in]  aBool                The boolean value to add to input frame.
     *
     * @retval OT_ERROR_NONE            Successfully added given byte to the frame.
     * @retval OT_ERROR_NO_BUFS         Insufficient buffer space available to add the value.
     * @retval OT_ERROR_INVALID_STATE   `BeginFrame()` has not been called earlier to start the frame.
     *
     */
    otError WriteBool(bool aBool) { return mNcpBuffer.InFrameFeedByte(aBool ? 0x01 : 0x00); }

    /**
     * This method encodes and writes a `uint8_t` value to current input frame.
     *
     * Before using this method `BeginFrame()` must be called to start and prepare a new input frame. Otherwise, this
     * method does nothing and returns error status `OT_ERROR_INVALID_STATE`.
     *
     * If no buffer space is available, this method will discard and clear the current input frame and return the
     * error status `OT_ERROR_NO_BUFS`.
     *
     * @param[in]  aUint8               The value to add to input frame.
     *
     * @retval OT_ERROR_NONE            Successfully added given value to the frame.
     * @retval OT_ERROR_NO_BUFS         Insufficient buffer space available to add the value.
     * @retval OT_ERROR_INVALID_STATE   `BeginFrame()` has not been called earlier to start the frame.
     *
     */
    otError WriteUint8(uint8_t aUint8) { return mNcpBuffer.InFrameFeedByte(aUint8); }

    /**
     * This method encodes and writes an `int8_t` value to current input frame.
     *
     * Before using this method `BeginFrame()` must be called to start and prepare a new input frame. Otherwise, this
     * method does nothing and returns error status `OT_ERROR_INVALID_STATE`.
     *
     * If no buffer space is available, this method will discard and clear the current input frame and return the
     * error status `OT_ERROR_NO_BUFS`.
     *
     * @param[in]  aInt8                The value to add to input frame.
     *
     * @retval OT_ERROR_NONE            Successfully added given value to the frame.
     * @retval OT_ERROR_NO_BUFS         Insufficient buffer space available to add the value.
     * @retval OT_ERROR_INVALID_STATE   `BeginFrame()` has not been called earlier to start the frame.
     *
     */
    otError WriteInt8(int8_t aInt8) { return WriteUint8(static_cast<uint8_t>(aInt8)); }

    /**
     * This method encodes and writes a `uint16_t` value to current input frame.
     *
     * Before using this method `BeginFrame()` must be called to start and prepare a new input frame. Otherwise, this
     * method does nothing and returns error status `OT_ERROR_INVALID_STATE`.
     *
     * If no buffer space is available, this method will discard and clear the current input frame and return the
     * error status `OT_ERROR_NO_BUFS`.
     *
     * @param[in]  aUint16              The value to add to input frame.
     *
     * @retval OT_ERROR_NONE            Successfully added given value to the frame.
     * @retval OT_ERROR_NO_BUFS         Insufficient buffer space available to add the value.
     * @retval OT_ERROR_INVALID_STATE   `BeginFrame()` has not been called earlier to start the frame.
     *
     */
    otError WriteUint16(uint16_t aUint16);

    /**
     * This method encodes and writes an `int16_t` value to current input frame.
     *
     * Before using this method `BeginFrame()` must be called to start and prepare a new input frame. Otherwise, this
     * method does nothing and returns error status `OT_ERROR_INVALID_STATE`.
     *
     * If no buffer space is available, this method will discard and clear the current input frame and return the
     * error status `OT_ERROR_NO_BUFS`.
     *
     * @param[in]  aInt16              The value to add to input frame.
     *
     * @retval OT_ERROR_NONE            Successfully added given value to the frame.
     * @retval OT_ERROR_NO_BUFS         Insufficient buffer space available to add the value.
     * @retval OT_ERROR_INVALID_STATE   `BeginFrame()` has not been called earlier to start the frame.
     *
     */
    otError WriteInt16(int16_t aInt16) { return WriteUint16(static_cast<uint16_t>(aInt16)); }

    /**
     * This method encodes and writes a `uint32_t` value to current input frame.
     *
     * Before using this method `BeginFrame()` must be called to start and prepare a new input frame. Otherwise, this
     * method does nothing and returns error status `OT_ERROR_INVALID_STATE`.
     *
     * If no buffer space is available, this method will discard and clear the current input frame and return the
     * error status `OT_ERROR_NO_BUFS`.
     *
     * @param[in]  aUint32              The value to add to input frame.
     *
     * @retval OT_ERROR_NONE            Successfully added given value to the frame.
     * @retval OT_ERROR_NO_BUFS         Insufficient buffer space available to add the value.
     * @retval OT_ERROR_INVALID_STATE   `BeginFrame()` has not been called earlier to start the frame.
     *
     */
    otError WriteUint32(uint32_t aUint32);

    /**
     * This method encodes and writes an `int32_t` value to current input frame.
     *
     * Before using this method `BeginFrame()` must be called to start and prepare a new input frame. Otherwise, this
     * method does nothing and returns error status `OT_ERROR_INVALID_STATE`.
     *
     * If no buffer space is available, this method will discard and clear the current input frame and return the
     * error status `OT_ERROR_NO_BUFS`.
     *
     * @param[in]  aInt32               The value to add to input frame.
     *
     * @retval OT_ERROR_NONE            Successfully added given value to the frame.
     * @retval OT_ERROR_NO_BUFS         Insufficient buffer space available to add the value.
     * @retval OT_ERROR_INVALID_STATE   `BeginFrame()` has not been called earlier to start the frame.
     *
     */
    otError WriteInt32(int32_t aInt32) { return WriteUint32(static_cast<uint32_t>(aInt32)); }

    /**
     * This method encodes and writes a `uint64_t` value to current input frame.
     *
     * Before using this method `BeginFrame()` must be called to start and prepare a new input frame. Otherwise, this
     * method does nothing and returns error status `OT_ERROR_INVALID_STATE`.
     *
     * If no buffer space is available, this method will discard and clear the current input frame and return the
     * error status `OT_ERROR_NO_BUFS`.
     *
     * @param[in]  aUint64              The value to add to input frame.
     *
     * @retval OT_ERROR_NONE            Successfully added given value to the frame.
     * @retval OT_ERROR_NO_BUFS         Insufficient buffer space available to add the value.
     * @retval OT_ERROR_INVALID_STATE   `BeginFrame()` has not been called earlier to start the frame.
     *
     */
    otError WriteUint64(uint64_t aUint64);

    /**
     * This method encodes and writes an `int64_t` value to current input frame.
     *
     * Before using this method `BeginFrame()` must be called to start and prepare a new input frame. Otherwise, this
     * method does nothing and returns error status `OT_ERROR_INVALID_STATE`.
     *
     * If no buffer space is available, this method will discard and clear the current input frame and return the
     * error status `OT_ERROR_NO_BUFS`.
     *
     * @param[in]  aInt64               The value to add to input frame.
     *
     * @retval OT_ERROR_NONE            Successfully added given value to the frame.
     * @retval OT_ERROR_NO_BUFS         Insufficient buffer space available to add the value.
     * @retval OT_ERROR_INVALID_STATE   `BeginFrame()` has not been called earlier to start the frame.
     *
     */
    otError WriteInt64(int64_t aInt64) { return WriteUint64(static_cast<uint64_t>(aInt64)); }

    /**
     * This method encodes (using spinel packed integer format) and writes a value to current input frame.
     *
     * Before using this method `BeginFrame()` must be called to start and prepare a new input frame. Otherwise, this
     * method does nothing and returns error status `OT_ERROR_INVALID_STATE`.
     *
     * If no buffer space is available, this method will discard and clear the current input frame and return the
     * error status `OT_ERROR_NO_BUFS`.
     *
     * @param[in]  aUint                The value to add to input frame.
     *
     * @retval OT_ERROR_NONE            Successfully added given value to the frame.
     * @retval OT_ERROR_NO_BUFS         Insufficient buffer space available to add the value.
     * @retval OT_ERROR_INVALID_STATE   `BeginFrame()` has not been called earlier to start the frame.
     *
     */
    otError WriteUintPacked(unsigned int aUint);

    /**
     * This method encodes and writes an IPv6 address to current input frame.
     *
     * Before using this method `BeginFrame()` must be called to start and prepare a new input frame. Otherwise, this
     * method does nothing and returns error status `OT_ERROR_INVALID_STATE`.
     *
     * If no buffer space is available, this method will discard and clear the current input frame and return the
     * error status `OT_ERROR_NO_BUFS`.
     *
     * @param[in]  aIp6Addr             A reference to the IPv6 address to be added (as `spinel_ipv6addr_t`)
     *
     * @retval OT_ERROR_NONE            Successfully added given address to the frame.
     * @retval OT_ERROR_NO_BUFS         Insufficient buffer space available to add the IP address.
     * @retval OT_ERROR_INVALID_STATE   `BeginFrame()` has not been called earlier to start the frame.
     *
     */
    otError WriteIp6Address(const spinel_ipv6addr_t &aIp6Addr) { return WriteIp6Address(aIp6Addr.bytes); }

    /**
     * This method encodes and writes an IPv6 address to current input frame.
     *
     * Before using this method `BeginFrame()` must be called to start and prepare a new input frame. Otherwise, this
     * method does nothing and returns error status `OT_ERROR_INVALID_STATE`.
     *
     * If no buffer space is available, this method will discard and clear the current input frame and return the
     * error status `OT_ERROR_NO_BUFS`.
     *
     * @param[in]  aIp6Addr             A reference to the IPv6 address to be added (as `otIp6Address`)
     *
     * @retval OT_ERROR_NONE            Successfully added given address to the frame.
     * @retval OT_ERROR_NO_BUFS         Insufficient buffer space available to add the IP address.
     * @retval OT_ERROR_INVALID_STATE   `BeginFrame()` has not been called earlier to start the frame.
     *
     */
    otError WriteIp6Address(const otIp6Address &aIp6Addr) { return WriteIp6Address(aIp6Addr.mFields.m8); }

    /**
     * This method encodes and writes an IPv6 address to current input frame.
     *
     * Before using this method `BeginFrame()` must be called to start and prepare a new input frame. Otherwise, this
     * method does nothing and returns error status `OT_ERROR_INVALID_STATE`.
     *
     * If no buffer space is available, this method will discard and clear the current input frame and return the
     * error status `OT_ERROR_NO_BUFS`.
     *
     * @param[in]  aIp6AddrBuf          A pointer to a buffer containing the IPv6 address.
     *
     * @retval OT_ERROR_NONE            Successfully added given address to the frame.
     * @retval OT_ERROR_NO_BUFS         Insufficient buffer space available to add the IP address.
     * @retval OT_ERROR_INVALID_STATE   `BeginFrame()` has not been called earlier to start the frame.
     *
     */
    otError WriteIp6Address(const uint8_t *aIp6AddrBuf) { return WriteData(aIp6AddrBuf, sizeof(spinel_ipv6addr_t)); }

    /**
     * This method encodes and writes an EUI64 value to current input frame.
     *
     * Before using this method `BeginFrame()` must be called to start and prepare a new input frame. Otherwise, this
     * method does nothing and returns error status `OT_ERROR_INVALID_STATE`.
     *
     * If no buffer space is available, this method will discard and clear the current input frame and return the
     * error status `OT_ERROR_NO_BUFS`.
     *
     * @param[in]  aEui64               A reference to the EUI64 value as a `spinel_eui64_t` type.
     *
     * @retval OT_ERROR_NONE            Successfully added given value to the frame.
     * @retval OT_ERROR_NO_BUFS         Insufficient buffer space available to add the EUI64 value.
     * @retval OT_ERROR_INVALID_STATE   `BeginFrame()` has not been called earlier to start the frame.
     *
     */
    otError WriteEui64(const spinel_eui64_t &aEui64) { return WriteEui64(aEui64.bytes); }

    /**
     * This method encodes and writes an EUI64 value to current input frame.
     *
     * Before using this method `BeginFrame()` must be called to start and prepare a new input frame. Otherwise, this
     * method does nothing and returns error status `OT_ERROR_INVALID_STATE`.
     *
     * If no buffer space is available, this method will discard and clear the current input frame and return the
     * error status `OT_ERROR_NO_BUFS`.
     *
     * @param[in]  aExtAddress          A reference to an `otExtAddress`
     *
     * @retval OT_ERROR_NONE            Successfully added given value to the frame.
     * @retval OT_ERROR_NO_BUFS         Insufficient buffer space available to add the EUI64 value.
     * @retval OT_ERROR_INVALID_STATE   `BeginFrame()` has not been called earlier to start the frame.
     *
     */
    otError WriteEui64(const otExtAddress &aExtAddress) { return WriteEui64(aExtAddress.m8); }

    /**
     * This method encodes and writes an EUI64 value to current input frame.
     *
     * Before using this method `BeginFrame()` must be called to start and prepare a new input frame. Otherwise, this
     * method does nothing and returns error status `OT_ERROR_INVALID_STATE`.
     *
     * If no buffer space is available, this method will discard and clear the current input frame and return the
     * error status `OT_ERROR_NO_BUFS`.
     *
     * @param[in]  aExtAddress          A pointer to a buffer containing the EUI64 value.
     *
     * @retval OT_ERROR_NONE            Successfully added given value to the frame.
     * @retval OT_ERROR_NO_BUFS         Insufficient buffer space available to add the EUI64 value.
     * @retval OT_ERROR_INVALID_STATE   `BeginFrame()` has not been called earlier to start the frame.
     *
     */
    otError WriteEui64(const uint8_t *aEui64) { return WriteData(aEui64, sizeof(spinel_eui64_t)); }

    /**
     * This method encodes and writes an EUI48 value to current input frame.
     *
     * Before using this method `BeginFrame()` must be called to start and prepare a new input frame. Otherwise, this
     * method does nothing and returns error status `OT_ERROR_INVALID_STATE`.
     *
     * If no buffer space is available, this method will discard and clear the current input frame and return the
     * error status `OT_ERROR_NO_BUFS`.
     *
     * @param[in]  aEui48               A reference to the EUI48 value.
     *
     * @retval OT_ERROR_NONE            Successfully added given value to the frame.
     * @retval OT_ERROR_NO_BUFS         Insufficient buffer space available to add the EUI48 value.
     * @retval OT_ERROR_INVALID_STATE   `BeginFrame()` has not been called earlier to start the frame.
     *
     */
    otError WriteEui48(const spinel_eui48_t &aEui48) { return WriteEui48(aEui48.bytes); }

    /**
     * This method encodes and writes an EUI48 value to current input frame.
     *
     * Before using this method `BeginFrame()` must be called to start and prepare a new input frame. Otherwise, this
     * method does nothing and returns error status `OT_ERROR_INVALID_STATE`.
     *
     * If no buffer space is available, this method will discard and clear the current input frame and return the
     * error status `OT_ERROR_NO_BUFS`.
     *
     * @param[in]  aEui48               A pointer to a buffer containing the EUI64 value.
     *
     * @retval OT_ERROR_NONE            Successfully added given value to the frame.
     * @retval OT_ERROR_NO_BUFS         Insufficient buffer space available to add the EUI48 value.
     * @retval OT_ERROR_INVALID_STATE   `BeginFrame()` has not been called earlier to start the frame.
     *
     */
    otError WriteEui48(const uint8_t *aEui48) { return WriteData(aEui48, sizeof(spinel_eui48_t)); }

    /**
     * This method encodes and writes a UTF8 string to current input frame.
     *
     * Before using this method `BeginFrame()` must be called to start and prepare a new input frame. Otherwise, this
     * method does nothing and returns error status `OT_ERROR_INVALID_STATE`.
     *
     * If no buffer space is available, this method will discard and clear the current input frame and return the
     * error status `OT_ERROR_NO_BUFS`.
     *
     * @param[in]  aUtf8                A const character pointer (C string).
     *
     * @retval OT_ERROR_NONE            Successfully added given string to the frame.
     * @retval OT_ERROR_NO_BUFS         Insufficient buffer space available to add the string.
     * @retval OT_ERROR_INVALID_STATE   `BeginFrame()` has not been called earlier to start the frame.
     *
     */
    otError WriteUtf8(const char *aUtf8);

    /**
     * This method encodes and writes a sequence of bytes to current input frame.
     *
     * Before using this method `BeginFrame()` must be called to start and prepare a new input frame. Otherwise, this
     * method does nothing and returns error status `OT_ERROR_INVALID_STATE`.
     *
     * If no buffer space is available, this method will discard and clear the current input frame and return the
     * error status `OT_ERROR_NO_BUFS`.
     *
     * @param[in]  aData                A pointer to data buffer.
     * @param[in]  aDataLen             The length (number of bytes) in the data buffer.
     *
     * @retval OT_ERROR_NONE            Successfully added given data to the frame.
     * @retval OT_ERROR_NO_BUFS         Insufficient buffer space available to add the byte.
     * @retval OT_ERROR_INVALID_STATE   `BeginFrame()` has not been called earlier to start the frame.
     *
     */
    otError WriteData(const uint8_t *aData, uint16_t aDataLen) { return mNcpBuffer.InFrameFeedData(aData, aDataLen); }

    /**
     * This method encodes and writes a data blob (sequence of bytes) with its length prepended before the data.
     *
     * The length of the data (in bytes) is prepended (with the length encoded as a `uint16`). The size of the length
     * field is not included in the length. This is similar to `SPINEL_DATATYPE_DATA_WLEN` type.
     *
     * Before using this method `BeginFrame()` must be called to start and prepare a new input frame. Otherwise, this
     * method does nothing and returns error status `OT_ERROR_INVALID_STATE`.
     *
     * If no buffer space is available, this method will discard and clear the current input frame and return the
     * error status `OT_ERROR_NO_BUFS`.
     *
     * @param[in]  aData                A pointer to data buffer.
     * @param[in]  aDataLen             The length (number of bytes) in the data buffer.
     *
     * @retval OT_ERROR_NONE            Successfully added given data to the frame.
     * @retval OT_ERROR_NO_BUFS         Insufficient buffer space available to add the byte.
     * @retval OT_ERROR_INVALID_STATE   `BeginFrame()` has not been called earlier to start the frame.
     *
     */
    otError WriteDataWithLen(const uint8_t *aData, uint16_t aDataLen);

#if OPENTHREAD_SPINEL_CONFIG_OPENTHREAD_MESSAGE_ENABLE
    /**
     * This method adds a message to the current input frame.
     *
     * Before using this method `BeginFrame()` must be called to start and prepare a new input frame. Otherwise, this
     * method does nothing and returns error status `OT_ERROR_INVALID_STATE`.
     *
     * If no buffer space is available, this method will discard and clear the frame and return error status
     * `OT_ERROR_NO_BUFS`.
     *
     * The ownership of the passed-in message @p aMessage changes to underlying `Spinel::Buffer` ONLY when the entire
     * frame is successfully finished (i.e., with a successful call to `EndFrame()` for the current frame being
     * written), and in this case the `otMessage` instance will be freed once the frame is removed from the
     * `Spinel::Buffer`. However, if the frame gets discarded before it is finished (e.g., running out of buffer space),
     * the  `otMessage` instance remains unchanged.
     *
     * @param[in] aMessage              A message to be added to current frame.
     *
     * @retval OT_ERROR_NONE            Successfully added the message to the frame.
     * @retval OT_ERROR_NO_BUFS         Insufficient buffer space available to add the message.
     * @retval OT_ERROR_INVALID_STATE   `BeginFrame()` has not been called earlier to start the frame.
     * @retval OT_ERROR_INVALID_ARGS    If @p aMessage is nullptr.
     *
     */
    otError WriteMessage(otMessage *aMessage) { return mNcpBuffer.InFrameFeedMessage(aMessage); }
#endif

    /**
     * This method encodes and writes a set of variables to the current input frame using a given spinel packing format
     * string.
     *
     * Before using this method `BeginFrame()` must be called to start and prepare a new input frame. Otherwise, this
     * method does nothing and returns error status `OT_ERROR_INVALID_STATE`.
     *
     * If no buffer space is available, this method will discard and clear the current input frame and return the
     * error status `OT_ERROR_NO_BUFS`.
     *
     * Note that the encoded buffer should fit in `kPackFormatBufferSize` bytes.
     *
     * @param[in]  aPackFormat          A string giving the spinel packing format.
     * @param[in]  ...                  Variable arguments corresponding to the types given in @p aPackFormat (see
     *                                  `spinel_datatype_pack()`).
     *
     * @retval OT_ERROR_NONE            Successfully added given data to the frame.
     * @retval OT_ERROR_NO_BUFS         Insufficient buffer space available to add the byte.
     * @retval OT_ERROR_INVALID_STATE   `BeginFrame()` has not been called earlier to start the frame.
     *
     */
    otError WritePacked(const char *aPackFormat, ...);

    /**
     * This method encodes and writes a set of variables to the current input frame using a given spinel packing format
     * string.
     *
     * Before using this method `BeginFrame()` must be called to start and prepare a new input frame. Otherwise, this
     * method does nothing and returns error status `OT_ERROR_INVALID_STATE`.
     *
     * If no buffer space is available, this method will discard and clear the current input frame and return the
     * error status `OT_ERROR_NO_BUFS`.
     *
     * Note that the encoded buffer should fit in `kPackFormatBufferSize` bytes.
     *
     * @param[in]  aPackFormat          A string giving the spinel packing format.
     * @param[in]  aArgs                Variable arguments corresponding to the types given in @p aPackFormat (see
     *                                  `spinel_datatype_pack()`).
     *
     * @retval OT_ERROR_NONE            Successfully added given data to the frame.
     * @retval OT_ERROR_NO_BUFS         Insufficient buffer space available to add the byte.
     * @retval OT_ERROR_INVALID_STATE   `BeginFrame()` has not been called earlier to start the frame.
     *
     */
    otError WriteVPacked(const char *aPackFormat, va_list aArgs);

    /**
     * This method opens a struct in the current input frame.
     *
     * After a successful call to this method, all the subsequent `Write<SomeType>()` methods add the field/value to
     * the current open struct until the struct is closed using `CloseStruct()` method. Structures can be nested. Up to
     * `kMaxNestedStructs` nested structs can be opened at the same time.
     *
     * Before using this method `BeginFrame()` must be called to start and prepare a new input frame. Otherwise, this
     * method does nothing and returns error status `OT_ERROR_INVALID_STATE`.
     *
     * If no buffer space is available, this method will discard and clear the frame and return error status
     * `OT_ERROR_NO_BUFS`.
     *
     * @retval OT_ERROR_NONE            Successfully opened the struct.
     * @retval OT_ERROR_NO_BUFS         Insufficient buffer space available to open the struct.
     * @retval OT_ERROR_INVALID_STATE   `BeginFrame()` has not been called earlier to start the frame or if we reached
     *                                  the maximum number of nested open structures.
     *
     */
    otError OpenStruct(void);

    /**
     * This method closes the most recently opened struct (using `OpenStruct()`) in the current input frame.
     *
     * Each call to `CloseStruct()` must correspond to an earlier successfully opened struct. If a frame is ended using
     * `EndFrame()` with remaining open structs, the `EndFrame()` method will close all the remaining structs.
     *
     * If no buffer space is available, this method will discard and clear the frame and return error status
     * `OT_ERROR_NO_BUFS`.
     *
     * @retval OT_ERROR_NONE            Successfully closed the most recently opened struct.
     * @retval OT_ERROR_NO_BUFS         Insufficient buffer space available to open the struct.
     * @retval OT_ERROR_INVALID_STATE   `BeginFrame()` has not been called earlier to start the frame or if there is no
     *                                  open struct to close
     */
    otError CloseStruct(void);

    /**
     * This method saves the current write position in the input frame.
     *
     * The saved position can later be used to discard a portion of written/encoded frame and move the write pointer
     * back to the saved position (using `ResetToSaved()`).
     *
     * @retval OT_ERROR_NONE            Successfully saved current write position in @p aPosition.
     * @retval OT_ERROR_INVALID_STATE   `BeginFrame()` has not been called earlier to start the frame.
     *
     */
    otError SavePosition(void);

    /**
     * This method resets the write position of input frame back to a previously saved position. Any added content
     * after the write position is discarded.
     *
     * The saved position must belong to the same input frame saved earlier with `SavePosition()`. This method cannot
     * be used if the input frame has an added `otMessage`.
     *
     * @retval OT_ERROR_NONE            Successfully reset the write position of current input frame.
     * @retval OT_ERROR_INVALID_STATE   `BeginFrame()` has not been called earlier to start the frame.
     * @retval OT_ERROR_INVALID_ARGS    The saved position is not valid (does not belong to same input frame), or
     *                                  the input frame has an added `otMessage`.
     *
     */
    otError ResetToSaved(void);

private:
    enum
    {
        kPackFormatBufferSize = 96, ///< Size of buffer used when encoding using `WritePacked()` or `WriteVPacked()`.
        kMaxNestedStructs     = 4,  ///< Maximum number of nested structs.
    };

    Spinel::Buffer &              mNcpBuffer;
    Spinel::Buffer::WritePosition mStructPosition[kMaxNestedStructs];
    uint8_t                       mNumOpenStructs;

    uint8_t                       mSavedNumOpenStructs;
    Spinel::Buffer::WritePosition mSavedPosition;
};

} // namespace Spinel
} // namespace ot

#endif // SPINEL_ENCODER_HPP_
