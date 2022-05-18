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
import ipaddress
import logging
import unittest

import config
import thread_cert

# Test description:
#   This test verifies that OTBR publishes the meshcop service using a proper
#   configuration.
#
# Topology:
#    ----------------(eth)-----------------------------
#           |             |                    |
#          BR1           BR2           HOST (mDNS Browser)
#
#

BR1 = 1
BR2 = 2
HOST = 3


class PublishMeshCopService(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False

    TOPOLOGY = {
        BR1: {
            'name': 'BR_1',
            'allowlist': [],
            'is_otbr': True,
            'version': '1.2',
        },
        BR2: {
            'name': 'BR_2',
            'allowlist': [],
            'is_otbr': True,
            'version': '1.2',
        },
        HOST: {
            'name': 'Host',
            'is_host': True
        },
    }

    def test(self):
        host = self.nodes[HOST]
        br1 = self.nodes[BR1]
        br2 = self.nodes[BR2]
        br2.disable_br()

        # TODO: verify the behavior when thread is disabled
        host.bash('service otbr-agent stop')
        host.start(start_radvd=False)
        self.simulator.go(10)

        br1.start()
        self.simulator.go(10)
        self.assertEqual('leader', br1.get_state())

        self.check_meshcop_service(br1, host)

        br1.disable_backbone_router()
        self.simulator.go(10)
        self.check_meshcop_service(br1, host)

        # verify that there are two meshcop services
        br2.start()
        br2.disable_backbone_router()
        br2.enable_br()
        self.simulator.go(10)
        service_instances = host.browse_mdns_services('_meshcop._udp')
        self.assertEqual(len(service_instances), 2)
        br1_service = self.check_meshcop_service(br1, host, 'OpenThread_BorderRouter')
        for instance in service_instances:
            if instance != 'OpenThread_BorderRouter':
                br2_service = self.check_meshcop_service(br2, host, instance)
        self.assertNotEqual(br1_service['host'], br2_service['host'])

    def check_meshcop_service(self, br, host, instance_name='OpenThread_BorderRouter'):
        data = host.discover_mdns_service(instance_name, '_meshcop._udp', None)
        sb_data = data['txt']['sb'].encode('raw_unicode_escape')
        state_bitmap = int.from_bytes(sb_data, byteorder='big')
        logging.info(bin(state_bitmap))
        self.assertEqual((state_bitmap & 7), 1)  # connection mode = PskC
        if br.get_state() == 'disabled':
            self.assertEqual((state_bitmap >> 3 & 3), 0)  # Thread is disabled
        elif br.get_state() == 'detached':
            self.assertEqual((state_bitmap >> 3 & 3), 1)  # Thread is detached
        else:
            self.assertEqual((state_bitmap >> 3 & 3), 2)  # Thread is attached
        self.assertEqual((state_bitmap >> 5 & 3), 1)  # high availability
        self.assertEqual((state_bitmap >> 7 & 1),
                         br.get_backbone_router_state() != 'Disabled')  # BBR is enabled or not
        self.assertEqual((state_bitmap >> 8 & 1), br.get_backbone_router_state() == 'Primary')  # BBR is primary or not
        self.assertEqual(data['txt']['nn'], br.get_network_name())
        self.assertEqual(data['txt']['rv'], '1')
        self.assertIn(data['txt']['tv'], ['1.1.0', '1.1.1', '1.2.0'])
        return data


if __name__ == '__main__':
    unittest.main()
