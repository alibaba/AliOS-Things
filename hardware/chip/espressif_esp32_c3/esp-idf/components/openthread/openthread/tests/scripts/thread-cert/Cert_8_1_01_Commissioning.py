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

import command
import dtls
import thread_cert
from pktverify.consts import MLE_DISCOVERY_REQUEST, MLE_DISCOVERY_RESPONSE, HANDSHAKE_CLIENT_HELLO, HANDSHAKE_SERVER_HELLO, HANDSHAKE_SERVER_KEY_EXCHANGE, HANDSHAKE_SERVER_HELLO_DONE, HANDSHAKE_CLIENT_KEY_EXCHANGE, HANDSHAKE_HELLO_VERIFY_REQUEST, CONTENT_APPLICATION_DATA, NM_EXTENDED_PAN_ID_TLV, NM_NETWORK_NAME_TLV, NM_STEERING_DATA_TLV, NM_COMMISSIONER_UDP_PORT_TLV, NM_JOINER_UDP_PORT_TLV, NM_DISCOVERY_REQUEST_TLV, NM_DISCOVERY_RESPONSE_TLV, THREAD_DISCOVERY_TLV, CONTENT_CHANGE_CIPHER_SPEC, CONTENT_HANDSHAKE, CONTENT_ALERT
from pktverify.packet_verifier import PacketVerifier

COMMISSIONER = 1
JOINER = 2

# Test Purpose and Description:
# -----------------------------
# The purpose of this test case is to verify the DTLS sessions between the
# on-mesh Commissioner and a Joiner when the correct PSKd is used
#
# Note that many of the messages/records exchanged are encrypted
# and cannot be observe
#
# Test Topology:
# -------------
# Commissioner
#    |
# Joiner
#
# DUT Types:
# ----------
#  Commissioner
#  Joiner


class Cert_8_1_01_Commissioning(thread_cert.TestCase):
    SUPPORT_NCP = False

    TOPOLOGY = {
        COMMISSIONER: {
            'name': 'COMMISSIONER',
            'networkkey': '00112233445566778899aabbccddeeff',
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
        self.simulator.go(3)
        self.nodes[COMMISSIONER].commissioner_add_joiner(self.nodes[JOINER].get_eui64(), 'PSKD01')

        self.nodes[JOINER].interface_up()
        self.nodes[JOINER].joiner_start('PSKD01')
        self.simulator.go(10)
        self.simulator.read_cert_messages_in_commissioning_log([COMMISSIONER, JOINER])
        self.assertEqual(
            self.nodes[JOINER].get_networkkey(),
            self.nodes[COMMISSIONER].get_networkkey(),
        )
        joiner_messages = self.simulator.get_messages_sent_by(JOINER)
        commissioner_messages = self.simulator.get_messages_sent_by(COMMISSIONER)

        # 5.8,9,10,11
        # - Joiner
        command.check_joiner_commissioning_messages(joiner_messages.commissioning_messages)
        # - Commissioner
        command.check_commissioner_commissioning_messages(commissioner_messages.commissioning_messages)
        # As commissioner is also joiner router
        command.check_joiner_router_commissioning_messages(commissioner_messages.commissioning_messages)
        self.nodes[JOINER].thread_start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[JOINER].get_state(), 'router')

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        COMMISSIONER = pv.vars['COMMISSIONER']
        COMMISSIONER_VERSION = pv.vars['COMMISSIONER_VERSION']
        JOINER_VERSION = pv.vars['JOINER_VERSION']

        # Step 3: Joiner sends MLE Discovery Request
        #         MLE Discovery Request message MUST have these values:
        #             - MLE Security Suite: 255 (No MLE Security)
        #             - Thread Discovery TLV
        #               Sub-TLVs:
        #                   - Discovery Request TLV
        #                       - Protocol Version: 2 or 3
        #                       (depends on the Thread stack version in testing)
        pkts.filter_mle_cmd(MLE_DISCOVERY_REQUEST).\
            filter_LLARMA().\
            filter(lambda p:
                   [THREAD_DISCOVERY_TLV] == p.mle.tlv.type and\
                   [NM_DISCOVERY_REQUEST_TLV] == p.thread_meshcop.tlv.type and\
                   p.thread_meshcop.tlv.discovery_req_ver == JOINER_VERSION
            ).\
        must_next()

        # Step 4: Commissioner sends MLE Discovery Response
        #         MLE Discovery Response message MUST have these values:
        #             - MLE Security Suite: 255 (No MLE Security)
        #             - Source Address in IEEE 802.15.4 header MUST be set to
        #               the MAC Extended Address (64-bit)
        #             - Destination Address in IEEE 802.15.4 header MUST be
        #               set to Discovery Request Source Address
        #             - Thread Discovery TLV
        #               Sub-TLVs:
        #                   - Discovery Request TLV
        #                       - Protocol Version: 2 or 3
        #                       (depends on the Thread stack version in testing)
        #                   - Extended PAN ID TLV
        #                   - Joiner UDP Port TLV
        #                   - Network Name TLV
        #                   - Steering Data TLV
        #                   - Commissioner UDP Port TLV (optional)
        _rs_pkt = pkts.filter_wpan_src64(COMMISSIONER).\
            filter_mle_cmd(MLE_DISCOVERY_RESPONSE).\
            filter(lambda p: {
                              NM_EXTENDED_PAN_ID_TLV,
                              NM_NETWORK_NAME_TLV,
                              NM_STEERING_DATA_TLV,
                              NM_JOINER_UDP_PORT_TLV,
                              NM_DISCOVERY_RESPONSE_TLV
                            } <= set(p.thread_meshcop.tlv.type) and\
                   p.thread_meshcop.tlv.discovery_rsp_ver == COMMISSIONER_VERSION
                  ).\
            must_next()

        # Step 5: Verify the following details occur in the exchange between
        #         Joiner and the Commissioner
        # 1. UDP port (Specified by the Commissioner: in Discovery Response)
        #    is used as destination port for UDP datagrams from Joiner to
        #    the Commissioner.

        # 2. Joiner sends an initial DTLS-ClientHello handshake record to the
        #    Commissioner
        pkts.filter_wpan_dst64(COMMISSIONER).\
            filter(lambda p:
                   p.dtls.handshake.type == [HANDSHAKE_CLIENT_HELLO] and\
                   p.udp.srcport in _rs_pkt.thread_meshcop.tlv.udp_port and\
                   p.udp.dstport in _rs_pkt.thread_meshcop.tlv.udp_port
                   ).\
            must_next()

        # 3. The Commissioner receives the initial DTLS-ClientHello handshake
        #    record and sends a DTLS-HelloVerifyRequest handshake record Joiner
        _pkt = pkts.filter_wpan_src64(COMMISSIONER).\
            filter(lambda p: p.dtls.handshake.type == [HANDSHAKE_HELLO_VERIFY_REQUEST]).\
            must_next()
        _pkt.must_verify(lambda p: p.dtls.handshake.cookie is not None)

        # 4. Joiner receives the DTLS-HelloVerifyRequest handshake record and sends
        #    a subsequent DTLS-ClientHello handshake record in one UDP datagram to the
        #    Commissioner
        #      Verify that both DTLS-HelloVerifyRequest and subsequent DTLS-ClientHello
        #      contain the same cookie
        pkts.filter_wpan_dst64(COMMISSIONER).\
            filter(lambda p:
                       p.dtls.handshake.type == [HANDSHAKE_CLIENT_HELLO] and\
                       p.dtls.handshake.cookie == _pkt.dtls.handshake.cookie
                       ).\
            must_next()

        # 5. Commissioner must correctly receive the subsequent DTLSClientHello
        #    handshake record and then send, in order, DTLSServerHello,
        #    DTLS-ServerKeyExchange and DTLSServerHelloDone handshake records to Joiner
        pkts.filter_wpan_src64(COMMISSIONER).\
            filter(lambda p:
                      p.dtls.handshake.type == [HANDSHAKE_SERVER_HELLO,
                                                HANDSHAKE_SERVER_KEY_EXCHANGE,
                                                HANDSHAKE_SERVER_HELLO_DONE]
                      ).\
               must_next()

        # 6. Joiner receives the DTLS-ServerHello, DTLSServerKeyExchange and
        #    DTLS-ServerHelloDone handshake records and sends, in order,
        #    a DTLS-ClientKeyExchange handshake record,
        #    a DTLS-ChangeCipherSpec record and
        #    an encrypted DTLS-Finished handshake record to the Commissioner.
        pkts.filter_wpan_dst64(COMMISSIONER).\
            filter(lambda p:
                       p.dtls.handshake.type == [HANDSHAKE_CLIENT_KEY_EXCHANGE] and\
                       {
                        CONTENT_CHANGE_CIPHER_SPEC,
                        CONTENT_HANDSHAKE
                       } == set(p.dtls.record.content_type)
                       ).\
               must_next()

        # 7. Commissioner receives the DTLS-ClientKeyExchange handshake record, the
        #    DTLS-ChangeCipherSpec record and the encrypted DTLS-Finished handshake record,
        #    and sends a DTLS-ChangeCipherSpec record and an encrypted DTLSFinished handshake
        #    record in that order to Joiner
        pkts.filter_wpan_src64(COMMISSIONER).\
            filter(lambda p: {
                              CONTENT_CHANGE_CIPHER_SPEC,
                              CONTENT_HANDSHAKE
                             } == set(p.dtls.record.content_type)
                      ).\
              must_next()

        # 8. Joiner receives the DTLS-ChangeCipherSpec record and the encrypted DTLS-Finished
        #    handshake record and sends a JOIN_FIN.req message in an encrypted DTLS-ApplicationData
        #    record in a single UDP datagram to Commissioner.
        pkts.filter_wpan_dst64(COMMISSIONER).\
            filter(lambda p:
                   [CONTENT_APPLICATION_DATA] == p.dtls.record.content_type
                  ).\
               must_next()

        # 9. Commissioner receives the encrypted DTLS-ApplicationData record and sends a
        #    JOIN_FIN.rsp message in an encrypted DTLS-ApplicationData record in a single
        #    UDP datagram to Joiner
        pkts.filter_wpan_src64(COMMISSIONER).\
            filter(lambda p:
                   [CONTENT_APPLICATION_DATA] == p.dtls.record.content_type
                  ).\
               must_next()

        # 10. Commissioner sends an encrypted JOIN_ENT.ntf message to Joiner

        # 11. Joiner receives the encrypted JOIN_ENT.ntf message and sends an encrypted
        #     JOIN_ENT.ntf dummy response to Commissioner

        # Check Step 8 ~ 11 in test()

        # 12. Joiner sends an encrypted DTLS-Alert record with a code of 0 (close_notify)
        #     to Commissioner
        pkts.filter_wpan_dst64(COMMISSIONER).\
            filter(lambda p:
                   [CONTENT_ALERT] == p.dtls.record.content_type
                  ).\
               must_next()


if __name__ == '__main__':
    unittest.main()
