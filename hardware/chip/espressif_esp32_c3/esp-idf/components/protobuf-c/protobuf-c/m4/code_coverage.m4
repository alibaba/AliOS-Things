# SYNOPSIS
#
#   MY_CODE_COVERAGE()
#
# DESCRIPTION
#
#   Defines CODE_COVERAGE_CFLAGS and CODE_COVERAGE_LDFLAGS which should be
#   included in the CFLAGS and LIBS/LDFLAGS variables of every build target
#   (program or library) which should be built with code coverage support.
#   Also defines CODE_COVERAGE_RULES which should be substituted in your
#   Makefile; and $enable_code_coverage which can be used in subsequent
#   configure output. CODE_COVERAGE_ENABLED is defined and substituted, and
#   corresponds to the value of the --enable-code-coverage option, which
#   defaults to being disabled.
#
#   Usage example:
#   configure.ac:
#      MY_CODE_COVERAGE
#
#   Makefile.am:
#      @CODE_COVERAGE_RULES@
#      my_program_LIBS = … $(CODE_COVERAGE_LDFLAGS) …
#      my_program_CFLAGS = … $(CODE_COVERAGE_CFLAGS) …
#
#   This results in a “check-code-coverage” rule being added to any Makefile.am
#   which includes “@CODE_COVERAGE_RULES@” (assuming the module has been
#   configured with --enable-code-coverage). Running `make check-code-coverage`
#   in that directory will run the module’s test suite (`make check`) and build
#   a code coverage report detailing the code which was touched, then print the
#   URI for the report.
#
# LICENSE
#
#   Copyright © 2012, 2014 Philip Withnall
#   Copyright © 2012 Xan Lopez
#   Copyright © 2012 Christian Persch
#   Copyright © 2012 Paolo Borelli
#   Copyright © 2012 Dan Winship
#
#   Derived from Makefile.decl in GLib, originally licenced under LGPLv2.1+.
#   This file is licenced under LGPLv2.1+.

AC_DEFUN([MY_CODE_COVERAGE],[
	dnl Check for --enable-code-coverage
	AC_MSG_CHECKING([whether to build with code coverage support])
	AC_ARG_ENABLE([code-coverage], AS_HELP_STRING([--enable-code-coverage], [Whether to enable code coverage support]),, enable_code_coverage=no)
	AM_CONDITIONAL([CODE_COVERAGE_ENABLED], [test x$enable_code_coverage = xyes])
	AC_SUBST([CODE_COVERAGE_ENABLED], [$enable_code_coverage])
	AC_MSG_RESULT($enable_code_coverage)

	AS_IF([ test "$enable_code_coverage" = "yes" ], [
		dnl Check if gcc is being used
		AS_IF([ test "$GCC" = "no" ], [
			AC_MSG_ERROR([not compiling with gcc, which is required for gcov code coverage])
		])

		AC_CHECK_PROG([LCOV], [lcov], [lcov])
		AC_CHECK_PROG([GENHTML], [genhtml], [genhtml])

		AS_IF([ test -z "$LCOV" ], [
			AC_MSG_ERROR([The lcov program was not found. Please install lcov!])
		])

		AS_IF([ test -z "$GENHTML" ], [
			AC_MSG_ERROR([The genhtml program was not found. Please install lcov!])
		])

		dnl Build the code coverage flags
		CODE_COVERAGE_CFLAGS="-O0 -g --coverage"
		CODE_COVERAGE_LDFLAGS="--coverage"

		AC_SUBST([CODE_COVERAGE_CFLAGS])
		AC_SUBST([CODE_COVERAGE_LDFLAGS])

    dnl Strip optimisation flags
    changequote({,})
    CFLAGS=`echo "$CFLAGS" | $SED -e 's/-O[0-9]*//g'`
    changequote([,])
	])

CODE_COVERAGE_RULES='
# Code coverage
#
# Optional:
#  - CODE_COVERAGE_DIRECTORY: Top-level directory for code coverage reporting.
#    (Default: $(top_builddir))
#  - CODE_COVERAGE_OUTPUT_FILE: Filename and path for the .info file generated
#    by lcov for code coverage. (Default:
#    $(PACKAGE_NAME)-$(PACKAGE_VERSION)-coverage.info)
#  - CODE_COVERAGE_OUTPUT_DIRECTORY: Directory for generated code coverage
#    reports to be created. (Default:
#    $(PACKAGE_NAME)-$(PACKAGE_VERSION)-coverage)
#  - CODE_COVERAGE_LCOV_OPTIONS: Extra options to pass to the lcov instance.
#    (Default: empty)
#  - CODE_COVERAGE_GENHTML_OPTIONS: Extra options to pass to the genhtml
#    instance. (Default: empty)
#  - CODE_COVERAGE_IGNORE_PATTERN: Extra glob pattern of files to ignore
#
# The generated report will be titled using the $(PACKAGE_NAME) and
# $(PACKAGE_VERSION). In order to add the current git hash to the title,
# use the git-version-gen script, available online.

# Optional variables
CODE_COVERAGE_DIRECTORY ?= $(abs_top_builddir)
CODE_COVERAGE_OUTPUT_FILE ?= $(PACKAGE_NAME)-$(PACKAGE_VERSION)-coverage.info
CODE_COVERAGE_OUTPUT_DIRECTORY ?= $(PACKAGE_NAME)-$(PACKAGE_VERSION)-coverage
CODE_COVERAGE_LCOV_OPTIONS ?=
CODE_COVERAGE_GENHTML_OPTIONS ?=
CODE_COVERAGE_IGNORE_PATTERN ?=

code_coverage_quiet = $(code_coverage_quiet_$(V))
code_coverage_quiet_ = $(code_coverage_quiet_$(AM_DEFAULT_VERBOSITY))
code_coverage_quiet_0 = --quiet

# Use recursive makes in order to ignore errors during check
check-code-coverage:
ifeq ($(CODE_COVERAGE_ENABLED),yes)
	-$(MAKE) $(AM_MAKEFLAGS) -k check
	$(MAKE) $(AM_MAKEFLAGS) code-coverage-capture
else
	@echo "Need to reconfigure with --enable-code-coverage"
endif

# Capture code coverage data
code-coverage-capture: code-coverage-capture-hook
ifeq ($(CODE_COVERAGE_ENABLED),yes)
	$(LCOV) $(code_coverage_quiet) --directory $(CODE_COVERAGE_DIRECTORY) --capture --output-file "$(CODE_COVERAGE_OUTPUT_FILE).tmp" --test-name "$(PACKAGE_NAME)-$(PACKAGE_VERSION)" --no-checksum --compat-libtool $(CODE_COVERAGE_LCOV_OPTIONS)
	$(LCOV) $(code_coverage_quiet) --directory $(CODE_COVERAGE_DIRECTORY) --remove "$(CODE_COVERAGE_OUTPUT_FILE).tmp" "/tmp/*" $(CODE_COVERAGE_IGNORE_PATTERN) --output-file "$(CODE_COVERAGE_OUTPUT_FILE)"
	-@rm -f $(CODE_COVERAGE_OUTPUT_FILE).tmp
	LANG=C $(GENHTML) $(code_coverage_quiet) --prefix $(CODE_COVERAGE_DIRECTORY) --output-directory "$(CODE_COVERAGE_OUTPUT_DIRECTORY)" --title "$(PACKAGE_NAME)-$(PACKAGE_VERSION) Code Coverage" --legend --show-details "$(CODE_COVERAGE_OUTPUT_FILE)" $(CODE_COVERAGE_GENHTML_OPTIONS)
	@echo "file://$(abs_builddir)/$(CODE_COVERAGE_OUTPUT_DIRECTORY)/index.html"
else
	@echo "Need to reconfigure with --enable-code-coverage"
endif

# Hook rule executed before code-coverage-capture, overridable by the user
code-coverage-capture-hook:

ifeq ($(CODE_COVERAGE_ENABLED),yes)
clean: code-coverage-clean
code-coverage-clean:
	-$(LCOV) --directory $(CODE_COVERAGE_DIRECTORY) -z
	-rm -rf $(CODE_COVERAGE_OUTPUT_FILE) $(CODE_COVERAGE_OUTPUT_FILE).tmp $(CODE_COVERAGE_OUTPUT_DIRECTORY)
	-find . -name "*.gcda" -o -name "*.gcov" -delete
endif

GITIGNOREFILES ?=
GITIGNOREFILES += $(CODE_COVERAGE_OUTPUT_FILE) $(CODE_COVERAGE_OUTPUT_DIRECTORY)

DISTCHECK_CONFIGURE_FLAGS ?=
DISTCHECK_CONFIGURE_FLAGS += --disable-code-coverage

.PHONY: check-code-coverage code-coverage-capture code-coverage-capture-hook code-coverage-clean
'

	AC_SUBST([CODE_COVERAGE_RULES])
	m4_ifdef([_AM_SUBST_NOTMAKE], [_AM_SUBST_NOTMAKE([CODE_COVERAGE_RULES])])
])
