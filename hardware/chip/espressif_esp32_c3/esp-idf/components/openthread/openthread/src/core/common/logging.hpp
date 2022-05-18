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
 *   This file includes logging related macro/function definitions.
 */

#ifndef LOGGING_HPP_
#define LOGGING_HPP_

#include "openthread-core-config.h"

#include <ctype.h>
#include <stdio.h>

#include <openthread/logging.h>
#include <openthread/platform/logging.h>

#include "common/arg_macros.hpp"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Log level prefix string definitions.
 *
 */
#if OPENTHREAD_CONFIG_LOG_PREPEND_LEVEL
#define _OT_LEVEL_NONE_PREFIX "[NONE]"
#define _OT_LEVEL_CRIT_PREFIX "[CRIT]"
#define _OT_LEVEL_WARN_PREFIX "[WARN]"
#define _OT_LEVEL_NOTE_PREFIX "[NOTE]"
#define _OT_LEVEL_INFO_PREFIX "[INFO]"
#define _OT_LEVEL_DEBG_PREFIX "[DEBG]"
#define _OT_REGION_SUFFIX ": "
#else
#define _OT_LEVEL_NONE_PREFIX ""
#define _OT_LEVEL_CRIT_PREFIX ""
#define _OT_LEVEL_WARN_PREFIX ""
#define _OT_LEVEL_NOTE_PREFIX ""
#define _OT_LEVEL_INFO_PREFIX ""
#define _OT_LEVEL_DEBG_PREFIX ""
#define _OT_REGION_SUFFIX ""
#endif

/**
 * Log region prefix string definitions.
 *
 */
#if OPENTHREAD_CONFIG_LOG_PREPEND_REGION
#define _OT_REGION_API_PREFIX "-API-----: "
#define _OT_REGION_MLE_PREFIX "-MLE-----: "
#define _OT_REGION_ARP_PREFIX "-ARP-----: "
#define _OT_REGION_NET_DATA_PREFIX "-N-DATA--: "
#define _OT_REGION_ICMP_PREFIX "-ICMP----: "
#define _OT_REGION_IP6_PREFIX "-IP6-----: "
#define _OT_REGION_TCP_PREFIX "-TCP-----: "
#define _OT_REGION_MAC_PREFIX "-MAC-----: "
#define _OT_REGION_MEM_PREFIX "-MEM-----: "
#define _OT_REGION_NCP_PREFIX "-NCP-----: "
#define _OT_REGION_MESH_COP_PREFIX "-MESH-CP-: "
#define _OT_REGION_NET_DIAG_PREFIX "-DIAG----: "
#define _OT_REGION_PLATFORM_PREFIX "-PLAT----: "
#define _OT_REGION_COAP_PREFIX "-COAP----: "
#define _OT_REGION_CLI_PREFIX "-CLI-----: "
#define _OT_REGION_CORE_PREFIX "-CORE----: "
#define _OT_REGION_UTIL_PREFIX "-UTIL----: "
#define _OT_REGION_BBR_PREFIX "-BBR-----: "
#define _OT_REGION_MLR_PREFIX "-MLR-----: "
#define _OT_REGION_DUA_PREFIX "-DUA-----: "
#define _OT_REGION_BR_PREFIX "-BR------: "
#define _OT_REGION_SRP_PREFIX "-SRP-----: "
#define _OT_REGION_DNS_PREFIX "-DNS-----: "

// When adding a new log region, please ensure to update the array
// `kRegionPrefixStrings[]` in `Log()` function in `logging.cpp`.

#else
#define _OT_REGION_API_PREFIX _OT_REGION_SUFFIX
#define _OT_REGION_MLE_PREFIX _OT_REGION_SUFFIX
#define _OT_REGION_ARP_PREFIX _OT_REGION_SUFFIX
#define _OT_REGION_NET_DATA_PREFIX _OT_REGION_SUFFIX
#define _OT_REGION_ICMP_PREFIX _OT_REGION_SUFFIX
#define _OT_REGION_IP6_PREFIX _OT_REGION_SUFFIX
#define _OT_REGION_TCP_PREFIX _OT_REGION_PREFIX
#define _OT_REGION_MAC_PREFIX _OT_REGION_SUFFIX
#define _OT_REGION_MEM_PREFIX _OT_REGION_SUFFIX
#define _OT_REGION_NCP_PREFIX _OT_REGION_SUFFIX
#define _OT_REGION_MESH_COP_PREFIX _OT_REGION_SUFFIX
#define _OT_REGION_NET_DIAG_PREFIX _OT_REGION_SUFFIX
#define _OT_REGION_PLATFORM_PREFIX _OT_REGION_SUFFIX
#define _OT_REGION_COAP_PREFIX _OT_REGION_SUFFIX
#define _OT_REGION_CLI_PREFIX _OT_REGION_SUFFIX
#define _OT_REGION_CORE_PREFIX _OT_REGION_SUFFIX
#define _OT_REGION_UTIL_PREFIX _OT_REGION_SUFFIX
#define _OT_REGION_BBR_PREFIX _OT_REGION_SUFFIX
#define _OT_REGION_MLR_PREFIX _OT_REGION_SUFFIX
#define _OT_REGION_DUA_PREFIX _OT_REGION_SUFFIX
#define _OT_REGION_BR_PREFIX _OT_REGION_SUFFIX
#define _OT_REGION_SRP_PREFIX _OT_REGION_SUFFIX
#define _OT_REGION_DNS_PREFIX _OT_REGION_SUFFIX
#endif

/**
 * @def otLogCrit
 *
 * Logging at log level critical.
 *
 * @param[in]  aRegion   The log region.
 * @param[in]  ...       Arguments for the format specification.
 *
 */
#if OPENTHREAD_CONFIG_LOG_LEVEL < OT_LOG_LEVEL_CRIT
#define otLogCrit(aRegion, aRegionPrefix, ...)
#elif OPENTHREAD_CONFIG_LOG_DEFINE_AS_MACRO_ONLY
#define otLogCrit(aRegion, aRegionPrefix, ...) \
    _otLogFormatter(OT_LOG_LEVEL_CRIT, aRegion, _OT_LEVEL_CRIT_PREFIX aRegionPrefix __VA_ARGS__)
#else
#define otLogCrit(aRegion, aRegionPrefix, ...) _otLogCrit(aRegion, __VA_ARGS__)
void _otLogCrit(otLogRegion aRegion, const char *aFormat, ...);
#endif

/**
 * @def otLogWarn
 *
 * Logging at log level warning.
 *
 * @param[in]  aRegion   The log region.
 * @param[in]  ...       Arguments for the format specification.
 *
 */
#if OPENTHREAD_CONFIG_LOG_LEVEL < OT_LOG_LEVEL_WARN
#define otLogWarn(aRegion, aRegionPrefix, ...)
#elif OPENTHREAD_CONFIG_LOG_DEFINE_AS_MACRO_ONLY
#define otLogWarn(aRegion, aRegionPrefix, ...) \
    _otLogFormatter(OT_LOG_LEVEL_WARN, aRegion, _OT_LEVEL_WARN_PREFIX aRegionPrefix __VA_ARGS__)
#else
#define otLogWarn(aRegion, aRegionPrefix, ...) _otLogWarn(aRegion, __VA_ARGS__)
void _otLogWarn(otLogRegion aRegion, const char *aFormat, ...);
#endif

/**
 * @def otLogNote
 *
 * Logging at log level note
 *
 * @param[in]  aRegion   The log region.
 * @param[in]  ...       Arguments for the format specification.
 *
 */
#if OPENTHREAD_CONFIG_LOG_LEVEL < OT_LOG_LEVEL_NOTE
#define otLogNote(aRegion, aRegionPrefix, ...)
#elif OPENTHREAD_CONFIG_LOG_DEFINE_AS_MACRO_ONLY
#define otLogNote(aRegion, aRegionPrefix, ...) \
    _otLogFormatter(OT_LOG_LEVEL_NOTE, aRegion, _OT_LEVEL_NOTE_PREFIX aRegionPrefix __VA_ARGS__)
#else
#define otLogNote(aRegion, aRegionPrefix, ...) _otLogNote(aRegion, __VA_ARGS__)
void _otLogNote(otLogRegion aRegion, const char *aFormat, ...);
#endif

/**
 * @def otLogInfo
 *
 * Logging at log level info.
 *
 * @param[in]  aRegion   The log region.
 * @param[in]  ...       Arguments for the format specification.
 *
 */
#if OPENTHREAD_CONFIG_LOG_LEVEL < OT_LOG_LEVEL_INFO
#define otLogInfo(aRegion, aRegionPrefix, ...)
#elif OPENTHREAD_CONFIG_LOG_DEFINE_AS_MACRO_ONLY
#define otLogInfo(aRegion, aRegionPrefix, ...) \
    _otLogFormatter(OT_LOG_LEVEL_INFO, aRegion, _OT_LEVEL_INFO_PREFIX aRegionPrefix __VA_ARGS__)
#else
#define otLogInfo(aRegion, aRegionPrefix, ...) _otLogInfo(aRegion, __VA_ARGS__)
void _otLogInfo(otLogRegion aRegion, const char *aFormat, ...);
#endif

/**
 * @def otLogDebg
 *
 * Logging at log level debug.
 *
 * @param[in]  aRegion   The log region.
 * @param[in]  ...       Arguments for the format specification.
 *
 */
#if OPENTHREAD_CONFIG_LOG_LEVEL < OT_LOG_LEVEL_DEBG
#define otLogDebg(aRegion, aRegionPrefix, ...)
#elif OPENTHREAD_CONFIG_LOG_DEFINE_AS_MACRO_ONLY
#define otLogDebg(aRegion, aRegionPrefix, ...) \
    _otLogFormatter(OT_LOG_LEVEL_DEBG, aRegion, _OT_LEVEL_DEBG_PREFIX aRegionPrefix __VA_ARGS__)
#else
#define otLogDebg(aRegion, aRegionPrefix, ...) _otLogDebg(aRegion, __VA_ARGS__)
void _otLogDebg(otLogRegion aRegion, const char *aFormat, ...);
#endif

/**
 * @def otLogCritApi
 *
 * This function generates a log with level critical for the API region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogWarnApi
 *
 * This function generates a log with level warning for the API region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogNoteApi
 *
 * This function generates a log with level note for the API region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogInfoApi
 *
 * This function generates a log with level info for the API region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogDebgApi
 *
 * This function generates a log with level debug for the API region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */
#if OPENTHREAD_CONFIG_LOG_API
#define otLogCritApi(...) otLogCrit(OT_LOG_REGION_API, _OT_REGION_API_PREFIX, __VA_ARGS__)
#define otLogWarnApi(...) otLogWarn(OT_LOG_REGION_API, _OT_REGION_API_PREFIX, __VA_ARGS__)
#define otLogNoteApi(...) otLogNote(OT_LOG_REGION_API, _OT_REGION_API_PREFIX, __VA_ARGS__)
#define otLogInfoApi(...) otLogInfo(OT_LOG_REGION_API, _OT_REGION_API_PREFIX, __VA_ARGS__)
#define otLogDebgApi(...) otLogDebg(OT_LOG_REGION_API, _OT_REGION_API_PREFIX, __VA_ARGS__)
#else
#define otLogCritApi(...)
#define otLogWarnApi(...)
#define otLogNoteApi(...)
#define otLogInfoApi(...)
#define otLogDebgApi(...)
#endif

/**
 * @def otLogCritMeshCoP
 *
 * This function generates a log with level critical for the MeshCoP region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 *
 */

/**
 * @def otLogWarnMeshCoP
 *
 * This function generates a log with level warning for the MeshCoP region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogNoteMeshCoP
 *
 * This function generates a log with level note for the MeshCoP region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogInfoMeshCoP
 *
 * This function generates a log with level info for the MeshCoP region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogDebgMeshCoP
 *
 * This function generates a log with level debug for the MeshCoP region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */
#if OPENTHREAD_CONFIG_LOG_MESHCOP
#define otLogCritMeshCoP(...) otLogCrit(OT_LOG_REGION_MESH_COP, _OT_REGION_MESH_COP_PREFIX, __VA_ARGS__)
#define otLogWarnMeshCoP(...) otLogWarn(OT_LOG_REGION_MESH_COP, _OT_REGION_MESH_COP_PREFIX, __VA_ARGS__)
#define otLogNoteMeshCoP(...) otLogNote(OT_LOG_REGION_MESH_COP, _OT_REGION_MESH_COP_PREFIX, __VA_ARGS__)
#define otLogInfoMeshCoP(...) otLogInfo(OT_LOG_REGION_MESH_COP, _OT_REGION_MESH_COP_PREFIX, __VA_ARGS__)
#define otLogDebgMeshCoP(...) otLogDebg(OT_LOG_REGION_MESH_COP, _OT_REGION_MESH_COP_PREFIX, __VA_ARGS__)
#else
#define otLogCritMeshCoP(...)
#define otLogWarnMeshCoP(...)
#define otLogNoteMeshCoP(...)
#define otLogInfoMeshCoP(...)
#define otLogDebgMeshCoP(...)
#endif

#define otLogCritMbedTls(...) otLogCritMeshCoP(__VA_ARGS__)
#define otLogWarnMbedTls(...) otLogWarnMeshCoP(__VA_ARGS__)
#define otLogNoteMbedTls(...) otLogNoteMeshCoP(__VA_ARGS__)
#define otLogInfoMbedTls(...) otLogInfoMeshCoP(__VA_ARGS__)
#define otLogDebgMbedTls(...) otLogDebgMeshCoP(__VA_ARGS__)

/**
 * @def otLogCritBr
 *
 * This function generates a log with level critical for the BR region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogWarnBr
 *
 * This function generates a log with level warning for the BR region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogNoteBr
 *
 * This function generates a log with level note for the BR region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogInfoBr
 *
 * This function generates a log with level info for the BR region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogDebgBr
 *
 * This function generates a log with level debug for the BR region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */
#if OPENTHREAD_CONFIG_LOG_BR
#define otLogCritBr(...) otLogCrit(OT_LOG_REGION_BR, _OT_REGION_BR_PREFIX, __VA_ARGS__)
#define otLogWarnBr(...) otLogWarn(OT_LOG_REGION_BR, _OT_REGION_BR_PREFIX, __VA_ARGS__)
#define otLogNoteBr(...) otLogNote(OT_LOG_REGION_BR, _OT_REGION_BR_PREFIX, __VA_ARGS__)
#define otLogInfoBr(...) otLogInfo(OT_LOG_REGION_BR, _OT_REGION_BR_PREFIX, __VA_ARGS__)
#define otLogDebgBr(...) otLogDebg(OT_LOG_REGION_BR, _OT_REGION_BR_PREFIX, __VA_ARGS__)
#else
#define otLogCritBr(...)
#define otLogWarnBr(...)
#define otLogNoteBr(...)
#define otLogInfoBr(...)
#define otLogDebgBr(...)
#endif

/**
 * @def otLogCritMle
 *
 * This function generates a log with level critical for the MLE region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogWarnMle
 *
 * This function generates a log with level warning for the MLE region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogNoteMle
 *
 * This function generates a log with level note for the MLE region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogInfoMle
 *
 * This function generates a log with level info for the MLE region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogDebgMle
 *
 * This function generates a log with level debug for the MLE region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 *
 */
#if OPENTHREAD_CONFIG_LOG_MLE
#define otLogCritMle(...) otLogCrit(OT_LOG_REGION_MLE, _OT_REGION_MLE_PREFIX, __VA_ARGS__)
#define otLogWarnMle(...) otLogWarn(OT_LOG_REGION_MLE, _OT_REGION_MLE_PREFIX, __VA_ARGS__)
#define otLogNoteMle(...) otLogNote(OT_LOG_REGION_MLE, _OT_REGION_MLE_PREFIX, __VA_ARGS__)
#define otLogInfoMle(...) otLogInfo(OT_LOG_REGION_MLE, _OT_REGION_MLE_PREFIX, __VA_ARGS__)
#define otLogDebgMle(...) otLogDebg(OT_LOG_REGION_MLE, _OT_REGION_MLE_PREFIX, __VA_ARGS__)
#else
#define otLogCritMle(...)
#define otLogWarnMle(...)
#define otLogNoteMle(...)
#define otLogInfoMle(...)
#define otLogDebgMle(...)
#endif

/**
 * @def otLogCritArp
 *
 * This function generates a log with level critical for the EID-to-RLOC mapping region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogWarnArp
 *
 * This function generates a log with level warning for the EID-to-RLOC mapping region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogNoteArp
 *
 * This function generates a log with level note for the EID-to-RLOC mapping region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogInfoArp
 *
 * This function generates a log with level info for the EID-to-RLOC mapping region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogDebgArp
 *
 * This function generates a log with level debug for the EID-to-RLOC mapping region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */
#if OPENTHREAD_CONFIG_LOG_ARP
#define otLogCritArp(...) otLogCrit(OT_LOG_REGION_ARP, _OT_REGION_ARP_PREFIX, __VA_ARGS__)
#define otLogWarnArp(...) otLogWarn(OT_LOG_REGION_ARP, _OT_REGION_ARP_PREFIX, __VA_ARGS__)
#define otLogNoteArp(...) otLogNote(OT_LOG_REGION_ARP, _OT_REGION_ARP_PREFIX, __VA_ARGS__)
#define otLogInfoArp(...) otLogInfo(OT_LOG_REGION_ARP, _OT_REGION_ARP_PREFIX, __VA_ARGS__)
#define otLogDebgArp(...) otLogDebg(OT_LOG_REGION_ARP, _OT_REGION_ARP_PREFIX, __VA_ARGS__)
#else
#define otLogCritArp(...)
#define otLogWarnArp(...)
#define otLogNoteArp(...)
#define otLogInfoArp(...)
#define otLogDebgArp(...)
#endif

/**
 * @def otLogCritBbr
 *
 * This function generates a log with level critical for the Backbone Router (BBR) region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogWarnBbr
 *
 * This function generates a log with level warning for the Backbone Router (BBR) region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogNoteBbr
 *
 * This function generates a log with level note for the Backbone Router (BBR) region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogInfoBbr
 *
 * This function generates a log with level info for the Backbone Router (BBR) region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogDebgBbr
 *
 * This function generates a log with level debug for the Backbone Router (BBR) region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */
#if OPENTHREAD_CONFIG_LOG_BBR
#define otLogCritBbr(...) otLogCrit(OT_LOG_REGION_BBR, _OT_REGION_BBR_PREFIX, __VA_ARGS__)
#define otLogWarnBbr(...) otLogWarn(OT_LOG_REGION_BBR, _OT_REGION_BBR_PREFIX, __VA_ARGS__)
#define otLogNoteBbr(...) otLogNote(OT_LOG_REGION_BBR, _OT_REGION_BBR_PREFIX, __VA_ARGS__)
#define otLogInfoBbr(...) otLogInfo(OT_LOG_REGION_BBR, _OT_REGION_BBR_PREFIX, __VA_ARGS__)
#define otLogDebgBbr(...) otLogDebg(OT_LOG_REGION_BBR, _OT_REGION_BBR_PREFIX, __VA_ARGS__)
#else
#define otLogCritBbr(...)
#define otLogWarnBbr(...)
#define otLogNoteBbr(...)
#define otLogInfoBbr(...)
#define otLogDebgBbr(...)
#endif

/**
 * @def otLogCritMlr
 *
 * This function generates a log with level critical for the Multicast Listener Registration (MLR) region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogWarnMlr
 *
 * This function generates a log with level warning for the Multicast Listener Registration (MLR) region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogNoteMlr
 *
 * This function generates a log with level note for the Multicast Listener Registration (MLR) region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogInfoMlr
 *
 * This function generates a log with level info for the Multicast Listener Registration (MLR) region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogDebgMlr
 *
 * This function generates a log with level debug for the Multicast Listener Registration (MLR) region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */
#if OPENTHREAD_CONFIG_LOG_MLR
#define otLogCritMlr(...) otLogCrit(OT_LOG_REGION_MLR, _OT_REGION_MLR_PREFIX, __VA_ARGS__)
#define otLogWarnMlr(...) otLogWarn(OT_LOG_REGION_MLR, _OT_REGION_MLR_PREFIX, __VA_ARGS__)
#define otLogNoteMlr(...) otLogNote(OT_LOG_REGION_MLR, _OT_REGION_MLR_PREFIX, __VA_ARGS__)
#define otLogInfoMlr(...) otLogInfo(OT_LOG_REGION_MLR, _OT_REGION_MLR_PREFIX, __VA_ARGS__)
#define otLogDebgMlr(...) otLogDebg(OT_LOG_REGION_MLR, _OT_REGION_MLR_PREFIX, __VA_ARGS__)
#else
#define otLogCritMlr(...)
#define otLogWarnMlr(...)
#define otLogNoteMlr(...)
#define otLogInfoMlr(...)
#define otLogDebgMlr(...)
#endif

/**
 * @def otLogCritNetData
 *
 * This function generates a log with level critical for the Network Data region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogWarnNetData
 *
 * This function generates a log with level warning for the Network Data region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogNoteNetData
 *
 * This function generates a log with level note for the Network Data region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogInfoNetData
 *
 * This function generates a log with level info for the Network Data region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogDebgNetData
 *
 * This function generates a log with level debug for the Network Data region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */
#if OPENTHREAD_CONFIG_LOG_NETDATA
#define otLogCritNetData(...) otLogCrit(OT_LOG_REGION_NET_DATA, _OT_REGION_NET_DATA_PREFIX, __VA_ARGS__)
#define otLogWarnNetData(...) otLogWarn(OT_LOG_REGION_NET_DATA, _OT_REGION_NET_DATA_PREFIX, __VA_ARGS__)
#define otLogNoteNetData(...) otLogNote(OT_LOG_REGION_NET_DATA, _OT_REGION_NET_DATA_PREFIX, __VA_ARGS__)
#define otLogInfoNetData(...) otLogInfo(OT_LOG_REGION_NET_DATA, _OT_REGION_NET_DATA_PREFIX, __VA_ARGS__)
#define otLogDebgNetData(...) otLogDebg(OT_LOG_REGION_NET_DATA, _OT_REGION_NET_DATA_PREFIX, __VA_ARGS__)
#else
#define otLogCritNetData(...)
#define otLogWarnNetData(...)
#define otLogNoteNetData(...)
#define otLogInfoNetData(...)
#define otLogDebgNetData(...)
#endif

/**
 * @def otLogCritIcmp
 *
 * This function generates a log with level critical for the ICMPv6 region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogWarnIcmp
 *
 * This function generates a log with level warning for the ICMPv6 region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogNoteIcmp
 *
 * This function generates a log with level note for the ICMPv6 region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogInfoIcmp
 *
 * This function generates a log with level info for the ICMPv6 region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogDebgIcmp
 *
 * This function generates a log with level debug for the ICMPv6 region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */
#if OPENTHREAD_CONFIG_LOG_ICMP
#define otLogCritIcmp(...) otLogCrit(OT_LOG_REGION_ICMP, _OT_REGION_ICMP_PREFIX, __VA_ARGS__)
#define otLogWarnIcmp(...) otLogWarn(OT_LOG_REGION_ICMP, _OT_REGION_ICMP_PREFIX, __VA_ARGS__)
#define otLogNoteIcmp(...) otLogNote(OT_LOG_REGION_ICMP, _OT_REGION_ICMP_PREFIX, __VA_ARGS__)
#define otLogInfoIcmp(...) otLogInfo(OT_LOG_REGION_ICMP, _OT_REGION_ICMP_PREFIX, __VA_ARGS__)
#define otLogDebgIcmp(...) otLogDebg(OT_LOG_REGION_ICMP, _OT_REGION_ICMP_PREFIX, __VA_ARGS__)
#else
#define otLogCritIcmp(...)
#define otLogWarnIcmp(...)
#define otLogNoteIcmp(...)
#define otLogInfoIcmp(...)
#define otLogDebgIcmp(...)
#endif

/**
 * @def otLogCritIp6
 *
 * This function generates a log with level critical for the IPv6 region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogWarnIp6
 *
 * This function generates a log with level warning for the IPv6 region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogNoteIp6
 *
 * This function generates a log with level note for the IPv6 region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogInfoIp6
 *
 * This function generates a log with level info for the IPv6 region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogDebgIp6
 *
 * This function generates a log with level debug for the IPv6 region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */
#if OPENTHREAD_CONFIG_LOG_IP6
#define otLogCritIp6(...) otLogCrit(OT_LOG_REGION_IP6, _OT_REGION_IP6_PREFIX, __VA_ARGS__)
#define otLogWarnIp6(...) otLogWarn(OT_LOG_REGION_IP6, _OT_REGION_IP6_PREFIX, __VA_ARGS__)
#define otLogNoteIp6(...) otLogNote(OT_LOG_REGION_IP6, _OT_REGION_IP6_PREFIX, __VA_ARGS__)
#define otLogInfoIp6(...) otLogInfo(OT_LOG_REGION_IP6, _OT_REGION_IP6_PREFIX, __VA_ARGS__)
#define otLogDebgIp6(...) otLogDebg(OT_LOG_REGION_IP6, _OT_REGION_IP6_PREFIX, __VA_ARGS__)
#else
#define otLogCritIp6(...)
#define otLogWarnIp6(...)
#define otLogNoteIp6(...)
#define otLogInfoIp6(...)
#define otLogDebgIp6(...)
#endif

/**
 * @def otLogCritTcp
 *
 * This function generates a log with level critical for the TCP region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogWarnTcp
 *
 * This function generates a log with level warning for the TCP region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogNoteTcp
 *
 * This function generates a log with level note for the TCP region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogInfoTcp
 *
 * This function generates a log with level info for the TCP region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogDebgTcp
 *
 * This function generates a log with level debug for the TCP region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */
#if OPENTHREAD_CONFIG_LOG_TCP
#define otLogCritTcp(...) otLogCrit(OT_LOG_REGION_TCP, _OT_REGION_TCP_PREFIX, __VA_ARGS__)
#define otLogWarnTcp(...) otLogWarn(OT_LOG_REGION_TCP, _OT_REGION_TCP_PREFIX, __VA_ARGS__)
#define otLogNoteTcp(...) otLogNote(OT_LOG_REGION_TCP, _OT_REGION_TCP_PREFIX, __VA_ARGS__)
#define otLogInfoTcp(...) otLogInfo(OT_LOG_REGION_TCP, _OT_REGION_TCP_PREFIX, __VA_ARGS__)
#define otLogDebgTcp(...) otLogDebg(OT_LOG_REGION_TCP, _OT_REGION_TCP_PREFIX, __VA_ARGS__)
#else
#define otLogCritTcp(...)
#define otLogWarnTcp(...)
#define otLogNoteTcp(...)
#define otLogInfoTcp(...)
#define otLogDebgTcp(...)
#endif

/**
 * @def otLogCritMac
 *
 * This function generates a log with level critical for the MAC region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogWarnMac
 *
 * This function generates a log with level warning for the MAC region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogNoteMac
 *
 * This function generates a log with level note for the MAC region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogInfoMac
 *
 * This function generates a log with level info for the MAC region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogDebgMac
 *
 * This function generates a log with level debug for the MAC region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogMac
 *
 * This function generates a log with a given log level for the MAC region.
 *
 * @param[in]  aLogLevel  A log level.
 * @param[in]  aFormat    A pointer to the format string.
 * @param[in]  ...        Arguments for the format specification.
 *
 */
#if OPENTHREAD_CONFIG_LOG_MAC
#define otLogCritMac(...) otLogCrit(OT_LOG_REGION_MAC, _OT_REGION_MAC_PREFIX, __VA_ARGS__)
#define otLogWarnMac(...) otLogWarn(OT_LOG_REGION_MAC, _OT_REGION_MAC_PREFIX, __VA_ARGS__)
#define otLogNoteMac(...) otLogNote(OT_LOG_REGION_MAC, _OT_REGION_MAC_PREFIX, __VA_ARGS__)
#define otLogInfoMac(...) otLogInfo(OT_LOG_REGION_MAC, _OT_REGION_MAC_PREFIX, __VA_ARGS__)
#define otLogDebgMac(...) otLogDebg(OT_LOG_REGION_MAC, _OT_REGION_MAC_PREFIX, __VA_ARGS__)
#if OPENTHREAD_CONFIG_LOG_DEFINE_AS_MACRO_ONLY
#define otLogMac(aLogLevel, aFormat, ...)                                                     \
    do                                                                                        \
    {                                                                                         \
        if (otLoggingGetLevel() >= aLogLevel)                                                 \
        {                                                                                     \
            _otLogFormatter(aLogLevel, OT_LOG_REGION_MAC, "%s" _OT_REGION_MAC_PREFIX aFormat, \
                            otLogLevelToPrefixString(aLogLevel), __VA_ARGS__);                \
        }                                                                                     \
    } while (false)
#else
void otLogMac(otLogLevel aLogLevel, const char *aFormat, ...);
#endif
#else
#define otLogCritMac(...)
#define otLogWarnMac(...)
#define otLogNoteMac(...)
#define otLogInfoMac(...)
#define otLogDebgMac(...)
#define otLogMac(aLogLevel, ...)
#endif

/**
 * @def otLogCritCore
 *
 * This function generates a log with level critical for the Core region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogWarnCore
 *
 * This function generates a log with level warning for the Core region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogNoteCore
 *
 * This function generates a log with level note for the Core region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogInfoCore
 *
 * This function generates a log with level info for the Core region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogDebgCore
 *
 * This function generates a log with level debug for the Core region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */
#if OPENTHREAD_CONFIG_LOG_CORE
#define otLogCritCore(...) otLogCrit(OT_LOG_REGION_CORE, _OT_REGION_CORE_PREFIX, __VA_ARGS__)
#define otLogWarnCore(...) otLogWarn(OT_LOG_REGION_CORE, _OT_REGION_CORE_PREFIX, __VA_ARGS__)
#define otLogNoteCore(...) otLogNote(OT_LOG_REGION_CORE, _OT_REGION_CORE_PREFIX, __VA_ARGS__)
#define otLogInfoCore(...) otLogInfo(OT_LOG_REGION_CORE, _OT_REGION_CORE_PREFIX, __VA_ARGS__)
#define otLogDebgCore(...) otLogDebg(OT_LOG_REGION_CORE, _OT_REGION_CORE_PREFIX, __VA_ARGS__)
#else
#define otLogCritCore(...)
#define otLogWarnCore(...)
#define otLogInfoCore(...)
#define otLogDebgCore(...)
#endif

/**
 * @def otLogCritMem
 *
 * This function generates a log with level critical for the memory region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogWarnMem
 *
 * This function generates a log with level warning for the memory region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogNoteMem
 *
 * This function generates a log with level note for the memory region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogInfoMem
 *
 * This function generates a log with level info for the memory region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogDebgMem
 *
 * This function generates a log with level debug for the memory region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */
#if OPENTHREAD_CONFIG_LOG_MEM
#define otLogCritMem(...) otLogCrit(OT_LOG_REGION_MEM, _OT_REGION_MEM_PREFIX, __VA_ARGS__)
#define otLogWarnMem(...) otLogWarn(OT_LOG_REGION_MEM, _OT_REGION_MEM_PREFIX, __VA_ARGS__)
#define otLogNoteMem(...) otLogNote(OT_LOG_REGION_MEM, _OT_REGION_MEM_PREFIX, __VA_ARGS__)
#define otLogInfoMem(...) otLogInfo(OT_LOG_REGION_MEM, _OT_REGION_MEM_PREFIX, __VA_ARGS__)
#define otLogDebgMem(...) otLogDebg(OT_LOG_REGION_MEM, _OT_REGION_MEM_PREFIX, __VA_ARGS__)
#else
#define otLogCritMem(...)
#define otLogWarnMem(...)
#define otLogNoteMem(...)
#define otLogInfoMem(...)
#define otLogDebgMem(...)
#endif

/**
 * @def otLogCritUtil
 *
 * This function generates a log with level critical for the Util region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogWarnUtil
 *
 * This function generates a log with level warning for the Util region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogNoteUtil
 *
 * This function generates a log with level note for the Util region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogInfoUtil
 *
 * This function generates a log with level info for the Util region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogDebgUtil
 *
 * This function generates a log with level debug for the Util region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */
#if OPENTHREAD_CONFIG_LOG_UTIL
#define otLogCritUtil(...) otLogCrit(OT_LOG_REGION_UTIL, _OT_REGION_UTIL_PREFIX, __VA_ARGS__)
#define otLogWarnUtil(...) otLogWarn(OT_LOG_REGION_UTIL, _OT_REGION_UTIL_PREFIX, __VA_ARGS__)
#define otLogNoteUtil(...) otLogNote(OT_LOG_REGION_UTIL, _OT_REGION_UTIL_PREFIX, __VA_ARGS__)
#define otLogInfoUtil(...) otLogInfo(OT_LOG_REGION_UTIL, _OT_REGION_UTIL_PREFIX, __VA_ARGS__)
#define otLogDebgUtil(...) otLogDebg(OT_LOG_REGION_UTIL, _OT_REGION_UTIL_PREFIX, __VA_ARGS__)
#else
#define otLogCritUtil(...)
#define otLogWarnUtil(...)
#define otLogNoteUtil(...)
#define otLogInfoUtil(...)
#define otLogDebgUtil(...)
#endif

/**
 * @def otLogCritNetDiag
 *
 * This function generates a log with level critical for the NETDIAG region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogWarnNetDiag
 *
 * This function generates a log with level warning for the NETDIAG region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogNoteNetDiag
 *
 * This function generates a log with level note for the NETDIAG region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogInfoNetDiag
 *
 * This function generates a log with level info for the NETDIAG region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogDebgNetDiag
 *
 * This function generates a log with level debug for the NETDIAG region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */
#if OPENTHREAD_CONFIG_LOG_NETDIAG
#define otLogCritNetDiag(...) otLogCrit(OT_LOG_REGION_NET_DIAG, _OT_REGION_NET_DIAG_PREFIX, __VA_ARGS__)
#define otLogWarnNetDiag(...) otLogWarn(OT_LOG_REGION_NET_DIAG, _OT_REGION_NET_DIAG_PREFIX, __VA_ARGS__)
#define otLogNoteNetDiag(...) otLogNote(OT_LOG_REGION_NET_DIAG, _OT_REGION_NET_DIAG_PREFIX, __VA_ARGS__)
#define otLogInfoNetDiag(...) otLogInfo(OT_LOG_REGION_NET_DIAG, _OT_REGION_NET_DIAG_PREFIX, __VA_ARGS__)
#define otLogDebgNetDiag(...) otLogDebg(OT_LOG_REGION_NET_DIAG, _OT_REGION_NET_DIAG_PREFIX, __VA_ARGS__)
#else
#define otLogCritNetDiag(...)
#define otLogWarnNetDiag(...)
#define otLogNoteNetDiag(...)
#define otLogInfoNetDiag(...)
#define otLogDebgNetDiag(...)
#endif

/**
 * @def otLogCert
 *
 * This function generates a log with level none for the certification test.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 *
 */
#if !OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
#define otLogCertMeshCoP(...)
#elif OPENTHREAD_CONFIG_LOG_DEFINE_AS_MACRO_ONLY
#define otLogCertMeshCoP(...) _otLogFormatter(OT_LOG_LEVEL_NONE, OT_LOG_REGION_MESH_COP, __VA_ARGS__)
#else
void otLogCertMeshCoP(const char *aFormat, ...);
#endif

/**
 * @def otLogCritCli
 *
 * This function generates a log with level critical for the CLI region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogWarnCli
 *
 * This function generates a log with level warning for the CLI region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogNoteCli
 *
 * This function generates a log with level note for the CLI region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogInfoCli
 *
 * This function generates a log with level info for the CLI region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogDebgCli
 *
 * This function generates a log with level debug for the CLI region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */
#if OPENTHREAD_CONFIG_LOG_CLI

#define otLogCritCli(...) otLogCrit(OT_LOG_REGION_CLI, _OT_REGION_CLI_PREFIX, __VA_ARGS__)
#define otLogWarnCli(...) otLogWarn(OT_LOG_REGION_CLI, _OT_REGION_CLI_PREFIX, __VA_ARGS__)
#define otLogNoteCli(...) otLogNote(OT_LOG_REGION_CLI, _OT_REGION_CLI_PREFIX, __VA_ARGS__)
#define otLogInfoCli(...) otLogInfo(OT_LOG_REGION_CLI, _OT_REGION_CLI_PREFIX, __VA_ARGS__)
#define otLogDebgCli(...) otLogDebg(OT_LOG_REGION_CLI, _OT_REGION_CLI_PREFIX, __VA_ARGS__)
#else
#define otLogCritCli(...)
#define otLogWarnCli(...)
#define otLogNoteCli(...)
#define otLogInfoCli(...)
#define otLogDebgCli(...)
#endif

/**
 * @def otLogCritCoap
 *
 * This function generates a log with level critical for the CoAP region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogWarnCoap
 *
 * This function generates a log with level warning for the CoAP region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogNoteCoap
 *
 * This function generates a log with level note for the CoAP region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogInfoCoap
 *
 * This function generates a log with level info for the CoAP region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogDebgCoap
 *
 * This function generates a log with level debug for the CoAP region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */
#if OPENTHREAD_CONFIG_LOG_COAP
#define otLogCritCoap(...) otLogCrit(OT_LOG_REGION_COAP, _OT_REGION_COAP_PREFIX, __VA_ARGS__)
#define otLogWarnCoap(...) otLogWarn(OT_LOG_REGION_COAP, _OT_REGION_COAP_PREFIX, __VA_ARGS__)
#define otLogNoteCoap(...) otLogNote(OT_LOG_REGION_COAP, _OT_REGION_COAP_PREFIX, __VA_ARGS__)
#define otLogInfoCoap(...) otLogInfo(OT_LOG_REGION_COAP, _OT_REGION_COAP_PREFIX, __VA_ARGS__)
#define otLogDebgCoap(...) otLogDebg(OT_LOG_REGION_COAP, _OT_REGION_COAP_PREFIX, __VA_ARGS__)
#else
#define otLogCritCoap(...)
#define otLogWarnCoap(...)
#define otLogNoteCoap(...)
#define otLogInfoCoap(...)
#define otLogDebgCoap(...)
#endif

/**
 * @def otLogCritDua
 *
 * This function generates a log with level critical for the Domain Unicast Address region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogWarnDua
 *
 * This function generates a log with level warning for the Domain Unicast Address region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogNoteDua
 *
 * This function generates a log with level note for the Domain Unicast Address region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogInfoDua
 *
 * This function generates a log with level info for the Domain Unicast Address region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogDebgDua
 *
 * This function generates a log with level debug for the Domain Unicast Address region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */
#if OPENTHREAD_CONFIG_LOG_DUA
#define otLogCritDua(...) otLogCrit(OT_LOG_REGION_DUA, _OT_REGION_DUA_PREFIX, __VA_ARGS__)
#define otLogWarnDua(...) otLogWarn(OT_LOG_REGION_DUA, _OT_REGION_DUA_PREFIX, __VA_ARGS__)
#define otLogNoteDua(...) otLogNote(OT_LOG_REGION_DUA, _OT_REGION_DUA_PREFIX, __VA_ARGS__)
#define otLogInfoDua(...) otLogInfo(OT_LOG_REGION_DUA, _OT_REGION_DUA_PREFIX, __VA_ARGS__)
#define otLogDebgDua(...) otLogDebg(OT_LOG_REGION_DUA, _OT_REGION_DUA_PREFIX, __VA_ARGS__)
#else
#define otLogCritDua(...)
#define otLogWarnDua(...)
#define otLogNoteDua(...)
#define otLogInfoDua(...)
#define otLogDebgDua(...)
#endif

/**
 * @def otLogCritSrp
 *
 * This function generates a log with level critical for the Service Registration Protocol (SRP) region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogWarnSrp
 *
 * This function generates a log with level warning for the Service Registration Protocol (SRP) region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogNoteSrp
 *
 * This function generates a log with level note for the Service Registration Protocol (SRP) region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogInfoSrp
 *
 * This function generates a log with level info for the Service Registration Protocol (SRP) region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogDebgSrp
 *
 * This function generates a log with level debug for the Service Registration Protocol (SRP) region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */
#if OPENTHREAD_CONFIG_LOG_SRP
#define otLogCritSrp(...) otLogCrit(OT_LOG_REGION_SRP, _OT_REGION_SRP_PREFIX, __VA_ARGS__)
#define otLogWarnSrp(...) otLogWarn(OT_LOG_REGION_SRP, _OT_REGION_SRP_PREFIX, __VA_ARGS__)
#define otLogNoteSrp(...) otLogNote(OT_LOG_REGION_SRP, _OT_REGION_SRP_PREFIX, __VA_ARGS__)
#define otLogInfoSrp(...) otLogInfo(OT_LOG_REGION_SRP, _OT_REGION_SRP_PREFIX, __VA_ARGS__)
#define otLogDebgSrp(...) otLogDebg(OT_LOG_REGION_SRP, _OT_REGION_SRP_PREFIX, __VA_ARGS__)
#else
#define otLogCritSrp(...)
#define otLogWarnSrp(...)
#define otLogNoteSrp(...)
#define otLogInfoSrp(...)
#define otLogDebgSrp(...)
#endif

/**
 * @def otLogCritDns
 *
 * This function generates a log with level critical for the DNS region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogWarnDns
 *
 * This function generates a log with level warning for the DNS region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogNoteDns
 *
 * This function generates a log with level note for the DNS region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogInfoDns
 *
 * This function generates a log with level info for the DNS region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */

/**
 * @def otLogDebgDns
 *
 * This function generates a log with level debug for the DNS region.
 *
 * @param[in]  ...  Arguments for the format specification.
 *
 */
#if OPENTHREAD_CONFIG_LOG_DNS
#define otLogCritDns(...) otLogCrit(OT_LOG_REGION_DNS, _OT_REGION_DNS_PREFIX, __VA_ARGS__)
#define otLogWarnDns(...) otLogWarn(OT_LOG_REGION_DNS, _OT_REGION_DNS_PREFIX, __VA_ARGS__)
#define otLogNoteDns(...) otLogNote(OT_LOG_REGION_DNS, _OT_REGION_DNS_PREFIX, __VA_ARGS__)
#define otLogInfoDns(...) otLogInfo(OT_LOG_REGION_DNS, _OT_REGION_DNS_PREFIX, __VA_ARGS__)
#define otLogDebgDns(...) otLogDebg(OT_LOG_REGION_DNS, _OT_REGION_DNS_PREFIX, __VA_ARGS__)
#else
#define otLogCritDns(...)
#define otLogWarnDns(...)
#define otLogNoteDns(...)
#define otLogInfoDns(...)
#define otLogDebgDns(...)
#endif

/**
 * @def otLogCritPlat
 *
 * This function generates a log with level critical for the Platform region.
 *
 * @param[in]  ...       Arguments for the format specification.
 *
 */

/**
 * @def otLogWarnPlat
 *
 * This function generates a log with level warning for the Platform region.
 *
 * @param[in]  ...       Arguments for the format specification.
 *
 */

/**
 * @def otLogNotePlat
 *
 * This function generates a log with level note for the Platform region.
 *
 * @param[in]  ...       Arguments for the format specification.
 *
 */

/**
 * @def otLogInfoPlat
 *
 * This function generates a log with level info for the Platform region.
 *
 * @param[in]  ...       Arguments for the format specification.
 *
 */

/**
 * @def otLogDebgPlat
 *
 * This function generates a log with level debug for the Platform region.
 *
 * @param[in]  ...       Arguments for the format specification.
 *
 */
#if OPENTHREAD_CONFIG_LOG_PLATFORM
#define otLogCritPlat(...) otLogCrit(OT_LOG_REGION_PLATFORM, _OT_REGION_PLATFORM_PREFIX, __VA_ARGS__)
#define otLogWarnPlat(...) otLogWarn(OT_LOG_REGION_PLATFORM, _OT_REGION_PLATFORM_PREFIX, __VA_ARGS__)
#define otLogNotePlat(...) otLogNote(OT_LOG_REGION_PLATFORM, _OT_REGION_PLATFORM_PREFIX, __VA_ARGS__)
#define otLogInfoPlat(...) otLogInfo(OT_LOG_REGION_PLATFORM, _OT_REGION_PLATFORM_PREFIX, __VA_ARGS__)
#define otLogDebgPlat(...) otLogDebg(OT_LOG_REGION_PLATFORM, _OT_REGION_PLATFORM_PREFIX, __VA_ARGS__)
#else
#define otLogCritPlat(...)
#define otLogWarnPlat(...)
#define otLogNotePlat(...)
#define otLogInfoPlat(...)
#define otLogDebgPlat(...)
#endif

/**
 * @def otLogOtns
 *
 * This function generates a log with level none for the Core region,
 * and is specifically for OTNS visualization use.
 *
 * @param[in]  ...       Arguments for the format specification.
 *
 */
#if OPENTHREAD_CONFIG_OTNS_ENABLE
#if OPENTHREAD_CONFIG_LOG_DEFINE_AS_MACRO_ONLY
#define otLogOtns(...) _otLogFormatter(OT_LOG_LEVEL_NONE, OT_LOG_REGION_CORE, _OT_LEVEL_NONE_PREFIX __VA_ARGS__)
#else
void otLogOtns(const char *aFormat, ...);
#endif
#endif

/**
 * @def otDumpCrit
 *
 * This function generates a memory dump with log level critical.
 *
 * @param[in]  aRegion      The log region.
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */
#if OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_CRIT
#define otDumpCrit(aRegion, aId, aBuf, aLength) otDump(OT_LOG_LEVEL_CRIT, aRegion, aId, aBuf, aLength)
#else
#define otDumpCrit(aRegion, aId, aBuf, aLength)
#endif

/**
 * @def otDumpWarn
 *
 * This function generates a memory dump with log level warning.
 *
 * @param[in]  aRegion      The log region.
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */
#if OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_WARN
#define otDumpWarn(aRegion, aId, aBuf, aLength) otDump(OT_LOG_LEVEL_WARN, aRegion, aId, aBuf, aLength)
#else
#define otDumpWarn(aRegion, aId, aBuf, aLength)
#endif

/**
 * @def otDumpNote
 *
 * This function generates a memory dump with log level note.
 *
 * @param[in]  aRegion      The log region.
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */
#if OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_NOTE
#define otDumpNote(aRegion, aId, aBuf, aLength) otDump(OT_LOG_LEVEL_NOTE, aRegion, aId, aBuf, aLength)
#else
#define otDumpNote(aRegion, aId, aBuf, aLength)
#endif

/**
 * @def otDumpInfo
 *
 * This function generates a memory dump with log level info.
 *
 * @param[in]  aRegion      The log region.
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */
#if OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_INFO
#define otDumpInfo(aRegion, aId, aBuf, aLength) otDump(OT_LOG_LEVEL_INFO, aRegion, aId, aBuf, aLength)
#else
#define otDumpInfo(aRegion, aId, aBuf, aLength)
#endif

/**
 * @def otDumpDebg
 *
 * This function generates a memory dump with log level debug.
 *
 * @param[in]  aRegion      The log region.
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */
#if OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_DEBG
#define otDumpDebg(aRegion, aId, aBuf, aLength) otDump(OT_LOG_LEVEL_DEBG, aRegion, aId, aBuf, aLength)
#else
#define otDumpDebg(aRegion, aId, aBuf, aLength)
#endif

/**
 * @def otDumpCritNetData
 *
 * This function generates a memory dump with log level debug and region Network Data.
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */

/**
 * @def otDumpWarnNetData
 *
 * This function generates a memory dump with log level warning and region Network Data.
 *
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */

/**
 * @def otDumpNoteNetData
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */

/**
 * @def otDumpInfoNetData
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */

/**
 * @def otDumpDebgNetData
 *
 * This function generates a memory dump with log level debug and region Network Data.
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */
#if OPENTHREAD_CONFIG_LOG_NETDATA
#define otDumpCritNetData(aId, aBuf, aLength) otDumpCrit(OT_LOG_REGION_NET_DATA, aId, aBuf, aLength)
#define otDumpWarnNetData(aId, aBuf, aLength) otDumpWarn(OT_LOG_REGION_NET_DATA, aId, aBuf, aLength)
#define otDumpNoteNetData(aId, aBuf, aLength) otDumpNote(OT_LOG_REGION_NET_DATA, aId, aBuf, aLength)
#define otDumpInfoNetData(aId, aBuf, aLength) otDumpInfo(OT_LOG_REGION_NET_DATA, aId, aBuf, aLength)
#define otDumpDebgNetData(aId, aBuf, aLength) otDumpDebg(OT_LOG_REGION_NET_DATA, aId, aBuf, aLength)
#else
#define otDumpCritNetData(aId, aBuf, aLength)
#define otDumpWarnNetData(aId, aBuf, aLength)
#define otDumpNoteNetData(aId, aBuf, aLength)
#define otDumpInfoNetData(aId, aBuf, aLength)
#define otDumpDebgNetData(aId, aBuf, aLength)
#endif

/**
 * @def otDumpCritMle
 *
 * This function generates a memory dump with log level debug and region MLE.
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */

/**
 * @def otDumpWarnMle
 *
 * This function generates a memory dump with log level warning and region MLE.
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */

/**
 * @def otDumpNoteMle
 *
 * This function generates a memory dump with log level note and region MLE.
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */

/**
 * @def otDumpInfoMle
 *
 * This function generates a memory dump with log level info and region MLE.
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */

/**
 * @def otDumpDebgMle
 *
 * This function generates a memory dump with log level debug and region MLE.
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */
#if OPENTHREAD_CONFIG_LOG_MLE
#define otDumpCritMle(aId, aBuf, aLength) otDumpCrit(OT_LOG_REGION_MLE, aId, aBuf, aLength)
#define otDumpWarnMle(aId, aBuf, aLength) otDumpWarn(OT_LOG_REGION_MLE, aId, aBuf, aLength)
#define otDumpNoteMle(aId, aBuf, aLength) otDumpNote(OT_LOG_REGION_MLE, aId, aBuf, aLength)
#define otDumpInfoMle(aId, aBuf, aLength) otDumpInfo(OT_LOG_REGION_MLE, aId, aBuf, aLength)
#define otDumpDebgMle(aId, aBuf, aLength) otDumpDebg(OT_LOG_REGION_MLE, aId, aBuf, aLength)
#else
#define otDumpCritMle(aId, aBuf, aLength)
#define otDumpWarnMle(aId, aBuf, aLength)
#define otDumpNoteMle(aId, aBuf, aLength)
#define otDumpInfoMle(aId, aBuf, aLength)
#define otDumpDebgMle(aId, aBuf, aLength)
#endif

/**
 * @def otDumpCritArp
 *
 * This function generates a memory dump with log level debug and region EID-to-RLOC mapping.
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */

/**
 * @def otDumpWarnArp
 *
 * This function generates a memory dump with log level warning and region EID-to-RLOC mapping.
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */

/**
 * @def otDumpNoteArp
 *
 * This function generates a memory dump with log level note and region EID-to-RLOC mapping.
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */

/**
 * @def otDumpInfoArp
 *
 * This function generates a memory dump with log level info and region EID-to-RLOC mapping.
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */

/**
 * @def otDumpDebgArp
 *
 * This function generates a memory dump with log level debug and region EID-to-RLOC mapping.
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */
#if OPENTHREAD_CONFIG_LOG_ARP
#define otDumpCritArp(aId, aBuf, aLength) otDumpCrit(OT_LOG_REGION_ARP, aId, aBuf, aLength)
#define otDumpWarnArp(aId, aBuf, aLength) otDumpWarn(OT_LOG_REGION_ARP, aId, aBuf, aLength)
#define otDumpNoteArp(aId, aBuf, aLength) otDumpNote(OT_LOG_REGION_ARP, aId, aBuf, aLength)
#define otDumpInfoArp(aId, aBuf, aLength) otDumpInfo(OT_LOG_REGION_ARP, aId, aBuf, aLength)
#define otDumpDebgArp(aId, aBuf, aLength) otDumpDebg(OT_LOG_REGION_ARP, aId, aBuf, aLength)
#else
#define otDumpCritArp(aId, aBuf, aLength)
#define otDumpWarnArp(aId, aBuf, aLength)
#define otDumpNoteArp(aId, aBuf, aLength)
#define otDumpInfoArp(aId, aBuf, aLength)
#define otDumpDebgArp(aId, aBuf, aLength)
#endif

/**
 * @def otDumpCritBbr
 *
 * This function generates a memory dump with log level critical and region Backbone Router (BBR).
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */

/**
 * @def otDumpWarnBbr
 *
 * This function generates a memory dump with log level warning and region Backbone Router (BBR).
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */

/**
 * @def otDumpNoteBbr
 *
 * This function generates a memory dump with log level note and region Backbone Router (BBR).
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */

/**
 * @def otDumpInfoBbr
 *
 * This function generates a memory dump with log level info and region Backbone Router (BBR).
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */

/**
 * @def otDumpDebgBbr
 *
 * This function generates a memory dump with log level debug and region Backbone Router (BBR).
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */
#if OPENTHREAD_CONFIG_LOG_BBR
#define otDumpCritBbr(aId, aBuf, aLength) otDumpCrit(OT_LOG_REGION_BBR, aId, aBuf, aLength)
#define otDumpWarnBbr(aId, aBuf, aLength) otDumpWarn(OT_LOG_REGION_BBR, aId, aBuf, aLength)
#define otDumpNoteBbr(aId, aBuf, aLength) otDumpNote(OT_LOG_REGION_BBR, aId, aBuf, aLength)
#define otDumpInfoBbr(aId, aBuf, aLength) otDumpInfo(OT_LOG_REGION_BBR, aId, aBuf, aLength)
#define otDumpDebgBbr(aId, aBuf, aLength) otDumpDebg(OT_LOG_REGION_BBR, aId, aBuf, aLength)
#else
#define otDumpCritBbr(aId, aBuf, aLength)
#define otDumpWarnBbr(aId, aBuf, aLength)
#define otDumpNoteBbr(aId, aBuf, aLength)
#define otDumpInfoBbr(aId, aBuf, aLength)
#define otDumpDebgBbr(aId, aBuf, aLength)
#endif

/**
 * @def otDumpCritMlr
 *
 * This function generates a memory dump with log level critical and region Multicast Listener Registration (MLR).
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */

/**
 * @def otDumpWarnMlr
 *
 * This function generates a memory dump with log level warning and region Multicast Listener Registration (MLR).
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */

/**
 * @def otDumpNoteMlr
 *
 * This function generates a memory dump with log level note and region Multicast Listener Registration (MLR).
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */

/**
 * @def otDumpInfoMlr
 *
 * This function generates a memory dump with log level info and region Multicast Listener Registration (MLR).
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */

/**
 * @def otDumpDebgMlr
 *
 * This function generates a memory dump with log level debug and region Multicast Listener Registration (MLR).
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */
#if OPENTHREAD_CONFIG_LOG_MLR
#define otDumpCritMlr(aId, aBuf, aLength) otDumpCrit(OT_LOG_REGION_MLR, aId, aBuf, aLength)
#define otDumpWarnMlr(aId, aBuf, aLength) otDumpWarn(OT_LOG_REGION_MLR, aId, aBuf, aLength)
#define otDumpNoteMlr(aId, aBuf, aLength) otDumpNote(OT_LOG_REGION_MLR, aId, aBuf, aLength)
#define otDumpInfoMlr(aId, aBuf, aLength) otDumpInfo(OT_LOG_REGION_MLR, aId, aBuf, aLength)
#define otDumpDebgMlr(aId, aBuf, aLength) otDumpDebg(OT_LOG_REGION_MLR, aId, aBuf, aLength)
#else
#define otDumpCritMlr(aId, aBuf, aLength)
#define otDumpWarnMlr(aId, aBuf, aLength)
#define otDumpNoteMlr(aId, aBuf, aLength)
#define otDumpInfoMlr(aId, aBuf, aLength)
#define otDumpDebgMlr(aId, aBuf, aLength)
#endif

/**
 * @def otDumpCritIcmp
 *
 * This function generates a memory dump with log level debug and region ICMPv6.
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */

/**
 * @def otDumpWarnIcmp
 *
 * This function generates a memory dump with log level warning and region ICMPv6.
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */

/**
 * @def otDumpNoteIcmp
 *
 * This function generates a memory dump with log level note and region ICMPv6.
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */

/**
 * @def otDumpInfoIcmp
 *
 * This function generates a memory dump with log level info and region ICMPv6.
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */

/**
 * @def otDumpDebgIcmp
 *
 * This function generates a memory dump with log level debug and region ICMPv6.
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */
#if OPENTHREAD_CONFIG_LOG_ICMP
#define otDumpCritIcmp(aId, aBuf, aLength) otDumpCrit(OT_LOG_REGION_ICMP, aId, aBuf, aLength)
#define otDumpWarnIcmp(aId, aBuf, aLength) otDumpWarn(OT_LOG_REGION_ICMP, aId, aBuf, aLength)
#define otDumpNoteIcmp(aId, aBuf, aLength) otDumpNote(OT_LOG_REGION_ICMP, aId, aBuf, aLength)
#define otDumpInfoIcmp(aId, aBuf, aLength) otDumpInfo(OT_LOG_REGION_ICMP, aId, aBuf, aLength)
#define otDumpDebgIcmp(aId, aBuf, aLength) otDumpDebg(OT_LOG_REGION_ICMP, aId, aBuf, aLength)
#else
#define otDumpCritIcmp(aId, aBuf, aLength)
#define otDumpWarnIcmp(aId, aBuf, aLength)
#define otDumpNoteIcmp(aId, aBuf, aLength)
#define otDumpInfoIcmp(aId, aBuf, aLength)
#define otDumpDebgIcmp(aId, aBuf, aLength)
#endif

/**
 * @def otDumpCritIp6
 *
 * This function generates a memory dump with log level debug and region IPv6.
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */

/**
 * @def otDumpWarnIp6
 *
 * This function generates a memory dump with log level warning and region IPv6.
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */

/**
 * @def otDumpNoteIp6
 *
 * This function generates a memory dump with log level note and region IPv6.
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */

/**
 * @def otDumpInfoIp6
 *
 * This function generates a memory dump with log level info and region IPv6.
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */

/**
 * @def otDumpDebgIp6
 *
 * This function generates a memory dump with log level debug and region IPv6.
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */
#if OPENTHREAD_CONFIG_LOG_IP6
#define otDumpCritIp6(aId, aBuf, aLength) otDumpCrit(OT_LOG_REGION_IP6, aId, aBuf, aLength)
#define otDumpWarnIp6(aId, aBuf, aLength) otDumpWarn(OT_LOG_REGION_IP6, aId, aBuf, aLength)
#define otDumpNoteIp6(aId, aBuf, aLength) otDumpNote(OT_LOG_REGION_IP6, aId, aBuf, aLength)
#define otDumpInfoIp6(aId, aBuf, aLength) otDumpInfo(OT_LOG_REGION_IP6, aId, aBuf, aLength)
#define otDumpDebgIp6(aId, aBuf, aLength) otDumpDebg(OT_LOG_REGION_IP6, aId, aBuf, aLength)
#else
#define otDumpCritIp6(aId, aBuf, aLength)
#define otDumpWarnIp6(aId, aBuf, aLength)
#define otDumpNoteIp6(aId, aBuf, aLength)
#define otDumpInfoIp6(aId, aBuf, aLength)
#define otDumpDebgIp6(aId, aBuf, aLength)
#endif

/**
 * @def otDumpCritTcp
 *
 * This function generates a memory dump with log level debug and region TCP.
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */

/**
 * @def otDumpWartTcp
 *
 * This function generates a memory dump with log level warning and region TCP.
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */

/**
 * @def otDumpNottTcp
 *
 * This function generates a memory dump with log level note and region TCP.
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */

/**
 * @def otDumpInftTcp
 *
 * This function generates a memory dump with log level info and region TCP.
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */

/**
 * @def otDumpDebtTcp
 *
 * This function generates a memory dump with log level debug and region TCP.
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */
#if OPENTHREAD_CONFIG_LOG_TCP
#define otDumpCritTcp(aId, aBuf, aLength) otDumpCrit(OT_LOG_REGION_TCP, aId, aBuf, aLength)
#define otDumpWarnTcp(aId, aBuf, aLength) otDumpWarn(OT_LOG_REGION_TCP, aId, aBuf, aLength)
#define otDumpNoteTcp(aId, aBuf, aLength) otDumpNote(OT_LOG_REGION_TCP, aId, aBuf, aLength)
#define otDumpInfoTcp(aId, aBuf, aLength) otDumpInfo(OT_LOG_REGION_TCP, aId, aBuf, aLength)
#define otDumpDebgTcp(aId, aBuf, aLength) otDumpDebg(OT_LOG_REGION_TCP, aId, aBuf, aLength)
#else
#define otDumpCritTcp(aId, aBuf, aLength)
#define otDumpWarnTcp(aId, aBuf, aLength)
#define otDumpNoteTcp(aId, aBuf, aLength)
#define otDumpInfoTcp(aId, aBuf, aLength)
#define otDumpDebgTcp(aId, aBuf, aLength)
#endif

/**
 * @def otDumpCritMac
 *
 * This function generates a memory dump with log level debug and region MAC.
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */

/**
 * @def otDumpWarnMac
 *
 * This function generates a memory dump with log level warning and region MAC.
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */

/**
 * @def otDumpNoteMac
 *
 * This function generates a memory dump with log level note and region MAC.
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */

/**
 * @def otDumpInfoMac
 *
 * This function generates a memory dump with log level info and region MAC.
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */

/**
 * @def otDumpDebgMac
 *
 * This function generates a memory dump with log level debug and region MAC.
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */
#if OPENTHREAD_CONFIG_LOG_MAC
void otDumpMacFrame(otLogLevel aLogLevel, const char *aId, const void *aBuf, size_t aLength);
#define otDumpCritMac(aId, aBuf, aLength) otDumpMacFrame(OT_LOG_LEVEL_CRIT, aId, aBuf, aLength)
#define otDumpWarnMac(aId, aBuf, aLength) otDumpMacFrame(OT_LOG_LEVEL_WARN, aId, aBuf, aLength)
#define otDumpNoteMac(aId, aBuf, aLength) otDumpMacFrame(OT_LOG_LEVEL_NOTE, aId, aBuf, aLength)
#define otDumpInfoMac(aId, aBuf, aLength) otDumpMacFrame(OT_LOG_LEVEL_INFO, aId, aBuf, aLength)
#define otDumpDebgMac(aId, aBuf, aLength) otDumpMacFrame(OT_LOG_LEVEL_DEBG, aId, aBuf, aLength)
#else
#define otDumpCritMac(aId, aBuf, aLength)
#define otDumpWarnMac(aId, aBuf, aLength)
#define otDumpNoteMac(aId, aBuf, aLength)
#define otDumpInfoMac(aId, aBuf, aLength)
#define otDumpDebgMac(aId, aBuf, aLength)
#endif

/**
 * @def otDumpCritCore
 *
 * This function generates a memory dump with log level debug and region Core.
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */

/**
 * @def otDumpWarnCore
 *
 * This function generates a memory dump with log level warning and region Core.
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */

/**
 * @def otDumpNoteCore
 *
 * This function generates a memory dump with log level note and region Core.
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */

/**
 * @def otDumpInfoCore
 *
 * This function generates a memory dump with log level info and region Core.
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */

/**
 * @def otDumpDebgCore
 *
 * This function generates a memory dump with log level debug and region Core.
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */
#if OPENTHREAD_CONFIG_LOG_CORE
#define otDumpCritCore(aId, aBuf, aLength) otDumpCrit(OT_LOG_REGION_CORE, aId, aBuf, aLength)
#define otDumpWarnCore(aId, aBuf, aLength) otDumpWarn(OT_LOG_REGION_CORE, aId, aBuf, aLength)
#define otDumpNoteCore(aId, aBuf, aLength) otDumpNote(OT_LOG_REGION_CORE, aId, aBuf, aLength)
#define otDumpInfoCore(aId, aBuf, aLength) otDumpInfo(OT_LOG_REGION_CORE, aId, aBuf, aLength)
#define otDumpDebgCore(aId, aBuf, aLength) otDumpDebg(OT_LOG_REGION_CORE, aId, aBuf, aLength)
#else
#define otDumpCritCore(aId, aBuf, aLength)
#define otDumpWarnCore(aId, aBuf, aLength)
#define otDumpNoteCore(aId, aBuf, aLength)
#define otDumpInfoCore(aId, aBuf, aLength)
#define otDumpDebgCore(aId, aBuf, aLength)
#endif

/**
 * @def otDumpCritDua
 *
 * This function generates a memory dump with log level critical and region Domain Unicast Address.
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */

/**
 * @def otDumpWarnDua
 *
 * This function generates a memory dump with log level warning and region Domain Unicast Address.
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */

/**
 * @def otDumpNoteDua
 *
 * This function generates a memory dump with log level note and region Domain Unicast Address.
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */

/**
 * @def otDumpInfoDua
 *
 * This function generates a memory dump with log level info and region Domain Unicast Address.
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */

/**
 * @def otDumpDebgDua
 *
 * This function generates a memory dump with log level debug and region Domain Unicast Address.
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */
#if OPENTHREAD_CONFIG_LOG_DUA
#define otDumpCritDua(aId, aBuf, aLength) otDumpCrit(OT_LOG_REGION_DUA, aId, aBuf, aLength)
#define otDumpWarnDua(aId, aBuf, aLength) otDumpWarn(OT_LOG_REGION_DUA, aId, aBuf, aLength)
#define otDumpNoteDua(aId, aBuf, aLength) otDumpNote(OT_LOG_REGION_DUA, aId, aBuf, aLength)
#define otDumpInfoDua(aId, aBuf, aLength) otDumpInfo(OT_LOG_REGION_DUA, aId, aBuf, aLength)
#define otDumpDebgDua(aId, aBuf, aLength) otDumpDebg(OT_LOG_REGION_DUA, aId, aBuf, aLength)
#else
#define otDumpCritDua(aId, aBuf, aLength)
#define otDumpWarnDua(aId, aBuf, aLength)
#define otDumpNoteDua(aId, aBuf, aLength)
#define otDumpInfoDua(aId, aBuf, aLength)
#define otDumpDebgDua(aId, aBuf, aLength)
#endif

/**
 * @def otDumpCritSrp
 *
 * This function generates a memory dump with log level critical and region Service Registration Protocol (SRP).
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */

/**
 * @def otDumpWarnSrp
 *
 * This function generates a memory dump with log level warning and region Service Registration Protocol (SRP).
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */

/**
 * @def otDumpNoteSrp
 *
 * This function generates a memory dump with log level note and region Service Registration Protocol (SRP).
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */

/**
 * @def otDumpInfoSrp
 *
 * This function generates a memory dump with log level info and region Service Registration Protocol (SRP).
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */

/**
 * @def otDumpDebgSrp
 *
 * This function generates a memory dump with log level debug and region Service Registration Protocol (SRP).
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */
#if OPENTHREAD_CONFIG_LOG_SRP
#define otDumpCritSrp(aId, aBuf, aLength) otDumpCrit(OT_LOG_REGION_SRP, aId, aBuf, aLength)
#define otDumpWarnSrp(aId, aBuf, aLength) otDumpWarn(OT_LOG_REGION_SRP, aId, aBuf, aLength)
#define otDumpNoteSrp(aId, aBuf, aLength) otDumpNote(OT_LOG_REGION_SRP, aId, aBuf, aLength)
#define otDumpInfoSrp(aId, aBuf, aLength) otDumpInfo(OT_LOG_REGION_SRP, aId, aBuf, aLength)
#define otDumpDebgSrp(aId, aBuf, aLength) otDumpDebg(OT_LOG_REGION_SRP, aId, aBuf, aLength)
#else
#define otDumpCritSrp(aId, aBuf, aLength)
#define otDumpWarnSrp(aId, aBuf, aLength)
#define otDumpNoteSrp(aId, aBuf, aLength)
#define otDumpInfoSrp(aId, aBuf, aLength)
#define otDumpDebgSrp(aId, aBuf, aLength)
#endif

/**
 * @def otDumpCritBr
 *
 * This function generates a memory dump with log level critical and region Border Routing.
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */

/**
 * @def otDumpWarnBr
 *
 * This function generates a memory dump with log level warning and region Border Routing.
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */

/**
 * @def otDumpNoteBr
 *
 * This function generates a memory dump with log level note and region Border Routing.
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */

/**
 * @def otDumpInfoBr
 *
 * This function generates a memory dump with log level info and region Border Routing.
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */

/**
 * @def otDumpDebgBr
 *
 * This function generates a memory dump with log level debug and region Border Routing.
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */
#if OPENTHREAD_CONFIG_LOG_BR
#define otDumpCritBr(aId, aBuf, aLength) otDumpCrit(OT_LOG_REGION_BR, aId, aBuf, aLength)
#define otDumpWarnBr(aId, aBuf, aLength) otDumpWarn(OT_LOG_REGION_BR, aId, aBuf, aLength)
#define otDumpNoteBr(aId, aBuf, aLength) otDumpNote(OT_LOG_REGION_BR, aId, aBuf, aLength)
#define otDumpInfoBr(aId, aBuf, aLength) otDumpInfo(OT_LOG_REGION_BR, aId, aBuf, aLength)
#define otDumpDebgBr(aId, aBuf, aLength) otDumpDebg(OT_LOG_REGION_BR, aId, aBuf, aLength)
#else
#define otDumpCritBr(aId, aBuf, aLength)
#define otDumpWarnBr(aId, aBuf, aLength)
#define otDumpNoteBr(aId, aBuf, aLength)
#define otDumpInfoBr(aId, aBuf, aLength)
#define otDumpDebgBr(aId, aBuf, aLength)
#endif

/**
 * @def otDumpCritDns
 *
 * This function generates a memory dump with log level critical and region DNS.
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */

/**
 * @def otDumpWarnDns
 *
 * This function generates a memory dump with log level warning and region DNS.
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */

/**
 * @def otDumpNoteDns
 *
 * This function generates a memory dump with log level note and region DNS.
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */

/**
 * @def otDumpInfoDns
 *
 * This function generates a memory dump with log level info and region DNS.
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */

/**
 * @def otDumpDebgDns
 *
 * This function generates a memory dump with log level debug and region DNS.
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */
#if OPENTHREAD_CONFIG_LOG_SRP
#define otDumpCritDns(aId, aBuf, aLength) otDumpCrit(OT_LOG_REGION_SRP, aId, aBuf, aLength)
#define otDumpWarnDns(aId, aBuf, aLength) otDumpWarn(OT_LOG_REGION_SRP, aId, aBuf, aLength)
#define otDumpNoteDns(aId, aBuf, aLength) otDumpNote(OT_LOG_REGION_SRP, aId, aBuf, aLength)
#define otDumpInfoDns(aId, aBuf, aLength) otDumpInfo(OT_LOG_REGION_SRP, aId, aBuf, aLength)
#define otDumpDebgDns(aId, aBuf, aLength) otDumpDebg(OT_LOG_REGION_SRP, aId, aBuf, aLength)
#else
#define otDumpCritDns(aId, aBuf, aLength)
#define otDumpWarnDns(aId, aBuf, aLength)
#define otDumpNoteDns(aId, aBuf, aLength)
#define otDumpInfoDns(aId, aBuf, aLength)
#define otDumpDebgDns(aId, aBuf, aLength)
#endif

/**
 * @def otDumpCritMem
 *
 * This function generates a memory dump with log level debug and region memory.
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */

/**
 * @def otDumpWarnMem
 *
 * This function generates a memory dump with log level warning and region memory.
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */

/**
 * @def otDumpNoteMem
 *
 * This function generates a memory dump with log level note and region memory.
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */

/**
 * @def otDumpInfoMem
 *
 * This function generates a memory dump with log level info and region memory.
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */

/**
 * @def otDumpDebgMem
 *
 * This function generates a memory dump with log level debug and region memory.
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */
#if OPENTHREAD_CONFIG_LOG_MEM
#define otDumpCritMem(aId, aBuf, aLength) otDumpCrit(OT_LOG_REGION_MEM, aId, aBuf, aLength)
#define otDumpWarnMem(aId, aBuf, aLength) otDumpWarn(OT_LOG_REGION_MEM, aId, aBuf, aLength)
#define otDumpNoteMem(aId, aBuf, aLength) otDumpNote(OT_LOG_REGION_MEM, aId, aBuf, aLength)
#define otDumpInfoMem(aId, aBuf, aLength) otDumpInfo(OT_LOG_REGION_MEM, aId, aBuf, aLength)
#define otDumpDebgMem(aId, aBuf, aLength) otDumpDebg(OT_LOG_REGION_MEM, aId, aBuf, aLength)
#else
#define otDumpCritMem(aId, aBuf, aLength)
#define otDumpWarnMem(aId, aBuf, aLength)
#define otDumpNoteMem(aId, aBuf, aLength)
#define otDumpInfoMem(aId, aBuf, aLength)
#define otDumpDebgMem(aId, aBuf, aLength)
#endif

/**
 * @def otDumpCertMeshCoP
 *
 * This function generates a memory dump with log level none for the certification test.
 *
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */
#if OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
#define otDumpCertMeshCoP(aId, aBuf, aLength) otDump(OT_LOG_LEVEL_NONE, OT_LOG_REGION_MESH_COP, aId, aBuf, aLength)
#else
#define otDumpCertMeshCoP(aId, aBuf, aLength)
#endif

/**
 * This function dumps bytes to the log in a human-readable fashion.
 *
 * @param[in]  aLogLevel    The log level.
 * @param[in]  aLogRegion   The log region.
 * @param[in]  aId          A pointer to a NULL-terminated string that is printed before the bytes.
 * @param[in]  aBuf         A pointer to the buffer.
 * @param[in]  aLength      Number of bytes to print.
 *
 */
void otDump(otLogLevel aLogLevel, otLogRegion aLogRegion, const char *aId, const void *aBuf, size_t aLength);

#if OPENTHREAD_CONFIG_LOG_DEFINE_AS_MACRO_ONLY || OPENTHREAD_CONFIG_LOG_PREPEND_LEVEL
/**
 * This function converts a log level to a prefix string for appending to log message.
 *
 * @param[in]  aLogLevel    A log level.
 *
 * @returns A C string representing the log level.
 *
 */
const char *otLogLevelToPrefixString(otLogLevel aLogLevel);
#endif

#if OPENTHREAD_CONFIG_LOG_DEFINE_AS_MACRO_ONLY

/**
 * Local/private macro to format the log message
 */
#define _otLogFormatter(aLogLevel, aRegion, ...) \
    _otDynamicLog(aLogLevel, aRegion, OT_FIRST_ARG(__VA_ARGS__) OPENTHREAD_CONFIG_LOG_SUFFIX OT_REST_ARGS(__VA_ARGS__))

#if OPENTHREAD_CONFIG_LOG_LEVEL_DYNAMIC_ENABLE

#if OPENTHREAD_CONFIG_MULTIPLE_INSTANCE_ENABLE
#error "Dynamic log level is not supported along with multiple OT instance feature"
#endif

/**
 * Local/private macro to dynamically filter log level.
 */
#define _otDynamicLog(aLogLevel, aRegion, ...)           \
    do                                                   \
    {                                                    \
        if (otLoggingGetLevel() >= aLogLevel)            \
            _otPlatLog(aLogLevel, aRegion, __VA_ARGS__); \
    } while (false)

#else // OPENTHREAD_CONFIG_LOG_LEVEL_DYNAMIC_ENABLE

#define _otDynamicLog(aLogLevel, aRegion, ...) _otPlatLog(aLogLevel, aRegion, __VA_ARGS__)

#endif // OPENTHREAD_CONFIG_LOG_LEVEL_DYNAMIC_ENABLE

/**
 * `OPENTHREAD_CONFIG_PLAT_LOG_MACRO_NAME` is a configuration parameter (see `config/logging.h`) which specifies the
 * function/macro to be used for logging in OpenThread. By default it is set to `otPlatLog()`.
 *
 */
#define _otPlatLog(aLogLevel, aRegion, ...) OPENTHREAD_CONFIG_PLAT_LOG_MACRO_NAME(aLogLevel, aRegion, __VA_ARGS__)

#endif // OPENTHREAD_CONFIG_LOG_DEFINE_AS_MACRO_ONLY

#define _otLogResult(aRegion, aError, ...)                                                 \
    do                                                                                     \
    {                                                                                      \
        otError _err = (aError);                                                           \
                                                                                           \
        if (_err == OT_ERROR_NONE)                                                         \
        {                                                                                  \
            otLogInfo##aRegion(OT_FIRST_ARG(__VA_ARGS__) ": %s" OT_REST_ARGS(__VA_ARGS__), \
                               otThreadErrorToString(_err));                               \
        }                                                                                  \
        else                                                                               \
        {                                                                                  \
            otLogWarn##aRegion(OT_FIRST_ARG(__VA_ARGS__) ": %s" OT_REST_ARGS(__VA_ARGS__), \
                               otThreadErrorToString(_err));                               \
        }                                                                                  \
    } while (false)

/**
 * @def otLogResultPlat
 *
 * This function generates a log for the Plat region according to the error result. If @p aError is `kErrorNone`, the
 * log level is info. Otherwise the log level is warn.
 *
 * @param[in]  aError    The error result.
 * @param[in]  ...       Arguments for the format specification.
 *
 */
#define otLogResultPlat(aError, ...) _otLogResult(Plat, aError, __VA_ARGS__)

/**
 * @def otLogResultBbr
 *
 * This function generates a log for the BBR region according to the error result. If @p aError is `OT_ERROR_NONE`, the
 * log level is info. Otherwise the log level is warn.
 *
 * @param[in]  aError    The error result.
 * @param[in]  ...       Arguments for the format specification.
 *
 */
#define otLogResultBbr(aError, ...) _otLogResult(Bbr, aError, __VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif // LOGGING_HPP_
