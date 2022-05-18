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
# Test description: Child Supervision feature
#
# This test covers the behavior of Child Supervision feature.
#
# This test uses MAC allowlisting to emulate the situation where a child is
# removed from parent's child table while the child continues to stay attached
# to the parent (since data polls from child are acked at radio platform layer).
# Specifically the test verifies that once supervision check is enabled on the
# child, the child detects that it is no longer present in the parent's table
# and tries to re-attach.

# The test verifies the behavior of both parent and child, when supervision is
# enabled. It verifies that parent is periodically sending supervision messages
# to the child and that the child is monitoring the messages.
#
# This test also indirectly verifies the child timeout on parent.
#

test_name = __file__[:-3] if __file__.endswith('.py') else __file__
print('-' * 120)
print('Starting \'{}\''.format(test_name))

# -----------------------------------------------------------------------------------------------------------------------
# Creating `wpan.Nodes` instances

speedup = 2
wpan.Node.set_time_speedup_factor(speedup)

parent = wpan.Node()
child = wpan.Node()

# -----------------------------------------------------------------------------------------------------------------------
# Init all nodes

wpan.Node.init_all_nodes()

# -----------------------------------------------------------------------------------------------------------------------
# Build network topology

CHILD_TIMEOUT = 6
CHILD_SUPERVISION_CHECK_TIMEOUT = 2
PARENT_SUPERVISION_INTERVAL = 1

child.set(wpan.WPAN_POLL_INTERVAL, '500')
child.set(wpan.WPAN_THREAD_CHILD_TIMEOUT, str(CHILD_TIMEOUT))

parent.form("child-sup")
child.join_node(parent, wpan.JOIN_TYPE_SLEEPY_END_DEVICE)

# -----------------------------------------------------------------------------------------------------------------------
# Test implementation

# Disable child supervision on child and parent
parent.set(wpan.WPAN_CHILD_SUPERVISION_INTERVAL, '0')
child.set(wpan.WPAN_CHILD_SUPERVISION_CHECK_TIMEOUT, '0')
verify(int(parent.get(wpan.WPAN_CHILD_SUPERVISION_INTERVAL), 0) == 0)
verify(int(child.get(wpan.WPAN_CHILD_SUPERVISION_CHECK_TIMEOUT), 0) == 0)

# Check that the child is associated and has correct timeout
verify(child.is_associated())
verify(int(child.get(wpan.WPAN_THREAD_CHILD_TIMEOUT), 0) == CHILD_TIMEOUT)

# Verify the child table on parent contains the child with correct timeout
child_table = wpan.parse_child_table_result(parent.get(wpan.WPAN_THREAD_CHILD_TABLE))
verify(len(child_table) == 1)
verify(int(child_table[0].timeout, 0) == CHILD_TIMEOUT)

# Enabling allowlisting on parent
#
# Since child is not in parent's allowlist, the data polls from child
# should be rejected and the child should be removed from parent's
# child table after timeout. The child however should continue to
# stay attached (since data polls are acked by radio driver) and
# supervision check is disabled on the child.

parent.set(wpan.WPAN_MAC_ALLOWLIST_ENABLED, '1')


def check_child_is_removed_from_parent_child_table():
    child_table = wpan.parse_child_table_result(parent.get(wpan.WPAN_THREAD_CHILD_TABLE))
    verify(len(child_table) == 0)


# wait till child is removed from parent's child table
# after this child should still be associated
wpan.verify_within(check_child_is_removed_from_parent_child_table, CHILD_TIMEOUT / speedup + 2)
verify(child.is_associated())

# Enable supervision check on child and expect the child to
# become detached after the check timeout

child.set(
    wpan.WPAN_CHILD_SUPERVISION_CHECK_TIMEOUT,
    str(CHILD_SUPERVISION_CHECK_TIMEOUT),
)


def check_child_is_detached():
    verify(not child.is_associated())


wpan.verify_within(check_child_is_detached, CHILD_SUPERVISION_CHECK_TIMEOUT / speedup + 8)

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

# Enable child supervision on parent and disable allowlisting

parent.set(wpan.WPAN_CHILD_SUPERVISION_INTERVAL, str(PARENT_SUPERVISION_INTERVAL))
parent.set(wpan.WPAN_MAC_ALLOWLIST_ENABLED, '0')

# Wait for the child to attach back


def check_child_is_attached():
    verify(child.is_associated())


wpan.verify_within(check_child_is_attached, 5)

# MAC counters are used to verify the child supervision behavior.

parent_unicast_tx_count = int(parent.get("NCP:Counter:TX_PKT_UNICAST"), 0)

time.sleep(PARENT_SUPERVISION_INTERVAL * 1.2 / speedup)

# To verify that the parent is indeed sending empty "supervision"
# messages to its child, MAC counter for number of unicast tx is
# used. Note that supervision interval on parent is set to 1 sec.

verify(int(parent.get("NCP:Counter:TX_PKT_UNICAST"), 0) >= parent_unicast_tx_count + 1)

verify(child.is_associated())

# Disable child supervision on parent
parent.set(wpan.WPAN_CHILD_SUPERVISION_INTERVAL, '0')

time.sleep(CHILD_SUPERVISION_CHECK_TIMEOUT * 3 / speedup)
verify(child.is_associated())

# -----------------------------------------------------------------------------------------------------------------------
# Test finished

wpan.Node.finalize_all_nodes()

print('\'{}\' passed.'.format(test_name))
