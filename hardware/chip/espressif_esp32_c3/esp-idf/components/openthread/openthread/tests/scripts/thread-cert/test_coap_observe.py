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


class TestCoapObserve(thread_cert.TestCase):
    """
    Test suite for CoAP Observations (RFC7641).
    """

    SUPPORT_NCP = False

    TOPOLOGY = {
        LEADER: {
            'mode': 'rdn',
            'allowlist': [ROUTER]
        },
        ROUTER: {
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
    }

    def _do_notification_test(self, con):
        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        self.nodes[ROUTER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER].get_state(), 'router')

        mleid = self.nodes[LEADER].get_ip6_address(config.ADDRESS_TYPE.ML_EID)

        self.nodes[LEADER].coap_start()
        self.nodes[LEADER].coap_set_resource_path('test')
        self.nodes[LEADER].coap_set_content('Test123')

        self.nodes[ROUTER].coap_start()
        response = self.nodes[ROUTER].coap_observe(mleid, 'test', con=con)

        first_observe = response['observe']
        self.assertIsNotNone(first_observe)
        self.assertEqual(response['payload'], 'Test123')
        self.assertEqual(response['source'], mleid)

        # This should have been emitted already, so should return immediately
        self.nodes[LEADER].coap_wait_subscribe()

        # Now change the content on the leader and wait for it to show up
        # on the router.  We will do this a few times with a short delay.
        for n in range(0, 5):
            content = 'msg%d' % n

            self.nodes[LEADER].coap_set_content(content)

            response = self.nodes[ROUTER].coap_wait_response()
            self.assertGreater(response['observe'], first_observe)
            self.assertEqual(response['payload'], content)
            self.assertEqual(response['source'], mleid)

        # Stop subscription
        self.nodes[ROUTER].coap_cancel()

        # We should see the response, but with no Observe option
        response = self.nodes[ROUTER].coap_wait_response()
        self.assertIsNone(response['observe'])
        # Content won't have changed.
        self.assertEqual(response['payload'], content)

        # Make another change, no notification should be sent
        self.nodes[LEADER].coap_set_content('LastNote')

        # This should time out!
        try:
            self.nodes[ROUTER].coap_wait_response()
            self.fail('Should not have received notification')
        except pexpect.exceptions.TIMEOUT:
            pass

        self.nodes[ROUTER].coap_stop()
        self.nodes[LEADER].coap_stop()

    def test_con(self):
        """
        Test notification using CON messages.
        """
        for trial in range(0, 3):
            try:
                self._do_notification_test(con=True)
                break
            except (AssertionError, pexpect.exceptions.TIMEOUT):
                continue

    def test_non(self):
        """
        Test notification using NON messages.
        """
        for trial in range(0, 3):
            try:
                self._do_notification_test(con=False)
                break
            except (AssertionError, pexpect.exceptions.TIMEOUT):
                continue


if __name__ == '__main__':
    unittest.main()
