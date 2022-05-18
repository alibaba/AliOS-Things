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

import struct

from binascii import hexlify
from enum import IntEnum

import ipaddress

# Map of 2 bits of parent priority.
pp_map = {1: 1, 0: 0, 3: -1, 2: -2}

UDP_TEST_PORT = 12345


# Get the signed parent priority from the byte that parent priority is in.
def map_pp(pp_byte):
    return pp_map[((pp_byte & 0xC0) >> 6)]


def expect_the_same_class(self, other):
    if not isinstance(other, self.__class__):
        raise TypeError("Expected the same class. Got {} and {}".format(type(self), type(other)))


class MessageInfo(object):

    def __init__(self):
        self.aux_sec_hdr = None
        self.aux_sec_hdr_bytes = None

        self.mhr_bytes = None
        self.extra_open_fields = None

        self.source_mac_address = None
        self.destination_mac_address = None

        self._source_ipv6 = None
        self._destination_ipv6 = None

        self._src_port = None
        self._dst_port = None

        self.stable = None
        self.payload_length = 0

    def _convert_value_to_ip_address(self, value):
        if isinstance(value, bytearray):
            value = bytes(value)

        return ipaddress.ip_address(value)

    @property
    def source_ipv6(self):
        return self._source_ipv6

    @source_ipv6.setter
    def source_ipv6(self, value):
        self._source_ipv6 = self._convert_value_to_ip_address(value)

    @property
    def destination_ipv6(self):
        return self._destination_ipv6

    @destination_ipv6.setter
    def destination_ipv6(self, value):
        self._destination_ipv6 = self._convert_value_to_ip_address(value)

    @property
    def src_port(self):
        return self._src_port

    @src_port.setter
    def src_port(self, value):
        self._src_port = value

    @property
    def dst_port(self):
        return self._dst_port

    @dst_port.setter
    def dst_port(self, value):
        self._dst_port = value


class MacAddressType(IntEnum):
    SHORT = 0
    LONG = 1


class MacAddress(object):

    def __init__(self, mac_address, _type, big_endian=True):
        if _type == MacAddressType.SHORT:
            length = 2
        elif _type == MacAddressType.LONG:
            length = 8

        if not big_endian:
            mac_address = mac_address[::-1]

        self._mac_address = bytearray(mac_address[:length])
        self._type = _type

    @property
    def type(self):
        return self._type

    @property
    def mac_address(self):
        return self._mac_address

    @property
    def rloc(self):
        return struct.unpack(">H", self._mac_address)[0]

    def convert_to_iid(self):
        if self._type == MacAddressType.SHORT:
            return (bytearray([0x00, 0x00, 0x00, 0xff, 0xfe, 0x00]) + self._mac_address[:2])
        elif self._type == MacAddressType.LONG:
            return (bytearray([self._mac_address[0] ^ 0x02]) + self._mac_address[1:])
        else:
            raise RuntimeError("Could not convert to IID. Invalid MAC address type: {}".format(self._type))

    @classmethod
    def from_eui64(cls, eui64, big_endian=True):
        if not isinstance(eui64, bytearray):
            raise RuntimeError("Could not create MAC address from EUI64. Invalid data type: {}".format(type(eui64)))

        return cls(eui64, MacAddressType.LONG)

    @classmethod
    def from_rloc16(cls, rloc16, big_endian=True):
        if isinstance(rloc16, int):
            mac_address = struct.pack(">H", rloc16)
        elif isinstance(rloc16, bytearray):
            mac_address = rloc16[:2]
        else:
            raise RuntimeError("Could not create MAC address from RLOC16. Invalid data type: {}".format(type(rloc16)))

        return cls(mac_address, MacAddressType.SHORT)

    def __eq__(self, other):
        return (self.type == other.type) and (self.mac_address == other.mac_address)

    def __repr__(self):
        return "MacAddress(mac_address=b'{}', type={})".format(hexlify(self.mac_address), MacAddressType(self._type))
