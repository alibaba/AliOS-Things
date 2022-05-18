#!/usr/bin/env python3
#
#  Copyright (c) 2016, The OpenThread Authors.
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
#

import unittest
import copy

import config
import thread_cert
from pktverify.consts import MLE_PARENT_REQUEST
from pktverify.packet_verifier import PacketVerifier

LEADER = 1
ROUTER1 = 2
MTD = 3

# Test Purpose and Description:
# -----------------------------
# The purpose of this test case is to show that the DUT upholds connectivity, or
# reattaches with its parent, when the Leader is removed and the Router creates a
# new partition
#
# Test Topology:
# -------------
#  Leader
#    |
#  Router
#    |
#   DUT
#
# DUT Types:
# ----------
#  ED
#  SED


class Cert_6_2_1_NewPartition_Base(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False

    TOPOLOGY = {
        LEADER: {
            'name': 'LEADER',
            'mode': 'rdn',
            'allowlist': [ROUTER1]
        },
        ROUTER1: {
            'name': 'ROUTER',
            'mode': 'rdn',
            'allowlist': [LEADER, MTD]
        },
        MTD: {
            'name': 'DUT',
            'is_mtd': True,
            'timeout': config.DEFAULT_CHILD_TIMEOUT,
            'allowlist': [ROUTER1]
        },
    }

    def test(self):
        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        self.nodes[ROUTER1].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER1].get_state(), 'router')

        self.nodes[MTD].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[MTD].get_state(), 'child')

        self.nodes[LEADER].stop()
        self.simulator.go(140)
        self.assertEqual(self.nodes[ROUTER1].get_state(), 'leader')
        self.assertEqual(self.nodes[MTD].get_state(), 'child')

        self.collect_ipaddrs()

        dut_addr = self.nodes[MTD].get_ip6_address(config.ADDRESS_TYPE.LINK_LOCAL)
        self.assertTrue(self.nodes[ROUTER1].ping(dut_addr))

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        ROUTER = pv.vars['ROUTER']
        ROUTER_LLA = pv.vars['ROUTER_LLA']
        DUT = pv.vars['DUT']
        DUT_LLA = pv.vars['DUT_LLA']

        # Step 1: Ensure topology is formed correctly
        pkts.filter_wpan_src64(LEADER).\
            filter_mle_advertisement('Leader').\
            must_next()

        pv.verify_attached('ROUTER', 'LEADER')
        pv.verify_attached('DUT', 'ROUTER', 'MTD')

        # Step 3: Router automatically creates new partition and begins transmitting
        #         MLE Advertisements
        pkts.filter_wpan_src64(ROUTER).\
            filter_LLARMA().\
            filter_mle_cmd(MLE_PARENT_REQUEST).\
            must_next()
        pkts.filter_wpan_src64(ROUTER).\
            filter_mle_advertisement('Router').\
            must_next()

        # Step 5: Router verifies connectivity by sending an ICMPv6 Echo Request
        #         to the DUT link local address
        #         DUT responds with ICMPv6 Echo Reply
        _pkt = pkts.filter_ping_request().\
            filter_ipv6_src_dst(ROUTER_LLA, DUT_LLA).\
            must_next()
        pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            filter_ipv6_src_dst(DUT_LLA, ROUTER_LLA).\
            must_next()


class Cert_6_2_1_NewPartition_ED(Cert_6_2_1_NewPartition_Base):
    TOPOLOGY = copy.deepcopy(Cert_6_2_1_NewPartition_Base.TOPOLOGY)
    TOPOLOGY[MTD]['mode'] = 'rn'


class Cert_6_2_1_NewPartition_SED(Cert_6_2_1_NewPartition_Base):
    TOPOLOGY = copy.deepcopy(Cert_6_2_1_NewPartition_Base.TOPOLOGY)
    TOPOLOGY[MTD]['mode'] = '-'


del (Cert_6_2_1_NewPartition_Base)

if __name__ == '__main__':
    unittest.main()
