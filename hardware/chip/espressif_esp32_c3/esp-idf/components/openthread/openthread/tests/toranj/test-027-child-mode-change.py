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

import wpan
from wpan import verify

# -----------------------------------------------------------------------------------------------------------------------
# Test description: Verify Thread mode change on children and recovery after parent reset.
#

test_name = __file__[:-3] if __file__.endswith('.py') else __file__
print('-' * 120)
print('Starting \'{}\''.format(test_name))

# -----------------------------------------------------------------------------------------------------------------------
# Utility functions


def verify_child_table(parent, children):
    """
    This function verifies that child table on `parent` node contains all the entries in `children` list and the child
    table entry's mode value matches the children Thread mode.
    """
    child_table = wpan.parse_child_table_result(parent.get(wpan.WPAN_THREAD_CHILD_TABLE))
    verify(len(child_table) == len(children))
    for child in children:
        ext_addr = child.get(wpan.WPAN_EXT_ADDRESS)[1:-1]
        for entry in child_table:
            if entry.ext_address == ext_addr:
                break
        else:
            raise wpan.VerifyError('Failed to find a child entry for extended address {} in table'.format(ext_addr))
            exit(1)

        verify(int(entry.rloc16, 16) == int(child.get(wpan.WPAN_THREAD_RLOC16), 16))
        mode = int(child.get(wpan.WPAN_THREAD_DEVICE_MODE), 0)
        verify(entry.is_rx_on_when_idle() == (mode & wpan.THREAD_MODE_FLAG_RX_ON_WHEN_IDLE != 0))
        verify(entry.is_ftd() == (mode & wpan.THREAD_MODE_FLAG_FULL_THREAD_DEV != 0))
        verify(entry.is_full_net_data() == (mode & wpan.THREAD_MODE_FLAG_FULL_NETWORK_DATA != 0))


# -----------------------------------------------------------------------------------------------------------------------
# Creating `wpan.Nodes` instances

speedup = 4
wpan.Node.set_time_speedup_factor(speedup)

parent = wpan.Node()
child1 = wpan.Node()
child2 = wpan.Node()

# -----------------------------------------------------------------------------------------------------------------------
# Init all nodes

wpan.Node.init_all_nodes()

# -----------------------------------------------------------------------------------------------------------------------
# Build network topology
#

parent.form("ModeChangeReset")

child1.join_node(parent, wpan.JOIN_TYPE_END_DEVICE)
child2.join_node(parent, wpan.JOIN_TYPE_SLEEPY_END_DEVICE)

child1.set(wpan.WPAN_POLL_INTERVAL, '8000')
child2.set(wpan.WPAN_POLL_INTERVAL, '8000')

children = [child1, child2]

# -----------------------------------------------------------------------------------------------------------------------
# Test implementation

WAIT_INTERVAL = 6

# Thread Mode for end-device and sleepy end-device
DEVICE_MODE_SLEEPY_END_DEVICE = (wpan.THREAD_MODE_FLAG_FULL_NETWORK_DATA)
DEVICE_MODE_END_DEVICE = (wpan.THREAD_MODE_FLAG_FULL_NETWORK_DATA | wpan.THREAD_MODE_FLAG_FULL_THREAD_DEV |
                          wpan.THREAD_MODE_FLAG_RX_ON_WHEN_IDLE)

# Disable child supervision on all devices
parent.set(wpan.WPAN_CHILD_SUPERVISION_INTERVAL, '0')
child1.set(wpan.WPAN_CHILD_SUPERVISION_CHECK_TIMEOUT, '0')
child2.set(wpan.WPAN_CHILD_SUPERVISION_CHECK_TIMEOUT, '0')

# Verify Thread Device Mode on both children
verify(int(child1.get(wpan.WPAN_THREAD_DEVICE_MODE), 0) == DEVICE_MODE_END_DEVICE)
verify(int(child2.get(wpan.WPAN_THREAD_DEVICE_MODE), 0) == DEVICE_MODE_SLEEPY_END_DEVICE)


def check_child_table():
    verify_child_table(parent, children)


wpan.verify_within(check_child_table, WAIT_INTERVAL)

# Prepare a set of IP message tx to both children
NUM_MSGS = 6

parent_ml_address = parent.get(wpan.WPAN_IP6_MESH_LOCAL_ADDRESS)[1:-1]
child2_ml_address = child2.get(wpan.WPAN_IP6_MESH_LOCAL_ADDRESS)[1:-1]

sender = parent.prepare_tx(parent_ml_address, child2_ml_address, 800, NUM_MSGS)

child2_rx_ip_counter = int(child2.get(wpan.WPAN_NCP_COUNTER_RX_IP_SEC_TOTAL), 0)

wpan.Node.perform_async_tx_rx()

verify(sender.was_successful)

# Change mode on both children (make child1 sleepy, and child2 non-sleepy)

child1.set(wpan.WPAN_THREAD_DEVICE_MODE, str(DEVICE_MODE_SLEEPY_END_DEVICE))
verify(int(child1.get(wpan.WPAN_THREAD_DEVICE_MODE), 0) == DEVICE_MODE_SLEEPY_END_DEVICE)

child2.set(wpan.WPAN_THREAD_DEVICE_MODE, str(DEVICE_MODE_END_DEVICE))
verify(int(child2.get(wpan.WPAN_THREAD_DEVICE_MODE), 0) == DEVICE_MODE_END_DEVICE)

# Verify that the child table on parent is also updated
wpan.verify_within(check_child_table, WAIT_INTERVAL)


def check_child2_received_msg():
    verify(int(child2.get(wpan.WPAN_NCP_COUNTER_RX_IP_SEC_TOTAL), 0) >= child2_rx_ip_counter + NUM_MSGS)


wpan.verify_within(check_child2_received_msg, WAIT_INTERVAL)

# Reset parent and verify all children are recovered
parent.reset()
wpan.verify_within(check_child_table, WAIT_INTERVAL)

# -----------------------------------------------------------------------------------------------------------------------
# Test finished

wpan.Node.finalize_all_nodes()

print('\'{}\' passed.'.format(test_name))
