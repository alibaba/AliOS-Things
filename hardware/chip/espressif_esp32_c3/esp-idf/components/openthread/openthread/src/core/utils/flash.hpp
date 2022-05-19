/*
 *  Copyright (c) 2020, The OpenThread Authors.
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

#ifndef FLASH_HPP_
#define FLASH_HPP_

#include "openthread-core-config.h"

#if OPENTHREAD_CONFIG_PLATFORM_FLASH_API_ENABLE

#include <stdint.h>
#include <string.h>

#include <openthread/platform/toolchain.h>

#include "common/debug.hpp"
#include "common/error.hpp"
#include "common/locator.hpp"

namespace ot {

/**
 * This class implements the flash storage driver.
 *
 */
class Flash : public InstanceLocator
{
public:
    /**
     * Constructor.
     *
     */
    explicit Flash(Instance &aInstance)
        : InstanceLocator(aInstance)
    {
    }

    /**
     * This method initializes the flash storage driver.
     *
     */
    void Init(void);

    /**
     * This method fetches the value identified by @p aKey.
     *
     * @param[in]     aKey          The key associated with the requested value.
     * @param[in]     aIndex        The index of the specific item to get.
     * @param[out]    aValue        A pointer to where the value of the setting should be written.
     *                              May be nullptr if just testing for the presence or length of a key.
     * @param[inout]  aValueLength  A pointer to the length of the value.
     *                              When called, this should point to an integer containing the maximum bytes that
     *                              can be written to @p aValue.
     *                              At return, the actual length of the setting is written.
     *                              May be nullptr if performing a presence check.
     *
     * @retval kErrorNone       The value was fetched successfully.
     * @retval kErrorNotFound   The key was not found.
     *
     */
    Error Get(uint16_t aKey, int aIndex, uint8_t *aValue, uint16_t *aValueLength) const;

    /**
     * This method sets or replaces the value identified by @p aKey.
     *
     * If there was more than one value previously associated with @p aKey, then they are all deleted and replaced with
     * this single entry.
     *
     * @param[in]  aKey          The key associated with the value.
     * @param[in]  aValue        A pointer to where the new value of the setting should be read from.
     *                           MUST NOT be nullptr if @p aValueLength is non-zero.
     * @param[in]  aValueLength  The length of the data pointed to by @p aValue. May be zero.
     *
     * @retval kErrorNone     The value was changed.
     * @retval kErrorNoBufs   Not enough space to store the value.
     *
     */
    Error Set(uint16_t aKey, const uint8_t *aValue, uint16_t aValueLength);

    /**
     * This method adds a value to @p aKey.
     *
     * @param[in]  aKey          The key associated with the value.
     * @param[in]  aValue        A pointer to where the new value of the setting should be read from.
     *                           MUST NOT be nullptr if @p aValueLength is non-zero.
     * @param[in]  aValueLength  The length of the data pointed to by @p aValue. May be zero.
     *
     * @retval kErrorNone    The value was added.
     * @retval kErrorNoBufs  Not enough space to store the value.
     *
     */
    Error Add(uint16_t aKey, const uint8_t *aValue, uint16_t aValueLength);

    /**
     * This method removes a value from @p aKey.
     *
     *
     * @param[in] aKey    The key associated with the value.
     * @param[in] aIndex  The index of the value to be removed.
     *                    If set to -1, all values for @p aKey will be removed.
     *
     * @retval kErrorNone      The given key and index was found and removed successfully.
     * @retval kErrorNotFound  The given key or index was not found.
     *
     */
    Error Delete(uint16_t aKey, int aIndex);

    /**
     * This method removes all values.
     *
     */
    void Wipe(void);

private:
    static constexpr uint32_t kSwapMarkerSize = 4; // in bytes

    static const uint32_t sSwapActive   = 0xbe5cc5ee;
    static const uint32_t sSwapInactive = 0xbe5cc5ec;

    OT_TOOL_PACKED_BEGIN
    class RecordHeader
    {
    public:
        void Init(uint16_t aKey, bool aFirst)
        {
            mKey   = aKey;
            mFlags = kFlagsInit & ~kFlagAddBegin;

            if (aFirst)
            {
                mFlags &= ~kFlagFirst;
            }

            mLength   = 0;
            mReserved = 0xffff;
        }

        uint16_t GetKey(void) const { return mKey; }
        void     SetKey(uint16_t aKey) { mKey = aKey; }

        uint16_t GetLength(void) const { return mLength; }
        void     SetLength(uint16_t aLength) { mLength = aLength; }

        uint16_t GetSize(void) const { return sizeof(*this) + ((mLength + 3) & 0xfffc); }

        bool IsValid(void) const { return ((mFlags & (kFlagAddComplete | kFlagDelete)) == kFlagDelete); }

        bool IsAddBeginSet(void) const { return (mFlags & kFlagAddBegin) == 0; }
        void SetAddBeginFlag(void) { mFlags &= ~kFlagAddBegin; }

        bool IsAddCompleteSet(void) const { return (mFlags & kFlagAddComplete) == 0; }
        void SetAddCompleteFlag(void) { mFlags &= ~kFlagAddComplete; }

        bool IsDeleted(void) const { return (mFlags & kFlagDelete) == 0; }
        void SetDeleted(void) { mFlags &= ~kFlagDelete; }

        bool IsFirst(void) const { return (mFlags & kFlagFirst) == 0; }
        void SetFirst(void) { mFlags &= ~kFlagFirst; }

    private:
        static constexpr uint16_t kFlagsInit       = 0xffff; // Flags initialize to all-ones.
        static constexpr uint16_t kFlagAddBegin    = 1 << 0; // 0 indicates record write has started, 1 otherwise.
        static constexpr uint16_t kFlagAddComplete = 1 << 1; // 0 indicates record write has completed, 1 otherwise.
        static constexpr uint16_t kFlagDelete      = 1 << 2; // 0 indicates record was deleted, 1 otherwise.
        static constexpr uint16_t kFlagFirst       = 1 << 3; // 0 indicates first record for key, 1 otherwise.

        uint16_t mKey;
        uint16_t mFlags;
        uint16_t mLength;
        uint16_t mReserved;
    } OT_TOOL_PACKED_END;

    OT_TOOL_PACKED_BEGIN
    class Record : public RecordHeader
    {
    public:
        const uint8_t *GetData(void) const { return mData; }
        void           SetData(const uint8_t *aData, uint16_t aDataLength)
        {
            OT_ASSERT(aDataLength <= kMaxDataSize);
            memcpy(mData, aData, aDataLength);
            SetLength(aDataLength);
        }

    private:
        static constexpr uint16_t kMaxDataSize = 255;

        uint8_t mData[kMaxDataSize];
    } OT_TOOL_PACKED_END;

    Error Add(uint16_t aKey, bool aFirst, const uint8_t *aValue, uint16_t aValueLength);
    bool  DoesValidRecordExist(uint32_t aOffset, uint16_t aKey) const;
    void  SanitizeFreeSpace(void);
    void  Swap(void);

    uint32_t mSwapSize;
    uint32_t mSwapUsed;
    uint8_t  mSwapIndex;
};

} // namespace ot

#endif // OPENTHREAD_CONFIG_PLATFORM_FLASH_API_ENABLE

#endif // FLASH_HPP_
