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
import time

# -----------------------------------------------------------------------------------------------------------------------
# Test description: Orphaned node attach through MLE Announcement

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

router = wpan.Node()
c1 = wpan.Node()
c2 = wpan.Node()

all_nodes = [router, c1, c2]

# -----------------------------------------------------------------------------------------------------------------------
# Init all nodes

wpan.Node.init_all_nodes()

# -----------------------------------------------------------------------------------------------------------------------
# Build network topology

router.form('announce-tst', channel=11)

c1.join_node(router, node_type=wpan.JOIN_TYPE_SLEEPY_END_DEVICE)
c2.join_node(router, node_type=wpan.JOIN_TYPE_SLEEPY_END_DEVICE)

c1.set(wpan.WPAN_POLL_INTERVAL, '500')
c2.set(wpan.WPAN_POLL_INTERVAL, '500')

c1.set(wpan.WPAN_THREAD_DEVICE_MODE, '5')
c2.set(wpan.WPAN_THREAD_DEVICE_MODE, '5')

# -----------------------------------------------------------------------------------------------------------------------
# Test implementation

# Reset c2 and keep it in detached state
c2.set('Daemon:AutoAssociateAfterReset', 'false')
c2.reset()

# Switch the rest of network to channel 26
router.set(wpan.WPAN_CHANNEL_MANAGER_NEW_CHANNEL, '26')
verify_channel([router, c1], 26)

# Now re-enable c2 and verify that it does attach to router and is on channel 26
# c2 would go through the ML Announce recovery.

c2.set('Daemon:AutoAssociateAfterReset', 'true')
c2.reset()
verify(int(c2.get(wpan.WPAN_CHANNEL), 0) == 11)

# wait for 20s for c2 to be attached/associated


def check_c2_is_associated():
    verify(c2.is_associated())


wpan.verify_within(check_c2_is_associated, 20)

# Check that c2 is now on channel 26.
verify(int(c2.get(wpan.WPAN_CHANNEL), 0) == 26)

# -----------------------------------------------------------------------------------------------------------------------
# Test finished

wpan.Node.finalize_all_nodes()

print('\'{}\' passed.'.format(test_name))
