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
# This test verifies that the basic MLR feature works.
#
import logging
import unittest

import config
import thread_cert
from pktverify.packet_verifier import PacketVerifier

PBBR = 1
SBBR = 2
ROUTER = 3
LEADER = 4
FED = 5
MED = 6
SED = 7
HOST = 8

SED_POLL_PERIOD = 1000  # Milliseconds

MA1 = 'ff04::1234:777a:1'
MA2 = 'ff05::1234:777a:2'
MA3 = 'ff05::1234:777a:3'
MA4 = 'ff05::1234:777a:4'
MA5 = 'ff05::1234:777a:5'

BBR_REGISTRATION_JITTER = 1
WAIT_REDUNDANCE = 3


class TestMlr(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False

    # Topology:
    #   --------(eth)---------
    #       |     |      |
    #     PBBR---SBBR   HOST
    #       \    /  \
    #        \  /    \
    #        ROUTER--LEADER
    #       /  |  \
    #      /   |   \
    #    FED  MED  SED
    #
    # More links:
    #    PBBR---LEADER
    #
    TOPOLOGY = {
        PBBR: {
            'name': 'PBBR',
            'allowlist': [SBBR, ROUTER, LEADER],
            'is_otbr': True,
            'version': '1.2',
            'bbr_registration_jitter': BBR_REGISTRATION_JITTER,
        },
        SBBR: {
            'name': 'SBBR',
            'allowlist': [PBBR, ROUTER, LEADER],
            'is_otbr': True,
            'version': '1.2',
            'bbr_registration_jitter': BBR_REGISTRATION_JITTER,
        },
        ROUTER: {
            'name': 'ROUTER',
            'allowlist': [PBBR, SBBR, LEADER, FED, MED, SED],
            'version': '1.2',
        },
        LEADER: {
            'name': 'LEADER',
            'allowlist': [PBBR, SBBR, ROUTER],
            'version': '1.2',
        },
        FED: {
            'name': 'FED',
            'allowlist': [ROUTER],
            'version': '1.2',
            'router_upgrade_threshold': 0,
        },
        MED: {
            'name': 'MED',
            'allowlist': [ROUTER],
            'version': '1.2',
            'mode': 'rn',
        },
        SED: {
            'name': 'SED',
            'allowlist': [ROUTER],
            'version': '1.2',
            'mode': 'n'
        },
        HOST: {
            'name': 'Host',
            'is_host': True
        },
    }

    def test(self):
        # Bring up Host
        self.nodes[HOST].start()

        # Bring up Leader
        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual('leader', self.nodes[LEADER].get_state())

        # Bring up Router
        self.nodes[ROUTER].start()
        self.simulator.go(5)
        self.assertEqual('router', self.nodes[ROUTER].get_state())

        # Bring up PBBR
        self.nodes[PBBR].start()
        self.simulator.go(5)
        self.assertEqual('router', self.nodes[PBBR].get_state())
        self.nodes[PBBR].enable_backbone_router()
        self.simulator.go(10)
        self.assertTrue(self.nodes[PBBR].is_primary_backbone_router)
        self.nodes[PBBR].add_prefix(config.DOMAIN_PREFIX, "parosD")
        self.nodes[PBBR].register_netdata()

        # Bring up SBBR
        self.nodes[SBBR].start()
        self.simulator.go(5)
        self.assertEqual('router', self.nodes[SBBR].get_state())
        self.nodes[SBBR].enable_backbone_router()
        self.simulator.go(10)
        self.assertFalse(self.nodes[SBBR].is_primary_backbone_router)

        # Bring up FED, MED, SED
        self.nodes[FED].start()
        self.nodes[MED].start()
        self.nodes[SED].set_pollperiod(SED_POLL_PERIOD)
        self.nodes[SED].start()
        self.simulator.go(5)
        self.assertEqual('child', self.nodes[FED].get_state())
        self.assertEqual('child', self.nodes[MED].get_state())
        self.assertEqual('child', self.nodes[SED].get_state())

        # Verify Multicast Routing works for all devices
        self._verify_multicast_routing(ROUTER, MA1)
        self._verify_multicast_routing(LEADER, MA2)
        self._verify_multicast_routing(FED, MA3)
        self._verify_multicast_routing(MED, MA4, is_med=True)
        self._verify_multicast_routing(SED, MA5, is_med=True, is_sed=True)

        # Verify MA_scope2 is not reachable from Host
        MA_scope2 = 'ff02::10'
        self.nodes[ROUTER].add_ipmaddr(MA_scope2)
        self.simulator.go(3)
        self.assertFalse(self.nodes[HOST].ping(MA_scope2, backbone=True, ttl=10))
        self.nodes[ROUTER].del_ipmaddr(MA_scope2)

        # Verify MA_scope3 is not reachable from Host
        MA_scope3 = 'ff03::1234:777a:5'
        self.nodes[ROUTER].add_ipmaddr(MA_scope3)
        self.simulator.go(3)
        self.assertFalse(self.nodes[HOST].ping(MA_scope3, backbone=True, ttl=10))
        self.nodes[ROUTER].del_ipmaddr(MA_scope3)

        # Router subscribes MA2 and MA3 at the same time and verify that they are both reachable
        self.nodes[ROUTER].add_ipmaddr(MA2)
        self.nodes[ROUTER].add_ipmaddr(MA3)
        self.simulator.go(3)
        self.assertTrue(self.nodes[HOST].ping(MA2, backbone=True, ttl=10))
        self.assertTrue(self.nodes[HOST].ping(MA3, backbone=True, ttl=10))
        self.nodes[ROUTER].del_ipmaddr(MA2)
        self.nodes[ROUTER].del_ipmaddr(MA3)
        self.simulator.go(1)

    def _verify_multicast_routing(self, nodeid: int, ma: str, is_med=False, is_sed=False):
        logging.info('_verify_multicast_routing: nodeid=%d, MA=%s', nodeid, ma)
        # Verify MA is not reachable from Host initially
        self.assertFalse(self.nodes[HOST].ping(ma, backbone=True, ttl=10))

        # Device subscribes MA
        self.nodes[nodeid].add_ipmaddr(ma)
        self.simulator.go(3 + (SED_POLL_PERIOD * 2 / 1000) * is_sed + config.PARENT_AGGREGATIOIN_DELAY * is_med +
                          WAIT_REDUNDANCE)

        # Verify MA is reachable from Host
        self.assertTrue(self.nodes[HOST].ping(ma, backbone=True, ttl=10))

        # Device unsubscribes MA
        self.nodes[nodeid].del_ipmaddr(ma)
        self.simulator.go(1)

        # Verify MA is not reachable from Host after unsubscribed
        self.assertFalse(self.nodes[HOST].ping(ma, backbone=True, ttl=10))


if __name__ == '__main__':
    unittest.main()
