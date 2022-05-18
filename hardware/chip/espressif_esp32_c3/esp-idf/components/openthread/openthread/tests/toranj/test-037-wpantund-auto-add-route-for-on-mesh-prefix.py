#!/usr/bin/env python
#
#  Copyright (c) 2019, The OpenThread Authors.
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

import wpan
from wpan import verify

# -----------------------------------------------------------------------------------------------------------------------
# Test description:
#
# This test covers behavior of wpantund feature for managing of host interface routes (related to on-mesh prefixes
# within the Thread network).
#
# When enabled, wpantund would add a route on host primary interface for any prefix from thread network (with on-mesh
# flag set). This in turn ensures that traffic destined to an IPv6 address matching the prefix would be correctly
# forwarded to the `wpan` interface on host.

test_name = __file__[:-3] if __file__.endswith('.py') else __file__
print('-' * 120)
print('Starting \'{}\''.format(test_name))

# -----------------------------------------------------------------------------------------------------------------------
# Utility functions


def verify_interface_routes(node, route_list):
    """
    This function verifies that node has the same interface routes as given by `route_list` which is an array of
    tuples of (route, prefix_len, metric).
    """
    node_routes = wpan.parse_interface_routes_result(node.get(wpan.WPAN_IP6_INTERFACE_ROUTES))

    verify(len(route_list) == len(node_routes))

    for route in route_list:
        for node_route in node_routes:
            if (node_route.route_prefix, node_route.prefix_len, node_route.metric) == route:
                break
        else:
            raise wpan.VerifyError('Did not find route {} on node {}'.format(route, node))


# -----------------------------------------------------------------------------------------------------------------------
# Creating `wpan.Nodes` instances

speedup = 4
wpan.Node.set_time_speedup_factor(speedup)

r1 = wpan.Node()
r2 = wpan.Node()
c2 = wpan.Node()

# -----------------------------------------------------------------------------------------------------------------------
# Init all nodes

wpan.Node.init_all_nodes()

# -----------------------------------------------------------------------------------------------------------------------
# Build network topology
#

r1.form("prefix-route")

r1.allowlist_node(r2)
r2.allowlist_node(r1)
r2.join_node(r1, wpan.JOIN_TYPE_ROUTER)

c2.allowlist_node(r2)
r2.allowlist_node(c2)
c2.join_node(r2, wpan.JOIN_TYPE_END_DEVICE)

# -----------------------------------------------------------------------------------------------------------------------
# Test implementation

PREFIX1 = 'fd00:abba::'
ADDRESS1 = PREFIX1 + '1'
LEN1 = 64

PREFIX2 = 'fd00:cafe:feed::'
LEN2 = 64

PREFIX3 = 'fd00:1234::'
LEN3 = 64

ROUTE4 = 'fd00:1234::'
LEN4 = 48

ROUTE5 = 'fd00:beef::'
LEN5 = 64

MEDIUM_METRIC = 256
MEDIUM_PRIORITY = 0

WAIT_TIME = 10

# Verify the default daemon configuration
verify(r1.get(wpan.WPAN_DAEMON_ON_MESH_PREFIX_AUTO_ADD_AS_INTERFACE_ROUTE) == 'true')

r1.set(wpan.WPAN_DAEMON_OFF_MESH_ROUTE_AUTO_ADD_ON_INTERFACE, 'false')
verify(r1.get(wpan.WPAN_DAEMON_OFF_MESH_ROUTE_AUTO_ADD_ON_INTERFACE) == 'false')

# Add on-mesh prefixes 1, 2, and 3 and off-mesh route 5 on r2.
r2.add_prefix(PREFIX1, prefix_len=LEN1, on_mesh=True, slaac=False)
r2.add_prefix(PREFIX2, prefix_len=LEN2, on_mesh=True, slaac=True)
r2.add_prefix(PREFIX3, prefix_len=LEN2, on_mesh=False, slaac=False)
r2.add_route(ROUTE5, prefix_len=LEN5, priority=MEDIUM_PRIORITY)


# We expect to only see routes associated the first two (which are on-mesh) on r1.
def check_routes_on_r1_is_prefix1_and_prefix2():
    verify_interface_routes(r1, [(PREFIX1, LEN1, MEDIUM_METRIC), (PREFIX2, LEN2, MEDIUM_METRIC)])


wpan.verify_within(check_routes_on_r1_is_prefix1_and_prefix2, WAIT_TIME)

# Remove all prefixes
r2.remove_prefix(PREFIX1, prefix_len=LEN1)
r2.remove_prefix(PREFIX2, prefix_len=LEN2)
r2.remove_prefix(PREFIX3, prefix_len=LEN2)
r2.remove_route(ROUTE5, prefix_len=LEN5)


# We expect all associated routes to be removed on r1
def check_routes_on_r1_is_empty():
    verify_interface_routes(r1, [])


wpan.verify_within(check_routes_on_r1_is_empty, WAIT_TIME)

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Test behavior when an address with same prefix in already added on the interface

r1.add_ip6_address_on_interface(ADDRESS1, prefix_len=LEN1)
r2.add_prefix(PREFIX1, prefix_len=LEN1, on_mesh=True, slaac=False)


def check_routes_on_r1_is_only_prefix1():
    verify_interface_routes(r1, [(PREFIX1, LEN1, MEDIUM_METRIC)])


wpan.verify_within(check_routes_on_r1_is_only_prefix1, WAIT_TIME)

r1.remove_ip6_address_on_interface(ADDRESS1, prefix_len=LEN1)
wpan.verify_within(check_routes_on_r1_is_only_prefix1, WAIT_TIME)

r2.remove_prefix(PREFIX1, prefix_len=LEN1)

wpan.verify_within(check_routes_on_r1_is_empty, WAIT_TIME)

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Test behavior when we have similar route and prefix (different prefix_len)

r1.set(wpan.WPAN_DAEMON_OFF_MESH_ROUTE_AUTO_ADD_ON_INTERFACE, 'true')
verify(r1.get(wpan.WPAN_DAEMON_OFF_MESH_ROUTE_AUTO_ADD_ON_INTERFACE) == 'true')

r2.add_route(ROUTE5, prefix_len=LEN5, priority=MEDIUM_PRIORITY)
r2.add_route(ROUTE4, prefix_len=LEN4, priority=MEDIUM_PRIORITY)
r1.add_prefix(PREFIX3, prefix_len=LEN3, on_mesh=True, slaac=False)


def check_routes_on_r1_is_prefix3_route4_and_route5():
    route_list = [(PREFIX3, LEN3, MEDIUM_METRIC), (ROUTE4, LEN4, MEDIUM_METRIC), (ROUTE5, LEN5, MEDIUM_METRIC)]
    verify_interface_routes(r1, route_list)


wpan.verify_within(check_routes_on_r1_is_prefix3_route4_and_route5, WAIT_TIME)

r2.remove_route(ROUTE5, prefix_len=LEN5)
r1.remove_prefix(PREFIX3, prefix_len=LEN3)


def check_routes_on_r1_is_only_route4():
    verify_interface_routes(r1, [(ROUTE4, LEN4, MEDIUM_METRIC)])


wpan.verify_within(check_routes_on_r1_is_only_route4, WAIT_TIME)

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Test behavior when feature is disabled

r1.set(wpan.WPAN_DAEMON_ON_MESH_PREFIX_AUTO_ADD_AS_INTERFACE_ROUTE, 'false')
verify(r1.get(wpan.WPAN_DAEMON_ON_MESH_PREFIX_AUTO_ADD_AS_INTERFACE_ROUTE) == 'false')

r1.add_prefix(PREFIX3, prefix_len=LEN3, on_mesh=True, slaac=False)
wpan.verify_within(check_routes_on_r1_is_only_route4, WAIT_TIME)

# -----------------------------------------------------------------------------------------------------------------------
# Test finished

wpan.Node.finalize_all_nodes()

print('\'{}\' passed.'.format(test_name))
