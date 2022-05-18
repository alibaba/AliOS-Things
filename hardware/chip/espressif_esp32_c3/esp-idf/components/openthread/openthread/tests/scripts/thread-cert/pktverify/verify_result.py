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

from pktverify.packet import Packet
from pktverify.packet_filter import PacketFilter


class VerifyResult(object):
    """
    Represents a verification result of a packet.
    """
    __slots__ = ('_packet_found', '_packet_indexes', '_seek_indexes')

    def __init__(self):
        self._packet_found = {}
        self._packet_indexes = {}
        self._seek_indexes = {}

    def record_last(self, name: str, pkts: PacketFilter) -> None:
        """
        Record the information of the last found packet.

        :param name: The record name.
        :param pkts: The packet filter.
        """
        assert name not in self._packet_found, f'duplicate name: {name}'
        self._packet_found[name] = pkts.last()
        self._packet_indexes[name] = pkts.last_index
        self._seek_indexes[name] = pkts.index

    def packet_index(self, name: str) -> int:
        """
        Returns a recorded packet index.

        :param name: The record name.
        :return: The packet index.
        """
        return self._packet_indexes[name]

    def packet(self, name: str) -> Packet:
        """
        Returns the recorded packet.

        :param name: The record name.
        :return: The packet.
        """
        return self._packet_found[name]

    def seek_index(self, name: str) -> tuple:
        """
        Returns the recorded seek index.

        :param name: The record name.
        :return: The seek index.
        """
        return self._seek_indexes[name]

    def __str__(self):
        return "VerifyResult%s" % self._packet_found
