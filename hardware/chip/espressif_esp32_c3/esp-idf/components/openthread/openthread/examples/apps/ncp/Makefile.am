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
include $(top_srcdir)/examples/platforms/Makefile.platform.am

bin_PROGRAMS                                                           = \
    $(NULL)

CPPFLAGS_COMMON                                                       += \
    -I$(top_srcdir)/include                                              \
    -I$(top_srcdir)/src                                                  \
    -I$(top_srcdir)/src/core                                             \
    -I$(top_srcdir)/examples/platforms                                   \
    $(NULL)

LDADD_COMMON                                                          += \
    $(top_builddir)/third_party/tcplp/libtcplp.a                         \
    $(NULL)

LDFLAGS_COMMON                                                        += \
    $(NULL)

LIBTOOLFLAGS_COMMON                                                   += \
    $(NULL)

SOURCES_COMMON                                                        += \
    main.c                                                               \
    ncp.c                                                                \
    $(NULL)

LDADD_MBEDTLS                                                          = \
    $(NULL)

LDADD_MBEDTLS_RADIO                                                    = \
    $(NULL)

LDADD_DIAG                                                             = \
    $(NULL)

if OPENTHREAD_ENABLE_BUILTIN_MBEDTLS
LDADD_MBEDTLS                                                         += \
    $(top_builddir)/third_party/mbedtls/libmbedcrypto.a                  \
    $(NULL)

LDADD_MBEDTLS_RADIO                                                   += \
    $(top_builddir)/third_party/mbedtls/libmbedcrypto-radio.a            \
    $(NULL)
endif # OPENTHREAD_ENABLE_BUILTIN_MBEDTLS

if OPENTHREAD_ENABLE_NCP_SPINEL_ENCRYPTER
LDADD_COMMON                                                          += \
    $(OPENTHREAD_NCP_SPINEL_ENCRYPTER_LIBS)                              \
    $(NULL)
endif # OPENTHREAD_ENABLE_NCP_SPINEL_ENCRYPTER

if OPENTHREAD_ENABLE_NCP
if OPENTHREAD_ENABLE_FTD
bin_PROGRAMS                                                          += \
    ot-ncp-ftd                                                           \
    $(NULL)
endif
endif

ot_ncp_ftd_CPPFLAGS                                                    = \
    $(CPPFLAGS_COMMON)                                                   \
    $(NULL)

ot_ncp_ftd_LDADD                                                       = \
    $(top_builddir)/src/ncp/libopenthread-ncp-ftd.a                      \
    $(top_builddir)/src/core/libopenthread-ftd.a                         \
    $(LDADD_COMMON)                                                      \
    $(LDADD_MBEDTLS)                                                     \
    $(LDADD_DIAG)                                                        \
    $(top_builddir)/src/core/libopenthread-ftd.a                         \
    $(LDADD_COMMON)                                                      \
    $(LDADD_MBEDTLS)                                                     \
    $(LDADD_DIAG)                                                        \
    $(NULL)

ot_ncp_ftd_LDFLAGS                                                     = \
    $(LDFLAGS_COMMON)                                                    \
    $(NULL)

ot_ncp_ftd_LIBTOOLFLAGS                                                = \
    $(LIBTOOLFLAGS_COMMON)                                               \
    $(NULL)

ot_ncp_ftd_SOURCES                                                     = \
    $(SOURCES_COMMON)                                                    \
    $(NULL)

if OPENTHREAD_ENABLE_NCP
if OPENTHREAD_ENABLE_MTD
bin_PROGRAMS                                                          += \
    ot-ncp-mtd                                                           \
    $(NULL)
endif
endif

ot_ncp_mtd_CPPFLAGS                                                    = \
    $(CPPFLAGS_COMMON)                                                   \
    $(NULL)

ot_ncp_mtd_LDADD                                                       = \
    $(top_builddir)/src/ncp/libopenthread-ncp-mtd.a                      \
    $(top_builddir)/src/core/libopenthread-mtd.a                         \
    $(LDADD_COMMON)                                                      \
    $(LDADD_MBEDTLS)                                                     \
    $(LDADD_DIAG)                                                        \
    $(top_builddir)/src/core/libopenthread-mtd.a                         \
    $(LDADD_COMMON)                                                      \
    $(LDADD_MBEDTLS)                                                     \
    $(LDADD_DIAG)                                                        \
    $(NULL)

ot_ncp_mtd_LDFLAGS                                                     = \
    $(LDFLAGS_COMMON)                                                    \
    $(NULL)

ot_ncp_mtd_LIBTOOLFLAGS                                                = \
    $(LIBTOOLFLAGS_COMMON)                                               \
    $(NULL)

ot_ncp_mtd_SOURCES                                                     = \
    $(SOURCES_COMMON)                                                    \
    $(NULL)

if OPENTHREAD_ENABLE_RADIO_ONLY
bin_PROGRAMS                                                          += \
    ot-rcp                                                               \
    $(NULL)
endif

ot_rcp_CPPFLAGS                                                        = \
    $(CPPFLAGS_COMMON)                                                   \
    $(NULL)

ot_rcp_LDADD                                                           = \
    $(top_builddir)/src/ncp/libopenthread-rcp.a                          \
    $(top_builddir)/src/core/libopenthread-radio.a                       \
    $(LDADD_COMMON)                                                      \
    $(LDADD_MBEDTLS_RADIO)                                               \
    $(top_builddir)/src/core/libopenthread-radio.a                       \
    $(LDADD_COMMON)                                                      \
    $(LDADD_MBEDTLS_RADIO)                                               \
    $(NULL)

ot_rcp_LDFLAGS                                                         = \
    $(LDFLAGS_COMMON)                                                    \
    $(NULL)

ot_rcp_LIBTOOLFLAGS                                                    = \
    $(LIBTOOLFLAGS_COMMON)                                               \
    $(NULL)

ot_rcp_SOURCES                                                         = \
    $(SOURCES_COMMON)                                                    \
    $(NULL)

if OPENTHREAD_ENABLE_LINKER_MAP
ot_ncp_ftd_LDFLAGS                                                    += -Wl,-Map=ot-ncp-ftd.map
ot_ncp_mtd_LDFLAGS                                                    += -Wl,-Map=ot-ncp-mtd.map
ot_rcp_LDFLAGS                                                        += -Wl,-Map=ot-rcp.map
endif

if OPENTHREAD_BUILD_COVERAGE
CPPFLAGS_COMMON                                                       += \
    -DOPENTHREAD_ENABLE_COVERAGE                                         \
    $(NULL)

CLEANFILES                                                             = $(wildcard *.gcda *.gcno)
endif # OPENTHREAD_BUILD_COVERAGE

include $(abs_top_nlbuild_autotools_dir)/automake/post.am
