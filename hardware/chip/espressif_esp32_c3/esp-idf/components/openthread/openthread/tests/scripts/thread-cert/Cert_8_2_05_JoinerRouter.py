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

import command
from mesh_cop import MeshCopState
import thread_cert
from pktverify.consts import MLE_CHILD_ID_RESPONSE, MLE_DISCOVERY_RESPONSE, HANDSHAKE_CLIENT_HELLO, HANDSHAKE_HELLO_VERIFY_REQUEST, HANDSHAKE_SERVER_HELLO, HANDSHAKE_SERVER_KEY_EXCHANGE, HANDSHAKE_SERVER_HELLO_DONE, HANDSHAKE_CLIENT_KEY_EXCHANGE, CONTENT_CHANGE_CIPHER_SPEC, CONTENT_HANDSHAKE, CONTENT_APPLICATION_DATA, NM_EXTENDED_PAN_ID_TLV, NM_NETWORK_NAME_TLV, NM_STEERING_DATA_TLV, NM_COMMISSIONER_UDP_PORT_TLV, NM_JOINER_UDP_PORT_TLV, NM_DISCOVERY_RESPONSE_TLV, NM_JOINER_DTLS_ENCAPSULATION_TLV, NM_JOINER_UDP_PORT_TLV, NM_JOINER_IID_TLV, NM_JOINER_ROUTER_LOCATOR_TLV, NM_JOINER_ROUTER_KEK_TLV, RLY_RX_URI, RLY_TX_URI
from pktverify.packet_verifier import PacketVerifier

COMMISSIONER = 1
JOINER_ROUTER = 2
JOINER = 3
PSKD = 'PSKD01'
URL_1 = 'www.openthread.org'
URL_2 = 'www.wrongurl.org'

# Test Purpose and Description:
# -----------------------------
# The purpose of this test case is to verify that the DUT sends and receives
# relayed DTLS traffic correctly, when the wrong commissioning application is
# requested by the Joiner.
# It also verifies that the DUT (as a Joiner Router) sends JOIN_ENT.ntf encrypted
# with KEK.
#
# Test Topology:
# -------------
#   Commissioner
#       |
# Joiner Router
#       |
#    Joiner
#
# DUT Types:
# ----------
#  Commissioner
#  Joiner Router


class Cert_8_2_05_JoinerRouter(thread_cert.TestCase):
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

        self.nodes[COMMISSIONER].commissioner_set_provisioning_url(URL_1)
        self.nodes[COMMISSIONER].commissioner_start()
        self.simulator.go(5)
        self.nodes[COMMISSIONER].commissioner_add_joiner(self.nodes[JOINER_ROUTER].get_eui64(), PSKD)
        self.nodes[COMMISSIONER].commissioner_add_joiner(self.nodes[JOINER].get_eui64(), PSKD)
        self.simulator.go(5)

        self.nodes[JOINER_ROUTER].interface_up()
        self.nodes[JOINER_ROUTER].joiner_start(PSKD, URL_1)
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
        self.nodes[JOINER].joiner_start(PSKD, URL_2)
        self.simulator.go(10)

        self.simulator.read_cert_messages_in_commissioning_log([COMMISSIONER, JOINER_ROUTER])
        commissioner_messages = self.simulator.get_messages_sent_by(COMMISSIONER)

        self.assertEqual(
            self.nodes[JOINER].get_networkkey(),
            self.nodes[COMMISSIONER].get_networkkey(),
        )
        # check commissioner sends JOIN_FIN.rsp with reject
        command.check_commissioner_commissioning_messages(commissioner_messages.commissioning_messages,
                                                          MeshCopState.REJECT)
        self.collect_rloc16s()
        self.collect_ipaddrs()

        self.nodes[JOINER].thread_start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[JOINER].get_state(), 'router')

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        COMMISSIONER = pv.vars['COMMISSIONER']
        JOINER_ROUTER_RLOC16 = pv.vars["JOINER_ROUTER_RLOC16"]
        COMMISSIONER_RLOC16 = pv.vars["COMMISSIONER_RLOC16"]
        JOINER_ROUTER_LLA = pv.vars["JOINER_ROUTER_LLA"]

        # verify Joiner Router attaches to Commissioner
        pkts.filter_wpan_src64(COMMISSIONER).\
            filter_ipv6_dst(JOINER_ROUTER_LLA).\
            filter_mle_cmd(MLE_CHILD_ID_RESPONSE).\
            must_next()

        _rs_pkt = pkts.filter_mle_cmd(MLE_DISCOVERY_RESPONSE).\
            filter(lambda p: {
                              NM_EXTENDED_PAN_ID_TLV,
                              NM_NETWORK_NAME_TLV,
                              NM_STEERING_DATA_TLV,
                              NM_COMMISSIONER_UDP_PORT_TLV,
                              NM_JOINER_UDP_PORT_TLV,
                              NM_DISCOVERY_RESPONSE_TLV
                            } == set(p.thread_meshcop.tlv.type)
                  ).\
            must_next()

        # Step 3: Verify that the following details occur in the exchange between the
        #         Joiner, the Joiner_Router and the Commissioner

        # 1. UDP port (Specified by Commissioner in Discovery Response) is used as
        #    destination port for UDP datagrams from Joiner to Commissioner.

        # 2. Joiner sends an initial DTLS-ClientHello handshake record to the
        #    Joiner Router
        _ch_pkt = pkts.filter_ipv6_dst(JOINER_ROUTER_LLA).\
            filter(lambda p:
                   p.dtls.handshake.type == [HANDSHAKE_CLIENT_HELLO] and\
                   p.udp.srcport in _rs_pkt.thread_meshcop.tlv.udp_port and\
                   p.udp.dstport in _rs_pkt.thread_meshcop.tlv.udp_port
                   ).\
            must_next()

        # 3. The Joiner_Router receives the initial DTLS-ClientHello handshake record and
        #    sends a RLY_RX.ntf message to the Commissioner containing:
        #    CoAP URI-Path
        #        NON POST coap://<C>/c/rx
        #    CoAP Payload
        #        Joiner DTLS Encapsulation TLV
        #        Joiner UDP Port TLV
        #        Joiner IID TLV
        #        Joiner_Router Locator TLV
        _pkt = pkts.filter_coap_request(RLY_RX_URI).\
            filter_wpan_src16(JOINER_ROUTER_RLOC16).\
            filter_wpan_dst16(COMMISSIONER_RLOC16).\
            filter(lambda p: {
                              NM_JOINER_DTLS_ENCAPSULATION_TLV,
                              NM_JOINER_UDP_PORT_TLV,
                              NM_JOINER_IID_TLV,
                              NM_JOINER_ROUTER_LOCATOR_TLV
                             } == set(p.thread_meshcop.tlv.type) and\
                   p.thread_meshcop.tlv.udp_port == [_ch_pkt.udp.dstport] and\
                   p.thread_meshcop.tlv.jr_locator == JOINER_ROUTER_RLOC16
                   ).\
             must_next()

        # 4. The Commissioner receives the RLY_RX.ntf message and sends a RLY_TX.ntf message to
        #    the Joiner_Router containing:
        #    CoAP URI-Path
        #        NON POST coap://<JR>/c/tx
        #    CoAP Payload
        #        Joiner DTLS Encapsulation TLV
        #        Joiner UDP Port TLV
        #        Joiner IID TLV
        #        Joiner_Router Locator TLV
        #        Joiner_Router KEK TLV
        pkts.filter_coap_request(RLY_TX_URI).\
            filter_wpan_src16(COMMISSIONER_RLOC16).\
            filter_wpan_dst16(JOINER_ROUTER_RLOC16).\
            filter(lambda p: {
                              NM_JOINER_DTLS_ENCAPSULATION_TLV,
                              NM_JOINER_UDP_PORT_TLV,
                              NM_JOINER_IID_TLV,
                              NM_JOINER_ROUTER_LOCATOR_TLV,
                              NM_JOINER_ROUTER_KEK_TLV
                             } == set(p.thread_meshcop.tlv.type) and\
                   p.thread_meshcop.tlv.udp_port == [_ch_pkt.udp.dstport] and\
                   p.thread_meshcop.tlv.jr_locator == JOINER_ROUTER_RLOC16
                   ).\
             must_next()


if __name__ == '__main__':
    unittest.main()
