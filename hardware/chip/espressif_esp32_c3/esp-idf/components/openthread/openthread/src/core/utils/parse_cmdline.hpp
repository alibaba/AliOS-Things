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
 *   This file includes definitions for command line parser.
 */

#ifndef PARSE_CMD_LINE_HPP_
#define PARSE_CMD_LINE_HPP_

#include <stdint.h>
#include <string.h>

#include <openthread/error.h>
#include <openthread/ip6.h>

namespace ot {
namespace Utils {
namespace CmdLineParser {

/**
 * @addtogroup utils-parse-cmd-line
 *
 * @brief
 *   This module includes definitions for command line parser.
 *
 * @{
 */

/**
 * This function parses a string as a `uint8_t` value.
 *
 * The number in string is parsed as decimal or hex format (if contains `0x` or `0X` prefix).
 *
 * @param[in]  aString   The string to parse.
 * @param[out] aUint8    A reference to an `uint8_t` variable to output the parsed value.
 *
 * @retval kErrorNone         The string was parsed successfully.
 * @retval kErrorInvalidArgs  The string does not contain valid number (e.g., value out of range).
 *
 */
otError ParseAsUint8(const char *aString, uint8_t &aUint8);

/**
 * This function parses a string as a `uint16_t` value.
 *
 * The number in string is parsed as decimal or hex format (if contains `0x` or `0X` prefix).
 *
 * @param[in]  aString   The string to parse.
 * @param[out] aUint16   A reference to an `uint16_t` variable to output the parsed value.
 *
 * @retval kErrorNone         The string was parsed successfully.
 * @retval kErrorInvalidArgs  The string does not contain valid number (e.g., value out of range).
 *
 */
otError ParseAsUint16(const char *aString, uint16_t &aUint16);

/**
 * This function parses a string as a `uint32_t` value.
 *
 * The number in string is parsed as decimal or hex format (if contains `0x` or `0X` prefix).
 *
 * @param[in]  aString   The string to parse.
 * @param[out] aUint32   A reference to an `uint32_t` variable to output the parsed value.
 *
 * @retval kErrorNone         The string was parsed successfully.
 * @retval kErrorInvalidArgs  The string does not contain valid number (e.g., value out of range).
 *
 */
otError ParseAsUint32(const char *aString, uint32_t &aUint32);

/**
 * This function parses a string as a `uint64_t` value.
 *
 * The number in string is parsed as decimal or hex format (if contains `0x` or `0X` prefix).
 *
 * @param[in]  aString   The string to parse.
 * @param[out] aUint64   A reference to an `uint64_t` variable to output the parsed value.
 *
 * @retval kErrorNone         The string was parsed successfully.
 * @retval kErrorInvalidArgs  The string does not contain valid number (e.g., value out of range).
 *
 */
otError ParseAsUint64(const char *aString, uint64_t &aUint64);

/**
 * This function parses a string as a `int8_t` value.
 *
 * The number in string is parsed as decimal or hex format (if contains `0x` or `0X` prefix). The string can start with
 * `+`/`-` sign.
 *
 * @param[in]  aString   The string to parse.
 * @param[out] aInt8     A reference to an `int8_t` variable to output the parsed value.
 *
 * @retval kErrorNone         The string was parsed successfully.
 * @retval kErrorInvalidArgs  The string does not contain valid number (e.g., value out of range).
 *
 */
otError ParseAsInt8(const char *aString, int8_t &aInt8);

/**
 * This function parses a string as a `int16_t` value.
 *
 * The number in string is parsed as decimal or hex format (if contains `0x` or `0X` prefix). The string can start with
 * `+`/`-` sign.
 *
 * @param[in]  aString   The string to parse.
 * @param[out] aInt16    A reference to an `int16_t` variable to output the parsed value.
 *
 * @retval kErrorNone         The string was parsed successfully.
 * @retval kErrorInvalidArgs  The string does not contain valid number (e.g., value out of range).
 *
 */
otError ParseAsInt16(const char *aString, int16_t &aInt16);

/**
 * This function parses a string as a `int32_t` value.
 *
 * The number in string is parsed as decimal or hex format (if contains `0x` or `0X` prefix). The string can start with
 * `+`/`-` sign.
 *
 * @param[in]  aString   The string to parse.
 * @param[out] aInt32    A reference to an `int32_t` variable to output the parsed value.
 *
 * @retval kErrorNone         The string was parsed successfully.
 * @retval kErrorInvalidArgs  The string does not contain valid number (e.g., value out of range).
 *
 */
otError ParseAsInt32(const char *aString, int32_t &aInt32);

/**
 * This function parses a string as a `bool` value.
 *
 * Zero value is treated as `false`, non-zero value as `true`.
 *
 * @param[in]  aString   The string to parse.
 * @param[out] aBool     A reference to a `bool` variable to output the parsed value.
 *
 * @retval kErrorNone         The string was parsed successfully.
 * @retval kErrorInvalidArgs  The string does not contain valid number.
 *
 */
otError ParseAsBool(const char *aString, bool &aBool);

#if OPENTHREAD_FTD || OPENTHREAD_MTD
/**
 * This function parses a string as an IPv6 address.
 *
 *
 * @param[in]  aString   The string to parse.
 * @param[out] aAddress  A reference to an `otIp6Address` to output the parsed IPv6 address.
 *
 * @retval kErrorNone         The string was parsed successfully.
 * @retval kErrorInvalidArgs  The string does not contain valid IPv6 address.
 *
 */
otError ParseAsIp6Address(const char *aString, otIp6Address &aAddress);

/**
 * This function parses a string as an IPv6 prefix.
 *
 * The string is parsed as `{IPv6Address}/{PrefixLength}`.
 *
 * @param[in]  aString   The string to parse.
 * @param[out] aPrefix   A reference to an `otIp6Prefix` to output the parsed IPv6 prefix.
 *
 * @retval kErrorNone         The string was parsed successfully.
 * @retval kErrorInvalidArgs  The string does not contain a valid IPv6 prefix.
 *
 */
otError ParseAsIp6Prefix(const char *aString, otIp6Prefix &aPrefix);
#endif // OPENTHREAD_FTD || OPENTHREAD_MTD

/**
 * This function parses a hex string into a byte array of fixed expected size.
 *
 * This function returns `kErrorNone` only when the hex string contains exactly @p aSize bytes (after parsing). If
 * there are fewer or more bytes in hex string that @p aSize, the parsed bytes (up to @p aSize) are copied into the
 * `aBuffer` and `kErrorInvalidArgs` is returned.
 *
 * This function correctly handles hex strings with even or odd length. For example, "AABBCCDD" (with even length) is
 * parsed as {0xaa, 0xbb, 0xcc, 0xdd} and "123" (with odd length) is parsed as {0x01, 0x23}.
 *
 * @param[in]  aString   The string to parse.
 * @param[out] aBuffer   A pointer to a buffer to output the parsed byte sequence.
 * @param[in]  aSize     The expected size of byte sequence (number of bytes after parsing).
 *
 * @retval kErrorNone         The string was parsed successfully.
 * @retval kErrorInvalidArgs  The string does not contain valid hex bytes and/or not @p aSize bytes.
 *
 */
otError ParseAsHexString(const char *aString, uint8_t *aBuffer, uint16_t aSize);

/**
 * This template function parses a hex string into a a given fixed size array.
 *
 * This function returns `kErrorNone` only when the hex string contains exactly @p kBufferSize bytes (after parsing).
 * If there are fewer or more bytes in hex string that @p kBufferSize, the parsed bytes (up to @p kBufferSize) are
 * copied into the `aBuffer` and `kErrorInvalidArgs` is returned.
 *
 * This function correctly handles hex strings with even or odd length. For example, "AABBCCDD" (with even length) is
 * parsed as {0xaa, 0xbb, 0xcc, 0xdd} and "123" (with odd length) is parsed as {0x01, 0x23}.
 *
 * @tparam kBufferSize   The byte array size (number of bytes).
 *
 * @param[in]  aString   The string to parse.
 * @param[out] aBuffer   A reference to a byte array to output the parsed byte sequence.
 *
 * @retval kErrorNone         The string was parsed successfully.
 * @retval kErrorInvalidArgs  The string does not contain valid hex bytes and/or not @p aSize bytes.
 *
 */
template <uint16_t kBufferSize> static otError ParseAsHexString(const char *aString, uint8_t (&aBuffer)[kBufferSize])
{
    return ParseAsHexString(aString, aBuffer, kBufferSize);
}

/**
 * This function parses a hex string into a byte array.
 *
 * This function verifies that the parsed hex string bytes fit in @p aBuffer with its given @p aSize.
 *
 * This function correctly handles hex strings with even or odd length. For example, "AABBCCDD" (with even length) is
 * parsed as {0xaa, 0xbb, 0xcc, 0xdd} and "123" (with odd length) is parsed as {0x01, 0x23}.
 *
 * @param[in]     aString   The string to parse.
 * @param[inout]  aSize     On entry indicates the number of bytes in @p aBuffer (max size of @p aBuffer).
 *                          On exit provides number of bytes parsed and copied into @p aBuffer.
 * @param[out]    aBuffer   A pointer to a buffer to output the parsed byte sequence.
 *
 * @retval kErrorNone        The string was parsed successfully.
 * @retval kErrorInvalidArgs The string does not contain valid format or too many bytes.
 *
 */
otError ParseAsHexString(const char *aString, uint16_t &aSize, uint8_t *aBuffer);

/**
 * This function parses a segment of a hex string up to a given size.
 *
 * This function allows a longer hex string to be parsed and read in smaller segments into a given buffer. If the
 * entire hex string bytes can fit in the given @p aBuffer with its @p aSize, they are copied into @p aBuffer and
 * function returns `kErrorNone`. Otherwise, @p aSize bytes are read and copied and function returns `kErrorPending`
 * to indicate that there are more bytes to parse. The @p aString is also updated to skip over the parsed segment.
 *
 * This function correctly handles hex strings with even or odd length. For example, "AABBCCDD" (with even length) is
 * parsed as {0xaa, 0xbb, 0xcc, 0xdd} and "123" (with odd length) is parsed as {0x01, 0x23}.
 *
 * @param[inout] aString     A reference to string to parse. On successful parse, updated to skip parsed digits.
 * @param[inout] aSize       On entry indicates the segment size (number of bytes in @p aBuffer).
 *                           On exit provides number of bytes parsed and copied into @p aBuffer.
 * @param[out]   aBuffer     A pointer to a buffer to output the parsed byte sequence.
 *
 * @retval kErrorNone        The string was parsed successfully to the end of string.
 * @retval kErrorPedning     The string segment was parsed successfully, but there are additional bytes remaining
 *                           to be parsed.
 * @retval kErrorInvalidArgs The string does not contain valid format hex digits.
 *
 */
otError ParseAsHexStringSegment(const char *&aString, uint16_t &aSize, uint8_t *aBuffer);

/**
 * This class represents a single argument from an argument list.
 *
 */
class Arg
{
public:
    /**
     * This method clears the argument.
     *
     */
    void Clear(void) { mString = nullptr; }

    /**
     * This method indicates whether or not the argument is empty (i.e., reached the end of argument list).
     *
     * @retval TRUE   The argument is empty.
     * @retval FALSE  The argument is not empty.
     *
     */
    bool IsEmpty(void) const { return (mString == nullptr); }

    /**
     * This method returns the length (number of characters) in the argument C string.
     *
     * @returns The argument string length if argument is not empty, zero otherwise.
     *
     */
    uint16_t GetLength(void) const;

    /**
     * This method gets the argument as a C string.
     *
     * @returns A pointer to the argument as a C string, or `nullptr` if argument is empty.
     *
     */
    const char *GetCString(void) const { return mString; }

    /**
     * This method gets the argument as C string.
     *
     * @returns A pointer to the argument as a C string, or `nullptr` if argument is empty.
     *
     */
    char *GetCString(void) { return mString; }

    /**
     * This method sets the argument with a given C string.
     *
     * @param[in] aString    A pointer to the new C string.
     *
     */
    void SetCString(char *aString) { mString = aString; }

    /**
     * This method overload the operator `==` to evaluate whether the argument is equal to a given C string.
     *
     * If the argument is empty (`IsEmpty()` is `true`) then comparing it using operator `==` with any C string will
     * return false.
     *
     * @param[in] aString    The C string to compare with (MUST not be `nullptr`).
     *
     * @retval TRUE   If the argument is not empty and is equal to @p aString.
     * @retval FALSE  If the argument is not equal to @p aString, or if the argument is empty.
     *
     */
    bool operator==(const char *aString) const;

    /**
     * This method overload the operator `!=` to evaluate whether the argument is unequal to a given C string.
     *
     * @param[in] aString    The C string to compare with (MUST not be `nullptr`).
     *
     * @retval TRUE   If the argument is not equal to @p aString, or if the argument is empty.
     * @retval FALSE  If the argument is not empty and equal to @p aString.
     *
     */
    bool operator!=(const char *aString) const { return !(*this == aString); }

    /**
     * This method parses the argument as a `uint8_t` value.
     *
     * The number is parsed as decimal or hex format (if contains `0x` or `0X` prefix).
     *
     * @param[out] aUint8    A reference to an `uint8_t` variable to output the parsed value.
     *
     * @retval kErrorNone         The argument was parsed successfully.
     * @retval kErrorInvalidArgs  The argument is empty or does not contain valid number (e.g., value out of range).
     *
     */
    otError ParseAsUint8(uint8_t &aUint8) const { return CmdLineParser::ParseAsUint8(mString, aUint8); }

    /**
     * This method parses the argument as a `uint16_t` value.
     *
     * The number is parsed as decimal or hex format (if contains `0x` or `0X` prefix).
     *
     * @param[out] aUint16   A reference to an `uint16_t` variable to output the parsed value.
     *
     * @retval kErrorNone         The argument was parsed successfully.
     * @retval kErrorInvalidArgs  The argument is empty or does not contain valid number (e.g., value out of range).
     *
     */
    otError ParseAsUint16(uint16_t &aUint16) const { return CmdLineParser::ParseAsUint16(mString, aUint16); }

    /**
     * This method parses the argument as a `uint32_t` value.
     *
     * The number is parsed as decimal or hex format (if contains `0x` or `0X` prefix).
     *
     * @param[out] aUint32   A reference to an `uint32_t` variable to output the parsed value.
     *
     * @retval kErrorNone         The argument was parsed successfully.
     * @retval kErrorInvalidArgs  The argument is empty or does not contain valid number (e.g., value out of range).
     *
     */
    otError ParseAsUint32(uint32_t &aUint32) const { return CmdLineParser::ParseAsUint32(mString, aUint32); }

    /**
     * This method parses the argument as a `uint64_t` value.
     *
     * The number is parsed as decimal or hex format (if contains `0x` or `0X` prefix).
     *
     * @param[out] aUint64   A reference to an `uint64_t` variable to output the parsed value.
     *
     * @retval kErrorNone         The argument was parsed successfully.
     * @retval kErrorInvalidArgs  The argument is empty or does not contain valid number (e.g., value out of range).
     *
     */
    otError ParseAsUint64(uint64_t &aUint64) const { return CmdLineParser::ParseAsUint64(mString, aUint64); }

    /**
     * This method parses the argument as a `int8_t` value.
     *
     * The number is parsed as decimal or hex format (if contains `0x` or `0X` prefix). The string can start with
     * `+`/`-` sign.
     *
     * @param[out] aInt8     A reference to an `int8_t` variable to output the parsed value.
     *
     * @retval kErrorNone         The argument was parsed successfully.
     * @retval kErrorInvalidArgs  The argument is empty or does not contain valid number (e.g., value out of range).
     *
     */
    otError ParseAsInt8(int8_t &aInt8) const { return CmdLineParser::ParseAsInt8(mString, aInt8); }

    /**
     * This method parses the argument as a `int16_t` value.
     *
     * The number is parsed as decimal or hex format (if contains `0x` or `0X` prefix). The string can start with
     * `+`/`-` sign.
     *
     * @param[out] aInt16    A reference to an `int16_t` variable to output the parsed value.
     *
     * @retval kErrorNone         The argument was parsed successfully.
     * @retval kErrorInvalidArgs  The argument is empty or does not contain valid number (e.g., value out of range).
     *
     */
    otError ParseAsInt16(int16_t &aInt16) const { return CmdLineParser::ParseAsInt16(mString, aInt16); }

    /**
     * This method parses the argument as a `int32_t` value.
     *
     * The number is parsed as decimal or hex format (if contains `0x` or `0X` prefix). The string can start with
     * `+`/`-` sign.
     *
     * @param[out] aInt32    A reference to an `int32_t` variable to output the parsed value.
     *
     * @retval kErrorNone         The argument was parsed successfully.
     * @retval kErrorInvalidArgs  The argument is empty or does not contain valid number (e.g., value out of range).
     *
     */
    otError ParseAsInt32(int32_t &aInt32) const { return CmdLineParser::ParseAsInt32(mString, aInt32); }

    /**
     * This method parses the argument as a `bool` value.
     *
     * Zero value is treated as `false`, non-zero value as `true`.
     *
     * @param[out] aBool     A reference to a `bool` variable to output the parsed value.
     *
     * @retval kErrorNone         The argument was parsed successfully.
     * @retval kErrorInvalidArgs  The argument is empty or does not contain valid number.
     *
     */
    otError ParseAsBool(bool &aBool) const { return CmdLineParser::ParseAsBool(mString, aBool); }

#if OPENTHREAD_FTD || OPENTHREAD_MTD
    /**
     * This method parses the argument as an IPv6 address.
     *
     * @param[out] aAddress  A reference to an `otIp6Address` to output the parsed IPv6 address.
     *
     * @retval kErrorNone         The argument was parsed successfully.
     * @retval kErrorInvalidArgs  The argument is empty or does not contain valid IPv6 address.
     *
     */
    otError ParseAsIp6Address(otIp6Address &aAddress) const
    {
        return CmdLineParser::ParseAsIp6Address(mString, aAddress);
    }

    /**
     * This method parses the argument as an IPv6 prefix.
     *
     * The string is parsed as `{IPv6Address}/{PrefixLength}`.
     *
     * @param[out] aPrefix   A reference to an `otIp6Prefix` to output the parsed IPv6 prefix.
     *
     * @retval kErrorNone         The argument was parsed successfully.
     * @retval kErrorInvalidArgs  The argument is empty or does not contain a valid IPv6 prefix.
     *
     */
    otError ParseAsIp6Prefix(otIp6Prefix &aPrefix) const { return CmdLineParser::ParseAsIp6Prefix(mString, aPrefix); }

#endif // OPENTHREAD_FTD || OPENTHREAD_MTD

    /**
     * This template method parses the argument as a specified value type.
     *
     * @tparam Type               The value type.
     *
     * @param[out] aValue         A reference to output the parsed value.
     *
     * @retval kErrorNone         The argument was parsed successfully.
     * @retval kErrorInvalidArgs  The argument is empty or does not contain a valid value.
     *
     */
    template <typename Type> otError ParseAs(Type &aValue) const;

    /**
     * This method parses the argument as a hex string into a byte array of fixed expected size.
     *
     * This method returns `kErrorNone` only when the hex string contains exactly @p aSize bytes (after parsing). If
     * there are fewer or more bytes in hex string that @p aSize, the parsed bytes (up to @p aSize) are copied into the
     * `aBuffer` and `kErrorInvalidArgs` is returned.
     *
     * @param[out] aBuffer   A pointer to a buffer to output the parsed byte sequence.
     * @param[in]  aSize     The expected size of byte sequence (number of bytes after parsing).
     *
     * @retval kErrorNone         The argument was parsed successfully.
     * @retval kErrorInvalidArgs  The argument is empty or does not contain valid hex bytes and/or not @p aSize bytes.
     *
     */
    otError ParseAsHexString(uint8_t *aBuffer, uint16_t aSize) const
    {
        return CmdLineParser::ParseAsHexString(mString, aBuffer, aSize);
    }

    /**
     * This template method parses the argument as a hex string into a a given fixed size array.
     *
     * This method returns `kErrorNone` only when the hex string contains exactly @p kBufferSize bytes (after parsing).
     * If there are fewer or more bytes in hex string that @p kBufferSize, the parsed bytes (up to @p kBufferSize) are
     * copied into the `aBuffer` and `kErrorInvalidArgs` is returned.
     *
     * @tparam kBufferSize   The byte array size (number of bytes).
     *
     * @param[out] aBuffer   A reference to a byte array to output the parsed byte sequence.
     *
     * @retval kErrorNone         The argument was parsed successfully.
     * @retval kErrorInvalidArgs  The argument is empty or does not contain valid hex bytes and/or not @p aSize bytes.
     *
     */
    template <uint16_t kBufferSize> otError ParseAsHexString(uint8_t (&aBuffer)[kBufferSize])
    {
        return ParseAsHexString(aBuffer, kBufferSize);
    }

    /**
     * This method parses the argument as a hex string into a byte array.
     *
     * This method verifies that the parsed hex string bytes fit in @p aBuffer with its given @p aSize.
     *
     * @param[inout]  aSize     On entry indicates the number of bytes in @p aBuffer (max size of @p aBuffer).
     *                          On exit provides number of bytes parsed and copied into @p aBuffer.
     * @param[out]    aBuffer   A pointer to a buffer to output the parsed byte sequence.
     *
     * @retval kErrorNone        The argument was parsed successfully.
     * @retval kErrorInvalidArgs The argument does not contain valid format or too many bytes.
     *
     */
    otError ParseAsHexString(uint16_t &aSize, uint8_t *aBuffer)
    {
        return CmdLineParser::ParseAsHexString(mString, aSize, aBuffer);
    }

    /**
     * This static method copies the argument string pointers from an `Arg` array to a C string array.
     *
     * @note this method only copies the string pointer value (i.e., `GetString()` pointer) from `aArgs` array to the
     * @p aStrings array (the content of strings are not copied).
     *
     * @param[in]  aArgs        An `Arg` array.
     * @param[out] aStrings     An `char *` array to populate with the argument string pointers. The @p aString array
     *                          MUST contain at least same number of entries as in @p aArgs array.
     *
     */
    static void CopyArgsToStringArray(Arg aArgs[], char *aStrings[]);

    /**
     * This static method returns the length of argument array, i.e. number of consecutive non-empty arguments.
     *
     * @param[in] aArgs  An `Arg` array.
     *
     * @returns  Number of non-empty arguments in the array.
     *
     */
    static uint8_t GetArgsLength(Arg aArgs[]);

private:
    char *mString;
};

/**
 * This function parses a given command line string and breaks it into an argument list.
 *
 * This function may change the input @p aCommandString, it will put a '\0' by the end of each argument, and @p aArgs
 * will point to the arguments in the input @p aCommandString. Backslash ('\') can be used to escape separators
 * (' ', '\t', '\r', '\n') and the backslash itself.
 *
 * As the arguments are parsed, the @p aArgs array entries are populated. Any remaining @p aArgs entries in the array
 * will be cleared and marked as empty. So the number of arguments can be determined by going through @p aArgs array
 * entries till we get to an empty `Arg` (i.e., `Arg::IsEmpty()` returns `true).
 *
 * This function ensures that the last entry in @p aArgs array is always used to indicate the end (always  marked as
 * empty), so the @p aArgs array should have one more entry than the desired max number of arguments.
 *
 * @param[in]   aCommandString  A null-terminated input string.
 * @param[out]  aArgs           The argument array.
 * @param[in]   aArgsMaxLength  The max length of @p aArgs array.
 *
 * @retval OT_ERROR_NONE          The command line parsed successfully and @p aArgs array is populated.
 * @retval OT_ERROR_INVALID_ARGS  Too many arguments in @p aCommandString and could not fit in @p aArgs array.
 *
 */
otError ParseCmd(char *aCommandString, Arg aArgs[], uint8_t aArgsMaxLength);

template <uint8_t kLength> inline otError ParseCmd(char *aCommandString, Arg (&aArgs)[kLength])
{
    return ParseCmd(aCommandString, aArgs, kLength);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Specializations of `Arg::ParseAs<Type>()` method.

template <> inline otError Arg::ParseAs(uint8_t &aValue) const
{
    return ParseAsUint8(aValue);
}

template <> inline otError Arg::ParseAs(uint16_t &aValue) const
{
    return ParseAsUint16(aValue);
}

template <> inline otError Arg::ParseAs(uint32_t &aValue) const
{
    return ParseAsUint32(aValue);
}

template <> inline otError Arg::ParseAs(uint64_t &aValue) const
{
    return ParseAsUint64(aValue);
}

template <> inline otError Arg::ParseAs(bool &aValue) const
{
    return ParseAsBool(aValue);
}

template <> inline otError Arg::ParseAs(int8_t &aValue) const
{
    return ParseAsInt8(aValue);
}

template <> inline otError Arg::ParseAs(int16_t &aValue) const
{
    return ParseAsInt16(aValue);
}

template <> inline otError Arg::ParseAs(int32_t &aValue) const
{
    return ParseAsInt32(aValue);
}

#if OPENTHREAD_FTD || OPENTHREAD_MTD

template <> inline otError Arg::ParseAs(otIp6Address &aValue) const
{
    return ParseAsIp6Address(aValue);
}

template <> inline otError Arg::ParseAs(otIp6Prefix &aValue) const
{
    return ParseAsIp6Prefix(aValue);
}

#endif

/**
 * @}
 */

} // namespace CmdLineParser
} // namespace Utils
} // namespace ot

#endif // PARSE_CMD_LINE_HPP_
