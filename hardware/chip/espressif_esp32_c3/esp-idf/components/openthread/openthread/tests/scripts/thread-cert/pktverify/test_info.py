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

import json
import os
from typing import Dict

from pktverify.addrs import EthAddr, ExtAddr, Ipv6Addr


class TestInfo(object):
    """
    Represents the test information.
    """

    def __init__(self, filename):
        self.filename = filename
        with open(filename, 'rt') as fd:
            test_info = json.loads(fd.read())

            self.testcase = test_info.get('testcase', '')
            self._pcap = test_info.get('pcap', 'current.pcap')
            self.topology = self._convert_keys_to_ints(test_info['topology'])
            self.extaddrs = {int(k): ExtAddr(v) for k, v in test_info.get('extaddrs', {}).items()}
            self.ethaddrs = {int(k): EthAddr(v) for k, v in test_info.get('ethaddrs', {}).items()}
            self.ipaddrs = {int(k): [Ipv6Addr(x) for x in l] for k, l in test_info.get('ipaddrs', {}).items()}
            self.mleids = {int(k): Ipv6Addr(v) for k, v in test_info.get('mleids', {}).items()}
            self.rlocs = {int(k): Ipv6Addr(v) for k, v in test_info.get('rlocs', {}).items()}
            self.rloc16s = self._convert_hex_values(self._convert_keys_to_ints(test_info.get('rloc16s', {})))
            self.extra_vars = test_info.get('extra_vars', {})
            self.leader_aloc = Ipv6Addr(test_info.get('leader_aloc')) if 'leader_aloc' in test_info else ''

    def __str__(self):
        macs = dict(self.extaddrs)
        macs.update({k + '_ETH': v for k, v in self.ethaddrs.items()})
        macs = ",\n\t".join("%s=%s" % (k, v) for k, v in macs.items())

        return "TestInfo<{case}|{pcap}|\n\t{macs}>".format(case=self.testcase, pcap=self._pcap, macs=macs)

    @property
    def pcap_path(self) -> str:
        """
        :return: The path to the Pcap file.
        """
        dir = os.path.dirname(self.filename)
        return os.path.join(dir, self._pcap)

    @staticmethod
    def _convert_keys_to_ints(d: Dict[str, any]) -> Dict[int, any]:
        return {int(k): v for k, v in d.items()}

    @staticmethod
    def _convert_hex_values(d: Dict[any, str]) -> Dict[any, int]:
        return {k: int(v, 16) for k, v in d.items()}

    def get_node_name(self, node_id: int) -> str:
        """
        Gets the name of the device.

        :param node_id: The device ID.
        :return: The name of the device.
        """
        return self.topology[node_id].get('name', 'Node%d' % node_id)
