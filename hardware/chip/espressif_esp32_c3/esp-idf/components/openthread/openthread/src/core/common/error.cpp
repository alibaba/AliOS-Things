/*
 *  Copyright (c) 2016-2021, The OpenThread Authors.
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
 *   This file implements the error code functions used by OpenThread core modules.
 */

#include "error.hpp"

#include "common/code_utils.hpp"

namespace ot {

const char *ErrorToString(Error aError)
{
    static const char *const kErrorStrings[kNumErrors] = {
        "OK",                         // (0)  kErrorNone
        "Failed",                     // (1)  kErrorFailed
        "Drop",                       // (2)  kErrorDrop
        "NoBufs",                     // (3)  kErrorNoBufs
        "NoRoute",                    // (4)  kErrorNoRoute
        "Busy",                       // (5)  kErrorBusy
        "Parse",                      // (6)  kErrorParse
        "InvalidArgs",                // (7)  kErrorInvalidArgs
        "Security",                   // (8)  kErrorSecurity
        "AddressQuery",               // (9)  kErrorAddressQuery
        "NoAddress",                  // (10) kErrorNoAddress
        "Abort",                      // (11) kErrorAbort
        "NotImplemented",             // (12) kErrorNotImplemented
        "InvalidState",               // (13) kErrorInvalidState
        "NoAck",                      // (14) kErrorNoAck
        "ChannelAccessFailure",       // (15) kErrorChannelAccessFailure
        "Detached",                   // (16) kErrorDetached
        "FcsErr",                     // (17) kErrorFcs
        "NoFrameReceived",            // (18) kErrorNoFrameReceived
        "UnknownNeighbor",            // (19) kErrorUnknownNeighbor
        "InvalidSourceAddress",       // (20) kErrorInvalidSourceAddress
        "AddressFiltered",            // (21) kErrorAddressFiltered
        "DestinationAddressFiltered", // (22) kErrorDestinationAddressFiltered
        "NotFound",                   // (23) kErrorNotFound
        "Already",                    // (24) kErrorAlready
        "ReservedError25",            // (25) Error 25 is reserved
        "Ipv6AddressCreationFailure", // (26) kErrorIp6AddressCreationFailure
        "NotCapable",                 // (27) kErrorNotCapable
        "ResponseTimeout",            // (28) kErrorResponseTimeout
        "Duplicated",                 // (29) kErrorDuplicated
        "ReassemblyTimeout",          // (30) kErrorReassemblyTimeout
        "NotTmf",                     // (31) kErrorNotTmf
        "NonLowpanDataFrame",         // (32) kErrorNotLowpanDataFrame
        "ReservedError33",            // (33) Error 33 is reserved
        "LinkMarginLow",              // (34) kErrorLinkMarginLow
        "InvalidCommand",             // (35) kErrorInvalidCommand
        "Pending",                    // (36) kErrorPending
        "Rejected",                   // (37) kErrorRejected
    };

    return aError < OT_ARRAY_LENGTH(kErrorStrings) ? kErrorStrings[aError] : "UnknownErrorType";
}

} // namespace ot
