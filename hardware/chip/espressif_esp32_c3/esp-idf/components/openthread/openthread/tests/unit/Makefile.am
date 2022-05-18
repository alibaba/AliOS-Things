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

#
# Local headers to build against and distribute but not to install
# since they are not part of the package.
#
noinst_HEADERS                                                      = \
    test_lowpan.hpp                                                   \
    test_platform.h                                                   \
    test_util.h                                                       \
    test_util.hpp                                                     \
    $(NULL)

#
# Other files we do want to distribute with the package.
#
EXTRA_DIST                                                          = \
    $(NULL)

if OPENTHREAD_BUILD_TESTS
# C preprocessor option flags that will apply to all compiled objects in this
# makefile.

AM_CPPFLAGS                                                         = \
    -DOPENTHREAD_FTD=1                                                \
    -DOPENTHREAD_MTD=0                                                \
    -DOPENTHREAD_RADIO=0                                              \
    -DOPENTHREAD_RADIO_CLI=0                                          \
    -DOPENTHREAD_SPINEL_CONFIG_OPENTHREAD_MESSAGE_ENABLE=1            \
    -I$(top_srcdir)/include                                           \
    -I$(top_srcdir)/src                                               \
    -I$(top_srcdir)/src/core                                          \
    $(NULL)

if OPENTHREAD_EXAMPLES_SIMULATION
AM_CPPFLAGS                                                        += \
    -I$(top_srcdir)/examples/platforms                                \
    $(NULL)
endif

if OPENTHREAD_PLATFORM_POSIX
AM_CPPFLAGS                                                        += \
    -DOPENTHREAD_PLATFORM_POSIX=1                                     \
    -I$(top_srcdir)/src/posix/platform                                \
    $(NULL)
endif

COMMON_LDADD                                                        = \
    $(top_builddir)/third_party/tcplp/libtcplp.a                      \
    $(NULL)

if OPENTHREAD_ENABLE_NCP
COMMON_LDADD                                                       += \
    $(top_builddir)/src/ncp/libopenthread-ncp-ftd.a                   \
    $(NULL)
endif

COMMON_LDADD                                                       += \
    $(top_builddir)/src/core/libopenthread-ftd.a                      \
    -lpthread                                                         \
    $(NULL)

if OPENTHREAD_ENABLE_BUILTIN_MBEDTLS
COMMON_LDADD                                                       += \
    $(top_builddir)/third_party/mbedtls/libmbedcrypto.a               \
    $(NULL)
endif

if OPENTHREAD_PLATFORM_POSIX
COMMON_LDADD                                                       += \
    -lutil
    $(NULL)
endif

# Test applications that should be run when the 'check' target is run.

check_PROGRAMS                                                      = \
    ot-test-toolchain                                                 \
    $(NULL)

if OPENTHREAD_ENABLE_FTD
check_PROGRAMS                                                     += \
    ot-test-aes                                                       \
    ot-test-array                                                     \
    ot-test-checksum                                                  \
    ot-test-child                                                     \
    ot-test-child-table                                               \
    ot-test-cmd-line-parser                                           \
    ot-test-data                                                      \
    ot-test-dns                                                       \
    ot-test-ecdsa                                                     \
    ot-test-flash                                                     \
    ot-test-heap                                                      \
    ot-test-heap-string                                               \
    ot-test-hkdf-sha256                                               \
    ot-test-hmac-sha256                                               \
    ot-test-ip-address                                                \
    ot-test-link-quality                                              \
    ot-test-linked-list                                               \
    ot-test-lookup-table                                              \
    ot-test-lowpan                                                    \
    ot-test-mac-frame                                                 \
    ot-test-macros                                                    \
    ot-test-message                                                   \
    ot-test-message-queue                                             \
    ot-test-multicast-listeners-table                                 \
    ot-test-ndproxy-table                                             \
    ot-test-netif                                                     \
    ot-test-network-data                                              \
    ot-test-pool                                                      \
    ot-test-priority-queue                                            \
    ot-test-pskc                                                      \
    ot-test-steering-data                                             \
    ot-test-string                                                    \
    ot-test-timer                                                     \
    $(NULL)

if OPENTHREAD_ENABLE_NCP
check_PROGRAMS                                                     += \
    ot-test-hdlc                                                      \
    ot-test-spinel-buffer                                             \
    ot-test-spinel-decoder                                            \
    ot-test-spinel-encoder                                            \
    $(NULL)
endif
endif # OPENTHREAD_ENABLE_FTD

XFAIL_TESTS                                                         = \
    $(NULL)

if OPENTHREAD_WITH_ADDRESS_SANITIZER
check_PROGRAMS                    += ot-test-address-sanitizer
XFAIL_TESTS                       += ot-test-address-sanitizer
ot_test_address_sanitizer_SOURCES  = test_address_sanitizer.cpp
endif # OPENTHREAD_WITH_ADDRESS_SANITIZER


# Test applications and scripts that should be built and run when the
# 'check' target is run.

TESTS                                                               = \
    $(check_PROGRAMS)                                                 \
    $(NULL)

# The additional environment variables and their values that will be
# made available to all programs and scripts in TESTS.

TESTS_ENVIRONMENT                                                   = \
    top_srcdir='$(top_srcdir)'                                        \
    $(NULL)

COMMON_SOURCES               = test_platform.cpp test_util.cpp

# Source, compiler, and linker options for test programs.

ot_test_aes_LDADD               = $(COMMON_LDADD)
ot_test_aes_SOURCES             = $(COMMON_SOURCES) test_aes.cpp

ot_test_array_LDADD             = $(COMMON_LDADD)
ot_test_array_SOURCES           = $(COMMON_SOURCES) test_array.cpp

ot_test_checksum_LDADD          = $(COMMON_LDADD)
ot_test_checksum_SOURCES        = $(COMMON_SOURCES) test_checksum.cpp

ot_test_child_LDADD             = $(COMMON_LDADD)
ot_test_child_SOURCES           = $(COMMON_SOURCES) test_child.cpp

ot_test_child_table_LDADD       = $(COMMON_LDADD)
ot_test_child_table_SOURCES     = $(COMMON_SOURCES) test_child_table.cpp

ot_test_cmd_line_parser_LDADD   = $(COMMON_LDADD)
ot_test_cmd_line_parser_SOURCES = $(COMMON_SOURCES) test_cmd_line_parser.cpp

ot_test_data_LDADD              = $(COMMON_LDADD)
ot_test_data_SOURCES            = $(COMMON_SOURCES) test_data.cpp

ot_test_dns_LDADD               = $(COMMON_LDADD)
ot_test_dns_SOURCES             = $(COMMON_SOURCES) test_dns.cpp

ot_test_ecdsa_LDADD             = $(COMMON_LDADD)
ot_test_ecdsa_SOURCES           = $(COMMON_SOURCES) test_ecdsa.cpp

ot_test_flash_LDADD             = $(COMMON_LDADD)
ot_test_flash_SOURCES           = $(COMMON_SOURCES) test_flash.cpp

ot_test_hdlc_LDADD              = $(COMMON_LDADD)
ot_test_hdlc_SOURCES            = $(COMMON_SOURCES) test_hdlc.cpp

ot_test_heap_LDADD              = $(COMMON_LDADD)
ot_test_heap_SOURCES            = $(COMMON_SOURCES) test_heap.cpp

ot_test_heap_string_LDADD       = $(COMMON_LDADD)
ot_test_heap_string_SOURCES     = $(COMMON_SOURCES) test_heap_string.cpp

ot_test_hkdf_sha256_LDADD       = $(COMMON_LDADD)
ot_test_hkdf_sha256_SOURCES     = $(COMMON_SOURCES) test_hkdf_sha256.cpp

ot_test_hmac_sha256_LDADD       = $(COMMON_LDADD)
ot_test_hmac_sha256_SOURCES     = $(COMMON_SOURCES) test_hmac_sha256.cpp

ot_test_ip_address_LDADD        = $(COMMON_LDADD)
ot_test_ip_address_SOURCES      = $(COMMON_SOURCES) test_ip_address.cpp

ot_test_link_quality_LDADD      = $(COMMON_LDADD)
ot_test_link_quality_SOURCES    = $(COMMON_SOURCES) test_link_quality.cpp

ot_test_linked_list_LDADD       = $(COMMON_LDADD)
ot_test_linked_list_SOURCES     = $(COMMON_SOURCES) test_linked_list.cpp

ot_test_lookup_table_LDADD      = $(COMMON_LDADD)
ot_test_lookup_table_SOURCES    = $(COMMON_SOURCES) test_lookup_table.cpp

ot_test_lowpan_LDADD            = $(COMMON_LDADD)
ot_test_lowpan_SOURCES          = $(COMMON_SOURCES) test_lowpan.cpp

ot_test_mac_frame_LDADD         = $(COMMON_LDADD)
ot_test_mac_frame_SOURCES       = $(COMMON_SOURCES) test_mac_frame.cpp

ot_test_macros_LDADD            = $(COMMON_LDADD)
ot_test_macros_SOURCES          = $(COMMON_SOURCES) test_macros.cpp

ot_test_message_LDADD           = $(COMMON_LDADD)
ot_test_message_SOURCES         = $(COMMON_SOURCES) test_message.cpp

ot_test_message_queue_LDADD     = $(COMMON_LDADD)
ot_test_message_queue_SOURCES   = $(COMMON_SOURCES) test_message_queue.cpp

ot_test_multicast_listeners_table_LDADD   = $(COMMON_LDADD)
ot_test_multicast_listeners_table_SOURCES = $(COMMON_SOURCES) test_multicast_listeners_table.cpp

ot_test_spinel_buffer_LDADD     = $(COMMON_LDADD)
ot_test_spinel_buffer_SOURCES   = $(COMMON_SOURCES) test_spinel_buffer.cpp

ot_test_ndproxy_table_LDADD     = $(COMMON_LDADD)
ot_test_ndproxy_table_SOURCES   = $(COMMON_SOURCES) test_ndproxy_table.cpp

ot_test_netif_LDADD             = $(COMMON_LDADD)
ot_test_netif_SOURCES           = $(COMMON_SOURCES) test_netif.cpp

ot_test_network_data_LDADD      = $(COMMON_LDADD)
ot_test_network_data_SOURCES    = $(COMMON_SOURCES) test_network_data.cpp

ot_test_pool_LDADD              = $(COMMON_LDADD)
ot_test_pool_SOURCES            = $(COMMON_SOURCES) test_pool.cpp

ot_test_priority_queue_LDADD    = $(COMMON_LDADD)
ot_test_priority_queue_SOURCES  = $(COMMON_SOURCES) test_priority_queue.cpp

ot_test_pskc_LDADD              = $(COMMON_LDADD)
ot_test_pskc_SOURCES            = $(COMMON_SOURCES) test_pskc.cpp

ot_test_steering_data_LDADD     = $(COMMON_LDADD)
ot_test_steering_data_SOURCES   = $(COMMON_SOURCES) test_steering_data.cpp

ot_test_string_LDADD            = $(COMMON_LDADD)
ot_test_string_SOURCES          = $(COMMON_SOURCES) test_string.cpp

ot_test_spinel_decoder_LDADD    = $(COMMON_LDADD)
ot_test_spinel_decoder_SOURCES  = $(COMMON_SOURCES) test_spinel_decoder.cpp

ot_test_spinel_encoder_LDADD    = $(COMMON_LDADD)
ot_test_spinel_encoder_SOURCES  = $(COMMON_SOURCES) test_spinel_encoder.cpp

ot_test_timer_LDADD             = $(COMMON_LDADD)
ot_test_timer_SOURCES           = $(COMMON_SOURCES) test_timer.cpp

ot_test_toolchain_LDADD         = $(NULL)
ot_test_toolchain_SOURCES       = test_toolchain.cpp test_toolchain_c.c

if OPENTHREAD_BUILD_COVERAGE
CLEANFILES                   = $(wildcard *.gcda *.gcno)
endif # OPENTHREAD_BUILD_COVERAGE

endif # OPENTHREAD_BUILD_TESTS

include $(abs_top_nlbuild_autotools_dir)/automake/post.am
