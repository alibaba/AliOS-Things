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
 *   This file includes definitions for Thread URIs.
 */

#include "uri_paths.hpp"

namespace ot {

const char UriPath::kAddressQuery[]           = "a/aq";
const char UriPath::kAddressNotify[]          = "a/an";
const char UriPath::kAddressError[]           = "a/ae";
const char UriPath::kAddressRelease[]         = "a/ar";
const char UriPath::kAddressSolicit[]         = "a/as";
const char UriPath::kAnycastLocate[]          = "a/yl";
const char UriPath::kActiveGet[]              = "c/ag";
const char UriPath::kActiveSet[]              = "c/as";
const char UriPath::kDatasetChanged[]         = "c/dc";
const char UriPath::kEnergyScan[]             = "c/es";
const char UriPath::kEnergyReport[]           = "c/er";
const char UriPath::kPendingGet[]             = "c/pg";
const char UriPath::kPendingSet[]             = "c/ps";
const char UriPath::kServerData[]             = "a/sd";
const char UriPath::kAnnounceBegin[]          = "c/ab";
const char UriPath::kProxyRx[]                = "c/ur";
const char UriPath::kProxyTx[]                = "c/ut";
const char UriPath::kRelayRx[]                = "c/rx";
const char UriPath::kRelayTx[]                = "c/tx";
const char UriPath::kJoinerFinalize[]         = "c/jf";
const char UriPath::kJoinerEntrust[]          = "c/je";
const char UriPath::kLeaderPetition[]         = "c/lp";
const char UriPath::kLeaderKeepAlive[]        = "c/la";
const char UriPath::kPanIdConflict[]          = "c/pc";
const char UriPath::kPanIdQuery[]             = "c/pq";
const char UriPath::kCommissionerGet[]        = "c/cg";
const char UriPath::kCommissionerKeepAlive[]  = "c/ca";
const char UriPath::kCommissionerPetition[]   = "c/cp";
const char UriPath::kCommissionerSet[]        = "c/cs";
const char UriPath::kDiagnosticGetRequest[]   = "d/dg";
const char UriPath::kDiagnosticGetQuery[]     = "d/dq";
const char UriPath::kDiagnosticGetAnswer[]    = "d/da";
const char UriPath::kDiagnosticReset[]        = "d/dr";
const char UriPath::kMlr[]                    = "n/mr";
const char UriPath::kDuaRegistrationRequest[] = "n/dr";
const char UriPath::kDuaRegistrationNotify[]  = "n/dn";
const char UriPath::kBackboneQuery[]          = "b/bq";
const char UriPath::kBackboneAnswer[]         = "b/ba";
const char UriPath::kBackboneMlr[]            = "b/bmr";

} // namespace ot
