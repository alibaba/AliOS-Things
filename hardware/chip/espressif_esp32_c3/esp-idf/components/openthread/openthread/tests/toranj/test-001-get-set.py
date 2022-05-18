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
# Test description: simple wpanctl get and set commands

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
# Test implementation

verify(node.get(wpan.WPAN_STATE) == wpan.STATE_OFFLINE)

# set some of properties and check and verify that the value is indeed
# changed...

node.set(wpan.WPAN_NAME, 'test-network')
verify(node.get(wpan.WPAN_NAME) == '"test-network"')

node.set(wpan.WPAN_NAME, 'a')
verify(node.get(wpan.WPAN_NAME) == '"a"')

node.set(wpan.WPAN_PANID, '0xABBA')
verify(node.get(wpan.WPAN_PANID) == '0xABBA')

node.set(wpan.WPAN_XPANID, '1020031510006016', binary_data=True)
verify(node.get(wpan.WPAN_XPANID) == '0x1020031510006016')

node.set(wpan.WPAN_KEY, '0123456789abcdeffecdba9876543210', binary_data=True)
verify(node.get(wpan.WPAN_KEY) == '[0123456789ABCDEFFECDBA9876543210]')

node.set(wpan.WPAN_MAC_ALLOWLIST_ENABLED, '1')
verify(node.get(wpan.WPAN_MAC_ALLOWLIST_ENABLED) == 'true')

node.set(wpan.WPAN_MAC_ALLOWLIST_ENABLED, '0')
verify(node.get(wpan.WPAN_MAC_ALLOWLIST_ENABLED) == 'false')

node.set(wpan.WPAN_MAC_ALLOWLIST_ENABLED, 'true')
verify(node.get(wpan.WPAN_MAC_ALLOWLIST_ENABLED) == 'true')

node.set(wpan.WPAN_THREAD_ROUTER_UPGRADE_THRESHOLD, '100')
verify(int(node.get(wpan.WPAN_THREAD_ROUTER_UPGRADE_THRESHOLD), 0) == 100)

node.set(wpan.WPAN_THREAD_ROUTER_DOWNGRADE_THRESHOLD, '40')
verify(int(node.get(wpan.WPAN_THREAD_ROUTER_DOWNGRADE_THRESHOLD), 0) == 40)

# Ensure `get` is successful with all gettable properties

all_gettable_props = [
    wpan.WPAN_STATE,
    wpan.WPAN_NAME,
    wpan.WPAN_PANID,
    wpan.WPAN_XPANID,
    wpan.WPAN_KEY,
    wpan.WPAN_CHANNEL,
    wpan.WPAN_HW_ADDRESS,
    wpan.WPAN_EXT_ADDRESS,
    wpan.WPAN_POLL_INTERVAL,
    wpan.WPAN_NODE_TYPE,
    wpan.WPAN_ROLE,
    wpan.WPAN_PARTITION_ID,
    wpan.WPAN_NCP_VERSION,
    wpan.WPAN_NCP_MCU_POWER_STATE,
    wpan.WPAN_NETWORK_ALLOW_JOIN,
    wpan.WPAN_NETWORK_PASSTHRU_PORT,
    wpan.WPAN_IP6_LINK_LOCAL_ADDRESS,
    wpan.WPAN_IP6_MESH_LOCAL_ADDRESS,
    wpan.WPAN_IP6_MESH_LOCAL_PREFIX,
    wpan.WPAN_IP6_ALL_ADDRESSES,
    wpan.WPAN_IP6_MULTICAST_ADDRESSES,
    wpan.WPAN_THREAD_RLOC16,
    wpan.WPAN_THREAD_ROUTER_ID,
    wpan.WPAN_THREAD_NETWORK_DATA,
    wpan.WPAN_THREAD_CHILD_TABLE,
    wpan.WPAN_THREAD_CHILD_TABLE_ASVALMAP,
    wpan.WPAN_THREAD_CHILD_TABLE_ADDRESSES,
    wpan.WPAN_THREAD_NEIGHBOR_TABLE,
    wpan.WPAN_THREAD_NEIGHBOR_TABLE_ASVALMAP,
    wpan.WPAN_THREAD_NEIGHBOR_TABLE_ERR_RATES,
    wpan.WPAN_THREAD_ROUTER_TABLE,
    wpan.WPAN_THREAD_ROUTER_TABLE_ASVALMAP,
    wpan.WPAN_THREAD_CHILD_TIMEOUT,
    wpan.WPAN_THREAD_NETWORK_DATA_VERSION,
    wpan.WPAN_THREAD_STABLE_NETWORK_DATA,
    wpan.WPAN_THREAD_STABLE_NETWORK_DATA_VERSION,
    wpan.WPAN_THREAD_DEVICE_MODE,
    wpan.WPAN_THREAD_OFF_MESH_ROUTES,
    wpan.WPAN_THREAD_ON_MESH_PREFIXES,
    wpan.WPAN_THREAD_ROUTER_ROLE_ENABLED,
    wpan.WPAN_THREAD_CONFIG_FILTER_RLOC_ADDRESSES,
    wpan.WPAN_THREAD_ROUTER_UPGRADE_THRESHOLD,
    wpan.WPAN_THREAD_ROUTER_DOWNGRADE_THRESHOLD,
    wpan.WPAN_THREAD_ACTIVE_DATASET,
    wpan.WPAN_THREAD_ACTIVE_DATASET_ASVALMAP,
    wpan.WPAN_THREAD_PENDING_DATASET,
    wpan.WPAN_THREAD_PENDING_DATASET_ASVALMAP,
    wpan.WPAN_OT_LOG_LEVEL,
    wpan.WPAN_OT_STEERING_DATA_ADDRESS,
    wpan.WPAN_OT_STEERING_DATA_SET_WHEN_JOINABLE,
    wpan.WPAN_OT_MSG_BUFFER_COUNTERS,
    wpan.WPAN_OT_MSG_BUFFER_COUNTERS_AS_STRING,
    wpan.WPAN_NCP_COUNTER_ALL_MAC,
    wpan.WPAN_NCP_COUNTER_ALL_MAC_ASVALMAP,
    wpan.WPAN_MAC_ALLOWLIST_ENABLED,
    wpan.WPAN_MAC_ALLOWLIST_ENTRIES,
    wpan.WPAN_MAC_ALLOWLIST_ENTRIES_ASVALMAP,
    wpan.WPAN_MAC_DENYLIST_ENABLED,
    wpan.WPAN_MAC_DENYLIST_ENTRIES,
    wpan.WPAN_MAC_DENYLIST_ENTRIES_ASVALMAP,
    wpan.WPAN_JAM_DETECTION_STATUS,
    wpan.WPAN_JAM_DETECTION_ENABLE,
    wpan.WPAN_JAM_DETECTION_RSSI_THRESHOLD,
    wpan.WPAN_JAM_DETECTION_WINDOW,
    wpan.WPAN_JAM_DETECTION_BUSY_PERIOD,
    wpan.WPAN_JAM_DETECTION_DEBUG_HISTORY_BITMAP,
    wpan.WPAN_CHANNEL_MONITOR_SAMPLE_INTERVAL,
    wpan.WPAN_CHANNEL_MONITOR_RSSI_THRESHOLD,
    wpan.WPAN_CHANNEL_MONITOR_SAMPLE_WINDOW,
    wpan.WPAN_CHANNEL_MONITOR_SAMPLE_COUNT,
    wpan.WPAN_CHANNEL_MONITOR_CHANNEL_QUALITY,
    wpan.WPAN_CHANNEL_MONITOR_CHANNEL_QUALITY_ASVALMAP,
    wpan.WPAN_CHANNEL_MANAGER_NEW_CHANNEL,
    wpan.WPAN_CHANNEL_MANAGER_DELAY,
    wpan.WPAN_CHANNEL_MANAGER_CHANNEL_SELECT,
    wpan.WPAN_CHANNEL_MANAGER_AUTO_SELECT_ENABLED,
    wpan.WPAN_CHANNEL_MANAGER_AUTO_SELECT_INTERVAL,
    wpan.WPAN_CHANNEL_MANAGER_SUPPORTED_CHANNEL_MASK,
    wpan.WPAN_CHANNEL_MANAGER_FAVORED_CHANNEL_MASK,
    wpan.WPAN_THREAD_LEADER_ADDRESS,
    wpan.WPAN_THREAD_LEADER_ROUTER_ID,
    wpan.WPAN_THREAD_LEADER_WEIGHT,
    wpan.WPAN_THREAD_LEADER_LOCAL_WEIGHT,
    wpan.WPAN_THREAD_LEADER_NETWORK_DATA,
    wpan.WPAN_THREAD_STABLE_LEADER_NETWORK_DATA,
]

# note: Partition Id only takes effect after forming one Thread network.
node.set(wpan.WPAN_PARTITION_ID, '12345678')

node.form('get-set')

# verify that Partition Id property is indeed changed.
verify(int(node.get(wpan.WPAN_PARTITION_ID), 0) == 12345678)

for prop in all_gettable_props:
    node.get(prop)

# -----------------------------------------------------------------------------------------------------------------------
# Test finished

wpan.Node.finalize_all_nodes()

print('\'{}\' passed.'.format(test_name))
