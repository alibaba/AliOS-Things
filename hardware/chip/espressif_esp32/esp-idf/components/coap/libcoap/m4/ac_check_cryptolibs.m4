#
# SYNOPSIS
#
#   AX_CHECK_{GNUTLS|OPENSSL}_VERSION
#
# DESCRIPTION
#
#   This m4 file contains helper functions for checking the version of the
#   respective cryptographic library version of GnuTLS or OpenSSL on the
#   host. The variables '$gnutls_version_require', '$openssl_version_required',
#   hold the minimum required version and are set up externaly in configure.ac.
#
#   Example:
#
#     AX_CHECK_GNUTLS_VERSION
#    or
#     AX_CHECK_OPENSSL_VERSION
#
# LICENSE
#
#   Copyright (c) 2017 Carsten Schoenert <c.schoenert@t-online.de>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

AC_DEFUN([AX_CHECK_GNUTLS_VERSION],
         [AC_MSG_CHECKING([for compatible GnuTLS version (>= $gnutls_version_required)])
          AS_VERSION_COMPARE([$gnutls_version],
                             [$gnutls_version_required],
                             [AC_MSG_RESULT([no])
                              GNUTLSV=""],
                             [AC_MSG_RESULT([yes $gnutls_version])
                              GNUTLSV="$gnutls_version"],
                             [AC_MSG_RESULT([yes $gnutls_version])
                              GNUTLSV="$gnutls_version"])
          if test "x$GNUTLSV" = "x"; then
              AC_MSG_ERROR([==> GnuTLS $gnutls_version too old. GnuTLS >= $gnutls_version_required required for suitable DTLS support build.])
          fi
]) dnl AX_CHECK_GNUTLS_VERSION

AC_DEFUN([AX_CHECK_OPENSSL_VERSION],
         [AC_MSG_CHECKING([for compatible OpenSSL version (>= $openssl_version_required)])
          AS_VERSION_COMPARE([$openssl_version], [$openssl_version_required],
                             [AC_MSG_RESULT([no])
                              OPENSSLV=""],
                             [AC_MSG_RESULT([yes $openssl_version])
                              OPENSSLV="$openssl_version"],
                             [AC_MSG_RESULT([yes $openssl_version])
                              OPENSSLV="$openssl_version"])
          if test "x$OPENSSLV" = "x"; then
              AC_MSG_ERROR([==> OpenSSL $openssl_version too old. OpenSSL >= $openssl_version_required required for suitable DTLS support build.])
          fi
]) dnl AX_CHECK_OPENSSL_VERSION

