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

#ifndef URI_PATHS_HPP_
#define URI_PATHS_HPP_

#include "openthread-core-config.h"

namespace ot {

/**
 *
 * This structure contains Thread URI Path string definitions.
 *
 */
struct UriPath
{
    static const char kAddressQuery[];           ///< The URI Path for Address Query ("a/aq").
    static const char kAddressNotify[];          ///< The URI Path for Address Notify ("a/an").
    static const char kAddressError[];           ///< The URI Path for Address Error ("a/ae").
    static const char kAddressRelease[];         ///< The URI Path for Address Release ("a/ar").
    static const char kAddressSolicit[];         ///< The URI Path for Address Solicit ("a/as").
    static const char kAnycastLocate[];          ///< The URI Path for Anycast Locate ("a/yl")
    static const char kActiveGet[];              ///< The URI Path for MGMT_ACTIVE_GE ("c/ag")T
    static const char kActiveSet[];              ///< The URI Path for MGMT_ACTIVE_SET ("c/as").
    static const char kDatasetChanged[];         ///< The URI Path for MGMT_DATASET_CHANGED ("c/dc").
    static const char kEnergyScan[];             ///< The URI Path for Energy Scan ("c/es").
    static const char kEnergyReport[];           ///< The URI Path for Energy Report ("c/er").
    static const char kPendingGet[];             ///< The URI Path for MGMT_PENDING_GET ("c/pg").
    static const char kPendingSet[];             ///< The URI Path for MGMT_PENDING_SET ("c/ps").
    static const char kServerData[];             ///< The URI Path for Server Data Registration ("a/sd").
    static const char kAnnounceBegin[];          ///< The URI Path for Announce Begin ("c/ab").
    static const char kProxyRx[];                ///< The URI Path for Proxy RX ("c/ur").
    static const char kProxyTx[];                ///< The URI Path for Proxy TX ("c/ut").
    static const char kRelayRx[];                ///< The URI Path for Relay RX ("c/rx").
    static const char kRelayTx[];                ///< The URI Path for Relay TX ("c/tx").
    static const char kJoinerFinalize[];         ///< The URI Path for Joiner Finalize ("c/jf").
    static const char kJoinerEntrust[];          ///< The URI Path for Joiner Entrust ("c/je").
    static const char kLeaderPetition[];         ///< The URI Path for Leader Petition ("c/lp").
    static const char kLeaderKeepAlive[];        ///< The URI Path for Leader Keep Alive ("c/la").
    static const char kPanIdConflict[];          ///< The URI Path for PAN ID Conflict ("c/pc").
    static const char kPanIdQuery[];             ///< The URI Path for PAN ID Query ("c/pq").
    static const char kCommissionerGet[];        ///< The URI Path for MGMT_COMMISSIONER_GET ("c/cg").
    static const char kCommissionerKeepAlive[];  ///< The URI Path for Commissioner Keep Alive ("c/ca").
    static const char kCommissionerPetition[];   ///< The URI Path for Commissioner Petition ("c/cp").
    static const char kCommissionerSet[];        ///< The URI Path for MGMT_COMMISSIONER_SET ("c/cs").
    static const char kDiagnosticGetRequest[];   ///< The URI Path for Network Diagnostic Get Request ("d/dg").
    static const char kDiagnosticGetQuery[];     ///< The URI Path for Network Diagnostic Get Query ("d/dq").
    static const char kDiagnosticGetAnswer[];    ///< The URI Path for Network Diagnostic Get Answer ("d/da").
    static const char kDiagnosticReset[];        ///< The URI Path for Network Diagnostic Reset ("d/dr").
    static const char kMlr[];                    ///< The URI Path for Multicast Listener Registration ("n/mr").
    static const char kDuaRegistrationRequest[]; ///< The URI Path for DUA Registration Request ("n/dr").
    static const char kDuaRegistrationNotify[];  ///< The URI Path for DUA Registration Notification ("n/dn").
    static const char kBackboneQuery[];          ///< The URI Path for Backbone Query ("b/bq").
    static const char kBackboneAnswer[];         ///< The URI Path for Backbone Answer / Backbone Notification ("b/ba").
    static const char kBackboneMlr[];            ///< The URI Path for Backbone Multicast Listener Report ("b/bmr").
};

} // namespace ot

#endif // URI_PATHS_HPP_
