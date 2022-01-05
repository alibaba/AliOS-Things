#! /bin/sh

# This script creates a libcoap archive, unpacks it and does an
# out-of-tree build and installation afterwards. The environment
# variables PLATFORM, TESTS, TLS and DOCS control the build
# configuration. Suggested invocation from travis CI is:
#
# PLATFORM=posix TESTS=yes TLS=no DOCS=yes scripts/dist.sh
#
# Copyright (C) 2018 Olaf Bergmann <bergmann@tzi.org>
#
# This file is part of the CoAP C library libcoap. Please see README
# and COPYING for terms of use.
#

if test "x${PLATFORM}" != "xposix"; then
    exit 0
fi

# where to install the library
WORK=`pwd`/libcoap-build
mkdir -p $WORK || exit 1
PREFIX="--prefix=`pwd`/libcoap-installation"
SILENT="--enable-silent-rules"

WITH_TESTS="`scripts/fix-cunit.sh` --enable-tests"
test -f `pwd`/cunit.pc && echo cat `pwd`/cunit.pc

case "x${TLS}" in
    xno)       WITH_TLS="--disable-dtls"
               ;;
    xopenssl)  WITH_TLS="--with-openssl"
               ;;
    xgnutls)   WITH_TLS="--with-gnutls"
               ;;
    xtinydtls) WITH_TLS="--with-tinydtls --disable-shared"
               ;;
    *)         WITH_TLS="--with-gnutls"
               ;;
esac

case "x${DOCS}" in
    xyes)      WITH_DOCS="--enable-documentation"
               ;;
    *)         WITH_DOCS="--disable-documentation"
               ;;
esac

config() {
    echo "./configure $*"
    ./configure $* || cat config.log
}

work_dir=$PWD
config "$WITH_TESTS $SILENT --enable-documentation --enable-examples --disable-dtls" && make dist

err=$?
ARCHIVE=`ls -1t libcoap-*.tar.bz2 |head -1`
echo $ARCHIVE
if test $err = 0 -a "x$ARCHIVE" != "x"; then
    DIR=`pwd`/`tar taf $ARCHIVE |cut -d/ -f1|head -1`
    tar xaf $ARCHIVE && cd $WORK && \
        $DIR/configure $PREFIX $WITH_TESTS $SILENT $WITH_DOCS --enable-examples $WITH_TLS && \
        make && make install
    err=$?
fi

exit $err
