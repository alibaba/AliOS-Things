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

AM_MAKEFLAGS                        = --no-print-directory

AM_DISTCHECK_CONFIGURE_FLAGS        = \
    --enable-address-sanitizer        \
    --enable-cli                      \
    --enable-ftd                      \
    --enable-mtd                      \
    --enable-ncp                      \
    --enable-radio-only               \
    --enable-radio-cli                \
    --with-examples=simulation        \
    $(NULL)

SUBDIRS                             = \
    include                           \
    third_party                       \
    src                               \
    examples                          \
    tests                             \
    tools                             \
    doc                               \
    $(NULL)

EXTRA_DIST                          = \
    .default-version                  \
    bootstrap                         \
    etc                               \
    README.md                         \
    NOTICE                            \
    CONTRIBUTING.md                   \
    LICENSE                           \
    $(NULL)

BUILT_SOURCES                       = \
    .local-version                    \
    $(NULL)

dist_doc_DATA                       = \
    $(NULL)

DISTCLEANFILES                      = \
    .local-version                    \
    $(NULL)

# Ignore the pseudo flash files on Posix platform during diskcheck
distcleancheck_listfiles            = \
    $(AM_V_at)find . -type f -name "*flash"

#
# Package version files:
#
# .default-version - The default package version. This file is ALWAYS checked
#                    in and should always represent the current baseline
#                    version of the package.
#
# .dist-version    - The distributed package version. This file is NEVER
#                    checked in within the upstream repository, is auto-
#                    generated, and is only found in the package distribution.
#
# .local-version   - The current source code controlled package version. This
#                    file is NEVER checked in within the upstream repository,
#                    is auto-generated, and can always be found in both the
#                    build tree and distribution.
#
# When present, the .local-version file is preferred first, the
# .dist-version second, and the .default-version last.
#

VERSION_FILE                      := $(if $(wildcard $(builddir)/.local-version),$(builddir)/.local-version,$(if $(wildcard $(srcdir)/.dist-version),$(srcdir)/.dist-version,$(srcdir)/.default-version))

#
# Override autotool's default notion of the package version variables.
# This ensures that when we create a source distribution that the
# version is always the current version, not the version when the
# package was bootstrapped.
#

OPENTHREAD_VERSION                ?= $(shell cat $(VERSION_FILE) 2> /dev/null)

PACKAGE_VERSION                    = $(OPENTHREAD_VERSION)
VERSION                            = $(PACKAGE_VERSION)

distdir = $(PACKAGE)-$(shell                                     \
if [ "$(origin OPENTHREAD_VERSION)" != "file" ]; then            \
    echo "$(OPENTHREAD_VERSION)" ;                               \
else                                                             \
    $(abs_top_nlbuild_autotools_dir)/scripts/mkversion           \
        -b "$(OPENTHREAD_VERSION)" "$(top_srcdir)";              \
fi )

#
# check-file-.local-version
#
# Speculatively regenerate .local-version and check to see if it needs
# to be updated.
#
# If OPENTHREAD_VERSION has been supplied anywhere other than in this file
# (which is implicitly the contents of .local-version), then use that;
# otherwise, attempt to generate it from the SCM system.
#
# This is called from $(call check-file,.local-version).
#
define check-file-.local-version
if [ "$(origin OPENTHREAD_VERSION)" != "file" ]; then \
    echo "$(OPENTHREAD_VERSION)" > "$(2)";            \
else                                                             \
    $(abs_top_nlbuild_autotools_dir)/scripts/mkversion           \
        -b "$(OPENTHREAD_VERSION)" "$(top_srcdir)"    \
        > "$(2)";                                                \
fi
endef

#
# check-file-.dist-version
#
# Speculatively regenerate .dist-version and check to see if it needs
# to be updated.
#
# This is called from $(call check-file,.dist-version).
#
define check-file-.dist-version
cat "$(1)" > "$(2)"
endef

#
# A convenience target to allow package users to easily rerun the
# package configuration according to the current configuration.
#
.PHONY: reconfigure
reconfigure: $(builddir)/config.status
	$(AM_V_at)$(<) --recheck

#
# Version file regeneration rules.
#
.PHONY: force

$(builddir)/.local-version: $(srcdir)/.default-version force

$(distdir)/.dist-version: $(builddir)/.local-version force

$(distdir)/.dist-version $(builddir)/.local-version:
	$(call check-file,$(@F))

dist distcheck: $(BUILT_SOURCES)

dist-hook: $(distdir)/.dist-version

#
# Top-level convenience target for making a documentation-only
# distribution whose results appear at the top level of the build tree
# in the same fashion that the distribution would be for 'make dist'.
#

.PHONY: docdist
docdist: $(BUILT_SOURCES)
	$(MAKE) -C doc docdistdir=$(abs_builddir) $(@)

include $(abs_top_nlbuild_autotools_dir)/automake/post.am
