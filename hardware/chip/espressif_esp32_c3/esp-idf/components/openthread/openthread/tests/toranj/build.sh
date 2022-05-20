#!/bin/bash
#
#  Copyright (c) 2018, The OpenThread Authors.
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

display_usage()
{
    echo ""
    echo "Toranj Build script "
    echo ""
    echo "Usage: $(basename "$0") [options] <config>"
    echo "    <config> can be:"
    echo "        ncp             : Build OpenThread NCP FTD mode with simulation platform"
    echo "        ncp-15.4        : Build OpenThread NCP FTD mode with simulation platform - 15.4 radio"
    echo "        ncp-trel        : Build OpenThread NCP FTD mode with simulation platform - TREL radio "
    echo "        ncp-15.4+trel   : Build OpenThread NCP FTD mode with simulation platform - multi radio (15.4+TREL)"
    echo "        rcp             : Build OpenThread RCP (NCP in radio mode) with simulation platform"
    echo "        posix           : Build OpenThread POSIX NCP"
    echo "        posix-15.4      : Build OpenThread POSIX NCP - 15.4 radio"
    echo "        posix-trel      : Build OpenThread POSIX NCP - TREL radio "
    echo "        posix-15.4+trel : Build OpenThread POSIX NCP - multi radio (15.4+TREL)"
    echo "        cmake           : Configure and build OpenThread using cmake/ninja (RCP and NCP) only"
    echo "        cmake-posix:    : Configure and build OpenThread POSIX using cmake/ninja"
    echo ""
    echo "Options:"
    echo "        -c/--enable-coverage  Enable code coverage"
    echo "        -t/--enable-tests     Enable tests"
    echo ""
}

die()
{
    echo " *** ERROR: " "$*"
    exit 1
}

cd "$(dirname "$0")" || die "cd failed"
cd ../.. || die "cd failed"

coverage=no
tests=no

while [ $# -ge 2 ]; do
    case $1 in
        -c | --enable-coverage)
            coverage=yes
            shift
            ;;
        -t | --enable-tests)
            tests=yes
            shift
            ;;
        "")
            shift
            ;;
        *)
            echo "Error: Unknown option \"$1\""
            display_usage
            exit 1
            ;;
    esac
done

if [ "$#" -ne 1 ]; then
    display_usage
    exit 1
fi

build_config=$1

configure_options=(
    "--disable-docs"
    "--enable-tests=$tests"
    "--enable-coverage=$coverage"
    "--enable-ftd"
    "--enable-ncp"
)

posix_configure_options=(
    "--disable-docs"
    "--enable-tests=$tests"
    "--enable-coverage=$coverage"
    "--enable-ftd"
    "--enable-cli"
)

if [ -n "${top_builddir}" ]; then
    top_srcdir=$(pwd)
    mkdir -p "${top_builddir}"
else
    top_srcdir=.
    top_builddir=.
fi

case ${build_config} in
    ncp | ncp-)
        echo "==================================================================================================="
        echo "Building OpenThread NCP FTD mode with simulation platform (radios determined by config)"
        echo "==================================================================================================="
        ./bootstrap || die "bootstrap failed"
        cd "${top_builddir}" || die "cd failed"
        cppflags_config='-DOPENTHREAD_PROJECT_CORE_CONFIG_FILE=\"../tests/toranj/openthread-core-toranj-config-simulation.h\"'
        ${top_srcdir}/configure \
            CPPFLAGS="$cppflags_config" \
            --with-examples=simulation \
            "${configure_options[@]}" || die
        make -j 8 || die
        ;;

    ncp-15.4)
        echo "==================================================================================================="
        echo "Building OpenThread NCP FTD mode with simulation platform - 15.4 radio"
        echo "==================================================================================================="
        cppflags_config='-DOPENTHREAD_PROJECT_CORE_CONFIG_FILE=\"../tests/toranj/openthread-core-toranj-config-simulation.h\"'
        cppflags_config="${cppflags_config} -DOPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE=1"
        cppflags_config="${cppflags_config} -DOPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE=0"
        ./bootstrap || die "bootstrap failed"
        cd "${top_builddir}" || die "cd failed"
        ${top_srcdir}/configure \
            CPPFLAGS="$cppflags_config" \
            --with-examples=simulation \
            "${configure_options[@]}" || die
        make -j 8 || die
        cp -p ${top_builddir}/examples/apps/ncp/ot-ncp-ftd ${top_builddir}/examples/apps/ncp/ot-ncp-ftd-15.4
        ;;

    ncp-trel)
        echo "==================================================================================================="
        echo "Building OpenThread NCP FTD mode with simulation platform - TREL radio"
        echo "==================================================================================================="
        cppflags_config='-DOPENTHREAD_PROJECT_CORE_CONFIG_FILE=\"../tests/toranj/openthread-core-toranj-config-simulation.h\"'
        cppflags_config="${cppflags_config} -DOPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE=0"
        cppflags_config="${cppflags_config} -DOPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE=1"
        ./bootstrap || die "bootstrap failed"
        cd "${top_builddir}" || die "cd failed"
        ${top_srcdir}/configure \
            CPPFLAGS="$cppflags_config" \
            --with-examples=simulation \
            "${configure_options[@]}" || die
        make -j 8 || die
        cp -p ${top_builddir}/examples/apps/ncp/ot-ncp-ftd ${top_builddir}/examples/apps/ncp/ot-ncp-ftd-trel
        ;;

    ncp-15.4+trel | ncp-trel+15.4)
        echo "==================================================================================================="
        echo "Building OpenThread NCP FTD mode with simulation platform - multi radio (15.4 + TREL)"
        echo "==================================================================================================="
        cppflags_config='-DOPENTHREAD_PROJECT_CORE_CONFIG_FILE=\"../tests/toranj/openthread-core-toranj-config-simulation.h\"'
        cppflags_config="${cppflags_config} -DOPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE=1"
        cppflags_config="${cppflags_config} -DOPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE=1"
        ./bootstrap || die "bootstrap failed"
        cd "${top_builddir}" || die "cd failed"
        ${top_srcdir}/configure \
            CPPFLAGS="$cppflags_config" \
            --with-examples=simulation \
            "${configure_options[@]}" || die
        make -j 8 || die
        cp -p ${top_builddir}/examples/apps/ncp/ot-ncp-ftd ${top_builddir}/examples/apps/ncp/ot-ncp-ftd-15.4-trel
        ;;

    rcp)
        echo "===================================================================================================="
        echo "Building OpenThread RCP (NCP in radio mode) with simulation platform"
        echo "===================================================================================================="
        ./bootstrap || die "bootstrap failed"
        cd "${top_builddir}" || die "cd failed"
        cppflags_config='-DOPENTHREAD_PROJECT_CORE_CONFIG_FILE=\"../tests/toranj/openthread-core-toranj-config-simulation.h\"'
        ${top_srcdir}/configure \
            CPPFLAGS="$cppflags_config " \
            --enable-coverage=${coverage} \
            --enable-ncp \
            --enable-radio-only \
            --with-examples=simulation \
            --disable-docs \
            --enable-tests=$tests || die
        make -j 8 || die
        ;;

    posix | posix-)
        echo "===================================================================================================="
        echo "Building OpenThread POSIX (radios determined by config)"
        echo "===================================================================================================="
        cppflags_config='-DOPENTHREAD_PROJECT_CORE_CONFIG_FILE=\"../tests/toranj/openthread-core-toranj-config-posix.h\"'
        ./bootstrap || die "bootstrap failed"
        cd "${top_builddir}" || die "cd failed"
        ${top_srcdir}/configure \
            CPPFLAGS="$cppflags_config" \
            --with-platform=posix \
            "${posix_configure_options[@]}" || die
        make -j 8 || die
        ;;

    posix-15.4)
        echo "===================================================================================================="
        echo "Building OpenThread POSIX - 15.4 radio"
        echo "===================================================================================================="
        cppflags_config='-DOPENTHREAD_PROJECT_CORE_CONFIG_FILE=\"../tests/toranj/openthread-core-toranj-config-posix.h\"'
        cppflags_config="${cppflags_config} -DOPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE=1"
        cppflags_config="${cppflags_config} -DOPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE=0"
        ./bootstrap || die "bootstrap failed"
        cd "${top_builddir}" || die "cd failed"
        ${top_srcdir}/configure \
            CPPFLAGS="$cppflags_config" \
            --with-platform=posix \
            "${posix_configure_options[@]}" || die
        make -j 8 || die
        ;;

    posix-trel)
        echo "===================================================================================================="
        echo "Building OpenThread POSIX - TREL radio"
        echo "===================================================================================================="
        cppflags_config='-DOPENTHREAD_PROJECT_CORE_CONFIG_FILE=\"../tests/toranj/openthread-core-toranj-config-posix.h\"'
        cppflags_config="${cppflags_config} -DOPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE=0"
        cppflags_config="${cppflags_config} -DOPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE=1"
        ./bootstrap || die "bootstrap failed"
        cd "${top_builddir}" || die "cd failed"
        ${top_srcdir}/configure \
            CPPFLAGS="$cppflags_config" \
            --with-platform=posix \
            "${posix_configure_options[@]}" || die
        make -j 8 || die
        ;;

    posix-trel+15.4 | posix-15.4+trel)
        echo "===================================================================================================="
        echo "Building OpenThread POSIX - multi radio link (15.4 + TREL)"
        echo "===================================================================================================="
        cppflags_config='-DOPENTHREAD_PROJECT_CORE_CONFIG_FILE=\"../tests/toranj/openthread-core-toranj-config-posix.h\"'
        cppflags_config="${cppflags_config} -DOPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE=1"
        cppflags_config="${cppflags_config} -DOPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE=1"
        ./bootstrap || die "bootstrap failed"
        cd "${top_builddir}" || die "cd failed"
        ${top_srcdir}/configure \
            CPPFLAGS="$cppflags_config" \
            --with-platform=posix \
            "${posix_configure_options[@]}" || die
        make -j 8 || die
        ;;

    cmake)
        echo "===================================================================================================="
        echo "Building OpenThread (NCP/CLI for FTD/MTD/RCP mode) with simulation platform using cmake"
        echo "===================================================================================================="
        cd "${top_builddir}" || die "cd failed"
        cmake -GNinja -DOT_PLATFORM=simulation -DOT_COMPILE_WARNING_AS_ERROR=on -DOT_APP_CLI=on \
            -DOT_CONFIG=../tests/toranj/openthread-core-toranj-config-simulation.h \
            "${top_srcdir}" || die
        ninja || die
        ;;

    cmake-posix-host | cmake-posix | cmake-p)
        echo "===================================================================================================="
        echo "Building OpenThread POSIX using cmake"
        echo "===================================================================================================="
        cd "${top_builddir}" || die "cd failed"
        cmake -GNinja -DOT_PLATFORM=posix -DOT_COMPILE_WARNING_AS_ERROR=on -DOT_APP_CLI=off \
            -DOT_CONFIG=../tests/toranj/openthread-core-toranj-config-posix.h \
            "${top_srcdir}" || die
        ninja || die
        ;;

    *)
        echo "Error: Unknown configuration \"$1\""
        display_usage
        exit 1
        ;;
esac

exit 0
