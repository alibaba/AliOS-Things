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

import logging
import operator
import sys

from pktverify import consts
from pktverify.test_info import TestInfo


class NodeSummary(object):
    """
    Represents a summary of a node.
    """

    def __init__(self, role, extaddr):
        self._role = role
        self._extaddr = extaddr
        self._ipaddrs = {}

    @property
    def role(self):
        return self._role

    @property
    def extaddr(self):
        return self._extaddr

    @property
    def ipaddr_link_local(self):
        for a, _ in self._iter_ipaddrs_rev():
            if a.is_link_local:
                return a

        return None

    @property
    def ipaddr_mleid(self):
        for a, _ in self._iter_ipaddrs_rev():
            if a.is_mleid:
                return a

        return None

    def _iter_ipaddrs_rev(self):
        return sorted(self._ipaddrs.items(), key=operator.itemgetter(1), reverse=True)

    def add_ipaddr(self, ipaddr, index):
        if ipaddr not in self._ipaddrs:
            self._ipaddrs[ipaddr] = index

    def __str__(self):
        return "[node {role} extaddr {extaddr} ipaddrs {ipaddrs}]".format(
            role=self._role,
            extaddr=self.extaddr,
            ipaddrs=", ".join(map(str, sorted(self._ipaddrs))),
        )

    __repr__ = __str__


class Summary(object):
    """
    Represents a summary of the test.
    """

    def __init__(self, pkts, test_info: TestInfo):
        self._pkts = pkts
        self._test_info = test_info
        self._leader_id = None
        self._analyze()

    def iterroles(self):
        return self._role_to_node.items()

    def _analyze(self):
        self._analyze_test_info()

        with self._pkts.save_index():
            for f in [
                    self._analyze_leader,
                    self._analyze_packets,
            ]:
                self._pkts.index = (0, 0)
                f()

    def _analyze_test_info(self):
        self._role_to_node = {}
        self._extaddr_to_node = {}

        for role, extaddr in self._test_info.extaddrs.items():
            assert role not in self._role_to_node
            assert extaddr not in self._extaddr_to_node

            node = NodeSummary(role, extaddr)
            self._role_to_node[role] = node
            self._extaddr_to_node[extaddr] = node

    def _analyze_leader(self):
        for p in self._pkts:

            if p.mle.cmd in [consts.MLE_DATA_RESPONSE, consts.MLE_ADVERTISEMENT]:

                p.mle.__getattr__('tlv')
                p.mle.__getattr__('tlv.leader_data')
                p.mle.__getattr__('tlv.leader_data.router_id')

                tlv = p.mle.tlv
                if tlv.leader_data:
                    self._leader_id = tlv.leader_data.router_id
                    logging.info("leader found in pcap: %d", self._leader_id)
                    break
        else:
            logging.warning("leader not found in pcap")

    def _analyze_packets(self):
        for i, p in enumerate(self._pkts):
            extaddr, src = None, None
            # each packet should be either wpan or eth
            assert (p.wpan and not p.eth) or (p.eth and not p.wpan)
            if p.wpan:
                # it is a 802.15.4 packet
                extaddr = p.wpan.src64

            if p.ipv6:
                # it is a IPv6 packet
                src = p.ipv6.src

            if extaddr and src:
                if extaddr in self._extaddr_to_node:
                    role_sum = self._extaddr_to_node[extaddr]
                    role_sum.add_ipaddr(src, i)
                else:
                    logging.warn("Extaddr %s is not in the testbed", extaddr)

    def show(self):
        show_roles = "\n\t\t".join(map(str, self._role_to_node.values()))
        sys.stderr.write("""{header}
    Pcap Summary:
        packets = {num_packets}
        roles = {num_roles}
            {show_roles}
    {tailer}
    """.format(
            header='>' * 120,
            num_packets=len(self._pkts),
            num_roles=len(self._role_to_node),
            show_roles=show_roles,
            tailer='<' * 120,
        ))

    def ipaddr_mleid_by_role(self, role):
        node = self._role_to_node[role]
        return node.ipaddr_mleid

    def ipaddr_link_local_by_role(self, role):
        node = self._role_to_node[role]
        return node.ipaddr_link_local

    def role(self, r):
        return self._role_to_node[r]
