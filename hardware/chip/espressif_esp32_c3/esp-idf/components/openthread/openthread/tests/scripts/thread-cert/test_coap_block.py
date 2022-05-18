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

import pexpect
import config
import thread_cert

LEADER = 1
ROUTER = 2


class TestCoapBlockTransfer(thread_cert.TestCase):
    """
    Test suite for CoAP Block-Wise Transfers (RFC7959).
    """

    SUPPORT_NCP = False

    TOPOLOGY = {
        LEADER: {
            'mode': 'rdn',
            'whitelist': [ROUTER]
        },
        ROUTER: {
            'mode': 'rdn',
            'whitelist': [LEADER]
        },
    }

    def _do_transfer_test(self, method):
        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        self.nodes[ROUTER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER].get_state(), 'router')

        mleid = self.nodes[LEADER].get_ip6_address(config.ADDRESS_TYPE.ML_EID)

        self.nodes[LEADER].coap_start()
        self.nodes[LEADER].coap_set_resource_path_block('test', 10)

        self.nodes[ROUTER].coap_start()

        if method == 'GET':
            response = self.nodes[ROUTER].coap_get_block(mleid, 'test', size=32)
            response_payload = response['payload']
            self.assertIsNotNone(response_payload)

        if method == 'PUT':
            self.nodes[ROUTER].coap_put_block(mleid, 'test', size=256, count=2)
            request = self.nodes[ROUTER].coap_wait_request()
            request_payload = request['payload']
            self.assertIsNotNone(request_payload)

        if method == 'POST':
            self.nodes[ROUTER].coap_post_block(mleid, 'test', size=1024, count=2)
            request = self.nodes[ROUTER].coap_wait_request()
            request_payload = request['payload']
            self.assertIsNotNone(request_payload)

        self.simulator.go(10)

        self.nodes[ROUTER].coap_stop()
        self.nodes[LEADER].coap_stop()

    def test_get(self):
        """
        Test block-wise transfer using GET method.
        """
        for trial in range(0, 3):
            try:
                self._do_transfer_test(method='GET')
                break
            except (AssertionError, pexpect.exceptions.TIMEOUT):
                continue

    def test_put(self):
        """
        Test block-wise transfer using PUT method.
        """
        for trial in range(0, 3):
            try:
                self._do_transfer_test(method='PUT')
                break
            except (AssertionError, pexpect.exceptions.TIMEOUT):
                continue

    def test_post(self):
        """
        Test block-wise transfer using POST method.
        """
        for trial in range(0, 3):
            try:
                self._do_transfer_test(method='POST')
                break
            except (AssertionError, pexpect.exceptions.TIMEOUT):
                continue


if __name__ == '__main__':
    unittest.main()
