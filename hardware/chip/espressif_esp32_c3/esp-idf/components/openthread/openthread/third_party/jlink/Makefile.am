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

include $(abs_top_nlbuild_autotools_dir)/automake/pre.am

# Do not enable -Wundef for jlink library
override CFLAGS                                      := $(filter-out -Wundef,$(CFLAGS))
override CXXFLAGS                                    := $(filter-out -Wundef,$(CXXFLAGS))

# Do not enable -Wcast-align for jlink library
override CFLAGS                                      := $(filter-out -Wcast-align,$(CFLAGS))
override CXXFLAGS                                    := $(filter-out -Wcast-align,$(CXXFLAGS))

lib_LIBRARIES                                             = libjlinkrtt.a

libjlinkrtt_a_CPPFLAGS                                    = \
    -I$(top_srcdir)/third_party/jlink/SEGGER_RTT_V640/RTT   \
    $(NULL)

libjlinkrtt_a_SOURCES                                     = \
    SEGGER_RTT_V640/RTT/SEGGER_RTT.c                        \
    $(NULL)

noinst_HEADERS                                            = \
    SEGGER_RTT_V640/RTT/SEGGER_RTT.h                        \
    SEGGER_RTT_V640/RTT/SEGGER_RTT_Conf.h                   \
    $(NULL)

include $(abs_top_nlbuild_autotools_dir)/automake/post.am
