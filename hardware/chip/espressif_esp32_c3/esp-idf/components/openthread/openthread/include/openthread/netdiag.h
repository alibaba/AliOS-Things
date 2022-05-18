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
 * @brief
 *  This file defines the OpenThread Network Diagnostic API.
 */

#ifndef OPENTHREAD_NETDIAG_H_
#define OPENTHREAD_NETDIAG_H_

#include <openthread/ip6.h>
#include <openthread/thread.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup api-thread-general
 *
 * @{
 *
 */

/**
 * Maximum Number of Network Diagnostic TLV Types to Request or Reset.
 */
#define OT_NETWORK_DIAGNOSTIC_TYPELIST_MAX_ENTRIES 19

/**
 * Size of Network Diagnostic Child Table entry.
 */
#define OT_NETWORK_DIAGNOSTIC_CHILD_TABLE_ENTRY_SIZE 3

/**
 * Initializer for otNetworkDiagIterator.
 */
#define OT_NETWORK_DIAGNOSTIC_ITERATOR_INIT 0

enum
{
    OT_NETWORK_DIAGNOSTIC_TLV_EXT_ADDRESS       = 0,  ///< MAC Extended Address TLV
    OT_NETWORK_DIAGNOSTIC_TLV_SHORT_ADDRESS     = 1,  ///< Address16 TLV
    OT_NETWORK_DIAGNOSTIC_TLV_MODE              = 2,  ///< Mode TLV
    OT_NETWORK_DIAGNOSTIC_TLV_TIMEOUT           = 3,  ///< Timeout TLV (the maximum polling time period for SEDs)
    OT_NETWORK_DIAGNOSTIC_TLV_CONNECTIVITY      = 4,  ///< Connectivity TLV
    OT_NETWORK_DIAGNOSTIC_TLV_ROUTE             = 5,  ///< Route64 TLV
    OT_NETWORK_DIAGNOSTIC_TLV_LEADER_DATA       = 6,  ///< Leader Data TLV
    OT_NETWORK_DIAGNOSTIC_TLV_NETWORK_DATA      = 7,  ///< Network Data TLV
    OT_NETWORK_DIAGNOSTIC_TLV_IP6_ADDR_LIST     = 8,  ///< IPv6 Address List TLV
    OT_NETWORK_DIAGNOSTIC_TLV_MAC_COUNTERS      = 9,  ///< MAC Counters TLV
    OT_NETWORK_DIAGNOSTIC_TLV_BATTERY_LEVEL     = 14, ///< Battery Level TLV
    OT_NETWORK_DIAGNOSTIC_TLV_SUPPLY_VOLTAGE    = 15, ///< Supply Voltage TLV
    OT_NETWORK_DIAGNOSTIC_TLV_CHILD_TABLE       = 16, ///< Child Table TLV
    OT_NETWORK_DIAGNOSTIC_TLV_CHANNEL_PAGES     = 17, ///< Channel Pages TLV
    OT_NETWORK_DIAGNOSTIC_TLV_TYPE_LIST         = 18, ///< Type List TLV
    OT_NETWORK_DIAGNOSTIC_TLV_MAX_CHILD_TIMEOUT = 19, ///< Max Child Timeout TLV
};

typedef uint16_t otNetworkDiagIterator; ///< Used to iterate through Network Diagnostic TLV.

/**
 * This structure represents a Network Diagnostic Connectivity value.
 *
 */
typedef struct otNetworkDiagConnectivity
{
    /**
     * The priority of the sender as a parent.
     */
    int8_t mParentPriority;

    /**
     * The number of neighboring devices with which the sender shares a link of quality 3.
     */
    uint8_t mLinkQuality3;

    /**
     * The number of neighboring devices with which the sender shares a link of quality 2.
     */
    uint8_t mLinkQuality2;

    /**
     * The number of neighboring devices with which the sender shares a link of quality 1.
     */
    uint8_t mLinkQuality1;

    /**
     * The sender's routing cost to the Leader.
     */
    uint8_t mLeaderCost;

    /**
     * The most recent ID sequence number received by the sender.
     */
    uint8_t mIdSequence;

    /**
     * The number of active Routers in the sender's Thread Network Partition.
     */
    uint8_t mActiveRouters;

    /**
     * The guaranteed buffer capacity in octets for all IPv6 datagrams destined to a given SED. Optional.
     */
    uint16_t mSedBufferSize;

    /**
     * The guaranteed queue capacity in number of IPv6 datagrams destined to a given SED. Optional.
     */
    uint8_t mSedDatagramCount;
} otNetworkDiagConnectivity;

/**
 * This structure represents a Network Diagnostic Route data.
 *
 */
typedef struct otNetworkDiagRouteData
{
    uint8_t mRouterId;           ///< The Assigned Router ID.
    uint8_t mLinkQualityOut : 2; ///< Link Quality Out.
    uint8_t mLinkQualityIn : 2;  ///< Link Quality In.
    uint8_t mRouteCost : 4;      ///< Routing Cost. Infinite routing cost is represented by value 0.
} otNetworkDiagRouteData;

/**
 * This structure represents a Network Diagnostic Route TLV value.
 *
 */
typedef struct otNetworkDiagRoute
{
    /**
     * The sequence number associated with the set of Router ID assignments in #mRouteData.
     */
    uint8_t mIdSequence;

    /**
     * Number of elements in #mRouteData.
     */
    uint8_t mRouteCount;

    /**
     * Link Quality and Routing Cost data.
     */
    otNetworkDiagRouteData mRouteData[OT_NETWORK_MAX_ROUTER_ID + 1];
} otNetworkDiagRoute;

/**
 * This structure represents a Network Diagnostic Mac Counters value.
 *
 * See <a href="https://www.ietf.org/rfc/rfc2863">RFC 2863</a> for definitions of member fields.
 *
 */
typedef struct otNetworkDiagMacCounters
{
    uint32_t mIfInUnknownProtos;
    uint32_t mIfInErrors;
    uint32_t mIfOutErrors;
    uint32_t mIfInUcastPkts;
    uint32_t mIfInBroadcastPkts;
    uint32_t mIfInDiscards;
    uint32_t mIfOutUcastPkts;
    uint32_t mIfOutBroadcastPkts;
    uint32_t mIfOutDiscards;
} otNetworkDiagMacCounters;

/**
 * This structure represents a Network Diagnostic Child Table Entry.
 *
 */
typedef struct otNetworkDiagChildEntry
{
    /**
     * Expected poll time expressed as 2^(Timeout-4) seconds.
     */
    uint16_t mTimeout : 5;

    /**
     * Child ID from which an RLOC can be generated.
     */
    uint16_t mChildId : 9;

    /**
     * Link mode bits.
     */
    otLinkModeConfig mMode;
} otNetworkDiagChildEntry;

/**
 * This structure represents a Network Diagnostic TLV.
 *
 */
typedef struct otNetworkDiagTlv
{
    /**
     * The Network Diagnostic TLV type.
     */
    uint8_t mType;

    union
    {
        otExtAddress              mExtAddress;
        uint16_t                  mAddr16;
        otLinkModeConfig          mMode;
        uint32_t                  mTimeout;
        otNetworkDiagConnectivity mConnectivity;
        otNetworkDiagRoute        mRoute;
        otLeaderData              mLeaderData;
        otNetworkDiagMacCounters  mMacCounters;
        uint8_t                   mBatteryLevel;
        uint16_t                  mSupplyVoltage;
        uint32_t                  mMaxChildTimeout;
        struct
        {
            uint8_t mCount;
            uint8_t m8[OT_NETWORK_BASE_TLV_MAX_LENGTH];
        } mNetworkData;
        struct
        {
            uint8_t      mCount;
            otIp6Address mList[OT_NETWORK_BASE_TLV_MAX_LENGTH / OT_IP6_ADDRESS_SIZE];
        } mIp6AddrList;
        struct
        {
            uint8_t mCount;
            otNetworkDiagChildEntry
                mTable[OT_NETWORK_BASE_TLV_MAX_LENGTH / OT_NETWORK_DIAGNOSTIC_CHILD_TABLE_ENTRY_SIZE];
        } mChildTable;
        struct
        {
            uint8_t mCount;
            uint8_t m8[OT_NETWORK_BASE_TLV_MAX_LENGTH];
        } mChannelPages;
    } mData;
} otNetworkDiagTlv;

/**
 * This function gets the next Network Diagnostic TLV in the message.
 *
 * @param[in]     aMessage         A pointer to a message.
 * @param[inout]  aIterator        A pointer to the Network Diagnostic iterator context. To get the first
 *                                 Network Diagnostic TLV it should be set to OT_NETWORK_DIAGNOSTIC_ITERATOR_INIT.
 * @param[out]    aNetworkDiagTlv  A pointer to where the Network Diagnostic TLV information will be placed.
 *
 * @retval OT_ERROR_NONE       Successfully found the next Network Diagnostic TLV.
 * @retval OT_ERROR_NOT_FOUND  No subsequent Network Diagnostic TLV exists in the message.
 * @retval OT_ERROR_PARSE      Parsing the next Network Diagnostic failed.
 *
 * @Note A subsequent call to this function is allowed only when current return value is OT_ERROR_NONE.
 *
 */
otError otThreadGetNextDiagnosticTlv(const otMessage *      aMessage,
                                     otNetworkDiagIterator *aIterator,
                                     otNetworkDiagTlv *     aNetworkDiagTlv);

/**
 * This function pointer is called when Network Diagnostic Get response is received.
 *
 * @param[in]  aError        The error when failed to get the response.
 * @param[in]  aMessage      A pointer to the message buffer containing the received Network Diagnostic
 *                           Get response payload. Available only when @p aError is `OT_ERROR_NONE`.
 * @param[in]  aMessageInfo  A pointer to the message info for @p aMessage. Available only when
 *                           @p aError is `OT_ERROR_NONE`.
 * @param[in]  aContext      A pointer to application-specific context.
 *
 */
typedef void (*otReceiveDiagnosticGetCallback)(otError              aError,
                                               otMessage *          aMessage,
                                               const otMessageInfo *aMessageInfo,
                                               void *               aContext);

/**
 * Send a Network Diagnostic Get request.
 *
 * @param[in]  aInstance         A pointer to an OpenThread instance.
 * @param[in]  aDestination      A pointer to destination address.
 * @param[in]  aTlvTypes         An array of Network Diagnostic TLV types.
 * @param[in]  aCount            Number of types in aTlvTypes.
 * @param[in]  aCallback         A pointer to a function that is called when Network Diagnostic Get response
 *                               is received or NULL to disable the callback.
 * @param[in]  aCallbackContext  A pointer to application-specific context.
 *
 * @retval OT_ERROR_NONE    Successfully queued the DIAG_GET.req.
 * @retval OT_ERROR_NO_BUFS Insufficient message buffers available to send DIAG_GET.req.
 *
 */
otError otThreadSendDiagnosticGet(otInstance *                   aInstance,
                                  const otIp6Address *           aDestination,
                                  const uint8_t                  aTlvTypes[],
                                  uint8_t                        aCount,
                                  otReceiveDiagnosticGetCallback aCallback,
                                  void *                         aCallbackContext);

/**
 * Send a Network Diagnostic Reset request.
 *
 * @param[in]  aInstance      A pointer to an OpenThread instance.
 * @param[in]  aDestination   A pointer to destination address.
 * @param[in]  aTlvTypes      An array of Network Diagnostic TLV types. Currently only Type 9 is allowed.
 * @param[in]  aCount         Number of types in aTlvTypes
 *
 * @retval OT_ERROR_NONE    Successfully queued the DIAG_RST.ntf.
 * @retval OT_ERROR_NO_BUFS Insufficient message buffers available to send DIAG_RST.ntf.
 *
 */
otError otThreadSendDiagnosticReset(otInstance *        aInstance,
                                    const otIp6Address *aDestination,
                                    const uint8_t       aTlvTypes[],
                                    uint8_t             aCount);

/**
 * @}
 *
 */

#ifdef __cplusplus
} // extern "C"
#endif

#endif // OPENTHREAD_NETDIAG_H_
