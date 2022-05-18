#!/usr/bin/env python3
#
#  Copyright (c) 2020, The OpenThread Authors.
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
# Test description: Test MLE discover scan with nodes supporting different radios
#

test_name = __file__[:-3] if __file__.endswith('.py') else __file__
print('-' * 120)
print('Starting \'{}\''.format(test_name))

# -----------------------------------------------------------------------------------------------------------------------
# Creating `wpan.Nodes` instances

speedup = 1
wpan.Node.set_time_speedup_factor(speedup)

n1 = wpan.Node(wpan.NODE_15_4)
n2 = wpan.Node(wpan.NODE_TREL)
n3 = wpan.Node(wpan.NODE_15_4_TREL)
s1 = wpan.Node(wpan.NODE_15_4)
s2 = wpan.Node(wpan.NODE_TREL)
s3 = wpan.Node(wpan.NODE_15_4_TREL)

# -----------------------------------------------------------------------------------------------------------------------
# Init all nodes

wpan.Node.init_all_nodes()

# -----------------------------------------------------------------------------------------------------------------------
# Build network topology

n1.form("n1", channel='20')
n2.form("n2", channel='21')
n3.form("n3", channel='22')

# -----------------------------------------------------------------------------------------------------------------------
# Test implementation

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Scan by scanner nodes (no network)

# Scan by s1 (15.4 only), expect to see n1(15.4) and n3(15.4+trel)
result = wpan.parse_scan_result(s1.discover_scan())
verify(n1.is_in_scan_result(result))
verify(not n2.is_in_scan_result(result))
verify(n3.is_in_scan_result(result))

# Scan by s2 (trel only), expect to see n2(trel) and n3(15.4+trel)
result = wpan.parse_scan_result(s2.discover_scan())
verify(not n1.is_in_scan_result(result))
verify(n2.is_in_scan_result(result))
verify(n3.is_in_scan_result(result))

# Scan by s3 (trel+15.4), expect to see all nodes
result = wpan.parse_scan_result(s3.discover_scan())
verify(n1.is_in_scan_result(result))
verify(n2.is_in_scan_result(result))
verify(n3.is_in_scan_result(result))

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Scan by the nodes

# Scan by n1 (15.4 only), expect to see only n3(15.4+trel)
result = wpan.parse_scan_result(n1.discover_scan())
verify(not n1.is_in_scan_result(result))
verify(not n2.is_in_scan_result(result))
verify(n3.is_in_scan_result(result))

# Scan by n2 (trel only), expect to see only n3(15.4+trel)
result = wpan.parse_scan_result(n2.discover_scan())
verify(not n1.is_in_scan_result(result))
verify(not n2.is_in_scan_result(result))
verify(n3.is_in_scan_result(result))

# Scan by n3 (15.4+trel), expect to see n1(15.4) and n2(trel)
result = wpan.parse_scan_result(n3.discover_scan())
verify(n1.is_in_scan_result(result))
verify(n2.is_in_scan_result(result))
verify(not n3.is_in_scan_result(result))

# -----------------------------------------------------------------------------------------------------------------------
# Test finished

wpan.Node.finalize_all_nodes()

print('\'{}\' passed.'.format(test_name))
