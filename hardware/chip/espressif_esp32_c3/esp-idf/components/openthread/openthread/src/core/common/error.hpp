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
 *   This file defines the errors used by OpenThread core.
 */

#ifndef ERROR_HPP_
#define ERROR_HPP_

#include "openthread-core-config.h"

#include <openthread/error.h>

#include <stdint.h>

namespace ot {

/**
 * This type represents error codes used by OpenThread core modules.
 *
 */
typedef otError Error;

/*
 * The `OT_ERROR_*` enumeration values are re-defined using `kError` style format.
 * See `openthread/error.h` for more details about each error.
 *
 */
constexpr Error kErrorNone                       = OT_ERROR_NONE;
constexpr Error kErrorFailed                     = OT_ERROR_FAILED;
constexpr Error kErrorDrop                       = OT_ERROR_DROP;
constexpr Error kErrorNoBufs                     = OT_ERROR_NO_BUFS;
constexpr Error kErrorNoRoute                    = OT_ERROR_NO_ROUTE;
constexpr Error kErrorBusy                       = OT_ERROR_BUSY;
constexpr Error kErrorParse                      = OT_ERROR_PARSE;
constexpr Error kErrorInvalidArgs                = OT_ERROR_INVALID_ARGS;
constexpr Error kErrorSecurity                   = OT_ERROR_SECURITY;
constexpr Error kErrorAddressQuery               = OT_ERROR_ADDRESS_QUERY;
constexpr Error kErrorNoAddress                  = OT_ERROR_NO_ADDRESS;
constexpr Error kErrorAbort                      = OT_ERROR_ABORT;
constexpr Error kErrorNotImplemented             = OT_ERROR_NOT_IMPLEMENTED;
constexpr Error kErrorInvalidState               = OT_ERROR_INVALID_STATE;
constexpr Error kErrorNoAck                      = OT_ERROR_NO_ACK;
constexpr Error kErrorChannelAccessFailure       = OT_ERROR_CHANNEL_ACCESS_FAILURE;
constexpr Error kErrorDetached                   = OT_ERROR_DETACHED;
constexpr Error kErrorFcs                        = OT_ERROR_FCS;
constexpr Error kErrorNoFrameReceived            = OT_ERROR_NO_FRAME_RECEIVED;
constexpr Error kErrorUnknownNeighbor            = OT_ERROR_UNKNOWN_NEIGHBOR;
constexpr Error kErrorInvalidSourceAddress       = OT_ERROR_INVALID_SOURCE_ADDRESS;
constexpr Error kErrorAddressFiltered            = OT_ERROR_ADDRESS_FILTERED;
constexpr Error kErrorDestinationAddressFiltered = OT_ERROR_DESTINATION_ADDRESS_FILTERED;
constexpr Error kErrorNotFound                   = OT_ERROR_NOT_FOUND;
constexpr Error kErrorAlready                    = OT_ERROR_ALREADY;
constexpr Error kErrorIp6AddressCreationFailure  = OT_ERROR_IP6_ADDRESS_CREATION_FAILURE;
constexpr Error kErrorNotCapable                 = OT_ERROR_NOT_CAPABLE;
constexpr Error kErrorResponseTimeout            = OT_ERROR_RESPONSE_TIMEOUT;
constexpr Error kErrorDuplicated                 = OT_ERROR_DUPLICATED;
constexpr Error kErrorReassemblyTimeout          = OT_ERROR_REASSEMBLY_TIMEOUT;
constexpr Error kErrorNotTmf                     = OT_ERROR_NOT_TMF;
constexpr Error kErrorNotLowpanDataFrame         = OT_ERROR_NOT_LOWPAN_DATA_FRAME;
constexpr Error kErrorLinkMarginLow              = OT_ERROR_LINK_MARGIN_LOW;
constexpr Error kErrorInvalidCommand             = OT_ERROR_INVALID_COMMAND;
constexpr Error kErrorPending                    = OT_ERROR_PENDING;
constexpr Error kErrorRejected                   = OT_ERROR_REJECTED;
constexpr Error kErrorGeneric                    = OT_ERROR_GENERIC;

constexpr uint8_t kNumErrors = OT_NUM_ERRORS;

/**
 * This function converts an `Error` into a string.
 *
 * @param[in]  aError     An error.
 *
 * @returns  A string representation of @p aError.
 *
 */
const char *ErrorToString(Error aError);

} // namespace ot

#endif // ERROR_HPP_
