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

import ipaddress
import logging
import time
import unittest
from typing import Union, List

import config
import network_layer
import thread_cert

logging.basicConfig(level=logging.DEBUG)

_, BBR_1, BBR_2, ROUTER_1_2, ROUTER_1_1, SED_1, MED_1, MED_2, FED_1 = range(9)

WAIT_ATTACH = 5
WAIT_REDUNDANCE = 3
ROUTER_SELECTION_JITTER = 1
BBR_REGISTRATION_JITTER = 5
SED_POLL_PERIOD = 1000  # 1s

REREG_DELAY = 10
MLR_TIMEOUT = 300
PARENT_AGGREGATE_DELAY = 5

MA1 = 'ff04::1234:777a:1'
MA1g = 'ff0e::1234:777a:1'
MA2 = 'ff05::1234:777a:2'
MA3 = 'ff0e::1234:777a:3'
MA4 = 'ff05::1234:777a:4'

MA5 = 'ff03::1234:777a:5'
MA6 = 'ff02::10'
"""
 Initial topology:

   BBR_1---BBR_2
     |     /   \                       |
     |    /     \                      |
   ROUTER_1_2  ROUTER_1_1
    |     |  \____                     |
    |     |       \                    |
  SED_1  MED_1/2  FED_1
"""


class TestMulticastListenerRegistration(thread_cert.TestCase):
    TOPOLOGY = {
        BBR_1: {
            'version': '1.2',
            'allowlist': [BBR_2, ROUTER_1_2],
            'is_bbr': True,
        },
        BBR_2: {
            'version': '1.2',
            'allowlist': [BBR_1, ROUTER_1_2, ROUTER_1_1],
            'is_bbr': True,
        },
        ROUTER_1_2: {
            'version': '1.2',
            'allowlist': [BBR_1, BBR_2, SED_1, MED_1, MED_2, FED_1],
        },
        ROUTER_1_1: {
            'version': '1.1',
            'allowlist': [BBR_2],
        },
        MED_1: {
            'mode': 'rn',
            'version': '1.2',
            'allowlist': [ROUTER_1_2],
            'timeout': config.DEFAULT_CHILD_TIMEOUT,
        },
        MED_2: {
            'mode': 'rn',
            'version': '1.2',
            'allowlist': [ROUTER_1_2],
            'timeout': config.DEFAULT_CHILD_TIMEOUT,
        },
        SED_1: {
            'mode': 'n',
            'version': '1.2',
            'allowlist': [ROUTER_1_2],
            'timeout': config.DEFAULT_CHILD_TIMEOUT,
        },
        FED_1: {
            'mode': 'rdn',
            'version': '1.2',
            'allowlist': [ROUTER_1_2],
            'router_upgrade_threshold': 0,
            'timeout': config.DEFAULT_CHILD_TIMEOUT,
        },
    }
    """All nodes are created with default configurations"""

    def _bootstrap(self, bbr_1_enable_backbone_router=True, turn_on_bbr_2=True, turn_on_router_1_1=True):
        assert (turn_on_bbr_2 or not turn_on_router_1_1)  # ROUTER_1_1 needs BBR_2

        # starting context id
        t0 = time.time()
        context_id = 1

        # Bring up BBR_1, BBR_1 becomes Leader and Primary Backbone Router
        self.nodes[BBR_1].set_router_selection_jitter(ROUTER_SELECTION_JITTER)
        self.nodes[BBR_1].set_bbr_registration_jitter(BBR_REGISTRATION_JITTER)

        self.nodes[BBR_1].set_backbone_router(seqno=1, reg_delay=REREG_DELAY, mlr_timeout=MLR_TIMEOUT)
        self.nodes[BBR_1].start()
        WAIT_TIME = WAIT_ATTACH + ROUTER_SELECTION_JITTER
        self.simulator.go(WAIT_TIME)
        self.assertEqual(self.nodes[BBR_1].get_state(), 'leader')

        if bbr_1_enable_backbone_router:
            self.nodes[BBR_1].enable_backbone_router()
            WAIT_TIME = BBR_REGISTRATION_JITTER + WAIT_REDUNDANCE
            self.simulator.go(WAIT_TIME)
            self.assertEqual(self.nodes[BBR_1].get_backbone_router_state(), 'Primary')

        self.pbbr_seq = 1
        self.pbbr_id = BBR_1

        if turn_on_bbr_2:
            # Bring up BBR_2, BBR_2 becomes Router and Secondary Backbone Router
            self.nodes[BBR_2].set_router_selection_jitter(ROUTER_SELECTION_JITTER)
            self.nodes[BBR_2].set_bbr_registration_jitter(BBR_REGISTRATION_JITTER)
            self.nodes[BBR_2].set_backbone_router(seqno=2, reg_delay=REREG_DELAY, mlr_timeout=MLR_TIMEOUT)
            self.nodes[BBR_2].start()
            WAIT_TIME = WAIT_ATTACH + ROUTER_SELECTION_JITTER
            self.simulator.go(WAIT_TIME)
            self.assertEqual(self.nodes[BBR_2].get_state(), 'router')
            self.nodes[BBR_2].enable_backbone_router()
            WAIT_TIME = BBR_REGISTRATION_JITTER + WAIT_REDUNDANCE
            self.simulator.go(WAIT_TIME)
            self.assertEqual(self.nodes[BBR_2].get_backbone_router_state(), 'Secondary')

        self.simulator.set_lowpan_context(context_id, config.DOMAIN_PREFIX)
        domain_prefix_cid = context_id

        # Bring up ROUTER_1_2
        self.nodes[ROUTER_1_2].set_router_selection_jitter(ROUTER_SELECTION_JITTER)
        self.nodes[ROUTER_1_2].start()
        WAIT_TIME = WAIT_ATTACH + ROUTER_SELECTION_JITTER
        self.simulator.go(WAIT_TIME)
        self.assertEqual(self.nodes[ROUTER_1_2].get_state(), 'router')

        if turn_on_router_1_1:
            # Bring up ROUTER_1_1
            self.nodes[ROUTER_1_1].set_router_selection_jitter(ROUTER_SELECTION_JITTER)
            self.nodes[ROUTER_1_1].start()
            WAIT_TIME = WAIT_ATTACH + ROUTER_SELECTION_JITTER
            self.simulator.go(WAIT_TIME)
            self.assertEqual(self.nodes[ROUTER_1_1].get_state(), 'router')

        # Bring up FED_1
        self.nodes[FED_1].start()
        self.simulator.go(WAIT_ATTACH)
        self.assertEqual(self.nodes[FED_1].get_state(), 'child')

        # Bring up MED_1
        self.nodes[MED_1].start()
        self.simulator.go(WAIT_ATTACH)
        self.assertEqual(self.nodes[MED_1].get_state(), 'child')

        # Bring up MED_2
        self.nodes[MED_2].start()
        self.simulator.go(WAIT_ATTACH)
        self.assertEqual(self.nodes[MED_2].get_state(), 'child')

        # Bring up SED_1
        self.nodes[SED_1].set_pollperiod(SED_POLL_PERIOD)
        self.nodes[SED_1].start()
        self.simulator.go(WAIT_ATTACH)
        self.assertEqual(self.nodes[SED_1].get_state(), 'child')

        logging.info("bootstrap takes %f seconds", time.time() - t0)

    def test(self):
        self._bootstrap()

        # Verify MLR.req for each device when parent is 1.2
        self.__check_mlr_ok(ROUTER_1_2, is_ftd=True)
        self.__check_mlr_ok(FED_1, is_ftd=True)
        self.__check_mlr_ok(MED_1, is_ftd=False)
        self.__check_mlr_ok(SED_1, is_ftd=False)

        # Switch to parent 1.1
        self.__switch_to_1_1_parent()

        # Verify MLR.req for each device when parent is 1.1
        self.__check_mlr_ok(FED_1, is_ftd=True, is_parent_1p1=True)
        self.__check_mlr_ok(MED_1, is_ftd=False, is_parent_1p1=True)
        self.__check_mlr_ok(SED_1, is_ftd=False, is_parent_1p1=True)

        # Switch to parent 1.2
        self.__switch_to_1_2_parent()

    def testParentMergeMedMlrReq(self):
        self._bootstrap()

        # Make sure Parent registers multiple MAs of MED Children in one MLR.req
        self.__check_parent_merge_med_mlr_req([MED_1, MED_2], ROUTER_1_2)

    def testNotSendMlrReqIfSubscribed(self):
        self._bootstrap()

        # Make sure Parent does not send MLR.req of Child if it's already subscribed by Netif or other Children
        self.__check_not_send_mlr_req_if_subscribed([MED_1, MED_2], ROUTER_1_2)

    def testIpmaddrAddBeforeBBREnable(self):
        self._bootstrap(bbr_1_enable_backbone_router=False, turn_on_bbr_2=False, turn_on_router_1_1=False)

        self.flush_all()

        # Subscribing to MAs when there is no PBBR should not send MLR.req
        self.nodes[ROUTER_1_2].add_ipmaddr("ff04::1")
        self.nodes[FED_1].add_ipmaddr("ff04::2")
        self.nodes[MED_1].add_ipmaddr("ff04::3")
        self.nodes[SED_1].add_ipmaddr("ff04::4")

        self.simulator.go(PARENT_AGGREGATE_DELAY + WAIT_REDUNDANCE)
        router_reg = ["ff04::1", "ff04::2", "ff04::3", "ff04::4"]
        self.__check_send_mlr_req(ROUTER_1_2, router_reg, should_send=False)

        self.flush_all()

        # Turn on PBBR
        self.nodes[BBR_1].enable_backbone_router()

        WAIT_TIME = BBR_REGISTRATION_JITTER + WAIT_REDUNDANCE
        self.simulator.go(WAIT_TIME)
        self.assertEqual(self.nodes[BBR_1].get_backbone_router_state(), 'Primary')

        self.simulator.go(REREG_DELAY)
        # Expect MLR.req sent by ROUTER_1_2 and FED_1
        self.__check_send_mlr_req(ROUTER_1_2, router_reg, should_send=True, expect_mlr_rsp=True)

    def testMulticastListenersTableAdd(self):
        self._bootstrap()
        self.__test_multicast_listeners_table_add()

    def testMulticastListenersTableExpire(self):
        self._bootstrap()
        self.__test_multicast_listeners_table_expire()

    def testMulticastListenersTableFull(self):
        self._bootstrap()
        self.__test_multicast_listeners_table_full()

    def testMulticastListenersTableTwoFreeSlot(self):
        self._bootstrap()
        self.__test_multicast_listeners_table_two_free_slots()

    def testMulticastListenerTableAPI(self):
        self._bootstrap()
        self.__test_multicast_listeners_table_api()

    def testMlrConfigResponse(self):
        self._bootstrap()
        self.__test_mlr_config_response()

    def __test_mlr_config_response(self):
        bbr = self.nodes[BBR_1]
        router = self.nodes[ROUTER_1_2]

        self.flush_all()

        # Configure next response to 0
        bbr.set_next_mlr_response(0)
        router.add_ipmaddr("ff04::1")
        self.simulator.go(WAIT_REDUNDANCE)
        self.__check_send_mlr_req(ROUTER_1_2, ["ff04::1"],
                                  should_send=True,
                                  expect_mlr_rsp=True,
                                  expect_mlr_rsp_status=0)
        self.assertNotIn(ipaddress.IPv6Address("ff04::1"), bbr.multicast_listener_list())

        router.del_ipmaddr("ff04::1")
        self.simulator.go(WAIT_REDUNDANCE)
        self.flush_all()

        # Configure next response to 2
        bbr.set_next_mlr_response(2)
        router.add_ipmaddr("ff04::2")
        self.simulator.go(WAIT_REDUNDANCE)
        self.__check_send_mlr_req(ROUTER_1_2, ["ff04::2"],
                                  should_send=True,
                                  expect_mlr_rsp=True,
                                  expect_mlr_rsp_status=2)

        router.del_ipmaddr("ff04::2")
        self.simulator.go(WAIT_REDUNDANCE)
        self.flush_all()

        # Configure next response to 4
        bbr.set_next_mlr_response(4)
        router.add_ipmaddr("ff04::4")
        self.simulator.go(WAIT_REDUNDANCE)
        self.__check_send_mlr_req(ROUTER_1_2, ["ff04::4"],
                                  should_send=True,
                                  expect_mlr_rsp=True,
                                  expect_mlr_rsp_status=4)

        # The MA should be eventually registered after reregistration
        self.simulator.go(REREG_DELAY + WAIT_REDUNDANCE)
        self.assertIn(ipaddress.IPv6Address("ff04::4"), bbr.multicast_listener_list())

        router.del_ipmaddr("ff04::4")
        self.simulator.go(WAIT_REDUNDANCE)
        self.flush_all()

    def testCommissionerRegisterMulticastListeners(self):
        self._bootstrap()

        # Use ROUTER_1_2 as the Commissioner
        commissioiner = self.nodes[ROUTER_1_2]

        self.assertEqual((0, []), commissioiner.register_multicast_listener("ff04::1"))

        commissioiner.commissioner_start()
        self.simulator.go(10)

        # Now the Commissioner should be able to register MAs
        for ip in ["ff04::1", "ff04::2"]:
            status, failed_ips = commissioiner.register_multicast_listener(ip)
            self.assertTrue(status == 0 and not failed_ips)
            self.__check_multicast_listener(ip, expect_mlr_timeout_range=[290, 300])

        # Register existing MA with a new timeout should be able to update the timeout
        for ip in ["ff04::1", "ff04::2"]:
            status, failed_ips = commissioiner.register_multicast_listener(ip, timeout=1000)
            self.assertTrue(status == 0 and not failed_ips)
            self.__check_multicast_listener(ip, expect_mlr_timeout_range=[990, 1000])

        # Register MAs with given timeouts
        for ip, timeout in [("ff05::1", 400), ("ff05::2", 500), ("ff05::3", 600)]:
            status, failed_ips = commissioiner.register_multicast_listener(ip, timeout=timeout)
            self.assertTrue(status == 0 and not failed_ips)
            self.__check_multicast_listener(ip, expect_mlr_timeout_range=[timeout - 10, timeout])

        # Register multiple MAs with one call
        ips = ["ff05::4", "ff05::5", "ff05::6"]
        status, failed_ips = commissioiner.register_multicast_listener(*ips, timeout=700)
        self.assertTrue(status == 0 and not failed_ips)
        for ip in ips:
            self.__check_multicast_listener(ip, expect_mlr_timeout_range=[690, 700])

        # Register multiple MAs with one call (without timeout)
        ips = ["ff05::7", "ff05::8", "ff05::9"]
        status, failed_ips = commissioiner.register_multicast_listener(*ips)
        self.assertTrue(status == 0 and not failed_ips)
        for ip in ips:
            self.__check_multicast_listener(ip, expect_mlr_timeout_range=[290, 300])

        # Unregister MAs using timeout=0
        for ip in ["ff05::1", "ff05::2", "ff05::3"]:
            status, failed_ips = commissioiner.register_multicast_listener(ip, timeout=0)
            self.assertTrue(status == 0 and not failed_ips)
            self.__check_multicast_listener(ip, expect_not_present=True)

        # Unregister multiple MAs
        ips = ["ff05::4", "ff05::5", "ff05::6"]
        status, failed_ips = commissioiner.register_multicast_listener(*ips, timeout=0)
        self.assertTrue(status == 0 and not failed_ips)
        self.__check_multicast_listener(ip, expect_not_present=True)

        # Remove MAs that are not subscribed should not fail
        ips = ["ff06::1", "ff02::1", "2001::1"]
        status, failed_ips = commissioiner.register_multicast_listener(*ips, timeout=0)
        self.assertTrue(status == 0 and not failed_ips)
        self.__check_multicast_listener(ip, expect_not_present=True)

        # Register invalid MAs should fail
        for ip in ["ff02::1", "ff03::1", "2001::1"]:
            status, failed_ips = commissioiner.register_multicast_listener(ip)
            self.assertEqual(status, 2)
            self.assertEqual(set(failed_ips), {ipaddress.IPv6Address(ip)})
            self.__check_multicast_listener(ip, expect_not_present=True)

        # Register valid MAs with invalid MAs should succeed partially
        ips = ["ff05::1", "ff05::2", "ff02::1", "2001::1"]
        status, failed_ips = commissioiner.register_multicast_listener(*ips)
        self.assertEqual(status, 2)
        self.assertEqual(set(failed_ips), {ipaddress.IPv6Address("ff02::1"), ipaddress.IPv6Address("2001::1")})
        self.__check_multicast_listener("ff05::1")
        self.__check_multicast_listener("ff05::2")
        self.__check_multicast_listener("ff02::1", expect_not_present=True)
        self.__check_multicast_listener("2001::1", expect_not_present=True)

        # Registering persistent MAs should fail for now
        status, failed_ips = commissioiner.register_multicast_listener("ff06::1", timeout=0xffffffff)
        self.assertEqual(status, 3)
        # "ff06::1" should not be included in failed IPs because all IPs failed
        self.assertTrue(not failed_ips == 0)

    def __check_multicast_listener(self, *addrs, expect_mlr_timeout_range=None, expect_not_present=False):
        addrs = map(ipaddress.IPv6Address, addrs)
        listeners = self.nodes[BBR_1].multicast_listener_list()
        logging.info("__check_multicast_listener get listeners: %s", listeners)

        for addr in addrs:
            if not expect_not_present:
                self.assertIn(addr, listeners)
                if expect_mlr_timeout_range is not None:
                    self.assertGreaterEqual(listeners[addr], expect_mlr_timeout_range[0])
                    self.assertLessEqual(listeners[addr], expect_mlr_timeout_range[1])
            else:
                self.assertNotIn(addr, listeners)

    def __test_multicast_listeners_table_api(self):
        self.assertTrue(self.nodes[BBR_1].multicast_listener_list() == {})

        self.nodes[BBR_1].multicast_listener_add("ff04::1")
        self.assertEqual(1, len(self.nodes[BBR_1].multicast_listener_list()))
        self.nodes[BBR_1].multicast_listener_add("ff04::2", 300)
        self.assertEqual(2, len(self.nodes[BBR_1].multicast_listener_list()))
        self.nodes[BBR_1].multicast_listener_clear()
        self.assertTrue(self.nodes[BBR_1].multicast_listener_list() == {})

    def __test_multicast_listeners_table_add(self):
        self.assertTrue(self.nodes[BBR_1].multicast_listener_list() == {})

        all_mas = set()

        CHECK_LIST = [(ROUTER_1_2, "ff04::1"), (FED_1, "ff04::2"), (MED_1, "ff04::3"), (SED_1, "ff04::4")]

        for id, ip in CHECK_LIST:
            self.nodes[id].add_ipmaddr(ip)
            self.simulator.go(PARENT_AGGREGATE_DELAY + WAIT_REDUNDANCE)
            all_mas.add(ipaddress.IPv6Address(ip))
            self.assertEqual(all_mas, set(self.nodes[BBR_1].multicast_listener_list().keys()))

        # restore
        for id, ip in CHECK_LIST:
            self.nodes[id].del_ipmaddr(ip)

        self.simulator.go(WAIT_REDUNDANCE)

    def __test_multicast_listeners_table_expire(self):
        self.assertEqual({}, self.nodes[BBR_1].multicast_listener_list())

        all_mas = set()

        CHECK_LIST = [(ROUTER_1_2, "ff04::1"), (FED_1, "ff04::2"), (MED_1, "ff04::3"), (SED_1, "ff04::4")]

        for id, ip in CHECK_LIST:
            self.nodes[id].add_ipmaddr(ip)
            self.simulator.go(PARENT_AGGREGATE_DELAY + WAIT_REDUNDANCE)
            all_mas.add(ipaddress.IPv6Address(ip))
            self.assertEqual(all_mas, set(self.nodes[BBR_1].multicast_listener_list().keys()))

        # remove MAs from nodes, and wait for Multicast Listeners to expire on BBR_1
        for id, ip in CHECK_LIST:
            self.nodes[id].del_ipmaddr(ip)

        # Wait for MLR_TIMEOUT/3, and expect Multicast Listeners not to expire.
        self.simulator.go(MLR_TIMEOUT / 3)
        self.assertEqual(all_mas, set(self.nodes[BBR_1].multicast_listener_list().keys()))

        # Wait for MLR_TIMEOUT*2/3, and expect all Multicast Listeners to expire.
        self.simulator.go(MLR_TIMEOUT * 2 / 3 + WAIT_REDUNDANCE)
        self.assertEqual({}, self.nodes[BBR_1].multicast_listener_list())

    def __test_multicast_listeners_table_full(self):
        self.assertTrue(self.nodes[BBR_1].multicast_listener_list() == {})

        table = set()

        # Add to Multicast Listeners Table until it's full
        for i in range(1, 76):
            self.nodes[BBR_1].multicast_listener_add(f"ff04::{i}")
            table.add(ipaddress.IPv6Address(f"ff04::{i}"))
            self.assertEqual(table, set(self.nodes[BBR_1].multicast_listener_list().keys()))

        # Add when Multicast Listeners Table is full should not succeed
        self.nodes[BBR_1].multicast_listener_add(f"ff05::1")
        self.assertEqual(table, set(self.nodes[BBR_1].multicast_listener_list().keys()))

        self.flush_all()

        # Expect PBBR to respond with MLR_NO_RESOURCES Multicast Listeners Table when it's full
        self.nodes[ROUTER_1_2].add_ipmaddr("ff06::1")
        self.simulator.go(WAIT_REDUNDANCE)

        self.__check_send_mlr_req(ROUTER_1_2,
                                  "ff06::1",
                                  should_send=True,
                                  expect_mlr_rsp=True,
                                  expect_mlr_rsp_status=4)

        self.assertEqual(table, set(self.nodes[BBR_1].multicast_listener_list().keys()))

        self.nodes[MED_1].add_ipmaddr("ff06::2")
        self.simulator.go(PARENT_AGGREGATE_DELAY + WAIT_REDUNDANCE)

        self.__check_send_mlr_req(ROUTER_1_2,
                                  "ff06::2",
                                  should_send=True,
                                  expect_mlr_rsp=True,
                                  expect_mlr_rsp_status=4)

        self.assertEqual(table, set(self.nodes[BBR_1].multicast_listener_list().keys()))

        # the ROUTER_1_2 should be resending both ff06::1 and ff06::2
        for i in range(3):
            self.simulator.go(REREG_DELAY + WAIT_REDUNDANCE)
            self.__check_send_mlr_req(ROUTER_1_2, ['ff06::1', 'ff06::2'],
                                      should_send=True,
                                      expect_mlr_rsp=True,
                                      expect_mlr_rsp_status=4)

        # Restore
        self.nodes[ROUTER_1_2].del_ipmaddr("ff06::1")
        self.nodes[MED_1].del_ipmaddr("ff06::2")
        self.simulator.go(WAIT_REDUNDANCE)

    def __test_multicast_listeners_table_two_free_slots(self):

        # Add to Multicast Listeners Table until there is only two free slots
        for i in range(1, 74):
            self.nodes[BBR_1].multicast_listener_add(f"ff04::{i}")

        self.assertEqual(73, len(self.nodes[BBR_1].multicast_listener_list()))

        self.nodes[MED_1].add_ipmaddr("ff05::1")
        self.nodes[MED_1].add_ipmaddr("ff05::2")
        self.nodes[MED_2].add_ipmaddr("ff05::3")
        self.nodes[MED_2].add_ipmaddr("ff05::4")
        self.nodes[SED_1].add_ipmaddr("ff05::5")
        self.nodes[SED_1].add_ipmaddr("ff05::6")

        self.simulator.go(PARENT_AGGREGATE_DELAY + WAIT_REDUNDANCE)
        self.__check_send_mlr_req(ROUTER_1_2, ["ff05::1", "ff05::2", "ff05::3", "ff05::4", "ff05::5", "ff05::6"])

        self.simulator.go(PARENT_AGGREGATE_DELAY + WAIT_REDUNDANCE)
        self.flush_all()
        # two addresses should be registered, others can not
        for i in range(3):
            self.simulator.go(PARENT_AGGREGATE_DELAY + WAIT_REDUNDANCE)
            self.assertEqual(4, len(set(self.__get_registered_MAs(ROUTER_1_2))))

        # Restore
        self.nodes[MED_1].del_ipmaddr("ff05::1")
        self.nodes[MED_1].del_ipmaddr("ff05::2")
        self.nodes[MED_2].del_ipmaddr("ff05::3")
        self.nodes[MED_2].del_ipmaddr("ff05::4")
        self.nodes[SED_1].del_ipmaddr("ff05::5")
        self.nodes[SED_1].del_ipmaddr("ff05::6")

    def __check_mlr_ok(self, id, is_ftd, is_parent_1p1=False):
        """Check if MLR works for the node"""
        # Add MA1 and send MLR.req
        logging.info("======== checking MLR: Node%d (%s), Parent=%s ========" %
                     (id, 'FTD' if is_ftd else 'MTD', '1.1' if is_parent_1p1 else '1.2'))
        expect_mlr_req = is_ftd or is_parent_1p1

        if id == ROUTER_1_2:
            parent_id = None
        else:
            parent_id = ROUTER_1_1 if is_parent_1p1 else ROUTER_1_2

        for addr in [MA1, MA1g, MA2, MA3, MA4]:
            self.__check_ipmaddr_add(id,
                                     parent_id,
                                     addr,
                                     expect_mlr_req=expect_mlr_req,
                                     expect_mlr_req_proxied=(not expect_mlr_req))

        for addr in [MA5, MA6]:
            self.__check_ipmaddr_add(id, parent_id, addr, expect_mlr_req=False, expect_mlr_req_proxied=False)
        logging.info('=' * 120)

    def __check_ipmaddr_add(self, id, parent_id, addr, expect_mlr_req=True, expect_mlr_req_proxied=False):
        """Check MLR works for the added multicast address"""
        logging.info("Node %d: ipmaddr %s" % (id, addr))
        self.flush_all()
        self.nodes[id].add_ipmaddr(addr)
        self.assertTrue(self.nodes[id].has_ipmaddr(addr))
        self.simulator.go(PARENT_AGGREGATE_DELAY + WAIT_REDUNDANCE)

        self.__check_send_mlr_req(id, addr, should_send=expect_mlr_req, expect_mlr_rsp=expect_mlr_req)
        # Parent should either forward or proxy the MLR.req
        if parent_id:
            self.__check_send_mlr_req(parent_id,
                                      addr,
                                      should_send=expect_mlr_req or expect_mlr_req_proxied,
                                      expect_mlr_rsp=expect_mlr_req_proxied)

        self.__check_rereg(id,
                           parent_id,
                           addr,
                           expect_mlr_req=expect_mlr_req,
                           expect_mlr_req_proxied=expect_mlr_req_proxied)
        self.__check_renewing(id,
                              parent_id,
                              addr,
                              expect_mlr_req=expect_mlr_req,
                              expect_mlr_req_proxied=expect_mlr_req_proxied)

        self.nodes[id].del_ipmaddr(addr)
        self.simulator.go(1)

    def __check_send_mlr_req(self,
                             id,
                             addrs: Union[List[str], str],
                             should_send=True,
                             expect_mlr_rsp=False,
                             expect_mlr_rsp_status=0,
                             expect_mlr_req_num=None,
                             expect_unique_reg=False):
        if isinstance(addrs, str):
            addrs = [addrs]

        message_ids = []
        reg_mas = self.__get_registered_MAs(id, expect_mlr_req_num=expect_mlr_req_num, message_ids=message_ids)
        if should_send:
            for addr in addrs:
                self.assertIn(ipaddress.IPv6Address(addr), reg_mas)
                if expect_unique_reg:
                    self.assertEqual(1, reg_mas.count(ipaddress.IPv6Address(addr)))

            # BBR should send MLR.rsp ACK
            if expect_mlr_rsp:
                message_id = message_ids[0]
                rsp = self.__expect_MLR_rsp(message_id)

                logging.info('MLR.rsp %s uri_path=%s, payload=%s', rsp, rsp.coap.uri_path, rsp.coap.payload)

                statusTlv = None
                for tlv in rsp.coap.payload:
                    if isinstance(tlv, network_layer.Status):
                        statusTlv = tlv
                        break

                self.assertIsNotNone(statusTlv)
                self.assertEqual(expect_mlr_rsp_status, statusTlv.status)

        else:
            for addr in addrs:
                self.assertNotIn(ipaddress.IPv6Address(addr), reg_mas)

    def __expect_MLR_rsp(self, message_id):
        logging.info("Expecting MLR.rsp with message ID = %s", message_id)
        messages = self.simulator.get_messages_sent_by(self.pbbr_id)
        logging.info("PBBR %d messages: %s", self.pbbr_id, messages)

        while True:
            msg = messages.next_coap_message('2.04')
            logging.info('Check ACK for %s: %s, %s, %s, %s', message_id, msg, msg.coap.message_id, msg.coap.uri_path,
                         msg.coap.payload)
            if msg.coap.message_id == message_id:
                return msg

    def __get_registered_MAs(self, id, expect_mlr_req_num=None, message_ids=None):
        """Get MAs registered via MLR.req by the node"""
        messages = self.simulator.get_messages_sent_by(id)
        reg_mas = []
        while True:
            msg = messages.next_coap_message('0.02', '/n/mr', assert_enabled=False)
            if not msg:
                break

            logging.info("MLR.req: %s %s" % (msg.coap.message_id, msg.coap.payload))
            addrs = msg.get_coap_message_tlv(network_layer.IPv6Addresses)
            reg_mas.append(addrs)
            if message_ids is not None:
                message_ids.append(msg.coap.message_id)

        logging.info('Node %d registered MAs: %s' % (id, reg_mas))

        if expect_mlr_req_num is not None:
            self.assertEqual(len(reg_mas), expect_mlr_req_num)

        # expand from [[...], [...], ...] to [...]
        reg_mas = [ma for mas in reg_mas for ma in mas]

        return reg_mas

    def __check_renewing(self, id, parent_id, addr, expect_mlr_req=True, expect_mlr_req_proxied=False):
        """Check if MLR works that a node can renew it's registered MAs"""
        assert self.pbbr_id == BBR_1
        self.flush_all()
        self.simulator.go(MLR_TIMEOUT + WAIT_REDUNDANCE)

        self.__check_send_mlr_req(id, addr, should_send=expect_mlr_req, expect_mlr_rsp=expect_mlr_req)
        # Parent should either forward or proxy the MLR.req
        if parent_id:
            self.__check_send_mlr_req(parent_id,
                                      addr,
                                      should_send=expect_mlr_req or expect_mlr_req_proxied,
                                      expect_mlr_rsp=expect_mlr_req_proxied)

    def __check_rereg(self, id, parent_id, addr, expect_mlr_req=True, expect_mlr_req_proxied=False):
        """Check if MLR works that a node can do MLR reregistration when necessary"""
        self.__check_rereg_seqno(id,
                                 parent_id,
                                 addr,
                                 expect_mlr_req=expect_mlr_req,
                                 expect_mlr_req_proxied=expect_mlr_req_proxied)
        self.__check_rereg_pbbr_change(id,
                                       parent_id,
                                       addr,
                                       expect_mlr_req=expect_mlr_req,
                                       expect_mlr_req_proxied=expect_mlr_req_proxied)

    def __check_rereg_seqno(self, id, parent_id, addr, expect_mlr_req=True, expect_mlr_req_proxied=False):
        """Check if MLR works that a node can do MLR reregistration when PBBR seqno changes"""
        # Change seq on PBBR and expect MLR.req within REREG_DELAY
        self.flush_all()
        self.pbbr_seq = (self.pbbr_seq + 10) % 256
        self.nodes[BBR_1].set_backbone_router(seqno=self.pbbr_seq)
        self.simulator.go(REREG_DELAY + WAIT_REDUNDANCE)

        self.__check_send_mlr_req(id, addr, should_send=expect_mlr_req, expect_mlr_rsp=expect_mlr_req)
        # Parent should either forward or proxy the MLR.req
        if parent_id:
            self.__check_send_mlr_req(parent_id,
                                      addr,
                                      should_send=expect_mlr_req or expect_mlr_req_proxied,
                                      expect_mlr_rsp=expect_mlr_req_proxied)

    def __check_rereg_pbbr_change(self, id, parent_id, addr, expect_mlr_req=True, expect_mlr_req_proxied=False):
        """Check if MLR works that a node can do MLR reregistration when PBBR changes"""
        # Make BBR_2 to be Primary and expect MLR.req within REREG_DELAY
        assert self.pbbr_id == BBR_1

        self.flush_all()
        self.nodes[BBR_1].disable_backbone_router()
        self.simulator.go(BBR_REGISTRATION_JITTER + WAIT_REDUNDANCE)
        self.assertEqual(self.nodes[BBR_2].get_backbone_router_state(), 'Primary')
        self.pbbr_id = BBR_2

        self.simulator.go(REREG_DELAY + WAIT_REDUNDANCE)

        self.__check_send_mlr_req(id, addr, should_send=expect_mlr_req, expect_mlr_rsp=expect_mlr_req)
        # Parent should either forward or proxy the MLR.req
        if parent_id:
            self.__check_send_mlr_req(parent_id,
                                      addr,
                                      should_send=expect_mlr_req or expect_mlr_req_proxied,
                                      expect_mlr_rsp=expect_mlr_req_proxied)

        # Restore BBR_1 to be Primary and BBR_2 to be Secondary
        self.nodes[BBR_2].disable_backbone_router()
        self.nodes[BBR_1].enable_backbone_router()
        self.simulator.go(BBR_REGISTRATION_JITTER + WAIT_REDUNDANCE)
        self.assertEqual(self.nodes[BBR_1].get_backbone_router_state(), 'Primary')
        self.nodes[BBR_2].enable_backbone_router()
        self.simulator.go(BBR_REGISTRATION_JITTER + WAIT_REDUNDANCE)
        self.assertEqual(self.nodes[BBR_2].get_backbone_router_state(), 'Secondary')
        self.pbbr_id = BBR_1

    def __switch_to_1_1_parent(self):
        """Check if MLR works when nodes are switching to a 1.1 parent"""
        # Add MA1 to EDs to prepare for parent switching
        logging.info("=" * 10 + " switching to 1.1 parent " + '=' * 10)

        self.flush_all()

        self.nodes[FED_1].add_ipmaddr(MA1)
        self.nodes[MED_1].add_ipmaddr(MA1)
        self.nodes[SED_1].add_ipmaddr(MA1)

        self.simulator.go(REREG_DELAY + WAIT_REDUNDANCE)
        self.assertIn(ipaddress.IPv6Address(MA1), self.__get_registered_MAs(FED_1))
        self.assertNotIn(ipaddress.IPv6Address(MA1), self.__get_registered_MAs(MED_1))
        self.assertNotIn(ipaddress.IPv6Address(MA1), self.__get_registered_MAs(SED_1))

        self.flush_all()

        # Turn off Router 1.2 and turn on Router 1.1
        self.nodes[ROUTER_1_2].stop()
        for id in [FED_1, MED_1, SED_1]:
            self.nodes[ROUTER_1_1].add_allowlist(self.nodes[id].get_addr64())
            self.nodes[id].add_allowlist(self.nodes[ROUTER_1_1].get_addr64())
            self.simulator.go(config.DEFAULT_CHILD_TIMEOUT + WAIT_REDUNDANCE)

            self.assertEqual(self.nodes[id].get_state(), 'child')
            self.assertEqual(self.nodes[id].get_router_id(), self.nodes[ROUTER_1_1].get_router_id())

        self.simulator.go(REREG_DELAY + WAIT_REDUNDANCE)

        # Verify all FED send MLR.req within REREG_DELAY when parent is 1.1
        self.assertIn(ipaddress.IPv6Address(MA1), self.__get_registered_MAs(FED_1))
        self.assertIn(ipaddress.IPv6Address(MA1), self.__get_registered_MAs(MED_1))
        self.assertIn(ipaddress.IPv6Address(MA1), self.__get_registered_MAs(SED_1))

        self.nodes[FED_1].del_ipmaddr(MA1)
        self.nodes[MED_1].del_ipmaddr(MA1)
        self.nodes[SED_1].del_ipmaddr(MA1)

        self.simulator.go(WAIT_REDUNDANCE)

    def __switch_to_1_2_parent(self):
        """Check if MLR works when nodes are switching to a 1.2 parent"""
        # Add MA1 to EDs to prepare for parent switching
        logging.info("=" * 10, "switching to 1.2 parent", '=' * 10)

        self.flush_all()

        self.nodes[FED_1].add_ipmaddr(MA1)
        self.nodes[MED_1].add_ipmaddr(MA1)
        self.nodes[SED_1].add_ipmaddr(MA1)

        self.simulator.go(REREG_DELAY + WAIT_REDUNDANCE)
        self.assertIn(ipaddress.IPv6Address(MA1), self.__get_registered_MAs(FED_1))
        self.assertIn(ipaddress.IPv6Address(MA1), self.__get_registered_MAs(MED_1))
        self.assertIn(ipaddress.IPv6Address(MA1), self.__get_registered_MAs(SED_1))

        self.flush_all()

        # Turn off Router 1.1 and turn on Router 1.2
        self.nodes[ROUTER_1_1].stop()
        self.nodes[ROUTER_1_2].start()
        for id in [FED_1, MED_1, SED_1]:
            self.simulator.go(config.DEFAULT_CHILD_TIMEOUT + WAIT_REDUNDANCE)

            self.assertEqual(self.nodes[id].get_state(), 'child')
            self.assertEqual(self.nodes[id].get_router_id(), self.nodes[ROUTER_1_2].get_router_id())

        self.simulator.go(REREG_DELAY + WAIT_REDUNDANCE)

        # Verify only FTD sends MLR.req within REREG_DELAY when parent is 1.2
        self.assertIn(ipaddress.IPv6Address(MA1), self.__get_registered_MAs(FED_1))

        # MED and SED might still send MLR.req during this period because it could be sending to it's 1.2 parent.

        self.nodes[FED_1].del_ipmaddr(MA1)
        self.nodes[MED_1].del_ipmaddr(MA1)
        self.nodes[SED_1].del_ipmaddr(MA1)

        self.simulator.go(WAIT_REDUNDANCE)

    def __check_parent_merge_med_mlr_req(self, meds, parent_id):
        """Check that the 1.2 parent merge multiple multicast addresses for MED children."""
        self.flush_all()
        for med in meds:
            self.nodes[med].add_ipmaddr(MA1)

        self.nodes[meds[0]].add_ipmaddr(MA2)
        self.nodes[meds[1]].add_ipmaddr(MA3)

        self.simulator.go(PARENT_AGGREGATE_DELAY + WAIT_REDUNDANCE)

        self.__check_send_mlr_req(parent_id, [MA1, MA2, MA3],
                                  should_send=True,
                                  expect_mlr_rsp=True,
                                  expect_mlr_req_num=1,
                                  expect_unique_reg=True)

        # restore
        self.nodes[meds[0]].del_ipmaddr(MA2)
        self.nodes[meds[1]].del_ipmaddr(MA3)
        for med in meds:
            self.nodes[med].del_ipmaddr(MA1)

        self.simulator.go(WAIT_REDUNDANCE)

    def __check_not_send_mlr_req_if_subscribed(self, meds, parent_id):
        """Check that the 1.2 parent does not send MLR.req if the MA is already subscribed."""
        # Parent should register MA1 on Netif
        self.flush_all()
        self.nodes[parent_id].add_ipmaddr(MA1)
        self.simulator.go(WAIT_REDUNDANCE)
        self.__check_send_mlr_req(parent_id, MA1, should_send=True, expect_mlr_rsp=True)

        # Parent should not register MA1 of Child 1 because it's already registerd
        self.flush_all()
        self.nodes[meds[0]].add_ipmaddr(MA1)
        self.simulator.go(PARENT_AGGREGATE_DELAY + WAIT_REDUNDANCE)
        self.__check_send_mlr_req(parent_id, MA1, should_send=False)

        # Parent should register MA2 of Child 1 because it's new
        self.flush_all()
        self.nodes[meds[0]].add_ipmaddr(MA2)
        self.simulator.go(PARENT_AGGREGATE_DELAY + WAIT_REDUNDANCE)
        self.__check_send_mlr_req(parent_id, MA2, should_send=True)

        # Parent should not register MA2 of Child 2 because it's already registered for Child 1
        self.flush_all()
        self.nodes[meds[1]].add_ipmaddr(MA2)
        self.simulator.go(PARENT_AGGREGATE_DELAY + WAIT_REDUNDANCE)
        self.__check_send_mlr_req(parent_id, MA2, should_send=False)

        # Parent should register MA3 of Child 2 because it's new
        self.flush_all()
        self.nodes[meds[1]].add_ipmaddr(MA3)
        self.simulator.go(PARENT_AGGREGATE_DELAY + WAIT_REDUNDANCE)
        self.__check_send_mlr_req(parent_id, MA3, should_send=True)

        # Parent should not register MA2 and MA3 because they are already registered for Child2 itself
        self.flush_all()
        self.nodes[meds[1]].del_ipmaddr(MA2)
        self.nodes[meds[1]].del_ipmaddr(MA3)
        self.nodes[meds[1]].add_ipmaddr(MA2)
        self.nodes[meds[1]].add_ipmaddr(MA3)
        self.simulator.go(PARENT_AGGREGATE_DELAY + WAIT_REDUNDANCE)
        self.__check_send_mlr_req(parent_id, [MA2, MA3], should_send=False)

        # Restore
        self.nodes[parent_id].del_ipmaddr(MA1)
        self.nodes[meds[0]].del_ipmaddr(MA1)
        self.nodes[meds[0]].del_ipmaddr(MA2)
        self.nodes[meds[1]].del_ipmaddr(MA2)
        self.nodes[meds[1]].del_ipmaddr(MA3)
        self.simulator.go(WAIT_REDUNDANCE)


if __name__ == '__main__':
    unittest.main()
