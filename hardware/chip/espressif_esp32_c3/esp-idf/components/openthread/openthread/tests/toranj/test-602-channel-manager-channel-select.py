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
# Test description: verifies `ChannelManager` channel selection procedure

test_name = __file__[:-3] if __file__.endswith('.py') else __file__
print('-' * 120)
print('Starting \'{}\''.format(test_name))


def verify_channel(nodes, new_channel, wait_time=20):
    """
    This function checks the channel on a given list of `nodes` and verifies that all nodes
    switch to a given `new_channel` (as int) within certain `wait_time` (int and in seconds)
    """
    start_time = time.time()

    while not all([(new_channel == int(node.get(wpan.WPAN_CHANNEL), 0)) for node in nodes]):
        if time.time() - start_time > wait_time:
            print('Took too long to switch to channel {} ({}>{} sec)'.format(new_channel,
                                                                             time.time() - start_time, wait_time))
            exit(1)
        time.sleep(0.1)


# -----------------------------------------------------------------------------------------------------------------------
# Creating `wpan.Nodes` instances

# Run the test with 10,000 time speedup factor
wpan.Node.set_time_speedup_factor(10000)

node = wpan.Node()

# -----------------------------------------------------------------------------------------------------------------------
# Init all nodes

wpan.Node.init_all_nodes()

# -----------------------------------------------------------------------------------------------------------------------
# Build network topology

node.form('channel-manager', channel=24)

# -----------------------------------------------------------------------------------------------------------------------
# Test implementation

all_channels_mask = int('0x7fff800', 0)
chan_12_to_15_mask = int('0x000f000', 0)
chan_15_to_17_mask = int('0x0038000', 0)

# Set supported channel mask to be all channels
node.set(wpan.WPAN_CHANNEL_MANAGER_SUPPORTED_CHANNEL_MASK, str(all_channels_mask))
verify(int(node.get(wpan.WPAN_CHANNEL_MANAGER_SUPPORTED_CHANNEL_MASK), 0) == all_channels_mask)

WAIT_TIME = 15
EXPECTED_SAMEPLE_COUNT = 970

# Sleep for 4.5 second with speedup factor of 10,000 this is more than 12
# hours. We sleep instead of immediately checking the sample counter in
# order to not add more actions/events into simulation (specially since
# we are running at very high speedup).
time.sleep(4.5)


def check_sample_count():
    verify(int(node.get(wpan.WPAN_CHANNEL_MONITOR_SAMPLE_COUNT), 0) > EXPECTED_SAMEPLE_COUNT)


wpan.verify_within(check_sample_count, WAIT_TIME)

# Verify the initial value of `NEW_CHANNEL` (should be zero if there has
# been no channel change so far).

verify(int(node.get(wpan.WPAN_CHANNEL_MANAGER_NEW_CHANNEL), 0) == 0)

# Issue a channel-select with quality check enabled, and verify that no
# action is taken.

node.set(wpan.WPAN_CHANNEL_MANAGER_CHANNEL_SELECT, 'false')
verify(int(node.get(wpan.WPAN_CHANNEL_MANAGER_NEW_CHANNEL), 0) == 0)

# Issue a channel-select with quality check disabled, verify that channel
# is switched to channel 11.

node.set(wpan.WPAN_CHANNEL_MANAGER_CHANNEL_SELECT, 'true')
verify(int(node.get(wpan.WPAN_CHANNEL_MANAGER_NEW_CHANNEL), 0) == 11)
verify_channel([node], 11)

# Set channels 12-15 as favorable and request a channel select, verify that channel is switched to 12.
#
# Even though 11 would be best, quality difference between 11 and 12 is not high enough for selection
# algorithm to pick an unfavored channel.

node.set(wpan.WPAN_CHANNEL_MANAGER_FAVORED_CHANNEL_MASK, str(chan_12_to_15_mask))
node.set(wpan.WPAN_CHANNEL_MANAGER_NEW_CHANNEL, '25')  # request a channel change to 25
verify_channel([node], 25)
node.set(wpan.WPAN_CHANNEL_MANAGER_CHANNEL_SELECT, 'true')
verify(int(node.get(wpan.WPAN_CHANNEL_MANAGER_NEW_CHANNEL), 0) == 12)
verify_channel([node], 12)

# Set channels 15-17 as favorables and request a channel select, verify that channel is switched to 11.
#
# This time the quality difference between 11 and 15 should be high enough for selection
# algorithm to pick the best though unfavored channel (i.e., channel 11).

node.set(wpan.WPAN_CHANNEL_MANAGER_NEW_CHANNEL, '25')  # request a channel change to 25
verify_channel([node], 25)
node.set(wpan.WPAN_CHANNEL_MANAGER_FAVORED_CHANNEL_MASK, str(chan_15_to_17_mask))
node.set(wpan.WPAN_CHANNEL_MANAGER_CHANNEL_SELECT, 'true')
verify(int(node.get(wpan.WPAN_CHANNEL_MANAGER_NEW_CHANNEL), 0) == 11)
verify_channel([node], 11)

# Set channels 12-15 as favorable and request a channel select, verify
# that channel is not switched.

node.set(wpan.WPAN_CHANNEL_MANAGER_FAVORED_CHANNEL_MASK, str(chan_12_to_15_mask))
node.set(wpan.WPAN_CHANNEL_MANAGER_CHANNEL_SELECT, 'true')
verify(int(node.get(wpan.WPAN_CHANNEL_MANAGER_NEW_CHANNEL), 0) == 11)
verify_channel([node], 11)

# Starting from channel 12 and issuing a channel select (which would pick 11 as best channel).
# However, since quality difference between current channel 12 and new best channel 11 is not large
# enough, no action should be taken.

node.set(wpan.WPAN_CHANNEL_MANAGER_NEW_CHANNEL, '12')  # request a channel change to 12
verify(int(node.get(wpan.WPAN_CHANNEL_MANAGER_NEW_CHANNEL), 0) == 12)
verify_channel([node], 12)
node.set(wpan.WPAN_CHANNEL_MANAGER_FAVORED_CHANNEL_MASK, str(all_channels_mask))
node.set(wpan.WPAN_CHANNEL_MANAGER_CHANNEL_SELECT, 'true')
verify(int(node.get(wpan.WPAN_CHANNEL_MANAGER_NEW_CHANNEL), 0) == 12)
verify_channel([node], 12)

# -----------------------------------------------------------------------------------------------------------------------
# Test finished

wpan.Node.finalize_all_nodes()

print('\'{}\' passed.'.format(test_name))
