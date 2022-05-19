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

EXTRA_DIST                                      = \
    $(srcdir)/Doxyfile.in                         \
    $(srcdir)/header.html                         \
    $(srcdir)/images/Open-Thread-Logo-200x42.png  \
    $(srcdir)/images/openthread_logo.png          \
    $(srcdir)/images/ot-contrib-arm.png           \
    $(srcdir)/images/ot-contrib-cascoda.png       \
    $(srcdir)/images/ot-contrib-google.png        \
    $(srcdir)/images/ot-contrib-nordic.png        \
    $(srcdir)/images/ot-contrib-nxp.png           \
    $(srcdir)/images/ot-contrib-qc.png            \
    $(srcdir)/images/ot-contrib-qorvo.png         \
    $(srcdir)/images/ot-contrib-samsung.png       \
    $(srcdir)/images/ot-contrib-silabs.png        \
    $(srcdir)/images/ot-contrib-stm.png           \
    $(srcdir)/images/ot-contrib-synopsys.png      \
    $(srcdir)/images/ot-contrib-ti.png            \
    $(srcdir)/images/ot-contrib-zephyr.png        \
    $(srcdir)/ot_api_doc.h                        \
    $(NULL)

#
# Override autotool's default notion of the package version variables.
# This ensures that when we create a doc distribution that the
# version is always the current version, not the version when the
# package was bootstrapped.
#
PACKAGE_VERSION                                 = $(shell cat $(top_builddir)/.local-version)
VERSION                                         = $(PACKAGE_VERSION)


docdistdir                                     ?= .

openthread_docdist_alias                        = \
    $(PACKAGE_TARNAME)-docs

openthread_docdist_name                         = \
    $(openthread_docdist_alias)-$(VERSION)

openthread_docdist_archive                      = \
    $(docdistdir)/$(openthread_docdist_name).tar.gz

CLEANFILES                                      = \
    Doxyfile                                      \
    $(openthread_docdist_archive)                 \
    $(NULL)

if OPENTHREAD_BUILD_DOCS

openthreaddir = $(includedir)/openthread
dist_openthread_HEADERS = ot_api_doc.h

all-local: html/index.html

#
# We choose to manually transform Doxyfile.in into Doxyfile here in
# the makefile rather than in the configure script so that we can take
# advantage of live, at build time (rather than at configure time),
# updating of the package version number.
#

Doxyfile: $(srcdir)/Doxyfile.in Makefile
	$(AM_V_GEN)$(SED)                                     \
	    -e "s,\@DOXYGEN_USE_DOT\@,$(DOXYGEN_USE_DOT),g"   \
	    -e "s,\@PACKAGE_NAME\@,$(PACKAGE_NAME),g"         \
	    -e "s,\@PACKAGE_VERSION\@,$(PACKAGE_VERSION),g"   \
	    -e "s,\@PERL\@,$(PERL),g"                         \
	    -e "s,\@abs_builddir\@,$(abs_builddir),g"         \
	    -e "s,\@abs_srcdir\@,$(abs_srcdir),g"             \
	    -e "s,\@abs_top_builddir\@,$(abs_top_builddir),g" \
	    -e "s,\@abs_top_srcdir\@,$(abs_top_srcdir),g"     \
	    < "$(srcdir)/Doxyfile.in" > "$(@)"

html/index.html: Doxyfile
	$(AM_V_GEN)$(DOXYGEN) $(<)

#
# Addition rules and commands to create a documentation-only
# distribution of openthread
#

$(openthread_docdist_name): html/index.html
	$(AM_V_at)rm -f -r $(@)
	$(call create-directory)
	$(AM_V_at)cp -R html $(@)

$(openthread_docdist_archive): $(openthread_docdist_name)
	$(AM_V_at)echo "  TAR      $(@)"
	$(AM_V_at)tardir="$(<)" && $(am__tar) | GZIP=$(GZIP_ENV) gzip -c > "$(@)" && rm -rf $(<)

docdist $(openthread_docdist_alias): $(openthread_docdist_archive)

clean-local:
	$(AM_V_at)rm -f -r html

endif # OPENTHREAD_BUILD_DOCS

include $(abs_top_nlbuild_autotools_dir)/automake/post.am
