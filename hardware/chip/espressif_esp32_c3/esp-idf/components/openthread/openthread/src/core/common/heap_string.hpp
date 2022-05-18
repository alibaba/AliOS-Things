/*
 *  Copyright (c) 2021, The OpenThread Authors.
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
 *   This file includes definitions for `HeapString` (a heap allocated string).
 */

#ifndef HEAP_STRING_HPP_
#define HEAP_STRING_HPP_

#include "openthread-core-config.h"

#include "common/equatable.hpp"
#include "common/error.hpp"

namespace ot {

/**
 * This class represents a heap allocated string.
 *
 * The buffer to store the string is allocated from heap and is manged by the `HeapString` class itself, e.g., it may
 * be reused and/or freed and reallocated when the string is set. The `HeapString` destructor will always free the
 * allocated buffer.
 *
 */
class HeapString : public Unequatable<HeapString>
{
public:
    /**
     * This constructor initializes the `HeapString` as null (or empty).
     *
     */
    HeapString(void)
        : mStringBuffer(nullptr)
    {
    }

    /**
     * This is the move constructor for `HeapString`.
     *
     * `HeapString` is non-copyable (copy constructor is deleted) but move constructor is provided to allow it to to be
     * used as return type (return by value) from functions/methods (which will then use move semantics).
     *
     */
    HeapString(HeapString &&aString)
        : mStringBuffer(aString.mStringBuffer)
    {
        aString.mStringBuffer = nullptr;
    }

    /**
     * This is the destructor for `HealString` object
     *
     */
    ~HeapString(void) { Free(); }

    /**
     * This method indicates whether or not the `HeapString` is null (i.e., it was never successfully set or it was
     * freed).
     *
     * @retval TRUE  The `HeapString` is null.
     * @retval FALSE The `HeapString` is not null.
     *
     */
    bool IsNull(void) const { return (mStringBuffer == nullptr); }

    /**
     * This method returns the `HeapString` as a C string.
     *
     * @returns A pointer to C string buffer or `nullptr` if the `HeapString` is null (never set or freed).
     *
     */
    const char *AsCString(void) const { return mStringBuffer; }

    /**
     * This method sets the string from a given C string.
     *
     * @param[in] aCString   A pointer to c string buffer. Can be `nullptr` which then frees the `HeapString`.
     *
     * @retval kErrorNone     Successfully set the string.
     * @retval kErrorNoBufs   Failed to allocate buffer for string.
     *
     */
    Error Set(const char *aCString);

    /**
     * This method sets the string from another `HeapString`.
     *
     * @param[in] aString   The other `HeapString` to set from.
     *
     * @retval kErrorNone     Successfully set the string.
     * @retval kErrorNoBufs   Failed to allocate buffer for string.
     *
     */
    Error Set(const HeapString &aString) { return Set(aString.AsCString()); }

    /**
     * This method sets the string from another `HeapString`.
     *
     * @param[in] aString     The other `HeapString` to set from (rvalue reference using move semantics).
     *
     * @retval kErrorNone     Successfully set the string.
     * @retval kErrorNoBufs   Failed to allocate buffer for string.
     *
     */
    Error Set(HeapString &&aString);

    /**
     * This method frees any buffer allocated by the `HeapString`.
     *
     * The `HeapString` destructor will automatically call `Free()`. This method allows caller to free buffer
     * explicitly.
     *
     */
    void Free(void);

    /**
     * This method overloads operator `==` to evaluate whether or not the `HeapString` is equal to a given C string.
     *
     * @param[in]  aCString  A C string to compare with. Can be `nullptr` which then checks if `HeapString` is null.
     *
     * @retval TRUE   If the two strings are equal.
     * @retval FALSE  If the two strings are not equal.
     *
     */
    bool operator==(const char *aCString) const;

    /**
     * This method overloads operator `!=` to evaluate whether or not the `HeapString` is unequal to a given C string.
     *
     * @param[in]  aCString  A C string to compare with. Can be `nullptr` which then checks if `HeapString` is not null.
     *
     * @retval TRUE   If the two strings are not equal.
     * @retval FALSE  If the two strings are equal.
     *
     */
    bool operator!=(const char *aCString) const { return !(*this == aCString); }

    /**
     * This method overloads operator `==` to evaluate whether or not two `HeapString` are equal.
     *
     * @param[in]  aString  The other string to compare with.
     *
     * @retval TRUE   If the two strings are equal.
     * @retval FALSE  If the two strings are not equal.
     *
     */
    bool operator==(const HeapString &aString) const { return (*this == aString.AsCString()); }

    HeapString(const HeapString &) = delete;
    HeapString &operator=(const HeapString &) = delete;

private:
    char *mStringBuffer;
};

} // namespace ot

#endif // HEAP_STRING_HPP_
