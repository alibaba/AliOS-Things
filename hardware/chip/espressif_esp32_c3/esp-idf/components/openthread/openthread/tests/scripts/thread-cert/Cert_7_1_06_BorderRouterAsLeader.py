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

LEADER = 1
ROUTER_1 = 2
ROUTER_2 = 3
MED = 4
SED = 5

MTDS = [MED, SED]
PREFIX_2001 = '2001:0db8:0001::/64'

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


class Cert_7_1_6_BorderRouterAsLeader(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False

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
    CASE_WIRESHARK_PREFS = WIRESHARK_OVERRIDE_PREFS
    CASE_WIRESHARK_PREFS['6lowpan.context1'] = PREFIX_2001

    def _setUpRouter_1(self):
        self.nodes[ROUTER_1].add_allowlist(self.nodes[LEADER].get_addr64())
        self.nodes[ROUTER_1].enable_allowlist()
        self.nodes[ROUTER_1].set_router_selection_jitter(1)

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

        self.nodes[ROUTER_1].add_prefix(PREFIX_2001, 'paros')
        self.nodes[ROUTER_1].register_netdata()
        self.nodes[ROUTER_2].add_prefix(PREFIX_2001, 'paro')
        self.nodes[ROUTER_2].register_netdata()
        self.simulator.go(10)
        self.collect_ipaddrs()

        self.nodes[ROUTER_1].reset()
        self._setUpRouter_1()
        self.simulator.go(720)

        self.nodes[ROUTER_1].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER_1].get_state(), 'router')
        self.collect_rloc16s()

        self.nodes[ROUTER_1].add_prefix(PREFIX_2001, 'paros')
        self.nodes[ROUTER_1].register_netdata()
        self.simulator.go(10)

        dut_addr = self.nodes[LEADER].get_addr(PREFIX_2001)
        self.assertTrue(self.nodes[ROUTER_1].ping(dut_addr))
        self.simulator.go(1)
        self.assertTrue(self.nodes[SED].ping(dut_addr))

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
        SED = pv.vars['SED']
        MED = pv.vars['MED']
        GUA = {}

        for node in ('LEADER', 'ROUTER_1', 'SED'):
            for addr in pv.vars['%s_IPADDRS' % node]:
                if addr.startswith(Bytes(PREFIX_2001[:-5])):
                    GUA[node] = addr

        # Step 1: Ensure topology is formed correctly
        pv.verify_attached('ROUTER_1', 'LEADER')
        pv.verify_attached('ROUTER_2', 'LEADER')
        pv.verify_attached('MED', 'LEADER', 'MTD')
        pv.verify_attached('SED', 'LEADER', 'MTD')
        _pkt = pkts.last()

        # Step 2,3: Router_1 and Router_2  MUST send a CoAP Server Data
        #           Notification frame to the Leader including the server’s
        #           information(Prefix, Border Router):
        #             CoAP Request URI
        #                 coap://[<Leader address>]:MM/a/sd
        #             CoAP Payload
        #                 Thread Network Data TLV

        # Step 4: Leader sends a CoAP ACK frame to each of Router_1 and
        #         Router_2
        for i in (1, 2):
            with pkts.save_index():
                pkts.filter_wpan_src64(pv.vars['ROUTER_%d' %i]).\
                    filter_wpan_dst16(LEADER_RLOC16).\
                    filter_coap_request(SVR_DATA_URI).\
                    filter(lambda p:
                           [Ipv6Addr(PREFIX_2001[:-3])] ==
                           p.thread_nwd.tlv.prefix and\
                           [pv.vars['ROUTER_%d_RLOC16' %i]] ==
                           p.thread_nwd.tlv.border_router_16
                           ).\
                    must_next()
                pkts.filter_wpan_src64(LEADER).\
                    filter_ipv6_dst(pv.vars['ROUTER_%d_RLOC' %i]).\
                    filter_coap_ack(SVR_DATA_URI).\
                    must_next()

        # Step 5: Leader MUST multicast MLE Data Response with the new
        #         information collected from Router_1 and Router_2,
        #         including the following TLVs:,
        #              - Source Address TLV
        #              - Leader Data TLV
        #                  - Data Version field <incremented>
        #                  - Stable Data Version field <incremented>
        #              - Network Data TLV
        #                  - At least one Prefix TLV (Prefix 1)
        #                      - Two Border Router sub-TLVs
        #                      - 6LoWPAN ID sub-TLV
        _dr_pkt = pkts.filter_wpan_src64(LEADER).\
            filter_LLANMA().\
            filter_mle_cmd(MLE_DATA_RESPONSE).\
            filter(lambda p: {
                              NETWORK_DATA_TLV,
                              SOURCE_ADDRESS_TLV,
                              LEADER_DATA_TLV
                             } <= set(p.mle.tlv.type) and\
                   p.thread_nwd.tlv.border_router.flag.p == [1] and\
                   p.thread_nwd.tlv.border_router.flag.s == [1] and\
                   p.thread_nwd.tlv.border_router.flag.r == [1] and\
                   p.thread_nwd.tlv.border_router.flag.o == [1] and\
                   [Ipv6Addr(PREFIX_2001[:-3])] ==
                   p.thread_nwd.tlv.prefix
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
                                  NWD_BORDER_ROUTER_TLV,
                                  NWD_6LOWPAN_ID_TLV
                                 } <= set(p.thread_nwd.tlv.type) and\
                       p.thread_nwd.tlv.border_router.flag.p == [1, 1] and\
                       p.thread_nwd.tlv.border_router.flag.s == [1, 1] and\
                       p.thread_nwd.tlv.border_router.flag.r == [1, 1] and\
                       p.thread_nwd.tlv.border_router.flag.o == [1, 1] and\
                       p.mle.tlv.leader_data.data_version ==
                       (_dr_pkt.mle.tlv.leader_data.data_version + 1) % 256 and\
                       (p.mle.tlv.leader_data.stable_data_version ==
                        (_dr_pkt.mle.tlv.leader_data.stable_data_version + 1) % 256 or\
                       p.mle.tlv.leader_data.stable_data_version ==
                        (_pkt.mle.tlv.leader_data.stable_data_version + 1) % 256) and\
                       [Ipv6Addr(PREFIX_2001[:-3])] ==
                       p.thread_nwd.tlv.prefix
                       ).\
                must_next()

        # Step 6: Leader MUST send a MLE Child Update Request or MLE Data
        #         Response to SED, including the following TLVs:
        #             - Network Data TLV
        #                 At least one Prefix TLV (Prefix 1) including:
        #                     - Border Router sub-TLV(corresponding to Router_1)
        #                     - 6LoWPAN ID sub-TLV
        #                         - P_border_router_16<0xFFFE>
        #             - Source Address TLV
        #             - Leader Data TLV
        #                 Data version numbers should be the same as the ones
        #                 sent in the multicast data response in step 5.
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
                   [Ipv6Addr(PREFIX_2001[:-3])] ==
                   p.thread_nwd.tlv.prefix and\
                   p.thread_nwd.tlv.stable == [1, 1, 1] and\
                   p.mle.tlv.leader_data.data_version  ==
                   _dr_pkt1.mle.tlv.leader_data.data_version and\
                   p.mle.tlv.leader_data.stable_data_version  ==
                   _dr_pkt1.mle.tlv.leader_data.stable_data_version and\
                   [0xFFFE] == p.thread_nwd.tlv.border_router_16
                   ).\
            must_next()

        # Step 8: Leader  MUST detect that Router_1 is removed from the network and
        #         update the Router ID Set. Leader MUST remove the Network Data
        #         section corresponding to Router_1 and increment the Data Version
        #         and Stable Data Version

        # Step 9: Leader MUST multicast MLE Data Response neighbors and rx-on-when-idle
        #         Children (MED) including the following TLVs:,
        #              - Source Address TLV
        #              - Leader Data TLV
        #                  - Data Version field <incremented>
        #                  - Stable Data Version field <incremented>
        #              - Network Data TLV
        #                  - Router_1’s Network Data section MUST be removed
        pkts.filter_wpan_src64(LEADER).\
            filter_LLANMA().\
            filter_mle_cmd(MLE_DATA_RESPONSE).\
            filter(lambda p: {
                              NETWORK_DATA_TLV,
                              SOURCE_ADDRESS_TLV,
                              LEADER_DATA_TLV,
                             } <= set(p.mle.tlv.type) and\
                   [Ipv6Addr(PREFIX_2001[:-3])] ==
                   p.thread_nwd.tlv.prefix and\
                   [ROUTER_2_RLOC16] == p.thread_nwd.tlv.border_router_16 and\
                   p.mle.tlv.leader_data.data_version ==
                   (_dr_pkt1.mle.tlv.leader_data.data_version + 1) % 256 and\
                   p.mle.tlv.leader_data.stable_data_version ==
                   (_dr_pkt1.mle.tlv.leader_data.stable_data_version + 1) % 256
                   ).\
            must_next()

        # Step 10: The DUT MUST send a MLE Child Update Request or MLE Data
        #         Response to SED, containing the updated Network Data:
        #             - Network Data TLV
        #             - Source Address TLV
        #             - Active Timestamp TLV
        _pkt = pkts.filter_wpan_src64(LEADER).\
            filter_wpan_dst64(SED).\
            filter_mle_cmd2(MLE_CHILD_UPDATE_REQUEST, MLE_DATA_RESPONSE).\
            filter(lambda p: {
                              NETWORK_DATA_TLV,
                              SOURCE_ADDRESS_TLV,
                              LEADER_DATA_TLV,
                             } <= set(p.mle.tlv.type) and\
                   [Ipv6Addr(PREFIX_2001[:-3])] ==
                   p.thread_nwd.tlv.prefix and\
                   p.thread_nwd.tlv.border_router_16 is nullField and\
                   p.mle.tlv.leader_data.data_version ==
                   (_dr_pkt1.mle.tlv.leader_data.data_version + 1) % 256 and\
                   p.mle.tlv.leader_data.stable_data_version ==
                   (_dr_pkt1.mle.tlv.leader_data.stable_data_version + 1) % 256
                   ).\
            must_next()

        # Step 12: Leader MUST send MLE Child ID Response to Router_1, which
        #          includes the following TLVs:
        #              - Source Address TLV
        #              - Leader Data TLV
        #              - Address16 TLV
        #              - Route64 TLV
        #              - Network Data TLV
        #                  - At least one Prefix TLV (Prefix 1)
        #                    including:
        #                        - Border Router sub-tlv corresponding to Router_2
        #                        - 6LoWPAN ID sub-TLV
        pkts.filter_wpan_src64(LEADER).\
            filter_wpan_dst64(ROUTER_1).\
            filter_mle_cmd(MLE_CHILD_ID_RESPONSE).\
            filter(lambda p: {
                              ADDRESS16_TLV,
                              LEADER_DATA_TLV,
                              NETWORK_DATA_TLV,
                              SOURCE_ADDRESS_TLV,
                              ROUTE64_TLV
                              } <= set(p.mle.tlv.type) and\
                             {
                              NWD_BORDER_ROUTER_TLV,
                              NWD_6LOWPAN_ID_TLV
                             } <= set(p.thread_nwd.tlv.type) and\
                   [Ipv6Addr(PREFIX_2001[:-3])] ==
                   p.thread_nwd.tlv.prefix and\
                   [ROUTER_2_RLOC16] == p.thread_nwd.tlv.border_router_16
                   ).\
             must_next()

        # Step 13: Router_1 MUST send a CoAP Server DataNotification frame to
        #          the Leader including the server’s information(Prefix, Border Router):
        #             CoAP Request URI
        #                 coap://[<Leader address>]:MM/a/sd
        #             CoAP Payload
        #                 Thread Network Data TLV

        # Step 14: Leader sends a CoAP ACK frame to each of Router_1
        with pkts.save_index():
            pkts.filter_wpan_src64(ROUTER_1).\
                filter_wpan_dst16(LEADER_RLOC16).\
                filter_coap_request(SVR_DATA_URI).\
                filter(lambda p:
                       [Ipv6Addr(PREFIX_2001[:-3])] ==
                       p.thread_nwd.tlv.prefix and\
                       [ROUTER_1_RLOC16] ==
                       p.thread_nwd.tlv.border_router_16
                       ).\
                must_next()
            pkts.filter_wpan_src64(LEADER).\
                filter_ipv6_dst(ROUTER_1_RLOC).\
                filter_coap_ack(SVR_DATA_URI).\
                must_next()

        # Step 15: Leader MUST multicast MLE Data Response with the new
        #         information collected from Router_1 and Router_2,
        #         including the following TLVs:,
        #              - Source Address TLV
        #              - Leader Data TLV
        #                  - Data Version field <incremented>
        #                  - Stable Data Version field <incremented>
        #              - Network Data TLV
        #                  - At least one Prefix TLV (Prefix 1)
        #                      - Two Border Router sub-TLVs
        #                        corresponding to Router_1 and Router_2
        #                      - 6LoWPAN ID sub-TLV
        _dr_pkt2 = pkts.filter_wpan_src64(LEADER).\
            filter_LLANMA().\
            filter_mle_cmd(MLE_DATA_RESPONSE).\
            filter(lambda p: {
                              NETWORK_DATA_TLV,
                              SOURCE_ADDRESS_TLV,
                              LEADER_DATA_TLV
                             } <= set(p.mle.tlv.type) and\
                   {ROUTER_1_RLOC16, ROUTER_2_RLOC16} ==
                   set(p.thread_nwd.tlv.border_router_16) and\
                   [Ipv6Addr(PREFIX_2001[:-3])] ==
                   p.thread_nwd.tlv.prefix and\
                   p.mle.tlv.leader_data.data_version ==
                   (_pkt.mle.tlv.leader_data.data_version + 1) % 256 and\
                   p.mle.tlv.leader_data.stable_data_version ==
                   (_pkt.mle.tlv.leader_data.stable_data_version + 1) % 256
                   ).\
            must_next()

        # Step 16: Leader MUST send a MLE Child Update Request or MLE Data
        #          Response to SED, including the following TLVs:
        #              - Network Data TLV
        #                 At least one Prefix TLV (Prefix 1)
        #                     - Border Router TLV (corresponding to Router_1)
        #                     - 6LoWPAN ID sub-TLV
        #                         - P_border_router_16<0xFFFE>
        #              - Source Address TLV
        #              - Leader Data TLV
        #                 Data version numbers should be the same as those
        #                 sent in the multicast data response in step 15.
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
                   [Ipv6Addr(PREFIX_2001[:-3])] ==
                   p.thread_nwd.tlv.prefix and\
                   p.mle.tlv.leader_data.data_version  ==
                   _dr_pkt2.mle.tlv.leader_data.data_version and\
                   p.thread_nwd.tlv.stable == [1, 1, 1] and\
                   [0xFFFE] == p.thread_nwd.tlv.border_router_16
                   ).\
            must_next()

        # Step 17: Verifies connectivity by sending ICMPv6 Echo Requests from
        #          Router_1 and SED_1 to the Leader Prefix_1 based address.
        #          Leader must respond with ICMPv6 Echo Replies
        for node in ('ROUTER_1', 'SED'):
            _pkt = pkts.filter_ping_request().\
                filter_ipv6_src_dst(GUA[node], GUA['LEADER']).\
                must_next()
            pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
                filter_ipv6_src_dst(GUA['LEADER'], GUA[node]).\
                must_next()


if __name__ == '__main__':
    unittest.main()
