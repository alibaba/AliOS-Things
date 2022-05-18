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
import logging
import unittest

import config
import thread_cert

# Test description:
#   This test verifies bi-directional connectivity between Thread end device
#   and infra host.
#
# Topology:
#    ----------------(eth)--------------------
#           |                 |
#          BR (Leader)      HOST
#           |
#        ROUTER
#

BR = 1
ROUTER = 2
HOST = 3

# The two prefixes are set small enough that a random-generated OMR prefix is
# very likely greater than them. So that the BR will remove the random-generated one.
ON_MESH_PREFIX1 = "fd00:00:00:01::/64"
ON_MESH_PREFIX2 = "fd00:00:00:02::/64"


class SingleBorderRouter(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False

    TOPOLOGY = {
        BR: {
            'name': 'BR',
            'allowlist': [ROUTER],
            'is_otbr': True,
            'version': '1.2',
        },
        ROUTER: {
            'name': 'Router',
            'allowlist': [BR],
            'version': '1.2',
        },
        HOST: {
            'name': 'Host',
            'is_host': True
        },
    }

    def test(self):
        br = self.nodes[BR]
        router = self.nodes[ROUTER]
        host = self.nodes[HOST]

        host.start(start_radvd=False)
        self.simulator.go(5)

        br.start()
        self.simulator.go(5)
        self.assertEqual('leader', br.get_state())

        router.start()
        self.simulator.go(5)
        self.assertEqual('router', router.get_state())

        #
        # Case 1. There is no OMR prefix or on-link prefix.
        #

        self.simulator.go(10)
        self.collect_ipaddrs()

        logging.info("BR      addrs: %r", br.get_addrs())
        logging.info("ROUTER  addrs: %r", router.get_addrs())
        logging.info("HOST    addrs: %r", host.get_addrs())

        self.assertEqual(len(br.get_prefixes()), 1)
        self.assertEqual(len(router.get_prefixes()), 1)
        self.assertEqual(len(br.get_routes()), 1)
        self.assertEqual(len(router.get_routes()), 1)

        omr_prefix = br.get_prefixes()[0]
        external_route = br.get_routes()[0]

        self.assertEqual(len(br.get_ip6_address(config.ADDRESS_TYPE.OMR)), 1)
        self.assertEqual(len(router.get_ip6_address(config.ADDRESS_TYPE.OMR)), 1)
        self.assertEqual(len(host.get_ip6_address(config.ADDRESS_TYPE.ONLINK_ULA)), 1)

        br1_omr_address = br.get_ip6_address(config.ADDRESS_TYPE.OMR)[0]
        router1_omr_address = router.get_ip6_address(config.ADDRESS_TYPE.OMR)[0]
        host_ula_address = host.get_ip6_address(config.ADDRESS_TYPE.ONLINK_ULA)[0]

        # Router1 can ping to/from the Host on infra link.
        self.assertTrue(router.ping(host.get_ip6_address(config.ADDRESS_TYPE.ONLINK_ULA)[0]))
        self.assertTrue(host.ping(router.get_ip6_address(config.ADDRESS_TYPE.OMR)[0], backbone=True))

        #
        # Case 2. User adds smaller on-mesh prefix.
        #         1. Should deregister our local OMR prefix.
        #         2. Should re-register our local OMR prefix when user prefix
        #            is removed.
        #

        br.add_prefix(ON_MESH_PREFIX1)
        br.add_prefix(ON_MESH_PREFIX2)
        br.register_netdata()

        self.simulator.go(10)
        self.collect_ipaddrs()

        logging.info("BR     addrs: %r", br.get_addrs())
        logging.info("ROUTER addrs: %r", router.get_addrs())
        logging.info("HOST    addrs: %r", host.get_addrs())

        self.assertGreaterEqual(len(host.get_addrs()), 2)

        self.assertEqual(len(br.get_prefixes()), 2)
        self.assertEqual(len(router.get_prefixes()), 2)
        self.assertEqual(len(br.get_routes()), 1)
        self.assertEqual(len(router.get_routes()), 1)

        self.assertEqual(len(br.get_ip6_address(config.ADDRESS_TYPE.OMR)), 2)
        self.assertEqual(len(router.get_ip6_address(config.ADDRESS_TYPE.OMR)), 2)
        self.assertEqual(len(host.get_ip6_address(config.ADDRESS_TYPE.ONLINK_ULA)), 1)

        # Router1 can ping to/from the Host on infra link.
        self.assertTrue(router.ping(host.get_ip6_address(config.ADDRESS_TYPE.ONLINK_ULA)[0]))
        self.assertTrue(host.ping(router.get_ip6_address(config.ADDRESS_TYPE.OMR)[0], backbone=True))
        self.assertTrue(host.ping(router.get_ip6_address(config.ADDRESS_TYPE.OMR)[1], backbone=True))

        # Remove user prefixes, should re-register local OMR prefix.
        br.remove_prefix(ON_MESH_PREFIX1)
        br.remove_prefix(ON_MESH_PREFIX2)
        br.register_netdata()

        self.simulator.go(10)
        self.collect_ipaddrs()

        logging.info("BR     addrs: %r", br.get_addrs())
        logging.info("ROUTER addrs: %r", router.get_addrs())
        logging.info("HOST    addrs: %r", host.get_addrs())

        self.assertEqual(len(br.get_prefixes()), 1)
        self.assertEqual(len(router.get_prefixes()), 1)
        self.assertEqual(len(br.get_routes()), 1)
        self.assertEqual(len(router.get_routes()), 1)

        # The same local OMR and on-link prefix should be re-register.
        self.assertEqual(br.get_prefixes(), [omr_prefix])
        self.assertEqual(router.get_prefixes(), [omr_prefix])
        self.assertEqual(br.get_routes(), [external_route])
        self.assertEqual(router.get_routes(), [external_route])

        self.assertEqual(len(br.get_ip6_address(config.ADDRESS_TYPE.OMR)), 1)
        self.assertEqual(len(router.get_ip6_address(config.ADDRESS_TYPE.OMR)), 1)
        self.assertEqual(len(host.get_ip6_address(config.ADDRESS_TYPE.ONLINK_ULA)), 1)

        self.assertEqual(br.get_ip6_address(config.ADDRESS_TYPE.OMR), [br1_omr_address])
        self.assertEqual(router.get_ip6_address(config.ADDRESS_TYPE.OMR), [router1_omr_address])
        self.assertEqual(host.get_ip6_address(config.ADDRESS_TYPE.ONLINK_ULA), [host_ula_address])

        # Router1 can ping to/from the Host on infra link.
        self.assertTrue(router.ping(host.get_ip6_address(config.ADDRESS_TYPE.ONLINK_ULA)[0]))
        self.assertTrue(host.ping(router.get_ip6_address(config.ADDRESS_TYPE.OMR)[0], backbone=True))

        #
        # Case 3. OMR and on-link prefixes should be removed when Border Routing is
        #         explicitly disabled and added when Border Routing is enabled again.
        #

        br.disable_br()

        self.simulator.go(10)
        self.collect_ipaddrs()

        logging.info("BR     addrs: %r", br.get_addrs())
        logging.info("ROUTER addrs: %r", router.get_addrs())
        logging.info("HOST    addrs: %r", host.get_addrs())

        self.assertEqual(len(br.get_prefixes()), 0)
        self.assertEqual(len(router.get_prefixes()), 0)
        self.assertEqual(len(br.get_routes()), 0)
        self.assertEqual(len(router.get_routes()), 0)
        self.assertEqual(len(br.get_ip6_address(config.ADDRESS_TYPE.OMR)), 0)
        self.assertEqual(len(router.get_ip6_address(config.ADDRESS_TYPE.OMR)), 0)

        br.enable_br()

        # It takes around 10 seconds to start sending RA messages.
        self.simulator.go(15)
        self.collect_ipaddrs()

        logging.info("BR     addrs: %r", br.get_addrs())
        logging.info("ROUTER addrs: %r", router.get_addrs())
        logging.info("HOST    addrs: %r", host.get_addrs())

        self.assertEqual(len(br.get_prefixes()), 1)
        self.assertEqual(len(router.get_prefixes()), 1)
        self.assertEqual(len(br.get_routes()), 1)
        self.assertEqual(len(router.get_routes()), 1)

        # The same local OMR and on-link prefix should be re-registered.
        self.assertEqual(br.get_prefixes(), [omr_prefix])
        self.assertEqual(router.get_prefixes(), [omr_prefix])
        self.assertEqual(br.get_routes(), [external_route])
        self.assertEqual(router.get_routes(), [external_route])

        self.assertEqual(len(br.get_ip6_address(config.ADDRESS_TYPE.OMR)), 1)
        self.assertEqual(len(router.get_ip6_address(config.ADDRESS_TYPE.OMR)), 1)
        self.assertEqual(len(host.get_ip6_address(config.ADDRESS_TYPE.ONLINK_ULA)), 1)

        self.assertEqual(br.get_ip6_address(config.ADDRESS_TYPE.OMR), [br1_omr_address])
        self.assertEqual(router.get_ip6_address(config.ADDRESS_TYPE.OMR), [router1_omr_address])
        self.assertEqual(host.get_ip6_address(config.ADDRESS_TYPE.ONLINK_ULA), [host_ula_address])

        # Router1 can ping to/from the Host on infra link.
        self.assertTrue(router.ping(host.get_ip6_address(config.ADDRESS_TYPE.ONLINK_ULA)[0]))
        self.assertTrue(host.ping(router.get_ip6_address(config.ADDRESS_TYPE.OMR)[0], backbone=True))

        #
        # Case 4. The Routing Manager should be stopped if the infra interface went down.
        #

        br.disable_ether()

        self.simulator.go(10)
        self.collect_ipaddrs()

        logging.info("BR     addrs: %r", br.get_addrs())
        logging.info("ROUTER addrs: %r", router.get_addrs())
        logging.info("HOST    addrs: %r", host.get_addrs())

        self.assertEqual(len(br.get_prefixes()), 0)
        self.assertEqual(len(router.get_prefixes()), 0)
        self.assertEqual(len(br.get_routes()), 0)
        self.assertEqual(len(router.get_routes()), 0)
        self.assertEqual(len(br.get_ip6_address(config.ADDRESS_TYPE.OMR)), 0)
        self.assertEqual(len(router.get_ip6_address(config.ADDRESS_TYPE.OMR)), 0)

        br.enable_ether()

        # The routing manager may fail to send RS and will wait for 60 seconds
        # before retrying.
        self.simulator.go(80)
        self.collect_ipaddrs()

        logging.info("BR     addrs: %r", br.get_addrs())
        logging.info("ROUTER addrs: %r", router.get_addrs())
        logging.info("HOST    addrs: %r", host.get_addrs())

        self.assertEqual(len(br.get_prefixes()), 1)
        self.assertEqual(len(router.get_prefixes()), 1)
        self.assertEqual(len(br.get_routes()), 1)
        self.assertEqual(len(router.get_routes()), 1)

        # The same local OMR and on-link prefix should be re-registered.
        self.assertEqual(br.get_prefixes(), [omr_prefix])
        self.assertEqual(router.get_prefixes(), [omr_prefix])
        self.assertEqual(br.get_routes(), [external_route])
        self.assertEqual(router.get_routes(), [external_route])

        self.assertEqual(len(br.get_ip6_address(config.ADDRESS_TYPE.OMR)), 1)
        self.assertEqual(len(router.get_ip6_address(config.ADDRESS_TYPE.OMR)), 1)
        self.assertEqual(len(host.get_ip6_address(config.ADDRESS_TYPE.ONLINK_ULA)), 1)

        self.assertEqual(br.get_ip6_address(config.ADDRESS_TYPE.OMR), [br1_omr_address])
        self.assertEqual(router.get_ip6_address(config.ADDRESS_TYPE.OMR), [router1_omr_address])
        self.assertEqual(host.get_ip6_address(config.ADDRESS_TYPE.ONLINK_ULA), [host_ula_address])

        # Router1 can ping to/from the Host on infra link.
        self.assertTrue(router.ping(host.get_ip6_address(config.ADDRESS_TYPE.ONLINK_ULA)[0]))
        self.assertTrue(host.ping(router.get_ip6_address(config.ADDRESS_TYPE.OMR)[0], backbone=True))

        #
        # Case 5. Test if the linux host is still reachable if rejoin the network.
        #

        host.disable_ether()
        self.simulator.go(10)
        host.enable_ether()
        self.simulator.go(10)

        self.assertTrue(router.ping(host.get_ip6_address(config.ADDRESS_TYPE.ONLINK_ULA)[0]))
        self.assertTrue(host.ping(router.get_ip6_address(config.ADDRESS_TYPE.OMR)[0], backbone=True))

        #
        # Case 6. Test if the Border Router will remove the on-link prefix when
        #         another RA daemon is started on the same infra interface.
        #

        br.start_radvd_service(prefix=config.ONLINK_GUA_PREFIX, slaac=True)
        self.simulator.go(5)

        self.assertEqual(len(br.get_routes()), 2)
        self.assertEqual(len(router.get_routes()), 2)

        self.assertTrue(router.ping(host.get_ip6_address(config.ADDRESS_TYPE.ONLINK_GUA)[0]))
        self.assertTrue(router.ping(host.get_ip6_address(config.ADDRESS_TYPE.ONLINK_ULA)[0]))
        self.assertTrue(host.ping(router.get_ip6_address(config.ADDRESS_TYPE.OMR)[0], backbone=True))


if __name__ == '__main__':
    unittest.main()
