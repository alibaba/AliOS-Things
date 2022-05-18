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
 *   This file includes macros for validating runtime conditions.
 */

#ifndef CODE_UTILS_HPP_
#define CODE_UTILS_HPP_

#include <stdbool.h>

#include <openthread/error.h>

#include "common/arg_macros.hpp"

/**
 * This macro calculates the number of elements in an array.
 *
 * @param[in] aArray  Name of the array variable.
 *
 * @returns Number of elements in the array.
 *
 */
#define OT_ARRAY_LENGTH(aArray) (sizeof(aArray) / sizeof(aArray[0]))

/**
 * This macro returns a pointer to end of a given array (pointing to the past-the-end element).
 *
 * Note that the past-the-end element is a theoretical element that would follow the last element in the array. It does
 * not point to an actual element in array, and thus should not be dereferenced.
 *
 * @param[in]    Name of the array variable
 *
 * @returns Pointer to the past-the-end element.
 *
 */
#define OT_ARRAY_END(aArray) (&aArray[OT_ARRAY_LENGTH(aArray)])

/**
 * This macro returns a pointer aligned by @p aAlignment.
 *
 * @param[in] aPointer      A pointer to contiguous space.
 * @param[in] aAlignment    The desired alignment.
 *
 * @returns The aligned pointer.
 *
 */
#define OT_ALIGN(aPointer, aAlignment) \
    ((void *)(((uintptr_t)(aPointer) + (aAlignment)-1UL) & ~((uintptr_t)(aAlignment)-1UL)))

// Calculates the aligned variable size.
#define OT_ALIGNED_VAR_SIZE(size, align_type) (((size) + (sizeof(align_type) - 1)) / sizeof(align_type))

// Allocate the structure using "raw" storage.
#define OT_DEFINE_ALIGNED_VAR(name, size, align_type) \
    align_type name[(((size) + (sizeof(align_type) - 1)) / sizeof(align_type))]

/**
 * This macro returns the smaller of @p a and @p b.
 *
 */
#define OT_MIN(a, b) ((b) < (a) ? (b) : (a))

/**
 * This macro returns the greater of @p a and @p b.
 *
 */
#define OT_MAX(a, b) ((a) < (b) ? (b) : (a))

/**
 * This macro checks for the specified status, which is expected to commonly be successful, and branches to the local
 * label 'exit' if the status is unsuccessful.
 *
 * @param[in]  aStatus     A scalar status to be evaluated against zero (0).
 *
 */
#define SuccessOrExit(aStatus) \
    do                         \
    {                          \
        if ((aStatus) != 0)    \
        {                      \
            goto exit;         \
        }                      \
    } while (false)

/**
 * This macro checks for the specified condition, which is expected to commonly be true, and both executes @a ... and
 * branches to the local label 'exit' if the condition is false.
 *
 * @param[in]  aCondition  A Boolean expression to be evaluated.
 * @param[in]  aAction     An optional expression or block to execute when the assertion fails.
 *
 */
#define VerifyOrExit(...)                 \
    do                                    \
    {                                     \
        if (!(OT_FIRST_ARG(__VA_ARGS__))) \
        {                                 \
            OT_SECOND_ARG(__VA_ARGS__);   \
            goto exit;                    \
        }                                 \
    } while (false)

/**
 * This macro unconditionally executes @a ... and branches to the local label 'exit'.
 *
 * @note The use of this interface implies neither success nor failure for the overall exit status of the enclosing
 *       function body.
 *
 * @param[in]  ...         An optional expression or block to execute when the assertion fails.
 *
 */
#define ExitNow(...) \
    do               \
    {                \
        __VA_ARGS__; \
        goto exit;   \
    } while (false)

/*
 * This macro executes the `statement` and ignores the return value.
 *
 * This is primarily used to indicate the intention of developer that the return value of a function/method can be
 * safely ignored.
 *
 * @param[in]  aStatement  The function/method to execute.
 *
 */
#define IgnoreReturnValue(aStatement) \
    do                                \
    {                                 \
        if (aStatement)               \
        {                             \
        }                             \
    } while (false)

/**
 * This function ignores an error explicitly.
 *
 * This is primarily used to indicate the intention of developer that
 * the error can be safely ignored or there is guaranteed to be no error.
 *
 * @param[in]  aError  The error to be ignored.
 *
 */
static inline void IgnoreError(otError aError)
{
    OT_UNUSED_VARIABLE(aError);
}

#endif // CODE_UTILS_HPP_
