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
# This test covers behavior of wpantund feature for managing of host interface routes (related to off-mesh routes
# within the Thread network). This feature can be enabled using "Daemon:OffMeshRoute:AutoAddOnInterface" property (is
# enabled by default).
#
# A route corresponding to an off-mesh route would be added on host primary interface (by wpantund),
# if it is added by at least one (other) device within the network and
#    (a) either it is not added by host/this-device, or
#    (b) if it is also added by the device itself then
#        - filtering of self added routes is not enabled, and
#        - it is added at lower preference level.

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
r3 = wpan.Node()
c3 = wpan.Node()

# -----------------------------------------------------------------------------------------------------------------------
# Init all nodes

wpan.Node.init_all_nodes()

# -----------------------------------------------------------------------------------------------------------------------
# Build network topology
#
# Test topology:
#
#    r1 ---- r2
#     \      /
#      \    /
#       \  /
#        r3 ---- c3
#
# 3 routers, c3 is added to ensure r3 is promoted to a router quickly!

r1.form("route-test")

r1.allowlist_node(r2)
r2.allowlist_node(r1)
r2.join_node(r1, wpan.JOIN_TYPE_ROUTER)

r3.allowlist_node(r2)
r2.allowlist_node(r3)
r3.join_node(r2, wpan.JOIN_TYPE_ROUTER)

c3.allowlist_node(r3)
r3.allowlist_node(c3)
c3.join_node(r3, wpan.JOIN_TYPE_END_DEVICE)

r3.allowlist_node(r1)
r1.allowlist_node(r3)

# -----------------------------------------------------------------------------------------------------------------------
# Test implementation

ROUTE1 = 'fd00:abba::'
LEN1 = 64

ROUTE2 = 'fd00:cafe:feed::'
LEN2 = 64

ROUTE3 = 'fd00:abba::'
LEN3 = 48

ROUTE4 = 'fd00:1234::'
LEN4 = 64

# Route Priority for off-mesh routes
HIGH_PRIORITY = 1
MEDIUM_PRIORITY = 0
LOW_PRIORITY = -1

# Host route metric mapping to off-mesh route (note lower metric value is higher priority)
HIGH_METRIC = 1
MEDIUM_METRIC = 256
LOW_METRIC = 512

WAIT_TIME = 10

# Verify the default daemon configuration for managing host/off-mesh routes
verify(r1.get(wpan.WPAN_DAEMON_OFF_MESH_ROUTE_AUTO_ADD_ON_INTERFACE) == 'true')
verify(r1.get(wpan.WPAN_DAEMON_OFF_MESH_ROUTE_FILTER_SELF_AUTO_ADDED) == 'true')

# Disable the auto route add on r2.
r2.set(wpan.WPAN_DAEMON_OFF_MESH_ROUTE_AUTO_ADD_ON_INTERFACE, 'false')

# Verify the host interface routes are empty when we start.
verify_interface_routes(r1, [])

# Add all 3 routes on r2.
r2.add_route(ROUTE1, prefix_len=LEN1, priority=LOW_PRIORITY)
r2.add_route(ROUTE2, prefix_len=LEN2, priority=MEDIUM_PRIORITY)
r2.add_route(ROUTE3, prefix_len=LEN3, priority=HIGH_PRIORITY)


# We expect to see all 3 routes added on r1 host interface with same priority levels as r2.
def check_routes_on_r1_1():
    verify_interface_routes(r1, [(ROUTE1, LEN1, LOW_METRIC), (ROUTE2, LEN2, MEDIUM_METRIC),
                                 (ROUTE3, LEN3, HIGH_METRIC)])


wpan.verify_within(check_routes_on_r1_1, WAIT_TIME)

# - - - - - - - - - - - - - - - - - - - - - - - - - - - -

# Add the same routes on r3 with different priorities.
r3.add_route(ROUTE1, prefix_len=LEN1, priority=MEDIUM_PRIORITY)
r3.add_route(ROUTE2, prefix_len=LEN2, priority=LOW_PRIORITY)


# We expect the host interface routes on r1 to change accordingly
def check_routes_on_r1_2():
    route_list = [(ROUTE1, LEN1, MEDIUM_METRIC), (ROUTE2, LEN2, MEDIUM_METRIC), (ROUTE3, LEN3, HIGH_METRIC)]
    verify_interface_routes(r1, route_list)


wpan.verify_within(check_routes_on_r1_2, WAIT_TIME)
verify_interface_routes(r2, [])

# - - - - - - - - - - - - - - - - - - - - - - - - - - - -

# Remove the previously added routes from r2.
r2.remove_route(ROUTE1, prefix_len=LEN1)
r2.remove_route(ROUTE2, prefix_len=LEN2)
r2.remove_route(ROUTE3, prefix_len=LEN3)


# We expect the host interface routes on r1 to again change accordingly:
def check_routes_on_r1_3():
    verify_interface_routes(r1, [(ROUTE1, LEN1, MEDIUM_METRIC), (ROUTE2, LEN2, LOW_METRIC)])


wpan.verify_within(check_routes_on_r1_3, WAIT_TIME)
verify_interface_routes(r2, [])

# - - - - - - - - - - - - - - - - - - - - - - - - - - - -

# Disable "Daemon:OffMeshRoute:FilterSelfAutoAdded" feature on wpantund.
#
# The route should be added on host primary interface, if it
# is added by at least one other device within the network and,
#  (a) either it is not added by host/this-device, or
#  (b) if it is also added by device then
#      - filtering of self added routes is not enabled, and
#      - it is added at lower preference level.

r1.set(wpan.WPAN_DAEMON_OFF_MESH_ROUTE_FILTER_SELF_AUTO_ADDED, 'false')
verify(r1.get(wpan.WPAN_DAEMON_OFF_MESH_ROUTE_FILTER_SELF_AUTO_ADDED) == 'false')

# Add ROUTE1 on r1 with low-priority. Since it's also present on r3 with
# medium priority, we should still see the route on host (as medium).

r1.add_route(ROUTE1, prefix_len=LEN1, priority=LOW_PRIORITY)

verify_interface_routes(r1, [(ROUTE1, LEN1, MEDIUM_METRIC), (ROUTE2, LEN2, LOW_METRIC)])

# Now change ROUTE1 on r1 to be same priority as on r2, now the route should
# no longer be present on host interface routes.

r1.remove_route(ROUTE1, prefix_len=LEN1)
r1.add_route(ROUTE1, prefix_len=LEN1, priority=MEDIUM_PRIORITY)

verify_interface_routes(r1, [(ROUTE2, LEN2, LOW_METRIC)])

# Adding ROUTE2 with higher priority should remove it from interface routes
r1.add_route(ROUTE2, prefix_len=LEN2, priority=MEDIUM_PRIORITY)

verify_interface_routes(r1, [])

# Adding a new ROUTE4 on r1 should not change anything related to interface host routes.
r1.add_route(ROUTE4, prefix_len=LEN4, priority=MEDIUM_METRIC)
verify_interface_routes(r1, [])

# Removing ROUTE1 and ROUT2 on r1 should cause them to be added back on host
# interface (since they are still present as off-mesh routes on r3).
r1.remove_route(ROUTE1, prefix_len=LEN1)
r1.remove_route(ROUTE2, prefix_len=LEN2)

verify_interface_routes(r1, [(ROUTE1, LEN1, MEDIUM_METRIC), (ROUTE2, LEN2, LOW_METRIC)])

verify_interface_routes(r2, [])

# - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Enable "Daemon:OffMeshRoute:FilterSelfAutoAdded" feature on wpantund.

r1.set(wpan.WPAN_DAEMON_OFF_MESH_ROUTE_FILTER_SELF_AUTO_ADDED, 'true')
verify(r1.get(wpan.WPAN_DAEMON_OFF_MESH_ROUTE_FILTER_SELF_AUTO_ADDED) == 'true')

# Adding ROUTE1 with any priority should remove it from host interface routes.
r1.add_route(ROUTE1, prefix_len=LEN1, priority=LOW_PRIORITY)

verify_interface_routes(r1, [(ROUTE2, LEN2, LOW_METRIC)])

r1.remove_route(ROUTE1, prefix_len=LEN1)

verify_interface_routes(r1, [(ROUTE1, LEN1, MEDIUM_METRIC), (ROUTE2, LEN2, LOW_METRIC)])

verify_interface_routes(r2, [])

# -----------------------------------------------------------------------------------------------------------------------
# Test finished

wpan.Node.finalize_all_nodes()

print('\'{}\' passed.'.format(test_name))
