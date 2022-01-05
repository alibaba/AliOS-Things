# configure.ac for the libcoap package
#
# Copyright (C) 2010-2015 Olaf Bergmann <bergmann@tzi.org>
#           (C) 2015-2018 Carsten Schoenert <c.schoenert@t-online.de>
#
# Please run 'autogen.sh' to let autoconf produce a configure script.

# Define the libcoap software version here. Note! The libtool versions are
# defined later.
m4_define([libcoap_major_version], [4])
m4_define([libcoap_minor_version], [2])
m4_define([libcoap_micro_version], [0])

# define an appending release state if needed, for example for pre-releases
# like 'alpha' or 'rc1', for a full release keep the value empty!
m4_define([libcoap_release_state], [])

# concatenate the full libcoap version string
m4_define([libcoap_version], [m4_format([%s.%s.%s%s], libcoap_major_version, libcoap_minor_version, libcoap_micro_version, libcoap_release_state)])

AC_INIT([libcoap], [libcoap_version], [libcoap-developers@lists.sourceforge.net], [libcoap], [https://libcoap.net/])
AC_PREREQ([2.64])
AM_INIT_AUTOMAKE([1.10 -Wall no-define no-dist-gzip dist-bzip2])
PKG_PROG_PKG_CONFIG([0.20])
AM_SILENT_RULES([yes])

# Generate one configuration header file for building the library itself with
# an auto generated template. We need later a second one (include/libcoap.h)
# that will be installed alongside the library.
AC_CONFIG_HEADERS([coap_config.h])

AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_SED
AC_CONFIG_MACRO_DIR([m4])
m4_pattern_allow([AM_PROG_AR])
AM_PROG_AR

AC_C_BIGENDIAN

# enable the automatically build of shared and static libraries
LT_INIT([shared static])

# Setting the libtool versioning
###################################################################################
#                                                                                 #
#  To set the version of the library, libtool provides the -version-info          #
#  parameter, which accepts three numbers, separated by colons, that are called   #
#  respectively, current, revision and age. Both their name and their behavior,   #
#  nowadays, have to be considered fully arbitrary, as the explanation provided   #
#  in the official documentation is confusing to say the least, and can be, in    #
#  some cases, considered completely wrong.                                       #
#  https://autotools.io/libtool/version.html                                      #
#                                                                                 #
###################################################################################
#
# How to work with the libtool versioning?
#
# Follow the followings steps from top to bottom. This means always start at point 1
# if you plan to make a release and change the values.
# Every new library starts with a version 'current'  (short 'c') = 0
#                                         'revision' (short 'r') = 0
#                                         'age'      (short 'a') = 0
#
# Update the libtool versioning only after the release of a public release of libcoap.
# Go through the following checklist from top to bottom and check your needs, following
# the reminded changes if you can say "Yes" for specific check.
#
# 1. Only existing code has changed, no functional changes
#    If the library source code has changed but *no* new symbols were added at all
#    since the last update, then increment revision (c:r:a becomes c:r+1:a).
#    This is usually happen if the existing source of a function was changed for
#    bug fixing e.g.
#
#  --> Increase the 'LT_LIBCOAP_REVISION' value with *every* new software release
#      within one release cycle.
#
# 2. Interfaces were added, functions have changed or are removed
#    If any interfaces [exported functions or data] have been added, got internal
#    changes that implies a different behavior or removed and by this the visible
#    symbols have changed since the last update, increment current, and set
#    revision to 0 (c:r:a becomes c+1:r=0:a).
#
#  --> Increase the 'LT_LIBCOAP_CURRENT' value whenever as an interface has been added
#      or removed. This implies also a API change! You mostly have to change the
#      'libcoap_major_version' or at least 'libcoap_minor_version' then too!
#  --> Set 'LT_LIBCOAP_REVISION' to 0.
#
# 3. Interfaces were added but none removed or changed
#    If any interfaces have been added since the last public release and non of the
#    existing interfaces were removed and existing interfaces have not changed internally
#    functionality then the new library is backward compatible. Existing binaries can
#    use the new library the same than as the existing old library without loosing
#    existing functionality or breakage.
#    Increase age by 1 (c:r:a becomes c:r:a+1).
#
#  --> Increase the 'LT_LIBCOAP_AGE' value only if the changes made to the ABI are
#      backward compatible.
#
# 4. Interfaces were removed or have functional changes
#    If any interfaces within the library have been removed since the last public
#    release or got some internal changes that let the interface act different than
#    before, then set age to 0. The library isn't backwards compatible.
#
#  --> Set 'LT_LIBCOAP_AGE' to 0.

LT_LIBCOAP_CURRENT=1
LT_LIBCOAP_REVISION=0
LT_LIBCOAP_AGE=1
LIBCOAP_SO_VERSION=$LT_LIBCOAP_CURRENT.$LT_LIBCOAP_REVISION.$LT_LIBCOAP_AGE

# Announce the libtool version
AC_SUBST(LT_LIBCOAP_CURRENT)
AC_SUBST(LT_LIBCOAP_REVISION)
AC_SUBST(LT_LIBCOAP_AGE)
AC_SUBST(LIBCOAP_SO_VERSION)

# Defining the API Version
LIBCOAP_API_VERSION=2
AC_SUBST(LIBCOAP_API_VERSION)

# Adding some default warning options for code QS
# see https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html
# and http://www.gnu.org/software/automake/manual/html_node/Flag-Variables-Ordering.html
WARNING_CFLAGS="\
-pedantic \
-Wall \
-Wcast-qual \
-Wextra \
-Wformat-security \
-Winline \
-Wmissing-declarations \
-Wmissing-prototypes \
-Wnested-externs \
-Wpointer-arith \
-Wshadow \
-Wstrict-prototypes \
-Wswitch-default \
-Wswitch-enum \
-Wunused \
-Wwrite-strings \
"

# check whether or not the compiler supports -Wlogical-op (clang does not...)
AX_CHECK_COMPILE_FLAG([-Wlogical-op], [WARNING_CFLAGS="$WARNING_CFLAGS -Wlogical-op"],,[-Werror])
AX_CHECK_COMPILE_FLAG([-fdiagnostics-color], [CFLAGS="$CFLAGS -fdiagnostics-color"],,[-Werror])
AX_CHECK_COMPILE_FLAG([-Wunused-result], [WARNING_CFLAGS="$WARNING_CFLAGS -Wunused-result"])

AC_SUBST([WARNING_CFLAGS])

AX_CHECK_LINK_FLAG([-Wl,--version-script=${srcdir}/libcoap-${LIBCOAP_API_VERSION}.map],
                   [libcoap_SYMBOLS="-Wl,--version-script=\$(srcdir)/libcoap-\$(LIBCOAP_API_VERSION).map"],
                   [libcoap_SYMBOLS="-export-symbols \$(top_builddir)/libcoap-\$(LIBCOAP_API_VERSION).sym"])

AC_SUBST(libcoap_SYMBOLS)

# configure options
# __documentation__

AC_ARG_ENABLE([documentation],
              [AS_HELP_STRING([--enable-documentation],
                              [Enable building all the documentation [default=yes]])],
              [build_documentation="$enableval"],
              [build_documentation="yes"])

AM_CONDITIONAL(BUILD_DOCUMENTATION, [test "x$build_documentation" = "xyes"])

doxygen_version_required=1.7.0
dot_version_required=2.26.0

AC_ARG_ENABLE([doxygen],
              [AS_HELP_STRING([--enable-doxygen],
                              [Enable building the doxygen documentation [default=yes]])],
              [build_doxygen="$enableval"],
              [build_doxygen="yes"])

if test -z "$enable_doxygen"; then
    if test "x$enable_documentation" = "xno"; then
        build_doxygen="no"
    fi
fi

if test "x$build_doxygen" = "xyes"; then
    # Check for doxygen
    AC_PATH_PROGS([DOXYGEN], [doxygen])
    if test -z "$DOXYGEN"; then
        if test "x$build_doxygen" = "xyes"; then
            AC_MSG_WARN([==> You want to build the doxygen documentation but doxygen was not found!])
            AC_MSG_ERROR([==> Install the package that contains doxygen or disable the doxygen documentation using '--disable-doxygen'.])
        fi
    else
        AC_MSG_CHECKING([for compatible doxygen version (>= $doxygen_version_required)])
        doxygen_version=`$DOXYGEN --version`
        AS_VERSION_COMPARE([$doxygen_version],
                           [$doxygen_version_required],
                           [AC_MSG_RESULT([no])
                            DOXYGEN=""],
                           [AC_MSG_RESULT([yes $doxygen_version])],
                           [AC_MSG_RESULT([yes $doxygen_version])])
        if test "x$DOXYGEN" = "x" -a "x$build_doxygen" = "xyes"; then
            AC_MSG_WARN([==> Doxygen $doxygen_version too old. Doxygen >= $doxygen_version_required required for documentation build.])
            AC_MSG_ERROR([==> Install required doxygen version or disable the doxygen documentation using '--disable-doxygen'.])
        else
            # we found doxygen and the version is valid
            DOC_DIR=doc
            AC_SUBST(DOC_DIR)
            # now checking dot (needed for graphics)
            AC_PATH_PROG([DOT], [dot])
            if test "x$DOT" = "x"; then
                AC_MSG_WARN([==> dot not found - continuing without DOT support])
                AC_MSG_WARN([==> The libcoap html documentation will be build without DOT graphics!])
                HAVE_DOT="NO"
                USE_CALL_GRAPH="NO"
            else
                AC_MSG_CHECKING([for compatible dot version (>= $dot_version_required)])
                case $host in
                    *-freebsd1*)
                    # csh and tcsh have a different output redirection than more recent shells
                    # cmd >& file   # Redirect both stdout and stderr to file.
                    # cmd >>& file  # Append both stdout and stderr to file.
                    # cmd1 | cmd2   # pipe stdout to cmd2
                    # cmd1 |& cmd2  # pipe stdout and stderr to cmd2
                    # Using an explicit call with the default always available C-shell on FreeBSD,
                    # the user may have installed another shell from a port which we don't know here
                    dot_version=`export DOT=$DOT && csh -c '$DOT -V |& cut -f5 -d" "'`
                    ;;

                    *)
                    dot_version=`$DOT -V 2>&1 | cut -f5 -d" "`
                    ;;

                esac
                AS_VERSION_COMPARE([$dot_version],
                                   [$dot_version_required],
                                   [AC_MSG_RESULT([no])
                                    DOT=""],
                                   [AC_MSG_RESULT([yes $dot_version])],
                                   [AC_MSG_RESULT([yes $dot_version])])
                if test "x$DOT" = "x" -a "x$build_doxygen" = "xyes"; then
                    AC_MSG_WARN([==> Graphviz dot $dot_version too old. Graphviz >= $dot_version_required required for doxygen build.])
                    AC_MSG_ERROR([==> Install required graphviz version or disable the doxygen documentation using '--disable-doxygen'.])
                fi
                # we found dot and the version is valid
                HAVE_DOT="YES"
                # let doxygen create caller graphics
                # see http://www.stack.nl/~dimitri/doxygen/manual/config.html#cfg_call_graph
                USE_CALL_GRAPH="YES"
                # exporting the tests to doc/Doxygen(.in)
                AC_SUBST(HAVE_DOT)
                AC_SUBST(USE_CALL_GRAPH)
            fi
        fi
    fi
fi
AM_CONDITIONAL(BUILD_DOXYGEN, [test "x$build_doxygen" = "xyes"])

AM_CONDITIONAL([HAVE_DOXYGEN], [test -n "$DOXYGEN"])
AM_COND_IF([HAVE_DOXYGEN], [AC_CONFIG_FILES([doc/Doxyfile])])

# __manpages__

AC_ARG_ENABLE([manpages],
              [AS_HELP_STRING([--enable-manpages],
                              [Enable building the manpages [default=yes]])],
              [build_manpages="$enableval"],
              [build_manpages="yes"])

if test -z "$enable_manpages"; then
    if test "x$enable_documentation" = "xno"; then
        build_manpages="no"
    fi
fi

if test "x$build_manpages" = "xyes"; then
    AC_ARG_VAR([A2X], [a2x command])
    AC_PATH_PROG([A2X], [a2x])
    if test "x$A2X" = "x"; then
        AC_MSG_WARN([==> You want to build the manpages, but a2x was not found!])
        AC_MSG_ERROR([==> Install the package that contains a2x (mostly asciidoc) or disable the build of the manpages using '--disable-manpages'.])
        build_manpages="no"
    else
        AX_CHECK_A2X_TO_MANPAGE([], [
            AC_MSG_RESULT([no])
            AC_MSG_WARN([==> You want to build the manpages with a2x, but manpage formatting does not work!])
            AC_MSG_ERROR([==> Install the packages that contains the docbook DTD and XSL stylesheets (presumably docbook, docbook-xml) or disable the build of the manpages using '--disable-manpages'.])
            build_manpages="no"
        ])
        MAN_DIR=man
        AC_SUBST(MAN_DIR)
    fi
fi
AM_CONDITIONAL(BUILD_MANPAGES, [test "x$build_manpages" = "xyes"])

# configure options
# __dtls__
# The Datagram Transport Layer Security (DTLS) feature needs cryptography
# functions.
# We currently support the GnuTLS and OpenSSL library. The user can preselect
# the cryptography library that should be used by adding '--with-gnutls' or
# '--with-openssl'.
# If the user isn't using a selection we first search for GnuTLS and fallback
# to OpenSSL if the GnuTLS library couldn't be found.

gnutls_version_required=3.3.0
openssl_version_required=1.1.0

AC_ARG_ENABLE([dtls],
              [AS_HELP_STRING([--enable-dtls],
                              [Enable building with DTLS support [default=yes]])],
              [build_dtls="$enableval"],
              [build_dtls="yes"])

AC_ARG_WITH([gnutls],
            [AS_HELP_STRING([--with-gnutls],
                            [Use GnuTLS for DTLS functions])],
            [with_gnutls="$withval"],
            [with_gnutls="no"])

AC_ARG_WITH([openssl],
            [AS_HELP_STRING([--with-openssl],
                            [Use OpenSSL for DTLS functions])],
            [with_openssl="$withval"],
            [with_openssl="no"])

AC_ARG_WITH([tinydtls],
           [AS_HELP_STRING([--with-tinydtls],
                           [Use TinyDTLS for DTLS functions])],
           [with_tinydtls="$withval"],
           [with_tinydtls="no"])

if test "x$with_gnutls" = "xyes" -o "x$with_openssl" = "xyes"; then
    if test "x$build_dtls" = "x"; then
        # Give an advice that '--with_gnutls' or '--with_openssl' was used but
        # DTLS support isn't configured.
        AC_MSG_NOTICE([==> Using the configure options '--with-gnutls' or '--with-openssl' without '--enable-dtls' is useles and will be ignored.])
    fi
fi

# O.K. the user hasn't de-selected DTLS
if test "x$build_dtls" = "xyes"; then
    # The user can't select multiple crypto libraries.
    if test "x$with_gnutls" = "xyes" -a "x$with_openssl" = "xyes"; then
        AC_MSG_ERROR([==> You can't use the options '--with-gnutls' and '--with-openssl' at the same time while '--enable-dtls' is selected!
                  ==> Please note, the option '--enable-dtls' is turned on by default if not explicitly disabled!])
    fi

    # Check for all possible usable and supported SSL crypto libraries
    # GnuTLS
    PKG_CHECK_MODULES([GnuTLS],
                      [gnutls],
                      [have_gnutls="yes"],
                      [have_gnutls="no"])

    # OpenSSL
    PKG_CHECK_MODULES([OpenSSL],
                      [openssl],
                      [have_openssl="yes"],
                      [have_openssl="no"])

    # TinyDTLS ?
    # TBD ?

    # The user wants to use explicit GnuTLS if '--with-gnutls' was set.
    if test "x$with_gnutls" = "xyes"; then
        # Some more sanity checking.
        if test "x$have_gnutls" != "xyes"; then
            AC_MSG_ERROR([==> You want to build libcoap with DTLS support by the GnuTLS library but pkg-config file 'gnutls.pc' could not be found!
                      Install the package(s) that contains the development files for GnuTLS,
                      or use '--with-openssl' or disable the DTLS support using '--disable-dtls'.])
        fi
        AC_MSG_NOTICE([The use of GnuTLS was explicit requested due configure option '--with-gnutls'!])

        # check for valid GnuTLS version
        gnutls_version=`pkg-config --modversion gnutls`
        AX_CHECK_GNUTLS_VERSION
        have_openssl="no" # don't confuse AC_MSG_RESULT at the end of the script
    fi

    # The user wants to use explicit OpenSSL if '--with-openssl was set'.
    if test "x$with_openssl" = "xyes"; then
        # Some more sanity checking.
        if test "x$have_openssl" != "xyes"; then
            AC_MSG_ERROR([==> You want to build libcoap with DTLS support by the OpenSSL library but pkg-config file 'openssl.pc' could not be found!
                      Install the package(s) that contains the development files for OpenSSL,
                      or use '--with-gnutls' or disable the DTLS support using '--disable-dtls'.])
        fi
        AC_MSG_NOTICE([The use of OpenSSL was explicit requested due configure option '--with-openssl'!])

        # check for valid OpenSSL version
        openssl_version=`pkg-config --modversion openssl`
        AX_CHECK_OPENSSL_VERSION
        have_gnutls="no" # don't confuse AC_MSG_RESULT at the end of the script
    fi

    # The user wants to use explicit OpenSSL if '--with-tinydtls was set'.
    if test "x$with_tinydtls" = "xyes" ; then
        if test -d "$srcdir/ext/tinydtls"; then
           AC_CONFIG_SUBDIRS([ext/tinydtls])
           have_tinydtls="yes"
         else
           have_tinydtls="no"
         fi
    fi

    # The user hasn't requested the use of a specific cryptography library
    # we try first GnuTLS for usability ...
    if test "x$have_gnutls" = "xyes" -a "x$with_gnutls" = "xno" -a "x$with_tinydtls" = "xno"; then
        gnutls_version=`pkg-config --modversion gnutls`
        AX_CHECK_GNUTLS_VERSION
        AC_MSG_NOTICE([Using auto selected library GnuTLS for DTLS support!])
        with_gnutls_auto="yes"
        have_openssl="no" # don't confuse AC_MSG_RESULT at the end of the script
    fi

    # ... and if not found check OpenSSL is suitable.
    if test "x$have_openssl" = "xyes" -a "x$with_openssl" = "xno" -a "x$with_gnutls_auto" = "x" -a "x$with_tinydtls" = "xno"; then
        openssl_version=`pkg-config --modversion openssl`
        AX_CHECK_OPENSSL_VERSION
        AC_MSG_NOTICE([Using auto selected library OpenSSL for DTLS support!])
        with_openssl_auto="yes"
        have_gnutls="no" # don't confuse AC_MSG_RESULT at the end of the script
    fi

    # Note that tinyDTLS is used only when explicitly requested.

    # Giving out an error message if we haven't found at least one crypto library.
    if test "x$have_gnutls" = "xno" -a "x$have_openssl" = "xno" -a "x$have_tinydtls" = "xno"; then
        AC_MSG_ERROR([==> Option '--enable-dtls' is set but one of the needed cryptography library GnuTLS nor OpenSSL nor tinyDTLS could be found!
                      Install at least one of the package(s) that contains the development files for GnuTLS (>= $gnutls_version_required) or OpenSSL(>= $openssl_version_required)
                      or disable the DTLS support using '--disable-dtls'.])
    fi

    # Saving the DTLS related Compiler flags.
    if test "x$with_gnutls" = "xyes" -o "x$with_gnutls_auto" = "xyes"; then
        DTLS_CFLAGS="$GnuTLS_CFLAGS"
        DTLS_LIBS="$GnuTLS_LIBS"
        AC_DEFINE(HAVE_LIBGNUTLS, [1], [Define if the system has libgnutls28])
    fi
    if test "x$with_openssl" = "xyes" -o "x$with_openssl_auto" = "xyes"; then
        DTLS_CFLAGS="$OpenSSL_CFLAGS"
        DTLS_LIBS="$OpenSSL_LIBS"
        AC_DEFINE(HAVE_OPENSSL, [1], [Define if the system has libssl1.1])
    fi
    if test "x$with_tinydtls" = "xyes"; then
        DTLS_CFLAGS="-I \$(top_srcdir)/ext/tinydtls"
        if test "x$enable_shared" = "xyes"; then
            DTLS_LIBS="-L\$(top_builddir)/ext/tinydtls -ltinydtls"
        else
            # Needed as TinyDTLS compiling does not recognize --disable-shared
            # and still builds libtinydtls.so which gets linked in otherwise
            DTLS_LIBS="-L\$(top_builddir)/ext/tinydtls -l:libtinydtls.a"
        fi
        AC_DEFINE(HAVE_LIBTINYDTLS, [1], [Define if the system has libtinydtls])
    fi
    AC_SUBST(DTLS_CFLAGS)
    AC_SUBST(DTLS_LIBS)
fi

# Define the Library name extension for the TLS the library was linked against
if test "x$with_openssl" = "xyes" -o "x$with_openssl_auto" = "xyes"; then
    LIBCOAP_DTLS_LIB_EXTENSION_NAME=-openssl
elif test "x$with_gnutls" = "xyes" -o "x$with_gnutls_auto" = "xyes"; then
    LIBCOAP_DTLS_LIB_EXTENSION_NAME=-gnutls
elif test "x$with_tinydtls" = "xyes"; then
    LIBCOAP_DTLS_LIB_EXTENSION_NAME=-tinydtls
else
    LIBCOAP_DTLS_LIB_EXTENSION_NAME=
fi

if test "x$LIBCOAP_DTLS_LIB_EXTENSION_NAME" != "x"; then
    LIBCOAP_NAME_SUFFIX="$LIBCOAP_API_VERSION$LIBCOAP_DTLS_LIB_EXTENSION_NAME"
else
    LIBCOAP_NAME_SUFFIX="$LIBCOAP_API_VERSION"
fi

AC_SUBST(LIBCOAP_NAME_SUFFIX)
AC_SUBST(LIBCOAP_DTLS_LIB_EXTENSION_NAME)
AC_SUBST([DOLLAR_SIGN],[$])

# configure options
# __tests__
AC_ARG_ENABLE([tests],
              [AS_HELP_STRING([--enable-tests],
                              [Enable building the binary testsuite [default=no]])],
              [build_tests="$enableval"],
              [build_tests="no"])

if test "x$build_tests" = "xyes"; then
    PKG_CHECK_MODULES([CUNIT],
                      [cunit],
                      [have_cunit=yes
                       AC_DEFINE(HAVE_LIBCUNIT, [1], [Define if the system has libcunit])],
                      [have_cunit=no
                       AC_MSG_WARN([==> You want to build the testing binary but the pkg-config file cunit.pc could not be found or installed CUnit version is too old!])
                       AC_MSG_ERROR([==> Install the package(s) that contains the development files for CUnit or disable the testing binary using '--disable-tests'.])
                      ])
fi
AM_CONDITIONAL(HAVE_CUNIT, [test "x$CUNIT_LIBS" != "x"])

# configure options
# __examples__
AC_ARG_ENABLE([examples],
              [AS_HELP_STRING([--enable-examples],
                              [Enable building the example binaries [default=yes]])],
              [build_examples="$enableval"],
              [build_examples="yes"])

AM_CONDITIONAL(BUILD_EXAMPLES, [test "x$build_examples" = "xyes"])

# configure options
# __gcov__
AC_ARG_ENABLE([gcov],
              [AS_HELP_STRING([--enable-gcov],
                              [Enable building with gcov test coverage support [default=no]])],
              [build_gcov="$enableval"],
              [build_gcov="no"
               AC_MSG_WARN([gcov is disabled])
              ])

if test "x$build_gcov" = "xyes"; then
   if test "x$GCC" != "xyes"; then
        AC_MSG_ERROR([Currently, gcov is assumed to work with GCC-compatible compilers only.])
   else
        AX_CHECK_COMPILE_FLAG([-fprofile-arcs], [CFLAGS="$CFLAGS -fprofile-arcs"])
        AX_CHECK_COMPILE_FLAG([-ftest-coverage], [CFLAGS="$CFLAGS -ftest-coverage"])
        # FIXME: clang complains about '--coverage'
        AX_CHECK_COMPILE_FLAG([--coverage], [CFLAGS="$CFLAGS --coverage -O0" LDFLAGS="$LDFLAGS --coverage"])
   fi
fi

# end configure options
#######################

###########################################
# from configure options independent checks

# Check for (ex)ctags binary
# The needed ctags binary name differs on FreeBSD and Linux, on Linux
# systems we search for 'ctags', on FreeBSD for 'exctags'
case $host in
    # FreeBSD has exctags from the ctags port
    *-freebsd1*)
    AC_ARG_VAR([CTAGS_PROG],[the 'exctags' program to use for make target 'update-map-file'])
    AC_PATH_PROG([CTAGS_PROG],[exctags])
    ;;

    *)
    # Linux distributions have exuberant-ctags
    AC_ARG_VAR([CTAGS_PROG],[the 'ctags' program to use for make target 'update-map-file'])
    AC_PATH_PROG([CTAGS_PROG],[ctags])
    ;;

esac

if test "x$CTAGS_PROG" = "x"; then
    AC_MSG_NOTICE([==> Note: '(ex)ctags' command not found!])
    AC_MSG_WARN([==> Without ctags you will be unable to run the target 'update-map-file'!])
    AC_MSG_WARN([==> This is no problem if you just want to build the library libcoap.])
else
    if test "`$CTAGS_PROG --help | grep '\--<LANG>-kinds'`" = ""; then
        AC_MSG_NOTICE([==> Note: Your ctags binary does not support '--c-kinds'!])
        AC_MSG_NOTICE([==> Most likely, you are using the GNU Emacs ctag and not exuberant ctag.])
        AC_MSG_WARN([==> This option is required for the target 'update-map-file'.])
        AC_MSG_WARN([==> which is not a problem if you just want to build the library libcoap.])
    fi
fi

# Checks for header files.
AC_CHECK_HEADERS([assert.h arpa/inet.h limits.h netdb.h netinet/in.h \
                  stdlib.h string.h strings.h sys/socket.h sys/time.h \
                  time.h unistd.h sys/unistd.h syslog.h sys/ioctl.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T

# Checks for library functions.
AC_CHECK_FUNCS([memset select socket strcasecmp strrchr getaddrinfo \
                strnlen malloc])

# Check if -lsocket -lnsl is required (specifically Solaris)
AC_SEARCH_LIBS([socket], [socket])
AC_SEARCH_LIBS([inet_ntop], [nsl])

# Check if clock_gettime() requires librt, when available
AC_SEARCH_LIBS([clock_gettime], [rt])

#check for struct cmsghdr
AC_CHECK_TYPES([struct cmsghdr],,,[
AC_INCLUDES_DEFAULT
#include <sys/socket.h>])

AC_MSG_CHECKING([operating system])

# Set up here some extra platform depended defines and variables.
# The "ADDITIONAL_CFLAGS" is need as this stand-alone definition
# for the doxygen part.
case $host in
    *-linux* | *-uclinux*)
    AC_MSG_RESULT([Linux])
    ADDITIONAL_CFLAGS="-D_GNU_SOURCE"

    # Not yet needed but if some code definitions have to depend on the platform.
    #AC_DEFINE(OS_LINUX, 1, [Linux backend])
    #AC_SUBST(OS_LINUX)
    ;;

    *-solaris*)
    AC_MSG_RESULT([Solaris])
    # set _XOPEN_SOURCE and _XOPEN_SOURCE_EXTENDED to enable XPG4v2 (POSIX 2004)
    # set __EXTENSION__ to shut up feature test macros that restrict -std=c99
    # to only C99 (and nothing newer)
    ADDITIONAL_CFLAGS="-D_XOPEN_SOURCE=600 -D_XOPEN_SOURCE_EXTENDED=600 -D__EXTENSIONS__=1"
    ;;

    *-darwin*)
    AC_MSG_RESULT([Darwin])
    ADDITIONAL_CFLAGS="-D_GNU_SOURCE"

    AC_DEFINE(__APPLE_USE_RFC_3542, 1, [Define this to 1 for ancillary data on MacOS])

    # Not yet needed but if some code definitions have to depend on the platform.
    #AC_DEFINE(OS_MACOS, 1, [MacOS backend])
    #AC_SUBST(OS_MACOS)
    ;;

    *-freebsd1*)
    AC_MSG_RESULT([FreeBSD-1x])
    ADDITIONAL_CFLAGS="-D_GNU_SOURCE"
    ;;

    *kfreebsd*)
    AC_MSG_RESULT([kFreeBSD])
    ADDITIONAL_CFLAGS="-D_GNU_SOURCE"
    ;;

    *)
    AC_MSG_WARN([==> Currently unsupported operating system '${host}' !])
    AC_MSG_ERROR([==> If you can provide patches to support your operating system please write to 'libcoap-developers@lists.sourceforge.net'.])
esac

# Exporting the PREDEFINED_CFLAGS definition
PREDEFINED_CFLAGS=`echo $ADDITIONAL_CFLAGS | $SED -e 's/-D//g'`
AC_SUBST(PREDEFINED_CFLAGS)

# And finally combining the CFLAGS together ...
CFLAGS="$CFLAGS $ADDITIONAL_CFLAGS"

# Override the various template files, currently just makefiles and the
# pkgconfig *.pc file.
# Later if the API version is changing don't forget to change the
# libcoap-$LIBCOAP_API_VERSION.pc.in file too!! You will have to change
# the 'Cflags' variable to something like
#     Cflags: -I${includedir}/coap-@LIBCOAP_API_VERSION@
#
AC_CONFIG_FILES([
Makefile
doc/Makefile
examples/Makefile
include/coap2/coap.h
man/coap.txt
man/coap_attribute.txt
man/coap_context.txt
man/coap_encryption.txt
man/coap_handler.txt
man/coap_observe.txt
man/coap_logging.txt
man/coap_pdu_setup.txt
man/coap_recovery.txt
man/coap_resource.txt
man/coap_session.txt
man/coap_tls_library.txt
man/coap-client.txt
man/coap-server.txt
man/coap-rd.txt
man/Makefile
tests/Makefile
tests/oss-fuzz/Makefile.ci
libcoap-$LIBCOAP_NAME_SUFFIX.pc:libcoap-$LIBCOAP_API_VERSION.pc.in
])

AC_OUTPUT

AC_MSG_RESULT([
libcoap configuration summary:
      libcoap package version : "$PACKAGE_VERSION"
      libcoap library version : "$LIBCOAP_SO_VERSION"
      libcoap API version     : "$LIBCOAP_API_VERSION"
      libcoap DTLS lib extn   : "$LIBCOAP_DTLS_LIB_EXTENSION_NAME"
      host system             : "$host"]);
if test "x$with_gnutls" = "xyes" -o "x$with_gnutls_auto" = "xyes"; then
    AC_MSG_RESULT([      build DTLS support      : "yes"])
    AC_MSG_RESULT([          -->  GnuTLS around  : "yes" (found GnuTLS $gnutls_version)])
    AC_MSG_RESULT([               GNUTLS_CFLAGS  : "$GnuTLS_CFLAGS"])
    AC_MSG_RESULT([               GNUTLS_LIBS    : "$GnuTLS_LIBS"])
fi
if test "x$with_openssl" = "xyes" -o "x$with_openssl_auto" = "xyes"; then
    AC_MSG_RESULT([      build DTLS support      : "yes"])
    AC_MSG_RESULT([         -->  OpenSSL around  : "yes" (found OpenSSL $openssl_version)])
    AC_MSG_RESULT([              OPENSSL_CFLAGS  : "$OpenSSL_CFLAGS"])
    AC_MSG_RESULT([              OPENSSL_LIBS    : "$OpenSSL_LIBS"])
fi
if test "x$with_tinydtls" = "xyes"; then
    AC_MSG_RESULT([      build DTLS support      : "yes"])
    AC_MSG_RESULT([         -->  tinyDTLS around : "yes"])
    AC_MSG_RESULT([              TINYDTLS_CFLAGS : "$DTLS_CFLAGS"])
    AC_MSG_RESULT([              TINYDTLS_LIBS   : "$DTLS_LIBS"])
fi
if test "x$build_dtls" != "xyes"; then
    AC_MSG_RESULT([      build DTLS support      : "no"])
fi
if test "x$build_doxygen" = "xyes"; then
    AC_MSG_RESULT([      build doxygen pages     : "yes"])
    AC_MSG_RESULT([          --> Doxygen around  : "yes" ($DOXYGEN $doxygen_version)])
    if test "x$DOT" = "x"; then
        AC_MSG_RESULT([             -->  dot around  : "no" (DOT not found!)])
    else
        AC_MSG_RESULT([             -->  dot around  : "yes" ($DOT $dot_version)])
    fi
else
    if test "x$build_doxygen" = "xno"; then
        AC_MSG_RESULT([      build doxygen pages     : "no"])
    fi
fi
if test "x$build_manpages" = "xyes"; then
    AC_MSG_RESULT([      build man pages         : "yes"])
else
    AC_MSG_RESULT([      build man pages         : "no"])
fi
if test "x$build_tests" = "xyes"; then
    AC_MSG_RESULT([      build unit test binary  : "yes"])
else
    AC_MSG_RESULT([      build unit test binary  : "no"])
fi
if test "x$build_examples" = "xyes"; then
    AC_MSG_RESULT([      build examples          : "yes"])
else
    AC_MSG_RESULT([      build examples          : "no"])
fi
if test "x$build_gcov" = "xyes"; then
    AC_MSG_RESULT([      build with gcov support : "yes"])
else
    AC_MSG_RESULT([      build with gcov support : "no"])
fi
