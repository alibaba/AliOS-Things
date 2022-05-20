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
include $(top_srcdir)/src/lib/common.am

EXTRA_DIST                                        = \
    example_vendor_hook.cpp                         \
    $(NULL)

# Pull in the sources that comprise the OpenThread NCP library.

lib_LIBRARIES                                     = $(NULL)

if OPENTHREAD_ENABLE_NCP
if OPENTHREAD_ENABLE_FTD
lib_LIBRARIES                                    += libopenthread-ncp-ftd.a
endif

if OPENTHREAD_ENABLE_MTD
lib_LIBRARIES                                    += libopenthread-ncp-mtd.a
endif
endif

if OPENTHREAD_ENABLE_RADIO_ONLY
lib_LIBRARIES                                    += libopenthread-rcp.a
endif

COMMON_CPPFLAGS                                   = \
    -I$(top_srcdir)/include                         \
    -I$(top_srcdir)/src                             \
    -I$(top_srcdir)/src/core                        \
    -I$(top_srcdir)/third_party                     \
    -D_GNU_SOURCE                                   \
    -DSPINEL_PLATFORM_HEADER=\"spinel_platform.h\"  \
    $(OPENTHREAD_TARGET_DEFINES)                    \
    $(NULL)

libopenthread_ncp_mtd_a_CPPFLAGS                         = \
    -DOPENTHREAD_RADIO=0                                   \
    -DOPENTHREAD_FTD=0                                     \
    -DOPENTHREAD_MTD=1                                     \
    -DOPENTHREAD_SPINEL_CONFIG_OPENTHREAD_MESSAGE_ENABLE=1 \
    $(COMMON_CPPFLAGS)                                     \
    $(NULL)

libopenthread_ncp_ftd_a_CPPFLAGS                         = \
    -DOPENTHREAD_RADIO=0                                   \
    -DOPENTHREAD_FTD=1                                     \
    -DOPENTHREAD_MTD=0                                     \
    -DOPENTHREAD_SPINEL_CONFIG_OPENTHREAD_MESSAGE_ENABLE=1 \
    $(COMMON_CPPFLAGS)                                     \
    $(NULL)

libopenthread_rcp_a_CPPFLAGS                             = \
    -DOPENTHREAD_RADIO=1                                   \
    -DOPENTHREAD_RADIO_CLI=0                               \
    -DOPENTHREAD_FTD=0                                     \
    -DOPENTHREAD_MTD=0                                     \
    -DOPENTHREAD_SPINEL_CONFIG_OPENTHREAD_MESSAGE_ENABLE=0 \
    $(COMMON_CPPFLAGS)                                     \
    $(NULL)

libopenthread_ncp_mtd_a_LIBADD                                                      = \
    $(call ot_list_objects,$(top_builddir)/src/lib/hdlc/libopenthread-hdlc.a)         \
    $(call ot_list_objects,$(top_builddir)/src/lib/spinel/libopenthread-spinel-ncp.a) \
    $(NULL)

libopenthread_ncp_ftd_a_LIBADD                                                      = \
    $(call ot_list_objects,$(top_builddir)/src/lib/hdlc/libopenthread-hdlc.a  )       \
    $(call ot_list_objects,$(top_builddir)/src/lib/spinel/libopenthread-spinel-ncp.a) \
    $(NULL)

libopenthread_rcp_a_LIBADD                                                          = \
    $(call ot_list_objects,$(top_builddir)/src/lib/hdlc/libopenthread-hdlc.a)         \
    $(call ot_list_objects,$(top_builddir)/src/lib/spinel/libopenthread-spinel-rcp.a) \
    $(NULL)

COMMON_SOURCES                                    = \
    changed_props_set.cpp                           \
    changed_props_set.hpp                           \
    ncp_base.cpp                                    \
    ncp_base.hpp                                    \
    ncp_base_ftd.cpp                                \
    ncp_base_mtd.cpp                                \
    ncp_base_radio.cpp                              \
    ncp_base_dispatcher.cpp                         \
    ncp_config.h                                    \
    ncp_spi.cpp                                     \
    ncp_spi.hpp                                     \
    ncp_hdlc.cpp                                    \
    ncp_hdlc.hpp                                    \
    $(NULL)

if OPENTHREAD_ENABLE_NCP_VENDOR_HOOK

.INTERMEDIATE: ncp_vendor_hook_temp.cpp
ncp_vendor_hook_temp.cpp: ${OPENTHREAD_NCP_VENDOR_HOOK_SOURCE}
	$(AM_V_GEN)cp $< $@

# "nodist_" prefix tells automake not to include in 'make dist'
nodist_libopenthread_ncp_mtd_a_SOURCES            = \
    ncp_vendor_hook_temp.cpp                        \
    $(NULL)

# "nodist_" prefix tells automake not to include in 'make dist'
nodist_libopenthread_ncp_ftd_a_SOURCES            = \
    ncp_vendor_hook_temp.cpp                        \
    $(NULL)

endif # if OPENTHREAD_ENABLE_NCP_VENDOR_HOOK

libopenthread_ncp_mtd_a_SOURCES                   = \
   $(COMMON_SOURCES)                                \
   $(NULL)

libopenthread_ncp_ftd_a_SOURCES                   = \
   $(COMMON_SOURCES)                                \
   $(NULL)

libopenthread_rcp_a_SOURCES                       = \
   $(COMMON_SOURCES)                                \
   $(NULL)

include_HEADERS                                   = \
    $(NULL)

include $(abs_top_nlbuild_autotools_dir)/automake/post.am
