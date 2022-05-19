/*
 *  Copyright (c) 2016, The OpenThread Authors.
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
 *   This file contains definitions for the CLI interpreter.
 */

#ifndef CLI_HPP_
#define CLI_HPP_

#include "openthread-core-config.h"

#include "cli_config.h"

#include <stdarg.h>

#include <openthread/cli.h>
#include <openthread/dataset.h>
#include <openthread/dns_client.h>
#include <openthread/instance.h>
#include <openthread/ip6.h>
#include <openthread/link.h>
#include <openthread/ping_sender.h>
#include <openthread/sntp.h>
#if OPENTHREAD_CONFIG_TCP_ENABLE && OPENTHREAD_CONFIG_CLI_TCP_ENABLE
#include <openthread/tcp.h>
#endif
#include <openthread/thread.h>
#include <openthread/thread_ftd.h>
#include <openthread/udp.h>

#include "cli/cli_commissioner.hpp"
#include "cli/cli_dataset.hpp"
#include "cli/cli_history.hpp"
#include "cli/cli_joiner.hpp"
#include "cli/cli_network_data.hpp"
#include "cli/cli_output.hpp"
#include "cli/cli_srp_client.hpp"
#include "cli/cli_srp_server.hpp"
#include "cli/cli_tcp.hpp"
#include "cli/cli_udp.hpp"
#if OPENTHREAD_CONFIG_COAP_API_ENABLE
#include "cli/cli_coap.hpp"
#endif
#if OPENTHREAD_CONFIG_COAP_SECURE_API_ENABLE
#include "cli/cli_coap_secure.hpp"
#endif

#include "common/code_utils.hpp"
#include "common/debug.hpp"
#include "common/instance.hpp"
#include "common/type_traits.hpp"
#include "utils/lookup_table.hpp"
#include "utils/parse_cmdline.hpp"

namespace ot {

/**
 * @namespace ot::Cli
 *
 * @brief
 *   This namespace contains definitions for the CLI interpreter.
 *
 */
namespace Cli {

extern "C" void otCliPlatLogv(otLogLevel, otLogRegion, const char *, va_list);
extern "C" void otCliPlatLogLine(otLogLevel, otLogRegion, const char *);
extern "C" void otCliAppendResult(otError aError);
extern "C" void otCliOutputBytes(const uint8_t *aBytes, uint8_t aLength);
extern "C" void otCliOutputFormat(const char *aFmt, ...);

/**
 * This class implements the CLI interpreter.
 *
 */
class Interpreter : public Output
{
#if OPENTHREAD_FTD || OPENTHREAD_MTD
    friend class Commissioner;
    friend class Joiner;
    friend class NetworkData;
    friend class SrpClient;
#endif
    friend void otCliPlatLogv(otLogLevel, otLogRegion, const char *, va_list);
    friend void otCliPlatLogLine(otLogLevel, otLogRegion, const char *);
    friend void otCliAppendResult(otError aError);
    friend void otCliOutputBytes(const uint8_t *aBytes, uint8_t aLength);
    friend void otCliOutputFormat(const char *aFmt, ...);

public:
    typedef Utils::CmdLineParser::Arg Arg;

    /**
     * Constructor
     *
     * @param[in]  aInstance    The OpenThread instance structure.
     * @param[in]  aCallback    A callback method called to process CLI output.
     * @param[in]  aContext     A user context pointer.
     */
    explicit Interpreter(Instance *aInstance, otCliOutputCallback aCallback, void *aContext);

    /**
     * This method returns a reference to the interpreter object.
     *
     * @returns A reference to the interpreter object.
     *
     */
    static Interpreter &GetInterpreter(void)
    {
        OT_ASSERT(sInterpreter != nullptr);

        return *sInterpreter;
    }

    /**
     * This method initializes the Console interpreter.
     *
     * @param[in]  aInstance  The OpenThread instance structure.
     * @param[in]  aCallback  A pointer to a callback method.
     * @param[in]  aContext   A pointer to a user context.
     *
     */
    static void Initialize(otInstance *aInstance, otCliOutputCallback aCallback, void *aContext);

    /**
     * This method returns whether the interpreter is initialized.
     *
     * @returns  Whether the interpreter is initialized.
     *
     */
    static bool IsInitialized(void) { return sInterpreter != nullptr; }

    /**
     * This method interprets a CLI command.
     *
     * @param[in]  aBuf        A pointer to a string.
     *
     */
    void ProcessLine(char *aBuf);

    /**
     * This static method checks a given argument string against "enable" or "disable" commands.
     *
     * @param[in]  aArgs    The argument string to parse.
     * @param[out] aEnable  Boolean variable to return outcome on success.
     *                      Set to TRUE for "enable" command, and FALSE for "disable" command.
     *
     * @retval OT_ERROR_NONE             Successfully parsed the @p aString and updated @p aEnable.
     * @retval OT_ERROR_INVALID_COMMAND  The @p aString is not "enable" or "disable" command.
     *
     */
    static otError ParseEnableOrDisable(const Arg &aArg, bool &aEnable);

    /**
     * This method sets the user command table.
     *
     * @param[in]  aUserCommands  A pointer to an array with user commands.
     * @param[in]  aLength        @p aUserCommands length.
     * @param[in]  aContext       @p aUserCommands length.
     *
     */
    void SetUserCommands(const otCliCommand *aCommands, uint8_t aLength, void *aContext);

    static constexpr uint8_t kLinkModeStringSize = sizeof("rdn"); ///< Size of string buffer for a MLE Link Mode.

    /**
     * This method converts a given MLE Link Mode to flag string.
     *
     * The characters 'r', 'd', and 'n' are respectively used for `mRxOnWhenIdle`, `mDeviceType` and `mNetworkData`
     * flags. If all flags are `false`, then "-" is returned.
     *
     * @param[in]  aLinkMode       The MLE Link Mode to convert.
     * @param[out] aStringBuffer   A reference to an string array to place the string.
     *
     * @returns A pointer @p aStringBuffer which contains the converted string.
     *
     */
    static const char *LinkModeToString(const otLinkModeConfig &aLinkMode, char (&aStringBuffer)[kLinkModeStringSize]);

protected:
    static Interpreter *sInterpreter;

private:
    enum
    {
        kIndentSize       = 4,
        kMaxArgs          = 32,
        kMaxAutoAddresses = 8,
        kMaxLineLength    = OPENTHREAD_CONFIG_CLI_MAX_LINE_LENGTH,
    };

    static constexpr uint32_t kNetworkDiagnosticTimeoutMsecs = 5000;
    static constexpr uint32_t kLocateTimeoutMsecs            = 2500;

    struct Command
    {
        const char *mName;
        otError (Interpreter::*mHandler)(Arg aArgs[]);
    };

    template <typename ValueType> using GetHandler         = ValueType (&)(otInstance *);
    template <typename ValueType> using SetHandler         = void (&)(otInstance *, ValueType);
    template <typename ValueType> using SetHandlerFailable = otError (&)(otInstance *, ValueType);

    // Returns format string to output a `ValueType` (e.g., "%u" for `uint16_t`).
    template <typename ValueType> static constexpr const char *FormatStringFor(void);

    template <typename ValueType> otError ProcessGet(Arg aArgs[], GetHandler<ValueType> aGetHandler)
    {
        static_assert(
            TypeTraits::IsSame<ValueType, uint8_t>::kValue || TypeTraits::IsSame<ValueType, uint16_t>::kValue ||
                TypeTraits::IsSame<ValueType, uint32_t>::kValue || TypeTraits::IsSame<ValueType, int8_t>::kValue ||
                TypeTraits::IsSame<ValueType, int16_t>::kValue || TypeTraits::IsSame<ValueType, int32_t>::kValue,
            "ValueType must be an  8, 16, or 32 bit `int` or `uint` type");

        otError error = OT_ERROR_NONE;

        VerifyOrExit(aArgs[0].IsEmpty(), error = OT_ERROR_INVALID_ARGS);
        OutputLine(FormatStringFor<ValueType>(), aGetHandler(GetInstancePtr()));

    exit:
        return error;
    }

    template <typename ValueType> otError ProcessSet(Arg aArgs[], SetHandler<ValueType> aSetHandler)
    {
        otError   error;
        ValueType value;

        SuccessOrExit(error = aArgs[0].ParseAs<ValueType>(value));
        VerifyOrExit(aArgs[1].IsEmpty(), error = OT_ERROR_INVALID_ARGS);

        aSetHandler(GetInstancePtr(), value);

    exit:
        return error;
    }

    template <typename ValueType> otError ProcessSet(Arg aArgs[], SetHandlerFailable<ValueType> aSetHandler)
    {
        otError   error;
        ValueType value;

        SuccessOrExit(error = aArgs[0].ParseAs<ValueType>(value));
        VerifyOrExit(aArgs[1].IsEmpty(), error = OT_ERROR_INVALID_ARGS);

        error = aSetHandler(GetInstancePtr(), value);

    exit:
        return error;
    }

    template <typename ValueType>
    otError ProcessGetSet(Arg aArgs[], GetHandler<ValueType> aGetHandler, SetHandler<ValueType> aSetHandler)
    {
        otError error = ProcessGet(aArgs, aGetHandler);

        VerifyOrExit(error != OT_ERROR_NONE);
        error = ProcessSet(aArgs, aSetHandler);

    exit:
        return error;
    }

    template <typename ValueType>
    otError ProcessGetSet(Arg aArgs[], GetHandler<ValueType> aGetHandler, SetHandlerFailable<ValueType> aSetHandler)
    {
        otError error = ProcessGet(aArgs, aGetHandler);

        VerifyOrExit(error != OT_ERROR_NONE);
        error = ProcessSet(aArgs, aSetHandler);

    exit:
        return error;
    }

    void OutputPrompt(void);
    void OutputResult(otError aError);

#if OPENTHREAD_CONFIG_PING_SENDER_ENABLE
    otError ParsePingInterval(const Arg &aArg, uint32_t &aInterval);
#endif
    static otError ParseJoinerDiscerner(Arg &aArg, otJoinerDiscerner &aDiscerner);
#if OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE
    static otError ParsePrefix(Arg aArgs[], otBorderRouterConfig &aConfig);
    static otError ParseRoute(Arg aArgs[], otExternalRouteConfig &aConfig);
#endif

    // Process methods on FTD/MTD/RCP
#if OPENTHREAD_CONFIG_DIAG_ENABLE
    otError ProcessDiag(Arg aArgs[]);
#endif
    otError ProcessHelp(Arg aArgs[]);
    otError ProcessHistory(Arg aArgs[]);
    otError ProcessReset(Arg aArgs[]);
    otError ProcessUserCommands(Arg aArgs[]);
    otError ProcessVersion(Arg aArgs[]);

    // Process methods only on FTD/MTD
#if OPENTHREAD_FTD || OPENTHREAD_MTD
    otError ProcessCcaThreshold(Arg aArgs[]);
    otError ProcessBufferInfo(Arg aArgs[]);
    otError ProcessChannel(Arg aArgs[]);
#if OPENTHREAD_CONFIG_BORDER_AGENT_ENABLE
    otError ProcessBorderAgent(Arg aArgs[]);
#endif
#if OPENTHREAD_CONFIG_BORDER_ROUTING_ENABLE
    otError ProcessBorderRouting(Arg aArgs[]);
#endif
#if (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)
    otError ProcessBackboneRouter(Arg aArgs[]);

#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE
    otError ProcessBackboneRouterLocal(Arg aArgs[]);
#if OPENTHREAD_CONFIG_BACKBONE_ROUTER_MULTICAST_ROUTING_ENABLE
    otError ProcessBackboneRouterMgmtMlr(Arg aArgs[]);
    void    PrintMulticastListenersTable(void);
#endif
#endif

    otError ProcessDomainName(Arg aArgs[]);

#if OPENTHREAD_CONFIG_DUA_ENABLE
    otError ProcessDua(Arg aArgs[]);
#endif

#endif // (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)

#if OPENTHREAD_FTD
    otError ProcessChild(Arg aArgs[]);
    otError ProcessChildIp(Arg aArgs[]);
    otError ProcessChildMax(Arg aArgs[]);
#endif
#if OPENTHREAD_CONFIG_CHILD_SUPERVISION_ENABLE
    otError ProcessChildSupervision(Arg aArgs[]);
#endif
    otError ProcessChildTimeout(Arg aArgs[]);
#if OPENTHREAD_CONFIG_COAP_API_ENABLE
    otError ProcessCoap(Arg aArgs[]);
#endif
#if OPENTHREAD_CONFIG_COAP_SECURE_API_ENABLE
    otError ProcessCoapSecure(Arg aArgs[]);
#endif
#if OPENTHREAD_CONFIG_PLATFORM_RADIO_COEX_ENABLE
    otError ProcessCoexMetrics(Arg aArgs[]);
#endif
#if OPENTHREAD_CONFIG_COMMISSIONER_ENABLE && OPENTHREAD_FTD
    otError ProcessCommissioner(Arg aArgs[]);
#endif
#if OPENTHREAD_FTD
    otError ProcessContextIdReuseDelay(Arg aArgs[]);
#endif
    otError ProcessCounters(Arg aArgs[]);
    otError ProcessCsl(Arg aArgs[]);
#if OPENTHREAD_FTD
    otError ProcessDelayTimerMin(Arg aArgs[]);
#endif
    otError ProcessDiscover(Arg aArgs[]);
    otError ProcessDns(Arg aArgs[]);
#if OPENTHREAD_FTD
    void    OutputEidCacheEntry(const otCacheEntryInfo &aEntry);
    otError ProcessEidCache(Arg aArgs[]);
#endif
    otError ProcessEui64(Arg aArgs[]);
#if OPENTHREAD_POSIX && !defined(FUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION)
    otError ProcessExit(Arg aArgs[]);
#endif
    otError ProcessLog(Arg aArgs[]);
    otError ProcessExtAddress(Arg aArgs[]);
    otError ProcessExtPanId(Arg aArgs[]);
    otError ProcessFactoryReset(Arg aArgs[]);
#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
    otError ProcessFake(Arg aArgs[]);
#endif
    otError ProcessFem(Arg aArgs[]);
    otError ProcessIfconfig(Arg aArgs[]);
    otError ProcessIpAddr(Arg aArgs[]);
    otError ProcessIpAddrAdd(Arg aArgs[]);
    otError ProcessIpAddrDel(Arg aArgs[]);
    otError ProcessIpMulticastAddr(Arg aArgs[]);
    otError ProcessIpMulticastAddrAdd(Arg aArgs[]);
    otError ProcessIpMulticastAddrDel(Arg aArgs[]);
    otError ProcessMulticastPromiscuous(Arg aArgs[]);
#if OPENTHREAD_CONFIG_JOINER_ENABLE
    otError ProcessJoiner(Arg aArgs[]);
#endif
#if OPENTHREAD_FTD
    otError ProcessJoinerPort(Arg aArgs[]);
#endif
    otError ProcessKeySequence(Arg aArgs[]);
    otError ProcessLeaderData(Arg aArgs[]);
#if OPENTHREAD_FTD
    otError ProcessPartitionId(Arg aArgs[]);
    otError ProcessLeaderWeight(Arg aArgs[]);
#endif
#if OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
    otError ProcessMlIid(Arg aArgs[]);
#endif
#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_INITIATOR_ENABLE
    otError ProcessLinkMetrics(Arg aArgs[]);
    otError ProcessLinkMetricsQuery(Arg aArgs[]);
    otError ProcessLinkMetricsMgmt(Arg aArgs[]);
    otError ProcessLinkMetricsProbe(Arg aArgs[]);
    otError ParseLinkMetricsFlags(otLinkMetrics &aLinkMetrics, const Arg &aFlags);
#endif
#if OPENTHREAD_CONFIG_TMF_ANYCAST_LOCATOR_ENABLE
    otError     ProcessLocate(Arg aArgs[]);
    static void HandleLocateResult(void *              aContext,
                                   otError             aError,
                                   const otIp6Address *aMeshLocalAddress,
                                   uint16_t            aRloc16);
    void        HandleLocateResult(otError aError, const otIp6Address *aMeshLocalAddress, uint16_t aRloc16);
#endif
#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_TMF_PROXY_MLR_ENABLE && OPENTHREAD_CONFIG_COMMISSIONER_ENABLE
    otError ProcessMlr(Arg aArgs[]);

    otError ProcessMlrReg(Arg aArgs[]);

    static void HandleMlrRegResult(void *              aContext,
                                   otError             aError,
                                   uint8_t             aMlrStatus,
                                   const otIp6Address *aFailedAddresses,
                                   uint8_t             aFailedAddressNum);
    void        HandleMlrRegResult(otError             aError,
                                   uint8_t             aMlrStatus,
                                   const otIp6Address *aFailedAddresses,
                                   uint8_t             aFailedAddressNum);
#endif
    otError ProcessMode(Arg aArgs[]);
    otError ProcessMultiRadio(Arg aArgs[]);
#if OPENTHREAD_CONFIG_MULTI_RADIO
    void OutputMultiRadioInfo(const otMultiRadioNeighborInfo &aMultiRadioInfo);
#endif
#if OPENTHREAD_FTD
    otError ProcessNeighbor(Arg aArgs[]);
#endif
    otError ProcessNetworkData(Arg aArgs[]);
    otError ProcessNetworkDataPrefix(void);
    otError ProcessNetworkDataRoute(void);
    otError ProcessNetworkDataService(void);

    otError ProcessNetstat(Arg aArgs[]);
#if OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE
    otError ProcessService(Arg aArgs[]);
    otError ProcessServiceList(void);
#endif
#if OPENTHREAD_FTD || OPENTHREAD_CONFIG_TMF_NETWORK_DIAG_MTD_ENABLE
    otError ProcessNetworkDiagnostic(Arg aArgs[]);
#endif
#if OPENTHREAD_FTD
    otError ProcessNetworkIdTimeout(Arg aArgs[]);
#endif
    otError ProcessNetworkKey(Arg aArgs[]);
#if OPENTHREAD_CONFIG_PLATFORM_KEY_REFERENCES_ENABLE
    otError ProcessNetworkKeyRef(Arg aArgs[]);
#endif
    otError ProcessNetworkName(Arg aArgs[]);
#if OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
    otError ProcessNetworkTime(Arg aArgs[]);
#endif
    otError ProcessPanId(Arg aArgs[]);
    otError ProcessParent(Arg aArgs[]);
#if OPENTHREAD_FTD
    otError ProcessParentPriority(Arg aArgs[]);
#endif
#if OPENTHREAD_CONFIG_PING_SENDER_ENABLE
    otError ProcessPing(Arg aArgs[]);
#endif
    otError ProcessPollPeriod(Arg aArgs[]);
#if OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE
    otError ProcessPrefix(Arg aArgs[]);
    otError ProcessPrefixAdd(Arg aArgs[]);
    otError ProcessPrefixRemove(Arg aArgs[]);
    otError ProcessPrefixList(void);
#endif
    otError ProcessPromiscuous(Arg aArgs[]);
#if OPENTHREAD_FTD
    otError ProcessPreferRouterId(Arg aArgs[]);
    otError ProcessPskc(Arg aArgs[]);
#if OPENTHREAD_CONFIG_PLATFORM_KEY_REFERENCES_ENABLE
    otError ProcessPskcRef(Arg aArgs[]);
#endif
#endif
    otError ProcessRcp(Arg aArgs[]);
    otError ProcessRegion(Arg aArgs[]);
#if OPENTHREAD_FTD
    otError ProcessReleaseRouterId(Arg aArgs[]);
#endif
#if OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE
    otError ProcessRoute(Arg aArgs[]);
    otError ProcessRouteAdd(Arg aArgs[]);
    otError ProcessRouteRemove(Arg aArgs[]);
    otError ProcessRouteList(void);
#endif
#if OPENTHREAD_FTD
    otError ProcessRouter(Arg aArgs[]);
    otError ProcessRouterDowngradeThreshold(Arg aArgs[]);
    otError ProcessRouterEligible(Arg aArgs[]);
    otError ProcessRouterSelectionJitter(Arg aArgs[]);
    otError ProcessRouterUpgradeThreshold(Arg aArgs[]);
#endif
    otError ProcessRloc16(Arg aArgs[]);
    otError ProcessScan(Arg aArgs[]);
    otError ProcessSingleton(Arg aArgs[]);
#if OPENTHREAD_CONFIG_SNTP_CLIENT_ENABLE
    otError ProcessSntp(Arg aArgs[]);
#endif
#if OPENTHREAD_CONFIG_SRP_CLIENT_ENABLE || OPENTHREAD_CONFIG_SRP_SERVER_ENABLE
    otError ProcessSrp(Arg aArgs[]);
#endif
    otError ProcessState(Arg aArgs[]);
    otError ProcessThread(Arg aArgs[]);
    otError ProcessDataset(Arg aArgs[]);
    otError ProcessTxPower(Arg aArgs[]);
#if OPENTHREAD_CONFIG_TCP_ENABLE && OPENTHREAD_CONFIG_CLI_TCP_ENABLE
    otError ProcessTcp(Arg aArgs[]);
#endif
    otError ProcessUdp(Arg aArgs[]);
    otError ProcessUnsecurePort(Arg aArgs[]);
#if OPENTHREAD_CONFIG_UPTIME_ENABLE
    otError ProcessUptime(Arg aArgs[]);
#endif
#if OPENTHREAD_CONFIG_MAC_FILTER_ENABLE
    otError ProcessMacFilter(Arg aArgs[]);
    void    PrintMacFilter(void);
    otError ProcessMacFilterAddress(Arg aArgs[]);
    otError ProcessMacFilterRss(Arg aArgs[]);
#endif
    otError ProcessMac(Arg aArgs[]);
    otError ProcessMacRetries(Arg aArgs[]);
#if OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
    otError ProcessMacSend(Arg aArgs[]);
#endif
#if OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE
    otError ProcessTrel(Arg aArgs[]);
#endif

#if OPENTHREAD_CONFIG_PING_SENDER_ENABLE
    static void HandlePingReply(const otPingSenderReply *aReply, void *aContext);
    static void HandlePingStatistics(const otPingSenderStatistics *aStatistics, void *aContext);
#endif
    void        OutputScanTableHeader(void);
    static void HandleActiveScanResult(otActiveScanResult *aResult, void *aContext);
    static void HandleEnergyScanResult(otEnergyScanResult *aResult, void *aContext);
    static void HandleLinkPcapReceive(const otRadioFrame *aFrame, bool aIsTx, void *aContext);

#if OPENTHREAD_FTD || (OPENTHREAD_MTD && OPENTHREAD_CONFIG_TMF_NETWORK_DIAG_MTD_ENABLE)
    void HandleDiagnosticGetResponse(otError aError, const otMessage *aMessage, const Ip6::MessageInfo *aMessageInfo);
    static void HandleDiagnosticGetResponse(otError              aError,
                                            otMessage *          aMessage,
                                            const otMessageInfo *aMessageInfo,
                                            void *               aContext);

    void OutputMode(uint8_t aIndentSize, const otLinkModeConfig &aMode);
    void OutputConnectivity(uint8_t aIndentSize, const otNetworkDiagConnectivity &aConnectivity);
    void OutputRoute(uint8_t aIndentSize, const otNetworkDiagRoute &aRoute);
    void OutputRouteData(uint8_t aIndentSize, const otNetworkDiagRouteData &aRouteData);
    void OutputLeaderData(uint8_t aIndentSize, const otLeaderData &aLeaderData);
    void OutputNetworkDiagMacCounters(uint8_t aIndentSize, const otNetworkDiagMacCounters &aMacCounters);
    void OutputChildTableEntry(uint8_t aIndentSize, const otNetworkDiagChildEntry &aChildEntry);
#endif

#if OPENTHREAD_CONFIG_DNS_CLIENT_ENABLE
    otError     GetDnsConfig(Arg aArgs[], otDnsQueryConfig *&aConfig);
    static void HandleDnsAddressResponse(otError aError, const otDnsAddressResponse *aResponse, void *aContext);
    void        HandleDnsAddressResponse(otError aError, const otDnsAddressResponse *aResponse);
#if OPENTHREAD_CONFIG_DNS_CLIENT_SERVICE_DISCOVERY_ENABLE
    void        OutputDnsServiceInfo(uint8_t aIndentSize, const otDnsServiceInfo &aServiceInfo);
    static void HandleDnsBrowseResponse(otError aError, const otDnsBrowseResponse *aResponse, void *aContext);
    void        HandleDnsBrowseResponse(otError aError, const otDnsBrowseResponse *aResponse);
    static void HandleDnsServiceResponse(otError aError, const otDnsServiceResponse *aResponse, void *aContext);
    void        HandleDnsServiceResponse(otError aError, const otDnsServiceResponse *aResponse);
#endif
#endif

#if OPENTHREAD_CONFIG_SNTP_CLIENT_ENABLE
    static void HandleSntpResponse(void *aContext, uint64_t aTime, otError aResult);
#endif

#if OPENTHREAD_CONFIG_PING_SENDER_ENABLE
    void HandlePingReply(const otPingSenderReply *aReply);
    void HandlePingStatistics(const otPingSenderStatistics *aStatistics);
#endif
    void HandleActiveScanResult(otActiveScanResult *aResult);
    void HandleEnergyScanResult(otEnergyScanResult *aResult);
    void HandleLinkPcapReceive(const otRadioFrame *aFrame, bool aIsTx);
#if OPENTHREAD_CONFIG_SNTP_CLIENT_ENABLE
    void HandleSntpResponse(uint64_t aTime, otError aResult);
#endif
#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_INITIATOR_ENABLE
    void PrintLinkMetricsValue(const otLinkMetricsValues *aMetricsValues);

    static void HandleLinkMetricsReport(const otIp6Address *       aAddress,
                                        const otLinkMetricsValues *aMetricsValues,
                                        uint8_t                    aStatus,
                                        void *                     aContext);

    void HandleLinkMetricsReport(const otIp6Address *       aAddress,
                                 const otLinkMetricsValues *aMetricsValues,
                                 uint8_t                    aStatus);

    static void HandleLinkMetricsMgmtResponse(const otIp6Address *aAddress, uint8_t aStatus, void *aContext);

    void HandleLinkMetricsMgmtResponse(const otIp6Address *aAddress, uint8_t aStatus);

    static void HandleLinkMetricsEnhAckProbingIe(otShortAddress             aShortAddress,
                                                 const otExtAddress *       aExtAddress,
                                                 const otLinkMetricsValues *aMetricsValues,
                                                 void *                     aContext);

    void HandleLinkMetricsEnhAckProbingIe(otShortAddress             aShortAddress,
                                          const otExtAddress *       aExtAddress,
                                          const otLinkMetricsValues *aMetricsValues);

    const char *LinkMetricsStatusToStr(uint8_t aStatus);
#endif // OPENTHREAD_CONFIG_MLE_LINK_METRICS_INITIATOR_ENABLE

    static void HandleDiscoveryRequest(const otThreadDiscoveryRequestInfo *aInfo, void *aContext)
    {
        static_cast<Interpreter *>(aContext)->HandleDiscoveryRequest(*aInfo);
    }
    void HandleDiscoveryRequest(const otThreadDiscoveryRequestInfo &aInfo);

#endif // OPENTHREAD_FTD || OPENTHREAD_MTD

    void SetCommandTimeout(uint32_t aTimeoutMilli);

    static void HandleTimer(Timer &aTimer);
    void        HandleTimer(void);

    // Commands supported by radio:
    // [diag, help, reset, version]
    static constexpr Command sCommands[] = {
#if OPENTHREAD_FTD || OPENTHREAD_MTD
#if OPENTHREAD_CONFIG_BORDER_AGENT_ENABLE
        {"ba", &Interpreter::ProcessBorderAgent},
#endif
#if (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)
        {"bbr", &Interpreter::ProcessBackboneRouter},
#endif
#if OPENTHREAD_CONFIG_BORDER_ROUTING_ENABLE
        {"br", &Interpreter::ProcessBorderRouting},
#endif
        {"bufferinfo", &Interpreter::ProcessBufferInfo},
        {"ccathreshold", &Interpreter::ProcessCcaThreshold},
        {"channel", &Interpreter::ProcessChannel},
#if OPENTHREAD_FTD
        {"child", &Interpreter::ProcessChild},
        {"childip", &Interpreter::ProcessChildIp},
        {"childmax", &Interpreter::ProcessChildMax},
#endif
#if OPENTHREAD_CONFIG_CHILD_SUPERVISION_ENABLE
        {"childsupervision", &Interpreter::ProcessChildSupervision},
#endif
        {"childtimeout", &Interpreter::ProcessChildTimeout},
#if OPENTHREAD_CONFIG_COAP_API_ENABLE
        {"coap", &Interpreter::ProcessCoap},
#endif
#if OPENTHREAD_CONFIG_COAP_SECURE_API_ENABLE
        {"coaps", &Interpreter::ProcessCoapSecure},
#endif
#if OPENTHREAD_CONFIG_PLATFORM_RADIO_COEX_ENABLE
        {"coex", &Interpreter::ProcessCoexMetrics},
#endif
#if OPENTHREAD_CONFIG_COMMISSIONER_ENABLE && OPENTHREAD_FTD
        {"commissioner", &Interpreter::ProcessCommissioner},
#endif
#if OPENTHREAD_FTD
        {"contextreusedelay", &Interpreter::ProcessContextIdReuseDelay},
#endif
        {"counters", &Interpreter::ProcessCounters},
#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
        {"csl", &Interpreter::ProcessCsl},
#endif
        {"dataset", &Interpreter::ProcessDataset},
#if OPENTHREAD_FTD
        {"delaytimermin", &Interpreter::ProcessDelayTimerMin},
#endif
#endif // OPENTHREAD_FTD || OPENTHREAD_MTD
#if OPENTHREAD_CONFIG_DIAG_ENABLE
        {"diag", &Interpreter::ProcessDiag},
#endif
#if OPENTHREAD_FTD || OPENTHREAD_MTD
        {"discover", &Interpreter::ProcessDiscover},
        {"dns", &Interpreter::ProcessDns},
#if (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)
        {"domainname", &Interpreter::ProcessDomainName},
#endif
#if OPENTHREAD_CONFIG_DUA_ENABLE
        {"dua", &Interpreter::ProcessDua},
#endif
#if OPENTHREAD_FTD
        {"eidcache", &Interpreter::ProcessEidCache},
#endif
        {"eui64", &Interpreter::ProcessEui64},
        {"extaddr", &Interpreter::ProcessExtAddress},
        {"extpanid", &Interpreter::ProcessExtPanId},
        {"factoryreset", &Interpreter::ProcessFactoryReset},
#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
        {"fake", &Interpreter::ProcessFake},
#endif
        {"fem", &Interpreter::ProcessFem},
#endif // OPENTHREAD_FTD || OPENTHREAD_MTD
        {"help", &Interpreter::ProcessHelp},
#if OPENTHREAD_FTD || OPENTHREAD_MTD
#if OPENTHREAD_CONFIG_HISTORY_TRACKER_ENABLE
        {"history", &Interpreter::ProcessHistory},
#endif
        {"ifconfig", &Interpreter::ProcessIfconfig},
        {"ipaddr", &Interpreter::ProcessIpAddr},
        {"ipmaddr", &Interpreter::ProcessIpMulticastAddr},
#if OPENTHREAD_CONFIG_JOINER_ENABLE
        {"joiner", &Interpreter::ProcessJoiner},
#endif
#if OPENTHREAD_FTD
        {"joinerport", &Interpreter::ProcessJoinerPort},
#endif
        {"keysequence", &Interpreter::ProcessKeySequence},
        {"leaderdata", &Interpreter::ProcessLeaderData},
#if OPENTHREAD_FTD
        {"leaderweight", &Interpreter::ProcessLeaderWeight},
#endif
#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_INITIATOR_ENABLE
        {"linkmetrics", &Interpreter::ProcessLinkMetrics},
#endif
#if OPENTHREAD_CONFIG_TMF_ANYCAST_LOCATOR_ENABLE
        {"locate", &Interpreter::ProcessLocate},
#endif
        {"log", &Interpreter::ProcessLog},
        {"mac", &Interpreter::ProcessMac},
#if OPENTHREAD_CONFIG_MAC_FILTER_ENABLE
        {"macfilter", &Interpreter::ProcessMacFilter},
#endif
#if OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
        {"mliid", &Interpreter::ProcessMlIid},
#endif
#if (OPENTHREAD_FTD && OPENTHREAD_CONFIG_TMF_PROXY_MLR_ENABLE) && OPENTHREAD_CONFIG_COMMISSIONER_ENABLE
        {"mlr", &Interpreter::ProcessMlr},
#endif
        {"mode", &Interpreter::ProcessMode},
        {"multiradio", &Interpreter::ProcessMultiRadio},
#if OPENTHREAD_FTD
        {"neighbor", &Interpreter::ProcessNeighbor},
#endif
        {"netdata", &Interpreter::ProcessNetworkData},
        {"netstat", &Interpreter::ProcessNetstat},
#if OPENTHREAD_FTD || OPENTHREAD_CONFIG_TMF_NETWORK_DIAG_MTD_ENABLE
        {"networkdiagnostic", &Interpreter::ProcessNetworkDiagnostic},
#endif
#if OPENTHREAD_FTD
        {"networkidtimeout", &Interpreter::ProcessNetworkIdTimeout},
#endif
        {"networkkey", &Interpreter::ProcessNetworkKey},
#if OPENTHREAD_CONFIG_PLATFORM_KEY_REFERENCES_ENABLE
        {"networkkeyref", &Interpreter::ProcessNetworkKeyRef},
#endif
        {"networkname", &Interpreter::ProcessNetworkName},
#if OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
        {"networktime", &Interpreter::ProcessNetworkTime},
#endif
        {"panid", &Interpreter::ProcessPanId},
        {"parent", &Interpreter::ProcessParent},
#if OPENTHREAD_FTD
        {"parentpriority", &Interpreter::ProcessParentPriority},
        {"partitionid", &Interpreter::ProcessPartitionId},
#endif
#if OPENTHREAD_CONFIG_PING_SENDER_ENABLE
        {"ping", &Interpreter::ProcessPing},
#endif
        {"pollperiod", &Interpreter::ProcessPollPeriod},
#if OPENTHREAD_FTD
        {"preferrouterid", &Interpreter::ProcessPreferRouterId},
#endif
#if OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE
        {"prefix", &Interpreter::ProcessPrefix},
#endif
        {"promiscuous", &Interpreter::ProcessPromiscuous},
#if OPENTHREAD_FTD
        {"pskc", &Interpreter::ProcessPskc},
#if OPENTHREAD_CONFIG_PLATFORM_KEY_REFERENCES_ENABLE
        {"pskcref", &Interpreter::ProcessPskcRef},
#endif
#endif
        {"rcp", &Interpreter::ProcessRcp},
        {"region", &Interpreter::ProcessRegion},
#if OPENTHREAD_FTD
        {"releaserouterid", &Interpreter::ProcessReleaseRouterId},
#endif
#endif // OPENTHREAD_FTD || OPENTHREAD_MTD
        {"reset", &Interpreter::ProcessReset},
#if OPENTHREAD_FTD || OPENTHREAD_MTD
        {"rloc16", &Interpreter::ProcessRloc16},
#if OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE
        {"route", &Interpreter::ProcessRoute},
#endif
#if OPENTHREAD_FTD
        {"router", &Interpreter::ProcessRouter},
        {"routerdowngradethreshold", &Interpreter::ProcessRouterDowngradeThreshold},
        {"routereligible", &Interpreter::ProcessRouterEligible},
        {"routerselectionjitter", &Interpreter::ProcessRouterSelectionJitter},
        {"routerupgradethreshold", &Interpreter::ProcessRouterUpgradeThreshold},
#endif
        {"scan", &Interpreter::ProcessScan},
#if OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE
        {"service", &Interpreter::ProcessService},
#endif
        {"singleton", &Interpreter::ProcessSingleton},
#if OPENTHREAD_CONFIG_SNTP_CLIENT_ENABLE
        {"sntp", &Interpreter::ProcessSntp},
#endif
#if OPENTHREAD_CONFIG_SRP_CLIENT_ENABLE || OPENTHREAD_CONFIG_SRP_SERVER_ENABLE
        {"srp", &Interpreter::ProcessSrp},
#endif
        {"state", &Interpreter::ProcessState},
#if OPENTHREAD_CONFIG_TCP_ENABLE && OPENTHREAD_CONFIG_CLI_TCP_ENABLE
        {"tcp", &Interpreter::ProcessTcp},
#endif
        {"thread", &Interpreter::ProcessThread},
#if OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE
        {"trel", &Interpreter::ProcessTrel},
#endif
        {"txpower", &Interpreter::ProcessTxPower},
        {"udp", &Interpreter::ProcessUdp},
        {"unsecureport", &Interpreter::ProcessUnsecurePort},
#if OPENTHREAD_CONFIG_UPTIME_ENABLE
        {"uptime", &Interpreter::ProcessUptime},
#endif
#endif // OPENTHREAD_FTD || OPENTHREAD_MTD
        {"version", &Interpreter::ProcessVersion},
    };

    static_assert(Utils::LookupTable::IsSorted(sCommands), "Command Table is not sorted");

    const otCliCommand *mUserCommands;
    uint8_t             mUserCommandsLength;
    void *              mUserCommandsContext;
    bool                mCommandIsPending;

    TimerMilliContext mTimer;

#if OPENTHREAD_FTD || OPENTHREAD_MTD
#if OPENTHREAD_CONFIG_SNTP_CLIENT_ENABLE
    bool mSntpQueryingInProgress;
#endif

    Dataset     mDataset;
    NetworkData mNetworkData;
    UdpExample  mUdp;

#if OPENTHREAD_CONFIG_TCP_ENABLE && OPENTHREAD_CONFIG_CLI_TCP_ENABLE
    TcpExample mTcp;
#endif

#if OPENTHREAD_CONFIG_COAP_API_ENABLE
    Coap mCoap;
#endif

#if OPENTHREAD_CONFIG_COAP_SECURE_API_ENABLE
    CoapSecure mCoapSecure;
#endif

#if OPENTHREAD_CONFIG_COMMISSIONER_ENABLE && OPENTHREAD_FTD
    Commissioner mCommissioner;
#endif

#if OPENTHREAD_CONFIG_JOINER_ENABLE
    Joiner mJoiner;
#endif

#if OPENTHREAD_CONFIG_SRP_CLIENT_ENABLE
    SrpClient mSrpClient;
#endif

#if OPENTHREAD_CONFIG_SRP_SERVER_ENABLE
    SrpServer mSrpServer;
#endif

#if OPENTHREAD_CONFIG_HISTORY_TRACKER_ENABLE
    History mHistory;
#endif
#endif // OPENTHREAD_FTD || OPENTHREAD_MTD

#if OPENTHREAD_CONFIG_PING_SENDER_ENABLE
    bool mPingIsAsync : 1;
#endif
#if OPENTHREAD_CONFIG_TMF_ANYCAST_LOCATOR_ENABLE
    bool mLocateInProgress : 1;
#endif
};

// Specializations of `FormatStringFor<ValueType>()`

template <> inline constexpr const char *Interpreter::FormatStringFor<uint8_t>(void)
{
    return "%u";
}

template <> inline constexpr const char *Interpreter::FormatStringFor<uint16_t>(void)
{
    return "%u";
}

template <> inline constexpr const char *Interpreter::FormatStringFor<uint32_t>(void)
{
    return "%u";
}

template <> inline constexpr const char *Interpreter::FormatStringFor<int8_t>(void)
{
    return "%d";
}

template <> inline constexpr const char *Interpreter::FormatStringFor<int16_t>(void)
{
    return "%d";
}

template <> inline constexpr const char *Interpreter::FormatStringFor<int32_t>(void)
{
    return "%d";
}

} // namespace Cli
} // namespace ot

#endif // CLI_HPP_
