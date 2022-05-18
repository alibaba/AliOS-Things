#!/usr/bin/env python3
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

import time
import wpan
from wpan import verify

# -----------------------------------------------------------------------------------------------------------------------
# Test description: SLAAC address
#
# This test covers the behavior of SLAAC module in OpenThread (NCP):
#
# - Verify that adding prefix (with SLAAC flag) causes a corresponding SLAAC IPv6 address to be added.
# - Verify that removing the prefix, would remove the SLAAC address.
# - Verify behavior when same prefix is added/removed on multiple nodes (with or without SLAAC flag).
# - Check behavior when a user-added address with the same prefix already exists.
# - Check behavior when a user-added address with same prefix is removed (SLAAC module should add a SLAAC address).
# - Ensure removal of prefix does not remove user-added address with same prefix.
# - Ensure disabling SLAAC module removes previously added SLAAC addresses, and re-enabling it adds them back.
# - Check behavior when prefix is added while SLAAC module is disabled and then enabled later.

test_name = __file__[:-3] if __file__.endswith('.py') else __file__
print('-' * 120)
print('Starting \'{}\''.format(test_name))

# -----------------------------------------------------------------------------------------------------------------------
# Utility functions


def verify_address(node_list, prefix):
    """
    This function verifies that all nodes in the `node_list` contain an IPv6 address with the given `prefix`.
    """
    for node in node_list:
        all_addrs = wpan.parse_list(node.get(wpan.WPAN_IP6_ALL_ADDRESSES))
        verify(any([addr.startswith(prefix[:-1]) for addr in all_addrs]))


def verify_no_address(node_list, prefix):
    """
    This function verifies that none of nodes in the `node_list` contain an IPv6 address with the given `prefix`.
    """
    for node in node_list:
        all_addrs = wpan.parse_list(node.get(wpan.WPAN_IP6_ALL_ADDRESSES))
        verify(all([not addr.startswith(prefix[:-1]) for addr in all_addrs]))


def verify_prefix(
    node_list,
    prefix,
    prefix_len=64,
    stable=True,
    priority='med',
    on_mesh=False,
    slaac=False,
    dhcp=False,
    configure=False,
    default_route=False,
    preferred=False,
):
    """
    This function verifies that the `prefix` is present on all nodes in the `node_list`.
    """
    for node in node_list:
        prefixes = wpan.parse_on_mesh_prefix_result(node.get(wpan.WPAN_THREAD_ON_MESH_PREFIXES))
        for p in prefixes:
            if p.prefix == prefix:
                if (int(p.prefix_len) == prefix_len and p.is_stable() == stable and p.is_on_mesh() == on_mesh and
                        p.is_def_route() == default_route and p.is_slaac() == slaac and p.is_dhcp() == dhcp and
                        p.is_config() == configure and p.is_preferred() == preferred and p.priority == priority):
                    break
        else:
            raise wpan.VerifyError("Did not find prefix {} on node {}".format(prefix, node))


def verify_no_prefix(node_list, prefix):
    """
    This function verifies that the `prefix` is NOT present on any node in the `node_list`.
    """
    for node in node_list:
        prefixes = wpan.parse_on_mesh_prefix_result(node.get(wpan.WPAN_THREAD_ON_MESH_PREFIXES))
        for p in prefixes:
            verify(not p.prefix == prefix)


# -----------------------------------------------------------------------------------------------------------------------
# Creating `wpan.Nodes` instances

speedup = 4
wpan.Node.set_time_speedup_factor(speedup)

r1 = wpan.Node()
r2 = wpan.Node()
c2 = wpan.Node()

all_nodes = [r1, r2, c2]

# -----------------------------------------------------------------------------------------------------------------------
# Init all nodes

wpan.Node.init_all_nodes()

# -----------------------------------------------------------------------------------------------------------------------
# Build network topology

r1.form('slaac-ncp')

r1.allowlist_node(r2)
r2.allowlist_node(r1)

r2.join_node(r1, node_type=wpan.JOIN_TYPE_ROUTER)

c2.allowlist_node(r2)
r2.allowlist_node(c2)

c2.join_node(r2, node_type=wpan.JOIN_TYPE_END_DEVICE)

# -----------------------------------------------------------------------------------------------------------------------
# Test implementation

# This test covers the SLAAC address management by NCP. So before starting the test we ensure that SLAAC module
# on NCP is enabled on all nodes, and disable it on wpantund.

for node in all_nodes:
    verify(node.get(wpan.WPAN_OT_SLAAC_ENABLED) == 'true')
    node.set("Daemon:IPv6:AutoAddSLAACAddress", 'false')
    verify(node.get("Daemon:IPv6:AutoAddSLAACAddress") == 'false')

WAIT_INTERVAL = 5

PREFIX = 'fd00:1234::'

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Add prefix and check all nodes get the prefix and add a corresponding
# SLAAC address.

r1.add_prefix(PREFIX, stable=True, on_mesh=True, slaac=True)


def check_prefix_and_slaac_address_are_added():
    verify_prefix(all_nodes, PREFIX, stable=True, on_mesh=True, slaac=True)
    verify_address(all_nodes, PREFIX)


wpan.verify_within(check_prefix_and_slaac_address_are_added, WAIT_INTERVAL)

# Save the assigned SLAAC addresses.
slaac_addrs = [node.find_ip6_address_with_prefix(PREFIX) for node in all_nodes]

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Check recovery after reseting r1 and c1 (same SLAAC address to be added)

r1.reset()
wpan.verify_within(check_prefix_and_slaac_address_are_added, WAIT_INTERVAL)
verify([node.find_ip6_address_with_prefix(PREFIX) for node in all_nodes] == slaac_addrs)

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Remove the prefix on r1 and verify that the address and prefix are
# removed on all nodes.

r1.remove_prefix(PREFIX)


def check_prefix_and_slaac_address_are_removed():
    verify_no_prefix(all_nodes, PREFIX)
    verify_no_address(all_nodes, PREFIX)


wpan.verify_within(check_prefix_and_slaac_address_are_removed, WAIT_INTERVAL)

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Add the prefix on both r1 and r2, then remove from r1 and ensure SLAAC
# addresses are assigned on all nodes.

# Add prefix on r2
r2.add_prefix(PREFIX, stable=True, on_mesh=True, slaac=True)
wpan.verify_within(check_prefix_and_slaac_address_are_added, WAIT_INTERVAL)
verify([node.find_ip6_address_with_prefix(PREFIX) for node in all_nodes] == slaac_addrs)

# Add same prefix on r1 and verify prefix and addresses stay as before
r1.add_prefix(PREFIX, stable=True, on_mesh=True, slaac=True)
wpan.verify_within(check_prefix_and_slaac_address_are_added, WAIT_INTERVAL)
verify([node.find_ip6_address_with_prefix(PREFIX) for node in all_nodes] == slaac_addrs)

# Remove on r1, addresses and prefixes should stay as before (r2 still has
# the same prefix)
r1.remove_prefix(PREFIX)
time.sleep(0.5)
wpan.verify_within(check_prefix_and_slaac_address_are_added, WAIT_INTERVAL)
verify([node.find_ip6_address_with_prefix(PREFIX) for node in all_nodes] == slaac_addrs)

# Remove the prefix on r2 and verify that the address and prefix are now
# removed on all nodes.
r2.remove_prefix(PREFIX)
wpan.verify_within(check_prefix_and_slaac_address_are_removed, WAIT_INTERVAL)

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Add the prefix on r1 (without SLAAC flag) and r2 (with SLAAC flag)

# Add prefix on r1 without SLAAC flag, and on r2 with SLAAC flag
r1.add_prefix(PREFIX, stable=True, on_mesh=True, slaac=False)
r2.add_prefix(PREFIX, stable=True, on_mesh=True, slaac=True)
wpan.verify_within(check_prefix_and_slaac_address_are_added, WAIT_INTERVAL)
verify([node.find_ip6_address_with_prefix(PREFIX) for node in all_nodes] == slaac_addrs)

# Now remove the prefix on r2 and verify that SLAAC address is removed
r2.remove_prefix(PREFIX)


def check_slaac_address_is_removed():
    verify_no_address(all_nodes, PREFIX)


wpan.verify_within(check_slaac_address_is_removed, WAIT_INTERVAL)

r1.remove_prefix(PREFIX)
wpan.verify_within(check_prefix_and_slaac_address_are_removed, WAIT_INTERVAL)

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Check behavior when a user-added address with same prefix already exists.

IP_ADDRESS = PREFIX + "1234"

# Explicitly add an address with the prefix on r1
r1.add_ip6_address_on_interface(IP_ADDRESS)

# Afterwards, add the prefix on r2 (with SLAAC flag)
r2.add_prefix(PREFIX, stable=True, on_mesh=True, slaac=True)
wpan.verify_within(check_prefix_and_slaac_address_are_added, WAIT_INTERVAL)

# Verify that on r1 we do see the user-added address
r1_addrs = wpan.parse_list(r1.get(wpan.WPAN_IP6_ALL_ADDRESSES))
verify(IP_ADDRESS in r1_addrs)

# Also verify that adding the prefix did not add a SLAAC address for same
# prefix on r1
r1_addrs.remove(IP_ADDRESS)
verify(all([not addr.startswith(PREFIX[:-1]) for addr in r1_addrs]))

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Check behavior when a user-added address with same prefix is removed
# (SLAAC module should add a SLAAC address).

r1.remove_ip6_address_on_interface(IP_ADDRESS)
wpan.verify_within(check_prefix_and_slaac_address_are_added, WAIT_INTERVAL)
verify([node.find_ip6_address_with_prefix(PREFIX) for node in all_nodes] == slaac_addrs)

# Re-add the address
r1.add_ip6_address_on_interface(IP_ADDRESS)

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Ensure removal of prefix does not remove user-added address with same prefix.

r2.remove_prefix(PREFIX)


def check_ip6_addresses():
    # Verify that SLAAC addresses are removed on r2 and c2
    verify_no_address([r2, c2], PREFIX)
    # And that user-added address matching the prefix is not removed on r1
    r1_addrs = wpan.parse_list(r1.get(wpan.WPAN_IP6_ALL_ADDRESSES))
    verify(IP_ADDRESS in r1_addrs)


wpan.verify_within(check_ip6_addresses, WAIT_INTERVAL)

r1.remove_ip6_address_on_interface(IP_ADDRESS)

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Ensure disabling SLAAC module removes previously added SLAAC addresses,
# and re-enabling it adds them back.

r1.add_prefix(PREFIX, stable=True, on_mesh=True, slaac=True)
wpan.verify_within(check_prefix_and_slaac_address_are_added, WAIT_INTERVAL)
verify([node.find_ip6_address_with_prefix(PREFIX) for node in all_nodes] == slaac_addrs)

for node in all_nodes:
    node.set(wpan.WPAN_OT_SLAAC_ENABLED, 'false')
wpan.verify_within(check_slaac_address_is_removed, WAIT_INTERVAL)

# Re-enable SLAAC support on NCP and verify addresses are re-added back.
for node in all_nodes:
    node.set(wpan.WPAN_OT_SLAAC_ENABLED, 'true')
wpan.verify_within(check_prefix_and_slaac_address_are_added, WAIT_INTERVAL)
verify([node.find_ip6_address_with_prefix(PREFIX) for node in all_nodes] == slaac_addrs)

r1.remove_prefix(PREFIX)
wpan.verify_within(check_prefix_and_slaac_address_are_removed, WAIT_INTERVAL)

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Check behavior when prefix is added while SLAAC is disabled and then
# enabled later.

for node in all_nodes:
    node.set(wpan.WPAN_OT_SLAAC_ENABLED, 'false')

# Add prefix and verify that there is no address added
r1.add_prefix(PREFIX, stable=True, on_mesh=True, slaac=True)
wpan.verify_within(check_slaac_address_is_removed, WAIT_INTERVAL)

for node in all_nodes:
    node.set(wpan.WPAN_OT_SLAAC_ENABLED, 'true')
wpan.verify_within(check_prefix_and_slaac_address_are_added, WAIT_INTERVAL)
verify([node.find_ip6_address_with_prefix(PREFIX) for node in all_nodes] == slaac_addrs)

# -----------------------------------------------------------------------------------------------------------------------
# Test finished

wpan.Node.finalize_all_nodes()

print('\'{}\' passed.'.format(test_name))
