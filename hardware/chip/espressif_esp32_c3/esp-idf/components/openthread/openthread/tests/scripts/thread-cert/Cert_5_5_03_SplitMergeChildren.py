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

import thread_cert
import config
from pktverify.consts import MLE_ADVERTISEMENT, MLE_PARENT_REQUEST, MLE_PARENT_RESPONSE, MLE_CHILD_UPDATE_RESPONSE, MLE_CHILD_ID_REQUEST, ADDR_SOL_URI, SOURCE_ADDRESS_TLV, MODE_TLV, TIMEOUT_TLV, CHALLENGE_TLV, RESPONSE_TLV, LINK_LAYER_FRAME_COUNTER_TLV, ROUTE64_TLV, ADDRESS16_TLV, LEADER_DATA_TLV, NETWORK_DATA_TLV, TLV_REQUEST_TLV, SCAN_MASK_TLV, VERSION_TLV, ADDRESS_REGISTRATION_TLV, ACTIVE_TIMESTAMP_TLV, NL_PARENT_PARTITION_CHANGE
from pktverify.packet_verifier import PacketVerifier
from pktverify.null_field import nullField

LEADER = 1
ROUTER1 = 2
ROUTER2 = 3
ED2 = 4
ED3 = 5

MTDS = [ED2, ED3]


class Cert_5_5_3_SplitMergeChildren(thread_cert.TestCase):
    SUPPORT_NCP = False

    TOPOLOGY = {
        LEADER: {
            'name': 'LEADER',
            'mode': 'rdn',
            'allowlist': [ROUTER1, ROUTER2]
        },
        ROUTER1: {
            'name': 'ROUTER_1',
            'mode': 'rdn',
            'allowlist': [LEADER, ED2]
        },
        ROUTER2: {
            'name': 'ROUTER_2',
            'mode': 'rdn',
            'allowlist': [LEADER, ED3]
        },
        ED2: {
            'name': 'MED_2',
            'is_mtd': True,
            'mode': 'rn',
            'allowlist': [ROUTER1]
        },
        ED3: {
            'name': 'MED_3',
            'is_mtd': True,
            'mode': 'rn',
            'allowlist': [ROUTER2]
        },
    }

    def _setUpLeader(self):
        self.nodes[LEADER].add_allowlist(self.nodes[ROUTER1].get_addr64())
        self.nodes[LEADER].add_allowlist(self.nodes[ROUTER2].get_addr64())
        self.nodes[LEADER].enable_allowlist()
        self.nodes[LEADER].set_router_selection_jitter(1)

    def test(self):
        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        self.nodes[ROUTER1].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER1].get_state(), 'router')

        self.nodes[ROUTER2].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER1].get_state(), 'router')

        self.nodes[ED2].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ED2].get_state(), 'child')

        self.nodes[ED3].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ED2].get_state(), 'child')

        self.nodes[LEADER].reset()
        self._setUpLeader()
        self.nodes[ROUTER2].set_preferred_partition_id(0xffffffff)

        self.simulator.go(140)

        self.assertEqual(self.nodes[ROUTER1].get_state(), 'leader')
        self.assertEqual(self.nodes[ROUTER2].get_state(), 'leader')

        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'router')

        self.simulator.go(30)
        self.assertEqual(self.nodes[ROUTER1].get_state(), 'router')
        self.assertEqual(self.nodes[ROUTER2].get_state(), 'leader')

        self.collect_rloc16s()

        addrs = self.nodes[ED3].get_addrs()
        for addr in addrs:
            if addr[0:4] != 'fe80':
                self.assertTrue(self.nodes[ED2].ping(addr))

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        LEADER_RLOC16 = pv.vars['LEADER_RLOC16']
        ROUTER_1 = pv.vars['ROUTER_1']
        ROUTER_2 = pv.vars['ROUTER_2']
        MED_2 = pv.vars['MED_2']
        MED_3 = pv.vars['MED_3']
        _lpkts = pkts.filter_wpan_src64(LEADER)
        _router1_pkts = pkts.filter_wpan_src64(ROUTER_1)

        # Step 2: The Leader and Router_1 MUST send properly formatted MLE Advertisements
        pkts.filter_wpan_src64(LEADER).filter_LLANMA().filter_mle_cmd(MLE_ADVERTISEMENT).must_next().must_verify(
            lambda p: {SOURCE_ADDRESS_TLV, LEADER_DATA_TLV, ROUTE64_TLV} == set(p.mle.tlv.type))
        _pkt = pkts.filter_wpan_src64(ROUTER_1).filter_LLANMA().filter_mle_cmd(MLE_ADVERTISEMENT).must_next()
        _pkt.must_verify(
            lambda p: {SOURCE_ADDRESS_TLV, LEADER_DATA_TLV, ROUTE64_TLV} == set(p.mle.tlv.type) and p.ipv6.hlim == 255)

        # Step 4: Router_1 MUST attempt to reattach to its original partition by
        # sending MLE Parent Requests to the All-Routers multicast address
        _router1_pkts.range(pkts.index).filter_LLARMA().filter_mle_cmd(MLE_PARENT_REQUEST).must_next().must_verify(
            lambda p: {MODE_TLV, CHALLENGE_TLV, SCAN_MASK_TLV, VERSION_TLV} == set(
                p.mle.tlv.type) and p.mle.tlv.scan_mask.r == 1 and p.mle.tlv.scan_mask.e == 1 and p.ipv6.hlim == 255)
        lreset_start = _router1_pkts.index

        # Step 6: Router_1 MUST attempt to attach to any other Partition
        # within range by sending a MLE Parent Request.
        _router1_pkts.filter_LLARMA().filter_mle_cmd(MLE_PARENT_REQUEST).filter(
            lambda p: p.mle.tlv.scan_mask.r == 1 and p.mle.tlv.scan_mask.e == 0).must_next().must_verify(
                lambda p: {MODE_TLV, CHALLENGE_TLV, SCAN_MASK_TLV, VERSION_TLV} == set(p.mle.tlv.type
                                                                                      ) and p.ipv6.hlim == 255)
        lreset_stop = _router1_pkts.index

        # Step 3: The Leader MUST stop sending MLE advertisements.
        _lpkts.range(lreset_start, lreset_stop).filter_LLARMA().filter_mle_cmd(MLE_ADVERTISEMENT).must_not_next()

        # Step 5: Leader MUST NOT respond to the MLE Parent Requests
        _lpkts.range(lreset_start,
                     lreset_stop).filter_wpan_src64(LEADER).filter_mle_cmd(MLE_PARENT_RESPONSE).must_not_next()

        # Step 7: Router_1 takes over leader role of a new Partition and
        # begin transmitting MLE Advertisements
        with _router1_pkts.save_index():
            _router1_pkts.filter_LLANMA().filter_mle_cmd(MLE_ADVERTISEMENT).filter(
                lambda p: {SOURCE_ADDRESS_TLV, LEADER_DATA_TLV, ROUTE64_TLV} == set(p.mle.tlv.type) and p.mle.tlv.
                leader_data.partition_id != _pkt.mle.tlv.leader_data.partition_id and p.mle.tlv.leader_data.
                data_version != _pkt.mle.tlv.leader_data.data_version and p.mle.tlv.leader_data.stable_data_version !=
                _pkt.mle.tlv.leader_data.stable_data_version and p.ipv6.hlim == 255).must_next()

        # Step 9: Router_1 MUST respond with an MLE Child Update Response,
        # with the updated TLVs of the new partition
        _router1_pkts.filter_wpan_dst64(MED_2).filter_mle_cmd(MLE_CHILD_UPDATE_RESPONSE).must_next().must_verify(
            lambda p: {SOURCE_ADDRESS_TLV, MODE_TLV, LEADER_DATA_TLV, ADDRESS_REGISTRATION_TLV} < set(p.mle.tlv.type))

        # Step 10: The Leader MUST send properly formatted MLE Parent
        # Requests to the All-Routers multicast address
        _lpkts.filter_LLARMA().filter_mle_cmd(MLE_PARENT_REQUEST).must_next().must_verify(
            lambda p: {MODE_TLV, CHALLENGE_TLV, SCAN_MASK_TLV, VERSION_TLV} == set(p.mle.tlv.type
                                                                                  ) and p.ipv6.hlim == 255)

        # Step 11: Leader send MLE Child ID Request to Router_2
        _lpkts.filter_wpan_dst64(ROUTER_2).filter_mle_cmd(MLE_CHILD_ID_REQUEST).must_next().must_verify(
            lambda p: {
                RESPONSE_TLV, LINK_LAYER_FRAME_COUNTER_TLV, MODE_TLV, TIMEOUT_TLV, VERSION_TLV, TLV_REQUEST_TLV,
                ADDRESS16_TLV, NETWORK_DATA_TLV, ROUTE64_TLV, ACTIVE_TIMESTAMP_TLV
            } < set(p.mle.tlv.type))

        # Step 12: Leader send MLE ADVERTISEMENT
        _lpkts.filter_mle_cmd(MLE_ADVERTISEMENT).must_next().must_verify(
            lambda p: {SOURCE_ADDRESS_TLV, LEADER_DATA_TLV, ROUTE64_TLV} == set(p.mle.tlv.type) and p.ipv6.hlim == 255)

        # Step 13: Router_1 send an Address Solicit Request
        _router1_pkts.filter_coap_request(ADDR_SOL_URI).must_next().must_verify(
            lambda p: p.wpan.dst16 == LEADER_RLOC16 and p.thread_address.tlv.ext_mac_addr is not nullField and p.
            thread_address.tlv.status == NL_PARENT_PARTITION_CHANGE)

        # Step 14: MED_2 MUST receive an ICMPv6 Echo Reply from MED_3
        p = pkts.filter_ping_request().filter_wpan_src64(MED_2).must_next()
        pkts.filter_ping_reply(identifier=p.icmpv6.echo.identifier).filter_wpan_src64(MED_3).must_next()


if __name__ == '__main__':
    unittest.main()
