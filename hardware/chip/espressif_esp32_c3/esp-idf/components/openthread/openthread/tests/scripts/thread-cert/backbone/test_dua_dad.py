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
# This test verifies that PBBR sets DUA routes correctly.
#
import ipaddress
import re
import unittest

import config
import thread_cert
from pktverify.packet_verifier import PacketVerifier

# Use two channels
CH1 = 11
CH2 = 22

PBBR = 1
SBBR = 2
ROUTER1 = 3
HOST = 4
PBBR2 = 5
ROUTER2 = 6

REREG_DELAY = 5  # Seconds
MLR_TIMEOUT = 300  # Seconds
WAIT_REDUNDANCE = 3

DUPLICATE_DUA = 'fd00:7d03:7d03:7d03:11:2233:4455:6677'
assert (re.match(config.DOMAIN_PREFIX_REGEX_PATTERN, DUPLICATE_DUA))

DUPLICATE_IID = ''.join('%02x' % c for c in ipaddress.IPv6Address(DUPLICATE_DUA).packed[-8:])

print('DUPLICATE_DUA: ', DUPLICATE_DUA)
print('DUPLICATE_IID: ', DUPLICATE_IID)


class TestDuaDad(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False

    # Topology:
    #    ------(eth)----------------------
    #       |        |       |      |
    #      PBBR----SBBR    HOST   PBBR2
    #        \ CH1 /               | CH2
    #        ROUTER1             ROUTER2
    #
    # PBBR2 is in the secondary channel
    #
    TOPOLOGY = {
        PBBR: {
            'name': 'PBBR',
            'allowlist': [SBBR, ROUTER1],
            'is_otbr': True,
            'version': '1.2',
            'channel': CH1,
            'prefer_router_id': 0x1,  # Use prefer_router_id to avoid Router ID conflicts in the two channels.
        },
        SBBR: {
            'name': 'SBBR',
            'allowlist': [PBBR, ROUTER1],
            'is_otbr': True,
            'version': '1.2',
            'channel': CH1,
            'prefer_router_id': 0x2,
        },
        ROUTER1: {
            'name': 'ROUTER1',
            'allowlist': [PBBR, SBBR],
            'version': '1.2',
            'channel': CH1,
            'prefer_router_id': 0x3,
        },
        HOST: {
            'name': 'HOST',
            'is_host': True
        },
        PBBR2: {
            'name': 'PBBR2',
            'is_otbr': True,
            'version': '1.2',
            'channel': CH2,
            'prefer_router_id': 0x4,
        },
        ROUTER2: {
            'name': 'ROUTER2',
            'version': '1.2',
            'channel': CH2,
            'prefer_router_id': 0x5,
        },
    }

    def _bootstrap(self):
        # Bring up HOST
        self.nodes[HOST].start()

        # Bring up PBBR
        self.nodes[PBBR].start()
        self.simulator.go(5)
        self.assertEqual('leader', self.nodes[PBBR].get_state())
        self.wait_node_state(PBBR, 'leader', 5)

        self.nodes[PBBR].set_backbone_router(reg_delay=REREG_DELAY, mlr_timeout=MLR_TIMEOUT)
        self.nodes[PBBR].enable_backbone_router()
        self.nodes[PBBR].set_domain_prefix(config.DOMAIN_PREFIX, 'prosD')
        self.simulator.go(5)
        self.assertTrue(self.nodes[PBBR].is_primary_backbone_router)
        self.assertIsNotNone(self.nodes[PBBR].get_ip6_address(config.ADDRESS_TYPE.DUA))

        # Bring up SBBR
        self.nodes[SBBR].start()
        self.simulator.go(5)
        self.assertEqual('router', self.nodes[SBBR].get_state())

        self.nodes[SBBR].set_backbone_router(reg_delay=REREG_DELAY, mlr_timeout=MLR_TIMEOUT)
        self.nodes[SBBR].enable_backbone_router()
        self.simulator.go(5)
        self.assertFalse(self.nodes[SBBR].is_primary_backbone_router)
        self.assertIsNotNone(self.nodes[SBBR].get_ip6_address(config.ADDRESS_TYPE.DUA))

        # Bring up ROUTER1
        self.nodes[ROUTER1].start()
        self.simulator.go(5)
        self.assertEqual('router', self.nodes[ROUTER1].get_state())
        self.simulator.go(config.DUA_DAD_DELAY + WAIT_REDUNDANCE)
        self.assertIsNotNone(self.nodes[ROUTER1].get_ip6_address(config.ADDRESS_TYPE.DUA))

        # Bring up PBBR2
        self.nodes[PBBR2].start()
        self.simulator.go(5)
        self.assertEqual('leader', self.nodes[PBBR2].get_state())
        self.wait_node_state(PBBR2, 'leader', 5)

        self.nodes[PBBR2].set_backbone_router(reg_delay=REREG_DELAY, mlr_timeout=MLR_TIMEOUT)
        self.nodes[PBBR2].enable_backbone_router()
        self.nodes[PBBR2].set_domain_prefix(config.DOMAIN_PREFIX, 'prosD')
        self.simulator.go(config.DUA_DAD_DELAY)
        self.assertTrue(self.nodes[PBBR2].is_primary_backbone_router)
        self.assertIsNotNone(self.nodes[PBBR2].get_ip6_address(config.ADDRESS_TYPE.DUA))

        # Bring up ROUTER2
        self.nodes[ROUTER2].start()
        self.simulator.go(5)
        self.assertEqual('router', self.nodes[ROUTER2].get_state())
        self.simulator.go(config.DUA_DAD_DELAY + WAIT_REDUNDANCE)
        self.assertIsNotNone(self.nodes[ROUTER2].get_ip6_address(config.ADDRESS_TYPE.DUA))

    def test(self):
        self._bootstrap()

        self.collect_ipaddrs()
        self.collect_rloc16s()
        self.collect_rlocs()

        self._test_extend_backbone_query()

        self._test_dad_duplicate()

        self._send_fake_pro_bb_ntf()

    def _test_extend_backbone_query(self):
        self.nodes[ROUTER2].ping(self.nodes[ROUTER1].get_ip6_address(config.ADDRESS_TYPE.DUA))

    def _test_dad_duplicate(self):
        self.nodes[ROUTER2].set_dua_iid(DUPLICATE_IID)
        self.simulator.go(config.DUA_DAD_DELAY + WAIT_REDUNDANCE)
        self.nodes[ROUTER1].set_dua_iid(DUPLICATE_IID)
        self.simulator.go(config.DUA_DAD_DELAY + WAIT_REDUNDANCE)

        # now Router1 should have generated new DUA
        self.assertNotEqual(DUPLICATE_DUA, self.nodes[ROUTER1].get_ip6_address(config.ADDRESS_TYPE.DUA))

    def _send_fake_pro_bb_ntf(self):
        router1_dua = self.nodes[ROUTER1].get_ip6_address(config.ADDRESS_TYPE.DUA)
        self.nodes[PBBR2].send_proactive_backbone_notification(router1_dua, DUPLICATE_IID, 0)
        self.simulator.go(config.DUA_DAD_DELAY + WAIT_REDUNDANCE)

    def verify(self, pv: PacketVerifier):
        pkts = pv.pkts
        pv.add_common_vars()
        pv.summary.show()

        PBBR = pv.vars['PBBR']
        ROUTER1 = pv.vars['ROUTER1']
        ROUTER1_DUA = pv.vars['ROUTER1_DUA']
        ROUTER2 = pv.vars['ROUTER2']
        ROUTER2_DUA = pv.vars['ROUTER2_DUA']
        PBBR_ETH = pv.vars['PBBR_ETH']
        SBBR_ETH = pv.vars['SBBR_ETH']
        PBBR2 = pv.vars['PBBR2']
        PBBR2_ETH = pv.vars['PBBR2_ETH']

        MM = pv.vars['MM_PORT']
        BB = pv.vars['BB_PORT']

        # Verify the whole DAD process for ROUTER1
        pv.verify_dua_registration(ROUTER1, ROUTER1_DUA, pbbr_eth=PBBR_ETH, pbbr_src64=PBBR, sbbr_eth=SBBR_ETH)

        # Verify the whole DAD process for ROUTER2
        pv.verify_dua_registration(ROUTER2, ROUTER2_DUA, pbbr_eth=PBBR2_ETH, pbbr_src64=PBBR2)

        ###############################################################################################################
        # Now we verify Extending ADDR.qry to BB.qry works for the Ping Request from Router2 to Router1
        ###############################################################################################################

        # Router2 should send ADDR.qry in the Thread network for Router1's DUA
        pkts.filter_wpan_src64(ROUTER2).filter_coap_request('/a/aq', port=MM).filter(
            'thread_address.tlv.target_eid == {ROUTER1_DUA}', ROUTER1_DUA=ROUTER1_DUA).must_next()
        # PBBR2 should extend ADDR.qry to BB.qry
        pkts.filter_backbone_query(ROUTER1_DUA, eth_src=PBBR2_ETH, port=BB).must_next()
        # SBBR should not answer with BB.ans
        pkts.filter_backbone_query(ROUTER1_DUA, eth_src=SBBR_ETH, port=BB).must_not_next()
        # PBBR1 should answer with BB.ans
        pkts.filter_backbone_answer(ROUTER1_DUA, eth_src=PBBR_ETH, port=BB).must_next()
        # PBBR2 should send ADDR.ntf to Router2
        pkts.filter_wpan_src64(PBBR2).filter_coap_request('/a/an', port=MM).filter(
            'thread_address.tlv.target_eid == {ROUTER1_DUA}', ROUTER1_DUA=ROUTER1_DUA).must_next()

        # Now, Router2 should send the Ping Request to PBBR2
        pkts.filter_wpan_src64(ROUTER2).filter_ping_request().filter_ipv6_dst(ROUTER1_DUA).must_next()

        ###############################################################################################################
        # Now we start to verify that DAD duplicate is handled correctly
        ###############################################################################################################

        # PBBR should send /b/bq for DUPLICATE_DUA
        pkts.filter_backbone_query(DUPLICATE_DUA, eth_src=PBBR_ETH, port=BB).must_next()

        # PBBR2 should send /b/ba for DUPLICATE_DUA
        ba = pkts.filter_backbone_answer(DUPLICATE_DUA, eth_src=PBBR2_ETH, port=BB).must_next()
        ba.must_verify("""
            set(thread_bl.tlv.type) == {tlvs}
            and thread_bl.tlv.last_transaction_time > 0
            and thread_meshcop.tlv.net_name == [{NET_NAME}]
        """,
                       tlvs={0, 3, 6, 12},
                       **pv.vars)

        # PBBR should NOT send /b/bq for DUPLICATE_DUA anymore
        pkts.filter_backbone_query(DUPLICATE_DUA, eth_src=PBBR_ETH, port=BB).must_not_next()

        # PBBR should send ADDR_ERR.ntf to Router1
        ROUTER1_RLOC = pv.vars['ROUTER1_RLOC']
        pkts.filter_wpan_src64(PBBR).filter_ipv6_dst(ROUTER1_RLOC).filter_coap_request('/a/ae', port=MM).must_next() \
            .must_verify("""
                thread_address.tlv.target_eid == {DUPLICATE_DUA}
                and thread_address.tlv.ml_eid == {ml_eid} 
            """, DUPLICATE_DUA=DUPLICATE_DUA, ml_eid=ba.thread_bl.tlv.ml_eid)

        # Router1 should generate new DUA and register again
        # Router1 should generate and register a new DUA
        with pkts.save_index():
            new_dr = pkts.filter_wpan_src64(ROUTER1).filter_coap_request('/n/dr', port=MM).must_next().must_verify(
                """
                thread_nm.tlv.target_eid != {DUPLICATE_DUA}
            """,
                DUPLICATE_DUA=DUPLICATE_DUA)
            router1_dua2 = new_dr.thread_nm.tlv.target_eid

        # Verify the DAD process for the new DUA
        pv.verify_dua_registration(ROUTER1, router1_dua2, pbbr_eth=PBBR_ETH, pbbr_src64=PBBR, sbbr_eth=SBBR_ETH)

        ###############################################################################################################
        # Now we start to verify that PRO_BB.ntf is handled correctly
        ###############################################################################################################

        # Scripted: PBBR2 should send PRO_BB.ntf for Router1's DUA
        pkts.filter_eth_src(PBBR2_ETH).filter_backbone_answer(router1_dua2,
                                                              port=BB,
                                                              confirmable=False,
                                                              mliid=DUPLICATE_IID).must_next()

        # PBBR should broadcaset /a/ae to the Thread network
        pkts.filter_wpan_src64(PBBR).filter_coap_request('/a/ae', port=MM,
                                                         confirmable=False).filter_RLARMA().must_next()

        # Router1 should generate and register a new DUA
        with pkts.save_index():
            new_dr = pkts.filter_wpan_src64(ROUTER1).filter_coap_request('/n/dr', port=MM).must_next()
            router1_dua3 = new_dr.thread_nm.tlv.target_eid

        # Verify the DAD process for the new DUA
        pv.verify_dua_registration(ROUTER1, router1_dua3, pbbr_eth=PBBR_ETH, pbbr_src64=PBBR, sbbr_eth=SBBR_ETH)


if __name__ == '__main__':
    unittest.main()
