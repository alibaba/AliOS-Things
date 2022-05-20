#!/usr/bin/env python3
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

import time
import wpan
from wpan import verify

# -----------------------------------------------------------------------------------------------------------------------
# Test description: Adding off-mesh routes (on routers and FEDs) and traffic flow to off-mesh addresses.
#
# Test topology:
#
#     r1 ---- r2
#     |       |
#     |       |
#    fed1    sed2
#
# The off-mesh-routes are added as follows:
# - `r1`   adds `OFF_MESH_ROUTE_1`,
# - `r2`   adds `OFF_MESH_ROUTE_2`,
# - `fed1` adds `OFF_MESH_ROUTE_3`.
#
# Traffic flow:
# - From `sed2` to an address matching `OFF_MESH_ROUTE_1` (verify it is received on `r1`),
# - From `r1`   to an address matching `OFF_MESH_ROUTE_2` (verify it is received on `r2`),
# - From `r2`   to an address matching `OFF_MESH_ROUTE_3` (verify it is received on `fed1`)
#

test_name = __file__[:-3] if __file__.endswith('.py') else __file__
print('-' * 120)
print('Starting \'{}\''.format(test_name))

# -----------------------------------------------------------------------------------------------------------------------
# Creating `wpan.Nodes` instances

speedup = 4
wpan.Node.set_time_speedup_factor(speedup)

r1 = wpan.Node()
fed1 = wpan.Node()
r2 = wpan.Node()
sed2 = wpan.Node()

all_nodes = [r1, fed1, r2, sed2]

# -----------------------------------------------------------------------------------------------------------------------
# Init all nodes

wpan.Node.init_all_nodes()
for node in all_nodes:
    # Disable `AutoUpdateInterfaceAddrsOnNCP` feature on wpantund
    # for all nodes. This ensures that added IPv6 address (on linux
    # interface) are not pushed to NCP (and therefore are not
    # on-mesh).
    node.set("Daemon:IPv6:AutoUpdateInterfaceAddrsOnNCP", '0')

# -----------------------------------------------------------------------------------------------------------------------
# Build network topology
#
#   r1 ---- r2
#   |       |
#   |       |
#  fed1    sed2

r1.allowlist_node(r2)
r2.allowlist_node(r1)

r1.allowlist_node(fed1)
fed1.allowlist_node(r1)

r2.allowlist_node(sed2)
sed2.allowlist_node(r2)

r1.form("off-mesh")
r2.join_node(r1, wpan.JOIN_TYPE_ROUTER)

fed1.join_node(r1, wpan.JOIN_TYPE_END_DEVICE)
sed2.join_node(r2, wpan.JOIN_TYPE_SLEEPY_END_DEVICE)

sed2.set(wpan.WPAN_POLL_INTERVAL, '1500')

# -----------------------------------------------------------------------------------------------------------------------
# Test implementation

WAIT_TIME = 10
NUM_ROUTES = 3
NUM_ROUTES_LOCAL = 1
ON_MESH_PREFIX = "fd00:1234::"
OFF_MESH_ROUTE_1 = "fd00:abba::"
OFF_MESH_ROUTE_2 = "fd00:cafe::"
OFF_MESH_ROUTE_3 = "fd00:baba::"
OFF_MESH_ADDR_1 = OFF_MESH_ROUTE_1 + "1"
OFF_MESH_ADDR_2 = OFF_MESH_ROUTE_2 + "2"
OFF_MESH_ADDR_3 = OFF_MESH_ROUTE_3 + "3"

# Add on-mesh prefix
r1.config_gateway(ON_MESH_PREFIX)

# The off-mesh-routes are added as follows:
# - `r1` adds OFF_MESH_ROUTE_1,
# - `r2` adds OFF_MESH_ROUTE_2,
# - `fed1` adds OFF_MESH_ROUTE_3.

r1.add_route(OFF_MESH_ROUTE_1)
r1.add_ip6_address_on_interface(OFF_MESH_ADDR_1)

r2.add_route(OFF_MESH_ROUTE_2)
r2.add_ip6_address_on_interface(OFF_MESH_ADDR_2)

fed1.add_route(OFF_MESH_ROUTE_3)
fed1.add_ip6_address_on_interface(OFF_MESH_ADDR_3)

# Wait till network data is updated on r1, r2, and sed2 and they all see all
# the added off-mesh routes.


def check_off_mesh_routes():
    # If a node itself adds a route, the route entry will be seen twice in
    # its WPAN_THREAD_OFF_MESH_ROUTES list (one time as part of network-wide
    # network data and again as part of the local network data). Note that
    # `r1 and `r2` each add a route, while `sed2` does not.
    verify(len(wpan.parse_list(r1.get(wpan.WPAN_THREAD_OFF_MESH_ROUTES))) == NUM_ROUTES + NUM_ROUTES_LOCAL)
    verify(len(wpan.parse_list(r2.get(wpan.WPAN_THREAD_OFF_MESH_ROUTES))) == NUM_ROUTES + NUM_ROUTES_LOCAL)
    verify(len(wpan.parse_list(sed2.get(wpan.WPAN_THREAD_OFF_MESH_ROUTES))) == NUM_ROUTES)


wpan.verify_within(check_off_mesh_routes, WAIT_TIME)

# Traffic from `sed2` to `OFF_MESH_ADDR_1` (verify that it is received on
# `r1`).

src = sed2.find_ip6_address_with_prefix(ON_MESH_PREFIX)
sender = sed2.prepare_tx(src, OFF_MESH_ADDR_1, "Hello Route1")
recver = r1.prepare_rx(sender)
wpan.Node.perform_async_tx_rx()
verify(sender.was_successful)
verify(recver.was_successful)

# Traffic from `r1` to `OFF_MESH_ADDR_2` (verify that it is received on `r2`),

src = r1.find_ip6_address_with_prefix(ON_MESH_PREFIX)
sender = r1.prepare_tx(src, OFF_MESH_ADDR_2, "Hello Route2")
recver = r2.prepare_rx(sender)
wpan.Node.perform_async_tx_rx()
verify(sender.was_successful)
verify(recver.was_successful)

# Traffic from `r2` to `OFF_MESH_ADDR_3` (verify that it is received on `fed1`)

src = r2.find_ip6_address_with_prefix(ON_MESH_PREFIX)
sender = r2.prepare_tx(src, OFF_MESH_ADDR_3, "Hello Route3")
recver = fed1.prepare_rx(sender)
wpan.Node.perform_async_tx_rx()
verify(sender.was_successful)
verify(recver.was_successful)

# -----------------------------------------------------------------------------------------------------------------------
# Test finished

wpan.Node.finalize_all_nodes()

print('\'{}\' passed.'.format(test_name))
