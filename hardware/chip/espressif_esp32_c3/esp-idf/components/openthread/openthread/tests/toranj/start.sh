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

die()
{
    echo " *** ERROR: " "$*"
    exit 1
}

cleanup()
{
    # Clear logs and flash files
    sudo rm tmp/*.flash tmp/*.data tmp/*.swap >/dev/null 2>&1
    sudo rm ./*.log >/dev/null 2>&1

    # Clear any wpantund instances
    sudo killall wpantund >/dev/null 2>&1

    while read -r interface; do
        sudo ip link delete "$interface" >/dev/null 2>&1
    done < <(ifconfig 2>/dev/null | grep -o "wpan[0-9]*")

    sudo ip address flush trel

    sleep 0.3
}

prepare_trel_link()
{
    # Prepares a netif "trel" as virtual eth to use for
    # testing "TREL" radio link in POSIX platform.

    echo "Preparing trel netif"
    sudo ip link delete trel
    sudo ip link add trel type veth peer name trel-peer || die
    sudo ip link set trel multicast on || die
    sudo ip link set trel up || die
    sudo ip link set trel-peer multicast on || die
    sudo ip link set trel-peer up || die
}

run()
{
    counter=0
    while true; do

        if sudo -E python "$1"; then
            cleanup
            return
        fi

        # We allow a failed test to be retried up to 7 attempts.
        if [ "$counter" -lt 7 ]; then
            counter=$((counter + 1))
            echo Attempt $counter running "$1" failed. Trying again.
            cleanup
            sleep 10
            continue
        fi

        echo " *** TEST FAILED"
        tail -n 40 wpantund-logs*.log
        exit 1
    done
}

cd "$(dirname "$0")" || die "cd failed"

if [ -z "${top_builddir}" ]; then
    top_builddir=.
fi

if [ "$COVERAGE" = 1 ]; then
    coverage_option="--enable-coverage"
else
    coverage_option=""
fi

if [ "$TORANJ_RADIO" = "multi" ]; then
    # Build all combinations
    ./build.sh "${coverage_option}" ncp-15.4 || die "ncp-15.4 build failed"
    (cd ${top_builddir} && make clean) || die "cd and clean failed"
    ./build.sh "${coverage_option}" ncp-trel || die "ncp-trel build failed"
    (cd ${top_builddir} && make clean) || die "cd and clean failed"
    ./build.sh "${coverage_option}" ncp-15.4+trel || die "ncp-15.4+trel build failed"
    (cd ${top_builddir} && make clean) || die "cd and clean failed"
else
    ./build.sh "${coverage_option}" ncp-"${TORANJ_RADIO}" || die "ncp build failed"
fi

cleanup

if [ "$TORANJ_RADIO" = "multi" ]; then
    run test-700-multi-radio-join.py
    run test-701-multi-radio-probe.py
    run test-702-multi-radio-discovery-by-rx.py
    run test-703-multi-radio-mesh-header-msg.py
    run test-704-multi-radio-scan.py
    run test-705-multi-radio-discover-scan.py

    exit 0
fi

run test-001-get-set.py
run test-002-form.py
run test-003-join.py
run test-004-scan.py
run test-005-discover-scan.py
run test-006-traffic-router-end-device.py
run test-007-traffic-router-sleepy.py
run test-008-permit-join.py
run test-009-insecure-traffic-join.py
run test-010-on-mesh-prefix-config-gateway.py
run test-011-child-table.py
run test-012-multi-hop-traffic.py
run test-013-off-mesh-route-traffic.py
run test-014-ip6-address-add.py
run test-015-same-prefix-on-multiple-nodes.py
run test-016-neighbor-table.py
run test-017-parent-reset-child-recovery.py
run test-018-child-supervision.py
run test-019-inform-previous-parent.py
run test-020-router-table.py
run test-021-address-cache-table.py
run test-022-multicast-ip6-address.py
run test-023-multicast-traffic.py
run test-024-partition-merge.py
run test-025-network-data-timeout.py
run test-026-slaac-address-wpantund.py
run test-027-child-mode-change.py
run test-028-router-leader-reset-recovery.py
run test-029-data-poll-interval.py
run test-030-slaac-address-ncp.py
run test-031-meshcop-joiner-commissioner.py
run test-032-child-attach-with-multiple-ip-addresses.py
run test-033-mesh-local-prefix-change.py
run test-034-poor-link-parent-child-attach.py
run test-035-child-timeout-large-data-poll.py
run test-036-wpantund-host-route-management.py
run test-037-wpantund-auto-add-route-for-on-mesh-prefix.py
run test-038-clear-address-cache-for-sed.py
run test-039-address-cache-table-snoop.py
run test-040-network-data-stable-full.py
run test-041-lowpan-fragmentation.py
run test-042-meshcop-joiner-discerner.py
run test-043-meshcop-joiner-router.py
run test-100-mcu-power-state.py
run test-600-channel-manager-properties.py
run test-601-channel-manager-channel-change.py

# Skip the "channel-select" test on a TREL only radio link, since it
# requires energy scan which is not supported in this case.

if [ "$TORANJ_RADIO" != "trel" ]; then
    run test-602-channel-manager-channel-select.py
fi

run test-603-channel-manager-announce-recovery.py

exit 0
