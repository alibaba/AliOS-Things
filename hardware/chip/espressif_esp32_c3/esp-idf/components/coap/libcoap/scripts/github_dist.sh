#! /bin/sh

# This script creates a libcoap archive, unpacks it and does an
# out-of-tree build and installation afterwards.
#
# Copyright (C) 2021 Olaf Bergmann <bergmann@tzi.org>
#
# This file is part of the CoAP C library libcoap. Please see README
# and COPYING for terms of use.
#

PREFIX=--prefix=`pwd`/libcoap-install
ARCHIVE=`ls -1t libcoap-*.tar.bz2 |head -1`
err=$?
echo $ARCHIVE
if test $err = 0 -a "x$ARCHIVE" != "x"; then
    DIR=`pwd`/`tar taf $ARCHIVE |cut -d/ -f1|head -1`
    tar xaf $ARCHIVE && cd $DIR && \
        $DIR/configure $PREFIX --enable-tests  --enable-silent-rules --enable-documentation --enable-examples --disable-dtls && \
        make && make install
    err=$?
fi

exit $err
