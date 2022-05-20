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
import mle
import thread_cert

LEADER_1_2 = 1
MED_1_2 = 2
SED_1_2 = 3
MED_1_1 = 4
SED_1_1 = 5
ROUTER_1_1 = 6
MED_1_2_2 = 7
SED_1_2_2 = 8

WAIT_ATTACH = 5
WAIT_REDUNDANCE = 3
ROUTER_SELECTION_JITTER = 1

MA1_LINKLOCAL = 'ff02::1:2:3:4'
MA2_ADMINSCOPE = 'ff04::1:2:3:4'
"""
 Topology

             SED_1_2
                |
                |
MED_1_2 --- LEADER_1_2 --- MED_1_1
                |     \
                |       \
             SED_1_1     ROUTER_1_1 --- MED_1_2_2
                            |
                            |
                         SED_1_2_2

 1) Bring up Leader_1_2.

 2) Bring up MED_1_2, which attaches to Thread 1.2 parent, only register MA with scope larger than realm local.
    a) add MA1_LINKLOCAL which would NOT be registered in AddressRegistrationTLV of Child Update Request.
    b) add MA2_ADMINSCOPE which would be registered in AddressRegistrationTLV of Child Update Request.

 3) Bring up SED_1_2, which attaches to Thread 1.2 parent, register any external MA for indirect transmission.
    a) add MA1_LINKLOCAL which would be registered in AddressRegistrationTLV of Child Update Request
    b) add MA2_ADMINSCOPE which would be registered in AddressRegistrationTLV of Child Update Request.

 4) Bring up MED_1_1, which attaches to Thread 1.2 parent, not register any external MA.
    a) add MA1_LINKLOCAL which would NOT be registered in AddressRegistrationTLV of Child Update Request.
    b) add MA2_ADMINSCOPE which would NOT be registered in AddressRegistrationTLV of Child Update Request.

 5) Bring up SED_1_1, which attaches to Thread 1.2 parent, register any external MA for indirect transmission.
    a) add MA1_LINKLOCAL which would be registered in AddressRegistrationTLV of Child Update Request
    b) add MA2_ADMINSCOPE which would be registered in AddressRegistrationTLV of Child Update Request.

 6) Bring up ROUTER_1_1.

 7) Bring up MED_1_2_2 which attaches to Thread 1.1 parent, not register any external MA.
    a) add MA1_LINKLOCAL which would NOT be registered in AddressRegistrationTLV of Child Update Request
    b) add MA2_ADMINSCOPE which would NOT be registered in AddressRegistrationTLV of Child Update Request.

 8) Bring up SED_1_2_2 which attaches to Thread 1.1 parent, register any external MA for indirect transmission.
    a) add MA1_LINKLOCAL which would be registered in AddressRegistrationTLV of Child Update Request
    b) add MA2_ADMINSCOPE which would be registered in AddressRegistrationTLV of Child Update Request.

"""


class TestMulticastRegistration(thread_cert.TestCase):
    TOPOLOGY = {
        LEADER_1_2: {
            'version': '1.2',
            'allowlist': [MED_1_2, SED_1_2, MED_1_1, SED_1_1, ROUTER_1_1],
        },
        MED_1_2: {
            'mode': 'rn',
            'version': '1.2',
            'allowlist': [LEADER_1_2],
        },
        SED_1_2: {
            'mode': 'n',
            'version': '1.2',
            'allowlist': [LEADER_1_2],
        },
        MED_1_1: {
            'mode': 'rn',
            'version': '1.1',
            'allowlist': [LEADER_1_2],
        },
        SED_1_1: {
            'mode': 'n',
            'version': '1.1',
            'allowlist': [LEADER_1_2],
        },
        ROUTER_1_1: {
            'version': '1.1',
            'allowlist': [LEADER_1_2, MED_1_2_2, SED_1_2_2],
        },
        MED_1_2_2: {
            'mode': 'rn',
            'version': '1.2',
            'allowlist': [ROUTER_1_1],
        },
        SED_1_2_2: {
            'mode': 'n',
            'version': '1.2',
            'allowlist': [ROUTER_1_1],
        },
    }
    """All nodes are created with default configurations"""

    def __check_multicast_registration(self,
                                       node,
                                       multicast_address,
                                       child_update_request_assert=True,
                                       in_address_registration=True):
        ''' Check whether or not the addition of the multicast address on the specific node
        would trigger Child Update Request for multicast address registration via Address
        Registraion TLV.

        Args:
            node (int) : The device id
            multicast_address (string): The multicast address
            child_update_request_assert (bool): whether or not the addition should trigger Child Update Request
            in_address_registration (bool): Whether or not the multicast_address should be registered
        '''
        # Flush relative message queues.
        self.flush_nodes([node])

        self.nodes[node].add_ipmaddr(multicast_address)
        WAIT_TIME = WAIT_REDUNDANCE
        self.simulator.go(WAIT_TIME)

        messages = self.simulator.get_messages_sent_by(node)

        msg = messages.next_mle_message(mle.CommandType.CHILD_UPDATE_REQUEST,
                                        assert_enabled=child_update_request_assert)

        if msg:
            is_in = command.check_address_registration_tlv(msg, multicast_address)

            if in_address_registration:
                assert is_in, 'Error: Expected {} in AddressRegistrationTLV not found'.format(multicast_address)
            else:
                assert not is_in, 'Error: Unexpected {} in AddressRegistrationTLV'.format(multicast_address)

    def test(self):

        # 1) Bring up Leader_1_2.
        self.nodes[LEADER_1_2].start()
        WAIT_TIME = WAIT_ATTACH
        self.simulator.go(WAIT_TIME)
        self.assertEqual(self.nodes[LEADER_1_2].get_state(), 'leader')

        # 2) Bring up MED_1_2, which attaches to Thread 1.2 parent, only register MA with scope larger than realm local.
        self.nodes[MED_1_2].start()
        WAIT_TIME = WAIT_ATTACH
        self.simulator.go(WAIT_TIME)
        self.assertEqual(self.nodes[MED_1_2].get_state(), 'child')

        # 2a) add MA1_LINKLOCAL which would NOT be registered in AddressRegistrationTLV of Child Update Request.
        self.__check_multicast_registration(MED_1_2,
                                            MA1_LINKLOCAL,
                                            child_update_request_assert=False,
                                            in_address_registration=False)

        # 2b) add MA2_ADMINSCOPE which would be registered in AddressRegistrationTLV of Child Update Request.
        self.__check_multicast_registration(MED_1_2,
                                            MA2_ADMINSCOPE,
                                            child_update_request_assert=True,
                                            in_address_registration=True)

        # 3) Bring up SED_1_2, which attaches to Thread 1.2 parent, register any external MA for indirect transmission.
        self.nodes[SED_1_2].start()
        WAIT_TIME = WAIT_ATTACH
        self.simulator.go(WAIT_TIME)
        self.assertEqual(self.nodes[SED_1_2].get_state(), 'child')

        # 3a) add MA1_LINKLOCAL which would be registered in AddressRegistrationTLV of Child Update Request.
        self.__check_multicast_registration(SED_1_2,
                                            MA1_LINKLOCAL,
                                            child_update_request_assert=True,
                                            in_address_registration=True)

        # 3b) add MA2_ADMINSCOPE which would be registered in AddressRegistrationTLV of Child Update Request.
        self.__check_multicast_registration(SED_1_2,
                                            MA2_ADMINSCOPE,
                                            child_update_request_assert=True,
                                            in_address_registration=True)

        # 4) Bring up MED_1_1, which attaches to Thread 1.2 parent, not register any external MA.
        self.nodes[MED_1_1].start()
        WAIT_TIME = WAIT_ATTACH
        self.simulator.go(WAIT_TIME)
        self.assertEqual(self.nodes[MED_1_1].get_state(), 'child')

        # 4a) add MA1_LINKLOCAL which would NOT be registered in AddressRegistrationTLV of Child Update Request.
        self.__check_multicast_registration(MED_1_1,
                                            MA1_LINKLOCAL,
                                            child_update_request_assert=False,
                                            in_address_registration=False)

        # 4b) add MA2_ADMINSCOPE which would NOT be registered in AddressRegistrationTLV of Child Update Request.
        self.__check_multicast_registration(MED_1_1,
                                            MA2_ADMINSCOPE,
                                            child_update_request_assert=False,
                                            in_address_registration=False)

        # 5) Bring up SED_1_1, which attaches to Thread 1.2 parent, register any external MA for indirect transmission.
        self.nodes[SED_1_1].start()
        WAIT_TIME = WAIT_ATTACH
        self.simulator.go(WAIT_TIME)
        self.assertEqual(self.nodes[SED_1_1].get_state(), 'child')

        # 5a) add MA1_LINKLOCAL which would be registered in AddressRegistrationTLV of Child Update Request.
        self.__check_multicast_registration(SED_1_1,
                                            MA1_LINKLOCAL,
                                            child_update_request_assert=True,
                                            in_address_registration=True)

        # 5b) add MA2_ADMINSCOPE which would be registered in AddressRegistrationTLV of Child Update Request.
        self.__check_multicast_registration(SED_1_1,
                                            MA2_ADMINSCOPE,
                                            child_update_request_assert=True,
                                            in_address_registration=True)

        #6) Bring up ROUTER_1_1.
        self.nodes[ROUTER_1_1].set_router_selection_jitter(ROUTER_SELECTION_JITTER)
        self.nodes[ROUTER_1_1].start()
        WAIT_TIME = WAIT_ATTACH + ROUTER_SELECTION_JITTER
        self.simulator.go(WAIT_TIME)
        self.assertEqual(self.nodes[ROUTER_1_1].get_state(), 'router')

        # 7) Bring up MED_1_2_2 which attaches to Thread 1.1 parent, not register any external MA.
        self.nodes[MED_1_2_2].start()
        WAIT_TIME = WAIT_ATTACH
        self.simulator.go(WAIT_TIME)
        self.assertEqual(self.nodes[MED_1_2_2].get_state(), 'child')

        # 7a) add MA1_LINKLOCAL which would NOT be registered in AddressRegistrationTLV of Child Update Request
        self.__check_multicast_registration(MED_1_2_2,
                                            MA1_LINKLOCAL,
                                            child_update_request_assert=False,
                                            in_address_registration=False)

        # 7b) add MA2_ADMINSCOPE which would NOT be registered in AddressRegistrationTLV of Child Update Request.
        self.__check_multicast_registration(MED_1_2_2,
                                            MA2_ADMINSCOPE,
                                            child_update_request_assert=False,
                                            in_address_registration=False)

        # 8) Bring up SED_1_2_2 which attaches to Thread 1.1 parent, register any external MA for indirect transmission.
        self.nodes[SED_1_2_2].start()
        WAIT_TIME = WAIT_ATTACH
        self.simulator.go(WAIT_TIME)
        self.assertEqual(self.nodes[SED_1_2_2].get_state(), 'child')

        # 8a) add MA1_LINKLOCAL which would be registered in AddressRegistrationTLV of Child Update Request
        self.__check_multicast_registration(SED_1_2_2,
                                            MA1_LINKLOCAL,
                                            child_update_request_assert=True,
                                            in_address_registration=True)

        # 8b) add MA2_ADMINSCOPE which would be registered in AddressRegistrationTLV of Child Update Request.
        self.__check_multicast_registration(SED_1_2_2,
                                            MA2_ADMINSCOPE,
                                            child_update_request_assert=True,
                                            in_address_registration=True)


if __name__ == '__main__':
    unittest.main()
