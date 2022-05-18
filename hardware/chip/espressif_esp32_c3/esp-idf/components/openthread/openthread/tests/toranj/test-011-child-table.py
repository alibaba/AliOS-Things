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
# Test description: Child-table and child recovery
#

test_name = __file__[:-3] if __file__.endswith('.py') else __file__
print('-' * 120)
print('Starting \'{}\''.format(test_name))

# -----------------------------------------------------------------------------------------------------------------------
# Creating `wpan.Nodes` instances

speedup = 4
wpan.Node.set_time_speedup_factor(speedup)

NUM_CHILDREN = 7

router = wpan.Node()

children = []
for i in range(NUM_CHILDREN):
    children.append(wpan.Node())

all_nodes = [router] + children

# -----------------------------------------------------------------------------------------------------------------------
# Init all nodes

wpan.Node.init_all_nodes()

# -----------------------------------------------------------------------------------------------------------------------
# Build network topology

router.form('child-table')
for child in children:
    child.join_node(router, node_type=wpan.JOIN_TYPE_SLEEPY_END_DEVICE)
    child.set(wpan.WPAN_POLL_INTERVAL, '1000')

# -----------------------------------------------------------------------------------------------------------------------
# Test implementation

# Get the child table and verify all children are in the table.

child_table = wpan.parse_child_table_result(router.get(wpan.WPAN_THREAD_CHILD_TABLE))

verify(len(child_table) == len(children))

for child in children:
    ext_addr = child.get(wpan.WPAN_EXT_ADDRESS)[1:-1]
    for entry in child_table:
        if entry.ext_address == ext_addr:
            break
    else:
        print('Failed to find a child entry for extended address {} in table'.format(ext_addr))
        exit(1)

    verify(int(entry.rloc16, 16) == int(child.get(wpan.WPAN_THREAD_RLOC16), 16))
    verify(int(entry.timeout, 0) == 120)
    verify(entry.is_rx_on_when_idle() is False)
    verify(entry.is_ftd() is False)

# -----------------------------------------------------------------------------------------------------------------------
# Test finished

wpan.Node.finalize_all_nodes()

print('\'{}\' passed.'.format(test_name))
