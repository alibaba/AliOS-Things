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
 *   This file contains definitions for the NCP frame buffer class.
 */

#ifndef NCP_FRAME_BUFFER_HPP_
#define NCP_FRAME_BUFFER_HPP_

#include "openthread-spinel-config.h"

#include <openthread/message.h>

namespace ot {
namespace Spinel {

/**
 * This class implements a buffer/queue for storing Ncp frames.
 *
 * A frame can consist of a sequence of data bytes and/or the content of an `otMessage` or a combination of the two.
 * `Buffer` implements priority FIFO logic for storing and reading frames. Two priority levels of high and low
 * are supported. Within same priority level first-in-first-out order is preserved. High priority frames are read
 * ahead of any low priority ones.
 *
 */
class Buffer
{
    friend class Encoder;

public:
    /**
     * Defines the priority of a frame. High priority frames are read before low priority frames. Within same priority
     * level FIFO order is preserved.
     *
     */
    enum Priority
    {
        kPriorityLow  = 0, //< Indicates low/normal priority for a frame.
        kPriorityHigh = 1, //< Indicates high priority for a frame.
    };

    /**
     * Defines the (abstract) frame tag type. The tag is a unique value (within currently queued frames) associated
     * with a frame in the `Buffer`. Frame tags can be compared with one another using operator `==`.
     *
     */
    typedef const void *FrameTag;

    /**
     * Defines the tag value indicating an invalid tag (e.g., when there is no frame).
     *
     */
    static const FrameTag kInvalidTag;

    /**
     * Defines the structure to hold a write position for an input frame (frame being written).
     *
     * It should be considered as an opaque data structure to users of `Buffer`.
     *
     */
    struct WritePosition
    {
    public:
        /**
         * The constructor for a `WritePosition` object.
         *
         */
        WritePosition(void)
            : mPosition(nullptr)
            , mSegmentHead(nullptr)
        {
        }

    private:
        uint8_t *mPosition;    //< Pointer into buffer corresponding to saved write position.
        uint8_t *mSegmentHead; //< Pointer to segment head.

        friend class Buffer;
    };

    /**
     * Defines a function pointer callback which is invoked to inform a change in `Buffer` either when a new
     * frame is added/written to `Buffer` or when a frame is removed from `Buffer`.
     *
     * @param[in] aContext              A pointer to arbitrary context information.
     * @param[in] aTag                  The tag associated with the frame which is added or removed.
     * @param[in] aPriority             The priority of frame.
     * @param[in] aBuffer               A pointer to the `Buffer`.
     *
     */
    typedef void (*BufferCallback)(void *aContext, FrameTag aTag, Priority aPriority, Buffer *aBuffer);

    /**
     * This constructor initializes an NCP frame buffer.
     *
     * @param[in] aBuffer               A pointer to a buffer which will be used by NCP frame buffer.
     * @param[in] aBufferLength         The buffer size (in bytes).
     *
     */
    Buffer(uint8_t *aBuffer, uint16_t aBufferLength);

    /**
     * This method clears the NCP frame buffer. All the frames are cleared/removed.
     *
     */
    void Clear(void);

    /**
     * This method sets the FrameAdded callback and its context.
     *
     * Subsequent calls to this method will overwrite the previous callback and its context.
     *
     * @param[in] aFrameAddedCallback   Callback invoked when a new frame is successfully added to buffer.
     * @param[in] aFrameAddedContext    A pointer to arbitrary context used with frame added callback.
     *
     */
    void SetFrameAddedCallback(BufferCallback aFrameAddedCallback, void *aFrameAddedContext);

    /**
     * This method sets the FrameRemoved callback and its context.
     *
     * Subsequent calls to this method will overwrite the previous callback and its context.
     *
     * @param[in] aFrameRemovedCallback Callback invoked when a frame is removed from buffer.
     * @param[in] aFrameRemovedContext  A pointer to arbitrary context used with frame removed callback.
     *
     */
    void SetFrameRemovedCallback(BufferCallback aFrameRemovedCallback, void *aFrameRemovedContext);

    /**
     * This method begins a new input frame (InFrame) to be added/written to the frame buffer.

     * If there is a previous frame being written (for which `InFrameEnd()` has not yet been called), calling
     * `InFrameBegin()` will discard and clear the previous unfinished frame.
     *
     * @param[in] aPriority             Priority level of the new input frame.
     *
     */
    void InFrameBegin(Priority aPriority);

    /**
     * This method adds a single byte to current input frame.
     *
     * Before using this method `InFrameBegin()` must be called to start and prepare a new input frame. Otherwise, this
     * method does nothing and returns error status `OT_ERROR_INVALID_STATE`.
     *
     * If no buffer space is available, this method will discard and clear the current input frame and return the
     * error status `OT_ERROR_NO_BUFS`.
     *
     * @param[in]  aByte                The byte value to add to input frame.
     *
     * @retval OT_ERROR_NONE            Successfully added given byte to the frame.
     * @retval OT_ERROR_NO_BUFS         Insufficient buffer space available to add the byte.
     * @retval OT_ERROR_INVALID_STATE   `InFrameBegin()` has not been called earlier to start the frame.
     *
     */
    otError InFrameFeedByte(uint8_t aByte);

    /**
     * This method adds data to the current input frame.
     *
     * Before using this method `InFrameBegin()` must be called to start and prepare a new input frame. Otherwise, this
     * method does nothing and returns error status `OT_ERROR_INVALID_STATE`.
     *
     * If no buffer space is available, this method will discard and clear the current input frame and return the
     * error status `OT_ERROR_NO_BUFS`.
     *
     * @param[in]  aDataBuffer          A pointer to data buffer.
     * @param[in]  aDataBufferLength    The length of the data buffer.
     *
     * @retval OT_ERROR_NONE            Successfully added new data to the frame.
     * @retval OT_ERROR_NO_BUFS         Insufficient buffer space available to add data.
     * @retval OT_ERROR_INVALID_STATE   `InFrameBegin()` has not been called earlier to start the frame.
     *
     */
    otError InFrameFeedData(const uint8_t *aDataBuffer, uint16_t aDataBufferLength);

#if OPENTHREAD_SPINEL_CONFIG_OPENTHREAD_MESSAGE_ENABLE
    /**
     * This method adds a message to the current input frame.
     *
     * Before using this method `InFrameBegin()` must be called to start and prepare a new input frame. Otherwise, this
     * method does nothing and returns error status `OT_ERROR_INVALID_STATE`.
     *
     * If no buffer space is available, this method will discard and clear the frame and return error status
     * `OT_ERROR_NO_BUFS`.
     *
     * The ownership of the passed-in message @p aMessage changes to `Buffer` ONLY when the entire frame is
     * successfully finished (i.e., with a successful call to `InFrameEnd()` for the current input frame), and in this
     * case the `otMessage` instance will be freed once the frame is removed (using `OutFrameRemove()`) from NCP buffer.
     * However, if the input frame gets discarded before it is finished (e.g., running out of buffer space), the
     * `otMessage` instance remains unchanged.
     *
     * @param[in] aMessage              A message to be added to current frame.
     *
     * @retval OT_ERROR_NONE            Successfully added the message to the frame.
     * @retval OT_ERROR_NO_BUFS         Insufficient buffer space available to add the message.
     * @retval OT_ERROR_INVALID_STATE   `InFrameBegin()` has not been called earlier to start the frame.
     * @retval OT_ERROR_INVALID_ARGS    If @p aMessage is nullptr.
     *
     */
    otError InFrameFeedMessage(otMessage *aMessage);
#endif

    /**
     * This method gets the current write position in the input frame.
     *
     * The write position is returned in @p aPosition. The saved position can later be used to overwrite the frame
     * content (using `InFrameOverwrite()`) or discard a portion of written frame and move the write pointer back to
     * the saved position (using `InFrameReset()`).
     *
     * @param[out] aPosition            A reference to a `WritePosition` to save the current write position.
     *
     * @retval OT_ERROR_NONE            Successfully saved current write position in @p aPosition.
     * @retval OT_ERROR_INVALID_STATE   `InFrameBegin()` has not been called earlier to start the frame.
     *
     */
    otError InFrameGetPosition(WritePosition &aPosition);

    /**
     * This method overwrites the previously written content in the current input frame at a given write position.
     *
     * The write position @p aPostion must belong to the same input frame saved earlier with `InFrameGetPosition()`.
     * This method does not allow writing beyond the current end of the input frame (i.e., it can only write over
     * previously added content). If writing @p aDataBufferLength bytes from write position @p aPosition goes beyond
     * the end, this method does not change the input frame and returns error status `OT_ERROR_INVALID_ARGS`.
     * This method cannot be used if the input frame has an added `otMessage` (i.e., a previous call to
     * `InFrameFeedMessage()`).
     *
     * @param[in] aPosition             A reference to the write position.
     * @param[in] aDataBuffer           A pointer to data buffer.
     * @param[in] aDataBufferLength     The length of the data buffer.
     *
     * @retval OT_ERROR_NONE            Successfully overwrote the data at the given write position.
     * @retval OT_ERROR_INVALID_STATE   No input frame (`InFrameBegin()` has not been called).
     * @retval OT_ERROR_INVALID_ARGS    The given write position is not valid (i.e., if it does not belong to same
     *                                  input frame), or the input frame has an added `otMessage`, or the write
     *                                  operation will go beyond the current end of the input frame.
     *
     */
    otError InFrameOverwrite(const WritePosition &aPosition, const uint8_t *aDataBuffer, uint16_t aDataBufferLength);

    /**
     * This method resets the write position of input frame back to a previously saved position. Any previously
     * added content after the write position is discarded.
     *
     * The write position @p aPosition must belong to the same input frame saved earlier with `InFrameGetPosition()`.
     * This method cannot be used if the input frame has an added `otMessage` (i.e., a previous call to
     * `InFrameFeedMessage()`).
     *
     * @param[in] aPosition             A reference to write position
     *
     * @retval OT_ERROR_NONE            Successfully reset the write position of current input frame..
     * @retval OT_ERROR_INVALID_STATE   No input frame (`InFrameBegin()` has not been called).
     * @retval OT_ERROR_INVALID_ARGS    The given write position is not valid (does not belong to same input frame), or
     *                                  the input frame has an added `otMessage`.
     *
     */
    otError InFrameReset(const WritePosition &aPosition);

    /**
     * This method gets the distance (number of bytes) from a given saved position to current end of frame.
     *
     * The write position @p aPosition must belong to the same input frame saved earlier with `InFrameGetPosition()`.
     * This method cannot be used if the input frame has an added `otMessage` (i.e., a previous call to
     * `InFrameFeedMessage()`). In case of invalid argument, this method returns zero.
     *
     * @param[in] aPosition             A reference to write position
     *
     * @returns The distance (number of bytes) from a write position to current end of frame, or zero for invalid
     *          arguments.
     *
     */
    uint16_t InFrameGetDistance(const WritePosition &aPosition) const;

    /**
     * This method finalizes/ends the current input frame being written to the buffer.
     *
     * Before using this method `InFrameBegin()` must be called to start and prepare a new input frame. Otherwise, this
     * method does nothing and returns error status `OT_ERROR_INVALID_STATE`.
     *
     * If no buffer space is available, this method will discard and clear the frame and return error status
     * `OT_ERROR_NO_BUFS`.
     *
     * @retval OT_ERROR_NONE            Successfully ended the input frame.
     * @retval OT_ERROR_NO_BUFS         Insufficient buffer space available to add message.
     * @retval OT_ERROR_INVALID_STATE   `InFrameBegin()` has not been called earlier to start the frame.
     *
     */
    otError InFrameEnd(void);

    /**
     * This method returns the tag assigned to last successfully written/added frame to NcpBuffer (i.e., last input
     * frame for which `InFrameEnd()` was called and returned success status). The tag is a unique value (within
     * currently queued frames) associated with a frame in the `Buffer`. The tag can be used to identify the
     * same frame when it is read and removed from the NcpBuffer. Tags can be compared using operator `==`.
     *
     * @returns The tag of the last successfully written frame, or `kInvalidTag` if no frame is written so far.
     */
    FrameTag InFrameGetLastTag(void) const;

    /**
     * This method checks if the buffer is empty. A non-empty buffer contains at least one full frame for reading.
     *
     * @retval TRUE                     Buffer is not empty and contains at least one full frame for reading.
     * @retval FALSE                    Buffer is empty and contains no frame for reading.
     *
     */
    bool IsEmpty(void) const;

    /**
     * This method begins/prepares an output frame to be read from the frame buffer if there is no current active output
     * frame, or resets the read offset if there is a current active output frame.
     *
     * The NCP buffer maintains a read offset for the current frame being read. Before reading any bytes from the frame
     * this method should be called to prepare the frame and set the read offset.
     *
     * If part or even all of current frame has been read, a sub-sequent call to this method  will reset the read
     * offset back to beginning of current output frame (note that the current output frame will remain unchanged even
     * in case where a higher priority frame was written to buffer while reading current output frame). A prepared
     * output frame will stay active as current output frame until it is explicitly removed using `OutFrameRemove()`.
     *
     * @retval OT_ERROR_NONE            Successfully started/prepared a new output frame for reading.
     * @retval OT_ERROR_NOT_FOUND       No frame available in buffer for reading.
     *
     */
    otError OutFrameBegin(void);

    /**
     * This method checks if the current output frame (being read) has ended.
     *
     * The NCP buffer maintains a read offset for the current output frame being read. This method returns `true` if
     * the read offset has reached the end of the frame and there are no more bytes available to read, or if there is
     * no currently active output frame.
     *
     * @retval TRUE                     Frame has ended (no more bytes available to read from current output frame), or
     *                                  there is currently no prepared/active output frame.
     * @retval FALSE                    Frame still has more data to read.
     *
     */
    bool OutFrameHasEnded(void);

    /**
     * This method reads and returns the next byte from the current output frame.
     *
     * The NCP buffer maintains a read offset for the current output frame being read. This method reads and returns
     * the next byte from the current frame and moves the read offset forward. If read offset is already at the end
     * current output frame, this method returns zero.
     *
     * @returns The next byte from the current output frame or zero if current output frame has ended or there is
     * prepared/active output from.
     *
     */
    uint8_t OutFrameReadByte(void);

    /**
     * This method reads and copies bytes from the current output frame into a given buffer.
     *
     * The NCP buffer maintains a read offset for the current output frame being read. This method attempts to read
     * the given number of bytes (@p aReadLength) from the current frame and copies the bytes into the given
     * data buffer (@p aDataBuffer). It also moves the read offset forward accordingly. If there are fewer bytes
     * remaining in current frame than the requested @p aReadLength, the available bytes are read/copied. This methods
     * returns the number of bytes read from frame and copied into @p aDataBuffer.
     *
     * @param[in]   aReadLength          Number of bytes to read.
     * @param[out]  aDataBuffer          A pointer to a data buffer.
     *
     * @returns The number of bytes read and copied into data buffer.
     *
     */
    uint16_t OutFrameRead(uint16_t aReadLength, uint8_t *aDataBuffer);

    /**
     * This method removes the current or front output frame from the buffer.
     *
     * If there is an active output from being read (an output frame was prepared earlier with a successful call to
     * `OutFrameBegin()`), this method removes the current active output frame. If there is no current active frame,
     * the front frame in the queue (the next frame which would have been read) will be removed.
     *
     * When a frame is removed all its associated messages will be freed.
     *
     * If the remove operation is successful, this method will invoke the `FrameRemovedCallback` (if not nullptr) before
     * returning the success state.
     *
     * @retval OT_ERROR_NONE            Successfully removed the front frame.
     * @retval OT_ERROR_NOT_FOUND       No frame available in NCP frame buffer to remove.
     *
     */
    otError OutFrameRemove(void);

    /**
     * This method returns the number of bytes (length) of current or front frame in the NCP frame buffer.
     *
     * If there is an active output from being read (an output frame was prepared earlier with successful call to
     * `OutFrameBegin()`), this method returns the length of the current output frame. If there is no current active
     * frame, the length of the front frame in the queue (the next frame which would have been read) will be returned.
     *
     * If there is no frame in buffer, this method returns zero.
     *
     * @returns The number of bytes (length) of current/front frame, or zero if no frame in buffer.
     *
     */
    uint16_t OutFrameGetLength(void);

    /**
     * This method returns the tag value associated to current or front frame in the NCP frame buffer.
     *
     * If there is an active output from being read (an output frame was prepared earlier with successful call to
     * `OutFrameBegin()`), this method returns the tag associated with current output frame. If there is no current
     * active frame, the tag associated with the front frame in the queue (the next frame which would have been read)
     * will be returned.
     *
     * If there is no frame in buffer, this method returns `kInvalidTag`.
     *
     * @returns The tag assigned to the current/from output frame, or `kInvalidTag` if no frame in buffer.
     *
     */
    FrameTag OutFrameGetTag(void);

private:
    /*
     * `Buffer` Implementation
     * -------------------------------
     *
     * `Buffer` internally stores a frame as a sequence of data segments. Each segment stores a portion of
     * frame. The data segments are stored in the main buffer `mBuffer`. `mBuffer` is utilized as a circular buffer.

     * The content of messages (which are added using `InFrameFeedMessage()`) are not directly copied in the `mBuffer`
     * but instead they are enqueued in a message queue `mMessageQueue`.
     *
     * Every data segments starts with a header before the data portion. The header is 2 bytes long with the following
     * format:
     *
     *    Bit 0-13: Give the length of the data segment (max segment len is 2^14 = 16,384 bytes).
     *    Bit 14:   Flag bit set to indicate that this segment has an associated `Message` (appended to its end).
     *    Bit 15:   Flag bit set to indicate that this segment defines the start of a new frame.
     *
     *        Bit  15         Bit 14                     Bits: 0 - 13
     *    +--------------+--------------+--------------------------------------------------------+
     *    |   New Frame  |  Has Message |  Length of segment (excluding the header)              |
     *    +--------------+--------------+--------------------------------------------------------+
     *
     * The header is encoded in big-endian (msb first) style.

     * Consider the following calls to create a frame:
     *
     *    ncpBuffer.InFrameBegin();
     *    ncpBuffer.InFrameFeedData("Hello", 5);
     *    ncpBuffer.InFrameFeedData("There", 5);
     *    ncpBuffer.InFrameFeedMessage(*someMessage);
     *    ncpBuffer.InFrameFeedData("Bye", 3);
     *    ncpBuffer.InFrameEnd();
     *
     * This frame is stored as two segments:
     *
     *    - Segment #1 contains "HelloThere" with a header value `0xC00A` which shows that this segment contains 10
     *      data bytes, and it starts a new frame, and also includes an appended message from the message queue.
     *
     *    - Segment #2 contains "Bye" with a header value of `0x0003` showing length of 3 and no appended message.
     *
     *    +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
     *    | C0  | 0A  | 'H' | 'e' | 'l' | 'l' | 'o' | 'T' | 'h' | 'e' | 'r' | 'e' | 00  | 03  | 'B' | 'y' | 'e' |
     *    +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
     *     \         /                                                             \         /
     *   Segment #1 Header                                                      Segment #2 Header
     *
     *
     * `Buffer` uses the `mBuffer` as a circular/ring buffer. To support two frame priorities the buffer is
     * divided into two high-priority and low-priority regions. The high priority frames are stored in buffer in
     * backward direction while the low-priority frames use the buffer in forward direction. This model ensures the
     * available buffer space is utilized efficiently between all frame types.
     *
     *                                       mReadFrameStart[kPriorityLow]
     *                                                 |
     *                                                 |                   mWriteFrameStart[kPriorityLow]
     *                                                 |                              |
     *                                                 V    Low Priority Frames       V
     *   --------------+------------------------------+------------------------------+------------------
     *             ... |        <--------             |         -------->            | ...
     *   --------------+------------------------------+------------------------------+------------------
     *                ^       High Priority Frames   ^
     *                |                              |
     *                |                    mReadFrameStart[kPriorityHigh]
     *                |
     *          mWriteFrameStart[kPriorityHigh]
     *
     *
     *
     * When frames are removed, if possible, the `mReadFrameStart` and `mWriteFrameStart` pointers of the two priority
     * levels are moved closer to avoid gaps.
     *
     * For an output frame (frame being read), Buffer maintains a `ReadState` along with a set of pointers
     * into the buffer:
     *
     *             mReadFrameStart[priority]: Start of the current/front frame.
     *             |
     *             |            mReadSegmentHead: Start of the current segment.
     *             |            |
     *             |            |              mReadPointer: Pointer to the next byte to read.
     *             |            |              |
     *             |            |              |           mReadSegmentTail: End of the current segment.
     *             |            |              |           |
     *             V            V              V           V
     *   ---------+------------+--------------------------+------+----------------+-----------------------------------
     *       ...  | Segment 1  | Segment  2               | ...  | Last Segment   | ... (possible) next frame
     *   ---------+------------+--------------------------+------+----------------+-----------------------------------
     *             \           |                         |                       /
     *              |          \------------v-----------/                       |
     *              |                   Current Segment                         |
     *              |                                                           |
     *               \---------------------------V-----------------------------/
     *                              Current OutFrame (being read)
     *
     * Note that the diagram above shows the pointers for a low-priority frame (with pointers increasing in forward
     * direction).
     *
     * The `ReadState` indicates the state of current output frame and its read offset (e.g., if read offset is in
     * middle of a segment or if it is is middle of an appended message, or if we are done with entire frame).
     *
     * For an input frame (frame being written), the following pointers are maintained:
     *
     *            mWriteFrameWrite[priority]: Start of the current/next frame being written.
     *                       |
     *                       |      mWriteSegmentHead: Start of the current segment of the active input frame.
     *                       |                 |
     *                       |                 |   mWriteSegmentTail: Pointer to the next byte to write.
     *                       |                 |                       |
     *                       |                 |                       |
     *                       |                 |                       |
     *                       V                 V                       V
     *    ------------------+------------------+-------------------------------------------------------------------
     *    Previous Frames   | Segment 1        | Segment  2            : . . .
     *    ------------------+------------------+-------------------------------------------------------------------
     *                       \                                        /
     *                        |                                      |
     *                         \------------------V-----------------/
     *                             Current InFrame (being written)
     *
     *
     */

    enum
    {
        kReadByteAfterFrameHasEnded = 0,      // Value returned by ReadByte() when frame has ended.
        kMessageReadBufferSize      = 16,     // Size of message buffer array `mMessageBuffer`.
        kUnknownFrameLength         = 0xffff, // Value used when frame length is unknown.
        kSegmentHeaderSize          = 2,      // Length of the segment header.
        kSegmentHeaderLengthMask    = 0x3fff, // Bit mask to get the length from the segment header
        kMaxSegments                = 10,     // Max number of segments allowed in a frame

        kSegmentHeaderNoFlag               = 0,         // No flags are set.
        kSegmentHeaderNewFrameFlag         = (1 << 15), // Indicates that this segment starts a new frame.
        kSegmentHeaderMessageIndicatorFlag = (1 << 14), // Indicates this segment ends with a Message.

        kNumPrios = (kPriorityHigh + 1), // Number of priorities.
    };

    enum ReadState
    {
        kReadStateNotActive, // No current prepared output frame.
        kReadStateInSegment, // In middle of a data segment while reading current frame.
        kReadStateInMessage, // In middle of a message while reading current frame.
        kReadStateDone,      // Current output frame is read fully.
    };

    enum Direction
    {
        kForward  = kPriorityLow,
        kBackward = kPriorityHigh,
        kUnknown,
    };

    uint8_t *GetUpdatedBufPtr(uint8_t *aBufPtr, uint16_t aOffset, Direction aDirection) const;
    uint16_t GetDistance(const uint8_t *aStartPtr, const uint8_t *aEndPtr, Direction aDirection) const;

    uint16_t ReadUint16At(uint8_t *aBufPtr, Direction aDirection);
    void     WriteUint16At(uint8_t *aBufPtr, uint16_t aValue, Direction aDirection);

    bool HasFrame(Priority aPriority) const;
    void UpdateReadWriteStartPointers(void);

    otError InFrameAppend(uint8_t aByte);
    otError InFrameBeginSegment(void);
    void    InFrameEndSegment(uint16_t aSegmentHeaderFlags);
    void    InFrameDiscard(void);
    bool    InFrameIsWriting(Priority aPriority) const;

    void    OutFrameSelectReadDirection(void);
    otError OutFramePrepareSegment(void);
    void    OutFrameMoveToNextSegment(void);

#if OPENTHREAD_SPINEL_CONFIG_OPENTHREAD_MESSAGE_ENABLE
    otError OutFramePrepareMessage(void);
    otError OutFrameFillMessageBuffer(void);
#endif

    uint8_t *const mBuffer;       // Pointer to the buffer used to store the data.
    uint8_t *const mBufferEnd;    // Points to after the end of buffer.
    const uint16_t mBufferLength; // Length of the buffer.

    BufferCallback mFrameAddedCallback;   // Callback to signal when a new frame is added
    void *         mFrameAddedContext;    // Context passed to `mFrameAddedCallback`.
    BufferCallback mFrameRemovedCallback; // Callback to signal when a frame is removed.
    void *         mFrameRemovedContext;  // Context passed to `mFrameRemovedCallback`.

    Direction mWriteDirection;             // Direction (priority) for current frame being read.
    uint8_t * mWriteFrameStart[kNumPrios]; // Pointer to start of current frame being written.
    uint8_t * mWriteSegmentHead;           // Pointer to start of current segment in the frame being written.
    uint8_t * mWriteSegmentTail;           // Pointer to end of current segment in the frame being written.
    FrameTag  mWriteFrameTag;              // Tag associated with last successfully written frame.

    Direction mReadDirection;   // Direction (priority) for current frame being read.
    ReadState mReadState;       // Read state.
    uint16_t  mReadFrameLength; // Length of current frame being read.

    uint8_t *mReadFrameStart[kNumPrios]; // Pointer to start of current frame being read.
    uint8_t *mReadSegmentHead;           // Pointer to start of current segment in the frame being read.
    uint8_t *mReadSegmentTail;           // Pointer to end of current segment in the frame being read.
    uint8_t *mReadPointer;               // Pointer to next byte to read (either in segment or in msg buffer).

#if OPENTHREAD_SPINEL_CONFIG_OPENTHREAD_MESSAGE_ENABLE
    otMessageQueue mWriteFrameMessageQueue;                // Message queue for the current frame being written.
    otMessageQueue mMessageQueue[kNumPrios];               // Main message queues.
    otMessage *    mReadMessage;                           // Current Message in the frame being read.
    uint16_t       mReadMessageOffset;                     // Offset within current message being read.
    uint8_t        mMessageBuffer[kMessageReadBufferSize]; // Buffer to hold part of current message being read.
    uint8_t *      mReadMessageTail;                       // Pointer to end of current part in mMessageBuffer.
#endif
};

} // namespace Spinel
} // namespace ot

#endif // NCP_FRAME_BUFFER_HPP_
