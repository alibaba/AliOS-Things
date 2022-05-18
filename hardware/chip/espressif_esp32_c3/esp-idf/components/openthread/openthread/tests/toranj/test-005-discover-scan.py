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
# Test description: discover scan

test_name = __file__[:-3] if __file__.endswith('.py') else __file__
print('-' * 120)
print('Starting \'{}\''.format(test_name))

# -----------------------------------------------------------------------------------------------------------------------
# Creating `wpan.Nodes` instances

speedup = 2
wpan.Node.set_time_speedup_factor(speedup)

node1 = wpan.Node()
node2 = wpan.Node()
node3 = wpan.Node()
node4 = wpan.Node()
node5 = wpan.Node()

scanner = wpan.Node()

# -----------------------------------------------------------------------------------------------------------------------
# Init all nodes

wpan.Node.init_all_nodes()

# -----------------------------------------------------------------------------------------------------------------------
# Build network topology

node1.form("net1", channel='11', panid='0x0001')
node2.form("net2", channel='12', panid='0x0002')
node3.form("net3", channel='13', panid='0x0002')
node4.form("net4", channel='13', panid='0x0004')
node5.form("net5", channel='14', panid='0x0005')

# -----------------------------------------------------------------------------------------------------------------------
# Test implementation

# Perform active scan and check that all nodes are seen in the scan result.

scan_result = wpan.parse_scan_result(scanner.discover_scan())

verify(len(scan_result) == 5)
for node in [node1, node2, node3, node4, node5]:
    verify(node.is_in_scan_result(scan_result))

# Scan from an already associated node.

scan_result = wpan.parse_scan_result(node1.discover_scan())

verify(len(scan_result) == 4)
for node in [node2, node3, node4, node5]:
    verify(node.is_in_scan_result(scan_result))

# Scan on specific subset of channels

scan_result = wpan.parse_scan_result(scanner.discover_scan(channel="11-13"))

verify(len(scan_result) == 4)
for node in [node1, node2, node3, node4]:
    verify(node.is_in_scan_result(scan_result))

# Filter on specific PAN ID.

scan_result = wpan.parse_scan_result(scanner.discover_scan(panid_filter="0x0002"))

verify(len(scan_result) == 2)
for node in [node2, node3]:
    verify(node.is_in_scan_result(scan_result))

# Scan joinable only.

scanner_hw_addr = scanner.get(wpan.WPAN_HW_ADDRESS)[1:-1]  # Remove the `[]`

node1.commissioner_start()
node1.commissioner_add_joiner(scanner_hw_addr, '123456')
node2.commissioner_start()
node2.commissioner_add_joiner('1122334455667788', '123456')

scan_result = wpan.parse_scan_result(scanner.discover_scan(joiner_only=True))

verify(len(scan_result) == 2)
for node in [node1, node2]:
    verify(node.is_in_scan_result(scan_result))

# Scan with filter enabled

scan_result = wpan.parse_scan_result(scanner.discover_scan(enable_filtering=True))

verify(len(scan_result) == 1)
verify(node1.is_in_scan_result(scan_result))

# -----------------------------------------------------------------------------------------------------------------------
# Test finished

wpan.Node.finalize_all_nodes()

print('\'{}\' passed.'.format(test_name))
