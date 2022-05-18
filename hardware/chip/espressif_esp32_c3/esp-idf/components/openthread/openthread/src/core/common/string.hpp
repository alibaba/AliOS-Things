/*
 *  Copyright (c) 2018, The OpenThread Authors.
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
 *  This file defines OpenThread String class.
 */

#ifndef STRING_HPP_
#define STRING_HPP_

#include "openthread-core-config.h"

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

#include "common/code_utils.hpp"
#include "common/error.hpp"

namespace ot {

/**
 * @addtogroup core-string
 *
 * @brief
 *   This module includes definitions for OpenThread String class.
 *
 * @{
 *
 */

/**
 * This function returns the number of characters that precede the terminating nullptr character.
 *
 * @param[in] aString      A pointer to the string.
 * @param[in] aMaxLength   The maximum length in bytes.
 *
 * @returns The number of characters that precede the terminating nullptr character or @p aMaxLength, whichever is
 * smaller.
 *
 */
uint16_t StringLength(const char *aString, uint16_t aMaxLength);

/**
 * This function finds the first occurrence of a given character in a null-terminated string.
 *
 * @param[in] aString     A pointer to the string.
 * @param[in] aChar       A char to search for in the string.
 *
 * @returns The pointer to first occurrence of the @p aChar in @p aString, or nullptr if cannot be found.
 *
 */
const char *StringFind(const char *aString, char aChar);

/**
 * This function finds the first occurrence of a given sub-string in a null-terminated string.
 *
 * @param[in] aString     A pointer to the string.
 * @param[in] aSubString  A sub-string to search for.
 *
 * @returns The pointer to first occurrence of the @p aSubString in @p aString, or nullptr if cannot be found.
 *
 */
const char *StringFind(const char *aString, const char *aSubString);

/**
 * This function checks whether a null-terminated string ends with a given character.
 *
 * @param[in] aString  A pointer to the string.
 * @param[in] aChar    A char to check.
 *
 * @retval TRUE   If @p aString ends with character @p aChar.
 * @retval FALSE  If @p aString does not end with character @p aChar.
 *
 */
bool StringEndsWith(const char *aString, char aChar);

/**
 * This function checks whether a null-terminated string ends with a given sub-string.
 *
 * @param[in] aString      A pointer to the string.
 * @param[in] aSubString   A sun-string to check against.
 *
 * @retval TRUE   If @p aString ends with sub-string @p aSubString.
 * @retval FALSE  If @p aString does not end with sub-string @p aSubString.
 *
 */
bool StringEndsWith(const char *aString, const char *aSubString);

/**
 * This class implements writing to a string buffer.
 *
 */
class StringWriter
{
public:
    /**
     * This constructor initializes the object as cleared on the provided buffer.
     *
     * @param[in] aBuffer  A pointer to the char buffer to write into.
     * @param[in] aSize    The size of @p aBuffer.
     *
     */
    StringWriter(char *aBuffer, uint16_t aSize);

    /**
     * This method clears the string writer.
     *
     * @returns The string writer.
     *
     */
    StringWriter &Clear(void);

    /**
     * This method returns whether the output is truncated.
     *
     * @note If the output is truncated, the buffer is still null-terminated.
     *
     * @retval  true    The output is truncated.
     * @retval  false   The output is not truncated.
     *
     */
    bool IsTruncated(void) const { return mLength >= mSize; }

    /**
     * This method gets the length of the wanted string.
     *
     * Similar to `strlen()` the length does not include the null character at the end of the string.
     *
     * @returns The string length.
     *
     */
    uint16_t GetLength(void) const { return mLength; }

    /**
     * This method returns the size (number of chars) in the buffer.
     *
     * @returns The size of the buffer.
     *
     */
    uint16_t GetSize(void) const { return mSize; }

    /**
     * This method appends `printf()` style formatted data to the buffer.
     *
     * @param[in] aFormat    A pointer to the format string.
     * @param[in] ...        Arguments for the format specification.
     *
     * @returns The string writer.
     *
     */
    StringWriter &Append(const char *aFormat, ...);

    /**
     * This method appends `printf()` style formatted data to the buffer.
     *
     * @param[in] aFormat    A pointer to the format string.
     * @param[in] aArgs      Arguments for the format specification (as `va_list`).
     *
     * @returns The string writer.
     *
     */
    StringWriter &AppendVarArgs(const char *aFormat, va_list aArgs);

    /**
     * This method appends an array of bytes in hex representation (using "%02x" style) to the buffer.
     *
     * @param[in] aBytes    A pointer to buffer containing the bytes to append.
     * @param[in] aLength   The length of @p aBytes buffer (in bytes).
     *
     * @returns The string writer.
     *
     */
    StringWriter &AppendHexBytes(const uint8_t *aBytes, uint16_t aLength);

private:
    char *         mBuffer;
    uint16_t       mLength;
    const uint16_t mSize;
};

/**
 * This class defines a fixed-size string.
 *
 */
template <uint16_t kSize> class String : public StringWriter
{
    static_assert(kSize > 0, "String buffer cannot be empty.");

public:
    /**
     * This constructor initializes the string as empty.
     *
     */
    String(void)
        : StringWriter(mBuffer, sizeof(mBuffer))
    {
    }

    /**
     * This method returns the string as a null-terminated C string.
     *
     * @returns The null-terminated C string.
     *
     */
    const char *AsCString(void) const { return mBuffer; }

private:
    char mBuffer[kSize];
};

/**
 * This function validates whether a given byte sequence (string) follows UTF-8 encoding.
 * Control characters are not allowed.
 *
 * @param[in]  aString  A null-terminated byte sequence.
 *
 * @retval TRUE   The sequence is a valid UTF-8 string.
 * @retval FALSE  The sequence is not a valid UTF-8 string.
 *
 */
bool IsValidUtf8String(const char *aString);

/**
 * This function validates whether a given byte sequence (string) follows UTF-8 encoding.
 * Control characters are not allowed.
 *
 * @param[in]  aString  A byte sequence.
 * @param[in]  aLength  Length of the sequence.
 *
 * @retval TRUE   The sequence is a valid UTF-8 string.
 * @retval FALSE  The sequence is not a valid UTF-8 string.
 *
 */
bool IsValidUtf8String(const char *aString, size_t aLength);

/**
 * @}
 *
 */

} // namespace ot

#endif // STRING_HPP_
