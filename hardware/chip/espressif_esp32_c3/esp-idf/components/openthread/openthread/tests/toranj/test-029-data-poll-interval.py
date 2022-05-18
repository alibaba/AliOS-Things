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

import time
import wpan
from wpan import verify

# -----------------------------------------------------------------------------------------------------------------------
# Test description: Verify transmission of data polls and poll interval change.

test_name = __file__[:-3] if __file__.endswith('.py') else __file__
print('-' * 120)
print('Starting \'{}\''.format(test_name))

# -----------------------------------------------------------------------------------------------------------------------
# Creating `wpan.Nodes` instances

speedup = 4
wpan.Node.set_time_speedup_factor(speedup)

parent = wpan.Node()
child = wpan.Node()

# -----------------------------------------------------------------------------------------------------------------------
# Init all nodes

wpan.Node.init_all_nodes()

# -----------------------------------------------------------------------------------------------------------------------
# Build network topology
#

parent.form("poll-interval")

child.join_node(parent, wpan.JOIN_TYPE_SLEEPY_END_DEVICE)

# -----------------------------------------------------------------------------------------------------------------------
# Test implementation

# Verify the default poll interval is smaller than child_timeout

child_timeout = int(child.get(wpan.WPAN_THREAD_CHILD_TIMEOUT), 0) * 1000
default_poll_interval = int(child.get(wpan.WPAN_POLL_INTERVAL), 0)
verify(0 < default_poll_interval <= child_timeout)

WAIT_TIME = 0.36  # in seconds

# Check number of data polls with different poll intervals

for poll_interval in [100, 200, 500, 50]:  # in milliseconds

    poll_count_before = int(child.get(wpan.WPAN_NCP_COUNTER_TX_PKT_DATA_POLL), 0)

    child.set(wpan.WPAN_POLL_INTERVAL, str(poll_interval))
    verify(int(child.get(wpan.WPAN_POLL_INTERVAL), 0) == poll_interval)

    time.sleep(WAIT_TIME)
    poll_count_after = int(child.get(wpan.WPAN_NCP_COUNTER_TX_PKT_DATA_POLL), 0)
    actual_polls = poll_count_after - poll_count_before

    expected_polls = WAIT_TIME * 1000 * speedup / poll_interval

    print("poll interval {} ms, polls -> actual {}, expected {}".format(poll_interval, actual_polls, expected_polls))

    verify(actual_polls >= int(expected_polls))

# Verify behavior when poll interval is switched from long to short.
#
#  - Poll interval set to 5 seconds
#  - Wait for 200 ms
#  - Change poll interval to 200 ms
#  - This should immediately trigger a poll tx since 100 ms is
#    already passed since last poll transmission.

child.set(wpan.WPAN_POLL_INTERVAL, '5000')
WIAT_TIME = 0.2  # in seconds
time.sleep(WAIT_TIME / speedup)
poll_count_before = int(child.get(wpan.WPAN_NCP_COUNTER_TX_PKT_DATA_POLL), 0)
child.set(wpan.WPAN_POLL_INTERVAL, str(WAIT_TIME * 1000))
time.sleep(0.01)
poll_count_after = int(child.get(wpan.WPAN_NCP_COUNTER_TX_PKT_DATA_POLL), 0)
verify(poll_count_after > poll_count_before)

# Verify behavior when poll interval is set to zero.
# Poll interval should use default interval again (based on child timeout).

child.set(wpan.WPAN_POLL_INTERVAL, '0')
verify(int(child.get(wpan.WPAN_POLL_INTERVAL), 0) == default_poll_interval)

# Change "child timeout" and verify that default interval does change

child.set(wpan.WPAN_THREAD_CHILD_TIMEOUT, str(child_timeout / 1000 * 2))
new_default_interval = int(child.get(wpan.WPAN_POLL_INTERVAL), 0)
verify(default_poll_interval < new_default_interval <= child_timeout * 2)

child.set(wpan.WPAN_THREAD_CHILD_TIMEOUT, str(child_timeout / 1000))
verify(int(child.get(wpan.WPAN_POLL_INTERVAL), 0) == default_poll_interval)

# -----------------------------------------------------------------------------------------------------------------------
# Test finished

wpan.Node.finalize_all_nodes()

print('\'{}\' passed.'.format(test_name))
