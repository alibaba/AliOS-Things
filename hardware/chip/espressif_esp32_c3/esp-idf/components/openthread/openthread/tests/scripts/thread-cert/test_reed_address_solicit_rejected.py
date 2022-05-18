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

import config
import re
import unittest

import thread_cert

LEADER = 1
REED = 2

SRV_0_ID = 0
SRV_0_ENT_NUMBER = '123'
SRV_0_SERVICE_DATA = '123'
SRV_0_SERVER_DATA = 'abc'

# Topology:
# LEADER -- REED
#


class TestREEDAddressSolicitRejected(thread_cert.TestCase):
    SUPPORT_NCP = False

    TOPOLOGY = {
        LEADER: {
            'mode': 'rdn',
        },
        REED: {
            'mode': 'rdn',
        },
    }

    def testAddressSolicitRejectedBeforeSvrData(self):
        self.nodes[LEADER].start()
        self.nodes[LEADER].set_router_upgrade_threshold(1)
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        self.nodes[REED].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[REED].get_state(), 'child')

        self.nodes[REED].add_service(SRV_0_ENT_NUMBER, SRV_0_SERVICE_DATA, SRV_0_SERVER_DATA)
        self.nodes[REED].register_netdata()
        self.simulator.go(2)

        self.assertEqual(self.hasAloc(REED, SRV_0_ID), True)

    def testAddressSolicitRejectedAfterSvrData(self):
        self.nodes[LEADER].start()
        self.nodes[LEADER].set_router_upgrade_threshold(1)
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        self.nodes[REED].set_router_selection_jitter(120)
        self.nodes[REED].start()
        # set routerupgradethreshold=1 on REED so that it won't send ADDR_SOL.req
        self.nodes[REED].set_router_upgrade_threshold(1)
        self.simulator.go(5)
        self.assertEqual(self.nodes[REED].get_state(), 'child')

        # restore routerupgradethreshold to 16 and add service
        self.nodes[REED].set_router_upgrade_threshold(16)
        self.nodes[REED].add_service(SRV_0_ENT_NUMBER, SRV_0_SERVICE_DATA, SRV_0_SERVER_DATA)
        self.nodes[REED].register_netdata()
        self.simulator.go(130)
        self.assertEqual(self.hasAloc(REED, SRV_0_ID), True)

    def hasAloc(self, node_id, service_id):
        for addr in self.nodes[node_id].get_ip6_address(config.ADDRESS_TYPE.ALOC):
            m = re.match('.*:fc(..)$', addr, re.I)
            if m is not None:
                if m.group(1) == str(service_id + 10):  # for service_id=3 look for '...:fc13'
                    return True

        return False


if __name__ == '__main__':
    unittest.main()
