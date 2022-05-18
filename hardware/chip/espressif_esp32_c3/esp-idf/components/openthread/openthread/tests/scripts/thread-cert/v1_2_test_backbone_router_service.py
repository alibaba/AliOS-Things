#!/usr/bin/env python3
#
#  Copyright (c) 2019, The OpenThread Authors.
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

import unittest

import thread_cert
import config

LEADER_1_1 = 1
BBR_1 = 2
BBR_2 = 3

WAIT_ATTACH = 5
WAIT_REDUNDANCE = 3
ROUTER_SELECTION_JITTER = 1
BBR_REGISTRATION_JITTER = 5
"""
 Topology

 LEADER_1_1 --- BBR_1
          \        |
            \      |
              \    |
                BBR_2

 1) Bring up Leader_1_1 and then BBR_1, BBR_1 becomes Primary Backbone Router.
 2) Reset BBR_1, if bring back soon, it could restore the Backbone Router Service
    from the network, after increasing sequence number, it will reregister its
    Backbone Router Service to the Leader and become Primary.
 3) Reset BBR_1, if bring back after it is released in the network, BBR_1 will
    choose a random sequence number, register its Backbone Router Service to
    Leader and become Primary.
 4) Configure BBR_2 with highest sequence number and explicitly trigger SRV_DATA.ntf.
    BBR_2 would become Primary and BBR_1 would change to Secondary with sequence
    number increased by 1.
    a) Check communication via DUA.
 5) Stop BBR_2, BBR_1 would become Primary after detecting there is no available
    Backbone Router Service in Thread Network.
 6) Bring back BBR_2, and it would become Secondary.
    a) Check the uniqueness of DUA by comparing the one in above 4a).
    b) Check communication via DUA.

"""


class TestBackboneRouterService(thread_cert.TestCase):
    TOPOLOGY = {
        LEADER_1_1: {
            'version': '1.1',
            'allowlist': [BBR_1, BBR_2],
        },
        BBR_1: {
            'version': '1.2',
            'allowlist': [LEADER_1_1, BBR_2],
            'is_bbr': True
        },
        BBR_2: {
            'version': '1.2',
            'allowlist': [LEADER_1_1, BBR_1],
            'is_bbr': True
        },
    }
    """All nodes are created with default configurations"""

    def test(self):

        self.nodes[LEADER_1_1].start()
        WAIT_TIME = WAIT_ATTACH
        self.simulator.go(WAIT_TIME)
        self.assertEqual(self.nodes[LEADER_1_1].get_state(), 'leader')
        self.simulator.set_lowpan_context(1, config.DOMAIN_PREFIX)

        # 1) First Backbone Router would become the Primary.
        self.nodes[BBR_1].set_router_selection_jitter(ROUTER_SELECTION_JITTER)
        self.nodes[BBR_1].set_bbr_registration_jitter(BBR_REGISTRATION_JITTER)
        self.nodes[BBR_1].set_backbone_router(seqno=1)
        self.nodes[BBR_1].start()
        WAIT_TIME = WAIT_ATTACH + ROUTER_SELECTION_JITTER
        self.simulator.go(WAIT_TIME)
        self.assertEqual(self.nodes[BBR_1].get_state(), 'router')
        self.nodes[BBR_1].enable_backbone_router()
        WAIT_TIME = BBR_REGISTRATION_JITTER + WAIT_REDUNDANCE
        self.simulator.go(WAIT_TIME)
        self.assertEqual(self.nodes[BBR_1].get_backbone_router_state(), 'Primary')

        self.nodes[BBR_1].set_domain_prefix(config.DOMAIN_PREFIX)
        WAIT_TIME = WAIT_REDUNDANCE
        self.simulator.go(WAIT_TIME)

        # 2) Reset BBR_1 and bring it back soon.
        # Verify that it restores Primary State with sequence number
        # increased by 1.
        self.nodes[BBR_1].reset()
        self.nodes[BBR_1].set_bbr_registration_jitter(BBR_REGISTRATION_JITTER)
        self.nodes[BBR_1].set_router_selection_jitter(ROUTER_SELECTION_JITTER)
        self.nodes[BBR_1].set_domain_prefix(config.DOMAIN_PREFIX)
        self.nodes[BBR_1].enable_backbone_router()
        self.nodes[BBR_1].start()
        WAIT_TIME = WAIT_ATTACH + ROUTER_SELECTION_JITTER
        self.simulator.go(WAIT_TIME)
        self.assertEqual(self.nodes[BBR_1].get_state(), 'router')
        WAIT_TIME = BBR_REGISTRATION_JITTER + WAIT_REDUNDANCE
        self.simulator.go(WAIT_TIME)
        self.assertEqual(self.nodes[BBR_1].get_backbone_router_state(), 'Primary')
        assert self.nodes[BBR_1].get_backbone_router()['seqno'] == 2

        # 3) Reset BBR_1 and bring it back after its original router id is released
        # 200s (100s MaxNeighborAge + 90s InfiniteCost + 10s redundance)
        # Verify it becomes Primary again.
        # Note: To ensure test in next step, here Step 3) will repeat until
        # the random sequence number is not the highest value 255.
        while True:
            self.nodes[BBR_1].reset()
            WAIT_TIME = 200
            self.simulator.go(WAIT_TIME)
            self.nodes[BBR_1].set_router_selection_jitter(ROUTER_SELECTION_JITTER)
            self.nodes[BBR_1].set_bbr_registration_jitter(BBR_REGISTRATION_JITTER)
            self.nodes[BBR_1].set_domain_prefix(config.DOMAIN_PREFIX)
            self.nodes[BBR_1].enable_backbone_router()
            self.nodes[BBR_1].start()
            WAIT_TIME = WAIT_ATTACH + ROUTER_SELECTION_JITTER
            self.simulator.go(WAIT_TIME)
            self.assertEqual(self.nodes[BBR_1].get_state(), 'router')
            WAIT_TIME = BBR_REGISTRATION_JITTER + WAIT_REDUNDANCE
            self.simulator.go(WAIT_TIME)
            self.assertEqual(self.nodes[BBR_1].get_backbone_router_state(), 'Primary')
            BBR_1_SEQNO = self.nodes[BBR_1].get_backbone_router()['seqno']
            if (BBR_1_SEQNO != 255):
                break

        #4) Configure BBR_2 with highest sequence number (255) and
        #   explicitly trigger SRV_DATA.ntf.
        #   Verify BBR_2 would become Primary and BBR_1 would change to
        #   Secondary with sequence number increased by 1.

        # Bring up BBR_2, it becomes Router with backbone function disabled
        # by default.
        self.nodes[BBR_2].set_router_selection_jitter(ROUTER_SELECTION_JITTER)
        self.nodes[BBR_2].set_bbr_registration_jitter(BBR_REGISTRATION_JITTER)
        self.nodes[BBR_2].set_domain_prefix(config.DOMAIN_PREFIX)
        self.nodes[BBR_2].start()
        WAIT_TIME = WAIT_ATTACH + ROUTER_SELECTION_JITTER
        self.simulator.go(WAIT_TIME)
        self.assertEqual(self.nodes[BBR_2].get_state(), 'router')
        WAIT_TIME = BBR_REGISTRATION_JITTER + WAIT_REDUNDANCE
        self.simulator.go(WAIT_TIME)
        self.assertEqual(self.nodes[BBR_2].get_backbone_router_state(), 'Disabled')

        # Enable Backbone function, it will stay at Secondary state as
        # there is Primary Backbone Router already.
        # Here removes the Domain Prefix before enabling backbone function
        # intentionally to avoid SRV_DATA.ntf due to prefix inconsistency.
        self.nodes[BBR_2].remove_domain_prefix(config.DOMAIN_PREFIX)
        self.nodes[BBR_2].enable_backbone_router()
        self.nodes[BBR_2].set_backbone_router(seqno=255)
        WAIT_TIME = BBR_REGISTRATION_JITTER + WAIT_REDUNDANCE
        self.simulator.go(WAIT_TIME)
        self.assertEqual(self.nodes[BBR_2].get_backbone_router_state(), 'Secondary')

        # Check no SRV_DATA.ntf.
        messages = self.simulator.get_messages_sent_by(BBR_2)
        msg = messages.next_coap_message('0.02', '/a/sd', False)
        assert (msg is None), "Error: %d sent unexpected SRV_DATA.ntf when there is PBbr already"

        # Flush relative message queue.
        self.flush_nodes([BBR_1])

        # BBR_2 registers SRV_DATA.ntf explicitly.
        self.nodes[BBR_2].register_backbone_router()
        WAIT_TIME = WAIT_REDUNDANCE
        self.simulator.go(WAIT_TIME)
        self.assertEqual(self.nodes[BBR_2].get_backbone_router_state(), 'Primary')

        # Verify BBR_1 becomes Secondary and sends SRV_DATA.ntf to deregister
        # its service.
        messages = self.simulator.get_messages_sent_by(BBR_1)
        messages.next_coap_message('0.02', '/a/sd', True)
        self.assertEqual(self.nodes[BBR_1].get_backbone_router_state(), 'Secondary')
        # Verify Sequence number increases when become Secondary from Primary.
        assert self.nodes[BBR_1].get_backbone_router()['seqno'] == (BBR_1_SEQNO + 1)

        # 4a) Check communication via DUA.
        bbr2_dua = self.nodes[BBR_2].get_addr(config.DOMAIN_PREFIX)
        self.assertTrue(self.nodes[BBR_1].ping(bbr2_dua))

        # 5) Stop BBR_2, BBR_1 becomes Primary after detecting there is no
        #    available Backbone Router Service.
        self.nodes[BBR_2].reset()
        self.nodes[LEADER_1_1].release_router_id(self.nodes[BBR_2].get_router_id())
        # Wait for the dissemination of Network Data without Backbone Router service
        self.simulator.go(10)

        # BBR_1 becomes Primary.
        self.assertEqual(self.nodes[BBR_1].get_backbone_router_state(), 'Primary')
        messages = self.simulator.get_messages_sent_by(BBR_1)
        messages.next_coap_message('0.02', '/a/sd', True)

        # 6) Bring back BBR_2.
        #    Verify that BBR_2 stays at Secondary.
        self.nodes[BBR_2].set_router_selection_jitter(ROUTER_SELECTION_JITTER)
        self.nodes[BBR_2].set_bbr_registration_jitter(BBR_REGISTRATION_JITTER)
        self.nodes[BBR_1].set_domain_prefix(config.DOMAIN_PREFIX)
        self.nodes[BBR_2].enable_backbone_router()
        self.nodes[BBR_2].interface_up()
        self.nodes[BBR_2].thread_start()
        WAIT_TIME = WAIT_ATTACH + ROUTER_SELECTION_JITTER
        self.simulator.go(WAIT_TIME)
        self.assertEqual(self.nodes[BBR_2].get_state(), 'router')
        WAIT_TIME = BBR_REGISTRATION_JITTER + WAIT_REDUNDANCE
        self.simulator.go(WAIT_TIME)
        self.assertEqual(self.nodes[BBR_2].get_backbone_router_state(), 'Secondary')

        # 6a) Check the uniqueness of DUA by comparing the one in above 4a).
        bbr2_dua2 = self.nodes[BBR_2].get_addr(config.DOMAIN_PREFIX)
        assert bbr2_dua == bbr2_dua2, 'Error: Unexpected different DUA ({} v.s. {})'.format(bbr2_dua, bbr2_dua2)

        # 6b) Check communication via DUA
        self.assertTrue(self.nodes[BBR_1].ping(bbr2_dua))


if __name__ == '__main__':
    unittest.main()
