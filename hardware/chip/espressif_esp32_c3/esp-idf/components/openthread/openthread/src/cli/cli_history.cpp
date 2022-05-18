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
 *   This file implements CLI for the History Tracker.
 */

#include "cli_history.hpp"

#if OPENTHREAD_CONFIG_HISTORY_TRACKER_ENABLE

#include <string.h>

#include "cli/cli.hpp"

namespace ot {
namespace Cli {

constexpr History::Command History::sCommands[];

otError History::ProcessHelp(Arg aArgs[])
{
    OT_UNUSED_VARIABLE(aArgs);

    for (const Command &command : sCommands)
    {
        OutputLine(command.mName);
    }

    return OT_ERROR_NONE;
}

otError History::ParseArgs(Arg aArgs[], bool &aIsList, uint16_t &aNumEntries) const
{
    if (*aArgs == "list")
    {
        aArgs++;
        aIsList = true;
    }
    else
    {
        aIsList = false;
    }

    if (aArgs->ParseAsUint16(aNumEntries) == OT_ERROR_NONE)
    {
        aArgs++;
    }
    else
    {
        aNumEntries = 0;
    }

    return aArgs[0].IsEmpty() ? OT_ERROR_NONE : OT_ERROR_INVALID_ARGS;
}

otError History::ProcessIpAddr(Arg aArgs[])
{
    static const char *const kEventStrings[] = {
        "Added",  // (0) OT_HISTORY_TRACKER_ADDRESS_EVENT_ADDED
        "Removed" // (1) OT_HISTORY_TRACKER_ADDRESS_EVENT_REMOVED
    };

    otError                                   error;
    bool                                      isList;
    uint16_t                                  numEntries;
    otHistoryTrackerIterator                  iterator;
    const otHistoryTrackerUnicastAddressInfo *info;
    uint32_t                                  entryAge;
    char                                      ageString[OT_HISTORY_TRACKER_ENTRY_AGE_STRING_SIZE];
    char                                      addressString[OT_IP6_ADDRESS_STRING_SIZE + 4];

    static_assert(0 == OT_HISTORY_TRACKER_ADDRESS_EVENT_ADDED, "ADDRESS_EVENT_ADDED is incorrect");
    static_assert(1 == OT_HISTORY_TRACKER_ADDRESS_EVENT_REMOVED, "ADDRESS_EVENT_REMOVED is incorrect");

    SuccessOrExit(error = ParseArgs(aArgs, isList, numEntries));

    if (!isList)
    {
        // | Age                  | Event   | Address / PrefixLen                  /123   | Origin |Scope| P | V | R |
        // +----------------------+---------+---------------------------------------------+--------+-----+---+---+---+

        static const char *const kUnicastAddrInfoTitles[] = {
            "Age", "Event", "Address / PrefixLength", "Origin", "Scope", "P", "V", "R"};

        static const uint8_t kUnicastAddrInfoColumnWidths[] = {22, 9, 45, 8, 5, 3, 3, 3};

        OutputTableHeader(kUnicastAddrInfoTitles, kUnicastAddrInfoColumnWidths);
    }

    otHistoryTrackerInitIterator(&iterator);

    for (uint16_t index = 0; (numEntries == 0) || (index < numEntries); index++)
    {
        info = otHistoryTrackerIterateUnicastAddressHistory(GetInstancePtr(), &iterator, &entryAge);
        VerifyOrExit(info != nullptr);

        otHistoryTrackerEntryAgeToString(entryAge, ageString, sizeof(ageString));
        otIp6AddressToString(&info->mAddress, addressString, sizeof(addressString));

        if (!isList)
        {
            sprintf(&addressString[strlen(addressString)], "/%d", info->mPrefixLength);

            OutputLine("| %20s | %-7s | %-43s | %-6s | %3d | %c | %c | %c |", ageString, kEventStrings[info->mEvent],
                       addressString, AddressOriginToString(info->mAddressOrigin), info->mScope,
                       info->mPreferred ? 'Y' : 'N', info->mValid ? 'Y' : 'N', info->mRloc ? 'Y' : 'N');
        }
        else
        {
            OutputLine("%s -> event:%s address:%s prefixlen:%d origin:%s scope:%d preferred:%s valid:%s rloc:%s",
                       ageString, kEventStrings[info->mEvent], addressString, info->mPrefixLength,
                       AddressOriginToString(info->mAddressOrigin), info->mScope, info->mPreferred ? "yes" : "no",
                       info->mValid ? "yes" : "no", info->mRloc ? "yes" : "no");
        }
    }

exit:
    return error;
}

otError History::ProcessIpMulticastAddr(Arg aArgs[])
{
    static const char *const kEventStrings[] = {
        "Subscribed",  // (0) OT_HISTORY_TRACKER_ADDRESS_EVENT_ADDED
        "Unsubscribed" // (1) OT_HISTORY_TRACKER_ADDRESS_EVENT_REMOVED
    };

    otError                                     error;
    bool                                        isList;
    uint16_t                                    numEntries;
    otHistoryTrackerIterator                    iterator;
    const otHistoryTrackerMulticastAddressInfo *info;
    uint32_t                                    entryAge;
    char                                        ageString[OT_HISTORY_TRACKER_ENTRY_AGE_STRING_SIZE];
    char                                        addressString[OT_IP6_ADDRESS_STRING_SIZE];

    static_assert(0 == OT_HISTORY_TRACKER_ADDRESS_EVENT_ADDED, "ADDRESS_EVENT_ADDED is incorrect");
    static_assert(1 == OT_HISTORY_TRACKER_ADDRESS_EVENT_REMOVED, "ADDRESS_EVENT_REMOVED is incorrect");

    SuccessOrExit(error = ParseArgs(aArgs, isList, numEntries));

    if (!isList)
    {
        // | Age                  | Event        | Multicast Address                       | Origin |
        // +----------------------+--------------+-----------------------------------------+--------+

        static const char *const kMulticastAddrInfoTitles[] = {
            "Age",
            "Event",
            "Multicast Address",
            "Origin",
        };

        static const uint8_t kMulticastAddrInfoColumnWidths[] = {22, 14, 42, 8};

        OutputTableHeader(kMulticastAddrInfoTitles, kMulticastAddrInfoColumnWidths);
    }

    otHistoryTrackerInitIterator(&iterator);

    for (uint16_t index = 0; (numEntries == 0) || (index < numEntries); index++)
    {
        info = otHistoryTrackerIterateMulticastAddressHistory(GetInstancePtr(), &iterator, &entryAge);
        VerifyOrExit(info != nullptr);

        otHistoryTrackerEntryAgeToString(entryAge, ageString, sizeof(ageString));
        otIp6AddressToString(&info->mAddress, addressString, sizeof(addressString));

        OutputLine(isList ? "%s -> event:%s address:%s origin:%s" : "| %20s | %-12s | %-39s | %-6s |", ageString,
                   kEventStrings[info->mEvent], addressString, AddressOriginToString(info->mAddressOrigin));
    }

exit:
    return error;
}

otError History::ProcessNeighbor(Arg aArgs[])
{
    static const char *const kEventString[] = {
        /* (0) OT_HISTORY_TRACKER_NEIGHBOR_EVENT_ADDED     -> */ "Added",
        /* (1) OT_HISTORY_TRACKER_NEIGHBOR_EVENT_REMOVED   -> */ "Removed",
        /* (2) OT_HISTORY_TRACKER_NEIGHBOR_EVENT_CHANGED   -> */ "Changed",
        /* (3) OT_HISTORY_TRACKER_NEIGHBOR_EVENT_RESTORING -> */ "Restoring",
    };

    otError                             error;
    bool                                isList;
    uint16_t                            numEntries;
    otHistoryTrackerIterator            iterator;
    const otHistoryTrackerNeighborInfo *info;
    uint32_t                            entryAge;
    char                                ageString[OT_HISTORY_TRACKER_ENTRY_AGE_STRING_SIZE];
    otLinkModeConfig                    mode;
    char                                linkModeString[Interpreter::kLinkModeStringSize];

    static_assert(0 == OT_HISTORY_TRACKER_NEIGHBOR_EVENT_ADDED, "NEIGHBOR_EVENT_ADDED value is incorrect");
    static_assert(1 == OT_HISTORY_TRACKER_NEIGHBOR_EVENT_REMOVED, "NEIGHBOR_EVENT_REMOVED value is incorrect");
    static_assert(2 == OT_HISTORY_TRACKER_NEIGHBOR_EVENT_CHANGED, "NEIGHBOR_EVENT_CHANGED value is incorrect");
    static_assert(3 == OT_HISTORY_TRACKER_NEIGHBOR_EVENT_RESTORING, "NEIGHBOR_EVENT_RESTORING value is incorrect");

    SuccessOrExit(error = ParseArgs(aArgs, isList, numEntries));

    if (!isList)
    {
        // | Age                  | Type   | Event     | Extended Address | RLOC16 | Mode | Ave RSS |
        // +----------------------+--------+-----------+------------------+--------+------+---------+

        static const char *const kNeighborInfoTitles[] = {
            "Age", "Type", "Event", "Extended Address", "RLOC16", "Mode", "Ave RSS",
        };

        static const uint8_t kNeighborInfoColumnWidths[] = {22, 8, 11, 18, 8, 6, 9};

        OutputTableHeader(kNeighborInfoTitles, kNeighborInfoColumnWidths);
    }

    otHistoryTrackerInitIterator(&iterator);

    for (uint16_t index = 0; (numEntries == 0) || (index < numEntries); index++)
    {
        info = otHistoryTrackerIterateNeighborHistory(GetInstancePtr(), &iterator, &entryAge);
        VerifyOrExit(info != nullptr);

        otHistoryTrackerEntryAgeToString(entryAge, ageString, sizeof(ageString));

        mode.mRxOnWhenIdle = info->mRxOnWhenIdle;
        mode.mDeviceType   = info->mFullThreadDevice;
        mode.mNetworkData  = info->mFullNetworkData;
        Interpreter::LinkModeToString(mode, linkModeString);

        OutputFormat(isList ? "%s -> type:%s event:%s extaddr:" : "| %20s | %-6s | %-9s | ", ageString,
                     info->mIsChild ? "Child" : "Router", kEventString[info->mEvent]);
        OutputExtAddress(info->mExtAddress);
        OutputLine(isList ? " rloc16:0x%04x mode:%s rss:%d" : " | 0x%04x | %-4s | %7d |", info->mRloc16, linkModeString,
                   info->mAverageRssi);
    }

exit:
    return error;
}

otError History::ProcessNetInfo(Arg aArgs[])
{
    otError                            error;
    bool                               isList;
    uint16_t                           numEntries;
    otHistoryTrackerIterator           iterator;
    const otHistoryTrackerNetworkInfo *info;
    uint32_t                           entryAge;
    char                               ageString[OT_HISTORY_TRACKER_ENTRY_AGE_STRING_SIZE];
    char                               linkModeString[Interpreter::kLinkModeStringSize];

    SuccessOrExit(error = ParseArgs(aArgs, isList, numEntries));

    if (!isList)
    {
        // | Age                  | Role     | Mode | RLOC16 | Partition ID |
        // +----------------------+----------+------+--------+--------------+

        static const char *const kNetInfoTitles[]       = {"Age", "Role", "Mode", "RLOC16", "Partition ID"};
        static const uint8_t     kNetInfoColumnWidths[] = {22, 10, 6, 8, 14};

        OutputTableHeader(kNetInfoTitles, kNetInfoColumnWidths);
    }

    otHistoryTrackerInitIterator(&iterator);

    for (uint16_t index = 0; (numEntries == 0) || (index < numEntries); index++)
    {
        info = otHistoryTrackerIterateNetInfoHistory(GetInstancePtr(), &iterator, &entryAge);
        VerifyOrExit(info != nullptr);

        otHistoryTrackerEntryAgeToString(entryAge, ageString, sizeof(ageString));

        OutputLine(isList ? "%s -> role:%s mode:%s rloc16:0x%04x partition-id:%u"
                          : "| %20s | %-8s | %-4s | 0x%04x | %12u |",
                   ageString, otThreadDeviceRoleToString(info->mRole),
                   Interpreter::LinkModeToString(info->mMode, linkModeString), info->mRloc16, info->mPartitionId);
    }

exit:
    return error;
}

otError History::ProcessRx(Arg aArgs[])
{
    return ProcessRxTxHistory(kRx, aArgs);
}

otError History::ProcessRxTx(Arg aArgs[])
{
    return ProcessRxTxHistory(kRxTx, aArgs);
}

otError History::ProcessTx(Arg aArgs[])
{
    return ProcessRxTxHistory(kTx, aArgs);
}

const char *History::AddressOriginToString(uint8_t aOrigin)
{
    const char *str = "Unknown";

    switch (aOrigin)
    {
    case OT_ADDRESS_ORIGIN_THREAD:
        str = "Thread";
        break;

    case OT_ADDRESS_ORIGIN_SLAAC:
        str = "SLAAC";
        break;

    case OT_ADDRESS_ORIGIN_DHCPV6:
        str = "DHCPv6";
        break;

    case OT_ADDRESS_ORIGIN_MANUAL:
        str = "Manual";
        break;

    default:
        break;
    }

    return str;
}

const char *History::MessagePriorityToString(uint8_t aPriority)
{
    const char *str = "unkn";

    switch (aPriority)
    {
    case OT_HISTORY_TRACKER_MSG_PRIORITY_LOW:
        str = "low";
        break;

    case OT_HISTORY_TRACKER_MSG_PRIORITY_NORMAL:
        str = "norm";
        break;

    case OT_HISTORY_TRACKER_MSG_PRIORITY_HIGH:
        str = "high";
        break;

    case OT_HISTORY_TRACKER_MSG_PRIORITY_NET:
        str = "net";
        break;

    default:
        break;
    }

    return str;
}

const char *History::RadioTypeToString(const otHistoryTrackerMessageInfo &aInfo)
{
    const char *str = "none";

    if (aInfo.mRadioTrelUdp6 && aInfo.mRadioIeee802154)
    {
        str = "all";
    }
    else if (aInfo.mRadioIeee802154)
    {
        str = "15.4";
    }
    else if (aInfo.mRadioTrelUdp6)
    {
        str = "trel";
    }

    return str;
}

const char *History::MessageTypeToString(const otHistoryTrackerMessageInfo &aInfo)
{
    const char *str = otIp6ProtoToString(aInfo.mIpProto);

    if (aInfo.mIpProto == OT_IP6_PROTO_ICMP6)
    {
        switch (aInfo.mIcmp6Type)
        {
        case OT_ICMP6_TYPE_DST_UNREACH:
            str = "ICMP6(Unreach)";
            break;
        case OT_ICMP6_TYPE_PACKET_TO_BIG:
            str = "ICMP6(TooBig)";
            break;
        case OT_ICMP6_TYPE_ECHO_REQUEST:
            str = "ICMP6(EchoReqst)";
            break;
        case OT_ICMP6_TYPE_ECHO_REPLY:
            str = "ICMP6(EchoReply)";
            break;
        case OT_ICMP6_TYPE_ROUTER_SOLICIT:
            str = "ICMP6(RouterSol)";
            break;
        case OT_ICMP6_TYPE_ROUTER_ADVERT:
            str = "ICMP6(RouterAdv)";
            break;
        default:
            str = "ICMP6(Other)";
            break;
        }
    }

    return str;
}

otError History::ProcessRxTxHistory(RxTx aRxTx, Arg aArgs[])
{
    otError                            error;
    bool                               isList;
    uint16_t                           numEntries;
    otHistoryTrackerIterator           rxIterator;
    otHistoryTrackerIterator           txIterator;
    bool                               isRx   = false;
    const otHistoryTrackerMessageInfo *info   = nullptr;
    const otHistoryTrackerMessageInfo *rxInfo = nullptr;
    const otHistoryTrackerMessageInfo *txInfo = nullptr;
    uint32_t                           entryAge;
    uint32_t                           rxEntryAge;
    uint32_t                           txEntryAge;

    // | Age                  | Type             | Len   | Chksum | Sec | Prio | RSS  |Dir | Neighb | Radio |
    // +----------------------+------------------+-------+--------+-----+------+------+----+--------+-------+

    static const char *const kTableTitles[] = {"Age",  "Type", "Len", "Chksum", "Sec",
                                               "Prio", "RSS",  "Dir", "Neighb", "Radio"};

    static const uint8_t kTableColumnWidths[] = {22, 18, 7, 8, 5, 6, 6, 4, 8, 7};

    SuccessOrExit(error = ParseArgs(aArgs, isList, numEntries));

    if (!isList)
    {
        OutputTableHeader(kTableTitles, kTableColumnWidths);
    }

    otHistoryTrackerInitIterator(&txIterator);
    otHistoryTrackerInitIterator(&rxIterator);

    for (uint16_t index = 0; (numEntries == 0) || (index < numEntries); index++)
    {
        switch (aRxTx)
        {
        case kRx:
            info = otHistoryTrackerIterateRxHistory(GetInstancePtr(), &rxIterator, &entryAge);
            isRx = true;
            break;

        case kTx:
            info = otHistoryTrackerIterateTxHistory(GetInstancePtr(), &txIterator, &entryAge);
            isRx = false;
            break;

        case kRxTx:
            // Iterate through both RX and TX lists and determine the entry
            // with earlier age.

            if (rxInfo == nullptr)
            {
                rxInfo = otHistoryTrackerIterateRxHistory(GetInstancePtr(), &rxIterator, &rxEntryAge);
            }

            if (txInfo == nullptr)
            {
                txInfo = otHistoryTrackerIterateTxHistory(GetInstancePtr(), &txIterator, &txEntryAge);
            }

            if ((rxInfo != nullptr) && ((txInfo == nullptr) || (rxEntryAge <= txEntryAge)))
            {
                info     = rxInfo;
                entryAge = rxEntryAge;
                isRx     = true;
                rxInfo   = nullptr;
            }
            else
            {
                info     = txInfo;
                entryAge = txEntryAge;
                isRx     = false;
                txInfo   = nullptr;
            }

            break;
        }

        VerifyOrExit(info != nullptr);

        if (isList)
        {
            OutputRxTxEntryListFormat(*info, entryAge, isRx);
        }
        else
        {
            if (index != 0)
            {
                OutputTableSeparator(kTableColumnWidths);
            }

            OutputRxTxEntryTableFormat(*info, entryAge, isRx);
        }
    }

exit:
    return error;
}

void History::OutputRxTxEntryListFormat(const otHistoryTrackerMessageInfo &aInfo, uint32_t aEntryAge, bool aIsRx)
{
    constexpr uint8_t kIndentSize = 4;

    char ageString[OT_HISTORY_TRACKER_ENTRY_AGE_STRING_SIZE];
    char addrString[OT_IP6_SOCK_ADDR_STRING_SIZE];

    otHistoryTrackerEntryAgeToString(aEntryAge, ageString, sizeof(ageString));

    OutputLine("%s", ageString);
    OutputFormat(kIndentSize, "type:%s len:%u cheksum:0x%04x sec:%s prio:%s ", MessageTypeToString(aInfo),
                 aInfo.mPayloadLength, aInfo.mChecksum, aInfo.mLinkSecurity ? "yes" : "no",
                 MessagePriorityToString(aInfo.mPriority));
    if (aIsRx)
    {
        OutputFormat("rss:%d", aInfo.mAveRxRss);
    }
    else
    {
        OutputFormat("tx-success:%s", aInfo.mTxSuccess ? "yes" : "no");
    }

    OutputLine(" %s:0x%04x radio:%s", aIsRx ? "from" : "to", aInfo.mNeighborRloc16, RadioTypeToString(aInfo));

    otIp6SockAddrToString(&aInfo.mSource, addrString, sizeof(addrString));
    OutputLine(kIndentSize, "src:%s", addrString);

    otIp6SockAddrToString(&aInfo.mDestination, addrString, sizeof(addrString));
    OutputLine(kIndentSize, "dst:%s", addrString);
}

void History::OutputRxTxEntryTableFormat(const otHistoryTrackerMessageInfo &aInfo, uint32_t aEntryAge, bool aIsRx)
{
    char ageString[OT_HISTORY_TRACKER_ENTRY_AGE_STRING_SIZE];
    char addrString[OT_IP6_SOCK_ADDR_STRING_SIZE];

    otHistoryTrackerEntryAgeToString(aEntryAge, ageString, sizeof(ageString));

    OutputFormat("| %20s | %-16.16s | %5u | 0x%04x | %3s | %4s | ", "", MessageTypeToString(aInfo),
                 aInfo.mPayloadLength, aInfo.mChecksum, aInfo.mLinkSecurity ? "yes" : "no",
                 MessagePriorityToString(aInfo.mPriority));

    if (aIsRx)
    {
        OutputFormat("%4d | RX ", aInfo.mAveRxRss);
    }
    else
    {
        OutputFormat(" NA  |");
        OutputFormat(aInfo.mTxSuccess ? " TX " : "TX-F");
    }

    if (aInfo.mNeighborRloc16 == kShortAddrBroadcast)
    {
        OutputFormat("| bcast  ");
    }
    else if (aInfo.mNeighborRloc16 == kShortAddrInvalid)
    {
        OutputFormat("| unknwn ");
    }
    else
    {
        OutputFormat("| 0x%04x ", aInfo.mNeighborRloc16);
    }

    OutputLine("| %5.5s |", RadioTypeToString(aInfo));

    otIp6SockAddrToString(&aInfo.mSource, addrString, sizeof(addrString));
    OutputLine("| %20s | src: %-70s |", ageString, addrString);

    otIp6SockAddrToString(&aInfo.mDestination, addrString, sizeof(addrString));
    OutputLine("| %20s | dst: %-70s |", "", addrString);
}

otError History::Process(Arg aArgs[])
{
    otError        error = OT_ERROR_INVALID_COMMAND;
    const Command *command;

    if (aArgs[0].IsEmpty())
    {
        IgnoreError(ProcessHelp(aArgs));
        ExitNow();
    }

    command = Utils::LookupTable::Find(aArgs[0].GetCString(), sCommands);
    VerifyOrExit(command != nullptr);

    error = (this->*command->mHandler)(aArgs + 1);

exit:
    return error;
}

} // namespace Cli
} // namespace ot

#endif // OPENTHREAD_CONFIG_SRP_CLIENT_ENABLE
