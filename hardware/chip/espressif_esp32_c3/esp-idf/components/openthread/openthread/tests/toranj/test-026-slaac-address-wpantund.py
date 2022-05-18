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
# Test description: SLAAC address
#
# This test covers the addition/removal of SLAAC IPv6 address by `wpantund`.
#

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

r1.form('slaac-test')

r1.allowlist_node(r2)
r2.allowlist_node(r1)

r2.join_node(r1, node_type=wpan.JOIN_TYPE_ROUTER)

c2.allowlist_node(r2)
r2.allowlist_node(c2)

c2.join_node(r2, node_type=wpan.JOIN_TYPE_END_DEVICE)

# -----------------------------------------------------------------------------------------------------------------------
# Test implementation

# This test covers the SLAAC address management by `wpantund`. So before starting the test we ensure that SLAAC module
# on NCP is disabled on all nodes

for node in all_nodes:
    node.set(wpan.WPAN_OT_SLAAC_ENABLED, 'false')
    verify(node.get(wpan.WPAN_OT_SLAAC_ENABLED) == 'false')

WAIT_INTERVAL = 5

PREFIX = 'fd00:abba:beef:cafe::'

r1.add_prefix(PREFIX, stable=True, on_mesh=True, slaac=True)

# Verify that all nodes get the prefix and add the SLAAC address


def check_prefix_and_slaac_address_are_added():
    verify_prefix(all_nodes, PREFIX, stable=True, on_mesh=True, slaac=True)
    verify_address(all_nodes, PREFIX)


wpan.verify_within(check_prefix_and_slaac_address_are_added, WAIT_INTERVAL)

# Reset r1 and check that prefix and SLAAC address are re-added
r1.reset()
wpan.verify_within(check_prefix_and_slaac_address_are_added, WAIT_INTERVAL)

# Remove the prefix on r1 and verify that the address and prefix are
# removed on all nodes.
r1.remove_prefix(PREFIX)


def check_prefix_and_slaac_address_are_removed():
    verify_no_prefix(all_nodes, PREFIX)
    verify_no_address(all_nodes, PREFIX)


wpan.verify_within(check_prefix_and_slaac_address_are_removed, WAIT_INTERVAL)

# Add prefix on r2
r2.add_prefix(PREFIX, stable=True, on_mesh=True, slaac=True)
wpan.verify_within(check_prefix_and_slaac_address_are_added, WAIT_INTERVAL)

# Add same prefix on r1 and verify prefix and addresses stay as before
r1.add_prefix(PREFIX, stable=True, on_mesh=True, slaac=True)
wpan.verify_within(check_prefix_and_slaac_address_are_added, WAIT_INTERVAL)

# Remove on r1, addresses and prefixes should stay as before (r2 still has
# the same prefix)
r1.remove_prefix(PREFIX)
time.sleep(0.5)
wpan.verify_within(check_prefix_and_slaac_address_are_added, WAIT_INTERVAL)

# Remove the prefix on r2 and verify that the address and prefix are now
# removed on all nodes.
r2.remove_prefix(PREFIX)
wpan.verify_within(check_prefix_and_slaac_address_are_removed, WAIT_INTERVAL)

# Add prefix on r1 without SLAAC flag, and or r2 with SLAAC flag
r1.add_prefix(PREFIX, stable=True, on_mesh=True, slaac=False)
r2.add_prefix(PREFIX, stable=True, on_mesh=True, slaac=True)
wpan.verify_within(check_prefix_and_slaac_address_are_added, WAIT_INTERVAL)

# Now remove the prefix on r2 and verify that SLAAC address is removed
r2.remove_prefix(PREFIX)


def check_slaac_address_is_removed():
    verify_no_address(all_nodes, PREFIX)


wpan.verify_within(check_slaac_address_is_removed, WAIT_INTERVAL)

r1.remove_prefix(PREFIX)
wpan.verify_within(check_prefix_and_slaac_address_are_removed, WAIT_INTERVAL)

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

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

# Remove the PREFIX on r2
r2.remove_prefix(PREFIX)


def check_ip6_addresses():
    # Verify that SLAAC addresses are removed on r2 and c2
    verify_no_address([r2, c2], PREFIX)
    # And that user-added address matching the preifx is not removed on r1
    r1_addrs = wpan.parse_list(r1.get(wpan.WPAN_IP6_ALL_ADDRESSES))
    verify(IP_ADDRESS in r1_addrs)


wpan.verify_within(check_ip6_addresses, WAIT_INTERVAL)

# Send from r2 to r1 using the user-added address verifying that address
# is present on NCP
IP_ADDRESS_2 = PREFIX + "2"
r2.add_ip6_address_on_interface(IP_ADDRESS_2)
sender = r2.prepare_tx(IP_ADDRESS_2, IP_ADDRESS, "Hello r1 from r2")
recver = r1.prepare_rx(sender)
wpan.Node.perform_async_tx_rx()
verify(sender.was_successful)
verify(recver.was_successful)

# -----------------------------------------------------------------------------------------------------------------------
# Test finished

wpan.Node.finalize_all_nodes()

print('\'{}\' passed.'.format(test_name))
