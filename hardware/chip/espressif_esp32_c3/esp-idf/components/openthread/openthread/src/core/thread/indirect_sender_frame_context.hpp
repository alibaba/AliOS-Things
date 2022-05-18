/*
 *  Copyright (c) 2019, The OpenThread Authors.
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
 *   This file includes definitions of frame context used for indirect transmission.
 */

#ifndef INDIRECT_SENDER_FRAME_CONTETX_HPP_
#define INDIRECT_SENDER_FRAME_CONTETX_HPP_

#include "openthread-core-config.h"

#include <stdint.h>

namespace ot {

/**
 * @addtogroup core-mesh-forwarding
 *
 * @brief
 *   This module includes definitions frame context used by indirect sender.
 *
 * @{
 */

/**
 * This class defines the `FrameContext` type.
 *
 * This is the base class for `IndirectSender`.
 *
 */
class IndirectSenderBase
{
public:
    /**
     * This type defines the frame context used by `IndirectSender`.
     *
     * This type specifies all the info that `IndirectSender` requires to be saved along with a prepared frame for
     * indirect transmission.  `IndirectSender` is designed to contain the common code for handling of indirect
     * transmission to sleepy children and be able to interface to different lower-layer implementation of
     * `DataPollHandler`. While the `FrameContext` is defined by the `IndirectSender` itself, the lower-layer
     * (`DataPollHandler`) is expected to provide the buffer/object for context to be stored (it is provided from the
     * lower-layer callback asking for a frame to be prepared). This model allows different implementations of
     * `DataPollHandler` to adopt different strategies on how to save the context.
     *
     */
    struct FrameContext
    {
        friend class IndirectSender;
        friend class CslTxScheduler;

    private:
        uint16_t mMessageNextOffset; ///< The next offset into the message associated with the prepared frame.
    };
};

/**
 * @}
 *
 */

} // namespace ot

#endif // INDIRECT_SENDER_FRAME_CONTETX_HPP_
