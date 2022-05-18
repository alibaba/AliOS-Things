#!/usr/bin/env python3
#
#  Copyright (c) 2021, The OpenThread Authors.
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
import ipaddress

import config
import thread_cert

LEADER = 1


class Test_SetMlIid(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False

    TOPOLOGY = {
        LEADER: {
            'mode': 'rdn',
        },
    }

    def test(self):
        # Set ML-IID before Thread is enabled.
        self.nodes[LEADER].set_mliid('1122334455667788')

        self.nodes[LEADER].start()
        self.simulator.go(4)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        # Ensure set ML-IID was effective.
        mleid = self.nodes[LEADER].get_ip6_address(config.ADDRESS_TYPE.ML_EID)
        self.assertEqual(b'\x11\x22\x33\x44\x55\x66\x77\x88', ipaddress.IPv6Address(mleid).packed[-8:])

        # Ensure set ML-IID fail after Thread is enabled.
        self.assertRaises(Exception, lambda: self.nodes[LEADER].set_mliid('5566778811223344'))

        self.nodes[LEADER].reset()

        self.nodes[LEADER].start()
        self.simulator.go(10)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        # Ensure ML-IID is persistent after reset.
        mleid = self.nodes[LEADER].get_ip6_address(config.ADDRESS_TYPE.ML_EID)
        self.assertEqual(b'\x11\x22\x33\x44\x55\x66\x77\x88', ipaddress.IPv6Address(mleid).packed[-8:])


if __name__ == '__main__':
    unittest.main()
