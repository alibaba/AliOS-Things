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
import ipaddress
import struct

import common
import ipv6


class LowpanIPHC:
    """
    Class representing a compressed IP header.

    More details:
        - URL: https://tools.ietf.org/html/rfc6282
        - section: 3.1. LOWPAN_IPHC Encoding Format
    """

    def __init__(self, tf, nh, hlim, cid, sac, sam, m, dac, dam):
        self._tf = tf
        self._nh = nh
        self._hlim = hlim
        self._cid = cid
        self._sac = sac
        self._sam = sam
        self._m = m
        self._dac = dac
        self._dam = dam

    @property
    def tf(self):
        return self._tf

    @property
    def nh(self):
        return self._nh

    @property
    def hlim(self):
        return self._hlim

    @property
    def cid(self):
        return self._cid

    @property
    def sac(self):
        return self._sac

    @property
    def sam(self):
        return self._sam

    @property
    def m(self):
        return self._m

    @property
    def dac(self):
        return self._dac

    @property
    def dam(self):
        return self._dam

    @classmethod
    def from_bytes(cls, data_bytes):
        data_byte = data_bytes[0]

        hdr = (data_byte >> 5) & 0x07
        if hdr != 0x03:
            raise RuntimeError("Not a 6LowPAN packet.")

        tf = (data_byte >> 3) & 0x03
        nh = (data_byte >> 2) & 0x01
        hlim = data_byte & 0x03

        data_byte = data_bytes[1]

        cid = (data_byte >> 7) & 0x01
        sac = (data_byte >> 6) & 0x01
        sam = (data_byte >> 4) & 0x03
        m = (data_byte >> 3) & 0x01
        dac = (data_byte >> 2) & 0x01
        dam = data_byte & 0x03

        return cls(tf, nh, hlim, cid, sac, sam, m, dac, dam)

    def __repr__(self):
        return "LowpanIPHC(tf={}, nh={}, hlim={}, cid={}, sac={}, sam={}, m={}, dac={}, dam={})".format(
            self.tf,
            self.nh,
            self.hlim,
            self.cid,
            self.sac,
            self.sam,
            self.m,
            self.dac,
            self.dam,
        )


class LowpanNHC:
    """
    Class representing a compressed extension header.

    More details:
        - URL: https://tools.ietf.org/html/rfc6282
        - section: 4.1. LOWPAN_NHC Format

    """

    NHC_EID_HOP_BY_HOP = 0
    NHC_EID_ROUTING = 1
    NHC_EID_FRAGMENT = 2
    NHC_EID_DST_OPT = 3
    NHC_EID_MOBILITY = 4
    NHC_EID_IPV6_HDR = 7

    def __init__(self, eid, nh):
        self._eid = eid
        self._nh = nh

    @property
    def eid(self):
        return self._eid

    @property
    def nh(self):
        return self._nh

    @classmethod
    def from_bytes(cls, data_bytes):
        header_byte = data_bytes[0]

        eid = (header_byte >> 1) & 0x07
        nh = header_byte & 0x01

        return cls(eid, nh)

    def __repr__(self):
        return "LowpanNHC(eid={}, nh={})".format(self.eid, self.nh)


class LowpanUDPHC:
    """
    Class representing compressed UDP header.

    More details:
        - URL: https://tools.ietf.org/html/rfc6282
        - section: 4.3.3. UDP LOWPAN_NHC Format

    """

    def __init__(self, c, p):
        self._c = c
        self._p = p

    @property
    def c(self):
        return self._c

    @property
    def p(self):
        return self._p

    @classmethod
    def from_bytes(cls, data_bytes):
        data_byte = data_bytes[0]

        hdr = (data_byte >> 3) & 0x1F
        if hdr != 0x1E:
            raise RuntimeError("Not a 6LowPAN UDP header.")

        c = (data_byte >> 2) & 0x01
        p = data_byte & 0x03

        return cls(c, p)

    def __repr__(self):
        return "LowpanUDPHC(c={}, p={})".format(self.c, self.p)


class LowpanHopByHopFactory:
    """ Factory that produces HopByHop extension header. """

    def __init__(self, hop_by_hop_options_factory):
        self._hop_by_hop_options_factory = hop_by_hop_options_factory

    def parse(self, data, next_header, message_info):
        ext_header_length = ord(data.read(1))

        ext_header_data = data.read(ext_header_length)

        options = self._hop_by_hop_options_factory.parse(io.BytesIO(ext_header_data), message_info)

        ext_header = ipv6.HopByHop(next_header, options)

        message_info.payload_length += len(ext_header)

        return ext_header


class LowpanExtensionHeadersFactory:
    """ Factory that produces extension headers. """

    NHC_NH_INLINE = 0
    NHC_NH_COMPRESSED = 1

    def __init__(self, ext_headers_factories):
        self._ext_headers_factories = (ext_headers_factories if ext_headers_factories is not None else {})

    def _decompress_nh(self, hc, data):
        if hc.nh == self.NHC_NH_INLINE:
            return ord(data.read(1))

        elif hc.nh == self.NHC_NH_COMPRESSED:
            return None

    def _get_ext_headers_factory(self, eid):
        try:
            return self._ext_headers_factories[eid]

        except BaseException:
            raise RuntimeError("Could not find an extension header factory for the EID type: {}".format(eid))

    def parse(self, data, message_info):
        nhc = LowpanNHC.from_bytes(bytearray(data.read(1)))

        next_header = self._decompress_nh(nhc, data)

        factory = self._get_ext_headers_factory(nhc.eid)

        return factory.parse(data, next_header, message_info)


class LowpanUdpHeaderFactory:
    """ Factory producing UDP header. """

    UDP_HC_C_INLINE = 0
    UDP_HC_C_ELIDED = 1

    UDP_HC_P_BOTH_FULL = 0
    UDP_HC_P_DST_COMPR = 1
    UDP_HC_P_SRC_COMPR = 2
    UDP_HC_P_BOTH_COMPR = 3

    def _decompress_udp_ports(self, udphc, data):
        if udphc.p == self.UDP_HC_P_BOTH_FULL:
            src_port = struct.unpack(">H", data.read(2))[0]
            dst_port = struct.unpack(">H", data.read(2))[0]

        elif udphc.p == self.UDP_HC_P_DST_COMPR:
            src_port = struct.unpack(">H", data.read(2))[0]
            dst_port = 0xf000 + ord(data.read(1))

        elif udphc.p == self.UDP_HC_P_SRC_COMPR:
            src_port = 0xf000 + ord(data.read(1))
            dst_port = struct.unpack(">H", data.read(2))[0]

        elif udphc.p == self.UDP_HC_P_BOTH_COMPR:
            udp_ports_byte = ord(data.read(1))
            src_port = 0xf0b0 + ((udp_ports_byte >> 4) & 0x0F)
            dst_port = 0xf0b0 + (udp_ports_byte & 0x0F)

        return src_port, dst_port

    def _decompress_udp_checksum(self, udphc, data):
        if udphc.c == self.UDP_HC_C_INLINE:
            checksum = struct.unpack(">H", data.read(2))[0]

        if udphc.c == self.UDP_HC_C_ELIDED:
            checksum = 0

        return checksum

    def parse(self, data, message_info):
        udphc = LowpanUDPHC.from_bytes(bytearray(data.read(1)))

        src_port, dst_port = self._decompress_udp_ports(udphc, data)

        checksum = self._decompress_udp_checksum(udphc, data)

        header = ipv6.UDPHeader(src_port, dst_port, checksum=checksum)

        return header


class Context:

    def __init__(self, prefix, prefix_length=None):
        if isinstance(prefix, str):
            prefix, prefix_length = prefix.split("/")
            prefix_length = int(prefix_length)

            a = ipaddress.ip_address(prefix)

            self._prefix = bytearray(a.packed)
            self._prefix_length = prefix_length

        elif isinstance(prefix, bytearray):
            self._prefix = prefix
            self._prefix_length = (prefix_length if prefix_length is not None else len(self._prefix) * 8)

    @property
    def prefix(self):
        return self._prefix[:self.prefix_length_all_bytes]

    @property
    def prefix_full_bytes(self):
        return self._prefix[:self.prefix_length_full_bytes]

    @property
    def prefix_length(self):
        return self._prefix_length

    @property
    def prefix_length_full_bytes(self):
        return int(self._prefix_length / 8)

    @property
    def prefix_length_rest_bits(self):
        return int(self._prefix_length % 8)

    @property
    def prefix_length_all_bytes(self):
        if self.prefix_length_rest_bits > 0:
            return self.prefix_length_full_bytes + 1

        return self.prefix_length_full_bytes


class ContextManager(dict):
    """ Class representing Context Manager. """

    def __check_index(self, index):
        if index < 0 or index > 15:
            raise IndexError("Invalid index: {}. Valid index is in range [0, 15]".format(index))

    def __check_type(self, value):
        if not isinstance(value, Context):
            raise TypeError("Invalid value type: {}".format(type(value)))

    def __getitem__(self, index):
        self.__check_index(index)

        return super(ContextManager, self).__getitem__(index)

    def __setitem__(self, index, value):
        self.__check_index(index)
        self.__check_type(value)

        return super(ContextManager, self).__setitem__(index, value)


class LowpanIpv6HeaderFactory:
    """ Factory that produces IPv6 header. """

    IPV6_LINKLOCAL_PREFIX = bytearray([0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00])

    SHORT_ADDR_PADDING_BYTES = bytearray([0x00, 0x00, 0x00, 0xff, 0xfe, 0x00])

    IPHC_TF_4B = 0
    IPHC_TF_3B = 1
    IPHC_TF_1B = 2
    IPHC_TF_ELIDED = 3

    IPHC_NH_INLINE = 0
    IPHC_NH_COMPRESSED = 1

    IPHC_HLIM_CALCULATE = -1
    IPHC_HLIM_INLINE = 0
    IPHC_HLIM_1 = 1
    IPHC_HLIM_64 = 2
    IPHC_HLIM_255 = 3

    IPHC_CID_CLEAR = 0
    IPHC_CID_SET = 1

    IPHC_SAC_STATELESS = 0
    IPHC_SAC_STATEFUL = 1

    IPHC_SAM_128B = 0
    IPHC_SAM_UNSPECIFIED = 0
    IPHC_SAM_64B = 1
    IPHC_SAM_16B = 2
    IPHC_SAM_0B = 3
    IPHC_SAM_ELIDED = 3

    IPHC_M_NO = 0
    IPHC_M_YES = 1

    IPHC_DAC_STATELESS = 0
    IPHC_DAC_STATEFUL = 1

    IPHC_DAM_128B = 0
    IPHC_DAM_64B = 1
    IPHC_DAM_48B = 1
    IPHC_DAM_32B = 2
    IPHC_DAM_16B = 2
    IPHC_DAM_8B = 3
    IPHC_DAM_0B = 3
    IPHC_DAM_ELIDED = 3

    IPHC_M_DAM_00 = 0
    IPHC_M_DAM_01 = 1
    IPHC_M_DAM_10 = 2
    IPHC_M_DAM_11 = 3

    def __init__(self, context_manager=None):
        self._context_manager = context_manager

    def _flow_label(self, data_bytes):
        flow_label = (data_bytes[0] & 0x0F) << 16
        flow_label += data_bytes[1] << 8
        flow_label += data_bytes[2]
        return flow_label

    def _traffic_class(self, dscp, ecn):
        return (dscp << 2) | ecn

    def _unpack_dscp(self, data_byte):
        return data_byte & 0x3F

    def _unpack_ecn(self, data_byte):
        return data_byte >> 6

    def _decompress_tf_4bytes(self, data):
        data_bytes = [b for b in bytearray(data.read(4))]

        dscp = self._unpack_dscp(data_bytes[0])
        ecn = self._unpack_ecn(data_bytes[0])

        traffic_class = self._traffic_class(dscp, ecn)
        flow_label = self._flow_label(data_bytes[1:])

        return traffic_class, flow_label

    def _decompress_tf_3bytes(self, data):
        data_bytes = [b for b in bytearray(data.read(3))]

        ecn = self._unpack_ecn(data_bytes[0])

        traffic_class = self._traffic_class(dscp=0, ecn=ecn)
        flow_label = self._flow_label(data_bytes)

        return traffic_class, flow_label

    def _decompress_tf_1byte(self, data):
        data_byte = ord(data.read(1))

        dscp = self._unpack_dscp(data_byte)
        ecn = self._unpack_ecn(data_byte)

        traffic_class = self._traffic_class(dscp, ecn)
        flow_label = 0

        return traffic_class, flow_label

    def _decompress_tf(self, iphc, data):
        if iphc.tf == self.IPHC_TF_4B:
            return self._decompress_tf_4bytes(data)

        elif iphc.tf == self.IPHC_TF_3B:
            return self._decompress_tf_3bytes(data)

        elif iphc.tf == self.IPHC_TF_1B:
            return self._decompress_tf_1byte(data)

        elif iphc.tf == self.IPHC_TF_ELIDED:
            return 0, 0

    def _decompress_nh(self, hc, data):
        if hc.nh == self.IPHC_NH_INLINE:
            return ord(data.read(1))

        elif hc.nh == self.IPHC_NH_COMPRESSED:
            return None

    def _decompress_hlim(self, iphc, data):
        if iphc.hlim == self.IPHC_HLIM_INLINE:
            return ord(data.read(1))

        elif iphc.hlim == self.IPHC_HLIM_1:
            return 1

        elif iphc.hlim == self.IPHC_HLIM_64:
            return 64

        elif iphc.hlim == self.IPHC_HLIM_255:
            return 255

    def _decompress_cid(self, iphc, data):
        if iphc.cid == self.IPHC_CID_SET:
            cid = ord(data.read(1))

            sci = (cid >> 4) & 0x0F
            dci = cid & 0x0F

            return sci, dci

        elif iphc.cid == self.IPHC_CID_CLEAR:
            return 0, 0

    def _decompress_src_addr_stateless(self, iphc, src_mac_addr, data):
        if iphc.sam == self.IPHC_SAM_128B:
            return bytearray(data.read(16))

        elif iphc.sam == self.IPHC_SAM_64B:
            return self.IPV6_LINKLOCAL_PREFIX + bytearray(data.read(8))

        elif iphc.sam == self.IPHC_SAM_16B:
            return (self.IPV6_LINKLOCAL_PREFIX + self.SHORT_ADDR_PADDING_BYTES + bytearray(data.read(2)))

        elif iphc.sam == self.IPHC_SAM_ELIDED:
            return self.IPV6_LINKLOCAL_PREFIX + src_mac_addr.convert_to_iid()

    def _merge_prefix_with_address(self, prefix, prefix_length, address_bytes):
        required_bytes = 16

        prefix_length_full_bytes = int(prefix_length / 8)
        prefix_length_rest_bits = int(prefix_length % 8)

        prefix_length_all_bytes = prefix_length_full_bytes

        if prefix_length_rest_bits > 0:
            prefix_length_all_bytes += 1

        # Case in which some bytes overlap
        if (prefix_length_all_bytes + len(address_bytes)) > required_bytes:
            ###################################################################
            # Example:
            #
            # Total address length: 128 bits
            #   * prefix length: 68 bits
            #   * address length: 64 bits
            #
            #   overlap: 4 bits ==> the last 4 bits of the address must be replaced by the last 4 bits of prefix
            #
            # Result:
            #  +--------------------+---------------------+
            #  |  prefix (68 bits)  |  address (64 bits)  |
            #  +--------------------+---------------------+
            ###################################################################

            src_addr = prefix[:prefix_length_full_bytes]
            required_bytes -= prefix_length_full_bytes

            if prefix_length_rest_bits > 0:
                prefix_overlapping_byte = prefix[prefix_length_all_bytes - 1]
                address_overlapping_byte = address_bytes[-required_bytes]

                overlapping_byte = prefix_overlapping_byte & ~(0xff >> prefix_length_rest_bits)
                overlapping_byte |= address_overlapping_byte & (0xff >> prefix_length_rest_bits)

                src_addr += bytearray([overlapping_byte])
                required_bytes -= 1

            if required_bytes > 0:
                src_addr += address_bytes[-required_bytes:]

        else:
            required_bytes -= prefix_length_all_bytes
            required_bytes -= len(address_bytes)

            src_addr = (prefix[:prefix_length_all_bytes] + bytearray([0x00] * required_bytes) + address_bytes)

        return src_addr

    def _decompress_src_addr_stateful(self, iphc, src_mac_addr, sci, data):
        if iphc.sam == self.IPHC_SAM_UNSPECIFIED:
            return bytearray([0x00] * 16)

        elif iphc.sam == self.IPHC_SAM_64B:
            context = self._context_manager[sci]

            return self._merge_prefix_with_address(
                prefix=context.prefix,
                prefix_length=context.prefix_length,
                address_bytes=bytearray(data.read(8)),
            )

        elif iphc.sam == self.IPHC_SAM_16B:
            context = self._context_manager[sci]
            address_bytes = self.SHORT_ADDR_PADDING_BYTES + bytearray(data.read(2))

            return self._merge_prefix_with_address(
                prefix=context.prefix,
                prefix_length=context.prefix_length,
                address_bytes=address_bytes,
            )

        elif iphc.sam == self.IPHC_SAM_0B:
            context = self._context_manager[sci]

            return self._merge_prefix_with_address(
                prefix=context.prefix,
                prefix_length=context.prefix_length,
                address_bytes=src_mac_addr.convert_to_iid(),
            )

    def _decompress_src_addr(self, iphc, src_mac_addr, sci, data):
        if iphc.sac == self.IPHC_SAC_STATELESS:
            return self._decompress_src_addr_stateless(iphc, src_mac_addr, data)

        elif iphc.sac == self.IPHC_SAC_STATEFUL:
            return self._decompress_src_addr_stateful(iphc, src_mac_addr, sci, data)

    def _decompress_unicast_dst_addr_stateless(self, iphc, dst_mac_addr, data):
        if iphc.dam == self.IPHC_DAM_128B:
            return bytearray(data.read(16))

        elif iphc.dam == self.IPHC_DAM_64B:
            return self.IPV6_LINKLOCAL_PREFIX + bytearray(data.read(8))

        elif iphc.dam == self.IPHC_DAM_16B:
            return (self.IPV6_LINKLOCAL_PREFIX + self.SHORT_ADDR_PADDING_BYTES + bytearray(data.read(2)))

        elif iphc.dam == self.IPHC_DAM_ELIDED:
            return self.IPV6_LINKLOCAL_PREFIX + dst_mac_addr.convert_to_iid()

    def _decompress_unicast_dst_addr_stateful(self, iphc, dst_mac_addr, dci, data):
        if iphc.dam == self.IPHC_DAM_128B:
            raise RuntimeError("Reserved")

        elif iphc.dam == self.IPHC_DAM_64B:
            context = self._context_manager[dci]

            return self._merge_prefix_with_address(
                prefix=context.prefix,
                prefix_length=context.prefix_length,
                address_bytes=bytearray(data.read(8)),
            )

        elif iphc.dam == self.IPHC_DAM_16B:
            context = self._context_manager[dci]
            address_bytes = self.SHORT_ADDR_PADDING_BYTES + bytearray(data.read(2))

            return self._merge_prefix_with_address(
                prefix=context.prefix,
                prefix_length=context.prefix_length,
                address_bytes=address_bytes,
            )

        elif iphc.dam == self.IPHC_DAM_0B:
            context = self._context_manager[dci]

            return self._merge_prefix_with_address(
                prefix=context.prefix,
                prefix_length=context.prefix_length,
                address_bytes=dst_mac_addr.convert_to_iid(),
            )

    def _decompress_unicast_dst_addr(self, iphc, dst_mac_addr, dci, data):
        if iphc.dac == self.IPHC_DAC_STATELESS:
            return self._decompress_unicast_dst_addr_stateless(iphc, dst_mac_addr, data)

        elif iphc.dac == self.IPHC_DAC_STATEFUL:
            return self._decompress_unicast_dst_addr_stateful(iphc, dst_mac_addr, dci, data)

    def _decompress_multicast_dst_addr_stateless(self, iphc, data):
        if iphc.dam == self.IPHC_DAM_128B:
            return bytearray(data.read(16))

        elif iphc.dam == self.IPHC_DAM_48B:
            addr48b = bytearray(data.read(6))
            return (bytearray([0xff, addr48b[0]]) + bytearray([0x00] * 9) + addr48b[1:])

        elif iphc.dam == self.IPHC_DAM_32B:
            addr32b = bytearray(data.read(4))
            return (bytearray([0xff, addr32b[0]]) + bytearray([0x00] * 11) + addr32b[1:])

        elif iphc.dam == self.IPHC_DAM_8B:
            return (bytearray([0xff, 0x02]) + bytearray([0x00] * 13) + data.read(1))

    def _decompress_multicast_dst_addr_stateful(self, iphc, dci, data):
        if iphc.dam == self.IPHC_M_DAM_00:
            context = self._context_manager[dci]

            addr48b = bytearray(data.read(6))

            p_bytes_count = 8

            prefix = context.prefix[:p_bytes_count]
            prefix_length = context.prefix_length

            missing_bytes = p_bytes_count - len(prefix)

            if missing_bytes > 0:
                prefix += bytearray([0x00] * missing_bytes)

            return (bytearray([0xff]) + addr48b[:2] + bytearray([prefix_length]) + prefix + addr48b[2:])

        elif iphc.dam == self.IPHC_M_DAM_01:
            raise RuntimeError("Reserved")

        elif iphc.dam == self.IPHC_M_DAM_10:
            raise RuntimeError("Reserved")

        elif iphc.dam == self.IPHC_M_DAM_11:
            raise RuntimeError("Reserved")

    def _decompress_multicast_dst_addr(self, iphc, dci, data):
        if iphc.dac == self.IPHC_DAC_STATELESS:
            return self._decompress_multicast_dst_addr_stateless(iphc, data)

        elif iphc.dac == self.IPHC_DAC_STATEFUL:
            return self._decompress_multicast_dst_addr_stateful(iphc, dci, data)

    def _decompress_dst_addr(self, iphc, dst_mac_addr, dci, data):
        if iphc.m == self.IPHC_M_NO:
            return self._decompress_unicast_dst_addr(iphc, dst_mac_addr, dci, data)

        elif iphc.m == self.IPHC_M_YES:
            return self._decompress_multicast_dst_addr(iphc, dci, data)

    def set_lowpan_context(self, cid, prefix):
        self._context_manager[cid] = Context(prefix)

    def parse(self, data, message_info):
        iphc = LowpanIPHC.from_bytes(bytearray(data.read(2)))

        sci, dci = self._decompress_cid(iphc, data)

        traffic_class, flow_label = self._decompress_tf(iphc, data)

        next_header = self._decompress_nh(iphc, data)

        hop_limit = self._decompress_hlim(iphc, data)

        src_address = self._decompress_src_addr(iphc, message_info.source_mac_address, sci, data)

        dst_address = self._decompress_dst_addr(iphc, message_info.destination_mac_address, dci, data)

        header = ipv6.IPv6Header(src_address, dst_address, traffic_class, flow_label, hop_limit)

        header.next_header = next_header

        return header


class LowpanDecompressor:
    """ Class decompressing 6LoWPAN packets. """

    def __init__(
        self,
        lowpan_ip_header_factory,
        lowpan_extension_headers_factory,
        lowpan_udp_header_factory,
    ):
        self._lowpan_ip_header_factory = lowpan_ip_header_factory
        self._lowpan_extension_headers_factory = (lowpan_extension_headers_factory)
        self._lowpan_udp_header_factory = lowpan_udp_header_factory

    def _is_ipv6_extension_header(self, header_first_byte):
        return ((header_first_byte >> 4) & 0x0F) == 0x0E

    def _is_udp_header(self, header_first_byte):
        return ((header_first_byte >> 4) & 0x0F) == 0x0F

    def _peek_n_bytes(self, data, n):
        read_data = data.read(n)
        data.seek(-n, io.SEEK_CUR)
        return read_data

    def _is_next_header_compressed(self, header):
        return header.next_header is None

    def set_lowpan_context(self, cid, prefix):
        self._lowpan_ip_header_factory.set_lowpan_context(cid, prefix)

    def decompress(self, data, message_info):
        ipv6_header = self._lowpan_ip_header_factory.parse(data, message_info)

        previous_header = ipv6_header

        extension_headers = []
        udp_header = None

        if self._is_next_header_compressed(ipv6_header):

            while data.tell() < len(data.getvalue()):
                header_first_byte = ord(self._peek_n_bytes(data, 1))

                if self._is_ipv6_extension_header(header_first_byte):
                    extension_header = self._lowpan_extension_headers_factory.parse(data, message_info)
                    extension_headers.append(extension_header)

                    # Update next header field in the previous header
                    previous_header.next_header = extension_header.type
                    previous_header = extension_header

                    if not self._is_next_header_compressed(extension_header):
                        # There is not more compressed headers
                        break

                elif self._is_udp_header(header_first_byte):
                    udp_header = self._lowpan_udp_header_factory.parse(data, message_info)

                    # Update next header field in the previous header
                    previous_header.next_header = udp_header.type

                    # There is not more headers after UDP header
                    break

        return ipv6_header, extension_headers, udp_header


class LowpanMeshHeader(object):
    """ Class representing 6LoWPAN mesh header (RFC 4944 5.2). """

    def __init__(self, hops_left, originator_address, final_destination_address):
        self._hops_left = hops_left
        self._originator_address = originator_address
        self._final_destination_address = final_destination_address

    @property
    def hops_left(self):
        return self._hops_left

    @property
    def originator_address(self):
        return self._originator_address

    @property
    def final_destination_address(self):
        return self._final_destination_address


class LowpanMeshHeaderFactory:

    def _parse_address(self, data, is_short):
        if is_short:
            return common.MacAddress.from_rloc16(bytearray(data.read(2)))
        else:
            return common.MacAddress.from_eui64(bytearray(data.read(8)))

    def parse(self, data, message_info):
        data_byte = ord(data.read(1))

        is_short_originator_address = bool(data_byte & 0x20)
        is_short_final_destination_address = bool(data_byte & 0x10)

        if (data_byte & 0x0F) != 0x0F:
            hops_left = data_byte & 0x0F
        else:
            hops_left = ord(data.read(1))

        originator_address = self._parse_address(data, is_short_originator_address)
        final_destination_address = self._parse_address(data, is_short_final_destination_address)

        return LowpanMeshHeader(hops_left, originator_address, final_destination_address)


class LowpanFragmentationHeader(object):

    def __init__(self, datagram_size, datagram_tag, datagram_offset=0):
        self._datagram_size = datagram_size
        self._datagram_tag = datagram_tag
        self._datagram_offset = datagram_offset

    @property
    def datagram_size(self):
        return self._datagram_size

    @property
    def datagram_tag(self):
        return self._datagram_tag

    @property
    def datagram_offset(self):
        return self._datagram_offset

    @property
    def is_first(self):
        return self.datagram_offset == 0

    @classmethod
    def from_bytes(cls, data):
        datagram_size = struct.unpack(">H", data.read(2))[0]
        has_offset = ((datagram_size >> 11) & 0x1F) == 0x1C

        datagram_size &= 0x7FF
        datagram_tag = struct.unpack(">H", data.read(2))[0]
        datagram_offset = 0

        if has_offset:
            datagram_offset = ord(data.read(1))

        return cls(datagram_size, datagram_tag, datagram_offset)


class LowpanFragmentsBuffer(object):

    def __init__(self, buffer_size):
        self._buffer = [None] * buffer_size
        self._position = 0

    def write(self, data):
        if (self._position + len(data)) > len(self._buffer):
            raise ValueError("Write failure. Data length is bigger than the destination buffer length.")

        for i, byte in enumerate(data):
            self._buffer[self._position + i] = byte

        self._position += len(data)
        return len(data)

    def seek(self, offset):
        if offset >= len(self._buffer):
            raise ValueError("Could not seek current offset. Offset value is bigger than the buffer length.")

        self._position = offset

    def tell(self):
        return self._position

    def whole_packet_received(self):
        return all([byte is not None for byte in self._buffer])

    def read(self):
        if not self.whole_packet_received():
            raise ValueError("Only a part of the packet has been stored in the buffer.")

        return bytearray(self._buffer)

    def __len__(self):
        return len(self._buffer)


class LowpanFragmentsBuffersManager(object):

    def __init__(self):
        self._fragments_buffers = {}

    def _create_key(self, message_info, datagram_tag):
        key = (bytes(message_info.source_mac_address.mac_address) +
               bytes(message_info.destination_mac_address.mac_address) + bytes(datagram_tag))
        return key

    def _allocate_fragments_buffer(self, key, datagram_size):
        if datagram_size is None or datagram_size < 0:
            raise ValueError("Could not allocate fragments buffer. Invalid datagram size: {}".format(datagram_size))

        fragments_buffer = LowpanFragmentsBuffer(datagram_size)

        self._fragments_buffers[key] = fragments_buffer
        return fragments_buffer

    def get_fragments_buffer(self, message_info, datagram_tag, datagram_size=None):
        key = self._create_key(message_info, datagram_tag)

        if key not in self._fragments_buffers:
            self._allocate_fragments_buffer(key, datagram_size)

        return self._fragments_buffers[key]

    def free_fragments_buffer(self, message_info, datagram_tag):
        key = self._create_key(message_info, datagram_tag)

        del self._fragments_buffers[key]


class LowpanParser(object):

    def __init__(
        self,
        lowpan_mesh_header_factory,
        lowpan_decompressor,
        lowpan_fragements_buffers_manager,
        ipv6_packet_factory,
    ):
        self._lowpan_mesh_header_factory = lowpan_mesh_header_factory
        self._lowpan_decompressor = lowpan_decompressor
        self._lowpan_fragments_buffers_manager = (lowpan_fragements_buffers_manager)
        self._ipv6_packet_factory = ipv6_packet_factory

    def _peek_n_bytes(self, data, n):
        data_bytes = data.read(n)
        data.seek(-n, io.SEEK_CUR)
        return data_bytes

    def _is_mesh_header(self, first_byte):
        return ((first_byte >> 6) & 0x03) == 0x02

    def _is_first_fragmentation_header(self, first_byte):
        return ((first_byte >> 3) & 0x1F) == 0x18

    def _is_subsequent_fragmentation_header(self, first_byte):
        return ((first_byte >> 3) & 0x1F) == 0x1C

    def _is_iphc(self, first_byte):
        return ((first_byte >> 5) & 0x07) == 0x03

    def _decompress_iphc(self, data, message_info):
        return self._lowpan_decompressor.decompress(data, message_info)

    def _handle_first_fragmentation_header(self, data, message_info):
        fragmentation_header = LowpanFragmentationHeader.from_bytes(data)

        fragments_buffer = self._lowpan_fragments_buffers_manager.get_fragments_buffer(
            message_info,
            fragmentation_header.datagram_tag,
            fragmentation_header.datagram_size,
        )

        ipv6_header, extension_headers, udp_header = self._decompress_iphc(data, message_info)

        uncompressed_data = data.read()

        # Update payload lengths
        ipv6_header.payload_length = fragmentation_header.datagram_size - len(ipv6_header)

        fragments_buffer.seek(0)
        fragments_buffer.write(ipv6_header.to_bytes())

        for extension_header in extension_headers:
            fragments_buffer.write(extension_header.to_bytes())

        if udp_header is not None:
            fragments_buffer.write(udp_header.to_bytes())

        fragments_buffer.write(uncompressed_data)

        if fragments_buffer.whole_packet_received():
            data = io.BytesIO(fragments_buffer.read())

            self._lowpan_fragments_buffers_manager.free_fragments_buffer(message_info,
                                                                         fragmentation_header.datagram_tag)

            return self._ipv6_packet_factory.parse(data, message_info)

        return None

    def _handle_subsequent_fragmentation_header(self, data, message_info):
        fragmentation_header = LowpanFragmentationHeader.from_bytes(data)

        fragments_buffer = self._lowpan_fragments_buffers_manager.get_fragments_buffer(
            message_info,
            fragmentation_header.datagram_tag,
            fragmentation_header.datagram_size,
        )

        offset = fragmentation_header.datagram_offset * 8

        fragments_buffer.seek(offset)
        fragments_buffer.write(data.read())

        if fragments_buffer.whole_packet_received():
            data = io.BytesIO(fragments_buffer.read())

            self._lowpan_fragments_buffers_manager.free_fragments_buffer(message_info,
                                                                         fragmentation_header.datagram_tag)

            return self._ipv6_packet_factory.parse(data, message_info)

        return None

    def _handle_iphc_header(self, data, message_info):
        ipv6_header, extension_headers, udp_header = self._decompress_iphc(data, message_info)

        uncompressed_data = data.read()

        decompressed_data = bytearray([])

        for extension_header in extension_headers:
            decompressed_data += extension_header.to_bytes()

        if udp_header is not None:
            udp_header.payload_length = len(uncompressed_data)

            decompressed_data += udp_header.to_bytes()

        decompressed_data += uncompressed_data

        ipv6_header.payload_length = len(decompressed_data)

        decompressed_data = ipv6_header.to_bytes() + decompressed_data

        return self._ipv6_packet_factory.parse(io.BytesIO(decompressed_data), message_info)

    def set_lowpan_context(self, cid, prefix):
        self._lowpan_decompressor.set_lowpan_context(cid, prefix)

    def parse(self, data, message_info):

        while data.tell() < len(data.getvalue()):
            first_byte = ord(self._peek_n_bytes(data, n=1))

            if self._is_mesh_header(first_byte):
                mesh_header = self._lowpan_mesh_header_factory.parse(data, message_info)

                message_info.source_mac_address = (mesh_header.originator_address)
                message_info.destination_mac_address = (mesh_header.final_destination_address)

            elif self._is_first_fragmentation_header(first_byte):
                return self._handle_first_fragmentation_header(data, message_info)

            elif self._is_subsequent_fragmentation_header(first_byte):
                return self._handle_subsequent_fragmentation_header(data, message_info)

            elif self._is_iphc(first_byte):
                return self._handle_iphc_header(data, message_info)

            else:
                raise RuntimeError("Unsupported header type: 0x{:02x}".format(first_byte))
