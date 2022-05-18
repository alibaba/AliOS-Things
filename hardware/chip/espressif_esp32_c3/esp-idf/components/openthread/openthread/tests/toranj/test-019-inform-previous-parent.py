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
# Test description: Test behavior of "Inform Previous Parent" feature
#
# With this feature enabled, when a child attaches to a new parent, it will send
# an IP message (with empty payload and mesh-local IP address as the source
# address) to its previous parent. Upon receiving this message the previous
# parent would immediately remove the child from its child table. Without this
# feature, the child entry on previous parent would stay (and parent would
# continue to queue messages for the sleepy child) until the child is timed out
# and removed from child table.
#
#
# Test topology:
#
# `child` is first attached to `parent2`. It is then forced to switch to `parent1`
#
#     parent1--- parent2
#          .      /
#           \    /
#            .  /
#            child
#
# This test verifies the behavior of the child and parents under this feature.
#
# Note that `OPENTHREAD_CONFIG_MLE_INFORM_PREVIOUS_PARENT_ON_REATTACH` is enabled in
# `openthread-core-toranj.config.h` header file.
#

test_name = __file__[:-3] if __file__.endswith('.py') else __file__
print('-' * 120)
print('Starting \'{}\''.format(test_name))

# -----------------------------------------------------------------------------------------------------------------------
# Creating `wpan.Nodes` instances

speedup = 4
wpan.Node.set_time_speedup_factor(speedup)

parent1 = wpan.Node()
parent2 = wpan.Node()
child = wpan.Node()

# -----------------------------------------------------------------------------------------------------------------------
# Init all nodes

wpan.Node.init_all_nodes()

# -----------------------------------------------------------------------------------------------------------------------
# Build network topology
#
# `child` is first attached to `parent2`. It is then forced to switch to `parent1`.
#
#     parent1--- parent2
#          .      /
#           \    /
#            .  /
#            child
#

parent1.allowlist_node(parent2)
parent2.allowlist_node(parent1)
parent2.allowlist_node(child)

parent1.form("inform-parent")
parent2.join_node(parent1, wpan.JOIN_TYPE_ROUTER)
child.join_node(parent2, wpan.JOIN_TYPE_SLEEPY_END_DEVICE)
child.set(wpan.WPAN_POLL_INTERVAL, '300')

# -----------------------------------------------------------------------------------------------------------------------
# Test implementation
#

CHILD_SUPERVISION_CHECK_TIMEOUT = 2
PARENT_SUPERVISION_INTERVAL = 1

# Verify the `child` is attached to `parent2`.
child_table = wpan.parse_list(parent2.get(wpan.WPAN_THREAD_CHILD_TABLE))
verify(len(child_table) == 1)

# Remove the `child` from allowlist of `parent2` and add it to allowlist
# of `parent1` instead.
parent1.allowlist_node(child)
parent2.un_allowlist_node(child)

# Enable supervision check on the `child` and also on `parent1`.

child.set(
    wpan.WPAN_CHILD_SUPERVISION_CHECK_TIMEOUT,
    str(CHILD_SUPERVISION_CHECK_TIMEOUT),
)
parent1.set(wpan.WPAN_CHILD_SUPERVISION_INTERVAL, str(PARENT_SUPERVISION_INTERVAL))

# Since child supervision is not enabled on `parent2` and the `child` is
# removed from allowlist on `parent2`, after the supervision check timeout
# the `child` should realize that it can no longer talk to its current
# parent (`parent2`) and try to reattach. All re-attach attempts to `parent2`
# should fail (due to allowlist) and cause the `child` to get detached and
# search for a new parent and then attach to `parent1`.
#
# To verify that the `child` does get detached and attach to a new parent, we
# monitor the number of state changes using wpantund property "stat:ncp".

child_num_state_changes = len(wpan.parse_list(child.get("stat:ncp")))


def check_child_is_reattached():
    verify(len(wpan.parse_list(child.get("stat:ncp"))) > child_num_state_changes)
    verify(child.is_associated())


wpan.verify_within(check_child_is_reattached, CHILD_SUPERVISION_CHECK_TIMEOUT / speedup + 5)

# Verify that the `child` is now attached to `parent1`
child_table = wpan.parse_list(parent1.get(wpan.WPAN_THREAD_CHILD_TABLE))
verify(len(child_table) == 1)

# Finally verify that the `child` is removed from previous parent's child
# table (which indicates that the `child` did indeed inform its previous
# parent).


def check_child_is_removed_from_parent2_table():
    child_table = wpan.parse_list(parent2.get(wpan.WPAN_THREAD_CHILD_TABLE))
    verify(len(child_table) == 0)


wpan.verify_within(check_child_is_removed_from_parent2_table, 1)

# -----------------------------------------------------------------------------------------------------------------------
# Test finished

wpan.Node.finalize_all_nodes()

print('\'{}\' passed.'.format(test_name))
