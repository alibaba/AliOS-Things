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

import config
import thread_cert
from pktverify.consts import MLE_PARENT_RESPONSE, MLE_CHILD_ID_RESPONSE, SOURCE_ADDRESS_TLV, CHALLENGE_TLV, RESPONSE_TLV, LINK_LAYER_FRAME_COUNTER_TLV, ADDRESS16_TLV, LEADER_DATA_TLV, NETWORK_DATA_TLV, CONNECTIVITY_TLV, LINK_MARGIN_TLV, VERSION_TLV, ADDRESS_REGISTRATION_TLV
from pktverify.packet_verifier import PacketVerifier
from pktverify.null_field import nullField

LEADER = 1
ROUTER = 2
SED1 = 7

# Test Purpose and Description:
# -----------------------------
# The purpose of this test case is to validate the minimum
# conformance requirements for router-capable devices:
# a)Minimum number of supported children.
# b)Minimum MTU requirement when sending/forwarding an
#   IPv6 datagram to a SED.
# c)Minimum number of sent/forwarded IPv6 datagrams to
#   SED children.
#
# Test Topology:
# -------------
#
#         Leader
#           |
#       Router[DUT]
#      /          \
# MED1 - MED4 SED1 - SED6
#
# DUT Types:
# ----------
#  Router


class Cert_5_1_07_MaxChildCount(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False

    TOPOLOGY = {
        LEADER: {
            'name': 'LEADER',
            'mode': 'rdn',
            'allowlist': [ROUTER]
        },
        ROUTER: {
            'name': 'ROUTER',
            'max_children': 10,
            'mode': 'rdn',
            'allowlist': [LEADER, 3, 4, 5, 6, SED1, 8, 9, 10, 11, 12]
        },
        3: {
            'name': 'MED1',
            'is_mtd': True,
            'mode': 'rn',
            'timeout': config.DEFAULT_CHILD_TIMEOUT,
            'allowlist': [ROUTER]
        },
        4: {
            'name': 'MED2',
            'is_mtd': True,
            'mode': 'rn',
            'timeout': config.DEFAULT_CHILD_TIMEOUT,
            'allowlist': [ROUTER]
        },
        5: {
            'name': 'MED3',
            'is_mtd': True,
            'mode': 'rn',
            'timeout': config.DEFAULT_CHILD_TIMEOUT,
            'allowlist': [ROUTER]
        },
        6: {
            'name': 'MED4',
            'is_mtd': True,
            'mode': 'rn',
            'timeout': config.DEFAULT_CHILD_TIMEOUT,
            'allowlist': [ROUTER]
        },
        SED1: {
            'name': 'SED1',
            'is_mtd': True,
            'mode': '-',
            'timeout': config.DEFAULT_CHILD_TIMEOUT,
            'allowlist': [ROUTER]
        },
        8: {
            'name': 'SED2',
            'is_mtd': True,
            'mode': '-',
            'timeout': config.DEFAULT_CHILD_TIMEOUT,
            'allowlist': [ROUTER]
        },
        9: {
            'name': 'SED3',
            'is_mtd': True,
            'mode': '-',
            'timeout': config.DEFAULT_CHILD_TIMEOUT,
            'allowlist': [ROUTER]
        },
        10: {
            'name': 'SED4',
            'is_mtd': True,
            'mode': '-',
            'timeout': config.DEFAULT_CHILD_TIMEOUT,
            'allowlist': [ROUTER]
        },
        11: {
            'name': 'SED5',
            'is_mtd': True,
            'mode': '-',
            'timeout': config.DEFAULT_CHILD_TIMEOUT,
            'allowlist': [ROUTER]
        },
        12: {
            'name': 'SED6',
            'is_mtd': True,
            'mode': '-',
            'timeout': config.DEFAULT_CHILD_TIMEOUT,
            'allowlist': [ROUTER]
        },
    }

    def test(self):
        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        self.nodes[ROUTER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER].get_state(), 'router')

        for i in range(3, 13):
            self.nodes[i].start()
            self.simulator.go(7)
            self.assertEqual(self.nodes[i].get_state(), 'child')

        self.collect_rloc16s()
        self.collect_ipaddrs()

        ipaddrs = self.nodes[SED1].get_addrs()
        for addr in ipaddrs:
            if addr[0:4] != 'fe80' and 'ff:fe00' not in addr:
                self.assertTrue(self.nodes[LEADER].ping(addr, size=1232))
                break

        for i in range(3, 13):
            ipaddrs = self.nodes[i].get_addrs()
            for addr in ipaddrs:
                if addr[0:4] != 'fe80' and 'ff:fe00' not in addr:
                    self.assertTrue(self.nodes[LEADER].ping(addr, size=106))
                    break

    def verify(self, pv: PacketVerifier):
        pkts = pv.pkts
        pv.summary.show()

        ROUTER = pv.vars['ROUTER']
        router_pkts = pkts.filter_wpan_src64(ROUTER)

        # Step 1: The DUT MUST send properly formatted MLE Parent Response
        #         and MLE Child ID Response to each child.
        for i in range(1, 7):
            _pkts = router_pkts.copy().filter_wpan_dst64(pv.vars['SED%d' % i])
            _pkts.filter_mle_cmd(MLE_PARENT_RESPONSE).\
                filter(lambda p: {
                                  CHALLENGE_TLV,
                                  CONNECTIVITY_TLV,
                                  LEADER_DATA_TLV,
                                  LINK_LAYER_FRAME_COUNTER_TLV,
                                  LINK_MARGIN_TLV,
                                  RESPONSE_TLV,
                                  SOURCE_ADDRESS_TLV,
                                  VERSION_TLV
                                } <= set(p.mle.tlv.type)
                       ).\
                must_next()
            _pkts.filter_mle_cmd(MLE_CHILD_ID_RESPONSE).\
                filter(lambda p: {
                                  SOURCE_ADDRESS_TLV,
                                  LEADER_DATA_TLV,
                                  ADDRESS16_TLV,
                                  NETWORK_DATA_TLV,
                                  ADDRESS_REGISTRATION_TLV
                        } <= set(p.mle.tlv.type) and\
                       p.mle.tlv.addr16 is not nullField and\
                       p.thread_nwd.tlv.type is not None and\
                       p.thread_meshcop.tlv.type is not None
                       ).\
                must_next()

        for i in range(1, 5):
            _pkts = router_pkts.copy().filter_wpan_dst64(pv.vars['MED%d' % i])
            _pkts.filter_mle_cmd(MLE_PARENT_RESPONSE).\
                filter(lambda p: {
                                  CHALLENGE_TLV,
                                  CONNECTIVITY_TLV,
                                  LEADER_DATA_TLV,
                                  LINK_LAYER_FRAME_COUNTER_TLV,
                                  LINK_MARGIN_TLV,
                                  RESPONSE_TLV,
                                  SOURCE_ADDRESS_TLV,
                                  VERSION_TLV
                                } <= set(p.mle.tlv.type)
                       ).\
                must_next()

            _pkts.filter_mle_cmd(MLE_CHILD_ID_RESPONSE).\
                filter(lambda p: {
                                  SOURCE_ADDRESS_TLV,
                                  LEADER_DATA_TLV,
                                  ADDRESS16_TLV,
                                  NETWORK_DATA_TLV,
                                  ADDRESS_REGISTRATION_TLV
                        } <= set(p.mle.tlv.type) and\
                       p.mle.tlv.addr16 is not nullField and\
                       p.thread_meshcop.tlv.type is not None
                       ).\
                must_next()

        # Step 2: The DUT MUST properly forward ICMPv6 Echo Requests to all MED children
        #         The DUT MUST properly forward ICMPv6 Echo Replies to the Leader
        leader_rloc16 = pv.vars['LEADER_RLOC16']
        for i in range(1, 5):
            rloc16 = pv.vars['MED%d_RLOC16' % i]
            _pkts = router_pkts.copy()
            p = _pkts.filter('wpan.dst16 == {rloc16}', rloc16=rloc16).\
                filter_ping_request().\
                must_next()
            _pkts.filter('wpan.dst16 == {rloc16}',
                         rloc16=leader_rloc16).\
                  filter_ping_reply(identifier=p.icmpv6.echo.identifier).\
                  must_next()

        # Step 3: The DUT MUST properly forward ICMPv6 Echo Requests to all SED children
        #         The DUT MUST properly forward ICMPv6 Echo Replies to the Leader
        for i in range(1, 7):
            rloc16 = pv.vars['SED%d_RLOC16' % i]
            _pkts = router_pkts.copy()
            p = _pkts.filter('wpan.dst16 == {rloc16}', rloc16=rloc16).\
                filter_ping_request().\
                must_next()
            _pkts.filter('wpan.dst16 == {rloc16}',
                         rloc16=leader_rloc16).\
                  filter_ping_reply(identifier=p.icmpv6.echo.identifier).\
                  must_next()


if __name__ == '__main__':
    unittest.main()
