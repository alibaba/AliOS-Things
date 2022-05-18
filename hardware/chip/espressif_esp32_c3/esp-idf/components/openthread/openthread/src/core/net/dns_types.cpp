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

/**
 * @file
 *   This file implements generating and processing of DNS headers and helper functions/methods.
 */

#include "dns_types.hpp"

#include "common/code_utils.hpp"
#include "common/debug.hpp"
#include "common/instance.hpp"
#include "common/logging.hpp"
#include "common/random.hpp"
#include "common/string.hpp"

namespace ot {
namespace Dns {

using ot::Encoding::BigEndian::HostSwap16;

Error Header::SetRandomMessageId(void)
{
    return Random::Crypto::FillBuffer(reinterpret_cast<uint8_t *>(&mMessageId), sizeof(mMessageId));
}

Error Header::ResponseCodeToError(Response aResponse)
{
    Error error = kErrorFailed;

    switch (aResponse)
    {
    case kResponseSuccess:
        error = kErrorNone;
        break;

    case kResponseFormatError:   // Server unable to interpret request due to format error.
    case kResponseBadName:       // Bad name.
    case kResponseBadTruncation: // Bad truncation.
    case kResponseNotZone:       // A name is not in the zone.
        error = kErrorParse;
        break;

    case kResponseServerFailure: // Server encountered an internal failure.
        error = kErrorFailed;
        break;

    case kResponseNameError:       // Name that ought to exist, does not exists.
    case kResponseRecordNotExists: // Some RRset that ought to exist, does not exist.
        error = kErrorNotFound;
        break;

    case kResponseNotImplemented: // Server does not support the query type (OpCode).
        error = kErrorNotImplemented;
        break;

    case kResponseBadAlg: // Bad algorithm.
        error = kErrorNotCapable;
        break;

    case kResponseNameExists:   // Some name that ought not to exist, does exist.
    case kResponseRecordExists: // Some RRset that ought not to exist, does exist.
        error = kErrorDuplicated;
        break;

    case kResponseRefused: // Server refused to perform operation for policy or security reasons.
    case kResponseNotAuth: // Service is not authoritative for zone.
        error = kErrorSecurity;
        break;

    default:
        break;
    }

    return error;
}

Error Name::AppendTo(Message &aMessage) const
{
    Error error;

    if (IsEmpty())
    {
        error = AppendTerminator(aMessage);
    }
    else if (IsFromCString())
    {
        error = AppendName(GetAsCString(), aMessage);
    }
    else
    {
        // Name is from a message. Read labels one by one from
        // `mMessage` and and append each to the `aMessage`.

        LabelIterator iterator(*mMessage, mOffset);

        while (true)
        {
            error = iterator.GetNextLabel();

            switch (error)
            {
            case kErrorNone:
                SuccessOrExit(error = iterator.AppendLabel(aMessage));
                break;

            case kErrorNotFound:
                // We reached the end of name successfully.
                error = AppendTerminator(aMessage);

                OT_FALL_THROUGH;

            default:
                ExitNow();
            }
        }
    }

exit:
    return error;
}

Error Name::AppendLabel(const char *aLabel, Message &aMessage)
{
    return AppendLabel(aLabel, static_cast<uint8_t>(StringLength(aLabel, kMaxLabelSize)), aMessage);
}

Error Name::AppendLabel(const char *aLabel, uint8_t aLength, Message &aMessage)
{
    Error error = kErrorNone;

    VerifyOrExit((0 < aLength) && (aLength <= kMaxLabelLength), error = kErrorInvalidArgs);

    SuccessOrExit(error = aMessage.Append(aLength));
    error = aMessage.AppendBytes(aLabel, aLength);

exit:
    return error;
}

Error Name::AppendMultipleLabels(const char *aLabels, Message &aMessage)
{
    return AppendMultipleLabels(aLabels, kMaxNameLength, aMessage);
}

Error Name::AppendMultipleLabels(const char *aLabels, uint8_t aLength, Message &aMessage)
{
    Error    error           = kErrorNone;
    uint16_t index           = 0;
    uint16_t labelStartIndex = 0;
    char     ch;

    VerifyOrExit(aLabels != nullptr);

    do
    {
        ch = index < aLength ? aLabels[index] : static_cast<char>(kNullChar);

        if ((ch == kNullChar) || (ch == kLabelSeperatorChar))
        {
            uint8_t labelLength = static_cast<uint8_t>(index - labelStartIndex);

            if (labelLength == 0)
            {
                // Empty label (e.g., consecutive dots) is invalid, but we
                // allow for two cases: (1) where `aLabels` ends with a dot
                // (`labelLength` is zero but we are at end of `aLabels` string
                // and `ch` is null char. (2) if `aLabels` is just "." (we
                // see a dot at index 0, and index 1 is null char).

                error =
                    ((ch == kNullChar) || ((index == 0) && (aLabels[1] == kNullChar))) ? kErrorNone : kErrorInvalidArgs;
                ExitNow();
            }

            VerifyOrExit(index + 1 < kMaxEncodedLength, error = kErrorInvalidArgs);
            SuccessOrExit(error = AppendLabel(&aLabels[labelStartIndex], labelLength, aMessage));

            labelStartIndex = index + 1;
        }

        index++;

    } while (ch != kNullChar);

exit:
    return error;
}

Error Name::AppendTerminator(Message &aMessage)
{
    uint8_t terminator = 0;

    return aMessage.Append(terminator);
}

Error Name::AppendPointerLabel(uint16_t aOffset, Message &aMessage)
{
    Error    error;
    uint16_t value;

#if OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
    if (!Instance::IsDnsNameCompressionEnabled())
    {
        // If "DNS name compression" mode is disabled, instead of
        // appending the pointer label, read the name from the message
        // and append it uncompressed. Note that the `aOffset` parameter
        // in this method is given relative to the start of DNS header
        // in `aMessage` (which `aMessage.GetOffset()` specifies).

        error = Name(aMessage, aOffset + aMessage.GetOffset()).AppendTo(aMessage);
        ExitNow();
    }
#endif

    // A pointer label takes the form of a two byte sequence as a
    // `uint16_t` value. The first two bits are ones. This allows a
    // pointer to be distinguished from a text label, since the text
    // label must begin with two zero bits (note that labels are
    // restricted to 63 octets or less). The next 14-bits specify
    // an offset value relative to start of DNS header.

    OT_ASSERT(aOffset < kPointerLabelTypeUint16);

    value = HostSwap16(aOffset | kPointerLabelTypeUint16);

    ExitNow(error = aMessage.Append(value));

exit:
    return error;
}

Error Name::AppendName(const char *aName, Message &aMessage)
{
    Error error;

    SuccessOrExit(error = AppendMultipleLabels(aName, aMessage));
    error = AppendTerminator(aMessage);

exit:
    return error;
}

Error Name::ParseName(const Message &aMessage, uint16_t &aOffset)
{
    Error         error;
    LabelIterator iterator(aMessage, aOffset);

    while (true)
    {
        error = iterator.GetNextLabel();

        switch (error)
        {
        case kErrorNone:
            break;

        case kErrorNotFound:
            // We reached the end of name successfully.
            aOffset = iterator.mNameEndOffset;
            error   = kErrorNone;

            OT_FALL_THROUGH;

        default:
            ExitNow();
        }
    }

exit:
    return error;
}

Error Name::ReadLabel(const Message &aMessage, uint16_t &aOffset, char *aLabelBuffer, uint8_t &aLabelLength)
{
    Error         error;
    LabelIterator iterator(aMessage, aOffset);

    SuccessOrExit(error = iterator.GetNextLabel());
    SuccessOrExit(error = iterator.ReadLabel(aLabelBuffer, aLabelLength, /* aAllowDotCharInLabel */ true));
    aOffset = iterator.mNextLabelOffset;

exit:
    return error;
}

Error Name::ReadName(const Message &aMessage, uint16_t &aOffset, char *aNameBuffer, uint16_t aNameBufferSize)
{
    Error         error;
    LabelIterator iterator(aMessage, aOffset);
    bool          firstLabel = true;
    uint8_t       labelLength;

    while (true)
    {
        error = iterator.GetNextLabel();

        switch (error)
        {
        case kErrorNone:

            if (!firstLabel)
            {
                *aNameBuffer++ = kLabelSeperatorChar;
                aNameBufferSize--;

                // No need to check if we have reached end of the name buffer
                // here since `iterator.ReadLabel()` would verify it.
            }

            labelLength = static_cast<uint8_t>(OT_MIN(static_cast<uint8_t>(kMaxLabelSize), aNameBufferSize));
            SuccessOrExit(error = iterator.ReadLabel(aNameBuffer, labelLength, /* aAllowDotCharInLabel */ false));
            aNameBuffer += labelLength;
            aNameBufferSize -= labelLength;
            firstLabel = false;
            break;

        case kErrorNotFound:
            // We reach the end of name successfully. Always add a terminating dot
            // at the end.
            *aNameBuffer++ = kLabelSeperatorChar;
            aNameBufferSize--;
            VerifyOrExit(aNameBufferSize >= sizeof(uint8_t), error = kErrorNoBufs);
            *aNameBuffer = kNullChar;
            aOffset      = iterator.mNameEndOffset;
            error        = kErrorNone;

            OT_FALL_THROUGH;

        default:
            ExitNow();
        }
    }

exit:
    return error;
}

Error Name::CompareLabel(const Message &aMessage, uint16_t &aOffset, const char *aLabel)
{
    Error         error;
    LabelIterator iterator(aMessage, aOffset);

    SuccessOrExit(error = iterator.GetNextLabel());
    VerifyOrExit(iterator.CompareLabel(aLabel, /* aIsSingleLabel */ true), error = kErrorNotFound);
    aOffset = iterator.mNextLabelOffset;

exit:
    return error;
}

Error Name::CompareName(const Message &aMessage, uint16_t &aOffset, const char *aName)
{
    Error         error;
    LabelIterator iterator(aMessage, aOffset);
    bool          matches = true;

    if (*aName == kLabelSeperatorChar)
    {
        aName++;
        VerifyOrExit(*aName == kNullChar, error = kErrorInvalidArgs);
    }

    while (true)
    {
        error = iterator.GetNextLabel();

        switch (error)
        {
        case kErrorNone:
            if (matches && !iterator.CompareLabel(aName, /* aIsSingleLabel */ false))
            {
                matches = false;
            }

            break;

        case kErrorNotFound:
            // We reached the end of the name in `aMessage`. We check if
            // all the previous labels matched so far, and we are also
            // at the end of `aName` string (see null char), then we
            // return `kErrorNone` indicating a successful comparison
            // (full match). Otherwise we return `kErrorNotFound` to
            // indicate failed comparison.

            if (matches && (*aName == kNullChar))
            {
                error = kErrorNone;
            }

            aOffset = iterator.mNameEndOffset;

            OT_FALL_THROUGH;

        default:
            ExitNow();
        }
    }

exit:
    return error;
}

Error Name::CompareName(const Message &aMessage, uint16_t &aOffset, const Message &aMessage2, uint16_t aOffset2)
{
    Error         error;
    LabelIterator iterator(aMessage, aOffset);
    LabelIterator iterator2(aMessage2, aOffset2);
    bool          matches = true;

    while (true)
    {
        error = iterator.GetNextLabel();

        switch (error)
        {
        case kErrorNone:
            // If all the previous labels matched so far, then verify
            // that we can get the next label on `iterator2` and that it
            // matches the label from `iterator`.
            if (matches && (iterator2.GetNextLabel() != kErrorNone || !iterator.CompareLabel(iterator2)))
            {
                matches = false;
            }

            break;

        case kErrorNotFound:
            // We reached the end of the name in `aMessage`. We check
            // that `iterator2` is also at its end, and if all previous
            // labels matched we return `kErrorNone`.

            if (matches && (iterator2.GetNextLabel() == kErrorNotFound))
            {
                error = kErrorNone;
            }

            aOffset = iterator.mNameEndOffset;

            OT_FALL_THROUGH;

        default:
            ExitNow();
        }
    }

exit:
    return error;
}

Error Name::CompareName(const Message &aMessage, uint16_t &aOffset, const Name &aName)
{
    return aName.IsFromCString()
               ? CompareName(aMessage, aOffset, aName.mString)
               : (aName.IsFromMessage() ? CompareName(aMessage, aOffset, *aName.mMessage, aName.mOffset)
                                        : ParseName(aMessage, aOffset));
}

Error Name::LabelIterator::GetNextLabel(void)
{
    Error error;

    while (true)
    {
        uint8_t labelLength;
        uint8_t labelType;

        SuccessOrExit(error = mMessage.Read(mNextLabelOffset, labelLength));

        labelType = labelLength & kLabelTypeMask;

        if (labelType == kTextLabelType)
        {
            if (labelLength == 0)
            {
                // Zero label length indicates end of a name.

                if (!IsEndOffsetSet())
                {
                    mNameEndOffset = mNextLabelOffset + sizeof(uint8_t);
                }

                ExitNow(error = kErrorNotFound);
            }

            mLabelStartOffset = mNextLabelOffset + sizeof(uint8_t);
            mLabelLength      = labelLength;
            mNextLabelOffset  = mLabelStartOffset + labelLength;
            ExitNow();
        }
        else if (labelType == kPointerLabelType)
        {
            // A pointer label takes the form of a two byte sequence as a
            // `uint16_t` value. The first two bits are ones. The next 14 bits
            // specify an offset value from the start of the DNS header.

            uint16_t pointerValue;

            SuccessOrExit(error = mMessage.Read(mNextLabelOffset, pointerValue));

            if (!IsEndOffsetSet())
            {
                mNameEndOffset = mNextLabelOffset + sizeof(uint16_t);
            }

            // `mMessage.GetOffset()` must point to the start of the
            // DNS header.
            mNextLabelOffset = mMessage.GetOffset() + (HostSwap16(pointerValue) & kPointerLabelOffsetMask);

            // Go back through the `while(true)` loop to get the next label.
        }
        else
        {
            ExitNow(error = kErrorParse);
        }
    }

exit:
    return error;
}

Error Name::LabelIterator::ReadLabel(char *aLabelBuffer, uint8_t &aLabelLength, bool aAllowDotCharInLabel) const
{
    Error error;

    VerifyOrExit(mLabelLength < aLabelLength, error = kErrorNoBufs);

    SuccessOrExit(error = mMessage.Read(mLabelStartOffset, aLabelBuffer, mLabelLength));
    aLabelBuffer[mLabelLength] = kNullChar;
    aLabelLength               = mLabelLength;

    if (!aAllowDotCharInLabel)
    {
        VerifyOrExit(StringFind(aLabelBuffer, kLabelSeperatorChar) == nullptr, error = kErrorParse);
    }

exit:
    return error;
}

bool Name::LabelIterator::CompareLabel(const char *&aName, bool aIsSingleLabel) const
{
    // This method compares the current label in the iterator with the
    // `aName` string. `aIsSingleLabel` indicates whether `aName` is a
    // single label, or a sequence of labels separated by dot '.' char.
    // If the label matches `aName`, then `aName` pointer is moved
    // forward to the start of the next label (skipping over the `.`
    // char). This method returns `true` when the labels match, `false`
    // otherwise.

    bool matches = false;

    VerifyOrExit(StringLength(aName, mLabelLength) == mLabelLength);
    matches = mMessage.CompareBytes(mLabelStartOffset, aName, mLabelLength);

    VerifyOrExit(matches);

    aName += mLabelLength;

    // If `aName` is a single label, we should be also at the end of the
    // `aName` string. Otherwise, we should see either null or dot '.'
    // character (in case `aName` contains multiple labels).

    matches = (*aName == kNullChar);

    if (!aIsSingleLabel && (*aName == kLabelSeperatorChar))
    {
        matches = true;
        aName++;
    }

exit:
    return matches;
}

bool Name::LabelIterator::CompareLabel(const LabelIterator &aOtherIterator) const
{
    // This method compares the current label in the iterator with the
    // label from another iterator.

    return (mLabelLength == aOtherIterator.mLabelLength) &&
           mMessage.CompareBytes(mLabelStartOffset, aOtherIterator.mMessage, aOtherIterator.mLabelStartOffset,
                                 mLabelLength);
}

Error Name::LabelIterator::AppendLabel(Message &aMessage) const
{
    // This method reads and appends the current label in the iterator
    // to `aMessage`.

    Error error;

    VerifyOrExit((0 < mLabelLength) && (mLabelLength <= kMaxLabelLength), error = kErrorInvalidArgs);
    SuccessOrExit(error = aMessage.Append(mLabelLength));
    error = aMessage.AppendBytesFromMessage(mMessage, mLabelStartOffset, mLabelLength);

exit:
    return error;
}

bool Name::IsSubDomainOf(const char *aName, const char *aDomain)
{
    bool     match        = false;
    uint16_t nameLength   = StringLength(aName, kMaxNameLength);
    uint16_t domainLength = StringLength(aDomain, kMaxNameLength);

    if (nameLength > 0 && aName[nameLength - 1] == kLabelSeperatorChar)
    {
        --nameLength;
    }

    if (domainLength > 0 && aDomain[domainLength - 1] == kLabelSeperatorChar)
    {
        --domainLength;
    }

    VerifyOrExit(nameLength >= domainLength);
    aName += nameLength - domainLength;

    if (nameLength > domainLength)
    {
        VerifyOrExit(aName[-1] == kLabelSeperatorChar);
    }
    VerifyOrExit(memcmp(aName, aDomain, domainLength) == 0);

    match = true;

exit:
    return match;
}

Error ResourceRecord::ParseRecords(const Message &aMessage, uint16_t &aOffset, uint16_t aNumRecords)
{
    Error error = kErrorNone;

    while (aNumRecords > 0)
    {
        ResourceRecord record;

        SuccessOrExit(error = Name::ParseName(aMessage, aOffset));
        SuccessOrExit(error = record.ReadFrom(aMessage, aOffset));
        aOffset += static_cast<uint16_t>(record.GetSize());
        aNumRecords--;
    }

exit:
    return error;
}

Error ResourceRecord::FindRecord(const Message &aMessage, uint16_t &aOffset, uint16_t &aNumRecords, const Name &aName)
{
    Error error;

    while (aNumRecords > 0)
    {
        bool           matches = true;
        ResourceRecord record;

        error = Name::CompareName(aMessage, aOffset, aName);

        switch (error)
        {
        case kErrorNone:
            break;
        case kErrorNotFound:
            matches = false;
            break;
        default:
            ExitNow();
        }

        SuccessOrExit(error = record.ReadFrom(aMessage, aOffset));
        aNumRecords--;
        VerifyOrExit(!matches);
        aOffset += static_cast<uint16_t>(record.GetSize());
    }

    error = kErrorNotFound;

exit:
    return error;
}

Error ResourceRecord::FindRecord(const Message & aMessage,
                                 uint16_t &      aOffset,
                                 uint16_t        aNumRecords,
                                 uint16_t        aIndex,
                                 const Name &    aName,
                                 uint16_t        aType,
                                 ResourceRecord &aRecord,
                                 uint16_t        aMinRecordSize)
{
    // This static method searches in `aMessage` starting from `aOffset`
    // up to maximum of `aNumRecords`, for the `(aIndex+1)`th
    // occurrence of a resource record of type `aType` with record name
    // matching `aName`. It also verifies that the record size is larger
    // than `aMinRecordSize`. If found, `aMinRecordSize` bytes from the
    // record are read and copied into `aRecord`. In this case `aOffset`
    // is updated to point to the last record byte read from the message
    // (so that the caller can read any remaining fields in the record
    // data).

    Error    error;
    uint16_t offset = aOffset;
    uint16_t recordOffset;

    while (aNumRecords > 0)
    {
        SuccessOrExit(error = FindRecord(aMessage, offset, aNumRecords, aName));

        // Save the offset to start of `ResourceRecord` fields.
        recordOffset = offset;

        error = ReadRecord(aMessage, offset, aType, aRecord, aMinRecordSize);

        if (error == kErrorNotFound)
        {
            // `ReadRecord()` already updates the `offset` to skip
            // over a non-matching record.
            continue;
        }

        SuccessOrExit(error);

        if (aIndex == 0)
        {
            aOffset = offset;
            ExitNow();
        }

        aIndex--;

        // Skip over the record.
        offset = static_cast<uint16_t>(recordOffset + aRecord.GetSize());
    }

    error = kErrorNotFound;

exit:
    return error;
}

Error ResourceRecord::ReadRecord(const Message & aMessage,
                                 uint16_t &      aOffset,
                                 uint16_t        aType,
                                 ResourceRecord &aRecord,
                                 uint16_t        aMinRecordSize)
{
    // This static method tries to read a matching resource record of a
    // given type and a minimum record size from a message. The `aType`
    // value of `kTypeAny` matches any type.  If the record in the
    // message does not match, it skips over the record. Please see
    // `ReadRecord<RecordType>()` for more details.

    Error          error;
    ResourceRecord record;

    SuccessOrExit(error = record.ReadFrom(aMessage, aOffset));

    if (((aType == kTypeAny) || (record.GetType() == aType)) && (record.GetSize() >= aMinRecordSize))
    {
        IgnoreError(aMessage.Read(aOffset, &aRecord, aMinRecordSize));
        aOffset += aMinRecordSize;
    }
    else
    {
        // Skip over the entire record.
        aOffset += static_cast<uint16_t>(record.GetSize());
        error = kErrorNotFound;
    }

exit:
    return error;
}

Error ResourceRecord::ReadName(const Message &aMessage,
                               uint16_t &     aOffset,
                               uint16_t       aStartOffset,
                               char *         aNameBuffer,
                               uint16_t       aNameBufferSize,
                               bool           aSkipRecord) const
{
    // This protected method parses and reads a name field in a record
    // from a message. It is intended only for sub-classes of
    // `ResourceRecord`.
    //
    // On input `aOffset` gives the offset in `aMessage` to the start of
    // name field. `aStartOffset` gives the offset to the start of the
    // `ResourceRecord`. `aSkipRecord` indicates whether to skip over
    // the entire resource record or just the read name. On exit, when
    // successfully read, `aOffset` is updated to either point after the
    // end of record or after the the name field.
    //
    // When read successfully, this method returns `kErrorNone`. On a
    // parse error (invalid format) returns `kErrorParse`. If the
    // name does not fit in the given name buffer it returns
    // `kErrorNoBufs`

    Error error = kErrorNone;

    SuccessOrExit(error = Name::ReadName(aMessage, aOffset, aNameBuffer, aNameBufferSize));
    VerifyOrExit(aOffset <= aStartOffset + GetSize(), error = kErrorParse);

    VerifyOrExit(aSkipRecord);
    aOffset = aStartOffset;
    error   = SkipRecord(aMessage, aOffset);

exit:
    return error;
}

Error ResourceRecord::SkipRecord(const Message &aMessage, uint16_t &aOffset) const
{
    // This protected method parses and skips over a resource record
    // in a message.
    //
    // On input `aOffset` gives the offset in `aMessage` to the start of
    // the `ResourceRecord`. On exit, when successfully parsed, `aOffset`
    // is updated to point to byte after the entire record.

    Error error;

    SuccessOrExit(error = CheckRecord(aMessage, aOffset));
    aOffset += static_cast<uint16_t>(GetSize());

exit:
    return error;
}

Error ResourceRecord::CheckRecord(const Message &aMessage, uint16_t aOffset) const
{
    // This method checks that the entire record (including record data)
    // is present in `aMessage` at `aOffset` (pointing to the start of
    // the `ResourceRecord` in `aMessage`).

    return (aOffset + GetSize() <= aMessage.GetLength()) ? kErrorNone : kErrorParse;
}

Error ResourceRecord::ReadFrom(const Message &aMessage, uint16_t aOffset)
{
    // This method reads the `ResourceRecord` from `aMessage` at
    // `aOffset`. It verifies that the entire record (including record
    // data) is present in the message.

    Error error;

    SuccessOrExit(error = aMessage.Read(aOffset, *this));
    error = CheckRecord(aMessage, aOffset);

exit:
    return error;
}

void TxtEntry::Iterator::Init(const uint8_t *aTxtData, uint16_t aTxtDataLength)
{
    SetTxtData(aTxtData);
    SetTxtDataLength(aTxtDataLength);
    SetTxtDataPosition(0);
}

Error TxtEntry::Iterator::GetNextEntry(TxtEntry &aEntry)
{
    Error       error = kErrorNone;
    uint8_t     length;
    uint8_t     index;
    const char *cur;
    char *      keyBuffer = GetKeyBuffer();

    static_assert(sizeof(mChar) == TxtEntry::kMaxKeyLength + 1, "KeyBuffer cannot fit the max key length");

    VerifyOrExit(GetTxtData() != nullptr, error = kErrorParse);

    aEntry.mKey = keyBuffer;

    while ((cur = GetTxtData() + GetTxtDataPosition()) < GetTxtDataEnd())
    {
        length = static_cast<uint8_t>(*cur);

        cur++;
        VerifyOrExit(cur + length <= GetTxtDataEnd(), error = kErrorParse);
        IncreaseTxtDataPosition(sizeof(uint8_t) + length);

        // Silently skip over an empty string or if the string starts with
        // a `=` character (i.e., missing key) - RFC 6763 - section 6.4.

        if ((length == 0) || (cur[0] == kKeyValueSeparator))
        {
            continue;
        }

        for (index = 0; index < length; index++)
        {
            if (cur[index] == kKeyValueSeparator)
            {
                keyBuffer[index++]  = kNullChar; // Increment index to skip over `=`.
                aEntry.mValue       = reinterpret_cast<const uint8_t *>(&cur[index]);
                aEntry.mValueLength = length - index;
                ExitNow();
            }

            if (index >= kMaxKeyLength)
            {
                // The key is larger than recommended max key length.
                // In this case, we return the full encoded string in
                // `mValue` and `mValueLength` and set `mKey` to
                // `nullptr`.

                aEntry.mKey         = nullptr;
                aEntry.mValue       = reinterpret_cast<const uint8_t *>(cur);
                aEntry.mValueLength = length;
                ExitNow();
            }

            keyBuffer[index] = cur[index];
        }

        // If we reach the end of the string without finding `=` then
        // it is a boolean key attribute (encoded as "key").

        keyBuffer[index]    = kNullChar;
        aEntry.mValue       = nullptr;
        aEntry.mValueLength = 0;
        ExitNow();
    }

    error = kErrorNotFound;

exit:
    return error;
}

Error TxtEntry::AppendTo(Message &aMessage) const
{
    Error    error = kErrorNone;
    uint16_t keyLength;
    char     separator = kKeyValueSeparator;

    if (mKey == nullptr)
    {
        VerifyOrExit((mValue != nullptr) && (mValueLength != 0));
        error = aMessage.AppendBytes(mValue, mValueLength);
        ExitNow();
    }

    keyLength = StringLength(mKey, static_cast<uint16_t>(kMaxKeyValueEncodedSize) + 1);

    VerifyOrExit(kMinKeyLength <= keyLength, error = kErrorInvalidArgs);

    if (mValue == nullptr)
    {
        // Treat as a boolean attribute and encoded as "key" (with no `=`).
        SuccessOrExit(error = aMessage.Append<uint8_t>(static_cast<uint8_t>(keyLength)));
        error = aMessage.AppendBytes(mKey, keyLength);
        ExitNow();
    }

    // Treat as key/value and encode as "key=value", value may be empty.

    VerifyOrExit(mValueLength + keyLength + sizeof(char) <= kMaxKeyValueEncodedSize, error = kErrorInvalidArgs);

    SuccessOrExit(error = aMessage.Append<uint8_t>(static_cast<uint8_t>(keyLength + mValueLength + sizeof(char))));
    SuccessOrExit(error = aMessage.AppendBytes(mKey, keyLength));
    SuccessOrExit(error = aMessage.Append(separator));
    error = aMessage.AppendBytes(mValue, mValueLength);

exit:
    return error;
}

Error TxtEntry::AppendEntries(const TxtEntry *aEntries, uint8_t aNumEntries, Message &aMessage)
{
    Error    error       = kErrorNone;
    uint16_t startOffset = aMessage.GetLength();

    for (uint8_t index = 0; index < aNumEntries; index++)
    {
        SuccessOrExit(error = aEntries[index].AppendTo(aMessage));
    }

    if (aMessage.GetLength() == startOffset)
    {
        error = aMessage.Append<uint8_t>(0);
    }

exit:
    return error;
}

bool AaaaRecord::IsValid(void) const
{
    return GetType() == Dns::ResourceRecord::kTypeAaaa && GetSize() == sizeof(*this);
}

bool KeyRecord::IsValid(void) const
{
    return GetType() == Dns::ResourceRecord::kTypeKey;
}

#if OPENTHREAD_CONFIG_SRP_SERVER_ENABLE
void Ecdsa256KeyRecord::Init(void)
{
    KeyRecord::Init();
    SetAlgorithm(kAlgorithmEcdsaP256Sha256);
}

bool Ecdsa256KeyRecord::IsValid(void) const
{
    return KeyRecord::IsValid() && GetLength() == sizeof(*this) - sizeof(ResourceRecord) &&
           GetAlgorithm() == kAlgorithmEcdsaP256Sha256;
}
#endif

bool SigRecord::IsValid(void) const
{
    return GetType() == Dns::ResourceRecord::kTypeSig && GetLength() >= sizeof(*this) - sizeof(ResourceRecord);
}

bool LeaseOption::IsValid(void) const
{
    return GetLeaseInterval() <= GetKeyLeaseInterval();
}

Error PtrRecord::ReadPtrName(const Message &aMessage,
                             uint16_t &     aOffset,
                             char *         aLabelBuffer,
                             uint8_t        aLabelBufferSize,
                             char *         aNameBuffer,
                             uint16_t       aNameBufferSize) const
{
    Error    error       = kErrorNone;
    uint16_t startOffset = aOffset - sizeof(PtrRecord); // start of `PtrRecord`.

    // Verify that the name is within the record data length.
    SuccessOrExit(error = Name::ParseName(aMessage, aOffset));
    VerifyOrExit(aOffset <= startOffset + GetSize(), error = kErrorParse);

    aOffset = startOffset + sizeof(PtrRecord);
    SuccessOrExit(error = Name::ReadLabel(aMessage, aOffset, aLabelBuffer, aLabelBufferSize));

    if (aNameBuffer != nullptr)
    {
        SuccessOrExit(error = Name::ReadName(aMessage, aOffset, aNameBuffer, aNameBufferSize));
    }

    aOffset = startOffset;
    error   = SkipRecord(aMessage, aOffset);

exit:
    return error;
}

Error TxtRecord::ReadTxtData(const Message &aMessage,
                             uint16_t &     aOffset,
                             uint8_t *      aTxtBuffer,
                             uint16_t &     aTxtBufferSize) const
{
    Error error = kErrorNone;

    VerifyOrExit(GetLength() <= aTxtBufferSize, error = kErrorNoBufs);
    SuccessOrExit(error = aMessage.Read(aOffset, aTxtBuffer, GetLength()));
    VerifyOrExit(VerifyTxtData(aTxtBuffer, GetLength(), /* aAllowEmpty */ true), error = kErrorParse);
    aTxtBufferSize = GetLength();
    aOffset += GetLength();

exit:
    return error;
}

bool TxtRecord::VerifyTxtData(const uint8_t *aTxtData, uint16_t aTxtLength, bool aAllowEmpty)
{
    bool    valid          = false;
    uint8_t curEntryLength = 0;

    // Per RFC 1035, TXT-DATA MUST have one or more <character-string>s.
    VerifyOrExit(aAllowEmpty || aTxtLength > 0);

    for (uint16_t i = 0; i < aTxtLength; ++i)
    {
        if (curEntryLength == 0)
        {
            curEntryLength = aTxtData[i];
        }
        else
        {
            --curEntryLength;
        }
    }

    valid = (curEntryLength == 0);

exit:
    return valid;
}

} // namespace Dns
} // namespace ot
