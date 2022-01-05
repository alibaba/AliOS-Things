#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
#
# Copyright (c) 2011, 2012, 2013, 2014, 2015 Olaf Bergmann (TZI) and others.
# All rights reserved. This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v1.0
# and Eclipse Distribution License v. 1.0 which accompanies this distribution.
#
# The Eclipse Public License is available at http://www.eclipse.org/legal/epl-v10.html
# and the Eclipse Distribution License is available at 
# http://www.eclipse.org/org/documents/edl-v10.php.
#
# Contributors:
#    Olaf Bergmann  - initial API and implementation
#    Hauke Mehrtens - memory optimization, ECC integration

AC_PREREQ([2.65])
AC_INIT([tinydtls], [0.8.6])
AC_CONFIG_SRCDIR([dtls.c])
dnl AC_CONFIG_HEADERS([config.h])

AC_PATH_PROG(DOXYGEN, doxygen, [:])
AC_PATH_PROG(ETAGS, etags, [/bin/false])

# Make configure happy
test -e install-sh || touch install-sh

# Checks for programs.
AC_PROG_MAKE_SET
AC_PROG_CC
AC_PROG_RANLIB
m4_ifdef([AM_PROG_AR], [AM_PROG_AR], [AR=ar])

AC_C_BIGENDIAN

CFLAGS="-fPIC"

# Adding some default warning options for code QS
# see https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html
# and http://www.gnu.org/software/automake/manual/html_node/Flag-Variables-Ordering.html
WARNING_CFLAGS="\
-pedantic \
-Wall \
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
"

AC_SUBST([WARNING_CFLAGS])

# Checks for libraries.
AC_SEARCH_LIBS([gethostbyname], [nsl])
AC_SEARCH_LIBS([socket], [socket])

AC_ARG_WITH(debug,
  [AS_HELP_STRING([--without-debug],[disable all debug output and assertions])],
  [CPPFLAGS="${CPPFLAGS} -DNDEBUG"
   NDEBUG=1], 
  [])

AC_ARG_WITH(ecc,
  [AS_HELP_STRING([--without-ecc],[disable support for TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8])],
  [],
  [AC_DEFINE(DTLS_ECC, 1, [Define to 1 if building with ECC support.])
   OPT_OBJS="${OPT_OBJS} ecc/ecc.o"
   DTLS_ECC=1])

AC_ARG_WITH(psk,
  [AS_HELP_STRING([--without-psk],[disable support for TLS_PSK_WITH_AES_128_CCM_8])],
  [],
  [AC_DEFINE(DTLS_PSK, 1, [Define to 1 if building with PSK support])
   DTLS_PSK=1])

CPPFLAGS="${CPPFLAGS} -DDTLSv12 -DWITH_SHA256"
OPT_OBJS="${OPT_OBJS} sha2/sha2.o"

AC_SUBST(OPT_OBJS)
AC_SUBST(NDEBUG)
AC_SUBST(DTLS_ECC)
AC_SUBST(DTLS_PSK)
AC_SUBST(AR)

# Checks for header files.
AC_CHECK_HEADERS([assert.h arpa/inet.h fcntl.h inttypes.h netdb.h netinet/in.h stddef.h stdint.h stdlib.h string.h strings.h sys/param.h sys/socket.h unistd.h])

AC_CHECK_HEADERS([sys/time.h time.h])
AC_CHECK_HEADERS([sys/types.h sys/stat.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_SIZE_T

AC_CHECK_MEMBER([struct sockaddr_in6.sin6_len],
		[AC_DEFINE(HAVE_SOCKADDR_IN6_SIN6_LEN, [1], 
                  [Define to 1 if struct sockaddr_in6 has a member sin6_len.])], [], 
		[#include <netinet/in.h>])

# Checks for library functions.
AC_CHECK_FUNCS([memset select socket strdup strerror strnlen fls vprintf])

AC_CONFIG_HEADERS([dtls_config.h])


AC_CONFIG_FILES([Makefile
                 doc/Makefile
                 doc/Doxyfile
                 tests/Makefile
                 platform-specific/Makefile
		 sha2/Makefile
		 aes/Makefile
		 ecc/Makefile])
AC_OUTPUT
