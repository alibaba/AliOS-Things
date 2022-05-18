/*
 *    Copyright (c) 2016, The OpenThread Authors.
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
 *   This file includes definitions for an HDLC-lite encoder and decoder.
 */

#ifndef HDLC_HPP_
#define HDLC_HPP_

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <openthread/error.h>

#include "common/code_utils.hpp"
#include "common/debug.hpp"
#include "common/encoding.hpp"

namespace ot {

/**
 * @namespace ot::Hdlc
 *
 * @brief
 *   This namespace includes definitions for the HDLC-lite encoder and decoder.
 *
 */
namespace Hdlc {

/**
 * This class defines a frame write pointer used by `Hdlc::Encoder` or `Hdlc::Decoder`.
 *
 * This class defines the minimum set of APIs used by `Encoder/Decoder` for writing an encoded/decoded frame. It is
 * simply a wrapper over a pointer into a buffer indicating where next byte should be written. Along with a write
 * pointer, this class stores a remaining length variable indicating number of remaining bytes that can be written into
 * the buffer.
 *
 * @note This class does NOT define the underlying buffer space or how it is being managed.
 *
 * `Encoder` or `Decoder` users are expected to use sub-classes of this class adding the buffer space and implementing
 * the frame buffer management scheme.
 *
 * Two template sub-class `FrameBuffer` and `MultiFrameBuffer` are defined which respectively allow storing a single
 * frame or multiple frames (FIFO queue of frame) in a buffer of a given size.
 *
 */
class FrameWritePointer
{
public:
    /**
     * This method indicates whether there is buffer space available to write @p aWriteLength bytes.
     *
     * param[in] aWriteLength       Number of bytes to write.
     *
     * @retval TRUE                 Enough buffer space is available to write the requested number of bytes.
     * @retval FALSE                Insufficient buffer space to write the requested number of bytes.
     *
     */
    bool CanWrite(uint16_t aWriteLength) const { return (mRemainingLength >= aWriteLength); }

    /**
     * This method writes a byte into the buffer and updates the write pointer (if space is available).
     *
     * @retval OT_ERROR_NONE     Successfully wrote the byte and updated the pointer.
     * @retval OT_ERROR_NO_BUFS  Insufficient buffer space to write the byte.
     *
     */
    otError WriteByte(uint8_t aByte)
    {
        return CanWrite(sizeof(uint8_t)) ? (*mWritePointer++ = aByte, mRemainingLength--, OT_ERROR_NONE)
                                         : OT_ERROR_NO_BUFS;
    }

    /**
     * This method undoes the last @p aUndoLength writes, removing them from frame.
     *
     * @note Caller should ensure that @p aUndoLength is less than or equal to the number of previously written bytes
     * into the frame. This method does not perform any checks and its behavior is undefined if @p aUndoLength is
     * larger than the number of bytes previously written into the frame.
     *
     * @param[in]  aUndoLength   Number of bytes to remove (number of last `WriteByte()` calls to undo).
     *
     */
    void UndoLastWrites(uint16_t aUndoLength)
    {
        mWritePointer -= aUndoLength;
        mRemainingLength += aUndoLength;
    }

protected:
    FrameWritePointer(void)
        : mWritePointer(nullptr)
        , mRemainingLength(0)
    {
    }

    uint8_t *mWritePointer;    ///< A pointer to current write position in the buffer.
    uint16_t mRemainingLength; ///< Number of remaining bytes available to write.
};

/**
 * This class defines a template frame buffer of a given size for storing a single frame.
 *
 * The template parameter `kSize` specifies the size of the buffer.
 *
 */
template <uint16_t kSize> class FrameBuffer : public FrameWritePointer
{
public:
    /**
     * This constructor initializes the `FrameBuffer` object.
     *
     */
    FrameBuffer(void)
        : FrameWritePointer()
    {
        Clear();
    }

    /**
     * This method clears the buffer, moving the write pointer to the beginning of the buffer.
     *
     */
    void Clear(void)
    {
        mWritePointer    = mBuffer;
        mRemainingLength = sizeof(mBuffer);
    }

    /**
     * This method indicates whether the buffer is empty or contains a frame.
     *
     * @retval TRUE  Buffer is empty
     * @retval FALSE Buffer contains a frame
     *
     */
    bool IsEmpty(void) const { return (mWritePointer == mBuffer); }

    /**
     * This method gets the length (number of bytes) in the frame.
     *
     * @returns The length (number of bytes) in the frame.
     *
     */
    uint16_t GetLength(void) const { return static_cast<uint16_t>(mWritePointer - mBuffer); }

    /**
     * This method gets a pointer to start of the frame.
     *
     * @returns A pointer to start of the frame.
     *
     */
    uint8_t *GetFrame(void) { return mBuffer; }

private:
    uint8_t mBuffer[kSize];
};

/**
 * This class defines a template frame buffer of a given size for storing multiple frames.
 *
 * The template parameter `kSize` specifies the total size of the buffer.
 *
 * Unlike `FrameBuffer` class where a single frame can be stored, this class is capable of saving multiple frames
 * in a FIFO queue format.
 *
 */
template <uint16_t kSize> class MultiFrameBuffer : public FrameWritePointer
{
public:
    /**
     * This constructor initializes the `MultiFrameBuffer` object.
     *
     */
    MultiFrameBuffer(void)
        : FrameWritePointer()
    {
        Clear();
    }

    /**
     * This method clears the buffer, removing current frame and all previously saved frames.
     *
     * It moves the write pointer to the beginning of the buffer.
     *
     */
    void Clear(void)
    {
        mWriteFrameStart = mBuffer;
        mWritePointer    = mBuffer + kHeaderSize;
        mRemainingLength = kSize - kHeaderSize;

        IgnoreError(SetSkipLength(0));
    }

    /**
     * This method indicates whether the current frame (being written) is empty or not.
     *
     * @retval TRUE  Current frame is empty.
     * @retval FALSE Current frame is not empty.
     *
     */
    bool HasFrame(void) const { return (mWritePointer != GetFrame()); }

    /**
     * This method sets the length (number of bytes) of the current frame being written.
     *
     * param[in] aLength  The length of current frame.
     *
     * @retval OT_ERROR_NONE     Successfully set the length of the current frame.
     * @retval OT_ERROR_NO_BUFS  Insufficient buffer space to hold a frame of length @p aLength.
     *
     */
    otError SetLength(uint16_t aLength)
    {
        otError error = OT_ERROR_NO_BUFS;

        if (GetFrame() + aLength <= OT_ARRAY_END(mBuffer))
        {
            mWritePointer    = GetFrame() + aLength;
            mRemainingLength = static_cast<uint16_t>(mBuffer + kSize - mWritePointer);
            error            = OT_ERROR_NONE;
        }

        return error;
    }

    /**
     * This method gets the length (number of bytes) in the current frame being written into the buffer.
     *
     * @returns The length (number of bytes) in the frame.
     *
     */
    uint16_t GetLength(void) const { return static_cast<uint16_t>(mWritePointer - GetFrame()); }

    /**
     * This method sets the length (number of bytes) of reserved buffer in front of the current frame being written.
     *
     * param[in] aSkipLength  The length of reserved buffer.
     *
     * @retval OT_ERROR_NONE     Successfully set the length of reserved buffer.
     * @retval OT_ERROR_NO_BUFS  Insufficient buffer space to hold a reserved buffer of length @p aLength.
     *
     */
    otError SetSkipLength(uint16_t aSkipLength)
    {
        otError error = OT_ERROR_NO_BUFS;

        if (mWriteFrameStart + kHeaderSize + aSkipLength <= OT_ARRAY_END(mBuffer))
        {
            Encoding::LittleEndian::WriteUint16(aSkipLength, mWriteFrameStart + kHeaderSkipLengthOffset);
            mWritePointer    = GetFrame();
            mRemainingLength = static_cast<uint16_t>(mBuffer + kSize - mWritePointer);
            error            = OT_ERROR_NONE;
        }

        return error;
    }

    /**
     * This method gets the length (number of bytes) of reserved buffer in front of the current frame being written.
     *
     * @returns The length (number of bytes) of the reserved buffer.
     *
     */
    uint16_t GetSkipLength(void) const
    {
        return Encoding::LittleEndian::ReadUint16(mWriteFrameStart + kHeaderSkipLengthOffset);
    }

    /**
     * This method gets a pointer to the start of the current frame.
     *
     * @returns A pointer to the start of the frame.
     *
     */
    uint8_t *GetFrame(void) const { return mWriteFrameStart + kHeaderSize + GetSkipLength(); }

    /**
     * This method gets the maximum length of the current frame.
     *
     * @returns The maximum length of the current frame.
     *
     */
    uint16_t GetFrameMaxLength(void) const { return static_cast<uint16_t>(mBuffer + kSize - GetFrame()); }

    /**
     * This method saves the current frame and prepares the write pointer for a next frame to be written into the
     * buffer.
     *
     * Saved frame can be retrieved later using `GetNextSavedFrame()`.
     *
     */
    void SaveFrame(void)
    {
        Encoding::LittleEndian::WriteUint16(GetSkipLength() + GetLength(), mWriteFrameStart + kHeaderTotalLengthOffset);
        mWriteFrameStart = mWritePointer;
        IgnoreError(SetSkipLength(0));
        mWritePointer    = GetFrame();
        mRemainingLength = static_cast<uint16_t>(mBuffer + kSize - mWritePointer);
    }

    /**
     * This method discards the current frame and prepares the write pointer for a next frame to be written into the
     * buffer.
     *
     */
    void DiscardFrame(void)
    {
        IgnoreError(SetSkipLength(0));

        mWritePointer    = GetFrame();
        mRemainingLength = static_cast<uint16_t>(mBuffer + kSize - mWritePointer);
    }

    /**
     * This method indicates whether there are any saved frames in the buffer.
     *
     * @retval TRUE  There is at least one saved frame in the buffer.
     * @retval FALSE There is no saved frame in the buffer.
     *
     */
    bool HasSavedFrame(void) const { return (mWriteFrameStart != mBuffer); }

    /**
     * This method iterates through previously saved frames in the buffer, getting a next frame in the queue.
     *
     * @param[inout] aFrame   On entry, should point to a previous saved frame or nullptr to get the first frame.
     *                        On exit, the pointer variable is updated to next frame or set to nullptr if there are
     * none.
     * @param[inout] aLength  On entry, should be a reference to the frame length of the previous saved frame.
     *                        On exit, the reference is updated to the frame length (number of bytes) of next frame.
     *
     * @retval OT_ERROR_NONE       Updated @aFrame and @aLength successfully with the next saved frame.
     * @retval OT_ERROR_NOT_FOUND  No more saved frame in the buffer.
     *
     */
    otError GetNextSavedFrame(uint8_t *&aFrame, uint16_t &aLength)
    {
        otError error = OT_ERROR_NONE;

        OT_ASSERT(aFrame == nullptr || (mBuffer <= aFrame && aFrame < OT_ARRAY_END(mBuffer)));

        aFrame = (aFrame == nullptr) ? mBuffer : aFrame + aLength;

        if (aFrame != mWriteFrameStart)
        {
            uint16_t totalLength = Encoding::LittleEndian::ReadUint16(aFrame + kHeaderTotalLengthOffset);
            uint16_t skipLength  = Encoding::LittleEndian::ReadUint16(aFrame + kHeaderSkipLengthOffset);

            aLength = totalLength - skipLength;
            aFrame += kHeaderSize + skipLength;
        }
        else
        {
            aLength = 0;
            aFrame  = nullptr;
            error   = OT_ERROR_NOT_FOUND;
        }

        return error;
    }

    /**
     * This method clears all saved frames from the buffer and adjusts all the pointers.
     *
     * @note This method moves the pointers into the buffer and also copies the content. Any previously retrieved
     * pointer to buffer (from `GetFrame()` or `GetNextSavedFrame()`) should be considered invalid after calling this
     * method.
     *
     */
    void ClearSavedFrames(void)
    {
        uint16_t len = static_cast<uint16_t>(mWriteFrameStart - mBuffer);

        if (len > 0)
        {
            memmove(mBuffer, mWriteFrameStart, static_cast<uint16_t>(mWritePointer - mWriteFrameStart));
            mWritePointer -= len;
            mWriteFrameStart -= len;
            mRemainingLength += len;
        }
    }

private:
    /*
     * The diagram below illustrates the format of a saved frame.
     *
     *  +---------+-------------+------------+----------------+----------------------------+
     *  | Octets: |      2      |      2     |   SkipLength   |  TotalLength - SkipLength  |
     *  +---------+-------------+------------+----------------+----------------------------+
     *  | Fields: | TotalLength | SkipLength | ReservedBuffer |         FrameBuffer        |
     *  +---------+-------------+------------+----------------+----------------------------+
     *
     *   -  "TotalLength"   : The total length of the `ReservedBuffer` and `FrameBuffer`. It is stored in header bytes
     *                        as a `uint16_t` value using little-endian encoding.
     *   -  "SkipLength"    : The length of the `ReservedBuffer`. It is stored in header bytes as a `uint16_t` value
     *                        using little-endian encoding.
     *   -  "ReservedBuffer": A reserved buffer in front of `FrameBuffer`. User can use it to store extra header, etc.
     *   -  "FrameBuffer"   : Frame buffer.
     *
     * The diagram below illustrates how the frames are saved in the buffer.
     *
     * The diagram shows `mBuffer` and different pointers into the buffer. It represents buffer state when there are
     * two saved frames in the buffer.
     *
     *          Saved frame #1           Saved frame #2       Current frame being written
     *   /                        \ /                      \ /                           \
     *   +-----------+-------------+-----------+------------+---------+--------------------------------------------+
     *   | header #1 |   ...       | header #2 |  ...       | header  |  ...             | ...                     |
     *   +-----------+-------------+-----------+------------+---------+--------------------------------------------+
     *   ^                                                  ^                            ^\                       /^
     *   |                                                  |                            |   mRemainingLength      |
     *  mBuffer[0]                                          mWriteFrameStart             |                         |
     *                                                                                   |              mBuffer[kSize]
     *                                                                                 mWritePointer
     */

    enum
    {
        kHeaderTotalLengthOffset = 0,
        kHeaderSkipLengthOffset  = sizeof(uint16_t),
        kHeaderSize              = sizeof(uint16_t) + sizeof(uint16_t),
    };

    uint8_t  mBuffer[kSize];
    uint8_t *mWriteFrameStart; // Pointer to start of current frame being written.
};

/**
 * This class implements the HDLC-lite encoder.
 *
 */
class Encoder
{
public:
    /**
     * This constructor initializes the object.
     *
     * @param[in] aWritePointer   The `FrameWritePointer` used by `Encoder` to write the encoded frames.
     *
     */
    explicit Encoder(FrameWritePointer &aWritePointer);

    /**
     * This method begins an HDLC frame.
     *
     * @retval OT_ERROR_NONE     Successfully started the HDLC frame.
     * @retval OT_ERROR_NO_BUFS  Insufficient buffer space available to start the HDLC frame.
     *
     */
    otError BeginFrame(void);

    /**
     * This method encodes a single byte into current frame.
     *
     * If there is no space to add the byte, the write pointer in frame buffer remains the same.
     *
     * @param[in]    aByte       A byte value to encode and add to frame.
     *
     * @retval OT_ERROR_NONE     Successfully encoded and added the byte to frame buffer.
     * @retval OT_ERROR_NO_BUFS  Insufficient buffer space available to encode and add the byte.
     *
     */
    otError Encode(uint8_t aByte);

    /**
     * This method encodes a given block of data into current frame.
     *
     * This method returns success only if there is space in buffer to encode the entire block of data. If there is no
     * space to encode the entire block of data, the write pointer in frame buffer remains the same.
     *
     * @param[in]    aData       A pointer to a buffer containing the data to encode.
     * @param[in]    aLength     The number of bytes in @p aData.
     *
     * @retval OT_ERROR_NONE     Successfully encoded and added the data to frame.
     * @retval OT_ERROR_NO_BUFS  Insufficient buffer space available to add the frame.
     *
     */
    otError Encode(const uint8_t *aData, uint16_t aLength);

    /**
     * This method ends/finalizes the HDLC frame.
     *
     * @retval OT_ERROR_NONE     Successfully ended the HDLC frame.
     * @retval OT_ERROR_NO_BUFS  Insufficient buffer space available to end the HDLC frame.
     *
     */
    otError EndFrame(void);

private:
    FrameWritePointer &mWritePointer;
    uint16_t           mFcs;
};

/**
 * This class implements the HDLC-lite decoder.
 *
 */
class Decoder
{
public:
    /**
     * This function pointer is called when either a complete frame has been decoded or an error occurs during
     * decoding.
     *
     * The decoded frame (or the partially decoded frame in case of an error) is available in `aFrameWritePointer`
     * buffer given in `Decoder` constructor.
     *
     * @param[in] aContext A pointer to arbitrary context information.
     * @param[in] aError   OT_ERROR_NONE    if the frame was decoded successfully,
     *                     OT_ERROR_PARSE   if the Frame Check Sequence (FCS) was incorrect in decoded frame,
     *                     OT_ERROR_NO_BUFS insufficient buffer space available to save the decoded frame.
     *
     */
    typedef void (*FrameHandler)(void *aContext, otError aError);

    /**
     * This constructor initializes the decoder.
     *
     * @param[in] aFrameWritePointer   The `FrameWritePointer` used by `Decoder` to write the decoded frames.
     * @param[in] aFrameHandler        The frame handler callback function pointer.
     * @param[in] aContext             A pointer to arbitrary context information.
     *
     */
    Decoder(FrameWritePointer &aFrameWritePointer, FrameHandler aFrameHandler, void *aContext);

    /**
     * This method feeds a block of data into the decoder.
     *
     * If during decoding, a full HDLC frame is successfully decoded or an error occurs, the `FrameHandler` callback
     * is called. The decoded frame (or the partially decoded frame in case of an error) is available in
     * `aFrameWritePointer` buffer from the constructor. The `Decoder` user (if required) must update/reset the write
     * pointer from this callback for the next frame to be decoded.
     *
     * @param[in]  aData    A pointer to a buffer containing data to be fed to decoder.
     * @param[in]  aLength  The number of bytes in @p aData.
     *
     */
    void Decode(const uint8_t *aData, uint16_t aLength);

    /**
     * This method resets internal states of the decoder.
     *
     */
    void Reset(void);

private:
    enum State
    {
        kStateNoSync,
        kStateSync,
        kStateEscaped,
    };

    State              mState;
    FrameWritePointer &mWritePointer;
    FrameHandler       mFrameHandler;
    void *             mContext;
    uint16_t           mFcs;
    uint16_t           mDecodedLength;
};

} // namespace Hdlc
} // namespace ot

#endif // HDLC_HPP_
