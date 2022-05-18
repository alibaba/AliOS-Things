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
 *   This file includes definitions for Equatable class for OpenThread objects.
 */

#ifndef EQUATABLE_HPP_
#define EQUATABLE_HPP_

#include "openthread-core-config.h"

#include <string.h>

namespace ot {

/**
 * This template class defines an overload of operator `!=`.
 *
 * The `!=` implementation uses an existing `==` overload provided by the `Type` class.
 *
 * Users of this class should follow CRTP-style inheritance, i.e., the `Type` class itself should publicly inherit
 * from `Unequatable<Type>`.
 *
 */
template <typename Type> class Unequatable
{
public:
    /**
     * This method overloads operator `!=` to evaluate whether or not two instances of `Type` are equal.
     *
     * This is implemented in terms of an existing `==` overload provided by `Type` class itself.
     *
     * @param[in]  aOther  The other `Type` instance to compare with.
     *
     * @retval TRUE   If the two `Type` instances are not equal.
     * @retval FALSE  If the two `Type` instances are equal.
     *
     */
    bool operator!=(const Type &aOther) const { return !(*static_cast<const Type *>(this) == aOther); }
};

/**
 * This template class defines overloads of operators `==` and `!=`.
 *
 * The `==` implementation simply compares all the bytes of two `Type` instances to be equal (using `memcmp()`).
 *
 * Users of this class should follow CRTP-style inheritance, i.e., the `Type` class itself should publicly inherit
 * from `Equatable<Type>`.
 *
 */
template <typename Type> class Equatable : public Unequatable<Type>
{
public:
    /**
     * This method overloads operator `==` to evaluate whether or not two instances of `Type` are equal.
     *
     * @param[in]  aOther  The other `Type` instance to compare with.
     *
     * @retval TRUE   If the two `Type` instances are equal.
     * @retval FALSE  If the two `Type` instances are not equal.
     *
     */
    bool operator==(const Type &aOther) const { return memcmp(this, &aOther, sizeof(Type)) == 0; }
};

} // namespace ot

#endif // EQUATABLE_HPP_
