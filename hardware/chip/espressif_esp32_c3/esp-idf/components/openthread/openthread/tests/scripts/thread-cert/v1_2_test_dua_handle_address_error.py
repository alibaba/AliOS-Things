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
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS'
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
# This script tests how node would handle duplicated Domain Unicast Address.
#

import unittest

import config
import thread_cert

BBR_1 = 1  # Collapsed with Leader Role
ROUTER = 2
FED = 3
MED = 4

WAIT_ATTACH = 5
WAIT_REDUNDANCE = 3
BBR_REGISTRATION_JITTER = 5
SED_POLL_PERIOD = 2000  # 2s
MED_TIMEOUT = 20  # 20s
"""
 Topology

  BBR_1 (Leader)
     |
  ROUTER_1---FED
     |
    MED
"""

WAIT_REDUNDANCE = 3
REG_DELAY = 5


class TestDomainUnicastAddress(thread_cert.TestCase):
    TOPOLOGY = {
        BBR_1: {
            'version': '1.2',
            'allowlist': [ROUTER],
            'is_bbr': True
        },
        ROUTER: {
            'version': '1.2',
            'allowlist': [BBR_1, FED, MED],
        },
        FED: {
            'version': '1.2',
            'allowlist': [ROUTER],
            'router_eligible': False,
            'mode': 'rdn',
        },
        MED: {
            'version': '1.2',
            'allowlist': [ROUTER],
            'mode': 'rn',
        },
    }

    def test(self):
        self.simulator.set_lowpan_context(1, config.DOMAIN_PREFIX)

        # 1) Bring up BBR_1, BBR_1 becomes Leader and Primary Backbone Router, with Domain
        #    Prefix without `P_slaac`.
        self.nodes[BBR_1].set_bbr_registration_jitter(BBR_REGISTRATION_JITTER)
        self.nodes[BBR_1].set_backbone_router(seqno=1, reg_delay=REG_DELAY)
        self.nodes[BBR_1].start()

        self.simulator.go(WAIT_ATTACH + config.DEFAULT_ROUTER_SELECTION_JITTER)
        self.assertEqual(self.nodes[BBR_1].get_state(), 'leader')
        self.nodes[BBR_1].enable_backbone_router()
        self.simulator.go(BBR_REGISTRATION_JITTER + WAIT_REDUNDANCE)
        self.assertEqual(self.nodes[BBR_1].get_backbone_router_state(), 'Primary')

        self.nodes[BBR_1].set_domain_prefix(config.DOMAIN_PREFIX, 'prosD')
        self.simulator.go(WAIT_REDUNDANCE)

        # 2) Bring up ROUTER_1
        self.nodes[ROUTER].start()
        self.simulator.go(WAIT_ATTACH + REG_DELAY + WAIT_REDUNDANCE)
        self.assertEqual(self.nodes[ROUTER].get_state(), 'router')

        # Bring up FED
        self.nodes[FED].start()
        self.simulator.go(WAIT_ATTACH + REG_DELAY + WAIT_REDUNDANCE)
        self.assertEqual(self.nodes[FED].get_state(), 'child')

        # Bring up MED
        self.nodes[MED].start()
        self.simulator.go(WAIT_ATTACH + config.PARENT_AGGREGATIOIN_DELAY + REG_DELAY + WAIT_REDUNDANCE)
        self.assertEqual(self.nodes[MED].get_state(), 'child')

        self._verify_dua_handle_address_error(ROUTER)
        self._verify_dua_handle_address_error(FED)
        self._verify_dua_handle_address_error(MED, is_med=True)

    def _verify_dua_handle_address_error(self, nodeid, is_med=False):
        dua = self.nodes[nodeid].get_addr(config.DOMAIN_PREFIX)
        self.assertIsNotNone(dua)

        # Ping the DUA to verify reachability, and also fill the EID cache on BBR_1
        self.assertTrue(self.nodes[BBR_1].ping(dua))

        self.simulator.go(WAIT_REDUNDANCE)

        # Send fake /a/an from ROUTER to BBR_1 for the node's DUA
        pbbr_rloc = self.nodes[BBR_1].get_ip6_address(config.ADDRESS_TYPE.RLOC)
        self.nodes[ROUTER].send_address_notification(pbbr_rloc, dua, f'000000000000{nodeid:04x}')

        self.simulator.go(config.PARENT_AGGREGATIOIN_DELAY * is_med + REG_DELAY + WAIT_REDUNDANCE + 50)

        # Make sure device handles /a/ae correctly by generating new DUA
        new_dua = self.nodes[nodeid].get_addr(config.DOMAIN_PREFIX)
        self.assertNotEqual(dua, new_dua)
        self.assertTrue(self.nodes[BBR_1].ping(new_dua))
        self.assertFalse(self.nodes[BBR_1].ping(dua))

        self.simulator.go(3)


if __name__ == '__main__':
    unittest.main()
