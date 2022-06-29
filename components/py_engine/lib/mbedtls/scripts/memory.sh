#!/bin/sh

# Measure memory usage of a minimal client using a small configuration
# Currently hardwired to ccm-psk and suite-b, may be expanded later
#
# Use different build options for measuring executable size and memory usage,
# since for memory we want debug information.
#
# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
#
# This file is provided under the Apache License 2.0, or the
# GNU General Public License v2.0 or later.
#
# **********
# Apache License 2.0:
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may
# not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# **********
#
# **********
# GNU General Public License v2.0 or later:
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#
# **********

set -eu

CONFIG_H='include/mbedtls/config.h'

CLIENT='mini_client'

CFLAGS_EXEC='-fno-asynchronous-unwind-tables -Wl,--gc-section -ffunction-sections -fdata-sections'
CFLAGS_MEM=-g3

if [ -r $CONFIG_H ]; then :; else
    echo "$CONFIG_H not found" >&2
    exit 1
fi

if grep -i cmake Makefile >/dev/null; then
    echo "Not compatible with CMake" >&2
    exit 1
fi

if [ $( uname ) != Linux ]; then
    echo "Only work on Linux" >&2
    exit 1
fi

if git status | grep -F $CONFIG_H >/dev/null 2>&1; then
    echo "config.h not clean" >&2
    exit 1
fi

# make measurements with one configuration
# usage: do_config <name> <unset-list> <server-args>
do_config()
{
    NAME=$1
    UNSET_LIST=$2
    SERVER_ARGS=$3

    echo ""
    echo "config-$NAME:"
    cp configs/config-$NAME.h $CONFIG_H
    scripts/config.pl unset MBEDTLS_SSL_SRV_C

    for FLAG in $UNSET_LIST; do
        scripts/config.pl unset $FLAG
    done

    grep -F SSL_MAX_CONTENT_LEN $CONFIG_H || echo 'SSL_MAX_CONTENT_LEN=16384'

    printf "    Executable size... "

    make clean
    CFLAGS=$CFLAGS_EXEC make OFLAGS=-Os lib >/dev/null 2>&1
    cd programs
    CFLAGS=$CFLAGS_EXEC make OFLAGS=-Os ssl/$CLIENT >/dev/null
    strip ssl/$CLIENT
    stat -c '%s' ssl/$CLIENT
    cd ..

    printf "    Peak ram usage... "

    make clean
    CFLAGS=$CFLAGS_MEM make OFLAGS=-Os lib >/dev/null 2>&1
    cd programs
    CFLAGS=$CFLAGS_MEM make OFLAGS=-Os ssl/$CLIENT >/dev/null
    cd ..

    ./ssl_server2 $SERVER_ARGS >/dev/null &
    SRV_PID=$!
    sleep 1;

    if valgrind --tool=massif --stacks=yes programs/ssl/$CLIENT >/dev/null 2>&1
    then
        FAILED=0
    else
        echo "client failed" >&2
        FAILED=1
    fi

    kill $SRV_PID
    wait $SRV_PID

    scripts/massif_max.pl massif.out.*
    mv massif.out.* massif-$NAME.$$
}

# preparation

CONFIG_BAK=${CONFIG_H}.bak
cp $CONFIG_H $CONFIG_BAK

rm -f massif.out.*

printf "building server... "

make clean
make lib >/dev/null 2>&1
(cd programs && make ssl/ssl_server2) >/dev/null
cp programs/ssl/ssl_server2 .

echo "done"

# actual measurements

do_config   "ccm-psk-tls1_2" \
            "" \
            "psk=000102030405060708090A0B0C0D0E0F"

do_config   "suite-b" \
            "MBEDTLS_BASE64_C MBEDTLS_PEM_PARSE_C MBEDTLS_CERTS_C" \
            ""

# cleanup

mv $CONFIG_BAK $CONFIG_H
make clean
rm ssl_server2

exit $FAILED
