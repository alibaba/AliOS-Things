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
# Test description: This test verifies wpantund properties related to
# `ChannelManager` feature

test_name = __file__[:-3] if __file__.endswith('.py') else __file__
print('-' * 120)
print('Starting \'{}\''.format(test_name))

# -----------------------------------------------------------------------------------------------------------------------
# Creating `wpan.Nodes` instances

node = wpan.Node()

# -----------------------------------------------------------------------------------------------------------------------
# Init all nodes

wpan.Node.init_all_nodes()

# -----------------------------------------------------------------------------------------------------------------------
# Build network topology

node.form("channel-manager", channel=11)

# -----------------------------------------------------------------------------------------------------------------------
# Test implementation

# Check default property values

verify(int(node.get(wpan.WPAN_CHANNEL_MANAGER_NEW_CHANNEL), 0) == 0)
verify(node.get(wpan.WPAN_CHANNEL_MANAGER_AUTO_SELECT_ENABLED) == 'false')
verify(int(node.get(wpan.WPAN_CHANNEL_MANAGER_SUPPORTED_CHANNEL_MASK), 0) == 0)
verify(int(node.get(wpan.WPAN_CHANNEL_MANAGER_FAVORED_CHANNEL_MASK), 0) == 0)

# Set different wpan Channel Manager properties and get and check the output

node.set(wpan.WPAN_CHANNEL_MANAGER_DELAY, '180')
verify(int(node.get(wpan.WPAN_CHANNEL_MANAGER_DELAY), 0) == 180)

node.set(wpan.WPAN_CHANNEL_MANAGER_AUTO_SELECT_ENABLED, '1')
verify(node.get(wpan.WPAN_CHANNEL_MANAGER_AUTO_SELECT_ENABLED) == 'true')

node.set(wpan.WPAN_CHANNEL_MANAGER_AUTO_SELECT_ENABLED, '0')
verify(node.get(wpan.WPAN_CHANNEL_MANAGER_AUTO_SELECT_ENABLED) == 'false')

node.set(wpan.WPAN_CHANNEL_MANAGER_AUTO_SELECT_INTERVAL, '1000')
verify(int(node.get(wpan.WPAN_CHANNEL_MANAGER_AUTO_SELECT_INTERVAL), 0) == 1000)

all_channels_mask = int('0x7fff800', 0)
chan_11_mask = int('0x800', 0)
chan_11_to_13_mask = int('0x3800', 0)

node.set(wpan.WPAN_CHANNEL_MANAGER_SUPPORTED_CHANNEL_MASK, str(all_channels_mask))
verify(int(node.get(wpan.WPAN_CHANNEL_MANAGER_SUPPORTED_CHANNEL_MASK), 0) == all_channels_mask)

node.set(wpan.WPAN_CHANNEL_MANAGER_FAVORED_CHANNEL_MASK, str(chan_11_mask))
verify(int(node.get(wpan.WPAN_CHANNEL_MANAGER_FAVORED_CHANNEL_MASK), 0) == chan_11_mask)

node.set(wpan.WPAN_CHANNEL_MANAGER_SUPPORTED_CHANNEL_MASK, str(chan_11_to_13_mask))
verify(int(node.get(wpan.WPAN_CHANNEL_MANAGER_SUPPORTED_CHANNEL_MASK), 0) == chan_11_to_13_mask)

node.set(wpan.WPAN_CHANNEL_MANAGER_FAVORED_CHANNEL_MASK, str(all_channels_mask))
verify(int(node.get(wpan.WPAN_CHANNEL_MANAGER_FAVORED_CHANNEL_MASK), 0) == all_channels_mask)

node.set(wpan.WPAN_CHANNEL_MANAGER_AUTO_SELECT_ENABLED, '1')
verify(node.get(wpan.WPAN_CHANNEL_MANAGER_AUTO_SELECT_ENABLED) == 'true')

# Check to ensure the property values are retained after an NCP reset

node.reset()

start_time = time.time()
wait_time = 20

while node.get(wpan.WPAN_STATE) != wpan.STATE_ASSOCIATED:
    if time.time() - start_time > wait_time:
        print('Took too long to restore after reset ({}>{} sec)'.format(time.time() - start_time, wait_time))
        exit(1)
    time.sleep(2)

verify(node.get(wpan.WPAN_CHANNEL_MANAGER_AUTO_SELECT_ENABLED) == 'true')
verify(int(node.get(wpan.WPAN_CHANNEL_MANAGER_FAVORED_CHANNEL_MASK), 0) == all_channels_mask)
verify(int(node.get(wpan.WPAN_CHANNEL_MANAGER_SUPPORTED_CHANNEL_MASK), 0) == chan_11_to_13_mask)
verify(int(node.get(wpan.WPAN_CHANNEL_MANAGER_AUTO_SELECT_INTERVAL), 0) == 1000)
verify(int(node.get(wpan.WPAN_CHANNEL_MANAGER_DELAY), 0) == 180)

# -----------------------------------------------------------------------------------------------------------------------
# Test finished

wpan.Node.finalize_all_nodes()

print('\'{}\' passed.'.format(test_name))
