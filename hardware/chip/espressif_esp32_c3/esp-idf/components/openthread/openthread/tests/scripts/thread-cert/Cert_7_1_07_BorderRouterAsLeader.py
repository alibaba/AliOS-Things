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
#

import unittest
import copy

import config
import thread_cert
from pktverify.consts import WIRESHARK_OVERRIDE_PREFS, MLE_CHILD_UPDATE_REQUEST, MLE_DATA_RESPONSE, MLE_CHILD_ID_RESPONSE, SVR_DATA_URI, ACTIVE_TIMESTAMP_TLV, RESPONSE_TLV, LINK_LAYER_FRAME_COUNTER_TLV, VERSION_TLV, TLV_REQUEST_TLV, ADDRESS16_TLV, NETWORK_DATA_TLV, ROUTE64_TLV, CHALLENGE_TLV, SOURCE_ADDRESS_TLV, LEADER_DATA_TLV, ADDRESS_REGISTRATION_TLV, NWD_BORDER_ROUTER_TLV, NWD_6LOWPAN_ID_TLV
from pktverify.packet_verifier import PacketVerifier
from pktverify.bytes import Bytes
from pktverify.addrs import Ipv6Addr
from pktverify.null_field import nullField
from pktverify.utils import is_sublist

LEADER = 1
ROUTER_1 = 2
ROUTER_2 = 3
MED = 4
SED = 5

MTDS = [MED, SED]
PREFIX_1 = '2001:0db8:0001::/64'
PREFIX_2 = '2001:0db8:0002::/64'

# Test Purpose and Description:
# -----------------------------
# The purpose of this test case is to verify that network data is properly updated
# when a server from the network leaves and rejoins.
# Router_1 is configured as Border Router for prefix 2001:db8:1::/64.
# Router_2 is configured as Border Router for prefix 2001:db8:1::/64.
# MED is configured to require complete network data.
# SED is configured to request only stable network data.
#
# Test Topology:
# -------------
#             SED
#              |
# Router_1 - Leader(DUT) - MED
#              |
#            Router_2
#
# DUT Types:
# ----------
# Leader


class Cert_7_1_7_BorderRouterAsLeader(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False
    SUPPORT_NCP = False

    TOPOLOGY = {
        LEADER: {
            'name': 'LEADER',
            'mode': 'rdn',
            'allowlist': [ROUTER_1, ROUTER_2, MED, SED]
        },
        ROUTER_1: {
            'name': 'ROUTER_1',
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        ROUTER_2: {
            'name': 'ROUTER_2',
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        MED: {
            'name': 'MED',
            'is_mtd': True,
            'mode': 'rn',
            'timeout': config.DEFAULT_CHILD_TIMEOUT,
            'allowlist': [LEADER]
        },
        SED: {
            'name': 'SED',
            'is_mtd': True,
            'mode': '-',
            'timeout': config.DEFAULT_CHILD_TIMEOUT,
            'allowlist': [LEADER]
        },
    }
    # override wireshark preferences with case needed parameters
    CASE_WIRESHARK_PREFS = copy.deepcopy(WIRESHARK_OVERRIDE_PREFS)
    CASE_WIRESHARK_PREFS['6lowpan.context1'] = PREFIX_1
    CASE_WIRESHARK_PREFS['6lowpan.context2'] = PREFIX_2

    def test(self):
        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        for i in (2, 3):
            self.nodes[i].start()
            self.simulator.go(5)
            self.assertEqual(self.nodes[i].get_state(), 'router')

        self.nodes[MED].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[MED].get_state(), 'child')

        self.nodes[SED].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[SED].get_state(), 'child')

        self.collect_rlocs()

        self.nodes[ROUTER_1].add_prefix(PREFIX_1, 'paosr')
        self.nodes[ROUTER_1].register_netdata()
        self.simulator.go(5)
        self.nodes[ROUTER_2].add_prefix(PREFIX_1, 'paro')
        self.nodes[ROUTER_2].register_netdata()
        self.simulator.go(5)

        self.nodes[ROUTER_2].set_preferred_partition_id(1)
        self.nodes[ROUTER_2].set_network_id_timeout(50)

        self.nodes[ROUTER_2].remove_allowlist(self.nodes[LEADER].get_addr64())
        self.nodes[LEADER].remove_allowlist(self.nodes[ROUTER_2].get_addr64())

        # Wait for NETWORK_ID_TIMEOUT taking effect
        # Router_2 creates a new partition
        self.simulator.go(60)
        self.assertEqual(self.nodes[ROUTER_2].get_state(), 'leader')
        self.nodes[ROUTER_2].remove_domain_prefix(PREFIX_1)
        self.nodes[ROUTER_2].add_prefix(PREFIX_2, 'paros')
        self.nodes[ROUTER_2].register_netdata()

        # Router_2 reattaches to Leader
        self.nodes[ROUTER_2].add_allowlist(self.nodes[LEADER].get_addr64())
        self.nodes[LEADER].add_allowlist(self.nodes[ROUTER_2].get_addr64())

        # Wait for Router_2 reattachment and network data propagation
        # ADVERTISEMENT_I_MAX + DEFAULT_CHILD_TIMEOUT + ATTACH_DELAY + Extra
        self.simulator.go(60)
        self.assertEqual(self.nodes[ROUTER_2].get_state(), 'router')
        self.collect_ipaddrs()
        self.collect_rloc16s()

        # ping Leader's PREFIX_1 and PREFIX_2 addrs
        dut_addrs = []
        dut_addrs.append(self.nodes[LEADER].get_addr(PREFIX_1))
        dut_addrs.append(self.nodes[LEADER].get_addr(PREFIX_2))
        for addr in dut_addrs:
            self.assertTrue(self.nodes[ROUTER_1].ping(addr))
            self.simulator.go(1)
            self.assertTrue(self.nodes[SED].ping(addr))
            self.simulator.go(1)

        self.nodes[ROUTER_2].remove_domain_prefix(PREFIX_2)
        self.nodes[ROUTER_2].register_netdata()
        self.simulator.go(5)

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        LEADER_RLOC = pv.vars['LEADER_RLOC']
        LEADER_RLOC16 = pv.vars['LEADER_RLOC16']
        ROUTER_1 = pv.vars['ROUTER_1']
        ROUTER_1_RLOC16 = pv.vars['ROUTER_1_RLOC16']
        ROUTER_1_RLOC = pv.vars['ROUTER_1_RLOC']
        ROUTER_2 = pv.vars['ROUTER_2']
        ROUTER_2_RLOC16 = pv.vars['ROUTER_2_RLOC16']
        ROUTER_2_RLOC = pv.vars['ROUTER_2_RLOC']
        SED = pv.vars['SED']
        MED = pv.vars['MED']
        GUA = [{}, {}]
        PREFIXES = [Bytes(PREFIX_1[:-5]), Bytes(PREFIX_2[:-5])]

        for i in (0, 1):
            for node in ('LEADER', 'ROUTER_1', 'SED'):
                for addr in pv.vars['%s_IPADDRS' % node]:
                    if addr.startswith(PREFIXES[i]):
                        GUA[i][node] = addr

        # Step 1: Ensure topology is formed correctly
        pv.verify_attached('ROUTER_1', 'LEADER')
        pv.verify_attached('ROUTER_2', 'LEADER')
        pv.verify_attached('MED', 'LEADER', 'MTD')
        pv.verify_attached('SED', 'LEADER', 'MTD')
        _pkt = pkts.last()

        # Step 2,3: Router_1 and Router_2 MUST send a CoAP Server Data
        #           Notification frame to the Leader including the server’s
        #           information(Prefix, Border Router):
        #             CoAP Request URI
        #                 coap://[<Leader address>]:MM/a/sd
        #             CoAP Payload
        #                 Thread Network Data TLV

        # Step 4: Leader sends a CoAP ACK frame to each of Router_1 and
        #         Router_2
        with pkts.save_index():
            for node in ('ROUTER_1', 'ROUTER_2'):
                _dn_pkt = pkts.filter_wpan_src64(pv.vars['%s' %node]).\
                    filter_wpan_dst16(LEADER_RLOC16).\
                    filter_coap_request(SVR_DATA_URI).\
                    filter(lambda p:
                           [Ipv6Addr(PREFIX_1[:-3])] ==
                           p.thread_nwd.tlv.prefix and\
                           [pv.vars['%s_RLOC16' %node]] ==
                           p.thread_nwd.tlv.border_router_16
                           ).\
                    must_next()
                pkts.filter_wpan_src64(LEADER).\
                    filter_ipv6_dst(pv.vars['%s_RLOC' %node]).\
                    filter_coap_ack(SVR_DATA_URI).\
                    filter(lambda p: p.coap.mid == _dn_pkt.coap.mid).\
                    must_next()

        # Step 5: Leader MUST multicast MLE Data Response with the new
        #         information collected from Router_1 and Router_2,
        #         including the following TLVs:,
        #              - Source Address TLV
        #              - Leader Data TLV
        #                  - Data Version field <incremented>
        #                  - Stable Data Version field <incremented>
        #              - Network Data TLV
        #                  - Stable Flag set
        #                  - At least one Prefix TLV (Prefix 1)
        #                      - Two Border Router sub-TLVs
        #                          Border Router1 TLV: Stable Flag set
        #                          Border Router2 TLV: Stable Flag not set
        #                      - 6LoWPAN ID sub-TLV
        #                          Stable Flag set
        _dr_pkt = pkts.filter_wpan_src64(LEADER).\
            filter_LLANMA().\
            filter_mle_cmd(MLE_DATA_RESPONSE).\
            filter(lambda p: {
                              NETWORK_DATA_TLV,
                              SOURCE_ADDRESS_TLV,
                              LEADER_DATA_TLV
                             } <= set(p.mle.tlv.type) and\
                   [Ipv6Addr(PREFIX_1[:-3])] ==
                   p.thread_nwd.tlv.prefix and\
                   p.mle.tlv.leader_data.data_version ==
                   (_pkt.mle.tlv.leader_data.data_version + 1) % 256 and\
                   p.mle.tlv.leader_data.stable_data_version ==
                   (_pkt.mle.tlv.leader_data.stable_data_version + 1) % 256
                   ).\
            must_next()
        with pkts.save_index():
            _dr_pkt1 = pkts.filter_wpan_src64(LEADER).\
                filter_LLANMA().\
                filter_mle_cmd(MLE_DATA_RESPONSE).\
                filter(lambda p: {
                                  NETWORK_DATA_TLV,
                                  SOURCE_ADDRESS_TLV,
                                  LEADER_DATA_TLV
                                 } <= set(p.mle.tlv.type) and\
                                 {
                                  NWD_BORDER_ROUTER_TLV,
                                  NWD_6LOWPAN_ID_TLV
                                 } <= set(p.thread_nwd.tlv.type) and\
                       p.mle.tlv.leader_data.data_version ==
                       (_dr_pkt.mle.tlv.leader_data.data_version + 1) % 256 and\
                       p.mle.tlv.leader_data.stable_data_version ==
                       _dr_pkt.mle.tlv.leader_data.stable_data_version and\
                       is_sublist([ROUTER_1_RLOC16, ROUTER_2_RLOC16],
                               p.thread_nwd.tlv.border_router_16) and\
                       is_sublist([0, 1, 1, 1, 0], p.thread_nwd.tlv.stable) and\
                       is_sublist([1], getattr(p.thread_nwd.tlv, '6co').flag.c) and\
                       is_sublist([Ipv6Addr(PREFIX_1[:-3])], p.thread_nwd.tlv.prefix)
                       ).\
                must_next()

        # Step 6: Leader MUST send a MLE Child Update Request or MLE Data
        #         Response to SED, including the following TLVs:
        #             - Network Data TLV
        #                 At least one Prefix TLV (Prefix 1) including:
        #                     - Stable Flag set
        #                     - Border Router sub-TLV(corresponding to Router_1)
        #                         - P_border_router_16<0xFFFE>
        #                         - Stable Flag set
        #                     - 6LoWPAN ID sub-TLV
        #                         - Stable Flag set
        #             - Source Address TLV
        #             - Leader Data TLV
        #             - Active Timestamp TLV
        pkts.filter_wpan_src64(LEADER).\
            filter_wpan_dst64(SED).\
            filter_mle_cmd2(MLE_CHILD_UPDATE_REQUEST, MLE_DATA_RESPONSE).\
            filter(lambda p: {
                              NETWORK_DATA_TLV,
                              SOURCE_ADDRESS_TLV,
                              LEADER_DATA_TLV,
                              ACTIVE_TIMESTAMP_TLV
                             } == set(p.mle.tlv.type) and\
                   is_sublist([Ipv6Addr(PREFIX_1[:-3])], p.thread_nwd.tlv.prefix) and\
                   is_sublist([1, 1, 1], p.thread_nwd.tlv.stable) and\
                   is_sublist([1], getattr(p.thread_nwd.tlv, '6co').flag.c) and\
                   is_sublist([0xFFFE], p.thread_nwd.tlv.border_router_16)
                   ).\
            must_next()

        # Step 10: Router_2 automatically reattaches to the Leader and sends
        #          a CoAP Server Data Notification message with the server’s
        #          information (Prefix, Border Router) to the Leader:
        #              CoAP Request URI
        #                  coap://[<leader address>]:MM/a/sd
        #              CoAP Payload
        #                  Thread Network Data TLV

        # Step 11: Leader sends a CoAP ACK frame to each of Routers
        pv.verify_attached('ROUTER_2', 'LEADER')
        with pkts.save_index():
            _dn_pkt = pkts.filter_wpan_src64(ROUTER_2).\
                  filter_wpan_dst16(LEADER_RLOC16).\
                  filter_coap_request(SVR_DATA_URI).\
                  filter(lambda p:
                         [Ipv6Addr(PREFIX_2[:-3])] ==
                         p.thread_nwd.tlv.prefix and\
                         [ROUTER_2_RLOC16] ==
                         p.thread_nwd.tlv.border_router_16
                         ).\
                  must_next()

            pkts.filter_wpan_src64(LEADER).\
                filter_ipv6_dst(ROUTER_2_RLOC).\
                filter_coap_ack(SVR_DATA_URI).\
                filter(lambda p: p.coap.mid == _dn_pkt.coap.mid).\
                must_next()

        # Step 12: Leader MUST multicast MLE Data Response with the new
        #          information collected from Router_2,
        #          including the following TLVs:,
        #              - Source Address TLV
        #              - Leader Data TLV
        #                  - Data Version field <incremented>
        #                  - Stable Data Version field <incremented>
        #              - Network Data TLV
        #                  - Stable Flag set
        #                  - At least two Prefix TLVs (Prefix 1 and Prefix 2)
        #                      - Prefix 1 TLV
        #                          - Stable Flag set
        #                          - Only one Border Router sub-TLV
        #                            corresponding to Router_1
        #                          - 6LoWPAN ID sub-TLV
        #                                - Stable Flag set
        #                      - Prefix 2 TLV
        #                          - Stable Flag set
        #                          - Only one Border Router sub-TLV
        #                            corresponding to Router_2
        #                          - 6LoWPAN ID sub-TLV
        _dr_pkt2 = pkts.filter_wpan_src64(LEADER).\
            filter_LLANMA().\
            filter_mle_cmd(MLE_DATA_RESPONSE).\
            filter(lambda p: {
                              NETWORK_DATA_TLV,
                              SOURCE_ADDRESS_TLV,
                              LEADER_DATA_TLV
                             } <= set(p.mle.tlv.type) and\
                             {
                              NWD_BORDER_ROUTER_TLV,
                              NWD_6LOWPAN_ID_TLV
                             } <= set(p.thread_nwd.tlv.type) and\
                   is_sublist([ROUTER_1_RLOC16, ROUTER_2_RLOC16],
                           p.thread_nwd.tlv.border_router_16) and\
                   is_sublist([0, 1, 1, 1, 1, 1, 1],
                           p.thread_nwd.tlv.stable) and\
                   is_sublist([1, 1], getattr(p.thread_nwd.tlv, '6co').flag.c) and\
                   is_sublist([Ipv6Addr(PREFIX_1[:-3]), Ipv6Addr(PREFIX_2[:-3])],
                           p.thread_nwd.tlv.prefix) and\
                   p.mle.tlv.leader_data.data_version ==
                   (_dr_pkt1.mle.tlv.leader_data.data_version + 1) % 256 and\
                   p.mle.tlv.leader_data.stable_data_version ==
                   (_dr_pkt1.mle.tlv.leader_data.stable_data_version + 1) % 256
                   ).\
            must_next()

        # Step 13: Leader MUST send a MLE Child Update Request or MLE Data
        #          Response to SED, containing the stable Network Data
        #          including the following TLVs:
        #              - Source Address TLV
        #              - Leader Data TLV
        #              - Network Data TLV
        #                  - At least two Prefix TLVs (Prefix 1 and Prefix 2)
        #                      - Prefix 1 TLV
        #                          - Stable Flag set
        #                          - Border Router sub-TLV
        #                            corresponding to Router_1
        #                                - P_border_router_16 <0xFFFE>
        #                                - Stable flag set
        #                          - 6LoWPAN ID sub-TLV
        #                                - Stable flag set
        #                      - Prefix 2 TLV
        #                          - Stable Flag set
        #                          - Border Router sub-TLV
        #                            corresponding to Router_2
        #                                - P_border_router_16 <0xFFFE>
        #                                - Stable flag set
        #                          - 6LoWPAN ID sub-TLV
        #                                - Stable flag set
        #              - Active Timestamp TLV
        with pkts.save_index():
            pkts.filter_wpan_src64(LEADER).\
                filter_wpan_dst64(SED).\
                filter_mle_cmd2(MLE_CHILD_UPDATE_REQUEST, MLE_DATA_RESPONSE).\
                filter(lambda p: {
                                  NETWORK_DATA_TLV,
                                  SOURCE_ADDRESS_TLV,
                                  LEADER_DATA_TLV,
                                  ACTIVE_TIMESTAMP_TLV
                                 } == set(p.mle.tlv.type) and\
                       is_sublist([1, 1, 1, 1, 1, 1],
                               p.thread_nwd.tlv.stable) and\
                       is_sublist([1, 1], getattr(p.thread_nwd.tlv, '6co').flag.c) and\
                       is_sublist([Ipv6Addr(PREFIX_1[:-3]), Ipv6Addr(PREFIX_2[:-3])],
                               p.thread_nwd.tlv.prefix) and\
                       is_sublist([0xFFFE, 0xFFFE], p.thread_nwd.tlv.border_router_16)
                       ).\
                must_next()

        # Step 14: Verifies connectivity by sending ICMPv6 Echo Requests from
        #          Router_1 and SED_1 to the Leader Prefix_1 and Prefix_2-based
        #          address.
        #          Leader must respond with ICMPv6 Echo Replies
        for i in (0, 1):
            for node in ('ROUTER_1', 'SED'):
                _pkt = pkts.filter_ping_request().\
                    filter_ipv6_src_dst(GUA[i][node], GUA[i]['LEADER']).\
                    must_next()
                pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
                    filter_ipv6_src_dst(GUA[i]['LEADER'], GUA[i][node]).\
                    must_next()

        # Step 15: Router_2 sends a CoAP Server Data Notification message with
        #          the server’s information (Prefix, Border Router) to the Leader:
        #              CoAP Request URI
        #                  coap://[<leader address>]:MM/a/sd
        #              CoAP Payload
        #                  empty payload

        # Step 16: Leader sends a CoAP ACK frame to each of Router_1 and
        #          Router_2
        with pkts.save_index():
            _dn_pkt = pkts.filter_wpan_src64(ROUTER_2).\
                filter_wpan_dst16(LEADER_RLOC16).\
                filter_coap_request(SVR_DATA_URI).\
                filter(lambda p:
                       p.thread_nwd.tlv.border_router_16 is nullField
                       ).\
                must_next()

            pkts.filter_wpan_src64(LEADER).\
                filter_ipv6_dst(ROUTER_2_RLOC).\
                filter_coap_ack(SVR_DATA_URI).\
                filter(lambda p: p.coap.mid == _dn_pkt.coap.mid).\
                must_next()

        # Step 17: Leader MUST multicast MLE Data Response with the new
        #          information collected from Router_2,
        #          including the following TLVs:,
        #              - Source Address TLV
        #              - Leader Data TLV
        #                  - Data Version field <incremented>
        #                  - Stable Data Version field <incremented>
        #              - Network Data TLV
        #                  - Stable Flag set
        #                  - At least two Prefix TLVs (Prefix 1 and Prefix 2)
        #                      - Prefix 1 TLV
        #                          - Stable Flag set
        #                          - Only one Border Router sub-TLV
        #                            corresponding to Router_1
        #                          - 6LoWPAN ID sub-TLV
        #                                - Stable Flag set
        #                      - Prefix 2 TLV
        #                          - Stable Flag set
        #                          - 6LoWPAN ID sub-TLV
        #                                - Stable Flag set
        #                                - compression flag set to 0
        _pkt = pkts.filter_wpan_src64(LEADER).\
            filter_LLANMA().\
            filter_mle_cmd(MLE_DATA_RESPONSE).\
            filter(lambda p: {
                              NETWORK_DATA_TLV,
                              SOURCE_ADDRESS_TLV,
                              LEADER_DATA_TLV
                             } <= set(p.mle.tlv.type) and\
                             {
                              NWD_BORDER_ROUTER_TLV,
                              NWD_6LOWPAN_ID_TLV
                             } <= set(p.thread_nwd.tlv.type) and\
                   p.mle.tlv.leader_data.data_version ==
                   (_dr_pkt2.mle.tlv.leader_data.data_version + 1) % 256 and\
                   p.mle.tlv.leader_data.stable_data_version ==
                   (_dr_pkt2.mle.tlv.leader_data.stable_data_version + 1) % 256 and\
                   is_sublist([Ipv6Addr(PREFIX_1[:-3]), Ipv6Addr(PREFIX_2[:-3])],
                           p.thread_nwd.tlv.prefix) and\
                   is_sublist([1,0], getattr(p.thread_nwd.tlv, '6co').flag.c)
                   ).\
            must_next()

        # Step 18: Leader MUST send a MLE Child Update Request or MLE Data
        #          Response to SED, containing the stable Network Data
        #          including the following TLVs:
        #              - Source Address TLV
        #              - Leader Data TLV
        #              - Network Data TLV
        #                  - At least two Prefix TLVs (Prefix 1 and Prefix 2)
        #                      - Prefix 1 TLV
        #                          - Stable Flag set
        #                          - Border Router sub-TLV
        #                            corresponding to Router_1
        #                                - P_border_router_16 <0xFFFE>
        #                                - Stable flag set
        #                          - 6LoWPAN ID sub-TLV
        #                                - Stable flag set
        #                      - Prefix 2 TLV
        #                          - Stable Flag set
        #                          - 6LoWPAN ID sub-TLV
        #                                - Stable flag set
        #                                - compression flag set to 0
        #              - Active Timestamp TLV
        pkts.filter_wpan_src64(LEADER).\
            filter_wpan_dst64(SED).\
            filter_mle_cmd2(MLE_CHILD_UPDATE_REQUEST, MLE_DATA_RESPONSE).\
            filter(lambda p: {
                              NETWORK_DATA_TLV,
                              SOURCE_ADDRESS_TLV,
                              LEADER_DATA_TLV,
                              ACTIVE_TIMESTAMP_TLV
                             } == set(p.mle.tlv.type) and\
                   is_sublist([Ipv6Addr(PREFIX_1[:-3]), Ipv6Addr(PREFIX_2[:-3])],
                           p.thread_nwd.tlv.prefix) and\
                   is_sublist([1, 1, 1, 1, 1], p.thread_nwd.tlv.stable) and\
                   is_sublist([0xFFFE], p.thread_nwd.tlv.border_router_16) and\
                   is_sublist([1,0], getattr(p.thread_nwd.tlv, '6co').flag.c)
                   ).\
            must_next()


if __name__ == '__main__':
    unittest.main()
