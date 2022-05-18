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
import random
import struct
import unittest
import ipaddress

import common


def any_eui64():
    return bytearray([random.getrandbits(8) for _ in range(8)])


def any_rloc16_int():
    return random.getrandbits(16)


def any_rloc16_bytearray():
    return bytearray([random.getrandbits(8) for _ in range(2)])


def any_ipv6_address():
    return bytearray([random.getrandbits(8) for _ in range(16)])


class TestMessageInfo(unittest.TestCase):

    def test_should_return_source_ipv6_value_when_source_ipv6_property_is_called(self):
        # GIVEN
        source_ipv6 = any_ipv6_address()

        message_info = common.MessageInfo()
        message_info.source_ipv6 = source_ipv6

        # WHEN
        actual_source_ipv6 = message_info.source_ipv6

        # THEN
        self.assertEqual(ipaddress.ip_address(bytes(source_ipv6)), actual_source_ipv6)

    def test_should_return_destination_ipv6_value_when_destination_ipv6_property_is_called(self):
        # GIVEN
        destination_ipv6 = any_ipv6_address()

        message_info = common.MessageInfo()
        message_info.destination_ipv6 = destination_ipv6

        # WHEN
        actual_destination_ipv6 = message_info.destination_ipv6

        # THEN
        self.assertEqual(
            ipaddress.ip_address(bytes(destination_ipv6)),
            actual_destination_ipv6,
        )

    def test_should_return_source_eui64_value_when_source_eui64_property_is_called(self):
        # GIVEN
        source_mac_address = any_eui64()

        message_info = common.MessageInfo()
        message_info.source_mac_address = source_mac_address

        # WHEN
        actual_source_mac_address = message_info.source_mac_address

        # THEN
        self.assertEqual(source_mac_address, actual_source_mac_address)

    def test_should_return_destination_eui64_value_when_destination_eui64_property_is_called(self):
        # GIVEN
        destination_mac_address = any_eui64()

        message_info = common.MessageInfo()
        message_info.destination_mac_address = destination_mac_address

        # WHEN
        actual_destination_mac_address = message_info.destination_mac_address

        # THEN
        self.assertEqual(destination_mac_address, actual_destination_mac_address)


class TestMacAddress(unittest.TestCase):

    def test_should_create_MacAddress_from_eui64_when_from_eui64_classmethod_is_called(self):
        # GIVEN
        eui64 = any_eui64()

        # WHEN
        mac_address = common.MacAddress.from_eui64(eui64)

        # THEN
        self.assertEqual(common.MacAddressType.LONG, mac_address.type)
        self.assertEqual(eui64, mac_address.mac_address)

    def test_should_create_MacAddress_from_rloc16_int_when_from_rloc16_classmethod_is_called(self):
        # GIVEN
        rloc16 = any_rloc16_int()

        # WHEN
        mac_address = common.MacAddress.from_rloc16(int(rloc16))

        # THEN
        self.assertEqual(common.MacAddressType.SHORT, mac_address.type)
        self.assertEqual(struct.pack(">H", rloc16), mac_address.mac_address)

    def test_should_create_MacAddress_from_rloc16_bytearray_when_from_rloc16_classmethod_is_called(self):
        # GIVEN
        rloc16 = any_rloc16_bytearray()

        # WHEN
        mac_address = common.MacAddress.from_rloc16(rloc16)

        # THEN
        self.assertEqual(common.MacAddressType.SHORT, mac_address.type)
        self.assertEqual(rloc16, mac_address.mac_address)

    def test_should_convert_short_MacAddress_to_iid_when_convert_method_is_called(self):
        # GIVEN
        rloc16 = any_rloc16_bytearray()

        mac_address = common.MacAddress.from_rloc16(rloc16)

        # WHEN
        iid = mac_address.convert_to_iid()

        # THEN
        self.assertEqual(bytearray([0x00, 0x00, 0x00, 0xff, 0xfe, 0x00]) + rloc16, iid)

    def test_should_convert_eui64_MacAddress_to_iid_when_convert_method_is_called(self):
        # GIVEN
        eui64 = any_eui64()

        mac_address = common.MacAddress.from_eui64(eui64)

        # WHEN
        iid = mac_address.convert_to_iid()

        # THEN
        self.assertEqual(bytearray([eui64[0] ^ 0x02]) + eui64[1:], iid)


if __name__ == "__main__":
    unittest.main()
