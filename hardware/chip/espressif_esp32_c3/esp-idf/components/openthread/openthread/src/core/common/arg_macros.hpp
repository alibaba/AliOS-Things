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
 *   This file includes macros for parsing variadic arguments.
 */

#ifndef OT_ARG_MACROS_HPP_
#define OT_ARG_MACROS_HPP_

/**
 * This macro returns the first argument in a list of input arguments.
 *
 * @param[in] ...   A list of arguments (MUST contain at least one).
 *
 * @returns The first argument in the given list of input arguments.
 *
 */
#define OT_FIRST_ARG(...) _OT_GET_FIRST_ARG(__VA_ARGS__, JUNK)

/**
 * This macro returns the second argument in a list of input arguments if any.
 *
 * @note: This macro works when the list contains either one or two arguments.
 *
 * @param[in] ...   A list of arguments (MUST contain either one or two arguments).
 *
 * @returns The second argument if any.
 *
 */
#define OT_SECOND_ARG(...) _OT_GET_SECOND_ARG(_OT_HAS_ONE_ARG_OR_TWO_ARGS(__VA_ARGS__), __VA_ARGS__)

/**
 * This macro expands to comma separated list of arguments excluding the first one.
 *
 * If there is only one argument, it expands to empty. If there is more than one argument, it expands to an initial
 * comma followed by all the rest of arguments excluding the first one.
 *
 * @note: This macro supports up to 20 arguments.
 *
 * @param[in] ...  A list of arguments (MUST contain at least one).
 *
 * @returns A comma separated list of arguments excluding the first one.
 *
 */
#define OT_REST_ARGS(...) _OT_GET_REST_ARGS(_OT_HAS_ONE_ARG_VS_TWO_OR_MORE(__VA_ARGS__), __VA_ARGS__)

//---------------------------------------------------------------------------------------------------------------------
// Private/local macros - for use in this header only.

#define _OT_GET_FIRST_ARG(aFirst, ...) aFirst

#define _OT_HAS_ONE_ARG_OR_TWO_ARGS(...) _OT_SELECT_3(__VA_ARGS__, TWO_ARGS, ONE_ARG, JUNK)
#define _OT_SELECT_3(a1, a2, a3, ...) a3

#define _OT_GET_SECOND_ARG(aNum, ...) _OT_GET_SECOND_APPEND_NUM_ARGS(aNum, __VA_ARGS__)
#define _OT_GET_SECOND_APPEND_NUM_ARGS(aNum, ...) _OT_GET_SECOND_WITH_##aNum(__VA_ARGS__)

#define _OT_GET_SECOND_WITH_ONE_ARG(aFirst)
#define _OT_GET_SECOND_WITH_TWO_ARGS(aFirst, aSecond) aSecond

#define _OT_GET_REST_ARGS(aNum, ...) _OT_GET_REST_APPEND_NUM_ARGS(aNum, __VA_ARGS__)
#define _OT_GET_REST_APPEND_NUM_ARGS(aNum, ...) _OT_GET_REST_WITH_##aNum(__VA_ARGS__)

#define _OT_GET_REST_WITH_ONE_ARG(aFirst)
#define _OT_GET_REST_WITH_TWO_OR_MORE_ARGS(aFirst, ...) , __VA_ARGS__

#define _OT_HAS_ONE_ARG_VS_TWO_OR_MORE(...)                                                                 \
    _OT_SELECT_20(__VA_ARGS__, TWO_OR_MORE_ARGS, TWO_OR_MORE_ARGS, TWO_OR_MORE_ARGS, TWO_OR_MORE_ARGS,      \
                  TWO_OR_MORE_ARGS, TWO_OR_MORE_ARGS, TWO_OR_MORE_ARGS, TWO_OR_MORE_ARGS, TWO_OR_MORE_ARGS, \
                  TWO_OR_MORE_ARGS, TWO_OR_MORE_ARGS, TWO_OR_MORE_ARGS, TWO_OR_MORE_ARGS, TWO_OR_MORE_ARGS, \
                  TWO_OR_MORE_ARGS, TWO_OR_MORE_ARGS, TWO_OR_MORE_ARGS, TWO_OR_MORE_ARGS, ONE_ARG, JUNK)

#define _OT_SELECT_20(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, ...) \
    a20

#endif // OT_ARG_MACROS_HPP_
