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

import copy
import unittest

import config
import thread_cert
from pktverify.consts import MLE_CHILD_ID_RESPONSE, MGMT_ED_SCAN, MGMT_ED_REPORT, NM_CHANNEL_MASK_TLV, NM_ENERGY_LIST_TLV
from pktverify.packet_verifier import PacketVerifier

COMMISSIONER = 1
LEADER = 2
ROUTER1 = 3
ED = 4


class Cert_9_2_13_EnergyScan_Base(thread_cert.TestCase):
    SUPPORT_NCP = False

    TOPOLOGY = {
        COMMISSIONER: {
            'name': 'COMMISSIONER',
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        LEADER: {
            'name': 'LEADER',
            'mode': 'rdn',
            'allowlist': [COMMISSIONER, ROUTER1]
        },
        ROUTER1: {
            'name': 'ROUTER',
            'mode': 'rdn',
            'allowlist': [LEADER, ED]
        },
        ED: {
            'allowlist': [ROUTER1]
        },
    }

    def test(self):
        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        self.nodes[COMMISSIONER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[COMMISSIONER].get_state(), 'router')
        self.nodes[COMMISSIONER].commissioner_start()
        self.simulator.go(5)

        self.nodes[ROUTER1].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER1].get_state(), 'router')

        self.nodes[ED].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ED].get_state(), 'child')
        self.collect_rlocs()

        if self.TOPOLOGY[ED]['name'] == 'DUT':
            ipaddrs = self.nodes[ED].get_addrs()
        else:
            ipaddrs = self.nodes[ROUTER1].get_addrs()

        for ipaddr in ipaddrs:
            if ipaddr[0:4] != 'fe80':
                break

        self.nodes[COMMISSIONER].energy_scan(0x50000, 0x02, 0x20, 0xc8, ipaddr)
        self.simulator.go(3)
        self.nodes[COMMISSIONER].energy_scan(0x50000, 0x02, 0x20, 0xc8, 'ff33:0040:fd00:db8:0:0:0:1')
        self.simulator.go(3)

        self.assertTrue(self.nodes[COMMISSIONER].ping(ipaddr))

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        DUT = pv.vars['DUT']
        DUT_RLOC = pv.vars['DUT_RLOC']
        COMMISSIONER_RLOC = pv.vars['COMMISSIONER_RLOC']
        _pkts = pkts.filter_wpan_src64(DUT)

        # Step 3: The DUT MUST send MGMT_ED_REPORT.ans to the Commissioner and report energy measurements
        _pkts.filter_ipv6_dst(COMMISSIONER_RLOC).filter_coap_request(MGMT_ED_REPORT).must_next().must_verify(
            lambda p: {NM_CHANNEL_MASK_TLV, NM_ENERGY_LIST_TLV} == set(p.thread_meshcop.tlv.type) and p.thread_meshcop.
            tlv.chan_mask_mask == '0000a000' and len(p.thread_meshcop.tlv.energy_list) == 2)

        # Step 5: The DUT MUST send MGMT_ED_REPORT.ans to the Commissioner and report energy measurements
        _pkts.filter_ipv6_dst(COMMISSIONER_RLOC).filter_coap_request(MGMT_ED_REPORT).must_next().must_verify(
            lambda p: {NM_CHANNEL_MASK_TLV, NM_ENERGY_LIST_TLV} == set(p.thread_meshcop.tlv.type) and p.thread_meshcop.
            tlv.chan_mask_mask == '0000a000' and len(p.thread_meshcop.tlv.energy_list) == 2)

        # Step 6: The DUT MUST respond with ICMPv6 Echo Reply
        _pkts.filter_ping_reply().filter_ipv6_src_dst(DUT_RLOC, COMMISSIONER_RLOC).must_next()


class Cert_9_2_13_EnergyScan_FED(Cert_9_2_13_EnergyScan_Base):
    TOPOLOGY = copy.deepcopy(Cert_9_2_13_EnergyScan_Base.TOPOLOGY)
    TOPOLOGY[ROUTER1]['name'] = 'ROUTER'
    TOPOLOGY[ED]['name'] = 'DUT'
    TOPOLOGY[ED]['router_upgrade_threshold'] = 0


class Cert_9_2_13_EnergyScan_MED(Cert_9_2_13_EnergyScan_Base):
    TOPOLOGY = copy.deepcopy(Cert_9_2_13_EnergyScan_Base.TOPOLOGY)
    TOPOLOGY[ROUTER1]['name'] = 'ROUTER'
    TOPOLOGY[ED]['name'] = 'DUT'
    TOPOLOGY[ED]['mode'] = 'rn'
    TOPOLOGY[ED]['is_mtd'] = True


class Cert_9_2_13_EnergyScan_ROUTER(Cert_9_2_13_EnergyScan_Base):
    TOPOLOGY = copy.deepcopy(Cert_9_2_13_EnergyScan_Base.TOPOLOGY)
    TOPOLOGY[ROUTER1]['name'] = 'DUT'
    TOPOLOGY[ED]['name'] = 'ED'
    TOPOLOGY[ED]['mode'] = 'rn'
    TOPOLOGY[ED]['is_mtd'] = True


class Cert_9_2_13_EnergyScan_SED(Cert_9_2_13_EnergyScan_Base):
    TOPOLOGY = copy.deepcopy(Cert_9_2_13_EnergyScan_Base.TOPOLOGY)
    TOPOLOGY[ROUTER1]['name'] = 'ROUTER'
    TOPOLOGY[ED]['name'] = 'DUT'
    TOPOLOGY[ED]['mode'] = '-'
    TOPOLOGY[ED]['is_mtd'] = True
    TOPOLOGY[ED]['timeout'] = config.DEFAULT_CHILD_TIMEOUT


del (Cert_9_2_13_EnergyScan_Base)

if __name__ == '__main__':
    unittest.main()
