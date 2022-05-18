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

# Always package (e.g. for 'make dist') these subdirectories.

DIST_SUBDIRS                            = \
    unit                                  \
    scripts                               \
    fuzz                                  \
    $(NULL)

# Always build (e.g. for 'make all') these subdirectories.

SUBDIRS                                 = \
    $(NULL)

SUBDIRS                                += \
    unit                                  \
    $(NULL)

if OPENTHREAD_POSIX
if OPENTHREAD_ENABLE_CLI
SUBDIRS                                += \
    scripts                               \
    $(NULL)
endif
endif

if OPENTHREAD_ENABLE_FUZZ_TARGETS
SUBDIRS                                += fuzz
endif

if OPENTHREAD_BUILD_TESTS
if OPENTHREAD_BUILD_COVERAGE
CLEANFILES                             = $(wildcard *.gcda *.gcno)

if OPENTHREAD_BUILD_COVERAGE_REPORTS
# The bundle should positively be qualified with the absolute build
# path. Otherwise, VPATH will get auto-prefixed to it if there is
# already such a directory in the non-colocated source tree.

OPENTHREAD_COVERAGE_BUNDLE             = ${abs_builddir}/${PACKAGE}${NL_COVERAGE_BUNDLE_SUFFIX}
OPENTHREAD_COVERAGE_INFO               = ${OPENTHREAD_COVERAGE_BUNDLE}/${PACKAGE}${NL_COVERAGE_INFO_SUFFIX}

$(OPENTHREAD_COVERAGE_BUNDLE):
	$(call create-directory)

$(OPENTHREAD_COVERAGE_INFO): check | $(dir $(OPENTHREAD_COVERAGE_INFO))
	$(call generate-coverage-report,${top_builddir})

coverage-local: $(OPENTHREAD_COVERAGE_INFO)

clean-local: clean-local-coverage

.PHONY: clean-local-coverage
clean-local-coverage:
	-$(AM_V_at)rm -rf $(OPENTHREAD_COVERAGE_BUNDLE)
endif # OPENTHREAD_BUILD_COVERAGE_REPORTS
endif # OPENTHREAD_BUILD_COVERAGE
endif # OPENTHREAD_BUILD_TESTS

include $(abs_top_nlbuild_autotools_dir)/automake/post.am
