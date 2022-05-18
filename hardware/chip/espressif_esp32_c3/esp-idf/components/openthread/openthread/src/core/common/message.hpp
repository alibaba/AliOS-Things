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
 *   This file includes definitions for the message buffer pool and message buffers.
 */

#ifndef MESSAGE_HPP_
#define MESSAGE_HPP_

#include "openthread-core-config.h"

#include <stdint.h>

#include <openthread/message.h>
#include <openthread/platform/messagepool.h>

#include "common/code_utils.hpp"
#include "common/const_cast.hpp"
#include "common/data.hpp"
#include "common/encoding.hpp"
#include "common/linked_list.hpp"
#include "common/locator.hpp"
#include "common/non_copyable.hpp"
#include "common/pool.hpp"
#include "common/type_traits.hpp"
#include "mac/mac_types.hpp"
#include "thread/child_mask.hpp"
#include "thread/link_quality.hpp"

/**
 * This struct represents an opaque (and empty) type for an OpenThread message buffer.
 *
 */
struct otMessage
{
};

namespace ot {

namespace Crypto {

class Sha256;
class HmacSha256;

} // namespace Crypto

/**
 * @addtogroup core-message
 *
 * @brief
 *   This module includes definitions for the message buffer pool and message buffers.
 *
 * @{
 *
 */

/**
 * This macro frees a given message buffer if not nullptr.
 *
 * This macro and the ones that follow contain small but common code patterns used in many of the core modules. They
 * are intentionally defined as macros instead of inline methods/functions to ensure that they are fully inlined.
 * Note that an `inline` method/function is not necessarily always inlined by the toolchain and not inlining such
 * small implementations can add a rather large code-size overhead.
 *
 * @param[in] aMessage    A pointer to a `Message` to free (can be nullptr).
 *
 */
#define FreeMessage(aMessage)      \
    do                             \
    {                              \
        if ((aMessage) != nullptr) \
        {                          \
            (aMessage)->Free();    \
        }                          \
    } while (false)

/**
 * This macro frees a given message buffer if a given `Error` indicates an error.
 *
 * The parameter @p aMessage can be nullptr in which case this macro does nothing.
 *
 * @param[in] aMessage    A pointer to a `Message` to free (can be nullptr).
 * @param[in] aError      The `Error` to check.
 *
 */
#define FreeMessageOnError(aMessage, aError)                     \
    do                                                           \
    {                                                            \
        if (((aError) != kErrorNone) && ((aMessage) != nullptr)) \
        {                                                        \
            (aMessage)->Free();                                  \
        }                                                        \
    } while (false)

/**
 * This macro frees a given message buffer if a given `Error` indicates an error and sets the `aMessage` to `nullptr`.
 *
 * @param[in] aMessage    A pointer to a `Message` to free (can be nullptr).
 * @param[in] aError      The `Error` to check.
 *
 */
#define FreeAndNullMessageOnError(aMessage, aError)              \
    do                                                           \
    {                                                            \
        if (((aError) != kErrorNone) && ((aMessage) != nullptr)) \
        {                                                        \
            (aMessage)->Free();                                  \
            (aMessage) = nullptr;                                \
        }                                                        \
    } while (false)

constexpr uint16_t kNumBuffers = OPENTHREAD_CONFIG_NUM_MESSAGE_BUFFERS;
constexpr uint16_t kBufferSize = OPENTHREAD_CONFIG_MESSAGE_BUFFER_SIZE;

class Message;
class MessagePool;
class MessageQueue;
class PriorityQueue;
class ThreadLinkInfo;

/**
 * This class represents a Message buffer.
 *
 */
class Buffer : public otMessageBuffer, public LinkedListEntry<Buffer>
{
    friend class Message;

public:
    /**
     * This method returns a pointer to the next message buffer.
     *
     * @returns A pointer to the next message buffer.
     *
     */
    Buffer *GetNextBuffer(void) { return GetNext(); }

    /**
     * This method returns a pointer to the next message buffer.
     *
     * @returns A pointer to the next message buffer.
     *
     */
    const Buffer *GetNextBuffer(void) const { return GetNext(); }

    /**
     * This method sets the pointer to the next message buffer.
     *
     * @param[in] aNext  A pointer to the next buffer.
     *
     */
    void SetNextBuffer(Buffer *aNext) { SetNext(aNext); }

protected:
    struct Metadata
    {
        Message *    mNext;        // Next message in a doubly linked list.
        Message *    mPrev;        // Previous message in a doubly linked list.
        MessagePool *mMessagePool; // Message pool for this message.
        void *       mQueue;       // The queue where message is queued (if any). Queue type from `mInPriorityQ`.
        uint32_t     mDatagramTag; // The datagram tag used for 6LoWPAN frags or IPv6fragmentation.
        uint16_t     mReserved;    // Number of reserved bytes (for header).
        uint16_t     mLength;      // Current message length (number of bytes).
        uint16_t     mOffset;      // A byte offset within the message.
        uint16_t     mMeshDest;    // Used for unicast non-link-local messages.
        uint16_t     mPanId;       // PAN ID (used for MLE Discover Request and Response).
        uint8_t      mChannel;     // The message channel (used for MLE Announce).
        uint8_t      mTimeout;     // Seconds remaining before dropping the message.
        RssAverager  mRssAverager; // The averager maintaining the received signal strength (RSS) average.
#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
        LqiAverager mLqiAverager; // The averager maintaining the Link quality indicator (LQI) average.
#endif
        ChildMask mChildMask; // ChildMask to indicate which sleepy children need to receive this.

        uint8_t mType : 3;          // The message type.
        uint8_t mSubType : 4;       // The message sub type.
        bool    mDirectTx : 1;      // Whether a direct transmission is required.
        bool    mLinkSecurity : 1;  // Whether link security is enabled.
        uint8_t mPriority : 2;      // The message priority level (higher value is higher priority).
        bool    mInPriorityQ : 1;   // Whether the message is queued in normal or priority queue.
        bool    mTxSuccess : 1;     // Whether the direct tx of the message was successful.
        bool    mDoNotEvict : 1;    // Whether this message may be evicted.
        bool    mMulticastLoop : 1; // Whether this multicast message may be looped back.
#if OPENTHREAD_CONFIG_MULTI_RADIO
        uint8_t mRadioType : 2;      // The radio link type the message was received on, or should be sent on.
        bool    mIsRadioTypeSet : 1; // Whether the radio type is set.
        static_assert(Mac::kNumRadioTypes <= (1 << 2), "mRadioType bitfield cannot store all radio type values");
#endif
#if OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
        bool    mTimeSync : 1;      // Whether the message is also used for time sync purpose.
        int64_t mNetworkTimeOffset; // The time offset to the Thread network time, in microseconds.
        uint8_t mTimeSyncSeq;       // The time sync sequence.
#endif
    };

    static_assert(kBufferSize > sizeof(Metadata) + sizeof(otMessageBuffer), "Metadata does not fit in a single buffer");

    static constexpr uint16_t kBufferDataSize     = kBufferSize - sizeof(otMessageBuffer);
    static constexpr uint16_t kHeadBufferDataSize = kBufferDataSize - sizeof(Metadata);

    Metadata &      GetMetadata(void) { return mBuffer.mHead.mMetadata; }
    const Metadata &GetMetadata(void) const { return mBuffer.mHead.mMetadata; }

    uint8_t *      GetFirstData(void) { return mBuffer.mHead.mData; }
    const uint8_t *GetFirstData(void) const { return mBuffer.mHead.mData; }

    uint8_t *      GetData(void) { return mBuffer.mData; }
    const uint8_t *GetData(void) const { return mBuffer.mData; }

private:
    union
    {
        struct
        {
            Metadata mMetadata;
            uint8_t  mData[kHeadBufferDataSize];
        } mHead;
        uint8_t mData[kBufferDataSize];
    } mBuffer;
};

static_assert(sizeof(Buffer) >= kBufferSize, "Buffer size if not valid");

/**
 * This class represents a message.
 *
 */
class Message : public otMessage, public Buffer
{
    friend class Checksum;
    friend class Crypto::HmacSha256;
    friend class Crypto::Sha256;
    friend class MessagePool;
    friend class MessageQueue;
    friend class PriorityQueue;

public:
    /**
     * This enumeration represents the message type.
     *
     */
    enum Type : uint8_t
    {
        kTypeIp6          = 0, ///< A full uncompressed IPv6 packet
        kType6lowpan      = 1, ///< A 6lowpan frame
        kTypeSupervision  = 2, ///< A child supervision frame.
        kTypeMacEmptyData = 3, ///< An empty MAC data frame.
        kTypeOther        = 4, ///< Other (data) message.
    };

    /**
     * This enumeration represents the message sub-type.
     *
     */
    enum SubType : uint8_t
    {
        kSubTypeNone                   = 0,  ///< None
        kSubTypeMleAnnounce            = 1,  ///< MLE Announce
        kSubTypeMleDiscoverRequest     = 2,  ///< MLE Discover Request
        kSubTypeMleDiscoverResponse    = 3,  ///< MLE Discover Response
        kSubTypeJoinerEntrust          = 4,  ///< Joiner Entrust
        kSubTypeMplRetransmission      = 5,  ///< MPL next retransmission message
        kSubTypeMleGeneral             = 6,  ///< General MLE
        kSubTypeJoinerFinalizeResponse = 7,  ///< Joiner Finalize Response
        kSubTypeMleChildUpdateRequest  = 8,  ///< MLE Child Update Request
        kSubTypeMleDataResponse        = 9,  ///< MLE Data Response
        kSubTypeMleChildIdRequest      = 10, ///< MLE Child ID Request
        kSubTypeMleDataRequest         = 11, ///< MLE Data Request
    };

    enum Priority : uint8_t
    {
        kPriorityLow    = OT_MESSAGE_PRIORITY_LOW,      ///< Low priority level.
        kPriorityNormal = OT_MESSAGE_PRIORITY_NORMAL,   ///< Normal priority level.
        kPriorityHigh   = OT_MESSAGE_PRIORITY_HIGH,     ///< High priority level.
        kPriorityNet    = OT_MESSAGE_PRIORITY_HIGH + 1, ///< Network Control priority level.
    };

    static constexpr uint8_t kNumPriorities = 4; ///< Number of priority levels.

    /**
     * This enumeration represents the link security mode (used by `Settings` constructor).
     *
     */
    enum LinkSecurityMode : bool
    {
        kNoLinkSecurity   = false, ///< Link security disabled (no link security).
        kWithLinkSecurity = true,  ///< Link security enabled.
    };

    /**
     * This enumeration represents the message ownership model when a `Message` instance is passed to a method/function.
     *
     */
    enum Ownership : uint8_t
    {
        /**
         * This value indicates that the method/function receiving a `Message` instance should take custody of the
         * message (e.g., the method should `Free()` the message if no longer needed).
         *
         */
        kTakeCustody,

        /**
         * This value indicates that the method/function receiving a `Message` instance does not own the message (e.g.,
         * it should not `Free()` or `Enqueue()` it in a queue). The receiving method/function should create a
         * copy/clone of the message to keep (if/when needed).
         *
         */
        kCopyToUse,
    };

    /**
     * This class represents settings used for creating a new message.
     *
     */
    class Settings : public otMessageSettings
    {
    public:
        /**
         * This constructor initializes the Settings object.
         *
         * @param[in]  aSecurityMode  A link security mode.
         * @param[in]  aPriority      A message priority.
         *
         */
        Settings(LinkSecurityMode aSecurityMode, Priority aPriority);

        /**
         * This method gets the message priority.
         *
         * @returns The message priority.
         *
         */
        Priority GetPriority(void) const { return static_cast<Priority>(mPriority); }

        /**
         * This method indicates whether the link security should be enabled.
         *
         * @returns TRUE if link security should be enabled, FALSE otherwise.
         *
         */
        bool IsLinkSecurityEnabled(void) const { return mLinkSecurityEnabled; }

        /**
         * This static method converts a pointer to an `otMessageSettings` to a `Settings`.
         *
         * @param[in] aSettings  A pointer to `otMessageSettings` to covert from.
         *                       If it is `nullptr`, then the default settings `GetDefault()` will be used.
         *
         * @returns A reference to the converted `Settings` or the default if @p aSettings is `nullptr`.
         *
         */
        static const Settings &From(const otMessageSettings *aSettings);

        /**
         * This static method returns the default settings with link security enabled and `kPriorityNormal` priority.
         *
         * @returns A reference to the default settings (link security enable and `kPriorityNormal` priority).
         *
         */
        static const Settings &GetDefault(void) { return static_cast<const Settings &>(kDefault); }

    private:
        static const otMessageSettings kDefault;
    };

    /**
     * This method frees this message buffer.
     *
     */
    void Free(void);

    /**
     * This method returns a pointer to the next message.
     *
     * @returns A pointer to the next message in the list or nullptr if at the end of the list.
     *
     */
    Message *GetNext(void) const;

    /**
     * This method returns the number of bytes in the message.
     *
     * @returns The number of bytes in the message.
     *
     */
    uint16_t GetLength(void) const { return GetMetadata().mLength; }

    /**
     * This method sets the number of bytes in the message.
     *
     * @param[in]  aLength  Requested number of bytes in the message.
     *
     * @retval kErrorNone    Successfully set the length of the message.
     * @retval kErrorNoBufs  Failed to grow the size of the message because insufficient buffers were available.
     *
     */
    Error SetLength(uint16_t aLength);

    /**
     * This method returns the number of buffers in the message.
     *
     */
    uint8_t GetBufferCount(void) const;

    /**
     * This method returns the byte offset within the message.
     *
     * @returns A byte offset within the message.
     *
     */
    uint16_t GetOffset(void) const { return GetMetadata().mOffset; }

    /**
     * This method moves the byte offset within the message.
     *
     * @param[in]  aDelta  The number of bytes to move the current offset, which may be positive or negative.
     *
     */
    void MoveOffset(int aDelta);

    /**
     * This method sets the byte offset within the message.
     *
     * @param[in]  aOffset  The byte offset within the message.
     *
     */
    void SetOffset(uint16_t aOffset);

    /**
     * This method returns the type of the message.
     *
     * @returns The type of the message.
     *
     */
    Type GetType(void) const { return static_cast<Type>(GetMetadata().mType); }

    /**
     * This method sets the message type.
     *
     * @param[in]  aType  The message type.
     *
     */
    void SetType(Type aType) { GetMetadata().mType = aType; }

    /**
     * This method returns the sub type of the message.
     *
     * @returns The sub type of the message.
     *
     */
    SubType GetSubType(void) const { return static_cast<SubType>(GetMetadata().mSubType); }

    /**
     * This method sets the message sub type.
     *
     * @param[in]  aSubType  The message sub type.
     *
     */
    void SetSubType(SubType aSubType) { GetMetadata().mSubType = aSubType; }

    /**
     * This method returns whether or not the message is of MLE subtype.
     *
     * @retval TRUE   If message is of MLE subtype.
     * @retval FALSE  If message is not of MLE subtype.
     *
     */
    bool IsSubTypeMle(void) const;

    /**
     * This method checks whether this multicast message may be looped back.
     *
     * @retval TRUE   If message may be looped back.
     * @retval FALSE  If message must not be looped back.
     *
     */
    bool GetMulticastLoop(void) const { return GetMetadata().mMulticastLoop; }

    /**
     * This method sets whether multicast may be looped back.
     *
     * @param[in]  aMulticastLoop  Whether allow looping back multicast.
     *
     */
    void SetMulticastLoop(bool aMulticastLoop) { GetMetadata().mMulticastLoop = aMulticastLoop; }

    /**
     * This method returns the message priority level.
     *
     * @returns The priority level associated with this message.
     *
     */
    Priority GetPriority(void) const { return static_cast<Priority>(GetMetadata().mPriority); }

    /**
     * This method sets the messages priority.
     * If the message is already queued in a priority queue, changing the priority ensures to
     * update the message in the associated queue.
     *
     * @param[in]  aPriority  The message priority level.
     *
     * @retval kErrorNone          Successfully set the priority for the message.
     * @retval kErrorInvalidArgs   Priority level is not invalid.
     *
     */
    Error SetPriority(Priority aPriority);

    /**
     * This static method convert a `Priority` to a string.
     *
     * @param[in] aPriority  The priority level.
     *
     * @returns A string representation of @p aPriority.
     *
     */
    static const char *PriorityToString(Priority aPriority);

    /**
     * This method prepends bytes to the front of the message.
     *
     * On success, this method grows the message by @p aLength bytes.
     *
     * @param[in]  aBuf     A pointer to a data buffer (can be `nullptr` to grow message without writing bytes).
     * @param[in]  aLength  The number of bytes to prepend.
     *
     * @retval kErrorNone    Successfully prepended the bytes.
     * @retval kErrorNoBufs  Not enough reserved bytes in the message.
     *
     */
    Error PrependBytes(const void *aBuf, uint16_t aLength);

    /**
     * This method prepends an object to the front of the message.
     *
     * On success, this method grows the message by the size of the object.
     *
     * @tparam    ObjectType   The object type to prepend to the message.
     *
     * @param[in] aObject      A reference to the object to prepend to the message.
     *
     * @retval kErrorNone    Successfully prepended the object.
     * @retval kErrorNoBufs  Not enough reserved bytes in the message.
     *
     */
    template <typename ObjectType> Error Prepend(const ObjectType &aObject)
    {
        static_assert(!TypeTraits::IsPointer<ObjectType>::kValue, "ObjectType must not be a pointer");

        return PrependBytes(&aObject, sizeof(ObjectType));
    }

    /**
     * This method removes header bytes from the message.
     *
     * @param[in]  aLength  Number of header bytes to remove.
     *
     */
    void RemoveHeader(uint16_t aLength);

    /**
     * This method appends bytes to the end of the message.
     *
     * On success, this method grows the message by @p aLength bytes.
     *
     * @param[in]  aBuf     A pointer to a data buffer (MUST not be `nullptr`).
     * @param[in]  aLength  The number of bytes to append.
     *
     * @retval kErrorNone    Successfully appended the bytes.
     * @retval kErrorNoBufs  Insufficient available buffers to grow the message.
     *
     */
    Error AppendBytes(const void *aBuf, uint16_t aLength);

    /**
     * This method appends bytes read from another or potentially the same message to the end of the current message.
     *
     * On success, this method grows the message by @p aLength bytes.
     *
     * @param[in] aMessage   The message to read the bytes from (it can be the same as the current message).
     * @param[in] aOffset    The offset in @p aMessage to start reading the bytes from.
     * @param[in] aLength    The number of bytes to read from @p aMessage and append.
     *
     * @retval kErrorNone    Successfully appended the bytes.
     * @retval kErrorNoBufs  Insufficient available buffers to grow the message.
     * @retval kErrorParse   Not enough bytes in @p aMessage to read @p aLength bytes from @p aOffset.
     *
     */
    Error AppendBytesFromMessage(const Message &aMessage, uint16_t aOffset, uint16_t aLength);

    /**
     * This method appends an object to the end of the message.
     *
     * On success, this method grows the message by the size of the appended object
     *
     * @tparam    ObjectType   The object type to append to the message.
     *
     * @param[in] aObject      A reference to the object to append to the message.
     *
     * @retval kErrorNone    Successfully appended the object.
     * @retval kErrorNoBufs  Insufficient available buffers to grow the message.
     *
     */
    template <typename ObjectType> Error Append(const ObjectType &aObject)
    {
        static_assert(!TypeTraits::IsPointer<ObjectType>::kValue, "ObjectType must not be a pointer");

        return AppendBytes(&aObject, sizeof(ObjectType));
    }

    /**
     * This method reads bytes from the message.
     *
     * @param[in]  aOffset  Byte offset within the message to begin reading.
     * @param[out] aBuf     A pointer to a data buffer to copy the read bytes into.
     * @param[in]  aLength  Number of bytes to read.
     *
     * @returns The number of bytes read.
     *
     */
    uint16_t ReadBytes(uint16_t aOffset, void *aBuf, uint16_t aLength) const;

    /**
     * This method reads a given number of bytes from the message.
     *
     * If there are fewer bytes available in the message than the requested read length, the available bytes will be
     * read and copied into @p aBuf. In this case `kErrorParse` will be returned.
     *
     * @param[in]  aOffset  Byte offset within the message to begin reading.
     * @param[out] aBuf     A pointer to a data buffer to copy the read bytes into.
     * @param[in]  aLength  Number of bytes to read.
     *
     * @retval kErrorNone     @p aLength bytes were successfully read from message.
     * @retval kErrorParse    Not enough bytes remaining in message to read the entire object.
     *
     */
    Error Read(uint16_t aOffset, void *aBuf, uint16_t aLength) const;

    /**
     * This method reads an object from the message.
     *
     * If there are fewer bytes available in the message than the requested object size, the available bytes will be
     * read and copied into @p aObject (@p aObject will be read partially). In this case `kErrorParse` will
     * be returned.
     *
     * @tparam     ObjectType   The object type to read from the message.
     *
     * @param[in]  aOffset      Byte offset within the message to begin reading.
     * @param[out] aObject      A reference to the object to read into.
     *
     * @retval kErrorNone     Object @p aObject was successfully read from message.
     * @retval kErrorParse    Not enough bytes remaining in message to read the entire object.
     *
     */
    template <typename ObjectType> Error Read(uint16_t aOffset, ObjectType &aObject) const
    {
        static_assert(!TypeTraits::IsPointer<ObjectType>::kValue, "ObjectType must not be a pointer");

        return Read(aOffset, &aObject, sizeof(ObjectType));
    }

    /**
     * This method compares the bytes in the message at a given offset with a given byte array.
     *
     * If there are fewer bytes available in the message than the requested @p aLength, the comparison is treated as
     * failure (returns FALSE).
     *
     * @param[in]  aOffset    Byte offset within the message to read from for the comparison.
     * @param[in]  aBuf       A pointer to a data buffer to compare with the bytes from message.
     * @param[in]  aLength    Number of bytes in @p aBuf.
     *
     * @returns TRUE if there are enough bytes available in @p aMessage and they match the bytes from @p aBuf,
     *          FALSE otherwise.
     *
     */
    bool CompareBytes(uint16_t aOffset, const void *aBuf, uint16_t aLength) const;

    /**
     * This method compares the bytes in the message at a given offset with bytes read from another message.
     *
     * If either message has fewer bytes available than the requested @p aLength, the comparison is treated as failure
     * (returns FALSE).
     *
     * @param[in]  aOffset        Byte offset within the message to read from for the comparison.
     * @param[in]  aOtherMessage  The other message to compare with.
     * @param[in]  aOtherOffset   Byte offset within @p aOtherMessage to read from for the comparison.
     * @param[in]  aLength        Number of bytes to compare.
     *
     * @returns TRUE if there are enough bytes available in both messages and they all match. FALSE otherwise.
     *
     */
    bool CompareBytes(uint16_t aOffset, const Message &aOtherMessage, uint16_t aOtherOffset, uint16_t aLength) const;

    /**
     * This method compares the bytes in the message at a given offset with an object.
     *
     * The bytes in the message are compared with the bytes in @p aObject. If there are fewer bytes available in the
     * message than the requested object size, it is treated as failed comparison (returns FALSE).
     *
     * @tparam     ObjectType   The object type to compare with the bytes in message.
     *
     * @param[in] aOffset      Byte offset within the message to read from for the comparison.
     * @param[in] aObject      A reference to the object to compare with the message bytes.
     *
     * @returns TRUE if there are enough bytes available in @p aMessage and they match the bytes in @p aObject,
     *          FALSE otherwise.
     *
     */
    template <typename ObjectType> bool Compare(uint16_t aOffset, const ObjectType &aObject) const
    {
        static_assert(!TypeTraits::IsPointer<ObjectType>::kValue, "ObjectType must not be a pointer");

        return CompareBytes(aOffset, &aObject, sizeof(ObjectType));
    }

    /**
     * This method writes bytes to the message.
     *
     * This method will not resize the message. The given data to write (with @p aLength bytes) MUST fit within the
     * existing message buffer (from the given offset @p aOffset up to the message's length).
     *
     * @param[in]  aOffset  Byte offset within the message to begin writing.
     * @param[in]  aBuf     A pointer to a data buffer.
     * @param[in]  aLength  Number of bytes to write.
     *
     */
    void WriteBytes(uint16_t aOffset, const void *aBuf, uint16_t aLength);

    /**
     * This methods writes an object to the message.
     *
     * This method will not resize the message. The entire given object (all its bytes) MUST fit within the existing
     * message buffer (from the given offset @p aOffset up to the message's length).
     *
     * @tparam     ObjectType   The object type to write to the message.
     *
     * @param[in]  aOffset      Byte offset within the message to begin writing.
     * @param[in]  aObject      A reference to the object to write.
     *
     */
    template <typename ObjectType> void Write(uint16_t aOffset, const ObjectType &aObject)
    {
        static_assert(!TypeTraits::IsPointer<ObjectType>::kValue, "ObjectType must not be a pointer");

        WriteBytes(aOffset, &aObject, sizeof(ObjectType));
    }

    /**
     * This method copies bytes from one message to another.
     *
     * If source and destination messages are the same, `CopyTo()` can be used to perform a backward copy, but
     * it MUST not be used to forward copy within the same message (i.e., when source and destination messages are the
     * same and source offset is smaller than the destination offset).
     *
     * @param[in] aSourceOffset       Byte offset within the source message to begin reading.
     * @param[in] aDestinationOffset  Byte offset within the destination message to begin writing.
     * @param[in] aLength             Number of bytes to copy.
     * @param[in] aMessage            Message to copy to.
     *
     * @returns The number of bytes copied.
     *
     */
    uint16_t CopyTo(uint16_t aSourceOffset, uint16_t aDestinationOffset, uint16_t aLength, Message &aMessage) const;

    /**
     * This method creates a copy of the message.
     *
     * It allocates the new message from the same message pool as the original one and copies @p aLength octets
     * of the payload. The `Type`, `SubType`, `LinkSecurity`, `Offset`, `InterfaceId`, and `Priority` fields on the
     * cloned message are also copied from the original one.
     *
     * @param[in] aLength  Number of payload bytes to copy.
     *
     * @returns A pointer to the message or nullptr if insufficient message buffers are available.
     *
     */
    Message *Clone(uint16_t aLength) const;

    /**
     * This method creates a copy of the message.
     *
     * It allocates the new message from the same message pool as the original one and copies the entire payload. The
     * `Type`, `SubType`, `LinkSecurity`, `Offset`, `InterfaceId`, and `Priority` fields on the cloned message are also
     * copied from the original one.
     *
     * @returns A pointer to the message or nullptr if insufficient message buffers are available.
     *
     */
    Message *Clone(void) const { return Clone(GetLength()); }

    /**
     * This method returns the datagram tag used for 6LoWPAN fragmentation or the identification used for IPv6
     * fragmentation.
     *
     * @returns The 6LoWPAN datagram tag or the IPv6 fragment identification.
     *
     */
    uint32_t GetDatagramTag(void) const { return GetMetadata().mDatagramTag; }

    /**
     * This method sets the datagram tag used for 6LoWPAN fragmentation.
     *
     * @param[in]  aTag  The 6LoWPAN datagram tag.
     *
     */
    void SetDatagramTag(uint32_t aTag) { GetMetadata().mDatagramTag = aTag; }

    /**
     * This method returns whether or not the message forwarding is scheduled for the child.
     *
     * @param[in]  aChildIndex  The index into the child table.
     *
     * @retval TRUE   If the message is scheduled to be forwarded to the child.
     * @retval FALSE  If the message is not scheduled to be forwarded to the child.
     *
     */
    bool GetChildMask(uint16_t aChildIndex) const;

    /**
     * This method unschedules forwarding of the message to the child.
     *
     * @param[in]  aChildIndex  The index into the child table.
     *
     */
    void ClearChildMask(uint16_t aChildIndex);

    /**
     * This method schedules forwarding of the message to the child.
     *
     * @param[in]  aChildIndex  The index into the child table.
     *
     */
    void SetChildMask(uint16_t aChildIndex);

    /**
     * This method returns whether or not the message forwarding is scheduled for at least one child.
     *
     * @retval TRUE   If message forwarding is scheduled for at least one child.
     * @retval FALSE  If message forwarding is not scheduled for any child.
     *
     */
    bool IsChildPending(void) const;

    /**
     * This method returns the RLOC16 of the mesh destination.
     *
     * @note Only use this for non-link-local unicast messages.
     *
     * @returns The IEEE 802.15.4 Destination PAN ID.
     *
     */
    uint16_t GetMeshDest(void) const { return GetMetadata().mMeshDest; }

    /**
     * This method sets the RLOC16 of the mesh destination.
     *
     * @note Only use this when sending non-link-local unicast messages.
     *
     * @param[in]  aMeshDest  The IEEE 802.15.4 Destination PAN ID.
     *
     */
    void SetMeshDest(uint16_t aMeshDest) { GetMetadata().mMeshDest = aMeshDest; }

    /**
     * This method returns the IEEE 802.15.4 Destination PAN ID.
     *
     * @note Only use this when sending MLE Discover Request or Response messages.
     *
     * @returns The IEEE 802.15.4 Destination PAN ID.
     *
     */
    uint16_t GetPanId(void) const { return GetMetadata().mPanId; }

    /**
     * This method sets the IEEE 802.15.4 Destination PAN ID.
     *
     * @note Only use this when sending MLE Discover Request or Response messages.
     *
     * @param[in]  aPanId  The IEEE 802.15.4 Destination PAN ID.
     *
     */
    void SetPanId(uint16_t aPanId) { GetMetadata().mPanId = aPanId; }

    /**
     * This method returns the IEEE 802.15.4 Channel to use for transmission.
     *
     * @note Only use this when sending MLE Announce messages.
     *
     * @returns The IEEE 802.15.4 Channel to use for transmission.
     *
     */
    uint8_t GetChannel(void) const { return GetMetadata().mChannel; }

    /**
     * This method sets the IEEE 802.15.4 Channel to use for transmission.
     *
     * @note Only use this when sending MLE Announce messages.
     *
     * @param[in]  aChannel  The IEEE 802.15.4 Channel to use for transmission.
     *
     */
    void SetChannel(uint8_t aChannel) { GetMetadata().mChannel = aChannel; }

    /**
     * This method returns the timeout used for 6LoWPAN reassembly.
     *
     * @returns The time remaining in seconds.
     *
     */
    uint8_t GetTimeout(void) const { return GetMetadata().mTimeout; }

    /**
     * This method sets the timeout used for 6LoWPAN reassembly.
     *
     * @param[in]  aTimeout  The timeout value.
     *
     */
    void SetTimeout(uint8_t aTimeout) { GetMetadata().mTimeout = aTimeout; }

    /**
     * This method decrements the timeout.
     *
     */
    void DecrementTimeout(void) { GetMetadata().mTimeout--; }

    /**
     * This method returns whether or not message forwarding is scheduled for direct transmission.
     *
     * @retval TRUE   If message forwarding is scheduled for direct transmission.
     * @retval FALSE  If message forwarding is not scheduled for direct transmission.
     *
     */
    bool GetDirectTransmission(void) const { return GetMetadata().mDirectTx; }

    /**
     * This method unschedules forwarding using direct transmission.
     *
     */
    void ClearDirectTransmission(void) { GetMetadata().mDirectTx = false; }

    /**
     * This method schedules forwarding using direct transmission.
     *
     */
    void SetDirectTransmission(void) { GetMetadata().mDirectTx = true; }

    /**
     * This method indicates whether the direct transmission of message was successful.
     *
     * @retval TRUE   If direct transmission of message was successful (all fragments were delivered and acked).
     * @retval FALSE  If direct transmission of message failed (at least one fragment failed).
     *
     */
    bool GetTxSuccess(void) const { return GetMetadata().mTxSuccess; }

    /**
     * This method sets whether the direct transmission of message was successful.
     *
     * @param[in] aTxSuccess   TRUE if the direct transmission is successful, FALSE otherwise (i.e., at least one
     *                         fragment transmission failed).
     *
     */
    void SetTxSuccess(bool aTxSuccess) { GetMetadata().mTxSuccess = aTxSuccess; }

    /**
     * This method indicates whether the message may be evicted.
     *
     * @retval TRUE   If the message must not be evicted.
     * @retval FALSE  If the message may be evicted.
     *
     */
    bool GetDoNotEvict(void) const { return GetMetadata().mDoNotEvict; }

    /**
     * This method sets whether the message may be evicted.
     *
     * @param[in]  aDoNotEvict  TRUE if the message may not be evicted, FALSE otherwise.
     *
     */
    void SetDoNotEvict(bool aDoNotEvict) { GetMetadata().mDoNotEvict = aDoNotEvict; }

    /**
     * This method indicates whether or not link security is enabled for the message.
     *
     * @retval TRUE   If link security is enabled.
     * @retval FALSE  If link security is not enabled.
     *
     */
    bool IsLinkSecurityEnabled(void) const { return GetMetadata().mLinkSecurity; }

    /**
     * This method sets whether or not link security is enabled for the message.
     *
     * @param[in]  aEnabled  TRUE if link security is enabled, FALSE otherwise.
     *
     */
    void SetLinkSecurityEnabled(bool aEnabled) { GetMetadata().mLinkSecurity = aEnabled; }

    /**
     * This method updates the average RSS (Received Signal Strength) associated with the message by adding the given
     * RSS value to the average. Note that a message can be composed of multiple 802.15.4 data frame fragments each
     * received with a different signal strength.
     *
     * @param[in] aRss A new RSS value (in dBm) to be added to average.
     *
     */
    void AddRss(int8_t aRss) { IgnoreError(GetMetadata().mRssAverager.Add(aRss)); }

    /**
     * This method returns the average RSS (Received Signal Strength) associated with the message.
     *
     * @returns The current average RSS value (in dBm) or OT_RADIO_RSSI_INVALID if no average is available.
     *
     */
    int8_t GetAverageRss(void) const { return GetMetadata().mRssAverager.GetAverage(); }

    /**
     * This method returns a const reference to RssAverager of the message.
     *
     * @returns A const reference to the RssAverager of the message.
     *
     */
    const RssAverager &GetRssAverager(void) const { return GetMetadata().mRssAverager; }

#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
    /**
     * This method updates the average LQI (Link Quality Indicator) associated with the message.
     *
     * The given LQI value would be added to the average. Note that a message can be composed of multiple 802.15.4
     * frame fragments each received with a different signal strength.
     *
     * @param[in] aLQI A new LQI value (has no unit) to be added to average.
     *
     */
    void AddLqi(uint8_t aLqi) { GetMetadata().mLqiAverager.Add(aLqi); }

    /**
     * This method returns the average LQI (Link Quality Indicator) associated with the message.
     *
     * @returns The current average LQI value (in dBm) or OT_RADIO_LQI_NONE if no average is available.
     *
     */
    uint8_t GetAverageLqi(void) const { return GetMetadata().mLqiAverager.GetAverage(); }

    /**
     * This method returns the count of frames counted so far.
     *
     * @returns The count of frames that have been counted.
     *
     */
    uint8_t GetPsduCount(void) const { return GetMetadata().mLqiAverager.GetCount(); }
#endif

    /**
     * This method sets the message's link info properties (PAN ID, link security, RSS) from a given `ThreadLinkInfo`.
     *
     * @param[in] aLinkInfo   The `ThreadLinkInfo` instance from which to set message's related properties.
     *
     */
    void SetLinkInfo(const ThreadLinkInfo &aLinkInfo);

    /**
     * This method returns a pointer to the message queue (if any) where this message is queued.
     *
     * @returns A pointer to the message queue or nullptr if not in any message queue.
     *
     */
    MessageQueue *GetMessageQueue(void) const
    {
        return !GetMetadata().mInPriorityQ ? static_cast<MessageQueue *>(GetMetadata().mQueue) : nullptr;
    }

    /**
     * This method returns a pointer to the priority message queue (if any) where this message is queued.
     *
     * @returns A pointer to the priority queue or nullptr if not in any priority queue.
     *
     */
    PriorityQueue *GetPriorityQueue(void) const
    {
        return GetMetadata().mInPriorityQ ? static_cast<PriorityQueue *>(GetMetadata().mQueue) : nullptr;
    }

    /**
     * This method indicates whether or not the message is also used for time sync purpose.
     *
     * When OPENTHREAD_CONFIG_TIME_SYNC_ENABLE is 0, this method always return false.
     *
     * @retval TRUE   If the message is also used for time sync purpose.
     * @retval FALSE  If the message is not used for time sync purpose.
     *
     */
    bool IsTimeSync(void) const;

#if OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
    /**
     * This method sets whether or not the message is also used for time sync purpose.
     *
     * @param[in]  aEnabled  TRUE if the message is also used for time sync purpose, FALSE otherwise.
     *
     */
    void SetTimeSync(bool aEnabled) { GetMetadata().mTimeSync = aEnabled; }

    /**
     * This method sets the offset to network time.
     *
     * @param[in]  aNetworkTimeOffset  The offset to network time.
     *
     */
    void SetNetworkTimeOffset(int64_t aNetworkTimeOffset) { GetMetadata().mNetworkTimeOffset = aNetworkTimeOffset; }

    /**
     * This method gets the offset to network time.
     *
     * @returns  The offset to network time.
     *
     */
    int64_t GetNetworkTimeOffset(void) const { return GetMetadata().mNetworkTimeOffset; }

    /**
     * This method sets the time sync sequence.
     *
     * @param[in]  aTimeSyncSeq  The time sync sequence.
     *
     */
    void SetTimeSyncSeq(uint8_t aTimeSyncSeq) { GetMetadata().mTimeSyncSeq = aTimeSyncSeq; }

    /**
     * This method gets the time sync sequence.
     *
     * @returns  The time sync sequence.
     *
     */
    uint8_t GetTimeSyncSeq(void) const { return GetMetadata().mTimeSyncSeq; }
#endif // OPENTHREAD_CONFIG_TIME_SYNC_ENABLE

#if OPENTHREAD_CONFIG_MULTI_RADIO
    /**
     * This method indicates whether the radio type is set.
     *
     * @retval TRUE   If the radio type is set.
     * @retval FALSE  If the radio type is not set.
     *
     */
    bool IsRadioTypeSet(void) const { return GetMetadata().mIsRadioTypeSet; }

    /**
     * This method gets the radio link type the message was received on, or should be sent on.
     *
     * This method should be used only when `IsRadioTypeSet()` returns `true`.
     *
     * @returns The radio link type of the message.
     *
     */
    Mac::RadioType GetRadioType(void) const { return static_cast<Mac::RadioType>(GetMetadata().mRadioType); }

    /**
     * This method sets the radio link type the message was received on, or should be sent on.
     *
     * @param[in] aRadioType   A radio link type of the message.
     *
     */
    void SetRadioType(Mac::RadioType aRadioType)
    {
        GetMetadata().mIsRadioTypeSet = true;
        GetMetadata().mRadioType      = aRadioType;
    }

    /**
     * This method clears any previously set radio type on the message.
     *
     * After calling this method, `IsRadioTypeSet()` returns false until radio type is set (`SetRadioType()`).
     *
     */
    void ClearRadioType(void) { GetMetadata().mIsRadioTypeSet = false; }

#endif // #if OPENTHREAD_CONFIG_MULTI_RADIO

protected:
    uint16_t GetReserved(void) const { return GetMetadata().mReserved; }
    void     SetReserved(uint16_t aReservedHeader) { GetMetadata().mReserved = aReservedHeader; }

private:
    class Chunk : public Data<kWithUint16Length>
    {
    public:
        const Buffer *GetBuffer(void) const { return mBuffer; }
        void          SetBuffer(const Buffer *aBuffer) { mBuffer = aBuffer; }

    private:
        const Buffer *mBuffer; // Buffer containing the chunk
    };

    class MutableChunk : public Chunk
    {
    public:
        uint8_t *GetBytes(void) { return AsNonConst(Chunk::GetBytes()); }
    };

    void GetFirstChunk(uint16_t aOffset, uint16_t &aLength, Chunk &aChunk) const;
    void GetNextChunk(uint16_t &aLength, Chunk &aChunk) const;

    void GetFirstChunk(uint16_t aOffset, uint16_t &aLength, MutableChunk &aChunk)
    {
        AsConst(this)->GetFirstChunk(aOffset, aLength, static_cast<Chunk &>(aChunk));
    }

    void GetNextChunk(uint16_t &aLength, MutableChunk &aChunk)
    {
        AsConst(this)->GetNextChunk(aLength, static_cast<Chunk &>(aChunk));
    }

    MessagePool *GetMessagePool(void) const { return GetMetadata().mMessagePool; }
    void         SetMessagePool(MessagePool *aMessagePool) { GetMetadata().mMessagePool = aMessagePool; }

    bool IsInAQueue(void) const { return (GetMetadata().mQueue != nullptr); }
    void SetMessageQueue(MessageQueue *aMessageQueue);
    void SetPriorityQueue(PriorityQueue *aPriorityQueue);

    Message *&      Next(void) { return GetMetadata().mNext; }
    Message *const &Next(void) const { return GetMetadata().mNext; }
    Message *&      Prev(void) { return GetMetadata().mPrev; }

    Error ResizeMessage(uint16_t aLength);
};

/**
 * This class implements a message queue.
 *
 */
class MessageQueue : public otMessageQueue
{
    friend class Message;
    friend class PriorityQueue;

public:
    /**
     * This enumeration represents a position (head or tail) in the queue. This is used to specify where a new message
     * should be added in the queue.
     *
     */
    enum QueuePosition : uint8_t
    {
        kQueuePositionHead, ///< Indicates the head (front) of the list.
        kQueuePositionTail, ///< Indicates the tail (end) of the list.
    };

    /**
     * This constructor initializes the message queue.
     *
     */
    MessageQueue(void);

    /**
     * This method returns a pointer to the first message.
     *
     * @returns A pointer to the first message.
     *
     */
    Message *GetHead(void) const;

    /**
     * This method adds a message to the end of the list.
     *
     * @param[in]  aMessage  The message to add.
     *
     */
    void Enqueue(Message &aMessage) { Enqueue(aMessage, kQueuePositionTail); }

    /**
     * This method adds a message at a given position (head/tail) of the list.
     *
     * @param[in]  aMessage  The message to add.
     * @param[in]  aPosition The position (head or tail) where to add the message.
     *
     */
    void Enqueue(Message &aMessage, QueuePosition aPosition);

    /**
     * This method removes a message from the list.
     *
     * @param[in]  aMessage  The message to remove.
     *
     */
    void Dequeue(Message &aMessage);

    /**
     * This method removes a message from the queue and frees it.
     *
     * @param[in]  aMessage  The message to remove and free.
     *
     */
    void DequeueAndFree(Message &aMessage);

    /**
     * This method removes and frees all messages from the queue.
     *
     */
    void DequeueAndFreeAll(void);

    /**
     * This method returns the number of messages and buffers enqueued.
     *
     * @param[out]  aMessageCount  Returns the number of messages enqueued.
     * @param[out]  aBufferCount   Returns the number of buffers enqueued.
     *
     */
    void GetInfo(uint16_t &aMessageCount, uint16_t &aBufferCount) const;

private:
    Message *GetTail(void) const { return static_cast<Message *>(mData); }
    void     SetTail(Message *aMessage) { mData = aMessage; }
};

/**
 * This class implements a priority queue.
 *
 */
class PriorityQueue
{
    friend class Message;
    friend class MessageQueue;
    friend class MessagePool;

public:
    /**
     * This constructor initializes the priority queue.
     *
     */
    PriorityQueue(void);

    /**
     * This method returns a pointer to the first message.
     *
     * @returns A pointer to the first message.
     *
     */
    Message *GetHead(void) const;

    /**
     * This method returns a pointer to the first message for a given priority level.
     *
     * @param[in] aPriority   Priority level.
     *
     * @returns A pointer to the first message with given priority level or nullptr if there is no messages with
     *          this priority level.
     *
     */
    Message *GetHeadForPriority(Message::Priority aPriority) const;

    /**
     * This method adds a message to the queue.
     *
     * @param[in]  aMessage  The message to add.
     *
     */
    void Enqueue(Message &aMessage);

    /**
     * This method removes a message from the list.
     *
     * @param[in]  aMessage  The message to remove.
     *
     */
    void Dequeue(Message &aMessage);

    /**
     * This method removes a message from the queue and frees it.
     *
     * @param[in]  aMessage  The message to remove and free.
     *
     */
    void DequeueAndFree(Message &aMessage);

    /**
     * This method removes and frees all messages from the queue.
     *
     */
    void DequeueAndFreeAll(void);

    /**
     * This method returns the number of messages and buffers enqueued.
     *
     * @param[out]  aMessageCount  Returns the number of messages enqueued.
     * @param[out]  aBufferCount   Returns the number of buffers enqueued.
     *
     */
    void GetInfo(uint16_t &aMessageCount, uint16_t &aBufferCount) const;

    /**
     * This method returns the tail of the list (last message in the list)
     *
     * @returns A pointer to the tail of the list.
     *
     */
    Message *GetTail(void) const;

private:
    uint8_t PrevPriority(uint8_t aPriority) const
    {
        return (aPriority == Message::kNumPriorities - 1) ? 0 : (aPriority + 1);
    }

    Message *FindFirstNonNullTail(Message::Priority aStartPriorityLevel) const;

    Message *mTails[Message::kNumPriorities]; // Tail pointers associated with different priority levels.
};

/**
 * This class represents a message pool
 *
 */
class MessagePool : public InstanceLocator, private NonCopyable
{
    friend class Message;
    friend class MessageQueue;
    friend class PriorityQueue;

public:
    /**
     * This constructor initializes the object.
     *
     */
    explicit MessagePool(Instance &aInstance);

    /**
     * This method is used to obtain a new message.
     *
     * The link security is enabled by default on the newly obtained message.
     *
     * @param[in]  aType           The message type.
     * @param[in]  aReserveHeader  The number of header bytes to reserve.
     * @param[in]  aPriority       The priority level of the message.
     *
     * @returns A pointer to the message or nullptr if no message buffers are available.
     *
     */
    Message *New(Message::Type aType, uint16_t aReserveHeader, Message::Priority aPriority);

    /**
     * This method is used to obtain a new message with specified settings.
     *
     * @param[in]  aType           The message type.
     * @param[in]  aReserveHeader  The number of header bytes to reserve.
     * @param[in]  aSettings       The message settings.
     *
     * @returns A pointer to the message or nullptr if no message buffers are available.
     *
     */
    Message *New(Message::Type            aType,
                 uint16_t                 aReserveHeader,
                 const Message::Settings &aSettings = Message::Settings::GetDefault());

    /**
     * This method is used to free a message and return all message buffers to the buffer pool.
     *
     * @param[in]  aMessage  The message to free.
     *
     */
    void Free(Message *aMessage);

    /**
     * This method returns the number of free buffers.
     *
     * @returns The number of free buffers.
     *
     */
    uint16_t GetFreeBufferCount(void) const;

    /**
     * This method returns the total number of buffers.
     *
     * @returns The total number of buffers.
     *
     */
    uint16_t GetTotalBufferCount(void) const;

private:
    Buffer *NewBuffer(Message::Priority aPriority);
    void    FreeBuffers(Buffer *aBuffer);
    Error   ReclaimBuffers(Message::Priority aPriority);

#if !OPENTHREAD_CONFIG_PLATFORM_MESSAGE_MANAGEMENT && !OPENTHREAD_CONFIG_MESSAGE_USE_HEAP_ENABLE
    uint16_t                  mNumFreeBuffers;
    Pool<Buffer, kNumBuffers> mBufferPool;
#endif
};

/**
 * @}
 *
 */

} // namespace ot

#endif // MESSAGE_HPP_
