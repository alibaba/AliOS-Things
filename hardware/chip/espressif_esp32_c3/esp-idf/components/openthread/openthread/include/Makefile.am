#
#  Copyright (c) 2016, The OpenThread Authors.
#  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are met:
#  1. Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#  2. Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#  3. Neither the name of the copyright holder nor the
#     names of its contributors may be used to endorse or promote products
#     derived from this software without specific prior written permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
#  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
#  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
#  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
#  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
#  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
#  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
#  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
#  POSSIBILITY OF SUCH DAMAGE.
#

include $(abs_top_nlbuild_autotools_dir)/automake/pre.am

# Always package (e.g. for 'make dist') these subdirectories.

EXTRA_DIST                              = \
    openthread                            \
    $(NULL)

openthread_headers                      = \
    openthread/backbone_router.h          \
    openthread/backbone_router_ftd.h      \
    openthread/border_agent.h             \
    openthread/border_router.h            \
    openthread/channel_manager.h          \
    openthread/channel_monitor.h          \
    openthread/child_supervision.h        \
    openthread/cli.h                      \
    openthread/coap.h                     \
    openthread/coap_secure.h              \
    openthread/commissioner.h             \
    openthread/config.h                   \
    openthread/crypto.h                   \
    openthread/dataset.h                  \
    openthread/dataset_ftd.h              \
    openthread/dataset_updater.h          \
    openthread/diag.h                     \
    openthread/dns.h                      \
    openthread/dns_client.h               \
    openthread/dnssd_server.h             \
    openthread/entropy.h                  \
    openthread/error.h                    \
    openthread/heap.h                     \
    openthread/history_tracker.h          \
    openthread/icmp6.h                    \
    openthread/instance.h                 \
    openthread/ip6.h                      \
    openthread/jam_detection.h            \
    openthread/joiner.h                   \
    openthread/link.h                     \
    openthread/link_metrics.h             \
    openthread/link_raw.h                 \
    openthread/logging.h                  \
    openthread/message.h                  \
    openthread/multi_radio.h              \
    openthread/ncp.h                      \
    openthread/netdata.h                  \
    openthread/netdata_publisher.h        \
    openthread/netdiag.h                  \
    openthread/network_time.h             \
    openthread/ping_sender.h              \
    openthread/random_crypto.h            \
    openthread/random_noncrypto.h         \
    openthread/server.h                   \
    openthread/sntp.h                     \
    openthread/srp_client.h               \
    openthread/srp_client_buffers.h       \
    openthread/srp_server.h               \
    openthread/tasklet.h                  \
    openthread/tcp.h                      \
    openthread/thread.h                   \
    openthread/thread_ftd.h               \
    openthread/udp.h                      \
    $(NULL)

openthreaddir = $(includedir)/openthread
dist_openthread_HEADERS = $(openthread_headers)

ot_platform_headers                     = \
    openthread/platform/alarm-micro.h     \
    openthread/platform/alarm-milli.h     \
    openthread/platform/crypto.h          \
    openthread/platform/debug_uart.h      \
    openthread/platform/diag.h            \
    openthread/platform/entropy.h         \
    openthread/platform/flash.h           \
    openthread/platform/infra_if.h        \
    openthread/platform/logging.h         \
    openthread/platform/memory.h          \
    openthread/platform/messagepool.h     \
    openthread/platform/misc.h            \
    openthread/platform/otns.h            \
    openthread/platform/radio.h           \
    openthread/platform/settings.h        \
    openthread/platform/spi-slave.h       \
    openthread/platform/time.h            \
    openthread/platform/toolchain.h       \
    openthread/platform/trel-udp6.h       \
    openthread/platform/udp.h             \
    $(NULL)

ot_platformdir = $(includedir)/openthread/platform
dist_ot_platform_HEADERS = $(ot_platform_headers)

noinst_HEADERS                          = \
    openthread/config.h                   \
    openthread/platform/debug_uart.h      \
    $(NULL)

install-headers: install-includeHEADERS

include $(abs_top_nlbuild_autotools_dir)/automake/post.am
