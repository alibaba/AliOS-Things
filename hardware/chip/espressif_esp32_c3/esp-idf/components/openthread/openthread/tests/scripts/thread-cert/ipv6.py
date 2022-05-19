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

import abc
import io
import struct

from binascii import hexlify
from ipaddress import ip_address

import common

try:
    from itertools import izip_longest as zip_longest
except ImportError:
    from itertools import zip_longest

# Next headers for IPv6 protocols
IPV6_NEXT_HEADER_HOP_BY_HOP = 0
IPV6_NEXT_HEADER_TCP = 6
IPV6_NEXT_HEADER_UDP = 17
IPV6_NEXT_HEADER_FRAGMENT = 44
IPV6_NEXT_HEADER_ICMP = 58

UPPER_LAYER_PROTOCOLS = [
    IPV6_NEXT_HEADER_TCP,
    IPV6_NEXT_HEADER_UDP,
    IPV6_NEXT_HEADER_ICMP,
]

# ICMP Protocol codes
ICMP_DESTINATION_UNREACHABLE = 1
ICMP_TIME_EXCEEDED = 3
ICMP_ECHO_REQUEST = 128
ICMP_ECHO_RESPONSE = 129

# Default hop limit for IPv6
HOP_LIMIT_DEFAULT = 64


def calculate_checksum(data):
    """ Calculate checksum from data bytes.

    How to calculate checksum (RFC 2460):
        https://tools.ietf.org/html/rfc2460#page-27

    Args:
        data (bytes): input data from which checksum will be calculated

    Returns:
        int: calculated checksum
    """
    # Create halfwords from data bytes. Example: data[0] = 0x01, data[1] =
    # 0xb2 => 0x01b2
    halfwords = [((byte0 << 8) | byte1) for byte0, byte1 in zip_longest(data[::2], data[1::2], fillvalue=0x00)]

    checksum = 0
    for halfword in halfwords:
        checksum += halfword
        checksum = (checksum & 0xffff) + (checksum >> 16)

    checksum ^= 0xffff

    if checksum == 0:
        return 0xffff
    else:
        return checksum


class PacketFactory(object):
    """ Interface for classes that produce objects from data. """

    def parse(self, data, message_info):
        """ Convert data to object.

        Args:
            data (BytesIO)
            message_info (MessageInfo)

        """
        raise NotImplementedError


class BuildableFromBytes(object):
    """ Interface for classes which can be built from bytes. """

    @classmethod
    def from_bytes(cls, data):
        """ Convert data to object.

        Args:
            data (bytes)

        """
        raise NotImplementedError


class ConvertibleToBytes(object):
    """ Interface for classes which can be converted to bytes. """

    def to_bytes(self):
        """ Convert object to data.

        Returns:
            bytes
        """
        raise NotImplementedError

    def __len__(self):
        """ Length of data (in bytes).

        Returns:
            int
        """
        raise NotImplementedError


class Header(object):
    """ Interface for header classes. """

    __metaclass__ = abc.ABCMeta

    @abc.abstractproperty
    def type(self):
        """ Number which can be used in the next header field in IPv6 header or next headers.

        Returns:
            int
        """


class ExtensionHeader(object):
    """ Base for classes representing Extension Headers in IPv6 packets. """

    def __init__(self, next_header, hdr_ext_len=0):
        self.next_header = next_header
        self.hdr_ext_len = hdr_ext_len


class UpperLayerProtocol(Header, ConvertibleToBytes):
    """ Base for classes representing upper layer protocol payload in IPv6 packets. """

    def __init__(self, header):
        self.header = header

    @property
    def checksum(self):
        """ Return checksum from upper layer protocol header. """
        return self.header.checksum

    @checksum.setter
    def checksum(self, value):
        """ Set checksum value in upper layer protocol header. """
        self.header.checksum = value

    def is_valid_checksum(self):
        """ Return information if set checksum is valid.

        It is not possible to get zero from checksum calculation.
        Zero indicates invalid checksum value.

        Returns:
            bool
        """
        return self.checksum != 0


class IPv6PseudoHeader(ConvertibleToBytes):
    """ Class representing IPv6 pseudo header which is required to calculate
    upper layer protocol (like e.g. UDP or ICMPv6) checksum.

    This class is used only during upper layer protocol checksum calculation. Do not use it outside of this module.

    """

    def __init__(self, source_address, destination_address, payload_length, next_header):
        self._source_address = self._convert_to_ipaddress(source_address)
        self._destination_address = self._convert_to_ipaddress(destination_address)
        self.payload_length = payload_length
        self.next_header = next_header

    def _convert_to_ipaddress(self, value):
        if isinstance(value, bytearray):
            value = bytes(value)

        return ip_address(value)

    @property
    def source_address(self):
        return self._source_address

    @source_address.setter
    def source_address(self, value):
        self._source_address = self._convert_to_ipaddress(value)

    @property
    def destination_address(self):
        return self._destination_address

    @destination_address.setter
    def destination_address(self, value):
        self._source_address = self._convert_to_ipaddress(value)

    def to_bytes(self):
        data = bytearray()
        data += self.source_address.packed
        data += self.destination_address.packed
        data += struct.pack(">I", self.payload_length)
        data += struct.pack(">I", self.next_header)

        return data


class IPv6Header(ConvertibleToBytes, BuildableFromBytes):
    """ Class representing IPv6 packet header. """

    _version = 6

    _header_length = 40

    def __init__(
        self,
        source_address,
        destination_address,
        traffic_class=0,
        flow_label=0,
        hop_limit=64,
        payload_length=0,
        next_header=0,
    ):
        self.version = self._version
        self._source_address = self._convert_to_ipaddress(source_address)
        self._destination_address = self._convert_to_ipaddress(destination_address)
        self.traffic_class = traffic_class
        self.flow_label = flow_label
        self.hop_limit = hop_limit
        self.payload_length = payload_length
        self.next_header = next_header

    def _convert_to_ipaddress(self, value):
        if isinstance(value, bytearray):
            value = bytes(value)

        return ip_address(value)

    @property
    def source_address(self):
        return self._source_address

    @source_address.setter
    def source_address(self, value):
        self._source_address = self._convert_to_ipaddress(value)

    @property
    def destination_address(self):
        return self._destination_address

    def to_bytes(self):
        data = bytearray([
            ((self.version & 0x0F) << 4) | ((self.traffic_class >> 4) & 0x0F),
            ((self.traffic_class & 0x0F) << 4) | ((self.flow_label >> 16) & 0x0F),
            ((self.flow_label >> 8) & 0xff),
            ((self.flow_label & 0xff)),
        ])
        data += struct.pack(">H", self.payload_length)
        data += bytearray([self.next_header, self.hop_limit])
        data += self.source_address.packed
        data += self.destination_address.packed

        return data

    @classmethod
    def from_bytes(cls, data):
        b = bytearray(data.read(4))

        (b[0] >> 4) & 0x0F
        traffic_class = ((b[0] & 0x0F) << 4) | ((b[1] >> 4) & 0x0F)
        flow_label = ((b[1] & 0x0F) << 16) | (b[2] << 8) | b[3]

        payload_length = struct.unpack(">H", data.read(2))[0]
        next_header = ord(data.read(1))
        hop_limit = ord(data.read(1))
        src_addr = bytearray(data.read(16))
        dst_addr = bytearray(data.read(16))

        return cls(
            src_addr,
            dst_addr,
            traffic_class,
            flow_label,
            hop_limit,
            payload_length,
            next_header,
        )

    def __repr__(self):
        return "IPv6Header(source_address={}, destination_address={}, next_header={}, payload_length={}, \
            hop_limit={}, traffic_class={}, flow_label={})".format(
            self.source_address.compressed,
            self.destination_address.compressed,
            self.next_header,
            self.payload_length,
            self.hop_limit,
            self.traffic_class,
            self.flow_label,
        )

    def __len__(self):
        return self._header_length


class IPv6Packet(ConvertibleToBytes):
    """ Class representing IPv6 packet.

    IPv6 packet consists of IPv6 header, optional extension header, and upper layer protocol.

                                            IPv6 packet

    +-------------+----------------------------------+----------------------------------------------+
    |             |                                  |                                              |
    | IPv6 header | extension headers (zero or more) | upper layer protocol (e.g. UDP, TCP, ICMPv6) |
    |             |                                  |                                              |
    +-------------+----------------------------------+----------------------------------------------+

    Extension headers:
        - HopByHop
        - Routing header (not implemented in this module)
        - Fragment Header

    Upper layer protocols:
        - ICMPv6
        - UDP
        - TCP (not implemented in this module)

    Example:
        IPv6 packet construction without extension headers:

        ipv6_packet = IPv6Packet(IPv6Header("fd00:1234:4555::ff:fe00:1800", "ff03::1"),
                                 ICMPv6(ICMPv6Header(128, 0),
                                        ICMPv6EchoBody(0, 2,  bytes([0x80, 0x00, 0xc7, 0xbf, 0x00, 0x00, 0x00, 0x01,
                                                                     0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41,
                                                                     0x41, 0x41]))))

        IPv6 packet construction with extension headers:

        ipv6_packet = IPv6Packet(IPv6Header("fd00:1234:4555::ff:fe00:1800", "ff03::1"),
                                 ICMPv6(ICMPv6Header(128, 0),
                                        ICMPv6EchoBody(0, 2,  bytes([0x80, 0x00, 0xc7, 0xbf, 0x00, 0x00, 0x00, 0x01,
                                                                     0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41,
                                                                     0x41, 0x41])),
                                 [HopByHop(options=[
                                     HopByHopOption(HopByHopOptionHeader(_type=0x6d),
                                                    MPLOption(S=1, M=0, V=0, sequence=2, seed_id=bytes([0x00, 0x18])))
                                 ])])

    """

    def __init__(self, ipv6_header, upper_layer_protocol, extension_headers=None):
        self.ipv6_header = ipv6_header

        self.upper_layer_protocol = upper_layer_protocol

        self.extension_headers = (extension_headers if extension_headers is not None else [])

        self._update_next_header_values_in_headers()

        if not upper_layer_protocol.is_valid_checksum():
            self.upper_layer_protocol.checksum = self.calculate_checksum()

    def _validate_checksum(self):
        checksum = self.calculate_checksum()

        if self.upper_layer_protocol.checksum != checksum:
            raise RuntimeError("Could not create IPv6 packet. "
                               "Invalid checksum: {}!={}".format(self.upper_layer_protocol.checksum, checksum))

        self.upper_layer_protocol.checksum = checksum

    def _update_payload_length_value_in_ipv6_header(self):
        self.ipv6_header.payload_length = len(self.upper_layer_protocol) + sum(
            [len(extension_header) for extension_header in self.extension_headers])

    def _update_next_header_values_in_headers(self):
        last_header = self.ipv6_header

        for extension_header in self.extension_headers:
            last_header.next_header = extension_header.type
            last_header = extension_header

        last_header.next_header = self.upper_layer_protocol.type

    def calculate_checksum(self):
        saved_checksum = self.upper_layer_protocol.checksum

        self.upper_layer_protocol.checksum = 0

        upper_layer_protocol_bytes = self.upper_layer_protocol.to_bytes()

        self.upper_layer_protocol.checksum = saved_checksum

        pseudo_header = IPv6PseudoHeader(
            self.ipv6_header.source_address,
            self.ipv6_header.destination_address,
            len(upper_layer_protocol_bytes),
            self.upper_layer_protocol.type,
        )

        return calculate_checksum(pseudo_header.to_bytes() + upper_layer_protocol_bytes)

    def to_bytes(self):
        self._update_payload_length_value_in_ipv6_header()
        self._update_next_header_values_in_headers()
        self.upper_layer_protocol.checksum = self.calculate_checksum()

        ipv6_packet = self.ipv6_header.to_bytes()

        for extension_header in self.extension_headers:
            ipv6_packet += extension_header.to_bytes()

        ipv6_packet += self.upper_layer_protocol.to_bytes()

        return ipv6_packet

    def __repr__(self):
        return "IPv6Packet(header={}, upper_layer_protocol={})".format(self.ipv6_header, self.upper_layer_protocol)


class UDPHeader(ConvertibleToBytes, BuildableFromBytes):
    """ Class representing UDP datagram header.

    This header is required to construct UDP datagram.

    """

    _header_length = 8

    def __init__(self, src_port, dst_port, payload_length=0, checksum=0):
        self.src_port = src_port
        self.dst_port = dst_port

        self._payload_length = payload_length
        self.checksum = checksum

    @property
    def type(self):
        return 17

    @property
    def payload_length(self):
        return self._payload_length

    @payload_length.setter
    def payload_length(self, value):
        self._payload_length = self._header_length + value

    def to_bytes(self):
        data = struct.pack(">H", self.src_port)
        data += struct.pack(">H", self.dst_port)
        data += struct.pack(">H", self.payload_length)
        data += struct.pack(">H", self.checksum)

        return data

    @classmethod
    def from_bytes(cls, data):
        src_port = struct.unpack(">H", data.read(2))[0]
        dst_port = struct.unpack(">H", data.read(2))[0]
        payload_length = struct.unpack(">H", data.read(2))[0]
        checksum = struct.unpack(">H", data.read(2))[0]

        return cls(src_port, dst_port, payload_length, checksum)

    def __len__(self):
        return self._header_length


class UDPDatagram(UpperLayerProtocol):
    """ Class representing UDP datagram.

    UDP is an upper layer protocol for IPv6 so it can be passed to IPv6 packet as upper_layer_protocol.

    This class consists of a UDP header and payload. The example below shows how a UDP datagram can be constructed.

    Example:
        udp_dgram = UDPDatagram(UDPHeader(src_port=19788, dst_port=19788),
                                bytes([0x00, 0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                       0x00, 0x00, 0x01, 0x09, 0x01, 0x01, 0x0b, 0x03,
                                       0x04, 0xc6, 0x69, 0x73, 0x51, 0x0e, 0x01, 0x80,
                                       0x12, 0x02, 0x00, 0x01, 0xde, 0xad, 0xbe, 0xef]))

    """

    @property
    def type(self):
        return 17

    def __init__(self, header, payload):
        super(UDPDatagram, self).__init__(header)
        self.payload = payload

    def to_bytes(self):
        self.header.payload_length = len(self.payload)

        data = bytearray()
        data += self.header.to_bytes()
        data += self.payload.to_bytes()
        return data

    def __len__(self):
        return len(self.header) + len(self.payload)


class ICMPv6Header(ConvertibleToBytes, BuildableFromBytes):
    """ Class representing ICMPv6 message header.

    This header is required to construct ICMPv6 message.

    """

    _header_length = 4

    def __init__(self, _type, code, checksum=0):
        self.type = _type
        self.code = code

        self.checksum = checksum

    def to_bytes(self):
        return bytearray([self.type, self.code]) + struct.pack(">H", self.checksum)

    @classmethod
    def from_bytes(cls, data):
        _type = ord(data.read(1))
        code = ord(data.read(1))
        checksum = struct.unpack(">H", data.read(2))[0]

        return cls(_type, code, checksum)

    def __len__(self):
        return self._header_length


class ICMPv6(UpperLayerProtocol):
    """ Class representing ICMPv6 message.

    ICMPv6 is an upper layer protocol for IPv6 so it can be passed to IPv6 packet as upper_layer_protocol.

    This class consists of an ICMPv6 header and body. The example below shows how an ICMPv6 message can be constructed.

    Example:
        icmpv6_msg = ICMPv6(ICMPv6Header(128, 0),
                            ICMPv6EchoBody(0, 2, bytes([0x80, 0x00, 0xc7, 0xbf, 0x00, 0x00, 0x00, 0x01,
                                                        0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41,
                                                        0x41, 0x41])))

    """

    @property
    def type(self):
        return 58

    def __init__(self, header, body):
        super(ICMPv6, self).__init__(header)
        self.body = body

    def to_bytes(self):
        return bytearray(self.header.to_bytes() + self.body.to_bytes())

    def __len__(self):
        return len(self.header) + len(self.body)


class FragmentHeader(ExtensionHeader):
    """ Class representing Fragment extension header.

    +-------------+----------+-----------------+-----+---+----------------+
    | Next Header | Reserved | Fragment Offset | Res | M | Identification |
    +-------------+----------+-----------------+-----+---+----------------+

    Fragment extention header consists of:
        - next_header type (8 bit)
        - fragment offset which is multiple of 8 (13 bit)
        - more_flag to indicate further data (1 bit)
        - identification for all associated fragments (32 bit)
    """

    @property
    def type(self):
        return 44

    @property
    def identification(self):
        return self._identification

    @property
    def more_flag(self):
        return self._more_flag

    @property
    def offset(self):
        return self._fragm_offset

    def __init__(self, next_header=None, fragm_offset=0, more_flag=False, identification=0):
        super(FragmentHeader, self).__init__(next_header, 0)
        self._fragm_offset = fragm_offset
        self._more_flag = more_flag
        self._identification = identification

    def callculate_offset(self, position):
        return position >> 3

    def to_bytes(self):
        data = bytearray([self.next_header, 0x00])
        data += bytearray([self._fragm_offset >> 5, ((self._fragm_offset << 3) | self._more_flag) & 0xff])
        data += struct.pack(">I", self._identification)

        return data

    @classmethod
    def from_bytes(cls, data):
        next_header = struct.unpack(">B", data.read(1))[0]
        struct.unpack(">B", data.read(1))[0]  # reserved
        fragment_offset = struct.unpack(">H", data.read(2))[0]
        more_flag = fragment_offset & 0x1
        identificaton = struct.unpack(">I", data.read(4))[0]

        fragment_offset = fragment_offset >> 3

        return cls(next_header, fragment_offset, more_flag, identificaton)

    def __len__(self):
        return 64


class HopByHop(ExtensionHeader):
    """ Class representing HopByHop extension header.

    HopByHop extension header consists of:
        - next_header type
        - extension header length which is multiple of 8
        - options

    """

    _one_byte_padding = 0x00
    _many_bytes_padding = 0x01

    @property
    def type(self):
        return 0

    def __init__(self, next_header=None, options=None, hdr_ext_len=None):
        super(HopByHop, self).__init__(next_header, hdr_ext_len)
        self.options = options if options is not None else []

        if hdr_ext_len is not None:
            self.hdr_ext_len = hdr_ext_len
        else:
            payload_length = self._calculate_payload_length()
            self.hdr_ext_len = self._calculate_hdr_ext_len(payload_length)

    def _calculate_payload_length(self):
        payload_length = 2

        for option in self.options:
            payload_length += len(option)

        return payload_length

    def _calculate_hdr_ext_len(self, payload_length):
        count = payload_length >> 3

        if (payload_length & 0x7) == 0 and count > 0:
            return count - 1

        return count

    def to_bytes(self):
        data = bytearray([self.next_header, self.hdr_ext_len])

        for option in self.options:
            data += option.to_bytes()

        # Padding
        #
        # More details:
        #   https://tools.ietf.org/html/rfc2460#section-4.2
        #
        excess_bytes = len(data) & 0x7

        if excess_bytes > 0:
            padding_length = 8 - excess_bytes

            if padding_length == 1:
                data += bytearray([self._one_byte_padding])

            else:
                padding_length -= 2
                data += bytearray([self._many_bytes_padding, padding_length])
                data += bytearray([0x00 for _ in range(padding_length)])

        return data

    def __len__(self):
        """ HopByHop extension header length

        More details:
            https://tools.ietf.org/html/rfc2460#section-4.3

        """
        return (self.hdr_ext_len + 1) * 8


class HopByHopOptionHeader(ConvertibleToBytes, BuildableFromBytes):
    """ Class representing HopByHop option header. """

    _header_length = 2

    def __init__(self, _type, length=None):
        self.type = _type
        self.length = length if length is not None else 0

    def to_bytes(self):
        return bytearray([self.type, self.length])

    @classmethod
    def from_bytes(cls, data):
        _type = ord(data.read(1))
        length = ord(data.read(1))
        return cls(_type, length)

    def __len__(self):
        return self._header_length

    def __repr__(self):
        return "HopByHopOptionHeader(type={}, length={})".format(self.type, self.length)


class HopByHopOption(ConvertibleToBytes):
    """ Class representing HopByHop option.

    Class consists of two elements: HopByHopOptionHeader and value (e.g. for MPLOption).

    The following example shows how any HopByHop option can be constructed.

    Example:
        HopByHop(next_header=0x3a,
                 options=[HopByHopOption(HopByHopOptionHeader(_type=0x6d),
                                         MPLOption(S=1, M=0, V=0, sequence=2, seed_id=bytes([0x00, 0x18])))

    """

    def __init__(self, header, value):
        self.value = value

        self.header = header
        self.header.length = len(self.value)

    def to_bytes(self):
        return self.header.to_bytes() + self.value.to_bytes()

    def __len__(self):
        return len(self.header) + len(self.value)

    def __repr__(self):
        return "HopByHopOption(header={}, value={})".format(self.header, self.value)


class MPLOption(ConvertibleToBytes):
    """ Class representing MPL option. """

    _header_length = 2

    _seed_id_length = {0: 0, 1: 2, 2: 8, 3: 16}

    def __init__(self, S, M, V, sequence, seed_id):
        self.S = S
        self.M = M
        self.V = V
        self.sequence = sequence
        self.seed_id = seed_id

    def to_bytes(self):
        smv = (((self.S & 0x03) << 6) | ((self.M & 0x01) << 5) | ((self.V & 0x01) << 4))

        return bytearray([smv, self.sequence]) + self.seed_id

    @classmethod
    def from_bytes(cls, data):
        b = ord(data.read(1))

        s = (b >> 6) & 0x03
        m = (b >> 5) & 0x01
        v = (b >> 4) & 0x01

        sequence = ord(data.read(1))
        seed_id = data.read(cls._seed_id_length[s])

        return cls(s, m, v, sequence, seed_id)

    def __len__(self):
        return self._header_length + self._seed_id_length[self.S]

    def __repr__(self):
        return "MPLOption(S={}, M={}, V={}, sequence={}, seed_id={})".format(self.S, self.M, self.V, self.sequence,
                                                                             hexlify(self.seed_id))


class IPv6PacketFactory(PacketFactory):
    """ Factory that produces IPv6 packets from data.

    This factory must be initialized with factories which allow to parse extension headers and upper layer protocols.

    The following example shows preferable setup of IPv6PacketFactory.

    Header types:
        0: HopByHop
        17: UDP
        58: ICMPv6

    Option types:
        109: MPL

    ICMPv6 body types:
        128: Echo request
        129: Echo response

    Example usage:

        ipv6_factory = IPv6PacketFactory(
            ehf={
                0: HopByHopFactory(options_factories={
                    109: MPLOptionFactory()
                })
            },
            ulpf={
                17: UDPDatagramFactory(dst_port_factories={
                    19788: MLEMessageFactory(),
                    19789: CoAPMessageFactory()
                }),
                58: ICMPv6Factory(body_factories={
                    128: ICMPv6EchoBodyFactory(),
                    129: ICMPv6EchoBodyFactory()
                })
            }
        )

    """

    def __init__(self, ehf=None, ulpf=None):
        """
        ehf - Extension Header Factory
        ulpf - Upper Layer Protocol Factory

        Args:
            ehf(dict[int: PacketFactory]): Dictionary mapping extension header types on specialized factories.
            ulpf(dict[int: PacketFactory]): Dictionary mapping upper layer protocol types on specialized factories.
        """
        self._ehf = ehf if ehf is not None else {}
        self._ulpf = ulpf if ulpf is not None else {}

    def _is_extension_header(self, header_type):
        return header_type not in UPPER_LAYER_PROTOCOLS

    def _get_extension_header_factory_for(self, next_header):
        try:
            return self._ehf[next_header]
        except KeyError:
            raise RuntimeError("Could not get Extension Header factory for next_header={}.".format(next_header))

    def _get_upper_layer_protocol_factory_for(self, next_header):
        try:
            return self._ulpf[next_header]
        except KeyError:
            raise RuntimeError("Could not get Upper Layer Protocol factory for next_header={}.".format(next_header))

    def _parse_extension_headers(self, data, next_header, message_info):
        extension_headers = []

        while self._is_extension_header(next_header):
            factory = self._get_extension_header_factory_for(next_header)

            extension_header = factory.parse(data, message_info)

            next_header = extension_header.next_header

            extension_headers.append(extension_header)

        return next_header, extension_headers

    def _parse_upper_layer_protocol(self, data, next_header, message_info):
        factory = self._get_upper_layer_protocol_factory_for(next_header)

        return factory.parse(data, message_info)

    def parse(self, data, message_info):
        ipv6_header = IPv6Header.from_bytes(data)

        message_info.source_ipv6 = ipv6_header.source_address
        message_info.destination_ipv6 = ipv6_header.destination_address

        next_header, extension_headers = self._parse_extension_headers(data, ipv6_header.next_header, message_info)

        upper_layer_protocol = self._parse_upper_layer_protocol(data, next_header, message_info)

        return IPv6Packet(ipv6_header, upper_layer_protocol, extension_headers)


class HopByHopOptionsFactory(object):
    """ Factory that produces HopByHop options. """

    _one_byte_padding = 0x00
    _many_bytes_padding = 0x01

    def __init__(self, options_factories=None):
        self._options_factories = (options_factories if options_factories is not None else {})

    def _get_HopByHopOption_value_factory(self, _type):
        try:
            return self._options_factories[_type]
        except KeyError:
            raise RuntimeError("Could not find HopByHopOption value factory for type={}.".format(_type))

    def parse(self, data, message_info):
        options = []

        while data.tell() < len(data.getvalue()):
            option_header = HopByHopOptionHeader.from_bytes(data)

            if option_header.type == self._one_byte_padding:
                # skip one byte padding
                data.read(1)

            elif option_header.type == self._many_bytes_padding:
                # skip n bytes padding
                data.read(option_header.length)

            else:
                factory = self._get_HopByHopOption_value_factory(option_header.type)

                option_data = data.read(option_header.length)

                option = HopByHopOption(
                    option_header,
                    factory.parse(io.BytesIO(option_data), message_info),
                )

                options.append(option)

        return options


class HopByHopFactory(PacketFactory):
    """ Factory that produces HopByHop extension headers from data. """

    def __init__(self, hop_by_hop_options_factory):
        self._hop_by_hop_options_factory = hop_by_hop_options_factory

    def _calculate_extension_header_length(self, hdr_ext_len):
        return (hdr_ext_len + 1) * 8

    def parse(self, data, message_info):
        next_header = ord(data.read(1))

        hdr_ext_len = ord(data.read(1))

        # Note! Two bytes were read (next_header and hdr_ext_len) so they must
        # be substracted from header length
        hop_by_hop_length = (self._calculate_extension_header_length(hdr_ext_len) - 2)

        hop_by_hop_data = data.read(hop_by_hop_length)

        options = self._hop_by_hop_options_factory.parse(io.BytesIO(hop_by_hop_data), message_info)

        hop_by_hop = HopByHop(next_header, options, hdr_ext_len)

        message_info.payload_length += len(hop_by_hop)

        return hop_by_hop


class MPLOptionFactory(PacketFactory):
    """ Factory that produces MPL options for HopByHop extension header. """

    def parse(self, data, message_info):
        return MPLOption.from_bytes(data)


class UDPHeaderFactory:
    """ Factory that produces UDP header. """

    def parse(self, data, message_info):
        return UDPHeader.from_bytes(data)


class UdpBasedOnSrcDstPortsPayloadFactory:

    # TODO: Unittests
    """ Factory that produces UDP payload. """

    def __init__(self, src_dst_port_based_payload_factories):
        """
        Args:
            src_dst_port_based_payload_factories (PacketFactory):
                Factories parse UDP payload based on source or
                destination port.
        """
        self._factories = src_dst_port_based_payload_factories

    def parse(self, data, message_info):
        factory = None

        if message_info.dst_port in self._factories:
            factory = self._factories[message_info.dst_port]

        if message_info.src_port in self._factories:
            factory = self._factories[message_info.src_port]

        if message_info.dst_port == common.UDP_TEST_PORT:
            # Ignore traffic targeted to test port
            return None
        elif factory is None:
            raise RuntimeError("Could not find factory to build UDP payload.")

        return factory.parse(data, message_info)


class UDPDatagramFactory(PacketFactory):

    # TODO: Unittests
    """ Factory that produces UDP datagrams. """

    def __init__(self, udp_header_factory, udp_payload_factory):
        self._udp_header_factory = udp_header_factory
        self._udp_payload_factory = udp_payload_factory

    def parse(self, data, message_info):
        header = self._udp_header_factory.parse(data, message_info)

        # Update message payload length: UDP header (8B) + payload length
        message_info.payload_length += len(header) + (len(data.getvalue()) - data.tell())

        message_info.src_port = header.src_port
        message_info.dst_port = header.dst_port

        payload = self._udp_payload_factory.parse(data, message_info)

        return UDPDatagram(header, payload)


class ICMPv6Factory(PacketFactory):
    """ Factory that produces ICMPv6 messages from data. """

    def __init__(self, body_factories=None):
        self._body_factories = (body_factories if body_factories is not None else {})

    def _get_icmpv6_body_factory(self, _type):
        try:
            return self._body_factories[_type]

        except KeyError:
            if "default" not in self._body_factories:
                raise RuntimeError("Could not find specialized factory to parse ICMP body. "
                                   "Unsupported ICMP type: {}".format(_type))

            default_factory = self._body_factories["default"]

            print("Could not find specialized factory to parse ICMP body. "
                  "Take the default one: {}".format(type(default_factory)))

            return default_factory

    def parse(self, data, message_info):
        header = ICMPv6Header.from_bytes(data)

        factory = self._get_icmpv6_body_factory(header.type)

        message_info.payload_length += len(header) + (len(data.getvalue()) - data.tell())

        return ICMPv6(header, factory.parse(data, message_info))


class ICMPv6EchoBodyFactory(PacketFactory):
    """ Factory that produces ICMPv6 echo message body. """

    def parse(self, data, message_info):
        return ICMPv6EchoBody.from_bytes(data)


class BytesPayload(ConvertibleToBytes, BuildableFromBytes):
    """ Class representing bytes payload. """

    def __init__(self, data):
        self.data = data

    def to_bytes(self):
        return bytearray(self.data)

    @classmethod
    def from_bytes(cls, data):
        return cls(data)

    def __len__(self):
        return len(self.data)


class BytesPayloadFactory(PacketFactory):
    """ Factory that produces bytes payload. """

    def parse(self, data, message_info):
        return BytesPayload(data.read())


class ICMPv6EchoBody(ConvertibleToBytes, BuildableFromBytes):
    """ Class representing body of ICMPv6 echo messages. """

    _header_length = 4

    def __init__(self, identifier, sequence_number, data):
        self.identifier = identifier
        self.sequence_number = sequence_number
        self.data = data

    def to_bytes(self):
        data = struct.pack(">H", self.identifier)
        data += struct.pack(">H", self.sequence_number)
        data += self.data
        return data

    @classmethod
    def from_bytes(cls, data):
        identifier = struct.unpack(">H", data.read(2))[0]
        sequence_number = struct.unpack(">H", data.read(2))[0]

        return cls(identifier, sequence_number, data.read())

    def __len__(self):
        return self._header_length + len(self.data)


class ICMPv6DestinationUnreachableFactory(PacketFactory):
    """ Factory that produces ICMPv6 echo message body. """

    def parse(self, data, message_info):
        return ICMPv6DestinationUnreachable.from_bytes(data)


class ICMPv6DestinationUnreachable(ConvertibleToBytes, BuildableFromBytes):
    """ Class representing body of ICMPv6 Destination Unreachable messages. """

    _header_length = 4
    _unused = 0

    def __init__(self, data):
        self.data = data

    def to_bytes(self):
        data = bytearray(struct.pack(">I", self._unused))
        data += self.data
        return data

    @classmethod
    def from_bytes(cls, data):
        unused = struct.unpack(">I", data.read(4))[0]
        if unused != 0:
            raise RuntimeError(
                "Invalid value of unused field in the ICMPv6 Destination Unreachable data. Expected value: 0.")

        return cls(bytearray(data.read()))

    def __len__(self):
        return self._header_length + len(self.data)
