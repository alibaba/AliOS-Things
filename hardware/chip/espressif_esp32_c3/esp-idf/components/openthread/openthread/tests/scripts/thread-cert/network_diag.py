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

import struct

from enum import IntEnum
from typing import List

import common
import ipaddress
import mle


class TlvType(IntEnum):
    EXT_ADDRESS = 0
    ADDRESS16 = 1
    MODE = 2
    POLLING_PERIOD = 3
    CONNECTIVITY = 4
    ROUTE64 = 5
    LEADER_DATA = 6
    NETWORK_DATA = 7
    IPV6_ADDRESS_LIST = 8
    MAC_COUNTERS = 9
    BATTERY_LEVEL = 14
    SUPPLY_VOLTAGE = 15
    CHILD_TABLE = 16
    CHANNEL_PAGES = 17
    TYPE_LIST = 18
    MAX_CHILD_TIMEOUT = 19


class Ipv6AddressList:

    def __init__(self, addresses: List[ipaddress.IPv6Address]):
        self._addresses = addresses

    @property
    def addresses(self):
        return self._addresses

    def __eq__(self, other):
        common.expect_the_same_class(self, other)
        return self.addresses == other.addresses

    def __repr__(self):
        return f'Ipv6AddressList({self.addresses})'


class Ipv6AddressListFactory:

    def parse(self, data, message_info):
        addresses = []
        while data.tell() < message_info.length:
            addresses.append(ipaddress.IPv6Address(data.read(16)))
        return Ipv6AddressList(addresses)


class MacCounters:

    def __init__(self, counters: List[int]):
        self._counters = counters

    @property
    def if_in_unknown_protos(self):
        return self._counters[0]

    @property
    def if_in_errors(self):
        return self._counters[1]

    @property
    def if_out_errors(self):
        return self._counters[2]

    @property
    def if_in_ucast_pkts(self):
        return self._counters[3]

    @property
    def if_in_broadcast_pkts(self):
        return self._counters[4]

    @property
    def if_in_discards(self):
        return self._counters[5]

    @property
    def if_out_ucast_pkts(self):
        return self._counters[6]

    @property
    def if_out_broadcast_pkts(self):
        return self._counters[7]

    @property
    def if_out_discards(self):
        return self._counters[8]

    @property
    def counters(self):
        return self._counters

    def __eq__(self, other):
        common.expect_the_same_class(self, other)

        return self.counters == other.counters

    def __repr__(self):
        return ('MacCounters(' + f'if_in_unknown_protos={self.if_in_unknown_protos}, ' +
                f'if_in_errors={self.if_in_errors}, ' + f'if_out_errors={self.if_out_errors}, ' +
                f'if_in_ucast_pkts={self.if_in_ucast_pkts}, ' + f'if_in_broadcast_pkts={self.if_in_broadcast_pkts}, ' +
                f'if_in_discards={self.if_in_discards}, ' + f'if_out_ucast_pkts={self.if_out_ucast_pkts}, ' +
                f'if_out_broadcast_pkts={self.if_out_broadcast_pkts}, ' + f'if_out_discards={self.if_out_discards})')


class MacCountersFactory:

    def parse(self, data, message_info):
        return MacCounters(struct.unpack('>9I', data.read(4 * 9)))


class BatteryLevel:

    def __init__(self, battery_level: int):
        self._battery_level = battery_level

    @property
    def battery_level(self):
        return self._battery_level

    def __eq__(self, other):
        common.expect_the_same_class(self, other)

        return self.battery_level == other.battery_level

    def __repr__(self):
        return f'BatteryLevel(battery_level={self.battery_level})'


class BatteryLevelFactory:

    def parse(self, data, message_info):
        return BatteryLevel(struct.unpack('>B', data.read(1))[0])


class SupplyVoltage:

    def __init__(self, supply_voltage: int):
        self._supply_voltage = supply_voltage

    @property
    def supply_voltage(self):
        return self._supply_voltage

    def __eq__(self, other):
        common.expect_the_same_class(self, other)

        return self.supply_voltage == other.supply_voltage

    def __repr__(self):
        return f'SupplyVoltage(supply_voltage={self.supply_voltage})'


class SupplyVoltageFactory:

    def parse(self, data, message_info):
        return SupplyVoltage(struct.unpack('>H', data.read(2))[0])


class ChildTableEntry:

    def __init__(self, timeout: int, child_id: int, mode: mle.Mode):
        self._timeout = timeout
        self._child_id = child_id
        self._mode = mode

    @property
    def timeout(self):
        return self._timeout

    @property
    def child_id(self):
        return self._child_id

    @property
    def mode(self):
        return self._mode

    def __eq__(self, other):
        common.expect_the_same_class(self, other)

        return (self.timeout == other.timeout and self.child_id == other.child_id and self.mode == other.mode)

    def __repr__(self):
        return f'ChildTableEntry(timeout={self.timeout}, child_id={self.child_id}, mode={self.mode})'


class ChildTable:

    def __init__(self, children: List[ChildTableEntry]):
        self._children = sorted(children, key=lambda child: child.child_id)

    @property
    def children(self):
        return self._children

    def __eq__(self, other):
        common.expect_the_same_class(self, other)

        return self.children == other.children

    def __repr__(self):
        return f'ChildTable({self.children})'


class ChildTableFactory:

    def parse(self, data, message_info):
        children = []
        while message_info.length > 0:
            timeout_and_id = struct.unpack('>H', data.read(2))[0]
            message_info.length -= 2

            timeout = (timeout_and_id & 0xf800) >> 11
            child_id = timeout_and_id & 0x1fff

            mode = mle.ModeFactory().parse(data, message_info)
            message_info.length -= 1

            children.append(ChildTableEntry(timeout, child_id, mode))
        return ChildTable(children)


class ChannelPages:

    def __init__(self, channel_pages: bytes):
        self._channel_pages = channel_pages

    @property
    def channel_pages(self):
        return self._channel_pages

    def __eq__(self, other):
        common.expect_the_same_class(self, other)

        return self.channel_pages == other.channel_pages

    def __repr__(self):
        return f'ChannelPages(channel_pages={self.channel_pages})'


class ChannelPagesFactory:

    def parse(self, data, message_info):
        return ChannelPages(data.getvalue())


class TypeList:

    def __init__(self, tlv_types: List[int]):
        self._tlv_types = tlv_types

    @property
    def tlv_types(self):
        return self._tlv_types

    def __eq__(self, other):
        common.expect_the_same_class(self, other)

        return self.tlv_types == other.tlv_types

    def __repr__(self):
        return f'TypeList(tlv_types={self.tlv_types})'


class TypeListFactory:

    def parse(self, data, message_info):
        return TypeList([ord(t) for t in data.getvalue()])


class MaxChildTimeout:

    def __init__(self, max_child_timeout: int):
        self._max_child_timeout = max_child_timeout

    @property
    def max_child_timeout(self):
        return self._max_child_timeout

    def __eq__(self, other):
        common.expect_the_same_class(self, other)

        return self.max_child_timeout == other.max_child_timeout

    def __repr__(self):
        return f'MaxChildTimeout(max_child_timeout={self.max_child_timeout})'


class MaxChildTimeoutFactory:

    def parse(self, data, message_info):
        return MaxChildTimeout(struct.unpack('>I', data.read(4))[0])
