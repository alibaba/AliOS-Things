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

lib_LIBRARIES                           = libopenthread-platform-utils.a
noinst_LTLIBRARIES                      = libutils-link-metrics.la

COMMON_FLAGS                                            = \
    -I$(top_srcdir)/include                               \
    -I$(top_srcdir)/examples/platforms                    \
    -I$(top_srcdir)/src/core                              \
    -I$(top_srcdir)/third_party/jlink/SEGGER_RTT_V640/RTT \
    $(NULL)

libutils_link_metrics_la_CPPFLAGS                       = \
    $(COMMON_FLAGS)                                       \
    -fno-threadsafe-statics                               \
    $(NULL)

libutils_link_metrics_la_SOURCES                        = \
    link_metrics.cpp                                      \
    link_metrics.h                                        \
    $(NULL)

libopenthread_platform_utils_a_CPPFLAGS                 = \
    $(COMMON_FLAGS)                                       \
    $(NULL)

libopenthread_platform_utils_a_SOURCES  = \
    code_utils.h                          \
    debug_uart.c                          \
    logging_rtt.c                         \
    logging_rtt.h                         \
    mac_frame.cpp                         \
    mac_frame.h                           \
    otns_utils.cpp                        \
    settings.h                            \
    settings_ram.c                        \
    soft_source_match_table.c             \
    soft_source_match_table.h             \
    uart.h                                \
    $(NULL)

libopenthread_platform_utils_a_LIBADD  = \
    libutils-link-metrics.la             \
    $(NULL)

include $(abs_top_nlbuild_autotools_dir)/automake/post.am
