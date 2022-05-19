AC_PREREQ(2.63)

AC_INIT([protobuf-c],
        [1.3.0],
        [https://github.com/protobuf-c/protobuf-c/issues],
        [protobuf-c],
        [https://github.com/protobuf-c/protobuf-c])
PACKAGE_DESCRIPTION="Protocol Buffers implementation in C"
AC_SUBST(PACKAGE_DESCRIPTION)

AC_CONFIG_SRCDIR([protobuf-c/protobuf-c.c])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([foreign 1.11 -Wall -Wno-portability silent-rules subdir-objects])
AC_PROG_CC_STDC
AC_PROG_CXX
AC_PROG_LN_S
AC_PROG_MKDIR_P
AC_USE_SYSTEM_EXTENSIONS
AC_SYS_LARGEFILE
AC_CONFIG_MACRO_DIR([m4])
AM_SILENT_RULES([yes])
LT_INIT

AC_CONFIG_HEADERS(config.h)
AC_CONFIG_FILES([Makefile protobuf-c/libprotobuf-c.pc])

my_CFLAGS="\
-Wall \
-Wchar-subscripts \
-Wdeclaration-after-statement \
-Wformat-security \
-Wmissing-declarations \
-Wmissing-prototypes \
-Wnested-externs \
-Wpointer-arith \
-Wshadow \
-Wsign-compare \
-Wstrict-prototypes \
-Wtype-limits \
"
#AX_CHECK_COMPILE_FLAG(["-Wc90-c99-compat"],
#                      [my_CFLAGS="$my_CFLAGS -Wc90-c99-compat"])
AX_CHECK_COMPILE_FLAG(["-Wc99-c11-compat"],
                      [my_CFLAGS="$my_CFLAGS -Wc99-c11-compat"])
AX_CHECK_COMPILE_FLAG(["-Werror=incompatible-pointer-types"],
                      [my_CFLAGS="$my_CFLAGS -Werror=incompatible-pointer-types"])
AX_CHECK_COMPILE_FLAG(["-Werror=int-conversion"],
                      [my_CFLAGS="$my_CFLAGS -Werror=int-conversion"])
AX_CHECK_COMPILE_FLAG(["-Wnull-dereference"],
                      [my_CFLAGS="$my_CFLAGS -Wnull-dereference"])
AC_SUBST([my_CFLAGS])

AC_CHECK_PROGS([DOXYGEN], [doxygen])
AM_CONDITIONAL([HAVE_DOXYGEN],
               [test -n "$DOXYGEN"])
AM_COND_IF([HAVE_DOXYGEN],
           [AC_CONFIG_FILES([Doxyfile])
            DOXYGEN_INPUT="${srcdir}/protobuf-c"
            AC_SUBST(DOXYGEN_INPUT)
           ])

PKG_PROG_PKG_CONFIG
if test -n "$PKG_CONFIG"; then
    # Horrible hack for systems where the pkg-config install directory is simply wrong!
    if $PKG_CONFIG --variable=pc_path pkg-config 2>/dev/null | grep -q /libdata/; then
        PKG_INSTALLDIR(['${prefix}/libdata/pkgconfig'])
    else
        PKG_INSTALLDIR
    fi
fi

proto3_supported="no"

AC_ARG_ENABLE([protoc],
  AS_HELP_STRING([--disable-protoc], [Disable building protoc_c (also disables tests)]))
if test "x$enable_protoc" != "xno"; then
  AC_LANG_PUSH([C++])

  PKG_CHECK_MODULES([protobuf], [protobuf >= 3.0.0],
    [proto3_supported=yes],
    [PKG_CHECK_MODULES([protobuf], [protobuf >= 2.6.0])]
  )

  save_CPPFLAGS="$CPPFLAGS"
  CPPFLAGS="$save_CPPFLAGS $protobuf_CFLAGS"
  AC_CHECK_HEADERS([google/protobuf/compiler/command_line_interface.h],
                   [],
                   [AC_MSG_ERROR([required protobuf header file not found])])
  CPPFLAGS="$save_CPPFLAGS"

  AC_ARG_VAR([PROTOC], [protobuf compiler command])
  AC_PATH_PROG([PROTOC], [protoc], [],
               [`$PKG_CONFIG --variable=exec_prefix protobuf`/bin:$PATH])
  if test -z "$PROTOC"; then
    AC_MSG_ERROR([Please install the protobuf compiler from https://code.google.com/p/protobuf/.])
  fi

  PROTOBUF_VERSION="$($PROTOC --version)"

else
  PROTOBUF_VERSION="not required, not building compiler"
fi

AM_CONDITIONAL([BUILD_COMPILER], [test "x$enable_protoc" != "xno"])
AM_CONDITIONAL([BUILD_PROTO3], [test "x$proto3_supported" != "xno"])
AM_CONDITIONAL([CROSS_COMPILING], [test "x$cross_compiling" != "xno"])

AM_COND_IF([BUILD_PROTO3], [AC_DEFINE([HAVE_PROTO3], [1], [Support proto3 syntax])])

gl_LD_VERSION_SCRIPT

gl_VALGRIND_TESTS

MY_CODE_COVERAGE

AC_C_BIGENDIAN

AC_OUTPUT
AC_MSG_RESULT([
    $PACKAGE $VERSION

        CC:                     ${CC}
        CFLAGS:                 ${CFLAGS}
        CXX:                    ${CXX}
        CXXFLAGS:               ${CXXFLAGS}
        LDFLAGS:                ${LDFLAGS}
        LIBS:                   ${LIBS}

        prefix:                 ${prefix}
        sysconfdir:             ${sysconfdir}
        libdir:                 ${libdir}
        includedir:             ${includedir}
        pkgconfigdir:           ${pkgconfigdir}

        bigendian:              ${ac_cv_c_bigendian}
        protobuf version:       ${PROTOBUF_VERSION}
])
