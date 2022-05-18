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

from wpan import verify
import wpan

# -----------------------------------------------------------------------------------------------------------------------
# Test description: forming a Thread network

test_name = __file__[:-3] if __file__.endswith('.py') else __file__
print('-' * 120)
print('Starting \'{}\''.format(test_name))

# -----------------------------------------------------------------------------------------------------------------------
# Creating `wpan.Nodes` instances

speedup = 4
wpan.Node.set_time_speedup_factor(speedup)

node = wpan.Node()

# -----------------------------------------------------------------------------------------------------------------------
# Init all nodes

wpan.Node.init_all_nodes()

# -----------------------------------------------------------------------------------------------------------------------
# Test implementation

# default values after reset
DEFAULT_NAME = '"OpenThread"'
DEFAULT_PANID = '0xFFFF'
DEFAULT_XPANID = '0xDEAD00BEEF00CAFE'

verify(node.get(wpan.WPAN_STATE) == wpan.STATE_OFFLINE)
verify(node.get(wpan.WPAN_NAME) == DEFAULT_NAME)
verify(node.get(wpan.WPAN_PANID) == DEFAULT_PANID)
verify(node.get(wpan.WPAN_XPANID) == DEFAULT_XPANID)

# Form a network

node.form('asha')
verify(node.get(wpan.WPAN_STATE) == wpan.STATE_ASSOCIATED)
verify(node.get(wpan.WPAN_NODE_TYPE) == wpan.NODE_TYPE_LEADER)
verify(node.get(wpan.WPAN_NAME) == '"asha"')
verify(node.get(wpan.WPAN_PANID) != DEFAULT_PANID)
verify(node.get(wpan.WPAN_XPANID) != DEFAULT_XPANID)

node.leave()
verify(node.get(wpan.WPAN_STATE) == wpan.STATE_OFFLINE)

# Form a network on a specific channel.

node.form('ahura', channel=20)
verify(node.get(wpan.WPAN_STATE) == wpan.STATE_ASSOCIATED)
verify(node.get(wpan.WPAN_NAME) == '"ahura"')
verify(node.get(wpan.WPAN_CHANNEL) == '20')
verify(node.get(wpan.WPAN_PANID) != DEFAULT_PANID)
verify(node.get(wpan.WPAN_XPANID) != DEFAULT_XPANID)

node.leave()
verify(node.get(wpan.WPAN_STATE) == wpan.STATE_OFFLINE)

# Form a network with a specific panid, xpanid and key specified separately

node.set(wpan.WPAN_PANID, '0x1977')
node.set(wpan.WPAN_XPANID, '1020031510006016', binary_data=True)
node.set(wpan.WPAN_KEY, '0123456789abcdeffecdba9876543210', binary_data=True)

node.form('mazda', channel=12)
verify(node.get(wpan.WPAN_STATE) == wpan.STATE_ASSOCIATED)
verify(node.get(wpan.WPAN_NAME) == '"mazda"')
verify(node.get(wpan.WPAN_CHANNEL) == '12')
verify(node.get(wpan.WPAN_KEY) == '[0123456789ABCDEFFECDBA9876543210]')
verify(node.get(wpan.WPAN_PANID) == '0x1977')
verify(node.get(wpan.WPAN_XPANID) == '0x1020031510006016')

node.leave()
verify(node.get(wpan.WPAN_STATE) == wpan.STATE_OFFLINE)

# Form a network with all parameters given as part of `form` command itself

node.form(
    'vahman',
    channel_mask='15,20-24',
    panid='0x1977',
    xpanid='1020031510006016',
    key='0123456789abcdeffecdba9876543210',
    key_index='1',
    mesh_local_prefix='fd00:cafe::',
)

verify(node.get(wpan.WPAN_STATE) == wpan.STATE_ASSOCIATED)
verify(node.get(wpan.WPAN_NAME) == '"vahman"')
channel = int(node.get(wpan.WPAN_CHANNEL), 0)
verify(channel == 15 or (20 <= channel <= 24))
verify(node.get(wpan.WPAN_KEY) == '[0123456789ABCDEFFECDBA9876543210]')
verify(node.get(wpan.WPAN_KEY_INDEX) == '1')
verify(node.get(wpan.WPAN_PANID) == '0x1977')
verify(node.get(wpan.WPAN_XPANID) == '0x1020031510006016')
verify(node.get(wpan.WPAN_IP6_MESH_LOCAL_PREFIX) == '"fd00:cafe::/64"')

# Verify behavior when commands are issued immediately after a `reset`

node.reset()
node.leave()

node.reset()
node.form('net-after-reset')

# -----------------------------------------------------------------------------------------------------------------------
# Test finished

wpan.Node.finalize_all_nodes()

print('\'{}\' passed.'.format(test_name))
