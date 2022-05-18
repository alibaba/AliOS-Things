#
#  Copyright (c) 2018, The OpenThread Authors.
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

LOCAL_PATH := $(call my-dir)

ifeq ($(OPENTHREAD_ENABLE_ANDROID_MK),1)

OPENTHREAD_DEFAULT_VERSION := $(shell cat $(LOCAL_PATH)/.default-version)
OPENTHREAD_SOURCE_VERSION := $(shell git -C $(LOCAL_PATH) describe --always --match "[0-9].*" 2> /dev/null)

OPENTHREAD_PROJECT_CFLAGS                                                 ?= \
    -DOPENTHREAD_PROJECT_CORE_CONFIG_FILE=\"openthread-core-posix-config.h\" \
    -DOPENTHREAD_CONFIG_FILE=\<openthread-config-android.h\>                 \
    $(NULL)

OPENTHREAD_PUBLIC_CFLAGS                                         := \
    -DOPENTHREAD_CONFIG_PING_SENDER_ENABLE=1                        \
    -DOPENTHREAD_CONFIG_COMMISSIONER_ENABLE=1                       \
    -DOPENTHREAD_CONFIG_IP6_SLAAC_ENABLE=1                          \
    -DOPENTHREAD_CONFIG_LOG_LEVEL_DYNAMIC_ENABLE=1                  \
    -DOPENTHREAD_CONFIG_MAC_FILTER_ENABLE=1                         \
    -DOPENTHREAD_POSIX_CONFIG_RCP_PTY_ENABLE=1                      \
    -DOPENTHREAD_FTD=1                                              \
    -DOPENTHREAD_SPINEL_CONFIG_OPENTHREAD_MESSAGE_ENABLE=1          \
    $(NULL)

OPENTHREAD_PRIVATE_CFLAGS                                        := \
    -DMBEDTLS_CONFIG_FILE=\"mbedtls-config.h\"                      \
    -DPACKAGE=\"openthread\"                                        \
    -DPACKAGE_BUGREPORT=\"openthread-devel@googlegroups.com\"       \
    -DPACKAGE_NAME=\"OPENTHREAD\"                                   \
    -DPACKAGE_STRING=\"OPENTHREAD\ $(OPENTHREAD_DEFAULT_VERSION)\"  \
    -DPACKAGE_TARNAME=\"openthread\"                                \
    -DPACKAGE_URL=\"http://github.com/openthread/openthread\"       \
    -DPACKAGE_VERSION=\"$(OPENTHREAD_SOURCE_VERSION)\"              \
    -DSPINEL_PLATFORM_HEADER=\"spinel_platform.h\"                  \
    -DVERSION=\"$(OPENTHREAD_DEFAULT_VERSION)\"                     \
    $(NULL)

# Enable required features for on-device tests.
ifeq ($(TARGET_BUILD_VARIANT),eng)
OPENTHREAD_PUBLIC_CFLAGS                                         += \
    -DOPENTHREAD_CONFIG_DIAG_ENABLE=1                               \
    $(NULL)
endif

ifeq ($(USE_OTBR_DAEMON), 1)
OPENTHREAD_PUBLIC_CFLAGS                                         += \
    -DOPENTHREAD_CONFIG_PLATFORM_NETIF_ENABLE=1                     \
    -DOPENTHREAD_CONFIG_PLATFORM_UDP_ENABLE=1                       \
    -DOPENTHREAD_CONFIG_UNSECURE_TRAFFIC_MANAGED_BY_STACK_ENABLE=1  \
    -DOPENTHREAD_POSIX_CONFIG_DAEMON_ENABLE=1                       \
    $(NULL)
else
OPENTHREAD_PUBLIC_CFLAGS += -DOPENTHREAD_CONFIG_UDP_FORWARD_ENABLE=1
endif

ifeq ($(USE_OT_RCP_BUS), spi)
OPENTHREAD_PUBLIC_CFLAGS += -DOPENTHREAD_POSIX_CONFIG_RCP_BUS=OT_POSIX_RCP_BUS_SPI
else
OPENTHREAD_PUBLIC_CFLAGS += -DOPENTHREAD_POSIX_CONFIG_RCP_BUS=OT_POSIX_RCP_BUS_UART
endif

# Enable all optional features for CI tests.
ifeq ($(TARGET_PRODUCT),generic)
OPENTHREAD_PUBLIC_CFLAGS                                         += \
    -DOPENTHREAD_CONFIG_COAP_API_ENABLE=1                           \
    -DOPENTHREAD_CONFIG_DHCP6_CLIENT_ENABLE=1                       \
    -DOPENTHREAD_CONFIG_DHCP6_SERVER_ENABLE=1                       \
    -DOPENTHREAD_CONFIG_DNS_CLIENT_ENABLE=1                         \
    -DOPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE=0                   \
    -DOPENTHREAD_CONFIG_TMF_NETWORK_DIAG_MTD_ENABLE=1               \
    $(NULL)
endif

include $(CLEAR_VARS)

LOCAL_MODULE := spi-hdlc-adapter
LOCAL_MODULE_TAGS := eng
LOCAL_SRC_FILES := tools/spi-hdlc-adapter/spi-hdlc-adapter.c

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_MODULE := ot-core
LOCAL_MODULE_TAGS := eng

LOCAL_C_INCLUDES                                         := \
    $(OPENTHREAD_PROJECT_INCLUDES)                          \
    $(LOCAL_PATH)/include                                   \
    $(LOCAL_PATH)/src                                       \
    $(LOCAL_PATH)/src/cli                                   \
    $(LOCAL_PATH)/src/core                                  \
    $(LOCAL_PATH)/src/ncp                                   \
    $(LOCAL_PATH)/src/posix/platform                        \
    $(LOCAL_PATH)/src/posix/platform/include                \
    $(LOCAL_PATH)/third_party                               \
    $(LOCAL_PATH)/third_party/mbedtls                       \
    $(LOCAL_PATH)/third_party/mbedtls/repo/include          \
    $(LOCAL_PATH)/third_party/mbedtls/repo/library          \
    $(NULL)

LOCAL_CFLAGS                                                                := \
    $(OPENTHREAD_PUBLIC_CFLAGS)                                                \
    $(OPENTHREAD_PRIVATE_CFLAGS)                                               \
    $(OPENTHREAD_PROJECT_CFLAGS)                                               \
    $(NULL)

LOCAL_EXPORT_CFLAGS                                        := \
    $(OPENTHREAD_PUBLIC_CFLAGS)                               \
    $(OPENTHREAD_PROJECT_CFLAGS)                              \
    $(NULL)

LOCAL_EXPORT_C_INCLUDE_DIRS     := \
    $(OPENTHREAD_PROJECT_INCLUDES) \
    $(LOCAL_PATH)/include          \
    $(LOCAL_PATH)/src              \
    $(NULL)

LOCAL_CPPFLAGS                                                              := \
    -std=c++11                                                                 \
    -Wno-error=non-virtual-dtor                                                \
    -pedantic-errors                                                           \
    $(NULL)

ifeq ($(ANDROID_NDK),1)
LOCAL_SHARED_LIBRARIES := libcutils

LOCAL_CFLAGS                                             += \
    -DOPENTHREAD_ENABLE_ANDROID_NDK=1                       \
    $(NULL)
endif

LOCAL_SRC_FILES                                                  := \
    src/core/api/backbone_router_api.cpp                            \
    src/core/api/backbone_router_ftd_api.cpp                        \
    src/core/api/border_agent_api.cpp                               \
    src/core/api/border_router_api.cpp                              \
    src/core/api/channel_manager_api.cpp                            \
    src/core/api/channel_monitor_api.cpp                            \
    src/core/api/child_supervision_api.cpp                          \
    src/core/api/coap_api.cpp                                       \
    src/core/api/coap_secure_api.cpp                                \
    src/core/api/commissioner_api.cpp                               \
    src/core/api/crypto_api.cpp                                     \
    src/core/api/dataset_api.cpp                                    \
    src/core/api/dataset_ftd_api.cpp                                \
    src/core/api/dataset_updater_api.cpp                            \
    src/core/api/diags_api.cpp                                      \
    src/core/api/dns_api.cpp                                        \
    src/core/api/dns_server_api.cpp                                 \
    src/core/api/entropy_api.cpp                                    \
    src/core/api/error_api.cpp                                      \
    src/core/api/heap_api.cpp                                       \
    src/core/api/history_tracker_api.cpp                            \
    src/core/api/icmp6_api.cpp                                      \
    src/core/api/instance_api.cpp                                   \
    src/core/api/ip6_api.cpp                                        \
    src/core/api/jam_detection_api.cpp                              \
    src/core/api/joiner_api.cpp                                     \
    src/core/api/link_api.cpp                                       \
    src/core/api/link_metrics_api.cpp                               \
    src/core/api/link_raw_api.cpp                                   \
    src/core/api/logging_api.cpp                                    \
    src/core/api/message_api.cpp                                    \
    src/core/api/multi_radio_api.cpp                                \
    src/core/api/netdata_api.cpp                                    \
    src/core/api/netdata_publisher_api.cpp                          \
    src/core/api/netdiag_api.cpp                                    \
    src/core/api/network_time_api.cpp                               \
    src/core/api/ping_sender_api.cpp                                \
    src/core/api/random_crypto_api.cpp                              \
    src/core/api/random_noncrypto_api.cpp                           \
    src/core/api/server_api.cpp                                     \
    src/core/api/sntp_api.cpp                                       \
    src/core/api/srp_client_api.cpp                                 \
    src/core/api/srp_client_buffers_api.cpp                         \
    src/core/api/srp_server_api.cpp                                 \
    src/core/api/tasklet_api.cpp                                    \
    src/core/api/tcp_api.cpp                                        \
    src/core/api/thread_api.cpp                                     \
    src/core/api/thread_ftd_api.cpp                                 \
    src/core/api/udp_api.cpp                                        \
    src/core/backbone_router/backbone_tmf.cpp                       \
    src/core/backbone_router/bbr_leader.cpp                         \
    src/core/backbone_router/bbr_local.cpp                          \
    src/core/backbone_router/bbr_manager.cpp                        \
    src/core/backbone_router/multicast_listeners_table.cpp          \
    src/core/backbone_router/ndproxy_table.cpp                      \
    src/core/border_router/infra_if_platform.cpp                    \
    src/core/border_router/router_advertisement.cpp                 \
    src/core/border_router/routing_manager.cpp                      \
    src/core/coap/coap.cpp                                          \
    src/core/coap/coap_message.cpp                                  \
    src/core/coap/coap_secure.cpp                                   \
    src/core/common/crc16.cpp                                       \
    src/core/common/error.cpp                                       \
    src/core/common/heap_string.cpp                                 \
    src/core/common/instance.cpp                                    \
    src/core/common/logging.cpp                                     \
    src/core/common/message.cpp                                     \
    src/core/common/notifier.cpp                                    \
    src/core/common/random_manager.cpp                              \
    src/core/common/settings.cpp                                    \
    src/core/common/string.cpp                                      \
    src/core/common/tasklet.cpp                                     \
    src/core/common/time_ticker.cpp                                 \
    src/core/common/timer.cpp                                       \
    src/core/common/tlvs.cpp                                        \
    src/core/common/trickle_timer.cpp                               \
    src/core/common/uptime.cpp                                      \
    src/core/crypto/aes_ccm.cpp                                     \
    src/core/crypto/aes_ecb.cpp                                     \
    src/core/crypto/crypto_platform.cpp                             \
    src/core/crypto/ecdsa.cpp                                       \
    src/core/crypto/hkdf_sha256.cpp                                 \
    src/core/crypto/hmac_sha256.cpp                                 \
    src/core/crypto/mbedtls.cpp                                     \
    src/core/crypto/pbkdf2_cmac.cpp                                 \
    src/core/crypto/sha256.cpp                                      \
    src/core/crypto/storage.cpp                                     \
    src/core/diags/factory_diags.cpp                                \
    src/core/mac/channel_mask.cpp                                   \
    src/core/mac/data_poll_handler.cpp                              \
    src/core/mac/data_poll_sender.cpp                               \
    src/core/mac/link_raw.cpp                                       \
    src/core/mac/mac.cpp                                            \
    src/core/mac/mac_filter.cpp                                     \
    src/core/mac/mac_frame.cpp                                      \
    src/core/mac/mac_links.cpp                                      \
    src/core/mac/mac_types.cpp                                      \
    src/core/mac/sub_mac.cpp                                        \
    src/core/mac/sub_mac_callbacks.cpp                              \
    src/core/meshcop/announce_begin_client.cpp                      \
    src/core/meshcop/border_agent.cpp                               \
    src/core/meshcop/commissioner.cpp                               \
    src/core/meshcop/dataset.cpp                                    \
    src/core/meshcop/dataset_local.cpp                              \
    src/core/meshcop/dataset_manager.cpp                            \
    src/core/meshcop/dataset_manager_ftd.cpp                        \
    src/core/meshcop/dataset_updater.cpp                            \
    src/core/meshcop/dtls.cpp                                       \
    src/core/meshcop/energy_scan_client.cpp                         \
    src/core/meshcop/joiner.cpp                                     \
    src/core/meshcop/joiner_router.cpp                              \
    src/core/meshcop/meshcop.cpp                                    \
    src/core/meshcop/meshcop_leader.cpp                             \
    src/core/meshcop/meshcop_tlvs.cpp                               \
    src/core/meshcop/panid_query_client.cpp                         \
    src/core/meshcop/timestamp.cpp                                  \
    src/core/net/checksum.cpp                                       \
    src/core/net/dhcp6_client.cpp                                   \
    src/core/net/dhcp6_server.cpp                                   \
    src/core/net/dns_client.cpp                                     \
    src/core/net/dns_types.cpp                                      \
    src/core/net/dnssd_server.cpp                                   \
    src/core/net/icmp6.cpp                                          \
    src/core/net/ip4_address.cpp                                    \
    src/core/net/ip6.cpp                                            \
    src/core/net/ip6_address.cpp                                    \
    src/core/net/ip6_filter.cpp                                     \
    src/core/net/ip6_headers.cpp                                    \
    src/core/net/ip6_mpl.cpp                                        \
    src/core/net/nd_agent.cpp                                       \
    src/core/net/netif.cpp                                          \
    src/core/net/sntp_client.cpp                                    \
    src/core/net/socket.cpp                                         \
    src/core/net/srp_client.cpp                                     \
    src/core/net/srp_server.cpp                                     \
    src/core/net/tcp6.cpp                                           \
    src/core/net/udp6.cpp                                           \
    src/core/radio/radio.cpp                                        \
    src/core/radio/radio_callbacks.cpp                              \
    src/core/radio/radio_platform.cpp                               \
    src/core/radio/trel_interface.cpp                               \
    src/core/radio/trel_link.cpp                                    \
    src/core/radio/trel_packet.cpp                                  \
    src/core/thread/address_resolver.cpp                            \
    src/core/thread/announce_begin_server.cpp                       \
    src/core/thread/announce_sender.cpp                             \
    src/core/thread/anycast_locator.cpp                             \
    src/core/thread/child_table.cpp                                 \
    src/core/thread/csl_tx_scheduler.cpp                            \
    src/core/thread/discover_scanner.cpp                            \
    src/core/thread/dua_manager.cpp                                 \
    src/core/thread/energy_scan_server.cpp                          \
    src/core/thread/indirect_sender.cpp                             \
    src/core/thread/key_manager.cpp                                 \
    src/core/thread/link_metrics.cpp                                \
    src/core/thread/link_quality.cpp                                \
    src/core/thread/lowpan.cpp                                      \
    src/core/thread/mesh_forwarder.cpp                              \
    src/core/thread/mesh_forwarder_ftd.cpp                          \
    src/core/thread/mesh_forwarder_mtd.cpp                          \
    src/core/thread/mle.cpp                                         \
    src/core/thread/mle_router.cpp                                  \
    src/core/thread/mle_types.cpp                                   \
    src/core/thread/mlr_manager.cpp                                 \
    src/core/thread/neighbor_table.cpp                              \
    src/core/thread/network_data.cpp                                \
    src/core/thread/network_data_leader.cpp                         \
    src/core/thread/network_data_leader_ftd.cpp                     \
    src/core/thread/network_data_local.cpp                          \
    src/core/thread/network_data_notifier.cpp                       \
    src/core/thread/network_data_publisher.cpp                      \
    src/core/thread/network_data_service.cpp                        \
    src/core/thread/network_data_tlvs.cpp                           \
    src/core/thread/network_data_types.cpp                          \
    src/core/thread/network_diagnostic.cpp                          \
    src/core/thread/panid_query_server.cpp                          \
    src/core/thread/radio_selector.cpp                              \
    src/core/thread/router_table.cpp                                \
    src/core/thread/src_match_controller.cpp                        \
    src/core/thread/thread_netif.cpp                                \
    src/core/thread/time_sync_service.cpp                           \
    src/core/thread/tmf.cpp                                         \
    src/core/thread/topology.cpp                                    \
    src/core/thread/uri_paths.cpp                                   \
    src/core/utils/channel_manager.cpp                              \
    src/core/utils/channel_monitor.cpp                              \
    src/core/utils/child_supervision.cpp                            \
    src/core/utils/flash.cpp                                        \
    src/core/utils/heap.cpp                                         \
    src/core/utils/history_tracker.cpp                              \
    src/core/utils/jam_detector.cpp                                 \
    src/core/utils/lookup_table.cpp                                 \
    src/core/utils/otns.cpp                                         \
    src/core/utils/parse_cmdline.cpp                                \
    src/core/utils/ping_sender.cpp                                  \
    src/core/utils/slaac_address.cpp                                \
    src/core/utils/srp_client_buffers.cpp                           \
    src/lib/hdlc/hdlc.cpp                                           \
    src/lib/platform/exit_code.c                                    \
    src/lib/spinel/spinel.c                                         \
    src/lib/spinel/spinel_decoder.cpp                               \
    src/lib/spinel/spinel_encoder.cpp                               \
    src/lib/url/url.cpp                                             \
    src/posix/platform/alarm.cpp                                    \
    src/posix/platform/backbone.cpp                                 \
    src/posix/platform/daemon.cpp                                   \
    src/posix/platform/entropy.cpp                                  \
    src/posix/platform/hdlc_interface.cpp                           \
    src/posix/platform/infra_if.cpp                                 \
    src/posix/platform/logging.cpp                                  \
    src/posix/platform/mainloop.cpp                                 \
    src/posix/platform/memory.cpp                                   \
    src/posix/platform/misc.cpp                                     \
    src/posix/platform/multicast_routing.cpp                        \
    src/posix/platform/netif.cpp                                    \
    src/posix/platform/radio.cpp                                    \
    src/posix/platform/radio_url.cpp                                \
    src/posix/platform/settings.cpp                                 \
    src/posix/platform/spi_interface.cpp                            \
    src/posix/platform/system.cpp                                   \
    src/posix/platform/trel_udp6.cpp                                \
    src/posix/platform/udp.cpp                                      \
    third_party/mbedtls/repo/library/aes.c                          \
    third_party/mbedtls/repo/library/aesni.c                        \
    third_party/mbedtls/repo/library/arc4.c                         \
    third_party/mbedtls/repo/library/aria.c                         \
    third_party/mbedtls/repo/library/asn1parse.c                    \
    third_party/mbedtls/repo/library/asn1write.c                    \
    third_party/mbedtls/repo/library/base64.c                       \
    third_party/mbedtls/repo/library/bignum.c                       \
    third_party/mbedtls/repo/library/blowfish.c                     \
    third_party/mbedtls/repo/library/camellia.c                     \
    third_party/mbedtls/repo/library/ccm.c                          \
    third_party/mbedtls/repo/library/certs.c                        \
    third_party/mbedtls/repo/library/chacha20.c                     \
    third_party/mbedtls/repo/library/chachapoly.c                   \
    third_party/mbedtls/repo/library/cipher.c                       \
    third_party/mbedtls/repo/library/cipher_wrap.c                  \
    third_party/mbedtls/repo/library/cmac.c                         \
    third_party/mbedtls/repo/library/ctr_drbg.c                     \
    third_party/mbedtls/repo/library/debug.c                        \
    third_party/mbedtls/repo/library/des.c                          \
    third_party/mbedtls/repo/library/dhm.c                          \
    third_party/mbedtls/repo/library/ecdh.c                         \
    third_party/mbedtls/repo/library/ecdsa.c                        \
    third_party/mbedtls/repo/library/ecjpake.c                      \
    third_party/mbedtls/repo/library/ecp.c                          \
    third_party/mbedtls/repo/library/ecp_curves.c                   \
    third_party/mbedtls/repo/library/entropy.c                      \
    third_party/mbedtls/repo/library/entropy_poll.c                 \
    third_party/mbedtls/repo/library/error.c                        \
    third_party/mbedtls/repo/library/gcm.c                          \
    third_party/mbedtls/repo/library/havege.c                       \
    third_party/mbedtls/repo/library/hkdf.c                         \
    third_party/mbedtls/repo/library/hmac_drbg.c                    \
    third_party/mbedtls/repo/library/md.c                           \
    third_party/mbedtls/repo/library/md2.c                          \
    third_party/mbedtls/repo/library/md4.c                          \
    third_party/mbedtls/repo/library/md5.c                          \
    third_party/mbedtls/repo/library/memory_buffer_alloc.c          \
    third_party/mbedtls/repo/library/net_sockets.c                  \
    third_party/mbedtls/repo/library/nist_kw.c                      \
    third_party/mbedtls/repo/library/oid.c                          \
    third_party/mbedtls/repo/library/padlock.c                      \
    third_party/mbedtls/repo/library/pem.c                          \
    third_party/mbedtls/repo/library/pk.c                           \
    third_party/mbedtls/repo/library/pk_wrap.c                      \
    third_party/mbedtls/repo/library/pkcs11.c                       \
    third_party/mbedtls/repo/library/pkcs12.c                       \
    third_party/mbedtls/repo/library/pkcs5.c                        \
    third_party/mbedtls/repo/library/pkparse.c                      \
    third_party/mbedtls/repo/library/pkwrite.c                      \
    third_party/mbedtls/repo/library/platform.c                     \
    third_party/mbedtls/repo/library/platform_util.c                \
    third_party/mbedtls/repo/library/poly1305.c                     \
    third_party/mbedtls/repo/library/psa_crypto.c                   \
    third_party/mbedtls/repo/library/psa_crypto_driver_wrappers.c   \
    third_party/mbedtls/repo/library/psa_crypto_se.c                \
    third_party/mbedtls/repo/library/psa_crypto_slot_management.c   \
    third_party/mbedtls/repo/library/psa_crypto_storage.c           \
    third_party/mbedtls/repo/library/psa_its_file.c                 \
    third_party/mbedtls/repo/library/ripemd160.c                    \
    third_party/mbedtls/repo/library/rsa.c                          \
    third_party/mbedtls/repo/library/rsa_internal.c                 \
    third_party/mbedtls/repo/library/sha1.c                         \
    third_party/mbedtls/repo/library/sha256.c                       \
    third_party/mbedtls/repo/library/sha512.c                       \
    third_party/mbedtls/repo/library/ssl_cache.c                    \
    third_party/mbedtls/repo/library/ssl_ciphersuites.c             \
    third_party/mbedtls/repo/library/ssl_cli.c                      \
    third_party/mbedtls/repo/library/ssl_cookie.c                   \
    third_party/mbedtls/repo/library/ssl_msg.c                      \
    third_party/mbedtls/repo/library/ssl_srv.c                      \
    third_party/mbedtls/repo/library/ssl_ticket.c                   \
    third_party/mbedtls/repo/library/ssl_tls.c                      \
    third_party/mbedtls/repo/library/ssl_tls13_keys.c               \
    third_party/mbedtls/repo/library/threading.c                    \
    third_party/mbedtls/repo/library/timing.c                       \
    third_party/mbedtls/repo/library/version.c                      \
    third_party/mbedtls/repo/library/version_features.c             \
    third_party/mbedtls/repo/library/x509.c                         \
    third_party/mbedtls/repo/library/x509_create.c                  \
    third_party/mbedtls/repo/library/x509_crl.c                     \
    third_party/mbedtls/repo/library/x509_crt.c                     \
    third_party/mbedtls/repo/library/x509_csr.c                     \
    third_party/mbedtls/repo/library/x509write_crt.c                \
    third_party/mbedtls/repo/library/x509write_csr.c                \
    third_party/mbedtls/repo/library/xtea.c                         \
    $(OPENTHREAD_PROJECT_SRC_FILES)                                 \
    $(NULL)

include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := libopenthread-cli
LOCAL_MODULE_TAGS := eng

LOCAL_C_INCLUDES                                         := \
    $(OPENTHREAD_PROJECT_INCLUDES)                          \
    $(LOCAL_PATH)/include                                   \
    $(LOCAL_PATH)/src                                       \
    $(LOCAL_PATH)/src/cli                                   \
    $(LOCAL_PATH)/src/core                                  \
    $(LOCAL_PATH)/src/posix/platform                        \
    $(LOCAL_PATH)/src/posix/platform/include                \
    $(LOCAL_PATH)/third_party/mbedtls                       \
    $(LOCAL_PATH)/third_party/mbedtls/repo/include          \
    $(NULL)

LOCAL_CFLAGS                                                                := \
    $(OPENTHREAD_PUBLIC_CFLAGS)                                                \
    $(OPENTHREAD_PRIVATE_CFLAGS)                                               \
    $(OPENTHREAD_PROJECT_CFLAGS)                                               \
    $(NULL)

LOCAL_CPPFLAGS                                                              := \
    -std=c++11                                                                 \
    -pedantic-errors                                                           \
    $(NULL)

LOCAL_SRC_FILES                            := \
    src/cli/cli.cpp                           \
    src/cli/cli_coap.cpp                      \
    src/cli/cli_coap_secure.cpp               \
    src/cli/cli_commissioner.cpp              \
    src/cli/cli_dataset.cpp                   \
    src/cli/cli_history.cpp                   \
    src/cli/cli_joiner.cpp                    \
    src/cli/cli_network_data.cpp              \
    src/cli/cli_output.cpp                    \
    src/cli/cli_srp_client.cpp                \
    src/cli/cli_srp_server.cpp                \
    src/cli/cli_tcp.cpp                       \
    src/cli/cli_udp.cpp                       \
    $(NULL)

include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := ot-cli
LOCAL_MODULE_TAGS := eng

ifneq ($(ANDROID_NDK),1)
LOCAL_SHARED_LIBRARIES := libcutils
endif

LOCAL_C_INCLUDES                                         := \
    $(OPENTHREAD_PROJECT_INCLUDES)                          \
    $(LOCAL_PATH)/include                                   \
    $(LOCAL_PATH)/src                                       \
    $(LOCAL_PATH)/src/cli                                   \
    $(LOCAL_PATH)/src/core                                  \
    $(LOCAL_PATH)/src/posix/platform                        \
    $(LOCAL_PATH)/src/posix/platform/include                \
    $(LOCAL_PATH)/third_party/mbedtls                       \
    $(LOCAL_PATH)/third_party/mbedtls/repo/include          \
    $(NULL)

LOCAL_CFLAGS                                                                := \
    $(OPENTHREAD_PUBLIC_CFLAGS)                                                \
    $(OPENTHREAD_PRIVATE_CFLAGS)                                               \
    $(OPENTHREAD_PROJECT_CFLAGS)                                               \
    $(NULL)

LOCAL_CPPFLAGS                                                              := \
    -std=c++11                                                                 \
    -pedantic-errors                                                           \
    $(NULL)

LOCAL_LDLIBS                               := \
    -lrt                                      \
    -lutil

LOCAL_SRC_FILES                            := \
    src/posix/cli_readline.cpp                \
    src/posix/cli_stdio.cpp                   \
    src/posix/main.c                          \
    $(NULL)

LOCAL_STATIC_LIBRARIES = libopenthread-cli ot-core
include $(BUILD_EXECUTABLE)

ifeq ($(USE_OTBR_DAEMON), 1)
include $(CLEAR_VARS)

LOCAL_MODULE := ot-ctl
LOCAL_MODULE_TAGS := eng

LOCAL_CPPFLAGS                                                              := \
    -std=c++11                                                                 \
    -pedantic-errors                                                           \
    $(NULL)

LOCAL_CFLAGS                                                                := \
    $(OPENTHREAD_PUBLIC_CFLAGS)                                                \
    $(OPENTHREAD_PRIVATE_CFLAGS)                                               \
    $(OPENTHREAD_PROJECT_CFLAGS)                                               \
    $(NULL)

LOCAL_C_INCLUDES                                         := \
    $(OPENTHREAD_PROJECT_INCLUDES)                          \
    $(LOCAL_PATH)/include                                   \
    $(LOCAL_PATH)/src/                                      \
    $(LOCAL_PATH)/src/core                                  \
    $(LOCAL_PATH)/src/posix/platform                        \
    $(LOCAL_PATH)/src/posix/platform/include                \
    $(NULL)

LOCAL_SRC_FILES := src/posix/client.cpp

include $(BUILD_EXECUTABLE)
endif # ($(USE_OTBR_DAEMON), 1)

ifneq ($(OPENTHREAD_PROJECT_ANDROID_MK),)
include $(OPENTHREAD_PROJECT_ANDROID_MK)
endif

endif # ($(OPENTHREAD_ENABLE_ANDROID_MK),1)
