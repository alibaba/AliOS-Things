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
 *   This file includes definitions for OpenThread vendor extension hooks.
 */

#ifndef EXTENSION_HPP_
#define EXTENSION_HPP_

#include "openthread-core-config.h"

#if OPENTHREAD_ENABLE_VENDOR_EXTENSION

#include "common/locator.hpp"
#include "common/non_copyable.hpp"
#include "common/notifier.hpp"

namespace ot {
namespace Ncp {
class NcpBase;
}
namespace Extension {

/**
 * @addtogroup core-extension
 *
 * @brief
 *   This module includes definitions for OpenThread vendor extension hooks.
 *
 * @{
 *
 */

/**
 * This class defines the base class for an OpenThread vendor Extension object.
 *
 * This class is used by OpenThread core to interact with the extension module. Methods in this class are expected
 * to be implemented by the vendor extension module.
 *
 * Support for vendor extension can be enabled using `OPENTHREAD_ENABLE_VENDOR_EXTENSION` configuration option.
 *
 */
class ExtensionBase : public InstanceLocator, private NonCopyable
{
public:
    /**
     * This static method initializes and gets a vendor extension instance.
     *
     * @param[in]  aInstance     A reference to the OpenThread instance.
     *
     * @note When this method is called, the @p aInstance itself may not yet be fully initialized. The method
     * `SignalInstanceInit()` is called when `aInstance` is fully initialized.
     *
     * @returns A reference to the object.
     *
     */
    static ExtensionBase &Init(Instance &aInstance);

    /**
     * This method notifies the extension object that OpenThread instance has been initialized.
     *
     */
    void SignalInstanceInit(void);

    /**
     * This method notifies the extension object that NCP instance has been initialized.
     *
     * @param[in] aNcpInstance   A reference to the NCP object.
     *
     */
    void SignalNcpInit(Ncp::NcpBase &aNcpInstance);

    /**
     * This method notifies the extension object of events from  OpenThread `Notifier`.
     *
     * @param[in] aEvents   The list of events emitted by `Notifier`.
     *
     */
    void HandleNotifierEvents(Events aEvents);

protected:
    /**
     * This constructor initializes the object.
     *
     * @param[in]  aInstance     A reference to the OpenThread instance.
     *
     */
    explicit ExtensionBase(Instance &aInstance)
        : InstanceLocator(aInstance)
        , mIsInitialized(true)
    {
    }

    bool mIsInitialized;
};

/**
 * @}
 *
 */

} // namespace Extension
} // namespace ot

#endif // #if OPENTHREAD_ENABLE_VENDOR_EXTENSION

#endif // EXTENSION_HPP_
