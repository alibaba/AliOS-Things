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
    cli_uart.cpp                                                         \
    main.c                                                               \
    $(NULL)

if OPENTHREAD_ENABLE_BUILTIN_MBEDTLS
LDADD_COMMON                                                          += \
    $(top_builddir)/third_party/mbedtls/libmbedcrypto.a                  \
    $(NULL)
endif # OPENTHREAD_ENABLE_BUILTIN_MBEDTLS

if OPENTHREAD_ENABLE_FTD
bin_PROGRAMS                                                          += \
    ot-cli-ftd                                                           \
    $(NULL)
endif

ot_cli_ftd_CPPFLAGS                                                    = \
    $(CPPFLAGS_COMMON)                                                   \
    $(NULL)

ot_cli_ftd_LDADD                                                       = \
    $(top_builddir)/src/cli/libopenthread-cli-ftd.a                      \
    $(top_builddir)/src/core/libopenthread-ftd.a                         \
    $(LDADD_COMMON)                                                      \
    $(top_builddir)/src/core/libopenthread-ftd.a                         \
    $(LDADD_COMMON)                                                      \
    $(NULL)

ot_cli_ftd_LDFLAGS                                                     = \
    $(LDFLAGS_COMMON)                                                    \
    $(NULL)

ot_cli_ftd_LIBTOOLFLAGS                                                = \
    $(LIBTOOLFLAGS_COMMON)                                               \
    $(NULL)

ot_cli_ftd_SOURCES                                                     = \
    $(SOURCES_COMMON)                                                    \
    $(NULL)

if OPENTHREAD_ENABLE_MTD
bin_PROGRAMS                                                          += \
    ot-cli-mtd                                                           \
    $(NULL)
endif

ot_cli_mtd_CPPFLAGS                                                    = \
    $(CPPFLAGS_COMMON)                                                   \
    $(NULL)

ot_cli_mtd_LDADD                                                       = \
    $(top_builddir)/src/cli/libopenthread-cli-mtd.a                      \
    $(top_builddir)/src/core/libopenthread-mtd.a                         \
    $(LDADD_COMMON)                                                      \
    $(top_builddir)/src/core/libopenthread-mtd.a                         \
    $(LDADD_COMMON)                                                      \
    $(NULL)

ot_cli_mtd_LDFLAGS                                                     = \
    $(LDFLAGS_COMMON)                                                    \
    $(NULL)

ot_cli_mtd_LIBTOOLFLAGS                                                = \
    $(LIBTOOLFLAGS_COMMON)                                               \
    $(NULL)

ot_cli_mtd_SOURCES                                                     = \
    $(SOURCES_COMMON)                                                    \
    $(NULL)

if OPENTHREAD_ENABLE_RADIO_CLI
bin_PROGRAMS                                                            += \
    ot-cli-radio                                                           \
    $(NULL)
endif

ot_cli_radio_CPPFLAGS                                                    = \
    $(CPPFLAGS_COMMON)                                                     \
    $(NULL)

ot_cli_radio_LDADD                                                       = \
    $(top_builddir)/src/cli/libopenthread-cli-radio.a                      \
    $(top_builddir)/src/core/libopenthread-radio-cli.a                     \
    $(LDADD_COMMON)                                                        \
    $(top_builddir)/src/core/libopenthread-radio-cli.a                     \
    $(LDADD_COMMON)                                                        \
    $(NULL)

ot_cli_radio_LDFLAGS                                                     = \
    $(LDFLAGS_COMMON)                                                      \
    $(NULL)

ot_cli_radio_LIBTOOLFLAGS                                                = \
    $(LIBTOOLFLAGS_COMMON)                                                 \
    $(NULL)

ot_cli_radio_SOURCES                                                     = \
    $(SOURCES_COMMON)                                                      \
    $(NULL)

if OPENTHREAD_ENABLE_LINKER_MAP
ot_cli_ftd_LDFLAGS                                                    += -Wl,-Map=ot-cli-ftd.map
ot_cli_mtd_LDFLAGS                                                    += -Wl,-Map=ot-cli-mtd.map
ot_cli_radio_LDFLAGS                                                  += -Wl,-Map=ot-cli-radio.map
endif

if OPENTHREAD_BUILD_COVERAGE
CPPFLAGS_COMMON                                                       += \
    -DOPENTHREAD_ENABLE_COVERAGE                                         \
    $(NULL)

CLEANFILES                                                             = $(wildcard *.gcda *.gcno)
endif # OPENTHREAD_BUILD_COVERAGE

include $(abs_top_nlbuild_autotools_dir)/automake/post.am
