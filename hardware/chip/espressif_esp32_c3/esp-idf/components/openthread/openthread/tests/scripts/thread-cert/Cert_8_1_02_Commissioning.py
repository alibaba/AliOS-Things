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
from pktverify.consts import MLE_DISCOVERY_RESPONSE, HANDSHAKE_CLIENT_HELLO, HANDSHAKE_SERVER_HELLO, HANDSHAKE_SERVER_KEY_EXCHANGE, HANDSHAKE_SERVER_HELLO_DONE, HANDSHAKE_CLIENT_KEY_EXCHANGE, HANDSHAKE_HELLO_VERIFY_REQUEST, NM_EXTENDED_PAN_ID_TLV, NM_NETWORK_NAME_TLV, NM_STEERING_DATA_TLV, NM_COMMISSIONER_UDP_PORT_TLV, NM_JOINER_UDP_PORT_TLV, NM_DISCOVERY_RESPONSE_TLV, CONTENT_CHANGE_CIPHER_SPEC, CONTENT_ALERT, CONTENT_HANDSHAKE
from pktverify.packet_verifier import PacketVerifier

COMMISSIONER = 1
JOINER = 2


class Cert_8_1_02_Commissioning(thread_cert.TestCase):
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
        self.nodes[JOINER].joiner_start('10DKSP')
        self.simulator.go(10)

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        COMMISSIONER = pv.vars['COMMISSIONER']
        _cpkts = pkts.filter_wpan_src64(COMMISSIONER)
        _cpkts2 = pkts.filter_wpan_dst64(COMMISSIONER)

        # Step 3: Verify the following details occur in the exchange between
        # Joiner_1 and the Commissioner
        # 1. UDP port (Specified by the Commissioner: in Discovery Response) is used as destination port for UDP datagrams from Joiner_1 to the Commissioner.
        _cpkts.filter_mle_cmd(MLE_DISCOVERY_RESPONSE).must_next().must_verify(
            lambda p: {
                NM_EXTENDED_PAN_ID_TLV, NM_NETWORK_NAME_TLV, NM_STEERING_DATA_TLV, NM_COMMISSIONER_UDP_PORT_TLV,
                NM_JOINER_UDP_PORT_TLV, NM_DISCOVERY_RESPONSE_TLV
            } == set(p.thread_meshcop.tlv.type))

        # 2. Joiner_1 sends an initial DTLS-ClientHello handshake record to the Commissioner
        _cpkts2.range(_cpkts.index).filter(lambda p: p.dtls.handshake.type == [HANDSHAKE_CLIENT_HELLO]).must_next()

        # 3. The Commissioner must correctly receive the initial DTLS-ClientHello handshake record and send a DTLS-HelloVerifyRequest handshake record to Joiner_1
        _pkt = _cpkts.range(
            _cpkts2.index).filter(lambda p: p.dtls.handshake.type == [HANDSHAKE_HELLO_VERIFY_REQUEST]).must_next()
        _pkt.must_verify(lambda p: p.dtls.handshake.cookie is not None)

        # 4. Joiner_1 receives the DTLS-HelloVerifyRequest handshake record and sends a subsequent DTLS-ClientHello handshake record in one UDP datagram to the Commissioner
        # 5. Verify that both DTLS-HelloVerifyRequest and subsequent DTLS-ClientHello contain the same cookie
        _cpkts2.filter(lambda p: p.dtls.handshake.type == [HANDSHAKE_CLIENT_HELLO]).must_next().must_verify(
            lambda p: p.dtls.handshake.cookie == _pkt.dtls.handshake.cookie)

        # 6. The Commissioner must correctly receive the subsequent DTLSClientHello handshake record and then send, in order, DTLSServerHello, DTLS-ServerKeyExchange and DTLSServerHelloDone handshake records to Joiner_1
        _cpkts.filter(
            lambda p: p.dtls.handshake.type ==
            [HANDSHAKE_SERVER_HELLO, HANDSHAKE_SERVER_KEY_EXCHANGE, HANDSHAKE_SERVER_HELLO_DONE]).must_next()

        # 7. Joiner_1 receives the DTLS-ServerHello, DTLSServerKeyExchange and DTLS-ServerHelloDone handshake records and sends, in order, a DTLS-ClientKeyExchange handshake record, a DTLS-ChangeCipherSpec record and an encrypted DTLS-Finished handshake record to the Commissioner.
        _cpkts2.filter(lambda p: p.dtls.handshake.type == [HANDSHAKE_CLIENT_KEY_EXCHANGE]).must_next().must_verify(
            lambda p: p.dtls.handshake.type == [HANDSHAKE_CLIENT_KEY_EXCHANGE] and set(
                p.dtls.record.content_type) == {CONTENT_CHANGE_CIPHER_SPEC, CONTENT_HANDSHAKE})

        # 8. The Commissioner must receive the DTLS-ClientKeyExchange handshake record, the DTLS-ChangeCipherSpec and the encrypted DTLS-Finished handshake record, and then send a DTLS-Alert record with error code 20 (bad record MAC) - in one UDP datagram - to Joiner_1
        _cpkts.filter(
            lambda p: p.dtls.record.content_type == [CONTENT_ALERT] and p.dtls.alert_message.desc == 20).must_next()


if __name__ == '__main__':
    unittest.main()
