/*
 *    Copyright (c) 2016-2017, The OpenThread Authors.
 *    All rights reserved.
 *
 *    Redistribution and use in source and binary forms, with or without
 *    modification, are permitted provided that the following conditions are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *    3. Neither the name of the copyright holder nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 *    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
 *    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file shows how to implement the NCP vendor hook.
 */

#if OPENTHREAD_ENABLE_NCP_VENDOR_HOOK

#include "ncp_base.hpp"

namespace ot {
namespace Ncp {

otError NcpBase::VendorCommandHandler(uint8_t aHeader, unsigned int aCommand)
{
    otError error = OT_ERROR_NONE;

    switch (aCommand)
    {
        // TODO: Implement your command handlers here.

    default:
        error = PrepareLastStatusResponse(aHeader, SPINEL_STATUS_INVALID_COMMAND);
    }

    return error;
}

void NcpBase::VendorHandleFrameRemovedFromNcpBuffer(Spinel::Buffer::FrameTag aFrameTag)
{
    // This method is a callback which mirrors `NcpBase::HandleFrameRemovedFromNcpBuffer()`.
    // It is called when a spinel frame is sent and removed from NCP buffer.
    //
    // (a) This can be used to track and verify that a vendor spinel frame response is
    //     delivered to the host (tracking the frame using its tag).
    //
    // (b) It indicates that NCP buffer space is now available (since a spinel frame is
    //     removed). This can be used to implement reliability mechanisms to re-send
    //     a failed spinel command response (or an async spinel frame) transmission
    //     (failed earlier due to NCP buffer being full).

    OT_UNUSED_VARIABLE(aFrameTag);
}

otError NcpBase::VendorGetPropertyHandler(spinel_prop_key_t aPropKey)
{
    otError error = OT_ERROR_NONE;

    switch (aPropKey)
    {
        // TODO: Implement your property get handlers here.
        //
        // Get handler should retrieve the property value and then encode and write the
        // value into the NCP buffer. If the "get" operation itself fails, handler should
        // write a `LAST_STATUS` with the error status into the NCP buffer. `OT_ERROR_NO_BUFS`
        // should be returned if NCP buffer is full and response cannot be written.

    default:
        error = OT_ERROR_NOT_FOUND;
        break;
    }

    return error;
}

otError NcpBase::VendorSetPropertyHandler(spinel_prop_key_t aPropKey)
{
    otError error = OT_ERROR_NONE;

    switch (aPropKey)
    {
        // TODO: Implement your property set handlers here.
        //
        // Set handler should first decode the value from the input Spinel frame and then
        // perform the corresponding set operation. The handler should not prepare the
        // spinel response and therefore should not write anything to the NCP buffer.
        // The error returned from handler (other than `OT_ERROR_NOT_FOUND`) indicates the
        // error in either parsing of the input or the error of the set operation. In case
        // of a successful "set", `NcpBase` set command handler will invoke the
        // `VendorGetPropertyHandler()` for the same property key to prepare the response.

    default:
        error = OT_ERROR_NOT_FOUND;
        break;
    }

    return error;
}

} // namespace Ncp
} // namespace ot

//-------------------------------------------------------------------------------------------------------------------
// When OPENTHREAD_ENABLE_NCP_VENDOR_HOOK is enabled, vendor code is
// expected to provide the `otAppNcpInit()` function. The reason behind
// this is to enable vendor code to define its own sub-class of
// `NcpBase` or `NcpHdlc`/`NcpSpi`.
//
// Example below show how to add a vendor sub-class over `NcpHdlc`.

#include "ncp_hdlc.hpp"
#include "common/new.hpp"

class NcpVendorUart : public ot::Ncp::NcpHdlc
{
    static int SendHdlc(const uint8_t *aBuf, uint16_t aBufLength)
    {
        OT_UNUSED_VARIABLE(aBuf);
        OT_UNUSED_VARIABLE(aBufLength);
        return 0;
    }

public:
    NcpVendorUart(ot::Instance *aInstance)
        : ot::Ncp::NcpHdlc(aInstance, &NcpVendorUart::SendHdlc)
    {
    }

    // Add public/private methods or member variables
};

static OT_DEFINE_ALIGNED_VAR(sNcpVendorRaw, sizeof(NcpVendorUart), uint64_t);

extern "C" void otAppNcpInit(otInstance *aInstance)
{
    NcpVendorUart *ncpVendor = nullptr;
    ot::Instance * instance  = static_cast<ot::Instance *>(aInstance);

    ncpVendor = new (&sNcpVendorRaw) NcpVendorUart(instance);

    if (ncpVendor == nullptr || ncpVendor != ot::Ncp::NcpBase::GetNcpInstance())
    {
        // assert(false);
    }
}

#endif // #if OPENTHREAD_ENABLE_NCP_VENDOR_HOOK
