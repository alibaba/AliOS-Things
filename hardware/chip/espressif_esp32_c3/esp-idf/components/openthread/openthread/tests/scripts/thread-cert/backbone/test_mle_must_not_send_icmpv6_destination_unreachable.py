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
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS'
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
#
# This test verifies that no ICMPv6 message is sent for MLE.
#
import unittest

import thread_cert
from pktverify.consts import ICMPV6_TYPE_DESTINATION_UNREACHABLE
from pktverify.packet_verifier import PacketVerifier

PBBR = 1
ROUTER = 2


class TestMleMustNotSendIcmpv6DestinationUnreachable(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False

    # Topology:
    #
    #    ------(eth)----------
    #           |
    #         PBBR---ROUTER
    #
    TOPOLOGY = {
        PBBR: {
            'name': 'PBBR',
            'allowlist': [ROUTER],
            'is_otbr': True,
            'version': '1.2',
        },
        ROUTER: {
            'name': 'ROUTER',
            'allowlist': [PBBR],
            'version': '1.2',
        },
    }

    def test(self):
        self.nodes[PBBR].start()
        self.simulator.go(5)
        self.assertEqual('leader', self.nodes[PBBR].get_state())
        self.nodes[PBBR].enable_backbone_router()
        self.simulator.go(3)
        self.assertTrue(self.nodes[PBBR].is_primary_backbone_router)

        self.nodes[ROUTER].start()
        self.simulator.go(5)
        self.assertEqual('router', self.nodes[ROUTER].get_state())

        self.simulator.go(5)

        self.collect_ipaddrs()

    def verify(self, pv: PacketVerifier):
        pkts = pv.pkts
        pv.add_common_vars()
        pv.summary.show()

        with pkts.save_index():
            pv.verify_attached('ROUTER')

        PBBR = pv.vars['PBBR']
        ROUTER = pv.vars['ROUTER']

        # PBBR MUST NOT send ICMPv6 Destination Unreachable
        pkts.filter_wpan_src64(PBBR).filter_wpan_dst64(ROUTER).filter(
            f'icmpv6.type == {ICMPV6_TYPE_DESTINATION_UNREACHABLE}').must_not_next()


if __name__ == '__main__':
    unittest.main()
