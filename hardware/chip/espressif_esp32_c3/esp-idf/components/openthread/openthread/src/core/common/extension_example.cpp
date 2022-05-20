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
 *   This file provides an example on how to implement an OpenThread vendor extension.
 */

#include "openthread-core-config.h"

#include <stdbool.h>
#include <stdint.h>

#include "common/code_utils.hpp"
#include "common/extension.hpp"
#include "common/new.hpp"

namespace ot {
namespace Extension {

/**
 * This class defines the vendor extension object.
 *
 */
class Extension : public ExtensionBase
{
public:
    explicit Extension(Instance &aInstance)
        : ExtensionBase(aInstance)
    {
    }

    // TODO: Add vendor extension code (add methods and/or member variables).
};

// ----------------------------------------------------------------------------
// `ExtensionBase` API
// ----------------------------------------------------------------------------

static OT_DEFINE_ALIGNED_VAR(sExtensionRaw, sizeof(Extension), uint64_t);

ExtensionBase &ExtensionBase::Init(Instance &aInstance)
{
    ExtensionBase *ext = reinterpret_cast<ExtensionBase *>(&sExtensionRaw);

    VerifyOrExit(!ext->mIsInitialized);

    ext = new (&sExtensionRaw) Extension(aInstance);

exit:
    return *ext;
}

void ExtensionBase::SignalInstanceInit(void)
{
    // OpenThread instance is initialized and ready.

    // TODO: Implement vendor extension code here and start interaction with OpenThread instance.
}

void ExtensionBase::SignalNcpInit(Ncp::NcpBase &aNcpBase)
{
    // NCP instance is initialized and ready.

    // TODO: Implement vendor extension code here and start interaction with NCP instance.

    OT_UNUSED_VARIABLE(aNcpBase);
}

void ExtensionBase::HandleNotifierEvents(Events aEvents)
{
    // TODO: Implement vendor extension code here to handle notifier events.

    OT_UNUSED_VARIABLE(aEvents);
}

} // namespace Extension
} // namespace ot
