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
 *   This file includes type traits definitions.
 */

#ifndef OT_TYPE_TRAITS_HPP_
#define OT_TYPE_TRAITS_HPP_

namespace ot {
namespace TypeTraits {

/**
 * This type represents a true value (contains a `true` static `kValue` member variable).
 *
 */
struct TrueValue
{
    constexpr static bool kValue = true; ///< true value.
};

/**
 * This type represents a false value (contains a `false` static `kValue` member variable).
 *
 */
struct FalseValue
{
    constexpr static bool kValue = false; ///< false value.
};

/**
 * This type indicates whether or not a given template `Type` is a pointer type.
 *
 * The `constexpr` expression `IsPointer<Type>::kValue` would be `true` when the `Type` is a pointer, otherwise it
 * would be `false`.
 *
 * @tparam Type    A type to check if is a pointer.
 *
 */
template <typename Type> struct IsPointer : public FalseValue
{
};

// Partial template specializations of the `IsPointer<Type>`

template <typename Type> struct IsPointer<Type *> : public TrueValue
{
};

template <typename Type> struct IsPointer<const Type *> : public TrueValue
{
};

template <typename Type> struct IsPointer<volatile Type *> : public TrueValue
{
};

template <typename Type> struct IsPointer<const volatile Type *> : TrueValue
{
};

/**
 * This type indicates whether or not a given template `FirstType is the same as `SecondType`.
 *
 * The `constexpr` expression `IsSame<FirstType, SecondType>::kValue` would be `true` when the two types are the same,
 * otherwise it would be `false`.
 *
 * @tparam FirstType     The first type.
 * @tparam SecondType    The second type.
 *
 */
template <typename FirstType, typename SecondType> struct IsSame : public FalseValue
{
};

template <typename Type> struct IsSame<Type, Type> : public TrueValue
{
};

/**
 * This type selects between two given types based on a boolean condition at compile time.
 *
 * It provides member type named `Type` which is defined as `TypeOnTrue` if `kCondition` is `true` at compile time, or
 * as `TypeOnFalse` if `kCondition` is `false`.
 *
 * @tparam kCondition   The boolean condition which is used to select between the two types.
 * @tparam TypeOnTrue   The type to select when `kCondition` is `true`.
 * @tparam TypeOnFalse  The type to select when `kCondition` is `false`.
 *
 */
template <bool kCondition, typename TypeOnTrue, typename TypeOnFalse> struct Conditional
{
    typedef TypeOnFalse Type; ///< The selected type based on `kCondition`.
};

template <typename TypeOnTrue, typename TypeOnFalse> struct Conditional<true, TypeOnTrue, TypeOnFalse>
{
    typedef TypeOnTrue Type;
};

} // namespace TypeTraits
} // namespace ot

#endif // OT_TYPE_TRAITS_HPP_
