#
#  Copyright (c) 2017, The OpenThread Authors.
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

bin_PROGRAMS                                              = \
    ot-cli-received-fuzzer                                  \
    ot-ip6-send-fuzzer                                      \
    ot-radio-receive-done-fuzzer                            \
    ot-ncp-hdlc-received-fuzzer                             \
    $(NULL)

AM_CPPFLAGS                                               = \
    -I$(top_srcdir)/include                                 \
    -I$(top_srcdir)/src/core                                \
    $(NULL)

COMMON_LDADD                                              = \
    $(top_builddir)/src/core/libopenthread-ftd.a            \
    $(top_builddir)/third_party/mbedtls/libmbedcrypto.a     \
    $(top_builddir)/third_party/tcplp/libtcplp.a            \
    $(LIB_FUZZING_ENGINE)                                   \
    $(NULL)

COMMON_SOURCES                                            = \
    fuzzer_platform.cpp                                     \
    fuzzer_platform.h                                       \
    $(NULL)

ot_cli_received_fuzzer_LDADD                              = \
    $(top_builddir)/src/cli/libopenthread-cli-ftd.a         \
    $(COMMON_LDADD)                                         \
    $(NULL)

ot_cli_received_fuzzer_SOURCES                            = \
    $(COMMON_SOURCES)                                       \
    cli_received.cpp                                        \
    $(NULL)

ot_ip6_send_fuzzer_LDADD                                  = \
    $(COMMON_LDADD)                                         \
    $(NULL)

ot_ip6_send_fuzzer_SOURCES                                = \
    $(COMMON_SOURCES)                                       \
    ip6_send.cpp                                            \
    $(NULL)

ot_radio_receive_done_fuzzer_LDADD                        = \
    $(COMMON_LDADD)                                         \
    $(NULL)

ot_radio_receive_done_fuzzer_SOURCES                      = \
    $(COMMON_SOURCES)                                       \
    radio_receive_done.cpp                                  \
    $(NULL)

ot_ncp_hdlc_received_fuzzer_LDADD                         = \
    $(top_builddir)/src/ncp/libopenthread-ncp-ftd.a         \
    $(COMMON_LDADD)                                         \
    $(NULL)

ot_ncp_hdlc_received_fuzzer_SOURCES                       = \
    $(COMMON_SOURCES)                                       \
    ncp_hdlc_received.cpp                                   \
    $(NULL)

include $(abs_top_nlbuild_autotools_dir)/automake/post.am
