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

import unittest

import command
import config
import ipaddress
import mle
import thread_cert

BBR_1 = 1  # Collapsed with Leader Role
ROUTER_1_1 = 2
ROUTER_1_2 = 3
MED_1_2 = 4
SED_1_2 = 5

WAIT_ATTACH = 5
WAIT_REDUNDANCE = 3
ROUTER_SELECTION_JITTER = 1
BBR_REGISTRATION_JITTER = 5
SED_POLL_PERIOD = 2000  # 2s
MED_TIMEOUT = 20  # 20s

DUA_IID_MANUAL1 = '4444333322221111'
DUA_IID_MANUAL2 = '5555444433332222'

TEST_PREFIX1 = '2001:0:0:1::/64'
TEST_PREFIX2 = '2001:0:0:2::/64'
TEST_PREFIX3 = '2001:0:0:3::/64'
"""
 Topology


   SED_1_2
     |
     |
 ROUTER_1_1         MED_1_2
     |                 |
     |                 |
 BBR_1 (LEADER) --- ROUTER_1_2


 1) Bring up BBR_1, BBR_1 becomes Leader and Primary Backbone Router, with Domain
    Prefix without `P_slaac`.

 2) Bring up ROUTER_1_1, no DUA was added due to that `P_slaac` flag is not set.

 3) Bring up ROUTER_1_2, verify that it has DUA generated.

 4) Bring up MED_1_2 with DUA_IID_MANUAL1 set in advance, verify
    a) DUA_IID_MANUAL1 is registered in Address Registration TLV via Child Update Request.
    b) Remove DUA_IID_MANUAL1, a new DUA generated via SLAAC would be registered in Address
       Registration TLV via Child Update Request.
    c) Set DUA_IID_MANUAL2 which should override the generated one and be registered in Address
       Registration TLV via Child Update Request.
    d) Remove DUA_IID_MANUAL2, a new DUA generated via SLAAC, the same as in above b) would
       be registered in Address Registration TLV via  Child Update Request.

 5) Change BBR_1 from config.DOMAIN_PREFIX to config.DOMAIN_PRFIX_ALTER. Verify that MED_1_2
    generate a new Interface Identifier different from the one generated in 4d) due to the
    Domain Prefix change.

 6) Recover config.Domain_Prefix on BBR_1. Verify that MED_1_2 generates and registers the same
    DUA as in step 4b).

 7) Configure ROUTER_1_1 as Border Router with 3 SLAAC prefixes, verify MED_1_2 would register
    its DUA in Address Registration TLV.

 8) Bring up SED_1_2, verify it generates one DUA, and registers it to its parent, though the parent
    is a Thread 1.1 device.

"""


class TestDomainUnicastAddress(thread_cert.TestCase):
    TOPOLOGY = {
        BBR_1: {
            'version': '1.2',
            'allowlist': [ROUTER_1_1, ROUTER_1_2],
            'is_bbr': True
        },
        ROUTER_1_1: {
            'version': '1.1',
            'allowlist': [BBR_1, SED_1_2]
        },
        ROUTER_1_2: {
            'version': '1.2',
            'allowlist': [BBR_1, MED_1_2]
        },
        MED_1_2: {
            'mode': 'rn',
            'version': '1.2',
            'allowlist': [ROUTER_1_2],
        },
        SED_1_2: {
            'mode': 'n',
            'version': '1.2',
            'allowlist': [ROUTER_1_1],
        },
    }
    """All nodes are created with default configurations"""

    def __get_iid(self, address):
        ''' Get the interface identifier of an IPv6 address.

        Args:
            address (string): An IPv6 address;
        '''
        return ''.join(ipaddress.ip_address(address).exploded.split(':')[4:])

    def __check_dua_registration(self, node, iid, dp_cid):
        ''' Check whether or not the specified Domain Unicast Address is registered in Address
        Registraion TLV.

        Args:
            node (int) : The device id
            iid (string): The Interface Identifier
            dp_cid (int): The context id of the domain prefix.
        '''

        messages = self.simulator.get_messages_sent_by(node)
        msg = messages.next_mle_message(mle.CommandType.CHILD_UPDATE_REQUEST)
        command.check_compressed_address_registration_tlv(msg, dp_cid, iid, cid_present_once=True)

    def test(self):
        # starting context id
        context_id = 1

        # 1) Bring up BBR_1, BBR_1 becomes Leader and Primary Backbone Router, with Domain
        #    Prefix without `P_slaac`.
        self.nodes[BBR_1].set_router_selection_jitter(ROUTER_SELECTION_JITTER)
        self.nodes[BBR_1].set_bbr_registration_jitter(BBR_REGISTRATION_JITTER)
        self.nodes[BBR_1].set_backbone_router(seqno=1)
        self.nodes[BBR_1].start()
        WAIT_TIME = WAIT_ATTACH + ROUTER_SELECTION_JITTER
        self.simulator.go(WAIT_TIME)
        self.assertEqual(self.nodes[BBR_1].get_state(), 'leader')
        self.nodes[BBR_1].enable_backbone_router()
        WAIT_TIME = BBR_REGISTRATION_JITTER + WAIT_REDUNDANCE
        self.simulator.go(WAIT_TIME)
        self.assertEqual(self.nodes[BBR_1].get_backbone_router_state(), 'Primary')

        self.nodes[BBR_1].set_domain_prefix(config.DOMAIN_PREFIX, 'prosD')
        WAIT_TIME = WAIT_REDUNDANCE
        self.simulator.go(WAIT_TIME)

        self.simulator.set_lowpan_context(context_id, config.DOMAIN_PREFIX)
        domain_prefix_cid = context_id

        # 2) Bring up ROUTER_1_1, no DUA was added due to that `P_slaac` flag is not set.
        self.nodes[ROUTER_1_1].set_router_selection_jitter(ROUTER_SELECTION_JITTER)
        WAIT_TIME = WAIT_ATTACH + ROUTER_SELECTION_JITTER
        self.nodes[ROUTER_1_1].start()
        self.simulator.go(WAIT_TIME)
        self.assertEqual(self.nodes[ROUTER_1_1].get_state(), 'router')
        dua = self.nodes[ROUTER_1_1].get_addr(config.DOMAIN_PREFIX)
        assert not dua, 'Error: Unexpected DUA ({})'.format(dua)

        # 3) Bring up ROUTER_1_2, verify that it has DUA generated.
        self.nodes[ROUTER_1_2].set_router_selection_jitter(ROUTER_SELECTION_JITTER)
        self.nodes[ROUTER_1_2].start()
        WAIT_TIME = WAIT_ATTACH + ROUTER_SELECTION_JITTER
        self.simulator.go(WAIT_TIME)
        self.assertEqual(self.nodes[ROUTER_1_2].get_state(), 'router')
        dua = self.nodes[ROUTER_1_2].get_addr(config.DOMAIN_PREFIX)
        assert dua, 'Error: Expected DUA not found'
        self.assertTrue(self.nodes[BBR_1].ping(dua))

        # 4) Bring up MED_1_2 with DUA_IID_MANUAL1 set in advance
        self.nodes[MED_1_2].set_dua_iid(DUA_IID_MANUAL1)
        self.nodes[MED_1_2].set_timeout(MED_TIMEOUT)
        self.nodes[MED_1_2].start()
        WAIT_TIME = WAIT_ATTACH
        self.simulator.go(WAIT_TIME)
        self.assertEqual(self.nodes[MED_1_2].get_state(), 'child')

        # 4a) DUA_IID_MANUAL1 is registered in Address Registration TLV via Child Update Request.
        self.__check_dua_registration(MED_1_2, DUA_IID_MANUAL1, domain_prefix_cid)

        # 4b) Remove DUA_IID_MANUAL1, a new DUA generated via SLAAC would be registered in Address
        #   Registration TLV via Child Update Request.

        # Flush relative message queues.
        messages = self.simulator.get_messages_sent_by(MED_1_2)

        self.nodes[MED_1_2].clear_dua_iid()
        WAIT_TIME = MED_TIMEOUT + WAIT_REDUNDANCE
        self.simulator.go(WAIT_TIME)

        med_1_2_dua = self.nodes[MED_1_2].get_addr(config.DOMAIN_PREFIX)
        assert med_1_2_dua, 'Error: Expected DUA not found'

        med_1_2_dua_iid = self.__get_iid(med_1_2_dua)
        self.__check_dua_registration(MED_1_2, med_1_2_dua_iid, domain_prefix_cid)

        # 4c) Set DUA_IID_MANUAL2 which should override the generated one and be registered in Address
        #   Registration TLV via Child Update Request.

        # Flush relative message queues.
        messages = self.simulator.get_messages_sent_by(MED_1_2)
        self.nodes[MED_1_2].set_dua_iid(DUA_IID_MANUAL2)
        WAIT_TIME = WAIT_REDUNDANCE
        self.simulator.go(WAIT_TIME)

        dua = self.nodes[MED_1_2].get_addr(config.DOMAIN_PREFIX)

        self.__check_dua_registration(MED_1_2, DUA_IID_MANUAL2, domain_prefix_cid)

        # 4d) Remove DUA_IID_MANUAL2, a new DUA generated via SLAAC, the same as in above b) would
        #     be registered in Address Registration TLV via  Child Update Request.

        # Flush relative message queues.
        messages = self.simulator.get_messages_sent_by(MED_1_2)
        self.nodes[MED_1_2].clear_dua_iid()
        WAIT_TIME = WAIT_REDUNDANCE
        self.simulator.go(WAIT_TIME)
        dua = self.nodes[MED_1_2].get_addr(config.DOMAIN_PREFIX)
        assert ipaddress.ip_address(dua) == ipaddress.ip_address(
            med_1_2_dua), 'Error: Expected SLAAC DUA not generated'
        assert ipaddress.ip_address(med_1_2_dua) == ipaddress.ip_address(
            dua), 'Error: Expected same SLAAC DUA not generated'

        self.__check_dua_registration(MED_1_2, med_1_2_dua_iid, domain_prefix_cid)

        # 5) Change BBR_1 from config.DOMAIN_PREFIX to config.DOMAIN_PRFIX_ALTER. Verify that MED_1_2
        #   generates a new Interface Identifier different from the one generated in 4d) due to the
        #   Domain Prefix change.
        context_id += 1
        self.simulator.set_lowpan_context(context_id, config.DOMAIN_PREFIX_ALTER)
        self.nodes[BBR_1].set_domain_prefix(config.DOMAIN_PREFIX_ALTER)
        WAIT_TIME = WAIT_REDUNDANCE
        self.simulator.go(WAIT_TIME)

        med_1_2_dua2 = self.nodes[MED_1_2].get_addr(config.DOMAIN_PREFIX_ALTER)
        med_1_2_dua2_iid = self.__get_iid(med_1_2_dua2)
        self.__check_dua_registration(MED_1_2, med_1_2_dua2_iid, context_id)

        #6) Recover config.Domain_Prefix on BBR_1. Verify that MED_1_2 generates and registers the same
        #   DUA as in step 4b).
        self.nodes[BBR_1].set_domain_prefix(config.DOMAIN_PREFIX)
        WAIT_TIME = WAIT_REDUNDANCE
        self.simulator.go(WAIT_TIME)
        dua = self.nodes[MED_1_2].get_addr(config.DOMAIN_PREFIX)
        assert dua, 'Error: Expected DUA not found'
        assert ipaddress.ip_address(med_1_2_dua) == ipaddress.ip_address(
            dua), 'Error: Expected same SLAAC DUA not generated'

        self.__check_dua_registration(MED_1_2, med_1_2_dua_iid, domain_prefix_cid)

        #7) Configure ROUTER_1_1 as Border Router with 3 SLAAC prefixes, verify MED_1_2 would register
        #   its DUA in Address Registration TLV.

        # Flush relative message queues.
        messages = self.simulator.get_messages_sent_by(MED_1_2)

        context_id += 1
        self.simulator.set_lowpan_context(context_id, TEST_PREFIX1)
        self.nodes[ROUTER_1_1].add_prefix(TEST_PREFIX1)

        context_id += 1
        self.simulator.set_lowpan_context(context_id, TEST_PREFIX2)
        self.nodes[ROUTER_1_1].add_prefix(TEST_PREFIX2)
        context_id += 1
        self.simulator.set_lowpan_context(context_id, TEST_PREFIX3)
        self.nodes[ROUTER_1_1].add_prefix(TEST_PREFIX3)
        self.nodes[ROUTER_1_1].register_netdata()

        WAIT_TIME = WAIT_REDUNDANCE
        self.simulator.go(WAIT_TIME)

        WAIT_TIME = MED_TIMEOUT
        self.simulator.go(WAIT_TIME)
        dua = self.nodes[MED_1_2].get_addr(config.DOMAIN_PREFIX)
        assert dua, 'Error: Expected DUA not found'
        assert ipaddress.ip_address(med_1_2_dua) == ipaddress.ip_address(
            dua), 'Error: Expected same SLAAC DUA not generated'

        self.__check_dua_registration(MED_1_2, med_1_2_dua_iid, domain_prefix_cid)

        #8) Bring up SED_1_2, verify that it generates one DUA, and registers it to its parent, though the parent
        #   is a Thread 1.1 device.
        self.nodes[SED_1_2].set_pollperiod(SED_POLL_PERIOD)
        self.nodes[SED_1_2].start()
        WAIT_TIME = WAIT_ATTACH
        self.simulator.go(WAIT_TIME)

        dua = self.nodes[SED_1_2].get_addr(config.DOMAIN_PREFIX)
        assert dua, 'Error: Expected DUA not found'

        dua_iid = self.__get_iid(dua)
        self.__check_dua_registration(SED_1_2, dua_iid, domain_prefix_cid)

        self.assertTrue(self.nodes[BBR_1].ping(dua))


if __name__ == '__main__':
    unittest.main()
