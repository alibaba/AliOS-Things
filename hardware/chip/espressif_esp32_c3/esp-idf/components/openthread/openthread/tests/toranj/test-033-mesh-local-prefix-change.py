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

import wpan
from wpan import verify

# -----------------------------------------------------------------------------------------------------------------------
# Test description:
#
# This test covers the situation where a node attached to a parent with a different mesh-local prefix. It verifies
# that the attaching node adopts the parent's mesh-local prefix and the RLOC addresses on the node are correctly
# filtered (by wpantund).

test_name = __file__[:-3] if __file__.endswith('.py') else __file__
print('-' * 120)
print('Starting \'{}\''.format(test_name))

# -----------------------------------------------------------------------------------------------------------------------
# Creating `wpan.Nodes` instances

speedup = 4
wpan.Node.set_time_speedup_factor(speedup)

node1 = wpan.Node()
node2 = wpan.Node()

# -----------------------------------------------------------------------------------------------------------------------
# Init all nodes

wpan.Node.init_all_nodes()

# -----------------------------------------------------------------------------------------------------------------------
# Test implementation

NET_NAME = 'ml-change'
CHANNEL = 11
PANID = '0x1977'
XPANID = '1020031510006016'
KEY = '0123456789abcdeffecdba9876543210'

ML_PREFIX_1 = 'fd00:1::'
ML_PREFIX_2 = 'fd00:2::'

# Form a network on node1
node1.form(
    NET_NAME,
    channel=CHANNEL,
    panid=PANID,
    xpanid=XPANID,
    key=KEY,
    mesh_local_prefix=ML_PREFIX_1,
)

# On node2, form a network with same parameters but a different mesh-local
# prefix
node2.form(
    NET_NAME,
    channel=CHANNEL,
    panid=PANID,
    xpanid=XPANID,
    key=KEY,
    mesh_local_prefix=ML_PREFIX_2,
)

# Node 2 is expected to attach to node1 and adopt the mesh-local prefix
# from node1

verify(node2.is_associated())
verify(node2.get(wpan.WPAN_IP6_MESH_LOCAL_PREFIX) == node1.get(wpan.WPAN_IP6_MESH_LOCAL_PREFIX))

# Ensure that there are only two addresses on the node2 (link-local and mesh-local address) and that RLOC
# address is correctly filtered (by wpantund).
verify(len(wpan.parse_list(node2.get(wpan.WPAN_IP6_ALL_ADDRESSES))) == 2)

# -----------------------------------------------------------------------------------------------------------------------
# Test finished

wpan.Node.finalize_all_nodes()

print('\'{}\' passed.'.format(test_name))
