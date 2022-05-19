/*
 *  Copyright (c) 2017, The OpenThread Authors.
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
 *   This file includes definitions for locator class for OpenThread objects.
 */

#ifndef LOCATOR_HPP_
#define LOCATOR_HPP_

#include "openthread-core-config.h"

#include <openthread/platform/toolchain.h>

#include <stdint.h>

namespace ot {

class Instance;

#if !OPENTHREAD_CONFIG_MULTIPLE_INSTANCE_ENABLE
extern uint64_t gInstanceRaw[];
#endif

/**
 * @addtogroup core-locator
 *
 * @brief
 *   This module includes definitions for OpenThread instance locator.
 *
 * @{
 *
 */

/**
 * This class implements a locator for an OpenThread Instance object.
 *
 * The `InstanceLocator` is used as base class of almost all other OpenThread classes. It provides a way for an object
 * to get to its owning/parent OpenThread `Instance` and also any other `Type` within the `Instance` member variable
 * property hierarchy (using `Get<Type>()` method).
 *
 * If multiple-instance feature is supported, the owning/parent OpenThread `Instance` is tracked as a reference. In the
 * single-instance case, this class becomes an empty base class.
 *
 */
class InstanceLocator
{
    friend class InstanceLocatorInit;

public:
    /**
     * This method returns a reference to the parent OpenThread Instance.
     *
     * @returns A reference to the parent otInstance.
     *
     */
#if OPENTHREAD_CONFIG_MULTIPLE_INSTANCE_ENABLE
    Instance &GetInstance(void) const { return *mInstance; }
#else
    Instance &GetInstance(void) const { return *reinterpret_cast<Instance *>(&gInstanceRaw); }
#endif

    /**
     * This template method returns a reference to a given `Type` object belonging to the OpenThread instance.
     *
     * For example, `Get<MeshForwarder>()` returns a reference to the `MeshForwarder` object of the instance.
     *
     * Note that any `Type` for which the `Get<Type>` is defined MUST be uniquely accessible from the OpenThread
     * `Instance` through the member variable property hierarchy.
     *
     * @returns A reference to the `Type` object of the instance.
     *
     */
    template <typename Type> inline Type &Get(void) const; // Implemented in `locator_getters.hpp`.

protected:
    /**
     * This constructor initializes the object.
     *
     * @param[in]  aInstance  A reference to the OpenThread Instance.
     *
     */
    explicit InstanceLocator(Instance &aInstance)
#if OPENTHREAD_CONFIG_MULTIPLE_INSTANCE_ENABLE
        : mInstance(&aInstance)
#endif
    {
        OT_UNUSED_VARIABLE(aInstance);
    }

private:
    InstanceLocator(void) = default;

#if OPENTHREAD_CONFIG_MULTIPLE_INSTANCE_ENABLE
    Instance *mInstance;
#endif
};

/**
 * This class implements a locator for an OpenThread Instance object.
 *
 * The `InstanceLocatorInit` is similar to `InstanceLocator` but provides a default constructor (instead of a
 * parameterized one) and allows an inheriting class to initialize the object (set the OpenThread Instance) post
 * constructor call using the `Init()` method. This class is intended for types that require a default constructor and
 * cannot use a parameterized one. (e.g., `Neighbor`/`Child`/`Router` classes which are used as a C array element type
 * in`ChildTable`/`RouterTable`).
 *
 * The inheriting class from `InstanceLocatorInit` should ensure that object is properly initialized after the object
 * is created and more importantly that it is re-initialized when/if it is cleared or reset.
 *
 */
class InstanceLocatorInit : public InstanceLocator
{
protected:
    /**
     * This is the default constructor for the `InstanceLocatorInit` object.
     *
     */
    InstanceLocatorInit(void)
        : InstanceLocator()
    {
    }

    /**
     * This method (re)initializes the object and sets the OpenThread Instance.
     *
     * @param[in] aInstance  A reference to the OpenThread Instance.
     */
    void Init(Instance &aInstance)
    {
#if OPENTHREAD_CONFIG_MULTIPLE_INSTANCE_ENABLE
        mInstance = &aInstance;
#endif
        OT_UNUSED_VARIABLE(aInstance);
    }
};

/**
 * @}
 *
 */

} // namespace ot

#endif // LOCATOR_HPP_
