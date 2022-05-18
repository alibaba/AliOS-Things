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
import network_layer
import thread_cert

BBR_1 = 1  # Collapsed with Leader Role
ROUTER_1_1 = 2
ROUTER_1_2 = 3

FED_1_2_1 = 4
MED_1_2_1 = 5
SED_1_2_1 = 6

FED_1_2_2 = 7
MED_1_2_2 = 8
SED_1_2_2 = 9

WAIT_ATTACH = 5
WAIT_REDUNDANCE = 3
ROUTER_SELECTION_JITTER = 1
BBR_REGISTRATION_JITTER = 5
SED_POLL_PERIOD = 2000  # 2s
MED_TIMEOUT = 20  # 20s
PARENT_AGGREGATE_DELAY = 5  # 5s

DUA_IID_MANUAL1 = '4444333322221111'

ST_DUA_SUCCESS = 0
ST_DUA_REREGISTER = 1
ST_DUA_INVALID = 2
ST_DUA_DUPLICATE = 3
ST_DUA_NO_RESOURCES = 4
ST_DUA_BBR_NOT_PRIMARY = 5
ST_DUA_GENERAL_FAILURE = 6

BBR_REREGISTRATION_DELAY = 10
"""
 Topology


    MED_1_2_1   SED_1_2_1
            \     |
             \    |
FED_1_2_1 --- ROUTER_1_1         FED_1_2_2  MED_1_2_2
                 |                 |        /
                 |                 |       /
             BBR_1 (LEADER) --- ROUTER_1_2 --- SED_1_2_2


 1) Bring up BBR_1, BBR_1 becomes Leader and Primary Backbone Router, with Domain
    Prefix without `P_slaac`.

 2) Test behaviors of ROUTER_1_2 under various response status:
    a) Bring up ROUTER_1_2 with DUA_IID_MANUAL1, one DUA.req should happen to register DUA.
    b) Remove DUA_IID_MANUAL1, one DUA.req should happen for the new generated DUA via SLAAC.
    c) Configure BBR_1 to respond with the fatal error ST_DUA_INVALID, update BBR_1 with
       BBR_REREGISTRATION_DELAY, ROUTER_1_2 should re-register its DUA within BBR_REREGISTRATION_DELAY.
         - ROUTER_1_2 should remove its dua
         - update network data, ROUTER_1_2 would regenerate and register the same dua
    d) Configure BBR_1 to respond with the fatal error ST_DUA_DUPLICATE, update seqno to trigger reregistration.
       After received DUA.rsp with ST_DUA_DUPLICATE, ROUTER_1_2 should
       - increase dad counter
       - regenerate a new DUA
       - send DUA.req
    e) (repeated) Configure BBR_1 to respond with per remaining error status:
       - increase BBR seqno to trigger reregistration
       - ROUTER_1_2 should re-register within BBR_REREGISTRATION_DELAY. For the not fatal errors, ROUTER_1_2
         should re-register within another BBR_REREGISTRATION_DELAY (with least delay if ST_DUA_REREGISTER)
 3) Bring up FED_1_2_1, MED_1_2_1, SED_1_2_1, they should send DUA.req by themselves as the parent
    is of Thread 1.1 version.
 4) Bring up FED_1_2_2, it sends DUA.req itself as it it FTD.
 5) MED_1_2_2, SED_1_2_2, MTDs should should register their DUA to their parent
    by Child Update Request, and the parent would send DUA.req for them on behalf.
 6) Increase seqno on BBR_1, within  BBR_REREGISTRATION_DELAY, there should be one DUA.req from
    per [FED_1_2_1, MED_1_2_1, SED_1_2_1, FED_1_2_2], and 3 DUA.req from ROUTER_1_2 among which
    2 DUA.req are for its MTD children.

"""


class TestDomainUnicastAddressRegistration(thread_cert.TestCase):
    TOPOLOGY = {
        BBR_1: {
            'version': '1.2',
            'allowlist': [ROUTER_1_1, ROUTER_1_2],
            'is_bbr': True
        },
        ROUTER_1_1: {
            'version': '1.1',
            'allowlist': [BBR_1, FED_1_2_1, MED_1_2_1, SED_1_2_1]
        },
        ROUTER_1_2: {
            'version': '1.2',
            'allowlist': [BBR_1, FED_1_2_2, MED_1_2_2, SED_1_2_2]
        },
        FED_1_2_1: {
            'version': '1.2',
            'allowlist': [ROUTER_1_1],
        },
        MED_1_2_1: {
            'mode': 'rn',
            'version': '1.2',
            'allowlist': [ROUTER_1_1],
        },
        SED_1_2_1: {
            'mode': 'n',
            'version': '1.2',
            'allowlist': [ROUTER_1_1],
        },
        FED_1_2_2: {
            'version': '1.2',
            'allowlist': [ROUTER_1_2],
        },
        MED_1_2_2: {
            'mode': 'rn',
            'version': '1.2',
            'allowlist': [ROUTER_1_2],
        },
        SED_1_2_2: {
            'mode': 'n',
            'version': '1.2',
            'allowlist': [ROUTER_1_2],
        },
    }
    """All nodes are created with default configurations"""

    def __get_iid(self, address):
        ''' Get the interface identifier of an IPv6 address.

        Args:
            address (string): An IPv6 address;
        '''
        return ''.join(ipaddress.ip_address(address).exploded.split(':')[4:])

    def __check_dua_registration_tmf(self, node, occurences=1, ml_eid=None):

        messages = self.simulator.get_messages_sent_by(node)
        for i in range(occurences):
            msg = messages.next_coap_message('0.02', '/n/dr', False)
            assert msg, 'Expected {}, but {}th not found\n node: {}(extaddr: {})'.format(
                occurences, i + 1, node, self.nodes[node].get_addr64())
            if ml_eid:
                ml_eid_tlv = msg.get_coap_message_tlv(network_layer.MlEid)
                self.assertEqual(ml_eid, ml_eid_tlv.ml_eid.hex())

    def test(self):
        # starting context id
        context_id = 1
        seq_num = 1

        # 1) Bring up BBR_1, BBR_1 becomes Leader and Primary Backbone Router, with Domain
        #    Prefix without `P_slaac`.
        self.nodes[BBR_1].set_router_selection_jitter(ROUTER_SELECTION_JITTER)
        self.nodes[BBR_1].set_bbr_registration_jitter(BBR_REGISTRATION_JITTER)
        self.nodes[BBR_1].set_backbone_router(seqno=seq_num, reg_delay=BBR_REREGISTRATION_DELAY)
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

        # 2) Test behaviors of ROUTER_1_2 under various response status:
        #  a) Bring up ROUTER_1_2 with DUA_IID_MANUAL1, one DUA.req should happen to register DUA.

        # Flush relative message queues.
        self.flush_nodes([ROUTER_1_2])

        self.nodes[ROUTER_1_2].set_dua_iid(DUA_IID_MANUAL1)
        self.nodes[ROUTER_1_2].set_router_selection_jitter(ROUTER_SELECTION_JITTER)
        self.nodes[ROUTER_1_2].start()
        WAIT_TIME = WAIT_ATTACH
        self.simulator.go(WAIT_TIME)
        self.assertEqual(self.nodes[ROUTER_1_2].get_state(), 'router')

        mliid = self.__get_iid(self.nodes[ROUTER_1_2].get_mleid())

        WAIT_TIME = WAIT_ATTACH + WAIT_REDUNDANCE
        self.simulator.go(WAIT_TIME)
        self.__check_dua_registration_tmf(ROUTER_1_2, 1, self.nodes[ROUTER_1_2].get_mleid_iid())

        #  b) Remove DUA_IID_MANUAL1, one DUA.req should happen for the new generated DUA via SLAAC.

        # Flush relative message queues.
        self.flush_nodes([ROUTER_1_2])
        self.nodes[ROUTER_1_2].clear_dua_iid()
        WAIT_TIME = WAIT_ATTACH + WAIT_REDUNDANCE
        self.simulator.go(WAIT_TIME)
        self.__check_dua_registration_tmf(ROUTER_1_2, 1, self.nodes[ROUTER_1_2].get_mleid_iid())

        #c) Configure BBR_1 to respond with the fatal error ST_DUA_INVALID, update BBR_1 with
        #   BBR_REREGISTRATION_DELAY, ROUTER_1_2 should re-register its DUA within BBR_REREGISTRATION_DELAY.
        #     - ROUTER_1_2 should remove its dua
        #     - update network data, ROUTER_1_2 would regenerate and register the same dua

        # Flush relative message queues.
        self.flush_nodes([ROUTER_1_2])
        seq_num = seq_num + 1
        self.nodes[BBR_1].set_next_dua_response(ST_DUA_INVALID, mliid)
        self.nodes[BBR_1].set_backbone_router(seqno=seq_num)
        WAIT_TIME = BBR_REREGISTRATION_DELAY + WAIT_REDUNDANCE
        self.simulator.go(WAIT_TIME)

        self.__check_dua_registration_tmf(ROUTER_1_2, 1, self.nodes[ROUTER_1_2].get_mleid_iid())
        dua = self.nodes[ROUTER_1_2].get_addr(config.DOMAIN_PREFIX)
        assert not dua, 'Error: Unexpected DUA ({}) found'.format(dua)

        # Retry after new network data is available
        seq_num = seq_num + 1
        dua = self.nodes[ROUTER_1_2].get_addr(config.DOMAIN_PREFIX)
        self.nodes[BBR_1].set_backbone_router(seqno=seq_num)
        WAIT_TIME = BBR_REREGISTRATION_DELAY + WAIT_REDUNDANCE
        self.simulator.go(WAIT_TIME)

        self.__check_dua_registration_tmf(ROUTER_1_2, 1, self.nodes[ROUTER_1_2].get_mleid_iid())
        dua = self.nodes[ROUTER_1_2].get_addr(config.DOMAIN_PREFIX)
        assert dua, 'Error: Expected DUA ({}) not found'.format(dua)

        #d) Configure BBR_1 to respond with the fatal error ST_DUA_DUPLICATE, update seqno to trigger reregistration.
        #   After received DUA.rsp with ST_DUA_DUPLICATE, ROUTER_1_2 should
        #   - increase dad counter
        #   - regenerate a new DUA
        #   - send DUA.req

        # Flush relative message queues.
        self.flush_nodes([ROUTER_1_2])
        seq_num = seq_num + 1
        self.nodes[BBR_1].set_next_dua_response(ST_DUA_DUPLICATE, mliid)
        self.nodes[BBR_1].set_backbone_router(seqno=seq_num)
        WAIT_TIME = BBR_REREGISTRATION_DELAY * 2 + WAIT_REDUNDANCE
        self.simulator.go(WAIT_TIME)

        self.__check_dua_registration_tmf(ROUTER_1_2, 2, self.nodes[ROUTER_1_2].get_mleid_iid())

        dua2 = self.nodes[ROUTER_1_2].get_addr(config.DOMAIN_PREFIX)
        assert dua2, 'Error: Expected DUA ({}) not found'.format(dua2)
        assert dua2 != dua, 'Error: Expected Different DUA not found, same DUA {}'.format(dua2)

        # e) (repeated) Configure BBR_1 to respond with per remaining error status:
        #   - increase BBR seqno to trigger reregistration
        #   - ROUTER_1_2 should re-register within BBR_REREGISTRATION_DELAY. For the not fatal errors, ROUTER_1_2
        #     should re-register within another BBR_REREGISTRATION_DELAY (with least delay if ST_DUA_REREGISTER)
        for status in ['5.00', ST_DUA_REREGISTER, ST_DUA_NO_RESOURCES, ST_DUA_BBR_NOT_PRIMARY, ST_DUA_GENERAL_FAILURE]:
            print(f'Testing Status {status}...')
            # Flush relative message queues.
            self.flush_nodes([ROUTER_1_2])
            seq_num = seq_num + 1
            self.nodes[BBR_1].set_next_dua_response(status, mliid)
            self.nodes[BBR_1].set_backbone_router(seqno=seq_num)
            WAIT_TIME = BBR_REREGISTRATION_DELAY + WAIT_REDUNDANCE
            if status != ST_DUA_REREGISTER:
                WAIT_TIME += BBR_REREGISTRATION_DELAY

            self.simulator.go(WAIT_TIME)

            self.__check_dua_registration_tmf(ROUTER_1_2, 2, self.nodes[ROUTER_1_2].get_mleid_iid())

        # Bring up Router_1_1
        self.nodes[ROUTER_1_1].set_router_selection_jitter(ROUTER_SELECTION_JITTER)
        self.nodes[ROUTER_1_1].start()
        WAIT_TIME = WAIT_ATTACH
        self.simulator.go(WAIT_TIME)
        self.assertEqual(self.nodes[ROUTER_1_1].get_state(), 'router')

        dua = self.nodes[ROUTER_1_1].get_addr(config.DOMAIN_PREFIX)
        assert not dua, 'Error: Unexpected DUA ({}) found'.format(dua)

        # Configure children
        for node in [FED_1_2_1, FED_1_2_2]:
            self.nodes[node].set_routereligible(False)

        for node in [SED_1_2_1, SED_1_2_2]:
            self.nodes[node].set_pollperiod(SED_POLL_PERIOD)

        for node in [MED_1_2_1, MED_1_2_2]:
            self.nodes[node].set_timeout(MED_TIMEOUT)

        # 3) Bring up FED_1_2_1, MED_1_2_1, SED_1_2_1, they should send DUA.req by themselves as the parent
        #    is of Thread 1.1 version.
        # 4) Bring up FED_1_2_2, it sends DUA.req itself as it it FTD.
        for node in [FED_1_2_1, MED_1_2_1, SED_1_2_1, FED_1_2_2]:
            print("Starting child {} (extaddr: {})...".format(node, self.nodes[node].get_addr64()))
            # Flush all message queues.
            self.flush_all()
            self.nodes[node].start()
            WAIT_TIME = WAIT_ATTACH
            self.simulator.go(WAIT_TIME)
            self.assertEqual(self.nodes[node].get_state(), 'child')
            WAIT_TIME = PARENT_AGGREGATE_DELAY + WAIT_REDUNDANCE
            self.simulator.go(WAIT_TIME)
            self.__check_dua_registration_tmf(node, 1, self.nodes[node].get_mleid_iid())

        # 5) MED_1_2_2, SED_1_2_2, MTDs should should register their DUA to their parent
        #    by Child Update Request, and the parent would send DUA.req for them on behalf.
        for node in [MED_1_2_2, SED_1_2_2]:
            print("Starting child {} (extaddr: {})...".format(node, self.nodes[node].get_addr64()))
            # Flush all message queues.
            self.flush_all()
            self.nodes[node].start()
            WAIT_TIME = WAIT_ATTACH
            self.simulator.go(WAIT_TIME)
            self.assertEqual(self.nodes[node].get_state(), 'child')

            WAIT_TIME = PARENT_AGGREGATE_DELAY + WAIT_REDUNDANCE
            print("waiting {}".format(WAIT_TIME))
            self.simulator.go(WAIT_TIME)
            self.__check_dua_registration_tmf(ROUTER_1_2, 1, self.nodes[node].get_mleid_iid())

        # 6) Increase seqno on BBR_1, within  BBR_REREGISTRATION_DELAY, there should be one DUA.req from
        #    per [FED_1_2_1, MED_1_2_1, SED_1_2_1, FED_1_2_2], and 3 DUA.req from ROUTER_1_2 among which
        #    2 DUA.req are for its MTD children.

        # Flush all message queues.
        self.flush_all()
        seq_num = seq_num + 1
        self.nodes[BBR_1].set_backbone_router(seqno=seq_num)
        WAIT_TIME = BBR_REREGISTRATION_DELAY + WAIT_REDUNDANCE
        self.simulator.go(WAIT_TIME)
        WAIT_TIME = BBR_REREGISTRATION_DELAY + WAIT_REDUNDANCE
        self.simulator.go(WAIT_TIME)
        for node in [FED_1_2_1, MED_1_2_1, SED_1_2_1, FED_1_2_2]:
            self.__check_dua_registration_tmf(node, 1, self.nodes[node].get_mleid_iid())

        self.__check_dua_registration_tmf(ROUTER_1_2, 3)


if __name__ == '__main__':
    unittest.main()
