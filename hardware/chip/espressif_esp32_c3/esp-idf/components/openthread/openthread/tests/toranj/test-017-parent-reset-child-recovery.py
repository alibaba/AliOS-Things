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
# Test description: Parent restoring children after reset
#
# This test covers the following:
#
# - A parent node with a given number sleepy and rx-on children.
# - Parent node is reset
# - It is verified that all children are recovered and are present in the parent's child table
# - It is also verified that all children are restored on parent through "Child Update" exchange process and none of
#   them got detached and needed to attach back.
#

test_name = __file__[:-3] if __file__.endswith('.py') else __file__
print('-' * 120)
print('Starting \'{}\''.format(test_name))

# -----------------------------------------------------------------------------------------------------------------------
# Creating `wpan.Nodes` instances

speedup = 2
wpan.Node.set_time_speedup_factor(speedup)

NUM_SLEEPY_CHILDREN = 3
NUM_RX_ON_CHILDREN = 2

NUM_CHILDREN = NUM_SLEEPY_CHILDREN + NUM_RX_ON_CHILDREN

parent = wpan.Node()

sleepy_children = []
for num in range(NUM_SLEEPY_CHILDREN):
    sleepy_children.append(wpan.Node())

rx_on_children = []
for num in range(NUM_RX_ON_CHILDREN):
    rx_on_children.append(wpan.Node())

all_children = sleepy_children + rx_on_children

# -----------------------------------------------------------------------------------------------------------------------
# Init all nodes

wpan.Node.init_all_nodes()

# -----------------------------------------------------------------------------------------------------------------------
# Build network topology
#

parent.form("recovery")

for child in sleepy_children:
    child.join_node(parent, wpan.JOIN_TYPE_SLEEPY_END_DEVICE)
    child.set(wpan.WPAN_POLL_INTERVAL, '4000')

for child in rx_on_children:
    child.join_node(parent, wpan.JOIN_TYPE_END_DEVICE)

# -----------------------------------------------------------------------------------------------------------------------
# Test implementation

# The test verifies that all children are recovered after the parent is reset.
#
# The `wpantund` statistics collector is used to determine if the children are
# recovered through "Child Update Request/Response" exchange triggered by parent
# after it was reset (child restoration process) or if they got detached and
# went through full attach process again. The wpantund stat-collector "stat:ncp"
# property keeps track of all NCP state changes on a device:
#
# Node.wpanctl('get -v stat:ncp'):
#    [
#       "NCP State History"
#       "-------------------------"
#       "00:00:03.220 ago -> associated"
#       "00:00:03.604 ago -> associating"
#       "00:00:06.027 ago -> offline"
#    ]
#
#
# In the first case (child being restored through "Child Update" exchange) the
# child should never be detached and its "stat:ncp" history should remain the
# same. In the second case the child would get detached and we should
# observe a wpantund state change "associated:no-parent" before it gets attached
# again:
#
# Node.wpanctl('get -v stat:ncp'):
#    [
#       "NCP State History"
#       "-------------------------"
#       "00:00:01.160 ago -> associated"
#       "00:00:01.814 ago -> associated:no-parent"
#       "00:00:04.911 ago -> associated"
#       "00:00:05.298 ago -> associating"
#       "00:00:06.475 ago -> offline"
#    ]
#


def check_child_table():
    # Checks the child table includes the expected number of children.
    child_table = wpan.parse_list(parent.get(wpan.WPAN_THREAD_CHILD_TABLE))
    verify(len(child_table) == NUM_CHILDREN)


# Verify that all children are present in the child table
check_child_table()

# Remember number of NCP state changes (using "stat:ncp" property) per child
child_num_state_changes = []
for child in all_children:
    child_num_state_changes.append(len(wpan.parse_list(child.get("stat:ncp"))))

# Reset the parent
parent.reset()


def check_parent_is_associated():
    verify(parent.is_associated())


wpan.verify_within(check_parent_is_associated, 5)

# Verify that all the children are recovered and present in the parent's
# child table again (within 5 seconds).
wpan.verify_within(check_child_table, 9)

# Verify that number of state changes on all children stays as before
# (indicating they did not get detached).
for i in range(len(all_children)):
    verify(child_num_state_changes[i] == len(wpan.parse_list(all_children[i].get("stat:ncp"))))

# -----------------------------------------------------------------------------------------------------------------------
# Test finished

wpan.Node.finalize_all_nodes()

print('\'{}\' passed.'.format(test_name))
