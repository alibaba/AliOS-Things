#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

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

#
# Declare autoconf version requirements
#
AC_PREREQ([2.68])

#
# Initialize autoconf for the package
#
AC_INIT([OPENTHREAD],
        m4_esyscmd([third_party/nlbuild-autotools/repo/scripts/mkversion -b `cat .default-version` .]),
        [openthread-devel@googlegroups.com],
        [openthread],
        [http://github.com/openthread/openthread])

# Tell the rest of the build system the absolute path where the
# nlbuild-autotools repository is rooted at.

AC_SUBST(nlbuild_autotools_stem,[third_party/nlbuild-autotools/repo])
AC_SUBST(abs_top_nlbuild_autotools_dir,[\${abs_top_srcdir}/\${nlbuild_autotools_stem}])

#
# OPENTHREAD interface current, revision, and age versions.
#
# Maintainters: Please manage these fields as follows:
#
#   Interfaces removed:    CURRENT++, AGE = 0, REVISION = 0
#   Interfaces added:      CURRENT++, AGE++,   REVISION = 0
#   No interfaces changed:                     REVISION++
#
#
AC_SUBST(LIBOPENTHREAD_VERSION_CURRENT,  [0])
AC_SUBST(LIBOPENTHREAD_VERSION_AGE,      [5])
AC_SUBST(LIBOPENTHREAD_VERSION_REVISION, [0])
AC_SUBST(LIBOPENTHREAD_VERSION_INFO,     [${LIBOPENTHREAD_VERSION_CURRENT}:${LIBOPENTHREAD_VERSION_REVISION}:${LIBOPENTHREAD_VERSION_AGE}])

#
# Check the sanity of the source directory by checking for the
# presence of a key watch file
#
AC_CONFIG_SRCDIR([include/openthread/error.h])

#
# Tell autoconf where to find auxilliary build tools (e.g. config.guess,
# install-sh, missing, etc.)
#
AC_CONFIG_AUX_DIR([third_party/nlbuild-autotools/repo/third_party/autoconf])

#
# Tell autoconf where to find auxilliary M4 macros
#
AC_CONFIG_MACRO_DIRS([third_party/nlbuild-autotools/repo/third_party/autoconf/m4 third_party/nlbuild-autotools/repo/autoconf/m4])

#
# Tell autoconf what file the package is using to aggregate C preprocessor
# defines.
#
AC_CONFIG_HEADERS([include/openthread-config-generic.h])

#
# Figure out what the canonical build and host tuples are.
#
AC_CANONICAL_BUILD
AC_CANONICAL_HOST

#
# Mac OS X / Darwin ends up putting some versioning cruft on the end of its
# tuple that we don't care about in this script. Create "clean" variables
# devoid of it.
#

NL_FILTERED_CANONICAL_BUILD
NL_FILTERED_CANONICAL_HOST

#
# Configure automake with the desired options, indicating that this is not
# a native GNU package, that we want "silent" build rules, and that we want
# objects built in the same subdirectory as their source rather than collapsed
# together at the top-level directory.
#
# Disable silent build rules by either passing --disable-silent-rules to
# configure or passing V=1 to make
#
AM_INIT_AUTOMAKE([1.14 foreign silent-rules subdir-objects tar-pax])

#
# Silent build rules requires at least automake-1.11. Employ
# techniques for not breaking earlier versions of automake.
#
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
AM_SILENT_RULES([yes])

#
# Enable maintainer mode to prevent the package from constantly trying
# to rebuild configure, Makefile.in, etc. Rebuilding such files rarely,
# if ever, needs to be done "in the field".
#
# Use the included 'bootstrap' script instead when necessary.
#
AM_MAINTAINER_MODE

#
# Host-os-specific checks
#

case ${host_os} in

    *darwin*)
        OPENTHREAD_TARGET=darwin
        ;;

    *linux*)
        OPENTHREAD_TARGET=linux
        ;;

esac

AM_CONDITIONAL([OPENTHREAD_TARGET_DARWIN], [test "${OPENTHREAD_TARGET}" = "darwin"])
AM_CONDITIONAL([OPENTHREAD_TARGET_LINUX], [test "${OPENTHREAD_TARGET}" = "linux"])

#
# Checks for build host programs
#

# This is a hack to restore some old broken behavior that was
# removed in pull request #1527. It's use is highly discouraged,
# you should try to fix your build environment instead.
AC_ARG_ENABLE(no-executables-hack,
    [AS_HELP_STRING([--enable-no-executables-hack],
        [Enable hack that prevents link checks at configure time. Highly discouraged.])])
AC_MSG_CHECKING([whether to disable executable checking])
if test "${enable_no_executables_hack}" = "yes"
then
    AC_MSG_RESULT([yes])
    AC_NO_EXECUTABLES
else
    AC_MSG_RESULT([no])
fi

# Passing -Werror to GCC-based or -compatible compilers breaks some
# autoconf tests (see
# http://lists.gnu.org/archive/html/autoconf-patches/2008-09/msg00014.html).
#
# If -Werror has been passed transform it into -Wno-error. We'll
# transform it back later with NL_RESTORE_WERROR.

NL_SAVE_WERROR

# Check for compilers.
#
# These should be checked BEFORE we check for and, implicitly,
# initialize libtool such that libtool knows what languages it has to
# work with.

AC_PROG_CPP
AC_PROG_CPP_WERROR

AC_PROG_CC
AC_PROG_CC_C_O

AC_PROG_CXXCPP

AC_PROG_CXX
AC_PROG_CXX_C_O

AM_PROG_AS

AC_C_BIGENDIAN

# Check for other compiler toolchain tools.

AC_CHECK_TOOL(AR, ar)
AC_CHECK_TOOL(RANLIB, ranlib)
AC_CHECK_TOOL(OBJCOPY, objcopy)
AC_CHECK_TOOL(STRIP, strip)

# Check for other host tools.

AC_PROG_INSTALL
AC_PROG_LN_S

AC_PATH_PROG(CMP, cmp)

# Check for and initialize libtool

LT_INIT
AC_PROG_LIBTOOL

#
# Checks for specific compiler characteristics
#

#
# Common compiler flags we would like to have.
#
#   -Wall                        CC, CXX
#

PROSPECTIVE_CFLAGS="-Wall -Wextra -Wshadow -Wundef -Wcast-align -Werror -Wno-error=undef -std=c99 -pedantic-errors"
PROSPECTIVE_CXXFLAGS="-Wall -Wextra -Wshadow -Wundef -Wcast-align -Werror -Wno-error=undef -std=c++11 -Wno-c++14-compat -fno-exceptions -pedantic-errors"

AC_CACHE_CHECK([whether $CC is Clang],
    [nl_cv_clang],
    [nl_cv_clang=no
    if test "x${GCC}" = "xyes"; then
        AC_EGREP_CPP([NL_CC_IS_CLANG],
            [/* Note: Clang 2.7 lacks __clang_[a-z]+__ */
#            if defined(__clang__) && defined(__llvm__)
             NL_CC_IS_CLANG
#            endif
            ],
            [nl_cv_clang=yes])
    fi
    ])

if test "${nl_cv_clang}" = "yes"; then
    PROSPECTIVE_CFLAGS="${PROSPECTIVE_CFLAGS} -Wconversion -Wno-gnu-zero-variadic-macro-arguments"
    PROSPECTIVE_CXXFLAGS="${PROSPECTIVE_CXXFLAGS} -Wconversion"
fi

convert_warning_flags_to_positive() {
    flags_to_check=""
    warning_flag_backup=""
    for option in ${1}; do
        case "$option"
        in
           -Wno-*)
                flags_to_check="${flags_to_check} -W${option##-Wno-}"
                warning_flag_backup="${warning_flag_backup} ${option##-Wno-}";;
            *)
                flags_to_check="${flags_to_check} ${option}";;
        esac
    done
}

restore_negative_form_of_warning_flags() {
    final_flags=${1}
    for positive_option in ${warning_flag_backup}; do
        final_flags=$(echo $ECHO_N $final_flags | $SED "s|-W${positive_option}|-Wno-${positive_option}|g")
    done
    unset flags_to_check
    unset warning_flag_backup
}

check_prospective_CFLAGS() {
    if test "${nl_cv_clang}" = "yes"; then
        AX_CHECK_COMPILER_OPTIONS([C], ${PROSPECTIVE_CFLAGS})
    else
        convert_warning_flags_to_positive "${PROSPECTIVE_CFLAGS}"
        AX_CHECK_COMPILER_OPTIONS([C], ${flags_to_check})
        restore_negative_form_of_warning_flags "${CFLAGS}"
        CFLAGS=${final_flags}
        unset final_flags
    fi
}

check_prospective_CXXFLAGS() {
    if test "${nl_cv_clang}" = "yes"; then
        AX_CHECK_COMPILER_OPTIONS([C++], ${PROSPECTIVE_CXXFLAGS})
    else
        convert_warning_flags_to_positive "${PROSPECTIVE_CXXFLAGS}"
        AX_CHECK_COMPILER_OPTIONS([C++], ${flags_to_check})
        restore_negative_form_of_warning_flags "${CXXFLAGS}"
        CXXFLAGS=${final_flags}
        unset final_flags
    fi
}

check_prospective_CFLAGS
check_prospective_CXXFLAGS

# Disable building shared libraries by default (can be enabled with --enable-shared)

AC_DISABLE_SHARED

#
# Debug instances
#
AC_MSG_NOTICE([checking whether to build debug instances])

# Debug

NL_ENABLE_DEBUG([no])

AM_CONDITIONAL([OPENTHREAD_BUILD_DEBUG], [test "${nl_cv_build_debug}" = "yes"])

#
# Code coverage and compiler optimization
#

# Coverage

NL_ENABLE_COVERAGE([no])

AM_CONDITIONAL([OPENTHREAD_BUILD_COVERAGE], [test "${nl_cv_build_coverage}" = "yes"])

NL_ENABLE_COVERAGE_REPORTS([auto])

AM_CONDITIONAL([OPENTHREAD_BUILD_COVERAGE_REPORTS], [test "${nl_cv_build_coverage_reports}" = "yes"])

# Optimization

NL_ENABLE_OPTIMIZATION([yes])

AM_CONDITIONAL([OPENTHREAD_BUILD_OPTIMIZED], [test "${nl_cv_build_optimized}" = "yes"])

# Fuzz Targets

AC_MSG_CHECKING([whether to build fuzz targets])
AC_ARG_ENABLE(fuzz-targets,
    [AS_HELP_STRING([--enable-fuzz-targets],[Enable building of fuzz targets @<:@default=no@:>@.])],
    [
        case "${enableval}" in

        no|yes)
            enable_fuzz_targets=${enableval}
            ;;

        *)
            AC_MSG_ERROR([Invalid value ${enableval} for --enable-fuzz-targets])
            ;;

        esac
    ],
    [enable_fuzz_targets=no])
AC_MSG_RESULT(${enable_fuzz_targets})

AM_CONDITIONAL([OPENTHREAD_ENABLE_FUZZ_TARGETS], [test "${enable_fuzz_targets}" = "yes"])

if test "${enable_fuzz_targets}" = "no" ; then
    PROSPECTIVE_CXXFLAGS="-fno-rtti"
    check_prospective_CXXFLAGS
fi

# Address Sanitizer

AC_MSG_CHECKING([whether to build with Address Sanitizer support])
AC_ARG_ENABLE(address-sanitizer,
    [AS_HELP_STRING([--enable-address-sanitizer],[Enable Address Sanitizer support @<:@default=no@:>@.])],
    [
        case "${enableval}" in

        no|yes)
            enable_address_sanitizer=${enableval}
            ;;

        *)
            AC_MSG_ERROR([Invalid value ${enableval} for --enable-address-sanitizer])
            ;;

        esac
    ],
    [enable_address_sanitizer=no])
AC_MSG_RESULT(${enable_address_sanitizer})

AM_CONDITIONAL([OPENTHREAD_WITH_ADDRESS_SANITIZER], [test "${enable_address_sanitizer}" = "yes"])

if test "${enable_address_sanitizer}" = "yes" ; then

    PROSPECTIVE_CFLAGS="-fsanitize=address"
    PROSPECTIVE_CXXFLAGS="-fsanitize=address"

    # Check if the compilers support address sanitizer
    check_prospective_CFLAGS
    check_prospective_CXXFLAGS
fi

#
# Tests
#
AC_MSG_NOTICE([checking whether to build tests])

NL_ENABLE_TESTS([yes])

AM_CONDITIONAL([OPENTHREAD_BUILD_TESTS], [test "${nl_cv_build_tests}" = "yes"])

#
# Builtin mbedtls
#

AC_ARG_ENABLE(builtin-mbedtls,
    [AS_HELP_STRING([--enable-builtin-mbedtls],[Enable builtin mbedtls @<:@default=yes@:>@.])],
    [
        case "${enableval}" in

        no|yes)
            enable_builtin_mbedtls=${enableval}
            ;;

        *)
            AC_MSG_ERROR([Invalid value ${enable_builtin_mbedtls} for --enable-builtin-mbedtls])
            ;;
        esac
    ],
    [enable_builtin_mbedtls=yes])

if test "$enable_builtin_mbedtls" = "yes" -a ! "${MBEDTLS_CPPFLAGS}"; then
    MBEDTLS_CPPFLAGS="-I\${abs_top_srcdir}/third_party/mbedtls"
    MBEDTLS_CPPFLAGS="${MBEDTLS_CPPFLAGS} -I\${abs_top_srcdir}/third_party/mbedtls/repo/include"
    MBEDTLS_CPPFLAGS="${MBEDTLS_CPPFLAGS} -DMBEDTLS_CONFIG_FILE=\\\"mbedtls-config.h\\\""
fi

if test "$enable_builtin_mbedtls" = "yes"; then
    CPPFLAGS="${CPPFLAGS} -DOPENTHREAD_CONFIG_ENABLE_BUILTIN_MBEDTLS=1"
else
    CPPFLAGS="${CPPFLAGS} -DOPENTHREAD_CONFIG_ENABLE_BUILTIN_MBEDTLS=0"
fi

AC_MSG_CHECKING([whether mbed TLS should be enabled])
AC_MSG_RESULT(${enable_builtin_mbedtls})
AM_CONDITIONAL([OPENTHREAD_ENABLE_BUILTIN_MBEDTLS], [test "${enable_builtin_mbedtls}" = "yes"])

#
# POSIX Daemon
#

AC_MSG_CHECKING([whether to build POSIX applicaton in daemon mode])
AC_ARG_ENABLE(posix-daemon,
    [AS_HELP_STRING([--enable-posix-daemon], [Build POSIX application in daemon mode@<:@default=no@:>@.])],
    [
        case "${enableval}" in

        no|yes)
            enable_posix_daemon=${enableval}
            ;;

        *)
            AC_MSG_ERROR([Invalid value ${enable_posix} for --enable-posix-daemon])
            ;;
        esac
    ],
    [enable_posix_daemon=no])

if test "$enable_posix_daemon" = "yes"; then
    CPPFLAGS="${CPPFLAGS} -DOPENTHREAD_POSIX_CONFIG_DAEMON_ENABLE=1"
else
    CPPFLAGS="${CPPFLAGS} -DOPENTHREAD_POSIX_CONFIG_DAEMON_ENABLE=0"
fi

AC_MSG_RESULT(${enable_posix_daemon})
AM_CONDITIONAL([OPENTHREAD_POSIX_CONFIG_DAEMON_ENABLE], [test "${enable_posix_daemon}" = "yes"])

#
# FTD Library
#

AC_MSG_CHECKING([whether to build FTD library])
AC_ARG_ENABLE(ftd,
    [AS_HELP_STRING([--enable-ftd], [Build FTD library @<:@default=no@:>@.])],
    [
        case "${enableval}" in

        no|yes)
            enable_ftd=${enableval}
            ;;

        *)
            AC_MSG_ERROR([Invalid value ${enable_ftd} for --enable-ftd])
            ;;
        esac
    ],
    [enable_ftd=no])

AC_MSG_RESULT(${enable_ftd})
AM_CONDITIONAL([OPENTHREAD_ENABLE_FTD], [test "${enable_ftd}" = "yes"])

#
# MTD Library
#

AC_MSG_CHECKING([whether to build MTD library])
AC_ARG_ENABLE(mtd,
    [AS_HELP_STRING([--enable-mtd], [Build MTD library @<:@default=no@:>@.])],
    [
        case "${enableval}" in

        no|yes)
            enable_mtd=${enableval}
            ;;

        *)
            AC_MSG_ERROR([Invalid value ${enable_mtd} for --enable-mtd])
            ;;
        esac
    ],
    [enable_mtd=no])

AC_MSG_RESULT(${enable_mtd})
AM_CONDITIONAL([OPENTHREAD_ENABLE_MTD], [test "${enable_mtd}" = "yes"])

#
# Radio Library
#

AC_MSG_CHECKING([whether to build radio-only library])
AC_ARG_ENABLE(radio-only,
    [AS_HELP_STRING([--enable-radio-only], [Build radio-only library @<:@default=no@:>@.])],
    [
        case "${enableval}" in

        no|yes)
            enable_radio_only=${enableval}
            ;;

        *)
            AC_MSG_ERROR([Invalid value ${enable_radio_only} for --enable-radio-only])
            ;;
        esac
    ],
    [enable_radio_only=no])

AC_MSG_RESULT(${enable_radio_only})
AM_CONDITIONAL([OPENTHREAD_ENABLE_RADIO_ONLY], [test "${enable_radio_only}" = "yes"])

#
# Radio Cli Library
#

AC_MSG_CHECKING([whether to build radio-cli library])
AC_ARG_ENABLE(radio-cli,
    [AS_HELP_STRING([--enable-radio-cli], [Build radio-cli library @<:@default=no@:>@.])],
    [
        case "${enableval}" in

        no|yes)
            enable_radio_cli=${enableval}
            ;;

        *)
            AC_MSG_ERROR([Invalid value ${enable_radio_cli} for --enable-radio-cli])
            ;;
        esac
    ],
    [enable_radio_cli=no])

AC_MSG_RESULT(${enable_radio_cli})
AM_CONDITIONAL([OPENTHREAD_ENABLE_RADIO_CLI], [test "${enable_radio_cli}" = "yes"])

#
# whether to build executables
#

AC_MSG_CHECKING([whether to build executables])
AC_ARG_ENABLE(executable,
    [AS_HELP_STRING([--enable-executable], [Build executables @<:@default=yes@:>@.])],
    [
        case "${enableval}" in

        no|yes)
            enable_executable=${enableval}
            ;;

        *)
            AC_MSG_ERROR([Invalid value ${enable_executable} for --enable-executable])
            ;;
        esac
    ],
    [enable_executable=yes])

AC_MSG_RESULT(${enable_executable})
AM_CONDITIONAL([OPENTHREAD_ENABLE_EXECUTABLE], [test "${enable_executable}" = "yes"])

#
# CLI Library
#

AC_MSG_CHECKING([whether to build CLI library])
AC_ARG_ENABLE(cli,
    [AS_HELP_STRING([--enable-cli], [Build CLI library @<:@default=no@:>@.])],
    [
        case "${enableval}" in

        no|yes)
            enable_cli=${enableval}
            ;;

        *)
            AC_MSG_ERROR([Invalid value ${enable_cli} for --enable-cli])
            ;;
        esac
    ],
    [enable_cli=no])

AC_MSG_RESULT(${enable_cli})
AM_CONDITIONAL([OPENTHREAD_ENABLE_CLI], [test "${enable_cli}" = "yes"])

#
# NCP Library
#

AC_MSG_CHECKING([whether to build NCP library])
AC_ARG_ENABLE(ncp,
    [AS_HELP_STRING([--enable-ncp], [Build NCP library @<:@default=no@:>@.])],
    [
        case "${enableval}" in

        no|yes)
            enable_ncp=${enableval}
            ;;

        *)
            AC_MSG_ERROR([Invalid value ${enable_ncp} for --enable-ncp])
            ;;
        esac
    ],
    [enable_ncp=no])

AC_MSG_RESULT(${enable_ncp})
AM_CONDITIONAL([OPENTHREAD_ENABLE_NCP], [test "${enable_ncp}" = "yes"])

#
# Readline - readline library to use for POSIX CLI
#

AC_ARG_WITH(
    [readline],
    [AS_HELP_STRING([--with-readline],[Specify the readline library (no|readline|edit) @<:@default=no@:>@.])],
    [],
    [with_readline=no])

case "${with_readline}" in
"no")
    ;;
"readline")
    # Prefer GNU readline installed by Homebrew.
    if test "${OPENTHREAD_TARGET}" = darwin && brew --prefix readline > /dev/null; then
        LDFLAGS="-L$(brew --prefix readline)/lib ${LDFLAGS}"
        CPPFLAGS="-I$(brew --prefix readline)/include ${CPPFLAGS}"
        PKG_CONFIG_PATH="$(brew --prefix)/lib/pkgconfig:$PKG_CONFIG_PATH"
    fi
    AC_CHECK_LIB([readline], [rl_callback_handler_install])
    ;;
"edit")
    AC_CHECK_LIB([edit], [rl_callback_handler_install])
    ;;
*)
    AC_MSG_ERROR([unexpected --with-readline=${with_readline}])
    ;;
esac

#
# Vendor Extension - Specify a C++ source file which will be built as part of OpenThread core library.
#

AC_ARG_WITH(
    [vendor-extension],
    [AS_HELP_STRING([--with-vendor-extension=<VENDOR_EXT.CPP>],[Specify a C++ source file built as part of OpenThread core library. @<:@default=no@:>@.])],
    [
        if test "${withval}" = "no"
        then OPENTHREAD_ENABLE_VENDOR_EXTENSION=0
        elif test '!' -f "${withval}"
        then AC_MSG_ERROR([Can't open ${withval} for --with-vendor-extension])
        else
            OPENTHREAD_ENABLE_VENDOR_EXTENSION=1

            # Get the absolute path.
            OPENTHREAD_VENDOR_EXTENSION_SOURCE=$(cd `dirname ${withval}` && pwd)/$(basename ${withval})
        fi
    ],
    [
        OPENTHREAD_ENABLE_VENDOR_EXTENSION=0
        with_vendor_extension=no
    ])

CPPFLAGS="${CPPFLAGS} -DOPENTHREAD_ENABLE_VENDOR_EXTENSION=${OPENTHREAD_ENABLE_VENDOR_EXTENSION}"
AC_MSG_CHECKING([for vendor extension])
AC_MSG_RESULT(${OPENTHREAD_VENDOR_EXTENSION_SOURCE-no})
AC_SUBST(OPENTHREAD_VENDOR_EXTENSION_SOURCE)
AM_CONDITIONAL([OPENTHREAD_ENABLE_VENDOR_EXTENSION], [test "${OPENTHREAD_ENABLE_VENDOR_EXTENSION}" = "1"])

#
# NCP Vendor Hook Source - Specify a C++ file that implements the NCP vendor hook.
#

AC_ARG_WITH(
    [ncp-vendor-hook-source],
    [AS_HELP_STRING([--with-ncp-vendor-hook-source=<VENDOR_HOOK.CPP>],[Specify a C++ file that implements the NCP vendor hook. @<:@default=no@:>@.])],
    [
        if test "${withval}" = "no"
        then OPENTHREAD_ENABLE_NCP_VENDOR_HOOK=0
        elif test '!' -f "${withval}"
        then AC_MSG_ERROR([Can't open ${withval} for --with-ncp-vendor-hook-source])
        else
            OPENTHREAD_ENABLE_NCP_VENDOR_HOOK=1
            CPPFLAGS="${CPPFLAGS} -DOPENTHREAD_ENABLE_NCP_VENDOR_HOOK=1"

            # Get the absolute path.
            OPENTHREAD_NCP_VENDOR_HOOK_SOURCE=$(cd `dirname ${withval}` && pwd)/$(basename ${withval})
        fi
    ],
    [
        OPENTHREAD_ENABLE_NCP_VENDOR_HOOK=0
        with_ncp_vendor_hook_source=no
    ])

AC_MSG_CHECKING([for NCP vendor hook source])
AC_MSG_RESULT(${OPENTHREAD_NCP_VENDOR_HOOK_SOURCE-no})
AC_SUBST(OPENTHREAD_NCP_VENDOR_HOOK_SOURCE)
AM_CONDITIONAL([OPENTHREAD_ENABLE_NCP_VENDOR_HOOK], [test "${OPENTHREAD_ENABLE_NCP_VENDOR_HOOK}" = "1"])

#
# NCP Spinel Encrypter
#

AC_ARG_WITH(
    [ncp-spinel-encrypter-libs],
    [AS_HELP_STRING([--with-ncp-spinel-encrypter-libs=<LIBSPINEL_ENCRYPTER.A>],[Specify library files (absolute paths) implementing the NCP Spinel Encrypter. @<:@default=no@:>@.])],
    [
        if test "${withval}" = "no"
        then OPENTHREAD_ENABLE_NCP_SPINEL_ENCRYPTER=0
        else
            OPENTHREAD_ENABLE_NCP_SPINEL_ENCRYPTER=1
            OPENTHREAD_NCP_SPINEL_ENCRYPTER_LIBS=${withval}
        fi
    ],
    [
        OPENTHREAD_ENABLE_NCP_SPINEL_ENCRYPTER=0
        with_ncp_spinel_encrypter_libs=no
    ])

CPPFLAGS="${CPPFLAGS} -DOPENTHREAD_ENABLE_NCP_SPINEL_ENCRYPTER=${OPENTHREAD_ENABLE_NCP_SPINEL_ENCRYPTER}"
AC_MSG_CHECKING([for NCP Spinel Encrypter])
AC_MSG_RESULT(${OPENTHREAD_NCP_SPINEL_ENCRYPTER_LIBS-no})
AC_SUBST(OPENTHREAD_NCP_SPINEL_ENCRYPTER_LIBS)
AM_CONDITIONAL([OPENTHREAD_ENABLE_NCP_SPINEL_ENCRYPTER], [test "${OPENTHREAD_ENABLE_NCP_SPINEL_ENCRYPTER}" = "1"])

#
# Linker Map Output
#

AC_ARG_ENABLE(linker-map,
    [AS_HELP_STRING([--enable-linker-map],[Enable linker map output @<:@default=no@:>@.])],
    [
        case "${enableval}" in

        no|yes)
            enable_linker_map=${enableval}
            ;;

        *)
            AC_MSG_ERROR([Invalid value ${enable_linker_map} for --enable-linker-map])
            ;;
        esac
    ],
    [enable_linker_map=no])

AC_MSG_CHECKING([whether to enable linker map output])
AC_MSG_RESULT(${enable_linker_map})
AM_CONDITIONAL([OPENTHREAD_ENABLE_LINKER_MAP], [test "${enable_linker_map}" = "yes"])

#
# Custom linker file
#

AC_ARG_WITH(
    [custom-linker-file],
    [AS_HELP_STRING([--with-custom-linker-file=<linkerfile.ld>],[Specify custom linker file (absolute path). @<:@default=no@:>@.])],
    [
        with_custom_linker_file=yes
        OPENTHREAD_CUSTOM_LINKER_FILE=${withval}
    ],
    [
        with_custom_linker_file=no
    ])

AC_SUBST(OPENTHREAD_CUSTOM_LINKER_FILE)
AM_CONDITIONAL([OPENTHREAD_ENABLE_CUSTOM_LINKER_FILE], [test "${with_custom_linker_file}" = "yes"])

#
# Examples
#

AC_MSG_CHECKING([whether to build examples])

AC_ARG_WITH(examples,
    [AS_HELP_STRING([--with-examples=TARGET],
        [Build example applications for one of: simulation, cc2538 @<:@default=no@:>@.
         Note that building example applications also builds the associated OpenThread platform libraries
         and any third_party libraries needed to support the examples.])],
    [
        case "${with_examples}" in
        no)
            ;;
        simulation|cc2538)
            ;;
        *)
            AC_MSG_RESULT(ERROR)
            AC_MSG_ERROR([Invalid value given for --with-examples: ${with_examples}])
            ;;
        esac
    ],
    [with_examples=no])

AM_CONDITIONAL([OPENTHREAD_ENABLE_EXAMPLES], [test ${with_examples} != "no"])

AM_CONDITIONAL([OPENTHREAD_EXAMPLES_SIMULATION],[test "${with_examples}" = "simulation"])
AM_CONDITIONAL([OPENTHREAD_EXAMPLES_CC2538],    [test "${with_examples}" = "cc2538"])

AM_COND_IF([OPENTHREAD_EXAMPLES_SIMULATION], CPPFLAGS="${CPPFLAGS} -DOPENTHREAD_EXAMPLES_SIMULATION=1", CPPFLAGS="${CPPFLAGS} -DOPENTHREAD_EXAMPLES_SIMULATION=0")

case ${with_examples} in
    no)
        AC_MSG_RESULT([no])
        ;;
    *)
        AC_MSG_RESULT([yes (${with_examples})])
        ;;
esac

#
# Platform
#

AC_MSG_CHECKING([whether to build platform libraries])

AC_ARG_WITH(platform,
    [AS_HELP_STRING([--with-platform=TARGET],
        [Build OpenThread platform libraries for one of: cc2538, posix, simulation @<:@default=simulation@:>@.])],
    [
        # Make sure the given target is valid.
        case "${with_platform}" in
        no|cc2538|posix|simulation)
            ;;
        *)
            AC_MSG_RESULT(ERROR)
            AC_MSG_ERROR([Invalid value given for --with-platform: ${with_platform}])
            ;;
        esac

        # If both --with-platform and --with-examples are specified, make sure the targets match.
        case "${with_examples}" in
        no)
            ;;
        ${with_platform})
            ;;
        *)
            AC_MSG_RESULT(ERROR)
            AC_MSG_ERROR([Invalid value given for --with-platform: The targets for --with-examples and --with-platform must match.])
            ;;
        esac

    ],
    [
        # If --with-platform is NOT specified, but --with-examples is, automatically build the
        # corresponding platform libraries. (Essentially, --with-examples implies --with-platform).
        with_platform=${with_examples}
    ])

AM_CONDITIONAL([OPENTHREAD_ENABLE_PLATFORM], [test ${with_platform} != "no"])

OPENTHREAD_ENABLE_PLATFORM=${with_platform}

AM_CONDITIONAL([OPENTHREAD_PLATFORM_CC2538],    [test "${with_platform}" = "cc2538"])
AM_CONDITIONAL([OPENTHREAD_PLATFORM_POSIX],     [test "${with_platform}" = "posix"])
AM_CONDITIONAL([OPENTHREAD_PLATFORM_SIMULATION],[test "${with_platform}" = "simulation"])

AM_COND_IF([OPENTHREAD_PLATFORM_POSIX], CPPFLAGS="${CPPFLAGS} -DOPENTHREAD_PLATFORM_POSIX=1", CPPFLAGS="${CPPFLAGS} -DOPENTHREAD_PLATFORM_POSIX=0")

case ${with_platform} in
    no)
        AC_MSG_RESULT([no])
        ;;
    *)
        AC_MSG_RESULT([yes (${with_platform})])
        ;;
esac

#
# Tools
#

AC_MSG_CHECKING([whether to build tools])
AC_ARG_ENABLE(tools,
    [AS_HELP_STRING([--disable-tools],[Disable building of tools @<:@default=no@:>@.])],
    [
        case "${enableval}" in

        no|yes)
            build_tools=${enableval}
            ;;

        *)
            AC_MSG_ERROR([Invalid value ${enableval} for --enable-tools])
            ;;

        esac
    ],
    [build_tools=yes])
AC_MSG_RESULT(${build_tools})
AM_CONDITIONAL([OPENTHREAD_BUILD_TOOLS], [test "${build_tools}" = "yes"])

#
# Documentation
#

# Determine whether or not documentation (via Doxygen) should be built
# or not, with 'auto' as the default and establish a default support
# value for GraphViz 'dot' support.

NL_ENABLE_DOCS([auto],[NO])

AM_CONDITIONAL(OPENTHREAD_BUILD_DOCS, [test "${nl_cv_build_docs}" = "yes"])

AM_CONDITIONAL(OPENTHREAD_POSIX, [test "${with_platform}" = "posix" -o "${with_examples}" = "simulation"])

AM_CONDITIONAL([OPENTHREAD_ENABLE_SPINEL_RCP], [test OPENTHREAD_PLATFORM_POSIX || test OPENTHREAD_ENABLE_RADIO_ONLY])

#
# Checks for libraries and packages.
#
# At minimum, the following packages are optional, depending on
# configuration:
#
#   * TBD
#
AC_MSG_NOTICE([checking required package dependencies])

# NL_WITH_PACKAGE(...)

#
# Check for headers
#

OLD_CFLAGS="${CFLAGS}"
CFLAGS="${CFLAGS} -Wno-error=address"
AC_HEADER_STDBOOL
CFLAGS="${OLD_CFLAGS}"
AC_HEADER_STDC

AC_CHECK_HEADERS([stdint.h])
AC_CHECK_HEADERS([string.h])

#
# Check for types and structures
#
AC_TYPE_INT8_T
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_UINT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T

#
# Checks for library functions
#

if test "${ac_no_link}" != "yes"; then
    AC_CHECK_FUNCS([memcpy])
fi

# Add any mbedtls CPPFLAGS

CPPFLAGS="${CPPFLAGS} ${MBEDTLS_CPPFLAGS}"

# Add any code coverage CPPFLAGS and LDFLAGS

CPPFLAGS="${CPPFLAGS} ${NL_COVERAGE_CPPFLAGS}"
LIBS="${LIBS} ${NL_COVERAGE_LIBS}"
LDFLAGS="${LDFLAGS} ${NL_COVERAGE_LDFLAGS}"

# At this point, we can restore the compiler flags to whatever the
# user passed in, now that we're clear of an -Werror issues by
# transforming -Wno-error back to -Werror.

NL_RESTORE_WERROR

#
# Identify the various makefiles and auto-generated files for the package
#
AC_CONFIG_FILES([
Makefile
include/Makefile
src/Makefile
src/cli/Makefile
src/ncp/Makefile
src/core/Makefile
src/posix/Makefile
src/posix/platform/Makefile
src/lib/Makefile
src/lib/hdlc/Makefile
src/lib/platform/Makefile
src/lib/spinel/Makefile
src/lib/url/Makefile
third_party/Makefile
third_party/jlink/Makefile
third_party/mbedtls/Makefile
third_party/tcplp/Makefile
examples/Makefile
examples/apps/Makefile
examples/apps/cli/Makefile
examples/apps/ncp/Makefile
examples/platforms/Makefile
examples/platforms/cc2538/Makefile
examples/platforms/simulation/Makefile
examples/platforms/utils/Makefile
tools/Makefile
tools/harness-automation/Makefile
tools/harness-thci/Makefile
tools/spi-hdlc-adapter/Makefile
tests/Makefile
tests/fuzz/Makefile
tests/scripts/Makefile
tests/scripts/thread-cert/Makefile
tests/unit/Makefile
doc/Makefile
])

#
# Generate the auto-generated files for the package
#
AC_OUTPUT

#
# Summarize the package configuration
#

AC_MSG_NOTICE([

  Configuration Summary
  ---------------------
  Package                                   : ${PACKAGE_NAME}
  Version                                   : ${PACKAGE_VERSION}
  Interface                                 : $(echo $LIBOPENTHREAD_VERSION_INFO | $SED 's/:/./g')
  Build system                              : ${build}
  Host system                               : ${host}
  Host architecture                         : ${host_cpu}
  Host OS                                   : ${host_os}
  Cross compiling                           : ${cross_compiling}
  Build shared libraries                    : ${enable_shared}
  Build static libraries                    : ${enable_static}
  Build debug libraries                     : ${nl_cv_build_debug}
  Build optimized libraries                 : ${nl_cv_build_optimized}
  Build coverage libraries                  : ${nl_cv_build_coverage}
  Build coverage reports                    : ${nl_cv_build_coverage_reports}
  Address sanitizer support                 : ${enable_address_sanitizer}
  Lcov                                      : ${LCOV:--}
  Genhtml                                   : ${GENHTML:--}
  Build tests                               : ${nl_cv_build_tests}
  Build fuzz targets                        : ${enable_fuzz_targets}
  Build tools                               : ${build_tools}
  OpenThread tests                          : ${with_tests}
  Prefix                                    : ${prefix}
  Documentation support                     : ${nl_cv_build_docs}
  Doxygen                                   : ${DOXYGEN:--}
  GraphViz dot                              : ${DOT:--}
  C Preprocessor                            : ${CPP}
  C Compiler                                : ${CC}
  C++ Preprocessor                          : ${CXXCPP}
  C++ Compiler                              : ${CXX}
  Assembler Compiler                        : ${CCAS}
  Archiver                                  : ${AR}
  Archive Indexer                           : ${RANLIB}
  Symbol Stripper                           : ${STRIP}
  Object Copier                             : ${OBJCOPY}
  C Preprocessor flags                      : ${CPPFLAGS:--}
  C Compile flags                           : ${CFLAGS:--}
  C++ Compile flags                         : ${CXXFLAGS:--}
  Assembler flags                           : ${CCASFLAGS:--}
  Link flags                                : ${LDFLAGS:--}
  Link libraries                            : ${LIBS}
  Link maps                                 : ${enable_linker_map}
  OpenThread FTD support                    : ${enable_ftd}
  OpenThread MTD support                    : ${enable_mtd}
  OpenThread Radio Only support             : ${enable_radio_only}
  OpenThread CLI support                    : ${enable_cli}
  OpenThread NCP support                    : ${enable_ncp}
  OpenThread NCP Vendor Hook Source         : ${with_ncp_vendor_hook_source}
  OpenThread NCP Spinel Encrypter           : ${with_ncp_spinel_encrypter_libs}
  OpenThread Vendor Extension Source        : ${with_vendor_extension}
  OpenThread builtin mbedtls support        : ${enable_builtin_mbedtls}
  OpenThread Examples                       : ${with_examples}
  OpenThread Platform Libraries             : ${with_platform}

])
