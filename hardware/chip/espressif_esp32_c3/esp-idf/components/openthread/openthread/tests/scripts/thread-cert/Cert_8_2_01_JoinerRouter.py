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
from pktverify.consts import MLE_CHILD_ID_RESPONSE, MLE_DISCOVERY_RESPONSE, HANDSHAKE_CLIENT_HELLO, NM_EXTENDED_PAN_ID_TLV, NM_NETWORK_NAME_TLV, NM_STEERING_DATA_TLV, NM_COMMISSIONER_UDP_PORT_TLV, NM_JOINER_UDP_PORT_TLV, NM_DISCOVERY_RESPONSE_TLV, RLY_RX_URI, RLY_TX_URI
from pktverify.packet_verifier import PacketVerifier

COMMISSIONER = 1
JOINER_ROUTER = 2
JOINER = 3


class Cert_8_2_01_JoinerRouter(thread_cert.TestCase):
    SUPPORT_NCP = False

    TOPOLOGY = {
        COMMISSIONER: {
            'name': 'COMMISSIONER',
            'networkkey': '00112233445566778899aabbccddeeff',
            'mode': 'rdn',
        },
        JOINER_ROUTER: {
            'name': 'JOINER_ROUTER',
            'networkkey': 'deadbeefdeadbeefdeadbeefdeadbeef',
            'mode': 'rdn',
        },
        JOINER: {
            'name': 'JOINER',
            'networkkey': 'deadbeefdeadbeefdeadbeefdeadbeef',
            'mode': 'rdn',
        },
    }

    def test(self):
        self.nodes[COMMISSIONER].interface_up()
        self.nodes[COMMISSIONER].thread_start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[COMMISSIONER].get_state(), 'leader')

        self.nodes[COMMISSIONER].commissioner_start()
        self.simulator.go(5)
        self.nodes[COMMISSIONER].commissioner_add_joiner(self.nodes[JOINER_ROUTER].get_eui64(), 'PSKD01')
        self.nodes[COMMISSIONER].commissioner_add_joiner(self.nodes[JOINER].get_eui64(), 'PSKD02')
        self.simulator.go(5)

        self.nodes[JOINER_ROUTER].interface_up()
        self.nodes[JOINER_ROUTER].joiner_start('PSKD01')
        self.simulator.go(10)
        self.assertEqual(
            self.nodes[JOINER_ROUTER].get_networkkey(),
            self.nodes[COMMISSIONER].get_networkkey(),
        )

        self.nodes[JOINER_ROUTER].thread_start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[JOINER_ROUTER].get_state(), 'router')

        self.nodes[COMMISSIONER].enable_allowlist()
        self.nodes[COMMISSIONER].add_allowlist(self.nodes[JOINER_ROUTER].get_addr64())

        self.nodes[JOINER].enable_allowlist()
        self.nodes[JOINER].add_allowlist(self.nodes[JOINER_ROUTER].get_addr64())

        self.nodes[JOINER].interface_up()
        self.nodes[JOINER].joiner_start('PSKD02')
        self.simulator.go(10)
        self.assertEqual(
            self.nodes[JOINER].get_networkkey(),
            self.nodes[COMMISSIONER].get_networkkey(),
        )

        self.nodes[JOINER].thread_start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[JOINER].get_state(), 'router')
        self.collect_rloc16s()

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        COMMISSIONER = pv.vars['COMMISSIONER']
        _cpkts = pkts.filter_wpan_src64(COMMISSIONER)
        _cpkts.filter_mle_cmd(MLE_CHILD_ID_RESPONSE).must_next()

        # Step 3: Verify that the following details occur in the exchange between the Joiner,
        # the Joiner_Router and the Commissioner
        # 1. UDP port (Specified by the Commissioner: in Discovery Response) is used as destination port
        # for UDP datagrams from Joiner_1 to the Commissioner.
        pkts.range(_cpkts.index).filter_mle_cmd(MLE_DISCOVERY_RESPONSE).must_next().must_verify(
            lambda p: {
                NM_EXTENDED_PAN_ID_TLV, NM_NETWORK_NAME_TLV, NM_STEERING_DATA_TLV, NM_COMMISSIONER_UDP_PORT_TLV,
                NM_JOINER_UDP_PORT_TLV, NM_DISCOVERY_RESPONSE_TLV
            } == set(p.thread_meshcop.tlv.type))

        # 2. Joiner_1 sends an initial DTLS-ClientHello handshake record to the Commissioner
        pkts.filter(lambda p: p.dtls.handshake.type == [HANDSHAKE_CLIENT_HELLO]).must_next()

        # 3. The Joiner_Router receives the initial DTLS-ClientHello handshake record and sends a RLY_RX.ntf
        # message to the Commissioner
        # Todo: verify coap payload
        jr_rloc16 = pv.vars["JOINER_ROUTER_RLOC16"]
        c_rloc16 = pv.vars["COMMISSIONER_RLOC16"]
        pkts.filter_coap_request(RLY_RX_URI).must_next().must_verify(
            lambda p: p.wpan.src16 == jr_rloc16 and p.wpan.dst16 == c_rloc16)

        # 4. The Commissioner receives the RLY_RX.ntf message and sends a RLY_TX.ntf message to the Joiner_Router
        pkts.filter_coap_request(RLY_TX_URI).must_next().must_verify(
            lambda p: p.wpan.src16 == c_rloc16 and p.wpan.dst16 == jr_rloc16)


if __name__ == '__main__':
    unittest.main()
