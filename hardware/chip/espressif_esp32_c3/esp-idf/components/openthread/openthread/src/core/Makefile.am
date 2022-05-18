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

lib_LIBRARIES                              = $(NULL)

if OPENTHREAD_ENABLE_FTD
lib_LIBRARIES                             += libopenthread-ftd.a
endif

if OPENTHREAD_ENABLE_MTD
lib_LIBRARIES                             += libopenthread-mtd.a
endif

if OPENTHREAD_ENABLE_RADIO_ONLY
lib_LIBRARIES                             += libopenthread-radio.a
endif

if OPENTHREAD_ENABLE_RADIO_CLI
lib_LIBRARIES                             += libopenthread-radio-cli.a
endif

CPPFLAGS_COMMON                            = \
    -I$(top_srcdir)/include                  \
    $(OPENTHREAD_TARGET_DEFINES)             \
    $(NULL)

libopenthread_radio_a_CPPFLAGS             = \
    $(CPPFLAGS_COMMON)                       \
    -DOPENTHREAD_MTD=0                       \
    -DOPENTHREAD_FTD=0                       \
    -DOPENTHREAD_RADIO=1                     \
    -DOPENTHREAD_RADIO_CLI=0                 \
    $(NULL)

libopenthread_radio_cli_a_CPPFLAGS         = \
    $(CPPFLAGS_COMMON)                       \
    -DOPENTHREAD_MTD=0                       \
    -DOPENTHREAD_FTD=0                       \
    -DOPENTHREAD_RADIO=1                     \
    -DOPENTHREAD_RADIO_CLI=1                 \
    $(NULL)

libopenthread_ftd_a_CPPFLAGS               = \
    $(CPPFLAGS_COMMON)                       \
    -DOPENTHREAD_MTD=0                       \
    -DOPENTHREAD_FTD=1                       \
    -DOPENTHREAD_RADIO=0                     \
    $(NULL)

libopenthread_mtd_a_CPPFLAGS               = \
    $(CPPFLAGS_COMMON)                       \
    -DOPENTHREAD_MTD=1                       \
    -DOPENTHREAD_FTD=0                       \
    -DOPENTHREAD_RADIO=0                     \
    $(NULL)

#------------------------------------------------------
# Note to maintainer/developers about "SOURCES_COMMON"
#
# The traditional method for GNU style Makefile.am files
# is to have IF statements within the Makefile
#
# These IF statements would add (or not add) certain
# source files from the build/library being built.
#
# In general, IDEs do not easily support this.
# Some do, some do not, those that do support it
# often do so in a very different or complex way.
# Bottom line: It is a very mixed bag of results.
#
# Thus we bend to the IDEs, and impose these rules:
#
# Rule #1
#     All code(files) should be compiled
#     Even if it is not used or required
#
#     For example: In the MTD build, some features
#     are disabled and thus in the traditional scheme
#     source files could be excluded SOURCES_COMMON
#     via makefile IF/ELSE/ENDIF constructs.
#
#     This makes it impossibly hard for IDEs.
#     Thus we compile everything.
#
#     And then rely upon Rule #2.
#
# Rule #2
#     Files that are not required, in certain
#     configurations require appropriate wrappers
#     Such as #if/#else/#endif
#
#     The net result is sort of an "empty translation
#     unit"
#

SOURCES_COMMON                                  = \
    api/backbone_router_api.cpp                   \
    api/backbone_router_ftd_api.cpp               \
    api/border_agent_api.cpp                      \
    api/border_router_api.cpp                     \
    api/channel_manager_api.cpp                   \
    api/channel_monitor_api.cpp                   \
    api/child_supervision_api.cpp                 \
    api/coap_api.cpp                              \
    api/coap_secure_api.cpp                       \
    api/commissioner_api.cpp                      \
    api/crypto_api.cpp                            \
    api/dataset_api.cpp                           \
    api/dataset_ftd_api.cpp                       \
    api/dataset_updater_api.cpp                   \
    api/diags_api.cpp                             \
    api/dns_api.cpp                               \
    api/dns_server_api.cpp                        \
    api/entropy_api.cpp                           \
    api/error_api.cpp                             \
    api/heap_api.cpp                              \
    api/history_tracker_api.cpp                   \
    api/icmp6_api.cpp                             \
    api/instance_api.cpp                          \
    api/ip6_api.cpp                               \
    api/jam_detection_api.cpp                     \
    api/joiner_api.cpp                            \
    api/link_api.cpp                              \
    api/link_metrics_api.cpp                      \
    api/link_raw_api.cpp                          \
    api/logging_api.cpp                           \
    api/message_api.cpp                           \
    api/multi_radio_api.cpp                       \
    api/netdata_api.cpp                           \
    api/netdata_publisher_api.cpp                 \
    api/netdiag_api.cpp                           \
    api/network_time_api.cpp                      \
    api/ping_sender_api.cpp                       \
    api/random_crypto_api.cpp                     \
    api/random_noncrypto_api.cpp                  \
    api/server_api.cpp                            \
    api/sntp_api.cpp                              \
    api/srp_client_api.cpp                        \
    api/srp_client_buffers_api.cpp                \
    api/srp_server_api.cpp                        \
    api/tasklet_api.cpp                           \
    api/tcp_api.cpp                               \
    api/thread_api.cpp                            \
    api/thread_ftd_api.cpp                        \
    api/udp_api.cpp                               \
    backbone_router/backbone_tmf.cpp              \
    backbone_router/bbr_leader.cpp                \
    backbone_router/bbr_local.cpp                 \
    backbone_router/bbr_manager.cpp               \
    backbone_router/multicast_listeners_table.cpp \
    backbone_router/ndproxy_table.cpp             \
    border_router/infra_if_platform.cpp           \
    border_router/router_advertisement.cpp        \
    border_router/routing_manager.cpp             \
    coap/coap.cpp                                 \
    coap/coap_message.cpp                         \
    coap/coap_secure.cpp                          \
    common/crc16.cpp                              \
    common/error.cpp                              \
    common/heap_string.cpp                        \
    common/instance.cpp                           \
    common/logging.cpp                            \
    common/message.cpp                            \
    common/notifier.cpp                           \
    common/random_manager.cpp                     \
    common/settings.cpp                           \
    common/string.cpp                             \
    common/tasklet.cpp                            \
    common/time_ticker.cpp                        \
    common/timer.cpp                              \
    common/tlvs.cpp                               \
    common/trickle_timer.cpp                      \
    common/uptime.cpp                             \
    crypto/aes_ccm.cpp                            \
    crypto/aes_ecb.cpp                            \
    crypto/crypto_platform.cpp                    \
    crypto/ecdsa.cpp                              \
    crypto/hkdf_sha256.cpp                        \
    crypto/hmac_sha256.cpp                        \
    crypto/mbedtls.cpp                            \
    crypto/pbkdf2_cmac.cpp                        \
    crypto/sha256.cpp                             \
    crypto/storage.cpp                            \
    diags/factory_diags.cpp                       \
    mac/channel_mask.cpp                          \
    mac/data_poll_handler.cpp                     \
    mac/data_poll_sender.cpp                      \
    mac/link_raw.cpp                              \
    mac/mac.cpp                                   \
    mac/mac_filter.cpp                            \
    mac/mac_frame.cpp                             \
    mac/mac_links.cpp                             \
    mac/mac_types.cpp                             \
    mac/sub_mac.cpp                               \
    mac/sub_mac_callbacks.cpp                     \
    meshcop/announce_begin_client.cpp             \
    meshcop/border_agent.cpp                      \
    meshcop/commissioner.cpp                      \
    meshcop/dataset.cpp                           \
    meshcop/dataset_local.cpp                     \
    meshcop/dataset_manager.cpp                   \
    meshcop/dataset_manager_ftd.cpp               \
    meshcop/dataset_updater.cpp                   \
    meshcop/dtls.cpp                              \
    meshcop/energy_scan_client.cpp                \
    meshcop/joiner.cpp                            \
    meshcop/joiner_router.cpp                     \
    meshcop/meshcop.cpp                           \
    meshcop/meshcop_leader.cpp                    \
    meshcop/meshcop_tlvs.cpp                      \
    meshcop/panid_query_client.cpp                \
    meshcop/timestamp.cpp                         \
    net/checksum.cpp                              \
    net/dhcp6_client.cpp                          \
    net/dhcp6_server.cpp                          \
    net/dns_client.cpp                            \
    net/dns_types.cpp                             \
    net/dnssd_server.cpp                          \
    net/icmp6.cpp                                 \
    net/ip4_address.cpp                           \
    net/ip6.cpp                                   \
    net/ip6_address.cpp                           \
    net/ip6_filter.cpp                            \
    net/ip6_headers.cpp                           \
    net/ip6_mpl.cpp                               \
    net/nd_agent.cpp                              \
    net/netif.cpp                                 \
    net/sntp_client.cpp                           \
    net/socket.cpp                                \
    net/srp_client.cpp                            \
    net/srp_server.cpp                            \
    net/tcp6.cpp                                  \
    net/udp6.cpp                                  \
    radio/radio.cpp                               \
    radio/radio_callbacks.cpp                     \
    radio/radio_platform.cpp                      \
    radio/trel_interface.cpp                      \
    radio/trel_link.cpp                           \
    radio/trel_packet.cpp                         \
    thread/address_resolver.cpp                   \
    thread/announce_begin_server.cpp              \
    thread/announce_sender.cpp                    \
    thread/anycast_locator.cpp                    \
    thread/child_table.cpp                        \
    thread/csl_tx_scheduler.cpp                   \
    thread/discover_scanner.cpp                   \
    thread/dua_manager.cpp                        \
    thread/energy_scan_server.cpp                 \
    thread/indirect_sender.cpp                    \
    thread/key_manager.cpp                        \
    thread/link_metrics.cpp                       \
    thread/link_quality.cpp                       \
    thread/lowpan.cpp                             \
    thread/mesh_forwarder.cpp                     \
    thread/mesh_forwarder_ftd.cpp                 \
    thread/mesh_forwarder_mtd.cpp                 \
    thread/mle.cpp                                \
    thread/mle_router.cpp                         \
    thread/mle_types.cpp                          \
    thread/mlr_manager.cpp                        \
    thread/neighbor_table.cpp                     \
    thread/network_data.cpp                       \
    thread/network_data_leader.cpp                \
    thread/network_data_leader_ftd.cpp            \
    thread/network_data_local.cpp                 \
    thread/network_data_notifier.cpp              \
    thread/network_data_publisher.cpp             \
    thread/network_data_service.cpp               \
    thread/network_data_tlvs.cpp                  \
    thread/network_data_types.cpp                 \
    thread/network_diagnostic.cpp                 \
    thread/panid_query_server.cpp                 \
    thread/radio_selector.cpp                     \
    thread/router_table.cpp                       \
    thread/src_match_controller.cpp               \
    thread/thread_netif.cpp                       \
    thread/time_sync_service.cpp                  \
    thread/tmf.cpp                                \
    thread/topology.cpp                           \
    thread/uri_paths.cpp                          \
    utils/channel_manager.cpp                     \
    utils/channel_monitor.cpp                     \
    utils/child_supervision.cpp                   \
    utils/flash.cpp                               \
    utils/heap.cpp                                \
    utils/history_tracker.cpp                     \
    utils/jam_detector.cpp                        \
    utils/lookup_table.cpp                        \
    utils/otns.cpp                                \
    utils/parse_cmdline.cpp                       \
    utils/ping_sender.cpp                         \
    utils/slaac_address.cpp                       \
    utils/srp_client_buffers.cpp                  \
    $(NULL)

RADIO_SOURCES_COMMON                       = \
    api/diags_api.cpp                        \
    api/error_api.cpp                        \
    api/instance_api.cpp                     \
    api/link_raw_api.cpp                     \
    api/logging_api.cpp                      \
    api/random_noncrypto_api.cpp             \
    api/tasklet_api.cpp                      \
    common/error.cpp                         \
    common/instance.cpp                      \
    common/logging.cpp                       \
    common/random_manager.cpp                \
    common/string.cpp                        \
    common/tasklet.cpp                       \
    common/timer.cpp                         \
    common/uptime.cpp                        \
    crypto/aes_ccm.cpp                       \
    crypto/aes_ecb.cpp                       \
    crypto/crypto_platform.cpp               \
    crypto/storage.cpp                       \
    diags/factory_diags.cpp                  \
    mac/link_raw.cpp                         \
    mac/mac_frame.cpp                        \
    mac/mac_types.cpp                        \
    mac/sub_mac.cpp                          \
    mac/sub_mac_callbacks.cpp                \
    radio/radio.cpp                          \
    radio/radio_callbacks.cpp                \
    radio/radio_platform.cpp                 \
    thread/link_quality.cpp                  \
    utils/lookup_table.cpp                   \
    utils/parse_cmdline.cpp                  \
    $(NULL)

EXTRA_DIST                                 = \
    common/extension_example.cpp             \
    $(NULL)

libopenthread_radio_a_SOURCES              = \
    $(RADIO_SOURCES_COMMON)                  \
    $(NULL)

libopenthread_radio_cli_a_SOURCES          = \
    $(RADIO_SOURCES_COMMON)                  \
    $(NULL)

libopenthread_mtd_a_SOURCES                = \
    $(SOURCES_COMMON)                        \
    $(NULL)

libopenthread_ftd_a_SOURCES                = \
    $(SOURCES_COMMON)                        \
    $(NULL)

if OPENTHREAD_ENABLE_VENDOR_EXTENSION

.INTERMEDIATE: vendor_extension_temp.cpp
vendor_extension_temp.cpp: ${OPENTHREAD_VENDOR_EXTENSION_SOURCE}
	$(AM_V_GEN)cp $< $@

# "nodist_" prefix tells automake not to include in 'make dist'

nodist_libopenthread_ftd_a_SOURCES         = \
    vendor_extension_temp.cpp                \
    $(NULL)

nodist_libopenthread_mtd_a_SOURCES         = \
    vendor_extension_temp.cpp                \
    $(NULL)

nodist_libopenthread_radio_a_SOURCES       = \
    vendor_extension_temp.cpp                \
    $(NULL)

nodist_libopenthread_radio_cli_a_SOURCES   = \
    vendor_extension_temp.cpp                \
    $(NULL)

endif # OPENTHREAD_ENABLE_VENDOR_EXTENSION

HEADERS_COMMON                                  = \
    backbone_router/backbone_tmf.hpp              \
    backbone_router/bbr_leader.hpp                \
    backbone_router/bbr_local.hpp                 \
    backbone_router/bbr_manager.hpp               \
    backbone_router/multicast_listeners_table.hpp \
    backbone_router/ndproxy_table.hpp             \
    border_router/router_advertisement.hpp        \
    border_router/routing_manager.hpp             \
    coap/coap.hpp                                 \
    coap/coap_message.hpp                         \
    coap/coap_secure.hpp                          \
    common/arg_macros.hpp                         \
    common/array.hpp                              \
    common/as_core_type.hpp                       \
    common/bit_vector.hpp                         \
    common/clearable.hpp                          \
    common/code_utils.hpp                         \
    common/const_cast.hpp                         \
    common/crc16.hpp                              \
    common/data.hpp                               \
    common/debug.hpp                              \
    common/encoding.hpp                           \
    common/equatable.hpp                          \
    common/error.hpp                              \
    common/extension.hpp                          \
    common/heap_string.hpp                        \
    common/instance.hpp                           \
    common/iterator_utils.hpp                     \
    common/linked_list.hpp                        \
    common/locator.hpp                            \
    common/locator_getters.hpp                    \
    common/logging.hpp                            \
    common/message.hpp                            \
    common/new.hpp                                \
    common/non_copyable.hpp                       \
    common/notifier.hpp                           \
    common/numeric_limits.hpp                     \
    common/pool.hpp                               \
    common/random.hpp                             \
    common/random_manager.hpp                     \
    common/settings.hpp                           \
    common/settings_driver.hpp                    \
    common/string.hpp                             \
    common/tasklet.hpp                            \
    common/time.hpp                               \
    common/time_ticker.hpp                        \
    common/timer.hpp                              \
    common/tlvs.hpp                               \
    common/trickle_timer.hpp                      \
    common/type_traits.hpp                        \
    common/uptime.hpp                             \
    config/announce_sender.h                      \
    config/backbone_router.h                      \
    config/border_router.h                        \
    config/channel_manager.h                      \
    config/channel_monitor.h                      \
    config/child_supervision.h                    \
    config/coap.h                                 \
    config/commissioner.h                         \
    config/crypto.h                               \
    config/dataset_updater.h                      \
    config/dhcp6_client.h                         \
    config/dhcp6_server.h                         \
    config/diag.h                                 \
    config/dns_client.h                           \
    config/dnssd_server.h                         \
    config/dtls.h                                 \
    config/history_tracker.h                      \
    config/ip6.h                                  \
    config/joiner.h                               \
    config/link_quality.h                         \
    config/link_raw.h                             \
    config/logging.h                              \
    config/mac.h                                  \
    config/mle.h                                  \
    config/netdata_publisher.h                    \
    config/openthread-core-config-check.h         \
    config/openthread-core-default-config.h       \
    config/parent_search.h                        \
    config/ping_sender.h                          \
    config/platform.h                             \
    config/radio_link.h                           \
    config/sntp_client.h                          \
    config/srp_client.h                           \
    config/srp_server.h                           \
    config/time_sync.h                            \
    config/tmf.h                                  \
    crypto/aes_ccm.hpp                            \
    crypto/aes_ecb.hpp                            \
    crypto/context_size.hpp                       \
    crypto/ecdsa.hpp                              \
    crypto/hkdf_sha256.hpp                        \
    crypto/hmac_sha256.hpp                        \
    crypto/mbedtls.hpp                            \
    crypto/pbkdf2_cmac.hpp                        \
    crypto/sha256.hpp                             \
    crypto/storage.hpp                            \
    diags/factory_diags.hpp                       \
    mac/channel_mask.hpp                          \
    mac/data_poll_handler.hpp                     \
    mac/data_poll_sender.hpp                      \
    mac/link_raw.hpp                              \
    mac/mac.hpp                                   \
    mac/mac_filter.hpp                            \
    mac/mac_frame.hpp                             \
    mac/mac_links.hpp                             \
    mac/mac_types.hpp                             \
    mac/sub_mac.hpp                               \
    meshcop/announce_begin_client.hpp             \
    meshcop/border_agent.hpp                      \
    meshcop/commissioner.hpp                      \
    meshcop/dataset.hpp                           \
    meshcop/dataset_local.hpp                     \
    meshcop/dataset_manager.hpp                   \
    meshcop/dataset_updater.hpp                   \
    meshcop/dtls.hpp                              \
    meshcop/energy_scan_client.hpp                \
    meshcop/joiner.hpp                            \
    meshcop/joiner_router.hpp                     \
    meshcop/meshcop.hpp                           \
    meshcop/meshcop_leader.hpp                    \
    meshcop/meshcop_tlvs.hpp                      \
    meshcop/panid_query_client.hpp                \
    meshcop/timestamp.hpp                         \
    net/checksum.hpp                              \
    net/dhcp6.hpp                                 \
    net/dhcp6_client.hpp                          \
    net/dhcp6_server.hpp                          \
    net/dns_client.hpp                            \
    net/dns_types.hpp                             \
    net/dnssd_server.hpp                          \
    net/icmp6.hpp                                 \
    net/ip4_address.hpp                           \
    net/ip6.hpp                                   \
    net/ip6_address.hpp                           \
    net/ip6_filter.hpp                            \
    net/ip6_headers.hpp                           \
    net/ip6_mpl.hpp                               \
    net/nd_agent.hpp                              \
    net/netif.hpp                                 \
    net/sntp_client.hpp                           \
    net/socket.hpp                                \
    net/srp_client.hpp                            \
    net/srp_server.hpp                            \
    net/tcp6.hpp                                  \
    net/udp6.hpp                                  \
    openthread-core-config.h                      \
    radio/max_power_table.hpp                     \
    radio/radio.hpp                               \
    radio/trel_interface.hpp                      \
    radio/trel_link.hpp                           \
    radio/trel_packet.hpp                         \
    thread/address_resolver.hpp                   \
    thread/announce_begin_server.hpp              \
    thread/announce_sender.hpp                    \
    thread/anycast_locator.hpp                    \
    thread/child_mask.hpp                         \
    thread/child_table.hpp                        \
    thread/csl_tx_scheduler.hpp                   \
    thread/discover_scanner.hpp                   \
    thread/dua_manager.hpp                        \
    thread/energy_scan_server.hpp                 \
    thread/indirect_sender.hpp                    \
    thread/indirect_sender_frame_context.hpp      \
    thread/key_manager.hpp                        \
    thread/link_metrics.hpp                       \
    thread/link_metrics_tlvs.hpp                  \
    thread/link_quality.hpp                       \
    thread/lowpan.hpp                             \
    thread/mesh_forwarder.hpp                     \
    thread/mle.hpp                                \
    thread/mle_router.hpp                         \
    thread/mle_tlvs.hpp                           \
    thread/mle_types.hpp                          \
    thread/mlr_manager.hpp                        \
    thread/mlr_types.hpp                          \
    thread/neighbor_table.hpp                     \
    thread/network_data.hpp                       \
    thread/network_data_leader.hpp                \
    thread/network_data_leader_ftd.hpp            \
    thread/network_data_local.hpp                 \
    thread/network_data_notifier.hpp              \
    thread/network_data_publisher.hpp             \
    thread/network_data_service.hpp               \
    thread/network_data_tlvs.hpp                  \
    thread/network_data_types.hpp                 \
    thread/network_diagnostic.hpp                 \
    thread/network_diagnostic_tlvs.hpp            \
    thread/panid_query_server.hpp                 \
    thread/radio_selector.hpp                     \
    thread/router_table.hpp                       \
    thread/src_match_controller.hpp               \
    thread/thread_netif.hpp                       \
    thread/thread_tlvs.hpp                        \
    thread/time_sync_service.hpp                  \
    thread/tmf.hpp                                \
    thread/topology.hpp                           \
    thread/uri_paths.hpp                          \
    utils/channel_manager.hpp                     \
    utils/channel_monitor.hpp                     \
    utils/child_supervision.hpp                   \
    utils/flash.hpp                               \
    utils/heap.hpp                                \
    utils/history_tracker.hpp                     \
    utils/jam_detector.hpp                        \
    utils/lookup_table.hpp                        \
    utils/otns.hpp                                \
    utils/parse_cmdline.hpp                       \
    utils/ping_sender.hpp                         \
    utils/slaac_address.hpp                       \
    utils/srp_client_buffers.hpp                  \
    $(NULL)

noinst_HEADERS                             = \
    $(HEADERS_COMMON)                        \
    $(NULL)

if OPENTHREAD_BUILD_COVERAGE
Dash       = -
CLEANFILES = $(shell find $(top_builddir)/src/core $(Dash)name "*.gcda" $(Dash)o $(Dash)name "*.gcno")
endif

include $(abs_top_nlbuild_autotools_dir)/automake/post.am
