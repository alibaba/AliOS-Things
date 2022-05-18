#!/usr/bin/env python3
#
#  Copyright (c) 2016, The OpenThread Authors.
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

import unittest
import copy

import config
import thread_cert
from pktverify.consts import MLE_PARENT_REQUEST, MLE_DATA_RESPONSE, MLE_DATA_REQUEST, MLE_CHILD_UPDATE_REQUEST, MGMT_PENDING_SET_URI, ACTIVE_OPERATION_DATASET_TLV, ACTIVE_TIMESTAMP_TLV, PENDING_TIMESTAMP_TLV, TLV_REQUEST_TLV, NETWORK_DATA_TLV, NM_BORDER_AGENT_LOCATOR_TLV, NM_COMMISSIONER_SESSION_ID_TLV, NM_DELAY_TIMER_TLV, PENDING_OPERATION_DATASET_TLV, NWD_COMMISSIONING_DATA_TLV, LEADER_ALOC
from pktverify.packet_verifier import PacketVerifier
from pktverify.null_field import nullField

COMMISSIONER = 1
LEADER = 2
DUT = 3

CHANNEL_INIT = 19
PANID_INIT = 0xface
LEADER_ACTIVE_TIMESTAMP = 10

COMMISSIONER_PENDING_TIMESTAMP = 20
COMMISSIONER_ACTIVE_TIMESTAMP = 70
COMMISSIONER_DELAY_TIMER = 60000
COMMISSIONER_PENDING_CHANNEL = 20
COMMISSIONER_PENDING_PANID = 0xafce

# Test Purpose and Description:
# -----------------------------
# The purpose of this test case is to verify that after a reset, the DUT
# reattaches to the test network using parameters set in Active/Pending
# Operational Datasets.
#
# Test Topology:
# -------------
#  Commissioner
#       |
#     Leader
#       |
#      DUT
#
# DUT Types:
# ----------
#  Router
#  ED
#  SED


class Cert_9_2_8_PersistentDatasets_Base(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False
    SUPPORT_NCP = False

    TOPOLOGY = {
        COMMISSIONER: {
            'name': 'COMMISSIONER',
            'mode': 'rdn',
            'panid': PANID_INIT,
            'channel': CHANNEL_INIT,
            'allowlist': [LEADER]
        },
        LEADER: {
            'name': 'LEADER',
            'active_dataset': {
                'timestamp': LEADER_ACTIVE_TIMESTAMP,
                'panid': PANID_INIT,
                'channel': CHANNEL_INIT
            },
            'mode': 'rdn',
            'allowlist': [COMMISSIONER, DUT]
        },
        DUT: {
            'name': 'DUT',
            'panid': PANID_INIT,
            'channel': CHANNEL_INIT,
            'allowlist': [LEADER]
        },
    }

    def _setUpDUT(self):
        self.nodes[DUT].add_allowlist(self.nodes[LEADER].get_addr64())
        self.nodes[DUT].enable_allowlist()
        if self.TOPOLOGY[DUT]['mode'] == 'rdn':
            self.nodes[DUT].set_router_selection_jitter(1)
        else:
            self.nodes[DUT].set_timeout(config.DEFAULT_CHILD_TIMEOUT)

    def test(self):
        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        self.nodes[COMMISSIONER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[COMMISSIONER].get_state(), 'router')

        self.nodes[DUT].start()
        self.simulator.go(5)
        if self.TOPOLOGY[DUT]['mode'] == 'rdn':
            self.assertEqual(self.nodes[DUT].get_state(), 'router')
        else:
            self.assertEqual(self.nodes[DUT].get_state(), 'child')

        self.collect_rlocs()

        self.nodes[COMMISSIONER].commissioner_start()
        self.simulator.go(3)

        self.nodes[COMMISSIONER].send_mgmt_pending_set(
            pending_timestamp=COMMISSIONER_PENDING_TIMESTAMP,
            active_timestamp=COMMISSIONER_ACTIVE_TIMESTAMP,
            delay_timer=COMMISSIONER_DELAY_TIMER,
            channel=COMMISSIONER_PENDING_CHANNEL,
            panid=COMMISSIONER_PENDING_PANID,
        )
        self.simulator.go(5)

        # power down the DUT for 60 seconds
        self.nodes[DUT].reset()
        self.simulator.go(60)

        # the network moves to COMMISSIONER_PENDING_PANID
        self.assertEqual(self.nodes[LEADER].get_panid(), COMMISSIONER_PENDING_PANID)
        self.assertEqual(self.nodes[COMMISSIONER].get_panid(), COMMISSIONER_PENDING_PANID)
        self.assertEqual(self.nodes[LEADER].get_channel(), COMMISSIONER_PENDING_CHANNEL)
        self.assertEqual(self.nodes[COMMISSIONER].get_channel(), COMMISSIONER_PENDING_CHANNEL)

        # restart the DUT to attach to COMMISSIONER_PENDING_CHANNEL
        self.nodes[DUT].reset()
        self._setUpDUT()
        self.nodes[DUT].start()

        self.assertEqual(self.nodes[DUT].get_panid(), PANID_INIT)
        self.assertEqual(self.nodes[DUT].get_channel(), CHANNEL_INIT)
        self.simulator.go(30)

        self.assertEqual(self.nodes[DUT].get_panid(), COMMISSIONER_PENDING_PANID)
        self.assertEqual(self.nodes[DUT].get_channel(), COMMISSIONER_PENDING_CHANNEL)
        self.collect_ipaddrs()

        ipaddr = self.nodes[DUT].get_ip6_address(config.ADDRESS_TYPE.ML_EID)
        self.assertTrue(self.nodes[COMMISSIONER].ping(ipaddr))

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        LEADER_RLOC = pv.vars['LEADER_RLOC']
        COMMISSIONER = pv.vars['COMMISSIONER']
        COMMISSIONER_MLEID = pv.vars['COMMISSIONER_MLEID']
        COMMISSIONER_RLOC = pv.vars['COMMISSIONER_RLOC']
        DUT_EXTADDR = pv.vars['DUT']
        DUT_MLEID = pv.vars['DUT_MLEID']

        # Step 1: Ensure the topology is formed correctly
        pv.verify_attached('COMMISSIONER', 'LEADER')
        if self.TOPOLOGY[DUT]['mode'] == 'rdn':
            pv.verify_attached('DUT', 'LEADER')
        else:
            pv.verify_attached('DUT', 'LEADER', 'MTD')
        _pkt = pkts.last()

        # Step 2: Commissioner to send MGMT_PENDING_SET.req to the Leader Anycast
        #         or Routing Locator:
        #         CoAP Request URI
        #             coap://[<L>]:MM/c/ps
        #         CoAP Payload
        #             - valid Commissioner Session ID TLV
        #             - Pending Timestamp TLV : 20s
        #             - Active Timestamp TLV : 70s
        #             - Delay Timer TLV : 60s
        #             - Channel TLV : 20
        #             - PAN ID TLV: 0xAFCE
        pkts.filter_coap_request(MGMT_PENDING_SET_URI).\
            filter_ipv6_2dsts(LEADER_RLOC, LEADER_ALOC).\
            filter(lambda p: p.thread_meshcop.tlv.active_tstamp ==
                   COMMISSIONER_ACTIVE_TIMESTAMP and\
                   p.thread_meshcop.tlv.pending_tstamp ==
                   COMMISSIONER_PENDING_TIMESTAMP and\
                   p.thread_meshcop.tlv.delay_timer ==
                   COMMISSIONER_DELAY_TIMER and\
                   p.thread_meshcop.tlv.channel ==
                   [COMMISSIONER_PENDING_CHANNEL] and\
                   p.thread_meshcop.tlv.pan_id ==
                   [COMMISSIONER_PENDING_PANID] and\
                   NM_COMMISSIONER_SESSION_ID_TLV in p.thread_meshcop.tlv.type
                  ).\
            must_next()

        # Step 3: Leader sends MGMT_PENDING_SET.rsq to the Commissioner:
        #         CoAP Response Code
        #             2.04 Changed
        #         CoAP Payload
        #             - State TLV (value = Accept)
        pkts.filter_coap_ack(MGMT_PENDING_SET_URI).\
            filter_wpan_src64(LEADER).\
            filter_ipv6_dst(COMMISSIONER_RLOC).\
            must_next().\
            must_verify(lambda p: p.thread_meshcop.tlv.state == 1)

        if self.TOPOLOGY[DUT]['mode'] != '-':
            # Step 4: Leader sends a multicast MLE Data Response with the new network data,
            #         including the following TLVs:
            #             - Leader Data TLV:
            #                 Data Version field incremented
            #                 Stable Version field incremented
            #             - Network Data TLV:
            #                 - Commissioner Data TLV:
            #                     Stable flag set to 0
            #                     Border Agent Locator TLV
            #                     Commissioner Session ID TLV
            #             - Active Timestamp TLV: 70s
            #             - Pending Timestamp TLV: 20s
            pkts.filter_mle_cmd(MLE_DATA_RESPONSE).\
                filter_wpan_src64(LEADER).\
                filter_LLANMA().\
                filter(lambda p: p.mle.tlv.active_tstamp ==
                       LEADER_ACTIVE_TIMESTAMP and\
                       p.mle.tlv.pending_tstamp ==
                       COMMISSIONER_PENDING_TIMESTAMP and\
                       p.mle.tlv.leader_data.data_version !=
                       (_pkt.mle.tlv.leader_data.data_version + 1) % 256 and\
                       p.mle.tlv.leader_data.stable_data_version !=
                       (_pkt.mle.tlv.leader_data.stable_data_version + 1) % 256 and\
                       p.thread_nwd.tlv.stable == [0] and\
                       NWD_COMMISSIONING_DATA_TLV in p.thread_nwd.tlv.type and\
                       NM_COMMISSIONER_SESSION_ID_TLV in p.thread_meshcop.tlv.type and\
                       NM_BORDER_AGENT_LOCATOR_TLV in p.thread_meshcop.tlv.type
                       ).\
                must_next()
        else:
            # Step 5: Leader MUST send a MLE Child Update Request or MLE Data
            #         Response to SED, including the following TLVs:
            #             - Leader Data TLV:
            #                 Data Version field incremented
            #                 Stable Version field incremented
            #             - Network Data TLV:
            #             - Active Timestamp TLV: 70s
            #             - Pending Timestamp TLV: 20s
            pkts.filter_wpan_src64(LEADER).\
                filter_wpan_dst64(DUT_EXTADDR).\
                filter_mle_cmd2(MLE_CHILD_UPDATE_REQUEST, MLE_DATA_RESPONSE).\
                filter(lambda p: p.mle.tlv.active_tstamp ==
                       LEADER_ACTIVE_TIMESTAMP and\
                       p.mle.tlv.pending_tstamp ==
                       COMMISSIONER_PENDING_TIMESTAMP and\
                       p.mle.tlv.leader_data.data_version !=
                       (_pkt.mle.tlv.leader_data.data_version + 1) % 256 and\
                       p.mle.tlv.leader_data.stable_data_version !=
                       (_pkt.mle.tlv.leader_data.stable_data_version + 1) % 256 and\
                       NETWORK_DATA_TLV in p.mle.tlv.type
                       ).\
                must_next()

        # Step 6: The DUT MUST send a MLE Data Request to the Leader and include its current
        #         Active Timestamp
        pkts.filter_mle_cmd(MLE_DATA_REQUEST).\
            filter_wpan_src64(DUT_EXTADDR).\
            filter_wpan_dst64(LEADER).\
            filter(lambda p: {
                              TLV_REQUEST_TLV,
                              NETWORK_DATA_TLV
                             } < set(p.mle.tlv.type) and\
                   p.thread_nwd.tlv.type is nullField and\
                   p.mle.tlv.active_tstamp == LEADER_ACTIVE_TIMESTAMP
                  ).\
            must_next()

        # Step 6: Leader sends a MLE Data Response including the following TLVs:
        #             - Active Timestamp TLV
        #             - Pending Timestamp TLV
        #             - Pending Operational Dataset TLV
        pkts.filter_mle_cmd(MLE_DATA_RESPONSE).\
            filter_wpan_src64(LEADER).\
            filter_wpan_dst64(DUT_EXTADDR).\
            filter(lambda p: {
                              ACTIVE_TIMESTAMP_TLV,
                              PENDING_TIMESTAMP_TLV,
                              PENDING_OPERATION_DATASET_TLV
                              } < set(p.mle.tlv.type)
                   ).\
            must_next()

        # Step 9: The DUT MUST attempt to reattach by sending Parent Request using the parameters
        #         from Active Operational Dataset (Channel ='Primary', PANID: 0xFACE)
        #         The DUT MUST then attach using the parameters from the Pending Operational
        #         Dataset (Channel = 'Secondary', PANID:0xAFCE)
        for pan_id in (PANID_INIT, COMMISSIONER_PENDING_PANID):
            pkts.filter_mle_cmd(MLE_PARENT_REQUEST).\
                filter_wpan_src64(DUT_EXTADDR).\
                filter_LLARMA().\
                filter(lambda p: p.wpan.dst_pan == pan_id).\
                must_next()

        # Step 10: The DUT MUST respond with an ICMPv6 Echo Reply
        _pkt = pkts.filter_ping_request().\
            filter_wpan_src64(COMMISSIONER).\
            filter_ipv6_dst(DUT_MLEID).\
            must_next()
        pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            filter_wpan_src64(DUT_EXTADDR).\
            filter_ipv6_dst(COMMISSIONER_MLEID).\
            must_next()


class Cert_9_2_8_PersistentDatasets_ROUTER(Cert_9_2_8_PersistentDatasets_Base):
    TOPOLOGY = copy.deepcopy(Cert_9_2_8_PersistentDatasets_Base.TOPOLOGY)
    TOPOLOGY[DUT]['mode'] = 'rdn'


class Cert_9_2_8_PersistentDatasets_ED(Cert_9_2_8_PersistentDatasets_Base):
    TOPOLOGY = copy.deepcopy(Cert_9_2_8_PersistentDatasets_Base.TOPOLOGY)
    TOPOLOGY[DUT]['mode'] = 'rn'
    TOPOLOGY[DUT]['is_mtd'] = True
    TOPOLOGY[DUT]['timeout'] = config.DEFAULT_CHILD_TIMEOUT


class Cert_9_2_8_PersistentDatasets_SED(Cert_9_2_8_PersistentDatasets_Base):
    TOPOLOGY = copy.deepcopy(Cert_9_2_8_PersistentDatasets_Base.TOPOLOGY)
    TOPOLOGY[DUT]['mode'] = '-'
    TOPOLOGY[DUT]['is_mtd'] = True
    TOPOLOGY[DUT]['timeout'] = config.DEFAULT_CHILD_TIMEOUT


del (Cert_9_2_8_PersistentDatasets_Base)

if __name__ == '__main__':
    unittest.main()
