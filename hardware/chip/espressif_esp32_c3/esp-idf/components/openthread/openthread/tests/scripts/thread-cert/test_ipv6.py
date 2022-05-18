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

import io
import random
import string
import struct
import unittest

from ipaddress import ip_address

from ipv6 import (
    ICMPv6Header,
    UDPHeader,
    IPv6Header,
    IPv6PacketFactory,
    UDPDatagram,
    UDPDatagramFactory,
    ICMPv6Factory,
    HopByHopFactory,
    MPLOptionFactory,
    ICMPv6,
    HopByHopOptionHeader,
    HopByHopOption,
    HopByHop,
    MPLOption,
    IPv6Packet,
    ICMPv6EchoBody,
    BytesPayload,
    ICMPv6EchoBodyFactory,
    UpperLayerProtocol,
    UDPHeaderFactory,
    HopByHopOptionsFactory,
    ICMPv6DestinationUnreachableFactory,
    BytesPayloadFactory,
    ICMPv6DestinationUnreachable,
    UdpBasedOnSrcDstPortsPayloadFactory,
    FragmentHeader,
)

import common


class HopByHopOptionBytesValue:
    """ Test helper class """

    _value = "value"

    def __init__(self, _bytes):
        self.bytes = _bytes

    def to_bytes(self):
        return self.bytes

    def to_dict(self, d=None):
        d = d if d is not None else dict()

        d[self._value] = self.bytes
        return d

    def __len__(self):
        return len(self.bytes)


class ICMPv6BytesBody:
    """ Test helper class """

    _icmp_body = "icmp_body"

    def __init__(self, _bytes):
        self.bytes = _bytes

    def to_bytes(self):
        return self.bytes

    def to_dict(self, d=None):

        d[self._icmp_body] = self.bytes
        return d

    def __len__(self):
        return len(self.bytes)


class ICMPv6BytesBodyFactory:
    """ Test helper class """

    def parse(self, data, context):
        return ICMPv6BytesBody(data.read())


class DummyHeader:

    def __init__(self):
        self.checksum = 0


class DummyUpperLayerProtocol(UpperLayerProtocol):

    def __init__(self, header, data, _type):
        super(DummyUpperLayerProtocol, self).__init__(header)
        self._data = data
        self._type = _type

    @property
    def type(self):
        return self._type

    def to_bytes(self):
        return self._data

    def __len__(self):
        return len(self._data)


def any_uint(bits):
    return random.randint(0, (1 << bits) - 1)


def any_type():
    return any_uint(8)


def any_code():
    return any_uint(8)


def any_checksum():
    return any_uint(16)


def any_fragment_offset():
    return any_uint(13)


def any_bool():
    return (any_uint(1) == 1)


def any_fragment_identification():
    return any_uint(32)


def any_icmp_payload(_type, code, checksum, body):
    return bytearray([_type, code, (checksum >> 8) & 0xff, checksum & 0xff]) + body


def any_udp_payload(src_port, dst_port, payload, checksum):
    payload_len = len(payload) + 8
    return bytearray([(src_port >> 8) & 0xff, src_port & 0xff, (dst_port >> 8) & 0xff, dst_port & 0xff,
                      (payload_len >> 8) & 0xff, payload_len & 0xff,
                      (checksum >> 8) & 0xff, checksum & 0xff]) + payload


def any_hop_by_hop_payload(next_header, hdr_ext_len, payload):
    return bytearray([next_header, hdr_ext_len]) + payload


def any_body():
    length = any_uint(8)
    body = "".join([random.choice(string.ascii_letters + string.digits + string.hexdigits) for _ in range(length)])
    return bytearray(body.encode("utf-8"))


def any_payload():
    length = any_uint(8)
    payload = "".join([random.choice(string.printable) for _ in range(length)])
    return bytearray(payload.encode("utf-8"))


def any_ip_address():
    return bytearray([0xfe, 0x80]) + bytearray([0x00] * 6) + bytearray([random.getrandbits(8)] * 8)


def any_port():
    return any_uint(16)


def any_mpl_opt_type():
    return any_uint(8)


def any_mpl_opt_data_len():
    return any_uint(8)


def any_mpl_S():
    return any_uint(2)


def any_mpl_M():
    return any_uint(1)


def any_mpl_V():
    return any_uint(1)


def any_mpl_sequence():
    return any_uint(8)


def any_mpl_seed_id(S):
    length = MPLOption._seed_id_length[S]
    seed_id = "".join([random.choice(string.ascii_letters + string.digits + string.hexdigits) for _ in range(length)])
    return bytearray(seed_id.encode("utf-8"))


def any_next_header():
    return any_uint(8)


def any_traffic_class():
    return any_uint(8)


def any_flow_label():
    return any_uint(20)


def any_hop_limit():
    return any_uint(8)


def any_payload_length():
    return any_uint(16)


def any_hdr_ext_len():
    return any_uint(3)


def any_length():
    return any_uint(4)


def any_str(length=8):
    s = "".join(random.choice(string.printable) for _ in range(length))
    return s.encode("utf-8")


def any_bytes(length=4):
    return bytearray(any_str(length))


def any_dict(keys_count=4):
    keys = [any_str() for _ in range(keys_count)]

    d = {}
    for key in keys:
        d[key] = any_bytes()

    return d


def any_mpl_option():
    S = any_mpl_S()
    M = any_mpl_M()
    V = any_mpl_V()
    sequence = any_mpl_sequence()
    seed_id = any_mpl_seed_id(S)

    return MPLOption(S, M, V, sequence, seed_id)


def any_hop_by_hop_bytes_option_header(length=4):
    _type = any_type()

    # 0 or 1 means padding, so type have to be higher than 1
    while _type <= 1:
        _type = any_type()

    return HopByHopOptionHeader(_type, length)


def any_hop_by_hop_bytes_value(length=2):
    return HopByHopOptionBytesValue(any_bytes(length))


def any_hop_by_hop_bytes_option():
    length = any_length()
    return HopByHopOption(any_hop_by_hop_bytes_option_header(length), any_hop_by_hop_bytes_value(length))


def any_hop_by_hop_mpl_option():
    mpl_option = any_mpl_option()
    return HopByHopOption(any_hop_by_hop_bytes_option_header(len(mpl_option)), mpl_option)


def any_identifier():
    return any_uint(16)


def any_sequence_number():
    return any_uint(16)


def any_data():
    return any_bytes(random.randint(0, 32))


def any_upper_layer_payload(data, _type):
    return DummyUpperLayerProtocol(DummyHeader(), data, _type)


def any_extension_headers():
    return []


def any_message_info():
    return common.MessageInfo()


class TestIPv6Header(unittest.TestCase):

    def test_should_convert_IPv6_header_to_bytes_when_to_bytes_method_is_called(self):
        # GIVEN
        traffic_class = any_traffic_class()
        flow_label = any_flow_label()
        payload_length = any_payload_length()
        next_header = any_next_header()
        hop_limit = any_hop_limit()
        source_address = any_ip_address()
        destination_address = any_ip_address()

        ipv6_header = IPv6Header(source_address, destination_address, traffic_class, flow_label, hop_limit,
                                 payload_length, next_header)

        # WHEN
        data = ipv6_header.to_bytes()

        # THEN
        self.assertEqual(6, data[0] >> 4)
        self.assertEqual(traffic_class, ((data[0] << 8 | data[1]) >> 4) & 0xff)
        self.assertEqual(flow_label, ((data[1] & 0x0F) << 16) | (data[2] << 8) | data[3])
        self.assertEqual(payload_length, struct.unpack("!H", data[4:6])[0])
        self.assertEqual(next_header, data[6])
        self.assertEqual(hop_limit, data[7])
        self.assertEqual(source_address, data[8:24])
        self.assertEqual(destination_address, data[24:40])

    def test_should_create_IPv6Header_when_from_bytes_classmethod_is_called(self):
        # GIVEN
        traffic_class = any_traffic_class()
        flow_label = any_flow_label()
        payload_length = any_payload_length()
        next_header = any_next_header()
        hop_limit = any_hop_limit()
        source_address = any_ip_address()
        destination_address = any_ip_address()

        data = bytearray([(6 << 4) | (traffic_class >> 4), (traffic_class & 0xF) << 4 | (flow_label >> 16) & 0xF,
                          (flow_label >> 8) & 0xff, flow_label & 0xff, payload_length >> 8, payload_length & 0xff,
                          next_header, hop_limit])
        data += ip_address(bytes(source_address)).packed + ip_address(bytes(destination_address)).packed

        # WHEN
        ipv6_header = IPv6Header.from_bytes(io.BytesIO(data))

        # THEN
        self.assertEqual(6, ipv6_header.version)
        self.assertEqual(traffic_class, ipv6_header.traffic_class)
        self.assertEqual(flow_label, ipv6_header.flow_label)
        self.assertEqual(payload_length, ipv6_header.payload_length)
        self.assertEqual(next_header, ipv6_header.next_header)
        self.assertEqual(hop_limit, ipv6_header.hop_limit)
        self.assertEqual(source_address, ipv6_header.source_address.packed)
        self.assertEqual(destination_address, ipv6_header.destination_address.packed)

    def test_should_return_proper_header_length_when_IPv6Packet_object_is_called_in_len(self):
        # GIVEN
        ipv6_header = IPv6Header(any_traffic_class(), any_flow_label(), any_payload_length(), any_next_header(),
                                 any_hop_limit(), any_ip_address(), any_ip_address())

        # WHEN
        ipv6_header_length = len(ipv6_header)

        # THEN
        self.assertEqual(40, ipv6_header_length)


class TestUDPHeader(unittest.TestCase):

    def test_should_convert_UDP_header_to_bytes_when_to_bytes_method_is_called(self):
        # GIVEN
        src_port = any_port()
        dst_port = any_port()
        payload_length = any_payload_length()
        checksum = any_checksum()

        udp_header = UDPHeader(src_port, dst_port, payload_length, checksum)

        # WHEN
        data = udp_header.to_bytes()

        # THEN
        self.assertEqual(src_port, struct.unpack("!H", data[0:2])[0])
        self.assertEqual(dst_port, struct.unpack("!H", data[2:4])[0])
        self.assertEqual(payload_length, struct.unpack("!H", data[4:6])[0])
        self.assertEqual(checksum, struct.unpack("!H", data[6:])[0])

    def test_should_create_UDPHeader_when_from_bytes_classmethod_is_called(self):
        # GIVEN
        src_port = any_port()
        dst_port = any_port()
        payload_length = any_payload_length()
        checksum = any_checksum()

        data = struct.pack("!H", src_port) + struct.pack("!H", dst_port) + \
            struct.pack("!H", payload_length) + struct.pack("!H", checksum)

        # WHEN
        udp_header = UDPHeader.from_bytes(io.BytesIO(data))

        # THEN
        self.assertEqual(src_port, udp_header.src_port)
        self.assertEqual(dst_port, udp_header.dst_port)
        self.assertEqual(payload_length, udp_header.payload_length)
        self.assertEqual(checksum, udp_header.checksum)

    def test_should_return_proper_header_length_when_UDPHeader_object_is_called_in_len(self):
        # GIVEN
        udp_header = UDPHeader(any_port(), any_port(), any_payload_length(), any_checksum())

        # WHEN
        udp_header_length = len(udp_header)

        # THEN
        self.assertEqual(8, udp_header_length)

    def test_should_return_17_when_type_property_is_called(self):
        # GIVEN
        udp_header = UDPHeader(any_port(), any_port(), any_payload_length(), any_checksum())

        # THEN
        self.assertEqual(17, udp_header.type)


class TestICMPv6Header(unittest.TestCase):

    def test_should_convert_icmp_message_header_to_bytes_when_to_bytes_method_is_called(self):
        # GIVEN
        _type = any_type()
        code = any_code()
        checksum = any_checksum()

        icmpv6_header = ICMPv6Header(_type, code, checksum)

        # WHEN
        data = icmpv6_header.to_bytes()

        # THEN
        self.assertEqual(_type, data[0])
        self.assertEqual(code, data[1])
        self.assertEqual(checksum, struct.unpack("!H", data[2:])[0])

    def test_should_create_ICMPv6Header_when_to_bytes_classmethod_is_called(self):
        # GIVEN
        _type = any_type()
        code = any_code()
        checksum = any_checksum()

        data = bytearray([_type, code]) + struct.pack("!H", checksum)

        # WHEN
        icmpv6_header = ICMPv6Header.from_bytes(io.BytesIO(data))

        # THEN
        self.assertEqual(_type, icmpv6_header.type)
        self.assertEqual(code, icmpv6_header.code)
        self.assertEqual(checksum, icmpv6_header.checksum)

    def test_should_return_proper_header_length_when_ICMPv6Header_object_is_called_in_len(self):
        # GIVEN
        icmpv6_header = ICMPv6Header(any_type(), any_code(), any_checksum())

        # WHEN
        icmpv6_header_length = len(icmpv6_header)

        # THEN
        self.assertEqual(4, icmpv6_header_length)


class TestIPv6Packet(unittest.TestCase):

    def test_should_build_IPv6Packet_with_ICMP_payload_from_well_know_values_when_to_bytes_method_is_called(self):
        # GIVEN

        ipv6_packet = IPv6Packet(
            IPv6Header(source_address="fd00:1234:4555::ff:fe00:1800", destination_address="ff03::1"),
            ICMPv6(
                ICMPv6Header(128, 0),
                ICMPv6EchoBody(
                    0, 2,
                    bytearray([
                        0x80, 0x00, 0xc7, 0xbf, 0x00, 0x00, 0x00, 0x01, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41,
                        0x41, 0x41
                    ]))), [
                        HopByHop(options=[
                            HopByHopOption(HopByHopOptionHeader(
                                _type=0x6d), MPLOption(S=1, M=0, V=0, sequence=2, seed_id=bytearray([0x00, 0x18])))
                        ])
                    ])

        # WHEN
        ipv6_packet_bytes = ipv6_packet.to_bytes()

        # THEN
        expected_ipv6_packet_bytes = bytearray([
            0x60, 0x00, 0x00, 0x00, 0x00, 0x22, 0x00, 0x40, 0xfd, 0x00, 0x12, 0x34, 0x45, 0x55, 0x00, 0x00, 0x00, 0x00,
            0x00, 0xff, 0xfe, 0x00, 0x18, 0x00, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x01, 0x3a, 0x00, 0x6d, 0x04, 0x40, 0x02, 0x00, 0x18, 0x80, 0x00, 0x87, 0x12, 0x00, 0x00,
            0x00, 0x02, 0x80, 0x00, 0xc7, 0xbf, 0x00, 0x00, 0x00, 0x01, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41,
            0x41, 0x41
        ])

        self.assertEqual(expected_ipv6_packet_bytes, ipv6_packet_bytes)

    def test_should_build_IPv6Packet_with_UDP_payload_from_well_know_values_when_to_bytes_method_is_called(self):
        # GIVEN
        ipv6_header = IPv6Header(source_address="fe80::1", destination_address="ff02::2", hop_limit=255)

        udp_dgram = UDPDatagram(
            UDPHeader(src_port=19788, dst_port=19788),
            BytesPayload(
                bytearray([
                    0x00, 0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x09, 0x01, 0x01, 0x0b, 0x03,
                    0x04, 0xc6, 0x69, 0x73, 0x51, 0x0e, 0x01, 0x80, 0x12, 0x02, 0x00, 0x01, 0xde, 0xad, 0xbe, 0xef
                ])))

        ipv6_packet = IPv6Packet(ipv6_header, udp_dgram)

        # WHEN
        ipv6_packet_bytes = ipv6_packet.to_bytes()

        # THEN
        expected_ipv6_packet_bytes = bytearray([
            0x60, 0x00, 0x00, 0x00, 0x00, 0x28, 0x11, 0xff, 0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x02, 0x4d, 0x4c, 0x4d, 0x4c, 0x00, 0x28, 0xe9, 0xf4, 0x00, 0x15, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x01, 0x09, 0x01, 0x01, 0x0b, 0x03, 0x04, 0xc6, 0x69, 0x73, 0x51, 0x0e, 0x01, 0x80,
            0x12, 0x02, 0x00, 0x01, 0xde, 0xad, 0xbe, 0xef
        ])

        self.assertEqual(expected_ipv6_packet_bytes, ipv6_packet_bytes)


class TestIPv6PacketFactory(unittest.TestCase):

    def test_should_create_IPv6Packet_with_MPL_and_ICMP_when_to_bytes_method_is_called(self):
        # GIVEN
        ipv6_packet_bytes = bytearray([
            0x60, 0x00, 0x00, 0x00, 0x00, 0x22, 0x00, 0x40, 0xfd, 0x00, 0x12, 0x34, 0x45, 0x55, 0x00, 0x00, 0x00, 0x00,
            0x00, 0xff, 0xfe, 0x00, 0x18, 0x00, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x01, 0x3a, 0x00, 0x6d, 0x04, 0x40, 0x02, 0x00, 0x18, 0x80, 0x00, 0x87, 0x12, 0x00, 0x00,
            0x00, 0x02, 0x80, 0x00, 0xc7, 0xbf, 0x00, 0x00, 0x00, 0x01, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41,
            0x41, 0x41
        ])

        ipv6_factory = IPv6PacketFactory(ehf={
            0:
                HopByHopFactory(hop_by_hop_options_factory=HopByHopOptionsFactory(
                    options_factories={109: MPLOptionFactory()}))
        },
                                         ulpf={58: ICMPv6Factory(body_factories={128: ICMPv6EchoBodyFactory()})})

        # WHEN
        ipv6_packet = ipv6_factory.parse(io.BytesIO(ipv6_packet_bytes), any_message_info())

        # THEN
        self.assertEqual('fd00:1234:4555::ff:fe00:1800', ipv6_packet.ipv6_header.source_address.compressed)
        self.assertEqual('ff03::1', ipv6_packet.ipv6_header.destination_address.compressed)
        self.assertEqual(64, ipv6_packet.ipv6_header.hop_limit)
        self.assertEqual(0, ipv6_packet.ipv6_header.next_header)
        self.assertEqual(34, ipv6_packet.ipv6_header.payload_length)
        self.assertEqual(0, ipv6_packet.ipv6_header.flow_label)
        self.assertEqual(0, ipv6_packet.ipv6_header.traffic_class)
        self.assertEqual(6, ipv6_packet.ipv6_header.version)

        self.assertEqual(1, ipv6_packet.extension_headers[0].options[0].value.S)
        self.assertEqual(0, ipv6_packet.extension_headers[0].options[0].value.M)
        self.assertEqual(0, ipv6_packet.extension_headers[0].options[0].value.V)
        self.assertEqual(2, ipv6_packet.extension_headers[0].options[0].value.sequence)
        self.assertEqual(bytearray([0x00, 0x18]), ipv6_packet.extension_headers[0].options[0].value.seed_id)

        self.assertEqual(34578, ipv6_packet.upper_layer_protocol.header.checksum)
        self.assertEqual(128, ipv6_packet.upper_layer_protocol.header.type)
        self.assertEqual(0, ipv6_packet.upper_layer_protocol.header.code)
        self.assertEqual(0, ipv6_packet.upper_layer_protocol.body.identifier)
        self.assertEqual(2, ipv6_packet.upper_layer_protocol.body.sequence_number)
        self.assertEqual(b'\x80\x00\xc7\xbf\x00\x00\x00\x01AAAAAAAAAA', ipv6_packet.upper_layer_protocol.body.data)

    def test_should_create_IPv6Packet_without_any_extension_header_with_ICMP_when_to_bytes_method_is_called(self):
        # GIVEN
        ipv6_packet_bytes = bytearray([
            0x60, 0x00, 0x00, 0x00, 0x00, 0x1A, 0x3A, 0x40, 0xfd, 0x00, 0x12, 0x34, 0x45, 0x55, 0x00, 0x00, 0x00, 0x00,
            0x00, 0xff, 0xfe, 0x00, 0x18, 0x00, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x87, 0x12, 0x00, 0x00, 0x00, 0x02, 0x80, 0x00, 0xc7, 0xbf, 0x00, 0x00,
            0x00, 0x01, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41
        ])

        ipv6_factory = IPv6PacketFactory(ulpf={58: ICMPv6Factory(body_factories={128: ICMPv6EchoBodyFactory()})})

        # WHEN
        ipv6_packet = ipv6_factory.parse(io.BytesIO(ipv6_packet_bytes), any_message_info())
        ipv6_packet._validate_checksum()

        # THEN
        self.assertEqual('fd00:1234:4555::ff:fe00:1800', ipv6_packet.ipv6_header.source_address.compressed)
        self.assertEqual('ff03::1', ipv6_packet.ipv6_header.destination_address.compressed)
        self.assertEqual(64, ipv6_packet.ipv6_header.hop_limit)
        self.assertEqual(58, ipv6_packet.ipv6_header.next_header)
        self.assertEqual(26, ipv6_packet.ipv6_header.payload_length)
        self.assertEqual(0, ipv6_packet.ipv6_header.flow_label)
        self.assertEqual(0, ipv6_packet.ipv6_header.traffic_class)
        self.assertEqual(6, ipv6_packet.ipv6_header.version)

        self.assertEqual(34578, ipv6_packet.upper_layer_protocol.header.checksum)
        self.assertEqual(128, ipv6_packet.upper_layer_protocol.header.type)
        self.assertEqual(0, ipv6_packet.upper_layer_protocol.header.code)
        self.assertEqual(0, ipv6_packet.upper_layer_protocol.body.identifier)
        self.assertEqual(2, ipv6_packet.upper_layer_protocol.body.sequence_number)
        self.assertEqual(b'\x80\x00\xc7\xbf\x00\x00\x00\x01AAAAAAAAAA', ipv6_packet.upper_layer_protocol.body.data)

    def test_should_set_message_info_field_when_to_bytes_method_is_called(self):
        # GIVEN
        ipv6_packet_data = bytearray([
            0x60, 0x00, 0x00, 0x00, 0x00, 0x1A, 0x3A, 0x40, 0xfd, 0x00, 0x12, 0x34, 0x45, 0x55, 0x00, 0x00, 0x00, 0x00,
            0x00, 0xff, 0xfe, 0x00, 0x18, 0x00, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x87, 0x12, 0x00, 0x00, 0x00, 0x02, 0x80, 0x00, 0xc7, 0xbf, 0x00, 0x00,
            0x00, 0x01, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41
        ])

        message_info = any_message_info()
        message_info.source_ipv6 = "ff::"
        message_info.destination_address = "ff::"

        factory = IPv6PacketFactory(ulpf={58: ICMPv6Factory(body_factories={128: ICMPv6EchoBodyFactory()})})

        # WHEN
        factory.parse(io.BytesIO(ipv6_packet_data), message_info)

        # THEN
        self.assertEqual("fd00:1234:4555::ff:fe00:1800", message_info.source_ipv6.compressed)
        self.assertEqual("ff03::1", message_info.destination_ipv6.compressed)


class TestUDPDatagram(unittest.TestCase):

    def test_should_creates_bytes_from_UDPHeader_and_payload_when_to_bytes_method_is_called(self):
        # GIVEN
        src_port = any_port()
        dst_port = any_port()
        checksum = any_checksum()

        payload = any_payload()
        payload_length = len(payload) + 8  # UDP length consists of UDP header length and payload length

        udp_header = UDPHeader(src_port, dst_port, payload_length, checksum)
        udp_payload = BytesPayload(payload)
        udp_dgram = UDPDatagram(udp_header, udp_payload)

        # WHEN
        udp_dgram_bytes = udp_dgram.to_bytes()

        # THEN
        expected_udp_dgram_bytes = struct.pack("!H", src_port) + struct.pack("!H", dst_port) + \
            struct.pack("!H", payload_length) + struct.pack("!H", checksum) + payload

        self.assertEqual(expected_udp_dgram_bytes, udp_dgram_bytes)


class TestIPv6FragmentHeader(unittest.TestCase):

    def test_shold_convert_IPv6_fragment_header_to_bytes_when_to_bytes_method_is_called(self):
        # GIVEN
        type = any_type()
        offset = any_fragment_offset()
        more_flag = any_bool()
        identification = any_fragment_identification()

        ipv6_fragment_header = FragmentHeader(type, offset, more_flag, identification)

        # WHEN
        actual = ipv6_fragment_header.to_bytes()

        # THEN
        expected = bytearray([type, 0x00, offset >> 5, ((offset << 3) & 0xff) | more_flag])\
            + struct.pack("!I", identification)

        self.assertEqual(expected, actual)

    def test_should_create_FragmentHeader_when_from_bytes_classmethod_is_called(self):
        # GIVEN
        type = any_type()
        offset = any_fragment_offset()
        more_flag = any_bool()
        identification = any_fragment_identification()

        data = bytearray([type, 0x00, offset >> 5, ((offset << 3) & 0xff) | more_flag])\
            + struct.pack("!I", identification)

        # WHEN
        ipv6_fragment_header = FragmentHeader.from_bytes(io.BytesIO(data))

        # THEN
        self.assertEqual(type, ipv6_fragment_header.next_header)
        self.assertEqual(offset, ipv6_fragment_header.offset)
        self.assertEqual(more_flag, ipv6_fragment_header.more_flag)
        self.assertEqual(identification, ipv6_fragment_header.identification)


class TestICMPv6(unittest.TestCase):

    def test_should_creates_bytes_from_ICMPv6Header_and_body_when_to_bytes_method_is_called(self):
        # GIVEN
        _type = any_type()
        code = any_code()
        checksum = any_checksum()
        body = any_body()

        icmpv6_header = ICMPv6Header(_type, code, checksum)
        icmpv6_body = ICMPv6BytesBody(body)
        icmpv6_msg = ICMPv6(icmpv6_header, icmpv6_body)

        # WHEN
        actual = icmpv6_msg.to_bytes()

        # THEN
        expected = bytearray([_type, code]) + struct.pack("!H", checksum) + body

        self.assertEqual(expected, actual)


class TestHopByHop(unittest.TestCase):

    def _calculate_hdr_ext_len(self, payload_len):
        count = payload_len // 8
        rest = payload_len % 8

        if rest != 0:
            count += 1

        if count == 0 and rest == 0:
            return count

        return count - 1

    def _calculate_required_padding(self, content_length):
        excess_bytes = content_length & 0x7

        if excess_bytes > 0:
            return 8 - excess_bytes

        return 0

    def create_padding(self, padding_length):
        if padding_length == 1:
            return bytearray([0x00])
        elif padding_length > 1:
            padding_length -= 2
            return bytearray([0x01, padding_length]) + bytearray([0x00 for _ in range(padding_length)])
        else:
            return bytearray()

    def test_should_create_bytes_from_HopByHop_when_to_bytes_method_is_called(self):
        # GIVEN
        next_header = any_next_header()
        hop_by_hop_option = any_hop_by_hop_bytes_option()
        hdr_ext_len = self._calculate_hdr_ext_len(2 + len(hop_by_hop_option))

        hop_by_hop = HopByHop(next_header, [hop_by_hop_option])

        # WHEN
        data = hop_by_hop.to_bytes()

        # THEN
        expected_data = bytearray([next_header, hdr_ext_len]) + hop_by_hop_option.to_bytes()
        padding_length = self._calculate_required_padding(len(expected_data))
        expected_data += self.create_padding(padding_length)

        self.assertEqual(expected_data, data)


class TestMPLOption(unittest.TestCase):

    def test_should_convert_MPLOption_to_bytes_when_to_bytes_method_is_called(self):
        # GIVEN
        S = any_mpl_S()
        M = any_mpl_M()
        V = any_mpl_V()
        sequence = any_mpl_sequence()
        seed_id = any_mpl_seed_id(S)

        mpl_option = MPLOption(S, M, V, sequence, seed_id)

        # WHEN
        data = mpl_option.to_bytes()

        # THEN
        expected_data = bytearray([(S << 6) | (M << 5) | (V << 4), sequence]) + seed_id
        self.assertEqual(expected_data, data)

    def test_should_create_MPLOption_when_to_bytes_method_is_called_with_data(self):
        # GIVEN
        S = any_mpl_S()
        M = any_mpl_M()
        V = any_mpl_V()
        sequence = any_mpl_sequence()
        seed_id = any_mpl_seed_id(S)

        data = bytearray([(S << 6) | (M << 5) | (V << 4), sequence]) + seed_id

        # WHEN
        mpl_option = MPLOption.from_bytes(io.BytesIO(data))

        # THEN
        self.assertEqual(S, mpl_option.S)
        self.assertEqual(M, mpl_option.M)
        self.assertEqual(V, mpl_option.V)
        self.assertEqual(sequence, mpl_option.sequence)
        self.assertEqual(seed_id, mpl_option.seed_id)

    def test_check_if_mpl_seed_id_length_values_was_not_changed(self):
        self.assertEqual(0, MPLOption._seed_id_length[0])
        self.assertEqual(2, MPLOption._seed_id_length[1])
        self.assertEqual(8, MPLOption._seed_id_length[2])
        self.assertEqual(16, MPLOption._seed_id_length[3])

    def test_should_return_proper_length_when_len_is_called_with_mpl_option_object(self):
        # GIVEN
        S = any_mpl_S()
        M = any_mpl_M()
        V = any_mpl_V()
        sequence = any_mpl_sequence()
        seed_id = any_mpl_seed_id(S)

        mpl_option = MPLOption(S, M, V, sequence, seed_id)

        # WHEN
        mpl_option_length = len(mpl_option)

        # THEN
        SMV_and_sequence_length = 2
        self.assertEqual(SMV_and_sequence_length + len(seed_id), mpl_option_length)


class TestclassHopByHopOption(unittest.TestCase):

    def test_should_convert_HopByHopOption_to_bytes_when_to_bytes_method_is_called(self):
        # GIVEN
        length = any_length()
        header = any_hop_by_hop_bytes_option_header(length)
        value = any_hop_by_hop_bytes_value(length)

        hop_by_hop_option = HopByHopOption(header, value)

        # WHEN
        data = hop_by_hop_option.to_bytes()

        # THEN
        expected_data = header.to_bytes() + value.to_bytes()
        self.assertEqual(expected_data, data)

    def test_should_return_length_of_HopByHopOption_when_len_is_called_with_hop_by_hop_option_object(self):
        # GIVEN
        length = any_length()
        header = any_hop_by_hop_bytes_option_header(length)
        value = any_hop_by_hop_bytes_value(length)

        hop_by_hop_option = HopByHopOption(header, value)

        # WHEN
        hop_by_hop_option_length = len(hop_by_hop_option)

        # THEN
        header_length = 2
        expected_hop_by_hop_option_length = header_length + length
        self.assertEqual(expected_hop_by_hop_option_length, hop_by_hop_option_length)


class TestHopByHopOptionHeader(unittest.TestCase):

    def test_should_convert_HopByHopOptionHeader_to_bytes_when_to_bytes_method_is_called(self):
        # GIVEN
        _type = any_type()
        length = any_length()

        hop_by_hop_option_header = HopByHopOptionHeader(_type, length)

        # WHEN
        data = hop_by_hop_option_header.to_bytes()

        # THEN
        expected_data = bytearray([_type, length])
        self.assertEqual(expected_data, data)

    def test_should_create_HopByHopOptionHeader_when_to_bytes_method_is_called_with_data(self):
        # GIVEN
        _type = any_type()
        length = any_length()

        data = bytearray([_type, length])

        # WHEN
        option_header = HopByHopOptionHeader.from_bytes(io.BytesIO(data))

        # THEN
        self.assertEqual(_type, option_header.type)
        self.assertEqual(length, option_header.length)

    def test_should_return_proper_length_when_len_is_called_with_HopByHopOptionHeader_object(self):
        # GIVEN
        _type = any_type()
        length = any_length()

        option_header = HopByHopOptionHeader(_type, length)

        # WHEN
        option_header_length = len(option_header)

        # THEN
        expected_option_header_length = 2
        self.assertEqual(expected_option_header_length, option_header_length)


class TestHopByHopFactory(unittest.TestCase):

    def _calculate_hdr_ext_len(self, payload_length):
        count = payload_length >> 3

        if (payload_length & 0x7) == 0 and count > 0:
            return count - 1

        return count

    def padding(self, content_length):
        excess_bytes = content_length & 0x7

        if excess_bytes > 0:
            padding_length = 8 - excess_bytes

            if padding_length == 1:
                return bytearray([0x00])
            elif padding_length > 1:
                padding_length -= 2
                return bytearray([0x01, padding_length]) + bytearray([0x00 for _ in range(padding_length)])

        return bytearray()

    def test_should_create_HopByHop_object_instance_when_to_bytes_method_is_called_with_data(self):
        # GIVEN
        hop_by_hop_option = any_hop_by_hop_mpl_option()
        hop_by_hop_option_type = hop_by_hop_option.header.type

        next_header = any_next_header()
        hdr_ext_len = self._calculate_hdr_ext_len(2 + len(hop_by_hop_option))

        hop_by_hop_factory = HopByHopFactory(hop_by_hop_options_factory=HopByHopOptionsFactory(
            options_factories={hop_by_hop_option_type: MPLOptionFactory()}))

        data = bytearray([next_header, hdr_ext_len]) + hop_by_hop_option.to_bytes()
        data += self.padding(len(data))

        # WHEN
        hop_by_hop = hop_by_hop_factory.parse(io.BytesIO(data), any_message_info())

        # THEN
        self.assertEqual(hop_by_hop_option.value.S, hop_by_hop.options[0].value.S)
        self.assertEqual(hop_by_hop_option.value.V, hop_by_hop.options[0].value.V)
        self.assertEqual(hop_by_hop_option.value.M, hop_by_hop.options[0].value.M)
        self.assertEqual(hop_by_hop_option.value.sequence, hop_by_hop.options[0].value.sequence)
        self.assertEqual(hop_by_hop_option.value.seed_id, hop_by_hop.options[0].value.seed_id)

    def test_should_raise_RuntimeError_when_no_option_factory_is_set_and_parse_method_is_called(self):
        # GIVEN
        hop_by_hop_option = any_hop_by_hop_mpl_option()

        next_header = any_next_header()
        hdr_ext_len = self._calculate_hdr_ext_len(2 + len(hop_by_hop_option))

        hop_by_hop_factory = HopByHopFactory(hop_by_hop_options_factory=HopByHopOptionsFactory())

        data = bytes([next_header, hdr_ext_len]) + hop_by_hop_option.to_bytes()
        data += self.padding(len(data))

        # THEN
        self.assertRaises(RuntimeError, hop_by_hop_factory.parse, io.BytesIO(data), any_message_info())


class TestMPLOptionFactory(unittest.TestCase):

    def test_should_produce_MPLOption_from_bytes_when_to_bytes_method_is_called_with_data(self):
        # GIVEN
        S = any_mpl_S()
        M = any_mpl_M()
        V = any_mpl_V()
        sequence = any_mpl_sequence()
        seed_id = any_mpl_seed_id(S)

        SMV = (S << 6) | (M << 5) | (V << 4)
        data = bytearray([SMV, sequence]) + seed_id

        factory = MPLOptionFactory()

        # WHEN
        mpl_opt = factory.parse(io.BytesIO(data), any_message_info())

        # THEN
        self.assertEqual(mpl_opt.S, S)
        self.assertEqual(mpl_opt.M, M)
        self.assertEqual(mpl_opt.V, V)
        self.assertEqual(mpl_opt.sequence, sequence)
        self.assertEqual(mpl_opt.seed_id, seed_id)


class TestUdpBasedOnSrcDstPortsPayloadFactory(unittest.TestCase):

    def test_should_create_payload_from_data_when_src_port_factory_is_defined_and_parse_method_is_called(self):
        # GIVEN
        data = any_data()

        message_info = common.MessageInfo()
        message_info.src_port = any_port()
        message_info.dst_port = any_port()

        factory = UdpBasedOnSrcDstPortsPayloadFactory(
            src_dst_port_based_payload_factories={message_info.src_port: BytesPayloadFactory()})

        # WHEN
        actual_data = factory.parse(io.BytesIO(data), message_info)

        # THEN
        self.assertEqual(data, actual_data.data)

    def test_should_create_payload_from_data_when_dst_port_factory_is_defined_and_parse_method_is_called(self):
        # GIVEN
        data = any_data()

        message_info = common.MessageInfo()
        message_info.src_port = any_port()
        message_info.dst_port = any_port()

        factory = UdpBasedOnSrcDstPortsPayloadFactory(
            src_dst_port_based_payload_factories={message_info.dst_port: BytesPayloadFactory()})

        # WHEN
        actual_data = factory.parse(io.BytesIO(data), message_info)

        # THEN
        self.assertEqual(data, actual_data.data)

    def test_should_raise_RuntimeError_when_parse_method_is_called_but_required_factory_is_not_defined(self):
        # GIVEN
        data = any_data()

        message_info = common.MessageInfo()
        message_info.src_port = any_port()
        message_info.dst_port = any_port()

        factory = UdpBasedOnSrcDstPortsPayloadFactory(src_dst_port_based_payload_factories={})

        # THEN
        self.assertRaises(RuntimeError, factory.parse, io.BytesIO(data), message_info)


class TestUDPDatagramFactory(unittest.TestCase):

    def test_should_produce_UDPDatagram_from_bytes_when_to_bytes_method_is_called_with_data(self):
        # GIVEN
        src_port = any_port()
        dst_port = any_port()
        checksum = any_checksum()

        payload = any_payload()
        payload_length = len(payload) + len(UDPHeader(0, 0))

        data = bytearray([(src_port >> 8), (src_port & 0xff), (dst_port >> 8),
                          (dst_port & 0xff), (payload_length >> 8), (payload_length & 0xff), (checksum >> 8),
                          (checksum & 0xff)]) + payload

        factory = UDPDatagramFactory(UDPHeaderFactory(), BytesPayloadFactory())

        # WHEN
        udp_dgram = factory.parse(io.BytesIO(data), any_message_info())

        # THEN
        self.assertEqual(udp_dgram.header.src_port, src_port)
        self.assertEqual(udp_dgram.header.dst_port, dst_port)
        self.assertEqual(udp_dgram.header.payload_length, payload_length)
        self.assertEqual(udp_dgram.header.checksum, checksum)
        self.assertEqual(udp_dgram.payload.data, payload)

    def test_should_set_src_and_dst_port_in_message_info_when_parse_method_is_called(self):
        # GIVEN
        message_info = any_message_info()

        src_port = any_port()
        dst_port = any_port()
        checksum = any_checksum()

        payload = any_payload()
        payload_length = len(payload) + len(UDPHeader(0, 0))

        data = (bytearray([
            (src_port >> 8),
            (src_port & 0xff),
            (dst_port >> 8),
            (dst_port & 0xff),
            (payload_length >> 8),
            (payload_length & 0xff),
            (checksum >> 8),
            (checksum & 0xff),
        ]) + payload)

        factory = UDPDatagramFactory(UDPHeaderFactory(), BytesPayloadFactory())

        # WHEN
        factory.parse(io.BytesIO(data), message_info)

        # THEN
        self.assertEqual(src_port, message_info.src_port)
        self.assertEqual(dst_port, message_info.dst_port)


class TestICMPv6Factory(unittest.TestCase):

    def test_should_produce_ICMPv6_from_bytes_when_to_bytes_method_is_called_with_data(self):
        # GIVEN
        _type = any_type()
        code = any_code()
        checksum = any_checksum()
        body = any_body()

        data = bytearray([_type, code, (checksum >> 8), (checksum & 0xff)]) + body

        factory = ICMPv6Factory(body_factories={_type: ICMPv6BytesBodyFactory()})

        # WHEN
        icmpv6_msg = factory.parse(io.BytesIO(data), any_message_info())

        # THEN
        self.assertEqual(icmpv6_msg.header.type, _type)
        self.assertEqual(icmpv6_msg.header.code, code)
        self.assertEqual(icmpv6_msg.header.checksum, checksum)
        self.assertEqual(icmpv6_msg.body.bytes, body)

    def test_should_raise_RuntimeError_when_method_parse_is_called_but_body_factory_is_not_present(self):
        # GIVEN
        _type = any_type()
        code = any_code()
        checksum = any_checksum()
        body = any_body()

        data = bytes([_type, code, (checksum >> 8), (checksum & 0xff)]) + body

        factory = ICMPv6Factory()

        # WHEN
        self.assertRaises(RuntimeError, factory.parse, io.BytesIO(data), any_message_info())


class TestBytesPayload(unittest.TestCase):

    def test_should_create_BytesPayload_when_from_bytes_class_method_is_called(self):
        # GIVEN
        data = any_data()

        # WHEN
        actual = BytesPayload.from_bytes(data)

        # THEN
        self.assertEqual(data, actual.data)

    def test_should_return_exactly_the_same_data_as_passed_to_constructor_when_to_bytes_method_is_called(self):
        # GIVEN
        data = any_data()
        payload = BytesPayload(data)

        # WHEN
        actual = payload.to_bytes()

        # THEN
        self.assertEqual(data, actual)

    def test_should_return_the_same_len_as_data_passed_to_constructor_when_len_is_called_on_BytesPayload_object(self):
        # GIVEN
        data = any_data()
        payload = BytesPayload(data)

        # WHEN
        actual = len(payload)

        # THEN
        self.assertEqual(len(data), actual)


class TestICMPv6EchoBody(unittest.TestCase):

    def test_convert_ICMPv6_echo_body_to_data_when_to_bytes_method_is_called(self):
        # GIVEN
        identifier = any_identifier()
        sequence_number = any_sequence_number()
        data = any_data()

        body = ICMPv6EchoBody(identifier, sequence_number, data)

        # WHEN
        actual = body.to_bytes()

        # THEN
        expected = bytearray([identifier >> 8, identifier & 0xff, sequence_number >> 8, sequence_number & 0xff]) + data
        self.assertEqual(expected, actual)

    def test_should_create_ICMPv6EchoBody_from_data_when_from_bytes_classmethod_is_called(self):
        # GIVEN
        identifier = any_identifier()
        sequence_number = any_sequence_number()
        body_data = any_data()

        data = bytearray([(identifier >> 8), (identifier & 0xff), (sequence_number >> 8), (sequence_number & 0xff)])
        data += body_data

        # WHEN
        actual = ICMPv6EchoBody.from_bytes(io.BytesIO(data))

        # THEN
        self.assertEqual(identifier, actual.identifier)
        self.assertEqual(sequence_number, actual.sequence_number)
        self.assertEqual(body_data, actual.data)

    def test_should_build_ICMPv6EchoBody_from_well_know_values_when_to_bytes_method_is_called(self):
        # GIVEN
        body = ICMPv6EchoBody(
            0, 2,
            bytearray([
                0x80, 0x00, 0xc7, 0xbf, 0x00, 0x00, 0x00, 0x01, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41,
                0x41
            ]))

        # WHEN
        actual = body.to_bytes()

        # THEN
        expected = bytearray([
            0x00, 0x00, 0x00, 0x02, 0x80, 0x00, 0xc7, 0xbf, 0x00, 0x00, 0x00, 0x01, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41,
            0x41, 0x41, 0x41, 0x41
        ])

        self.assertEqual(expected, actual)


class TestICMPv6EchoBodyFactory(unittest.TestCase):

    def test_should_build_ICMPv6EchoBody_when_to_bytes_method_is_called(self):
        # GIVEN
        identifier = any_identifier()
        sequence_number = any_sequence_number()
        body_data = any_data()

        data = bytearray([(identifier >> 8) & 0xff, identifier & 0xff,
                          (sequence_number >> 8) & 0xff, sequence_number & 0xff]) + body_data

        factory = ICMPv6EchoBodyFactory()

        # WHEN
        actual = factory.parse(io.BytesIO(data), any_message_info())

        # THEN
        self.assertTrue(isinstance(actual, ICMPv6EchoBody))

        self.assertEqual(identifier, actual.identifier)
        self.assertEqual(sequence_number, actual.sequence_number)
        self.assertEqual(body_data, actual.data)


class TestICMPv6DestinationUnreachable(unittest.TestCase):

    def test_should_convert_ICMPv6DestinationUnreachable_to_bytearray_when_to_bytes_method_is_called(self):
        # GIVEN
        data = any_data()

        icmpv6_dest_unreachable = ICMPv6DestinationUnreachable(data)

        # WHEN
        actual_data = icmpv6_dest_unreachable.to_bytes()

        # THEN
        self.assertEqual(bytearray([0x00, 0x00, 0x00, 0x00]) + data, actual_data)

    def test_should_convert_bytearray_to_ICMPv6DestinationUnreachable_when_from_bytes_method_is_called(self):
        # GIVEN
        data = any_data()

        # WHEN
        icmpv6_dest_unreachable = ICMPv6DestinationUnreachable.from_bytes(
            io.BytesIO(bytearray([0x00, 0x00, 0x00, 0x00]) + data))

        # THEN
        self.assertEqual(data, icmpv6_dest_unreachable.data)

    def test_should_raise_RuntimeError_when_from_bytes_method_is_called(self):
        # GIVEN
        data = any_data()

        unused = random.randint(1, 1 << 32)

        # WHEN
        self.assertRaises(RuntimeError, ICMPv6DestinationUnreachable.from_bytes,
                          io.BytesIO(bytearray(struct.pack(">I", unused)) + data))


class TestICMPv6DestinationUnreachableFactory(unittest.TestCase):

    def test_should_create_ICMPv6DestinationUnreachable_when_parse_method_is_called(self):
        # GIVEN
        icmp_data = any_data()

        factory = ICMPv6DestinationUnreachableFactory()

        data = bytearray([0x00, 0x00, 0x00, 0x00]) + icmp_data

        # WHEN
        icmpv6_dest_unreachable = factory.parse(io.BytesIO(data), any_message_info())

        # THEN
        self.assertEqual(icmp_data, icmpv6_dest_unreachable.data)


class TestUDPHeaderFactory(unittest.TestCase):

    def test_should_create_UDPHeader_when_to_bytes_method_is_called(self):
        # GIVEN
        factory = UDPHeaderFactory()

        src_port = any_port()
        dst_port = any_port()
        payload_length = any_payload_length()
        checksum = any_checksum()

        data = struct.pack("!H", src_port) + struct.pack("!H", dst_port) + \
            struct.pack("!H", payload_length) + struct.pack("!H", checksum)

        # WHEN
        udp_header = factory.parse(io.BytesIO(data), any_message_info())

        # THEN
        self.assertEqual(src_port, udp_header.src_port)
        self.assertEqual(dst_port, udp_header.dst_port)
        self.assertEqual(payload_length, udp_header.payload_length)
        self.assertEqual(checksum, udp_header.checksum)


class TestHopByHopOptionsFactory(unittest.TestCase):

    def test_should_create_option_from_bytearray_when_to_bytes_method_is_called(self):
        # GIVEN

        class DummyOptionFactory:

            def parse(self, data, message_info):
                return data.read()

        factory = HopByHopOptionsFactory(options_factories={2: DummyOptionFactory()})

        data = bytearray([0x02, 0x03, 0x11, 0x22, 0x33, 0x01, 0x00])

        # WHEN
        actual_options = factory.parse(io.BytesIO(data), any_message_info())

        # THEN
        self.assertEqual(1, len(actual_options))
        self.assertEqual(2, actual_options[0].header.type)
        self.assertEqual(3, actual_options[0].header.length)


if __name__ == "__main__":
    unittest.main()
