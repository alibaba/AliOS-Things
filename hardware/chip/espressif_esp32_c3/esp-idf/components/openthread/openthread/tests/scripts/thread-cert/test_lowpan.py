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
import struct
import unittest

import common
import config
import ipv6
import lowpan


def create_default_lowpan_parser(context_manager):
    return lowpan.LowpanParser(
        lowpan_mesh_header_factory=lowpan.LowpanMeshHeaderFactory(),
        lowpan_decompressor=config.create_default_lowpan_decompressor(context_manager),
        lowpan_fragements_buffers_manager=lowpan.LowpanFragmentsBuffersManager(),
        ipv6_packet_factory=ipv6.IPv6PacketFactory(
            ehf=config.create_default_ipv6_extension_headers_factories(),
            ulpf={
                17:
                    ipv6.UDPDatagramFactory(udp_header_factory=ipv6.UDPHeaderFactory(),
                                            udp_payload_factory=ipv6.BytesPayloadFactory()),
                58:
                    ipv6.ICMPv6Factory(body_factories=config.create_default_ipv6_icmp_body_factories())
            }))


def any_tf():
    return random.getrandbits(2)


def any_nh():
    return random.getrandbits(1)


def any_hlim():
    return random.getrandbits(2)


def any_cid():
    return random.getrandbits(1)


def any_sac():
    return random.getrandbits(1)


def any_sam():
    return random.getrandbits(2)


def any_m():
    return random.getrandbits(1)


def any_dac():
    return random.getrandbits(1)


def any_dam():
    return random.getrandbits(2)


def any_ecn():
    return random.getrandbits(2)


def any_dscp():
    return random.getrandbits(6)


def any_flow_label():
    return random.getrandbits(6)


def any_hop_limit():
    return random.getrandbits(8)


def any_src_addr():
    return bytearray([random.getrandbits(8) for _ in range(16)])


def any_dst_addr():
    return bytearray([random.getrandbits(8) for _ in range(16)])


def any_eui64():
    return bytearray([random.getrandbits(8) for _ in range(8)])


def any_rloc16():
    return bytearray([random.getrandbits(8) for _ in range(2)])


def any_48bits_addr():
    return bytearray([random.getrandbits(8) for _ in range(6)])


def any_32bits_addr():
    return bytearray([random.getrandbits(8) for _ in range(4)])


def any_8bits_addr():
    return bytearray([random.getrandbits(8)])


def any_c():
    return random.getrandbits(1)


def any_p():
    return random.getrandbits(2)


def any_src_port():
    return random.getrandbits(16)


def any_dst_port():
    return random.getrandbits(16)


def any_compressable_src_port():
    return 0xf000 + random.getrandbits(8)


def any_compressable_dst_port():
    return 0xf000 + random.getrandbits(8)


def any_nibble_src_port():
    return 0xf0b0 + random.getrandbits(4)


def any_nibble_dst_port():
    return 0xf0b0 + random.getrandbits(4)


def any_checksum():
    return random.getrandbits(16)


def any_next_header():
    return random.getrandbits(8)


def any_sci():
    return random.getrandbits(4)


def any_dci():
    return random.getrandbits(4)


def any_src_mac_addr():
    return bytearray([random.getrandbits(8) for _ in range(8)])


def any_dst_mac_addr():
    return bytearray([random.getrandbits(8) for _ in range(8)])


def any_context():
    prefix = bytearray([random.getrandbits(8) for _ in range(random.randint(2, 15))])
    prefix_length = len(prefix)
    return lowpan.Context(prefix, prefix_length * 8)


def any_mac_address():
    length = random.choice([2, 8])
    if length == 2:
        return common.MacAddress.from_rloc16(bytearray([random.getrandbits(8) for _ in range(length)]))
    elif length == 8:
        return common.MacAddress.from_eui64(bytearray([random.getrandbits(8) for _ in range(length)]))


def any_hops_left():
    return random.getrandbits(8)


def any_data(length=None):
    length = length if length is not None else random.randint(1, 64)
    return bytearray([random.getrandbits(8) for _ in range(length)])


def any_datagram_size():
    return random.getrandbits(11)


def any_datagram_tag():
    return random.getrandbits(16)


def any_datagram_offset():
    return random.getrandbits(8)


class TestLowpanIPHC(unittest.TestCase):

    def test_should_create_LowpanIPHC_object_when_from_bytes_classmethod_called(self):
        # GIVEN
        tf = any_tf()
        nh = any_nh()
        hlim = any_hlim()
        cid = any_cid()
        sac = any_sac()
        sam = any_sam()
        m = any_m()
        dac = any_dac()
        dam = any_dam()

        byte0 = (3 << 5) | (tf << 3) | (nh << 2) | hlim
        byte1 = (cid << 7) | (sac << 6) | (sam << 4) | (m << 3) | (dac << 2) | dam

        data_bytes = bytearray([byte0, byte1])

        # WHEN
        actual = lowpan.LowpanIPHC.from_bytes(data_bytes)

        # THEN
        self.assertEqual(tf, actual.tf)
        self.assertEqual(nh, actual.nh)
        self.assertEqual(hlim, actual.hlim)
        self.assertEqual(cid, actual.cid)
        self.assertEqual(sac, actual.sac)
        self.assertEqual(sam, actual.sam)
        self.assertEqual(m, actual.m)
        self.assertEqual(dac, actual.dac)
        self.assertEqual(dam, actual.dam)


class TestLowpanParser(unittest.TestCase):

    def test_should_parse_6lo_with_mesh_hdr_that_contains_hlim_stored_on_2_bytes_when_decompress_method_called(self):
        # GIVEN
        lowpan_packet = bytearray([
            0xbf, 0x13, 0x90, 0x00, 0x48, 0x01, 0x7c, 0x77, 0x3f, 0xf2, 0xbf, 0xc0, 0x00, 0x24, 0xb1, 0x62, 0x44, 0x02,
            0xf0, 0xba, 0x0d, 0xff, 0x04, 0x01, 0x00, 0x02, 0x02, 0x08, 0x00, 0x07, 0x09, 0x50, 0x20, 0x00, 0x20, 0x00,
            0x08, 0x00, 0x00, 0x00
        ])

        ipv6_packet = bytearray([
            0x60, 0x00, 0x00, 0x00, 0x00, 0x21, 0x11, 0x3f, 0xfd, 0x00, 0x0d, 0xb8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0xff, 0xfe, 0x00, 0x90, 0x00, 0xfd, 0x00, 0x0d, 0xb8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff,
            0xfe, 0x00, 0x48, 0x01, 0xf0, 0xbf, 0xc0, 0x00, 0x00, 0x21, 0xe2, 0xdd, 0x62, 0x44, 0x02, 0xf0, 0xba, 0x0d,
            0xff, 0x04, 0x01, 0x00, 0x02, 0x02, 0x08, 0x00, 0x07, 0x09, 0x50, 0x20, 0x00, 0x20, 0x00, 0x08, 0x00, 0x00,
            0x00
        ])

        message_info = common.MessageInfo()
        message_info.source_mac_address = common.MacAddress.from_eui64(
            bytearray([0x00, 0x99, 0x99, 0xff, 0xfe, 0x22, 0x11, 0x00]))
        message_info.destination_mac_address = common.MacAddress.from_eui64(
            bytearray([0x34, 0x29, 0x96, 0xff, 0xfe, 0xac, 0xff, 0x17]))

        context_manager = lowpan.ContextManager()
        context_manager[0] = lowpan.Context(prefix="fd00:db8::/64")

        parser = create_default_lowpan_parser(context_manager)

        # WHEN
        actual_ipv6_packet = parser.parse(io.BytesIO(lowpan_packet), message_info)

        # THEN
        self.assertEqual(ipv6_packet, actual_ipv6_packet.to_bytes())

    def test_should_parse_6lo_with_uncompressed_udp_and_without_hbh_when_decompress_method_called(self):
        # GIVEN
        lowpan_packet = bytearray([
            0x7a, 0x33, 0x11, 0x16, 0x33, 0x16, 0x34, 0x00, 0x14, 0xcf, 0x63, 0x80, 0x00, 0xfa, 0xa5, 0x0b, 0xc0, 0x00,
            0x04, 0x4e, 0x92, 0xbb, 0x53
        ])

        ipv6_packet = bytearray([
            0x60, 0x00, 0x00, 0x00, 0x00, 0x14, 0x11, 0x40, 0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x99,
            0x99, 0xff, 0xfe, 0x22, 0x11, 0x00, 0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x29, 0x96, 0xff,
            0xfe, 0xac, 0xff, 0x17, 0x16, 0x33, 0x16, 0x34, 0x00, 0x14, 0xcf, 0x63, 0x80, 0x00, 0xfa, 0xa5, 0x0b, 0xc0,
            0x00, 0x04, 0x4e, 0x92, 0xbb, 0x53
        ])

        message_info = common.MessageInfo()
        message_info.source_mac_address = common.MacAddress.from_eui64(
            bytearray([0x00, 0x99, 0x99, 0xff, 0xfe, 0x22, 0x11, 0x00]))
        message_info.destination_mac_address = common.MacAddress.from_eui64(
            bytearray([0x34, 0x29, 0x96, 0xff, 0xfe, 0xac, 0xff, 0x17]))

        parser = create_default_lowpan_parser(context_manager=None)

        # WHEN
        actual_ipv6_packet = parser.parse(io.BytesIO(lowpan_packet), message_info)

        # THEN
        self.assertEqual(ipv6_packet, actual_ipv6_packet.to_bytes())

    def test_should_parse_6lo_with_compressed_udp_and_without_hbh_when_decompress_method_called(self):
        # GIVEN
        lowpan_packet = bytearray([
            0x7e, 0x33, 0xf0, 0x16, 0x33, 0x16, 0x34, 0x04, 0xd2, 0x80, 0x00, 0xfa, 0xa5, 0x0b, 0xc0, 0x00, 0x04, 0x4e,
            0x92, 0xbb, 0x53
        ])

        ipv6_packet = bytearray([
            0x60, 0x00, 0x00, 0x00, 0x00, 0x14, 0x11, 0x40, 0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x99,
            0x99, 0xff, 0xfe, 0x22, 0x11, 0x00, 0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x29, 0x96, 0xff,
            0xfe, 0xac, 0xff, 0x17, 0x16, 0x33, 0x16, 0x34, 0x00, 0x14, 0xcf, 0x63, 0x80, 0x00, 0xfa, 0xa5, 0x0b, 0xc0,
            0x00, 0x04, 0x4e, 0x92, 0xbb, 0x53
        ])

        message_info = common.MessageInfo()
        message_info.source_mac_address = common.MacAddress.from_eui64(
            bytearray([0x00, 0x99, 0x99, 0xff, 0xfe, 0x22, 0x11, 0x00]))
        message_info.destination_mac_address = common.MacAddress.from_eui64(
            bytearray([0x34, 0x29, 0x96, 0xff, 0xfe, 0xac, 0xff, 0x17]))

        parser = create_default_lowpan_parser(context_manager=None)

        # WHEN
        actual_ipv6_packet = parser.parse(io.BytesIO(lowpan_packet), message_info)

        # THEN
        self.assertEqual(ipv6_packet, actual_ipv6_packet.to_bytes())

    def test_should_parse_6lo_with_uncompressed_udp_and_with_uncompressed_hbh_when_decompress_method_called(self):
        # GIVEN
        lowpan_packet = bytearray([
            0x7a, 0x33, 0x00, 0x11, 0x00, 0x6d, 0x04, 0x40, 0x02, 0x00, 0x18, 0x16, 0x33, 0x16, 0x34, 0x00, 0x0c, 0x04,
            0xd2, 0x80, 0x00, 0xfa, 0xa5, 0x0b, 0xc0, 0x00, 0x04, 0x4e, 0x92, 0xbb, 0x53
        ])

        ipv6_packet = bytearray([
            0x60, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x40, 0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x99,
            0x99, 0xff, 0xfe, 0x22, 0x11, 0x00, 0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x29, 0x96, 0xff,
            0xfe, 0xac, 0xff, 0x17, 0x11, 0x00, 0x6d, 0x04, 0x40, 0x02, 0x00, 0x18, 0x16, 0x33, 0x16, 0x34, 0x00, 0x14,
            0xcf, 0x63, 0x80, 0x00, 0xfa, 0xa5, 0x0b, 0xc0, 0x00, 0x04, 0x4e, 0x92, 0xbb, 0x53
        ])

        message_info = common.MessageInfo()
        message_info.source_mac_address = common.MacAddress.from_eui64(
            bytearray([0x00, 0x99, 0x99, 0xff, 0xfe, 0x22, 0x11, 0x00]))
        message_info.destination_mac_address = common.MacAddress.from_eui64(
            bytearray([0x34, 0x29, 0x96, 0xff, 0xfe, 0xac, 0xff, 0x17]))

        parser = create_default_lowpan_parser(context_manager=None)

        # WHEN
        actual_ipv6_packet = parser.parse(io.BytesIO(lowpan_packet), message_info)

        # THEN
        self.assertEqual(ipv6_packet, actual_ipv6_packet.to_bytes())

    def test_should_parse_6lo_with_uncompressed_udp_and_with_compressed_hbh_when_decompress_method_called(self):
        # GIVEN
        lowpan_packet = bytearray([
            0x7e, 0x33, 0xe0, 0x11, 0x06, 0x6d, 0x04, 0x40, 0x02, 0x00, 0x18, 0x16, 0x33, 0x16, 0x34, 0x00, 0x0c, 0x04,
            0xd2, 0x80, 0x00, 0xfa, 0xa5, 0x0b, 0xc0, 0x00, 0x04, 0x4e, 0x92, 0xbb, 0x53
        ])

        ipv6_packet = bytearray([
            0x60, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x40, 0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x99,
            0x99, 0xff, 0xfe, 0x22, 0x11, 0x00, 0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x29, 0x96, 0xff,
            0xfe, 0xac, 0xff, 0x17, 0x11, 0x00, 0x6d, 0x04, 0x40, 0x02, 0x00, 0x18, 0x16, 0x33, 0x16, 0x34, 0x00, 0x14,
            0xcf, 0x63, 0x80, 0x00, 0xfa, 0xa5, 0x0b, 0xc0, 0x00, 0x04, 0x4e, 0x92, 0xbb, 0x53
        ])

        message_info = common.MessageInfo()
        message_info.source_mac_address = common.MacAddress.from_eui64(
            bytearray([0x00, 0x99, 0x99, 0xff, 0xfe, 0x22, 0x11, 0x00]))
        message_info.destination_mac_address = common.MacAddress.from_eui64(
            bytearray([0x34, 0x29, 0x96, 0xff, 0xfe, 0xac, 0xff, 0x17]))

        parser = create_default_lowpan_parser(context_manager=None)

        # WHEN
        actual_ipv6_packet = parser.parse(io.BytesIO(lowpan_packet), message_info)

        # THEN
        self.assertEqual(ipv6_packet, actual_ipv6_packet.to_bytes())

    def test_should_parse_6lo_with_compressed_udp_and_with_compressed_hbh_when_decompress_method_called(self):
        # GIVEN
        lowpan_packet = bytearray([
            0x7e, 0x33, 0xe1, 0x06, 0x6d, 0x04, 0x40, 0x02, 0x00, 0x18, 0xf0, 0x16, 0x33, 0x16, 0x34, 0x04, 0xd2, 0x80,
            0x00, 0xfa, 0xa5, 0x0b, 0xc0, 0x00, 0x04, 0x4e, 0x92, 0xbb, 0x53
        ])

        ipv6_packet = bytearray([
            0x60, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x40, 0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x99,
            0x99, 0xff, 0xfe, 0x22, 0x11, 0x00, 0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x29, 0x96, 0xff,
            0xfe, 0xac, 0xff, 0x17, 0x11, 0x00, 0x6d, 0x04, 0x40, 0x02, 0x00, 0x18, 0x16, 0x33, 0x16, 0x34, 0x00, 0x14,
            0xcf, 0x63, 0x80, 0x00, 0xfa, 0xa5, 0x0b, 0xc0, 0x00, 0x04, 0x4e, 0x92, 0xbb, 0x53
        ])

        message_info = common.MessageInfo()
        message_info.source_mac_address = common.MacAddress.from_eui64(
            bytearray([0x00, 0x99, 0x99, 0xff, 0xfe, 0x22, 0x11, 0x00]))
        message_info.destination_mac_address = common.MacAddress.from_eui64(
            bytearray([0x34, 0x29, 0x96, 0xff, 0xfe, 0xac, 0xff, 0x17]))

        parser = create_default_lowpan_parser(context_manager=None)

        # WHEN
        actual_ipv6_packet = parser.parse(io.BytesIO(lowpan_packet), message_info)

        # THEN
        self.assertEqual(ipv6_packet, actual_ipv6_packet.to_bytes())

    def test_should_parse_6lo_with_compressed_icmp_and_without_compressed_hbh_when_decompress_method_called(self):
        # GIVEN
        lowpan_packet = bytearray([
            0x7a, 0xd5, 0xaa, 0x3a, 0x02, 0x99, 0x99, 0xff, 0xfe, 0x22, 0x11, 0x01, 0x36, 0x29, 0x96, 0xff, 0xfe, 0xac,
            0xff, 0x18, 0x80, 0x00, 0xfa, 0xa5, 0x0b, 0xc0, 0x00, 0x04, 0x4e, 0x92, 0xbb, 0x53
        ])

        ipv6_packet = bytearray([
            0x60, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x3a, 0x40, 0x20, 0x00, 0x0d, 0xb8, 0x00, 0x00, 0x00, 0x00, 0x02, 0x99,
            0x99, 0xff, 0xfe, 0x22, 0x11, 0x01, 0x20, 0x00, 0x0d, 0xb8, 0x00, 0x00, 0x00, 0x00, 0x36, 0x29, 0x96, 0xff,
            0xfe, 0xac, 0xff, 0x18, 0x80, 0x00, 0x97, 0xf3, 0x0b, 0xc0, 0x00, 0x04, 0x4e, 0x92, 0xbb, 0x53
        ])

        message_info = common.MessageInfo()
        message_info.source_mac_address = common.MacAddress.from_eui64(
            bytearray([0x00, 0x99, 0x99, 0xff, 0xfe, 0x22, 0x11, 0x00]))
        message_info.destination_mac_address = common.MacAddress.from_eui64(
            bytearray([0x34, 0x29, 0x96, 0xff, 0xfe, 0xac, 0xff, 0x17]))

        context_manager = lowpan.ContextManager()
        context_manager[10] = lowpan.Context(prefix="2000:0db8::/64")

        parser = create_default_lowpan_parser(context_manager)

        # WHEN
        actual_ipv6_packet = parser.parse(io.BytesIO(lowpan_packet), message_info)

        # THEN
        self.assertEqual(ipv6_packet, actual_ipv6_packet.to_bytes())

    def test_should_parse_6lo_with_compressed_icmp_and_without_compressed_hbh_when_decompress_method_called_1(self):
        # GIVEN
        lowpan_packet = bytearray([
            0x7a, 0xd5, 0xaa, 0x3a, 0x02, 0x99, 0x99, 0xff, 0xfe, 0x22, 0x11, 0x01, 0x36, 0x29, 0x96, 0xff, 0xfe, 0xac,
            0xff, 0x18, 0x80, 0x00, 0xfa, 0xa5, 0x0b, 0xc0, 0x00, 0x04, 0x4e, 0x92, 0xbb, 0x53
        ])

        ipv6_packet = bytearray([
            0x60, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x3a, 0x40, 0x20, 0x00, 0x0d, 0xb8, 0x00, 0x00, 0x00, 0x00, 0x02, 0x99,
            0x99, 0xff, 0xfe, 0x22, 0x11, 0x01, 0x20, 0x00, 0x0d, 0xb8, 0x00, 0x00, 0x00, 0x00, 0x36, 0x29, 0x96, 0xff,
            0xfe, 0xac, 0xff, 0x18, 0x80, 0x00, 0x97, 0xf3, 0x0b, 0xc0, 0x00, 0x04, 0x4e, 0x92, 0xbb, 0x53
        ])

        message_info = common.MessageInfo()
        message_info.source_mac_address = common.MacAddress.from_eui64(
            bytearray([0x00, 0x99, 0x99, 0xff, 0xfe, 0x22, 0x11, 0x00]))
        message_info.destination_mac_address = common.MacAddress.from_eui64(
            bytearray([0x34, 0x29, 0x96, 0xff, 0xfe, 0xac, 0xff, 0x17]))

        context_manager = lowpan.ContextManager()
        context_manager[10] = lowpan.Context(prefix="2000:0db8::/64")

        parser = create_default_lowpan_parser(context_manager)

        # WHEN
        actual_ipv6_packet = parser.parse(io.BytesIO(lowpan_packet), message_info)

        # THEN
        self.assertEqual(ipv6_packet, actual_ipv6_packet.to_bytes())

    def test_should_parse_6lo_with_compressed_icmp_and_without_compressed_hbh_when_decompress_method_called_2(self):
        # GIVEN
        lowpan_packet = bytearray([
            0x7a, 0xf0, 0xa0, 0x3a, 0x20, 0x0d, 0x14, 0x56, 0x12, 0x55, 0x00, 0x00, 0x25, 0x14, 0x46, 0xff, 0xfe, 0xdd,
            0x2a, 0xfe, 0x80, 0x00, 0xfa, 0xa5, 0x0b, 0xc0, 0x00, 0x04, 0x4e, 0x92, 0xbb, 0x53
        ])

        ipv6_packet = bytearray([
            0x60, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x3a, 0x40, 0x20, 0x00, 0x0d, 0xb8, 0x00, 0x00, 0x00, 0x00, 0x02, 0x99,
            0x99, 0xff, 0xfe, 0x22, 0x11, 0x00, 0x20, 0x0d, 0x14, 0x56, 0x12, 0x55, 0x00, 0x00, 0x25, 0x14, 0x46, 0xff,
            0xfe, 0xdd, 0x2a, 0xfe, 0x80, 0x00, 0xb3, 0xf3, 0x0b, 0xc0, 0x00, 0x04, 0x4e, 0x92, 0xbb, 0x53
        ])

        message_info = common.MessageInfo()
        message_info.source_mac_address = common.MacAddress.from_eui64(
            bytearray([0x00, 0x99, 0x99, 0xff, 0xfe, 0x22, 0x11, 0x00]))
        message_info.destination_mac_address = common.MacAddress.from_eui64(
            bytearray([0x34, 0x29, 0x96, 0xff, 0xfe, 0xac, 0xff, 0x17]))

        context_manager = lowpan.ContextManager()
        context_manager[10] = lowpan.Context(prefix="2000:0db8::/64")

        parser = create_default_lowpan_parser(context_manager)

        # WHEN
        actual_ipv6_packet = parser.parse(io.BytesIO(lowpan_packet), message_info)

        # THEN
        self.assertEqual(ipv6_packet, actual_ipv6_packet.to_bytes())

    def test_should_parse_6lo_with_compressed_icmp_and_without_compressed_hbh_when_decompress_method_called_3(self):
        # GIVEN
        lowpan_packet = bytearray([
            0x7a, 0xd5, 0xaa, 0x3a, 0x02, 0x99, 0x99, 0xff, 0xfe, 0x22, 0x11, 0x01, 0x36, 0x29, 0x96, 0xff, 0xfe, 0xac,
            0xff, 0x18, 0x80, 0x00, 0xfa, 0xa5, 0x0b, 0xc0, 0x00, 0x04, 0x4e, 0x92, 0xbb, 0x53
        ])

        ipv6_packet = bytearray([
            0x60, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x3a, 0x40, 0x20, 0x00, 0x0d, 0xb8, 0x00, 0x00, 0x00, 0x00, 0x02, 0x99,
            0x99, 0xff, 0xfe, 0x22, 0x11, 0x01, 0x20, 0x00, 0x0d, 0xb8, 0x00, 0x00, 0x00, 0x00, 0x36, 0x29, 0x96, 0xff,
            0xfe, 0xac, 0xff, 0x18, 0x80, 0x00, 0x97, 0xf3, 0x0b, 0xc0, 0x00, 0x04, 0x4e, 0x92, 0xbb, 0x53
        ])

        message_info = common.MessageInfo()
        message_info.source_mac_address = common.MacAddress.from_eui64(
            bytearray([0x00, 0x99, 0x99, 0xff, 0xfe, 0x22, 0x11, 0x00]))
        message_info.destination_mac_address = common.MacAddress.from_eui64(
            bytearray([0x34, 0x29, 0x96, 0xff, 0xfe, 0xac, 0xff, 0x17]))

        context_manager = lowpan.ContextManager()
        context_manager[10] = lowpan.Context(prefix="2000:0db8::/64")

        parser = create_default_lowpan_parser(context_manager)

        # WHEN
        actual_ipv6_packet = parser.parse(io.BytesIO(lowpan_packet), message_info)

        # THEN
        self.assertEqual(ipv6_packet, actual_ipv6_packet.to_bytes())

    def test_should_parse_6lo_with_compressed_icmp_and_without_compressed_hbh_when_decompress_method_called_4(self):
        # GIVEN
        lowpan_packet = bytearray([
            0x7a, 0xf5, 0xaa, 0x3a, 0x36, 0x29, 0x96, 0xff, 0xfe, 0xac, 0xff, 0x18, 0x80, 0x00, 0xfa, 0xa5, 0x0b, 0xc0,
            0x00, 0x04, 0x4e, 0x92, 0xbb, 0x53
        ])

        ipv6_packet = bytearray([
            0x60, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x3a, 0x40, 0x20, 0x00, 0x0d, 0xb8, 0x00, 0x00, 0x00, 0x00, 0x02, 0x99,
            0x99, 0xff, 0xfe, 0x22, 0x11, 0x00, 0x20, 0x00, 0x0d, 0xb8, 0x00, 0x00, 0x00, 0x00, 0x36, 0x29, 0x96, 0xff,
            0xfe, 0xac, 0xff, 0x18, 0x80, 0x00, 0x97, 0xf4, 0x0b, 0xc0, 0x00, 0x04, 0x4e, 0x92, 0xbb, 0x53
        ])

        message_info = common.MessageInfo()
        message_info.source_mac_address = common.MacAddress.from_eui64(
            bytearray([0x00, 0x99, 0x99, 0xff, 0xfe, 0x22, 0x11, 0x00]))
        message_info.destination_mac_address = common.MacAddress.from_eui64(
            bytearray([0x34, 0x29, 0x96, 0xff, 0xfe, 0xac, 0xff, 0x17]))

        context_manager = lowpan.ContextManager()
        context_manager[10] = lowpan.Context(prefix="2000:0db8::/64")

        parser = create_default_lowpan_parser(context_manager)

        # WHEN
        actual_ipv6_packet = parser.parse(io.BytesIO(lowpan_packet), message_info)

        # THEN
        self.assertEqual(ipv6_packet, actual_ipv6_packet.to_bytes())

    def test_should_parse_6lo_with_compressed_icmp_and_without_compressed_hbh_when_decompress_method_called_5(self):
        # GIVEN
        lowpan_packet = bytearray(
            [0x7a, 0xf7, 0xac, 0x3a, 0x80, 0x00, 0xfa, 0xa5, 0x0b, 0xc0, 0x00, 0x04, 0x4e, 0x92, 0xbb, 0x53])

        ipv6_packet = bytearray([
            0x60, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x3a, 0x40, 0x20, 0x00, 0x0d, 0xb8, 0x00, 0x00, 0x00, 0x00, 0x02, 0x99,
            0x99, 0xff, 0xfe, 0x22, 0x11, 0x00, 0x20, 0x0d, 0x14, 0x56, 0x12, 0x55, 0x00, 0x00, 0x25, 0x14, 0x46, 0xff,
            0xfe, 0xdd, 0x2a, 0xfe, 0x80, 0x00, 0xb3, 0xf3, 0x0b, 0xc0, 0x00, 0x04, 0x4e, 0x92, 0xbb, 0x53
        ])

        message_info = common.MessageInfo()
        message_info.source_mac_address = common.MacAddress.from_eui64(
            bytearray([0x00, 0x99, 0x99, 0xff, 0xfe, 0x22, 0x11, 0x00]))
        message_info.destination_mac_address = common.MacAddress.from_eui64(
            bytearray([0x34, 0x29, 0x96, 0xff, 0xfe, 0xac, 0xff, 0x17]))

        context_manager = lowpan.ContextManager()
        context_manager[10] = lowpan.Context(prefix="2000:0db8::/64")
        context_manager[12] = lowpan.Context(prefix="200d:1456:1255:0000:2514:46ff:fedd:2afe/128")

        parser = create_default_lowpan_parser(context_manager)

        # WHEN
        actual_ipv6_packet = parser.parse(io.BytesIO(lowpan_packet), message_info)

        # THEN
        self.assertEqual(ipv6_packet, actual_ipv6_packet.to_bytes())

    def test_should_parse_6lo_with_compressed_icmp_and_without_compressed_hbh_when_decompress_method_called_6(self):
        # GIVEN
        lowpan_packet = bytearray([
            0x7a, 0xf0, 0xc0, 0x3a, 0x20, 0x0d, 0x14, 0x56, 0x12, 0x54, 0x00, 0x00, 0x12, 0x54, 0x11, 0xff, 0xfe, 0x1c,
            0x7e, 0xff, 0x80, 0x00, 0xfa, 0xa5, 0x0b, 0xc0, 0x00, 0x04, 0x4e, 0x92, 0xbb, 0x53
        ])

        ipv6_packet = bytearray([
            0x60, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x3a, 0x40, 0x20, 0x0d, 0x14, 0x56, 0x12, 0x55, 0x00, 0x00, 0x25, 0x14,
            0x46, 0xff, 0xfe, 0xdd, 0x2a, 0xfe, 0x20, 0x0d, 0x14, 0x56, 0x12, 0x54, 0x00, 0x00, 0x12, 0x54, 0x11, 0xff,
            0xfe, 0x1c, 0x7e, 0xff, 0x80, 0x00, 0xa5, 0x40, 0x0b, 0xc0, 0x00, 0x04, 0x4e, 0x92, 0xbb, 0x53
        ])

        message_info = common.MessageInfo()
        message_info.source_mac_address = common.MacAddress.from_eui64(
            bytearray([0x00, 0x99, 0x99, 0xff, 0xfe, 0x22, 0x11, 0x00]))
        message_info.destination_mac_address = common.MacAddress.from_eui64(
            bytearray([0x34, 0x29, 0x96, 0xff, 0xfe, 0xac, 0xff, 0x17]))

        context_manager = lowpan.ContextManager()
        context_manager[12] = lowpan.Context(prefix="200d:1456:1255:0000:2514:46ff:fedd:2afe/128")

        parser = create_default_lowpan_parser(context_manager)

        # WHEN
        actual_ipv6_packet = parser.parse(io.BytesIO(lowpan_packet), message_info)

        # THEN
        self.assertEqual(ipv6_packet, actual_ipv6_packet.to_bytes())

    def test_should_parse_6lo_with_compressed_icmp_and_without_compressed_hbh_when_decompress_method_called_7(self):
        # GIVEN
        lowpan_packet = bytearray([
            0x7a, 0xd0, 0xd0, 0x3a, 0x00, 0x02, 0x98, 0xff, 0xfe, 0x22, 0x12, 0x00, 0x20, 0x0d, 0x14, 0x56, 0x12, 0x55,
            0x00, 0x00, 0x25, 0x14, 0x46, 0xff, 0xfe, 0xdd, 0x2a, 0xfe, 0x80, 0x00, 0xfa, 0xa5, 0x0b, 0xc0, 0x00, 0x04,
            0x4e, 0x92, 0xbb, 0x53
        ])

        ipv6_packet = bytearray([
            0x60, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x3a, 0x40, 0xaa, 0xbb, 0xcc, 0xdd, 0x00, 0x00, 0x00, 0x00, 0x77, 0x82,
            0x98, 0xff, 0xfe, 0x22, 0x12, 0x00, 0x20, 0x0d, 0x14, 0x56, 0x12, 0x55, 0x00, 0x00, 0x25, 0x14, 0x46, 0xff,
            0xfe, 0xdd, 0x2a, 0xfe, 0x80, 0x00, 0xf5, 0x28, 0x0b, 0xc0, 0x00, 0x04, 0x4e, 0x92, 0xbb, 0x53
        ])

        message_info = common.MessageInfo()
        message_info.source_mac_address = common.MacAddress.from_eui64(
            bytearray([0x00, 0x99, 0x99, 0xff, 0xfe, 0x22, 0x11, 0x00]))
        message_info.destination_mac_address = common.MacAddress.from_eui64(
            bytearray([0x34, 0x29, 0x96, 0xff, 0xfe, 0xac, 0xff, 0x17]))

        context_manager = lowpan.ContextManager()
        context_manager[13] = lowpan.Context(prefix="AABB:CCDD:0000:0000:7796::/75")

        parser = create_default_lowpan_parser(context_manager)

        # WHEN
        actual_ipv6_packet = parser.parse(io.BytesIO(lowpan_packet), message_info)

        # THEN
        self.assertEqual(ipv6_packet, actual_ipv6_packet.to_bytes())

    def test_should_parse_6lo_with_compressed_icmp_and_without_compressed_hbh_when_decompress_method_called_8(self):
        # GIVEN
        lowpan_packet = bytearray([
            0x7a, 0xf0, 0xd0, 0x3a, 0x20, 0x0d, 0x14, 0x56, 0x12, 0x55, 0x00, 0x00, 0x25, 0x14, 0x46, 0xff, 0xfe, 0xdd,
            0x2a, 0xfe, 0x80, 0x00, 0xfa, 0xa5, 0x0b, 0xc0, 0x00, 0x04, 0x4e, 0x92, 0xbb, 0x53
        ])

        ipv6_packet = bytearray([
            0x60, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x3a, 0x40, 0xaa, 0xbb, 0xcc, 0xdd, 0x00, 0x00, 0x00, 0x00, 0x77, 0x99,
            0x99, 0xff, 0xfe, 0x22, 0x11, 0x00, 0x20, 0x0d, 0x14, 0x56, 0x12, 0x55, 0x00, 0x00, 0x25, 0x14, 0x46, 0xff,
            0xfe, 0xdd, 0x2a, 0xfe, 0x80, 0x00, 0xf5, 0x11, 0x0b, 0xc0, 0x00, 0x04, 0x4e, 0x92, 0xbb, 0x53
        ])

        message_info = common.MessageInfo()
        message_info.source_mac_address = common.MacAddress.from_eui64(
            bytearray([0x00, 0x99, 0x99, 0xff, 0xfe, 0x22, 0x11, 0x00]))
        message_info.destination_mac_address = common.MacAddress.from_eui64(
            bytearray([0x34, 0x29, 0x96, 0xff, 0xfe, 0xac, 0xff, 0x17]))

        context_manager = lowpan.ContextManager()
        context_manager[13] = lowpan.Context(prefix="AABB:CCDD:0000:0000:7796::/75")

        parser = create_default_lowpan_parser(context_manager)

        # WHEN
        actual_ipv6_packet = parser.parse(io.BytesIO(lowpan_packet), message_info)

        # THEN
        self.assertEqual(ipv6_packet, actual_ipv6_packet.to_bytes())

    def test_should_parse_6lo_with_compressed_icmp_and_without_compressed_hbh_when_decompress_method_called_9(self):
        # GIVEN
        lowpan_packet = bytearray([
            0x7a, 0xf0, 0xd0, 0x3a, 0x20, 0x0d, 0x14, 0x56, 0x12, 0x55, 0x00, 0x00, 0x25, 0x14, 0x46, 0xff, 0xfe, 0xdd,
            0x2a, 0xfe, 0x80, 0x00, 0xfa, 0xa5, 0x0b, 0xc0, 0x00, 0x04, 0x4e, 0x92, 0xbb, 0x53
        ])

        ipv6_packet = bytearray([
            0x60, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x3a, 0x40, 0xaa, 0xbb, 0xcc, 0xdd, 0x00, 0x00, 0x00, 0x00, 0x77, 0x99,
            0x99, 0xff, 0xfe, 0x22, 0x11, 0x00, 0x20, 0x0d, 0x14, 0x56, 0x12, 0x55, 0x00, 0x00, 0x25, 0x14, 0x46, 0xff,
            0xfe, 0xdd, 0x2a, 0xfe, 0x80, 0x00, 0xf5, 0x11, 0x0b, 0xc0, 0x00, 0x04, 0x4e, 0x92, 0xbb, 0x53
        ])

        message_info = common.MessageInfo()
        message_info.source_mac_address = common.MacAddress.from_eui64(
            bytearray([0x00, 0x99, 0x99, 0xff, 0xfe, 0x22, 0x11, 0x00]))
        message_info.destination_mac_address = common.MacAddress.from_eui64(
            bytearray([0x34, 0x29, 0x96, 0xff, 0xfe, 0xac, 0xff, 0x17]))
        context_manager = lowpan.ContextManager()
        context_manager[13] = lowpan.Context(prefix="AABB:CCDD:0000:0000:7796::/75")

        parser = create_default_lowpan_parser(context_manager)

        # WHEN
        actual_ipv6_packet = parser.parse(io.BytesIO(lowpan_packet), message_info)

        # THEN
        self.assertEqual(ipv6_packet, actual_ipv6_packet.to_bytes())

    def test_should_defragment_big_IPv6_packet_when_parse_method_called_with_fragments_in_random_order(self):
        # GIVEN
        fragment_1 = bytearray([
            0xC5, 0x00, 0x31, 0x9F, 0x7A, 0x33, 0x3A, 0x80, 0x00, 0xFA, 0xA5, 0x0B, 0xC0, 0x00, 0x04, 0x4E, 0x92, 0xBB,
            0x53, 0x11, 0x44, 0x66, 0x4E, 0x92, 0xBB, 0x53, 0x1A, 0x44, 0x66, 0x77, 0x99, 0x15, 0xB3, 0x00, 0x54, 0xCC,
            0x54, 0x01, 0xAA, 0x44, 0x54, 0x12, 0x43, 0x53, 0x11, 0x44, 0x66, 0x4E, 0x92, 0xBB, 0x53, 0x1A, 0x44, 0x66,
            0x77, 0x99, 0x1A, 0x92, 0xBB, 0x53, 0x11, 0x44, 0x66, 0x92, 0xBB, 0x53, 0x1A, 0x44, 0x66, 0x77, 0x99, 0x15,
            0xB3, 0x00, 0x54, 0xCC, 0x54, 0x01, 0xAA, 0x11, 0x44, 0x66, 0x4E, 0x92, 0xBB, 0x53, 0x1A, 0x80, 0x00, 0xFA,
            0xA5, 0x0B, 0xC0, 0x00, 0x04, 0x4E, 0x92, 0xBB, 0x53, 0x11, 0x4C, 0x66, 0x4E
        ])

        fragment_2 = bytearray([
            0xE5, 0x00, 0x31, 0x9F, 0x11, 0x92, 0xBB, 0x53, 0x1A, 0x44, 0x66, 0x77, 0x99, 0x15, 0xB3, 0x00, 0x54, 0xCC,
            0x54, 0x01, 0xAA, 0x44, 0x54, 0x12, 0xa3, 0x53, 0x11, 0x44, 0x66, 0xFE, 0x92, 0xBB, 0x53, 0x1A, 0x44, 0x66,
            0x77, 0x99, 0x1A, 0x92, 0xBB, 0x53, 0x11, 0x44, 0x66, 0x92, 0xBB, 0x53, 0x1A, 0x44, 0x66, 0x77, 0x99, 0x15,
            0xB3, 0x00, 0x54, 0xCC, 0x54, 0x01, 0xAA, 0x11, 0x44, 0x66, 0x4E, 0x92, 0xBB, 0x53, 0x1A, 0x80, 0x00, 0xFA,
            0xA5, 0x0B, 0xC0, 0x00, 0x04, 0x4E, 0x92, 0x1B, 0x53, 0x11, 0x44, 0x66, 0x4E, 0x22, 0xBB, 0x53, 0x1A, 0x44,
            0x66, 0x77, 0x99, 0x15, 0xB3, 0x00, 0x54, 0xCC, 0x54, 0x01, 0xAA
        ])

        fragment_3 = bytearray([
            0xE5, 0x00, 0x31, 0x9F, 0x1D, 0x44, 0x54, 0x12, 0xD3, 0x53, 0x11, 0x44, 0x66, 0x4E, 0x92, 0xBB, 0x53, 0x1A,
            0x44, 0x66, 0x77, 0x99, 0x1A, 0x92, 0xBB, 0x53, 0x11, 0x44, 0x66, 0x92, 0xBB, 0x53, 0x1A, 0x44, 0x66, 0x77,
            0x99, 0x15, 0xB3, 0x00, 0x54, 0xCC, 0x54, 0x01, 0xAA, 0x11, 0x44, 0x66, 0x4E, 0x92, 0xBB, 0x53, 0x1A, 0xC0,
            0x00, 0xFA, 0xA5, 0x0B, 0xC0, 0x00, 0x04, 0x4E, 0x92, 0xBB, 0x53, 0x11, 0x44, 0xCC, 0x4E, 0x92, 0xBB, 0x53,
            0x1A, 0x44, 0x66, 0x77, 0x99, 0x15, 0xB3, 0x00, 0x54, 0xCC, 0x54, 0x01, 0xAA, 0x44, 0x54, 0x12, 0x43, 0x53,
            0x11, 0x44, 0x66, 0x4E, 0x92, 0xBC, 0x53, 0x1A, 0x44, 0x66, 0x77
        ])

        fragment_4 = bytearray([
            0xE5, 0x00, 0x31, 0x9F, 0x29, 0x99, 0x1A, 0x92, 0xBB, 0x53, 0x11, 0x44, 0x66, 0x92, 0xBB, 0x53, 0x1A, 0x44,
            0x66, 0x77, 0x99, 0x15, 0xB3, 0x00, 0x54, 0xCC, 0x54, 0x01, 0xAA, 0x11, 0x44, 0x66, 0x4E, 0x92, 0xBB, 0x53,
            0x1A, 0x80, 0x00, 0xFA, 0xA5, 0x0B, 0xC0, 0x00, 0x04, 0x4E, 0x92, 0xBB, 0x53, 0x11, 0x44, 0x66, 0x4E, 0x92,
            0xBB, 0x53, 0x1A, 0x44, 0x66, 0x77, 0x99, 0x15, 0xB3, 0x00, 0x54, 0xCC, 0x54, 0x01, 0xAA, 0x44, 0x54, 0x12,
            0x43, 0x53, 0x11, 0x44, 0x66, 0x4E, 0x92, 0xBB, 0x53, 0x1A, 0x44, 0x66, 0x77, 0x99, 0x1A, 0x92, 0xBB, 0x53,
            0x11, 0x44, 0x66, 0x92, 0xBB, 0x53, 0x1A, 0x44, 0x66, 0x77, 0x99
        ])

        fragment_5 = bytearray([
            0xE5, 0x00, 0x31, 0x9F, 0x35, 0x15, 0xB3, 0x00, 0x54, 0xCC, 0x54, 0x01, 0xAA, 0x11, 0x44, 0x66, 0x4E, 0x92,
            0xBB, 0x53, 0x1A, 0x80, 0x00, 0xFA, 0xA5, 0x0B, 0xC0, 0x00, 0x04, 0x4E, 0x92, 0xBB, 0x53, 0x11, 0x4C, 0x66,
            0x4E, 0x92, 0xBB, 0x53, 0x1A, 0x44, 0x66, 0x77, 0x99, 0x15, 0xB3, 0x00, 0x54, 0xCC, 0x54, 0x01, 0xAA, 0x44,
            0x54, 0x12, 0xa3, 0x53, 0x11, 0x44, 0x66, 0xFE, 0x92, 0xBB, 0x53, 0x1A, 0x44, 0x66, 0x77, 0x99, 0x1A, 0x92,
            0xBB, 0x53, 0x11, 0x44, 0x66, 0x92, 0xBB, 0x53, 0x1A, 0x44, 0x66, 0x77, 0x99, 0x15, 0xB3, 0x00, 0x54, 0xCC,
            0x54, 0x01, 0xAA, 0x11, 0x44, 0x66, 0x4E, 0x92, 0xBB, 0x53, 0x1A
        ])

        fragment_6 = bytearray([
            0xE5, 0x00, 0x31, 0x9F, 0x41, 0x80, 0x00, 0xFA, 0xA5, 0x0B, 0xC0, 0x00, 0x04, 0x4E, 0x92, 0x1B, 0x53, 0x11,
            0x44, 0x66, 0x4E, 0x22, 0xBB, 0x53, 0x1A, 0x44, 0x67, 0x77, 0x99, 0x15, 0xB3, 0x00, 0x54, 0xCC, 0x54, 0x01,
            0xAA, 0x44, 0x54, 0x12, 0xD3, 0x53, 0x11, 0x44, 0x66, 0x4E, 0x92, 0xBB, 0x53, 0x1A, 0x44, 0x66, 0x77, 0x99,
            0x1A, 0x92, 0xBB, 0x53, 0x11, 0x44, 0x66, 0x92, 0xBB, 0x53, 0x1A, 0x44, 0x66, 0x77, 0x99, 0x15, 0xB3, 0x00,
            0x54, 0xCC, 0x54, 0x01, 0xAA, 0x11, 0x44, 0x66, 0x4E, 0x92, 0xBB, 0x53, 0x1A, 0xC0, 0x00, 0xFA, 0x15, 0x0B,
            0xC0, 0x00, 0x04, 0x4E, 0x92, 0xBB, 0x53, 0x11, 0x44, 0xCC, 0x4E
        ])

        fragment_7 = bytearray([
            0xE5, 0x00, 0x31, 0x9F, 0x4D, 0x92, 0xBB, 0x53, 0x1A, 0x44, 0x66, 0x77, 0x99, 0x15, 0xB3, 0x00, 0x54, 0xCC,
            0x54, 0x01, 0xAA, 0x44, 0x54, 0x12, 0x43, 0x53, 0x11, 0x44, 0x66, 0x4E, 0x92, 0xBC, 0x53, 0x1A, 0x44, 0x66,
            0x77, 0x99, 0x1A, 0x92, 0xBB, 0x53, 0x11, 0x44, 0x66, 0x92, 0xBB, 0x53, 0x1A, 0x44, 0x66, 0x77, 0x99, 0x15,
            0xB3, 0x00, 0x54, 0xCC, 0x54, 0x01, 0xAA, 0x11, 0x44, 0x66, 0x4E, 0x92, 0xBA, 0x53, 0x1A, 0x60, 0x00, 0x00,
            0x00, 0x00, 0x10, 0x3A, 0x64, 0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x11, 0x12,
            0x13, 0x14, 0x15, 0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        ])

        fragment_8 = bytearray([
            0xE5, 0x00, 0x31, 0x9F, 0x59, 0x02, 0x00, 0x1A, 0x2A, 0x3F, 0x09, 0xAB, 0x43, 0x60, 0x00, 0xF0, 0x00, 0x00,
            0x10, 0x3A, 0x64, 0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x11, 0x12, 0x13, 0x14,
            0x15, 0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x1A, 0x2A, 0x3F, 0x09, 0xAB, 0x43, 0x80,
            0x00, 0xFA, 0xA5, 0x0B, 0xC0, 0x00, 0x04, 0x4E, 0x92, 0xBB, 0x53, 0x11, 0x44, 0x66, 0x4E, 0x92, 0xBB, 0x53,
            0x1A, 0x44, 0x66, 0x77, 0x99, 0x15, 0xB3, 0x00, 0x54, 0xCC, 0x54, 0x01, 0xAA, 0x44, 0x54, 0x12, 0x43, 0x53,
            0x11, 0x44, 0x66, 0x4E, 0x92, 0xBC, 0x53, 0x1A, 0x44, 0x66, 0x77
        ])

        fragment_9 = bytearray([
            0xE5, 0x00, 0x31, 0x9F, 0x65, 0x99, 0x1A, 0x92, 0xBB, 0x53, 0x11, 0x44, 0x66, 0x92, 0xBB, 0x53, 0x1A, 0x44,
            0x66, 0x77, 0x99, 0x15, 0xB3, 0x00, 0x54, 0xCC, 0x54, 0x01, 0xAA, 0x11, 0x44, 0x66, 0x4E, 0x92, 0xBB, 0x53,
            0x1A, 0x80, 0x00, 0xFA, 0xA5, 0x0B, 0xC0, 0x00, 0x04, 0x4E, 0x92, 0xBB, 0x53, 0x11, 0x4C, 0x66, 0x4E, 0x92,
            0xBB, 0x53, 0x1A, 0x44, 0x66, 0x77, 0x99, 0x15, 0xB3, 0x00, 0x54, 0xCC, 0x54, 0x01, 0xAA, 0x44, 0x54, 0x12,
            0xa3, 0x53, 0x11, 0x44, 0x66, 0xFE, 0x92, 0xBB, 0x53, 0x1A, 0x44, 0x66, 0x77, 0x99, 0x1A, 0x92, 0xBB, 0x53,
            0x11, 0x44, 0x66, 0x92, 0xBB, 0x53, 0x1A, 0x4D, 0x66, 0x77, 0x99
        ])

        fragment_10 = bytearray([
            0xE5, 0x00, 0x31, 0x9F, 0x71, 0x15, 0xB3, 0x00, 0x54, 0xCC, 0x54, 0x01, 0xAA, 0x11, 0x44, 0x66, 0x4E, 0x92,
            0xBB, 0x53, 0x1A, 0x80, 0x00, 0xFA, 0xA5, 0x0B, 0xC0, 0x00, 0x04, 0x4E, 0x92, 0x1B, 0x53, 0x11, 0x44, 0x66,
            0x4E, 0x22, 0xBB, 0x51, 0x1A, 0x44, 0x66, 0x77, 0x99, 0x15, 0xB3, 0x00, 0x54, 0xCC, 0x54, 0x01, 0xAA, 0x44,
            0x54, 0x12, 0xD3, 0x53, 0x11, 0x44, 0x66, 0x4E, 0x92, 0xBB, 0x53, 0x1A, 0x44, 0x66, 0x77, 0x99, 0x1A, 0x92,
            0xBB, 0x53, 0x11, 0x44, 0x66, 0x92, 0xBB, 0x53, 0x1A, 0x44, 0x66, 0x77, 0x99, 0x15, 0xB3, 0x00, 0x54, 0xCC,
            0x54, 0x01, 0xAA, 0x11, 0x44, 0x66, 0x4E, 0x92, 0xBB, 0x53, 0x1A
        ])

        fragment_11 = bytearray([
            0xE5, 0x00, 0x31, 0x9F, 0x7D, 0xC0, 0x00, 0xFA, 0xA5, 0x0B, 0xC0, 0x00, 0x04, 0x4E, 0x92, 0xBB, 0x53, 0x11,
            0x44, 0xCC, 0x4E, 0x92, 0xBB, 0x53, 0x1A, 0x44, 0x66, 0x77, 0x99, 0x15, 0xB3, 0x00, 0x54, 0xCC, 0x54, 0x01,
            0xAA, 0x44, 0x54, 0x12, 0x4A, 0x53, 0x11, 0x44, 0x66, 0x4E, 0x92, 0xBC, 0x53, 0x1A, 0x44, 0x66, 0x77, 0x99,
            0x1A, 0x92, 0xBB, 0x53, 0x11, 0x44, 0x66, 0x92, 0xBB, 0x53, 0x1A, 0x44, 0x66, 0x77, 0x99, 0x15, 0xB3, 0x00,
            0x54, 0xCC, 0x54, 0x01, 0xAA, 0x11, 0x44, 0x66, 0x4E, 0x92, 0xBB, 0x53, 0x1A, 0x80, 0x00, 0xFA, 0xA5, 0x0B,
            0xC0, 0x00, 0x04, 0x4E, 0x92, 0xBB, 0x53, 0x11, 0x44, 0x66, 0x4E
        ])

        fragment_12 = bytearray([
            0xE5, 0x00, 0x31, 0x9F, 0x89, 0x92, 0xBB, 0x53, 0x1A, 0x44, 0x66, 0x77, 0x99, 0x15, 0xB3, 0x00, 0x54, 0xCC,
            0x54, 0x01, 0xAA, 0x44, 0x54, 0x12, 0x43, 0x53, 0x11, 0x44, 0x66, 0x4E, 0x92, 0xBB, 0x53, 0x3A, 0x44, 0x66,
            0x77, 0x99, 0x1A, 0x92, 0xBB, 0x53, 0x11, 0x44, 0x66, 0x92, 0xBB, 0x53, 0x1A, 0x44, 0x66, 0x77, 0x99, 0x15,
            0xB3, 0x00, 0x54, 0xCC, 0x54, 0x01, 0xAA, 0x11, 0x44, 0x66, 0x4E, 0x92, 0xBB, 0x53, 0x1A, 0x80, 0x00, 0xFA,
            0xA5, 0x0B, 0xC0, 0x00, 0x04, 0x4E, 0x92, 0xBB, 0x53, 0x11, 0x4C, 0x66, 0x4E, 0x92, 0xBB, 0x53, 0x1A, 0x44,
            0x66, 0x77, 0x99, 0x15, 0xB3, 0x00, 0x54, 0xCC, 0x54, 0x01, 0xAA
        ])

        fragment_13 = bytearray([
            0xE5, 0x00, 0x31, 0x9F, 0x95, 0x44, 0x54, 0x12, 0xa3, 0x53, 0x11, 0x44, 0x66, 0xFE, 0x92, 0xBB, 0x53, 0x1A,
            0x44, 0x66, 0x77, 0x99, 0x1A, 0x92, 0xBB, 0x53, 0x11, 0x44, 0x66, 0x92, 0xBB, 0x53, 0x1A, 0x44, 0x66, 0x77,
            0x99, 0x15, 0xB3, 0x00, 0x54, 0xCC, 0x54, 0x01, 0xAA, 0x11, 0x44, 0x66, 0x4E, 0x92, 0xBB, 0x53, 0x1A, 0x80,
            0x00, 0xFA, 0xA5, 0x1B, 0xC0, 0x00, 0x04, 0x4E, 0x92, 0x1B, 0x53, 0x11, 0x44, 0x66, 0x4E, 0x22, 0xBB, 0x53,
            0x1A, 0x44, 0x66, 0x77, 0x99, 0x15, 0xB3, 0x00, 0x54, 0xCC, 0x54, 0x01, 0xAA, 0x44, 0x54, 0x12, 0xD3, 0x53,
            0x11, 0x44, 0x66
        ])

        message_info = common.MessageInfo()
        message_info.source_mac_address = common.MacAddress.from_eui64(
            bytearray([0x00, 0x00, 0x00, 0x11, 0x12, 0x13, 0x14, 0x15]))
        message_info.destination_mac_address = common.MacAddress.from_eui64(
            bytearray([0x00, 0x00, 0x1A, 0x2A, 0x3F, 0x09, 0xAB, 0x43]))

        parser = create_default_lowpan_parser(context_manager=None)

        # WHEN
        self.assertIsNone(parser.parse(io.BytesIO(fragment_4), message_info))
        self.assertIsNone(parser.parse(io.BytesIO(fragment_2), message_info))
        self.assertIsNone(parser.parse(io.BytesIO(fragment_3), message_info))
        self.assertIsNone(parser.parse(io.BytesIO(fragment_13), message_info))
        self.assertIsNone(parser.parse(io.BytesIO(fragment_5), message_info))
        self.assertIsNone(parser.parse(io.BytesIO(fragment_6), message_info))
        self.assertIsNone(parser.parse(io.BytesIO(fragment_7), message_info))
        self.assertIsNone(parser.parse(io.BytesIO(fragment_8), message_info))
        self.assertIsNone(parser.parse(io.BytesIO(fragment_9), message_info))
        self.assertIsNone(parser.parse(io.BytesIO(fragment_10), message_info))
        self.assertIsNone(parser.parse(io.BytesIO(fragment_11), message_info))
        self.assertIsNone(parser.parse(io.BytesIO(fragment_12), message_info))
        actual_ipv6_packet = parser.parse(io.BytesIO(fragment_1), message_info)

        # THEN
        ipv6_packet = bytearray([
            0x60,
            0x00,
            0x00,
            0x00,
            0x04,
            0xD8,
            0x3A,
            0x40,
            0xfe,
            0x80,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x02,
            0x00,
            0x00,
            0x11,
            0x12,
            0x13,
            0x14,
            0x15,
            0xfe,
            0x80,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x02,
            0x00,
            0x1A,
            0x2A,
            0x3F,
            0x09,
            0xAB,
            0x43,  # / * 40 * /
            0x80,
            0x00,
            0xAB,
            0x64,
            0x0B,
            0xC0,
            0x00,
            0x04,
            0x4E,
            0x92,
            0xBB,
            0x53,
            0x11,
            0x44,
            0x66,
            0x4E,
            0x92,
            0xBB,
            0x53,
            0x1A,
            0x44,
            0x66,
            0x77,
            0x99,
            0x15,
            0xB3,
            0x00,
            0x54,
            0xCC,
            0x54,
            0x01,
            0xAA,
            0x44,
            0x54,
            0x12,
            0x43,
            0x53,
            0x11,
            0x44,
            0x66,
            0x4E,
            0x92,
            0xBB,
            0x53,
            0x1A,
            0x44,
            0x66,
            0x77,
            0x99,
            0x1A,
            0x92,
            0xBB,
            0x53,
            0x11,
            0x44,
            0x66,
            0x92,
            0xBB,
            0x53,
            0x1A,
            0x44,
            0x66,
            0x77,
            0x99,
            0x15,
            0xB3,
            0x00,
            0x54,
            0xCC,
            0x54,
            0x01,
            0xAA,
            0x11,
            0x44,
            0x66,
            0x4E,
            0x92,
            0xBB,
            0x53,
            0x1A,  # / * 120 * /
            0x80,
            0x00,
            0xFA,
            0xA5,
            0x0B,
            0xC0,
            0x00,
            0x04,
            0x4E,
            0x92,
            0xBB,
            0x53,
            0x11,
            0x4C,
            0x66,
            0x4E,
            0x92,
            0xBB,
            0x53,
            0x1A,
            0x44,
            0x66,
            0x77,
            0x99,
            0x15,
            0xB3,
            0x00,
            0x54,
            0xCC,
            0x54,
            0x01,
            0xAA,
            0x44,
            0x54,
            0x12,
            0xa3,
            0x53,
            0x11,
            0x44,
            0x66,
            0xFE,
            0x92,
            0xBB,
            0x53,
            0x1A,
            0x44,
            0x66,
            0x77,
            0x99,
            0x1A,
            0x92,
            0xBB,
            0x53,
            0x11,
            0x44,
            0x66,
            0x92,
            0xBB,
            0x53,
            0x1A,
            0x44,
            0x66,
            0x77,
            0x99,
            0x15,
            0xB3,
            0x00,
            0x54,
            0xCC,
            0x54,
            0x01,
            0xAA,
            0x11,
            0x44,
            0x66,
            0x4E,
            0x92,
            0xBB,
            0x53,
            0x1A,  # / * 200 * /
            0x80,
            0x00,
            0xFA,
            0xA5,
            0x0B,
            0xC0,
            0x00,
            0x04,
            0x4E,
            0x92,
            0x1B,
            0x53,
            0x11,
            0x44,
            0x66,
            0x4E,
            0x22,
            0xBB,
            0x53,
            0x1A,
            0x44,
            0x66,
            0x77,
            0x99,
            0x15,
            0xB3,
            0x00,
            0x54,
            0xCC,
            0x54,
            0x01,
            0xAA,
            0x44,
            0x54,
            0x12,
            0xD3,
            0x53,
            0x11,
            0x44,
            0x66,
            0x4E,
            0x92,
            0xBB,
            0x53,
            0x1A,
            0x44,
            0x66,
            0x77,
            0x99,
            0x1A,
            0x92,
            0xBB,
            0x53,
            0x11,
            0x44,
            0x66,
            0x92,
            0xBB,
            0x53,
            0x1A,
            0x44,
            0x66,
            0x77,
            0x99,
            0x15,
            0xB3,
            0x00,
            0x54,
            0xCC,
            0x54,
            0x01,
            0xAA,
            0x11,
            0x44,
            0x66,
            0x4E,
            0x92,
            0xBB,
            0x53,
            0x1A,  # / * 280 * /
            0xC0,
            0x00,
            0xFA,
            0xA5,
            0x0B,
            0xC0,
            0x00,
            0x04,
            0x4E,
            0x92,
            0xBB,
            0x53,
            0x11,
            0x44,
            0xCC,
            0x4E,
            0x92,
            0xBB,
            0x53,
            0x1A,
            0x44,
            0x66,
            0x77,
            0x99,
            0x15,
            0xB3,
            0x00,
            0x54,
            0xCC,
            0x54,
            0x01,
            0xAA,
            0x44,
            0x54,
            0x12,
            0x43,
            0x53,
            0x11,
            0x44,
            0x66,
            0x4E,
            0x92,
            0xBC,
            0x53,
            0x1A,
            0x44,
            0x66,
            0x77,
            0x99,
            0x1A,
            0x92,
            0xBB,
            0x53,
            0x11,
            0x44,
            0x66,
            0x92,
            0xBB,
            0x53,
            0x1A,
            0x44,
            0x66,
            0x77,
            0x99,
            0x15,
            0xB3,
            0x00,
            0x54,
            0xCC,
            0x54,
            0x01,
            0xAA,
            0x11,
            0x44,
            0x66,
            0x4E,
            0x92,
            0xBB,
            0x53,
            0x1A,  # / * 360 * /
            0x80,
            0x00,
            0xFA,
            0xA5,
            0x0B,
            0xC0,
            0x00,
            0x04,
            0x4E,
            0x92,
            0xBB,
            0x53,
            0x11,
            0x44,
            0x66,
            0x4E,
            0x92,
            0xBB,
            0x53,
            0x1A,
            0x44,
            0x66,
            0x77,
            0x99,
            0x15,
            0xB3,
            0x00,
            0x54,
            0xCC,
            0x54,
            0x01,
            0xAA,
            0x44,
            0x54,
            0x12,
            0x43,
            0x53,
            0x11,
            0x44,
            0x66,
            0x4E,
            0x92,
            0xBB,
            0x53,
            0x1A,
            0x44,
            0x66,
            0x77,
            0x99,
            0x1A,
            0x92,
            0xBB,
            0x53,
            0x11,
            0x44,
            0x66,
            0x92,
            0xBB,
            0x53,
            0x1A,
            0x44,
            0x66,
            0x77,
            0x99,
            0x15,
            0xB3,
            0x00,
            0x54,
            0xCC,
            0x54,
            0x01,
            0xAA,
            0x11,
            0x44,
            0x66,
            0x4E,
            0x92,
            0xBB,
            0x53,
            0x1A,
            0x80,
            0x00,
            0xFA,
            0xA5,
            0x0B,
            0xC0,
            0x00,
            0x04,
            0x4E,
            0x92,
            0xBB,
            0x53,
            0x11,
            0x4C,
            0x66,
            0x4E,
            0x92,
            0xBB,
            0x53,
            0x1A,
            0x44,
            0x66,
            0x77,
            0x99,
            0x15,
            0xB3,
            0x00,
            0x54,
            0xCC,
            0x54,
            0x01,
            0xAA,
            0x44,
            0x54,
            0x12,
            0xa3,
            0x53,
            0x11,
            0x44,
            0x66,
            0xFE,
            0x92,
            0xBB,
            0x53,
            0x1A,
            0x44,
            0x66,
            0x77,
            0x99,
            0x1A,
            0x92,
            0xBB,
            0x53,
            0x11,
            0x44,
            0x66,
            0x92,
            0xBB,
            0x53,
            0x1A,
            0x44,
            0x66,
            0x77,
            0x99,
            0x15,
            0xB3,
            0x00,
            0x54,
            0xCC,
            0x54,
            0x01,
            0xAA,
            0x11,
            0x44,
            0x66,
            0x4E,
            0x92,
            0xBB,
            0x53,
            0x1A,
            0x80,
            0x00,
            0xFA,
            0xA5,
            0x0B,
            0xC0,
            0x00,
            0x04,
            0x4E,
            0x92,
            0x1B,
            0x53,
            0x11,
            0x44,
            0x66,
            0x4E,
            0x22,
            0xBB,
            0x53,
            0x1A,
            0x44,
            0x67,
            0x77,
            0x99,
            0x15,
            0xB3,
            0x00,
            0x54,
            0xCC,
            0x54,
            0x01,
            0xAA,
            0x44,
            0x54,
            0x12,
            0xD3,
            0x53,
            0x11,
            0x44,
            0x66,
            0x4E,
            0x92,
            0xBB,
            0x53,
            0x1A,
            0x44,
            0x66,
            0x77,
            0x99,
            0x1A,
            0x92,
            0xBB,
            0x53,
            0x11,
            0x44,
            0x66,
            0x92,
            0xBB,
            0x53,
            0x1A,
            0x44,
            0x66,
            0x77,
            0x99,
            0x15,
            0xB3,
            0x00,
            0x54,
            0xCC,
            0x54,
            0x01,
            0xAA,
            0x11,
            0x44,
            0x66,
            0x4E,
            0x92,
            0xBB,
            0x53,
            0x1A,
            0xC0,
            0x00,
            0xFA,
            0x15,
            0x0B,
            0xC0,
            0x00,
            0x04,
            0x4E,
            0x92,
            0xBB,
            0x53,
            0x11,
            0x44,
            0xCC,
            0x4E,
            0x92,
            0xBB,
            0x53,
            0x1A,
            0x44,
            0x66,
            0x77,
            0x99,
            0x15,
            0xB3,
            0x00,
            0x54,
            0xCC,
            0x54,
            0x01,
            0xAA,
            0x44,
            0x54,
            0x12,
            0x43,
            0x53,
            0x11,
            0x44,
            0x66,
            0x4E,
            0x92,
            0xBC,
            0x53,
            0x1A,
            0x44,
            0x66,
            0x77,
            0x99,
            0x1A,
            0x92,
            0xBB,
            0x53,
            0x11,
            0x44,
            0x66,
            0x92,
            0xBB,
            0x53,
            0x1A,
            0x44,
            0x66,
            0x77,
            0x99,
            0x15,
            0xB3,
            0x00,
            0x54,
            0xCC,
            0x54,
            0x01,
            0xAA,
            0x11,
            0x44,
            0x66,
            0x4E,
            0x92,
            0xBA,
            0x53,
            0x1A,
            0x60,
            0x00,
            0x00,
            0x00,
            0x00,
            0x10,
            0x3A,
            0x64,
            0xfe,
            0x80,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x02,
            0x00,
            0x00,
            0x11,
            0x12,
            0x13,
            0x14,
            0x15,
            0xfe,
            0x80,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x02,
            0x00,
            0x1A,
            0x2A,
            0x3F,
            0x09,
            0xAB,
            0x43,  # / * 720 * /
            0x60,
            0x00,
            0xF0,
            0x00,
            0x00,
            0x10,
            0x3A,
            0x64,
            0xfe,
            0x80,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x02,
            0x00,
            0x00,
            0x11,
            0x12,
            0x13,
            0x14,
            0x15,
            0xfe,
            0x80,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x02,
            0x00,
            0x1A,
            0x2A,
            0x3F,
            0x09,
            0xAB,
            0x43,
            0x80,
            0x00,
            0xFA,
            0xA5,
            0x0B,
            0xC0,
            0x00,
            0x04,
            0x4E,
            0x92,
            0xBB,
            0x53,
            0x11,
            0x44,
            0x66,
            0x4E,
            0x92,
            0xBB,
            0x53,
            0x1A,
            0x44,
            0x66,
            0x77,
            0x99,
            0x15,
            0xB3,
            0x00,
            0x54,
            0xCC,
            0x54,
            0x01,
            0xAA,
            0x44,
            0x54,
            0x12,
            0x43,
            0x53,
            0x11,
            0x44,
            0x66,
            0x4E,
            0x92,
            0xBC,
            0x53,
            0x1A,
            0x44,
            0x66,
            0x77,
            0x99,
            0x1A,
            0x92,
            0xBB,
            0x53,
            0x11,
            0x44,
            0x66,
            0x92,
            0xBB,
            0x53,
            0x1A,
            0x44,
            0x66,
            0x77,
            0x99,
            0x15,
            0xB3,
            0x00,
            0x54,
            0xCC,
            0x54,
            0x01,
            0xAA,
            0x11,
            0x44,
            0x66,
            0x4E,
            0x92,
            0xBB,
            0x53,
            0x1A,
            0x80,
            0x00,
            0xFA,
            0xA5,
            0x0B,
            0xC0,
            0x00,
            0x04,
            0x4E,
            0x92,
            0xBB,
            0x53,
            0x11,
            0x4C,
            0x66,
            0x4E,
            0x92,
            0xBB,
            0x53,
            0x1A,
            0x44,
            0x66,
            0x77,
            0x99,
            0x15,
            0xB3,
            0x00,
            0x54,
            0xCC,
            0x54,
            0x01,
            0xAA,
            0x44,
            0x54,
            0x12,
            0xa3,
            0x53,
            0x11,
            0x44,
            0x66,
            0xFE,
            0x92,
            0xBB,
            0x53,
            0x1A,
            0x44,
            0x66,
            0x77,
            0x99,
            0x1A,
            0x92,
            0xBB,
            0x53,
            0x11,
            0x44,
            0x66,
            0x92,
            0xBB,
            0x53,
            0x1A,
            0x4D,
            0x66,
            0x77,
            0x99,
            0x15,
            0xB3,
            0x00,
            0x54,
            0xCC,
            0x54,
            0x01,
            0xAA,
            0x11,
            0x44,
            0x66,
            0x4E,
            0x92,
            0xBB,
            0x53,
            0x1A,
            0x80,
            0x00,
            0xFA,
            0xA5,
            0x0B,
            0xC0,
            0x00,
            0x04,
            0x4E,
            0x92,
            0x1B,
            0x53,
            0x11,
            0x44,
            0x66,
            0x4E,
            0x22,
            0xBB,
            0x51,
            0x1A,
            0x44,
            0x66,
            0x77,
            0x99,
            0x15,
            0xB3,
            0x00,
            0x54,
            0xCC,
            0x54,
            0x01,
            0xAA,
            0x44,
            0x54,
            0x12,
            0xD3,
            0x53,
            0x11,
            0x44,
            0x66,
            0x4E,
            0x92,
            0xBB,
            0x53,
            0x1A,
            0x44,
            0x66,
            0x77,
            0x99,
            0x1A,
            0x92,
            0xBB,
            0x53,
            0x11,
            0x44,
            0x66,
            0x92,
            0xBB,
            0x53,
            0x1A,
            0x44,
            0x66,
            0x77,
            0x99,
            0x15,
            0xB3,
            0x00,
            0x54,
            0xCC,
            0x54,
            0x01,
            0xAA,
            0x11,
            0x44,
            0x66,
            0x4E,
            0x92,
            0xBB,
            0x53,
            0x1A,
            0xC0,
            0x00,
            0xFA,
            0xA5,
            0x0B,
            0xC0,
            0x00,
            0x04,
            0x4E,
            0x92,
            0xBB,
            0x53,
            0x11,
            0x44,
            0xCC,
            0x4E,
            0x92,
            0xBB,
            0x53,
            0x1A,
            0x44,
            0x66,
            0x77,
            0x99,
            0x15,
            0xB3,
            0x00,
            0x54,
            0xCC,
            0x54,
            0x01,
            0xAA,
            0x44,
            0x54,
            0x12,
            0x4A,
            0x53,
            0x11,
            0x44,
            0x66,
            0x4E,
            0x92,
            0xBC,
            0x53,
            0x1A,
            0x44,
            0x66,
            0x77,
            0x99,
            0x1A,
            0x92,
            0xBB,
            0x53,
            0x11,
            0x44,
            0x66,
            0x92,
            0xBB,
            0x53,
            0x1A,
            0x44,
            0x66,
            0x77,
            0x99,
            0x15,
            0xB3,
            0x00,
            0x54,
            0xCC,
            0x54,
            0x01,
            0xAA,
            0x11,
            0x44,
            0x66,
            0x4E,
            0x92,
            0xBB,
            0x53,
            0x1A,  # / * 1080 * /
            0x80,
            0x00,
            0xFA,
            0xA5,
            0x0B,
            0xC0,
            0x00,
            0x04,
            0x4E,
            0x92,
            0xBB,
            0x53,
            0x11,
            0x44,
            0x66,
            0x4E,
            0x92,
            0xBB,
            0x53,
            0x1A,
            0x44,
            0x66,
            0x77,
            0x99,
            0x15,
            0xB3,
            0x00,
            0x54,
            0xCC,
            0x54,
            0x01,
            0xAA,
            0x44,
            0x54,
            0x12,
            0x43,
            0x53,
            0x11,
            0x44,
            0x66,
            0x4E,
            0x92,
            0xBB,
            0x53,
            0x3A,
            0x44,
            0x66,
            0x77,
            0x99,
            0x1A,
            0x92,
            0xBB,
            0x53,
            0x11,
            0x44,
            0x66,
            0x92,
            0xBB,
            0x53,
            0x1A,
            0x44,
            0x66,
            0x77,
            0x99,
            0x15,
            0xB3,
            0x00,
            0x54,
            0xCC,
            0x54,
            0x01,
            0xAA,
            0x11,
            0x44,
            0x66,
            0x4E,
            0x92,
            0xBB,
            0x53,
            0x1A,
            0x80,
            0x00,
            0xFA,
            0xA5,
            0x0B,
            0xC0,
            0x00,
            0x04,
            0x4E,
            0x92,
            0xBB,
            0x53,
            0x11,
            0x4C,
            0x66,
            0x4E,
            0x92,
            0xBB,
            0x53,
            0x1A,
            0x44,
            0x66,
            0x77,
            0x99,
            0x15,
            0xB3,
            0x00,
            0x54,
            0xCC,
            0x54,
            0x01,
            0xAA,
            0x44,
            0x54,
            0x12,
            0xa3,
            0x53,
            0x11,
            0x44,
            0x66,
            0xFE,
            0x92,
            0xBB,
            0x53,
            0x1A,
            0x44,
            0x66,
            0x77,
            0x99,
            0x1A,
            0x92,
            0xBB,
            0x53,
            0x11,
            0x44,
            0x66,
            0x92,
            0xBB,
            0x53,
            0x1A,
            0x44,
            0x66,
            0x77,
            0x99,
            0x15,
            0xB3,
            0x00,
            0x54,
            0xCC,
            0x54,
            0x01,
            0xAA,
            0x11,
            0x44,
            0x66,
            0x4E,
            0x92,
            0xBB,
            0x53,
            0x1A,
            0x80,
            0x00,
            0xFA,
            0xA5,
            0x1B,
            0xC0,
            0x00,
            0x04,
            0x4E,
            0x92,
            0x1B,
            0x53,
            0x11,
            0x44,
            0x66,
            0x4E,
            0x22,
            0xBB,
            0x53,
            0x1A,
            0x44,
            0x66,
            0x77,
            0x99,
            0x15,
            0xB3,
            0x00,
            0x54,
            0xCC,
            0x54,
            0x01,
            0xAA,
            0x44,
            0x54,
            0x12,
            0xD3,
            0x53,
            0x11,
            0x44,
            0x66
        ])
        self.assertEqual(ipv6_packet, actual_ipv6_packet.to_bytes())

    def test_should_defragment_IPv6_packet_when_parse_method_called_with_fragments(self):
        # GIVEN
        message_info = common.MessageInfo()
        message_info.source_mac_address = common.MacAddress.from_eui64(
            bytearray([0x00, 0x00, 0x00, 0x11, 0x12, 0x13, 0x14, 0x15]))
        message_info.destination_mac_address = common.MacAddress.from_eui64(
            bytearray([0x00, 0x00, 0x1A, 0x2A, 0x3F, 0x09, 0xAB, 0x43]))

        fragment_1 = bytearray(
            [0xC0, 0x38, 0x12, 0x34, 0x7A, 0x33, 0x3A, 0x80, 0x00, 0x1A, 0x33, 0x0B, 0xC0, 0x00, 0x04])

        fragment_2 = bytearray([0xE0, 0x38, 0x12, 0x34, 0x06, 0x4E, 0x92, 0xBB, 0x53, 0x11, 0x12, 0x13, 0x14])

        parser = create_default_lowpan_parser(None)

        # WHEN
        self.assertIsNone(parser.parse(io.BytesIO(fragment_1), message_info=message_info))
        actual_ipv6_packet = parser.parse(io.BytesIO(fragment_2), message_info=message_info)

        # THEN
        ipv6_packet = bytearray([
            0x60, 0x00, 0x00, 0x00, 0x00, 0x10, 0x3A, 0x40, 0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00,
            0x00, 0x11, 0x12, 0x13, 0x14, 0x15, 0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x1A, 0x2A,
            0x3F, 0x09, 0xAB, 0x43, 0x80, 0x00, 0x1A, 0x33, 0x0B, 0xC0, 0x00, 0x04, 0x4E, 0x92, 0xBB, 0x53, 0x11, 0x12,
            0x13, 0x14
        ])
        self.assertEqual(ipv6_packet, actual_ipv6_packet.to_bytes())


class TestLowpanUdpHeaderFactory(unittest.TestCase):

    def test_should_parse_udp_datagram_ports_when_decompress_udp_ports_method_called_with_udphc_p_eq_0(self):
        # GIVEN
        factory = lowpan.LowpanUdpHeaderFactory()

        p = factory.UDP_HC_P_BOTH_FULL

        udphc = lowpan.LowpanUDPHC(any_c(), p)

        src_port = any_src_port()
        dst_port = any_dst_port()

        data_bytes = struct.pack(">H", src_port) + struct.pack(">H", dst_port)

        # WHEN
        actual_src_port, actual_dst_port = factory._decompress_udp_ports(udphc, io.BytesIO(data_bytes))

        # THEN
        self.assertEqual(src_port, actual_src_port)
        self.assertEqual(dst_port, actual_dst_port)
        self.assertEqual(0, p)

    def test_should_parse_udp_datagram_ports_when_decompress_udp_ports_method_called_with_udphc_p_eq_1(self):
        # GIVEN
        factory = lowpan.LowpanUdpHeaderFactory()

        p = factory.UDP_HC_P_DST_COMPR

        udphc = lowpan.LowpanUDPHC(any_c(), p)

        src_port = any_src_port()
        dst_port = any_compressable_dst_port()

        data_bytes = struct.pack(">H", src_port) + bytearray([struct.pack(">H", dst_port)[1]])

        # WHEN
        actual_src_port, actual_dst_port = factory._decompress_udp_ports(udphc, io.BytesIO(data_bytes))

        # THEN
        self.assertEqual(1, p)
        self.assertEqual(src_port, actual_src_port)
        self.assertEqual(dst_port, actual_dst_port)

    def test_should_parse_udp_datagram_ports_when_decompress_udp_ports_method_called_with_udphc_p_eq_2(self):
        # GIVEN
        factory = lowpan.LowpanUdpHeaderFactory()

        p = factory.UDP_HC_P_SRC_COMPR

        udphc = lowpan.LowpanUDPHC(any_c(), p)

        src_port = any_compressable_src_port()
        dst_port = any_dst_port()

        data_bytes = bytearray([struct.pack(">H", src_port)[1]]) + struct.pack(">H", dst_port)

        # WHEN
        actual_src_port, actual_dst_port = factory._decompress_udp_ports(udphc, io.BytesIO(data_bytes))

        # THEN
        self.assertEqual(2, p)
        self.assertEqual(src_port, actual_src_port)
        self.assertEqual(dst_port, actual_dst_port)

    def test_should_parse_udp_datagram_ports_when_decompress_udp_ports_method_called_with_udphc_p_eq_3(self):
        # GIVEN
        factory = lowpan.LowpanUdpHeaderFactory()

        p = factory.UDP_HC_P_BOTH_COMPR

        udphc = lowpan.LowpanUDPHC(any_c(), p)

        src_port = any_nibble_src_port()
        dst_port = any_nibble_dst_port()

        data_bytes = bytearray([((src_port & 0x0F) << 4) | (dst_port & 0x0F)])

        # WHEN
        actual_src_port, actual_dst_port = factory._decompress_udp_ports(udphc, io.BytesIO(data_bytes))

        # THEN
        self.assertEqual(3, p)
        self.assertEqual(src_port, actual_src_port)
        self.assertEqual(dst_port, actual_dst_port)

    def test_should_parse_udp_datagram_checksum_when_decompress_udp_checksum_called_with_udphc_c_eq_0(self):
        # GIVEN
        factory = lowpan.LowpanUdpHeaderFactory()

        c = factory.UDP_HC_C_INLINE

        udphc = lowpan.LowpanUDPHC(c, any_p())

        checksum = any_checksum()

        data_bytes = struct.pack(">H", checksum)

        # WHEN
        actual_checksum = factory._decompress_udp_checksum(udphc, io.BytesIO(data_bytes))

        # THEN
        self.assertEqual(0, c)
        self.assertEqual(checksum, actual_checksum)

    def test_should_parse_udp_datagram_checksum_when_decompress_udp_checksum_called_with_udphc_c_eq_1(self):
        # GIVEN
        factory = lowpan.LowpanUdpHeaderFactory()

        c = factory.UDP_HC_C_ELIDED

        udphc = lowpan.LowpanUDPHC(c, any_p())

        data_bytes = bytearray()

        # WHEN
        actual_checksum = factory._decompress_udp_checksum(udphc, io.BytesIO(data_bytes))

        # THEN
        self.assertEqual(1, c)
        self.assertEqual(0, actual_checksum)


class TestLowpanIpv6HeaderFactory(unittest.TestCase):

    IPV6_LINKLOCAL_PREFIX = bytearray([0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00])

    def test_should_parse_traffic_class_and_flow_label_when_decompress_tf_method_called_with_iphc_tf_eq_0(self):
        # GIVEN
        ecn = any_ecn()
        dscp = any_dscp()
        flow_label = any_flow_label()

        data_bytes = bytearray()
        data_bytes.append((ecn << 6) | dscp)
        data_bytes.append((flow_label >> 16) & 0x0F)
        data_bytes.append((flow_label >> 8) & 0xFF)
        data_bytes.append(flow_label & 0xFF)

        factory = lowpan.LowpanIpv6HeaderFactory()

        tf = factory.IPHC_TF_4B
        iphc = lowpan.LowpanIPHC(tf, any_nh(), any_hlim(), any_cid(), any_sac(), any_sam(), any_m(), any_dac(),
                                 any_dam())

        # WHEN
        actual_traffic_class, actual_flow_label = factory._decompress_tf(iphc, io.BytesIO(data_bytes))

        # THEN
        self.assertEqual(0, tf)
        self.assertEqual((dscp << 2) | ecn, actual_traffic_class)
        self.assertEqual(flow_label, actual_flow_label)

    def test_should_parse_traffic_class_and_flow_label_when_decompress_tf_method_called_with_iphc_tf_eq_1(self):
        # GIVEN
        ecn = any_ecn()
        flow_label = any_flow_label()

        data_bytes = bytearray()
        data_bytes.append((ecn << 6) | (flow_label >> 16) & 0x0F)
        data_bytes.append((flow_label >> 8) & 0xFF)
        data_bytes.append(flow_label & 0xFF)

        factory = lowpan.LowpanIpv6HeaderFactory()

        tf = factory.IPHC_TF_3B
        iphc = lowpan.LowpanIPHC(tf, any_nh(), any_hlim(), any_cid(), any_sac(), any_sam(), any_m(), any_dac(),
                                 any_dam())

        # WHEN
        actual_traffic_class, actual_flow_label = factory._decompress_tf(iphc, io.BytesIO(data_bytes))

        # THEN
        self.assertEqual(1, tf)
        self.assertEqual(ecn, actual_traffic_class)
        self.assertEqual(flow_label, actual_flow_label)

    def test_should_parse_traffic_class_and_flow_label_when_decompress_tf_method_called_with_iphc_tf_eq_2(self):
        # GIVEN
        ecn = any_ecn()
        dscp = any_dscp()

        data_bytes = bytearray([(ecn << 6) | dscp])

        factory = lowpan.LowpanIpv6HeaderFactory()

        tf = factory.IPHC_TF_1B
        iphc = lowpan.LowpanIPHC(tf, any_nh(), any_hlim(), any_cid(), any_sac(), any_sam(), any_m(), any_dac(),
                                 any_dam())

        # WHEN
        actual_traffic_class, actual_flow_label = factory._decompress_tf(iphc, io.BytesIO(data_bytes))

        # THEN
        self.assertEqual(2, tf)
        self.assertEqual((dscp << 2) | ecn, actual_traffic_class)
        self.assertEqual(0, actual_flow_label)

    def test_should_parse_traffic_class_and_flow_label_when_decompress_tf_method_called_with_iphc_tf_eq_3(self):
        data_bytes = bytearray()

        factory = lowpan.LowpanIpv6HeaderFactory()

        tf = factory.IPHC_TF_ELIDED
        iphc = lowpan.LowpanIPHC(tf, any_nh(), any_hlim(), any_cid(), any_sac(), any_sam(), any_m(), any_dac(),
                                 any_dam())

        # WHEN
        actual_traffic_class, actual_flow_label = factory._decompress_tf(iphc, io.BytesIO(data_bytes))

        # THEN
        self.assertEqual(3, tf)
        self.assertEqual(0, actual_traffic_class)
        self.assertEqual(0, actual_flow_label)

    def test_should_parse_traffic_class_and_flow_label_when_decompress_nh_method_called_with_iphc_nh_eq_0(self):
        # GIVEN
        factory = lowpan.LowpanIpv6HeaderFactory()

        next_header = any_next_header()

        nh = factory.IPHC_NH_INLINE
        iphc = lowpan.LowpanIPHC(any_tf(), nh, any_hlim(), any_cid(), any_sac(), any_sam(), any_m(), any_dac(),
                                 any_dam())

        data_bytes = bytearray([next_header])

        # WHEN
        actual_next_header = factory._decompress_nh(iphc, io.BytesIO(data_bytes))

        # THEN
        self.assertEqual(0, nh)
        self.assertEqual(next_header, actual_next_header)

    def test_should_parse_traffic_class_and_flow_label_when_decompress_nh_method_called_with_iphc_nh_eq_1(self):
        # GIVEN
        factory = lowpan.LowpanIpv6HeaderFactory()

        nh = factory.IPHC_NH_COMPRESSED
        iphc = lowpan.LowpanIPHC(any_tf(), nh, any_hlim(), any_cid(), any_sac(), any_sam(), any_m(), any_dac(),
                                 any_dam())

        data_bytes = bytearray()

        # WHEN
        actual_next_header = factory._decompress_nh(iphc, io.BytesIO(data_bytes))

        # THEN
        self.assertEqual(1, nh)
        self.assertEqual(None, actual_next_header)

    def test_should_parse_hop_limit_when_decompress_hlim_called_with_iphc_hlim_eq_0(self):
        # GIVEN
        hop_limit = any_hop_limit()

        factory = lowpan.LowpanIpv6HeaderFactory()

        hlim = factory.IPHC_HLIM_INLINE
        iphc = lowpan.LowpanIPHC(any_tf(), any_nh(), hlim, any_cid(), any_sac(), any_sam(), any_m(), any_dac(),
                                 any_dam())

        data_bytes = bytearray([hop_limit])

        # WHEN
        actual_hop_limit = factory._decompress_hlim(iphc, io.BytesIO(data_bytes))

        # THEN
        self.assertEqual(0, hlim)
        self.assertEqual(hop_limit, actual_hop_limit)

    def test_should_parse_hop_limit_when_decompress_hlim_called_with_iphc_hlim_eq_1(self):
        # GIVEN
        factory = lowpan.LowpanIpv6HeaderFactory()

        hlim = factory.IPHC_HLIM_1
        iphc = lowpan.LowpanIPHC(any_tf(), any_nh(), hlim, any_cid(), any_sac(), any_sam(), any_m(), any_dac(),
                                 any_dam())

        data_bytes = bytearray()

        # WHEN
        actual_hop_limit = factory._decompress_hlim(iphc, io.BytesIO(data_bytes))

        # THEN
        self.assertEqual(1, hlim)
        self.assertEqual(1, actual_hop_limit)

    def test_should_parse_hop_limit_when_decompress_hlim_called_with_iphc_hlim_eq_2(self):
        # GIVEN
        factory = lowpan.LowpanIpv6HeaderFactory()

        hlim = factory.IPHC_HLIM_64
        iphc = lowpan.LowpanIPHC(any_tf(), any_nh(), hlim, any_cid(), any_sac(), any_sam(), any_m(), any_dac(),
                                 any_dam())

        data_bytes = bytearray()

        # WHEN
        actual_hop_limit = factory._decompress_hlim(iphc, io.BytesIO(data_bytes))

        # THEN
        self.assertEqual(2, hlim)
        self.assertEqual(64, actual_hop_limit)

    def test_should_parse_hop_limit_when_decompress_hlim_called_with_iphc_hlim_eq_3(self):
        # GIVEN
        factory = lowpan.LowpanIpv6HeaderFactory()

        hlim = factory.IPHC_HLIM_255
        iphc = lowpan.LowpanIPHC(any_tf(), any_nh(), hlim, any_cid(), any_sac(), any_sam(), any_m(), any_dac(),
                                 any_dam())

        data_bytes = bytearray()

        # WHEN
        actual_hop_limit = factory._decompress_hlim(iphc, io.BytesIO(data_bytes))

        # THEN
        self.assertEqual(3, hlim)
        self.assertEqual(255, actual_hop_limit)

    def test_should_parse_source_address_when_decompress_src_addr_called_with_sac_eq_0_and_sam_eq_0(self):
        # GIVEN
        factory = lowpan.LowpanIpv6HeaderFactory()

        src_addr = any_src_addr()

        sac = factory.IPHC_SAC_STATELESS
        sam = factory.IPHC_SAM_128B

        iphc = lowpan.LowpanIPHC(any_tf(), any_nh(), any_hlim(), any_cid(), sac, sam, any_m(), any_dac(), any_dam())

        # WHEN
        actual_src_addr = factory._decompress_src_addr(iphc, any_src_mac_addr(), any_sci(), io.BytesIO(src_addr))

        # THEN
        self.assertEqual(0, sac)
        self.assertEqual(0, sam)
        self.assertEqual(bytes(src_addr), actual_src_addr)

    def test_should_parse_source_address_when_decompress_src_addr_called_with_sac_eq_0_and_sam_eq_1(self):
        # GIVEN
        factory = lowpan.LowpanIpv6HeaderFactory()

        eui64 = any_eui64()

        sac = factory.IPHC_SAC_STATELESS
        sam = factory.IPHC_SAM_64B

        iphc = lowpan.LowpanIPHC(any_tf(), any_nh(), any_hlim(), any_cid(), sac, sam, any_m(), any_dac(), any_dam())

        # WHEN
        actual_src_addr = factory._decompress_src_addr(iphc, any_src_mac_addr(), any_sci(), io.BytesIO(eui64))

        # THEN
        self.assertEqual(0, sac)
        self.assertEqual(1, sam)
        self.assertEqual(self.IPV6_LINKLOCAL_PREFIX + eui64, actual_src_addr)

    def test_should_parse_source_address_when_decompress_src_addr_called_with_sac_eq_0_and_sam_eq_2(self):
        # GIVEN
        factory = lowpan.LowpanIpv6HeaderFactory()

        rloc16 = any_rloc16()

        sac = factory.IPHC_SAC_STATELESS
        sam = factory.IPHC_SAM_16B

        iphc = lowpan.LowpanIPHC(any_tf(), any_nh(), any_hlim(), any_cid(), sac, sam, any_m(), any_dac(), any_dam())

        # WHEN
        actual_src_addr = factory._decompress_src_addr(iphc, any_src_mac_addr(), any_sci(), io.BytesIO(rloc16))

        # THEN
        self.assertEqual(0, sac)
        self.assertEqual(2, sam)
        self.assertEqual(self.IPV6_LINKLOCAL_PREFIX + bytearray([0x00, 0x00, 0x00, 0xff, 0xfe, 0x00]) + rloc16,
                         actual_src_addr)

    def test_should_parse_source_address_when_decompress_src_addr_called_with_sac_eq_0_and_sam_eq_3(self):
        # GIVEN
        factory = lowpan.LowpanIpv6HeaderFactory()

        src_mac_addr = common.MacAddress.from_eui64(any_src_mac_addr())

        sac = factory.IPHC_SAC_STATELESS
        sam = factory.IPHC_SAM_ELIDED

        iphc = lowpan.LowpanIPHC(any_tf(), any_nh(), any_hlim(), any_cid(), sac, sam, any_m(), any_dac(), any_dam())

        data_bytes = bytearray([])

        # WHEN
        actual_src_addr = factory._decompress_src_addr(iphc, src_mac_addr, any_sci(), io.BytesIO(data_bytes))

        # THEN
        self.assertEqual(0, sac)
        self.assertEqual(3, sam)
        self.assertEqual(
            self.IPV6_LINKLOCAL_PREFIX + bytearray([src_mac_addr.mac_address[0] ^ 0x02]) +
            src_mac_addr.mac_address[1:], actual_src_addr)

    def _merge_prefix_and_address(self, prefix, prefix_length, address):
        total_bytes = 16

        prefix_length_in_bytes = int(prefix_length / 8)

        if (prefix_length_in_bytes + len(address)) > total_bytes:
            total_bytes -= prefix_length_in_bytes

            return prefix[:prefix_length_in_bytes] + address[-total_bytes:]

        else:
            total_bytes -= prefix_length_in_bytes
            total_bytes -= len(address)

            return prefix[:prefix_length_in_bytes] + bytearray([0x00] * total_bytes) + address

    def test_should_parse_source_address_when_decompress_src_addr_called_with_sac_eq_1_and_sam_eq_0(self):
        # GIVEN
        factory = lowpan.LowpanIpv6HeaderFactory(None)

        src_addr = any_src_addr()

        sac = factory.IPHC_SAC_STATEFUL
        sam = factory.IPHC_SAM_UNSPECIFIED

        iphc = lowpan.LowpanIPHC(any_tf(), any_nh(), any_hlim(), any_cid(), sac, sam, any_m(), any_dac(), any_dam())

        # WHEN
        actual_src_addr = factory._decompress_src_addr(iphc, any_src_mac_addr(), any_sci(), io.BytesIO(src_addr))

        # THEN
        self.assertEqual(1, sac)
        self.assertEqual(0, sam)
        self.assertEqual(bytearray([0x00] * 16), actual_src_addr)

    def test_should_parse_source_address_when_decompress_src_addr_called_with_sac_eq_1_and_sam_eq_1(self):
        # GIVEN
        sci = any_sci()

        context = any_context()

        context_manager = lowpan.ContextManager()
        context_manager[sci] = context

        factory = lowpan.LowpanIpv6HeaderFactory(context_manager)

        eui64 = any_eui64()

        sac = factory.IPHC_SAC_STATEFUL
        sam = factory.IPHC_SAM_64B

        iphc = lowpan.LowpanIPHC(any_tf(), any_nh(), any_hlim(), any_cid(), sac, sam, any_m(), any_dac(), any_dam())

        src_addr = self._merge_prefix_and_address(context.prefix, context.prefix_length, eui64)

        # WHEN
        actual_src_addr = factory._decompress_src_addr(iphc, any_src_mac_addr(), sci, io.BytesIO(eui64))

        # THEN
        self.assertEqual(1, sac)
        self.assertEqual(1, sam)
        self.assertEqual(src_addr, actual_src_addr)

    def test_should_parse_source_address_when_decompress_src_addr_called_with_sac_eq_1_and_sam_eq_2(self):
        # GIVEN
        sci = any_sci()

        context = any_context()

        context_manager = lowpan.ContextManager()
        context_manager[sci] = context

        factory = lowpan.LowpanIpv6HeaderFactory(context_manager)

        rloc16 = any_rloc16()

        sac = factory.IPHC_SAC_STATEFUL
        sam = factory.IPHC_SAM_16B

        iphc = lowpan.LowpanIPHC(any_tf(), any_nh(), any_hlim(), any_cid(), sac, sam, any_m(), any_dac(), any_dam())

        iid = bytearray([0x00, 0x00, 0x00, 0xff, 0xfe, 0x00]) + rloc16

        src_addr = self._merge_prefix_and_address(context.prefix, context.prefix_length, iid)

        # WHEN
        actual_src_addr = factory._decompress_src_addr(iphc, any_src_mac_addr(), sci, io.BytesIO(rloc16))

        # THEN
        self.assertEqual(1, sac)
        self.assertEqual(2, sam)
        self.assertEqual(src_addr, actual_src_addr)

    def test_should_parse_source_address_when_decompress_src_addr_called_with_sac_eq_1_and_sam_eq_3(self):
        # GIVEN
        sci = any_sci()

        context = any_context()

        context_manager = lowpan.ContextManager()
        context_manager[sci] = context

        factory = lowpan.LowpanIpv6HeaderFactory(context_manager)

        src_mac_addr = common.MacAddress.from_eui64(any_src_mac_addr())

        sac = factory.IPHC_SAC_STATEFUL
        sam = factory.IPHC_SAM_0B

        iphc = lowpan.LowpanIPHC(any_tf(), any_nh(), any_hlim(), any_cid(), sac, sam, any_m(), any_dac(), any_dam())

        iid = bytearray([src_mac_addr.mac_address[0] ^ 0x02]) + src_mac_addr.mac_address[1:]

        src_addr = self._merge_prefix_and_address(context.prefix, context.prefix_length, iid)

        data_bytes = bytearray([])

        # WHEN
        actual_src_addr = factory._decompress_src_addr(iphc, src_mac_addr, sci, io.BytesIO(data_bytes))

        # THEN
        self.assertEqual(1, sac)
        self.assertEqual(3, sam)
        self.assertEqual(src_addr, actual_src_addr)

    def test_should_parse_dst_addr_when_decompress_dst_addr_called_with_m_eq_0_and_dac_eq_0_and_dam_eq_0(self):
        # GIVEN
        factory = lowpan.LowpanIpv6HeaderFactory()

        ipv6_addr = any_dst_addr()

        m = factory.IPHC_M_NO
        dac = factory.IPHC_DAC_STATELESS
        dam = factory.IPHC_DAM_128B

        iphc = lowpan.LowpanIPHC(any_tf(), any_nh(), any_hlim(), any_cid(), any_sac(), any_sam(), m, dac, dam)

        dst_mac_addr = bytearray([0x00] * 8)

        # WHEN
        actual_dst_addr = factory._decompress_dst_addr(iphc, dst_mac_addr, any_dci(), io.BytesIO(ipv6_addr))

        # THEN
        self.assertEqual(0, m)
        self.assertEqual(0, dac)
        self.assertEqual(0, dam)
        self.assertEqual(ipv6_addr, actual_dst_addr)

    def test_should_parse_dst_addr_when_decompress_dst_addr_called_with_m_eq_0_and_dac_eq_0_and_dam_eq_1(self):
        # GIVEN
        factory = lowpan.LowpanIpv6HeaderFactory()

        eui64 = any_eui64()

        m = factory.IPHC_M_NO
        dac = factory.IPHC_DAC_STATELESS
        dam = factory.IPHC_DAM_64B

        iphc = lowpan.LowpanIPHC(any_tf(), any_nh(), any_hlim(), any_cid(), any_sac(), any_sam(), m, dac, dam)

        # WHEN
        actual_dst_addr = factory._decompress_dst_addr(iphc, any_dst_mac_addr(), any_dci(), io.BytesIO(eui64))

        # THEN
        self.assertEqual(0, m)
        self.assertEqual(0, dac)
        self.assertEqual(1, dam)
        self.assertEqual(self.IPV6_LINKLOCAL_PREFIX + eui64, actual_dst_addr)

    def test_should_parse_dst_addr_when_decompress_dst_addr_called_with_m_eq_0_and_dac_eq_0_and_dam_eq_2(self):
        # GIVEN
        factory = lowpan.LowpanIpv6HeaderFactory()

        rloc16 = any_rloc16()

        m = factory.IPHC_M_NO
        dac = factory.IPHC_DAC_STATELESS
        dam = factory.IPHC_DAM_16B

        iphc = lowpan.LowpanIPHC(any_tf(), any_nh(), any_hlim(), any_cid(), any_sac(), any_sam(), m, dac, dam)

        # WHEN
        actual_dst_addr = factory._decompress_dst_addr(iphc, any_dst_mac_addr(), any_dci(), io.BytesIO(rloc16))

        # THEN
        self.assertEqual(0, m)
        self.assertEqual(0, dac)
        self.assertEqual(2, dam)
        self.assertEqual(self.IPV6_LINKLOCAL_PREFIX + bytearray([0x00, 0x00, 0x00, 0xff, 0xfe, 0x00]) + rloc16,
                         actual_dst_addr)

    def test_should_parse_dst_addr_when_decompress_dst_addr_called_with_m_eq_0_and_dac_eq_0_and_dam_eq_3(self):
        # GIVEN
        factory = lowpan.LowpanIpv6HeaderFactory()

        dst_mac_addr = common.MacAddress.from_eui64(any_dst_mac_addr())

        m = factory.IPHC_M_NO
        dac = factory.IPHC_DAC_STATELESS
        dam = factory.IPHC_DAM_ELIDED

        iphc = lowpan.LowpanIPHC(any_tf(), any_nh(), any_hlim(), any_cid(), any_sac(), any_sam(), m, dac, dam)

        data_bytes = bytearray([])

        # WHEN
        actual_dst_addr = factory._decompress_dst_addr(iphc, dst_mac_addr, any_dci(), io.BytesIO(data_bytes))

        # THEN
        self.assertEqual(0, m)
        self.assertEqual(0, dac)
        self.assertEqual(3, dam)
        self.assertEqual(
            self.IPV6_LINKLOCAL_PREFIX + bytearray([dst_mac_addr.mac_address[0] ^ 0x02]) +
            dst_mac_addr.mac_address[1:], actual_dst_addr)

    def test_should_raise_RuntimeError_when_decompress_dst_addr_called_with_m_eq_0_and_dac_eq_1_and_dam_eq_0(self):
        # GIVEN
        factory = lowpan.LowpanIpv6HeaderFactory()

        ipv6_addr = any_dst_addr()

        m = factory.IPHC_M_NO
        dac = factory.IPHC_DAC_STATEFUL
        dam = factory.IPHC_DAM_128B

        iphc = lowpan.LowpanIPHC(any_tf(), any_nh(), any_hlim(), any_cid(), any_sac(), any_sam(), m, dac, dam)

        # WHEN
        self.assertRaises(RuntimeError, factory._decompress_dst_addr, iphc, any_dst_mac_addr(), any_dci(),
                          io.BytesIO(ipv6_addr))

    def test_should_parse_dst_addr_when_decompress_dst_addr_called_with_m_eq_0_and_dac_eq_1_and_dam_eq_1(self):
        # GIVEN
        dci = any_dci()

        context = any_context()

        context_manager = lowpan.ContextManager()
        context_manager[dci] = context

        factory = lowpan.LowpanIpv6HeaderFactory(context_manager)

        eui64 = any_eui64()

        m = factory.IPHC_M_NO
        dac = factory.IPHC_DAC_STATEFUL
        dam = factory.IPHC_DAM_64B

        iphc = lowpan.LowpanIPHC(any_tf(), any_nh(), any_hlim(), any_cid(), any_sac(), any_sam(), m, dac, dam)

        dst_addr = self._merge_prefix_and_address(context.prefix, context.prefix_length, eui64)

        # WHEN
        actual_dst_addr = factory._decompress_dst_addr(iphc, any_dst_mac_addr(), dci, io.BytesIO(eui64))

        # THEN
        self.assertEqual(0, m)
        self.assertEqual(1, dac)
        self.assertEqual(1, dam)
        self.assertEqual(dst_addr, actual_dst_addr)

    def test_should_parse_dst_addr_when_decompress_dst_addr_called_with_m_eq_0_and_dac_eq_1_and_dam_eq_2(self):
        # GIVEN
        dci = any_dci()

        context = any_context()

        context_manager = lowpan.ContextManager()
        context_manager[dci] = context

        factory = lowpan.LowpanIpv6HeaderFactory(context_manager)

        rloc16 = any_rloc16()

        m = factory.IPHC_M_NO
        dac = factory.IPHC_DAC_STATEFUL
        dam = factory.IPHC_DAM_16B

        iphc = lowpan.LowpanIPHC(any_tf(), any_nh(), any_hlim(), any_cid(), any_sac(), any_sam(), m, dac, dam)

        iid = bytearray([0x00, 0x00, 0x00, 0xff, 0xfe, 0x00]) + rloc16

        dst_addr = self._merge_prefix_and_address(context.prefix, context.prefix_length, iid)

        # WHEN
        actual_dst_addr = factory._decompress_dst_addr(iphc, any_dst_mac_addr(), dci, io.BytesIO(rloc16))

        # THEN
        self.assertEqual(0, m)
        self.assertEqual(1, dac)
        self.assertEqual(2, dam)
        self.assertEqual(dst_addr, actual_dst_addr)

    def test_should_parse_dst_addr_when_decompress_dst_addr_called_with_m_eq_0_and_dac_eq_1_and_dam_eq_3(self):
        # GIVEN
        dci = any_dci()

        context = any_context()

        context_manager = lowpan.ContextManager()
        context_manager[dci] = context

        factory = lowpan.LowpanIpv6HeaderFactory(context_manager)

        dst_mac_addr = common.MacAddress.from_eui64(any_dst_mac_addr())

        m = factory.IPHC_M_NO
        dac = factory.IPHC_DAC_STATEFUL
        dam = factory.IPHC_DAM_0B

        iphc = lowpan.LowpanIPHC(any_tf(), any_nh(), any_hlim(), any_cid(), any_sac(), any_sam(), m, dac, dam)

        iid = bytearray([dst_mac_addr.mac_address[0] ^ 0x02]) + dst_mac_addr.mac_address[1:]

        dst_addr = self._merge_prefix_and_address(context.prefix, context.prefix_length, iid)

        data_bytes = bytearray([])

        # WHEN
        actual_dst_addr = factory._decompress_dst_addr(iphc, dst_mac_addr, dci, io.BytesIO(data_bytes))

        # THEN
        self.assertEqual(0, m)
        self.assertEqual(1, dac)
        self.assertEqual(3, dam)
        self.assertEqual(dst_addr, actual_dst_addr)

    def test_should_parse_dst_addr_when_decompress_dst_addr_called_with_m_eq_1_and_dac_eq_0_and_dam_eq_0(self):
        # GIVEN
        factory = lowpan.LowpanIpv6HeaderFactory()

        ipv6_addr = any_dst_addr()

        m = factory.IPHC_M_YES
        dac = factory.IPHC_DAC_STATELESS
        dam = factory.IPHC_DAM_128B

        iphc = lowpan.LowpanIPHC(any_tf(), any_nh(), any_hlim(), any_cid(), any_sac(), any_sam(), m, dac, dam)

        # WHEN
        actual_dst_addr = factory._decompress_dst_addr(iphc, any_dst_mac_addr(), any_dci(), io.BytesIO(ipv6_addr))

        # THEN
        self.assertEqual(1, m)
        self.assertEqual(0, dac)
        self.assertEqual(0, dam)
        self.assertEqual(ipv6_addr, actual_dst_addr)

    def test_should_parse_dst_addr_when_decompress_dst_addr_called_with_m_eq_1_and_dac_eq_0_and_dam_eq_1(self):
        # GIVEN
        factory = lowpan.LowpanIpv6HeaderFactory()

        addr48b = any_48bits_addr()

        m = factory.IPHC_M_YES
        dac = factory.IPHC_DAC_STATELESS
        dam = factory.IPHC_DAM_48B

        iphc = lowpan.LowpanIPHC(any_tf(), any_nh(), any_hlim(), any_cid(), any_sac(), any_sam(), m, dac, dam)

        expected_dst_addr = bytearray([
            0xff, addr48b[0], 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, addr48b[1], addr48b[2], addr48b[3],
            addr48b[4], addr48b[5]
        ])

        # WHEN
        actual_dst_addr = factory._decompress_dst_addr(iphc, any_dst_mac_addr(), any_dci(), io.BytesIO(addr48b))

        # THEN
        self.assertEqual(1, m)
        self.assertEqual(0, dac)
        self.assertEqual(1, dam)
        self.assertEqual(expected_dst_addr, actual_dst_addr)

    def test_should_parse_dst_addr_when_decompress_dst_addr_called_with_m_eq_1_and_dac_eq_0_and_dam_eq_2(self):
        # GIVEN
        factory = lowpan.LowpanIpv6HeaderFactory()

        addr32b = any_32bits_addr()

        m = factory.IPHC_M_YES
        dac = factory.IPHC_DAC_STATELESS
        dam = factory.IPHC_DAM_32B

        iphc = lowpan.LowpanIPHC(any_tf(), any_nh(), any_hlim(), any_cid(), any_sac(), any_sam(), m, dac, dam)

        expected_dst_addr = bytearray([
            0xff, addr32b[0], 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, addr32b[1], addr32b[2],
            addr32b[3]
        ])

        # WHEN
        actual_dst_addr = factory._decompress_dst_addr(iphc, any_dst_mac_addr(), any_dci(), io.BytesIO(addr32b))

        # THEN
        self.assertEqual(1, m)
        self.assertEqual(0, dac)
        self.assertEqual(2, dam)
        self.assertEqual(expected_dst_addr, actual_dst_addr)

    def test_should_parse_dst_addr_when_decompress_dst_addr_called_with_m_eq_1_and_dac_eq_0_and_dam_eq_3(self):
        # GIVEN
        factory = lowpan.LowpanIpv6HeaderFactory()

        addr8b = any_8bits_addr()

        m = factory.IPHC_M_YES
        dac = factory.IPHC_DAC_STATELESS
        dam = factory.IPHC_DAM_8B

        iphc = lowpan.LowpanIPHC(any_tf(), any_nh(), any_hlim(), any_cid(), any_sac(), any_sam(), m, dac, dam)

        expected_dst_addr = bytearray(
            [0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, addr8b[0]])

        # WHEN
        actual_dst_addr = factory._decompress_dst_addr(iphc, any_dst_mac_addr(), any_dci(), io.BytesIO(addr8b))

        # THEN
        self.assertEqual(1, m)
        self.assertEqual(0, dac)
        self.assertEqual(3, dam)
        self.assertEqual(expected_dst_addr, actual_dst_addr)

    def test_should_raise_RuntimeError_when_decompress_dst_addr_called_with_m_eq_1_and_dac_eq_1_and_dam_eq_0(self):
        # GIVEN
        dci = any_dci()

        context = any_context()

        context_manager = lowpan.ContextManager()
        context_manager[dci] = context

        factory = lowpan.LowpanIpv6HeaderFactory(context_manager)

        addr48b = any_48bits_addr()

        m = factory.IPHC_M_YES
        dac = factory.IPHC_DAC_STATEFUL
        dam = factory.IPHC_DAM_128B

        iphc = lowpan.LowpanIPHC(any_tf(), any_nh(), any_hlim(), any_cid(), any_sac(), any_sam(), m, dac, dam)

        prefix = context.prefix[:8]

        if len(prefix) < 8:
            missing_bytes_count = 8 - len(prefix)
            prefix += bytearray([0x00] * missing_bytes_count)

        prefix_length = context.prefix_length

        dst_addr = bytearray([0xff]) + addr48b[:2] + bytearray([prefix_length]) + prefix + addr48b[2:]

        # WHEN
        actual_dst_addr = factory._decompress_dst_addr(iphc, any_dst_mac_addr(), dci, io.BytesIO(addr48b))

        # THEN
        self.assertEqual(1, m)
        self.assertEqual(1, dac)
        self.assertEqual(0, dam)
        self.assertEqual(dst_addr, actual_dst_addr)

    def test_should_raise_RuntimeError_when_decompress_dst_addr_called_with_m_eq_1_and_dac_eq_1_and_dam_eq_1(self):
        # GIVEN
        factory = lowpan.LowpanIpv6HeaderFactory()

        addr48b = any_48bits_addr()

        m = factory.IPHC_M_YES
        dac = factory.IPHC_DAC_STATEFUL
        dam = factory.IPHC_DAM_48B

        iphc = lowpan.LowpanIPHC(any_tf(), any_nh(), any_hlim(), any_cid(), any_sac(), any_sam(), m, dac, dam)

        # WHEN
        self.assertRaises(RuntimeError, factory._decompress_dst_addr, iphc, any_dst_mac_addr(), any_dci(),
                          io.BytesIO(addr48b))

    def test_should_raise_RuntimeError_when_decompress_dst_addr_called_with_m_eq_1_and_dac_eq_1_and_dam_eq_2(self):
        # GIVEN
        factory = lowpan.LowpanIpv6HeaderFactory()

        addr32b = any_32bits_addr()

        m = factory.IPHC_M_YES
        dac = factory.IPHC_DAC_STATEFUL
        dam = factory.IPHC_DAM_32B

        iphc = lowpan.LowpanIPHC(any_tf(), any_nh(), any_hlim(), any_cid(), any_sac(), any_sam(), m, dac, dam)

        # WHEN
        self.assertRaises(RuntimeError, factory._decompress_dst_addr, iphc, any_dst_mac_addr(), any_dci(),
                          io.BytesIO(addr32b))

    def test_should_parse_dst_addr_when_decompress_dst_addr_called_with_m_eq_1_and_dac_eq_1_and_dam_eq_3(self):
        # GIVEN
        factory = lowpan.LowpanIpv6HeaderFactory()

        addr8b = any_8bits_addr()

        m = factory.IPHC_M_YES
        dac = factory.IPHC_DAC_STATEFUL
        dam = factory.IPHC_DAM_8B

        iphc = lowpan.LowpanIPHC(any_tf(), any_nh(), any_hlim(), any_cid(), any_sac(), any_sam(), m, dac, dam)

        # WHEN
        self.assertRaises(RuntimeError, factory._decompress_dst_addr, iphc, any_dst_mac_addr(), any_dci(),
                          io.BytesIO(addr8b))

    def test_should_merge_pfx_with_addr_bytes_when_merge_method_called_with_pfx_shorter_than_missing_bits(self):
        # GIVEN
        factory = lowpan.LowpanIpv6HeaderFactory()

        prefix = bytearray([0x20, 0x00, 0x0d, 0xb8])
        prefix_length = 32

        address_bytes = bytearray([0x1a, 0x2b, 0x3c, 0x4d, 0x5e, 0x6f, 0x70, 0x81])

        addr = prefix + bytearray([0x00] * 4) + address_bytes

        # WHEN
        actual_addr = factory._merge_prefix_with_address(prefix, prefix_length, address_bytes)

        # THEN
        self.assertEqual(addr, actual_addr)

    def test_should_merge_pfx_with_addr_bytes_when_merge_method_called_with_pfx_longer_than_missing_bits_overlap(self):
        # GIVEN
        factory = lowpan.LowpanIpv6HeaderFactory()

        prefix = bytearray([0x20, 0x00, 0x0d, 0xb8, 0x00, 0x00, 0x00, 0x00, 0x22])
        prefix_length = 68

        address_bytes = bytearray([0x1a, 0x2b, 0x3c, 0x4d, 0x5e, 0x6f, 0x70, 0x81])

        addr = prefix[:-1] + bytearray([0x2a]) + address_bytes[1:]

        # WHEN
        actual_addr = factory._merge_prefix_with_address(prefix, prefix_length, address_bytes)

        # THEN
        self.assertEqual(addr, actual_addr)

    def test_should_merge_pfx_with_address_bytes_when_merge_method_called_with_pfx_longer_than_missing_bits(self):
        # GIVEN
        factory = lowpan.LowpanIpv6HeaderFactory()

        prefix = bytearray(
            [0x20, 0x00, 0x0d, 0xb8, 0x00, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x11, 0x01, 0x11, 0x01, 0x22])
        prefix_length = 128

        address_bytes = bytearray([0x1a, 0x2b, 0x3c, 0x4d, 0x5e, 0x6f, 0x70, 0x81])

        addr = prefix

        # WHEN
        actual_addr = factory._merge_prefix_with_address(prefix, prefix_length, address_bytes)

        # THEN
        self.assertEqual(addr, actual_addr)


class TestContext(unittest.TestCase):

    def test_should_extract_context_from_str_representation_when_constructor_called(self):
        # GIVEN
        prefix = "2000:db8::/64"

        # WHEN
        c = lowpan.Context(prefix)

        # THEN
        self.assertEqual(bytearray([0x20, 0x00, 0x0d, 0xb8, 0x00, 0x00, 0x00, 0x00]), c.prefix)
        self.assertEqual(64, c.prefix_length)
        self.assertEqual(8, c.prefix_length_full_bytes)

    def test_should_extract_context_from_bytearray_when_construct_called(self):
        # GIVEN
        prefix = bytearray([0x20, 0x00, 0x0d, 0xb8, 0x00, 0x00, 0x00, 0x00])

        # WHEN
        c = lowpan.Context(prefix)

        # THEN
        self.assertEqual(bytearray([0x20, 0x00, 0x0d, 0xb8, 0x00, 0x00, 0x00, 0x00]), c.prefix)
        self.assertEqual(8, c.prefix_length_full_bytes)
        self.assertEqual(64, c.prefix_length)


class TestContextManager(unittest.TestCase):

    def test_should_raise_IndexError_when_index_is_larger_than_15(self):
        # GIVEN
        context_manager = lowpan.ContextManager()

        index = random.randint(16, 255)

        # WHEN
        with self.assertRaises(IndexError):
            context_manager[index] = any_context()

    def test_should_raise_IndexError_when_index_is_smaller_than_0(self):
        # GIVEN
        context_manager = lowpan.ContextManager()

        index = random.randint(-255, -1)

        # WHEN
        with self.assertRaises(IndexError):
            context_manager[index] = any_context()

    def test_should_raise_TypeError_when_set_value_is_not_Context(self):
        # GIVEN
        context_manager = lowpan.ContextManager()

        # WHEN
        with self.assertRaises(TypeError):
            context_manager[0] = int


class TestLowpanMeshHeader(unittest.TestCase):

    def test_should_return_hops_left_value_when_hops_left_property_called(self):
        # GIVEN
        hops_left = any_hops_left()

        mesh_header = lowpan.LowpanMeshHeader(hops_left, any_mac_address(), any_mac_address())

        # WHEN
        actual_hops_left = mesh_header.hops_left

        # THEN
        self.assertEqual(hops_left, actual_hops_left)

    def test_should_return_originator_address_value_when_originator_address_property_called(self):
        # GIVEN
        originator_address = any_mac_address()

        mesh_header = lowpan.LowpanMeshHeader(any_hops_left(), originator_address, any_mac_address())

        # WHEN
        actual_originator_address = mesh_header.originator_address

        # THEN
        self.assertEqual(originator_address, actual_originator_address)

    def test_should_return_final_destination_address_value_when_final_destination_address_property_called(self):
        # GIVEN
        final_destination_address = any_mac_address()

        mesh_header = lowpan.LowpanMeshHeader(any_hops_left(), any_mac_address(), final_destination_address)

        # WHEN
        actual_final_destination_address = mesh_header.final_destination_address

        # THEN
        self.assertEqual(final_destination_address, actual_final_destination_address)


class TestLowpanMeshHeaderFactory(unittest.TestCase):

    def test_should_create_LowpanMeshHeader_when_parse_method_called(self):
        # GIVEN
        hops_left = any_hops_left()

        originator_address = any_mac_address()
        final_destination_address = any_mac_address()

        v = int(originator_address.type == common.MacAddressType.SHORT)
        f = int(final_destination_address.type == common.MacAddressType.SHORT)

        mesh_header_first_byte = (2 << 6) | (v << 5) | (f << 4)

        if hops_left >= 0x0f:
            mesh_header_data = bytearray([mesh_header_first_byte | 0x0f, hops_left])
        else:
            mesh_header_data = bytearray([mesh_header_first_byte | hops_left])

        mesh_header_data += originator_address.mac_address + final_destination_address.mac_address

        mesh_header_factory = lowpan.LowpanMeshHeaderFactory()

        # WHEN
        mesh_header = mesh_header_factory.parse(io.BytesIO(mesh_header_data), None)

        # THEN
        self.assertEqual(hops_left, mesh_header.hops_left)
        self.assertEqual(originator_address, mesh_header.originator_address)
        self.assertEqual(final_destination_address, mesh_header.final_destination_address)


class TestLowpanFragmentationHeader(unittest.TestCase):

    def test_should_return_datagram_size_value_when_datagram_size_property_called(self):
        # GIVEN
        datagram_size = any_datagram_size()

        fragmentation_header = lowpan.LowpanFragmentationHeader(datagram_size, any_datagram_tag(),
                                                                any_datagram_offset())

        # WHEN
        actual_datagram_size = fragmentation_header.datagram_size

        # THEN
        self.assertEqual(datagram_size, actual_datagram_size)

    def test_should_return_datagram_tag_value_when_datagram_tag_property_called(self):
        # GIVEN
        datagram_tag = any_datagram_tag()

        fragmentation_header = lowpan.LowpanFragmentationHeader(any_datagram_size(), datagram_tag,
                                                                any_datagram_offset())

        # WHEN
        actual_datagram_tag = fragmentation_header.datagram_tag

        # THEN
        self.assertEqual(datagram_tag, actual_datagram_tag)

    def test_should_return_datagram_offset_value_when_datagram_offset_property_called(self):
        # GIVEN
        datagram_offset = any_datagram_offset()

        fragmentation_header = lowpan.LowpanFragmentationHeader(any_datagram_size(), any_datagram_tag(),
                                                                datagram_offset)

        # WHEN
        actual_datagram_offset = fragmentation_header.datagram_offset

        # THEN
        self.assertEqual(datagram_offset, actual_datagram_offset)

    def test_should_return_False_when_is_first_property_called_and_datagram_offset_is_not_eq_0(self):
        # GIVEN
        datagram_offset = random.randint(1, (1 << 8) - 1)

        fragmentation_header = lowpan.LowpanFragmentationHeader(any_datagram_size(), any_datagram_tag(),
                                                                datagram_offset)

        # WHEN
        is_first = fragmentation_header.is_first

        # THEN
        self.assertFalse(is_first)

    def test_should_to_bytes_LowpanFragmentationHeader_from_bytes_when_from_bytes_class_method_called(self):
        # GIVEN
        datagram_size = any_datagram_size()
        datagram_tag = any_datagram_tag()
        datagram_offset = any_datagram_offset()

        data = struct.pack(">HHB", ((3 << 14) | (int(datagram_offset != 0) << 13) | datagram_size), datagram_tag,
                           datagram_offset)

        # WHEN
        fragmentation_header = lowpan.LowpanFragmentationHeader.from_bytes(io.BytesIO(data))

        # THEN
        self.assertEqual(datagram_size, fragmentation_header.datagram_size)
        self.assertEqual(datagram_tag, fragmentation_header.datagram_tag)
        self.assertEqual(datagram_offset, fragmentation_header.datagram_offset)


class TestLowpanDecompressor(unittest.TestCase):

    def test_should_parse_parent_request_when_decompress_method_called(self):
        # GIVEN
        data = bytearray([
            0x7f, 0x3b, 0x02, 0xf0, 0x4d, 0x4c, 0x4d, 0x4c, 0x5e, 0xaf, 0x00, 0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x01, 0x3b, 0xfb, 0x0e, 0x3b, 0x15, 0xa1, 0xf9, 0xf5, 0x64, 0xf4, 0x99, 0xef, 0x70, 0x78, 0x6c,
            0x3c, 0x0f, 0x54, 0x4e, 0x95, 0xe8, 0xf5, 0x27, 0x4c, 0xfc
        ])

        message_info = common.MessageInfo()
        message_info.source_mac_address = common.MacAddress.from_eui64(
            bytearray([0x12, 0xcf, 0xd3, 0x8b, 0x3b, 0x61, 0x55, 0x58]))

        decompressor = config.create_default_lowpan_decompressor(context_manager=None)

        # WHEN
        ipv6_header, extension_headers, udp_header = decompressor.decompress(io.BytesIO(data), message_info)

        # THEN
        self.assertEqual("fe80::10cf:d38b:3b61:5558", ipv6_header.source_address.compressed)
        self.assertEqual("ff02::2", ipv6_header.destination_address.compressed)
        self.assertEqual(17, ipv6_header.next_header)
        self.assertEqual(255, ipv6_header.hop_limit)

        self.assertEqual([], extension_headers)

    def test_should_parse_parent_response_when_decompress_method_called(self):
        # GIVEN
        data = bytearray([
            0x7f, 0x33, 0xf0, 0x4d, 0x4c, 0x4d, 0x4c, 0x0f, 0xe8, 0x00, 0x15, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x01, 0x31, 0xb8, 0x16, 0x02, 0x61, 0xcc, 0x98, 0x90, 0xd6, 0xfd, 0x69, 0xd3, 0x89, 0xa0, 0x30, 0x49,
            0x83, 0x7c, 0xf7, 0xb5, 0x7f, 0x83, 0x2a, 0x04, 0xf6, 0x3b, 0x8c, 0xe8, 0xb6, 0x37, 0x51, 0x5b, 0x28, 0x9a,
            0x3b, 0xbe, 0x0d, 0xb3, 0x4e, 0x9f, 0xd8, 0x14, 0xc8, 0xc9, 0xf4, 0x28, 0xf6, 0x8d, 0xb7, 0xf0, 0x7d, 0x46,
            0x13, 0xc2, 0xb1, 0x69, 0x4d, 0xae, 0xc1, 0x23, 0x16, 0x62, 0x90, 0xea, 0xff, 0x1b, 0xb7, 0xd7, 0x1e, 0x5c
        ])

        message_info = common.MessageInfo()
        message_info.source_mac_address = common.MacAddress.from_eui64(
            bytearray([0x3a, 0x3e, 0x9e, 0xed, 0x7a, 0x01, 0x36, 0xa5]))
        message_info.destination_mac_address = common.MacAddress.from_eui64(
            bytearray([0x12, 0xcf, 0xd3, 0x8b, 0x3b, 0x61, 0x55, 0x58]))

        decompressor = config.create_default_lowpan_decompressor(context_manager=None)

        # WHEN
        ipv6_header, extension_headers, udp_header = decompressor.decompress(io.BytesIO(data), message_info)

        # THEN
        self.assertEqual("fe80::383e:9eed:7a01:36a5", ipv6_header.source_address.compressed)
        self.assertEqual("fe80::10cf:d38b:3b61:5558", ipv6_header.destination_address.compressed)
        self.assertEqual(17, ipv6_header.next_header)
        self.assertEqual(255, ipv6_header.hop_limit)

        self.assertEqual([], extension_headers)

        self.assertEqual(19788, udp_header.src_port)
        self.assertEqual(19788, udp_header.dst_port)

    def test_should_parse_child_id_request_when_decompress_method_called(self):
        # GIVEN
        data = bytearray([
            0x7f, 0x33, 0xf0, 0x4d, 0x4c, 0x4d, 0x4c, 0x9a, 0x62, 0x00, 0x15, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x01, 0x14, 0x03, 0xe3, 0x72, 0x50, 0x4f, 0x8c, 0x5c, 0x42, 0x81, 0x68, 0xe2, 0x11, 0xfc, 0xf5, 0x8c,
            0x62, 0x8e, 0x83, 0x99, 0xe7, 0x26, 0x86, 0x34, 0x3b, 0xa7, 0x68, 0xc7, 0x93, 0xfb, 0x72, 0xd9, 0xcc, 0x13,
            0x5e, 0x5b, 0x96, 0x0e, 0xf1, 0x80, 0x03, 0x55, 0x4f, 0x27, 0xc2, 0x96, 0xf4, 0x9c, 0x65, 0x82, 0x97, 0xcf,
            0x97, 0x35, 0x89, 0xc2
        ])

        message_info = common.MessageInfo()
        message_info.source_mac_address = common.MacAddress.from_eui64(
            bytearray([0x12, 0xcf, 0xd3, 0x8b, 0x3b, 0x61, 0x55, 0x58]))
        message_info.destination_mac_address = common.MacAddress.from_eui64(
            bytearray([0x3a, 0x3e, 0x9e, 0xed, 0x7a, 0x01, 0x36, 0xa5]))

        decompressor = config.create_default_lowpan_decompressor(context_manager=None)

        # WHEN
        ipv6_header, extension_headers, udp_header = decompressor.decompress(io.BytesIO(data), message_info)

        # THEN
        self.assertEqual("fe80::10cf:d38b:3b61:5558", ipv6_header.source_address.compressed)
        self.assertEqual("fe80::383e:9eed:7a01:36a5", ipv6_header.destination_address.compressed)
        self.assertEqual(17, ipv6_header.next_header)
        self.assertEqual(255, ipv6_header.hop_limit)

        self.assertEqual([], extension_headers)

        self.assertEqual(19788, udp_header.src_port)
        self.assertEqual(19788, udp_header.dst_port)

    def test_should_parse_child_id_response_when_decompress_method_called(self):
        # GIVEN
        data = bytearray([
            0x7f, 0x33, 0xf0, 0x4d, 0x4c, 0x4d, 0x4c, 0x7b, 0xe3, 0x00, 0x15, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x01, 0xe0, 0x57, 0xbf, 0x2f, 0xc0, 0x4b, 0x1d, 0xac, 0x3c, 0x24, 0x16, 0xdf, 0xeb, 0x96, 0xeb, 0xda,
            0x42, 0xeb, 0x00, 0x89, 0x5f, 0x39, 0xc9, 0x2b, 0x7d, 0x31, 0xd5, 0x83, 0x9d, 0xdb, 0xb7, 0xc8, 0xe6, 0x25,
            0xd3, 0x7a, 0x1e, 0x5f, 0x66, 0x9e, 0x63, 0x2d, 0x42, 0x27, 0x19, 0x41, 0xdc, 0xc4, 0xc4, 0xc0, 0x8c, 0x07
        ])

        message_info = common.MessageInfo()
        message_info.source_mac_address = common.MacAddress.from_eui64(
            bytearray([0x3a, 0x3e, 0x9e, 0xed, 0x7a, 0x01, 0x36, 0xa5]))
        message_info.destination_mac_address = common.MacAddress.from_eui64(
            bytearray([0x12, 0xcf, 0xd3, 0x8b, 0x3b, 0x61, 0x55, 0x58]))

        decompressor = config.create_default_lowpan_decompressor(context_manager=None)

        # WHEN
        ipv6_header, extension_headers, udp_header = decompressor.decompress(io.BytesIO(data), message_info)

        # THEN
        self.assertEqual("fe80::383e:9eed:7a01:36a5", ipv6_header.source_address.compressed)
        self.assertEqual("fe80::10cf:d38b:3b61:5558", ipv6_header.destination_address.compressed)
        self.assertEqual(17, ipv6_header.next_header)
        self.assertEqual(255, ipv6_header.hop_limit)

        self.assertEqual([], extension_headers)

        self.assertEqual(19788, udp_header.src_port)
        self.assertEqual(19788, udp_header.dst_port)

    def test_should_parse_advertisement_when_decompress_method_called(self):
        # GIVEN
        data = bytearray([
            0x7f, 0x3b, 0x01, 0xf0, 0x4d, 0x4c, 0x4d, 0x4c, 0x35, 0x9f, 0x00, 0x15, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x01, 0x9e, 0xb8, 0xd0, 0x2f, 0x2a, 0xe0, 0x00, 0x5d, 0x66, 0x63, 0x05, 0xa0, 0x59, 0xb0, 0xd4,
            0x95, 0x7f, 0xe6, 0x79, 0x17, 0x87, 0x2c, 0x1d, 0x83, 0xad, 0xc2, 0x64, 0x47, 0x20, 0x7a, 0xe2
        ])

        message_info = common.MessageInfo()
        message_info.source_mac_address = common.MacAddress.from_eui64(
            bytearray([0x3a, 0x3e, 0x9e, 0xed, 0x7a, 0x01, 0x36, 0xa5]))

        decompressor = config.create_default_lowpan_decompressor(context_manager=None)

        # WHEN
        ipv6_header, extension_headers, udp_header = decompressor.decompress(io.BytesIO(data), message_info)

        # THEN
        self.assertEqual("fe80::383e:9eed:7a01:36a5", ipv6_header.source_address.compressed)
        self.assertEqual("ff02::1", ipv6_header.destination_address.compressed)
        self.assertEqual(17, ipv6_header.next_header)
        self.assertEqual(255, ipv6_header.hop_limit)

        self.assertEqual([], extension_headers)

        self.assertEqual(19788, udp_header.src_port)
        self.assertEqual(19788, udp_header.dst_port)


class TestLowpanFragmentsBuffer(unittest.TestCase):

    def test_should_raise_ValueError_when_write_method_called_with_data_length_bigger_than_buffer_length(self):
        # GIVEN
        length = random.randint(1, 1280)

        fragments_buffer = lowpan.LowpanFragmentsBuffer(buffer_size=(length - 1))

        # THEN
        self.assertRaises(ValueError, fragments_buffer.write, any_data(length))

    def test_should_move_write_position_by_the_data_length_when_write_method_called(self):
        # GIVEN
        length = random.randint(1, 1280)

        fragments_buffer = lowpan.LowpanFragmentsBuffer(buffer_size=length)

        start_position = fragments_buffer.tell()

        data = any_data(length=random.randint(1, length))

        # WHEN
        fragments_buffer.write(data)

        # THEN
        self.assertEqual(fragments_buffer.tell() - start_position, len(data))

    def test_should_raise_ValueError_when_read_method_called_but_not_whole_packet_has_been_stored_in_buffer(self):
        # GIVEN
        data = any_data(length=3)

        fragments_buffer = lowpan.LowpanFragmentsBuffer(buffer_size=random.randint(4, 1280))
        fragments_buffer.write(data)

        # WHEN
        self.assertRaises(ValueError, fragments_buffer.read)

    def test_should_raise_ValueError_when_seek_method_called_with_offset_bigger_than_buffer_length(self):
        # GIVEN
        offset = random.randint(1281, 2500)

        fragments_buffer = lowpan.LowpanFragmentsBuffer(buffer_size=1280)

        # THEN
        self.assertRaises(ValueError, fragments_buffer.seek, offset)

    def test_should_set_write_position_when_seek_method_called(self):
        # GIVEN
        length = random.randint(1, 1280)
        offset = random.randint(0, length - 1)

        fragments_buffer = lowpan.LowpanFragmentsBuffer(buffer_size=length)

        # WHEN
        fragments_buffer.seek(offset)

        # THEN
        self.assertEqual(offset, fragments_buffer.tell())

    def test_should_write_whole_packet_to_buffer_when_write_method_called(self):
        # GIVEN
        data = any_data(length=random.randint(1, 1280))

        fragments_buffer = lowpan.LowpanFragmentsBuffer(buffer_size=len(data))

        # WHEN
        fragments_buffer.write(data)

        # THEN
        self.assertEqual(data, fragments_buffer.read())

    def test_should_write_many_frags_to_the_buffer_and_return_whole_message_when_write_method_called_many_times(self):
        # GIVEN
        buffer_size = 42
        fragments_buffer = lowpan.LowpanFragmentsBuffer(buffer_size=buffer_size)

        offset_1 = 0
        fragment_1 = bytearray([0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08])

        offset_2 = 8
        fragment_2 = bytearray([0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10])

        offset_3 = 16
        fragment_3 = bytearray([0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18])

        offset_4 = 24
        fragment_4 = bytearray([0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20])

        offset_5 = 32
        fragment_5 = bytearray([0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28])

        offset_6 = 40
        fragment_6 = bytearray([0x29, 0x2a])

        # WHEN
        fragments_buffer.seek(offset_1)
        fragments_buffer.write(fragment_1)

        fragments_buffer.seek(offset_2)
        fragments_buffer.write(fragment_2)

        fragments_buffer.seek(offset_3)
        fragments_buffer.write(fragment_3)

        fragments_buffer.seek(offset_4)
        fragments_buffer.write(fragment_4)

        fragments_buffer.seek(offset_5)
        fragments_buffer.write(fragment_5)

        fragments_buffer.seek(offset_6)
        fragments_buffer.write(fragment_6)

        # THEN
        self.assertEqual(
            bytearray([
                0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11,
                0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22,
                0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a
            ]), fragments_buffer.read())


class TestLowpanFragmentsBuffersManager(unittest.TestCase):

    def test_should_raise_ValueError_when_get_fragments_buffer_method_called_with_invalid_dgram_size(self):
        # GIVEN
        message_info = common.MessageInfo()
        message_info.source_mac_address = any_mac_address()
        message_info.destination_mac_address = any_mac_address()

        negative_int = -random.randint(1, 1280)

        manager = lowpan.LowpanFragmentsBuffersManager()

        # THEN
        self.assertRaises(ValueError, manager.get_fragments_buffer, message_info, any_datagram_tag(), None)
        self.assertRaises(ValueError, manager.get_fragments_buffer, message_info, any_datagram_tag(), negative_int)

    def test_should_return_LowpanFragmentsBuffer_when_get_fragments_buffer_method_called_with_valid_dgram_size(self):
        # GIVEN
        message_info = common.MessageInfo()
        message_info.source_mac_address = any_mac_address()
        message_info.destination_mac_address = any_mac_address()

        datagram_size = any_datagram_size()

        manager = lowpan.LowpanFragmentsBuffersManager()

        # WHEN
        fragments_buffer = manager.get_fragments_buffer(message_info, any_datagram_tag(), datagram_size)

        # THEN
        self.assertIsInstance(fragments_buffer, lowpan.LowpanFragmentsBuffer)
        self.assertEqual(datagram_size, len(fragments_buffer))


if __name__ == "__main__":
    unittest.main(verbosity=1)
