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
 *   This file includes compile-time configuration constants for OpenThread.
 */

#ifndef OPENTHREAD_CORE_CONFIG_H_
#define OPENTHREAD_CORE_CONFIG_H_

#include <openthread/config.h>

#define OT_THREAD_VERSION_INVALID 0
#define OT_THREAD_VERSION_1_1 2
#define OT_THREAD_VERSION_1_2 3

#define OPENTHREAD_CORE_CONFIG_H_IN

#ifdef OPENTHREAD_PROJECT_CORE_CONFIG_FILE
#include OPENTHREAD_PROJECT_CORE_CONFIG_FILE
#elif defined(OPENTHREAD_CONFIG_CORE_USER_CONFIG_HEADER_ENABLE)
// This configuration header file should be provided by the user when
// OPENTHREAD_CONFIG_CORE_USER_CONFIG_HEADER_ENABLE is defined to 1.
#include "openthread-core-user-config.h"
#endif

#ifndef OPENTHREAD_CONFIG_THREAD_VERSION
#define OPENTHREAD_CONFIG_THREAD_VERSION OT_THREAD_VERSION_1_2
#endif

#include "config/openthread-core-default-config.h"

#include "config/announce_sender.h"
#include "config/backbone_router.h"
#include "config/border_router.h"
#include "config/channel_manager.h"
#include "config/channel_monitor.h"
#include "config/child_supervision.h"
#include "config/coap.h"
#include "config/commissioner.h"
#include "config/crypto.h"
#include "config/dataset_updater.h"
#include "config/dhcp6_client.h"
#include "config/dhcp6_server.h"
#include "config/diag.h"
#include "config/dns_client.h"
#include "config/dnssd_server.h"
#include "config/dtls.h"
#include "config/history_tracker.h"
#include "config/ip6.h"
#include "config/joiner.h"
#include "config/link_quality.h"
#include "config/link_raw.h"
#include "config/logging.h"
#include "config/mac.h"
#include "config/mle.h"
#include "config/netdata_publisher.h"
#include "config/parent_search.h"
#include "config/ping_sender.h"
#include "config/platform.h"
#include "config/radio_link.h"
#include "config/sntp_client.h"
#include "config/srp_client.h"
#include "config/srp_server.h"
#include "config/time_sync.h"
#include "config/tmf.h"

#undef OPENTHREAD_CORE_CONFIG_H_IN

#include "config/openthread-core-config-check.h"

#ifdef OPENTHREAD_CORE_CONFIG_PLATFORM_CHECK_FILE
#include OPENTHREAD_CORE_CONFIG_PLATFORM_CHECK_FILE
#endif

#endif // OPENTHREAD_CORE_CONFIG_H_
