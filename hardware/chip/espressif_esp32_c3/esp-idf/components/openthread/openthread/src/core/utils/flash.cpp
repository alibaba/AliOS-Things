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

#include "flash.hpp"

#if OPENTHREAD_CONFIG_PLATFORM_FLASH_API_ENABLE

#include <stdio.h>

#include <openthread/platform/flash.h>

#include "common/code_utils.hpp"
#include "common/instance.hpp"

namespace ot {

const uint32_t ot::Flash::sSwapActive;
const uint32_t ot::Flash::sSwapInactive;

void Flash::Init(void)
{
    RecordHeader record;

    otPlatFlashInit(&GetInstance());

    mSwapSize = otPlatFlashGetSwapSize(&GetInstance());

    for (mSwapIndex = 0;; mSwapIndex++)
    {
        uint32_t swapMarker;

        if (mSwapIndex >= 2)
        {
            Wipe();
            ExitNow();
        }

        otPlatFlashRead(&GetInstance(), mSwapIndex, 0, &swapMarker, sizeof(swapMarker));

        if (swapMarker == sSwapActive)
        {
            break;
        }
    }

    for (mSwapUsed = kSwapMarkerSize; mSwapUsed <= mSwapSize - sizeof(record); mSwapUsed += record.GetSize())
    {
        otPlatFlashRead(&GetInstance(), mSwapIndex, mSwapUsed, &record, sizeof(record));
        if (!record.IsAddBeginSet())
        {
            break;
        }

        if (!record.IsAddCompleteSet())
        {
            break;
        }
    }

    SanitizeFreeSpace();

exit:
    return;
}

void Flash::SanitizeFreeSpace(void)
{
    uint32_t temp;
    bool     sanitizeNeeded = false;

    if (mSwapUsed & 3)
    {
        ExitNow(sanitizeNeeded = true);
    }

    for (uint32_t offset = mSwapUsed; offset < mSwapSize; offset += sizeof(temp))
    {
        otPlatFlashRead(&GetInstance(), mSwapIndex, offset, &temp, sizeof(temp));
        if (temp != ~0U)
        {
            ExitNow(sanitizeNeeded = true);
        }
    }

exit:
    if (sanitizeNeeded)
    {
        Swap();
    }
}

Error Flash::Get(uint16_t aKey, int aIndex, uint8_t *aValue, uint16_t *aValueLength) const
{
    Error        error       = kErrorNotFound;
    uint16_t     valueLength = 0;
    int          index       = 0; // This must be initalized to 0. See [Note] in Delete().
    uint32_t     offset;
    RecordHeader record;

    for (offset = kSwapMarkerSize; offset < mSwapUsed; offset += record.GetSize())
    {
        otPlatFlashRead(&GetInstance(), mSwapIndex, offset, &record, sizeof(record));

        if ((record.GetKey() != aKey) || !record.IsValid())
        {
            continue;
        }

        if (record.IsFirst())
        {
            index = 0;
        }

        if (index == aIndex)
        {
            if (aValue && aValueLength)
            {
                uint16_t readLength = *aValueLength;

                if (readLength > record.GetLength())
                {
                    readLength = record.GetLength();
                }

                otPlatFlashRead(&GetInstance(), mSwapIndex, offset + sizeof(record), aValue, readLength);
            }

            valueLength = record.GetLength();
            error       = kErrorNone;
        }

        index++;
    }

    if (aValueLength)
    {
        *aValueLength = valueLength;
    }

    return error;
}

Error Flash::Set(uint16_t aKey, const uint8_t *aValue, uint16_t aValueLength)
{
    return Add(aKey, true, aValue, aValueLength);
}

Error Flash::Add(uint16_t aKey, const uint8_t *aValue, uint16_t aValueLength)
{
    bool first = (Get(aKey, 0, nullptr, nullptr) == kErrorNotFound);

    return Add(aKey, first, aValue, aValueLength);
}

Error Flash::Add(uint16_t aKey, bool aFirst, const uint8_t *aValue, uint16_t aValueLength)
{
    Error  error = kErrorNone;
    Record record;

    record.Init(aKey, aFirst);
    record.SetData(aValue, aValueLength);

    OT_ASSERT((mSwapSize - record.GetSize()) >= kSwapMarkerSize);

    if ((mSwapSize - record.GetSize()) < mSwapUsed)
    {
        Swap();
        VerifyOrExit((mSwapSize - record.GetSize()) >= mSwapUsed, error = kErrorNoBufs);
    }

    otPlatFlashWrite(&GetInstance(), mSwapIndex, mSwapUsed, &record, record.GetSize());

    record.SetAddCompleteFlag();
    otPlatFlashWrite(&GetInstance(), mSwapIndex, mSwapUsed, &record, sizeof(RecordHeader));

    mSwapUsed += record.GetSize();

exit:
    return error;
}

bool Flash::DoesValidRecordExist(uint32_t aOffset, uint16_t aKey) const
{
    RecordHeader record;
    bool         rval = false;

    for (; aOffset < mSwapUsed; aOffset += record.GetSize())
    {
        otPlatFlashRead(&GetInstance(), mSwapIndex, aOffset, &record, sizeof(record));

        if (record.IsValid() && record.IsFirst() && (record.GetKey() == aKey))
        {
            ExitNow(rval = true);
        }
    }

exit:
    return rval;
}

void Flash::Swap(void)
{
    uint8_t  dstIndex  = !mSwapIndex;
    uint32_t dstOffset = kSwapMarkerSize;
    Record   record;

    otPlatFlashErase(&GetInstance(), dstIndex);

    for (uint32_t srcOffset = kSwapMarkerSize; srcOffset < mSwapUsed; srcOffset += record.GetSize())
    {
        otPlatFlashRead(&GetInstance(), mSwapIndex, srcOffset, &record, sizeof(RecordHeader));

        VerifyOrExit(record.IsAddBeginSet());

        if (!record.IsValid() || DoesValidRecordExist(srcOffset + record.GetSize(), record.GetKey()))
        {
            continue;
        }

        otPlatFlashRead(&GetInstance(), mSwapIndex, srcOffset, &record, record.GetSize());
        otPlatFlashWrite(&GetInstance(), dstIndex, dstOffset, &record, record.GetSize());
        dstOffset += record.GetSize();
    }

exit:
    otPlatFlashWrite(&GetInstance(), dstIndex, 0, &sSwapActive, sizeof(sSwapActive));
    otPlatFlashWrite(&GetInstance(), mSwapIndex, 0, &sSwapInactive, sizeof(sSwapInactive));

    mSwapIndex = dstIndex;
    mSwapUsed  = dstOffset;
}

Error Flash::Delete(uint16_t aKey, int aIndex)
{
    Error        error = kErrorNotFound;
    int          index = 0; // This must be initalized to 0. See [Note] below.
    RecordHeader record;

    for (uint32_t offset = kSwapMarkerSize; offset < mSwapUsed; offset += record.GetSize())
    {
        otPlatFlashRead(&GetInstance(), mSwapIndex, offset, &record, sizeof(record));

        if ((record.GetKey() != aKey) || !record.IsValid())
        {
            continue;
        }

        if (record.IsFirst())
        {
            index = 0;
        }

        if ((aIndex == index) || (aIndex == -1))
        {
            record.SetDeleted();
            otPlatFlashWrite(&GetInstance(), mSwapIndex, offset, &record, sizeof(record));
            error = kErrorNone;
        }

        /* [Note] If the operation gets interrupted here and aIndex is 0, the next record (index == 1) will never get
         * marked as first. However, this is not actually an issue because all the methods that iterate over the
         * settings area initialize the index to 0, without expecting any record to be effectively marked as first. */

        if ((index == 1) && (aIndex == 0))
        {
            record.SetFirst();
            otPlatFlashWrite(&GetInstance(), mSwapIndex, offset, &record, sizeof(record));
        }

        index++;
    }

    return error;
}

void Flash::Wipe(void)
{
    otPlatFlashErase(&GetInstance(), 0);
    otPlatFlashWrite(&GetInstance(), 0, 0, &sSwapActive, sizeof(sSwapActive));

    mSwapIndex = 0;
    mSwapUsed  = sizeof(sSwapActive);
}

} // namespace ot

#endif // OPENTHREAD_CONFIG_PLATFORM_FLASH_API_ENABLE
